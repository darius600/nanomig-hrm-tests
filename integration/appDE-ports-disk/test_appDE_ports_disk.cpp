// test_appDE_ports_disk.cpp -- appDE-ports-disk. See SPECIFICATION.md for
// every assertion's proposition, page, stimulus and expected value. This
// file only implements what SPECIFICATION.md already committed to.
//
// Built here: P5 (the pen-pressed button) in every configuration and the light
// pen, P1-P4 with the preconditions C0-C6 and the graded line A2 (section 2 of
// SPECIFICATION.md). Section 0.1 (external disk identification) is not built;
// the claims of section 0 are named on RECORDED lines.
//
// NMH_MUTATE selects a mutated program or a fault-injected parameter in
// place of its normal counterpart; none is exercised by a plain
// `make test`. The values that start with LP- run one contrasting light-pen
// run (ECS, PAL, the harness's joystick1) beside the run with BPLCON0 bit 3
// clear; FIX-LOAD and FIX-MARK0 run only the fixture lines; the others change
// the P5 run (ECS, PAL).
//
// Fixture and P5 records:
// MUTATION FIXTURE-LOAD stimulus red NMH_MUTATE=FIX-LOAD: the harness loads prog_pot_missing.bin, which does not exist; the line prints FAIL and FIXTURE-MARK0 is NOT GRADED
// MUTATION FIXTURE-MARK0 stimulus red NMH_MUTATE=FIX-MARK0: prog_pot_fix.bin stores $BEEF as its first store; the harness takes the first store to MARK0 whatever it holds and the line prints FAIL
// MUTATION P5-C1-DONE stimulus red NMH_MUTATE=C1: prog_pot_nodone.bin never stores the done mark; the line prints FAIL and P5A-PIN5-FOLLOWS and P5B-OTHER-POT-BITS-1 are NOT GRADED
// MUTATION P5-C0-IDENTITY stimulus red NMH_MUTATE=C0: the harness holds the right mouse button (mouse_buttons bit 1) instead of the left one in the identity loop; neither bit 6 nor bit 7 of CIA-A port A follows and the line prints FAIL
// MUTATION P5-POTGO-READ stimulus red NMH_MUTATE=P5-POTGO: POTGO's memory word stores $0001 instead of the declared $0000; the line prints FAIL
// MUTATION P5-SCHEDULE stimulus red NMH_MUTATE=P5-SCHED: the harness also drives joystick0 bit 0 (pin 4 of the other connector) in the second level interval; the log shows changes on an input other than pin 5 and pin 9 and the line prints FAIL
// MUTATION P5-WINDOW-COUNT stimulus red NMH_MUTATE=P5-SHORT: the third level interval (window 2) lasts 5000 cycles, under the 300 us settle bound; it has no graded sample and the line prints FAIL for window 2
// MUTATION P5A-PIN5-FOLLOWS stimulus red NMH_MUTATE=P5-PIN9: in the second low interval (window 2) joystick1 bit 5 (pin 9) is driven low instead of pin 5; pin 5's bit stays 1 and the window-2 line prints FAIL
// MUTATION P5B-OTHER-POT-BITS-1 stimulus red NMH_MUTATE=P5-PIN9: in the second low interval (window 2) joystick1 bit 5 (pin 9) is driven low instead of pin 5; POTGOR bit 10 reads 0 and the window-2 line prints FAIL
//
// Light-pen records. A record with "private chipset" names a run on a copy of
// the chipset sources, outside the repository, that contain a latch following
// p.238 and p.239 (SPECIFICATION.md 4.6): on the repository's chipset the
// latch lines are red before any stimulus changes, so each of them is shown by
// an implementation record to pass on a chipset that follows the pages, and by
// a stimulus record to fail there when the stimulus changes. Both cores give
// every result below. An oracle record changes what the test declares, not
// what the machine is asked to do.
// MUTATION LP-FIXTURE-MARK0 stimulus red NMH_MUTATE=LP-FIX: prog_lpen_fix.bin stores $BEEF instead of $5A5A as its first store; the run synchronises on the first store whatever it holds, the fixture line prints FAIL, every other precondition line holds and the proposition lines are NOT GRADED
// MUTATION LP-C0-IDENTITY stimulus red NMH_MUTATE=LP-C0: S2 drives pin 3 instead of pin 4 of the connector under test; neither JOY0DAT nor JOY1DAT bit 1 follows and the line prints FAIL, the program still finishes
// MUTATION LP-C1-DONE stimulus red NMH_MUTATE=LP-C1: prog_lpen_nodone.bin never stores the done mark; the line prints FAIL, every sample is still stored
// MUTATION LP-C2-STORED stimulus red NMH_MUTATE=LP-C2: cfg_BPLCON0 holds $0000 while $0008 is declared; the port shows the read of $0000 and the line prints FAIL
// MUTATION LP-C3-A1-READS-CURRENT oracle red NMH_MUTATE=LP-A1: the model is built with 9 clk_sys per colour clock instead of 8; no model fits the reference reads and the line prints FAIL from the no-model branch, the comparator is not reached
// MUTATION LP-C3-A1-READS-CURRENT stimulus red NMH_MUTATE=LP-A1W: prog_lpen_v8.bin sets V8 of the vertical counter (VPOSW) for 100 reads inside the reference phase and clears it again; a model is fitted to the other reads, the flipped reads match no instant of their brackets and the line prints FAIL
// MUTATION LP-A2-LINE-LENGTHS oracle red NMH_MUTATE=LP-STD: the run is PAL and NTSC is declared; the measured pattern (all 227) is not the declared alternating one and the line prints FAIL
// MUTATION LP-A2-LINE-LENGTHS stimulus red NMH_MUTATE=LP-A2: prog_lpen_hw.bin writes a horizontal count of $F0 (VHPOSW) inside the reference phase; the largest H read exceeds $E2 and the line prints FAIL; the counter stays displaced, so LP-C3-A1-READS-CURRENT and LP-C3-A4-FIELD-ANCHOR print FAIL as well
// MUTATION LP-A2-LINE-LENGTHS implementation green: A2 passes in the four PAL runs on the repository's chipset and on the private chipset (the all-227 branch of the comparator; the alternating-line branch has no green control, SPECIFICATION.md 4.1)
// MUTATION LP-C3-A3-FIELD-LENGTH oracle red NMH_MUTATE=LP-STD: the run is PAL and NTSC is declared; 313 lines are not 262 or 263 and the line prints FAIL
// MUTATION LP-C3-A3-FIELD-LENGTH stimulus red NMH_MUTATE=LP-A3: the run is NTSC (chipset_config bit 1) and PAL is declared; 263 lines are not 312 or 313 and the line prints FAIL, no other precondition line moves
// MUTATION LP-C3-A4-FIELD-ANCHOR stimulus red NMH_MUTATE=LP-A4W: prog_lpen_m8.bin sets V8 of the vertical counter (VPOSW) for 100 reads of F2 before its pulse and clears it again; those reads do not equal the model and the line prints FAIL, no other precondition line moves
// MUTATION LP-C3-A4-FIELD-ANCHOR stimulus red NMH_MUTATE=LP-A4 private chipset: the F2 pulse is driven at line 40 instead of line 100, so the reads between line 40 and the declared pulse are held and not current; the line prints FAIL
// MUTATION LP-C4-DENSITY stimulus red NMH_MUTATE=LP-C4: prog_lpen_slow.bin delays every loop body by about 3.3 lines, the reads are far apart and the line prints FAIL
// MUTATION LP-C5-PULSES stimulus red NMH_MUTATE=LP-C5: the pulses are driven on pin 9 instead of pin 6; the line prints FAIL
// MUTATION LP-C6-LPENDIS-CLEAR stimulus red NMH_MUTATE=LP-C6: BEAMCON0 is stored and declared as $2020, bit 13 set; the line prints FAIL and C2 holds
// MUTATION LP-P1A-LATCH-HOLDS implementation green private chipset: P1A passes in every run, PAL and NTSC, on both connectors
// MUTATION LP-P1A-LATCH-HOLDS stimulus red NMH_MUTATE=LP-P1 private chipset: cfg_BPLCON0 stores and declares $0000 in the base run, LPEN stays clear, the reads keep following the beam and the line prints FAIL
// MUTATION LP-P1B-NO-LATCH-WHEN-CLEAR implementation green: P1B passes in every run on both connectors, on the repository's chipset and on the private chipset
// MUTATION LP-P1B-NO-LATCH-WHEN-CLEAR stimulus red NMH_MUTATE=LP-P1B private chipset: the bit-clear run stores and declares $0008, the first trigger freezes the registers and the line prints FAIL
// MUTATION LP-P1B-NO-LATCH-WHEN-CLEAR implementation red NMH_MUTATE=LP-BASE second private chipset: the latch captures a pulse whatever BPLCON0 bit 3 holds (the end-of-field latch stays gated by bit 3); the bit-clear run latches and the line prints FAIL, the base run's lines pass
// MUTATION LP-P1-LATCH-ENABLE implementation green private chipset: the aggregate passes in every run, PAL and NTSC
// MUTATION LP-P1-LATCH-ENABLE stimulus red NMH_MUTATE=LP-P1 private chipset: the base run's half (P1A) prints FAIL and the aggregate prints FAIL
// MUTATION LP-P1-LATCH-ENABLE stimulus red NMH_MUTATE=LP-P1B private chipset: the bit-clear run's half (P1B) prints FAIL and the aggregate prints FAIL
// MUTATION LP-P1-LATCH-ENABLE implementation red NMH_MUTATE=LP-BASE second private chipset: the latch captures a pulse whatever BPLCON0 bit 3 holds; the bit-clear half prints FAIL and the aggregate prints FAIL
// MUTATION LP-P2-LATCH-AT-PULSE implementation green private chipset: P2 passes in every run, PAL and NTSC
// MUTATION LP-P2-LATCH-AT-PULSE stimulus red NMH_MUTATE=LP-P2A private chipset: the F2 pulse is not driven; the first read after the would-be pulse is a live read and the line prints FAIL
// MUTATION LP-P2-LATCH-AT-PULSE stimulus red NMH_MUTATE=LP-P2B private chipset: the F3 pulse is driven 2 colour clocks later; the offset between F3 and F2 is 3 instead of 1 and the line prints FAIL
// MUTATION LP-P3A-HOLD implementation green private chipset: P3(a) passes in every run, PAL and NTSC
// MUTATION LP-P3A-HOLD stimulus red NMH_MUTATE=LP-P2A private chipset: the F2 pulse is not driven; the reads after it differ from the first one and the line prints FAIL
// MUTATION LP-P3B-HELD-PULSE-IGNORED implementation green private chipset: P3(b) passes in every run, PAL and NTSC
// MUTATION LP-P3B-HELD-PULSE-IGNORED stimulus red NMH_MUTATE=LP-P3B private chipset: the F7 pulse at line 60 is not driven, the line-180 pulse is the first trigger and the reads after it differ from P_F7; the line prints FAIL
// MUTATION LP-P3C-RELEASE implementation green private chipset: P3(c) passes in every run, PAL and NTSC
// MUTATION LP-P3C-RELEASE stimulus red NMH_MUTATE=LP-P3C private chipset: a pulse is added at line 50 of F8, whose live window is declared untriggered; the held reads are not current and the line prints FAIL
// MUTATION LP-P3D-PULSE-BEFORE-ENABLE implementation green private chipset: P3(d) passes in every run, PAL and NTSC
// MUTATION LP-P3D-PULSE-BEFORE-ENABLE stimulus red NMH_MUTATE=LP-P3D private chipset: the F9 pulse is driven at line 50 instead of line 10; the position is held from line 50 and the line prints FAIL
// MUTATION LP-P4-DETERMINE-TRIGGER implementation green private chipset: P4 passes in every run, PAL and NTSC
// MUTATION LP-P4-DETERMINE-TRIGGER stimulus red NMH_MUTATE=LP-P4A private chipset: a pulse at line 100 is added to F1, the pairs after it are equal and the blanking value is below $10500; the line prints FAIL
// MUTATION LP-LATCH-ENABLE-AND-POSITION implementation green private chipset: the aggregate passes in every run, PAL and NTSC
// MUTATION LP-LATCH-ENABLE-AND-POSITION stimulus red NMH_MUTATE=LP-P1 private chipset: LPEN stays clear in the base run and the line prints FAIL
// MUTATION LP-LATCH-ENABLE-AND-POSITION stimulus red NMH_MUTATE=LP-P1B private chipset: the bit-clear run latches and the line prints FAIL

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
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
// Measured: the program's first store (MARK0) lands about 2.27 million clk_sys
// cycles after cpu_nrst_out. The harness synchronises on that store (the first
// write to MARK0, whatever it stores) rather than assuming a cycle count.
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
// The harness drives the mouse-button inputs of nanomig.v: bit 0 is the left
// button (CIA-A port A, p.232, which names the connector), bit 2 is the pin-5
// button whose level POTGOR bit 8 or 12 shows. Which connector a run grades
// comes from the CIA-A bits (P5-C0-IDENTITY), not from the name of the input.

std::string num(long long v) { return std::to_string(v); }

// One top-level input bit the harness drives (port 0 = mouse_buttons, 1 = joystick0, 2 = joystick1).
struct Pin { const char* name; int port; uint8_t mask; };
enum PinId { kPinLeft = 0, kPinRight, kPinPin5, kPinPin9, kPinOther };
const Pin kPins[] = {
    {"mouse_buttons bit 0 (left button)", 0, 0x01},
    {"mouse_buttons bit 1 (right button)", 0, 0x02},
    {"mouse_buttons bit 2 (pin 5)", 0, 0x04},
    {"joystick1 bit 5 (pin 9)", 2, 0x20},
    {"joystick0 bit 0 (pin 4 of the other connector)", 1, 0x01},
};

uint8_t& pin_reg(nmh::NanomigHarness& h, int port) {
    auto* p = h.ports();
    return port == 0 ? p->mouse_buttons : (port == 1 ? p->joystick0 : p->joystick1);
}

struct PinEv { uint64_t sched = 0, applied = 0; int pin = 0; bool on = false; int readback = -1; };

// Drives one input bit and logs the change (a write that changes nothing logs nothing).
void drive(nmh::NanomigHarness& h, std::vector<PinEv>* log, int pin, bool on, uint64_t sched) {
    const Pin& pn = kPins[pin];
    uint8_t& r = pin_reg(h, pn.port);
    const uint8_t before = r;
    r = on ? uint8_t(r | pn.mask) : uint8_t(r & ~pn.mask);
    if (r == before || !log) return;
    PinEv e;
    e.sched = sched; e.applied = h.cycles(); e.pin = pin; e.on = on;
    e.readback = (pin_reg(h, pn.port) & pn.mask) ? 1 : 0;
    log->push_back(e);
}

struct FirstWrite { uint64_t cycle = 0; uint16_t data = 0; };

// Runs until the port shows the first write to a logical byte address, whatever it stores.
std::optional<FirstWrite> first_write(nmh::NanomigHarness& h, uint32_t logical_byte_addr, uint64_t max_cycles) {
    const uint32_t w = logical_byte_addr / 2;
    size_t seen = h.ram_log().size();
    const uint64_t start = h.cycles();
    while (h.cycles() - start < max_cycles) {
        h.run_cycles(16);
        const auto& lg = h.ram_log();
        for (; seen < lg.size(); ++seen)
            if (lg[seen].is_write && lg[seen].word_addr == w) return FirstWrite{lg[seen].cycle, lg[seen].data};
    }
    return std::nullopt;
}

struct P5Opts {
    bool ecs = true, ntsc = false;
    std::string bin = "prog_pot.bin";
    uint16_t cfg_potgo = 0x0000;
    int id_pin = kPinLeft;       // the input driven in the identity phase
    bool pin9 = false;           // second low interval: pin 9 instead of pin 5
    bool shortw = false;         // second low interval shorter than the settle bound
    bool other = false;          // an extra input (not pin 5 or pin 9) driven in the second interval
};

struct P5Run {
    bool loaded = false, released = false, mark0 = false;
    uint16_t mark0_val = 0;
    std::optional<uint64_t> done, potgo_read, s2_end;
    std::vector<uint16_t> s2;                 // CIA-A port A, one word per read
    std::vector<PotSample> pot;
    uint64_t flip_cycle = 0;
    uint64_t sched[4] = {0, 0, 0, 0};         // scheduled instants of the four level changes
    uint64_t phase_start[4] = {0, 0, 0, 0};
    uint64_t phase_end[4]   = {0, 0, 0, 0};
    bool idle_before = false;                 // every controller input inactive before the first change
    std::vector<PinEv> ev;                    // the level changes
};

P5Run run_p5(const std::string& dir, const P5Opts& o) {
    P5Run r;
    nmh::Config cfg;
    cfg.chipset_config = uint8_t((o.ecs ? 0x08 : 0x00) | (o.ntsc ? 0x02 : 0x00));
    nmh::NanomigHarness h(cfg);
    h.poke_word(kCfgPotgo, o.cfg_potgo);
    if (h.load_program(dir + "/" + o.bin) == 0) return r;
    r.loaded = true;
    r.released = h.boot(kBootBudget).cpu_released;
    if (!r.released) return r;

    // Synchronise on the program's own first store rather than assuming a
    // cycle count for the chipset's own internal reset (see kMark0Budget).
    auto m0 = first_write(h, kMark0 * 2, kMark0Budget);
    if (!m0) return r;
    r.mark0 = true; r.mark0_val = m0->data;

    // ---- C0: the left mouse button down, then up, while the program samples CIA-A port A
    drive(h, nullptr, o.id_pin, true, 0);
    h.run_cycles(kC0FlipAt);
    r.flip_cycle = h.cycles();
    drive(h, nullptr, o.id_pin, false, 0);
    r.s2_end = h.run_until_write(kMarkPhase * 2, 0xB5B5, kMarkPhaseBudget);
    {
        const auto& log = h.ram_log();
        const uint64_t cutoff = r.s2_end.value_or(r.flip_cycle + kMarkPhaseBudget) + 1;
        for (const auto& a : log)
            if (a.is_write && a.word_addr >= kRingS2 && a.word_addr < kRingS2 + kRingS2Words && a.cycle < cutoff)
                r.s2.push_back(a.data);
    }

    const uint64_t win = o.shortw ? kWindowShort : kWindowNormal;
    const bool levels_low[4] = {true, false, true, false};  // low(pressed) in windows 0,2; high in 1,3
    const uint64_t durs[4] = {kWindowNormal, kWindowNormal, win, kWindowNormal};
    auto* p = h.ports();
    r.idle_before = (p->mouse_buttons == 0 && p->joystick0 == 0 && p->joystick1 == 0);
    uint64_t t = h.cycles();
    for (int i = 0; i < 4; ++i) { r.sched[i] = t; t += durs[i]; }
    for (int i = 0; i < 4; ++i) {
        r.phase_start[i] = h.cycles();
        const bool pressed = levels_low[i];
        // P5-PIN9 substitutes the driven input only in the second low
        // interval (i==2), per SPECIFICATION.md's own contrast; the first
        // low interval (i==0) always drives pin 5, so the precondition
        // that pin 5 was driven at some point still holds.
        const bool use_pin9 = o.pin9 && (i == 2);
        drive(h, &r.ev, kPinPin5, pressed && !use_pin9, r.sched[i]);
        drive(h, &r.ev, kPinPin9, pressed && use_pin9, r.sched[i]);
        if (o.other && (i == 1 || i == 2)) drive(h, &r.ev, kPinOther, i == 1, r.sched[i]);
        h.run_cycles(durs[i]);
        r.phase_end[i] = h.cycles();
    }
    drive(h, nullptr, kPinPin5, false, 0);
    drive(h, nullptr, kPinPin9, false, 0);
    drive(h, nullptr, kPinOther, false, 0);

    r.done = h.run_until_write(kDoneAddrW * 2, kDoneValue, kDoneBudget);
    const auto& log = h.ram_log();
    r.pot = decode_pot(log, kRingPot, 3, 3 * kRingPotCount);
    r.potgo_read = find_read(log, kCfgPotgo, 0x0000);  // the specification's own declared value, always
    return r;
}

std::string cfg_name(bool ecs, bool ntsc) { return std::string(ecs ? "ECS" : "OCS") + "/" + (ntsc ? "NTSC" : "PAL"); }

// Which connector the harness's mouse inputs reach, from CIA-A port A (p.232: port 1 uses bit 6, port 2 uses bit 7).
struct P5Id { bool ok = false; bool j12 = false; int b6a = -1, b6b = -1, b7a = -1, b7b = -1; };

P5Id eval_p5_id(const std::vector<uint16_t>& s2) {
    P5Id id;
    if (s2.empty()) return id;
    id.b6b = (s2.front() >> 6) & 1; id.b7b = (s2.front() >> 7) & 1;
    id.b6a = (s2.back() >> 6) & 1;  id.b7a = (s2.back() >> 7) & 1;
    const bool f6 = (id.b6b != id.b6a), f7 = (id.b7b != id.b7a);
    id.ok = (f6 != f7);                // exactly one of the two bits follows the button
    id.j12 = f6;
    return id;
}

void grade_p5(Checks& checks, bool ecs, bool ntsc, const P5Run& r) {
    const P5Id id = eval_p5_id(r.s2);
    const int bit_pin5 = (id.ok && !id.j12) ? 12 : 8;       // the connector's own pin-5 bit
    const std::string conn = id.ok ? (id.j12 ? "J12" : "J11") : "connector not identified";
    const std::string tag = cfg_name(ecs, ntsc) + " " + conn;
    {
        std::string s2_end_s = r.s2_end ? std::to_string(*r.s2_end) : "none";
        std::string done_s = r.done ? std::to_string(*r.done) : "none";
        char buf[256];
        std::snprintf(buf, sizeof buf,
                      "%s diagnostic: flip_cycle=%llu s2_end=%s s2_samples=%zu pot_samples=%zu (first=%llu last=%llu) done=%s",
                      tag.c_str(), (unsigned long long)r.flip_cycle, s2_end_s.c_str(), r.s2.size(), r.pot.size(),
                      r.pot.empty() ? 0ULL : (unsigned long long)r.pot.front().cycle,
                      r.pot.empty() ? 0ULL : (unsigned long long)r.pot.back().cycle, done_s.c_str());
        checks.note(buf);
        char buf2[256];
        std::snprintf(buf2, sizeof buf2,
                      "%s C0 diagnostic: CIA-A port A bit 6 first=%d last=%d, bit 7 first=%d last=%d",
                      tag.c_str(), id.b6b, id.b6a, id.b7b, id.b7a);
        checks.note(buf2);
    }
    const bool c1 = r.loaded && r.done.has_value();
    const bool potgo = r.potgo_read.has_value();
    bool sched = r.idle_before && r.ev.size() == 4;
    std::string sched_detail = "idle before the first change: " + std::string(r.idle_before ? "yes" : "no") +
                               ", level changes logged: " + num(static_cast<long long>(r.ev.size())) + " of 4";
    for (size_t i = 0; sched && i < 4; ++i) {
        const PinEv& e = r.ev[i];
        const bool low = (i == 0 || i == 2);
        const bool good = e.sched == r.sched[i] && e.applied == e.sched && e.on == low &&
                          (e.pin == kPinPin5 || e.pin == kPinPin9) && e.readback == (low ? 1 : 0);
        if (!good) sched = false;
    }
    checks.expect(c1, "P5-C1-DONE " + tag + ": the program finished (prog_pot.bin's done mark is present)");
    checks.expect(id.ok, "P5-C0-IDENTITY " + tag + ": connector identity: exactly one of bit 6 and bit 7 of CIA-A port A follows the "
                  "left mouse button the harness drives; bit 6 is port 1 (J12), bit 7 is port 2 (J11) [p.232, p.327]");
    checks.expect(potgo, "P5-POTGO-READ " + tag + ": the port shows the CPU reading POTGO's declared memory word ($0000) "
                  "before the sampling loop [p.240]");
    checks.expect(sched, "P5-SCHEDULE " + tag + ": the harness log shows the four level changes of pin 5 (low, high, low, high) "
                  "at their scheduled cycles, read back as set, with every controller input inactive before the first and no "
                  "input other than pin 5 and pin 9 changing (" + sched_detail + ") [p.240]");
    const bool pre = c1 && id.ok && potgo && sched;

    // The other connector's pin-5 half: no input this harness drives reaches it.
    {
        int n = 0, ones = 0;
        const int other_bit = (bit_pin5 == 8) ? 12 : 8;
        const std::string other_name = (bit_pin5 == 8) ? "J11" : "J12";
        for (const auto& s : r.pot) { ++n; if ((s.potgor >> other_bit) & 1) ++ones; }
        char buf[256];
        std::snprintf(buf, sizeof buf,
                      "%s: POTGOR bit %d (%s's pin 5) sampled %d times while the run drove the mouse-button "
                      "input that moves bit %d; %d of %d reads were 1",
                      tag.c_str(), other_bit, other_name.c_str(), n, bit_pin5, ones, n);
        checks.note(buf);
        checks.record(tag + ": pin 5 of " + other_name + " is not testable at nanomig.v's ports -- no input this harness "
                      "drives changes POTGOR bit " + std::to_string(other_bit) +
                      " (opus:E-gameport.c6, sol:E-gameport.c5, p.327, for " + other_name + "'s half only)");
    }

    for (int i = 0; i < 4; ++i) {
        const bool want_low = (i == 0 || i == 2);
        const uint64_t win_lo = r.phase_start[i] + kSettleCycles, win_hi = r.phase_end[i];
        int n = 0, bad = 0, other_bad = 0;
        int zero_by_bit[16] = {0};
        for (const auto& s : r.pot) {
            const int V = Vof(mask17(s.vh));
            if (V < 100 || V > 200) continue;
            if (s.cycle < win_lo || s.cycle >= win_hi) continue;
            ++n;
            const bool bit_is_0 = !((s.potgor >> bit_pin5) & 1);
            if (bit_is_0 != want_low) ++bad;
            bool any = false;
            for (int b : {8, 10, 12, 14})
                if (b != bit_pin5 && !((s.potgor >> b) & 1)) { ++zero_by_bit[b]; any = true; }
            if (any) ++other_bad;
        }
        const std::string idx = std::to_string(i);
        char buf[256];
        std::snprintf(buf, sizeof buf,
                      "%s window %s (%s, V in 100-200, >=300us after the change): n=%d bad=%d other_bits_bad=%d (zero reads of bit 8: %d, 10: %d, 12: %d, 14: %d)",
                      tag.c_str(), idx.c_str(), want_low ? "pin5 driven low" : "pin5 released high", n, bad, other_bad,
                      zero_by_bit[8], zero_by_bit[10], zero_by_bit[12], zero_by_bit[14]);
        checks.note(buf);
        const bool cnt = n >= 50;
        checks.expect(cnt, "P5-WINDOW-COUNT " + tag + " window " + idx +
                      ": at least 50 graded samples fall inside it (>=300us after the change, V in "
                      "100-200) [p.240]");
        if (pre && cnt) {
            checks.expect(bad == 0, "P5A-PIN5-FOLLOWS " + tag + " window " + idx +
                          ": POTGOR's pin-5 bit follows the driven level of pin 5 (p.327; p.240 Table 8-4; p.238)");
            checks.expect(other_bad == 0, "P5B-OTHER-POT-BITS-1 " + tag + " window " + idx +
                          ": the pin-9 bits of both connectors and the other connector's pin-5 bit (POTGOR) stay 1 throughout (p.240, p.327)");
        } else {
            checks.record("P5A-PIN5-FOLLOWS " + tag + " window " + idx + ": NOT GRADED, a precondition does not hold (P5-C1-DONE, "
                          "P5-C0-IDENTITY, P5-POTGO-READ, P5-SCHEDULE or P5-WINDOW-COUNT)");
            checks.record("P5B-OTHER-POT-BITS-1 " + tag + " window " + idx + ": NOT GRADED, a precondition does not hold (P5-C1-DONE, "
                          "P5-C0-IDENTITY, P5-POTGO-READ, P5-SCHEDULE or P5-WINDOW-COUNT)");
        }
    }
}

// ==== light-pen runs (P1-P4, C0-C6) =========================================
// Memory layout restated from appDE_regs.i (word addresses = byte/2).
constexpr uint32_t kCfgBeamcon0   = 0x0C0010 / 2;
constexpr uint32_t kCfgBplconRef  = 0x0C0012 / 2;
constexpr uint32_t kCfgBplcon     = 0x0C0014 / 2;
constexpr uint32_t kMarkRefEnd    = 0x0C0024 / 2;
constexpr uint32_t kSamplesW      = 0x100000 / 2;
constexpr uint32_t kSamplesCount  = 66000;       // N_REF + N_MEAS reads of prog_lpen.s
constexpr uint32_t kCodeLoW       = 0x080400 / 2;
constexpr uint32_t kCodeHiW       = 0x080400 / 2 + 128;  // every light-pen program is under 256 bytes
constexpr int      kCck           = 8;           // clk_sys cycles per colour clock
constexpr uint8_t  kPin6          = 0x10;        // joystick byte: fire = pin 6 (LPENTG*)
constexpr uint8_t  kPin9          = 0x20;        // fire2 = pin 9
constexpr uint8_t  kPin4          = 0x01;        // right = pin 4
constexpr uint8_t  kPin3          = 0x02;        // left = pin 3
constexpr int      kPulseCck      = 4;           // pulse width in colour clocks (SPECIFICATION.md 2.5)

int64_t fdiv(int64_t n, int64_t d) { int64_t q = n / d; if ((n % d != 0) && ((n < 0) != (d < 0))) --q; return q; }
int64_t fmod_(int64_t n, int64_t d) { return n - fdiv(n, d) * d; }
std::string hexs(unsigned v) { char b[16]; std::snprintf(b, sizeof b, "%x", v); return b; }

struct LS {                  // one long read VPOSR:VHPOSR
    int64_t a = 0;           // last visible CPU access before the reads
    int64_t b = 0;           // first visible CPU access after them (the store)
    uint32_t raw = 0;
    uint32_t P = 0;          // bits 16-0 of the long word (SPECIFICATION.md 2.2)
    int V() const { return int(P >> 8); }
    int H() const { return int(P & 0xFF); }
};

struct Pulse { int field; int line; int h; };

// The schedule of SPECIFICATION.md 2.5 (fields counted from F0).
const std::vector<Pulse> kBaseSchedule = {
    {2, 100, 0x40}, {3, 100, 0x41}, {4, 100, 0x48}, {5, 101, 0x40}, {6, 258, 0x40},
    {7, 60, 0x40}, {7, 180, 0x40}, {9, 10, 0x40}};
constexpr int kLastField = 10;

// What the test expects was stored and which standard it expects (SPECIFICATION.md 2.8).
struct Decl {
    bool ntsc = false;
    int R = kCck;
    uint16_t beamcon0 = 0x0020, bplcon0_ref = 0x0000, bplcon0 = 0x0008;
    int exp_ref = 20000, exp_meas = 46000;
    int rb() const { return ntsc ? 21 : 29; }
};

struct LpOpts {
    bool ecs = true, ntsc = false;
    int x = 1;                         // connector under test: 0 = joystick0, 1 = joystick1
    std::string prog = "prog_lpen.bin";
    uint16_t beamcon0 = 0x0020;        // stored in cfg_BEAMCON0
    uint16_t bplcon0_ref = 0x0000;     // stored in cfg_BPLCON0_ref
    uint16_t bplcon0 = 0x0008;         // stored in cfg_BPLCON0
    uint8_t  c0_pin = kPin4;           // S2 identity pin
    uint8_t  pulse_pin = kPin6;        // pin the pulses are driven on
    std::vector<Pulse> schedule = kBaseSchedule;
    Decl decl;
    bool keep_log = false;
    bool bit_clear = false;            // the run with BPLCON0 bit 3 clear (the other half of P1)
};

struct PinEvent { int64_t cycle; int conn; uint8_t bit; bool on; int64_t applied = -1; int readback = -1; };
struct CfgRead { int64_t cycle = -1; int val = -1; };

struct LpRun {
    LpOpts o;
    bool loaded = false, released = false, mark0 = false, s2done = false, refend = false, done = false;
    uint16_t mark0_val = 0;
    int64_t t_mark0 = 0, t_flip = 0, t_s2end = 0, t_refend = 0, t_done = 0;
    CfgRead beam, ref, bpl;
    std::vector<Sample> s2;
    size_t n_s2 = 0;
    std::vector<LS> samples;
    std::vector<int64_t> frame_start, frame_len;   // video fields inside the reference phase
    std::vector<PinEvent> events;
    int64_t f0_sched = 0;                          // F0 as the scheduler took it
    std::vector<nmh::RamAccess> log;               // only when LpOpts::keep_log is set
};

CfgRead find_cfg_read(const std::vector<nmh::RamAccess>& log, uint32_t word_addr, int64_t after) {
    CfgRead c;
    for (const auto& a : log)
        if (!a.is_write && a.word_addr == word_addr && int64_t(a.cycle) > after) { c.cycle = int64_t(a.cycle); c.val = a.data; break; }
    return c;
}

std::vector<LS> extract_samples(const std::vector<nmh::RamAccess>& log) {
    std::vector<int64_t> code;            // cycles of the program's own instruction fetches
    std::vector<uint16_t> hi(kSamplesCount, 0), lo(kSamplesCount, 0);
    std::vector<int64_t> hic(kSamplesCount, -1), loc(kSamplesCount, -1);
    for (const auto& a : log) {
        if (!a.is_write) {
            if (a.word_addr >= kCodeLoW && a.word_addr < kCodeHiW) code.push_back(int64_t(a.cycle));
        } else if (a.word_addr >= kSamplesW && a.word_addr < kSamplesW + 2 * kSamplesCount) {
            uint32_t i = (a.word_addr - kSamplesW);
            if (i % 2 == 0) { hi[i / 2] = a.data; hic[i / 2] = int64_t(a.cycle); }
            else            { lo[i / 2] = a.data; loc[i / 2] = int64_t(a.cycle); }
        }
    }
    std::vector<LS> out;
    for (uint32_t i = 0; i < kSamplesCount; ++i) {
        if (hic[i] < 0 || loc[i] < 0) break;
        LS s;
        s.b = hic[i];
        auto it = std::lower_bound(code.begin(), code.end(), s.b);   // first fetch at or after b
        s.a = (it == code.begin()) ? s.b : *(it - 1);
        s.raw = (uint32_t(hi[i]) << 16) | lo[i];
        s.P = s.raw & 0x1FFFF;
        out.push_back(s);
    }
    return out;
}

// Field lengths in clk_sys from the vertical-sync pulses of the video port, between cycles lo and hi.
// A field is counted when its end edge is seen; a sync already high at lo is not an edge.
void video_frames(const std::vector<nmh::VideoSample>& v, int64_t lo, int64_t hi,
                  std::vector<int64_t>& starts, std::vector<int64_t>& lens) {
    bool prev = true;
    int64_t last = -1;
    for (int64_t i = lo; i < hi && i < int64_t(v.size()); ++i) {
        bool vs = v[size_t(i)].vs;
        if (vs && !prev) {
            if (last >= 0) { starts.push_back(last); lens.push_back(i - last); }
            last = i;
        }
        prev = vs;
    }
}

// ---- the beam model (SPECIFICATION.md 2.6) ------------------------------
struct Model {
    bool ok = false;
    int R = kCck;
    int nlines = 0;
    int kind = -1;                 // 0: all 227, 1: 227 first then alternating, 2: 228 first, 3: all 228
    int vlag = 0;                  // colour clocks by which the vertical count trails the horizontal wrap
    std::vector<int> start, len;   // colour clocks; start has nlines + 1 entries
    int total = 0;
    int64_t T0 = 0, lo = 0, hi = -1;   // T0: the instant a field starts; [lo,hi]: every T0 the reads allow
    int u = 0;                         // uncertainty in colour clocks
    size_t usable = 0, covering = 0;
    int64_t F() const { return int64_t(total) * R; }
    int64_t t_of(int64_t field, int v, int h) const { return T0 + field * F() + (int64_t(start[size_t(v)]) + h) * R; }
    int64_t field_of(int64_t t) const { return fdiv(t - T0, F()); }
    int line_of(int64_t n) const {      // n in [0, total)
        auto it = std::upper_bound(start.begin(), start.end(), int(n));
        return int(it - start.begin()) - 1;
    }
    void pos(int64_t t, int& v, int& h) const {
        int64_t n = fmod_(t - T0, F()) / R;
        int w = line_of(n);
        h = int(n - start[size_t(w)]);
        v = line_of(fmod_(n - vlag, total));
    }
};

int hyp_len(int kind, int v) {
    switch (kind) { case 0: return 227; case 1: return 227 + (v & 1); case 2: return 228 - (v & 1); default: return 228; }
}

Model make_hyp(int kind, int n, int R) {
    Model m;
    m.R = R; m.kind = kind; m.nlines = n;
    m.start.assign(size_t(n) + 1, 0);
    m.len.assign(size_t(n), 0);
    int acc = 0;
    for (int v = 0; v < n; ++v) { m.start[size_t(v)] = acc; m.len[size_t(v)] = hyp_len(kind, v); acc += m.len[size_t(v)]; }
    m.start[size_t(n)] = acc;
    m.total = acc;
    return m;
}

// Fits T0 to the reads: every read bounds T0 to an interval; the model keeps the largest set of
// reads whose intervals overlap, and u is the width of that overlap.
bool fit_t0(Model& m, const std::vector<LS>& ref) {
    struct Iv { int64_t lo, hi; };
    std::vector<Iv> iv;
    int64_t F = m.F(), base = 0;
    bool have_base = false;
    for (const auto& s : ref) {
        int V = s.V(), H = s.H();
        if (V >= m.nlines || H >= m.len[size_t(V)]) continue;
        int64_t n = int64_t(m.start[size_t(V)]) + H;
        if (H < m.vlag) n = int64_t(m.start[size_t(V) + 1]) + H;      // the vertical count has not advanced yet
        int64_t lo = s.a - (n + 1) * m.R + 1, hi = s.b - n * m.R;
        if (!have_base) { base = lo; have_base = true; }
        int64_t k = fdiv(lo - base + F / 2, F);
        iv.push_back({lo - k * F, hi - k * F});
    }
    m.usable = iv.size();
    if (iv.size() < 100) return false;
    std::vector<std::pair<int64_t, int>> ev;
    for (const auto& i : iv) { ev.push_back({i.lo, +1}); ev.push_back({i.hi + 1, -1}); }
    std::sort(ev.begin(), ev.end());
    int cov = 0, best = 0;
    int64_t best_lo = 0, best_hi = -1;
    for (size_t i = 0; i + 1 < ev.size(); ++i) {
        cov += ev[i].second;
        if (cov > best) { best = cov; best_lo = ev[i].first; best_hi = ev[i + 1].first - 1; }
    }
    m.covering = size_t(best);
    m.lo = best_lo; m.hi = best_hi;
    m.u = int((best_hi - best_lo + 1 + m.R - 1) / m.R);
    m.T0 = fmod_((best_lo + best_hi) / 2, F);
    m.ok = best_hi >= best_lo;
    return m.ok;
}

// "Equals the model value at some instant inside the bracket", for each of the two words.
bool word_vh_ok(const Model& m, const LS& s) {
    uint32_t want = s.P & 0xFFFF;
    for (int64_t t = s.a; t <= s.b; ++t) {
        int v, h; m.pos(t, v, h);
        if ((uint32_t((v & 0xFF) << 8) | uint32_t(h)) == want) return true;
    }
    return false;
}
bool word_v8_ok(const Model& m, const LS& s) {
    uint32_t want = (s.P >> 16) & 1;
    for (int64_t t = s.a; t <= s.b; ++t) {
        int v, h; m.pos(t, v, h);
        if (uint32_t((v >> 8) & 1) == want) return true;
    }
    return false;
}
bool current(const Model& m, const LS& s) { return word_vh_ok(m, s) && word_v8_ok(m, s); }

// Builds the model for a run from its reference-phase reads and the video field lengths. Of the
// hypotheses whose field length equals the video field length it keeps the first under which every
// reference-phase word read is current; if none is, the one that covers the most reads.
Model build_model(const LpRun& r, const Decl& d) {
    Model none;
    if (r.samples.size() < size_t(d.exp_ref) || r.frame_len.size() < 2) return none;
    std::vector<LS> ref(r.samples.begin(), r.samples.begin() + d.exp_ref);
    std::vector<int64_t> lens(r.frame_len.begin() + 1, r.frame_len.end());   // the first field is partial
    std::sort(lens.begin(), lens.end());
    int64_t Fclk = lens[lens.size() / 2];
    Model best;
    bool have = false;
    for (int kind = 0; kind < 4; ++kind) {
        for (int n = 250; n <= 330; ++n) {
            for (int vlag = 0; vlag <= 2; ++vlag) {
                Model m = make_hyp(kind, n, d.R);
                m.vlag = vlag;
                if (m.F() != Fclk) continue;
                if (!fit_t0(m, ref)) continue;
                bool clean = true;
                for (const auto& s : ref) if (!word_vh_ok(m, s) || !word_v8_ok(m, s)) { clean = false; break; }
                if (clean) return m;
                if (!have || m.covering > best.covering) { best = m; have = true; }
            }
        }
    }
    return have ? best : none;
}

// ---- scheduling the pulses against the model ------------------------------
std::vector<PinEvent> make_events(const LpRun& r, const Model& m, int64_t f0) {
    std::vector<PinEvent> ev;
    if (!m.ok) return ev;
    for (const auto& p : r.o.schedule) {
        int64_t t_fall = m.t_of(f0 + p.field, p.line, p.h) + m.R / 2;
        ev.push_back({t_fall, r.o.x, r.o.pulse_pin, true});
        ev.push_back({t_fall + int64_t(kPulseCck) * m.R, r.o.x, r.o.pulse_pin, false});
    }
    return ev;
}

LpRun lp_run(const std::string& dir, const LpOpts& o) {
    LpRun r;
    r.o = o;
    nmh::Config cfg;
    cfg.chipset_config = uint8_t((o.ecs ? 0x08 : 0x00) | (o.ntsc ? 0x02 : 0x00));
    nmh::NanomigHarness h(cfg);
    h.poke_word(kCfgBeamcon0, o.beamcon0);
    h.poke_word(kCfgBplconRef, o.bplcon0_ref);
    h.poke_word(kCfgBplcon, o.bplcon0);
    if (h.load_program(dir + "/" + o.prog) == 0) return r;
    r.loaded = true;
    r.released = h.boot(kBootBudget).cpu_released;
    if (!r.released) return r;

    auto m0 = first_write(h, kMark0 * 2, kMark0Budget);
    r.mark0 = m0.has_value();
    if (!r.mark0) return r;
    r.t_mark0 = int64_t(m0->cycle);
    r.mark0_val = m0->data;

    // S2: pin 4 (pin 3 in the contrast) of the connector under test low, then high.
    auto* p = h.ports();
    uint8_t& jreg = (o.x == 0) ? p->joystick0 : p->joystick1;
    jreg = uint8_t(jreg | o.c0_pin);
    h.run_cycles(kC0FlipAt);
    r.t_flip = int64_t(h.cycles());
    jreg = uint8_t(jreg & ~o.c0_pin);
    auto s2e = h.run_until_write(kMarkPhase * 2, 0xB5B5, kMarkPhaseBudget);
    r.s2done = s2e.has_value();
    if (!r.s2done) return r;
    r.t_s2end = int64_t(*s2e);
    r.s2 = decode_pairs(h.ram_log(), kRingS2, kRingS2Words, 0, uint64_t(r.t_s2end) + 1);
    r.n_s2 = r.s2.size();

    auto re = h.run_until_write(kMarkRefEnd * 2, 0xA001, 30'000'000);
    r.refend = re.has_value();
    if (!r.refend) return r;
    r.t_refend = int64_t(*re);
    {
        const auto& lg = h.ram_log();
        r.ref = find_cfg_read(lg, kCfgBplconRef, r.t_s2end);
        r.beam = find_cfg_read(lg, kCfgBeamcon0, r.t_s2end);
        r.samples = extract_samples(lg);
        video_frames(h.video(), r.ref.cycle > 0 ? r.ref.cycle : r.t_s2end, int64_t(h.cycles()),
                     r.frame_start, r.frame_len);
    }
    Model m = build_model(r, o.decl);
    r.f0_sched = m.ok ? m.field_of(r.t_refend + 40) : 0;
    r.events = make_events(r, m, r.f0_sched);
    std::sort(r.events.begin(), r.events.end(),
              [](const PinEvent& x, const PinEvent& y) { return x.cycle < y.cycle; });
    for (auto& e : r.events) {
        int64_t now = int64_t(h.cycles());
        if (e.cycle > now) h.run_cycles(uint64_t(e.cycle - now));
        uint8_t& j = (e.conn == 0) ? p->joystick0 : p->joystick1;
        j = e.on ? uint8_t(j | e.bit) : uint8_t(j & ~e.bit);
        e.applied = int64_t(h.cycles());
        e.readback = (e.conn == 0) ? p->joystick0 : p->joystick1;
    }
    auto dn = h.run_until_write(kDoneAddrW * 2, kDoneValue, 40'000'000);
    r.done = dn.has_value();
    if (r.done) r.t_done = int64_t(*dn);
    {
        const auto& lg = h.ram_log();
        r.samples = extract_samples(lg);
        r.bpl = find_cfg_read(lg, kCfgBplcon, r.t_refend);
        if (o.keep_log) r.log = lg;
    }
    return r;
}

// ---- analysis -----------------------------------------------------------
struct Win { int64_t t0, t1; };            // reads whose both words lie inside: a >= t0 and b < t1

std::vector<size_t> rd_idx(const std::vector<LS>& s, int64_t t0, int64_t t1) {
    std::vector<size_t> o;
    auto it = std::lower_bound(s.begin(), s.end(), t0, [](const LS& x, int64_t v) { return x.a < v; });
    for (; it != s.end() && it->b < t1; ++it) o.push_back(size_t(it - s.begin()));
    return o;
}

struct LpAn {
    Model m;
    int64_t f0 = 0;
    std::string tag;                       // e.g. "ECS/PAL J12"
    bool conn_known = false, is_j12 = false;
    bool fixture = false, c0 = false, c1 = false, c2 = false, a1 = false, a2 = false, a3 = false, a4 = false,
         c4 = false, c5 = false, c6 = false, c6_applicable = false;
    bool pre_ok = false;                   // C0-C6 hold (A2 is its own graded line, not a precondition)
    bool have_model = false;
    // measured numbers for the MEASURED lines
    int u = 0, nlines = 0, kind = -1;
    size_t usable = 0, covering = 0, a1_bad = 0, maxH = 0;
    std::string c4_detail, c5_detail, a4_detail, c2_detail;
    // per triggered field (index = field number)
    bool have_P[kLastField + 2] = {false};
    uint32_t P[kLastField + 2] = {0};
    int64_t t_fall[kLastField + 2] = {0}, t_rise[kLastField + 2] = {0};
    // proposition results for this run
    bool p2_a = false, p2_c = false, p2_ok = false;
    bool p1 = false, p1b = false, p3a = false, p3b = false, p3c = false, p3d = false, p4 = false;
    std::string p_detail;
};

// The first declared pulse of a triggered field (fields 2..7).
const Pulse* first_pulse(int field) {
    for (const auto& p : kBaseSchedule) if (p.field == field) return &p;
    return nullptr;
}

LpAn analyze_run(const LpRun& r) {
    LpAn A;
    const Decl& d = r.o.decl;
    const LpOpts& o = r.o;
    A.m = build_model(r, d);
    const Model& m = A.m;
    // connector identity (C0)
    C0Result c0 = eval_c0(r.s2, uint64_t(r.t_flip));
    A.c0 = c0.ok;
    if (c0.ok) { A.conn_known = true; A.is_j12 = (c0.b_joy0 != c0.a_joy0); }
    A.tag = std::string(o.ecs ? "ECS" : "OCS") + "/" + (o.ntsc ? "NTSC" : "PAL") + " " +
            (A.conn_known ? (A.is_j12 ? "J12" : "J11") : (o.x == 0 ? "joystick0" : "joystick1")) +
            (o.bit_clear ? " (BPLCON0 bit 3 clear)" : "");
    A.fixture = r.mark0 && r.mark0_val == 0x5A5A;
    A.c1 = r.done && r.samples.size() == size_t(d.exp_ref + d.exp_meas) && r.n_s2 == 3000;
    // C2: the port shows the CPU reading each configuration word, with the declared value, in order
    {
        bool beam_ok = !o.ecs || (r.beam.cycle > 0 && r.beam.val == d.beamcon0);
        bool ref_ok = r.ref.cycle > 0 && r.ref.val == d.bplcon0_ref;
        bool bpl_ok = r.bpl.cycle > 0 && r.bpl.val == d.bplcon0;
        bool order_ok = (!o.ecs || r.beam.cycle < r.ref.cycle) && r.ref.cycle < r.bpl.cycle;
        A.c2 = beam_ok && ref_ok && bpl_ok && order_ok;
        A.c2_detail = "BEAMCON0 read=" + (r.beam.cycle > 0 ? "$" + hexs(unsigned(r.beam.val)) : std::string("none")) +
                      " declared=$" + hexs(d.beamcon0) + (o.ecs ? "" : " (OCS: not written)") +
                      ", BPLCON0 ref read=" + (r.ref.cycle > 0 ? "$" + hexs(unsigned(r.ref.val)) : std::string("none")) +
                      " declared=$" + hexs(d.bplcon0_ref) +
                      ", BPLCON0 read=" + (r.bpl.cycle > 0 ? "$" + hexs(unsigned(r.bpl.val)) : std::string("none")) +
                      " declared=$" + hexs(d.bplcon0);
    }
    A.c6_applicable = o.ecs;
    A.c6 = !o.ecs || (r.beam.cycle > 0 && ((r.beam.val >> 13) & 1) == 0);
    A.have_model = m.ok && r.samples.size() >= size_t(d.exp_ref);
    if (!A.have_model) {
        A.c5_detail = A.a4_detail = A.c4_detail = "no model";
        return A;
    }
    A.u = m.u; A.nlines = m.nlines; A.kind = m.kind; A.usable = m.usable; A.covering = m.covering;
    const std::vector<LS>& S = r.samples;
    // A1: every reference-phase word read equals the model value at some instant inside its bracket
    for (int i = 0; i < d.exp_ref; ++i) if (!word_vh_ok(m, S[size_t(i)]) || !word_v8_ok(m, S[size_t(i)])) ++A.a1_bad;
    A.a1 = (A.a1_bad == 0);
    // A2: line lengths and the largest H
    int maxH = 0;
    for (int i = 0; i < d.exp_ref; ++i) maxH = std::max(maxH, S[size_t(i)].H());
    A.maxH = size_t(maxH);
    {
        bool pat = d.ntsc ? (m.kind == 1 || m.kind == 2) : (m.kind == 0);
        bool hmax = maxH <= (d.ntsc ? 0xE3 : 0xE2);
        A.a2 = pat && hmax;
    }
    // A3: the field length, the same for every reference field
    {
        bool same = true;
        for (size_t i = 2; i < r.frame_len.size(); ++i) if (r.frame_len[i] != r.frame_len[1]) same = false;
        bool inset = d.ntsc ? (m.nlines == 262 || m.nlines == 263) : (m.nlines == 312 || m.nlines == 313);
        A.a3 = same && inset && r.frame_len.size() >= 4;
    }
    // F0 as S6's write places it
    A.f0 = r.bpl.cycle > 0 ? m.field_of(r.bpl.cycle) : 0;
    const int rb = d.rb();
    auto LT = [&](int k, int v) { return m.t_of(A.f0 + k, v, 0); };   // start of line v of field Fk (v may be nlines)
    for (int f = 2; f <= 7; ++f) {
        const Pulse* p = first_pulse(f);
        A.t_fall[f] = m.t_of(A.f0 + p->field, p->line, p->h) + m.R / 2;
        A.t_rise[f] = A.t_fall[f] + int64_t(kPulseCck) * m.R;
    }
    // windows (SPECIFICATION.md 2.7)
    auto w_blank = [&](int k) { return Win{LT(k, 1), LT(k, 20)}; };
    auto w_live = [&](int k) {
        Win w{LT(k, rb), LT(k, m.nlines - 1)};
        if (k >= 2 && k <= 7) w.t1 = std::min(w.t1, A.t_fall[k]);
        if (k == kLastField) w.t1 = std::min(w.t1, LT(k, 100));
        return w;
    };
    auto w_trig = [&](int k) { return Win{A.t_rise[k], LT(k + 1, 20)}; };
    auto idx_of = [&](const Win& w) { return rd_idx(S, w.t0, w.t1); };
    auto all_cur = [&](const std::vector<size_t>& ix) { for (size_t i : ix) if (!current(m, S[i])) return false; return true; };
    auto n_cur = [&](const std::vector<size_t>& ix) { size_t c = 0; for (size_t i : ix) if (current(m, S[i])) ++c; return c; };
    auto all_eq = [&](const std::vector<size_t>& ix) { for (size_t i : ix) if (S[i].P != S[ix[0]].P) return false; return !ix.empty(); };
    auto max_gap = [&](const std::vector<size_t>& ix) { int64_t g = 0; for (size_t i = 1; i < ix.size(); ++i) g = std::max(g, S[ix[i]].a - S[ix[i - 1]].a); return g; };

    // A4: the reads of F2 before its pulse equal the model, and every triggered field has at least
    // two such reads before its pulse
    {
        auto ix = idx_of(w_live(2));
        bool all = !ix.empty() && all_cur(ix);
        bool two = true;
        std::string det;
        for (int f = 2; f <= 7; ++f) {
            size_t c = n_cur(idx_of(w_live(f)));
            if (c < 2) two = false;
            det += " F" + num(f) + "=" + num(static_cast<long long>(c));
        }
        A.a4 = all && ix.size() >= 2 && two;
        A.a4_detail = "F2 reads before its pulse: " + num(static_cast<long long>(ix.size())) + ", all current=" + (all ? "yes" : "no") +
                      "; current reads before the pulse per triggered field:" + det;
    }
    // C4: density
    {
        bool ok = true;
        int64_t worst = 0;
        size_t min_blank = 1u << 30;
        for (int k = 1; k <= kLastField; ++k) {
            auto lv = idx_of(w_live(k));
            if (lv.empty()) ok = false;
            worst = std::max(worst, max_gap(lv));
        }
        for (int k : {2, 9, 10}) {
            auto bl = idx_of(w_blank(k));
            min_blank = std::min(min_blank, bl.size());
            worst = std::max(worst, max_gap(bl));
            if (bl.size() < 10) ok = false;
        }
        size_t min_after = 1u << 30;
        for (const auto& p : kBaseSchedule) {
            int64_t tr = m.t_of(A.f0 + p.field, p.line, p.h) + m.R / 2 + int64_t(kPulseCck) * m.R;
            size_t c = 0;
            for (auto it = std::lower_bound(S.begin(), S.end(), tr, [](const LS& x, int64_t v) { return x.a < v; });
                 it != S.end() && it->a < tr + 64 * m.R; ++it) ++c;
            min_after = std::min(min_after, c);
            if (c < 2) ok = false;
        }
        for (int f = 2; f <= 7; ++f) {
            auto tg = idx_of(w_trig(f));
            worst = std::max(worst, max_gap(tg));
            if (tg.size() < 2) ok = false;
        }
        bool reach = !S.empty() && S.back().b >= LT(kLastField, 100);
        if (worst > 32 * m.R) ok = false;
        if (!reach) ok = false;
        A.c4 = ok;
        A.c4_detail = "largest gap between consecutive reads " + num(worst / m.R) + " colour clocks (at most 32), fewest reads in a blanking window " +
                      num(static_cast<long long>(min_blank)) + " (at least 10), fewest reads within 64 colour clocks after a pulse " +
                      num(static_cast<long long>(min_after)) + " (at least 2), reads reach line 100 of F10: " + (reach ? "yes" : "no");
    }
    // C5: the pulses are as scheduled
    {
        bool ok = r.events.size() == 2 * o.schedule.size();
        std::string det = num(static_cast<long long>(r.events.size())) + " pin changes for " + num(static_cast<long long>(o.schedule.size())) + " pulses";
        for (size_t i = 0; ok && i + 1 < r.events.size(); i += 2) {
            const PinEvent& e0 = r.events[i];
            const PinEvent& e1 = r.events[i + 1];
            bool good = e0.on && !e1.on && e0.bit == kPin6 && e1.bit == kPin6 && e0.conn == o.x && e1.conn == o.x &&
                        e0.applied == e0.cycle && e1.applied == e1.cycle && e1.cycle - e0.cycle == int64_t(kPulseCck) * m.R &&
                        ((e0.readback & kPin6) != 0) && ((e1.readback & kPin6) == 0);
            if (!good) ok = false;
        }
        if (r.f0_sched != A.f0) { ok = false; det += "; F0 differs between the schedule and S6's write"; }
        for (const auto& e : r.events) if (e.cycle <= r.t_refend) ok = false;
        A.c5 = ok;
        A.c5_detail = det;
    }
    A.pre_ok = A.fixture && A.c0 && A.c1 && A.c2 && A.a1 && A.a3 && A.a4 && A.c4 && A.c5 && A.c6;

    // ---- the propositions (computed whether or not the preconditions hold; graded only when they do)
    // P_f: the first read after the rising edge of the declared first pulse
    for (int f = 2; f <= 7; ++f) {
        auto it = std::lower_bound(S.begin(), S.end(), A.t_rise[f], [](const LS& x, int64_t v) { return x.a < v; });
        if (it != S.end() && it->b < LT(f + 1, 20)) { A.have_P[f] = true; A.P[f] = it->P; }
    }
    // P2 (a), (c)
    {
        bool have = true;
        for (int f = 2; f <= 7; ++f) have = have && A.have_P[f];
        bool a = have;
        if (have) {
            for (int f = 2; f <= 7; ++f) {
                const Pulse* p = first_pulse(f);
                if (int(A.P[f] >> 8) != p->line) a = false;
            }
        }
        bool c = have;
        if (have) {
            auto H = [&](int f) { return int(A.P[f] & 0xFF); };
            auto V = [&](int f) { return int(A.P[f] >> 8); };
            c = (H(3) - H(2) == 1) && (H(4) - H(2) == 8) && (H(5) == H(2)) && (V(5) == V(2) + 1);
        }
        A.p2_a = a; A.p2_c = c; A.p2_ok = a && c;
        std::string det;
        for (int f = 2; f <= 7; ++f) {
            if (!A.have_P[f]) { det += " F" + num(f) + ": no read;"; continue; }
            const Pulse* p = first_pulse(f);
            det += " F" + num(f) + ": V=" + num(A.P[f] >> 8) + " (target " + num(p->line) + ") H=" + num(A.P[f] & 0xFF) +
                   " (target " + num(p->h) + ", offset " + num(int(A.P[f] & 0xFF) - p->h) + ");";
        }
        A.p_detail = det;
    }
    // P1: W_trig constant for F2..F7, W_blank constant for F2, F9, F10
    {
        bool ok = true;
        for (int f = 2; f <= 7; ++f) { auto ix = idx_of(w_trig(f)); if (!all_eq(ix)) ok = false; }
        for (int k : {2, 9, 10}) { auto ix = idx_of(w_blank(k)); if (!all_eq(ix)) ok = false; }
        A.p1 = ok;
    }
    // P1 (bit 3 clear): in the same windows every read equals the model value and consecutive reads differ
    {
        bool ok = true;
        auto chk = [&](const std::vector<size_t>& ix) {
            if (ix.size() < 2 || !all_cur(ix)) { ok = false; return; }
            for (size_t i = 1; i < ix.size(); ++i) if (S[ix[i]].P == S[ix[i - 1]].P) ok = false;
        };
        for (int f = 2; f <= 7; ++f) chk(idx_of(w_trig(f)));
        for (int k : {2, 9, 10}) chk(idx_of(w_blank(k)));
        A.p1b = ok;
    }
    // P3 (a): every read in W_trig(f) has P = P_f
    {
        bool ok = true;
        for (int f = 2; f <= 7; ++f) {
            auto ix = idx_of(w_trig(f));
            if (ix.empty() || !A.have_P[f]) { ok = false; continue; }
            for (size_t i : ix) if (S[i].P != A.P[f]) ok = false;
        }
        A.p3a = ok;
    }
    // P3 (b): F7 after the second pulse
    {
        const Pulse* p2 = nullptr;
        for (const auto& p : kBaseSchedule) if (p.field == 7) p2 = &p;       // the later of F7's two pulses
        int64_t tr = m.t_of(A.f0 + 7, p2->line, p2->h) + m.R / 2 + int64_t(kPulseCck) * m.R;
        auto ix = rd_idx(S, tr, LT(7 + 1, 20));
        bool ok = ix.size() >= 2 && A.have_P[7];
        for (size_t i : ix) if (S[i].P != A.P[7]) ok = false;
        A.p3b = ok;
    }
    // P3 (c): W_live(f+1) current and consecutive reads differ
    {
        bool ok = true;
        for (int f = 2; f <= 7; ++f) {
            auto ix = idx_of(w_live(f + 1));
            if (ix.size() < 2) { ok = false; continue; }
            if (!all_cur(ix)) ok = false;
            for (size_t i = 1; i < ix.size(); ++i) if (S[ix[i]].P == S[ix[i - 1]].P) ok = false;
        }
        A.p3c = ok;
    }
    // P3 (d): F9
    {
        auto bl = idx_of(w_blank(9));
        auto lv = idx_of(w_live(9));
        int64_t pulse_t = m.t_of(A.f0 + 9, 10, 0x40);
        size_t before = 0, after = 0;
        for (size_t i : bl) { if (S[i].b < pulse_t) ++before; else if (S[i].a > pulse_t) ++after; }
        A.p3d = all_eq(bl) && before >= 2 && after >= 2 && lv.size() >= 2 && all_cur(lv);
    }
    // P4
    {
        bool ok = true;
        for (int f = 2; f <= 7; ++f) {
            auto ix = idx_of(w_trig(f));
            if (ix.size() < 2 || !A.have_P[f]) { ok = false; continue; }
            for (size_t i = 1; i < ix.size(); ++i) if (S[ix[i]].P != S[ix[i - 1]].P) ok = false;
            if (A.P[f] >= 0x10500) ok = false;
            auto bl = idx_of(w_blank(f + 1));
            if (bl.empty()) ok = false;
            for (size_t i : bl) if (S[i].P >= 0x10500 || S[i].P != A.P[f]) ok = false;
        }
        for (int k : {1, 8}) {
            auto ix = idx_of(w_live(k));
            if (ix.size() < 2) ok = false;
            for (size_t i = 1; i < ix.size(); ++i) if (S[ix[i]].P == S[ix[i - 1]].P) ok = false;
        }
        for (int k : {1, 8, 9}) {
            auto bl = idx_of(w_blank(k + 1));
            if (!all_eq(bl)) ok = false;
            for (size_t i : bl) if (S[i].P <= 0x10500) ok = false;
        }
        A.p4 = ok;
    }
    return A;
}

// ---- grading --------------------------------------------------------------
const char* kind_name(int k) {
    switch (k) {
        case 0: return "all lines 227 colour clocks";
        case 1: return "lines alternating 227, 228 from a 227 line";
        case 2: return "lines alternating 228, 227 from a 228 line";
        case 3: return "all lines 228 colour clocks";
        default: return "unknown";
    }
}


std::string failed_pre(const LpAn& A) {
    std::string f;
    auto add = [&](bool ok, const char* n) { if (!ok) f += std::string(f.empty() ? "" : ", ") + n; };
    add(A.fixture, "LP-FIXTURE-MARK0");
    add(A.c0, "LP-C0-IDENTITY");
    add(A.c1, "LP-C1-DONE");
    add(A.c2, "LP-C2-STORED");
    add(A.a1, "LP-C3-A1-READS-CURRENT");
    add(A.a3, "LP-C3-A3-FIELD-LENGTH");
    add(A.a4, "LP-C3-A4-FIELD-ANCHOR");
    add(A.c4, "LP-C4-DENSITY");
    add(A.c5, "LP-C5-PULSES");
    add(A.c6, "LP-C6-LPENDIS-CLEAR");
    return f;
}

void grade_pre(Checks& c, const LpRun& r, const LpAn& A, bool show_a2) {
    const std::string& t = A.tag;
    const LpOpts& o = r.o;
    std::string m1;
    if (A.have_model) {
        m1 = t + " model: " + num(A.nlines) + " lines per field, " + kind_name(A.kind) + ", the vertical count trails the horizontal wrap by " +
             num(A.m.vlag) + " colour clock(s), anchor uncertainty u=" + num(A.u) + " colour clocks (" + num(static_cast<long long>(A.covering)) +
             " of " + num(static_cast<long long>(A.usable)) + " usable reference reads bound it), F0=" + num(A.f0) + ", largest H read " +
             num(static_cast<long long>(A.maxH)) + ", reference field lengths in clk_sys:";
        for (size_t i = 1; i < r.frame_len.size(); ++i) m1 += " " + num(r.frame_len[i]);
    } else {
        m1 = t + " model: none could be fitted to the reference reads";
    }
    c.note(m1);

    std::string d_c1 = "reads=" + num(static_cast<long long>(r.samples.size())) + " of " + num(o.decl.exp_ref + o.decl.exp_meas) +
                       ", S2 samples=" + num(static_cast<long long>(r.n_s2)) + " of 3000, done mark " + (r.done ? "present" : "absent");
    c.expect(A.fixture, "LP-FIXTURE-MARK0 " + t + ": the program stores a known constant (MARK0=$5A5A) before anything is read back");
    c.expect(A.c0, "LP-C0-IDENTITY " + t + ": connector identity: bit 1 of exactly one of JOY0DAT and JOY1DAT follows pin 4 of the connector under test [p.329, p.281, p.327]");
    c.expect(A.c1, "LP-C1-DONE " + t + ": the program finished and the sample count is as built (" + d_c1 + ")");
    c.expect(A.c2, "LP-C2-STORED " + t + ": the port shows the CPU reading each configuration word with its declared value, in the order S3, S4, S6 (" + A.c2_detail + ")");
    if (!A.have_model) {
        c.expect(false, "LP-C3-A1-READS-CURRENT " + t + ": every reference-phase word read equals the model value at some instant inside its bracket (no model could be fitted) [p.216]");
        for (const char* id : {"LP-A2-LINE-LENGTHS", "LP-C3-A3-FIELD-LENGTH", "LP-C3-A4-FIELD-ANCHOR", "LP-C4-DENSITY", "LP-C5-PULSES"})
            c.record(std::string(id) + " " + t + ": NOT GRADED, no model could be fitted");
    } else {
        std::string d_a1 = num(static_cast<long long>(A.a1_bad)) + " of " + num(o.decl.exp_ref) + " reads have a word that matches no instant of its bracket";
        c.expect(A.a1, "LP-C3-A1-READS-CURRENT " + t + ": every reference-phase word read equals the model value at some instant inside its bracket (" + d_a1 + ") [p.216]");
        std::string d_a2 = std::string("measured: ") + kind_name(A.kind) + ", largest H read " + num(static_cast<long long>(A.maxH)) +
                           "; expected " + (o.decl.ntsc ? "alternating 227 and 228, H at most $E3" : "all 227, H at most $E2");
        if (show_a2) c.expect(A.a2, "LP-A2-LINE-LENGTHS " + t + ": line lengths are 227 and 228 alternating (NTSC) or all 227 (PAL), and H never exceeds $E3 (NTSC) or $E2 (PAL) (" + d_a2 + ") [p.23]");
        std::string d_a3 = num(A.nlines) + " lines per field in each of " + num(static_cast<long long>(r.frame_len.size()) - 1) + " reference fields";
        c.expect(A.a3, "LP-C3-A3-FIELD-LENGTH " + t + ": the field has 262 or 263 lines (NTSC) or 312 or 313 lines (PAL), the same in every reference field (" + d_a3 + ") [p.24]");
        c.expect(A.a4, "LP-C3-A4-FIELD-ANCHOR " + t + ": in F2 every read before the pulse, and in every triggered field at least two, equals the model value inside its bracket (" + A.a4_detail + ") [p.216]");
        c.expect(A.c4, "LP-C4-DENSITY " + t + ": the reads are dense enough in every graded window and reach line 100 of F10 (" + A.c4_detail + ")");
        c.expect(A.c5, "LP-C5-PULSES " + t + ": every pulse is on pin 6 of the connector under test, 4 colour clocks wide, at its scheduled cycle, and no other controller pin changes (" + A.c5_detail + ") [p.333]");
    }
    if (o.ecs) {
        c.expect(A.c6, "LP-C6-LPENDIS-CLEAR " + t + ": BEAMCON0 was written with bit 13 (LPENDIS) = 0 [p.305]");
    } else {
        c.record("LP-C6-LPENDIS-CLEAR " + t + ": not applicable under OCS, BEAMCON0 is not written (SPECIFICATION.md 2.8)");
    }
}

// Grades one configuration from its runs: the runs with BPLCON0 bit 3 set (both connectors in the
// normal run, one in a contrast) and the runs with it clear.
void grade_config(Checks& c, const std::vector<const LpRun*>& runs, const std::vector<LpAn>& An,
                  const std::vector<const LpRun*>& cruns, const std::vector<LpAn>& cAn, bool ecs, bool ntsc) {
    const std::string cfg = cfg_name(ecs, ntsc);
    for (size_t i = 0; i < runs.size(); ++i) grade_pre(c, *runs[i], An[i], true);
    for (size_t i = 0; i < cruns.size(); ++i) grade_pre(c, *cruns[i], cAn[i], false);
    // measured pen positions, whether or not the preconditions hold
    for (size_t i = 0; i < runs.size(); ++i) {
        if (!An[i].have_model) continue;
        c.note(An[i].tag + " P2 measured, first read after the rising edge of each triggered field's first pulse:" + An[i].p_detail);
        std::string pr = An[i].tag + " P1-P4 measured: P1=" + (An[i].p1 ? "holds" : "does not hold") + ", P2(a)=" + (An[i].p2_a ? "holds" : "does not hold") +
                         ", P2(c)=" + (An[i].p2_c ? "holds" : "does not hold") + ", P3(a)=" + (An[i].p3a ? "holds" : "does not hold") +
                         ", P3(b)=" + (An[i].p3b ? "holds" : "does not hold") + ", P3(c)=" + (An[i].p3c ? "holds" : "does not hold") +
                         ", P3(d)=" + (An[i].p3d ? "holds" : "does not hold") + ", P4=" + (An[i].p4 ? "holds" : "does not hold");
        c.note(pr);
    }
    for (size_t i = 0; i < cruns.size(); ++i) {
        if (!cAn[i].have_model) continue;
        c.note(cAn[i].tag + " P1 (bit 3 clear) measured: every read equals the model value and consecutive reads differ in the windows: " +
               (cAn[i].p1b ? "holds" : "does not hold"));
    }
    int u = 0;
    for (const auto& A : An) if (A.have_model) u = std::max(u, A.u);
    std::string hnote = (u > 2) ? "NOT GRADED, its precondition does not hold: the anchor uncertainty is " + num(u) +
                                  " colour clocks, the check requires at most 2 (the port shows no custom-register access, so each read is bracketed by the surrounding memory accesses)"
                                : "NOT GRADED, not built for an anchor uncertainty of " + num(u) + " colour clocks";
    c.record("LP-P2-H-ABSOLUTE " + cfg + ": the absolute horizontal position of the latch is " + hnote);

    std::vector<size_t> ok_runs;
    for (size_t i = 0; i < runs.size(); ++i) if (An[i].pre_ok) ok_runs.push_back(i);
    std::vector<size_t> ok_cruns;
    for (size_t i = 0; i < cruns.size(); ++i) if (cAn[i].pre_ok) ok_cruns.push_back(i);
    // P1, bit 3 clear: every connector's run
    bool p1b_all = true;
    for (size_t i = 0; i < cruns.size(); ++i) {
        if (cAn[i].pre_ok) {
            c.expect(cAn[i].p1b, "LP-P1B-NO-LATCH-WHEN-CLEAR " + cAn[i].tag + ": with BPLCON0 bit 3 clear the pulses of the schedule leave VPOSR:VHPOSR reading the moving beam: every read in W_trig of F2 to F7 and in W_blank of F2, F9 and F10 equals the model value and consecutive reads differ [p.239, p.216]");
            if (!cAn[i].p1b) p1b_all = false;
        } else {
            c.record("LP-P1B-NO-LATCH-WHEN-CLEAR " + cAn[i].tag + ": NOT GRADED, precondition(s) failed: " + failed_pre(cAn[i]));
        }
    }
    const bool p1_graded = !cruns.empty() && ok_cruns.size() == cruns.size();
    if (ok_runs.empty()) {
        for (size_t i = 0; i < runs.size(); ++i) {
            const std::string fp = failed_pre(An[i]);
            for (const char* id : {"LP-P1A-LATCH-HOLDS", "LP-P2-LATCH-AT-PULSE", "LP-P3A-HOLD", "LP-P3B-HELD-PULSE-IGNORED", "LP-P3C-RELEASE",
                                   "LP-P3D-PULSE-BEFORE-ENABLE", "LP-P4-DETERMINE-TRIGGER", "LP-P1-LATCH-ENABLE", "LP-LATCH-ENABLE-AND-POSITION"})
                c.record(std::string(id) + " " + An[i].tag + ": NOT GRADED, precondition(s) failed: " + fp);
        }
        return;
    }
    // P2(d): in at least one run (a), (c) hold in every triggered field
    bool d = false;
    std::string who;
    for (size_t i : ok_runs) {
        if (An[i].p2_ok) { d = true; who += std::string(who.empty() ? "" : ", ") + An[i].tag; }
    }
    std::string d_p2 = d ? "latching on: " + who : "no connector latched";
    c.expect(d, "LP-P2-LATCH-AT-PULSE " + cfg + ": a pulse on pin 6 of a controller connector latches the beam position of its instant: the vertical position is the pulse line and the horizontal offsets follow the pulse positions exactly, in every triggered field of at least one connector (" + d_p2 +
                ") [p.333, p.238] opus:E-lightpen.c12 sol:E-lightpen.c10 sol:E-lightpen.c21");
    // the light-pen connector(s): where P2 passes; if none, every run whose preconditions hold
    std::vector<size_t> pen;
    for (size_t i : ok_runs) if (An[i].p2_ok) pen.push_back(i);
    if (pen.empty()) pen = ok_runs;
    bool p1_all = true;
    for (size_t i : pen) {
        const LpAn& A = An[i];
        const std::string& t = A.tag;
        c.expect(A.p1, "LP-P1A-LATCH-HOLDS " + t + ": with BPLCON0 bit 3 set, a pulse freezes VPOSR:VHPOSR and so does the end of a field without one: every read in W_trig of F2 to F7 and in W_blank of F2, F9 and F10 has one position [p.333, p.239]");
        if (!A.p1) p1_all = false;
        c.expect(A.p3a, "LP-P3A-HOLD " + t + ": after a trigger every read until the end of the following vertical blanking has the latched position [p.333, p.239] opus:E-lightpen.c5 sol:E-lightpen.c4");
        c.expect(A.p3b, "LP-P3B-HELD-PULSE-IGNORED " + t + ": a second pulse while the position is held changes nothing (F7, line 180) [p.239]");
        if (A.p3a)
            c.expect(A.p3c, "LP-P3C-RELEASE " + t + ": at the end of internal vertical blanking the registers read the moving beam again [p.239, p.216]");
        else
            c.record("LP-P3C-RELEASE " + t + ": NOT GRADED, no position was held in the preceding fields (LP-P3A-HOLD does not hold), so there was nothing to release");
        c.expect(A.p3d, "LP-P3D-PULSE-BEFORE-ENABLE " + t + ": a pulse before the capture circuitry is enabled (F9, line 10) is not captured, because the position of the end of F8 is still held: every read in W_blank of F9 has one position and every read in W_live of F9 equals the model value [p.239: \"If no trigger is seen, the counters latch at the end of the display field\"; p.238]");
        c.expect(A.p4, "LP-P4-DETERMINE-TRIGGER " + t + ": the two determinations of chapter 8 tell a triggered field from an untriggered one: two long reads agree and the value is below $10500 after a trigger, they differ without one, and the value read during blanking is above $10500 [p.239]");
    }
    if (p1_graded)
        c.expect(p1_all && p1b_all, "LP-P1-LATCH-ENABLE " + cfg + ": bit 3 of BPLCON0 is the light pen enable: with it set a pulse freezes VPOSR:VHPOSR and so does the end of a field without one (LP-P1A-LATCH-HOLDS), with it clear the same pulses leave the registers reading the moving beam (LP-P1B-NO-LATCH-WHEN-CLEAR) [p.333, p.239] opus:E-lightpen.c21 sol:E-lightpen.c18");
    else
        c.record("LP-P1-LATCH-ENABLE " + cfg + ": NOT GRADED, the bit-clear half has no run whose preconditions hold");
    if (p1_graded)
        c.expect(d && p1_all && p1b_all, "LP-LATCH-ENABLE-AND-POSITION " + cfg + ": a trigger pulse is generated which can be enabled to latch the horizontal and vertical beam positions: LPEN enables the latch and the latch holds the position of the pulse [p.333] opus:E-lightpen.c3 sol:E-lightpen.c2");
    else
        c.record("LP-LATCH-ENABLE-AND-POSITION " + cfg + ": NOT GRADED, the bit-clear half has no run whose preconditions hold");
    for (size_t i = 0; i < runs.size(); ++i) {
        bool graded = false;
        for (size_t j : pen) if (j == i) graded = true;
        if (!graded)
            c.record("LP-P1-P4 " + An[i].tag + ": the propositions are graded on the light-pen connector, not on this run (" + (An[i].pre_ok ? std::string("its pulses did not latch while another connector's did") : "its preconditions failed: " + failed_pre(An[i])) + ")");
    }
}

LpOpts make_opts(bool ecs, bool ntsc, int x) {
    LpOpts o;
    o.ecs = ecs; o.ntsc = ntsc; o.x = x;
    o.prog = ecs ? "prog_lpen.bin" : "prog_lpen_ocs.bin";
    o.beamcon0 = ntsc ? 0x0000 : 0x0020;
    o.decl.ntsc = ntsc;
    o.decl.beamcon0 = o.beamcon0;
    return o;
}

// The run with BPLCON0 bit 3 clear: the same schedule, the stored and declared word $0000.
LpOpts make_clear_opts(bool ecs, bool ntsc, int x) {
    LpOpts o = make_opts(ecs, ntsc, x);
    o.bplcon0 = 0x0000;
    o.decl.bplcon0 = 0x0000;
    o.bit_clear = true;
    return o;
}

void run_lp_config(Checks& c, const std::string& dir, bool ecs, bool ntsc) {
    std::vector<LpRun> runs, cruns;
    std::vector<LpAn> an, can;
    runs.reserve(2);
    cruns.reserve(2);
    for (int x = 0; x < 2; ++x) {
        runs.push_back(lp_run(dir, make_opts(ecs, ntsc, x)));
        an.push_back(analyze_run(runs.back()));
    }
    for (int x = 0; x < 2; ++x) {
        cruns.push_back(lp_run(dir, make_clear_opts(ecs, ntsc, x)));
        can.push_back(analyze_run(cruns.back()));
    }
    std::vector<const LpRun*> rp, cp;
    for (const auto& r : runs) rp.push_back(&r);
    for (const auto& r : cruns) cp.push_back(&r);
    grade_config(c, rp, an, cp, can, ecs, ntsc);
}

// One contrasting run: ECS, PAL, the connector joystick1, and the change NMH_MUTATE names. The run with
// bit 3 clear (the other half of P1) is run beside it, unchanged except where the change names it.
bool run_lp_mutation(Checks& c, const std::string& dir, const std::string& mv) {
    LpOpts o = make_opts(true, false, 1);
    LpOpts oc = make_clear_opts(true, false, 1);
    auto drop = [&](int field, int line) {
        std::vector<Pulse> s;
        for (const auto& p : o.schedule) if (!(p.field == field && p.line == line)) s.push_back(p);
        o.schedule = s;
    };
    auto move_to = [&](int field, int line, int nline, int nh) {
        for (auto& p : o.schedule) if (p.field == field && p.line == line) { p.line = nline; p.h = nh; }
    };
    if (mv == "LP-BASE") { /* no change: the unchanged ECS/PAL run on joystick1 and its bit-clear run */ }
    else if (mv == "LP-FIX") o.prog = "prog_lpen_fix.bin";
    else if (mv == "LP-C0") o.c0_pin = kPin3;
    else if (mv == "LP-C1") o.prog = "prog_lpen_nodone.bin";
    else if (mv == "LP-C2") o.bplcon0 = 0x0000;
    else if (mv == "LP-A1") o.decl.R = kCck + 1;
    else if (mv == "LP-A1W") o.prog = "prog_lpen_v8.bin";
    else if (mv == "LP-STD") o.decl.ntsc = true;
    else if (mv == "LP-A3") { o = make_opts(true, true, 1); o.decl.ntsc = false; }
    else if (mv == "LP-A2") o.prog = "prog_lpen_hw.bin";
    else if (mv == "LP-A4") move_to(2, 100, 40, 0x40);
    else if (mv == "LP-A4W") o.prog = "prog_lpen_m8.bin";
    else if (mv == "LP-C4") { o.prog = "prog_lpen_slow.bin"; o.decl.exp_ref = 600; o.decl.exp_meas = 2000; }
    else if (mv == "LP-C5") o.pulse_pin = kPin9;
    else if (mv == "LP-C6") { o.beamcon0 = 0x2020; o.decl.beamcon0 = 0x2020; }
    else if (mv == "LP-P1") { o.bplcon0 = 0x0000; o.decl.bplcon0 = 0x0000; }
    else if (mv == "LP-P1B") { oc.bplcon0 = 0x0008; oc.decl.bplcon0 = 0x0008; }
    else if (mv == "LP-P2A") drop(2, 100);
    else if (mv == "LP-P2B") move_to(3, 100, 100, 0x43);
    else if (mv == "LP-P3B") drop(7, 60);
    else if (mv == "LP-P3C") o.schedule.push_back({8, 50, 0x40});
    else if (mv == "LP-P3D") move_to(9, 10, 50, 0x40);
    else if (mv == "LP-P4A") o.schedule.push_back({1, 100, 0x40});
    else return false;
    LpRun r = lp_run(dir, o);
    LpRun rc = lp_run(dir, oc);
    std::vector<LpAn> an, can;
    an.push_back(analyze_run(r));
    can.push_back(analyze_run(rc));
    std::vector<const LpRun*> rp, cp;
    rp.push_back(&r);
    cp.push_back(&rc);
    grade_config(c, rp, an, cp, can, o.ecs, o.ntsc);
    return true;
}

// Claims the package declines (SPECIFICATION.md section 0): named here, graded nowhere.
void record_declined(Checks& c) {
    c.record("LP-DECLINED external disk identification: opus:E-extdisk-id.c2 sol:E-disk-id.c1 opus:E-extdisk-id.c3 sol:E-disk-id.c2 opus:E-extdisk-id.c4 sol:E-disk-id.c3 "
             "opus:E-extdisk-id.c5 sol:E-disk-id.c4 opus:E-extdisk-id.c6 sol:E-disk-id.c5 opus:E-extdisk-id.c7 sol:E-disk-id.c6 sol:E-disk-id.c7 opus:E-extdisk-id.c9 sol:E-disk-id.c8 "
             "opus:E-extdisk-id.c10 sol:E-disk-id.c9 opus:E-extdisk-id.c13 sol:E-disk-id.c12 (p.335): the stream comes from an external device the test cannot attach, RDY* cannot be "
             "driven and MTRXD*, SELxB* cannot be seen at the ports, and the only device-free case has no agreed value (SPECIFICATION.md 0.1)");
    c.record("LP-DECLINED pins marked Unused: opus:E-lightpen.c7 opus:E-lightpen.c8 opus:E-lightpen.c9 opus:E-lightpen.c10 opus:E-lightpen.c15 sol:E-lightpen.c5 sol:E-lightpen.c6 "
             "sol:E-lightpen.c7 sol:E-lightpen.c8 sol:E-lightpen.c13 (p.333): \"Unused\" states no behaviour, and the same pins are the joystick switches and POTY (SPECIFICATION.md 0.2)");
    c.record("LP-DECLINED opus:E-lightpen.c24 (p.333 figure): the figure's \"PORT 0\" is not graded, the pages disagree about the port; the latch itself is graded in LP-P2-LATCH-AT-PULSE (SPECIFICATION.md 0.3)");
    c.record("LP-DECLINED opus:E-lightpen.c4 sol:E-lightpen.c3 (p.333): \"There is no hardware bit to indicate this trigger\" cannot be graded by a finite observation, so the claims are not "
             "credited to LP-P4-DETERMINE-TRIGGER, which grades only the second clause (SPECIFICATION.md 0.3)");
}

}  // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;
    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";
    auto finish = [&]() {
        std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
        return checks.failed == 0 ? 0 : 1;
    };

    // ==== Fixture, ahead of anything graded ==============================
    {
        nmh::Config cfg;
        nmh::NanomigHarness h(cfg);
        std::string bin = dir + (mv == "FIX-LOAD" ? "/prog_pot_missing.bin" : (mv == "FIX-MARK0" ? "/prog_pot_fix.bin" : "/prog_pot.bin"));
        bool loaded = h.load_program(bin) > 0;
        checks.expect(loaded, "FIXTURE-LOAD stimulus: prog_pot.bin loads at PROGRAM_ORG");
        if (loaded) {
            h.boot(kBootBudget);
            auto first = first_write(h, kMark0 * 2, kMark0Budget);
            checks.expect(first.has_value() && first->data == 0x5A5A,
                          "FIXTURE-MARK0 stimulus: the program stores a known constant (MARK0=$5A5A) before "
                          "anything is read back");
        } else {
            checks.record("FIXTURE-MARK0 stimulus: NOT GRADED, the program did not load");
        }
    }
    if (mv == "FIX-LOAD" || mv == "FIX-MARK0") return finish();

    if (mv.rfind("LP-", 0) == 0) {
        if (!run_lp_mutation(checks, dir, mv)) { std::printf("unknown NMH_MUTATE value %s\n", mv.c_str()); return 2; }
        return finish();
    }

    // ==== P5: one run per configuration; the connector it grades comes from C0 ==
    if (!mv.empty()) {
        P5Opts o;
        if (mv == "C0") o.id_pin = kPinRight;
        else if (mv == "C1") o.bin = "prog_pot_nodone.bin";
        else if (mv == "P5-PIN9") o.pin9 = true;
        else if (mv == "P5-POTGO") o.cfg_potgo = 0x0001;
        else if (mv == "P5-SHORT") o.shortw = true;
        else if (mv == "P5-SCHED") o.other = true;
        else { std::printf("unknown NMH_MUTATE value %s\n", mv.c_str()); return 2; }
        P5Run r = run_p5(dir, o);
        grade_p5(checks, o.ecs, o.ntsc, r);
        return finish();
    }
    for (bool ntsc : {false, true})
        for (bool ecs : {true, false}) {
            P5Opts o;
            o.ecs = ecs; o.ntsc = ntsc;
            P5Run r = run_p5(dir, o);
            grade_p5(checks, ecs, ntsc, r);
        }

    // ==== the light pen: P1-P4 and the preconditions C0-C6, every configuration ==
    record_declined(checks);
    for (bool ntsc : {false, true})
        for (bool ecs : {true, false})
            run_lp_config(checks, dir, ecs, ntsc);

    return finish();
}
