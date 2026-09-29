// test_ch4_sprite_registers.cpp
//
// ch4-sprite-registers: see SPECIFICATION.md.
//
// Five independent 68000 programs (MANUAL, AUTO/AUTO-ODD, CPU-PTR, JUMP,
// BLIT), each in its own NanomigHarness instance, grade P4-P11. With
// NMH_MUTATE unset, every baseline binary runs and every proposition,
// precondition and fixture check prints. With NMH_MUTATE set to one of the
// tags below, only the one relevant binary runs and only the check(s) it
// targets print.
//
// NMH_MUTATE tags -> program loaded (see the individual prog_*.s headers
// for exactly what each changes):
//   P4-READ, P4-WRITE, P4-PRE1, P4-PRE2, P5-PROP, P5-REF, P5-PRE1, P5-PRE2,
//   FIXTURE-MANUAL                              -> prog_manual_*.bin
//   P6-PROP, P6-REF, P6-PRE1, P6-PRE2, P7-PROP, FIXTURE-AUTO -> prog_auto_*.bin
//   P8-PROP (prog_autoodd_c1), P8-PRE2 (plain prog_auto.bin, precondition
//   comparator applied to the even-pointer run)  -> AUTO-ODD family
//   P9-PROP, P9-PRE1, P9-PRE2, P9-PRE3, FIXTURE-CPUPTR -> prog_cpuptr_*.bin
//   P10-PROP, P10-PRE1, P10-PRE2, P10-PRE3, FIXTURE-JUMP -> prog_jump_*.bin
//   P11-PROP, P11-PRE1, P11-PRE3, FIXTURE-BLIT   -> prog_blit_*.bin
//
// Every line below was run on a forced `make -k -B test` on both cores;
// fx68k and TG68K printed identical PASS/FAIL verdicts throughout.
//
// MUTATION P4 stimulus red NMH_MUTATE=P4-READ: reads a mirror word written just before the read instead of the register -- fx68k/TG68K: FAIL via the read-half clause (RESULT+4=0060 RESULT+8=0061 RESULT+6=0001 RESULT+10=0000, tracking what was written)
// MUTATION P4 stimulus red NMH_MUTATE=P4-WRITE: omits field B's SPR0POS write -- fx68k/TG68K: FAIL via the write-half clause (field B's sprite-0 run does not appear, d=-1)
// MUTATION P4-PRE1 stimulus red NMH_MUTATE=P4-PRE1: omits the DECOY reads -- fx68k/TG68K: FAIL (0 DECOY reads observed)
// MUTATION P4-PRE2 stimulus red NMH_MUTATE=P4-PRE2: reads at V7-0=$0080 (inside the display window) instead of $00F8 -- fx68k/TG68K: FAIL (measured V(RA)=V(RB)=0080)
// MUTATION P5 stimulus red NMH_MUTATE=P5-PROP: field B SPR0CTL=$0001 instead of $0000 -- fx68k/TG68K: FAIL (measured dB=70, expected 68)
// MUTATION P5 stimulus red NMH_MUTATE=P5-REF: omits the SPR4DATA write -- fx68k/TG68K: FAIL (no sprite-4 run in either field, dA=dB=-1)
// MUTATION P5-PRE1 stimulus red NMH_MUTATE=P5-PRE1: DMACON=$83A0 (SPREN set) -- fx68k/TG68K: FAIL (DMACONR bit 5=1)
// MUTATION P5-PRE2 stimulus red NMH_MUTATE=P5-PRE2: delays field B's writes until V7-0>=$0080 -- fx68k/TG68K: FAIL (measured V(MB)=0080, not <$2C)
// MUTATION FIXTURE-MANUAL stimulus red NMH_MUTATE=FIXTURE-MANUAL: $DEAD stored instead of $BEEF -- fx68k/TG68K: FAIL
// MUTATION P6 stimulus red NMH_MUTATE=P6-PROP: S1 usage 2's CTL=$5E01 -- fx68k/TG68K: FAIL (measured d2=70, expected 68)
// MUTATION P6 stimulus red NMH_MUTATE=P6-REF: S4's CTL=$6700 (VSTOP $67), 31 pairs -- fx68k/TG68K: FAIL (measured h4=31, expected 32)
// MUTATION P6-PRE1 stimulus red NMH_MUTATE=P6-PRE1: DMACON=$8380 (SPREN clear) -- fx68k/TG68K: FAIL (DMACONR bit 5=0)
// MUTATION P6-PRE2 stimulus red NMH_MUTATE=P6-PRE2: a WAIT for line $60 inserted before the SPR0PT MOVEs -- fx68k/TG68K: FAIL
// MUTATION P7 stimulus red NMH_MUTATE=P7-PROP: the Copper's SPR0PT MOVEs target S2_DATA instead of S1_DATA -- fx68k/TG68K: FAIL (no reads at $25000-$25032)
// MUTATION FIXTURE-AUTO stimulus red NMH_MUTATE=FIXTURE-AUTO: $DEAD stored instead of $BEEF -- fx68k/TG68K: FAIL
// MUTATION P8 stimulus red NMH_MUTATE=P8-PROP: the Copper's SPR0PTL MOVE carries $5002 instead of $5001 -- fx68k/TG68K: FAIL (first read at $25002, not $25000)
// MUTATION P8-PRE2 stimulus red NMH_MUTATE=P8-PRE2: plain AUTO's own log (SPR0PTL=$5000, not the odd $5001) checked against the fixed "$5001" expectation -- fx68k/TG68K: FAIL
// MUTATION P9 stimulus red NMH_MUTATE=P9-PROP: MOVE.W #$0002,SPR0PTH alone instead of the long-word write, leaving SPR0PTL at its decoy $7000 -- fx68k/TG68K: FAIL (reads at S2 instead of S1, measured d=130)
// MUTATION P9-PRE1 stimulus red NMH_MUTATE=P9-PRE1: waits until V7-0=$0060 instead of a VERTB before each field's pointer write -- fx68k/TG68K: FAIL
// MUTATION P9-PRE2 stimulus red NMH_MUTATE=P9-PRE2: DMACON=$8380 (SPREN clear, as P6's precondition 1) -- fx68k/TG68K: FAIL (DMACONR bit 5=0)
// MUTATION P9-PRE3 stimulus red NMH_MUTATE=P9-PRE3: SPR4PT=NULL in the Copper list -- fx68k/TG68K: FAIL (no sprite-4 run in any field)
// MUTATION FIXTURE-CPUPTR stimulus red NMH_MUTATE=FIXTURE-CPUPTR: $DEAD stored instead of $BEEF -- fx68k/TG68K: FAIL
// MUTATION P10 stimulus red NMH_MUTATE=P10-PROP: field 4 writes COLOR01 (not a strobe) instead of COPJMP2 -- fx68k/TG68K: FAIL (field 4 fetches L1's MOVE, not L2)
// MUTATION P10-PRE1 stimulus red NMH_MUTATE=P10-PRE1: DMACON without COPEN -- fx68k/TG68K: FAIL (0 reads of $20000 at all)
// MUTATION P10-PRE2 stimulus red NMH_MUTATE=P10-PRE2: omits field 2's marker write -- fx68k/TG68K: FAIL (M_2 missing)
// MUTATION P10-PRE3 stimulus red NMH_MUTATE=P10-PRE3: waits until V7-0=$F4 (instead of $80) before every marker -- fx68k/TG68K: FAIL (markers fall after the $20004 read, not before)
// MUTATION FIXTURE-JUMP stimulus red NMH_MUTATE=FIXTURE-JUMP: $DEAD stored instead of $BEEF -- fx68k/TG68K: FAIL
// MUTATION P11 stimulus red NMH_MUTATE=P11-PROP: BLTCON0=$08F0 (USED cleared) -- fx68k/TG68K: FAIL (0 writes into DST1)
// MUTATION P11-PRE1 stimulus red NMH_MUTATE=P11-PRE1: DMACON without BLTEN -- fx68k/TG68K: FAIL (no A reads; same mutation also fails P11-PRE2 below)
// MUTATION P11-PRE2 stimulus red NMH_MUTATE=P11-PRE1: the same DMACON-without-BLTEN change -- fx68k/TG68K: FAIL (DMACONR bit 6=0)
// MUTATION P11-PRE3 stimulus red NMH_MUTATE=P11-PRE3: an extra CPU write to Chip memory ($30300) inside the M1-M2 window -- fx68k/TG68K: FAIL (window flagged not quiet)
// MUTATION FIXTURE-BLIT stimulus red NMH_MUTATE=FIXTURE-BLIT: $DEAD stored instead of $BEEF -- fx68k/TG68K: FAIL

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <array>
#include <optional>
#include <algorithm>
#include <memory>
#include <iostream>

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

// ---- constants mirroring nanomig.i / sprreg.i --------------------------
constexpr uint32_t kResults   = 0x0C0000;
constexpr uint32_t kMark      = kResults + 0x0100;
constexpr uint32_t kDecoy     = kResults + 0x0200;
constexpr uint16_t kDecoyVal  = 0xA5A5;
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;

constexpr uint64_t kBootBudget   = 4'000'000;
constexpr uint64_t kChunk        = 200'000;
constexpr uint64_t kMaxAfterBoot = 24'000'000;

std::string hex4(uint16_t v) { char b[8]; std::snprintf(b, sizeof b, "%04X", v); return b; }
std::string hex6(uint32_t v) { char b[10]; std::snprintf(b, sizeof b, "%06X", v & 0xFFFFFF); return b; }

// ---- running a program to completion ------------------------------------

struct RunOutcome {
    std::unique_ptr<nmh::NanomigHarness> h;
    size_t loaded = 0;
    bool released = false;
    bool done_seen = false;
    uint64_t done_cycle = 0;
};

RunOutcome run_program(const nmh::Config& cfg, const std::string& bin, uint64_t settle_after_done = 0) {
    RunOutcome r;
    r.h = std::make_unique<nmh::NanomigHarness>(cfg);
    r.h->poke_word(kDecoy / 2, kDecoyVal);
    r.loaded = r.h->load_program(bin);
    if (!r.loaded) return r;
    auto boot = r.h->boot(kBootBudget);
    r.released = boot.cpu_released;
    if (!r.released) return r;
    auto d = r.h->run_until_write(kDoneAddr, kDoneValue, kMaxAfterBoot);
    if (d) {
        r.done_seen = true; r.done_cycle = *d;
        // Some checks (e.g. JUMP's P10-PRE3) need a port event that only
        // occurs later in the field the program's own last marker belongs
        // to (the Copper's WAIT for line $F0 completing), well after the
        // program itself has nothing left to do and reaches DONE. Run a
        // little further so that trailing event is captured too.
        if (settle_after_done) r.h->run_cycles(settle_after_done);
    }
    return r;
}

// ---- RAM-log helpers ------------------------------------------------------

std::optional<uint64_t> find_write(const std::vector<nmh::RamAccess>& log, uint32_t word_addr, uint16_t data) {
    for (auto& a : log) if (a.is_write && a.word_addr == word_addr && a.data == data) return a.cycle;
    return std::nullopt;
}

std::optional<uint64_t> find_write_any(const std::vector<nmh::RamAccess>& log, uint32_t word_addr) {
    for (auto& a : log) if (a.is_write && a.word_addr == word_addr) return a.cycle;
    return std::nullopt;
}

std::optional<uint64_t> find_read(const std::vector<nmh::RamAccess>& log, uint32_t word_addr, uint64_t after_cycle) {
    for (auto& a : log) if (!a.is_write && a.word_addr == word_addr && a.cycle >= after_cycle) return a.cycle;
    return std::nullopt;
}

std::vector<uint64_t> find_reads(const std::vector<nmh::RamAccess>& log, uint32_t word_addr) {
    std::vector<uint64_t> out;
    for (auto& a : log) if (!a.is_write && a.word_addr == word_addr) out.push_back(a.cycle);
    return out;
}

// Within [lo,hi), checks that reads at word addresses [base,base+count)
// occur, each exactly once, in strictly ascending address order (SPECIFICATION.md
// P7/P8's own comparator: "the first N reads ... are at base, base+2, ...,
// in ascending order, each once").
bool check_full_traversal(const std::vector<nmh::RamAccess>& log, uint32_t base, int count,
                           uint64_t lo, uint64_t hi, uint32_t* first_addr_out = nullptr) {
    std::vector<const nmh::RamAccess*> hits;
    for (auto& a : log)
        if (!a.is_write && a.cycle >= lo && a.cycle < hi && a.word_addr >= base && a.word_addr < base + (uint32_t)count)
            hits.push_back(&a);
    std::sort(hits.begin(), hits.end(), [](const nmh::RamAccess* x, const nmh::RamAccess* y) { return x->cycle < y->cycle; });
    if (hits.empty()) return false;
    if (first_addr_out) *first_addr_out = hits.front()->word_addr;
    int expect_next = 0;
    for (auto* h : hits) {
        int off = (int)(h->word_addr - base);
        if (off != expect_next) return false;
        ++expect_next;
    }
    return expect_next == count;
}

// ---- p.194's per-line refresh anchors (technique from
// ch6-blitter-cpu-sharing: refresh reads are RAM-port reads below word
// address $1000 -- far below every chip-RAM address this package uses
// ($10000 and up) -- 4 reads per line, 2 slots (16 clk_sys) apart, grouped
// by a gap over 100 clk_sys starting a new line. SPECIFICATION.md R5: "the
// number of scan lines between two times is the number of refresh groups
// that begin between them." ------------------------------------------------

constexpr uint64_t kCck = 8;

struct LineRef { std::vector<uint64_t> firsts; };

LineRef make_line_ref(const std::vector<nmh::RamAccess>& log) {
    LineRef ref;
    std::vector<uint64_t> low_reads;
    for (const auto& a : log) if (!a.is_write && a.word_addr < 0x1000) low_reads.push_back(a.cycle);
    if (low_reads.empty()) return ref;
    std::sort(low_reads.begin(), low_reads.end());
    std::vector<std::vector<uint64_t>> groups;
    groups.push_back({low_reads[0]});
    for (size_t i = 1; i < low_reads.size(); ++i) {
        if (low_reads[i] - low_reads[i - 1] > 100) groups.push_back({});
        groups.back().push_back(low_reads[i]);
    }
    for (const auto& g : groups) {
        if (g.size() != 4) continue;
        bool shape = true;
        for (size_t i = 1; i < g.size(); ++i)
            if (g[i] - g[i - 1] != 2 * kCck) { shape = false; break; }
        if (shape) ref.firsts.push_back(g.front());
    }
    return ref;
}

// R5: "the number of scan lines between two times is the number of refresh
// groups that begin between them."
long scanlines_between(const LineRef& ref, uint64_t lo, uint64_t hi) {
    if (hi <= lo) return 0;
    auto a = std::lower_bound(ref.firsts.begin(), ref.firsts.end(), lo);
    auto b = std::lower_bound(ref.firsts.begin(), ref.firsts.end(), hi);
    return (long)(b - a);
}

// ---- video colour classification (SPECIFICATION.md 3.6's plain set:
// COLOR17=red, COLOR18=green, COLOR19=blue -- sprite 0; COLOR25-27=white --
// sprite 4/5; all else black) -----------------------------------------------

enum class Px { Bg, White, Red, Green, Blue };

Px classify(const nmh::VideoSample& s) {
    bool r = s.r != 0, g = s.g != 0, b = s.b != 0;
    if (r && g && b) return Px::White;
    if (r && !g && !b) return Px::Red;
    if (!r && g && !b) return Px::Green;
    if (!r && !g && b) return Px::Blue;
    return Px::Bg;
}

bool is_sprite0(Px p) { return p == Px::Red || p == Px::Green || p == Px::Blue; }

// ---- per-line e_A (white run)/e_B (sprite-0 run) edges (SPECIFICATION.md
// R4) and the 24-pixel colour check from e_B (P5/P6 share this exact
// derivation) -----------------------------------------------------------

struct LineEdges { bool white_found = false, sprite0_found = false; long e_a = -1, e_b = -1; };

LineEdges find_edges(const std::vector<nmh::VideoSample>& video, nmh::Span line) {
    LineEdges e;
    for (size_t i = line.begin; i < line.end; ++i) {
        if (classify(video[i]) == Px::White) { e.e_a = (long)(i - line.begin); e.white_found = true; break; }
    }
    if (!e.white_found) return e;
    // end of the white run
    size_t i = line.begin + (size_t)e.e_a;
    while (i < line.end && classify(video[i]) == Px::White) ++i;
    for (; i < line.end; ++i) {
        if (is_sprite0(classify(video[i]))) { e.e_b = (long)(i - line.begin); e.sprite0_found = true; break; }
    }
    return e;
}

// checks the 24 output pixels from e_B against three 8-pixel colour bands.
bool check_24(const std::vector<nmh::VideoSample>& video, nmh::Span line, long e_b, Px c0, Px c1, Px c2) {
    auto band_ok = [&](long off, Px want) {
        for (long k = 0; k < 8; ++k) {
            size_t idx = line.begin + (size_t)(e_b + off + k);
            if (idx >= line.end || idx >= video.size()) return false;
            if (classify(video[idx]) != want) return false;
        }
        return true;
    };
    return band_ok(0, c0) && band_ok(8, c1) && band_ok(16, c2);
}

} // namespace

// ============================================================================
// MANUAL: P4, P5
// ============================================================================
namespace manual {

constexpr uint32_t kMarkMA = kMark / 2 + 0, kMarkRA = kMark / 2 + 1, kMarkMB = kMark / 2 + 2, kMarkRB = kMark / 2 + 3;
constexpr uint32_t kR = kResults / 2;

std::string bin_for(const std::string& mv) {
    if (mv == "P4-READ")  return "prog_manual_p4read.bin";
    if (mv == "P4-WRITE") return "prog_manual_p4write.bin";
    if (mv == "P4-PRE1")  return "prog_manual_p4pre1.bin";
    if (mv == "P4-PRE2")  return "prog_manual_p4pre2.bin";
    if (mv == "P5-PROP")  return "prog_manual_p5prop.bin";
    if (mv == "P5-REF")   return "prog_manual_p5ref.bin";
    if (mv == "P5-PRE1")  return "prog_manual_p5pre1.bin";
    if (mv == "P5-PRE2")  return "prog_manual_p5pre2.bin";
    if (mv == "FIXTURE-MANUAL") return "prog_manual_fixture.bin";
    return "prog_manual.bin";
}

void run(const std::string& dir, const std::string& mv, Checks& c) {
    nmh::Config cfg; // ECS PAL, default
    auto r = run_program(cfg, dir + "/" + bin_for(mv));
    bool run_ok = r.loaded > 0 && r.released && r.done_seen;

    if (mv.empty())
        c.expect(run_ok, "RUN-MANUAL: prog_manual.bin loaded (" + std::to_string(r.loaded) +
                 " bytes), CPU released from reset, DONE reached" +
                 (r.done_seen ? (" at cycle " + std::to_string(r.done_cycle)) : " -- not seen"));
    if (!run_ok) { if (!mv.empty()) c.expect(false, mv + ": run did not complete, nothing graded"); return; }

    const auto& log = r.h->ram_log();
    const auto& video = r.h->video();

    auto fixture = r.h->peek_logical(kResults + 0);
    if (mv.empty() || mv == "FIXTURE-MANUAL")
        c.expect(fixture.has_value() && *fixture == 0xBEEF,
                 "FIXTURE-MANUAL: prog stores a known constant (BEEF) at RESULTS before anything else is trusted"
                 " (read back " + (fixture ? hex4(*fixture) : std::string("nothing")) + ")");

    auto dmaconr = r.h->peek_logical(kResults + 2);

    // ---- markers -----------------------------------------------------
    auto ma = find_write(log, kMarkMA, 1);
    auto ra = find_write(log, kMarkRA, 2);
    auto mb = find_write(log, kMarkMB, 3);
    auto rb = find_write(log, kMarkRB, 4);
    bool markers_ok = ma && ra && mb && rb && *ma < *ra && *ra < *mb && *mb < *rb;

    // ---- P5 precondition 1: DMACONR bit5=0, bit9=1 --------------------
    if (mv.empty() || mv == "P5-PRE1") {
        bool ok = dmaconr.has_value() && ((*dmaconr >> 5) & 1) == 0 && ((*dmaconr >> 9) & 1) == 1;
        c.expect(ok, "P5-PRE1: RESULT+2 (DMACONR) has bit 5=0 and bit 9=1, so no sprite DMA can have loaded "
                 "SPR0POS/SPR0CTL/SPR0DATA/SPR0DATB (DMACONR=" + (dmaconr ? hex4(*dmaconr) : std::string("--")) + ")");
    }

    // ---- P5 precondition 2 / P4 precondition 2: markers MA/MB precede the
    // display window ($2C), RA/RB fall after it ($F8), measured from the V
    // the program itself read at each marker (VMARK, RESULTS+12/14/16/18,
    // SPECIFICATION.md 3.8's own VPOSR/VHPOSR technique). ------------------
    auto vma = r.h->peek_logical(kResults + 12);
    auto vra = r.h->peek_logical(kResults + 14);
    auto vmb = r.h->peek_logical(kResults + 16);
    auto vrb = r.h->peek_logical(kResults + 18);
    if (mv.empty() || mv == "P5-PRE2") {
        bool ok = vma && vmb && *vma < 0x2C && *vmb < 0x2C;
        c.expect(ok, "P5-PRE2: markers MA and MB precede the display window of their field (V<$2C at each; "
                 "measured V(MA)=" + (vma ? hex4(*vma) : std::string("--")) +
                 " V(MB)=" + (vmb ? hex4(*vmb) : std::string("--")) + ")");
    }
    if (mv.empty() || mv == "P4-PRE2") {
        bool ok = vra && vrb && *vra >= 0xF8 && *vrb >= 0xF8;
        c.expect(ok, "P4-PRE2: the reads fall after the last display-window line of their field and before the "
                 "next vertical blank (V>=$F8 at each; measured V(RA)=" + (vra ? hex4(*vra) : std::string("--")) +
                 " V(RB)=" + (vrb ? hex4(*vrb) : std::string("--")) + ")");
    }

    // ---- P4 precondition 1: a DECOY read appears between each store and
    // the next, at the port (ram_log; DECOY is chip RAM). -------------------
    if (mv.empty() || mv == "P4-PRE1") {
        uint32_t kW = kDecoy / 2, kR4 = kR + 2, kR6 = kR + 3, kR8 = kR + 4, kR10 = kR + 5;
        auto decoy_reads = find_reads(log, kW);
        bool ok = markers_ok && decoy_reads.size() >= 4;
        if (ok) {
            auto r4 = find_write_any(log, kR4), r6 = find_write_any(log, kR6);
            auto r8 = find_write_any(log, kR8), r10 = find_write_any(log, kR10);
            ok = r4 && r6 && r8 && r10;
            if (ok) {
                // one decoy read between MA and R4, one between R4-store and R6-store,
                // one between MB and R8, one between R8-store and R10-store
                auto between = [&](uint64_t lo, uint64_t hi) {
                    for (auto d : decoy_reads) if (d > lo && d < hi) return true;
                    return false;
                };
                ok = between(*ma, *r4) && between(*r4, *r6) && between(*mb, *r8) && between(*r8, *r10);
            }
        }
        c.expect(ok, "P4-PRE1: at the port, a CPU read of DECOY appears after the store that precedes each "
                 "register read (a RESULT store or a marker) and before the RESULT store that follows it (" +
                 std::to_string(decoy_reads.size()) + " DECOY reads observed)");
    }

    if (!markers_ok) {
        if (mv.empty() || mv == "P4-WRITE" || mv == "P5-PROP" || mv == "P5-REF")
            c.expect(false, (mv.empty() ? "P4/P5" : mv) + ": markers MA/RA/MB/RB not observed in order, nothing graded");
        return;
    }

    // ---- P5 (CH4-data.c1) and P4 (CH4-appC-position.c10 CH4-appC-position.c15) ----
    auto r4 = r.h->peek_logical(kResults + 4), r6 = r.h->peek_logical(kResults + 6);
    auto r8 = r.h->peek_logical(kResults + 8), r10 = r.h->peek_logical(kResults + 10);

    // field A / field B sprite-4 (white) + sprite-0 edge/colour check, over
    // every output line in [MA,RA) / [MB,RB) that shows the white run.
    auto grade_field = [&](uint64_t lo, uint64_t hi, Px c0, Px c1, Px c2, long& d_out, long& n_lines) -> bool {
        std::vector<nmh::VideoSample> slice(video.begin() + (long)lo, video.begin() + (long)std::min(hi, (uint64_t)video.size()));
        auto lines = nmh::find_lines(slice);
        bool ok = true;
        n_lines = 0;
        d_out = -1;
        for (auto& ln : lines) {
            LineEdges e = find_edges(slice, ln);
            if (!e.white_found) continue; // not a sprite-4 line
            ++n_lines;
            if (!e.sprite0_found) { ok = false; continue; }
            long d = e.e_b - e.e_a;
            if (d_out < 0) d_out = d; else if (d != d_out) ok = false;
            if (!check_24(slice, ln, e.e_b, c0, c1, c2)) ok = false;
        }
        return ok && n_lines > 0;
    };

    long dA = -1, nA = 0, dB = -1, nB = 0;
    bool fieldA_ok = grade_field(*ma, *ra, Px::Blue, Px::Red, Px::Green, dA, nA);
    bool fieldB_ok = grade_field(*mb, *rb, Px::Blue, Px::Green, Px::Red, dB, nB);

    c.note("MANUAL field A: " + std::to_string(nA) + " sprite-4 lines, d=" + std::to_string(dA) +
           "; field B: " + std::to_string(nB) + " sprite-4 lines, d=" + std::to_string(dB));

    if (mv.empty() || mv == "P5-PROP" || mv == "P5-REF") {
        bool p5 = fieldA_ok && dA == 66 && fieldB_ok && dB == 68;
        c.expect(p5, "P5 (CH4-data.c1): the 68000 can load each of SPR0POS/SPR0CTL/SPR0DATA/SPR0DATB manually "
                 "(p.123,125,129) -- field A d=66 (COLOR19,COLOR17,COLOR18 from e_B), field B d=68 "
                 "(COLOR19,COLOR18,COLOR17), on every display-window line showing the sprite-4 reference"
                 " (measured dA=" + std::to_string(dA) + " nA=" + std::to_string(nA) +
                 " dB=" + std::to_string(dB) + " nB=" + std::to_string(nB) + ")");
    }

    if (mv.empty() || mv == "P4-WRITE" || mv == "P4-READ") {
        bool p4_write = fieldA_ok && dA == 66 && fieldB_ok && dB == 68;
        // SPECIFICATION.md P4 Expected, read half, unchanged across the
        // baseline and MUT_P4READ (which reads a mirror word that DOES
        // track what was written, so this same formula goes red on it).
        bool p4_read_ok = !(r4 && r8 && *r4 == 0x0060 && *r8 == 0x0061) && !(r6 && r10 && *r6 == 0x0001 && *r10 == 0x0000);
        bool p4 = p4_write && p4_read_ok;
        c.expect(p4, "P4 (CH4-appC-position.c10 CH4-appC-position.c15): on ECS, a 68000 write to SPR0POS/SPR0CTL "
                 "takes effect (write half, same d=66/68 evidence as P5) and a 68000 read of the same address "
                 "does not return what was written (p.303, p.287, p.299) (RESULT+4=" +
                 (r4 ? hex4(*r4) : std::string("--")) + " RESULT+6=" + (r6 ? hex4(*r6) : std::string("--")) +
                 " RESULT+8=" + (r8 ? hex4(*r8) : std::string("--")) + " RESULT+10=" + (r10 ? hex4(*r10) : std::string("--")) + ")");
    }
}

} // namespace manual

// ============================================================================
// AUTO / AUTO-ODD: P6, P7, P8
// ============================================================================
namespace autorun {

constexpr uint32_t kMarkM1 = kMark / 2 + 0, kMarkM2 = kMark / 2 + 1;
constexpr uint32_t kCopWord0 = 0x020000 / 2; // COPLIST_L1's own first word (field anchor, R5 2.7)
constexpr uint32_t kSpr0PtlDataBase = 0x020000 / 2 + 7;   // base layout (see prog_auto.s)
constexpr uint32_t kSpr0PtlDataPre2 = 0x020000 / 2 + 9;   // MUT_P6PRE2 layout (WAIT inserted)
constexpr uint32_t kS1Base = 0x025000 / 2, kS2Base = 0x027000 / 2, kS4Base = 0x028000 / 2;

std::string bin_for(const std::string& mv) {
    if (mv == "P6-PROP") return "prog_auto_p6prop.bin";
    if (mv == "P6-REF")  return "prog_auto_p6ref.bin";
    if (mv == "P6-PRE1") return "prog_auto_p6pre1.bin";
    if (mv == "P6-PRE2") return "prog_auto_p6pre2.bin";
    if (mv == "P7-PROP") return "prog_auto_p7prop.bin";
    if (mv == "FIXTURE-AUTO") return "prog_auto_fixture.bin";
    if (mv == "P8-PROP") return "prog_autoodd_c1.bin";
    if (mv == "P8-PRE2") return "prog_auto.bin"; // precondition comparator applied to the even ($5000) run
    return "prog_auto.bin";
}

// Finds every complete field: [fetch_i, fetch_{i+1}) for consecutive reads
// of the Copper's own SPR0PTL data word (or, absent that -- MUT_P9PRE3/P7PROP
// leave the read address the same, only its data differs -- consecutive
// reads of L1's own first word, R5 2.7, always present every field).
std::vector<std::pair<uint64_t,uint64_t>> field_intervals(const std::vector<nmh::RamAccess>& log,
                                                            uint64_t lo, uint64_t hi) {
    auto fetches = find_reads(log, kCopWord0);
    std::sort(fetches.begin(), fetches.end());
    std::vector<std::pair<uint64_t,uint64_t>> out;
    for (size_t i = 0; i + 1 < fetches.size(); ++i) {
        uint64_t a = fetches[i], b = fetches[i + 1];
        if (a >= lo && b <= hi) out.push_back({a, b});
    }
    return out;
}

void run(const std::string& dir, const std::string& mv, Checks& c) {
    nmh::Config cfg;
    auto r = run_program(cfg, dir + "/" + bin_for(mv));
    bool run_ok = r.loaded > 0 && r.released && r.done_seen;

    std::string tag = mv.empty() ? "AUTO" : mv;
    if (mv.empty())
        c.expect(run_ok, "RUN-AUTO: prog_auto.bin loaded (" + std::to_string(r.loaded) +
                 " bytes), CPU released from reset, DONE reached" +
                 (r.done_seen ? (" at cycle " + std::to_string(r.done_cycle)) : " -- not seen"));
    if (!run_ok) { if (!mv.empty()) c.expect(false, tag + ": run did not complete, nothing graded"); return; }

    const auto& log = r.h->ram_log();
    const auto& video = r.h->video();

    auto fixture = r.h->peek_logical(kResults + 0);
    if (mv.empty() || mv == "FIXTURE-AUTO")
        c.expect(fixture.has_value() && *fixture == 0xBEEF,
                 "FIXTURE-AUTO: prog stores a known constant (BEEF) at RESULTS before anything else is trusted"
                 " (read back " + (fixture ? hex4(*fixture) : std::string("nothing")) + ")");

    auto dmaconr = r.h->peek_logical(kResults + 2);
    auto m1 = find_write(log, kMarkM1, 1);
    auto m2 = find_write(log, kMarkM2, 2);

    if (mv.empty() || mv == "P6-PRE1")
        c.expect(dmaconr.has_value() && ((*dmaconr >> 9) & 1) == 1 && ((*dmaconr >> 5) & 1) == 1,
                 "P6-PRE1: RESULT+2 (DMACONR) has bits 9 and 5 set (DMAEN, SPREN) (DMACONR=" +
                 (dmaconr ? hex4(*dmaconr) : std::string("--")) + ")");

    if (!m1 || !m2) { c.expect(false, tag + ": M1/M2 not observed, nothing graded"); return; }

    auto fields = field_intervals(log, *m1, *m2);
    c.note(tag + ": " + std::to_string(fields.size()) + " complete Copper fields found between M1 and M2");

    LineRef lref = make_line_ref(log);

    // ---- P7 precondition 2 / P6 precondition 2: the Copper's fetch of the
    // SPR0PTL MOVE's data word precedes the first S1/S2 read in each field.
    uint32_t data_word = (mv == "P6-PRE2") ? kSpr0PtlDataPre2 : kSpr0PtlDataBase;
    auto spr0ptl_fetches = find_reads(log, data_word);
    std::sort(spr0ptl_fetches.begin(), spr0ptl_fetches.end());

    if (mv.empty() || mv == "P6-PRE2") {
        bool ok = !fields.empty();
        for (auto& fi : fields) {
            bool have_fetch = false;
            for (auto f : spr0ptl_fetches) if (f >= fi.first && f < fi.second) { have_fetch = true;
                auto first_s1 = find_read(log, kS1Base, fi.first);
                if (first_s1 && *first_s1 < f) ok = false;
                break; }
            if (!have_fetch) ok = false;
        }
        c.expect(ok, "P6-PRE2: in each graded field, the Copper's fetch of the data word of its SPR0PTL MOVE "
                 "precedes the first port read in $25000-$25032 (p.110)");
    }

    // ---- P6 (CH4-fig4-13.c9 CH4-fig413.f6, jointly with P5): DMA loads
    // SPRxPOS/CTL/DATA/DATB -- sprite 4 (32 scan lines, one column) and
    // sprite 0 (two groups: 4 lines @ +8, 6 lines @ +6... offsets +8/+8). --
    if (mv.empty() || mv == "P6-PROP" || mv == "P6-REF") {
        bool p6 = !fields.empty();
        long last_h4 = -1, last_h1 = -1, last_h2 = -1, last_off1 = -1, last_off2 = -1;
        long last_d1 = -1, last_d2 = -1;
        for (auto& fi : fields) {
            std::vector<nmh::VideoSample> slice(video.begin() + (long)fi.first,
                                                 video.begin() + (long)std::min(fi.second, (uint64_t)video.size()));
            auto lines = nmh::find_lines(slice);
            // classify each output line by what it shows
            std::optional<size_t> first_white, first_not_white_after, first_red_grp1, first_end_grp1, first_red_grp2, first_end_grp2;
            bool d1_ok = true, d2_ok = true;
            long d1 = -1, d2 = -1;
            bool in_white = false, seen_white = false;
            bool in_grp1 = false, seen_grp1 = false, grp1_done = false;
            bool in_grp2 = false, seen_grp2 = false;
            for (size_t li = 0; li < lines.size(); ++li) {
                LineEdges e = find_edges(slice, lines[li]);
                bool white = e.white_found;
                bool sprite0 = e.sprite0_found && !white; // a line can show only sprite0 after sprite4 ends there too; still fine
                if (white) {
                    if (!seen_white) { first_white = li; seen_white = true; }
                    in_white = true;
                } else if (in_white && !first_not_white_after) {
                    first_not_white_after = li;
                    in_white = false;
                }
                if (e.sprite0_found) {
                    long d = e.e_b - e.e_a;
                    if (!grp1_done) {
                        if (!seen_grp1) { first_red_grp1 = li; seen_grp1 = true; d1 = d; }
                        else if (d != d1) d1_ok = false;
                        in_grp1 = true;
                    } else {
                        if (!seen_grp2) { first_red_grp2 = li; seen_grp2 = true; d2 = d; }
                        else if (d != d2) d2_ok = false;
                        in_grp2 = true;
                    }
                    if (!check_24(slice, lines[li], e.e_b,
                                   grp1_done ? Px::Blue : Px::Blue,
                                   grp1_done ? Px::Green : Px::Red,
                                   grp1_done ? Px::Red : Px::Green)) {
                        if (grp1_done) d2_ok = false; else d1_ok = false;
                    }
                } else if (in_grp1) {
                    in_grp1 = false; grp1_done = true; first_end_grp1 = li;
                } else if (in_grp2) {
                    in_grp2 = false; first_end_grp2 = li;
                }
            }
            if (!first_white || !first_not_white_after || !first_red_grp1 || !first_red_grp2) { p6 = false; continue; }
            // first_*/first_end_* hold indices into `lines`; convert to absolute
            // cycles via that output line's own start sample before counting
            // refresh groups between them (R5).
            uint64_t t_white = fi.first + lines[*first_white].begin;
            uint64_t t_white_end = fi.first + lines[*first_not_white_after].begin;
            uint64_t t_grp1 = fi.first + lines[*first_red_grp1].begin;
            uint64_t t_grp2 = fi.first + lines[*first_red_grp2].begin;
            long h4 = scanlines_between(lref, t_white, t_white_end);
            long off1 = scanlines_between(lref, t_white, t_grp1);
            long off2 = scanlines_between(lref, t_grp1, t_grp2);
            long h1 = first_end_grp1 ? scanlines_between(lref, t_grp1, fi.first + lines[*first_end_grp1].begin) : -1;
            long h2 = first_end_grp2 ? scanlines_between(lref, t_grp2, fi.first + lines[*first_end_grp2].begin) : -1;
            // Fixed, unchanged expected values (SPECIFICATION.md P6's own
            // Expected): under MUT_P6REF/MUT_P6PROP the measured h4/d2
            // naturally differ from these (31/70, per the page's own
            // prediction for the changed stimulus) and clash with them here.
            const long exp_h4 = 32, exp_h1 = 4, exp_h2 = 6, exp_off1 = 8, exp_off2 = 8;
            const long exp_d1 = 66, exp_d2 = 68;
            bool field_ok = (h4 == (long)exp_h4) && (h1 == (long)exp_h1) && (h2 == (long)exp_h2) &&
                             (off1 == (long)exp_off1) && (off2 == (long)exp_off2) &&
                             d1_ok && d2_ok && d1 == (long)exp_d1 && d2 == (long)exp_d2;
            if (!field_ok) p6 = false;
            last_h4 = h4; last_h1 = h1; last_h2 = h2; last_off1 = off1; last_off2 = off2; last_d1 = d1; last_d2 = d2;
        }
        bool p6_joint = p6; // P6's own assertion; joined with P5 only in the baseline summary (see main())
        c.expect(p6_joint, "P6 (CH4-fig4-13.c9 CH4-fig413.f6, jointly with P5's 68000-load result): sprite DMA "
                 "loads SPRxPOS/CTL/DATA/DATB (p.108,126,107) -- sprite 4 32 scan lines at one column, sprite-0 "
                 "group 1 (4 lines, +8 after sprite 4, d=66) and group 2 (6 lines, +8 after group 1, d=68)"
                 " (measured h4=" + std::to_string(last_h4) + " h1=" + std::to_string(last_h1) +
                 " h2=" + std::to_string(last_h2) + " off1=" + std::to_string(last_off1) +
                 " off2=" + std::to_string(last_off2) + " d1=" + std::to_string(last_d1) +
                 " d2=" + std::to_string(last_d2) + ")");
    }

    // ---- P7 (CH4-p104.c6 CH4-mem.c5 CH4-p105.c12): sprite data taken from
    // Chip memory at the pointer, in order, every field. ------------------
    if (mv.empty() || mv == "P7-PROP") {
        // Fixed, unchanged comparator (base=kS1Base always, matching the
        // proposition's own claim): under MUT_P7PROP the Copper's SPR0PT
        // MOVEs target S2 instead, so no reads land at S1 any more and
        // this naturally fails through the same check (the page's own
        // prediction is $27000-$27032 instead, not graded here again).
        bool ok = !fields.empty();
        for (auto& fi : fields) if (!check_full_traversal(log, kS1Base, 26, fi.first, fi.second)) ok = false;
        c.expect(ok, "P7 (CH4-p104.c6 CH4-mem.c5 CH4-p105.c12): the sprite DMA channel takes the whole sprite "
                 "data structure from Chip memory, at the pointer, in order (p.104,105) -- 26 reads at $25000.."
                 "$25032 ascending, each once, per field");
    }

    // ---- P8 (CH4-build.c12 CH4-p104.c7): SPRxPTL's LSB is ignored. --------
    if (mv.empty() || mv == "P8-PROP" || mv == "P8-PRE2") {
        // For the baseline family ("mv.empty()") this grades AUTO-ODD's own
        // run (prog_autoodd.bin), not plain AUTO -- run it here explicitly.
        // P8-PROP already loaded prog_autoodd_c1.bin above (bin_for); P8-PRE2
        // reuses this function's own already-run plain AUTO log (SPR0PTL=$5000).
        const std::vector<nmh::RamAccess>* plog = &log;
        // Fixed, unchanged comparator: the data word must read back $5001.
        // Under MUT_P8PRE2 (plain AUTO's own log, SPR0PTL=$5000) this
        // naturally clashes with the same, unchanged expectation.
        constexpr uint32_t want_data = 0x5001;
        std::unique_ptr<RunOutcome> owned;
        std::optional<uint64_t> odd_m1 = m1, odd_m2 = m2;
        if (mv.empty()) {
            owned = std::make_unique<RunOutcome>(run_program(cfg, dir + "/prog_autoodd.bin"));
            if (owned->loaded && owned->released && owned->done_seen) {
                plog = &owned->h->ram_log();
                odd_m1 = find_write(*plog, kMarkM1, 1);
                odd_m2 = find_write(*plog, kMarkM2, 2);
            } else { c.expect(false, "P8: AUTO-ODD run did not complete, nothing graded"); plog = nullptr; }
        }
        if (plog && odd_m1 && odd_m2) {
            auto own_fields = field_intervals(*plog, *odd_m1, *odd_m2);
            if (mv.empty() || mv == "P8-PROP") {
                bool ok = !own_fields.empty();
                for (auto& fi : own_fields) if (!check_full_traversal(*plog, kS1Base, 26, fi.first, fi.second)) ok = false;
                c.expect(ok, "P8 (CH4-build.c12 CH4-p104.c7): the least significant bit of SPRxPTL is ignored -- "
                         "SPR0PTL=$5001 still fetches the structure from $25000, the same 26 reads ascending, "
                         "each once, per field (p.104,110)");
            }
            if (mv.empty() || mv == "P8-PRE2") {
                bool ok = false;
                for (auto& a : *plog) if (!a.is_write && a.word_addr == data_word && a.data == want_data) { ok = true; break; }
                c.expect(ok, "P8-PRE2: the Copper's fetch of the SPR0PTL MOVE's data word returns $" + hex4(want_data) +
                         " at the port, showing that value was the one written");
            }
        }
    }
}

} // namespace autorun

// ============================================================================
// CPU-PTR: P9
// ============================================================================
namespace cpuptr {

constexpr uint32_t kS1Base = 0x025000 / 2, kS2Base = 0x027000 / 2;

std::string bin_for(const std::string& mv) {
    if (mv == "P9-PROP") return "prog_cpuptr_prop.bin";
    if (mv == "P9-PRE1") return "prog_cpuptr_pre1.bin";
    if (mv == "P9-PRE2") return "prog_cpuptr_pre2.bin";
    if (mv == "P9-PRE3") return "prog_cpuptr_pre3.bin";
    if (mv == "FIXTURE-CPUPTR") return "prog_cpuptr_fixture.bin";
    return "prog_cpuptr.bin";
}

void run(const std::string& dir, const std::string& mv, Checks& c) {
    nmh::Config cfg;
    auto r = run_program(cfg, dir + "/" + bin_for(mv));
    bool run_ok = r.loaded > 0 && r.released && r.done_seen;
    std::string tag = mv.empty() ? "CPU-PTR" : mv;

    if (mv.empty())
        c.expect(run_ok, "RUN-CPUPTR: prog_cpuptr.bin loaded (" + std::to_string(r.loaded) +
                 " bytes), CPU released from reset, DONE reached" +
                 (r.done_seen ? (" at cycle " + std::to_string(r.done_cycle)) : " -- not seen"));
    if (!run_ok) { if (!mv.empty()) c.expect(false, tag + ": run did not complete, nothing graded"); return; }

    const auto& log = r.h->ram_log();

    auto fixture = r.h->peek_logical(kResults + 0);
    if (mv.empty() || mv == "FIXTURE-CPUPTR")
        c.expect(fixture.has_value() && *fixture == 0xBEEF,
                 "FIXTURE-CPUPTR: prog stores a known constant (BEEF) at RESULTS before anything else is trusted"
                 " (read back " + (fixture ? hex4(*fixture) : std::string("nothing")) + ")");

    auto dmaconr = r.h->peek_logical(kResults + 2);
    if (mv.empty() || mv == "P9-PRE2")
        c.expect(dmaconr.has_value() && ((*dmaconr >> 9) & 1) == 1 && ((*dmaconr >> 5) & 1) == 1,
                 "P9-PRE2: DMACONR bits 9 and 5 set (as P6's precondition 1) (DMACONR=" +
                 (dmaconr ? hex4(*dmaconr) : std::string("--")) + ")");

    // markers P_1..P_5 at MARK+0,2,4,6,8, each tag 1
    std::vector<std::optional<uint64_t>> p(5);
    for (int i = 0; i < 5; ++i) p[i] = find_write(log, kMark / 2 + i, 1);
    bool markers_ok = true;
    for (int i = 0; i < 5; ++i) if (!p[i]) markers_ok = false;
    for (int i = 1; i < 5 && markers_ok; ++i) if (*p[i] <= *p[i - 1]) markers_ok = false;
    if (!markers_ok) { c.expect(false, tag + ": P_1..P_5 not observed in order, nothing graded"); return; }

    // ---- precondition 1: P_f precedes the first sprite-0 read of its field
    if (mv.empty() || mv == "P9-PRE1") {
        bool ok = true;
        for (int f = 0; f < 4; ++f) {
            auto first_s1 = find_read(log, kS1Base, *p[f]);
            if (!first_s1 || *first_s1 <= *p[f]) ok = false;
        }
        c.expect(ok, "P9-PRE1: in each field, P_f precedes the first sprite-0 read at the port");
    }

    const auto& video = r.h->video();

    // ---- P9 precondition 3: sprite 4 appears (as in P6) on every field.
    // Unchanged comparator: under MUT_P9PRE3 (SPR4PT=NULL) no white run
    // exists, so this same "white found in every field" check goes red.
    if (mv.empty() || mv == "P9-PRE3") {
        bool ok = true;
        for (int f = 0; f + 1 < 5; ++f) {
            uint64_t lo = *p[f], hi = *p[f + 1];
            std::vector<nmh::VideoSample> slice(video.begin() + (long)lo, video.begin() + (long)std::min(hi, (uint64_t)video.size()));
            auto lines = nmh::find_lines(slice);
            bool any_white = false;
            for (auto& ln : lines) if (find_edges(slice, ln).white_found) any_white = true;
            if (!any_white) ok = false;
        }
        c.expect(ok, "P9-PRE3: sprite 4 appears as in P6, on every field of this run");
    }

    // ---- P9 (CH4-p110.c7 CH4-ptr.c10): a 68000 long-word write to SPR0PTH
    // loads both SPR0PTH and SPR0PTL. Fixed, unchanged comparator: reads at
    // kS1Base..+26 ascending, each once, and group 1's d=66; under
    // MUT_P9PROP (short high-word-only write) SPR0PTL keeps its earlier
    // decoy $7000, so the Copper reads S2 instead and both clauses
    // naturally clash with these same, unchanged expectations.
    if (mv.empty() || mv == "P9-PROP") {
        bool ok = true;
        long last_d = -1;
        for (int f = 0; f + 1 < 5; ++f) {
            uint64_t lo = *p[f], hi = *p[f + 1];
            if (!check_full_traversal(log, kS1Base, 26, lo, hi)) ok = false;
            std::vector<nmh::VideoSample> slice(video.begin() + (long)lo, video.begin() + (long)std::min(hi, (uint64_t)video.size()));
            auto lines = nmh::find_lines(slice);
            bool found_group1 = false;
            for (auto& ln : lines) {
                LineEdges e = find_edges(slice, ln);
                if (!e.white_found || !e.sprite0_found) continue;
                if (!found_group1) { last_d = e.e_b - e.e_a; found_group1 = true; }
            }
            if (!found_group1) ok = false;
        }
        ok = ok && last_d == 66;
        c.expect(ok, "P9 (CH4-p110.c7 CH4-ptr.c10): one 68000 long-word write to SPR0PTH sets the whole "
                 "pointer (p.110, p.25) -- reads at $25000-$25032, group 1 d=66, per field"
                 " (measured d=" + std::to_string(last_d) + ")");
    }
}

} // namespace cpuptr

// ============================================================================
// JUMP: P10
// ============================================================================
namespace jump {

constexpr uint32_t kL1Base = 0x020000 / 2, kL2Base = 0x020800 / 2;

std::string bin_for(const std::string& mv) {
    if (mv == "P10-PROP") return "prog_jump_prop.bin";
    if (mv == "P10-PRE1") return "prog_jump_pre1.bin";
    if (mv == "P10-PRE2") return "prog_jump_pre2.bin";
    if (mv == "P10-PRE3") return "prog_jump_pre3.bin";
    if (mv == "FIXTURE-JUMP") return "prog_jump_fixture.bin";
    return "prog_jump.bin";
}

void run(const std::string& dir, const std::string& mv, Checks& c) {
    nmh::Config cfg;
    // P10-PRE3 needs the Copper's WAIT-for-line-$F0 read of field 6, which
    // happens after the program's own last marker and DONE (see run_program).
    auto r = run_program(cfg, dir + "/" + bin_for(mv), 700'000);
    bool run_ok = r.loaded > 0 && r.released && r.done_seen;
    std::string tag = mv.empty() ? "JUMP" : mv;

    if (mv.empty())
        c.expect(run_ok, "RUN-JUMP: prog_jump.bin loaded (" + std::to_string(r.loaded) +
                 " bytes), CPU released from reset, DONE reached" +
                 (r.done_seen ? (" at cycle " + std::to_string(r.done_cycle)) : " -- not seen"));
    if (!run_ok) { if (!mv.empty()) c.expect(false, tag + ": run did not complete, nothing graded"); return; }

    const auto& log = r.h->ram_log();

    auto fixture = r.h->peek_logical(kResults + 0);
    if (mv.empty() || mv == "FIXTURE-JUMP")
        c.expect(fixture.has_value() && *fixture == 0xBEEF,
                 "FIXTURE-JUMP: prog stores a known constant (BEEF) at RESULTS before anything else is trusted"
                 " (read back " + (fixture ? hex4(*fixture) : std::string("nothing")) + ")");

    // markers M_1..M_6, tag 1, at MARK+0..MARK+10 -- unless MUT_P10PRE2 omits M_2
    std::vector<std::optional<uint64_t>> m(6);
    for (int i = 0; i < 6; ++i) m[i] = find_write(log, kMark / 2 + i, 1);

    if (mv.empty() || mv == "P10-PRE2") {
        bool ok = true;
        for (int i = 0; i < 6; ++i) if (!m[i].has_value()) ok = false;
        c.expect(ok, "P10-PRE2: every M_f appears at the port (field 2's M_2 in particular)");
    }

    // field boundaries: each field runs from the Copper's read of L1's first
    // word preceding M_f to its next read (R5 2.7).
    auto l1fetches = find_reads(log, kL1Base);
    std::sort(l1fetches.begin(), l1fetches.end());
    auto field_of = [&](uint64_t mcycle) -> std::optional<std::pair<uint64_t,uint64_t>> {
        std::optional<uint64_t> lo;
        for (auto f : l1fetches) if (f <= mcycle) lo = f; else break;
        if (!lo) return std::nullopt;
        std::optional<uint64_t> hi;
        for (auto f : l1fetches) if (f > mcycle) { hi = f; break; }
        return std::make_pair(*lo, hi.value_or(*lo + 1'000'000));
    };

    // P10-PRE1: in every field, the Copper reads $20000 and $20002 once, at
    // the field's head. Unchanged comparator: under MUT_P10PRE1 (DMACON
    // without COPEN) there are no L1 reads at all, so field_of() finds no
    // field for any marker and this goes red through that same check.
    if (mv.empty() || mv == "P10-PRE1") {
        bool ok = true;
        for (int i = 0; i < 6; ++i) {
            if (!m[i]) continue;
            auto fi = field_of(*m[i]);
            if (!fi) { ok = false; continue; }
            int c20000 = 0, c20002 = 0;
            for (auto& a : log) if (!a.is_write && a.cycle >= fi->first && a.cycle < fi->second) {
                if (a.word_addr == kL1Base) ++c20000;
                if (a.word_addr == kL1Base + 1) ++c20002;
            }
            if (c20000 != 1 || c20002 != 1) ok = false;
        }
        c.expect(ok, "P10-PRE1: in every field, the Copper reads $20000 and $20002 once, at the field's head (" +
                 std::to_string(l1fetches.size()) + " total reads of $20000 observed)");
    }

    // P10-PRE3: M_f falls before the Copper's read of $20004 in its field
    // (when the WAIT for line $F0 completes). Unchanged comparator: under
    // MUT_P10PRE3 (wait until V7-0=$F4 before the marker) M_f naturally
    // falls after that read, so this goes red through the same check.
    if (mv.empty() || mv == "P10-PRE3") {
        bool ok = true;
        for (int i = 0; i < 6; ++i) {
            if (!m[i]) continue;
            auto fi = field_of(*m[i]);
            if (!fi) { ok = false; continue; }
            auto r20004 = find_read(log, kL1Base + 2, fi->first);
            if (!(r20004 && *m[i] < *r20004)) ok = false;
        }
        c.expect(ok, "P10-PRE3: M_f falls before the Copper's read of $20004 in its field, which is when the "
                 "WAIT for line $F0 completes");
    }

    // ---- P10 (APPB2-legend.s1 APPB2-legend.s2 CH4-APPB-LEGEND.c022
    // CH4-APPB-LEGEND.c023 CH4-APPB-LEGEND.c024): a strobe register takes
    // effect by being written, whatever data is written. -------------------
    if (mv.empty() || mv == "P10-PROP") {
        bool ok = true;
        for (int i = 0; i < 6; ++i) {
            int fnum = i + 1;
            if (!m[i]) { ok = false; continue; }
            auto fi = field_of(*m[i]);
            if (!fi) { ok = false; continue; }
            bool l2_before_m = false;
            for (auto& a : log) if (!a.is_write && a.cycle < *m[i] && a.cycle >= fi->first &&
                                     a.word_addr >= kL2Base && a.word_addr < kL2Base + 4) l2_before_m = true;
            if (l2_before_m) ok = false;

            // first read after M_i in either list
            std::optional<uint64_t> first_after_cyc; uint32_t first_after_addr = 0;
            for (auto& a : log) if (!a.is_write && a.cycle >= *m[i] && a.cycle < fi->second &&
                                     ((a.word_addr >= kL1Base && a.word_addr < kL1Base + 6) ||
                                      (a.word_addr >= kL2Base && a.word_addr < kL2Base + 4))) {
                if (!first_after_cyc || a.cycle < *first_after_cyc) { first_after_cyc = a.cycle; first_after_addr = a.word_addr; }
            }
            // Fixed, unchanged comparator: fields 2/4/6 always expect L2
            // first. Under MUT_P10PROP field 4 writes COLOR01 (not a
            // strobe) instead of COPJMP2, so no restart happens there and
            // this naturally clashes with the same, unchanged expectation.
            bool expect_l2 = (fnum == 2 || fnum == 4 || fnum == 6);
            c.note("P10 field " + std::to_string(fnum) + ": first read after M_" + std::to_string(fnum) +
                   " in either list is at $" + hex6(first_after_addr * 2) +
                   (first_after_cyc ? (" (cycle " + std::to_string(*first_after_cyc) + ")") : " (none)"));
            if (expect_l2) {
                if (!(first_after_cyc && first_after_addr == kL2Base)) ok = false;
            } else {
                if (!(first_after_cyc && first_after_addr == kL1Base + 2)) ok = false;
                bool any_l2 = false;
                for (auto& a : log) if (!a.is_write && a.cycle >= *m[i] && a.cycle < fi->second &&
                                         a.word_addr >= kL2Base && a.word_addr < kL2Base + 4) any_l2 = true;
                if (any_l2) ok = false;
            }
        }
        c.expect(ok, "P10 (APPB2-legend.s1 APPB2-legend.s2 CH4-APPB-LEGEND.c022 CH4-APPB-LEGEND.c023 "
                 "CH4-APPB-LEGEND.c024): writing a strobe (COPJMP2) causes its effect whatever data is written "
                 "(p.287,25,26) -- fields 2/4/6 fetch L2 first, fields 1/3/5 fetch L1's MOVE first and never L2");
    }
}

} // namespace jump

// ============================================================================
// BLIT: P11
// ============================================================================
namespace blit {

constexpr uint32_t kSrcBase = 0x030000 / 2, kDstBase = 0x030200 / 2;

std::string bin_for(const std::string& mv) {
    if (mv == "P11-PROP") return "prog_blit_prop.bin";
    if (mv == "P11-PRE1") return "prog_blit_pre1.bin";
    if (mv == "P11-PRE3") return "prog_blit_pre3.bin";
    if (mv == "FIXTURE-BLIT") return "prog_blit_fixture.bin";
    return "prog_blit.bin";
}

void run(const std::string& dir, const std::string& mv, Checks& c) {
    nmh::Config cfg;
    auto r = run_program(cfg, dir + "/" + bin_for(mv));
    bool run_ok = r.loaded > 0 && r.released && r.done_seen;
    std::string tag = mv.empty() ? "BLIT" : mv;

    if (mv.empty())
        c.expect(run_ok, "RUN-BLIT: prog_blit.bin loaded (" + std::to_string(r.loaded) +
                 " bytes), CPU released from reset, DONE reached" +
                 (r.done_seen ? (" at cycle " + std::to_string(r.done_cycle)) : " -- not seen"));
    if (!run_ok) { if (!mv.empty()) c.expect(false, tag + ": run did not complete, nothing graded"); return; }

    const auto& log = r.h->ram_log();

    auto fixture = r.h->peek_logical(kResults + 0);
    if (mv.empty() || mv == "FIXTURE-BLIT")
        c.expect(fixture.has_value() && *fixture == 0xBEEF,
                 "FIXTURE-BLIT: prog stores a known constant (BEEF) at RESULTS before anything else is trusted"
                 " (read back " + (fixture ? hex4(*fixture) : std::string("nothing")) + ")");

    // Both preconditions below are graded together under NMH_MUTATE=P11-PRE1
    // (DMACON without BLTEN), which the page predicts breaks both at once.
    auto dmaconr = r.h->peek_logical(kResults + 2);
    if (mv.empty() || mv == "P11-PRE1")
        c.expect(dmaconr.has_value() && ((*dmaconr >> 9) & 1) == 1 && ((*dmaconr >> 6) & 1) == 1,
                 "P11-PRE2: RESULT+2 (DMACONR) has bits 9 and 6 set (DMAEN, BLTEN) (DMACONR=" +
                 (dmaconr ? hex4(*dmaconr) : std::string("--")) + ")");

    auto m1 = find_write(log, kMark / 2 + 0, 1);
    auto m2 = find_write(log, kMark / 2 + 1, 2);
    if (!m1 || !m2) { c.expect(false, tag + ": M1/M2 not observed, nothing graded"); return; }

    // precondition 1: four reads at $30000-$30006 in M1-M2 (the blit ran)
    bool a_read_ok = check_full_traversal(log, kSrcBase, 4, *m1, *m2);
    if (mv.empty() || mv == "P11-PRE1")
        c.expect(a_read_ok, "P11-PRE1: four reads at $30000-$30006, in M1-M2, show the blit ran");

    // precondition 3: M1-M2 is a quiet window (no CPU write inside beyond
    // the blitter's own DMA writes to DST1). A CPU write is any write NOT
    // explained by the expected 4-word blitter output at kDstBase.
    if (mv.empty() || mv == "P11-PRE3") {
        bool extra_write = false;
        for (auto& a : log) if (a.is_write && a.cycle > *m1 && a.cycle < *m2 &&
                                 !(a.word_addr >= kDstBase && a.word_addr < kDstBase + 4))
            extra_write = true;
        bool quiet = !extra_write;
        c.expect(quiet, "P11-PRE3: M1-M2 is a quiet window (no CPU write to Chip memory beyond the blitter's own "
                 "DST1 output)");
    }

    // ---- P11 (CH4-APPB-LEGEND.c017): an early read from the blitter is a
    // DMA transfer into Chip memory. --------------------------------------
    if (mv.empty() || mv == "P11-PROP") {
        std::vector<const nmh::RamAccess*> writes;
        for (auto& a : log) if (a.is_write && a.cycle > *m1 && a.cycle < *m2 &&
                                 a.word_addr >= kDstBase && a.word_addr < kDstBase + 5) writes.push_back(&a);
        std::sort(writes.begin(), writes.end(), [](auto x, auto y) { return x->cycle < y->cycle; });
        // Fixed, unchanged comparator: exactly four writes with these exact
        // values. Under MUT_P11PROP (USED cleared) the page predicts no
        // writes into DST1 at all, so writes.size()==4 clashes naturally.
        bool ok = writes.size() == 4 && writes[0]->word_addr == kDstBase && writes[0]->data == 0x1234 &&
                  writes[1]->word_addr == kDstBase + 1 && writes[1]->data == 0x5678 &&
                  writes[2]->word_addr == kDstBase + 2 && writes[2]->data == 0x9ABC &&
                  writes[3]->word_addr == kDstBase + 3 && writes[3]->data == 0xDEF0;
        bool no_guard = true;
        for (auto& a : log) if (a.is_write && a.cycle > *m1 && a.cycle < *m2 && a.word_addr == kDstBase + 4) no_guard = false;
        ok = ok && no_guard && a_read_ok;
        c.expect(ok, "P11 (CH4-APPB-LEGEND.c017): an early read from the blitter is a DMA data transfer to RAM "
                 "(p.287,269) -- exactly four writes at $30200-$30206, $1234/$5678/$9ABC/$DEF0, no write at "
                 "$30208, in M1-M2 (" + std::to_string(writes.size()) + " writes observed)");
    }
}

} // namespace blit

// ============================================================================
int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    const char* mutate_env = std::getenv("NMH_MUTATE");
    std::string mv = mutate_env ? mutate_env : "";
    std::printf("MUTATE: %s\n", mv.empty() ? "(none)" : mv.c_str());

    Checks c;

    if (mv.empty()) {
        manual::run(dir, mv, c);
        autorun::run(dir, mv, c);
        cpuptr::run(dir, mv, c);
        jump::run(dir, mv, c);
        blit::run(dir, mv, c);
    } else if (mv.rfind("P4", 0) == 0 || mv.rfind("P5", 0) == 0 || mv == "FIXTURE-MANUAL") {
        manual::run(dir, mv, c);
    } else if (mv.rfind("P6", 0) == 0 || mv.rfind("P7", 0) == 0 || mv.rfind("P8", 0) == 0 || mv == "FIXTURE-AUTO") {
        autorun::run(dir, mv, c);
    } else if (mv.rfind("P9", 0) == 0 || mv == "FIXTURE-CPUPTR") {
        cpuptr::run(dir, mv, c);
    } else if (mv.rfind("P10", 0) == 0 || mv == "FIXTURE-JUMP") {
        jump::run(dir, mv, c);
    } else if (mv.rfind("P11", 0) == 0 || mv == "FIXTURE-BLIT") {
        blit::run(dir, mv, c);
    } else {
        c.expect(false, "NMH_MUTATE=" + mv + ": unknown tag");
    }

    std::printf("%d passed, %d failed\n", c.passed, c.failed);
    return c.failed == 0 ? 0 : 1;
}
