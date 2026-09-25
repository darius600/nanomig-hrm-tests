// test_blitter_cpu_sharing.cpp -- ch6-blitter-cpu-sharing. See
// SPECIFICATION.md for every assertion's proposition, program, observed
// value and expected value. This file only implements what
// SPECIFICATION.md already committed to.
//
// NMH_MUTATE selects a mutated program (or configuration) in place of its
// normal counterpart, one assertion at a time; none is exercised by a
// plain `make test`. The normal run prints only the fixture, the
// preconditions and the assertions -- no separate "-mutation" line.
//
// MUTATION FIXTURE stimulus red stored constant changed to DEAD instead of BEEF, via NMH_MUTATE=FIXTURE
// MUTATION A1 stimulus red pointer value changed to a different source address, via NMH_MUTATE=A1
// MUTATION DESC stimulus red the off-by-one capture substituted with the same last-word capture (no displacement to compare against), via NMH_MUTATE=DESC
// MUTATION A2 stimulus red blit shrunk to a near-instant one-word transfer, via NMH_MUTATE=A2
// MUTATION A3 stimulus red blit size grown from one word to two hundred, via NMH_MUTATE=A3
// MUTATION A3PRE stimulus red dummy chip-memory read moved before the blit is started, via NMH_MUTATE=A3PRE
// MUTATION BBUSY-PREDICTS stimulus red the true 200-word blit substituted with the one-word build (already finished, BBUSY clear), via NMH_MUTATE=BBUSY-PREDICTS
// MUTATION A4 stimulus red BLTPRI cleared (nasty -> share), via NMH_MUTATE=A4
// MUTATION A6 stimulus red six aligned planes substituted for four inside the fetch window, via NMH_MUTATE=A6
// MUTATION A8 stimulus red baseline capture substituted with the four-lores-plane in-window accesses (live, all even-slot, same unchanged comparator), via NMH_MUTATE=A8
// MUTATION A9 stimulus red bitplane resolution changed from low to high, via NMH_MUTATE=A9
// MUTATION A9PRE stimulus red bitplane program substituted with one that never enables bitplane DMA, via NMH_MUTATE=A9PRE
// MUTATION A10 stimulus red bitplane resolution changed from high to low, via NMH_MUTATE=A10
// MUTATION A10PRE stimulus red bitplane program substituted with one that never enables bitplane DMA, via NMH_MUTATE=A10PRE
// MUTATION A11 stimulus red both sides run the same four-bitplane config, via NMH_MUTATE=A11
// MUTATION PARITY stimulus red no-DMA and share-window captures substituted with the four-lores-plane in-window accesses (live, all even-slot, same unchanged comparator), via NMH_MUTATE=PARITY
// MUTATION SYSDMA-C29 stimulus red no-DMA baseline substituted with the whole-4-clock-aligned loop (no fractional-slot instruction, no missed cycle), via NMH_MUTATE=SYSDMA-C29
// MUTATION SLOTSPAN stimulus red baseline capture substituted with the share-mode blit's own active-window CPU accesses (contended, 5 slots apart), via NMH_MUTATE=SLOTSPAN
// MUTATION OPEN50 stimulus red six-plane side substituted with the four-plane capture (no reduction), via NMH_MUTATE=OPEN50
// MUTATION ORDER stimulus red the 500-word CPU copy loop substituted with a 1-word copy (finishes before the blit), via NMH_MUTATE=ORDER
// MUTATION TAS stimulus red TAS.B replaced by an ordinary, separately-arbitrated read-then-write pair of the same byte under the same share-mode contention, via NMH_MUTATE=TAS
//
// A4's precondition (the nasty-mode window's own length) is not exercised
// by a dedicated mutation: every capture in this package that shrinks the
// window also changes BLTPRI or the display configuration, which would
// flip the main assertion rather than isolate the window-length
// precondition alone -- not exercised, by a scope decision. A5 is
// withdrawn (see the A5-WITHDRAWN record below); it carries no assertion
// and no mutation. ch6-multi.c4 is withdrawn (see the MULTI-C4-WITHDRAWN
// record below); it carries no assertion and no mutation.
// 6-blitter-done-flag.c8/ch6-done.c9 is withdrawn (see the
// FASTRUN-WITHDRAWN record below); it carries no assertion and no
// mutation.

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <iostream>
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

constexpr uint32_t kResults   = 0x0C0000;
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kDoneRunBudget = 3'000'000;

// prog_sharing.s / prog_bitplane.s addresses (see those sources).
constexpr uint32_t kShareScratch = 0x0C0100;
constexpr uint32_t kShareDstLo   = 0x0C4000;
constexpr uint32_t kShareDstHi   = 0x0C4000 + 2 * 500; // 500 words
constexpr uint64_t kShareRunBudget = 3'000'000;

constexpr uint32_t kBpScratch = 0x0C0100;
constexpr uint32_t kBpPtrLo   = 0x0C6000;
constexpr uint32_t kBpPtrHi   = 0x0C6000 + 2 * 32768; // generous: pointer climbs across frames
constexpr uint64_t kBpRunBudget = 4'000'000;

// prog_earlycheck.s addresses. The SMALL build's blit is one word; the
// LARGE build's (height=200,width=1, zero modulo) is 200 contiguous words.
constexpr uint32_t kEcDst     = 0x0C2100;
constexpr uint32_t kEcDstLargeN = 200;
constexpr uint32_t kEcDummy   = 0x0C0002;

// prog_copyloop.s addresses.
constexpr uint32_t kClDst = 0x0C8800;
constexpr uint32_t kClN   = 500;

// prog_tas_share.s address.
constexpr uint32_t kTasAddr = 0x0C2000;

// Every program in this package is well under 512 bytes; PROGRAM_ORG is
// fixed at $080400 (nanomig.i). A read in this range is a CPU instruction
// fetch, not a DMA access.
constexpr uint32_t kProgLo = 0x080400;
constexpr uint32_t kProgHi = 0x080600;

// amiga_clk.v: cck toggles once every 4 clk_sys, so one colour clock (one
// memory slot) is 8 clk_sys.
constexpr uint64_t kCck = 8;

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

uint32_t word_addr(uint32_t byte_addr) { return byte_addr / 2; }

bool is_cpu_access(const nmh::RamAccess& a, uint32_t scratch_word) {
    return (!a.is_write && a.word_addr >= word_addr(kProgLo) && a.word_addr < word_addr(kProgHi)) ||
           (a.is_write && a.word_addr == scratch_word);
}

// ---- generic capture -------------------------------------------------

struct Capture {
    bool loaded = false;
    bool released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
    std::vector<std::optional<uint16_t>> peeks;
};

Capture run_capture(const std::string& bin, uint64_t run_budget, bool wait_done,
                     const std::vector<uint32_t>& peek_addrs, nmh::Config cfg = nmh::Config{}) {
    Capture c;
    nmh::NanomigHarness h(cfg);
    c.loaded = h.load_program(bin) > 0;
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    if (wait_done) {
        c.done = h.run_until_write(kDoneAddr, kDoneValue, run_budget);
    } else {
        h.run_cycles(run_budget);
    }
    // Not coalesced here: coalesce() would also merge two genuinely
    // separate accesses that happen to share an address/data pair (e.g.
    // refresh revisiting the same address), which this package's window
    // and gap counts depend on being distinct. TAS's own analysis (the one
    // place a single logical cycle is expected to span multiple samples)
    // coalesces its own narrow, single-address slice instead, below.
    c.log = h.ram_log();
    for (auto a : peek_addrs) c.peeks.push_back(h.peek_logical(a));
    return c;
}

// ---- analysis helpers --------------------------------------------------

std::vector<uint64_t> write_cycles_in_range(const std::vector<nmh::RamAccess>& log,
                                             uint32_t word_lo, uint32_t word_hi) {
    std::vector<uint64_t> out;
    for (const auto& a : log)
        if (a.is_write && a.word_addr >= word_lo && a.word_addr < word_hi)
            out.push_back(a.cycle);
    return out;
}

std::vector<uint64_t> read_cycles_in_range(const std::vector<nmh::RamAccess>& log,
                                            uint32_t word_lo, uint32_t word_hi) {
    std::vector<uint64_t> out;
    for (const auto& a : log)
        if (!a.is_write && a.word_addr >= word_lo && a.word_addr < word_hi)
            out.push_back(a.cycle);
    return out;
}

size_t count_in_window(const std::vector<uint64_t>& cycles, uint64_t lo, uint64_t hi) {
    size_t n = 0;
    for (auto c : cycles) if (c >= lo && c <= hi) ++n;
    return n;
}

uint64_t median(std::vector<uint64_t> v) {
    if (v.empty()) return 0;
    std::sort(v.begin(), v.end());
    return v[v.size() / 2];
}

// Most frequent value (the histogram's mode), not the middle of the sorted
// list -- used where a single dominant, structurally-recurring value is
// the thing being compared (loop periods, access spacings), and a few
// outlier gaps (a line wrap, a refresh collision) should not move it.
uint64_t mode_of(const std::vector<uint64_t>& v) {
    if (v.empty()) return 0;
    std::vector<uint64_t> s = v;
    std::sort(s.begin(), s.end());
    uint64_t best_v = s[0]; size_t best_n = 0, n = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        if (i == 0 || s[i] != s[i - 1]) n = 0;
        ++n;
        if (n > best_n) { best_n = n; best_v = s[i]; }
    }
    return best_v;
}

// ---- p.197's per-line fetch windows -------------------------------------
//
// Groups a chronological list of BPL1-range read cycles into per-line
// bursts (a gap over `gap` clk_sys starts a new burst), then derives each
// burst's own window: from `lead` slots before its first read to its last
// read, where `lead` is the burst's own modal read-to-read spacing minus
// one slot. Figure 6-11 (six-bitplane, p.197) and Figure 6-12
// (high-resolution four-bitplane, p.197) both show BPL1 fetched last in
// its own repeating group -- so that group's length is exactly the
// spacing between consecutive BPL1 reads, whatever the resolution or plane
// count, and "lead = spacing - 1 slot" recovers the whole group's start
// without a resolution-specific constant.

struct Burst { uint64_t first, last; size_t count; };

std::vector<Burst> group_bursts(const std::vector<uint64_t>& cycles, uint64_t gap = 200) {
    std::vector<Burst> out;
    if (cycles.empty()) return out;
    uint64_t s = cycles[0], p = cycles[0]; size_t n = 1;
    for (size_t i = 1; i < cycles.size(); ++i) {
        uint64_t c = cycles[i];
        if (c - p > gap) { out.push_back({s, p, n}); s = c; n = 0; }
        p = c; ++n;
    }
    out.push_back({s, p, n});
    return out;
}

struct FetchWindow { uint64_t lo, hi; };

std::vector<FetchWindow> fetch_windows(const std::vector<uint64_t>& bpl_reads) {
    std::vector<FetchWindow> out;
    for (auto& b : group_bursts(bpl_reads)) {
        if (b.count < 2) continue;
        uint64_t spacing = (b.last - b.first) / (b.count - 1);
        uint64_t lead = spacing > kCck ? spacing - kCck : 0;
        out.push_back({b.first - lead, b.last});
    }
    return out;
}

struct WinCounts { size_t cpu = 0, dma = 0, total = 0; };

// Counts every RAM-port access inside [lo,hi] (inclusive), splitting CPU
// (program fetches in kProgLo..kProgHi, or writes to `scratch_word`) from
// everything else (bitplane DMA, refresh, other bitplane pointers). `cyc`
// is log's cycle column, already sorted (the log is chronological), so
// the window's slice is found by binary search once per window.
WinCounts count_window(const std::vector<nmh::RamAccess>& log, const std::vector<uint64_t>& cyc,
                        uint64_t lo, uint64_t hi, uint32_t scratch_word) {
    size_t i = std::lower_bound(cyc.begin(), cyc.end(), lo) - cyc.begin();
    size_t j = std::upper_bound(cyc.begin(), cyc.end(), hi) - cyc.begin();
    WinCounts w;
    for (size_t k = i; k < j; ++k) {
        ++w.total;
        if (is_cpu_access(log[k], scratch_word)) ++w.cpu; else ++w.dma;
    }
    return w;
}

std::vector<uint64_t> cyc_of(const Capture& c) {
    std::vector<uint64_t> cyc; cyc.reserve(c.log.size());
    for (auto& a : c.log) cyc.push_back(a.cycle);
    return cyc;
}

std::vector<uint64_t> bp_reads_of(const Capture& c) {
    return read_cycles_in_range(c.log, word_addr(kBpPtrLo), word_addr(kBpPtrHi));
}

size_t bp_scratch_count(const Capture& c) {
    return write_cycles_in_range(c.log, word_addr(kBpScratch), word_addr(kBpScratch) + 1).size();
}

// ---- p.194/Figure 6-9's per-line slot numbering -------------------------
//
// Refresh reads are RAM-port reads below word address $1000 -- far below
// every program-specific range this package uses (kProgLo/kProgHi, every
// scratch word, every blit source/destination, BPL1PTR): 4 reads per
// line, 2 slots apart, grouped here by a gap over 100 clk_sys starting a
// new line. Figure 6-9 draws refresh on odd-numbered slots, the last one
// at $05. A CPU access is sampled at cycle == 5 (mod 8), a DMA (refresh)
// access at cycle == 1 (mod 8), so a CPU cycle minus a refresh cycle is
// always == 4 (mod 8), never an exact multiple of `kCck` -- it is the
// PARITY of floor((cpu_cycle - line's own last refresh cycle) / kCck),
// not its remainder, that identifies the slot: an odd floored offset from
// the line's own last refresh read lands on an even-numbered slot (see
// SPECIFICATION.md, "How slots are numbered at the RAM port").

int64_t floordiv(int64_t a, int64_t b) {
    int64_t q = a / b, r = a % b;
    if (r != 0 && ((r < 0) != (b < 0))) --q;
    return q;
}

struct LineGroup { uint64_t first, last; };

struct LineRef {
    std::vector<uint64_t> firsts;
    std::vector<LineGroup> groups;
};

LineRef make_line_ref(const std::vector<nmh::RamAccess>& log) {
    LineRef ref;
    std::vector<uint64_t> refresh;
    for (const auto& a : log) if (!a.is_write && a.word_addr < 0x1000) refresh.push_back(a.cycle);
    if (refresh.empty()) return ref;
    uint64_t s = refresh[0], p = refresh[0];
    for (size_t i = 1; i < refresh.size(); ++i) {
        uint64_t c = refresh[i];
        if (c - p > 100) { ref.groups.push_back({s, p}); s = c; }
        p = c;
    }
    ref.groups.push_back({s, p});
    for (auto& g : ref.groups) ref.firsts.push_back(g.first);
    return ref;
}

// The line-group index a cycle belongs to (the most recent group whose own
// first refresh read precedes it), or -1 if it precedes every line found.
int64_t line_of(const LineRef& ref, uint64_t cycle) {
    size_t i = std::upper_bound(ref.firsts.begin(), ref.firsts.end(), cycle) - ref.firsts.begin();
    return i == 0 ? -1 : (int64_t)i - 1;
}

// True when a line boundary (some line's own first refresh read) falls
// strictly between two chronologically-ordered cycles -- such a pair
// spans two lines, so a fixed-cycle reference's parity is not comparable
// across it (Figure 6-9's numbering restarts every line, p.194: 227
// slots per line, an odd count).
bool crosses_line(const LineRef& ref, uint64_t prev, uint64_t cur) {
    size_t i = std::upper_bound(ref.firsts.begin(), ref.firsts.end(), prev) - ref.firsts.begin();
    return i < ref.firsts.size() && ref.firsts[i] <= cur;
}

// True if `cycle` (a CPU chip-bus access) is on an even-numbered slot by
// Figure 6-9's own per-line numbering; false (with *known = false) if it
// precedes every line this capture found.
bool is_even_slot(const LineRef& ref, uint64_t cycle, bool* known = nullptr) {
    int64_t i = line_of(ref, cycle);
    if (known) *known = i >= 0;
    if (i < 0) return false;
    int64_t d = floordiv((int64_t)cycle - (int64_t)ref.groups[(size_t)i].last, (int64_t)kCck);
    return (d % 2) != 0; // odd slot-offset from the line's own last refresh read => even slot
}

struct PairStats { size_t pairs = 0, odd = 0; };

// Consecutive-pair slot parity (A8/SYSDMA-C29's shared comparator): among
// `cycles` (already sorted, chronological CPU accesses), counts pairs
// whose gap is a whole number of slots and that do not cross a line
// boundary (see crosses_line), splitting even-slot-spacing (`pairs -
// odd`) from odd-slot-spacing (`odd`) pairs.
PairStats within_line_parity(const LineRef& ref, const std::vector<uint64_t>& cycles) {
    PairStats s;
    for (size_t i = 1; i < cycles.size(); ++i) {
        uint64_t prev = cycles[i - 1], cur = cycles[i];
        uint64_t gap = cur - prev;
        if (gap % kCck != 0) continue;
        if (crosses_line(ref, prev, cur)) continue;
        ++s.pairs;
        if ((gap / kCck) % 2 == 1) ++s.odd;
    }
    return s;
}

std::vector<uint64_t> sorted_cpu_cycles(const Capture& cap, uint32_t scratch_word) {
    std::vector<uint64_t> v;
    for (const auto& a : cap.log) if (is_cpu_access(a, scratch_word)) v.push_back(a.cycle);
    std::sort(v.begin(), v.end());
    return v;
}

// The four-lores-plane build's own CPU chip-bus accesses that fall inside
// its own p.197 fetch windows -- a live (nonzero), independently-live
// dataset that p.196 predicts is entirely even-slot. Used as the
// live-input mutation for A8 and PARITY: the same, unchanged comparator
// fed a different, genuinely-occupied capture.
std::vector<uint64_t> lores_inwindow_cpu_cycles(const Capture& cap_lores4) {
    auto wins = fetch_windows(bp_reads_of(cap_lores4));
    std::vector<uint64_t> out;
    for (const auto& a : cap_lores4.log) {
        if (!is_cpu_access(a, word_addr(kBpScratch))) continue;
        for (auto& w : wins) {
            if (a.cycle >= w.lo && a.cycle <= w.hi) { out.push_back(a.cycle); break; }
        }
    }
    std::sort(out.begin(), out.end());
    return out;
}

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";
    const bool is_tg68k = std::string(NMH_XSTR(NANOMIG_CORE_NAME)) == "tg68k";

    // ---- FIXTURE (no claim id) -----------------------------------------
    {
        std::string bin = dir + (mv == "FIXTURE" ? "/prog_fixture_mut.bin" : "/prog_fixture.bin");
        Capture c = run_capture(bin, kBootBudget + kDoneRunBudget, true, {kResults, 0x0C2100});
        bool ok = c.loaded && c.done.has_value() &&
                  c.peeks.size() == 2 && c.peeks[0] && *c.peeks[0] == 0xBEEF &&
                  c.peeks[1] && *c.peeks[1] == 0xDEAD;
        checks.expect(ok, "FIXTURE: a stored constant and a known one-word A-to-D blit "
                           "both read back correctly [not graded -- apparatus check, no claim uid]");
    }

    // ---- A1 [p170] pointer register: adjacent words, one 32-bit write --
    {
        std::string bin = dir + (mv == "A1" ? "/prog_pointer_mutant.bin" : "/prog_pointer.bin");
        Capture primary = run_capture(bin, kBootBudget + kDoneRunBudget, true, {kResults});
        bool primary_ok = primary.loaded && primary.done.has_value() &&
                           primary.peeks.size() == 1 && primary.peeks[0] && *primary.peeks[0] == 0xCAFE;
        checks.expect(primary_ok,
            "A1 [p170] 6-dma-channels.c16,6-dma-channels.c18,ch6-dma.c14,ch6-dma.c16: "
            "a single 32-bit MOVE.L to BLTAPTH sets both pointer words; the blit reads the "
            "intended source word ($CAFE)");
    }

    // ---- DESC-BASE / DESC-OFFBY1 [p182] ch6-desc.c12: the blitter's own
    // descending-mode pointer convention (BLTAPTH/BLTDPTH at the LAST
    // word, BLTCON1 bit 1, agnus_blitter.v:286) differs from the 680x0
    // -(An) predecrement convention (which would leave a pointer one word
    // past the last): starting one word too high displaces the whole
    // descending copy by one word, observable in ram_log() as which
    // destination words are written.
    {
        bool mutated = (mv == "DESC");
        Capture db_a = run_capture(dir + "/prog_descblit.bin", kBootBudget + kDoneRunBudget, true,
                                    {0x0C2100, 0x0C2102, 0x0C2104, 0x0C2106, 0x0C2108});
        Capture db_b = run_capture(dir + (mutated ? "/prog_descblit.bin" : "/prog_descblit_offby1.bin"),
                                    kBootBudget + kDoneRunBudget, true, {});

        bool a_ok = db_a.loaded && db_a.done.has_value() && db_a.peeks.size() == 5;
        bool a_faithful = a_ok && db_a.peeks[0] && *db_a.peeks[0] == 0x1111 &&
                           db_a.peeks[1] && *db_a.peeks[1] == 0x2222 &&
                           db_a.peeks[2] && *db_a.peeks[2] == 0x3333 &&
                           db_a.peeks[3] && *db_a.peeks[3] == 0x4444 &&
                           db_a.peeks[4] && *db_a.peeks[4] == 0x0000; // one past the block, untouched
        checks.expect(a_faithful,
            "DESC-BASE [p182]: the blitter's own descending-mode pointer convention (BLTAPTH/"
            "BLTDPTH at the last word) copies a known 4-word pattern faithfully, leaving the word "
            "one past the block untouched");

        constexpr uint32_t kDbDst = 0x0C2100;
        auto written = [](const Capture& c, uint32_t byte_addr) {
            return !write_cycles_in_range(c.log, word_addr(byte_addr), word_addr(byte_addr) + 1).empty();
        };
        bool a_first = written(db_a, kDbDst);
        bool a_extra = written(db_a, kDbDst + 8);
        bool b_first = written(db_b, kDbDst);
        bool b_extra = written(db_b, kDbDst + 8);
        checks.note("DESC-OFFBY1: A (pointer at last word) writes first=" + std::string(a_first ? "yes" : "no") +
                    " extra=" + std::string(a_extra ? "yes" : "no") + "; B (" +
                    (mutated ? "same capture as A" : "pointer one word past the last") +
                    ") writes first=" + std::string(b_first ? "yes" : "no") +
                    " extra=" + std::string(b_extra ? "yes" : "no"));
        checks.expect(a_first && !a_extra && !b_first && b_extra,
            "DESC-OFFBY1 [p182] ch6-desc.c12: pointing BLTAPTH/BLTDPTH one word past the last word "
            "(the 680x0 -(An) predecrement convention) displaces the same descending copy by one "
            "word -- the block's own first destination word goes unwritten and the word past it is "
            "written instead, where the blitter's own convention (pointer at the last word) writes "
            "the first word and leaves that extra one alone");
    }

    // ---- A3 [p187] a small blit can finish before the CPU's first check,
    // A3PRE (its own precondition, own mutation), and BBUSY-PREDICTS
    // [p186], which always needs the true "large" (200-word) capture.
    {
        // A3's own capture: normally the true SMALL (one-word) build;
        // under NMH_MUTATE=A3 substituted with the LARGE (200-word)
        // build, so the unchanged "BBUSY reads clear" check must go red.
        Capture a3_cap = run_capture(dir + (mv == "A3" ? "/prog_earlycheck_large.bin" : "/prog_earlycheck.bin"),
                                      kBootBudget + kDoneRunBudget, true, {kResults});
        bool a3_ok = a3_cap.loaded && a3_cap.done.has_value() && a3_cap.peeks.size() == 1 &&
                     a3_cap.peeks[0].has_value();
        bool a3_busy = a3_ok && (*a3_cap.peeks[0] & 0x4000);
        checks.expect(a3_ok && !a3_busy,
            "A3 [p187] ch6-multi.c3: a one-word blit's own busy bit (DMACONR bit 14) already "
            "reads clear on the CPU's first, immediate check after starting it");

        // A3PRE's own capture: normally the true SMALL build; under
        // NMH_MUTATE=A3PRE substituted with a build that performs the
        // dummy read before the blit is even started, so the unchanged
        // "destination write precedes the dummy read" check must go red.
        Capture pre_cap = run_capture(dir + (mv == "A3PRE" ? "/prog_earlycheck_reordered.bin" : "/prog_earlycheck.bin"),
                                       kBootBudget + kDoneRunBudget, true, {kResults});
        auto dst_w = write_cycles_in_range(pre_cap.log, word_addr(kEcDst), word_addr(kEcDst) + 1);
        auto dummy_r = read_cycles_in_range(pre_cap.log, word_addr(kEcDummy), word_addr(kEcDummy) + 1);
        bool a3_pre = pre_cap.loaded && pre_cap.done.has_value() && !dst_w.empty() && !dummy_r.empty() &&
                      dst_w.front() < dummy_r.front();
        checks.expect(a3_pre,
            "A3PRE [p187] ch6-multi.c3: the blit's destination write is already in "
            "ram_log() before the manual's prescribed dummy chip-memory read");

        // BBUSY-PREDICTS's own capture: normally the true LARGE (200-word)
        // build; under NMH_MUTATE=BBUSY-PREDICTS substituted with the true
        // SMALL (one-word) build, already finished (BBUSY clear), so the
        // unchanged "BBUSY set, more writes still to come" check must go
        // red. The bound is the RESULTS store's own cycle (the moment the
        // read BBUSY value was actually captured), not the earlier dummy
        // read.
        bool mutated_bbusy = (mv == "BBUSY-PREDICTS");
        Capture bbusy_cap = run_capture(dir + (mutated_bbusy ? "/prog_earlycheck.bin" : "/prog_earlycheck_large.bin"),
                                         kBootBudget + kDoneRunBudget, true, {kResults});
        bool bbusy_ok = bbusy_cap.loaded && bbusy_cap.done.has_value() && bbusy_cap.peeks.size() == 1 &&
                         bbusy_cap.peeks[0].has_value();
        bool bbusy_busy = bbusy_ok && (*bbusy_cap.peeks[0] & 0x4000);
        uint32_t dst_n = mutated_bbusy ? 1 : kEcDstLargeN;
        auto bbusy_dst_w = write_cycles_in_range(bbusy_cap.log, word_addr(kEcDst), word_addr(kEcDst) + dst_n);
        auto bbusy_res_w = write_cycles_in_range(bbusy_cap.log, word_addr(kResults), word_addr(kResults) + 1);
        uint64_t last_dst_write = bbusy_dst_w.empty() ? 0 : bbusy_dst_w.back();
        uint64_t results_cycle = bbusy_res_w.empty() ? 0 : bbusy_res_w.front();
        bool writes_still_incoming = !bbusy_dst_w.empty() && last_dst_write > results_cycle;
        checks.note("BBUSY-PREDICTS: " + std::string(mutated_bbusy ? "one-word build" : "200-word build") +
                    " -- last D-write at cycle " + std::to_string(last_dst_write) +
                    ", RESULTS store at cycle " + std::to_string(results_cycle) +
                    ", " + std::to_string(bbusy_dst_w.size()) + " D-writes total");
        checks.expect(bbusy_ok && bbusy_busy && writes_still_incoming,
            "BBUSY-PREDICTS [p186] 6-blitter-done-flag.c10,ch6-done.c12: after the "
            "prescribed intervening read, BBUSY reading set correctly predicts more destination "
            "writes still to come in ram_log() -- the last D-write (cycle " + std::to_string(last_dst_write) +
            ") is after the RESULTS store that captured the read (cycle " + std::to_string(results_cycle) + ")");
    }

    // ---- load every capture used below, up front --------------------

    Capture cap_base   = run_capture(dir + "/prog_sharing.bin", kShareRunBudget, false, {});
    Capture cap_share  = run_capture(dir + "/prog_sharing_share.bin", kShareRunBudget, false, {});
    Capture cap_nasty  = run_capture(dir + "/prog_sharing_nasty.bin", kShareRunBudget, false, {});
    Capture cap_tiny   = run_capture(dir + "/prog_sharing_tiny.bin", kShareRunBudget, false, {});
    Capture cap_base_aligned = run_capture(dir + "/prog_sharing_aligned.bin", kShareRunBudget, false, {});

    Capture cap_lores4 = run_capture(dir + "/prog_bitplane.bin", kBpRunBudget, false, {});
    Capture cap_hires4 = run_capture(dir + "/prog_bitplane_hires.bin", kBpRunBudget, false, {});
    Capture cap_lores6 = run_capture(dir + "/prog_bitplane_planes6.bin", kBpRunBudget, false, {});
    Capture cap_lores4_aligned = run_capture(dir + "/prog_bitplane_aligned.bin", kBpRunBudget, false, {});
    Capture cap_lores6_aligned = run_capture(dir + "/prog_bitplane_planes6_aligned.bin", kBpRunBudget, false, {});

    checks.note("capture health: base.log.size=" + std::to_string(cap_base.log.size()) +
                " share.log.size=" + std::to_string(cap_share.log.size()) +
                " nasty.log.size=" + std::to_string(cap_nasty.log.size()) +
                " tiny.log.size=" + std::to_string(cap_tiny.log.size()) +
                " base_aligned.log.size=" + std::to_string(cap_base_aligned.log.size()));
    checks.note("capture health: lores4.log.size=" + std::to_string(cap_lores4.log.size()) +
                " hires4.log.size=" + std::to_string(cap_hires4.log.size()) +
                " lores6.log.size=" + std::to_string(cap_lores6.log.size()) +
                " lores4_aligned.log.size=" + std::to_string(cap_lores4_aligned.log.size()) +
                " lores6_aligned.log.size=" + std::to_string(cap_lores6_aligned.log.size()));

    auto dst_writes = [](const Capture& c) {
        return write_cycles_in_range(c.log, word_addr(kShareDstLo), word_addr(kShareDstHi));
    };
    auto scratch_writes = [](const Capture& c) {
        return write_cycles_in_range(c.log, word_addr(kShareScratch), word_addr(kShareScratch) + 1);
    };
    auto dst_span = [&](const Capture& c) -> std::pair<uint64_t,uint64_t> {
        auto d = dst_writes(c);
        if (d.empty()) return {0, 0};
        return {*std::min_element(d.begin(), d.end()), *std::max_element(d.begin(), d.end())};
    };

    // ---- A2 [p169,p186] the CPU keeps running while the blitter works --
    {
        const Capture& cap = (mv == "A2") ? cap_tiny : cap_share;
        auto [lo, hi] = dst_span(cap);
        auto sc = scratch_writes(cap);
        std::vector<uint64_t> in_win;
        for (auto c : sc) if (c >= lo && c <= hi) in_win.push_back(c);
        uint64_t max_gap = 0;
        for (size_t i = 1; i < in_win.size(); ++i) max_gap = std::max(max_gap, in_win[i] - in_win[i - 1]);
        uint64_t win_len = (lo == hi) ? 0 : hi - lo;
        checks.note("A2: active window " + std::to_string(lo) + ".." + std::to_string(hi) +
                    " (" + std::to_string(win_len) + " clk_sys), " + std::to_string(in_win.size()) +
                    " scratch writes inside it, largest in-window gap " + std::to_string(max_gap) + " clk_sys");
        // No numeric factor from the page: "half the window" is a
        // deliberately loose sanity bound (the measured gap is a small
        // fraction of it), not a calibrated one -- it only needs to rule
        // out the CPU stopping for a meaningful fraction of the window,
        // which a genuinely halted CPU would fail by a wide margin.
        checks.expect(lo != hi && in_win.size() >= 2 && max_gap * 2 < win_len,
            "A2 [p169,p186] 6-intro.c20,ch6-intro.c21,ch6-intro.c22,6-blitter-done-flag.c2,"
            "6-blitter-done-flag.c3,ch6-done.c2,ch6-done.c3: the CPU keeps writing to its own "
            "counter throughout a share-mode blit's active window, with no gap between "
            "consecutive writes reaching even half the window's own length (largest gap " +
            std::to_string(max_gap) + " of " + std::to_string(win_len) + " clk_sys)");
    }

    // ---- A4 [p194,p198] the blitter has higher DMA priority than the 68000 --
    {
        const Capture& cap = (mv == "A4") ? cap_share : cap_nasty;
        auto [lo, hi] = dst_span(cap);
        auto sc = scratch_writes(cap);
        size_t in_win = count_in_window(sc, lo, hi);
        bool nontrivial = lo != hi && (hi - lo) > 2000;
        checks.expect(nontrivial,
            "A4PRE: the nasty-mode blit's active window is long enough to have contained "
            "several CPU-loop iterations had the CPU been let through (" + std::to_string(hi - lo) + " cycles)");
        checks.expect(nontrivial && in_win == 0,
            "A4 [p194,p198] 6-system-dma.c13,ch6-sysdma.c11,6-blitter-priority.c1,ch6-sysdma.c52: "
            "with BLTPRI set (nasty), the CPU makes no chip-bus progress at all during the blit's "
            "active window (" + std::to_string(in_win) + " scratch writes)");
    }

    // ---- A5 [p198] share mode: the blitter yields after N consecutive
    // denied CPU cycles -- WITHDRAWN. p.198 fixes "three" denials then a
    // released fourth slot, but not which slot after a granted CPU access
    // the "unsatisfied" count starts from. The RAM-port log gives three
    // readings of "unsatisfied", each predicting a different CPU-to-CPU
    // grant spacing in the share-mode window (measured: 5 slots, 4
    // blitter accesses between, fx68k 189 of 241, TG68K 243 of 253):
    //   - every slot counted from g+1: 4 slots between grants, 3 blitter
    //   - every slot counted from g+2 (past Figure 6-10's own internal-
    //     operation portion): 5 slots between grants, 4 blitter
    //   - only the CPU's own even-numbered memory cycles (p.196), from
    //     g+2: 8 slots between grants, 7 blitter
    // The measured 5-slot spacing matches the second reading, but that
    // reading puts every other grant on an ODD slot, which p.196
    // excludes -- no reading agrees with both p.198 and p.196 at once.
    // The page is ambiguous at the point tested (BRIEF.md's halt
    // condition); the odd-slot grants themselves are not lost, though --
    // they are exactly what the corrected PARITY assertion below grades,
    // red on both cores regardless of which of the three readings is
    // meant.
    checks.record(
        "A5-WITHDRAWN [p198] 6-blitter-priority.c13,ch6-sysdma.c53,ch6-sysdma.c62: withdrawn -- "
        "the page does not fix which slot after a granted CPU access the unsatisfied-cycle count "
        "starts from, three readings of it each predict a different grant spacing, and the "
        "measured spacing (5 slots, 4 blitter accesses between, both cores) matches only one of "
        "them -- the one that puts every other CPU grant on an odd slot, which p.196 excludes. "
        "The page is ambiguous at this point; the odd-slot grants themselves are graded by PARITY "
        "below, not lost by this withdrawal");

    // ---- A8 [p194,p196] a denied cycle means the CPU simply waits for
    // its own next available (same-parity) slot -- graded per line
    // (Figure 6-9 restarts slot numbering every line, and a line is 227
    // slots, p.194, an odd count, so a fixed-cycle reference's parity
    // flips every line: a pair spanning a line start is excluded, not
    // counted as a violation).
    {
        bool mutated = (mv == "A8");
        const Capture& src_cap = mutated ? cap_lores4 : cap_base;
        std::vector<uint64_t> cpu_src = mutated ? lores_inwindow_cpu_cycles(cap_lores4)
                                                 : sorted_cpu_cycles(cap_base, word_addr(kShareScratch));
        LineRef lref = make_line_ref(src_cap.log);
        PairStats s = within_line_parity(lref, cpu_src);
        checks.note("A8: " + std::to_string(s.odd) + " odd-slot-spacing instance(s) of " +
                    std::to_string(s.pairs) + " within-line, slot-aligned consecutive-CPU-access "
                    "pairs (" + (mutated ? "four-lores-plane in-window accesses" : "no-DMA baseline") + ")");
        checks.expect(s.pairs > 0 && s.odd == 0,
            "A8 [p194,p196] 6-system-dma.c54,6-system-dma.c55,ch6-sysdma.c30: no two consecutive "
            "CPU bus accesses within one line are an odd number of slots apart -- a denied cycle "
            "always waits for the CPU's own next available (same-parity) slot, never a different "
            "one (" + std::to_string(s.odd) + " odd-spacing instance(s) of " + std::to_string(s.pairs) + ")");
    }

    // ---- SYSDMA-C29 [p196] ch6-sysdma.c29: some 68000 instructions miss
    // the even-cycle allocation. Existential -- fx68k's own no-DMA loop
    // (BRA.S, a 10-clock internal operation, not a whole multiple of 4)
    // shows the fetch after it land on an odd slot once per iteration
    // instead of waiting for the next even one (the same fact A8 grades
    // above) -- a recurring, structural count (thousands of instances),
    // not an isolated one. The whole-4-clock-aligned loop
    // (prog_sharing_aligned.s, JMP (An) in place of BRA.S) shows no such
    // recurring gap -- used here as SYSDMA-C29's own mutation. (A single
    // stray instance can still occur once, at the loop's own startup
    // transient before its steady rhythm settles; requiring more than one
    // instance is what tells a recurring, per-iteration miss apart from
    // that one-off, exactly as `mode_of()` elsewhere in this file discounts
    // a lone outlier gap rather than a structurally-recurring value.)
    // TG68K's own BRA.S-based loop shows no such gap either, so it is
    // reported not exercised there rather than graded.
    {
        bool mutated = (mv == "SYSDMA-C29");
        const Capture& cap = mutated ? cap_base_aligned : cap_base;
        LineRef lref = make_line_ref(cap.log);
        PairStats s = within_line_parity(lref, sorted_cpu_cycles(cap, word_addr(kShareScratch)));
        checks.note("SYSDMA-C29: " + std::to_string(s.odd) + " odd-slot (missed-cycle) instance(s) of " +
                    std::to_string(s.pairs) + " within-line pairs, no-DMA baseline (" +
                    (mutated ? "whole-4-clock-aligned loop" : "BRA.S-based loop") + ")");
        if (!is_tg68k) {
            checks.expect(s.pairs > 0 && s.odd > 1,
                "SYSDMA-C29 [p196] ch6-sysdma.c29: some 68000 instructions miss the even-cycle "
                "allocation -- the fetch after a fractional-slot internal operation (BRA.S) lands "
                "on an odd slot once per loop iteration instead of waiting for the next even one, "
                "recurring rather than a single one-off (" +
                std::to_string(s.odd) + " of " + std::to_string(s.pairs) + " within-line pairs)");
        } else {
            checks.note("SYSDMA-C29 [p196] ch6-sysdma.c29: not exercised on TG68K -- its own "
                        "no-DMA loop shows no odd-slot (missed-cycle) gap within a line");
        }
    }

    // ---- A9 [p196] four or fewer low-resolution bitplanes still let the CPU through
    {
        const Capture& pre_cap = (mv == "A9PRE") ? cap_base : cap_lores4;
        auto pre_wins = fetch_windows(bp_reads_of(pre_cap));
        checks.expect(!pre_wins.empty(),
            "A9PRE [p196]: bitplane fetch windows are actually found (" +
            std::to_string(pre_wins.size()) + ")");

        const Capture& cap = (mv == "A9") ? cap_hires4 : cap_lores4;
        auto wins = fetch_windows(bp_reads_of(cap));
        auto cyc = cyc_of(cap);
        size_t min_cpu = wins.empty() ? 0 : ~size_t(0);
        for (auto& w : wins) min_cpu = std::min(min_cpu, count_window(cap.log, cyc, w.lo, w.hi, word_addr(kBpScratch)).cpu);
        checks.note("A9: " + std::to_string(wins.size()) + " fetch window(s); minimum CPU accesses "
                    "in any one window = " + std::to_string(wins.empty() ? 0 : min_cpu));
        checks.expect(!wins.empty() && min_cpu > 0,
            "A9 [p196] 6-system-dma.c61,ch6-sysdma.c34: with four or fewer low-resolution "
            "bitplanes, the CPU gets at least one chip-bus access (a program fetch or a scratch "
            "write) inside every line's own fetch window (minimum " +
            std::to_string(wins.empty() ? 0 : min_cpu) + " per window)");
    }

    // ---- A10 [p196,p197] four high-resolution bitplanes lock the CPU out
    {
        const Capture& pre_cap = (mv == "A10PRE") ? cap_base : cap_hires4;
        auto pre_wins = fetch_windows(bp_reads_of(pre_cap));
        auto pre_cyc = cyc_of(pre_cap);
        bool pre_all_full = !pre_wins.empty();
        for (auto& w : pre_wins)
            if (count_window(pre_cap.log, pre_cyc, w.lo, w.hi, word_addr(kBpScratch)).dma != 160) pre_all_full = false;
        checks.expect(!pre_wins.empty() && pre_all_full,
            "A10PRE [p196,p197]: every one of the line's 160 fetch-window slots (p.197's "
            "\"40 * 4 = 160 time slots\") is a bitplane fetch, none left over (" +
            std::to_string(pre_wins.size()) + " window(s))");

        const Capture& cap = (mv == "A10") ? cap_lores4 : cap_hires4;
        auto wins = fetch_windows(bp_reads_of(cap));
        auto cyc = cyc_of(cap);
        size_t max_cpu = 0;
        for (auto& w : wins) max_cpu = std::max(max_cpu, count_window(cap.log, cyc, w.lo, w.hi, word_addr(kBpScratch)).cpu);
        checks.note("A10: " + std::to_string(wins.size()) + " fetch window(s); maximum CPU accesses "
                    "in any one window = " + std::to_string(max_cpu));
        checks.expect(!wins.empty() && max_cpu == 0,
            "A10 [p196,p197] 6-system-dma.c71,ch6-sysdma.c42: four high-resolution bitplanes give "
            "the CPU no chip-bus access at all inside any line's fetch window (maximum " +
            std::to_string(max_cpu) + " per window)");
    }

    // ---- A11 [p196] more than four low-resolution bitplanes measurably steal cycles
    {
        size_t lo4 = bp_scratch_count(cap_lores4);
        size_t lo6 = bp_scratch_count((mv == "A11") ? cap_lores4 : cap_lores6);
        checks.expect(lo4 > 0 && lo6 * 100 <= lo4 * 95,
            "A11 [p196] 6-system-dma.c62,ch6-sysdma.c36: six low-resolution bitplanes (" +
            std::to_string(lo6) + " scratch writes) measurably reduce the CPU's chip-bus "
            "accesses relative to four (" + std::to_string(lo4) + ")");
    }

    // ---- A6 [p196] alternate cycles are enough for full speed in the
    // normal case -- the stimulus loop's own bus cycles must all align to
    // whole slots (MC68000 timing tables: ADDQ.W Dn = 4 clocks, MOVE.W
    // Dn,(An) = 8, JMP (An) = 8 -- every one a whole multiple of 4,
    // unlike BRA.S's 10; see SYSDMA-C29 above and prog_sharing_aligned.s).
    {
        const Capture& cap = (mv == "A6") ? cap_lores6_aligned : cap_lores4_aligned;
        auto wins = fetch_windows(bp_reads_of(cap));
        auto sw = write_cycles_in_range(cap.log, word_addr(kBpScratch), word_addr(kBpScratch) + 1);
        std::vector<uint64_t> in_win;
        for (auto& w : wins) for (auto c : sw) if (c >= w.lo && c <= w.hi) in_win.push_back(c);
        std::sort(in_win.begin(), in_win.end());
        in_win.erase(std::unique(in_win.begin(), in_win.end()), in_win.end());
        std::vector<uint64_t> in_win_gaps;
        for (size_t i = 1; i < in_win.size(); ++i) in_win_gaps.push_back(in_win[i] - in_win[i - 1]);
        uint64_t in_win_period = mode_of(in_win_gaps);

        auto base_sc = scratch_writes(cap_base_aligned);
        std::vector<uint64_t> base_gaps;
        for (size_t i = 1; i < base_sc.size(); ++i) base_gaps.push_back(base_sc[i] - base_sc[i - 1]);
        uint64_t base_period = mode_of(base_gaps);

        checks.note("A6: aligned-loop CPU period inside the fetch window = " + std::to_string(in_win_period) +
                    " clk_sys (modal, " + std::to_string(in_win_gaps.size()) + " gaps); no-DMA period = " +
                    std::to_string(base_period) + " clk_sys (modal, " + std::to_string(base_gaps.size()) + " gaps)");
        checks.expect(in_win_period > 0 && in_win_period == base_period,
            "A6 [p196] 6-system-dma.c53,ch6-sysdma.c28: with a stimulus loop whose own bus cycles "
            "all align to whole slots (MC68000 timing tables), the CPU's loop period inside the "
            "four-lores-plane fetch window (" + std::to_string(in_win_period) +
            " clk_sys) equals its no-DMA period (" + std::to_string(base_period) + " clk_sys)");
    }

    // ---- SLOTSPAN [p196] 6-system-dma.c58: Figure 6-10 divides an
    // average 68000 cycle into an internal-operation half and a
    // memory-access half -- one memory slot, one free slot. Measured as
    // the modal gap between consecutive CPU bus accesses in the no-DMA
    // baseline (or, under NMH_MUTATE=SLOTSPAN, the SAME quantity --
    // consecutive CPU-access gaps -- taken from the share-mode blit's own
    // active window instead, contended and 5 slots apart, not 2).
    {
        std::vector<uint64_t> gaps;
        if (mv == "SLOTSPAN") {
            auto [lo, hi] = dst_span(cap_share);
            std::vector<uint64_t> cpu_cycles;
            for (const auto& a : cap_share.log)
                if (is_cpu_access(a, word_addr(kShareScratch)) && a.cycle >= lo && a.cycle <= hi)
                    cpu_cycles.push_back(a.cycle);
            std::sort(cpu_cycles.begin(), cpu_cycles.end());
            for (size_t i = 1; i < cpu_cycles.size(); ++i) gaps.push_back(cpu_cycles[i] - cpu_cycles[i - 1]);
        } else {
            std::vector<uint64_t> cpu_cycles;
            for (const auto& a : cap_base.log)
                if (is_cpu_access(a, word_addr(kShareScratch))) cpu_cycles.push_back(a.cycle);
            std::sort(cpu_cycles.begin(), cpu_cycles.end());
            for (size_t i = 1; i < cpu_cycles.size(); ++i) gaps.push_back(cpu_cycles[i] - cpu_cycles[i - 1]);
        }
        uint64_t m = mode_of(gaps);
        checks.note("SLOTSPAN: modal consecutive-CPU-access gap (" +
                    std::string(mv == "SLOTSPAN" ? "share-mode blit's own active window" : "no-DMA baseline") +
                    ") = " + std::to_string(m) + " clk_sys (" + std::to_string(m / kCck) + " slot(s))");
        checks.expect(!gaps.empty() && m == 2 * kCck,
            "SLOTSPAN [p196] 6-system-dma.c58: each 68000 bus cycle occupies one memory slot "
            "and leaves the neighbouring slot free -- the modal gap between consecutive CPU "
            "accesses is exactly two slots (" + std::to_string(m) + " clk_sys)");
    }

    // ---- OPEN50 [p196,p197] six bitplanes steal 50% of the open slots four would have left
    {
        auto median_dma_per_window = [](const Capture& c) {
            auto wins = fetch_windows(bp_reads_of(c));
            auto cyc = cyc_of(c);
            std::vector<uint64_t> dma;
            for (auto& w : wins) dma.push_back(count_window(c.log, cyc, w.lo, w.hi, word_addr(kBpScratch)).dma);
            return median(dma);
        };
        uint64_t dma_lo4 = median_dma_per_window(cap_lores4);
        uint64_t dma_lo6 = median_dma_per_window((mv == "OPEN50") ? cap_lores4 : cap_lores6);
        int64_t open_lo4 = 160 - (int64_t)dma_lo4;
        int64_t open_lo6 = 160 - (int64_t)dma_lo6;
        checks.note("OPEN50: median DMA accesses per 160-slot window -- four planes=" + std::to_string(dma_lo4) +
                    " (open=" + std::to_string(open_lo4) + "), six planes=" + std::to_string(dma_lo6) +
                    " (open=" + std::to_string(open_lo6) + ")");
        checks.expect(open_lo4 > 0 && open_lo6 * 2 == open_lo4,
            "OPEN50 [p196,p197] 6-system-dma.c65,ch6-sysdma.c38: six low-resolution bitplanes "
            "steal 50 percent of the open slots four bitplanes would have left (" +
            std::to_string(open_lo6) + " of " + std::to_string(open_lo4) + " open slots remain)");
    }

    // ---- PARITY [p196] 6-system-dma.c50,ch6-sysdma.c25,6-system-dma.c60:
    // the 68000 uses only the even-numbered memory-access cycles, by
    // Figure 6-9's own per-line slot numbering (see "How slots are
    // numbered at the RAM port" in SPECIFICATION.md) -- graded over the
    // no-DMA run and, separately, the share-mode blit's own active
    // window.
    {
        bool mutated = (mv == "PARITY");

        std::vector<uint64_t> base_cpu = mutated ? lores_inwindow_cpu_cycles(cap_lores4)
                                                  : sorted_cpu_cycles(cap_base, word_addr(kShareScratch));
        LineRef base_lref = make_line_ref((mutated ? cap_lores4 : cap_base).log);
        size_t base_even = 0, base_odd = 0;
        for (auto c : base_cpu) {
            bool known; bool even = is_even_slot(base_lref, c, &known);
            if (!known) continue;
            (even ? base_even : base_odd)++;
        }

        std::vector<uint64_t> share_cpu;
        if (mutated) {
            share_cpu = lores_inwindow_cpu_cycles(cap_lores4);
        } else {
            auto [lo, hi] = dst_span(cap_share);
            for (const auto& a : cap_share.log)
                if (is_cpu_access(a, word_addr(kShareScratch)) && a.cycle >= lo && a.cycle <= hi)
                    share_cpu.push_back(a.cycle);
            std::sort(share_cpu.begin(), share_cpu.end());
        }
        LineRef share_lref = make_line_ref((mutated ? cap_lores4 : cap_share).log);
        size_t share_even = 0, share_odd = 0;
        for (auto c : share_cpu) {
            bool known; bool even = is_even_slot(share_lref, c, &known);
            if (!known) continue;
            (even ? share_even : share_odd)++;
        }

        checks.note("PARITY: no-DMA (" + std::string(mutated ? "four-lores-plane in-window" : "baseline") +
                    ") odd-slot accesses " + std::to_string(base_odd) + " of " +
                    std::to_string(base_even + base_odd) + "; share-mode window (" +
                    std::string(mutated ? "four-lores-plane in-window" : "blit's own active window") +
                    ") odd-slot accesses " + std::to_string(share_odd) + " of " +
                    std::to_string(share_even + share_odd));
        checks.expect((base_even + base_odd) > 0 && base_odd == 0 &&
                      (share_even + share_odd) > 0 && share_odd == 0,
            "PARITY [p196] 6-system-dma.c50,ch6-sysdma.c25,6-system-dma.c60: every CPU chip-bus "
            "access falls on an even-numbered slot by Figure 6-9's own per-line numbering, both "
            "with no DMA at all and inside a share-mode blit's own active window (" +
            std::to_string(base_odd) + " of " + std::to_string(base_even + base_odd) + " no-DMA, " +
            std::to_string(share_odd) + " of " + std::to_string(share_even + share_odd) + " in-window, on odd slots)");
    }

    // ---- ORDER [p169] a blitter copy finishes before an equal-sized CPU copy loop
    {
        bool mutated = (mv == "ORDER");
        std::string cl_bin = dir + (mutated ? "/prog_copyloop_tiny.bin" : "/prog_copyloop.bin");
        Capture cl = run_capture(cl_bin, kBootBudget + kDoneRunBudget, true, {});
        uint32_t n = mutated ? 1u : kClN;
        auto cl_writes = write_cycles_in_range(cl.log, word_addr(kClDst), word_addr(kClDst) + n);
        bool cl_ok = cl.loaded && cl.done.has_value() && cl_writes.size() == n;
        uint64_t cl_first = cl_writes.empty() ? 0 : *std::min_element(cl_writes.begin(), cl_writes.end());
        uint64_t cl_last  = cl_writes.empty() ? 0 : *std::max_element(cl_writes.begin(), cl_writes.end());
        uint64_t cl_span = cl_writes.empty() ? 0 : cl_last - cl_first;
        auto [blo, bhi] = dst_span(cap_nasty);
        uint64_t blit_span = blo == bhi ? 0 : bhi - blo;
        checks.note("ORDER: CPU copy loop of " + std::to_string(n) + " word(s) spans " +
                    std::to_string(cl_span) + " clk_sys (" + std::to_string(cl_writes.size()) +
                    " writes seen); the nasty-mode blit's own 500-word copy spans " +
                    std::to_string(blit_span) + " clk_sys, same yardstick (first to last destination write)");
        checks.expect(cl_ok && blit_span > 0 && blit_span < cl_span,
            "ORDER [p169]: a blitter A-to-D copy of 500 words (the nasty-mode capture, uncontended) "
            "finishes before a 68000 MOVE.W copy loop of the same 500 words (" +
            std::to_string(blit_span) + " clk_sys vs. " + std::to_string(cl_span) +
            " clk_sys) -- ordering only, no numeric factor asserted");
    }

    // ---- TAS [p196] 6-system-dma.c56: TAS's read-modify-write cycle is
    // indivisible, graded under active share-mode contention (a
    // continuously-demanding blit keeps requesting cycles throughout) by
    // whether any OTHER RAM-port access lands between TAS's own read and
    // its own write. A CPU-core bus-protocol property (fx68k and TG68K
    // model the 68000's own bus cycle, not the Amiga chipset's
    // arbitration): reported per core, not reconciled.
    {
        std::string bin = dir + (mv == "TAS" ? "/prog_tas_share_mutant.bin" : "/prog_tas_share.bin");
        Capture t = run_capture(bin, kBootBudget + kDoneRunBudget, true, {});
        std::vector<nmh::RamAccess> raw_at_addr;
        for (const auto& a : t.log) if (a.word_addr == word_addr(kTasAddr)) raw_at_addr.push_back(a);
        auto coalesced = nmh::coalesce(raw_at_addr);
        std::vector<std::pair<uint64_t, bool>> at_addr; // (cycle, is_write)
        for (const auto& a : coalesced) at_addr.push_back({a.cycle, a.is_write});
        std::sort(at_addr.begin(), at_addr.end());
        int64_t r_cycle = -1, w_cycle = -1;
        for (size_t i = 0; i + 1 < at_addr.size(); ++i) {
            if (!at_addr[i].second && at_addr[i + 1].second) { // read then write
                r_cycle = (int64_t)at_addr[i].first;
                w_cycle = (int64_t)at_addr[i + 1].first;
                break;
            }
        }
        size_t between = 0;
        if (r_cycle >= 0) {
            for (const auto& a : t.log)
                if ((int64_t)a.cycle > r_cycle && (int64_t)a.cycle < w_cycle) ++between;
        }
        checks.note("TAS: read-to-write gap = " + std::to_string(w_cycle - r_cycle) + " clk_sys, " +
                    std::to_string(between) + " other RAM-port access(es) between them (" +
                    (mv == "TAS" ? "ordinary read-then-write pair" : "TAS.B") + ", share-mode blit concurrent)");
        checks.expect(r_cycle >= 0 && between == 0,
            "TAS [p196] 6-system-dma.c56: TAS's read-modify-write cycle is indivisible -- no other "
            "RAM-port access lands between its own read and its own write while a share-mode blit "
            "actively contends for the bus (" + std::to_string(between) +
            " intervening access(es); a 68000 bus-cycle property, reported per core)");
    }

    // ---- ch6-multi.c4 [p187] -- TG68K only; cpu_config selects
    // "CPU-Type: 00 = 68000, 01 = 68010, 11 = 68020" (nanomig.v:198).
    // fx68k is a fixed 68000 implementation and does not read it.
    if (is_tg68k) {
        nmh::Config cfg020; cfg020.cpu_config = 3;
        nmh::Config cfg000; cfg000.cpu_config = 0;
        Capture c020 = run_capture(dir + "/prog_sharing.bin", kShareRunBudget, false, {}, cfg020);
        Capture c000 = run_capture(dir + "/prog_sharing.bin", kShareRunBudget, false, {}, cfg000);
        size_t n020 = scratch_writes(c020).size();
        size_t n000 = scratch_writes(c000).size();
        checks.note("ch6-multi.c4 [p187]: loop iterations over " + std::to_string(kShareRunBudget) +
                    " cycles, no DMA -- cpu_config=3 (68020): " + std::to_string(n020) +
                    ", cpu_config=0 (68000): " + std::to_string(n000) +
                    " -- contradicts nothing on the page (no DMA is competing for the bus here)");
        checks.record(
            "MULTI-C4-WITHDRAWN [p187] ch6-multi.c4: not tested -- the page's own consequence is a "
            "read of the blit's destination, served from a 68020's cache, before the blitter has "
            "finished writing it (\"a 68020, perhaps running out of its cache, may be able to get "
            "at the data before the blitter has finished writing it\"). That needs cpu_config=3 "
            "plus the cache path (cpu_wrapper.v:514 cchip = turbochip_d && (turbodata_d || "
            "cpustate == 2'b00); nanomig.v:208 gates it with ~ovl, so a program would first have "
            "to clear the overlay with a CIA-A write) while a blit actively writes the same "
            "address the CPU reads -- no such stimulus is written here, so this is not tested, "
            "not a harness gap");
    } else {
        checks.note("ch6-multi.c4 [p187]: not applicable on fx68k (fixed 68000 implementation; "
                    "cpu_config selects TG68K-only behaviour, nanomig.v:198)");
    }

    // ---- 6-blitter-done-flag.c8,ch6-done.c9 [p186] -- attempted, withdrawn.
    checks.record(
        "FASTRUN-WITHDRAWN [p186] 6-blitter-done-flag.c8,ch6-done.c9: not tested -- a program was "
        "built and run (not committed, since its own measurement was not trustworthy): it enabled "
        "the design's Zorro II fast-RAM path with one register write at $E80048 (cpu_wrapper.v's "
        "sel_z2ram, independent of Config.turbo_config/the CPU cache and of the overlay), copied a "
        "counting loop there, and ran it concurrently with a nasty blit. The write decodes and the "
        "run completes (it does not hang), but the elapsed time between the last chip-bus access "
        "before the jump and the RESULTS store after the loop came out identical, to the clk_sys, "
        "for a 50-iteration and a 2000-iteration loop on fx68k -- a result the loop's own execution "
        "cannot explain, so something about this measurement is not understood and is not reported "
        "as a finding either way. Not a harness gap in the sense the first report used the phrase "
        "(the Zorro II path itself is reachable through the public Config/harness surface, per the "
        "validator's own reading of cpu_wrapper.v); the step that fails is establishing what the "
        "fixed span actually is");

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
