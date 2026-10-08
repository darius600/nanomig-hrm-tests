// test_appHJ_drive_pins.cpp -- appHJ-drive-pins. See SPECIFICATION.md for
// every assertion's proposition, program, observed value and expected
// value. This file only implements what SPECIFICATION.md already
// committed to, plus the stimulus mapping SPECIFICATION.md 3.4 and 5.x
// assign to the builder (which harness port drives which register, and
// which harness input drives which pot pin -- see the header comment
// blocks below each section).
//
// NMH_MUTATE selects one mutated case in place of its normal counterpart;
// none is exercised by a plain `make test`. Every mutation still runs the
// package's own comparator -- the same expect() label, unchanged -- through
// the same four-phase or four-pin run; only the one named clause changes,
// and the comparator is never inverted while a mutation is selected.
//
// MUTATION PJ1 stimulus red NMH_MUTATE=P1-WRONGPORT: P1's schedule is applied to joystick0 instead of joystick1, so the action record shows the other port's bits driven; also P1-P4 become not checkable; TG68K FAIL (PJ1; P1-P4 not checkable; 10 passed, 1 failed), fx68k FAIL (PJ1; P1-P4 not checkable; 10 passed, 1 failed).
// MUTATION PJ1 stimulus red NMH_MUTATE=P2-WRONGPORT: P2's schedule is applied to joystick0 instead of joystick1, so the action record shows the other port's bits driven; also P1-P4 become not checkable; TG68K FAIL (PJ1; P1-P4 not checkable; 10 passed, 1 failed), fx68k FAIL (PJ1; P1-P4 not checkable; 10 passed, 1 failed).
// MUTATION PJ1 stimulus red NMH_MUTATE=P3-WRONGPORT: P3's schedule is applied to joystick1 instead of joystick0, so the action record shows the other port's bits driven; also P1-P4 become not checkable; TG68K FAIL (PJ1; P1-P4 not checkable; 10 passed, 1 failed), fx68k FAIL (PJ1; P1-P4 not checkable; 10 passed, 1 failed).
// MUTATION PJ1 stimulus red NMH_MUTATE=P4-WRONGPORT: P4's schedule is applied to joystick1 instead of joystick0, so the action record shows the other port's bits driven; also P1-P4 become not checkable; TG68K FAIL (PJ1; P1-P4 not checkable; 10 passed, 1 failed), fx68k FAIL (PJ1; P1-P4 not checkable; 10 passed, 1 failed).
// MUTATION P1 stimulus red NMH_MUTATE=P1-WRONGREG: the program stores JOY1DAT and JOY0DAT in the other order for phase 0 (prog_mouse_sw0.bin), the action record and the other preconditions staying true, so the register that P1 reads holds the other port's count; TG68K FAIL (P1; 14 passed, 1 failed), fx68k FAIL (P1; 14 passed, 1 failed).
// MUTATION P2 stimulus red NMH_MUTATE=P2-WRONGREG: the program stores JOY1DAT and JOY0DAT in the other order for phase 1 (prog_mouse_sw1.bin), the action record and the other preconditions staying true, so the register that P2 reads holds the other port's count; TG68K FAIL (P2; 14 passed, 1 failed), fx68k FAIL (P2; 14 passed, 1 failed).
// MUTATION P3 stimulus red NMH_MUTATE=P3-WRONGREG: the program stores JOY1DAT and JOY0DAT in the other order for phase 2 (prog_mouse_sw2.bin), the action record and the other preconditions staying true, so the register that P3 reads holds the other port's count; TG68K FAIL (P3; 14 passed, 1 failed), fx68k FAIL (P3; 14 passed, 1 failed).
// MUTATION P4 stimulus red NMH_MUTATE=P4-WRONGREG: the program stores JOY1DAT and JOY0DAT in the other order for phase 3 (prog_mouse_sw3.bin), the action record and the other preconditions staying true, so the register that P4 reads holds the other port's count; TG68K FAIL (P4; 14 passed, 1 failed), fx68k FAIL (P4; 14 passed, 1 failed).
// MUTATION PJ1 stimulus red NMH_MUTATE=PJ1: one scheduled pin change (phase 0, step 5) is dropped while the grader keeps its plan; TG68K FAIL (PJ1; P1-P4 not checkable; 10 passed, 1 failed), fx68k FAIL (PJ1; P1-P4 not checkable; 10 passed, 1 failed).
// MUTATION PJ2 stimulus red NMH_MUTATE=PJ2: phase 0's step 5 hold is shortened to 2 lines, so the window holds fewer than 3 real stored samples; TG68K FAIL (PJ2; P1-P4 not checkable; 10 passed, 1 failed), fx68k FAIL (PJ2; P1-P4 not checkable; 10 passed, 1 failed).
// MUTATION PJ2 stimulus red NMH_MUTATE=PJ2-FRAME: the sampling loop leaves line 255 out (prog_mouse_pj2.bin), so one stored sample ends at VHPOSR bits 15-8 = $00 with V8 going from 0 to 1 and no frame start; TG68K FAIL (PJ2; P1-P4 not checkable; 10 passed, 1 failed), fx68k FAIL (PJ2; P1-P4 not checkable; 10 passed, 1 failed).
// MUTATION PJ3 stimulus red NMH_MUTATE=PJ3: the program writes JOYTEST ($0404) after sample 8, inside phase 0's baseline window (prog_mouse_pj3.bin), so the baseline samples of JOY0DAT and JOY1DAT differ while the action record stays true; TG68K FAIL (PJ3; P1-P4 not checkable; 10 passed, 1 failed), fx68k FAIL (PJ3; P1-P4 not checkable; 10 passed, 1 failed).
// MUTATION PJ1 stimulus red NMH_MUTATE=PJ3-KBDMOUSE: a mouse count is injected through kbd_mouse_* during phase 0's baseline window, which is the mouse movement PJ1's idle clause forbids; TG68K FAIL (PJ1 and PJ3; P1-P4 not checkable; 9 passed, 2 failed), fx68k FAIL (PJ1 and PJ3; P1-P4 not checkable; 9 passed, 2 failed).
// MUTATION PP1 stimulus red NMH_MUTATE=P5-WRONGPIN: P5's button press is applied to mouse_buttons bit 1 instead of mouse_buttons bit 2, so the action record shows another source or bit driven; also P5, P6 and P8 become not checkable; TG68K FAIL (PP1; P5, P6, P8 not checkable; 11 passed, 1 failed), fx68k FAIL (PP1; P5, P6, P8 not checkable; 11 passed, 1 failed).
// MUTATION PP1 stimulus red NMH_MUTATE=P6-WRONGPIN: P6's button press is applied to mouse_buttons bit 2 instead of mouse_buttons bit 1, so the action record shows another source or bit driven; also P5, P6 and P8 become not checkable; TG68K FAIL (PP1; P5, P6, P8 not checkable; 11 passed, 1 failed), fx68k FAIL (PP1; P5, P6, P8 not checkable; 11 passed, 1 failed).
// MUTATION PP1 stimulus red NMH_MUTATE=P8-WRONGPIN: P8's button press is applied to joystick1 bit 5 instead of joystick0 bit 5, so the action record shows another source or bit driven; also P5, P6 and P8 become not checkable; TG68K FAIL (PP1; P5, P6, P8 not checkable; 11 passed, 1 failed), fx68k FAIL (PP1; P5, P6, P8 not checkable; 11 passed, 1 failed).
// MUTATION P5 stimulus red NMH_MUTATE=P5-WRONGREG: the program stores JOY0DAT where it reads POTGOR for the samples of P5's pin (prog_pot_p5.bin), the action record and the other preconditions staying true, so POTGOR's DAT bit of the pin is not what the comparator reads; TG68K FAIL (P5; 14 passed, 1 failed), fx68k FAIL (P5; 14 passed, 1 failed).
// MUTATION P6 stimulus red NMH_MUTATE=P6-WRONGREG: the program stores JOY0DAT where it reads POTGOR for the samples of P6's pin (prog_pot_p6.bin), the action record and the other preconditions staying true, so POTGOR's DAT bit of the pin is not what the comparator reads; TG68K FAIL (P6; 14 passed, 1 failed), fx68k FAIL (P6; 14 passed, 1 failed).
// MUTATION P8 stimulus red NMH_MUTATE=P8-WRONGREG: the program stores JOY0DAT where it reads POTGOR for the samples of P8's pin (prog_pot_p8.bin), the action record and the other preconditions staying true, so POTGOR's DAT bit of the pin is not what the comparator reads; TG68K FAIL (P8; 14 passed, 1 failed), fx68k FAIL (P8; 14 passed, 1 failed).
// MUTATION PP1 stimulus red NMH_MUTATE=PP1: the C2 button press is omitted, so PP1 reports it missing instead of grading C2; TG68K FAIL (PP1; P5, P6, P8 not checkable; 11 passed, 1 failed), fx68k FAIL (PP1; P5, P6, P8 not checkable; 11 passed, 1 failed).
// MUTATION PP2 stimulus red NMH_MUTATE=PP2: pin 5's C1 write sets POTGO bit 0 = 1 (prog_pot_pp2.bin); TG68K FAIL (PP2; P5, P6, P8 not checkable; 11 passed, 1 failed), fx68k FAIL (PP2; P5, P6, P8 not checkable; 11 passed, 1 failed).
// MUTATION PP3 stimulus red NMH_MUTATE=PP3: pin 5's C1 step is shortened to 6 lines (prog_pot_pp3.bin), leaving fewer than 3 real stored samples in its window; TG68K FAIL (PP3; P5, P6, P8 not checkable; 11 passed, 1 failed), fx68k FAIL (PP3; P5, P6, P8 not checkable; 11 passed, 1 failed).
// MUTATION FIXTURE none impossible: the section 4 known-value check only verifies that chip-memory writes and reads work at all, a plumbing precondition every other assertion of the package depends on and would fail through; not exercised, by a scope decision.
// MUTATION FIXTURE none impossible: the section 5 known-value check only verifies that chip-memory writes and reads work at all, as above; not exercised, by a scope decision.
// MUTATION FIXTURE none impossible: the section 6 known-value check only verifies that chip-memory writes and reads work at all, as above; not exercised, by a scope decision.
// MUTATION section none impossible: the section 4 DONE check only verifies that the program reaches DONE, a plumbing precondition every other assertion of the package depends on; not exercised, by a scope decision.
// MUTATION section none impossible: the section 5 DONE check only verifies that the program reaches DONE, as above; not exercised, by a scope decision.
// MUTATION section none impossible: the section 6 DONE check only verifies that the program reaches DONE, as above; not exercised, by a scope decision.
//
// Stimulus mapping established by this build (SPECIFICATION.md 3.4, 5.1
// assign it to the builder), from the harness's own ports, matched against
// Table 8-1 (p.228) and Figure 8-2 (p.230) -- see SPECIFICATION.md 8.1:
//   joystick1 drives JOY0DAT (the left port, P1/P2); joystick0 drives
//   JOY1DAT (the right port, P3/P4). Within either port, bit 3 = up
//   (pin 1), bit 2 = down (pin 2), bit 1 = left (pin 3), bit 0 = right
//   (pin 4), each bit active high at this boundary (1 = pressed).
//   POTGOR bit 8 (POT0X/DATLX) follows mouse_buttons bit 2 (third/middle
//   button); bit 10 (POT0Y/DATLY) follows mouse_buttons bit 1 (right
//   button) (joystick1 bit 5 presses it too; this build presses only the
//   mouse bit); bit 14 (POT1Y/DATRY) follows joystick0 bit 5; bit 12
//   (POT1X/DATRX) follows no harness input at all (P7, withdrawn: SPECIFICATION.md 5.1, 8.2).

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

using namespace nmh;

namespace {

constexpr uint32_t kResults      = 0x0C0000;
constexpr uint32_t kDoneAddr     = 0x0C0FFE;
constexpr uint16_t kDoneValue    = 0x600D;
constexpr uint32_t kFixtureAddr  = 0x0C0FFC;
constexpr uint16_t kFixtureValue = 0xBEEF;
constexpr uint32_t kBufBase      = 0x0C1000;
constexpr uint64_t kBootBudget   = 6'000'000;

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

std::string hex4(uint16_t v) { char b[8]; std::snprintf(b, sizeof b, "$%04X", unsigned(v)); return b; }
std::string hex2(uint8_t v)  { char b[8]; std::snprintf(b, sizeof b, "$%02X", unsigned(v)); return b; }

// =====================================================================
// Section 4 (P1-P4): mouse-quadrature run
// =====================================================================

// One schedule step: (a,b) are the connector-pin levels of SPECIFICATION.md
// section 4 (1=open/released, 0=pressed/closed); n is the net signed count
// of quadrature edges applied before this step's own window (E2).
struct MouseStep { int a, b, hold_lines, n; };

std::vector<MouseStep> build_mouse_schedule() {
    std::vector<MouseStep> s;
    s.push_back({1, 1, 16, 0});
    int fwd[4][2]  = {{0,1},{0,0},{1,0},{1,1}};
    int back[4][2] = {{1,0},{0,0},{0,1},{1,1}};
    int n = 0;
    for (int c = 0; c < 6; ++c)
        for (int k = 0; k < 4; ++k) { ++n; s.push_back({fwd[k][0], fwd[k][1], 8, n}); }
    for (int c = 0; c < 9; ++c)
        for (int k = 0; k < 4; ++k) { --n; s.push_back({back[k][0], back[k][1], 8, n}); }
    s.push_back({1, 1, 16, n});
    return s;
}

struct MousePhase { bool use_joystick1; int a_bit, b_bit; };

// P1..P4 in order: joystick1 carries the left port (P1,P2), joystick0 the
// right port (P3,P4) -- the mapping this build established (see header
// comment, SPECIFICATION.md 8.1).
MousePhase kMousePhases[4] = {
    {true,  2, 0},  // P1 M0H: pin a=2(down)->bit2, pin b=4(right)->bit0
    {true,  3, 1},  // P2 M0V: pin a=1(up)->bit3,   pin b=3(left)->bit1
    {false, 2, 0},  // P3 M1H
    {false, 3, 1},  // P4 M1V
};

// One stored sample: VHPOSR, JOY0DAT, JOY1DAT and VPOSR (bit 0 is V8).
struct Sample3 { std::optional<uint16_t> v, j0, j1, vp; };

std::vector<Sample3> extract_samples3(const std::vector<RamAccess>& log, uint32_t base_w, int count) {
    std::vector<Sample3> s(count);
    for (auto& a : log) {
        if (!a.is_write || a.word_addr < base_w) continue;
        uint32_t off = a.word_addr - base_w;
        uint32_t idx = off / 4;
        if (idx >= (uint32_t)count) continue;
        uint32_t sub = off % 4;
        if (sub == 0) s[idx].v = a.data; else if (sub == 1) s[idx].j0 = a.data;
        else if (sub == 2) s[idx].j1 = a.data; else s[idx].vp = a.data;
    }
    return s;
}

// One event of the harness's own action record: the sample index at which
// it was actually applied (from live simulation), the sample index the
// plan called for, and what it changed. This is the "logged record of the
// input ports" SPECIFICATION.md 4.5 (PJ1) and 5.5 (PP1) ask the
// preconditions to be checked from, not the driver's `skipped` flag alone.
// j0, j1 and mb are the values of joystick0, joystick1 and mouse_buttons read
// back from the port right after the change: the port and bits actually driven.
struct AppliedMouseEvent { size_t sample_index; size_t planned_index; int phase; size_t step; int a, b; bool skipped;
                           uint8_t j0, j1, mb; };

struct MouseRun {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<RamAccess> log;
    std::optional<uint16_t> fixture;
    std::vector<AppliedMouseEvent> applied;
    bool pj3_pulse_applied = false;
};

// Runs prog_mouse.bin, interactively applying the schedule of
// build_mouse_schedule() to all four phases as the sample buffer fills
// (SPECIFICATION.md 3.3: "the harness makes each input change by counting
// samples as they appear at the port"). `mutate` selects one of the
// MUTATION cases in the header comment; everything else is the baseline.
MouseRun run_mouse(const std::string& dir, const std::string& mutate) {
    MouseRun r;
    Config cfg;
    NanomigHarness h(cfg);
    std::string bin = "/prog_mouse.bin";
    if (mutate == "PJ2-FRAME") bin = "/prog_mouse_pj2.bin";
    if (mutate == "PJ3") bin = "/prog_mouse_pj3.bin";
    for (int i = 0; i < 4; ++i)
        if (mutate == std::string("P") + std::to_string(i + 1) + "-WRONGREG") bin = "/prog_mouse_sw" + std::to_string(i) + ".bin";
    r.loaded = h.load_program(dir + bin) > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;

    MousePhase phases[4];
    for (int i = 0; i < 4; ++i) phases[i] = kMousePhases[i];
    if (mutate == "P1-WRONGPORT") phases[0].use_joystick1 = !phases[0].use_joystick1;
    if (mutate == "P2-WRONGPORT") phases[1].use_joystick1 = !phases[1].use_joystick1;
    if (mutate == "P3-WRONGPORT") phases[2].use_joystick1 = !phases[2].use_joystick1;
    if (mutate == "P4-WRONGPORT") phases[3].use_joystick1 = !phases[3].use_joystick1;

    auto steps = build_mouse_schedule();
    std::vector<int> lines(steps.size());
    for (size_t i = 0; i < steps.size(); ++i) lines[i] = steps[i].hold_lines;
    if (mutate == "PJ2") lines[5] = 2; // phase 0 only, applied below

    struct Ev { size_t g; int phase; size_t step; };
    std::vector<Ev> events;
    for (int p = 0; p < 4; ++p) {
        size_t acc = (size_t)p * 512;
        for (size_t i = 0; i < steps.size(); ++i) {
            events.push_back({acc, p, i});
            acc += (p == 0) ? (size_t)lines[i] : (size_t)steps[i].hold_lines;
        }
    }
    int skip_ev = -1;
    if (mutate == "PJ1") {
        for (size_t i = 0; i < events.size(); ++i) if (events[i].phase == 0 && events[i].step == 5) { skip_ev = (int)i; break; }
    }
    // PJ3-KBDMOUSE: inject a mouse count pulse partway through phase 0's baseline
    // window (samples [2,15] are graded there).
    bool do_pj3 = (mutate == "PJ3-KBDMOUSE");
    bool pj3_done = false;

    const uint32_t buf_w0 = word_addr(kBufBase);
    size_t samples_seen = 0;
    size_t ev_idx = 0;
    size_t log_seen = 0;
    bool done_seen = false;
    const uint64_t kMaxCycles = 10'000'000;
    for (uint64_t c = 0; c < kMaxCycles && !done_seen; ++c) {
        h.run_cycles(1);
        auto& lg = h.ram_log();
        for (; log_seen < lg.size(); ++log_seen) {
            const auto& a = lg[log_seen];
            if (!a.is_write) continue;
            if (a.word_addr == word_addr(kDoneAddr) && a.data == kDoneValue) { done_seen = true; }
            if (a.word_addr == buf_w0 + 4 * samples_seen) {
                // sample `samples_seen` (0-indexed) has just started being
                // stored (its VHPOSR word). Apply any event whose threshold
                // is already reached using this PRE-increment count, so a
                // change never lands mid-store of the previous sample (that
                // sample's own store, VHPOSR then JOY0DAT then JOY1DAT, is
                // only 1-2 bus cycles from finishing) -- it takes effect no
                // earlier than the first sample of its own window, which
                // the 2-line guard already discards.
                while (ev_idx < events.size() && samples_seen >= events[ev_idx].g) {
                    auto& e = events[ev_idx];
                    auto& st = steps[e.step];
                    bool skipped = ((int)ev_idx == skip_ev);
                    if (!skipped) {
                        uint8_t bit_a = st.a ? 0 : 1;
                        uint8_t bit_b = st.b ? 0 : 1;
                        auto& ph = phases[e.phase];
                        uint8_t byte = uint8_t((bit_a << ph.a_bit) | (bit_b << ph.b_bit));
                        if (ph.use_joystick1) h.ports()->joystick1 = byte; else h.ports()->joystick0 = byte;
                    }
                    r.applied.push_back({samples_seen, e.g, e.phase, e.step, st.a, st.b, skipped,
                                         h.ports()->joystick0, h.ports()->joystick1, h.ports()->mouse_buttons});
                    ++ev_idx;
                }
                ++samples_seen;
                if (do_pj3 && !pj3_done && samples_seen == 8) { // mid-baseline of phase 0
                    h.ports()->kbd_mouse_type = 0; // x-count delta
                    h.ports()->kbd_mouse_data = 1;
                    h.ports()->kbd_mouse_level = !h.ports()->kbd_mouse_level;
                    r.pj3_pulse_applied = true;
                    pj3_done = true;
                } else if (do_pj3 && pj3_done && samples_seen == 9) {
                    h.ports()->kbd_mouse_data = 0;
                }
            }
        }
    }
    r.log = h.ram_log();
    r.done = done_seen ? std::optional<uint64_t>(h.cycles()) : std::nullopt;
    r.fixture = h.peek_logical(kFixtureAddr);
    return r;
}

// =====================================================================
// Section 5 (P5-P8): pot-pin run
// =====================================================================

struct PotStep { uint16_t potgo; bool is_write; bool press; bool release; };

// C1..C8 (SPECIFICATION.md 5.2). is_write marks a POTGO-write step (driver
// does nothing); press/release mark a harness button-change step.
std::vector<PotStep> build_pot_steps(uint16_t c1, uint16_t c4, uint16_t c6, uint16_t c8) {
    return {
        {c1, true,  false, false},  // C1
        {0,  false, true,  false},  // C2 press
        {0,  false, false, true},   // C3 release
        {c4, true,  false, false},  // C4
        {0,  false, true,  false},  // C5 press
        {c6, true,  false, false},  // C6 (button stays pressed)
        {0,  false, false, true},   // C7 release
        {c8, true,  false, false},  // C8
    };
}

struct Sample2 { std::optional<uint16_t> v, pg; };

std::vector<Sample2> extract_samples2(const std::vector<RamAccess>& log, uint32_t base_w, int count) {
    std::vector<Sample2> s(count);
    for (auto& a : log) {
        if (!a.is_write || a.word_addr < base_w) continue;
        uint32_t off = a.word_addr - base_w;
        uint32_t idx = off / 2;
        if (idx >= (uint32_t)count) continue;
        if (off % 2 == 0) s[idx].v = a.data; else s[idx].pg = a.data;
    }
    return s;
}

// Which harness input presses each pin's button, and at which bit
// (SPECIFICATION.md 5.1; see header comment). kNone marks P7 (POT1X),
// which has no wired input at all (withdrawn: SPECIFICATION.md 5.1, 8.2).
enum class Src { kMouseButtons, kJoystick0, kJoystick1, kNone };
struct PotPin { Src src; int bit; int dat_bit; uint16_t c1, c4, c6, c8; };

PotPin kPotPins[4] = {
    {Src::kMouseButtons, 2, 8,  0xFF00, 0xFE00, 0xFC00, 0xFF00},  // P5 POT0X
    {Src::kMouseButtons, 1, 10, 0xFF00, 0xFB00, 0xF300, 0xFF00},  // P6 POT0Y
    {Src::kNone,         0, 12, 0xFF00, 0xEF00, 0xCF00, 0xFF00},  // P7 POT1X (withdrawn)
    {Src::kJoystick0,    5, 14, 0xFF00, 0xBF00, 0x3F00, 0xFF00},  // P8 POT1Y
};

// src and bit are the source and bit the change was actually applied to;
// mb, j0 and j1 are mouse_buttons, joystick0 and joystick1 read back from the
// port right after it (the register the change did not touch must read 0).
struct AppliedPotEvent { size_t sample_index; size_t planned_index; int pin; size_t step; bool press; bool skipped;
                         Src src; int bit; uint8_t mb, j0, j1; };

struct PotRun {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<RamAccess> log;
    std::optional<uint16_t> fixture;
    std::vector<AppliedPotEvent> applied;
    std::vector<uint16_t> markers;       // RESULTS writes, in order
    std::vector<size_t> marker_sample;   // sample index seen at each marker write
};

PotRun run_pot(const std::string& dir, const std::string& mutate) {
    PotRun r;
    std::string bin = dir + "/prog_pot.bin";
    if (mutate == "PP2") bin = dir + "/prog_pot_pp2.bin";
    if (mutate == "PP3") bin = dir + "/prog_pot_pp3.bin";
    if (mutate == "P5-WRONGREG") bin = dir + "/prog_pot_p5.bin";
    if (mutate == "P6-WRONGREG") bin = dir + "/prog_pot_p6.bin";
    if (mutate == "P8-WRONGREG") bin = dir + "/prog_pot_p8.bin";
    Config cfg;
    NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;

    PotPin pins[4];
    for (int i = 0; i < 4; ++i) pins[i] = kPotPins[i];
    if (mutate == "P5-WRONGPIN") { pins[0].src = Src::kMouseButtons; pins[0].bit = 1; } // P6's own input
    if (mutate == "P6-WRONGPIN") { pins[1].src = Src::kMouseButtons; pins[1].bit = 2; } // P5's own input
    if (mutate == "P8-WRONGPIN") { pins[3].src = Src::kJoystick1;    pins[3].bit = 5; } // wrong port

    int c1_lines = (mutate == "PP3") ? 6 : 24;

    struct Ev { size_t g; int pin; size_t step; };
    std::vector<Ev> events;
    size_t acc = 0;
    for (int p = 0; p < 4; ++p) {
        auto steps = build_pot_steps(pins[p].c1, pins[p].c4, pins[p].c6, pins[p].c8);
        for (size_t i = 0; i < steps.size(); ++i) {
            events.push_back({acc, p, i});
            int hold = (p == 0 && i == 0) ? c1_lines : 24;
            acc += (size_t)hold;
        }
    }
    int skip_ev = -1;
    if (mutate == "PP1") {
        for (size_t i = 0; i < events.size(); ++i) if (events[i].pin == 0 && events[i].step == 1) { skip_ev = (int)i; break; }
    }

    const uint32_t buf_w0 = word_addr(kBufBase);
    const uint32_t results_w = word_addr(kResults);
    size_t samples_seen = 0;
    size_t ev_idx = 0;
    size_t log_seen = 0;
    bool done_seen = false;
    const uint64_t kMaxCycles = 6'000'000;
    for (uint64_t c = 0; c < kMaxCycles && !done_seen; ++c) {
        h.run_cycles(1);
        auto& lg = h.ram_log();
        for (; log_seen < lg.size(); ++log_seen) {
            const auto& a = lg[log_seen];
            if (!a.is_write) continue;
            if (a.word_addr == word_addr(kDoneAddr) && a.data == kDoneValue) done_seen = true;
            if (a.word_addr == results_w) { r.markers.push_back(a.data); r.marker_sample.push_back(samples_seen); }
            if (a.word_addr == buf_w0 + 2 * samples_seen) {
                // See run_mouse() for why this uses the pre-increment count.
                while (ev_idx < events.size() && samples_seen >= events[ev_idx].g) {
                    auto& e = events[ev_idx];
                    bool press = (e.step == 1 || e.step == 4);
                    bool release = (e.step == 2 || e.step == 6);
                    bool skipped = ((int)ev_idx == skip_ev);
                    if ((press || release) && !skipped) {
                        auto& pin = pins[e.pin];
                        bool set_pressed = press;
                        switch (pin.src) {
                            case Src::kMouseButtons: {
                                uint8_t v = h.ports()->mouse_buttons;
                                if (set_pressed) v |= (1u << pin.bit); else v &= ~(1u << pin.bit);
                                h.ports()->mouse_buttons = v;
                                break;
                            }
                            case Src::kJoystick0: {
                                uint8_t v = h.ports()->joystick0;
                                if (set_pressed) v |= (1u << pin.bit); else v &= ~(1u << pin.bit);
                                h.ports()->joystick0 = v;
                                break;
                            }
                            case Src::kJoystick1: {
                                uint8_t v = h.ports()->joystick1;
                                if (set_pressed) v |= (1u << pin.bit); else v &= ~(1u << pin.bit);
                                h.ports()->joystick1 = v;
                                break;
                            }
                            case Src::kNone: break;
                        }
                    }
                    r.applied.push_back({samples_seen, e.g, e.pin, e.step, press, skipped,
                                         pins[e.pin].src, pins[e.pin].bit,
                                         h.ports()->mouse_buttons, h.ports()->joystick0, h.ports()->joystick1});
                    ++ev_idx;
                }
                ++samples_seen;
            }
        }
    }
    r.log = h.ram_log();
    r.done = done_seen ? std::optional<uint64_t>(h.cycles()) : std::nullopt;
    r.fixture = h.peek_logical(kFixtureAddr);
    return r;
}

// =====================================================================
// Section 6 (record only): drive light
// =====================================================================

struct DriveLightRun {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<RamAccess> log;
    std::optional<uint16_t> fixture;
    std::vector<uint8_t> led;
    uint64_t origin = 0;
    std::optional<uint8_t> led_at(uint64_t cyc) const {
        if (cyc < origin) return std::nullopt;
        uint64_t i = cyc - origin;
        return i < led.size() ? std::optional<uint8_t>(led[i]) : std::nullopt;
    }
};

DriveLightRun run_drivelight(const std::string& dir) {
    DriveLightRun r;
    Config cfg; // default (floppy_config=0): nanomig_harness.h's own Config
                // comment documents this as already giving drive 0, so no
                // override is used (SPECIFICATION.md 3.6 leaves the value
                // to the builder).
    NanomigHarness h(cfg);
    r.loaded = h.load_program(dir + "/prog_drivelight.bin") > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;
    r.origin = h.cycles();
    const uint64_t kMaxCycles = 4'000'000;
    r.led.reserve(kMaxCycles);
    size_t seen = h.ram_log().size();
    bool done_seen = false;
    for (uint64_t i = 0; i < kMaxCycles && !done_seen; ++i) {
        h.run_cycles(1);
        r.led.push_back(h.ports()->fdd_led);
        auto& lg = h.ram_log();
        for (; seen < lg.size(); ++seen)
            if (lg[seen].is_write && lg[seen].word_addr == word_addr(kDoneAddr) && lg[seen].data == kDoneValue) {
                r.done = lg[seen].cycle; done_seen = true;
            }
    }
    r.log = h.ram_log();
    r.fixture = h.peek_logical(kFixtureAddr);
    return r;
}

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    // ---- claims that give no proposition (SPECIFICATION.md section 1) ----
    if (mv.empty()) {
        checks.record("opus:AHJ-118, sol:H-092 [SPECIFICATION.md 1.1, p.371]: declined -- no expected value "
                       "exists for this system: the device I.D. belongs to the attached drive, and no page "
                       "states this system's drive type");
        checks.record("opus:AHJ-161, sol:J-030 [SPECIFICATION.md 1.2, p.379]: declined -- a pin's direction "
                       "cannot be seen at this boundary, and no harness setting corresponds to the Original "
                       "Agnus specifically");
        checks.record("opus:AHJ-326 [SPECIFICATION.md 1.3, p.381]: declined -- an internal output pin's "
                       "direction has no observable effect at this boundary: the test sees no chip pin, and no "
                       "page names a register or output effect of Agnus driving LP*");
        checks.record("opus:AHJ-300, sol:J-166 [SPECIFICATION.md 1.4, p.381]: declined -- the pin is internal "
                       "to the system, between chips, with no visible effect at this boundary");
        checks.record("opus:AHJ-090, sol:H-064 [SPECIFICATION.md 1.5, p.370]: declined -- the test sees no pin "
                       "of the disk connector, and \"approximately\" carries no tolerance");
        checks.record("opus:AHJ-085, sol:H-060 [SPECIFICATION.md 1.6, p.370]: declined -- the flip-flop "
                       "described is the external drive's own, and the only drive this system models is drive "
                       "0, not an external drive");
    }

    // ---- section 4: mouse-quadrature run ---------------------------------
    MouseRun mr = run_mouse(dir, mv);
    bool m_fixture_ok = mr.loaded && mr.fixture && *mr.fixture == kFixtureValue;
    if (mv.empty())
        checks.expect(m_fixture_ok, "FIXTURE (section 4): the program can deliver a known stored constant before any other result is graded");
    else
        checks.note(std::string("FIXTURE (section 4) under NMH_MUTATE=") + mv + ": " + (m_fixture_ok ? "ok" : "FAILED"));

    checks.expect(mr.loaded && mr.released && mr.done.has_value(),
                  "section 4 fixture: prog_mouse reaches DONE (loaded=" + std::to_string(mr.loaded) +
                  " released=" + std::to_string(mr.released) + " done=" + std::to_string(mr.done.has_value()) + ")");

    auto steps = build_mouse_schedule();
    auto samples3 = extract_samples3(mr.log, word_addr(kBufBase), 2048);

    // PJ1 [SPECIFICATION.md 4.5]: from the driver's own action record (the
    // logged input-port writes, each with the sample index it landed at) --
    // every scheduled change was actually applied, at its planned sample
    // index, and no other channel (the mouse-count injection) was touched.
    bool pj1_missing_ok = true;
    for (auto& e : mr.applied) if (e.skipped) pj1_missing_ok = false;
    bool pj1_timing_ok = true;
    for (auto& e : mr.applied) if (!e.skipped && e.sample_index != e.planned_index) pj1_timing_ok = false;
    // The port and bits actually driven, against the plan's own mapping
    // (kMousePhases): the planned port carries exactly the scheduled pin
    // levels on the proposition's two pins, and the other joystick port and
    // the mouse buttons are idle.
    bool pj1_pins_ok = true;
    for (auto& e : mr.applied) {
        if (e.skipped) continue;
        const MousePhase& ph = kMousePhases[e.phase];
        const MouseStep& st = steps[e.step];
        uint8_t want = uint8_t(((st.a ? 0 : 1) << ph.a_bit) | ((st.b ? 0 : 1) << ph.b_bit));
        uint8_t want_j1 = ph.use_joystick1 ? want : 0, want_j0 = ph.use_joystick1 ? 0 : want;
        if (e.j0 != want_j0 || e.j1 != want_j1 || e.mb != 0) pj1_pins_ok = false;
    }
    bool pj1_idle_ok = !mr.pj3_pulse_applied;
    bool pj1_ok = pj1_missing_ok && pj1_timing_ok && pj1_pins_ok && pj1_idle_ok;
    checks.expect(pj1_ok, "PJ1 [SPECIFICATION.md 4.5]: the driver's own action record shows every scheduled "
                           "change applied, at its scheduled sample index, on the proposition's two pins only, "
                           "with no other controller-port input, mouse movement or mouse-button activity");

    // Per-phase grading. Step windows are bounded by consecutive entries of
    // the driver's own action record (mr.applied's sample_index, taken from
    // live simulation), never by the test's own hold-line constants, so that
    // PJ2 and PP3 assert the window population from the stored samples.
    struct PhaseResult { bool e1_ok = true, e2_ok = true, e3_ok = true; bool pj3_ok = true; bool population_ok = true;
                          uint8_t c0 = 0; int graded_windows = 0; };
    PhaseResult pr[4];
    const char* names[4] = {"P1", "P2", "P3", "P4"};
    const char* pages[4] = {"p.380", "p.380", "p.380", "p.380"}; // Appendix J pin table (SPECIFICATION.md 4.1); p.281 supplies the routing, section 3.4

    for (int p = 0; p < 4; ++p) {
        bool have_c0 = false;
        bool have_other_base = false;
        uint8_t base_other_byte = 0;
        uint16_t base_other_reg = 0;
        std::optional<uint16_t> base_j0, base_j1;
        for (size_t i = 0; i < steps.size(); ++i) {
            size_t ai = (size_t)p * steps.size() + i;
            size_t start = mr.applied[ai].sample_index;
            size_t end;
            if (i + 1 < steps.size()) end = mr.applied[ai + 1].sample_index;
            else if (p + 1 < 4) end = mr.applied[(size_t)(p + 1) * steps.size() + 0].sample_index;
            else end = samples3.size();

            if (end < start + 3) { pr[p].population_ok = false; continue; } // 2-line guard leaves < 1 sample
            size_t win_lo = start + 2, win_hi = end - 1;
            int real_count = 0;
            for (size_t idx = win_lo; idx <= win_hi && idx < samples3.size(); ++idx)
                if (samples3[idx].j0 && samples3[idx].j1) ++real_count;
            if (real_count < 3) { pr[p].population_ok = false; continue; }
            ++pr[p].graded_windows;

            for (size_t idx = win_lo; idx <= win_hi && idx < samples3.size(); ++idx) {
                auto& s = samples3[idx];
                if (!s.j0 || !s.j1) { pr[p].e1_ok = pr[p].e2_ok = false; continue; }
                uint16_t reg = (p < 2) ? *s.j0 : *s.j1;
                // p%2==0 -> lo byte (P1,P3); p%2==1 -> hi byte (P2,P4)
                uint8_t byte = (p % 2 == 1) ? uint8_t(reg >> 8) : uint8_t(reg & 0xFF);
                uint8_t hi = (byte >> 1) & 1, lo = byte & 1;
                uint8_t exp_hi = steps[i].b ? 0 : 1; // hi = NOT b
                uint8_t exp_lo = uint8_t(exp_hi ^ (steps[i].a ? 0 : 1)); // lo = hi XOR NOT a
                if (hi != exp_hi || lo != exp_lo) pr[p].e1_ok = false;

                if (i == 0) { // baseline window establishes c0
                    if (!have_c0) { pr[p].c0 = byte; have_c0 = true; }
                } else {
                    uint8_t expected_count = uint8_t(pr[p].c0 + steps[i].n);
                    if (byte != expected_count) pr[p].e2_ok = false;
                }
                // E3: independence -- the other byte of the same register,
                // and both bytes of the other port's register.
                uint16_t other_same = (p < 2) ? *s.j0 : *s.j1;
                uint8_t other_byte = (p % 2 == 0) ? uint8_t(other_same >> 8) : uint8_t(other_same & 0xFF);
                uint16_t other_reg = (p < 2) ? *s.j1 : *s.j0;
                if (!have_other_base) { base_other_byte = other_byte; base_other_reg = other_reg; have_other_base = true; }
                else if (other_byte != base_other_byte || other_reg != base_other_reg) pr[p].e3_ok = false;

                // PJ3 [SPECIFICATION.md 4.5]: in the baseline window, both
                // registers (not just the byte under test) stay identical.
                if (i == 0) {
                    if (!base_j0) { base_j0 = s.j0; base_j1 = s.j1; }
                    else if (*s.j0 != *base_j0 || *s.j1 != *base_j1) pr[p].pj3_ok = false;
                }
            }
        }
    }

    bool pj3_ok = pr[0].pj3_ok && pr[1].pj3_ok && pr[2].pj3_ok && pr[3].pj3_ok;
    checks.expect(pj3_ok, "PJ3 [SPECIFICATION.md 4.5]: in the baseline window of every phase, all samples of "
                           "JOY0DAT and JOY1DAT are identical");

    bool pj2_population_ok = pr[0].population_ok && pr[1].population_ok && pr[2].population_ok && pr[3].population_ok;
    bool pj2_continuity_ok = true;
    for (size_t idx = 1; idx < samples3.size(); ++idx) {
        if (!samples3[idx - 1].v || !samples3[idx].v) continue;
        uint8_t prev_hi = uint8_t(*samples3[idx - 1].v >> 8);
        uint8_t cur_hi  = uint8_t(*samples3[idx].v >> 8);
        if (cur_hi == uint8_t(prev_hi + 1)) continue;
        // The return to $00 is allowed only at the start of a frame: V8
        // (VPOSR bit 0) was 1 in the previous sample and is 0 in this one.
        bool frame_start = cur_hi == 0 && samples3[idx - 1].vp && samples3[idx].vp &&
                           (*samples3[idx - 1].vp & 1) == 1 && (*samples3[idx].vp & 1) == 0;
        if (!frame_start) pj2_continuity_ok = false;
    }
    bool pj2_ok = pj2_population_ok && pj2_continuity_ok;
    checks.expect(pj2_ok, "PJ2 [SPECIFICATION.md 4.5]: every hold window contains at least 3 graded samples, "
                           "and VHPOSR bits 15-8 of consecutive stored samples differ by +1 modulo 256 or "
                           "return to $00 at a frame start, where V8 "
                           "(VPOSR bit 0) goes from 1 to 0");

    // P1-P4 [spec 3.7]: gated on section 4's preconditions -- a failed
    // precondition makes the propositions "not checkable", never FAIL.
    bool section4_pre_ok = pj1_ok && pj2_ok && pj3_ok;
    for (int p = 0; p < 4; ++p) {
        std::string label = std::string(names[p]) + " [" + pages[p] + ": opus:AHJ-" +
            (p==0?"178, sol:J-046":p==1?"206, sol:J-074":p==2?"176, sol:J-044":"208, sol:J-076") +
            "]: Denise's " + (p==0?"M0H":p==1?"M0V":p==2?"M1H":"M1V") +
            " is an input; the connector pins determine the state bits and the count of " +
            (p<2?"JOY0DAT":"JOY1DAT") + "'s " + ((p%2==0)?"horizontal":"vertical") + " counter";
        if (section4_pre_ok) {
            bool prop_ok = pr[p].e1_ok && pr[p].e2_ok && pr[p].e3_ok && pr[p].graded_windows > 0;
            checks.expect(prop_ok, label);
        } else {
            checks.record(label + " -- not checkable: a section 4 precondition (PJ1, PJ2 or PJ3) did not hold");
        }
    }

    // ---- section 5: pot-pin run -------------------------------------------
    PotRun pt = run_pot(dir, mv);
    bool p_fixture_ok = pt.loaded && pt.fixture && *pt.fixture == kFixtureValue;
    if (mv.empty())
        checks.expect(p_fixture_ok, "FIXTURE (section 5): the program can deliver a known stored constant before any other result is graded");
    else
        checks.note(std::string("FIXTURE (section 5) under NMH_MUTATE=") + mv + ": " + (p_fixture_ok ? "ok" : "FAILED"));

    checks.expect(pt.loaded && pt.released && pt.done.has_value(),
                  "section 5 fixture: prog_pot reaches DONE (loaded=" + std::to_string(pt.loaded) +
                  " released=" + std::to_string(pt.released) + " done=" + std::to_string(pt.done.has_value()) + ")");

    auto samples2 = extract_samples2(pt.log, word_addr(kBufBase), 768);

    // PP1 [SPECIFICATION.md 5.5]: from the driver's own action record --
    // every scheduled button change applied, at its planned sample index,
    // at least 6 samples from each of that step's POTGO markers.
    bool pp1_missing_ok = true;
    for (auto& e : pt.applied) if (e.skipped) pp1_missing_ok = false;
    bool pp1_timing_ok = true;
    for (auto& e : pt.applied) if (!e.skipped && e.sample_index != e.planned_index) pp1_timing_ok = false;
    // The source and bit actually driven, against the plan's own mapping
    // (kPotPins): a press leaves exactly the pin's own bit set in its own
    // source, a release leaves every source at 0.
    bool pp1_pins_ok = true;
    for (auto& e : pt.applied) {
        if (e.skipped || (e.step != 1 && e.step != 2 && e.step != 4 && e.step != 6)) continue;
        const PotPin& plan = kPotPins[e.pin];
        uint8_t want = e.press ? uint8_t(1u << plan.bit) : 0;
        uint8_t want_mb = plan.src == Src::kMouseButtons ? want : 0;
        uint8_t want_j0 = plan.src == Src::kJoystick0 ? want : 0;
        uint8_t want_j1 = plan.src == Src::kJoystick1 ? want : 0;
        if (plan.src == Src::kNone) want_mb = want_j0 = want_j1 = 0;
        if (e.src != plan.src || e.bit != plan.bit || e.mb != want_mb || e.j0 != want_j0 || e.j1 != want_j1)
            pp1_pins_ok = false;
    }
    bool pp1_gap_ok = true;
    {
        // Steps 1(C2),2(C3) sit between markers 0(C1) and 1(C4); step 4(C5)
        // between markers 1(C4) and 2(C6); step 6(C7) between markers
        // 2(C6) and 3(C8). Steps 0,3,5,7 are the marker (POTGO-write) steps
        // themselves, not button changes, and are skipped here.
        int prev_j[8] = {-1, 0, 0, -1, 1, -1, 2, -1};
        int next_j[8] = {-1, 1, 1, -1, 2, -1, 3, -1};
        for (auto& e : pt.applied) {
            if (e.skipped) continue;
            if (prev_j[e.step] < 0) continue;
            size_t pmi = (size_t)e.pin * 4 + (size_t)prev_j[e.step];
            size_t nmi = (size_t)e.pin * 4 + (size_t)next_j[e.step];
            if (pmi >= pt.marker_sample.size() || nmi >= pt.marker_sample.size()) { pp1_gap_ok = false; continue; }
            size_t pm = pt.marker_sample[pmi], nm = pt.marker_sample[nmi];
            if (e.sample_index < pm + 6) pp1_gap_ok = false;
            if (e.sample_index + 6 > nm) pp1_gap_ok = false;
        }
    }
    bool pp1_ok = pp1_missing_ok && pp1_timing_ok && pp1_pins_ok && pp1_gap_ok;
    checks.expect(pp1_ok, "PP1 [SPECIFICATION.md 5.5]: the driver's own action record shows the button changes "
                           "on the pin under test only, each in its intended step, at least 6 samples stored "
                           "from each of that step's markers");

    // PP2: markers (16 expected, 4 per pin) match the planned POTGO values,
    // bit 0 = 0 in all.
    bool pp2_ok;
    {
        std::vector<uint16_t> plan;
        for (int p = 0; p < 4; ++p) { plan.push_back(kPotPins[p].c1); plan.push_back(kPotPins[p].c4);
                                       plan.push_back(kPotPins[p].c6); plan.push_back(kPotPins[p].c8); }
        if (mv == "PP2" && !plan.empty()) plan[0] = 0xFF01; // pin 5's C1
        pp2_ok = pt.markers.size() == plan.size();
        for (size_t i = 0; pp2_ok && i < plan.size(); ++i)
            if (pt.markers[i] != plan[i] || (pt.markers[i] & 1) != 0) pp2_ok = false;
        checks.expect(pp2_ok, "PP2 [SPECIFICATION.md 5.5]: the POTGO values stored with the markers are the "
                               "planned ones, and bit 0 is 0 in all of them (" + std::to_string(pt.markers.size()) +
                               " markers seen)");
    }

    // Per-pin grading. Step windows, like section 4's, are bounded by
    // consecutive entries of the driver's own action record, and a window
    // is graded only if it holds at least 3 really-stored samples (a
    // missing sample no longer passes silently).
    struct PinResult { bool ok = true; int graded_windows = 0; bool population_ok = true;
                        std::vector<uint16_t> observed_d; std::vector<int> observed_step; };
    PinResult pinr[4];
    const int expect_bit[8] = {1, 0, 1, 0, 0, 0, -1, 1}; // C1..C8; -1 = not graded (C7)
    for (int p = 0; p < 4; ++p) {
        for (int step = 0; step < 8; ++step) {
            size_t ai = (size_t)p * 8 + (size_t)step;
            size_t start = pt.applied[ai].sample_index;
            size_t end;
            if (step + 1 < 8) end = pt.applied[ai + 1].sample_index;
            else if (p + 1 < 4) end = pt.applied[(size_t)(p + 1) * 8 + 0].sample_index;
            else end = samples2.size();

            if (end < start + 7) { pinr[p].population_ok = false; continue; } // 6-line guard leaves < 1 sample
            size_t lo = start + 6, hi = end - 1;
            int real_count = 0;
            for (size_t idx = lo; idx <= hi && idx < samples2.size(); ++idx) if (samples2[idx].pg) ++real_count;
            if (real_count < 3) { pinr[p].population_ok = false; continue; }
            ++pinr[p].graded_windows;

            for (size_t idx = lo; idx <= hi && idx < samples2.size(); ++idx) {
                auto& s = samples2[idx];
                if (!s.pg) continue;
                uint16_t d = (*s.pg >> kPotPins[p].dat_bit) & 1;
                pinr[p].observed_d.push_back(d);
                pinr[p].observed_step.push_back(step);
                if (expect_bit[step] >= 0 && (int)d != expect_bit[step]) pinr[p].ok = false;
                // other three DAT bits stay 1
                for (int q = 0; q < 4; ++q) {
                    if (q == p) continue;
                    uint16_t od = (*s.pg >> kPotPins[q].dat_bit) & 1;
                    if (od != 1) pinr[p].ok = false;
                }
            }
        }
    }

    bool pp3_ok = pinr[0].population_ok && pinr[1].population_ok && pinr[2].population_ok && pinr[3].population_ok;
    checks.expect(pp3_ok, "PP3 [SPECIFICATION.md 5.5]: every graded window holds at least 3 samples");

    const char* pnames[4] = {"P5", "P6", "P7", "P8"};
    const char* pids[4] = {"opus:AHJ-243, sol:J-110", "opus:AHJ-245, sol:J-112", "opus:AHJ-249, sol:J-116", "opus:AHJ-251, sol:J-118"};
    const char* ppins[4] = {"POT0X (left, pin 5)", "POT0Y (left, pin 9)", "POT1X (right, pin 5)", "POT1Y (right, pin 9)"};
    bool section5_pre_ok = pp1_ok && pp2_ok && pp3_ok;
    for (int p = 0; p < 4; ++p) {
        if (p == 2) continue; // P7: withdrawn, see below (not a graded proposition)
        std::string label = std::string(pnames[p]) + " [" + pages[p] + ": " + pids[p] + "]: Paula's " + ppins[p] +
            " is bidirectional: OUT/DAT drive the pin, and POTGOR reports its actual level including a forced button";
        if (section5_pre_ok) {
            bool prop_ok = pinr[p].ok && pinr[p].graded_windows > 0;
            checks.expect(prop_ok, label);
        } else {
            checks.record(label + " -- not checkable: a section 5 precondition (PP1, PP2 or PP3) did not hold");
        }
    }
    // One value per step and pin: the DAT bit when every observed sample of
    // the step reads it alike, "0/1" when they differ, "?" when the step
    // holds none, and "-" for C7 (not graded).
    auto dat_by_step = [&](int p) {
        std::string s;
        for (int step = 0; step < 8; ++step) {
            bool saw0 = false, saw1 = false;
            for (size_t k = 0; k < pinr[p].observed_d.size(); ++k)
                if (pinr[p].observed_step[k] == step) (pinr[p].observed_d[k] ? saw1 : saw0) = true;
            std::string v = expect_bit[step] < 0 ? "-" : (saw0 && saw1) ? "0/1" : saw1 ? "1" : saw0 ? "0" : "?";
            s += (step ? " " : "") + std::string("C") + std::to_string(step + 1) + "=" + v;
        }
        return s;
    };
    checks.note("P5-P8 observed DAT bit per step (C1..C8; - = not graded, 0/1 = the samples of the step differ, "
                "? = no sample): P5 [" + dat_by_step(0) + "] P6 [" + dat_by_step(1) + "] P7 [" + dat_by_step(2) +
                "] P8 [" + dat_by_step(3) + "]");

    // P7 (POT1X): withdrawn (SPECIFICATION.md 5.1, 8.2). The input half (a
    // pressed button forcing right-port pin 5) cannot be set up at this
    // boundary. C1, C3, C4 and C8 (the output half) are measured without a
    // claim id.
    if (mv.empty()) {
        bool p7_output_ok = true; int p7_output_checked = 0;
        for (size_t k = 0; k < pinr[2].observed_d.size(); ++k) {
            int step = pinr[2].observed_step[k];
            if (step == 0 || step == 2 || step == 3 || step == 7) { // C1, C3, C4, C8
                ++p7_output_checked;
                if (expect_bit[step] >= 0 && (int)pinr[2].observed_d[k] != expect_bit[step]) p7_output_ok = false;
            }
        }
        checks.note("POT1X (right, pin 5), output steps C1/C3/C4/C8 only, no claim id (the input half is "
                     "withdrawn below): " + std::string(p7_output_ok ? "match p.240/241's OUT/DAT prediction" :
                     "do not match") + " in " + std::to_string(p7_output_checked) + " graded samples");
        checks.record("opus:AHJ-249, sol:J-116 [p.380: \"35 POT1X Pot 1X I/O\"]: withdrawn -- the input half (a "
                       "button forcing right-port pin 5 low) cannot be set up at this boundary: the right port's "
                       "only input is joystick0, and Table 8-1 (p.228) gives a joystick nothing on pin 5 "
                       "(\"---\"); for the condition actually set up (no button), p.241's \"a 1 if it is not\" "
                       "is what both cores read (C2)");
    }

    // ---- section 6: drive-light record (no claim, not graded) -------------
    if (mv.empty()) {
        DriveLightRun dl = run_drivelight(dir);
        bool dl_fixture_ok = dl.loaded && dl.fixture && *dl.fixture == kFixtureValue;
        checks.expect(dl_fixture_ok, "FIXTURE (section 6): the program can deliver a known stored constant before "
                                      "any other result is graded");
        checks.expect(dl.loaded && dl.released && dl.done.has_value(),
                      "section 6 fixture: prog_drivelight reaches DONE");
        // 9 markers (S0..S8), in order, at RESULTS.
        std::vector<uint16_t> mk;
        uint32_t results_w = word_addr(kResults);
        for (auto& a : dl.log) if (a.is_write && a.word_addr == results_w) mk.push_back(a.data);
        std::vector<uint64_t> mk_cycle;
        { uint32_t seen = 0; for (auto& a : dl.log) if (a.is_write && a.word_addr == results_w) { mk_cycle.push_back(a.cycle); ++seen; } }
        uint64_t done_cycle = dl.done.value_or(0);
        const char* snames[9] = {"S0","S1","S2","S3","S4","S5","S6","S7","S8"};
        std::string line = "section 6 [p.244]: fdd_led level just before the next marker, S0..S8 (motor/select "
                            "sequence, drive 0): ";
        for (int i = 0; i < 9 && i < (int)mk_cycle.size(); ++i) {
            uint64_t before_cycle = (i + 1 < (int)mk_cycle.size()) ? mk_cycle[i + 1] - 1 : (done_cycle ? done_cycle - 1 : mk_cycle[i]);
            auto led = dl.led_at(before_cycle);
            line += std::string(snames[i]) + "=" + (led ? (*led ? "on" : "off") : "?") + " ";
        }
        checks.record(line);
        checks.note("section 6 markers observed (PRB byte written, zero-extended): " +
                     [&]{ std::string s; for (auto v : mk) s += hex4(v) + " "; return s; }());
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
