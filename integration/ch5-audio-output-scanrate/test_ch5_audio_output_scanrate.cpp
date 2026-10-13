// test_ch5_audio_output_scanrate.cpp -- see SPECIFICATION.md.
//
// P1-P4: the audio program (prog_audio.s) plays tables T1, T2, T3 in
// sequence; table content is poked directly into chip RAM (the same
// "direct memory preload" mechanism load_program() itself uses), so one
// capture's ram_log()+audio() trace serves all four propositions and
// their shared preconditions A1-A6. P3B: prog_t2s.s. P5: prog_adkcon.s.
// P6/F1/F3: prog_diskrate.s.
//
// Measured, per core and per config: channel 0's own byte sequence
// appears on whichever of audio_left/audio_right varies (see the "audio
// output side" MEASURED line below); the manual's pages disagree on
// which named jack carries channel 0 (p.137 vs. Appendix E's pin list,
// p.326). SPECIFICATION.md 2.5 observes whichever side carries channel
// 0 and does not grade which named jack that is; this test picks that
// side at runtime (audio_side_is_right()), not by a fixed choice.
//
// Every table's analysis window starts on the first output transition after
// the table's first restart (t_2, SPECIFICATION.md 2.4) and ends in the middle
// of level number N x K, with the level taken from the programmed period
// (SPECIFICATION.md 2.5). The first pass, the reads before t_2, is
// printed per table as a MEASURED line that names NM-HRM-084 and is not
// graded.
//
// NMH_MUTATE selects one mutated program or table in place of its normal
// counterpart; unset, every section runs its normal binary and table, and
// the summary counts the fixture, preconditions and assertions only.
// Selectors: P1 P1LAST P2 P3 P4 A1 A2 A2T A3 A4 A5 A5SYS A6 P3B P5 C1 C1DUP FASTF0 FASTS1 B2 B3 F1
// FIXCONST FIXDATA FIXWRITE NODONE.
//
// Each MUTATION record states the measured result of its selector on both
// cores, fx68k (with microrom.mem and nanorom.mem in its object directory)
// and TG68K, in every configuration the line is graded in. Every selector
// reached DONE on both cores, except NODONE, whose program never writes
// the done marker. A line that goes red or is not graded under a selector
// is named in its record, on both cores.
//
// Every mutation branch below runs the SAME comparator its normal
// counterpart does; only the input (stimulus or table data) differs.
// None of them invert a verdict before printing: a reproduced defect
// prints FAIL through the clause it targets, not an inverted PASS.
//
// MUTATION P1 stimulus red NMH_MUTATE=P1: T1's last word $6070 -> $7060
//   measured: fx68k and TG68K: P1 red (ECS/PAL, OCS/PAL); P2 not graded (RECORDED, "P1 is red"); no other line changes
//   fx68k 44 passed, 2 failed; TG68K 44 passed, 2 failed
// MUTATION P1 stimulus red NMH_MUTATE=P1LAST: T1 as normal; after the DMA starts the program waits a measured number of
//   lines and writes one byte of T1 with the CPU, the byte the window's last level comes from, between the DMA's last two
//   reads of that word (a first run of the same program writes the byte's own value and measures the byte and the
//   instant; the changed byte is $7F)
//   measured: fx68k and TG68K: P1 red (ECS/PAL, OCS/PAL); P2 not graded (RECORDED, "P1 is red"); FIXTURE-DATA T1 green
//   (the one CPU write is the fixture's own expected write count); the MEASURED line names the one entry of L that
//   differs from the level one pass earlier, the last (index 63); no other line changes
//   fx68k 44 passed, 2 failed; TG68K 44 passed, 2 failed
// MUTATION P2 stimulus red NMH_MUTATE=P2: the bytes of every word of T1 swapped ($1000, $3020, $5040, $7060)
//   measured: fx68k and TG68K: P2 red (ECS/PAL, OCS/PAL), P1 green; no other line changes
//   fx68k 46 passed, 2 failed; TG68K 46 passed, 2 failed
// MUTATION P3 stimulus red NMH_MUTATE=P3: every byte of T2 with bit 7 flipped
//   measured: fx68k and TG68K: P3 red (ECS/PAL, OCS/PAL); no other line changes
//   fx68k 46 passed, 2 failed; TG68K 46 passed, 2 failed
// MUTATION P4 stimulus red NMH_MUTATE=P4: T3's word for code +5 (k=133) $0505 -> $0707
//   measured: fx68k and TG68K: P4 red (ECS/PAL, OCS/PAL); no other line changes
//   fx68k 46 passed, 2 failed; TG68K 46 passed, 2 failed
// MUTATION A1 stimulus red NMH_MUTATE=A1: for T1, DMACON $8200 instead of $8201 (AUD0EN not set)
//   measured: fx68k and TG68K: A1 for T1 red (ECS/PAL, OCS/PAL); FIXTURE-DATA T1 also red, no read of T1 at all;
//   T1's A3, A4, A5, P1 and P2 not graded (RECORDED); A1 for T2 and T3 and every other line unchanged
//   fx68k 34 passed, 4 failed; TG68K 34 passed, 4 failed
// MUTATION A2 stimulus red NMH_MUTATE=A2: ADKCON $8001 (USE0V1) set before the ADKCONR read of prog_audio
//   measured: fx68k and TG68K: A2 red (ECS/PAL, OCS/PAL); A3, A4, A5 and the propositions of every table not
//   graded (RECORDED, "precondition A2 failed or not graded"); A1 of every table unchanged
//   fx68k 20 passed, 2 failed; TG68K 20 passed, 2 failed
// MUTATION A2 stimulus red NMH_MUTATE=A2T: ADKCON $8001 (USE0V1) set before the ADKCONR read of prog_t2s
//   measured: fx68k and TG68K: A2 for T2S red (ECS/PAL, OCS/PAL); T2S's A3-A5 and P3B not graded (RECORDED,
//   "precondition A2 failed"); A1 and A6 for T2S and every other line unchanged
//   fx68k 12 passed, 2 failed; TG68K 12 passed, 2 failed
// MUTATION A3 stimulus red NMH_MUTATE=A3: T1's AUD0PER = 129 instead of 128 (T2 and T3 keep 128)
//   measured: fx68k and TG68K: A3 for T1 red (ECS/PAL, OCS/PAL; median run 1032 against the programmed held level
//   of 1024); T1's A4, A5, P1 and P2 not graded (RECORDED, "precondition A3 failed"); A1 and A6 for T1, T2 and T3
//   unchanged
//   fx68k 38 passed, 2 failed; TG68K 38 passed, 2 failed
// MUTATION A4 stimulus red NMH_MUTATE=A4: T1's AUD0VOL set to 0 seven lines after its DMA starts, inside the window
//   measured: fx68k and TG68K: A4 for T1 red (ECS/PAL, OCS/PAL; L has 5 entries on fx68k and 4 on TG68K, the output
//   flat from there on); A3 for T1 green (median run 1024); T1's P1 and P2 not graded (RECORDED, "precondition A4
//   failed"); A1 for T1, T2 and T3 unchanged
//   fx68k 40 passed, 2 failed; TG68K 40 passed, 2 failed
// MUTATION A5 stimulus red NMH_MUTATE=A5: T2's word 2 changed from $0101 to $0000 (repeats word 1's code, adjacent
//   within a pass)
//   measured: fx68k and TG68K: A5 for T2 red (ECS/PAL, OCS/PAL; 24 found where 32 is the programmed population: one
//   level lost per pass, 8 passes); P3 not graded (RECORDED, "precondition A5 failed"); no other line changes
//   fx68k 44 passed, 2 failed; TG68K 44 passed, 2 failed
// MUTATION A3 stimulus red NMH_MUTATE=A5SYS: T1's words $0000, $2020, $4040, $6060 (every pair of adjacent codes equal:
//   levels twice as long, half as many)
//   measured: fx68k and TG68K: A3 for T1 red (ECS/PAL, OCS/PAL; median run 2048 against the programmed held level of
//   1024); T1's A4, A5, P1 and P2 not graded (RECORDED, "precondition A3 failed"); no other line changes
//   fx68k 38 passed, 2 failed; TG68K 38 passed, 2 failed
// MUTATION A6 stimulus red NMH_MUTATE=A6: before the tables start, VTOTAL=524/HTOTAL=113/BEAMCON0=$00A0 in prog_audio and in
//   prog_t2s (ECS only, 2.4)
//   measured: fx68k and TG68K: A6 red for prog_audio and for T2S (ECS/PAL only; OCS/PAL has no such registers and stays
//   green); behind them not graded (RECORDED, "precondition A6 failed"): A1, A3-A5 and P1-P4 of T1, T2, T3 and
//   A1-A5 and P3B of T2S in ECS/PAL; every other line unchanged
//   fx68k 46 passed, 2 failed; TG68K 46 passed, 2 failed
// MUTATION P3B stimulus red NMH_MUTATE=P3B: T2S's two words byte-swapped ($8001,$7F00 -> $0180,$007F)
//   measured: fx68k and TG68K: P3B red (ECS/PAL, OCS/PAL); no other line changes (T2S's own program is
//   independent of prog_audio.s)
//   fx68k 20 passed, 2 failed; TG68K 20 passed, 2 failed
// MUTATION FIXTURE stimulus red NMH_MUTATE=NODONE: prog_audio is replaced by prog_nodone, which never writes the
//   done marker (the T2S, adkcon and disk sections are not run under this selector)
//   measured: fx68k and TG68K: FIXTURE red for prog_audio (ECS/PAL, OCS/PAL), the audio lines not graded (RECORDED,
//   "audio fixture did not complete"); 0 passed, 2 failed on both cores
// MUTATION FIXTURE-CONST stimulus red NMH_MUTATE=FIXCONST: each of the four programs' own KFIX store becomes $DEAD instead of $BEEF
//   measured: fx68k and TG68K: all nine FIXTURE-CONST lines red (prog_audio x2, prog_t2s x2, prog_adkcon x2,
//   prog_diskrate x3); behind them not graded (RECORDED): A2 and A2 for T2S, then A3, A4, A5 and P1-P4 of T1, T2 and
//   T3 and A3-A5 and P3B of T2S (ECS/PAL, OCS/PAL), C1 and P5 (x2), B1 and B2 of both disk runs (x3); A6 of both
//   programs, which reads no result slot, stays graded and green; every other line unchanged
//   fx68k 63 passed, 9 failed; TG68K 63 passed, 9 failed
// MUTATION FIXTURE-DATA stimulus red NMH_MUTATE=FIXDATA: one word of every poked table other than the content the
//   check expects (T1 word 2 $2031, T2 word 2 $0100, T3 word 11 flipped in bit 0, T2S word 2 $7F01) and both disk
//   buffers poked with $5555 instead of $AAAA
//   measured: fx68k and TG68K: FIXTURE-DATA red for T1, T2, T3 (x2 configs), T2S (x2) and both buffers (x3), 14
//   lines; behind the table lines not graded (RECORDED): A4, A5 and the propositions of T1, T2, T3 and T2S; A1, A2, A3
//   and A6 stay graded and green; the buffer lines gate nothing; every other line unchanged
//   fx68k 78 passed, 14 failed; TG68K 78 passed, 14 failed
// MUTATION FIXTURE-WRITE stimulus red NMH_MUTATE=FIXWRITE: the ADKCONR store into the A2 slot (prog_audio and prog_t2s) and
//   run F's store of its ADKCONR read into the B1 slot (prog_diskrate) are left out
//   measured: fx68k and TG68K: FIXTURE-WRITE red for the A2 slot of prog_audio (ECS/PAL, OCS/PAL), of prog_t2s
//   (ECS/PAL, OCS/PAL) and the B1-for-run-F slot (ECS/PAL, OCS/PAL, ECS/NTSC); behind them not graded (RECORDED): A2
//   (prog_audio and T2S) and, through it, A3-A5 and the propositions of T1, T2 and T3 and A3-A5 and P3B of T2S (x2),
//   B1 for run F (x3); every other line unchanged
//   fx68k 70 passed, 7 failed; TG68K 70 passed, 7 failed
// MUTATION P5 stimulus red NMH_MUTATE=P5: step 2 writes $8200 instead of $8100
//   measured: fx68k and TG68K: P5 red (ECS/PAL, OCS/PAL); no other line changes
//   fx68k 6 passed, 2 failed; TG68K 6 passed, 2 failed
// MUTATION C1 stimulus red NMH_MUTATE=C1: the store of R2 (step 3's readback) left out
//   measured: fx68k and TG68K: C1 red (ECS/PAL, OCS/PAL); P5 not graded (RECORDED, "precondition C1 failed"); the
//   fixture lines, which no longer depend on the slots, stay green
//   fx68k 4 passed, 2 failed; TG68K 4 passed, 2 failed
// MUTATION C1 stimulus red NMH_MUTATE=C1DUP: the store of R2 made twice
//   measured: fx68k and TG68K: C1 red (ECS/PAL, OCS/PAL; two writes to R2); P5 not graded (RECORDED, "precondition C1
//   failed"); the fixture lines stay green
//   fx68k 4 passed, 2 failed; TG68K 4 passed, 2 failed
// MUTATION B1 stimulus red NMH_MUTATE=FASTF0: run F's ADKCON write becomes $0100 instead of $8100
//   measured: fx68k and TG68K: B1 for run F red (ECS/PAL, OCS/PAL, ECS/NTSC); no other line changes
//   fx68k 45 passed, 3 failed; TG68K 45 passed, 3 failed
// MUTATION B1 stimulus red NMH_MUTATE=FASTS1: run S's ADKCON write becomes $8100 instead of $0100
//   measured: fx68k and TG68K: B1 for run S red (ECS/PAL, OCS/PAL, ECS/NTSC); no other line changes
//   fx68k 45 passed, 3 failed; TG68K 45 passed, 3 failed
// MUTATION B2 stimulus red NMH_MUTATE=B2: run F's DMACON write becomes $8010 instead of $8210 (DMAEN, bit 9,
//   clear; p.247, p.248, p.278 agree DMAON then reads 0)
//   measured: fx68k and TG68K: B2 for run F red (ECS/PAL, OCS/PAL, ECS/NTSC); B3 for run F and FIXTURE-DATA
//   buffer-A also red (0 reads of buffer A at all), the necessary side effect of clearing the master DMA enable;
//   B4 for run F and G1-G3 not graded (RECORDED); every other line unchanged
//   fx68k 36 passed, 9 failed; TG68K 36 passed, 9 failed
// MUTATION B3 stimulus red NMH_MUTATE=B3: run F's LENGTH shrunk to 100 (DSKLEN=$C064) in steps 5 and 6
//   measured: fx68k and TG68K: B3 for run F red ("100 found" against the required 529; ECS/PAL, OCS/PAL,
//   ECS/NTSC); B4 for run F and G1-G3 not graded (RECORDED); every other line unchanged
//   fx68k 42 passed, 3 failed; TG68K 42 passed, 3 failed
// MUTATION B4 stimulus red NMH_MUTATE=F1: before run F, VTOTAL=524/HTOTAL=113/BEAMCON0 rewritten (2.4, ECS only;
//   NTSC uses BEAMCON0=$0080, PAL uses $00A0), measured inside both run F's and run S's own P6 interval (read k=16
//   to k=528 of each buffer)
//   measured: fx68k and TG68K: B4 red for run F and run S (114.00 CCK; ECS/PAL, ECS/NTSC); disk write DMA's own
//   rate also halves under this stimulus (D_F, D_S: 310520 -> 155936 cycles, both runs, both cores)
//   fx68k 28 passed, 4 failed; TG68K 28 passed, 4 failed
// G1, G2 and G3 are RECORDED declines (SPECIFICATION.md 1.2, P6 3-4): no page ties the
//   Chip-memory read rate they measure to the bit-cell time of pp.249/265, so they are not
//   graded propositions and carry no mutation of their own.

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>
#include <iostream>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

// ---- the assertion call form (BRIEF.md) --------------------------------
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

// ---- nanomig.i, restated -------------------------------------------------
constexpr uint32_t kResults   = 0x0C0000;
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;

// FIXTURE: each program's own known-constant slot (KFIX in its .s source),
// asserted first, before anything measured is stored or read (BRIEF.md).
constexpr uint16_t kFixValue     = 0xBEEF;
constexpr uint16_t kFixMutValue  = 0xDEAD;
constexpr uint32_t kFixAudio     = kResults + 0x20;   // prog_audio.s, prog_t2s.s
constexpr uint32_t kFixAdkcon    = kResults + 10;     // prog_adkcon.s
constexpr uint32_t kFixDisk      = kResults + 12;     // prog_diskrate.s

constexpr uint64_t kBootBudget      = 6'000'000;
constexpr uint64_t kAudioRunBudget  = 8'000'000;
constexpr uint64_t kAdkconRunBudget = 2'000'000;
constexpr uint64_t kDiskRunBudget   = 20'000'000;

// clk_sys cycles per color clock: 8, taken from the design (nanomig.i
// declares no such ratio). Guarded by B4, measured inside the P6
// interval it is used in (SPECIFICATION.md 2.4, section 5): every line
// there comes out at 227.00 color clocks, both cores, all three
// configurations.
constexpr uint64_t kCck = 8;
constexpr double   kUsPerCckPal  = 0.281937;   // p.141
constexpr double   kUsPerCckNtsc = 0.279365;   // p.141

constexpr uint32_t T1_ADDR = 0x002000;
constexpr uint32_t T2_ADDR = 0x002100;
constexpr uint32_t T3_ADDR = 0x002200;
constexpr uint16_t kDiskWord = 0xAAAA;   // p.249/p.250: raw MFM of data zeros
constexpr uint32_t DISKBUF_A = 0x003000;
constexpr uint32_t DISKBUF_B = 0x004000;

constexpr int kNT1 = 8, kNT2 = 8, kNT3 = 2, kNT2S = 8;   // N (2.6 table)
// AUD0PER of prog_audio.s and prog_t2s.s, in color clocks (SPECIFICATION.md
// 2.6, "Period P"); restated here the way nanomig.i is.
constexpr uint32_t kAudPer = 128;
// prog_audio.s under MUT_P1LAST: three parameter words that the test pokes
// before boot(), and the store whose port cycle marks the start of the wait.
constexpr uint32_t kPlLines = kResults + 0x30;
constexpr uint32_t kPlAddr  = kResults + 0x32;
constexpr uint32_t kPlVal   = kResults + 0x36;
constexpr uint32_t kPlMark  = kResults + 0x40;

// A table under test: N passes of K held levels, each level u bytes long
// (SPECIFICATION.md 2.6 table).
struct TableSpec { const char* name; uint32_t addr; int words, N, K, unit; };
constexpr uint32_t T2S_ADDR = 0x002000;   // P3B (prog_t2s.s, its own program): 2 words

// ---- table content (P1-P4's own data mutations live here; see
// prog_audio.s's header) --------------------------------------------------
// The poked content of a table under the selector `mv`. NMH_MUTATE=FIXDATA
// pokes one word of every table other than the content the FIXTURE-DATA
// check expects (the expected content is always table(mv == FIXDATA ? "" : mv)).
std::vector<uint16_t> t1_table(const std::string& mv) {
    if (mv == "P1") return {0x0010, 0x2030, 0x4050, 0x7060};
    // A5's systematic contrast: every pair of adjacent codes equal, so every
    // level lasts two periods and there are half as many.
    if (mv == "A5SYS") return {0x0000, 0x2020, 0x4040, 0x6060};
    if (mv == "P2") return {0x1000, 0x3020, 0x5040, 0x7060};
    if (mv == "FIXDATA") return {0x0010, 0x2031, 0x4050, 0x6070};
    return {0x0010, 0x2030, 0x4050, 0x6070};
}
std::vector<uint16_t> t2_table(const std::string& mv) {
    if (mv == "P3") return {0x8080, 0x8181, 0xFFFF, 0x0000};
    // A5's own contrasting case: word 2 duplicates word 1's code (0101 ->
    // 0000), so two adjacent held levels within a pass collapse into one
    // run -- the population A5 (2.7) grades comes up short.
    if (mv == "A5") return {0x0000, 0x0000, 0x7F7F, 0x8080};
    if (mv == "FIXDATA") return {0x0000, 0x0100, 0x7F7F, 0x8080};
    return {0x0000, 0x0101, 0x7F7F, 0x8080};
}
std::vector<uint16_t> t3_table(const std::string& mv) {
    std::vector<uint16_t> t(256);
    for (int k = 0; k < 256; ++k) {
        uint8_t b = (uint8_t)((k + 0x80) & 0xFF);
        t[k] = (uint16_t)((b << 8) | b);
    }
    if (mv == "P4") t[133] = 0x0707;   // code +5 (k=133): $0505 -> $0707
    if (mv == "FIXDATA") t[10] ^= 0x0001;
    return t;
}
// T2S (P3B): two words whose two bytes differ, so a byte-position swap
// changes what is output, unlike T2 (2.6: T2's equal-byte words "do not
// depend on byte order"). Both words carry the pair's own sign extremes
// ($80 most negative, $7F most positive, p.265) so that dropping either
// byte position would change which extreme is seen where.
std::vector<uint16_t> t2s_table(const std::string& mv) {
    if (mv == "P3B") return {0x0180, 0x007F};   // byte-swapped: P3B's own contrasting case
    if (mv == "FIXDATA") return {0x8001, 0x7F01};
    return {0x8001, 0x7F00};
}

std::vector<int> c_up()  { return {0,16,32,48,64,80,96,112}; }
std::vector<int> c_low() { return {16,0,48,32,80,64,112,96}; }
std::vector<int> c2()    { return {0,1,127,-128}; }
std::vector<int> c3() {
    std::vector<int> v(256);
    for (int k = 0; k < 256; ++k) v[k] = k - 128;
    return v;
}
// T2S's own expected code sequence, upper byte of each word first (P2's
// own established order): word1=$8001 -> -128,1; word2=$7F00 -> 127,0.
std::vector<int> c2s() { return {-128, 1, 127, 0}; }

// ---- Config helpers -------------------------------------------------------
nmh::Config cfg_ecs_pal()  { nmh::Config c; c.chipset_config = 0x08; return c; }
nmh::Config cfg_ocs_pal()  { nmh::Config c; c.chipset_config = 0x00; return c; }
nmh::Config cfg_ecs_ntsc() { nmh::Config c; c.chipset_config = 0x0A; return c; }

void poke_table(nmh::NanomigHarness& h, uint32_t byte_addr, const std::vector<uint16_t>& words) {
    for (size_t i = 0; i < words.size(); ++i) h.poke_word(byte_addr / 2 + (uint32_t)i, words[i]);
}
void poke_fill(nmh::NanomigHarness& h, uint32_t byte_addr, uint16_t value, uint32_t count) {
    for (uint32_t i = 0; i < count; ++i) h.poke_word(byte_addr / 2 + i, value);
}

// ---- F2: audio pass boundaries (2.4) -------------------------------------
struct F2Result {
    bool anchor_ok = false;
    std::vector<uint64_t> pass_starts;   // t_1, t_2, ... (cycle of each read of the table's first word)
    size_t first_pass_reads = 0;         // reads of the table's words before t_2: the first pass
    std::string diag;   // set only when the anchor breaks, for MEASURED reporting
};

// F2 (2.4): from t_2, the table's first restart, on, every read of a word
// is the next word after the one read before it, and every later read of
// the first word (t_3, t_4, ...) is immediately preceded by a read of the
// table's last word. What precedes t_2 is the first pass: it is counted
// (first_pass_reads) and never examined, so no verdict of this package
// depends on its length (NM-HRM-084 grades it).
F2Result f2_check(const std::vector<nmh::RamAccess>& log, uint32_t lo_word, uint32_t hi_word) {
    F2Result r;
    uint32_t prev_addr = 0;
    for (const auto& a : log) {
        if (a.is_write || a.word_addr < lo_word || a.word_addr > hi_word) continue;
        if (r.pass_starts.size() < 2) {
            if (a.word_addr == lo_word) {
                r.pass_starts.push_back(a.cycle);
                if (r.pass_starts.size() == 2) { prev_addr = a.word_addr; continue; }   // t_2: the anchor starts here
            }
            ++r.first_pass_reads;   // t_1 and the reads between t_1 and t_2
            continue;
        }
        if (a.word_addr == lo_word) {
            if (prev_addr != hi_word) {
                char buf[200];
                std::snprintf(buf, sizeof buf,
                    "t_%zu (word_addr=0x%X) at cycle %llu is preceded by 0x%X, not the "
                    "table's last word (0x%X)",
                    r.pass_starts.size() + 1, a.word_addr, (unsigned long long)a.cycle,
                    prev_addr, hi_word);
                r.diag = buf;
                r.anchor_ok = false;
                return r;
            }
            r.pass_starts.push_back(a.cycle);
        } else if (a.word_addr != prev_addr + 1) {
            char buf[160];
            std::snprintf(buf, sizeof buf,
                "expected word_addr=0x%X, got 0x%X (data=0x%04X) at cycle %llu, after %zu reads of lo",
                prev_addr + 1, a.word_addr, a.data, (unsigned long long)a.cycle, r.pass_starts.size());
            r.diag = buf;
            r.anchor_ok = false;
            return r;
        }
        prev_addr = a.word_addr;
    }
    r.anchor_ok = r.pass_starts.size() >= 2;
    return r;
}

// The first pass, printed as a MEASURED line: not graded, not counted in
// passed or failed; NM-HRM-084 grades it in hrm/ch5-audio-dma-first-pass.
std::string first_pass_text(const std::string& table, const std::string& cfg, const F2Result& f2, int words) {
    return "first pass of " + table + " " + cfg + " (the reads before t_2; not graded here, NM-HRM-084 grades "
        "it): " + std::to_string(f2.first_pass_reads) + " reads of " + std::to_string(words) +
        " words before t_2 (" + std::to_string(f2.pass_starts.size()) + " reads of the first word in all)";
}

// ---- LEVELS(W) and FIT(L, C) (2.5) ---------------------------------------
// nanomig_harness.h documents only the width of audio_left/audio_right
// ("15 bits significant", AudioSample) -- not their sign or scale. This
// reads the raw magnitude the port carries (bits 14-0), with no
// assumption about which bit, if any, is a sign bit: run detection below
// only needs exact equality, which is representation-independent, and
// FIT(L, C) recovers a continuous, comparable sequence from these raw
// magnitudes through unwrap15() (below), which uses only the domain size
// the harness contract states, not a chosen sign convention.
int32_t raw_sample(uint16_t raw) { return raw & 0x7FFF; }

struct RawRun { int32_t value; uint64_t len; };

std::vector<RawRun> raw_runs(const std::vector<nmh::AudioSample>& audio, uint64_t begin, uint64_t end,
                             bool use_right) {
    std::vector<RawRun> runs;
    for (uint64_t i = begin; i < end && i < audio.size(); ++i) {
        int32_t v = raw_sample(use_right ? audio[i].right : audio[i].left);
        if (!runs.empty() && runs.back().value == v) ++runs.back().len;
        else runs.push_back({v, 1});
    }
    return runs;
}

// unwrap15: turns a time-ordered sequence of raw 15-bit magnitudes
// (0..32767, the domain nanomig_harness.h's "15 bits significant"
// states) into a continuous sequence, by adding or subtracting whole
// multiples of that domain so that each step from its predecessor is the
// smallest one available -- a standard modular/relative unwrap. It uses
// only the domain's size, never a choice of which raw bit is a sign bit,
// so it recovers the same linear relationship (up to the arbitrary
// additive constant FIT(L, C)'s own `a` already absorbs) whichever way
// the port's own scale happens to be offset.
std::vector<double> unwrap15(const std::vector<int32_t>& raw) {
    constexpr double kDomain = 32768.0;
    std::vector<double> out;
    out.reserve(raw.size());
    if (raw.empty()) return out;
    double acc = (double)raw[0];
    out.push_back(acc);
    for (size_t i = 1; i < raw.size(); ++i) {
        double step = (double)raw[i] - (double)raw[i - 1];
        step -= kDomain * std::round(step / kDomain);
        acc += step;
        out.push_back(acc);
    }
    return out;
}

// SPECIFICATION.md 2.5: the observed output is whichever audio output
// carries channel 0, not a fixed choice -- p.137 and Appendix E's pin
// list (p.326) disagree on which named jack that is. Measured per
// capture: audio_right stays flat throughout every run on this RTL, so
// this returns false (use audio_left) whenever no variation is seen on
// either side.
bool audio_side_is_right(const std::vector<nmh::AudioSample>& audio) {
    std::optional<int32_t> first;
    for (auto& s : audio) {
        int32_t v = raw_sample(s.right);
        if (!first) first = v;
        else if (v != *first) return true;
    }
    return false;
}

// A C1-style port-write check (SPECIFICATION.md section 5): every result
// slot a proposition reads must show exactly one CPU write at the
// Chip-memory port, so an unwritten, zero-initialised slot can never
// pass silently.
bool has_one_write(const std::vector<nmh::RamAccess>& log, uint32_t word_addr) {
    int n = 0;
    for (auto& a : log) if (a.is_write && a.word_addr == word_addr) ++n;
    return n == 1;
}

// FIXTURE-DATA: the directly poked audio/disk tables asserted on the RAM
// port before use, not merely trusted from poke_word(). True when every
// read of any word of the table carries exactly the data the word holds at
// that moment -- the poked content, as changed by the CPU writes to the
// table that the port shows before the read -- the number of such writes is
// `expected_writes` (the stimulus's own), and the port shows at least one
// read of every word (every_word) or at least one read (not every_word, for
// a buffer whose length a selector shortens).
bool all_reads_match(const std::vector<nmh::RamAccess>& log, uint32_t base_word,
                     const std::vector<uint16_t>& expected, bool every_word = true,
                     size_t expected_writes = 0) {
    std::vector<uint16_t> cur = expected;
    std::vector<bool> seen(expected.size(), false);
    size_t writes = 0;
    for (auto& a : log) {
        if (a.word_addr < base_word || a.word_addr >= base_word + expected.size()) continue;
        size_t i = a.word_addr - base_word;
        if (a.is_write) { cur[i] = a.data; ++writes; continue; }
        if (a.data != cur[i]) return false;
        seen[i] = true;
    }
    if (writes != expected_writes) return false;
    if (every_word) return std::find(seen.begin(), seen.end(), false) == seen.end();
    return std::find(seen.begin(), seen.end(), true) != seen.end();
}

double median_run_len(const std::vector<RawRun>& runs) {
    if (runs.empty()) return 0.0;
    std::vector<uint64_t> lens;
    for (auto& r : runs) lens.push_back(r.len);
    std::sort(lens.begin(), lens.end());
    return (double)lens[lens.size() / 2];
}

// The window's start (SPECIFICATION.md 2.5): the first output transition at
// or after the DMA read t_2, that is the first sample at or after `begin`
// whose value differs from the sample before it, searched up to `end`. The
// run that starts there is complete by construction. std::nullopt when the
// output does not change inside the window (a flat output).
std::optional<uint64_t> first_transition(const std::vector<nmh::AudioSample>& audio, uint64_t begin,
                                         uint64_t end, bool use_right) {
    for (uint64_t i = std::max<uint64_t>(begin, 1); i < end && i < audio.size(); ++i)
        if (raw_sample(use_right ? audio[i].right : audio[i].left) !=
            raw_sample(use_right ? audio[i - 1].right : audio[i - 1].left))
            return i;
    return std::nullopt;
}

// The analysis window of a table (SPECIFICATION.md 2.5, step 1): it starts
// on the first output transition at or after t_2 and ends in the middle of
// level number N x K, with the level taken from the programmed period (u x P
// x kCck samples), not from the output. Every run in it is kept, the last
// one (cut by e) included.
struct Window {
    bool have = false;
    uint64_t s = 0, e = 0, level = 0;
    std::vector<RawRun> runs;
};

Window make_window(const std::vector<nmh::AudioSample>& audio, const F2Result& f2, const TableSpec& ts,
                   bool use_right) {
    Window w;
    if (f2.pass_starts.size() < 2) return w;
    const uint64_t t2 = f2.pass_starts[1];
    w.level = (uint64_t)ts.unit * kAudPer * kCck;
    const uint64_t span = ((2ull * (uint64_t)ts.N * (uint64_t)ts.K - 1ull) * w.level) / 2ull;
    auto tr = first_transition(audio, t2, std::min<uint64_t>(audio.size(), t2 + 4 * w.level), use_right);
    w.s = tr ? *tr : t2;
    w.e = w.s + span;
    if (w.e > audio.size()) return w;
    w.runs = raw_runs(audio, w.s, w.e, use_right);
    w.have = true;
    return w;
}

// LEVELS(W) (SPECIFICATION.md 2.5): the value of every run, in time order.
std::vector<int32_t> run_values(const std::vector<RawRun>& runs) {
    std::vector<int32_t> out;
    out.reserve(runs.size());
    for (auto& r : runs) out.push_back(r.value);
    return out;
}

struct FitResult { bool matches = false; double a = 0, b = 0; int rotation = -1; double resid = 1e300; };

// kExactTol: FIT(L, C) (SPECIFICATION.md 2.5) grades an exact digital
// relation, not a band the page never gives: every absolute residual
// |L[i] - a - b * s| at most kExactTol, in raw port counts. Measured on this
// RTL (every table, every config, both cores, the genuine match case): the
// best rotation's largest residual is 0 to double-precision arithmetic --
// audio_left/audio_right hold a value that
// is an exact linear function of the byte code for the whole of a run, so
// a residual above floating-point noise means the level list does not
// match the code sequence, not that it matches within some tolerance.
constexpr double kExactTol = 1e-6;

// Returns the BEST rotation's (a, b) even when no rotation satisfies
// kExactTol: the fitted |b| is printed as a MEASURED line (2.7, A4)
// independently of whether the pattern matches the expected code
// sequence, so a proposition's own mutated data (which legitimately breaks
// the match) does not also silently break that line.
// `matches` is the actual FIT(L, C) verdict P1-P4 grade.
FitResult fit(const std::vector<double>& L, const std::vector<int>& s) {
    FitResult best;
    double best_resid = 1e300;
    size_t K = s.size();
    if (L.empty() || K == 0) return best;
    for (size_t r = 0; r < K; ++r) {
        double n = (double)L.size();
        double sx = 0, sy = 0, sxx = 0, sxy = 0;
        for (size_t i = 0; i < L.size(); ++i) {
            double x = (double)s[(i + r) % K];
            double y = (double)L[i];
            sx += x; sy += y; sxx += x * x; sxy += x * y;
        }
        double denom = n * sxx - sx * sx;
        if (std::fabs(denom) < 1e-9) continue;
        double b = (n * sxy - sx * sy) / denom;
        double a = (sy - b * sx) / n;
        double max_resid = 0;
        for (size_t i = 0; i < L.size(); ++i) {
            double x = (double)s[(i + r) % K];
            double resid = std::fabs((double)L[i] - a - b * x);
            if (resid > max_resid) max_resid = resid;
        }
        if (std::fabs(b) <= 1e-9) continue;
        if (max_resid < best_resid) {
            best_resid = max_resid;
            best = {max_resid <= kExactTol, a, b, (int)r, max_resid};
        }
    }
    return best;
}

// ---- F1: refresh-group time base (2.4), used by P6 -----------------------
struct F1Result { bool anchor_ok = false; double color_clocks = 0.0; size_t groups = 0; };

// Bounded to [lo_cycle, hi_cycle]: SPECIFICATION.md 2.4 says F1 is "used
// by P6", so its 100 lines must lie inside the interval P6 itself grades
// (F3's read k=16 to k=528 of the run in question), not before it.
//
// all_groups: false averages the first 100 group spacings (B4, P6's interval);
// true averages every group spacing between lo_cycle and hi_cycle (A6).
F1Result f1_check(const std::vector<nmh::RamAccess>& log, uint64_t lo_cycle, uint64_t hi_cycle,
                  bool all_groups = false) {
    F1Result r;
    std::vector<uint64_t> refresh;
    for (const auto& a : log)
        if (!a.is_write && a.word_addr == 0 && a.cycle >= lo_cycle && a.cycle <= hi_cycle)
            refresh.push_back(a.cycle);
    std::vector<uint64_t> groups;
    constexpr uint64_t kGroupGap = 100;   // within-group ~4*kCck=32 cycles; between-group ~227.5*kCck
    for (size_t i = 0; i < refresh.size(); ++i)
        if (i == 0 || refresh[i] - refresh[i - 1] > kGroupGap) groups.push_back(refresh[i]);
    r.groups = groups.size();
    if (groups.size() < 101) return r;
    const size_t n = all_groups ? groups.size() - 1 : 100;
    double total = 0;
    for (size_t i = 0; i < n; ++i) total += (double)(groups[i + 1] - groups[i]);
    double mean_cycles = total / (double)n;
    r.color_clocks = mean_cycles / (double)kCck;
    r.anchor_ok = (r.color_clocks >= 227.0 && r.color_clocks <= 228.0);
    return r;
}

// ---- F3: disk word index (2.4), used by P6 -------------------------------
struct F3Result { bool anchor_ok = false; std::vector<uint64_t> times; };

F3Result f3_check(const std::vector<nmh::RamAccess>& log, uint32_t base_word, uint32_t max_words) {
    F3Result r;
    uint32_t expect = base_word;
    for (const auto& a : log) {
        if (a.is_write || a.word_addr < base_word || a.word_addr >= base_word + max_words) continue;
        if (a.word_addr != expect) { r.anchor_ok = false; return r; }
        r.times.push_back(a.cycle);
        ++expect;
        if (expect >= base_word + max_words) break;
    }
    r.anchor_ok = !r.times.empty();
    return r;
}

// ---- audio program run ----------------------------------------------------
struct AudioCapture {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::optional<uint16_t> adkconr;
    std::optional<uint16_t> fixval;   // FIXTURE's own known-constant slot (KFIX)
    std::vector<nmh::RamAccess> log;       // coalesced: for F2
    std::vector<nmh::RamAccess> log_raw;   // NOT coalesced: for F1 (A6)
    std::vector<nmh::AudioSample> audio;
};

// A6 (SPECIFICATION.md 2.7): F1's anchor in the audio run itself, over the
// interval from the first to the last read of the audio tables.
struct CalResult { bool graded = false, ok = false; double cck = 0; size_t groups = 0; uint64_t lo = 0, hi = 0; };

CalResult calibrate(const AudioCapture& cap, uint32_t lo_word, uint32_t hi_word) {
    CalResult r;
    bool any = false;
    for (const auto& a : cap.log) {
        if (a.is_write || a.word_addr < lo_word || a.word_addr > hi_word) continue;
        if (!any) { r.lo = a.cycle; any = true; }
        r.hi = a.cycle;
    }
    if (!any) return r;
    F1Result f = f1_check(cap.log_raw, r.lo, r.hi, true);
    r.graded = true; r.ok = f.anchor_ok; r.cck = f.color_clocks; r.groups = f.groups;
    return r;
}

AudioCapture run_audio(const std::string& dir, const std::string& mv, const nmh::Config& cfg,
                       const std::vector<std::pair<uint32_t, uint16_t>>& pokes = {}) {
    AudioCapture r;
    std::string bin = dir + "/prog_audio.bin";
    if (mv == "A1") bin = dir + "/prog_audio_a1mut.bin";
    else if (mv == "A2") bin = dir + "/prog_audio_a2mut.bin";
    else if (mv == "A3") bin = dir + "/prog_audio_a3mut.bin";
    else if (mv == "A4") bin = dir + "/prog_audio_a4mut.bin";
    else if (mv == "FIXCONST") bin = dir + "/prog_audio_fixturemut.bin";
    else if (mv == "FIXWRITE") bin = dir + "/prog_audio_fixwritemut.bin";
    else if (mv == "A6") bin = dir + "/prog_audio_a6mut.bin";
    else if (mv == "P1LAST") bin = dir + "/prog_audio_p1lastmut.bin";
    else if (mv == "NODONE") bin = dir + "/prog_nodone.bin";

    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;

    poke_table(h, T1_ADDR, t1_table(mv));
    poke_table(h, T2_ADDR, t2_table(mv));
    poke_table(h, T3_ADDR, t3_table(mv));
    for (const auto& pk : pokes) h.poke_word(pk.first / 2, pk.second);

    r.released = h.boot(kBootBudget).cpu_released;
    r.done = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kAudioRunBudget);
    // The known constant first; no other result slot is read unless it
    // reads back (SPECIFICATION.md 2.3).
    r.fixval = h.peek_logical(kFixAudio);
    if (r.fixval && *r.fixval == kFixValue) r.adkconr = h.peek_logical(kResults);
    r.log_raw = h.ram_log();
    r.log = nmh::coalesce(r.log_raw);
    r.audio = h.audio();
    return r;
}

// P1's last-level contrast (SPECIFICATION.md 3, P1): a first run of the same
// program writes the byte's own value; from its capture the test takes the
// code that the window's last level comes from, the instant between the
// DMA's last two reads of that word, and the number of lines the program
// has to wait. The returned capture is the second run, which writes a
// different value there. `note` receives the measured set-up.
AudioCapture run_audio_p1last(const std::string& dir, const nmh::Config& cfg, std::string& note) {
    const TableSpec t1{"T1", T1_ADDR, 4, kNT1, 8, 1};
    auto params = [&](uint16_t lines, uint32_t addr, uint16_t val) {
        return std::vector<std::pair<uint32_t, uint16_t>>{
            {kPlLines, lines}, {kPlAddr, (uint16_t)(addr >> 16)}, {kPlAddr + 2, (uint16_t)(addr & 0xFFFF)},
            {kPlVal, val}};
    };
    AudioCapture probe = run_audio(dir, "P1LAST", cfg, params(4, T1_ADDR, 0x00));
    if (!probe.loaded || !probe.done) { note = "P1 last-level set-up: the first run did not complete"; return probe; }
    const bool use_right = audio_side_is_right(probe.audio);
    F2Result f2 = f2_check(probe.log, T1_ADDR / 2, T1_ADDR / 2 + 3);
    Window w = make_window(probe.audio, f2, t1, use_right);
    CalResult cal = calibrate(probe, T1_ADDR / 2, T1_ADDR / 2 + 3);
    if (!f2.anchor_ok || !w.have || !cal.graded) { note = "P1 last-level set-up: no window in the first run"; return probe; }
    FitResult fu = fit(unwrap15(run_values(w.runs)), c_up());
    if (!fu.matches) { note = "P1 last-level set-up: the first run's levels do not match C_up"; return probe; }
    const uint32_t code = (uint32_t)((t1.N * t1.K - 1 + fu.rotation) % t1.K);   // code of the window's last level
    const uint32_t word = T1_ADDR / 2 + code / 2;
    std::vector<uint64_t> reads;
    for (const auto& a : probe.log) if (!a.is_write && a.word_addr == word && a.cycle < w.e) reads.push_back(a.cycle);
    uint64_t mark = 0;
    for (const auto& a : probe.log) if (a.is_write && a.word_addr == kPlMark / 2) { mark = a.cycle; break; }
    if (reads.size() < 2 || mark == 0) { note = "P1 last-level set-up: reads of the word or the marker not found"; return probe; }
    const uint64_t r_prev = reads[reads.size() - 2], r_last = reads.back();
    const double line_cycles = cal.cck * (double)kCck;
    const double target = 0.5 * (double)(r_prev + r_last);
    long lines = std::lround((target - (double)mark) / line_cycles + 0.5);
    if (lines < 1) lines = 1;
    AudioCapture mut = run_audio(dir, "P1LAST", cfg, params((uint16_t)lines, T1_ADDR + code, 0x7F));
    uint64_t wr = 0;
    for (const auto& a : mut.log) if (a.is_write && a.word_addr == word) { wr = a.cycle; break; }
    char buf[400];
    std::snprintf(buf, sizeof buf,
        "P1 last-level set-up (not graded): the window's last level is code %u of C_up, byte %s of T1's word %u; "
        "the DMA read that word at cycles %llu and %llu; the program waited %ld lines and its CPU write came at "
        "cycle %llu (%s)",
        code, (code % 2) ? "lower" : "upper", code / 2, (unsigned long long)r_prev, (unsigned long long)r_last, lines,
        (unsigned long long)wr, (wr > r_prev && wr < r_last) ? "between the two reads" : "NOT between the two reads");
    note = buf;
    return mut;
}

// ---- T2S (P3B) program run: independent of prog_audio.s and its own
// mutations (2.4, section 5 -- keeping this table's own fresh DMA start
// out of prog_audio.s means an A1/A2/A4/P1-P4 mutation there can never
// disturb P3B's own capture). --------------------------------------------
AudioCapture run_t2s(const std::string& dir, const std::string& mv, const nmh::Config& cfg) {
    AudioCapture r;
    std::string bin = dir + "/prog_t2s.bin";
    if (mv == "FIXCONST") bin = dir + "/prog_t2s_fixturemut.bin";
    else if (mv == "A2T") bin = dir + "/prog_t2s_a2mut.bin";
    else if (mv == "FIXWRITE") bin = dir + "/prog_t2s_fixwritemut.bin";
    else if (mv == "A6") bin = dir + "/prog_t2s_a6mut.bin";
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    poke_table(h, T2S_ADDR, t2s_table(mv));
    r.released = h.boot(kBootBudget).cpu_released;
    r.done = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kAudioRunBudget);
    r.fixval = h.peek_logical(kFixAudio);
    if (r.fixval && *r.fixval == kFixValue) r.adkconr = h.peek_logical(kResults);
    r.log_raw = h.ram_log();
    r.log = nmh::coalesce(r.log_raw);
    r.audio = h.audio();
    return r;
}

// ---- adkcon program run ----------------------------------------------------
struct AdkconCapture {
    bool loaded = false;
    std::optional<uint64_t> done;
    std::optional<uint16_t> r0, r1, r2, r3, r4;
    std::optional<uint16_t> fixval;   // FIXTURE's own known-constant slot (KFIX)
    std::vector<nmh::RamAccess> log;
};

AdkconCapture run_adkcon(const std::string& dir, const std::string& mv, const nmh::Config& cfg) {
    AdkconCapture r;
    std::string bin = dir + "/prog_adkcon.bin";
    if (mv == "P5") bin = dir + "/prog_adkcon_p5mut.bin";
    else if (mv == "C1") bin = dir + "/prog_adkcon_c1mut.bin";
    else if (mv == "C1DUP") bin = dir + "/prog_adkcon_c1dupmut.bin";
    else if (mv == "FIXCONST") bin = dir + "/prog_adkcon_fixturemut.bin";

    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    h.boot(kBootBudget);
    r.done = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kAdkconRunBudget);
    r.fixval = h.peek_logical(kFixAdkcon);
    if (r.fixval && *r.fixval == kFixValue) {
        r.r0 = h.peek_logical(kResults + 0);
        r.r1 = h.peek_logical(kResults + 2);
        r.r2 = h.peek_logical(kResults + 4);
        r.r3 = h.peek_logical(kResults + 6);
        r.r4 = h.peek_logical(kResults + 8);
    }
    r.log = nmh::coalesce(h.ram_log());
    return r;
}

// ---- diskrate program run ----------------------------------------------------
struct DiskCapture {
    bool loaded = false;
    std::optional<uint64_t> done;
    std::optional<uint16_t> b1_f, b2_f, b1_s, b2_s;
    std::optional<uint16_t> fixval;   // FIXTURE's own known-constant slot (KFIX)
    std::vector<nmh::RamAccess> log;       // coalesced: for F3 (each k is its own address)
    std::vector<nmh::RamAccess> log_raw;   // NOT coalesced: for F1 (refresh always reads word_addr
                                            // 0 with the same data, so a long idle stretch of
                                            // identical-looking refresh cycles would otherwise
                                            // collapse into one logical access and hide most lines)
};

DiskCapture run_diskrate(const std::string& dir, const std::string& bin_suffix, const nmh::Config& cfg,
                         bool fixdata) {
    DiskCapture r;
    std::string bin = dir + "/prog_diskrate" + bin_suffix + ".bin";
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    // NMH_MUTATE=FIXDATA pokes $5555 instead of the $AAAA the FIXTURE-DATA
    // check expects (kDiskWord).
    poke_fill(h, DISKBUF_A, fixdata ? 0x5555 : kDiskWord, 544);
    poke_fill(h, DISKBUF_B, fixdata ? 0x5555 : kDiskWord, 544);
    h.boot(kBootBudget);
    r.done = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kDiskRunBudget);
    r.fixval = h.peek_logical(kFixDisk);
    if (r.fixval && *r.fixval == kFixValue) {
        r.b1_f = h.peek_logical(kResults + 0);
        r.b2_f = h.peek_logical(kResults + 2);
        r.b1_s = h.peek_logical(kResults + 6);
        r.b2_s = h.peek_logical(kResults + 8);
    }
    r.log_raw = h.ram_log();
    r.log = nmh::coalesce(r.log_raw);
    return r;
}

std::string fmt(double v, int prec = 2) {
    char buf[64];
    std::snprintf(buf, sizeof buf, "%.*f", prec, v);
    return std::string(buf);
}

}  // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    // One selector per fixture line; each applies to the programs that
    // line is printed for. FIXCONST: every program stores $DEAD instead of
    // $BEEF. FIXDATA: every table and buffer is poked with one word other
    // than the content the check expects. FIXWRITE: the audio program and
    // the disk program leave out one result-slot store. NODONE: the audio
    // program is replaced by one that never writes the done marker.
    const bool fixconst_mv = (mv=="FIXCONST");
    const bool fixdata_mv  = (mv=="FIXDATA");
    const bool fixwrite_mv = (mv=="FIXWRITE");
    const bool nodone_mv   = (mv=="NODONE");
    const bool audio_mv   = (mv=="P1"||mv=="P2"||mv=="P3"||mv=="P4"||mv=="A1"||mv=="A2"||mv=="A4"||mv=="A5"||
                             mv=="A5SYS"||mv=="A6"||mv=="A3"||mv=="P1LAST"||
                             fixconst_mv||fixdata_mv||fixwrite_mv||nodone_mv);
    const bool t2s_mv     = (mv=="P3B"||mv=="A2T"||mv=="A6"||fixconst_mv||fixdata_mv||fixwrite_mv);
    const bool adkcon_mv  = (mv=="P5"||mv=="C1"||mv=="C1DUP"||fixconst_mv);
    const bool disk_mv    = (mv=="FASTF0"||mv=="FASTS1"||mv=="B2"||mv=="B3"||mv=="F1"||fixconst_mv||fixdata_mv||fixwrite_mv);
    const bool need_audio  = mv.empty() || audio_mv;
    const bool need_t2s    = mv.empty() || t2s_mv;
    const bool need_adkcon = mv.empty() || adkcon_mv;
    const bool need_disk   = mv.empty() || disk_mv;
    const std::string amv  = audio_mv ? mv : "";
    const std::string kmv  = adkcon_mv ? mv : "";
    // The content FIXTURE-DATA expects: the poked content, except under
    // FIXDATA, which pokes something else.
    const std::string emv  = fixdata_mv ? "" : mv;

    // =====================================================================
    // P1-P4: audio byte output, order, two's complement, per-LSB step
    // =====================================================================
    if (need_audio) {
        struct CfgSpec { const char* label; nmh::Config cfg; };
        std::vector<CfgSpec> configs = { {"ECS/PAL", cfg_ecs_pal()}, {"OCS/PAL", cfg_ocs_pal()} };

        for (auto& cs : configs) {
            std::string p1last_note;
            AudioCapture cap = (mv == "P1LAST") ? run_audio_p1last(dir, cs.cfg, p1last_note)
                                                : run_audio(dir, amv, cs.cfg);
            bool base_ok = cap.loaded && cap.done.has_value();
            std::string done_label = std::string("FIXTURE ") + cs.label +
                ": prog_audio reached DONE [not graded -- fixture, no claim uid]";
            if (nodone_mv) done_label += " [NMH_MUTATE demonstrates this fixture red]";
            checks.expect(base_ok, done_label);
            if (!base_ok) {
                checks.record(std::string("audio fixture did not complete in ") + cs.label +
                    "; A1-A6, P1-P4 not graded there");
                continue;
            }

            // FIXTURE-CONST: a program-stored known constant, asserted
            // first, before anything this program measures is stored or
            // read. The unchanged comparator (== kFixValue) either way:
            // NMH_MUTATE=FIXCONST stores kFixMutValue instead, so this
            // prints FAIL through the same clause. The stored result slot
            // (A2) is graded only behind it.
            std::string fix_label = std::string("FIXTURE-CONST ") + cs.label +
                ": prog_audio's own known constant reads back correctly "
                "[not graded -- fixture, no claim uid]";
            if (fixconst_mv) fix_label += " [NMH_MUTATE demonstrates this fixture red]";
            bool fix_const_ok = cap.fixval.has_value() && *cap.fixval == kFixValue;
            checks.expect(fix_const_ok, fix_label);

            // FIXTURE-DATA: the preloaded DMA data (T1/T2/T3, poked
            // directly into chip RAM before boot()) is asserted on the
            // Chip-memory port itself -- every read of each table returns
            // the content expected there, and every word of it is read --
            // not merely trusted from poke_word(). The table's A4, A5 and
            // propositions are graded only behind its own line.
            auto t1w = t1_table(emv); auto t2w = t2_table(emv); auto t3w = t3_table(emv);
            bool t1_data_ok = all_reads_match(cap.log, T1_ADDR / 2, t1w, true, mv == "P1LAST" ? 1 : 0);
            bool t2_data_ok = all_reads_match(cap.log, T2_ADDR / 2, t2w);
            bool t3_data_ok = all_reads_match(cap.log, T3_ADDR / 2, t3w);
            {
                std::string d1 = std::string("FIXTURE-DATA T1 ") + cs.label +
                    ": every read of T1 on the Chip-memory port returns the poked table content, every word read "
                    "[not graded -- fixture, no claim uid]";
                std::string d2 = std::string("FIXTURE-DATA T2 ") + cs.label +
                    ": every read of T2 on the Chip-memory port returns the poked table content, every word read "
                    "[not graded -- fixture, no claim uid]";
                std::string d3 = std::string("FIXTURE-DATA T3 ") + cs.label +
                    ": every read of T3 on the Chip-memory port returns the poked table content, every word read "
                    "[not graded -- fixture, no claim uid]";
                if (fixdata_mv) {
                    d1 += " [NMH_MUTATE demonstrates this fixture red]";
                    d2 += " [NMH_MUTATE demonstrates this fixture red]";
                    d3 += " [NMH_MUTATE demonstrates this fixture red]";
                }
                checks.expect(t1_data_ok, d1);
                checks.expect(t2_data_ok, d2);
                checks.expect(t3_data_ok, d3);
            }

            bool use_right = audio_side_is_right(cap.audio);
            if (!p1last_note.empty()) checks.note(p1last_note + " [" + cs.label + "]");
            checks.note(std::string("audio output side carrying channel 0, ") + cs.label + ": " +
                (use_right ? "audio_right" : "audio_left") +
                " (p.137: right-side jack; Appendix E's pin list, p.326: left-side jack -- a "
                "measured page disagreement; SPECIFICATION.md 2.5 does not grade which named "
                "jack this is)");

            // FIXTURE-WRITE: the A2 result slot was written by the program,
            // so the value read back from it is the program's own.
            bool a2_write = has_one_write(cap.log, kResults / 2);
            std::string wr_label = std::string("FIXTURE-WRITE ") + cs.label +
                ": the Chip-memory port shows one CPU write to the A2 result slot "
                "[not graded -- fixture, no claim uid]";
            if (fixwrite_mv) wr_label += " [NMH_MUTATE demonstrates this fixture red]";
            checks.expect(a2_write, wr_label);

            // A2 reads a stored slot: graded only when the fixture constant
            // and the slot's write both hold.
            bool a2_ok = false;
            if (fix_const_ok && a2_write) {
                a2_ok = cap.adkconr.has_value() && ((*cap.adkconr) & 0x00FF) == 0;
                std::string a2_label = std::string("A2 ") + cs.label +
                    ": ADKCONR bits 7-0 read 0, no attach mode [p.265]";
                if (mv == "A2") a2_label += " [NMH_MUTATE demonstrates this precondition red]";
                checks.expect(a2_ok, a2_label);
            } else {
                checks.record(std::string("A2 ") + cs.label +
                    ": not graded, FIXTURE-CONST or FIXTURE-WRITE failed");
            }

            // A6: the color-clock unit of the window (2.5, 2.7) holds in this
            // run, F1's anchor over the interval of its audio reads.
            bool a6_ok = false;
            {
                CalResult cal = calibrate(cap, T1_ADDR / 2, T3_ADDR / 2 + 255);
                if (cal.graded) {
                    checks.note(std::string("F1 anchor in prog_audio's own run ") + cs.label + ": mean refresh-group "
                        "spacing " + fmt(cal.cck, 2) + " color clocks over " + std::to_string(cal.groups) +
                        " groups found");
                    a6_ok = cal.ok;
                    std::string a6l = std::string("A6 ") + cs.label +
                        ": the refresh groups of prog_audio's own audio DMA interval, all of them, recur 227-228 color "
                        "clocks apart on average, so the window's level is 8 clk_sys cycles per color clock of the "
                        "programmed period";
                    if (mv == "A6" && std::string(cs.label) == "ECS/PAL") a6l += " [NMH_MUTATE demonstrates this precondition red]";
                    checks.expect(a6_ok, a6l);
                } else {
                    checks.record(std::string("A6 ") + cs.label + ": not graded, no audio read of any table");
                }
            }

            TableSpec t1{"T1", T1_ADDR, 4, kNT1, 8, 1}, t2{"T2", T2_ADDR, 4, kNT2, 4, 2},
                      t3{"T3", T3_ADDR, 256, kNT3, 256, 2};

            // A6 (the window's time unit), A1 (F2's anchor from the first
            // restart on, covering the window), A2 and A3 (median run
            // length against the programmed level): shared by every
            // proposition on this table, graded in that order. `ok` is false
            // only when one of them already failed (and that failure has
            // already been printed or recorded); it stays true into A4's own
            // territory, so a window that holds few runs (A4's own
            // contrasting case: the output goes flat inside it) is graded as
            // A4 red, not swallowed as "a precondition failed" further
            // upstream.
            //
            // Every mutation below runs this same, unchanged comparator: the
            // label states what the mutated stimulus is expected to make it
            // print, but the printed verdict (PASS/FAIL) always reports what
            // the comparator actually found, never an inverted "the defect
            // was reproduced, so print PASS" reading.
            struct PrecondResult { bool ok; std::vector<double> L; uint64_t level; };
            auto grade_precond = [&](const TableSpec& ts, bool a1_mut, bool a3_mut) -> PrecondResult {
                uint32_t lo = ts.addr / 2, hi = lo + ts.words - 1;
                F2Result f2 = f2_check(cap.log, lo, hi);
                checks.note(first_pass_text(ts.name, cs.label, f2, ts.words));
                // The window is built with A6's unit: A1, A3-A5 and the
                // propositions are graded only behind A6.
                if (!a6_ok) {
                    checks.record(std::string(ts.name) + " " + cs.label +
                        ": A1, A3, A4, A5 and its proposition(s) not graded, precondition A6 failed or not graded");
                    return {false, {}, 0};
                }
                Window win = make_window(cap.audio, f2, ts, use_right);
                bool a1 = f2.anchor_ok && (int)f2.pass_starts.size() >= ts.N + 3 && win.have &&
                          win.e <= f2.pass_starts.back();
                std::string a1_label = std::string("A1 for ") + ts.name + " " + cs.label +
                    ": audio DMA ran pass after pass from t_2 on, F2 holds with >= " + std::to_string(ts.N + 3) +
                    " reads of " + ts.name + "'s first word, the last of them at or after the end of W_" + ts.name;
                if (a1_mut) a1_label += " [NMH_MUTATE demonstrates this precondition red]";
                checks.expect(a1, a1_label);
                if (!f2.diag.empty()) checks.note(std::string(ts.name) + " " + cs.label + " F2 break: " + f2.diag);

                if (!a1) {
                    checks.record(std::string(ts.name) + " " + cs.label +
                        ": A3, A4, A5 and its proposition(s) not graded, precondition A1 failed");
                    return {false, {}, 0};
                }
                if (!a2_ok) {
                    checks.record(std::string(ts.name) + " " + cs.label +
                        ": A3, A4, A5 and its proposition(s) not graded, precondition A2 failed or not graded");
                    return {false, {}, 0};
                }
                double median = median_run_len(win.runs);
                bool a3 = median >= 4.0 && median == (double)win.level;
                std::string a3_label = std::string("A3 for ") + ts.name + " " + cs.label +
                    ": the median run in W_" + ts.name + " spans >= 4 output samples and equals the programmed held level of " +
                    std::to_string(win.level) + " (" + fmt(median, 1) + ")";
                if (a3_mut) a3_label += " [NMH_MUTATE demonstrates this precondition red]";
                checks.expect(a3, a3_label);
                uint64_t shortest = ~0ull, longest = 0;
                for (auto& r : win.runs) { shortest = std::min(shortest, r.len); longest = std::max(longest, r.len); }
                checks.note(std::string("window W_") + ts.name + " " + cs.label + " (not graded): " +
                    std::to_string(win.e - win.s) + " samples from the first output transition after t_2, " +
                    std::to_string(ts.N) + " passes x " + std::to_string(ts.K) + " levels of " +
                    std::to_string(win.level) + " samples programmed (the last one cut); " +
                    std::to_string(win.runs.size()) + " runs found, median " + fmt(median, 1) + ", shortest " +
                    std::to_string(win.runs.empty() ? 0 : shortest) + ", longest " + std::to_string(longest));
                if (!a3) {
                    checks.record(std::string(ts.name) + " " + cs.label +
                        ": A4, A5 and its proposition(s) not graded, precondition A3 failed");
                    return {false, {}, 0};
                }
                return {true, unwrap15(run_values(win.runs)), win.level};
            };

            // A5: the window holds the programmed population, N x K held
            // levels, the last one kept (SPECIFICATION.md 2.7). Where A4
            // already failed, A5 is not reached (same "not graded" chain).
            auto grade_a5 = [&](const TableSpec& ts, const PrecondResult& pc) -> bool {
                size_t expected = (size_t)ts.N * (size_t)ts.K;
                size_t found = pc.L.size();
                bool a5 = found == expected;
                std::string a5_label = std::string("A5 for ") + ts.name + " " + cs.label +
                    ": the window holds the programmed population of exactly " + std::to_string(expected) +
                    " held levels, the last one kept (" + std::to_string(ts.N) + " passes x " +
                    std::to_string(ts.K) + " codes, " + std::to_string(pc.level) +
                    " samples per level programmed; " + std::to_string(found) + " found)";
                if (mv == "A5" && std::string(ts.name) == "T2") a5_label += " [NMH_MUTATE demonstrates this precondition red]";
                checks.expect(a5, a5_label);
                return a5;
            };

            // ---- T1: P1, P2 ----
            {
                auto pc = grade_precond(t1, mv == "A1", mv == "A3" || mv == "A5SYS");
                if (!pc.ok) {
                    checks.record(std::string("P1/P2 ") + cs.label + ": not graded, a precondition failed");
                } else if (!t1_data_ok) {
                    checks.record(std::string("P1/P2 ") + cs.label + ": not graded, A4, A5 and the "
                        "propositions need FIXTURE-DATA for T1");
                } else {
                    auto& L = pc.L;
                    FitResult fu, fl;
                    if (!L.empty()) { fu = fit(L, c_up()); fl = fit(L, c_low()); }
                    double b = fu.matches ? fu.b : fl.b;
                    bool a4 = L.size() >= (size_t)(2 * t1.K);
                    std::string a4_label = std::string("A4 for T1 ") + cs.label +
                        ": L has >= " + std::to_string(2 * t1.K) + " entries (" + std::to_string(L.size()) + ")";
                    if (mv == "A4") a4_label += " [NMH_MUTATE demonstrates this precondition red]";
                    checks.expect(a4, a4_label);
                    checks.note(std::string("fitted output change per LSB, T1 ") + cs.label + " (not graded): |b| = " +
                        fmt(std::fabs(b), 3) + " raw port counts");

                    if (!a4) {
                        checks.record(std::string("P1/P2 ") + cs.label + ": not graded, precondition A4 failed");
                    } else if (!grade_a5(t1, pc)) {
                        checks.record(std::string("P1/P2 ") + cs.label + ": not graded, precondition A5 failed");
                    } else {
                        bool p1 = fu.matches || fl.matches;
                        std::string p1_label = std::string("P1 ") + cs.label +
                            " [p.265]: both bytes of every audio data word are output, "
                            "one after the other (FIT(L,C_up) or FIT(L,C_low) matches, all 4 words of "
                            "every pass in W_T1)";
                        if (mv == "P1") p1_label += " [NMH_MUTATE demonstrates this proposition red]";
                        checks.expect(p1, p1_label);
                        if (mv == "P1LAST") {
                            // MEASURED: which entries of L differ from the level K entries
                            // earlier (the same code, one pass before).
                            std::string idx;
                            size_t n_off = 0;
                            for (size_t i = (size_t)t1.K; i < L.size(); ++i)
                                if (std::fabs(L[i] - L[i - (size_t)t1.K]) > kExactTol) {
                                    ++n_off; idx += (idx.empty() ? "" : ",") + std::to_string(i);
                                }
                            checks.note(std::string("P1 last-level contrast, T1 ") + cs.label + " (not graded): " +
                                std::to_string(n_off) + " of " + std::to_string(L.size() - (size_t)t1.K) +
                                " entries of L differ from the level one pass earlier, at index " + idx +
                                " (the last entry is " + std::to_string(L.size() - 1) + ")");
                        }

                        if (p1) {
                            bool p2 = fu.matches;
                            std::string p2_label = std::string("P2 ") + cs.label +
                                " [p.265,p.164,p.138]: the upper byte is output first (FIT(L,C_up) matches)";
                            if (mv == "P2") p2_label += " [NMH_MUTATE demonstrates this proposition red]";
                            checks.expect(p2, p2_label);
                        } else {
                            checks.record(std::string("P2 ") + cs.label + ": not graded, P1 is red");
                        }
                    }
                }
            }

            // ---- T2: P3 ----
            {
                auto pc = grade_precond(t2, false, false);
                if (!pc.ok) {
                    checks.record(std::string("P3 ") + cs.label + ": not graded, a precondition failed");
                } else if (!t2_data_ok) {
                    checks.record(std::string("P3 ") + cs.label + ": not graded, A4, A5 and the "
                        "proposition need FIXTURE-DATA for T2");
                } else {
                    auto& L = pc.L;
                    FitResult f2c = L.empty() ? FitResult{} : fit(L, c2());
                    bool a4 = L.size() >= (size_t)(2 * t2.K);
                    checks.expect(a4, std::string("A4 for T2 ") + cs.label +
                        ": L has >= " + std::to_string(2 * t2.K) + " entries (" + std::to_string(L.size()) + ")");
                    checks.note(std::string("fitted output change per LSB, T2 ") + cs.label + " (not graded): |b| = " +
                        fmt(std::fabs(f2c.b), 3) + " raw port counts");
                    if (!a4) {
                        checks.record(std::string("P3 ") + cs.label + ": not graded, precondition A4 failed");
                    } else if (!grade_a5(t2, pc)) {
                        checks.record(std::string("P3 ") + cs.label + ": not graded, precondition A5 failed");
                    } else {
                        bool p3 = f2c.matches;
                        std::string p3_label = std::string("P3 ") + cs.label +
                            " [p.265,p.138]: each byte is output as a two's-complement value, at "
                            "four representative codes with equal upper/lower bytes in every word "
                            "(FIT(L,C2) matches, s=0,1,127,-128; P3B grades the same claim with "
                            "distinct codes in both byte positions)";
                        if (mv == "P3") p3_label += " [NMH_MUTATE demonstrates this proposition red]";
                        checks.expect(p3, p3_label);
                    }
                }
            }

            // ---- T3: P4 ----
            {
                auto pc = grade_precond(t3, false, false);
                if (!pc.ok) {
                    checks.record(std::string("P4 ") + cs.label + ": not graded, a precondition failed");
                } else if (!t3_data_ok) {
                    checks.record(std::string("P4 ") + cs.label + ": not graded, A4, A5 and the "
                        "proposition need FIXTURE-DATA for T3");
                } else {
                    auto& L = pc.L;
                    FitResult f3c = L.empty() ? FitResult{} : fit(L, c3());
                    bool a4 = L.size() >= (size_t)(t3.words + 1);
                    checks.expect(a4, std::string("A4 for T3 ") + cs.label +
                        ": L has >= " + std::to_string(t3.words + 1) + " entries (" + std::to_string(L.size()) + ")");
                    checks.note(std::string("fitted output change per LSB, T3 ") + cs.label + " (not graded): |b| = " +
                        fmt(std::fabs(f3c.b), 3) + " raw port counts");
                    if (!a4) {
                        checks.record(std::string("P4 ") + cs.label + ": not graded, precondition A4 failed");
                    } else if (!grade_a5(t3, pc)) {
                        checks.record(std::string("P4 ") + cs.label + ": not graded, precondition A5 failed");
                    } else {
                        bool p4 = f3c.matches;
                        std::string p4_label = std::string("P4 ") + cs.label +
                            " [p.265]: every one-LSB step of a data byte moves the audio "
                            "output by the same amount (FIT(L,C3) matches, all 255 adjacent steps, s=-128..127)";
                        if (mv == "P4") p4_label += " [NMH_MUTATE demonstrates this proposition red]";
                        checks.expect(p4, p4_label);
                    }
                }
            }
        }
        checks.record("A-audxdat.c5 (p.265, \"LSB = 3 MV\"): the 3 mV magnitude is not observable at "
            "these ports (samples, not volts); its two's-complement wording is anchor text shared "
            "with 5-appA-p265.c42, not a separate clause of its own -- declined in full, not "
            "asserted on P3 or P4 (SPECIFICATION.md 1.2)");
        checks.record("5-appA-p265.c42 (p.265, \"...are outputted sequentially (with "
            "digital-to-analog conversion) to the audio output pins\"): withdrawn from P1, P3 and "
            "P3B -- every assertion in this package observes only the top-level digital sample bus "
            "(audio_left/audio_right), never a D/A-converted analog level or a physical output pin; "
            "their own narrower, page-derived checks (byte content, byte order, two's-complement "
            "value) stay graded without this id (SPECIFICATION.md 1.2)");
    }

    // =====================================================================
    // P3B: each byte is output as a two's-complement value independent of
    // byte position -- T2S (prog_t2s.s) carries distinct codes in both
    // byte positions, unlike T2 (2.6: T2's equal-byte words "do not
    // depend on byte order"). Its own program, independent of
    // prog_audio.s and its own mutations (see run_t2s()).
    // =====================================================================
    if (need_t2s) {
        struct CfgSpec { const char* label; nmh::Config cfg; };
        std::vector<CfgSpec> configs = { {"ECS/PAL", cfg_ecs_pal()}, {"OCS/PAL", cfg_ocs_pal()} };
        for (auto& cs : configs) {
            AudioCapture cap = run_t2s(dir, t2s_mv ? mv : "", cs.cfg);
            bool base_ok = cap.loaded && cap.done.has_value();
            checks.expect(base_ok, std::string("FIXTURE T2S ") + cs.label +
                ": prog_t2s reached DONE [not graded -- fixture, no claim uid]");
            if (!base_ok) {
                checks.record(std::string("T2S fixture did not complete in ") + cs.label +
                    "; A1-A6, P3B not graded there");
                continue;
            }

            std::string t2s_fix_label = std::string("FIXTURE-CONST T2S ") + cs.label +
                ": prog_t2s's own known constant reads back correctly "
                "[not graded -- fixture, no claim uid]";
            if (fixconst_mv) t2s_fix_label += " [NMH_MUTATE demonstrates this fixture red]";
            const bool t2s_fix_ok = cap.fixval.has_value() && *cap.fixval == kFixValue;
            checks.expect(t2s_fix_ok, t2s_fix_label);

            // FIXTURE-WRITE: the A2 result slot was written by the program.
            bool t2s_a2_write = has_one_write(cap.log, kResults / 2);
            {
                std::string wr = std::string("FIXTURE-WRITE T2S ") + cs.label +
                    ": the Chip-memory port shows one CPU write to the A2 result slot "
                    "[not graded -- fixture, no claim uid]";
                if (fixwrite_mv) wr += " [NMH_MUTATE demonstrates this fixture red]";
                checks.expect(t2s_a2_write, wr);
            }

            bool t2s_data_ok = false;
            {
                auto t2sw = t2s_table(t2s_mv ? emv : "");
                t2s_data_ok = all_reads_match(cap.log, T2S_ADDR / 2, t2sw);
                std::string d = std::string("FIXTURE-DATA T2S ") + cs.label +
                    ": every read of T2S on the Chip-memory port returns the poked table content, every word read "
                    "[not graded -- fixture, no claim uid]";
                if (fixdata_mv) d += " [NMH_MUTATE demonstrates this fixture red]";
                checks.expect(t2s_data_ok, d);
            }

            bool use_right = audio_side_is_right(cap.audio);
            checks.note(std::string("audio output side carrying channel 0, T2S ") + cs.label + ": " +
                (use_right ? "audio_right" : "audio_left") +
                " (p.137: right-side jack; Appendix E's pin list, p.326: left-side jack -- a "
                "measured page disagreement; SPECIFICATION.md 2.5 does not grade which named "
                "jack this is)");

            const TableSpec ts2s{"T2S", T2S_ADDR, 2, kNT2S, 4, 1};
            uint32_t lo = T2S_ADDR / 2, hi = lo + 1;   // 2 words
            F2Result f2 = f2_check(cap.log, lo, hi);
            checks.note(first_pass_text("T2S", cs.label, f2, 2));
            // A6: the color-clock unit of the window holds in this run; the
            // window, A1 and everything after them are graded only behind it.
            {
                CalResult cal = calibrate(cap, lo, hi);
                checks.note(std::string("F1 anchor in prog_t2s's own run ") + cs.label + ": mean refresh-group "
                    "spacing " + fmt(cal.cck, 2) + " color clocks over " + std::to_string(cal.groups) +
                    " groups found");
                std::string a6l = std::string("A6 for T2S ") + cs.label +
                    ": the refresh groups of prog_t2s's own audio DMA interval, all of them, recur 227-228 color "
                    "clocks apart on average, so the window's level is 8 clk_sys cycles per color clock of the "
                    "programmed period";
                if (mv == "A6" && std::string(cs.label) == "ECS/PAL") a6l += " [NMH_MUTATE demonstrates this precondition red]";
                checks.expect(cal.ok, a6l);
                if (!cal.ok) {
                    checks.record(std::string("T2S ") + cs.label +
                        ": A1-A5, P3B not graded, precondition A6 failed");
                    continue;
                }
            }
            Window win = make_window(cap.audio, f2, ts2s, use_right);
            bool a1 = f2.anchor_ok && (int)f2.pass_starts.size() >= kNT2S + 3 && win.have &&
                      win.e <= f2.pass_starts.back();
            std::string a1_label = std::string("A1 for T2S ") + cs.label +
                ": audio DMA ran pass after pass from t_2 on, F2 holds with >= " + std::to_string(kNT2S + 3) +
                " reads of T2S's first word, the last of them at or after the end of W_T2S";
            checks.expect(a1, a1_label);
            if (!f2.diag.empty()) checks.note(std::string("T2S ") + cs.label + " F2 break: " + f2.diag);
            if (!a1) {
                checks.record(std::string("T2S ") + cs.label +
                    ": A2-A5, P3B not graded, precondition A1 failed");
                continue;
            }
            // A2 reads a stored slot: graded only when the fixture constant
            // and the slot's write both hold.
            if (!(t2s_fix_ok && t2s_a2_write)) {
                checks.record(std::string("A2 for T2S ") + cs.label +
                    ": not graded, FIXTURE-CONST or FIXTURE-WRITE failed");
                checks.record(std::string("T2S ") + cs.label +
                    ": A3-A5, P3B not graded, precondition A2 not graded");
                continue;
            }
            bool t2s_a2 = cap.adkconr.has_value() && ((*cap.adkconr) & 0x00FF) == 0;
            {
                std::string a2l = std::string("A2 for T2S ") + cs.label +
                    ": ADKCONR bits 7-0 read 0, no attach mode [p.265]";
                if (mv == "A2T") a2l += " [NMH_MUTATE demonstrates this precondition red]";
                checks.expect(t2s_a2, a2l);
            }
            if (!t2s_a2) {
                checks.record(std::string("T2S ") + cs.label +
                    ": A3-A5, P3B not graded, precondition A2 failed");
                continue;
            }
            double median = median_run_len(win.runs);
            bool a3 = median >= 4.0 && median == (double)win.level;
            checks.expect(a3, std::string("A3 for T2S ") + cs.label +
                ": the median run in W_T2S spans >= 4 output samples and equals the programmed held level of " +
                std::to_string(win.level) + " (" + fmt(median, 1) + ")");
            {
                uint64_t shortest = ~0ull, longest = 0;
                for (auto& r : win.runs) { shortest = std::min(shortest, r.len); longest = std::max(longest, r.len); }
                checks.note(std::string("window W_T2S ") + cs.label + " (not graded): " +
                    std::to_string(win.e - win.s) + " samples from the first output transition after t_2, " +
                    std::to_string(kNT2S) + " passes x 4 levels of " + std::to_string(win.level) +
                    " samples programmed (the last one cut); " + std::to_string(win.runs.size()) +
                    " runs found, median " + fmt(median, 1) + ", shortest " +
                    std::to_string(win.runs.empty() ? 0 : shortest) + ", longest " + std::to_string(longest));
            }
            if (!a3) {
                checks.record(std::string("T2S ") + cs.label +
                    ": A4, A5, P3B not graded, precondition A3 failed");
                continue;
            }
            if (!t2s_data_ok) {
                checks.record(std::string("T2S ") + cs.label +
                    ": A4, A5, P3B not graded, FIXTURE-DATA for T2S failed");
                continue;
            }
            auto L = unwrap15(run_values(win.runs));
            FitResult fs = L.empty() ? FitResult{} : fit(L, c2s());
            bool a4 = L.size() >= (size_t)(2 * ts2s.K);
            checks.expect(a4, std::string("A4 for T2S ") + cs.label +
                ": L has >= " + std::to_string(2 * ts2s.K) + " entries (" + std::to_string(L.size()) + ")");
            checks.note(std::string("fitted output change per LSB, T2S ") + cs.label + " (not graded): |b| = " +
                fmt(std::fabs(fs.b), 3) + " raw port counts");
            if (!a4) {
                checks.record(std::string("P3B ") + cs.label + ": not graded, precondition A4 failed");
                continue;
            }
            size_t expected = (size_t)kNT2S * 4;
            bool a5 = L.size() == expected;
            checks.expect(a5, std::string("A5 for T2S ") + cs.label +
                ": the window holds the programmed population of exactly " + std::to_string(expected) +
                " held levels, the last one kept (" + std::to_string(kNT2S) + " passes x 4 codes, " +
                std::to_string(win.level) + " samples per level programmed; " + std::to_string(L.size()) +
                " found)");
            if (!a5) {
                checks.record(std::string("P3B ") + cs.label + ": not graded, precondition A5 failed");
                continue;
            }
            bool p3b = fs.matches;
            std::string p3b_label = std::string("P3B ") + cs.label +
                " [p.265]: each byte is output as a two's-complement value independent "
                "of byte position -- $80 most negative and $7F most positive whichever "
                "byte position carries them, each byte position graded on its own code "
                "(FIT(L,C_sign) matches, s=-128,1,127,0)";
            if (mv == "P3B") p3b_label += " [NMH_MUTATE demonstrates this proposition red]";
            checks.expect(p3b, p3b_label);
        }
    }

    // =====================================================================
    // P5: ADKCON bits 8 (FAST) and 9 (MSBSYNC) read back through ADKCONR
    // =====================================================================
    if (need_adkcon) {
        std::vector<std::pair<const char*, nmh::Config>> configs = {
            {"ECS/PAL", cfg_ecs_pal()}, {"OCS/PAL", cfg_ocs_pal()} };
        for (auto& [label, cfg] : configs) {
            AdkconCapture cap = run_adkcon(dir, kmv, cfg);
            auto w_range = [&](uint32_t off) {
                uint32_t w = (kResults + off) / 2;
                std::vector<uint64_t> v;
                for (auto& a : cap.log) if (a.is_write && a.word_addr == w) v.push_back(a.cycle);
                return v;
            };
            auto w0 = w_range(0), w1 = w_range(2), w2 = w_range(4), w3 = w_range(6), w4 = w_range(8);
            // The fixture grades the run's completion and its known
            // constant only; the writes to R0-R4 are C1's alone (an
            // unwritten result slot still peeks back some value, zero
            // initialisation or a stale one, so a presence check on a
            // peeked word cannot tell a genuine store from an empty slot).
            bool base_ok = cap.loaded && cap.done.has_value();
            checks.expect(base_ok, std::string("FIXTURE ") + label +
                ": prog_adkcon reached DONE [not graded -- fixture, no claim uid]");
            if (!base_ok) { checks.record(std::string("C1/P5 ") + label + ": not graded, fixture failed"); continue; }

            std::string adk_fix_label = std::string("FIXTURE-CONST ") + label +
                ": prog_adkcon's own known constant reads back correctly "
                "[not graded -- fixture, no claim uid]";
            if (kmv == "FIXCONST") adk_fix_label += " [NMH_MUTATE demonstrates this fixture red]";
            bool fix_const_ok = cap.fixval.has_value() && *cap.fixval == kFixValue;
            checks.expect(fix_const_ok, adk_fix_label);
            // C1 and P5 read the stored result slots: graded only behind it.
            if (!fix_const_ok) {
                checks.record(std::string("C1 ") + label + ": not graded, FIXTURE-CONST failed");
                checks.record(std::string("P5 ") + label + ": not graded, FIXTURE-CONST failed");
                continue;
            }

            // The precondition's own, unconditional truth: exactly one write to
            // each of R0-R4, in step order. NMH_MUTATE=C1 leaves out the R2
            // store, so w2 is genuinely empty there and c1 is genuinely false;
            // this is the same unchanged comparator either way, so the mutant
            // run prints FAIL through this clause rather than an inverted PASS.
            bool c1 = w0.size() == 1 && w1.size() == 1 && w2.size() == 1 && w3.size() == 1 && w4.size() == 1 &&
                      w0[0] < w1[0] && w1[0] < w2[0] && w2[0] < w3[0] && w3[0] < w4[0];
            std::string c1_label = std::string("C1 ") + label +
                ": the Chip-memory port shows one CPU write to each of R0-R4, in step order";
            if (kmv == "C1" || kmv == "C1DUP") c1_label += " [NMH_MUTATE demonstrates this precondition red]";
            checks.expect(c1, c1_label);
            if (!c1) {
                checks.record(std::string("P5 ") + label + ": not graded, precondition C1 failed");
                continue;
            }

            auto bits = [&](std::optional<uint16_t> v) {
                return std::make_pair(v ? ((*v >> 9) & 1) : -1, v ? ((*v >> 8) & 1) : -1);
            };
            auto [b9_0, b8_0] = bits(cap.r0);
            auto [b9_1, b8_1] = bits(cap.r1);
            auto [b9_2, b8_2] = bits(cap.r2);
            auto [b9_3, b8_3] = bits(cap.r3);
            auto [b9_4, b8_4] = bits(cap.r4);
            // The unchanged P5 comparator, graded exactly the same way whether or
            // not NMH_MUTATE=P5 is in effect: under P5, step 2 wrote $8200 (sets
            // bit 9, not bit 8), so this same comparator's own R1=(0,1) term is
            // what goes red (measured R1=(1,0)), not a substitute expectation.
            bool p5 = (b9_0==0&&b8_0==0) && (b9_1==0&&b8_1==1) && (b9_2==1&&b8_2==1) &&
                      (b9_3==1&&b8_3==0) && (b9_4==0&&b8_4==0);
            std::string p5_label = std::string("P5 ") + label +
                " [p.265]: bits 8 (FAST) and 9 (MSBSYNC) of ADKCON are separate and ADKCONR "
                "reads back each state (R0=(0,0),R1=(0,1),R2=(1,1),R3=(1,0),R4=(0,0), bit9/bit8) "
                "-- register part only, what the bits do to disk data is not tested "
                "(SPECIFICATION.md 1.2)";
            if (kmv == "P5") p5_label += " [NMH_MUTATE demonstrates this proposition red]";
            checks.expect(p5, p5_label);
        }
        checks.record("A-adkcon.b09/5-appA-p265.c20's disk-read-synchronizing function is not tested: "
            "the harness serves no disk image and a test cannot drive the disk read-data input (SPECIFICATION.md 1.2)");
        checks.record("A-adkcon.b08/5-appA-p265.c21's disk data clock rate cannot be checked at this "
            "boundary: nanomig.v has no disk data clock port (the disk side is the SD-card sector "
            "interface), and no page ties a Chip-memory read rate to it -- G1/G2/G3 in P6 measure "
            "that read rate and decline the comparison for this reason (SPECIFICATION.md 1.2); the "
            "\"(fast for MFM, slow for MFM or GCR)\" advisory is separately not graded (advice on use)");
    }

    // =====================================================================
    // P6 / F1 / F3: disk write DMA, FAST = ADKCON bit 8. The program
    // measures the rate at which disk write DMA reads Chip memory, in two
    // runs (FAST = 1 and FAST = 0); G1, G2 and G3 are RECORDED declines of
    // the comparison with the bit-cell time, since no page ties that rate
    // to the bit cell. See SPECIFICATION.md section 3 for the
    // propositions and section 5 for the measurements.
    // =====================================================================
    if (need_disk) {
        struct CfgSpec { const char* label; nmh::Config cfg; bool ecs; bool ntsc; double us_per_cck; };
        std::vector<CfgSpec> configs = {
            {"ECS/PAL", cfg_ecs_pal(), true, false, kUsPerCckPal},
            {"OCS/PAL", cfg_ocs_pal(), false, false, kUsPerCckPal},
            {"ECS/NTSC", cfg_ecs_ntsc(), true, true, kUsPerCckNtsc},
        };
        for (auto& cs : configs) {
            if (mv == "F1" && !cs.ecs) continue;   // F1/B4's contrasting case is ECS only (2.4)
            std::string suffix = "";
            if (mv == "FASTF0") suffix = "_fastf0";
            else if (mv == "FASTS1") suffix = "_fasts1";
            else if (mv == "B2") suffix = "_b2";
            else if (mv == "B3") suffix = "_b3";
            else if (mv == "F1") suffix = cs.ntsc ? "_f1mut_ntsc" : "_f1mut";
            else if (mv == "FIXCONST") suffix = "_fixturemut";
            else if (mv == "FIXWRITE") suffix = "_fixwritemut";

            DiskCapture cap = run_diskrate(dir, suffix, cs.cfg, fixdata_mv);
            bool base_ok = cap.loaded && cap.done.has_value();
            checks.expect(base_ok, std::string("FIXTURE ") + cs.label +
                ": prog_diskrate reached DONE [not graded -- fixture, no claim uid]");
            if (!base_ok) { checks.record(std::string("P6 ") + cs.label + ": not graded, fixture failed"); continue; }

            std::string disk_fix_label = std::string("FIXTURE-CONST ") + cs.label +
                ": prog_diskrate's own known constant reads back correctly "
                "[not graded -- fixture, no claim uid]";
            if (fixconst_mv) disk_fix_label += " [NMH_MUTATE demonstrates this fixture red]";
            bool fix_const_ok = cap.fixval.has_value() && *cap.fixval == kFixValue;
            checks.expect(fix_const_ok, disk_fix_label);

            // The preloaded disk buffers (poked with $AAAA in every word,
            // p.249/p.250) asserted on the RAM port before use, not
            // merely trusted from poke_word(): every read of a buffer's
            // words returns the content expected there. No graded line of
            // this program reads buffer content, so no line is graded
            // behind these two.
            std::vector<uint16_t> disk_expected(544, kDiskWord);
            std::string da = std::string("FIXTURE-DATA buffer-A ") + cs.label +
                ": every read of buffer A on the Chip-memory port returns the poked $AAAA content, at least one read "
                "[not graded -- fixture, no claim uid]";
            std::string db = std::string("FIXTURE-DATA buffer-B ") + cs.label +
                ": every read of buffer B on the Chip-memory port returns the poked $AAAA content, at least one read "
                "[not graded -- fixture, no claim uid]";
            if (fixdata_mv) {
                da += " [NMH_MUTATE demonstrates this fixture red]";
                db += " [NMH_MUTATE demonstrates this fixture red]";
            }
            checks.expect(all_reads_match(cap.log, DISKBUF_A / 2, disk_expected, false), da);
            checks.expect(all_reads_match(cap.log, DISKBUF_B / 2, disk_expected, false), db);

            // FIXTURE-WRITE: each stored result slot was written by the
            // program; B1 and B2 of a run are graded only behind their own
            // slot's line and FIXTURE-CONST.
            uint32_t w_b1f = (kResults + 0) / 2, w_b2f = (kResults + 2) / 2;
            uint32_t w_b1s = (kResults + 6) / 2, w_b2s = (kResults + 8) / 2;
            bool wr_b1f = has_one_write(cap.log, w_b1f), wr_b2f = has_one_write(cap.log, w_b2f);
            bool wr_b1s = has_one_write(cap.log, w_b1s), wr_b2s = has_one_write(cap.log, w_b2s);
            {
                std::string wb1f = std::string("FIXTURE-WRITE ") + cs.label +
                    ": the Chip-memory port shows one CPU write to the B1-for-run-F result slot "
                    "[not graded -- fixture, no claim uid]";
                if (fixwrite_mv) wb1f += " [NMH_MUTATE demonstrates this fixture red]";
                checks.expect(wr_b1f, wb1f);
                checks.expect(wr_b2f, std::string("FIXTURE-WRITE ") + cs.label +
                    ": the Chip-memory port shows one CPU write to the B2-for-run-F result slot "
                    "[not graded -- fixture, no claim uid]");
                checks.expect(wr_b1s, std::string("FIXTURE-WRITE ") + cs.label +
                    ": the Chip-memory port shows one CPU write to the B1-for-run-S result slot "
                    "[not graded -- fixture, no claim uid]");
                checks.expect(wr_b2s, std::string("FIXTURE-WRITE ") + cs.label +
                    ": the Chip-memory port shows one CPU write to the B2-for-run-S result slot "
                    "[not graded -- fixture, no claim uid]");
            }

            // ---- preconditions: each is its own PASS/FAIL line, printed as
            // measured. A mutation deliberately breaks exactly one of these
            // (B1 for run F under FASTF0, B1 for run S under FASTS1, B2/B3
            // under their own labels, B4 under F1); every other precondition
            // is expected to still hold.
            // The stored slots are only read when the constant read back
            // (run_diskrate); a verdict computed from an unread slot is
            // never printed (graded_stored below).
            const uint16_t b1_f_v = cap.b1_f.value_or(0), b1_s_v = cap.b1_s.value_or(0);
            const uint16_t b2_f_v = cap.b2_f.value_or(0), b2_s_v = cap.b2_s.value_or(0);
            bool fast_f_bit = ((b1_f_v >> 8) & 1) != 0;
            bool fast_s_bit = ((b1_s_v >> 8) & 1) != 0;
            bool b2_f = ((b2_f_v >> 14) & 1) == 1 && ((b2_f_v >> 13) & 1) == 1;
            bool b2_s = ((b2_s_v >> 14) & 1) == 1 && ((b2_s_v >> 13) & 1) == 1;

            F3Result f3f = f3_check(cap.log, DISKBUF_A / 2, 544);
            F3Result f3s = f3_check(cap.log, DISKBUF_B / 2, 544);
            bool b3_f = f3f.anchor_ok && f3f.times.size() >= 529;
            bool b3_s = f3s.anchor_ok && f3s.times.size() >= 529;

            // B4 (F1 anchor), used by P6 (2.4): measured inside each run's
            // own graded interval (F3, read k=16 to k=528), not the boot
            // phase that precedes it.
            if (f3f.times.size() > 528) {
                F1Result f1rf = f1_check(cap.log_raw, f3f.times[16], f3f.times[528]);
                bool b4_f = f1rf.anchor_ok;
                checks.note(std::string("F1 anchor for run F ") + cs.label + ": mean refresh-group "
                    "spacing " + fmt(f1rf.color_clocks, 2) + " color clocks over " +
                    std::to_string(f1rf.groups) + " groups found");
                std::string b4f_label = std::string("B4 for run F ") + cs.label +
                    ": refresh groups recur 227-228 color clocks apart, measured in run F's own "
                    "P6 interval";
                if (mv == "F1") b4f_label += " [NMH_MUTATE demonstrates this precondition red]";
                checks.expect(b4_f, b4f_label);
            } else {
                checks.record(std::string("B4 for run F ") + cs.label + ": not graded, B3 for run F failed");
            }
            if (f3s.times.size() > 528) {
                F1Result f1rs = f1_check(cap.log_raw, f3s.times[16], f3s.times[528]);
                bool b4_s = f1rs.anchor_ok;
                checks.note(std::string("F1 anchor for run S ") + cs.label + ": mean refresh-group "
                    "spacing " + fmt(f1rs.color_clocks, 2) + " color clocks over " +
                    std::to_string(f1rs.groups) + " groups found");
                std::string b4s_label = std::string("B4 for run S ") + cs.label +
                    ": refresh groups recur 227-228 color clocks apart, measured in run S's own "
                    "P6 interval";
                if (mv == "F1") b4s_label += " [NMH_MUTATE demonstrates this precondition red]";
                checks.expect(b4_s, b4s_label);
            } else {
                checks.record(std::string("B4 for run S ") + cs.label + ": not graded, B3 for run S failed");
            }

            // B1 for run F: the unchanged comparator ("ADKCONR bit 8 == 1",
            // run F's own intended setting) either way. Under FASTF0, run
            // F's ADKCON write itself becomes $0100 (bit 8 cleared), so
            // fast_f_bit measures 0 and this clause prints FAIL through
            // the same label, not a redefinition graded to PASS.
            // B1 and B2 read stored result slots: each is graded only when
            // FIXTURE-CONST and the slot's own FIXTURE-WRITE line hold.
            auto graded_stored = [&](bool slot_written, bool verdict, const std::string& label) {
                if (fix_const_ok && slot_written) {
                    checks.expect(verdict, label);
                } else {
                    checks.record(label + " -- not graded, FIXTURE-CONST or FIXTURE-WRITE failed");
                }
            };
            {
                std::string b1f_label = std::string("B1 for run F ") + cs.label + ": ADKCONR bit 8 == 1";
                if (mv == "FASTF0") b1f_label += " [NMH_MUTATE demonstrates this precondition red]";
                graded_stored(wr_b1f, fast_f_bit, b1f_label);
            }
            {
                std::string b2f_label = std::string("B2 for run F ") + cs.label +
                    ": DSKBYTR shows DMAON=1, DISKWRITE=1";
                if (mv == "B2") b2f_label += " [NMH_MUTATE demonstrates this precondition red]";
                graded_stored(wr_b2f, b2_f, b2f_label);
            }
            {
                std::string b3f_label = std::string("B3 for run F ") + cs.label +
                    ": disk DMA reads of buffer A, k=0..528 or more, one per k, in order (" +
                    std::to_string(f3f.times.size()) + " found)";
                if (mv == "B3") b3f_label += " [NMH_MUTATE demonstrates this precondition red]";
                else if (mv == "B2") b3f_label += " [B2's own side effect: clearing DMACON's master "
                    "DMA enable (bit 9) stops all disk DMA, not just DMAON's readback]";
                checks.expect(b3_f, b3f_label);
            }
            // B1 for run S: the same unchanged comparator ("ADKCONR bit 8 ==
            // 0"). Under FASTS1, run S's ADKCON write becomes $8100 (bit 8
            // set), so fast_s_bit measures 1 and this clause prints FAIL.
            {
                std::string b1s_label = std::string("B1 for run S ") + cs.label + ": ADKCONR bit 8 == 0";
                if (mv == "FASTS1") b1s_label += " [NMH_MUTATE demonstrates this precondition red]";
                graded_stored(wr_b1s, !fast_s_bit, b1s_label);
            }
            graded_stored(wr_b2s, b2_s, std::string("B2 for run S ") + cs.label + ": DSKBYTR shows DMAON=1, DISKWRITE=1");
            checks.expect(b3_s, std::string("B3 for run S ") + cs.label +
                ": disk DMA reads of buffer B, k=0..528 or more, one per k, in order (" +
                std::to_string(f3s.times.size()) + " found)");

            if (mv == "B2" || mv == "B3") {
                checks.record(std::string("G1 ") + cs.label + ": not graded under this precondition's own demonstration (B2/B3)");
                checks.record(std::string("G2 ") + cs.label + ": not graded under this precondition's own demonstration (B2/B3)");
                checks.record(std::string("G3 ") + cs.label + ": not graded under this precondition's own demonstration (B2/B3)");
                continue;
            }

            // ---- G1, G2, G3: RECORDED declines (SPECIFICATION.md 1.2, 3),
            // never graded; D_F and D_S are printed as MEASURED lines.
            bool have_df = f3f.times.size() > 528;
            bool have_ds = f3s.times.size() > 528;
            if (!have_df && !have_ds) {
                checks.record(std::string("G1/G2/G3 ") + cs.label + ": not graded, B3 did not hold for either run");
                continue;
            }

            double d_f = have_df ? (double)(f3f.times[528] - f3f.times[16]) : 0.0;
            double d_s = have_ds ? (double)(f3s.times[528] - f3s.times[16]) : 0.0;
            double t_f_us = (d_f / (double)kCck) * cs.us_per_cck / (16.0 * 512.0);
            double t_s_us = (d_s / (double)kCck) * cs.us_per_cck / (16.0 * 512.0);
            checks.note(std::string("disk rate ") + cs.label + ": D_F=" + fmt(d_f, 0) + " cycles, D_S=" +
                fmt(d_s, 0) + " cycles, T_F=" + fmt(t_f_us, 3) + " us, T_S=" + fmt(t_s_us, 3) + " us");

            if (have_df) {
                checks.record(std::string("G1 ") + cs.label +
                    " [p.265,p.249]: cannot be checked at this boundary -- the disk data clock is "
                    "not observable at nanomig.v's ports, and no page ties a Chip-memory read rate "
                    "to it (A-adkcon.b08,5-appA-p265.c21 declined, SPECIFICATION.md 1.2)");
            } else {
                checks.record(std::string("G1 ") + cs.label + ": not graded, B3 for run F failed");
            }

            if (have_ds) {
                checks.record(std::string("G2 ") + cs.label +
                    " [p.265,p.249]: cannot be checked at this boundary -- the disk data clock is "
                    "not observable at nanomig.v's ports, and no page ties a Chip-memory read rate "
                    "to it (A-adkcon.b08,5-appA-p265.c21 declined, SPECIFICATION.md 1.2)");
            } else {
                checks.record(std::string("G2 ") + cs.label + ": not graded, B3 for run S failed");
            }

            if (have_df && have_ds) {
                checks.record(std::string("G3 ") + cs.label +
                    " [p.265,p.249]: cannot be checked at this boundary -- the disk data clock is "
                    "not observable at nanomig.v's ports, and no page ties a Chip-memory read rate "
                    "to it (SPECIFICATION.md 1.2, 3)");
            } else {
                checks.record(std::string("G3 ") + cs.label + ": not graded, B3 failed for a run");
            }
        }
    }

    // ---- claims not turned into a proposition (SPECIFICATION.md 1.1) -----
    if (mv.empty()) {
        checks.record("5-appC-p310.c5, 5-appC-p310.c6, 5-appC-p310.c7, 5-appC-p310.c8, "
            "C-interp.per.c1, C-interp.per.c2, C-interp.per.c3, C-interp.per.c4 (p.310, AUDxPER "
            "and the programmable scan rate): no value or rule the page gives for any scan rate's "
            "maximum period, the one directional statement is hedged (\"Generally\"), AUDxPER is "
            "write-only on every page that lists it so \"the maximum value read from this "
            "register\" cannot be read, and c3 adds a sentence (\"holds for the standard chip set "
            "only\") that appears on neither p.310 nor p.140 -- not testable at these ports "
            "(SPECIFICATION.md 1.1)");
        checks.record("5-p158.c37,5-scale.c13 (p.158, the 7 KHz cut-off filter): pp.155/156/158 give "
            "three different frequencies for the same low-pass filter and no attenuation at any stated "
            "frequency, and the filter sits outside the chips whose output this test samples "
            "(SPECIFICATION.md 1.1) -- not testable at these ports");
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
