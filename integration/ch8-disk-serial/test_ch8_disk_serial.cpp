// test_ch8_disk_serial.cpp -- ch8-disk-serial. See SPECIFICATION.md for
// every assertion's proposition, program, observed value and expected
// value. This file only implements what SPECIFICATION.md already
// committed to.
//
// NMH_MUTATE selects a mutated binary in place of its normal counterpart for
// every capture built from it, one substitution at a time; none is
// exercised by a plain `make test`. A single NMH_MUTATE value can flip
// several labels at once where they share a binary or a capture (P1, P2 and
// P3 all grade the same N=371 NTSC run; 1c/1d/1e and 2c/2d/2e share the
// N=371 binary across both standards) -- that is the shared stimulus, not a
// separate mutation each. Every mutation below feeds its contrasting
// capture into the package's own, unchanged comparator, so the same label
// that grades the normal run is what prints FAIL under the mutation -- no
// selector adds a second, inverted line of its own.
//
// 1a/2a, 1b/2b, 4a/4b (all four P4 tags) and 5a's own R1-anchor and
// R2-standard checks carry no mutation: no stimulus on the pages moves the
// refresh slots or the field-line count, so this boundary offers no
// contrasting capture for them (unfalsifiable, not exercised).
//
// Every record below states exactly which lines went from PASS to FAIL,
// measured on this package by direct replay of the built Vnanomig binaries
// (obj_dir_fx68k, obj_dir_tg68k) under each NMH_MUTATE value, both cores.
// The three baseline reds (P1, P2, P3) stay red, unaffected, throughout.
//
// MUTATION P1-NPLUS1 stimulus red NMH_MUTATE=P1-NPLUS1: SERPER written with N+1 (1, 372, 32397) and graded against N -- both cores (69 passed, 8 failed): flips "P1 control (green today) [p255]: N=0 (W=1) alone, same grid check", "P1 control (green today) [p255]: N=32396 (W=1) alone, same grid check", "P1 control (green today) [p255]: the within-word intervals ...", "P2(ii) control (green today) [p255]: the (N=0, N=32396) pair alone ...", and "P2 [p255]: on PAL, the fixed period equals one PAL colour clock ..." (its own U/C=1 check, via pal_n0/pal_n32396's own +1 binaries)
// MUTATION 1c/2c stimulus red NMH_MUTATE=P1-1C: ADKCON UARTBRK forced low then high between the lead-in and MARK=0 -- both cores (71 passed, 6 failed): flips "1c [p257]: TxD does not change ...", "2c [p257]: TxD idle before MARK=0, PAL N=371 run" and "P1 control (green today) [p255]: the within-word intervals ..." (the two extra TxD changes move t0). The R1 anchor and population lines 1a and 2a stay green on both cores: the population counts only refresh groups that lie wholly inside the capture
// MUTATION 1d/2d stimulus red NMH_MUTATE=P1-1D: next word polled on TSRE instead of TBE before being written -- both cores (72 passed, 5 failed): flips "1d [p257,p283]: for k=2..4, MARK=k comes before word (k-1)'s own stop-bit change ..." and "2d [p257,p283]: words sent back to back, PAL N=371 run"
// MUTATION 1e/2e stimulus red NMH_MUTATE=P1-1E: word 2's SERDAT write left out while still polling for its transfer -- both cores (69 passed, 8 failed): flips "1d [p257,p283]: ...", "1e [p283]: MARK=DONE is present and no poll ... ended at its own bound in the N=371 NTSC run", "2d [p257,p283]: ...", "2e [p283]: run completed, no poll ... PAL N=371 run" and "P1 control (green today) [p255]: the within-word intervals ..."
// MUTATION 4c stimulus red NMH_MUTATE=P4-4C: FAST=1 run writes ADKCON <- $0100 instead of $8100 (bit 15 clear clears the bit) -- both cores (72 passed, 5 failed): flips "P4-NTSC-FAST1 4c [p249]: ADKCONR bit 8 read back as 1 ..." and "P4-PAL-FAST1 4c [p249]: ADKCONR bit 8 read back as 1 ..."
// MUTATION 4d stimulus red NMH_MUTATE=P4-4D: once the FAST=1 poll count first drops to or below a tuned threshold, DSKEN (DMACON bit 4) is cleared and DSKBYTR is sampled into PROBE_D while cleared, then DMAEN+DSKEN are set again -- both cores (72 passed, 5 failed): flips "P4-NTSC-FAST1 4d [p248]: every PROBE_D value between t_64 and t_192 has DMAON and DISKWRITE set ..." and "P4-PAL-FAST1 4d [p248]: ..." (199 samples on fx68k, 223/224 on TG68K; only the sample count differs, not which lines flip); the same transfer resumes and 4a, 4b, 4c, 4e and completion all stay green
// MUTATION 4e stimulus red NMH_MUTATE=P4-4E: LENGTH=128 instead of 256 -- both cores (70 passed, 7 failed): flips "P4-NTSC-FAST1 4d [p248]: ..." (0 samples: t_192 is never read) and "P4-NTSC-FAST1 4e [p247]: reads of B+2i are seen for i=0..192 ..." and their PAL-FAST1 counterparts
// P4-MAIN (the T_cell-vs-band comparison) is a RECORDED decline (SPECIFICATION.md 1.4, P4): no page ties the
//   Chip-memory read rate it measures to the bit-cell time, so it is not a graded proposition and carries no
//   mutation of its own. |(t128-t64)-(t192-t128)| is measured, not graded, for the same reason (no page
//   sentence bounds it) and likewise carries no mutation.
// MUTATION 5b stimulus red NMH_MUTATE=P5-5B: only the ADKCON write that sets UARTBRK is left out; MARK=BRK is still written, so the run still shows 12 markers and the level between MARK=BRK and MARK=UNBRK reads 1, like the level after MARK=UNBRK -- both cores (73 passed, 4 failed): flips "5b [p257]: TxD is 0 between MARK=BRK and MARK=UNBRK ..." through that level comparison
// MUTATION 5c stimulus red NMH_MUTATE=P5-5C: character 0 read and cleared only after s_2, so OVRUN sets -- both cores (70 passed, 7 failed): flips "5a [p283]: MARK=DONE is present ..., run A", "5c [p256]: OVRUN (bit 15) is 0 ..., run A", "5d [p256]: MARK=READY precedes s_0 ..., run A" and "(a)+(c) [p255,p256,p257,p283]: ..."
// MUTATION 5d stimulus red NMH_MUTATE=P5-5D: the INTREQ clear left out, so the next poll sees RBF=1 at once -- both cores (70 passed, 7 failed): flips "5d [p256]: ..., run A", "5e [p255]: every G5 bracket (run A) ...", "(a)+(c) [p255,p256,p257,p283]: ..." and "(b) [p256]: the G5 bracket's lower end ..."
// MUTATION 5e stimulus red NMH_MUTATE=P5-5E: every poll iteration padded by >= T/4, so the G5 bracket is no longer narrower than T/4 -- both cores (71 passed, 6 failed): flips "5e [p255]: every G5 bracket (run A) ...", "(a)+(c) [p255,p256,p257,p283]: ..." and "(b) [p256]: ..."
// MUTATION (a)+(c) stimulus red NMH_MUTATE=P5-A: run A itself is driven at a bit time of 2T, so the unchanged (a)+(c) and (b) comparators read run A's own byte mismatch and timing -- both cores (72 passed, 5 failed): flips "(a)+(c) [p255,p256,p257,p283]: ..." and "(b) [p256]: ..."; 5a/5c/5d/5e (run A) stay green
// MUTATION 5f stimulus red NMH_MUTATE=P5-5F: run B itself is driven at a bit time of 2T, so the unchanged 5f comparator reads run B's own byte mismatch -- both cores (71 passed, 6 failed): flips "5f [p255]: RESULT bits 7-0 = byte_j for every j, run B", and (c)'s own timing half also moves enough to flip "(a)+(c) [p255,p256,p257,p283]: ..." and "(b) [p256]: ..." with it
// MUTATION (b) stimulus red NMH_MUTATE=P5-B: bin_a itself becomes prog_rx_a_altper.bin (SERPER<-$80B9, N=185, driver unchanged) -- both cores (72 passed, 5 failed): flips "(b) [p256]: ..." and, since (a)+(c)'s own timing half shares the same brackets, "(a)+(c) [p255,p256,p257,p283]: ..." with it; 5a/5c/5d/5e (run A) stay green
// MUTATION (a)+(c) stimulus red NMH_MUTATE=P5-C: bin_b itself becomes prog_rx_a.bin (SERPER<-LONG in run B too) -- both cores (73 passed, 4 failed): flips only "(a)+(c) [p255,p256,p257,p283]: ..." (the timing half is no longer one bit time later); 5a/5c/5d/5e/5f (run B, now really run A's own program) stay green

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

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

constexpr uint32_t kMarkAddr  = 0x0C1000;
constexpr uint16_t kMarkDone  = 0x00F5;
constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint32_t kPollTimeoutAddr = 0x0C0020; // G4: POLLBIT ORs 1 in here if any poll reaches its bound

// chipset_config bit1 selects the video standard (1=NTSC, 0=PAL); R2 (below)
// confirms which one a given capture actually shows. Bit3 stays set (ECS),
// matching the harness default.
nmh::Config cfg_for(bool ntsc) {
    nmh::Config c;
    c.chipset_config = ntsc ? 0x0A : 0x08;
    return c;
}

// ---- R1: the colour clock, from the four refresh accesses per line -------

struct RefreshGroup { uint64_t r1, r2, r3, r4; };

// The candidate groups of low-address reads, split where two reads lie more
// than 100 cycles apart. The last group is dropped when the capture ended
// within 100 cycles of its last read: the gap rule has not closed it, so it
// may lack reads the capture never saw, and it does not lie wholly inside the
// capture.
std::vector<std::vector<uint64_t>> candidate_groups(const std::vector<nmh::RamAccess>& log, uint64_t end_cycle) {
    std::vector<uint64_t> low;
    for (const auto& a : log) if (!a.is_write && a.word_addr < 0x1000) low.push_back(a.cycle);
    std::vector<std::vector<uint64_t>> groups;
    if (low.empty()) return groups;
    groups.push_back({low[0]});
    for (size_t i = 1; i < low.size(); ++i) {
        if (low[i] - low[i - 1] > 100) groups.push_back({});
        groups.back().push_back(low[i]);
    }
    if (end_cycle - groups.back().back() <= 100) groups.pop_back();
    return groups;
}

std::vector<RefreshGroup> find_refresh_groups(const std::vector<nmh::RamAccess>& log, uint64_t end_cycle) {
    std::vector<RefreshGroup> out;
    auto groups = candidate_groups(log, end_cycle);
    for (const auto& g : groups) {
        if (g.size() != 4) continue;
        out.push_back({g[0], g[1], g[2], g[3]});
    }
    return out;
}

// Check (a): every group's three internal gaps equal 2*candidate_C, in
// every line. Passing the *measured* C grades the real anchor.
bool check_r1_anchor(const std::vector<RefreshGroup>& groups, double candidate_C) {
    if (groups.empty() || candidate_C <= 0) return false;
    double expected = 2.0 * candidate_C;
    for (const auto& g : groups) {
        double g1 = double(g.r2 - g.r1), g2 = double(g.r3 - g.r2), g3 = double(g.r4 - g.r3);
        if (std::abs(g1 - expected) > 0.01 || std::abs(g2 - expected) > 0.01 || std::abs(g3 - expected) > 0.01)
            return false;
    }
    return true;
}

// R1 population: find_refresh_groups() silently drops every candidate group
// whose size is not exactly four, so check_r1_anchor() alone can pass while
// a malformed or missing line was discarded before it ever saw one. This
// re-walks the same low-address-read candidates (grouped by the same >100
// cycle gap) and reports how many of them are NOT the shape Figure 6-9
// draws: exactly four reads, each 2*C after the one before -- the same
// shape check_r1_anchor grades, so nothing here is a new oracle, only a
// completeness count over every candidate group, not just the retained
// ones.
struct RefreshPopulation { int groups = 0, malformed = 0; };

RefreshPopulation refresh_population(const std::vector<nmh::RamAccess>& log, double C, uint64_t end_cycle) {
    RefreshPopulation p;
    auto groups = candidate_groups(log, end_cycle);
    if (groups.empty() || C <= 0) return p;
    p.groups = int(groups.size());
    double expected = 2.0 * C;
    for (const auto& g : groups) {
        bool shape_ok = g.size() == 4;
        for (size_t i = 1; shape_ok && i < g.size(); ++i)
            if (std::abs(double(g[i] - g[i - 1]) - expected) > 0.01) shape_ok = false;
        if (!shape_ok) ++p.malformed;
    }
    return p;
}

// ---- shared TX capture (P1/P2/P3) -----------------------------------------

struct TxRun {
    bool loaded = false, released = false;
    std::vector<std::pair<uint64_t, uint16_t>> marks; // (cycle, value), chronological
    std::vector<uint64_t> tx_changes;                 // cycle of every uart_tx transition
    std::vector<RefreshGroup> refresh;
    double C = 0.0;
    int field_lines = 0; // R2: refresh-groups in one representative field
    bool poll_timeout = false; // G4: a POLLBIT in this run reached its own bound
    int pop_groups = 0, pop_malformed = 0; // R1 population (refresh_population)
};

std::optional<uint64_t> mark_cycle(const TxRun& r, uint16_t value) {
    for (const auto& m : r.marks) if (m.second == value) return m.first;
    return std::nullopt;
}

uint64_t budget_for(int nval, int wcount) {
    return 20'000'000ull + 10ull * uint64_t(wcount) * uint64_t(nval + 1) * 8ull + 500'000ull;
}

TxRun run_tx(const std::string& bin, bool ntsc, uint64_t post_boot_budget) {
    TxRun r;
    nmh::NanomigHarness h(cfg_for(ntsc));
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;

    bool level = true; // TxD idle level is 1 (R4; precondition 1c/5b anchor this)
    bool done = false;
    const uint32_t mark_word = word_addr(kMarkAddr);
    size_t seen = 0;
    for (uint64_t i = 0; i < post_boot_budget && !done; ++i) {
        h.run_cycles(1);
        bool cur = h.ports()->uart_tx;
        if (cur != level) { r.tx_changes.push_back(h.cycles() - 1); level = cur; }
        const auto& log = h.ram_log();
        for (; seen < log.size(); ++seen) {
            if (log[seen].is_write && log[seen].word_addr == mark_word) {
                r.marks.push_back({log[seen].cycle, log[seen].data});
                if (log[seen].data == kMarkDone) done = true;
            }
        }
    }

    r.refresh = find_refresh_groups(h.ram_log(), h.cycles());
    if (!r.refresh.empty()) r.C = double(r.refresh[0].r4 - r.refresh[0].r1) / 6.0;
    { auto pop = refresh_population(h.ram_log(), r.C, h.cycles()); r.pop_groups = pop.groups; r.pop_malformed = pop.malformed; }

    auto frames = nmh::find_frames(h.video());
    if (frames.size() >= 3) {
        int count = 0;
        for (const auto& g : r.refresh) if (g.r1 >= frames[1].begin && g.r1 < frames[1].end) ++count;
        r.field_lines = count;
    }
    auto to = h.peek_logical(kPollTimeoutAddr);
    r.poll_timeout = to.has_value() && *to != 0;
    return r;
}

// ---- P1/P2/P3 grid check ---------------------------------------------------

struct GridResult {
    bool ok = false;
    int matched = 0, total_expected = 0;
    size_t total_changes = 0;
    uint64_t t0 = 0;
};

GridResult check_grid(const std::vector<uint64_t>& changes, double C, int N, int W) {
    GridResult g;
    g.total_expected = 10 * W;
    if (changes.empty() || C <= 0) return g;
    g.t0 = changes[0];
    double T = double(N + 1) * C;
    double tol = C / 2.0;
    std::vector<bool> used(changes.size(), false);
    int matched = 0;
    for (int k = 0; k < 10 * W; ++k) {
        double target = double(g.t0) + double(k) * T;
        for (size_t i = 0; i < changes.size(); ++i) {
            if (used[i]) continue;
            if (std::abs(double(changes[i]) - target) <= tol) { used[i] = true; ++matched; break; }
        }
    }
    g.matched = matched;
    g.total_changes = changes.size();
    g.ok = (matched == 10 * W) && (changes.size() == size_t(10 * W));
    return g;
}

// Within-word grid (control): for a WMULTI run, the changes come in groups
// of 10 per word (start, 8 data bits, stop). The 9 gaps WITHIN each word
// (not the 10th, which crosses into the next word's own start bit) are
// graded in isolation from any inter-word drift.
bool check_within_word(const std::vector<uint64_t>& changes, double C, int N, int W) {
    if ((int)changes.size() != 10 * W || C <= 0) return false;
    double T = double(N + 1) * C, tol = C / 2.0;
    for (int w = 0; w < W; ++w)
        for (int i = 0; i < 9; ++i) {
            double gap = double(changes[w * 10 + i + 1]) - double(changes[w * 10 + i]);
            if (std::abs(gap - T) > tol) return false;
        }
    return true;
}

// ---- P2's own checks: uniform grid within a run, and cross-run period ----

struct UniformResult { bool ok = false; double T_N = 0; size_t total_changes = 0; };

UniformResult check_uniform(const std::vector<uint64_t>& changes, double C, int W) {
    UniformResult u;
    int total = 10 * W;
    if ((int)changes.size() < 2 || C <= 0) return u;
    u.total_changes = changes.size();
    u.T_N = double(changes.back() - changes.front()) / double(total - 1);
    double tol = C / 2.0;
    uint64_t t0 = changes.front();
    std::vector<bool> used(changes.size(), false);
    int matched = 0;
    for (int k = 0; k < total; ++k) {
        double target = double(t0) + double(k) * u.T_N;
        for (size_t i = 0; i < changes.size(); ++i) {
            if (used[i]) continue;
            if (std::abs(double(changes[i]) - target) <= tol) { used[i] = true; ++matched; break; }
        }
    }
    u.ok = (matched == total) && (changes.size() == size_t(total));
    return u;
}

// ==== P5: nine-bit receive capture and analysis =============================

constexpr uint32_t kResultAddr = 0x0C0000; // shared with P4 (never run concurrently)
constexpr uint16_t kMarkBrk   = 0x00F1;
constexpr uint16_t kMarkUnbrk = 0x00F2;
constexpr uint16_t kMarkReady = 0x00F3;
constexpr uint32_t kProbeIAddr = 0x0C0012; // kept clear of RESULT..RESULT+$0E (ch8_defs.i)
const std::vector<uint8_t> kRxBytes = {0xA5, 0x5A, 0x81, 0x7E, 0x3C, 0xC3, 0x66, 0x99};

struct RxRun {
    bool loaded = false, released = false;
    std::vector<std::pair<uint64_t, uint16_t>> marks;
    std::vector<std::pair<uint64_t, uint16_t>> probe_i;
    std::vector<uint64_t> tx_changes;
    std::vector<std::optional<uint16_t>> results;
    std::vector<RefreshGroup> refresh;
    double C = 0.0;
    int field_lines = 0;
    uint64_t s0 = 0;
    bool scheduled = false;
    bool poll_timeout = false; // G4: any of the custom RBF poll loops reached its own bound
    int pop_groups = 0, pop_malformed = 0; // R1 population (refresh_population)
};

std::optional<uint64_t> mark_cycle_rx(const RxRun& r, uint16_t value) {
    for (const auto& m : r.marks) if (m.second == value) return m.first;
    return std::nullopt;
}

// RxD level at cycle `cyc`: idle (1) before s0 and after the last
// character; within a character, bit 0 is the start bit (0), bits 1-8 are
// byte_j's own bits 0-7 (LSB first, harmless either way since every byte_j
// is a bit-palindrome), bit 9 (the ninth bit) is 1 unless `ninth_zero`
// selects the recorded, not-graded variant (SPECIFICATION.md P5), and bit
// 10 (stop) and anything past bit 10 (the idle gap before the next
// character) are always 1.
bool rxd_level_at(uint64_t cyc, uint64_t s0, double T, double step, const std::vector<uint8_t>& bytes,
                   bool ninth_zero = false) {
    if (cyc < s0) return true;
    int j = int(double(cyc - s0) / step);
    if (j < 0 || j >= (int)bytes.size()) return true;
    double sj = double(s0) + double(j) * step;
    double pos = double(cyc) - sj;
    if (pos < 0) return true;
    int bit = int(pos / T);
    if (bit == 0) return false;
    if (bit >= 1 && bit <= 8) return (bytes[j] >> (bit - 1)) & 1;
    if (bit == 9 && ninth_zero) return false;
    return true;
}

// `t_scale` multiplies the driver's own bit time relative to T (1.0 =
// correct N=371 rate; 2.0 = the (a)/5f contrasting cases' wrong rate).
// `ntsc` is true for every graded P5 run (P5 is NTSC only); the MUTATION
// 5a-STANDARD contrast passes false to capture the same binary on PAL.
// `ninth_zero` selects the recorded run-A variant (SPECIFICATION.md P5)
// whose every ninth bit is driven 0 instead of 1.
RxRun run_rx(const std::string& bin, uint64_t budget, double T, double t_scale, bool ntsc = true,
             bool ninth_zero = false) {
    RxRun r;
    nmh::NanomigHarness h(cfg_for(ntsc));
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;

    const double Tdrv = T * t_scale;
    const double step = 16.125 * Tdrv; // 16T + T/8
    const uint64_t lead_margin = uint64_t(3.0 * T); // >= 2T (G2's own margin note)
    bool tx_level = true;
    bool done = false;
    const uint32_t mark_word = word_addr(kMarkAddr);
    const uint32_t probe_i_word = word_addr(kProbeIAddr);
    size_t seen = 0;
    for (uint64_t i = 0; i < budget && !done; ++i) {
        bool level = true;
        if (r.scheduled) level = rxd_level_at(h.cycles(), r.s0, Tdrv, step, kRxBytes, ninth_zero);
        h.ports()->uart_rx = level;
        h.run_cycles(1);
        bool cur_tx = h.ports()->uart_tx;
        if (cur_tx != tx_level) { r.tx_changes.push_back(h.cycles() - 1); tx_level = cur_tx; }
        const auto& log = h.ram_log();
        for (; seen < log.size(); ++seen) {
            const auto& a = log[seen];
            if (a.is_write && a.word_addr == mark_word) {
                r.marks.push_back({a.cycle, a.data});
                if (a.data == kMarkDone) done = true;
                if (a.data == kMarkReady && !r.scheduled) { r.s0 = a.cycle + lead_margin; r.scheduled = true; }
            }
            if (a.is_write && a.word_addr == probe_i_word) r.probe_i.push_back({a.cycle, a.data});
        }
    }
    for (int j = 0; j < 8; ++j) r.results.push_back(h.peek_logical(kResultAddr + 2 * j));
    r.refresh = find_refresh_groups(h.ram_log(), h.cycles());
    if (!r.refresh.empty()) r.C = double(r.refresh[0].r4 - r.refresh[0].r1) / 6.0;
    { auto pop = refresh_population(h.ram_log(), r.C, h.cycles()); r.pop_groups = pop.groups; r.pop_malformed = pop.malformed; }
    auto frames = nmh::find_frames(h.video());
    if (frames.size() >= 3) {
        int count = 0;
        for (const auto& g : r.refresh) if (g.r1 >= frames[1].begin && g.r1 < frames[1].end) ++count;
        r.field_lines = count;
    }
    auto to = h.peek_logical(kPollTimeoutAddr);
    r.poll_timeout = to.has_value() && *to != 0;
    return r;
}

bool level_before(const std::vector<uint64_t>& changes, uint64_t cycle) {
    bool level = true;
    for (auto c : changes) { if (c > cycle) break; level = !level; }
    return level;
}

struct Bracket { uint64_t t0 = 0, t1 = 0, S = 0; bool ok = false; };

// G5's own bracket for character j: the probe_i samples strictly after
// `lo` (the previous MARK) and up to and including `hi` (this character's
// own MARK) are exactly that character's poll; the loop always stores
// INTREQR before testing it, so the last sample is the first one showing
// RBF=1 and the one before it is the last showing RBF=0 (t1, t0), and S is
// the widest gap between consecutive stores in that same poll.
Bracket g5_bracket(const std::vector<std::pair<uint64_t, uint16_t>>& probe_i, uint64_t lo, uint64_t hi) {
    Bracket b;
    std::vector<uint64_t> cyc;
    for (const auto& p : probe_i) if (p.first > lo && p.first <= hi) cyc.push_back(p.first);
    if (cyc.size() < 2) return b;
    std::sort(cyc.begin(), cyc.end());
    for (size_t i = 1; i < cyc.size(); ++i) b.S = std::max(b.S, cyc[i] - cyc[i - 1]);
    b.t1 = cyc.back();
    b.t0 = cyc[cyc.size() - 2];
    b.ok = true;
    return b;
}

double bracket_midpoint(const Bracket& b) {
    return (double(b.t0) - double(b.S) + double(b.t1)) / 2.0;
}

std::pair<uint64_t, uint64_t> rx_window(const RxRun& r, int j) {
    uint64_t lo = (j == 0) ? mark_cycle_rx(r, kMarkReady).value_or(0) : mark_cycle_rx(r, uint16_t(j)).value_or(0);
    uint64_t hi = mark_cycle_rx(r, uint16_t(j + 1)).value_or(0);
    return {lo, hi};
}

void run_p5(const std::string& dir, const std::string& mv, Checks& checks) {
    auto bin_a = dir + (mv == "P5-5B" ? "/prog_rx_a_5b.bin" :
                         mv == "P5-5C" ? "/prog_rx_a_5c.bin" :
                         mv == "P5-5D" ? "/prog_rx_a_5d.bin" :
                         mv == "P5-5E" ? "/prog_rx_a_5e.bin" :
                         mv == "P5-B"  ? "/prog_rx_a_altper.bin" : "/prog_rx_a.bin");
    auto bin_b = dir + (mv == "P5-C" ? "/prog_rx_a.bin" : "/prog_rx_b.bin");
    constexpr uint64_t kP5Budget = 25'000'000;
    const double T = 372.0 * 8.0; // N=371, C confirmed 8.0 by 5a below and every other proposition's own R1
    const double step = 16.125 * T;

    // NMH_MUTATE=P5-A drives run A itself at a bit time of 2T; NMH_MUTATE=
    // P5-5F drives run B itself at 2T; NMH_MUTATE=P5-C already swapped
    // bin_b to prog_rx_a.bin above. Every check below reads a/b as they
    // come out of these two calls, so a mutation's contrasting capture
    // reaches every check that uses it, through the check's own unchanged
    // comparator.
    RxRun a = run_rx(bin_a, kP5Budget, T, mv == "P5-A" ? 2.0 : 1.0);
    RxRun b = run_rx(bin_b, kP5Budget, T, mv == "P5-5F" ? 2.0 : 1.0);

    checks.note("P5 capture health: a loaded=" + std::to_string(a.loaded) + " released=" + std::to_string(a.released) +
                " marks=" + std::to_string(a.marks.size()) + " probe_i=" + std::to_string(a.probe_i.size()) +
                " s0=" + std::to_string(a.s0) + " C=" + std::to_string(a.C) + " field_lines=" + std::to_string(a.field_lines) +
                " poll_timeout=" + std::to_string(a.poll_timeout));
    checks.note("P5 capture health: b loaded=" + std::to_string(b.loaded) + " released=" + std::to_string(b.released) +
                " marks=" + std::to_string(b.marks.size()) + " probe_i=" + std::to_string(b.probe_i.size()) +
                " s0=" + std::to_string(b.s0) + " C=" + std::to_string(b.C) + " field_lines=" + std::to_string(b.field_lines) +
                " poll_timeout=" + std::to_string(b.poll_timeout));

    // ---- 5a: R1 anchor, R2 shows NTSC, and the driver's own C=8
    // assumption, on run A and on run B; and completion (G4), on both. ----
    {
        checks.note("5a run A: R1 population groups=" + std::to_string(a.pop_groups) +
                    " malformed=" + std::to_string(a.pop_malformed));
        bool r1ok = !a.refresh.empty() && check_r1_anchor(a.refresh, a.C) &&
                    a.pop_groups > 0 && a.pop_malformed == 0;
        checks.expect(r1ok,
            "5a [p194,p195] R1 anchor, run A: every candidate refresh group in the run is 2C, 2C "
            "and 2C apart (Figure 6-9's own shape), none discarded before grading");
        bool ntsc_ok = a.field_lines == 262 || a.field_lines == 263;
        checks.expect(ntsc_ok, "5a [p24] R2 shows NTSC, run A (measured field_lines=" + std::to_string(a.field_lines) + ")");
        // The P5 driver's own T=372*8.0 assumes C=8.0 (comment above); 5a's own
        // run must confirm that, not just recite it (see SPECIFICATION.md).
        bool c_ok = std::abs(a.C - 8.0) < 0.001;
        checks.expect(c_ok,
            "5a [p194,p195]: the run's own R1 anchor C equals the driver's own C=8.0 assumption, "
            "run A (measured C=" + std::to_string(a.C) + ")");
        bool done_a = mark_cycle_rx(a, kMarkDone).has_value();
        checks.expect(done_a && !a.poll_timeout,
            "5a [p283]: MARK=DONE is present and no poll (G4, any of them) ended at its own bound, "
            "run A");

        checks.note("5a run B: R1 population groups=" + std::to_string(b.pop_groups) +
                    " malformed=" + std::to_string(b.pop_malformed));
        bool r1ok_b = !b.refresh.empty() && check_r1_anchor(b.refresh, b.C) &&
                      b.pop_groups > 0 && b.pop_malformed == 0;
        checks.expect(r1ok_b,
            "5a [p194,p195] R1 anchor, run B: every candidate refresh group in the run is 2C, 2C "
            "and 2C apart (Figure 6-9's own shape), none discarded before grading");
        bool ntsc_ok_b = b.field_lines == 262 || b.field_lines == 263;
        checks.expect(ntsc_ok_b, "5a [p24] R2 shows NTSC, run B (measured field_lines=" + std::to_string(b.field_lines) + ")");
        bool c_ok_b = std::abs(b.C - 8.0) < 0.001;
        checks.expect(c_ok_b,
            "5a [p194,p195]: the run's own R1 anchor C equals the driver's own C=8.0 assumption, "
            "run B (measured C=" + std::to_string(b.C) + ")");
        bool done_b = mark_cycle_rx(b, kMarkDone).has_value();
        checks.expect(done_b && !b.poll_timeout,
            "5a [p283]: MARK=DONE is present and no poll (G4, any of them) ended at its own bound, "
            "run B");
    }

    // ---- 5b: level anchor ----
    // NMH_MUTATE=P5-5B swaps bin_a to the no-UARTBRK-set variant above: the
    // level between MARK=BRK and MARK=UNBRK then also reads 1, and this same check flips.
    {
        auto mbrk = mark_cycle_rx(a, kMarkBrk), munbrk = mark_cycle_rx(a, kMarkUnbrk);
        bool brk_level = mbrk.has_value() && level_before(a.tx_changes, *mbrk + 50);
        bool unbrk_level = munbrk.has_value() && level_before(a.tx_changes, *munbrk + 50);
        bool ok = mbrk.has_value() && munbrk.has_value() && !brk_level && unbrk_level;
        checks.expect(ok,
            "5b [p257]: TxD is 0 between MARK=BRK and MARK=UNBRK (UARTBRK forces the transmit pin "
            "to zero) and 1 after MARK=UNBRK (idle)");
    }

    // ---- 5c: no overrun, on run A and on run B ----
    // NMH_MUTATE=P5-5C swaps bin_a to the delayed-read-of-character-0
    // variant: p.256 predicts OVRUN sets, and this same check flips.
    {
        bool ok = true;
        for (const auto& res : a.results) if (!res || (*res & 0x8000)) ok = false;
        checks.expect(ok, "5c [p256]: OVRUN (bit 15) is 0 in every RESULT value, run A");

        bool ok_b = true;
        for (const auto& res : b.results) if (!res || (*res & 0x8000)) ok_b = false;
        checks.expect(ok_b, "5c [p256]: OVRUN (bit 15) is 0 in every RESULT value, run B");
    }

    // ---- 5d: every RBF event is observed, on run A and on run B ----
    // NMH_MUTATE=P5-5D swaps bin_a to the missing-INTREQ-clear variant.
    auto check_5d = [](const RxRun& r) {
        auto ready = mark_cycle_rx(r, kMarkReady);
        bool ok = ready.has_value() && *ready < r.s0;
        for (const auto& p : r.probe_i) if (p.first < r.s0 && (p.second & 0x0800)) ok = false;
        for (int j = 0; j < 8; ++j) {
            auto [lo, hi] = rx_window(r, j);
            bool seen_zero = false, jok = false;
            for (const auto& p : r.probe_i) {
                if (p.first < lo || p.first > hi) continue;
                bool bit11 = (p.second & 0x0800) != 0;
                if (!bit11) seen_zero = true; else { jok = seen_zero; break; }
            }
            if (!jok) ok = false;
        }
        return ok;
    };
    {
        bool ok = check_5d(a);
        checks.expect(ok,
            "5d [p256]: MARK=READY precedes s_0, no PROBE_I value before s_0 shows RBF, and every "
            "character's own poll sees RBF=0 before its first RBF=1, run A");
        bool ok_b = check_5d(b);
        checks.expect(ok_b,
            "5d [p256]: MARK=READY precedes s_0, no PROBE_I value before s_0 shows RBF, and every "
            "character's own poll sees RBF=0 before its first RBF=1, run B");
    }

    // ---- 5e: resolution -- every G5 bracket narrower than T/4, on run A
    // and on run B ----
    // NMH_MUTATE=P5-5E swaps bin_a to the padded-poll-iteration variant.
    std::vector<Bracket> brackets_a(8), brackets_b(8);
    for (int j = 0; j < 8; ++j) {
        auto [lo, hi] = rx_window(a, j);
        brackets_a[j] = g5_bracket(a.probe_i, lo, hi);
    }
    for (int j = 0; j < 8; ++j) {
        auto [lo, hi] = rx_window(b, j);
        brackets_b[j] = g5_bracket(b.probe_i, lo, hi);
    }
    {
        bool ok = true;
        for (int j = 0; j < 8; ++j) {
            double width = brackets_a[j].ok ? double(brackets_a[j].t1 - brackets_a[j].t0 + brackets_a[j].S) : 1e18;
            if (!brackets_a[j].ok || !(width < T / 4.0)) ok = false;
        }
        checks.expect(ok, "5e [p255]: every G5 bracket (run A) is narrower than T/4");

        bool ok_b = true;
        for (int j = 0; j < 8; ++j) {
            double width = brackets_b[j].ok ? double(brackets_b[j].t1 - brackets_b[j].t0 + brackets_b[j].S) : 1e18;
            if (!brackets_b[j].ok || !(width < T / 4.0)) ok_b = false;
        }
        checks.expect(ok_b, "5e [p255]: every G5 bracket (run B) is narrower than T/4");
    }

    // ---- (a)+(c): run A recovers the byte, bit 9 and bit 8, and its RBF
    // event comes one bit time later than run B's -- together, not
    // separately, since the timing half alone does not depend on the
    // nine-bit (LONG) setting. ----
    double meanD_a = 0, meanD_b = 0, diffAB = 0;
    {
        bool data_ok = true;
        std::string dump;
        for (int j = 0; j < 8; ++j) {
            char buf[48];
            uint32_t got = a.results[j] ? uint32_t(*a.results[j]) : 0xFFFFFFFFu;
            std::snprintf(buf, sizeof buf, " j%d(exp %02X)=%04X", j, kRxBytes[j], got);
            dump += buf;
            if (!a.results[j] || (*a.results[j] & 0x03FF) != (0x0300 | kRxBytes[j])) data_ok = false;
        }
        checks.note("(a) run A RESULT dump:" + dump);

        double sumA = 0, sumB = 0;
        for (int j = 0; j < 8; ++j) sumA += bracket_midpoint(brackets_a[j]) - (double(a.s0) + j * step);
        for (int j = 0; j < 8; ++j) sumB += bracket_midpoint(brackets_b[j]) - (double(b.s0) + j * step);
        meanD_a = sumA / 8.0; meanD_b = sumB / 8.0;
        diffAB = meanD_a - meanD_b;
        checks.note("(c): meanD_a=" + std::to_string(meanD_a) + " meanD_b=" + std::to_string(meanD_b) +
                    " diff=" + std::to_string(diffAB) + " T=" + std::to_string(T));
        bool timing_ok = diffAB >= T / 2.0 && diffAB <= 1.5 * T;

        checks.expect(data_ok && timing_ok,
            "(a)+(c) [p255,p256,p257,p283]: run A recovers byte_j (RESULT bits 7-0), bit 9 and "
            "bit 8 for every j, and its RBF event comes one bit time later than run B's own "
            "(mean_j D_j(A) - mean_j D_j(B) in [T/2,3T/2])");
        checks.record(
            "opus:8-serial.c36,sol:8-serial.c21 [p255,p256,p257,p283]: withdrawn from the graded "
            "line above -- establishing that bit 8 specifically carries the ninth (parity) bit "
            "needs a run where the ninth bit is driven 0, and p.257 and p.283 disagree on what "
            "RESULT bit 8 should then read (0 or 1; see the run-A ninth-bit-zero variant below), "
            "so that discriminating case cannot be graded at this boundary");
    }

    // ---- (b) No early signal ----
    // G5 places the event in (t0-S, t1]; only the bracket's lower end
    // bounds how early it could have been, so this reads t0-S, not t1
    // (SPECIFICATION.md).
    {
        bool ok = true;
        for (int j = 0; j < 8; ++j) {
            double sj = double(a.s0) + double(j) * step;
            double lo = double(brackets_a[j].t0) - double(brackets_a[j].S);
            if (!brackets_a[j].ok || !(lo >= sj + 9.0 * T)) ok = false;
        }
        for (int j = 0; j < 8; ++j) {
            double sj = double(b.s0) + double(j) * step;
            double lo = double(brackets_b[j].t0) - double(brackets_b[j].S);
            if (!brackets_b[j].ok || !(lo >= sj + 8.0 * T)) ok = false;
        }
        checks.expect(ok,
            "(b) [p256]: the G5 bracket's lower end (t0-S) for every j is no earlier than "
            "s_j+9T (run A) or s_j+8T (run B)");
    }

    // ---- 5f: run B received the same characters ----
    {
        bool ok = true;
        std::string dump;
        for (int j = 0; j < 8; ++j) {
            char buf[48];
            uint32_t got = b.results[j] ? uint32_t(*b.results[j]) : 0xFFFFFFFFu;
            std::snprintf(buf, sizeof buf, " j%d(exp %02X)=%04X", j, kRxBytes[j], got);
            dump += buf;
            if (!b.results[j] || (*b.results[j] & 0x00FF) != kRxBytes[j]) ok = false;
        }
        checks.note("5f run B RESULT dump:" + dump);
        checks.expect(ok, "5f [p255]: RESULT bits 7-0 = byte_j for every j, run B");
    }

    // ---- P5 run-A ninth-bit-zero variant: recorded, not graded (p.257
    // predicts RESULT bit 8 = 0, p.283 predicts 1; SPECIFICATION.md P5).
    // Always the pristine run-A binary, regardless of NMH_MUTATE, since it
    // exercises the RxD driver's own ninth-bit level, not a stimulus
    // mutation. ----
    {
        RxRun a0 = run_rx(dir + "/prog_rx_a.bin", kP5Budget, T, 1.0, true, true);
        std::string dump;
        for (int j = 0; j < 8; ++j) {
            char buf[16];
            int bit8 = a0.results[j] ? int((*a0.results[j] >> 8) & 1) : -1;
            std::snprintf(buf, sizeof buf, " j%d=%d", j, bit8);
            dump += buf;
        }
        checks.record("P5 run-A ninth-bit-zero variant [p257,p283]: RESULT bit 8 per character, "
                       "every ninth bit driven 0:" + dump + " -- p.257 predicts 0, p.283 predicts "
                       "1, so the pages disagree and this is not graded");
    }
}

// ==== P4: disk write DMA capture and analysis ==============================

constexpr uint32_t kDbufAddr   = 0x020000; // the program's own DBUF (ch8_defs.i)
constexpr uint32_t kProbeDAddr = 0x0C0010; // kept clear of RESULT..RESULT+$0E (ch8_defs.i)
constexpr int       kDbufLen   = 256;

struct FastRun {
    bool loaded = false, released = false;
    std::optional<uint64_t> done_cycle;
    std::vector<nmh::RamAccess> log;
    std::optional<uint16_t> result;
    std::vector<RefreshGroup> refresh;
    double C = 0.0;
    int field_lines = 0;
    bool poll_timeout = false; // G4: the custom disk poll loop reached its own bound
    int pop_groups = 0, pop_malformed = 0; // R1 population (refresh_population)
};

FastRun run_fast(const std::string& bin, bool ntsc, uint64_t budget) {
    FastRun r;
    nmh::NanomigHarness h(cfg_for(ntsc));
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;
    r.done_cycle = h.run_until_write(kMarkAddr, kMarkDone, budget);
    r.log = h.ram_log();
    r.result = h.peek_logical(kResultAddr);
    r.refresh = find_refresh_groups(r.log, h.cycles());
    if (!r.refresh.empty()) r.C = double(r.refresh[0].r4 - r.refresh[0].r1) / 6.0;
    { auto pop = refresh_population(r.log, r.C, h.cycles()); r.pop_groups = pop.groups; r.pop_malformed = pop.malformed; }
    auto frames = nmh::find_frames(h.video());
    if (frames.size() >= 3) {
        int count = 0;
        for (const auto& g : r.refresh) if (g.r1 >= frames[1].begin && g.r1 < frames[1].end) ++count;
        r.field_lines = count;
    }
    auto to = h.peek_logical(kPollTimeoutAddr);
    r.poll_timeout = to.has_value() && *to != 0;
    return r;
}

// First read cycle of each word B+2i, i=0..n-1.
std::vector<std::optional<uint64_t>> extract_ti(const FastRun& r, int n = kDbufLen) {
    std::vector<std::optional<uint64_t>> t(n);
    uint32_t base = word_addr(kDbufAddr);
    for (const auto& a : r.log) {
        if (a.is_write || a.word_addr < base || a.word_addr >= base + uint32_t(n)) continue;
        int i = int(a.word_addr - base);
        if (!t[i]) t[i] = a.cycle;
    }
    return t;
}

// Total read count of each word B+2i, i=0..n-1 -- unlike extract_ti(), this
// counts every read, not just the first, so a duplicate read of an
// already-seen index is visible.
std::vector<int> count_reads(const FastRun& r, int n = kDbufLen) {
    std::vector<int> c(n, 0);
    uint32_t base = word_addr(kDbufAddr);
    for (const auto& a : r.log) {
        if (a.is_write || a.word_addr < base || a.word_addr >= base + uint32_t(n)) continue;
        ++c[int(a.word_addr - base)];
    }
    return c;
}

bool result_fast_bit_matches(const FastRun& r, bool expect_fast1) {
    if (!r.result) return false;
    return ((*r.result & 0x0100) != 0) == expect_fast1;
}

// 4d: every PROBE_D value stored strictly inside [lo,hi] has DMAON (bit14)
// and DISKWRITE (bit13) set.
bool check_4d(const FastRun& r, uint64_t lo, uint64_t hi, int* n_checked) {
    int n = 0; bool ok = true;
    uint32_t pd = word_addr(kProbeDAddr);
    for (const auto& a : r.log) {
        if (a.is_write && a.word_addr == pd && a.cycle >= lo && a.cycle <= hi) {
            ++n;
            if ((a.data & 0x4000) == 0 || (a.data & 0x2000) == 0) ok = false;
        }
    }
    if (n_checked) *n_checked = n;
    return ok && n > 0;
}

double T_cell_us(uint64_t t_lo, uint64_t t_hi, double C, bool ntsc) {
    if (C <= 0) return -1;
    double cc = double(t_hi - t_lo) / C;
    double cc_per_cell = cc / (128.0 * 16.0);
    double us_per_cc = ntsc ? 0.27936 : 0.28194;
    return cc_per_cell * us_per_cc;
}

void run_p4(const std::string& dir, const std::string& mv, Checks& checks) {
    auto bin_fast1 = dir + (mv == "P4-4C" ? "/prog_fast1_4c.bin" :
                             mv == "P4-4D" ? "/prog_fast1_4d.bin" :
                             mv == "P4-4E" ? "/prog_fast1_4e.bin" : "/prog_fast1.bin");
    auto bin_fast0 = dir + "/prog_fast0.bin";
    constexpr uint64_t kP4Budget = 21'000'000;

    FastRun ntsc_f1 = run_fast(bin_fast1, true, kP4Budget);
    FastRun ntsc_f0 = run_fast(bin_fast0, true, kP4Budget);
    FastRun pal_f1  = run_fast(bin_fast1, false, kP4Budget);
    FastRun pal_f0  = run_fast(bin_fast0, false, kP4Budget);

    checks.note("P4 capture health: ntsc_f1 loaded=" + std::to_string(ntsc_f1.loaded) +
                " done=" + std::to_string(ntsc_f1.done_cycle.has_value()) +
                " C=" + std::to_string(ntsc_f1.C) + " field_lines=" + std::to_string(ntsc_f1.field_lines) +
                " result=" + std::to_string(ntsc_f1.result.value_or(0xFFFF)));
    checks.note("P4 capture health: ntsc_f0 loaded=" + std::to_string(ntsc_f0.loaded) +
                " done=" + std::to_string(ntsc_f0.done_cycle.has_value()) +
                " C=" + std::to_string(ntsc_f0.C) + " field_lines=" + std::to_string(ntsc_f0.field_lines) +
                " result=" + std::to_string(ntsc_f0.result.value_or(0xFFFF)));
    checks.note("P4 capture health: pal_f1 loaded=" + std::to_string(pal_f1.loaded) +
                " done=" + std::to_string(pal_f1.done_cycle.has_value()) +
                " C=" + std::to_string(pal_f1.C) + " field_lines=" + std::to_string(pal_f1.field_lines));
    checks.note("P4 capture health: pal_f0 loaded=" + std::to_string(pal_f0.loaded) +
                " done=" + std::to_string(pal_f0.done_cycle.has_value()) +
                " C=" + std::to_string(pal_f0.C) + " field_lines=" + std::to_string(pal_f0.field_lines));

    auto grade_run = [&](const FastRun& r, bool ntsc, bool fast1, const char* tag) {
        auto t = extract_ti(r);
        bool has_window = t[64].has_value() && t[128].has_value() && t[192].has_value();
        double tcell = has_window ? T_cell_us(*t[64], *t[192], r.C, ntsc) : -1;
        checks.note(std::string(tag) + ": T_cell=" + std::to_string(tcell) + " us, t64=" +
                    std::to_string(t[64].value_or(0)) + " t128=" + std::to_string(t[128].value_or(0)) +
                    " t192=" + std::to_string(t[192].value_or(0)));

        checks.record(std::string(tag) +
            " [p265,p249]: cannot be checked at this boundary -- the disk data clock is not "
            "among the top-level observable ports, and no page ties a Chip-memory read rate to "
            "it (opus:8-adkcon.c33 declined, SPECIFICATION.md 1.4)");

        // 4a: R1 anchor, and the run's own refresh-group population.
        checks.note(std::string(tag) + " R1 population groups=" + std::to_string(r.pop_groups) +
                    " malformed=" + std::to_string(r.pop_malformed));
        bool r1ok = !r.refresh.empty() && check_r1_anchor(r.refresh, r.C) &&
                    r.pop_groups > 0 && r.pop_malformed == 0;
        checks.expect(r1ok, std::string(tag) +
            " 4a [p194,p195]: R1 anchor holds on this run, and every candidate refresh group is "
            "the same four-read, 2C-apart shape (Figure 6-9), none discarded before grading");

        // 4b: R2 shows the standard selected.
        bool std_ok = ntsc ? (r.field_lines == 262 || r.field_lines == 263)
                            : (r.field_lines == 312 || r.field_lines == 313);
        checks.expect(std_ok,
            std::string(tag) + " 4b [p24]: R2's own field-line count (" + std::to_string(r.field_lines) +
            ") matches the standard selected");

        // 4c: the FAST bit took effect (ADKCONR readback).
        bool c_ok = result_fast_bit_matches(r, fast1);
        checks.expect(c_ok,
            std::string(tag) + " 4c [p249]: ADKCONR bit 8 read back as " + (fast1 ? "1" : "0") +
            " (RESULT=" + std::to_string(r.result.value_or(0xFFFF)) + ")");

        // 4d: DMA runs throughout the window.
        int n4d = 0;
        bool d_ok = has_window && check_4d(r, *t[64], *t[192], &n4d);
        checks.note(std::string(tag) + " 4d: " + std::to_string(n4d) + " PROBE_D samples in window");
        checks.expect(d_ok,
            std::string(tag) + " 4d [p248]: every PROBE_D value between t_64 and t_192 has DMAON "
            "and DISKWRITE set (" + std::to_string(n4d) + " samples)");

        // 4e: fetches cover the window (i=0..192 seen, each exactly once, rising order).
        bool e_ok = true;
        uint64_t prev = 0; bool have_prev = false;
        for (int i = 0; i <= 192; ++i) {
            if (!t[i]) { e_ok = false; continue; }
            if (have_prev && !(*t[i] > prev)) e_ok = false;
            prev = *t[i]; have_prev = true;
        }
        auto counts = count_reads(r);
        int dup_total = 0;
        for (int i = 0; i <= 192; ++i) if (counts[i] != 1) { e_ok = false; dup_total += counts[i]; }
        checks.note(std::string(tag) + " 4e: read counts for i=0..192, total=" + std::to_string(dup_total) +
                    " (192 expected if every index is read exactly once)");
        checks.expect(e_ok,
            std::string(tag) + " 4e [p247]: reads of B+2i are seen for i=0..192, each exactly "
            "once (by full read count, not just first-seen), in rising order");

        // Also measured, not graded: no page sentence bounds the fetch cadence.
        bool has_span = has_window;
        std::string span = has_span ?
            (" (measured " + std::to_string(int64_t(*t[128] - *t[64]) - int64_t(*t[192] - *t[128])) + ")") :
            std::string(" (no window observed)");
        checks.note(std::string(tag) + " |(t128-t64)-(t192-t128)|" + span);

        // Completion (G4): the run reached DONE and no poll ended at its own bound.
        checks.expect(r.done_cycle.has_value() && !r.poll_timeout,
            std::string(tag) + " [p283]: MARK=DONE is present and no poll (G4, any of them) "
            "ended at its own bound");
    };

    grade_run(ntsc_f1, true, true, "P4-NTSC-FAST1");
    grade_run(ntsc_f0, true, false, "P4-NTSC-FAST0");
    grade_run(pal_f1, false, true, "P4-PAL-FAST1");
    grade_run(pal_f0, false, false, "P4-PAL-FAST0");

    // 4c's own mutation needs no separate code: NMH_MUTATE=P4-4C already
    // swapped bin_fast1 above, so ntsc_f1 (and pal_f1, sharing the same
    // binary) *is* the mutant under that env var, and grade_run's own
    // "P4-...-FAST1 4c [...]" check flips on its own (ADKCONR reads back 0
    // instead of 1). NMH_MUTATE=P4-4D/4E likewise need no separate code:
    // each swaps bin_fast1 to its own contrast binary, and grade_run's
    // "4d [...]"/"4e [...]" checks on P4-NTSC-FAST1/P4-PAL-FAST1 flip on
    // their own.
}

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    // ==== P1/P2/P3: shared captures =====================================
    //
    // Each base binary is captured once per standard; NMH_MUTATE selects a
    // whole different binary (or nothing) uniformly for every capture that
    // reads it, so a single env var exercises the same substitution on
    // both NTSC and PAL alike, and on every proposition that shares it.

    auto bin_n0        = dir + (mv == "P1-1C" || mv == "P1-1D" || mv == "P1-1E" ? "/prog_tx_n0.bin" :
                                  mv == "P1-NPLUS1" ? "/prog_tx_n0_plus1.bin" : "/prog_tx_n0.bin");
    auto bin_n371       = dir + (mv == "P1-1C" ? "/prog_tx_n371_1c.bin" :
                                  mv == "P1-1D" ? "/prog_tx_n371_1d.bin" :
                                  mv == "P1-1E" ? "/prog_tx_n371_1e.bin" :
                                  mv == "P1-NPLUS1" ? "/prog_tx_n371_plus1.bin" : "/prog_tx_n371.bin");
    auto bin_n32396     = dir + (mv == "P1-NPLUS1" ? "/prog_tx_n32396_plus1.bin" : "/prog_tx_n32396.bin");

    TxRun ntsc_n0    = run_tx(bin_n0, true, budget_for(0, 1));
    TxRun ntsc_n371  = run_tx(bin_n371, true, budget_for(371, 4));
    TxRun ntsc_n32396 = run_tx(bin_n32396, true, budget_for(32396, 1));
    TxRun pal_n0     = run_tx(bin_n0, false, budget_for(0, 1));
    TxRun pal_n371   = run_tx(bin_n371, false, budget_for(371, 4));
    TxRun pal_n32396 = run_tx(bin_n32396, false, budget_for(32396, 1));

    checks.note("capture health: ntsc_n0 loaded=" + std::to_string(ntsc_n0.loaded) +
                " released=" + std::to_string(ntsc_n0.released) +
                " changes=" + std::to_string(ntsc_n0.tx_changes.size()) +
                " marks=" + std::to_string(ntsc_n0.marks.size()) +
                " mark0=" + std::to_string(mark_cycle(ntsc_n0, 0).value_or(999999999)) +
                " C=" + std::to_string(ntsc_n0.C) +
                " field_lines=" + std::to_string(ntsc_n0.field_lines));
    checks.note("capture health: ntsc_n371 loaded=" + std::to_string(ntsc_n371.loaded) +
                " released=" + std::to_string(ntsc_n371.released) +
                " changes=" + std::to_string(ntsc_n371.tx_changes.size()) +
                " marks=" + std::to_string(ntsc_n371.marks.size()) +
                " C=" + std::to_string(ntsc_n371.C) +
                " field_lines=" + std::to_string(ntsc_n371.field_lines));
    checks.note("capture health: ntsc_n32396 loaded=" + std::to_string(ntsc_n32396.loaded) +
                " released=" + std::to_string(ntsc_n32396.released) +
                " changes=" + std::to_string(ntsc_n32396.tx_changes.size()) +
                " marks=" + std::to_string(ntsc_n32396.marks.size()) +
                " C=" + std::to_string(ntsc_n32396.C) +
                " field_lines=" + std::to_string(ntsc_n32396.field_lines));
    checks.note("capture health: pal_n0 loaded=" + std::to_string(pal_n0.loaded) +
                " changes=" + std::to_string(pal_n0.tx_changes.size()) +
                " marks=" + std::to_string(pal_n0.marks.size()) +
                " mark0=" + std::to_string(mark_cycle(pal_n0, 0).value_or(999999999)) +
                " C=" + std::to_string(pal_n0.C) +
                " field_lines=" + std::to_string(pal_n0.field_lines));
    checks.note("capture health: pal_n371 loaded=" + std::to_string(pal_n371.loaded) +
                " changes=" + std::to_string(pal_n371.tx_changes.size()) +
                " marks=" + std::to_string(pal_n371.marks.size()) +
                " mark0=" + std::to_string(mark_cycle(pal_n371, 0).value_or(999999999)) +
                " C=" + std::to_string(pal_n371.C) +
                " field_lines=" + std::to_string(pal_n371.field_lines));
    checks.note("capture health: pal_n32396 loaded=" + std::to_string(pal_n32396.loaded) +
                " changes=" + std::to_string(pal_n32396.tx_changes.size()) +
                " C=" + std::to_string(pal_n32396.C) +
                " field_lines=" + std::to_string(pal_n32396.field_lines));

    // ---- FIXTURE: the fixture proves it can deliver a known value before
    // anything reads one -- the run's own marker sequence 0,2,3,4,DONE, in
    // rising cycle order.
    {
        auto m0 = mark_cycle(ntsc_n371, 0), m2 = mark_cycle(ntsc_n371, 2),
             m3 = mark_cycle(ntsc_n371, 3), m4 = mark_cycle(ntsc_n371, 4),
             md = mark_cycle(ntsc_n371, kMarkDone);
        bool seq_ok = ntsc_n371.loaded && ntsc_n371.released && m0 && m2 && m3 && m4 && md &&
                      *m0 < *m2 && *m2 < *m3 && *m3 < *m4 && *m4 < *md;
        checks.expect(seq_ok,
            "FIXTURE: the N=371 program's own MARK sequence (0,2,3,4,DONE) is present in "
            "ram_log() in rising cycle order [not graded -- apparatus check, no claim uid]");
    }

    // ==== P1: On NTSC, N+1 color clocks per bit ==========================
    {
        GridResult g0 = check_grid(ntsc_n0.tx_changes, ntsc_n0.C, 0, 1);
        GridResult g371 = check_grid(ntsc_n371.tx_changes, ntsc_n371.C, 371, 4);
        GridResult g32396 = check_grid(ntsc_n32396.tx_changes, ntsc_n32396.C, 32396, 1);
        checks.note("P1 grid: N=0 matched " + std::to_string(g0.matched) + "/" + std::to_string(g0.total_expected) +
                    " of " + std::to_string(g0.total_changes) + " changes, C=" + std::to_string(ntsc_n0.C));
        checks.note("P1 grid: N=371 matched " + std::to_string(g371.matched) + "/" + std::to_string(g371.total_expected) +
                    " of " + std::to_string(g371.total_changes) + " changes, C=" + std::to_string(ntsc_n371.C));
        checks.note("P1 grid: N=32396 matched " + std::to_string(g32396.matched) + "/" + std::to_string(g32396.total_expected) +
                    " of " + std::to_string(g32396.total_changes) + " changes, C=" + std::to_string(ntsc_n32396.C));
        checks.expect(g0.ok && g371.ok && g32396.ok,
            "P1 [p255] \"N+1 color clocks occur ... between transmissions of output bits\": on "
            "NTSC, with SERPER=N, every TxD change lands within +/-C/2 of t0+k*(N+1)*C for "
            "k=0..10W-1, and there is no other change, for N=0 (W=1), N=371 (W=4) and N=32396 (W=1)");
        checks.record(
            "sol:8-serper.c8 [p255]: withdrawn from the graded line above -- \"the number N\" "
            "covers every 15-bit divisor, and N=0, 371 and 32396 sample three of them, not the "
            "whole domain; the N=371 grid above is red (words 3-4 miss +/-C/2 of t0+k*372*C), "
            "which stands as the counterexample the claim's every-N reading would have to survive");

        // Controls, graded on the same comparator: P1's own N=371 run is
        // confirmed red (SPECIFICATION.md 4.3), which the AND above hides
        // behind N=0 and N=32396's own passing grids. Each is its own line.
        // g0/g32396 are flipped by NMH_MUTATE=P1-NPLUS1 (its own binary
        // swap covers N=0 and N=32396 too); the within-word control shares
        // ntsc_n371 with 1c/1d/1e's own contrasts, so it also flips under
        // NMH_MUTATE=P1-1C/1D/1E, not only P1-NPLUS1.
        checks.expect(g0.ok,
            "P1 control (green today) [p255]: N=0 (W=1) alone, same grid check");
        checks.expect(g32396.ok,
            "P1 control (green today) [p255]: N=32396 (W=1) alone, same grid check");
        bool within_word_ok = check_within_word(ntsc_n371.tx_changes, ntsc_n371.C, 371, 4);
        checks.expect(within_word_ok,
            "P1 control (green today) [p255]: the within-word intervals of the N=371 NTSC run (the "
            "9 bit-to-bit gaps inside each word, excluding the stop-to-next-start boundary) all "
            "land within +/-C/2 of T=(N+1)*C");
    }

    // ---- precondition 1a / R1 anchor (shared with P2, P4) ----
    {
        checks.note("1a: R1 population groups=" + std::to_string(ntsc_n371.pop_groups) +
                    " malformed=" + std::to_string(ntsc_n371.pop_malformed));
        bool ok = !ntsc_n371.refresh.empty() && check_r1_anchor(ntsc_n371.refresh, ntsc_n371.C) &&
                  ntsc_n371.pop_groups > 0 && ntsc_n371.pop_malformed == 0;
        checks.expect(ok,
            "1a [p194,p195] R1 anchor: the four refresh accesses in every line of the N=371 NTSC "
            "run are 2C, 2C and 2C apart, the same C in every line, and every candidate refresh "
            "group in the run has that same four-read shape (Figure 6-9), none discarded before "
            "grading");
    }

    // ---- precondition 1b / R2 shows NTSC ----
    {
        bool ntsc_ok = ntsc_n371.field_lines == 262 || ntsc_n371.field_lines == 263;
        checks.note("1b: NTSC capture field_lines=" + std::to_string(ntsc_n371.field_lines) +
                    ", PAL capture of the same binary field_lines=" + std::to_string(pal_n371.field_lines));
        checks.expect(ntsc_ok,
            "1b [p24] R2: the NTSC capture's own refresh-group count between two vertical syncs is "
            "262 or 263 (measured " + std::to_string(ntsc_n371.field_lines) + ")");
    }

    // ---- precondition 1c: TxD idle before the window ----
    // NMH_MUTATE=P1-1C already swapped bin_n371 above, so ntsc_n371 (and
    // pal_n371 below, sharing the same binary) *is* the mutant under that
    // env var: this same check flips PASS->FAIL, its own mutation record.
    {
        auto m0 = mark_cycle(ntsc_n371, 0);
        long long before_count = m0 ? std::count_if(ntsc_n371.tx_changes.begin(), ntsc_n371.tx_changes.end(),
                                                      [&](uint64_t c) { return c < *m0; })
                                     : -1;
        checks.note("1c: changes-before-MARK0=" + std::to_string(before_count));
        checks.expect(m0.has_value() && before_count == 0,
            "1c [p257]: TxD does not change from the start of the run until MARK=0 in the N=371 "
            "NTSC run");
    }

    // ---- precondition 1d: words sent back to back (N=371 only) ----
    // NMH_MUTATE=P1-1D swaps bin_n371 to the TSRE-polling variant.
    {
        double T = 372.0 * ntsc_n371.C;
        bool ok = true;
        for (int k = 2; k <= 4; ++k) {
            auto mk = mark_cycle(ntsc_n371, uint16_t(k));
            if (!mk) { ok = false; continue; }
            double bound = double(ntsc_n371.tx_changes.empty() ? 0 : ntsc_n371.tx_changes[0]) +
                           double(10 * (k - 1) - 1) * T;
            if (!(double(*mk) < bound)) ok = false;
        }
        checks.expect(ok,
            "1d [p257,p283]: for k=2..4, MARK=k comes before word (k-1)'s own stop-bit change in "
            "the N=371 NTSC run");
    }

    // ---- precondition 1e: the run completed (no poll ended at its bound) ----
    // NMH_MUTATE=P1-1E swaps bin_n371 to the missing-word-2 variant: word
    // 3's TBE poll then reaches BND_TBE, and POLL_TIMEOUT (set directly by
    // POLLBIT, ch8_defs.i) records that for every poll in the run -- the
    // VERTB lead-in polls and every TBE/TSRE poll alike, not just k=3's.
    {
        auto m2 = mark_cycle(ntsc_n371, 2), m3 = mark_cycle(ntsc_n371, 3);
        auto gap = (m2 && m3) ? std::optional<int64_t>(int64_t(*m3) - int64_t(*m2)) : std::nullopt;
        bool done = mark_cycle(ntsc_n371, kMarkDone).has_value();
        checks.note("1e: MARK2->MARK3 gap=" + std::to_string(gap.value_or(-1)) + " cycles, DONE present=" +
                    std::to_string(done) + ", POLL_TIMEOUT=" + std::to_string(ntsc_n371.poll_timeout));
        checks.expect(done && !ntsc_n371.poll_timeout,
            "1e [p283]: MARK=DONE is present and no poll (G4's POLLBIT, any of them) ended at its "
            "own bound in the N=371 NTSC run");
    }

    // ---- N=0 and N=32396 controls: the same per-run preconditions (R1
    // anchor+population, R2 standard, TxD idle before MARK=0, completion)
    // that 1a/1b/1c/1e grade for the N=371 run, applied to the single-word
    // runs whose own grid checks are graded above and in P2 below. (1d
    // does not apply: a W=1 run has no "next word".) ----
    auto check_run_preconditions = [&](const TxRun& r, const char* tag, bool ntsc) {
        checks.note(std::string(tag) + ": R1 population groups=" + std::to_string(r.pop_groups) +
                    " malformed=" + std::to_string(r.pop_malformed));
        bool anchor_ok = !r.refresh.empty() && check_r1_anchor(r.refresh, r.C) &&
                         r.pop_groups > 0 && r.pop_malformed == 0;
        checks.expect(anchor_ok, std::string(tag) +
            " [p194,p195] R1 anchor: the four refresh accesses in every line are 2C, 2C and 2C "
            "apart, the same C in every line, and every candidate refresh group has that same "
            "four-read shape (Figure 6-9), none discarded before grading");
        bool std_ok = ntsc ? (r.field_lines == 262 || r.field_lines == 263)
                            : (r.field_lines == 312 || r.field_lines == 313);
        checks.expect(std_ok, std::string(tag) +
            " [p24] R2: the run's own refresh-group count (" + std::to_string(r.field_lines) +
            ") matches the standard selected");
        auto m0 = mark_cycle(r, 0);
        bool idle_ok = m0.has_value() &&
                       std::none_of(r.tx_changes.begin(), r.tx_changes.end(),
                                    [&](uint64_t c) { return c < *m0; });
        checks.expect(idle_ok, std::string(tag) +
            " [p257]: TxD does not change from the start of the run until MARK=0");
        bool done = mark_cycle(r, kMarkDone).has_value();
        checks.expect(done && !r.poll_timeout, std::string(tag) +
            " [p283]: MARK=DONE is present and no poll (G4's POLLBIT, any of them) ended at its "
            "own bound");
    };
    check_run_preconditions(ntsc_n0, "P1 N=0", true);
    check_run_preconditions(ntsc_n32396, "P1 N=32396", true);
    check_run_preconditions(pal_n0, "P2 N=0", false);
    check_run_preconditions(pal_n32396, "P2 N=32396", false);

    // ---- P1's own main contrasting case: SERPER <- N+1, graded against N ----
    // NMH_MUTATE=P1-NPLUS1 swaps bin_n0/bin_n371/bin_n32396 to the N+1
    // variants above, while the P1 check() call two blocks up still grades
    // against N=0/371/32396 -- the same "P1 [p255,p284] ..." label flips.

    // ==== P2: On PAL, the bit time is one fixed period times N+1 =========
    {
        UniformResult u0 = check_uniform(pal_n0.tx_changes, pal_n0.C, 1);
        UniformResult u371 = check_uniform(pal_n371.tx_changes, pal_n371.C, 4);
        UniformResult u32396 = check_uniform(pal_n32396.tx_changes, pal_n32396.C, 1);
        checks.note("P2 uniform grid: N=0 T_N=" + std::to_string(u0.T_N) + " ok=" + std::to_string(u0.ok) +
                    "; N=371 T_N=" + std::to_string(u371.T_N) + " ok=" + std::to_string(u371.ok) +
                    "; N=32396 T_N=" + std::to_string(u32396.T_N) + " ok=" + std::to_string(u32396.ok));

        // (ii) the same period in every run: U_N = T_N/(N+1), pairwise tolerance.
        struct RunU { double U; int W; int N; };
        std::vector<RunU> us = {
            {u0.T_N / 1.0, 1, 0}, {u371.T_N / 372.0, 4, 371}, {u32396.T_N / 32397.0, 1, 32396}
        };
        bool pair_ok = true;
        std::string detail;
        for (size_t i = 0; i < us.size(); ++i)
            for (size_t j = i + 1; j < us.size(); ++j) {
                double tol = pal_n371.C * (1.0 / (double(10 * us[i].W - 1) * double(us[i].N + 1)) +
                                            1.0 / (double(10 * us[j].W - 1) * double(us[j].N + 1)));
                double diff = std::abs(us[i].U - us[j].U);
                bool ok = diff <= tol;
                if (!ok) pair_ok = false;
                detail += " |U" + std::to_string(us[i].N) + "-U" + std::to_string(us[j].N) + "|=" +
                          std::to_string(diff) + "(tol " + std::to_string(tol) + ")";
            }
        checks.note("P2(ii) pairwise U comparison:" + detail);

        // Control: the (N=0, N=32396) pair alone, excluding N=371 -- P2(ii)
        // is confirmed red only through pairs that include N=371
        // (SPECIFICATION.md 4.3); this isolates the pair that does not.
        {
            double tol_pair = pal_n371.C * (1.0 / (double(10 * us[0].W - 1) * double(us[0].N + 1)) +
                                             1.0 / (double(10 * us[2].W - 1) * double(us[2].N + 1)));
            double diff_pair = std::abs(us[0].U - us[2].U);
            checks.expect(diff_pair <= tol_pair,
                "P2(ii) control (green today) [p255]: the (N=0, N=32396) pair alone agrees within "
                "the stated tolerance (diff=" + std::to_string(diff_pair) + ", tol=" +
                std::to_string(tol_pair) + ")");
        }

        // (i) and (ii) together are what establishes "N+1 times one fixed
        // period, the same for every bit and for every N" -- (i) alone
        // fits each run's own period whatever it is, and only (ii) grades
        // the N+1 relation across runs, so both are graded on one line.
        checks.expect(u0.ok && u371.ok && u32396.ok && pair_ok,
            "P2 [p255] \"N+1 color clocks occur ... between transmissions of output bits\": on "
            "PAL, (i) each run's own uniform grid (T_N=span/(10W-1)) accounts for every TxD "
            "change with no other change, and (ii) every pair of runs' own U_N=T_N/(N+1) agree "
            "within the stated tolerance, for N=0, N=371 and N=32396");

        checks.record("P2 diagnostic (not graded): U/C = " + std::to_string(us[0].U / pal_n371.C) +
                       " (N=0), " + std::to_string(us[1].U / pal_n371.C) + " (N=371), " +
                       std::to_string(us[2].U / pal_n371.C) + " (N=32396) -- p.255 predicts 1, "
                       "p.284 predicts 0.991");

        // sol:8-serper.c9's own PAL-colour-clock value (p.255: "281.94ns on
        // PAL machines"), in the same cycle units as C -- gradable directly
        // for the single-word runs, which the N=371 word-boundary drift
        // (confirmed red above) does not touch.
        double ratio0 = us[0].U / pal_n371.C, ratio32396 = us[2].U / pal_n371.C;
        double tol0 = 1.0 / double(10 * us[0].W - 1), tol32396 = 1.0 / double(10 * us[2].W - 1);
        checks.note("U/C: N=0 ratio=" + std::to_string(ratio0) + " (tol " +
                    std::to_string(tol0) + "), N=32396 ratio=" + std::to_string(ratio32396) +
                    " (tol " + std::to_string(tol32396) + ")");
        checks.expect(std::abs(ratio0 - 1.0) <= tol0 && std::abs(ratio32396 - 1.0) <= tol32396,
            "P2 [p255]: on PAL, the fixed period equals one PAL colour clock (U_N/C = 1 within "
            "+/-1/(10W-1)), for N=0 and N=32396");

        checks.record(
            "sol:8-serper.c9 [p255]: withdrawn from the graded lines above -- \"the number N\" "
            "covers every 15-bit divisor, and N=0, 371 and 32396 sample three of them, not the "
            "whole domain; the N=371 pairwise comparison above is red (|U371-U32396|=" +
            std::to_string(std::abs(us[1].U - us[2].U)) + " exceeds its own tolerance), which "
            "stands as the counterexample the claim's every-N reading would have to survive");
    }

    // ---- P2 preconditions 2a-2e: 1a-1e restated on the PAL run ----
    // (2a/2b are comparator-only, like 1a/1b; 2c/2d/2e's own mutations are
    // the same NMH_MUTATE=P1-1C/1D/1E swap of bin_n371, shared with 1c/1d/1e
    // since pal_n371 is built from that same binary)
    {
        checks.note("2a: R1 population groups=" + std::to_string(pal_n371.pop_groups) +
                    " malformed=" + std::to_string(pal_n371.pop_malformed));
        bool ok = !pal_n371.refresh.empty() && check_r1_anchor(pal_n371.refresh, pal_n371.C) &&
                  pal_n371.pop_groups > 0 && pal_n371.pop_malformed == 0;
        checks.expect(ok, "2a [p194,p195] R1 anchor on the PAL N=371 run: the four refresh "
            "accesses in every line are 2C, 2C and 2C apart, and every candidate refresh group "
            "has that same shape (Figure 6-9), none discarded before grading");

        bool pal_ok = pal_n371.field_lines == 312 || pal_n371.field_lines == 313;
        checks.expect(pal_ok,
            "2b [p24] R2: the PAL capture's own refresh-group count is 312 or 313 (measured " +
            std::to_string(pal_n371.field_lines) + ")");

        auto m0 = mark_cycle(pal_n371, 0);
        checks.expect(m0.has_value() &&
                      std::none_of(pal_n371.tx_changes.begin(), pal_n371.tx_changes.end(),
                                   [&](uint64_t c) { return c < *m0; }),
            "2c [p257]: TxD idle before MARK=0, PAL N=371 run");

        double T = 372.0 * pal_n371.C;
        bool d_ok = true;
        for (int k = 2; k <= 4; ++k) {
            auto mk = mark_cycle(pal_n371, uint16_t(k));
            if (!mk) { d_ok = false; continue; }
            double bound = double(pal_n371.tx_changes.empty() ? 0 : pal_n371.tx_changes[0]) +
                           double(10 * (k - 1) - 1) * T;
            if (!(double(*mk) < bound)) d_ok = false;
        }
        checks.expect(d_ok, "2d [p257,p283]: words sent back to back, PAL N=371 run");

        bool done2 = mark_cycle(pal_n371, kMarkDone).has_value();
        checks.note("2e: POLL_TIMEOUT=" + std::to_string(pal_n371.poll_timeout));
        checks.expect(done2 && !pal_n371.poll_timeout,
            "2e [p283]: run completed, no poll (G4's POLLBIT, any of them) ended at its own bound, "
            "PAL N=371 run");
    }

    // P2's own contrasting case (NMH_MUTATE=P1-NPLUS1, SERPER<-372 graded as
    // N=371) needs no separate code: pal_n371 is already the +1 mutant
    // under that env var, and the P2 block above still divides its T_N by
    // 372.0 (the N=371 slot's own N+1), so the same "P2 ..." label flips
    // on its own -- U_371 becomes 373*C/372 instead of C, exactly the "off
    // by a factor 373/372" the page predicts. The same mutation also
    // flips the sol:8-serper.c9 U/C=1 check via pal_n0/pal_n32396's own
    // +1 binaries (U/C = 2.000 for N=0, per the replay table).

    // ==== P3: SERPER=371 on NTSC is the setting p.255 calls 9600 baud =====
    // Reuses ntsc_n371 (P1's own run): NMH_MUTATE=P1-NPLUS1 (SERPER<-372,
    // graded as N=371) and P1-1C/1D/1E all flip this same "P3 ..." label
    // exactly as they flip "P1 ...", since both grade the identical capture
    // with the identical check_grid(...,371,4) call.
    {
        GridResult g = check_grid(ntsc_n371.tx_changes, ntsc_n371.C, 371, 4);
        checks.note("P3: reusing P1's N=371 NTSC run, matched " + std::to_string(g.matched) + "/" +
                    std::to_string(g.total_expected));
        checks.expect(g.ok,
            "P3 [p255] \"N+1 color clocks occur ... between transmissions of output bits\": "
            "every change of the N=371 NTSC run lies within +/-C/2 of t0+k*372*C for k=0..39");
        checks.record(
            "sol:8-serper.c10 [p255]: withdrawn from the graded line above -- the claim reads 371 "
            "as the proper NTSC setting for 9600 baud, but the rate = 3,579,545/372 = 9622.4 "
            "bit/s; the page's own arithmetic (3,579,545/9600-1=371.87, printed 371) makes its "
            "\"9600 baud\" actually 9622 bit/s, so 9600 itself is not graded (only the p.255 "
            "N+1 grid above is), and that grid is red at N=371 (words 3-4 miss +/-C/2 of "
            "t0+k*372*C)");

        // Preconditions 3a-3e are 1a-1e, already graded above on this same run.
        checks.record("3a-3e [p194-p257]: preconditions 1a-1e, already graded above on this same "
                       "N=371 NTSC run (P3's own stimulus and observation)");
    }

    // ==== P4: ADKCON FAST sets the disk bit cell to 2us or 4us ============
    run_p4(dir, mv, checks);

    // ==== P5: the nine-bit receive setting takes eight data bits plus a
    // parity bit ============================================================
    run_p5(dir, mv, checks);

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
