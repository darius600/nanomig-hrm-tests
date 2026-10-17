// Test purpose: grade the memory-cycle parity of refresh, disk, audio, sprite and low-resolution
//   bitplane accesses at the Chip-memory port against HRM p.20, that the Copper's cycles do
//   not move when those channels run, and what one sprite DMA channel displays when it is
//   moved or reused several times in one field (HRM p.27) as the video output shows it.
// Test class: HRM_CONFORMANCE for P1-P5 and P7-P12; the preconditions F1-F3, F2-COUNT, A1,
//   K2a-K6, T1-T3, FRAMES, Q2-Q5, Q-ref, K9 and IMG4 are INTEGRATION_SMOKE checks that the
//   exercised path is live.
// Primary oracle: Amiga Hardware Reference Manual, 3rd edition, p.20, p.23, p.27, p.46, p.59,
//   p.98, pp.101-110, pp.113-117, pp.194-197, Appendix A p.275 and p.277 (see SPECIFICATION.md).
// Requirement IDs: ch2-about-the-copper.c22 .c24 .c25 .c26 and COP2-about-the-copper.c27 .c29
//   .c30 .c31 (graded); .c21 (P6, withdrawn) .c23 and COP2-about-the-copper.c28 are recorded,
//   not graded. COP2-putting-together-a-copper-instruction-list.c1 .c2 .c3 .c4 .c5 and
//   ch2-putting-together-a-copper-instruction-list.c5 .c6 .c7 (P8-P12) are graded.
// Non-goals: requests and collisions (the port shows accesses, and its accesses carry no
//   requester), P6, the blitter, the 68000's own cycles, bitplanes 5-6 and
//   high-resolution fetches, P8-P12 in the NTSC and OCS modes.
//
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
// NMH_MUTATE selects a mutated capture (a different program) in place of a
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
constexpr uint32_t CH_DISK_MOVED = 0x00C000;
constexpr uint32_t CH_BPL[6]    = {0x010000, 0x018000, 0x020000,
                                    0x028000, 0x030000, 0x038000};

// Every program's own code/vectors/stack/results (attribution table,
// SPECIFICATION.md 2.2): the identity range the harness maps 1:1
// ($080000-$1FFFFF, nmh::kChipIdentityLo/Hi). Every declared DMA block in
// this package is placed below $080000 but at or above $001000 (ch2dma.i),
// so this threshold separates "CPU block" from every DMA block without
// listing sub-ranges -- except the level-3 (VERTB) autovector word at
// byte $6C, which every program's own S0 step makes an ordinary,
// CPU-reachable part of chip RAM (CLEAR_OVL): S0 writes it once (its own
// INSTALL_VECT) and the CPU itself reads it on every VERTB exception, a
// CPU bus access below kCpuWordThreshold that the declared DMA blocks
// (all at $001000 and up) never reach. Refresh (D7) reads word 0 of the
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

// p.59, Table 3-9, PAL: DIWSTRT VSTART $2C and DIWSTOP VSTOP $2C (= $12C, the
// stop position's MSB is the complement of the next MSB). The C4 and C6
// programs write DIWSTRT $2C81 and DIWSTOP $2CC1, so the display window is
// kDiwVStop - kDiwVStart lines high.
constexpr uint32_t kDiwVStart = 0x2C, kDiwVStop = 0x12C;

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

// A per-observed-frame count (three frames), as the K preconditions grade it.
bool all_frames_nonempty(const std::array<size_t, 3>& n) { return n[0] > 0 && n[1] > 0 && n[2] > 0; }

std::string frame_counts_text(const std::array<size_t, 3>& n) {
    return std::to_string(n[0]) + "/" + std::to_string(n[1]) + "/" + std::to_string(n[2]);
}

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
// `list_starts` (word addresses). Frame k = [restart[k-1], restart[k])
// (0-indexed restart[0] is restart #1).
//
// A restart fetch is identified by address and list sequence alone: the Copper's read of
// the word at one of `list_starts`. No run of this package has another reader of that
// word, so no parity enters the identification. A bus cycle held over two samples is one
// fetch (its first sample).
std::vector<uint64_t> restart_fetches(const std::vector<nmh::RamAccess>& log, const std::vector<uint32_t>& list_starts) {
    std::vector<uint64_t> out;
    const std::vector<bool> held = nmh::held_continuations(log, gS);
    for (size_t i = 0; i < log.size(); ++i) {
        const auto& a = log[i];
        if (a.is_write || held[i]) continue;
        bool match = false;
        for (auto s : list_starts) if (a.word_addr == s) { match = true; break; }
        if (!match) continue;
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

// ---- generic per-population parity check (P1-P5) -----------------------

// The four logical refresh slots of a line and where Fig. 6-9 (p.195) draws
// them, in memory cycles from the line's first refresh slot (the cycle the
// figure's footnote calls cycle 0): the figure draws the four hatched
// "MEMORY REFRESH" slots on every second slot, at 0, 2, 4 and 6. p.194:
// "4 cycles for memory refresh".
constexpr int kRefreshSlots = 4;
constexpr int kRefreshSlotOffset[kRefreshSlots] = {0, 2, 4, 6};

// F2's comparator: the first four accesses of a sorted refresh group lie at
// the offsets Fig. 6-9 draws, in units of S.
bool refresh_slots_ok(const std::vector<uint64_t>& group, uint64_t S) {
    if (group.size() < (size_t)kRefreshSlots) return false;
    for (int k = 0; k < kRefreshSlots; ++k)
        if (group[(size_t)k] - group[0] != (uint64_t)kRefreshSlotOffset[k] * S) return false;
    return true;
}

// F2-COUNT's comparator: a sorted refresh group holds exactly the four entries
// p.194 allocates ("4 cycles for memory refresh"), no more and no fewer.
bool refresh_count_ok(const std::vector<uint64_t>& group) {
    return group.size() == (size_t)kRefreshSlots;
}

struct ParityResult {
    bool f1_ok = false, f2_ok = false, f2count_ok = false, f3_ok = false, a1_ok = false, restarts_ok = false;
    bool all_even = true, all_odd = true;
    size_t population = 0;
    size_t a1_violations = 0;
    size_t groups_checked = 0;       // refresh groups whose first access lies in the observed frames
    std::map<std::string, size_t> slot_patterns; // "0,2,4,6" -> number of groups
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

// The refresh set R grouped as section 2.3 describes: sorted by time, a new
// group at every access more than 16*S after the previous one. Each group is
// its sorted cycle list.
std::vector<std::vector<uint64_t>> refresh_groups(const std::vector<nmh::RamAccess>& log,
                                                  const std::vector<Block>& blocks) {
    std::vector<uint64_t> r_cycles;
    for (const auto& a : log) if (!a.is_write && attribute(a.word_addr, blocks) == "R") r_cycles.push_back(a.cycle);
    std::sort(r_cycles.begin(), r_cycles.end());
    std::vector<std::vector<uint64_t>> groups;
    for (size_t i = 0; i < r_cycles.size(); ++i) {
        if (i == 0 || r_cycles[i] - r_cycles[i - 1] > 16 * gS) groups.emplace_back();
        groups.back().push_back(r_cycles[i]);
    }
    return groups;
}

// Evaluates every access of `target_block` within the observed frames:
// F1/F2/F2-COUNT/F3 (on the refresh-derived line grid), A1 (only meaningful
// for the baseline run, but harmless elsewhere), and the even/odd population.
ParityResult analyze_population(const std::vector<nmh::RamAccess>& log, const std::vector<Block>& blocks,
                                 const std::vector<uint32_t>& list_starts, const std::string& target_block,
                                 bool check_a1 = false) {
    ParityResult r;
    LineRef lref = make_lines(log, blocks);
    auto restarts = restart_fetches(log, list_starts);
    auto span = observed_span(restarts);
    r.restarts_ok = span.ok;
    if (!span.ok) return r;

    r.f1_ok = check_f1(log, blocks, span.lo, span.hi);

    auto groups = refresh_groups(log, blocks);
    bool f2 = true, f2c = true, f3 = true;
    for (size_t i = 0; i < groups.size(); ++i) {
        if (groups[i][0] < span.lo || groups[i][0] >= span.hi) continue;
        ++r.groups_checked;
        // F2: the group's first four accesses are the four refresh slots, at
        // the offsets Fig. 6-9 draws from the first one.
        std::string pattern;
        for (size_t k = 0; k < groups[i].size() && k < (size_t)kRefreshSlots; ++k) {
            uint64_t d = groups[i][k] - groups[i][0];
            pattern += (k ? "," : "") + std::to_string(d / gS) + (d % gS ? "+" : "");
        }
        pattern += " of " + std::to_string(groups[i].size());
        ++r.slot_patterns[pattern];
        if (!refresh_slots_ok(groups[i], gS)) f2 = false;
        if (!refresh_count_ok(groups[i])) f2c = false;
        if (i + 1 < groups.size()) {
            uint64_t gap = groups[i + 1][0] - groups[i][0];
            if (gap != 227 * gS && gap != 228 * gS) f3 = false;
        }
    }
    r.f2_ok = f2 && r.groups_checked > 0;
    r.f2count_ok = f2c && r.groups_checked > 0;
    r.f3_ok = f3;

    if (check_a1) {
        for (const auto& a : log) {
            if (a.cycle < span.lo || a.cycle >= span.hi) continue;
            std::string cls = attribute(a.word_addr, blocks);
            if (cls != "R" && cls != "CPU" && cls != "Copper") ++r.a1_violations;
        }
        r.a1_ok = r.a1_violations == 0;
    } else r.a1_ok = true;

    for (size_t i = 0; i < log.size(); ++i) {
        const auto& a = log[i];
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

// One run together with how it is read: its declared blocks, the word addresses
// of its list starts.
struct Pop {
    std::string name; RunResult run; std::vector<Block> blocks; std::vector<uint32_t> starts;
};

using Poker =std::function<void(nmh::NanomigHarness&)>;

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
Block bitplane_block(int y) { return {"BPL" + std::to_string(y + 1), wordof(CH_BPL[y]), wordof(CH_BPL[y] + 0x8000)}; }

std::vector<Block> blocks_C0() { return {copper_block()}; }

std::vector<Block> blocks_audio() {
    return {copper_block(), audio_block(0), audio_block(1), audio_block(2), audio_block(3)};
}
std::vector<Block> blocks_disk(uint32_t lo = CH_DISK, uint32_t words = 8000) {
    return {copper_block(), disk_block(lo, words)};
}
// The K2b contrast moves the disk transfer to CH_DISK_MOVED: that run declares the
// moved transfer as a block of its own, so its accesses do not join the refresh set.
std::vector<Block> blocks_disk_moved() {
    std::vector<Block> b = blocks_disk();
    b.push_back({"DiskMoved", wordof(CH_DISK_MOVED), wordof(CH_DISK_MOVED) + 8000});
    return b;
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
RunResult run_C0htotal(const std::string& dir) { return run_config(dir + "/prog_c0_htotal.bin", nmh::Config{}, nopoke); }
RunResult run_C0_nofinish(const std::string& dir) { return run_config(dir + "/prog_c0_nofinish.bin", nmh::Config{}, nopoke); }

RunResult run_C1a(const std::string& dir, bool no_aud0 = false) {
    return run_config(dir + (no_aud0 ? "/prog_c1a_noaud0.bin" : "/prog_c1a.bin"), nmh::Config{}, nopoke);
}
RunResult run_C1b(const std::string& dir) { return run_config(dir + "/prog_c1b.bin", nmh::Config{}, nopoke); }

RunResult run_C2a(const std::string& dir) {
    return run_config(dir + "/prog_c2a.bin", nmh::Config{}, nopoke, {RES_DSKBYTR});
}
RunResult run_C2b(const std::string& dir) {
    return run_config(dir + "/prog_c2b.bin", nmh::Config{}, nopoke, {RES_DSKBYTR});
}
RunResult run_C2a_earlyread(const std::string& dir) {
    return run_config(dir + "/prog_c2a_earlyread.bin", nmh::Config{}, nopoke, {RES_DSKBYTR});
}
RunResult run_C2a_dskpt(const std::string& dir) {
    return run_config(dir + "/prog_c2a_dskpt.bin", nmh::Config{}, nopoke, {RES_DSKBYTR});
}

RunResult run_C3(const std::string& dir, bool no_spren = false) {
    return run_config(dir + (no_spren ? "/prog_c3_nospren.bin" : "/prog_c3.bin"), nmh::Config{}, nopoke);
}

RunResult run_C4(const std::string& dir, int n, bool wide_start = false, bool no_bplen = false) {
    std::string bin = "/prog_c4_" + std::to_string(n) + ".bin";
    if (n == 4 && wide_start) bin = "/prog_c4_4w.bin";
    if (n == 4 && no_bplen) bin = "/prog_c4_4_nobplen.bin";
    return run_config(dir + bin, nmh::Config{}, nopoke);
}

RunResult run_C5(const std::string& dir) { return run_config(dir + "/prog_c5.bin", nmh::Config{}, nopoke, {RES_DSKBYTR}); }
RunResult run_C6(const std::string& dir, const char* bin = "/prog_c6.bin", nmh::Config cfg = nmh::Config{}) {
    return run_config(dir + bin, cfg, nopoke, {RES_DSKBYTR});
}
RunResult run_C6prime(const std::string& dir) { return run_config(dir + "/prog_c6_prime.bin", nmh::Config{}, nopoke); }
RunResult run_C6_skip50(const std::string& dir) { return run_config(dir + "/prog_c6_skip50.bin", nmh::Config{}, nopoke); }
RunResult run_C6_endtrailer(const std::string& dir) { return run_config(dir + "/prog_c6_endtrailer.bin", nmh::Config{}, nopoke); }
RunResult run_C6_nofinish(const std::string& dir) { return run_config(dir + "/prog_c6_nofinish.bin", nmh::Config{}, nopoke, {RES_DSKBYTR}); }
RunResult run_C6_norestart(const std::string& dir) { return run_config(dir + "/prog_c6_norestart.bin", nmh::Config{}, nopoke, {RES_DSKBYTR}); }
RunResult run_C6_htotal(const std::string& dir) { return run_config(dir + "/prog_c6_htotal.bin", nmh::Config{}, nopoke, {RES_DSKBYTR}); }

RunResult run_C7a(const std::string& dir) { return run_config(dir + "/prog_c7a.bin", nmh::Config{}, nopoke); }
RunResult run_C7b(const std::string& dir) { return run_config(dir + "/prog_c7b.bin", nmh::Config{}, nopoke); }

RunResult run_S1(const std::string& dir, nmh::Config cfg = nmh::Config{}) {
    return run_config(dir + "/prog_s1.bin", cfg, nopoke);
}
RunResult run_S2(const std::string& dir) { return run_config(dir + "/prog_s2.bin", nmh::Config{}, nopoke); }

// ---- precondition machinery ----------------------------------------------
//
// Every precondition is a comparator that follows the declarations of the run it
// grades: that run's blocks, its list starts and its restart identification
// (Pop). The same comparator grades the normal runs and the contrast runs; a
// contrast that breaks the precondition of its own line leaves that line
// NOT GRADED, never FAIL.

struct WordRange { uint32_t lo, hi; }; // word addresses, half-open

bool in_ranges(uint32_t w, const std::vector<WordRange>& rs) {
    for (const auto& r : rs) if (w >= r.lo && w < r.hi) return true;
    return false;
}

// The words the Copper's lists occupy wherever a run has them (L5 at COP_MAIN:
// 972 words; L5b at COP_SECOND: 12 words), with room for the word the Copper
// reads after each END. A precondition that must show a channel's own activity
// in a run where the channel and the Copper read the same words excludes them.
std::vector<WordRange> copper_list_words() {
    return {{wordof(COP_MAIN), wordof(COP_MAIN) + 1024}, {wordof(COP_SECOND), wordof(COP_SECOND) + 32}};
}

std::string unmet_if(bool ok, const std::string& name) { return ok ? std::string() : name + " "; }

// The observation preconditions of one run: it reached DONE (C1), its restart
// fetches delimit the observed frames, and F1, F2, F2-COUNT and F3 hold. P1 alone
// leaves F2 and F2-COUNT out (`with_f2` false): they grade the very accesses P1
// grades, so a gate on them would make P1's own clause unable to go red.
std::string observation_unmet(const std::string& name, const RunResult& run, const ParityResult& r,
                              bool with_f2 = true) {
    std::string u = unmet_if(run.done.has_value(), name + ":C1");
    if (!r.restarts_ok) return u + name + ":restarts ";
    u += unmet_if(r.f1_ok, name + ":F1");
    if (with_f2) u += unmet_if(r.f2_ok, name + ":F2") + unmet_if(r.f2count_ok, name + ":F2-COUNT");
    return u + unmet_if(r.f3_ok, name + ":F3");
}

// The observation preconditions of a run that is read as its Pop declares it.
std::string run_unmet(const Pop& p, bool with_f2 = true) {
    auto pr = analyze_population(p.run.log, p.blocks, p.starts, "R", false);
    return observation_unmet(p.name, p.run, pr, with_f2);
}

// The same, with the run's observation printed (a mutation run prints the
// preconditions of the line under test before the line).
std::string observe(Checks& checks, const Pop& p, bool with_f2 = true) {
    auto pr = analyze_population(p.run.log, p.blocks, p.starts, "R", false);
    std::string u = observation_unmet(p.name, p.run, pr, with_f2);
    checks.note("observation " + p.name + ": done=" + std::to_string(p.run.done.has_value()) +
                " restarts=" + std::to_string(restart_fetches(p.run.log, p.starts).size()) +
                (pr.restarts_ok
                     ? " F1=" + std::to_string(pr.f1_ok) + " F2=" + std::to_string(pr.f2_ok) +
                           " F2-COUNT=" + std::to_string(pr.f2count_ok) + " F3=" + std::to_string(pr.f3_ok)
                     : std::string(" F1, F2, F2-COUNT and F3 not evaluated (no observed frames)")) +
                (u.empty() ? " (all met)" : " unmet: " + u));
    return u;
}

void not_graded(Checks& checks, const std::string& id, const std::string& unmet) {
    checks.record("NOT GRADED " + id + ": precondition(s) not met: " + unmet);
}

std::vector<std::string> plane_names(int n) {
    std::vector<std::string> v;
    for (int y = 0; y < n; ++y) v.push_back("BPL" + std::to_string(y + 1));
    return v;
}

// P5(a): every access in plane blocks 1-4 of the run (only, regardless of how
// many planes the run has) has even s. The run's observation preconditions that
// are not met are appended to `unmet`.
bool p5a_check(const Pop& p, int n, Checks& checks, std::string& unmet) {
    bool ok = true; std::ostringstream note;
    int upto = std::min(n, 4);
    for (int y = 0; y < upto; ++y) {
        std::string name = "BPL" + std::to_string(y + 1);
        auto pr = analyze_population(p.run.log, p.blocks, p.starts, name, false);
        if (y == 0) unmet += observation_unmet(p.name, p.run, pr);
        bool this_ok = pr.population > 0 && pr.all_even;
        ok = ok && this_ok;
        note << name << ":pop=" << pr.population << ",even=" << pr.all_even << (this_ok ? "" : "(FAIL)") << ";";
    }
    checks.note("P5a " + p.name + ": " + note.str());
    return ok;
}

// P5(b): in every line with any bitplane fetch (planes 1..n), even-cycle
// fetches outnumber odd-cycle ones ("most").
bool p5b_check(const Pop& p, Checks& checks) {
    LineRef lref = make_lines(p.run.log, p.blocks);
    auto restarts = restart_fetches(p.run.log, p.starts);
    auto span = observed_span(restarts);
    if (!span.ok) { checks.note("P5b " + p.name + ": NOSPAN"); return false; }
    std::map<int64_t, std::pair<int, int>> counts; // line -> (even,odd)
    for (const auto& a : p.run.log) {
        if (a.is_write || a.cycle < span.lo || a.cycle >= span.hi) continue;
        std::string cls = attribute(a.word_addr, p.blocks);
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
    checks.note("P5b " + p.name + ": lines=" + std::to_string(counts.size()) + " bad_lines=" + std::to_string(bad));
    return ok;
}

// K5: every enabled plane (`planes`, the names of the run's declared plane
// blocks) has fetches in at least as many lines of every observed frame as the
// display window is high (p.59 Table 3-9, PAL: kDiwVStop - kDiwVStart = 256
// lines). The premise that every line of the window is fetched is Appendix A
// p.277: "The vertical bitplane DMA timing is identical to the display windows
// described above." Checked per frame, not merged across the whole observed
// span.
bool k5_check(const Pop& p, const std::vector<std::string>& planes, Checks& checks) {
    LineRef lref = make_lines(p.run.log, p.blocks);
    auto restarts = restart_fetches(p.run.log, p.starts);
    if (restarts.size() < 6) { checks.note("K5 " + p.name + ": NOSPAN"); return false; }
    bool ok = true; std::ostringstream note;
    for (int f = 0; f < 3; ++f) {
        uint64_t flo = restarts[2 + f], fhi = restarts[3 + f];
        for (const auto& name : planes) {
            std::set<int64_t> lines;
            for (const auto& a : p.run.log) {
                if (a.is_write || a.cycle < flo || a.cycle >= fhi) continue;
                if (attribute(a.word_addr, p.blocks) != name) continue;
                int64_t li = line_of(lref, a.cycle);
                if (li >= 0) lines.insert(li);
            }
            bool this_ok = lines.size() >= kDiwVStop - kDiwVStart;
            ok = ok && this_ok;
            note << "f" << f << name << "=" << lines.size() << (this_ok ? "" : "(FAIL)") << ";";
        }
    }
    checks.note("K5 " + p.name + ": " + note.str());
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

struct Structure { std::string name; uint32_t base_word; int n_words; };

// The eight sprite structures of C3 and C6 (86 words each: 2 control + 40
// data + 2 control + 40 data + 2 end).
std::vector<Structure> sprite_structures_c3() {
    std::vector<Structure> s;
    for (int x = 0; x < 8; ++x) s.push_back({"spr" + std::to_string(x), wordof(CH_SPR[x]), 86});
    return s;
}

// K4: in each observed frame, every word of each structure of the run is read
// at least once. The Pop carries the run's own declared blocks and restart
// identification (2.4).
bool k4_check(const Pop& p, const std::vector<Structure>& structures, Checks& checks) {
    LineRef lref = make_lines(p.run.log, p.blocks);
    auto restarts = restart_fetches(p.run.log, p.starts);
    if (restarts.size() < 6) { checks.note("K4 " + p.name + ": NOSPAN"); return false; }
    bool ok = true; std::ostringstream note;
    for (int f = 0; f < 3; ++f) {
        uint64_t flo = restarts[2 + f], fhi = restarts[3 + f];
        for (const auto& st : structures) {
            size_t cnt = 0;
            bool full = words_fully_read(p.run.log, flo, fhi, st.base_word, st.n_words, &cnt);
            ok = ok && full;
            note << "f" << f << st.name << "=" << cnt << "/" << st.n_words << (full ? "" : "(FAIL)") << ";";
        }
    }
    checks.note("K4 " + p.name + ": " + note.str());
    return ok;
}

// K6: in each observed frame, every word of L5 (972 words from COP_MAIN) and
// of L5b (12 words from COP_SECOND) is read at least once (the list ran to its
// end).
bool k6_check(const Pop& p, Checks& checks) {
    LineRef lref = make_lines(p.run.log, p.blocks);
    auto restarts = restart_fetches(p.run.log, p.starts);
    if (restarts.size() < 6) { checks.note("K6 " + p.name + ": NOSPAN"); return false; }
    bool ok = true; std::ostringstream note;
    for (int f = 0; f < 3; ++f) {
        uint64_t flo = restarts[2 + f], fhi = restarts[3 + f];
        size_t c5, c5b;
        bool full5 = words_fully_read(p.run.log, flo, fhi, wordof(COP_MAIN), 972, &c5);
        bool full5b = words_fully_read(p.run.log, flo, fhi, wordof(COP_SECOND), 12, &c5b);
        bool this_ok = full5 && full5b;
        ok = ok && this_ok;
        note << "f" << f << ":L5=" << c5 << "/972,L5b=" << c5b << "/12" << (this_ok ? "" : "(FAIL)") << ";";
    }
    checks.note("K6 " + p.name + ": " + note.str());
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
// [60+8i+2, 60+8(i+1)+2).
constexpr uint32_t kSegBase = 62; // 60+2: first word after WAIT 0
uint32_t seg_lo_word(uint32_t cop_main_word, int i) { return cop_main_word + kSegBase + 8u * (uint32_t)i; }
uint32_t seg_hi_word(uint32_t cop_main_word, int i) { return cop_main_word + kSegBase + 8u * (uint32_t)(i + 1); }

using PairList = std::vector<std::pair<int64_t, int64_t>>; // (line offset from segment's first access, s)

// [frame 0..2][segment 0..112] -> PairList, time-ordered (run.log is
// chronological). Only valid for a run using the list_L5 layout (C5, C6,
// C7a, C7b, and C6' -- SELFTEST-P7's run, which only substitutes one WAIT's
// own HP word and does not shift any segment).
// `blocks` must be the run's OWN full declared-block list (e.g.
// blocks_full(4) for C6): the refresh set is identified by exclusion
// (SPECIFICATION.md D7/2.2), so a run's other live DMA traffic must be
// declared here or it is misattributed "R" and corrupts the line/refresh-
// group construction that segments and lines are anchored on.
std::vector<std::vector<PairList>> compute_segments(const RunResult& run, const std::vector<Block>& blocks) {
    std::vector<std::vector<PairList>> out(3, std::vector<PairList>(113));
    LineRef lref = make_lines(run.log, blocks);
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)});
    if (restarts.size() < 6) return out;
    uint32_t base = wordof(COP_MAIN);
    std::array<uint32_t, 114> bounds;
    for (int i = 0; i <= 113; ++i) bounds[(size_t)i] = seg_lo_word(base, i);
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

// T1: every segment has the same number of Copper accesses in both runs, and
// every segment of C5 has at least one (an empty population fails its line,
// SPECIFICATION.md 2.11). `name` is the second run's name, for the printed line.
bool t1_check(const RunResult& c5, const std::vector<Block>& b5, const RunResult& c6, const std::vector<Block>& b6,
              const std::string& name, Checks& checks) {
    auto s5 = compute_segments(c5, b5), s6 = compute_segments(c6, b6);
    int mismatched = 0, empty_in_c5 = 0;
    std::ostringstream which; // frame, segment and the two access counts of the first differing pairs
    for (int f = 0; f < 3; ++f)
        for (int i = 0; i < 113; ++i) {
            if (s5[f][i].size() != s6[f][i].size()) {
                if (mismatched < 12) which << " f" << f << "s" << i << ":" << s5[f][i].size() << "/" << s6[f][i].size();
                ++mismatched;
            }
            if (s5[f][i].empty()) ++empty_in_c5;
        }
    checks.note("T1: segments with differing access count (C5 vs " + name + ") = " + std::to_string(mismatched) +
                (mismatched ? " (frame, segment, count in C5/count in " + name + "):" + which.str() : std::string()) +
                ", segments empty in C5 = " + std::to_string(empty_in_c5));
    return mismatched == 0 && empty_in_c5 == 0;
}

// T2: no CPU-block access lies in any line spanned by a segment, and some line
// is spanned by a segment (an empty population fails its line).
bool t2_check(const RunResult& run, const std::vector<Block>& blocks, Checks& checks, const std::string& name) {
    LineRef lref = make_lines(run.log, blocks);
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)});
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
    return !segment_lines.empty() && violations == 0;
}

// T3: in the lines of the segments in C6, bitplane fetches occur in every
// line; sprite fetches occur in every line of segments i=16..35 and
// 64..83 (VP $50-$63 and $80-$93, matching VP=$40+i by list_L5's own
// construction); audio and disk accesses each occur in the segment lines
// of every observed frame (the channels are active while the segments run).
bool t3_check(const RunResult& run, const std::vector<Block>& blocks, Checks& checks) {
    LineRef lref = make_lines(run.log, blocks);
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)});
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
    int bpl_missing = 0, spr_missing = 0, total_lines = 0;
    std::array<size_t, 3> aud_f = {0, 0, 0}, dsk_f = {0, 0, 0}; // segment lines per frame with audio / disk
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
            if (aud) ++aud_f[(size_t)f];
            if (dsk) ++dsk_f[(size_t)f];
        }
    }
    bool ok = total_lines > 0 && bpl_missing == 0 && spr_missing == 0 &&
              all_frames_nonempty(aud_f) && all_frames_nonempty(dsk_f);
    checks.note("T3: total_lines=" + std::to_string(total_lines) + " bpl_missing=" + std::to_string(bpl_missing) +
                " spr_missing(in-range)=" + std::to_string(spr_missing) + " aud_lines(per frame)=" + frame_counts_text(aud_f) +
                " dsk_lines(per frame)=" + frame_counts_text(dsk_f));
    return ok;
}

// ---- folding several configs' runs into one population check -----------

struct Fold {
    bool ok = false;    // every run's population is non-empty and every access lies on the expected parity
    std::string unmet;  // observation preconditions not met in some run (empty when all are met)
};

// The parity verdict of the target population over the named runs. The runs'
// observation preconditions (C1, restarts, F1, F2, F2-COUNT, F3; P1 without
// F2 and F2-COUNT) are reported in `unmet`, never folded into `ok`; the caller
// grades the line only when `unmet` is empty, and prints NOT GRADED otherwise.
Fold fold_check(const std::vector<Pop>& all, const std::vector<std::string>& names, const std::string& target,
                bool want_even, Checks& checks, const std::string& tag, bool with_f2 = true) {
    Fold out;
    out.ok = true;
    std::ostringstream note;
    for (const auto& n : names) {
        const Pop* p = nullptr;
        for (const auto& c : all) if (c.name == n) { p = &c; break; }
        if (!p) { out.ok = false; out.unmet += n + ":MISSING "; note << n << ":MISSING;"; continue; }
        auto r = analyze_population(p->run.log, p->blocks, p->starts, target, false);
        out.unmet += observation_unmet(p->name, p->run, r, with_f2);
        bool this_ok = r.population > 0 && (want_even ? r.all_even : r.all_odd);
        out.ok = out.ok && this_ok;
        note << n << ":pop=" << r.population << (want_even ? ",even=" : ",odd=")
             << (want_even ? r.all_even : r.all_odd);
        note << ",F1F2F3=" << r.f1_ok << r.f2_ok << r.f3_ok;
        note << (this_ok ? "" : "(FAIL)") << ";";
    }
    checks.note(tag + " per-config: " + note.str());
    return out;
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

// The number of accesses to the words in `in`, less those in `excluded`, in
// each of the three observed frames (frame f = [restart f+3, restart f+4),
// section 2.4) of the run, whose restarts are identified as the run declares.
// Reads only unless `with_writes`. All zero when the run has too few restart
// fetches to delimit the frames.
std::array<size_t, 3> frame_access_counts(const Pop& p, const WordRange& in, const std::vector<WordRange>& excluded,
                                           bool with_writes) {
    std::array<size_t, 3> n = {0, 0, 0};
    LineRef lref = make_lines(p.run.log, p.blocks);
    auto restarts = restart_fetches(p.run.log, p.starts);
    if (restarts.size() < 6) return n;
    for (const auto& a : p.run.log) {
        if ((a.is_write && !with_writes) || a.word_addr < in.lo || a.word_addr >= in.hi) continue;
        if (in_ranges(a.word_addr, excluded)) continue;
        for (int f = 0; f < 3; ++f)
            if (a.cycle >= restarts[2 + f] && a.cycle < restarts[3 + f]) { ++n[(size_t)f]; break; }
    }
    return n;
}

// K2b's own comparator (SPECIFICATION.md P2.5): the disk block receives
// accesses, reads or writes, in each observed frame. The words of the
// Copper's lists do not count: the Copper reads them whether or not the disk
// channel runs.
WordRange plain_disk_range() { return {wordof(CH_DISK), wordof(CH_DISK) + 8000}; }
std::array<size_t, 3> disk_frame_counts(const Pop& p, const WordRange& disk) {
    return frame_access_counts(p, disk, copper_list_words(), true);
}

// K3's own comparator (SPECIFICATION.md P3.5): each audio channel's own reads
// in each observed frame, not counting the words of the Copper's lists.
std::vector<WordRange> plain_audio_ranges() {
    std::vector<WordRange> r;
    for (int x = 0; x < 4; ++x) r.push_back({wordof(CH_AUD[x]), wordof(CH_AUD[x] + 0x200)});
    return r;
}
bool k3_check(const Pop& p, const std::vector<WordRange>& channels, Checks& checks) {
    bool ok = true; std::ostringstream note;
    for (size_t x = 0; x < channels.size(); ++x) {
        auto n = frame_access_counts(p, channels[x], copper_list_words(), false);
        bool this_ok = all_frames_nonempty(n);
        ok = ok && this_ok;
        note << "ch" << x << "=" << frame_counts_text(n) << (this_ok ? "" : "(FAIL)") << ",";
    }
    checks.note("K3 " + p.name + " per-channel accesses per observed frame: " + note.str());
    return ok;
}

// A1 (precondition of P1-P5): in the baseline configuration C0, every access in
// the observed frames is in R, the CPU block or the Copper block. Every
// reserved block that is not the refresh set is declared (blocks_full(6):
// Copper, four audio, disk, sprites and the null sprite, six planes), so an
// access in any of them is counted as a violation.
bool a1_holds(const Pop& c0, Checks& checks) {
    auto pr = analyze_population(c0.run.log, blocks_full(6), {wordof(COP_MAIN)}, "R", true);
    checks.note("A1: C0 observed-span violations=" + std::to_string(pr.a1_violations));
    return pr.restarts_ok && pr.a1_ok;
}

// The MEASURED line of the withdrawn P6: the Copper-block reads in the observed frames that fall
// on an even in-line cycle, split into those that continue the bus cycle of the
// sample directly before them (same word, direction, data and byte enables,
// exactly one memory cycle gS earlier) and
// those that do not, with the in-line cycle numbers they fall on.
struct EvenReadInfo { size_t raw_even = 0, held_even = 0, remaining_even = 0; std::set<int64_t> s_values; };
EvenReadInfo measure_p6_even_reads(const RunResult& run, const std::vector<Block>& blocks) {
    EvenReadInfo info;
    LineRef lref = make_lines(run.log, blocks);
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)});
    auto span = observed_span(restarts);
    if (!span.ok) return info;
    const std::vector<bool> held = nmh::held_continuations(run.log, gS);
    for (size_t i = 0; i < run.log.size(); ++i) {
        const auto& a = run.log[i];
        if (a.cycle < span.lo || a.cycle >= span.hi) continue;
        if (attribute(a.word_addr, blocks) != "Copper") continue;
        int64_t li = line_of(lref, a.cycle);
        if (li < 0) continue;
        int64_t s = in_line_cycle(lref, li, a.cycle);
        if (s % 2 != 0) continue;
        ++info.raw_even;
        info.s_values.insert(s);
        if (held[i]) ++info.held_even; else ++info.remaining_even;
    }
    return info;
}

// The Copper's first fetch after S4: the first read of a Copper-block word in
// the run (before S4 no Copper DMA runs and the CPU only writes the lists).
// Its in-line cycle number is read only from a regular line: the line from one
// refresh group that has the four slots of Fig. 6-9 (F2's test) to the next such
// group, 227*S or 228*S later. A read that other, stray refresh-set entries cut
// out of a regular line (the plain construction of section 2.3 would start a
// line of its own at the first of them) is placed in the regular line that holds
// it; a fetch that no regular line brackets is dropped, with the length of the
// plain line that holds it. Also whether it continues the sample before it.
std::string copper_first_fetch_text(const Pop& p) {
    LineRef lref = make_lines(p.run.log, p.blocks);
    const std::vector<bool> held = nmh::held_continuations(p.run.log, gS);
    std::vector<uint64_t> anchors;
    for (const auto& g : refresh_groups(p.run.log, p.blocks))
        if (refresh_slots_ok(g, gS)) anchors.push_back(g[0]);
    for (size_t i = 0; i < p.run.log.size(); ++i) {
        const auto& a = p.run.log[i];
        if (a.is_write || attribute(a.word_addr, p.blocks) != "Copper") continue;
        std::string head = p.name + ": word=0x" + hex(a.word_addr);
        std::string plain;
        int64_t li = line_of(lref, a.cycle);
        if (li >= 0 && (size_t)li + 1 < lref.firsts.size())
            plain = "; the plain line holding it is " + std::to_string((lref.firsts[(size_t)li + 1] - lref.firsts[(size_t)li]) / gS) +
                    " memory cycles long";
        auto it = std::upper_bound(anchors.begin(), anchors.end(), a.cycle);
        if (it == anchors.begin() || it == anchors.end())
            return head + " dropped: no regular line brackets it" + plain;
        const uint64_t lo = *(it - 1), hi = *it;
        if (hi - lo != 227 * gS && hi - lo != 228 * gS)
            return head + " dropped: the regular lines around it are " + std::to_string((hi - lo) / gS) +
                   " memory cycles apart, not 227 or 228" + plain;
        int64_t s = (int64_t)((a.cycle - lo) / gS);
        return head + " regular line of " + std::to_string((hi - lo) / gS) + " memory cycles, s=" + std::to_string(s) +
               (s % 2 ? " (odd)" : " (even)") + (held[i] ? " held" : " not held") + plain;
    }
    return p.name + ": no Copper-block read";
}

// NMH_MUTATE=<line>-NOFINISH, <line>-HTOTAL or <line>-NORESTART for the lines whose gate is the
// observation preconditions of the runs they grade (SPECIFICATION.md 2.11).
bool gate_selector(const std::string& mv) {
    for (const char* line : {"K2b", "K3", "K4", "K5", "K6", "T1", "T2", "T3"})
        for (const char* suffix : {"-NOFINISH", "-HTOTAL", "-NORESTART"})
            if (mv == std::string(line) + suffix) return true;
    return false;
}

// ---- video apparatus for P8-P12 (SPECIFICATION.md 2.5) ---------------------
//
// The harness samples the video pins once per system-clock cycle and the RAM port at the same
// clock, so a video sample's index is the time base of RamAccess::cycle. Everything below reads
// the captured video samples and the Chip-memory log of a run; nothing reads an internal signal.
//
// An output frame is the span from one rising edge of vs to the next. Its output lines are the
// spans between rising edges of hs inside it; the piece before the first rising edge is not a line
// and is not counted, so output line 0 is the first line that begins at an hs edge. An image is a
// maximal run of consecutive output lines that hold at least one non-background sample
// (COLOR00 is $000 in every sprite run, so a non-zero sample belongs to a sprite). Its left edge
// is the first non-background sample of its first line, W the length of the run of non-background
// samples from there, and p = W/16 the samples per sprite pixel (Q2). Heights and tops are
// counted in output lines and divided by k_v, the output lines per beam line, which is the
// ratio of the beam-line period at the RAM port (the interval between refresh groups) to the
// period of hs in the video (Q3).

struct VRun {
    RunResult run;
    std::vector<nmh::VideoSample> video;
};

VRun run_video(const std::string& bin, nmh::Config cfg = nmh::Config{}) {
    VRun v;
    nmh::NanomigHarness h(cfg);
    v.run.loaded = h.load_program(bin) > 0;
    if (!v.run.loaded) return v;
    v.run.released = h.boot(kBootBudget).cpu_released;
    v.run.done = h.run_until_write(kDoneAddr, kDoneValue, kRunBudget);
    v.run.log = h.ram_log();
    v.video = h.video();
    return v;
}

unsigned rgb_of(const nmh::VideoSample& s) {
    return ((unsigned(s.r) & 15u) << 8) | ((unsigned(s.g) & 15u) << 4) | (unsigned(s.b) & 15u);
}
constexpr unsigned kMixed = 0x1000; // a sprite pixel whose samples do not all show one colour

struct OFrame {
    size_t begin = 0, end = 0;
    std::vector<nmh::Span> lines; // output lines, absolute sample indices
};

std::vector<nmh::Span> output_lines(const std::vector<nmh::VideoSample>& v, size_t begin, size_t end) {
    std::vector<nmh::Span> out;
    if (begin >= end || end > v.size()) return out;
    bool prev = v[begin].hs, have = false;
    size_t start = 0;
    for (size_t i = begin + 1; i < end; ++i) {
        if (v[i].hs && !prev) {
            if (have) out.push_back({start, i});
            start = i;
            have = true;
        }
        prev = v[i].hs;
    }
    if (have) out.push_back({start, end});
    return out;
}

// The output frames used (SPECIFICATION.md 2.5): complete output frames that begin at or after
// S4 (t_s4, the Copper's first fetch) and end by S7 (t_s7, the completion word's write); from the
// third of them to the last.
std::vector<OFrame> used_output_frames(const std::vector<nmh::VideoSample>& v, uint64_t t_s4, uint64_t t_s7) {
    std::vector<OFrame> out;
    auto spans = nmh::find_frames(v);
    std::vector<size_t> complete;
    for (size_t i = 1; i + 1 < spans.size(); ++i)
        if (spans[i].begin >= t_s4 && spans[i].end <= t_s7) complete.push_back(i);
    for (size_t k = 2; k < complete.size(); ++k) {
        OFrame f;
        f.begin = spans[complete[k]].begin;
        f.end = spans[complete[k]].end;
        f.lines = output_lines(v, f.begin, f.end);
        out.push_back(f);
    }
    return out;
}

struct Image {
    long top = 0, nlines = 0, left = 0, w = 0, p = 0;
    unsigned first_rgb = 0;
    size_t first_line_abs = 0;
    std::vector<std::array<unsigned, 16>> rows; // per output line: the 16 sprite-pixel colours; empty unless p > 0
};

std::vector<Image> extract_images(const std::vector<nmh::VideoSample>& v, const OFrame& f) {
    std::vector<Image> out;
    const size_t n = f.lines.size();
    auto line_any = [&](size_t li) {
        for (size_t i = f.lines[li].begin; i < f.lines[li].end; ++i) if (rgb_of(v[i]) != 0) return true;
        return false;
    };
    size_t li = 0;
    while (li < n) {
        if (!line_any(li)) { ++li; continue; }
        size_t lj = li;
        while (lj < n && line_any(lj)) ++lj;
        Image im;
        im.top = (long)li;
        im.nlines = (long)(lj - li);
        im.first_line_abs = f.lines[li].begin;
        const nmh::Span sp = f.lines[li];
        size_t x = sp.begin;
        while (x < sp.end && rgb_of(v[x]) == 0) ++x;
        im.left = (long)(x - sp.begin);
        im.first_rgb = rgb_of(v[x]);
        size_t y = x;
        while (y < sp.end && rgb_of(v[y]) != 0) ++y;
        im.w = (long)(y - x);
        im.p = (im.w > 0 && im.w % 16 == 0) ? im.w / 16 : 0;
        if (im.p > 0) {
            for (size_t r = li; r < lj; ++r) {
                std::array<unsigned, 16> cells{};
                for (size_t k = 0; k < 16; ++k) {
                    const size_t base = f.lines[r].begin + (size_t)im.left + k * (size_t)im.p;
                    if (base + (size_t)im.p > f.lines[r].end) { cells[k] = kMixed; continue; }
                    unsigned c = rgb_of(v[base]);
                    for (size_t q = 1; q < (size_t)im.p; ++q) if (rgb_of(v[base + q]) != c) c = kMixed;
                    cells[k] = c;
                }
                im.rows.push_back(cells);
            }
        }
        out.push_back(im);
        li = lj;
    }
    return out;
}

uint64_t modal_value(const std::vector<uint64_t>& d) {
    std::map<uint64_t, size_t> hist;
    for (auto x : d) ++hist[x];
    uint64_t best = 0; size_t bc = 0;
    for (auto& kv : hist) if (kv.second > bc) { bc = kv.second; best = kv.first; }
    return best;
}

uint64_t hs_period(const std::vector<nmh::VideoSample>& v) {
    std::vector<uint64_t> d;
    uint64_t last = 0; bool have = false;
    for (size_t i = 1; i < v.size(); ++i)
        if (v[i].hs && !v[i - 1].hs) {
            if (have) d.push_back(i - last);
            last = i; have = true;
        }
    return modal_value(d);
}

long kv_from_periods(uint64_t beam, uint64_t out) { return (out > 0 && beam % out == 0) ? (long)(beam / out) : 0; }

// Q3's comparator: every image top and every image height, in output lines, is a multiple of
// k_v; the frame holds at least one image.
bool q3_images_ok(const std::vector<Image>& imgs, long kv) {
    if (kv <= 0 || imgs.empty()) return false;
    for (const auto& im : imgs) if (im.top % kv != 0 || im.nlines % kv != 0) return false;
    return true;
}

// Q2's comparator: W > 0 and W is divisible by 16.
bool q2_image_ok(const Image* im) { return im && im->w > 0 && im->w % 16 == 0; }

struct PortFrame {
    bool ok = false;
    uint64_t lo = 0, hi = 0;
    uint32_t start = 0; // word address of the list the restart fetch at lo read first
};

std::vector<std::pair<uint64_t, uint32_t>> restart_events(const std::vector<nmh::RamAccess>& log,
                                                          const std::vector<bool>& held,
                                                          const std::vector<uint32_t>& starts) {
    std::vector<std::pair<uint64_t, uint32_t>> out;
    for (size_t i = 0; i < log.size(); ++i) {
        const auto& a = log[i];
        if (a.is_write || held[i]) continue;
        for (auto s : starts) if (a.word_addr == s) { out.push_back({a.cycle, a.word_addr}); break; }
    }
    std::sort(out.begin(), out.end());
    return out;
}

// One sprite run read through the apparatus. `is_s2` selects how the reference image is named:
// in S2 it is the image whose first line shows COLOR21 (sprite 2, structure R); in S1 it is the
// first image (use 1 of structure Z).
struct SRun {
    std::string name;
    bool is_s2 = false;
    VRun v;
    std::vector<Block> blocks;
    std::vector<uint32_t> starts;
    uint32_t list_words = 0;
    std::vector<uint16_t> colour_regs; // the colour registers the run's list must write
    uint64_t t4 = 0, t7 = 0, beam_period = 0, out_period = 0;
    long kv = 0;
    std::vector<bool> held;
    std::vector<OFrame> frames;
    std::vector<std::vector<Image>> imgs;
    std::vector<PortFrame> pfs;
};

constexpr unsigned kColS0 = 0xF00; // COLOR17 as L_S1, L_A and L_B write it: sprite 0, pixel value 1
constexpr unsigned kColR = 0xFF0;  // COLOR21 as L_A and L_B write it: sprite 2, pixel value 1
constexpr unsigned kRegCol[4] = {0x000, 0xF00, 0x0F0, 0x00F}; // COLOR17-19 of L_S1 by pixel value
constexpr uint32_t kCopWordsS1 = 44;  // L_S1: V_spr 32 + V_bpl(1) 4 + three colour MOVEs 6 + END 2
constexpr uint32_t kCopWordsS2 = 54;  // L_A and L_B: V_spr 32 + V_bpl(1) 4 + six colour MOVEs 12 + COP1LC pair 4 + END 2

SRun make_srun(const std::string& name, bool is_s2, VRun&& vr, std::vector<Block> blocks,
               std::vector<uint32_t> starts, uint32_t list_words, std::vector<uint16_t> colour_regs) {
    SRun s;
    s.name = name; s.is_s2 = is_s2; s.v = std::move(vr);
    s.blocks = blocks; s.starts = starts; s.list_words = list_words; s.colour_regs = colour_regs;
    const auto& log = s.v.run.log;
    s.held = nmh::held_continuations(log, gS);
    s.t7 = s.v.run.done ? *s.v.run.done : 0;
    bool found = false;
    for (const auto& a : log)
        if (!a.is_write && a.word_addr >= wordof(COP_MAIN) && a.word_addr < wordof(COP_BLOCK_END)) {
            s.t4 = a.cycle; found = true; break;
        }
    if (found && s.v.run.done) s.frames = used_output_frames(s.v.video, s.t4, s.t7);
    for (const auto& f : s.frames) s.imgs.push_back(extract_images(s.v.video, f));
    {
        std::vector<uint64_t> d;
        auto groups = refresh_groups(log, s.blocks);
        for (size_t i = 0; i + 1 < groups.size(); ++i) d.push_back(groups[i + 1][0] - groups[i][0]);
        s.beam_period = modal_value(d);
        s.out_period = hs_period(s.v.video);
        s.kv = kv_from_periods(s.beam_period, s.out_period);
    }
    auto rs = restart_events(log, s.held, s.starts);
    for (size_t fi = 0; fi < s.frames.size(); ++fi) {
        PortFrame pf;
        const uint64_t tref = s.imgs[fi].empty() ? s.frames[fi].begin : s.imgs[fi][0].first_line_abs;
        long j = -1;
        for (size_t k = 0; k < rs.size(); ++k) if (rs[k].first <= tref) j = (long)k;
        if (j >= 0 && (size_t)j + 1 < rs.size()) {
            pf.ok = true; pf.lo = rs[(size_t)j].first; pf.hi = rs[(size_t)j + 1].first; pf.start = rs[(size_t)j].second;
        }
        s.pfs.push_back(pf);
    }
    return s;
}

const Image* find_by_rgb(const std::vector<Image>& v, unsigned rgb, int* count) {
    const Image* r = nullptr; int n = 0;
    for (const auto& im : v) if (im.first_rgb == rgb) { if (!r) r = &im; ++n; }
    if (count) *count = n;
    return r;
}

const Image* ref_image(const SRun& s, size_t fi) {
    if (s.is_s2) return find_by_rgb(s.imgs[fi], kColR, nullptr);
    return s.imgs[fi].empty() ? nullptr : &s.imgs[fi][0];
}

std::string list_name(uint32_t start) {
    return start == wordof(COP_MAIN) ? "L_A/L_S1" : start == wordof(COP_SECOND) ? "L_B" : "?";
}

bool pre_c1(const SRun& s) { return s.v.run.loaded && s.v.run.released && s.v.run.done.has_value(); }

bool pre_frames(const SRun& s, Checks& checks) {
    checks.note("FRAMES " + s.name + ": output frames used=" + std::to_string(s.frames.size()) + " (S4 at " +
                std::to_string(s.t4) + ", S7 at " + std::to_string(s.t7) + ")");
    return s.frames.size() >= 3;
}

bool pre_q2(const SRun& s, Checks& checks) {
    bool ok = !s.frames.empty();
    std::ostringstream m;
    for (size_t fi = 0; fi < s.frames.size(); ++fi) {
        const Image* r = ref_image(s, fi);
        ok = ok && q2_image_ok(r);
        m << "f" << fi << ":W=" << (r ? std::to_string(r->w) : std::string("none")) << ",p="
          << (r ? std::to_string(r->p) : std::string("-")) << ";";
    }
    checks.note("Q2 " + s.name + " (first line of the reference image): " + m.str());
    return ok;
}

bool pre_q3(const SRun& s, Checks& checks) {
    bool ok = !s.frames.empty() && s.kv > 0;
    std::ostringstream m;
    for (size_t fi = 0; fi < s.frames.size(); ++fi) {
        ok = ok && q3_images_ok(s.imgs[fi], s.kv);
        m << "f" << fi << ":";
        for (const auto& im : s.imgs[fi]) m << "(top " << im.top << ",lines " << im.nlines << ")";
        m << ";";
    }
    checks.note("Q3 " + s.name + ": beam-line period=" + std::to_string(s.beam_period) + " hs period=" +
                std::to_string(s.out_period) + " k_v=" + std::to_string(s.kv) + " images " + m.str());
    return ok;
}

// The colour writes the Copper fetched from the run's list in a port frame: the list words in
// order, a MOVE's register word at an even offset from the list start and its datum after it.
std::vector<std::pair<uint16_t, uint16_t>> colour_writes(const SRun& s, const PortFrame& pf) {
    std::vector<int> d(s.list_words, -1);
    for (size_t i = 0; i < s.v.run.log.size(); ++i) {
        const auto& a = s.v.run.log[i];
        if (a.is_write || s.held[i] || a.cycle < pf.lo || a.cycle >= pf.hi) continue;
        if (a.word_addr >= pf.start && a.word_addr < pf.start + s.list_words) d[a.word_addr - pf.start] = a.data;
    }
    std::vector<std::pair<uint16_t, uint16_t>> out;
    for (size_t i = 0; i + 1 < d.size(); i += 2) {
        if (d[i] < 0 || d[i + 1] < 0) continue;
        const uint16_t reg = (uint16_t)d[i];
        if (reg == COLOR17 || reg == COLOR18 || reg == COLOR19 || reg == COLOR21 || reg == COLOR22 || reg == COLOR23) {
            std::pair<uint16_t, uint16_t> w{reg, (uint16_t)d[i + 1]};
            if (std::find(out.begin(), out.end(), w) == out.end()) out.push_back(w);
        }
    }
    return out;
}

// Q4: the colours the list writes are pairwise distinct from each other and from COLOR00 ($000),
// and the list writes every colour register the run's images use.
bool pre_q4(const SRun& s, Checks& checks) {
    bool ok = !s.frames.empty();
    std::ostringstream m;
    for (size_t fi = 0; fi < s.pfs.size(); ++fi) {
        if (!s.pfs[fi].ok) { ok = false; m << "f" << fi << ":no port frame;"; continue; }
        auto w = colour_writes(s, s.pfs[fi]);
        bool this_ok = true;
        for (auto reg : s.colour_regs) {
            bool have = false;
            for (auto& x : w) if (x.first == reg) have = true;
            this_ok = this_ok && have;
        }
        for (size_t i = 0; i < w.size(); ++i) {
            if (w[i].second == 0x000) this_ok = false;
            for (size_t j = i + 1; j < w.size(); ++j) if (w[i].second == w[j].second) this_ok = false;
        }
        ok = ok && this_ok;
        m << "f" << fi << ":" << w.size() << " writes" << (this_ok ? "" : "(FAIL)") << ";";
    }
    checks.note("Q4 " + s.name + " (colour MOVEs fetched per port frame): " + m.str());
    return ok;
}

// Q5: every word of the list the frame's restart fetch started is fetched in that frame, and
// (S2) the restart fetches of consecutive frames alternate between L_A and L_B.
bool pre_q5(const SRun& s, Checks& checks) {
    bool ok = !s.frames.empty();
    std::ostringstream m;
    for (size_t fi = 0; fi < s.pfs.size(); ++fi) {
        const PortFrame& pf = s.pfs[fi];
        if (!pf.ok) { ok = false; m << "f" << fi << ":no port frame;"; continue; }
        size_t cnt = 0;
        bool full = words_fully_read(s.v.run.log, pf.lo, pf.hi, pf.start, (int)s.list_words, &cnt);
        ok = ok && full;
        if (s.is_s2) {
            if (pf.start != wordof(COP_MAIN) && pf.start != wordof(COP_SECOND)) ok = false;
            if (fi > 0 && pf.start == s.pfs[fi - 1].start) ok = false;
        } else if (pf.start != wordof(COP_MAIN)) ok = false;
        m << "f" << fi << ":" << list_name(pf.start) << " " << cnt << "/" << s.list_words << (full ? "" : "(FAIL)") << ";";
    }
    checks.note("Q5 " + s.name + ": " + m.str());
    return ok;
}

bool pre_qref(const SRun& s, Checks& checks) {
    bool ok = !s.frames.empty();
    std::ostringstream m;
    for (size_t fi = 0; fi < s.frames.size(); ++fi) {
        int n = 0;
        const Image* r = find_by_rgb(s.imgs[fi], kColR, &n);
        if (n != 1) { ok = false; m << "f" << fi << ":" << n << " images;"; continue; }
        const Image* r0 = find_by_rgb(s.imgs[0], kColR, nullptr);
        if (!r0 || r->top != r0->top || r->left != r0->left) ok = false;
        m << "f" << fi << ":(top " << r->top << ",left " << r->left << ");";
    }
    checks.note("Q-ref " + s.name + ": " + m.str());
    return ok;
}

std::vector<uint32_t> block0_reads(const SRun& s, const PortFrame& pf) {
    std::vector<uint32_t> out;
    const uint32_t lo = wordof(CH_SPR[0]), hi = wordof(CH_SPR[0] + 0x400);
    for (size_t i = 0; i < s.v.run.log.size(); ++i) {
        const auto& a = s.v.run.log[i];
        if (a.is_write || s.held[i] || a.cycle < pf.lo || a.cycle >= pf.hi) continue;
        if (a.word_addr >= lo && a.word_addr < hi) out.push_back(a.word_addr - lo);
    }
    return out;
}

bool pre_k9(const SRun& s, Checks& checks) {
    bool ok = !s.frames.empty();
    std::ostringstream m;
    for (size_t fi = 0; fi < s.pfs.size(); ++fi) {
        if (!s.pfs[fi].ok) { ok = false; m << "f" << fi << ":no port frame;"; continue; }
        auto r = block0_reads(s, s.pfs[fi]);
        const bool this_ok = !r.empty() && r.front() == 0;
        ok = ok && this_ok;
        m << "f" << fi << ":" << r.size() << " reads, first word " << (r.empty() ? std::string("-") : std::to_string(r.front()))
          << (this_ok ? "" : "(FAIL)") << ";";
    }
    checks.note("K9 " + s.name + ": " + m.str());
    return ok;
}

bool pre_img4(const SRun& s, Checks& checks) {
    bool ok = !s.frames.empty();
    std::ostringstream m;
    for (size_t fi = 0; fi < s.imgs.size(); ++fi) {
        ok = ok && s.imgs[fi].size() == 4;
        m << "f" << fi << ":" << s.imgs[fi].size() << " images;";
    }
    checks.note("IMG4 " + s.name + ": " + m.str());
    return ok;
}

// ---- the propositions' comparators ----------------------------------------

// Structure Z (SPECIFICATION.md 2.9): per use the height in beam lines, the data words of lines
// 2.., the top and the left edge relative to use 1 (beam lines, sprite pixels).
struct ZUse { int height; uint16_t lo, hi; int top, left; };
constexpr ZUse kZ[4] = {{8, 0xF0F0, 0xFF00, 0, 0}, {16, 0x0F0F, 0x00FF, 16, 40},
                        {4, 0xCCCC, 0xF0F0, 40, -16}, {12, 0xAAAA, 0xCCCC, 56, 80}};

// The value (0..3) of sprite pixel k (0 leftmost) on data line j of use `use`; line 0 is the
// first data line $FFFF,$0000. Two times the bit of the high word plus the bit of the low word.
int z_value(int use, int j, int k) {
    const uint16_t lo = j == 0 ? 0xFFFF : kZ[use].lo, hi = j == 0 ? 0x0000 : kZ[use].hi;
    return 2 * ((hi >> (15 - k)) & 1) + ((lo >> (15 - k)) & 1);
}

// P8: in every frame used the offset of sprite 0's image from sprite 2's is (16 sprite pixels,
// 32 beam lines) or (40, 42), and consecutive frames differ by +-(24 pixels, 10 lines).
bool p8_check(const SRun& s, Checks& checks) {
    bool ok = s.frames.size() >= 1;
    std::ostringstream m;
    std::vector<std::pair<long, long>> d; // (samples, output lines) of sprite 0 from sprite 2
    std::vector<long> pp;
    for (size_t fi = 0; fi < s.frames.size(); ++fi) {
        int nr = 0, n0 = 0;
        const Image* r = find_by_rgb(s.imgs[fi], kColR, &nr);
        const Image* s0 = find_by_rgb(s.imgs[fi], kColS0, &n0);
        if (nr != 1 || n0 != 1 || r->p <= 0 || s.kv <= 0) {
            ok = false; m << "f" << fi << ":R " << nr << " images, sprite 0 " << n0 << " images;";
            d.push_back({0, 0}); pp.push_back(1);
            continue;
        }
        const long dx = s0->left - r->left, dy = s0->top - r->top, p = r->p;
        const bool is_a = dx == 16 * p && dy == 32 * s.kv, is_b = dx == 40 * p && dy == 42 * s.kv;
        ok = ok && (is_a || is_b);
        d.push_back({dx, dy}); pp.push_back(p);
        m << "f" << fi << ":(" << dx << " samples=" << (dx / p) << " px, " << dy << " lines=" << (dy / s.kv) << " beam lines) "
          << (is_a ? "A" : is_b ? "B" : "neither") << " list " << list_name(s.pfs[fi].start) << ";";
    }
    for (size_t fi = 1; fi < d.size(); ++fi) {
        const long ddx = d[fi].first - d[fi - 1].first, ddy = d[fi].second - d[fi - 1].second, p = pp[fi];
        const bool alt = (ddx == 24 * p && ddy == 10 * s.kv) || (ddx == -24 * p && ddy == -10 * s.kv);
        ok = ok && alt;
    }
    checks.note("P8 " + s.name + ": " + m.str());
    return ok;
}

// P9: four images in every frame used, at the tops and left edges of structure Z's uses, and in
// every port frame the words of Z read in address order, each once.
bool p9_check(const SRun& s, Checks& checks) {
    bool ok = !s.frames.empty();
    std::ostringstream m;
    for (size_t fi = 0; fi < s.frames.size(); ++fi) {
        const auto& im = s.imgs[fi];
        bool a_ok = im.size() == 4 && im[0].p > 0 && s.kv > 0;
        if (a_ok)
            for (int u = 0; u < 4; ++u) {
                if (im[(size_t)u].top - im[0].top != kZ[u].top * s.kv) a_ok = false;
                if (im[(size_t)u].left - im[0].left != kZ[u].left * im[0].p) a_ok = false;
            }
        bool b_ok = s.pfs[fi].ok;
        size_t nreads = 0;
        if (b_ok) {
            auto r = block0_reads(s, s.pfs[fi]);
            nreads = r.size();
            b_ok = r.size() == 90;
            for (size_t i = 0; b_ok && i < r.size(); ++i) if (r[i] != i) b_ok = false;
        }
        ok = ok && a_ok && b_ok;
        m << "f" << fi << ":" << im.size() << " images";
        if (im.size() == 4) m << " tops " << im[0].top << "," << im[1].top << "," << im[2].top << "," << im[3].top
                              << " lefts " << im[0].left << "," << im[1].left << "," << im[2].left << "," << im[3].left;
        m << " reads " << nreads << (a_ok ? "" : " (a FAIL)") << (b_ok ? "" : " (b FAIL)") << ";";
    }
    checks.note("P9 " + s.name + ": " + m.str());
    return ok;
}

// P10: the heights of the four images are 8, 16, 4 and 12 beam lines.
bool p10_check(const SRun& s, Checks& checks) {
    bool ok = !s.frames.empty() && s.kv > 0;
    std::ostringstream m;
    for (size_t fi = 0; fi < s.frames.size(); ++fi) {
        const auto& im = s.imgs[fi];
        bool f_ok = im.size() == 4;
        m << "f" << fi << ":";
        for (size_t u = 0; u < im.size(); ++u) {
            m << im[u].nlines << " lines,";
            if (u < 4 && s.kv > 0 && im[u].nlines != kZ[u].height * s.kv) f_ok = false;
        }
        ok = ok && f_ok;
        m << (f_ok ? "" : "(FAIL)") << ";";
    }
    checks.note("P10 " + s.name + ": " + m.str());
    return ok;
}

// P11: on every displayed line of every image, the pixels of value 0 show the background and the
// others do not; within one image, pixels of one value show one colour and pixels of different
// non-zero values show different colours.
bool p11_check(const SRun& s, Checks& checks) {
    bool ok = !s.frames.empty() && s.kv > 0;
    size_t cells = 0, bad = 0;
    for (size_t fi = 0; fi < s.frames.size(); ++fi) {
        const auto& im = s.imgs[fi];
        if (im.size() != 4) { ok = false; continue; }
        for (int u = 0; u < 4; ++u) {
            const Image& img = im[(size_t)u];
            unsigned col[4] = {0, 0, 0, 0};
            if (img.p <= 0) { ok = false; continue; }
            for (size_t r = 0; r < img.rows.size(); ++r) {
                const long j = s.kv > 0 ? (long)r / s.kv : 0;
                if (j >= kZ[u].height) { ++bad; continue; }
                for (int k = 0; k < 16; ++k) {
                    const int v = z_value(u, (int)j, k);
                    const unsigned c = img.rows[r][(size_t)k];
                    ++cells;
                    if (c == kMixed) { ++bad; continue; }
                    if (v == 0) { if (c != 0) ++bad; continue; }
                    if (c == 0) { ++bad; continue; }
                    if (col[v] == 0) col[v] = c; else if (col[v] != c) ++bad;
                }
            }
            if ((col[1] && col[1] == col[2]) || (col[1] && col[1] == col[3]) || (col[2] && col[2] == col[3])) ++bad;
        }
    }
    checks.note("P11 " + s.name + ": cells compared=" + std::to_string(cells) + " cells off=" + std::to_string(bad));
    return ok && cells > 0 && bad == 0;
}

// P12: every non-transparent pixel of value 1, 2, 3 shows $F00, $0F0, $00F, in all four uses.
bool p12_check(const SRun& s, Checks& checks) {
    bool ok = !s.frames.empty() && s.kv > 0;
    size_t cells = 0, bad = 0;
    std::map<unsigned, size_t> seen[4];
    for (size_t fi = 0; fi < s.frames.size(); ++fi) {
        const auto& im = s.imgs[fi];
        if (im.size() != 4) { ok = false; continue; }
        for (int u = 0; u < 4; ++u) {
            const Image& img = im[(size_t)u];
            if (img.p <= 0) { ok = false; continue; }
            for (size_t r = 0; r < img.rows.size(); ++r) {
                const long j = s.kv > 0 ? (long)r / s.kv : 0;
                if (j >= kZ[u].height) { ++bad; continue; }
                for (int k = 0; k < 16; ++k) {
                    const int v = z_value(u, (int)j, k);
                    if (v == 0) continue;
                    ++cells;
                    ++seen[u][img.rows[r][(size_t)k]];
                    if (img.rows[r][(size_t)k] != kRegCol[v]) ++bad;
                }
            }
        }
    }
    std::ostringstream m;
    for (int u = 0; u < 4; ++u) {
        m << "use" << (u + 1) << ":";
        for (auto& kv : seen[u]) m << " " << hex(kv.first).substr(1) << "x" << kv.second;
        m << ";";
    }
    checks.note("P12 " + s.name + ": non-transparent pixels compared=" + std::to_string(cells) + " off=" + std::to_string(bad) +
                " colours seen " + m.str());
    return ok && cells > 0 && bad == 0;
}

// ---- synthetic captures for the apparatus' own self-tests ------------------

struct SynthImg {
    long top, nlines, left, p; // output lines, output lines, samples, samples per sprite pixel
    unsigned first_rgb;        // the colour of the first line (all 16 pixels set) and of value 1
    bool pattern;              // later lines show the pixel pattern (k/4)%4 in colours first_rgb, 0x0F0, 0x00F
};
constexpr size_t kSynthL = 64, kSynthLPF = 24, kSynthFP = kSynthL * kSynthLPF, kSynthVs = 17;

std::vector<nmh::VideoSample> synth_video(int nframes, const std::function<std::vector<SynthImg>(int)>& imgs_of,
                                          long altered_row = -1) {
    std::vector<nmh::VideoSample> v((size_t)nframes * kSynthFP + kSynthFP, nmh::VideoSample{0, 0, 0, false, false});
    for (size_t i = 0; i < v.size(); ++i) {
        v[i].hs = (i % kSynthL) < 4;
        v[i].vs = (i % kSynthFP) >= kSynthVs && (i % kSynthFP) < kSynthVs + 5;
    }
    auto put = [&](size_t i, unsigned rgb) { v[i].r = rgb >> 8; v[i].g = (rgb >> 4) & 15; v[i].b = rgb & 15; };
    for (int f = 0; f < nframes; ++f)
        for (const auto& im : imgs_of(f))
            for (long r = 0; r < im.nlines; ++r)
                for (int k = 0; k < 16; ++k) {
                    unsigned c = im.first_rgb;
                    if (r > 0 && im.pattern) {
                        const unsigned pal[4] = {0, im.first_rgb, 0x0F0, 0x00F};
                        c = pal[(k / 4) % 4];
                    }
                    if (altered_row == r && k == 5) c = 0x123;
                    if (c == 0) continue;
                    for (long q = 0; q < im.p; ++q)
                        put((size_t)f * kSynthFP + kSynthL * (size_t)(im.top + r + 1) + (size_t)(im.left + k * im.p + q), c);
                }
    return v;
}

// ---- the video lines ------------------------------------------------------

bool video_selector(const std::string& mv) {
    for (const char* s : {"FRAMES", "Q2", "Q4", "Q5", "Q-ref", "K9", "IMG4", "P8", "P9", "P10", "P11", "P10-P11", "P12"})
        if (mv == s) return true;
    return false;
}

nmh::Config video_config() { return nmh::Config{}; }

// Builds S2 and S1 (normal runs, or the named variants in their place) and returns them.
std::pair<SRun, SRun> make_video_runs(const std::string& dir, const std::string& s2bin, const std::string& s1bin,
                                      uint32_t s1_words) {
    SRun s2 = make_srun("S2", true, run_video(dir + "/" + s2bin), blocks_S2(), {wordof(COP_MAIN), wordof(COP_SECOND)},
                        kCopWordsS2, {COLOR17, COLOR18, COLOR19, COLOR21, COLOR22, COLOR23});
    SRun s1 = make_srun("S1", false, run_video(dir + "/" + s1bin), blocks_S1(), {wordof(COP_MAIN)}, s1_words,
                        {COLOR17, COLOR18, COLOR19});
    return {std::move(s2), std::move(s1)};
}

// Evaluates and prints every video line. `only` empty: all of them are graded lines (the normal
// run). `only` a line's id (a mutation run): that line is the graded line and the others print
// as MEASURED, so that the lines a contrast leaves alone or moves are visible.
void video_lines(Checks& checks, const SRun& s2, const SRun& s1, const std::string& only) {
    auto want = [&](const char* id) { return only.empty() || only == id; };
    auto verdict = [&](const char* id, bool ok, const std::string& unmet) {
        checks.note(std::string("line ") + id + " (not the line under test): " + (unmet.empty() ? (ok ? "PASS" : "FAIL") : "NOT GRADED " + unmet));
    };
    const std::string c1u = unmet_if(pre_c1(s2), "S2:C1") + unmet_if(pre_c1(s1), "S1:C1");

    const bool fr2 = pre_frames(s2, checks), fr1 = pre_frames(s1, checks);
    if (want("FRAMES")) {
        if (c1u.empty())
            checks.expect(fr2 && fr1, "FRAMES: at least three complete output frames lie from the third complete output frame after S4 "
                          "up to the last complete output frame before S7, in S2 and in S1. SPECIFICATION.md 2.5 P8.4");
        else not_graded(checks, "FRAMES", c1u);
    } else verdict("FRAMES", fr2 && fr1, c1u);

    const bool q2_2 = pre_q2(s2, checks), q2_1 = pre_q2(s1, checks);
    if (want("Q2")) {
        if (c1u.empty())
            checks.expect(q2_2 && q2_1, "Q2: on the first line of the reference image (S2: sprite 2's image; S1: use 1) all 16 sprite pixels "
                          "are set and that line's run of non-background output pixels has a length W that is greater than 0 and divisible by 16. "
                          "p.98 p.101 p.108");
        else not_graded(checks, "Q2", c1u);
    } else verdict("Q2", q2_2 && q2_1, c1u);

    const bool q3_2 = pre_q3(s2, checks), q3_1 = pre_q3(s1, checks);
    if (want("Q3")) {
        if (c1u.empty())
            checks.expect(q3_2 && q3_1, "Q3: every measured image top, and every image height, in output lines is a multiple of k_v, the "
                          "output lines per beam line. p.107 SPECIFICATION.md 2.5");
        else not_graded(checks, "Q3", c1u);
    } else verdict("Q3", q3_2 && q3_1, c1u);

    const bool q4_2 = pre_q4(s2, checks), q4_1 = pre_q4(s1, checks);
    if (want("Q4")) {
        if (c1u.empty())
            checks.expect(q4_2 && q4_1, "Q4: the colours the run's Copper list writes (COLOR17-19 in S1; COLOR17-19 and COLOR21-23 in S2) "
                          "are pairwise distinct from each other and from COLOR00 ($000). p.46 p.108");
        else not_graded(checks, "Q4", c1u);
    } else verdict("Q4", q4_2 && q4_1, c1u);

    const bool q5_2 = pre_q5(s2, checks), q5_1 = pre_q5(s1, checks);
    if (want("Q5")) {
        if (c1u.empty())
            checks.expect(q5_2 && q5_1, "Q5: every word of the Copper list a frame's restart fetch starts is fetched in that frame, and in S2 the "
                          "restart fetches of consecutive frames alternate between L_A and L_B. p.25 p.26");
        else not_graded(checks, "Q5", c1u);
    } else verdict("Q5", q5_2 && q5_1, c1u);

    const bool qref = pre_qref(s2, checks);
    const std::string c1_2 = unmet_if(pre_c1(s2), "S2:C1");
    if (want("Q-ref")) {
        if (c1_2.empty())
            checks.expect(qref, "Q-ref: sprite 2's image (the reference R) is at the same output position in every output frame used of S2. "
                          "p.113");
        else not_graded(checks, "Q-ref", c1_2);
    } else verdict("Q-ref", qref, c1_2);

    const bool k9 = pre_k9(s1, checks);
    const std::string c1_1 = unmet_if(pre_c1(s1), "S1:C1");
    if (want("K9")) {
        if (c1_1.empty())
            checks.expect(k9, "K9: in each observed frame of S1, sprite block 0 is read from use 1's first control word onward. p.109 p.116");
        else not_graded(checks, "K9", c1_1);
    } else verdict("K9", k9, c1_1);

    const bool img4 = pre_img4(s1, checks);
    if (want("IMG4")) {
        if (c1_1.empty())
            checks.expect(img4, "IMG4: every output frame used of S1 shows exactly four images of sprite 0. p.116 p.117");
        else not_graded(checks, "IMG4", c1_1);
    } else verdict("IMG4", img4, c1_1);

    // P8 (S2)
    {
        const std::string u = c1_2 + unmet_if(fr2, "FRAMES") + unmet_if(q2_2, "Q2") + unmet_if(q3_2, "Q3") + unmet_if(q4_2, "Q4") +
                              unmet_if(q5_2, "Q5") + unmet_if(qref, "Q-ref");
        const bool ok = p8_check(s2, checks);
        if (want("P8")) {
            if (u.empty())
                checks.expect(ok, "P8: the object sprite 0 displays is placed by the position in its data structure and moves by exactly the "
                              "change in that position from one field to the next: the offset of sprite 0's image from sprite 2's is "
                              "(+16 sprite pixels, +32 beam lines) or (+40, +42), consecutive frames alternate and differ by +-24 pixels and "
                              "+-10 lines. COP2-putting-together-a-copper-instruction-list.c1 p.27 p.113");
            else not_graded(checks, "P8", u);
        } else verdict("P8", ok, u);
    }

    // P9 to P12 (S1)
    const std::string u9 = c1_1 + unmet_if(fr1, "FRAMES") + unmet_if(q2_1, "Q2") + unmet_if(q3_1, "Q3") + unmet_if(q4_1, "Q4") +
                           unmet_if(q5_1, "Q5") + unmet_if(k9, "K9");
    const std::string u10 = u9 + unmet_if(img4, "IMG4");
    {
        const bool ok = p9_check(s1, checks);
        if (want("P9")) {
            if (u9.empty())
                checks.expect(ok, "P9: one sprite DMA channel following one chained data structure displays four separate images in the same "
                              "field (tops +0, +16, +40, +56 beam lines and left edges +0, +40, -16, +80 sprite pixels from use 1) and reads "
                              "the 90 words of all four uses in address order, each once, between two consecutive restart fetches. "
                              "COP2-putting-together-a-copper-instruction-list.c2 ch2-putting-together-a-copper-instruction-list.c5 p.27 p.110 p.116");
            else not_graded(checks, "P9", u9);
        } else verdict("P9", ok, u9);
    }
    const bool ok10 = p10_check(s1, checks);
    if (want("P10")) {
        if (u10.empty())
            checks.expect(ok10, "P10: the four images of one reused sprite are 8, 16, 4 and 12 beam lines high, each its own VSTOP - VSTART. "
                          "COP2-putting-together-a-copper-instruction-list.c3 p.27 p.107");
        else not_graded(checks, "P10", u10);
    } else verdict("P10", ok10, u10);
    const bool ok11 = p11_check(s1, checks);
    if (want("P11")) {
        if (u10.empty())
            checks.expect(ok11, "P11: on every line of every use the pixels of value 0 show the background and the others do not, and "
                          "pixels of equal value show equal colours while pixels of different non-zero value show different colours. "
                          "COP2-putting-together-a-copper-instruction-list.c4 p.27 p.101 p.107 p.108");
        else not_graded(checks, "P11", u10);
    } else verdict("P11", ok11, u10);
    if (want("P10-P11")) {
        if (u10.empty())
            checks.expect(ok10 && ok11, "P10-P11: the size and the shape of the reuses of a sprite can be changed: both the heights (P10) and the "
                          "pixel patterns (P11) of the four reuses of one sprite show their own data structure. "
                          "ch2-putting-together-a-copper-instruction-list.c6 p.27");
        else not_graded(checks, "P10-P11", u10);
    } else verdict("P10-P11", ok10 && ok11, u10);
    const bool ok12 = p12_check(s1, checks);
    if (want("P12")) {
        if (u10.empty())
            checks.expect(ok12, "P12: with no Copper write to COLOR17-19 between the uses, every non-transparent pixel of value 1, 2, 3 shows "
                          "$F00, $0F0, $00F in all four uses of the sprite. "
                          "ch2-putting-together-a-copper-instruction-list.c7 COP2-putting-together-a-copper-instruction-list.c5 p.27 p.46 p.108");
        else not_graded(checks, "P12", u10);
    } else verdict("P12", ok12, u10);
}

// The apparatus' own self-tests, on synthetic captures (no module runs).
void video_selftests(Checks& checks) {
    const long kv = 2;
    auto base_imgs = [&](int) {
        return std::vector<SynthImg>{{4, 6, 10, 2, 0xF00, true}, {14, 4, 30, 2, 0xFF0, false}};
    };
    // Extraction: two images in every frame, found with the painted tops, heights, left edges, widths and pixels.
    {
        auto v = synth_video(8, base_imgs);
        auto spans = nmh::find_frames(v);
        const uint64_t t4 = kSynthFP * 1 + 100, t7 = kSynthFP * 6 + 100;
        auto used = used_output_frames(v, t4, t7);
        bool sel_ok = spans.size() >= 8 && used.size() == 2 && used[0].begin == kSynthFP * 4 + kSynthVs &&
                      used[1].begin == kSynthFP * 5 + kSynthVs;
        bool ext_ok = sel_ok;
        std::string what;
        if (sel_ok) {
            for (const auto& f : used) {
                auto im = extract_images(v, f);
                bool f_ok = im.size() == 2 && f.lines.size() == kSynthLPF;
                if (f_ok) {
                    f_ok = im[0].top == 4 && im[0].nlines == 6 && im[0].left == 10 && im[0].w == 32 && im[0].p == 2 && im[0].first_rgb == 0xF00 &&
                           im[1].top == 14 && im[1].nlines == 4 && im[1].left == 30 && im[1].w == 32 && im[1].p == 2 && im[1].first_rgb == 0xFF0 &&
                           im[0].rows.size() == 6 && im[1].rows.size() == 4;
                    const unsigned pal[4] = {0, 0xF00, 0x0F0, 0x00F};
                    for (size_t r = 1; f_ok && r < im[0].rows.size(); ++r)
                        for (size_t k = 0; k < 16; ++k) if (im[0].rows[r][k] != pal[(k / 4) % 4]) f_ok = false;
                    for (size_t k = 0; f_ok && k < 16; ++k) if (im[0].rows[0][k] != 0xF00 || im[1].rows[3][k] != 0xFF0) f_ok = false;
                }
                ext_ok = ext_ok && f_ok;
            }
        }
        // The same capture with one pixel changed must read differently.
        bool sens_ok = false;
        if (sel_ok) {
            auto v2 = synth_video(8, base_imgs, 3);
            auto used2 = used_output_frames(v2, t4, t7);
            if (used2.size() == 2) {
                auto im = extract_images(v2, used2[0]);
                sens_ok = im.size() == 2 && im[0].rows.size() == 6 && im[0].rows[3][5] == 0x123;
            }
        }
        // hs period and k_v: a beam-line period of two output lines gives 2; an incommensurate one gives 0.
        const bool kv_ok = hs_period(v) == kSynthL && kv_from_periods(2 * kSynthL, hs_period(v)) == 2 &&
                           kv_from_periods(100, hs_period(v)) == 0;
        checks.note("SELFTEST-VIDEO: used frames=" + std::to_string(used.size()) + " extraction=" + std::to_string(ext_ok) +
                    " altered pixel read back=" + std::to_string(sens_ok) + " hs period=" + std::to_string(hs_period(v)) +
                    " k_v checks=" + std::to_string(kv_ok));
        checks.expect(sel_ok && ext_ok && sens_ok && kv_ok,
                      "SELFTEST-VIDEO: on a synthetic capture the apparatus selects the output frames from the third complete one after S4 to "
                      "the last complete one before S7, finds each painted image with its top, height, left edge, width and 16 pixel colours, "
                      "reads a changed pixel as changed, and derives k_v from the two line periods. SPECIFICATION.md 2.5");
    }
    // Q3: tops and heights that are multiples of k_v pass; a top moved by one output line and a height of an odd number of lines fail.
    {
        auto q3_of = [&](std::vector<SynthImg> imgs) {
            auto v = synth_video(8, [&](int) { return imgs; });
            auto used = used_output_frames(v, kSynthFP * 1 + 100, kSynthFP * 6 + 100);
            if (used.empty()) return false;
            return q3_images_ok(extract_images(v, used[0]), kv);
        };
        const bool good = q3_of({{4, 6, 10, 2, 0xF00, true}, {14, 4, 30, 2, 0xFF0, false}});
        const bool shifted = q3_of({{4, 6, 10, 2, 0xF00, true}, {15, 4, 30, 2, 0xFF0, false}});
        const bool odd_height = q3_of({{4, 5, 10, 2, 0xF00, true}, {14, 4, 30, 2, 0xFF0, false}});
        checks.note("SELFTEST-Q3: aligned=" + std::to_string(good) + ", one image moved by one output line=" + std::to_string(shifted) +
                    ", a height of five output lines=" + std::to_string(odd_height));
        checks.expect(good && !shifted && !odd_height,
                      "SELFTEST-Q3: q3_images_ok passes a synthetic frame whose image tops and heights are multiples of k_v and fails the same frame "
                      "with one image moved by one output line and with an image of an odd number of output lines. p.107 SPECIFICATION.md 2.5");
    }
}

} // namespace

// Each record names an assertion's leading label, its kind and its result, and runs on one line. NMH_MUTATE=<selector> replays it on a core; the values are what that replay printed. A mutation run evaluates and prints the line's preconditions with the comparators of the normal run first; the normal run's line on the same comparator is the green control. Kind `stimulus red`: the line prints FAIL; kind `stimulus not graded`: the line prints NOT GRADED with the failed precondition.
// MUTATION P1 none impossible the four refresh slots of every line are fixed by the hardware (p.194, Fig. 6-9; DMACON has no enable bit for refresh) and no register or instruction moves one to an odd cycle; another agent's access to memory no block declares joins R by exclusion (D7), a false attribution and not a changed refresh: a read there joins or starts a refresh group and fails F2, F2-COUNT or F3 of the same run, a CPU write there fails F1 (the CPU's own phase), and a DMA write there (the blitter's D channel; no run enables the blitter) turns P1 red with every precondition met; P1's gate has C1, the restarts, F1 and F3 and neither F2 nor F2-COUNT, and its red path is shown by SELFTEST-P1
// MUTATION P2 none impossible the disk channel's slots are fixed by the hardware (p.20, p.194) and no register or instruction moves one to an odd cycle; DSKPT, DSKLEN and ADKCON change which words are transferred, not in which cycle, and a run that aims DSKPT at the Copper's words fails only through the address attribution
// MUTATION P3 none impossible the audio channels' slots are fixed by the hardware (p.20, p.194) and no register or instruction moves one to an odd cycle; AUDxLC, AUDxLEN and AUDxPER change which words and how often, not in which cycle, and a run that aims a channel at the Copper's words fails only through the address attribution
// MUTATION P4 none impossible the sprite channels' slots are fixed by the hardware (p.20, p.194) and no register or instruction moves one to an odd cycle; the sprite pointers and the data structure change which words are fetched, not in which cycle, and a run in which the Copper executes words a sprite channel reads fails only through the address attribution
// MUTATION P5 none impossible the fetch slots of low-resolution planes 1-4 are fixed on the even cycles (Fig. 6-9, Fig. 6-11) and no register or instruction moves one to an odd cycle; DDFSTRT, DDFSTOP, BPLCON1 and the modulos change which words and lines are fetched, not their parity, a high-resolution fetch lies outside P5's premise (D2), and a run that aims plane 1 at the Copper's words fails only through the address attribution
// MUTATION A1 stimulus red C1a graded with the normal run's table (every reserved block declared), NMH_MUTATE=A1; both cores: DONE=1, 3436 accesses outside R, the CPU block and the Copper block
// MUTATION F1 none impossible the DMA slot grid is the memory cycle of the machine and no register or instruction moves a DMA channel to another phase of it; the 68000's own accesses lie on another phase and are outside F1's scope, and a CPU read of a declared block fails F1 only through the address attribution; the check's sensitivity is shown by SELFTEST-F1
// MUTATION F2 none impossible the four refresh slots of every line are fixed by the hardware (p.194, Fig. 6-9) and no register or instruction moves one; a fifth or displaced access in the refresh set is another agent's access to memory no block declares (D7), which is a false attribution and not a changed refresh; the check's sensitivity is shown by SELFTEST-F2
// MUTATION F2-COUNT none impossible the count of refresh cycles per line is fixed by the hardware (p.194: 4 cycles for memory refresh; DMACON has no enable bit for refresh) and no register or instruction adds or removes one; an extra entry in a group is another agent's access to memory no block declares (D7); the check's sensitivity is shown by SELFTEST-F2, whose synthetic group of five F2-COUNT must reject
// MUTATION F3 stimulus red C0-htotal (S2 writes HTOTAL=$E0 and BEAMCON0=$00A0, VARBEAMEN with PAL kept: every line is 225 memory cycles; S taken from the normal C0 run as in every plain run), NMH_MUTATE=F3; both cores: DONE=1, S=8, 939 groups, F1=1 F2=1 F2-COUNT=1 F3=0, every observed line interval 225 S
// MUTATION FIXTURE stimulus red C0-nofinish (the STOP loop's exit condition removed; the completion word is missing), NMH_MUTATE=FIXTURE; both cores: loaded=1, released=1, DONE=0
// MUTATION FIXTURE-PEEK stimulus red C0-exit6 (the wait loop exits at FRAME_CTR 6; the stored word is 6 where the readback expects 7), NMH_MUTATE=FIXTURE-PEEK; both cores: DONE=1, FRAME_CTR=0006
// MUTATION K2a stimulus red C2a-earlyread (C2a with DSKBYTR read right after DSKLEN <- $4000, before the two writes that start the transfer: DSKLEN bit 15 clear, DMAON=0), NMH_MUTATE=K2a; both cores: DONE=1, DSKBYTR=B000, F1=1 F2=1 F2-COUNT=1 F3=1, K2b comparator 936/939/939 and P2 population 2814, all even, unchanged
// MUTATION K2b stimulus red C2a-dskpt (C2a with DSKPT at $00C000, outside the declared disk block and declared as a block of its own in that run's table; both DSKLEN writes keep bit 15 set), NMH_MUTATE=K2b; both cores: DONE=1, F1=1 F2=1 F2-COUNT=1 F3=1, K2a holds (DSKBYTR=F000), disk-block accesses per observed frame 0/0/0, the moved transfer 2814 accesses, all on even cycles
// MUTATION K3 stimulus red C1a without AUD0EN in the S4 write, NMH_MUTATE=K3; both cores: DONE=1, channel 0 accesses per observed frame 0/0/0, channels 1-3 286/287/286
// MUTATION K4 stimulus red C3 without SPREN, NMH_MUTATE=K4; both cores: DONE=1, every sprite structure 0/86 words read in every observed frame
// MUTATION K5 stimulus red C4-4 without BPLEN, NMH_MUTATE=K5; both cores: DONE=1, 0 fetch lines for every plane in every observed frame
// MUTATION K6 stimulus red C6-endtrailer (C6 with the trailer's MOVE of the COPJMP2 strobe replaced by DC.W $FFFF,$FFFE: L5 is read to its end and L5b never), NMH_MUTATE=K6; both cores: C5 and C6-endtrailer DONE=1, F1=1 F2=1 F2-COUNT=1 F3=1, L5 972/972 and L5b 0/12 words read in every observed frame, T1 0 segments differ, T2 342 segment lines and 0 CPU accesses in them, T3 339 lines with nothing missing, P7 comparator 0 mismatches
// MUTATION T1 stimulus red C6-skip50 (C6 in which the Copper repeats WAIT 50's line with the SKIP loop of p.33: segment 49's third MOVE loads COP2LCL with the address of WAIT 50, segment 50's MOVEs are SKIP VP=$73 HP=0, COPJMP2 and MOVE COLOR00, segment 51's first MOVE loads COP2LCL with the address of L5b), NMH_MUTATE=T1; both cores: C5 and C6-skip50 DONE=1, F1=1 F2=1 F2-COUNT=1 F3=1, 6 of 339 segment/frame pairs differ in access count (segments 49 and 50 in each of 3 frames: 8/18 and 8/38), K6 972/972 and 12/12, T2 342 segment lines and 0 CPU accesses in them, T3 339 lines with nothing missing
// MUTATION T2 stimulus red C6-stopself (STOP replaced by a self-branch loop, disk sequence unchanged), NMH_MUTATE=T2; DONE=1, K6, T1 and T3 hold; fx68k: 342 segment lines, 32014 CPU accesses in them; tg68k: 342 segment lines, 36269
// MUTATION T3 stimulus red C6-nospren (C6 without SPREN), NMH_MUTATE=T3; DONE=1, K6, T1 and T2 hold; both cores: 339 segment lines, bpl_missing=0, spr_missing=120, disk lines per frame 113/113/113; audio lines per frame fx68k 107/105/107, tg68k 106/106/106
// MUTATION P7 none impossible the Copper takes only the odd cycles and audio, disk, refresh, sprite and low-resolution plane DMA only the even ones (p.20), and no register or instruction moves one of those channels (audio, disk, refresh, sprite, low-resolution planes 1-4: p.20, Fig. 6-9, Fig. 6-11) to an odd cycle, so no program makes an even-cycle channel delay the Copper; the channels that take odd cycles (low-resolution planes 5 and 6, high-resolution planes, D2) lie outside P7's premise (at most four low-resolution planes), and changing the Copper's own list changes the comparison's inputs and not the clause
// MUTATION SELFTEST-P7 stimulus red C6' (list L5 with WAIT 32 at HP=$42 instead of $40; a self-test of p7_check, whose three mismatches come from the Copper's own list), NMH_MUTATE=SELFTEST-P7; both cores: C5 and C6' DONE=1, F1F2F3=111 for both, K6, T1, T2 and T3 hold, 3 mismatches of 339 segment/frame pairs
// MUTATION SELFTEST-F1 none impossible the check runs on a synthetic trace built in the test and no module stimulus enters it; it is a self-test of check_f1, printed in every plain run
// MUTATION SELFTEST-F2 none impossible the checks run on synthetic refresh groups built in the test (four slots, a moved slot, three, five) and no module stimulus enters them; it is a self-test of refresh_slots_ok and refresh_count_ok, printed in every plain run
// MUTATION SELFTEST-P1 none impossible the check runs on a synthetic trace built in the test (fourteen lines, the second refresh slot of one observed line on the third memory cycle) and no module stimulus enters it; it is a self-test of P1's comparator under P1's own gate, printed in every plain run
// MUTATION K2b-NOFINISH stimulus not graded C6-nofinish (C6 with the wait loop's exit condition removed: the completion word is missing), NMH_MUTATE=K2b-NOFINISH; both cores: the line's other runs meet their observation preconditions, C6-nofinish DONE=0 with F1=1 F2=1 F2-COUNT=1 F3=1, K2b prints NOT GRADED (C6-nofinish:C1)
// MUTATION K3-NOFINISH stimulus not graded C6-nofinish (C6 with the wait loop's exit condition removed: the completion word is missing), NMH_MUTATE=K3-NOFINISH; both cores: the line's other runs meet their observation preconditions, C6-nofinish DONE=0 with F1=1 F2=1 F2-COUNT=1 F3=1, K3 prints NOT GRADED (C6-nofinish:C1)
// MUTATION K4-NOFINISH stimulus not graded C6-nofinish (C6 with the wait loop's exit condition removed: the completion word is missing), NMH_MUTATE=K4-NOFINISH; both cores: the line's other runs meet their observation preconditions, C6-nofinish DONE=0 with F1=1 F2=1 F2-COUNT=1 F3=1, K4 prints NOT GRADED (C6-nofinish:C1)
// MUTATION K5-NOFINISH stimulus not graded C6-nofinish (C6 with the wait loop's exit condition removed: the completion word is missing), NMH_MUTATE=K5-NOFINISH; both cores: the line's other runs meet their observation preconditions, C6-nofinish DONE=0 with F1=1 F2=1 F2-COUNT=1 F3=1, K5 prints NOT GRADED (C6-nofinish:C1)
// MUTATION K6-NOFINISH stimulus not graded C6-nofinish (C6 with the wait loop's exit condition removed: the completion word is missing), NMH_MUTATE=K6-NOFINISH; both cores: the line's other runs meet their observation preconditions, C6-nofinish DONE=0 with F1=1 F2=1 F2-COUNT=1 F3=1, K6 prints NOT GRADED (C6-nofinish:C1)
// MUTATION T1-NOFINISH stimulus not graded C6-nofinish (C6 with the wait loop's exit condition removed: the completion word is missing), NMH_MUTATE=T1-NOFINISH; both cores: the line's other runs meet their observation preconditions, C6-nofinish DONE=0 with F1=1 F2=1 F2-COUNT=1 F3=1, T1 prints NOT GRADED (C6-nofinish:C1)
// MUTATION T2-NOFINISH stimulus not graded C6-nofinish (C6 with the wait loop's exit condition removed: the completion word is missing), NMH_MUTATE=T2-NOFINISH; both cores: the line's other runs meet their observation preconditions, C6-nofinish DONE=0 with F1=1 F2=1 F2-COUNT=1 F3=1, T2 prints NOT GRADED (C6-nofinish:C1)
// MUTATION T3-NOFINISH stimulus not graded C6-nofinish (C6 with the wait loop's exit condition removed: the completion word is missing), NMH_MUTATE=T3-NOFINISH; both cores: the line's other runs meet their observation preconditions, C6-nofinish DONE=0 with F1=1 F2=1 F2-COUNT=1 F3=1, T3 prints NOT GRADED (C6-nofinish:C1)
// MUTATION K2b-HTOTAL stimulus not graded C6-htotal (C6 with HTOTAL=$E0 and BEAMCON0=$00A0: every line is 225 memory cycles; S taken from the normal C0 run), NMH_MUTATE=K2b-HTOTAL; both cores: the line's other runs meet their observation preconditions, C6-htotal DONE=1 with F1=1 F2=1 F2-COUNT=1 F3=0, K2b prints NOT GRADED (C6-htotal:F3)
// MUTATION K3-HTOTAL stimulus not graded C6-htotal (C6 with HTOTAL=$E0 and BEAMCON0=$00A0: every line is 225 memory cycles; S taken from the normal C0 run), NMH_MUTATE=K3-HTOTAL; both cores: the line's other runs meet their observation preconditions, C6-htotal DONE=1 with F1=1 F2=1 F2-COUNT=1 F3=0, K3 prints NOT GRADED (C6-htotal:F3)
// MUTATION K4-HTOTAL stimulus not graded C6-htotal (C6 with HTOTAL=$E0 and BEAMCON0=$00A0: every line is 225 memory cycles; S taken from the normal C0 run), NMH_MUTATE=K4-HTOTAL; both cores: the line's other runs meet their observation preconditions, C6-htotal DONE=1 with F1=1 F2=1 F2-COUNT=1 F3=0, K4 prints NOT GRADED (C6-htotal:F3)
// MUTATION K5-HTOTAL stimulus not graded C6-htotal (C6 with HTOTAL=$E0 and BEAMCON0=$00A0: every line is 225 memory cycles; S taken from the normal C0 run), NMH_MUTATE=K5-HTOTAL; both cores: the line's other runs meet their observation preconditions, C6-htotal DONE=1 with F1=1 F2=1 F2-COUNT=1 F3=0, K5 prints NOT GRADED (C6-htotal:F3)
// MUTATION K6-HTOTAL stimulus not graded C6-htotal (C6 with HTOTAL=$E0 and BEAMCON0=$00A0: every line is 225 memory cycles; S taken from the normal C0 run), NMH_MUTATE=K6-HTOTAL; both cores: the line's other runs meet their observation preconditions, C6-htotal DONE=1 with F1=1 F2=1 F2-COUNT=1 F3=0, K6 prints NOT GRADED (C6-htotal:F3)
// MUTATION T1-HTOTAL stimulus not graded C6-htotal (C6 with HTOTAL=$E0 and BEAMCON0=$00A0: every line is 225 memory cycles; S taken from the normal C0 run), NMH_MUTATE=T1-HTOTAL; both cores: the line's other runs meet their observation preconditions, C6-htotal DONE=1 with F1=1 F2=1 F2-COUNT=1 F3=0, T1 prints NOT GRADED (C6-htotal:F3)
// MUTATION T2-HTOTAL stimulus not graded C6-htotal (C6 with HTOTAL=$E0 and BEAMCON0=$00A0: every line is 225 memory cycles; S taken from the normal C0 run), NMH_MUTATE=T2-HTOTAL; both cores: the line's other runs meet their observation preconditions, C6-htotal DONE=1 with F1=1 F2=1 F2-COUNT=1 F3=0, T2 prints NOT GRADED (C6-htotal:F3)
// MUTATION T3-HTOTAL stimulus not graded C6-htotal (C6 with HTOTAL=$E0 and BEAMCON0=$00A0: every line is 225 memory cycles; S taken from the normal C0 run), NMH_MUTATE=T3-HTOTAL; both cores: the line's other runs meet their observation preconditions, C6-htotal DONE=1 with F1=1 F2=1 F2-COUNT=1 F3=0, T3 prints NOT GRADED (C6-htotal:F3)
// MUTATION K2b-NORESTART stimulus not graded C6-norestart (C6 with COP1LC pointing at L5b: the word the run declares as its list start is never fetched), NMH_MUTATE=K2b-NORESTART; both cores: the line's other runs meet their observation preconditions, C6-norestart DONE=1 with no restart fetch found, K2b prints NOT GRADED (C6-norestart:restarts)
// MUTATION K3-NORESTART stimulus not graded C6-norestart (C6 with COP1LC pointing at L5b: the word the run declares as its list start is never fetched), NMH_MUTATE=K3-NORESTART; both cores: the line's other runs meet their observation preconditions, C6-norestart DONE=1 with no restart fetch found, K3 prints NOT GRADED (C6-norestart:restarts)
// MUTATION K4-NORESTART stimulus not graded C6-norestart (C6 with COP1LC pointing at L5b: the word the run declares as its list start is never fetched), NMH_MUTATE=K4-NORESTART; both cores: the line's other runs meet their observation preconditions, C6-norestart DONE=1 with no restart fetch found, K4 prints NOT GRADED (C6-norestart:restarts)
// MUTATION K5-NORESTART stimulus not graded C6-norestart (C6 with COP1LC pointing at L5b: the word the run declares as its list start is never fetched), NMH_MUTATE=K5-NORESTART; both cores: the line's other runs meet their observation preconditions, C6-norestart DONE=1 with no restart fetch found, K5 prints NOT GRADED (C6-norestart:restarts)
// MUTATION K6-NORESTART stimulus not graded C6-norestart (C6 with COP1LC pointing at L5b: the word the run declares as its list start is never fetched), NMH_MUTATE=K6-NORESTART; both cores: the line's other runs meet their observation preconditions, C6-norestart DONE=1 with no restart fetch found, K6 prints NOT GRADED (C6-norestart:restarts)
// MUTATION T1-NORESTART stimulus not graded C6-norestart (C6 with COP1LC pointing at L5b: the word the run declares as its list start is never fetched), NMH_MUTATE=T1-NORESTART; both cores: the line's other runs meet their observation preconditions, C6-norestart DONE=1 with no restart fetch found, T1 prints NOT GRADED (C6-norestart:restarts)
// MUTATION T2-NORESTART stimulus not graded C6-norestart (C6 with COP1LC pointing at L5b: the word the run declares as its list start is never fetched), NMH_MUTATE=T2-NORESTART; both cores: the line's other runs meet their observation preconditions, C6-norestart DONE=1 with no restart fetch found, T2 prints NOT GRADED (C6-norestart:restarts)
// MUTATION T3-NORESTART stimulus not graded C6-norestart (C6 with COP1LC pointing at L5b: the word the run declares as its list start is never fetched), NMH_MUTATE=T3-NORESTART; both cores: the line's other runs meet their observation preconditions, C6-norestart DONE=1 with no restart fetch found, T3 prints NOT GRADED (C6-norestart:restarts)
// MUTATION HELD-CONTROL none impossible unfalsifiable at this boundary: two genuine adjacent identical Copper accesses cannot be told from one held bus cycle in the RAM-port log, which carries no requester, so no stimulus produces the case; the control is the synthetic trace of this check
// MUTATION SELFTEST-VIDEO none impossible the check runs on synthetic captures built in the test (eight frames of 24 lines with two painted images, and the same capture with one pixel changed) and no module stimulus enters it; it is a self-test of used_output_frames, extract_images, hs_period and kv_from_periods, printed in every plain run
// MUTATION SELFTEST-Q3 none impossible the check runs on synthetic captures built in the test (the aligned frame, a frame with one image moved by one output line, a frame with an image of five output lines) and no module stimulus enters it; it is a self-test of q3_images_ok, printed in every plain run
// MUTATION FRAMES stimulus red S2-exit6 (S2 whose wait loop exits when FRAME_CTR reaches 6), NMH_MUTATE=FRAMES; both cores: DONE=1 in both runs, output frames used S2=2 and S1=3, FRAMES FAIL; Q2, Q3, Q4, Q5, Q-ref, K9, IMG4, P9, P10, P11, P10-P11 and P12 hold, P8 prints NOT GRADED (FRAMES)
// MUTATION Q2 stimulus red S1-q2 (S1 whose use 1 has the low data word of its first line at $7FFF, so the leftmost pixel is transparent), NMH_MUTATE=Q2; both cores: DONE=1, W=30 and p=0 on use 1's first line in each of the 3 frames used of S1, W=32 in S2, Q2 FAIL; FRAMES, Q3, Q4, Q5, Q-ref, K9 and IMG4 hold, P8 holds; P9, P10, P11, P10-P11 and P12 print NOT GRADED (Q2)
// MUTATION Q3 none impossible unfalsifiable at this boundary: a sprite's vertical start and stop are counts of whole beam lines (p.107, VSTART and VSTOP), so no stimulus puts an image edge between beam lines, and k_v is a property of the harness's line doubler and not of the chip set; the check's sensitivity is shown by SELFTEST-Q3
// MUTATION Q4 stimulus red S2-col0 (S2 with COLOR17 written as $0000 in both lists, equal to COLOR00), NMH_MUTATE=Q4; both cores: DONE=1, 6 colour writes fetched per port frame with COLOR17's equal to COLOR00 in each of the 3 frames, Q4 FAIL; FRAMES, Q2, Q3, Q5, Q-ref, K9, IMG4, P9, P10, P11, P10-P11 and P12 hold; P8 prints NOT GRADED (Q4), and its own comparator finds no sprite 0 image because use 1's first line is black
// MUTATION Q5 stimulus red S2-endv (S2 with END placed after V_spr in L_A), NMH_MUTATE=Q5; both cores: DONE=1, 35 of 54 words of L_A fetched in each of the 3 frames and no L_B frame, Q5 FAIL; Q2, Q3, Q4 and Q-ref FAIL with it because the colour MOVEs behind the END are never fetched, COLOR17-23 are never written and S2 shows no image (0 images in each frame); FRAMES, K9, IMG4, P9, P10, P11, P10-P11 and P12 hold; P8 prints NOT GRADED (Q2 Q3 Q4 Q5 Q-ref)
// MUTATION Q-ref stimulus red S2-noref (S2 whose list L_B points sprite 2 at the null block), NMH_MUTATE=Q-ref; both cores: DONE=1, sprite 2's image absent in the two L_B frames and at top 118, left 173 in the L_A frame, Q-ref FAIL; Q2 FAILs with it because the reference image whose first line it reads is absent in the same frames; FRAMES, Q3, Q4, Q5, K9, IMG4, P9, P10, P11, P10-P11 and P12 hold; P8 prints NOT GRADED (Q2 Q-ref)
// MUTATION K9 stimulus red S1-nospren (S1 without SPREN in the S4 write), NMH_MUTATE=K9; both cores: DONE=1, 0 reads in sprite block 0 in each of the 3 frames, K9 FAIL; Q2, Q3 and IMG4 FAIL with it because S1 shows no image (0 images in each frame); FRAMES, Q4, Q5, Q-ref hold and P8 holds; P9, P10, P11, P10-P11 and P12 print NOT GRADED (P9: Q2 Q3 K9; the others also IMG4)
// MUTATION IMG4 stimulus red S1-end (S1 whose use 2 control words are $0000,$0000), NMH_MUTATE=IMG4; both cores: DONE=1, 1 image per frame (use 1 only), IMG4 FAIL; P9 FAILs with it (its (a) needs four images and its (b) 90 reads, 20 are read); FRAMES, Q2, Q3, Q4, Q5, Q-ref, K9 hold and P8 holds; P10, P11, P10-P11 and P12 print NOT GRADED (IMG4)
// MUTATION P8 stimulus red S2-still (S2 whose list L_B also points sprite 0 at structure A), NMH_MUTATE=P8; both cores: DONE=1, offset (32 samples = 16 sprite pixels, 64 output lines = 32 beam lines) in each of the 3 frames, consecutive frames differ by (0, 0), P8 FAIL; FRAMES, Q2, Q3, Q4, Q5, Q-ref, K9, IMG4, P9, P10, P11, P10-P11 and P12 hold
// MUTATION P9 stimulus red S1-end (S1 whose use 2 control words are $0000,$0000, p.127), NMH_MUTATE=P9; both cores: DONE=1, 1 image per frame and 20 reads in sprite block 0 in each of the 3 frames, P9 FAIL; K9, Q2, Q3, Q4, Q5 hold; IMG4 FAILs with it (the count P9's (a) needs); P10, P11, P10-P11 and P12 print NOT GRADED (IMG4); P8 holds
// MUTATION P10 stimulus red S1-h (S1 whose use 2 has VSTOP $5C and four data pairs removed), NMH_MUTATE=P10; both cores: DONE=1, heights 16, 24, 8, 24 output lines (8, 12, 4, 12 beam lines) in each of the 3 frames, P10 FAIL; P11 holds (3456 cells, 0 off), P12 holds (2688 pixels, 0 off), IMG4 and the preconditions hold; P9 FAILs with it through its (b), which expects the 90 words of the specification's structure and reads 82, its (a) tops and left edges unchanged; P10-P11 FAILs with P10 (the aggregate)
// MUTATION P11 stimulus red S1-s (S1 whose use 2 data line 5 is $F0F0,$FF00), NMH_MUTATE=P11; both cores: DONE=1, 96 of 3840 cells off in the 3 frames (two output lines of 16 pixels per frame), P11 FAIL; P9 and P10 hold; P10-P11 FAILs with P11 (the aggregate); P12 FAILs with it (72 of 2976 pixels off) because it grades value-to-colour against the specification's data words, which line 5 no longer matches; the preconditions hold
// MUTATION P10-P11 stimulus red S1-h (S1 whose use 2 has VSTOP $5C and four data pairs removed), NMH_MUTATE=P10-P11; both cores: DONE=1, heights 8, 12, 4, 12 beam lines, P10 FAIL and so P10-P11 FAIL; P11 and P12 hold; P9 FAILs through its (b) (82 of 90 words read); the preconditions hold. S1-s (NMH_MUTATE=P11) fails the aggregate through P11
// MUTATION P12 stimulus red S1-c (S1 with list L_S1c: WAIT VP=$48 and COLOR17-19 <- $0FF0,$00FF,$0F0F between use 1 and use 2), NMH_MUTATE=P12; both cores: DONE=1, Q5 holds (52 of 52 words of L_S1c), uses 2 to 4 show $FF0, $0FF and $F0F where $F00, $0F0 and $00F are expected (2376 of 2976 pixels off), use 1 is unchanged, P12 FAIL; P9, P10, P11, P10-P11, the preconditions and P8 hold
//
// Not exercised, by a scope decision: P8-P12 in the NTSC/ECS, PAL/OCS and NTSC/OCS modes, and
// the blitter claims (see the RECORDED lines in the normal run).

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
        // ---- Every precondition of the line under test is evaluated and ---
        // ---- printed first, with the comparator the normal run uses; a ----
        // ---- precondition that fails leaves the line NOT GRADED. ----------
        if (mv == "A1") {
            // The same declaration table as the normal run: every reserved
            // block that is not the refresh set. Only the program differs (C1a
            // enables audio DMA, so accesses fall into the audio blocks).
            RunResult r = run_C1a(dir);
            set_gS(r.log, blocks_full(6), checks);
            auto pr = analyze_population(r.log, blocks_full(6), {wordof(COP_MAIN)}, "R", true);
            checks.note("A1 mutation (C1a): done=" + std::to_string(r.done.has_value()) +
                        " violations=" + std::to_string(pr.a1_violations));
            checks.expect(pr.restarts_ok && pr.a1_ok, "A1: in the baseline configuration C0, every access in the "
                          "observed frames is either in the refresh set R, the CPU block, or the Copper block. "
                          "SPECIFICATION.md 2.2");
        } else if (mv == "F3") {
            // F3: C0-htotal (S2 enables the programmable beam counters and
            // writes HTOTAL = $E0: every line is 225 memory cycles long). S is
            // the apparatus's own constant, a property of the memory cycle and
            // not of the line length: it is measured from the normal C0 run's
            // refresh trace, as in the normal run, and applied to the mutated
            // run unchanged.
            RunResult base = run_C0(dir);
            set_gS(base.log, blocks_C0(), checks);
            RunResult r = run_C0htotal(dir);
            auto pr = analyze_population(r.log, blocks_C0(), {wordof(COP_MAIN)}, "R", false);
            std::map<uint64_t, size_t> intervals; // A(n+1)-A(n) of the observed groups, in units of S (x10, to show a fraction)
            {
                auto groups = refresh_groups(r.log, blocks_C0());
                auto span = observed_span(restart_fetches(r.log, {wordof(COP_MAIN)}));
                for (size_t i = 0; i + 1 < groups.size(); ++i)
                    if (span.ok && groups[i][0] >= span.lo && groups[i][0] < span.hi)
                        ++intervals[(groups[i + 1][0] - groups[i][0]) * 10 / gS];
            }
            std::ostringstream iv;
            for (const auto& kv : intervals) iv << "[" << (kv.first / 10.0) << " S]x" << kv.second << " ";
            checks.note("C0-htotal: done=" + std::to_string(r.done.has_value()) + " S=" + std::to_string(gS) +
                        " groups=" + std::to_string(pr.groups_checked) + " F1=" + std::to_string(pr.f1_ok) +
                        " F2=" + std::to_string(pr.f2_ok) + " F2-COUNT=" + std::to_string(pr.f2count_ok) +
                        " F3=" + std::to_string(pr.f3_ok) + " line intervals=" + iv.str());
            std::string unmet = unmet_if(r.done.has_value(), "C0-htotal:C1") + unmet_if(pr.restarts_ok, "C0-htotal:restarts");
            if (!unmet.empty())
                not_graded(checks, "F3", unmet);
            else
                checks.expect(pr.f3_ok, "F3 C0-htotal: for every pair of consecutive refresh groups in the observed "
                              "frames, A(n+1)-A(n) equals 227*S or 228*S. p.23");
        } else if (mv == "K2a") {
            // C2a-earlyread: DSKBYTR is read while DSKLEN bit 15 is clear, the
            // transfer is started afterwards. The run's other lines are shown
            // unchanged: its observation, K2b's comparator and P2's population.
            RunResult r = run_C2a_earlyread(dir);
            set_gS(r.log, blocks_disk(), checks);
            Pop p{"C2a-earlyread", r, blocks_disk(), {wordof(COP_MAIN)}};
            uint16_t v = 0;
            bool ok = dskbytr_ok(r, true, &v);
            checks.note("C2a-earlyread: done=" + std::to_string(r.done.has_value()) + " DSKBYTR=" + hex(v));
            observe(checks, p);
            auto n = disk_frame_counts(p, plain_disk_range());
            auto pd = analyze_population(r.log, p.blocks, p.starts, "Disk", false);
            checks.note("C2a-earlyread: K2b comparator disk-block accesses per observed frame=" + frame_counts_text(n) +
                        " P2 population=" + std::to_string(pd.population) + " all_even=" + std::to_string(pd.all_even));
            checks.expect(ok, "K2a: the stored DSKBYTR has DMAON=1 and DISKWRITE=1 in C2a and C6, DISKWRITE=0 in "
                          "C2b. p.248 Appendix A p.278");
        } else if (mv == "K2b") {
            // C2a-dskpt: the transfer runs (DMAON=1, so K2a holds) with DSKPT
            // outside the declared disk block.
            // The moved transfer is declared as a block of its own (DiskMoved), so
            // its accesses do not join the refresh set.
            RunResult r = run_C2a_dskpt(dir);
            set_gS(r.log, blocks_disk_moved(), checks);
            Pop p{"C2a-dskpt", r, blocks_disk_moved(), {wordof(COP_MAIN)}};
            uint16_t v = 0;
            bool k2a = dskbytr_ok(r, true, &v);
            auto n = disk_frame_counts(p, plain_disk_range());
            auto pm = analyze_population(r.log, p.blocks, p.starts, "DiskMoved", false);
            std::string unmet = observe(checks, p);
            checks.note("C2a-dskpt: done=" + std::to_string(r.done.has_value()) + " K2a (unchanged comparator)=" +
                        std::to_string(k2a) + " DSKBYTR=" + hex(v) +
                        " disk-block accesses per observed frame=" + frame_counts_text(n) +
                        " moved-transfer accesses=" + std::to_string(pm.population) + " all_even=" + std::to_string(pm.all_even));
            if (unmet.empty())
                checks.expect(all_frames_nonempty(n), "K2b: the disk block receives accesses in each observed frame of "
                              "C2a and C6. p.246 p.247 p.194");
            else
                not_graded(checks, "K2b", unmet);
        } else if (mv == "K3") {
            RunResult r = run_C1a(dir, true);
            set_gS(r.log, blocks_audio(), checks);
            Pop p{"C1a-noaud0", r, blocks_audio(), {wordof(COP_MAIN)}};
            checks.note("C1a-noaud0: done=" + std::to_string(r.done.has_value()));
            std::string unmet = observe(checks, p);
            bool k3 = k3_check(p, plain_audio_ranges(), checks);
            if (unmet.empty())
                checks.expect(k3, "K3: each of the four audio channels has accesses in each observed frame of C1a, "
                              "C1b and C6. p.144");
            else
                not_graded(checks, "K3", unmet);
        } else if (mv == "K4") {
            RunResult r = run_C3(dir, true);
            set_gS(r.log, blocks_sprite(), checks);
            Pop p{"C3-nospren", r, blocks_sprite(), {wordof(COP_MAIN)}};
            checks.note("C3-nospren: done=" + std::to_string(r.done.has_value()));
            std::string unmet = observe(checks, p);
            bool k4 = k4_check(p, sprite_structures_c3(), checks);
            if (unmet.empty())
                checks.expect(k4, "K4: in each observed frame of C3 and C6, every word of each of the eight sprite "
                              "structures (control, data and end words) is read at least once. p.108");
            else
                not_graded(checks, "K4", unmet);
        } else if (mv == "K5") {
            RunResult r = run_C4(dir, 4, false, true);
            set_gS(r.log, blocks_bitplane(4), checks);
            Pop p{"C4-4-nobplen", r, blocks_bitplane(4), {wordof(COP_MAIN)}};
            checks.note("C4-4-nobplen: done=" + std::to_string(r.done.has_value()));
            std::string unmet = observe(checks, p);
            bool k5 = k5_check(p, plane_names(4), checks);
            if (unmet.empty())
                checks.expect(k5, "K5: every enabled plane has fetches in at least as many lines of every observed "
                              "frame as the display window is high (C4-1..C4-6, C4-4w, C6). p.59 Appendix A p.277");
            else
                not_graded(checks, "K5", unmet);
        } else if (mv == "K6") {
            // C6-endtrailer: the trailer ends the list instead of jumping to L5b.
            // The run's other lines are shown unchanged: T1 (against C5), T2, T3
            // and P7's comparator.
            RunResult c5 = run_C5(dir);
            RunResult r = run_C6_endtrailer(dir);
            set_gS(c5.log, blocks_full(4), checks);
            Pop p5{"C5", c5, blocks_full(4), {wordof(COP_MAIN)}};
            Pop p{"C6-endtrailer", r, blocks_full(4), {wordof(COP_MAIN)}};
            checks.note("C5: done=" + std::to_string(c5.done.has_value()) + " C6-endtrailer: done=" +
                        std::to_string(r.done.has_value()));
            std::string unmet = observe(checks, p);
            bool k6 = k6_check(p, checks);
            t1_check(c5, p5.blocks, r, p.blocks, p.name, checks);
            t2_check(r, p.blocks, checks, p.name);
            t3_check(r, p.blocks, checks);
            p7_check(c5, p5.blocks, r, p.blocks, checks);
            if (unmet.empty())
                checks.expect(k6, "K6: in every observed frame of C5, C6, C7a and C7b, every word of L5 and L5b is "
                              "read at least once (the list ran to its end).");
            else
                not_graded(checks, "K6", unmet);
        } else if (mv == "T1") {
            // C6-skip50: the Copper repeats WAIT 50's line with the SKIP loop of p.33.
            // The run's other lines are shown unchanged: K6, T2, T3 and P7's comparator.
            RunResult c5 = run_C5(dir);
            RunResult c6x = run_C6_skip50(dir);
            set_gS(c5.log, blocks_full(4), checks);
            Pop p5{"C5", c5, blocks_full(4), {wordof(COP_MAIN)}};
            Pop p{"C6-skip50", c6x, blocks_full(4), {wordof(COP_MAIN)}};
            checks.note("C5: done=" + std::to_string(c5.done.has_value()) + " C6-skip50: done=" +
                        std::to_string(c6x.done.has_value()));
            std::string unmet = observe(checks, p5) + observe(checks, p);
            k6_check(p, checks);
            t2_check(c6x, p.blocks, checks, p.name);
            t3_check(c6x, p.blocks, checks);
            p7_check(c5, p5.blocks, c6x, p.blocks, checks);
            bool t1 = t1_check(c5, p5.blocks, c6x, p.blocks, p.name, checks);
            if (unmet.empty())
                checks.expect(t1, "T1: every segment has the same number of Copper accesses in C5 and C6.");
            else
                not_graded(checks, "T1", unmet);
        } else if (mv == "T2") {
            // C6-stopself against C5; the run's other lines are shown unchanged.
            RunResult c5 = run_C5(dir);
            RunResult r = run_C6(dir, "/prog_c6_stopself.bin");
            set_gS(c5.log, blocks_full(4), checks);
            Pop p5{"C5", c5, blocks_full(4), {wordof(COP_MAIN)}};
            Pop p{"C6-stopself", r, blocks_full(4), {wordof(COP_MAIN)}};
            checks.note("C6-stopself: done=" + std::to_string(r.done.has_value()));
            std::string unmet = observe(checks, p5) + observe(checks, p);
            k6_check(p, checks);
            t1_check(c5, p5.blocks, r, p.blocks, p.name, checks);
            t3_check(r, p.blocks, checks);
            bool t2 = t2_check(c5, p5.blocks, checks, "C5");
            t2 = t2_check(r, p.blocks, checks, p.name) && t2;
            if (unmet.empty())
                checks.expect(t2, "T2: no CPU-block access lies in any line spanned by a segment, in C5 or C6.");
            else
                not_graded(checks, "T2", unmet);
        } else if (mv == "T3") {
            // C6-nospren against C5; the run's other lines are shown unchanged.
            RunResult c5 = run_C5(dir);
            RunResult r = run_C6(dir, "/prog_c6_nospren.bin");
            set_gS(c5.log, blocks_full(4), checks);
            Pop p5{"C5", c5, blocks_full(4), {wordof(COP_MAIN)}};
            Pop p{"C6-nospren", r, blocks_full(4), {wordof(COP_MAIN)}};
            checks.note("C6-nospren: done=" + std::to_string(r.done.has_value()));
            std::string unmet = observe(checks, p);
            k6_check(p, checks);
            t1_check(c5, p5.blocks, r, p.blocks, p.name, checks);
            t2_check(r, p.blocks, checks, p.name);
            bool t3 = t3_check(r, p.blocks, checks);
            if (unmet.empty())
                checks.expect(t3, "T3: in the lines of the segments in C6, bitplane fetches occur in every line; "
                              "sprite fetches occur in every line of segments 16-35 and 64-83 (VP $50-$63/$80-$93); audio "
                              "and disk accesses each occur in the segment lines of every observed frame.");
            else
                not_graded(checks, "T3", unmet);
        } else if (mv == "SELFTEST-P7") {
            RunResult c5 = run_C5(dir);
            RunResult c6p = run_C6prime(dir);
            set_gS(c5.log, blocks_full(4), checks);
            checks.note("C5: done=" + std::to_string(c5.done.has_value()) + " C6': done=" + std::to_string(c6p.done.has_value()));
            Pop pc5{"C5", c5, blocks_full(4), {wordof(COP_MAIN)}}, pc6p{"C6'", c6p, blocks_full(4), {wordof(COP_MAIN)}};
            std::string unmet;
            for (const Pop* q : {&pc5, &pc6p}) {
                auto pr = analyze_population(q->run.log, q->blocks, q->starts, "R", false);
                checks.note("SELFTEST-P7 " + q->name + ": F1F2F3=" + std::to_string(pr.f1_ok) +
                            std::to_string(pr.f2_ok) + std::to_string(pr.f3_ok));
                unmet += observation_unmet(q->name, q->run, pr);
            }
            bool k6a = k6_check(pc5, checks), k6b = k6_check(pc6p, checks);
            bool t1 = t1_check(c5, pc5.blocks, c6p, pc6p.blocks, "C6'", checks);
            bool t2a = t2_check(c5, pc5.blocks, checks, "C5"), t2b = t2_check(c6p, pc6p.blocks, checks, "C6'");
            bool t3 = t3_check(c6p, pc6p.blocks, checks);
            unmet += unmet_if(k6a && k6b, "K6") + unmet_if(t1, "T1") + unmet_if(t2a && t2b, "T2") + unmet_if(t3, "T3");
            bool p7 = p7_check(c5, pc5.blocks, c6p, pc6p.blocks, checks);
            if (unmet.empty())
                checks.expect(p7, "SELFTEST-P7: p7_check on C5 and C6' (the same comparator as P7, run on a list whose "
                              "WAIT 32 sits at HP $42 instead of $40) finds the segment lists identical. p.22");
            else
                not_graded(checks, "SELFTEST-P7", unmet);
        } else if (mv == "FIXTURE-PEEK") {
            // C0 whose wait loop exits at frame 6: DONE is reached, and the
            // stored FRAME_CTR is 6 where the readback expects 7.
            RunResult r = run_config(dir + "/prog_c0_exit6.bin", nmh::Config{}, nopoke, {FRAME_CTR});
            bool ok = !r.peeks.empty() && r.peeks[0] && *r.peeks[0] == 7;
            checks.note("C0-exit6: done=" + std::to_string(r.done.has_value()) + " FRAME_CTR (peek_logical) = " +
                        (!r.peeks.empty() && r.peeks[0] ? hex(*r.peeks[0]) : std::string("?")));
            checks.expect(ok, "FIXTURE-PEEK: a program-stored constant (FRAME_CTR = 7) is read back through "
                          "peek_logical before K2a relies on the same path for DSKBYTR.");
        } else if (mv == "FIXTURE") {
            RunResult r = run_C0_nofinish(dir);
            checks.note("C0-nofinish: loaded=" + std::to_string(r.loaded) + " released=" + std::to_string(r.released) +
                        " done=" + std::to_string(r.done.has_value()));
            checks.expect(r.loaded && r.released && r.done.has_value(),
                          "FIXTURE: every configuration's program loaded, released reset, and reached DONE");
        } else if (gate_selector(mv)) {
            // <line>-NOFINISH / -HTOTAL / -NORESTART: the line's gate (SPECIFICATION.md 2.11) with C6
            // replaced by a run in which C1 (the completion word), F3 (the line length) or the
            // restart identification (the list start the run declares is never fetched) fails.
            // S is taken from the plain C0 run, as in F3's contrast. The line's other runs are
            // the plain ones, built as in the normal run; the line prints NOT GRADED.
            const bool nofinish = mv.size() > 9 && mv.compare(mv.size() - 9, 9, "-NOFINISH") == 0;
            const bool norestart = mv.size() > 10 && mv.compare(mv.size() - 10, 10, "-NORESTART") == 0;
            const std::string line = mv.substr(0, mv.size() - (nofinish ? 9 : norestart ? 10 : 7));
            RunResult base = run_C0(dir);
            set_gS(base.log, blocks_C0(), checks);
            const std::vector<uint32_t> st = {wordof(COP_MAIN)};
            auto plain = [&](const std::string& n) -> Pop {
                if (n == "C1a") return {n, run_C1a(dir), blocks_audio(), st};
                if (n == "C1b") return {n, run_C1b(dir), blocks_audio(), st};
                if (n == "C2a") return {n, run_C2a(dir), blocks_disk(), st};
                if (n == "C3") return {n, run_C3(dir), blocks_sprite(), st};
                if (n == "C4-4w") return {n, run_C4(dir, 4, true), blocks_bitplane(4), st};
                if (n.rfind("C4-", 0) == 0) return {n, run_C4(dir, n[3] - '0'), blocks_bitplane(n[3] - '0'), st};
                if (n == "C5") return {n, run_C5(dir), blocks_full(4), st};
                if (n == "C7a") return {n, run_C7a(dir), blocks_bitplane(6), st};
                return {n, run_C7b(dir), blocks_bitplane(4), st};
            };
            Pop px{nofinish ? "C6-nofinish" : norestart ? "C6-norestart" : "C6-htotal",
                   nofinish ? run_C6_nofinish(dir) : norestart ? run_C6_norestart(dir) : run_C6_htotal(dir),
                   blocks_full(4), st};
            std::vector<Pop> runs; // the line's runs in the order of the normal run, the contrast in C6's place
            auto add = [&](const char* n) { runs.push_back(std::string(n) == "C6" ? px : plain(n)); };
            if (line == "K2b") { add("C2a"); add("C6"); }
            else if (line == "K3") { add("C1a"); add("C1b"); add("C6"); }
            else if (line == "K4") { add("C3"); add("C6"); }
            else if (line == "K5") { for (const char* n : {"C4-1", "C4-2", "C4-3", "C4-4", "C4-5", "C4-6", "C4-4w", "C6"}) add(n); }
            else if (line == "K6") { add("C5"); add("C6"); add("C7a"); add("C7b"); }
            else if (line == "T3") { add("C6"); }
            else { add("C5"); add("C6"); } // T1, T2
            std::string unmet;
            for (const auto& q : runs) unmet += observe(checks, q);
            if (line == "K2b") {
                auto n2 = disk_frame_counts(runs[0], plain_disk_range()), n6 = disk_frame_counts(px, plain_disk_range());
                checks.note("K2b: disk-block accesses per observed frame C2a=" + frame_counts_text(n2) + " " + px.name + "=" + frame_counts_text(n6));
                if (unmet.empty()) checks.expect(all_frames_nonempty(n2) && all_frames_nonempty(n6),
                    "K2b: the disk block receives accesses in each observed frame of C2a and C6. p.246 p.247 p.194");
            } else if (line == "K3") {
                bool k3 = true;
                for (const auto& q : runs) k3 = k3_check(q, plain_audio_ranges(), checks) && k3;
                if (unmet.empty()) checks.expect(k3, "K3: each of the four audio channels has accesses in each observed frame of C1a, "
                    "C1b and C6. p.144");
            } else if (line == "K4") {
                bool k4 = true;
                for (const auto& q : runs) k4 = k4_check(q, sprite_structures_c3(), checks) && k4;
                if (unmet.empty()) checks.expect(k4, "K4: in each observed frame of C3 and C6, every word of each of the eight sprite "
                    "structures (control, data and end words) is read at least once. p.108");
            } else if (line == "K5") {
                bool k5 = true;
                for (const auto& q : runs) {
                    int np = q.name == "C4-4w" ? 4 : q.name == px.name ? 4 : q.name[3] - '0';
                    k5 = k5_check(q, plane_names(np), checks) && k5;
                }
                if (unmet.empty()) checks.expect(k5, "K5: every enabled plane has fetches in at least as many lines of every observed "
                    "frame as the display window is high (C4-1..C4-6, C4-4w, C6). p.59 Appendix A p.277");
            } else if (line == "K6") {
                bool k6 = true;
                for (const auto& q : runs) k6 = k6_check(q, checks) && k6;
                if (unmet.empty()) checks.expect(k6, "K6: in every observed frame of C5, C6, C7a and C7b, every word of L5 and L5b is "
                    "read at least once (the list ran to its end).");
            } else if (line == "T1") {
                bool t1 = t1_check(runs[0].run, runs[0].blocks, px.run, px.blocks, px.name, checks);
                if (unmet.empty()) checks.expect(t1, "T1: every segment has the same number of Copper accesses in C5 and C6.");
            } else if (line == "T2") {
                bool t2 = t2_check(runs[0].run, runs[0].blocks, checks, "C5");
                t2 = t2_check(px.run, px.blocks, checks, px.name) && t2;
                if (unmet.empty()) checks.expect(t2, "T2: no CPU-block access lies in any line spanned by a segment, in C5 or C6.");
            } else {
                bool t3 = t3_check(px.run, px.blocks, checks);
                if (unmet.empty()) checks.expect(t3, "T3: in the lines of the segments in C6, bitplane fetches occur in every line; "
                    "sprite fetches occur in every line of segments 16-35 and 64-83 (VP $50-$63/$80-$93); audio "
                    "and disk accesses each occur in the segment lines of every observed frame.");
            }
            if (!unmet.empty()) not_graded(checks, line, unmet);
        } else if (video_selector(mv)) {
            // The video lines (P8-P12 and their preconditions): the line named by the selector is
            // the graded line, the others print as MEASURED. S2 and S1 are built as the normal run
            // builds them, except that the named contrast replaces one of them. S is taken from the
            // plain C0 run.
            RunResult base = run_C0(dir);
            set_gS(base.log, blocks_C0(), checks);
            std::string s2bin = "prog_s2.bin", s1bin = "prog_s1.bin";
            uint32_t s1_words = kCopWordsS1;
            if (mv == "FRAMES") s2bin = "prog_s2_exit6.bin";
            else if (mv == "Q2") s1bin = "prog_s1_q2.bin";
            else if (mv == "Q4") s2bin = "prog_s2_col0.bin";
            else if (mv == "Q5") s2bin = "prog_s2_endv.bin";
            else if (mv == "Q-ref") s2bin = "prog_s2_noref.bin";
            else if (mv == "K9") s1bin = "prog_s1_nospren.bin";
            else if (mv == "IMG4" || mv == "P9") s1bin = "prog_s1_end.bin";
            else if (mv == "P8") s2bin = "prog_s2_still.bin";
            else if (mv == "P10" || mv == "P10-P11") s1bin = "prog_s1_h.bin";
            else if (mv == "P11") s1bin = "prog_s1_s.bin";
            else if (mv == "P12") { s1bin = "prog_s1_c.bin"; s1_words = kCopWordsS1 + 8; }
            auto runs = make_video_runs(dir, s2bin, s1bin, s1_words);
            checks.note("video mutation " + mv + ": S2 " + s2bin + " done=" + std::to_string(runs.first.v.run.done.has_value()) +
                        ", S1 " + s1bin + " done=" + std::to_string(runs.second.v.run.done.has_value()) +
                        ", S=" + std::to_string(gS));
            video_lines(checks, runs.first, runs.second, mv);
        } else {
            checks.expect(false, "unknown NMH_MUTATE value: " + mv);
        }
        std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
        return checks.failed == 0 ? 0 : 1;
    }

    // ---- normal run: build every configuration once. Every precondition is
    // ---- evaluated and printed before the line that depends on it; a line
    // ---- whose precondition fails is NOT GRADED.
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
    // SPECIFICATION.md 2.10 mode variants of C0, C6 and S1 (P1 only):
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

    auto find = [&](const char* name) -> const Pop* {
        for (auto& p : all) if (p.name == name) return &p;
        return nullptr;
    };
    const Pop* c0 = find("C0");

    // S (SPECIFICATION.md 2.1): measured once, from C0's own refresh trace.
    {
        if (c0) set_gS(c0->run.log, c0->blocks, checks);
        else { checks.note("S measurement: no C0 run found"); gS = 1; }
        checks.note("S measured (most common refresh-group interval / 227, p.23) = " + std::to_string(gS));
    }

    // C1 (program finished) and the readback path, before any line that reads a run.
    bool fixture_ok = true;
    for (auto& p : all) fixture_ok = fixture_ok && p.run.loaded && p.run.released && p.run.done.has_value();
    checks.expect(fixture_ok, "FIXTURE: every configuration's program loaded, released reset, and reached DONE");

    {
        // The FIXTURE line above proves only that DONE was written. K2a
        // reads DSKBYTR through peek_logical; this proves that path can
        // read a program-stored constant first (FRAME_CTR = 7, the S6
        // loop's own exit value).
        bool ok = c0 && !c0->run.peeks.empty() && c0->run.peeks[0] && *c0->run.peeks[0] == 7;
        checks.note("FIXTURE peek: C0's FRAME_CTR (peek_logical) = " +
                    (c0 && !c0->run.peeks.empty() && c0->run.peeks[0] ? hex(*c0->run.peeks[0]) : std::string("?")));
        checks.expect(ok, "FIXTURE-PEEK: a program-stored constant (FRAME_CTR = 7) is read back through "
                      "peek_logical before K2a relies on the same path for DSKBYTR.");
    }

    // F1, F2, F2-COUNT, F3: their own printed, graded lines per run, before P1
    // (SPECIFICATION.md 2.1/2.3). F1 is scoped to DMA accesses (D5); the
    // CPU's own phase is measured, not graded, on a line of its own. A run
    // that did not finish, or whose restart fetches do not delimit the
    // observed frames, leaves its four lines NOT GRADED.
    for (auto& p : all) {
        auto pr = analyze_population(p.run.log, p.blocks, p.starts, "R", false);
        {
            std::ostringstream pat;
            for (const auto& kv : pr.slot_patterns) pat << "[" << kv.first << "]x" << kv.second << " ";
            checks.note("F2 " + p.name + ": groups=" + std::to_string(pr.groups_checked) +
                        " offsets(memory cycles from the first access)=" + pat.str());
        }
        std::string unmet = unmet_if(p.run.done.has_value(), p.name + ":C1") + unmet_if(pr.restarts_ok, p.name + ":restarts");
        if (!unmet.empty()) { not_graded(checks, "F1 F2 F2-COUNT F3 " + p.name, unmet); continue; }
        checks.expect(pr.f1_ok, "F1 " + p.name + ": the run's DMA accesses lie on one S-cycle phase (the CPU's own "
                      "phase is measured, not graded here, D5). SPECIFICATION.md 2.1");
        checks.expect(pr.f2_ok, "F2 " + p.name + ": every refresh group in the observed frames has its four refresh "
                      "slots at 0, 2, 4 and 6 memory cycles from its first access. p.194 Fig. 6-9 p.195");
        checks.expect(pr.f2count_ok, "F2-COUNT " + p.name + ": every refresh group in the observed frames holds exactly "
                      "four entries, p.194's \"4 cycles for memory refresh\". p.194 Fig. 6-9 p.195");
        checks.expect(pr.f3_ok, "F3 " + p.name + ": for every pair of consecutive refresh groups in the observed "
                      "frames, A(n+1)-A(n) equals 227*S or 228*S. p.23");
    }
    {
        // CPU phase relative to the DMA grid (F1's own scope, D5):
        // measured, not graded -- no page gives the 680x0 a phase at
        // these ports.
        std::set<uint64_t> phases;
        if (c0) {
            LineRef lref = make_lines(c0->run.log, c0->blocks);
            auto restarts = restart_fetches(c0->run.log, {wordof(COP_MAIN)});
            auto span = observed_span(restarts);
            if (span.ok)
                for (const auto& a : c0->run.log)
                    if (a.cycle >= span.lo && a.cycle < span.hi && attribute(a.word_addr, c0->blocks) == "CPU")
                        phases.insert(a.cycle % gS);
        }
        std::ostringstream ph; for (auto ps : phases) ph << ps << ";";
        checks.note("CPU phase mod S (C0, D5, not graded by F1): " + ph.str());
    }

    // Self-tests of the comparators on synthetic traces. They run no module
    // and move nothing in a recorded run: each shows that the checker named
    // fails on a trace that violates what it checks, and passes on one that
    // does not.
    {
        constexpr uint64_t kSyntheticS = 8;
        const uint64_t saved_gS = gS;
        gS = kSyntheticS;

        // F1: one access moved off the S-cycle grid by one time unit.
        std::vector<nmh::RamAccess> good = {
            {1000, 0x1000, 0, false, true, true},
            {1000 + kSyntheticS, 0x1002, 0, false, true, true},
        };
        std::vector<nmh::RamAccess> bad = good;
        bad[1].cycle += 1;
        bool f1_good = check_f1(good, {}, 0, 2000), f1_bad = check_f1(bad, {}, 0, 2000);
        checks.note("SELFTEST-F1: on-grid trace F1=" + std::to_string(f1_good) + ", one access moved by 1 time unit F1=" +
                    std::to_string(f1_bad));
        checks.expect(f1_good && !f1_bad, "SELFTEST-F1: check_f1 passes a trace on one S-cycle phase and fails the "
                      "same trace with one access moved by one time unit. SPECIFICATION.md 2.1");

        // F2 and F2-COUNT: a group of four slots at 0, 2, 4, 6, then the second slot moved to 3, a
        // group of three, and a group of five (the four slots and a fifth entry at 8).
        std::vector<uint64_t> g_good = {0, 2 * kSyntheticS, 4 * kSyntheticS, 6 * kSyntheticS};
        std::vector<uint64_t> g_moved = {0, 3 * kSyntheticS, 4 * kSyntheticS, 6 * kSyntheticS};
        std::vector<uint64_t> g_three = {0, 2 * kSyntheticS, 4 * kSyntheticS};
        std::vector<uint64_t> g_five = {0, 2 * kSyntheticS, 4 * kSyntheticS, 6 * kSyntheticS, 8 * kSyntheticS};
        bool f2_good = refresh_slots_ok(g_good, kSyntheticS), f2_moved = refresh_slots_ok(g_moved, kSyntheticS),
             f2_three = refresh_slots_ok(g_three, kSyntheticS);
        bool c_good = refresh_count_ok(g_good), c_three = refresh_count_ok(g_three), c_five = refresh_count_ok(g_five);
        checks.note("SELFTEST-F2: slots 0,2,4,6 F2=" + std::to_string(f2_good) + ", slot 2 moved to 3 F2=" +
                    std::to_string(f2_moved) + ", three slots F2=" + std::to_string(f2_three) +
                    "; F2-COUNT: four entries=" + std::to_string(c_good) + ", three=" + std::to_string(c_three) +
                    ", five=" + std::to_string(c_five) + " (F2 on the five: " +
                    std::to_string(refresh_slots_ok(g_five, kSyntheticS)) + ")");
        checks.expect(f2_good && !f2_moved && !f2_three && c_good && !c_three && !c_five,
                      "SELFTEST-F2: refresh_slots_ok passes four slots at 0, 2, 4 and 6 and fails a moved slot and a "
                      "group of three; refresh_count_ok passes a group of four and fails a group of three and a group "
                      "of five. p.194 Fig. 6-9 p.195");

        // P1 without F2 in its gate: fourteen lines of 227 memory cycles, each a refresh group at 0,
        // 2, 4 and 6, and a restart fetch of the Copper's list in every second line. In the
        // displaced trace the second slot of one observed line is at 3 instead of 2. C1, the
        // restarts, F1, F3 and F2-COUNT hold in both traces; F2 fails in the displaced one.
        auto p1_trace = [&](bool displaced) {
            RunResult run;
            run.loaded = run.released = true;
            run.done = 1;
            for (int n = 0; n < 14; ++n) {
                const uint64_t t0 = (100 + (uint64_t)n * 227) * kSyntheticS;
                const uint64_t slot[4] = {0, (displaced && n == 8) ? 3u : 2u, 4, 6};
                for (uint64_t off : slot) run.log.push_back({t0 + off * kSyntheticS, 0, 0, false, true, true});
                if (n % 2 == 1) run.log.push_back({t0 + 20 * kSyntheticS, wordof(COP_MAIN), 0, false, true, true});
            }
            return Pop{displaced ? "synthetic-displaced" : "synthetic-regular", run, {copper_block()}, {wordof(COP_MAIN)}};
        };
        const Pop p1_reg = p1_trace(false), p1_dis = p1_trace(true);
        const std::vector<Pop> p1_all = {p1_reg, p1_dis};
        const Fold f_reg = fold_check(p1_all, {"synthetic-regular"}, "R", true, checks, "SELFTEST-P1 regular", false);
        const Fold f_dis = fold_check(p1_all, {"synthetic-displaced"}, "R", true, checks, "SELFTEST-P1 displaced", false);
        const Fold f_old = fold_check(p1_all, {"synthetic-displaced"}, "R", true, checks, "SELFTEST-P1 old gate", true);
        bool p1_reg_gate = f_reg.unmet.empty(), p1_dis_gate = f_dis.unmet.empty(), p1_dis_old_gate = f_old.unmet.empty();
        bool p1_reg_ok = f_reg.ok, p1_dis_ok = f_dis.ok;
        checks.note("SELFTEST-P1: regular trace gate (C1, restarts, F1, F3) met=" + std::to_string(p1_reg_gate) +
                    " P1 comparator=" + std::to_string(p1_reg_ok) + "; displaced trace gate met=" +
                    std::to_string(p1_dis_gate) + " P1 comparator=" + std::to_string(p1_dis_ok) +
                    " gate with F2 and F2-COUNT met=" + std::to_string(p1_dis_old_gate));
        checks.expect(p1_reg_gate && p1_reg_ok && p1_dis_gate && !p1_dis_ok && !p1_dis_old_gate,
                      "SELFTEST-P1: P1's comparator under the observation gate P1 has (C1, restarts, F1, F3; not F2 or "
                      "F2-COUNT) passes a synthetic trace and fails the same trace with the second refresh slot of one "
                      "line on an odd cycle, which F2 also rejects. p.20 p.194 Fig. 6-9 p.195");

        // Held cycle: a word sampled in two consecutive memory cycles is one
        // access; the same word two memory cycles apart, a different datum, a
        // different direction and different byte enables are separate ones.
        auto acc = [](uint64_t cyc, uint32_t w, uint16_t d, bool wr, bool up, bool lo) {
            return nmh::RamAccess{cyc, w, d, wr, up, lo};
        };
        const uint64_t P = kSyntheticS;
        std::vector<nmh::RamAccess> held_log = {
            acc(1000, 0x20, 0xAAAA, false, true, true),
            acc(1000 + P, 0x20, 0xAAAA, false, true, true),     // held: same bus cycle
            acc(1000 + 2 * P, 0x20, 0xAAAA, false, true, true), // still held (chain)
            acc(1000 + 4 * P, 0x20, 0xAAAA, false, true, true), // two memory cycles later: separate
            acc(1000 + 5 * P, 0x20, 0xBBBB, false, true, true), // other datum: separate
            acc(1000 + 6 * P, 0x20, 0xBBBB, true, true, true),  // other direction: separate
            acc(1000 + 7 * P, 0x20, 0xBBBB, true, true, false), // other byte enables: separate
            acc(1000 + 8 * P + 1, 0x20, 0xBBBB, true, true, false), // not exactly one memory cycle: separate
        };
        std::vector<bool> held = nmh::held_continuations(held_log, P);
        std::vector<bool> want = {false, true, true, false, false, false, false, false};
        auto kept = nmh::coalesce_adjacent(held_log, P);
        bool held_ok = held == want && kept.size() == 6 && kept[0].cycle == 1000;
        checks.note("HELD-CONTROL: continuations marked=" + std::to_string(std::count(held.begin(), held.end(), true)) +
                    " of " + std::to_string(held_log.size()) + ", kept=" + std::to_string(kept.size()));
        checks.expect(held_ok, "HELD-CONTROL: dropping held continuations keeps the first sample of a held bus "
                      "cycle and keeps identical samples that are two memory cycles apart, or differ in datum, "
                      "direction, byte enables or by a fraction of a memory cycle.");

        gS = saved_gS;
    }

    // The video apparatus' own self-tests, before any proposition uses the apparatus.
    video_selftests(checks);

    std::vector<std::string> names_all;
    for (auto& p : all) names_all.push_back(p.name);

    // A1 (precondition of P1-P5): graded before the first line that depends on it.
    bool a1 = false;
    {
        a1 = a1_holds(*c0, checks);
        checks.expect(a1, "A1: in the baseline configuration C0, every access in the "
                      "observed frames is either in the refresh set R, the CPU block, or the Copper block. "
                      "SPECIFICATION.md 2.2");
    }

    // The observation preconditions (C1, restarts, F1, F2, F2-COUNT, F3) of the named
    // runs, concatenated: the gate of every line that grades those runs (SPECIFICATION.md 2.11).
    auto gate = [&](std::initializer_list<const char*> names) {
        std::string u;
        for (const char* n : names) {
            const Pop* q = find(n);
            u += q ? run_unmet(*q) : std::string(n) + ":MISSING ";
        }
        return u;
    };

    // P1: its preconditions are C1, restarts, F1, F3 (per run) and A1. F2 and F2-COUNT
    // grade the accesses P1 grades, so they do not gate it (SPECIFICATION.md P1.5).
    {
        Fold f = fold_check(all, names_all, "R", true, checks, "P1", false);
        std::string unmet = f.unmet + unmet_if(a1, "A1");
        if (unmet.empty())
            checks.expect(f.ok, "P1: every refresh access lies on an even in-line cycle, in every run and mode graded "
                          "(C0, C1a, C1b, C2a, C2b, C3, C4-1..6, C4-4w, C5, C6, C7a, C7b, S1, S2 in PAL/ECS; C0, C6 and S1 "
                          "also in NTSC/ECS, PAL/OCS and NTSC/OCS). ch2-about-the-copper.c24 COP2-about-the-copper.c29 p.20");
        else
            not_graded(checks, "P1", unmet);
    }

    // K2a/K2b (preconditions of P2) and P2 itself, with D9's own carve-out:
    // if K2b (disk-block accesses in each observed frame) holds for C2a and C6
    // but not C2b (no drive attached), the read direction is declined, not
    // graded red.
    {
        uint16_t va = 0, vb = 0, vc = 0;
        const Pop *c2a = find("C2a"), *c2b = find("C2b"), *c6 = find("C6");
        bool c2a_dskbytr = dskbytr_ok(c2a->run, true, &va);
        bool c2b_dskbytr = dskbytr_ok(c2b->run, false, &vb);
        bool c6_dskbytr = dskbytr_ok(c6->run, true, &vc);
        bool k2a = c2a_dskbytr && c2b_dskbytr && c6_dskbytr;
        checks.note("K2a: DSKBYTR C2a=" + hex(va) + " C2b=" + hex(vb) + " C6=" + hex(vc));
        checks.expect(k2a, "K2a: the stored DSKBYTR has DMAON=1 and "
                      "DISKWRITE=1 in C2a and C6, DISKWRITE=0 in C2b. p.248 Appendix A p.278");

        // K2b: the disk block is active, with accesses in each observed frame.
        // p.194 allocates at most 3 disk cycles to a line and p.246 gives the
        // transfer 8000 words, so the transfer outlasts the frames observed.
        auto cnt_c2a = disk_frame_counts(*c2a, plain_disk_range());
        auto cnt_c2b = disk_frame_counts(*c2b, plain_disk_range());
        auto cnt_c6 = disk_frame_counts(*c6, plain_disk_range());
        bool k2b_c2a = all_frames_nonempty(cnt_c2a), k2b_c2b = all_frames_nonempty(cnt_c2b),
             k2b_c6 = all_frames_nonempty(cnt_c6);
        checks.note("K2b: disk-block accesses per observed frame C2a=" + frame_counts_text(cnt_c2a) + " C2b=" +
                    frame_counts_text(cnt_c2b) + " C6=" + frame_counts_text(cnt_c6));
        std::string u2b = gate({"C2a", "C6"});
        if (u2b.empty())
            checks.expect(k2b_c2a && k2b_c6, "K2b: the disk block receives accesses in each observed frame of C2a "
                          "and C6. p.246 p.247 p.194");
        else
            not_graded(checks, "K2b", u2b);
        if (!k2b_c2b)
            checks.record("K2b/D9: C2b (read direction, no drive attached) shows " + frame_counts_text(cnt_c2b) +
                           " disk-block accesses in the observed frames; the read direction is not exercised, "
                           "not graded. p.250");
        else
            checks.expect(k2b_c2b, "K2b: the disk block receives accesses in each observed frame of C2b. p.246 p.247 p.194");

        std::vector<std::string> p2_names = {"C2a", "C6"};
        if (k2b_c2b) p2_names.push_back("C2b");
        Fold f = fold_check(all, p2_names, "Disk", true, checks, "P2");
        std::string unmet = f.unmet + unmet_if(a1, "A1") + unmet_if(k2a, "K2a") +
                            unmet_if(u2b.empty() && k2b_c2a && k2b_c6, "K2b");
        if (unmet.empty())
            checks.expect(f.ok, "P2: every disk-DMA access lies on an even in-line cycle (" +
                          (k2b_c2b ? std::string("C2a, C2b, C6") : std::string("C2a, C6; C2b's read direction declined, D9")) +
                          ").");
        else
            not_graded(checks, "P2", unmet);
        checks.record("ch2-about-the-copper.c23 COP2-about-the-copper.c28: only the write direction (C2a, C6) is "
                       "graded here; the disk-to-memory read direction (C2b) shows " + frame_counts_text(cnt_c2b) +
                       " disk-block accesses per observed frame and is declined (D9), so the ids' own \"only\" "
                       "is not established for both directions. p.20");
    }

    // K3 (precondition of P3): each of the four channels is active, with
    // accesses in each observed frame of C1a, C1b and C6 (p.144: DMA starts
    // for channel x when DMAEN and AUDxEN are set).
    {
        bool k3 = true; std::string u3;
        for (const char* cfgname : {"C1a", "C1b", "C6"}) {
            const Pop* p = find(cfgname);
            k3 = k3_check(*p, plain_audio_ranges(), checks) && k3;
            u3 += run_unmet(*p);
        }
        if (u3.empty())
            checks.expect(k3, "K3: each of the four audio channels has accesses in each observed frame of C1a, "
                          "C1b and C6. p.144");
        else
            not_graded(checks, "K3", u3);
        Fold f = fold_check(all, {"C1a", "C1b", "C6"}, "Audio", true, checks, "P3");
        std::string unmet = f.unmet + unmet_if(a1, "A1") + unmet_if(u3.empty() && k3, "K3");
        if (unmet.empty())
            checks.expect(f.ok, "P3: every audio-DMA access, all four channels, lies on an even in-line cycle (C1a, C1b, "
                          "C6). ch2-about-the-copper.c22 COP2-about-the-copper.c27 p.20");
        else
            not_graded(checks, "P3", unmet);
    }

    // K4 (precondition of P4): in each observed frame of C3 and C6, every
    // word of each of the eight sprite structures is read at least once.
    {
        const Pop *c3 = find("C3"), *c6k4 = find("C6");
        bool k4 = k4_check(*c3, sprite_structures_c3(), checks);
        k4 = k4_check(*c6k4, sprite_structures_c3(), checks) && k4;
        std::string u4 = gate({"C3", "C6"});
        if (u4.empty())
            checks.expect(k4, "K4: in each observed frame of C3 and C6, every word of each of the eight sprite "
                          "structures (control, data and end words) is read at least once. p.108");
        else
            not_graded(checks, "K4", u4);
        Fold f = fold_check(all, {"C3", "C6", "S1", "S2"}, "Sprite", true, checks, "P4");
        std::string unmet = f.unmet + unmet_if(a1, "A1") + unmet_if(u4.empty() && k4, "K4");
        if (unmet.empty())
            checks.expect(f.ok, "P4: every sprite-DMA access, all eight channels, lies on an even in-line cycle (C3, C6, "
                          "S1, S2). ch2-about-the-copper.c25 COP2-about-the-copper.c30 p.20");
        else
            not_graded(checks, "P4", unmet);
    }

    // K5 (precondition of P5) and P5 (C4-1..C4-6, C4-4w, C6).
    {
        std::vector<std::pair<std::string, int>> p5cfgs = {
            {"C4-1", 1}, {"C4-2", 2}, {"C4-3", 3}, {"C4-4", 4}, {"C4-5", 5}, {"C4-6", 6}, {"C4-4w", 4}, {"C6", 4}};
        bool k5 = true; std::string u5k;
        for (auto& [cfgname, n] : p5cfgs) {
            const Pop* p = find(cfgname.c_str());
            k5 = k5_check(*p, plane_names(n), checks) && k5;
            u5k += run_unmet(*p);
        }
        if (u5k.empty())
            checks.expect(k5, "K5: every enabled plane has fetches in at least as many lines of every observed frame "
                          "as the display window is high (C4-1..C4-6, C4-4w, C6). p.59 Appendix A p.277");
        else
            not_graded(checks, "K5", u5k);
        bool p5a = true, p5b = true;
        std::string unmet = unmet_if(a1, "A1") + unmet_if(u5k.empty() && k5, "K5");
        for (auto& [cfgname, n] : p5cfgs) {
            const Pop* p = find(cfgname.c_str());
            p5a = p5a_check(*p, n, checks, unmet) && p5a;
            p5b = p5b_check(*p, checks) && p5b;
        }
        if (unmet.empty())
            checks.expect(p5a && p5b, "P5: (a) every access in bitplanes 1-4 lies on an even in-line cycle, in every "
                          "config with 1-6 low-resolution planes and at both fetch starts, and in C6; (b) in every "
                          "line with bitplane fetches, even-cycle fetches outnumber odd-cycle ones. "
                          "ch2-about-the-copper.c26 COP2-about-the-copper.c31 p.20");
        else
            not_graded(checks, "P5", unmet);
    }

    // K6 (precondition of P7): in every observed frame of C5, C6, C7a
    // and C7b, every word of L5 and L5b is read at least once.
    const Pop *c5p = find("C5"), *c6p = find("C6"), *c7ap = find("C7a"), *c7bp = find("C7b");
    bool k6 = true; std::string u6;
    for (const Pop* p : {c5p, c6p, c7ap, c7bp}) {
        k6 = k6_check(*p, checks) && k6;
        u6 += run_unmet(*p);
    }
    if (u6.empty())
        checks.expect(k6, "K6: in every observed frame of C5, C6, C7a and C7b, every word of L5 and L5b is read "
                      "at least once (the list ran to its end).");
    else
        not_graded(checks, "K6", u6);

    // P6 is withdrawn: not graded, not mutated.
    checks.record("P6 withdrawn, ch2-about-the-copper.c21: not asserted. p.20 says the Copper requests the bus only "
                   "during odd-numbered memory cycles, and the same sentence bundles the absence of collision and five "
                   "DMA channels. The Chip-memory port shows accesses, and an access carries no requester: it cannot tell "
                   "a Copper request from a transfer held for a second memory cycle, nor a Copper access from another "
                   "channel's access to the same word, and naming the requester from Agnus's internal signals would "
                   "make the implementation its own oracle. The Copper-block accesses on an even in-line cycle and the "
                   "first Copper fetch after S4 are MEASURED below, not graded. P7 (the Copper's cycles do not move "
                   "when the even-cycle channels run) observes neither the requests nor the collisions, and leaves the "
                   "disk-to-memory direction undone (D9). p.20");
    {
        // The Copper-block reads on an even in-line cycle, per run: how many
        // there are, how many continue the sample directly before them, how
        // many do not, and the in-line cycle numbers they fall on.
        std::ostringstream m;
        for (auto& p : all) {
            auto info = measure_p6_even_reads(p.run, p.blocks);
            if (info.raw_even == 0) continue;
            m << p.name << ":even_raw=" << info.raw_even << ",held_continuations=" << info.held_even
              << ",even_remaining=" << info.remaining_even << ",s={";
            bool first = true;
            for (auto s : info.s_values) { m << (first ? "" : ",") << s; first = false; }
            m << "};";
        }
        checks.note("P6 (withdrawn) Copper-block reads on an even in-line cycle: " + m.str());
        // Samples that repeat the sample before them at half a memory cycle
        // (the other sampling phase): the held-cycle rule does not apply there.
        size_t half_repeats = 0;
        for (auto& p : all) {
            const std::vector<bool> held_half = nmh::held_continuations(p.run.log, gS / 2);
            for (size_t i = 0; i < p.run.log.size(); ++i)
                if (held_half[i] && attribute(p.run.log[i].word_addr, p.blocks) == "Copper") ++half_repeats;
        }
        checks.note("P6 (withdrawn) Copper-block samples that repeat the sample before them at S/2, all runs = " +
                    std::to_string(half_repeats));
    }
    {
        // The Copper's first fetch after S4, in the runs whose block tables
        // declare the Copper block: which memory cycle of its line it falls
        // on. Not graded: the port cannot tell the Copper's request from a held transfer.
        std::string m;
        for (const char* name : {"C0", "C7a", "S2", "C2a"}) m += copper_first_fetch_text(*find(name)) + "; ";
        checks.note("Copper first fetch after S4 (memory-cycle parity of its line, not graded): " + m);
    }

    // P7 and its own preconditions K6 (above), T1, T2, T3 (C5 vs C6).
    // Reuses the runs already built into `all` above -- no re-simulation.
    {
        const RunResult& c5 = c5p->run;
        const RunResult& c6 = c6p->run;

        // T1-T3 are gated by the observation preconditions of the runs they grade.
        std::string ut12 = gate({"C5", "C6"}), ut3 = gate({"C6"});

        bool t1 = t1_check(c5, c5p->blocks, c6, c6p->blocks, "C6", checks);
        if (ut12.empty())
            checks.expect(t1, "T1: every segment has the same number of Copper accesses in C5 and C6.");
        else
            not_graded(checks, "T1", ut12);

        bool t2 = t2_check(c5, c5p->blocks, checks, "C5");
        t2 = t2_check(c6, c6p->blocks, checks, "C6") && t2;
        if (ut12.empty())
            checks.expect(t2, "T2: no CPU-block access lies in any line spanned by a segment, in C5 or C6.");
        else
            not_graded(checks, "T2", ut12);

        bool t3 = t3_check(c6, c6p->blocks, checks);
        if (ut3.empty())
            checks.expect(t3, "T3: in the lines of the segments in C6, bitplane fetches occur in every line; sprite "
                          "fetches occur in every line of segments 16-35 and 64-83 (VP $50-$63/$80-$93); audio and "
                          "disk accesses each occur in the segment lines of every observed frame.");
        else
            not_graded(checks, "T3", ut3);

        bool p7 = p7_check(c5, c5p->blocks, c6, c6p->blocks, checks);
        std::string unmet = unmet_if(u6.empty() && k6, "K6") + unmet_if(ut12.empty() && t1, "T1") +
                            unmet_if(ut12.empty() && t2, "T2") + unmet_if(ut3.empty() && t3, "T3") + ut12;
        if (unmet.empty())
            checks.expect(p7, "P7: for every segment i (i=0..112) and every observed frame, the list of (line "
                          "offset, s) pairs is identical in C5 and C6 -- the channels on the even cycles never delay "
                          "the Copper. p.20");
        else
            not_graded(checks, "P7", unmet);
    }

    // P8-P12 (sprite channel reuse) and the video preconditions: S2 and S1 read through the video
    // apparatus (output frames from vs, k_v, Q2-Q5, the image extraction), printed in the order the
    // gates need.
    {
        auto runs = make_video_runs(dir, "prog_s2.bin", "prog_s1.bin", kCopWordsS1);
        video_lines(checks, runs.first, runs.second, "");
        checks.record("P8-P12 reading of the page: P8's \"at least 3\" is read as at least three output frames used (so at least two "
                       "consecutive pairs); \"normally\" in P12 is read as the page's own exception, a Copper write "
                       "between two uses (the next sentence of p.27), so the line grades the definite colour values of the registers "
                       "with no such write and the contrast writes the colours between the uses; P8-P12 are graded in PAL/ECS, "
                       "the mode variants of S1 are graded for P1 and P4 only.");
    }
    checks.record("COP2-about-the-copper.c34 .c35 .c36 (the blitter DMA channel): not built, by a scope decision. "
                   "The parenthesis they come from names a use (animation, line drawing, polygon filling), not a "
                   "machine function this package's claims define an observable for; a check would need pp.184-186 "
                   "and pp.189-192, pages outside this package's own claims.");

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
