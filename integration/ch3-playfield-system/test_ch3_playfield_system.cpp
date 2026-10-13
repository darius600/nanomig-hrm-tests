// test_ch3_playfield_system.cpp
//
// ch3-playfield-system: see SPECIFICATION.md.
//
// Test purpose: grade the playfield system bits of the printed pages at the system top: BPLCON0 bit 9 does not
//   change the RGB output (P1) and the RGB output is the picture the registers and bitplane data define (P2),
//   BPLCON0 bit 3 (LPEN) latches the beam position read through VPOSR/VHPOSR (P3) and is 0 after power-up (P4),
//   and VBSTRT sets the line at which vertical blanking starts (P5).
// Test class: HRM_CONFORMANCE for every graded line; the fixture and calibration lines (P1..P5 fixture, CAL-CC,
//   CAL-VIDEO, P3-L, P4-L) are INTEGRATION_SMOKE; MEASURED lines grade nothing.
// Primary oracle: Amiga Hardware Reference Manual, 3rd edition, the printed pages named in SPECIFICATION.md.
// Requirement IDs: none
// Non-goals: the composite video output, what a read of VBSTRT returns, the frozen value of the light pen latch,
//   the release of the latch after vertical position 20, what vertical blanking shows on the RGB output.
//
// P3 (BPLCON0 bit 3, LPEN) and P4 (LPEN after power-up) share one program family (prog_lpen_*.s /
// prog_lpen_core.s): five VERTB-synchronized bursts of 600 long reads of VPOSR/VHPOSR, stored to MEASURE_BASE.
// The burst length the block parser and P3-M/P4-M expect is the one the loaded program names in its own
// MOVE.W #n-1,D5.
// P3 grades LPEN=1 against LPEN=0, P4 grades the power-up state (BPLCON0 never written). P5 (prog_p5_*.s)
// reads VPOSR/VHPOSR the same way. P1 and P2 (prog_p1_*.s) start a preloaded Copper list and grade the video output.
//
// A CPU write to a custom register (BPLCON0, BEAMCON0) never reaches the Chip-memory port -- only chip-RAM
// traffic (CPU or Copper) does. Where a precondition is about "the program contains [no] write to $DFF1xx",
// this file scans the loaded program's own machine code for the one instruction form every source here uses
// for such a write, MOVE.W #imm,d16(An): (opcode & 0xF1FF) == 0x317C, An in bits 11-9, the immediate in the
// next word and the displacement in the word after it. The immediate is also looked up in the CPU's own
// instruction fetches at the port. A Copper-issued write to the same register is a normal chip-RAM read (the
// Copper list itself) and is checked on the RAM log instead.
// A CPU read of a custom register does not reach the port either, so the period of the VERTB polling loop,
// which the field-length check adds to its tolerance, is measured from the loop's own instruction fetches.
//
// Aggregate lines carry the claim ids; the component lines they are built from carry none. A line whose
// precondition failed prints RECORDED ... NOT GRADED, never PASS or FAIL. An aggregate prints FAIL as soon as one
// graded component failed, PASS when every component was graded and passed, and is otherwise not graded.
//
// NMH_MUTATE selects one mutated scenario in place of its normal counterpart, in the first system configuration
// only, and runs the group it belongs to; NMH_ONLY (a list of group names such as P3P4P5) restricts a plain run
// to those groups. Neither is exercised by a plain `make test`. Every mutation runs the package's comparator
// unchanged: the only NMH_MUTATE-conditioned code in this file selects which binary, Copper list or memory
// contents is fed to that comparator, never the pass/fail meaning of a clause.
//   P3FIX         prog_lpen_ecs_pal_l1_badstart.bin (the L1 program of S1 stores another start word)
//   P3MARK        prog_lpen_ecs_pal_l1_badmark.bin (every marker stored with bit 8 set)
//   P3A           prog_lpen_ecs_pal_l1_burst20.bin (20 reads, not 600)
//   P3L           prog_lpen_ecs_pal_l0_nopoll.bin (no INTREQR polling loop)
//   P3BLATE      prog_lpen_ecs_pal_l0_late.bin (the marker store about 26 lines after the VERTB detection)
//   P3C_LPENDIS   prog_lpen_ecs_pal_l1_lpendis.bin (LPENDIS=1)
//   P3C_WRONGSTD  prog_lpen_ecs_pal_l0_wrongstd.bin (BEAMCON0 bit 5 wrong)
//   P3D           prog_lpen_ecs_pal_l1_mutd.bin (extra BPLCON0 write)
//   P3MAIN        the L0 program stands in for the L1 program
//   P3LIVE        the L1 program stands in for the L0 program
//   P4FIX         prog_lpen_p4_ecs_pal_badstart.bin (the program of S1 stores another start word)
//   P4MARK        prog_lpen_p4_ecs_pal_badmark.bin (every marker stored with bit 8 set)
//   P4A          prog_lpen_p4_ecs_pal_burst20.bin
//   P4L           prog_lpen_p4_ecs_pal_nopoll.bin
//   P4C           prog_lpen_p4_ecs_pal_wrongstd.bin
//   P4B_PROG      prog_lpen_ecs_pal_l1.bin (a program that writes BPLCON0)
//   P4B_COP       prog_lpen_p4_copper.bin (+Copper preload)
//   P1FIX         run "1" of every configuration at S1 stores another start word (prog_p1_2c_pal_badstart.bin,
//                 prog_p1_16c_pal_badstart.bin)
//   P1A           a genuine PAL run's sync samples compared with a genuine NTSC run's
//   P1B           run "1" loads run "0"'s Copper list
//   P1C           DDFSTOP 8 colour clocks lower with bitplane DMA on (B, C, E: prog_p1_2c_pal_narrow.bin,
//                 prog_p1_16c_pal_narrow.bin), or A given a list that also points sprite 0 at the bitplane
//                 buffer and turns sprite DMA on (a transparent sprite stored there)
//   P1D           bitplane buffers zeroed (B, C, E), or COLOR00 set to another value by the Copper (A)
//   P1ESTD       prog_p1_2c_ntsc_wrongstd.bin in configuration B at S2 (BEAMCON0 bit 5 opposite the standard)
//   P1MAIN        run "1" also changes a colour register half way down the picture
//   P2FIX         run "1" of configuration B at S1 stores another start word (prog_p1_2c_pal_badstart.bin)
//   P2A           run "1" loads run "0"'s Copper list
//   P2B           prog_p1_2c_pal_mod2.bin (BPL1MOD=$0002), with 20 more lines of valid data words after the buffer
//   P2FLIP        bit 14 of w(100,5) flipped in memory, the expectation unchanged
//   P2ITEM1       prog_p1_2c_pal_vstoplow.bin (VSTOP one line lower)
//   P2EDGE        prog_p1_2c_pal_rightedge.bin (window 32 pixels wider, 21 words per line)
//   P5FIX         prog_p5_pal_v1_badstart.bin (the V1 program of S1 stores another start word)
//   P5MARK        prog_p5_pal_v1_badmark.bin (every marker stored with bit 8 set)
//   P5A          prog_p5_pal_v1_nobplen.bin
//   P5B           Copper WAIT(N-1,$30) before COLOR01=C2 replaced by WAIT(N-2,$30)
//   P5C           prog_p5_pal_v1_wrongstd.bin
//   P5MAIN        prog_p5_pal_v1_novarvben.bin
//
// P4-MAIN waits for P3-MAIN of the same system configuration, so it is graded only in a run that executes group P3
// (a plain run, or NMH_ONLY with P3 and P4); a mutation run of a P4 selector prints it NOT GRADED.
//
// The private chipset of the implementation records is a copy of the chipset sources outside the repository
// with three changes, each a reading of the printed pages: (1) BPLCON0 bit 3 enables a latch of the beam
// position at the end of the field, held until vertical position 20, and BEAMCON0 bit 13 makes a read ignore
// it (pp.239, 305), (2) the vertical position counts at the clock edge at which the horizontal position wraps,
// (3) with BEAMCON0 bit 12 set, VBSTRT starts and VBSTOP ends vertical blanking (pp.293, 304, 305). A second
// private copy differs in one thing: the latch is enabled after reset.
//
// MUTATION CAL-CC none impossible the 7 MHz clock enable at the port comes from the clock generator and no register write or program changes its period; the line measures that period before any program runs
// MUTATION CAL-VIDEO stimulus red NMH_MUTATE=P5C: the P5 V1 S1 program writes BEAMCON0 with bit 5 opposite the configured standard, the field has the other standard's line count and CAL-VIDEO V1 S1 prints FAIL; P5-c V1 S1 prints FAIL and P5-MAIN V1 S1 prints NOT GRADED
// MUTATION P3 stimulus red NMH_MUTATE=P3FIX: the L1 program of S1 stores a start word other than $5A03 and otherwise runs unchanged; both runs complete, the fixture line prints FAIL and every other P3 line of S1 prints NOT GRADED
// MUTATION P3-M stimulus red NMH_MUTATE=P3MARK: the L1 program of S1 stores every marker with bit 8 set; the fixture holds, P3-M S1 prints FAIL and P3-a, P3-b, P3-c, P3-LIVE and P3-MAIN of S1 print NOT GRADED
// MUTATION P3-a stimulus red NMH_MUTATE=P3A: 20 reads per burst instead of 600, fewer than 100 fall in the window; P3-LIVE S1 and P3-MAIN S1 print NOT GRADED
// MUTATION P3-L stimulus red NMH_MUTATE=P3L: the L0 program waits with a fixed delay, no INTREQR polling loop is seen in its fetches; P3-b S1, P3-c S1 and P3-MAIN S1 print NOT GRADED
// MUTATION P3-b stimulus red NMH_MUTATE=P3BLATE: the L0 program stores its marker about 26 lines after the VERTB detection, the reads have V above 19 and the line prints FAIL; P3-MAIN S1 prints NOT GRADED
// MUTATION P3-LIVE implementation green private chipset (vertical position counts at the horizontal wrap, no pair with V unchanged and H lower): P3-LIVE passes in S1-S4 on both cores; on the repository chipset, where V steps after the wrap, it passes as well
// MUTATION P3-LIVE stimulus red NMH_MUTATE=P3LIVE on the private chipset: the L1 program stands in for the L0 program, the L0 windows hold one frozen value and the line prints FAIL; P3-b S1 prints FAIL too (the frozen value has V above 19) and P3-MAIN S1 prints NOT GRADED
// MUTATION P3-c stimulus red NMH_MUTATE=P3C_LPENDIS: BEAMCON0 written with LPENDIS=1; P3-MAIN S1 prints NOT GRADED
// MUTATION P3-c stimulus red NMH_MUTATE=P3C_WRONGSTD: BEAMCON0 bit 5 opposite the configured standard; P3-b S1 prints FAIL through its field-length part and P3-MAIN S1 prints NOT GRADED
// MUTATION P3-d stimulus red NMH_MUTATE=P3D: an extra BPLCON0 write after the first VERTB detection; P3-MAIN S1 prints NOT GRADED
// MUTATION P3-MAIN implementation green private chipset (light pen latch): P3-MAIN passes in S1-S4 on both cores
// MUTATION P3-MAIN stimulus red NMH_MUTATE=P3MAIN on the private chipset: the L0 program stands in for L1, the reads count and the line prints FAIL
// MUTATION opus:A-bpc.b03.c0 implementation green private chipset (light pen latch): the aggregate passes on both cores
// MUTATION opus:A-bpc.b03.c0 stimulus red NMH_MUTATE=P3MAIN on the private chipset: the L0 program stands in for L1 in S1 and the aggregate prints FAIL
// MUTATION opus:A-bpc.b03.c0 stimulus red NMH_MUTATE=P3LIVE on the private chipset: the L1 program stands in for L0 in S1 and the aggregate prints FAIL through P3-LIVE
// MUTATION P4 stimulus red NMH_MUTATE=P4FIX: the program of S1 stores a start word other than $5A03 and otherwise runs unchanged; the run completes, the fixture line prints FAIL and every other P4 line of S1 prints NOT GRADED
// MUTATION P4-M stimulus red NMH_MUTATE=P4MARK: the program of S1 stores every marker with bit 8 set; the fixture holds, P4-M S1 prints FAIL and P4-a, P4-c and P4-MAIN of S1 print NOT GRADED
// MUTATION P4-a stimulus red NMH_MUTATE=P4A: 20 reads per burst instead of 600
// MUTATION P4-L stimulus red NMH_MUTATE=P4L: the program waits with a fixed delay, no INTREQR polling loop is seen in its fetches; P4-c S1 prints NOT GRADED
// MUTATION P4-c stimulus red NMH_MUTATE=P4C: BEAMCON0 bit 5 opposite the configured standard
// MUTATION P4-B-PROGRAM stimulus red NMH_MUTATE=P4B_PROG: the program is P3's L1 program, which writes BPLCON0
// MUTATION P4-B-COPPER stimulus red NMH_MUTATE=P4B_COP: a preloaded Copper list with a MOVE to BPLCON0 is started
// MUTATION P4-MAIN implementation green private chipset (light pen latch, vertical position counts at the horizontal wrap): P3-MAIN passes there and P4-MAIN passes in S1-S4 on both cores (NMH_ONLY=P3P4P5)
// MUTATION P4-MAIN implementation red second private chipset in which LPEN is 1 after reset: P3-MAIN passes, every other precondition holds and the line prints FAIL in S1-S4 on both cores (NMH_ONLY=P3P4)
// MUTATION opus:A-leg.lpen2 implementation green private chipset (light pen latch, vertical position counts at the horizontal wrap): the aggregate passes on both cores
// MUTATION opus:A-leg.lpen2 implementation red second private chipset in which LPEN is 1 after reset: the aggregate prints FAIL on both cores
// MUTATION P1 stimulus red NMH_MUTATE=P1FIX: run "1" of every configuration at S1 stores a start word other than $5A01 and otherwise runs unchanged; both runs complete, the fixture line prints FAIL and P1-MAIN prints NOT GRADED
// MUTATION P1-a stimulus red NMH_MUTATE=P1A: a genuine PAL run's sync samples are compared with a genuine NTSC run's; the contrast's own fixture line prints PASS
// MUTATION P1-b stimulus red NMH_MUTATE=P1B: run "1" loads run "0"'s Copper list, so the fetched BPLCON0 values are run "0"'s; P1-MAIN prints NOT GRADED
// MUTATION P1-c stimulus red NMH_MUTATE=P1C: DDFSTOP is 8 colour clocks lower with bitplane DMA on in B, C and E (each DMA field still reads a nonempty part of the buffers), and in A the Copper list also points sprite 0 at the buffer and turns sprite DMA on, so reads of the buffer's address range occur with the BPLCON0 words unchanged; P1-c prints FAIL in A, B, C and E, every other P1 line holds, and P1-MAIN prints NOT GRADED
// MUTATION P1-d stimulus red NMH_MUTATE=P1D: bitplane buffers zeroed in B, C and E, and COLOR00 set to another value by the Copper in A; P1-MAIN prints NOT GRADED
// MUTATION P1-e stimulus red NMH_MUTATE=P1ESTD (S2 only): the program of configuration B writes BEAMCON0 with bit 5 opposite the configured standard and the field length is the other standard's; the 200-line window fits the longer field, P1-c B holds, P1-e B S2 prints FAIL and P1-MAIN B S2 prints NOT GRADED
// MUTATION P1-MAIN stimulus red NMH_MUTATE=P1MAIN: run "1" keeps its bit-9 change and also sets a colour register half way down the picture, in A, B, C and E, every precondition holding
// MUTATION opus:3-a1000.c04 stimulus red NMH_MUTATE=P1MAIN: run "1" keeps its bit-9 change and also sets a colour register half way down the picture, the aggregate prints FAIL
// MUTATION P2 stimulus red NMH_MUTATE=P2FIX: run "1" of configuration B at S1 stores a start word other than $5A01 and otherwise runs unchanged; the fixture line prints FAIL and P2-MAIN prints NOT GRADED
// MUTATION P2-a stimulus red NMH_MUTATE=P2A: run "1" loads run "0"'s Copper list, so the fetched BPLCON0 values are run "0"'s; P2-I1, P2-I2 and P2-I3 print NOT GRADED
// MUTATION P2-b stimulus red NMH_MUTATE=P2B: BPL1MOD=$0002, later lines start two bytes further out, and 20 more lines of valid data words follow the buffer so every line still shows data; P2-I1 holds, P2-I2 and the P2 aggregate print NOT GRADED
// MUTATION P2-I1 stimulus red NMH_MUTATE=P2ITEM1: VSTOP one line lower, a data block has H-1 lines; P2-b prints FAIL too (the buffer is read for H-1 lines) and P2-I2 prints NOT GRADED
// MUTATION P2-I2 stimulus red NMH_MUTATE=P2FLIP: bit 14 of w(100,5) flipped in memory while the expectation is unchanged
// MUTATION P2-I3 stimulus red NMH_MUTATE=P2EDGE: the window is 32 pixels wider and 21 words per line are fetched, so the sample after the 320th pixel shows data; P2-b prints FAIL too and P2-I2 prints NOT GRADED
// MUTATION opus:3-a1000.c05 stimulus red NMH_MUTATE=P2FLIP: bit 14 of w(100,5) flipped in memory, the aggregate prints FAIL
// MUTATION P5 stimulus red NMH_MUTATE=P5FIX: the V1 program of S1 stores a start word other than $5A05 and otherwise runs unchanged; the run completes, the fixture line prints FAIL and P5-MAIN V1 S1 prints NOT GRADED
// MUTATION P5-M stimulus red NMH_MUTATE=P5MARK: the V1 program of S1 stores every marker with bit 8 set; the fixture holds, P5-M V1 S1 prints FAIL and P5-b, P5-c and P5-MAIN print NOT GRADED
// MUTATION P5-a stimulus red NMH_MUTATE=P5A: DMACON without BPLEN, no line shows C1; P5-MAIN prints NOT GRADED
// MUTATION P5-b stimulus red NMH_MUTATE=P5B: Copper WAIT(N-1,$30) before COLOR01=C2 replaced by WAIT(N-2,$30), the C2-side anchor finds V=N-2; P5-MAIN prints NOT GRADED
// MUTATION P5-c stimulus red NMH_MUTATE=P5C: BEAMCON0 bit 5 opposite the configured standard; CAL-VIDEO V1 S1 prints FAIL too and P5-MAIN prints NOT GRADED
// MUTATION P5-MAIN implementation green private chipset (VBSTRT blanking): P5-MAIN passes in V1 and V2, S1 and S2 on both cores
// MUTATION P5-MAIN stimulus red NMH_MUTATE=P5MAIN on the private chipset: BEAMCON0 without VARVBEN, line N also shows C2 and the line prints FAIL
// MUTATION opus:B-vbstrt.addr implementation green private chipset (VBSTRT blanking): the aggregate passes on both cores
// MUTATION opus:B-vbstrt.addr stimulus red NMH_MUTATE=P5MAIN on the private chipset: BEAMCON0 without VARVBEN, the aggregate prints FAIL

#include <algorithm>
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
// The start word every program stores right after its start-up writes (SPECIFICATION.md, Common definitions).
constexpr uint32_t kStartAddr = 0x0C0FF0;
constexpr uint16_t kStartP1 = 0x5A01, kStartLpen = 0x5A03, kStartP5 = 0x5A05;

constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudget  = 6'000'000;

// One colour clock in clk_sys cycles. p.24: "One horizontal count takes one
// cycle of the system clock (processor is twice this)": the colour clock is
// two processor clocks. The processor clock is the 7 MHz clock whose enable
// the design exports at the port clk7_en; the run measures its period at the
// port (CAL-CC below) and sets this value from it before anything else uses it.
uint64_t g_cc = 8;

uint64_t win_start()          { return 228 * g_cc; }       // t_f + 228 cc
uint64_t win_end()            { return 18 * 227 * g_cc; }   // t_f + 18*227 cc
uint64_t spread_slack()       { return 30 * g_cc; }         // 30 cc, P3-a

// Field-length check (SPECIFICATION.md, Common definitions): the time between
// two consecutive detections equals one of the page's two field lengths
// (PAL 312/313 lines of 227 cc = 70,824/71,051 cc; NTSC 262/263 lines =
// 59,605/59,833 cc) "within one line (227 cc) plus the period of the detecting
// loop". The period of the detecting loop is measured from the program's own
// instruction fetches (poll_loop below) and asserted (P3-L, P4-L); a vsync
// detection has no loop and passes 0.
bool field_length_ok(uint64_t delta, bool ntsc, uint64_t loop_cycles = 0) {
    const uint64_t slack = 227 * g_cc + loop_cycles;
    const uint64_t lo = (ntsc ? 59605ull : 70824ull) * g_cc;
    const uint64_t hi = (ntsc ? 59833ull : 71051ull) * g_cc;
    return delta + slack >= lo && delta <= hi + slack;
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

// An aggregate line carries a claim; its components carry none. It prints FAIL
// as soon as one graded component failed, PASS when every expected component
// was graded and passed, and is otherwise not graded.
struct Agg {
    int expected = 0, graded = 0, failed = 0;
    void add(bool was_graded, bool pass) {
        if (was_graded) { ++graded; if (!pass) ++failed; }
    }
    bool gradable() const { return failed > 0 || graded == expected; }
    std::string counts() const {
        return std::to_string(graded) + " of " + std::to_string(expected) + " components graded, " +
               std::to_string(failed) + " failed";
    }
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
bool cfg_is_ecs(const nmh::Config& c) { return (c.chipset_config & 0x08) != 0; }
std::string s_name(int s) {
    return std::string(s_is_ecs(s) ? "ECS" : "OCS") + "/" + (s_is_ntsc(s) ? "NTSC" : "PAL");
}

// CAL-CC: the period of the 7 MHz clock enable at the port clk7_en, in clk_sys
// cycles.
struct Clk7 {
    bool regular = false;
    uint64_t period = 0;
    int pulses = 0;
};
Clk7 measure_clk7() {
    Clk7 r;
    nmh::NanomigHarness h{nmh::Config{}};
    h.boot(200);  // drives por and reset; the clock enables run from there
    std::vector<uint64_t> at;
    for (uint64_t i = 0; i < 400; ++i) {
        h.run_cycles(1);
        if (h.ports()->clk7_en) at.push_back(i);
    }
    r.pulses = (int)at.size();
    if (at.size() >= 8) {
        r.period = at[1] - at[0];
        r.regular = r.period > 0;
        for (size_t i = 1; i < at.size(); ++i)
            if (at[i] - at[i - 1] != r.period) r.regular = false;
    }
    return r;
}

struct LoopStat {
    size_t iterations = 0;   // loop iterations seen (consecutive fetches closer than one line)
    uint64_t period = 0;     // the longest such interval, in clk_sys cycles
};

// True when the log holds a store of `value` to the Chip-memory byte address `addr`.
bool saw_store(const std::vector<nmh::RamAccess>& log, uint32_t addr, uint16_t value) {
    for (auto& a : log)
        if (a.is_write && a.word_addr == addr / 2 && a.data == value) return true;
    return false;
}

struct Run {
    size_t loaded = 0;
    bool released = false;
    bool started = false;              // the program's start word was stored
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;   // coalesced full log (reads and writes)
    std::vector<uint16_t> prog_words;  // the loaded program's own image
    LoopStat loop;
};

// The detecting loop of the VERTB programs is `MOVE.W INTREQR(A0),D0`
// (opcode $3028, displacement $001E) / BTST / BEQ. A CPU read of a custom
// register never reaches the Chip-memory port, but the loop's own instruction
// fetches do: the interval between consecutive fetches of its first word is
// the loop's period. Intervals of one line (227 cc, p.23) or more are the
// waits between bursts, not iterations.
LoopStat poll_loop(const std::vector<uint16_t>& prog, const std::vector<nmh::RamAccess>& log,
                   uint16_t opcode = 0x3028, uint16_t disp = 0x001E) {
    LoopStat ls;
    size_t at = SIZE_MAX;
    for (size_t i = 0; i + 1 < prog.size(); ++i)
        if (prog[i] == opcode && prog[i + 1] == disp) { at = i; break; }
    if (at == SIZE_MAX) return ls;
    const uint32_t wa = nmh::kProgramOrg / 2 + (uint32_t)at;
    uint64_t prev = 0;
    bool have = false;
    for (auto& a : log) {
        if (a.is_write || a.word_addr != wa || a.data != opcode) continue;
        if (have) {
            uint64_t d = a.cycle - prev;
            if (d < 227 * g_cc) { ++ls.iterations; ls.period = std::max(ls.period, d); }
        }
        prev = a.cycle;
        have = true;
    }
    return ls;
}

// preload: physical byte address (even, <$80000 is fine -- see playfield.i)
// -> words to poke there before boot(), for a program that only points a
// register at the address (a CPU write there would be lost to the overlay).
Run run_program(const std::string& bin, const nmh::Config& cfg, uint16_t want_start,
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
    r.started = saw_store(r.log, kStartAddr, want_start);
    r.loop = poll_loop(r.prog_words, r.log);
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

// The immediate word of that same instruction as the CPU fetched it at the Chip-memory port: the data of the
// first read at the word address the immediate has in the program image.
std::optional<uint16_t> fetched_write_value(const std::vector<uint16_t>& prog, const std::vector<nmh::RamAccess>& log,
                                            uint8_t an, uint16_t disp) {
    for (size_t i = 0; i + 2 < prog.size(); ++i) {
        uint16_t op = prog[i];
        if ((op & 0xF1FF) == 0x317C && ((op >> 9) & 7) == an && prog[i + 2] == disp) {
            const uint32_t wa = nmh::kProgramOrg / 2 + (uint32_t)(i + 1);
            for (auto& a : log)
                if (!a.is_write && a.word_addr == wa) return (uint16_t)a.data;
            return std::nullopt;
        }
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
    std::vector<std::pair<uint64_t, uint32_t>> reads;   // (cycle of the low-half store, value)
    std::vector<uint64_t> lower;                         // per read: the cycle of the store before it (the marker's, or the
                                                         // previous read's low-half store); the read lies between the two
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
        uint64_t prev_store = d.marker_cycle;
        for (int k = 0; k < burstlen && idx + 1 < buf.size(); ++k) {
            uint32_t hi = buf[idx].data; ++idx;
            uint32_t lo = buf[idx].data; uint64_t c_lo = buf[idx].cycle; ++idx;
            d.reads.push_back({c_lo, (hi << 16) | lo});
            d.lower.push_back(prev_store);
            prev_store = c_lo;
        }
        out.push_back(d);
    }
    return out;
}

// The burst length a program names: its `MOVE.W #n-1,D5` (opcode $3A3C, the immediate in the next word), n. 0 when
// the image holds no such instruction.
int program_burst_len(const std::vector<uint16_t>& prog) {
    for (size_t i = 0; i + 1 < prog.size(); ++i)
        if (prog[i] == 0x3A3C) return (int)prog[i + 1] + 1;
    return 0;
}

// P3-M, P4-M, P5-M: the stores to the measurement buffer are exactly `nblocks` blocks, block k being the
// marker word k followed by `burstlen` long words, each stored high half then low half, all at consecutive word
// addresses from the buffer's first.
bool blocks_shape_ok(const std::vector<nmh::RamAccess>& log, int burstlen, int nblocks) {
    const uint32_t base_word = kMeasureBase / 2;
    const size_t block_words = 1u + 2u * (size_t)burstlen;
    const size_t total = block_words * (size_t)nblocks;
    std::vector<nmh::RamAccess> buf;
    for (auto& a : log)
        if (a.is_write && a.word_addr >= base_word && a.word_addr < base_word + 5u * block_words) buf.push_back(a);
    if (buf.size() != total) return false;
    for (size_t i = 0; i < buf.size(); ++i) {
        if (buf[i].word_addr != base_word + i) return false;
        if (i % block_words == 0 && buf[i].data != (uint16_t)(i / block_words)) return false;
    }
    return true;
}

uint32_t v_of17(uint32_t mv) { return (mv >> 8) & 0x1FFu; }   // V8..V0
uint32_t h_of17(uint32_t mv) { return mv & 0xFFu; }            // H8..H1

struct WindowResult {
    int count = 0;
    bool has_min_100 = false;
    bool first_near_start = false;
    bool last_near_end = false;
    bool running = true;      // the three page-faithful criteria below all hold
    bool all_equal = true;
    bool v_in_1_19 = true;
    int pairs = 0;            // consecutive counted reads
    int equal_pairs = 0;      // (i) pairs of two equal 17-bit values (p.239: "If both values are not the same")
    int v_bad_pairs = 0;      // (ii) pairs in which V decreases or steps by more than 1
    int h_extra_dec = 0;      // (iii) pairs with V unchanged and H lower beyond the first one of each V value
    int lag_pairs = 0;        // MEASURED: pairs with V8..V0 unchanged and H8..H1 lower (H wrapped, V not yet stepped)
    int v_over_19 = 0;        // counted reads with V above 19
    std::vector<std::pair<uint32_t, uint32_t>> lags;   // those pairs' (H of the first read, H of the second)
};

WindowResult analyze_window(const Detection& d, uint64_t win_s, uint64_t win_e) {
    WindowResult wr;
    std::vector<std::pair<uint64_t, uint32_t>> in;
    // A read lies inside the window when both stores that bound it do (SPECIFICATION.md, Common definitions).
    for (size_t i = 0; i < d.reads.size(); ++i) {
        const auto& rc = d.reads[i];
        if (d.lower[i] < d.marker_cycle) continue;
        const uint64_t rel_lo = d.lower[i] - d.marker_cycle, rel_hi = rc.first - d.marker_cycle;
        if (rel_lo >= win_s && rel_hi <= win_e) in.push_back(rc);
    }
    wr.count = (int)in.size();
    wr.has_min_100 = wr.count >= 100;
    if (!in.empty()) {
        uint64_t rel_first = in.front().first - d.marker_cycle;
        uint64_t rel_last = in.back().first - d.marker_cycle;
        wr.first_near_start = (rel_first - win_s) <= spread_slack();
        wr.last_near_end = (win_e - rel_last) <= spread_slack();
    }
    bool first = true;
    uint32_t prev = 0;
    int dec_in_this_v = 0;
    for (auto& rc : in) {
        uint32_t mv = rc.second & 0x1FFFFu;
        uint32_t V = v_of17(mv);
        if (V < 1 || V > 19) wr.v_in_1_19 = false;
        if (V > 19) ++wr.v_over_19;
        if (!first) {
            ++wr.pairs;
            const uint32_t pV = v_of17(prev);
            if (mv == prev) ++wr.equal_pairs;                                       // (i)
            if (V < pV || V > pV + 1) ++wr.v_bad_pairs;                             // (ii)
            if (V != pV) dec_in_this_v = 0;
            else if (h_of17(mv) < h_of17(prev)) {                                   // (iii), and the MEASURED lag pairs
                ++wr.lag_pairs;
                wr.lags.push_back({h_of17(prev), h_of17(mv)});
                if (++dec_in_this_v > 1) ++wr.h_extra_dec;
            }
            if (mv != prev) wr.all_equal = false;
        }
        prev = mv; first = false;
    }
    wr.running = wr.count > 0 && wr.equal_pairs == 0 && wr.v_bad_pairs == 0 && wr.h_extra_dec == 0;
    return wr;
}

// MEASURED text: the pairs of consecutive counted reads with V unchanged and H lower, as "H a->b xN".
std::string lag_text(const std::vector<WindowResult>& ws) {
    std::vector<std::pair<std::pair<uint32_t, uint32_t>, int>> tally;
    for (auto& w : ws)
        for (auto& l : w.lags) {
            bool found = false;
            for (auto& t : tally) if (t.first == l) { ++t.second; found = true; break; }
            if (!found) tally.push_back({l, 1});
        }
    if (tally.empty()) return "none";
    std::string s;
    for (auto& t : tally)
        s += "H " + std::to_string(t.first.first) + "->" + std::to_string(t.first.second) + " x" + std::to_string(t.second) + " ";
    return s;
}

// MEASURED text: why a window is not a running counter, per criterion.
std::string pf_text(const WindowResult& a) {
    return "reads=" + std::to_string(a.count) + " pairs=" + std::to_string(a.pairs) + " equal=" + std::to_string(a.equal_pairs) +
           " V-bad=" + std::to_string(a.v_bad_pairs) + " H-extra-decreases=" + std::to_string(a.h_extra_dec) +
           " V-unchanged-H-lower=" + std::to_string(a.lag_pairs) + " V>19=" + std::to_string(a.v_over_19);
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
constexpr uint16_t kC1Visible = 0x0EA3;  // C1 with blue bit 0 set (P1 contrast)
constexpr uint16_t kC0Visible = 0x0358;  // C0 with blue bit 0 cleared (P1 contrast)
constexpr uint16_t kPalette16[16] = {0x0359, 0x0EA2, 0x0F3C, 0x04E7, 0x0A0F, 0x00F5, 0x0FF0, 0x0888,
                                      0x0C63, 0x036C, 0x09E1, 0x0E19, 0x05A5, 0x0D7B, 0x01BD, 0x0B42};

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

// What the pages say the Copper's MOVEs to BPLCON0 carry, in the order the
// list executes them within one field (SPECIFICATION.md P1, table and
// configuration E). It is the oracle for P1-b and does not depend on the list
// a run loads.
std::vector<uint16_t> expected_bplcon0(char cfg, int run) {
    std::vector<uint16_t> v;
    switch (cfg) {
        case 'A': v.push_back(run ? 0x0200 : 0x0000); break;
        case 'B': v.push_back(run ? 0x1200 : 0x1000); break;
        case 'C': v.push_back(run ? 0xC200 : 0xC000); break;
        default:
            v.push_back(0x1000);
            for (int L = 0x2C; L <= 0xFE; ++L) v.push_back(run ? ((L & 1) ? 0x1200 : 0x1000) : 0x1000);
    }
    return v;
}

// The Copper list of one run. contrast: the list also changes one colour
// register half way down the picture (a change the pages say is visible);
// colour_top (config A): COLOR00 is set to another value for the whole field;
// force: build the list of that configuration instead; sprite_dma (config A): the list also points sprite 0 at
// kBplPlane1 and turns sprite DMA on, so that a non-bitplane DMA channel reads the bitplane buffer's address range.
std::vector<uint16_t> build_cop(char cfg, int run, bool contrast, bool colour_top, char force, bool sprite_dma = false) {
    std::vector<uint16_t> l;
    const char eff = force ? force : cfg;
    const uint16_t v = expected_bplcon0(eff, run)[0];
    switch (eff) {
        case 'A':
            cop_move(l, 0x100, v);
            if (sprite_dma) {
                cop_move(l, 0x120, uint16_t(kBplPlane1 >> 16));
                cop_move(l, 0x122, uint16_t(kBplPlane1 & 0xFFFF));
                cop_move(l, 0x096, 0x8020);
            }
            if (colour_top) cop_move(l, 0x180, kC0Visible);
            if (contrast) {
                cop_move(l, 0x180, kC0);
                cop_wait(l, 0x80, 0x00);
                cop_move(l, 0x180, kC0Visible);
            }
            break;
        case 'B':
            cop_move(l, 0x100, v);
            cop_move(l, 0x0E0, uint16_t(kBplPlane1 >> 16));
            cop_move(l, 0x0E2, uint16_t(kBplPlane1 & 0xFFFF));
            if (contrast) {
                cop_move(l, 0x182, kC1);
                cop_wait(l, 0x80, 0x00);
                cop_move(l, 0x182, kC1Visible);
            }
            break;
        case 'C': {
            cop_move(l, 0x100, v);
            const uint32_t planes[4] = {kBplPlane1, kBplPlane2, kBplPlane3, kBplPlane4};
            const uint16_t ph[4] = {0x0E0, 0x0E4, 0x0E8, 0x0EC}, pl[4] = {0x0E2, 0x0E6, 0x0EA, 0x0EE};
            for (int i = 0; i < 4; ++i) {
                cop_move(l, ph[i], uint16_t(planes[i] >> 16));
                cop_move(l, pl[i], uint16_t(planes[i] & 0xFFFF));
            }
            if (contrast) {
                cop_move(l, 0x182, kC1);
                cop_wait(l, 0x80, 0x00);
                cop_move(l, 0x182, kC1Visible);
            }
            break;
        }
        default: {
            cop_move(l, 0x100, 0x1000);
            cop_move(l, 0x0E0, uint16_t(kBplPlane1 >> 16));
            cop_move(l, 0x0E2, uint16_t(kBplPlane1 & 0xFFFF));
            if (contrast) cop_move(l, 0x182, kC1);
            for (int L = 0x2C; L <= 0xFE; ++L) {
                cop_wait(l, (uint8_t)L, 0x80);
                cop_move(l, 0x100, run ? ((L & 1) ? 0x1200 : 0x1000) : 0x1000);
                if (contrast && L == 0x80) cop_move(l, 0x182, kC1Visible);
            }
        }
    }
    cop_end(l);
    return l;
}

// DMA field boundaries (P1-c): consecutive Copper fetches of the list's own
// first word (always the BPLCON0 MOVE's register-address word, $0100, at
// kCopListAddr).
std::vector<uint64_t> dma_field_starts(const std::vector<nmh::RamAccess>& log) {
    std::vector<uint64_t> out;
    for (auto& a : log) if (!a.is_write && a.word_addr == kCopListAddr / 2) out.push_back(a.cycle);
    return out;
}

// The data words of the Copper's fetched MOVEs to BPLCON0 in [from,to): a
// fetched first word $0100 at an even offset of the list, followed by the
// fetch of the next word.
std::vector<uint16_t> fetched_bplcon0(const std::vector<nmh::RamAccess>& log, uint64_t from, uint64_t to) {
    std::vector<uint16_t> out;
    const uint32_t base = kCopListAddr / 2;
    int64_t pending = -1;
    for (auto& a : log) {
        if (a.is_write || a.cycle < from || a.cycle >= to) continue;
        if (a.word_addr < base || a.word_addr >= base + 4096) continue;
        const uint32_t off = a.word_addr - base;
        if (pending >= 0 && (int64_t)off == pending) { out.push_back(a.data); pending = -1; }
        else if ((off & 1) == 0 && a.data == 0x0100) pending = (int64_t)off + 1;
        else pending = -1;
    }
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
    bool started = false;              // the program's start word was stored
    bool done = false;                 // the program's DONE write was stored
    std::vector<uint16_t> prog_words;  // the loaded program's own image
    std::vector<nmh::RamAccess> log;
    std::vector<nmh::VideoSample> video;
};

VideoRun run_video(const std::string& bin, const nmh::Config& cfg, uint64_t boot_budget, uint64_t run_cycles,
                    uint16_t want_start, const std::vector<std::pair<uint32_t, std::vector<uint16_t>>>& preloads) {
    VideoRun r;
    nmh::NanomigHarness h(cfg);
    for (auto& pl : preloads)
        for (size_t i = 0; i < pl.second.size(); ++i) h.poke_word(pl.first / 2 + i, pl.second[i]);
    r.loaded = h.load_program(bin);
    if (!r.loaded) return r;
    for (size_t i = 0; i < r.loaded; i += 2) {
        auto w = h.peek_logical(nmh::kProgramOrg + i);
        r.prog_words.push_back(w.value_or(0xDEAD));
    }
    r.released = h.boot(boot_budget).cpu_released;
    h.run_cycles(run_cycles);
    r.log = nmh::coalesce(h.ram_log());
    r.started = saw_store(r.log, kStartAddr, want_start);
    r.done = saw_store(r.log, kDoneAddr, kDoneValue);
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

// What a display pair's stimulus changes relative to the nominal one.
struct DispOpts {
    std::string ov0, ov1;            // program overrides ("" = the nominal program)
    bool contrast1 = false;          // run "1": the list also changes a colour register mid-picture
    bool list1_is_list0 = false;     // run "1" loads run "0"'s Copper list
    bool zero_bitplane = false;      // bitplane buffers filled with $0000
    int flip_word_index = -1;        // XOR bit 14 into this word of plane 1 (both runs)
    char force_list = 0;             // build the list of this configuration for both runs
    bool a_colour_top = false;       // config A: COLOR00 set to another value by the Copper
    bool a_sprite_dma = false;       // config A: sprite 0 reads a transparent sprite stored at kBplPlane1
    int pad_lines = 0;               // configs B, E: this many more lines of valid data words after the buffer
};

struct DisplayData {
    VideoRun run0, run1;
    int H = 0;
    bool ok = false;
};

// Builds and runs one display configuration's pair of runs ("0"/"1").
DisplayData build_and_run_display(const std::string& dir, char cfgname, int s, const DispOpts& o) {
    DisplayData d;
    bool ntsc = s_is_ntsc(s);
    d.H = ntsc ? 200 : 256;
    bool palette16 = (cfgname == 'C');
    std::string bin0 = o.ov0.empty() ? p1_bin(dir, palette16, ntsc) : o.ov0;
    std::string bin1 = o.ov1.empty() ? p1_bin(dir, palette16, ntsc) : o.ov1;

    std::vector<std::pair<uint32_t, std::vector<uint16_t>>> bpl;
    if (cfgname == 'B' || cfgname == 'E') {
        std::vector<uint16_t> data = o.zero_bitplane ? std::vector<uint16_t>((size_t)20 * d.H, 0)
                                                     : bitplane_be(d.H + o.pad_lines);
        if (o.flip_word_index >= 0 && (size_t)o.flip_word_index < data.size()) data[o.flip_word_index] ^= 0x4000;
        bpl.push_back({kBplPlane1, data});
    } else if (cfgname == 'C') {
        const uint32_t planes[4] = {kBplPlane1, kBplPlane2, kBplPlane3, kBplPlane4};
        for (int p = 1; p <= 4; ++p)
            bpl.push_back({planes[p - 1], o.zero_bitplane ? std::vector<uint16_t>((size_t)40 * d.H, 0) : bitplane_c(p, d.H)});
    } else if (o.a_sprite_dma) {
        // a transparent sprite: position and control words, 64 lines of two zero words, the end marker
        std::vector<uint16_t> spr(2 + 2 * 64 + 2, 0);
        spr[0] = 0x5040; spr[1] = 0x9000;
        bpl.push_back({kBplPlane1, spr});
    } else if (o.force_list == 'B') {
        bpl.push_back({kBplPlane1, bitplane_be(d.H)});
    }

    auto list0 = build_cop(cfgname, 0, false, o.a_colour_top, o.force_list, o.a_sprite_dma);
    auto list1 = o.list1_is_list0 ? list0 : build_cop(cfgname, 1, o.contrast1, o.a_colour_top, o.force_list, o.a_sprite_dma);
    auto preloads0 = bpl; preloads0.push_back({kCopListAddr, list0});
    auto preloads1 = bpl; preloads1.push_back({kCopListAddr, list1});

    d.run0 = run_video(bin0, cfg_for_s(s), kP1BootBudget, kP1RunCycles, kStartP1, preloads0);
    d.run1 = run_video(bin1, cfg_for_s(s), kP1BootBudget, kP1RunCycles, kStartP1, preloads1);
    d.ok = d.run0.loaded > 0 && d.run1.loaded > 0 && d.run0.released && d.run1.released &&
           d.run0.started && d.run1.started;
    return d;
}

// P1-b: in every complete DMA field that overlaps [b,e) (two fields at least),
// the Copper's fetched MOVEs to BPLCON0 carry the table's values, in order.
bool bplcon0_fetches_ok(const VideoRun& r, const std::vector<uint64_t>& starts, uint64_t b, uint64_t e,
                         const std::vector<uint16_t>& want, size_t* fields_checked = nullptr) {
    size_t n = 0;
    bool ok = true;
    for (size_t f = 0; f + 1 < starts.size(); ++f) {
        if (!(starts[f + 1] > b && starts[f] < e)) continue;
        if (fetched_bplcon0(r.log, starts[f], starts[f + 1]) != want) ok = false;
        ++n;
    }
    if (fields_checked) *fields_checked = n;
    return ok && n >= 2;
}

// P1-a: the hsync and vsync samples of [b0,e0) of one trace equal those of [b1,e1) of the other, sample by sample.
bool sync_samples_identical(const std::vector<nmh::VideoSample>& v0, size_t b0, size_t e0,
                            const std::vector<nmh::VideoSample>& v1, size_t b1, size_t e1) {
    if (e0 - b0 != e1 - b1) return false;
    for (size_t i = 0; i < e0 - b0; ++i)
        if (v0[b0 + i].hs != v1[b1 + i].hs || v0[b0 + i].vs != v1[b1 + i].vs) return false;
    return true;
}

// P1-e: consecutive vsync assertions recur at the configured standard's field length.
bool vsync_field_length_ok(const std::vector<size_t>& vsa, bool ntsc) {
    bool ok = vsa.size() > 1;
    for (size_t i = 1; i < vsa.size() && i < 8; ++i) ok = ok && field_length_ok(vsa[i] - vsa[i - 1], ntsc);
    return ok;
}

// Calibration of the scan doubler's units (SPECIFICATION.md, Video output):
// the output has m hs periods per beam line and k samples per low-resolution
// pixel. Both are checked against the pages in the run: the hs periods between
// consecutive vsync assertions number m times the page's line count of one
// field (p.24: 312/313 PAL, 262/263 NTSC); a doubler line is the m-th part of
// a beam line of 227 or 228 cc (p.23); a low-resolution pixel is half a colour
// clock (p.239: "2 low resolution pixels" per horizontal count), so k is the
// doubler line length divided by the 2*227 pixels of a line.
constexpr int kDoublerM = 2;
struct VideoCal {
    bool ok = false;
    int k = 0;
    size_t median_len = 0;
};
VideoCal calibrate_video(const std::vector<nmh::VideoSample>& v, const std::vector<size_t>& vsa, bool ntsc) {
    VideoCal vc;
    if (vsa.size() < 2) return vc;
    const size_t lo = ntsc ? 262 : 312, hi = ntsc ? 263 : 313;
    auto lines = nmh::find_lines(v);
    bool counts_ok = true;
    for (size_t i = 0; i + 1 < vsa.size(); ++i) {
        size_t n = 0;
        for (auto& sp : lines) if (sp.begin >= vsa[i] && sp.begin < vsa[i + 1]) ++n;
        if (!(n == kDoublerM * lo || n == kDoublerM * hi)) counts_ok = false;
    }
    std::vector<size_t> lens;
    for (auto& sp : lines) if (sp.begin >= vsa.front() && sp.end <= vsa.back()) lens.push_back(sp.end - sp.begin);
    bool len_ok = !lens.empty();
    for (auto l : lens) if (!(kDoublerM * l == 227 * g_cc || kDoublerM * l == 228 * g_cc)) len_ok = false;
    if (!lens.empty()) {
        std::sort(lens.begin(), lens.end());
        vc.median_len = lens[lens.size() / 2];
        vc.k = (int)((vc.median_len + 227) / (2 * 227));
    }
    vc.ok = counts_ok && len_ok && vc.k >= 1;
    return vc;
}

// A doubler line (one hs period) of the span [b,e) and what it shows.
struct DLine {
    size_t begin = 0, end = 0;
    bool c1 = false;
    size_t first_c1 = 0;   // sample index of the first sample showing C1
};
std::vector<DLine> doubler_lines(const std::vector<nmh::VideoSample>& v, size_t b, size_t e) {
    std::vector<DLine> out;
    for (auto& sp : nmh::find_lines(v)) {
        if (sp.begin < b || sp.end > e) continue;
        DLine d;
        d.begin = sp.begin; d.end = sp.end;
        for (size_t k = sp.begin; k < sp.end && k < v.size(); ++k)
            if (shows(v[k], kC1)) { d.c1 = true; d.first_c1 = k; break; }
        out.push_back(d);
    }
    return out;
}

// P2 main (SPECIFICATION.md P2, items 1-3), one run.
struct P2Eval {
    size_t blocks = 0, complete = 0;
    bool i1 = false, i2 = false, i3 = false;
    size_t first_bad_block = 0, first_bad_line = 0;
    std::vector<size_t> complete_lens;
};
P2Eval eval_p2(const VideoRun& r, size_t b, size_t e, int H, int k) {
    P2Eval ev;
    auto dl = doubler_lines(r.video, b, e);
    struct Blk { size_t start, len; };
    std::vector<Blk> blocks;
    for (size_t i = 0; i < dl.size();) {
        if (!dl[i].c1) { ++i; continue; }
        size_t j = i;
        while (j < dl.size() && dl[j].c1) ++j;
        blocks.push_back({i, j - i});
        i = j;
    }
    ev.blocks = blocks.size();
    bool lens_ok = true, pix_ok = true, border_ok = true;
    for (size_t bi = 0; bi < blocks.size(); ++bi) {
        const Blk& bk = blocks[bi];
        const bool complete = bk.start > 0 && bk.start + bk.len < dl.size();
        if (!complete) continue;
        ++ev.complete;
        ev.complete_lens.push_back(bk.len);
        if (bk.len != (size_t)kDoublerM * (size_t)H) lens_ok = false;
        for (size_t d = 0; d < bk.len; ++d) {
            const DLine& ln = dl[bk.start + d];
            const int j = (int)(d / kDoublerM);
            const size_t s0 = ln.first_c1;
            bool line_pix = s0 + (size_t)k * 320 <= ln.end;
            for (int i = 0; line_pix && i < 320; ++i) {
                const bool bit = (w_be(j, i / 16) >> (15 - (i % 16))) & 1;
                for (int q = 0; q < k && line_pix; ++q)
                    if (!shows(r.video[s0 + (size_t)k * i + q], bit ? kC1 : kC0)) line_pix = false;
            }
            if (!line_pix) { if (pix_ok) { ev.first_bad_block = bi; ev.first_bad_line = d; } pix_ok = false; }
            const bool left = s0 > ln.begin && shows(r.video[s0 - 1], kC0);
            const bool right = s0 + (size_t)k * 320 < ln.end && shows(r.video[s0 + (size_t)k * 320], kC0);
            if (!(left && right)) border_ok = false;
        }
    }
    ev.i1 = ev.complete >= 3 && lens_ok;
    ev.i2 = ev.complete >= 3 && pix_ok;
    ev.i3 = ev.complete >= 3 && border_ok;
    return ev;
}

// -----------------------------------------------------------------------
// P5 -- VBSTRT ($1CC, ECS) and the line at which vertical blanking starts.
// -----------------------------------------------------------------------

struct P5Cop {
    std::vector<uint16_t> list;
    uint32_t addr_c2 = 0;  // word address of the MOVE COLOR01,C2 data word
    uint32_t addr_c3 = 0;  // word address of the MOVE COLOR01,C3 data word
};

// marker_n is the vertical position the WAITs (and thus the anchor, P5-b) use.
P5Cop build_cop_P5(uint8_t wait_c2, uint8_t wait_c3) {
    P5Cop cb;
    cop_move(cb.list, 0x0E0, uint16_t(kBplPlane1 >> 16));
    cop_move(cb.list, 0x0E2, uint16_t(kBplPlane1 & 0xFFFF));
    cop_move(cb.list, 0x182, kC1);
    cop_wait(cb.list, wait_c2, 0x30);
    cb.addr_c2 = kCopListAddr / 2 + (uint32_t)cb.list.size() + 1;
    cop_move(cb.list, 0x182, kC2);
    cop_wait(cb.list, wait_c3, 0x30);
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

P5Run run_p5(const std::string& bin, const nmh::Config& cfg, uint8_t wait_c2, uint8_t wait_c3) {
    P5Run r;
    r.cop = build_cop_P5(wait_c2, wait_c3);
    std::vector<uint16_t> bitplane(20 * 196, 0xFFFF);
    std::vector<std::pair<uint32_t, std::vector<uint16_t>>> preloads = {
        {kBplPlane1, bitplane}, {kCopListAddr, r.cop.list}};
    r.vr = run_video(bin, cfg, kP1BootBudget, kP1RunCycles, kStartP5, preloads);
    r.ok = r.vr.loaded > 0 && r.vr.released && r.vr.started && r.vr.done;
    return r;
}

// The anchor (P5-b): around each of VERTB detections 2-4, the last VPOSR read
// before the Copper's fetch of the COLOR01 data word and the first read after
// it both show V == expect_v.
bool p5_anchor(const VideoRun& vr, const std::vector<Detection>& det, uint32_t addr, uint8_t expect_v) {
    for (int i = 1; i <= 3 && i < (int)det.size(); ++i) {
        uint64_t t0 = det[i].marker_cycle;
        uint64_t t1 = (i + 1 < (int)det.size()) ? det[i + 1].marker_cycle : t0 + 400000;
        std::optional<uint64_t> tc;
        for (auto& a : vr.log)
            if (!a.is_write && a.word_addr == addr && a.cycle >= t0 && a.cycle < t1) { tc = a.cycle; break; }
        if (!tc) return false;
        uint64_t last_before = 0, first_after = 0;
        bool have_before = false, have_after = false;
        uint32_t val_before = 0, val_after = 0;
        for (auto& rc : det[i].reads) {
            if (rc.first < *tc) {
                if (!have_before || rc.first > last_before) { last_before = rc.first; val_before = rc.second; have_before = true; }
            } else {
                if (!have_after || rc.first < first_after) { first_after = rc.first; val_after = rc.second; have_after = true; }
            }
        }
        auto v8 = [](uint32_t v32) { return (uint8_t)(v_of17(v32 & 0x1FFFFu) & 0xFF); };
        if (!(have_before && have_after && v8(val_before) == expect_v && v8(val_after) == expect_v)) return false;
    }
    return true;
}

}  // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks c;

    const char* mutate_c = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_c ? mutate_c : "";
    const char* only_c = std::getenv("NMH_ONLY");
    const std::string only = only_c ? only_c : "";
    auto grp = [&](const char* g) {
        if (!mv.empty()) return mv.rfind(g, 0) == 0;
        return only.empty() || only.find(g) != std::string::npos;
    };
    const bool do_p3 = grp("P3"), do_p4 = grp("P4"), do_p1 = grp("P1"), do_p5 = grp("P5");
    const bool do_p2 = grp("P2");
    // A mutation's run captures only what it changes, in the first system
    // configuration; the plain run grades every system configuration.
    const int smin = (mv == "P1ESTD") ? 2 : 1;
    const int smax = mv.empty() ? 4 : smin;
    const int smax_p5 = mv.empty() ? 2 : 1;
    auto mut = [&](const char* n) { return mv == n; };

    // ---------------------------------------------------------------
    // CAL-CC: the colour clock in clk_sys cycles, from the 7 MHz clock enable
    // at the port (p.24: the colour clock is two processor clocks).
    // ---------------------------------------------------------------
    {
        Clk7 k7 = measure_clk7();
        char b[96];
        std::snprintf(b, sizeof b, "clk7_en pulses=%d period=%llu clk_sys cycles", k7.pulses, (unsigned long long)k7.period);
        c.note(std::string("CAL-CC: ") + b);
        c.expect(k7.regular,
            "CAL-CC: the 7 MHz clock enable recurs at the port with one constant period over every pulse seen; "
            "one colour clock is two of its periods (p.24)");
        if (k7.regular) g_cc = 2 * k7.period;
    }
    c.note("colour clock = " + std::to_string(g_cc) + " clk_sys cycles");

    // A line that was not run because its fixture failed prints RECORDED: NOT GRADED, never PASS or FAIL.
    auto skipped = [&](const std::string& label, const std::string& fixture) {
        c.record("NOT GRADED: " + label + " -- precondition " + fixture + " failed");
    };

    // A graded line whose precondition failed prints RECORDED ... NOT GRADED, never PASS or FAIL. Returns true
    // when the line was graded and passed.
    auto gated = [&](bool pre, const std::string& head, const std::string& why, const std::string& label, bool cond) {
        if (!pre) { c.record(head + ": NOT GRADED, " + why); return false; }
        c.expect(cond, label);
        return cond;
    };

    // ---------------------------------------------------------------
    // P3 -- BPLCON0 bit 3 (LPEN) enables the light-pen latch (p.271, p.89,
    // p.229, p.239). Claims: opus:A-bpc.b03.c0, sol:A-bplcon.c49,
    // opus:A-leg.lpen1, sol:A-bplcon.c72, opus:3-r0.c09, sol:3-regsum-bplcon0.c6
    // ---------------------------------------------------------------
    bool p3main_pass[5] = {false, false, false, false, false};   // P3-MAIN graded and passed, per system configuration
    {
        Agg agg3; agg3.expected = 8;
        for (int s = 1; do_p3 && s <= smax; ++s) {
            const bool ntsc = s_is_ntsc(s);
            const bool ecs = s_is_ecs(s);
            const std::string sn = "S" + std::to_string(s);
            nmh::Config cfg = cfg_for_s(s);

            std::string bin_l0 = lpen_bin(dir, s, false);
            std::string bin_l1 = lpen_bin(dir, s, true);
            if (s == 1 && mut("P3FIX")) bin_l1 = dir + "/prog_lpen_ecs_pal_l1_badstart.bin";
            if (s == 1 && mut("P3MARK")) bin_l1 = dir + "/prog_lpen_ecs_pal_l1_badmark.bin";
            if (s == 1 && mut("P3MAIN")) bin_l1 = bin_l0;
            if (s == 1 && mut("P3LIVE")) bin_l0 = bin_l1;
            if (ecs && s == 1 && mut("P3D")) bin_l1 = dir + "/prog_lpen_ecs_pal_l1_mutd.bin";
            if (ecs && s == 1 && mut("P3A")) bin_l1 = dir + "/prog_lpen_ecs_pal_l1_burst20.bin";
            if (ecs && s == 1 && mut("P3C_LPENDIS")) bin_l1 = dir + "/prog_lpen_ecs_pal_l1_lpendis.bin";
            if (ecs && s == 1 && mut("P3C_WRONGSTD")) bin_l0 = dir + "/prog_lpen_ecs_pal_l0_wrongstd.bin";
            if (ecs && s == 1 && mut("P3L")) bin_l0 = dir + "/prog_lpen_ecs_pal_l0_nopoll.bin";
            if (ecs && s == 1 && mut("P3BLATE")) bin_l0 = dir + "/prog_lpen_ecs_pal_l0_late.bin";

            Run r0 = run_program(bin_l0, cfg, kStartLpen);
            Run r1 = run_program(bin_l1, cfg, kStartLpen);
            const bool ok0 = r0.loaded && r0.released && r0.started && r0.done.has_value();
            const bool ok1 = r1.loaded && r1.released && r1.started && r1.done.has_value();
            const bool fix3 = ok0 && ok1;
            const int burstlen0 = program_burst_len(r0.prog_words);
            const int burstlen1 = program_burst_len(r1.prog_words);

            c.expect(fix3, "P3 fixture " + sn + " (" + s_name(s) +
                      "): both L0 and L1 programs loaded, released reset, stored their start word and reached DONE");

            auto det0 = extract_detections(r0.log, burstlen0);
            auto det1 = extract_detections(r1.log, burstlen1);
            const uint64_t loopmax = std::max(r0.loop.period, r1.loop.period);

            // P3-M: the stores to the measurement buffer are the program's own five blocks, in both runs.
            const bool m3_ok = gated(fix3, "P3-M " + sn, "the fixture failed",
                "P3-M " + sn + ": in both runs the stores to the measurement buffer are exactly five blocks, block k "
                "being the marker word k (k = 0..4) followed by the burst's long words at consecutive word addresses",
                blocks_shape_ok(r0.log, burstlen0, 5) && blocks_shape_ok(r1.log, burstlen1, 5));

            // P3-a: >=100 reads per window, spread over it, in both runs.
            bool p3a_cond = true;
            for (int i = 1; i <= 4; ++i) {
                if (i >= (int)det0.size() || i >= (int)det1.size()) { p3a_cond = false; continue; }
                WindowResult a = analyze_window(det0[i], win_start(), win_end());
                WindowResult b = analyze_window(det1[i], win_start(), win_end());
                p3a_cond = p3a_cond && a.has_min_100 && a.first_near_start && a.last_near_end
                                    && b.has_min_100 && b.first_near_start && b.last_near_end;
            }
            const bool p3a_ok = gated(fix3 && m3_ok, "P3-a " + sn, "the fixture or P3-M failed",
                "P3-a " + sn + ": each window (L0 and L1) holds >=100 counted reads, "
                "the first within 30 cc of the window start and the last within 30 cc of its end", p3a_cond);

            // P3-L: the detecting loop's period, which the field-length check adds to its tolerance, is
            // measured from the program's own instruction fetches.
            const bool p3l_ok = gated(fix3, "P3-L " + sn, "the fixture failed",
                "P3-L " + sn + ": the VERTB polling loop is seen spinning in the instruction fetches of "
                "both runs (at least 100 iterations each), so its period is measured",
                r0.loop.iterations >= 100 && r1.loop.iterations >= 100);
            c.note("P3-L " + sn + ": polling-loop period L0=" + std::to_string(r0.loop.period) + " L1=" +
                   std::to_string(r1.loop.period) + " clk_sys cycles, iterations " +
                   std::to_string(r0.loop.iterations) + "/" + std::to_string(r1.loop.iterations));

            // P3-b (the anchor): in L0 every counted read has V between 1 and 19, and the VERTB detections of
            // both runs recur at the configured field length.
            bool b_vr = det0.size() >= 5, b_fl = det0.size() > 1 && det1.size() > 1;
            std::vector<WindowResult> anchor_w;
            for (int i = 1; i <= 4; ++i) {
                if (i >= (int)det0.size()) { b_vr = false; continue; }
                anchor_w.push_back(analyze_window(det0[i], win_start(), win_end()));
                b_vr = b_vr && anchor_w.back().v_in_1_19 && anchor_w.back().count > 0;
            }
            for (int i = 0; i + 1 < (int)det0.size(); ++i)
                b_fl = b_fl && field_length_ok(det0[i + 1].marker_cycle - det0[i].marker_cycle, ntsc, loopmax);
            for (int i = 0; i + 1 < (int)det1.size(); ++i)
                b_fl = b_fl && field_length_ok(det1[i + 1].marker_cycle - det1[i].marker_cycle, ntsc, loopmax);
            const bool p3b_ok = gated(fix3 && m3_ok && p3l_ok, "P3-b " + sn, "the fixture, P3-M or P3-L failed",
                "P3-b " + sn +
                ": in L0 every counted read has V between 1 and 19, and consecutive VERTB detections in both runs "
                "recur at the configured field length", b_vr && b_fl);
            {
                std::string diag = std::string("V-in-1..19=") + (b_vr ? "Y" : "N") + " field-length=" + (b_fl ? "Y" : "N") + " | ";
                for (size_t w = 0; w < anchor_w.size(); ++w)
                    diag += "[w" + std::to_string(w + 1) + " " + pf_text(anchor_w[w]) + "] ";
                for (int i = 0; i + 1 < (int)det0.size(); ++i) {
                    uint64_t delta = det0[i + 1].marker_cycle - det0[i].marker_cycle;
                    diag += "[L0 d" + std::to_string(i) + "=" + std::to_string(delta) +
                            " ok=" + (field_length_ok(delta, ntsc, loopmax) ? "Y" : "N") + "] ";
                }
                for (int i = 0; i + 1 < (int)det1.size(); ++i) {
                    uint64_t delta = det1[i + 1].marker_cycle - det1[i].marker_cycle;
                    diag += "[L1 d" + std::to_string(i) + "=" + std::to_string(delta) +
                            " ok=" + (field_length_ok(delta, ntsc, loopmax) ? "Y" : "N") + "] ";
                }
                c.note("P3-b " + sn + " parts: " + diag);
            }

            // P3-LIVE (the L0 control of P3-MAIN, p.239 "Read (long) VPOSR twice. If both values are not the
            // same ..."): in L0 the counted reads of every window are a running counter, by three criteria:
            // (i) no two consecutive reads are equal, (ii) V never decreases and steps by at most 1,
            // (iii) within one V value H increases except for at most one decrease (the wrap of H, p.23).
            std::vector<WindowResult> live_w;
            bool live_cond = det0.size() >= 5;
            for (int i = 1; i <= 4 && i < (int)det0.size(); ++i) {
                live_w.push_back(analyze_window(det0[i], win_start(), win_end()));
                live_cond = live_cond && live_w.back().running;
            }
            const bool live_pre = fix3 && m3_ok && p3a_ok;
            const bool live_ok = gated(live_pre, "P3-LIVE " + sn, "the fixture, P3-M or P3-a failed",
                "P3-LIVE " + sn + " (" + s_name(s) + "): in L0 the counted reads of the windows after VERTB detections "
                "2-5 are a running counter: no two consecutive reads equal, V never decreasing and stepping by at most 1, "
                "and within one V value H increasing except for at most one decrease", live_cond);
            {
                std::string diag;
                for (size_t w = 0; w < live_w.size(); ++w)
                    diag += "[w" + std::to_string(w + 1) + " " + pf_text(live_w[w]) + "] ";
                c.note("P3-LIVE " + sn + " windows: " + diag);
                c.note("P3-LIVE " + sn + " pairs with V unchanged and H lower (the wrap of H before V steps): " + lag_text(live_w));
            }
            agg3.add(live_pre, live_ok);

            // P3-c (ECS only): BEAMCON0 carries LPENDIS=0 and the configured standard's bit 5 in both
            // programs' own code and in the CPU's fetch of the immediate word, and the field-length check
            // reports that standard.
            bool p3c_ok = true;
            if (ecs) {
                auto val0 = find_write_value(r0.prog_words, 0, 0x1DC);
                auto val1 = find_write_value(r1.prog_words, 0, 0x1DC);
                auto fet0 = fetched_write_value(r0.prog_words, r0.log, 0, 0x1DC);
                auto fet1 = fetched_write_value(r1.prog_words, r1.log, 0, 0x1DC);
                auto value_ok = [&](std::optional<uint16_t> v, std::optional<uint16_t> f) {
                    return v && f && *f == *v && ((*v & 0x2000) == 0) && (((*v >> 5) & 1) == (ntsc ? 0 : 1));
                };
                const bool bit_ok = value_ok(val0, fet0) && value_ok(val1, fet1);
                bool std_ok = det0.size() > 1;
                for (int i = 0; i + 1 < (int)det0.size(); ++i)
                    std_ok = std_ok && field_length_ok(det0[i + 1].marker_cycle - det0[i].marker_cycle, ntsc, loopmax);
                p3c_ok = gated(fix3 && m3_ok && p3l_ok, "P3-c " + sn, "the fixture, P3-M or P3-L failed",
                    "P3-c " + sn + ": BEAMCON0 carries LPENDIS=0 and bit 5 for the configured standard "
                    "in both programs' own code and in the CPU's fetch of the immediate word, and the "
                    "field-length check reports that standard", bit_ok && std_ok);
            }

            // P3-d: BPLCON0 written exactly once by the CPU program (its own step-3 write); no Copper
            // fetch of a MOVE to $100 either (no Copper runs here).
            const int n0 = count_writes_to(r0.prog_words, 0, 0x100);
            const int n1 = count_writes_to(r1.prog_words, 0, 0x100);
            const bool cop_clean = !copper_fetched_move_to(r0.log, kCopListAddr, 0x0100) &&
                                   !copper_fetched_move_to(r1.log, kCopListAddr, 0x0100);
            const bool p3d_ok = gated(fix3, "P3-d " + sn, "the fixture failed",
                "P3-d " + sn + ": BPLCON0 written exactly once by the program (no other CPU write to "
                "$DFF100, no Copper fetch of a MOVE to $100 from the list at $020000)", n0 == 1 && n1 == 1 && cop_clean);

            // P3 main. Its preconditions: the fixture, P3-M, P3-a, P3-L, P3-b, P3-c and P3-d. The L0 line
            // P3-LIVE is a component of the aggregate and is not a precondition of this line.
            const bool pre3 = fix3 && m3_ok && p3a_ok && p3l_ok && p3b_ok && p3c_ok && p3d_ok;
            if (!pre3) {
                c.record("P3-MAIN " + sn + ": NOT GRADED, a precondition failed (fixture/P3-M/P3-a/P3-L/P3-b/P3-c/P3-d)");
                agg3.add(false, false);
            } else {
                bool main_ok = det1.size() >= 5;
                int min_count_l1 = INT32_MAX;
                for (int i = 1; i <= 4 && i < (int)det1.size(); ++i) {
                    WindowResult wr = analyze_window(det1[i], win_start(), win_end());
                    main_ok = main_ok && wr.all_equal && wr.count > 0;
                    min_count_l1 = std::min(min_count_l1, wr.count);
                }
                c.expect(main_ok, "P3-MAIN " + sn + " (" + s_name(s) + "): with LPEN=1, every counted VPOSR/VHPOSR "
                          "read in the windows after VERTB detections 2-5 shows the same 17-bit value (frozen)");
                agg3.add(true, main_ok);
                p3main_pass[s] = main_ok;
                c.note("P3-MAIN " + sn + ": L1 min window count " + std::to_string(min_count_l1) + " reads");
            }
        }
        if (do_p3) {
            const std::string ids3 = "opus:A-bpc.b03.c0 sol:A-bplcon.c49 opus:A-leg.lpen1 sol:A-bplcon.c72 opus:3-r0.c09 "
                                     "sol:3-regsum-bplcon0.c6 ";
            if (agg3.gradable())
                c.expect(agg3.failed == 0,
                    "opus:A-bpc.b03.c0 sol:A-bplcon.c49 opus:A-leg.lpen1 sol:A-bplcon.c72 opus:3-r0.c09 "
                    "sol:3-regsum-bplcon0.c6 p271 p89 p229 p239 P3 aggregate: with LPEN=1 the VPOSR/VHPOSR reads "
                    "in the windows after VERTB detections 2-5 hold one 17-bit value, and with LPEN=0 they are a "
                    "running counter, in all four system configurations");
            else
                c.record(ids3 + "P3 aggregate: NOT GRADED, " + agg3.counts());
        }
    }

    // ---------------------------------------------------------------
    // P4 -- LPEN is 0 after power-up (p.271). Claims: opus:A-leg.lpen2,
    // sol:A-bplcon.c73
    // ---------------------------------------------------------------
    {
        Agg agg4; agg4.expected = 4;
        for (int s = 1; do_p4 && s <= smax; ++s) {
            const bool ntsc = s_is_ntsc(s);
            const bool ecs = s_is_ecs(s);
            const std::string sn = "S" + std::to_string(s);
            nmh::Config cfg = cfg_for_s(s);

            std::string bin = p4_bin(dir, s);
            bool use_copper_preload = false;
            if (s == 1 && mut("P4FIX")) bin = dir + "/prog_lpen_p4_ecs_pal_badstart.bin";
            if (s == 1 && mut("P4MARK")) bin = dir + "/prog_lpen_p4_ecs_pal_badmark.bin";
            if (ecs && s == 1 && mut("P4B_PROG")) bin = dir + "/prog_lpen_ecs_pal_l1.bin";
            if (ecs && s == 1 && mut("P4A")) bin = dir + "/prog_lpen_p4_ecs_pal_burst20.bin";
            if (ecs && s == 1 && mut("P4C")) bin = dir + "/prog_lpen_p4_ecs_pal_wrongstd.bin";
            if (ecs && s == 1 && mut("P4L")) bin = dir + "/prog_lpen_p4_ecs_pal_nopoll.bin";
            if (s == 1 && mut("P4B_COP")) { bin = dir + "/prog_lpen_p4_copper.bin"; use_copper_preload = true; }

            std::vector<std::pair<uint32_t, std::vector<uint16_t>>> preloads;
            if (use_copper_preload) preloads.push_back({kCopListAddr, {0x0100, 0x0008, 0xFFFF, 0xFFFE}});

            Run r = run_program(bin, cfg, kStartLpen, preloads);
            const bool ok = r.loaded && r.released && r.started && r.done.has_value();
            const int burstlen = program_burst_len(r.prog_words);
            c.expect(ok, "P4 fixture " + sn + " (" + s_name(s) +
                      "): program loaded, released reset, stored its start word and reached DONE");

            auto det = extract_detections(r.log, burstlen);

            // P4-M: as P3-M, for the one program.
            const bool m4_ok = gated(ok, "P4-M " + sn, "the fixture failed",
                "P4-M " + sn + ": the stores to the measurement buffer are exactly five blocks, block k being the "
                "marker word k (k = 0..4) followed by the burst's long words at consecutive word addresses",
                blocks_shape_ok(r.log, burstlen, 5));

            // P4-a: as P3-a.
            bool p4a_cond = true;
            for (int i = 1; i <= 4; ++i) {
                if (i >= (int)det.size()) { p4a_cond = false; continue; }
                WindowResult wr = analyze_window(det[i], win_start(), win_end());
                p4a_cond = p4a_cond && wr.has_min_100 && wr.first_near_start && wr.last_near_end;
            }
            const bool p4a_ok = gated(ok && m4_ok, "P4-a " + sn, "the fixture or P4-M failed",
                "P4-a " + sn + ": each window holds >=100 counted reads, spread over it", p4a_cond);

            // P4-L: as P3-L.
            const bool p4l_ok = gated(ok, "P4-L " + sn, "the fixture failed",
                "P4-L " + sn + ": the VERTB polling loop is seen spinning in the instruction fetches "
                "(at least 100 iterations), so its period is measured", r.loop.iterations >= 100);
            c.note("P4-L " + sn + ": polling-loop period " + std::to_string(r.loop.period) +
                   " clk_sys cycles, iterations " + std::to_string(r.loop.iterations));

            // P4-c (ECS only): as P3-c's field-length half.
            bool p4c_ok = true;
            if (ecs) {
                bool std_ok = det.size() > 1;
                for (int i = 0; i + 1 < (int)det.size(); ++i)
                    std_ok = std_ok && field_length_ok(det[i + 1].marker_cycle - det[i].marker_cycle, ntsc, r.loop.period);
                p4c_ok = gated(ok && m4_ok && p4l_ok, "P4-c " + sn, "the fixture, P4-M or P4-L failed",
                    "P4-c " + sn + ": the field-length check reports the configured standard", std_ok);
            }

            // P4-b, program part: the program contains no write to $DFF100 at all.
            const bool p4bp_ok = gated(ok, "P4-B-PROGRAM " + sn, "the fixture failed",
                "P4-B-PROGRAM " + sn + (mut("P4B_PROG") ? " [P3's L1 program]" : "") +
                ": the program contains no write to $DFF100", count_writes_to(r.prog_words, 0, 0x100) == 0);

            // P4-b, Copper part: no Copper instruction fetched from the list at $020000 is a MOVE to $100.
            const bool p4bc_ok = gated(ok, "P4-B-COPPER " + sn, "the fixture failed",
                "P4-B-COPPER " + sn + (use_copper_preload ? " [Copper list writes BPLCON0]" : "") +
                ": no Copper instruction fetched from the list at $020000 is a MOVE to $100",
                !copper_fetched_move_to(r.log, kCopListAddr, 0x0100));

            // P4 main: gated on every precondition above and on the latch being shown live in the same
            // system configuration (P3-MAIN passed there); without a latch an LPEN of 1 would look like 0.
            const bool pre4 = ok && m4_ok && p4a_ok && p4l_ok && p4c_ok && p4bp_ok && p4bc_ok;
            std::vector<WindowResult> w4;
            bool main_ok = det.size() >= 5;
            for (int i = 1; i <= 4 && i < (int)det.size(); ++i) {
                w4.push_back(analyze_window(det[i], win_start(), win_end()));
                main_ok = main_ok && w4.back().running && w4.back().v_in_1_19;
            }
            if (!pre4) {
                c.record("P4-MAIN " + sn + ": NOT GRADED, a precondition failed (fixture/P4-M/P4-a/P4-L/P4-c/P4-b)");
                agg4.add(false, false);
            } else if (!p3main_pass[s]) {
                c.record("P4-MAIN " + sn + ": NOT GRADED, P3-MAIN did not pass in this configuration, so a set LPEN is "
                         "not shown to latch here and an LPEN of 1 would look like 0");
                agg4.add(false, false);
            } else {
                c.expect(main_ok, "P4-MAIN " + sn + " (" + s_name(s) + "): after power-up, before any BPLCON0 write, "
                          "the VPOSR/VHPOSR reads of every window after VERTB detections 2-5 are a running counter "
                          "(no two consecutive reads equal, V never decreasing and stepping by at most 1, within one V "
                          "value H increasing except for at most one decrease) with V in 1..19");
                agg4.add(true, main_ok);
            }
            {
                std::string diag;
                for (size_t w = 0; w < w4.size(); ++w) diag += "[w" + std::to_string(w + 1) + " " + pf_text(w4[w]) + "] ";
                c.note("P4-MAIN " + sn + " windows: " + diag);
                c.note("P4-MAIN " + sn + " pairs with V unchanged and H lower (the wrap of H before V steps): " + lag_text(w4));
            }
            c.note("P4 " + sn + ": DONE at " + (r.done ? std::to_string(*r.done) : std::string("(none)")));
        }
        if (do_p4) {
            const std::string ids4 = "opus:A-leg.lpen2 sol:A-bplcon.c73 ";
            if (agg4.gradable())
                c.expect(agg4.failed == 0,
                    "opus:A-leg.lpen2 sol:A-bplcon.c73 p271 P4 aggregate: after power-up, before any BPLCON0 write, the "
                    "VPOSR/VHPOSR reads of the windows after VERTB detections 2-5 are a running counter with V in "
                    "1..19, in all four system configurations");
            else
                c.record(ids4 + "P4 aggregate: NOT GRADED, " + agg4.counts());
        }
    }

    // ---------------------------------------------------------------
    // P1 -- BPLCON0 bit 9 (COLOR_ON) has no effect on the RGB output (p.63-64).
    // Claims: opus:3-a1000.c04, sol:3-composite.c6 (p.63), opus:3-sum.c21 (p.64).
    // Configurations A (background only), B (1 plane, low resolution), C (4
    // planes, high resolution, 16 colours) and E (bit 9 changed mid-line by the
    // Copper). P2 -- the RGB output is the correct picture whether bit 9 is set
    // or clear (p.63). Claim: opus:3-a1000.c05 (sol:3-composite.c7 is declined). P2 uses
    // configuration B, and shares its pair of runs with P1's configuration B.
    // ---------------------------------------------------------------
    {
        Agg agg1; agg1.expected = 16;
        Agg agg2; agg2.expected = 4;
        const char cfgs[4] = {'A', 'B', 'C', 'E'};
        // P1A is a dedicated contrast, run below; it does not run the main loop.
        const bool dedicated = mut("P1A");
        for (int s = smin; (do_p1 || do_p2) && !dedicated && s <= smax; ++s) {
            const bool ntsc = s_is_ntsc(s);
            const int H = ntsc ? 200 : 256;
            for (char cfg : cfgs) {
                if (cfg != 'B' && !do_p1) continue;
                const std::string tag = std::string(1, cfg) + " S" + std::to_string(s);
                const bool m1 = (s == 1);

                DispOpts o;
                if (do_p1 && m1) {
                    if (mut("P1MAIN")) o.contrast1 = true;
                    if (mut("P1B")) o.list1_is_list0 = true;
                    if (mut("P1C")) {
                        if (cfg == 'A') o.a_sprite_dma = true;
                        else if (cfg == 'C') o.ov0 = o.ov1 = dir + "/prog_p1_16c_pal_narrow.bin";
                        else o.ov0 = o.ov1 = dir + "/prog_p1_2c_pal_narrow.bin";
                    }
                    if (mut("P1D")) {
                        if (cfg == 'A') o.a_colour_top = true;
                        else o.zero_bitplane = true;
                    }
                    if (mut("P1FIX"))
                        o.ov1 = dir + (cfg == 'C' ? "/prog_p1_16c_pal_badstart.bin" : "/prog_p1_2c_pal_badstart.bin");
                }
                if (do_p1 && s == 2 && cfg == 'B' && mut("P1ESTD")) o.ov0 = o.ov1 = dir + "/prog_p1_2c_ntsc_wrongstd.bin";
                if (cfg == 'B' && do_p2 && m1) {
                    if (mut("P2A")) o.list1_is_list0 = true;
                    if (mut("P2B")) { o.ov0 = o.ov1 = dir + "/prog_p1_2c_pal_mod2.bin"; o.pad_lines = 20; }
                    if (mut("P2FLIP")) o.flip_word_index = 100 * 20 + 5;
                    if (mut("P2ITEM1")) o.ov0 = o.ov1 = dir + "/prog_p1_2c_pal_vstoplow.bin";
                    if (mut("P2EDGE")) o.ov0 = o.ov1 = dir + "/prog_p1_2c_pal_rightedge.bin";
                    if (mut("P2FIX")) o.ov1 = dir + "/prog_p1_2c_pal_badstart.bin";
                }

                DisplayData dd = build_and_run_display(dir, cfg, s, o);

                auto starts0 = dma_field_starts(dd.run0.log);
                auto starts1 = dma_field_starts(dd.run1.log);
                auto vsa0 = find_vsync_assertions(dd.run0.video);
                auto vsa1 = find_vsync_assertions(dd.run1.video);
                std::vector<size_t> w0, w1;
                if (dd.ok && !starts0.empty() && !starts1.empty()) {
                    for (auto v : vsa0) if ((uint64_t)v > starts0.front()) w0.push_back(v);
                    for (auto v : vsa1) if ((uint64_t)v > starts1.front()) w1.push_back(v);
                }
                const bool p1_here = do_p1;
                const bool p2_here = (cfg == 'B' && do_p2);
                const bool fixture_ok = dd.ok && w0.size() >= 4 && w1.size() >= 4 && starts0.size() >= 3 && starts1.size() >= 3;
                const bool p2fix_ok = dd.ok && w0.size() >= 6 && w1.size() >= 6 && starts0.size() >= 6 && starts1.size() >= 6;
                if (p1_here)
                    c.expect(fixture_ok, "P1 fixture " + tag + ": both runs loaded, released reset and stored their start word, "
                              "the Copper fetched the list, and at least 4 vsync assertions and 3 DMA-field starts follow the first Copper fetch in both runs");
                if (p2_here)
                    c.expect(p2fix_ok, "P2 fixture " + tag + ": both runs loaded, released reset and stored their start word, "
                              "the Copper fetched the list, and at least 6 vsync assertions and 6 DMA-field starts follow the first Copper fetch in both runs");
                if (p1_here && !fixture_ok) {
                    for (const char* l : {"P1-a", "P1-b", "P1-c", "P1-d", "P1-e"})
                        skipped(std::string(l) + " " + tag, "P1 fixture " + tag);
                    c.record("P1-MAIN " + tag + ": NOT GRADED, the fixture failed");
                    agg1.add(false, false);
                }
                if (p2_here && !p2fix_ok) {
                    for (const char* l : {"CAL-VIDEO", "P2-a", "P2-b", "P2-I1", "P2-I2", "P2-I3"})
                        skipped(std::string(l) + " " + tag, "P2 fixture " + tag);
                    agg2.add(false, false);
                }

                // ---- P1 ----
                if (p1_here && fixture_ok) {
                    const size_t b0 = w0[1], e0 = w0[3];  // 2nd..4th vsync assertion
                    const size_t b1 = w1[1], e1 = w1[3];
                    const size_t len0 = e0 - b0, len1 = e1 - b1;
                    const size_t cmp_len = std::min(len0, len1);

                    // P1-a: the sync samples of the two runs are identical over the compared span.
                    const bool p1a_ok = sync_samples_identical(dd.run0.video, b0, e0, dd.run1.video, b1, e1);
                    c.expect(p1a_ok,
                        "P1-a " + tag + ": hsync and vsync samples of the two runs are identical over the compared span");

                    // P1-b: the bit-9 value reached BPLCON0 -- the Copper's fetched MOVEs carry the table's values.
                    size_t nf0 = 0, nf1 = 0;
                    const bool p1b_ok = bplcon0_fetches_ok(dd.run0, starts0, b0, e0, expected_bplcon0(cfg, 0), &nf0) &&
                                        bplcon0_fetches_ok(dd.run1, starts1, b1, e1, expected_bplcon0(cfg, 1), &nf1);
                    c.expect(p1b_ok, "P1-b " + tag +
                        ": in every DMA field of the compared span the Copper's fetched MOVEs to BPLCON0 carry the table's values");

                    // P1-c: the display ran -- DMA fields 2 and 3 each show exactly N reads of the bitplane buffers.
                    std::vector<std::pair<size_t, size_t>> ranges;   // word-address ranges of the bitplane buffers
                    if (cfg == 'C') {
                        const uint32_t planes[4] = {kBplPlane1, kBplPlane2, kBplPlane3, kBplPlane4};
                        for (auto pl : planes) ranges.push_back({pl / 2, pl / 2 + (size_t)40 * H});
                    } else {
                        ranges.push_back({kBplPlane1 / 2, kBplPlane1 / 2 + (size_t)20 * H});
                    }
                    const size_t expect_reads = (cfg == 'A') ? 0 : (cfg == 'C' ? (size_t)4 * 40 * H : (size_t)20 * H);
                    auto count_bpl_reads = [&](const VideoRun& r, uint64_t from, uint64_t to) {
                        size_t n = 0;
                        for (auto& a : r.log) {
                            if (a.is_write || a.cycle < from || a.cycle >= to) continue;
                            for (auto& rg : ranges) if (a.word_addr >= rg.first && a.word_addr < rg.second) { ++n; break; }
                        }
                        return n;
                    };
                    const size_t n0f2 = count_bpl_reads(dd.run0, starts0[1], starts0[2]);
                    const size_t n0f3 = count_bpl_reads(dd.run0, starts0[2], starts0[3]);
                    const size_t n1f2 = count_bpl_reads(dd.run1, starts1[1], starts1[2]);
                    const size_t n1f3 = count_bpl_reads(dd.run1, starts1[2], starts1[3]);
                    const bool p1c_ok = (n0f2 == expect_reads && n0f3 == expect_reads &&
                                         n1f2 == expect_reads && n1f3 == expect_reads);
                    std::string p1c_counts = std::to_string(expect_reads) + " " + std::to_string(n0f2) + " " +
                        std::to_string(n0f3) + " " + std::to_string(n1f2) + " " + std::to_string(n1f3);
                    c.expect(p1c_ok,
                        "P1-c " + tag + ": DMA fields 2 and 3 each show exactly N bitplane-buffer reads "
                        "(expected/run0-field2/run0-field3/run1-field2/run1-field3 = " + p1c_counts + ")");

                    // P1-d: the picture is not trivially uniform.
                    auto colours_seen = [&](const VideoRun& r, size_t b, size_t len, bool want[16]) {
                        for (int i = 0; i < 16; ++i) want[i] = false;
                        for (size_t i = 0; i < len; ++i) {
                            const auto& x = r.video[b + i];
                            for (int q = 0; q < 16; ++q) if (shows(x, kPalette16[q])) { want[q] = true; break; }
                        }
                    };
                    bool seen0[16], seen1[16];
                    colours_seen(dd.run0, b0, len0, seen0);
                    colours_seen(dd.run1, b1, len1, seen1);
                    bool p1d_ok;
                    if (cfg == 'A') p1d_ok = seen0[0] && seen1[0];
                    else if (cfg == 'C') { p1d_ok = true; for (int q = 0; q < 16; ++q) p1d_ok = p1d_ok && seen0[q] && seen1[q]; }
                    else p1d_ok = seen0[0] && seen0[1] && seen1[0] && seen1[1];
                    c.expect(p1d_ok, "P1-d " + tag + ": the picture is not trivially uniform (the required colours occur)");

                    // P1-e: the field length matches the configured standard.
                    const bool p1e_ok = vsync_field_length_ok(vsa0, ntsc) && vsync_field_length_ok(vsa1, ntsc);
                    c.expect(p1e_ok,
                        "P1-e " + tag + (s == 2 && cfg == 'B' && mut("P1ESTD") ? " [contrast: BEAMCON0 bit 5 opposite the standard]" : "") +
                        ": in both runs consecutive vsync assertions recur at the configured standard's field length");

                    // P1 main, graded when every precondition above holds.
                    if (!(p1a_ok && p1b_ok && p1c_ok && p1d_ok && p1e_ok)) {
                        c.record("P1-MAIN " + tag + ": NOT GRADED, a precondition failed (P1-a/P1-b/P1-c/P1-d/P1-e)");
                        agg1.add(false, false);
                    } else {
                        size_t diffs = 0;
                        for (size_t i = 0; i < cmp_len; ++i) {
                            auto& x0 = dd.run0.video[b0 + i]; auto& x1 = dd.run1.video[b1 + i];
                            if (x0.r != x1.r || x0.g != x1.g || x0.b != x1.b) ++diffs;
                        }
                        char diffbuf[32]; std::snprintf(diffbuf, sizeof diffbuf, "%zu", diffs);
                        const bool main_ok = (diffs == 0);
                        c.expect(main_ok,
                            "P1-MAIN config " + tag + ": every R,G,B sample of run \"1\" equals run \"0\" at the same index (" +
                            std::string(diffbuf) + " differing samples)");
                        agg1.add(true, main_ok);
                    }
                }

                // ---- P2 ----
                if (p2_here && p2fix_ok) {
                    const size_t b0 = w0[1], e0 = w0[5], b1 = w1[1], e1 = w1[5];  // 2nd..6th vsync assertion

                    // CAL-VIDEO: the doubler's units, against the pages.
                    std::vector<size_t> span0(w0.begin() + 1, w0.begin() + 6), span1(w1.begin() + 1, w1.begin() + 6);
                    VideoCal cal0 = calibrate_video(dd.run0.video, span0, ntsc);
                    VideoCal cal1 = calibrate_video(dd.run1.video, span1, ntsc);
                    const bool cal_ok = cal0.ok && cal1.ok && cal0.k == cal1.k;
                    c.expect(cal_ok, "CAL-VIDEO " + tag + ": between consecutive vsync assertions the output has 2 hs periods "
                              "per page-given line count, each a half beam line of 227 or 228 cc, and a low-resolution "
                              "pixel spans k samples (k=" + std::to_string(cal0.k) + ", median doubler line " +
                              std::to_string(cal0.median_len) + " samples)");

                    // P2-a: P1-b and P1-e hold for the run.
                    const bool p2a_ok = bplcon0_fetches_ok(dd.run0, starts0, b0, e0, expected_bplcon0('B', 0)) &&
                                        bplcon0_fetches_ok(dd.run1, starts1, b1, e1, expected_bplcon0('B', 1)) &&
                                        vsync_field_length_ok(vsa0, ntsc) && vsync_field_length_ok(vsa1, ntsc);
                    c.expect(p2a_ok, "P2-a " + tag + ": P1-b and P1-e hold for both runs");

                    // P2-b: the buffer is read exactly once per word, in increasing address order, in DMA fields 2-5.
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
                    bool p2b_ok = true;
                    for (int f = 1; f <= 4; ++f)
                        p2b_ok = p2b_ok && p2b_check(dd.run0, starts0[f], starts0[f + 1]) &&
                                            p2b_check(dd.run1, starts1[f], starts1[f + 1]);
                    c.expect(p2b_ok, "P2-b " + tag + (m1 && mut("P2B") ? " [contrast: BPL1MOD=$0002]" : "") +
                              ": the bitplane buffer is read exactly once per word, in increasing "
                              "address order, across the 4-field span");

                    // Items 1-3, each run; the item lines need CAL-VIDEO and P2-a, item 2 also P2-b.
                    P2Eval ev0, ev1;
                    if (cal_ok) { ev0 = eval_p2(dd.run0, b0, e0, H, cal0.k); ev1 = eval_p2(dd.run1, b1, e1, H, cal1.k); }
                    const bool pre_common = fixture_ok && cal_ok && p2a_ok;
                    bool graded_all = true, any_fail = false;
                    if (!pre_common) {
                        c.record("P2-I1 " + tag + ": NOT GRADED, a precondition failed (CAL-VIDEO/P2-a)");
                        c.record("P2-I2 " + tag + ": NOT GRADED, a precondition failed (CAL-VIDEO/P2-a/P2-b)");
                        c.record("P2-I3 " + tag + ": NOT GRADED, a precondition failed (CAL-VIDEO/P2-a)");
                        graded_all = false;
                    } else {
                        const bool i1 = ev0.i1 && ev1.i1;
                        c.expect(i1, "P2-I1 " + tag + (m1 && mut("P2ITEM1") ? " [contrast: VSTOP one line lower]" : "") +
                                  ": each run shows at least three complete blocks of data lines and every complete block has "
                                  "exactly H beam lines (blocks " + std::to_string(ev0.complete) + "/" + std::to_string(ev1.complete) + ")");
                        any_fail = any_fail || !i1;
                        if (p2b_ok) {
                            const bool i2 = ev0.i2 && ev1.i2;
                            c.expect(i2, "P2-I2 " + tag + (m1 && mut("P2FLIP") ? " [contrast: bit 14 of w(100,5) flipped]" : "") +
                                      ": in every data line of every complete block, each of the 320 low-resolution pixels shows "
                                      "C1 where the bit of w(j,k) is 1 and C0 where it is 0");
                            any_fail = any_fail || !i2;
                        } else {
                            c.record("P2-I2 " + tag + ": NOT GRADED, a precondition failed (P2-b)");
                            graded_all = false;
                        }
                        const bool i3 = ev0.i3 && ev1.i3;
                        c.expect(i3, "P2-I3 " + tag + (m1 && mut("P2EDGE") ? " [contrast: window 32 pixels wider]" : "") +
                                  ": in every data line of every complete block the sample before the first C1 sample and "
                                  "the sample after the 320th pixel show C0");
                        any_fail = any_fail || !i3;
                    }
                    agg2.add(graded_all || any_fail, !any_fail);
                    c.note("P2 " + tag + ": complete blocks run0/run1 = " + std::to_string(ev0.complete) + "/" +
                           std::to_string(ev1.complete) + ", first block length " +
                           std::to_string(ev0.complete_lens.empty() ? 0 : ev0.complete_lens[0]) + " doubler lines, "
                           "first line of run0 whose pixels do not match (block,line) = " +
                           (ev0.i2 ? std::string("(none)") : "(" + std::to_string(ev0.first_bad_block) + "," +
                                                                   std::to_string(ev0.first_bad_line) + ")"));
                }
            }
        }
        if (do_p1 && !dedicated) {
            const std::string ids1 = "opus:3-a1000.c04 sol:3-composite.c6 opus:3-sum.c21 ";
            if (agg1.gradable())
                c.expect(agg1.failed == 0,
                    "opus:3-a1000.c04 sol:3-composite.c6 opus:3-sum.c21 p63 p64 P1 aggregate: changing only bit 9 of BPLCON0 "
                    "leaves every R,G,B sample of two fields unchanged, in the four display configurations A, B, C, E "
                    "and the four system configurations S1-S4");
            else
                c.record(ids1 + "P1 aggregate: NOT GRADED, " + agg1.counts());
        }
        if (do_p2) {
            if (agg2.gradable())
                c.expect(agg2.failed == 0,
                    "opus:3-a1000.c05 p63 P2 aggregate: with BPLCON0 bit 9 clear and set, the RGB output "
                    "shows the picture the playfield registers and bitplane data define, in all four system configurations");
            else
                c.record("opus:3-a1000.c05 P2 aggregate: NOT GRADED, " + agg2.counts());
            c.record("sol:3-composite.c7 declined: its quotation is the second sentence of p.63 alone (\"RGB video is "
                     "correctly generated regardless of the output of the composite video signal.\"); the composite video "
                     "signal is not an output a test can see, and the bit 9 handle is the first sentence's");
        }
    }

    // P1-a's own contrast: PAL for one run, NTSC for the other (config B, S1's
    // and S2's stimulus, both "run 0"), through the same comparator the main
    // loop's P1-a uses (hsync/vsync samples identical over the
    // 2nd-to-4th-vsync span) -- expected to fail by itself, since a genuine PAL
    // run and a genuine NTSC run have different field lengths.
    if (mut("P1A")) {
        DispOpts o; o.list1_is_list0 = true;
        DisplayData dpal = build_and_run_display(dir, 'B', 1, o);
        DisplayData dntsc = build_and_run_display(dir, 'B', 2, o);
        bool fixture_c = dpal.ok && dntsc.ok;
        bool p1a_ok = false;
        if (fixture_c) {
            auto starts0 = dma_field_starts(dpal.run0.log);
            auto starts1 = dma_field_starts(dntsc.run0.log);
            auto vsa0 = find_vsync_assertions(dpal.run0.video);
            auto vsa1 = find_vsync_assertions(dntsc.run0.video);
            std::vector<size_t> w0, w1;
            if (!starts0.empty() && !starts1.empty()) {
                for (auto v : vsa0) if ((uint64_t)v > starts0.front()) w0.push_back(v);
                for (auto v : vsa1) if ((uint64_t)v > starts1.front()) w1.push_back(v);
            }
            fixture_c = w0.size() >= 4 && w1.size() >= 4;
            if (fixture_c) p1a_ok = sync_samples_identical(dpal.run0.video, w0[1], w0[3], dntsc.run0.video, w1[1], w1[3]);
        }
        c.expect(fixture_c, "P1 fixture B S1/S2 [contrast: P1-a, a PAL run and an NTSC run]: both runs loaded, released reset "
                  "and stored their start word, the Copper fetched the list, and at least 4 vsync assertions follow the first Copper fetch in each");
        gated(fixture_c, "P1-a [contrast]", "the fixture failed",
              "P1-a [contrast: a genuine PAL run (S1) compared with a genuine NTSC run (S2), "
              "config B run \"0\" of each] : hsync and vsync samples of the two runs are identical over the "
              "compared span", p1a_ok);
    }

    // ---------------------------------------------------------------
    // P5 -- VBSTRT ($1CC, ECS) and the line at which vertical blanking
    // starts. Claim: opus:B-vbstrt.addr (p.293); opus:C-vbstrt.c01 and sol:C-scan.c20
    // (p.304) are declined. Run in S1, S2 only (the specification's own scope).
    // ---------------------------------------------------------------
    {
        Agg agg5; agg5.expected = 4;
        for (int s = 1; do_p5 && s <= smax_p5; ++s) {
            const bool ntsc = s_is_ntsc(s);
            for (int vidx = 0; vidx < (mv.empty() ? 2 : 1); ++vidx) {
                const uint8_t N = vidx == 0 ? 0xA0 : 0xB0;
                const std::string tag = "V" + std::to_string(vidx + 1) + " S" + std::to_string(s);
                const bool here1 = (s == 1 && vidx == 0);
                std::string bin = p5_bin(dir, ntsc, vidx == 1);
                if (here1 && mut("P5MAIN")) bin = dir + "/prog_p5_pal_v1_novarvben.bin";
                if (here1 && mut("P5A")) bin = dir + "/prog_p5_pal_v1_nobplen.bin";
                if (here1 && mut("P5C")) bin = dir + "/prog_p5_pal_v1_wrongstd.bin";
                if (here1 && mut("P5FIX")) bin = dir + "/prog_p5_pal_v1_badstart.bin";
                if (here1 && mut("P5MARK")) bin = dir + "/prog_p5_pal_v1_badmark.bin";
                nmh::Config cfgp = cfg_for_s(s);
                // the Copper's WAIT before the C2 change: N-1 (P5B contrast: N-2)
                const uint8_t wait_c2 = (here1 && mut("P5B")) ? uint8_t(N - 2) : uint8_t(N - 1);

                P5Run pr = run_p5(bin, cfgp, wait_c2, uint8_t(N + 1));
                std::vector<uint64_t> starts;
                std::vector<size_t> vsa, w;
                std::vector<Detection> det;
                if (pr.ok) {
                    starts = dma_field_starts(pr.vr.log);
                    vsa = find_vsync_assertions(pr.vr.video);
                    det = extract_detections(pr.vr.log, 200);
                    if (!starts.empty()) for (auto v : vsa) if ((uint64_t)v > starts.front()) w.push_back(v);
                }
                const bool fixture_ok = pr.ok && starts.size() >= 5 && w.size() >= 6 && det.size() >= 4;
                c.expect(fixture_ok, "P5 fixture " + tag + ": the program loaded, released reset, stored its start word and "
                          "reached DONE, and enough DMA-field starts, vsync assertions after the first Copper fetch and "
                          "VERTB-synchronized read bursts follow");
                if (!fixture_ok) {
                    for (const char* l : {"P5-M", "CAL-VIDEO", "P5-a", "P5-b", "P5-c", "P5-d"})
                        skipped(std::string(l) + " " + tag, "P5 fixture " + tag);
                    c.record("P5-MAIN " + tag + ": NOT GRADED, the fixture failed");
                    agg5.add(false, false);
                    continue;
                }

                // P5-M: the stores to the measurement buffer are the program's own four blocks.
                const bool m5_ok = blocks_shape_ok(pr.vr.log, 200, 4);
                c.expect(m5_ok, "P5-M " + tag + ": the stores to the measurement buffer are exactly four blocks, block k "
                          "being the marker word k (k = 0..3) followed by 200 long words at consecutive word addresses");

                // CAL-VIDEO: the doubler's units (the span below pairs hs periods into beam lines).
                std::vector<size_t> span(w.begin() + 1, w.begin() + 6);
                VideoCal cal = calibrate_video(pr.vr.video, span, ntsc);
                c.expect(cal.ok, "CAL-VIDEO " + tag + ": between consecutive vsync assertions the output has 2 hs periods "
                          "per page-given line count, each a half beam line of 227 or 228 cc, and a low-resolution "
                          "pixel spans k samples (k=" + std::to_string(cal.k) + ", median doubler line " +
                          std::to_string(cal.median_len) + " samples)");

                // Beam lines (hs periods, doubler copies merged) from the 2nd to the 6th vsync
                // assertion; per line, which of C1/C2/C3 a sample shows. The pairing starts at the
                // first doubler line that shows C1: data starts at the start of a beam line.
                const size_t b = w[1], e = w[5];
                auto dl_raw = nmh::find_lines(pr.vr.video);
                struct LC { bool c1 = false, c2 = false, c3 = false; };
                std::vector<nmh::Span> in_span;
                for (auto& sp : dl_raw) if (sp.begin >= b && sp.end <= e) in_span.push_back(sp);
                size_t first_c1_line = SIZE_MAX;
                for (size_t i = 0; i < in_span.size() && first_c1_line == SIZE_MAX; ++i)
                    for (size_t k = in_span[i].begin; k < in_span[i].end; ++k)
                        if (shows(pr.vr.video[k], kC1)) { first_c1_line = i; break; }
                std::vector<LC> lines;
                if (first_c1_line != SIZE_MAX) {
                    for (size_t i = first_c1_line % kDoublerM; i + 1 < in_span.size(); i += kDoublerM) {
                        LC lc;
                        for (int rep = 0; rep < kDoublerM; ++rep) {
                            nmh::Span sp = in_span[i + rep];
                            for (size_t k = sp.begin; k < sp.end && k < pr.vr.video.size(); ++k) {
                                auto& samp = pr.vr.video[k];
                                if (shows(samp, kC1)) lc.c1 = true;
                                if (shows(samp, kC2)) lc.c2 = true;
                                if (shows(samp, kC3)) lc.c3 = true;
                            }
                        }
                        lines.push_back(lc);
                    }
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

                // P5-a: beam lines N-10..N-2 (9 lines immediately before the first C2 line, which is
                // beam line N-1) each show C1.
                int idx_c2 = c2_pos.empty() ? -1 : (int)c2_pos.front();
                bool p5a_ok = idx_c2 >= 9;
                if (p5a_ok) for (int i = idx_c2 - 9; i < idx_c2; ++i) if (!lines[i].c1) p5a_ok = false;
                c.expect(p5a_ok, "P5-a " + tag + (here1 && mut("P5A") ? " [contrast: DMACON without BPLEN]" : "") +
                          ": beam lines N-10..N-2 each show C1");

                // P5-b (anchor): the VPOSR/VHPOSR reads around each of VERTB detections 2-4 bracketing the
                // Copper's COLOR01 changes show V=N-1 (C2 change) and V=N+1 (C3 change).
                const bool p5b_ok = gated(m5_ok, "P5-b " + tag, "P5-M failed",
                    "P5-b " + tag + (here1 && mut("P5B") ? " [contrast: WAIT(N-2,$30) before COLOR01=C2]" : "") +
                    ": the VPOSR/VHPOSR reads bracketing the Copper's COLOR01 change "
                    "show V=N-1 (before/at the C2 change) and V=N+1 (before/at the C3 change)",
                    p5_anchor(pr.vr, det, pr.cop.addr_c2, (uint8_t)(N - 1)) &&
                    p5_anchor(pr.vr, det, pr.cop.addr_c3, (uint8_t)(N + 1)));

                // P5-c: the field length matches the configured standard, on consecutive detection markers (with
                // the period of the program's two polling loops, measured from their instruction fetches, added to
                // the tolerance of one line) and on consecutive vsync assertions.
                const uint64_t loop5_intreq = poll_loop(pr.vr.prog_words, pr.vr.log).period;
                const uint64_t loop5_vhpos = poll_loop(pr.vr.prog_words, pr.vr.log, 0x3228, 0x0006).period;
                const uint64_t loop5 = std::max(loop5_intreq, loop5_vhpos);
                bool marker_fl = det.size() > 1;
                for (int i = 0; i + 1 < (int)det.size(); ++i)
                    marker_fl = marker_fl && field_length_ok(det[i + 1].marker_cycle - det[i].marker_cycle, ntsc, loop5);
                const bool p5c_ok = gated(m5_ok, "P5-c " + tag, "P5-M failed",
                    "P5-c " + tag + ": consecutive detection markers and consecutive vsync assertions recur at the "
                    "configured standard's field length",
                    marker_fl && vsync_field_length_ok(vsa, ntsc));
                c.note("P5-c " + tag + ": polling-loop period " + std::to_string(loop5) + " clk_sys cycles (INTREQR loop " +
                       std::to_string(loop5_intreq) + ", VHPOSR loop " + std::to_string(loop5_vhpos) + ")");

                // P5-d: the harness configuration of the run (p.298 lists VBSTRT and BEAMCON0 as new registers of
                // the Enhanced Chip Set). A configuration of the host, not a measurement: recorded, not graded.
                c.record("P5-d " + tag + ": the run was started with the harness configured " +
                         std::string(cfg_is_ecs(cfgp) ? "ECS" : "OCS") + " (a host setting, not graded)");

                if (!(m5_ok && cal.ok && p5a_ok && p5b_ok && p5c_ok)) {
                    c.record("P5-MAIN " + tag + ": NOT GRADED, a precondition failed (P5-M/CAL-VIDEO/P5-a/P5-b/P5-c)");
                    agg5.add(false, false);
                } else {
                    const bool main_ok = item1_ok && item2_ok;
                    c.expect(main_ok,
                        "P5-MAIN " + tag + (here1 && mut("P5MAIN") ? " [contrast: BEAMCON0 without VARVBEN]" : "") +
                        ": every beam line showing C2 is immediately preceded by a line showing C1 (not C2) and "
                        "immediately followed by a line showing none of C1/C2/C3; at least 3 such lines, one field apart");
                    agg5.add(true, main_ok);
                }
                std::string s5 = "P5 " + tag + ": first graded field, lines N-2..N+8 (N = the line VBSTRT names):";
                for (long k = -2; !c2_pos.empty() && c2_pos.front() >= 1 && c2_pos.front() + 1 + k < lines.size() && k <= 8; ++k) {
                    const auto& l = lines[c2_pos.front() + 1 + k];
                    s5 += " N" + std::string(k > 0 ? "+" : "") + (k ? std::to_string(k) : "") + "=" + (l.c1 || l.c2 || l.c3 ? std::string(l.c1 ? "C1" : "") + (l.c2 ? "C2" : "") + (l.c3 ? "C3" : "") : "none");
                }
                c.note(s5);
            }
        }
        if (do_p5) {
            if (agg5.gradable())
                c.expect(agg5.failed == 0,
                    "opus:B-vbstrt.addr p293 p304 p305 p79 P5 aggregate: with VARVBEN set "
                    "the line before the line VBSTRT names shows playfield data and the line it names does not, for "
                    "VBSTRT=$A0 and $B0, in PAL and NTSC");
            else
                c.record("opus:B-vbstrt.addr P5 aggregate: NOT GRADED, " + agg5.counts());
            c.record("opus:C-vbstrt.c01 sol:C-scan.c20 declined: the quotation also names the register's write-only access "
                     "(W) and the chip that holds it (A), and no page says what a read of $DFF1CC returns and no output "
                     "of the test shows which chip holds a register; only the VBLANK start line is graded, by the P5 aggregate");
        }
    }

    std::printf("%d passed, %d failed\n", c.passed, c.failed);
    return c.failed == 0 ? 0 : 1;
}
