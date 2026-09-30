// test_ch3_playfield_system.cpp
//
// ch3-playfield-system: see SPECIFICATION.md.
//
// P3 (BPLCON0 bit 3, LPEN) and P4 (LPEN after power-up) share one program
// family (prog_lpen_*.s / prog_lpen_core.s): five VERTB-synchronized bursts
// of 600 long reads of VPOSR/VHPOSR, stored to MEASURE_BASE. P3 grades LPEN=1
// against LPEN=0; P4 grades the power-up state (BPLCON0 never written).
//
// A CPU write to a custom register (BPLCON0, BEAMCON0) never reaches the
// Chip-memory port -- only chip-RAM traffic (CPU or Copper) does. Where a
// precondition is about "the program contains [no] write to $DFF1xx", this
// file scans the loaded program's own machine code for the one instruction
// form every source here uses for such a write, MOVE.W #imm,d16(An)
// (opcode & 0xF1FF) == 0x317C, An in bits 11-9 -- verified against a vasm
// build (opcode_test.s, not part of this package) before being relied on
// here. A Copper-issued write to the same register is a normal chip-RAM
// read (the Copper list itself) and is checked on ram_log instead.
//
// NMH_MUTATE selects one mutated scenario in place of its normal
// counterpart; none is exercised by a plain `make test`. Every mutation runs
// the package's normal comparator unchanged: the only NMH_MUTATE-conditioned
// code left in this file selects which binary, Copper list or window is fed
// to that unchanged comparator (never the pass/fail meaning of a clause).
//   NMH_MUTATE=P3D          prog_lpen_ecs_pal_l1_mutd.bin   (extra BPLCON0 write)
//   NMH_MUTATE=P3A          prog_lpen_ecs_pal_l1_burst20.bin (20 reads, not 600)
//   NMH_MUTATE=P3B          window end moved to t_f+30*227cc
//   NMH_MUTATE=P3C_LPENDIS  prog_lpen_ecs_pal_l1_lpendis.bin (LPENDIS=1)
//   NMH_MUTATE=P3C_WRONGSTD prog_lpen_ecs_pal_l0_wrongstd.bin (BEAMCON0 bit5 wrong)
//   NMH_MUTATE=P4MAIN       prog_lpen_ecs_pal_l1.bin graded under P4's check
//   NMH_MUTATE=P4B_PROG     prog_lpen_ecs_pal_l1.bin, static-scan precondition
//   NMH_MUTATE=P4B_COP      prog_lpen_p4_copper.bin (+Copper preload)
//   NMH_MUTATE=P4A          prog_lpen_p4_ecs_pal_burst20.bin
//   NMH_MUTATE=P4C          prog_lpen_p4_ecs_pal_wrongstd.bin
//   NMH_MUTATE=P1MAIN       config A/B run "1" uses a colour-register change
//                           instead of bit 9, with run "0"'s Copper list
//   NMH_MUTATE=P1A          dedicated PAL-vs-NTSC (S1 vs S2) run "0" comparison
//   NMH_MUTATE=P1C          prog_p1_2c_pal_nobplen.bin (config B, S1, both runs)
//   NMH_MUTATE=P1D          config B, S1, both runs' bitplane buffer zeroed
//   NMH_MUTATE=P1E          dedicated: a genuine PAL run graded against NTSC
//   NMH_MUTATE=P2B          prog_p1_2c_pal_mod2.bin (config B, S1, both runs)
//   NMH_MUTATE=P5MAIN       prog_p5_pal_v1_novarvben.bin
//   NMH_MUTATE=P5A          prog_p5_pal_v1_nobplen.bin
//   NMH_MUTATE=P5B          dedicated: Copper WAIT(N-1,$30) before COLOR01=C2
//                           replaced by WAIT(N-2,$30); the C3-side WAIT unchanged
//   NMH_MUTATE=P5C          prog_p5_pal_v1_wrongstd.bin
//
// MUTATION opus:A-bpc.b03.c0 stimulus red an extra BPLCON0 write after the first VERTB detection, via NMH_MUTATE=P3D
// MUTATION P3-a stimulus red burst reduced from 600 to 20 reads, fewer than 100 fall in the window, via NMH_MUTATE=P3A
// MUTATION P3-b stimulus red window end moved to t_f+30*227cc, reads with V>19 appear in L0, via NMH_MUTATE=P3B
// MUTATION P3-c stimulus red BEAMCON0 LPENDIS=1, L1's windows show strictly increasing values, via NMH_MUTATE=P3C_LPENDIS
// MUTATION P3-c stimulus red BEAMCON0 bit5 opposite the configured standard, field-length check reports the other standard, via NMH_MUTATE=P3C_WRONGSTD
// MUTATION opus:A-leg.lpen2 stimulus red the P3-L1 program graded under P4's strictly-increasing check, via NMH_MUTATE=P4MAIN
// MUTATION P4-b stimulus red the P3-L1 program's static scan shows a BPLCON0 write P4's own program must not have, via NMH_MUTATE=P4B_PROG
// MUTATION P4-b stimulus red a Copper list writes BPLCON0 even though the program itself never does, via NMH_MUTATE=P4B_COP
// MUTATION P4-a stimulus red burst reduced to 20 reads in the P4 program, via NMH_MUTATE=P4A
// MUTATION P4-c stimulus red BEAMCON0 bit5 opposite the configured standard in the P4 program, via NMH_MUTATE=P4C
// MUTATION opus:3-a1000.c04 stimulus red a colour register changes instead of bit 9, same Copper list otherwise, via NMH_MUTATE=P1MAIN
// MUTATION P1-a stimulus red a genuine PAL run's vsync-interval pattern compared with a genuine NTSC run's, via NMH_MUTATE=P1A
// MUTATION P1-c stimulus red DMACON without BPLEN, no bitplane reads occur, via NMH_MUTATE=P1C
// MUTATION P1-d stimulus red the bitplane buffer filled with $0000, no sample shows C1, via NMH_MUTATE=P1D
// MUTATION P1-e stimulus red a genuine PAL run's field length graded against NTSC's values, via NMH_MUTATE=P1E
// MUTATION P2-b stimulus red BPL1MOD=$0002, later lines start two bytes further out, via NMH_MUTATE=P2B
// MUTATION opus:B-vbstrt.addr stimulus red BEAMCON0 without VARVBEN, line N also shows C2, via NMH_MUTATE=P5MAIN
// MUTATION P5-a stimulus red DMACON without BPLEN, no line shows C1, via NMH_MUTATE=P5A
// MUTATION P5-b stimulus red Copper WAIT(N-1,$30) before COLOR01=C2 replaced by WAIT(N-2,$30), the C2-side anchor finds V=N-2, via NMH_MUTATE=P5B
// MUTATION P5-c stimulus red BEAMCON0 bit5 opposite the configured standard in the P5 program, via NMH_MUTATE=P5C

#include <cstdio>
#include <cstdlib>
#include <cstring>
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

// nanomig.i, restated.
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint32_t kMeasureBase = 0x0C1000;
constexpr uint32_t kCopListAddr = 0x020000;

constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudget  = 6'000'000;

// 1 colour clock (cc) = 8 clk_sys cycles. p.24: "One horizontal count takes
// one cycle of the system clock (processor is twice this)", i.e. one cc
// equals two processor-clock cycles; nanomig_harness.h documents the RAM
// port as sampled "once per 7 MHz bus period" (see RamAccess and tick_pair's
// comment), i.e. the processor's own clock runs one cycle every 4 clk_sys
// cycles here -- so one cc is 2*4 = 8 clk_sys cycles. This ratio is a fixed
// property of the harness's clock generator (SPECIFICATION.md, "Line and
// colour-clock lengths": "converts colour clocks to the harness's
// system-clock cycles using the harness's clock definition"), the same for
// every run; it is not a value fitted to make any check pass.
constexpr uint64_t CC = 8;

constexpr uint64_t kWinStart = 228 * CC;         // t_f + 228 cc
constexpr uint64_t kWinEnd   = 18 * 227 * CC;     // t_f + 18*227 cc
constexpr uint64_t kWinEndP3bMutant = 30 * 227 * CC; // P3-b contrast
constexpr uint64_t kSlack    = 30 * CC;           // 30 cc, P3-a

// Field-length check (Common definitions): PAL 312/313 lines of 227 cc;
// NTSC 262/263 lines alternating 227/228 cc, within one line (227 cc) plus
// the period of the detecting loop. The 227 cc term is the page's own
// figure; the "+300" clk_sys-cycle term is a margin for the VERTB-polling
// loop's own period, tuned on this RTL because a CPU register read (unlike
// the chip-RAM write of its own marker/burst store) never reaches the
// Chip-memory port and so cannot be timed directly -- disclosed in
// SPECIFICATION.md's build notes, together with the observed field-length
// deltas the run prints for every system configuration (MEASURED lines).
constexpr uint64_t kFieldSlack = 227 * CC + 300;
constexpr uint64_t kPalMin  = 70824ull * CC, kPalMax  = 71051ull * CC;
constexpr uint64_t kNtscMin = 59605ull * CC, kNtscMax = 59833ull * CC;

bool field_length_ok(uint64_t delta, bool ntsc) {
    uint64_t a = ntsc ? kNtscMin : kPalMin;
    uint64_t b = ntsc ? kNtscMax : kPalMax;
    auto near = [&](uint64_t x) { return delta + kFieldSlack >= x && delta <= x + kFieldSlack; };
    return near(a) || near(b);
}

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

nmh::Config cfg_for_s(int s) {
    nmh::Config c;
    switch (s) {
        case 1: c.chipset_config = 0x08; break; // ECS, PAL
        case 2: c.chipset_config = 0x0A; break; // ECS, NTSC
        case 3: c.chipset_config = 0x00; break; // OCS, PAL
        case 4: c.chipset_config = 0x02; break; // OCS, NTSC
    }
    return c;
}
bool s_is_ntsc(int s) { return s == 2 || s == 4; }
bool s_is_ecs(int s)  { return s == 1 || s == 2; }
std::string s_name(int s) {
    return std::string(s_is_ecs(s) ? "ECS" : "OCS") + "/" + (s_is_ntsc(s) ? "NTSC" : "PAL");
}

struct Run {
    size_t loaded = 0;
    bool released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;   // coalesced full log (reads and writes)
    std::vector<uint16_t> prog_words;  // the loaded program's own image
};

// preload: physical byte address (even, <$80000 is fine -- see playfield.i)
// -> words to poke there before boot(), for a program that only points a
// register at the address (a CPU write there would be lost to the overlay).
Run run_program(const std::string& bin, const nmh::Config& cfg,
                 const std::vector<std::pair<uint32_t, std::vector<uint16_t>>>& preloads = {}) {
    Run r;
    nmh::NanomigHarness h(cfg);
    for (auto& pl : preloads)
        for (size_t i = 0; i < pl.second.size(); ++i) h.poke_word(pl.first / 2 + i, pl.second[i]);
    r.loaded = h.load_program(bin);
    if (!r.loaded) return r;
    for (size_t i = 0; i < r.loaded; i += 2) {
        auto w = h.peek_logical(nmh::kProgramOrg + i);
        r.prog_words.push_back(w.value_or(0xDEAD));
    }
    r.released = h.boot(kBootBudget).cpu_released;
    r.done = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    r.log = nmh::coalesce(h.ram_log());
    return r;
}

// Scans for MOVE.W #imm,d16(An) -- opcode & 0xF1FF == 0x317C, An in bits
// 11-9 -- with displacement == disp and register == an, and returns how
// many such instructions the loaded program contains.
int count_writes_to(const std::vector<uint16_t>& prog, uint8_t an, uint16_t disp) {
    int n = 0;
    for (size_t i = 0; i + 2 < prog.size(); ++i) {
        uint16_t op = prog[i];
        if ((op & 0xF1FF) == 0x317C && ((op >> 9) & 7) == an && prog[i + 2] == disp) ++n;
    }
    return n;
}
std::optional<uint16_t> find_write_value(const std::vector<uint16_t>& prog, uint8_t an, uint16_t disp) {
    for (size_t i = 0; i + 2 < prog.size(); ++i) {
        uint16_t op = prog[i];
        if ((op & 0xF1FF) == 0x317C && ((op >> 9) & 7) == an && prog[i + 2] == disp) return prog[i + 1];
    }
    return std::nullopt;
}

bool copper_fetched_move_to(const std::vector<nmh::RamAccess>& log, uint32_t list_addr, uint16_t reg) {
    for (auto& a : log)
        if (!a.is_write && a.word_addr == list_addr / 2 && a.data == reg) return true;
    return false;
}

// One VERTB-synchronized burst: the marker's store cycle, then (cycle,
// 32-bit value) for every long read that followed it.
struct Detection {
    uint64_t marker_cycle = 0;
    std::vector<std::pair<uint64_t, uint32_t>> reads;
};

std::vector<Detection> extract_detections(const std::vector<nmh::RamAccess>& log, int burstlen) {
    std::vector<Detection> out;
    const uint32_t base_word = kMeasureBase / 2;
    const uint32_t span = 5u * (1u + 2u * (uint32_t)burstlen);
    std::vector<nmh::RamAccess> buf;
    for (auto& a : log)
        if (a.is_write && a.word_addr >= base_word && a.word_addr < base_word + span) buf.push_back(a);
    size_t idx = 0;
    for (int it = 0; it < 5 && idx < buf.size(); ++it) {
        Detection d;
        d.marker_cycle = buf[idx].cycle; ++idx;
        for (int k = 0; k < burstlen && idx + 1 < buf.size(); ++k) {
            uint32_t hi = buf[idx].data; ++idx;
            uint32_t lo = buf[idx].data; uint64_t c_lo = buf[idx].cycle; ++idx;
            d.reads.push_back({c_lo, (hi << 16) | lo});
        }
        out.push_back(d);
    }
    return out;
}

struct WindowResult {
    int count = 0;
    bool has_min_100 = false;
    bool first_near_start = false;
    bool last_near_end = false;
    bool strictly_increasing = true;
    bool all_equal = true;
    bool v_in_1_19 = true;
};

WindowResult analyze_window(const Detection& d, uint64_t win_start, uint64_t win_end) {
    WindowResult wr;
    std::vector<std::pair<uint64_t, uint32_t>> in;
    for (auto& rc : d.reads) {
        if (rc.first < d.marker_cycle) continue;
        uint64_t rel = rc.first - d.marker_cycle;
        if (rel >= win_start && rel <= win_end) in.push_back(rc);
    }
    wr.count = (int)in.size();
    wr.has_min_100 = wr.count >= 100;
    if (!in.empty()) {
        uint64_t rel_first = in.front().first - d.marker_cycle;
        uint64_t rel_last = in.back().first - d.marker_cycle;
        wr.first_near_start = (rel_first - win_start) <= kSlack;
        wr.last_near_end = (win_end - rel_last) <= kSlack;
    }
    bool first = true;
    uint32_t prev = 0;
    for (auto& rc : in) {
        uint32_t mv = rc.second & 0x1FFFFu;
        uint32_t V = (mv >> 8) & 0x1FFu;
        if (V < 1 || V > 19) wr.v_in_1_19 = false;
        if (!first) {
            if (!(mv > prev)) wr.strictly_increasing = false;
            if (mv != prev) wr.all_equal = false;
        }
        prev = mv; first = false;
    }
    return wr;
}

// Diagnostic only (does not affect any grade): the first pair inside a
// window's counted reads where mv does not increase, with its position,
// cycles and values, or "(none)" if the whole window is strictly increasing.
std::string first_violation(const Detection& d, uint64_t win_start, uint64_t win_end) {
    std::vector<std::pair<uint64_t, uint32_t>> in;
    for (auto& rc : d.reads) {
        if (rc.first < d.marker_cycle) continue;
        uint64_t rel = rc.first - d.marker_cycle;
        if (rel >= win_start && rel <= win_end) in.push_back(rc);
    }
    for (size_t i = 1; i < in.size(); ++i) {
        uint32_t prev = in[i - 1].second & 0x1FFFFu, cur = in[i].second & 0x1FFFFu;
        if (!(cur > prev)) {
            char buf[192];
            std::snprintf(buf, sizeof buf,
                "at read %zu/%zu: prev mv=%05X (V=%u H=%u) cyc=%llu -> cur mv=%05X (V=%u H=%u) cyc=%llu (delta cyc=%lld)",
                i, in.size(), prev, (prev >> 8) & 0x1FFu, prev & 0xFFu, (unsigned long long)in[i - 1].first,
                cur, (cur >> 8) & 0x1FFu, cur & 0xFFu, (unsigned long long)in[i].first,
                (long long)(in[i].first - in[i - 1].first));
            return std::string(buf);
        }
    }
    return "(none)";
}

std::string lpen_bin(const std::string& dir, int s, bool lpen1) {
    if (s_is_ecs(s))
        return dir + (s_is_ntsc(s) ? (lpen1 ? "/prog_lpen_ecs_ntsc_l1.bin" : "/prog_lpen_ecs_ntsc_l0.bin")
                                    : (lpen1 ? "/prog_lpen_ecs_pal_l1.bin" : "/prog_lpen_ecs_pal_l0.bin"));
    return dir + (lpen1 ? "/prog_lpen_ocs_l1.bin" : "/prog_lpen_ocs_l0.bin");
}
std::string p4_bin(const std::string& dir, int s) {
    if (s_is_ecs(s)) return dir + (s_is_ntsc(s) ? "/prog_lpen_p4_ecs_ntsc.bin" : "/prog_lpen_p4_ecs_pal.bin");
    return dir + "/prog_lpen_p4_ocs.bin";
}

// -----------------------------------------------------------------------
// P1/P2 -- the basic playfield (pp.63-64): a Copper-driven display, video-
// graded. The Copper list and bitplane data are preloaded by this test with
// poke_word/poke_long at fixed physical addresses below $40000 (see
// playfield.i) -- the overlay blocks a CPU write there (nanomig_harness.h's
// address-space notes), and the Copper/Agnus's own DMA fetches are not
// gated by the overlay (it only redirects CPU *reads*), so a preloaded
// value there is exactly what the design's own DMA reads back.
// -----------------------------------------------------------------------

constexpr uint32_t kBplPlane1 = 0x028000;
constexpr uint32_t kBplPlane2 = 0x02D000;
constexpr uint32_t kBplPlane3 = 0x032000;
constexpr uint32_t kBplPlane4 = 0x037000;

constexpr uint16_t kC0 = 0x0359, kC1 = 0x0EA2, kC2 = 0x0F3C, kC3 = 0x04E7;

bool shows(const nmh::VideoSample& s, uint16_t cw) {
    return s.r == ((cw >> 8) & 0xF) && s.g == ((cw >> 4) & 0xF) && s.b == (cw & 0xF);
}

// Copper instruction encoding, p.21/p.22 (restated in SPECIFICATION.md
// Common definitions): MOVE = register address word (bit 0 = 0), then data;
// WAIT = (VP<<8)|(HP&$FE)|1, then $FFFE (BFD=1, all compares enabled).
void cop_move(std::vector<uint16_t>& l, uint16_t reg, uint16_t data) { l.push_back(reg); l.push_back(data); }
void cop_wait(std::vector<uint16_t>& l, uint8_t vp, uint8_t hp) {
    l.push_back((uint16_t(vp) << 8) | (uint16_t(hp) & 0xFE) | 1);
    l.push_back(0xFFFE);
}
void cop_end(std::vector<uint16_t>& l) { l.push_back(0xFFFF); l.push_back(0xFFFE); }

// SPECIFICATION.md P1 stimulus: w(L,k) = $8001 OR ((($9E37*(20L+k+1)) mod
// $10000) AND $7FFE).
uint16_t w_be(int L, int k) {
    uint32_t idx = 20u * (uint32_t)L + (uint32_t)k + 1u;
    uint32_t v = (0x9E37u * idx) & 0xFFFFu;
    return uint16_t(0x8001u | (v & 0x7FFEu));
}
// w_p(L,k) = ($9E37*(40L+k+1+10007p)) mod $10000.
uint16_t w_c(int p, int L, int k) {
    uint32_t idx = 40u * (uint32_t)L + (uint32_t)k + 1u + 10007u * (uint32_t)p;
    return uint16_t((0x9E37u * idx) & 0xFFFFu);
}
std::vector<uint16_t> bitplane_be(int H) {
    std::vector<uint16_t> d((size_t)20 * H);
    for (int L = 0; L < H; ++L) for (int k = 0; k < 20; ++k) d[(size_t)L * 20 + k] = w_be(L, k);
    return d;
}
std::vector<uint16_t> bitplane_c(int plane, int H) {
    std::vector<uint16_t> d((size_t)40 * H);
    for (int L = 0; L < H; ++L) for (int k = 0; k < 40; ++k) d[(size_t)L * 40 + k] = w_c(plane, L, k);
    return d;
}

// A Copper list built for a specific run ("0"/"1"), plus, for every MOVE to
// BPLCON0 it contains, the physical word address of that MOVE's *data*
// word and the value it must carry (P1-b/P2-a). The list repeats every
// field (COP1LC auto-restart, p.25/p.30), so each such address is fetched
// once per field with the log accumulating one entry per field -- P1-b
// counts them.
struct CopBuild {
    std::vector<uint16_t> list;
    std::vector<std::pair<uint32_t, uint16_t>> bplcon0_fetches;  // (word addr, expected data)
};

CopBuild build_cop_A(int run) {
    CopBuild cb;
    uint16_t val = run == 0 ? 0x0000 : 0x0200;
    cb.bplcon0_fetches.push_back({kCopListAddr / 2 + 1, val});
    cop_move(cb.list, 0x100, val);
    cop_end(cb.list);
    return cb;
}
CopBuild build_cop_B(int run) {
    CopBuild cb;
    uint16_t val = run == 0 ? 0x1000 : 0x1200;
    cb.bplcon0_fetches.push_back({kCopListAddr / 2 + 1, val});
    cop_move(cb.list, 0x100, val);
    cop_move(cb.list, 0x0E0, uint16_t(kBplPlane1 >> 16));
    cop_move(cb.list, 0x0E2, uint16_t(kBplPlane1 & 0xFFFF));
    cop_end(cb.list);
    return cb;
}
CopBuild build_cop_C(int run) {
    CopBuild cb;
    uint16_t val = run == 0 ? 0xC000 : 0xC200;
    cb.bplcon0_fetches.push_back({kCopListAddr / 2 + 1, val});
    cop_move(cb.list, 0x100, val);
    uint32_t planes[4] = {kBplPlane1, kBplPlane2, kBplPlane3, kBplPlane4};
    uint16_t ph[4] = {0x0E0, 0x0E4, 0x0E8, 0x0EC}, pl[4] = {0x0E2, 0x0E6, 0x0EA, 0x0EE};
    for (int i = 0; i < 4; ++i) {
        cop_move(cb.list, ph[i], uint16_t(planes[i] >> 16));
        cop_move(cb.list, pl[i], uint16_t(planes[i] & 0xFFFF));
    }
    cop_end(cb.list);
    return cb;
}
CopBuild build_cop_E(int run) {
    CopBuild cb;
    cb.bplcon0_fetches.push_back({kCopListAddr / 2 + 1, 0x1000});
    cop_move(cb.list, 0x100, 0x1000);
    cop_move(cb.list, 0x0E0, uint16_t(kBplPlane1 >> 16));
    cop_move(cb.list, 0x0E2, uint16_t(kBplPlane1 & 0xFFFF));
    for (int L = 0x2C; L <= 0xFE; ++L) {
        cop_wait(cb.list, (uint8_t)L, 0x80);
        uint32_t off = (uint32_t)cb.list.size();
        uint16_t val = (run == 0) ? 0x1000 : ((L & 1) ? 0x1200 : 0x1000);
        cop_move(cb.list, 0x100, val);
        cb.bplcon0_fetches.push_back({kCopListAddr / 2 + off + 1, val});
    }
    cop_end(cb.list);
    return cb;
}

// DMA field boundaries (P1-c): consecutive Copper fetches of the list's own
// first word (always the BPLCON0 MOVE's register-address word, $0100, at
// kCopListAddr).
std::vector<uint64_t> dma_field_starts(const std::vector<nmh::RamAccess>& log) {
    std::vector<uint64_t> out;
    for (auto& a : log) if (!a.is_write && a.word_addr == kCopListAddr / 2) out.push_back(a.cycle);
    return out;
}

// vsync assertion (Common definitions): the transition into the level vsync
// holds for the shorter part of a field. Found generically (without
// assuming polarity) as the transition into whichever level is the
// minority over the whole trace.
std::vector<size_t> find_vsync_assertions(const std::vector<nmh::VideoSample>& v) {
    size_t true_count = 0;
    for (auto& s : v) if (s.vs) ++true_count;
    bool active = (true_count * 2 < v.size());
    std::vector<size_t> out;
    bool prev = v.empty() ? false : v[0].vs;
    for (size_t i = 1; i < v.size(); ++i) {
        if (v[i].vs == active && prev != active) out.push_back(i);
        prev = v[i].vs;
    }
    return out;
}

struct VideoRun {
    size_t loaded = 0;
    bool released = false;
    std::vector<nmh::RamAccess> log;
    std::vector<nmh::VideoSample> video;
};

VideoRun run_video(const std::string& bin, const nmh::Config& cfg, uint64_t boot_budget, uint64_t run_cycles,
                    const std::vector<std::pair<uint32_t, std::vector<uint16_t>>>& preloads) {
    VideoRun r;
    nmh::NanomigHarness h(cfg);
    for (auto& pl : preloads)
        for (size_t i = 0; i < pl.second.size(); ++i) h.poke_word(pl.first / 2 + i, pl.second[i]);
    r.loaded = h.load_program(bin);
    if (!r.loaded) return r;
    r.released = h.boot(boot_budget).cpu_released;
    h.run_cycles(run_cycles);
    r.log = nmh::coalesce(h.ram_log());
    r.video = h.video();
    return r;
}

std::string p1_bin(const std::string& dir, bool palette16, bool ntsc) {
    if (palette16) return dir + (ntsc ? "/prog_p1_16c_ntsc.bin" : "/prog_p1_16c_pal.bin");
    return dir + (ntsc ? "/prog_p1_2c_ntsc.bin" : "/prog_p1_2c_pal.bin");
}

constexpr uint64_t kP1BootBudget = 3'000'000;
constexpr uint64_t kP1RunCycles  = 5'500'000;  // >= 9 PAL max fields (568408*9 ~= 5.12M) + margin;
                                                // P2 needs 6 DMA-field starts / 7 vsync assertions

struct DisplayData {
    VideoRun run0, run1;
    CopBuild c0, c1;
    int H = 0;
    bool ok = false;
};

// Builds and runs one display configuration's pair of runs ("0"/"1"). ov0/ov1
// override the CPU-program binary (empty = the normal one for cfgname/
// standard); flip_word_index (config B/E only) XORs bit 14 of that word
// index into the (unmodified-formula) bitplane data of BOTH runs (P2's
// main-check contrast); mutant_same_list makes run "1" reuse run "0"'s
// Copper list verbatim (P1's main-check contrast: bit 9 never changes,
// only a colour register does, via ov1); zero_bitplane fills the buffer
// with $0000 (P1-d contrast).
DisplayData build_and_run_display(const std::string& dir, char cfgname, int s,
                                   const std::string& ov0, const std::string& ov1,
                                   bool mutant_same_list, bool zero_bitplane, int flip_word_index = -1) {
    DisplayData d;
    bool ntsc = s_is_ntsc(s);
    d.H = ntsc ? 200 : 256;
    bool palette16 = (cfgname == 'C');
    std::string bin0 = ov0.empty() ? p1_bin(dir, palette16, ntsc) : ov0;
    std::string bin1 = ov1.empty() ? p1_bin(dir, palette16, ntsc) : ov1;

    std::vector<std::pair<uint32_t, std::vector<uint16_t>>> bpl;
    if (cfgname == 'B' || cfgname == 'E') {
        std::vector<uint16_t> data = zero_bitplane ? std::vector<uint16_t>((size_t)20 * d.H, 0) : bitplane_be(d.H);
        if (flip_word_index >= 0 && (size_t)flip_word_index < data.size()) data[flip_word_index] ^= 0x4000;
        bpl.push_back({kBplPlane1, data});
    } else if (cfgname == 'C') {
        bpl.push_back({kBplPlane1, bitplane_c(1, d.H)});
        bpl.push_back({kBplPlane2, bitplane_c(2, d.H)});
        bpl.push_back({kBplPlane3, bitplane_c(3, d.H)});
        bpl.push_back({kBplPlane4, bitplane_c(4, d.H)});
    }

    if (cfgname == 'A') { d.c0 = build_cop_A(0); d.c1 = build_cop_A(1); }
    else if (cfgname == 'B') { d.c0 = build_cop_B(0); d.c1 = build_cop_B(1); }
    else if (cfgname == 'C') { d.c0 = build_cop_C(0); d.c1 = build_cop_C(1); }
    else { d.c0 = build_cop_E(0); d.c1 = build_cop_E(1); }

    auto preloads0 = bpl; preloads0.push_back({kCopListAddr, d.c0.list});
    auto preloads1 = bpl; preloads1.push_back({kCopListAddr, mutant_same_list ? d.c0.list : d.c1.list});

    d.run0 = run_video(bin0, cfg_for_s(s), kP1BootBudget, kP1RunCycles, preloads0);
    d.run1 = run_video(bin1, cfg_for_s(s), kP1BootBudget, kP1RunCycles, preloads1);
    d.ok = d.run0.loaded > 0 && d.run1.loaded > 0;
    return d;
}

// -----------------------------------------------------------------------
// P5 -- VBSTRT ($1CC, ECS) and the line at which vertical blanking starts.
// -----------------------------------------------------------------------

struct P5Cop {
    std::vector<uint16_t> list;
    uint32_t addr_c2 = 0;  // word address of the MOVE COLOR01,C2 data word
    uint32_t addr_c3 = 0;  // word address of the MOVE COLOR01,C3 data word
};

// marker_n is the vertical position the WAITs (and thus the anchor, P5-b)
// use; reg_n is what VBSTRT/VBSTOP actually get set to in the CPU program.
// They are equal except for the "value" contrasting case, where the
// program writes VBSTRT=reg_n while the Copper markers stay at marker_n.
P5Cop build_cop_P5(uint8_t marker_n) {
    P5Cop cb;
    cop_move(cb.list, 0x0E0, uint16_t(kBplPlane1 >> 16));
    cop_move(cb.list, 0x0E2, uint16_t(kBplPlane1 & 0xFFFF));
    cop_move(cb.list, 0x182, kC1);
    cop_wait(cb.list, uint8_t(marker_n - 1), 0x30);
    cb.addr_c2 = kCopListAddr / 2 + (uint32_t)cb.list.size() + 1;
    cop_move(cb.list, 0x182, kC2);
    cop_wait(cb.list, uint8_t(marker_n + 1), 0x30);
    cb.addr_c3 = kCopListAddr / 2 + (uint32_t)cb.list.size() + 1;
    cop_move(cb.list, 0x182, kC3);
    cop_end(cb.list);
    return cb;
}

struct P5Run {
    VideoRun vr;
    P5Cop cop;
    bool ok = false;
};

std::string p5_bin(const std::string& dir, bool ntsc, bool v2) {
    if (ntsc) return dir + (v2 ? "/prog_p5_ntsc_v2.bin" : "/prog_p5_ntsc_v1.bin");
    return dir + (v2 ? "/prog_p5_pal_v2.bin" : "/prog_p5_pal_v1.bin");
}

P5Run run_p5(const std::string& bin, int s, uint8_t marker_n) {
    P5Run r;
    r.cop = build_cop_P5(marker_n);
    std::vector<uint16_t> bitplane(20 * 196, 0xFFFF);
    std::vector<std::pair<uint32_t, std::vector<uint16_t>>> preloads = {
        {kBplPlane1, bitplane}, {kCopListAddr, r.cop.list}};
    r.vr = run_video(bin, cfg_for_s(s), kP1BootBudget, kP1RunCycles, preloads);
    r.ok = r.vr.loaded > 0;
    return r;
}

}  // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks c;

    const char* mutate_c = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_c ? mutate_c : "";
    // A mutation's run captures only what it changes (its own proposition
    // group): the plain run (mv empty) still runs every group. (Named do_p*,
    // not run_p*, so as not to shadow the run_p5() function below.)
    const bool do_p3 = mv.empty() || mv.rfind("P3", 0) == 0;
    const bool do_p4 = mv.empty() || mv.rfind("P4", 0) == 0;
    const bool do_p1 = mv.empty() || mv.rfind("P1", 0) == 0;
    const bool do_p2 = mv.empty() || mv.rfind("P2", 0) == 0;
    const bool do_p5 = mv.empty() || mv.rfind("P5", 0) == 0;
    const bool mut_p3d          = mv == "P3D";
    const bool mut_p3a          = mv == "P3A";
    const bool mut_p3b          = mv == "P3B";
    const bool mut_p3c_lpendis  = mv == "P3C_LPENDIS";
    const bool mut_p3c_wrongstd = mv == "P3C_WRONGSTD";
    const bool mut_p4main       = mv == "P4MAIN";
    const bool mut_p4b_prog     = mv == "P4B_PROG";
    const bool mut_p4b_cop      = mv == "P4B_COP";
    const bool mut_p4a          = mv == "P4A";
    const bool mut_p4c          = mv == "P4C";

    const uint64_t win_end_p3b = mut_p3b ? kWinEndP3bMutant : kWinEnd;

    // ---------------------------------------------------------------
    // P3 -- BPLCON0 bit 3 (LPEN) enables the light-pen latch (p.271, p.89,
    // p.229, p.239). Claims: opus:A-bpc.b03.c0, sol:A-bplcon.c49,
    // opus:A-leg.lpen1, sol:A-bplcon.c72, opus:3-r0.c09, sol:3-regsum-bplcon0.c6
    // ---------------------------------------------------------------
    for (int s = 1; do_p3 && s <= 4; ++s) {
        const bool ntsc = s_is_ntsc(s);
        const bool ecs = s_is_ecs(s);
        nmh::Config cfg = cfg_for_s(s);

        std::string bin_l0 = lpen_bin(dir, s, false);
        std::string bin_l1 = lpen_bin(dir, s, true);
        if (ecs && s == 1 && mut_p3d) bin_l1 = dir + "/prog_lpen_ecs_pal_l1_mutd.bin";
        if (ecs && s == 1 && mut_p3a) bin_l1 = dir + "/prog_lpen_ecs_pal_l1_burst20.bin";
        if (ecs && s == 1 && mut_p3c_lpendis) bin_l1 = dir + "/prog_lpen_ecs_pal_l1_lpendis.bin";
        if (ecs && s == 1 && mut_p3c_wrongstd) bin_l0 = dir + "/prog_lpen_ecs_pal_l0_wrongstd.bin";
        const int burstlen = (ecs && s == 1 && mut_p3a) ? 20 : 600;

        Run r0 = run_program(bin_l0, cfg);
        Run r1 = run_program(bin_l1, cfg);
        const bool ok0 = r0.loaded && r0.released && r0.done.has_value();
        const bool ok1 = r1.loaded && r1.released && r1.done.has_value();

        c.expect(ok0 && ok1, std::string("P3 fixture S") + std::to_string(s) + " (" + s_name(s) +
                  "): both L0 and L1 programs loaded, released reset and reached DONE");

        auto det0 = extract_detections(r0.log, (ecs && s == 1 && mut_p3a) ? 600 : 600); // L0 never mutated for burst
        auto det1 = extract_detections(r1.log, burstlen);

        // Main proposition: in L1, every window (detections 2-5) holds one value.
        bool main_ok = ok1;
        int min_count_l1 = INT32_MAX;
        for (int i = 1; i <= 4 && i < (int)det1.size(); ++i) {
            WindowResult wr = analyze_window(det1[i], kWinStart, kWinEnd);
            main_ok = main_ok && wr.all_equal && wr.count > 0;
            min_count_l1 = std::min(min_count_l1, wr.count);
        }
        c.expect(main_ok,
            "opus:A-bpc.b03.c0 sol:A-bplcon.c49 opus:A-leg.lpen1 sol:A-bplcon.c72 opus:3-r0.c09 sol:3-regsum-bplcon0.c6 "
            "p271 p89 p229 p239 S" + std::to_string(s) + " (" + s_name(s) + "): with LPEN=1, every counted VPOSR/VHPOSR "
            "read in the windows after VERTB detections 2-5 shows the same 17-bit value (frozen)");

        // P3-a: >=100 reads per window, spread over it, in both runs.
        bool p3a_ok = ok0 && ok1;
        for (int i = 1; i <= 4; ++i) {
            if (i >= (int)det0.size() || i >= (int)det1.size()) { p3a_ok = false; continue; }
            WindowResult a = analyze_window(det0[i], kWinStart, kWinEnd);
            WindowResult b = analyze_window(det1[i], kWinStart, kWinEnd);
            p3a_ok = p3a_ok && a.has_min_100 && a.first_near_start && a.last_near_end
                             && b.has_min_100 && b.first_near_start && b.last_near_end;
        }
        c.expect(p3a_ok, "P3-a S" + std::to_string(s) + ": each window (L0 and L1) holds >=100 counted reads, "
                  "the first within 30 cc of the window start and the last within 30 cc of its end");

        // P3-b: L0's anchor (strictly increasing, V in 1..19) and field length in both runs.
        // Under NMH_MUTATE=P3B, win_end_p3b alone widens the window (t_f+30*227cc);
        // the unchanged v_in_1_19 requirement then goes red by itself once V exceeds
        // 19 inside the wider span -- no separate inversion of the verdict.
        bool p3b_ok = ok0 && ok1;
        for (int i = 1; i <= 4 && i < (int)det0.size(); ++i) {
            WindowResult a = analyze_window(det0[i], kWinStart, win_end_p3b);
            p3b_ok = p3b_ok && a.strictly_increasing && a.v_in_1_19;
        }
        for (int i = 0; i + 1 < (int)det0.size(); ++i)
            p3b_ok = p3b_ok && field_length_ok(det0[i + 1].marker_cycle - det0[i].marker_cycle, ntsc);
        for (int i = 0; i + 1 < (int)det1.size(); ++i)
            p3b_ok = p3b_ok && field_length_ok(det1[i + 1].marker_cycle - det1[i].marker_cycle, ntsc);
        c.expect(p3b_ok, "P3-b S" + std::to_string(s) + (mut_p3b ? " [window end -> t_f+30*227cc]" : "") +
                  ": in L0 the counted reads increase strictly with V between 1 and 19, and consecutive VERTB "
                  "detections in both runs recur at the configured field length");
        {
            // Disclosure for the tuned "+300" clk_sys margin in kFieldSlack
            // (SPECIFICATION.md build notes): the actual field-length deltas this
            // run measured, for every system configuration, so the margin's effect
            // stays auditable.
            std::string diag;
            for (int i = 1; i <= 4 && i < (int)det0.size(); ++i) {
                WindowResult a = analyze_window(det0[i], kWinStart, win_end_p3b);
                diag += "[w" + std::to_string(i) + " incr=" + (a.strictly_increasing ? "Y" : "N") +
                        " v1_19=" + (a.v_in_1_19 ? "Y" : "N") + " cnt=" + std::to_string(a.count) + "] ";
                if (!a.strictly_increasing)
                    diag += "[w" + std::to_string(i) + " first_violation " +
                            first_violation(det0[i], kWinStart, win_end_p3b) + "] ";
            }
            for (int i = 0; i + 1 < (int)det0.size(); ++i) {
                uint64_t delta = det0[i + 1].marker_cycle - det0[i].marker_cycle;
                diag += "[L0 d" + std::to_string(i) + "=" + std::to_string(delta) +
                        " ok=" + (field_length_ok(delta, ntsc) ? "Y" : "N") + "] ";
            }
            for (int i = 0; i + 1 < (int)det1.size(); ++i) {
                uint64_t delta = det1[i + 1].marker_cycle - det1[i].marker_cycle;
                diag += "[L1 d" + std::to_string(i) + "=" + std::to_string(delta) +
                        " ok=" + (field_length_ok(delta, ntsc) ? "Y" : "N") + "] ";
            }
            c.note("P3-b S" + std::to_string(s) + " diagnostic: " + diag);
        }

        // P3-c (ECS only): BEAMCON0 carries LPENDIS=0 and the configured standard's
        // bit 5 in both programs' own code, and the field-length check reports that
        // standard. The check itself is never touched by NMH_MUTATE: under
        // P3C_LPENDIS the L1 binary's own BEAMCON0 immediate genuinely carries
        // LPENDIS=1, so the unconditional (*val1 & 0x2000)==0 requirement goes red
        // by itself; under P3C_WRONGSTD the L0 binary's own bit 5 is genuinely
        // wrong, so the unconditional bit-5 and field-length checks (always against
        // the harness-configured standard) go red by themselves.
        if (ecs) {
            auto val0 = find_write_value(r0.prog_words, 0, 0x1DC);
            auto val1 = find_write_value(r1.prog_words, 0, 0x1DC);
            bool bit_ok = val0 && val1
                          && ((*val0 & 0x2000) == 0) && (((*val0 >> 5) & 1) == (ntsc ? 0 : 1))
                          && ((*val1 & 0x2000) == 0) && (((*val1 >> 5) & 1) == (ntsc ? 0 : 1));
            bool std_ok = true;
            for (int i = 0; i + 1 < (int)det0.size(); ++i)
                std_ok = std_ok && field_length_ok(det0[i + 1].marker_cycle - det0[i].marker_cycle, ntsc);
            c.expect(bit_ok && std_ok,
                "P3-c S" + std::to_string(s) + ": BEAMCON0 carries LPENDIS=0 and bit 5 for the configured standard "
                "in both programs' own code, and the field-length check reports that standard");
        }

        // P3-d: BPLCON0 written exactly once by the CPU program (its own step-3
        // write); no Copper fetch of a MOVE to $100 either (no Copper runs here).
        // Unconditional: under NMH_MUTATE=P3D the L1 binary genuinely contains a
        // second write, so n1==1 fails by itself.
        int n0 = count_writes_to(r0.prog_words, 0, 0x100);
        int n1 = count_writes_to(r1.prog_words, 0, 0x100);
        bool cop_clean = !copper_fetched_move_to(r0.log, kCopListAddr, 0x0100) &&
                         !copper_fetched_move_to(r1.log, kCopListAddr, 0x0100);
        c.expect(n0 == 1 && n1 == 1 && cop_clean,
            "P3-d S" + std::to_string(s) + ": BPLCON0 written exactly once by the program (no other CPU write to "
            "$DFF100, no Copper fetch of a MOVE to $100)");

        if (s == 1 && !det0.empty() && det0.size() > 1) {
            std::string dump;
            int shown = 0;
            for (auto& rc : det0[1].reads) {
                if (rc.first < det0[1].marker_cycle) continue;
                uint64_t rel = rc.first - det0[1].marker_cycle;
                if (rel >= kWinStart && rel <= kWinEnd && shown < 12) {
                    uint32_t mv = rc.second & 0x1FFFFu;
                    char buf[96];
                    std::snprintf(buf, sizeof buf, "[rel=%llu raw=%08X mv=%05X V=%u H=%u] ",
                        (unsigned long long)rel, rc.second, mv, (mv >> 8) & 0x1FFu, mv & 0xFFu);
                    dump += buf; ++shown;
                }
            }
            c.note("P3 S1 L0 window1 raw (first 12 in-window): " + dump);
        }

        c.note("P3 S" + std::to_string(s) + ": L1 min window count " +
               (min_count_l1 == INT32_MAX ? std::string("n/a") : std::to_string(min_count_l1)) +
               " reads; L0/L1 DONE at " + (r0.done ? std::to_string(*r0.done) : std::string("(none)")) + "/" +
               (r1.done ? std::to_string(*r1.done) : std::string("(none)")));
    }

    // ---------------------------------------------------------------
    // P4 -- LPEN is 0 after power-up (p.271). Claims: opus:A-leg.lpen2,
    // sol:A-bplcon.c73
    // ---------------------------------------------------------------
    for (int s = 1; do_p4 && s <= 4; ++s) {
        const bool ntsc = s_is_ntsc(s);
        const bool ecs = s_is_ecs(s);
        nmh::Config cfg = cfg_for_s(s);

        std::string bin = p4_bin(dir, s);
        bool use_copper_preload = false;
        if (ecs && s == 1 && mut_p4main) bin = dir + "/prog_lpen_ecs_pal_l1.bin";
        if (ecs && s == 1 && mut_p4b_prog) bin = dir + "/prog_lpen_ecs_pal_l1.bin";
        if (ecs && s == 1 && mut_p4a) bin = dir + "/prog_lpen_p4_ecs_pal_burst20.bin";
        if (ecs && s == 1 && mut_p4c) bin = dir + "/prog_lpen_p4_ecs_pal_wrongstd.bin";
        if (s == 1 && mut_p4b_cop) { bin = dir + "/prog_lpen_p4_copper.bin"; use_copper_preload = true; }
        const int burstlen = (ecs && s == 1 && mut_p4a) ? 20 : 600;

        std::vector<std::pair<uint32_t, std::vector<uint16_t>>> preloads;
        if (use_copper_preload) preloads.push_back({kCopListAddr, {0x0100, 0x0008, 0xFFFF, 0xFFFE}});

        Run r = run_program(bin, cfg, preloads);
        const bool ok = r.loaded && r.released && r.done.has_value();
        c.expect(ok, "P4 fixture S" + std::to_string(s) + " (" + s_name(s) +
                  "): program loaded, released reset and reached DONE");

        // Unconditional: under NMH_MUTATE=P4MAIN the loaded binary is genuinely
        // P3's L1 program (LPEN=1), whose VPOSR/VHPOSR reads are frozen, not
        // strictly increasing, so this_ok goes red by itself.
        auto det = extract_detections(r.log, burstlen);
        bool main_ok = ok;
        for (int i = 1; i <= 4 && i < (int)det.size(); ++i) {
            WindowResult wr = analyze_window(det[i], kWinStart, kWinEnd);
            bool this_ok = wr.strictly_increasing && wr.v_in_1_19 && wr.count > 0;
            main_ok = main_ok && this_ok;
        }
        c.expect(main_ok, "opus:A-leg.lpen2 sol:A-bplcon.c73 p271 S" + std::to_string(s) + " (" + s_name(s) +
                  (mut_p4main ? ", program is P3's L1" : "") +
                  "): after power-up, before any BPLCON0 write, VPOSR/VHPOSR count strictly with V in 1..19 "
                  "in every window after VERTB detections 2-5");

        // P4-a: as P3-a.
        bool p4a_ok = ok;
        for (int i = 1; i <= 4 && i < (int)det.size(); ++i) {
            WindowResult wr = analyze_window(det[i], kWinStart, kWinEnd);
            p4a_ok = p4a_ok && wr.has_min_100 && wr.first_near_start && wr.last_near_end;
        }
        c.expect(p4a_ok, "P4-a S" + std::to_string(s) + ": each window holds >=100 counted reads, spread over it");

        // P4-c (ECS only): as P3-c's field-length half, unconditional -- under
        // NMH_MUTATE=P4C the loaded binary's own BEAMCON0 bit 5 is genuinely wrong.
        if (ecs) {
            bool std_ok = true;
            for (int i = 0; i + 1 < (int)det.size(); ++i)
                std_ok = std_ok && field_length_ok(det[i + 1].marker_cycle - det[i].marker_cycle, ntsc);
            c.expect(std_ok,
                "P4-c S" + std::to_string(s) + ": the field-length check reports the configured standard");
        }

        // P4-b, program part: the program contains no write to $DFF100 at all.
        // Unconditional: under NMH_MUTATE=P4B_PROG the loaded binary is genuinely
        // P3's L1 program, which contains exactly one such write, so n==0 fails by
        // itself.
        int n = count_writes_to(r.prog_words, 0, 0x100);
        c.expect(n == 0, "P4-b(program) S" + std::to_string(s) +
                  (mut_p4b_prog ? " [P3's L1 program]" : "") +
                  ": the program contains no write to $DFF100");

        // P4-b, Copper part: no Copper instruction fetched is a MOVE to $100.
        // Unconditional: under NMH_MUTATE=P4B_COP the preloaded, DMA-started list
        // genuinely is one, so cop_clean is genuinely false and fails by itself.
        bool cop_clean = !copper_fetched_move_to(r.log, kCopListAddr, 0x0100);
        c.expect(cop_clean,
            "P4-b(copper) S" + std::to_string(s) + (use_copper_preload ? " [Copper list writes BPLCON0]" : "") +
            ": no Copper instruction fetched at the Chip-memory port is a MOVE to $100");

        c.note("P4 S" + std::to_string(s) + ": DONE at " +
               (r.done ? std::to_string(*r.done) : std::string("(none)")));
    }

    // ---------------------------------------------------------------
    // P1 -- BPLCON0 bit 9 (COLOR_ON) has no effect on the RGB output
    // (p.63-64). Claims: opus:3-a1000.c04, sol:3-composite.c6 (p.63),
    // opus:3-sum.c21 (p.64). Configurations built here: A (background only),
    // B (1 plane, low resolution). C (4 planes, high resolution) and E (bit
    // 9 changed mid-line by the Copper) are not built in this pass; see the
    // report.
    // ---------------------------------------------------------------
    const bool mut_p1main = mv == "P1MAIN";
    const bool mut_p1a    = mv == "P1A";
    const bool mut_p1c    = mv == "P1C";
    const bool mut_p1d    = mv == "P1D";
    const bool mut_p1e    = mv == "P1E";

    for (char cfg : {'A', 'B'}) {
        for (int s = 1; do_p1 && s <= 4; ++s) {
            const bool ntsc = s_is_ntsc(s);
            const int H = ntsc ? 200 : 256;
            const std::string tag = std::string(1, cfg) + " S" + std::to_string(s);

            std::string ov0, ov1;
            bool mutant_same_list = false;
            bool zero_bpl = false;
            if (s == 1 && mut_p1main) {
                ov1 = dir + (cfg == 'A' ? "/prog_p1_2c_pal_mutcolor00.bin" : "/prog_p1_2c_pal_mutcolor01.bin");
                mutant_same_list = true;
            }
            if (s == 1 && cfg == 'B' && mut_p1c) ov0 = ov1 = dir + "/prog_p1_2c_pal_nobplen.bin";
            if (s == 1 && cfg == 'B' && mut_p1d) zero_bpl = true;

            DisplayData dd = build_and_run_display(dir, cfg, s, ov0, ov1, mutant_same_list, zero_bpl);
            if (!dd.ok) { c.expect(false, "P1 fixture " + tag + ": both runs loaded"); continue; }

            auto starts0 = dma_field_starts(dd.run0.log);
            auto starts1 = dma_field_starts(dd.run1.log);
            auto vsa0 = find_vsync_assertions(dd.run0.video);
            auto vsa1 = find_vsync_assertions(dd.run1.video);
            if (starts0.empty() || starts1.empty()) {
                c.expect(false, "P1 fixture " + tag + ": the Copper never fetched the list");
                continue;
            }
            uint64_t fetch0 = starts0.front(), fetch1 = starts1.front();
            std::vector<size_t> w0, w1;
            for (auto v : vsa0) if ((uint64_t)v > fetch0) w0.push_back(v);
            for (auto v : vsa1) if ((uint64_t)v > fetch1) w1.push_back(v);

            bool fixture_ok = w0.size() >= 4 && w1.size() >= 4 && starts0.size() >= 3 && starts1.size() >= 3;
            c.expect(fixture_ok, "P1 fixture " + tag + ": at least 4 vsync assertions and 3 DMA-field starts "
                      "after the first Copper fetch, in both runs");
            if (!fixture_ok) continue;

            size_t b0 = w0[1], e0 = w0[3];  // 2nd..4th vsync assertion
            size_t b1 = w1[1], e1 = w1[3];
            size_t len0 = e0 - b0, len1 = e1 - b1;
            size_t cmp_len = std::min(len0, len1);

            bool p1a_ok = (len0 == len1);
            for (size_t i = 0; i < cmp_len && p1a_ok; ++i) {
                auto& x0 = dd.run0.video[b0 + i]; auto& x1 = dd.run1.video[b1 + i];
                if (x0.hs != x1.hs || x0.vs != x1.vs) p1a_ok = false;
            }
            // P1-a's own mutation (a dedicated PAL-vs-NTSC comparison) is graded in
            // the block below; nothing here swaps the stimulus for configs A/B at
            // s==1 under NMH_MUTATE=P1A, so p1a_ok is left unconditional.
            c.expect(p1a_ok,
                "P1-a " + tag + ": hsync and vsync samples of the two runs are identical over the compared span");

            size_t diffs = 0;
            for (size_t i = 0; i < cmp_len; ++i) {
                auto& x0 = dd.run0.video[b0 + i]; auto& x1 = dd.run1.video[b1 + i];
                if (x0.r != x1.r || x0.g != x1.g || x0.b != x1.b) ++diffs;
            }
            bool contrast_expected_red = (s == 1 && mut_p1main);
            char diffbuf[32]; std::snprintf(diffbuf, sizeof diffbuf, "%zu", diffs);
            // Unconditional: under NMH_MUTATE=P1MAIN run "1" genuinely uses a
            // program with a changed colour register (not bit 9) and the SAME
            // Copper list as run "0" (mutant_same_list), so diffs becomes genuinely
            // nonzero and diffs==0 fails by itself.
            c.expect(p1a_ok && diffs == 0,
                "opus:3-a1000.c04 sol:3-composite.c6 opus:3-sum.c21 p63 p64 config" + tag +
                (contrast_expected_red ? " [contrast: a colour register changed instead of bit 9]" : "") +
                ": every R,G,B sample of run \"1\" equals run \"0\" at the same index (" +
                std::string(diffbuf) + " differing samples)");

            size_t plane_lo = kBplPlane1 / 2, plane_hi = plane_lo + (size_t)20 * H;
            size_t expect_reads = (cfg == 'A') ? 0 : (size_t)20 * H;
            auto count_bpl_reads = [&](const VideoRun& r, uint64_t from, uint64_t to) {
                size_t n = 0;
                for (auto& a : r.log)
                    if (!a.is_write && a.cycle >= from && a.cycle < to && a.word_addr >= plane_lo && a.word_addr < plane_hi) ++n;
                return n;
            };
            // Each field independently re-fetches the whole buffer (the
            // Copper list reloads the same pointer every field), so the
            // count is graded per field, not summed across the span.
            size_t n0f2 = count_bpl_reads(dd.run0, starts0[1], starts0[2]);
            size_t n0f3 = count_bpl_reads(dd.run0, starts0[2], starts0[3]);
            size_t n1f2 = count_bpl_reads(dd.run1, starts1[1], starts1[2]);
            size_t n1f3 = count_bpl_reads(dd.run1, starts1[2], starts1[3]);
            // Unconditional: under NMH_MUTATE=P1C (config B, S1) both binaries are
            // genuinely the no-BPLEN variant, so the actual read counts are 0 and
            // the ==expect_reads requirement fails by itself (expect_reads stays
            // 20*H, unchanged).
            bool p1c_ok = (n0f2 == expect_reads && n0f3 == expect_reads &&
                           n1f2 == expect_reads && n1f3 == expect_reads);
            std::string p1c_counts = std::to_string(expect_reads) + " " + std::to_string(n0f2) + " " +
                std::to_string(n0f3) + " " + std::to_string(n1f2) + " " + std::to_string(n1f3);
            c.expect(p1c_ok,
                "P1-c " + tag + ": DMA fields 2 and 3 each show exactly N bitplane-buffer reads "
                "(expected/run0-field2/run0-field3/run1-field2/run1-field3 = " + p1c_counts + ")");

            auto fetch_ok = [&](const VideoRun& r, const CopBuild& cb, uint64_t from, uint64_t to) {
                for (auto& f : cb.bplcon0_fetches) {
                    int cnt = 0;
                    for (auto& a : r.log) {
                        if (a.is_write || a.word_addr != f.first || a.cycle < from || a.cycle >= to) continue;
                        ++cnt;
                        if (a.data != f.second) return false;
                    }
                    if (cnt < 2) return false;
                }
                return true;
            };
            bool p1b_ok = fetch_ok(dd.run0, dd.c0, b0, e0) &&
                          fetch_ok(dd.run1, mutant_same_list ? dd.c0 : dd.c1, b1, e1);
            c.expect(p1b_ok, "P1-b " + tag +
                ": the Copper's fetched MOVE to BPLCON0 carries the table's value in every field of the compared span");

            bool c0_0 = false, c1_0 = false, c0_1 = false, c1_1 = false;
            for (size_t i = 0; i < len0; ++i) { auto& x = dd.run0.video[b0 + i]; if (shows(x, kC0)) c0_0 = true; if (shows(x, kC1)) c1_0 = true; }
            for (size_t i = 0; i < len1; ++i) { auto& x = dd.run1.video[b1 + i]; if (shows(x, kC0)) c0_1 = true; if (shows(x, kC1)) c1_1 = true; }
            // Unconditional: under NMH_MUTATE=P1D (config B, S1) both runs' bitplane
            // buffer is genuinely filled with $0000, so c1_0/c1_1 become genuinely
            // false and the c1_0&&c1_1 requirement fails by itself.
            bool p1d_ok = (cfg == 'A') ? (c0_0 && c0_1) : (c0_0 && c1_0 && c0_1 && c1_1);
            c.expect(p1d_ok,
                "P1-d " + tag + ": the picture is not trivially uniform (the required colours occur)");

            bool p1e_ok = true;
            for (size_t i = 1; i < vsa0.size() && i < 8; ++i)
                p1e_ok = p1e_ok && field_length_ok(vsa0[i] - vsa0[i - 1], ntsc);
            c.expect(p1e_ok,  // P1-e's own mutation is graded in a dedicated block below
                "P1-e " + tag + ": consecutive vsync assertions recur at the configured standard's field length");
        }
    }

    // P1-a's own contrast: PAL for one run, NTSC for the other (config B, S1's
    // and S2's stimulus, both "run 0"), through the same unchanged comparator
    // the main loop's P1-a uses (hsync/vsync samples identical over the
    // 2nd-to-4th-vsync span) -- expected to fail by itself, since a genuine PAL
    // run and a genuine NTSC run have different field lengths.
    if (mut_p1a) {
        DisplayData dpal = build_and_run_display(dir, 'B', 1, "", "", true, false);
        DisplayData dntsc = build_and_run_display(dir, 'B', 2, "", "", true, false);
        bool ok = dpal.ok && dntsc.ok;
        bool p1a_ok = false;
        if (ok) {
            auto starts0 = dma_field_starts(dpal.run0.log);
            auto starts1 = dma_field_starts(dntsc.run0.log);
            auto vsa0 = find_vsync_assertions(dpal.run0.video);
            auto vsa1 = find_vsync_assertions(dntsc.run0.video);
            if (!starts0.empty() && !starts1.empty()) {
                uint64_t fetch0 = starts0.front(), fetch1 = starts1.front();
                std::vector<size_t> w0, w1;
                for (auto v : vsa0) if ((uint64_t)v > fetch0) w0.push_back(v);
                for (auto v : vsa1) if ((uint64_t)v > fetch1) w1.push_back(v);
                if (w0.size() >= 4 && w1.size() >= 4) {
                    size_t b0 = w0[1], e0 = w0[3], b1 = w1[1], e1 = w1[3];
                    size_t len0 = e0 - b0, len1 = e1 - b1;
                    size_t cmp_len = std::min(len0, len1);
                    p1a_ok = (len0 == len1);
                    for (size_t i = 0; i < cmp_len && p1a_ok; ++i) {
                        auto& x0 = dpal.run0.video[b0 + i]; auto& x1 = dntsc.run0.video[b1 + i];
                        if (x0.hs != x1.hs || x0.vs != x1.vs) p1a_ok = false;
                    }
                }
            }
        }
        c.expect(ok && p1a_ok, "P1-a [contrast: a genuine PAL run (S1) compared with a genuine NTSC run (S2), "
                  "config B run \"0\" of each] : hsync and vsync samples of the two runs are identical over the "
                  "compared span");
    }

    // P1-e's own contrast: no in-program stimulus reaches the video standard
    // (unlike P3/P4/P5, config B never writes BEAMCON0), so this runs the
    // unchanged field_length_ok comparator on a genuine PAL run's (config B, S1)
    // real vsync-interval deltas, graded against NTSC's values instead of PAL's --
    // the spec's own contrast ("check the field length against the other
    // standard's values; it fails").
    if (mut_p1e) {
        DisplayData dpal = build_and_run_display(dir, 'B', 1, "", "", true, false);
        bool ok = dpal.ok;
        auto vsa = ok ? find_vsync_assertions(dpal.run0.video) : std::vector<size_t>{};
        bool wrong_std_ok = ok && vsa.size() > 1;
        for (size_t i = 1; i < vsa.size() && i < 8 && wrong_std_ok; ++i)
            wrong_std_ok = wrong_std_ok && field_length_ok(vsa[i] - vsa[i - 1], /*ntsc=*/true);
        c.expect(ok && wrong_std_ok, "P1-e [contrast: a genuine PAL run's field length graded against NTSC's "
                  "values] B S1: consecutive vsync assertions recur at the configured standard's field length");
    }

    // ---------------------------------------------------------------
    // P2 -- the RGB output is the correct picture whether bit 9 is set or
    // clear (p.63). Claims: opus:3-a1000.c05, sol:3-composite.c7. Uses
    // config B (as the specification directs), a fresh run per system
    // configuration.
    // ---------------------------------------------------------------
    // P2's main proposition (items 1-3) is not built in this pass -- see the
    // report -- so only its fixture and its two graded preconditions, P2-a and
    // P2-b, take a mutation.
    const bool mut_p2b = mv == "P2B";

    for (int s = 1; do_p2 && s <= 4; ++s) {
        const bool ntsc = s_is_ntsc(s);
        const int H = ntsc ? 200 : 256;
        const std::string tag = std::string("B S") + std::to_string(s);
        std::string ov;
        if (s == 1 && mut_p2b) ov = dir + "/prog_p1_2c_pal_mod2.bin";

        DisplayData dd = build_and_run_display(dir, 'B', s, ov, ov, false, false);
        if (!dd.ok) { c.expect(false, "P2 fixture " + tag + ": both runs loaded"); continue; }

        auto starts0 = dma_field_starts(dd.run0.log);
        auto starts1 = dma_field_starts(dd.run1.log);
        auto vsa0 = find_vsync_assertions(dd.run0.video);
        auto vsa1 = find_vsync_assertions(dd.run1.video);
        if (starts0.size() < 6 || starts1.size() < 6 || vsa0.size() < 7 || vsa1.size() < 7) {
            c.expect(false, "P2 fixture " + tag + ": enough DMA fields and vsync assertions for a 4-field span");
            continue;
        }

        // P2-a: P1-b and P1-e.
        bool p2a_ok = true;
        {
            for (auto* dr : {&dd.run0, &dd.run1}) {
                bool this_ok = true;
                for (auto& f : (dr == &dd.run0 ? dd.c0 : dd.c1).bplcon0_fetches) {
                    int cnt = 0;
                    for (auto& a : dr->log) if (!a.is_write && a.word_addr == f.first) { ++cnt; if (a.data != f.second) this_ok = false; }
                    if (cnt < 4) this_ok = false;
                }
                p2a_ok = p2a_ok && this_ok;
            }
            for (size_t i = 1; i < vsa0.size() && i < 6; ++i) p2a_ok = p2a_ok && field_length_ok(vsa0[i] - vsa0[i - 1], ntsc);
        }
        c.expect(p2a_ok, "P2-a " + tag + ": P1-b and P1-e hold for the run");

        // P2-b: the buffer is read exactly once per word, in increasing
        // address order, within each run's graded span.
        auto p2b_check = [&](const VideoRun& r, uint64_t from, uint64_t to) {
            std::vector<uint64_t> addrs;
            for (auto& a : r.log)
                if (!a.is_write && a.cycle >= from && a.cycle < to && a.word_addr >= kBplPlane1 / 2 &&
                    a.word_addr < kBplPlane1 / 2 + (uint64_t)20 * H)
                    addrs.push_back(a.word_addr);
            if (addrs.size() != (size_t)20 * H) return false;
            for (size_t i = 1; i < addrs.size(); ++i) if (!(addrs[i] > addrs[i - 1])) return false;
            return true;
        };
        // Each of DMA fields 2-5 independently shows one pass (the Copper
        // list reloads the same pointer every field), so this is graded per
        // field, not summed across all four. Unconditional: under
        // NMH_MUTATE=P2B both binaries genuinely carry BPL1MOD=$0002, so later
        // lines start two bytes further out and addrs.size()!=20*H fails by itself.
        bool p2b_ok = true;
        for (int f = 1; f <= 4; ++f) {
            p2b_ok = p2b_ok && p2b_check(dd.run0, starts0[f], starts0[f + 1]) &&
                                p2b_check(dd.run1, starts1[f], starts1[f + 1]);
        }
        c.expect(p2b_ok, "P2-b " + tag + (s == 1 && mut_p2b ? " [contrast: BPL1MOD=$0002]" : "") +
                  ": the bitplane buffer is read exactly once per word, in increasing "
                  "address order, across the 4-field span");

        c.record("P2 " + tag + ": the main proposition (items 1-3: complete blocks of H lines, the bit-for-bit "
                  "picture, the C0 border samples) is not graded in this pass -- only its fixture and "
                  "preconditions P2-a/P2-b are. See the report.");
    }

    // ---------------------------------------------------------------
    // P5 -- VBSTRT ($1CC, ECS) and the line at which vertical blanking
    // starts. Claims: opus:B-vbstrt.addr (p.293), opus:C-vbstrt.c01,
    // sol:C-scan.c20 (p.304). Run in S1, S2 only (the specification's own
    // scope).
    // ---------------------------------------------------------------
    const bool mut_p5main = mv == "P5MAIN";
    const bool mut_p5a    = mv == "P5A";
    const bool mut_p5b    = mv == "P5B";
    const bool mut_p5c    = mv == "P5C";

    for (int s = 1; do_p5 && s <= 2; ++s) {
        const bool ntsc = s_is_ntsc(s);
        for (int vidx = 0; vidx < 2; ++vidx) {
            const uint8_t N = vidx == 0 ? 0xA0 : 0xB0;
            const std::string tag = "V" + std::to_string(vidx + 1) + " S" + std::to_string(s);
            std::string bin = p5_bin(dir, ntsc, vidx == 1);
            const bool here1 = (s == 1 && vidx == 0);
            if (here1 && mut_p5main) bin = dir + "/prog_p5_pal_v1_novarvben.bin";
            if (here1 && mut_p5a) bin = dir + "/prog_p5_pal_v1_nobplen.bin";
            if (here1 && mut_p5c) bin = dir + "/prog_p5_pal_v1_wrongstd.bin";

            P5Run pr = run_p5(bin, s, N);
            if (!pr.ok) { c.expect(false, "P5 fixture " + tag + ": program loaded"); continue; }

            auto starts = dma_field_starts(pr.vr.log);
            auto vsa = find_vsync_assertions(pr.vr.video);
            auto det = extract_detections(pr.vr.log, 200);
            bool fixture_ok = starts.size() >= 5 && vsa.size() >= 7 && det.size() >= 4;
            c.expect(fixture_ok, "P5 fixture " + tag + ": enough DMA-field starts, vsync assertions and "
                      "VERTB-synchronized read bursts");
            if (!fixture_ok) continue;

            uint64_t fetch0 = starts.front();
            std::vector<size_t> w;
            for (auto v : vsa) if ((uint64_t)v > fetch0) w.push_back(v);
            if (w.size() < 6) { c.expect(false, "P5 fixture " + tag + ": enough vsync assertions after the first Copper fetch"); continue; }

            // Beam lines (hsync-delimited, doubler copies merged) from the
            // 2nd to the 6th vsync assertion; per line, which of C1/C2/C3 a
            // sample shows.
            size_t b = w[1], e = w[5];
            auto raw_lines = nmh::find_lines(pr.vr.video);
            struct LC { bool c1 = false, c2 = false, c3 = false; };
            std::vector<LC> lines;
            for (size_t i = 0; i + 1 < raw_lines.size(); i += 2) {
                if (raw_lines[i].begin < b || raw_lines[i].end > e) continue;
                LC lc;
                for (int rep = 0; rep < 2; ++rep) {
                    nmh::Span sp = rep == 0 ? raw_lines[i] : raw_lines[i + 1];
                    for (size_t k = sp.begin; k < sp.end && k < pr.vr.video.size(); ++k) {
                        auto& samp = pr.vr.video[k];
                        if (shows(samp, kC1)) lc.c1 = true;
                        if (shows(samp, kC2)) lc.c2 = true;
                        if (shows(samp, kC3)) lc.c3 = true;
                    }
                }
                lines.push_back(lc);
            }

            bool item1_ok = true;
            int c2_count = 0;
            std::vector<size_t> c2_pos;
            for (size_t i = 0; i < lines.size(); ++i) {
                if (!lines[i].c2) continue;
                ++c2_count; c2_pos.push_back(i);
                bool prev_ok = i > 0 && lines[i - 1].c1 && !lines[i - 1].c2;
                bool next_ok = i + 1 < lines.size() && !lines[i + 1].c1 && !lines[i + 1].c2 && !lines[i + 1].c3;
                if (!prev_ok || !next_ok) item1_ok = false;
            }
            bool item2_ok = c2_count >= 3;
            for (size_t i = 1; i < c2_pos.size(); ++i) {
                size_t delta = c2_pos[i] - c2_pos[i - 1];
                if (!(ntsc ? (delta == 262 || delta == 263) : (delta == 312 || delta == 313))) item2_ok = false;
            }
            // Unconditional: under NMH_MUTATE=P5MAIN the loaded binary genuinely
            // lacks VARVBEN, so blanking never starts at N and item1_ok goes red by
            // itself (line N also shows C2, breaking next_ok).
            bool main_ok = item1_ok && item2_ok;
            c.expect(main_ok,
                "opus:B-vbstrt.addr opus:C-vbstrt.c01 sol:C-scan.c20 p293 p304 p305 p79 " + tag +
                (here1 && mut_p5main ? " [contrast: BEAMCON0 without VARVBEN]" : "") +
                ": every beam line showing C2 is immediately preceded by a line showing C1 (not C2) and "
                "immediately followed by a line showing none of C1/C2/C3; at least 3 such lines, one field apart");

            // P5-a: beam lines N-10..N-2 (9 lines immediately before the
            // first C2 line, which is beam line N-1) each show C1. Unconditional:
            // under NMH_MUTATE=P5A the loaded binary genuinely has DMACON without
            // BPLEN, so no line shows C1 and p5a_ok goes red by itself.
            int idx_c2 = c2_pos.empty() ? -1 : (int)c2_pos.front();
            bool p5a_ok = idx_c2 >= 9;
            if (p5a_ok) for (int i = idx_c2 - 9; i < idx_c2; ++i) if (!lines[i].c1) p5a_ok = false;
            c.expect(p5a_ok, "P5-a " + tag + (here1 && mut_p5a ? " [contrast: DMACON without BPLEN]" : "") +
                ": beam lines N-10..N-2 each show C1");

            // P5-b (anchor): around each of VERTB detections 2-4, the last
            // VPOSR read before the Copper's fetch of the COLOR01=C2 data
            // word, and the first read after it, both show V=N-1; likewise
            // C3 with V=N+1.
            bool p5b_ok = true;
            for (int i = 1; i <= 3 && i < (int)det.size(); ++i) {
                uint64_t t0 = det[i].marker_cycle;
                uint64_t t1 = (i + 1 < (int)det.size()) ? det[i + 1].marker_cycle : t0 + 400000;
                auto find_fetch = [&](uint32_t addr) -> std::optional<uint64_t> {
                    for (auto& a : pr.vr.log)
                        if (!a.is_write && a.word_addr == addr && a.cycle >= t0 && a.cycle < t1) return a.cycle;
                    return std::nullopt;
                };
                auto v_of = [&](uint32_t v32) { return (uint8_t)(((v32 & 0x1FFFFu) >> 8) & 0x1FFu); };
                auto check_anchor = [&](uint32_t addr, uint8_t expect_v) {
                    auto tc = find_fetch(addr);
                    if (!tc) return false;
                    uint64_t last_before = 0; bool have_before = false;
                    uint64_t first_after = 0; bool have_after = false;
                    uint32_t val_before = 0, val_after = 0;
                    for (auto& rc : det[i].reads) {
                        if (rc.first < *tc) { if (!have_before || rc.first > last_before) { last_before = rc.first; val_before = rc.second; have_before = true; } }
                        else { if (!have_after || rc.first < first_after) { first_after = rc.first; val_after = rc.second; have_after = true; } }
                    }
                    return have_before && have_after && v_of(val_before) == expect_v && v_of(val_after) == expect_v;
                };
                p5b_ok = p5b_ok && check_anchor(pr.cop.addr_c2, (uint8_t)(N - 1)) && check_anchor(pr.cop.addr_c3, (uint8_t)(N + 1));
            }
            c.expect(p5b_ok, "P5-b " + tag + ": the VPOSR/VHPOSR reads bracketing the Copper's COLOR01 change "
                      "show V=N-1 (before/at the C2 change) and V=N+1 (before/at the C3 change)");

            // P5-c: field length matches the configured standard. Unconditional:
            // under NMH_MUTATE=P5C the loaded binary's own BEAMCON0 bit 5 is
            // genuinely wrong.
            bool p5c_ok = true;
            for (size_t i = 1; i < vsa.size() && i < 8; ++i)
                p5c_ok = p5c_ok && field_length_ok(vsa[i] - vsa[i - 1], ntsc);
            c.expect(p5c_ok,
                "P5-c " + tag + ": consecutive vsync assertions recur at the configured standard's field length");
        }
    }

    // P5-b's own contrast, exactly as SPECIFICATION.md states it: "replace
    // WAIT(N-1,$30) by WAIT(N-2,$30); the reads around t_c2 show V=N-2." Reuses
    // V1/PAL's program (VBSTRT stays N=$A0 in the assembled binary) with a Copper
    // list built directly here: only the WAIT before the COLOR01=C2 change moves
    // to N-2=$9E; the WAIT before C3 stays at N+1=$A1, unchanged. The unchanged
    // anchor check (V=N-1=$9F around the C2 fetch) then finds the real beam
    // position one line early and fails by itself; the C3-side anchor, unaffected,
    // still passes.
    if (mut_p5b) {
        P5Cop cop;
        cop_move(cop.list, 0x0E0, uint16_t(kBplPlane1 >> 16));
        cop_move(cop.list, 0x0E2, uint16_t(kBplPlane1 & 0xFFFF));
        cop_move(cop.list, 0x182, kC1);
        cop_wait(cop.list, 0x9E, 0x30);  // WAIT(N-2,$30) instead of WAIT(N-1,$30)
        cop.addr_c2 = kCopListAddr / 2 + (uint32_t)cop.list.size() + 1;
        cop_move(cop.list, 0x182, kC2);
        cop_wait(cop.list, 0xA1, 0x30);  // WAIT(N+1,$30), unchanged
        cop.addr_c3 = kCopListAddr / 2 + (uint32_t)cop.list.size() + 1;
        cop_move(cop.list, 0x182, kC3);
        cop_end(cop.list);
        std::vector<uint16_t> bitplane(20 * 196, 0xFFFF);
        std::vector<std::pair<uint32_t, std::vector<uint16_t>>> preloads = {
            {kBplPlane1, bitplane}, {kCopListAddr, cop.list}};
        VideoRun vr = run_video(dir + "/prog_p5_pal_v1.bin", cfg_for_s(1), kP1BootBudget, kP1RunCycles, preloads);
        auto det = extract_detections(vr.log, 200);
        bool ok = vr.loaded > 0 && det.size() >= 4;
        bool p5b_ok = ok;
        for (int i = 1; i <= 3 && i < (int)det.size(); ++i) {
            uint64_t t0 = det[i].marker_cycle;
            uint64_t t1 = (i + 1 < (int)det.size()) ? det[i + 1].marker_cycle : t0 + 400000;
            auto find_fetch = [&](uint32_t addr) -> std::optional<uint64_t> {
                for (auto& a : vr.log)
                    if (!a.is_write && a.word_addr == addr && a.cycle >= t0 && a.cycle < t1) return a.cycle;
                return std::nullopt;
            };
            auto v_of = [&](uint32_t v32) { return (uint8_t)(((v32 & 0x1FFFFu) >> 8) & 0x1FFu); };
            auto check_anchor = [&](uint32_t addr, uint8_t expect_v) {
                auto tc = find_fetch(addr);
                if (!tc) return false;
                uint64_t last_before = 0; bool have_before = false;
                uint64_t first_after = 0; bool have_after = false;
                uint32_t val_before = 0, val_after = 0;
                for (auto& rc : det[i].reads) {
                    if (rc.first < *tc) { if (!have_before || rc.first > last_before) { last_before = rc.first; val_before = rc.second; have_before = true; } }
                    else { if (!have_after || rc.first < first_after) { first_after = rc.first; val_after = rc.second; have_after = true; } }
                }
                return have_before && have_after && v_of(val_before) == expect_v && v_of(val_after) == expect_v;
            };
            p5b_ok = p5b_ok && check_anchor(cop.addr_c2, 0x9F) && check_anchor(cop.addr_c3, 0xA1);
        }
        c.expect(ok && p5b_ok, "P5-b [contrast: Copper WAIT(N-1,$30) before COLOR01=C2 replaced by WAIT(N-2,$30), "
                  "program's VBSTRT stays N] V1 S1: the VPOSR/VHPOSR reads bracketing the Copper's COLOR01 change "
                  "show V=N-1 (before/at the C2 change) and V=N+1 (before/at the C3 change)");
    }

    std::printf("%d passed, %d failed\n", c.passed, c.failed);
    return c.failed == 0 ? 0 : 1;
}
