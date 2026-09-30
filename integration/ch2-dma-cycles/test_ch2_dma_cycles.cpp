// test_ch2_dma_cycles.cpp -- ch2-dma-cycles. See SPECIFICATION.md for every
// assertion's proposition, program, observed value and expected value. This
// file only implements what SPECIFICATION.md already committed to.
//
// Every configuration's Copper list and every sprite structure its own
// Copper list points at is built by the program itself (S1, ch2build_*.i),
// once its own S0 step releases the CIA-A boot overlay -- see the .s files.
// This file only points the harness at each program's binary and its own
// Config, and reads back what the ports and peek_logical show.
//
// NMH_MUTATE selects a mutated capture (a different program, or the same
// program analyzed against a different block declaration) in place of a
// proposition's normal one, one assertion at a time; none is exercised by
// a plain `make test`.

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

// ---- harness-side constants (nanomig.i) --------------------------------
constexpr uint32_t kResults   = 0x0C0000;
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudget  = 6'500'000;

// S (SPECIFICATION.md 2.1): the number of trace time units in one memory
// cycle. Measured once per process, from a run's own refresh trace -- see
// measure_S() -- never a constant read out of an RTL source file.
uint64_t gS = 0;

// ---- ch2dma.i, restated: must match exactly ----------------------------
constexpr uint32_t FRAME_CTR    = kResults + 0;
constexpr uint32_t RES_DSKBYTR  = kResults + 2;

constexpr uint32_t COP_MAIN     = 0x001000;
constexpr uint32_t COP_SECOND   = 0x002000;
constexpr uint32_t CH_AUD[4]    = {0x003000, 0x003200, 0x003400, 0x003600};
constexpr uint32_t CH_DISK      = 0x004000;
constexpr uint32_t CH_SPR[8]    = {0x008000, 0x008400, 0x008800, 0x008C00,
                                    0x009000, 0x009400, 0x009800, 0x009C00};
constexpr uint32_t CH_SPR_NULL  = 0x00A000;
constexpr uint32_t CH_U_SUBLIST = 0x00B000;
constexpr uint32_t CH_BPL[6]    = {0x010000, 0x018000, 0x020000,
                                    0x028000, 0x030000, 0x038000};

// Every program's own code/vectors/stack/results (attribution table,
// SPECIFICATION.md 2.2): the identity range the harness maps 1:1
// ($080000-$1FFFFF, nmh::kChipIdentityLo/Hi). Every declared DMA block in
// this package is placed below $080000 but at or above $008000 (ch2dma.i),
// so this threshold separates "CPU block" from every DMA block without
// listing sub-ranges -- except the level-3 (VERTB) autovector word at
// byte $6C, which every program's own S0 step makes an ordinary,
// CPU-reachable part of chip RAM (CLEAR_OVL): S0 writes it once (its own
// INSTALL_VECT) and the CPU itself reads it on every VERTB exception, a
// CPU bus access below kCpuWordThreshold that the declared DMA blocks
// (all at $008000 and up) never reach. Refresh (D7) reads word 0 of the
// vector table (measured, not a page fact), so only the level-3 vector's
// own two words are carved out here, not the whole table.
constexpr uint32_t kCpuWordThreshold = 0x080000 / 2;
constexpr uint32_t kLevel3VectorLoWord = 0x6C / 2;
constexpr uint32_t kLevel3VectorHiWord = kLevel3VectorLoWord + 2; // one long word

// ---- custom register offsets (ch2dma.i, restated for the C++ side) -----
constexpr uint16_t DMACON = 0x096, INTENA = 0x09A, INTREQ = 0x09C, ADKCON = 0x09E;
constexpr uint16_t DSKPTH = 0x020, DSKPTL = 0x022, DSKLEN = 0x024, DSKBYTR = 0x01A;
constexpr uint16_t COP1LCH = 0x080, COP1LCL = 0x082, COP2LCH = 0x084, COP2LCL = 0x086;
constexpr uint16_t COPJMP1 = 0x088, COPJMP2 = 0x08A;
constexpr uint16_t DIWSTRT = 0x08E, DIWSTOP = 0x090, DDFSTRT = 0x092, DDFSTOP = 0x094;
constexpr uint16_t AUD0LCH = 0x0A0;
constexpr uint16_t BPL1PTH = 0x0E0, BPLCON0 = 0x100, BPLCON1 = 0x102, BPL1MOD = 0x108, BPL2MOD = 0x10A;
constexpr uint16_t SPR0PTH = 0x120;
constexpr uint16_t COLOR00 = 0x180;
constexpr uint16_t COLOR17 = 0x1A2, COLOR18 = 0x1A4, COLOR19 = 0x1A6;
constexpr uint16_t COLOR21 = 0x1AA, COLOR22 = 0x1AC, COLOR23 = 0x1AE;

constexpr uint16_t DMAEN_BIT = 0x0200, BPLEN_BIT = 0x0100, COPEN_BIT = 0x0080;
constexpr uint16_t SPREN_BIT = 0x0020, DSKEN_BIT = 0x0010, AUD0123EN_BIT = 0x000F;

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

std::string hex(uint32_t v) { char b[16]; std::snprintf(b, sizeof b, "%04X", v); return b; }

uint32_t wordof(uint32_t byte_addr) { return byte_addr / 2; }

// ---- attribution & reference-frame machinery (SPECIFICATION.md 2.2-2.4) -

struct Block { std::string name; uint32_t lo, hi; }; // word-address half-open range

std::string attribute(uint32_t word_addr, const std::vector<Block>& blocks) {
    if (word_addr >= kCpuWordThreshold) return "CPU";
    if (word_addr >= kLevel3VectorLoWord && word_addr < kLevel3VectorHiWord) return "CPU";
    for (const auto& b : blocks) if (word_addr >= b.lo && word_addr < b.hi) return b.name;
    return "R";
}

struct LineRef { std::vector<uint64_t> firsts; }; // A_n per line, sorted

// Section 2.3 construction: sort refresh set R by time; start a new group
// at a gap > 16*S; A_n = first access time of group n.
LineRef make_lines(const std::vector<nmh::RamAccess>& log, const std::vector<Block>& blocks) {
    std::vector<uint64_t> r;
    for (const auto& a : log) if (!a.is_write && attribute(a.word_addr, blocks) == "R") r.push_back(a.cycle);
    std::sort(r.begin(), r.end());
    LineRef ref;
    if (r.empty()) return ref;
    ref.firsts.push_back(r[0]);
    for (size_t i = 1; i < r.size(); ++i)
        if (r[i] - r[i - 1] > 16 * gS) ref.firsts.push_back(r[i]);
    return ref;
}

// The line index a cycle belongs to (line n = [firsts[n], firsts[n+1))), or
// -1 if it precedes the first line found.
int64_t line_of(const LineRef& ref, uint64_t cycle) {
    if (ref.firsts.empty() || cycle < ref.firsts.front()) return -1;
    size_t i = std::upper_bound(ref.firsts.begin(), ref.firsts.end(), cycle) - ref.firsts.begin();
    return (int64_t)i - 1;
}

int64_t in_line_cycle(const LineRef& ref, int64_t line_idx, uint64_t cycle) {
    return (int64_t)((cycle - ref.firsts[(size_t)line_idx]) / gS);
}

// Section 2.4: a restart fetch is the Copper's read of the word at any of
// `list_starts` (word addresses) -- identified, among reads of that
// address, by the odd in-line cycle the page gives the Copper (p.20): a
// same-address read by another channel (X-aud/X-dsk/X-bpl's own
// contrasts, which deliberately alias a DMA block onto the Copper's own
// list) falls on an even cycle and is excluded. Before any line reference
// is available (line_of < 0) a candidate is kept, not dropped, so the
// very first restart -- which can precede the first refresh access the
// line grid is built from -- is never silently lost. Frame k = [restart
// [k-1], restart[k]) (0-indexed restart[0] is restart #1).
std::vector<uint64_t> restart_fetches(const std::vector<nmh::RamAccess>& log, const std::vector<uint32_t>& list_starts,
                                       const LineRef& lref) {
    std::vector<uint64_t> out;
    for (const auto& a : log) {
        if (a.is_write) continue;
        bool match = false;
        for (auto s : list_starts) if (a.word_addr == s) { match = true; break; }
        if (!match) continue;
        int64_t li = line_of(lref, a.cycle);
        if (li >= 0 && in_line_cycle(lref, li, a.cycle) % 2 == 0) continue; // even-s: not the Copper
        out.push_back(a.cycle);
    }
    std::sort(out.begin(), out.end());
    return out;
}

struct ObservedSpan { bool ok; uint64_t lo, hi; };
ObservedSpan observed_span(const std::vector<uint64_t>& restarts) {
    if (restarts.size() < 6) return {false, 0, 0};
    return {true, restarts[2], restarts[5]};
}

// Measures S (SPECIFICATION.md 2.1): the measured line period divided by
// 227 (p.23's shorter of PAL's fixed length and NTSC's alternating
// lengths). Two steps, neither assuming S:
//  1. The smallest positive gap between two refresh-access times is 2S
//     (p.194's four refresh accesses per line, evenly spaced); every other
//     gap -- between lines, or a rare frame-boundary artifact where the
//     Copper's own restart fetch perturbs a neighbouring refresh access --
//     is far larger. This gives S's own grid unit without circularity.
//  2. Refresh accesses are grouped at a gap > 8x that unit (the same
//     16S margin make_lines() uses, restated here since gS is not set
//     yet), and the measured line period is the most common interval
//     between consecutive groups' first access -- robust against the rare
//     artifact above, which never outnumbers the ~939 clean lines a run
//     observes.
uint64_t measure_S(const std::vector<nmh::RamAccess>& log, const std::vector<Block>& blocks) {
    std::vector<uint64_t> rc;
    for (const auto& a : log) if (!a.is_write && attribute(a.word_addr, blocks) == "R") rc.push_back(a.cycle);
    std::sort(rc.begin(), rc.end());
    if (rc.size() < 2) return 0;
    uint64_t small = UINT64_MAX;
    for (size_t i = 1; i < rc.size(); ++i) {
        uint64_t d = rc[i] - rc[i - 1];
        if (d > 0 && d < small) small = d;
    }
    if (small == UINT64_MAX) return 0;
    std::vector<uint64_t> starts;
    starts.push_back(rc[0]);
    for (size_t i = 1; i < rc.size(); ++i)
        if (rc[i] - rc[i - 1] > 8 * small) starts.push_back(rc[i]);
    std::map<uint64_t, int> hist;
    for (size_t i = 1; i < starts.size(); ++i) ++hist[starts[i] - starts[i - 1]];
    uint64_t best = 0; int best_count = 0;
    for (auto& kv : hist) if (kv.second > best_count) { best_count = kv.second; best = kv.first; }
    if (best == 0) return 0;
    return best / 227;
}

// Sets gS from measure_S(), with a defensive fallback: measure_S returns 0
// only when a run's own refresh trace is too degenerate to derive a slot
// period from (fewer than two refresh accesses). Dividing or taking a
// modulus by gS elsewhere would then be undefined behaviour, crashing the
// whole run and losing every result already printed -- so this reports the
// failure as its own line and substitutes 1, which lets the run finish and
// every other check state its own, honest verdict instead of none at all.
void set_gS(const std::vector<nmh::RamAccess>& log, const std::vector<Block>& blocks, Checks& checks) {
    gS = measure_S(log, blocks);
    if (gS == 0) {
        checks.note("S measurement found fewer than two refresh accesses in this run; S could not be derived. "
                     "Substituting S=1 so the run finishes -- every check below is against that substitute, not "
                     "a measured value, and should be read accordingly.");
        gS = 1;
    }
}

// F2's own self-test (as F1's, SPECIFICATION.md 2.3): no stimulus falls a
// refresh group's own count below two (the F23 mutation's own record: C0-m
// splits a line without doing that). Operates directly on a sorted cycle
// list, independent of attribution/restart machinery.
bool check_f2_synthetic(std::vector<uint64_t> cycles, uint64_t S) {
    std::sort(cycles.begin(), cycles.end());
    if (cycles.empty()) return false;
    size_t cnt = 1; bool ok = true;
    for (size_t i = 1; i <= cycles.size(); ++i) {
        if (i == cycles.size() || cycles[i] - cycles[i - 1] > 16 * S) {
            if (cnt < 2) ok = false;
            cnt = 1;
        } else ++cnt;
    }
    return ok;
}

// ---- generic per-population parity check (P1-P4, P6) -------------------

struct ParityResult {
    bool f1_ok = false, f2_ok = false, f3_ok = false, a1_ok = false, restarts_ok = false;
    bool all_even = true, all_odd = true;
    size_t population = 0;
    size_t a1_violations = 0;
};

// F1: S integer (given, gS), single phase across the run's own DMA
// accesses -- every population this specification declares a block for,
// excluding the CPU block (D5 keeps the CPU off the bus wherever a check
// could depend on it, but the level-3 handler still runs a handful of
// instructions right at each frame's own restart, which can fall inside
// the observed span; no page gives the CPU's own accesses a phase at
// these ports, so its phase is measured and reported on its own line, not
// folded into F1 -- see the per-run CPU-phase note in main()).
bool check_f1(const std::vector<nmh::RamAccess>& log, const std::vector<Block>& blocks,
               uint64_t obs_lo, uint64_t obs_hi) {
    std::optional<uint64_t> phi;
    for (const auto& a : log) {
        if (a.cycle < obs_lo || a.cycle >= obs_hi) continue;
        if (attribute(a.word_addr, blocks) == "CPU") continue;
        uint64_t p = a.cycle % gS;
        if (!phi) phi = p;
        else if (*phi != p) return false;
    }
    return true;
}

// Evaluates every access of `target_block` within the observed frames:
// F1/F2/F3 (on the refresh-derived line grid), A1 (only meaningful for
// the baseline run, but harmless elsewhere), and the even/odd population.
ParityResult analyze_population(const std::vector<nmh::RamAccess>& log, const std::vector<Block>& blocks,
                                 const std::vector<uint32_t>& list_starts, const std::string& target_block,
                                 bool check_a1 = false) {
    ParityResult r;
    LineRef lref = make_lines(log, blocks);
    auto restarts = restart_fetches(log, list_starts, lref);
    auto span = observed_span(restarts);
    r.restarts_ok = span.ok;
    if (!span.ok) return r;

    r.f1_ok = check_f1(log, blocks, span.lo, span.hi);

    std::vector<uint64_t> r_cycles;
    for (const auto& a : log) if (!a.is_write && attribute(a.word_addr, blocks) == "R") r_cycles.push_back(a.cycle);
    std::sort(r_cycles.begin(), r_cycles.end());
    std::vector<std::pair<uint64_t, size_t>> groups; // (A_n, count)
    if (!r_cycles.empty()) {
        uint64_t start = r_cycles[0]; size_t cnt = 1;
        for (size_t i = 1; i < r_cycles.size(); ++i) {
            if (r_cycles[i] - r_cycles[i - 1] > 16 * gS) { groups.push_back({start, cnt}); start = r_cycles[i]; cnt = 0; }
            ++cnt;
        }
        groups.push_back({start, cnt});
    }
    bool f2 = true, f3 = true; size_t f2n = 0;
    for (size_t i = 0; i < groups.size(); ++i) {
        if (groups[i].first < span.lo || groups[i].first >= span.hi) continue;
        ++f2n;
        if (groups[i].second < 2) f2 = false;
        if (i + 1 < groups.size()) {
            uint64_t gap = groups[i + 1].first - groups[i].first;
            if (gap != 227 * gS && gap != 228 * gS) f3 = false;
        }
    }
    r.f2_ok = f2 && f2n > 0;
    r.f3_ok = f3;

    if (check_a1) {
        for (const auto& a : log) {
            if (a.cycle < span.lo || a.cycle >= span.hi) continue;
            std::string cls = attribute(a.word_addr, blocks);
            if (cls != "R" && cls != "CPU" && cls != "Copper") ++r.a1_violations;
        }
        r.a1_ok = r.a1_violations == 0;
    } else r.a1_ok = true;

    for (const auto& a : log) {
        if (a.cycle < span.lo || a.cycle >= span.hi) continue;
        if (attribute(a.word_addr, blocks) != target_block) continue;
        int64_t li = line_of(lref, a.cycle);
        if (li < 0 || (size_t)li >= lref.firsts.size()) continue;
        int64_t s = in_line_cycle(lref, li, a.cycle);
        ++r.population;
        if (s % 2 == 0) r.all_odd = false; else r.all_even = false;
    }
    return r;
}

// ---- run harness ---------------------------------------------------------

struct RunResult {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
    std::vector<std::optional<uint16_t>> peeks;
};

using Poker = std::function<void(nmh::NanomigHarness&)>;

RunResult run_config(const std::string& bin, nmh::Config cfg, const Poker& poke,
                      const std::vector<uint32_t>& peek_addrs = {}) {
    RunResult r;
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    poke(h);
    r.released = h.boot(kBootBudget).cpu_released;
    r.done = h.run_until_write(kDoneAddr, kDoneValue, kRunBudget);
    r.log = h.ram_log();
    for (auto a : peek_addrs) r.peeks.push_back(h.peek_logical(a));
    return r;
}

void nopoke(nmh::NanomigHarness&) {}

// PAL/ECS (mode 0, the default), NTSC/ECS (1), PAL/OCS (2), NTSC/OCS (3):
// SPECIFICATION.md 2.10. chipset_config bit 3 is ECS, bit 1 is NTSC (both
// nanomig.v ports; no new program is needed to select either).
nmh::Config mode_config(int mode) {
    nmh::Config c;
    switch (mode) {
        case 1: c.chipset_config = 0x0A; break; // NTSC/ECS
        case 2: c.chipset_config = 0x00; break; // PAL/OCS
        case 3: c.chipset_config = 0x02; break; // NTSC/OCS
        default: c.chipset_config = 0x08; break; // PAL/ECS
    }
    return c;
}

constexpr uint32_t COP_BLOCK_END = 0x003000; // end of COP_SECOND's reserve
Block copper_block() { return {"Copper", wordof(COP_MAIN), wordof(COP_BLOCK_END)}; }
Block audio_block(int x) { return {"Audio", wordof(CH_AUD[x]), wordof(CH_AUD[x] + 0x200)}; }
Block disk_block(uint32_t lo, uint32_t words) { return {"Disk", wordof(lo), wordof(lo) + words}; }
Block sprite_block(int x) { return {"Sprite", wordof(CH_SPR[x]), wordof(CH_SPR[x] + 0x400)}; }
Block sprite_null_block() { return {"Sprite", wordof(CH_SPR_NULL), wordof(0x00B000)}; }
Block bitplane_block(int y) { return {"Bitplane", wordof(CH_BPL[y]), wordof(CH_BPL[y] + 0x8000)}; }

std::vector<Block> blocks_C0() { return {copper_block()}; }

std::vector<Block> blocks_audio() {
    return {copper_block(), audio_block(0), audio_block(1), audio_block(2), audio_block(3)};
}
std::vector<Block> blocks_disk(uint32_t lo = CH_DISK, uint32_t words = 8000) {
    return {copper_block(), disk_block(lo, words)};
}
std::vector<Block> blocks_sprite() {
    std::vector<Block> b = {copper_block(), sprite_null_block()};
    for (int x = 0; x < 8; ++x) b.push_back(sprite_block(x));
    return b;
}
std::vector<Block> blocks_bitplane(int n) {
    std::vector<Block> b = {copper_block()};
    for (int y = 0; y < n; ++y) b.push_back(bitplane_block(y));
    return b;
}
std::vector<Block> blocks_full(int n_bpl) { // C6/C5/S1/S2-style: everything at once
    std::vector<Block> b = {copper_block(), audio_block(0), audio_block(1), audio_block(2), audio_block(3),
                             disk_block(CH_DISK, 8000), sprite_null_block()};
    for (int x = 0; x < 8; ++x) b.push_back(sprite_block(x));
    for (int y = 0; y < n_bpl; ++y) b.push_back(bitplane_block(y));
    return b;
}
std::vector<Block> blocks_S1() { return {copper_block(), sprite_block(0), sprite_null_block(), bitplane_block(0)}; }
std::vector<Block> blocks_S2() {
    return {copper_block(), sprite_block(0), sprite_block(2), sprite_null_block(), bitplane_block(0)};
}

// ---- configuration runners (SPECIFICATION.md 2.9) -----------------------
// Every program builds its own Copper list(s) and sprite structures (S1);
// this side only loads the binary, runs it, and reads back the port log
// and any peeked results.

RunResult run_C0(const std::string& dir, nmh::Config cfg = nmh::Config{}) {
    return run_config(dir + "/prog_c0.bin", cfg, nopoke, {FRAME_CTR});
}
RunResult run_C0u(const std::string& dir) { return run_config(dir + "/prog_c0_u.bin", nmh::Config{}, nopoke); }
RunResult run_C0m(const std::string& dir) { return run_config(dir + "/prog_c0_m.bin", nmh::Config{}, nopoke); }
RunResult run_C0_nofinish(const std::string& dir) { return run_config(dir + "/prog_c0_nofinish.bin", nmh::Config{}, nopoke); }

RunResult run_C1a(const std::string& dir, bool no_aud0 = false) {
    return run_config(dir + (no_aud0 ? "/prog_c1a_noaud0.bin" : "/prog_c1a.bin"), nmh::Config{}, nopoke);
}
RunResult run_C1b(const std::string& dir) { return run_config(dir + "/prog_c1b.bin", nmh::Config{}, nopoke); }
RunResult run_XAud(const std::string& dir) { return run_config(dir + "/prog_c1a_xaud.bin", nmh::Config{}, nopoke); }

RunResult run_C2a(const std::string& dir) {
    return run_config(dir + "/prog_c2a.bin", nmh::Config{}, nopoke, {RES_DSKBYTR});
}
RunResult run_C2b(const std::string& dir) {
    return run_config(dir + "/prog_c2b.bin", nmh::Config{}, nopoke, {RES_DSKBYTR});
}
RunResult run_C2a_noenable(const std::string& dir) {
    return run_config(dir + "/prog_c2a_noenable.bin", nmh::Config{}, nopoke, {RES_DSKBYTR});
}
RunResult run_XDsk(const std::string& dir) {
    return run_config(dir + "/prog_c2a_xdsk.bin", nmh::Config{}, nopoke, {RES_DSKBYTR});
}

RunResult run_C3(const std::string& dir, bool no_spren = false) {
    return run_config(dir + (no_spren ? "/prog_c3_nospren.bin" : "/prog_c3.bin"), nmh::Config{}, nopoke);
}
RunResult run_XSpr(const std::string& dir) { return run_config(dir + "/prog_c3_xspr.bin", nmh::Config{}, nopoke); }

RunResult run_C4(const std::string& dir, int n, bool wide_start = false, bool no_bplen = false) {
    std::string bin = "/prog_c4_" + std::to_string(n) + ".bin";
    if (n == 4 && wide_start) bin = "/prog_c4_4w.bin";
    if (n == 4 && no_bplen) bin = "/prog_c4_4_nobplen.bin";
    return run_config(dir + bin, nmh::Config{}, nopoke);
}
RunResult run_XBpl(const std::string& dir) { return run_config(dir + "/prog_c4_4_xbpl.bin", nmh::Config{}, nopoke); }

RunResult run_C5(const std::string& dir) { return run_config(dir + "/prog_c5.bin", nmh::Config{}, nopoke, {RES_DSKBYTR}); }
RunResult run_C6(const std::string& dir, const char* bin = "/prog_c6.bin", nmh::Config cfg = nmh::Config{}) {
    return run_config(dir + bin, cfg, nopoke, {RES_DSKBYTR});
}
RunResult run_C6prime(const std::string& dir) { return run_config(dir + "/prog_c6_prime.bin", nmh::Config{}, nopoke); }
RunResult run_C6_extra50(const std::string& dir) { return run_config(dir + "/prog_c6_extra50.bin", nmh::Config{}, nopoke); }
RunResult run_C6_truncated(const std::string& dir) { return run_config(dir + "/prog_c6_trunc.bin", nmh::Config{}, nopoke); }

RunResult run_C7a(const std::string& dir) { return run_config(dir + "/prog_c7a.bin", nmh::Config{}, nopoke); }
RunResult run_C7b(const std::string& dir) { return run_config(dir + "/prog_c7b.bin", nmh::Config{}, nopoke); }

RunResult run_S1(const std::string& dir, nmh::Config cfg = nmh::Config{}) {
    return run_config(dir + "/prog_s1.bin", cfg, nopoke);
}
RunResult run_S2(const std::string& dir) { return run_config(dir + "/prog_s2.bin", nmh::Config{}, nopoke); }

// ---- named per-plane blocks (P5) and word-completeness (K4/K6) ----------

Block bitplane_block_named(int y) { return {"BPL" + std::to_string(y + 1), wordof(CH_BPL[y]), wordof(CH_BPL[y] + 0x8000)}; }
std::vector<Block> blocks_bitplane_named(int n) {
    std::vector<Block> b = {copper_block()};
    for (int y = 0; y < n; ++y) b.push_back(bitplane_block_named(y));
    return b;
}

// P5(a): every access in plane blocks 1-4 (only, regardless of how many
// planes n the run has) has even s.
bool p5a_check(const RunResult& run, int n, Checks& checks, const std::string& cfgname) {
    auto blocks = blocks_bitplane_named(n);
    bool ok = true; std::ostringstream note;
    int upto = std::min(n, 4);
    for (int y = 0; y < upto; ++y) {
        std::string name = "BPL" + std::to_string(y + 1);
        auto pr = analyze_population(run.log, blocks, {wordof(COP_MAIN)}, name, false);
        bool this_ok = pr.restarts_ok && pr.f1_ok && pr.f2_ok && pr.f3_ok && pr.population > 0 && pr.all_even;
        ok = ok && this_ok;
        note << name << ":pop=" << pr.population << ",even=" << pr.all_even << (this_ok ? "" : "(FAIL)") << ";";
    }
    checks.note("P5a " + cfgname + ": " + note.str());
    return ok;
}

// P5(b): in every line with any bitplane fetch (planes 1..n), even-cycle
// fetches outnumber odd-cycle ones ("most").
bool p5b_check(const RunResult& run, int n, Checks& checks, const std::string& cfgname) {
    auto blocks = blocks_bitplane_named(n);
    LineRef lref = make_lines(run.log, blocks);
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)}, lref);
    auto span = observed_span(restarts);
    if (!span.ok) { checks.note("P5b " + cfgname + ": NOSPAN"); return false; }
    std::map<int64_t, std::pair<int, int>> counts; // line -> (even,odd)
    for (const auto& a : run.log) {
        if (a.is_write || a.cycle < span.lo || a.cycle >= span.hi) continue;
        std::string cls = attribute(a.word_addr, blocks);
        if (cls.rfind("BPL", 0) != 0) continue;
        int64_t li = line_of(lref, a.cycle);
        if (li < 0 || (size_t)li >= lref.firsts.size()) continue;
        int64_t s = in_line_cycle(lref, li, a.cycle);
        auto& pr = counts[li];
        if (s % 2 == 0) ++pr.first; else ++pr.second;
    }
    int bad = 0;
    for (const auto& kv : counts) if (!(kv.second.first > kv.second.second)) ++bad;
    bool ok = !counts.empty() && bad == 0;
    checks.note("P5b " + cfgname + ": lines=" + std::to_string(counts.size()) + " bad_lines=" + std::to_string(bad));
    return ok;
}

// K5: every enabled plane has fetches in >=150 lines of every observed
// frame (checked per frame, not merged across the whole observed span).
bool k5_check(const RunResult& run, int n, Checks& checks, const std::string& cfgname) {
    auto blocks = blocks_bitplane_named(n);
    LineRef lref = make_lines(run.log, blocks);
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)}, lref);
    if (restarts.size() < 6) { checks.note("K5 " + cfgname + ": NOSPAN"); return false; }
    bool ok = true; std::ostringstream note;
    for (int f = 0; f < 3; ++f) {
        uint64_t flo = restarts[2 + f], fhi = restarts[3 + f];
        for (int y = 0; y < n; ++y) {
            std::string name = "BPL" + std::to_string(y + 1);
            std::set<int64_t> lines;
            for (const auto& a : run.log) {
                if (a.is_write || a.cycle < flo || a.cycle >= fhi) continue;
                if (attribute(a.word_addr, blocks) != name) continue;
                int64_t li = line_of(lref, a.cycle);
                if (li >= 0) lines.insert(li);
            }
            bool this_ok = lines.size() >= 150;
            ok = ok && this_ok;
            note << "f" << f << name << "=" << lines.size() << (this_ok ? "" : "(FAIL)") << ";";
        }
    }
    checks.note("K5 " + cfgname + ": " + note.str());
    return ok;
}

// K4/K6: every word of [base_word, base_word+n_words) is read at least once
// within [lo,hi).
bool words_fully_read(const std::vector<nmh::RamAccess>& log, uint64_t lo, uint64_t hi,
                       uint32_t base_word, int n_words, size_t* out_count = nullptr) {
    std::set<uint32_t> seen;
    for (const auto& a : log)
        if (!a.is_write && a.cycle >= lo && a.cycle < hi && a.word_addr >= base_word && a.word_addr < base_word + (uint32_t)n_words)
            seen.insert(a.word_addr);
    if (out_count) *out_count = seen.size();
    return seen.size() == (size_t)n_words;
}

// K4: in each observed frame of C3/C6, every word of each of the eight
// sprite structures (86 words: 2 control + 40 data + 2 control + 40 data +
// 2 end) is read at least once. `blocks` must be the run's own declared
// blocks, so the restart fetch is correctly identified (2.4).
bool k4_check(const RunResult& run, const std::vector<Block>& blocks, Checks& checks, const std::string& cfgname) {
    LineRef lref = make_lines(run.log, blocks);
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)}, lref);
    if (restarts.size() < 6) { checks.note("K4 " + cfgname + ": NOSPAN"); return false; }
    bool ok = true; std::ostringstream note;
    for (int f = 0; f < 3; ++f) {
        uint64_t flo = restarts[2 + f], fhi = restarts[3 + f];
        for (int x = 0; x < 8; ++x) {
            size_t cnt = 0;
            bool full = words_fully_read(run.log, flo, fhi, wordof(CH_SPR[x]), 86, &cnt);
            ok = ok && full;
            note << "f" << f << "spr" << x << "=" << cnt << "/86" << (full ? "" : "(FAIL)") << ";";
        }
    }
    checks.note("K4 " + cfgname + ": " + note.str());
    return ok;
}

// K6: in each observed frame of C5/C6/C7a/C7b, every word of L5 (972 words
// from COP_MAIN) and of L5b (12 words from COP_SECOND) is read at least
// once (the list ran to its end). `blocks` as K4.
bool k6_check(const RunResult& run, const std::vector<Block>& blocks, Checks& checks, const std::string& cfgname) {
    LineRef lref = make_lines(run.log, blocks);
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)}, lref);
    if (restarts.size() < 6) { checks.note("K6 " + cfgname + ": NOSPAN"); return false; }
    bool ok = true; std::ostringstream note;
    for (int f = 0; f < 3; ++f) {
        uint64_t flo = restarts[2 + f], fhi = restarts[3 + f];
        size_t c5, c5b;
        bool full5 = words_fully_read(run.log, flo, fhi, wordof(COP_MAIN), 972, &c5);
        bool full5b = words_fully_read(run.log, flo, fhi, wordof(COP_SECOND), 12, &c5b);
        bool this_ok = full5 && full5b;
        ok = ok && this_ok;
        note << "f" << f << ":L5=" << c5 << "/972,L5b=" << c5b << "/12" << (this_ok ? "" : "(FAIL)") << ";";
    }
    checks.note("K6 " + cfgname + ": " + note.str());
    return ok;
}

// ---- P7 machinery: per-segment (line offset, s) pairs, C5 vs C6 --------
//
// list_L5's own word layout: V_spr(32)+V_bpl6(24)+COP2LC movel(4) = 60
// words before the WAIT/MOVE loop; each loop iteration i (0..112) is
// WAIT(2)+3*MOVE(6) = 8 words; the trailer's own WAIT-at-$B4 sits right
// after iteration 112, i.e. at word offset 60+8*113=964. Segment i
// (SPECIFICATION.md P7.3) = the accesses after WAIT i's own two words, up
// to and including WAIT (i+1)'s own two words: word-offset range
// [60+8i+2, 60+8(i+1)+2). `shift_after_i` (>=0) is T1's own contrast
// (SPECIFICATION.md P7.6): one extra DC.W pair inserted right after
// segment `shift_after_i`'s own three MOVEs, widening that segment by 2
// words and moving every later segment's own boundary by 2 words; -1
// means the list is unmodified.
constexpr uint32_t kSegBase = 62; // 60+2: first word after WAIT 0
uint32_t seg_lo_word(uint32_t cop_main_word, int i, int shift_after_i = -1) {
    uint32_t w = cop_main_word + kSegBase + 8u * (uint32_t)i;
    if (shift_after_i >= 0 && i > shift_after_i) w += 2;
    return w;
}
uint32_t seg_hi_word(uint32_t cop_main_word, int i, int shift_after_i = -1) {
    uint32_t w = cop_main_word + kSegBase + 8u * (uint32_t)(i + 1);
    if (shift_after_i >= 0 && i >= shift_after_i) w += 2;
    return w;
}

using PairList = std::vector<std::pair<int64_t, int64_t>>; // (line offset from segment's first access, s)

// [frame 0..2][segment 0..112] -> PairList, time-ordered (run.log is
// chronological). Only valid for a run using the list_L5 layout (C5, C6,
// C7a, C7b, and C6' -- the P7 mutation, which only substitutes one WAIT's
// own HP word and does not shift any segment). For a run that inserts a
// word (T1's own contrast, run_C6_extra50), pass its own `shift_after_i`.
// `blocks` must be the run's OWN full declared-block list (e.g.
// blocks_full(4) for C6): the refresh set is identified by exclusion
// (SPECIFICATION.md D7/2.2), so a run's other live DMA traffic must be
// declared here or it is misattributed "R" and corrupts the line/refresh-
// group construction that segments and lines are anchored on.
std::vector<std::vector<PairList>> compute_segments(const RunResult& run, const std::vector<Block>& blocks,
                                                      int shift_after_i = -1) {
    std::vector<std::vector<PairList>> out(3, std::vector<PairList>(113));
    LineRef lref = make_lines(run.log, blocks);
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)}, lref);
    if (restarts.size() < 6) return out;
    uint32_t base = wordof(COP_MAIN);
    std::array<uint32_t, 114> bounds;
    for (int i = 0; i <= 113; ++i) bounds[(size_t)i] = seg_lo_word(base, i, shift_after_i);
    auto seg_index = [&](uint32_t w) -> int {
        if (w < bounds[0] || w >= bounds[113]) return -1;
        size_t idx = std::upper_bound(bounds.begin(), bounds.end(), w) - bounds.begin();
        return (int)idx - 1;
    };
    std::vector<std::optional<int64_t>> li0(3 * 113);
    for (const auto& a : run.log) {
        if (a.is_write) continue;
        int f = -1;
        for (int k = 0; k < 3; ++k) if (a.cycle >= restarts[2 + k] && a.cycle < restarts[3 + k]) { f = k; break; }
        if (f < 0) continue;
        int i = seg_index(a.word_addr);
        if (i < 0) continue;
        int64_t li = line_of(lref, a.cycle);
        if (li < 0) continue;
        int idx = f * 113 + i;
        if (!li0[idx]) li0[idx] = li;
        int64_t s = in_line_cycle(lref, li, a.cycle);
        out[f][i].push_back({li - *li0[idx], s});
    }
    return out;
}

// P7: for every i and every observed frame, segment i's (line offset, s)
// pair list is identical in C5 and C6.
bool p7_check(const RunResult& c5, const std::vector<Block>& b5, const RunResult& c6, const std::vector<Block>& b6,
              Checks& checks) {
    auto s5 = compute_segments(c5, b5), s6 = compute_segments(c6, b6);
    int mismatches = 0;
    for (int f = 0; f < 3; ++f) for (int i = 0; i < 113; ++i) if (s5[f][i] != s6[f][i]) ++mismatches;
    checks.note("P7: segment mismatches (C5 vs C6, 3 frames x 113 segments) = " + std::to_string(mismatches));
    return mismatches == 0;
}

// T1: every segment has the same number of Copper accesses in both runs.
// `shift_c6` is T1's own contrast on the C6 side (run_C6_extra50): -1 for
// the normal (unshifted) check.
bool t1_check(const RunResult& c5, const std::vector<Block>& b5, const RunResult& c6, const std::vector<Block>& b6,
              Checks& checks, int shift_c6 = -1) {
    auto s5 = compute_segments(c5, b5, -1), s6 = compute_segments(c6, b6, shift_c6);
    int mismatched = 0;
    for (int f = 0; f < 3; ++f) for (int i = 0; i < 113; ++i) if (s5[f][i].size() != s6[f][i].size()) ++mismatched;
    checks.note("T1: segments with differing access count (C5 vs C6) = " + std::to_string(mismatched));
    return mismatched == 0;
}

// T2: no CPU-block access lies in any line spanned by a segment.
bool t2_check(const RunResult& run, const std::vector<Block>& blocks, Checks& checks, const std::string& name) {
    LineRef lref = make_lines(run.log, blocks);
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)}, lref);
    auto span = observed_span(restarts);
    if (!span.ok) { checks.note("T2 " + name + ": NOSPAN"); return false; }
    uint32_t base = wordof(COP_MAIN);
    std::set<int64_t> segment_lines;
    for (const auto& a : run.log) {
        if (a.is_write || a.cycle < span.lo || a.cycle >= span.hi) continue;
        if (a.word_addr < seg_lo_word(base, 0) || a.word_addr >= seg_hi_word(base, 112)) continue;
        int64_t li = line_of(lref, a.cycle);
        if (li >= 0) segment_lines.insert(li);
    }
    int violations = 0;
    for (const auto& a : run.log) {
        if (a.cycle < span.lo || a.cycle >= span.hi) continue;
        if (attribute(a.word_addr, blocks) != "CPU") continue;
        int64_t li = line_of(lref, a.cycle);
        if (li >= 0 && segment_lines.count(li)) ++violations;
    }
    checks.note("T2 " + name + ": segment_lines=" + std::to_string(segment_lines.size()) +
                " cpu_violations=" + std::to_string(violations));
    return violations == 0;
}

// T3: in the lines of the segments in C6, bitplane fetches occur in every
// line; sprite fetches occur in every line of segments i=16..35 and
// 64..83 (VP $50-$63 and $80-$93, matching VP=$40+i by list_L5's own
// construction); audio and disk accesses each occur in at least half of
// the lines.
bool t3_check(const RunResult& run, const std::vector<Block>& blocks, Checks& checks) {
    LineRef lref = make_lines(run.log, blocks);
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)}, lref);
    if (restarts.size() < 6) { checks.note("T3: NOSPAN"); return false; }
    uint32_t base = wordof(COP_MAIN);
    std::vector<std::vector<int64_t>> seg_line(3, std::vector<int64_t>(113, -1));
    for (const auto& a : run.log) {
        if (a.is_write) continue;
        int f = -1;
        for (int k = 0; k < 3; ++k) if (a.cycle >= restarts[2 + k] && a.cycle < restarts[3 + k]) { f = k; break; }
        if (f < 0) continue;
        if (a.word_addr < seg_lo_word(base, 0) || a.word_addr >= seg_hi_word(base, 112)) continue;
        int i = (int)((a.word_addr - seg_lo_word(base, 0)) / 8);
        if (i < 0 || i > 112) continue;
        int64_t li = line_of(lref, a.cycle);
        if (li < 0) continue;
        if (seg_line[f][i] < 0) seg_line[f][i] = li;
    }
    int bpl_missing = 0, spr_missing = 0, total_lines = 0, aud_lines = 0, dsk_lines = 0;
    for (int f = 0; f < 3; ++f) {
        uint64_t flo = restarts[2 + f], fhi = restarts[3 + f];
        for (int i = 0; i < 113; ++i) {
            if (seg_line[f][i] < 0) continue;
            bool bpl = false, spr = false, aud = false, dsk = false;
            for (const auto& a : run.log) {
                if (a.is_write || a.cycle < flo || a.cycle >= fhi) continue;
                if (line_of(lref, a.cycle) != seg_line[f][i]) continue;
                uint32_t w = a.word_addr;
                if (w >= wordof(CH_BPL[0]) && w < wordof(CH_BPL[3] + 0x8000)) bpl = true;
                else if ((w >= wordof(CH_SPR[0]) && w < wordof(CH_SPR[7] + 0x400)) ||
                         (w >= wordof(CH_SPR_NULL) && w < wordof(0x00B000))) spr = true;
                else if (w >= wordof(CH_AUD[0]) && w < wordof(CH_AUD[3] + 0x200)) aud = true;
                else if (w >= wordof(CH_DISK) && w < wordof(CH_DISK + 16000)) dsk = true;
            }
            ++total_lines;
            if (!bpl) ++bpl_missing;
            bool in_spr_range = (i >= 16 && i <= 35) || (i >= 64 && i <= 83);
            if (in_spr_range && !spr) ++spr_missing;
            if (aud) ++aud_lines;
            if (dsk) ++dsk_lines;
        }
    }
    bool ok = total_lines > 0 && bpl_missing == 0 && spr_missing == 0 &&
              aud_lines * 2 >= total_lines && dsk_lines * 2 >= total_lines;
    checks.note("T3: total_lines=" + std::to_string(total_lines) + " bpl_missing=" + std::to_string(bpl_missing) +
                " spr_missing(in-range)=" + std::to_string(spr_missing) + " aud_lines=" + std::to_string(aud_lines) +
                " dsk_lines=" + std::to_string(dsk_lines));
    return ok;
}

// ---- folding several configs' runs into one population check -----------

struct Pop { std::string name; RunResult run; std::vector<Block> blocks; std::vector<uint32_t> starts; };

bool fold_check(const std::vector<Pop>& all, const std::vector<std::string>& names, const std::string& target,
                 bool want_even, Checks& checks, const std::string& tag) {
    bool ok = true;
    std::ostringstream note;
    for (const auto& n : names) {
        const Pop* p = nullptr;
        for (const auto& c : all) if (c.name == n) { p = &c; break; }
        if (!p) { ok = false; note << n << ":MISSING;"; continue; }
        auto r = analyze_population(p->run.log, p->blocks, p->starts, target, false);
        bool this_ok = p->run.done.has_value() && r.restarts_ok && r.f1_ok && r.f2_ok && r.f3_ok && r.population > 0 &&
                        (want_even ? r.all_even : r.all_odd);
        ok = ok && this_ok;
        note << n << ":pop=" << r.population << (want_even ? ",even=" : ",odd=")
             << (want_even ? r.all_even : r.all_odd) << (this_ok ? "" : "(FAIL)") << ";";
    }
    checks.note(tag + " per-config: " + note.str());
    return ok;
}

// ---- small comparators reused by both the normal run and a mutation ----

// K2a's own comparator (SPECIFICATION.md P2.5): the stored DSKBYTR has
// DMAON=1 and DISKWRITE==want_write.
bool dskbytr_ok(const RunResult& run, bool want_write, uint16_t* out_v = nullptr) {
    if (run.peeks.empty() || !run.peeks[0]) return false;
    uint16_t v = *run.peeks[0];
    if (out_v) *out_v = v;
    bool dmaon = (v & 0x4000) != 0, diskwrite = (v & 0x2000) != 0;
    return dmaon && diskwrite == want_write;
}

// K2b's own comparator: the disk-block population (>=500 is graded by the
// caller).
size_t disk_population(const RunResult& run, const std::vector<Block>& blocks) {
    return analyze_population(run.log, blocks, {wordof(COP_MAIN)}, "Disk", false).population;
}

// K3's own comparator: channel x's own population in the observed frames
// (SPECIFICATION.md P3.5). `blocks` must be the run's own declared blocks
// (D2.4).
size_t audio_channel_population(const RunResult& run, const std::vector<Block>& blocks, int x) {
    LineRef lref = make_lines(run.log, blocks);
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)}, lref);
    auto span = observed_span(restarts);
    if (!span.ok) return 0;
    size_t n = 0;
    for (const auto& a : run.log)
        if (!a.is_write && a.cycle >= span.lo && a.cycle < span.hi &&
            a.word_addr >= wordof(CH_AUD[x]) && a.word_addr < wordof(CH_AUD[x] + 0x200)) ++n;
    return n;
}

// P6's own MEASURED line (defect 1): among the Copper-block's own reads
// in the observed frames, how many fall on an even in-line cycle, at
// which s, and whether each one repeats (same word, one cycle -- gS --
// earlier) the previous read.
struct EvenReadInfo { size_t count = 0; bool all_at_s226 = true; bool all_repeat_prev = true; };
EvenReadInfo measure_p6_even_reads(const RunResult& run, const std::vector<Block>& blocks) {
    EvenReadInfo info;
    LineRef lref = make_lines(run.log, blocks);
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)}, lref);
    auto span = observed_span(restarts);
    if (!span.ok) return info;
    std::set<std::pair<uint32_t, uint64_t>> reads;
    for (const auto& a : run.log)
        if (!a.is_write && a.cycle >= span.lo && a.cycle < span.hi) reads.insert({a.word_addr, a.cycle});
    for (const auto& a : run.log) {
        if (a.is_write || a.cycle < span.lo || a.cycle >= span.hi) continue;
        if (attribute(a.word_addr, blocks) != "Copper") continue;
        int64_t li = line_of(lref, a.cycle);
        if (li < 0) continue;
        int64_t s = in_line_cycle(lref, li, a.cycle);
        if (s % 2 != 0) continue;
        ++info.count;
        if (s != 226) info.all_at_s226 = false;
        if (a.cycle < gS || !reads.count({a.word_addr, a.cycle - gS})) info.all_repeat_prev = false;
    }
    return info;
}

} // namespace

// MUTATION P1 stimulus red C0-u (Copper jumps to sub-list U right after line $80's refresh group), via NMH_MUTATE=P1 -- measured: red on fx68k, red on tg68k
// MUTATION P2 stimulus red X-dsk (C2a with list L5, DSKPT at the Copper block), via NMH_MUTATE=P2 -- measured: red on fx68k, red on tg68k
// MUTATION P3 stimulus red X-aud (C1a with list L5, AUD0LC at the Copper block), via NMH_MUTATE=P3 -- measured: red on fx68k, red on tg68k
// MUTATION P4 stimulus red X-spr (C3 with list L3x, structure X for sprite 0), via NMH_MUTATE=P4 -- measured: red on fx68k, red on tg68k
// MUTATION P5 stimulus red X-bpl (C4-4 with list L5, BPL1PT at the Copper block), via NMH_MUTATE=P5 -- measured: red on fx68k, red on tg68k
// MUTATION P6 (already red at baseline; contrast, not a stimulus) the green control (C0-C4, S1, S2) through the same comparator, via NMH_MUTATE=P6 -- measured: green on fx68k, green on tg68k
// MUTATION A1 stimulus red C1a graded with every layout block declared (no longer vacuous by construction), via NMH_MUTATE=A1 -- measured: red on fx68k, red on tg68k
// MUTATION F1 self-test: a synthetic trace with one access moved by one time unit must fail F1 (no RTL run), via NMH_MUTATE=F1 -- measured: green on fx68k, green on tg68k (the checker fails on the perturbed trace, as it must)
// MUTATION F2 self-test: a synthetic refresh group reduced to one access must fail F2 (no RTL run; no stimulus falsifies it directly, as for F1), via NMH_MUTATE=F2 -- measured: green on fx68k, green on tg68k (the checker fails on the perturbed trace, as it must)
// MUTATION F23 stimulus red C0-m (list L0m, Copper fetches U near HP=$70 of line $80, splitting it): F3 only -- F2 stays true in this run, via NMH_MUTATE=F23 -- measured: red on fx68k, red on tg68k, through F3 only
// MUTATION K2a stimulus red C2a with both DSKLEN writes $4000|8000 (bit15 clear, DMAON=0), via NMH_MUTATE=K2a -- measured: red on fx68k, red on tg68k
// MUTATION K3 stimulus red C1a without AUD0EN in the S4 write, via NMH_MUTATE=K3 -- measured: red on fx68k, red on tg68k
// MUTATION K4 stimulus red C3 without SPREN, via NMH_MUTATE=K4 -- measured: red on fx68k, red on tg68k
// MUTATION K5 stimulus red C4-4 without BPLEN, via NMH_MUTATE=K5 -- measured: red on fx68k, red on tg68k
// MUTATION K6 stimulus red C6 with list L5 truncated to V_spr+V_bpl(6)+END, via NMH_MUTATE=K6 -- measured: red on fx68k, red on tg68k
// MUTATION T1 stimulus red C6-extra50 (list L5 with one extra DC.W $0180,$0000 after WAIT 50): only segment 50 differs, via NMH_MUTATE=T1 -- measured: red on fx68k, red on tg68k, 3 (segment, frame) mismatches (segment 50, all 3 observed frames), through the clause it targets
// MUTATION T2 stimulus red C6 with STOP replaced by a self-branch loop (prog_c6_stopself, disk sequence unchanged), via NMH_MUTATE=T2 -- measured: red on fx68k, red on tg68k
// MUTATION T3 stimulus red C6 without SPREN (prog_c6_nospren): no sprite fetches in segments 16-35/64-83, via NMH_MUTATE=T3 -- measured: red on fx68k, red on tg68k
// MUTATION P7 stimulus red C6' (list L5 with WAIT 32 at HP=$42 instead of $40): segment 32's pair list differs, via NMH_MUTATE=P7 -- measured: red on fx68k, red on tg68k
// MUTATION C1 stimulus red: the STOP loop's own exit condition removed (prog_c0_nofinish); the completion word is then missing, via NMH_MUTATE=C1 -- measured: red on fx68k, red on tg68k
//
// Not exercised, by a scope decision: a dedicated mutation for K2b (the
// >=500-access count alone, independent of K2a's DMAON=0) -- C2b's own
// baseline run already shows the >=500 threshold failing organically (0
// disk-block accesses, D9's no-drive case), which is a real decline, not a
// synthetic contrast, so no separate mutant was built for it. P8-P12
// (video/sprite-image propositions) are not implemented in this package at
// all -- see the RECORDED lines in the normal run.

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;
    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    if (!mv.empty()) {
        // ---- mutation runs: exactly one capture, never counted as the -----
        // ---- normal fixture/precondition/assertion summary. Every branch --
        // ---- runs the package's own comparator unchanged and expects its --
        // ---- own real verdict directly -- never negated -- so a mutation --
        // ---- that breaks the clause under test prints FAIL through it. ----
        if (mv == "P1") {
            RunResult r = run_C0u(dir);
            set_gS(r.log, blocks_C0(), checks);
            std::vector<Pop> tmp = {{"C0-u", r, blocks_C0(), {wordof(COP_MAIN)}}};
            bool result = fold_check(tmp, {"C0-u"}, "R", true, checks, "P1 mutation");
            checks.note("C0-u: done=" + std::to_string(r.done.has_value()) + " S=" + std::to_string(gS));
            checks.expect(result, "P1: every refresh access lies on an even in-line cycle, in every run and mode graded. "
                          "ch2-about-the-copper.c24 COP2-about-the-copper.c29 p.20");
        } else if (mv == "P2") {
            RunResult r = run_XDsk(dir);
            set_gS(r.log, blocks_disk(COP_MAIN, 8000), checks);
            // Disk's own declared span (8000 words = 16000 bytes from
            // COP_MAIN) already reaches past COP_SECOND, so L5b is not
            // separately misattributed here (unlike X-aud's narrower
            // Shared span, defect 5).
            std::vector<Block> b = {disk_block(COP_MAIN, 8000)};
            std::vector<Pop> tmp = {{"X-dsk", r, b, {wordof(COP_MAIN)}}};
            bool result = fold_check(tmp, {"X-dsk"}, "Disk", true, checks, "P2 mutation");
            checks.note("X-dsk: done=" + std::to_string(r.done.has_value()) + " S=" + std::to_string(gS));
            checks.expect(result, "P2: every disk-DMA access lies on an even in-line cycle (C2a, C2b, C6). "
                          "ch2-about-the-copper.c23 COP2-about-the-copper.c28 p.20");
        } else if (mv == "P3" || mv == "P6") {
            RunResult r = run_XAud(dir);
            // Every live block of this run must be declared, or its
            // traffic is misattributed "R" by exclusion and corrupts the
            // line/refresh-group construction (defect 5): the Shared span
            // (Copper's L5 and audio channel 0's own overlap) reaches only
            // to COP_MAIN+$1000, short of COP_SECOND (L5b), and channels
            // 1-3 are still live (C1a's own DMACON) at their own,
            // non-overlapping blocks.
            std::vector<Block> b = {
                {"Shared", wordof(COP_MAIN), wordof(COP_MAIN + 0x1000)},
                {"L5b", wordof(COP_SECOND), wordof(COP_SECOND + 0x100)},
                audio_block(1), audio_block(2), audio_block(3),
            };
            set_gS(r.log, b, checks);
            if (mv == "P3") {
                std::vector<Pop> tmp = {{"X-aud", r, b, {wordof(COP_MAIN)}}};
                bool result = fold_check(tmp, {"X-aud"}, "Shared", true, checks, "P3 mutation");
                checks.note("X-aud: done=" + std::to_string(r.done.has_value()) + " S=" + std::to_string(gS));
                checks.expect(result, "P3: every audio-DMA access, all four channels, lies on an even in-line cycle "
                              "(C1a, C1b, C6). ch2-about-the-copper.c22 COP2-about-the-copper.c27 p.20");
            } else {
                // P6 is already red at baseline (C5/C6/C7a/C7b: see the
                // normal run's own record). For an assertion that is
                // already red, the record shows a different, proven-live
                // input through the same comparator going green instead of
                // a stimulus red: the green control (C0-C4, S1, S2).
                std::vector<Pop> greens;
                RunResult c0 = run_C0(dir), c1a = run_C1a(dir), c1b = run_C1b(dir), c2a = run_C2a(dir), c2b = run_C2b(dir), c3 = run_C3(dir);
                RunResult c4[6];
                for (int n = 1; n <= 6; ++n) c4[n - 1] = run_C4(dir, n);
                RunResult c44w = run_C4(dir, 4, true);
                RunResult s1 = run_S1(dir), s2 = run_S2(dir);
                greens.push_back({"C0", c0, blocks_C0(), {wordof(COP_MAIN)}});
                greens.push_back({"C1a", c1a, blocks_audio(), {wordof(COP_MAIN)}});
                greens.push_back({"C1b", c1b, blocks_audio(), {wordof(COP_MAIN)}});
                greens.push_back({"C2a", c2a, blocks_disk(), {wordof(COP_MAIN)}});
                greens.push_back({"C2b", c2b, blocks_disk(), {wordof(COP_MAIN)}});
                greens.push_back({"C3", c3, blocks_sprite(), {wordof(COP_MAIN)}});
                for (int n = 1; n <= 6; ++n) greens.push_back({"C4-" + std::to_string(n), c4[n - 1], blocks_bitplane(n), {wordof(COP_MAIN)}});
                greens.push_back({"C4-4w", c44w, blocks_bitplane(4), {wordof(COP_MAIN)}});
                greens.push_back({"S1", s1, blocks_S1(), {wordof(COP_MAIN)}});
                greens.push_back({"S2", s2, blocks_S2(), {wordof(COP_MAIN), wordof(COP_SECOND)}});
                std::vector<std::string> names; for (auto& p : greens) names.push_back(p.name);
                bool green = fold_check(greens, names, "Copper", false, checks, "P6 control");
                checks.expect(green, "P6: every Copper-block access lies on an odd in-line cycle, in every run and mode "
                              "graded. ch2-about-the-copper.c21 p.20 -- shown here on the green control (C0-C4, S1, S2); "
                              "C5/C6/C7a/C7b's own FAIL is the package's standing red (see the normal run).");
            }
        } else if (mv == "P5") {
            RunResult r = run_XBpl(dir);
            // XBPL1's own declared span (COP_MAIN, +$2800 bytes) already
            // reaches past COP_SECOND (L5b); planes 2-4 (CH_BPL2-4) are
            // still live (BPLEN) at their own, non-overlapping blocks
            // (defect 6).
            std::vector<Block> b = {
                {"XBPL1", wordof(COP_MAIN), wordof(COP_MAIN + 0x2800)},
                bitplane_block(1), bitplane_block(2), bitplane_block(3),
            };
            set_gS(r.log, b, checks);
            std::vector<Pop> tmp = {{"X-bpl", r, b, {wordof(COP_MAIN)}}};
            bool result = fold_check(tmp, {"X-bpl"}, "XBPL1", true, checks, "P5 mutation");
            checks.note("X-bpl: done=" + std::to_string(r.done.has_value()) + " S=" + std::to_string(gS));
            checks.expect(result, "P5: (a) every access in bitplanes 1-4 lies on an even in-line cycle, in every "
                          "config with 1-6 low-resolution planes and at both fetch starts, and in C6; (b) in every "
                          "line with bitplane fetches, even-cycle fetches outnumber odd-cycle ones. "
                          "ch2-about-the-copper.c26 COP2-about-the-copper.c31 p.20");
        } else if (mv == "P4") {
            RunResult r = run_XSpr(dir);
            set_gS(r.log, blocks_sprite(), checks);
            std::vector<Pop> tmp = {{"X-spr", r, blocks_sprite(), {wordof(COP_MAIN)}}};
            bool result = fold_check(tmp, {"X-spr"}, "Sprite", true, checks, "P4 mutation");
            checks.note("X-spr: done=" + std::to_string(r.done.has_value()) + " S=" + std::to_string(gS));
            checks.expect(result, "P4: every sprite-DMA access, all eight channels, lies on an even in-line cycle "
                          "(C3, C6, S1, S2). ch2-about-the-copper.c25 COP2-about-the-copper.c30 p.20");
        } else if (mv == "A1") {
            // The audio blocks (and every other channel's own block) must
            // be declared for this to be a real contrast: attribute()
            // falls back to "R" for any word address outside every
            // declared block (D7), so grading against a block list that
            // declares only Copper would silently reclassify every audio
            // access as refresh and a1_violations could never be anything
            // but 0 -- the baseline check is graded the same way (defect 3).
            RunResult r = run_C1a(dir);
            set_gS(r.log, blocks_full(4), checks);
            auto pr = analyze_population(r.log, blocks_full(4), {wordof(COP_MAIN)}, "R", true);
            checks.note("A1 mutation (C1a graded with every layout block declared): violations=" + std::to_string(pr.a1_violations));
            checks.expect(pr.restarts_ok && pr.a1_ok, "A1: in the baseline configuration C0, every access in the "
                          "observed frames is either in the refresh set R, the CPU block, or the Copper block. "
                          "SPECIFICATION.md 2.2");
        } else if (mv == "F1") {
            // Self-test, no RTL run (SPECIFICATION.md F1's own contrast:
            // "None exists ... verified by a self-test"): a synthetic
            // two-access trace on an S-cycle grid, then one access moved by
            // one time unit, must fail check_f1. The grid period is a
            // fixed, documented synthetic value (this tests the checker's
            // own logic, not a hardware measurement).
            constexpr uint64_t kSyntheticS = 8;
            gS = kSyntheticS;
            std::vector<nmh::RamAccess> good = {
                {1000, 0x1000, 0, false, true, true},
                {1000 + kSyntheticS, 0x1002, 0, false, true, true},
            };
            std::vector<nmh::RamAccess> bad = good;
            bad[1].cycle += 1;
            bool good_ok = check_f1(good, {}, 0, 2000);
            bool bad_ok = check_f1(bad, {}, 0, 2000);
            checks.note("F1 self-test: unperturbed trace F1=" + std::to_string(good_ok) +
                        ", one access moved by 1 time unit F1=" + std::to_string(bad_ok));
            checks.expect(good_ok && !bad_ok, "F1 mutation: moving one access off the S-cycle grid by one time "
                          "unit makes check_f1 fail. SPECIFICATION.md 2.1");
        } else if (mv == "F2") {
            // F2's own self-test (as F1's): SPECIFICATION.md's own
            // contrast for F2 (C0-m, shared with F3) does not falsify it
            // (see the F23 record); no stimulus does. A synthetic trace
            // with two 4-access refresh groups, then one group reduced to
            // a single access, must fail check_f2_synthetic.
            constexpr uint64_t kSyntheticS = 8;
            gS = kSyntheticS;
            std::vector<uint64_t> good = {0, 2 * kSyntheticS, 4 * kSyntheticS, 6 * kSyntheticS,
                                           227 * kSyntheticS, 227 * kSyntheticS + 2 * kSyntheticS,
                                           227 * kSyntheticS + 4 * kSyntheticS, 227 * kSyntheticS + 6 * kSyntheticS};
            std::vector<uint64_t> bad = {0, 227 * kSyntheticS, 227 * kSyntheticS + 2 * kSyntheticS,
                                          227 * kSyntheticS + 4 * kSyntheticS, 227 * kSyntheticS + 6 * kSyntheticS};
            bool good_ok = check_f2_synthetic(good, kSyntheticS);
            bool bad_ok = check_f2_synthetic(bad, kSyntheticS);
            checks.note("F2 self-test: two 4-access groups F2=" + std::to_string(good_ok) +
                        ", one group reduced to a single access F2=" + std::to_string(bad_ok));
            checks.expect(good_ok && !bad_ok, "F2 mutation: a refresh group reduced to a single access makes "
                          "check_f2 fail (no stimulus falsifies F2 directly, as for F1). SPECIFICATION.md 2.3");
        } else if (mv == "F23") {
            RunResult r = run_C0m(dir);
            set_gS(r.log, blocks_C0(), checks);
            auto pr = analyze_population(r.log, blocks_C0(), {wordof(COP_MAIN)}, "R", false);
            checks.note("C0-m: done=" + std::to_string(r.done.has_value()) + " F2=" + std::to_string(pr.f2_ok) +
                        " F3=" + std::to_string(pr.f3_ok));
            checks.expect(pr.f3_ok, "F3: for every pair of consecutive groups in the observed frames, "
                          "A(n+1)-A(n) equals 227*S or 228*S. p.23");
        } else if (mv == "K2a") {
            RunResult r = run_C2a_noenable(dir);
            set_gS(r.log, blocks_disk(), checks);
            uint16_t v = 0;
            bool ok = dskbytr_ok(r, true, &v);
            checks.note("C2a-noenable: DSKBYTR=" + hex(v));
            checks.expect(ok, "K2a: the stored DSKBYTR has DMAON=1 and DISKWRITE=1 in C2a and C6, DISKWRITE=0 in "
                          "C2b. p.248 Appendix A p.278");
        } else if (mv == "K3") {
            RunResult r = run_C1a(dir, true);
            set_gS(r.log, blocks_audio(), checks);
            size_t ch0 = audio_channel_population(r, blocks_audio(), 0);
            checks.note("C1a-noaud0: channel0_population=" + std::to_string(ch0));
            checks.expect(ch0 >= 100, "K3: each of the four audio channels has at least 100 accesses in the "
                          "observed frames of C1a, C1b and C6. p.144");
        } else if (mv == "K4") {
            RunResult r = run_C3(dir, true);
            set_gS(r.log, blocks_sprite(), checks);
            bool k4 = k4_check(r, blocks_sprite(), checks, "C3-nospren");
            checks.expect(k4, "K4: in each observed frame of C3 and C6, every word of each of the eight sprite "
                          "structures (control, data and end words) is read at least once. p.108");
        } else if (mv == "K5") {
            RunResult r = run_C4(dir, 4, false, true);
            set_gS(r.log, blocks_bitplane(4), checks);
            bool k5 = k5_check(r, 4, checks, "C4-4-nobplen");
            checks.expect(k5, "K5: every enabled plane has fetches in at least 150 lines of every observed frame "
                          "(C4-1..C4-6, C4-4w, C6).");
        } else if (mv == "K6") {
            RunResult r = run_C6_truncated(dir);
            set_gS(r.log, blocks_full(4), checks);
            bool k6 = k6_check(r, blocks_full(4), checks, "C6-truncated");
            checks.expect(k6, "K6: in every observed frame of C5, C6, C7a and C7b, every word of L5 and L5b is "
                          "read at least once (the list ran to its end).");
        } else if (mv == "T1") {
            RunResult c5 = run_C5(dir);
            RunResult c6x = run_C6_extra50(dir);
            set_gS(c5.log, blocks_full(4), checks);
            bool t1 = t1_check(c5, blocks_full(4), c6x, blocks_full(4), checks, 50);
            checks.expect(t1, "T1: every segment has the same number of Copper accesses in C5 and C6.");
        } else if (mv == "T2") {
            RunResult r = run_C6(dir, "/prog_c6_stopself.bin");
            set_gS(r.log, blocks_full(4), checks);
            bool t2 = t2_check(r, blocks_full(4), checks, "C6-stopself");
            checks.expect(t2, "T2: no CPU-block access lies in any line spanned by a segment, in C5 or C6.");
        } else if (mv == "T3") {
            RunResult r = run_C6(dir, "/prog_c6_nospren.bin");
            set_gS(r.log, blocks_full(4), checks);
            bool t3 = t3_check(r, blocks_full(4), checks);
            checks.expect(t3, "T3: in the lines of the segments in C6, bitplane fetches occur in every line; "
                          "sprite fetches occur in every line of segments 16-35/64-83 (VP $50-$63/$80-$93); audio "
                          "and disk accesses each occur in at least half of the lines.");
        } else if (mv == "P7") {
            RunResult c5 = run_C5(dir);
            RunResult c6p = run_C6prime(dir);
            set_gS(c5.log, blocks_full(4), checks);
            bool p7 = p7_check(c5, blocks_full(4), c6p, blocks_full(4), checks);
            checks.expect(p7, "P7: for every segment i (i=0..112) and every observed frame, the list of (line "
                          "offset, s) pairs is identical in C5 and C6 -- the channels on the even cycles never "
                          "delay the Copper. ch2-about-the-copper.c21 p.20");
        } else if (mv == "C1") {
            RunResult r = run_C0_nofinish(dir);
            checks.note("C0-nofinish: loaded=" + std::to_string(r.loaded) + " released=" + std::to_string(r.released) +
                        " done=" + std::to_string(r.done.has_value()));
            checks.expect(r.loaded && r.released && r.done.has_value(),
                          "FIXTURE: every configuration's program loaded, released reset, and reached DONE");
        } else {
            checks.expect(false, "unknown NMH_MUTATE value: " + mv);
        }
        std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
        return checks.failed == 0 ? 0 : 1;
    }

    // ---- normal run: build every configuration once, fold into P1-P4,P6 --
    std::vector<Pop> all;
    all.push_back({"C0", run_C0(dir), blocks_C0(), {wordof(COP_MAIN)}});
    all.push_back({"C1a", run_C1a(dir), blocks_audio(), {wordof(COP_MAIN)}});
    all.push_back({"C1b", run_C1b(dir), blocks_audio(), {wordof(COP_MAIN)}});
    all.push_back({"C2a", run_C2a(dir), blocks_disk(), {wordof(COP_MAIN)}});
    all.push_back({"C2b", run_C2b(dir), blocks_disk(), {wordof(COP_MAIN)}});
    all.push_back({"C3", run_C3(dir), blocks_sprite(), {wordof(COP_MAIN)}});
    for (int n = 1; n <= 6; ++n)
        all.push_back({"C4-" + std::to_string(n), run_C4(dir, n), blocks_bitplane(n), {wordof(COP_MAIN)}});
    all.push_back({"C4-4w", run_C4(dir, 4, true), blocks_bitplane(4), {wordof(COP_MAIN)}});
    all.push_back({"C5", run_C5(dir), blocks_full(4), {wordof(COP_MAIN)}});
    all.push_back({"C6", run_C6(dir), blocks_full(4), {wordof(COP_MAIN)}});
    all.push_back({"C7a", run_C7a(dir), blocks_bitplane(6), {wordof(COP_MAIN)}});
    all.push_back({"C7b", run_C7b(dir), blocks_bitplane(4), {wordof(COP_MAIN)}});
    all.push_back({"S1", run_S1(dir), blocks_S1(), {wordof(COP_MAIN)}});
    all.push_back({"S2", run_S2(dir), blocks_S2(), {wordof(COP_MAIN), wordof(COP_SECOND)}});
    // SPECIFICATION.md 2.10 mode variants of C0, C6 and S1 (P1, P6 only):
    // no new program is needed, only a different Config per run.
    all.push_back({"C0-ntsc-ecs", run_C0(dir, mode_config(1)), blocks_C0(), {wordof(COP_MAIN)}});
    all.push_back({"C0-pal-ocs", run_C0(dir, mode_config(2)), blocks_C0(), {wordof(COP_MAIN)}});
    all.push_back({"C0-ntsc-ocs", run_C0(dir, mode_config(3)), blocks_C0(), {wordof(COP_MAIN)}});
    all.push_back({"C6-ntsc-ecs", run_C6(dir, "/prog_c6.bin", mode_config(1)), blocks_full(4), {wordof(COP_MAIN)}});
    all.push_back({"C6-pal-ocs", run_C6(dir, "/prog_c6.bin", mode_config(2)), blocks_full(4), {wordof(COP_MAIN)}});
    all.push_back({"C6-ntsc-ocs", run_C6(dir, "/prog_c6.bin", mode_config(3)), blocks_full(4), {wordof(COP_MAIN)}});
    all.push_back({"S1-ntsc-ecs", run_S1(dir, mode_config(1)), blocks_S1(), {wordof(COP_MAIN)}});
    all.push_back({"S1-pal-ocs", run_S1(dir, mode_config(2)), blocks_S1(), {wordof(COP_MAIN)}});
    all.push_back({"S1-ntsc-ocs", run_S1(dir, mode_config(3)), blocks_S1(), {wordof(COP_MAIN)}});

    // S (SPECIFICATION.md 2.1): measured once, from C0's own refresh trace.
    {
        const Pop* c0 = nullptr;
        for (auto& p : all) if (p.name == "C0") c0 = &p;
        if (c0) set_gS(c0->run.log, c0->blocks, checks);
        else { checks.note("S measurement: no C0 run found"); gS = 1; }
        checks.note("S measured (shortest refresh-group interval / 227, p.23) = " + std::to_string(gS));
    }

    // F1, F2, F3: their own printed, graded lines per run, before P1
    // (SPECIFICATION.md 2.1/2.3). F1 is scoped to DMA accesses (D5); the
    // CPU's own phase is measured, not graded, on a line of its own.
    for (auto& p : all) {
        auto pr = analyze_population(p.run.log, p.blocks, p.starts, "R", false);
        checks.expect(pr.f1_ok, "F1 " + p.name + ": the run's DMA accesses lie on one S-cycle phase (the CPU's own "
                      "phase is measured, not graded here, D5). SPECIFICATION.md 2.1");
        checks.expect(pr.f2_ok, "F2 " + p.name + ": every refresh group in the observed frames has at least two "
                      "accesses. p.194");
        checks.expect(pr.f3_ok, "F3 " + p.name + ": for every pair of consecutive refresh groups in the observed "
                      "frames, A(n+1)-A(n) equals 227*S or 228*S. p.23");
    }
    {
        // CPU phase relative to the DMA grid (F1's own scope, D5):
        // measured, not graded -- no page gives the 680x0 a phase at
        // these ports.
        const Pop* c0 = nullptr;
        for (auto& p : all) if (p.name == "C0") c0 = &p;
        std::set<uint64_t> phases;
        if (c0) {
            LineRef lref = make_lines(c0->run.log, c0->blocks);
            auto restarts = restart_fetches(c0->run.log, {wordof(COP_MAIN)}, lref);
            auto span = observed_span(restarts);
            if (span.ok)
                for (const auto& a : c0->run.log)
                    if (a.cycle >= span.lo && a.cycle < span.hi && attribute(a.word_addr, c0->blocks) == "CPU")
                        phases.insert(a.cycle % gS);
        }
        std::ostringstream ph; for (auto ps : phases) ph << ps << ";";
        checks.note("CPU phase mod S (C0, D5, not graded by F1): " + ph.str());
    }

    bool fixture_ok = true;
    for (auto& p : all) fixture_ok = fixture_ok && p.run.loaded && p.run.released && p.run.done.has_value();
    checks.expect(fixture_ok, "FIXTURE: every configuration's program loaded, released reset, and reached DONE");

    {
        // The FIXTURE line above proves only that DONE was written. K2a
        // reads DSKBYTR through peek_logical; this proves that path can
        // read a program-stored constant first (FRAME_CTR = 7, the S6
        // loop's own exit value).
        const Pop* c0 = nullptr;
        for (auto& p : all) if (p.name == "C0") c0 = &p;
        bool ok = c0 && !c0->run.peeks.empty() && c0->run.peeks[0] && *c0->run.peeks[0] == 7;
        checks.note("FIXTURE peek: C0's FRAME_CTR (peek_logical) = " +
                    (c0 && !c0->run.peeks.empty() && c0->run.peeks[0] ? hex(*c0->run.peeks[0]) : std::string("?")));
        checks.expect(ok, "FIXTURE: a program-stored constant (FRAME_CTR = 7) is read back through peek_logical "
                      "before K2a relies on the same path for DSKBYTR.");
    }

    std::vector<std::string> names_all;
    for (auto& p : all) names_all.push_back(p.name);

    bool p1 = fold_check(all, names_all, "R", true, checks, "P1");
    checks.expect(p1, "P1: every refresh access lies on an even in-line cycle, in every run and mode graded "
                  "(C0, C1a, C1b, C2a, C2b, C3, C4-1..6, C4-4w, C5, C6, C7a, C7b, S1, S2 in PAL/ECS; C0, C6 and S1 "
                  "also in NTSC/ECS, PAL/OCS and NTSC/OCS). ch2-about-the-copper.c24 COP2-about-the-copper.c29 p.20");

    // K2a/K2b (preconditions of P2) and P2 itself, with D9's own carve-out:
    // if K2b (>=500 disk-block accesses) holds for C2a and C6 but not C2b
    // (no drive attached), the read direction is declined, not graded red.
    {
        uint16_t va = 0, vb = 0, vc = 0;
        const Pop *c2a = nullptr, *c2b = nullptr, *c6 = nullptr;
        for (auto& p : all) { if (p.name == "C2a") c2a = &p; if (p.name == "C2b") c2b = &p; if (p.name == "C6") c6 = &p; }
        bool c2a_dskbytr = dskbytr_ok(c2a->run, true, &va);
        bool c2b_dskbytr = dskbytr_ok(c2b->run, false, &vb);
        bool c6_dskbytr = dskbytr_ok(c6->run, true, &vc);
        checks.note("K2a: DSKBYTR C2a=" + hex(va) + " C2b=" + hex(vb) + " C6=" + hex(vc));
        checks.expect(c2a_dskbytr && c2b_dskbytr && c6_dskbytr, "K2a: the stored DSKBYTR has DMAON=1 and "
                      "DISKWRITE=1 in C2a and C6, DISKWRITE=0 in C2b. p.248 Appendix A p.278");

        size_t pop_c2a = disk_population(c2a->run, blocks_disk());
        size_t pop_c2b = disk_population(c2b->run, blocks_disk());
        size_t pop_c6 = disk_population(c6->run, blocks_disk());
        bool k2b_c2a = pop_c2a >= 500, k2b_c2b = pop_c2b >= 500, k2b_c6 = pop_c6 >= 500;
        checks.note("K2b: disk-block population C2a=" + std::to_string(pop_c2a) + " C2b=" + std::to_string(pop_c2b) +
                    " C6=" + std::to_string(pop_c6));
        checks.expect(k2b_c2a && k2b_c6, "K2b: at least 500 disk-block accesses in the observed frames of C2a and C6.");
        if (!k2b_c2b)
            checks.record("K2b/D9: C2b (read direction, no drive attached) shows " + std::to_string(pop_c2b) +
                           " disk-block accesses in the observed frames (<500); the read direction is not exercised, "
                           "not graded. p.250");
        else
            checks.expect(k2b_c2b, "K2b: at least 500 disk-block accesses in the observed frames of C2b.");

        std::vector<std::string> p2_names = {"C2a", "C6"};
        if (k2b_c2b) p2_names.push_back("C2b");
        bool p2 = fold_check(all, p2_names, "Disk", true, checks, "P2");
        checks.expect(p2, "P2: every disk-DMA access lies on an even in-line cycle (" +
                      (k2b_c2b ? std::string("C2a, C2b, C6") : std::string("C2a, C6; C2b's read direction declined, D9")) +
                      ").");
        checks.record("ch2-about-the-copper.c23 COP2-about-the-copper.c28: only the write direction (C2a, C6) is "
                       "graded here; the disk-to-memory read direction (C2b) shows " + std::to_string(pop_c2b) +
                       " disk-block accesses in the observed frames and is declined (D9), so the ids' own \"only\" "
                       "is not established for both directions. p.20");
    }

    bool p3 = fold_check(all, {"C1a", "C1b", "C6"}, "Audio", true, checks, "P3");
    checks.expect(p3, "P3: every audio-DMA access, all four channels, lies on an even in-line cycle (C1a, C1b, "
                  "C6). ch2-about-the-copper.c22 COP2-about-the-copper.c27 p.20");

    // K3 (precondition of P3): each of the four channels has >=100 accesses
    // in the observed frames of C1a, C1b and C6.
    {
        bool k3_ok = true; std::ostringstream note;
        for (const char* cfgname : {"C1a", "C1b", "C6"}) {
            const Pop* p = nullptr;
            for (auto& c : all) if (c.name == cfgname) p = &c;
            note << cfgname << ":";
            for (int x = 0; x < 4; ++x) {
                size_t n = audio_channel_population(p->run, p->blocks, x);
                bool ok = n >= 100;
                k3_ok = k3_ok && ok;
                note << "ch" << x << "=" << n << (ok ? "" : "(FAIL)") << ",";
            }
            note << ";";
        }
        checks.note("K3 per-channel: " + note.str());
        checks.expect(k3_ok, "K3: each of the four audio channels has at least 100 accesses in the observed "
                      "frames of C1a, C1b and C6. p.144");
    }

    bool p4 = fold_check(all, {"C3", "C6", "S1", "S2"}, "Sprite", true, checks, "P4");
    checks.expect(p4, "P4: every sprite-DMA access, all eight channels, lies on an even in-line cycle (C3, C6, "
                  "S1, S2). ch2-about-the-copper.c25 COP2-about-the-copper.c30 p.20");

    // K4 (precondition of P4): in each observed frame of C3 and C6, every
    // word of each of the eight sprite structures is read at least once.
    {
        const Pop *c3 = nullptr, *c6k4 = nullptr;
        for (auto& p : all) { if (p.name == "C3") c3 = &p; if (p.name == "C6") c6k4 = &p; }
        bool k4 = k4_check(c3->run, c3->blocks, checks, "C3") && k4_check(c6k4->run, c6k4->blocks, checks, "C6");
        checks.expect(k4, "K4: in each observed frame of C3 and C6, every word of each of the eight sprite "
                      "structures (control, data and end words) is read at least once. p.108");
    }

    bool p6 = fold_check(all, names_all, "Copper", false, checks, "P6");
    checks.expect(p6, "P6: every Copper-block access lies on an odd in-line cycle, in every run and mode graded "
                  "(as P1). ch2-about-the-copper.c21 p.20");
    {
        // P6's own MEASURED line (the record the lead names wrong): the
        // even-s reads are counted, their s is shown, and each is shown to
        // repeat the previous read of the same word -- for every run,
        // not only the four that go red.
        std::ostringstream m;
        for (auto& p : all) {
            auto info = measure_p6_even_reads(p.run, p.blocks);
            if (info.count == 0) continue;
            m << p.name << ":count=" << info.count << ",all_s226=" << info.all_at_s226
              << ",all_repeat_prev=" << info.all_repeat_prev << ";";
        }
        checks.note("P6 even-s Copper-block reads (s, and each repeats the previous read of the same word): " + m.str());
    }

    // A1 (precondition of P1-P6): in the baseline configuration C0, every
    // access in the observed frames is in R, the CPU block, or the Copper
    // block. Graded with every layout block declared (not just Copper), so
    // the check is not vacuous by construction (defect 3).
    {
        const Pop* c0 = nullptr;
        for (auto& p : all) if (p.name == "C0") c0 = &p;
        auto pr = analyze_population(c0->run.log, blocks_full(4), {wordof(COP_MAIN)}, "R", true);
        checks.note("A1: C0 observed-span violations=" + std::to_string(pr.a1_violations));
        checks.expect(pr.restarts_ok && pr.a1_ok, "A1: in the baseline configuration C0, every access in the "
                      "observed frames is either in the refresh set R, the CPU block, or the Copper block. "
                      "SPECIFICATION.md 2.2");
    }

    // P5 and its own precondition K5 (C4-1..C4-6, C4-4w, C6).
    {
        bool p5a = true, p5b = true, k5 = true;
        std::vector<std::pair<std::string, int>> p5cfgs = {
            {"C4-1", 1}, {"C4-2", 2}, {"C4-3", 3}, {"C4-4", 4}, {"C4-5", 5}, {"C4-6", 6}, {"C4-4w", 4}, {"C6", 4}};
        for (auto& [cfgname, n] : p5cfgs) {
            const Pop* p = nullptr;
            for (auto& c : all) if (c.name == cfgname) p = &c;
            p5a = p5a_check(p->run, n, checks, cfgname) && p5a;
            p5b = p5b_check(p->run, n, checks, cfgname) && p5b;
            k5 = k5_check(p->run, n, checks, cfgname) && k5;
        }
        checks.expect(k5, "K5: every enabled plane has fetches in at least 150 lines of every observed frame "
                      "(C4-1..C4-6, C4-4w, C6).");
        checks.expect(p5a && p5b, "P5: (a) every access in bitplanes 1-4 lies on an even in-line cycle, in every "
                      "config with 1-6 low-resolution planes and at both fetch starts, and in C6; (b) in every "
                      "line with bitplane fetches, even-cycle fetches outnumber odd-cycle ones. "
                      "ch2-about-the-copper.c26 COP2-about-the-copper.c31 p.20");
    }

    // P7 and its own preconditions K6, T1, T2, T3 (C5 vs C6, C7a, C7b).
    // Reuses the runs already built into `all` above -- no re-simulation.
    {
        const Pop *c5p = nullptr, *c6p = nullptr, *c7ap = nullptr, *c7bp = nullptr;
        for (auto& p : all) {
            if (p.name == "C5") c5p = &p;
            if (p.name == "C6") c6p = &p;
            if (p.name == "C7a") c7ap = &p;
            if (p.name == "C7b") c7bp = &p;
        }
        const RunResult& c5 = c5p->run;
        const RunResult& c6 = c6p->run;

        bool k6 = k6_check(c5, c5p->blocks, checks, "C5") && k6_check(c6, c6p->blocks, checks, "C6") &&
                  k6_check(c7ap->run, c7ap->blocks, checks, "C7a") && k6_check(c7bp->run, c7bp->blocks, checks, "C7b");
        checks.expect(k6, "K6: in every observed frame of C5, C6, C7a and C7b, every word of L5 and L5b is read "
                      "at least once (the list ran to its end).");

        bool t1 = t1_check(c5, c5p->blocks, c6, c6p->blocks, checks);
        checks.expect(t1, "T1: every segment has the same number of Copper accesses in C5 and C6.");

        bool t2 = t2_check(c5, c5p->blocks, checks, "C5") && t2_check(c6, c6p->blocks, checks, "C6");
        checks.expect(t2, "T2: no CPU-block access lies in any line spanned by a segment, in C5 or C6.");

        bool t3 = t3_check(c6, c6p->blocks, checks);
        checks.expect(t3, "T3: in the lines of the segments in C6, bitplane fetches occur in every line; sprite "
                      "fetches occur in every line of segments 16-35 and 64-83 (VP $50-$63/$80-$93); audio and "
                      "disk accesses each occur in at least half of the lines.");

        bool p7 = p7_check(c5, c5p->blocks, c6, c6p->blocks, checks);
        checks.expect(p7, "P7: for every segment i (i=0..112) and every observed frame, the list of (line "
                      "offset, s) pairs is identical in C5 and C6 -- the channels on the even cycles never delay "
                      "the Copper. ch2-about-the-copper.c21 p.20");
    }

    // P8-P12 and the blitter claims: not built, by a scope decision (a
    // video-measurement apparatus this package does not have -- frame
    // boundaries from vs, an output-line-to-beam-line calibration with its
    // own self-test, a sprite-pixel-width self-test, and per-frame sprite-
    // image edge/colour extraction). Building them means adding that
    // apparatus and P8-P12's own propositions; S1/S2 are already built and
    // graded here for P4.
    checks.record("COP2-putting-together-a-copper-instruction-list.c1 .c2 .c3 .c4 .c5 and "
                   "ch2-putting-together-a-copper-instruction-list.c5 .c6 .c7 (P8-P12, sprite channel reuse): not "
                   "built, by a scope decision. Building them needs a video-measurement apparatus this package "
                   "does not have (frame boundaries from vs, an output-line-to-beam-line calibration with its own "
                   "self-test, a sprite-pixel-width self-test, per-frame sprite-image edge/colour extraction) and "
                   "P8-P12's own propositions.");
    checks.record("COP2-about-the-copper.c34 .c35 .c36 (the blitter DMA channel): not built, by a scope decision. "
                   "The parenthesis they come from names a use (animation, line drawing, polygon filling), not a "
                   "machine function this package's claims define an observable for; a check would need pp.184-186 "
                   "and pp.189-192, pages outside this package's own claims.");

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}

