// test_appK_autoconfig.cpp -- appK-autoconfig. See SPECIFICATION.md for
// every assertion's proposition, program, observed value and expected
// value. This file only implements what SPECIFICATION.md already
// committed to.
//
// NMH_MUTATE selects a mutated program in place of its normal counterpart,
// one assertion or precondition at a time; none is exercised by a plain
// `make test`.
//
// MUTATION P1A stimulus green RESET at A replaced by NOP (R1a): the same "no LED change" comparator now correctly reports one, via NMH_MUTATE=P1A
// MUTATION P2 stimulus green MOVE.W D0,(A0) at A+2 replaced by NOP (R1b): the same "no C2 pixel" comparator now correctly reports one, via NMH_MUTATE=P2
// MUTATION P1-DISPANCHOR stimulus green COLOR00 preset to C1'=$0C3F instead of C1: the same anchor comparator, fed the mutation's own live colour, still recognises F_ref, via NMH_MUTATE=P1-DISPANCHOR
// MUTATION P1-NOEXCEPTION stimulus green SR set to $0700 (user mode) before RESET, so RESET is a privilege violation: the same "no exception" comparator now correctly reports one, via NMH_MUTATE=P1-NOEXCEPTION
// MUTATION P3-IMG1 stimulus red step b writes register 48 (configures immediately) in place of the 4A write, via NMH_MUTATE=P3-IMG1
// MUTATION P3-IMG2 stimulus red steps c and d skipped, board never configured, via NMH_MUTATE=P3-IMG2
// MUTATION P3-CONTENT stimulus red R0's own run substitutes R3u (steps c and d skipped), so R0/R1 stay at their unwritten default, via NMH_MUTATE=P3-CONTENT
// MUTATION GATE-G stimulus red the shut-up write to 4C skipped (IMG re-read from the same unconfigured PIC instead), via NMH_MUTATE=GATE-G
// MUTATION P6 stimulus green RESET at A replaced by NOP (R6a/R6am): the same "board stays configured" comparators (P6-CONTRAST, P7-PART-III) now correctly report it, via NMH_MUTATE=P6
//
// P3-BASE is not exercised by a dedicated mutation, by a scope decision:
// see the comment at its own check, below.
// P3-NOTOTHER is not exercised by a dedicated mutation, by a scope
// decision: this system offers exactly one AUTOCONFIG board, so no
// stimulus can make it answer at a base it was not given; R3's own
// content check (P3-CONTENT) already proves the same comparator green on
// a live, matching input at the base that *was* given.
//
// P4 and P5 carry no assertion: SPECIFICATION.md section 1 declines
// K-0840 and K-0838 whole (no Zorro III device at this boundary). Not
// exercised.
//
// P1a carries no assertion either: SPECIFICATION.md's drive-LED
// calibration assumes nanomig.v's fdd_led output follows the drive-motor
// line p.244 describes ("DSKMOTOR* also controls the activity light on
// the front of the disk drive"). Measured here (FIXTURE-FDD, below):
// across the full motor off-latch/on-latch sequence, on every run, on
// both cores, fdd_led's level never changes at all -- it stays at its
// initial sample throughout. No level of fdd_led is graded; P1a is
// reported not checkable at this boundary.

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
constexpr uint64_t kSimpleBudget = 6'000'000;   // gate G / P3: no frame waits
constexpr uint64_t kResetBudget  = 9'000'000;   // P1/P2/P6: up to ~6 frames of waiting

constexpr uint32_t kMK0     = kResults + 0;
constexpr uint32_t kMK_E    = kResults + 2;
constexpr uint32_t kVECOK   = kResults + 4;
constexpr uint32_t kMK_NEXT = kResults + 6;

constexpr uint16_t kV0        = 0xAAAA;
constexpr uint16_t kVE        = 0xDEAD;
constexpr uint16_t kVecokYes  = 0xFACE;

// R1-family (prog_r1*.s)
constexpr uint32_t kMK1 = kMK_NEXT + 0;
constexpr uint32_t kMK2 = kMK_NEXT + 2;
constexpr uint16_t kV1 = 0xBBBB, kV2 = 0xCCCC;
constexpr uint16_t kC1 = 0x0C30, kC1Prime = 0x0C3F, kC2 = 0x03AC, kCE = 0x0F0F;

// gate G / P3-family IMG base (prog_rg*.s, prog_r3*.s all start their own
// IMG0 at MK_NEXT)
constexpr uint32_t kImgAtMkNext = kMK_NEXT;

// P3-family (prog_r3*.s)
constexpr uint32_t kR3Img0 = kMK_NEXT + 0;
constexpr uint32_t kR3Img1 = kMK_NEXT + 64;
constexpr uint32_t kR3Img2 = kMK_NEXT + 128;
constexpr uint32_t kR3R0   = kMK_NEXT + 192;
constexpr uint32_t kR3R1   = kMK_NEXT + 194;
constexpr uint32_t kR3R2   = kMK_NEXT + 198;
constexpr uint16_t kP0v = 0xA55A, kP1v = 0x3CC3, kP2v = 0x5AA5;
constexpr uint32_t kB1 = 0x00200000, kB2 = 0x00400000;

// R6-family (prog_r6*.s)
constexpr uint32_t kR6Mk1  = kMK_NEXT + 0;
constexpr uint32_t kR6Mk2  = kMK_NEXT + 2;
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
    // offered"). nanomig.v's own fastram_config port (3 bits) selects a
    // Zorro II board whenever it is nonzero and a size from its value; 1
    // selects a 2 MB board (measured: register 00 reads size code 6, the
    // p.433 "Unextended" Zorro II size for 2 MB, and a memory board
    // written and read back correctly at the base it was configured to).
    cfg.fastram_config = 1;
    // floppy_config: the harness documents only 0 ("no floppy drives").
    // nanomig.v's own floppy_config port packs the drive count into bits
    // 3-2; 1 there (floppy_config=$04) connects one drive (drive 0).
    cfg.floppy_config = 0x04;
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

// ---- R1-family capture (P1, P2 and, via reuse, P6/P7's reset witness) -

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
    bool led_dim_seen_before_m1 = false;
    bool led_bright_seen_before_m1 = false;
    bool pwr_bright_at_m1 = false;
    std::optional<uint64_t> t_v2;
    uint64_t t_c2 = 0;
    bool c2_pixel_seen = false;
    bool pwr_change_seen = false;
    uint64_t pwr_change_cycle = 0;
    bool pwr_change_to_dim = false;
    bool pwr_change_in_window = false; // pwr_change_cycle in [t_m1, t_c2]
    bool fdd_ever_changed = false; // for the P1a finding only
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

    if (!out.t_m1) return out;

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
        if (t.cycle <= *out.t_m1) {
            if (t.pwr) out.led_bright_seen_before_m1 = true; else out.led_dim_seen_before_m1 = true;
            out.pwr_bright_at_m1 = t.pwr;
        }
    }
    for (auto& t : trans) {
        if (t.cycle > *out.t_m1) {
            out.pwr_change_seen = true;
            out.pwr_change_cycle = t.cycle;
            out.pwr_change_to_dim = !t.pwr;
            out.pwr_change_in_window = t.cycle <= out.t_c2;
            break;
        }
    }

    return out;
}

// ---- gate G / P3 capture (no frame waits, DONE-bounded) ---------------

struct SimpleImgOut {
    bool loaded = false;
    std::optional<uint64_t> done;
    std::array<uint16_t, 32> img{};
};

SimpleImgOut run_simple_img(const std::string& bin, uint32_t img_base) {
    SimpleImgOut out;
    nmh::Config cfg = base_cfg();
    nmh::NanomigHarness h(cfg);
    out.loaded = h.load_program(bin) > 0;
    if (!out.loaded) return out;
    h.boot(kBootBudget);
    size_t seen = 0;
    for (uint64_t i = 0; i < kSimpleBudget && !out.done; ++i) {
        h.run_cycles(1);
        for (; seen < h.ram_log().size(); ++seen) {
            const auto& a = h.ram_log()[seen];
            if (a.is_write && a.word_addr == kDoneAddr / 2 && a.data == kDoneValue) out.done = a.cycle;
        }
    }
    if (out.done) out.img = read_img(h, img_base);
    return out;
}

struct R3Out {
    bool loaded = false;
    std::optional<uint64_t> done;
    std::array<uint16_t, 32> img0{}, img1{}, img2{};
    std::optional<uint16_t> r0, r1, r2;
    bool alias_before_r0 = false; // a P0/P1 write below $200000 before R0's store
};

R3Out run_r3(const std::string& bin) {
    R3Out out;
    nmh::Config cfg = base_cfg();
    nmh::NanomigHarness h(cfg);
    out.loaded = h.load_program(bin) > 0;
    if (!out.loaded) return out;
    h.boot(kBootBudget);
    size_t seen = 0;
    for (uint64_t i = 0; i < kSimpleBudget && !out.done; ++i) {
        h.run_cycles(1);
        for (; seen < h.ram_log().size(); ++seen) {
            const auto& a = h.ram_log()[seen];
            if (a.is_write && a.word_addr == kDoneAddr / 2 && a.data == kDoneValue) out.done = a.cycle;
        }
    }
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
    return out;
}

// ---- R6-family capture (P6: reuses P1's steps a-d, then RESET + tail) -

struct R6Out {
    bool loaded = false;
    std::optional<uint64_t> t_m1;
    bool led_witness = false;   // at least one LED change observed in [t_m1, t_c2]
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
    // core behaviour"). p.394 speaks of "the 680x0 RESET instruction";
    // core A (fx68k) implements only the 68000, core B (TG68K) the three.
    std::vector<uint8_t> settings = is_tg68k ? std::vector<uint8_t>{0, 1, 3} : std::vector<uint8_t>{0};
    // Under a mutation, one CPU setting is enough to see the predicted
    // stimulus-red/stimulus-green outcome: the normal (NMH_MUTATE unset)
    // run above already covers every setting, and is unaffected by this.
    if (!mv.empty()) settings = {settings[0]};

    // ---- FIXTURE: vector-table check, on every R1-family setting -------
    bool fixture_ok = true;
    bool fdd_ever_changed_any = false;
    for (uint8_t s : settings) {
        R1Out r = run_r1(dir + "/prog_r1.bin", s, kC1);
        if (!(r.loaded && r.vecok && r.m0_seen)) fixture_ok = false;
        if (r.fdd_ever_changed) fdd_ever_changed_any = true;
    }
    checks.expect(fixture_ok,
        "FIXTURE: R1 loads, the vector-table check passes and M0 is written, on every CPU setting "
        "[not graded -- apparatus check, no claim uid]");
    checks.record(std::string("FIXTURE-FDD: across the drive-motor off-latch/on-latch sequence of run R1, "
        "fdd_led ") + (fdd_ever_changed_any ? "changed level at least once" : "never changed level") +
        " on any CPU setting: nanomig.v's fdd_led output reflects disk-DMA activity only "
        "(no disk image is mounted and no DMA ever starts), not the drive-select/motor line p.244 describes "
        "(\"DSKMOTOR* also controls the activity light on the front of the disk drive\").");

    // ================= P1 (P1a not checkable; P1b graded) ===============
    {
        bool p1b_ok = true;
        for (uint8_t s : settings) {
            R1Out r = run_r1(dir + "/prog_r1.bin", s, kC1);
            bool preconds = r.loaded && r.v1_writes == 1 && r.v0_writes_before_m1 == 1 &&
                             r.no_exception && r.fref_ok &&
                             r.led_dim_seen_before_m1 && r.led_bright_seen_before_m1 && r.pwr_bright_at_m1;
            bool val = r.pwr_change_seen && r.pwr_change_to_dim && r.pwr_change_in_window;
            if (!(preconds && val)) p1b_ok = false;
        }
        checks.expect(p1b_ok,
            "P1b [p351,p354,p223]: CIA-A's port A reverts to inputs and the power LED leaves its bright "
            "state after the RESET instruction, on every CPU setting");
        checks.record("P1a [p244,p370]: not checkable at this boundary (see FIXTURE-FDD above): the "
            "drive-LED calibration precondition (L_on != L_off from the motor latch alone) cannot be "
            "satisfied, because fdd_led never changes with the latch. No level of fdd_led is graded.");
    }

    // ================= P2 =================================================
    {
        bool p2_ok = true;
        int v0_after_total = 0;
        for (uint8_t s : settings) {
            R1Out r = run_r1(dir + "/prog_r1.bin", s, kC1);
            bool preconds = r.loaded && r.v1_writes == 1 && r.no_exception && r.fref_ok &&
                             r.pwr_change_seen && r.pwr_change_in_window; // reset witnessed (P1b's LED change)
            v0_after_total += r.v0_writes_after_m1;
            if (!(preconds && r.c2_pixel_seen)) p2_ok = false;
        }
        checks.expect(p2_ok,
            "P2 [p223,p394,p411]: RESET does not reset the CPU; execution continues with the next "
            "instruction (a visible pixel of C2 appears after RESET), on every CPU setting");
        checks.note(std::string("P2 V0-after-M1 writes across all CPU settings: ") + std::to_string(v0_after_total) +
            " (not graded: no page bounds how long a full-system reset, which this is not, would take)");
    }

    // ================= Gate G =============================================
    SimpleImgOut g = run_simple_img(dir + "/prog_rg.bin", kImgAtMkNext);
    bool g2 = false, g3 = false;
    if (g.done) {
        // register 00's physical byte (not complemented): bits7-4 from the
        // word at AC_BASE+0, bits3-0 from the word at AC_BASE+2 (section 4,
        // Figure K-14 a)).
        int nib_hi = (g.img[0] >> 12) & 0xF;
        int nib_lo = (g.img[1] >> 12) & 0xF;
        int type_byte = (nib_hi << 4) | nib_lo;
        int type = (type_byte >> 6) & 0x3;
        g2 = type == 0b10 || type == 0b11;
        auto reserved_ones = [&](int idx) { return (g.img[idx] & 0xF000) == 0xF000; };
        g3 = reserved_ones(6) && reserved_ones(26) && reserved_ones(28) && reserved_ones(30); // 0C,34,38,3C
    }
    bool gate_g_pass = g.loaded && g.done.has_value() && g2 && g3;
    if (gate_g_pass) {
        checks.note("Gate G: an unconfigured PIC answers in $00E8xxxx (type field and reserved registers "
            "0C/34/38/3C read as documented)");
    } else {
        checks.record("Gate G failed: no AUTOCONFIG device answers in $00E8xxxx (fastram_config offers "
            "none, or the harness's start-up had already configured it)");
    }

    // ---- Gate G contrast (GATE-G): the real stimulus writes $00 to the
    // shut-up address 4C; under NMH_MUTATE=GATE-G that write is skipped
    // (prog_rg.bin re-run instead), so the unchanged "differs from IMG0"
    // check must go red (an unconfigured PIC answers the same way twice).
    // Measured red on both cores even on the unmutated run (register 08
    // bit 6 reads logical 0, "can be shut up", so the precondition holds):
    // the image after the write is identical to IMG0. Not worked around
    // -- see the report to the lead.
    {
        std::string bin = dir + (mv == "GATE-G" ? "/prog_rg.bin" : "/prog_rg_shutup.bin");
        SimpleImgOut r = run_simple_img(bin, kImgAtMkNext);
        bool differs = !r.done || !img_equal(r.img, g.img);
        checks.expect(gate_g_pass ? differs : true,
            "GATE-G [p436]: writing $00 to the shut-up address 4C makes the PIC that supports shut-up "
            "disappear from $00E8xxxx (a differing image, or no answer)");
    }

    // ================= P3 (only if gate G passed) =========================
    if (gate_g_pass) {
        // ---- IMG1 = IMG0 (contrast: P3-IMG1 / R3c) -----------------------
        // img1 is written at step b, before any of R3's own risky access,
        // so it is trustworthy on the unmutated run regardless of
        // out.done. Under the R3c mutation img1 *is* the run's own last
        // (possibly-unanswered) action: if register 48's own decoder stops
        // answering there too, img1 stays at its unwritten (zero) default,
        // which the comparator below already reads as "not equal" to the
        // real, nonzero img0 -- the same formula catches both outcomes.
        {
            std::string bin = dir + (mv == "P3-IMG1" ? "/prog_r3_c48first.bin" : "/prog_r3.bin");
            R3Out r = run_r3(bin);
            bool ok = r.loaded && img_equal(r.img1, r.img0);
            checks.expect(ok,
                "P3-IMG1 [p431,p436]: the nybble write to register 4A alone does not configure the "
                "board (register-image unchanged)");
        }

        // ---- IMG2 != IMG0, or no answer (contrast: P3-IMG2 / R3u) --------
        {
            std::string bin = dir + (mv == "P3-IMG2" ? "/prog_r3_nocfg.bin" : "/prog_r3.bin");
            R3Out r = run_r3(bin);
            bool ok = r.loaded && (!r.done || !img_equal(r.img2, r.img0));
            checks.expect(ok,
                "P3-IMG2 [p435,p431]: after the byte write to register 48 the PIC leaves $00E8xxxx "
                "(one AUTOCONFIG device offered; a differing register-image)");
        }

        // ---- memory-board content check (R0=P0, R1=P1V), both R3 and R3n -
        // R0/R1 are written at step d, before R3's own risky IMG2 read (or
        // R3n's own risky OTHER_BASE read): trustworthy regardless of done.
        // Under NMH_MUTATE=P3-CONTENT, r's own run substitutes R3u (steps c
        // and d skipped): R0/R1 stay at their unwritten default, so the
        // unchanged "R0=P0" check must go red.
        {
            std::string bin_r = dir + (mv == "P3-CONTENT" ? "/prog_r3_nocfg.bin" : "/prog_r3.bin");
            R3Out r = run_r3(bin_r);
            R3Out rn = run_r3(dir + "/prog_r3_n.bin");
            bool ok = r.loaded && r.r0 && *r.r0 == kP0v && r.r1 && *r.r1 == kP1v &&
                      rn.loaded && rn.r0 && *rn.r0 == kP0v && rn.r1 && *rn.r1 == kP1v &&
                      !r.alias_before_r0 && !rn.alias_before_r0;
            checks.expect(ok,
                "P3-CONTENT [p433]: a memory board answers with what software wrote at the base it was "
                "given, in both runs, with no aliasing write below $200000 before the readback");
        }

        // ---- R2 != P2, or no answer (R3n's own check) --------------------
        // Measured red on both cores: the board answers at OTHER_BASE too
        // (R2 reads back as P2), not just at the base it was given. Not
        // worked around -- see the report to the lead.
        {
            R3Out rn = run_r3(dir + "/prog_r3_n.bin");
            bool ok = rn.loaded && (!rn.done || *rn.r2 != kP2v);
            checks.expect(ok,
                "P3-NOTOTHER [p431]: the memory board does not also answer at the base it was not given");
        }

        // ---- base-exchange contrast (P3-BASE / R3', R3n') -----------------
        // Not exercised by a dedicated mutation, by a scope decision: R3'
        // and R3n' already are the contrast (the base the board answers at
        // follows the value written to 4A/48, not a fixed choice), and
        // each program checks its own self-consistent pair (the base it
        // configured holds the content; the other does not). Swapping in
        // the *unswapped* pair under NMH_MUTATE would not go red -- both
        // pairs are symmetric under the exchange -- so no substitution
        // isolates this assertion further without a program that writes
        // one base's configuration and deliberately reads back at the
        // other, which this build does not have. Measured red on both
        // cores regardless, for the same reason as P3-NOTOTHER above.
        {
            R3Out r = run_r3(dir + "/prog_r3_swap.bin");
            R3Out rn = run_r3(dir + "/prog_r3_swap_n.bin");
            bool ok = r.loaded && r.r0 && *r.r0 == kP0v && r.r1 && *r.r1 == kP1v &&
                      rn.loaded && (!rn.done || *rn.r2 != kP2v);
            checks.expect(ok,
                "P3-BASE [p388,p433]: with B1 and B2 exchanged the board answers at B2 and not at B1");
        }
    } else {
        checks.record("P3 [p384,p391,p431,p435,p436]: not checkable: no AUTOCONFIG device answers in "
            "$00E8xxxx (gate G failed)");
    }

    // P4, P5: declined whole (SPECIFICATION.md section 1); no assertion, no mutation.
    checks.record("P4 [p435] (K-0840): not specified -- the 4A/A19-A16 table is headed for Zorro III "
        "boards configured in the Zorro II space; this boundary offers only Zorro II fast RAM.");
    checks.record("P5 [p435] (K-0838): not specified -- the 46/A27-A24 table needs a Zorro III board "
        "and a 32-bit address bus; nanomig.v's memory address ports are 24 bits wide.");

    // ================= P6 (only if gate G passed) ==========================
    bool p6_pass = false;
    bool p6_checked_this_run = false;
    if (gate_g_pass) {
        bool ok = true;
        bool any_led_witness = false, any_config_effect = false, any_chip_answers = false;
        for (uint8_t s : settings) {
            R6Out r6 = run_r6(dir + "/prog_r6.bin", s, false);
            R6Out r6m = run_r6(dir + "/prog_r6m.bin", s, true);
            bool r6_precond = r6.loaded && r6.no_exception && r6.led_witness && r6.config_took_effect;
            bool r6m_precond = r6m.loaded && r6m.no_exception && r6m.led_witness && r6m.config_took_effect;
            bool r6_chip_answers = r6.done.has_value();
            bool r6m_chip_answers = r6m.done.has_value();
            if (r6.led_witness) any_led_witness = true;
            if (r6.config_took_effect) any_config_effect = true;
            if (r6_chip_answers) any_chip_answers = true;

            if (!(r6_precond && r6_chip_answers && img_equal(r6.img3, r6.img0))) ok = false;
            if (!(r6m_precond && r6m_chip_answers && (!r6m.r3 || *r6m.r3 != kP0Prime))) ok = false;
        }
        p6_checked_this_run = any_led_witness && any_config_effect && any_chip_answers;
        p6_pass = ok && p6_checked_this_run;
        if (p6_checked_this_run) {
            checks.expect(p6_pass,
                "P6 [p223,p394,p431]: RESET returns a configured PIC to the unconfigured state "
                "(register-image restored; a memory board no longer answers at its old base), on every "
                "CPU setting");
        } else {
            checks.record("P6 [p223,p394,p431]: not checkable on this system: Chip memory did not answer "
                "at the program's addresses after RESET on any CPU setting (p.223: \"Software must assume "
                "that memory is not available\").");
        }

        // ---- contrast P6 (RESET->NOP): R6a, R6am --------------------------
        // The normal run never executes R6a/R6am (a mutant); it uses R6/R6m
        // again, and the check is trivially true. Only NMH_MUTATE=P6
        // substitutes the mutants and checks the real, predicted outcome.
        {
            bool mutated = (mv == "P6");
            std::string bin_a = dir + (mutated ? "/prog_r6a.bin" : "/prog_r6.bin");
            std::string bin_am = dir + (mutated ? "/prog_r6am.bin" : "/prog_r6m.bin");
            R6Out ra = run_r6(bin_a, 0, false);
            R6Out ram = run_r6(bin_am, 0, true);
            bool stays_configured = ra.loaded && ra.done && !img_equal(ra.img3, ra.img0) &&
                                     ram.loaded && ram.done && ram.r3 && *ram.r3 == kP0Prime;
            checks.expect(mutated ? stays_configured : true,
                "P6-CONTRAST [p431,p223]: with RESET replaced by NOP the board stays configured (a "
                "differing register-image from IMG0, and the memory board still answers with P0')");
        }
    } else {
        checks.record("P6 [p223,p394,p431]: not checkable: no AUTOCONFIG device answers in $00E8xxxx "
            "(gate G failed)");
    }

    // ================= P7 (combines P1's reset witness, P2, P6) ============
    {
        bool part_i = true, part_ii = true;
        for (uint8_t s : settings) {
            R1Out r = run_r1(dir + "/prog_r1.bin", s, kC1);
            if (!(r.loaded && r.pwr_change_seen && r.pwr_change_to_dim && r.pwr_change_in_window)) part_i = false;
            if (!(r.loaded && r.c2_pixel_seen)) part_ii = false;
        }
        bool part_iii = gate_g_pass && p6_checked_this_run && p6_pass;

        if (!gate_g_pass) {
            checks.record("P7 (opus:K-0164, sol:K-0155) [p394]: withdrawn whole to this record: gate G "
                "failed (fastram_config at the harness default 0 offers no Zorro II fast RAM, so "
                "/BUSRST has no witness).");
        } else if (!p6_checked_this_run) {
            checks.record("P7 (opus:K-0164, sol:K-0155) [p394]: withdrawn whole to this record: part "
                "(iii)'s precondition failed (Chip memory did not answer after RESET on this system).");
        } else {
            checks.expect(part_i && part_ii && part_iii,
                "P7 (opus:K-0164, sol:K-0155) [p394]: the RESET instruction drives only /RST and "
                "/BUSRST -- the system reset (i), not /HLT (ii), and expansion devices reset their "
                "autoconfiguration logic (iii) -- on every CPU setting");
        }

        // ---- contrasts, one per part -----------------------------------
        {
            std::string bin = dir + (mv == "P1A" ? "/prog_r1a.bin" : "/prog_r1.bin");
            R1Out r = run_r1(bin, 0, kC1);
            bool no_led_change = r.loaded && !r.pwr_change_seen;
            checks.expect(mv == "P1A" ? no_led_change : true,
                "P7-PART-I [p394,p370,p244]: with RESET replaced by NOP no LED change occurs (part (i) "
                "fails)");
        }
        {
            std::string bin = dir + (mv == "P2" ? "/prog_r1b.bin" : "/prog_r1.bin");
            R1Out r = run_r1(bin, 0, kC1);
            bool no_c2 = r.loaded && !r.c2_pixel_seen;
            checks.expect(mv == "P2" ? no_c2 : true,
                "P7-PART-II [p394,p223]: with the A+2 MOVE replaced by NOP no C2 pixel appears (part "
                "(ii) fails)");
        }
        if (gate_g_pass) {
            std::string bin_a = dir + (mv == "P6" ? "/prog_r6a.bin" : "/prog_r6.bin");
            std::string bin_am = dir + (mv == "P6" ? "/prog_r6am.bin" : "/prog_r6m.bin");
            R6Out ra = run_r6(bin_a, 0, false);
            R6Out ram = run_r6(bin_am, 0, true);
            bool part_iii_fails = ra.loaded && ra.done && !img_equal(ra.img3, ra.img0) &&
                                   ram.loaded && ram.done && ram.r3 && *ram.r3 == kP0Prime;
            checks.expect(mv == "P6" ? part_iii_fails : true,
                "P7-PART-III [p394,p431]: with RESET replaced by NOP the board stays configured (part "
                "(iii) fails)");
        }
    }

    // ---- display-anchor and no-exception precondition contrasts ---------
    {
        std::string bin = dir + (mv == "P1-DISPANCHOR" ? "/prog_r1_c1prime.bin" : "/prog_r1.bin");
        uint16_t bg = (mv == "P1-DISPANCHOR") ? kC1Prime : kC1;
        R1Out r = run_r1(bin, 0, bg);
        checks.expect(r.loaded && r.fref_ok,
            "P1-DISPANCHOR [p46,p48]: F_ref shows only COLOR00's value (C1, or C1'=$0C3F under the "
            "mutation) and black, confirming the display anchor");
    }
    {
        std::string bin = dir + (mv == "P1-NOEXCEPTION" ? "/prog_r1_priv.bin" : "/prog_r1.bin");
        R1Out r = run_r1(bin, 0, kC1);
        bool exception_fired = r.loaded && !r.no_exception;
        checks.expect(mv == "P1-NOEXCEPTION" ? exception_fired : (r.loaded && r.no_exception),
            "P1-NOEXCEPTION: under the mutation, switching to user mode before RESET turns it into a "
            "privilege violation (H_E runs: a C_E pixel and V_E appear); the unmutated run shows neither");
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
