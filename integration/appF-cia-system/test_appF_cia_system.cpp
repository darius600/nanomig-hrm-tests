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
// MUTATION P1-MAIN-CRA/CRB/P1-MAIN already red on the unmutated run: CIAB's own PBON override never
//   reaches PRB (b1/c1 read the DDRB/PR-data value, not the timer's own level); the green control
//   through the same comparator is P1-CONTROL below (PBON=0, same legs, same instances)
// MUTATION P1-OTHERBITS stimulus red CIAB/timer-B's own leg-B pattern bit 0 flipped (p1_ciab_tb_otherbits.bin):
//   b1's bits 0-5 no longer match this instance's own written pattern, via NMH_MUTATE=P1-OTHERBITS
// MUTATION P1-PRE1.1 stimulus red CIAB/timer-B's pull-up reference read with DDRB bit n=1 and the data bit 0 beforehand (p1_ciab_tb_pre11.bin), via NMH_MUTATE=P1-PRE11
// MUTATION P1-PRE1.2 stimulus red CIAB/timer-B's step-4 latch changed to $FFFF (p1_ciab_tb_pre12.bin): the poll limit runs out, via NMH_MUTATE=P1-PRE12
// MUTATION P1-PRE1.3 stimulus red CIAB/timer-B's step-7 latch changed to $0004 (p1_ciab_tb_pre13.bin): an underflow happens during leg C, via NMH_MUTATE=P1-PRE13
// MUTATION P1-PRE1.4/O1 stimulus red CIAB/timer-B's own O1-contrast leg left at data bit n=0 instead of 1
//   (p1_ciab_tb_pre14.bin): the "data 1 reads 1" half of the precondition no longer holds, via NMH_MUTATE=P1-PRE14.
//   Side effect: CIAB/timer B's own Pre1.4 failing also excludes it from P1-MAIN-CRB (both of that
//   conjunction's instances are then excluded), so P1-MAIN-CRB reads PASS (vacuous) under this mutation
//   where it reads FAIL at baseline -- measured, not assumed; replayed on both cores
// MUTATION P1-LP no longer asserted (see the P1-LP/P1-O2 record below): fdd_led follows disk DMA, not
//   MTR* at select (NM-HRM-046), so no input through R-SELECT turns this record's own reading green
// MUTATION P1-CONTROL is the green control for P1-MAIN's own red (PBON=0, same comparator, all four
//   instances, graded against the page's own PBON=0 values)
// MUTATION P2-LEGR/P2-MAIN/P2-LEGH (component lines) and P2-TOGGLE (opus:F-timers.c14) already red on
//   the unmutated run: PBON's own override never reaches PRB (same RTL gap as P1-MAIN)
// MUTATION P2-LEGH-CONTRAST unfalsifiable at this boundary: CIAB/timer-B's own h1 start written as
//   CR<-$06 (START=0) instead of $07 (p2_ciab_tb_legh0.bin), via NMH_MUTATE=P2-LEGH0 -- h1 reads the
//   same driven level whether started or not (the RTL does not read CR's own START bit for this line
//   either), which is itself evidence that the red above is the RTL's, not the fixture's
// MUTATION P2-CONTRAST unfalsifiable at this boundary: CIAB/timer-B's own start-4-not-started run
//   (p2_ciab_tb_contrast.bin) shows the same rise count (0, no rise) for s_4 as the normal run, via
//   NMH_MUTATE=P2-CONTRAST -- live; not distinguishable on this build, baseline already reads 0
//   throughout for s_4 (P2-MAIN's own red). Side effect, measured: leaving start 4 unstarted also
//   means it never underflows, so its own ICR flag never sets; P2-PRE2.2, which checks the flag for
//   all four starts of all four instances, reads FAIL under this mutation where it reads PASS at
//   baseline -- a genuine consequence of the mutation, not a fixture defect
// MUTATION P2-PRE2.1 unfalsifiable at this boundary: CIAB/timer-B's own extra-start-before-leg-R run
//   (p2_ciab_tb_pre21.bin) reads the same pulled-up r1 as the baseline run, via NMH_MUTATE=P2-PRE21 --
//   live; not distinguishable on this build, baseline already reads r1=1 (pulled up), the same level
//   P2-LEGR's own red already shows
// MUTATION P2-PRE2.2 stimulus red CIAB/timer-B's leg-S latch changed to $FFFF (p2_ciab_tb_pre22.bin): the
//   stored poll count for start 1 hits the limit and i_1's own TB flag reads 0, via NMH_MUTATE=P2-PRE22
// MUTATION P2-PRE2.3 stimulus red CIAB/timer-B's start 1 written as $0D with DDRB bit n left an input
//   (p2_ciab_tb_pre23.bin): z_1 reads 1 (pulled up) instead of the baseline's own driven 0, via
//   NMH_MUTATE=P2-PRE23. Side effect: this mutant leaves DDRB undriven for all of leg S, not only
//   start 1, so CIAB/timer B's own s_4 also reads the pulled-up level throughout instead of the
//   baseline's own driven 0; P2-CONTRAST's own comparator, reading the same capture as its baseline,
//   then finds base_rises=1 and grades a PASS under this mutation where it is RECORDED (unfalsifiable)
//   at the normal baseline -- measured, not assumed; replayed on both cores
// MUTATION P2-PRE2.4 stimulus red CIAB/timer-B's leg-H latch changed to $0008 (p2_ciab_tb_pre24.bin): the
//   ih flag reads 1 where the baseline run's reads 0, via NMH_MUTATE=P2-PRE24
// MUTATION EP1 stimulus red a write to CIAA CRA precedes SETUP's own read (ep1_contrast.bin): the read
//   returns $0A instead of $00, via NMH_MUTATE=EP1
// MUTATION EP2 stimulus red Copper DMA started in the window (ep2_contrast.bin): the Copper's own fetch
//   of $0C3000 appears at the Chip-memory port, via NMH_MUTATE=EP2
// MUTATION EP3 stimulus red the program never reaches DONE (ep3_contrast.bin): no final marker, no
//   graded result, via NMH_MUTATE=EP3
// MUTATION F1-ANCHOR stimulus red the same live refresh capture, evaluated against 1.25x the measured
//   T_CPU instead of 1x, via NMH_MUTATE=(none -- recomputed from the normal capture, no separate binary)
// MUTATION FIXTURE not separately mutated: it is the same program-stored-constant channel every other
//   result store already uses; a break in it would show as every other result reading wrong too
//
// LP's own record, and Pre2.1/Pre2.3/Pre2.4's own contrasts, each isolate one mechanism at the one
// instance (CIAB/timer B) the proposition's own O2 channel and leg R already exercise; the same
// mechanism is not retested per instance (SPECIFICATION.md states each proposition once for "the four
// instances", not once per instance).

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <functional>
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

constexpr uint32_t kFixtureAddr  = 0x0C1FFC;
constexpr uint16_t kFixtureValue = 0xBEEF;

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
constexpr uint32_t kP1O1C   = kResults + 28;

// P1 peek layout: 0=R0 1=A2 2=Poll1 3=I1 4=A1 5=B1 6=C1 7=I2 8=D1 9=D2
// 10=O1C 11..14=EP1 CRA_A/CRB_A/CRA_B/CRB_B.
constexpr size_t kP1IdxD1  = 8;
constexpr size_t kP1IdxD2  = 9;
constexpr size_t kP1IdxO1C = 10;
constexpr size_t kP1IdxEp1 = 11;

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
constexpr uint32_t kP2Poll = kP2Ih + 2;              // 4 words
constexpr uint32_t kP2S0  = kP2Poll + 4 * 2;         // 1 word

// P2 peek layout: 0=R1 1-4=Z[0..3] 5-8=I[0..3] 9=H1 10=H2 11=H3 12=IH
// 13..268=S bits (4*64) 269..272=Poll[0..3] 273=S0 274..277=EP1.
constexpr size_t kP2IdxSBase   = 13;
constexpr size_t kP2IdxPoll    = 269;
constexpr size_t kP2IdxS0      = 273;
constexpr size_t kP2IdxEp1     = 274;

// ICR bits (ciaregs.i).
constexpr uint16_t kIcrTa = 0x01, kIcrTb = 0x02;

uint32_t word_addr(uint32_t byte_addr) { return byte_addr / 2; }

std::string hex2(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "$%02X", unsigned(v & 0xFF));
    return buf;
}

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

Capture view_of(const LedCapture& lc) {
    Capture c;
    c.loaded = lc.loaded; c.released = lc.released; c.done = lc.done;
    c.peeks = lc.peeks; c.log = lc.log;
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

// ---- P1: per-instance leg grading, gated by Pre1.1 (a1,a2,d2), Pre1.3
// (c1) and Pre1.4/O1 (the whole instance -- SPECIFICATION.md P1
// Preconditions). d1/O1C together ARE Pre1.4/O1: DDRB bit n=1, data 0,
// PBON=0 reads 0 (d1); the contrast, data 1 reads 1 (O1C).
struct P1Legs {
    bool o1_live = false;
    bool a2_ok = true, a1_ok = true, b1_ok = true, c1_ok = true, d1_ok = true, d2_ok = true;
    bool otherbits_ok = true;
};

P1Legs grade_p1(const Capture& c, uint16_t n_mask, uint16_t icr_mask, uint16_t Q1, uint16_t Q2) {
    P1Legs g;
    if (!c.loaded || !c.done || c.peeks.size() <= kP1IdxO1C) return g;
    uint16_t r0 = pk(c,0), a2 = pk(c,1), i2v = pk(c,7), a1 = pk(c,4), b1 = pk(c,5), c1 = pk(c,6);
    uint16_t d1 = pk(c,kP1IdxD1), d2 = pk(c,kP1IdxD2), o1c = pk(c,kP1IdxO1C);
    bool pre11 = pk_ok(c,0) && (r0 & n_mask);
    bool pre13 = pk_ok(c,7) && !(i2v & icr_mask);
    bool o1_pos = pk_ok(c,kP1IdxD1) && !(d1 & n_mask);
    bool o1_con = pk_ok(c,kP1IdxO1C) && (o1c & n_mask);
    g.o1_live = o1_pos && o1_con;
    if (!g.o1_live) return g;   // instance excluded; CIA-A's read-back is recorded separately
    if (pre11) { g.a2_ok = !(a2 & n_mask); g.a1_ok = !(a1 & n_mask); g.d2_ok = (d2 & n_mask) != 0; }
    g.b1_ok = !(b1 & n_mask);
    if (pre13) g.c1_ok = (c1 & n_mask) != 0;
    g.d1_ok = !(d1 & n_mask);
    uint16_t bit_o = (n_mask == 0x40) ? 0x80 : 0x40;
    bool ok_a1o = ((a1 ^ r0) & uint16_t(~n_mask & 0xFF)) == 0;
    bool ok_a2o = ((a2 ^ r0) & uint16_t(~n_mask & 0xFF)) == 0;
    bool ok_b1o = ((b1 & 0x3F) == Q1) && (((b1 ^ r0) & bit_o) == 0);
    bool ok_c1o = ((c1 & 0x3F) == Q2) && (((c1 ^ r0) & bit_o) == 0);
    g.otherbits_ok = ok_a1o && ok_a2o && ok_b1o && ok_c1o;
    return g;
}

bool p1_main_ok(const P1Legs& g) { return g.a2_ok && g.a1_ok && g.b1_ok && g.c1_ok && g.d1_ok && g.d2_ok; }

// P1's own control (PBON=0 throughout): a1,a2,b1 bit n=1; c1=0; d1=0
// (unchanged -- CR=$00 in both runs); d2=1 (unchanged). SPECIFICATION.md
// P1, "Contrasting case"/"P1's control".
bool p1_control_ok(const Capture& c, uint16_t n_mask) {
    if (!c.loaded || !c.done || c.peeks.size() <= kP1IdxD2) return false;
    uint16_t a2=pk(c,1), a1=pk(c,4), b1=pk(c,5), c1=pk(c,6), d1=pk(c,kP1IdxD1), d2=pk(c,kP1IdxD2);
    bool bitA2=(a2&n_mask)!=0, bitA1=(a1&n_mask)!=0, bitB1=(b1&n_mask)!=0, bitC1=(c1&n_mask)!=0;
    bool bitD1=(d1&n_mask)!=0, bitD2=(d2&n_mask)!=0;
    return bitA2 && bitA1 && bitB1 && !bitC1 && !bitD1 && bitD2;
}

bool o1_live_of(const Capture& c, uint16_t n_mask) {
    if (!c.loaded || !c.done || c.peeks.size() <= kP1IdxO1C) return false;
    return pk_ok(c,kP1IdxD1) && !(pk(c,kP1IdxD1)&n_mask) && pk_ok(c,kP1IdxO1C) && (pk(c,kP1IdxO1C)&n_mask);
}

// ---- P2: leg S's rise check, with the level before each start prepended
// (SPECIFICATION.md P2-MAIN's own comparator fix) and Pre2.3's own gating
// (a start whose level-before is not 0 is not graded, its value reported).
struct P2LegSResult {
    bool graded_any = false;
    bool all_ok = true;
    std::vector<std::string> notes;
};

P2LegSResult grade_p2_legs(const Capture& c, uint16_t n_mask, const char* tag,
                            std::function<std::optional<bool>(const Capture&, int, int)> s_bit,
                            std::function<std::optional<uint16_t>(const Capture&, int)> z_of) {
    P2LegSResult r;
    if (!c.loaded || !c.done || c.peeks.size() <= kP2IdxS0) return r;
    std::optional<bool> before;
    if (pk_ok(c, kP2IdxS0)) before = (pk(c, kP2IdxS0) & n_mask) != 0;
    for (int k = 0; k < 4; ++k) {
        bool pre23 = before.has_value() && !*before;
        if (pre23) {
            r.graded_any = true;
            int rises = 0; bool any1 = false;
            bool prev = false; // the level before, known 0 here (Pre2.3)
            for (int j = 0; j < 64; ++j) {
                auto b = s_bit(c,k,j);
                if (!b) continue;
                if (*b) any1 = true;
                if (!prev && *b) ++rises;
                prev = *b;
            }
            if (!(rises == 1 && any1)) r.all_ok = false;
        } else {
            std::string state = before.has_value() ? (*before ? "1" : "?") : "unknown";
            r.notes.push_back(std::string(tag) + " start " + std::to_string(k+1) +
                " not graded (Pre2.3: level before is " + state + ", not 0)");
        }
        auto z = z_of(c,k);
        before = z ? std::optional<bool>((*z & n_mask) != 0) : std::optional<bool>();
    }
    return r;
}

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    // ---- fixture, asserted first (SPECIFICATION.md 1.2 E7) ------------
    {
        Capture f = run_capture(dir + "/p1_ciaa_ta.bin", kRunBudget, {kFixtureAddr});
        checks.expect(f.loaded && f.done.has_value() && pk_ok(f,0) && pk(f,0) == kFixtureValue,
            "FIXTURE [SPECIFICATION.md 1.2 E7]: the program can deliver a known stored constant "
            "before any other result is graded");
    }

    // ---- P1 instances -----------------------------------------------------
    std::string p1_ciaa_ta = dir + "/p1_ciaa_ta.bin";
    std::string p1_ciaa_tb = dir + "/p1_ciaa_tb.bin";
    std::string p1_ciab_ta = dir + "/p1_ciab_ta.bin";
    std::string p1_ciab_tb = dir + (mv == "P1-PRE11" ? "/p1_ciab_tb_pre11.bin"
                                    : mv == "P1-PRE12" ? "/p1_ciab_tb_pre12.bin"
                                    : mv == "P1-PRE13" ? "/p1_ciab_tb_pre13.bin"
                                    : mv == "P1-PRE14" ? "/p1_ciab_tb_pre14.bin"
                                    : mv == "P1-OTHERBITS" ? "/p1_ciab_tb_otherbits.bin"
                                    : "/p1_ciab_tb.bin");

    std::vector<uint32_t> p1_addrs = {kP1R0,kP1A2,kP1Poll1,kP1I1,kP1A1,kP1B1,kP1C1,kP1I2,kP1D1,kP1D2,
                                       kP1O1C,kEp1CraA,kEp1CrbA,kEp1CraB,kEp1CrbB};
    Capture p1a = run_capture(p1_ciaa_ta, kRunBudget, p1_addrs);
    Capture p1b = run_capture(p1_ciaa_tb, kRunBudget, p1_addrs);
    Capture p1c = run_capture(p1_ciab_ta, kRunBudget, p1_addrs);
    // CIAB/timer-B needs the LED trace too (O2), so it uses run_capture_led
    // instead of the plain capture.
    LedCapture p1d = run_capture_led(p1_ciab_tb, kRunBudget, p1_addrs);
    Capture p1d_view = view_of(p1d);

    // ---- P1-PRE1.1 - P1-PRE1.3 (unchanged channels; O1C does not move
    // their own indices) --------------------------------------------------
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

    // ---- P1-PRE1.4/O1: DDRB bit n=1, data 0, PBON=0 reads 0 (d1); data 1
    // reads 1 (O1C's own contrast leg). Graded only where a live channel is
    // expected (CIAB); CIA-A's own dead read-back is recorded, not asserted
    // as a failed precondition -- it is the fact P1's own legs are gated on.
    P1Legs ga = grade_p1(p1a, 0x40, kIcrTa, 0x2A, 0x15);       // CIAA/TA
    P1Legs gb = grade_p1(p1b, 0x80, kIcrTb, 0x2A, 0x15);       // CIAA/TB
    P1Legs gc = grade_p1(p1c, 0x40, kIcrTa, 0x3D, 0x3B);       // CIAB/TA
    P1Legs gd = grade_p1(p1d_view, 0x80, kIcrTb, 0x3D, 0x3B);  // CIAB/TB

    checks.expect(gc.o1_live, "P1-PRE1.4/O1 [p341]: DDRB bit n=1, data 0, PBON=0 reads 0, and data 1 "
                              "reads 1 (CIAB/timer A)");
    checks.expect(gd.o1_live, "P1-PRE1.4/O1 [p341]: DDRB bit n=1, data 0, PBON=0 reads 0, and data 1 "
                              "reads 1 (CIAB/timer B)");
    checks.record("P1-PRE1.4/O1 [p341], CIA-A: a PRB read never shows a driven level -- measured " +
                   hex2(pk(p1a,kP1IdxD1)) + " (d1) and " + hex2(pk(p1a,kP1IdxO1C)) + " (O1 contrast) on "
                   "CIAA/timer A, " + hex2(pk(p1b,kP1IdxD1)) + " and " + hex2(pk(p1b,kP1IdxO1C)) +
                   " on CIAA/timer B, whatever DDRB and the data register hold -- port B returns the "
                   "lines, not the data register (NM-HRM-054) -- so CIA-A's instances are not graded "
                   "on P1's PBON legs or on P1-OTHERBITS' pattern-bit clause");

    // ---- P1-MAIN, split by id (the lead's decision): timer-A instances
    // carry the CRA id, timer-B instances the CRB ids, and the p341/p342
    // ids sit on the conjunction of all four plus the other-bits clause.
    // Each instance's legs are gated by Pre1.1, Pre1.3 and Pre1.4/O1 above;
    // an excluded instance (O1 not live) contributes vacuously.
    bool cra_ok = p1_main_ok(ga) && p1_main_ok(gc);
    bool crb_ok = p1_main_ok(gb) && p1_main_ok(gd);
    bool otherbits_all = ga.otherbits_ok && gb.otherbits_ok && gc.otherbits_ok && gd.otherbits_ok;
    bool main_ok = cra_ok && crb_ok && otherbits_all;

    checks.expect(cra_ok, "P1-MAIN-CRA [p349: sol:F-cra.c17]: PBON=1 puts timer A's output on PB6, "
                          "forcing an output over DDRB and the PR data, for the timer-A instances "
                          "whose read-back is live (Pre1.4/O1)");
    checks.expect(crb_ok, "P1-MAIN-CRB [p349: sol:F-crb.c3, opus:F-crb.c2; p350: sol:F-crb.c14]: PBON=1 "
                          "puts timer B's output on PB7, forcing an output over DDRB and the PR data, "
                          "for the timer-B instances whose read-back is live (Pre1.4/O1)");
    checks.expect(main_ok,
        "P1-MAIN [p342: opus:F-timers.c10, opus:F-timers.c11, sol:F-timers.c12; "
        "p341: opus:F-ports.c7, sol:F-ports.c7]: PBON=1 puts the timer's output on PB6 (timer A) or "
        "PB7 (timer B) only, forcing an output over DDRB and the PR data, for the instances whose "
        "read-back is live, including the other-bits clause");

    checks.expect(otherbits_all,
        "P1-OTHERBITS [p342]: bits 0-5 of b1 and c1 match this instance's own written pattern, and bit "
        "o matches r0, for the instances whose read-back is live (Pre1.4/O1)");
    checks.note("P1-OTHERBITS, CIA-A (not graded): CIAA/timer A b1=" + hex2(pk(p1a,5)) + " c1=" + hex2(pk(p1a,6)) +
                " (pattern $2A/$15 expected in bits 0-5); CIAA/timer B b1=" + hex2(pk(p1b,5)) + " c1=" +
                hex2(pk(p1b,6)) + " (pattern $2A/$15 expected)");

    // Per-instance values, for the record (SPECIFICATION.md section 5).
    auto p1_dump = [&](const Capture& c, const char* tag) {
        checks.note(std::string(tag) + ": r0=" + hex2(pk(c,0)) + " a2=" + hex2(pk(c,1)) +
            " a1=" + hex2(pk(c,4)) + " b1=" + hex2(pk(c,5)) + " c1=" + hex2(pk(c,6)) +
            " d1=" + hex2(pk(c,kP1IdxD1)) + " o1c=" + hex2(pk(c,kP1IdxO1C)) + " d2=" + hex2(pk(c,kP1IdxD2)));
    };
    p1_dump(p1a, "P1 CIAA/TA"); p1_dump(p1b, "P1 CIAA/TB");
    p1_dump(p1c, "P1 CIAB/TA"); p1_dump(p1d_view, "P1 CIAB/TB");

    // ---- P1-LP / P1-O2: fdd_led follows disk DMA, not MTR* latched at
    // select (NM-HRM-046). Recorded, not graded; nothing is graded on the
    // light (the lead's decision).
    auto mk_cycle = [&](uint32_t addr) { return mark(p1d.log, addr); };
    bool lp1 = !p1d.led_before_boot;
    bool lp2 = false, lp3 = false;
    auto lc2 = mk_cycle(kMkLpOnEnd), lc3 = mk_cycle(kMkLpOffEnd);
    if (lc2) { auto v = p1d.led_at(*lc2); if (v) lp2 = (*v != 0); }
    if (lc3) { auto v = p1d.led_at(*lc3); if (v) lp3 = (*v == 0); }
    checks.record("P1-LP/P1-O2 [p244]: fdd_led follows disk DMA, not MTR* latched at drive select "
                   "(NM-HRM-046) -- measured: LED before boot=" + std::string(p1d.led_before_boot?"on":"off") +
                   ", after R-SELECT with MTR*=0 (on leg)=" + std::string(lp2?"on":"off") +
                   ", after R-SELECT with MTR*=1 (off leg)=" + std::string(lp3?"on":"off") +
                   " -- live; not distinguishable on this build, the LED already reads off through every "
                   "R-SELECT here, and the only proven-live input is disk DMA, not this port's own "
                   "select/motor write -- O2's own legs (LED-A1, LED-C, LED-D1, LED-D2) are not graded");
    (void)lp1;

    // ---- P1's own control (PBON=0), all four instances, graded against
    // the page's own PBON=0 values -- the green control P1-MAIN's own red
    // needs, on the same comparator basis (SPECIFICATION.md P1, "P1's
    // control").
    {
        Capture ca = run_capture(dir + "/p1_ciaa_ta_pbon0.bin", kRunBudget, p1_addrs);
        Capture cb = run_capture(dir + "/p1_ciaa_tb_pbon0.bin", kRunBudget, p1_addrs);
        Capture cc = run_capture(dir + "/p1_ciab_ta_pbon0.bin", kRunBudget, p1_addrs);
        Capture cd = run_capture(dir + "/p1_ciab_tb_pbon0.bin", kRunBudget, p1_addrs);
        bool la = o1_live_of(ca,0x40), lb = o1_live_of(cb,0x80), lcc = o1_live_of(cc,0x40), ld = o1_live_of(cd,0x80);
        bool control_ok = (!la || p1_control_ok(ca,0x40)) && (!lb || p1_control_ok(cb,0x80)) &&
                           (!lcc || p1_control_ok(cc,0x40)) && (!ld || p1_control_ok(cd,0x80));
        checks.expect(control_ok,
            "P1-CONTROL [p342]: with PBON=0 throughout, every leg reads the page's own PBON=0 values "
            "(a2,a1,b1 bit n=1; c1=0; d1=0; d2=1), for the instances whose read-back is live (Pre1.4/O1)");
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
                                        : mv == "P2-LEGH0" ? "/p2_ciab_tb_legh0.bin"
                                        : "/p2_ciab_tb.bin");

    std::vector<uint32_t> p2_addrs = {kP2R1, kP2Z+0,kP2Z+2,kP2Z+4,kP2Z+6, kP2I+0,kP2I+2,kP2I+4,kP2I+6,
                                       kP2H1,kP2H2,kP2H3,kP2Ih};
    for (int k=0;k<4;++k) for (int i=0;i<64;++i) p2_addrs.push_back(kP2S + k*128 + i*2);
    for (int k=0;k<4;++k) p2_addrs.push_back(kP2Poll + k*2);
    p2_addrs.push_back(kP2S0);
    p2_addrs.push_back(kEp1CraA); p2_addrs.push_back(kEp1CrbA);
    p2_addrs.push_back(kEp1CraB); p2_addrs.push_back(kEp1CrbB);

    Capture p2a = run_capture(p2_ciaa_ta, kRunBudget, p2_addrs);
    Capture p2b = run_capture(p2_ciaa_tb, kRunBudget, p2_addrs);
    Capture p2c = run_capture(p2_ciab_ta, kRunBudget, p2_addrs);
    LedCapture p2d = run_capture_led(p2_ciab_tb_bin, kRunBudget, p2_addrs);
    Capture p2d_view = view_of(p2d);

    // s_k[j] as bit n, from the flat peek array (index 13 + k*64 + j).
    auto p2_s_bit = [](const Capture& c, int k, int j, uint16_t n_mask) -> std::optional<bool> {
        size_t idx = kP2IdxSBase + size_t(k)*64 + size_t(j);
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

    // ---- component lines (no claim id -- SPECIFICATION.md P2, the lead's
    // decision: "opus:F-timers.c14 on one assertion over legs R, S and H
    // together, component lines without the id") -----------------------
    // Gated the same way as P1-MAIN (Pre1.4/O1, reused from the P1 captures
    // above: it is a fact of the instance's own read-back channel, not of
    // which program runs it) -- CIA-A's instances are excluded, as they are
    // from P1-MAIN; P2-MAIN gets the same exclusion for free through its
    // own Pre2.3 (s0 never reads 0 on CIA-A).
    auto p2_legr_component = [](const Capture& c, uint16_t n_mask) {
        return c.loaded && c.done.has_value() && pk_ok(c,0) && !(pk(c,0)&n_mask);
    };
    bool p2_legr_all = (!ga.o1_live || p2_legr_component(p2a,0x40)) &&
                        (!gb.o1_live || p2_legr_component(p2b,0x80)) &&
                        (!gc.o1_live || p2_legr_component(p2c,0x40)) &&
                        (!gd.o1_live || p2_legr_component(p2d_view,0x80));
    checks.expect(p2_legr_all, "P2-LEGR [p342]: the toggle output is low after RES (before any start), "
                              "for the instances whose read-back is live (Pre1.4/O1)");

    P2LegSResult r_a = grade_p2_legs(p2a, 0x40, "CIAA/TA",
        [&](const Capture& c, int k, int j){ return p2_s_bit(c,k,j,0x40); }, p2_z);
    P2LegSResult r_b = grade_p2_legs(p2b, 0x80, "CIAA/TB",
        [&](const Capture& c, int k, int j){ return p2_s_bit(c,k,j,0x80); }, p2_z);
    P2LegSResult r_c = grade_p2_legs(p2c, 0x40, "CIAB/TA",
        [&](const Capture& c, int k, int j){ return p2_s_bit(c,k,j,0x40); }, p2_z);
    P2LegSResult r_d = grade_p2_legs(p2d_view, 0x80, "CIAB/TB",
        [&](const Capture& c, int k, int j){ return p2_s_bit(c,k,j,0x80); }, p2_z);
    bool p2_legs_all = r_a.all_ok && r_b.all_ok && r_c.all_ok && r_d.all_ok;
    checks.expect(p2_legs_all, "P2-MAIN [p342]: every start whose own level-before is 0 (Pre2.3) sets the "
                              "toggle output high (the level before, prepended, followed by exactly one "
                              "rise and at least one 1), for all four instances");
    for (auto& n : r_a.notes) checks.note(n);
    for (auto& n : r_b.notes) checks.note(n);
    for (auto& n : r_c.notes) checks.note(n);
    for (auto& n : r_d.notes) checks.note(n);

    auto p2_legh_component = [](const Capture& c, uint16_t n_mask) {
        return c.loaded && c.done.has_value() && pk_ok(c,9) && pk_ok(c,11) && (pk(c,9)&n_mask) && (pk(c,11)&n_mask);
    };
    bool p2_legh_all = (!ga.o1_live || p2_legh_component(p2a,0x40)) &&
                        (!gb.o1_live || p2_legh_component(p2b,0x80)) &&
                        (!gc.o1_live || p2_legh_component(p2c,0x40)) &&
                        (!gd.o1_live || p2_legh_component(p2d_view,0x80));
    checks.expect(p2_legh_all, "P2-LEGH [p342]: h1=1 (set high on start) and h3=1 (set high again after a "
                              "stop, whether or not h2 separated it from toggling), for the instances whose "
                              "read-back is live (Pre1.4/O1)");

    // Per-instance values, for the record (SPECIFICATION.md section 5).
    auto p2_dump = [&](const Capture& c, const char* tag) {
        auto z1=p2_z(c,0), z2=p2_z(c,1), z3=p2_z(c,2), z4=p2_z(c,3);
        checks.note(std::string(tag) + ": r1=" + hex2(pk(c,0)) +
            " z1=" + (z1?hex2(*z1):"?") + " z2=" + (z2?hex2(*z2):"?") +
            " z3=" + (z3?hex2(*z3):"?") + " z4=" + (z4?hex2(*z4):"?") +
            " h1=" + hex2(pk(c,9)) + " h2=" + hex2(pk(c,10)) + " h3=" + hex2(pk(c,11)) +
            " s0=" + (pk_ok(c,kP2IdxS0) ? hex2(pk(c,kP2IdxS0)) : "?"));
    };
    p2_dump(p2a, "P2 CIAA/TA"); p2_dump(p2b, "P2 CIAA/TB");
    p2_dump(p2c, "P2 CIAB/TA"); p2_dump(p2d_view, "P2 CIAB/TB");

    bool p2_toggle_ok = p2_legr_all && p2_legs_all && p2_legh_all;
    checks.expect(p2_toggle_ok,
        "P2-TOGGLE [p342] opus:F-timers.c14: the toggle output is set low by RES and set high whenever "
        "the timer is started, whether it was low or high before (legs R, S and H together)");

    checks.record("LED-R [p244]: not graded -- fdd_led follows disk DMA, not MTR* latched at select "
                   "(NM-HRM-046); O2 is not observable at this port, the same fact P1-LP/P1-O2 record");

    // ---- Pre2.1: unfalsifiable at this boundary, sharing P2-LEGR's own
    // channel. Mutated via the top-level p2_ciab_tb_bin selection above.
    checks.record("P2-PRE2.1 [p342]: live; not distinguishable on this build, baseline already reads "
                   "r1=1 (pulled up) -- the same level P2-LEGR's own red already shows, whichever CR "
                   "value preceded leg R's write (an inserted start before it reads the same r1 under "
                   "NMH_MUTATE=P2-PRE21)");

    // ---- Pre2.2: before each restart, the previous one-shot has ended
    // (CR bit 0 read 0 within the poll limit -- stored -- and the flag
    // set), for all four starts of all four instances.
    auto p2_poll_ok = [&](const Capture& c, uint16_t icr_mask) {
        if (!c.loaded || !c.done) return false;
        bool ok = true;
        for (int k=0;k<4;++k) {
            size_t idx = kP2IdxPoll + k;
            if (idx >= c.peeks.size() || !c.peeks[idx]) return false;
            uint16_t poll = *c.peeks[idx];
            auto ik = p2_i(c,k);
            bool flagset = ik.has_value() && (*ik & icr_mask);
            ok = ok && (poll < 1000) && flagset;
        }
        return ok;
    };
    bool pre22_all = p2_poll_ok(p2a,kIcrTa) && p2_poll_ok(p2b,kIcrTb) && p2_poll_ok(p2c,kIcrTa) && p2_poll_ok(p2d_view,kIcrTb);
    checks.expect(pre22_all, "P2-PRE2.2 [p342]: before each restart, the previous one-shot has ended "
                             "(CR bit 0 read 0 within the poll limit, i_k's own flag set), for all four "
                             "starts of all four instances");

    // ---- Pre2.3: the level before start 2 is 0 (z_1's own bit n clear),
    // so start 2 is graded. Mutated via the top-level selection (start 1
    // written as $0D with DDRB left an input).
    {
        auto z1 = p2_z(p2d_view, 0);
        bool ok = z1.has_value() && !(*z1 & 0x80);
        checks.expect(ok, "P2-PRE2.3 [p342]: the level before start 2 is 0 (z_1's own bit n clear, "
                          "CIAB/timer B), so start 2 is graded");
    }

    // ---- Pre2.4: no underflow happens in leg H (ih's own flag clear),
    // for all four instances.
    auto p2_ih_ok = [](const Capture& c, uint16_t icr_mask) {
        return c.loaded && c.done.has_value() && pk_ok(c,12) && !(pk(c,12)&icr_mask);
    };
    bool pre24_all = p2_ih_ok(p2a,kIcrTa) && p2_ih_ok(p2b,kIcrTb) && p2_ih_ok(p2c,kIcrTa) && p2_ih_ok(p2d_view,kIcrTb);
    checks.expect(pre24_all, "P2-PRE2.4 [p342]: no underflow happens in leg H (ih's own flag clear), "
                             "for all four instances");

    // ---- P2's own contrasting case: start 4 not started (CR<-$0E). Selected
    // via the top-level p2_ciab_tb_bin above (NMH_MUTATE=P2-CONTRAST) -- this
    // block reads the already-captured p2d_view rather than running the
    // mutant a second time, so the normal run does not execute it.
    {
        int rises = 0; bool any1 = false; bool prev = false;
        for (int j = 0; j < 64; ++j) {
            auto b = p2_s_bit(p2d_view,3,j,0x80);
            if (!b) continue;
            if (*b) any1 = true;
            if (!prev && *b) ++rises;
            prev = *b;
        }
        (void)any1;
        checks.record("P2-CONTRAST [p342]: s_4 (CIAB/timer B) shows " + std::to_string(rises) +
                      " rise(s) in this run -- live; not distinguishable on this build: both the "
                      "started baseline (CR<-$0F) and the not-started mutant (NMH_MUTATE=P2-CONTRAST, "
                      "CR<-$0E) read 0 rises for s_4, because this build's s_4 never rises regardless "
                      "of CR (P2-MAIN's own red)");
    }

    // ---- P2-LEGH's own START=0 mutation: h1's own start written as
    // CR<-$06 instead of $07. Selected via the top-level p2_ciab_tb_bin
    // above (NMH_MUTATE=P2-LEGH0) -- this block reads p2d_view rather than
    // running the mutant a second time.
    {
        bool h1_bit = (pk(p2d_view,9) & 0x80) != 0;
        checks.record("P2-LEGH-CONTRAST [p342]: h1 (CIAB/timer B) reads " + hex2(pk(p2d_view,9)) +
                      " in this run -- live; not distinguishable on this build: h1 reads the same "
                      "driven level whether started (baseline, CR<-$07) or not (NMH_MUTATE=P2-LEGH0, "
                      "CR<-$06) -- the RTL does not read CR's own START bit for this line either, "
                      "which is itself evidence the red above is the RTL's, not the fixture's");
        (void)h1_bit;
    }

    // ---- environment preconditions (EP1-EP3, F1 anchor) ----------------
    {
        std::string bin = dir + (mv == "EP1" ? "/ep1_contrast.bin" : "/p1_ciaa_ta.bin");
        Capture e = run_capture(bin, kRunBudget, {kEp1CraA,kEp1CrbA,kEp1CraB,kEp1CrbB});
        bool ok = e.loaded && e.done.has_value() && pk_ok(e,0) && pk_ok(e,1) && pk_ok(e,2) && pk_ok(e,3) &&
                  pk(e,0)==0 && pk(e,1)==0 && pk(e,2)==0 && pk(e,3)==0;
        checks.expect(ok, "EP1 [p354]: the first reads of CRA and CRB on both CIAs, made before any write "
                          "to them, return $00");
    }
    {
        std::string bin = dir + (mv == "EP2" ? "/ep2_contrast.bin" : "/p1_ciaa_ta.bin");
        Capture e = run_capture(bin, kRunBudget, {});
        auto s = mark(e.log, kMkStart);
        bool ok = e.loaded && e.done.has_value() && s.has_value() && ep2_ok(e.log, *s, *e.done);
        checks.expect(ok,
            "EP2 [SPECIFICATION.md 1.2]: between the first and last marker, the Chip-memory port "
            "shows only CPU accesses and memory-refresh accesses");
    }
    {
        uint64_t budget = (mv == "EP3") ? 500'000 : kRunBudget;
        std::string bin = dir + (mv == "EP3" ? "/ep3_contrast.bin" : "/p1_ciaa_ta.bin");
        Capture e = run_capture(bin, budget, {});
        checks.expect(e.loaded && e.done.has_value(),
            "EP3 [SPECIFICATION.md 1.2]: the program reaches DONE and has a final marker");
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

    // ---- EP1, EP2 and F1's anchor, asserted per run (SPECIFICATION.md
    // 1.2: "Every run asserts all three") -- computed from the eight P1/P2
    // instance captures already made above, with no further harness runs.
    {
        struct RunRef { const Capture* c; size_t ep1_base; };
        std::vector<RunRef> runs = {
            {&p1a, kP1IdxEp1}, {&p1b, kP1IdxEp1}, {&p1c, kP1IdxEp1}, {&p1d_view, kP1IdxEp1},
            {&p2a, kP2IdxEp1}, {&p2b, kP2IdxEp1}, {&p2c, kP2IdxEp1}, {&p2d_view, kP2IdxEp1},
        };
        bool ep1_all = true, ep2_all = true, f1_all = true;
        for (auto& r : runs) {
            const Capture& c = *r.c;
            bool ep1ok = c.loaded && c.done.has_value() && c.peeks.size() >= r.ep1_base + 4;
            for (int i = 0; i < 4 && ep1ok; ++i)
                if (!pk_ok(c, r.ep1_base+i) || pk(c, r.ep1_base+i) != 0) ep1ok = false;
            ep1_all = ep1_all && ep1ok;

            auto s = mark(c.log, kMkStart);
            bool ep2ok = c.loaded && c.done.has_value() && s.has_value() && ep2_ok(c.log, *s, *c.done);
            ep2_all = ep2_all && ep2ok;

            Anchor anc = measure_anchor(c.log);
            f1_all = f1_all && anc.ok;
        }
        checks.expect(ep1_all, "EP1-PER-RUN [SPECIFICATION.md 1.2]: EP1 holds on every P1 and P2 "
                               "instance run (all eight)");
        checks.expect(ep2_all, "EP2-PER-RUN [SPECIFICATION.md 1.2]: EP2 holds on every P1 and P2 "
                               "instance run (all eight)");
        checks.expect(f1_all, "F1-ANCHOR-PER-RUN [p194,p195,p23]: F1's own anchor holds on every P1 "
                              "and P2 instance run (all eight)");
    }

    // ---- Section 0 declines, printed as RECORDED lines -------------------
    checks.record("opus:F-portsig.c47 [p351]: declined whole -- \"PC...not used\" (CIAB); no output this "
                  "package sees shows a PC pin on either CIA, and \"not used\" describes no behaviour");
    checks.record("opus:F-signals.c1, sol:F-signals.c69 [p353]: declined whole -- both say phi2 is a "
                  "\"TTL compatible input\", an electrical property of an input pin that nothing a test "
                  "sees depends on");

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
