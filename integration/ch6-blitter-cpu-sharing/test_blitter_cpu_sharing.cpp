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
// MUTATION DESC-BASE stimulus red the blitter's-own-convention capture substituted with the off-by-one capture, via NMH_MUTATE=DESC-BASE
// MUTATION DESC-OFFBY1 stimulus red the one-word-past capture substituted with a two-words-past capture (outside the exact range graded), via NMH_MUTATE=DESC-OFFBY1
// MUTATION A2 stimulus red blit shrunk to a near-instant one-word transfer, via NMH_MUTATE=A2
// MUTATION A3 stimulus red blit size grown from one word to two hundred, via NMH_MUTATE=A3
// MUTATION A3PRE stimulus red dummy chip-memory read moved before the blit is started, via NMH_MUTATE=A3PRE
// MUTATION BBUSY-PREDICTS stimulus red the true 200-word blit substituted with the one-word build (already finished, BBUSY clear), via NMH_MUTATE=BBUSY-PREDICTS
// MUTATION SLOT-UNIT stimulus red the refresh-pattern capture substituted with the four-lores-plane build, whose bitplane DMA adds reads to the refresh reads so that its groups are not four refresh slots per line, via NMH_MUTATE=SLOT-UNIT
// MUTATION A4PRE stimulus red nasty-mode blit shrunk to a near-instant one-word transfer (prog_sharing_nasty_tiny.s), via NMH_MUTATE=A4PRE
// MUTATION A4 stimulus red BLTPRI cleared (nasty -> share), via NMH_MUTATE=A4
// MUTATION A4-SHARE-PRE stimulus red share-mode blit shrunk to a near-instant one-word transfer (empty active window), via NMH_MUTATE=A4-SHARE-PRE
// MUTATION A4-SHARE stimulus red reference rate taken from the nasty-mode blit's own active window (the CPU makes two accesses there) instead of the no-DMA run, via NMH_MUTATE=A4-SHARE
// MUTATION A6 stimulus red six aligned planes substituted for four inside the fetch window, via NMH_MUTATE=A6
// MUTATION A8 stimulus green baseline capture substituted with the four-lores-plane in-window accesses (live, all even-slot, same unchanged comparator), via NMH_MUTATE=A8
// MUTATION A8 stimulus red baseline capture substituted with the share-mode blit's own active-window CPU accesses (live, odd-slot spacings on both cores, same unchanged comparator), via NMH_MUTATE=A8-SHARE
// MUTATION A9 stimulus red bitplane resolution changed from low to high, via NMH_MUTATE=A9
// MUTATION A9PRE stimulus red bitplane program substituted with one that never enables bitplane DMA, via NMH_MUTATE=A9PRE
// MUTATION A10 stimulus red bitplane resolution changed from high to low, via NMH_MUTATE=A10
// MUTATION A10PRE stimulus red bitplane program substituted with one that never enables bitplane DMA, via NMH_MUTATE=A10PRE
// MUTATION A11 stimulus red both sides run the same four-bitplane config, via NMH_MUTATE=A11
// MUTATION PARITY stimulus green no-DMA and share-window captures substituted with the four-lores-plane in-window accesses (live, all even-slot, same unchanged comparator): PARITY is red on both cores unmutated and reads green on both with this input, via NMH_MUTATE=PARITY
// MUTATION PARITY-REF stimulus red the lores-plane capture substituted with the no-bitplane-DMA baseline (no BPL1 reads at all), via NMH_MUTATE=PARITY-REF
// MUTATION SYSDMA-C29 stimulus red the misaligned (BRA.S) in-window capture substituted with the whole-4-clock-aligned loop's own (no waited step), via NMH_MUTATE=SYSDMA-C29
// MUTATION OPEN50 stimulus red six-plane side substituted with the four-plane capture (no reduction), via NMH_MUTATE=OPEN50
// MUTATION ORDER stimulus red the 500-word CPU copy loop substituted with a 50-word copy (shorter, nonzero span, finishes before the blit), via NMH_MUTATE=ORDER
// MUTATION FASTRUN stimulus red FR_CODE moved to chip RAM ($0C8000, prog_fastrun_chipcode.s/prog_fastrun_long_chipcode.s), FR_CNT still in fast RAM: A4 predicts zero counter writes in the blit's window, since a nasty blit denies the CPU every chip-bus cycle, fetches included; measured 0 in-window writes on fx68k (red through both the existence and the every-gap-equals-period clause) and 1 on TG68K (existence clause stays green; red only through the every-gap-equals-period clause, since a single in-window write forms no in-window gap), via NMH_MUTATE=FASTRUN
// MUTATION FASTRUN-LOCKOUT stimulus red FR_CODE moved to chip RAM ($0C0800, prog_fastrun_lockout_chipcode.s), FR_CNT still in fast RAM: the fetch window is entirely DMA (A10PRE: p.197's own "40 * 4 = 160 time slots", none left over), so no CPU access is possible anywhere strictly inside it; measured 0 counter writes strictly inside the window on both cores, via NMH_MUTATE=FASTRUN-LOCKOUT
// MUTATION FASTRUN-LOCKOUT-PRE stimulus red prog_fastrun_lockout_2plane.s (FR_2PLANE set): two hi-res bitplanes (BPLCON0 $A000, same DDFSTRT/DDFSTOP) leave 80 of the window's 160 slots free (Figure 6-9's own double dagger); the blitter's own first access lands inside the window instead of after it, so the precondition itself goes red, via NMH_MUTATE=FASTRUN-LOCKOUT-PRE
//
// A5 is withdrawn (see the A5-WITHDRAWN record below); it carries no
// assertion and no mutation. SLOTSPAN is withdrawn (see the SLOTSPAN-WITHDRAWN
// record below); it carries no assertion and no mutation. ch6-multi.c4 is
// withdrawn (see the MULTI-C4-WITHDRAWN record below); it carries no assertion
// and no mutation. 6-system-dma.c56 is withdrawn (see the TAS-WITHDRAWN record
// below); it carries no assertion and no mutation.

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <map>
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
constexpr uint32_t kClShortN = 50; // ORDER's own mutation (prog_copyloop_tiny.s, CL_SHORT set)

// prog_tas_share.s address.
constexpr uint32_t kTasAddr = 0x0C2000;

// prog_fastrun.s / prog_fastrun_lockout.s addresses (see those sources).
// Both keep the loop's own counter at the same fast-RAM word.
constexpr uint32_t kFrCnt = 0x200100;
constexpr uint64_t kFastrunStepBudget = kDoneRunBudget * 3;
// prog_fastrun_lockout.s only: BLIT_SRC's range (the blit's A-reads) and
// the Copper list's own MOVE-data-word for the real MOVE to BLTSIZE
// (COP_LIST+22, after the WAIT and its four timing-fill MOVEs -- the
// earliest RAM-port-observable bound on the BLTSIZE write itself, which
// is a custom-register write and not on the physical RAM port -- see
// that program's own header comment).
constexpr uint32_t kFrLockoutSrcLo = 0x0C2000;
constexpr uint32_t kFrLockoutSrcHi = 0x0C2000 + 2 * 500;
constexpr uint32_t kFrCopDataWord  = 0x0C1016;

// Every program in this package is well under 512 bytes; PROGRAM_ORG is
// fixed at $080400 (nanomig.i). A read in this range is a CPU instruction
// fetch, not a DMA access.
constexpr uint32_t kProgLo = 0x080400;
constexpr uint32_t kProgHi = 0x080600;

// The slot unit: one memory slot (one colour clock, p.194), in clk_sys cycles.
// Derived in main() from the refresh pattern of a run with no DMA enabled
// (Figure 6-9, p.194: four refresh slots, on every other memory cycle) and
// asserted there (SLOT-UNIT-PRE, SLOT-UNIT); never read from an RTL source.
// While it is not derived it is 1, so that no helper divides by zero.
uint64_t kCck = 1;

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

void not_graded(Checks& checks, const std::string& id, const std::string& unmet) {
    checks.record("NOT GRADED " + id + ": precondition(s) not met: " + unmet);
}

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

// ---- FASTRUN's own capture: the fast-RAM port, stepped by hand --------
//
// ram_log() only samples the physical chip-RAM port; fast-RAM (Zorro II)
// activity is only visible on nanomig.v's own fastram_sel/fastram_addr/
// fastram_wr ports (nanomig_harness.h's own comment on ports(): "the
// top-level ports only ... never use ->rootp"). Reaching them means
// stepping the harness one clk_sys cycle at a time through its already
// public run_cycles(1) and reading ports() after each step -- boot is
// still done in bulk (the fast-RAM port carries nothing during boot).
struct FastCapture {
    bool loaded = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;      // chip-bus port (ram_log())
    std::vector<nmh::RamAccess> fastlog;  // fast-RAM port, coalesced
    std::vector<std::optional<uint16_t>> peeks;
};

FastCapture run_capture_fastram(const std::string& bin, uint64_t step_budget,
                                 const std::vector<uint32_t>& peek_addrs, nmh::Config cfg) {
    FastCapture c;
    nmh::NanomigHarness h(cfg);
    c.loaded = h.load_program(bin) > 0;
    if (!c.loaded) return c;
    h.boot(kBootBudget);
    std::vector<nmh::RamAccess> raw_fast;
    for (uint64_t i = 0; i < step_budget; ++i) {
        h.run_cycles(1);
        if (h.ports()->fastram_sel) {
            // The sample taken this step reflects the cycle just finished
            // (h.cycles() was already incremented past it), matching
            // ram_log()'s own convention of recording the cycle a sample
            // was taken at.
            raw_fast.push_back({h.cycles() - 1, (uint32_t)h.ports()->fastram_addr, 0,
                                 bool(h.ports()->fastram_wr), false, false});
        }
    }
    c.log = h.ram_log();
    for (const auto& a : c.log)
        if (a.is_write && a.word_addr == word_addr(kDoneAddr) && a.data == kDoneValue) c.done = a.cycle;
    c.fastlog = nmh::coalesce(raw_fast);
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

// Every read below word address $1000 is grouped by gap (over 100 clk_sys
// starts a new group), then only the groups matching Figure 6-9's own
// refresh shape -- exactly four reads, each two slots (16 clk_sys) after
// the one before -- are kept as line anchors. In the bitplane builds,
// BPL2-4's own pointers are left unset and so also read low memory, in
// groups of a different size (60 or 48) and spacing; a group that is not
// four reads two slots apart is some other master's low read, not a
// line's own refresh, and must not anchor a line.
LineRef make_line_ref(const std::vector<nmh::RamAccess>& log) {
    LineRef ref;
    std::vector<uint64_t> low_reads;
    for (const auto& a : log) if (!a.is_write && a.word_addr < 0x1000) low_reads.push_back(a.cycle);
    if (low_reads.empty()) return ref;
    std::vector<std::vector<uint64_t>> groups;
    groups.push_back({low_reads[0]});
    for (size_t i = 1; i < low_reads.size(); ++i) {
        if (low_reads[i] - low_reads[i - 1] > 100) groups.push_back({});
        groups.back().push_back(low_reads[i]);
    }
    for (const auto& g : groups) {
        if (g.size() != 4) continue;
        bool refresh_shape = true;
        for (size_t i = 1; i < g.size(); ++i)
            if (g[i] - g[i - 1] != 2 * kCck) { refresh_shape = false; break; }
        if (!refresh_shape) continue;
        ref.groups.push_back({g.front(), g.back()});
    }
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

// Counts consecutive-pair gaps in `cycles` (already sorted) that are
// exactly `slots` colour-clock slots apart -- used to find one specific,
// recurring step (not every odd- or even-slot pair) among a mixed
// population of in-window CPU-access gaps.
size_t count_gap_slots(const std::vector<uint64_t>& cycles, uint64_t slots) {
    size_t n = 0;
    for (size_t i = 1; i < cycles.size(); ++i)
        if (cycles[i] - cycles[i - 1] == slots * kCck) ++n;
    return n;
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

// ---- the slot unit, from the refresh pattern -----------------------------
//
// A run with no DMA channel enabled (prog_sharing.s, BASELINE) carries only
// the 68000's own accesses (program fetches, the scratch write, the reset
// vector reads from the overlay window) and the refresh slots, which no
// DMACON bit switches off: every other read in it is refresh, by exclusion.
// p.194 lists "4 cycles for memory refresh" per line and Figure 6-9 draws
// them on every other memory cycle, so two refresh accesses of one line are
// two slots apart and the smallest positive gap between two refresh
// accesses is two slots.

struct RefreshSet {
    std::vector<uint64_t> reads;  // chronological cycles of every read taken as refresh
    size_t other = 0;             // accesses that are neither CPU, reset-vector nor refresh
    uint64_t end = 0;             // cycle of the last access of the capture
};

RefreshSet refresh_by_exclusion(const Capture& c, uint32_t scratch_word) {
    RefreshSet r;
    for (const auto& a : c.log) {
        r.end = a.cycle;
        if (is_cpu_access(a, scratch_word)) continue;
        if (!a.is_write && a.word_addr >= nmh::kKickShadowWordBase) continue; // reset-vector read
        if (a.is_write) ++r.other; else r.reads.push_back(a.cycle);
    }
    return r;
}

uint64_t smallest_gap(const std::vector<uint64_t>& v) {
    uint64_t small = 0;
    for (size_t i = 1; i < v.size(); ++i) {
        uint64_t d = v[i] - v[i - 1];
        if (d > 0 && (small == 0 || d < small)) small = d;
    }
    return small;
}

// Groups the refresh reads into lines: a gap over eight times the smallest
// gap starts a new group.
std::vector<std::vector<uint64_t>> group_refresh(const std::vector<uint64_t>& reads, uint64_t small) {
    std::vector<std::vector<uint64_t>> groups;
    for (size_t i = 0; i < reads.size(); ++i) {
        if (i == 0 || reads[i] - reads[i - 1] > 8 * small) groups.push_back({});
        groups.back().push_back(reads[i]);
    }
    return groups;
}

// SLOT-UNIT's comparator: every line's refresh group holds four accesses, at
// offsets 0, 2, 4 and 6 slots from the first (Figure 6-9). A trailing group
// the capture ended inside is not a line and is left out; `lines` counts the
// groups graded.
bool refresh_pattern_ok(const RefreshSet& r, uint64_t unit, uint64_t small, size_t& lines) {
    auto groups = group_refresh(r.reads, small);
    if (!groups.empty() && r.end - groups.back().back() < 2 * small && groups.back().size() < 4) groups.pop_back();
    lines = groups.size();
    for (const auto& g : groups) {
        if (g.size() != 4) return false;
        for (size_t k = 0; k < 4; ++k)
            if (g[k] - g[0] != 2 * k * unit) return false;
    }
    return lines > 0;
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
    // word, BLTCON1 bit 1 -- p.182: "Descending mode is turned on by
    // setting bit one of BLTCON1") differs from the 680x0 -(An)
    // predecrement convention (which would leave a pointer one word past
    // the last): starting one word too high displaces the whole
    // descending copy by one word, observable in ram_log() as which
    // destination words are written.
    {
        bool mutated_base = (mv == "DESC-BASE");
        bool mutated_offby1 = (mv == "DESC-OFFBY1");
        Capture db_a = run_capture(dir + (mutated_base ? "/prog_descblit_offby1.bin" : "/prog_descblit.bin"),
                                    kBootBudget + kDoneRunBudget, true,
                                    {0x0C2100, 0x0C2102, 0x0C2104, 0x0C2106, 0x0C2108});
        Capture db_b = run_capture(dir + (mutated_offby1 ? "/prog_descblit_offby2.bin" : "/prog_descblit_offby1.bin"),
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
        bool b_w0  = written(db_b, kDbDst + 0);
        bool b_w2  = written(db_b, kDbDst + 2);
        bool b_w4  = written(db_b, kDbDst + 4);
        bool b_w6  = written(db_b, kDbDst + 6);
        bool b_w8  = written(db_b, kDbDst + 8);
        bool b_w10 = written(db_b, kDbDst + 10);
        checks.note("DESC-OFFBY1: A (pointer at last word) writes first=" + std::string(a_first ? "yes" : "no") +
                    " extra=" + std::string(a_extra ? "yes" : "no") + "; B (" +
                    (mutated_offby1 ? "pointer two words past the last" : "pointer one word past the last") +
                    ") writes $0C2100=" + std::string(b_w0 ? "yes" : "no") +
                    " $0C2102=" + std::string(b_w2 ? "yes" : "no") +
                    " $0C2104=" + std::string(b_w4 ? "yes" : "no") +
                    " $0C2106=" + std::string(b_w6 ? "yes" : "no") +
                    " $0C2108=" + std::string(b_w8 ? "yes" : "no") +
                    " $0C210A=" + std::string(b_w10 ? "yes" : "no"));
        checks.expect(a_first && !a_extra && !b_w0 && b_w2 && b_w4 && b_w6 && b_w8 && !b_w10,
            "DESC-OFFBY1 [p182] ch6-desc.c12: pointing BLTAPTH/BLTDPTH one word past the last word "
            "(the 680x0 -(An) predecrement convention) displaces the same descending copy by "
            "exactly one word -- $0C2102-$0C2108 are written and neither $0C2100 nor $0C210A is, "
            "where the blitter's own convention (pointer at the last word) writes $0C2100-$0C2106 "
            "and leaves $0C2108 alone");
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
            "A3PRE [p187]: the blit's destination write is already in "
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
    Capture cap_nasty_tiny = run_capture(dir + "/prog_sharing_nasty_tiny.bin", kShareRunBudget, false, {});
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

    // ---- SLOT-UNIT [Figure 6-9, p194] (no claim id): the slot unit every
    // slot-dependent line below uses is derived from the refresh pattern of
    // the no-DMA capture, and that pattern is asserted: p.194 lists "4 cycles
    // for memory refresh" per horizontal line and Figure 6-9 draws them on
    // every other memory cycle. The unit is the smallest positive gap between
    // two refresh accesses divided by two (two slots apart); the assertion is
    // that every line's refresh group holds exactly four accesses at 0, 2, 4
    // and 6 units. A change of clock divider would move the unit and the
    // pattern together; a capture whose low reads are not four refresh slots
    // per line breaks the pattern.
    bool slot_ok = false;
    {
        RefreshSet rs_base = refresh_by_exclusion(cap_base, word_addr(kShareScratch));
        uint64_t small = smallest_gap(rs_base.reads);
        uint64_t unit = small / 2;
        size_t base_lines = 0;
        slot_ok = small > 0 && small % 2 == 0 && refresh_pattern_ok(rs_base, unit, small, base_lines);
        if (slot_ok) kCck = unit;

        const Capture& sel = (mv == "SLOT-UNIT") ? cap_lores4 : cap_base;
        RefreshSet rs_sel = refresh_by_exclusion(sel, word_addr(kShareScratch));
        size_t sel_lines = 0;
        bool pattern = small > 0 && small % 2 == 0 && refresh_pattern_ok(rs_sel, unit, small, sel_lines);

        std::vector<uint64_t> starts;
        for (const auto& g : group_refresh(rs_base.reads, small)) starts.push_back(g.front());
        std::map<uint64_t, size_t> period_hist;
        for (size_t i = 1; i < starts.size(); ++i) ++period_hist[starts[i] - starts[i - 1]];
        uint64_t line_period = 0; size_t line_period_n = 0;
        for (const auto& kv : period_hist) if (kv.second > line_period_n) { line_period_n = kv.second; line_period = kv.first; }
        checks.note("SLOT-UNIT: smallest gap between two refresh accesses = " + std::to_string(small) +
                    " clk_sys, slot unit = " + std::to_string(unit) + " clk_sys; modal line period = " +
                    std::to_string(line_period) + " clk_sys = " +
                    std::to_string(unit ? line_period / unit : 0) + " slots (" + std::to_string(line_period_n) +
                    " of " + std::to_string(starts.size() > 0 ? starts.size() - 1 : 0) + " line intervals); " +
                    std::to_string(sel_lines) + " refresh group(s) graded");
        checks.expect(pattern,
            "SLOT-UNIT [Figure 6-9, p194]: every line's refresh group holds exactly four accesses at 0, 2, 4 "
            "and 6 slot units from its first (unit " + std::to_string(unit) + " clk_sys = half the smallest "
            "refresh gap; " + std::to_string(sel_lines) + " group(s)) [not graded -- apparatus check, no claim uid]");
        if (!slot_ok) kCck = 1;
    }
    auto gated = [&](const std::string& id) {
        if (slot_ok) return true;
        not_graded(checks, id, "SLOT-UNIT");
        return false;
    };

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
    // existence only: p.169 ("the 680x0 CPU continues to run as the blit
    // is executing") and p.186 ("they can both work concurrently") give
    // no bound on how long the CPU may then wait between two chip-bus
    // writes -- p.198's own release rule (three denied cycles, one
    // released) is what governs that, graded separately (the A5 halt).
    {
        const Capture& cap = (mv == "A2") ? cap_tiny : cap_share;
        auto [lo, hi] = dst_span(cap);
        auto sc = scratch_writes(cap);
        std::vector<uint64_t> in_win;
        for (auto c : sc) if (c > lo && c < hi) in_win.push_back(c);
        checks.note("A2: active window " + std::to_string(lo) + ".." + std::to_string(hi) +
                    ", " + std::to_string(in_win.size()) +
                    " scratch write(s) strictly inside it (excluding the window's own endpoints)");
        checks.expect(lo != hi && !in_win.empty(),
            "A2 [p169,p186,p198] 6-intro.c20,ch6-intro.c21,ch6-intro.c22,6-blitter-done-flag.c2,"
            "6-blitter-done-flag.c3,ch6-done.c2,ch6-done.c3,ch6-sysdma.c53: the CPU makes at least one "
            "write to its own counter strictly inside a share-mode blit's active window (" +
            std::to_string(in_win.size()) + " such write(s)), so the blitter and the 68000 are both "
            "served from the chip bus cycles of that window -- existence only, no bound of its "
            "own on the gap between writes");
    }

    // ---- A4 [p198] with DMAF_BLITHOG (BLTPRI) set the blitter keeps the bus.
    // p.198: "If DMAF_BLITHOG is a 1, the blitter will keep the bus for every
    // available Chip memory cycle." No claim id is carried: the claim ids of
    // the blitter's priority over the 68000 say "normally", which A4-SHARE
    // below grades in share mode.
    {
        const Capture& cap = (mv == "A4") ? cap_share : (mv == "A4PRE" ? cap_nasty_tiny : cap_nasty);
        auto [lo, hi] = dst_span(cap);
        auto sc = scratch_writes(cap);
        size_t in_win = count_in_window(sc, lo, hi);
        bool nontrivial = lo != hi && (hi - lo) > 2000;
        checks.expect(nontrivial,
            "A4PRE: the nasty-mode blit's active window is long enough to have contained "
            "several CPU-loop iterations had the CPU been let through (" + std::to_string(hi - lo) + " cycles)");
        if (nontrivial) {
            checks.expect(in_win == 0,
                "A4 [p198]: with DMAF_BLITHOG (BLTPRI) set the blitter keeps the bus for every available "
                "Chip memory cycle -- the CPU writes its own counter not once during the blit's active "
                "window (" + std::to_string(in_win) + " scratch writes)");
        } else {
            not_graded(checks, "A4", "A4PRE");
        }
    }

    // ---- A4-SHARE [p194,p198] "the blitter normally has a higher priority
    // than the processor": with BLTPRI clear (share mode) a CPU loop that is
    // continuously ready (the BASELINE build runs it at its own free rate)
    // loses cycles to the blitter -- inside the blit's active window it makes
    // fewer chip-bus accesses per slot than it makes with the bus to itself. A
    // 68000 with priority over the blitter would be served first in every
    // contested slot and keep its free rate. Rates are compared by cross
    // multiplication (accesses * reference span < reference accesses * span),
    // so no threshold and no division enters.
    {
        struct Rate { uint64_t n = 0, span = 0; };
        auto cpu_rate = [](const Capture& c, uint64_t lo, uint64_t hi) {
            Rate r; r.span = hi > lo ? hi - lo : 0;
            for (const auto& a : c.log)
                if (is_cpu_access(a, word_addr(kShareScratch)) && a.cycle >= lo && a.cycle <= hi) ++r.n;
            return r;
        };
        auto cpu_span = [](const Capture& c) -> std::pair<uint64_t, uint64_t> {
            uint64_t first = 0, last = 0; bool any = false;
            for (const auto& a : c.log)
                if (is_cpu_access(a, word_addr(kShareScratch))) { if (!any) first = a.cycle; last = a.cycle; any = true; }
            return {first, last};
        };
        const Capture& cap = (mv == "A4-SHARE-PRE") ? cap_tiny : cap_share;
        auto [lo, hi] = dst_span(cap);
        Rate win = cpu_rate(cap, lo, hi);
        // Reference: the same loop with the bus to itself (the BASELINE
        // capture, first to last CPU access); the mutation takes the nasty-
        // mode blit's own active window instead.
        std::pair<uint64_t, uint64_t> rw = (mv == "A4-SHARE") ? dst_span(cap_nasty) : cpu_span(cap_base);
        Rate ref = cpu_rate((mv == "A4-SHARE") ? cap_nasty : cap_base, rw.first, rw.second);
        bool pre = lo != hi && win.span > 0 && ref.span > 0 && ref.n > 0;
        checks.note("A4-SHARE: CPU accesses " + std::to_string(win.n) + " in a share-mode window of " +
                    std::to_string(win.span) + " clk_sys; reference " + std::to_string(ref.n) + " in " +
                    std::to_string(ref.span) + " clk_sys (" + std::string(mv == "A4-SHARE" ? "nasty-mode window" : "no-DMA run") + ")");
        checks.expect(pre,
            "A4-SHARE-PRE: the share-mode blit has a nonempty active window and the reference run holds "
            "CPU accesses (window " + std::to_string(win.span) + " clk_sys, reference " + std::to_string(ref.span) +
            " clk_sys) [not graded -- apparatus check, no claim uid]");
        if (pre) {
            checks.expect(win.n * ref.span < ref.n * win.span,
                "A4-SHARE [p194,p198] 6-system-dma.c13,ch6-sysdma.c11,6-blitter-priority.c1,ch6-sysdma.c52: "
                "with BLTPRI clear the blitter still has priority over the 68000 -- the continuously ready "
                "CPU loop makes fewer chip-bus accesses per clk_sys inside the share-mode blit's active window (" +
                std::to_string(win.n) + " in " + std::to_string(win.span) + ") than with the bus to itself (" +
                std::to_string(ref.n) + " in " + std::to_string(ref.span) + ")");
        } else {
            not_graded(checks, "A4-SHARE", "A4-SHARE-PRE");
        }
    }

    // ---- A5 [p198] share mode: "If the 68000 is unsatisfied for three
    // consecutive memory cycles, the blitter will release the bus for one
    // cycle" -- WITHDRAWN. The sentence is explicit, but its antecedent is
    // the 68000's own bus request: a request is unsatisfied from the cycle it
    // is raised, and the RAM-port log shows only the cycles in which an
    // access happened, never the cycle in which one was requested and
    // denied. The count of consecutive unsatisfied cycles, and with it the
    // cycle at which the blitter's release is due, cannot be located at this
    // boundary; nanomig.v exposes no CPU bus-request signal. The measured
    // line below is computed from the run and graded by nothing.
    {
        auto [lo, hi] = dst_span(cap_share);
        std::vector<uint64_t> cpu;
        for (const auto& a : cap_share.log)
            if (is_cpu_access(a, word_addr(kShareScratch)) && a.cycle >= lo && a.cycle <= hi) cpu.push_back(a.cycle);
        std::map<uint64_t, size_t> hist;
        for (size_t i = 1; i < cpu.size(); ++i) ++hist[cpu[i] - cpu[i - 1]];
        uint64_t mode_gap = 0; size_t mode_n = 0;
        for (const auto& kv : hist) if (kv.second > mode_n) { mode_n = kv.second; mode_gap = kv.first; }
        checks.note("A5: inside the share-mode blit's active window consecutive CPU accesses are most often " +
                    std::to_string(mode_gap) + " clk_sys apart (" + std::to_string(mode_n) + " of " +
                    std::to_string(cpu.empty() ? 0 : cpu.size() - 1) + " intervals)");
        checks.record(
            "A5-WITHDRAWN [p198] 6-blitter-priority.c13,ch6-sysdma.c62: withdrawn -- the page's rule "
            "(three consecutive unsatisfied 68000 memory cycles, then the blitter releases the bus for one "
            "cycle) counts cycles in which the 68000 has a request outstanding, and the RAM port "
            "shows only the cycles in which an access happened; no cycle in which a request was "
            "raised and denied is visible, so where the count starts, and when the release is due, "
            "cannot be located at this boundary (nanomig.v has no CPU bus-request port)");
    }

    // ---- A8 [p194,p196] a denied cycle means the CPU simply waits for
    // its own next available (same-parity) slot -- graded per line
    // (Figure 6-9 restarts slot numbering every line, and a line is 227
    // slots, p.194, an odd count, so a fixed-cycle reference's parity
    // flips every line: a pair spanning a line start is excluded, not
    // counted as a violation).
    if (gated("A8")) {
        bool mutated = (mv == "A8");
        bool mutated_share = (mv == "A8-SHARE");
        const Capture& src_cap = mutated ? cap_lores4 : (mutated_share ? cap_share : cap_base);
        std::vector<uint64_t> cpu_src;
        std::string src_desc;
        if (mutated) { cpu_src = lores_inwindow_cpu_cycles(cap_lores4); src_desc = "four-lores-plane in-window accesses"; }
        else if (mutated_share) {
            auto [lo, hi] = dst_span(cap_share);
            for (const auto& a : cap_share.log)
                if (is_cpu_access(a, word_addr(kShareScratch)) && a.cycle >= lo && a.cycle <= hi)
                    cpu_src.push_back(a.cycle);
            std::sort(cpu_src.begin(), cpu_src.end());
            src_desc = "share-mode blit's own active window";
        } else { cpu_src = sorted_cpu_cycles(cap_base, word_addr(kShareScratch)); src_desc = "no-DMA baseline"; }
        LineRef lref = make_line_ref(src_cap.log);
        PairStats s = within_line_parity(lref, cpu_src);
        checks.note("A8: " + std::to_string(s.odd) + " odd-slot-spacing instance(s) of " +
                    std::to_string(s.pairs) + " within-line, slot-aligned consecutive-CPU-access "
                    "pairs (" + src_desc + ")");
        checks.expect(s.pairs > 0 && s.odd == 0,
            "A8 [p194,p196] 6-system-dma.c54,6-system-dma.c55,ch6-sysdma.c30: no two consecutive "
            "CPU bus accesses within one line are an odd number of slots apart -- a denied cycle "
            "always waits for the CPU's own next available (same-parity) slot, never a different "
            "one (" + std::to_string(s.odd) + " odd-spacing instance(s) of " + std::to_string(s.pairs) + ")");
    }

    // ---- SYSDMA-C29 [p196] ch6-sysdma.c29: some 68000 instructions miss
    // the even-cycle allocation, and p.196 gives the consequence of a
    // missed cycle as a wait, not a landing: "If cycles are missed, the
    // 68000 must wait until its next available memory slot before
    // continuing." That wait is only observable where the page's own
    // allocation actually holds the odd slots against the CPU -- inside
    // the four-lores-plane fetch window (Figure 6-11: planes on the odd
    // slots), not the free-running no-DMA baseline, where the same
    // fractional-slot instruction (BRA.S, a 10-clock internal operation,
    // not a whole multiple of 4) simply lands on the odd slot instead
    // (A8's/PARITY's own red, a different fact). Inside the window, the
    // fetch after BRA.S's internal operation instead waits a full extra
    // slot: a recurring 4-slot step, not the free-running 3-slot one.
    // The whole-4-clock-aligned loop (prog_bitplane_aligned.s, JMP (An)
    // in place of BRA.S) has no such step -- used here as SYSDMA-C29's
    // own mutation. TG68K's own loop has no fractional-slot instruction
    // either, so it is reported not exercised rather than graded.
    if (gated("SYSDMA-C29")) {
        bool mutated = (mv == "SYSDMA-C29");
        const Capture& cap = mutated ? cap_lores4_aligned : cap_lores4;
        std::vector<uint64_t> cpu_src = lores_inwindow_cpu_cycles(cap);
        size_t total_pairs = cpu_src.empty() ? 0 : cpu_src.size() - 1;
        size_t waited4 = count_gap_slots(cpu_src, 4);
        checks.note("SYSDMA-C29: " + std::to_string(waited4) + " in-window 4-slot (waited-to-"
                    "next-even) step instance(s) of " + std::to_string(total_pairs) +
                    " in-window consecutive-CPU-access pairs (" +
                    (mutated ? "whole-4-clock-aligned loop" : "BRA.S-based loop") + ")");
        if (!is_tg68k) {
            checks.expect(waited4 > 0,
                "SYSDMA-C29 [p196] ch6-sysdma.c29: some 68000 instructions miss the even-cycle "
                "allocation -- inside the four-lores-plane fetch window, the fetch after a "
                "fractional-slot internal operation (BRA.S) waits a full extra slot instead of "
                "landing on the odd one denied it (" + std::to_string(waited4) + " of " +
                std::to_string(total_pairs) + " in-window pairs, exactly 4 slots apart)");
        } else {
            checks.note("SYSDMA-C29 [p196] ch6-sysdma.c29: not exercised on TG68K -- its own "
                        "loop has no fractional-slot internal operation, so no such wait occurs");
        }
    }

    // ---- A9 [p196] four or fewer low-resolution bitplanes still let the CPU through
    if (gated("A9")) {
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
    if (gated("A10")) {
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
    if (gated("A11")) {
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
    if (gated("A6")) {
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

    // ---- SLOTSPAN [p196] 6-system-dma.c58 -- withdrawn. Figure 6-10 divides
    // an average 68000 cycle into an internal-operation portion and a
    // memory-access portion of equal length. The RAM port records one sample
    // per CPU bus access, at the moment of the access: an access's own
    // duration (the memory-access portion) and the internal-operation portion
    // are not separately observable, only their sum, the interval between two
    // accesses, and "average" ranges over an instruction mix the page does not
    // give. The modal interval of this package's own loop is printed as a
    // measurement and graded by nothing.
    {
        std::vector<uint64_t> cpu_cycles;
        for (const auto& a : cap_base.log)
            if (is_cpu_access(a, word_addr(kShareScratch))) cpu_cycles.push_back(a.cycle);
        std::vector<uint64_t> gaps;
        for (size_t i = 1; i < cpu_cycles.size(); ++i) gaps.push_back(cpu_cycles[i] - cpu_cycles[i - 1]);
        uint64_t m = mode_of(gaps);
        checks.note("SLOTSPAN: modal interval between consecutive CPU bus accesses of the BASELINE loop = " +
                    std::to_string(m) + " clk_sys (" + std::to_string(m / kCck) + " slot(s)), " +
                    std::to_string(gaps.size()) + " intervals");
        checks.record(
            "SLOTSPAN-WITHDRAWN [p196] 6-system-dma.c58: withdrawn -- the RAM port shows one sample per CPU "
            "bus access, not where the access began or ended, so the memory-access portion and the "
            "internal-operation portion of Figure 6-10's average 68000 cycle cannot be measured separately "
            "at this boundary, and the page names no instruction mix over which the average is taken");
    }


    // ---- OPEN50 [p196,p197] six bitplanes steal 50% of the open slots four would have left
    if (gated("OPEN50")) {
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

    // ---- PARITY-REF [Figure 6-9] (no claim id): the per-line slot
    // reference PARITY and A8 both use is not merely asserted by
    // construction -- it is anchored to the display schedule Figure 6-9
    // draws: refresh's own last read is on an odd slot, and BPL1 lands
    // 58 slots later at $3F, which is also odd. Every BPL1 fetch of the
    // four-lores-plane build must therefore classify odd by this same
    // reference; a reference that mislabelled the phase would not show
    // this.
    if (gated("PARITY-REF")) {
        const Capture& cap = (mv == "PARITY-REF") ? cap_base : cap_lores4;
        auto bpl = bp_reads_of(cap);
        LineRef lref = make_line_ref(cap.log);
        size_t known_n = 0, odd_n = 0;
        for (auto c : bpl) {
            bool known; bool even = is_even_slot(lref, c, &known);
            if (!known) continue;
            ++known_n;
            if (!even) ++odd_n;
        }
        checks.note("PARITY-REF: " + std::to_string(odd_n) + " of " + std::to_string(known_n) +
                    " BPL1 reads classify odd by the per-line reference (" +
                    std::string((mv == "PARITY-REF") ? "no-bitplane-DMA baseline" : "four-lores-plane build") + ")");
        checks.expect(known_n > 0 && odd_n == known_n,
            "PARITY-REF [Figure 6-9]: every BPL1 fetch of the four-lores-plane build classifies "
            "odd by the same per-line reference PARITY and A8 use (refresh's own last read is odd, "
            "and BPL1 lands 58 slots later at $3F -- still odd) (" + std::to_string(odd_n) + " of " +
            std::to_string(known_n) + ")");
    }

    // ---- PARITY [p196] 6-system-dma.c50,ch6-sysdma.c25,6-system-dma.c60:
    // the 68000 uses only the even-numbered memory-access cycles, by
    // Figure 6-9's own per-line slot numbering (see "How slots are
    // numbered at the RAM port" in SPECIFICATION.md) -- graded over the
    // no-DMA run and, separately, the share-mode blit's own active
    // window.
    if (gated("PARITY")) {
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
        uint32_t n = mutated ? kClShortN : kClN;
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

    // ---- 6-system-dma.c56 [p196] -- withdrawn. The NOTE ("the indivisible
    // read-modify-write cycle that is used only in this instruction will
    // not fit into a DMA memory access slot") states a fact about the
    // 68000's own bus protocol: it does not require the chipset to hold
    // other masters off between TAS's own read and its own write, and the
    // page names no chipset-observable consequence of that fact. Measured
    // under active share-mode contention (a continuously-demanding blit
    // keeps requesting cycles throughout) all the same, so the record
    // below states what the RAM port actually shows.
    {
        Capture t = run_capture(dir + "/prog_tas_share.bin", kBootBudget + kDoneRunBudget, true, {});
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
                    std::to_string(between) + " other RAM-port access(es) between them "
                    "(TAS.B, share-mode blit concurrent)");
        checks.record(
            "TAS-WITHDRAWN [p196] 6-system-dma.c56: 68000 bus-protocol fact; p.196 states no "
            "chipset-observable requirement -- the NOTE describes the 68000's own indivisible "
            "read-modify-write cycle not fitting a DMA memory access slot, not a bound on what the "
            "chipset may do between TAS's own read and its own write. Measured (share-mode blit "
            "concurrent): " + std::to_string(between) + " other RAM-port access(es) land between "
            "them, which the page does not forbid");
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

    // ---- FASTRUN [p186] ch6-done.c9: "may be running completely
    // uninhibited out of Fast memory or its internal cache and continue
    // to have memory cycles." prog_fastrun.s enables the design's Zorro
    // II fast-RAM path with one register write at $E80048
    // (cpu_wrapper.v:117 sel_z2ram, cpu_wrapper.v:640), relocates a
    // counting loop there, and starts a nasty blit through the Copper's
    // own list -- never through a CPU chip-bus write -- so the CPU's own
    // last chip-bus write before it jumps into fast RAM is unaffected by
    // the blit that begins slightly afterward (A4 shows 0 CPU chip-bus
    // accesses for the whole of a nasty blit's window otherwise). p.30's
    // own start sequence (COPJMP1 strobed in chip RAM while COPEN is
    // clear, then COPEN set as the one start event, from fast RAM) starts
    // the Copper running its list. p.25 has the list read again at every
    // following vertical blank; each later pass restarts it from COP1LC
    // and writes BLTSIZE again, starting a further nasty blit outside this
    // assertion's own graded windows, before the 2000-iteration loop's own
    // budget ends. The relocated loop returns
    // through an indirect JMP (a3), the return address loaded into a3
    // before the jump into fast RAM: a direct JMP to the instruction
    // immediately following it is what vasm deletes at assembly time (the
    // branch and the fall-through coincide), which an indirect jump
    // through a register does not. prog_fastrun_long.s (2000 iterations,
    // prog_fastrun.s: 50) runs a loop long enough to outlast the blit's
    // own ~8112 clk_sys (the same known span as A4/ORDER's own nasty
    // capture). Fast-RAM activity is only visible on nanomig.v's own
    // fastram_sel/fastram_addr/fastram_wr ports, reached here by
    // run_capture_fastram() stepping the harness one clk_sys cycle at a
    // time (see its own comment above).
    //
    // "Completely uninhibited" is graded as an equality, not an existence
    // count: every counter-write gap strictly inside the blit's own
    // window must equal the modal period after it -- not just the two
    // periods' own modes, which would also pass a loop slowed in only
    // some of its in-window iterations. A threshold on the count (however
    // small) would not be from the page; every gap matching the measured
    // after-window period is.
    {
        bool mutated = (mv == "FASTRUN");
        nmh::Config cfg_fr;
        cfg_fr.fastram_config = 0x01; // 2MB Zorro II fast RAM (cpu_wrapper.v:571-572) -- never toggled;
                                        // the mutation substitutes a different binary (below), not this config

        // Mutation: prog_fastrun_chipcode.s / prog_fastrun_long_chipcode.s
        // (FR_CHIPCODE set) -- only FR_CODE moves to chip RAM ($0C8000);
        // FR_CNT stays in fast RAM. A4 (BLTPRI set denies the CPU every
        // chip-bus cycle, fetches included, for the whole of the blit's
        // active window) predicts zero counter writes there, since the
        // CPU cannot even fetch its own next loop instruction while the
        // window holds -- the in-window clause (and, since a period
        // computed from fewer than two writes is 0, the period-equality
        // clause with it) must go red on its own, through that missing
        // fetch, with both builds still completing.
        FastCapture cap_short = run_capture_fastram(
            dir + (mutated ? "/prog_fastrun_chipcode.bin" : "/prog_fastrun.bin"),
            kFastrunStepBudget, {kResults}, cfg_fr);
        FastCapture cap_long = run_capture_fastram(
            dir + (mutated ? "/prog_fastrun_long_chipcode.bin" : "/prog_fastrun_long.bin"),
            kFastrunStepBudget, {kResults}, cfg_fr);

        // The blit's own active window, on the same yardstick (first to
        // last destination write) A2/A4/ORDER use -- prog_fastrun.s uses
        // the same BLIT_DST ($0C4000, 500 words) as prog_sharing.s.
        auto blit_window = [&](const FastCapture& c) -> std::pair<uint64_t, uint64_t> {
            auto d = write_cycles_in_range(c.log, word_addr(kShareDstLo), word_addr(kShareDstHi));
            if (d.empty()) return {0, 0};
            return {*std::min_element(d.begin(), d.end()), *std::max_element(d.begin(), d.end())};
        };
        auto fr_writes_strictly_inside = [&](const FastCapture& c, uint64_t lo, uint64_t hi) {
            size_t n = 0;
            for (const auto& a : c.fastlog)
                if (a.is_write && a.word_addr == word_addr(kFrCnt) && a.cycle > lo && a.cycle < hi) ++n;
            return n;
        };
        auto fr_write_cycles = [&](const FastCapture& c) {
            std::vector<uint64_t> v;
            for (const auto& a : c.fastlog)
                if (a.is_write && a.word_addr == word_addr(kFrCnt)) v.push_back(a.cycle);
            std::sort(v.begin(), v.end());
            return v;
        };

        auto [lo_s, hi_s] = blit_window(cap_short);
        auto [lo_l, hi_l] = blit_window(cap_long);
        size_t inwin_short = (lo_s != hi_s) ? fr_writes_strictly_inside(cap_short, lo_s, hi_s) : 0;
        size_t inwin_long  = (lo_l != hi_l) ? fr_writes_strictly_inside(cap_long, lo_l, hi_l) : 0;
        bool results_ok_short = cap_short.peeks.size() && cap_short.peeks[0] && *cap_short.peeks[0] == 50;
        bool results_ok_long  = cap_long.peeks.size() && cap_long.peeks[0] && *cap_long.peeks[0] == 2000;

        // The period comparison itself: from the long (2000-iteration)
        // capture, which outlasts the blit on both sides, split every
        // consecutive-write gap into "strictly inside the window" and
        // "at or after the window's own end" -- the same split A6 makes.
        // "Completely" is graded by requiring every in-window gap, not
        // just their mode, to equal the after-window period.
        auto fr_all = fr_write_cycles(cap_long);
        std::vector<uint64_t> in_gaps, after_gaps;
        for (size_t i = 1; i < fr_all.size(); ++i) {
            uint64_t gap = fr_all[i] - fr_all[i - 1];
            if (lo_l != hi_l && fr_all[i - 1] > lo_l && fr_all[i] < hi_l) in_gaps.push_back(gap);
            else if (lo_l != hi_l && fr_all[i - 1] >= hi_l) after_gaps.push_back(gap);
        }
        uint64_t in_period = mode_of(in_gaps);
        uint64_t after_period = mode_of(after_gaps);
        bool every_in_gap_matches_period = !in_gaps.empty() && !after_gaps.empty() &&
            std::all_of(in_gaps.begin(), in_gaps.end(),
                        [after_period](uint64_t g) { return g == after_period; });

        checks.note("FASTRUN: 50-iteration -- done=" + std::string(cap_short.done.has_value() ? "y" : "n") +
                    " RESULTS=" + (cap_short.peeks.size() && cap_short.peeks[0] ? std::to_string(*cap_short.peeks[0]) : std::string("?")) +
                    " blit window=" + std::to_string(lo_s) + ".." + std::to_string(hi_s) +
                    " FR_CNT fast-RAM writes strictly inside=" + std::to_string(inwin_short) +
                    "; 2000-iteration -- done=" + std::string(cap_long.done.has_value() ? "y" : "n") +
                    " RESULTS=" + (cap_long.peeks.size() && cap_long.peeks[0] ? std::to_string(*cap_long.peeks[0]) : std::string("?")) +
                    " blit window=" + std::to_string(lo_l) + ".." + std::to_string(hi_l) +
                    " FR_CNT fast-RAM writes strictly inside=" + std::to_string(inwin_long) +
                    "; in-window period(mode)=" + std::to_string(in_period) + " clk_sys (" + std::to_string(in_gaps.size()) +
                    " gaps), after-window period=" + std::to_string(after_period) + " clk_sys (" +
                    std::to_string(after_gaps.size()) + " gaps), every in-window gap equals after-window period=" +
                    (every_in_gap_matches_period ? std::string("y") : std::string("n")));

        checks.expect(
            cap_short.loaded && cap_short.done.has_value() && results_ok_short && inwin_short > 0 &&
            cap_long.loaded && cap_long.done.has_value() && results_ok_long &&
            every_in_gap_matches_period,
            "FASTRUN [p186] ch6-done.c9: \"may be running completely uninhibited out of Fast "
            "memory or its internal cache and continue to have memory cycles\" -- a CPU loop "
            "relocated into Zorro II fast RAM writes its own counter strictly inside a nasty "
            "blit's own active window (" + std::to_string(inwin_short) + " such write(s)) and runs "
            "to completion under both a 50- and a 2000-iteration budget (RESULTS 50 and 2000 "
            "respectively, both observed at DONE), and every counter-write gap strictly inside "
            "the window equals the period measured after it (" + std::to_string(after_period) +
            " clk_sys, " + std::to_string(in_gaps.size()) + " in-window gap(s) checked) -- "
            "\"completely\" graded as every gap, not a mode-to-mode comparison, not a count or "
            "bound the page lacks");
    }

    // ---- FASTRUN-LOCKOUT [p186,p197] 6-blitter-done-flag.c8: "If a blit
    // has just been started but has been locked out of memory access
    // because of, for instance, display fetches ... The processor, on
    // the other hand, may be running completely uninhibited out of Fast
    // memory or its internal cache, so it will continue to have memory
    // cycles." Unlike FASTRUN above (a nasty blit with no display, where
    // the blitter itself holds the bus and the CPU is the locked-out
    // party), prog_fastrun_lockout.s reaches the page's own case: a
    // four-high-resolution-bitplane display (A10's own build) whose fetch
    // windows are entirely DMA (A10PRE: 160 of 160 slots, p.197's own
    // "40 * 4 = 160 time slots") -- p.197 itself: "This effectively locks
    // out the 68000 (as well as the blitter or Copper)" -- and a blit
    // started by a Copper MOVE to BLTSIZE, timed by a preceding WAIT to
    // just before a fetch window, so the blitter cannot get its own first
    // DMA access until the window ends. The counting loop runs in fast
    // RAM throughout, already running (COPEN, the one start event, is set
    // from fast RAM, as in FASTRUN above) before the Copper -- and
    // through it, the blit -- is allowed to move at all. prog_fastrun_
    // lockout_2plane.s (FR_2PLANE set) instead frees half the window's
    // slots by reducing the display to two hi-res bitplanes: the
    // blitter's own first access then falls inside the window instead of
    // after it, which is this package's own precondition mutation
    // (NMH_MUTATE=FASTRUN-LOCKOUT-PRE, below).
    //
    // The Copper's own write to BLTSIZE is a custom-register write, not
    // on the physical RAM port (like COPJMP1, see prog_fastrun.s's own
    // PRE_JUMP comment); the Copper's own fetch of the MOVE's data word
    // (COP_LIST+22 = $0C1016, a plain chip-RAM read) is, and bounds the
    // BLTSIZE write from below by at most the Copper's own single-slot
    // MOVE-execution latency. The blitter's own first DMA access (its
    // first A-read, BLIT_SRC's range, or its first D-write if that
    // happens to be logged first) is a physical RAM-port access
    // throughout.
    //
    // Precondition (p.197's own lockout, not this package's invention):
    // that interval -- from the Copper's own data-word fetch to the
    // blitter's own first DMA access -- must fully contain one of the
    // display's own fetch windows (A10's own per-line construction),
    // which also means no blitter access before that window's own end.
    // p.186's own condition ("locked out ... because of ... display
    // fetches") holds only inside that fetch window, not in the slots
    // before or after it: the claim below is graded strictly inside the
    // window itself, not over the whole Copper-to-blitter interval.
    {
        bool mutated_chipcode = (mv == "FASTRUN-LOCKOUT");
        bool mutated_2plane = (mv == "FASTRUN-LOCKOUT-PRE");
        nmh::Config cfg_lockout;
        cfg_lockout.fastram_config = 0x01;
        std::string bin_lockout = dir + "/prog_fastrun_lockout.bin";
        if (mutated_chipcode) bin_lockout = dir + "/prog_fastrun_lockout_chipcode.bin";
        else if (mutated_2plane) bin_lockout = dir + "/prog_fastrun_lockout_2plane.bin";
        FastCapture cap = run_capture_fastram(bin_lockout, kFastrunStepBudget, {kResults}, cfg_lockout);

        auto cop_fetch = read_cycles_in_range(cap.log, word_addr(kFrCopDataWord), word_addr(kFrCopDataWord) + 1);
        auto blit_reads = read_cycles_in_range(cap.log, word_addr(kFrLockoutSrcLo), word_addr(kFrLockoutSrcHi));
        auto blit_writes = write_cycles_in_range(cap.log, word_addr(kShareDstLo), word_addr(kShareDstHi));
        std::vector<uint64_t> blit_all = blit_reads;
        blit_all.insert(blit_all.end(), blit_writes.begin(), blit_writes.end());

        uint64_t cop_bound = cop_fetch.empty() ? 0 : cop_fetch.front();
        uint64_t blit_first = blit_all.empty() ? 0 : *std::min_element(blit_all.begin(), blit_all.end());

        auto wins = fetch_windows(read_cycles_in_range(cap.log, word_addr(kBpPtrLo), word_addr(kBpPtrHi)));
        const FetchWindow* w = nullptr;
        for (auto& fw : wins) if (fw.lo >= cop_bound) { w = &fw; break; }
        bool precondition = !cop_fetch.empty() && !blit_all.empty() && w != nullptr && w->hi <= blit_first;

        // p.186's condition holds only inside the fetch window itself
        // (w->lo, w->hi), not over the whole Copper-to-blitter interval:
        // counted strictly inside the window, not (cop_bound, blit_first).
        size_t inwin = 0;
        if (w != nullptr)
            for (const auto& a : cap.fastlog)
                if (a.is_write && a.word_addr == word_addr(kFrCnt) && a.cycle > w->lo && a.cycle < w->hi) ++inwin;

        bool results_ok = cap.peeks.size() && cap.peeks[0] && *cap.peeks[0] == 8000;

        checks.note("FASTRUN-LOCKOUT: done=" + std::string(cap.done.has_value() ? "y" : "n") +
                    " RESULTS=" + (cap.peeks.size() && cap.peeks[0] ? std::to_string(*cap.peeks[0]) : std::string("?")) +
                    "; Copper's own MOVE-data-word fetch at cycle=" + std::to_string(cop_bound) +
                    "; fetch window=" + (w ? std::to_string(w->lo) + ".." + std::to_string(w->hi) : std::string("none found at/after the fetch")) +
                    "; blitter's own first DMA access at cycle=" + std::to_string(blit_first) +
                    "; FR_CNT fast-RAM writes strictly inside the fetch window=" + std::to_string(inwin));

        checks.expect(precondition,
            "FASTRUN-LOCKOUT-PRE [p197]: the interval from the Copper's own fetch of the MOVE's "
            "data word to the blitter's own first DMA access fully contains one four-high-"
            "resolution-bitplane fetch window (p.197's own 160-of-160-slot lockout), with no "
            "blitter access before that window's own end");

        checks.expect(cap.loaded && cap.done.has_value() && results_ok && precondition && inwin > 0,
            "FASTRUN-LOCKOUT [p186] 6-blitter-done-flag.c8: \"If a blit has just been started but "
            "has been locked out of memory access because of, for instance, display fetches ... "
            "The processor, on the other hand, may be running completely uninhibited out of Fast "
            "memory or its internal cache, so it will continue to have memory cycles\" -- a CPU "
            "loop relocated into Zorro II fast RAM writes its own counter (" + std::to_string(inwin) +
            " such write(s)) strictly inside the display's own fetch window, an interval the "
            "precondition (FASTRUN-LOCKOUT-PRE) confirms falls entirely between the Copper's own "
            "MOVE to BLTSIZE and the blitter's own first DMA access");
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
