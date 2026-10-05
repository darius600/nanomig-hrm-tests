// test_appK_autoconfig.cpp -- appK-autoconfig. See SPECIFICATION.md for
// every assertion's proposition, program, observed value and expected
// value. This file only implements what SPECIFICATION.md already
// committed to.
//
// Every precondition is printed as its own line before the proposition that
// rests on it; a proposition whose precondition printed FAIL is recorded as
// not graded and never reaches expect().
//
// NMH_MUTATE selects a mutated program in place of its normal counterpart,
// one assertion or precondition at a time; none is exercised by a plain
// `make test`. Under a mutation each program set runs at the first CPU
// setting only.
//
// MUTATION FIXTURE none impossible not exercised, by a scope decision: an apparatus check with no claim and no page-predicted contrast; a program that skips the vector write would falsify it and the package does not drive one
// MUTATION P1-MARKERS none impossible not exercised, by a scope decision: the uniqueness of the V1 write and of the V0 write before t_M1 are apparatus anchors that no page predicts a contrast for; a program that writes V1 twice would falsify them and the package does not drive one
// MUTATION P1-LEDCAL stimulus red the write that switches the power LED to bright (CIAAPRA bit 1 = 0) replaced by a write that keeps bit 1 = 1, RESET at A replaced by NOP so the run completes (prog_r1_nocal), via NMH_MUTATE=P1-LEDCAL: the unchanged calibration comparator, fed it, goes red (the LED never leaves its dim level)
// MUTATION P1-DISPANCHOR stimulus red COLOR00 preset to C1'=$0C3F instead of C1, RESET at A replaced by NOP so the run completes (prog_r1_c1prime), via NMH_MUTATE=P1-DISPANCHOR: the unchanged anchor comparator, still expecting C1, goes red -- F_ref shows C1', not C1; F_ref ends before t_M1, so the NOP does not reach the comparator
// MUTATION P1-NOEXCEPTION none impossible unavailable from this oracle: the only contrast is RESET executed in user mode, which rests on 680x0 privilege-violation behaviour that no cited manual page predicts (Motorola, not the manual)
// MUTATION P1b stimulus red RESET at A replaced by NOP (prog_r1a), via NMH_MUTATE=P1A: the unchanged "power LED leaves bright" comparator, fed it, goes red (no LED change occurs); the run completes
// MUTATION P2-WITNESS stimulus red RESET at A replaced by NOP (prog_r1a), via NMH_MUTATE=P1A: the unchanged "reset witnessed" comparator, fed it, goes red (no LED change occurs); the run completes
// MUTATION P2 none impossible unfalsifiable by a live page-predicted stimulus at this boundary: p.223 predicts that the instruction after RESET executes, so a stimulus that removes the witness instruction (the C2 write) leaves the clause true and turns only the witness red; no input of this boundary makes the CPU stop after RESET while keeping the witness
// MUTATION GATE-G-PRECOND none impossible unavailable from this oracle: register 08 bit 6 is a property of the board the harness offers; no stimulus changes what the board reports about itself
// MUTATION P3-IMG-PRECOND none impossible not exercised, by a scope decision: run liveness (loaded, window found) and the board type, size and memory flag are anchors of the offered board; no page predicts a contrast
// MUTATION P3-IMG1 stimulus red step b writes register 48 (configures immediately) in place of the 4A write (prog_r3_c48first), via NMH_MUTATE=P3-IMG1: the unchanged comparator goes red (IMG1 differs from IMG0)
// MUTATION P3-IMG2 stimulus red steps c and d skipped, board never configured (prog_r3_nocfg), via NMH_MUTATE=P3-IMG2: the unchanged comparator goes red (IMG2 equals IMG0)
// MUTATION P3-CONTENT-PRECOND none impossible not exercised, by a scope decision: the board-type, size, memory-flag and scratch-write anchors are properties of the offered board and of the program order; no page predicts a contrast
// MUTATION P3-CONTENT stimulus red step c (the register-48 write) left out, step d kept (prog_r3_no48): the board is never mapped in, so R0/R1 read back at their unanswered default instead of P0/P1V, via NMH_MUTATE=P3-CONTENT
// MUTATION P3-NOTOTHER-PRECOND none impossible not exercised, by a scope decision: same anchors as P3-CONTENT-PRECOND for the run R3n
// MUTATION P3-NOTOTHER-ASSIGNED stimulus red step c (the register-48 write) left out of R3n, step d kept (prog_r3_n_no48): the board is never mapped in, so the readback at B1 is the unanswered default instead of P0/P1V, via NMH_MUTATE=P3-NOTOTHER-ASSIGNED
// MUTATION P3-BASE-PRECOND none impossible not exercised, by a scope decision: same anchors as P3-CONTENT-PRECOND for the runs R3' and R3n'
// MUTATION P3-BASE-ASSIGNED stimulus red step c (the register-48 write) left out of R3', step d kept (prog_r3_swap_no48): the board is never mapped in at B2, so the readback at B2 is the unanswered default instead of P0/P1V, via NMH_MUTATE=P3-BASE-ASSIGNED
// MUTATION P3-CONTROL-IO-PRECOND none impossible not exercised, by a scope decision: same anchors as P3-CONTENT-PRECOND for the run of the control
// MUTATION P3-CONTROL-IO stimulus red the write/readback aimed at the configured base B1 itself instead of at $00A00000 (prog_r3_io_self), via NMH_MUTATE=P3-CONTROL-IO: the unchanged "R2 != P2V, or no answer" comparator goes red (a configured memory board answers at its base, p.433)
// MUTATION P3-CONTROL-UNCONF-PRECOND none impossible not exercised, by a scope decision: same anchors as P3-CONTENT-PRECOND for the run of the control
// MUTATION P3-CONTROL-UNCONF stimulus red the board configured at $00400000 (steps b-d with the bases exchanged) and the write/readback aimed at $00400000 (prog_r3_unconf_self), via NMH_MUTATE=P3-CONTROL-UNCONF: the unchanged comparator goes red (a configured memory board answers at its base, p.433)
// MUTATION P6-CONFIG none impossible not exercised, by a scope decision: the configuration took effect before RESET in R6 and R6m; a program that leaves out the register-48 write (as P3-CONTENT's) would falsify it and the package does not drive one in the R6 family
// MUTATION P6-WITNESS stimulus red RESET at A replaced by NOP in R6 and R6m (prog_r6a, prog_r6am), via NMH_MUTATE=P6: the unchanged "LED change observed after t_M1" comparator goes red (no LED changes); both runs complete
// MUTATION P6 none impossible unfalsifiable by a live page-predicted stimulus at this boundary: the page's only contrasting case for P6 (RESET replaced by NOP, R6a/R6am) also removes P6's own reset-witness precondition P6-WITNESS, so grading P6 against it reports not graded, not red, through P6's own clause (SPECIFICATION.md section 5.2 P6, "6. Contrasting cases"); a contrast that keeps the witness would need a RESET that does not drive /BUSRST, which no input of this boundary offers; P6 is green with no live mutation
// MUTATION P3-NOTOTHER stimulus green already red on the unmutated stimulus, which a stimulus mutation cannot make more red; the same "R2 != P2V, or no answer" comparator, fed the board configured at $00200000 and read at $00A00000 (prog_r3_io) or the board left unconfigured and read at $00400000 (prog_r3_unconf_n), prints PASS: P3-CONTROL-IO and P3-CONTROL-UNCONF
// MUTATION P3-BASE stimulus green already red on the unmutated stimulus; the same "R2 != P2V, or no answer" comparator, fed the same two control stimuli, prints PASS: P3-CONTROL-IO and P3-CONTROL-UNCONF
// MUTATION GATE-G stimulus green already red on the unmutated stimulus; the same "differs from IMG0, or no answer" comparator (no completion marker, or an image that differs from IMG0), fed run R3 (the board configured by the byte write to register 48 in place of the write to the shut-up address 4C), prints PASS: P3-IMG2
//
// GATE-G, P3-NOTOTHER and P3-BASE are red: GATE-G through p.436, P3-NOTOTHER
// and P3-BASE through pp.391 and 435. Each has green controls on the same
// comparator instead of a mutation of its own: GATE-G the comparator of
// P3-IMG2 (a PIC that leaves $00E8xxxx after the configuring write to 48);
// P3-NOTOTHER and P3-BASE the comparator of P3-CONTROL-IO and
// P3-CONTROL-UNCONF (the board configured normally and read at $00A00000;
// the board left unconfigured and read at $00400000). Every control carries
// its own mutation record above.
//
// P4 and P5 carry no assertion: SPECIFICATION.md section 1 declines
// K-0840 and K-0838 whole (no Zorro III device at this boundary). Not
// exercised.
//
// P1a carries no assertion in this package. Its precondition, the
// drive-LED calibration (SPECIFICATION.md section 5.1.5), is measured on
// every run as P1a-FDDCAL: the drive-LED port changes level between the
// motor off-latch/on-latch sequence and t_M1. The P1a record names what
// that measurement printed; FIXTURE-FDD prints whether the port changed
// level at all (NM-HRM-046).

#include <array>
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

namespace {

// ---- nanomig.i / common.i / program layout, restated ----------------
constexpr uint32_t kResults   = 0x0C0000;
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;

constexpr uint64_t kBootBudget   = 6'000'000;
constexpr uint64_t kSimpleBudget = 6'000'000;   // gate G / P3: safety ceiling only, never the oracle
constexpr uint64_t kResetBudget  = 9'000'000;   // P1/P2/P6: up to ~6 frames of waiting

constexpr uint32_t kMK0     = kResults + 0;
constexpr uint32_t kMK_E    = kResults + 2;
constexpr uint32_t kVECOK   = kResults + 4;
constexpr uint32_t kMK_NEXT = kResults + 6;

constexpr uint16_t kV0        = 0xAAAA;
constexpr uint16_t kVE        = 0xDEAD;
constexpr uint16_t kVecokYes  = 0xFACE;

// R1-family (prog_r1*.s): the power-LED calibration markers come first,
// so the test can tell the dim phase from the bright phase directly from
// the port trace instead of assuming a starting level (SPECIFICATION.md
// section 5.1.5).
constexpr uint32_t kMkCalDim    = kMK_NEXT + 0;
constexpr uint32_t kMkCalBright = kMK_NEXT + 2;
constexpr uint32_t kMK1 = kMK_NEXT + 4;
constexpr uint16_t kCalDimMark = 0xCA10, kCalBrightMark = 0xCA20;
constexpr uint16_t kV1 = 0xBBBB;
constexpr uint16_t kC1 = 0x0C30, kC2 = 0x03AC, kCE = 0x0F0F;

// gate G / P3-family IMG base (prog_rg*.s, prog_r3*.s all start their own
// IMG0 at MK_NEXT)
constexpr uint32_t kImgAtMkNext = kMK_NEXT;

// P3-family (prog_r3*.s)
constexpr uint32_t kR3Img0    = kMK_NEXT + 0;
constexpr uint32_t kR3Img1    = kMK_NEXT + 64;
constexpr uint32_t kR3Img2    = kMK_NEXT + 128;
constexpr uint32_t kR3R0      = kMK_NEXT + 192;
constexpr uint32_t kR3R1      = kMK_NEXT + 194;
constexpr uint32_t kR3Scratch = kMK_NEXT + 196;
constexpr uint32_t kR3R2      = kMK_NEXT + 198;
constexpr uint16_t kP0v = 0xA55A, kP1v = 0x3CC3, kP2v = 0x5AA5;

// R6-family (prog_r6*.s)
constexpr uint32_t kR6Mk1  = kMK_NEXT + 0;
constexpr uint32_t kR6Img0 = kMK_NEXT + 4;
constexpr uint32_t kR6R0   = kMK_NEXT + 68;
constexpr uint32_t kR6Img3 = kMK_NEXT + 72;
constexpr uint32_t kR6R3   = kMK_NEXT + 72; // same slot as IMG3; mutually exclusive
constexpr uint16_t kP0 = 0xA55A, kP0Prime = 0x6996;

// ---- helpers ----------------------------------------------------------

struct Color { int r, g, b; };
Color decode_color(uint16_t reg) {
    return { (reg >> 8) & 0xF, (reg >> 4) & 0xF, reg & 0xF };
}
bool pixel_is(const nmh::VideoSample& s, Color c) {
    return s.r == c.r && s.g == c.g && s.b == c.b;
}
bool pixel_is_black(const nmh::VideoSample& s) { return s.r == 0 && s.g == 0 && s.b == 0; }

nmh::Config base_cfg() {
    nmh::Config cfg;
    // fastram_config: the harness documents only 0 ("no Zorro II fast RAM
    // offered"). Gate G's own read of register 00 is the measurement of
    // what this setting produces (SPECIFICATION.md section 8.3).
    cfg.fastram_config = 1;
    // floppy_config is left at the harness's own documented default (one
    // drive, DF0); the propositions need only drive 0.
    return cfg;
}

std::array<uint16_t, 32> read_img(nmh::NanomigHarness& h, uint32_t base) {
    std::array<uint16_t, 32> img{};
    for (int i = 0; i < 32; ++i) {
        auto v = h.peek_logical(base + 2 * i);
        img[i] = v ? *v : 0xFFFF;
    }
    return img;
}
bool img_equal(const std::array<uint16_t, 32>& a, const std::array<uint16_t, 32>& b) {
    for (int i = 0; i < 32; ++i)
        if ((a[i] & 0xF000) != (b[i] & 0xF000)) return false;
    return true;
}

int count_writes(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr, uint16_t value) {
    int n = 0;
    for (const auto& a : log)
        if (a.is_write && a.word_addr == byte_addr / 2 && a.data == value) ++n;
    return n;
}
std::optional<uint64_t> first_write_cycle(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr, uint16_t value) {
    for (const auto& a : log)
        if (a.is_write && a.word_addr == byte_addr / 2 && a.data == value) return a.cycle;
    return std::nullopt;
}
std::vector<uint64_t> write_cycles(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr, uint16_t value) {
    std::vector<uint64_t> out;
    for (const auto& a : log)
        if (a.is_write && a.word_addr == byte_addr / 2 && a.data == value) out.push_back(a.cycle);
    return out;
}

// "Every" needs the whole population: an empty set is never true.
template <class V, class F>
bool all_of_pop(const V& v, F f) {
    if (v.empty()) return false;
    for (const auto& x : v) if (!f(x)) return false;
    return true;
}

// Register 00's physical byte (not complemented, section 4): bits7-4 from
// the word at a pair's first address, bits3-0 from the word at its
// second. Type = bits 7-6, memory flag = bit 5 ("If this bit is set, the
// PIC's memory will be linked into the system free pool", section 4),
// size = bits 2-0.
struct Reg00 { int type; bool memory; int size; };
Reg00 decode_reg00(const std::array<uint16_t, 32>& img0) {
    int nib_hi = (img0[0] >> 12) & 0xF;
    int nib_lo = (img0[1] >> 12) & 0xF;
    int byte = (nib_hi << 4) | nib_lo;
    return { (byte >> 6) & 0x3, ((byte >> 5) & 1) != 0, byte & 0x7 };
}
// Zorro II (type 11), memory flag set, size field 110 = 2 megabytes
// (p.432-433): the board the two 2 MB bases of section 4 are chosen for.
bool board_is_zorro2_2mb_memory(const std::array<uint16_t, 32>& img0) {
    Reg00 r = decode_reg00(img0);
    return r.type == 0b11 && r.memory && r.size == 0b110;
}

// Runs to the completion marker or to `budget` clk_sys cycles, whichever
// comes first -- `budget` is a cycle ceiling only, a build parameter, not
// an oracle (SPECIFICATION.md section 8.3). Section 2.2's own "no answer"
// definition is the second vertical-sync leading edge since the run
// started; that is checked here against the run's own video capture
// (the anchor a "no answer" verdict rests on), not assumed from a cycle
// count: `t_v2_found` is true once the run has demonstrably run for at
// least that long, whether or not the marker ever appeared.
struct DoneWait { std::optional<uint64_t> done; bool t_v2_found = false; };
DoneWait wait_done_or_second_vsync(nmh::NanomigHarness& h, uint32_t done_word_addr, uint16_t done_value, uint64_t budget) {
    DoneWait out;
    size_t seen = 0;
    int vs_edges = 0;
    bool prev_vs = h.video().empty() ? false : h.video().back().vs;
    for (uint64_t i = 0; i < budget && !out.done; ++i) {
        h.run_cycles(1);
        for (; seen < h.ram_log().size(); ++seen) {
            const auto& a = h.ram_log()[seen];
            if (a.is_write && a.word_addr == done_word_addr && a.data == done_value) out.done = a.cycle;
        }
        if (!h.video().empty()) {
            bool vs = h.video().back().vs;
            if (vs && !prev_vs) ++vs_edges;
            prev_vs = vs;
        }
    }
    out.t_v2_found = vs_edges >= 2;
    return out;
}

// ---- R1-family capture (P1, P2) ---------------------------------------

struct LedTrans { uint64_t cycle; bool pwr; bool fdd; };

struct R1Out {
    bool loaded = false;
    bool vecok = false;
    std::optional<uint64_t> t_m1;
    int v1_writes = 0;
    int v0_writes_before_m1 = 0;
    int v0_writes_after_m1 = 0;
    bool m0_seen = false;
    bool no_exception = true;
    bool fref_ok = false;
    bool led_calib_ok = false;    // both calibration phases held their level for their whole
                                   // marker-bounded duration, and the two levels differ
    bool l_dim = false;            // the calibrated power-LED levels (valid when led_calib_ok)
    bool l_bright = false;
    bool pwr_at_m1 = false;        // the power-LED level at t_M1
    std::optional<uint64_t> t_v2;
    uint64_t t_c2 = 0;
    bool c2_pixel_seen = false;
    bool pwr_change_seen = false;
    uint64_t pwr_change_cycle = 0;
    bool pwr_change_to_dim = false;   // the first change after t_M1 goes to the calibrated L_dim
    bool pwr_change_in_window = false; // pwr_change_cycle in [t_m1, t_c2]
    bool fdd_ever_changed = false; // any time in the run
    int fdd_changes_before_m1 = 0; // drive-LED level changes in (t_CALBRIGHT, t_M1)
    bool done = false;             // the completion marker was written
};

R1Out run_r1(const std::string& bin, uint8_t cpu_config, uint16_t bg_color) {
    R1Out out;
    nmh::Config cfg = base_cfg();
    cfg.cpu_config = cpu_config;
    nmh::NanomigHarness h(cfg);
    out.loaded = h.load_program(bin) > 0;
    if (!out.loaded) return out;
    h.boot(kBootBudget);

    std::vector<LedTrans> trans;
    bool prev_pwr = h.ports()->pwr_led;
    bool prev_fdd = h.ports()->fdd_led;
    trans.push_back({0, prev_pwr, prev_fdd});
    for (uint64_t i = 0; i < kResetBudget; ++i) {
        h.run_cycles(1);
        bool pwr = h.ports()->pwr_led;
        bool fdd = h.ports()->fdd_led;
        if (pwr != prev_pwr || fdd != prev_fdd) {
            trans.push_back({h.cycles(), pwr, fdd});
            if (fdd != prev_fdd) out.fdd_ever_changed = true;
            prev_pwr = pwr; prev_fdd = fdd;
        }
    }

    auto vecok_w = h.peek_logical(kVECOK);
    out.vecok = vecok_w && *vecok_w == kVecokYes;
    out.m0_seen = count_writes(h.ram_log(), kMK0, kV0) >= 1;
    out.v1_writes = count_writes(h.ram_log(), kMK1, kV1);
    out.t_m1 = first_write_cycle(h.ram_log(), kMK1, kV1);
    out.done = count_writes(h.ram_log(), kDoneAddr, kDoneValue) >= 1;

    if (out.t_m1) {
        for (const auto& a : h.ram_log()) {
            if (a.is_write && a.word_addr == kMK0 / 2 && a.data == kV0) {
                if (a.cycle < *out.t_m1) ++out.v0_writes_before_m1;
                else if (a.cycle > *out.t_m1) ++out.v0_writes_after_m1;
            }
        }
    }

    for (const auto& a : h.ram_log())
        if (a.is_write && a.word_addr == kMK_E / 2 && a.data == kVE) out.no_exception = false;
    {
        Color ce = decode_color(kCE);
        for (const auto& s : h.video())
            if (pixel_is(s, ce)) { out.no_exception = false; break; }
    }

    // Power-LED calibration (SPECIFICATION.md section 5.1.5): the LED shows
    // one level when the dim phase starts (the MK_CALDIM marker) and a
    // different level at t_M1, and the power LED makes exactly one
    // transition in (t_CALDIM, t_M1): the switch to bright. The levels are
    // read from the port trace, no timing margin is assumed.
    auto t_caldim = first_write_cycle(h.ram_log(), kMkCalDim, kCalDimMark);
    auto t_calbright = first_write_cycle(h.ram_log(), kMkCalBright, kCalBrightMark);
    if (t_caldim && out.t_m1 && *t_caldim < *out.t_m1) {
        bool level_before = false, level_after = false;
        int pwr_transitions = 0;
        bool prev = false, have_prev = false;
        for (auto& t : trans) {
            if (t.cycle <= *t_caldim) {
                level_before = t.pwr;
            } else if (t.cycle < *out.t_m1 && have_prev && t.pwr != prev) {
                ++pwr_transitions;
            }
            if (t.cycle < *out.t_m1) { level_after = t.pwr; }
            prev = t.pwr; have_prev = true;
        }
        out.led_calib_ok = pwr_transitions == 1 && level_before != level_after;
        out.l_dim = level_before;
        out.l_bright = level_after;
    }
    // The drive-LED calibration of section 5.1.5, from the same trace: the
    // drive-LED port changes level at least once between the MK_CALBRIGHT
    // marker and t_M1, the span that holds the motor off-latch and on-latch.
    if (t_calbright && out.t_m1 && *t_calbright < *out.t_m1) {
        bool prev_fdd_level = false; bool have = false;
        for (auto& t : trans) {
            if (t.cycle <= *t_calbright) { prev_fdd_level = t.fdd; have = true; }
            else if (t.cycle < *out.t_m1 && have && t.fdd != prev_fdd_level) { ++out.fdd_changes_before_m1; prev_fdd_level = t.fdd; }
        }
    }

    if (!out.t_m1) return out;

    // The power-LED level at t_M1: the last transition at or before it.
    for (auto& t : trans)
        if (t.cycle <= *out.t_m1) out.pwr_at_m1 = t.pwr;

    auto frames = nmh::find_frames(h.video());
    int fref_idx = -1;
    for (size_t i = 0; i < frames.size(); ++i)
        if (frames[i].end <= *out.t_m1) fref_idx = (int)i;
    if (fref_idx >= 0) {
        auto fr = frames[fref_idx];
        Color c1 = decode_color(bg_color);
        Color c2 = decode_color(kC2);
        bool has_c1 = false, all_ok = true, no_c2_before = true;
        for (size_t i = fr.begin; i < fr.end && i < h.video().size(); ++i) {
            const auto& s = h.video()[i];
            if (pixel_is(s, c1)) has_c1 = true;
            else if (!pixel_is_black(s)) all_ok = false;
        }
        for (uint64_t i = 0; i < *out.t_m1 && i < h.video().size(); ++i)
            if (pixel_is(h.video()[i], c2)) { no_c2_before = false; break; }
        out.fref_ok = has_c1 && all_ok && no_c2_before;
    }

    int after = 0;
    for (auto& fr : frames) {
        if (fr.begin > *out.t_m1) {
            ++after;
            if (after == 2) { out.t_v2 = fr.begin; break; }
        }
    }
    uint64_t search_end = out.t_v2 ? *out.t_v2 : (h.video().empty() ? *out.t_m1 : h.video().size() - 1);
    out.t_c2 = search_end;
    {
        Color c2 = decode_color(kC2);
        for (uint64_t i = *out.t_m1 + 1; i <= search_end && i < h.video().size(); ++i)
            if (pixel_is(h.video()[i], c2)) { out.c2_pixel_seen = true; out.t_c2 = i; break; }
    }

    for (auto& t : trans) {
        if (t.cycle > *out.t_m1) {
            out.pwr_change_seen = true;
            out.pwr_change_cycle = t.cycle;
            out.pwr_change_to_dim = t.pwr == out.l_dim;
            out.pwr_change_in_window = t.cycle <= out.t_c2;
            break;
        }
    }

    return out;
}

// ---- gate G / P3 capture (no frame waits, DONE-or-second-vsync bounded)

struct SimpleImgOut {
    bool loaded = false;
    std::optional<uint64_t> done;
    bool t_v2_found = false;
    std::array<uint16_t, 32> img{};
};

SimpleImgOut run_simple_img(const std::string& bin, uint32_t img_base) {
    SimpleImgOut out;
    nmh::Config cfg = base_cfg();
    nmh::NanomigHarness h(cfg);
    out.loaded = h.load_program(bin) > 0;
    if (!out.loaded) return out;
    h.boot(kBootBudget);
    auto w = wait_done_or_second_vsync(h, kDoneAddr / 2, kDoneValue, kSimpleBudget);
    out.done = w.done;
    out.t_v2_found = w.t_v2_found || out.done.has_value();
    if (out.done) out.img = read_img(h, img_base);
    return out;
}

struct R3Out {
    bool loaded = false;
    std::optional<uint64_t> done;
    bool t_v2_found = false;
    std::array<uint16_t, 32> img0{}, img1{}, img2{};
    std::optional<uint16_t> r0, r1, r2;
    bool alias_before_r0 = false; // a P0/P1 write below $200000 before R0's store
    bool scratch_before_r0 = false; // section 5.2 P3.5: scratch write between the writes and reads
    bool scratch_before_r2 = false;
};

R3Out run_r3(const std::string& bin) {
    R3Out out;
    nmh::Config cfg = base_cfg();
    nmh::NanomigHarness h(cfg);
    out.loaded = h.load_program(bin) > 0;
    if (!out.loaded) return out;
    h.boot(kBootBudget);
    auto w = wait_done_or_second_vsync(h, kDoneAddr / 2, kDoneValue, kSimpleBudget);
    out.done = w.done;
    out.t_v2_found = w.t_v2_found || out.done.has_value();
    // Every slot is read from the harness's memory regardless of whether
    // this run's own DONE mark appeared: img0 (step a), img1 (step b) and
    // r0/r1 (step d) are always written before that run's own last,
    // possibly-unanswered action (section 2.2), so they are trustworthy
    // outright. The slot written *by* that last action (img1 for R3c,
    // img2 for R3 and its base-exchange/no-config contrasts, r2 for R3n
    // and its contrast) is only meaningful when out.done is set; callers
    // that grade "X, or no answer" check out.done themselves rather than
    // trusting a default-initialised (zero) slot as a real answer.
    out.img0 = read_img(h, kR3Img0);
    out.img1 = read_img(h, kR3Img1);
    out.img2 = read_img(h, kR3Img2);
    out.r0 = h.peek_logical(kR3R0);
    out.r1 = h.peek_logical(kR3R1);
    out.r2 = h.peek_logical(kR3R2);

    auto r0_cycle = first_write_cycle(h.ram_log(), kR3R0, out.r0 ? *out.r0 : 0xFFFF);
    for (const auto& a : h.ram_log()) {
        if (!a.is_write) continue;
        uint32_t byte = a.word_addr * 2;
        if (byte >= 0x200000) continue;
        if (r0_cycle && a.cycle >= *r0_cycle) continue;
        if (a.data == kP0v || a.data == kP1v) out.alias_before_r0 = true;
    }

    auto scratch_cycles = write_cycles(h.ram_log(), kR3Scratch, 0x0000);
    if (r0_cycle)
        for (auto c : scratch_cycles) if (c < *r0_cycle) { out.scratch_before_r0 = true; break; }
    // The scratch write precedes the read of R2 in program order. When the
    // read is answered, its store of R2 bounds it. When no R2 was stored, the
    // order has nothing to bound: no page fixes what an unanswered access
    // does, so a run that never completes the preceding write is not a
    // failure of the order, and the condition counts as met.
    auto r2_cycle = (out.done && out.r2) ? first_write_cycle(h.ram_log(), kR3R2, *out.r2) : std::nullopt;
    if (r2_cycle) {
        for (auto c : scratch_cycles) if (c < *r2_cycle) { out.scratch_before_r2 = true; break; }
    } else {
        out.scratch_before_r2 = true;
    }

    return out;
}

bool r3_live(const R3Out& r) { return r.loaded && r.t_v2_found; }
// R0 = P0 and R1 = P1V: the board answers at the base it was given.
bool r3_assigned(const R3Out& r) {
    return r.r0 && *r.r0 == kP0v && r.r1 && *r.r1 == kP1v;
}
// What the write/readback at the other base measured: the completion marker
// and the stored R2.
void note_r2(Checks& c, const char* what, const R3Out& r) {
    char buf[160];
    std::snprintf(buf, sizeof buf, "%s: completion marker %s, R2 = $%04X (P2 = $%04X)", what,
                  r.done ? "written" : "absent", (unsigned)r.r2.value_or(0), (unsigned)kP2v);
    c.note(buf);
}
// "R2 != P2V, or no answer": the board does not answer at the address read.
bool r3_no_answer_or_other(const R3Out& r) {
    return !r.done || r.r2.value_or(0x0000) != kP2v;
}

// ---- R6-family capture (P6: reuses P1's steps a-d, then RESET + tail) -

struct R6Out {
    bool loaded = false;
    std::optional<uint64_t> t_m1;
    bool led_witness = false;   // at least one LED change observed in (t_m1, t_v2]
    bool no_exception = true;
    std::optional<uint64_t> done;
    std::array<uint16_t, 32> img0{}, img3{};
    std::optional<uint16_t> r0, r3;
    bool config_took_effect = false; // R0 == P0
};

R6Out run_r6(const std::string& bin, uint8_t cpu_config, bool mem_ending) {
    R6Out out;
    nmh::Config cfg = base_cfg();
    cfg.cpu_config = cpu_config;
    nmh::NanomigHarness h(cfg);
    out.loaded = h.load_program(bin) > 0;
    if (!out.loaded) return out;
    h.boot(kBootBudget);

    std::vector<LedTrans> trans;
    bool prev_pwr = h.ports()->pwr_led;
    bool prev_fdd = h.ports()->fdd_led;
    trans.push_back({0, prev_pwr, prev_fdd});
    size_t seen = 0;
    for (uint64_t i = 0; i < kResetBudget; ++i) {
        h.run_cycles(1);
        bool pwr = h.ports()->pwr_led;
        bool fdd = h.ports()->fdd_led;
        if (pwr != prev_pwr || fdd != prev_fdd) {
            trans.push_back({h.cycles(), pwr, fdd});
            prev_pwr = pwr; prev_fdd = fdd;
        }
        for (; seen < h.ram_log().size(); ++seen) {
            const auto& a = h.ram_log()[seen];
            if (a.is_write && a.word_addr == kDoneAddr / 2 && a.data == kDoneValue) out.done = a.cycle;
        }
    }

    for (const auto& a : h.ram_log())
        if (a.is_write && a.word_addr == kMK_E / 2 && a.data == kVE) out.no_exception = false;
    {
        Color ce = decode_color(kCE);
        for (const auto& s : h.video())
            if (pixel_is(s, ce)) { out.no_exception = false; break; }
    }

    out.t_m1 = first_write_cycle(h.ram_log(), kR6Mk1, kV1);
    if (out.t_m1) {
        auto frames = nmh::find_frames(h.video());
        int after = 0;
        uint64_t t_v2 = h.video().empty() ? *out.t_m1 : h.video().size() - 1;
        for (auto& fr : frames) {
            if (fr.begin > *out.t_m1) { ++after; if (after == 2) { t_v2 = fr.begin; break; } }
        }
        for (auto& t : trans)
            if (t.cycle > *out.t_m1 && t.cycle <= t_v2) { out.led_witness = true; break; }
    }

    out.img0 = read_img(h, kR6Img0);
    out.r0 = h.peek_logical(kR6R0);
    out.config_took_effect = out.r0 && *out.r0 == kP0;

    if (out.done) {
        if (mem_ending) out.r3 = h.peek_logical(kR6R3);
        else out.img3 = read_img(h, kR6Img3);
    }
    return out;
}

// The names of the preconditions that printed FAIL, for a "not graded" record.
std::string failed_names(std::initializer_list<std::pair<const char*, bool>> pre) {
    std::string out;
    for (const auto& p : pre)
        if (!p.second) out += (out.empty() ? "" : ", ") + std::string(p.first);
    return out;
}

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";
    const bool is_tg68k = std::string(NMH_XSTR(NANOMIG_CORE_NAME)) == "tg68k";

    // CPU settings this core runs (nanomig.v cpu_config, 00=68000,
    // 01=68010, 11=68020; the harness's own Config comment: "00 = 68000
    // core behaviour"). p.394 speaks of "the 680x0 RESET instruction"; core
    // A (fx68k) implements only the 68000, core B (TG68K) the three.
    std::vector<uint8_t> settings = is_tg68k ? std::vector<uint8_t>{0, 1, 3} : std::vector<uint8_t>{0};
    // Under a mutation, one CPU setting is enough to see the predicted
    // stimulus-red outcome: the normal (NMH_MUTATE unset) run covers every
    // setting and is unaffected by this.
    if (!mv.empty()) settings = {settings[0]};

    auto run_r1_set = [&](const char* prog, uint16_t bg) {
        std::vector<R1Out> v;
        for (uint8_t s : settings) v.push_back(run_r1(dir + "/" + prog + ".bin", s, bg));
        return v;
    };
    auto note_mutant_done = [&](const char* what, bool all_done) {
        checks.note(std::string("NMH_MUTATE=") + mv + ": the mutated run (" + what + ") wrote the completion marker: " +
                    (all_done ? "yes" : "no"));
    };

    // The unmutated R1 run set feeds every R1-family line except the one a
    // selector replaces.
    const std::vector<R1Out> r1_base = run_r1_set("prog_r1", kC1);
    std::vector<R1Out> r1_nop, r1_nocal, r1_disp;
    if (mv == "P1A")               { r1_nop   = run_r1_set("prog_r1a", kC1);
                                     note_mutant_done("prog_r1a", all_of_pop(r1_nop, [](const R1Out& r) { return r.done; })); }
    if (mv == "P1-LEDCAL")         { r1_nocal = run_r1_set("prog_r1_nocal", kC1);
                                     note_mutant_done("prog_r1_nocal", all_of_pop(r1_nocal, [](const R1Out& r) { return r.done; })); }
    if (mv == "P1-DISPANCHOR")     { r1_disp  = run_r1_set("prog_r1_c1prime", kC1);   // kC1 stays the expected colour
                                     note_mutant_done("prog_r1_c1prime", all_of_pop(r1_disp, [](const R1Out& r) { return r.done; })); }
    const std::vector<R1Out>& r1_ledcal_feed  = mv == "P1-LEDCAL" ? r1_nocal : r1_base;
    const std::vector<R1Out>& r1_disp_feed    = mv == "P1-DISPANCHOR" ? r1_disp : r1_base;
    const std::vector<R1Out>& r1_p1b_feed     = mv == "P1A" ? r1_nop : r1_base;
    const std::vector<R1Out>& r1_witness_feed = mv == "P1A" ? r1_nop : r1_base;

    // ---- FIXTURE: vector-table check, on every R1-family setting -------
    bool fdd_ever_changed_any = false;
    for (const auto& r : r1_base) if (r.fdd_ever_changed) fdd_ever_changed_any = true;
    const bool fixture_ok = all_of_pop(r1_base, [](const R1Out& r) { return r.loaded && r.vecok && r.m0_seen; });
    checks.expect(fixture_ok,
        "FIXTURE: R1 loads, the vector-table check passes and M0 is written, on every CPU setting "
        "[not graded -- apparatus check, no claim uid]");
    checks.note(std::string("FIXTURE-FDD: across run R1, the drive-LED port ") +
        (fdd_ever_changed_any ? "changed level at least once" : "never changed level") +
        " on any CPU setting (NM-HRM-046)");

    // ================= P1 / P2 preconditions, printed first ===============
    const bool p1_markers_ok = all_of_pop(r1_base, [](const R1Out& r) {
        return r.loaded && r.t_v2.has_value() && r.v1_writes == 1 && r.v0_writes_before_m1 == 1; });
    checks.expect(p1_markers_ok,
        "P1-MARKERS [apparatus anchor of section 3, no claim uid]: R1 loads, the second vertical-sync leading edge "
        "after t_M1 is found, the V1 write appears exactly once and the V0 write exactly once before t_M1, "
        "on every CPU setting");

    const bool p1_ledcal_ok = all_of_pop(r1_ledcal_feed, [](const R1Out& r) { return r.loaded && r.led_calib_ok; });
    checks.expect(p1_ledcal_ok,
        "P1-LEDCAL [p351]: the power LED makes exactly one transition between the dim-phase marker and t_M1, "
        "from one level (pra bit 1 = 1) to a different level (pra bit 1 = 0), on every CPU setting");

    const bool p1_disp_ok = all_of_pop(r1_disp_feed, [](const R1Out& r) { return r.loaded && r.fref_ok; });
    checks.expect(p1_disp_ok,
        "P1-DISPANCHOR [p46,p48]: F_ref shows only COLOR00's value and black, confirming the "
        "display anchor, on every CPU setting");

    const bool p1_noexc_ok = all_of_pop(r1_base, [](const R1Out& r) { return r.loaded && r.no_exception; });
    checks.expect(p1_noexc_ok,
        "P1-NOEXCEPTION [p223]: no pixel of value C_E and no write of V_E to MK_E anywhere in the "
        "run (H_E never runs), on every CPU setting");

    // ================= P1 (P1a not checkable; P1b graded) ===============
    {
        std::string why = failed_names({{"P1-MARKERS", p1_markers_ok}, {"P1-LEDCAL", p1_ledcal_ok},
                                        {"P1-DISPANCHOR", p1_disp_ok}, {"P1-NOEXCEPTION", p1_noexc_ok}});
        if (why.empty() && !all_of_pop(r1_p1b_feed, [](const R1Out& r) { return r.loaded; })) why = "the P1b run did not load";
        if (why.empty()) {
            const bool p1b_ok = all_of_pop(r1_p1b_feed, [](const R1Out& r) {
                return r.pwr_at_m1 == r.l_bright && r.pwr_change_seen && r.pwr_change_to_dim && r.pwr_change_in_window; });
            checks.expect(p1b_ok,
                "P1b [p351,p354,p223]: CIA-A's port A reverts to inputs and the power LED leaves its bright "
                "state for the calibrated dim level after the RESET instruction, on every CPU setting");
        } else {
            checks.record("P1b [p351,p354,p223]: not graded: precondition failed (" + why + ")");
        }
        // P1a's precondition (SPECIFICATION.md section 5.1.5, drive-LED calibration): the
        // drive-LED port shows the latched motor state, so it changes level between the motor
        // off-latch and the on-latch. Measured on every CPU setting before P1a.
        int fdd_changes_min = -1, fdd_changes_max = 0;
        for (const auto& r : r1_base) {
            if (fdd_changes_min < 0 || r.fdd_changes_before_m1 < fdd_changes_min) fdd_changes_min = r.fdd_changes_before_m1;
            if (r.fdd_changes_before_m1 > fdd_changes_max) fdd_changes_max = r.fdd_changes_before_m1;
        }
        const bool p1a_fddcal_ok = all_of_pop(r1_base, [](const R1Out& r) { return r.loaded && r.fdd_changes_before_m1 >= 1; });
        checks.note("P1a-FDDCAL (precondition of P1a): the drive-LED port changed level between the "
            "off-latch/on-latch sequence and t_M1 on every CPU setting: " + std::string(p1a_fddcal_ok ? "yes" : "no") +
            " (level changes per setting: min " + std::to_string(fdd_changes_min < 0 ? 0 : fdd_changes_min) +
            ", max " + std::to_string(fdd_changes_max) + ")");
        if (!p1a_fddcal_ok)
            checks.record("P1a [p244,p370]: not graded: precondition failed (P1a-FDDCAL: the drive-LED port did "
                "not show the latched motor state; NM-HRM-046)");
        else
            checks.record("P1a [p244,p370]: not graded: P1a-FDDCAL holds, but no assertion for P1a is built "
                "in this package");
    }

    // ================= P2 =================================================
    {
        const bool witness_ok = all_of_pop(r1_witness_feed, [](const R1Out& r) {
            return r.loaded && r.pwr_change_seen && r.pwr_change_in_window; });
        checks.expect(witness_ok,
            "P2-WITNESS [p394,p223]: a system reset took place in the R1 run: the power LED changes in "
            "[t_M1, t_C2], on every CPU setting");

        std::string why = failed_names({{"P1-MARKERS", p1_markers_ok}, {"P1-LEDCAL", p1_ledcal_ok},
                                        {"P1-DISPANCHOR", p1_disp_ok}, {"P1-NOEXCEPTION", p1_noexc_ok},
                                        {"P2-WITNESS", witness_ok}});
        int v0_after_total = 0;
        for (const auto& r : r1_base) v0_after_total += r.v0_writes_after_m1;
        if (why.empty()) {
            const bool p2_ok = all_of_pop(r1_base, [](const R1Out& r) { return r.c2_pixel_seen; });
            checks.expect(p2_ok,
                "P2 [p223,p394,p411]: RESET does not reset the CPU; execution continues with the next "
                "instruction (a visible pixel of C2 appears after RESET), on every CPU setting");
        } else {
            checks.record("P2 [p223,p394,p411]: not graded: precondition failed (" + why + ")");
        }
        checks.note(std::string("P2 V0-after-M1 writes across all CPU settings: ") + std::to_string(v0_after_total) +
            " (not graded: no page bounds how long a full-system reset, which this is not, would take)");
    }

    // ================= Gate G =============================================
    SimpleImgOut g = run_simple_img(dir + "/prog_rg.bin", kImgAtMkNext);
    bool g2 = false, g3 = false, reg08_can_shutup = false;
    if (g.done) {
        // register 00's physical byte (not complemented): bits7-4 from the
        // word at AC_BASE+0, bits3-0 from the word at AC_BASE+2 (section 4,
        // Figure K-14 a)).
        Reg00 r00 = decode_reg00(g.img);
        g2 = r00.type == 0b10 || r00.type == 0b11;
        auto reserved_ones = [&](int idx) { return (g.img[idx] & 0xF000) == 0xF000; };
        g3 = reserved_ones(6) && reserved_ones(7) && reserved_ones(26) && reserved_ones(27) &&
             reserved_ones(28) && reserved_ones(29) && reserved_ones(30) && reserved_ones(31); // 0C,34,38,3C, both addresses of each pair

        // register 08 (pair at $00E80008/$00E8000A, index 4 for the hi
        // nybble), complemented (section 4: every register but 00 is
        // physically complemented). Bit 6 of the logical byte is bit 2 of
        // the logical hi nybble.
        int nib08_phys = (g.img[4] >> 12) & 0xF;
        int nib08_logical = (~nib08_phys) & 0xF;
        reg08_can_shutup = ((nib08_logical >> 2) & 1) == 0;
    }
    bool gate_g_pass = g.loaded && g.done.has_value() && g.t_v2_found && g2 && g3;
    if (gate_g_pass) {
        checks.note("Gate G: an unconfigured PIC answers in $00E8xxxx (type field and reserved registers "
            "0C/34/38/3C read as documented)");
    } else {
        checks.record("Gate G failed: no AUTOCONFIG device answers in $00E8xxxx (fastram_config offers "
            "none, or the harness's start-up had already configured it)");
    }

    // ---- Gate G precondition, then GATE-G's own check: the stimulus writes
    // $00 to the shut-up address 4C and reads the image again.
    if (gate_g_pass) {
        checks.expect(reg08_can_shutup,
            "GATE-G-PRECOND [p433]: register 08 bit 6 reads logical 0, the board can be shut up");
    } else {
        checks.record("GATE-G-PRECOND [p433]: unavailable: Gate G supplied no register image");
    }
    if (gate_g_pass && reg08_can_shutup) {
        SimpleImgOut r = run_simple_img(dir + "/prog_rg_shutup.bin", kImgAtMkNext);
        if (r.loaded && r.t_v2_found) {
            checks.expect(!r.done || !img_equal(r.img, g.img),
                "GATE-G [p433,p436]: writing $00 to the shut-up address 4C makes the PIC that supports shut-up "
                "disappear from $00E8xxxx (a differing image, or no answer)");
        } else {
            checks.record("GATE-G [p433,p436]: not graded: the shut-up run did not load or did not run past "
                "the second vertical-sync leading edge");
        }
    } else {
        checks.record("GATE-G [p433,p436]: not graded: Gate G failed or register 08 bit 6 indicates the board "
            "cannot be shut up");
    }

    // ================= P3 (only if gate G passed) =========================
    if (gate_g_pass) {
        const R3Out r_base  = run_r3(dir + "/prog_r3.bin");
        const R3Out rn_base = run_r3(dir + "/prog_r3_n.bin");
        const R3Out rs_base = run_r3(dir + "/prog_r3_swap.bin");
        const R3Out rsn     = run_r3(dir + "/prog_r3_swap_n.bin");
        auto mutated_r3 = [&](const char* sel, const char* prog, const R3Out& normal) {
            if (mv != sel) return normal;
            R3Out m = run_r3(dir + "/" + prog + ".bin");
            checks.note(std::string("NMH_MUTATE=") + mv + ": the mutated run (" + prog + ") wrote the completion marker: " +
                        (m.done ? "yes" : "no"));
            return m;
        };
        const R3Out r_img1   = mutated_r3("P3-IMG1", "prog_r3_c48first", r_base);
        const R3Out r_img2   = mutated_r3("P3-IMG2", "prog_r3_nocfg", r_base);
        const R3Out r_cont   = mutated_r3("P3-CONTENT", "prog_r3_no48", r_base);
        const R3Out rs_feed  = mutated_r3("P3-BASE-ASSIGNED", "prog_r3_swap_no48", rs_base);
        const R3Out rn_feed  = mutated_r3("P3-NOTOTHER-ASSIGNED", "prog_r3_n_no48", rn_base);
        const R3Out r_io     = mutated_r3("P3-CONTROL-IO", "prog_r3_io_self", run_r3(dir + "/prog_r3_io.bin"));
        const R3Out r_unconf = mutated_r3("P3-CONTROL-UNCONF", "prog_r3_unconf_self", run_r3(dir + "/prog_r3_unconf_n.bin"));

        // ---- IMG1 = IMG0 (contrast: P3-IMG1 / R3c) and IMG2 != IMG0 --------
        // img1 is written at step b, before any of R3's own risky access,
        // so it is trustworthy on the unmutated run regardless of
        // out.done. Under the R3c mutation img1 *is* the run's own last
        // (possibly-unanswered) action: if register 48's own decoder stops
        // answering there too, img1 stays at its unwritten (zero) default,
        // which the comparator below already reads as "not equal" to the
        // real, nonzero img0 -- the same formula catches both outcomes.
        const bool img_pre = r3_live(r_img1) && r3_live(r_img2) &&
                             board_is_zorro2_2mb_memory(r_img1.img0) && board_is_zorro2_2mb_memory(r_img2.img0);
        checks.expect(img_pre,
            "P3-IMG-PRECOND [p432,p433]: the runs behind P3-IMG1 and P3-IMG2 loaded and ran to the completion "
            "marker or past the second vertical-sync leading edge; register 00 reads type 11, memory flag set "
            "and size 110 (2 megabytes)");
        if (img_pre) {
            checks.expect(img_equal(r_img1.img1, r_img1.img0),
                "P3-IMG1 [p431,p436]: the nybble write to register 4A alone does not configure the "
                "board (register-image unchanged)");
            checks.expect(!r_img2.done || !img_equal(r_img2.img2, r_img2.img0),
                "P3-IMG2 [p435,p431]: after the byte write to register 48 the PIC leaves $00E8xxxx "
                "(one AUTOCONFIG device offered; a differing register-image)");
        } else {
            checks.record("P3-IMG1 [p431,p436]: not graded: precondition failed (P3-IMG-PRECOND)");
            checks.record("P3-IMG2 [p435,p431]: not graded: precondition failed (P3-IMG-PRECOND)");
        }

        // ---- memory-board content check (R0=P0, R1=P1V), both R3 and R3n -
        // R0/R1 are written at step d, before R3's own risky IMG2 read (or
        // R3n's own risky OTHER_BASE read): trustworthy regardless of done.
        const bool content_pre = r3_live(r_cont) && r3_live(rn_base) &&
                                 board_is_zorro2_2mb_memory(r_cont.img0) && board_is_zorro2_2mb_memory(rn_base.img0) &&
                                 r_cont.scratch_before_r0 && rn_base.scratch_before_r0;
        checks.expect(content_pre,
            "P3-CONTENT-PRECOND [p432,p433]: both runs loaded and ran past the window; register 00 reads "
            "type 11, memory flag set and size 110 (2 megabytes); the scratch write precedes the readback");
        if (content_pre) {
            checks.expect(r_cont.r0 && *r_cont.r0 == kP0v && r_cont.r1 && *r_cont.r1 == kP1v &&
                          rn_base.r0 && *rn_base.r0 == kP0v && rn_base.r1 && *rn_base.r1 == kP1v &&
                          !r_cont.alias_before_r0 && !rn_base.alias_before_r0,
                "P3-CONTENT [p433]: a memory board answers with what software wrote at the base it was "
                "given, in both runs, with no aliasing write below $200000 before the readback");
        } else {
            checks.record("P3-CONTENT [p433]: not graded: precondition failed (P3-CONTENT-PRECOND)");
        }

        // ---- R2 != P2, or no answer (R3n's own check): P3-NOTOTHER -------
        const bool notother_pre = r3_live(rn_base) && board_is_zorro2_2mb_memory(rn_base.img0) &&
                                  rn_base.scratch_before_r2;
        checks.expect(notother_pre,
            "P3-NOTOTHER-PRECOND [p432,p433]: R3n loaded and ran past the window; register 00 reads type 11, "
            "memory flag set and size 110 (2 megabytes); the scratch write precedes the readback of R2");
        note_r2(checks, "P3-NOTOTHER run R3n", rn_base);
        if (notother_pre) {
            checks.expect(r3_assigned(rn_feed),
                "P3-NOTOTHER-ASSIGNED [p391,p435]: in run R3n a write/readback at B1, the base written to "
                "register 48, returns what was written (the board answers at the base it was given)");
            checks.expect(r3_assigned(rn_base) && r3_no_answer_or_other(rn_base),
                "P3-NOTOTHER (sol:K-0837, opus:K-0869) [p391,p433-435]: "
                "\"The software then tells each board where to reside in memory\" (p.391) and \"the "
                "low order register takes bits 23-16\" (p.435) -- the memory board answers at the base it "
                "was given and does not also answer at the base it was not given");
        } else {
            checks.record("P3-NOTOTHER [p391,p433-435]: not graded: precondition failed (P3-NOTOTHER-PRECOND)");
        }

        // ---- base-exchange contrast (P3-BASE / R3', R3n') -----------------
        const bool base_pre = r3_live(rs_feed) && r3_live(rsn) &&
                              board_is_zorro2_2mb_memory(rs_feed.img0) && board_is_zorro2_2mb_memory(rsn.img0) &&
                              rs_feed.scratch_before_r0 && rsn.scratch_before_r2;
        checks.expect(base_pre,
            "P3-BASE-PRECOND [p432,p433]: R3' and R3n' loaded and ran past the window; register 00 reads "
            "type 11, memory flag set and size 110 (2 megabytes) in both; the scratch write precedes the "
            "readback in each");
        note_r2(checks, "P3-BASE run R3n'", rsn);
        if (base_pre) {
            const bool assigned = r3_assigned(rs_feed);
            checks.expect(assigned,
                "P3-BASE-ASSIGNED [p391,p435]: with B1 and B2 exchanged and B2 written to register 48, a "
                "write/readback at B2 returns what was written (the board answers at the base it was given)");
            checks.expect(assigned && r3_no_answer_or_other(rsn),
                "P3-BASE (sol:K-0837, opus:K-0869) [p388,p391,p433,p435]: \"The "
                "software then tells each board where to reside in memory\" (p.391) and \"the low "
                "order register takes bits 23-16\" (p.435) -- with B1 and B2 exchanged the board "
                "answers at B2 and not at B1");
        } else {
            checks.record("P3-BASE-ASSIGNED [p391,p435]: not graded: precondition failed (P3-BASE-PRECOND)");
            checks.record("P3-BASE [p388,p391,p433,p435]: not graded: precondition failed (P3-BASE-PRECOND)");
        }

        // ---- green controls for P3-NOTOTHER and P3-BASE, same comparator -
        const bool io_pre = r3_live(r_io) && board_is_zorro2_2mb_memory(r_io.img0) && r_io.scratch_before_r2;
        checks.expect(io_pre,
            "P3-CONTROL-IO-PRECOND [p432,p433]: the control run loaded and ran past the window; register 00 "
            "reads type 11, memory flag set and size 110 (2 megabytes); the scratch write precedes the "
            "readback of R2");
        note_r2(checks, "P3-CONTROL-IO run", r_io);
        if (io_pre) {
            checks.expect(r3_no_answer_or_other(r_io),
                "P3-CONTROL-IO [p386]: with the board configured at $00200000, the same "
                "write/readback at $00A00000 (Figure K-1: Zorro II I/O expansion space) does not "
                "return what was written (R2 differs from P2, or no completion marker)");
        } else {
            checks.record("P3-CONTROL-IO [p386]: not graded: precondition failed (P3-CONTROL-IO-PRECOND)");
        }

        const bool unconf_pre = r3_live(r_unconf) && board_is_zorro2_2mb_memory(r_unconf.img0) && r_unconf.scratch_before_r2;
        checks.expect(unconf_pre,
            "P3-CONTROL-UNCONF-PRECOND [p432,p433]: the control run loaded and ran past the window; register "
            "00 reads type 11, memory flag set and size 110 (2 megabytes); the scratch write precedes the "
            "readback of R2");
        note_r2(checks, "P3-CONTROL-UNCONF run", r_unconf);
        if (unconf_pre) {
            checks.expect(r3_no_answer_or_other(r_unconf),
                "P3-CONTROL-UNCONF [p431]: with the board left unconfigured, the same "
                "write/readback at $00400000 does not return what was written (R2 differs from P2, "
                "or no completion marker)");
        } else {
            checks.record("P3-CONTROL-UNCONF [p431]: not graded: precondition failed (P3-CONTROL-UNCONF-PRECOND)");
        }
    } else {
        checks.record("P3 [p384,p391,p431,p435,p436]: not checkable: no AUTOCONFIG device answers in "
            "$00E8xxxx (gate G failed)");
    }

    // opus:K-0014, sol:K-0008 (p.384): withdrawn whole.
    checks.record("opus:K-0014 sol:K-0008 [p384]: withdrawn whole to this record: each claim also states that "
        "addressing jumpers were eliminated, which has no observable at this boundary; the software "
        "assignment of the base address is graded by P3-NOTOTHER and P3-BASE without these ids");

    // P4, P5: declined whole (SPECIFICATION.md section 1); no assertion, no mutation.
    checks.record("P4 [p435] (K-0840): not specified -- the 4A/A19-A16 table is headed for Zorro III "
        "boards configured in the Zorro II space; this boundary offers only Zorro II fast RAM.");
    checks.record("P5 [p435] (K-0838): not specified -- the 46/A27-A24 table needs a Zorro III board "
        "and a 32-bit address bus; nanomig.v's memory address ports are 24 bits wide.");

    // ================= P6 (only if gate G passed) ==========================
    // Each CPU setting runs R6 (register-image ending) and R6m (memory-board
    // ending) in full; P6 is graded only when both runs pass every
    // precondition at every setting. Under NMH_MUTATE=P6 both runs have RESET
    // replaced by NOP.
    if (gate_g_pass) {
        struct P6Pair { R6Out a, m; };
        std::vector<P6Pair> p6;
        const bool p6_nop = mv == "P6";
        for (uint8_t s : settings)
            p6.push_back({ run_r6(dir + (p6_nop ? "/prog_r6a.bin" : "/prog_r6.bin"), s, false),
                           run_r6(dir + (p6_nop ? "/prog_r6am.bin" : "/prog_r6m.bin"), s, true) });
        if (p6_nop)
            note_mutant_done("prog_r6a, prog_r6am", all_of_pop(p6, [](const P6Pair& p) { return p.a.done && p.m.done; }));

        const bool p6_config_ok = all_of_pop(p6, [](const P6Pair& p) {
            return p.a.loaded && p.m.loaded && p.a.no_exception && p.m.no_exception &&
                   p.a.config_took_effect && p.m.config_took_effect; });
        checks.expect(p6_config_ok,
            "P6-CONFIG [p431,p436]: R6 and R6m load, no exception handler runs, and the board answers at "
            "its base before RESET (R0 = P0), on every CPU setting");

        const bool p6_witness_ok = all_of_pop(p6, [](const P6Pair& p) { return p.a.led_witness && p.m.led_witness; });
        checks.expect(p6_witness_ok,
            "P6-WITNESS [p394,p223]: a system reset took place in R6 and R6m themselves: the LED changes "
            "after t_M1 and before the second vertical-sync leading edge, on every CPU setting");

        const bool p6_chipmem_ok = all_of_pop(p6, [](const P6Pair& p) { return p.a.done.has_value() && p.m.done.has_value(); });
        if (p6_chipmem_ok) {
            checks.note("P6 Chip memory answered at the program's addresses after RESET (the completion marker "
                "was written by R6 and R6m) on every CPU setting");
        } else {
            checks.record("P6-CHIPMEM [p223]: Chip memory did not answer at the program's addresses after "
                "RESET on some CPU setting (p.223: \"Software must assume that memory is not available\")");
        }

        std::string why = failed_names({{"P6-CONFIG", p6_config_ok}, {"P6-WITNESS", p6_witness_ok},
                                        {"P6-CHIPMEM", p6_chipmem_ok}});
        if (why.empty()) {
            const bool p6_ok = all_of_pop(p6, [](const P6Pair& p) {
                return img_equal(p.a.img3, p.a.img0) && p.m.r3 && *p.m.r3 != kP0Prime; });
            checks.expect(p6_ok,
                "P6 [p223,p394,p431]: RESET returns a configured PIC to the unconfigured state "
                "(register-image restored; a memory board no longer answers at its old base), on every "
                "CPU setting");
        } else {
            checks.record("P6 [p223,p394,p431]: not graded: precondition failed (" + why + ")");
        }
    } else {
        checks.record("P6 [p223,p394,p431]: not checkable: no AUTOCONFIG device answers in $00E8xxxx "
            "(gate G failed)");
    }

    // opus:K-0164, sol:K-0155 (p.394): withdrawn whole.
    checks.record("opus:K-0164 sol:K-0155 [p394]: withdrawn whole to this record: the claims state the "
        "levels of the expansion-bus lines /RST, /BUSRST and /HLT, and none of the three is an output "
        "of this boundary; the reset, CPU-continuation and autoconfiguration-reset consequences are "
        "graded as P1b, P2 and P6 without these ids");

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
