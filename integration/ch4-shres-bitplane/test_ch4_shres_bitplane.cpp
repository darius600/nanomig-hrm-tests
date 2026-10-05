// test_ch4_shres_bitplane.cpp -- ch4-shres-bitplane: SuperHires bitplane
// colour encoding. See SPECIFICATION.md for every proposition's page,
// stimulus, observation and expected value; this file only implements
// what SPECIFICATION.md already committed to (section 3's palettes and
// pattern generator, section 4's stimulus, section 5's reference frames,
// section 6's preconditions, section 7's propositions).
//
// The Copper-list and pixel-stream data poked into chip RAM by the 68000
// program (prog_main.s and its mutation wrappers) is data transcribed from
// SPECIFICATION.md sections 3 and 4; this file re-derives the same literal
// tables independently (not by reading the data tables' own bytes) so the
// check is against the specification, not against how the tables were
// built.
//
// NMH_MUTATE selects one mutation in place of the normal run; none is
// exercised by a plain `make test`.
//
// Every record below names the clause a mutation must turn red through, and
// what else moves with it (a mutation is a single stimulus change; the
// preconditions it shares with other clauses move too, honestly, not
// trimmed to look surgical). "->" separates the record's own targeted
// clause from the rest of the cascade. Counts are fx68k's; TG68K's PASS,
// FAIL and RECORDED lines are identical, line for line, on every record below.
//
// MUTATION fixture NMH_MUTATE=FIX: the program stores $DEAD instead of $BEEF -> fixture only (74 passed, 38 failed)
// MUTATION C1 NMH_MUTATE=C1: OCS instead of ECS (harness Config, no program change) -> C1, and every SuperHires-dependent check downstream: C5/R2, R2 (F2-F4, and the bottom-line check), C6 (F1 diversity), C8 (F1-F4 pair coverage), all twelve P1.cX, P2's three palette-diversity checks; P4, P1J and the V4c checks print nothing at all (their own frames never decode); C5-F7F10 goes red with them (25 passed, 32 failed)
// MUTATION C2 NMH_MUTATE=C2: the program never writes DONE -> C2 only; grading stops there, by the comparator's own rule (3 passed, 1 failed)
// MUTATION C3 NMH_MUTATE=C3: F1's BPLCON0=$1040 (one bitplane, only plane-1 fetched) -> C3, and F1's own downstream: C4 (header), C5/R2, C6's old diversity check, R2's bottom-line check, P2's three palette-diversity checks; P2.R/G/B themselves turn PASS, since F2-F4's own Q0 lines still populate them once F1 drops out (70 passed, 42 failed)
// MUTATION C4 NMH_MUTATE=C4: F5's list points at itself instead of F6 -> C4, and F6's own downstream: C7 (both the band count and the timing check), C6 (F6), C3 (the F1-F6 aggregate), R1 (the F1-F6 aggregate), R2 (F6's lores mapping); F7-F10 never run: C4-F7F10, C5-F7F10, C7-F7F10, C10, R1-F7F10 (61 passed, 48 failed)
// MUTATION C5/R2 NMH_MUTATE=C5: F1's BPLCON0=$A000 (hires instead of SuperHires) -> C5/R2, C4 (header), and P2's three palette-diversity checks; P2.R/G/B themselves turn PASS on F2-F4's own Q0 lines -- P2's own green control, on the same comparator, with the diversity check honestly reporting why (73 passed, 39 failed)
// MUTATION C6 NMH_MUTATE=C6: F1's DIWSTRT/DIWSTOP narrowed to a 64-lores-pixel window -> C6, and F1's own downstream: C4 (header), C5/R2, C6's old diversity check, R2's bottom-line check, P2's three palette-diversity checks; P2.R/G/B turn PASS the same way as under C5 (71 passed, 41 failed)
// MUTATION C7 NMH_MUTATE=C7: F1 band 1's WAIT at horizontal $80 instead of $00 -> C7's own timing check only; the band-count check and every other line stay at baseline (74 passed, 38 failed)
// MUTATION C8 NMH_MUTATE=C8: the pattern section restricted to values 0 and 3 -> C8's own pair-coverage check, and C6's old diversity check; eighteen P4 bits that FAIL at baseline and twelve that PASS become NOT OBSERVABLE instead (the restricted pattern no longer puts every value on the perturbed lines), not a new FAIL or a lost one (55 passed, 27 failed)
// MUTATION C9 NMH_MUTATE=C9: F1's SPR0PT points at the real sprite 0 structure -> C9, and all twelve P1.cX (sprite 0 draws over F1's own playfield) (62 passed, 50 failed)
// MUTATION P1.3R NMH_MUTATE=V1: F1 writes Q3x instead of Q3 for the Q3 band -> all twelve P1.cX, through both letters of every component, on the Q3 band's pixels (63 passed, 49 failed)
// MUTATION C5/R2 NMH_MUTATE=V2: F1 run in hires (BPLCON0=$A000) -> identical to C5 (same list, bp_cop_f1_v2.i); not the specification's V2, P2's own hires contrast, which the package does not build (73 passed, 39 failed)
// MUTATION P3.H/P3.L/P7.1 NMH_MUTATE=V3A: F1 in hires with S0-S3 replaced by a COLOR01 sweep -> C4 (header), C5/R2, C7's band count, P2's three palette-diversity checks, and P2.R/G/B turning PASS the same way as under C5; P3.H, P3.L and P7.1 stay FAIL, already red at baseline through their own SuperHires half (72 passed, 40 failed)
// MUTATION P3.H NMH_MUTATE=V3B: F5's COLOR01 sweep masked with $0CCC -> no new red: P3.H is already FAIL at baseline (its SuperHires half). MEASURED confirms the targeted mechanism still moves: the Hires side drops from 16 to 4 distinct values under this mutation (75 passed, 37 failed)
// MUTATION P3.L NMH_MUTATE=V3C: F6's COLOR01 sweep masked with $0CCC -> no new red, for the same reason as V3B; MEASURED confirms the Lores side drops from 16 to 4 distinct values (75 passed, 37 failed)
// MUTATION P4.n00b11 NMH_MUTATE=V4A: F2 band 11 (n=00,b=11) also inverts COLOR01 bit11 (another Color) -> P4.n00b11 only, of all 204 P4/P1J lines (74 passed, 38 failed)
// MUTATION C4/R2 NMH_MUTATE=V4B: F2-F4 run in hires (BPLCON0=$A000) -> C4 (header) and R2 (F2-F4's own mapping, now checked for the SuperHires form specifically); no P4 or P1J line moves, and C7's band count holds (F4's own list carries its closing restore band) (73 passed, 39 failed)
// MUTATION P5.L NMH_MUTATE=V5: F2-F4 run in hires (BPLCON0=$A000, same build as V4B) -> identical cascade to V4B (C4, R2); P5.L's SuperHires half stays FAIL, already red at baseline -- this run shows nothing new about it, and no mutation in this package can turn that comparator green through a live input (see the RECORDED line next to it) (73 passed, 39 failed)
// MUTATION P4 (24 p-half bits of COLOR00-03) NMH_MUTATE=V4D: F2-F4, every band also flips its partner bit -> exactly the 24 p-half bits, all PASS at baseline, turn FAIL; no other P4/P1J line moves (51 passed, 61 failed)
// MUTATION P5.L (Hires half) NMH_MUTATE=V5B: F5's COLOR05 band inverts COLOR01 instead -> "registers COLOR04-0F change no graded pixel in Hires mode" only (74 passed, 38 failed)
//
// F7-F10 (sprite colour frames), counts as above. A comparator of P5.U, P6.1 or P6.2 is not graded while C11 fails (it fails on both cores at baseline), so
// its verdict under a mutation is the MEASURED line "<id> comparator without the C11 gate (not graded) gives PASS|FAIL" and the RECORDED line
// "NOT GRADED, C11 did not hold"; "->" below names the verdict that line gives.
//
// MUTATION C4-F7F10 stimulus red NMH_MUTATE=C4B: F8's header writes BPLCON2 = $0000 instead of $0024 -> C4-F7F10 only; every F7-F10 comparator is then not graded (73 passed, 38 failed)
// MUTATION C4-F7F10 stimulus red NMH_MUTATE=C4C: F7's list points at itself instead of F8, so F8-F10 never run -> C4-F7F10 (frame order), and on the frames that never ran C10 (F8-F10 fetch no sprite data), C5-F7F10 and C7-F7F10 (70 passed, 41 failed)
// MUTATION C5-F7F10 stimulus red NMH_MUTATE=V6C: F7 runs with BPLCON0 = $A000 -> C5-F7F10 (no s fits F7's SuperHires mapping; the hires mapping fits, s = 141), with C4-F7F10 (the planned header) and RUN-ENDS (F7's runs are 32 periods long and end at 300, F8's end at 284); every F7-F10 comparator is then not graded (71 passed, 40 failed)
// MUTATION C7-F7F10 stimulus red NMH_MUTATE=C7B: the block 3 band's WAIT of F7 at horizontal $80 instead of $00 -> C7-F7F10 only, through bad_h=1 and bad_before=1 (its MOVEs land after the line's first bitplane fetch); every F7-F10 comparator is then not graded (73 passed, 38 failed)
// MUTATION C10 stimulus red NMH_MUTATE=C10: F7's SPR0PT points at the null sprite -> C10 (F7: pairs=0, null_reads=16); C11 and RUN-ENDS are not graded (no F7 line shows a run), and so is every F7-F10 comparator (72 passed, 37 failed)
// MUTATION C11 stimulus red NMH_MUTATE=C11: sprite 0's data words are $FF00 where they are $FFFF (8 pixels wide) -> C11 stays FAIL, as at baseline (F7 and F8 16 periods); F10's run is 16 periods instead of 32; C10 holds, every other line is at baseline (75 passed, 37 failed)
// MUTATION C11 stimulus green NMH_MUTATE=C11G: F7 and F8 run with BPLCON0 = $A000 -> C11 PASS through the same comparator (runs of 32 periods in F7, F8 and F10), with C4-F7F10 and C5-F7F10 red (73 passed, 38 failed)
// MUTATION RUN-ENDS stimulus red NMH_MUTATE=RUNENDS: F7's band at beam $5C changes COLOR00 to $0F00 for the rest of the frame -> RUN-ENDS (the run ends on 6 of 21 F7 baseline lines are [269,284]; the other 15 lines differ from the background over the whole interval); C11 stays FAIL; every F7-F10 comparator is then not graded (74 passed, 38 failed)
// MUTATION R1-F7F10 none impossible: not exercised, by a scope decision -- the copies of a source line are two read-outs of one stored line; no stimulus of this package changes one read-out and not the other, and none was found that does
// MUTATION P6.1-F9PF stimulus red NMH_MUTATE=V6Q: F9's band change at line 30 writes COLOR00-0F = Q0 instead of Q1 -> P6.1-F9PF (544 of 544 positions equal); P6.1 is then not graded (74 passed, 38 failed)
// MUTATION P6.1 stimulus red NMH_MUTATE=V6A: F7 writes COLOR16-31 = T0c while the expectation stays T0 -> P6.1's comparator gives FAIL (F7 clause: 882 of 882 component values differ, F9 clause: 840 of 840 pixels differ from F7's baseline); the F8 clause holds (75 passed, 37 failed)
// MUTATION P6.1 stimulus red NMH_MUTATE=V6E: F8 writes COLOR10-1F = T0 while the expectation stays T0c -> P6.1's comparator gives FAIL through the F8 clause only: 2520 of 2520 component values differ from the letters of T0c (75 passed, 37 failed)
// MUTATION P6.1 stimulus red NMH_MUTATE=V6B: F9 writes COLOR10-1F = T0c -> P6.1's comparator gives FAIL through the F9 clause only: 840 of 840 inner pixels differ from F7's baseline (75 passed, 37 failed)
// MUTATION P6.2-SHR stimulus green NMH_MUTATE=V6S: the block of F7 planned to invert COLOR20 inverts COLOR17 instead -> P6.2-SHR's comparator gives PASS (it gives FAIL at baseline) through the same comparator: 14 inner pixels of block 3's value-1 line change (75 passed, 37 failed)
// MUTATION P6.2-HIRES stimulus red NMH_MUTATE=V6D: the block of F10 planned to invert COLOR20 inverts COLOR17 instead -> P6.2-HIRES's comparator gives FAIL; P6.2-LIVE and every other line hold (75 passed, 37 failed)
// MUTATION P6.2-LIVE stimulus red NMH_MUTATE=V6L: the block of F10 planned to invert COLOR17 inverts COLOR20 instead -> P6.2-LIVE's comparator gives FAIL (the lines of value 1 in block 15 do not invert); P6.2-HIRES holds (75 passed, 37 failed)
// MUTATION P6.2 stimulus green NMH_MUTATE=V6S: as above -> P6.2's comparator gives PASS (SuperHires half, Hires half and Hires comparator all hold), where it gives FAIL at baseline through the SuperHires half (75 passed, 37 failed)
// MUTATION P6.2 stimulus red NMH_MUTATE=V6D: as above -> P6.2's comparator gives FAIL through the Hires half, the SuperHires half being FAIL as well at baseline (75 passed, 37 failed)
// MUTATION P5.U stimulus red NMH_MUTATE=V5C: block 2 of F7 also writes Color 0's inverted letters into the seven lower registers that hold them, block 3 restores them (the first lines of blocks 2 and 3 are spacer lines) -> P5.U's comparator gives FAIL: 1088 of 20128 compared playfield pixels change; every other comparator keeps its baseline verdict (75 passed, 37 failed)
// MUTATION P7.2 none impossible: no stimulus of this package makes P5.L's SuperHires half pass (no register COLOR04-0F changes a SuperHires pixel on either core), and P7.2 is FAIL through that half whatever P6.2 gives; P6.2's own half is exercised by P6.2's records
// MUTATION P7.3 none impossible: no stimulus of this package makes P5.L's SuperHires half pass, and P7.3 is FAIL through that half whatever P5.U gives; P5.U's own half is exercised by P5.U's records
//
// prog.bin loaded, cpu_nrst_out released within the boot budget: not
// exercised, by a scope decision -- both are harness/boot outcomes settled
// before the program's first instruction executes; no contrast in this
// package changes anything before the program starts.

#include <algorithm>
#include <array>
#include <climits>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

// ---- the fixed assertion call form (BRIEF.md) -----------------------------
struct Checks {
    void expect(bool condition, const std::string& label) {
        std::printf("%s: %s\n", condition ? "PASS" : "FAIL", label.c_str());
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::printf("MEASURED: %s\n", msg.c_str()); }
    void record(const std::string& msg) { std::printf("RECORDED: %s\n", msg.c_str()); }
    int passed = 0, failed = 0;
};

// ---- addresses (nanomig.i / ch4bp.i / SPECIFICATION.md 4.2, restated) ----
constexpr uint32_t kResults   = 0x0C0000;
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;

constexpr uint32_t kPlane1Buf = 0x010000;
constexpr uint32_t kPlane2Buf = 0x020000;
constexpr uint32_t kZeroBuf   = 0x030000;
constexpr uint32_t kSpr0Struct= 0x040000;
constexpr uint32_t kNullSpr   = 0x040100;
constexpr uint32_t kCopBase   = 0x041000;
constexpr uint32_t kCopStep   = 0x001000;
constexpr size_t   kBufWords  = 32768;

constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudget  = 60'000'000;  // 12 vertical blanks plus margin (measured)

uint32_t word_addr(uint32_t byte_addr) { return byte_addr / 2; }
uint32_t cop_addr(int frame) { return word_addr(kCopBase + kCopStep * (uint32_t)frame); }
int frame_of_word(uint32_t wa) {
    uint32_t base = word_addr(kCopBase);
    if (wa < base || wa >= base + 11u * (kCopStep/2)) return -1;
    return (int)((wa - base) / (kCopStep/2));
}

// ---- section 3's palettes, transcribed literally from SPECIFICATION.md ----
const std::vector<uint16_t> kQ0 = {0x005A,0x049E,0x08D2,0x0C16,0x016B,0x05AF,0x09E3,0x0D27,
                                    0x0278,0x06BC,0x0AF0,0x0E34,0x0349,0x078D,0x0BC1,0x0F05};
const std::vector<uint16_t> kQ1(16, 0x0AAA);
const std::vector<uint16_t> kQ2 = {0x05A5,0x0969,0x05A5,0x0969,0x0696,0x0A5A,0x0696,0x0A5A,
                                    0x05A5,0x0969,0x05A5,0x0969,0x0696,0x0A5A,0x0696,0x0A5A};
const std::vector<uint16_t> kQ3 = {0x005F,0x0817,0x0C93,0x04DB,0x024D,0x0A05,0x0E81,0x06C9,
                                    0x036C,0x0B24,0x0FA0,0x07E8,0x017E,0x0936,0x0DB2,0x05FA};
const std::vector<uint16_t> kQ4 = {0x0000,0x04CC,0x0C80,0x0004,0x0133,0x05FF,0x0DB3,0x0137,
                                    0x0320,0x07EC,0x0FA0,0x0324,0x0001,0x04CD,0x0C81,0x0005};
const std::vector<uint16_t> kQ5 = {0x0000,0x0000,0x004C,0x0CCC,0x0000,0x0000,0x004C,0x0CCC,
                                    0x0013,0x0013,0x005F,0x0CDF,0x0333,0x0333,0x037F,0x0FFF};
const std::vector<uint16_t> kZ(16, 0x0000);
const std::vector<uint16_t> kO(16, 0x0FFF);
const std::vector<uint16_t> kS0 = {0x005A,0x0012,0x08D2,0x0C16,0x0048,0x0000,0x08C0,0x0C04,
                                    0x0278,0x0230,0x0AF0,0x0E34,0x0349,0x0301,0x0BC1,0x0F05};
const std::vector<uint16_t> kS1 = {0x005A,0x0456,0x08D2,0x0C16,0x0159,0x0555,0x09D1,0x0D15,
                                    0x0278,0x0674,0x0AF0,0x0E34,0x0349,0x0745,0x0BC1,0x0F05};
const std::vector<uint16_t> kS2 = {0x005A,0x089A,0x08D2,0x0C16,0x026A,0x0AAA,0x0AE2,0x0E26,
                                    0x0278,0x0AB8,0x0AF0,0x0E34,0x0349,0x0B89,0x0BC1,0x0F05};
const std::vector<uint16_t> kS3 = {0x005A,0x0CDE,0x08D2,0x0C16,0x037B,0x0FFF,0x0BF3,0x0F37,
                                    0x0278,0x0EFC,0x0AF0,0x0E34,0x0349,0x0FCD,0x0BC1,0x0F05};
const std::vector<uint16_t> kQ3x= {0x0FA0,0x07E8,0x036C,0x0B24,0x0DB2,0x05FA,0x017E,0x0936,
                                    0x0C93,0x04DB,0x005F,0x0817,0x0E81,0x06C9,0x024D,0x0A05};

// F1's own band order and palettes (SPECIFICATION.md 4.6)
const std::vector<std::pair<std::string,const std::vector<uint16_t>*>> kF1Bands = {
    {"Q1",&kQ1},{"Q2",&kQ2},{"Q3",&kQ3},{"Q4",&kQ4},{"Q5",&kQ5},{"Z",&kZ},{"O",&kO},
    {"S0",&kS0},{"S1",&kS1},{"S2",&kS2},{"S3",&kS3},
};

// ---- section 4.2's xorshift generator, reimplemented independently for
// C3's "data equal to what the program wrote" check ------------------------
std::pair<std::vector<uint16_t>, std::vector<uint16_t>> build_pattern() {
    std::vector<uint16_t> p1(32512, 0), p2(32512, 0);
    uint32_t x = 0x2545F491u;
    for (int k = 0; k < 32512; ++k) {
        uint16_t w1 = 0, w2 = 0;
        for (int j = 0; j < 16; ++j) {
            x ^= (x << 13); x ^= (x >> 17); x ^= (x << 5);
            uint32_t v = x >> 30;
            int bit = 15 - j;
            if (v & 1) w1 |= (uint16_t)(1u << bit);
            if (v & 2) w2 |= (uint16_t)(1u << bit);
        }
        p1[k] = w1; p2[k] = w2;
    }
    return {p1, p2};
}

// ---- one captured run -------------------------------------------------
struct Capture {
    bool loaded=false, released=false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
    std::vector<nmh::VideoSample> video;
    std::optional<uint16_t> fixture, vposr, deniseid;
};

Capture run_case(const std::string& dir, const std::string& prog, const nmh::Config& cfg) {
    Capture c;
    nmh::NanomigHarness h(cfg);
    c.loaded = h.load_program(dir + "/" + prog + ".bin") > 0;
    if (!c.loaded) return c;
    c.released = h.boot(kBootBudget).cpu_released;
    c.done = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    c.log = h.ram_log();
    c.video = h.video();
    c.fixture  = h.peek_logical(kResults + 0);
    c.vposr    = h.peek_logical(kResults + 2);
    c.deniseid = h.peek_logical(kResults + 4);
    return c;
}

// ---- Copper-list decoding (R5): one pass per frame -------------------------
struct CopEvent { uint64_t cycle; bool is_wait; uint16_t w1, w2; };

std::vector<CopEvent> parse_frame_events(const std::vector<nmh::RamAccess>& log, int frame) {
    uint32_t base = cop_addr(frame);
    std::vector<nmh::RamAccess> reads;
    for (auto& a : log) {
        if (a.is_write) continue;
        if (a.word_addr < base || a.word_addr >= base + kCopStep/2) continue;
        reads.push_back(a);
    }
    std::stable_sort(reads.begin(), reads.end(), [](auto&a, auto&b){ return a.cycle < b.cycle; });
    auto c = nmh::coalesce(reads);
    std::vector<CopEvent> events;
    for (size_t i = 0; i + 1 < c.size(); i += 2) {
        uint16_t w1 = c[i].data, w2 = c[i+1].data;
        bool is_wait = (w1 & 1) != 0;
        events.push_back({c[i].cycle, is_wait, w1, w2});
        if (is_wait && w1 == 0xFFFF && w2 == 0xFFFE) break;
    }
    return events;
}

struct Band {
    uint64_t cycle; int beam; int line;
    uint16_t wait_w1 = 0;          // the WAIT event's own first word (C7: horizontal field)
    // The WAIT instruction word is FETCHED as soon as the Copper reaches it
    // in the list (immediately after the previous band's own last MOVE),
    // well before its condition is met; `cycle` is that fetch, not useful
    // for ordering. `first_move_cycle` is the cycle of the band's own first
    // MOVE, fetched only once the WAIT actually releases -- the right
    // reference point for "after the previous line's last bitplane fetch".
    uint64_t first_move_cycle = 0;
    uint64_t last_event_cycle = 0; // cycle of the last MOVE fetched in this band (C7: ordering)
    std::vector<std::pair<uint16_t,uint16_t>> moves;
};

struct FrameCop {
    bool found = false;
    std::map<uint16_t,uint16_t> header;               // register -> value, in header order
    std::vector<Band> bands;                            // one per WAIT (excluding the terminator)
    uint64_t start_cycle = 0, end_cycle = 0;
};

FrameCop decode_frame(const std::vector<nmh::RamAccess>& log, int frame) {
    FrameCop fc;
    auto ev = parse_frame_events(log, frame);
    if (ev.empty()) return fc;
    fc.found = true;
    fc.start_cycle = ev.front().cycle;
    fc.end_cycle = ev.back().cycle;
    bool in_header = true;
    for (auto& e : ev) {
        if (e.is_wait) {
            if (e.w1 == 0xFFFF && e.w2 == 0xFFFE) break; // terminator
            in_header = false;
            Band b; b.cycle = e.cycle; b.beam = (e.w1 >> 8) & 0xFF; b.line = b.beam - 0x2C;
            b.wait_w1 = e.w1; b.last_event_cycle = e.cycle;
            fc.bands.push_back(b);
        } else if (in_header) {
            fc.header[e.w1] = e.w2;
        } else {
            if (fc.bands.back().moves.empty()) fc.bands.back().first_move_cycle = e.cycle;
            fc.bands.back().moves.push_back({e.w1, e.w2});
            fc.bands.back().last_event_cycle = e.cycle;
        }
    }
    return fc;
}

// replay header + bands up to (not including) band index `upto` to get the
// effective register state at that point (SPECIFICATION.md 4.7's "rebuilt
// from the Copper MOVEs fetched before the line's first bitplane fetch").
std::map<uint16_t,uint16_t> effective_regs(const FrameCop& fc, size_t upto_band) {
    std::map<uint16_t,uint16_t> regs = fc.header;
    for (size_t i = 0; i < upto_band && i < fc.bands.size(); ++i)
        for (auto& mv : fc.bands[i].moves) regs[mv.first] = mv.second;
    return regs;
}

uint16_t color_reg(int n) { return (uint16_t)(0x180 + 2*n); } // n=0..31

// ---- bitplane fetch bursts (source lines) ----------------------------------

struct SourceLine {
    uint64_t cycle_lo, cycle_hi;
    std::vector<uint16_t> p1, p2;          // fetched data, in address order
    std::vector<uint32_t> p1_off, p2_off;  // word offset within the buffer
    bool pure_r = false;
    bool zero = false;                     // every fetched word came from the zero buffer
};

// Groups plane1/plane2 reads within [c_lo,c_hi) into per-line bursts by a
// cycle-gap threshold. `also_zero`: for F7+ the reload band switches the
// pointers to the zero buffer partway through -- callers pass the buffer
// base that applies (plane1/plane2, or the zero buffer) via `p1base/p2base`.
std::vector<SourceLine> find_source_lines(const std::vector<nmh::RamAccess>& log,
                                           uint64_t c_lo, uint64_t c_hi,
                                           uint32_t p1base, uint32_t p2base) {
    uint32_t b1 = word_addr(p1base), b2 = word_addr(p2base);
    std::vector<nmh::RamAccess> reads;
    for (auto& a : log) {
        if (a.is_write) continue;
        if (a.cycle < c_lo || a.cycle >= c_hi) continue;
        bool in1 = a.word_addr >= b1 && a.word_addr < b1 + kBufWords;
        bool in2 = a.word_addr >= b2 && a.word_addr < b2 + kBufWords;
        if (in1 || in2) reads.push_back(a);
    }
    std::stable_sort(reads.begin(), reads.end(), [](auto&a,auto&b){ return a.cycle < b.cycle; });
    auto co = nmh::coalesce(reads);
    std::vector<SourceLine> lines;
    constexpr uint64_t kGap = 300; // cycles; a raster line's active fetch is a
                                    // contiguous burst, separated from the next
                                    // line's burst by a much longer blanking gap
    for (auto& a : co) {
        if (lines.empty() || a.cycle - lines.back().cycle_hi > kGap) {
            lines.push_back(SourceLine{a.cycle, a.cycle, {}, {}, {}, {}, false});
        }
        auto& L = lines.back();
        L.cycle_hi = a.cycle;
        if (a.word_addr >= b1 && a.word_addr < b1 + kBufWords) {
            L.p1.push_back(a.data); L.p1_off.push_back(a.word_addr - b1);
        } else {
            L.p2.push_back(a.data); L.p2_off.push_back(a.word_addr - b2);
        }
    }
    for (auto& L : lines) {
        bool pure = !L.p1.empty();
        for (auto d : L.p1) if (d != 0xFFFF) pure = false;
        for (auto d : L.p2) if (d != 0xFFFF) pure = false;
        L.pure_r = pure;
    }
    return lines;
}

// ---- video helpers ----------------------------------------------------------

struct RGB2 { int r,g,b; }; // bits 3-2 of each channel
RGB2 top2(const nmh::VideoSample& s) { return { (s.r>>2)&3, (s.g>>2)&3, (s.b>>2)&3 }; }
bool eq2(const RGB2&a, const RGB2&b){ return a.r==b.r && a.g==b.g && a.b==b.b; }

struct Full12 { int r,g,b; }; // all 4 bits of each channel
Full12 full12(const nmh::VideoSample& s) { return { s.r, s.g, s.b }; }
bool eqfull(const Full12&a, const Full12&b){ return a.r==b.r && a.g==b.g && a.b==b.b; }

// ---- R1: which output-line span(s) a source-line burst is shown on --------
// (see the file header note on why direct cycle-range overlap is used
// instead of deriving an abstract m/Delta formula first).

struct LineMap { std::vector<size_t> span_idx; }; // indices into find_lines()

LineMap map_source_to_output(const std::vector<nmh::Span>& out_lines, uint64_t c_lo, uint64_t c_hi) {
    LineMap lm;
    for (size_t i = 0; i < out_lines.size(); ++i) {
        // overlap test: the burst's cycle range intersects the output line's
        // sample-index range (sample index == clk_sys cycle, see file header)
        if (out_lines[i].end > c_lo && out_lines[i].begin <= c_hi) lm.span_idx.push_back(i);
    }
    return lm;
}

// ---- pixel value from fetched words (R2) -----------------------------------
// v[i] = bit0 of plane1's word, bit1 of plane2's word, MSB (bit 15) first.
int pixel_value(const SourceLine& L, int i) {
    if (i < 0) return -1;
    size_t k = (size_t)(i / 16);
    int bitpos = 15 - (i % 16);
    if (k >= L.p1.size() || k >= L.p2.size()) return -1;
    int b1 = (L.p1[k] >> bitpos) & 1;
    int b2 = (L.p2[k] >> bitpos) & 1;
    return b1 | (b2 << 1);
}
int fetch_words(const SourceLine& L) { return (int)std::min(L.p1.size(), L.p2.size()); }

// ---- R0: hires-period sample width w and phase phi -------------------------
struct WPhi { int w = 0, phi = 0; bool ok = false; };

WPhi measure_w_phi(const std::vector<nmh::VideoSample>& video, nmh::Span span) {
    std::vector<std::pair<size_t,size_t>> runs;
    size_t i = span.begin;
    while (i < span.end) {
        size_t j = i + 1;
        while (j < span.end && eqfull(full12(video[j]), full12(video[i]))) ++j;
        runs.push_back({i, j});
        i = j;
    }
    if (runs.size() < 5) return {};
    int w = INT_MAX;
    for (size_t k = 1; k + 1 < runs.size(); ++k)
        w = std::min(w, (int)(runs[k].second - runs[k].first));
    if (w <= 0 || w == INT_MAX) return {};
    for (size_t k = 1; k + 1 < runs.size(); ++k)
        if ((int)(runs[k].second - runs[k].first) % w != 0) return {};
    int phi = -1;
    for (size_t k = 1; k + 1 < runs.size(); ++k) {
        int ph = (int)((runs[k].first - span.begin) % (size_t)w);
        if (phi < 0) phi = ph;
        else if (phi != ph) return {};
    }
    return {w, phi, true};
}

// ---- R3: the check interval on a pure-R line's output span ----------------
// The run of one constant colour bounded by different colour (or the edge
// of the span) on both sides; xL/xR are absolute video-sample indices.
struct Run12 { size_t lo = 0, hi = 0; bool ok = false; }; // [lo,hi)

// mode 0 (SuperHires): compares bits 3-2 only (L2: bits 1-0 carry no letter
// and are not predicted). mode 1/2 (hires/lores): compares all twelve bits
// (SPECIFICATION.md R3: "The value compared is bits 3-2 of R,G,B in
// SuperHires frames and all twelve bits in hires and lores frames.")
Run12 find_constant_run(const std::vector<nmh::VideoSample>& video, nmh::Span span, int mode) {
    Run12 best;
    size_t i = span.begin;
    auto same = [&](size_t a, size_t b) {
        if (mode == 0) return eq2(top2(video[a]), top2(video[b]));
        return eqfull(full12(video[a]), full12(video[b]));
    };
    while (i < span.end) {
        size_t j = i + 1;
        while (j < span.end && same(j, i)) ++j;
        if (j - i > best.hi - best.lo) { best.lo = i; best.hi = j; best.ok = true; }
        i = j;
    }
    // Discard a run touching either edge of the span (blanking/border, not
    // bounded by a colour change on that side).
    if (best.ok && (best.lo == span.begin || best.hi == span.end)) best.ok = false;
    return best;
}

// ---- R1+R2 combined: given a frame's source lines and w/phi/mode, find the
// unique s that makes bits3-2 (SuperHires) or all 12 bits (hires/lores) a
// consistent function of v[i], over every graded pixel of a set of lines.
// mode: 0=SuperHires (i=2x-s), 1=hires (i=x-s), 2=lores (i=floor((x-s)/2))
int pixel_index(int mode, int x, int s) {
    if (mode == 0) return 2*x - s;
    if (mode == 1) return x - s;
    int d = x - s;
    return (int)std::floor(d / 2.0);
}

// ---- P4 bit-to-(component,Color,upper/lower) table (SPECIFICATION.md 3,
// "Writing the table"): COLOR[n] = R_p<<10|R_q<<8|G_p<<6|G_q<<4|B_p<<2|B_q,
// p=n%4, q=n/4. Re-derived independently from the printed formula, not
// read from the data tables' own bytes.
enum class Comp { R, G, B };
struct BitTarget { Comp x; int c; bool upper; };
BitTarget bit_target(int n, int b) {
    int p = n % 4, q = n / 4;
    switch (b) {
        case 11: return {Comp::R, p, true};
        case 10: return {Comp::R, p, false};
        case 9:  return {Comp::R, q, true};
        case 8:  return {Comp::R, q, false};
        case 7:  return {Comp::G, p, true};
        case 6:  return {Comp::G, p, false};
        case 5:  return {Comp::G, q, true};
        case 4:  return {Comp::G, q, false};
        case 3:  return {Comp::B, p, true};
        case 2:  return {Comp::B, p, false};
        case 1:  return {Comp::B, q, true};
        default: return {Comp::B, q, false}; // b==0
    }
}
// The 192 (n,b) pairs in the SAME row-major order the stimulus plan uses
// (SPECIFICATION.md 4.6: n outer 00-0F, b inner 00-11, split 80/80/32
// across F2/F3/F4) -- this labels which proposition a band is FOR; the
// verdict itself is always read from the port, never assumed.
std::vector<std::pair<int,int>> perturbation_plan_all() {
    std::vector<std::pair<int,int>> v;
    for (int n = 0; n < 16; ++n) for (int b = 0; b < 12; ++b) v.push_back({n, b});
    return v;
}

// ---- a frame's graded lines, resolved once and reused by every proposition
struct GradedFrame {
    bool ok = false;
    int mode = 0;                 // 0=SuperHires,1=hires,2=lores
    FrameCop fc;
    std::vector<SourceLine> lines;              // every source line, in order
    std::vector<std::vector<nmh::Span>> out_of; // out_of[i] = output spans of lines[i]
    int s = 0;
    size_t xL = 0, xR = 0;        // check interval [xL,xR) in each line's own local x (0-based within its first output span)
    int w = 0, phi = 0;
};

int frame_mode(int frame_bplcon0) {
    if (frame_bplcon0 == 0x2040) return 0;
    if (frame_bplcon0 == 0xA000 || frame_bplcon0 == 0x1040 || frame_bplcon0 == 0x9000) return 1;
    if (frame_bplcon0 == 0x2000) return 2;
    return 0;
}

// ---- R0 applied: one video sample per visible pixel x -----------------
// x is counted in hires periods from the span's own begin; its representative
// sample is span.begin + phi + x*w (R0: "visible pixel x ... is the sample
// t_hs + phi + x*w"). Returns nullopt if that sample falls outside the span.
std::optional<size_t> pixel_sample(const nmh::Span& sp, int w, int phi, long x) {
    if (w <= 0) return std::nullopt;
    long long idx = (long long)sp.begin + phi + x * (long long)w;
    if (idx < (long long)sp.begin || (size_t)idx >= sp.end) return std::nullopt;
    return (size_t)idx;
}

// finds s by scanning candidate s in a generous range over lines[line_idxs],
// using their FIRST mapped output span only (R1: every copy must agree,
// checked separately as a precondition where it matters).
template <typename ColourT, typename SampleFn, typename EqFn>
bool try_s(const std::vector<nmh::VideoSample>& video, const std::vector<SourceLine>& lines,
           const std::vector<std::vector<nmh::Span>>& out_of, const std::vector<int>& line_idxs,
           int mode, int s, int w, int phi, long xlo, long xhi, SampleFn sample_fn, EqFn eq_fn) {
    std::map<int, ColourT> byval;
    bool any = false;
    for (int li : line_idxs) {
        if (out_of[li].empty()) continue;
        nmh::Span sp = out_of[li].back();
        for (long x = xlo; x < xhi; ++x) {
            auto oidx = pixel_sample(sp, w, phi, x);
            if (!oidx) continue;
            int i = pixel_index(mode, (int)x, s);
            int v = pixel_value(lines[li], i);
            if (v < 0) continue;
            ColourT c = sample_fn(video[*oidx]);
            auto it = byval.find(v);
            if (it == byval.end()) byval[v] = c;
            else if (!eq_fn(it->second, c)) return false;
            any = true;
        }
    }
    return any && byval.size() >= 2;
}

struct SSearch { int s = 0; int n_fit = 0; bool ok = false; };

SSearch find_s(const std::vector<nmh::VideoSample>& video, const std::vector<SourceLine>& lines,
               const std::vector<std::vector<nmh::Span>>& out_of, const std::vector<int>& line_idxs,
               int mode, int w, int phi, long xlo, long xhi) {
    SSearch r;
    // s absorbs the fetch-to-display offset (DDFSTRT starts fetching well
    // before DIWSTRT's own display start), so it is not small; search a
    // range comfortably wider than the whole check interval.
    for (int s = -900; s <= 900; ++s) {
        bool fits;
        if (mode == 0)
            fits = try_s<RGB2>(video, lines, out_of, line_idxs, mode, s, w, phi, xlo, xhi,
                                [](const nmh::VideoSample& v){ return top2(v); }, eq2);
        else
            fits = try_s<Full12>(video, lines, out_of, line_idxs, mode, s, w, phi, xlo, xhi,
                                  [](const nmh::VideoSample& v){ return full12(v); }, eqfull);
        if (fits) { r.s = s; ++r.n_fit; }
    }
    r.ok = (r.n_fit == 1);
    return r;
}

// ---- baseline builder for P4/P5.L: keyed by (v[i-1],v[i],v[i+1]) ----------
struct Triple { int a, b, c; bool operator<(const Triple& o) const {
    return std::tie(a,b,c) < std::tie(o.a,o.b,o.c); } };

struct Baseline {
    std::map<Triple, Full12> value;
    std::set<Triple> ambiguous;
};

Baseline build_baseline(const std::vector<nmh::VideoSample>& video, const std::vector<SourceLine>& lines,
                         const std::vector<std::vector<nmh::Span>>& out_of, const std::vector<int>& line_idxs,
                         int mode, int s, int w, int phi, long xlo, long xhi) {
    Baseline bl;
    for (int li : line_idxs) {
        if (out_of[li].empty()) continue;
        nmh::Span sp = out_of[li].back();
        for (long x = xlo; x < xhi; ++x) {
            auto oidx = pixel_sample(sp, w, phi, x);
            if (!oidx) continue;
            int i = pixel_index(mode, (int)x, s);
            int v = pixel_value(lines[li], i);
            // SPECIFICATION.md 7/P4 item 3 writes the baseline key literally as
            // (v[i-1], v[i], v[i+1]) -- one source-pixel step, not one
            // visible-pixel step, in every mode.
            int vm1 = pixel_value(lines[li], i - 1);
            int vp1 = pixel_value(lines[li], i + 1);
            if (v < 0) continue;
            Triple t{vm1, v, vp1};
            Full12 c = full12(video[*oidx]);
            auto it = bl.value.find(t);
            if (it == bl.value.end()) bl.value[t] = c;
            else if (!eqfull(it->second, c)) bl.ambiguous.insert(t);
        }
    }
    for (auto& t : bl.ambiguous) bl.value.erase(t);
    return bl;
}

struct FrameLines {
    FrameCop fc;
    std::vector<SourceLine> lines;
    std::vector<std::vector<nmh::Span>> out_of;
};

FrameLines decode_frame_lines(const Capture& cap, int frame_idx, uint32_t p1base, uint32_t p2base) {
    FrameLines fl;
    fl.fc = decode_frame(cap.log, frame_idx);
    if (!fl.fc.found) return fl;
    FrameCop nextfc = frame_idx < 10 ? decode_frame(cap.log, frame_idx + 1) : FrameCop{};
    uint64_t hi = (frame_idx < 10 && nextfc.found) ? nextfc.start_cycle : fl.fc.end_cycle + 3'000'000;
    fl.lines = find_source_lines(cap.log, fl.fc.start_cycle, hi, p1base, p2base);
    auto outlines = nmh::find_lines(cap.video);
    for (auto& L : fl.lines) {
        LineMap lm = map_source_to_output(outlines, L.cycle_lo, L.cycle_hi);
        std::vector<nmh::Span> spans;
        for (size_t idx : lm.span_idx) spans.push_back(outlines[idx]);
        fl.out_of.push_back(spans);
    }
    return fl;
}

// R0: measure w,phi once from a line comfortably inside the pattern section
// (not the R section) of any hires or SuperHires frame's own decoded lines.
WPhi measure_global_w_phi(const std::vector<nmh::VideoSample>& video, const FrameLines& fl) {
    for (size_t li = 10; li < fl.lines.size() && li < 40; ++li) {
        if (fl.lines[li].pure_r || fl.out_of[li].empty()) continue;
        WPhi wp = measure_w_phi(video, fl.out_of[li].back());
        if (wp.ok) return wp;
    }
    return {};
}

GradedFrame finish_graded_frame(const Capture& cap, FrameLines&& fl, int w, int phi, int top_n = 24) {
    GradedFrame gf;
    gf.fc = std::move(fl.fc);
    gf.lines = std::move(fl.lines);
    gf.out_of = std::move(fl.out_of);
    gf.w = w; gf.phi = phi;
    if (!gf.fc.found) return gf;
    auto it0 = gf.fc.header.find(0x100);
    int bplcon0 = it0 != gf.fc.header.end() ? it0->second : -1;
    gf.mode = frame_mode(bplcon0);
    // check interval (in pixel units) from a pure-R line whose run falls
    // inside C6's own stated bound (256-576 hires periods); the very first
    // source line of a frame is occasionally an edge artifact (its mapped
    // output span picks up extra content), so every pure-R line is tried
    // and the first one landing in-bound wins -- not a tolerance of our
    // own, C6's own numbers (SPECIFICATION.md 6).
    for (size_t li = 0; li < gf.lines.size(); ++li) {
        if (!gf.lines[li].pure_r || gf.out_of[li].empty()) continue;
        Run12 r = find_constant_run(cap.video, gf.out_of[li].back(), gf.mode);
        if (!r.ok) continue;
        nmh::Span sp = gf.out_of[li].back();
        long xlo = (long)((r.lo - sp.begin - phi + w - 1) / w);
        long xhi = (long)((r.hi - sp.begin - phi) / w);
        long width = xhi - xlo;
        if (width < 256 || width > 576) continue;
        gf.xL = (size_t)(xlo + 8);
        gf.xR = (size_t)(xhi - 8);
        break;
    }
    // Source line 0 (window line 0, the very first line after the Copper
    // header) is excluded here and everywhere else this package reads "the
    // top Q0 lines": measured on the port, it shows only the border colour
    // across the whole line instead of the fetched bitplane data (checked
    // directly: lines 1+ show all four of Q0's Colors exactly where
    // predicted). Harness/RTL note for the report, not a change to any
    // expected value -- lines 1-23 remain a graded, spec-sized population.
    std::vector<int> top_idx;
    for (int i = 0; i < top_n && i < (int)gf.lines.size(); ++i) top_idx.push_back(i);
    if (gf.xR > gf.xL) {
        SSearch ss = find_s(cap.video, gf.lines, gf.out_of, top_idx, gf.mode, w, phi,
                             (long)gf.xL, (long)gf.xR);
        gf.s = ss.s;
        gf.ok = ss.ok;
    }
    return gf;
}

// P1: per (Color,Component) proposition state, accumulated across every
// stimulus instance this package grades (F1's own bands; F2-F4's Q0
// sections). index = color*3+component (R=0,G=1,B=2).
struct P1State { long total[12] = {0}; long fail[12] = {0}; };
// P2 (SPECIFICATION.md P2): grouped by (component, the two encoded letters
// of X of Color c AS THE PALETTE GIVES THEM) -> every pixel in a group must
// show the identical full 4-bit component value. key = (comp, letters 0-3).
struct P2State {
    std::map<std::pair<int,int>, int> bykey[3];
    std::set<std::pair<int,int>> bad[3];
    long total[3] = {0};
    // SPECIFICATION.md 7/P2 item 4: "at least one group must contain pixels
    // from two palettes that differ in some other Color's letters of X" --
    // tracked as the set of distinct palettes that contributed to each
    // (c, letters) group.
    std::map<std::pair<int,int>, std::set<std::string>> sources[3];
};

void grade_range_p1p2(const std::vector<nmh::VideoSample>& video, const GradedFrame& gf,
                       const std::vector<uint16_t>& palette, int line_lo, int line_hi,
                       P1State* p1, P2State* p2, const std::string& band_id = "") {
    if (gf.mode != 0) return; // P1/P2 are SuperHires-only
    for (int li = line_lo; li <= line_hi && li < (int)gf.lines.size(); ++li) {
        if (gf.out_of[li].empty()) continue;
        nmh::Span sp = gf.out_of[li].back();
        for (size_t x = gf.xL; x < gf.xR; ++x) {
            auto oidx = pixel_sample(sp, gf.w, gf.phi, (long)x);
            if (!oidx) continue;
            size_t idx = *oidx;
            int i = pixel_index(0, (int)x, gf.s);
            int v = pixel_value(gf.lines[li], i);
            if (v < 0 || v > 3) continue;
            RGB2 top = top2(video[idx]);
            int shown[3] = { top.r, top.g, top.b };
            int full[3] = { video[idx].r, video[idx].g, video[idx].b };
            int shift[3] = {10, 6, 2};
            for (int comp = 0; comp < 3; ++comp) {
                int expected = (palette[v] >> shift[comp]) & 3;
                if (p1) {
                    int pidx = v*3+comp;
                    p1->total[pidx]++;
                    if (shown[comp] != expected) {
                        p1->fail[pidx]++;
                    }
                }
                if (p2) {
                    p2->total[comp]++;
                    auto k = std::make_pair(v, expected); // (c, letters)
                    auto it = p2->bykey[comp].find(k);
                    if (it == p2->bykey[comp].end()) p2->bykey[comp][k] = full[comp];
                    else if (it->second != full[comp]) {
                        p2->bad[comp].insert(k);
                    }
                    if (!band_id.empty()) p2->sources[comp][k].insert(band_id);
                }
            }
        }
    }
}

// P3 (coarseness): collects, per component, the set of distinct full 4-bit
// values shown at value-1 pixels across a set of bands.
struct P3Comp { std::set<int> vals[3]; long total = 0; };

void collect_p3(const std::vector<nmh::VideoSample>& video, const GradedFrame& gf,
                 int line_lo, int line_hi, int target_value, P3Comp& out) {
    for (int li = line_lo; li <= line_hi && li < (int)gf.lines.size(); ++li) {
        if (gf.out_of[li].empty()) continue;
        nmh::Span sp = gf.out_of[li].back();
        for (size_t x = gf.xL; x < gf.xR; ++x) {
            auto oidx = pixel_sample(sp, gf.w, gf.phi, (long)x);
            if (!oidx) continue;
            size_t idx = *oidx;
            int i = pixel_index(gf.mode, (int)x, gf.s);
            int v = pixel_value(gf.lines[li], i);
            if (v != target_value) continue;
            out.vals[0].insert(video[idx].r);
            out.vals[1].insert(video[idx].g);
            out.vals[2].insert(video[idx].b);
            out.total++;
        }
    }
}

// ---- P4: per-bit table propositions ----------------------------------------
struct P4Verdict {
    bool observed = false; bool fail = false;
    long changed = 0, checked = 0, excluded = 0;
    long any_changed = 0; // P5.L: counts ANY bit differing, not just the allowed one
};

// Grades one perturbation band (2 graded lines, both content -- these bands
// are 1-2 MOVEs, well under the 4-MOVE spacer threshold) against the
// baseline, for the (n,b) proposition this band is FOR.
P4Verdict grade_p4_band(const std::vector<nmh::VideoSample>& video, const GradedFrame& gf,
                         const Baseline& bl, int band_idx, int n, int b) {
    P4Verdict pv;
    if (band_idx < 0 || band_idx >= (int)gf.fc.bands.size()) return pv;
    const Band& band = gf.fc.bands[band_idx];
    int line_lo = band.line, line_hi = band.line + 1; // 2 content lines, not a spacer
    BitTarget t = bit_target(n, b);
    int shift = t.x == Comp::R ? 0 : t.x == Comp::G ? 1 : 2; // index into {r,g,b}
    for (int li = line_lo; li <= line_hi && li < (int)gf.lines.size(); ++li) {
        if (gf.out_of[li].empty()) continue;
        nmh::Span sp = gf.out_of[li].back();
        for (size_t x = gf.xL; x < gf.xR; ++x) {
            auto oidx = pixel_sample(sp, gf.w, gf.phi, (long)x);
            if (!oidx) continue;
            size_t idx = *oidx;
            int i = pixel_index(gf.mode, (int)x, gf.s);
            int v = pixel_value(gf.lines[li], i);
            // literal spec key: one source-pixel step, every mode.
            int vm1 = pixel_value(gf.lines[li], i - 1);
            int vp1 = pixel_value(gf.lines[li], i + 1);
            if (v < 0) continue;
            Triple tr{vm1, v, vp1};
            auto it = bl.value.find(tr);
            if (it == bl.value.end()) { pv.excluded++; continue; }
            Full12 base = it->second;
            Full12 obs = full12(video[idx]);
            pv.checked++;
            int base_c[3] = {base.r, base.g, base.b};
            int obs_c[3]  = {obs.r, obs.g, obs.b};
            bool any_diff = false;
            // "observed" (the allowed bit itself moved) and "no OTHER bit
            // moved" are separate questions -- a mutation (V4d) can make the
            // allowed bit move together with an extra one; that must show as
            // OBSERVED and FAIL through the extra bit, never as "not
            // observable" (BRIEF.md: a mutation prints FAIL through the
            // clause it targets, it does not hide the clause).
            bool allowed_only = true;
            bool saw_allowed_change = false;
            for (int comp = 0; comp < 3; ++comp) {
                if (base_c[comp] == obs_c[comp]) continue;
                any_diff = true;
                int diff = base_c[comp] ^ obs_c[comp];
                if (comp == shift && v == t.c) {
                    int own_bit = t.upper ? 0x8 : 0x4; // bit3 or bit2 of the nibble: the letter THIS proposition names
                    // SPECIFICATION.md 7/P4 item 4: the allowed places are "bit 3
                    // or 2 of X (per L) AND bits 1-0 of X" on a value-c pixel --
                    // the dashes (L2) are unencoded and may move on ANY
                    // inversion of a bit that shares X's nibble, not just this
                    // one; only OBSERVED tracks the bit this proposition itself
                    // names.
                    if (diff & own_bit) saw_allowed_change = true;
                    if ((diff & ~(own_bit | 0x3)) == 0) continue; // only allowed positions moved here
                }
                allowed_only = false;
            }
            if (!allowed_only) pv.fail = true;
            if (saw_allowed_change) { pv.observed = true; pv.changed++; }
            if (any_diff) pv.any_changed++;
        }
    }
    return pv;
}

// ---- P5.L: does any graded pixel outside the band's own register change? --
// (used for the SuperHires clause on registers 04-0F, and the Hires clause
// on 00-0F, via grade_p4_band's own byproduct: "changed" > 0 at all, for
// registers outside {0,1,2,3} in SuperHires mode signals the multiplexing.)

// ---- C9: no sprite pixels in F1-F6 (every sprite fetch reads $0000) -------
bool precondition_c9(const std::vector<nmh::RamAccess>& log, uint64_t lo, uint64_t hi) {
    uint32_t s0 = word_addr(kSpr0Struct), ns = word_addr(kNullSpr);
    bool any = false;
    for (auto& a : log) {
        if (a.is_write || a.cycle < lo || a.cycle >= hi) continue;
        bool in_spr = (a.word_addr >= s0 && a.word_addr < s0+124) || (a.word_addr >= ns && a.word_addr < ns+2);
        if (!in_spr) continue;
        any = true;
        if (a.data != 0x0000) return false;
    }
    (void)any; // C9 is "no sprite fetch shows nonzero"; the loop above already
               // returns false on the first violation, so an empty or an
               // all-zero population both satisfy it.
    return true;
}

// ---- C3 on one frame: F>=16, same count per plane, consecutive addresses,
// data equal to what the program wrote (independent xorshift regenerate).
bool precondition_c3(const GradedFrame& gf, const std::vector<uint16_t>& p1ref,
                      const std::vector<uint16_t>& p2ref, long* bad_lines) {
    long bad = 0;
    for (auto& L : gf.lines) {
        if (L.p1.size() < 16 || L.p1.size() != L.p2.size()) { ++bad; continue; }
        for (size_t k = 0; k + 1 < L.p1_off.size(); ++k)
            if (L.p1_off[k+1] != L.p1_off[k] + 1) { ++bad; break; }
        for (size_t k = 0; k < L.p1.size(); ++k) {
            uint32_t off = L.p1_off[k];
            uint16_t exp1 = off < 256 ? 0xFFFF : (off-256 < p1ref.size() ? p1ref[off-256] : 0xFFFF);
            uint32_t off2 = L.p2_off[k];
            uint16_t exp2 = off2 < 256 ? 0xFFFF : (off2-256 < p2ref.size() ? p2ref[off2-256] : 0xFFFF);
            if (L.p1[k] != exp1 || L.p2[k] != exp2) { ++bad; break; }
        }
    }
    if (bad_lines) *bad_lines = bad;
    return bad == 0 && !gf.lines.empty();
}

// ---- C8 contrast pattern (SPECIFICATION.md 6/C8): "the pattern section
// restricted to values 0 and 3" -- confirmed against bp_data_pattern1_vc8.i
// and bp_data_pattern2_vc8.i, which are word-for-word the baseline's own
// plane-2 table on BOTH planes (v = 2*b2+b1 = 2*b2+b2 = 3*b2, so v in {0,3}).
std::pair<std::vector<uint16_t>, std::vector<uint16_t>> build_pattern_vc8() {
    auto base = build_pattern();
    return {base.second, base.second};
}

// ---- C4: the header MOVEs carry the SPECIFICATION.md 4.3 data,
// not just the planned count. q0 gives COLOR00-0F; COLOR10-1F (the sprite
// palette T0) carries the identical sixteen values (SPECIFICATION.md 3: T0
// and Q0 are numerically the same table).
bool check_frame_header(const FrameCop& fc, uint16_t expected_bplcon0, const std::vector<uint16_t>& q0) {
    if (!fc.found) return false;
    auto get = [&](uint16_t reg) -> long {
        auto it = fc.header.find(reg);
        return it != fc.header.end() ? (long)it->second : -1;
    };
    if (get(0x100) != expected_bplcon0) return false;
    if (get(0x102) != 0x0000) return false;
    if (get(0x104) != 0x0024) return false;
    if (get(0x08E) != 0x2C81) return false;
    if (get(0x090) != 0xF4A1) return false;
    if (get(0x092) != 0x003C) return false;
    if (get(0x094) != 0x00D4) return false;
    if (get(0x108) != 0x0000) return false;
    if (get(0x10A) != 0x0000) return false;
    if (get(0x0E0) != (long)(kPlane1Buf >> 16)) return false;
    if (get(0x0E2) != (long)(kPlane1Buf & 0xFFFF)) return false;
    if (get(0x0E4) != (long)(kPlane2Buf >> 16)) return false;
    if (get(0x0E6) != (long)(kPlane2Buf & 0xFFFF)) return false;
    if (q0.size() != 16) return false;
    for (int n = 0; n < 16; ++n) if (get(color_reg(n)) != q0[n]) return false;
    for (int n = 16; n < 32; ++n) if (get(color_reg(n)) != q0[n-16]) return false; // COLOR10-1F = T0 = Q0's values
    return true;
}

// ---- R2: "in F1-F6 the same s fits the bottom Q0 lines" --
// re-runs find_s on the tail population (after the frame's own closing
// band) and checks it agrees with the s already found from the top lines.
bool check_bottom_s(const std::vector<nmh::VideoSample>& video, const GradedFrame& gf, int close_band_idx) {
    if (!gf.ok || close_band_idx < 0 || close_band_idx >= (int)gf.fc.bands.size()) return false;
    std::vector<int> idxs;
    int lo = gf.fc.bands[close_band_idx].line + 1;
    for (int i = lo; i <= 199 && i < (int)gf.lines.size(); ++i) idxs.push_back(i);
    if (idxs.empty() || gf.xR <= gf.xL) return false;
    SSearch ss = find_s(video, gf.lines, gf.out_of, idxs, gf.mode, gf.w, gf.phi, (long)gf.xL, (long)gf.xR);
    return ss.ok && ss.s == gf.s;
}

// ---- C6: "identical ends on all pure R lines", asserted, not
// silently filtered to the first line that happens to fit.
struct C6Result { bool ok = false; long xL = 0, xR = 0; int lines_checked = 0, lines_mismatch = 0; };
C6Result check_c6(const std::vector<nmh::VideoSample>& video, const GradedFrame& gf) {
    C6Result r;
    bool have = false;
    for (size_t li = 0; li < gf.lines.size(); ++li) {
        if (!gf.lines[li].pure_r || li >= gf.out_of.size() || gf.out_of[li].empty()) continue;
        Run12 run = find_constant_run(video, gf.out_of[li].back(), gf.mode);
        if (!run.ok) continue;
        nmh::Span sp = gf.out_of[li].back();
        long xlo = (long)((run.lo - sp.begin - gf.phi + gf.w - 1) / gf.w);
        long xhi = (long)((run.hi - sp.begin - gf.phi) / gf.w);
        long width = xhi - xlo;
        if (width < 256 || width > 576) continue; // SPECIFICATION.md 6/C6's own bound
        long xL = xlo + 8, xR = xhi - 8;
        ++r.lines_checked;
        if (!have) { r.xL = xL; r.xR = xR; have = true; }
        else if (xL != r.xL || xR != r.xR) ++r.lines_mismatch;
    }
    r.ok = have && r.lines_checked > 0 && r.lines_mismatch == 0 && r.xR > r.xL;
    return r;
}

// ---- C7: SPECIFICATION.md 6/C7's own timing condition -- every
// band's WAIT is at horizontal $00, fetched after the previous line's last
// bitplane fetch and, spacer lines excepted, before its own line's first.
// A spacer is a band of more than four MOVEs (4.4: "A band change of at
// most four MOVEs is planned to end before the line's first bitplane
// fetch... A band change that writes a whole palette (sixteen MOVEs)... is
// a spacer line").
struct C7Result { bool ok = false; int bands_checked = 0, bad_h = 0, bad_after = 0, bad_before = 0; };
C7Result check_c7_timing(const GradedFrame& gf) {
    C7Result r;
    for (const Band& band : gf.fc.bands) {
        ++r.bands_checked;
        if ((band.wait_w1 & 0xFF) != 0x01) ++r.bad_h;
        bool after_prev = true;
        if (band.line > 0 && band.line - 1 < (int)gf.lines.size() && !band.moves.empty())
            after_prev = gf.lines[band.line - 1].cycle_hi < band.first_move_cycle;
        if (!after_prev) ++r.bad_after;
        bool is_spacer = band.moves.size() > 4;
        bool before_own = true;
        if (!is_spacer && band.line >= 0 && band.line < (int)gf.lines.size())
            before_own = band.last_event_cycle < gf.lines[band.line].cycle_lo;
        if (!before_own) ++r.bad_before;
    }
    r.ok = r.bands_checked > 0 && r.bad_h == 0 && r.bad_after == 0 && r.bad_before == 0;
    return r;
}

// ---- C8: SPECIFICATION.md 6/C8's own population -- every one of
// the 16 ordered (v,v') pairs occurs, as (v[i],v[i+1]) and as (v[i-1],v[i]),
// over a band's own graded lines. i ranges a little past the check
// interval's own mapped span on each side, so the pair straddling either
// edge is covered too.
bool band_pair_coverage(const GradedFrame& gf, int line_lo, int line_hi) {
    if (gf.w <= 0 || gf.xR <= gf.xL) return false;
    long ilo = pixel_index(gf.mode, (int)gf.xL, gf.s) - 2;
    long ihi = pixel_index(gf.mode, (int)gf.xR, gf.s) + 2;
    std::set<std::pair<int,int>> fwd, bwd;
    for (int li = line_lo; li <= line_hi && li < (int)gf.lines.size(); ++li) {
        for (long i = ilo; i < ihi; ++i) {
            int v0 = pixel_value(gf.lines[li], (int)i), v1 = pixel_value(gf.lines[li], (int)i + 1);
            if (v0 >= 0 && v1 >= 0) fwd.insert({v0, v1});
        }
        for (long i = ilo + 1; i <= ihi; ++i) {
            int vm1 = pixel_value(gf.lines[li], (int)i - 1), v = pixel_value(gf.lines[li], (int)i);
            if (vm1 >= 0 && v >= 0) bwd.insert({vm1, v});
        }
    }
    return fwd.size() == 16 && bwd.size() == 16;
}

// ---- R1: "every copy is graded and the copies must agree"
// (SPECIFICATION.md 4.7) -- compares the FIRST and LAST output span of every
// source line that maps to two or more, at every graded x.
struct R1Result { bool ok = false; long pairs = 0, mismatches = 0; };
R1Result check_r1_copies(const std::vector<nmh::VideoSample>& video, const GradedFrame& gf) {
    R1Result r;
    if (gf.w <= 0 || gf.xR <= gf.xL) return r;
    // A source line's own two doubled copies are NOT out_of[li].front() and
    // out_of[li].back(): the fetch burst's cycle range overlaps the tail of
    // the PREVIOUS line's second display copy and the head of this line's
    // own first (SPECIFICATION.md 10.2's own measurement -- "the package's
    // 'last' span of line k is the FIRST of the two output lines that show
    // k, and its 'first' span of line k is the SECOND copy of line k-1").
    // So line li's own pair is out_of[li].back() (its first copy) and
    // out_of[li+1].front() (its second copy, found under li+1's own
    // mapping).
    for (size_t li = 0; li + 1 < gf.out_of.size(); ++li) {
        if (gf.out_of[li].empty() || gf.out_of[li+1].empty()) continue;
        nmh::Span a = gf.out_of[li].back();
        nmh::Span b = gf.out_of[li+1].front();
        for (size_t x = gf.xL; x < gf.xR; ++x) {
            auto ia = pixel_sample(a, gf.w, gf.phi, (long)x);
            auto ib = pixel_sample(b, gf.w, gf.phi, (long)x);
            if (!ia || !ib) continue;
            ++r.pairs;
            if (!eqfull(full12(video[*ia]), full12(video[*ib]))) ++r.mismatches;
        }
    }
    r.ok = r.pairs > 0 && r.mismatches == 0;
    return r;
}

// ============================================================================
// F7-F10: the sprite colour frames (SPECIFICATION.md 4.6, P5.U, P6.1, P6.2,
// C10, C11)
// ============================================================================

// Source lines of a frame whose bitplane pointers move from the stream buffers
// to the zero buffer partway down the frame (SPECIFICATION.md 4.6, F7-F10).
// Plane-1 and plane-2 reads of the zero buffer address the same words, so a
// line of the zero buffer is kept as a burst of zero words, not as two planes.
std::vector<SourceLine> find_source_lines_z(const std::vector<nmh::RamAccess>& log,
                                             uint64_t c_lo, uint64_t c_hi) {
    uint32_t b1 = word_addr(kPlane1Buf), b2 = word_addr(kPlane2Buf), bz = word_addr(kZeroBuf);
    struct Raw { uint64_t cycle; uint32_t wa; uint16_t data; };
    std::vector<Raw> all_reads;
    std::vector<int> kind; // 0 stream, 1 zero buffer
    for (auto& a : log) {
        if (a.is_write || a.cycle < c_lo || a.cycle >= c_hi) continue;
        bool in1 = a.word_addr >= b1 && a.word_addr < b1 + kBufWords;
        bool in2 = a.word_addr >= b2 && a.word_addr < b2 + kBufWords;
        bool inz = a.word_addr >= bz && a.word_addr < bz + kBufWords;
        if (!(in1 || in2 || inz)) continue;
        all_reads.push_back({a.cycle, a.word_addr, a.data});
        kind.push_back(inz ? 1 : 0);
    }
    std::vector<SourceLine> lines;
    constexpr uint64_t kGap = 300;
    std::vector<long> zero_reads; std::vector<bool> zero_all;
    nmh::RamAccess prev{}; bool have_prev = false;
    for (size_t i = 0; i < all_reads.size(); ++i) {
        const Raw& r = all_reads[i];
        if (lines.empty() || r.cycle - lines.back().cycle_hi > kGap) {
            lines.push_back(SourceLine{r.cycle, r.cycle, {}, {}, {}, {}, false, false});
            zero_reads.push_back(0); zero_all.push_back(true);
            have_prev = false;
        }
        auto& L = lines.back();
        L.cycle_hi = r.cycle;
        if (kind[i] == 1) {
            zero_reads.back()++;
            if (r.data != 0) zero_all.back() = false;
            continue;
        }
        // stream read: same coalescing rule as find_source_lines
        if (have_prev && prev.word_addr == r.wa && prev.data == r.data) continue;
        prev = nmh::RamAccess{r.cycle, r.wa, r.data, false, false, false};
        have_prev = true;
        if (r.wa >= b1 && r.wa < b1 + kBufWords) { L.p1.push_back(r.data); L.p1_off.push_back(r.wa - b1); }
        else                                      { L.p2.push_back(r.data); L.p2_off.push_back(r.wa - b2); }
    }
    for (size_t k = 0; k < lines.size(); ++k) {
        auto& L = lines[k];
        bool stream_empty = L.p1.empty() && L.p2.empty();
        if (stream_empty && zero_reads[k] > 0 && zero_all[k]) {
            L.zero = true;
            L.p1.assign((size_t)zero_reads[k] / 2, 0);
            L.p2.assign((size_t)zero_reads[k] / 2, 0);
            continue;
        }
        bool pure = !L.p1.empty() && zero_reads[k] == 0;
        for (auto d : L.p1) if (d != 0xFFFF) pure = false;
        for (auto d : L.p2) if (d != 0xFFFF) pure = false;
        L.pure_r = pure;
    }
    return lines;
}

FrameLines decode_frame_lines_z(const Capture& cap, int frame_idx, const std::vector<nmh::Span>& outlines) {
    FrameLines fl;
    fl.fc = decode_frame(cap.log, frame_idx);
    if (!fl.fc.found) return fl;
    FrameCop nextfc = frame_idx < 10 ? decode_frame(cap.log, frame_idx + 1) : FrameCop{};
    uint64_t hi = (frame_idx < 10 && nextfc.found) ? nextfc.start_cycle : fl.fc.end_cycle + 3'000'000;
    fl.lines = find_source_lines_z(cap.log, fl.fc.start_cycle, hi);
    if (fl.lines.size() > 200) fl.lines.resize(200);   // window lines 0-199 of this frame
    for (auto& L : fl.lines) {
        LineMap lm = map_source_to_output(outlines, L.cycle_lo, L.cycle_hi);
        std::vector<nmh::Span> spans;
        for (size_t idx : lm.span_idx) spans.push_back(outlines[idx]);
        fl.out_of.push_back(spans);
    }
    return fl;
}

constexpr int kSprPairs = 60;

// What the port shows of sprite 0's and the other sprites' DMA in one frame.
struct SprFetch {
    bool ctl_seen[2] = {false, false};
    uint16_t ctl[2] = {0, 0};
    std::array<int, 120> half;                 // data word of each pair half, -1 if never read
    std::array<int, kSprPairs> line;           // source line the pair's first word was fetched on, -1 if none
    long pair_reads = 0;                       // reads of the 120 data words
    long null_reads = 0;                       // reads of the null sprite
    bool null_zero = true;                     // every null-sprite read returned $0000
    SprFetch() { half.fill(-1); line.fill(-1); }
    int value(int j) const {                   // pair j's colour value: low word = bit 0, high word = bit 1
        if (half[2*j] < 0 || half[2*j+1] < 0) return -1;
        return (half[2*j] != 0 ? 1 : 0) + (half[2*j+1] != 0 ? 2 : 0);
    }
    int pairs_seen() const { int n = 0; for (int j = 0; j < kSprPairs; ++j) if (value(j) >= 0) ++n; return n; }
};

SprFetch decode_sprite_fetch(const Capture& cap, const FrameLines& fl) {
    SprFetch sf;
    if (!fl.fc.found || fl.lines.empty()) return sf;
    uint64_t lo = fl.fc.start_cycle, hi = fl.lines.back().cycle_hi;
    uint32_t s0 = word_addr(kSpr0Struct), ns = word_addr(kNullSpr);
    for (auto& a : cap.log) {
        if (a.is_write || a.cycle < lo || a.cycle > hi) continue;
        if (a.word_addr >= s0 && a.word_addr < s0 + 124) {
            int off = (int)(a.word_addr - s0);
            if (off < 2) { if (!sf.ctl_seen[off]) { sf.ctl_seen[off] = true; sf.ctl[off] = a.data; } }
            else if (off < 122) {
                int h = off - 2;
                ++sf.pair_reads;
                if (sf.half[h] < 0) sf.half[h] = a.data;
                if ((h & 1) == 0 && sf.line[h/2] < 0) {
                    // the line whose own bitplane burst follows this fetch
                    size_t k = 0;
                    while (k < fl.lines.size() && fl.lines[k].cycle_lo <= a.cycle) ++k;
                    sf.line[h/2] = k < fl.lines.size() ? (int)k : -1;
                }
            }
        } else if (a.word_addr >= ns && a.word_addr < ns + 2) {
            ++sf.null_reads;
            if (a.data != 0) sf.null_zero = false;
        }
    }
    return sf;
}

// A graded frame's pixels over the check interval [xL,xR) of one source line.
struct Row { bool ok = false; std::vector<uint16_t> px; };

uint16_t pack12(const nmh::VideoSample& s) { return (uint16_t)((s.r << 8) | (s.g << 4) | s.b); }

Row get_row(const std::vector<nmh::VideoSample>& video, const GradedFrame& gf, int li) {
    Row r;
    if (li < 0 || li >= (int)gf.lines.size() || li >= (int)gf.out_of.size() || gf.out_of[li].empty() ||
        gf.xR <= gf.xL || gf.w <= 0) return r;
    nmh::Span sp = gf.out_of[li].back();
    r.px.resize(gf.xR - gf.xL);
    for (size_t x = gf.xL; x < gf.xR; ++x) {
        auto o = pixel_sample(sp, gf.w, gf.phi, (long)x);
        if (!o) return Row{};
        r.px[x - gf.xL] = pack12(video[*o]);
    }
    r.ok = true;
    return r;
}

// SPECIFICATION.md P6 item 3: the run of a sprite line is the maximal set of
// consecutive visible pixels whose twelve bits differ from those at the same x
// on window lines 31-35 of the same frame.
struct RunInfo { bool shows = false; long xa = 0, xb = 0; long len() const { return xb - xa + 1; } };

struct SprFrame {
    int f = 0;
    bool found = false;
    GradedFrame g;
    SprFetch sf;
    std::vector<char> spacer;                  // per source line: its band change is a spacer line
    std::vector<Row> row;                      // per sprite line j
    std::vector<RunInfo> run;                  // per sprite line j
    std::vector<uint16_t> bg;                  // window lines 31-35, per x - xL
    std::vector<char> bg_ok;
    bool bg_have = false;
    int lines_ok() const { return (int)g.lines.size(); }
    int src_line(int j) const { return sf.line[j]; }
    bool line_graded(int j) const {
        if (!found || j < 0 || j >= (int)row.size()) return false;
        int li = sf.line[j];
        return li >= 0 && li < (int)spacer.size() && !spacer[li] && row[j].ok;
    }
};

SprFrame build_sprite_frame(const Capture& cap, FrameLines&& fl, int f, int w, int phi) {
    SprFrame s;
    s.f = f;
    s.sf = decode_sprite_fetch(cap, fl);
    s.g = finish_graded_frame(cap, std::move(fl), w, phi, 30);
    s.found = s.g.fc.found && s.g.lines.size() == 200;
    if (!s.found) return s;
    s.spacer.assign(s.g.lines.size(), 0);
    for (const Band& b : s.g.fc.bands)
        if (b.moves.size() > 4 && b.line >= 0 && b.line < (int)s.spacer.size()) s.spacer[b.line] = 1;
    s.row.assign(kSprPairs, Row{});
    s.run.assign(kSprPairs, RunInfo{});
    // background rows
    std::vector<Row> bgr;
    for (int li = 31; li <= 35; ++li) bgr.push_back(get_row(cap.video, s.g, li));
    bool all_ok = s.g.xR > s.g.xL;
    for (auto& r : bgr) all_ok = all_ok && r.ok;
    if (all_ok) {
        size_t n = s.g.xR - s.g.xL;
        s.bg.assign(n, 0); s.bg_ok.assign(n, 0);
        for (size_t i = 0; i < n; ++i) {
            bool same = true;
            for (auto& r : bgr) same = same && r.px[i] == bgr[0].px[i];
            s.bg[i] = bgr[0].px[i]; s.bg_ok[i] = same ? 1 : 0;
        }
        s.bg_have = true;
    }
    for (int j = 0; j < kSprPairs; ++j) {
        int li = s.sf.line[j];
        if (li < 0) continue;
        s.row[j] = get_row(cap.video, s.g, li);
        if (!s.row[j].ok || !s.bg_have) continue;
        // longest run of consecutive differing pixels
        long best_lo = -1, best_len = 0, cur_lo = -1, cur_len = 0;
        size_t n = s.row[j].px.size();
        for (size_t i = 0; i < n; ++i) {
            bool d = s.bg_ok[i] && s.row[j].px[i] != s.bg[i];
            if (d) { if (cur_len == 0) cur_lo = (long)i; ++cur_len; if (cur_len > best_len) { best_len = cur_len; best_lo = cur_lo; } }
            else cur_len = 0;
        }
        if (best_len > 0) {
            s.run[j].shows = true;
            s.run[j].xa = (long)s.g.xL + best_lo;
            s.run[j].xb = (long)s.g.xL + best_lo + best_len - 1;
        }
    }
    return s;
}

// Which of a frame's sprite blocks are baseline blocks (SPECIFICATION.md 4.6).
bool is_baseline_block(int f, int blk) {
    if (f == 7) return blk <= 1 || blk >= 15;
    if (f == 10) return blk <= 1 || blk >= 18;
    return true;
}

// Twelve-bit values shown at (sprite value v, position) on a frame's baseline sprite lines.
struct BaseSets { std::array<std::vector<std::set<uint16_t>>, 4> s; };

BaseSets build_base_sets(const SprFrame& sfr) {
    BaseSets b;
    size_t n = sfr.g.xR > sfr.g.xL ? sfr.g.xR - sfr.g.xL : 0;
    for (auto& v : b.s) v.assign(n, std::set<uint16_t>{});
    for (int j = 0; j < kSprPairs; ++j) {
        if (!is_baseline_block(sfr.f, j / 3) || !sfr.line_graded(j)) continue;
        int v = sfr.sf.value(j);
        if (v < 1 || v > 3) continue;
        for (size_t i = 0; i < n; ++i) b.s[v][i].insert(sfr.row[j].px[i]);
    }
    return b;
}

struct RunSummary {
    int lines = 0, with_run = 0, same_ends = 0;
    long xa = 0, xb = 0;
    long len_min = LONG_MAX, len_max = 0;
    bool consistent() const { return with_run > 0 && same_ends == with_run; }
};

RunSummary summarise_runs(const SprFrame& sfr, bool baseline_only) {
    RunSummary r;
    bool first = true;
    for (int j = 0; j < kSprPairs; ++j) {
        if (baseline_only && !is_baseline_block(sfr.f, j / 3)) continue;
        if (!sfr.line_graded(j)) continue;
        ++r.lines;
        if (!sfr.run[j].shows) continue;
        ++r.with_run;
        if (first) { r.xa = sfr.run[j].xa; r.xb = sfr.run[j].xb; first = false; }
        if (sfr.run[j].xa == r.xa && sfr.run[j].xb == r.xb) ++r.same_ends;
        r.len_min = std::min(r.len_min, sfr.run[j].len());
        r.len_max = std::max(r.len_max, sfr.run[j].len());
    }
    return r;
}

// C4 for F7-F10: the header MOVEs that SPECIFICATION.md 4.3 lists (BPLCON0-2,
// DIW, DDF, modulos, bitplane pointers) carry the planned data. The colour
// registers and the sprite pointers are the stimuli under test and are not read here.
bool check_sprite_header(const FrameCop& fc, uint16_t expected_bplcon0) {
    if (!fc.found || fc.header.size() != 61) return false;
    auto get = [&](uint16_t reg) -> long {
        auto it = fc.header.find(reg);
        return it != fc.header.end() ? (long)it->second : -1;
    };
    return get(0x100) == expected_bplcon0 && get(0x102) == 0x0000 && get(0x104) == 0x0024 &&
           get(0x08E) == 0x2C81 && get(0x090) == 0xF4A1 && get(0x092) == 0x003C && get(0x094) == 0x00D4 &&
           get(0x108) == 0x0000 && get(0x10A) == 0x0000 &&
           get(0x0E0) == (long)(kPlane1Buf >> 16) && get(0x0E2) == (long)(kPlane1Buf & 0xFFFF) &&
           get(0x0E4) == (long)(kPlane2Buf >> 16) && get(0x0E6) == (long)(kPlane2Buf & 0xFFFF);
}

// Grades F7-F10 (SPECIFICATION.md P5.U, P6.1, P6.2, P7.2, P7.3 and their
// preconditions C4, C5, C7, C10, C11). `p5l_state`: 0 not graded, 1 PASS, 2 FAIL.
void grade_sprite_frames(const Capture& cap, Checks& checks, const WPhi& wp, const GradedFrame& f6, int p5l_state) {
    auto outlines = nmh::find_lines(cap.video);
    SprFrame fr[4];
    for (int i = 0; i < 4; ++i) {
        FrameLines fl = decode_frame_lines_z(cap, 7 + i, outlines);
        fr[i] = build_sprite_frame(cap, std::move(fl), 7 + i, wp.w, wp.phi);
    }
    SprFrame& F7 = fr[0]; SprFrame& F8 = fr[1]; SprFrame& F9 = fr[2]; SprFrame& F10 = fr[3];
    static const char* kName[4] = {"F7", "F8", "F9", "F10"};
    static const uint16_t kBplcon0[4] = {0x2040, 0x2040, 0x2040, 0xA000};
    static const int kBands[4] = {15, 1, 1, 18};
    static const int kMode[4] = {0, 0, 0, 1};

    // ---- C4: F7-F10 ran their own lists in order, each with the planned header
    bool c4 = f6.fc.found;
    for (int i = 0; i < 4; ++i) {
        c4 = c4 && fr[i].found && check_sprite_header(fr[i].g.fc, kBplcon0[i]);
        uint64_t prev = i == 0 ? f6.fc.start_cycle : fr[i-1].g.fc.start_cycle;
        c4 = c4 && fr[i].g.fc.start_cycle > prev;
    }
    checks.expect(c4, "C4-F7F10 precondition: F7-F10 ran their own lists in order after F6, each with the planned "
                      "BPLCON0-2, DIW, DDF, modulo and bitplane-pointer header values");

    // ---- C5: the pixel mapping of lines 0-29 fits uniquely, in the mode the frame runs in
    bool c5 = true; std::string c5s;
    for (int i = 0; i < 4; ++i) {
        bool ok = fr[i].found && fr[i].g.ok && fr[i].g.mode == kMode[i] &&
                  fr[i].g.xL == fr[0].g.xL && fr[i].g.xR == fr[0].g.xR;
        c5 = c5 && ok;
        c5s += std::string(" ") + kName[i] + (ok ? "=ok" : "=no") + ":s=" + std::to_string(fr[i].g.s);
    }
    checks.expect(c5, "C5-F7F10 precondition: on lines 0-29 of F7-F9 the SuperHires pixel mapping (i=2x-s) and on F10 the hires "
                      "mapping (i=x-s) fits uniquely, with one check interval for the four frames;" + c5s);

    // ---- C7: every planned band change found once, every WAIT at h=$00, after the previous line's
    // last bitplane fetch and, spacer lines excepted, before its own line's first
    bool c7 = true; int bh = 0, ba = 0, bb = 0; std::string c7s;
    for (int i = 0; i < 4; ++i) {
        bool count_ok = fr[i].found && (int)fr[i].g.fc.bands.size() == kBands[i];
        C7Result t = fr[i].found ? check_c7_timing(fr[i].g) : C7Result{};
        c7 = c7 && count_ok && t.ok;
        bh += t.bad_h; ba += t.bad_after; bb += t.bad_before;
        c7s += std::string(" ") + kName[i] + ":bands=" + std::to_string(fr[i].found ? (int)fr[i].g.fc.bands.size() : -1);
    }
    checks.expect(c7, "C7-F7F10 precondition (SPECIFICATION.md 6/C7, block changes included): every planned band change of F7-F10 "
                      "was found once, every WAIT is at h=$00, after the previous line's last bitplane fetch and, spacer lines "
                      "excepted, before its own line's first, bad_h=" + std::to_string(bh) + " bad_after=" + std::to_string(ba) +
                      " bad_before=" + std::to_string(bb) + ";" + c7s);

    // ---- C10: sprite 0 fetches its two control words and 60 data pairs, sprites 1-7 only $0000 control words
    bool c10 = true; std::string c10s;
    for (int i = 0; i < 4; ++i) {
        const SprFetch& sf = fr[i].sf;
        bool ok = fr[i].found && sf.ctl_seen[0] && sf.ctl_seen[1] && sf.ctl[0] == 0x5060 && sf.ctl[1] == 0x8C00 &&
                  sf.pairs_seen() == kSprPairs && sf.pair_reads == 2 * kSprPairs &&
                  sf.null_reads > 0 && sf.null_zero;
        for (int j = 0; ok && j < kSprPairs; ++j) {
            if (sf.value(j) != (j % 3) + 1) ok = false;                   // line j with j mod 3 = 0 has value 1, 1 -> 2, 2 -> 3
            if (j > 0 && sf.line[j] != sf.line[j-1] + 1) ok = false;      // one pair per consecutive line
            if (sf.line[j] < 0) ok = false;
        }
        c10 = c10 && ok;
        c10s += std::string(" ") + kName[i] + ":pairs=" + std::to_string(sf.pairs_seen()) + ",reads=" + std::to_string(sf.pair_reads) +
                ",first_line=" + std::to_string(sf.line[0]) + ",null_reads=" + std::to_string(sf.null_reads);
    }
    checks.expect(c10, "C10 precondition p.105-108: in each of F7-F10 the port shows sprite 0 fetching its two control words and "
                       "60 data pairs, one pair on each of 60 consecutive lines with the planned values, and sprites 1-7 fetching only "
                       "$0000 control words;" + c10s);

    // ---- runs
    RunSummary r7 = summarise_runs(F7, true), r8 = summarise_runs(F8, true), r10 = summarise_runs(F10, true);
    RunSummary r9 = summarise_runs(F9, true);
    checks.note("F7 baseline sprite lines: " + std::to_string(r7.lines) + " graded, " + std::to_string(r7.with_run) +
                " show a run, ends [" + std::to_string(r7.xa) + "," + std::to_string(r7.xb) + "], length " +
                std::to_string(r7.with_run ? r7.len_min : 0) + ".." + std::to_string(r7.len_max) + " hires periods");
    checks.note("F8 sprite lines: " + std::to_string(r8.lines) + " graded, " + std::to_string(r8.with_run) +
                " show a run, ends [" + std::to_string(r8.xa) + "," + std::to_string(r8.xb) + "], length " +
                std::to_string(r8.with_run ? r8.len_min : 0) + ".." + std::to_string(r8.len_max));
    checks.note("F9 sprite lines: " + std::to_string(r9.lines) + " graded, " + std::to_string(r9.with_run) +
                " show a run, ends [" + std::to_string(r9.xa) + "," + std::to_string(r9.xb) + "], length " +
                std::to_string(r9.with_run ? r9.len_min : 0) + ".." + std::to_string(r9.len_max));
    checks.note("F10 baseline sprite lines: " + std::to_string(r10.lines) + " graded, " + std::to_string(r10.with_run) +
                " show a run, ends [" + std::to_string(r10.xa) + "," + std::to_string(r10.xb) + "], length " +
                std::to_string(r10.with_run ? r10.len_min : 0) + ".." + std::to_string(r10.len_max));

    bool have7 = r7.with_run > 0, have10 = r10.with_run > 0;

    // ---- C11: the run of every baseline line that shows one is 32 +- 1 hires periods
    // (p.101: a sprite is 16 pixels wide in every playfield mode; a lores pixel is two hires periods)
    bool c11 = have7;
    for (const RunSummary* r : {&r7, &r8, &r10})
        if (r->with_run > 0 && (r->len_min < 31 || r->len_max > 33)) c11 = false;
    if (have7) {
        checks.expect(c11, "C11 precondition p.101: the run of every baseline sprite line that shows one is 32 +- 1 hires periods long in F7, F8 and F10, "
                           "lengths F7 " + std::to_string(r7.len_min) + ".." + std::to_string(r7.len_max) + ", F8 " +
                           std::to_string(r8.with_run ? r8.len_min : 0) + ".." + std::to_string(r8.len_max) + ", F10 " +
                           std::to_string(r10.with_run ? r10.len_min : 0) + ".." + std::to_string(r10.len_max));
    } else {
        checks.record("C11 precondition p.101: NOT GRADED, F7 shows no run on any baseline sprite line");
    }

    // ---- the run's ends are the same on every baseline line that shows one; F8's are F7's
    bool ends = have7 && r7.consistent() && (r8.with_run == 0 || (r8.consistent() && r8.xa == r7.xa && r8.xb == r7.xb)) &&
                (!have10 || r10.consistent());
    if (have7) {
        checks.expect(ends, "RUN-ENDS precondition (SPECIFICATION.md 7/P6 item 3): the run has the same ends on every baseline sprite line that shows one, "
                            "F7 [" + std::to_string(r7.xa) + "," + std::to_string(r7.xb) + "] on " + std::to_string(r7.same_ends) + " of " +
                            std::to_string(r7.with_run) + " lines, F8 on " + std::to_string(r8.same_ends) + " of " + std::to_string(r8.with_run) +
                            " lines with its own ends and the ends of F7 " + std::string(r8.with_run == 0 || (r8.xa == r7.xa && r8.xb == r7.xb) ? "equal" : "different") +
                            ", F10 [" + std::to_string(r10.xa) + "," + std::to_string(r10.xb) + "] on " +
                            std::to_string(r10.same_ends) + " of " + std::to_string(r10.with_run) + " lines");
    } else {
        checks.record("RUN-ENDS precondition (SPECIFICATION.md 7/P6 item 3): NOT GRADED, F7 shows no run on any baseline sprite line");
    }

    // ---- R1 for F7-F10: every output copy of a graded source line agrees with the others
    long r1_pairs = 0, r1_diff = 0;
    for (int i = 0; i < 4; ++i) if (fr[i].found) {
        R1Result r = check_r1_copies(cap.video, fr[i].g);
        r1_pairs += r.pairs; r1_diff += r.mismatches;
    }
    checks.expect(r1_pairs > 0 && r1_diff == 0,
                  "R1-F7F10 precondition (SPECIFICATION.md 4.7): every output copy of a graded source line of F7-F10 agrees with the others, " +
                  std::to_string(r1_pairs) + " pairs checked, " + std::to_string(r1_diff) + " differing");

    bool bg_all = F7.bg_have && F8.bg_have && F9.bg_have && F10.bg_have;
    bool pre = c4 && c5 && c7 && c10 && bg_all;
    bool run_pre = pre && c11 && ends;          // the run anchors the inner pixels
    // C11 is the only precondition that did not hold: the comparators below still run and are printed as MEASURED
    bool c11_only = pre && have7 && !c11 && ends;

    // A comparator's result: `ok` is its verdict, `label` its graded line.
    struct Eval { bool ran = false; bool ok = false; std::string label; };
    auto emit = [&](const Eval& e, bool graded, const std::string& ng_label) {
        size_t cut = e.label.find(": ");
        std::string head = cut == std::string::npos ? e.label : e.label.substr(0, cut);
        std::string text = cut == std::string::npos ? std::string() : e.label.substr(cut + 2);
        std::string id = head.substr(0, head.find(' '));
        if (graded) checks.expect(e.ok, e.label);
        else if (e.ran && c11_only) {
            checks.record(head + ": NOT GRADED, C11 did not hold");
            checks.note(id + " comparator without the C11 gate (not graded) gives " + std::string(e.ok ? "PASS" : "FAIL") + ": " + text);
        } else checks.record(ng_label);
    };

    // ---- F9's lower registers changed: the playfield outside F7's run differs at every compared position
    bool f9pf_ok = false; long f9pf_n = 0, f9pf_same = 0;
    if (pre && have7) {
        for (long x = (long)F7.g.xL; x < (long)F7.g.xR; ++x) {
            if (x >= r7.xa && x <= r7.xb) continue;
            size_t i = (size_t)(x - (long)F7.g.xL);
            if (!F7.bg_ok[i] || !F9.bg_ok[i]) continue;
            ++f9pf_n;
            if (F9.bg[i] == F7.bg[i]) ++f9pf_same;
        }
        f9pf_ok = f9pf_n > 0 && f9pf_same == 0;
        checks.expect(f9pf_ok, "P6.1-F9PF precondition (SPECIFICATION.md 7/P6 item 4): COLOR00-0F changed from Q0 to Q1 between F7 and F9, so "
                               "the playfield outside the run differs at every compared position, positions " + std::to_string(f9pf_n) +
                               ", equal " + std::to_string(f9pf_same));
    } else {
        checks.record("P6.1-F9PF precondition: NOT GRADED, a precondition of the F7-F10 frames did not hold");
    }

    auto letters = [](int v, int comp) { return comp == 0 ? (v & 3) : comp == 1 ? ((v + 1) & 3) : ((v + 2) & 3); };
    auto comp_bits = [](uint16_t p, int c) { return (c == 0 ? (p >> 10) : c == 1 ? (p >> 6) : (p >> 2)) & 3; };

    // ---- P6.1 (APPC-shr.c9, p.302)
    if (pre && !have7) {
        // SPECIFICATION.md 7/P6 item 3: no baseline line shows a run although C10 holds
        checks.expect(have7, "P6.1 APPC-shr.c9 p.302: no baseline sprite line of F7 shows a run although C10 holds, so no inner pixel shows the letters of Color 16+v");
    } else {
        Eval e;
        if ((run_pre || c11_only) && f9pf_ok) {
            e.ran = true;
            long xa = r7.xa, xb = r7.xb;
            long n7 = 0, bad7 = 0, norun7 = 0, n8 = 0, bad8 = 0, n9 = 0, bad9 = 0;
            BaseSets b7 = build_base_sets(F7);
            for (int j = 0; j < kSprPairs; ++j) {
                int v = F7.sf.value(j);
                if (is_baseline_block(7, j / 3) && F7.line_graded(j)) {
                    if (!F7.run[j].shows) ++norun7;
                    else for (long x = xa + 1; x <= xb - 1; ++x) {
                        uint16_t p = F7.row[j].px[(size_t)(x - (long)F7.g.xL)];
                        for (int c = 0; c < 3; ++c) { ++n7; if (comp_bits(p, c) != letters(v, c)) ++bad7; }
                    }
                }
                if (F8.line_graded(j)) {
                    for (long x = xa + 1; x <= xb - 1; ++x) {
                        uint16_t p = F8.row[j].px[(size_t)(x - (long)F8.g.xL)];
                        for (int c = 0; c < 3; ++c) { ++n8; if (comp_bits(p, c) != 3 - letters(v, c)) ++bad8; }
                    }
                }
                if (F9.line_graded(j) && v >= 1 && v <= 3) {
                    for (long x = xa + 1; x <= xb - 1; ++x) {
                        size_t i = (size_t)(x - (long)F9.g.xL);
                        const std::set<uint16_t>& ref = b7.s[v][i];
                        ++n9;
                        if (!(ref.size() == 1 && *ref.begin() == F9.row[j].px[i])) ++bad9;
                    }
                }
            }
            checks.note("P6.1: F7 baseline " + std::to_string(n7) + " component values, " + std::to_string(bad7) + " differ from the letters of T0, " +
                        std::to_string(norun7) + " baseline lines show no run; F8 " + std::to_string(n8) + " component values, " +
                        std::to_string(bad8) + " differ from the letters of T0c; F9 " + std::to_string(n9) + " pixels, " +
                        std::to_string(bad9) + " differ from the baseline pixel of F7");
            e.ok = n7 > 0 && n8 > 0 && n9 > 0 && bad7 == 0 && norun7 == 0 && bad8 == 0 && bad9 == 0;
            e.label = "P6.1 APPC-shr.c9 p.302: the inner pixels of a sprite-0 line of value v show the encoded letters of Color 16+v in bits 3-2 of R, G and B "
                      "(F7 under T0, F8 under T0c) and are unchanged by COLOR00-0F (F9 against F7), pixels F7 " + std::to_string(n7) +
                      " F8 " + std::to_string(n8) + " F9 " + std::to_string(n9);
        }
        emit(e, run_pre && f9pf_ok, "P6.1 APPC-shr.c9 p.302: NOT GRADED, a precondition (C4, C5, C7, C10, C11, RUN-ENDS or P6.1-F9PF) did not hold");
    }

    // ---- P6.2 (CH4-appC-context.c6, p.301; p.108, p.103)
    int p62 = 0;   // 0 not graded, 1 PASS, 2 FAIL
    if ((run_pre || c11_only) && have10) {
        BaseSets b7 = build_base_sets(F7), b10 = build_base_sets(F10);
        bool graded = run_pre;
        // SuperHires half: some block among the thirteen inversion blocks changes an inner pixel
        long cmp7 = 0, chg7 = 0; std::string per;
        for (int blk = 2; blk <= 14; ++blk) {
            long ch = 0;
            for (int j = 3 * blk; j < 3 * blk + 3; ++j) {
                int v = F7.sf.value(j);
                if (!F7.line_graded(j) || v < 1 || v > 3) continue;
                for (long x = r7.xa + 1; x <= r7.xb - 1; ++x) {
                    size_t i = (size_t)(x - (long)F7.g.xL);
                    if (b7.s[v][i].empty()) continue;
                    ++cmp7;
                    if (!b7.s[v][i].count(F7.row[j].px[i])) { ++chg7; ++ch; }
                }
            }
            per += " block" + std::to_string(blk) + "=" + std::to_string(ch);
        }
        checks.note("P6.2 F7 inner pixels changed per block (blocks 2-14 invert COLOR16, COLOR20-31 in that order):" + per);
        bool shr_ok = cmp7 > 0 && chg7 > 0;
        Eval e1; e1.ran = true; e1.ok = shr_ok;
        e1.label = "P6.2-SHR p.301 (SuperHires half): at least one of the thirteen inversion blocks (COLOR16, COLOR20-31) of F7 changes an inner pixel of sprite 0, "
                   "inner pixels compared " + std::to_string(cmp7) + ", changed " + std::to_string(chg7);
        emit(e1, graded, "");
        // Hires half: none of them changes an inner pixel in F10
        long cmp10 = 0, chg10 = 0;
        for (int blk = 2; blk <= 14; ++blk)
            for (int j = 3 * blk; j < 3 * blk + 3; ++j) {
                int v = F10.sf.value(j);
                if (!F10.line_graded(j) || v < 1 || v > 3) continue;
                for (long x = r10.xa + 1; x <= r10.xb - 1; ++x) {
                    size_t i = (size_t)(x - (long)F10.g.xL);
                    if (b10.s[v][i].empty()) continue;
                    ++cmp10;
                    if (!b10.s[v][i].count(F10.row[j].px[i])) ++chg10;
                }
            }
        bool hires_ok = cmp10 > 0 && chg10 == 0;
        Eval e2; e2.ran = true; e2.ok = hires_ok;
        e2.label = "P6.2-HIRES p.108, p.103 (Hires half): none of the thirteen inversion blocks of F10 changes an inner pixel of sprite 0, "
                   "inner pixels compared " + std::to_string(cmp10) + ", changed " + std::to_string(chg10);
        emit(e2, graded, "");
        // the Hires comparator is live: the baseline shows register 16+v, blocks 15-17 invert 17, 18, 19 on the lines of value 1, 2, 3
        long cmpb = 0, badb = 0, cmpl = 0, badl = 0, cmpo = 0, bado = 0;
        for (int v = 1; v <= 3; ++v)
            for (long x = r10.xa + 1; x <= r10.xb - 1; ++x) {
                size_t i = (size_t)(x - (long)F10.g.xL);
                ++cmpb;
                const auto& st = b10.s[v][i];
                if (!(st.size() == 1 && *st.begin() == kQ0[v])) ++badb;
            }
        for (int blk = 15; blk <= 17; ++blk)
            for (int j = 3 * blk; j < 3 * blk + 3; ++j) {
                int v = F10.sf.value(j);
                if (!F10.line_graded(j) || v < 1 || v > 3) continue;
                for (long x = r10.xa + 1; x <= r10.xb - 1; ++x) {
                    size_t i = (size_t)(x - (long)F10.g.xL);
                    const auto& st = b10.s[v][i];
                    uint16_t p = F10.row[j].px[i];
                    if (v == blk - 14) {
                        ++cmpl;
                        if (!(st.size() == 1 && p == (uint16_t)(*st.begin() ^ 0x0FFF))) ++badl;
                    } else {
                        ++cmpo;
                        if (!st.count(p)) ++bado;
                    }
                }
            }
        bool live_ok = cmpb > 0 && cmpl > 0 && cmpo > 0 && badb == 0 && badl == 0 && bado == 0;
        Eval e3; e3.ran = true; e3.ok = live_ok;
        e3.label = "P6.2-LIVE p.108 (Hires half): baseline inner pixels of F10 show all twelve bits of register 16+v, blocks 15-17 invert all twelve bits of "
                   "the lines of value 1, 2, 3 and no other line, baseline " + std::to_string(cmpb) + " pixels (" + std::to_string(badb) +
                   " differ), inverted " + std::to_string(cmpl) + " (" + std::to_string(badl) + " differ), other " + std::to_string(cmpo) +
                   " (" + std::to_string(bado) + " changed)";
        emit(e3, graded, "");
        Eval e4; e4.ran = true; e4.ok = shr_ok && hires_ok && live_ok;
        e4.label = "P6.2 CH4-appC-context.c6 p.301, p.108: some inner sprite-0 pixel depends on an upper register other than COLOR17-19 in SuperHires "
                   "and none does in Hires (SuperHires half " + std::string(shr_ok ? "holds" : "fails") + ", Hires half " +
                   std::string(hires_ok ? "holds" : "fails") + ", Hires comparator " + std::string(live_ok ? "live" : "not live") + ")";
        emit(e4, graded, "");
        if (graded) p62 = e4.ok ? 1 : 2;
    } else if (pre && !have7) {
        checks.record("P6.2 CH4-appC-context.c6 p.301: NOT GRADED, no baseline sprite line of F7 shows a run, so there are no inner pixels");
    } else {
        checks.record("P6.2 CH4-appC-context.c6 p.301: NOT GRADED, a precondition (C4, C5, C7, C10, C11, RUN-ENDS) did not hold or F10 shows no run");
    }

    // ---- P5.U: no visible playfield pixel depends on an upper register (F7, outside the sprite run)
    int p5u = 0;
    if (pre && (!have7 || c11 || c11_only) && (!have7 || ends)) {
        BaseSets b7 = build_base_sets(F7);
        long ua = have7 ? r7.xa : 1, ub = have7 ? r7.xb : 0;     // no run: every graded pixel is outside it
        long cmp = 0, chg = 0, blines = 0; std::string per;
        for (int blk = 2; blk <= 14; ++blk) {
            long ch = 0;
            for (int j = 3 * blk; j < 3 * blk + 3; ++j) {
                int v = F7.sf.value(j);
                if (!F7.line_graded(j) || v < 1 || v > 3) continue;
                ++blines;
                for (long x = (long)F7.g.xL; x < (long)F7.g.xR; ++x) {
                    if (x >= ua && x <= ub) continue;
                    size_t i = (size_t)(x - (long)F7.g.xL);
                    if (b7.s[v][i].empty()) continue;
                    ++cmp;
                    if (!b7.s[v][i].count(F7.row[j].px[i])) { ++chg; ++ch; }
                }
            }
            per += " block" + std::to_string(blk) + "=" + std::to_string(ch);
        }
        checks.note("P5.U F7 playfield pixels outside the run changed per block (blocks 2-14 invert COLOR16, COLOR20-31 in that order):" + per);
        Eval e; e.ran = true; e.ok = cmp > 0 && chg == 0;
        e.label = "P5.U p.301/302: no inversion of COLOR16 or COLOR20-31 changes a visible playfield pixel of F7 outside the sprite run, "
                  "lines " + std::to_string(blines) + ", pixels compared " + std::to_string(cmp) + ", changed " + std::to_string(chg);
        bool graded = !have7 || run_pre;
        emit(e, graded, "");
        if (graded) p5u = e.ok ? 1 : 2;
    } else {
        checks.record("P5.U p.301/302: NOT GRADED, a precondition (C4, C5, C7, C10, C11 or RUN-ENDS) did not hold");
    }

    // ---- P7.2 and P7.3: claims that name the clauses of two propositions at once
    // (SPECIFICATION.md 7/P7: PASS when both halves pass, FAIL when either fails, not graded when either is not graded)
    auto joint = [&](int a, int b, const std::string& label, const std::string& ng) {
        std::string halves = std::string(a == 1 ? "holds" : a == 2 ? "fails" : "not graded") + " / " + (b == 1 ? "holds" : b == 2 ? "fails" : "not graded");
        bool failed = a == 2 || b == 2;
        if (!failed && (a == 0 || b == 0)) checks.record(ng);
        else checks.expect(a == 1 && b == 1, label + halves);
    };
    joint(p5l_state, p62, "P7.2 APPC-shr.c6 p.301: multiplexing occurs for bitplane and sprite colour information (P5.L and P6.2 together; P5.L / P6.2): ",
          "P7.2 APPC-shr.c6 p.301 (P5.L and P6.2 together): NOT GRADED, P5.L or P6.2 is not graded");
    joint(p5l_state, p5u, "P7.3 APPC-shr.c8 p.301: the bitplane colours are encoded in the sixteen lower registers (P5.L and P5.U together; P5.L / P5.U): ",
          "P7.3 APPC-shr.c8 p.301 (P5.L and P5.U together): NOT GRADED, P5.L or P5.U is not graded");
}

// Grades one capture against every Phase-A proposition and precondition.
// Shared by the normal run (prog_main, default Config) and every mutation
// (a different program and/or Config, same comparator, unchanged): the
// SAME checks.expect() calls run either way, per BRIEF.md's own rule that a
// mutation must never invert the expected verdict.
void grade_capture(const Capture& cap, Checks& checks, const std::string& mutate) {
    checks.expect(cap.loaded && cap.released, "fixture: program loaded and CPU released from reset");
    checks.expect(cap.fixture.has_value() && *cap.fixture == 0xBEEF,
                  "fixture: the program-stored constant $BEEF was read back before anything else is graded");
    checks.expect(cap.done.has_value(), "C2 precondition: the program's DONE mark was written (p.223/337/339 setup completed)");

    bool c1 = false;
    if (cap.vposr && cap.deniseid) {
        int chip = (*cap.vposr >> 8) & 0x7F;
        c1 = (chip == 0x20 || chip == 0x30) && (*cap.deniseid & 0xFF) == 0xFC;
        if (chip == 0x20) checks.note("VPOSR chip id $20: PAL enhanced Agnus (p.299)");
        else if (chip == 0x30) checks.note("VPOSR chip id $30: NTSC enhanced Agnus (p.299)");
    }
    checks.expect(c1, "C1 precondition: the Enhanced Chip Set is present (VPOSR chip id $20/$30, DENISEID low byte $FC, p.299)");

    if (!cap.done) {
        checks.record("run did not reach DONE; no proposition graded");
        return;
    }

    // C8's own contrast rewrites the pattern section, so C3 must
    // compare against what THIS run's program actually wrote, not always
    // the baseline generator.
    auto pat = (mutate == "C8") ? build_pattern_vc8() : build_pattern();

    // ---- graded frames F1-F6. F7-F10 are graded by grade_sprite_frames at
    // the end of this function.
    FrameLines fl1 = decode_frame_lines(cap, 1, kPlane1Buf, kPlane2Buf);
    FrameLines fl2 = decode_frame_lines(cap, 2, kPlane1Buf, kPlane2Buf);
    FrameLines fl3 = decode_frame_lines(cap, 3, kPlane1Buf, kPlane2Buf);
    FrameLines fl4 = decode_frame_lines(cap, 4, kPlane1Buf, kPlane2Buf);
    FrameLines fl5 = decode_frame_lines(cap, 5, kPlane1Buf, kPlane2Buf);
    FrameLines fl6 = decode_frame_lines(cap, 6, kPlane1Buf, kPlane2Buf);

    // R0: w and phi, measured once (F5's own pattern lines: hires mode,
    // easiest to get unambiguous single-sample runs from) and reused for
    // every frame (SPECIFICATION.md 5, R0: "the same w and phi serve every
    // frame, because a hires period lasts the same time in every mode").
    WPhi wp = measure_global_w_phi(cap.video, fl5);
    if (!wp.ok) wp = measure_global_w_phi(cap.video, fl1);
    checks.expect(wp.ok, "R0 precondition: a unique hires-period sample width w and phase phi exist");
    if (wp.ok) checks.note("R0: w=" + std::to_string(wp.w) + " phi=" + std::to_string(wp.phi));

    GradedFrame f1 = finish_graded_frame(cap, std::move(fl1), wp.w, wp.phi);
    GradedFrame f2 = finish_graded_frame(cap, std::move(fl2), wp.w, wp.phi);
    GradedFrame f3 = finish_graded_frame(cap, std::move(fl3), wp.w, wp.phi);
    GradedFrame f4 = finish_graded_frame(cap, std::move(fl4), wp.w, wp.phi);
    GradedFrame f5 = finish_graded_frame(cap, std::move(fl5), wp.w, wp.phi);
    GradedFrame f6 = finish_graded_frame(cap, std::move(fl6), wp.w, wp.phi);

    // R2's own check requires the SuperHires mapping specifically
    // (mode == 0), not just "a mapping fits uniquely" -- the hires mapping
    // fits just as uniquely when F2-F4 run in hires (NMH_MUTATE=V4B/V4D/V5),
    // which the plain f2.ok/f3.ok/f4.ok test could not tell apart.
    checks.expect(f1.ok && f1.mode == 0, "C5/R2 precondition: F1's SuperHires pixel mapping (i=2x-s) fits uniquely");
    checks.expect(f5.ok && f5.mode == 1, "R2 precondition: F5's hires pixel mapping (i=x-s) fits uniquely");
    checks.expect(f6.ok && f6.mode == 2, "R2 precondition: F6's lores pixel mapping fits uniquely");
    checks.expect(f2.ok && f2.mode == 0 && f3.ok && f3.mode == 0 && f4.ok && f4.mode == 0,
                  "R2 precondition: F2-F4's SuperHires pixel mapping (i=2x-s) fits uniquely, and is the SuperHires mapping");

    checks.note("R0/R1/R2 F1: ok=" + std::to_string(f1.ok) + " mode=" + std::to_string(f1.mode) +
                " s=" + std::to_string(f1.s) + " xL=" + std::to_string(f1.xL) + " xR=" + std::to_string(f1.xR));

    // "in F1-F6 the same s fits the bottom Q0 lines" (5/R2) --
    // re-searched independently from the top-line s already found.
    bool r2_bottom = true;
    if (f1.fc.bands.size() == 12) r2_bottom = r2_bottom && check_bottom_s(cap.video, f1, 11);
    if (f2.fc.bands.size() == 81) r2_bottom = r2_bottom && check_bottom_s(cap.video, f2, 80);
    if (f3.fc.bands.size() == 81) r2_bottom = r2_bottom && check_bottom_s(cap.video, f3, 80);
    if (f4.fc.bands.size() >= 33) r2_bottom = r2_bottom && check_bottom_s(cap.video, f4, 32);
    if (f5.fc.bands.size() == 33) r2_bottom = r2_bottom && check_bottom_s(cap.video, f5, 32);
    if (f6.fc.bands.size() == 17) r2_bottom = r2_bottom && check_bottom_s(cap.video, f6, 16);
    checks.expect(r2_bottom, "R2 precondition: the same s that fits the top Q0 lines also fits the bottom Q0 lines, in F1-F6");

    // C6: identical ends on every pure-R line, asserted per
    // frame -- not the first fitting line, silently.
    C6Result c6f[6] = { check_c6(cap.video, f1), check_c6(cap.video, f2), check_c6(cap.video, f3),
                         check_c6(cap.video, f4), check_c6(cap.video, f5), check_c6(cap.video, f6) };
    static const char* kFrameNames6[6] = {"F1","F2","F3","F4","F5","F6"};
    bool c6_all = true;
    for (int fi = 0; fi < 6; ++fi) {
        c6_all = c6_all && c6f[fi].ok;
        checks.expect(c6f[fi].ok,
            std::string("C6 precondition ") + kFrameNames6[fi] +
            ": the check interval has identical ends on every pure-R line (256-576 hires periods wide), " +
            std::to_string(c6f[fi].lines_checked) + " lines checked, " + std::to_string(c6f[fi].lines_mismatch) + " mismatched");
    }
    (void)c6_all;

    // C3: every graded line of F1-F6, not F1 alone, and
    // independent of C5 (f1.ok no longer gates it -- a fetch-continuity
    // precondition does not depend on whether the pixel mapping also fit).
    long c3_bad[6] = {0,0,0,0,0,0};
    GradedFrame* c3_frames[6] = {&f1,&f2,&f3,&f4,&f5,&f6};
    bool c3 = true;
    for (int fi = 0; fi < 6; ++fi)
        c3 = precondition_c3(*c3_frames[fi], pat.first, pat.second, &c3_bad[fi]) && c3;
    checks.expect(c3, "C3 precondition: F1-F6's graded lines fetch >=16 consecutive words per plane, data as written, bad lines=" +
                  std::to_string(c3_bad[0]+c3_bad[1]+c3_bad[2]+c3_bad[3]+c3_bad[4]+c3_bad[5]));

    bool c9 = precondition_c9(cap.log, f1.fc.start_cycle, f6.fc.end_cycle);
    checks.expect(c9, "C9 precondition: no sprite-0/null-sprite fetch in F1-F6 shows a nonzero word");

    // C4: the header MOVEs carry the planned register VALUES
    // (BPLCON0-2, DIW, DDF, modulos, pointers, both palettes), not just the
    // planned count; F0 (the lead-in copy of F1, never graded) also ran.
    FrameCop fc0 = decode_frame(cap.log, 0);
    bool c4_headers = check_frame_header(fc0, 0x2040, kQ0) &&
                       check_frame_header(f1.fc, 0x2040, kQ0) && check_frame_header(f2.fc, 0x2040, kQ0) &&
                       check_frame_header(f3.fc, 0x2040, kQ0) && check_frame_header(f4.fc, 0x2040, kQ0) &&
                       check_frame_header(f5.fc, 0xA000, kQ0) && check_frame_header(f6.fc, 0x2000, kQ0);
    bool c4 = fc0.found && f1.fc.found && f2.fc.found && f3.fc.found && f4.fc.found && f5.fc.found && f6.fc.found &&
              f1.fc.header.size()==61 && f2.fc.header.size()==61 && f3.fc.header.size()==61 &&
              f4.fc.header.size()==61 && f5.fc.header.size()==61 && f6.fc.header.size()==61 &&
              f1.fc.start_cycle < f2.fc.start_cycle && f2.fc.start_cycle < f3.fc.start_cycle &&
              f3.fc.start_cycle < f4.fc.start_cycle && f4.fc.start_cycle < f5.fc.start_cycle &&
              f5.fc.start_cycle < f6.fc.start_cycle && c4_headers;
    checks.expect(c4, "C4 precondition: F0-F6 ran their own lists in order, each with the planned header register values");

    // C7 band count (a coarse proxy kept alongside the timing check below):
    // a MOVE landing behind the line's own fetch would either merge into
    // the next band or go missing, changing this count.
    bool c7_count = f1.fc.bands.size()==12 && f2.fc.bands.size()==81 && f3.fc.bands.size()==81 &&
              f4.fc.bands.size()==34 && f5.fc.bands.size()==33 && f6.fc.bands.size()==17;
    checks.expect(c7_count, "C7 precondition (band count): every planned band change was found, once each, in F1-F6");

    // C7 timing: SPECIFICATION.md 6/C7's own condition -- every
    // band's WAIT at h=$00, after the previous line's last bitplane fetch
    // and, spacer lines excepted, before its own line's first.
    C7Result c7t[6] = { check_c7_timing(f1), check_c7_timing(f2), check_c7_timing(f3),
                        check_c7_timing(f4), check_c7_timing(f5), check_c7_timing(f6) };
    bool c7_timing = true; int c7_bad_h=0,c7_bad_after=0,c7_bad_before=0;
    for (int fi = 0; fi < 6; ++fi) {
        c7_timing = c7_timing && c7t[fi].ok;
        c7_bad_h += c7t[fi].bad_h; c7_bad_after += c7t[fi].bad_after; c7_bad_before += c7t[fi].bad_before;
    }
    checks.expect(c7_timing,
        "C7 precondition (SPECIFICATION.md 6/C7, timing): every band's WAIT is at h=$00, after the previous line's "
        "last bitplane fetch and, spacer lines excepted, before its own line's first, in F1-F6 -- bad_h=" +
        std::to_string(c7_bad_h) + " bad_after=" + std::to_string(c7_bad_after) + " bad_before=" + std::to_string(c7_bad_before));

    // C8 old (kept as a diversity proxy on F1's own Q0 lines):
    std::vector<int> f1_top_tail;
    for (int i = 0; i < 24 && i < (int)f1.lines.size(); ++i) f1_top_tail.push_back(i);
    if (f1.fc.bands.size() == 12) {
        int tail_lo = f1.fc.bands[11].line + 1, tail_hi = 199;
        for (int i = tail_lo; i <= tail_hi && i < (int)f1.lines.size(); ++i) f1_top_tail.push_back(i);
    }
    Baseline bl_q0_f1 = f1.ok ? build_baseline(cap.video, f1.lines, f1.out_of, f1_top_tail, f1.mode, f1.s, f1.w, f1.phi, (long)f1.xL, (long)f1.xR)
                               : Baseline{};
    checks.expect(bl_q0_f1.value.size() >= 32,
                  "C8 precondition (F1 Q0 diversity): F1's own Q0 lines show at least half of the 64 possible neighbour triples");

    // C8: SPECIFICATION.md 6/C8's own population -- all 16
    // ordered pairs, both as (v[i],v[i+1]) and (v[i-1],v[i]), in every band
    // of F1-F4 (F1's own eleven content bands, and F2-F4's 192 perturbation
    // bands).
    int c8_bands_ok = 0, c8_bands_total = 0;
    if (f1.ok) for (size_t bi = 0; bi < kF1Bands.size() && bi < f1.fc.bands.size(); ++bi) {
        const Band& b = f1.fc.bands[bi];
        ++c8_bands_total; if (band_pair_coverage(f1, b.line+1, b.line+12)) ++c8_bands_ok;
    }
    for (int gi = 0; gi < 192; ++gi) {
        GradedFrame* gf; int band_idx;
        if (gi < 80) { gf=&f2; band_idx=gi; } else if (gi < 160) { gf=&f3; band_idx=gi-80; } else { gf=&f4; band_idx=gi-160; }
        if (!gf->ok || band_idx >= (int)gf->fc.bands.size()) continue;
        const Band& b = gf->fc.bands[band_idx];
        ++c8_bands_total; if (band_pair_coverage(*gf, b.line, b.line+1)) ++c8_bands_ok;
    }
    checks.expect(c8_bands_total > 0 && c8_bands_ok == c8_bands_total,
        "C8 precondition (SPECIFICATION.md 6/C8): every ordered pair occurs both as (v[i],v[i+1]) and (v[i-1],v[i]) "
        "in every band of F1-F4, " + std::to_string(c8_bands_ok) + " of " + std::to_string(c8_bands_total) + " bands");

    // R1: every output copy of a graded source line agrees with
    // the others (SPECIFICATION.md 4.7) -- checked directly, not assumed
    // from using only the last copy throughout.
    R1Result r1f[6] = { check_r1_copies(cap.video, f1), check_r1_copies(cap.video, f2), check_r1_copies(cap.video, f3),
                         check_r1_copies(cap.video, f4), check_r1_copies(cap.video, f5), check_r1_copies(cap.video, f6) };
    long r1_pairs = 0, r1_mismatch = 0;
    for (auto& r : r1f) { r1_pairs += r.pairs; r1_mismatch += r.mismatches; }
    checks.expect(r1_pairs > 0 && r1_mismatch == 0,
        "R1 precondition (SPECIFICATION.md 4.7): every output copy of a graded source line agrees with the others, " +
        std::to_string(r1_pairs) + " pairs checked, " + std::to_string(r1_mismatch) + " differing");

    // ==== P1, P2 (SuperHires key), P3 (coarseness) ==========================
    P1State p1s; P2State p2s;
    // The diversity source id names the PALETTE, not the
    // stimulus instance: F1's own Q0 top/bottom and F2-F4's Q0 top/bottom
    // are all literally the same sixteen register values, so they must
    // collapse to one source ("Q0"), not count as six distinct ones -- only
    // a genuinely different palette (Q1, Q2, ...) should add diversity.
    if (f1.ok) {
        grade_range_p1p2(cap.video, f1, kQ0, 0, 23, &p1s, &p2s, "Q0");
        for (size_t bi = 0; bi < kF1Bands.size() && bi < f1.fc.bands.size(); ++bi) {
            const Band& b = f1.fc.bands[bi];
            grade_range_p1p2(cap.video, f1, *kF1Bands[bi].second, b.line+1, b.line+12, &p1s, &p2s,
                              kF1Bands[bi].first);
        }
        if (f1.fc.bands.size() == 12) {
            int tail_lo = f1.fc.bands[11].line + 1;
            grade_range_p1p2(cap.video, f1, kQ0, tail_lo, 199, &p1s, &p2s, "Q0");
        }
    }
    for (GradedFrame* gf : {&f2, &f3, &f4}) {
        if (!gf->ok) continue;
        grade_range_p1p2(cap.video, *gf, kQ0, 0, 23, &p1s, &p2s, "Q0");
        int last_band = (int)gf->fc.bands.size() - 1;
        if (last_band >= 0) grade_range_p1p2(cap.video, *gf, kQ0, gf->fc.bands[last_band].line+1, 199, &p1s, &p2s, "Q0");
    }

    static const char* kColorName[4] = {"0","1","2","3"};
    static const char* kCompName[3] = {"R","G","B"};
    static const char* kP1Id[12] = {
        "P1.0R APPC-bplkeykey.c0r","P1.0G APPC-bplkeykey.c0g","P1.0B APPC-bplkeykey.c0b",
        "P1.1R APPC-bplkeykey.c1r","P1.1G APPC-bplkeykey.c1g","P1.1B APPC-bplkeykey.c1b",
        "P1.2R APPC-bplkeykey.c2r","P1.2G APPC-bplkeykey.c2g","P1.2B APPC-bplkeykey.c2b",
        "P1.3R APPC-bplkeykey.c3r","P1.3G APPC-bplkeykey.c3g","P1.3B APPC-bplkeykey.c3b",
    };
    for (int idx = 0; idx < 12; ++idx) {
        std::string label = std::string(kP1Id[idx]) + " p.301: Color " + kColorName[idx/3] + " " +
            (idx%3==0?"red":idx%3==1?"green":"blue") + " shows the key's letters in bits 3-2, checked " +
            std::to_string(p1s.total[idx]) + " graded pixels";
        checks.expect(p1s.total[idx] > 0 && p1s.fail[idx] == 0, label);
    }
    static const char* kP2Id[3] = {
        "P2.R APPC-shr.c3 CH4-appC-context.c3", "P2.G APPC-shr.c4 CH4-appC-context.c4",
        "P2.B APPC-shr.c5 CH4-appC-context.c5",
    };
    for (int comp = 0; comp < 3; ++comp) {
        std::string label = std::string(kP2Id[comp]) + " p.301: " + kCompName[comp] +
            " depends only on its two encoded letters, " + std::to_string(p2s.total[comp]) + " graded pixels, " +
            std::to_string(p2s.bykey[comp].size()) + " groups";
        checks.expect(p2s.total[comp] > 0 && p2s.bad[comp].empty(), label);
    }
    // ---- P2's own palette-diversity requirement (SPECIFICATION.md
    // 7/P2 item 4): "For every (c, X), at least one group must contain
    // pixels from two palettes that differ in some other Color's letters of
    // X." Reported per group (how many palettes contributed) and asserted
    // as its own check -- this is what tells a green P2 (e.g. under
    // NMH_MUTATE=C5/V2, where only F2-F4's single Q0 palette contributes)
    // apart from a P2 that actually exercised the claim.
    for (int comp = 0; comp < 3; ++comp) {
        int groups_total = (int)p2s.bykey[comp].size();
        int groups_diverse = 0;
        for (auto& kv : p2s.sources[comp]) if (kv.second.size() >= 2) ++groups_diverse;
        checks.note("P2." + std::string(kCompName[comp]) + " palette diversity: " +
                    std::to_string(groups_diverse) + " of " + std::to_string(groups_total) +
                    " groups have >=2 contributing palettes");
        checks.expect(groups_total > 0 && groups_diverse >= 1,
                      std::string(kP2Id[comp]) + " p.301 (7/P2 item 4, palette diversity): at least one (value, letters) group is populated from two distinct palettes");
    }

    // ---- P3: SuperHires coarser than Hires and Lores ---------------------
    // SPECIFICATION.md 7/P3 item 2: graded over every band of F1
    // -- the top and bottom Q0 lines and every Q1-Q5/Z/O/S0-S3 band -- not
    // the S0-S3 bands alone, which trivially guarantees "four" because
    // Color 0 never changes there.
    P3Comp p3shr;
    if (f1.ok) {
        collect_p3(cap.video, f1, 0, 23, 1, p3shr);
        for (size_t bi = 0; bi < kF1Bands.size() && bi < f1.fc.bands.size(); ++bi) {
            const Band& b = f1.fc.bands[bi];
            collect_p3(cap.video, f1, b.line+1, b.line+12, 1, p3shr);
        }
        if (f1.fc.bands.size() == 12) {
            int tail_lo = f1.fc.bands[11].line + 1;
            collect_p3(cap.video, f1, tail_lo, 199, 1, p3shr);
        }
    }
    P3Comp p3hires;
    if (f5.ok) for (int k = 0; k < 16; ++k) if (k < (int)f5.fc.bands.size())
        collect_p3(cap.video, f5, f5.fc.bands[k].line, f5.fc.bands[k].line+1, 1, p3hires);
    P3Comp p3lores;
    if (f6.ok) for (int k = 0; k < 16; ++k) if (k < (int)f6.fc.bands.size())
        collect_p3(cap.video, f6, f6.fc.bands[k].line, f6.fc.bands[k].line+1, 1, p3lores);

    bool p3_shr_ok = p3shr.total > 0;
    for (int c = 0; c < 3; ++c) if (p3shr.vals[c].size() != 4) p3_shr_ok = false;
    bool p3h_ok = p3hires.total > 0;
    for (int c = 0; c < 3; ++c) if (p3hires.vals[c].size() != 16) p3h_ok = false;
    bool p3l_ok = p3lores.total > 0;
    for (int c = 0; c < 3; ++c) if (p3lores.vals[c].size() != 16) p3l_ok = false;

    checks.note("P3 SuperHires distinct values per component: R=" + std::to_string(p3shr.vals[0].size()) +
                " G=" + std::to_string(p3shr.vals[1].size()) + " B=" + std::to_string(p3shr.vals[2].size()));
    checks.note("P3 hires distinct values per component: R=" + std::to_string(p3hires.vals[0].size()) +
                " G=" + std::to_string(p3hires.vals[1].size()) + " B=" + std::to_string(p3hires.vals[2].size()));
    checks.note("P3 lores distinct values per component: R=" + std::to_string(p3lores.vals[0].size()) +
                " G=" + std::to_string(p3lores.vals[1].size()) + " B=" + std::to_string(p3lores.vals[2].size()));

    checks.expect(p3_shr_ok && p3h_ok,
                  "P3.H CH4-appC-context.c1 p.301: SuperHires (4 values, S0-S3) coarser than Hires (16 values, F5)");
    checks.expect(p3_shr_ok && p3l_ok,
                  "P3.L CH4-appC-context.c2 p.301: SuperHires (4 values, S0-S3) coarser than Lores (16 values, F6)");
    checks.expect(p3_shr_ok && p3h_ok && p3l_ok,
                  "P7.1 APPC-shr.c1 p.301: SuperHires coarser than either Hires or Lores (P3.H and P3.L together)");

    // ---- P4: 192 per-bit propositions (F2, F3, F4) -------------------------
    auto plan = perturbation_plan_all();
    Baseline bl2, bl3, bl4;
    if (f2.ok) {
        std::vector<int> idxs; for (int i=0;i<24 && i<(int)f2.lines.size();++i) idxs.push_back(i);
        if (f2.fc.bands.size()==81) { int lo=f2.fc.bands[80].line+1; for (int i=lo;i<=199 && i<(int)f2.lines.size();++i) idxs.push_back(i); }
        bl2 = build_baseline(cap.video, f2.lines, f2.out_of, idxs, f2.mode, f2.s, f2.w, f2.phi, (long)f2.xL, (long)f2.xR);
    }
    if (f3.ok) {
        std::vector<int> idxs; for (int i=0;i<24 && i<(int)f3.lines.size();++i) idxs.push_back(i);
        if (f3.fc.bands.size()==81) { int lo=f3.fc.bands[80].line+1; for (int i=lo;i<=199 && i<(int)f3.lines.size();++i) idxs.push_back(i); }
        bl3 = build_baseline(cap.video, f3.lines, f3.out_of, idxs, f3.mode, f3.s, f3.w, f3.phi, (long)f3.xL, (long)f3.xR);
    }
    if (f4.ok) {
        std::vector<int> idxs; for (int i=0;i<24 && i<(int)f4.lines.size();++i) idxs.push_back(i);
        if (f4.fc.bands.size()>=33) { int lo=f4.fc.bands[32].line+1; for (int i=lo;i<=199 && i<(int)f4.lines.size();++i) idxs.push_back(i); }
        bl4 = build_baseline(cap.video, f4.lines, f4.out_of, idxs, f4.mode, f4.s, f4.w, f4.phi, (long)f4.xL, (long)f4.xR);
    }

    long p5l_shr_changed = 0, p5l_shr_any = 0;
    long not_observable = 0;
    // P1J: per-(n,b) verdict for the joint key claim, filled as
    // the 192-bit loop runs. 0 = not graded (frame not decoded), 1 = NOT
    // OBSERVABLE, 2 = PASS, 3 = FAIL.
    int p4_state[16][12] = {};
    for (int gi = 0; gi < 192; ++gi) {
        int n = plan[gi].first, b = plan[gi].second;
        GradedFrame* gf; Baseline* bl; int band_idx;
        if (gi < 80)       { gf=&f2; bl=&bl2; band_idx=gi; }
        else if (gi < 160) { gf=&f3; bl=&bl3; band_idx=gi-80; }
        else               { gf=&f4; bl=&bl4; band_idx=gi-160; }
        if (!gf->ok) continue;
        P4Verdict pv = grade_p4_band(cap.video, *gf, *bl, band_idx, n, b);
        std::string label = "P4.n" + std::to_string(n) + "b" + std::to_string(b) +
            " APPC-bpltab.r" + [&]{ char buf[3]; std::snprintf(buf,sizeof buf,"%02X",n); return std::string(buf); }() +
            "b" + [&]{ char buf[3]; std::snprintf(buf,sizeof buf,"%02d",b); return std::string(buf); }() +
            " p.301: bit " + std::to_string(b) + " of COLOR" +
            [&]{ char buf[3]; std::snprintf(buf,sizeof buf,"%02X",n); return std::string(buf); }() +
            " changes only its own place, checked=" + std::to_string(pv.checked) +
            " changed=" + std::to_string(pv.changed) + " excluded=" + std::to_string(pv.excluded);
        // SPECIFICATION.md 7/P4 item 4: FAIL if any graded pixel differs
        // anywhere other than the allowed places; OBSERVED if the allowed
        // bit itself moved; PASS when OBSERVED and not FAIL; NOT OBSERVABLE
        // when NEITHER. A failing, unobserved bit is a FAIL, not
        // a dropped record: the old `if (!pv.observed) ... continue;` hid
        // exactly that case.
        if (!pv.observed && !pv.fail) {
            checks.record(label + " -- NOT OBSERVABLE (L1)");
            ++not_observable;
            p4_state[n][b] = 1;
            continue;
        }
        bool pass = pv.observed && !pv.fail;
        checks.expect(pass, label);
        p4_state[n][b] = pass ? 2 : 3;
        if (n >= 4) { p5l_shr_changed += pv.changed; p5l_shr_any += pv.any_changed; }
    }
    checks.note("P4: " + std::to_string(not_observable) + " of 192 bits not observable (L1)");

    // ---- V4c, the null case: each frame's own closing restore
    // band (no new inversion) is predicted to change no graded pixel; it
    // must come out NOT OBSERVED -- SPECIFICATION.md 7/P4's own contrast,
    // graded nowhere before this.
    auto grade_null_band = [&](GradedFrame& gf, Baseline& bl, int band_idx, const char* fname) {
        if (!gf.ok || band_idx < 0 || band_idx >= (int)gf.fc.bands.size()) {
            checks.record(std::string("V4c precondition (") + fname + "): closing restore band not decoded, not graded");
            return;
        }
        const Band& band = gf.fc.bands[band_idx];
        long checked = 0, diffs = 0;
        for (int li = band.line; li <= band.line + 1 && li < (int)gf.lines.size(); ++li) {
            if (gf.out_of[li].empty()) continue;
            nmh::Span sp = gf.out_of[li].back();
            for (size_t x = gf.xL; x < gf.xR; ++x) {
                auto oidx = pixel_sample(sp, gf.w, gf.phi, (long)x);
                if (!oidx) continue;
                int i = pixel_index(gf.mode, (int)x, gf.s);
                int v = pixel_value(gf.lines[li], i);
                int vm1 = pixel_value(gf.lines[li], i - 1), vp1 = pixel_value(gf.lines[li], i + 1);
                if (v < 0) continue;
                auto it = bl.value.find(Triple{vm1, v, vp1});
                if (it == bl.value.end()) continue;
                ++checked;
                if (!eqfull(it->second, full12(cap.video[*oidx]))) ++diffs;
            }
        }
        checks.expect(checked > 0 && diffs == 0,
                       std::string("V4c precondition (SPECIFICATION.md 7/P4 contrast, ") + fname +
                       "'s closing restore band): the null case changes no graded pixel (NOT OBSERVED), checked=" +
                       std::to_string(checked) + " diffs=" + std::to_string(diffs));
    };
    if (f2.fc.bands.size() == 81)  grade_null_band(f2, bl2, 80, "F2");
    if (f3.fc.bands.size() == 81)  grade_null_band(f3, bl3, 80, "F3");
    if (f4.fc.bands.size() >= 34)  grade_null_band(f4, bl4, 33, "F4");

    // ---- P5.L (SuperHires half): some register 04-0F changes a pixel ------
    // SPECIFICATION.md 7/P5 item 4: "Any change counts, whether P4 grades it
    // PASS or FAIL" -- graded on the any-bit count, not P4's own
    // allowed-bit-only "changed" count, which the old check reused.
    checks.expect(p5l_shr_any > 0,
                  "P5.L CH4-appC-context.c7 p.301/94 (SuperHires half): a register outside COLOR00-03 changes a visible playfield pixel, any bit, changed=" +
                  std::to_string(p5l_shr_any) + " (allowed-bit-only count " + std::to_string(p5l_shr_changed) + ")");
    // Searched for a live input that turns this SAME comparator
    // (p5l_shr_any, the any-bit count over F2-F4's n=04-0F bands) green,
    // the way NMH_MUTATE=C5/V2 does for P2 (F2-F4's Q0 lines still populate
    // P2 when F1 drops out). None exists: every mutation that could reach
    // this comparator (V5, V4B, V4D put F2-F4 in hires) also removes its
    // population, since the P4 loop's own `if (!gf->ok) continue` then
    // skips every n=04-0F band, leaving p5l_shr_any at its baseline value
    // of 0 rather than reaching it through a different, live path.
    checks.record("P5.L (SuperHires half): no mutation in this package turns its own comparator green through a live input; every one that could reach it (V5, V4B, V4D) removes the population instead");

    // ---- P5.L (Hires half): F5's registers 04-0F change nothing, 00-03
    // invert exactly the value-n pixels in all twelve bits --------------
    bool p5l_hires_graded = false, p5l_hires_pass = false;
    if (f5.ok && f5.fc.bands.size() == 33) {
        std::vector<int> idxs; for (int i=0;i<24 && i<(int)f5.lines.size();++i) idxs.push_back(i);
        int lo = f5.fc.bands[32].line+1; for (int i=lo;i<=199 && i<(int)f5.lines.size();++i) idxs.push_back(i);
        Baseline bl5 = build_baseline(cap.video, f5.lines, f5.out_of, idxs, f5.mode, f5.s, f5.w, f5.phi,
                                       (long)f5.xL, (long)f5.xR);
        bool hires_high_clean = true, hires_low_ok = true;
        long total_checked = 0; // a real population guard, not a discarded count
        for (int n = 0; n < 16; ++n) {
            int band_idx = 16 + n; // second half of F5's bands
            if (band_idx >= (int)f5.fc.bands.size()) continue;
            const Band& band = f5.fc.bands[band_idx];
            long changed=0, checked=0;
            for (int li = band.line; li <= band.line+1 && li < (int)f5.lines.size(); ++li) {
                if (f5.out_of[li].empty()) continue;
                nmh::Span sp = f5.out_of[li].back();
                for (size_t x = f5.xL; x < f5.xR; ++x) {
                    auto oidx = pixel_sample(sp, f5.w, f5.phi, (long)x);
                    if (!oidx) continue;
                    size_t idx = *oidx;
                    int i = pixel_index(1,(int)x,f5.s);
                    int v = pixel_value(f5.lines[li], i);
                    int vm1 = pixel_value(f5.lines[li], i-1), vp1 = pixel_value(f5.lines[li], i+1);
                    if (v<0) continue;
                    auto it = bl5.value.find(Triple{vm1,v,vp1});
                    if (it==bl5.value.end()) continue;
                    Full12 base=it->second, obs=full12(cap.video[idx]);
                    checked++;
                    bool differs = !eqfull(base,obs);
                    if (n>=4 && differs) hires_high_clean = false;
                    if (n<4) {
                        if (v==n) { if (!(obs.r==(base.r^0xF)&&obs.g==(base.g^0xF)&&obs.b==(base.b^0xF))) hires_low_ok=false; }
                        else if (differs) hires_low_ok=false;
                    }
                    if (n>=4 && differs) ++changed;
                }
            }
            (void)changed;
            total_checked += checked;
        }
        checks.expect(total_checked > 0 && hires_high_clean,
                      "P5.L p.301/94 (Hires half): registers COLOR04-0F change no graded pixel in Hires mode, checked=" +
                      std::to_string(total_checked));
        checks.expect(total_checked > 0 && hires_low_ok,
                      "P5.L p.301/94 (Hires half): registers COLOR00-03 invert exactly the value-n pixels, all twelve bits, checked=" +
                      std::to_string(total_checked));
        p5l_hires_graded = true;
        p5l_hires_pass = total_checked > 0 && hires_high_clean && hires_low_ok;
    } else {
        checks.record("P5.L (Hires half): F5's register-invert bands not decoded as planned, not graded");
    }

    // ---- P1J: the full key claim, letters and dashes together
    // (SPECIFICATION.md 7/P1 item 8) -----------------------------------
    {
        static const char* kCompName[3] = {"R","G","B"};
        static const char* kSuffix[3]   = {"r","g","b"};
        static const int   kUpperBit[3] = {9,5,1}; // q-half upper-letter bit, per component
        for (int c = 0; c < 4; ++c) {
            char nhex[3]; std::snprintf(nhex, sizeof nhex, "%02X", c);
            for (int comp = 0; comp < 3; ++comp) {
                int bu = kUpperBit[comp], blo = bu - 1;
                int su = p4_state[c][bu], sl = p4_state[c][blo];
                int p1idx = c*3+comp;
                bool p1_ok = p1s.total[p1idx] > 0 && p1s.fail[p1idx] == 0;
                std::string label = "P1J." + std::to_string(c) + kCompName[comp] +
                    " APPC-bplkeykey.c" + std::to_string(c) + kSuffix[comp] +
                    " p.301: P1." + std::to_string(c) + kCompName[comp] + " (the letters) and P4.n" +
                    nhex + "b" + std::to_string(bu) + "/b" + std::to_string(blo) +
                    " (the dashes: no other register bit stores this component's own value) together";
                if (su == 0 || sl == 0) { checks.record(label + " -- not graded (P4 half not decoded)"); continue; }
                if (su == 1 || sl == 1) { checks.record(label + " -- not graded (P4 half NOT OBSERVABLE, L1)"); continue; }
                bool p4_ok = (su == 2) && (sl == 2);
                checks.expect(p1_ok && p4_ok, label);
            }
        }
    }

    // ---- F7-F10: P5.U, P6.1, P6.2 and, with P5.L, P7.2 and P7.3
    int p5l_state = (p5l_shr_any <= 0) ? 2 : (p5l_hires_graded ? (p5l_hires_pass ? 1 : 2) : 0);
    grade_sprite_frames(cap, checks, wp, f6, p5l_state);
}

void run_normal(const std::string& dir, Checks& checks) {
    nmh::Config cfg;
    Capture cap = run_case(dir, "prog_main", cfg);
    grade_capture(cap, checks, "");
}

// Every mutation's program (or Config) and the comparator it is run
// through -- always grade_capture(), unchanged. C1 selects OCS via a
// harness Config change (no program symbol exists for it); every other
// mutation selects a prog_<id>.bin built from its own MUT_<ID> wrapper.
void run_mutation(const std::string& dir, const std::string& mut, Checks& checks) {
    nmh::Config cfg; // ECS default; only C1 changes it
    std::string prog = "prog_" + [&]{ std::string s=mut; for (auto& c: s) c=(char)std::tolower((unsigned char)c); return s; }();
    if (mut == "C1") { cfg.chipset_config = 0x00; prog = "prog_main"; }
    else if (mut == "C5") prog = "prog_c5";
    else if (mut == "V5") prog = "prog_v4b"; // P5's SuperHires contrast reuses P4's V4b build (same data)
    Capture cap = run_case(dir, prog, cfg);
    checks.record("NMH_MUTATE=" + mut + " ran " + prog + ".bin, chipset_config=" +
                   std::to_string((int)cfg.chipset_config));
    grade_capture(cap, checks, mut);
}

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    const char* env = std::getenv("NMH_MUTATE");
    std::string mutate = env ? env : "";
    std::printf("NMH_MUTATE: %s\n", mutate.empty() ? "(none)" : mutate.c_str());

    Checks checks;

    if (std::getenv("NMH_DIAG")) {
    nmh::Config cfg;
    Capture c = run_case(dir, "prog_main", cfg);
    std::printf("DIAG loaded=%d released=%d done=%s log=%zu video=%zu\n",
                c.loaded, c.released, c.done ? std::to_string(*c.done).c_str() : "(none)",
                c.log.size(), c.video.size());
    if (c.fixture) std::printf("DIAG fixture=%04X vposr=%04X deniseid=%04X\n",
                                *c.fixture, c.vposr ? *c.vposr : 0, c.deniseid ? *c.deniseid : 0);
    for (int f = 0; f <= 10; ++f) {
        FrameCop fc = decode_frame(c.log, f);
        std::printf("DIAG frame %2d found=%d header_regs=%zu bands=%zu start=%llu end=%llu\n",
                    f, fc.found, fc.header.size(), fc.bands.size(),
                    (unsigned long long)fc.start_cycle, (unsigned long long)fc.end_cycle);
        if (fc.found && f <= 6) {
            for (size_t bi = 0; bi < fc.bands.size() && bi < 3; ++bi)
                std::printf("  band[%zu] line=%d beam=%02X moves=%zu\n", bi, fc.bands[bi].line,
                            fc.bands[bi].beam, fc.bands[bi].moves.size());
        }
    }
    // source lines in F1's own window (start..end cycle of F1's Copper pass)
    {
        FrameCop f1 = decode_frame(c.log, 1);
        FrameCop f2 = decode_frame(c.log, 2);
        uint64_t lo = f1.start_cycle, hi = f2.found ? f2.start_cycle : f1.end_cycle + 2000000;
        auto lines = find_source_lines(c.log, lo, hi, kPlane1Buf, kPlane2Buf);
        std::printf("DIAG F1 source lines: %zu\n", lines.size());
        for (size_t i = 0; i < lines.size() && i < 5; ++i)
            std::printf("  line[%zu] cyc=[%llu,%llu] p1=%zu p2=%zu pure_r=%d\n", i,
                        (unsigned long long)lines[i].cycle_lo, (unsigned long long)lines[i].cycle_hi,
                        lines[i].p1.size(), lines[i].p2.size(), lines[i].pure_r);
        auto outlines = nmh::find_lines(c.video);
        std::printf("DIAG output lines in video: %zu\n", outlines.size());
        auto frames = nmh::find_frames(c.video);
        std::printf("DIAG output frames in video: %zu\n", frames.size());
    }
    // extra: DMACON writes, CIA writes, program-range fetch spread, buffer writes
    {
        uint32_t dmacon_wa = word_addr(0xDFF096);
        int dmacon_writes = 0; uint16_t last_dmacon = 0; uint64_t last_dmacon_cycle=0;
        int cia_writes = 0;
        int zerobuf_writes = 0, plane1_writes = 0, plane2_writes = 0, spr0_writes=0;
        uint32_t prog_lo = word_addr(0x080400), prog_hi = word_addr(0x080400) + 4000;
        uint32_t min_pc = 0xFFFFFFFF, max_pc = 0;
        uint64_t last_cycle = 0;
        for (auto& a : c.log) {
            if (a.cycle > last_cycle) last_cycle = a.cycle;
            if (a.is_write && a.word_addr == dmacon_wa) { ++dmacon_writes; last_dmacon = a.data; last_dmacon_cycle=a.cycle; }
            if (a.is_write && a.word_addr >= word_addr(0xBFE000) && a.word_addr < word_addr(0xBFE400)) ++cia_writes;
            if (a.is_write && a.word_addr >= word_addr(kZeroBuf) && a.word_addr < word_addr(kZeroBuf)+kBufWords) ++zerobuf_writes;
            if (a.is_write && a.word_addr >= word_addr(kPlane1Buf) && a.word_addr < word_addr(kPlane1Buf)+kBufWords) ++plane1_writes;
            if (a.is_write && a.word_addr >= word_addr(kPlane2Buf) && a.word_addr < word_addr(kPlane2Buf)+kBufWords) ++plane2_writes;
            if (a.is_write && a.word_addr >= word_addr(kSpr0Struct) && a.word_addr < word_addr(kSpr0Struct)+124) ++spr0_writes;
            if (!a.is_write && a.word_addr >= prog_lo && a.word_addr < prog_hi) {
                if (a.word_addr < min_pc) min_pc = a.word_addr;
                if (a.word_addr > max_pc) max_pc = a.word_addr;
            }
        }
        std::printf("DIAG dmacon_writes=%d last=%04X@%llu cia_writes=%d zerobuf_w=%d plane1_w=%d plane2_w=%d spr0_w=%d\n",
                    dmacon_writes, last_dmacon, (unsigned long long)last_dmacon_cycle, cia_writes,
                    zerobuf_writes, plane1_writes, plane2_writes, spr0_writes);
        std::printf("DIAG program fetch range: min=%08X max=%08X (org=%08X) last_log_cycle=%llu\n",
                    min_pc*2, max_pc*2, 0x080400, (unsigned long long)last_cycle);
        // last 20 program-range fetches, in order
        std::vector<uint32_t> tail;
        for (auto& a : c.log) if (!a.is_write && a.word_addr>=prog_lo && a.word_addr<prog_hi) tail.push_back(a.word_addr*2);
        std::printf("DIAG last program fetches:");
        for (size_t i = tail.size() > 20 ? tail.size()-20 : 0; i < tail.size(); ++i) std::printf(" %06X", tail[i]);
        std::printf("\n");
    }
    {
        FrameLines fl5 = decode_frame_lines(c, 5, kPlane1Buf, kPlane2Buf);
        std::printf("DIAG F5 found=%d lines=%zu\n", fl5.fc.found, fl5.lines.size());
        for (size_t li = 8; li < 14 && li < fl5.lines.size(); ++li) {
            std::printf("DIAG F5 line[%zu] pure_r=%d p1=%zu out_spans=%zu\n", li, fl5.lines[li].pure_r,
                        fl5.lines[li].p1.size(), fl5.out_of[li].size());
            if (fl5.out_of[li].empty()) continue;
            nmh::Span sp = fl5.out_of[li].back();
            std::printf("  span=[%zu,%zu) width=%zu\n", sp.begin, sp.end, sp.end-sp.begin);
            // run-length list across the WHOLE span
            std::vector<std::pair<int,int>> runs; // (value-as-int, length)
            size_t i = sp.begin;
            while (i < sp.end) {
                size_t j = i+1;
                while (j < sp.end && eqfull(full12(c.video[j]), full12(c.video[i]))) ++j;
                runs.push_back({(c.video[i].r<<8)|(c.video[i].g<<4)|c.video[i].b, (int)(j-i)});
                i = j;
            }
            std::printf("  %zu runs, lengths:", runs.size());
            for (size_t k = 0; k < runs.size() && k < 40; ++k) std::printf(" %03X:%d", runs[k].first, runs[k].second);
            std::printf("\n");
        }
    }
    {
        FrameLines fl1 = decode_frame_lines(c, 1, kPlane1Buf, kPlane2Buf);
        std::printf("DIAG F1 lines=%zu\n", fl1.lines.size());
        for (size_t li : {size_t(0), size_t(10), size_t(11)}) {
            if (li >= fl1.lines.size()) continue;
            std::printf("DIAG F1 line[%zu] pure_r=%d p1=%zu out_spans=%zu\n", li, fl1.lines[li].pure_r,
                        fl1.lines[li].p1.size(), fl1.out_of[li].size());
            if (fl1.out_of[li].empty()) continue;
            nmh::Span sp = fl1.out_of[li].back();
            Run12 r = find_constant_run(c.video, sp, 0);
            std::printf("  span=[%zu,%zu) run.ok=%d run=[%zu,%zu) len=%zd\n", sp.begin, sp.end, r.ok,
                        r.lo, r.hi, r.ok ? (ptrdiff_t)(r.hi-r.lo) : -1);
            std::vector<std::pair<int,int>> runs;
            size_t ii = sp.begin;
            while (ii < sp.end) {
                size_t jj = ii+1;
                while (jj < sp.end && eq2(top2(c.video[jj]), top2(c.video[ii]))) ++jj;
                runs.push_back({(top2(c.video[ii]).r<<4)|(top2(c.video[ii]).g<<2)|top2(c.video[ii]).b, (int)(jj-ii)});
                ii = jj;
            }
            std::printf("  %zu top2-runs:", runs.size());
            for (size_t k=0;k<runs.size()&&k<50;++k) std::printf(" %02X:%d", runs[k].first, runs[k].second);
            std::printf("\n");
        }
    }
    checks.record("diagnostic run, no assertions graded");
    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
    }

    static const std::set<std::string> kKnownMuts = {
        "V1","V2","V3A","V3B","V3C","V4A","V4B","V4D","V5","V5B",
        "C1","C2","C3","C4","C5","C6","C7","C8","C9","FIX",
        "V5C","V6A","V6B","V6C","V6D","V6E","V6L","V6Q","V6S",
        "C4B","C4C","C7B","C10","C11","C11G","RUNENDS",
    };
    if (mutate.empty()) {
        run_normal(dir, checks);
    } else if (kKnownMuts.count(mutate)) {
        run_mutation(dir, mutate, checks);
    } else {
        checks.record("unrecognised or not-yet-implemented NMH_MUTATE value: " + mutate);
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
