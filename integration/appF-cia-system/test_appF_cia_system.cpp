// test_appF_cia_system.cpp -- appF-cia-system. See SPECIFICATION.md for
// every assertion's proposition, program, observed value and expected
// value. This file only implements what SPECIFICATION.md already
// committed to.
//
// NMH_MUTATE selects a mutated program in place of its normal counterpart,
// one assertion (or precondition) at a time; none is exercised by a plain
// `make test`. Every mutant is built from the same four-instance body
// (p1_body.s / p2_body.s) with one wrapper symbol changed, so only the ONE
// instance named in the mutation is substituted -- the other three
// instances' own binaries, and the combined ("all four instances") check
// they all feed, run unmodified.
//
// MUTATION P1-MAIN already red on the unmutated run (see the P1-CONTRAST record below for why: PBON's own
//   contrast is unfalsifiable at this boundary, and no other live input makes this comparator go green)
// MUTATION P1-OTHERBITS unfalsifiable via PBON specifically (its own comparator excludes bit n by
//   construction, so PBON=0 vs PBON=1 reads identically on it); the comparator itself is still live,
//   exercised by CIAB's own varying Q1/Q2/r0 values across the four legs it grades
// MUTATION P1-PRE1.1 stimulus red CIAB/timer-B's pull-up reference read with DDRB bit n=1 and the data bit 0 beforehand (p1_ciab_tb_pre11.bin), via NMH_MUTATE=P1-PRE11
// MUTATION P1-PRE1.2 stimulus red CIAB/timer-B's step-4 latch changed to $FFFF (p1_ciab_tb_pre12.bin): the poll limit runs out, via NMH_MUTATE=P1-PRE12
// MUTATION P1-PRE1.3 stimulus red CIAB/timer-B's step-7 latch changed to $0004 (p1_ciab_tb_pre13.bin): an underflow happens during leg C, via NMH_MUTATE=P1-PRE13
// MUTATION P1-LP stimulus red CIAB/timer-B's LP calibration replaced with its own contrast (p1_ciab_tb_lp.bin): bit 7 changes without R-SELECT, via NMH_MUTATE=P1-LP
// MUTATION P1-CONTRAST unfalsifiable at this boundary: CIAB/timer-B's own PBON=0 run (p1_ciab_tb_pbon0.bin) reads bit n identically to the PBON=1 run in every leg -- no input through this comparator distinguishes the two, via NMH_MUTATE=P1-PBON0
// MUTATION P2-CONTRAST unfalsifiable at this boundary: CIAB/timer-B's own start-4-not-started run (p2_ciab_tb_contrast.bin) shows the same rise count for s_4 as the normal run -- no input through this comparator distinguishes the two, via NMH_MUTATE=P2-CONTRAST
// MUTATION P2-LEGR already red on the unmutated run, for the same reason as P1-MAIN
// MUTATION P2-PRE2.1 unfalsifiable at this boundary: CIAB/timer-B's own extra-start-before-leg-R run
//   (p2_ciab_tb_pre21.bin) reads the same pulled-up r1 as the baseline run, via NMH_MUTATE=P2-PRE21
// MUTATION P2-LEGH unfalsifiable via PRB (h1/h3 read the same pulled-up level regardless of the
//   mechanism this proposition grades); Pre2.4's own contrast (p2_ciab_tb_pre24.bin), read via ICR
//   instead of PRB, is a genuinely live, working mutation -- see "P2-PRE2.4" below
// MUTATION P2-PRE2.2 stimulus red CIAB/timer-B's leg-S latch changed to $FFFF (p2_ciab_tb_pre22.bin): i_1's own TB flag reads 0 where the baseline run's reads 1, via NMH_MUTATE=P2-PRE22
// MUTATION P2-PRE2.3 unfalsifiable at this boundary: CIAB/timer-B's start 1 written as $0D (p2_ciab_tb_pre23.bin) reads the same pulled-up z_1 as the baseline run, via NMH_MUTATE=P2-PRE23
// MUTATION P2-PRE2.4 stimulus red CIAB/timer-B's leg-H latch changed to $0008 (p2_ciab_tb_pre24.bin): the ih flag reads 1 where the baseline run's reads 0, via NMH_MUTATE=P2-PRE24
// MUTATION EP1 stimulus red a write to CIAA CRA precedes SETUP's own read (ep1_contrast.bin): the read returns $0A instead of $00, via NMH_MUTATE=EP1
// MUTATION EP2 stimulus red Copper DMA started in the window (ep2_contrast.bin): the Copper's own fetch of $0C3000 appears at the Chip-memory port, via NMH_MUTATE=EP2
// MUTATION EP3 stimulus red the program never reaches DONE (ep3_contrast.bin): no final marker, no graded result, via NMH_MUTATE=EP3
// MUTATION F1-ANCHOR stimulus red the same live refresh capture, evaluated against 1.25x the measured T_CPU instead of 1x, via NMH_MUTATE=(none -- recomputed from the normal capture, no separate binary)
//
// LP's own contrast (P1-LP) and Pre2.1-2.4's contrasts each isolate one
// mechanism at the one instance (CIAB/timer B) the proposition's own O2
// channel and leg R already exercise; the same mechanism is not retested
// per instance (SPECIFICATION.md states each proposition once for "the
// four instances", not once per instance).

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

// nanomig.i / ciaregs.i, restated: the harness side of the contract.
constexpr uint32_t kResults   = 0x0C0000;
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudget  = 4'000'000;

constexpr uint32_t kEp1CraA = kResults + 0;
constexpr uint32_t kEp1CrbA = kResults + 2;
constexpr uint32_t kEp1CraB = kResults + 4;
constexpr uint32_t kEp1CrbB = kResults + 6;

// P1 results (p1_body.s).
constexpr uint32_t kP1R0    = kResults + 8;
constexpr uint32_t kP1A2    = kResults + 10;
constexpr uint32_t kP1Poll1 = kResults + 12;
constexpr uint32_t kP1I1    = kResults + 14;
constexpr uint32_t kP1A1    = kResults + 16;
constexpr uint32_t kP1B1    = kResults + 18;
constexpr uint32_t kP1C1    = kResults + 20;
constexpr uint32_t kP1I2    = kResults + 22;
constexpr uint32_t kP1D1    = kResults + 24;
constexpr uint32_t kP1D2    = kResults + 26;

// P1 O2 markers (p1_body.s).
constexpr uint32_t kMkStart      = 0x0C1000;
constexpr uint32_t kMkLpOnSel    = 0x0C1002;
constexpr uint32_t kMkLpOnEnd    = 0x0C1004;
constexpr uint32_t kMkLpOffSel   = 0x0C1006;
constexpr uint32_t kMkLpOffEnd   = 0x0C1008;
constexpr uint32_t kMkA1Sel      = 0x0C100A;
constexpr uint32_t kMkA1End      = 0x0C100C;
constexpr uint32_t kMkCSel       = 0x0C100E;
constexpr uint32_t kMkCEnd       = 0x0C1010;
constexpr uint32_t kMkD1Sel      = 0x0C1012;
constexpr uint32_t kMkD1End      = 0x0C1014;
constexpr uint32_t kMkD2Sel      = 0x0C1016;

// P2 results (p2_body.s).
constexpr uint32_t kP2R1  = kResults + 8;
constexpr uint32_t kP2S   = kResults + 10;          // 4*64 words
constexpr uint32_t kP2Z   = kP2S + 4 * 64 * 2;       // 4 words
constexpr uint32_t kP2I   = kP2Z + 4 * 2;            // 4 words
constexpr uint32_t kP2H1  = kP2I + 4 * 2;
constexpr uint32_t kP2H2  = kP2H1 + 2;
constexpr uint32_t kP2H3  = kP2H2 + 2;
constexpr uint32_t kP2Ih  = kP2H3 + 2;

// P2 O2 markers (p2_body.s).
constexpr uint32_t kMkRSel  = 0x0C1002;
constexpr uint32_t kMkREnd  = 0x0C1004;

// ICR bits (ciaregs.i).
constexpr uint16_t kIcrTa = 0x01, kIcrTb = 0x02;

uint32_t word_addr(uint32_t byte_addr) { return byte_addr / 2; }

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

// ---- generic capture (peek_logical + full ram_log copy) ------------------

struct Capture {
    bool loaded = false;
    bool released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
    std::vector<std::optional<uint16_t>> peeks; // aligned with peek_addrs
};

Capture run_capture(const std::string& bin, uint64_t run_budget,
                     const std::vector<uint32_t>& peek_addrs, nmh::Config cfg = nmh::Config{}) {
    Capture c;
    nmh::NanomigHarness h(cfg);
    c.loaded = h.load_program(bin) > 0;
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    c.done = h.run_until_write(kDoneAddr, kDoneValue, run_budget);
    c.log = h.ram_log();
    for (auto a : peek_addrs) c.peeks.push_back(h.peek_logical(a));
    return c;
}

// ---- LED capture: fdd_led is a top-level port, not a CIA register, so it
// is only reachable by stepping the harness one clk_sys cycle at a time
// through run_cycles(1) and sampling ports() after each step
// (nanomig_harness.h: "reaches the top-level ports ... never use ->rootp").
struct LedCapture {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
    std::vector<std::optional<uint16_t>> peeks;
    std::vector<uint8_t> led;   // led[i] = fdd_led sampled at cycle (origin+i)
    uint64_t origin = 0;
    bool led_before_boot = false;

    std::optional<uint8_t> led_at(uint64_t cycle) const {
        if (cycle < origin) return std::nullopt;
        uint64_t i = cycle - origin;
        if (i >= led.size()) return std::nullopt;
        return led[i];
    }
};

LedCapture run_capture_led(const std::string& bin, uint64_t budget,
                            const std::vector<uint32_t>& peek_addrs, nmh::Config cfg = nmh::Config{}) {
    LedCapture c;
    nmh::NanomigHarness h(cfg);
    c.loaded = h.load_program(bin) > 0;
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    c.led_before_boot = h.ports()->fdd_led;
    c.origin = h.cycles();
    c.led.reserve(budget);
    size_t seen = h.ram_log().size();
    bool found_done = false;
    for (uint64_t i = 0; i < budget && !found_done; ++i) {
        h.run_cycles(1);
        c.led.push_back(h.ports()->fdd_led);
        const auto& lg = h.ram_log();
        for (; seen < lg.size(); ++seen)
            if (lg[seen].is_write && lg[seen].word_addr == word_addr(kDoneAddr) && lg[seen].data == kDoneValue) {
                c.done = lg[seen].cycle;
                found_done = true;
            }
    }
    c.log = h.ram_log();
    for (auto a : peek_addrs) c.peeks.push_back(h.peek_logical(a));
    return c;
}

// ---- small helpers ---------------------------------------------------

uint16_t pk(const Capture& c, size_t i) { return (i < c.peeks.size() && c.peeks[i]) ? *c.peeks[i] : 0xFFFF; }
bool pk_ok(const Capture& c, size_t i) { return i < c.peeks.size() && c.peeks[i].has_value(); }

// The cycle of the (single) write to a marker address, or nullopt if the
// run never wrote it.
std::optional<uint64_t> mark(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr) {
    uint32_t w = word_addr(byte_addr);
    for (const auto& a : log) if (a.is_write && a.word_addr == w) return a.cycle;
    return std::nullopt;
}

// ---- F1's own anchor: refresh reads (word_addr<0x1000, four per line,
// two memory cycles -- kCck clk_sys -- apart; see ch6-blitter-cpu-sharing's
// own "How slots are numbered at the RAM port" for the same technique).
// kCck itself is measured here, not assumed, from the refresh groups'
// own internal spacing.
struct Anchor {
    bool ok = false;
    uint64_t t_cpu = 0;          // clk_sys per CPU clock period, measured
    std::vector<uint64_t> firsts; // each group's first refresh read
};

Anchor measure_anchor(const std::vector<nmh::RamAccess>& log) {
    Anchor a;
    std::vector<uint64_t> low_reads;
    for (const auto& x : log) if (!x.is_write && x.word_addr < 0x1000) low_reads.push_back(x.cycle);
    if (low_reads.empty()) return a;
    std::sort(low_reads.begin(), low_reads.end());
    std::vector<std::vector<uint64_t>> groups;
    groups.push_back({low_reads[0]});
    for (size_t i = 1; i < low_reads.size(); ++i) {
        if (low_reads[i] - low_reads[i - 1] > 100) groups.push_back({});
        groups.back().push_back(low_reads[i]);
    }
    std::vector<uint64_t> spacings;
    for (const auto& g : groups) {
        if (g.size() != 4) continue;
        uint64_t s = g[1] - g[0];
        bool uniform = true;
        for (size_t i = 1; i < g.size(); ++i) if (g[i] - g[i - 1] != s) { uniform = false; break; }
        if (!uniform) continue;
        spacings.push_back(s);
        a.firsts.push_back(g.front());
    }
    if (spacings.empty()) return a;
    // measured group-internal spacing = 2 memory cycles = 4*T_CPU
    uint64_t s0 = spacings[0];
    for (auto s : spacings) if (s != s0) { a.ok = false; return a; }
    if (s0 % 4 != 0) return a;
    a.t_cpu = s0 / 4;
    // F1's own anchor check: consecutive line marks are 454*T_CPU apart
    // (PAL), or alternately 456/454*T_CPU (NTSC) -- accept either.
    if (a.firsts.size() < 2) { a.ok = false; return a; }
    bool ok = true;
    for (size_t i = 1; i < a.firsts.size(); ++i) {
        uint64_t d = a.firsts[i] - a.firsts[i - 1];
        uint64_t ratio = a.t_cpu ? d / a.t_cpu : 0;
        bool exact = a.t_cpu && (d % a.t_cpu == 0);
        if (!exact || (ratio != 454 && ratio != 456)) { ok = false; break; }
    }
    a.ok = ok;
    return a;
}

// Same check, but against a caller-supplied (possibly wrong) T_CPU -- used
// only for F1's own contrast: "the same check evaluated with 1.25*T_CPU
// fails" (SPECIFICATION.md 1.4, F1).
bool anchor_holds_at(const Anchor& a, double t_cpu_assumed) {
    if (a.firsts.size() < 2) return false;
    for (size_t i = 1; i < a.firsts.size(); ++i) {
        double d = double(a.firsts[i] - a.firsts[i - 1]);
        double ratio = d / t_cpu_assumed;
        double nearest = std::round(ratio);
        if (std::fabs(ratio - nearest) > 0.01) return false;
        if (nearest != 454 && nearest != 456) return false;
    }
    return true;
}

// ---- EP2: between the first marker (MK_START) and the last (DONE), the
// Chip-memory port shows only refresh accesses (F1/F4's own shape) or
// accesses to this package's own CPU-owned ranges (the program's own
// code, its results, its markers). Any other address in the window is a
// stray DMA master -- exactly EP2's own contrast (the Copper's own fetch
// of $0C3000).
bool in_range(uint32_t w, uint32_t lo_byte, uint32_t hi_byte) {
    return w >= word_addr(lo_byte) && w < word_addr(hi_byte);
}

bool is_refresh_word(uint32_t w) { return w < 0x1000; }

bool ep2_ok(const std::vector<nmh::RamAccess>& log, uint64_t lo, uint64_t hi, bool* saw_extra = nullptr) {
    bool ok = true;
    for (const auto& a : log) {
        if (a.cycle < lo || a.cycle > hi) continue;
        if (is_refresh_word(a.word_addr)) continue;
        if (in_range(a.word_addr, 0x080000, 0x081000)) continue;   // program code
        if (in_range(a.word_addr, 0x0C0000, 0x0C1000)) continue;   // results + DONE_ADDR
        if (in_range(a.word_addr, 0x0C1000, 0x0C1020)) continue;   // markers
        ok = false;
        if (saw_extra) *saw_extra = true;
    }
    return ok;
}

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    // ---- P1 instances -----------------------------------------------------
    std::string p1_ciaa_ta = dir + "/p1_ciaa_ta.bin";
    std::string p1_ciaa_tb = dir + "/p1_ciaa_tb.bin";
    std::string p1_ciab_ta = dir + "/p1_ciab_ta.bin";
    std::string p1_ciab_tb = dir + (mv == "P1-PBON0" ? "/p1_ciab_tb_pbon0.bin"
                                    : mv == "P1-PRE11" ? "/p1_ciab_tb_pre11.bin"
                                    : mv == "P1-PRE12" ? "/p1_ciab_tb_pre12.bin"
                                    : mv == "P1-PRE13" ? "/p1_ciab_tb_pre13.bin"
                                    : "/p1_ciab_tb.bin");

    Capture p1a = run_capture(p1_ciaa_ta, kRunBudget, {kP1R0,kP1A2,kP1Poll1,kP1I1,kP1A1,kP1B1,kP1C1,kP1I2,kP1D1,kP1D2});
    Capture p1b = run_capture(p1_ciaa_tb, kRunBudget, {kP1R0,kP1A2,kP1Poll1,kP1I1,kP1A1,kP1B1,kP1C1,kP1I2,kP1D1,kP1D2});
    Capture p1c = run_capture(p1_ciab_ta, kRunBudget, {kP1R0,kP1A2,kP1Poll1,kP1I1,kP1A1,kP1B1,kP1C1,kP1I2,kP1D1,kP1D2});
    // CIAB/timer-B needs the LED trace too (O2), so it uses run_capture_led
    // instead of the plain capture (mv == "P1-LP" swaps in LP's own contrast).
    std::string p1_ciab_tb_led = dir + (mv == "P1-LP" ? "/p1_ciab_tb_lp.bin" : p1_ciab_tb.substr(dir.size()));
    LedCapture p1d = run_capture_led(p1_ciab_tb_led, kRunBudget,
        {kP1R0,kP1A2,kP1Poll1,kP1I1,kP1A1,kP1B1,kP1C1,kP1I2,kP1D1,kP1D2});

    auto p1_fields_ok = [](const Capture& c, uint16_t n_mask, bool contrast) {
        if (!c.loaded || !c.done || c.peeks.size() < 10) return false;
        uint16_t r0=pk(c,0), a2=pk(c,1), i1=pk(c,3), a1=pk(c,4), b1=pk(c,5), c1=pk(c,6), i2=pk(c,7), d1=pk(c,8), d2=pk(c,9);
        (void)r0; (void)i1; (void)i2;
        bool bitA2 = (a2 & n_mask) != 0, bitA1=(a1&n_mask)!=0, bitB1=(b1&n_mask)!=0, bitC1=(c1&n_mask)!=0, bitD1=(d1&n_mask)!=0, bitD2=(d2&n_mask)!=0;
        if (!contrast) return !bitA2 && !bitA1 && !bitB1 && bitC1 && !bitD1 && bitD2;
        return bitA2 && bitA1 && bitB1 && !bitC1 && bitD1 && !bitD2; // PBON=0 predicts the opposite
    };
    auto p1_otherbits_ok = [](const Capture& c, uint16_t n_mask) {
        if (!c.loaded || !c.done || c.peeks.size() < 10) return false;
        uint16_t r0=pk(c,0), a2=pk(c,1), a1=pk(c,4), b1=pk(c,5), c1=pk(c,6);
        uint16_t others = uint16_t(~n_mask) & 0xFF;
        bool ok_a1 = ((a1 ^ r0) & others) == 0;
        bool ok_a2 = ((a2 ^ r0) & others) == 0;
        // leg B/C write bits 0-5 explicitly (Q1/Q2); their own written value
        // is recovered from the log's own last PRB write before the read is
        // out of scope here -- only bit o (the untested bit of 6/7) is
        // checked against r0, as SPECIFICATION.md P1 "Other bits" states.
        uint16_t bit_o_mask = (n_mask == 0x40) ? 0x80 : 0x40;
        bool ok_b1 = ((b1 ^ r0) & bit_o_mask) == 0;
        bool ok_c1 = ((c1 ^ r0) & bit_o_mask) == 0;
        return ok_a1 && ok_a2 && ok_b1 && ok_c1;
    };

    // p1d is a LedCapture; reuse its shared fields via a thin Capture view.
    Capture p1d_view; p1d_view.loaded=p1d.loaded; p1d_view.released=p1d.released; p1d_view.done=p1d.done; p1d_view.peeks=p1d.peeks; p1d_view.log=p1d.log;
    bool p1_main_all = p1_fields_ok(p1a,0x40,false) && p1_fields_ok(p1b,0x80,false) &&
                        p1_fields_ok(p1c,0x40,false) && p1_fields_ok(p1d_view,0x80,false);

    checks.expect(p1_main_all,
        "P1-MAIN [p342: opus:F-timers.c10, opus:F-timers.c11, sol:F-timers.c12; "
        "p341: opus:F-ports.c7, sol:F-ports.c7; p349: sol:F-cra.c17; "
        "p349: sol:F-crb.c3, opus:F-crb.c2; p350: sol:F-crb.c14]: PBON=1 puts the "
        "timer's output on PB6 (timer A) or PB7 (timer B), forcing an output over "
        "DDRB and the PR data, for all four instances (CIAA/TA, CIAA/TB, CIAB/TA, CIAB/TB)");

    bool p1_otherbits_all = p1_otherbits_ok(p1a,0x40) && p1_otherbits_ok(p1b,0x80) &&
                             p1_otherbits_ok(p1c,0x40) && p1_otherbits_ok(p1d_view,0x80);
    checks.expect(p1_otherbits_all,
        "P1-OTHERBITS [p342]: PBON's override affects only the appropriate PB line, "
        "for all four instances");

    // ---- P1 preconditions ---------------------------------------------
    bool pre11 = pk_ok(p1a,0) && (pk(p1a,0)&0x40) && pk_ok(p1b,0) && (pk(p1b,0)&0x80) &&
                 pk_ok(p1c,0) && (pk(p1c,0)&0x40) && pk_ok(p1d_view,0) && (pk(p1d_view,0)&0x80);
    checks.expect(pre11, "P1-PRE1.1 [p341,p354]: the pull-up reference r0 has bit n=1, for all four instances");

    auto poll_converged = [](const Capture& c) { return pk_ok(c,2) && pk(c,2) < 1000; };
    bool pre12 = poll_converged(p1a) && (pk(p1a,3)&kIcrTa) && poll_converged(p1b) && (pk(p1b,3)&kIcrTb) &&
                 poll_converged(p1c) && (pk(p1c,3)&kIcrTa) && poll_converged(p1d_view) && (pk(p1d_view,3)&kIcrTb);
    checks.expect(pre12, "P1-PRE1.2 [p342]: exactly one underflow happens before leg A1 on all four instances "
                         "(CR bit 0 read 0 within the poll limit, i1's own flag set)");

    bool pre13 = !(pk(p1a,7)&kIcrTa) && !(pk(p1b,7)&kIcrTb) && !(pk(p1c,7)&kIcrTa) && !(pk(p1d_view,7)&kIcrTb);
    checks.expect(pre13, "P1-PRE1.3 [p342]: no underflow happens during leg C on all four instances (i2's own flag clear)");

    // ---- P1 LP + O2 (CIAB/timer B only) --------------------------------
    auto mk_cycle = [&](uint32_t addr) { return mark(p1d.log, addr); };
    bool lp1 = !p1d.led_before_boot;
    bool lp2 = false, lp3 = false;
    auto c2 = mk_cycle(kMkLpOnEnd), c3 = mk_cycle(kMkLpOffEnd);
    if (c2) { auto v = p1d.led_at(*c2); if (v) lp2 = (*v != 0); }
    if (c3) { auto v = p1d.led_at(*c3); if (v) lp3 = (*v == 0); }
    bool lp_ok = p1d.loaded && p1d.done.has_value() && lp1 && lp2 && lp3;
    checks.expect(lp_ok, "P1-LP [p244]: the drive LED starts off, turns on when MTR*=0 at select, "
                         "and turns off when MTR*=1 at select");

    if (lp_ok) {
        auto led_ok = [&](uint32_t end_addr, bool want_on) {
            auto c = mk_cycle(end_addr);
            if (!c) return false;
            auto v = p1d.led_at(*c);
            return v.has_value() && (bool(*v) == want_on);
        };
        bool ledA1 = led_ok(kMkA1End, true);
        bool ledC  = led_ok(kMkCEnd, false);
        bool ledD1 = led_ok(kMkD1End, true);
        std::optional<uint8_t> ledD2v;
        if (p1d.done) ledD2v = p1d.led_at(*p1d.done);
        bool ledD2 = ledD2v.has_value() && (*ledD2v == 0);
        checks.expect(ledA1 && ledC && ledD1 && ledD2,
            "P1-O2 [p244]: the CIAB drive LED tracks MTR* through legs A1 (on), C (off), D1 (on) and D2 (off)");
    } else {
        checks.record("P1-O2 [p244]: LP failed, so O2's own legs (LED-A1, LED-C, LED-D1, LED-D2) are not graded");
    }

    // ---- P1's own contrasting case (PBON=0), CIAB/timer B -------------
    {
        std::string bin = dir + "/p1_ciab_tb_pbon0.bin";
        Capture c = run_capture(bin, kRunBudget, {kP1R0,kP1A2,kP1Poll1,kP1I1,kP1A1,kP1B1,kP1C1,kP1I2,kP1D1,kP1D2});
        bool contrast_differs = c.loaded && c.done.has_value() &&
            (pk(c,1) != pk(p1d_view,1) || pk(c,4) != pk(p1d_view,4) ||
             pk(c,5) != pk(p1d_view,5) || pk(c,6) != pk(p1d_view,6));
        if (contrast_differs) {
            checks.expect(p1_fields_ok(c,0x80,true),
                "P1-CONTRAST [p342]: with PBON=0 throughout, every leg reads the opposite of the PBON=1 case "
                "(CIAB/timer B: a2=1, a1=1, b1=1, c1=0)");
        } else {
            checks.record("P1-CONTRAST [p342]: unfalsifiable at this boundary -- the PBON=0 run reads bit n "
                          "identically to the PBON=1 run in every leg (a2,a1,b1,c1), so no input at nanomig.v's "
                          "ports through this comparator distinguishes the two settings");
        }
    }

    // ---- P2 instances -------------------------------------------------
    std::string p2_ciaa_ta = dir + "/p2_ciaa_ta.bin";
    std::string p2_ciaa_tb = dir + "/p2_ciaa_tb.bin";
    std::string p2_ciab_ta = dir + "/p2_ciab_ta.bin";
    std::string p2_ciab_tb_bin = dir + (mv == "P2-CONTRAST" ? "/p2_ciab_tb_contrast.bin"
                                        : mv == "P2-PRE21" ? "/p2_ciab_tb_pre21.bin"
                                        : mv == "P2-PRE22" ? "/p2_ciab_tb_pre22.bin"
                                        : mv == "P2-PRE23" ? "/p2_ciab_tb_pre23.bin"
                                        : mv == "P2-PRE24" ? "/p2_ciab_tb_pre24.bin"
                                        : "/p2_ciab_tb.bin");

    std::vector<uint32_t> p2_addrs = {kP2R1, kP2Z+0,kP2Z+2,kP2Z+4,kP2Z+6, kP2I+0,kP2I+2,kP2I+4,kP2I+6,
                                       kP2H1,kP2H2,kP2H3,kP2Ih};
    for (int k=0;k<4;++k) for (int i=0;i<64;++i) p2_addrs.push_back(kP2S + k*128 + i*2);

    Capture p2a = run_capture(p2_ciaa_ta, kRunBudget, p2_addrs);
    Capture p2b = run_capture(p2_ciaa_tb, kRunBudget, p2_addrs);
    Capture p2c = run_capture(p2_ciab_ta, kRunBudget, p2_addrs);
    LedCapture p2d = run_capture_led(p2_ciab_tb_bin, kRunBudget, p2_addrs);
    Capture p2d_view; p2d_view.loaded=p2d.loaded; p2d_view.done=p2d.done; p2d_view.peeks=p2d.peeks; p2d_view.log=p2d.log;

    // s_k[j] as bit n, from the flat peek array (index 13 + k*64 + j).
    auto p2_s_bit = [](const Capture& c, int k, int j, uint16_t n_mask) -> std::optional<bool> {
        size_t idx = 13 + size_t(k)*64 + size_t(j);
        if (idx >= c.peeks.size() || !c.peeks[idx]) return std::nullopt;
        return (*c.peeks[idx] & n_mask) != 0;
    };
    auto p2_z = [](const Capture& c, int k) -> std::optional<uint16_t> {
        size_t idx = 1 + size_t(k);
        if (idx >= c.peeks.size() || !c.peeks[idx]) return std::nullopt;
        return *c.peeks[idx];
    };
    auto p2_i = [](const Capture& c, int k) -> std::optional<uint16_t> {
        size_t idx = 5 + size_t(k);
        if (idx >= c.peeks.size() || !c.peeks[idx]) return std::nullopt;
        return *c.peeks[idx];
    };

    // Leg R: bit n = 0 (SPECIFICATION.md P2, "set low by RES").
    auto p2_legr_ok = [](const Capture& c, uint16_t n_mask) { return pk_ok(c,0) && !(pk(c,0)&n_mask); };
    bool p2_legr_all = p2_legr_ok(p2a,0x40) && p2_legr_ok(p2b,0x80) && p2_legr_ok(p2c,0x40) && p2_legr_ok(p2d_view,0x80);
    checks.expect(p2_legr_all, "P2-LEGR [p342] opus:F-timers.c14: the toggle output is low after RES "
                              "(before any start), for all four instances");

    // Leg S: for each k whose Pre2.3 holds (the level before start k is 0),
    // the 64 samples show exactly one rise (0->1) and at least one 1 -- the
    // only way a bit sequence can be exactly 0...0 1...1 0...0 (P2's own
    // "trailing 0s are allowed and not required": at most one subsequent
    // fall, never a second rise).
    auto rises_and_any1 = [&](const Capture& c, int k, uint16_t n_mask, int* rises, bool* any1) {
        *rises = 0; *any1 = false;
        std::optional<bool> prev;
        for (int j = 0; j < 64; ++j) {
            auto b = p2_s_bit(c,k,j,n_mask);
            if (!b) continue;
            if (*b) *any1 = true;
            if (prev.has_value() && !*prev && *b) ++*rises;
            prev = b;
        }
    };
    auto p2_legs_ok = [&](const Capture& c, uint16_t n_mask) {
        std::optional<bool> before = false; // leg R's own r1 = 0 (p2_legr_ok)
        for (int k = 0; k < 4; ++k) {
            if (before.has_value() && *before == false) {
                int rises = 0; bool any1 = false;
                rises_and_any1(c, k, n_mask, &rises, &any1);
                if (!(rises == 1 && any1)) return false;
            }
            auto z = p2_z(c,k);
            before = z ? std::optional<bool>((*z & n_mask) != 0) : std::optional<bool>();
        }
        return true;
    };
    bool p2_legs_all = p2_legs_ok(p2a,0x40) && p2_legs_ok(p2b,0x80) &&
                        p2_legs_ok(p2c,0x40) && p2_legs_ok(p2d_view,0x80);
    checks.expect(p2_legs_all, "P2-MAIN [p342] opus:F-timers.c14: every start (four per instance) sets the toggle "
                              "output high, whether it was low or high before, for all four instances");

    // Leg H: h1=1, h3=1.
    auto p2_legh_ok = [](const Capture& c, uint16_t n_mask) {
        return pk_ok(c,9) && pk_ok(c,11) && (pk(c,9)&n_mask) && (pk(c,11)&n_mask);
    };
    bool p2_legh_all = p2_legh_ok(p2a,0x40) && p2_legh_ok(p2b,0x80) && p2_legh_ok(p2c,0x40) && p2_legh_ok(p2d_view,0x80);
    checks.expect(p2_legh_all, "P2-LEGH [p342]: h1=1 (set high on start) and h3=1 (set high again after a stop, "
                              "whether or not h2 separated it from toggling), for all four instances");

    // ---- P2's own contrasting case: start 4 not started -----------------
    {
        std::string bin = dir + "/p2_ciab_tb_contrast.bin";
        Capture c = run_capture(bin, kRunBudget, p2_addrs);
        int mut_rises = 0; bool mut_any1 = false;
        rises_and_any1(c, 3, 0x80, &mut_rises, &mut_any1);
        int base_rises = 0; bool base_any1 = false;
        rises_and_any1(p2d_view, 3, 0x80, &base_rises, &base_any1);
        bool distinguishes = (base_rises == 1) && (mut_rises != 1);
        if (c.loaded && c.done.has_value() && distinguishes) {
            checks.expect(mut_rises != 1,
                "P2-CONTRAST [p342]: with start 4 written as CR<-$0E (START=0), s_4 shows no rise "
                "(CIAB/timer B)");
        } else {
            checks.record("P2-CONTRAST [p342]: unfalsifiable at this boundary -- start 4 not started "
                          "(CR<-$0E) produces the same rise count for s_4 as start 4's own normal run, "
                          "so no input at nanomig.v's ports through this comparator distinguishes the two");
        }
    }

    // Each Pre2.x contrast is checked against the SAME comparator's own
    // baseline reading (from the normal CIAB/timer-B run, p2d_view) before
    // it is graded: a mutation that reads identically to the baseline
    // cannot demonstrate the clause, and is reported instead of graded
    // (see P1-CONTRAST/P2-CONTRAST above for the same pattern; PRB's own
    // bit n reads the same pulled-up level in every P2 run here, since
    // DDRB never makes it an output in this proposition's own stimulus).

    // ---- Pre2.1 contrast --------------------------------------------------
    {
        std::string bin = dir + "/p2_ciab_tb_pre21.bin";
        Capture c = run_capture(bin, kRunBudget, p2_addrs);
        bool base_ok = pk_ok(p2d_view,0), mut_ok = pk_ok(c,0);
        bool distinguishes = base_ok && mut_ok && (pk(p2d_view,0) != pk(c,0));
        if (c.loaded && c.done.has_value() && distinguishes) {
            checks.expect((pk(c,0)&0x80) != 0,
                "P2-PRE2.1 [p342]: with an extra start inserted before leg R's own write, r1=1 "
                "(started, no underflow) instead of 0 (CIAB/timer B)");
        } else {
            checks.record("P2-PRE2.1 [p342]: unfalsifiable at this boundary -- the inserted start reads "
                          "the same r1 as the normal run (PRB's own bit n is pulled up throughout leg R, "
                          "whichever CR value preceded it)");
        }
    }
    // ---- Pre2.2 contrast ----------------------------------------------
    {
        std::string bin = dir + "/p2_ciab_tb_pre22.bin";
        Capture c = run_capture(bin, kRunBudget, p2_addrs);
        // Pre2.2 fails for start 1 if its own one-shot never ends within the
        // poll limit: the underflow that would set i_1's own TB flag has not
        // happened yet, unlike the baseline (normal) run's own start 1.
        auto i1 = p2_i(c,0);
        auto i1_base = p2_i(p2d_view,0);
        bool mut_flag = i1.has_value() && (*i1 & kIcrTb);
        bool base_flag = i1_base.has_value() && (*i1_base & kIcrTb);
        if (c.loaded && c.done.has_value() && base_flag && !mut_flag) {
            checks.expect(!mut_flag,
                "P2-PRE2.2 [p342]: with leg S's own latch at $FFFF, start 1's one-shot does not end "
                "within the poll limit (CIAB/timer B): i_1's own TB flag is 0, where the baseline run's is 1");
        } else {
            checks.record("P2-PRE2.2 [p342]: unfalsifiable at this boundary -- i_1's own TB flag does not "
                          "distinguish the $FFFF latch from the baseline run's own $0020 latch");
        }
    }
    // ---- Pre2.3 contrast ----------------------------------------------
    {
        std::string bin = dir + "/p2_ciab_tb_pre23.bin";
        Capture c = run_capture(bin, kRunBudget, p2_addrs);
        auto z1 = p2_z(c,0);
        auto z1_base = p2_z(p2d_view,0);
        bool distinguishes = z1.has_value() && z1_base.has_value() && (*z1 != *z1_base);
        if (c.loaded && c.done.has_value() && distinguishes) {
            checks.expect((*z1 & 0x80) != 0,
                "P2-PRE2.3 [p342]: with start 1 written as CR<-$0D (PBON=0), z_1=1 instead of 0, "
                "so start 2's own level-before is not 0 (CIAB/timer B)");
        } else {
            checks.record("P2-PRE2.3 [p342]: unfalsifiable at this boundary -- z_1 reads the same pulled-up "
                          "level in this run as in the baseline run (PRB's own bit n is pulled up throughout, "
                          "whichever CR value started it)");
        }
    }
    // ---- Pre2.4 contrast ------------------------------------------------
    {
        std::string bin = dir + "/p2_ciab_tb_pre24.bin";
        Capture c = run_capture(bin, kRunBudget, p2_addrs);
        auto ih_val = pk_ok(c,12) ? std::optional<uint16_t>(pk(c,12)) : std::optional<uint16_t>();
        auto ih_base = pk_ok(p2d_view,12) ? std::optional<uint16_t>(pk(p2d_view,12)) : std::optional<uint16_t>();
        bool mut_flag = ih_val.has_value() && (*ih_val & kIcrTb);
        bool base_flag = ih_base.has_value() && (*ih_base & kIcrTb);
        if (c.loaded && c.done.has_value() && mut_flag && !base_flag) {
            checks.expect(mut_flag,
                "P2-PRE2.4 [p342]: with leg H's own latch at $0008, the ih flag is 1 instead of 0 (CIAB/timer B)");
        } else {
            checks.record("P2-PRE2.4 [p342]: unfalsifiable at this boundary -- the ih flag does not "
                          "distinguish the $0008 latch from the baseline run's own $FFFF latch");
        }
    }

    // ---- environment preconditions (EP1-EP3, F1 anchor), asserted for
    // every run: shown here once, from the four P1 runs plus the three
    // dedicated environment-contrast programs.
    {
        // EP1's own reads live at kEp1CraA.. kEp1CrbB, separate from P1's own
        // peek list above; re-peek them from a fresh, cheap run (any P1
        // instance's own binary already includes SETUP).
        Capture e = run_capture(dir + "/p1_ciaa_ta.bin", kRunBudget, {kEp1CraA,kEp1CrbA,kEp1CraB,kEp1CrbB});
        bool ok = e.loaded && e.done.has_value() && pk_ok(e,0) && pk_ok(e,1) && pk_ok(e,2) && pk_ok(e,3) &&
                  pk(e,0)==0 && pk(e,1)==0 && pk(e,2)==0 && pk(e,3)==0;
        checks.expect(ok, "EP1 [p354]: the first reads of CRA and CRB on both CIAs, made before any write "
                          "to them, return $00");
    }
    {
        std::string bin = dir + (mv == "EP1" ? "/ep1_contrast.bin" : "/p1_ciaa_ta.bin");
        Capture e = run_capture(bin, kRunBudget, {kEp1CraA,kEp1CrbA,kEp1CraB,kEp1CrbB});
        bool want_0a = (mv == "EP1");
        bool ok = e.loaded && e.done.has_value() && pk_ok(e,0) &&
                  (want_0a ? pk(e,0) == 0x0A : pk(e,0) == 0x00);
        checks.expect(ok, "EP1-CONTRAST [p354]: a write to CIAA CRA before the read is reflected in it "
                          "($0A instead of $00)");
    }

    {
        std::string bin = dir + (mv == "EP2" ? "/ep2_contrast.bin" : "/p1_ciaa_ta.bin");
        Capture e = run_capture(bin, kRunBudget, {});
        auto s = mark(e.log, kMkStart);
        bool ok = e.loaded && e.done.has_value() && s.has_value() &&
                  ep2_ok(e.log, *s, *e.done);
        bool want_pass = (mv != "EP2");
        checks.expect(e.loaded && e.done.has_value() && s.has_value() && (ok == want_pass),
            "EP2 [SPECIFICATION.md 1.2]: between the first and last marker, the Chip-memory port "
            "shows only CPU accesses and memory-refresh accesses");
    }
    {
        Capture e = run_capture(dir + "/ep3_contrast.bin", 500'000, {});
        checks.expect(e.loaded && !e.done.has_value(),
            "EP3-CONTRAST [SPECIFICATION.md 1.2]: a program that never reaches DONE has no final "
            "marker and no graded result");
    }
    {
        Capture e = run_capture(dir + "/p1_ciaa_ta.bin", kRunBudget, {});
        Anchor anc = measure_anchor(e.log);
        checks.expect(anc.ok, "F1-ANCHOR [p194,p195,p23]: consecutive refresh line-marks are 454*T_CPU apart "
                              "(or alternately 456/454*T_CPU), measured from the run's own refresh reads");
        bool contrast_fails = anc.ok && !anchor_holds_at(anc, double(anc.t_cpu) * 1.25);
        checks.expect(contrast_fails, "F1-ANCHOR-CONTRAST [p194,p195,p23]: the same comparator, evaluated "
                                      "against 1.25x the measured T_CPU, fails");
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
