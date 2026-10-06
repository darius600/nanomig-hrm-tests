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
// MUTATION FIXTURE stimulus red stored constant at RESULTS changed to DEAD instead of BEEF, via NMH_MUTATE=FIXTURE
// MUTATION FIXTURE-R stimulus red stored constant at R changed to DEAD instead of BEEF, via NMH_MUTATE=FIXTURE-R
// MUTATION P1-G1 stimulus red branch-to-self inserted before END, via NMH_MUTATE=G1 -- P1a, P1b and P1 are NOT GRADED
// MUTATION P1-G2 stimulus red channel 0's enabling write leaves DMAEN out, via NMH_MUTATE=G2 -- P1a, P1b and P1 are NOT GRADED
// MUTATION P1-G3 stimulus red one extra CPU write into A_0, through an absolute address, inside W_0, via NMH_MUTATE=G3 -- P1a, P1b and P1 are NOT GRADED
// MUTATION P1-G4 stimulus red the per-channel delay collapses to nothing, via NMH_MUTATE=G4 -- P1a, P1b and P1 are NOT GRADED
// MUTATION P1-G5 stimulus red SPREN also set before the windows, via NMH_MUTATE=G5 -- P1a, P1b and P1 are NOT GRADED
// MUTATION P1a stimulus red channels 0-3's locations changed to A_x^H (the high-word contrast), via NMH_MUTATE=C1
// MUTATION P1a stimulus red channels 0-3's locations changed to A_x^L (the low-word contrast), via NMH_MUTATE=C2
// MUTATION P1a stimulus red channel 0 and channel 1's locations swapped (the register-to-channel contrast), via NMH_MUTATE=C3
// MUTATION P1b stimulus red channel 0 and channel 1's locations swapped (the register-to-channel contrast), via NMH_MUTATE=C3
// MUTATION P1 stimulus red channels 0-3's locations changed to A_x^H through the unchanged comparators of the aggregate line, via NMH_MUTATE=C1
// MUTATION P1 stimulus red channels 0-3's locations changed to A_x^L through the unchanged comparators of the aggregate line, via NMH_MUTATE=C2
// MUTATION P1 stimulus red channel 0 and channel 1's locations swapped, through the unchanged comparators of the aggregate line, via NMH_MUTATE=C3
// MUTATION P1-C1 stimulus red the main program (locations A_x) through the unchanged high-word comparator, via NMH_MUTATE=INVC1
// MUTATION P1-C2 stimulus red the main program (locations A_x) through the unchanged low-word comparator, via NMH_MUTATE=INVC2
// MUTATION P1-C3 stimulus red the main program (locations A_x) through the unchanged register-to-channel comparator, via NMH_MUTATE=INVC3
// MUTATION P2-G1 stimulus red branch-to-self inserted before DONE, via NMH_MUTATE=G1 -- P2 is NOT GRADED
// MUTATION P2 stimulus red the SET write for DMACON bit 0 in the sweep is left out, via NMH_MUTATE=OMIT
// MUTATION P2-OMIT stimulus red the main program (the SET write for DMACON bit 0 present) through the unchanged comparator of the omit line, via NMH_MUTATE=INVOMIT
// MUTATION P3-G1 stimulus red branch-to-self inserted before DONE, via NMH_MUTATE=G1 -- P3-ON and P3 are NOT GRADED
// MUTATION P3-G2 stimulus red DMAEN (bit 9) left out of the enabling write, BPLEN still set, via NMH_MUTATE=G2 -- P3-ON and P3 are NOT GRADED
// MUTATION P3-G3 stimulus red one extra CPU write into P (the block under test), via NMH_MUTATE=G3 -- P3-ON and P3 are NOT GRADED
// MUTATION P3-G4 stimulus red the delay collapses to nothing, via NMH_MUTATE=G4 -- P3-ON and P3 are NOT GRADED
// MUTATION P3-G5 stimulus red SPREN also set in the enabling write, via NMH_MUTATE=G5 -- P3-ON and P3 are NOT GRADED
// MUTATION P3-ON stimulus red BPLEN left clear: the contrast program through the unchanged on-comparator, via NMH_MUTATE=C1
// MUTATION P3-OFF stimulus red BPLEN set: the main program through the unchanged off-comparator, via NMH_MUTATE=INV
// MUTATION P3 stimulus red BPLEN left clear: the contrast program through the unchanged on-comparator of the aggregate line, via NMH_MUTATE=C1
// MUTATION P3 stimulus red BPLEN set: the main program through the unchanged off-comparator of the aggregate line, via NMH_MUTATE=INV
// MUTATION P4-G1 stimulus red branch-to-self inserted before DONE, via NMH_MUTATE=G1 -- P4-ON and P4 are NOT GRADED
// MUTATION P4-G2 stimulus red DMAEN (bit 9) left out of the enabling write, COPEN still set, via NMH_MUTATE=G2 -- P4-ON and P4 are NOT GRADED
// MUTATION P4-G3 stimulus red one extra CPU write into L (the block under test), past its own content, via NMH_MUTATE=G3 -- P4-ON and P4 are NOT GRADED
// MUTATION P4-G4 stimulus red the delay collapses to nothing, via NMH_MUTATE=G4 -- P4-ON and P4 are NOT GRADED
// MUTATION P4-G5 stimulus red SPREN also set in the enabling write, via NMH_MUTATE=G5 -- P4-ON and P4 are NOT GRADED
// MUTATION P4-ON stimulus red COPEN left clear: the contrast program through the unchanged on-comparator, via NMH_MUTATE=C1
// MUTATION P4-OFF stimulus red COPEN set: the main program through the unchanged off-comparator, via NMH_MUTATE=INV
// MUTATION P4 stimulus red COPEN left clear: the contrast program through the unchanged on-comparator of the aggregate line, via NMH_MUTATE=C1
// MUTATION P4 stimulus red COPEN set: the main program through the unchanged off-comparator of the aggregate line, via NMH_MUTATE=INV
// MUTATION P5-G1 stimulus red branch-to-self inserted before DONE, via NMH_MUTATE=G1 -- P5-ON and P5 are NOT GRADED
// MUTATION P5-G2 stimulus red DMAEN (bit 9) left out of the enabling write, BLTEN still set, via NMH_MUTATE=G2 -- P5-G3 moves with it (no footprint word is written once in the window), P5-ON and P5 are NOT GRADED
// MUTATION P5-G3 stimulus red one extra CPU write into Q (the block under test), outside the blit's destination footprint, via NMH_MUTATE=G3 -- P5-ON and P5 are NOT GRADED
// MUTATION P5-G3 stimulus red one extra CPU write onto the first word of the blit's own destination footprint, via NMH_MUTATE=G3IN -- P5-ON and P5 are NOT GRADED
// MUTATION P5-G4 stimulus red the delay collapses to nothing, via NMH_MUTATE=G4 -- P5-G3 moves with it (no footprint word is written once in the window), P5-ON and P5 are NOT GRADED
// MUTATION P5-G5 stimulus red SPREN also set in the enabling write, via NMH_MUTATE=G5 -- P5-ON and P5 are NOT GRADED
// MUTATION P5-ON stimulus red BLTEN left clear: the contrast program through the unchanged on-comparator, via NMH_MUTATE=C1
// MUTATION P5-OFF stimulus red BLTEN set: the main program through the unchanged off-comparator, via NMH_MUTATE=INV
// MUTATION P5 stimulus red BLTEN left clear: the contrast program through the unchanged on-comparator of the aggregate line, via NMH_MUTATE=C1
// MUTATION P5 stimulus red BLTEN set: the main program through the unchanged off-comparator of the aggregate line, via NMH_MUTATE=INV
// MUTATION P6-G1 stimulus red branch-to-self inserted before DONE, via NMH_MUTATE=G1 -- P6-ON and P6 are NOT GRADED
// MUTATION P6-G2 stimulus red DMAEN (bit 9) left out of the enabling write, SPREN still set, via NMH_MUTATE=G2 -- P6-ON and P6 are NOT GRADED
// MUTATION P6-G3 stimulus red one extra CPU write into S0 (the block under test), past its own content, via NMH_MUTATE=G3 -- P6-ON and P6 are NOT GRADED
// MUTATION P6-G4 stimulus red the delay collapses to nothing, via NMH_MUTATE=G4 -- P6-ON and P6 are NOT GRADED
// MUTATION P6-G5 stimulus red BPLEN also set before the window, via NMH_MUTATE=G5 -- P6-ON and P6 are NOT GRADED
// MUTATION P6-ON stimulus red SPREN left clear: the contrast program through the unchanged on-comparator, via NMH_MUTATE=C1
// MUTATION P6-OFF stimulus red SPREN set: the main program through the unchanged off-comparator, via NMH_MUTATE=INV
// MUTATION P6 stimulus red SPREN left clear: the contrast program through the unchanged on-comparator of the aggregate line, via NMH_MUTATE=C1
// MUTATION P6 stimulus red SPREN set: the main program through the unchanged off-comparator of the aggregate line, via NMH_MUTATE=INV
// MUTATION P7-G1 stimulus red branch-to-self inserted before END, via NMH_MUTATE=G1 -- P7-ON and P7 are NOT GRADED
// MUTATION P7-G2 stimulus red DMAEN (bit 9) left out of the enabling write, BLTPRI and BLTEN still set, via NMH_MUTATE=G2 -- P7-G3 and P7-G(CPU-still-wanted-the-bus) move with it (no blit runs without DMAEN); P7-ON and P7 are NOT GRADED
// MUTATION P7-G3 stimulus red the BLTSIZE write replaced by a COLOR00 write, so no blit starts, via NMH_MUTATE=G3 -- P7-G(CPU-still-wanted-the-bus) moves with it (no blit runs, so there is no T_last); P7-ON and P7 are NOT GRADED
// MUTATION P7-G(CPU-still-wanted-the-bus) stimulus red the CPU's logging loop runs before the BLTSIZE write, so no CPU write into G follows T_last, via NMH_MUTATE=GEARLY -- P7-ON and P7 are NOT GRADED
// MUTATION P7-ON stimulus red BLTPRI cleared: the share-mode program through the unchanged zero-access comparator, via NMH_MUTATE=C1
// MUTATION P7-OFF stimulus red BLTPRI set: the main program through the unchanged at-least-one comparator, via NMH_MUTATE=INV
// MUTATION P7 stimulus red BLTPRI cleared: the share-mode program through the unchanged zero-access comparator of the aggregate line, via NMH_MUTATE=C1
// MUTATION P7 stimulus red BLTPRI set: the main program through the unchanged at-least-one comparator of the aggregate line, via NMH_MUTATE=INV
// MUTATION P8-G1 stimulus red branch-to-self inserted before DONE, via NMH_MUTATE=G1 -- P8 is NOT GRADED
// MUTATION P8-G2 stimulus red BLTPRI set in step 2, via NMH_MUTATE=GBLTPRI -- P8 is NOT GRADED
// MUTATION P8-G3 stimulus red the BLTSIZE write replaced by a COLOR00 write, so no blit starts, via NMH_MUTATE=G3 -- P8-G4 moves with it (no blit runs, so no read precedes T_last); P8 is NOT GRADED
// MUTATION P8-G4 stimulus red a delay between the BLTSIZE write and the first poll read, so the blit has ended before any read, via NMH_MUTATE=G4 -- P8 is NOT GRADED
// MUTATION P8 stimulus red the blit runs and the fourth poll read comes from a zero word (Zb) instead of DMACONR, so a graded read made while the blit is in progress shows bit 14 = 0, via NMH_MUTATE=ZPOLL
// MUTATION P8-NOBLIT stimulus red the main program (a blit runs) through the unchanged no-blit comparator, via NMH_MUTATE=INVNOBLIT
// MUTATION P9-G(writes) stimulus red blit 1 has BLTSIZE $0103 (12 words) instead of $0104, its logic output still zero, via NMH_MUTATE=GWRITES -- P9-MAIN and P9 are NOT GRADED
// MUTATION P9-G(reads) stimulus red blit 4 also uses D, so it writes outside the CPU areas, via NMH_MUTATE=GDWRITE -- P9-MAIN and P9 are NOT GRADED
// MUTATION P9-G1 stimulus red branch-to-self inserted before END, via NMH_MUTATE=G1 -- P9-MAIN and P9 are NOT GRADED
// MUTATION P9-G2 stimulus red SPREN also set in the enabling write, via NMH_MUTATE=G2 -- P9-MAIN and P9 are NOT GRADED
// MUTATION P9-G(after-completion) stimulus red the wait before Z_1 is dropped, via NMH_MUTATE=NOWAIT -- P9-G(writes) moves with it (the next blit's registers are written while the first blit runs); P9-MAIN and P9 are NOT GRADED
// MUTATION P9-MAIN stimulus red BLTCON0 exchanged between blits 1 and 2 and the source exchanged between blits 4 and 5: the exchanged program through the unchanged main table, via NMH_MUTATE=C1
// MUTATION P9-SWAP stimulus red the main program through the unchanged exchanged table, via NMH_MUTATE=INV
// MUTATION P9 stimulus red the exchanged program through the unchanged main table of the aggregate line, via NMH_MUTATE=C1
// MUTATION P9 stimulus red the main program through the unchanged exchanged table of the aggregate line, via NMH_MUTATE=INV
// MUTATION P10 stimulus red the list that writes COLOR00 instead of the audio registers, through the unchanged audio comparator, via NMH_MUTATE=C1
// MUTATION P10-COLOR stimulus red the audio list through the unchanged COLOR00 comparator, via NMH_MUTATE=INV
// MUTATION P10-G1 stimulus red branch-to-self inserted before END, via NMH_MUTATE=G1
// MUTATION P10-G2 stimulus red DMAEN (bit 9) left out of the Copper enable, via NMH_MUTATE=G2K
// MUTATION P10-G2 stimulus red DMAEN cleared after the Copper window and left out of channel 0's enable, via NMH_MUTATE=G2W
// MUTATION P10-G3 stimulus red one extra CPU write into B_0 inside W_0, via NMH_MUTATE=G3
// MUTATION P10-G3 stimulus red one extra CPU write into the last word of L inside [K_O,K_C], via NMH_MUTATE=G3L
// MUTATION P10-G4 stimulus red the per-channel delay collapses to nothing, via NMH_MUTATE=G4
// MUTATION P10-G5 stimulus red SPREN also set before the windows, via NMH_MUTATE=G5
// MUTATION P10-G(list-ran) stimulus red COPEN omitted from the enabling write, via NMH_MUTATE=G_NOLIST
// MUTATION P11 stimulus red the list without the BLTSIZE MOVE, through the unchanged main comparator, via NMH_MUTATE=NOBLTSIZE
// MUTATION P11 stimulus red the list without the pointer MOVEs, through the unchanged main comparator, via NMH_MUTATE=NOPTR
// MUTATION P11-NOBLTSIZE stimulus red the real list through the unchanged no-BLTSIZE comparator, via NMH_MUTATE=INVNOBLTSIZE
// MUTATION P11-NOPTR stimulus red the real list through the unchanged no-pointer comparator, via NMH_MUTATE=INVNOPTR
// MUTATION P11-G1 stimulus red branch-to-self inserted before END, via NMH_MUTATE=G1
// MUTATION P11-G2 stimulus red DMAEN (bit 9) left out of the enabling write, via NMH_MUTATE=G2
// MUTATION P11-G3 stimulus red one extra CPU write into L, past its own content, inside the window, via NMH_MUTATE=G3
// MUTATION P11-G4 stimulus red the delay collapses to nothing, via NMH_MUTATE=G4
// MUTATION P11-G5 stimulus red SPREN also set in the enabling write, via NMH_MUTATE=G5
// MUTATION P11-G(list-ran) stimulus red COPEN omitted from the enabling write, via NMH_MUTATE=G_NOLIST



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
constexpr uint32_t kList2 = 0x64100;   // LST2 of ch5.i: an end-of-list pair the "list ran" mutations point COP1LC at

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

// The five preconditions most propositions share (SPECIFICATION.md 2.6),
// each evaluated on one run by itself: G1 the program finished, G2 the
// stored DMACONR readback shows the bits the run intends, G3 no CPU write
// into a DMA block inside a window, G4 the frames inside the window, G5 no
// other DMA bit among 8-0.
struct Pre {
    bool g1 = false, g2 = false, g3 = false, g4 = false, g5 = false;
};

// The preconditions a graded line depends on, collected over the inputs the
// line reads. The precondition lines print first; a line with an unmet
// precondition prints NOT GRADED and names it, never PASS or FAIL.
struct Deps {
    std::vector<std::string> unmet;
    void need(bool ok, const std::string& name) {
        if (!ok && std::find(unmet.begin(), unmet.end(), name) == unmet.end()) unmet.push_back(name);
    }
    void need_pre(const Pre& p, const std::string& prefix) {
        need(p.g1, prefix + "-G1");
        need(p.g2, prefix + "-G2");
        need(p.g3, prefix + "-G3");
        need(p.g4, prefix + "-G4");
        need(p.g5, prefix + "-G5");
    }
    bool met() const { return unmet.empty(); }
};

void not_graded(Checks& checks, const std::string& id, const Deps& d) {
    std::string s;
    for (const auto& u : d.unmet) s += (s.empty() ? "" : ", ") + u;
    checks.record("NOT GRADED " + id + ": precondition(s) not met: " + s);
}

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
    preload(h, kList2, {0xFFFF, 0xFFFE});
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

// Every write in [lo_byte,hi_byte) inside [lo,hi] carries `data`.
bool writes_carry(const std::vector<nmh::RamAccess>& log, uint32_t lo_byte, uint32_t hi_byte,
                  uint64_t lo, uint64_t hi, uint16_t data) {
    uint32_t wlo = word_addr(lo_byte), whi = word_addr(hi_byte);
    for (const auto& a : log)
        if (a.is_write && a.word_addr >= wlo && a.word_addr < whi && a.cycle >= lo && a.cycle <= hi &&
            a.data != data)
            return false;
    return true;
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
    // FIXTURE and FIXTURE-R each have their own mutated program: $DEAD at
    // RESULTS only, respectively at R only.
    std::string bin = dir + (mv == "FIXTURE" ? "/prog_fixture_mut.bin" :
                             mv == "FIXTURE-R" ? "/prog_fixture_mutr.bin" : "/prog_fixture.bin");
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

// C1 and C2: in W_x at least 2 reads at loc[x] (A_x^H, respectively A_x^L)
// and none at A_x.
bool check_loc(const Capture& cap, const Windows& w, const uint32_t* loc) {
    if (!w.ok) return false;
    for (int x = 0; x < 4; ++x) {
        if (reads_at(cap.log, loc[x], w.o[x], w.c[x]) < 2) return false;
        if (reads_at(cap.log, kA[x], w.o[x], w.c[x]) != 0) return false;
    }
    return true;
}

// C3: in W_0 at least 2 reads at A_1 and none at A_0; in W_1 the reverse.
bool check_c3(const Capture& cap, const Windows& w) {
    if (!w.ok) return false;
    return reads_at(cap.log, kA[1], w.o[0], w.c[0]) >= 2 && reads_at(cap.log, kA[0], w.o[0], w.c[0]) == 0 &&
           reads_at(cap.log, kA[0], w.o[1], w.c[1]) >= 2 && reads_at(cap.log, kA[1], w.o[1], w.c[1]) == 0;
}

const char kClaims[] =
    "p.266 5-appA-p266.c2, 5-appA-p266.c8, A-audxlch.acc, A-audxlcl.acc; "
    "p.290 5-appB-p290.c20, 5-appB-p290.c27, 5-appB-p290.c55, 5-appB-p290.c62, "
    "5-appB-p290.c90, 5-appB-p290.c97, 5-appB-p290.c125, 5-appB-p290.c132, "
    "B-aud0lch.acc, B-aud0lcl.acc, B-aud1lch.acc, B-aud1lcl.acc, "
    "B-aud2lch.acc, B-aud2lcl.acc, B-aud3lch.acc, B-aud3lcl.acc; "
    "p.298 5-appC-p298.c4, C-map.lch.acc; p.309 5-appC-p309.c4; "
    "p.263 5-appA-p263.c1, A-pre.c1";

// What one run of one program shows: the preconditions, and what every
// comparator of the proposition says about it.
struct Eval {
    Pre pre;
    bool a = false, b = false, c1 = false, c2 = false, c3 = false;
    std::string calib;
};

Eval evaluate(const Capture& cap, const char* cfg_name) {
    Eval e;
    Windows w = find_windows(cap);
    auto vs = vsync_starts(cap.video);
    auto r = stored_at(cap.log, kR, 4);

    e.pre.g1 = cap.loaded && cap.done.has_value() && w.ok && w.end.has_value() && r.size() == 4;

    e.pre.g2 = cap.loaded && r.size() == 4;
    e.pre.g5 = e.pre.g2;
    if (e.pre.g2) for (int x = 0; x < 4; ++x) {
        uint16_t v = r[x];
        if (!(v & 0x0200) || !(v & (1 << x))) e.pre.g2 = false;     // DMAEN + channel x bit
        if ((v & 0x01FF) != (1u << x)) e.pre.g5 = false;            // only channel x's own bit among 8-0
    }

    e.pre.g4 = cap.loaded && w.ok;
    if (e.pre.g4) for (int x = 0; x < 4; ++x)
        if (count_window(vs, w.o[x], w.c[x]) < 2) e.pre.g4 = false;
    if (e.pre.g4) for (int x = 0; x < 3; ++x)
        if (count_window(vs, w.c[x], w.o[x + 1]) < 1) e.pre.g4 = false;

    // G3: no CPU write falls into any DMA block during any window.
    e.pre.g3 = cap.loaded && w.ok;
    if (e.pre.g3) for (int x = 0; x < 4 && e.pre.g3; ++x) {
        if (writes_in_range(cap.log, kA[x], kA[x] + 16, w.o[x], w.c[x]) != 0) e.pre.g3 = false;
        if (writes_in_range(cap.log, kAL[x], kAL[x] + 16, w.o[x], w.c[x]) != 0) e.pre.g3 = false;
        if (writes_in_range(cap.log, kAH[x], kAH[x] + 16, w.o[x], w.c[x]) != 0) e.pre.g3 = false;
    }

    e.a = cap.loaded && check_a(cap, w, 2);
    e.b = cap.loaded && check_b(cap, w);
    e.c1 = cap.loaded && check_loc(cap, w, kAH);
    e.c2 = cap.loaded && check_loc(cap, w, kAL);
    e.c3 = cap.loaded && check_c3(cap, w);

    e.calib = std::string("P1 calibration ") + cfg_name + ": done=" +
              (cap.done ? num((long long)*cap.done) : std::string("(timeout)"));
    if (w.ok) {
        for (int x = 0; x < 4; ++x)
            e.calib += " V(W" + num(x) + ")=" + num((long long)count_window(vs, w.o[x], w.c[x])) +
                       " o=" + num((long long)w.o[x]) + " c=" + num((long long)w.c[x]);
    } else {
        e.calib += " windows not all found";
    }
    return e;
}

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    // SPECIFICATION.md P1: "Run under OCS and ECS". Every line below is
    // graded over both runs.
    nmh::Config cfg_ecs; cfg_ecs.chipset_config = 0x08;
    nmh::Config cfg_ocs; cfg_ocs.chipset_config = 0x00;
    const struct { const char* name; nmh::Config cfg; } runs[2] = {{"ECS", cfg_ecs}, {"OCS", cfg_ocs}};

    // A precondition mutation replaces the main program everywhere it is
    // read; the contrasting programs keep their own runs.
    std::string mutant;
    if (mv == "G1" || mv == "G2" || mv == "G3" || mv == "G4" || mv == "G5")
        mutant = "/prog_p1_" + std::string(1, char(mv[0] + ('a' - 'A'))) + mv.substr(1) + ".bin";

    Eval ev_main[2], ev_mut[2], ev_c1[2], ev_c2[2], ev_c3[2];
    for (int i = 0; i < 2; ++i) {
        ev_main[i] = evaluate(run_capture(dir + "/prog_p1.bin", run_budget, runs[i].cfg), runs[i].name);
        ev_c1[i] = evaluate(run_capture(dir + "/prog_p1_c1.bin", run_budget, runs[i].cfg), runs[i].name);
        ev_c2[i] = evaluate(run_capture(dir + "/prog_p1_c2.bin", run_budget, runs[i].cfg), runs[i].name);
        ev_c3[i] = evaluate(run_capture(dir + "/prog_p1_c3.bin", run_budget, runs[i].cfg), runs[i].name);
        if (!mutant.empty()) ev_mut[i] = evaluate(run_capture(dir + mutant, run_budget, runs[i].cfg), runs[i].name);
    }

    // The input of each line. C1, C2 and C3 put a contrasting program through
    // the comparators of P1a and P1b (C3 through both); INVC1, INVC2 and
    // INVC3 put the main program through the comparator of the contrasting
    // line. No comparator changes.
    const Eval* a_in[2]; const Eval* b_in[2]; const Eval* c1_in[2]; const Eval* c2_in[2]; const Eval* c3_in[2];
    for (int i = 0; i < 2; ++i) {
        const Eval& m = mutant.empty() ? ev_main[i] : ev_mut[i];
        a_in[i] = (mv == "C1") ? &ev_c1[i] : (mv == "C2") ? &ev_c2[i] : (mv == "C3") ? &ev_c3[i] : &m;
        b_in[i] = (mv == "C3") ? &ev_c3[i] : &m;
        c1_in[i] = (mv == "INVC1") ? &ev_main[i] : &ev_c1[i];
        c2_in[i] = (mv == "INVC2") ? &ev_main[i] : &ev_c2[i];
        c3_in[i] = (mv == "INVC3") ? &ev_main[i] : &ev_c3[i];
        checks.note(a_in[i]->calib);
        checks.note(std::string("P1 ") + runs[i].name + ": a=" + (a_in[i]->a ? "ok" : "FAILED") + " b=" +
                    (b_in[i]->b ? "ok" : "FAILED"));
    }

    // ---- the preconditions, over every input the lines below read ----------
    auto all_pre = [&](bool Pre::*member) {
        for (int i = 0; i < 2; ++i)
            for (const Eval* e : {a_in[i], b_in[i], c1_in[i], c2_in[i], c3_in[i]})
                if (!(e->pre.*member)) return false;
        return true;
    };
    checks.expect(all_pre(&Pre::g1),
        "P1-G1: the program finished -- END was written and every DMACONR readback was stored (OCS and ECS)");
    checks.expect(all_pre(&Pre::g2),
        "P1-G2 [p.144]: each stored DMACONR readback shows DMAEN (bit 9) and the enabled channel's own bit set "
        "(OCS and ECS)");
    checks.expect(all_pre(&Pre::g4),
        "P1-G4: V(W_x) >= 2 for every channel window, and at least one vertical-sync pulse "
        "separates C_x from O_(x+1) (OCS and ECS)");
    checks.expect(all_pre(&Pre::g5),
        "P1-G5: of DMACON bits 8-0 in each stored readback, only the channel x being enabled is set "
        "(OCS and ECS)");
    checks.expect(all_pre(&Pre::g3),
        "P1-G3: zero writes observed in any A_x/A_x^L/A_x^H range across all four windows (OCS and ECS)");

    // ---- the graded lines ---------------------------------------------------
    auto deps_of = [&](const Eval* const (&in)[2], const Eval* const (&in2)[2]) {
        Deps d;
        for (int i = 0; i < 2; ++i) { d.need_pre(in[i]->pre, "P1"); d.need_pre(in2[i]->pre, "P1"); }
        return d;
    };
    auto all_of = [&](const Eval* const (&in)[2], bool Eval::*member) { return in[0]->*member && in[1]->*member; };

    {
        Deps d = deps_of(a_in, a_in);
        if (d.met())
            checks.expect(all_of(a_in, &Eval::a),
                "P1a [p.139, p.144, p.266]: for every channel x=0..3, at least 2 reads at A_x in W_x once the "
                "location is written and the channel started, under OCS and ECS");
        else not_graded(checks, "P1a", d);
    }
    {
        Deps d = deps_of(b_in, b_in);
        if (d.met())
            checks.expect(all_of(b_in, &Eval::b),
                "P1b [p.144, p.145]: for every channel x=0..3, no read at A_y (y != x) in W_x -- no other channel "
                "takes the value, under OCS and ECS");
        else not_graded(checks, "P1b", d);
    }
    {
        Deps d = deps_of(a_in, b_in);
        if (d.met())
            checks.expect(all_of(a_in, &Eval::a) && all_of(b_in, &Eval::b),
                std::string("P1 [") + kClaims +
                "]: P1a and P1b both hold: each channel x fetches from the location written to AUDxLCH and "
                "AUDxLCL and no other channel takes the value, under OCS and ECS");
        else not_graded(checks, "P1", d);
    }
    {
        Deps d = deps_of(c1_in, c1_in);
        if (d.met())
            checks.expect(all_of(c1_in, &Eval::c1),
                "P1-C1 [p.139, p.266]: with only the word written to AUDxLCH changed, to A_x^H, at least 2 reads "
                "at A_x^H and none at A_x in W_x, for every channel x, under OCS and ECS");
        else not_graded(checks, "P1-C1", d);
    }
    {
        Deps d = deps_of(c2_in, c2_in);
        if (d.met())
            checks.expect(all_of(c2_in, &Eval::c2),
                "P1-C2 [p.139, p.266]: with only the word written to AUDxLCL changed, to A_x^L, at least 2 reads "
                "at A_x^L and none at A_x in W_x, for every channel x, under OCS and ECS");
        else not_graded(checks, "P1-C2", d);
    }
    {
        Deps d = deps_of(c3_in, c3_in);
        if (d.met())
            checks.expect(all_of(c3_in, &Eval::c3),
                "P1-C3 [p.263, p.266]: with the values written to channels 0 and 1 swapped, at least 2 reads at A_1 "
                "and none at A_0 in W_0, and at least 2 reads at A_0 and none at A_1 in W_1, under OCS and ECS");
        else not_graded(checks, "P1-C3", d);
    }

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

// What one run of one program shows: G1, and what the comparators of P2 and
// P2-OMIT say about it.
struct Eval {
    bool g1 = false;
    bool table = false;       // each stored value, masked, equals its row
    bool bit0_clear = false;  // the value stored after the omitted SET write shows bit 0 = 0
    int first_bad = -1;
    std::vector<uint16_t> r;
};

Eval evaluate(const Capture& cap) {
    Eval e;
    e.r = stored_at(cap.log, kR, 26);
    e.g1 = cap.loaded && cap.done.has_value() && e.r.size() == 26;
    if (e.r.size() == 26) {
        e.table = true;
        for (int i = 0; i < 26; ++i)
            if ((e.r[i] & 0x07FF) != kExpected[i]) { e.table = false; e.first_bad = i; break; }
        e.bit0_clear = (e.r[2] & 0x0001) == 0;
    }
    return e;
}

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    std::vector<ExtraPreload> extra = {{kList, {0xFFFF, 0xFFFE}}};

    // SPECIFICATION.md P2: "Run under OCS and ECS".
    nmh::Config cfg_ecs; cfg_ecs.chipset_config = 0x08;
    nmh::Config cfg_ocs; cfg_ocs.chipset_config = 0x00;
    const struct { const char* name; nmh::Config cfg; } runs[2] = {{"ECS", cfg_ecs}, {"OCS", cfg_ocs}};

    Eval ev_main[2], ev_omit[2], ev_g1[2];
    for (int i = 0; i < 2; ++i) {
        ev_main[i] = evaluate(run_capture(dir + "/prog_p2.bin", run_budget, runs[i].cfg, extra));
        ev_omit[i] = evaluate(run_capture(dir + "/prog_p2_omit.bin", run_budget, runs[i].cfg, extra));
        if (mv == "G1") ev_g1[i] = evaluate(run_capture(dir + "/prog_p2_g1.bin", run_budget, runs[i].cfg, extra));
    }

    // OMIT puts the program without one SET write through the comparator of
    // P2; INVOMIT puts the main program through the comparator of P2-OMIT;
    // G1 replaces the main program by the one that never finishes. No
    // comparator changes.
    const Eval* p2_in[2]; const Eval* omit_in[2];
    for (int i = 0; i < 2; ++i) {
        p2_in[i] = (mv == "OMIT") ? &ev_omit[i] : (mv == "G1") ? &ev_g1[i] : &ev_main[i];
        omit_in[i] = (mv == "INVOMIT") ? &ev_main[i] : &ev_omit[i];
        if (!p2_in[i]->table && p2_in[i]->first_bad >= 0)
            checks.note(std::string("P2 ") + runs[i].name + ": row " + num(p2_in[i]->first_bad) + " stored " +
                        hex16(p2_in[i]->r[p2_in[i]->first_bad] & 0x07FF) + ", expected " +
                        hex16(kExpected[p2_in[i]->first_bad]));
    }

    checks.expect(p2_in[0]->g1 && p2_in[1]->g1 && omit_in[0]->g1 && omit_in[1]->g1,
                  "P2-G1: the program finished and all 26 values were stored (OCS and ECS)");

    {
        Deps d;
        for (int i = 0; i < 2; ++i) d.need(p2_in[i]->g1, "P2-G1");
        if (d.met())
            checks.expect(p2_in[0]->table && p2_in[1]->table, std::string("P2 [") + kClaims +
                "]: each of the 26 stored DMACONR readbacks, masked with $07FF, equals the proposition's table "
                "(row 3 grades every DMA control bit on its own; row 5 grades bits written as zero staying "
                "unchanged), under OCS and ECS");
        else not_graded(checks, "P2", d);
    }
    {
        Deps d;
        for (int i = 0; i < 2; ++i) d.need(omit_in[i]->g1, "P2-G1");
        if (d.met())
            checks.expect(omit_in[0]->bit0_clear && omit_in[1]->bit0_clear,
                "P2-OMIT [p.278]: with the SET write for DMACON bit 0 left out of step 3, the DMACONR value stored "
                "next shows bit 0 = 0 (bits written with a zero are unchanged), under OCS and ECS");
        else not_graded(checks, "P2-OMIT", d);
    }

    // The "only" of "read-only" is not graded; the read half is.
    checks.record(
        "P2: the 'only' of 'read-only' is not graded. p.263 says of a write to a read-only register only that it "
        "'will cause unpredictable results', which gives no expected value, so no program writes $DFF002 "
        "(half graded: 5-appA-p278.c6, A-dmaconr.acc, 5-appB-p289.c2)");
    // The blitter-status half of the DMA-control and blitter-status register is graded by P8 and P9.
    checks.record(
        "P2: the blitter-status half of 'DMA-control and blitter-status read register' is not graded here: every "
        "stored value is masked with $07FF, which leaves out bit 14 (blitter busy) and bit 13 (blitter zero); P8 "
        "grades bit 14 and P9 grades bit 13 "
        "(half graded: 5-appA-p278.c8, 5-appB-p289.c4)");
}

}  // namespace p2
// ===========================================================================
// P3-P6 share one shape: one DMACON enable bit, with DMAEN set, turns one
// kind of DMA on, and with that bit clear the DMA does not happen. The run
// of each proposition is made twice, with the bit set (the main program) and
// with the bit clear (the _c1 program); both halves are graded in the normal
// run, and the contrasting cases of the mutation records put one program
// through the other half's unchanged comparator.
// ===========================================================================

namespace enableprop {

struct Def {
    const char* name;      // "P3"
    const char* prog;      // "p3": the programs are prog_p3.bin, prog_p3_c1.bin, prog_p3_g1.bin ...
    const char* claims;    // the ids and page of the aggregate line
    const char* pages;     // the pages of the two halves' lines
    const char* bit_name;  // "BPLEN"
    uint16_t bit;          // the DMACON bit's mask
    uint16_t o_id, c_id;   // the window's markers
    int min_v;             // G4: the vertical-sync pulses the window must hold
    const char* on_text;   // what the main run must show
    const char* off_text;  // what the contrast run must show
    const char* g3_text;   // G3's own wording
    const char* g4_text;   // G4's own wording
    std::vector<ExtraPreload> extra;
    bool sprite0_real;
    bool has_g3in;         // a second G3 mutation, inside the footprint
    bool (*seen_on)(const Capture&, const Window&);
    bool (*seen_off)(const Capture&, const Window&);
    // G3 for one run: no CPU write falls into the block under test; `on` says
    // whether the run is the one with the bit set.
    bool (*g3_ok)(const Capture&, const Window&, bool on);
};

// One run of one program, kept with the preconditions it shows. `on` says
// whether the run intends the bit set.
struct Run {
    Capture cap;
    std::optional<Window> w;
    Pre pre;
};

Run make_run(const Def& d, Capture cap, bool on) {
    Run r;
    r.cap = std::move(cap);
    r.w = find_window(r.cap.log, d.o_id, d.c_id);
    const Capture& c = r.cap;
    const std::optional<Window>& w = r.w;

    r.pre.g1 = c.loaded && c.done.has_value() && w.has_value();
    auto rs = stored_at(c.log, kR, 1);
    bool stored = c.loaded && rs.size() == 1;
    uint16_t v = stored ? rs[0] : uint16_t(0);
    // DMAEN (bit 9) set, and the bit under test set or clear as the run intends.
    r.pre.g2 = stored && (v & 0x0200) && (on ? (v & d.bit) != 0 : (v & d.bit) == 0);
    // No other DMA bit among 8-0.
    r.pre.g5 = stored && (v & 0x01FF) == (on ? d.bit : 0);
    r.pre.g4 = c.loaded && w.has_value() &&
               (int)count_window(vsync_starts(c.video), w->lo, w->hi) >= d.min_v;
    r.pre.g3 = c.loaded && w.has_value() && d.g3_ok(c, *w, on);
    return r;
}

void run(const Def& d, Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    auto bin = [&](const char* suffix) { return dir + "/prog_" + d.prog + suffix + ".bin"; };
    nmh::Config cfg;
    auto go = [&](const std::string& b) { return run_capture(b, run_budget, cfg, d.extra, d.sprite0_real); };
    const std::string nm = d.name;

    // A precondition mutation replaces the main program (the run with the
    // bit set) everywhere it is read; the contrast run keeps its own.
    const char* mutant = nullptr;
    if (mv == "G1") mutant = "_g1";
    else if (mv == "G2") mutant = "_g2";
    else if (mv == "G3") mutant = "_g3";
    else if (d.has_g3in && mv == "G3IN") mutant = "_g3in";
    else if (mv == "G4") mutant = "_g4";
    else if (mv == "G5") mutant = "_g5";

    Run main_run = make_run(d, go(bin(mutant ? mutant : "")), true);
    Run c1_run = make_run(d, go(bin("_c1")), false);

    // C1 puts the contrast program through the on-comparator and INV puts the
    // main program through the off-comparator; neither comparator changes.
    const Run& in_on = (mv == "C1") ? c1_run : main_run;
    const Run& in_off = (mv == "INV") ? main_run : c1_run;

    // ---- the preconditions, over the run with the bit set and the run with it clear ----
    checks.expect(main_run.pre.g1 && c1_run.pre.g1, nm + "-G1: the program finished");
    checks.expect(main_run.pre.g2 && c1_run.pre.g2,
        nm + "-G2 [p.144]: the stored DMACONR readback shows DMAEN (bit 9) set and " + d.bit_name +
        " set with the bit enabled and clear with the bit left clear");
    checks.expect(main_run.pre.g4 && c1_run.pre.g4, nm + "-G4: " + d.g4_text);
    checks.expect(main_run.pre.g5 && c1_run.pre.g5,
        nm + "-G5: " + std::string(d.bit_name) + " is the only DMACON bit 8-0 set in the stored readback when "
        "enabled, and none is set when the bit is left clear");
    checks.expect(main_run.pre.g3 && c1_run.pre.g3, nm + "-G3: " + d.g3_text);

    // ---- the graded lines ----------------------------------------------------
    Deps d_on, d_off, d_both;
    d_on.need_pre(in_on.pre, nm);
    d_off.need_pre(in_off.pre, nm);
    d_both.need_pre(in_on.pre, nm);
    d_both.need_pre(in_off.pre, nm);

    if (d_on.met())
        checks.expect(d.seen_on(in_on.cap, *in_on.w), nm + "-ON [" + d.pages + "]: " + d.on_text);
    else not_graded(checks, nm + "-ON", d_on);
    if (d_off.met())
        checks.expect(d.seen_off(in_off.cap, *in_off.w), nm + "-OFF [" + d.pages + "]: " + d.off_text);
    else not_graded(checks, nm + "-OFF", d_off);
    if (d_both.met())
        checks.expect(d.seen_on(in_on.cap, *in_on.w) && d.seen_off(in_off.cap, *in_off.w),
                      nm + " [" + d.claims + "]: " + d.on_text + "; " + d.off_text);
    else not_graded(checks, nm, d_both);
}

}  // namespace enableprop

// ===========================================================================
// P3: BPLEN (bit 8) turns bitplane DMA on (SPECIFICATION.md P3).
// ===========================================================================

namespace p3 {

constexpr uint16_t kO = 0x0020, kC = 0x0021;

bool seen_on(const Capture& cap, const Window& w) {
    size_t at_p = reads_at(cap.log, kP, w.lo, w.hi);
    size_t in_range = reads_in_range(cap.log, kP, kP + 0x8000, w.lo, w.hi);
    return at_p >= 1 && in_range > 0;
}

bool seen_off(const Capture& cap, const Window& w) {
    return reads_in_range(cap.log, kP, kP + 0x8000, w.lo, w.hi) == 0;
}

// G3: no CPU write falls into P during the window.
bool g3_ok(const Capture& cap, const Window& w, bool) {
    return writes_in_range(cap.log, kP, kP + 0x8000, w.lo, w.hi) == 0;
}

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    enableprop::Def d{
        "P3", "p3", "p.278 5-appA-p278.c17, A-dmacon.b08", "p.62, p.278", "BPLEN", 0x0100, kO, kC, 3,
        "with BPLEN set, at least one read at P and more than zero reads in [P,P+$8000) in the window",
        "with BPLEN clear, no read in [P,P+$8000) in the window",
        "zero writes observed in [P,P+$8000) in the window",
        "V(W) >= 3 (two whole frames and the display-window lines inside the window)",
        {}, false, false, seen_on, seen_off, g3_ok};
    enableprop::run(d, checks, dir, mv, run_budget);
}

}  // namespace p3

// ===========================================================================
// P4: COPEN (bit 7) turns Copper DMA on (SPECIFICATION.md P4).
// ===========================================================================

namespace p4 {

constexpr uint16_t kO = 0x0030, kC = 0x0031;

bool seen_on(const Capture& cap, const Window& w) {
    return reads_at(cap.log, kList, w.lo, w.hi) >= 2;
}

bool seen_off(const Capture& cap, const Window& w) {
    return reads_in_range(cap.log, kList, kList + 8, w.lo, w.hi) == 0;
}

// G3: no CPU write falls into L during the window.
bool g3_ok(const Capture& cap, const Window& w, bool) {
    return writes_in_range(cap.log, kList, kList + 64, w.lo, w.hi) == 0;
}

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    enableprop::Def d{
        "P4", "p4", "p.278 5-appA-p278.c18, A-dmacon.b07", "p.30, p.278", "COPEN", 0x0080, kO, kC, 3,
        "with COPEN set, at least 2 reads at L in the window (two vertical blankings, each restarting the "
        "Copper at L)",
        "with COPEN clear, no read in [L,L+8) in the window",
        "zero writes observed in [L,L+64) in the window",
        "V(W) >= 3",
        {{kList, {0x0180, 0x0F00, 0xFFFF, 0xFFFE}}}, false, false, seen_on, seen_off, g3_ok};
    enableprop::run(d, checks, dir, mv, run_budget);
}

}  // namespace p4

// ===========================================================================
// P5: BLTEN (bit 6) turns blitter DMA on (SPECIFICATION.md P5).
// ===========================================================================

namespace p5 {

constexpr uint16_t kO = 0x0040, kC = 0x0041;

bool seen_on(const Capture& cap, const Window& w) {
    return writes_in_range(cap.log, kQ, kQ + 0x200, w.lo, w.hi) >= 1;
}

// With BLTEN clear: no write in [Q,Q+$200) and no write anywhere outside the
// CPU's own areas. G, R and MK (SPECIFICATION.md 2.3) all fall inside one
// contiguous span, kCpuAreaLo..kCpuAreaHi.
bool seen_off(const Capture& cap, const Window& w) {
    size_t writes_q = writes_in_range(cap.log, kQ, kQ + 0x200, w.lo, w.hi);
    size_t writes_anywhere = writes_in_range(cap.log, 0x000000, 0x200000, w.lo, w.hi) -
                             writes_in_range(cap.log, kCpuAreaLo, kCpuAreaHi, w.lo, w.hi);
    return writes_q == 0 && writes_anywhere == 0;
}

// G3. The blitter's own destination footprint is [Q,Q+$200): BLTSIZE $0410 is
// height 16 and width 16 (p.269), 256 words, each written once. With the bit
// set, no write falls into the rest of Q's block, and every word of the
// footprint is written exactly once, so no CPU write lies among the
// blitter's. With the bit clear, no write falls into the block at all.
bool g3_ok(const Capture& cap, const Window& w, bool on) {
    if (!on) return writes_in_range(cap.log, kQ, kQ + 0x2000, w.lo, w.hi) == 0;
    if (writes_in_range(cap.log, kQ + 0x200, kQ + 0x2000, w.lo, w.hi) != 0) return false;
    std::vector<int> per_word(256, 0);
    uint32_t base = word_addr(kQ);
    for (const auto& a : cap.log)
        if (a.is_write && a.cycle >= w.lo && a.cycle <= w.hi && a.word_addr >= base && a.word_addr < base + 256)
            ++per_word[a.word_addr - base];
    for (int n : per_word) if (n != 1) return false;
    return true;
}

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    enableprop::Def d{
        "P5", "p5", "p.278 5-appA-p278.c19, A-dmacon.b06", "p.186, p.269, p.278", "BLTEN", 0x0040, kO, kC, 1,
        "with BLTEN set, at least one write in [Q,Q+$200) in the window",
        "with BLTEN clear, no write in [Q,Q+$200) and no write anywhere outside the CPU's own areas",
        "with BLTEN set, zero writes observed in [Q+$200,Q+$2000) in the window and each word of [Q,Q+$200) "
        "written exactly once (BLTSIZE $0410 is 16 x 16 words, p.269), so no CPU write lies among the "
        "blitter's own; with BLTEN clear, zero writes in [Q,Q+$2000)",
        "V(W) >= 1",
        {}, false, true, seen_on, seen_off, g3_ok};
    enableprop::run(d, checks, dir, mv, run_budget);
}

}  // namespace p5

// ===========================================================================
// P6: SPREN (bit 5) turns sprite DMA on for all eight sprites
// (SPECIFICATION.md P6).
// ===========================================================================

namespace p6 {

constexpr uint16_t kO = 0x0050, kC = 0x0051;

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

// G3: no CPU write falls into S0 (the block under test) during the window.
bool g3_ok(const Capture& cap, const Window& w, bool) {
    return writes_in_range(cap.log, kS0, kS0 + 0x100, w.lo, w.hi) == 0;
}

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    enableprop::Def d{
        "P6", "p6", "p.278 5-appA-p278.c20, A-dmacon.b05", "p.114, p.126, p.278", "SPREN", 0x0020, kO, kC, 3,
        "with SPREN set, each of the 16 addresses (2 words x 8 sprite structures) is read at least once in "
        "the window",
        "with SPREN clear, none of the eight blocks is read at all",
        "zero writes observed in [S0,S0+$100) in the window",
        "V(W) >= 3",
        {}, /*sprite0_real=*/true, false, all16_read, none_of_8_blocks, g3_ok};
    enableprop::run(d, checks, dir, mv, run_budget);
}

}  // namespace p6

// ===========================================================================
// P7: with BLTPRI (bit 10) set, the CPU gets no Chip-memory cycle while
// blitter DMA runs (SPECIFICATION.md P7).
// ===========================================================================

namespace p7 {

const char kClaims[] = "p.278 5-appA-p278.c15, A-dmacon.b10";

constexpr uint16_t kC = 0x0091, kEnd = 0x00FF;

// The blitter's A, B and C source blocks (ch5.i SRCA, SRCB, SRCC). The CPU
// never reads there, so a read in the range is a blitter fetch.
constexpr uint32_t kSrcLo = 0x0A0000;
constexpr uint32_t kSrcHi = 0x0A6000;

// "CPU accesses" per section 2.4: the program image (instruction fetches)
// and G (the loop's own counter writes). R/MK/stack are not touched
// during the interval this proposition measures.
size_t cpu_accesses_in(const std::vector<nmh::RamAccess>& log, uint64_t lo, uint64_t hi) {
    size_t n = reads_in_range(log, kProgLo, kProgHi, lo, hi);
    n += writes_in_range(log, kLog, kLog + 0x2000, lo, hi);
    return n;
}

// The blit's interval and the CPU's accesses inside it. T_first and T_last
// are the times of the first and last blitter source read.
struct Blit {
    bool ran = false;          // at least 2 source reads, so T_first < T_last
    uint64_t t_first = 0, t_last = 0;
    size_t cpu_n = 0;          // CPU accesses in (T_first, T_last)
};

Blit measure(const Capture& cap) {
    Blit b;
    uint32_t lo = word_addr(kSrcLo), hi = word_addr(kSrcHi);
    size_t n = 0;
    uint64_t tf = UINT64_MAX, tl = 0;
    for (const auto& a : cap.log)
        if (!a.is_write && a.word_addr >= lo && a.word_addr < hi) {
            ++n;
            tf = std::min(tf, a.cycle);
            tl = std::max(tl, a.cycle);
        }
    b.ran = cap.loaded && n >= 2;
    if (b.ran) {
        b.t_first = tf;
        b.t_last = tl;
        b.cpu_n = (tl > tf + 1) ? cpu_accesses_in(cap.log, tf + 1, tl - 1) : 0;
    }
    return b;
}

std::string describe(const char* name, const Blit& b) {
    if (!b.ran) return std::string("P7 ") + name + ": no blit";
    return std::string("P7 ") + name + ": T_first=" + num((long long)b.t_first) + " T_last=" +
           num((long long)b.t_last) + " CPU accesses in between=" + num((long long)b.cpu_n);
}

// The CPU still wanted the bus when the blit ended: at least one CPU write
// into G falls after T_last, with fewer than 2048 recorded before it.
bool cpu_still_wanted(const Capture& cap) {
    Blit b = measure(cap);
    if (!b.ran) return false;
    size_t after = 0, before = 0;
    for (const auto& a : cap.log)
        if (a.is_write && a.word_addr >= word_addr(kLog) && a.word_addr < word_addr(kLog + 0x2000)) {
            if (a.cycle > b.t_last) ++after; else ++before;
        }
    return after >= 1 && before < 2048;
}


// The blit of the D-only program (USE code 1): its interval is the first to
// the last destination write in [Q, Q + $2000), and the CPU accesses inside
// it are counted as for the graded blit. It is measured, not graded.
struct DBlit {
    bool ran = false;
    uint64_t t_first = 0, t_last = 0;
    size_t cpu_n = 0;
};

DBlit measure_d(const Capture& cap) {
    DBlit b;
    uint32_t lo = word_addr(kQ), hi = word_addr(kQ + 0x2000);
    size_t n = 0;
    uint64_t tf = UINT64_MAX, tl = 0;
    for (const auto& a : cap.log)
        if (a.is_write && a.word_addr >= lo && a.word_addr < hi) {
            ++n;
            tf = std::min(tf, a.cycle);
            tl = std::max(tl, a.cycle);
        }
    b.ran = cap.loaded && n >= 2;
    if (b.ran) {
        b.t_first = tf;
        b.t_last = tl;
        b.cpu_n = (tl > tf + 1) ? cpu_accesses_in(cap.log, tf + 1, tl - 1) : 0;
    }
    return b;
}

std::string describe_d(const char* name, const DBlit& b) {
    if (!b.ran) return std::string("P7 D-only blit, ") + name + ": no blit";
    return std::string("P7 D-only blit, ") + name + ": T_first=" + num((long long)b.t_first) + " T_last=" +
           num((long long)b.t_last) + " CPU accesses in between=" + num((long long)b.cpu_n);
}

// What one run of one program shows: the blit, and the preconditions.
struct Eval {
    Blit b;
    bool g1 = false;   // the program finished -- END was written
    bool g2 = false;   // the stored DMACONR readback shows what the run intends
    bool g3 = false;   // the blit ran
    bool g6 = false;   // the CPU still wanted the bus when the blit ended
};

// `bltpri` says whether the run intends BLTPRI set.
Eval evaluate(const Capture& cap, bool bltpri) {
    Eval e;
    e.b = measure(cap);
    e.g1 = cap.loaded && cap.done.has_value() && marker(cap.log, kC).has_value() &&
           marker(cap.log, kEnd).has_value();
    auto r = stored_at(cap.log, kR, 1);
    if (cap.loaded && r.size() == 1) {
        uint16_t v = r[0];
        e.g2 = (v & 0x0200) && (v & 0x0040) && (bltpri ? (v & 0x0400) != 0 : (v & 0x0400) == 0) &&
               ((v & 0x01FF & ~0x0040) == 0);
    }
    e.g3 = e.b.ran;
    e.g6 = cpu_still_wanted(cap);
    return e;
}

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    Capture cap_main = run_capture(dir + "/prog_p7.bin", run_budget);
    Capture cap_c1 = run_capture(dir + "/prog_p7_c1.bin", run_budget);
    Eval e_main = evaluate(cap_main, true);
    Eval e_c1 = evaluate(cap_c1, false);
    checks.note(describe("BLTPRI set", e_main.b));
    checks.note(describe("BLTPRI clear", e_c1.b));

    // A precondition mutation replaces the main program (BLTPRI set).
    Eval e_mut;
    const char* mutant = nullptr;
    if (mv == "G1") mutant = "prog_p7_g1.bin";
    else if (mv == "G2") mutant = "prog_p7_g2.bin";
    else if (mv == "G3") mutant = "prog_p7_g3.bin";
    else if (mv == "GEARLY") mutant = "prog_p7_gearly.bin";
    if (mutant) e_mut = evaluate(run_capture(dir + "/" + mutant, run_budget), true);
    const Eval& e_main_in = mutant ? e_mut : e_main;

    // C1 puts the BLTPRI-clear program through the on-comparator; INV puts the
    // main program through the off-comparator. Neither comparator changes.
    const Eval& in_on = (mv == "C1") ? e_c1 : e_main_in;
    const Eval& in_off = (mv == "INV") ? e_main : e_c1;

    // ---- the preconditions, over the run with BLTPRI set and the run with it clear ----
    checks.expect(e_main_in.g1 && e_c1.g1, "P7-G1: the program finished -- END was written");
    checks.expect(e_main_in.g2 && e_c1.g2,
        "P7-G2: the stored DMACONR readback shows DMAEN and BLTEN set, BLTPRI matching the run's own enabling "
        "write (set in the main run, clear in the contrast run), and no other bit among 8-0");
    checks.expect(e_main_in.g3 && e_c1.g3, "P7-G3: the blit ran -- at least 2 source reads, so T_first < T_last");
    checks.expect(e_main_in.g6 && e_c1.g6,
        "P7-G(CPU-still-wanted-the-bus): at least one CPU write into G falls after T_last, with fewer than 2048 "
        "recorded before it");

    // ---- the graded lines ----------------------------------------------------
    auto deps_of = [&](std::initializer_list<const Eval*> ins) {
        Deps d;
        for (const Eval* e : ins) {
            d.need(e->g1, "P7-G1");
            d.need(e->g2, "P7-G2");
            d.need(e->g3, "P7-G3");
            d.need(e->g6, "P7-G(CPU-still-wanted-the-bus)");
        }
        return d;
    };
    Deps d_on = deps_of({&in_on});
    Deps d_off = deps_of({&in_off});
    Deps d_both = deps_of({&in_on, &in_off});
    if (d_on.met())
        checks.expect(in_on.b.cpu_n == 0,
            "P7-ON [p.278, p.222, p.198]: with BLTPRI set, zero CPU accesses in (T_first,T_last), the first and last "
            "blitter source read of a blit that uses A, B and C and not D (USE code E, p.188: A0 B0 C0 A1 B1 C1 A2 B2 C2)");
    else not_graded(checks, "P7-ON", d_on);
    if (d_off.met())
        checks.expect(in_off.b.cpu_n >= 1,
            "P7-OFF [p.198]: with BLTPRI clear, at least one CPU access in (T_first,T_last) of the same blit");
    else not_graded(checks, "P7-OFF", d_off);
    if (d_both.met())
        checks.expect(in_on.b.cpu_n == 0 && in_off.b.cpu_n >= 1,
            std::string("P7 [") + kClaims +
            "]: with BLTPRI set, zero CPU accesses in (T_first,T_last); with BLTPRI clear, at least one (p.198)");
    else not_graded(checks, "P7", d_both);

    // ---- the case the pages leave open: a D-only blit, measured and not graded ----
    DBlit d_set = measure_d(run_capture(dir + "/prog_p7_donly.bin", run_budget));
    DBlit d_clr = measure_d(run_capture(dir + "/prog_p7_donly_c1.bin", run_budget));
    checks.note(describe_d("BLTPRI set", d_set));
    checks.note(describe_d("BLTPRI clear", d_clr));

    checks.record(
        "P7: with a blit whose Table 6-2 sequence leaves a cycle without a blitter access (D only: 'D0 - D1 - D2', "
        "p.188), the pages do not say whether BLTPRI withholds that unused cycle from the CPU. Read literally, "
        "p.278 ('disables /BLS pin, preventing micro from stealing any bus cycles while blitter DMA is running') "
        "gives the CPU no cycle. Read as a priority, p.278 ('Blitter DMA priority (over CPU micro)'), p.222 "
        "('the blitter has full (instead of partial) priority over the 680x0') and p.194 ('The lowest priorities "
        "are assigned to the blitter and the 68000, in that order') give the CPU a cycle that the blitter does "
        "not request. p.198 ('the blitter will keep the bus for every available Chip memory cycle') fits both. "
        "Such a blit is measured above and not graded.");
}

}  // namespace p7

// ===========================================================================
// P8: BBUSY (bit 14) reads 1 while a blit is in progress and 0 when no blit
// is in progress (SPECIFICATION.md P8).
// ===========================================================================

namespace p8 {

const char kClaims[] =
    "p.278 5-appA-p278.c13, A-dmacon.b14; p.278 5-appA-p278.c10, 5-appA-p278.c8; p.289 5-appB-p289.c4";

constexpr uint16_t kOM = 0x0060, kCM = 0x0061;

// G is $4000 bytes (SPECIFICATION.md 2.3); R lies above it.
constexpr uint32_t kLogEnd = kLog + 0x4000;

// The logged DMACONR reads of a run, in order: 4 idle reads, then the poll
// (read 1 first), then the 8 extra reads; and the blit's last destination
// write.
struct Poll {
    bool loaded = false;
    std::vector<uint64_t> c;
    std::vector<uint16_t> v;
    std::optional<uint64_t> t_last;
    size_t n_q = 0;
    size_t n_poll() const { return c.size() >= 12 ? c.size() - 12 : 0; }
};

Poll parse(const Capture& cap) {
    Poll p;
    p.loaded = cap.loaded;
    for (const auto& w : writes_seq(cap.log, kLog, kLogEnd)) { p.c.push_back(w.cycle); p.v.push_back(w.data); }
    uint64_t tl = 0;
    for (const auto& a : cap.log)
        if (a.is_write && a.word_addr >= word_addr(kQ) && a.word_addr < word_addr(kQ + 0x2000)) {
            ++p.n_q;
            tl = std::max(tl, a.cycle);
        }
    if (p.n_q > 0) p.t_last = tl;
    return p;
}

bool blit_ran(const Poll& p) { return p.t_last.has_value() && p.n_q >= 2; }

// (a): the 4 idle reads show bit 14 = 0.
bool idle_ok(const Poll& p) {
    if (p.c.size() < 4) return false;
    for (int i = 0; i < 4; ++i) if (p.v[i] & 0x4000) return false;
    return true;
}

// At least 1 poll read and the 8 extra reads were stored.
bool struct_ok(const Poll& p) { return p.c.size() >= 4 + 9; }

// The poll reads (0-based k; 1-based read number k+1) that (b) grades: read
// numbers 2 and up whose store lies before T_last.
std::vector<size_t> graded_set(const Poll& p) {
    std::vector<size_t> g;
    if (!struct_ok(p) || !blit_ran(p)) return g;
    for (size_t k = 1; k < p.n_poll(); ++k)
        if (p.c[4 + k] < *p.t_last) g.push_back(k);
    return g;
}

// (b): every graded read that the run holds shows bit 14 = 1; at least one
// graded read is held. `want` is the bit the reads must show.
bool graded_reads_show(const Poll& p, const std::vector<size_t>& graded, bool want, size_t* held = nullptr) {
    size_t present = 0;
    bool ok = true;
    for (size_t k : graded) {
        if (k >= p.n_poll()) continue;
        ++present;
        if (((p.v[4 + k] & 0x4000) != 0) != want) ok = false;
    }
    if (held) *held = present;
    return ok && present >= 1;
}

// (c): the poll ends on a 0 before the 8192-read cap, and every read from
// that first 0 on is also 0.
bool c_ok(const Poll& p) {
    if (p.c.size() < 4) return false;
    size_t n = p.n_poll();
    long first_zero = -1;
    for (size_t k = 1; k < n; ++k) if (!(p.v[4 + k] & 0x4000)) { first_zero = (long)k; break; }
    if (!(n < 8192 && first_zero >= 0)) return false;
    for (size_t k = (size_t)first_zero; k < p.c.size() - 4; ++k)
        if (p.v[4 + k] & 0x4000) return false;
    return true;
}

// What one run of one program shows.
struct Eval {
    Poll p;
    bool g1 = false;   // the program finished: DONE, both markers, every read stored
    bool g2 = false;   // the stored DMACONR readback and the stored idle reads show DMAEN and BLTEN set, BLTPRI clear, nothing else among 8-0
    bool g3 = false;   // the blit ran
    bool g4 = false;   // at least 2 graded reads, so (b) is not vacuous
};

Eval evaluate(const Capture& cap) {
    Eval e;
    e.p = parse(cap);
    e.g1 = cap.loaded && cap.done.has_value() && marker(cap.log, kOM).has_value() &&
           marker(cap.log, kCM).has_value() && struct_ok(e.p);
    // The stored readback in R and the four stored idle reads of (a) show the
    // enabling write's bits only.
    auto bits_ok = [](uint16_t v) {
        return (v & 0x0200) && (v & 0x0040) && !(v & 0x0400) && ((v & 0x01FF & ~0x0040) == 0);
    };
    auto r8 = stored_at(cap.log, kR, 1);
    if (cap.loaded && r8.size() == 1 && e.p.c.size() >= 4) {
        e.g2 = bits_ok(r8[0]);
        for (int i = 0; i < 4; ++i) if (!bits_ok(e.p.v[i])) e.g2 = false;
    }
    e.g3 = blit_ran(e.p);
    e.g4 = graded_set(e.p).size() >= 2;
    return e;
}

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    // The main run is the reference: its graded indices are the ones the
    // other runs are graded at.
    Eval ref = evaluate(run_capture(dir + "/prog_p8.bin", run_budget));
    std::vector<size_t> graded = graded_set(ref.p);
    Eval noblit = evaluate(run_capture(dir + "/prog_p8_c1.bin", run_budget));

    // ZPOLL (the fourth poll read comes from a zero word) is the mutation of
    // P8; each precondition has its own program, which replaces the main
    // program for P8; G3's program is the one without a blit.
    Eval mut;
    const char* mutant = nullptr;
    if (mv == "ZPOLL") mutant = "prog_p8_zpoll.bin";
    else if (mv == "G1") mutant = "prog_p8_g1.bin";
    else if (mv == "GBLTPRI") mutant = "prog_p8_gbltpri.bin";
    else if (mv == "G3") mutant = "prog_p8_c1.bin";
    else if (mv == "G4") mutant = "prog_p8_g4.bin";
    if (mutant) mut = evaluate(run_capture(dir + "/" + mutant, run_budget));
    const Eval& in_p8 = mutant ? mut : ref;
    // INVNOBLIT puts the main program through the comparator of P8-NOBLIT.
    const Eval& in_nb = (mv == "INVNOBLIT") ? ref : noblit;

    checks.note("P8: N(poll)=" + num((long long)ref.p.n_poll()) + " total_rest=" +
                num((long long)(ref.p.c.size() >= 4 ? ref.p.c.size() - 4 : 0)) +
                " t_last=" + (ref.p.t_last ? num((long long)*ref.p.t_last) : std::string("(none)")) +
                " b_nonvacuous=" + num((long long)graded.size()));
    size_t held = 0;
    bool b_this = graded_reads_show(in_p8.p, graded, true, &held);
    checks.note(std::string("P8 clauses of the graded run: idle=") + (idle_ok(in_p8.p) ? "ok" : "FAILED") +
                " (b)=" + (b_this ? "ok" : "FAILED") + " over " + num((long long)held) + " graded reads held" +
                " (c)=" + (c_ok(in_p8.p) ? "ok" : "FAILED"));

    // ---- the preconditions ---------------------------------------------------
    checks.expect(in_p8.g1 && in_nb.g1,
        "P8-G1: the program finished -- DONE was written, both markers were found and every read was stored");
    checks.expect(in_p8.g2 && in_nb.g2,
        "P8-G2: the stored DMACONR readback and the four stored idle reads show DMAEN and BLTEN set, BLTPRI clear, "
        "and no other bit among 8-0");
    checks.expect(in_p8.g3, "P8-G3: the blit ran -- T_last is defined (at least 2 writes in the Q range)");
    // G4: at least 2 poll reads with i >= 2 fall before T_last, so (b) is not vacuous.
    checks.expect(in_p8.g4, "P8-G4: at least 2 poll reads with i >= 2 have T_w(i) < T_last, so (b) is not vacuous");

    // ---- the graded lines ----------------------------------------------------
    {
        Deps d;
        d.need(in_p8.g1, "P8-G1");
        d.need(in_p8.g2, "P8-G2");
        d.need(in_p8.g3, "P8-G3");
        d.need(in_p8.g4, "P8-G4");
        if (d.met())
            checks.expect(idle_ok(in_p8.p) && b_this && c_ok(in_p8.p), std::string("P8 [") + kClaims +
                "]: the 4 idle reads show bit14=0; every poll read i>=2 with T_w(i) < T_last shows bit14=1; the poll "
                "ends on a 0 before the 8192-read cap, and every read after that (including the 8 extra) is also 0");
        else not_graded(checks, "P8", d);
    }
    {
        // The reads graded 1 in the main run, made by the program without a
        // blit, are predicted 0 (p.186 sets the flag only while a blit is in
        // progress). They are taken at the indices the main run grades.
        Deps d;
        d.need(in_nb.g1, "P8-G1");
        d.need(in_nb.g2, "P8-G2");
        d.need(ref.g3, "P8-G3");
        d.need(ref.g4, "P8-G4");
        if (d.met())
            checks.expect(graded_reads_show(in_nb.p, graded, false),
                "P8-NOBLIT [p.186]: with the BLTSIZE write replaced by a write to COLOR00, so that no blit starts, "
                "every read of the poll at an index the main run grades shows bit14=0, and at least one is held");
        else not_graded(checks, "P8-NOBLIT", d);
    }

    checks.record(
        "P8: the 'only' of 'read only' is not graded. p.278 gives bit 15, 'bits written with a 1 get set or "
        "cleared', and calls bit 14 '(read only)', and no page says what a DMACON write with bit 14 set does; "
        "every DMACON write here keeps bit 14 clear "
        "(half graded: 5-appA-p278.c13, A-dmacon.b14)");
}

}  // namespace p8

// ===========================================================================
// P9: after a blit, BZERO (bit 13) reads 1 exactly when the blit's logic
// output was zero throughout (SPECIFICATION.md P9).
// ===========================================================================

namespace p9 {

const char kClaims[] =
    "p.278 5-appA-p278.c14, A-dmacon.b13; p.278 5-appA-p278.c10, 5-appA-p278.c8; p.289 5-appB-p289.c4";

// The five blits' destination blocks (blits 1-3) and source blocks (blits 4
// and 5), each 16 words; the exchanged program reads Zb in blit 4 and F in
// blit 5.
constexpr uint32_t kBlock[5] = {kQ, kQ + 0x100, kQ + 0x200, kF, kZb};
constexpr uint32_t kBlockSwapped[5] = {kQ, kQ + 0x100, kQ + 0x200, kZb, kF};

// What one run of the program shows: Z_1..Z_5 with the times of their stores
// into R, and, logged into G, the read of each wait that showed bit 14 = 0.
struct Run {
    bool ok = false;
    std::vector<uint16_t> z;
    std::vector<uint64_t> z_cycle;
    std::vector<uint16_t> term;
    std::vector<uint64_t> term_cycle;
};

Run parse(const Capture& cap) {
    Run r;
    for (const auto& w : writes_seq(cap.log, kR, kR + 10)) { r.z.push_back(w.data); r.z_cycle.push_back(w.cycle); }
    for (const auto& w : writes_seq(cap.log, kLog, kLog + 10)) { r.term.push_back(w.data); r.term_cycle.push_back(w.cycle); }
    r.ok = cap.loaded && r.z.size() == 5 && r.term.size() == 5;
    return r;
}

// Bit 13 of each Z_k against a table of 1 = set.
bool table_ok(const Run& r, const int expect[5]) {
    if (!r.ok) return false;
    for (int k = 0; k < 5; ++k)
        if (((r.z[k] & 0x2000) != 0) != (expect[k] != 0)) return false;
    return true;
}

// 16 words, all of them written once with `data`.
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

// The time of the last port access in block k's 16 words, if any.
std::optional<uint64_t> last_access(const Capture& cap, int k, bool swapped) {
    uint32_t wlo = word_addr((swapped ? kBlockSwapped : kBlock)[k]), whi = wlo + 16;
    std::optional<uint64_t> t;
    for (const auto& a : cap.log)
        if (a.word_addr >= wlo && a.word_addr < whi) t = t ? std::max(*t, a.cycle) : a.cycle;
    return t;
}

// Blits 4 and 5 leave D unwritten: no write falls outside the CPU's own
// areas (G, R, MK) from the first access to F or Zb to the last.
bool no_write_outside_cpu_areas(const Capture& cap) {
    uint64_t first = UINT64_MAX, last = 0;
    bool any = false;
    for (uint32_t base : {kF, kZb}) {
        uint32_t wlo = word_addr(base), whi = wlo + 16;
        for (const auto& a : cap.log)
            if (a.word_addr >= wlo && a.word_addr < whi) {
                any = true;
                first = std::min(first, a.cycle);
                last = std::max(last, a.cycle);
            }
    }
    if (!any) return false;
    size_t outside = writes_in_range(cap.log, 0x000000, 0x200000, first, last) -
                     writes_in_range(cap.log, kCpuAreaLo, kCpuAreaHi, first, last);
    return outside == 0;
}

// Each Z_k was stored after the last port access of its blit, and after a
// read with bit 14 = 0.
bool after_completion(const Capture& cap, const Run& r, bool swapped) {
    if (!r.ok) return false;
    for (int k = 0; k < 5; ++k) {
        auto t = last_access(cap, k, swapped);
        if (!t) return false;
        if (r.term[k] & 0x4000) return false;
        if (!(r.term_cycle[k] < r.z_cycle[k])) return false;
        if (!(r.z_cycle[k] > *t)) return false;
    }
    return true;
}


constexpr uint16_t kEndId = 0x00FF;

// What one run of one program shows: Z_k, both tables' verdicts and the
// preconditions. `swapped` says the run is the exchanged program.
struct Eval {
    Run r;
    bool main_table = false, swap_table = false;
    bool g1 = false;    // the program finished: DONE, END, and every Z_k and wait read stored
    bool gw = false;    // the blits made the writes the run intends
    bool gr = false;    // blits 4 and 5 read F and Zb and wrote nowhere but the CPU's own areas
    bool g2 = false;    // every Z_k shows the enabling write's bits only
    bool gac = false;   // each Z_k was stored after its blit's last access and after a read with bit 14 = 0
};

Eval evaluate(const Capture& cap, bool swapped) {
    static const int expect_main[5] = {1, 0, 1, 0, 1};
    static const int expect_swap[5] = {0, 1, 1, 1, 0};
    Eval e;
    e.r = parse(cap);
    e.main_table = table_ok(e.r, expect_main);
    e.swap_table = table_ok(e.r, expect_swap);
    e.g1 = cap.loaded && cap.done.has_value() && marker(cap.log, kEndId).has_value() && e.r.ok;
    // Blits 1 and 3 write $0000 and blit 2 writes $FFFF in the main program; blits 1 and 2 are exchanged
    // in the other.
    const uint16_t d1 = swapped ? 0xFFFF : 0x0000, d2 = swapped ? 0x0000 : 0xFFFF;
    e.gw = cap.loaded && writes_all(cap, kQ, 16, d1) && writes_all(cap, kQ + 0x100, 16, d2) &&
           writes_all(cap, kQ + 0x200, 16, 0x0000);
    e.gr = cap.loaded && reads_only(cap, kF, 16) && reads_only(cap, kZb, 16) && no_write_outside_cpu_areas(cap);
    e.g2 = e.r.ok;
    if (e.g2) for (int k = 0; k < 5; ++k)
        if ((e.r.z[k] & 0x07FF) != 0x0240) { e.g2 = false; break; }
    e.gac = after_completion(cap, e.r, swapped);
    return e;
}

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    Eval ev_main = evaluate(run_capture(dir + "/prog_p9.bin", run_budget), false);
    Eval ev_swap = evaluate(run_capture(dir + "/prog_p9_c1.bin", run_budget), true);

    // A precondition mutation replaces the main program.
    Eval ev_mut;
    const char* mutant = nullptr;
    if (mv == "G1") mutant = "prog_p9_g1.bin";
    else if (mv == "G2") mutant = "prog_p9_g2.bin";
    else if (mv == "GWRITES") mutant = "prog_p9_gwrites.bin";
    else if (mv == "GDWRITE") mutant = "prog_p9_gdwrite.bin";
    else if (mv == "NOWAIT") mutant = "prog_p9_gnowait.bin";
    if (mutant) ev_mut = evaluate(run_capture(dir + "/" + mutant, run_budget), false);
    const Eval& m = mutant ? ev_mut : ev_main;

    // C1 puts the exchanged program through the main table; INV puts the
    // main program through the exchanged table. Neither table changes.
    const Eval& in_main = (mv == "C1") ? ev_swap : m;
    const Eval& in_swap = (mv == "INV") ? ev_main : ev_swap;

    if (ev_main.r.ok) {
        std::string zs = "P9: Z1..Z5 = ";
        for (int k = 0; k < 5; ++k) zs += hex16(ev_main.r.z[k]) + " ";
        checks.note(zs);
    }

    // ---- the preconditions, over the run read as the main program and the run read as the exchanged one ----
    checks.expect(in_main.gw && in_swap.gw,
        "P9-G(writes): blits 1 and 3 made 16 writes of $0000 in their ranges; blit 2 made 16 writes of $FFFF "
        "(blits 1 and 2 exchanged in the exchanged run)");
    checks.expect(in_main.gr && in_swap.gr,
        "P9-G(reads): blits 4 and 5 made 16 reads in F and Zb respectively, no write there, and no write outside "
        "the CPU's own areas while they ran");
    checks.expect(in_main.g1 && in_swap.g1,
        "P9-G1: the program finished -- END was written and every Z_k and every wait read was stored");
    checks.expect(in_main.g2 && in_swap.g2,
        "P9-G2: G2 for $8240 -- every stored Z_k shows DMAEN (bit 9) and BLTEN (bit 6) set and no other bit "
        "among 10 and 8-0");
    checks.expect(in_main.gac && in_swap.gac,
        "P9-G(after-completion): each Z_k (k = 1..5) was stored after the last port access of its blit and after "
        "a read with bit 14 = 0");

    // ---- the graded lines ----------------------------------------------------
    auto deps_of = [&](std::initializer_list<const Eval*> ins) {
        Deps d;
        for (const Eval* e : ins) {
            d.need(e->gw, "P9-G(writes)");
            d.need(e->gr, "P9-G(reads)");
            d.need(e->g1, "P9-G1");
            d.need(e->g2, "P9-G2");
            d.need(e->gac, "P9-G(after-completion)");
        }
        return d;
    };
    Deps d_main = deps_of({&in_main});
    Deps d_swap = deps_of({&in_swap});
    Deps d_both = deps_of({&in_main, &in_swap});
    if (d_main.met())
        checks.expect(in_main.main_table,
            "P9-MAIN [p.222, p.187]: bit 13 of each Z_k matches the proposition's table (blits 1-3 show the flag "
            "describes the latest blit; blits 4-5 grade 'even if D is not written')");
    else not_graded(checks, "P9-MAIN", d_main);
    if (d_swap.met())
        checks.expect(in_swap.swap_table,
            "P9-SWAP [p.222, p.187]: with BLTCON0 exchanged between blits 1 and 2 and the source exchanged between "
            "blits 4 and 5, bit 13 of each Z_k follows the exchanged table");
    else not_graded(checks, "P9-SWAP", d_swap);
    if (d_both.met())
        checks.expect(in_main.main_table && in_swap.swap_table, std::string("P9 [") + kClaims +
            "]: bit 13 of each Z_k matches the proposition's table, and the exchanged table when the logic outputs "
            "are exchanged");
    else not_graded(checks, "P9", d_both);

    checks.record(
        "P9: the 'only' of 'read only' is not graded. p.278 gives bit 15, 'bits written with a 1 get set or "
        "cleared', and calls bit 13 '(read only)', and no page says what a DMACON write with bit 13 set does; "
        "every DMACON write here keeps bit 13 clear "
        "(half graded: 5-appA-p278.c14, A-dmacon.b13)");
}

}  // namespace p9

// ===========================================================================
// P10: the Copper writes AUDxLCH and AUDxLCL whatever the danger bit,
// under OCS and ECS (SPECIFICATION.md P10).
// ===========================================================================

namespace p10 {

constexpr uint16_t kO[4] = {0x0010, 0x0012, 0x0014, 0x0016};
constexpr uint16_t kC[4] = {0x0011, 0x0013, 0x0015, 0x0017};
constexpr uint16_t kKO = 0x0070, kKC = 0x0071, kEnd = 0x00FF;

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

// The same eight MOVEs, addressed to COLOR00.
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

// With the audio list, in W_x channel x fetches from B_x and not from A_x;
// with the COLOR00 list it fetches from A_x and not from B_x. In both, no
// other channel's block is read.
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

// Everything one run shows: the comparison and the preconditions.
struct Eval { bool cmp = false, g1 = false, g2 = false, g3 = false, g4 = false, g5 = false, lst = false; };

Eval evaluate(const Capture& cap, bool expect_audio) {
    Eval e;
    W4 w = find4(cap.log);
    auto ko = marker(cap.log, kKO), kc = marker(cap.log, kKC);
    bool have = cap.loaded && w.ok && ko.has_value() && kc.has_value();
    e.cmp = cap.loaded && cap.done.has_value() && check_main(cap, w, expect_audio);
    e.g1 = have && cap.done.has_value() && marker(cap.log, kEnd).has_value();

    // The readbacks: R[0] after the Copper enable, R[1+x] after channel x's enable.
    auto r = stored_at(cap.log, kR, 5);
    bool stored = have && r.size() == 5;
    e.g2 = stored && (r[0] & 0x0200) && (r[0] & 0x0080);
    e.g5 = stored && (r[0] & 0x01FF) == 0x0080;
    if (stored) for (int x = 0; x < 4; ++x) {
        if (!((r[1 + x] & 0x0200) && (r[1 + x] & (1 << x)))) e.g2 = false;
        if ((r[1 + x] & 0x01FF) != (1u << x)) e.g5 = false;
    }

    e.g4 = have && count_window(vsync_starts(cap.video), *ko, *kc) >= 2;
    if (have) for (int x = 0; x < 4; ++x)
        if (count_window(vsync_starts(cap.video), w.o[x], w.c[x]) < 2) e.g4 = false;

    // G3: no CPU write falls into a DMA block during a window (A_x/B_x in
    // each W_x, L in [K_O,K_C]).
    e.g3 = have;
    if (have) {
        for (int x = 0; x < 4; ++x) {
            if (writes_in_range(cap.log, kA[x], kA[x] + 16, w.o[x], w.c[x]) != 0) e.g3 = false;
            if (writes_in_range(cap.log, kB[x], kB[x] + 16, w.o[x], w.c[x]) != 0) e.g3 = false;
        }
        if (writes_in_range(cap.log, kList, kList + 0x24, *ko, *kc) != 0) e.g3 = false;
    }

    // The Copper ran the list: every word of [L,L+$24) was read inside [K_O,K_C].
    e.lst = have;
    if (have) for (uint32_t off = 0; off < 0x24; off += 2)
        if (reads_at(cap.log, kList + off, *ko, *kc) < 1) { e.lst = false; break; }
    return e;
}

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    nmh::Config cfg_ocs; cfg_ocs.chipset_config = 0x00;
    nmh::Config cfg_ecs; cfg_ecs.chipset_config = 0x08;

    struct Combo { const char* name; bool cdang; nmh::Config cfg; };
    const Combo combos[4] = {
        {"OCS/CDANG0", false, cfg_ocs},
        {"OCS/CDANG1", true,  cfg_ocs},
        {"ECS/CDANG0", false, cfg_ecs},
        {"ECS/CDANG1", true,  cfg_ecs},
    };

    // The mutated programs of the preconditions replace the main program in
    // the runs with CDANG clear.
    std::string mutant;
    if (mv == "G1") mutant = "/prog_p10_g1.bin";
    else if (mv == "G2K") mutant = "/prog_p10_g2k.bin";
    else if (mv == "G2W") mutant = "/prog_p10_g2w.bin";
    else if (mv == "G3") mutant = "/prog_p10_g3.bin";
    else if (mv == "G3L") mutant = "/prog_p10_g3l.bin";
    else if (mv == "G4") mutant = "/prog_p10_g4.bin";
    else if (mv == "G5") mutant = "/prog_p10_g5.bin";
    else if (mv == "G_NOLIST") mutant = "/prog_p10_g_nolist.bin";

    // Every combination is run with the audio list (the main case) and with
    // the COLOR00 list (the contrasting case).
    Eval main_ev[4], color_ev[4];
    bool main_cmp_audio[4], main_cmp_color[4], color_cmp_color[4], color_cmp_audio[4];
    for (int i = 0; i < 4; ++i) {
        std::string bin = dir + (combos[i].cdang ? "/prog_p10_cdang.bin" : "/prog_p10.bin");
        std::string bin_main = (!mutant.empty() && !combos[i].cdang) ? dir + mutant : bin;

        std::vector<ExtraPreload> extra_audio = {{kList, list_audio()}};
        std::vector<ExtraPreload> extra_color = {{kList, list_color()}};
        Capture cap_a = run_capture(bin_main, run_budget, combos[i].cfg, extra_audio);
        Capture cap_c = run_capture(bin, run_budget, combos[i].cfg, extra_color);

        main_ev[i] = evaluate(cap_a, true);
        color_ev[i] = evaluate(cap_c, false);
        // The comparators the mutations of the two halves need.
        main_cmp_audio[i] = main_ev[i].cmp;
        main_cmp_color[i] = evaluate(cap_a, false).cmp;   // audio-list run, COLOR00 comparator
        color_cmp_color[i] = color_ev[i].cmp;
        color_cmp_audio[i] = evaluate(cap_c, true).cmp;   // COLOR00-list run, audio comparator
        checks.note(std::string("P10 ") + combos[i].name + ": audio list " + (main_cmp_audio[i] ? "ok" : "FAILED") +
                    ", COLOR00 list " + (color_cmp_color[i] ? "ok" : "FAILED"));
    }

    // C1 puts the COLOR00 list through the audio comparator; INV puts the audio
    // list through the COLOR00 comparator. Neither comparator changes.
    bool main_ok = true, color_ok = true;
    for (int i = 0; i < 4; ++i) {
        main_ok = main_ok && ((mv == "C1") ? color_cmp_audio[i] : main_cmp_audio[i]);
        color_ok = color_ok && ((mv == "INV") ? main_cmp_color[i] : color_cmp_color[i]);
    }
    checks.expect(main_ok,
        "P10 [p.139, p.147, p.21, p.26, p.287]: under OCS and ECS, with CDANG clear or set, in W_x: at least 2 reads "
        "at B_x and none at A_x, and none at A_y or B_y for y != x");
    checks.expect(color_ok,
        "P10-COLOR [p.21, p.139]: with the same eight MOVEs addressed to COLOR00, under OCS and ECS, with CDANG "
        "clear or set, in W_x: at least 2 reads at A_x and none at B_x, and none at A_y or B_y for y != x");

    // Preconditions, over all eight runs.
    auto all = [&](bool Eval::*member) {
        for (int i = 0; i < 4; ++i) if (!(main_ev[i].*member) || !(color_ev[i].*member)) return false;
        return true;
    };
    checks.expect(all(&Eval::g1), "P10-G1: the program finished -- END was written (every run)");
    checks.expect(all(&Eval::g2),
        "P10-G2 [p.144]: the stored DMACONR readbacks (after the Copper enable and after each channel's enable) "
        "show DMAEN (bit 9) set and the enabled COPEN (bit 7), respectively channel x's bit, set (every run)");
    checks.expect(all(&Eval::g4), "P10-G4: V >= 2 in [K_O,K_C] and in each W_x (every run)");
    checks.expect(all(&Eval::g3),
        "P10-G3: zero writes observed in any A_x/B_x range across all four windows, and in [L,L+$24) in "
        "[K_O,K_C] (every run)");
    checks.expect(all(&Eval::g5),
        "P10-G5: of DMACON bits 8-0 in each stored readback, only COPEN (Copper window) respectively the channel "
        "x being enabled is set (every run)");
    checks.expect(all(&Eval::lst),
        "P10-G(list-ran): every word of [L,L+$24) was read at least once inside [K_O,K_C] (every run)");

    checks.record(
        "C-interp.copcon C-interp.copcon.ecs: not graded. The claims name the whole range $DFF03E-$DFF07E "
        "(standard chip set) and 'all of the Amiga chip registers' (ECS). P10 grades the eight audio location "
        "registers only and P11 grades $054, $056 and $058 only; AUDxLEN, AUDxPER, AUDxVOL, AUDxDAT and the "
        "other registers of the range are not covered. The CDANG-clear half is declined as well: p.21 lets the "
        "Copper write $20 and up, p.26 and p.287 protect $40-$7E, and p.310 opens $03E-$07E");
}

}  // namespace p10

// ===========================================================================
// P11: with the danger bit set, the Copper writes the blitter registers at
// $054, $056 and $058 (SPECIFICATION.md P11).
// ===========================================================================

namespace p11 {

constexpr uint16_t kO = 0x0080, kC = 0x0081, kEnd = 0x00FF;

std::vector<uint16_t> list_main() {
    return {0x0054, uint16_t(kQ2 >> 16), 0x0056, uint16_t(kQ2 & 0xFFFF), 0x0058, 0x0104, 0xFFFF, 0xFFFE};
}
std::vector<uint16_t> list_no_bltsize() {
    return {0x0054, uint16_t(kQ2 >> 16), 0x0056, uint16_t(kQ2 & 0xFFFF), 0x0180, 0x0000, 0xFFFF, 0xFFFE};
}
std::vector<uint16_t> list_no_pointer() {
    return {0x0180, uint16_t(kQ2 >> 16), 0x0180, uint16_t(kQ2 & 0xFFFF), 0x0058, 0x0104, 0xFFFF, 0xFFFE};
}

// The three lists' writes in the window: with the real list a blit lands at
// Q2 and none at Q; without BLTSIZE neither; without the pointer MOVEs the
// blit lands at the CPU's own destination Q.
struct Seen { size_t q2 = 0, q = 0; };
bool main_cmp(const Seen& s) { return s.q2 >= 1 && s.q == 0; }
bool nobltsize_cmp(const Seen& s) { return s.q2 == 0 && s.q == 0; }
bool noptr_cmp(const Seen& s) { return s.q >= 1 && s.q2 == 0; }

struct Eval {
    bool have = false;
    Seen seen;
    bool g1 = false, g2 = false, g3 = false, g4 = false, g5 = false, lst = false;
};

Eval evaluate(const Capture& cap) {
    Eval e;
    auto w = find_window(cap.log, kO, kC);
    e.have = cap.loaded && cap.done.has_value() && w.has_value();
    if (!w) return e;
    e.seen.q2 = writes_in_range(cap.log, kQ2, kQ2 + 0x20, w->lo, w->hi);
    e.seen.q = writes_in_range(cap.log, kQ, kQ + 0x20, w->lo, w->hi);
    e.g1 = e.have && marker(cap.log, kEnd).has_value();
    auto r = stored_at(cap.log, kR, 1);
    bool stored = r.size() == 1;
    e.g2 = stored && (r[0] & 0x0200) && (r[0] & 0x0080) && (r[0] & 0x0040);
    e.g5 = stored && (r[0] & 0x01FF) == 0x00C0;
    e.g4 = count_window(vsync_starts(cap.video), w->lo, w->hi) >= 3;
    // G3: no CPU write falls into L during the window (Q2 is filled exactly
    // to its own $20-byte reservation by the blit itself, so it has no
    // headroom to check separately).
    e.g3 = writes_in_range(cap.log, kList, kList + 64, w->lo, w->hi) == 0;
    e.lst = true;
    for (uint32_t off = 0; off < 12; off += 2)
        if (reads_at(cap.log, kList + off, w->lo, w->hi) < 1) { e.lst = false; break; }
    return e;
}

void run(Checks& checks, const std::string& dir, const std::string& mv, uint64_t run_budget) {
    nmh::Config cfg_ocs; cfg_ocs.chipset_config = 0x00;
    nmh::Config cfg_ecs; cfg_ecs.chipset_config = 0x08;
    const struct { const char* name; nmh::Config cfg; } cfgs[2] = {{"OCS", cfg_ocs}, {"ECS", cfg_ecs}};

    std::string mutant;
    if (mv == "G1") mutant = "/prog_p11_g1.bin";
    else if (mv == "G2") mutant = "/prog_p11_g2.bin";
    else if (mv == "G3") mutant = "/prog_p11_g3.bin";
    else if (mv == "G4") mutant = "/prog_p11_g4.bin";
    else if (mv == "G5") mutant = "/prog_p11_g5.bin";
    else if (mv == "G_NOLIST") mutant = "/prog_p11_g_nolist.bin";

    Eval ev_main[2], ev_nobltsize[2], ev_noptr[2];
    for (int i = 0; i < 2; ++i) {
        std::string bin = dir + "/prog_p11.bin";
        std::string bin_main = mutant.empty() ? bin : dir + mutant;
        ev_main[i] = evaluate(run_capture(bin_main, run_budget, cfgs[i].cfg, {{kList, list_main()}}));
        ev_nobltsize[i] = evaluate(run_capture(bin, run_budget, cfgs[i].cfg, {{kList, list_no_bltsize()}}));
        ev_noptr[i] = evaluate(run_capture(bin, run_budget, cfgs[i].cfg, {{kList, list_no_pointer()}}));
        checks.note(std::string("P11 ") + cfgs[i].name + ": q2 writes " + num((long long)ev_main[i].seen.q2) +
                    " q writes " + num((long long)ev_main[i].seen.q) + " (real list); " +
                    num((long long)ev_nobltsize[i].seen.q2) + "/" + num((long long)ev_nobltsize[i].seen.q) +
                    " (no BLTSIZE); " + num((long long)ev_noptr[i].seen.q2) + "/" +
                    num((long long)ev_noptr[i].seen.q) + " (no pointer)");
    }

    // NOBLTSIZE and NOPTR put the contrasting lists through the main
    // comparator; INVNOBLTSIZE and INVNOPTR put the real list through the
    // contrasting comparators. No comparator changes.
    bool m_ok = true, nb_ok = true, np_ok = true;
    for (int i = 0; i < 2; ++i) {
        const Eval& m_in = (mv == "NOBLTSIZE") ? ev_nobltsize[i] : (mv == "NOPTR") ? ev_noptr[i] : ev_main[i];
        const Eval& nb_in = (mv == "INVNOBLTSIZE") ? ev_main[i] : ev_nobltsize[i];
        const Eval& np_in = (mv == "INVNOPTR") ? ev_main[i] : ev_noptr[i];
        m_ok = m_ok && m_in.have && main_cmp(m_in.seen);
        nb_ok = nb_ok && nb_in.have && nobltsize_cmp(nb_in.seen);
        np_ok = np_ok && np_in.have && noptr_cmp(np_in.seen);
    }
    checks.expect(m_ok,
        "P11 [p.310, p.26, p.287, p.21]: under OCS and ECS, at least one write in [Q2,Q2+$20) and none in "
        "[Q,Q+$20) in the window");
    checks.expect(nb_ok,
        "P11-NOBLTSIZE [p.186, p.269]: with the BLTSIZE MOVE replaced by a MOVE to COLOR00, under OCS and ECS, no write "
        "in [Q2,Q2+$20) and none in [Q,Q+$20) in the window");
    checks.expect(np_ok,
        "P11-NOPTR [p.186, p.269]: with the two pointer MOVEs replaced by MOVEs to COLOR00, under OCS and ECS, at least "
        "one write in [Q,Q+$20), the CPU's own destination, and none in [Q2,Q2+$20) in the window");

    // Preconditions, over all six runs.
    auto all = [&](bool Eval::*member) {
        for (int i = 0; i < 2; ++i)
            if (!(ev_main[i].*member) || !(ev_nobltsize[i].*member) || !(ev_noptr[i].*member)) return false;
        return true;
    };
    checks.expect(all(&Eval::g1), "P11-G1: the program finished -- END was written (every run)");
    checks.expect(all(&Eval::g2),
        "P11-G2 [p.144]: the stored DMACONR readback shows DMAEN, COPEN and BLTEN set (every run)");
    checks.expect(all(&Eval::g4), "P11-G4: V(W) >= 3 (every run)");
    checks.expect(all(&Eval::g3), "P11-G3: zero writes observed in [L,L+64) in the window (every run)");
    checks.expect(all(&Eval::g5),
        "P11-G5: of DMACON bits 8-0 in the stored readback, only COPEN and BLTEN are set (every run)");
    checks.expect(all(&Eval::lst),
        "P11-G(list-ran): the list's first 12 bytes were read at least once inside the window (every run)");
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
