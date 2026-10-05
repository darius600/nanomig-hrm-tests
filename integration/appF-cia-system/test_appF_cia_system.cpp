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
// MUTATION EP2 stimulus red Copper DMA started from a list in low Chip memory (ep2_contrast_low.bin): the Copper's own fetch lands below $1000 outside a refresh group, via NMH_MUTATE=EP2-LOW
// MUTATION EP3 stimulus red the program completes its work and stores $600E as its final marker (p1_ciaa_ta_ep3.bin): no DONE_VALUE write, via NMH_MUTATE=EP3
// MUTATION F1-ANCHOR stimulus red shorter lines (HTOTAL, BEAMCON0) set after the first marker (p1_ciaa_ta_f1.bin): consecutive refresh line marks are no longer 454*T_CPU apart, via NMH_MUTATE=F1-ANCHOR
// MUTATION F1-ANCHOR-CONTRAST none impossible unfalsifiable at this boundary: the line only compares the measured line marks against 1.25*T_CPU, and no stimulus makes line marks 454 or 456 apart at 1.25*T_CPU while they are not at T_CPU
// MUTATION FIXTURE stimulus red the program stores $BEE0 as its fixture word (p1_ciaa_ta_fixture.bin): the delivered word is not $BEEF, via NMH_MUTATE=FIXTURE
// MUTATION EP1-PER-RUN stimulus red the CIAA/timer-B P1 program writes CIAA CRA before its first read of it (p1_ciaa_tb_ep1.bin): that capture's EP1 words are not all $00, via NMH_MUTATE=EP1-PER-RUN
// MUTATION EP2-PER-RUN stimulus red the CIAA/timer-B P1 program starts Copper DMA from a list in low Chip memory (p1_ciaa_tb_ep2.bin): a non-refresh low-address fetch in that capture's window, via NMH_MUTATE=EP2-PER-RUN
// MUTATION EP3-PER-RUN stimulus red the CIAA/timer-B P1 program stores $600E as its final marker (p1_ciaa_tb_ep3.bin): that capture has no DONE_VALUE write, via NMH_MUTATE=EP3-PER-RUN
// MUTATION F1-ANCHOR-PER-RUN stimulus red the CIAA/timer-B P1 program sets shorter lines after the first marker (p1_ciaa_tb_f1.bin): that capture's line marks are off the cadence, via NMH_MUTATE=F1-ANCHOR-PER-RUN
// MUTATION P3 stimulus green a CPU-driven toggle through the same predicate: P3-GREENCTL below
// MUTATION P3-GREENCTL stimulus red the level before the first flag written as 0 instead of 1 (p3_ciab_tb_cputoggle_low.bin): the level conjunct of P3's predicate fails while the pairing holds, via NMH_MUTATE=P3-GREENCTL
// MUTATION P4 stimulus green a CPU-driven pulse at one fixed phase of the timer's period through the same comparator: P4-GREENCTL below
// MUTATION P4-GREENCTL stimulus red CIAB/timer B's L=40 control table with a second high count value (p4c_ciab_tb_l40_two.bin): two residues read high and the single-residue comparator fails, via NMH_MUTATE=P4-GREENCTL
// MUTATION P9-LEGF-TRANSITIONS stimulus green a CPU-driven transition train through the same comparator: P9-LEGF-TRANSITIONS-GREENCTL below
// MUTATION P9-LEGF-TRANSITIONS-GREENCTL stimulus red the CPU inverts PRB bit 7 after every second flagged read (p9_legf_ciab_tb_cputoggle_skip.bin): consecutive transitions are about 2*P(L) apart and the spacing comparator fails, via NMH_MUTATE=P9-LEGF-TRANSITIONS-GREENCTL
// MUTATION P9-LEGW-PRE2.2 stimulus red the L=40 leg-W latch changed to $FFFF (p9_legw_l40_nouf.bin): the one-shot has not ended when the sampler closes and the TB flag reads 0, via NMH_MUTATE=P9-LEGW-PRE22; P9-LEGW is then not graded
// MUTATION P9-LEGW-PRE2.3 stimulus red the L=40 leg-W run leaves DDRB bit 7 an input (p9_legw_l40_lvl1.bin): the level before the start reads the pull-up's 1, via NMH_MUTATE=P9-LEGW-PRE23; P9-LEGW is then not graded
// MUTATION P9-LEGW stimulus green a CPU-driven width through the same comparator: P9-LEGW-GREENCTL below
// MUTATION P9-LEGW-GREENCTL-PRE2.2 stimulus red the CPU-driven L=40 latch changed to $FFFF (p9_legw_ctl_l40_nouf.bin): the one-shot has not ended when the sampler closes and no TB flag is read, via NMH_MUTATE=P9-LEGW-GREENCTL-PRE22; P9-LEGW-GREENCTL is then not graded
// MUTATION P9-LEGW-GREENCTL-PRE2.3 stimulus red the CPU-driven L=40 run leaves DDRB bit 7 an input for the read before the start (p9_legw_ctl_l40_lvl1.bin): the level before the start reads the pull-up's 1, via NMH_MUTATE=P9-LEGW-GREENCTL-PRE23; P9-LEGW-GREENCTL is then not graded
// MUTATION P9-LEGW-GREENCTL stimulus red the CPU-driven L=40 run made with a latch of 20 (p9_legw_ctl_l20.bin): W(100)-W(20) is about 80, outside 60+-s, via NMH_MUTATE=P9-LEGW-GREENCTL
//
// LP's own record, and Pre2.1/Pre2.3/Pre2.4's own contrasts, each isolate one mechanism at the one
// instance (CIAB/timer B) the proposition's own O2 channel and leg R already exercise; the same
// mechanism is not retested per instance (SPECIFICATION.md states each proposition once for "the four
// instances", not once per instance).

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <set>
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
constexpr uint16_t kIcrTa = 0x01, kIcrTb = 0x02, kIcrAlrm = 0x04, kIcrSp = 0x08;

uint32_t word_addr(uint32_t byte_addr) { return byte_addr / 2; }

std::string hex2(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "$%02X", unsigned(v & 0xFF));
    return buf;
}

// A full 16-bit value, for timer/counter reads (SPECIFICATION.md section 5:
// "print 16-bit values in full") -- hex2 above is for the byte-wide CIA
// registers (CRA, ICR, PRB, DDRB) it was written for.
std::string hex4(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "$%04X", unsigned(v));
    return buf;
}

int64_t floordiv(int64_t a, int64_t b) {
    int64_t q = a / b;
    if ((a % b != 0) && ((a < 0) != (b < 0))) --q;
    return q;
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

// ---- the common preconditions of every run (SPECIFICATION.md 1.2 EP1-EP3, 1.4 F1) --
// Every capture the package consumes is recorded here as it is made; the per-run
// assertions at the end of main grade the whole population. The dedicated EP and F1
// contrast runs switch the recording off (NoRegister), so that their mutations
// change only their own assertions.
struct RunRecord {
    std::string bin;
    bool loaded = false, done = false, ep1 = false;
    bool f1 = false;              // F1's anchor holds
    bool ep2_graded = false;      // EP2 is graded only where the run completed and F1 holds
    bool ep2 = false;
};
std::vector<RunRecord>& run_registry() { static std::vector<RunRecord> v; return v; }
bool& registering() { static bool b = true; return b; }
struct NoRegister {
    bool saved;
    NoRegister() : saved(registering()) { registering() = false; }
    ~NoRegister() { registering() = saved; }
};
using Ep1Words = std::array<std::optional<uint16_t>, 4>;
void register_run(const std::string& bin, bool loaded, bool done, const Ep1Words& ep1,
                  const std::vector<nmh::RamAccess>& log);
template <class H> Ep1Words ep1_of(H& h) {
    return {h.peek_logical(kEp1CraA), h.peek_logical(kEp1CrbA), h.peek_logical(kEp1CraB), h.peek_logical(kEp1CrbB)};
}

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
    if (!c.loaded) { register_run(bin, false, false, Ep1Words{}, c.log); return c; }
    c.released = h.boot(kBootBudget).cpu_released;
    c.done = h.run_until_write(kDoneAddr, kDoneValue, run_budget);
    c.log = h.ram_log();
    for (auto a : peek_addrs) c.peeks.push_back(h.peek_logical(a));
    register_run(bin, true, c.done.has_value(), ep1_of(h), c.log);
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
    if (!c.loaded) { register_run(bin, false, false, Ep1Words{}, c.log); return c; }
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
    register_run(bin, true, c.done.has_value(), ep1_of(h), c.log);
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

// Pokes a parameter byte into the identity-mapped chip RAM parameter block
// (SPECIFICATION.md 1.2 E7/E9) before boot, in the low byte of the given
// word address (so the program reads it with `move.b addr+1,...`). Used
// only where the specification names "(E7)" explicitly (P7.3/P8.3's start
// value, P8.65 mode 11's CRB value): the same binary runs unmodified for
// both compared legs, and only the poked parameter differs.
Capture run_capture_param(const std::string& bin, uint32_t param_addr, uint8_t param_value,
                           uint64_t run_budget, const std::vector<uint32_t>& peek_addrs,
                           nmh::Config cfg = nmh::Config{}) {
    Capture c;
    nmh::NanomigHarness h(cfg);
    c.loaded = h.load_program(bin) > 0;
    if (!c.loaded) { register_run(bin, false, false, Ep1Words{}, c.log); return c; }
    h.poke_word(param_addr / 2, param_value);
    c.released = h.boot(kBootBudget).cpu_released;
    c.done = h.run_until_write(kDoneAddr, kDoneValue, run_budget);
    c.log = h.ram_log();
    for (auto a : peek_addrs) c.peeks.push_back(h.peek_logical(a));
    register_run(bin, true, c.done.has_value(), ep1_of(h), c.log);
    return c;
}

// ---- R-SAMPLE array extraction (SPECIFICATION.md 1.5 R-SAMPLE; F2/F3): a
// single pass over the log recovers each array slot's own store time and
// value from its own distinct word address (SPECIFICATION.md 1.2 E9), so
// per-iteration E-indices (F2) and period detection (F3) can be computed
// without a second harness run.
struct SampleSeries {
    std::vector<std::optional<uint64_t>> cycle;
    std::vector<std::optional<uint16_t>> value;
    size_t size() const { return cycle.size(); }
};

SampleSeries read_series(const std::vector<nmh::RamAccess>& log, uint32_t base_byte_addr, int count) {
    SampleSeries s;
    s.cycle.assign(count, std::nullopt);
    s.value.assign(count, std::nullopt);
    uint32_t base_w = word_addr(base_byte_addr);
    for (const auto& a : log) {
        if (!a.is_write) continue;
        if (a.word_addr < base_w) continue;
        uint32_t idx = a.word_addr - base_w;
        if (idx >= (uint32_t)count) continue;
        s.cycle[idx] = a.cycle;
        s.value[idx] = a.data;
    }
    return s;
}

// ---- F2: E-index and phase, against a T_CPU measured from the same run's
// own F1 anchor (SPECIFICATION.md 1.4 F1/F2).
double e_index(uint64_t t, uint64_t t0, uint64_t t_cpu) {
    double t_e = 10.0 * double(t_cpu);
    return std::round((double(t) - double(t0)) / t_e);
}

// (t - t0) mod T_E, folded into [-T_E/2, T_E/2) -- P6's own phi_i.
double fold_phase(uint64_t t, uint64_t t0, uint64_t t_cpu) {
    double t_e = 10.0 * double(t_cpu);
    double d = std::fmod(double(t) - double(t0), t_e);
    if (d < 0) d += t_e;
    if (d >= t_e / 2) d -= t_e;
    return d;
}

// ---- P6's own phase-band check, reused as the precondition ("the E-index
// frame is valid for this loop") by P4, P5(b) and P9 leg W
// (SPECIFICATION.md P6: "The R-SAMPLE loops ... are checked the same way").
// Band width is measured in units of T_CPU (rounded), not assumed.
struct PhaseCheck {
    bool has_data = false;
    double band_width_t_cpu = 0;   // (max-min)/T_CPU
    int distinct = 0;              // count of values >0.5*T_CPU apart from all others in a simple clustering
    bool band_ok = false;          // band_width <= 2*T_CPU (with small numeric slack)
};

PhaseCheck phase_check(const std::vector<double>& phases, uint64_t t_cpu) {
    PhaseCheck r;
    if (phases.empty() || t_cpu == 0) return r;
    r.has_data = true;
    double lo = *std::min_element(phases.begin(), phases.end());
    double hi = *std::max_element(phases.begin(), phases.end());
    r.band_width_t_cpu = (hi - lo) / double(t_cpu);
    r.band_ok = r.band_width_t_cpu <= 2.0 + 0.05;
    // cluster into buckets of width ~1 T_CPU to count distinct values
    std::vector<double> sorted = phases;
    std::sort(sorted.begin(), sorted.end());
    std::vector<double> clusters;
    for (double p : sorted) {
        if (clusters.empty() || p - clusters.back() > double(t_cpu) * 0.5) clusters.push_back(p);
    }
    r.distinct = (int)clusters.size();
    return r;
}

// ---- F3: underflow period P(L), fit exactly as SPECIFICATION.md 1.4 F3
// states it, not as the most frequent gap between flagged E-indices (that
// estimator is a different, weaker statistic: on a loop whose own iteration
// spacing is not a divisor of the true period, its gaps are multiples of the
// loop spacing, and "most frequent" picks one of those, not P).
//
// f_j are the E-indices of every ICR store in the series (flagged or not)
// and q_j their TA/TB flag. P(L) is the integer P for which one phase phi
// exists such that, for every j >= 1, q_j = 1 exactly when phi + kP lies in
// (f_(j-1), f_j] for some integer k -- i.e. the half-open interval crosses
// exactly one multiple of P from phi when q_j is set, and none when it is
// clear. "Exactly one P fits" (at least 16 flags, iteration spacing at most
// P/2) is checked by search: P(L) is unique when exactly one P in [2,400]
// admits such a phase.
struct PeriodResult {
    bool ok = false;               // exactly one P fits, over >=16 flags
    int64_t P = 0;
    size_t flag_count = 0;
};

PeriodResult measure_period(const SampleSeries& series, uint16_t mask, uint64_t t_cpu) {
    PeriodResult r;
    std::vector<int64_t> f;
    std::vector<bool> q;
    uint64_t t0 = 0; bool have = false;
    for (size_t i = 0; i < series.size(); ++i) {
        if (!series.cycle[i] || !series.value[i]) continue;
        if (!have) { t0 = *series.cycle[i]; have = true; }
        f.push_back((int64_t)e_index(*series.cycle[i], t0, t_cpu));
        bool fl = (*series.value[i] & mask) != 0;
        q.push_back(fl);
        if (fl) ++r.flag_count;
    }
    if (r.flag_count < 16 || f.size() < 2) return r;
    std::vector<int64_t> fits;
    for (int64_t P = 2; P <= 400; ++P) {
        bool anyphi = false;
        for (int64_t phi = 0; phi < P && !anyphi; ++phi) {
            bool ok = true;
            for (size_t j = 1; j < f.size() && ok; ++j) {
                int64_t cnt = floordiv(f[j] - phi, P) - floordiv(f[j - 1] - phi, P);
                if ((cnt > 0) != q[j]) ok = false;
            }
            if (ok) anyphi = true;
        }
        if (anyphi) fits.push_back(P);
        if (fits.size() > 1) break;   // already not unique
    }
    r.ok = (fits.size() == 1);
    if (!fits.empty()) r.P = fits.front();
    return r;
}

// Maximum gap between consecutive (present) E-indices of a whole series --
// used for "iteration spacing at most X E periods" preconditions.
int64_t max_gap_e(const std::vector<int64_t>& e_sorted) {
    int64_t m = 0;
    for (size_t i = 1; i < e_sorted.size(); ++i) m = std::max(m, e_sorted[i] - e_sorted[i - 1]);
    return m;
}

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

// ---- R-KEY (SPECIFICATION.md 1.5): runs `bin` cycle-by-cycle, waits for a
// write to `ready_addr` (the program's own "ready" marker), measures this
// run's own T_CPU from its own refresh captures, then -- if send_key is
// true -- at least `min_e_after_ready` E periods after the ready marker,
// puts one Amiga key code on the kbd_mouse_type/data/level port group for
// one clk_sys cycle, toggling kbd_mouse_level to mark a new code. No port
// carries KCLK/KDAT, so the test does not drive or time them (R-KEY).
// Continues until DONE or the cycle budget.
struct KeyCapture {
    bool loaded=false, released=false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
    std::vector<std::optional<uint16_t>> peeks;
    uint64_t t_cpu = 0;
    bool key_sent = false;
};

KeyCapture run_capture_key(const std::string& bin, uint32_t ready_addr, bool send_key,
                            uint16_t keycode, uint64_t min_e_after_ready, uint64_t budget,
                            const std::vector<uint32_t>& peek_addrs, nmh::Config cfg = nmh::Config{}) {
    KeyCapture r;
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) { register_run(bin, false, false, Ep1Words{}, r.log); return r; }
    r.released = h.boot(kBootBudget).cpu_released;
    uint32_t w_ready = word_addr(ready_addr);
    std::optional<uint64_t> ready_cycle;
    bool have_tcpu = false, key_pending = false, found_done = false;
    size_t seen = h.ram_log().size();
    for (uint64_t i = 0; i < budget && !found_done; ++i) {
        if (key_pending) {
            h.ports()->kbd_mouse_type = 2;
            h.ports()->kbd_mouse_data = keycode;
            h.ports()->kbd_mouse_level = !h.ports()->kbd_mouse_level;
        }
        h.run_cycles(1);
        if (key_pending) {
            h.ports()->kbd_mouse_type = 0;
            key_pending = false;
            r.key_sent = true;
        }
        const auto& lg = h.ram_log();
        for (; seen < lg.size(); ++seen) {
            const auto& a = lg[seen];
            if (a.is_write && a.word_addr == word_addr(kDoneAddr) && a.data == kDoneValue) { r.done = a.cycle; found_done = true; }
            if (a.is_write && a.word_addr == w_ready && !ready_cycle) ready_cycle = a.cycle;
        }
        if (!have_tcpu) {
            Anchor a = measure_anchor(h.ram_log());
            if (a.ok) { r.t_cpu = a.t_cpu; have_tcpu = true; }
        }
        if (send_key && !r.key_sent && !key_pending && ready_cycle && have_tcpu &&
            h.cycles() >= *ready_cycle + min_e_after_ready * 10 * r.t_cpu) {
            key_pending = true;
        }
    }
    r.log = h.ram_log();
    for (auto a : peek_addrs) r.peeks.push_back(h.peek_logical(a));
    register_run(bin, true, r.done.has_value(), ep1_of(h), r.log);
    return r;
}

Capture view_of(const KeyCapture& kc) {
    Capture c;
    c.loaded = kc.loaded; c.released = kc.released; c.done = kc.done;
    c.peeks = kc.peeks; c.log = kc.log;
    return c;
}

// ---- EP2: between the first marker (MK_START) and the last (DONE), the
// Chip-memory port shows only refresh accesses (F4's own shape: a group of
// four reads, two memory cycles apart, on F1's line cadence) or accesses to
// this package's own CPU-owned ranges (E9: the program's own code, its
// results, its markers, the parameter block, K3's byte). Any other access in
// the window is a stray DMA master -- exactly EP2's own contrast (the
// Copper's own fetch of $0C3000, or of a list in low Chip memory).
bool in_range(uint32_t w, uint32_t lo_byte, uint32_t hi_byte) {
    return w >= word_addr(lo_byte) && w < word_addr(hi_byte);
}

bool ep2_ok(const std::vector<nmh::RamAccess>& log, uint64_t lo, uint64_t hi, bool* saw_extra = nullptr) {
    // A read below $1000 is a refresh read only when it belongs to a group of
    // exactly four reads 4*T_CPU apart and F1's line-cadence anchor holds.
    Anchor anc = measure_anchor(log);
    if (!anc.ok) return false;
    std::vector<uint64_t> low_reads;
    for (const auto& x : log) if (!x.is_write && x.word_addr < 0x1000) low_reads.push_back(x.cycle);
    std::sort(low_reads.begin(), low_reads.end());
    std::vector<std::vector<uint64_t>> groups;
    for (size_t i = 0; i < low_reads.size(); ++i) {
        if (i == 0 || low_reads[i] - low_reads[i - 1] > 100) groups.push_back({});
        groups.back().push_back(low_reads[i]);
    }
    const uint64_t log_end = log.empty() ? 0 : log.back().cycle;
    std::set<uint64_t> refresh;
    for (size_t gi = 0; gi < groups.size(); ++gi) {
        const auto& g = groups[gi];
        bool uniform = true;
        for (size_t k = 1; k < g.size(); ++k) if (g[k] - g[k - 1] != 4 * anc.t_cpu) uniform = false;
        // A run ends at its DONE write, which can fall inside a refresh group: a last group of one to
        // three reads whose next read would come after the end of the log, and whose first read is a
        // line mark on F1's cadence, is a refresh group cut short by the end of the run.
        bool cut_short = false;
        if (g.size() < 4 && gi > 0 && gi + 1 == groups.size() && log_end < g.back() + 4 * anc.t_cpu) {
            uint64_t d = g.front() - groups[gi - 1].front();
            cut_short = (d == 454 * anc.t_cpu || d == 456 * anc.t_cpu);
        }
        if (uniform && (g.size() == 4 || cut_short)) for (uint64_t c : g) refresh.insert(c);
    }
    bool ok = true;
    for (const auto& a : log) {
        if (a.cycle < lo || a.cycle > hi) continue;
        if (!a.is_write && a.word_addr < 0x1000 && refresh.count(a.cycle)) continue;   // refresh
        if (in_range(a.word_addr, 0x080000, 0x081000)) continue;   // program code
        if (in_range(a.word_addr, 0x0C0000, 0x0C1000)) continue;   // results + DONE_ADDR
        if (in_range(a.word_addr, 0x0C1000, 0x0C1020)) continue;   // markers
        if (in_range(a.word_addr, 0x0C2000, 0x0C2020)) continue;   // parameter block
        if (in_range(a.word_addr, 0x0C4000, 0x0C4002)) continue;   // K3's byte
        ok = false;
        if (saw_extra) *saw_extra = true;
    }
    return ok;
}

void register_run(const std::string& bin, bool loaded, bool done, const Ep1Words& ep1,
                  const std::vector<nmh::RamAccess>& log) {
    if (!registering()) return;
    RunRecord r;
    r.bin = bin; r.loaded = loaded; r.done = done;
    if (!loaded) { run_registry().push_back(r); return; }
    r.ep1 = true;
    for (const auto& w : ep1) if (!w.has_value() || *w != 0) r.ep1 = false;
    r.f1 = measure_anchor(log).ok;
    r.ep2_graded = done && r.f1;
    if (r.ep2_graded) {
        auto s = mark(log, kMkStart);
        uint64_t last = log.empty() ? 0 : log.back().cycle;
        r.ep2 = s.has_value() && ep2_ok(log, *s, last);
    }
    run_registry().push_back(r);
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

// ---- P4: one latch run, and one instance's six latch runs ------------------
// SPECIFICATION.md P4. The same two functions grade the timer's own runs and
// P4-GREENCTL's CPU-driven pulse source.
struct P4LatchResult { bool loaded=false, pre43_ok=false, coverage_ok=false, pulse_ok=false; int64_t P=0; };

P4LatchResult grade_p4_latch(const std::string& bin, uint16_t n_mask, uint16_t icrmask) {
    P4LatchResult r;
    Capture c = run_capture(bin, kRunBudget, {});
    SampleSeries prb = read_series(c.log, kResults+8, 400);
    SampleSeries icr = read_series(c.log, kResults+8+400*2, 400);
    Anchor anc = measure_anchor(c.log);
    r.loaded = c.loaded && c.done.has_value() && anc.ok;
    if (!r.loaded) return r;
    uint64_t t0 = 0; bool have = false;
    std::vector<int64_t> e_all; std::vector<bool> bit_all;
    for (int i=0;i<400;++i) {
        if (!prb.cycle[i] || !prb.value[i]) continue;
        if (!have) { t0 = *prb.cycle[i]; have = true; }
        int64_t e = (int64_t)e_index(*prb.cycle[i], t0, anc.t_cpu);
        e_all.push_back(e);
        bit_all.push_back((*prb.value[i] & n_mask) != 0);
    }
    // F3's own f_j/q_j are the ICR stores' own E-indices (not PRB's).
    PeriodResult pr = measure_period(icr, icrmask, anc.t_cpu);
    r.pre43_ok = pr.ok;
    r.P = pr.P;
    if (!pr.ok || pr.P <= 0) return r;
    bool found = false;
    for (int64_t cand=0; cand<pr.P; ++cand) {
        bool ok = true; int highcount = 0;
        for (size_t i=0;i<e_all.size();++i) {
            int64_t res = ((e_all[i]-cand)%pr.P+pr.P)%pr.P;
            bool ishigh = (res==0);
            if (ishigh) ++highcount;
            if (bit_all[i] != ishigh) { ok = false; break; }
        }
        if (ok && highcount >= 3) { found = true; break; }
    }
    r.pulse_ok = found;
    std::vector<int> covcount(pr.P, 0);
    for (auto e : e_all) covcount[((e%pr.P)+pr.P)%pr.P]++;
    bool cov = true; for (auto cnt : covcount) if (cnt < 2) { cov = false; break; }
    r.coverage_ok = cov;
    return r;
}

struct P4InstanceSummary { bool ok=false; int usable=0; };

// bins[i] is the run for latch Ls[i]; note_tag names the measured lines.
P4InstanceSummary grade_p4_instance(Checks& checks, const std::string& note_tag, const std::string& tag,
                                    const std::vector<std::string>& bins, const std::vector<int>& Ls,
                                    uint16_t n_mask, uint16_t icrmask) {
    P4InstanceSummary s;
    std::vector<P4LatchResult> rs;
    for (size_t i=0;i<bins.size();++i) rs.push_back(grade_p4_latch(bins[i], n_mask, icrmask));
    for (size_t i=0;i<rs.size();++i)
        checks.note(note_tag + " " + tag + " L=" + std::to_string(Ls[i]) + ": measured P(L)=" + std::to_string(rs[i].P) +
                    (rs[i].pre43_ok ? " (uniquely determined)" : " (not uniquely determined)") +
                    ", coverage " + (rs[i].coverage_ok ? "met" : "not met"));
    std::vector<int64_t> deltas; bool pulse_all = true;
    for (size_t i=0;i<rs.size();++i) {
        if (!rs[i].loaded || !rs[i].pre43_ok || !rs[i].coverage_ok) continue;
        ++s.usable;
        deltas.push_back(rs[i].P - Ls[i]);
        if (!rs[i].pulse_ok) pulse_all = false;
    }
    bool delta_ok = !deltas.empty();
    for (auto d : deltas) if (d != deltas[0] || (d != 0 && d != 1)) delta_ok = false;
    s.ok = (s.usable >= 2) && pulse_all && delta_ok;
    return s;
}

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    // P10's own aggregate rows need the verdict of a sub-proposition after
    // its own block has gone out of scope; each block below sets the field
    // here as it computes its own verdict (SPECIFICATION.md P10: "none of
    // P1-P9 carries" a bitmap/multi-proposition claim id by itself).
    struct PropVerdicts {
        bool p3_ta=false, p3_tb=false, p4_ta=false, p4_tb=false, p4_graded=false;
        bool p5_ta=false, p5_tb=false;
        bool p70=false, p73=false, p74=false, p75=false, p76_ciaa=false, p76_ciab=false, p77=false;
        bool p80=false, p83=false, p84=false;
        bool p865_01=false, p865_10=false, p865_11=false;
        bool p87_alarm=false;
        bool p9_legf=false, p9_legw=false, p9_legw_graded=true;
        bool p1_control=false;
    } pv;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    // ---- fixture, asserted first (SPECIFICATION.md 1.2 E7) ------------
    {
        NoRegister dedicated;
        Capture f = run_capture(dir + (mv == "FIXTURE" ? "/p1_ciaa_ta_fixture.bin" : "/p1_ciaa_ta.bin"),
                                kRunBudget, {kFixtureAddr});
        checks.expect(f.loaded && f.done.has_value() && pk_ok(f,0) && pk(f,0) == kFixtureValue,
            "FIXTURE [SPECIFICATION.md 1.2]: the program can deliver a known stored constant "
            "before any other result is graded");
    }

    // ---- P1 instances -----------------------------------------------------
    std::string p1_ciaa_ta = dir + "/p1_ciaa_ta.bin";
    std::string p1_ciaa_tb = dir + (mv == "EP1-PER-RUN" ? "/p1_ciaa_tb_ep1.bin"
                                    : mv == "EP2-PER-RUN" ? "/p1_ciaa_tb_ep2.bin"
                                    : mv == "EP3-PER-RUN" ? "/p1_ciaa_tb_ep3.bin"
                                    : mv == "F1-ANCHOR-PER-RUN" ? "/p1_ciaa_tb_f1.bin"
                                    : "/p1_ciaa_tb.bin");
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

    // ---- P1-MAIN, split by id: timer-A instances carry the CRA id,
    // timer-B instances the CRB ids, and the p341/p342 ids sit on the
    // conjunction of all four plus the other-bits clause.
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
    // light. The three prints below are the raw level fdd_led read at each
    // point, not a comparison against the page's own prediction.
    auto mk_cycle = [&](uint32_t addr) { return mark(p1d.log, addr); };
    auto lc2 = mk_cycle(kMkLpOnEnd), lc3 = mk_cycle(kMkLpOffEnd);
    std::optional<uint8_t> led_on_leg, led_off_leg;
    if (lc2) led_on_leg = p1d.led_at(*lc2);
    if (lc3) led_off_leg = p1d.led_at(*lc3);
    auto led_str = [](std::optional<uint8_t> v) { return v ? (*v != 0 ? "on" : "off") : "?"; };
    checks.record("P1-LP/P1-O2 [p244]: fdd_led follows disk DMA, not MTR* latched at drive select "
                   "(NM-HRM-046) -- measured: LED when the CPU leaves reset=" +
                   std::string(p1d.led_before_boot?"on":"off") +
                   ", after R-SELECT with MTR*=0 (on leg)=" + led_str(led_on_leg) +
                   ", after R-SELECT with MTR*=1 (off leg)=" + led_str(led_off_leg) +
                   " -- reads off throughout this run, on both legs: this package starts no disk DMA, "
                   "and R-SELECT's own select/motor write is not a proven-live input at this port -- "
                   "O2's own legs (LED-A1, LED-C, LED-D1, LED-D2) are not graded");

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
        pv.p1_control = control_ok;
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

    // ---- component lines (no claim id: SPECIFICATION.md P2 puts
    // opus:F-timers.c14 on one assertion over legs R, S and H together,
    // with each leg's own line carrying no id) --------------------------
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
                      "CR<-$06)");
        (void)h1_bit;
    }

    // ---- environment preconditions (EP1-EP3, F1 anchor), on one dedicated run
    // each (the per-run assertions at the end of main grade every capture) ---
    {
        NoRegister dedicated;
        std::string bin = dir + (mv == "EP1" ? "/ep1_contrast.bin" : "/p1_ciaa_ta.bin");
        Capture e = run_capture(bin, kRunBudget, {kEp1CraA,kEp1CrbA,kEp1CraB,kEp1CrbB});
        bool ok = e.loaded && e.done.has_value() && pk_ok(e,0) && pk_ok(e,1) && pk_ok(e,2) && pk_ok(e,3) &&
                  pk(e,0)==0 && pk(e,1)==0 && pk(e,2)==0 && pk(e,3)==0;
        checks.expect(ok, "EP1 [p354]: the first reads of CRA and CRB on both CIAs, made before any write "
                          "to them, return $00");
    }
    {
        NoRegister dedicated;
        std::string bin = dir + (mv == "EP2" ? "/ep2_contrast.bin" : mv == "EP2-LOW" ? "/ep2_contrast_low.bin"
                                                                                      : "/p1_ciaa_ta.bin");
        Capture e = run_capture(bin, kRunBudget, {});
        auto s = mark(e.log, kMkStart);
        bool ok = e.loaded && e.done.has_value() && s.has_value() && ep2_ok(e.log, *s, *e.done);
        checks.expect(ok,
            "EP2 [SPECIFICATION.md 1.2]: between the first and last marker, the Chip-memory port "
            "shows only CPU accesses and memory-refresh accesses");
    }
    {
        NoRegister dedicated;
        std::string bin = dir + (mv == "EP3" ? "/p1_ciaa_ta_ep3.bin" : "/p1_ciaa_ta.bin");
        Capture e = run_capture(bin, kRunBudget, {});
        checks.expect(e.loaded && e.done.has_value(),
            "EP3 [SPECIFICATION.md 1.2]: the program reaches DONE and has a final marker");
    }
    {
        NoRegister dedicated;
        Capture e = run_capture(dir + (mv == "F1-ANCHOR" ? "/p1_ciaa_ta_f1.bin" : "/p1_ciaa_ta.bin"), kRunBudget, {});
        Anchor anc = measure_anchor(e.log);
        checks.expect(anc.ok, "F1-ANCHOR [p194,p195,p23]: consecutive refresh line-marks are 454*T_CPU apart "
                              "(or alternately 456/454*T_CPU), measured from the run's own refresh reads");
        bool contrast_fails = anc.ok && !anchor_holds_at(anc, double(anc.t_cpu) * 1.25);
        checks.expect(contrast_fails, "F1-ANCHOR-CONTRAST [p194,p195,p23]: the same comparator, evaluated "
                                      "against 1.25x the measured T_CPU, fails");
    }

    // ---- P3 (toggle mode inverts on every underflow, and at no other time) -
    struct P3Result { bool loaded=false, pairing_ok=false, level_ok=false, pre31_ok=false, pre32_ok=false; int nT=0,nQ=0; };
    auto p3_predicate = [](const P3Result& r) { return r.pairing_ok && r.level_ok; };
    auto grade_p3 = [&](const std::string& bin, uint16_t n_mask, uint16_t icrmask) {
        P3Result s;
        Capture c = run_capture(bin, kRunBudget, {});
        SampleSeries prb = read_series(c.log, kResults+8, 600);
        SampleSeries icr = read_series(c.log, kResults+8+600*2, 600);
        s.loaded = c.loaded && c.done.has_value();
        if (!s.loaded) return s;
        std::vector<bool> bits(600,false), flags(600,false);
        for (int i=0;i<600;++i) {
            bits[i] = prb.value[i] && (*prb.value[i] & n_mask);
            flags[i] = icr.value[i] && (*icr.value[i] & icrmask);
        }
        std::vector<int> T, Q;
        for (int i=0;i+1<600;++i) if (bits[i] != bits[i+1]) T.push_back(i);
        for (int i=0;i<600;++i) if (flags[i]) Q.push_back(i);
        s.nT = (int)T.size(); s.nQ = (int)Q.size();
        std::vector<bool> usedT(T.size(), false);
        bool pairing_ok = true;
        for (int q : Q) {
            if (q == 599) continue;   // the last sample's own flag has no later PRB read to pair with
            bool found = false;
            for (size_t k=0;k<T.size();++k) if (!usedT[k] && std::abs(T[k]-q) <= 1) { usedT[k]=true; found=true; break; }
            if (!found) { pairing_ok = false; break; }
        }
        for (size_t k=0;k<T.size();++k) if (!usedT[k]) pairing_ok = false;
        s.pairing_ok = pairing_ok;
        s.level_ok = !Q.empty() && bits[0];
        bool pre31 = true;
        for (size_t i=1;i<Q.size();++i) if (Q[i]-Q[i-1] < 3) { pre31=false; break; }
        s.pre31_ok = pre31;
        s.pre32_ok = Q.size() >= 16;
        return s;
    };
    {
        P3Result r_a = grade_p3(dir+"/p3_ciaa_ta.bin", 0x40, kIcrTa);
        P3Result r_b = grade_p3(dir+"/p3_ciaa_tb.bin", 0x80, kIcrTb);
        P3Result r_c = grade_p3(dir+"/p3_ciab_ta.bin", 0x40, kIcrTa);
        P3Result r_d = grade_p3(dir+"/p3_ciab_tb.bin", 0x80, kIcrTb);
        // Gated by Pre1.4/O1 (the same fact P1/P2 reuse): CIA-A's own PRB
        // read never shows a driven level, so it is not graded and its own
        // read-back is recorded instead (SPECIFICATION.md P1 Pre1.4).
        pv.p3_ta = (!ga.o1_live || p3_predicate(r_a)) && (!gc.o1_live || p3_predicate(r_c));
        pv.p3_tb = (!gb.o1_live || p3_predicate(r_b)) && (!gd.o1_live || p3_predicate(r_d));
        bool p3_ok = pv.p3_ta && pv.p3_tb;
        checks.expect(p3_ok, "P3 [p342,p348,p349,p350]: every underflow has its transition and every "
                      "transition has its underflow (paired within one iteration), and the level before "
                      "the first flag is 1, for the instances whose read-back is live (Pre1.4/O1)");
        checks.record("P3 [p342], CIA-A: PRB reads " + hex2(0xFF) + " in all 600 samples of both "
                      "instances while underflows continue to flag (CIAA/TA Q=" + std::to_string(r_a.nQ) +
                      ", CIAA/TB Q=" + std::to_string(r_b.nQ) + ") -- the same dead read-back P1-PRE1.4/O1 "
                      "already records; not graded");
        bool p3_pre_ok = r_a.pre31_ok && r_a.pre32_ok && r_b.pre31_ok && r_b.pre32_ok &&
                         r_c.pre31_ok && r_c.pre32_ok && r_d.pre31_ok && r_d.pre32_ok;
        checks.expect(p3_pre_ok, "P3-PRE [SPECIFICATION.md P3]: underflows are separated by at least 3 "
                      "iterations, and there are at least 16 of them, for all four instances");
        checks.note("P3 CIAA/TA T=" + std::to_string(r_a.nT) + " Q=" + std::to_string(r_a.nQ) +
                    "; CIAA/TB T=" + std::to_string(r_b.nT) + " Q=" + std::to_string(r_b.nQ) +
                    "; CIAB/TA T=" + std::to_string(r_c.nT) + " Q=" + std::to_string(r_c.nQ) +
                    "; CIAB/TB T=" + std::to_string(r_d.nT) + " Q=" + std::to_string(r_d.nQ));

        // Pre3.1's and Pre3.2's own contrasts are the page's own contrasting
        // cases: moved out of the normal run, selected only through
        // NMH_MUTATE.
        if (mv == "P3-PRE31CONTRAST") {
            P3Result r_l2 = grade_p3(dir+"/p3_ciab_tb_l2.bin", 0x80, kIcrTb);
            checks.expect(r_l2.loaded && !r_l2.pre31_ok, "P3-PRE31-CONTRAST [SPECIFICATION.md P3]: L=2 "
                          "makes flags appear in consecutive reads (CIAB/timer B)");
        }
        if (mv == "P3-PRE32CONTRAST") {
            P3Result r_ns = grade_p3(dir+"/p3_ciab_tb_nostart.bin", 0x80, kIcrTb);
            checks.expect(r_ns.loaded && r_ns.nQ == 0, "P3-PRE32-CONTRAST [SPECIFICATION.md P3]: CR<-$06 "
                          "(not started) gives |Q|=0 (CIAB/timer B)");
        }
        // P3-CONTRAST-PULSE/PBON0: recorded, not graded -- the baseline
        // already fails the same way (PRB constant x 600, T=0 in all three
        // runs), so neither shows anything a green baseline's own failure
        // under mutation would (BRIEF: "For an assertion that is already
        // red, show ... a proven-live input through the same comparator
        // goes GREEN" -- P3-GREENCTL below is that input).
        if (mv == "P3-CONTRAST-PULSE" || mv == "P3-CONTRAST-PBON0") {
            P3Result r_pulse = grade_p3(dir+"/p3_ciab_tb_pulse.bin", 0x80, kIcrTb);
            checks.record("P3-CONTRAST-PULSE [p342]: OUTMODE=0 (pulse), same latch: T=" +
                          std::to_string(r_pulse.nT) + " Q=" + std::to_string(r_pulse.nQ) +
                          " -- not distinguishable on this build: the baseline already fails the pairing "
                          "the same way (CIAB/timer B)");
            P3Result r_pbon0 = grade_p3(dir+"/p3_ciab_tb_pbon0.bin", 0x80, kIcrTb);
            checks.record("P3-CONTRAST-PBON0 [p342]: PBON=0 (input): T=" + std::to_string(r_pbon0.nT) +
                          " Q=" + std::to_string(r_pbon0.nQ) + " -- not distinguishable on this build: "
                          "the baseline already fails the pairing the same way (CIAB/timer B)");
        }
        // P3-GREENCTL: a CPU-driven toggle (PBON=0, DDRB bit 7 an output, the
        // CPU inverts PRB bit 7 after every flagged ICR read) through the whole
        // predicate P3's line grades: the pairing and the level before the
        // first flag.
        {
            P3Result r_cpu = grade_p3(dir + (mv == "P3-GREENCTL" ? "/p3_ciab_tb_cputoggle_low.bin"
                                                                 : "/p3_ciab_tb_cputoggle.bin"), 0x80, kIcrTb);
            checks.expect(r_cpu.loaded && p3_predicate(r_cpu), "P3-GREENCTL [SPECIFICATION.md P3]: the CPU "
                          "inverts PRB bit 7 once per flagged ICR read (PBON=0, DDRB bit 7 an output) -- "
                          "the pairing and the level before the first flag both hold through the same "
                          "predicate P3's own line grades, once the last sample's own unpaired flag is "
                          "excluded (CIAB/timer B)");
        }
    }

    // ---- P4 (pulse mode: low except one phi2 cycle at every underflow) ----
    {
        const std::vector<int> p4_ls = {20,21,22,23,24,25};
        auto p4_bins = [&](const std::string& prefix, const std::string& tag, const std::vector<int>& ls) {
            std::vector<std::string> v;
            for (int L : ls) v.push_back(dir+"/"+prefix+"_"+tag+"_l"+std::to_string(L)+".bin");
            return v;
        };
        P4InstanceSummary s_a = grade_p4_instance(checks, "P4", "ciaa_ta", p4_bins("p4","ciaa_ta",p4_ls), p4_ls, 0x40, kIcrTa);
        P4InstanceSummary s_b = grade_p4_instance(checks, "P4", "ciaa_tb", p4_bins("p4","ciaa_tb",p4_ls), p4_ls, 0x80, kIcrTb);
        P4InstanceSummary s_c = grade_p4_instance(checks, "P4", "ciab_ta", p4_bins("p4","ciab_ta",p4_ls), p4_ls, 0x40, kIcrTa);
        P4InstanceSummary s_d = grade_p4_instance(checks, "P4", "ciab_tb", p4_bins("p4","ciab_tb",p4_ls), p4_ls, 0x80, kIcrTb);
        // P4's own pulse-shape clause is gated by Pre1.4/O1 (CIA-A's PRB
        // read never shows a driven level, NM-HRM-054); P4-PRE (the usable-
        // latch count, F3-based) does not depend on port B and is not
        // gated -- it is the same on all four instances.
        bool p4_pre_ok = s_a.usable>=2 && s_b.usable>=2 && s_c.usable>=2 && s_d.usable>=2;
        pv.p4_graded = p4_pre_ok;
        checks.expect(p4_pre_ok, "P4-PRE [SPECIFICATION.md P4]: at least 2 of the 6 latch values are "
                      "usable (coverage and a unique period), for all four instances");
        pv.p4_ta = (!ga.o1_live || s_a.ok) && (!gc.o1_live || s_c.ok);
        pv.p4_tb = (!gb.o1_live || s_b.ok) && (!gd.o1_live || s_d.ok);
        bool p4_ok = pv.p4_ta && pv.p4_tb;
        // SPECIFICATION.md P4: a run whose own Pre4.2/Pre4.3 fails "is not
        // graded", reported rather than printed FAIL.
        if (!p4_pre_ok) {
            checks.record("P4 [p341,p342]: not graded -- P4-PRE (at least 2 usable latches) failed on at "
                          "least one instance");
        } else {
            checks.expect(p4_ok, "P4 [p341,p342]: P(L) is L or L+1 (constant offset), with one residue "
                          "high (every sample in it, no other residue ever high, >=3 high samples), for "
                          "the instances whose read-back is live (Pre1.4/O1), on the usable latches "
                          "(Pre4.2/Pre4.3)");
        }
        checks.record("P4 [p341,p342], CIA-A: PRB reads bit n=1 in all 400 samples of every run (the "
                      "same dead read-back P1-PRE1.4/O1 records) -- not graded");
        checks.note("P4 usable latches: CIAA/TA=" + std::to_string(s_a.usable) + " CIAA/TB=" + std::to_string(s_b.usable) +
                    " CIAB/TA=" + std::to_string(s_c.usable) + " CIAB/TB=" + std::to_string(s_d.usable));

        // P4-GREENCTL: the unchanged P4 comparator (grade_p4_instance) on a live
        // pulse source. The CPU, not the timer, drives DDRB bit n's data level
        // (PBON=0): each iteration reads the running timer's own count and
        // writes a level that is high only when the count equals one value, so
        // the pulse recurs at one fixed phase of the timer's own period. The
        // latch values are those with P(L) at least twice the loop's 16 E
        // spacing (F3).
        {
            const std::vector<int> ctl_ls = {40,41,42,43,44,45};
            auto ctl_tb = p4_bins("p4c","ciab_tb",ctl_ls);
            if (mv == "P4-GREENCTL") ctl_tb[0] = dir + "/p4c_ciab_tb_l40_two.bin";
            P4InstanceSummary g_a = grade_p4_instance(checks, "P4-GREENCTL", "ciab_ta", p4_bins("p4c","ciab_ta",ctl_ls), ctl_ls, 0x40, kIcrTa);
            P4InstanceSummary g_b = grade_p4_instance(checks, "P4-GREENCTL", "ciab_tb", ctl_tb, ctl_ls, 0x80, kIcrTb);
            checks.note("P4-GREENCTL usable latches: CIAB/TA=" + std::to_string(g_a.usable) + " CIAB/TB=" + std::to_string(g_b.usable));
            checks.expect(g_a.ok && g_b.ok, "P4-GREENCTL [SPECIFICATION.md P4]: the same P4 comparator passes "
                          "on a CPU-driven pulse at one fixed phase of the timer's period (PBON=0, DDRB bit n "
                          "an output, latches 40-45; the timer-A and timer-B instances of CIAB)");
        }
    }

    // ---- P9 (the latch is the divider of the input clock) -----------------
    {
        // Leg F: reuses P3's own mechanism (toggle mode, R-SAMPLE(600;PRB,ICR)),
        // CIAB/timer B, for L in {40,41,64,100} (SPECIFICATION.md P9 leg F).
        auto period_of = [&](const std::string& bin, uint16_t icrmask) -> PeriodResult {
            Capture c = run_capture(bin, kRunBudget, {});
            SampleSeries icr = read_series(c.log, kResults+8+600*2, 600);
            Anchor anc = measure_anchor(c.log);
            if (!c.loaded || !c.done.has_value() || !anc.ok) return PeriodResult{};
            return measure_period(icr, icrmask, anc.t_cpu);
        };
        PeriodResult p40 = period_of(dir+"/p9_legf_ciab_tb_l40.bin", kIcrTb);
        PeriodResult p41 = period_of(dir+"/p9_legf_ciab_tb_l41.bin", kIcrTb);
        PeriodResult p64 = period_of(dir+"/p9_legf_ciab_tb_l64.bin", kIcrTb);
        PeriodResult p100 = period_of(dir+"/p9_legf_ciab_tb_l100.bin", kIcrTb);
        bool all_ok = p40.ok && p41.ok && p64.ok && p100.ok;
        int64_t d40=p40.P-40, d41=p41.P-41, d64=p64.P-64, d100=p100.P-100;
        bool delta_ok = all_ok && d40==d41 && d41==d64 && d64==d100 && (d40==0||d40==1);
        pv.p9_legf = delta_ok;
        // opus:F-timers.c6/sol:F-timers.c8 bundle more than one proposition
        // (P4, leg F, leg W and P8.65 mode 10); P10 carries them (its own
        // row), not this component line (SPECIFICATION.md P10: "None
        // graded whole here").
        checks.expect(delta_ok, "P9-LEGF [p341]: delta = P(L)-L is the same for L=40,41,64,100 and is 0 "
                      "or 1 (CIAB/timer B)");
        checks.note("P9-LEGF P(40)=" + std::to_string(p40.P) + " P(41)=" + std::to_string(p41.P) +
                    " P(64)=" + std::to_string(p64.P) + " P(100)=" + std::to_string(p100.P));

        // "Consecutive PRB transitions are P(L) +- s E periods apart"
        // (SPECIFICATION.md P9), s the PRB sampling spacing (leg F's own
        // R-SAMPLE2 iteration time in E periods, measured, not assumed).
        auto transitions_of = [&](const std::string& bin, const PeriodResult& pr) {
            Capture c = run_capture(bin, kRunBudget, {});
            SampleSeries prb = read_series(c.log, kResults+8, 600);
            Anchor anc = measure_anchor(c.log);
            bool ok = false; double s_e = 0;
            if (!c.loaded || !c.done.has_value() || !anc.ok || !pr.ok) return ok;
            uint64_t t0 = 0; bool have = false; std::vector<int64_t> e_all; std::vector<bool> bit_all;
            double sum = 0; int n = 0; std::optional<uint64_t> prev;
            for (int i=0;i<600;++i) {
                if (!prb.cycle[i] || !prb.value[i]) continue;
                if (!have) { t0 = *prb.cycle[i]; have = true; }
                e_all.push_back((int64_t)e_index(*prb.cycle[i], t0, anc.t_cpu));
                bit_all.push_back((*prb.value[i] & 0x80) != 0);
                if (prev) { sum += double(*prb.cycle[i] - *prev) / (10.0*double(anc.t_cpu)); ++n; }
                prev = prb.cycle[i];
            }
            s_e = n ? sum / n : 0;
            std::vector<int64_t> trans;
            for (size_t i=0;i+1<bit_all.size();++i) if (bit_all[i] != bit_all[i+1]) trans.push_back(e_all[i+1]);
            if (trans.size() < 2) return ok;
            ok = true;
            for (size_t i=1;i<trans.size();++i)
                if (std::fabs(double(trans[i]-trans[i-1]) - double(pr.P)) > s_e + 0.5) { ok = false; break; }
            return ok;
        };
        bool trans_ok = transitions_of(dir+"/p9_legf_ciab_tb_l40.bin", p40);
        checks.expect(trans_ok, "P9-LEGF-TRANSITIONS [p341,p342]: consecutive PRB transitions are P(L) "
                      "+-s E periods apart, s the sampling spacing (CIAB/timer B, L=40)");

        // P9-LEGF-TRANSITIONS-GREENCTL: the same period measurement and the same
        // transition-spacing comparator on a live transition train. The CPU
        // (PBON=0, DDRB bit 7 an output) inverts PRB bit 7 after every flagged
        // ICR read, so its transitions follow the underflows, L=40.
        {
            std::string cbin = dir + (mv == "P9-LEGF-TRANSITIONS-GREENCTL" ? "/p9_legf_ciab_tb_cputoggle_skip.bin"
                                                                           : "/p9_legf_ciab_tb_cputoggle.bin");
            PeriodResult pc = period_of(cbin, kIcrTb);
            bool ctl_ok = transitions_of(cbin, pc);
            checks.note("P9-LEGF-TRANSITIONS-GREENCTL P(40)=" + std::to_string(pc.P) +
                        (pc.ok ? " (uniquely determined)" : " (not uniquely determined)"));
            checks.expect(ctl_ok, "P9-LEGF-TRANSITIONS-GREENCTL [p341,p342]: the same comparator passes on "
                          "a CPU-driven transition train that follows the underflows (CIAB/timer B, L=40, "
                          "PBON=0)");
        }

        // Leg W: P2 leg S's own mechanics, CIAB/timer B, for L in {40,100}.
        // Each run also stores the level before its start (Pre2.3) and, after
        // the sampler, the stop-poll count and the ICR value (Pre2.2).
        constexpr uint32_t kP9wPoll = kResults + 8 + 300*2;
        constexpr uint32_t kP9wI    = kP9wPoll + 2;
        constexpr uint32_t kP9wS0   = kP9wPoll + 4;
        struct WidthRun {
            bool loaded=false;
            std::optional<double> w;
            double s_e=0;
            bool underflow_ok=false;   // Pre2.2: the one-shot had ended when the sampler closed, TB flag read set
            bool level_ok=false;       // Pre2.3: the level before the start is 0
            uint16_t poll=0, icr=0, s0=0;
        };
        auto width_of = [&](const std::string& bin) -> WidthRun {
            WidthRun r;
            Capture c = run_capture(bin, kRunBudget, {kP9wPoll, kP9wI, kP9wS0});
            Anchor anc = measure_anchor(c.log);
            auto mpost = mark(c.log, 0x0C1004);
            r.loaded = c.loaded && c.done.has_value();
            if (pk_ok(c,0)) r.poll = pk(c,0);
            if (pk_ok(c,1)) r.icr = pk(c,1);
            if (pk_ok(c,2)) r.s0 = pk(c,2);
            r.underflow_ok = r.loaded && pk_ok(c,0) && pk_ok(c,1) && r.poll == 1 && (r.icr & kIcrTb);
            r.level_ok = r.loaded && pk_ok(c,2) && !(r.s0 & 0x80);
            if (!r.loaded || !anc.ok || !mpost) return r;
            // NSAMP differs per L (200 for 40, 300 for 100); read a generous 300-slot window --
            // slots beyond the program's own NSAMP simply have no value and are skipped.
            SampleSeries s = read_series(c.log, kResults+8, 300);
            bool prev_high = false; bool have_prev = false;
            double sum = 0; int n = 0; std::optional<uint64_t> prev_cyc;
            for (size_t i=0;i<s.size();++i) {
                if (!s.cycle[i] || !s.value[i]) continue;
                if (prev_cyc) { sum += double(*s.cycle[i] - *prev_cyc) / (10.0*double(anc.t_cpu)); ++n; }
                prev_cyc = s.cycle[i];
                bool high = (*s.value[i] & 0x80) != 0;
                if (have_prev && prev_high && !high && !r.w) r.w = e_index(*s.cycle[i], *mpost, anc.t_cpu);
                prev_high = high; have_prev = true;
            }
            r.s_e = n ? sum / n : 0;
            return r;
        };
        // The width comparator, shared by the leg-W runs and P9-LEGW-GREENCTL.
        auto width_ok = [](const WidthRun& a, const WidthRun& b) {
            double s_meas = std::max(a.s_e, b.s_e);
            return a.w.has_value() && b.w.has_value() && std::fabs((*b.w - *a.w) - 60.0) <= s_meas + 0.5;
        };
        auto width_str = [](const WidthRun& r) { return r.w ? std::to_string(*r.w) : std::string("?"); };

        std::string w40bin = dir + (mv == "P9-LEGW-PRE22" ? "/p9_legw_l40_nouf.bin"
                                    : mv == "P9-LEGW-PRE23" ? "/p9_legw_l40_lvl1.bin"
                                    : "/p9_legw_l40.bin");
        WidthRun w40 = width_of(w40bin);
        WidthRun w100 = width_of(dir+"/p9_legw_l100.bin");
        bool w_pre22 = w40.underflow_ok && w100.underflow_ok;
        bool w_pre23 = w40.level_ok && w100.level_ok;
        checks.expect(w40.underflow_ok, "P9-LEGW-PRE2.2 [p342]: the one-shot has ended when the sampler "
                      "closes (CR bit 0 read 0 at the first poll) and the ICR read shows the TB flag "
                      "(CIAB/timer B, L=40)");
        checks.expect(w100.underflow_ok, "P9-LEGW-PRE2.2 [p342]: the one-shot has ended when the sampler "
                      "closes (CR bit 0 read 0 at the first poll) and the ICR read shows the TB flag "
                      "(CIAB/timer B, L=100)");
        checks.expect(w40.level_ok, "P9-LEGW-PRE2.3 [p342]: the level before the start is 0 "
                      "(CIAB/timer B, L=40)");
        checks.expect(w100.level_ok, "P9-LEGW-PRE2.3 [p342]: the level before the start is 0 "
                      "(CIAB/timer B, L=100)");
        checks.note("P9-LEGW polls=" + std::to_string(w40.poll) + "/" + std::to_string(w100.poll) +
                    " icr=" + hex2(w40.icr) + "/" + hex2(w100.icr) +
                    " s0=" + hex2(w40.s0) + "/" + hex2(w100.s0) + " (L=40 / L=100)");
        double s_meas = std::max(w40.s_e, w100.s_e);
        bool w_graded = w_pre22 && w_pre23;
        bool w_ok = width_ok(w40, w100);
        pv.p9_legw = w_ok;
        pv.p9_legw_graded = w_graded;
        if (w_graded) {
            checks.expect(w_ok, "P9-LEGW [p341]: W(100)-W(40) = 60+-s, s the run's own measured PRB sampling "
                          "spacing, CIAB/timer B");
        } else {
            checks.record("P9-LEGW [p341]: not graded -- P9-LEGW-PRE2.2 or P9-LEGW-PRE2.3 failed");
        }
        checks.note("P9-LEGW W(40)=" + width_str(w40) + " W(100)=" + width_str(w100) +
                    " s=" + std::to_string(s_meas) + "E");

        // P9-LEGW-GREENCTL: the same width comparator on a live width source.
        // The CPU (PBON=0) sets PRB bit 7 right after the start marker and
        // clears it after the ICR read that shows the underflow; the same
        // underflow and level-before preconditions are asserted on these runs.
        {
            std::string c40bin = dir + (mv == "P9-LEGW-GREENCTL" ? "/p9_legw_ctl_l20.bin"
                                        : mv == "P9-LEGW-GREENCTL-PRE22" ? "/p9_legw_ctl_l40_nouf.bin"
                                        : mv == "P9-LEGW-GREENCTL-PRE23" ? "/p9_legw_ctl_l40_lvl1.bin"
                                        : "/p9_legw_ctl_l40.bin");
            WidthRun c40 = width_of(c40bin);
            WidthRun c100 = width_of(dir+"/p9_legw_ctl_l100.bin");
            checks.expect(c40.underflow_ok, "P9-LEGW-GREENCTL-PRE2.2 [p342]: the one-shot has ended when the "
                          "sampler closes and the sampler read the TB flag (CPU-driven, CIAB/timer B, L=40)");
            checks.expect(c100.underflow_ok, "P9-LEGW-GREENCTL-PRE2.2 [p342]: the one-shot has ended when the "
                          "sampler closes and the sampler read the TB flag (CPU-driven, CIAB/timer B, L=100)");
            checks.expect(c40.level_ok, "P9-LEGW-GREENCTL-PRE2.3 [p342]: the level before the start is 0 "
                          "(CPU-driven, CIAB/timer B, L=40)");
            checks.expect(c100.level_ok, "P9-LEGW-GREENCTL-PRE2.3 [p342]: the level before the start is 0 "
                          "(CPU-driven, CIAB/timer B, L=100)");
            bool c_pre = c40.underflow_ok && c100.underflow_ok && c40.level_ok && c100.level_ok;
            checks.note("P9-LEGW-GREENCTL W(40)=" + width_str(c40) + " W(100)=" + width_str(c100) +
                        " s=" + std::to_string(std::max(c40.s_e, c100.s_e)) + "E");
            if (c_pre) {
                checks.expect(width_ok(c40, c100), "P9-LEGW-GREENCTL [p341]: the same comparator passes on a "
                              "CPU-driven width that follows the latch: W(100)-W(40) = 60+-s (CIAB/timer B, "
                              "PBON=0)");
            } else {
                checks.record("P9-LEGW-GREENCTL [p341]: not graded -- P9-LEGW-GREENCTL-PRE2.2 or "
                              "P9-LEGW-GREENCTL-PRE2.3 failed");
            }
        }

        // Contrasting case: changing L by a delta changes the period by the
        // same delta -- already shown by the four leg-F latches above (each
        // pair is a contrast to the others, per SPECIFICATION.md P9).
        checks.record("P9-CONTRAST [SPECIFICATION.md P9]: every pair of leg F's own latch values (40, "
                      "41, 64, 100) is already a contrast to the others -- a period changed by exactly "
                      "the latch difference, or the \"same for all four\" clause above goes red");
    }

    // ========================================================================
    // P7 (CRA bit layout) / P8 (CRB bit layout). None of P7.0-P7.7/P8.0-P8.7's
    // own assertions carries a claim id (SPECIFICATION.md P7/P8: "None is
    // graded whole by one sub-proposition"); the ids sit on P10's own rows.
    // ========================================================================

    // ---- P7.0 / P8.0 (bit 0, START) ---------------------------------------
    {
        struct SI { const char* tag; std::string bin; uint16_t icrmask; };
        std::vector<SI> insts = {
            {"CIAA/CRA", dir + "/p7_ciaa_start.bin", kIcrTa},
            {"CIAB/CRA", dir + (mv == "P70-PRE" ? "/p7_ciab_start_stpre.bin"
                               : mv == "P70-CONTRAST" ? "/p7_ciab_start_contrast.bin"
                               : "/p7_ciab_start.bin"), kIcrTa},
        };
        std::vector<SI> insts8 = {
            {"CIAA/CRB", dir + "/p8_ciaa_start.bin", kIcrTb},
            {"CIAB/CRB", dir + (mv == "P80-PRE" ? "/p8_ciab_start_stpre.bin"
                               : mv == "P80-CONTRAST" ? "/p8_ciab_start_contrast.bin"
                               : "/p8_ciab_start.bin"), kIcrTb},
        };
        auto grade_start = [&](std::vector<SI>& list, const char* propid, const char* label) {
            bool all_ok = true, pre_ok = true;
            for (auto& s : list) {
                Capture c = run_capture(s.bin, kRunBudget, {kResults+8, kResults+10, kResults+12, kResults+14});
                bool ok = c.loaded && c.done.has_value() && pk_ok(c,0) && pk_ok(c,1) && pk_ok(c,2) &&
                          pk(c,0) == 0x8000 && pk(c,1) == 0x8000 && pk(c,2) < pk(c,1);
                bool pre = pk_ok(c,3) && !(pk(c,3) & s.icrmask);
                all_ok = all_ok && ok;
                pre_ok = pre_ok && pre;
                checks.note(std::string(propid) + " " + s.tag + ": u1=" + hex2(pk(c,0)>>8) + hex2(pk(c,0)&0xFF) +
                            " u2=" + hex2(pk(c,1)>>8) + hex2(pk(c,1)&0xFF) + " u3=" + hex2(pk(c,2)>>8) + hex2(pk(c,2)&0xFF));
            }
            checks.expect(pre_ok, std::string(propid) + "-PRE [p342]: no underflow happens before the stop "
                          "read, on both CIAs");
            checks.expect(all_ok, label);
            return all_ok;
        };
        pv.p70 = grade_start(insts, "P70", "P7.0 [p348]: u1=u2=$8000 (stopped, the latch write loaded the counter) "
                    "and u3<u2 (started then stopped decrements it), on both CIAs");
        pv.p80 = grade_start(insts8, "P80", "P8.0 [p349]: u1=u2=$8000 (stopped, the latch write loaded the counter) "
                    "and u3<u2 (started then stopped decrements it), on both CIAs");
    }

    // ---- P7.3 / P8.3 (bit 3, RUNMODE) --------------------------------------
    {
        auto max_gap_cycles = [](const SampleSeries& s) {
            uint64_t prev = 0; bool have = false; uint64_t mx = 0;
            for (auto& c : s.cycle) { if (!c) continue; if (have) mx = std::max(mx, *c - prev); prev = *c; have = true; }
            return mx;
        };
        auto count_flags = [](const SampleSeries& s, uint16_t mask) {
            int n = 0; for (auto& v : s.value) if (v && (*v & mask)) ++n; return n;
        };
        // one instance's own pair of legs (O: one-shot $09, C: continuous $01),
        // same binary, differing only in the E7 parameter poked before boot.
        struct RunLegs { bool ok=false, contrast_ok=false, pre_ok=false; int flagsO=0, flagsC=0; };
        // The padded mutant (NMH_MUTATE=P73-PRE/P83-PRE) needs a budget
        // beyond kRunBudget to reach DONE (measured: ~4.8-5.3M clk_sys
        // against kRunBudget's 4.0M); without it, the run never finishes
        // and the precondition goes red by an unfinished program, not by
        // its own clause.
        auto grade_run_instance = [&](const std::string& bin, uint16_t icrmask, uint64_t budget) {
            RunLegs r;
            Capture co = run_capture_param(bin, 0x0C2000, 0x09, budget, {kResults+808, kResults+810});
            Capture cc = run_capture_param(bin, 0x0C2000, 0x01, budget, {kResults+808, kResults+810});
            SampleSeries so = read_series(co.log, kResults+8, 400);
            SampleSeries sc = read_series(cc.log, kResults+8, 400);
            r.flagsO = count_flags(so, icrmask);
            r.flagsC = count_flags(sc, icrmask);
            bool cO = co.loaded && co.done.has_value() && pk_ok(co,0) && (pk(co,0)&0x01)==0 && pk_ok(co,1) && pk(co,1)==40;
            bool cC = cc.loaded && cc.done.has_value() && pk_ok(cc,0) && (pk(cc,0)&0x01)!=0;
            r.ok = (r.flagsO == 1) && cO && (r.flagsC > 1) && cC;
            r.contrast_ok = !(r.flagsC == 1) && !(r.flagsO > 1);
            Anchor ancO = measure_anchor(co.log), ancC = measure_anchor(cc.log);
            r.pre_ok = ancO.ok && ancC.ok &&
                       double(max_gap_cycles(so)) <= 20.0*10.0*double(ancO.t_cpu) + 1 &&
                       double(max_gap_cycles(sc)) <= 20.0*10.0*double(ancC.t_cpu) + 1;
            return r;
        };
        RunLegs r7a = grade_run_instance(dir+"/p7_ciaa_run.bin", kIcrTa, kRunBudget);
        std::string p7cb = dir + (mv=="P73-PRE" ? "/p7_ciab_run_pad.bin" : "/p7_ciab_run.bin");
        RunLegs r7b = grade_run_instance(p7cb, kIcrTa, mv=="P73-PRE" ? 10'000'000 : kRunBudget);
        pv.p73 = r7a.ok && r7b.ok;
        checks.expect(pv.p73, "P7.3 [p342,p348]: leg O (one-shot) shows exactly one TA flag and "
                      "START clears itself; leg C (continuous) shows more than one flag and START stays "
                      "set, on both CIAs");
        checks.expect(r7a.contrast_ok && r7b.contrast_ok, "P7.3-CONTRAST [p342,p348]: leg O's own \"exactly "
                      "one\" comparator fails on leg C's data, and leg C's own \"more than one\" comparator "
                      "fails on leg O's data, on both CIAs");
        checks.expect(r7a.pre_ok && r7b.pre_ok, "P7.3-PRE [SPECIFICATION.md P7.3]: consecutive ICR stores "
                      "are at most 20 E periods apart, on both CIAs (CIAB fails it under "
                      "NMH_MUTATE=P73-PRE, run with a raised budget so the padded loop reaches DONE)");
        checks.note("P7.3 CIAA flags O=" + std::to_string(r7a.flagsO) + " C=" + std::to_string(r7a.flagsC) +
                    "; CIAB flags O=" + std::to_string(r7b.flagsO) + " C=" + std::to_string(r7b.flagsC));

        RunLegs r8a = grade_run_instance(dir+"/p8_ciaa_run.bin", kIcrTb, kRunBudget);
        std::string p8cb = dir + (mv=="P83-PRE" ? "/p8_ciab_run_pad.bin" : "/p8_ciab_run.bin");
        RunLegs r8b = grade_run_instance(p8cb, kIcrTb, mv=="P83-PRE" ? 10'000'000 : kRunBudget);
        pv.p83 = r8a.ok && r8b.ok;
        checks.expect(pv.p83, "P8.3 [p342,p349]: leg O (one-shot) shows exactly one TB flag and "
                      "START clears itself; leg C (continuous) shows more than one flag and START stays "
                      "set, on both CIAs");
        checks.expect(r8a.contrast_ok && r8b.contrast_ok, "P8.3-CONTRAST [p342,p349]: leg O's own \"exactly "
                      "one\" comparator fails on leg C's data, and leg C's own \"more than one\" comparator "
                      "fails on leg O's data, on both CIAs");
        checks.expect(r8a.pre_ok && r8b.pre_ok, "P8.3-PRE [SPECIFICATION.md P8.3]: consecutive ICR stores "
                      "are at most 20 E periods apart, on both CIAs (CIAB fails it under "
                      "NMH_MUTATE=P83-PRE, run with a raised budget so the padded loop reaches DONE)");
        checks.note("P8.3 CIAA flags O=" + std::to_string(r8a.flagsO) + " C=" + std::to_string(r8a.flagsC) +
                    "; CIAB flags O=" + std::to_string(r8b.flagsO) + " C=" + std::to_string(r8b.flagsC));
    }

    // ---- P7.4 / P8.4 (bit 4, LOAD) -----------------------------------------
    {
        struct LoadResult { bool loaded=false; uint16_t v1=0,v2=0,c=0,v3=0; uint64_t mkA=0,mkB=0,mkC=0,mkD=0,mkRun=0; uint64_t t_cpu=0; bool anc_ok=false; };
        auto run_load = [&](const std::string& bin) {
            LoadResult r;
            Capture c = run_capture(bin, kRunBudget, {kResults+8,kResults+10,kResults+12,kResults+14});
            r.loaded = c.loaded && c.done.has_value();
            if (!r.loaded) return r;
            r.v1 = pk(c,0); r.v2 = pk(c,1); r.c = pk(c,2); r.v3 = pk(c,3);
            auto a = mark(c.log, 0x0C1002), b = mark(c.log, 0x0C1004), cc = mark(c.log, 0x0C1006), d = mark(c.log, 0x0C1008);
            auto rs = mark(c.log, 0x0C100A);
            if (a) r.mkA = *a; if (b) r.mkB = *b; if (cc) r.mkC = *cc; if (d) r.mkD = *d; if (rs) r.mkRun = *rs;
            Anchor anc = measure_anchor(c.log);
            r.anc_ok = anc.ok; r.t_cpu = anc.t_cpu;
            return r;
        };
        auto grade_load = [&](const std::string& base_a, const std::string& base_b,
                               const std::string& ld0_b, const std::string& run01_b,
                               const char* propid, const char* pagecite, const char* mutld0, const char* mutrun01) {
            LoadResult la = run_load(base_a), lb = run_load(base_b);
            auto ok_one = [](const LoadResult& r) {
                if (!r.loaded || !r.anc_ok) return false;
                double t_e = 10.0*double(r.t_cpu);
                double tau_ls = double(r.mkD) - double(r.mkA);
                double bound = tau_ls/t_e + 2.0;
                return r.v2 == 0x4000 && (r.c & 0x10) == 0 && (double(0xFFFF - r.v3) <= bound);
            };
            auto pre_one = [](const LoadResult& r) { return r.loaded && r.v1 != 0x4000; };
            bool ok = ok_one(la) && ok_one(lb);
            bool pre_ok = pre_one(la) && pre_one(lb);
            checks.expect(pre_ok, std::string(propid) + "-PRE [p342,p348]: the counter has moved before "
                          "leg 1's read (v1<>$4000), on both CIAs");
            checks.expect(ok, std::string(propid) + std::string(" [") + pagecite + "]: v2=$4000 (a force "
                          "load loads the latch), c bit4=0 (always reads 0), and $FFFF-v3 is bounded by "
                          "tau_LS/T_E+2 (the LOAD strobe reloaded the counter), on both CIAs");
            checks.note(std::string(propid) + " CIAA v1=" + hex4(la.v1) + " v2=" + hex4(la.v2) +
                        " c=" + hex2(la.c) + " v3=" + hex4(la.v3) +
                        "; CIAB v1=" + hex4(lb.v1) + " v2=" + hex4(lb.v2) +
                        " c=" + hex2(lb.c) + " v3=" + hex4(lb.v3));

            // LD0 and CONTRAST(RUN01) are the page's own contrasting case for
            // this proposition (SPECIFICATION.md P7.4/P8.4): moved out of the
            // normal run, selected only through NMH_MUTATE (BRIEF rule 13; see
            // the header's // MUTATION record), each graded through its own
            // dedicated comparator (unchanged here) rather than folded into
            // the pass/fail count of a `make test` with no NMH_MUTATE set.
            if (mv == mutld0) {
                LoadResult ld0 = run_load(ld0_b);
                bool ld0_ok = ld0.loaded && ld0.v2 == ld0.v1;
                checks.expect(ld0_ok, std::string(propid) + "-LD0 [p348]: CR<-$00 instead of $10 (\"writing a "
                              "0 has no effect\") gives v2=v1 (CIAB)");
            }
            if (mv == mutrun01) {
                LoadResult r01 = run_load(run01_b);
                bool r01_ok = false;
                if (r01.loaded && r01.anc_ok && r01.mkRun && r01.mkC) {
                    double t_e = 10.0*double(r01.t_cpu);
                    double tau_total = double(r01.mkC) - double(r01.mkRun);
                    r01_ok = double(0xFFFF - r01.v3) >= tau_total/t_e - 2.0;
                }
                checks.expect(r01_ok, std::string(propid) + "-CONTRAST [p342]: CR<-$01 instead of $11 (no "
                              "LOAD) leaves the counter running the whole interval, so $FFFF-v3 is at least "
                              "tau_total/T_E-2, tau_total from the marker after the running leg's own start "
                              "write to the marker before the stop write (CIAB)");
            }
            return ok;
        };
        pv.p74 = grade_load(dir+"/p7_ciaa_load.bin", dir+"/p7_ciab_load.bin", dir+"/p7_ciab_load_ld0.bin",
                   dir+"/p7_ciab_load_run01.bin", "P7.4", "p342,p348", "P74-LD0", "P74-CONTRAST");
        pv.p84 = grade_load(dir+"/p8_ciaa_load.bin", dir+"/p8_ciab_load.bin", dir+"/p8_ciab_load_ld0.bin",
                   dir+"/p8_ciab_load_run01.bin", "P8.4", "p342,p349", "P84-LD0", "P84-CONTRAST");
    }

    // ---- P7.7 (CRA bit 7, unused) -------------------------------------------
    {
        struct UnResult { bool loaded=false; uint16_t c1=0,c2=0,w=0; uint64_t mkA=0,mkD=0; uint64_t t_cpu=0; bool anc_ok=false; };
        auto run_un = [&](const std::string& bin) {
            UnResult r;
            Capture c = run_capture(bin, kRunBudget, {kResults+8,kResults+10,kResults+12});
            r.loaded = c.loaded && c.done.has_value();
            if (!r.loaded) return r;
            r.c1 = pk(c,0); r.c2 = pk(c,1); r.w = pk(c,2);
            auto a = mark(c.log, 0x0C1002), d = mark(c.log, 0x0C1008);
            if (a) r.mkA = *a; if (d) r.mkD = *d;
            Anchor anc = measure_anchor(c.log);
            r.anc_ok = anc.ok; r.t_cpu = anc.t_cpu;
            return r;
        };
        std::string b81b_bin = dir + (mv == "P77-STARTOFF" ? "/p7_ciab_unused81_startoff.bin" : "/p7_ciab_unused81.bin");
        auto b81a = run_un(dir+"/p7_ciaa_unused81.bin"), b01a = run_un(dir+"/p7_ciaa_unused01.bin");
        auto b81b = run_un(b81b_bin), b01b = run_un(dir+"/p7_ciab_unused01.bin");
        auto ok_pair = [](const UnResult& r81, const UnResult& r01) {
            if (!r81.loaded || !r01.loaded) return false;
            return r81.c1 == 0 && r01.c1 == 0 && r81.c2 == 0x01 && r01.c2 == 0x01 &&
                   std::abs(int(r81.w) - int(r01.w)) <= 1;
        };
        bool ok = ok_pair(b81a,b01a) && ok_pair(b81b,b01b);
        pv.p77 = ok;
        checks.expect(ok, "P7.7 [p349]: c1=$00 (bit7 forced to 0 on a read), c2=$01 (bit0 read back, "
                      "unaffected by bit7), and |w81-w01|<=1 (bit7 does not affect operation), on both "
                      "CIAs, in a $81-run and a $01-run of the same instance");
        // P7.7-PRE's own margin is the run's own T_E (F2: 10*T_CPU), not a
        // constant clk_sys figure measured on one earlier build.
        auto bracket_ok = [](const UnResult& r81, const UnResult& r01) {
            if (!r81.loaded || !r01.loaded || !r81.anc_ok || !r01.anc_ok) return false;
            int64_t tau81 = int64_t(r81.mkD) - int64_t(r81.mkA);
            int64_t tau01 = int64_t(r01.mkD) - int64_t(r01.mkA);
            return std::llabs(tau81 - tau01) <= (int64_t)(10 * r81.t_cpu);
        };
        checks.expect(bracket_ok(b81a,b01a) && bracket_ok(b81b,b01b),
                      "P7.7-PRE [SPECIFICATION.md P7.7]: both runs have the same bracket-to-bracket time "
                      "within about 1 E period (the run's own T_E), on both CIAs");
        checks.note("P7.7 CIAA c1(81)=" + hex2(b81a.c1) + " c2(81)=" + hex2(b81a.c2) + " w81=" + hex4(b81a.w) +
                    " c2(01)=" + hex2(b01a.c2) + " w01=" + hex4(b01a.w) +
                    "; CIAB c1(81)=" + hex2(b81b.c1) + " c2(81)=" + hex2(b81b.c2) + " w81=" + hex4(b81b.w) +
                    " c2(01)=" + hex2(b01b.c2) + " w01=" + hex4(b01b.w));

        // C1CONTRAST, WCONTRAST and the STARTOFF mutation are the page's own
        // contrasting cases: moved out of the normal run, selected only
        // through NMH_MUTATE (see the header's // MUTATION records).
        if (mv == "P77-C1CONTRAST") {
            auto c1c = run_un(dir+"/p7_ciab_unused_c1contrast.bin");
            checks.expect(c1c.loaded && c1c.c1 != 0, "P7.7-C1CONTRAST [p349]: CR<-$02 (a used bit) in place "
                          "of CR<-$80 makes c1's own \"forced to 0\" check fail (CIAB)");
        }
        if (mv == "P77-WCONTRAST") {
            auto wc = run_un(dir+"/p7_ciab_unused_wcontrast.bin");
            bool wc_ok = wc.loaded && std::abs(int(wc.w) - int(b01b.w)) > 1;
            checks.expect(wc_ok, "P7.7-WCONTRAST [p348,p349]: CR<-$21 (START and INMODE, used bits) in place "
                          "of CR<-$81 makes w21 differ from w01 by more than 1, so the \"unaffected by a "
                          "write\" check fails (CIAB)");
        }
    }

    // ---- P7.5 / P8.65 mode 01 (bit 5, INMODE: CNT vs phi2) + KP -----------
    struct InmodeResult {
        bool loaded=false; uint16_t i=0, v=0; double w_e=0; bool have_tcpu=false;
    };
    auto grade_inmode = [&](const std::string& bin, bool do_key) {
        InmodeResult r;
        KeyCapture kc = run_capture_key(bin, 0x0C1002, do_key, 0x35, 100, kRunBudget, {kResults+8,kResults+10});
        r.loaded = kc.loaded && kc.done.has_value();
        if (!r.loaded) return r;
        r.i = pk(view_of(kc),0); r.v = pk(view_of(kc),1);
        // W (SPECIFICATION.md P7.5): the marker after the start write (ready)
        // to the marker after the stop write -- not the wait loop's own end
        // marker, which the counter keeps running past.
        auto ready = mark(kc.log, 0x0C1002), stopb = mark(kc.log, 0x0C1008);
        r.have_tcpu = kc.t_cpu > 0;
        if (ready && stopb && r.have_tcpu) r.w_e = double(*stopb - *ready) / (10.0*double(kc.t_cpu));
        return r;
    };
    InmodeResult p75a, p75b;
    InmodeResult p865_01a, p865_01b;
    {
        p75a = grade_inmode(dir+"/p7_ciaa_inmode.bin", true);
        p75b = grade_inmode(dir+"/p7_ciab_inmode.bin", false);
        bool ciaa_ok = p75a.loaded && (0x1000 - p75a.v) == 8;
        bool ciab_ok = p75b.loaded && p75b.v == 0x1000;
        pv.p75 = ciaa_ok && ciab_ok;
        checks.expect(pv.p75, "P7.5 [p348,p351,p357]: with CRA INMODE=CNT, CIAA's timer A "
                      "decrements by 8 (one per key bit) over the window, and CIAB's (CNT undriven) "
                      "does not move");
        bool window_ok = p75a.loaded && (p75a.i & kIcrSp);
        checks.expect(window_ok, "P7.5-WINDOW [p345,p357]: the key transmission finished before the stop "
                      "write (i has SP=1) on CIAA");
        checks.note("P7.5 CIAA i=" + hex2(p75a.i) + " v=" + hex4(p75a.v) + " W=" + std::to_string(p75a.w_e) +
                    "E; CIAB v=" + hex4(p75b.v));

        // P7.5-CONTRAST is the page's own contrasting case: moved out of the
        // normal run, selected only through NMH_MUTATE (see the header).
        if (mv == "P75-CONTRAST") {
            InmodeResult p75a_phi2 = grade_inmode(dir+"/p7_ciaa_inmode_phi2.bin", true);
            InmodeResult p75b_phi2 = grade_inmode(dir+"/p7_ciab_inmode_phi2.bin", false);
            bool contrast_ok = p75a_phi2.loaded && p75a_phi2.have_tcpu &&
                                std::fabs(double(0x1000 - p75a_phi2.v) - std::round(p75a_phi2.w_e)) <= 2.0 &&
                                p75b_phi2.loaded && p75b_phi2.have_tcpu &&
                                std::fabs(double(0x1000 - p75b_phi2.v) - std::round(p75b_phi2.w_e)) <= 2.0;
            checks.expect(contrast_ok, "P7.5-CONTRAST [p348,p353]: CR<-$01 (phi2) instead of $21 gives "
                          "$1000-v = round(W/T_E)+-2, on both CIAs");
        }
        // Not-started green control (CIAB leg): CRA<-$20 (INMODE=CNT,
        // START=0) in place of $21 -- a proven-live input through the same
        // "v=$1000" comparator P7.5's already-red CIAB leg uses.
        if (mv == "P75-GREENCTL") {
            InmodeResult ns = grade_inmode(dir+"/p7_ciab_inmode_nostart.bin", false);
            checks.expect(ns.loaded && ns.v == 0x1000, "P7.5-CIAB-GREENCTL [SPECIFICATION.md P7.5]: "
                          "CRA<-$20 (INMODE=CNT, not started) gives v=$1000 through the same comparator "
                          "P7.5's CIAB leg uses (CIAB)");
        }

        p865_01a = grade_inmode(dir+"/p8_ciaa_inmode01.bin", true);
        p865_01b = grade_inmode(dir+"/p8_ciab_inmode01.bin", false);
        bool m01_ciaa_ok = p865_01a.loaded && (0x1000 - p865_01a.v) == 8;
        bool m01_ciab_ok = p865_01b.loaded && p865_01b.v == 0x1000;
        pv.p865_01 = m01_ciaa_ok && m01_ciab_ok;
        checks.expect(pv.p865_01, "P8.65-MODE01 [p349,p351,p357]: with CRB INMODE=01 "
                      "(CNT), CIAA's timer B decrements by 8 over the window, and CIAB's does not move");
        bool m01_window_ok = p865_01a.loaded && (p865_01a.i & kIcrSp);
        checks.expect(m01_window_ok, "P8.65-MODE01-WINDOW [p345,p357]: the key transmission finished "
                      "before the stop write (i has SP=1) on CIAA");
        checks.note("P8.65-MODE01 CIAA i=" + hex2(p865_01a.i) + " v=" + hex4(p865_01a.v) +
                    "; CIAB v=" + hex4(p865_01b.v));

        if (mv == "P865M01-CONTRAST") {
            InmodeResult p865_01a_phi2 = grade_inmode(dir+"/p8_ciaa_inmode01_phi2.bin", true);
            InmodeResult p865_01b_phi2 = grade_inmode(dir+"/p8_ciab_inmode01_phi2.bin", false);
            bool m01_contrast_ok = p865_01a_phi2.loaded && p865_01a_phi2.have_tcpu &&
                                    std::fabs(double(0x1000 - p865_01a_phi2.v) - std::round(p865_01a_phi2.w_e)) <= 2.0 &&
                                    p865_01b_phi2.loaded && p865_01b_phi2.have_tcpu &&
                                    std::fabs(double(0x1000 - p865_01b_phi2.v) - std::round(p865_01b_phi2.w_e)) <= 2.0;
            checks.expect(m01_contrast_ok, "P8.65-MODE01-CONTRAST [p349,p353]: CRB<-$01 (mode 00, phi2) "
                          "instead of $21 gives $1000-v = round(W/T_E)+-2, on both CIAs");
        }
        if (mv == "P865M01-GREENCTL") {
            InmodeResult ns = grade_inmode(dir+"/p8_ciab_inmode01_nostart.bin", false);
            checks.expect(ns.loaded && ns.v == 0x1000, "P8.65-MODE01-CIAB-GREENCTL [SPECIFICATION.md "
                          "P8.65]: CRB<-$20 (INMODE=CNT, not started) gives v=$1000 through the same "
                          "comparator P8.65 mode 01's CIAB leg uses (CIAB)");
        }
    }

    // ---- KP (keyboard idle): recorded, not a gating precondition (KP's own
    // probe counts CNT, the clause under test -- see SPECIFICATION.md 1.5
    // KP, "Not usable as a gating precondition").
    {
        InmodeResult kpa_noKey, kpa_key, kpb_noKey, kpb_key;
        auto grade_kp = [&](const std::string& bin, bool send_key) {
            InmodeResult r;
            KeyCapture kc = run_capture_key(bin, 0x0C1002, send_key, 0x35, 100, kRunBudget, {kResults+8});
            r.loaded = kc.loaded && kc.done.has_value();
            if (r.loaded) r.v = pk(view_of(kc),0);
            return r;
        };
        kpa_noKey = grade_kp(dir+"/kp_a.bin", false);
        kpa_key   = grade_kp(dir+"/kp_a.bin", true);
        kpb_noKey = grade_kp(dir+"/kp_b.bin", false);
        kpb_key   = grade_kp(dir+"/kp_b.bin", true);
        checks.record("KP-A [SPECIFICATION.md 1.5 KP]: not a gating precondition (it depends on the "
                      "clause under test, INMODE=CNT). Measured: no-key v=" + hex4(kpa_noKey.v) +
                      ", with-key v=" + hex4(kpa_key.v) + " -- both CNT, so this cannot be distinguished "
                      "on this build; P7.5's own CIAB leg carries the red without it");
        checks.record("KP-B [SPECIFICATION.md 1.5 KP]: not a gating precondition, the same reason. "
                      "Measured: no-key v=" + hex4(kpb_noKey.v) + ", with-key v=" + hex4(kpb_key.v) +
                      " -- P8.65 mode 01's own CIAB leg carries the red without it");
    }

    // ---- P7.6 (CRA bit 6, SPMODE) ------------------------------------------
    {
        // CIAA leg reuses P7.5's own run (i, made before its stop write) --
        // this leg's own positive result and its own window precondition.
        bool ciaa_ok = p75a.loaded && (p75a.i & kIcrSp);
        pv.p76_ciaa = ciaa_ok;
        checks.expect(ciaa_ok, "P7.6-CIAA [p345,p351,p357]: SP=1 in i, the ICR read P7.5's CIAA run "
                      "makes before its stop write (input mode, CRA bit6=0)");
        if (mv == "P76CIAA-CONTRAST") {
            InmodeResult ciaa_noKey = grade_inmode(dir+"/p7_ciaa_inmode.bin", false);
            bool ciaa_contrast_ok = ciaa_noKey.loaded && !(ciaa_noKey.i & kIcrSp);
            checks.expect(ciaa_contrast_ok, "P7.6-CIAA-CONTRAST [SPECIFICATION.md P7.6]: the same run "
                          "without R-KEY gives SP=0");
        }

        Capture cb = run_capture(dir+"/p76_ciab.bin", kRunBudget, {});
        SampleSeries icrb = read_series(cb.log, kResults+8, 300);
        int taCount = 0; bool anySp = false;
        for (auto& v : icrb.value) if (v) { if (*v & kIcrTa) ++taCount; if (*v & kIcrSp) anySp = true; }
        bool ciab_ok = cb.loaded && cb.done.has_value() && anySp;
        pv.p76_ciab = ciab_ok;
        checks.expect(ciab_ok, "P7.6-CIAB [p345,p348]: at least one ICR sample has SP=1 (CRA<-$41, "
                      "SPMODE output, SDR<-$A5)");
        bool ciab_pre_ok = taCount >= 20;
        checks.expect(ciab_pre_ok, "P7.6-CIAB-PRE [SPECIFICATION.md P7.6]: timer A underflows during the "
                      "window (at least 20 samples have TA=1)");
        checks.note("P7.6-CIAB TA samples=" + std::to_string(taCount) + " SP seen=" + std::string(anySp?"yes":"no"));

        if (mv == "P76CIAB-CONTRAST") {
            Capture cbc = run_capture(dir+"/p76_ciab_contrast.bin", kRunBudget, {});
            SampleSeries icrc = read_series(cbc.log, kResults+8, 300);
            bool anySpC = false; for (auto& v : icrc.value) if (v && (*v & kIcrSp)) anySpC = true;
            checks.expect(cbc.loaded && cbc.done.has_value() && !anySpC,
                          "P7.6-CIAB-CONTRAST [p345,p348]: CR<-$01 (bit6=0, input) gives SP=0 in every "
                          "sample (CIAB's CNT/POUT is not driven)");
        }
        if (mv == "P76CIAB-PRECONTRAST") {
            Capture cbp = run_capture(dir+"/p76_ciab_pre.bin", kRunBudget, {});
            SampleSeries icrp = read_series(cbp.log, kResults+8, 300);
            int taP = 0; bool spP = false;
            for (auto& v : icrp.value) if (v) { if (*v & kIcrTa) ++taP; if (*v & kIcrSp) spP = true; }
            checks.expect(cbp.loaded && cbp.done.has_value() && taP == 0 && !spP,
                          "P7.6-CIAB-PRECONTRAST [SPECIFICATION.md P7.6]: CR<-$40 (not started) gives no "
                          "TA and no SP");
        }
        // Green control: the same SPMODE=output program, run on CIA-A
        // instead (question 3/4): a proven-live input through the same
        // "at least one sample has SP=1" comparator P7.6-CIAB's red uses.
        if (mv == "P76CIAB-GREENCTL") {
            Capture ca = run_capture(dir+"/p76_ciaa.bin", kRunBudget, {});
            SampleSeries icra = read_series(ca.log, kResults+8, 300);
            bool anySpA = false; for (auto& v : icra.value) if (v && (*v & kIcrSp)) anySpA = true;
            checks.expect(ca.loaded && ca.done.has_value() && anySpA,
                          "P7.6-CIAB-GREENCTL [SPECIFICATION.md P7.6]: the same program run on CIA-A "
                          "gives at least one SP=1 sample, through the same comparator P7.6-CIAB's red "
                          "uses");
        }
    }

    // ---- P8.65 mode 10 (CRB counts timer A underflow pulses) --------------
    {
        auto grade_mode10 = [&](const std::string& bin, const char* tag) {
            Capture c = run_capture(bin, kRunBudget, {kResults+608, kResults+610});
            SampleSeries icr = read_series(c.log, kResults+8, 300);
            int nA = 0;
            for (auto& v : icr.value) if (v && (*v & kIcrTa)) ++nA;
            bool finalTa = pk_ok(c,0) && (pk(c,0) & kIcrTa);
            if (finalTa) ++nA;
            uint16_t v = pk(c,1);
            bool ok = c.loaded && c.done.has_value() && (0xFFFF - v) == nA;
            // Pre8.65-M10: iteration spacing <= P(30)/2 (F3), from the array's own E-indices.
            Anchor anc = measure_anchor(c.log);
            bool pre_ok = false;
            PeriodResult pr;
            if (anc.ok) {
                int64_t max_gap = 0; std::optional<uint64_t> prev;
                for (size_t i = 0; i < icr.size(); ++i) {
                    if (!icr.cycle[i]) continue;
                    if (prev) max_gap = std::max(max_gap, (int64_t)*icr.cycle[i] - (int64_t)*prev);
                    prev = icr.cycle[i];
                }
                pr = measure_period(icr, kIcrTa, anc.t_cpu);
                if (pr.ok) pre_ok = double(max_gap) <= double(pr.P) * 10.0 * double(anc.t_cpu) / 2.0 + 1.0;
                checks.note(std::string(tag) + ": measured P(30)=" + std::to_string(pr.P) +
                            (pr.ok ? " (uniquely determined)" : " (not uniquely determined)"));
            }
            checks.note(std::string(tag) + ": nA=" + std::to_string(nA) + " v=" + hex4(v) +
                        " flagged=" + std::to_string(pr.flag_count));
            return std::make_pair(ok, pre_ok);
        };
        auto [ciaa_ok, ciaa_pre] = grade_mode10(dir+"/p865_ciaa_mode10.bin", "P8.65-M10 CIAA");
        // M10-PRE (Pre8.65-M10's own contrast, SPECIFICATION.md: "with
        // R-LATCH_A(2), flags appear in consecutive reads") selects a
        // dedicated short-latch binary; the baseline previously left this
        // selector pointing at the same file both ways.
        std::string ciab_bin = dir + (mv == "M10-PRE" ? "/p865_ciab_mode10_shortlatch.bin" : "/p865_ciab_mode10.bin");
        auto [ciab_ok, ciab_pre] = grade_mode10(ciab_bin, "P8.65-M10 CIAB");
        pv.p865_10 = ciaa_ok && ciab_ok;
        checks.expect(pv.p865_10, "P8.65-MODE10 [p349,p350]: $FFFF-v equals N_A, the number of ICR "
                      "reads after CRA's start write (including the final one) that have TA=1, on both CIAs");
        checks.expect(ciaa_pre && ciab_pre, "P8.65-MODE10-PRE [SPECIFICATION.md F3]: the iteration spacing "
                      "is at most P(30)/2, so each TA flag is exactly one underflow, on both CIAs "
                      "(CIAB fails it under NMH_MUTATE=M10-PRE)");

        // P8.65-MODE10-CONTRAST is the page's own contrasting case: moved
        // out of the normal run, selected only through NMH_MUTATE.
        if (mv == "M10-CONTRAST") {
            auto grade_mode10_contrast = [&](const std::string& bin) {
                Capture c = run_capture(bin, kRunBudget, {kResults+608, kResults+610});
                SampleSeries icr = read_series(c.log, kResults+8, 300);
                uint64_t first = 0, last = 0; bool have = false;
                for (auto& cyc : icr.cycle) if (cyc) { if (!have) { first = *cyc; have = true; } last = *cyc; }
                Anchor anc = measure_anchor(c.log);
                uint16_t v = pk(c,1);
                if (!c.loaded || !c.done.has_value() || !anc.ok || !have) return false;
                double window_e = double(last - first) / (10.0*double(anc.t_cpu));
                return double(0xFFFF - v) >= window_e - 2.0;
            };
            bool contrast_ok = grade_mode10_contrast(dir+"/p865_ciab_mode10_contrast.bin");
            checks.expect(contrast_ok, "P8.65-MODE10-CONTRAST [p349]: CRB<-$01 (mode 00) gives $FFFF-v at "
                          "least window/T_E-2, far more than N_A (CIAB)");
        }
    }

    // ---- P8.65 mode 11 (CIAA only) -----------------------------------------
    {
        struct M11 { bool loaded=false; uint16_t i=0,v=0; uint64_t window=0; };
        auto run_m11 = [&](uint8_t crbval, bool send_key) {
            M11 r;
            std::string bin = dir + "/p865_ciaa_mode11.bin";
            nmh::NanomigHarness h(nmh::Config{});
            if (h.load_program(bin) == 0) { register_run(bin, false, false, Ep1Words{}, {}); return r; }
            h.poke_word(0x0C2000/2, crbval);
            h.boot(kBootBudget);
            uint32_t w_ready = word_addr(0x0C1004);
            std::optional<uint64_t> ready_cycle;
            bool have_tcpu = false, key_pending = false, key_sent = false, found_done = false;
            uint64_t t_cpu = 0;
            size_t seen = h.ram_log().size();
            std::optional<uint64_t> done;
            for (uint64_t iCyc = 0; iCyc < kRunBudget && !found_done; ++iCyc) {
                if (key_pending) {
                    h.ports()->kbd_mouse_type = 2; h.ports()->kbd_mouse_data = 0x35;
                    h.ports()->kbd_mouse_level = !h.ports()->kbd_mouse_level;
                }
                h.run_cycles(1);
                if (key_pending) { h.ports()->kbd_mouse_type = 0; key_pending = false; key_sent = true; }
                const auto& lg = h.ram_log();
                for (; seen < lg.size(); ++seen) {
                    const auto& a = lg[seen];
                    if (a.is_write && a.word_addr == word_addr(kDoneAddr) && a.data == kDoneValue) { done = a.cycle; found_done = true; }
                    if (a.is_write && a.word_addr == w_ready && !ready_cycle) ready_cycle = a.cycle;
                }
                if (!have_tcpu) { Anchor a = measure_anchor(h.ram_log()); if (a.ok) { t_cpu = a.t_cpu; have_tcpu = true; } }
                if (send_key && !key_sent && !key_pending && ready_cycle && have_tcpu &&
                    h.cycles() >= *ready_cycle + 100ull*10*t_cpu) key_pending = true;
            }
            r.loaded = done.has_value();
            register_run(bin, true, done.has_value(), ep1_of(h), h.ram_log());
            if (r.loaded) {
                r.i = h.peek_logical(0x0C0008).value_or(0xFFFF); r.v = h.peek_logical(0x0C000A).value_or(0xFFFF);
                auto lg = h.ram_log();
                auto mready = mark(lg, 0x0C1004), msb = mark(lg, 0x0C100A);
                if (mready && msb) r.window = *msb - *mready;
            }
            return r;
        };
        M11 nx = run_m11(0x41, false), ny = run_m11(0x61, false);
        M11 kx = run_m11(0x41, true),  ky = run_m11(0x61, true);
        bool pairN_ok = nx.loaded && ny.loaded && nx.v == ny.v;
        bool pairK_ok = kx.loaded && ky.loaded && (0xFFFF - ky.v) < (0xFFFF - kx.v);
        pv.p865_11 = pairN_ok && pairK_ok;
        checks.expect(pairN_ok, "P8.65-MODE11-PAIRN [p349,p357]: with no key sent, D_Y=D_X (mode 11 "
                      "counts every underflow mode 10 counts, CNT idle high)");
        checks.expect(pairK_ok, "P8.65-MODE11-PAIRK [p349,p357]: with a key sent, D_Y<D_X (mode 11 does "
                      "not count underflows while CNT is pulsed low)");
        checks.record("P8.65-MODE11-PAIRK [SPECIFICATION.md P8.65 mode 11]: a proven-live green input "
                      "through the same comparator is named but not built here: run Y with CRB<-$40 "
                      "(mode 10, START=0) gives D_Y=0<D_X, the same \"less than\" the red uses -- not "
                      "exercised, by a scope decision: the package's other green controls cover P3, "
                      "the CIA-B legs of P7.5 and P8.65 mode 01, P7.6-CIAB and P7.7");
        bool window_ok = kx.loaded && (kx.i & kIcrSp) && ky.loaded && (ky.i & kIcrSp);
        checks.expect(window_ok, "P8.65-MODE11-WINDOW [p345,p357]: pair K's own i has SP=1 in both runs "
                      "(the key transmission finished before CRA's stop write)");
        bool windows_eq = nx.loaded && ny.loaded && kx.loaded && ky.loaded && nx.window > 0 &&
                           nx.window == ny.window && kx.window == ky.window && nx.window == kx.window;
        checks.expect(windows_eq, "P8.65-MODE11-WINPRE [SPECIFICATION.md P8.65 mode 11]: in each pair, "
                      "the two runs' windows (marker after CRA's start write to marker after its stop "
                      "write) are equally long in system clock cycles, so modes 10 and 11 are offered "
                      "the same timer A underflows");
        checks.note("P8.65-MODE11 D_X(N)=" + std::to_string(0xFFFF-nx.v) + " D_Y(N)=" + std::to_string(0xFFFF-ny.v) +
                    " D_X(K)=" + std::to_string(0xFFFF-kx.v) + " D_Y(K)=" + std::to_string(0xFFFF-ky.v) +
                    " window(N)=" + std::to_string(nx.window) + " window(K)=" + std::to_string(kx.window));

        checks.record("P8.65-MODE11-CONTRAST [SPECIFICATION.md P8.65 mode 11]: each pair is the other's "
                      "own contrast (sending the key in pair N, or leaving it out of pair K), for D_X=D_Y "
                      "(PAIRN); pair K's own data (D_X=D_Y=559/666) also satisfies PAIRN, so that half is "
                      "not distinguishable from PAIRN's own baseline on this build. A run with CRB<-$01 "
                      "(mode 00, phi2) gives a usable contrast instead (NMH_MUTATE=M11-MODE00CONTRAST)");
        if (mv == "M11-MODE00CONTRAST") {
            M11 m00 = run_m11(0x01, true);
            bool ok = m00.loaded && kx.loaded && (0xFFFF - m00.v) != (0xFFFF - kx.v);
            checks.expect(ok, "P8.65-MODE11-MODE00CONTRAST [p349]: CRB<-$01 (mode 00, phi2) in place of "
                          "$41 (mode 10) gives a D differing from pair K's D_X, a usable contrast to "
                          "PAIRN where sending the key in pair K's own mode-10 leg does not change the "
                          "count");
        }
    }

    // ---- P8.7 (CRB bit 7, ALARM) -------------------------------------------
    {
        auto count_lines_between = [](const Anchor& a, uint64_t c1, uint64_t c2) {
            int n = 0; for (auto f : a.firsts) if (f > c1 && f <= c2) ++n; return n;
        };

        // CIAB
        std::string ciab_bin = dir + (mv == "P87-PRE" ? "/p87_ciab_nolsb.bin" : "/p87_ciab.bin");
        Capture cb = run_capture(ciab_bin, kRunBudget,
            {kResults+8,kResults+10,kResults+12,kResults+14,kResults+16,kResults+18});
        Anchor ancb = measure_anchor(cb.log);
        uint32_t t0 = (uint32_t(pk(cb,0))<<16)|pk(cb,1);
        uint32_t t1x = (uint32_t(pk(cb,2))<<16)|pk(cb,3);
        uint32_t t1y = (uint32_t(pk(cb,4))<<16)|pk(cb,5);
        auto mwr = mark(cb.log, 0x0C1010), mt0a = mark(cb.log, 0x0C1002), mt1x = mark(cb.log, 0x0C1006), mt1y = mark(cb.log, 0x0C1008);
        bool pre_ok = false;
        if (ancb.ok && mwr && mt0a) {
            int lines = count_lines_between(ancb, *mwr, *mt0a);
            pre_ok = std::abs(int(t0) - lines) <= 1;
            checks.note("P8.7-PRE CIAB: T0=" + std::to_string(t0) + " lines-measured=" + std::to_string(lines));
        }
        checks.expect(pre_ok, "P8.7-PRE [p340]: TOD counts lines -- T0 (from TOD<-0) matches the number "
                      "of lines elapsed (F4), within 1 (CIAB; the LSB never written keeps TOD stopped "
                      "under NMH_MUTATE=P87-PRE)");

        SampleSeries xtod = read_series(cb.log, kResults+20, 450);
        SampleSeries xicr = read_series(cb.log, kResults+20+450*2, 450);
        bool alrmSeenX = false; std::optional<size_t> firstMatchIdx;
        uint8_t targetLo = uint8_t((t0+12) & 0xFF);
        for (size_t i = 0; i < xtod.size(); ++i) {
            if (!firstMatchIdx && xtod.value[i] && uint8_t(*xtod.value[i] & 0xFF) == targetLo) firstMatchIdx = i;
            if (xicr.value[i] && (*xicr.value[i] & kIcrAlrm)) { if (!alrmSeenX) alrmSeenX = true; }
        }
        // The sampler reads TODLO before ICR every iteration, so the alarm
        // may already show in the (m-1)th ICR read (SPECIFICATION.md P8.7):
        // the first ALRM sample is ICR_(m-1) or ICR_m, none
        // earlier.
        std::optional<size_t> firstAlrmIdx;
        for (size_t i = 0; i < xicr.size(); ++i) if (xicr.value[i] && (*xicr.value[i] & kIcrAlrm)) { firstAlrmIdx = i; break; }
        bool alrmOrder_ok = firstMatchIdx && firstAlrmIdx &&
            (*firstAlrmIdx == *firstMatchIdx || *firstAlrmIdx + 1 == *firstMatchIdx);
        int linesX = (ancb.ok && mt0a && mt1x) ? count_lines_between(ancb, *mt0a, *mt1x) : -1;
        bool runX_ok = cb.loaded && cb.done.has_value() && linesX >= 0 &&
                       std::abs(int(t1x - t0) - linesX) <= 1 && alrmSeenX && alrmOrder_ok;
        checks.expect(runX_ok, "P8.7-RUNX [p344,p349]: T1-T0 equals the lines elapsed (+-1); the first "
                      "ALRM sample is ICR_(m-1) or ICR_m, where m is the first TODLO sample equal to "
                      "A&$FF, and none earlier has it (CIAB)");
        checks.note("P8.7-RUNX CIAB: firstMatch=" + (firstMatchIdx?std::to_string(*firstMatchIdx):"?") +
                    " firstALRM=" + (firstAlrmIdx?std::to_string(*firstAlrmIdx):"?"));

        SampleSeries ytod = read_series(cb.log, kResults+20+450*4, 450);
        SampleSeries yicr = read_series(cb.log, kResults+20+450*6, 450);
        bool alrmSeenY = false;
        for (auto& v : yicr.value) if (v && (*v & kIcrAlrm)) alrmSeenY = true;
        // F4: count the line marks between the marker after the TODLO write
        // (MK_87_T1Y) and the store of the T1Y read that follows it
        // immediately (no delay loop between them), rather than taking
        // T1-A itself as "the number of lines" (that was the same quantity
        // printed twice, not an independent count).
        auto t1y_store = mark(cb.log, kResults+16);
        int linesY = (ancb.ok && mt1y && t1y_store) ? count_lines_between(ancb, *mt1y, *t1y_store) : -1;
        bool runY_ok = cb.loaded && cb.done.has_value() && linesY >= 0 &&
                       std::abs(int(t1y - 0x001000) - linesY) <= 1 && !alrmSeenY;
        pv.p87_alarm = runX_ok && runY_ok;
        checks.expect(runY_ok, "P8.7-RUNY [p344,p349,p354]: T1-A equals the number of line marks (F4) "
                      "between the marker after the TODLO write and the T1 read's own store, +-1; no "
                      "ALRM flag appears (CIAB)");
        checks.record("P8.7-RUNY [SPECIFICATION.md P8.7]: run Y follows run X in the same program rather "
                      "than being its own run from reset (SPECIFICATION.md 1.6, \"each leg of a "
                      "proposition is its own run from reset unless its stimulus says otherwise\"; P8.7's "
                      "own stimulus does not say otherwise) -- its own alarm holds T0+12 (set by run X, "
                      "since CRB stays $00 through run Y and nothing clears it) rather than the reset "
                      "value 0 the page's own prediction argues from; measured, the prediction (no ALRM "
                      "in run Y, T1-A within the lines elapsed) holds either way");
        (void)ytod;
        checks.note("P8.7 CIAB T0=" + std::to_string(t0) + " T1X=" + std::to_string(t1x) +
                    " T1Y=" + std::to_string(t1y) + " linesX=" + std::to_string(linesX) +
                    " linesY=" + std::to_string(linesY));

        // CIAA
        Capture ca = run_capture(dir+"/p87_ciaa.bin", kRunBudget, {kResults+8,kResults+10,kResults+12,kResults+14,kResults+16,kResults+18});
        uint32_t t0a = (uint32_t(pk(ca,0))<<16)|pk(ca,1);
        uint32_t t1xa = (uint32_t(pk(ca,2))<<16)|pk(ca,3);
        uint32_t t1ya = (uint32_t(pk(ca,4))<<16)|pk(ca,5);
        bool ciaa_ok = ca.loaded && ca.done.has_value() &&
                       (int64_t(t1xa) - int64_t(t0a)) >= 0 && (int64_t(t1xa) - int64_t(t0a)) <= 2 &&
                       (int64_t(t1ya) - int64_t(0x001000)) >= 0 && (int64_t(t1ya) - int64_t(0x001000)) <= 2;
        checks.expect(ciaa_ok, "P8.7-CIAA [p339,p344,p349]: run X, T1-T0 in [0,2]; run Y, T1-A in [0,2]");
        checks.note("P8.7 CIAA T0=" + std::to_string(t0a) + " T1X=" + std::to_string(t1xa) + " T1Y=" + std::to_string(t1ya));
    }

    // ---- P5 (phi2 is the E clock, one tenth of the CPU clock) -------------
    struct P5aResult { bool loaded=false; uint16_t v=0,v2=0,i=0; uint64_t mkB=0,mkD=0; uint64_t t_cpu=0; bool anc_ok=false; };
    auto run_p5a = [&](const std::string& bin) {
        P5aResult r;
        // D2's own delay (SPECIFICATION.md P5, tau2~=550000*T_CPU) needs a
        // budget well above the package's usual kRunBudget.
        Capture c = run_capture(bin, 20'000'000, {kResults+8,kResults+10,kResults+12});
        r.loaded = c.loaded && c.done.has_value();
        if (!r.loaded) return r;
        r.v = pk(c,0); r.v2 = pk(c,1); r.i = pk(c,2);
        auto b = mark(c.log, 0x0C1004), d = mark(c.log, 0x0C1008);
        if (b) r.mkB = *b; if (d) r.mkD = *d;
        Anchor a = measure_anchor(c.log);
        r.anc_ok = a.ok; r.t_cpu = a.t_cpu;
        return r;
    };
    struct P5Summary { double delta_ok=false; bool pre51_ok=false, pre52_ok=false; double delta1=0, delta2=0, tau1=0, tau2=0; };
    P5Summary p5_ta_summary, p5_tb_summary;
    {
        auto grade_p5a_instance = [&](const std::string& d1bin, const std::string& d2bin, uint16_t icrmask) {
            P5Summary s;
            P5aResult r1 = run_p5a(d1bin), r2 = run_p5a(d2bin);
            if (!r1.loaded || !r2.loaded || !r1.anc_ok || !r2.anc_ok) return s;
            double t_e = 10.0 * double(r2.t_cpu);
            s.tau1 = double(r1.mkD) - double(r1.mkB);
            s.tau2 = double(r2.mkD) - double(r2.mkB);
            s.delta1 = double(0xFFFF - r1.v);
            s.delta2 = double(0xFFFF - r2.v);
            double predicted = std::round((s.tau2 - s.tau1) / t_e);
            s.delta_ok = std::fabs((s.delta2 - s.delta1) - predicted) <= 1.0;
            s.pre51_ok = !(r1.i & icrmask) && !(r2.i & icrmask);
            s.pre52_ok = (r1.v2 == r1.v) && (r2.v2 == r2.v);
            return s;
        };
        P5Summary a_ciaa_ta = grade_p5a_instance(dir+"/p5a_ciaa_ta_d1.bin", dir+"/p5a_ciaa_ta_d2.bin", kIcrTa);
        P5Summary a_ciaa_tb = grade_p5a_instance(dir+"/p5a_ciaa_tb_d1.bin", dir+"/p5a_ciaa_tb_d2.bin", kIcrTb);
        P5Summary a_ciab_ta = grade_p5a_instance(dir+"/p5a_ciab_ta_d1.bin", dir+"/p5a_ciab_ta_d2.bin", kIcrTa);
        P5Summary a_ciab_tb = grade_p5a_instance(dir+"/p5a_ciab_tb_d1.bin", dir+"/p5a_ciab_tb_d2.bin", kIcrTb);
        p5_ta_summary = a_ciaa_ta; // representative for the TA-instance id row (P10)
        p5_tb_summary = a_ciab_tb;

        pv.p5_ta = a_ciaa_ta.delta_ok && a_ciab_ta.delta_ok;
        pv.p5_tb = a_ciaa_tb.delta_ok && a_ciab_tb.delta_ok;
        bool delta_ok_all = a_ciaa_ta.delta_ok && a_ciaa_tb.delta_ok && a_ciab_ta.delta_ok && a_ciab_tb.delta_ok;
        checks.expect(delta_ok_all, "P5-LEGA [p353: opus:F-signals.c2, sol:F-signals.c70]: Delta2-Delta1 "
                      "= round((tau2-tau1)/(10*T_CPU)) +-1, for all four timers");
        bool pre51_ok = a_ciaa_ta.pre51_ok && a_ciaa_tb.pre51_ok && a_ciab_ta.pre51_ok && a_ciab_tb.pre51_ok;
        checks.expect(pre51_ok, "P5-PRE51 [SPECIFICATION.md P5]: no underflow happens in the window (i "
                      "flag clear), for all four timers, in both D1 and D2 runs");
        bool pre52_ok = a_ciaa_ta.pre52_ok && a_ciaa_tb.pre52_ok && a_ciab_ta.pre52_ok && a_ciab_tb.pre52_ok;
        checks.expect(pre52_ok, "P5-PRE52 [SPECIFICATION.md P5]: the counter reads the same value twice "
                      "while stopped, for all four timers, in both D1 and D2 runs");
        checks.note("P5-LEGA CIAA/TA tau1=" + std::to_string(a_ciaa_ta.tau1) + " tau2=" + std::to_string(a_ciaa_ta.tau2) +
                    " delta1=" + std::to_string(a_ciaa_ta.delta1) + " delta2=" + std::to_string(a_ciaa_ta.delta2));
        checks.note("P5-LEGA CIAA/TB tau1=" + std::to_string(a_ciaa_tb.tau1) + " tau2=" + std::to_string(a_ciaa_tb.tau2) +
                    " delta1=" + std::to_string(a_ciaa_tb.delta1) + " delta2=" + std::to_string(a_ciaa_tb.delta2));
        checks.note("P5-LEGA CIAB/TA tau1=" + std::to_string(a_ciab_ta.tau1) + " tau2=" + std::to_string(a_ciab_ta.tau2) +
                    " delta1=" + std::to_string(a_ciab_ta.delta1) + " delta2=" + std::to_string(a_ciab_ta.delta2));
        checks.note("P5-LEGA CIAB/TB tau1=" + std::to_string(a_ciab_tb.tau1) + " tau2=" + std::to_string(a_ciab_tb.tau2) +
                    " delta1=" + std::to_string(a_ciab_tb.delta1) + " delta2=" + std::to_string(a_ciab_tb.delta2));

        // Pre5.1's own contrast, Pre5.2's own contrast, and the CNT
        // contrasting case (CIAB/timer B only) are the page's own
        // contrasting cases: moved out of the normal run, selected only
        // through NMH_MUTATE.
        if (mv == "P5-PRE51CONTRAST") {
            P5aResult pre51c = run_p5a(dir+"/p5a_ciab_tb_d2_pre51.bin");
            checks.expect(pre51c.loaded && (pre51c.i & kIcrTb), "P5-PRE51-CONTRAST [SPECIFICATION.md P5]: "
                          "R-LATCH($1000) with D2 sets the flag (CIAB/timer B)");
        }
        if (mv == "P5-PRE52CONTRAST") {
            P5aResult pre52c = run_p5a(dir+"/p5a_ciab_tb_d1_pre52.bin");
            checks.expect(pre52c.loaded && pre52c.v2 != pre52c.v, "P5-PRE52-CONTRAST [SPECIFICATION.md "
                          "P5]: leaving out the stop write makes the two reads differ (CIAB/timer B)");
        }
        if (mv == "P5-CONTRAST") {
            P5aResult cnt1 = run_p5a(dir+"/p5a_ciab_tb_d1_cnt.bin"), cnt2 = run_p5a(dir+"/p5a_ciab_tb_d2_cnt.bin");
            bool cnt_ok = cnt1.loaded && cnt2.loaded && (0xFFFF-cnt1.v) == 0 && (0xFFFF-cnt2.v) == 0;
            checks.expect(cnt_ok, "P5-CONTRAST [p348,p357]: counting CNT instead of phi2, with no key "
                          "sent, gives Delta1=Delta2=0 (CIAB/timer B)");
        }
    }

    // ---- P5 leg (b) ---------------------------------------------------------
    {
        auto grade_p5b = [&](const std::string& bin, uint16_t n_mask) {
            Capture c = run_capture(bin, kRunBudget, {});
            SampleSeries s = read_series(c.log, kResults+8, 256);
            Anchor anc = measure_anchor(c.log);
            if (!c.loaded || !c.done.has_value() || !anc.ok) return false;
            uint64_t t0 = 0; bool have = false;
            std::vector<std::pair<int64_t,uint16_t>> pts;
            for (size_t i = 0; i < s.size(); ++i) {
                if (!s.cycle[i] || !s.value[i]) continue;
                if (!have) { t0 = *s.cycle[i]; have = true; }
                pts.push_back({(int64_t)e_index(*s.cycle[i], t0, anc.t_cpu), *s.value[i]});
            }
            (void)n_mask;
            if (pts.size() < 2) return false;
            bool ok = true;
            for (size_t i = 0; i+1 < pts.size(); ++i) {
                int expect_diff = int(pts[i+1].first - pts[i].first);
                int actual_diff = (int(pts[i].second) - int(pts[i+1].second)) & 0xFF;
                if (actual_diff != (expect_diff & 0xFF)) { ok = false; break; }
            }
            return ok;
        };
        bool b_ciaa_ta = grade_p5b(dir+"/p5b_ciaa_ta.bin", 0);
        bool b_ciaa_tb = grade_p5b(dir+"/p5b_ciaa_tb.bin", 0);
        bool b_ciab_ta = grade_p5b(dir+"/p5b_ciab_ta.bin", 0);
        bool b_ciab_tb = grade_p5b(dir+"/p5b_ciab_tb.bin", 0);
        checks.expect(b_ciaa_ta && b_ciaa_tb && b_ciab_ta && b_ciab_tb,
                      "P5-LEGB [p353: opus:F-signals.c2, sol:F-signals.c70]: (v_i-v_(i+1)) mod 256 = "
                      "e_(i+1)-e_i, for all four timers (Pre5.3: the E-index frame is valid for this "
                      "loop, P6)");
    }

    // ---- P6 (phi2 is the timing reference for bus transfers to a CIA) -----
    {
        Capture c = run_capture(dir+"/p6_k123.bin", kRunBudget, {});
        Anchor anc = measure_anchor(c.log);
        SampleSeries k1 = read_series(c.log, kResults+8, 200);
        SampleSeries k2 = read_series(c.log, kResults+408, 200);
        SampleSeries k3 = read_series(c.log, kResults+808, 200);
        auto phases_of = [&](const SampleSeries& s) {
            std::vector<double> ph; uint64_t t0 = 0; bool have = false;
            for (auto& cyc : s.cycle) {
                if (!cyc) continue;
                if (!have) { t0 = *cyc; have = true; }
                ph.push_back(fold_phase(*cyc, t0, anc.t_cpu));
            }
            return ph;
        };
        std::vector<int64_t> k3_cycles;
        for (auto& cyc : k3.cycle) if (cyc) k3_cycles.push_back((int64_t)*cyc);
        int64_t i3 = k3_cycles.size() >= 2 ? (k3_cycles[1]-k3_cycles[0]) : 0;
        bool pre61_ok = anc.ok && i3 > 0 && (i3 % (10*(int64_t)anc.t_cpu) != 0);
        checks.expect(pre61_ok, "P6-PRE61 [SPECIFICATION.md P6]: the measured I3 (K3's own iteration "
                      "time) is not a multiple of T_E");

        PhaseCheck pc1 = phase_check(phases_of(k1), anc.t_cpu);
        PhaseCheck pc2 = phase_check(phases_of(k2), anc.t_cpu);
        PhaseCheck pc3 = phase_check(phases_of(k3), anc.t_cpu);
        bool k1k2_ok = anc.ok && pc1.has_data && pc2.has_data && pc1.band_ok && pc2.band_ok &&
                       pc1.distinct <= 2 && pc2.distinct <= 2;
        checks.expect(k1k2_ok, "P6-K1K2 [p353,p354,p196]: K1 (CIAA PRB) and K2 (CIAB PRB) each show a "
                      "phase band no wider than 2*T_CPU, at most two values");
        bool k3_ok = anc.ok && pc3.has_data && !pc3.band_ok && pc3.distinct >= 5;
        checks.expect(k3_ok, "P6-K3 [p196]: K3 (a Chip-memory read, no CIA access) spreads across all "
                      "five phases 0,2,4,6,8*T_CPU, so the 2*T_CPU band does not hold for it");
        checks.note("P6 K1 band=" + std::to_string(pc1.band_width_t_cpu) + "*T_CPU distinct=" + std::to_string(pc1.distinct) +
                    "; K2 band=" + std::to_string(pc2.band_width_t_cpu) + "*T_CPU distinct=" + std::to_string(pc2.distinct) +
                    "; K3 band=" + std::to_string(pc3.band_width_t_cpu) + "*T_CPU distinct=" + std::to_string(pc3.distinct) +
                    "; I3=" + std::to_string(i3) + " clk_sys, T_E=" + std::to_string(10*anc.t_cpu));

        // "The R-SAMPLE loops used in P4, P5(b) and P9 ... are checked the
        // same way" -- one representative loop from each, reusing their
        // own already-built binaries.
        Capture p4c = run_capture(dir+"/p4_ciab_tb_l20.bin", kRunBudget, {});
        Anchor anc4 = measure_anchor(p4c.log);
        SampleSeries p4prb = read_series(p4c.log, kResults+8, 400);
        PhaseCheck pc4 = phase_check(phases_of(p4prb), anc4.t_cpu);
        bool p4_phase_ok = anc4.ok && pc4.has_data && pc4.band_ok && pc4.distinct <= 2;
        checks.expect(p4_phase_ok, "P6-P4LOOP [SPECIFICATION.md P6]: P4's own PRB-sample loop (CIAB/"
                      "timer B, L=20) meets the same phase band");

        Capture p5bc = run_capture(dir+"/p5b_ciab_tb.bin", kRunBudget, {});
        Anchor anc5 = measure_anchor(p5bc.log);
        SampleSeries p5blo = read_series(p5bc.log, kResults+8, 256);
        PhaseCheck pc5 = phase_check(phases_of(p5blo), anc5.t_cpu);
        bool p5b_phase_ok = anc5.ok && pc5.has_data && pc5.band_ok && pc5.distinct <= 2;
        checks.expect(p5b_phase_ok, "P6-P5BLOOP [SPECIFICATION.md P6]: P5 leg (b)'s own LO-read loop "
                      "(CIAB/timer B) meets the same phase band");

        Capture p9c = run_capture(dir+"/p9_legw_l40.bin", kRunBudget, {});
        Anchor anc9 = measure_anchor(p9c.log);
        SampleSeries p9s = read_series(p9c.log, kResults+8, 200);
        PhaseCheck pc9 = phase_check(phases_of(p9s), anc9.t_cpu);
        bool p9_phase_ok = anc9.ok && pc9.has_data && pc9.band_ok && pc9.distinct <= 2;
        checks.expect(p9_phase_ok, "P6-P9LOOP [SPECIFICATION.md P6]: P9 leg W's own sample loop (P2 leg "
                      "S's mechanics, CIAB/timer B, L=40) meets the same phase band");
    }

    // ---- P10 (claims that several propositions grade together) ------------
    // Each row is its own assertion, one per claim id (or id group), the
    // conjunction of the assertions SPECIFICATION.md's own P10 table names
    // for it (SPECIFICATION.md P10: "no one of them carries its id"). The
    // row "passes when every assertion it lists passed ...; it fails when
    // any of them failed; otherwise it is not graded" (SPECIFICATION.md
    // P10) -- a member can be NOTGRADED (P4, under its own failed P4-PRE)
    // without failing the row outright.
    {
        enum class V { PASS, FAIL, NOTGRADED };
        auto pf = [](bool ok) { return ok ? V::PASS : V::FAIL; };
        auto p4v = [&](bool ok) { return !pv.p4_graded ? V::NOTGRADED : (ok ? V::PASS : V::FAIL); };
        auto row = [&](std::initializer_list<std::pair<V,const char*>> members, const std::string& label) {
            bool any_fail = false; std::string notgraded;
            for (auto& [v, name] : members) {
                if (v == V::FAIL) any_fail = true;
                else if (v == V::NOTGRADED) { if (!notgraded.empty()) notgraded += ", "; notgraded += name; }
            }
            if (any_fail) { checks.expect(false, "P10 " + label); return; }
            if (!notgraded.empty()) { checks.record("P10 " + label + ": not graded -- " + notgraded + " not graded"); return; }
            checks.expect(true, "P10 " + label);
        };

        row({{pf(p2_toggle_ok),"P2"}, {pf(pv.p3_ta),"P3(TA)"}, {pf(pv.p3_tb),"P3(TB)"},
             {p4v(pv.p4_ta),"P4(TA)"}, {p4v(pv.p4_tb),"P4(TB)"}},
            "sol:F-timers.c13 [p342]: one bit selects toggle or a one-cycle positive pulse on every "
            "underflow; the toggle output is set high on start and low by RES (P2, P3, P4, all four "
            "instances each)");
        row({{pf(pv.p3_ta),"P3(TA)"}, {pf(pv.p3_tb),"P3(TB)"}, {p4v(pv.p4_ta),"P4(TA)"}, {p4v(pv.p4_tb),"P4(TB)"}},
            "opus:F-timers.c12, opus:F-timers.c13 [p342]: one bit selects the output while PB on/off "
            "is on; every underflow toggles it or gives a one-cycle positive pulse (P3, P4, all four "
            "instances each)");
        row({{pf(pv.p3_ta),"P3(TA)"}, {p4v(pv.p4_ta),"P4(TA)"}},
            "sol:F-cra.c16 [p349]: CRA OUTMODE: 0 pulse, 1 toggle (P3, P4, timer-A instances)");
        row({{pf(pv.p3_tb),"P3(TB)"}, {p4v(pv.p4_tb),"P4(TB)"}},
            "opus:F-crb.c3, sol:F-crb.c4, sol:F-crb.c13 [p349,p350]: CRB OUTMODE: 0 pulse, 1 toggle "
            "(P3, P4, timer-B instances)");
        row({{pf(pv.p5_ta),"P5(TA)"}, {pf(pv.p75),"P7.5"}},
            "sol:F-cra.c13 [p349]: CRA INMODE: 0 phi2, 1 CNT (P5, timer-A instances, and P7.5)");
        row({{pf(pv.p5_tb),"P5(TB)"}, {pf(pv.p865_01),"P8.65-01"}, {pf(pv.p865_10),"P8.65-10"}, {pf(pv.p865_11),"P8.65-11"}},
            "sol:F-crb.c7, sol:F-crb.c10 [p349,p350]: CRB INMODE: 00 phi2, 01 CNT, 10 timer A "
            "underflows, 11 timer A underflows while CNT is high (P5, timer-B instances, and P8.65 "
            "modes 01, 10 and 11)");

        // The bitmap claims split one row per id (each over its own
        // settings), not one row over "every field" (SPECIFICATION.md
        // P10: LOAD has no 0-setting, Section 0).
        row({{pf(pv.p70),"P7.0"}, {pf(pv.p1_control),"P1-CONTROL"}, {p4v(pv.p4_ta),"P4(TA)"},
             {pf(pv.p73),"P7.3"}, {pf(pv.p74),"P7.4"}, {pf(pv.p5_ta),"P5(TA)"}, {pf(pv.p76_ciaa),"P7.6-CIAA"}},
            "opus:F-cra.c12 [p349]: every 0-setting field of the CRA bitmap (P7.0 START=0; P1-CONTROL "
            "PBON=0; P4 OUTMODE=0, timer-A instances; P7.3 RUNMODE=0; P7.4 LOAD reads 0; P5 INMODE=0, "
            "timer-A instances; P7.6-CIAA SPMODE=0)");
        row({{pf(pv.p70),"P7.0"}, {pf(cra_ok),"P1-MAIN-CRA"}, {pf(pv.p3_ta),"P3(TA)"}, {pf(pv.p73),"P7.3"},
             {pf(pv.p74),"P7.4"}, {pf(pv.p75),"P7.5"}, {pf(pv.p76_ciab),"P7.6-CIAB"}, {pf(pv.p77),"P7.7"}},
            "opus:F-cra.c13 [p349]: every 1-setting field of the CRA bitmap (P7.0 START=1; P1 PBON=1, "
            "timer-A instances; P3 OUTMODE=1, timer-A instances; P7.3 RUNMODE=1; P7.4 LOAD=1; P7.5 "
            "INMODE=1; P7.6-CIAB SPMODE=1; P7.7 bit 7)");
        row({{pf(pv.p80),"P8.0"}, {pf(pv.p1_control),"P1-CONTROL"}, {p4v(pv.p4_tb),"P4(TB)"},
             {pf(pv.p83),"P8.3"}, {pf(pv.p84),"P8.4"}, {pf(pv.p5_tb),"P5(TB)"}, {pf(pv.p87_alarm),"P8.7"}},
            "opus:F-crb.c13 [p350]: every 0-setting field of the CRB bitmap (P8.0 START=0; P1-CONTROL "
            "PBON=0; P4 OUTMODE=0, timer-B instances; P8.3 RUNMODE=0; P8.4 LOAD reads 0; P5 INMODE=00, "
            "timer-B instances; P8.7 run Y ALARM=0)");
        row({{pf(pv.p80),"P8.0"}, {pf(crb_ok),"P1-MAIN-CRB"}, {pf(pv.p3_tb),"P3(TB)"}, {pf(pv.p83),"P8.3"},
             {pf(pv.p84),"P8.4"}, {pf(pv.p865_01),"P8.65-01"}, {pf(pv.p865_10),"P8.65-10"},
             {pf(pv.p865_11),"P8.65-11"}, {pf(pv.p87_alarm),"P8.7"}},
            "opus:F-crb.c14 [p350]: every 1-setting field of the CRB bitmap (P8.0 START=1; P1 PBON=1, "
            "timer-B instances; P3 OUTMODE=1, timer-B instances; P8.3 RUNMODE=1; P8.4 LOAD=1; P8.65 "
            "modes 01/10/11; P8.7 run X ALARM=1)");

        row({{p4v(pv.p4_ta),"P4(TA)"}, {p4v(pv.p4_tb),"P4(TB)"}, {pf(pv.p9_legf),"P9-LEGF"},
             {pv.p9_legw_graded ? pf(pv.p9_legw) : V::NOTGRADED,"P9-LEGW"}, {pf(pv.p865_10),"P8.65-10"}},
            "opus:F-timers.c6, sol:F-timers.c8 [p341]: the modes allow long time delays, variable "
            "width pulses, pulse trains, and variable frequency waveforms; the latch is a divider of the "
            "input clocking frequency (P4 pulse trains; P9 leg F frequency/divider and leg W width; "
            "P8.65 mode 10 long delays)");
    }

    // ---- Section 0 declines, printed as RECORDED lines -------------------
    checks.record("opus:F-portsig.c47 [p351]: declined whole -- \"PC...not used\" (CIAB); no output this "
                  "package sees shows a PC pin on either CIA, and \"not used\" describes no behaviour");
    checks.record("opus:F-signals.c1, sol:F-signals.c69 [p353]: declined whole -- both say phi2 is a "
                  "\"TTL compatible input\", an electrical property of an input pin that nothing a test "
                  "sees depends on");

    // ---- EP1, EP2, EP3 and F1's anchor, asserted on every capture the package
    // consumed (SPECIFICATION.md 1.2: "Every run asserts all three"). The
    // captures are recorded as they are made (register_run). EP2 is graded on
    // the runs that completed and passed F1: without F1's line cadence a
    // refresh read cannot be told from a stray one.
    {
        const auto& runs = run_registry();
        std::set<std::string> bins;
        size_t n_ep1 = 0, n_done = 0, n_f1 = 0, n_ep2_graded = 0, n_ep2 = 0, n_loaded = 0;
        std::string bad_ep1, bad_done, bad_f1, bad_ep2;
        auto add = [](std::string& list, const std::string& bin) {
            std::string shortname = bin.substr(bin.find_last_of('/') + 1);
            if (list.find(shortname) == std::string::npos) list += (list.empty() ? "" : ", ") + shortname;
        };
        for (const auto& r : runs) {
            bins.insert(r.bin);
            if (r.loaded) ++n_loaded;
            if (r.ep1) ++n_ep1; else add(bad_ep1, r.bin);
            if (r.done) ++n_done; else add(bad_done, r.bin);
            if (r.f1) ++n_f1; else add(bad_f1, r.bin);
            if (r.ep2_graded) { ++n_ep2_graded; if (r.ep2) ++n_ep2; else add(bad_ep2, r.bin); }
        }
        checks.note("per-run population: " + std::to_string(runs.size()) + " captures of " +
                    std::to_string(bins.size()) + " programs, " + std::to_string(n_loaded) + " loaded");
        checks.expect(!runs.empty() && n_ep1 == runs.size(),
            "EP1-PER-RUN [SPECIFICATION.md 1.2]: EP1 holds on every capture the package consumed");
        if (!bad_ep1.empty()) checks.note("EP1-PER-RUN failed on: " + bad_ep1);
        checks.expect(!runs.empty() && n_done == runs.size(),
            "EP3-PER-RUN [SPECIFICATION.md 1.2]: every capture the package consumed reaches DONE and has a final marker");
        if (!bad_done.empty()) checks.note("EP3-PER-RUN failed on: " + bad_done);
        checks.expect(!runs.empty() && n_f1 == runs.size(),
            "F1-ANCHOR-PER-RUN [p194,p195,p23]: F1's own anchor holds on every capture the package consumed");
        if (!bad_f1.empty()) checks.note("F1-ANCHOR-PER-RUN failed on: " + bad_f1);
        if (n_ep2_graded == runs.size() && !runs.empty()) {
            checks.expect(n_ep2 == n_ep2_graded,
                "EP2-PER-RUN [SPECIFICATION.md 1.2]: between the first and last marker, every capture the package "
                "consumed shows only CPU accesses and memory-refresh accesses");
            if (!bad_ep2.empty()) checks.note("EP2-PER-RUN failed on: " + bad_ep2);
        } else {
            checks.record("EP2-PER-RUN [SPECIFICATION.md 1.2]: not graded -- " + std::to_string(runs.size() - n_ep2_graded) +
                          " of " + std::to_string(runs.size()) + " captures did not complete or failed F1's anchor");
        }
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
