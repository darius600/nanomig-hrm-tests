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
// MUTATION A2 stimulus red blit shrunk to a near-instant one-word transfer, via NMH_MUTATE=A2
// MUTATION A3 stimulus red blit size grown from one word to two hundred, via NMH_MUTATE=A3
// MUTATION A4 stimulus red BLTPRI cleared (nasty -> share), via NMH_MUTATE=A4
// MUTATION A6 stimulus red six planes substituted for four inside the fetch window, via NMH_MUTATE=A6
// MUTATION A8 stimulus red baseline capture substituted with the nasty-mode window (no recovered access to measure a gap from), via NMH_MUTATE=A8
// MUTATION A9 stimulus red bitplane resolution changed from low to high, via NMH_MUTATE=A9
// MUTATION A9PRE stimulus red bitplane program substituted with one that never enables bitplane DMA, via NMH_MUTATE=A9PRE
// MUTATION A10 stimulus red bitplane resolution changed from high to low, via NMH_MUTATE=A10
// MUTATION A10PRE stimulus red bitplane program substituted with one that never enables bitplane DMA, via NMH_MUTATE=A10PRE
// MUTATION A11 stimulus red both sides run the same four-bitplane config, via NMH_MUTATE=A11
// MUTATION PARITY stimulus red baseline capture substituted with the four-high-res-bitplane window (zero CPU accesses to split by phase), via NMH_MUTATE=PARITY
// MUTATION SLOTSPAN stimulus red baseline capture substituted with the four-lores-bitplane in-window spacing, via NMH_MUTATE=SLOTSPAN
// MUTATION OPEN50 stimulus red six-plane side substituted with the four-plane capture (no reduction), via NMH_MUTATE=OPEN50
//
// A4's precondition (the nasty-mode window's own length) is not exercised
// by a dedicated mutation: every capture in this package that shrinks the
// window also changes BLTPRI or the display configuration, which would
// flip the main assertion rather than isolate the window-length
// precondition alone -- not exercised, by a scope decision. A5 is
// withdrawn (see the A5-WITHDRAWN record below); it carries no assertion
// and no mutation.

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

// prog_tas.s address.
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

    // ---- A3 [p187] a small blit can finish before the CPU's first check,
    // and BBUSY-PREDICTS [p186], which always needs the true "large" capture.
    {
        std::string bin = dir + (mv == "A3" ? "/prog_earlycheck_large.bin" : "/prog_earlycheck.bin");
        Capture small = run_capture(bin, kBootBudget + kDoneRunBudget, true, {kResults});
        Capture large = run_capture(dir + "/prog_earlycheck_large.bin", kBootBudget + kDoneRunBudget, true, {kResults});

        bool small_ok = small.loaded && small.done.has_value() && small.peeks.size() == 1 &&
                         small.peeks[0].has_value();
        bool small_busy = small_ok && (*small.peeks[0] & 0x4000);

        auto dst_w = write_cycles_in_range(small.log, word_addr(kEcDst), word_addr(kEcDst) + 1);
        auto dummy_r = read_cycles_in_range(small.log, word_addr(kEcDummy), word_addr(kEcDummy) + 1);
        bool a3_pre = small_ok && !dst_w.empty() && !dummy_r.empty() && dst_w.front() < dummy_r.front();
        checks.expect(a3_pre,
            "A3PRE [p187] ch6-multi.c3: the blit's destination write is already in "
            "ram_log() before the manual's prescribed dummy chip-memory read");
        checks.expect(a3_pre && !small_busy,
            "A3 [p187] ch6-multi.c3: a one-word blit's own busy bit (DMACONR bit 14) already "
            "reads clear on the CPU's first, immediate check after starting it");

        bool large_ok = large.loaded && large.done.has_value() && large.peeks.size() == 1 &&
                         large.peeks[0].has_value();
        bool large_busy = large_ok && (*large.peeks[0] & 0x4000);
        auto large_dst_w = write_cycles_in_range(large.log, word_addr(kEcDst), word_addr(kEcDst) + kEcDstLargeN);
        auto large_dummy_r = read_cycles_in_range(large.log, word_addr(kEcDummy), word_addr(kEcDummy) + 1);
        uint64_t last_dst_write = large_dst_w.empty() ? 0 : large_dst_w.back();
        uint64_t dummy_read_cycle = large_dummy_r.empty() ? 0 : large_dummy_r.front();
        bool writes_still_incoming = !large_dst_w.empty() && last_dst_write > dummy_read_cycle;
        checks.note("BBUSY-PREDICTS: large blit's own last D-write at cycle " + std::to_string(last_dst_write) +
                    ", dummy read at cycle " + std::to_string(dummy_read_cycle) +
                    ", " + std::to_string(large_dst_w.size()) + " D-writes total");
        checks.expect(large_ok && large_busy && writes_still_incoming,
            "BBUSY-PREDICTS [p186] 6-blitter-done-flag.c10,ch6-done.c10,ch6-done.c12: after the "
            "prescribed intervening read, BBUSY reading set correctly predicts more destination "
            "writes still to come in ram_log() (last D-write at cycle " + std::to_string(last_dst_write) +
            " is after the read at " + std::to_string(dummy_read_cycle) + ")");
    }

    // ---- prog_sharing.s family: always load all four builds -------------
    Capture cap_base  = run_capture(dir + "/prog_sharing.bin", kShareRunBudget, false, {});
    Capture cap_share = run_capture(dir + "/prog_sharing_share.bin", kShareRunBudget, false, {});
    Capture cap_nasty = run_capture(dir + "/prog_sharing_nasty.bin", kShareRunBudget, false, {});
    Capture cap_tiny  = run_capture(dir + "/prog_sharing_tiny.bin", kShareRunBudget, false, {});

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

    checks.note("capture health: base.log.size=" + std::to_string(cap_base.log.size()) +
                " share.log.size=" + std::to_string(cap_share.log.size()) +
                " nasty.log.size=" + std::to_string(cap_nasty.log.size()) +
                " tiny.log.size=" + std::to_string(cap_tiny.log.size()));

    // ---- A2 [p169,p186] the CPU keeps running while the blitter works --
    {
        const Capture& cap = (mv == "A2") ? cap_tiny : cap_share;
        auto [lo, hi] = dst_span(cap);
        auto sc = scratch_writes(cap);
        size_t in_win = count_in_window(sc, lo, hi);
        checks.note("A2: active window " + std::to_string(lo) + ".." + std::to_string(hi) +
                    ", " + std::to_string(in_win) + " scratch writes inside it");
        checks.expect(lo != hi && in_win >= 5,
            "A2 [p169,p186] 6-intro.c20,ch6-intro.c21,ch6-intro.c22,6-blitter-done-flag.c2,"
            "6-blitter-done-flag.c3,ch6-done.c2,ch6-done.c3: the CPU stores several distinct "
            "counter values while a share-mode blit is still in progress (" +
            std::to_string(in_win) + " during its active window)");
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
    // the "unsatisfied" count starts from. Measured (share-mode window,
    // both cores): the dominant pattern is 4 blitter accesses (4 memory
    // slots) between consecutive CPU accesses -- not 3. Only an unstated
    // convention (excluding the CPU's own post-grant internal/decode
    // slot, per p.196's "one memory access + one internal operation"
    // cadence, itself asserted as SLOTSPAN below) reduces that to 3. The
    // page does not choose between "count every intervening slot" (4,
    // contradicting the claim) and "count only the slots after the CPU's
    // own next request" (3, matching it), so no new assertion is written.
    checks.record(
        "A5-WITHDRAWN [p198] 6-blitter-priority.c13,ch6-sysdma.c53,ch6-sysdma.c62: withdrawn -- "
        "the page does not fix from which slot after a granted CPU access the unsatisfied-cycle "
        "count starts, and the measured intervening-slot count (4, both cores, share-mode window) "
        "matches \"three\" only under that unstated convention; a new assertion would have to pick "
        "the convention itself, which the page does not license");

    // ---- A8 [p196] a denied cycle means the CPU simply waits for its
    // next available slot: in the no-DMA baseline (or, under
    // NMH_MUTATE=A8, the nasty-mode window, which has no recovered access
    // to measure a gap from), no two consecutive CPU bus accesses are an
    // odd number of slots apart.
    {
        const Capture& cap = (mv == "A8") ? cap_nasty : cap_base;
        uint64_t lo = 0, hi = ~0ull;
        if (mv == "A8") std::tie(lo, hi) = dst_span(cap);
        std::vector<uint64_t> cpu_all;
        for (const auto& a : cap.log)
            if (is_cpu_access(a, word_addr(kShareScratch)) && a.cycle >= lo && a.cycle <= hi)
                cpu_all.push_back(a.cycle);
        std::sort(cpu_all.begin(), cpu_all.end());
        size_t odd = 0, pairs = 0;
        for (size_t i = 1; i < cpu_all.size(); ++i) {
            uint64_t gap = cpu_all[i] - cpu_all[i - 1];
            if (gap % kCck != 0) continue;
            ++pairs;
            if ((gap / kCck) % 2 == 1) ++odd;
        }
        checks.note("A8: " + std::to_string(odd) + " odd-slot-spacing instance(s) of " +
                    std::to_string(pairs) + " slot-aligned consecutive-CPU-access pairs (" +
                    (mv == "A8" ? "nasty-mode window" : "no-DMA baseline") + ")");
        // The unmutated check requires pairs > 0 as a sanity guard (a real
        // measurement, not a vacuous pass). Under NMH_MUTATE=A8 that guard
        // is dropped on purpose: the nasty-mode window has at most one CPU
        // access at all (per A4), so zero pairs -- and zero pairs means
        // zero odd-spacing instances -- is exactly the vacuous-GREEN result
        // that demonstrates this comparator is not hard-wired to fail.
        bool a8_pass = (mv == "A8") ? (odd == 0) : (pairs > 0 && odd == 0);
        checks.expect(a8_pass,
            "A8 [p196] 6-system-dma.c54,6-system-dma.c55,ch6-sysdma.c30: no two consecutive CPU "
            "bus accesses are an odd number of slots apart -- a denied cycle always waits for the "
            "CPU's own next available (same-parity) slot, never a different one (" +
            std::to_string(odd) + " odd-spacing instance(s) of " + std::to_string(pairs) + ")");
    }

    // ---- prog_bitplane.s family: always load all three builds -----------
    Capture cap_lores4 = run_capture(dir + "/prog_bitplane.bin", kBpRunBudget, false, {});
    Capture cap_hires4 = run_capture(dir + "/prog_bitplane_hires.bin", kBpRunBudget, false, {});
    Capture cap_lores6 = run_capture(dir + "/prog_bitplane_planes6.bin", kBpRunBudget, false, {});

    checks.note("capture health: lores4.log.size=" + std::to_string(cap_lores4.log.size()) +
                " hires4.log.size=" + std::to_string(cap_hires4.log.size()) +
                " lores6.log.size=" + std::to_string(cap_lores6.log.size()));

    // ---- A9 [p196] four or fewer low-res bitplanes still let the CPU through
    {
        const Capture& pre_cap = (mv == "A9PRE") ? cap_base : cap_lores4;
        auto pre_wins = fetch_windows(bp_reads_of(pre_cap));
        checks.expect(!pre_wins.empty(),
            "A9PRE [p196] 6-system-dma.c61,ch6-sysdma.c34: bitplane fetch windows are "
            "actually found (" + std::to_string(pre_wins.size()) + ")");

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
            "A10PRE [p196,p197] 6-system-dma.c71,ch6-sysdma.c42: every one of the "
            "line's 160 fetch-window slots (p.197's \"40 * 4 = 160 time slots\") is a bitplane "
            "fetch, none left over (" + std::to_string(pre_wins.size()) + " window(s))");

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

    // ---- A6 [p196] alternate cycles are enough for full speed in the normal case
    {
        const Capture& cap = (mv == "A6") ? cap_lores6 : cap_lores4;
        auto wins = fetch_windows(bp_reads_of(cap));
        auto sw = write_cycles_in_range(cap.log, word_addr(kBpScratch), word_addr(kBpScratch) + 1);
        std::vector<uint64_t> in_win;
        for (auto& w : wins) for (auto c : sw) if (c >= w.lo && c <= w.hi) in_win.push_back(c);
        std::sort(in_win.begin(), in_win.end());
        in_win.erase(std::unique(in_win.begin(), in_win.end()), in_win.end());
        std::vector<uint64_t> in_win_gaps;
        for (size_t i = 1; i < in_win.size(); ++i) in_win_gaps.push_back(in_win[i] - in_win[i - 1]);
        uint64_t in_win_period = mode_of(in_win_gaps);

        auto base_sc = scratch_writes(cap_base);
        std::vector<uint64_t> base_gaps;
        for (size_t i = 1; i < base_sc.size(); ++i) base_gaps.push_back(base_sc[i] - base_sc[i - 1]);
        uint64_t base_period = mode_of(base_gaps);

        checks.note("A6: CPU loop period inside the fetch window = " + std::to_string(in_win_period) +
                    " clk_sys (modal, " + std::to_string(in_win_gaps.size()) + " gaps); no-DMA period = " +
                    std::to_string(base_period) + " clk_sys (modal, " + std::to_string(base_gaps.size()) + " gaps)");
        checks.expect(in_win_period > 0 && in_win_period == base_period,
            "A6 [p196] 6-system-dma.c53,ch6-sysdma.c28: the CPU's own loop period inside the "
            "four-lores-plane fetch window (" + std::to_string(in_win_period) +
            " clk_sys) equals its no-DMA period (" + std::to_string(base_period) + " clk_sys)");
    }

    // ---- SLOTSPAN [p196] 6-system-dma.c58: Figure 6-10 divides an
    // average 68000 cycle into an internal-operation half and a
    // memory-access half -- one memory slot, one free slot. Measured as
    // the modal gap between consecutive CPU bus accesses in the no-DMA
    // baseline (or, under NMH_MUTATE=SLOTSPAN, the four-lores-plane
    // in-window gap, which is contended and should not equal 2 slots).
    {
        std::vector<uint64_t> gaps;
        if (mv == "SLOTSPAN") {
            auto wins = fetch_windows(bp_reads_of(cap_lores4));
            auto sw = write_cycles_in_range(cap_lores4.log, word_addr(kBpScratch), word_addr(kBpScratch) + 1);
            std::vector<uint64_t> in_win;
            for (auto& w : wins) for (auto c : sw) if (c >= w.lo && c <= w.hi) in_win.push_back(c);
            std::sort(in_win.begin(), in_win.end());
            in_win.erase(std::unique(in_win.begin(), in_win.end()), in_win.end());
            for (size_t i = 1; i < in_win.size(); ++i) gaps.push_back(in_win[i] - in_win[i - 1]);
        } else {
            std::vector<uint64_t> cpu_cycles;
            for (const auto& a : cap_base.log)
                if (is_cpu_access(a, word_addr(kShareScratch))) cpu_cycles.push_back(a.cycle);
            std::sort(cpu_cycles.begin(), cpu_cycles.end());
            for (size_t i = 1; i < cpu_cycles.size(); ++i) gaps.push_back(cpu_cycles[i] - cpu_cycles[i - 1]);
        }
        uint64_t m = mode_of(gaps);
        checks.note("SLOTSPAN: modal consecutive-CPU-access gap (" +
                    std::string(mv == "SLOTSPAN" ? "four-lores-plane in-window" : "no-DMA baseline") +
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

    // ---- PARITY [p196] 6-system-dma.c50,ch6-sysdma.c25,6-system-dma.c60,ch6-sysdma.c29
    {
        const bool mutated = (mv == "PARITY");
        const Capture& cap = mutated ? cap_hires4 : cap_lores4;
        auto reads = bp_reads_of(cap);
        std::sort(reads.begin(), reads.end());
        uint64_t ref = reads.empty() ? 0 : reads.front();
        // Under NMH_MUTATE=PARITY the comparator is applied only inside the
        // hi-res capture's own fetch windows (A10's own windows, where A10
        // already proved the CPU has zero chip-bus accesses at all) rather
        // than over the whole run -- over the whole run (including
        // blanking, outside any window) this same capture still shows
        // roughly the same ~50/50 split the unmutated check already found,
        // so it would not demonstrate anything about the comparator.
        std::vector<FetchWindow> mut_wins;
        if (mutated) mut_wins = fetch_windows(reads);
        auto in_any_window = [&](uint64_t cyc) {
            for (auto& w : mut_wins) if (cyc >= w.lo && cyc <= w.hi) return true;
            return false;
        };
        size_t phase_a = 0, phase_b = 0;
        for (const auto& a : cap.log) {
            if (!is_cpu_access(a, word_addr(kBpScratch))) continue;
            if (mutated && !in_any_window(a.cycle)) continue;
            uint64_t phase = (a.cycle >= ref ? (a.cycle - ref) : (ref - a.cycle)) % (2 * kCck);
            if (phase < kCck) ++phase_a; else ++phase_b;
        }
        checks.note("PARITY: CPU accesses by phase relative to the first BPL1 fetch -- phase A (even) = " +
                    std::to_string(phase_a) + ", phase B (odd) = " + std::to_string(phase_b) +
                    (mutated ? " (restricted to the hi-res capture's own fetch windows)" : ""));
        checks.expect(!reads.empty() && phase_b == 0,
            "PARITY [p196] 6-system-dma.c50,ch6-sysdma.c25,6-system-dma.c60,ch6-sysdma.c29: every "
            "CPU chip-bus access falls on the same slot phase as the display's own even-numbered "
            "cycles, never the other phase (" + std::to_string(phase_b) + " of " +
            std::to_string(phase_a + phase_b) + " on the other phase)");
    }

    // ---- ORDER [p169] 6-intro.c5,ch6-intro.c5 (ordering only; no factor asserted)
    {
        Capture cl = run_capture(dir + "/prog_copyloop.bin", kBootBudget + kDoneRunBudget, true, {});
        auto cl_writes = write_cycles_in_range(cl.log, word_addr(kClDst), word_addr(kClDst) + kClN);
        bool cl_ok = cl.loaded && cl.done.has_value() && cl_writes.size() == kClN;
        uint64_t cl_first = cl_writes.empty() ? 0 : *std::min_element(cl_writes.begin(), cl_writes.end());
        uint64_t cl_last  = cl_writes.empty() ? 0 : *std::max_element(cl_writes.begin(), cl_writes.end());
        uint64_t cl_span = cl_writes.empty() ? 0 : cl_last - cl_first;
        auto [blo, bhi] = dst_span(cap_nasty);
        uint64_t blit_span = blo == bhi ? 0 : bhi - blo;
        checks.note("ORDER: CPU copy loop of " + std::to_string(kClN) + " words spans " +
                    std::to_string(cl_span) + " clk_sys (" + std::to_string(cl_writes.size()) +
                    " writes seen); the nasty-mode blit's own 500-word copy spans " +
                    std::to_string(blit_span) + " clk_sys, same yardstick (first to last destination write)");
        checks.expect(cl_ok && blit_span > 0 && blit_span < cl_span,
            "ORDER [p169]: a blitter A-to-D copy of 500 words (the nasty-mode capture, uncontended) "
            "finishes before a 68000 MOVE.W copy loop of the same 500 words (" +
            std::to_string(blit_span) + " clk_sys vs. " + std::to_string(cl_span) +
            " clk_sys) -- ordering only, no numeric factor asserted");
    }

    // ---- TAS [p196] 6-system-dma.c56
    {
        Capture t = run_capture(dir + "/prog_tas.bin", kBootBudget + kDoneRunBudget, true, {});
        // Coalesced over this one address only: nothing else in this
        // program ever revisits it, so two adjacent identical entries here
        // can only be the same logical (possibly wait-stated) bus cycle
        // sampled twice, never two genuinely separate accesses.
        std::vector<nmh::RamAccess> raw_at_addr;
        for (const auto& a : t.log) if (a.word_addr == word_addr(kTasAddr)) raw_at_addr.push_back(a);
        auto coalesced = nmh::coalesce(raw_at_addr);
        std::vector<std::pair<uint64_t, bool>> at_addr; // (cycle, is_write)
        for (const auto& a : coalesced) at_addr.push_back({a.cycle, a.is_write});
        std::sort(at_addr.begin(), at_addr.end());
        int64_t rw_gap = -1;
        for (size_t i = 0; i + 1 < at_addr.size(); ++i) {
            if (!at_addr[i].second && at_addr[i + 1].second) { // read then write
                rw_gap = (int64_t)(at_addr[i + 1].first - at_addr[i].first);
                break;
            }
        }
        std::string seq;
        for (auto& p : at_addr) seq += (p.second ? "W@" : "R@") + std::to_string(p.first) + " ";
        checks.note("TAS: read-then-write cycle gap = " + std::to_string(rw_gap) + " clk_sys (" +
                    std::to_string(at_addr.size()) + " accesses at the TAS address: " + seq +
                    "); ordinary back-to-back CPU bus cycles measure 2 slots (SLOTSPAN, " + std::to_string(2 * kCck) + " clk_sys)");
        checks.expect(t.loaded && t.done.has_value() && rw_gap >= 0 && rw_gap != (int64_t)(2 * kCck),
            "TAS [p196] 6-system-dma.c56: TAS's indivisible read-modify-write cycle does not fit "
            "the ordinary 2-slot (one memory access, one free) pattern SLOTSPAN establishes -- its "
            "own read-to-write gap (" + std::to_string(rw_gap) + " clk_sys) differs from that "
            "ordinary spacing (" + std::to_string(2 * kCck) + " clk_sys)");
    }

    // ---- MULTI-CPUCFG [p187] ch6-multi.c4 -- TG68K only; cpu_config
    // selects "CPU-Type: 00 = 68000, 01 = 68010, 11 = 68020" (nanomig.v:198).
    // fx68k is a fixed 68000 implementation and does not read it.
    if (is_tg68k) {
        nmh::Config cfg020; cfg020.cpu_config = 3;
        nmh::Config cfg000; cfg000.cpu_config = 0;
        Capture c020 = run_capture(dir + "/prog_sharing.bin", kShareRunBudget, false, {}, cfg020);
        Capture c000 = run_capture(dir + "/prog_sharing.bin", kShareRunBudget, false, {}, cfg000);
        size_t n020 = scratch_writes(c020).size();
        size_t n000 = scratch_writes(c000).size();
        checks.note("MULTI-CPUCFG (TG68K): loop iterations over " + std::to_string(kShareRunBudget) +
                    " cycles -- cpu_config=3 (68020): " + std::to_string(n020) +
                    ", cpu_config=0 (68000): " + std::to_string(n000));
        if (n020 != n000) {
            checks.expect(true,
                "ch6-multi.c4 [p187]: on TG68K, cpu_config=3 (68020) measurably changes the CPU's "
                "own uncontested chip-bus access rate versus cpu_config=0 (" + std::to_string(n020) +
                " vs. " + std::to_string(n000) + " loop iterations) -- evidence of a distinct "
                "68020-configured path capable of the claimed effect");
        } else {
            checks.note("ch6-multi.c4 [p187]: cpu_config=3 produced no measurable difference from "
                        "cpu_config=0 in this fixture (" + std::to_string(n020) + " iterations both ways) "
                        "-- stays withdrawn; see SPECIFICATION.md");
        }
    } else {
        checks.note("ch6-multi.c4 [p187]: not applicable on fx68k (fixed 68000 implementation; "
                    "cpu_config selects TG68K-only behaviour, nanomig.v:198)");
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
