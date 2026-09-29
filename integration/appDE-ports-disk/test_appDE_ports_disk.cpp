// test_appDE_ports_disk.cpp -- appDE-ports-disk. See SPECIFICATION.md for
// every assertion's proposition, page, stimulus and expected value. This
// file only implements what SPECIFICATION.md already committed to.
//
// Scope built in this package: P5 (the pen-pressed button) only. P1-P4
// (the light-pen trigger/latch propositions) and section 0.1 (external
// disk identification) are not built -- see SPECIFICATION.md's build
// notes for what each would need.
//
// NMH_MUTATE selects a mutated program or a fault-injected parameter in
// place of its normal counterpart; none is exercised by a plain
// `make test`.
//
// MUTATION C0 stimulus red the harness drives pin 3 (LEFT*) of the
//   connector-identity connector instead of pin 4 (RIGHT*), via
//   NMH_MUTATE=C0
// MUTATION C1 stimulus red prog_pot_nodone.bin (the done mark is never
//   stored), via NMH_MUTATE=C1
// MUTATION P5 stimulus red pin 9 driven instead of pin 5 in the second
//   low interval, via NMH_MUTATE=P5-PIN9
// MUTATION P5 stimulus red POTGO's memory word stores $0001 instead of
//   the declared $0000, via NMH_MUTATE=P5-POTGO
// MUTATION P5 stimulus red one level interval shortened to under 300us,
//   via NMH_MUTATE=P5-SHORT

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
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

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

// ---- Memory layout (appDE_regs.i, restated; word addresses = byte/2) ---
constexpr uint32_t kCfgPotgo      = 0x0C0016 / 2;
constexpr uint32_t kMark0         = 0x0C0020 / 2;
constexpr uint32_t kMarkPhase     = 0x0C0022 / 2;
constexpr uint32_t kRingS2        = 0x0C0100 / 2;
constexpr uint32_t kRingS2Words   = 16;
constexpr uint32_t kRingPot       = 0x0C1000 / 2;
constexpr uint32_t kRingPotCount  = 64;
constexpr uint32_t kDoneAddrW     = 0x0C0FFE / 2;
constexpr uint16_t kDoneValue     = 0x600D;

constexpr uint64_t kBootBudget       = 6'000'000;
// The chipset's own internal reset (minimig_syscontrol, counted in whole
// video frames -- see nanomig_harness.h's boot() comment) releases well
// after cpu_nrst_out: measured, the program's first store (MARK0) lands
// only after roughly 4 frames. The harness synchronises on that store
// (run_until_write) rather than assuming a cycle count.
constexpr uint64_t kMark0Budget      = 8'000'000;  // generous wait for the program's own first store
constexpr uint64_t kC0FlipAt         = 400'000;    // host cycles the connector-identity pin is held active
constexpr uint64_t kMarkPhaseBudget  = 3'000'000;  // generous wait for the program's own phase marker
// A level interval must be long enough that the free-running beam (V is
// not synchronised to the interval boundaries: LPEN=0 throughout this
// program, so VPOSR/VHPOSR always read the moving beam) is guaranteed to
// pass through V=100-200 at least once inside it, on both video standards
// -- i.e. at least one whole field (measured: a PAL field is a little
// under 600'000 clk_sys cycles at this design's line/field lengths).
constexpr uint64_t kWindowNormal     = 700'000;    // per P5 level-interval duration
constexpr uint64_t kWindowShort      = 5'000;      // P5-SHORT: under the 300us settle bound below
constexpr uint64_t kSettleCycles     = 9'000;      // > 300us at ~28.375MHz clk_sys (p.240's own bound)
constexpr uint64_t kDoneBudget       = 2'500'000;  // follow-up budget for the sampling loop to reach DONE

// nmh::Config's joystick0/1 layout (BRIEF.md, nanomig_harness.h): bit7..0 =
// fire4,fire3,fire2,fire,up,down,left,right; 1 = the harness's own logical
// "active" sense (the DB9 pin driven to its active level), 0 = idle.
constexpr uint8_t kBitRight = 0x01;  // pin 4  (RIGHT*)
constexpr uint8_t kBitLeft  = 0x02;  // pin 3  (LEFT*)
constexpr uint8_t kBitFire2 = 0x20;  // pin 9 on the port-1 (J12) connector

// Which top-level nanomig.v joystick port to drive.
enum class NmhPort { kJoystick0, kJoystick1 };

void set_bit(nmh::NanomigHarness& h, NmhPort port, uint8_t bit, bool on) {
    auto* p = h.ports();
    uint8_t& reg = (port == NmhPort::kJoystick0) ? p->joystick0 : p->joystick1;
    reg = on ? uint8_t(reg | bit) : uint8_t(reg & ~bit);
}

// Which HRM-named connector a P5 run grades.
enum class Conn { kJ12, kJ11 };

struct Sample { uint64_t cycle; uint32_t val; };  // val = (hiword<<16)|loword

std::vector<Sample> decode_pairs(const std::vector<nmh::RamAccess>& log, uint32_t base, uint32_t words,
                                  uint64_t after_cycle, uint64_t before_cycle) {
    std::vector<Sample> out;
    bool have_hi = false; uint16_t hi = 0; uint64_t hi_cycle = 0;
    for (const auto& a : log) {
        if (!a.is_write || a.word_addr < base || a.word_addr >= base + words) continue;
        if (a.cycle <= after_cycle || a.cycle >= before_cycle) continue;
        if ((a.word_addr - base) % 2 == 0) { hi = a.data; hi_cycle = a.cycle; have_hi = true; }
        else if (have_hi) { out.push_back({hi_cycle, (uint32_t(hi) << 16) | a.data}); have_hi = false; }
    }
    return out;
}

struct PotSample { uint64_t cycle; uint32_t vh; uint16_t potgor; };

std::vector<PotSample> decode_pot(const std::vector<nmh::RamAccess>& log, uint32_t base, uint32_t stride_words, uint32_t total_words) {
    std::vector<nmh::RamAccess> rel;
    for (const auto& a : log) if (a.is_write && a.word_addr >= base && a.word_addr < base + total_words) rel.push_back(a);
    std::vector<PotSample> out;
    for (size_t i = 0; i + 2 < rel.size(); ) {
        uint32_t o0 = (rel[i].word_addr - base) % stride_words;
        uint32_t o1 = (rel[i+1].word_addr - base) % stride_words;
        uint32_t o2 = (rel[i+2].word_addr - base) % stride_words;
        if (o0 == 0 && o1 == 1 && o2 == 2) {
            out.push_back({rel[i].cycle, (uint32_t(rel[i].data) << 16) | rel[i+1].data, rel[i+2].data});
            i += 3;
        } else ++i;
    }
    return out;
}

std::optional<uint64_t> find_read(const std::vector<nmh::RamAccess>& log, uint32_t word_addr, uint16_t expect_val) {
    for (const auto& a : log) if (!a.is_write && a.word_addr == word_addr && a.data == expect_val) return a.cycle;
    return std::nullopt;
}

// P (17-bit) = (VPOSR<<16|VHPOSR) & 0x1FFFF; V=P>>8, H=P&0xFF (SPECIFICATION.md sec.2.2).
int Vof(uint32_t p17) { return int(p17 >> 8); }
uint32_t mask17(uint32_t raw32) { return raw32 & 0x1FFFF; }

// ---- C0: which top-level connector feeds which Amiga port -------------
// Established empirically, not assumed: the harness drives one nanomig.v
// joystick port's pin 4 (RIGHT*) active, then releases it, and this looks
// for exactly one of JOY0DAT/JOY1DAT's bit 1 changing between the two
// halves (p.329: "Right: bit#1"; p.281: "0=left controller pair, 1=right
// controller pair"). It compares the ring's first sample (pin 4 has been
// active since before the loop started) against its last (pin 4 has been
// released for a long stretch by the time the loop ends), not samples
// adjacent to the flip itself: a sample's logged cycle is when its value
// was written to the ring, a handful of cycles after the port read that
// produced it, so a sample logged close to the flip can already show the
// new level.
struct C0Result {
    bool ok = false;
    bool have_a = false, have_b = false;
    bool b_joy0 = false, a_joy0 = false, b_joy1 = false, a_joy1 = false;
};

C0Result eval_c0(const std::vector<Sample>& s2, uint64_t /*flip_cycle*/) {
    C0Result r;
    if (s2.empty()) return r;
    uint16_t b_joy0dat = uint16_t(s2.front().val >> 16), b_joy1dat = uint16_t(s2.front().val);
    uint16_t a_joy0dat = uint16_t(s2.back().val >> 16),  a_joy1dat = uint16_t(s2.back().val);
    r.have_b = r.have_a = true;
    r.b_joy0 = (b_joy0dat >> 1) & 1; r.b_joy1 = (b_joy1dat >> 1) & 1;
    r.a_joy0 = (a_joy0dat >> 1) & 1; r.a_joy1 = (a_joy1dat >> 1) & 1;
    bool joy0_follows = (r.b_joy0 != r.a_joy0);
    bool joy1_follows = (r.b_joy1 != r.a_joy1);
    r.ok = joy0_follows != joy1_follows;  // exactly one of JOY0DAT/JOY1DAT's bit 1 changed
    return r;
}

// ==== P5: pen-pressed button ============================================
//
// POTINP/POTGOR's bit layout, from p.240's Table 8-4 (restated in
// SPECIFICATION.md sec."P5.4"): bit 8 = pin 5 of J12 (port 1, DATLX), bit
// 10 = pin 9 of J12 (DATLY), bit 12 = pin 5 of J11 (port 2, DATRX), bit 14
// = pin 9 of J11 (DATRY).
//
// Driving, each confirmed by its own run's PASS/FAIL and MEASURED lines,
// not assumed from reading the design: J12's pin 5 (bit 8) follows
// nanomig.v's mouse_buttons bit 2 (the third mouse button). J12's pin 9
// (bit 10) can be forced toward 0 through nanomig.v's joystick0 bit 5
// (fire2) -- used only for the P5-PIN9 mutation, and confirmed by that
// mutation's own red result. J11's pin 9 (bit 14) is not driven by any
// assertion here, only observed to stay 1. No port this harness drives
// changes J11's pin 5 (bit 12): grade_p5() drives the same mouse_buttons
// signal that moves J12's bit 8 while sampling J11's run, and it stays
// constant -- that run's own MEASURED line is the evidence.

struct P5Run {
    bool loaded = false;
    std::optional<uint64_t> done, potgo_read, s2_end;
    std::vector<Sample> s2;
    std::vector<PotSample> pot;
    uint64_t flip_cycle = 0;
    uint64_t phase_start[4] = {0, 0, 0, 0};
    uint64_t phase_end[4]   = {0, 0, 0, 0};
};

P5Run run_p5(const nmh::Config& cfg, const std::string& bin, Conn conn, uint16_t cfg_potgo,
             bool mutate_c0_pin, bool mutate_pin9, bool mutate_short) {
    P5Run r;
    nmh::NanomigHarness h(cfg);
    h.poke_word(kCfgPotgo, cfg_potgo);
    if (h.load_program(bin) == 0) return r;
    r.loaded = true;
    h.boot(kBootBudget);

    // Synchronise on the program's own first store rather than assuming a
    // cycle count for the chipset's own internal reset (see kMark0Budget).
    h.run_until_write(kMark0 * 2, 0x5A5A, kMark0Budget);

    // ---- C0: drive the connector-identity pin on nanomig's joystick0
    // port (the same port for both Conn values -- the identity check does
    // not depend on which HRM connector is under test for P5 itself).
    uint8_t c0_bit = mutate_c0_pin ? kBitLeft : kBitRight;
    set_bit(h, NmhPort::kJoystick0, c0_bit, true);
    h.run_cycles(kC0FlipAt);
    r.flip_cycle = h.cycles();
    set_bit(h, NmhPort::kJoystick0, c0_bit, false);
    r.s2_end = h.run_until_write(kMarkPhase * 2, 0xB5B5, kMarkPhaseBudget);

    {
        const auto& log = h.ram_log();
        uint64_t cutoff = r.s2_end.value_or(r.flip_cycle + kMarkPhaseBudget) + 1;
        r.s2 = decode_pairs(log, kRingS2, kRingS2Words, 0, cutoff);
    }

    const uint64_t win = mutate_short ? kWindowShort : kWindowNormal;
    const bool levels_low[4] = {true, false, true, false};  // low(pressed) in windows 0,2; high in 1,3
    const uint64_t durs[4] = {kWindowNormal, kWindowNormal, win, kWindowNormal};
    for (int i = 0; i < 4; ++i) {
        r.phase_start[i] = h.cycles();
        bool pressed = levels_low[i];
        // P5-PIN9 substitutes the driven line only in the second low
        // interval (i==2), per SPECIFICATION.md's own contrast; the first
        // low interval (i==0) always drives pin 5, so the precondition
        // that pin 5 was driven at some point still holds.
        bool use_pin9 = mutate_pin9 && (i == 2) && conn == Conn::kJ12;
        auto* p = h.ports();
        if (use_pin9) {
            p->mouse_buttons = uint8_t(p->mouse_buttons & ~0x04u);
            set_bit(h, NmhPort::kJoystick0, kBitFire2, pressed);
        } else {
            p->mouse_buttons = pressed ? uint8_t(p->mouse_buttons | 0x04u) : uint8_t(p->mouse_buttons & ~0x04u);
            set_bit(h, NmhPort::kJoystick0, kBitFire2, false);
        }
        h.run_cycles(durs[i]);
        r.phase_end[i] = h.cycles();
    }
    h.ports()->mouse_buttons = 0;
    set_bit(h, NmhPort::kJoystick0, kBitFire2, false);

    r.done = h.run_until_write(kDoneAddrW * 2, kDoneValue, kDoneBudget);
    const auto& log = h.ram_log();
    r.pot = decode_pot(log, kRingPot, 3, 3 * kRingPotCount);
    r.potgo_read = find_read(log, kCfgPotgo, 0x0000);  // the specification's own declared value, always
    return r;
}

void grade_p5(Checks& checks, const std::string& tag, const P5Run& r, bool grade_pin5,
              int bit_pin5, int bit_pin9) {
    C0Result c0 = eval_c0(r.s2, r.flip_cycle);
    {
        std::string s2_end_s = r.s2_end ? std::to_string(*r.s2_end) : "none";
        std::string done_s = r.done ? std::to_string(*r.done) : "none";
        char buf[256];
        std::snprintf(buf, sizeof buf,
                      "%s diagnostic: flip_cycle=%llu s2_end=%s s2_samples=%zu (first=%llu last=%llu) "
                      "pot_samples=%zu (first=%llu last=%llu) done=%s",
                      tag.c_str(), (unsigned long long)r.flip_cycle, s2_end_s.c_str(),
                      r.s2.size(),
                      r.s2.empty() ? 0ULL : (unsigned long long)r.s2.front().cycle,
                      r.s2.empty() ? 0ULL : (unsigned long long)r.s2.back().cycle,
                      r.pot.size(),
                      r.pot.empty() ? 0ULL : (unsigned long long)r.pot.front().cycle,
                      r.pot.empty() ? 0ULL : (unsigned long long)r.pot.back().cycle,
                      done_s.c_str());
        checks.note(buf);
        char buf2[256];
        std::snprintf(buf2, sizeof buf2,
                      "%s C0 diagnostic: have_b=%d have_a=%d b_joy0bit1=%d a_joy0bit1=%d b_joy1bit1=%d a_joy1bit1=%d",
                      tag.c_str(), c0.have_b, c0.have_a, c0.b_joy0, c0.a_joy0, c0.b_joy1, c0.a_joy1);
        checks.note(buf2);
    }
    checks.expect(r.loaded && r.done.has_value(), tag + " precondition C1: the program finished "
                  "(prog_pot.bin's done mark is present)");
    checks.expect(c0.ok, tag + " precondition C0: connector identity (bit 1 of exactly one of "
                  "JOY0DAT/JOY1DAT follows the harness's connector-identity pin) [p.329,p.281,p.327]");
    checks.expect(r.potgo_read.has_value(), tag + " precondition: the port shows the CPU reading "
                  "POTGO's declared memory word ($0000) before the sampling loop [p.240]");

    if (!grade_pin5) {
        int n = 0, ones = 0, other_ones = 0;
        for (const auto& s : r.pot) {
            ++n;
            if ((s.potgor >> bit_pin5) & 1) ++ones;
            if ((s.potgor >> bit_pin9) & 1) ++other_ones;
        }
        char buf[256];
        std::snprintf(buf, sizeof buf,
                      "%s: POTGOR bit %d (this connector's pin 5) sampled %d times while the run drove "
                      "the same mouse-button signal that moves J12's bit 8; %d of %d reads were 1 "
                      "(bit %d, this connector's pin 9, was 1 in %d of %d)",
                      tag.c_str(), bit_pin5, n, ones, n, bit_pin9, other_ones, n);
        checks.note(buf);
        checks.record(tag + ": pin 5 of this connector is not testable at nanomig.v's ports -- no port "
                      "this harness drives changes POTGOR bit " + std::to_string(bit_pin5) +
                      " (opus:E-gameport.c6, sol:E-gameport.c5, p.327, for this connector's half only)");
        return;
    }

    for (int i = 0; i < 4; ++i) {
        bool want_low = (i == 0 || i == 2);
        uint64_t win_lo = r.phase_start[i] + kSettleCycles, win_hi = r.phase_end[i];
        int n = 0, bad = 0, other_bad = 0;
        for (const auto& s : r.pot) {
            int V = Vof(mask17(s.vh));
            if (V < 100 || V > 200) continue;
            if (s.cycle < win_lo || s.cycle >= win_hi) continue;
            ++n;
            bool bit_is_0 = !((s.potgor >> bit_pin5) & 1);
            if (bit_is_0 != want_low) ++bad;
            if (!((s.potgor >> bit_pin9) & 1)) ++other_bad;
        }
        std::string idx = std::to_string(i);
        char buf[256];
        std::snprintf(buf, sizeof buf,
                      "%s window %s (%s, V in 100-200, >=300us after the change): n=%d bad=%d other_bit_bad=%d",
                      tag.c_str(), idx.c_str(), want_low ? "pin5 driven low" : "pin5 released high", n, bad, other_bad);
        checks.note(buf);
        checks.expect(n >= 50, tag + " P5 precondition window " + idx +
                      ": at least 50 graded samples fall inside it (>=300us after the change, V in "
                      "100-200) [p.240]");
        checks.expect(n > 0 && bad == 0, tag + " P5(a) window " + idx +
                      ": POTGOR's pin-5 bit follows the driven level of pin 5 "
                      "(opus:E-gameport.c6, sol:E-gameport.c5, p.327; p.240 Table 8-4; p.238)");
        checks.expect(other_bad == 0, tag + " P5(b) window " + idx +
                      ": the same connector's pin-9 bit (POTGOR) stays 1 throughout "
                      "(opus:E-gameport.c6, sol:E-gameport.c5, p.327)");
    }
}

}  // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;
    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    // ==== Fixture, ahead of anything graded ==============================
    {
        nmh::Config cfg;
        nmh::NanomigHarness h(cfg);
        std::string bin = dir + "/prog_pot.bin";
        bool loaded = h.load_program(bin) > 0;
        checks.expect(loaded, "FIXTURE stimulus: prog_pot.bin loads at PROGRAM_ORG");
        if (loaded) {
            h.boot(kBootBudget);
            auto mark_cycle = h.run_until_write(kMark0 * 2, 0x5A5A, kBootBudget);
            checks.expect(mark_cycle.has_value(),
                          "FIXTURE stimulus: the program stores a known constant (MARK0=$5A5A) before "
                          "anything is read back");
        }
    }

    // ==== P5: connector J12 (port 1) -- graded ============================
    {
        bool mutate_c0    = (mv == "C0");
        bool mutate_c1    = (mv == "C1");
        bool mutate_pin9  = (mv == "P5-PIN9");
        bool mutate_potgo = (mv == "P5-POTGO");
        bool mutate_short = (mv == "P5-SHORT");
        std::string bin = dir + (mutate_c1 ? "/prog_pot_nodone.bin" : "/prog_pot.bin");
        uint16_t cfg_potgo = mutate_potgo ? 0x0001 : 0x0000;
        nmh::Config cfg;
        P5Run r = run_p5(cfg, bin, Conn::kJ12, cfg_potgo, mutate_c0, mutate_pin9, mutate_short);
        grade_p5(checks, "P5 J12", r, /*grade_pin5=*/true, /*bit_pin5=*/8, /*bit_pin9=*/10);
    }

    // ==== P5: connector J11 (port 2) -- pin 5 not reachable at nanomig.v's
    // ports; run only under the normal invocation (no claim id to protect
    // under a mutation). ==================================================
    if (mv.empty()) {
        nmh::Config cfg;
        P5Run r = run_p5(cfg, dir + "/prog_pot.bin", Conn::kJ11, 0x0000, false, false, false);
        grade_p5(checks, "P5 J11", r, /*grade_pin5=*/false, /*bit_pin5=*/12, /*bit_pin9=*/14);
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
