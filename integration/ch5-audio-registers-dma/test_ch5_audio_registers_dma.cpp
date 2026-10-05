// test_ch5_audio_registers_dma.cpp -- ch5-audio-registers-dma: see
// SPECIFICATION.md. Audio location registers (P1), DMACONR readback (P2),
// each DMACON enable bit in turn (P3-P9), and the Copper writing the audio
// location registers under the danger bit (P10, P11).
//
// NMH_MUTATE selects a mutated program (or configuration) in place of its
// normal counterpart, one assertion or precondition at a time; none is
// exercised by a plain `make test`. The normal run (NMH_MUTATE unset)
// prints only the fixture, the preconditions and the assertions.
// NMH_ONLY=P1,P7 restricts a run to the named propositions (the fixture
// always runs), to replay one mutation without the rest of the suite.
//
// MUTATION FIXTURE stimulus red stored constant changed to DEAD instead of BEEF, via NMH_MUTATE=FIXTURE
// MUTATION FIXTURE-R stimulus red stored constant changed to DEAD instead of BEEF, via NMH_MUTATE=FIXTURE
// MUTATION P1a stimulus red channels 0-3's locations changed to A_x^H (the high-word contrast), via NMH_MUTATE=C1
// MUTATION P1a stimulus red channels 0-3's locations changed to A_x^L (the low-word contrast), via NMH_MUTATE=C2
// MUTATION P1a stimulus red channel 0 and channel 1's locations swapped (the register-to-channel contrast), via NMH_MUTATE=C3
// MUTATION P1b stimulus red channel 0 and channel 1's locations swapped (the register-to-channel contrast), via NMH_MUTATE=C3
// MUTATION P1-G1 stimulus red branch-to-self inserted before END, via NMH_MUTATE=G1
// MUTATION P1-G2 stimulus red channel 0's enabling write leaves DMAEN out, via NMH_MUTATE=G2
// MUTATION P1-G3 stimulus red one extra CPU write into A_0, through an absolute address, inside W_0, via NMH_MUTATE=G3
// MUTATION P1-G4 stimulus red the per-channel delay collapses to nothing, via NMH_MUTATE=G4
// MUTATION P1-G5 stimulus red SPREN also set before the windows, via NMH_MUTATE=G5
// MUTATION P2 stimulus red the SET write for DMACON bit 0 in the sweep is left out, via NMH_MUTATE=OMIT
// MUTATION P2-G1 stimulus red branch-to-self inserted before DONE, via NMH_MUTATE=G1
// MUTATION P3-G1 stimulus red branch-to-self inserted before END, via NMH_MUTATE=G1
// MUTATION P3-G2 stimulus red DMAEN left out of the enabling write, via NMH_MUTATE=G2
// MUTATION P3-G3 stimulus red one extra CPU write into P (the block under test) inside the window, via NMH_MUTATE=G3
// MUTATION P3-G4 stimulus red the delay collapses to nothing, via NMH_MUTATE=G4
// MUTATION P3-G5 stimulus red SPREN also set before the window, via NMH_MUTATE=G5
// MUTATION P4-G1 stimulus red branch-to-self inserted before END, via NMH_MUTATE=G1
// MUTATION P4-G2 stimulus red DMAEN left out of the enabling write, via NMH_MUTATE=G2
// MUTATION P4-G3 stimulus red one extra CPU write into L (the block under test), past its own content, via NMH_MUTATE=G3
// MUTATION P4-G4 stimulus red the delay collapses to nothing, via NMH_MUTATE=G4
// MUTATION P4-G5 stimulus red SPREN also set before the window, via NMH_MUTATE=G5
// MUTATION P5-G1 stimulus red branch-to-self inserted before END, via NMH_MUTATE=G1
// MUTATION P5-G2 stimulus red DMAEN left out of the enabling write, via NMH_MUTATE=G2
// MUTATION P5-G3 stimulus red one extra CPU write into Q (the block under test), past the blit's own destination footprint, via NMH_MUTATE=G3
// MUTATION P5-G4 stimulus red the delay collapses to nothing, via NMH_MUTATE=G4
// MUTATION P5-G5 stimulus red SPREN also set before the window, via NMH_MUTATE=G5
// MUTATION P6-G1 stimulus red branch-to-self inserted before END, via NMH_MUTATE=G1
// MUTATION P6-G2 stimulus red DMAEN left out of the enabling write, via NMH_MUTATE=G2
// MUTATION P6-G3 stimulus red one extra CPU write into S0 (the block under test), past its own content, via NMH_MUTATE=G3
// MUTATION P6-G4 stimulus red the delay collapses to nothing, via NMH_MUTATE=G4
// MUTATION P6-G5 stimulus red BPLEN also set before the window, via NMH_MUTATE=G5
// MUTATION P7 stimulus green BLTPRI cleared (share mode) -- the >=1-access branch of the same comparator, live and proven, via NMH_MUTATE=C1
// MUTATION P7-G1 stimulus red branch-to-self inserted before DONE, via NMH_MUTATE=G1
// MUTATION P7-G2 stimulus red BLTEN left out of the enabling write, via NMH_MUTATE=G2
// MUTATION P7-G3 stimulus red the blit shrunk to one word (height=1,width=1), via NMH_MUTATE=G3
// MUTATION P7-G(CPU still wanted the bus) stimulus red the loop runs once then executes STOP, via NMH_MUTATE=GSTOP
// MUTATION P8 stimulus red the BLTSIZE write replaced by a COLOR00 write, so no blit starts, via NMH_MUTATE=C1
// MUTATION P8-G1 stimulus red branch-to-self inserted before DONE, via NMH_MUTATE=G1
// MUTATION P8-G2 stimulus red BLTPRI set in step 2, via NMH_MUTATE=GBLTPRI
// MUTATION P8-G3 stimulus red the BLTSIZE write replaced by a COLOR00 write, so no blit starts (P8's own C1 contrast), via NMH_MUTATE=C1
// MUTATION P9-G(writes) stimulus red BLTCON0 exchanged between blits 1 and 2 (P9's own C1 contrast), so blit 1 no longer writes the $0000 this fixed precondition checks, via NMH_MUTATE=C1
// MUTATION P9-G1 stimulus red branch-to-self inserted before DONE, via NMH_MUTATE=G1
// MUTATION P9-G(after completion) stimulus red the wait before Z_1's store is dropped, via NMH_MUTATE=NOWAIT
// MUTATION P10-G(list ran) stimulus red COPEN omitted from the enabling write, via NMH_MUTATE=G_NOLIST
// MUTATION P11-G(finished) stimulus red branch-to-self inserted before END, via NMH_MUTATE=G1
// MUTATION P11-G(list ran) stimulus red COPEN omitted from the enabling write, via NMH_MUTATE=G_NOLIST

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

// ---- shared conventions (SPECIFICATION.md section 2) ------------------

// R, MK and G (SPECIFICATION.md section 2.3). All three are below $80000,
// where every program clears the ROM overlay first (ch5.i's CLEAR_OVL)
// before writing them.
constexpr uint32_t kR         = 0x07C000; // R
constexpr uint32_t kMarker    = 0x07D000; // MK
constexpr uint32_t kLog       = 0x078000; // G
// The harness's own RESULTS/DONE_ADDR convention (nanomig.i), inside the
// identity-mapped $080000+ range. Only the FIXTURE uses it -- P1-P11's own
// results area is R, above.
constexpr uint32_t kResults   = 0x0C0000;
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
// The CPU's own bookkeeping areas (G, R, MK), for checks that exclude
// them from "any write" scans (P5). G is $4000 bytes (SPECIFICATION.md
// 2.3); R and MK fit in the rest of the same low bank up to $80000.
constexpr uint32_t kCpuAreaLo = 0x078000;
constexpr uint32_t kCpuAreaHi = 0x080000;

constexpr uint64_t kBootBudget = 6'000'000;

// DMA-block memory map (byte addresses), matching ch5.i.
constexpr uint32_t kA[4]  = {0x60000, 0x61000, 0x62000, 0x63000};
constexpr uint32_t kAL[4] = {0x60100, 0x61100, 0x62100, 0x63100};
constexpr uint32_t kAH[4] = {0x70000, 0x71000, 0x72000, 0x73000};
constexpr uint32_t kB[4]  = {0x70800, 0x71800, 0x72800, 0x73800};
constexpr uint32_t kList  = 0x64000;
constexpr uint32_t kS0    = 0x65000;
constexpr uint32_t kN[7]  = {0x65100, 0x65200, 0x65300, 0x65400, 0x65500, 0x65600, 0x65700};
constexpr uint32_t kF     = 0x66000;
constexpr uint32_t kZb    = 0x66100;
constexpr uint32_t kP     = 0x68000;
constexpr uint32_t kQ     = 0x74000;
constexpr uint32_t kQ2    = 0x75000;

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

std::string hex16(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "%04X", v);
    return std::string(buf);
}

std::string num(long long v) { return std::to_string(v); }

// ---- capture ------------------------------------------------------------

struct Capture {
    bool loaded = false;
    bool released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
    std::vector<nmh::VideoSample> video;
};

// Preloads a block of words directly (no bus cycle, not logged -- see
// nanomig_harness.h poke_word), the same mechanism load_program() itself
// uses to place the program image. Every DMA block this package reads
// from (audio data, sprite structures, the Copper list, blit source
// patterns) is filled this way, not by a CPU instruction: the data is
// simply there before reset releases, matching data the CPU has no
// reason to write (its content is immaterial -- SPECIFICATION.md 2.3
// gives "any values" -- and no check in this package grades a write into one of
// these blocks except as the precondition G3 violation it would be).
void preload(nmh::NanomigHarness& h, uint32_t byte_addr, const std::vector<uint16_t>& words) {
    uint32_t w = word_addr(byte_addr);
    for (size_t i = 0; i < words.size(); ++i) h.poke_word(w + (uint32_t)i, words[i]);
}

std::vector<uint16_t> pattern(uint16_t base, size_t n) {
    std::vector<uint16_t> v(n);
    for (size_t i = 0; i < n; ++i) v[i] = uint16_t(base + i);
    return v;
}

// Fills every DMA block this package ever reads from with a fixed,
// deterministic (but otherwise immaterial -- SPECIFICATION.md 2.3) pattern.
// Harmless for a run that does not use a given block.
void preload_all_blocks(nmh::NanomigHarness& h) {
    for (int x = 0; x < 4; ++x) {
        preload(h, kA[x],  pattern(0x1000 + 0x100 * x, 8));
        preload(h, kAL[x], pattern(0x2000 + 0x100 * x, 8));
        preload(h, kAH[x], pattern(0x3000 + 0x100 * x, 8));
    }
    // Null sprite 0 by default (P2's own convention); P6 asks for the real
    // p.109 structure instead via run_capture's sprite0_real flag, which
    // pokes over this after preload_all_blocks runs. N_1..N_7 are always null.
    preload(h, kS0, {0x0000, 0x0000});
    for (int k = 0; k < 7; ++k) preload(h, kN[k], {0x0000, 0x0000});
    preload(h, kF, std::vector<uint16_t>(16, 0xFFFF));
    // Zb and P are already all-zero (mem_ default-initialised); no preload needed.
}

struct ExtraPreload { uint32_t addr; std::vector<uint16_t> words; };

Capture run_capture(const std::string& bin, uint64_t run_budget, nmh::Config cfg = nmh::Config{},
                     const std::vector<ExtraPreload>& extra = {}, bool sprite0_real = false) {
    Capture c;
    nmh::NanomigHarness h(cfg);
    preload_all_blocks(h);
    if (sprite0_real) {
        const uint16_t s0words[14] = {0x6D60,0x7200, 0x0990,0x07E0, 0x13C8,0x0FF0, 0x23C4,0x1FF8,
                                        0x13C8,0x0FF0, 0x0990,0x07E0, 0x0000,0x0000};
        preload(h, kS0, std::vector<uint16_t>(s0words, s0words + 14));
    }
    for (const auto& e : extra) preload(h, e.addr, e.words);
    c.loaded = h.load_program(bin) > 0;
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    c.done = h.run_until_write(kDoneAddr, kDoneValue, run_budget);
    c.log = h.ram_log();
    c.video = h.video();
    return c;
}

// ---- markers, windows, frames -------------------------------------------

std::optional<uint64_t> marker(const std::vector<nmh::RamAccess>& log, uint16_t id) {
    uint32_t w = word_addr(kMarker);
    for (const auto& a : log)
        if (a.is_write && a.word_addr == w && a.data == id) return a.cycle;
    return std::nullopt;
}

std::vector<uint64_t> vsync_starts(const std::vector<nmh::VideoSample>& v) {
    std::vector<uint64_t> out;
    if (v.empty()) return out;
    bool prev = v[0].vs;
    for (size_t i = 1; i < v.size(); ++i) {
        if (v[i].vs && !prev) out.push_back((uint64_t)i);
        prev = v[i].vs;
    }
    return out;
}

size_t count_window(const std::vector<uint64_t>& cycles, uint64_t lo, uint64_t hi) {
    size_t n = 0;
    for (auto c : cycles) if (c >= lo && c <= hi) ++n;
    return n;
}

size_t reads_at(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr, uint64_t lo, uint64_t hi) {
    uint32_t w = word_addr(byte_addr);
    size_t n = 0;
    for (const auto& a : log)
        if (!a.is_write && a.word_addr == w && a.cycle >= lo && a.cycle <= hi) ++n;
    return n;
}

size_t writes_in_range(const std::vector<nmh::RamAccess>& log, uint32_t lo_byte, uint32_t hi_byte,
                        uint64_t lo, uint64_t hi) {
    uint32_t wlo = word_addr(lo_byte), whi = word_addr(hi_byte);
    size_t n = 0;
    for (const auto& a : log)
        if (a.is_write && a.word_addr >= wlo && a.word_addr < whi && a.cycle >= lo && a.cycle <= hi) ++n;
    return n;
}

size_t reads_in_range(const std::vector<nmh::RamAccess>& log, uint32_t lo_byte, uint32_t hi_byte,
                       uint64_t lo, uint64_t hi) {
    uint32_t wlo = word_addr(lo_byte), whi = word_addr(hi_byte);
    size_t n = 0;
    for (const auto& a : log)
        if (!a.is_write && a.word_addr >= wlo && a.word_addr < whi && a.cycle >= lo && a.cycle <= hi) ++n;
    return n;
}

// A single [marker O, marker C] window, for the propositions with only one.
struct Window { uint64_t lo, hi; };
std::optional<Window> find_window(const std::vector<nmh::RamAccess>& log, uint16_t o_id, uint16_t c_id) {
    auto o = marker(log, o_id);
    auto c = marker(log, c_id);
    if (!o || !c) return std::nullopt;
    return Window{*o, *c};
}

// The chronological sequence of writes in [lo_byte,hi_byte), each with its
// data payload -- used where the *value* a register-read stored matters
// (P8/P9), not just whether an address was touched.
struct Wr { uint64_t cycle; uint16_t data; };
std::vector<Wr> writes_seq(const std::vector<nmh::RamAccess>& log, uint32_t lo_byte, uint32_t hi_byte) {
    uint32_t wlo = word_addr(lo_byte), whi = word_addr(hi_byte);
    std::vector<Wr> out;
    for (const auto& a : log)
        if (a.is_write && a.word_addr >= wlo && a.word_addr < whi) out.push_back({a.cycle, a.data});
    return out;
}

// The values a program stored at `base` (R, or a row of R), read from the
// RAM port's own log of the CPU's store instructions -- not peeked from
// memory content, so a value that was never actually written is absent,
// not stale. `count` words are expected, in the order they were stored
// (every store in this package fills R with sequential MOVE.W ...,(aN)+).
// A short result (fewer than `count` entries) means fewer stores happened
// than expected.
std::vector<uint16_t> stored_at(const std::vector<nmh::RamAccess>& log, uint32_t base, size_t count) {
    auto w = writes_seq(log, base, base + uint32_t(count) * 2);
    std::vector<uint16_t> out;
    for (auto& x : w) out.push_back(x.data);
    return out;
}

// Every program in this package is well under 512 bytes; PROGRAM_ORG is
// fixed at $080400 (nanomig.i). A read in this range is a CPU instruction
// fetch, not a DMA access (section 2.4).
constexpr uint32_t kProgLo = 0x080400;
constexpr uint32_t kProgHi = 0x080800;

}  // namespace

// ===========================================================================
// FIXTURE
// ===========================================================================
static void run_fixture(Checks& checks, const std::string& dir, const std::string& mv) {
    std::string bin = dir + (mv == "FIXTURE" ? "/prog_fixture_mut.bin" : "/prog_fixture.bin");
    nmh::Config cfg;
    nmh::NanomigHarness h(cfg);
    bool loaded = h.load_program(bin) > 0;
    bool released = loaded && h.boot(kBootBudget).cpu_released;
    auto done = released ? h.run_until_write(kDoneAddr, kDoneValue, 3'000'000) : std::nullopt;
    auto v = h.peek_logical(kResults);
    bool ok = loaded && released && done.has_value() && v.has_value() && *v == 0xBEEF;
    checks.expect(ok, "FIXTURE: the program stores a known constant ($BEEF) at RESULTS before anything reads one");

    // The path every proposition reads its results by: a store into R after
    // the ROM overlay is cleared, read back from the RAM-port log.
    auto r = stored_at(h.ram_log(), kR, 1);
    bool r_ok = loaded && released && done.has_value() && r.size() == 1 && r[0] == 0xBEEF;
    checks.expect(r_ok, "FIXTURE-R: the program stores a known constant ($BEEF) at R after clearing the ROM overlay, "
                        "and the RAM-port log shows that store");
}

// ===========================================================================
// P1: the CPU writes AUDxLCH/AUDxLCL, and each channel fetches its data
// from the location written (SPECIFICATION.md P1).
// ===========================================================================

namespace p1 {

constexpr uint16_t kO[4] = {0x0010, 0x0012, 0x0014, 0x0016};
constexpr uint16_t kC[4] = {0x0011, 0x0013, 0x0015, 0x0017};
constexpr uint16_t kEnd  = 0x00FF;

struct Windows {
    bool ok = false;
    uint64_t o[4], c[4];
    std::optional<uint64_t> end;
};

Windows find_windows(const Capture& cap) {
    Windows w;
    for (int x = 0; x < 4; ++x) {
        auto o = marker(cap.log, kO[x]);
        auto c = marker(cap.log, kC[x]);
        if (!o || !c) return w;
        w.o[x] = *o; w.c[x] = *c;
    }
    w.end = marker(cap.log, kEnd);
    w.ok = true;
    return w;
}

// (a): at least `min_reads` reads at kA[x] in W_x, for every x.
bool check_a(const Capture& cap, const Windows& w, size_t min_reads = 2) {
    if (!w.ok) return false;
    for (int x = 0; x < 4; ++x)
        if (reads_at(cap.log, kA[x], w.o[x], w.c[x]) < min_reads) return false;
    return true;
}

// (b): no read at kA[y] (y != x) in W_x, for every x.
bool check_b(const Capture& cap, const Windows& w) {
    if (!w.ok) return false;
    for (int x = 0; x < 4; ++x)
        for (int y = 0; y < 4; ++y)
            if (y != x && reads_at(cap.log, kA[y], w.o[x], w.c[x]) > 0) return false;
    return true;
}

const char kClaims[] =
    "p.266 5-appA-p266.c2, 5-appA-p266.c8, A-audxlch.acc, A-audxlcl.acc; "
    "p.290 5-appB-p290.c20, 5-appB-p290.c27, 5-appB-p290.c55, 5-appB-p290.c62, "
    "5-appB-p290.c90, 5-appB-p290.c97, 5-appB-p290.c125, 5-appB-p290.c132, "
    "B-aud0lch.acc, B-aud0lcl.acc, B-aud1lch.acc, B-aud1lcl.acc, "
    "B-aud2lch.acc, B-aud2lcl.acc, B-aud3lch.acc, B-aud3lcl.acc; "
    "p.298 5-appC-p298.c4, C-map.lch.acc; p.309 5-appC-p309.c4; "
    "p.263 5-appA-p263.c1, A-pre.c1";

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    std::string main_bin = dir + "/prog_p1.bin";

    // The contrasting cases C1-C3 replace the program of the main assertions
    // (a) and (b); the comparators are unchanged.
    std::string a_bin = main_bin;
    if (mv == "C1") a_bin = dir + "/prog_p1_c1.bin";
    else if (mv == "C2") a_bin = dir + "/prog_p1_c2.bin";
    else if (mv == "C3") a_bin = dir + "/prog_p1_c3.bin";
    std::string b_bin = main_bin;
    if (mv == "C3") b_bin = dir + "/prog_p1_c3.bin";

    // SPECIFICATION.md P1: "Run under OCS and ECS". Every line below is
    // graded over both runs.
    nmh::Config cfg_ecs; cfg_ecs.chipset_config = 0x08;
    nmh::Config cfg_ocs; cfg_ocs.chipset_config = 0x00;
    const struct { const char* name; nmh::Config cfg; } runs[2] = {{"ECS", cfg_ecs}, {"OCS", cfg_ocs}};

    bool a_ok = true, b_ok = true, g1_ok = true, g2_ok = true, g3_ok = true, g4_ok = true, g5_ok = true;
    for (const auto& rn : runs) {
        // ---- main assertions (a) and (b), each rerun under NMH_MUTATE ----
        Capture cap_a = run_capture(a_bin, run_budget, rn.cfg);
        Windows w_a = find_windows(cap_a);
        {
            std::string diag = std::string("P1 calibration ") + rn.name + ": done=" +
                (cap_a.done ? num((long long)*cap_a.done) : std::string("(timeout)"));
            if (w_a.ok) {
                for (int x = 0; x < 4; ++x) {
                    size_t v = count_window(vsync_starts(cap_a.video), w_a.o[x], w_a.c[x]);
                    diag += " V(W" + num(x) + ")=" + num((long long)v) +
                            " o=" + num((long long)w_a.o[x]) + " c=" + num((long long)w_a.c[x]);
                }
            } else {
                diag += " windows not all found";
            }
            checks.note(diag);
        }
        bool a_this = cap_a.loaded && cap_a.done.has_value() && check_a(cap_a, w_a, 2);

        Capture cap_b = (b_bin == a_bin) ? cap_a : run_capture(b_bin, run_budget, rn.cfg);
        Windows w_b = (b_bin == a_bin) ? w_a : find_windows(cap_b);
        bool b_this = cap_b.loaded && cap_b.done.has_value() && check_b(cap_b, w_b);

        // ---- preconditions ---------------------------------------------
        Capture cap_g1 = (mv == "G1") ? run_capture(dir + "/prog_p1_g1.bin", run_budget, rn.cfg) : cap_a;
        Windows w_g1 = (mv == "G1") ? find_windows(cap_g1) : w_a;
        bool g1_this = cap_g1.loaded && cap_g1.done.has_value() && w_g1.ok && w_g1.end.has_value();

        Capture cap_g2 = (mv == "G2") ? run_capture(dir + "/prog_p1_g2.bin", run_budget, rn.cfg) : cap_a;
        auto r_g2 = stored_at(cap_g2.log, kR, 4);
        bool g2_this = cap_g2.loaded && cap_g2.done.has_value() && r_g2.size() == 4;
        if (g2_this) for (int x = 0; x < 4; ++x) {
            uint16_t v = r_g2[x];
            if (!(v & 0x0200) || !(v & (1 << x))) { g2_this = false; break; } // DMAEN + channel x bit
        }

        Capture cap_g4 = (mv == "G4") ? run_capture(dir + "/prog_p1_g4.bin", run_budget, rn.cfg) : cap_a;
        Windows w_g4 = (mv == "G4") ? find_windows(cap_g4) : w_a;
        bool g4_this = cap_g4.loaded && cap_g4.done.has_value() && w_g4.ok;
        if (g4_this) for (int x = 0; x < 4; ++x) {
            size_t v = count_window(vsync_starts(cap_g4.video), w_g4.o[x], w_g4.c[x]);
            if (v < 2) { g4_this = false; break; }
        }
        if (g4_this) for (int x = 0; x < 3; ++x) {
            size_t gapv = count_window(vsync_starts(cap_g4.video), w_g4.c[x], w_g4.o[x + 1]);
            if (gapv < 1) { g4_this = false; break; }
        }

        Capture cap_g5 = (mv == "G5") ? run_capture(dir + "/prog_p1_g5.bin", run_budget, rn.cfg) : cap_a;
        auto r_g5 = stored_at(cap_g5.log, kR, 4);
        bool g5_this = cap_g5.loaded && cap_g5.done.has_value() && r_g5.size() == 4;
        if (g5_this) for (int x = 0; x < 4; ++x) {
            uint16_t bits = r_g5[x] & 0x01FF; // bits 8-0
            if (bits != (1u << x)) { g5_this = false; break; } // only channel x's own bit
        }

        // G3: no CPU write falls into any DMA block during any window.
        // Checked as a live invariant on every run, and mutated (prog_p1_g3.s,
        // one extra CPU write into A_0 inside W_0).
        Capture cap_g3 = (mv == "G3") ? run_capture(dir + "/prog_p1_g3.bin", run_budget, rn.cfg) : cap_a;
        Windows w_g3 = (mv == "G3") ? find_windows(cap_g3) : w_a;
        bool g3_this = w_g3.ok;
        if (g3_this) for (int x = 0; x < 4 && g3_this; ++x) {
            if (writes_in_range(cap_g3.log, kA[x], kA[x] + 16, w_g3.o[x], w_g3.c[x]) != 0) g3_this = false;
            if (writes_in_range(cap_g3.log, kAL[x], kAL[x] + 16, w_g3.o[x], w_g3.c[x]) != 0) g3_this = false;
            if (writes_in_range(cap_g3.log, kAH[x], kAH[x] + 16, w_g3.o[x], w_g3.c[x]) != 0) g3_this = false;
        }

        checks.note(std::string("P1 ") + rn.name + ": a=" + (a_this ? "ok" : "FAILED") + " b=" +
                    (b_this ? "ok" : "FAILED"));
        a_ok = a_ok && a_this; b_ok = b_ok && b_this;
        g1_ok = g1_ok && g1_this; g2_ok = g2_ok && g2_this; g3_ok = g3_ok && g3_this;
        g4_ok = g4_ok && g4_this; g5_ok = g5_ok && g5_this;
    }

    checks.expect(a_ok,
        std::string("P1a [") + kClaims +
        "]: for every channel x=0..3, at least 2 reads at A_x in W_x once the location is written and the channel "
        "started, under OCS and ECS");
    checks.expect(b_ok,
        std::string("P1b [") + kClaims +
        "]: for every channel x=0..3, no read at A_y (y != x) in W_x -- no other channel takes the value, "
        "under OCS and ECS");
    checks.expect(g1_ok,
        "P1-G1: the program finished -- END was written and every DMACONR readback was stored (OCS and ECS)");
    checks.expect(g2_ok,
        "P1-G2 [p.144]: each stored DMACONR readback shows DMAEN (bit 9) and the enabled channel's own bit set "
        "(OCS and ECS)");
    checks.expect(g4_ok,
        "P1-G4: V(W_x) >= 2 for every channel window, and at least one vertical-sync pulse "
        "separates C_x from O_(x+1) (OCS and ECS)");
    checks.expect(g5_ok,
        "P1-G5: of DMACON bits 8-0 in each stored readback, only the channel x being enabled is set "
        "(OCS and ECS)");
    checks.expect(g3_ok, "P1-G3: zero writes observed in any A_x/A_x^L/A_x^H range across all four windows "
                         "(OCS and ECS)");

    // The "only" of "write-only" is not graded; the write half is.
    checks.record(
        "P1: the 'only' of 'write-only' is not graded. p.264 says of a read of a write-only register only that it "
        "'may trash the register and crash the system', which gives no expected value, so no program reads "
        "AUDxLCH or AUDxLCL "
        "(half graded: 5-appA-p266.c2, 5-appA-p266.c8, A-audxlch.acc, A-audxlcl.acc, "
        "5-appB-p290.c20, 5-appB-p290.c27, 5-appB-p290.c55, 5-appB-p290.c62, "
        "5-appB-p290.c90, 5-appB-p290.c97, 5-appB-p290.c125, 5-appB-p290.c132, "
        "B-aud0lch.acc, B-aud0lcl.acc, B-aud1lch.acc, B-aud1lcl.acc, "
        "B-aud2lch.acc, B-aud2lcl.acc, B-aud3lch.acc, B-aud3lcl.acc, "
        "5-appC-p298.c4, C-map.lch.acc, 5-appC-p309.c4)");
}

}  // namespace p1

// ===========================================================================
// P2: DMACONR at $DFF002 reads back the DMA control bits held by DMACON
// (SPECIFICATION.md P2).
// ===========================================================================

namespace p2 {

constexpr uint16_t kExpected[26] = {
    0x0000, 0x0200,
    0x0201, 0x0200,  0x0202, 0x0200,  0x0204, 0x0200,  0x0208, 0x0200,  0x0210, 0x0200,
    0x0220, 0x0200,  0x0240, 0x0200,  0x0280, 0x0200,  0x0300, 0x0200,  0x0600, 0x0200,
    0x07FF, 0x05FF, 0x07FF, 0x0000
};

const char kClaims[] =
    "p.278 5-appA-p278.c5, 5-appA-p278.c6, 5-appA-p278.c8, "
    "A-dmaconr.acc, A-dmaconr.addr; p.289 5-appB-p289.c1, 5-appB-p289.c2, "
    "5-appB-p289.c4; p.263 5-appA-p263.c1, A-pre.c1";

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    std::vector<ExtraPreload> extra = {{kList, {0xFFFF, 0xFFFE}}};

    std::string bin = dir + "/prog_p2.bin";
    if (mv == "OMIT") bin = dir + "/prog_p2_omit.bin";

    // SPECIFICATION.md P2: "Run under OCS and ECS".
    nmh::Config cfg_ecs; cfg_ecs.chipset_config = 0x08;
    nmh::Config cfg_ocs; cfg_ocs.chipset_config = 0x00;
    const struct { const char* name; nmh::Config cfg; } runs[2] = {{"ECS", cfg_ecs}, {"OCS", cfg_ocs}};

    bool all_ok = true, all_g1 = true;
    for (const auto& rn : runs) {
        Capture cap = run_capture(bin, run_budget, rn.cfg, extra);
        auto r = stored_at(cap.log, kR, 26);

        bool ok = cap.loaded && cap.done.has_value() && r.size() == 26;
        int first_bad = -1;
        if (ok) for (int i = 0; i < 26; ++i) {
            if ((r[i] & 0x07FF) != kExpected[i]) { ok = false; first_bad = i; break; }
        }
        if (!ok && first_bad >= 0 && (size_t)first_bad < r.size())
            checks.note(std::string("P2 ") + rn.name + ": row " + num(first_bad) + " stored " +
                        hex16(r[first_bad] & 0x07FF) + ", expected " + hex16(kExpected[first_bad]));
        all_ok = all_ok && ok;

        Capture cap_g1 = (mv == "G1") ? run_capture(dir + "/prog_p2_g1.bin", run_budget, rn.cfg, extra) : cap;
        auto r_g1 = stored_at(cap_g1.log, kR, 26);
        all_g1 = all_g1 && cap_g1.loaded && cap_g1.done.has_value() && r_g1.size() == 26;
    }
    checks.expect(all_ok, std::string("P2 [") + kClaims +
        "]: each of the 26 stored DMACONR readbacks, masked with $07FF, equals the proposition's table "
        "(row 3 grades every DMA control bit on its own; row 5 grades bits written as zero staying unchanged), "
        "under OCS and ECS");
    checks.expect(all_g1, "P2-G1: all 26 values were stored (OCS and ECS)");

    // The "only" of "read-only" is not graded; the read half is.
    checks.record(
        "P2: the 'only' of 'read-only' is not graded. p.263 says of a write to a read-only register only that it "
        "'will cause unpredictable results', which gives no expected value, so no program writes $DFF002 "
        "(half graded: 5-appA-p278.c6, A-dmaconr.acc, 5-appB-p289.c2)");
}

}  // namespace p2

// ===========================================================================
// P3: BPLEN (bit 8) turns bitplane DMA on (SPECIFICATION.md P3).
// ===========================================================================

namespace p3 {

constexpr uint16_t kO = 0x0020, kC = 0x0021;
const char kClaims[] = "p.278 5-appA-p278.c17, A-dmacon.b08";

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    bool bplen_on = (mv != "C1");
    std::string bin = dir + (mv == "C1" ? "/prog_p3_c1.bin" : "/prog_p3.bin");
    Capture cap = run_capture(bin, run_budget);
    auto w = find_window(cap.log, kO, kC);

    bool ok = cap.loaded && cap.done.has_value() && w.has_value();
    if (ok) {
        size_t at_p = reads_at(cap.log, kP, w->lo, w->hi);
        size_t in_range = reads_in_range(cap.log, kP, kP + 0x8000, w->lo, w->hi);
        ok = bplen_on ? (at_p >= 1 && in_range > 0) : (in_range == 0);
    }
    checks.expect(ok, std::string("P3 [") + kClaims +
        "]: with BPLEN set, at least one read at P and more than zero reads in [P,P+$8000) in the window; "
        "with BPLEN clear, none");

    Capture cap_g1 = (mv == "G1") ? run_capture(dir + "/prog_p3_g1.bin", run_budget) : cap;
    checks.expect(cap_g1.loaded && cap_g1.done.has_value() && find_window(cap_g1.log, kO, kC).has_value(),
        "P3-G1: the program finished");

    Capture cap_g2 = (mv == "G2") ? run_capture(dir + "/prog_p3_g2.bin", run_budget) : cap;
    auto r_g2 = stored_at(cap_g2.log, kR, 1);
    bool g2_ok = cap_g2.loaded && cap_g2.done.has_value() && r_g2.size() == 1;
    if (g2_ok) { uint16_t v = r_g2[0]; g2_ok = (v & 0x0200) && (bool(v & 0x0100) == bplen_on); }
    checks.expect(g2_ok, "P3-G2 [p.144]: the stored DMACONR readback shows DMAEN (bit 9) set and BPLEN (bit 8) "
                         "matching this run's own enabling write");

    Capture cap_g4 = (mv == "G4") ? run_capture(dir + "/prog_p3_g4.bin", run_budget) : cap;
    auto w_g4 = (mv == "G4") ? find_window(cap_g4.log, kO, kC) : w;
    bool g4_ok = cap_g4.loaded && cap_g4.done.has_value() && w_g4.has_value() &&
                 count_window(vsync_starts(cap_g4.video), w_g4->lo, w_g4->hi) >= 3;
    checks.expect(g4_ok, "P3-G4: V(W) >= 3 (two whole frames and the display-window lines inside the window)");

    Capture cap_g5 = (mv == "G5") ? run_capture(dir + "/prog_p3_g5.bin", run_budget) : cap;
    auto r_g5 = stored_at(cap_g5.log, kR, 1);
    bool g5_ok = cap_g5.loaded && cap_g5.done.has_value() && r_g5.size() == 1;
    if (g5_ok) g5_ok = (r_g5[0] & 0x01FF) == (bplen_on ? 0x0100 : 0x0000);
    checks.expect(g5_ok, "P3-G5: bit 8 (BPLEN) is the only DMACON bit 8-0 set in the stored readback");

    // G3: no CPU write falls into P during the window. Live invariant on
    // every run, mutated by prog_p3_g3.s (one extra CPU write into P).
    Capture cap_g3 = (mv == "G3") ? run_capture(dir + "/prog_p3_g3.bin", run_budget) : cap;
    auto w_g3 = (mv == "G3") ? find_window(cap_g3.log, kO, kC) : w;
    bool g3_ok = w_g3.has_value() && writes_in_range(cap_g3.log, kP, kP + 0x8000, w_g3->lo, w_g3->hi) == 0;
    checks.expect(g3_ok, "P3-G3: zero writes observed in [P,P+$8000) in the window");
}

}  // namespace p3

// ===========================================================================
// P4: COPEN (bit 7) turns Copper DMA on (SPECIFICATION.md P4).
// ===========================================================================

namespace p4 {

constexpr uint16_t kO = 0x0030, kC = 0x0031;
const char kClaims[] = "p.278 5-appA-p278.c18, A-dmacon.b07";

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    bool copen_on = (mv != "C1");
    std::vector<ExtraPreload> extra = {{kList, {0x0180, 0x0F00, 0xFFFF, 0xFFFE}}};
    std::string bin = dir + (mv == "C1" ? "/prog_p4_c1.bin" : "/prog_p4.bin");
    Capture cap = run_capture(bin, run_budget, nmh::Config{}, extra);
    auto w = find_window(cap.log, kO, kC);

    bool ok = cap.loaded && cap.done.has_value() && w.has_value();
    if (ok) {
        size_t at_l = reads_at(cap.log, kList, w->lo, w->hi);
        size_t in_range = reads_in_range(cap.log, kList, kList + 8, w->lo, w->hi);
        ok = copen_on ? (at_l >= 2) : (in_range == 0);
    }
    checks.expect(ok, std::string("P4 [") + kClaims +
        "]: with COPEN set, at least 2 reads at L in the window (two vertical blankings, each restarting the "
        "Copper at L); with COPEN clear, none in [L,L+8)");

    Capture cap_g1 = (mv == "G1") ? run_capture(dir + "/prog_p4_g1.bin", run_budget, nmh::Config{}, extra) : cap;
    checks.expect(cap_g1.loaded && cap_g1.done.has_value() && find_window(cap_g1.log, kO, kC).has_value(),
        "P4-G1: the program finished");

    Capture cap_g2 = (mv == "G2") ? run_capture(dir + "/prog_p4_g2.bin", run_budget, nmh::Config{}, extra) : cap;
    auto r_g2 = stored_at(cap_g2.log, kR, 1);
    bool g2_ok = cap_g2.loaded && cap_g2.done.has_value() && r_g2.size() == 1;
    if (g2_ok) { uint16_t v = r_g2[0]; g2_ok = (v & 0x0200) && (bool(v & 0x0080) == copen_on); }
    checks.expect(g2_ok, "P4-G2: the stored DMACONR readback shows DMAEN (bit 9) set and COPEN (bit 7) "
                         "matching this run's own enabling write");

    Capture cap_g4 = (mv == "G4") ? run_capture(dir + "/prog_p4_g4.bin", run_budget, nmh::Config{}, extra) : cap;
    auto w_g4 = (mv == "G4") ? find_window(cap_g4.log, kO, kC) : w;
    bool g4_ok = cap_g4.loaded && cap_g4.done.has_value() && w_g4.has_value() &&
                 count_window(vsync_starts(cap_g4.video), w_g4->lo, w_g4->hi) >= 3;
    checks.expect(g4_ok, "P4-G4: V(W) >= 3");

    Capture cap_g5 = (mv == "G5") ? run_capture(dir + "/prog_p4_g5.bin", run_budget, nmh::Config{}, extra) : cap;
    auto r_g5 = stored_at(cap_g5.log, kR, 1);
    bool g5_ok = cap_g5.loaded && cap_g5.done.has_value() && r_g5.size() == 1;
    if (g5_ok) g5_ok = (r_g5[0] & 0x01FF) == (copen_on ? 0x0080 : 0x0000);
    checks.expect(g5_ok, "P4-G5: bit 7 (COPEN) is the only DMACON bit 8-0 set in the stored readback");

    // G3: no CPU write falls into L during the window. Live invariant on
    // every run, mutated by prog_p4_g3.s (one extra CPU write into L).
    Capture cap_g3 = (mv == "G3") ? run_capture(dir + "/prog_p4_g3.bin", run_budget, nmh::Config{}, extra) : cap;
    auto w_g3 = (mv == "G3") ? find_window(cap_g3.log, kO, kC) : w;
    bool g3_ok = w_g3.has_value() && writes_in_range(cap_g3.log, kList, kList + 64, w_g3->lo, w_g3->hi) == 0;
    checks.expect(g3_ok, "P4-G3: zero writes observed in [L,L+64) in the window");
}

}  // namespace p4

// ===========================================================================
// P5: BLTEN (bit 6) turns blitter DMA on (SPECIFICATION.md P5).
// ===========================================================================

namespace p5 {

constexpr uint16_t kO = 0x0040, kC = 0x0041;
const char kClaims[] = "p.278 5-appA-p278.c19, A-dmacon.b06";

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    bool blten_on = (mv != "C1");
    std::string bin = dir + (mv == "C1" ? "/prog_p5_c1.bin" : "/prog_p5.bin");
    Capture cap = run_capture(bin, run_budget);
    auto w = find_window(cap.log, kO, kC);

    bool ok = cap.loaded && cap.done.has_value() && w.has_value();
    if (ok) {
        size_t writes_q = writes_in_range(cap.log, kQ, kQ + 0x200, w->lo, w->hi);
        if (blten_on) {
            ok = writes_q >= 1;
        } else {
            // "no write anywhere outside the CPU areas": G, R and MK
            // (SPECIFICATION.md 2.3) all fall inside one contiguous span,
            // kCpuAreaLo..kCpuAreaHi.
            size_t writes_anywhere = writes_in_range(cap.log, 0x000000, 0x200000, w->lo, w->hi) -
                                      writes_in_range(cap.log, kCpuAreaLo, kCpuAreaHi, w->lo, w->hi);
            ok = writes_q == 0 && writes_anywhere == 0;
        }
    }
    checks.expect(ok, std::string("P5 [") + kClaims +
        "]: with BLTEN set, at least one write in [Q,Q+$200) in the window; with BLTEN clear, none there and "
        "no write anywhere outside the CPU's own areas");

    Capture cap_g1 = (mv == "G1") ? run_capture(dir + "/prog_p5_g1.bin", run_budget) : cap;
    checks.expect(cap_g1.loaded && cap_g1.done.has_value() && find_window(cap_g1.log, kO, kC).has_value(),
        "P5-G1: the program finished");

    Capture cap_g2 = (mv == "G2") ? run_capture(dir + "/prog_p5_g2.bin", run_budget) : cap;
    auto r_g2 = stored_at(cap_g2.log, kR, 1);
    bool g2_ok = cap_g2.loaded && cap_g2.done.has_value() && r_g2.size() == 1;
    if (g2_ok) { uint16_t v = r_g2[0]; g2_ok = (v & 0x0200) && (bool(v & 0x0040) == blten_on); }
    checks.expect(g2_ok, "P5-G2: the stored DMACONR readback shows DMAEN (bit 9) set and BLTEN (bit 6) "
                         "matching this run's own enabling write");

    Capture cap_g4 = (mv == "G4") ? run_capture(dir + "/prog_p5_g4.bin", run_budget) : cap;
    auto w_g4 = (mv == "G4") ? find_window(cap_g4.log, kO, kC) : w;
    bool g4_ok = cap_g4.loaded && cap_g4.done.has_value() && w_g4.has_value() &&
                 count_window(vsync_starts(cap_g4.video), w_g4->lo, w_g4->hi) >= 1;
    checks.expect(g4_ok, "P5-G4: V(W) >= 1");

    Capture cap_g5 = (mv == "G5") ? run_capture(dir + "/prog_p5_g5.bin", run_budget) : cap;
    auto r_g5 = stored_at(cap_g5.log, kR, 1);
    bool g5_ok = cap_g5.loaded && cap_g5.done.has_value() && r_g5.size() == 1;
    if (g5_ok) g5_ok = (r_g5[0] & 0x01FF) == (blten_on ? 0x0040 : 0x0000);
    checks.expect(g5_ok, "P5-G5: bit 6 (BLTEN) is the only DMACON bit 8-0 set in the stored readback");

    // G3: no CPU write falls into Q during the window, outside the blit's
    // own destination footprint [Q,Q+$200) (which the main assertion
    // above already grades on its own terms). Live invariant on every
    // run, mutated by prog_p5_g3.s (one extra CPU write into Q).
    Capture cap_g3 = (mv == "G3") ? run_capture(dir + "/prog_p5_g3.bin", run_budget) : cap;
    auto w_g3 = (mv == "G3") ? find_window(cap_g3.log, kO, kC) : w;
    bool g3_ok = w_g3.has_value() &&
                 writes_in_range(cap_g3.log, kQ + 0x200, kQ + 0x2000, w_g3->lo, w_g3->hi) == 0;
    checks.expect(g3_ok, "P5-G3: zero writes observed in [Q+$200,Q+$2000) in the window");
}

}  // namespace p5

// ===========================================================================
// P6: SPREN (bit 5) turns sprite DMA on for all eight sprites
// (SPECIFICATION.md P6).
// ===========================================================================

namespace p6 {

constexpr uint16_t kO = 0x0050, kC = 0x0051;
const char kClaims[] = "p.278 5-appA-p278.c20, A-dmacon.b05";

bool all16_read(const Capture& cap, const Window& w) {
    if (reads_at(cap.log, kS0, w.lo, w.hi) < 1) return false;
    if (reads_at(cap.log, kS0 + 2, w.lo, w.hi) < 1) return false;
    for (int k = 0; k < 7; ++k) {
        if (reads_at(cap.log, kN[k], w.lo, w.hi) < 1) return false;
        if (reads_at(cap.log, kN[k] + 2, w.lo, w.hi) < 1) return false;
    }
    return true;
}

bool none_of_8_blocks(const Capture& cap, const Window& w) {
    if (reads_in_range(cap.log, kS0, kS0 + 0x100, w.lo, w.hi) != 0) return false;
    for (int k = 0; k < 7; ++k)
        if (reads_in_range(cap.log, kN[k], kN[k] + 0x100, w.lo, w.hi) != 0) return false;
    return true;
}

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    bool spren_on = (mv != "C1");
    std::string bin = dir + (mv == "C1" ? "/prog_p6_c1.bin" : "/prog_p6.bin");
    Capture cap = run_capture(bin, run_budget, nmh::Config{}, {}, /*sprite0_real=*/true);
    auto w = find_window(cap.log, kO, kC);

    bool ok = cap.loaded && cap.done.has_value() && w.has_value();
    if (ok) ok = spren_on ? all16_read(cap, *w) : none_of_8_blocks(cap, *w);
    checks.expect(ok, std::string("P6 [") + kClaims +
        "]: with SPREN set, each of the 16 addresses (2 words x 8 sprite structures) is read at least once "
        "in the window; with SPREN clear, none of the eight blocks is read at all");

    Capture cap_g1 = (mv == "G1") ? run_capture(dir + "/prog_p6_g1.bin", run_budget, nmh::Config{}, {}, true) : cap;
    checks.expect(cap_g1.loaded && cap_g1.done.has_value() && find_window(cap_g1.log, kO, kC).has_value(),
        "P6-G1: the program finished");

    Capture cap_g2 = (mv == "G2") ? run_capture(dir + "/prog_p6_g2.bin", run_budget, nmh::Config{}, {}, true) : cap;
    auto r_g2 = stored_at(cap_g2.log, kR, 1);
    bool g2_ok = cap_g2.loaded && cap_g2.done.has_value() && r_g2.size() == 1;
    if (g2_ok) { uint16_t v = r_g2[0]; g2_ok = (v & 0x0200) && (bool(v & 0x0020) == spren_on); }
    checks.expect(g2_ok, "P6-G2: the stored DMACONR readback shows DMAEN (bit 9) set and SPREN (bit 5) "
                         "matching this run's own enabling write");

    Capture cap_g4 = (mv == "G4") ? run_capture(dir + "/prog_p6_g4.bin", run_budget, nmh::Config{}, {}, true) : cap;
    auto w_g4 = (mv == "G4") ? find_window(cap_g4.log, kO, kC) : w;
    bool g4_ok = cap_g4.loaded && cap_g4.done.has_value() && w_g4.has_value() &&
                 count_window(vsync_starts(cap_g4.video), w_g4->lo, w_g4->hi) >= 3;
    checks.expect(g4_ok, "P6-G4: V(W) >= 3");

    // G5's own generic check, and the "bitplane DMA off" precondition (the
    // same stored-value clause, bit 8): only bit 5 (SPREN) among bits 8-0.
    Capture cap_g5 = (mv == "G5") ? run_capture(dir + "/prog_p6_g5.bin", run_budget, nmh::Config{}, {}, true) : cap;
    auto r_g5 = stored_at(cap_g5.log, kR, 1);
    bool g5_ok = cap_g5.loaded && cap_g5.done.has_value() && r_g5.size() == 1;
    if (g5_ok) g5_ok = (r_g5[0] & 0x01FF) == (spren_on ? 0x0020 : 0x0000);
    checks.expect(g5_ok, "P6-G5: bit 5 (SPREN) is the only DMACON bit 8-0 set in the stored readback "
                         "(bit 8, bitplane DMA, is clear -- the same clause as the bitplane-off precondition)");

    // G3: no CPU write falls into S0 (the block under test) during the
    // window. Live invariant on every run, mutated by prog_p6_g3.s (one
    // extra CPU write into S0, past its own 14-word content).
    Capture cap_g3 = (mv == "G3") ? run_capture(dir + "/prog_p6_g3.bin", run_budget, nmh::Config{}, {}, true) : cap;
    auto w_g3 = (mv == "G3") ? find_window(cap_g3.log, kO, kC) : w;
    bool g3_ok = w_g3.has_value() && writes_in_range(cap_g3.log, kS0, kS0 + 0x100, w_g3->lo, w_g3->hi) == 0;
    checks.expect(g3_ok, "P6-G3: zero writes observed in [S0,S0+$100) in the window");
}

}  // namespace p6

// ===========================================================================
// P7: with BLTPRI (bit 10) set, the CPU gets no Chip-memory cycle while
// blitter DMA runs (SPECIFICATION.md P7).
// ===========================================================================

namespace p7 {

const char kClaims[] = "p.278 5-appA-p278.c15, A-dmacon.b10";

// "CPU accesses" per section 2.4: the program image (instruction fetches)
// and G (the loop's own counter writes). R/MK/stack are not touched
// during the interval this proposition measures.
size_t cpu_accesses_in(const std::vector<nmh::RamAccess>& log, uint64_t lo, uint64_t hi) {
    size_t n = reads_in_range(log, kProgLo, kProgHi, lo, hi);
    n += writes_in_range(log, kLog, kLog + 0x2000, lo, hi);
    return n;
}

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    bool bltpri_on = (mv != "C1");
    std::string bin = dir + "/prog_p7.bin";
    if (mv == "C1") bin = dir + "/prog_p7_c1.bin";
    Capture cap = run_capture(bin, run_budget);

    auto q_writes = writes_in_range(cap.log, kQ, kQ + 0x2000, 0, UINT64_MAX);
    std::vector<uint64_t> q_write_cycles;
    for (const auto& a : cap.log)
        if (a.is_write && a.word_addr >= word_addr(kQ) && a.word_addr < word_addr(kQ + 0x2000)) q_write_cycles.push_back(a.cycle);

    bool ok = cap.loaded && q_write_cycles.size() >= 2;
    if (ok) {
        uint64_t t_first = q_write_cycles.front(), t_last = q_write_cycles.back();
        for (auto c : q_write_cycles) { t_first = std::min(t_first, c); t_last = std::max(t_last, c); }
        size_t cpu_n = (t_last > t_first + 1) ? cpu_accesses_in(cap.log, t_first + 1, t_last - 1) : 0;
        if (bltpri_on) ok = (cpu_n == 0);
        else ok = (cpu_n >= 1);
        checks.note("P7: T_first=" + num((long long)t_first) + " T_last=" + num((long long)t_last) +
                    " CPU accesses in between=" + num((long long)cpu_n));
    }
    checks.expect(ok, std::string("P7 [") + kClaims +
        "]: with BLTPRI set, zero CPU accesses in (T_first,T_last); with BLTPRI clear, at least one (p.198)");

    Capture cap_g1 = (mv == "G1") ? run_capture(dir + "/prog_p7_g1.bin", run_budget) : cap;
    checks.expect(cap_g1.loaded && cap_g1.done.has_value(), "P7-G1: the program finished");

    Capture cap_g2 = (mv == "G2") ? run_capture(dir + "/prog_p7_g2.bin", run_budget) : cap;
    auto r_g2 = stored_at(cap_g2.log, kR, 1);
    bool g2_ok = cap_g2.loaded && r_g2.size() == 1;
    if (g2_ok) {
        uint16_t v = r_g2[0];
        bool want10 = bltpri_on;
        g2_ok = (v & 0x0200) && (bool(v & 0x0040) == true) && (bool(v & 0x0400) == want10) &&
                ((v & 0x01FF & ~0x0040) == 0);
    }
    checks.expect(g2_ok, "P7-G2: the stored DMACONR readback shows DMAEN and BLTEN set, BLTPRI matching this "
                         "run's own enabling write, and no other bit among 8-0");

    Capture cap_g3 = (mv == "G3") ? run_capture(dir + "/prog_p7_g3.bin", run_budget) : cap;
    size_t g3_qwrites = 0;
    for (const auto& a : cap_g3.log)
        if (a.is_write && a.word_addr >= word_addr(kQ) && a.word_addr < word_addr(kQ + 0x2000)) ++g3_qwrites;
    checks.expect(cap_g3.loaded && g3_qwrites >= 2, "P7-G3: the blit ran -- at least 2 writes in the Q range");

    Capture cap_gs = (mv == "GSTOP") ? run_capture(dir + "/prog_p7_gstop.bin", run_budget) : cap;
    std::vector<uint64_t> gs_q;
    for (const auto& a : cap_gs.log)
        if (a.is_write && a.word_addr >= word_addr(kQ) && a.word_addr < word_addr(kQ + 0x2000)) gs_q.push_back(a.cycle);
    bool gstop_ok = cap_gs.loaded && !gs_q.empty();
    if (gstop_ok) {
        uint64_t t_last_gs = *std::max_element(gs_q.begin(), gs_q.end());
        size_t after = 0, before = 0;
        for (const auto& a : cap_gs.log)
            if (a.is_write && a.word_addr >= word_addr(kLog) && a.word_addr < word_addr(kLog + 0x2000)) {
                if (a.cycle > t_last_gs) ++after; else ++before;
            }
        gstop_ok = (after >= 1) && (before < 2048);
    }
    checks.expect(gstop_ok, "P7-G(CPU still wanted the bus): at least one CPU write into G falls after "
                            "T_last, with fewer than 2048 recorded before it");
}

}  // namespace p7

// ===========================================================================
// P8: BBUSY (bit 14) reads 1 while a blit is in progress and 0 when no blit
// is in progress (SPECIFICATION.md P8).
// ===========================================================================

namespace p8 {

const char kClaims[] =
    "p.278 5-appA-p278.c13, A-dmacon.b14; p.278 5-appA-p278.c10, 5-appA-p278.c8; p.289 5-appB-p289.c4";

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    std::string bin = dir + "/prog_p8.bin";
    if (mv == "C1") bin = dir + "/prog_p8_c1.bin";
    else if (mv == "GBLTPRI") bin = dir + "/prog_p8_gbltpri.bin";
    Capture cap = run_capture(bin, run_budget);

    auto g_all = writes_seq(cap.log, kLog, kLog + 0x4200);
    std::vector<uint64_t> q_cycles;
    for (const auto& a : cap.log)
        if (a.is_write && a.word_addr >= word_addr(kQ) && a.word_addr < word_addr(kQ + 0x2000))
            q_cycles.push_back(a.cycle);

    bool have_idle = g_all.size() >= 4;
    bool idle_ok = have_idle;
    for (int i = 0; i < 4 && idle_ok; ++i) if (g_all[i].data & 0x4000) idle_ok = false;

    std::vector<uint64_t> restC; std::vector<uint16_t> restV;
    for (size_t i = 4; i < g_all.size(); ++i) { restC.push_back(g_all[i].cycle); restV.push_back(g_all[i].data); }
    size_t total_rest = restC.size();
    bool struct_ok = total_rest >= 9;  // at least 1 poll read + 8 extra
    size_t N = struct_ok ? total_rest - 8 : 0;

    std::optional<uint64_t> t_last;
    if (!q_cycles.empty()) t_last = *std::max_element(q_cycles.begin(), q_cycles.end());
    bool blit_ran = t_last.has_value() && q_cycles.size() >= 2;

    size_t b_nonvacuous = 0;
    bool b_ok = true;
    if (struct_ok && blit_ran) {
        for (size_t i = 1; i < N; ++i) {  // 0-based i=1 == 1-based poll index 2
            if (restC[i] < *t_last) {
                ++b_nonvacuous;
                if (!(restV[i] & 0x4000)) b_ok = false;
            }
        }
    } else {
        b_ok = false;
    }

    bool c_ok = false;
    if (struct_ok) {
        long first_zero = -1;
        for (size_t i = 1; i < N; ++i) if (!(restV[i] & 0x4000)) { first_zero = (long)i; break; }
        bool ended_by_zero = (N < 8192) && first_zero >= 0;
        bool tail_all_zero = true;
        if (ended_by_zero)
            for (size_t i = (size_t)first_zero; i < total_rest; ++i)
                if (restV[i] & 0x4000) { tail_all_zero = false; break; }
        c_ok = ended_by_zero && tail_all_zero;
    }

    checks.note("P8: N(poll)=" + num((long long)N) + " total_rest=" + num((long long)total_rest) +
                " t_last=" + (t_last ? num((long long)*t_last) : std::string("(none)")) +
                " b_nonvacuous=" + num((long long)b_nonvacuous));

    bool ok = cap.loaded && idle_ok && struct_ok && blit_ran && b_ok && c_ok;
    checks.expect(ok, std::string("P8 [") + kClaims +
        "]: the 4 idle reads show bit14=0; every poll read i>=2 with T_w(i) < T_last shows bit14=1; the poll "
        "ends on a 0 before the 8192-read cap, and every read after that (including the 8 extra) is also 0");

    Capture cap_g1 = (mv == "G1") ? run_capture(dir + "/prog_p8_g1.bin", run_budget) : cap;
    checks.expect(cap_g1.loaded && cap_g1.done.has_value(), "P8-G1: the program finished");

    auto r8 = stored_at(cap.log, kR, 1);
    bool g2_ok = cap.loaded && r8.size() == 1;
    if (g2_ok) { uint16_t v = r8[0]; g2_ok = (v & 0x0200) && (v & 0x0040) && !(v & 0x0400) && ((v & 0x01FF & ~0x0040) == 0); }
    checks.expect(g2_ok, "P8-G2: the stored DMACONR readback shows DMAEN and BLTEN set, BLTPRI clear, and no "
                         "other bit among 8-0");

    checks.expect(blit_ran, "P8-G3: the blit ran -- T_last is defined (at least 2 writes in the Q range)");

    Capture cap_gp = (mv == "GBLTPRI") ? run_capture(dir + "/prog_p8_gbltpri.bin", run_budget) : cap;
    bool gp_ok;
    {
        auto g_all2 = writes_seq(cap_gp.log, kLog, kLog + 0x4200);
        std::vector<uint64_t> qc2;
        for (const auto& a : cap_gp.log)
            if (a.is_write && a.word_addr >= word_addr(kQ) && a.word_addr < word_addr(kQ + 0x2000))
                qc2.push_back(a.cycle);
        std::optional<uint64_t> tl2;
        if (!qc2.empty()) tl2 = *std::max_element(qc2.begin(), qc2.end());
        size_t cnt = 0;
        if (tl2.has_value() && g_all2.size() > 4) {
            std::vector<uint64_t> rc2;
            for (size_t i = 4; i < g_all2.size(); ++i) rc2.push_back(g_all2[i].cycle);
            for (size_t i = 1; i + 8 < rc2.size(); ++i) if (rc2[i] < *tl2) ++cnt;
        }
        gp_ok = cnt >= 2;
    }
    checks.expect(gp_ok, "P8-G4: at least 2 poll reads with i >= 2 have T_w(i) < T_last, so (b) is not vacuous");
}

}  // namespace p8

// ===========================================================================
// P9: after a blit, BZERO (bit 13) reads 1 exactly when the blit's logic
// output was zero throughout (SPECIFICATION.md P9).
// ===========================================================================

namespace p9 {

const char kClaims[] =
    "p.278 5-appA-p278.c14, A-dmacon.b13; p.278 5-appA-p278.c10, 5-appA-p278.c8; p.289 5-appB-p289.c4";

bool writes_all(const Capture& cap, uint32_t addr, size_t count, uint16_t data) {
    auto w = writes_seq(cap.log, addr, addr + uint32_t(count) * 2);
    if (w.size() != count) return false;
    for (auto& x : w) if (x.data != data) return false;
    return true;
}

bool reads_only(const Capture& cap, uint32_t addr, size_t count) {
    uint32_t wlo = word_addr(addr), whi = word_addr(addr) + uint32_t(count);
    size_t reads = 0;
    for (const auto& a : cap.log) {
        if (a.word_addr >= wlo && a.word_addr < whi) {
            if (a.is_write) return false;
            ++reads;
        }
    }
    return reads == count;
}

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    std::string bin = dir + "/prog_p9.bin";
    if (mv == "C1") bin = dir + "/prog_p9_c1.bin";
    Capture cap = run_capture(bin, run_budget);
    auto z = stored_at(cap.log, kR, 5);

    bool have_z = cap.loaded && z.size() == 5;

    const int expect_main[5] = {1, 0, 1, 0, 1};
    const int expect_c1[5]   = {0, 1, 1, 1, 0};
    const int* expect = (mv == "C1") ? expect_c1 : expect_main;

    bool z_ok = have_z;
    if (z_ok) for (int k = 0; k < 5; ++k) {
        bool bit13 = (z[k] & 0x2000) != 0;
        if (bit13 != (expect[k] != 0)) z_ok = false;
    }
    if (have_z) {
        std::string zs = "P9: Z1.." "Z5 = ";
        for (int k = 0; k < 5; ++k) zs += hex16(z[k]) + " ";
        checks.note(zs);
    }
    checks.expect(z_ok, std::string("P9 [") + kClaims +
        "]: bit 13 of each Z_k matches the proposition's table (blits 1-3 show the flag describes the "
        "latest blit; blits 4-5 grade 'even if D is not written')");

    // Data-content preconditions (main list only -- addresses are fixed
    // regardless of NMH_MUTATE, since only BLTCON0/source assignments swap
    // under C1, not which physical block each blit numeral touches).
    bool wr_ok = writes_all(cap, kQ, 16, 0x0000) && writes_all(cap, kQ + 0x100, 16, 0xFFFF) &&
                 writes_all(cap, kQ + 0x200, 16, 0x0000);
    checks.expect(wr_ok, "P9-G(writes): blits 1 and 3 made 16 writes of $0000 in their ranges; blit 2 made "
                         "16 writes of $FFFF");

    bool rd_ok = reads_only(cap, kF, 16) && reads_only(cap, kZb, 16);
    checks.expect(rd_ok, "P9-G(reads): blits 4 and 5 made 16 reads in F and Zb respectively, and no write there");

    Capture cap_g1 = (mv == "G1") ? run_capture(dir + "/prog_p9_g1.bin", run_budget) : cap;
    checks.expect(cap_g1.loaded && cap_g1.done.has_value(), "P9-G1: the program finished");

    bool g2_ok = z.size() >= 1; // DMAEN/BLTEN are implied by every blit having run at all
    checks.expect(g2_ok, "P9-G2: G2 for $8240 (DMAEN, BLTEN) -- every blit ran, which needs both set");

    Capture cap_gnw = (mv == "NOWAIT") ? run_capture(dir + "/prog_p9_gnowait.bin", run_budget) : cap;
    // "After completion": Z_1's store must follow the last port access of
    // blit 1 (its last write in the Q range).
    bool after_ok = false;
    {
        auto z1w = writes_seq(cap_gnw.log, kR, kR + 2);
        auto q1 = writes_seq(cap_gnw.log, kQ, kQ + 32);
        if (!z1w.empty() && !q1.empty()) {
            uint64_t z1_cycle = z1w.front().cycle;
            uint64_t last_q1 = q1.back().cycle;
            for (auto& w : q1) last_q1 = std::max(last_q1, w.cycle);
            after_ok = z1_cycle > last_q1;
        }
    }
    checks.expect(after_ok, "P9-G(after completion): Z_1 was stored after the last port access of blit 1");
}

}  // namespace p9

// ===========================================================================
// P10: the Copper writes AUDxLCH and AUDxLCL whatever the danger bit,
// under OCS and ECS (SPECIFICATION.md P10).
// ===========================================================================

namespace p10 {

constexpr uint16_t kO[4] = {0x0010, 0x0012, 0x0014, 0x0016};
constexpr uint16_t kC[4] = {0x0011, 0x0013, 0x0015, 0x0017};
constexpr uint16_t kKO = 0x0070, kKC = 0x0071;
const char kClaims[] = "p.310 C-interp.copcon, C-interp.copcon.ecs";

std::vector<uint16_t> list_audio() {
    std::vector<uint16_t> v;
    for (int x = 0; x < 4; ++x) {
        uint16_t hi_off = uint16_t(0x00A0 + 0x10 * x), lo_off = uint16_t(0x00A2 + 0x10 * x);
        uint32_t b = kB[x];
        v.push_back(hi_off); v.push_back(uint16_t(b >> 16));
        v.push_back(lo_off); v.push_back(uint16_t(b & 0xFFFF));
    }
    v.push_back(0xFFFF); v.push_back(0xFFFE);
    return v;
}

std::vector<uint16_t> list_color() {
    std::vector<uint16_t> v;
    for (int x = 0; x < 4; ++x) {
        uint32_t b = kB[x];
        v.push_back(0x0180); v.push_back(uint16_t(b >> 16));
        v.push_back(0x0180); v.push_back(uint16_t(b & 0xFFFF));
    }
    v.push_back(0xFFFF); v.push_back(0xFFFE);
    return v;
}

struct W4 { bool ok = false; uint64_t o[4] = {}, c[4] = {}; };

W4 find4(const std::vector<nmh::RamAccess>& log) {
    W4 w;
    for (int x = 0; x < 4; ++x) {
        auto o = marker(log, kO[x]); auto c = marker(log, kC[x]);
        if (!o || !c) return w;
        w.o[x] = *o; w.c[x] = *c;
    }
    w.ok = true;
    return w;
}

bool check_main(const Capture& cap, const W4& w, bool expect_audio) {
    if (!w.ok) return false;
    for (int x = 0; x < 4; ++x) {
        if (expect_audio) {
            if (reads_at(cap.log, kB[x], w.o[x], w.c[x]) < 2) return false;
            if (reads_at(cap.log, kA[x], w.o[x], w.c[x]) > 0) return false;
        } else {
            if (reads_at(cap.log, kA[x], w.o[x], w.c[x]) < 2) return false;
            if (reads_at(cap.log, kB[x], w.o[x], w.c[x]) > 0) return false;
        }
        for (int y = 0; y < 4; ++y) if (y != x) {
            if (reads_at(cap.log, kA[y], w.o[x], w.c[x]) > 0) return false;
            if (reads_at(cap.log, kB[y], w.o[x], w.c[x]) > 0) return false;
        }
    }
    return true;
}

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    bool list_is_color = (mv == "C1");
    std::vector<ExtraPreload> extra = {{kList, list_is_color ? list_color() : list_audio()}};

    std::string bin_main  = dir + "/prog_p10.bin";
    std::string bin_cdang = dir + "/prog_p10_cdang.bin";

    nmh::Config cfg_ocs; cfg_ocs.chipset_config = 0x00;
    nmh::Config cfg_ecs; cfg_ecs.chipset_config = 0x08;

    struct Combo { const char* name; std::string bin; nmh::Config cfg; };
    Combo combos[4] = {
        {"OCS/CDANG0", bin_main,  cfg_ocs},
        {"OCS/CDANG1", bin_cdang, cfg_ocs},
        {"ECS/CDANG0", bin_main,  cfg_ecs},
        {"ECS/CDANG1", bin_cdang, cfg_ecs},
    };
    bool ok = true;
    for (auto& combo : combos) {
        Capture cap = run_capture(combo.bin, run_budget, combo.cfg, extra);
        auto w = find4(cap.log);
        bool this_ok = cap.loaded && cap.done.has_value() && check_main(cap, w, !list_is_color);
        checks.note(std::string("P10 ") + combo.name + ": " + (this_ok ? "ok" : "FAILED"));
        if (!this_ok) ok = false;
    }
    checks.expect(ok, std::string("P10 [") + kClaims +
        "]: under OCS and ECS, with CDANG clear or set, in W_x: at least 2 reads at B_x and none at A_x, and "
        "none at A_y or B_y for y != x");

    // Preconditions, checked once (ECS, CDANG clear): the common ones are
    // graded and mutated by P1 (SPECIFICATION.md's own cross-reference).
    std::vector<ExtraPreload> extra_audio = {{kList, list_audio()}};
    Capture cap = run_capture(bin_main, run_budget, cfg_ecs, extra_audio);
    auto w = find4(cap.log);
    auto ko = marker(cap.log, kKO), kc = marker(cap.log, kKC);
    checks.expect(cap.loaded && cap.done.has_value() && w.ok && ko.has_value() && kc.has_value(),
        "P10-G1: the program finished");

    auto r10 = stored_at(cap.log, kR, 1);
    bool g2_ok = r10.size() == 1;
    if (g2_ok) { uint16_t v = r10[0]; g2_ok = (v & 0x0200) && (v & 0x0080); }
    checks.expect(g2_ok, "P10-G2: the stored DMACONR readback (Copper enable) shows DMAEN and COPEN set");

    bool g4_ok = ko.has_value() && kc.has_value() && w.ok && count_window(vsync_starts(cap.video), *ko, *kc) >= 2;
    if (g4_ok) for (int x = 0; x < 4; ++x)
        if (count_window(vsync_starts(cap.video), w.o[x], w.c[x]) < 2) g4_ok = false;
    checks.expect(g4_ok, "P10-G4: V >= 2 in [K_O,K_C] and in each W_x");

    // G3: no CPU write falls into a DMA block during any window (A_x/B_x
    // in each W_x, L in [K_O,K_C]). Live invariant only -- the mutation is
    // P1's (SPECIFICATION.md's own cross-reference).
    bool g3_ok = w.ok && ko.has_value() && kc.has_value();
    if (g3_ok) for (int x = 0; x < 4; ++x) {
        if (writes_in_range(cap.log, kA[x], kA[x] + 16, w.o[x], w.c[x]) != 0) g3_ok = false;
        if (writes_in_range(cap.log, kB[x], kB[x] + 16, w.o[x], w.c[x]) != 0) g3_ok = false;
    }
    if (g3_ok && writes_in_range(cap.log, kList, kList + 0x24, *ko, *kc) != 0) g3_ok = false;
    checks.expect(g3_ok, "P10-G3: zero writes observed in any A_x/B_x range across all four windows, and in "
                         "[L,L+$24) in [K_O,K_C]");

    Capture cap_g_nolist = (mv == "G_NOLIST") ? run_capture(dir + "/prog_p10_g_nolist.bin", run_budget, cfg_ecs, extra_audio) : cap;
    auto ko3 = marker(cap_g_nolist.log, kKO), kc3 = marker(cap_g_nolist.log, kKC);
    bool ranlist_ok = ko3.has_value() && kc3.has_value();
    if (ranlist_ok) for (uint32_t off = 0; off < 0x24; off += 2)
        if (reads_at(cap_g_nolist.log, kList + off, *ko3, *kc3) < 1) { ranlist_ok = false; break; }
    checks.expect(ranlist_ok, "P10-G(list ran): every word of [L,L+$24) was read at least once inside [K_O,K_C]");
}

}  // namespace p10

// ===========================================================================
// P11: with the danger bit set, the Copper writes the blitter registers at
// $054, $056 and $058 (SPECIFICATION.md P11).
// ===========================================================================

namespace p11 {

constexpr uint16_t kO = 0x0080, kC = 0x0081;
const char kClaims[] = "p.310 C-interp.copcon, C-interp.copcon.ecs";

std::vector<uint16_t> list_main() {
    return {0x0054, uint16_t(kQ2 >> 16), 0x0056, uint16_t(kQ2 & 0xFFFF), 0x0058, 0x0104, 0xFFFF, 0xFFFE};
}
std::vector<uint16_t> list_no_bltsize() {
    return {0x0054, uint16_t(kQ2 >> 16), 0x0056, uint16_t(kQ2 & 0xFFFF), 0x0180, 0x0000, 0xFFFF, 0xFFFE};
}
std::vector<uint16_t> list_no_pointer() {
    return {0x0180, uint16_t(kQ2 >> 16), 0x0180, uint16_t(kQ2 & 0xFFFF), 0x0058, 0x0104, 0xFFFF, 0xFFFE};
}

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    std::vector<uint16_t> list;
    if (mv == "NOBLTSIZE") list = list_no_bltsize();
    else if (mv == "NOPTR") list = list_no_pointer();
    else list = list_main();
    std::vector<ExtraPreload> extra = {{kList, list}};

    nmh::Config cfg_ocs; cfg_ocs.chipset_config = 0x00;
    nmh::Config cfg_ecs; cfg_ecs.chipset_config = 0x08;

    bool ok = true;
    for (auto cfg : {cfg_ocs, cfg_ecs}) {
        Capture cap = run_capture(dir + "/prog_p11.bin", run_budget, cfg, extra);
        auto w = find_window(cap.log, kO, kC);
        bool this_ok = cap.loaded && cap.done.has_value() && w.has_value();
        if (this_ok) {
            size_t wq2 = writes_in_range(cap.log, kQ2, kQ2 + 0x20, w->lo, w->hi);
            size_t wq  = writes_in_range(cap.log, kQ,  kQ  + 0x20, w->lo, w->hi);
            if (mv == "NOBLTSIZE") this_ok = (wq2 == 0 && wq == 0);
            else if (mv == "NOPTR") this_ok = (wq >= 1 && wq2 == 0);
            else this_ok = (wq2 >= 1 && wq == 0);
        }
        checks.note(std::string("P11 chipset_config=") + hex16(cfg.chipset_config) + ": " + (this_ok ? "ok" : "FAILED"));
        if (!this_ok) ok = false;
    }
    checks.expect(ok, std::string("P11 [") + kClaims +
        "]: under OCS and ECS, at least one write in [Q2,Q2+$20) and none in [Q,Q+$20) in the window "
        "(NOBLTSIZE: neither; NOPTR: the CPU's own Q destination instead, none at Q2)");

    // Preconditions, checked once under ECS with the real list. The common
    // ones are graded and mutated by P1 (SPECIFICATION.md's own cross-reference).
    Capture cap = run_capture(dir + "/prog_p11.bin", run_budget, cfg_ecs, {{kList, list_main()}});
    auto w = find_window(cap.log, kO, kC);
    auto r11 = stored_at(cap.log, kR, 1);
    bool g2_ok = r11.size() == 1;
    if (g2_ok) { uint16_t v = r11[0]; g2_ok = (v & 0x0200) && (v & 0x0080) && (v & 0x0040); }
    checks.expect(cap.loaded && cap.done.has_value() && w.has_value() && g2_ok,
        "P11-G1/G2: the program finished, and the stored DMACONR readback shows DMAEN, COPEN and BLTEN set");

    bool g4_ok = w.has_value() && count_window(vsync_starts(cap.video), w->lo, w->hi) >= 3;
    checks.expect(g4_ok, "P11-G4: V(W) >= 3");

    // G3: no CPU write falls into L during the window (Q2 is filled
    // exactly to its own $20-byte reservation by the blit itself, so it
    // has no headroom to check separately). Live invariant only -- the
    // mutation is P1's.
    bool g3_ok = w.has_value() && writes_in_range(cap.log, kList, kList + 64, w->lo, w->hi) == 0;
    checks.expect(g3_ok, "P11-G3: zero writes observed in [L,L+64) in the window");

    Capture cap_g1 = (mv == "G1") ? run_capture(dir + "/prog_p11_g1.bin", run_budget, cfg_ecs, {{kList, list_main()}}) : cap;
    checks.expect(cap_g1.loaded && cap_g1.done.has_value(), "P11-G(finished): the program finished");

    Capture cap_g_nolist = (mv == "G_NOLIST") ? run_capture(dir + "/prog_p11_g_nolist.bin", run_budget, cfg_ecs, {{kList, list_main()}}) : cap;
    auto w3 = find_window(cap_g_nolist.log, kO, kC);
    bool listran_ok = w3.has_value();
    if (listran_ok) for (uint32_t off = 0; off < 12; off += 2)
        if (reads_at(cap_g_nolist.log, kList + off, w3->lo, w3->hi) < 1) { listran_ok = false; break; }
    checks.expect(listran_ok, "P11-G(list ran): the list's first 12 bytes were read at least once inside the window");
}

}  // namespace p11

// Claims of SPECIFICATION.md section 1, which no proposition grades, each
// with its reason. None of these ids sits on a graded line.
static void record_declined(Checks& checks) {
    checks.record(
        "5-appA-p264.c2: not graded. p.264, 'If a register is marked as a write-only register, do not attempt to "
        "read from it, as this may trash the register and crash the system', names only a possible consequence "
        "('may'), so there is no expected value; no program reads a write-only register");
    checks.record(
        "5-appA-p263.c7 5-appA-p263.c8 A-pre.c8: not graded. p.263, 'using a non-Chip address will fail', names "
        "no observable outcome, and the system under test has no memory besides Chip memory");
    checks.record(
        "5-appA-p263.c9 A-pre.c9: not graded. p.263 instructs the programmer where DMA data must be located and "
        "does not say what happens when the instruction is broken; every stimulus obeys it");
    checks.record(
        "5-appB-p287.c5: not graded. p.287, 'Must be an even address pointing to chip memory', is the same kind of "
        "instruction; every location and pointer written is even and lies in Chip memory");
    checks.record(
        "5-appB-p290.c3: not graded. The chip column says which chip decodes a register and no output of the "
        "system shows that; the printed p.290 row reads 'DMACON 096 W A D P', which agrees with p.278, and the "
        "claim rests on a text-layer error");
}

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    // NMH_ONLY=P1,P7 runs only the named propositions (and the fixture); it
    // exists to replay one mutation without the rest of the suite.
    const char* only_env = std::getenv("NMH_ONLY");
    const std::string only = only_env ? std::string(",") + only_env + "," : "";
    auto want = [&](const char* p) { return only.empty() || only.find(std::string(",") + p + ",") != std::string::npos; };

    run_fixture(checks, dir, mv);
    if (want("P1"))  p1::run(checks, dir, mv, 30'000'000);
    if (want("P2"))  p2::run(checks, dir, mv, 15'000'000);
    if (want("P3"))  p3::run(checks, dir, mv, 12'000'000);
    if (want("P4"))  p4::run(checks, dir, mv, 12'000'000);
    if (want("P5"))  p5::run(checks, dir, mv, 12'000'000);
    if (want("P6"))  p6::run(checks, dir, mv, 12'000'000);
    if (want("P7"))  p7::run(checks, dir, mv, 15'000'000);
    if (want("P8"))  p8::run(checks, dir, mv, 15'000'000);
    if (want("P9"))  p9::run(checks, dir, mv, 15'000'000);
    if (want("P10")) p10::run(checks, dir, mv, 40'000'000);
    if (want("P11")) p11::run(checks, dir, mv, 15'000'000);
    record_declined(checks);

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
