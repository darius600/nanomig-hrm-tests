// test_ch2_dma_cycles.cpp -- ch2-dma-cycles. See SPECIFICATION.md for every
// assertion's proposition, program, observed value and expected value. This
// file only implements what SPECIFICATION.md already committed to.
//
// Every configuration's Copper list and every audio/disk/sprite/bitplane
// block's CONTENT is poked into chip memory by this file, through
// NanomigHarness::poke_word/poke_long, before boot() -- gary.v's DMA-side
// address decode (the `dbr` branch, gary.v:143-156) carries no `ovl` term,
// so Agnus reaches chip block 0 ($000000-$07FFFF) regardless of the
// overlay; a CPU write to the same range is blocked while ovl==1
// (gary.v:159), and this package never clears the overlay (CIA-A is never
// touched). Each configuration's own prog_*.s only points a pointer
// register at the block address (an ordinary custom-register write,
// unaffected by ovl either way) and enables DMA -- see ch2_skel.i.
//
// NMH_MUTATE selects a mutated capture (a different poked list/structure,
// a different declared block, or a different program) in place of a
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

// amiga_clk.v: clk7_cnt is a mod-4 counter of clk_sys (`clk28m`); cck
// toggles once every time clk7_cnt==1, i.e. once every 4 clk_sys, so one
// full colour-clock period (one memory cycle, Fig. 6-9's "cycle") is 8
// clk_sys -- verified directly against amiga_clk.v, not assumed, and the
// divider has no PAL/NTSC term, so this holds in every mode (D4).
constexpr uint64_t kS = 8;

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

constexpr uint32_t kHandlerAddr = nmh::kProgramOrg + 2; // ch2_skel.i's PROG_HEADER

// Every program's own code/vectors/stack/results (attribution table,
// SPECIFICATION.md 2.2): the identity range the harness maps 1:1
// ($080000-$1FFFFF, nmh::kChipIdentityLo/Hi) plus the kickstart-shadow
// vector-table reads the harness itself makes (physically >= that too,
// see nmh::kKickShadowByteBase). Every declared DMA block in this package
// is placed below $080000 (ch2dma.i), so this single threshold separates
// "CPU block" from everything else without listing sub-ranges.
constexpr uint32_t kCpuWordThreshold = 0x080000 / 2;

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

// ---- word/byte helpers --------------------------------------------------
uint32_t wordof(uint32_t byte_addr) { return byte_addr / 2; }
uint16_t hi16(uint32_t v) { return uint16_t(v >> 16); }
uint16_t lo16(uint32_t v) { return uint16_t(v & 0xFFFF); }

void poke_words(nmh::NanomigHarness& h, uint32_t byte_addr, const std::vector<uint16_t>& ws) {
    uint32_t w = wordof(byte_addr);
    for (size_t i = 0; i < ws.size(); ++i) h.poke_word(w + (uint32_t)i, ws[i]);
}

void install_handler_vector(nmh::NanomigHarness& h) {
    // Level-3 (VERTB) autovector, address $6C, in the kickstart shadow --
    // exactly where the harness itself writes the two reset vectors
    // (kKickShadowWordBase+0/+2), never through CIA-A/ovl.
    h.poke_long(nmh::kKickShadowWordBase + 0x6C / 2, kHandlerAddr);
}

// ---- Copper-list / structure builders (SPECIFICATION.md 2.8-2.9) -------

void emit_move(std::vector<uint16_t>& o, uint16_t reg, uint16_t data) { o.push_back(reg); o.push_back(data); }
void emit_movel(std::vector<uint16_t>& o, uint16_t regH, uint16_t regL, uint32_t addr) {
    emit_move(o, regH, hi16(addr));
    emit_move(o, regL, lo16(addr));
}
void emit_end(std::vector<uint16_t>& o) { o.push_back(0xFFFF); o.push_back(0xFFFE); }

void emit_Vspr(std::vector<uint16_t>& o, const std::array<uint32_t, 8>& ptrs) {
    for (int x = 0; x < 8; ++x) emit_movel(o, uint16_t(SPR0PTH + 4 * x), uint16_t(SPR0PTH + 2 + 4 * x), ptrs[x]);
}
void emit_Vbpl(std::vector<uint16_t>& o, int n, const std::array<uint32_t, 6>& ptrs) {
    for (int y = 0; y < n; ++y) emit_movel(o, uint16_t(BPL1PTH + 4 * y), uint16_t(BPL1PTH + 2 + 4 * y), ptrs[y]);
}

std::vector<uint16_t> list_L0() { std::vector<uint16_t> o; emit_end(o); return o; }

std::vector<uint16_t> list_L3(const std::array<uint32_t, 8>& spr) {
    std::vector<uint16_t> o; emit_Vspr(o, spr); emit_end(o); return o;
}

std::vector<uint16_t> list_L4(int n, const std::array<uint32_t, 6>& bpl) {
    std::vector<uint16_t> o; emit_Vbpl(o, n, bpl); emit_end(o); return o;
}

std::vector<uint16_t> list_L5b() {
    std::vector<uint16_t> o;
    o.push_back(0xB821); o.push_back(0xFFFE); // WAIT VP=$B8,HP=$20
    for (int i = 0; i < 4; ++i) { o.push_back(0x0180); o.push_back(0x0000); }
    emit_end(o);
    return o;
}

// L5 (SPECIFICATION.md 2.8). override_i/override_hp implement L5' (P7's
// own contrast): the WAIT for i==override_i uses override_hp instead of
// 2*i as its HP field.
std::vector<uint16_t> list_L5(const std::array<uint32_t, 8>& spr, const std::array<uint32_t, 6>& bpl,
                               uint32_t l5b_addr, int override_i = -1, uint16_t override_hp = 0,
                               int extra_after_wait = -1) {
    std::vector<uint16_t> o;
    emit_Vspr(o, spr);
    emit_Vbpl(o, 6, bpl);
    emit_movel(o, COP2LCH, COP2LCL, l5b_addr);
    for (int i = 0; i <= 112; ++i) {
        uint16_t hp = (i == override_i) ? override_hp : uint16_t(2 * i);
        uint16_t w1 = uint16_t(((0x40 + i) << 8) | hp | 1);
        o.push_back(w1); o.push_back(0xFFFE);
        for (int k = 0; k < 3; ++k) { o.push_back(0x0180); o.push_back(0x0000); }
        if (i == extra_after_wait) { o.push_back(0x0180); o.push_back(0x0000); } // T1's contrast
    }
    o.push_back(0xB401); o.push_back(0xFFFE);
    o.push_back(0xB441); o.push_back(0xFFFF);
    o.push_back(0x0180); o.push_back(0x0000);
    o.push_back(COPJMP2); o.push_back(0x0000);
    return o;
}

std::vector<uint16_t> list_L0u_or_m(uint32_t u_addr, uint16_t wait_word) {
    std::vector<uint16_t> o;
    emit_movel(o, COP2LCH, COP2LCL, u_addr);
    o.push_back(wait_word); o.push_back(0xFFFE);
    o.push_back(COPJMP2); o.push_back(0x0000);
    return o;
}

std::vector<uint16_t> list_U() {
    std::vector<uint16_t> o;
    o.push_back(0x0180); o.push_back(0x0000);
    emit_end(o);
    return o;
}

std::vector<uint16_t> list_L_S1(uint32_t z_addr, uint32_t null_addr, uint32_t plane1_addr, bool colours_variant) {
    std::array<uint32_t, 8> spr = {z_addr, null_addr, null_addr, null_addr, null_addr, null_addr, null_addr, null_addr};
    std::array<uint32_t, 6> bpl = {plane1_addr, 0, 0, 0, 0, 0};
    std::vector<uint16_t> o;
    emit_Vspr(o, spr);
    emit_Vbpl(o, 1, bpl);
    emit_move(o, COLOR17, 0x0F00);
    emit_move(o, COLOR18, 0x00F0);
    emit_move(o, COLOR19, 0x000F);
    if (colours_variant) { // L_S1c
        o.push_back(0x4801); o.push_back(0xFFFE); // WAIT VP=$48
        emit_move(o, COLOR17, 0x0FF0);
        emit_move(o, COLOR18, 0x00FF);
        emit_move(o, COLOR19, 0x0F0F);
    }
    emit_end(o);
    return o;
}

std::vector<uint16_t> list_L_AB(uint32_t spr0_addr, uint32_t r_addr, uint32_t null_addr,
                                 uint32_t plane1_addr, uint32_t other_list_addr, bool r_absent = false,
                                 uint16_t color17_override = 0x0F00) {
    std::array<uint32_t, 8> spr = {spr0_addr, null_addr, r_absent ? null_addr : r_addr, null_addr,
                                    null_addr, null_addr, null_addr, null_addr};
    std::array<uint32_t, 6> bpl = {plane1_addr, 0, 0, 0, 0, 0};
    std::vector<uint16_t> o;
    emit_Vspr(o, spr);
    emit_Vbpl(o, 1, bpl);
    emit_move(o, COLOR17, color17_override); emit_move(o, COLOR18, 0x00F0); emit_move(o, COLOR19, 0x000F);
    emit_move(o, COLOR21, 0x0FF0); emit_move(o, COLOR22, 0x00FF); emit_move(o, COLOR23, 0x0F0F);
    emit_movel(o, COP1LCH, COP1LCL, other_list_addr);
    emit_end(o);
    return o;
}

std::vector<uint16_t> structure_X() {
    std::vector<uint16_t> o = {0x5050, 0x5800};
    for (int i = 0; i < 7; ++i) { o.push_back(0x0180); o.push_back(0x0000); }
    o.push_back(0xFFFF); o.push_back(0xFFFE);
    o.push_back(0x0000); o.push_back(0x0000);
    return o;
}

// Q5's own contrast (P8.6): END placed right after V_spr, so the rest of
// L_A (including its own COP1LC rewrite) is never fetched.
std::vector<uint16_t> list_L_A_truncated(uint32_t spr0_addr, uint32_t r_addr, uint32_t null_addr) {
    std::array<uint32_t, 8> spr = {spr0_addr, null_addr, r_addr, null_addr, null_addr, null_addr, null_addr, null_addr};
    std::vector<uint16_t> o;
    emit_Vspr(o, spr);
    emit_end(o);
    return o;
}

std::vector<uint16_t> list_L3x(uint32_t x_addr, uint32_t null_addr) {
    std::array<uint32_t, 8> spr = {x_addr, null_addr, null_addr, null_addr, null_addr, null_addr, null_addr, null_addr};
    std::vector<uint16_t> o;
    emit_Vspr(o, spr);
    emit_movel(o, COP2LCH, COP2LCL, x_addr + 4);
    o.push_back(0xA001); o.push_back(0xFFFE);
    o.push_back(COPJMP2); o.push_back(0x0000);
    return o;
}

enum class ZMut { None, EndAfterU1, ShortU2Height, SwapU2Line5 };

std::vector<uint16_t> structure_Z(ZMut mut = ZMut::None) {
    std::vector<uint16_t> o;
    auto emit_use = [&](uint16_t c1, uint16_t c2, int height, uint16_t low, uint16_t high, int swap_line = -1) {
        o.push_back(c1); o.push_back(c2);
        o.push_back(0xFFFF); o.push_back(0x0000); // line 1, fixed
        for (int i = 2; i <= height; ++i) {
            if (i == swap_line) { o.push_back(0xF0F0); o.push_back(0xFF00); } // S1-s: U1's own pattern
            else { o.push_back(low); o.push_back(high); }
        }
    };
    emit_use(0x4050, 0x4800, 8, 0xF0F0, 0xFF00);
    if (mut == ZMut::EndAfterU1) { o.push_back(0x0000); o.push_back(0x0000); return o; }
    if (mut == ZMut::ShortU2Height) emit_use(0x5064, 0x5C00, 12, 0x0F0F, 0x00FF);
    else emit_use(0x5064, 0x6000, 16, 0x0F0F, 0x00FF, mut == ZMut::SwapU2Line5 ? 5 : -1);
    emit_use(0x6848, 0x6C00, 4, 0xCCCC, 0xF0F0);
    emit_use(0x7878, 0x8400, 12, 0xAAAA, 0xCCCC);
    o.push_back(0x0000); o.push_back(0x0000);
    return o;
}

std::vector<uint16_t> null_block() { return {0x0000, 0x0000}; }

// C3's own per-channel structure (SPECIFICATION.md 2.9): two 20-line uses,
// HSTART = $70 + $18*x, common to both uses of channel x.
std::vector<uint16_t> structure_C3(int x) {
    uint16_t hstart = uint16_t(0x70 + 0x18 * x);
    uint16_t ctrl1a = uint16_t((0x50 << 8) | (hstart >> 1));
    uint16_t ctrl2a = uint16_t((0x64 << 8) | (hstart & 1));
    uint16_t ctrl1b = uint16_t((0x80 << 8) | (hstart >> 1));
    uint16_t ctrl2b = uint16_t((0x94 << 8) | (hstart & 1));
    std::vector<uint16_t> o = {ctrl1a, ctrl2a};
    for (int i = 0; i < 20; ++i) { o.push_back(0xAAAA); o.push_back(0x5555); }
    o.push_back(ctrl1b); o.push_back(ctrl2b);
    for (int i = 0; i < 20; ++i) { o.push_back(0xAAAA); o.push_back(0x5555); }
    o.push_back(0x0000); o.push_back(0x0000);
    return o;
}

// ---- attribution & reference-frame machinery (SPECIFICATION.md 2.2-2.4) -

struct Block { std::string name; uint32_t lo, hi; }; // word-address half-open range

std::string attribute(uint32_t word_addr, const std::vector<Block>& blocks) {
    if (word_addr >= kCpuWordThreshold) return "CPU";
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
        if (r[i] - r[i - 1] > 16 * kS) ref.firsts.push_back(r[i]);
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
    return (int64_t)((cycle - ref.firsts[(size_t)line_idx]) / kS);
}

// Section 2.4: restart fetches are reads of the word at any of
// `list_starts` (word addresses). Frame k = [restart[k-1], restart[k])
// (0-indexed restart[0] is restart #1). Observed = frames 3,4,5 ->
// [restart[2], restart[5]).
std::vector<uint64_t> restart_fetches(const std::vector<nmh::RamAccess>& log, const std::vector<uint32_t>& list_starts) {
    std::vector<uint64_t> out;
    for (const auto& a : log)
        if (!a.is_write)
            for (auto s : list_starts) if (a.word_addr == s) { out.push_back(a.cycle); break; }
    std::sort(out.begin(), out.end());
    return out;
}

struct ObservedSpan { bool ok; uint64_t lo, hi; };
ObservedSpan observed_span(const std::vector<uint64_t>& restarts) {
    if (restarts.size() < 6) return {false, 0, 0};
    return {true, restarts[2], restarts[5]};
}

// ---- generic per-population parity check (P1-P4, P6) -------------------

struct ParityResult {
    bool f1_ok = false, f2_ok = false, f3_ok = false, a1_ok = false, restarts_ok = false;
    bool all_even = true, all_odd = true;
    size_t population = 0;
    size_t a1_violations = 0;
};

// F1: S integer (given, kS), single phase across the observed population.
// Excludes the CPU block: D5 keeps the CPU off the bus wherever a check
// could depend on it, but the level-3 handler still runs a handful of
// instructions right at each frame's own restart (both are vblank-
// triggered), which can fall inside the observed span; the CPU's own bus
// request is a separate, asynchronously-arbitrated path from every DMA
// master's (ch6-blitter-cpu-sharing's own finding: CPU and DMA differ by
// a half slot at the raw port), so F1 -- like every other check here --
// is a DMA-population precondition.
bool check_f1(const std::vector<nmh::RamAccess>& log, const std::vector<Block>& blocks,
               uint64_t obs_lo, uint64_t obs_hi) {
    std::optional<uint64_t> phi;
    for (const auto& a : log) {
        if (a.cycle < obs_lo || a.cycle >= obs_hi) continue;
        if (attribute(a.word_addr, blocks) == "CPU") continue;
        uint64_t p = a.cycle % kS;
        if (!phi) phi = p;
        else if (*phi != p) return false;
    }
    return true;
}

// Evaluates every access of `target_block` within the observed frames:
// F2/F3 (on the refresh-derived line grid), A1 (only meaningful for the
// baseline run, but harmless elsewhere), and the even/odd population.
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

    std::vector<uint64_t> r_cycles;
    for (const auto& a : log) if (!a.is_write && attribute(a.word_addr, blocks) == "R") r_cycles.push_back(a.cycle);
    std::sort(r_cycles.begin(), r_cycles.end());
    std::vector<std::pair<uint64_t, size_t>> groups; // (A_n, count)
    if (!r_cycles.empty()) {
        uint64_t start = r_cycles[0]; size_t cnt = 1;
        for (size_t i = 1; i < r_cycles.size(); ++i) {
            if (r_cycles[i] - r_cycles[i - 1] > 16 * kS) { groups.push_back({start, cnt}); start = r_cycles[i]; cnt = 0; }
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
            if (gap != 227 * kS && gap != 228 * kS) f3 = false;
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
    install_handler_vector(h);
    r.released = h.boot(kBootBudget).cpu_released;
    r.done = h.run_until_write(kDoneAddr, kDoneValue, kRunBudget);
    // Not coalesced: coalesce() would also merge two genuinely separate
    // accesses that happen to share an address/data pair (refresh
    // revisiting the same address with unchanged data is exactly such a
    // case -- ch6-blitter-cpu-sharing's own test file notes this), which
    // this package's line/slot grouping depends on being distinct.
    r.log = h.ram_log();
    for (auto a : peek_addrs) r.peeks.push_back(h.peek_logical(a));
    return r;
}

// ---- configuration C0 (baseline) and its C0-u contrast ------------------

RunResult run_C0(const std::string& dir) {
    return run_config(dir + "/prog_c0.bin", nmh::Config{}, [](nmh::NanomigHarness& h) {
        poke_words(h, COP_MAIN, list_L0());
    });
}

RunResult run_C0u(const std::string& dir) {
    return run_config(dir + "/prog_c0.bin", nmh::Config{}, [](nmh::NanomigHarness& h) {
        poke_words(h, COP_MAIN, list_L0u_or_m(CH_U_SUBLIST, 0x8005));
        poke_words(h, CH_U_SUBLIST, list_U());
    });
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

std::array<uint32_t, 8> sprite_ptrs_seq() {
    std::array<uint32_t, 8> a;
    for (int x = 0; x < 8; ++x) a[x] = CH_SPR[x];
    return a;
}
std::array<uint32_t, 8> sprite_ptrs_null() {
    std::array<uint32_t, 8> a; a.fill(CH_SPR_NULL); return a;
}
std::array<uint32_t, 6> bpl_ptrs_seq(int n) {
    std::array<uint32_t, 6> a; a.fill(0);
    for (int y = 0; y < n && y < 6; ++y) a[y] = CH_BPL[y];
    return a;
}

// ---- configuration runners (SPECIFICATION.md 2.9) -----------------------

RunResult run_C1a(const std::string& dir, bool no_aud0 = false) {
    return run_config(dir + (no_aud0 ? "/prog_c1a_noaud0.bin" : "/prog_c1a.bin"), nmh::Config{},
                       [](nmh::NanomigHarness& h) { poke_words(h, COP_MAIN, list_L0()); });
}
RunResult run_C1b(const std::string& dir) {
    return run_config(dir + "/prog_c1b.bin", nmh::Config{},
                       [](nmh::NanomigHarness& h) { poke_words(h, COP_MAIN, list_L0()); });
}
RunResult run_XAud(const std::string& dir) {
    return run_config(dir + "/prog_c1a_xaud.bin", nmh::Config{}, [](nmh::NanomigHarness& h) {
        poke_words(h, COP_MAIN, list_L5(sprite_ptrs_null(), bpl_ptrs_seq(0), COP_SECOND));
        poke_words(h, COP_SECOND, list_L5b());
    });
}

RunResult run_C2a(const std::string& dir) {
    return run_config(dir + "/prog_c2a.bin", nmh::Config{},
                       [](nmh::NanomigHarness& h) { poke_words(h, COP_MAIN, list_L0()); }, {RES_DSKBYTR});
}
RunResult run_C2b(const std::string& dir) {
    return run_config(dir + "/prog_c2b.bin", nmh::Config{},
                       [](nmh::NanomigHarness& h) { poke_words(h, COP_MAIN, list_L0()); }, {RES_DSKBYTR});
}
RunResult run_C2a_noenable(const std::string& dir) {
    return run_config(dir + "/prog_c2a_noenable.bin", nmh::Config{},
                       [](nmh::NanomigHarness& h) { poke_words(h, COP_MAIN, list_L0()); }, {RES_DSKBYTR});
}
RunResult run_XDsk(const std::string& dir) {
    return run_config(dir + "/prog_c2a_xdsk.bin", nmh::Config{}, [](nmh::NanomigHarness& h) {
        poke_words(h, COP_MAIN, list_L5(sprite_ptrs_null(), bpl_ptrs_seq(0), COP_SECOND));
        poke_words(h, COP_SECOND, list_L5b());
    }, {RES_DSKBYTR});
}

RunResult run_C3(const std::string& dir, bool no_spren = false) {
    std::array<uint32_t, 8> spr = sprite_ptrs_seq();
    return run_config(dir + (no_spren ? "/prog_c3_nospren.bin" : "/prog_c3.bin"), nmh::Config{},
                       [spr](nmh::NanomigHarness& h) {
                           poke_words(h, COP_MAIN, list_L3(spr));
                           for (int x = 0; x < 8; ++x) poke_words(h, CH_SPR[x], structure_C3(x));
                       });
}
RunResult run_XSpr(const std::string& dir) {
    return run_config(dir + "/prog_c3.bin", nmh::Config{}, [](nmh::NanomigHarness& h) {
        poke_words(h, COP_MAIN, list_L3x(CH_SPR[0], CH_SPR_NULL));
        poke_words(h, CH_SPR[0], structure_X());
        for (int x = 1; x < 8; ++x) poke_words(h, CH_SPR_NULL, null_block());
    });
}

RunResult run_C4(const std::string& dir, int n, bool wide_start = false, bool no_bplen = false) {
    std::string bin = "/prog_c4_" + std::to_string(n) + ".bin";
    if (n == 4 && wide_start) bin = "/prog_c4_4w.bin";
    if (n == 4 && no_bplen) bin = "/prog_c4_4_nobplen.bin";
    auto bpl = bpl_ptrs_seq(n);
    return run_config(dir + bin, nmh::Config{}, [n, bpl](nmh::NanomigHarness& h) {
        poke_words(h, COP_MAIN, list_L4(n, bpl));
        for (int y = 0; y < n; ++y) poke_words(h, CH_BPL[y], {0xAAAA, 0x5555});
    });
}

// Run X-bpl (SPECIFICATION.md P5.6): C4-4 with list L5 (not L4 -- L5's V_all
// keeps rewriting BPL1PTH/L every vblank and the Copper keeps reading odd
// cycles throughout the frame via its 113 WAITs, giving a sustained overlap
// with plane 1's own continuous even-cycle fetches, unlike L4 which the
// Copper only reads once at the very start of vertical blanking).
// BPL1PTH/L point at COP_MAIN; plane block 1 is declared (by the caller) as
// [COP_MAIN, COP_MAIN+0x2800) -- p5.6's own "that address plus 40x256 bytes".
RunResult run_XBpl(const std::string& dir) {
    std::array<uint32_t, 6> bpl = bpl_ptrs_seq(4);
    bpl[0] = COP_MAIN;
    return run_config(dir + "/prog_c4_4_xbpl.bin", nmh::Config{}, [bpl](nmh::NanomigHarness& h) {
        poke_words(h, COP_MAIN, list_L5(sprite_ptrs_null(), bpl, COP_SECOND));
        poke_words(h, COP_SECOND, list_L5b());
        for (int y = 1; y < 4; ++y) poke_words(h, CH_BPL[y], {0xAAAA, 0x5555}); // y=0 is COP_MAIN itself
    });
}

// Run C0-m (SPECIFICATION.md 2.3, D3, F2/F3's own contrast): C0 with list
// L0m -- as L0u but the WAIT's HP is $70 instead of $04, so the Copper's
// fetch of U's words (outside every declared block) lands near horizontal
// position $70 of line $80 instead of right after its refresh group. Those
// unattributed accesses then sit further than 16 cycles from any refresh
// group and form a spurious group of their own, splitting line $80 into two
// pieces whose lengths are neither 227 nor 228 -- F3 must fail.
RunResult run_C0m(const std::string& dir) {
    return run_config(dir + "/prog_c0.bin", nmh::Config{}, [](nmh::NanomigHarness& h) {
        poke_words(h, COP_MAIN, list_L0u_or_m(CH_U_SUBLIST, 0x8071));
        poke_words(h, CH_U_SUBLIST, list_U());
    });
}

// Run C6 with list L5 truncated to V_spr+V_bpl(6)+END (K6's own contrast,
// SPECIFICATION.md P6.6 "Contrast for K6"): the WAIT/MOVE words after END
// are then never fetched, so K6 (every word of L5/L5b read) must fail.
std::vector<uint16_t> list_L5_truncated(const std::array<uint32_t, 8>& spr, const std::array<uint32_t, 6>& bpl) {
    std::vector<uint16_t> o;
    emit_Vspr(o, spr);
    emit_Vbpl(o, 6, bpl);
    emit_end(o);
    return o;
}
RunResult run_C6_truncated(const std::string& dir) {
    auto spr = sprite_ptrs_seq(); auto bpl = bpl_ptrs_seq(4);
    return run_config(dir + "/prog_c6.bin", nmh::Config{}, [spr, bpl](nmh::NanomigHarness& h) {
        poke_words(h, COP_MAIN, list_L5_truncated(spr, bpl));
        for (int x = 0; x < 8; ++x) poke_words(h, CH_SPR[x], structure_C3(x));
        for (int y = 0; y < 4; ++y) poke_words(h, CH_BPL[y], {0xAAAA, 0x5555});
    });
}

RunResult run_C5(const std::string& dir) {
    auto spr = sprite_ptrs_seq(); auto bpl = bpl_ptrs_seq(4);
    return run_config(dir + "/prog_c5.bin", nmh::Config{}, [spr, bpl](nmh::NanomigHarness& h) {
        poke_words(h, COP_MAIN, list_L5(spr, bpl, COP_SECOND));
        poke_words(h, COP_SECOND, list_L5b());
        for (int x = 0; x < 8; ++x) poke_words(h, CH_SPR[x], structure_C3(x));
        for (int y = 0; y < 4; ++y) poke_words(h, CH_BPL[y], {0xAAAA, 0x5555});
    }, {RES_DSKBYTR});
}
RunResult run_C6(const std::string& dir, const char* bin = "/prog_c6.bin") {
    auto spr = sprite_ptrs_seq(); auto bpl = bpl_ptrs_seq(4);
    return run_config(dir + bin, nmh::Config{}, [spr, bpl](nmh::NanomigHarness& h) {
        poke_words(h, COP_MAIN, list_L5(spr, bpl, COP_SECOND));
        poke_words(h, COP_SECOND, list_L5b());
        for (int x = 0; x < 8; ++x) poke_words(h, CH_SPR[x], structure_C3(x));
        for (int y = 0; y < 4; ++y) poke_words(h, CH_BPL[y], {0xAAAA, 0x5555});
    }, {RES_DSKBYTR});
}
RunResult run_C6prime(const std::string& dir) { // L5' for P7's own contrast
    auto spr = sprite_ptrs_seq(); auto bpl = bpl_ptrs_seq(4);
    return run_config(dir + "/prog_c6.bin", nmh::Config{}, [spr, bpl](nmh::NanomigHarness& h) {
        poke_words(h, COP_MAIN, list_L5(spr, bpl, COP_SECOND, 32, 0x42));
        poke_words(h, COP_SECOND, list_L5b());
        for (int x = 0; x < 8; ++x) poke_words(h, CH_SPR[x], structure_C3(x));
        for (int y = 0; y < 4; ++y) poke_words(h, CH_BPL[y], {0xAAAA, 0x5555});
    });
}
RunResult run_C6_extra50(const std::string& dir) { // T1's own contrast
    auto spr = sprite_ptrs_seq(); auto bpl = bpl_ptrs_seq(4);
    return run_config(dir + "/prog_c6.bin", nmh::Config{}, [spr, bpl](nmh::NanomigHarness& h) {
        poke_words(h, COP_MAIN, list_L5(spr, bpl, COP_SECOND, -1, 0, 50));
        poke_words(h, COP_SECOND, list_L5b());
        for (int x = 0; x < 8; ++x) poke_words(h, CH_SPR[x], structure_C3(x));
        for (int y = 0; y < 4; ++y) poke_words(h, CH_BPL[y], {0xAAAA, 0x5555});
    });
}

RunResult run_C7a(const std::string& dir) {
    auto bpl = bpl_ptrs_seq(6);
    return run_config(dir + "/prog_c7a.bin", nmh::Config{}, [bpl](nmh::NanomigHarness& h) {
        poke_words(h, COP_MAIN, list_L5(sprite_ptrs_null(), bpl, COP_SECOND));
        poke_words(h, COP_SECOND, list_L5b());
        for (int y = 0; y < 6; ++y) poke_words(h, CH_BPL[y], {0xAAAA, 0x5555});
    });
}
RunResult run_C7b(const std::string& dir) {
    auto bpl = bpl_ptrs_seq(4);
    return run_config(dir + "/prog_c7b.bin", nmh::Config{}, [bpl](nmh::NanomigHarness& h) {
        poke_words(h, COP_MAIN, list_L5(sprite_ptrs_null(), bpl, COP_SECOND));
        poke_words(h, COP_SECOND, list_L5b());
        for (int y = 0; y < 4; ++y) poke_words(h, CH_BPL[y], {0xAAAA, 0x5555});
    });
}

// no_spren (P9's own K9 mutation, prog_s1_nospren.s) and the ZMut/
// colours_variant selectors (P9-P12's own image mutations) are not wired
// to a run here: P8-P12 are not built in this pass (see SPECIFICATION.md).
RunResult run_S1(const std::string& dir, ZMut mut = ZMut::None, bool colours_variant = false) {
    return run_config(dir + "/prog_s1.bin", nmh::Config{},
                       [mut, colours_variant](nmh::NanomigHarness& h) {
                           poke_words(h, COP_MAIN, list_L_S1(CH_SPR[0], CH_SPR_NULL, CH_BPL[0], colours_variant));
                           poke_words(h, CH_SPR[0], structure_Z(mut));
                           poke_words(h, CH_SPR_NULL, null_block());
                           poke_words(h, CH_BPL[0], std::vector<uint16_t>(20, 0x0000)); // all zeros (background only)
                       });
}

constexpr uint32_t kS2StructA = CH_SPR[0];
constexpr uint32_t kS2StructB = CH_SPR[0] + 0x40;
constexpr uint32_t kS2StructR = CH_SPR[2];

RunResult run_S2(const std::string& dir, bool still = false, bool no_ref = false, bool no_q4 = false, bool no_q5 = false) {
    return run_config(dir + "/prog_s2.bin", nmh::Config{},
                       [still, no_ref, no_q4, no_q5](nmh::NanomigHarness& h) {
        std::vector<uint16_t> A = {0x6050, 0x6A00};
        std::vector<uint16_t> B = {0x6A5C, 0x7400};
        for (int i = 0; i < 10; ++i) {
            if (i == 0) { A.push_back(0xFFFF); A.push_back(0x0000); B.push_back(0xFFFF); B.push_back(0x0000); }
            else { A.push_back(0x0F0F); A.push_back(0x00FF); B.push_back(0x0F0F); B.push_back(0x00FF); }
        }
        A.push_back(0x0000); A.push_back(0x0000);
        B.push_back(0x0000); B.push_back(0x0000);
        std::vector<uint16_t> R;
        R.push_back(0x4048); R.push_back(0x4600);
        for (int i = 0; i < 6; ++i) { R.push_back(0xFFFF); R.push_back(0x0000); }
        R.push_back(0x0000); R.push_back(0x0000);

        poke_words(h, kS2StructA, A);
        poke_words(h, kS2StructB, still ? A : B); // S2-still: L_B also points sprite0 at A (see list build below)
        poke_words(h, kS2StructR, R);
        poke_words(h, CH_SPR_NULL, null_block());
        poke_words(h, CH_BPL[0], std::vector<uint16_t>(20, 0x0000));

        uint32_t l_a = COP_MAIN, l_b = COP_SECOND;
        uint32_t bAddr = still ? kS2StructA : kS2StructB;
        uint16_t c17 = no_q4 ? 0x0000 : 0x0F00; // Q4 contrast: COLOR17 written as $0000 (equals COLOR00)
        if (no_q5) {
            poke_words(h, l_a, list_L_A_truncated(kS2StructA, kS2StructR, CH_SPR_NULL));
        } else {
            poke_words(h, l_a, list_L_AB(kS2StructA, kS2StructR, CH_SPR_NULL, CH_BPL[0], l_b, false, c17));
        }
        // Q-ref's own contrast changes L_B alone: sprite 2 -> null, R absent in B frames only.
        poke_words(h, l_b, list_L_AB(bAddr, kS2StructR, CH_SPR_NULL, CH_BPL[0], l_a, no_ref, c17));
    });
}

std::vector<Block> blocks_S1() { return {copper_block(), sprite_block(0), sprite_null_block(), bitplane_block(0)}; }
std::vector<Block> blocks_S2() {
    return {copper_block(), sprite_block(0), sprite_block(2), sprite_null_block(), bitplane_block(0)};
}

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
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)});
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

// K5: every enabled plane has fetches in >=150 lines of every observed frame
// (checked per frame, not merged across the whole observed span).
bool k5_check(const RunResult& run, int n, Checks& checks, const std::string& cfgname) {
    auto blocks = blocks_bitplane_named(n);
    LineRef lref = make_lines(run.log, blocks);
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)});
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
// 2 end) is read at least once.
bool k4_check(const RunResult& run, Checks& checks, const std::string& cfgname) {
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)});
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
// once (the list ran to its end).
bool k6_check(const RunResult& run, Checks& checks, const std::string& cfgname) {
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)});
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
// list_L5's own word layout (see list_L5 above): V_spr(32)+V_bpl6(24)+
// COP2LC movel(4) = 60 words before the WAIT/MOVE loop; each loop iteration
// i (0..112) is WAIT(2)+3*MOVE(6) = 8 words; the trailer's own WAIT-at-$B4
// sits right after iteration 112, i.e. at word offset 60+8*113=964.
// Segment i (SPECIFICATION.md P7.3) = the accesses after WAIT i's own two
// words, up to and including WAIT (i+1)'s own two words: word-offset range
// [60+8i+2, 60+8(i+1)+2), uniformly 8 words, whether i+1 is another loop
// iteration or (for i=112) the trailer's WAIT-at-$B4.
constexpr uint32_t kSegBase = 62; // 60+2: first word after WAIT 0
uint32_t seg_lo_word(uint32_t cop_main_word, int i) { return cop_main_word + kSegBase + 8u * (uint32_t)i; }
uint32_t seg_hi_word(uint32_t cop_main_word, int i) { return cop_main_word + kSegBase + 8u * (uint32_t)(i + 1); }

using PairList = std::vector<std::pair<int64_t, int64_t>>; // (line offset from segment's first access, s)

// [frame 0..2][segment 0..112] -> PairList, time-ordered (run.log is
// chronological). Only valid for a run using the untouched list_L5 layout
// (C5, C6, C7a, C7b, and C6' -- NOT the T1 contrast, which inserts an extra
// word and shifts every later segment; that contrast is measured directly,
// see t1_check). `blocks` must be the run's OWN full declared-block list
// (e.g. blocks_full(4) for C6): the refresh set is identified by exclusion
// (SPECIFICATION.md D7/2.2), so a run's other live DMA traffic (sprite,
// audio, disk, bitplane) must be declared here or it is misattributed "R"
// and corrupts the line/refresh-group construction that segments and lines
// are anchored on.
std::vector<std::vector<PairList>> compute_segments(const RunResult& run, const std::vector<Block>& blocks) {
    std::vector<std::vector<PairList>> out(3, std::vector<PairList>(113));
    LineRef lref = make_lines(run.log, blocks);
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)});
    if (restarts.size() < 6) return out;
    uint32_t base = wordof(COP_MAIN);
    std::vector<std::optional<int64_t>> li0(3 * 113);
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
bool t1_check(const RunResult& c5, const std::vector<Block>& b5, const RunResult& c6, const std::vector<Block>& b6,
              Checks& checks) {
    auto s5 = compute_segments(c5, b5), s6 = compute_segments(c6, b6);
    int mismatched = 0;
    for (int f = 0; f < 3; ++f) for (int i = 0; i < 113; ++i) if (s5[f][i].size() != s6[f][i].size()) ++mismatched;
    checks.note("T1: segments with differing access count (C5 vs C6) = " + std::to_string(mismatched));
    return mismatched == 0;
}

// T1's own contrast (run_C6_extra50, list L5'): segment 50 gains the one
// extra MOVE inserted right after WAIT 50, so it has 2 more accesses than
// the baseline C6; segment 50's own starting word offset is unaffected
// (the insertion happens before the following WAIT), so seg_lo_word(50) is
// still valid. Its own words run 10 wide instead of 8 -- and every later
// segment's fixed offset no longer lines up -- so `width` must be passed
// explicitly per run (8 for an unshifted list, 10 for run_C6_extra50):
// a fixed window would either clip the mutant's extra words or, for the
// baseline, spill 2 words into segment 51 and hide the very difference
// this contrast exists to show.
int seg50_count(const RunResult& run, uint32_t width) {
    auto restarts = restart_fetches(run.log, {wordof(COP_MAIN)});
    if (restarts.size() < 6) return -1;
    uint32_t base = wordof(COP_MAIN);
    uint32_t lo = seg_lo_word(base, 50);
    int total = 0;
    for (int f = 0; f < 3; ++f) {
        uint64_t flo = restarts[2 + f], fhi = restarts[3 + f];
        int n = 0;
        for (const auto& a : run.log)
            if (!a.is_write && a.cycle >= flo && a.cycle < fhi && a.word_addr >= lo && a.word_addr < lo + width)
                ++n;
        total += n;
    }
    return total;
}

// T2: no CPU-block access lies in any line spanned by a segment.
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
        if (a.word_addr < kCpuWordThreshold) continue;
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

} // namespace

// MUTATION P1 stimulus red C0-u (Copper jumps to sub-list U right after line $80's refresh group), via NMH_MUTATE=P1
// MUTATION P2 stimulus red X-dsk (C2a with list L5, DSKPT at the Copper block), via NMH_MUTATE=P2
// MUTATION P3 stimulus red X-aud (C1a with list L5, AUD0LC at the Copper block), via NMH_MUTATE=P3
// MUTATION P4 stimulus red X-spr (C3 with list L3x, structure X for sprite 0), via NMH_MUTATE=P4
// MUTATION P5 stimulus red X-bpl (C4-4 with list L5, BPL1PT at the Copper block), via NMH_MUTATE=P5
// MUTATION P6 stimulus red X-aud (same run as P3's own contrast: the Copper block's own check goes red), via NMH_MUTATE=P6
// MUTATION A1 stimulus red C1a graded against C0's own declared blocks (no Audio block), via NMH_MUTATE=A1
// MUTATION F1 self-test: a synthetic trace with one access moved by one time unit must fail F1 (no RTL run), via NMH_MUTATE=F1
// MUTATION F23 stimulus red C0-m (list L0m, Copper fetches U near HP=$70 of line $80, splitting it), via NMH_MUTATE=F23
// MUTATION K2a stimulus red C2a with both DSKLEN writes $4000|8000 (bit15 clear, DMAON=0), via NMH_MUTATE=K2a
// MUTATION K3 stimulus red C1a without AUD0EN in the S4 write, via NMH_MUTATE=K3
// MUTATION K4 stimulus red C3 without SPREN, via NMH_MUTATE=K4
// MUTATION K5 stimulus red C4-4 without BPLEN, via NMH_MUTATE=K5
// MUTATION K6 stimulus red C6 with list L5 truncated to V_spr+V_bpl(6)+END, via NMH_MUTATE=K6
// MUTATION T1 stimulus red C6' (list L5', WAIT 50 at HP=$42 instead of $40): only segment 50 differs, via NMH_MUTATE=T1
// MUTATION T2 stimulus red C6 with STOP replaced by a self-branch loop (prog_c6_stopself), via NMH_MUTATE=T2
// MUTATION T3 stimulus red C6 without SPREN (prog_c6_nospren): no sprite fetches in segments 16-35/64-83, via NMH_MUTATE=T3
// MUTATION P7 stimulus red C6' (same run as T1's own contrast: segment 50's pair list differs from C5's), via NMH_MUTATE=P7
//
// Not exercised, by a scope decision: a dedicated mutation for K2b (the
// >=500-access count alone, independent of K2a's DMAON=0) -- C2b's own
// baseline run already shows the >=500 threshold failing organically (0
// disk-block accesses, D9's no-drive case), which is a real decline, not a
// synthetic contrast, so no separate mutant was built for it. P8-P12
// (video/sprite-image propositions) are not implemented in this package at
// all -- see the final report.

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;
    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    if (!mv.empty()) {
        // ---- mutation runs: exactly one capture, never counted as the -----
        // ---- normal fixture/precondition/assertion summary. ---------------
        if (mv == "P1") {
            RunResult r = run_C0u(dir);
            auto pr = analyze_population(r.log, blocks_C0(), {wordof(COP_MAIN)}, "R", false);
            checks.note("C0-u: done=" + std::to_string(r.done.has_value()) + " F3=" + std::to_string(pr.f3_ok) +
                        " population=" + std::to_string(pr.population) + " all_even=" + std::to_string(pr.all_even));
            checks.expect(!pr.all_even, "P1 mutation: C0-u's refresh set gains an odd-s access. "
                          "ch2-about-the-copper.c24 COP2-about-the-copper.c29 p.20");
        } else if (mv == "P2") {
            RunResult r = run_XDsk(dir);
            // No copper_block() here: for this one run the disk pointer
            // covers the Copper's own list, and the spec's own text (P2.6)
            // wants that whole span graded as "Disk" -- including the
            // Copper's own odd-cycle fetches of the same words -- so the
            // disk-block check itself finds the odd-s accesses. Listing
            // copper_block() first (as the shared blocks_disk() helper
            // does for the ordinary disjoint runs) would shadow the
            // overlap and always attribute it "Copper" instead, silently
            // making this contrast vacuous.
            std::vector<Block> b = {disk_block(COP_MAIN, 8000)};
            auto pr = analyze_population(r.log, b, {wordof(COP_MAIN)}, "Disk", false);
            checks.note("X-dsk: done=" + std::to_string(r.done.has_value()) +
                        " population=" + std::to_string(pr.population) + " all_even=" + std::to_string(pr.all_even));
            checks.expect(!pr.all_even, "P2 mutation: X-dsk's disk-block check finds odd-s accesses. "
                          "ch2-about-the-copper.c23 COP2-about-the-copper.c28 p.20");
        } else if (mv == "P3" || mv == "P6") {
            RunResult r = run_XAud(dir);
            // One shared block over the overlap (audio channel 0's pointer
            // and the Copper's own list both read [COP_MAIN,COP_MAIN+0x1000)):
            // same reasoning as P2's own fix above -- no separate
            // copper_block() that would shadow it.
            std::vector<Block> b = {{"Shared", wordof(COP_MAIN), wordof(COP_MAIN + 0x1000)}};
            auto pr = analyze_population(r.log, b, {wordof(COP_MAIN)}, "Shared", false);
            checks.note("X-aud: done=" + std::to_string(r.done.has_value()) +
                        " shared_pop=" + std::to_string(pr.population) + " all_even=" + std::to_string(pr.all_even) +
                        " all_odd=" + std::to_string(pr.all_odd));
            if (mv == "P3")
                checks.expect(!pr.all_even, "P3 mutation: X-aud's audio-block check finds odd-s accesses (the "
                              "Copper reads the same words). ch2-about-the-copper.c22 COP2-about-the-copper.c27 p.20");
            else
                checks.expect(!pr.all_odd, "P6 mutation: X-aud's Copper-block check finds even-s accesses (audio "
                              "channel 0 reads the same words). ch2-about-the-copper.c21 p.20");
        } else if (mv == "P5") {
            RunResult r = run_XBpl(dir);
            // Plane block 1 declared as [COP_MAIN, COP_MAIN+0x2800) --
            // P5.6's own "that address plus 40x256 bytes" -- no separate
            // copper_block() for the same reason as P2/P3/P6 above.
            std::vector<Block> b = {{"XBPL1", wordof(COP_MAIN), wordof(COP_MAIN + 0x2800)}};
            auto pr = analyze_population(r.log, b, {wordof(COP_MAIN)}, "XBPL1", false);
            checks.note("X-bpl: done=" + std::to_string(r.done.has_value()) +
                        " population=" + std::to_string(pr.population) + " all_even=" + std::to_string(pr.all_even));
            checks.expect(!pr.all_even, "P5 mutation: X-bpl's plane-1 check (a) finds odd-s accesses (the Copper "
                          "reads the same words). ch2-about-the-copper.c26 COP2-about-the-copper.c31 p.20");
        } else if (mv == "A1") {
            // The audio blocks must be DECLARED for this to be a real
            // contrast: attribute() falls back to "R" for any word address
            // outside every declared block (D7, refresh identified by
            // exclusion), so grading against blocks_C0() alone -- which
            // declares nothing but Copper -- would silently reclassify
            // every audio access as refresh and a1_violations could never
            // be anything but 0. blocks_audio() declares audio blocks
            // 0-3, so C1a's real accesses there register as violations
            // (p.144 predicts DMA starts for every channel AUD0EN sets).
            RunResult r = run_C1a(dir);
            auto pr = analyze_population(r.log, blocks_audio(), {wordof(COP_MAIN)}, "R", true);
            checks.note("A1 mutation (C1a graded with audio blocks declared): violations=" + std::to_string(pr.a1_violations));
            checks.expect(pr.restarts_ok && !pr.a1_ok && pr.a1_violations > 0,
                          "A1 mutation: C1a's audio accesses fall into the declared audio blocks, not R/CPU/"
                          "Copper. p.144");
        } else if (mv == "F1") {
            // Self-test, no RTL run (SPECIFICATION.md F1's own contrast:
            // "None exists ... verified by a self-test"): a synthetic
            // two-access trace on the S=kS grid, then one access moved by
            // one time unit, must fail check_f1.
            std::vector<nmh::RamAccess> good = {
                {1000, 0x1000, 0, false, true, true},
                {1000 + kS, 0x1002, 0, false, true, true},
            };
            std::vector<nmh::RamAccess> bad = good;
            bad[1].cycle += 1;
            bool good_ok = check_f1(good, {}, 0, 2000);
            bool bad_ok = check_f1(bad, {}, 0, 2000);
            checks.note("F1 self-test: unperturbed trace F1=" + std::to_string(good_ok) +
                        ", one access moved by 1 time unit F1=" + std::to_string(bad_ok));
            checks.expect(good_ok && !bad_ok, "F1 mutation: moving one access off the S-cycle grid by one time "
                          "unit makes check_f1 fail. SPECIFICATION.md 2.1");
        } else if (mv == "F23") {
            RunResult r = run_C0m(dir);
            auto pr = analyze_population(r.log, blocks_C0(), {wordof(COP_MAIN)}, "R", false);
            checks.note("C0-m: done=" + std::to_string(r.done.has_value()) + " F2=" + std::to_string(pr.f2_ok) +
                        " F3=" + std::to_string(pr.f3_ok));
            checks.expect(r.done.has_value() && !pr.f3_ok, "F23 mutation: C0-m's Copper fetch near HP=$70 of line "
                          "$80 splits it into two pieces neither 227 nor 228 cycles long, and F3 fails. "
                          "SPECIFICATION.md 2.3 D3");
        } else if (mv == "K6") {
            RunResult r = run_C6_truncated(dir);
            bool k6 = k6_check(r, checks, "C6-truncated");
            checks.expect(r.done.has_value() && !k6, "K6 mutation: C6 with L5 truncated to V_spr+V_bpl(6)+END never "
                          "fetches the WAIT/MOVE words after it, and K6 fails.");
        } else if (mv == "T1") {
            RunResult c5 = run_C5(dir);
            RunResult c6x = run_C6_extra50(dir);
            int base = seg50_count(c5, 8), mut = seg50_count(c6x, 10);
            checks.note("T1 mutation: segment 50 access count C5=" + std::to_string(base) +
                        " C6-extra50=" + std::to_string(mut));
            // seg50_count sums over 3 observed frames, so 2 extra words
            // read once per frame is +6 in the total, not +2.
            checks.expect(base >= 0 && mut == base + 2 * 3, "T1 mutation: C6 with one extra MOVE after WAIT 50 has "
                          "exactly 2 more accesses in segment 50 than C5, and T1 fails for that segment.");
        } else if (mv == "T2") {
            RunResult r = run_C6(dir, "/prog_c6_stopself.bin");
            bool t2 = t2_check(r, blocks_full(4), checks, "C6-stopself");
            checks.expect(r.done.has_value() && !t2, "T2 mutation: C6 with the CPU branching to itself instead of "
                          "STOP fetches in every line, and T2 fails.");
        } else if (mv == "T3") {
            RunResult r = run_C6(dir, "/prog_c6_nospren.bin");
            bool t3 = t3_check(r, blocks_full(4), checks);
            checks.expect(r.done.has_value() && !t3, "T3 mutation: C6 without SPREN has no sprite fetches in "
                          "segments 16-35/64-83 (VP $50-$63/$80-$93), and T3 fails.");
        } else if (mv == "P7") {
            RunResult c5 = run_C5(dir);
            RunResult c6p = run_C6prime(dir);
            auto s5 = compute_segments(c5, blocks_full(4));
            // c6p's own list (L5') only substitutes WAIT 32's own HP word
            // ($40 -> $42); it does not insert or remove any word, so every
            // segment's word offset is unchanged and compute_segments'
            // fixed-offset table is still valid for it.
            auto s6p = compute_segments(c6p, blocks_full(4));
            int mismatches = 0;
            for (int f = 0; f < 3; ++f) for (int i = 0; i < 113; ++i) if (s5[f][i] != s6p[f][i]) ++mismatches;
            checks.note("P7 mutation: segment mismatches (C5 vs C6') = " + std::to_string(mismatches));
            checks.expect(mismatches > 0, "P7 mutation: C6' (WAIT 32 at HP=$42 instead of $40) makes segment 32's "
                          "(line offset, s) pairs differ from C5's, and P7 fails for that segment. p.22");
        } else if (mv == "P4") {
            RunResult r = run_XSpr(dir);
            auto pr = analyze_population(r.log, blocks_sprite(), {wordof(COP_MAIN)}, "Sprite", false);
            checks.note("X-spr: done=" + std::to_string(r.done.has_value()) +
                        " population=" + std::to_string(pr.population) + " all_even=" + std::to_string(pr.all_even));
            checks.expect(!pr.all_even, "P4 mutation: X-spr's sprite-block check finds odd-s accesses. "
                          "ch2-about-the-copper.c25 COP2-about-the-copper.c30 p.20");
        } else if (mv == "K2a") {
            RunResult r = run_C2a_noenable(dir);
            bool dmaon = r.peeks.size() && r.peeks[0] && (*r.peeks[0] & 0x4000);
            auto pr = analyze_population(r.log, blocks_disk(), {wordof(COP_MAIN)}, "Disk", false);
            checks.note("C2a-noenable: DSKBYTR=" + (r.peeks.size() && r.peeks[0] ? hex(*r.peeks[0]) : std::string("?")) +
                        " disk_population=" + std::to_string(pr.population));
            checks.expect(!dmaon && pr.population == 0, "K2a mutation: both DSKLEN writes with bit15 clear give "
                          "DMAON=0 and no disk accesses. p.248 Appendix A p.278");
        } else if (mv == "K3") {
            // K3 grades each of the four channels separately (>=100
            // accesses each); prog_c1a_noaud0 only drops AUD0EN, so
            // channels 1-3 still run -- the merged "Audio" population is
            // not the right measure here. Channel 0's own population must
            // be the one that drops to 0.
            RunResult r = run_C1a(dir, true);
            auto restarts = restart_fetches(r.log, {wordof(COP_MAIN)});
            auto span = observed_span(restarts);
            size_t ch0 = 0;
            if (span.ok)
                for (const auto& a : r.log)
                    if (!a.is_write && a.cycle >= span.lo && a.cycle < span.hi &&
                        a.word_addr >= wordof(CH_AUD[0]) && a.word_addr < wordof(CH_AUD[0] + 0x200)) ++ch0;
            checks.note("C1a-noaud0: channel0_population=" + std::to_string(ch0));
            checks.expect(span.ok && ch0 == 0, "K3 mutation: C1a without AUD0EN has 0 accesses on channel 0 "
                          "(<100, K3 itself now fails for that channel, as it must). p.144");
        } else if (mv == "K4") {
            RunResult r = run_C3(dir, true);
            auto pr = analyze_population(r.log, blocks_sprite(), {wordof(COP_MAIN)}, "Sprite", false);
            checks.note("C3-nospren: sprite_population=" + std::to_string(pr.population));
            checks.expect(pr.population == 0, "K4 mutation: C3 without SPREN reads no sprite accesses. p.109");
        } else if (mv == "K5") {
            RunResult r = run_C4(dir, 4, false, true);
            auto pr = analyze_population(r.log, blocks_bitplane(4), {wordof(COP_MAIN)}, "Bitplane", false);
            checks.note("C4-4-nobplen: bitplane_population=" + std::to_string(pr.population));
            checks.expect(pr.population == 0, "K5 mutation: C4-4 without BPLEN reads no plane fetches. p.196");
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

    bool fixture_ok = true;
    for (auto& p : all) fixture_ok = fixture_ok && p.run.loaded && p.run.released && p.run.done.has_value();
    checks.expect(fixture_ok, "FIXTURE: every configuration's program loaded, released reset, and reached DONE "
                              "[not graded]");

    std::vector<std::string> names_all;
    for (auto& p : all) names_all.push_back(p.name);

    bool p1 = fold_check(all, names_all, "R", true, checks, "P1");
    checks.expect(p1, "P1: every refresh access lies on an even in-line cycle, in every run graded (PAL/ECS only "
                  "-- NTSC/OCS mode variants of C0, C6 and S1 not run, budget). "
                  "ch2-about-the-copper.c24 COP2-about-the-copper.c29 p.20");

    // K2a/K2b (preconditions of P2) and P2 itself, with D9's own carve-out:
    // if K2b (>=500 disk-block accesses) holds for C2a and C6 but not C2b
    // (no drive attached), the read direction is declined, not graded red.
    auto disk_pop = [&](const char* name) -> size_t {
        const Pop* p = nullptr;
        for (auto& c : all) if (c.name == name) p = &c;
        if (!p) return 0;
        return analyze_population(p->run.log, blocks_disk(), {wordof(COP_MAIN)}, "Disk", false).population;
    };
    auto dskbytr_ok = [&](const char* name, bool want_write, uint16_t* out_v) -> bool {
        const Pop* p = nullptr;
        for (auto& c : all) if (c.name == name) p = &c;
        if (!p || p->run.peeks.empty() || !p->run.peeks[0]) return false;
        uint16_t v = *p->run.peeks[0];
        if (out_v) *out_v = v;
        bool dmaon = (v & 0x4000) != 0, diskwrite = (v & 0x2000) != 0;
        return dmaon && diskwrite == want_write;
    };
    {
        uint16_t va = 0, vb = 0, vc = 0;
        bool c2a_dskbytr = dskbytr_ok("C2a", true, &va);
        bool c2b_dskbytr = dskbytr_ok("C2b", false, &vb);
        bool c6_dskbytr = dskbytr_ok("C6", true, &vc);
        checks.note("K2a: DSKBYTR C2a=" + hex(va) + " C2b=" + hex(vb) + " C6=" + hex(vc));
        checks.expect(c2a_dskbytr && c2b_dskbytr && c6_dskbytr, "K2a: the stored DSKBYTR has DMAON=1 and "
                      "DISKWRITE=1 in C2a and C6, DISKWRITE=0 in C2b. p.248 Appendix A p.278");
    }
    size_t pop_c2a = disk_pop("C2a"), pop_c2b = disk_pop("C2b"), pop_c6 = disk_pop("C6");
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
                  "). ch2-about-the-copper.c23 COP2-about-the-copper.c28 p.20");

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
            auto restarts = restart_fetches(p->run.log, {wordof(COP_MAIN)});
            auto span = observed_span(restarts);
            note << cfgname << ":";
            if (!span.ok) { k3_ok = false; note << "NOSPAN;"; continue; }
            for (int x = 0; x < 4; ++x) {
                size_t n = 0;
                for (const auto& a : p->run.log)
                    if (!a.is_write && a.cycle >= span.lo && a.cycle < span.hi &&
                        a.word_addr >= wordof(CH_AUD[x]) && a.word_addr < wordof(CH_AUD[x] + 0x200)) ++n;
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
        bool k4 = k4_check(c3->run, checks, "C3") && k4_check(c6k4->run, checks, "C6");
        checks.expect(k4, "K4: in each observed frame of C3 and C6, every word of each of the eight sprite "
                      "structures (control, data and end words) is read at least once. p.108");
    }

    bool p6 = fold_check(all, names_all, "Copper", false, checks, "P6");
    checks.expect(p6, "P6: every Copper-block access lies on an odd in-line cycle, in every run graded (PAL/ECS "
                  "only -- NTSC/OCS mode variants not run, budget). ch2-about-the-copper.c21 p.20");

    // A1 (precondition of P1-P6): in the baseline configuration C0, every
    // access in the observed frames is in R, the CPU block, or the Copper
    // block.
    {
        const Pop* c0 = nullptr;
        for (auto& p : all) if (p.name == "C0") c0 = &p;
        auto pr = analyze_population(c0->run.log, blocks_C0(), {wordof(COP_MAIN)}, "R", true);
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

        bool k6 = k6_check(c5, checks, "C5") && k6_check(c6, checks, "C6") &&
                  k6_check(c7ap->run, checks, "C7a") && k6_check(c7bp->run, checks, "C7b");
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

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
