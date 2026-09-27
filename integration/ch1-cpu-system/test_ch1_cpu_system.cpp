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
// core" = TG68K. Baseline (no mutation): first core 52 passed/0 failed;
// second core 108 passed/0 failed. P4 and P6's own claims (opus/sol:1-
// guidelines.c59, c67) are declined (harness limit, see SPECIFICATION.md
// section 1 and 10), so neither prints a FAIL in the baseline. Where a
// mutation's own file is not loaded by a core's run at all, that core's
// count equals its baseline and is marked "not applicable".
//
// MUTATION P1-MOVE NMH_MUTATE=P1-MOVE: every CLR.x replaced by MOVE.x #0
//   (p1_move.bin) -- measured: first core 51/1 failed, second core 107/1
//   failed (P1 on both). MOVE.x #0 makes a single write, no read, on both
//   cores (case 1, .B (An): W@2274429 on the first core, W@2274317 on the
//   second, no preceding read) -- the page's own contrast, which flips P1's
//   read-then-write relation red through the unchanged comparator, exactly
//   as p.13 and p.263 predict.
// MUTATION P1A NMH_MUTATE=P1A: case 5's S_A store left out (p1_muta.bin) --
//   measured: first core 50/1 failed (P1.a), second core 106/1 failed
//   (P1.a). P1's own relation is gated on P1.a and is not evaluated
//   (recorded), not a second FAIL.
// MUTATION P1B NMH_MUTATE=P1B: case 8's effective address is T_8+8
//   (p1_mutb.bin) -- measured: first core 50/1 failed (P1.b), second core
//   106/1 failed (P1.b).
// MUTATION P1C NMH_MUTATE=P1C: case 11 assembled as CLR.L while the table
//   still names CLR.W (p1_mutc.bin) -- measured: first core 50/1 failed
//   (P1.c), second core 106/1 failed (P1.c).
// MUTATION C1 NMH_MUTATE=C1: an extra INTENA write follows step 2
//   (p1_mutc1.bin) -- measured: first core 50/1 failed (C1), second core
//   106/1 failed (C1).
// MUTATION C2 NMH_MUTATE=C2: an extra DMACON write follows step 4
//   (p1_mutc2.bin) -- measured: first core 50/1 failed (C2), second core
//   106/1 failed (C2).
// MUTATION C3 NMH_MUTATE=C3: a branch to FAILRUN precedes the normal S_END
//   write (p1_mutc3.bin) -- measured: first core 50/1 failed (C3), second
//   core 106/1 failed (C3).
// MUTATION C4 NMH_MUTATE=C4: the S_START write is left out (p1_mutc4.bin) --
//   measured: first core 49/2 failed (the peek_word fixture check, C4),
//   second core 105/2 failed (the same two): S_START never being written
//   also makes its own peek_word readback wrong, an independent clause.
// MUTATION P2-LOW NMH_MUTATE=P2-LOW: every register loaded with its
//   pattern's lower half, zero-extended (p2_low.bin) -- measured: first
//   core 51/1 failed (P2), second core 107/1 failed (P2).
// MUTATION P2A NMH_MUTATE=P2A: step 8's S_B write left out (p2_muta.bin) --
//   measured: first core 49/2 failed (P2.a, P2.b), second core 105/2
//   failed (P2.a, P2.b). P2's own relation is gated on P2.a/P2.b/P2.c and
//   is not evaluated, not a third FAIL.
// MUTATION P2B NMH_MUTATE=P2B: the store of D3 left out (p2_mutb.bin) --
//   measured: first core 50/1 failed (P2.b), second core 106/1 failed
//   (P2.b).
// MUTATION P2C NMH_MUTATE=P2C: a TRAP #0 through an RTE-only handler runs
//   between steps 6 and 7 (p2_mutc.bin) -- measured: first core 50/1 failed
//   (P2.c), second core 106/1 failed (P2.c).
// MUTATION P3A NMH_MUTATE=P3A: block 1's BRA.S replaced by NOP, falling
//   into the guard (p3_mutnop.bin) -- measured: first core 47/4 failed (C3,
//   P3.a, P3.b, P3.d), second core 103/4 failed (the same four, 68000-mode
//   iteration only). P3.c stays green: the block's own X_1 self-report
//   never runs, so P3.c's static check (run against the self-reported,
//   now-stale X_1) finds no operand referencing it -- a known limit of
//   this contrast, not a new claim.
// MUTATION P3B NMH_MUTATE=P3B: the same NOP substitution (p3_mutnop.bin) --
//   measured: first core 47/4 failed (C3, P3.a, P3.b, P3.d), second core
//   103/4 failed (the same four, 68000-mode iteration only).
// MUTATION P3C NMH_MUTATE=P3C: a TST.W W_3 is inserted before block 3
//   (p3_mutc.bin) -- measured: first core 50/1 failed (P3.c), second core
//   106/1 failed (P3.c, 68000-mode iteration only).
// MUTATION P3D NMH_MUTATE=P3D: block 1 assembled as BRA.W while the table
//   still names BRA.S (p3_mutd.bin) -- measured: first core 50/1 failed
//   (P3.d), second core 106/1 failed (P3.d, 68000-mode iteration only).
// MUTATION P5A NMH_MUTATE=P5A: case 1's TRAP #0 replaced by NOP, no
//   exception taken (p5_muta.bin) -- measured: first core 49/3 failed (C3,
//   P5.a, P5.c), second core 101/4 failed (C3, P5.a, P5.c, P5.b): case 1
//   never reaching its handler branches straight to FAILRUN, so every later
//   case's window is lost too (one aggregate P5.a/P5.c fail each, not five),
//   S_FAIL is written (C3), and PRE_SP for e=1..5 no longer matches the
//   68010/68020 runs (P5.b, second core only).
// MUTATION P5B NMH_MUTATE=P5B: SUBQ.L #4,A7 inserted before case 1 in the
//   68010 image (p5_mutb.bin) -- measured: first core 52/0 failed (not
//   applicable: this core never loads the 68010 image), second core 104/1
//   failed (P5.b).
// MUTATION P5C NMH_MUTATE=P5C: an extra MOVE.W D0,S_P lands inside W_2
//   (p5_mutc.bin) -- measured: first core 51/1 failed (P5.c), second core
//   107/1 failed (P5.c).
// MUTATION P6E NMH_MUTATE=P6E: every guard shortened to one word
//   (p6_mute.bin) -- measured: first core 50/2 failed (P6 alignment,
//   P6.e), second core 106/2 failed (the same two). P6.e goes red through
//   its own clause: D(68000 mode) = 1 = the guard length (1 word), so
//   "D < guard length" fails; guard_len is read back from the image
//   (block 1's guard opcode), not from NMH_MUTATE. Shortening the guard
//   also shifts every later X_k by a constant amount, which happens to
//   break the alignment precondition too (a side effect of this specific
//   mutation, not what it targets) -- P6.e is not gated on alignment, so
//   this does not swallow P6.e's own red.
// MUTATION P6ALIGN NMH_MUTATE=P6ALIGN: the first pair's extra pad word is
//   left out, so X1 and X2 land at the same parity mod 4 (p6_mutalign.bin)
//   -- measured: first core 51/1 failed (P6 alignment), second core 107/1
//   failed (P6 alignment). P6.e and D stay green: alignment does not gate
//   them.
//
// P3's own main relation has no page-predicted contrasting case
// (SPECIFICATION.md section 5: "None exists on the pages") -- not
// exercised, by a scope decision, per SPECIFICATION.md itself. P5's and
// P6's own "contrasting case" (first core vs. second core in 68000 mode)
// spans both cores' separate executables; neither build can read the
// other's data, so each core prints its own RECORDED frame/depth data and
// the comparison is drawn once both outputs exist, not as a checks.expect
// inside either binary. C5 (the record names the run's core/mode) cannot
// be measured with anything the harness currently exposes on the ports;
// withdrawn rather than worked around (reported to the lead). The C4
// ordering clause ("S_START comes before every window") is asserted but
// has no dedicated mutation of its own: reordering it would mean
// restructuring the shared SETUP macro, which every other precondition's
// contrast also depends on.

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
constexpr uint32_t POST_SP[6] = {0,0x4024,0x4028,0x402C,0x4030,0x4034}; // index e=1..5 -> POST_SP[e]; POST_SP1=$4024 etc
constexpr uint32_t T_BASE   = 0x4100;
constexpr uint32_t R_BASE   = 0x4300;
constexpr uint32_t XTAB     = 0x4400;
constexpr uint32_t M_BASE   = 0x4500;
constexpr uint32_t M_USP    = 0x4540;
constexpr uint32_t XTAB3    = 0x4550;
constexpr uint32_t XTAB6    = 0x4570;
constexpr uint32_t FARZ     = 0x1FFFFE;
constexpr uint32_t kProgramOrg = 0x080400; // nanomig_harness.h: load_program()'s fixed code entry

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    // A precondition gates its relation: only graded when the precondition
    // holds (SPECIFICATION.md 2.5, "not evaluated, never green").
    void expect_gated(bool precond_ok, bool relation, const std::string& label) {
        if (precond_ok) expect(relation, label);
        else record(label + " -- not evaluated: a precondition failed");
    }
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

struct WHitData { bool found = false; uint16_t data = 0; uint64_t cycle = 0; };
// Every CPU write to byte_addr with cycle > after; count and last data.
struct WriteInfo { int count = 0; uint16_t last_data = 0; };
WriteInfo writes_strictly_after(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr, uint64_t after) {
    WriteInfo w;
    uint32_t a = wa(byte_addr);
    for (const auto& x : log) if (x.is_write && x.word_addr == a && x.cycle > after) { ++w.count; w.last_data = x.data; }
    return w;
}
// The first CPU write to byte_addr in the whole log.
WHitData first_write(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr) {
    uint32_t a = wa(byte_addr);
    for (const auto& x : log) if (x.is_write && x.word_addr == a) return {true, x.data, x.cycle};
    return {};
}
// The last CPU read of byte_addr in the whole log: P1.c/P3.d/P6's reused
// P3.d ask for the opcode word a CPU read actually carried, not the memory
// model's own final word at that address.
WHitData last_read(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr) {
    WHitData w;
    uint32_t a = wa(byte_addr);
    for (const auto& x : log) if (!x.is_write && x.word_addr == a) w = {true, x.data, x.cycle};
    return w;
}

// Reads a whole file (a package .bin) into bytes; empty if it cannot be opened.
std::vector<uint8_t> read_file_bytes(const std::string& path) {
    std::vector<uint8_t> out;
    std::FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return out;
    std::fseek(f, 0, SEEK_END);
    long n = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    if (n > 0) { out.resize(size_t(n)); if (std::fread(out.data(), 1, out.size(), f) != out.size()) out.clear(); }
    std::fclose(f);
    return out;
}

// P3.c's static check: the assembled image (read from disk, not the memory
// model) contains no instruction whose data operand is the absolute address
// of a word in [w_lo,w_hi] (inclusive, word-aligned) anywhere outside that
// span itself. An absolute-long operand embeds its address as a 4-byte
// big-endian value; that is what SPECIFICATION.md 5's "static check on the
// listing" comes down to for this image.
bool static_no_operand_in_range(const std::vector<uint8_t>& img, uint32_t org, uint32_t w_lo, uint32_t w_hi) {
    if (img.empty()) return false;
    for (uint32_t probe = w_lo; probe <= w_hi; probe += 2) {
        uint8_t b0 = uint8_t(probe >> 24), b1 = uint8_t(probe >> 16), b2 = uint8_t(probe >> 8), b3 = uint8_t(probe);
        for (size_t off = 0; off + 4 <= img.size(); off += 2) {
            uint32_t at = org + uint32_t(off);
            if (at >= w_lo && at <= w_hi) continue; // the guard span's own bytes
            if (img[off] == b0 && img[off+1] == b1 && img[off+2] == b2 && img[off+3] == b3) return false;
        }
    }
    return true;
}

// P5.b's static half: the two images are the same length and every
// differing byte falls inside a single 4-byte span (SPECIFICATION.md
// section 7 step 3: MOVEC D0,VBR vs. two NOPs -- 2 words, 4 bytes; MOVEC's
// encoding, 4E7B 0801, happens to share its first byte with NOP's 4E71, so
// as few as 3 bytes actually differ).
bool images_equal_except_one_word_pair(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b) {
    if (a.empty() || a.size() != b.size()) return false;
    int first = -1, last = -1;
    for (size_t i = 0; i < a.size(); ++i) if (a[i] != b[i]) { if (first < 0) first = int(i); last = int(i); }
    return first >= 0 && (last - first + 1) <= 4;
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
    // "S_START comes before every window": since S_START is written exactly
    // once (checked above) and the log is chronological, it is enough that
    // the first window-open sentinel this program uses (S_A for P1/P2/P5,
    // S_P for P3/P6) comes later.
    auto s = find_write(log, S_START, 0x0001);
    if (!(s.found && s.count == 1)) return false;
    for (const auto& x : log) {
        if (x.is_write && (x.word_addr == wa(S_A) || x.word_addr == wa(S_P))) return x.cycle > s.cycle;
    }
    return true; // no window-open sentinel in this program
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

// Opcode word for case c's MOVE.<size> #0,<ea> (the P1-MOVE contrast,
// p1_move.s): same destination <ea> as expected_clr_opcode(c), re-fielded
// into MOVE's own layout (68000 instruction format). MOVE encodes size as
// 01=byte,11=word,10=long (bits 13-12) and the destination as register
// (bits 11-9) then mode (bits 8-6) -- reversed from CLR's mode-then-register
// low byte -- with an immediate source (mode 111, register 100, bits 5-0).
uint16_t expected_move_opcode(int c) {
    uint16_t clr = expected_clr_opcode(c);
    uint8_t clr_size = (clr >> 6) & 0x3;      // 00=B,01=W,10=L (CLR's own encoding)
    uint8_t modereg = clr & 0x3F;              // mmm rrr
    uint8_t mode = (modereg >> 3) & 0x7;
    uint8_t reg  = modereg & 0x7;
    uint16_t move_size = (clr_size == 0) ? 0x1 : (clr_size == 1 ? 0x3 : 0x2);
    uint16_t dest_field = uint16_t((reg << 3) | mode); // bits 11-6: register then mode
    return uint16_t((move_size << 12) | (dest_field << 6) | 0x3C);
}

struct Result {
    bool loaded=false, released=false, done=false;
    std::vector<nmh::RamAccess> log;
    uint32_t xtab[22]={0};
    uint16_t opcode[22]={0};
    uint16_t r_int=0, r_dma=0;
    uint16_t fixture_peek=0; // S_START read back through peek_word, before any other peek is trusted
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
    res.fixture_peek = peekw(h, S_START);
    for (int c = 1; c <= 21; ++c) {
        res.xtab[c] = peekl(h, XTAB + 4u*(c-1));
        res.opcode[c] = last_read(res.log, res.xtab[c]).data;
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

// P1.b, as specified: the last CPU write to the operand word(s) before W_c
// carries the prefill ($A5A5, and $5A5A at T_c+2 for .L), and the readback
// is the CPU write to R_c (and R_c+2 for .L) -- not just memory's final
// value, which a missing prefill (left at $0000 by the memory model) could
// satisfy by accident.
// T_c and R_c are each dedicated to case c (T_BASE+16*c, R_BASE+4*c: no
// other case ever touches them), so the FIRST CPU write to T_c is always
// the prefill (step 1, before the CLR/MOVE that clears it) and the write
// to R_c is always the single readback (step 6) -- in every program
// variant, independent of whether case c's own S_A/S_B window (P1.a) was
// found, so a missing window (P1A) does not also fail P1.b for that case.
bool precond_b_ok(const Result& r, const CaseSpec& cs) {
    uint32_t Tc = T_BASE + 16u*cs.c;
    uint32_t Rc = R_BASE + 4u*cs.c;
    auto pre_lo = first_write(r.log, Tc);
    if (!pre_lo.found || pre_lo.data != 0xA5A5) return false;
    if (cs.size == 'L') {
        auto pre_hi = first_write(r.log, Tc+2);
        if (!pre_hi.found || pre_hi.data != 0x5A5A) return false;
    }
    auto rb_lo = writes_strictly_after(r.log, Rc, 0);
    if (rb_lo.count != 1) return false;
    if (cs.size == 'B') return byte_readback_ok(rb_lo.last_data);
    if (rb_lo.last_data != 0x0000) return false;
    if (cs.size == 'W') return true;
    auto rb_hi = writes_strictly_after(r.log, Rc+2, 0);
    return rb_hi.count == 1 && rb_hi.last_data == 0x0000;
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
    uint16_t fixture_peek=0;
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
    res.fixture_peek = peekw(h, S_START);
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
    uint16_t fixture_peek=0;
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
    res.fixture_peek = peekw(h, S_START);
    for (int k = 1; k <= 4; ++k) {
        res.x[k] = peekl(h, XTAB3 + 4u*(k-1));
        res.opcode[k] = last_read(res.log, res.x[k]).data;
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
    uint16_t fixture_peek=0;
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
    res.fixture_peek = peekw(h, S_START);
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

// P5.a's vector-read clause: inside window e, a CPU read at the long word
// installed as case e's vector (step 2).
int vector_of(int e) { return (e==1)?0x80 : (e==2)?0x10 : (e==3)?0x28 : (e==4)?0x1C : 0x20; }
bool vector_read_ok(const Result& r, const Win& w, int e) {
    return read_count_between(r.log, uint32_t(vector_of(e)), w.a_cycle, w.b_cycle) > 0;
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
    uint16_t opcode[9]={0}; // opcode word at x[1..8]
    uint16_t fixture_peek=0;
    uint16_t guard_opcode=0; // the guard's own first word, read back from the image
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
    res.fixture_peek = peekw(h, S_START);
    for (int k = 1; k <= 8; ++k) res.x[k] = peekl(h, XTAB6 + 4u*(k-1));
    for (int k = 1; k <= 8; ++k) res.opcode[k] = last_read(res.log, res.x[k]).data;
    res.guard_opcode = peekw(h, res.x[1] + 2);
    return res;
}

// Block k's stop kind, 1..4 (BRA.S, JMP, RTS, BEQ.S), matching p3's table:
// P6 runs each kind twice (k, k+1).
int kind_of(int k) { return (k - 1) / 2 + 1; }

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

// g_k: distinct guard words read inside the run window, among the
// guard_len words at W_k, W_k+2, .... guard_len is the guard's own
// assembled length (6 normally; SPECIFICATION.md section 8's own reference
// frame), never more: under P6.e's contrast (MUT_E, guard_len=1) the words
// after the single guard word are the next block's real, executed
// instructions, and counting reads there would count execution, not
// prefetch (validation defect: g_of must count guard words only).
int g_of(const Result& r, const Win& w, uint32_t x_k, int guard_len) {
    int g = 0;
    for (int i = 0; i < guard_len; ++i) {
        uint32_t addr = x_k + 2 + 2u*i;
        if (read_count_between(r.log, addr, w.start, w.end) > 0) ++g;
    }
    return g;
}

// The guard's own assembled length, taken from the image (not from which
// NMH_MUTATE variant was selected): block 1's guard opcode word is either
// $33FC (MOVE.W #imm,(abs).L -- the 6-word guard) or $4AFC (ILLEGAL -- the
// 1-word P6.e contrast).
int guard_len_of(const Result& r) { return (r.guard_opcode == 0x4AFC) ? 1 : 6; }

// P3.a-P3.d, reused for every P6 run (SPECIFICATION.md section 8: "the
// contrasts are the same as in P3"), graded over all 8 blocks.
struct Precond { bool p3a=true, p3b=true, p3c=true, p3d=true; };
Precond common_precond(const Result& r, const Win& win, const std::string& bin) {
    Precond p;
    if (!win.ok) { p.p3a = p.p3b = p.p3c = p.p3d = false; return p; }
    auto img = read_file_bytes(bin);
    auto sx = find_write(r.log, S_X, 0xDEAD);
    auto sf = find_write(r.log, S_FAIL, 0xFA11);
    p.p3b = sx.count == 0 && sf.count == 0;
    for (int k = 1; k <= 8; ++k) {
        p.p3a = p.p3a && block_transferred(r, k);
        p.p3d = p.p3d && p3::opcode_matches_table(kind_of(k), r.opcode[k]);
        uint32_t Wk = r.x[k] + 2;
        if (!static_no_operand_in_range(img, kProgramOrg, Wk, Wk + 10)) p.p3c = false;
    }
    return p;
}

// P6's own alignment precondition: within each pair (blocks 2i-1,2i), the
// two copies' stop instructions lie at addresses of different parity mod 4
// (SPECIFICATION.md section 8: "In one copy, X_k lies at an address
// divisible by 4; in the other, at an address that leaves remainder 2").
bool alignment_ok(const Result& r) {
    for (int pair = 0; pair < 4; ++pair) {
        uint32_t xa = r.x[2*pair+1], xb = r.x[2*pair+2];
        if (xa % 4 == xb % 4) return false;
    }
    return true;
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
        bool peek_ok = r.fixture_peek == 0x0001;
        checks.expect(peek_ok, "P1 fixture: S_START reads back $0001 through peek_word, before any other "
                                "peeked value is trusted (" + core_label + ")");

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
        bool p1a_all = true, p1b_all = true, p1c_all = true, p1c_move_all = true, rel_all = true;
        bool is_move = (mv == "P1-MOVE");
        for (auto& cs : p1::kCases) {
            auto w = p1::window(r, cs.c);
            p1a_all = p1a_all && w.ok;
            p1b_all = p1b_all && p1::precond_b_ok(r, cs);
            if (!is_move) {
                bool c_ok = r.opcode[cs.c] == p1::expected_clr_opcode(cs.c);
                p1c_all = p1c_all && c_ok;
            } else {
                bool m_ok = r.opcode[cs.c] == p1::expected_move_opcode(cs.c);
                p1c_move_all = p1c_move_all && m_ok;
            }
            rel_all = rel_all && w.ok && p1::relation_ok(r, cs, w, false);
        }
        checks.expect(p1a_all, "P1.a: every case's window is a single, ordered S_A/S_B pair (" + core_label + ")");
        checks.expect(p1b_all, "P1.b: every case's operand held the prefill before W_c and the readback (the CPU "
                                "write to R_c) matches the prefill/clear pattern (" + core_label + ")");
        if (!is_move) {
            checks.expect(p1c_all, "P1.c: every case's instruction at X_c is the opcode the table names (" + core_label + ")");
        } else {
            // New assertion (validation defect): the P1-MOVE run must still
            // grade P1.c, against the MOVE opcode assembled in place of
            // CLR -- had the Makefile's -m68010 silently kept assembling
            // CLR here, this would have caught it.
            checks.expect(p1c_move_all, "P1.c (P1-MOVE contrast): every case's instruction at X_c is the MOVE "
                                         "opcode assembled in place of CLR (" + core_label + ")");
        }

        bool p1_precond_ok = fixture_ok && peek_ok && p1a_all && p1b_all &&
                             (is_move ? p1c_move_all : p1c_all) && c1_ok && c2_ok && c3_ok && c4_ok;
        checks.expect_gated(p1_precond_ok, rel_all,
            "P1 [p13,p263] opus:1-guidelines.c60,sol:1-guidelines.c60: on the 68000, every CLR with a "
            "memory operand makes a read access before the write that clears it, in all 21 cases (" + core_label + ")");
        if (is_move) {
            auto w1 = p1::window(r, 1);
            auto acc1 = accesses_between(r.log, T_BASE+16, w1.open, w1.close);
            std::string s;
            for (auto& a : acc1) s += (a.is_write?"W":"R") + std::string("@") + std::to_string(a.cycle) + "=" + std::to_string(a.data) + " ";
            bool any_read = false; for (auto& a : acc1) if (!a.is_write) any_read = true;
            checks.record("P1-MOVE: measured (case 1, .B (An)) " + s + "-- MOVE.B #0,(a0) makes " +
                          std::string(any_read ? "a read then a write, the same pattern as CLR" : "a single write, no read (the page's own contrast)") +
                          "; P1's read-then-write comparator is graded through the unchanged relation above, and "
                          "goes " + std::string(any_read ? "green" : "red") + " here for that reason");
        }

        // ---- P4 [p13]: on the 68020, CLR makes a single write access.
        // Second core, cpu_config=68020 only. The harness documents
        // cpu_config only as "00 = 68000 core behaviour" and claims no
        // 68020 bus or prefetch anywhere, so this run cannot confirm or
        // deny opus/sol:1-guidelines.c59 against p.13/p.263: recorded, not
        // graded (lead decision).
        if (is_tg68k) {
            auto r4 = p1::run(dir + "/p1.bin", 0x03);
            bool fixture4_ok = r4.loaded && r4.released && r4.done;
            checks.expect(fixture4_ok, "P4 fixture: p1.bin loaded, released and reached DONE on the second core");

            int rel4_pass = 0;
            for (auto& cs : p1::kCases) {
                auto w = p1::window(r4, cs.c);
                if (!w.ok) continue;
                if (p1::relation_ok(r4, cs, w, true)) ++rel4_pass;
            }
            auto w4_1 = p1::window(r4, 1);
            auto acc4_1 = accesses_between(r4.log, T_BASE+16, w4_1.open, w4_1.close);
            std::string s4;
            for (auto& a : acc4_1) s4 += (a.is_write?"W":"R") + std::string("@") + std::to_string(a.cycle) + "=" + std::to_string(a.data) + " ";
            checks.record("P4 [p13,p263] opus:1-guidelines.c59,sol:1-guidelines.c59 declined: measured under "
                "cpu_config=68020, " + std::to_string(rel4_pass) + "/21 cases showed a single write with no "
                "preceding read; case 1 (.B (An)): " + s4 + "-- the harness documents cpu_config only as "
                "\"00 = 68000 core behaviour\" (nanomig.v:198 comment) and names no 68020 bus or prefetch, and "
                "this run's own RAM-port log is identical, cycle for cycle, to cpu_config=0 (P1's own run above): "
                "a harness-option limit, not a measurement of a 68020 bus against p.13/p.263");
        } else {
            checks.record("P4 [p13]: the first core has no 68020 mode; not run on this core (SPECIFICATION.md section 6)");
        }

    }

    // =======================================================================
    // P2 [p1,p2]: the 68000's data and address registers, and USP, hold 32
    // bits. Graded on the first core and the second core in 68000 mode.
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
        bool peek_ok = r.fixture_peek == 0x0001;
        checks.expect(peek_ok, "P2 fixture: S_START reads back $0001 through peek_word, before any other "
                                "peeked value is trusted (" + core_label + ")");

        uint16_t ri=0, rd=0;
        bool c1_ok = check_c1(r.log, ri), c2_ok = check_c2(r.log, rd);
        bool c3_ok = check_c3(r.log), c4_ok = check_c4(r.log);
        checks.expect(c1_ok, "C1 [p280]: interrupts off (P2 run, " + core_label + ")");
        checks.expect(c2_ok, "C2 [p278]: DMA off (P2 run, " + core_label + ")");
        checks.expect(c3_ok, "C3: S_END written exactly once, S_FAIL never written (P2 run, " + core_label + ")");
        checks.expect(c4_ok, "C4: S_START written exactly once (P2 run, " + core_label + ")");

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
        bool p2_precond_ok = fixture_ok && peek_ok && w.ok && p2b && p2c && c1_ok && c2_ok && c3_ok && c4_ok;
        checks.expect_gated(p2_precond_ok, rel,
            "P2 [p1,p2]: every one of the 17 registers (D0-D7,A0-A7,USP) stores back all 32 bits of its "
            "loaded pattern (" + core_label + ")");
        // opus/sol:1-components.c25 ("while the data bus and ALU are 16
        // bits") and opus/sol:1-components.c6 (processor identity) bundle a
        // clause this package does not test (SPECIFICATION.md section 1):
        // withdrawn whole, not printed on the graded line above.
        checks.record("opus:1-components.c25,sol:1-components.c25,opus:1-components.c6,sol:1-components.c6 "
                       "withdrawn (P2 run, " + core_label + "): bundle a clause not tested at this port -- the "
                       "data-bus/ALU width (c25) and the processor's identity (c6); see SPECIFICATION.md section 1");
        if (is_low) {
            char buf[96];
            std::snprintf(buf, sizeof(buf), "D0 observed {hi,lo}={$%04X,$%04X}, pattern {$%04X,$%04X}",
                          uint16_t(r.m[0]>>16), uint16_t(r.m[0]), uint16_t(p2::kPatterns[0]>>16), uint16_t(p2::kPatterns[0]));
            checks.note(std::string("P2-LOW: ") + buf + " -- compared against the SAME full-pattern oracle "
                        "above, which is why the assertion goes red under this mutation");
        }
    }

    // =======================================================================
    // P3 [p13] opus:1-guidelines.c65,sol:1-guidelines.c65: every family
    // member reads instruction words ahead of the one it executes. Graded
    // on the first core, and the second core in 68000/68010/68020 mode.
    // (opus/sol:1-guidelines.c64, "All 68000 family processors", is
    // withdrawn below: only three of five members are available.)
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
            bool peek_ok = r.fixture_peek == 0x0001;
            checks.expect(peek_ok, "P3 fixture: S_START reads back $0001 through peek_word, before any other "
                                    "peeked value is trusted (" + m.label + ")");

            auto win = p3::run_window(r);
            uint16_t ri=0, rd=0;
            bool c1_ok = check_c1(r.log, ri), c2_ok = check_c2(r.log, rd);
            bool c3_ok = check_c3(r.log), c4_ok = check_c4(r.log);
            checks.expect(c1_ok, "C1 [p280]: interrupts off (P3 run, " + m.label + ")");
            checks.expect(c2_ok, "C2 [p278]: DMA off (P3 run, " + m.label + ")");
            checks.expect(c3_ok, "C3: S_END written exactly once, S_FAIL never written (P3 run, " + m.label + ")");
            checks.expect(c4_ok, "C4: S_START written exactly once (P3 run, " + m.label + ")");

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

            // P3.c, as specified: a static check on the assembled image --
            // no instruction's data operand addresses a word in W_k to
            // W_k+11 -- read from the .bin file on disk, not the memory
            // model. (C2 already holds DMA off, above; the harness's
            // RamAccess carries no requester field, so "only CPU-attributed
            // reads are counted" cannot itself be confirmed here -- reported
            // to the lead, not worked around.)
            bool p3c = true;
            {
                auto img = read_file_bytes(bin);
                for (int k = 1; k <= 4; ++k) {
                    uint32_t Wk = r.x[k] + 2;
                    if (!static_no_operand_in_range(img, kProgramOrg, Wk, Wk + 10)) p3c = false;
                }
            }
            checks.expect(p3c, "P3.c: the assembled image has no instruction whose data operand addresses a "
                                "guard word W_k to W_k+11 (" + m.label + ")");

            int reads_at_z = win.ok ? read_count_between(r.log, FARZ, win.start, win.end) : -1;
            checks.record("P3 measured reads per guard word W_1..W_4 (" + m.label + "): " +
                std::to_string(reads_at_w[1]) + "," + std::to_string(reads_at_w[2]) + "," +
                std::to_string(reads_at_w[3]) + "," + std::to_string(reads_at_w[4]) +
                "; far word Z reads: " + std::to_string(reads_at_z) + " (calibration, not a page prediction)");

            bool any_read_somewhere = (reads_at_w[1]>0)||(reads_at_w[2]>0)||(reads_at_w[3]>0)||(reads_at_w[4]>0);
            bool p3_precond_ok = fixture_ok && peek_ok && win.ok && p3a && p3b && p3c && p3d &&
                                 c1_ok && c2_ok && c3_ok && c4_ok;
            checks.expect_gated(p3_precond_ok, any_read_somewhere,
                "P3 [p13] opus:1-guidelines.c65,sol:1-guidelines.c65: at least one CPU read at a guard word W_k "
                "inside the run, showing the CPU loaded ahead of the instruction it executes (" + m.label + ")");
            checks.expect(reads_at_z == 0,
                "P3 calibration: the far word Z (never referenced, " + std::to_string(FARZ) +
                ") shows zero CPU reads, so the counter can report absence (" + m.label + ")");
        }
        checks.record("opus:1-guidelines.c64,sol:1-guidelines.c64 withdrawn (P3): \"All 68000 family processors\" "
                       "-- the 68000, 68010 and 68020 are graded above; the 68030 and 68040 are not available "
                       "(SPECIFICATION.md section 1)");
    }

    // =======================================================================
    // P5 [p13]: the exception stack frame differs between 68000-family
    // members. (opus/sol:1-guidelines.c52 is withdrawn below: universal
    // over "each member" with only three of five available, and the
    // 68010/68020 labels rest on cpu_config values the harness documents
    // only as "00 = 68000 core behaviour" -- see C5/P4/P6.)
    // =======================================================================
    {
        std::string bin0 = dir + "/p5_68000.bin";
        if (mv == "P5A") bin0 = dir + "/p5_muta.bin";
        else if (mv == "P5C") bin0 = dir + "/p5_mutc.bin";
        auto r0 = p5::run(bin0, 0x00);
        bool fixture0_ok = r0.loaded && r0.released && r0.done;
        checks.expect(fixture0_ok, "P5 fixture: the 68000-image loaded, released and reached DONE (" + core_label + ", 68000 mode)");
        bool peek0_ok = r0.fixture_peek == 0x0001;
        checks.expect(peek0_ok, "P5 fixture: S_START reads back $0001 through peek_word (" + core_label + ", 68000 mode)");
        uint16_t ri0=0, rd0=0;
        bool c1_0 = check_c1(r0.log, ri0), c2_0 = check_c2(r0.log, rd0);
        bool c3_0 = check_c3(r0.log), c4_0 = check_c4(r0.log);
        checks.expect(c1_0, "C1 [p280]: interrupts off (P5 run, " + core_label + ", 68000 mode)");
        checks.expect(c2_0, "C2 [p278]: DMA off (P5 run, " + core_label + ", 68000 mode)");
        checks.expect(c3_0, "C3: S_END written exactly once, S_FAIL never written (P5 run, " + core_label + ", 68000 mode)");
        checks.expect(c4_0, "C4: S_START written exactly once (P5 run, " + core_label + ", 68000 mode)");

        p5::Frame frames0[6];
        uint32_t pre_sp0[6] = {0};
        auto sfail0 = find_write(r0.log, S_FAIL, 0xFA11);
        bool p5a0 = sfail0.count == 0, p5c_normal = true;
        for (int e = 1; e <= 5; ++e) {
            auto w = p5::case_window(r0, e);
            p5a0 = p5a0 && w.ok;
            if (w.ok) {
                p5a0 = p5a0 && p5::vector_read_ok(r0, w, e);
                pre_sp0[e] = w.pre_sp;
                frames0[e] = p5::frame_of(r0, w);
                p5c_normal = p5c_normal && p5::writes_within_stack(frames0[e], w.pre_sp, r0.post_sp[e]);
            } else {
                p5c_normal = false;
            }
        }
        checks.expect(fixture0_ok && p5a0, "P5.a: every case was taken through its installed vector, no write to "
                                           "S_FAIL (68000 mode, " + core_label + ")");
        checks.expect(fixture0_ok && p5c_normal,
            "P5.c: every CPU write inside each case's window lies between POST_SP_e and PRE_SP-2 (68000 mode, " + core_label + ")");
        for (int e=1;e<=5;++e) checks.record("P5 F_" + std::to_string(e) + " (68000 mode, " + core_label + "): " + p5::frame_str(frames0[e]));

        // ---- 68010/68020 runs: only meaningful on the second core ----
        p5::Frame frames1[6], frames2[6];
        bool have_00 = fixture0_ok && peek0_ok && p5a0 && c1_0 && c2_0 && c3_0 && c4_0;
        bool have_10 = false, have_20 = false;
        if (is_tg68k) {
            std::string vbrbin = dir + (mv=="P5B" ? "/p5_mutb.bin" : "/p5_vbr.bin");
            auto r1 = p5::run(vbrbin, 0x01);
            auto r2 = p5::run(dir + "/p5_vbr.bin", 0x03);
            bool fixture1_ok = r1.loaded && r1.released && r1.done;
            bool fixture2_ok = r2.loaded && r2.released && r2.done;
            checks.expect(fixture1_ok, "P5 fixture: the VBR image loaded, released and reached DONE (second core, 68010 mode)");
            checks.expect(fixture2_ok, "P5 fixture: the VBR image loaded, released and reached DONE (second core, 68020 mode)");
            bool peek1_ok = r1.fixture_peek == 0x0001, peek2_ok = r2.fixture_peek == 0x0001;
            checks.expect(peek1_ok, "P5 fixture: S_START reads back $0001 through peek_word (second core, 68010 mode)");
            checks.expect(peek2_ok, "P5 fixture: S_START reads back $0001 through peek_word (second core, 68020 mode)");
            uint16_t ri1=0, rd1=0, ri2=0, rd2=0;
            bool c1_1=check_c1(r1.log,ri1), c2_1=check_c2(r1.log,rd1), c3_1=check_c3(r1.log), c4_1=check_c4(r1.log);
            bool c1_2=check_c1(r2.log,ri2), c2_2=check_c2(r2.log,rd2), c3_2=check_c3(r2.log), c4_2=check_c4(r2.log);
            checks.expect(c1_1, "C1 [p280]: interrupts off (P5 run, second core, 68010 mode)");
            checks.expect(c2_1, "C2 [p278]: DMA off (P5 run, second core, 68010 mode)");
            checks.expect(c3_1, "C3: S_END written exactly once, S_FAIL never written (P5 run, second core, 68010 mode)");
            checks.expect(c4_1, "C4: S_START written exactly once (P5 run, second core, 68010 mode)");
            checks.expect(c1_2, "C1 [p280]: interrupts off (P5 run, second core, 68020 mode)");
            checks.expect(c2_2, "C2 [p278]: DMA off (P5 run, second core, 68020 mode)");
            checks.expect(c3_2, "C3: S_END written exactly once, S_FAIL never written (P5 run, second core, 68020 mode)");
            checks.expect(c4_2, "C4: S_START written exactly once (P5 run, second core, 68020 mode)");

            bool p5a1=true, p5a2=true, p5c1=true, p5c2=true;
            auto sfail1 = find_write(r1.log, S_FAIL, 0xFA11);
            auto sfail2 = find_write(r2.log, S_FAIL, 0xFA11);
            p5a1 = sfail1.count == 0; p5a2 = sfail2.count == 0;
            uint32_t pre_sp1[6] = {0}, pre_sp2[6] = {0};
            for (int e = 1; e <= 5; ++e) {
                auto w1 = p5::case_window(r1, e);
                auto w2 = p5::case_window(r2, e);
                p5a1 = p5a1 && w1.ok;
                p5a2 = p5a2 && w2.ok;
                if (w1.ok) {
                    p5a1 = p5a1 && p5::vector_read_ok(r1, w1, e);
                    pre_sp1[e] = w1.pre_sp;
                    frames1[e] = p5::frame_of(r1, w1);
                    p5c1 = p5c1 && p5::writes_within_stack(frames1[e], w1.pre_sp, r1.post_sp[e]);
                } else p5c1 = false;
                if (w2.ok) {
                    p5a2 = p5a2 && p5::vector_read_ok(r2, w2, e);
                    pre_sp2[e] = w2.pre_sp;
                    frames2[e] = p5::frame_of(r2, w2);
                    p5c2 = p5c2 && p5::writes_within_stack(frames2[e], w2.pre_sp, r2.post_sp[e]);
                } else p5c2 = false;
            }
            checks.expect(fixture1_ok && p5a1, "P5.a: every case was taken through its installed vector, no write "
                                               "to S_FAIL (68010 mode)");
            checks.expect(fixture2_ok && p5a2, "P5.a: every case was taken through its installed vector, no write "
                                               "to S_FAIL (68020 mode)");
            checks.expect(fixture1_ok && p5c1, "P5.c: every CPU write inside each case's window lies between "
                                               "POST_SP_e and PRE_SP-2 (68010 mode)");
            checks.expect(fixture2_ok && p5c2, "P5.c: every CPU write inside each case's window lies between "
                                               "POST_SP_e and PRE_SP-2 (68020 mode)");
            for (int e=1;e<=5;++e) checks.record("P5 F_" + std::to_string(e) + " (68010 mode): " + p5::frame_str(frames1[e]));
            for (int e=1;e<=5;++e) checks.record("P5 F_" + std::to_string(e) + " (68020 mode): " + p5::frame_str(frames2[e]));
            have_10 = fixture1_ok && peek1_ok && p5a1 && c1_1 && c2_1 && c3_1 && c4_1;
            have_20 = fixture2_ok && peek2_ok && p5a2 && c1_2 && c2_2 && c3_2 && c4_2;

            // P5.b, dynamic half: PRE_SP equal across the 68000/68010/68020
            // runs, for every e = 1..5 (not just e=1). The SAME equality
            // oracle is used regardless of NMH_MUTATE: under P5B, r1 comes
            // from p5_mutb.bin (SUBQ.L #4,A7 inserted), so PRE_SP there
            // genuinely differs and this goes red on its own, for e=1 and
            // (SP stays offset) every later e too.
            bool p5b_dyn = true;
            for (int e = 1; e <= 5; ++e) {
                auto w0 = p5::case_window(r0, e);
                if (!(w0.ok)) { p5b_dyn = false; continue; }
                if (pre_sp0[e] != pre_sp1[e] || pre_sp0[e] != pre_sp2[e]) p5b_dyn = false;
            }
            // P5.b, static half: the assembled 68000-image and VBR-image
            // binaries are equal except for exactly the one word pair of
            // step 3 (fixed files, not the mutant variants: the mutation is
            // exercised through the dynamic half above).
            auto img68000 = read_file_bytes(dir + "/p5_68000.bin");
            auto imgvbr    = read_file_bytes(dir + "/p5_vbr.bin");
            bool p5b_static = images_equal_except_one_word_pair(img68000, imgvbr);
            bool p5b = p5b_dyn && p5b_static;
            checks.expect(p5b, "P5.b: PRE_SP is the same in every run for every e=1..5, and the images are equal "
                               "except for the two words of step 3");

            bool precond_00_10 = have_00 && have_10 && p5b;
            bool precond_00_20 = have_00 && have_20 && p5b;
            bool precond_10_20 = have_10 && have_20 && p5b;
            bool differ_00_10 = false, differ_00_20 = false, differ_10_20 = false;
            for (int e=1;e<=5;++e) {
                if (p5::frames_differ(frames0[e], frames1[e])) differ_00_10 = true;
                if (p5::frames_differ(frames0[e], frames2[e])) differ_00_20 = true;
                if (p5::frames_differ(frames1[e], frames2[e])) differ_10_20 = true;
            }
            checks.expect_gated(precond_00_10, differ_00_10,
                "P5 [p13]: for the pair (68000,68010), at least one exception writes a different frame (second core)");
            checks.expect_gated(precond_00_20, differ_00_20,
                "P5 [p13]: for the pair (68000,68020), at least one exception writes a different frame (second core)");
            checks.expect_gated(precond_10_20, differ_10_20,
                "P5 [p13]: for the pair (68010,68020), at least one exception writes a different frame (second core)");
            checks.record("opus:1-guidelines.c52,sol:1-guidelines.c52 withdrawn (P5, second core): \"different on "
                          "each member\" is universal over five members, three available (68030/68040 absent), "
                          "and the 68010/68020 labels rest on cpu_config values the harness documents only as "
                          "\"00 = 68000 core behaviour\" (see the P4/P6 harness-limit records above)");

        } else {
            checks.record("P5 CONTRAST data (first core, 68000 mode) recorded above as F_1..F_5; compare against "
                           "the second core's own 68000-mode F_1..F_5 once both outputs exist (both must be equal, "
                           "since two 68000s are predicted to write the same frame)");
        }

    }

    // =======================================================================
    // P6 [p13]: the 68020 prefetches more words than the 68000. Graded on
    // the second core only (68020 vs 68000); first core recorded for the
    // report-level contrast. (opus/sol:1-guidelines.c67 is declined below,
    // the same harness-option limit as P4/c59.)
    // =======================================================================
    {
        std::string bin = dir + "/p6.bin";
        if (mv == "P6E") bin = dir + "/p6_mute.bin";
        else if (mv == "P6ALIGN") bin = dir + "/p6_mutalign.bin";
        auto r = p6::run(bin, 0x00);
        bool fixture_ok = r.loaded && r.released && r.done;
        checks.expect(fixture_ok, "P6 fixture: p6.bin loaded, released and reached DONE (" + core_label + ", 68000 mode)");
        bool peek_ok = r.fixture_peek == 0x0001;
        checks.expect(peek_ok, "P6 fixture: S_START reads back $0001 through peek_word (" + core_label + ", 68000 mode)");
        uint16_t ri=0, rd=0;
        bool c1_ok = check_c1(r.log, ri), c2_ok = check_c2(r.log, rd);
        bool c3_ok = check_c3(r.log), c4_ok = check_c4(r.log);
        checks.expect(c1_ok, "C1 [p280]: interrupts off (P6 run, " + core_label + ", 68000 mode)");
        checks.expect(c2_ok, "C2 [p278]: DMA off (P6 run, " + core_label + ", 68000 mode)");
        checks.expect(c3_ok, "C3: S_END written exactly once, S_FAIL never written (P6 run, " + core_label + ", 68000 mode)");
        checks.expect(c4_ok, "C4: S_START written exactly once (P6 run, " + core_label + ", 68000 mode)");

        auto win = p6::run_window(r);
        auto pc = p6::common_precond(r, win, bin);
        checks.expect(pc.p3a, "P3.a (reused, SPECIFICATION.md section 8): every one of the 8 blocks' stop "
                              "instruction executed and transferred control (" + core_label + ", 68000 mode)");
        checks.expect(pc.p3b, "P3.b (reused): no guard word was executed, S_X and S_FAIL never written (" + core_label + ", 68000 mode)");
        checks.expect(pc.p3c, "P3.c (reused): the assembled image has no instruction whose data operand addresses "
                              "a guard word (" + core_label + ", 68000 mode)");
        checks.expect(pc.p3d, "P3.d (reused): every block's stop instruction at X_k is the opcode family the "
                              "table names (" + core_label + ", 68000 mode)");

        bool align_ok = p6::alignment_ok(r);
        checks.expect(align_ok, "P6 alignment: within each pair, the two copies' X_k lie at addresses of "
                                 "different parity mod 4 (" + core_label + ", 68000 mode)");

        int guard_len = p6::guard_len_of(r); // taken from the image, not from NMH_MUTATE
        int D0 = 0;
        for (int k = 1; k <= 8; ++k) D0 = std::max(D0, win.ok ? p6::g_of(r, win, r.x[k], guard_len) : 0);
        checks.record("P6 D(68000 mode, " + core_label + ") = " + std::to_string(D0) + " (guard length " + std::to_string(guard_len) + ")");

        // Alignment is not one of P6.e's own preconditions (SPECIFICATION.md
        // section 8 lists only P3.a-P3.d): it is asserted above on its own,
        // and does not gate P6.e, so P6.e's own contrast (P6E, which
        // deliberately shortens the guard) is not swallowed by a mutation
        // that -- as a side effect of the shorter guard -- also shifts
        // every X_k's parity.
        bool precond_e = fixture_ok && peek_ok && c1_ok && c2_ok && c3_ok && c4_ok &&
                         pc.p3a && pc.p3b && pc.p3c && pc.p3d;
        checks.expect_gated(precond_e, win.ok && D0 < guard_len,
            "P6.e: D(68000 mode) is less than the guard length (" + std::to_string(guard_len) + " words, " + core_label + ")");

        if (is_tg68k) {
            auto r20 = p6::run(dir + "/p6.bin", 0x03);
            bool fixture20_ok = r20.loaded && r20.released && r20.done;
            checks.expect(fixture20_ok, "P6 fixture: p6.bin loaded, released and reached DONE (second core, 68020 mode)");
            bool peek20_ok = r20.fixture_peek == 0x0001;
            checks.expect(peek20_ok, "P6 fixture: S_START reads back $0001 through peek_word (second core, 68020 mode)");
            uint16_t ri20=0, rd20=0;
            bool c1_20=check_c1(r20.log,ri20), c2_20=check_c2(r20.log,rd20), c3_20=check_c3(r20.log), c4_20=check_c4(r20.log);
            checks.expect(c1_20, "C1 [p280]: interrupts off (P6 run, second core, 68020 mode)");
            checks.expect(c2_20, "C2 [p278]: DMA off (P6 run, second core, 68020 mode)");
            checks.expect(c3_20, "C3: S_END written exactly once, S_FAIL never written (P6 run, second core, 68020 mode)");
            checks.expect(c4_20, "C4: S_START written exactly once (P6 run, second core, 68020 mode)");

            auto win20 = p6::run_window(r20);
            auto pc20 = p6::common_precond(r20, win20, dir + "/p6.bin");
            checks.expect(pc20.p3a, "P3.a (reused): every one of the 8 blocks' stop instruction executed and "
                                    "transferred control (second core, 68020 mode)");
            checks.expect(pc20.p3b, "P3.b (reused): no guard word was executed, S_X and S_FAIL never written (second core, 68020 mode)");
            checks.expect(pc20.p3c, "P3.c (reused): the assembled image has no instruction whose data operand "
                                    "addresses a guard word (second core, 68020 mode)");
            checks.expect(pc20.p3d, "P3.d (reused): every block's stop instruction at X_k is the opcode family "
                                    "the table names (second core, 68020 mode)");
            bool align20_ok = p6::alignment_ok(r20);
            checks.expect(align20_ok, "P6 alignment: within each pair, the two copies' X_k lie at addresses of "
                                       "different parity mod 4 (second core, 68020 mode)");

            int guard_len20 = p6::guard_len_of(r20);
            int D20 = 0;
            for (int k = 1; k <= 8; ++k) D20 = std::max(D20, win20.ok ? p6::g_of(r20, win20, r20.x[k], guard_len20) : 0);
            checks.record("P6 D(68020 mode, second core) = " + std::to_string(D20));

            // P6 [p13] opus:1-guidelines.c67,sol:1-guidelines.c67 declined
            // (lead decision, same reasoning as P4/c59): the harness
            // documents cpu_config only as "00 = 68000 core behaviour" and
            // names no 68020 prefetch depth, so "D(68020 mode) >
            // D(68000 mode)" is recorded, not graded.
            checks.record("P6 [p13] opus:1-guidelines.c67,sol:1-guidelines.c67 declined: measured D(68020 mode) = " +
                std::to_string(D20) + ", D(68000 mode) = " + std::to_string(D0) + " (same image, second core) -- "
                "the harness documents cpu_config only as \"00 = 68000 core behaviour\" and names no 68020 "
                "prefetch depth: a harness-option limit, not a measurement of a documented 68020 against p.13 "
                "(same reasoning as P4/c59)");
        } else {
            checks.record("P6 CONTRAST data (first core) recorded above as D(68000 mode); compare against the "
                           "second core's own 68000-mode D once both outputs exist (neither side is predicted "
                           "'more advanced', so 'more' must not hold)");
        }
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
