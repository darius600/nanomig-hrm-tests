// test_ch8_controller_ports.cpp -- ch8-controller-ports. See
// SPECIFICATION.md for every assertion's proposition, program, observed
// value and expected value. This file only implements what
// SPECIFICATION.md already committed to.
//
// Connector wiring (stimulus information, not an expected value):
// Connector A (JOY0DAT, CIAAPRA bit 6, the manual's "left"/"port 1") is
// the harness's `joystick1` port. Connector B (JOY1DAT, CIAAPRA bit 7,
// "right"/"port 2") is the harness's `joystick0` port -- the opposite of
// the naive index reading ("joystick0" = "0=left"). The P2 anchor (below)
// checks this choice empirically at every run; it is not assumed to hold.
//
// Bit layout within the 8-bit joystick0/joystick1 field: bit3=up(pin1),
// bit2=down(pin2), bit1=left(pin3), bit0=right(pin4), bit4=fire(pin6),
// bit5=the pot-line gate used below. A harness field bit of 1 grounds the
// pin. SPECIFICATION.md's "level" is the electrical level (1=open,
// 0=shorted), so: harness bit = !level.
//
// Pot lines (P1's four lines, pins 5 and 9 of both connectors), as ports
// reachable at all from nanomig.v: LX (connector A pin 5) only through
// mouse_buttons bit 2; LY (connector A pin 9) through mouse_buttons bit 1
// AND connector A's own joystick1 bit 5; RY (connector B pin 9) through
// connector B's own joystick0 bit 5; RX (connector B pin 5) through no
// port at all. See the K5 mutation below.
//
// MUTATION PC1 stimulus red the R step of every Fc waits for line 12 instead of line 10, via NMH_MUTATE=K3
// MUTATION PC2 stimulus red Phase P's F1 writes $AA00 instead of $FF00, via NMH_MUTATE=K4
// MUTATION PC3 stimulus red the harness holds all four pot lines it can reach (LX, LY, RY -- RX is unreachable, see above) at level 0 for the whole run instead of level 1, via NMH_MUTATE=K5
// MUTATION PC4 stimulus red the R step of every Fc waits for line 5 instead of line 10, via NMH_MUTATE=K6
// MUTATION C1-Fc-OUT stimulus green (unchanged clause) every Fc write withholds bit 0 (START), changing nothing else; the "1" (output) column of C1-C4 must stay green under K1, since it grades the output-enable/data bits, which K1 leaves untouched, via NMH_MUTATE=K1
// MUTATION Q1 stimulus red the harness applies Yup(A)'s levels only after it sees U_k instead of at S_k, via NMH_MUTATE=Q1
// MUTATION Q2 stimulus red the harness additionally grounds pin 9 of connector A throughout Yup(A), via NMH_MUTATE=Q2
// MUTATION Q3 stimulus red the harness applies Yup(A) sub-step 0's levels at N1 instead of at N1's own (released) step, via NMH_MUTATE=Q3
// MUTATION Q4 stimulus red F1(A) is driven at level 1 (released) instead of level 0, via NMH_MUTATE=Q4
// MUTATION P2-BLOCKA-JOY0 stimulus red block A's steps are applied to connector B and block B's to connector A (the F steps stay on their own connector, so the anchor is unaffected), via NMH_MUTATE=P2VAR
// MUTATION FIXTURE stimulus red stored constant read back does not match the value the program wrote, via NMH_MUTATE=FIXTURE
// MUTATION P2-ANCHOR stimulus red F1(A) is driven on connector B instead of connector A, via NMH_MUTATE=P2ANCHOR
// MUTATION P3-Y stimulus red in Yup(A), the harness drives pin 2 where the table says pin 1 (pin 1 stays released), via NMH_MUTATE=P3VAR
// MUTATION P4-PIN1 stimulus red in Buttons(A), the harness grounds pin 2 wherever the table says pin 1 (pin 1 stays released), via NMH_MUTATE=P4VAR
//
// K2 is not a separate mutation: every Phase-D config already carries its
// own "the other three lines" contrast in the same assertion (the "1"
// column next to the "0" column, SPECIFICATION.md section 4).

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint64_t kBootBudget = 6'000'000;

// clk_sys, the harness's own clock, runs at 28.375160 MHz (the PAL Amiga
// system clock; the same figure ch8-disk-serial's SPECIFICATION.md and
// hrm/appK-bus-clock-definitions cite). Converts a cycle count to
// nanoseconds for PC4's 300 us bound (p.240).
constexpr double kClkHz = 28'375'160.0;
double ns_from_cycles(uint64_t c) { return (double)c * (1.0e9 / kClkHz); }

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

uint32_t word_addr(uint32_t byte_addr) { return byte_addr / 2; }

// nmh::NanomigHarness::run_until_write, restated to match ANY value at a
// fixed word address (used for U_k -- the timestamp of a store whose
// value is the data itself, not a known marker).
std::optional<uint64_t> run_until_write_any(nmh::NanomigHarness& h, uint32_t byte_addr, uint64_t max_cycles) {
    const uint32_t w = word_addr(byte_addr);
    size_t seen = h.ram_log().size();
    const uint64_t start = h.cycles();
    while (h.cycles() - start < max_cycles) {
        h.run_cycles(1);
        for (; seen < h.ram_log().size(); ++seen)
            if (h.ram_log()[seen].is_write && h.ram_log()[seen].word_addr == w)
                return h.ram_log()[seen].cycle;
    }
    return std::nullopt;
}

struct VidCfg { const char* name; uint8_t chipset_config; };
constexpr VidCfg kConfigs[4] = {
    {"PAL+ECS",  0x08}, {"NTSC+ECS", 0x0A}, {"PAL+OCS",  0x00}, {"NTSC+OCS", 0x02},
};
nmh::Config cfg_for(const VidCfg& v) {
    nmh::Config c;
    c.chipset_config = v.chipset_config;
    return c;
}

// =========================================================================
// Proposition 1: POTGO bit 0 dumps the pot-line capacitors (p.240).
// =========================================================================

constexpr uint32_t kP1Base = 0x0C1000;

struct WSlot { int vw0 = -1, vw1 = -1, m1 = -1, rpre = -1; bool has_rpre = false; };
struct RSlot { int vr0[4], m2[4], pg[4], vr1[4]; int target; };

struct Cur {
    int pos = 0;
    WSlot w(bool rpre) {
        WSlot s;
        s.vw0 = pos++;
        if (rpre) { s.has_rpre = true; s.rpre = pos++; }
        s.vw1 = pos++;
        s.m1 = pos++;
        return s;
    }
    RSlot r(int target) {
        RSlot s;
        s.target = target;
        for (int i = 0; i < 4; ++i) { s.vr0[i] = pos++; s.m2[i] = pos++; s.pg[i] = pos++; s.vr1[i] = pos++; }
        return s;
    }
};

struct P1Frame {
    std::string label;
    WSlot w;
    RSlot r;
    bool is_p2_high = false;  // Phase P, output driven high (F1, F3): PC2's Rhigh/Rhigh2
    bool is_p2_low = false;   // Phase P, output driven low (F2): PC2's Rlow
    bool is_fa = false, is_fb = false, is_fc = false;
    uint16_t out_mask = 0;    // Fc only: which of $5500 is the output-driven bit
};

// PC1 and every other check below always grade against line 10 (p.238's
// own reference frame, L0+6): a mutant assembly (K3, K6) may itself wait
// for a different line, but the grader must not adopt that line as its
// own expectation, or it stops checking anything -- it would just agree
// with whatever the mutant did. Holding "10" fixed here is what makes
// PC1 fail when the assembly's own wait target moves.
std::vector<P1Frame> build_p1_frames() {
    constexpr int fc_target = 10;
    Cur c;
    std::vector<P1Frame> f;
    { P1Frame x; x.label = "F1"; x.w = c.w(false); x.r = c.r(10); x.is_p2_high = true; f.push_back(x); }
    { P1Frame x; x.label = "F2"; x.w = c.w(false); x.r = c.r(10); x.is_p2_low  = true; f.push_back(x); }
    { P1Frame x; x.label = "F3"; x.w = c.w(false); x.r = c.r(10); x.is_p2_high = true; f.push_back(x); }
    struct CfgDef { const char* name; uint16_t out_mask; };
    constexpr CfgDef cfgs[5] = {
        {"C0", 0x0000}, {"C1", 0x0100}, {"C2", 0x0400}, {"C3", 0x1000}, {"C4", 0x4000},
    };
    for (auto& cd : cfgs) {
        { P1Frame x; x.label = std::string(cd.name) + "-Fa"; x.w = c.w(false); x.r = c.r(10); x.is_fa = true; f.push_back(x); }
        { P1Frame x; x.label = std::string(cd.name) + "-Fb"; x.w = c.w(false); x.r = c.r(10); x.is_fb = true; f.push_back(x); }
        { P1Frame x; x.label = std::string(cd.name) + "-Fc"; x.w = c.w(true);  x.r = c.r(fc_target); x.is_fc = true; x.out_mask = cd.out_mask; f.push_back(x); }
    }
    return f;
}

struct P1Run {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::unordered_map<uint32_t, uint64_t> first_write; // word_addr -> cycle
    std::vector<std::optional<uint16_t>> word;          // word[i] = value at kP1Base+2*i
    int nwords = 0;
};

uint16_t wv(const P1Run& r, int idx) { return (idx >= 0 && idx < (int)r.word.size() && r.word[idx]) ? *r.word[idx] : 0xFFFF; }
uint64_t cyc(const P1Run& r, int word_index) {
    uint32_t addr = word_addr(kP1Base + 2 * word_index);
    auto it = r.first_write.find(addr);
    return it == r.first_write.end() ? 0 : it->second;
}

// =========================================================================
// The joystick program (shared by P2, P3, P4).
// =========================================================================

constexpr uint32_t kJoyMark    = 0x0C1C00;
constexpr uint32_t kJoyResults = 0x0C2000;
constexpr int kNSteps = 54;
constexpr uint64_t kStepBudget = 200'000; // >> one PAL/NTSC line (~1816 cycles)
// The gap between cpu_nrst_out (what NanomigHarness::boot() polls) and the
// CPU's first fetch measures several million clk_sys cycles longer than
// that one signal (measured directly: prog_fixture's own DONE, a
// three-instruction program, needed ~2.27M post-boot cycles here, not the
// low thousands boot() alone would suggest) -- minimig_syscontrol's own
// reset holds the CPU well past cpu_nrst_out's first edge. Budget for it
// once, before step 0's own wait.
constexpr uint64_t kFirstStepBudget = 6'000'000;

enum class Conn { NONE, A, B };

struct Levels { bool up = true, down = true, left = true, right = true, fire = true; };

uint8_t encode(const Levels& L) {
    uint8_t v = 0;
    if (!L.up)    v |= (1u << 3);
    if (!L.down)  v |= (1u << 2);
    if (!L.left)  v |= (1u << 1);
    if (!L.right) v |= (1u << 0);
    if (!L.fire)  v |= (1u << 4);
    return v;
}

struct StepDef { Conn conn; std::string group; Levels lv; };

std::vector<StepDef> baseline_steps() {
    std::vector<StepDef> s;
    s.push_back({Conn::NONE, "N", {}}); // N0
    s.push_back({Conn::NONE, "N", {}}); // N1
    for (Conn c : {Conn::A, Conn::B}) {
        s.push_back({c, "F1", {true, true, true, true, false}});
        s.push_back({c, "F0", {true, true, true, true, true}});
        const bool yu_up[4] = {false, false, true, true}, yu_lf[4] = {true, false, false, true};
        for (int i = 0; i < 4; ++i) s.push_back({c, "Yup", {yu_up[i], true, yu_lf[i], true, true}});
        const bool yd_up[4] = {true, false, false, true}, yd_lf[4] = {false, false, true, true};
        for (int i = 0; i < 4; ++i) s.push_back({c, "Ydown", {yd_up[i], true, yd_lf[i], true, true}});
        const bool xu_dn[4] = {false, false, true, true}, xu_rt[4] = {true, false, false, true};
        for (int i = 0; i < 4; ++i) s.push_back({c, "Xup", {true, xu_dn[i], true, xu_rt[i], true}});
        const bool xd_dn[4] = {true, false, false, true}, xd_rt[4] = {false, false, true, true};
        for (int i = 0; i < 4; ++i) s.push_back({c, "Xdown", {true, xd_dn[i], true, xd_rt[i], true}});
        struct BS { bool p1, p3, p4; };
        constexpr BS combos[8] = {
            {true, false, false}, {true, true, false}, {false, true, false}, {false, true, true},
            {true, true, true},   {true, false, true}, {false, false, true}, {false, false, false},
        };
        for (auto& b : combos) s.push_back({c, "Buttons", {!b.p1, true, !b.p3, !b.p4, true}});
    }
    return s;
}

Levels levels_of(const StepDef& s, Conn which) { return s.conn == which ? s.lv : Levels{}; }

int find_step(const std::vector<StepDef>& steps, Conn conn, const std::string& group, int occurrence = 0) {
    int seen = 0;
    for (int i = 0; i < (int)steps.size(); ++i)
        if (steps[i].conn == conn && steps[i].group == group) { if (seen == occurrence) return i; ++seen; }
    return -1;
}
std::vector<int> find_steps(const std::vector<StepDef>& steps, Conn conn, const std::string& group) {
    std::vector<int> out;
    for (int i = 0; i < (int)steps.size(); ++i) if (steps[i].conn == conn && steps[i].group == group) out.push_back(i);
    return out;
}

struct Applied { uint8_t j0 = 0, j1 = 0, mbtn = 0; bool delay_until_uk = false; };

Applied compute_applied(const std::vector<StepDef>& steps, int k, const std::string& mv) {
    StepDef s = steps[k];
    bool moving = (s.group == "Yup" || s.group == "Ydown" || s.group == "Xup" || s.group == "Xdown" || s.group == "Buttons");

    if (mv == "P2VAR" && moving && s.conn != Conn::NONE) s.conn = (s.conn == Conn::A) ? Conn::B : Conn::A;
    if (mv == "P2ANCHOR" && s.conn == Conn::A && s.group == "F1") s.conn = Conn::B;

    Levels a = levels_of(s, Conn::A);
    Levels b = levels_of(s, Conn::B);
    bool extra_a5 = false; // connector A pin9 (LY) forced to level 0

    if (mv == "Q2" && steps[k].conn == Conn::A && steps[k].group == "Yup") extra_a5 = true;
    if (mv == "Q3" && k == 1) a = Levels{false, true, true, true, true}; // Yup(A) sub-step 0's pattern, at N1
    if (mv == "Q4" && steps[k].conn == Conn::A && steps[k].group == "F1") a.fire = true; // released instead of pressed
    if (mv == "P3VAR" && steps[k].conn == Conn::A && steps[k].group == "Yup") { a.down = a.up; a.up = true; }
    if (mv == "P4VAR" && steps[k].conn == Conn::A && steps[k].group == "Buttons") { a.down = a.up; a.up = true; }

    Applied ap;
    ap.j1 = encode(a);
    ap.j0 = encode(b);
    if (extra_a5) ap.j1 |= (1u << 5);
    if (mv == "Q1" && steps[k].conn == Conn::A && steps[k].group == "Yup") ap.delay_until_uk = true;
    return ap;
}

struct JoyRun {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<std::array<uint16_t, 4>> rec; // [JOY0DAT, JOY1DAT, CIAAPRA(byte), POTGOR]
    std::vector<uint64_t> t_apply, t_t, t_u;
    bool timed_out = false;
    int steps_completed = 0;
};

JoyRun run_joy(const std::string& bin, const std::string& mv, nmh::Config cfg) {
    JoyRun r;
    static const std::vector<StepDef> steps = baseline_steps();
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;
    h.ports()->joystick0 = 0;
    h.ports()->joystick1 = 0;
    h.ports()->mouse_buttons = 0;

    for (int k = 0; k < kNSteps; ++k) {
        auto ts = h.run_until_write(kJoyMark, (uint16_t)k, k == 0 ? kFirstStepBudget : kStepBudget); // S_k
        if (!ts) { r.timed_out = true; break; }
        Applied ap = compute_applied(steps, k, mv);
        if (!ap.delay_until_uk) {
            h.ports()->joystick0 = ap.j0; h.ports()->joystick1 = ap.j1;
            r.t_apply.push_back(h.cycles()); // the real application time: now
        }
        auto tt = h.run_until_write(kJoyMark, (uint16_t)k, kStepBudget); // T_k
        if (!tt) { r.timed_out = true; break; }
        r.t_t.push_back(*tt);
        auto tu = run_until_write_any(h, kJoyResults + 8 * k, kStepBudget); // U_k (JOY0DAT store)
        if (!tu) { r.timed_out = true; break; }
        r.t_u.push_back(*tu);
        if (ap.delay_until_uk) {
            h.ports()->joystick0 = ap.j0; h.ports()->joystick1 = ap.j1;
            r.t_apply.push_back(h.cycles()); // the real application time: only now, after U_k
        }
        r.steps_completed = k + 1;
    }
    r.done = h.run_until_write(kDoneAddr, kDoneValue, kStepBudget);
    for (int k = 0; k < kNSteps; ++k) {
        std::array<uint16_t, 4> rr{0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
        for (int w = 0; w < 4; ++w) { auto v = h.peek_logical(kJoyResults + 8 * k + 2 * w); if (v) rr[w] = *v; }
        r.rec.push_back(rr);
    }
    return r;
}

// Figure 8-2 (p.230): (level1,level2) -> (D1,D0) -> state s.
int state_of(bool p1, bool p2) {
    if (p1 && p2) return 0;   // 1 1 : 0 0
    if (!p1 && p2) return 1;  // 0 1 : 0 1
    if (!p1 && !p2) return 2; // 0 0 : 1 0
    return 3;                 // 1 0 : 1 1
}
int d1d0_of(int s) {
    switch (s) { case 0: return 0b00; case 1: return 0b01; case 2: return 0b10; default: return 0b11; }
}

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";
    const nmh::Config default_cfg = cfg_for(kConfigs[0]);

    // =====================================================================
    // FIXTURE
    // =====================================================================
    {
        std::string bin = dir + (mv == "FIXTURE" ? "/prog_fixture_mut.bin" : "/prog_fixture.bin");
        nmh::NanomigHarness h(default_cfg);
        bool loaded = h.load_program(bin) > 0;
        bool released = loaded && h.boot(kBootBudget).cpu_released;
        auto done = released ? h.run_until_write(kDoneAddr, kDoneValue, 9'000'000) : std::nullopt;
        auto v = done ? h.peek_logical(0x0C0100) : std::nullopt;
        checks.expect(loaded && released && done.has_value() && v && *v == 0xFEED,
            "FIXTURE: a known constant stored by the program reads back correctly "
            "[not graded -- apparatus check, no claim uid]");
    }

    // =====================================================================
    // Proposition 1 (p.240): opus:8-dio.c21, sol:8-dio.c19
    // =====================================================================
    {
        auto frames = build_p1_frames();
        int nwords = 347;

        std::string bin = dir + "/" + (mv == "K1" ? "prog_p1_k1" : mv == "K3" ? "prog_p1_k3" :
                                        mv == "K4" ? "prog_p1_k4" : mv == "K6" ? "prog_p1_k6" : "prog_p1") + ".bin";

        // Baseline stimulus: all four pot lines released (level 1) throughout,
        // i.e. mouse_buttons=0 and joystick0/1 bit 5 = 0. K5 holds the three
        // reachable lines (LX, LY, RY) at level 0 (grounded) for the whole
        // run instead (RX cannot be forced low by any port -- see the header
        // comment); this is wider than "during Fb and Fc" (SPECIFICATION.md
        // K5), a scope simplification recorded below, which does not change
        // what K5 demonstrates (Rb/Rpre reading $0000 on the reachable lines).
        auto run_variant = [&](nmh::Config c) {
            nmh::NanomigHarness h(c);
            bool loaded = h.load_program(bin) > 0;
            P1Run r; r.nwords = nwords;
            if (!loaded) return r;
            r.loaded = true;
            if (mv == "K5") {
                h.ports()->mouse_buttons = (1u << 2) | (1u << 1); // grounds LX and LY
                h.ports()->joystick1 = (1u << 5);                 // connector A pin 9 (LY)
                h.ports()->joystick0 = (1u << 5);                 // connector B pin 9 (RY)
            } else {
                h.ports()->mouse_buttons = 0;
                h.ports()->joystick0 = 0;
                h.ports()->joystick1 = 0;
            }
            r.released = h.boot(kBootBudget).cpu_released;
            r.done = h.run_until_write(kDoneAddr, kDoneValue, 20'000'000);
            for (auto& a : h.ram_log())
                if (a.is_write) { auto it = r.first_write.find(a.word_addr); if (it == r.first_write.end()) r.first_write[a.word_addr] = a.cycle; }
            for (int i = 0; i < nwords; ++i) r.word.push_back(h.peek_logical(kP1Base + 2 * i));
            return r;
        };

        // Primary graded run: PAL+ECS (the harness's own default machine).
        P1Run r = run_variant(default_cfg);
        checks.expect(r.loaded && r.released && r.done.has_value(),
            "P1-FIXTURE: prog_p1 loads, releases reset and reaches DONE "
            "[not graded -- apparatus check, no claim uid]");

        if (r.loaded && r.done) {
            // Cross-config spot check: the other three (video standard x
            // chipset) combinations SPECIFICATION.md's "Configurations"
            // section asks every program to run under. p.240's dump
            // mechanism (or its absence) does not depend on any of them, so
            // this is graded as agreement with the primary run, not
            // separately per assertion.
            if (mv.empty()) {
                bool all_ran = true;
                std::string mismatch_note;
                for (int ci = 1; ci < 4; ++ci) {
                    P1Run r2 = run_variant(cfg_for(kConfigs[ci]));
                    if (!(r2.loaded && r2.done.has_value())) all_ran = false;
                    // Informational only: the exact R-step slot that lands on
                    // Vr0=Vr1=target can differ by video standard (a
                    // different number of instructions fit before a line
                    // boundary at NTSC's line rate), so a byte-for-byte
                    // comparison is not the right invariant here. What must
                    // agree is the graded verdict.
                    bool same_dump_pattern = true;
                    for (int ci2 = 0; ci2 < 5; ++ci2) {
                        auto& fc = frames[3 + 3 * ci2 + 2];
                        auto zero_ok_of = [&](const P1Run& rr) {
                            bool ok = true; int cnt = 0;
                            for (int i = 0; i < 4; ++i)
                                if (wv(rr, fc.r.vr0[i]) == fc.r.target && wv(rr, fc.r.vr1[i]) == fc.r.target) {
                                    ++cnt; if ((wv(rr, fc.r.pg[i]) & 0x5500 & ~fc.out_mask) != 0) ok = false;
                                }
                            return cnt > 0 && ok;
                        };
                        if (zero_ok_of(r2) != zero_ok_of(r)) same_dump_pattern = false;
                    }
                    if (!same_dump_pattern) mismatch_note += std::string(kConfigs[ci].name) + " ";
                }
                checks.expect(all_ran,
                    "P1-CONFIGS: PAL/NTSC x ECS/OCS all load, release and reach DONE "
                    "[not graded -- apparatus check, no claim uid]");
                if (!mismatch_note.empty())
                    checks.note("P1-CONFIGS: the dump verdict pattern differs from PAL+ECS under: " +
                                mismatch_note + "(the exact R-step slot that lands on the target line "
                                "can differ by video standard; see the comment above this line)");
            }

            // ---- PC1: line position -------------------------------------
            bool pc1 = true; int pc1_r_records = 0;
            for (auto& fr : frames) {
                if (wv(r, fr.w.vw0) != 4 || wv(r, fr.w.vw1) != 4) pc1 = false;
                bool any = false;
                for (int i = 0; i < 4; ++i)
                    if (wv(r, fr.r.vr0[i]) == fr.r.target && wv(r, fr.r.vr1[i]) == fr.r.target) { any = true; ++pc1_r_records; }
                if (!any) pc1 = false;
            }
            checks.expect(pc1,
                "PC1 [p.238,p.239]: every W's Vw0=Vw1=4, and every R step has at least one "
                "record with Vr0=Vr1=10");
            checks.note("PC1: " + std::to_string(pc1_r_records) + " qualifying R records across " +
                        std::to_string(frames.size()) + " R steps");

            // ---- PC2: POTGOR follows the output-driven level ------------
            auto mask_ok = [&](uint16_t pg, uint16_t want) { return (pg & 0x5500) == want; };
            bool pc2 = true;
            for (auto& fr : frames) {
                if (!(fr.is_p2_high || fr.is_p2_low || fr.is_fa)) continue;
                uint16_t want = fr.is_p2_low ? 0x0000 : 0x5500;
                for (int i = 0; i < 4; ++i)
                    if (wv(r, fr.r.vr0[i]) == fr.r.target && wv(r, fr.r.vr1[i]) == fr.r.target)
                        if (!mask_ok(wv(r, fr.r.pg[i]), want)) pc2 = false;
            }
            checks.expect(pc2,
                "PC2 [p.240 \"The state of the data bit controls the logic level on the output "
                "pin.\"; p.241]: Rhigh, Rhigh2 and every Ra read $5500; Rlow reads $0000");

            // ---- PC3: each input line reads 1 just before the dump ------
            bool pc3 = true;
            for (auto& fr : frames) {
                if (fr.is_fb)
                    for (int i = 0; i < 4; ++i)
                        if (wv(r, fr.r.vr0[i]) == fr.r.target && wv(r, fr.r.vr1[i]) == fr.r.target)
                            if (!mask_ok(wv(r, fr.r.pg[i]), 0x5500)) pc3 = false;
                if (fr.is_fc && fr.w.has_rpre)
                    if (!mask_ok(wv(r, fr.w.rpre), 0x5500)) pc3 = false;
            }
            checks.expect(pc3,
                "PC3 [p.240]: Rb and Rpre read $5500 in every configuration");

            // ---- PC4: settling time (>=300us between M1 and each M2) ----
            bool pc4 = true; int pc4_checked = 0;
            for (auto& fr : frames) {
                uint64_t t_m1 = cyc(r, fr.w.m1);
                for (int i = 0; i < 4; ++i)
                    if (wv(r, fr.r.vr0[i]) == fr.r.target && wv(r, fr.r.vr1[i]) == fr.r.target) {
                        uint64_t t_m2 = cyc(r, fr.r.m2[i]);
                        ++pc4_checked;
                        if (!(t_m2 > t_m1 && ns_from_cycles(t_m2 - t_m1) >= 300000.0)) pc4 = false;
                    }
            }
            checks.expect(pc4,
                "PC4 [p.240 \"it can take up to 300 microseconds for the line to change state\"]: "
                "every counted R record has t(M2)-t(M1) >= 300us");
            checks.note("PC4: checked " + std::to_string(pc4_checked) + " records against the 300us bound "
                        "(clk_sys=28.375160MHz)");

            // ---- Main proposition: the dump, per configuration ----------
            bool pre_ok = pc1 && pc2 && pc3 && pc4;
            for (int ci = 0; ci < 5; ++ci) {
                auto& fc = frames[3 + 3 * ci + 2]; // C{ci}-Fc
                bool zero_ok = true, one_ok = true; int counted = 0;
                for (int i = 0; i < 4; ++i)
                    if (wv(r, fc.r.vr0[i]) == fc.r.target && wv(r, fc.r.vr1[i]) == fc.r.target) {
                        ++counted;
                        uint16_t pg = wv(r, fc.r.pg[i]) & 0x5500;
                        if ((pg & ~fc.out_mask) != 0) zero_ok = false;  // any dumped-line bit set = fail
                        if (fc.out_mask && (pg & fc.out_mask) != fc.out_mask) one_ok = false;
                    }
                bool established = pre_ok && counted > 0;
                checks.expect(established && zero_ok,
                    fc.label + " [p.240 Table 8-4 \"00 START Start pots (dump capacitors, start "
                    "counters)\"] opus:8-dio.c21 sol:8-dio.c19: every input line of " + fc.label +
                    " reads 0 in POTGOR after START" +
                    (established ? "" : " (preconditions not established: not graded as pass)"));
                if (fc.out_mask)
                    checks.expect(established && one_ok,
                        fc.label + "-OUT [p.240 \"the Amiga disconnects the potentiometer control "
                        "circuitry...and configures the pin for output\"]: the output-driven line of " +
                        fc.label + " still reads 1 (not this package's dump claim)" +
                        (established ? "" : " (preconditions not established: not graded as pass)"));
                if (counted > 0 && !zero_ok)
                    checks.record(fc.label + ": measured POTGOR&$5500=" +
                        std::to_string(wv(r, fc.r.pg[0]) & 0x5500) +
                        " where the page predicts " + std::to_string(fc.out_mask));
            }
        }

        if (mv == "K5")
            checks.record("K5: RX (connector B pin 5) is excluded -- no nanomig.v port reaches it "
                "in this build. LX, LY and RY "
                "are held at level 0 for the whole run (not scoped to only Fb/Fc of each "
                "configuration); PC3 is checked below for those three.");
    }

    // =====================================================================
    // The joystick program: P2, P3, P4.
    // =====================================================================
    {
        static const std::vector<StepDef> steps = baseline_steps();
        std::string bin = dir + "/prog_joy.bin";

        auto run_cfg = [&](const std::string& mvv, nmh::Config c) { return run_joy(bin, mvv, c); };

        JoyRun r = run_cfg(mv, default_cfg);
        checks.expect(r.loaded && r.released && !r.timed_out && r.steps_completed == kNSteps && r.done.has_value(),
            "JOY-FIXTURE: prog_joy loads, releases reset, completes all 54 steps and reaches DONE "
            "[not graded -- apparatus check, no claim uid]");

        if (r.loaded && !r.timed_out && r.steps_completed == kNSteps && r.done.has_value()) {
            bool cross_agree = true; std::string disagree;
            if (mv.empty()) {
                for (int ci = 1; ci < 4; ++ci) {
                    JoyRun r2 = run_cfg("", cfg_for(kConfigs[ci]));
                    bool same = r2.loaded && !r2.timed_out && r2.steps_completed == kNSteps && r2.done.has_value();
                    for (int k = 0; same && k < kNSteps; ++k) same = (r2.rec[k] == r.rec[k]);
                    if (!same) { cross_agree = false; disagree += std::string(kConfigs[ci].name) + " "; }
                }
                checks.expect(cross_agree,
                    "JOY-CONFIGS: PAL/NTSC x ECS/OCS agree word-for-word with the PAL+ECS run "
                    "[not graded -- apparatus check, no claim uid]" +
                    (cross_agree ? std::string() : (" -- disagreed: " + disagree)));
            }

            // ---- Preconditions Q1-Q4 ----------------------------------
            auto yup_a = find_steps(steps, Conn::A, "Yup");
            bool q1 = true;
            for (int k : yup_a) if (!(r.t_apply[k] < r.t_t[k])) q1 = false;
            checks.expect(q1, "Q1 [protocol]: for every Yup(A) step, t_apply(k) < t(T_k)");

            bool q2 = true;
            for (int k = 0; k < kNSteps; ++k) if ((r.rec[k][3] & 0x5500) != 0x5500) q2 = false;
            checks.expect(q2, "Q2 [p.241 \"the state of the data bit controls...\"]: POTGOR bits 14,12,10,8 "
                "read 1 (the pot lines are inputs and released) in every record");

            int n0 = 0, n1 = 1;
            bool q3 = (r.rec[n0][0] == r.rec[n1][0]) && (r.rec[n0][1] == r.rec[n1][1]);
            checks.expect(q3, "Q3 [p.281 \"Each counter is clocked by signals from two controller "
                "pins.\"]: JOY0DAT and JOY1DAT are equal in N0 and N1");

            int f1a = find_step(steps, Conn::A, "F1"), f0a = find_step(steps, Conn::A, "F0");
            int f1b = find_step(steps, Conn::B, "F1"), f0b = find_step(steps, Conn::B, "F0");
            bool q4 = ((r.rec[f1a][2] & 0x40) == 0) && ((r.rec[f0a][2] & 0x40) != 0) &&
                      ((r.rec[f1b][2] & 0x80) == 0) && ((r.rec[f0b][2] & 0x80) != 0);
            checks.expect(q4, "Q4 [p.232 \"A logic state of 1 means 'switch open.' A logic state of 0 "
                "means 'switch closed.'\"]: level 0 on pin 6 of A reads 0 in CIAAPRA bit 6 in F1(A), "
                "and level 1 reads 1 in F0(A) (and symmetrically for B, bit 7)");

            // ---- P2 anchor (section 7) ----------------------------------
            bool anchor = ((r.rec[f1a][2] & 0x40) == 0) && ((r.rec[f1a][2] & 0x80) != 0) &&
                          ((r.rec[f1b][2] & 0x80) == 0) && ((r.rec[f1b][2] & 0x40) != 0);
            checks.expect(anchor, "P2-ANCHOR [p.230 \"Port 1 uses bit 6 and port 2 uses bit 7\"; p.233]: "
                "F1(A) reads CIAAPRA bit6=0,bit7=1; F1(B) reads bit7=0,bit6=1 -- connector A is the "
                "one whose fire button is bit 6");

            // ---- P2: which register serves which connector --------------
            auto block_range = [&](Conn c) {
                std::vector<int> idx; for (int i = 0; i < kNSteps; ++i) if (steps[i].conn == c) idx.push_back(i);
                return idx;
            };
            auto blockA = block_range(Conn::A), blockB = block_range(Conn::B);
            int ref_b_for_a = r.rec[n1][1];               // JOY1DAT at N1
            int ref_a_for_b = r.rec[blockA.back()][0];     // JOY0DAT at end of block A

            bool a_unchanged_other = true, a_moves = true;
            for (int k : blockA) {
                if (r.rec[k][1] != ref_b_for_a) a_unchanged_other = false;
                if (steps[k].group != "F1" && steps[k].group != "F0" && k > 0)
                    if (r.rec[k][0] == r.rec[k - 1][0]) a_moves = false;
            }
            checks.expect(a_unchanged_other,
                "P2-BLOCKA-JOY1 [p.281 \"0=left controller pair, 1=right controller pair (four "
                "counters total).\"] opus:8-appa-joy.c12 sol:8-appa-joy.c14: JOY1DAT stays at its N1 "
                "value throughout block A");
            checks.expect(a_moves,
                "P2-BLOCKA-JOY0 [p.281] opus:8-appa-joy.c11 sol:8-appa-joy.c13: JOY0DAT differs from "
                "the record before it in every Yup/Ydown/Xup/Xdown/Buttons step of block A");

            bool b_unchanged_other = true, b_moves = true;
            for (int k : blockB) {
                if (r.rec[k][0] != ref_a_for_b) b_unchanged_other = false;
                bool first_of_block = (k == blockB.front());
                if (steps[k].group != "F1" && steps[k].group != "F0" && !first_of_block)
                    if (r.rec[k][1] == r.rec[k - 1][1]) b_moves = false;
            }
            checks.expect(b_unchanged_other,
                "P2-BLOCKB-JOY0 [p.281] opus:8-appa-joy.c11 sol:8-appa-joy.c13: JOY0DAT stays at its "
                "end-of-block-A value throughout block B");
            checks.expect(b_moves,
                "P2-BLOCKB-JOY1 [p.281] opus:8-appa-joy.c12 sol:8-appa-joy.c14: JOY1DAT differs from "
                "the record before it in every Yup/Ydown/Xup/Xdown/Buttons step of block B");

            // "four counters total": Y steps move only the high byte, X only the low.
            auto byte_moves = [&](int prev, int cur, int reg, bool high) -> bool {
                uint8_t a = high ? (r.rec[prev][reg] >> 8) : (r.rec[prev][reg] & 0xFF);
                uint8_t b = high ? (r.rec[cur][reg] >> 8)  : (r.rec[cur][reg] & 0xFF);
                return a != b;
            };
            bool four_counters = true;
            for (Conn c : {Conn::A, Conn::B}) {
                int reg = (c == Conn::A) ? 0 : 1;
                for (auto& grp : {std::string("Yup"), std::string("Ydown")})
                    for (int k : find_steps(steps, c, grp))
                        if (!byte_moves(k - 1, k, reg, true) || byte_moves(k - 1, k, reg, false)) four_counters = false;
                for (auto& grp : {std::string("Xup"), std::string("Xdown")})
                    for (int k : find_steps(steps, c, grp))
                        if (!byte_moves(k - 1, k, reg, false) || byte_moves(k - 1, k, reg, true)) four_counters = false;
            }
            checks.expect(four_counters,
                "P2-FOUR-COUNTERS [p.281 \"(four counters total)\"]: the Y steps of each connector "
                "move only its register's high byte, the X steps only its low byte");

            // ---- P3: quadrature (pins1,3=Y; pins2,4=X) -------------------
            bool p3_y = true, p3_x = true;
            for (Conn c : {Conn::A, Conn::B}) {
                int reg = (c == Conn::A) ? 0 : 1;
                for (int k = 1; k < kNSteps; ++k) {
                    if (steps[k].conn == Conn::NONE && steps[k - 1].conn == Conn::NONE) continue; // N0/N1: no transition graded here
                    Levels lo = levels_of(steps[k - 1], c), ln = levels_of(steps[k], c);
                    int sy_o = state_of(lo.up, lo.left), sy_n = state_of(ln.up, ln.left);
                    int sx_o = state_of(lo.down, lo.right), sx_n = state_of(ln.down, ln.right);
                    uint8_t yb_o = r.rec[k - 1][reg] >> 8, yb_n = r.rec[k][reg] >> 8;
                    uint8_t xb_o = r.rec[k - 1][reg] & 0xFF, xb_n = r.rec[k][reg] & 0xFF;
                    int dy = (sy_n == (sy_o + 1) % 4) ? 1 : (sy_n == (sy_o + 3) % 4) ? -1 : 0;
                    int dx = (sx_n == (sx_o + 1) % 4) ? 1 : (sx_n == (sx_o + 3) % 4) ? -1 : 0;
                    if (yb_n != (uint8_t)(yb_o + dy)) p3_y = false;
                    if (xb_n != (uint8_t)(xb_o + dx)) p3_x = false;
                    if ((r.rec[k][reg] & 0x0300) >> 8 != d1d0_of(sy_n)) p3_y = false;
                    if ((r.rec[k][reg] & 0x0003) != d1d0_of(sx_n)) p3_x = false;
                }
            }
            checks.expect(p3_y,
                "P3-Y [p.281 \"Mouse counter usage: (pins 1,3=Yclock, pins 2,4=Xclock)\"; Figure 8-2] "
                "opus:8-appa-joy.c17 sol:8-appa-joy.c19: every step's Y-counter byte and low bits "
                "(9,8) follow pins 1 and 3 through Figure 8-2's state table, on both connectors and "
                "both directions");
            checks.expect(p3_x,
                "P3-X [p.281] opus:8-appa-joy.c18 sol:8-appa-joy.c20: every step's X-counter byte and "
                "low bits (1,0) follow pins 2 and 4 through Figure 8-2's state table, on both "
                "connectors and both directions");

            // ---- P4: Table 8-1 buttons ------------------------------------
            bool p4_pin3 = true, p4_pin4 = true, p4_pin1 = true, p4_pin2 = true;
            for (Conn c : {Conn::A, Conn::B}) {
                int reg = (c == Conn::A) ? 0 : 1;
                for (int k = 0; k < kNSteps; ++k) {
                    Levels lv = levels_of(steps[k], c);
                    uint16_t v = r.rec[k][reg];
                    bool bit9 = (v >> 9) & 1, bit8 = (v >> 8) & 1, bit1 = (v >> 1) & 1, bit0 = v & 1;
                    if (bit9 != !lv.left) p4_pin3 = false;
                    if (bit1 != !lv.right) p4_pin4 = false;
                    if ((bit9 ^ bit8) != (uint16_t)(!lv.up)) p4_pin1 = false;
                    if ((bit1 ^ bit0) != (uint16_t)(!lv.down)) p4_pin2 = false;
                }
            }
            checks.expect(p4_pin3,
                "P4-PIN3 [p.228 Table 8-1 \"3 Left VQ-pulse Left button Button 1\"; p.281 \"Left: Y1\"] "
                "opus:8-tbl8-1.c8 opus:8-tbl8-1.c9 sol:8-pin.c9 sol:8-pin.c10: bit 9 reads 1 exactly "
                "when pin 3 is at level 0, on both connectors, in every record");
            checks.expect(p4_pin4,
                "P4-PIN4 [p.228 Table 8-1 \"4 Right HQ-pulse Right button Button 2\"; p.281 \"Right: "
                "X1\"] opus:8-tbl8-1.c12 opus:8-tbl8-1.c13 sol:8-pin.c13 sol:8-pin.c14: bit 1 reads 1 "
                "exactly when pin 4 is at level 0, on both connectors, in every record");
            checks.expect(p4_pin1,
                "P4-PIN1 [p.228 Table 8-1 \"1 Forward V-pulse Button 3**\"; p.281 \"Forward: Y1 xor "
                "Y0\"] opus:8-tbl8-1.c3 sol:8-pin.c3: (bit9 xor bit8) reads 1 exactly when pin 1 is "
                "at level 0, on both connectors, in every record");
            checks.expect(p4_pin2,
                "P4-PIN2 [p.281 \"Back: X1 xor X0\"]: (bit1 xor bit0) reads 1 exactly when pin 2 is "
                "at level 0, on both connectors, in every record (Table 8-1 carries no button on pin "
                "2 -- this checks that none reads there either)");
        }
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
