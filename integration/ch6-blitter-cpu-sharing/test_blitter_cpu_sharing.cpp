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
// MUTATION A1-A stimulus red the single MOVE.L to BLTAPTH points at the A decoy word instead of the A source word, via NMH_MUTATE=A1-A
// MUTATION A1-B stimulus red the single MOVE.L to BLTBPTH points at the B decoy word instead of the B source word, via NMH_MUTATE=A1-B
// MUTATION A1-C stimulus red the single MOVE.L to BLTCPTH points at the C decoy word instead of the C source word, via NMH_MUTATE=A1-C
// MUTATION A1-D stimulus red the single MOVE.L to BLTDPTH points at the D decoy word instead of the intended destination word, via NMH_MUTATE=A1-D
// MUTATION A1 stimulus red the single MOVE.L of all four channels points at the channel's decoy word, via NMH_MUTATE=A1
// MUTATION DESC-BASE stimulus red the blitter's-own-convention capture substituted with the off-by-one capture, via NMH_MUTATE=DESC-BASE; DESC-OFFBY1 reads red as well, since its predicate also requires that same capture (first word written, word past the block not written)
// MUTATION DESC-OFFBY1 stimulus red the one-word-past capture substituted with a two-words-past capture (outside the exact range graded), via NMH_MUTATE=DESC-OFFBY1
// MUTATION A2 stimulus red blit shrunk to a near-instant one-word transfer, via NMH_MUTATE=A2
// MUTATION A3 stimulus red blit size grown from one word to two hundred, via NMH_MUTATE=A3
// MUTATION A3PRE stimulus red dummy chip-memory read moved before the blit is started, via NMH_MUTATE=A3PRE
// MUTATION BBUSY-PREDICTS stimulus red the true 200-word blit substituted with the one-word build (already finished, BBUSY clear), via NMH_MUTATE=BBUSY-PREDICTS
// MUTATION SLOT-UNIT stimulus red the refresh-pattern capture substituted with the four-lores-plane build, whose bitplane DMA adds reads to the refresh reads so that its groups are not four refresh slots per line, via NMH_MUTATE=SLOT-UNIT
// MUTATION A4PRE stimulus red nasty-mode blit shrunk to a near-instant one-word transfer (prog_sharing_nasty_tiny.s), via NMH_MUTATE=A4PRE
// MUTATION A4 none impossible NMH_MUTATE=-: already red at baseline, no stimulus is run through it: the line prints FAIL in every run because a CPU program fetch falls inside the nasty-mode blit's window; its comparator (is_cpu_access(), count zero over a window) is live on A4-CTRL, which runs the same helper over the four-high-resolution-bitplane display's fetch windows and reads green there
// MUTATION A4-CTRL-PRE stimulus red the four-high-resolution-bitplane capture substituted with the no-display run (no fetch windows), via NMH_MUTATE=A4-CTRL-PRE
// MUTATION A4-CTRL stimulus red the four-high-resolution-bitplane capture substituted with the four-low-resolution-bitplane capture, in whose fetch windows the CPU is granted accesses; A4-CTRL-PRE still reads the high-resolution capture and stays green, via NMH_MUTATE=A4-CTRL
// MUTATION A4-SHARE-PRE stimulus red share-mode blit shrunk to a near-instant one-word transfer (empty active window), via NMH_MUTATE=A4-SHARE-PRE
// MUTATION A4-SHARE stimulus red the no-DMA reference loop substituted with a slower one (prog_sharing_slow.s: two register-only MULU.W between its counter writes, run from a copy in Zorro II fast RAM, so that its instruction fetches do not use the Chip-memory bus and only its counter writes are the 68000's Chip-memory accesses, which then come less often than the share-mode window's, on the classifier every other run uses); the share-mode capture, A4-SHARE-PRE and the comparator are unchanged, via NMH_MUTATE=A4-SHARE
// MUTATION A8PRE stimulus red the no-DMA run (no blitter, so no DMA access takes a slot the 68000 was ready for) substituted for the share-mode run, via NMH_MUTATE=A8PRE
// MUTATION A8 stimulus green the share-mode run substituted with the six-lores-plane display against the whole-4-clock-aligned counting loop, every bitplane pointer set to Chip memory (prog_bitplane_planes6_aligned_ptrs.s), where the display takes even-numbered slots from a 68000 that is ready only on even-numbered slots; the unchanged comparator is red on both cores unmutated and reads green on both with this input; every odd-numbered slot of the fetch window is the display's, so this input cannot show a grant before the first free even-numbered slot on any implementation and does not exercise the share-mode release behind the red, via NMH_MUTATE=A8
// MUTATION A9PRE stimulus red the four-plane run substituted with the no-DMA aligned run (no display windows), via NMH_MUTATE=A9PRE
// MUTATION A9 stimulus red the four-plane aligned run substituted with the six-plane aligned run, via NMH_MUTATE=A9
// MUTATION A10PRE stimulus red bitplane program substituted with one that never enables bitplane DMA, via NMH_MUTATE=A10PRE
// MUTATION A10 stimulus red bitplane resolution changed from high to low, via NMH_MUTATE=A10
// MUTATION A11PRE stimulus red the five-plane run substituted with the no-DMA run (no display windows), via NMH_MUTATE=A11PRE
// MUTATION A11 stimulus red the five- and six-plane runs substituted with the four-plane run, via NMH_MUTATE=A11
// MUTATION A6 stimulus red six aligned planes substituted for four inside the fetch window, via NMH_MUTATE=A6
// MUTATION OPEN50PRE stimulus red the six-plane run substituted with the no-DMA run (no display windows), via NMH_MUTATE=OPEN50PRE
// MUTATION OPEN50 stimulus red six-plane side substituted with the four-plane capture (no reduction), via NMH_MUTATE=OPEN50
// MUTATION PARITY-REF stimulus red the lores-plane capture substituted with the no-bitplane-DMA baseline (no BPL1 reads at all), via NMH_MUTATE=PARITY-REF
// MUTATION PARITY stimulus green no-DMA and share-window captures substituted with the four-lores-plane in-window accesses (live, all even-slot, same unchanged comparator): PARITY is red on both cores unmutated and reads green on both with this input, via NMH_MUTATE=PARITY
// MUTATION ORDER stimulus red the CPU copy loop of 500 words substituted with one of 100 words (prog_copyloop_short.s), so that it does not write the 500 destination words the unchanged predicate requires, via NMH_MUTATE=ORDER
// MUTATION TAS-SPAN-PRE stimulus red the TAS.B replaced by NOP, so the TAS address shows three cycles instead of five (prog_tas_span_mut.s), via NMH_MUTATE=TAS-SPAN-PRE
// MUTATION TAS-SPAN none impossible unfalsifiable at this boundary: the distance from the TAS read to its write is fixed by the 68000's own read-modify-write cycle and the chip set can only lengthen it, never shorten it, so no stimulus brings it down to one slot or less; replacing the TAS by another instruction changes the shape the precondition checks (TAS-SPAN-PRE) instead of the span this line grades
// MUTATION FASTRUN stimulus red FR_CODE moved to chip RAM ($0C8000, prog_fastrun_chipcode.s/prog_fastrun_long_chipcode.s), FR_CNT still in fast RAM: A4 predicts zero counter writes in the blit's window, since a nasty blit denies the CPU every chip-bus cycle, fetches included; measured 0 in-window writes on fx68k (red through both the existence and the every-gap-equals-period clause) and 1 on TG68K (existence clause stays green; red only through the every-gap-equals-period clause, since a single in-window write forms no in-window gap), via NMH_MUTATE=FASTRUN
// MUTATION FASTRUN-LOCKOUT stimulus red FR_CODE moved to chip RAM ($0C0800, prog_fastrun_lockout_chipcode.s), FR_CNT still in fast RAM: the fetch window is entirely DMA (A10PRE: p.197's own "40 * 4 = 160 time slots", none left over), so no CPU access is possible anywhere strictly inside it; measured 0 counter writes strictly inside the window on both cores, via NMH_MUTATE=FASTRUN-LOCKOUT
// MUTATION FASTRUN-LOCKOUT-PRE stimulus red prog_fastrun_lockout_2plane.s (FR_2PLANE set): two hi-res bitplanes (BPLCON0 $A000, same DDFSTRT/DDFSTOP) leave 80 of the window's 160 slots free (Figure 6-9's own double dagger); the blitter's own first access lands inside the window instead of after it, so the precondition itself goes red, via NMH_MUTATE=FASTRUN-LOCKOUT-PRE
// MUTATION BBUSY-LOCKOUT-PRE stimulus red prog_bbusy_lockout_2plane.s (FR_2PLANE set): two hi-res bitplanes leave 80 of the window's 160 slots free; the blitter's first access lands inside the window, so the precondition goes red, via NMH_MUTATE=BBUSY-LOCKOUT-PRE
// MUTATION BBUSY-LOCKOUT stimulus red prog_bbusy_lockout_intena.s (BB_INTENA set): both tests read INTENAR, whose bit 14 is clear, instead of DMACONR, via NMH_MUTATE=BBUSY-LOCKOUT
//
// A5 is withdrawn (see the A5-WITHDRAWN record below); it carries no
// assertion and no mutation. The claim ids of A4-SHARE are recorded as a
// boundary (A4-SHARE-WITHDRAWN below); A4-SHARE stays as a line without a
// claim id. The claim id of SYSDMA-C29 is recorded as a boundary
// (SYSDMA-C29-BOUNDARY below); its counts are printed as measurements. SLOTSPAN is withdrawn (see the
// SLOTSPAN-WITHDRAWN record below); it carries no assertion and no mutation.
// ch6-multi.c4 is withdrawn (see the MULTI-C4-WITHDRAWN record below); it
// carries no assertion and no mutation. 6-system-dma.c56 is withdrawn (see the
// TAS-WITHDRAWN record below); it carries no assertion and no mutation.

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
// (Figure 6-9, pp.194-195: four refresh slots, on every other memory cycle) and
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

std::string hexs(uint32_t v) {
    char b[16]; std::snprintf(b, sizeof b, "%X", v);
    return std::string(b);
}

// The CPU Chip-memory accesses (is_cpu_access()) inside [lo,hi], inclusive:
// their count, how many are counter writes, and a list of the first
// `list_max` with their offsets from `lo`.
struct CpuHits { size_t n = 0, writes = 0; std::string list; };

CpuHits cpu_hits_in(const std::vector<nmh::RamAccess>& log, uint64_t lo, uint64_t hi,
                    uint32_t scratch_word, size_t list_max) {
    CpuHits h;
    for (const auto& a : log) {
        if (!is_cpu_access(a, scratch_word) || a.cycle < lo || a.cycle > hi) continue;
        ++h.n;
        if (a.is_write) ++h.writes;
        if (h.n <= list_max)
            h.list += std::string(h.list.empty() ? "" : ", ") + (a.is_write ? "write $" : "fetch $") +
                      hexs(a.word_addr * 2) + " at +" + std::to_string(a.cycle - lo) + " clk_sys";
    }
    return h;
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
            raw_fast.push_back({h.cycles() - 1, (uint32_t)h.ports()->fastram_addr,
                                 h.ports()->fastram_wr ? (uint16_t)h.ports()->fastram_din : (uint16_t)0,
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

std::vector<Burst> group_bursts(const std::vector<uint64_t>& cycles, uint64_t gap = 0) {
    if (gap == 0) gap = 25 * kCck;   // a gap over 25 slots starts a new burst
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

// Only full lines are windows: the burst of a line holds as many BPL1 reads
// as the run's most frequent burst (20 words of a low-resolution line, 40 of
// a high-resolution one, p.197); a burst the capture started or ended inside
// holds fewer and is left out.
std::vector<FetchWindow> fetch_windows(const std::vector<uint64_t>& bpl_reads) {
    std::vector<FetchWindow> out;
    auto bursts = group_bursts(bpl_reads);
    std::map<size_t, size_t> count_hist;
    for (auto& b : bursts) if (b.count >= 2) ++count_hist[b.count];
    size_t full = 0, full_n = 0;
    for (const auto& kv : count_hist) if (kv.second > full_n) { full_n = kv.second; full = kv.first; }
    for (auto& b : bursts) {
        if (b.count < 2 || b.count != full) continue;
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

// ---- p.194/Figure 6-9's per-line slot numbering -------------------------
//
// Refresh reads are RAM-port reads below word address $1000 -- far below
// every program-specific range this package uses (kProgLo/kProgHi, every
// scratch word, every blit source/destination, BPL1PTR): 4 reads per
// line, 2 slots apart, grouped here by a gap over 12.5 slots starting a
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

// The most frequent value of the gaps between consecutive entries of an
// increasing sequence (0 for fewer than two entries).
uint64_t modal_gap(const std::vector<uint64_t>& v) {
    std::map<uint64_t, size_t> hist;
    for (size_t i = 1; i < v.size(); ++i) ++hist[v[i] - v[i - 1]];
    uint64_t best = 0; size_t n = 0;
    for (const auto& kv : hist) if (kv.second > n) { n = kv.second; best = kv.first; }
    return best;
}

struct LineRef {
    std::vector<uint64_t> firsts;
    std::vector<LineGroup> groups;
    uint64_t line_len = 0;   // the modal gap between consecutive line anchors, in cycles
};

// Every read below word address $1000 is grouped by gap (over 12.5 slots
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
        if (low_reads[i] - low_reads[i - 1] > (25 * kCck) / 2) groups.push_back({});   // over 12.5 slots: a new group
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
    ref.line_len = modal_gap(ref.firsts);
    return ref;
}

// The line-group index a cycle belongs to (the most recent group whose own
// first refresh read precedes it), or -1 if it precedes every line found.
int64_t line_of(const LineRef& ref, uint64_t cycle) {
    size_t i = std::upper_bound(ref.firsts.begin(), ref.firsts.end(), cycle) - ref.firsts.begin();
    return i == 0 ? -1 : (int64_t)i - 1;
}

// True if `cycle` (a CPU chip-bus access) is on an even-numbered slot by
// Figure 6-9's own per-line numbering; false (with *known = false) if it
// precedes every line this capture found or lies a full line or more past
// the first refresh read of the line it would be numbered from (a stretch
// with no line anchor: its numbering would run on from an earlier line).
bool is_even_slot(const LineRef& ref, uint64_t cycle, bool* known = nullptr) {
    int64_t i = line_of(ref, cycle);
    bool anchored = i >= 0 && ref.line_len > 0 && cycle - ref.groups[(size_t)i].first < ref.line_len;
    if (known) *known = anchored;
    if (!anchored) return false;
    int64_t d = floordiv((int64_t)cycle - (int64_t)ref.groups[(size_t)i].last, (int64_t)kCck);
    return (d % 2) != 0; // odd slot-offset from the line's own last refresh read => even slot
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

// ---- slots, and the 68000's missed cycles ----------------------------------
//
// p.196: "Some 68000 instructions do not match perfectly with the allocation
// of even cycles and cause cycles to be missed. If cycles are missed, the
// 68000 must wait until its next available memory slot before continuing."
//
// The RAM port shows the cycles in which an access happened, never the
// cycle in which the 68000 asked for one, so a miss is identified from two
// accesses of the 68000's own and the address pair they form (a program
// fetch or the scratch write each has its own address, so the pair names the
// instruction boundary it brackets):
//  - with no DMA access in any slot between them, the second follows the
//    first after a gap of `g` slots, the pair's unimpeded gap; the largest
//    unimpeded gap measured for the pair in the run is `g`, so the 68000 is
//    ready for the second access `g` slots after the first at the latest;
//  - when the pair is further apart than `g`, the slot t = first + g is the
//    slot the 68000 was ready for. If a DMA access took t, the cycle was
//    missed, and the 68000 had to wait for its next available slot.
// A slot is numbered by Figure 6-9's per-line numbering (the first refresh
// slot of a line is number 1, so odd numbers carry refresh, and the even
// numbers are the 68000's); a pair that spans a line start is left out,
// because the numbering restarts there (a line is 227 slots, an odd number),
// and so is a pair a full line or more past its line's anchor, which has
// no numbering of its own.

struct CpuAccess { int64_t slot; uint32_t addr; bool wr; };

struct SlotMap {
    bool ok = false;
    int64_t phase = 0;                  // cycle modulo kCck of a refresh access
    std::vector<char> occ;              // slot -> a DMA (non-CPU) access took it
    std::vector<int64_t> line_first;    // slot of each line's first refresh access
    int64_t line_len = 0;               // the modal gap between consecutive line_first entries, in slots
    std::vector<CpuAccess> cpu;         // the 68000's own accesses, in order
    int64_t slot_of(uint64_t cycle) const { return ((int64_t)cycle - phase) / (int64_t)kCck; }
    bool occupied(int64_t s) const { return s >= 0 && s < (int64_t)occ.size() && occ[(size_t)s]; }
    int64_t line_index(int64_t s) const {
        size_t i = std::upper_bound(line_first.begin(), line_first.end(), s) - line_first.begin();
        return i == 0 ? -1 : (int64_t)i - 1;
    }
    // A slot less than one line past its anchor is numbered; one a full line or
    // more past it (a stretch with no anchor) is not.
    bool anchored(int64_t s) const {
        int64_t i = line_index(s);
        return i >= 0 && line_len > 0 && s - line_first[(size_t)i] < line_len;
    }
    // Even-numbered by Figure 6-9: the line's first refresh slot is number 1.
    bool fig_even(int64_t s) const {
        int64_t i = line_index(s);
        return i >= 0 && ((s - line_first[(size_t)i]) % 2) == 1;
    }
};

SlotMap make_slot_map(const Capture& c, uint32_t scratch_word) {
    SlotMap m;
    LineRef ref = make_line_ref(c.log);
    if (ref.groups.empty() || c.log.empty()) return m;
    m.phase = (int64_t)(ref.groups[0].first % kCck);
    m.occ.assign((size_t)(((int64_t)c.log.back().cycle - m.phase) / (int64_t)kCck) + 2, 0);
    for (const auto& a : c.log) {
        int64_t s = m.slot_of(a.cycle);
        if (is_cpu_access(a, scratch_word)) m.cpu.push_back({s, a.word_addr, a.is_write});
        else m.occ[(size_t)s] = 1;
    }
    for (uint64_t f : ref.firsts) m.line_first.push_back(m.slot_of(f));
    m.line_len = (int64_t)modal_gap(std::vector<uint64_t>(m.line_first.begin(), m.line_first.end()));
    m.ok = true;
    return m;
}

typedef std::tuple<uint32_t, bool, uint32_t, bool> PairKey;   // address and direction of both accesses

// The unimpeded gap of every address pair of a run: the largest gap, in
// slots, over its occurrences with no DMA access in any slot between.
std::map<PairKey, int64_t> unimpeded_gaps(const SlotMap& m) {
    std::map<PairKey, int64_t> g;
    for (size_t i = 1; i < m.cpu.size(); ++i) {
        const CpuAccess& y = m.cpu[i - 1];
        const CpuAccess& x = m.cpu[i];
        bool free_between = true;
        for (int64_t s = y.slot + 1; s < x.slot && free_between; ++s) if (m.occupied(s)) free_between = false;
        if (!free_between) continue;
        PairKey k{y.addr, y.wr, x.addr, x.wr};
        auto it = g.find(k);
        if (it == g.end() || it->second < x.slot - y.slot) g[k] = x.slot - y.slot;
    }
    return g;
}

struct MissStats {
    size_t pairs = 0;          // consecutive 68000 access pairs examined
    size_t nokey = 0;          // pairs whose address pair has no unimpeded gap
    size_t crossing = 0;       // delayed pairs that span a line start or lie a full line or more past their line's anchor
    size_t delayed_free = 0;   // delayed pairs whose ready slot no DMA access took
    size_t miss_even = 0;      // missed cycles: the ready slot is even-numbered and a DMA access took it
    size_t miss_odd = 0;       // missed cycles: the ready slot is odd-numbered and a DMA access took it
    size_t skipped = 0;        // missed even cycles with an even-numbered free slot left unused before the access
    size_t early = 0;          // missed even cycles whose access came before the first free even-numbered slot (on an odd slot)
    size_t on_first = 0;       // missed even cycles whose access came exactly on the first free even-numbered slot
    size_t at_fourth = 0;      // missed even cycles whose access came on the third slot after the ready slot, that slot and the two after it taken by DMA
    uint32_t ex_from = 0, ex_to = 0; bool ex_wr_from = false, ex_wr_to = false; int64_t ex_gap = 0; // first missed pair
};

MissStats analyse_misses(const SlotMap& m, const std::map<PairKey, int64_t>& g0) {
    MissStats st;
    for (size_t i = 1; i < m.cpu.size(); ++i) {
        const CpuAccess& y = m.cpu[i - 1];
        const CpuAccess& x = m.cpu[i];
        ++st.pairs;
        auto it = g0.find(PairKey{y.addr, y.wr, x.addr, x.wr});
        if (it == g0.end()) { ++st.nokey; continue; }
        int64_t g = it->second;
        if (x.slot - y.slot <= g) continue;                          // not delayed
        if (m.line_index(y.slot) != m.line_index(x.slot) || !m.anchored(y.slot) || !m.anchored(x.slot)) { ++st.crossing; continue; }
        int64_t t = y.slot + g;                                       // the slot the 68000 was ready for
        if (!m.occupied(t)) { ++st.delayed_free; continue; }
        bool even = m.fig_even(t);
        if (even) ++st.miss_even; else ++st.miss_odd;
        if (st.ex_gap == 0) { st.ex_from = y.addr; st.ex_wr_from = y.wr; st.ex_to = x.addr; st.ex_wr_to = x.wr; st.ex_gap = x.slot - y.slot; }
        if (even && x.slot == t + 3 && m.occupied(t + 1) && m.occupied(t + 2)) ++st.at_fourth;
        if (even) {
            int64_t f = -1;                                           // first free even-numbered slot at or after t
            for (int64_t s = t; s <= x.slot; ++s)
                if (m.fig_even(s) && !m.occupied(s)) { f = s; break; }
            if (f < 0) ++st.early;
            else if (x.slot == f) ++st.on_first;
            else ++st.skipped;
        }
    }
    return st;
}

std::string miss_note(const MissStats& st) {
    return std::to_string(st.pairs) + " access pairs, " + std::to_string(st.miss_even) + " missed even cycle(s), " +
           std::to_string(st.miss_odd) + " missed odd cycle(s), " + std::to_string(st.skipped) +
           " with an even free slot left unused (" + std::to_string(st.early) + " granted before the first free even slot, " +
           std::to_string(st.on_first) + " exactly on it), " + std::to_string(st.delayed_free) + " delayed with a free ready slot, " +
           std::to_string(st.crossing) + " spanning a line start, " + std::to_string(st.nokey) + " without an unimpeded gap";
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

    // ---- A1 [p170] "The two words of each register are adjacent in the 68000
    // address space, with the high address word first, so they can both be
    // written with one 32-bit write from the processor." Each of the four
    // pointer registers (A, B, C, D) is exercised on its own: in its blit
    // only that channel's pointer is set by a single MOVE.L, so a pointer
    // that missed its target shows in that channel's destination word alone.
    // The component lines carry no claim id; the aggregate carries them.
    {
        std::string bin = "/prog_pointer.bin";
        if (mv == "A1") bin = "/prog_pointer_mutant.bin";
        else if (mv == "A1-A") bin = "/prog_pointer_mutant_a.bin";
        else if (mv == "A1-B") bin = "/prog_pointer_mutant_b.bin";
        else if (mv == "A1-C") bin = "/prog_pointer_mutant_c.bin";
        else if (mv == "A1-D") bin = "/prog_pointer_mutant_d.bin";
        Capture p = run_capture(dir + bin, kBootBudget + kDoneRunBudget, true,
                                {kResults, kResults + 2, kResults + 4, kResults + 6});
        bool ran = p.loaded && p.done.has_value() && p.peeks.size() == 4;
        auto is = [&](size_t k, uint16_t want) { return ran && p.peeks[k] && *p.peeks[k] == want; };
        bool a_ok = is(0, 0xCAFE), b_ok = is(1, 0xB00B), c_ok = is(2, 0xC0DE), d_ok = is(3, 0xD00D);
        checks.expect(a_ok,
            "A1-A [p170]: a single 32-bit MOVE.L to BLTAPTH sets both words of the A pointer -- the A-to-D blit "
            "writes the source word $CAFE");
        checks.expect(b_ok,
            "A1-B [p170]: a single 32-bit MOVE.L to BLTBPTH sets both words of the B pointer -- the B-to-D blit "
            "writes the source word $B00B");
        checks.expect(c_ok,
            "A1-C [p170]: a single 32-bit MOVE.L to BLTCPTH sets both words of the C pointer -- the C-to-D blit "
            "writes the source word $C0DE");
        checks.expect(d_ok,
            "A1-D [p170]: a single 32-bit MOVE.L to BLTDPTH sets both words of the D pointer -- the A-to-D blit "
            "writes the source word $D00D to the intended destination word");
        checks.expect(a_ok && b_ok && c_ok && d_ok,
            "A1 [p170] 6-dma-channels.c16,6-dma-channels.c18,ch6-dma.c14,ch6-dma.c16: the two words of each of the "
            "blitter's pointer registers (A, B, C and D) are adjacent, high word first, and one 32-bit write sets both "
            "-- every channel's blit uses the intended word");
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
        uint32_t dst_n = kEcDstLargeN;
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
            "BBUSY-PREDICTS [p186]: DMAF_BLTDONE (DMACONR bit 14) \"is set when a blit is in progress\" -- "
            "after the prescribed intervening read, BBUSY reading set predicts more destination "
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
    Capture cap_lores5 = run_capture(dir + "/prog_bitplane_planes5.bin", kBpRunBudget, false, {});
    Capture cap_lores1_aligned = run_capture(dir + "/prog_bitplane_aligned_p1.bin", kBpRunBudget, false, {});
    Capture cap_lores2_aligned = run_capture(dir + "/prog_bitplane_aligned_p2.bin", kBpRunBudget, false, {});
    Capture cap_lores3_aligned = run_capture(dir + "/prog_bitplane_aligned_p3.bin", kBpRunBudget, false, {});

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

    // ---- SLOT-UNIT [Figure 6-9, p194,p195] (no claim id): the slot unit every
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
            "SLOT-UNIT [Figure 6-9, p194,p195]: every line's refresh group holds exactly four accesses at 0, 2, 4 "
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

    // ---- A2 [p169,p186,p198] the CPU keeps running while the blitter works --
    // existence only: p.169 ("the 680x0 CPU continues to run as the blit
    // is executing") and p.186 ("they can both work concurrently") give
    // no bound on how long the CPU may then wait between two chip-bus
    // writes -- p.198's own release rule (three denied cycles, one
    // released) is what governs that, withdrawn below (A5-WITHDRAWN).
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
    // the blitter's priority over the 68000 say "normally", which
    // A4-SHARE-WITHDRAWN below records for share mode.
    {
        const Capture& cap = (mv == "A4PRE" ? cap_nasty_tiny : cap_nasty);
        auto [lo, hi] = dst_span(cap);
        // Every CPU Chip-memory access, classified on the RAM-log basis of
        // is_cpu_access(): a program fetch or a write to the counter.
        CpuHits hits = cpu_hits_in(cap.log, lo, hi, word_addr(kShareScratch), 8);
        checks.note("A4: window " + std::to_string(lo) + ".." + std::to_string(hi) + ", " + std::to_string(hits.n) +
                    " CPU Chip-memory access(es) inside it" + (hits.n ? ": " + hits.list : std::string("")) +
                    " (offsets from the window's first destination write)");
        bool nontrivial = lo != hi && (hi - lo) > 2000;
        checks.expect(nontrivial,
            "A4PRE: the nasty-mode blit's active window is long enough to have contained "
            "several CPU-loop iterations had the CPU been let through (" + std::to_string(hi - lo) + " cycles)");
        if (nontrivial) {
            checks.expect(hits.n == 0,
                "A4 [p198]: with DMAF_BLITHOG (BLTPRI) set the blitter keeps the bus for every available "
                "Chip memory cycle -- the CPU makes no Chip-memory access at all (program fetch or counter "
                "write) during the blit's active window (" + std::to_string(hits.n) + " accesses, " +
                std::to_string(hits.writes) + " of them counter writes)");
        } else {
            not_graded(checks, "A4", "A4PRE");
        }
    }

    // ---- A4-CTRL: the comparator of A4 (is_cpu_access(), count zero over a
    // window, through cpu_hits_in()) on a live input whose answer is zero:
    // the four-high-resolution-bitplane display's fetch windows, in which
    // every one of the 160 slots is a bitplane fetch (A10PRE) while the CPU
    // runs outside them.
    if (gated("A4-CTRL")) {
        const Capture& pre_cap = (mv == "A4-CTRL-PRE") ? cap_base : cap_hires4;
        auto pre_wins = fetch_windows(bp_reads_of(pre_cap));
        auto pre_cyc = cyc_of(pre_cap);
        bool pre_full = !pre_wins.empty();
        size_t pre_inside = 0;
        for (auto& w : pre_wins) {
            WinCounts wc = count_window(pre_cap.log, pre_cyc, w.lo, w.hi, word_addr(kShareScratch));
            if (wc.dma != 160) pre_full = false;
            pre_inside += wc.cpu;
        }
        size_t pre_total = 0;
        for (const auto& a : pre_cap.log) if (is_cpu_access(a, word_addr(kShareScratch))) ++pre_total;
        bool pre_live = pre_total > pre_inside;
        checks.expect(pre_full && pre_live,
            "A4-CTRL-PRE: every one of the four-high-resolution-bitplane display's fetch windows (" +
            std::to_string(pre_wins.size()) + " window(s)) holds 160 bitplane fetches and no free slot, and the CPU "
            "makes Chip-memory accesses outside them (" + std::to_string(pre_total - pre_inside) +
            ") [not graded -- apparatus check, no claim uid]");
        if (pre_full && pre_live) {
            const Capture& cap = (mv == "A4-CTRL") ? cap_lores4 : cap_hires4;
            auto wins = fetch_windows(bp_reads_of(cap));
            size_t n = 0, writes = 0;
            for (auto& w : wins) {
                CpuHits h = cpu_hits_in(cap.log, w.lo, w.hi, word_addr(kShareScratch), 0);
                n += h.n; writes += h.writes;
            }
            checks.note("A4-CTRL: " + std::to_string(wins.size()) + " fetch window(s), " + std::to_string(n) +
                        " CPU Chip-memory access(es) inside them (" + std::to_string(writes) + " counter writes)");
            checks.expect(!wins.empty() && n == 0,
                "A4-CTRL: the comparator of A4 (program fetches and counter writes counted over a window, "
                "zero required) finds no CPU Chip-memory access in any of the " + std::to_string(wins.size()) +
                " fetch windows of the four-high-resolution-bitplane display (" + std::to_string(n) +
                " accesses) [not graded -- apparatus control, no claim uid]");
        } else {
            not_graded(checks, "A4-CTRL", "A4-CTRL-PRE");
        }
    }

    // ---- A4-SHARE [p194,p198] with BLTPRI clear (share mode) a CPU loop that
    // is continuously ready (the BASELINE build runs it at its own free rate)
    // makes fewer chip-bus accesses per slot inside the blit's active window
    // than it makes with the bus to itself. No claim id is carried: the
    // claims say the blitter "normally has a higher priority" than the 68000,
    // and a lower CPU rate in the window follows as well when the two
    // requesters alternate with neither outranking the other, so the line
    // does not separate the two (A4-SHARE-WITHDRAWN below). Rates are compared
    // by cross multiplication (accesses * reference span < reference accesses
    // * span), so no threshold and no division enters.
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
        // Graded side: the share-mode blit's active window. A4-SHARE-PRE's
        // mutation shrinks the blit to nothing.
        const Capture& cap = (mv == "A4-SHARE-PRE") ? cap_tiny : cap_share;
        std::pair<uint64_t, uint64_t> gw = dst_span(cap);
        auto [lo, hi] = gw;
        Rate win = cpu_rate(cap, lo, hi);
        // Reference: the same loop with the bus to itself (the BASELINE
        // capture, first to last CPU access).
        // NMH_MUTATE=A4-SHARE substitutes a slower no-DMA loop for the reference.
        Capture cap_slow;
        if (mv == "A4-SHARE") {
            nmh::Config cfg_slow;
            cfg_slow.fastram_config = 0x01;   // Zorro II fast RAM offered, as in FASTRUN's runs
            cap_slow = run_capture(dir + "/prog_sharing_slow.bin", kShareRunBudget, false, {}, cfg_slow);
        }
        const Capture& cap_ref = (mv == "A4-SHARE") ? cap_slow : cap_base;
        std::pair<uint64_t, uint64_t> rw = cpu_span(cap_ref);
        Rate ref = cpu_rate(cap_ref, rw.first, rw.second);
        bool pre = lo != hi && win.span > 0 && ref.span > 0 && ref.n > 0;
        checks.note("A4-SHARE: CPU accesses " + std::to_string(win.n) + " in a share-mode window of " +
                    std::to_string(win.span) + " clk_sys; reference " + std::to_string(ref.n) + " in " +
                    std::to_string(ref.span) + " clk_sys (no-DMA run)");
        checks.expect(pre,
            "A4-SHARE-PRE: the share-mode blit has a nonempty active window and the reference run holds "
            "CPU accesses (window " + std::to_string(win.span) + " clk_sys, reference " + std::to_string(ref.span) +
            " clk_sys) [not graded -- apparatus check, no claim uid]");
        if (pre) {
            checks.expect(win.n * ref.span < ref.n * win.span,
                "A4-SHARE [p194,p198]: with BLTPRI clear the continuously ready CPU loop makes fewer chip-bus "
                "accesses per clk_sys inside the share-mode blit's active window (" +
                std::to_string(win.n) + " in " + std::to_string(win.span) + ") than with the bus to itself (" +
                std::to_string(ref.n) + " in " + std::to_string(ref.span) + ")");
        } else {
            not_graded(checks, "A4-SHARE", "A4-SHARE-PRE");
        }
        checks.record(
            "A4-SHARE-WITHDRAWN [p194,p198] 6-system-dma.c13,ch6-sysdma.c11,6-blitter-priority.c1,ch6-sysdma.c52: "
            "withdrawn -- that the blitter outranks the 68000 is a statement about which requester is served "
            "when both ask, and the RAM port shows only the cycles in which an access happened, never one in "
            "which a request was raised and denied (the reason of A5-WITHDRAWN); a lower CPU access rate in the "
            "share-mode window also follows when the two requesters alternate with neither outranking the "
            "other, so the line above, which compares those rates, carries no claim id");
    }

    // ---- A5 [p198] share mode: "If the 68000 is unsatisfied for three
    // consecutive memory cycles, the blitter will release the bus for one
    // cycle" -- WITHDRAWN. The sentence is explicit, but its antecedent is
    // the 68000's own bus request: a request is unsatisfied from the cycle it
    // is raised, and the RAM-port log shows only the cycles in which an
    // access happened, never the cycle in which one was requested and
    // denied. The count of consecutive unsatisfied cycles, and with it the
    // cycle at which the blitter's release is due, cannot be located at this
    // boundary. The page also leaves open the unit in which the count runs
    // and which cycle is the released one. The measured lines below are
    // computed from the run and graded by nothing.
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
        if (slot_ok) {
            SlotMap m5 = make_slot_map(cap_share, word_addr(kShareScratch));
            MissStats st5 = analyse_misses(m5, unimpeded_gaps(m5));
            checks.note("A5: of " + std::to_string(st5.miss_even) + " missed even cycles in the share-mode run, " +
                        std::to_string(st5.at_fourth) + " were granted on the third slot after the slot the 68000 was ready for (the fourth counting that slot), " +
                        "that slot and the two after it taken by DMA");
        }
        checks.record(
            "A5-WITHDRAWN [p198] 6-blitter-priority.c13,ch6-sysdma.c62: withdrawn -- the page's rule "
            "(three consecutive unsatisfied 68000 memory cycles, then the blitter releases the bus for one "
            "cycle) does not say where the count starts, in which unit it runs, or which cycle is the "
            "released one: from the cycle the request is raised, which the RAM port (it shows only the "
            "cycles in which an access happened) does not show, or from the first even-numbered slot the "
            "68000 was ready for; in memory cycles, so that the release is the fourth slot and an "
            "odd-numbered one, or in the 68000's own alternate cycles, so that it is the seventh and an "
            "even-numbered one, which p.196 (the 68000 uses only the even-numbered cycles) would require; "
            "the measured pattern (the A5 measurement above: grants on the third slot after the ready "
            "slot, the ready slot and the two after it DMA-taken) fits the first reading in each pair, and grading it "
            "would adopt that reading from the run instead of from the page");
    }

    // ---- A8 [p196,p198] "If cycles are missed, the 68000 must wait until its
    // next available memory slot before continuing." A cycle is missed when a
    // DMA access takes the even-numbered slot the 68000 was ready for (see
    // "slots, and the 68000's missed cycles" above: the slot is found from two
    // accesses of the 68000's own, identified by address, and the largest
    // unimpeded gap of that address pair in the same run). The 68000's next
    // available slot is the first even-numbered slot no DMA access takes:
    // p.196 says the 68000 uses only the even-numbered memory access cycles
    // (PARITY grades that sentence). The graded line takes both halves of the
    // clause: the access comes neither later nor sooner than the first free
    // even-numbered slot, i.e. no free even-numbered slot lies unused between
    // the missed slot and the access, and no access falls on an earlier slot.
    // Population: the share-mode blit's own run, where the blitter takes slots
    // from the 68000 throughout the blit's active window.
    if (gated("A8")) {
        Capture cap_a8;
        if (mv == "A8") cap_a8 = run_capture(dir + "/prog_bitplane_planes6_aligned_ptrs.bin", kBpRunBudget, false, {});
        const Capture& cap = (mv == "A8PRE") ? cap_base : (mv == "A8") ? cap_a8 : cap_share;
        SlotMap m = make_slot_map(cap, word_addr(kShareScratch));
        std::map<PairKey, int64_t> g0 = unimpeded_gaps(m);
        MissStats st = analyse_misses(m, g0);
        std::string ex = st.ex_gap == 0 ? std::string("none")
                       : std::string(st.ex_wr_from ? "write $" : "fetch $") + hexs(st.ex_from * 2) + " -> " +
                         (st.ex_wr_to ? "write $" : "fetch $") + hexs(st.ex_to * 2) + ", " + std::to_string(st.ex_gap) + " slots";
        checks.note("A8: " + miss_note(st) + "; first missed pair: " + ex);
        bool pre = m.ok && st.miss_even > 0;
        checks.expect(pre,
            "A8PRE [p196]: some 68000 cycles are missed on even-numbered slots -- a DMA access takes the slot the "
            "68000 was ready for (" + std::to_string(st.miss_even) + " missed even cycle(s); first missed cycle of either parity: " + ex + ")");
        if (pre) {
            checks.expect(st.skipped == 0 && st.early == 0,
                "A8 [p196,p198]: every 68000 access whose ready even-numbered slot a DMA access took is granted "
                "exactly at the first free even-numbered slot -- not before it (a grant before it is on an "
                "odd-numbered slot, the event PARITY counts) and not after it (" + std::to_string(st.early) + " granted before it, " + std::to_string(st.skipped) +
                " after a free even-numbered slot was left unused, of " + std::to_string(st.miss_even) +
                " missed even cycles)");
        } else {
            not_graded(checks, "A8", "A8PRE");
        }
    }
    checks.record(
        "A8-BOUNDARY [p196] 6-system-dma.c54,6-system-dma.c55,ch6-sysdma.c30: not graded -- the page's missed cycles are "
        "those an instruction causes by not matching the allocation of even cycles, which it sets apart from blitter "
        "DMA interference (\"most instructions do not cause cycles to be missed, so the 68000 runs at full speed most "
        "of the time if there is no blitter DMA interference\"); A8 grades cycles a share-mode blit took from a ready "
        "68000, whose release p.198 governs, and its grants before the first free even-numbered slot are "
        "odd-numbered grants, which PARITY grades; an instruction's own miss is not found on an implementation that "
        "keeps the 68000 on even-numbered slots (SYSDMA-C29-BOUNDARY)");
    // ---- SYSDMA-C29 [p196] ch6-sysdma.c29: "Some 68000 instructions do not
    // match perfectly with the allocation of even cycles and cause cycles to be
    // missed." NOT GRADED (SYSDMA-C29-BOUNDARY below). The counts are printed
    // as measurements: the counting loop of the four-low-resolution-bitplane
    // display ends in BRA.S (10 clocks, five slots, an odd number), and the
    // same display with the loop ended by JMP (An) (8 clocks, four slots) is
    // the control; the loop sits at the same addresses in both builds. A
    // cycle missed by an instruction is one for which the 68000 is ready off
    // its even-numbered allocation (an odd-numbered slot, which the display
    // takes), found by the procedure of "slots, and the 68000's missed cycles".
    if (slot_ok) {
        for (int k = 0; k < 2; ++k) {
            const Capture& cap = (k == 0) ? cap_lores4 : cap_lores4_aligned;
            SlotMap m = make_slot_map(cap, word_addr(kBpScratch));
            std::map<PairKey, int64_t> g0 = unimpeded_gaps(m);
            MissStats st = analyse_misses(m, g0);
            std::string ex = st.ex_gap == 0 ? std::string("none")
                           : std::string(st.ex_wr_from ? "write $" : "fetch $") + hexs(st.ex_from * 2) + " -> " +
                             (st.ex_wr_to ? "write $" : "fetch $") + hexs(st.ex_to * 2) + ", " + std::to_string(st.ex_gap) + " slots";
            checks.note(std::string("SYSDMA-C29: four lores planes, ") + (k == 0 ? "BRA.S loop" : "aligned loop") + ": " +
                        miss_note(st) + "; first missed pair: " + ex);
        }
    }
    checks.record(
        "SYSDMA-C29-BOUNDARY [p196] ch6-sysdma.c29: not graded -- an instruction misses a cycle when it leaves the "
        "68000 ready off its even-numbered allocation; the procedure finds that slot as the first access plus the "
        "address pair's largest unimpeded gap, so on an implementation that keeps every 68000 access on an "
        "even-numbered slot, as p.196 requires, the wait is in every occurrence of the pair, the gap absorbs it and "
        "no miss is found whatever the instruction; a count above zero (the BRA.S loop on fx68k) exists only where "
        "the 68000 is granted odd-numbered slots, which PARITY grades");

    // ---- A9 [p196] "If the display contains four or fewer low resolution
    // bitplanes, the 68000 can be granted alternate memory cycles (if it is
    // ready to ask for the cycle and is the highest priority item at the
    // time)." Alternate memory cycles are every second slot, and the even
    // ones (Figure 6-10). The stimulus is a 68000 that is continuously ready,
    // the whole-4-clock-aligned counting loop, with nothing else enabled
    // beside the display, so that nothing ranks above it but the display's own
    // slots: for one, two, three and four low-resolution bitplanes, every pair
    // of consecutive CPU accesses inside a line's fetch window must be exactly
    // two slots apart and on an even-numbered slot. The precondition is that
    // the windows are found and that the loop is continuously ready: with no
    // DMA access between two of its accesses they are two slots apart.
    if (gated("A9")) {
        struct PlaneRun { int planes; const Capture* cap; };
        std::vector<PlaneRun> runs = {{1, &cap_lores1_aligned}, {2, &cap_lores2_aligned}, {3, &cap_lores3_aligned},
                                      {4, (mv == "A9") ? &cap_lores6_aligned : &cap_lores4_aligned}};
        if (mv == "A9PRE") runs[3].cap = &cap_base_aligned;
        bool pre = true, pattern = true;
        std::string per_pre, per_pat;
        for (const auto& r : runs) {
            auto wins = fetch_windows(bp_reads_of(*r.cap));
            SlotMap m = make_slot_map(*r.cap, word_addr(kBpScratch));
            std::vector<int64_t> slo, shi;
            for (const auto& w : wins) { slo.push_back(m.slot_of(w.lo)); shi.push_back(m.slot_of(w.hi)); }
            auto win_of = [&](int64_t s) -> int {
                size_t k = std::upper_bound(slo.begin(), slo.end(), s) - slo.begin();
                if (k == 0) return -1;
                --k;
                return s <= shi[k] ? (int)k : -1;
            };
            size_t pairs = 0, bad = 0, free_pairs = 0, free_off = 0;
            // The loop is running from its first scratch write on; the setup before it is not the loop.
            int64_t loop_from = -1;
            for (const auto& a : m.cpu) if (a.wr) { loop_from = a.slot; break; }
            for (size_t i = 1; m.ok && i < m.cpu.size(); ++i) {
                const CpuAccess& y = m.cpu[i - 1];
                const CpuAccess& x = m.cpu[i];
                int wy = win_of(y.slot), wx = win_of(x.slot);
                if (wy >= 0 && wy == wx) {
                    ++pairs;
                    if (x.slot - y.slot != 2 || !m.fig_even(y.slot) || !m.fig_even(x.slot)) ++bad;
                }
                bool free_between = true;
                for (int64_t s = y.slot + 1; s < x.slot && free_between; ++s) if (m.occupied(s)) free_between = false;
                if (free_between && loop_from >= 0 && y.slot >= loop_from && m.line_index(y.slot) == m.line_index(x.slot)) {
                    ++free_pairs;
                    if (x.slot - y.slot != 2) ++free_off;
                }
            }
            bool ready = free_pairs > 0 && free_off == 0;
            pre = pre && m.ok && !wins.empty() && ready;
            pattern = pattern && pairs > 0 && bad == 0;
            per_pre += " " + std::to_string(r.planes) + "pl:" + std::to_string(wins.size()) + "win/" +
                       std::to_string(free_pairs - free_off) + "of" + std::to_string(free_pairs) + "free";
            per_pat += " " + std::to_string(r.planes) + "pl:" + std::to_string(bad) + "of" + std::to_string(pairs);
        }
        checks.expect(pre,
            "A9PRE [p196]: for one to four low-resolution bitplanes the fetch windows are found and the aligned "
            "loop is continuously ready -- from its first scratch write on, two slots between consecutive "
            "accesses of one line wherever no DMA access lies between them (windows / ready pairs of free pairs:" +
            per_pre + ")");
        if (pre) {
            checks.expect(pattern,
                "A9 [p196] 6-system-dma.c61,ch6-sysdma.c34: with four or fewer low-resolution bitplanes the 68000 is "
                "granted alternate memory cycles -- every pair of consecutive CPU accesses inside a fetch window is "
                "exactly two slots apart, on even-numbered slots (pairs off the pattern of all pairs:" + per_pat + ")");
        } else {
            not_graded(checks, "A9", "A9PRE");
        }
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

    // ---- A11 [p196] "if there are more than four bitplanes, bitplane DMA will
    // begin to steal cycles from the 68000 during the display." Graded inside
    // the display windows: the beam is the same in every run (the boot is
    // identical until the display starts), so a window found at the same
    // cycle in the four-, five- and six-plane runs is the same line's fetch
    // window. In every such window the CPU makes strictly fewer chip-bus
    // accesses with five planes than with four, and with six than with four.
    // No percentage is asked for: the page gives only the direction.
    if (gated("A11")) {
        struct WinCpu { std::map<uint64_t, size_t> by_hi; size_t outside = 0; };
        auto per_window = [](const Capture& c) {
            WinCpu w;
            auto wins = fetch_windows(bp_reads_of(c));
            auto cyc = cyc_of(c);
            for (const auto& fw : wins) w.by_hi[fw.hi] = count_window(c.log, cyc, fw.lo, fw.hi, word_addr(kBpScratch)).cpu;
            size_t inside = 0;
            for (const auto& kv : w.by_hi) inside += kv.second;
            size_t total = 0;
            for (const auto& a : c.log) if (is_cpu_access(a, word_addr(kBpScratch))) ++total;
            w.outside = total - inside;
            return w;
        };
        WinCpu w4 = per_window(cap_lores4);
        WinCpu w5 = per_window((mv == "A11PRE") ? cap_base : (mv == "A11") ? cap_lores4 : cap_lores5);
        WinCpu w6 = per_window((mv == "A11") ? cap_lores4 : cap_lores6);
        size_t matched = 0, fewer5 = 0, fewer6 = 0;
        for (const auto& kv : w4.by_hi) {
            auto i5 = w5.by_hi.find(kv.first);
            auto i6 = w6.by_hi.find(kv.first);
            if (i5 == w5.by_hi.end() || i6 == w6.by_hi.end()) continue;
            ++matched;
            if (i5->second < kv.second) ++fewer5;
            if (i6->second < kv.second) ++fewer6;
        }
        bool pre = matched > 0 && matched == w4.by_hi.size() && matched == w5.by_hi.size() && matched == w6.by_hi.size() &&
                   w4.outside > 0 && w5.outside > 0 && w6.outside > 0;
        checks.note("A11: " + std::to_string(w4.by_hi.size()) + " four-plane window(s), " + std::to_string(matched) +
                    " found at the same cycle with five and six planes; CPU accesses outside the windows: " +
                    std::to_string(w4.outside) + " / " + std::to_string(w5.outside) + " / " + std::to_string(w6.outside));
        checks.expect(pre,
            "A11PRE [p196]: the four-, five- and six-plane runs show the same display windows (" +
            std::to_string(matched) + " of " + std::to_string(w4.by_hi.size()) + ", " + std::to_string(w5.by_hi.size()) +
            ", " + std::to_string(w6.by_hi.size()) + ") and the CPU keeps running in all three (accesses outside the windows) "
            "[not graded -- apparatus check, no claim uid]");
        if (pre) {
            checks.expect(fewer5 == matched && fewer6 == matched,
                "A11 [p196] 6-system-dma.c62,ch6-sysdma.c36: with more than four low-resolution bitplanes bitplane DMA "
                "steals cycles from the 68000 during the display -- inside every display window the CPU makes strictly "
                "fewer chip-bus accesses with five planes (" + std::to_string(fewer5) + " of " + std::to_string(matched) +
                " windows) and with six planes (" + std::to_string(fewer6) + " of " + std::to_string(matched) +
                " windows) than with four");
        } else {
            not_graded(checks, "A11", "A11PRE");
        }
    }

    // ---- A6 [p196] "the allocation of alternate memory cycles to the 68000
    // makes it appear to the 68000 that it has the memory all of the time, and
    // it will run at full speed." The stimulus loop's own bus cycles must all
    // be whole multiples of 4 clocks, an even number of slots (MC68000 timing
    // tables: ADDQ.W Dn = 4 clocks, MOVE.W
    // Dn,(An) = 8, JMP (An) = 8 -- every one a whole multiple of 4, unlike
    // BRA.S's 10, five slots; see SYSDMA-C29 above and prog_sharing_aligned.s). Full speed
    // is the loop's period with the bus to itself, the interval between two
    // scratch writes with no DMA access in any slot between them; it is
    // measured on the no-DMA run of the same loop. Inside the four-low-
    // resolution-bitplane display windows every interval between two scratch
    // writes of one window must equal it -- every one, not the most frequent.
    if (gated("A6")) {
        const Capture& cap = (mv == "A6") ? cap_lores6_aligned : cap_lores4_aligned;
        auto scratch_intervals = [](const Capture& c, bool unimpeded_only, const std::vector<FetchWindow>* wins) {
            SlotMap m = make_slot_map(c, word_addr(kBpScratch));
            std::vector<uint64_t> out;
            if (!m.ok) return out;
            std::vector<uint64_t> w;
            for (const auto& a : c.log) if (a.is_write && a.word_addr == word_addr(kBpScratch)) w.push_back(a.cycle);
            for (size_t i = 1; i < w.size(); ++i) {
                if (unimpeded_only) {
                    bool free_between = true;
                    for (int64_t s = m.slot_of(w[i - 1]) + 1; s < m.slot_of(w[i]) && free_between; ++s)
                        if (m.occupied(s)) free_between = false;
                    if (free_between) out.push_back(w[i] - w[i - 1]);
                } else {
                    for (const auto& fw : *wins)
                        if (w[i - 1] >= fw.lo && w[i] <= fw.hi) { out.push_back(w[i] - w[i - 1]); break; }
                }
            }
            return out;
        };
        auto wins = fetch_windows(bp_reads_of(cap));
        std::vector<uint64_t> in_win = scratch_intervals(cap, false, &wins);
        std::vector<uint64_t> free_run = scratch_intervals(cap_base_aligned, true, nullptr);
        uint64_t period = mode_of(free_run);
        size_t off = 0;
        for (auto g : in_win) if (g != period) ++off;
        checks.note("A6: no-DMA period of the aligned loop = " + std::to_string(period) + " clk_sys (" +
                    std::to_string(free_run.size()) + " unimpeded intervals); " + std::to_string(in_win.size()) +
                    " scratch-write interval(s) inside the display windows, " + std::to_string(off) + " not equal to it");
        checks.expect(period > 0 && !in_win.empty() && off == 0,
            "A6 [p196] 6-system-dma.c53,ch6-sysdma.c28: with a stimulus loop whose own bus cycles all align to whole "
            "slots (MC68000 timing tables), the CPU runs at full speed in the four-low-resolution-bitplane display -- "
            "every interval between two scratch writes inside a fetch window (" + std::to_string(in_win.size()) +
            " intervals) equals the loop's no-DMA period (" + std::to_string(period) + " clk_sys)");
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


    // ---- OPEN50 [p196,p197] "During the display time for a six bitplane
    // display (low resolution, 320 pixels wide), 160 time slots will be taken
    // by bitplane DMA for each horizontal line. As you can see from Figure
    // 6-11, bitplane DMA steals 50 percent of the open slots that the processor
    // might have used if there were only four bitplanes displayed." Each
    // fetch window spans the 160 time slots of the page; a slot not taken by a
    // DMA access is open. Graded in every window, not in the median one: all
    // four-plane windows leave the same number of slots open, all six-plane
    // windows leave half as many.
    if (gated("OPEN50")) {
        auto open_per_window = [](const Capture& c, bool& all_160) {
            std::vector<int64_t> open;
            SlotMap m = make_slot_map(c, word_addr(kBpScratch));
            auto wins = fetch_windows(bp_reads_of(c));
            auto cyc = cyc_of(c);
            all_160 = m.ok && !wins.empty();
            for (const auto& w : wins) {
                if (m.slot_of(w.hi) - m.slot_of(w.lo) + 1 != 160) all_160 = false;
                open.push_back(160 - (int64_t)count_window(c.log, cyc, w.lo, w.hi, word_addr(kBpScratch)).dma);
            }
            return open;
        };
        bool len4 = false, len6 = false;
        std::vector<int64_t> o4 = open_per_window(cap_lores4, len4);
        std::vector<int64_t> o6 = open_per_window((mv == "OPEN50") ? cap_lores4 : (mv == "OPEN50PRE") ? cap_base : cap_lores6, len6);
        bool pre = len4 && len6;
        checks.expect(pre,
            "OPEN50PRE [p196,p197]: every fetch window of the four- and six-plane runs spans the page's 160 time "
            "slots (" + std::to_string(o4.size()) + " and " + std::to_string(o6.size()) +
            " window(s)) [not graded -- apparatus check, no claim uid]");
        if (pre) {
            int64_t min4 = *std::min_element(o4.begin(), o4.end()), max4 = *std::max_element(o4.begin(), o4.end());
            int64_t min6 = *std::min_element(o6.begin(), o6.end()), max6 = *std::max_element(o6.begin(), o6.end());
            checks.note("OPEN50: open slots per 160-slot window -- four planes " + std::to_string(min4) + ".." + std::to_string(max4) +
                        ", six planes " + std::to_string(min6) + ".." + std::to_string(max6));
            checks.expect(min4 > 0 && min4 == max4 && min6 == max6 && min6 * 2 == min4,
                "OPEN50 [p196,p197] 6-system-dma.c65,ch6-sysdma.c38: six low-resolution bitplanes steal 50 percent "
                "of the open slots four bitplanes would have left -- in every fetch window the four-plane run leaves " +
                std::to_string(min4) + ".." + std::to_string(max4) + " slots open and the six-plane run " +
                std::to_string(min6) + ".." + std::to_string(max6) + " (" + std::to_string(o4.size()) + " and " +
                std::to_string(o6.size()) + " windows)");
        } else {
            not_graded(checks, "OPEN50", "OPEN50PRE");
        }
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
        Capture cl = run_capture(dir + (mv == "ORDER" ? "/prog_copyloop_short.bin" : "/prog_copyloop.bin"),
                                 kBootBudget + kDoneRunBudget, true, {});
        uint32_t n = kClN;
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

    // ---- TAS-SPAN [p196] 6-system-dma.c57, ch6-sysdma.c33: "the indivisible
    // read-modify-write cycle that is used only in this instruction will not
    // fit into a DMA memory access slot." An ordinary 68000 bus cycle occupies
    // one memory slot (Figure 6-10). On the RAM port a cycle is the run of
    // samples of one address and direction; its span is the cycle of its last
    // sample minus the cycle of its first. The TAS cycle is the read at the
    // TAS address and the write that follows it; it does not fit a slot if
    // its span from the read's first sample to the write exceeds one slot,
    // while every ordinary cycle at the same address (a plain write, a plain
    // read, a plain write, measured in the same run) spans at most one slot.
    // The run is prog_tas_span.s: no DMA channel is enabled, so no master
    // other than the 68000 and the refresh slots uses the bus.
    {
        std::string bin = (mv == "TAS-SPAN-PRE") ? "/prog_tas_span_mut.bin" : "/prog_tas_span.bin";
        Capture t = run_capture(dir + bin, kBootBudget + kDoneRunBudget, true, {});
        struct Cycle { bool wr; uint16_t data; uint64_t first, last; };
        std::vector<Cycle> cycles;
        for (const auto& a : t.log) {
            if (a.word_addr != word_addr(kTasAddr)) continue;
            if (!cycles.empty() && cycles.back().wr == a.is_write && cycles.back().data == a.data) { cycles.back().last = a.cycle; continue; }
            cycles.push_back({a.is_write, a.data, a.cycle, a.cycle});
        }
        bool shape = t.loaded && t.done.has_value() && cycles.size() == 5 &&
                     cycles[0].wr && !cycles[1].wr && cycles[2].wr && !cycles[3].wr && cycles[4].wr;
        checks.expect(shape,
            "TAS-SPAN-PRE [p196]: the TAS address shows the five cycles of the program -- write, read, write (the "
            "ordinary accesses), then the TAS read and write (" + std::to_string(cycles.size()) + " cycles) "
            "[not graded -- apparatus check, no claim uid]");
        if (shape && slot_ok) {
            uint64_t ordinary_max = 0;
            for (size_t k = 0; k < 3; ++k) ordinary_max = std::max(ordinary_max, cycles[k].last - cycles[k].first);
            uint64_t tas_span = cycles[4].first - cycles[3].first;
            checks.note("TAS-SPAN: slot unit " + std::to_string(kCck) + " clk_sys; the three ordinary cycles span at most " +
                        std::to_string(ordinary_max) + " clk_sys; the TAS read spans " + std::to_string(cycles[3].last - cycles[3].first) +
                        " clk_sys and its write follows the read's first sample after " + std::to_string(tas_span) + " clk_sys");
            checks.expect(tas_span > kCck && ordinary_max <= kCck,
                "TAS-SPAN [p196] 6-system-dma.c57,ch6-sysdma.c33: the indivisible read-modify-write cycle of TAS does not fit "
                "into one DMA memory access slot -- its write follows its read after " + std::to_string(tas_span) +
                " clk_sys, more than one slot (" + std::to_string(kCck) + " clk_sys), while the ordinary cycles of the same run span at most " +
                std::to_string(ordinary_max) + " clk_sys");
        } else {
            not_graded(checks, "TAS-SPAN", std::string(shape ? "" : "TAS-SPAN-PRE ") + (slot_ok ? "" : "SLOT-UNIT"));
        }
    }

    // ---- ch6-multi.c4 [p187] -- withdrawn. The claim is permissive ("may be
    // able to") and its consequence, a read served from the processor's
    // cache, makes no Chip-RAM access. `cpu_config` is the design's processor
    // type input (harness contract, Config::cpu_config: 00 selects 68000
    // behaviour, its other values the TG68K core's 68010/68020 behaviour), so
    // a 68020 statement has a subject on the TG68K build only. The scope
    // decision is a record with its source; the TG68K counts are the only
    // values the run computes.
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
            "MULTI-C4-WITHDRAWN [p187] ch6-multi.c4: not tested -- the claim is permissive (\"may be able to get "
            "at the data before the blitter has finished writing it\") and its consequence is a read served from "
            "a 68020's cache, which makes no Chip-RAM access; a test would need a program that reads the blit's "
            "destination while the blit is still writing it and stores what it read, a fixture this package "
            "does not build, so this is not tested, not a harness gap");
    } else {
        checks.record(
            "MULTI-C4-SCOPE-FX68K [p187] ch6-multi.c4: not applicable on fx68k -- source: the harness contract "
            "(Config::cpu_config in nanomig_harness.h) documents cpu_config as the processor-type input, 00 "
            "selecting 68000 behaviour and its other values the TG68K core's 68010/68020 behaviour, so a 68020 "
            "statement has a subject on the TG68K build only; a scope decision, not a measurement");
    }

    // ---- FASTRUN [p186] ch6-done.c9: "may be running completely
    // uninhibited out of Fast memory or its internal cache and continue
    // to have memory cycles." prog_fastrun.s enables the design's Zorro
    // II fast-RAM path with one register write at $E80048, relocates a
    // counting loop there, and starts a nasty blit through the Copper's
    // own list -- never through a CPU chip-bus write -- so the CPU's own
    // last chip-bus write before it jumps into fast RAM is unaffected by
    // the blit that begins slightly afterward (A4 shows 0 CPU counter
    // writes for the whole of a nasty blit's window otherwise). p.30's
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
        cfg_fr.fastram_config = 0x01; // 2MB Zorro II fast RAM -- never toggled;
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

    // ---- BBUSY-LOCKOUT [p186] 6-blitter-done-flag.c10, ch6-done.c12,
    // ch6-done.c10: "If a blit has just been started but has been locked out of
    // memory access because of, for instance, display fetches, this bit may not
    // yet be set. ... The solution is to read a chip memory or hardware register
    // address with the processor before testing the bit. ... The first "test" of
    // the blitter done bit may not return the correct result, but the second
    // will." The case is built as FASTRUN-LOCKOUT builds it (four high-
    // resolution bitplanes lock out the blitter and the 68000 alike, p.197; the
    // Copper starts the blit just before a fetch window), with the 68000 in
    // Fast memory reading DMACONR twice in a row, storing both values to Fast
    // memory (prog_bbusy_lockout.s). The 68000 does not need a Chip bus cycle
    // for anything but the two register reads, so it is the reads that are held
    // across the window: the iteration whose previous pair was stored before the
    // window began and whose own first store comes after it was pending on a
    // register read throughout the window (nothing else in the loop waits), so
    // its second read completed after the window and after the blit's start,
    // and before the blit's last write. The blitter's first access also comes
    // after the window, so the blit was started and locked out. The second
    // test of that iteration must show the blitter busy; the value of the first
    // test is a measurement (the page says it may not be correct).
    {
        std::string bin = "/prog_bbusy_lockout.bin";
        if (mv == "BBUSY-LOCKOUT-PRE") bin = "/prog_bbusy_lockout_2plane.bin";
        else if (mv == "BBUSY-LOCKOUT") bin = "/prog_bbusy_lockout_intena.bin";
        nmh::Config cfg_bb;
        cfg_bb.fastram_config = 0x01;
        FastCapture cap = run_capture_fastram(dir + bin, kDoneRunBudget, {}, cfg_bb);

        auto cop_fetch = read_cycles_in_range(cap.log, word_addr(kFrCopDataWord), word_addr(kFrCopDataWord) + 1);
        auto blit_reads = read_cycles_in_range(cap.log, word_addr(kFrLockoutSrcLo), word_addr(kFrLockoutSrcHi));
        auto blit_writes = write_cycles_in_range(cap.log, word_addr(kShareDstLo), word_addr(kShareDstHi));
        std::vector<uint64_t> blit_all = blit_reads;
        blit_all.insert(blit_all.end(), blit_writes.begin(), blit_writes.end());
        uint64_t cop_bound = cop_fetch.empty() ? 0 : cop_fetch.front();
        uint64_t blit_first = blit_all.empty() ? 0 : *std::min_element(blit_all.begin(), blit_all.end());
        uint64_t blit_last = blit_all.empty() ? 0 : *std::max_element(blit_all.begin(), blit_all.end());
        auto wins = fetch_windows(read_cycles_in_range(cap.log, word_addr(kBpPtrLo), word_addr(kBpPtrHi)));
        const FetchWindow* w = nullptr;
        for (auto& fw : wins) if (fw.lo >= cop_bound) { w = &fw; break; }
        bool locked = slot_ok && !cop_fetch.empty() && !blit_all.empty() && w != nullptr && w->hi <= blit_first;

        // The stored pairs: two consecutive Fast-memory words per iteration, the
        // first read's value then the second's.
        std::map<uint32_t, std::pair<uint64_t, uint16_t>> stored;   // word address -> (cycle, value)
        for (const auto& a : cap.fastlog)
            if (a.is_write && a.word_addr >= word_addr(kFrCnt) && a.word_addr < word_addr(kFrCnt) + 4096 && !stored.count(a.word_addr))
                stored[a.word_addr] = {a.cycle, a.data};
        size_t pending = 0; bool found = false;
        uint64_t t1 = 0; uint16_t first_v = 0, second_v = 0;
        if (w != nullptr) {
            for (size_t i = 1; i * 2 + 1 < 4096 && !found; ++i) {
                auto cur1 = stored.find(word_addr(kFrCnt) + (uint32_t)(2 * i));
                auto prev2 = stored.find(word_addr(kFrCnt) + (uint32_t)(2 * i - 1));
                auto cur2 = stored.find(word_addr(kFrCnt) + (uint32_t)(2 * i + 1));
                if (cur1 == stored.end() || prev2 == stored.end() || cur2 == stored.end()) continue;
                if (prev2->second.first < w->lo && cur1->second.first > w->hi) {
                    pending = i; found = true;
                    t1 = cur1->second.first; first_v = cur1->second.second; second_v = cur2->second.second;
                }
            }
        }
        bool pre = locked && found && cap.done.has_value() && t1 < blit_last;
        checks.note("BBUSY-LOCKOUT: Copper data-word fetch at cycle " + std::to_string(cop_bound) + "; fetch window " +
                    (w ? std::to_string(w->lo) + ".." + std::to_string(w->hi) : std::string("none")) +
                    "; blitter's first access at " + std::to_string(blit_first) + ", last at " + std::to_string(blit_last) +
                    "; pair pending across the window: " + (found ? "iteration " + std::to_string(pending) + ", first store at cycle " +
                    std::to_string(t1) : std::string("none")));
        if (found) {
            char fv[8], sv[8];
            std::snprintf(fv, sizeof fv, "%04X", (unsigned)first_v);
            std::snprintf(sv, sizeof sv, "%04X", (unsigned)second_v);
            checks.note("BBUSY-LOCKOUT: first test read " + std::string(fv) + " (BBUSY " + ((first_v & 0x4000) ? "set" : "clear") +
                        "), second test read " + std::string(sv) + " (BBUSY " + ((second_v & 0x4000) ? "set" : "clear") + ")");
        }
        checks.expect(pre,
            "BBUSY-LOCKOUT-PRE [p186,p197]: the blit was started and locked out -- one four-high-resolution-bitplane "
            "fetch window lies between the Copper's fetch of the BLTSIZE value and the blitter's first access, and a "
            "register-read pair of the 68000 was pending across it and completed before the blit's last write "
            "[not graded -- apparatus check, no claim uid]");
        if (pre) {
            std::string second_state = (second_v & 0x4000) ? "set" : "clear";
            checks.expect((second_v & 0x4000) != 0,
                "BBUSY-LOCKOUT [p186] 6-blitter-done-flag.c10,ch6-done.c12,ch6-done.c10: with a blit started and locked out "
                "of memory access by display fetches, the second test of the blitter done bit, after the prescribed "
                "intervening hardware-register read, returns the correct result -- DMACONR bit 14 reads set while the "
                "blit is in progress (second read " + second_state + ")");
        } else {
            not_graded(checks, "BBUSY-LOCKOUT", "BBUSY-LOCKOUT-PRE");
        }
    }

    // ---- claims this package does not assert: one record per row of
    // SPECIFICATION.md's "Claims not asserted" table, so that each id is
    // printed on a RECORDED line.
    checks.record(
        "ORDER-FACTOR-NOT-ASSERTED [p169] 6-intro.c5,ch6-intro.c5: not asserted -- the factor (\"approximately "
        "twice as fast\") has no numeric tolerance on the page; the ordering is graded by ORDER, which carries "
        "no claim id");
    checks.record(
        "DONE-FLAG-MAY-NOT-ASSERTED [p186] 6-blitter-done-flag.c9,ch6-done.c11,ch6-done.c8: not asserted -- the "
        "claims are permissive (\"may not return the correct result\", \"may not yet be set\"), so no "
        "simulation outcome contradicts them; BBUSY-LOCKOUT grades the mechanism the page gives for the second "
        "test, and the first test's value is printed as a measurement");
    checks.record(
        "PRIORITY-SCOPE-NOT-ASSERTED [p194] 6-system-dma.c12,ch6-sysdma.c10: not asserted -- \"the two lowest "
        "priority levels\" ranks the blitter and the 68000 against every other DMA channel, and confirming it "
        "needs all of them driven at once, a fixture beyond this package; the narrower fact that the blitter "
        "outranks the 68000 is recorded as A4-SHARE-WITHDRAWN");
    checks.record(
        "REQUEST-ONLY-IF-NOT-ASSERTED [p196] ch6-sysdma.c35: not asserted -- the \"only if\" direction (never "
        "granted when it has not asked or is outranked) needs the 68000's request visible, which the RAM port "
        "does not show; A9 grades the \"if\" direction");
    checks.record(
        "HALF-INSTRUCTION-TIME-NOT-ASSERTED [p196] 6-system-dma.c51,6-system-dma.c52,ch6-sysdma.c26,ch6-sysdma.c27: "
        "not asserted -- \"about half\" of instruction time is an average over the 68000's full instruction mix "
        "with no numeric tolerance on the page");
    checks.record(
        "TAS-ADVICE-NOT-ASSERTED [p196] ch6-sysdma.c32: not asserted -- advice to a programmer, not a claim "
        "observable at the RAM port");
    checks.record(
        "MOST-INSTRUCTIONS-NOT-ASSERTED [p196] ch6-sysdma.c31: not asserted -- \"most instructions do not cause "
        "cycles to be missed\" is a statistic over the instruction set with no numeric tolerance on the page");

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
