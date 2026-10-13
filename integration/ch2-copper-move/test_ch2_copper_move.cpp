// test_ch2_copper_move.cpp -- ch2-copper-move. See SPECIFICATION.md for every
// proposition, its stimulus, its observation and its expected value; this file
// implements what that document commits to.
//
// NMH_MUTATE=<selector> runs only the proposition that owns the selector, with
// the stimulus change the selector names; `make test` sets none. A record
// names the graded line (the label passed to expect()), the selector, the
// change of the stimulus, and what each core printed under it.
//
// MUTATION P1-RUN stimulus red NMH_MUTATE=P1-RUN: the program is prog_nodone, the baseline and nothing else, without the DONE write; TG68K FAIL (1 passed/1 failed; also NOT GRADED P1-PRE-A P1-PRE-B P1-PRE-C P1-COLOUR), fx68k FAIL (1 passed/1 failed; also NOT GRADED P1-PRE-A P1-PRE-B P1-PRE-C P1-COLOUR).
// MUTATION P1-ATTR stimulus red NMH_MUTATE=P1-ATTR: the program reads $30000, a word of the Copper list, before it starts the Copper (prog_p1_attr); TG68K FAIL (4 passed/1 failed; also NOT GRADED P1-COLOUR), fx68k FAIL (4 passed/1 failed; also NOT GRADED P1-COLOUR).
// MUTATION P1-PRE-A stimulus red NMH_MUTATE=P1-A: the Copper starts at an identical copy of the list at $30080 (prog_p1_a) so every pass starts at $30080 and all nine colours still appear; TG68K FAIL (4 passed/1 failed; also NOT GRADED P1-COLOUR), fx68k FAIL (4 passed/1 failed; also NOT GRADED P1-COLOUR).
// MUTATION P1-PRE-B stimulus red NMH_MUTATE=P1-B: sprite DMA is enabled (DMACON <- $8020) with the eight sprite pointers at the zeroed region $4E000 (prog_p1_b) so sprite accesses appear; TG68K FAIL (4 passed/1 failed; also NOT GRADED P1-COLOUR), fx68k FAIL (4 passed/1 failed; also NOT GRADED P1-COLOUR).
// MUTATION P1-PRE-C stimulus red NMH_MUTATE=P1-C: the WAIT at +20 of the list is $5001 instead of $6001 so the (6,6,6) band has no line; TG68K FAIL (4 passed/1 failed; also NOT GRADED P1-COLOUR), fx68k FAIL (4 passed/1 failed; also NOT GRADED P1-COLOUR).
// MUTATION P1-COLOUR stimulus red NMH_MUTATE=P1-MAIN: IR2 at +34 of the list is $0F00 instead of $0F80 so the (F,8,0) band is (F,0,0); all preconditions still hold; TG68K FAIL (5 passed/1 failed), fx68k FAIL (5 passed/1 failed).
// MUTATION P1-COLOUR stimulus red NMH_MUTATE=P1-DA: IR1 at +32 of the list is $0182 (COLOR01) instead of $0180 so the (F,8,0) band is not drawn; all preconditions still hold; TG68K FAIL (5 passed/1 failed), fx68k FAIL (5 passed/1 failed).
// MUTATION P2-RUN stimulus red NMH_MUTATE=P2-RUN: the program is prog_nodone, the baseline and nothing else, without the DONE write; TG68K FAIL (1 passed/1 failed; also NOT GRADED P2-PRE-A P2-PRE-B P2-RESTART), fx68k FAIL (1 passed/1 failed; also NOT GRADED P2-PRE-A P2-PRE-B P2-RESTART).
// MUTATION P2-ATTR stimulus red NMH_MUTATE=P2-ATTR: the program reads $34000, a word of the first segment, before it starts the Copper (prog_p2_attr); TG68K FAIL (3 passed/1 failed; also NOT GRADED P2-RESTART), fx68k FAIL (3 passed/1 failed; also NOT GRADED P2-RESTART).
// MUTATION P2-PRE-A stimulus red NMH_MUTATE=P2-A: the segment at $15554 holds its end WAIT first and the two MOVEs after it so the MOVEs are not read; TG68K FAIL (3 passed/1 failed; also NOT GRADED P2-RESTART), fx68k FAIL (3 passed/1 failed; also NOT GRADED P2-RESTART).
// MUTATION P2-PRE-B stimulus red NMH_MUTATE=P2-B: the segment at $13332 holds a WAIT for line $F0 in front of its end WAIT, so the Copper reads $1333E and $13340 before the next restart; TG68K FAIL (3 passed/1 failed; also NOT GRADED P2-RESTART), fx68k FAIL (3 passed/1 failed; also NOT GRADED P2-RESTART).
// MUTATION P2-RESTART stimulus red NMH_MUTATE=P2-MAIN: segment 3 writes $FF80 instead of $FF00 and a segment at $1FF80 continues the chain; all preconditions still hold; TG68K FAIL (4 passed/1 failed), fx68k FAIL (4 passed/1 failed).
// MUTATION P3-RUN stimulus red NMH_MUTATE=P3-RUN: the program is prog_nodone, the baseline and nothing else, without the DONE write; TG68K FAIL (1 passed/1 failed; also NOT GRADED P3-PRE-A P3-PRE-B P3-MOVED), fx68k FAIL (1 passed/1 failed; also NOT GRADED P3-PRE-A P3-PRE-B P3-MOVED).
// MUTATION P3-ATTR stimulus red NMH_MUTATE=P3-ATTR: the program reads the IR2 word $3600A before it starts the Copper (prog_p3_attr); TG68K FAIL (3 passed/1 failed; also NOT GRADED P3-MOVED), fx68k FAIL (3 passed/1 failed; also NOT GRADED P3-MOVED).
// MUTATION P3-PRE-A stimulus red NMH_MUTATE=P3-A: the four value writes follow each other within one field without a wait for the beam (prog_p3_fast); TG68K FAIL (3 passed/1 failed; also NOT GRADED P3-MOVED), fx68k FAIL (3 passed/1 failed; also NOT GRADED P3-MOVED).
// MUTATION P3-PRE-B stimulus red NMH_MUTATE=P3-B: the Copper starts at a list at $36080 (prog_p3_b) that reads no IR2 word and loads COP1LC with $36000, so the first pass has no read of $3600A and every value write is still followed by a read; TG68K FAIL (3 passed/1 failed; also NOT GRADED P3-MOVED), fx68k FAIL (3 passed/1 failed; also NOT GRADED P3-MOVED).
// MUTATION P3-MOVED stimulus red NMH_MUTATE=P3-MAIN: the CPU writes the four values to the spare word $36010 instead of the IR2 word $3600A (prog_p3_dest); clause (iii); TG68K FAIL (4 passed/1 failed), fx68k FAIL (4 passed/1 failed).
// MUTATION P3-MOVED stimulus red NMH_MUTATE=P3-II: a MOVE of $0F00 to COLOR00 stands behind a WAIT for line $C0 at +12 of the list, so the band rows after line $C0 are (F,0,0) while every read and every moved value is unchanged; clause (ii), all preconditions still hold; TG68K FAIL (4 passed/1 failed), fx68k FAIL (4 passed/1 failed).
// MUTATION P4-RUN stimulus red NMH_MUTATE=P4-RUN: the program is prog_nodone, the baseline and nothing else, without the DONE write; TG68K FAIL (1 passed/1 failed; also NOT GRADED P4-PRE-A P4-PRE-B P4-MOVE-AFTER-WAIT), fx68k FAIL (1 passed/1 failed; also NOT GRADED P4-PRE-A P4-PRE-B P4-MOVE-AFTER-WAIT).
// MUTATION P4-ATTR stimulus red NMH_MUTATE=P4-ATTR: the program reads $38000, a word of the Copper list, before it starts the Copper (prog_p4_attr); TG68K FAIL (3 passed/1 failed; also NOT GRADED P4-MOVE-AFTER-WAIT), fx68k FAIL (3 passed/1 failed; also NOT GRADED P4-MOVE-AFTER-WAIT).
// MUTATION P4-PRE-A stimulus red NMH_MUTATE=P4-A: the WAIT at +4 of the list is $FFFF,$FFFE so $3800C is never read; TG68K FAIL (3 passed/1 failed; also NOT GRADED P4-MOVE-AFTER-WAIT), fx68k FAIL (3 passed/1 failed; also NOT GRADED P4-MOVE-AFTER-WAIT).
// MUTATION P4-PRE-B stimulus red NMH_MUTATE=P4-B: the Copper starts at $38004 (prog_p4_b) so every pass begins at $38004; TG68K FAIL (3 passed/1 failed; also NOT GRADED P4-MOVE-AFTER-WAIT), fx68k FAIL (3 passed/1 failed; also NOT GRADED P4-MOVE-AFTER-WAIT).
// MUTATION P4-MOVE-AFTER-WAIT stimulus red NMH_MUTATE=P4-MAIN: the WAIT at +12 is $C001,$FF00 instead of $FFFF,$FFFE so MOVE B is performed at line $C0; all preconditions still hold; TG68K FAIL (4 passed/1 failed), fx68k FAIL (4 passed/1 failed).
// MUTATION P5-RUN stimulus red NMH_MUTATE=P5-RUN: all three programs are prog_nodone, the baseline and nothing else, without the DONE write; TG68K FAIL (6 passed/6 failed; also NOT GRADED P5-PRE-A P5-PRE-B P5-PRE-C P5-LOCATION P5-LOCATION-R2 P5-LOCATION-R2-CTRL), fx68k FAIL (6 passed/6 failed; also NOT GRADED P5-PRE-A P5-PRE-B P5-PRE-C P5-LOCATION P5-LOCATION-R2 P5-LOCATION-R2-CTRL).
// MUTATION P5-ATTR stimulus red NMH_MUTATE=P5-ATTR: the program of run R1 has a CPU read of $20004, a word of the first list, behind its DONE write (prog_p5_r1_attr); TG68K FAIL (29 passed/2 failed; also NOT GRADED P5-LOCATION P5-LOCATION-R2), fx68k FAIL (29 passed/2 failed; also NOT GRADED P5-LOCATION P5-LOCATION-R2).
// MUTATION P5-ATTR stimulus red NMH_MUTATE=P5-ATTR2: the program of run R2 has a CPU read of $20004, a word of the first list, behind its DONE write (prog_p5_r2_attr); TG68K FAIL (29 passed/2 failed; also NOT GRADED P5-LOCATION P5-LOCATION-R2), fx68k FAIL (29 passed/2 failed; also NOT GRADED P5-LOCATION P5-LOCATION-R2).
// MUTATION P5-PRE-A stimulus red NMH_MUTATE=P5-A: Copper DMA is enabled before markers 2 and 3 in run R1 (prog_p5_r1_a) so Copper accesses fall before marker 2; TG68K FAIL (29 passed/2 failed; also NOT GRADED P5-LOCATION P5-LOCATION-R2), fx68k FAIL (29 passed/2 failed; also NOT GRADED P5-LOCATION P5-LOCATION-R2).
// MUTATION P5-PRE-A stimulus red NMH_MUTATE=P5-A2: Copper DMA is enabled before markers 2 and 3 in run R2 (prog_p5_r2_a) so Copper accesses fall before marker 2; TG68K FAIL (29 passed/2 failed; also NOT GRADED P5-LOCATION P5-LOCATION-R2), fx68k FAIL (29 passed/2 failed; also NOT GRADED P5-LOCATION P5-LOCATION-R2).
// MUTATION P5-PRE-B stimulus red NMH_MUTATE=P5-B: run R1 idles two passages of the beam through line $A0 per phase instead of six (prog_p5_r1_b); TG68K FAIL (29 passed/2 failed; also NOT GRADED P5-LOCATION P5-LOCATION-R2), fx68k FAIL (29 passed/2 failed; also NOT GRADED P5-LOCATION P5-LOCATION-R2).
// MUTATION P5-PRE-B stimulus red NMH_MUTATE=P5-B2: run R2 idles two passages of the beam through line $A0 instead of six (prog_p5_r2_b); TG68K FAIL (29 passed/2 failed; also NOT GRADED P5-LOCATION P5-LOCATION-R2), fx68k FAIL (29 passed/2 failed; also NOT GRADED P5-LOCATION P5-LOCATION-R2).
// MUTATION P5-PRE-C stimulus red NMH_MUTATE=P5-C: the list at $20004 holds $0180,$0F00 in front of the end WAIT so each pass reads four words; TG68K FAIL (24 passed/6 failed; also NOT GRADED P5-LOCATION P5-LOCATION-R2 P5-LOCATION-R2-CTRL), fx68k FAIL (24 passed/6 failed; also NOT GRADED P5-LOCATION P5-LOCATION-R2 P5-LOCATION-R2-CTRL).
// MUTATION P5-LOCATION stimulus red NMH_MUTATE=P5-R1: the halves of $00020004 are written as separate words in the swapped order (prog_p5_r1x, run R1 swapped) so run R1 starts and restarts at $40002; the line grades run R1 alone and is green in the unmutated run; TG68K FAIL (31 passed/2 failed; also FAIL P5-LOCATION-R2), fx68k FAIL (31 passed/2 failed; also FAIL P5-LOCATION-R2).
// MUTATION P5-LOCATION-R2 stimulus green NMH_MUTATE=P5-R2C: the line is red in the unmutated run (run R2, whose COPJMP2 strobe is written with Copper DMA off, starts at COP1LC $20004 and not at COP2LC $25A5C); run R2c (prog_p5_r2c: COP1LC = COP2LC = $25A5C at the COPJMP2 strobe, then COP1LC <- $20004 after the enable marker, no strobe) runs in the place of run R2 through the same comparator -- measured: TG68K PASS (33 passed/0 failed), fx68k PASS (33 passed/0 failed).
// MUTATION P5-LOCATION-R2 stimulus red NMH_MUTATE=P5-R2: run R2 strobes COPJMP1 instead of COPJMP2 (prog_p5_r2x); the line grades run R2 alone and is red in the unmutated run as well, so this run shows no change in this design (a machine that loads the program counter from COP2LC after COPJMP2 starts at $20004 here and fails the comparator); the green control is P5-R2C; TG68K FAIL (32 passed/1 failed), fx68k FAIL (32 passed/1 failed).
// MUTATION P5-LOCATION-R2-CTRL stimulus red NMH_MUTATE=P5-R2CX: run R2c leaves out the COP1LC write after the enable marker (prog_p5_r2cx) so the restarts stay at $25A5C; TG68K FAIL (31 passed/2 failed; also FAIL P5-LOCATION-R2), fx68k FAIL (31 passed/2 failed; also FAIL P5-LOCATION-R2).
// MUTATION P6-RUN stimulus red NMH_MUTATE=P6-RUN: the program is prog_nodone, the baseline and nothing else, without the DONE write; TG68K FAIL (0 passed/3 failed; also NOT GRADED P6-PRE-A P6-PRE-B P6-LONG), fx68k FAIL (0 passed/1 failed; also NOT GRADED P6-PRE-A P6-PRE-B P6-LONG).
// MUTATION P6-PRE-A stimulus red NMH_MUTATE=P6-A: the two word writes before marker 1 are omitted (prog_p6_nopre); TG68K FAIL (6 passed/3 failed; also NOT GRADED P6-LONG), fx68k FAIL (2 passed/1 failed; also NOT GRADED P6-LONG).
// MUTATION P6-PRE-B stimulus red NMH_MUTATE=P6-B: the CPU identification is replaced by the constant 2 (prog_p6_probe2) so the 68000 and 68010 runs store the wrong word; TG68K FAIL (8 passed/2 failed; also NOT GRADED P6-LONG), fx68k FAIL (2 passed/1 failed; also NOT GRADED P6-LONG).
// MUTATION P6-LONG stimulus red NMH_MUTATE=P6-MAIN: the long word is $56781234 instead of $12345678 (prog_p6_value); TG68K FAIL (9 passed/3 failed), fx68k FAIL (3 passed/1 failed).
// MUTATION P7-RUN stimulus red NMH_MUTATE=P7-RUN: both programs are prog_nodone, the baseline and nothing else, without the DONE write; TG68K FAIL (2 passed/2 failed; also NOT GRADED P7-PRE-A P7-PRE-B P7-PRE-C P7-BLIT), fx68k FAIL (2 passed/2 failed; also NOT GRADED P7-PRE-A P7-PRE-B P7-PRE-C P7-BLIT).
// MUTATION P7-ATTR stimulus red NMH_MUTATE=P7-ATTR: the program has a second CPU write to $48000 behind its DONE write (prog_p7_attr); TG68K FAIL (8 passed/2 failed; also NOT GRADED P7-BLIT), fx68k FAIL (8 passed/2 failed; also NOT GRADED P7-BLIT).
// MUTATION P7-PRE-A stimulus red NMH_MUTATE=P7-A: the program enables blitter DMA, writes BLTSIZE itself and idles one field before it starts the Copper (prog_p7_a) so the blit writes $48000 before the Copper reads $3A002; TG68K FAIL (8 passed/2 failed; also NOT GRADED P7-BLIT), fx68k FAIL (8 passed/2 failed; also NOT GRADED P7-BLIT).
// MUTATION P7-PRE-B stimulus red NMH_MUTATE=P7-B: the CPU write of $0000 goes to $4F100 instead of $48000 (prog_p7_b); TG68K FAIL (8 passed/2 failed; also NOT GRADED P7-BLIT), fx68k FAIL (8 passed/2 failed; also NOT GRADED P7-BLIT).
// MUTATION P7-PRE-C stimulus red NMH_MUTATE=P7-C: the Copper starts at $3A004 (prog_p7_c) so it never reads $3A000 and $3A002; TG68K FAIL (8 passed/2 failed; also NOT GRADED P7-BLIT), fx68k FAIL (8 passed/2 failed; also NOT GRADED P7-BLIT).
// MUTATION P7-BLIT stimulus red NMH_MUTATE=P7-MAIN: IR1 at $3A000 is $0180 (COLOR00) instead of $0058 so no blit starts; all preconditions still hold; TG68K FAIL (10 passed/1 failed), fx68k FAIL (10 passed/1 failed).
// MUTATION P7-BLIT stimulus red NMH_MUTATE=P7-W2: the data of the BLTSIZE MOVE at $3A002 is $0042 (width 2) instead of $0041 so the blit writes $48000 and $48002; all preconditions still hold; TG68K FAIL (10 passed/1 failed), fx68k FAIL (10 passed/1 failed).
// MUTATION P7-BLIT stimulus red NMH_MUTATE=P7-LF: BLTCON0 is $0100 (LF = $00) instead of $01FF (prog_p7_lf0) so the blit writes $0000 and the result word is $0000; all preconditions still hold; TG68K FAIL (10 passed/1 failed), fx68k FAIL (10 passed/1 failed).
// MUTATION P8-RUN stimulus red NMH_MUTATE=P8-RUN: both programs are prog_nodone, the baseline and nothing else, without the DONE write; TG68K FAIL (2 passed/2 failed; also NOT GRADED P8-PRE-A P8-PRE-B P8-CTRL P8-RESTART), fx68k FAIL (2 passed/2 failed; also NOT GRADED P8-PRE-A P8-PRE-B P8-CTRL P8-RESTART).
// MUTATION P8-ATTR stimulus red NMH_MUTATE=P8-ATTR: the program of run M has a CPU read of $44000, a word of the audio data block, behind its DONE write (prog_p8_attr); TG68K FAIL (7 passed/1 failed; also NOT GRADED P8-CTRL P8-RESTART), fx68k FAIL (7 passed/1 failed; also NOT GRADED P8-CTRL P8-RESTART).
// MUTATION P8-ATTR stimulus red NMH_MUTATE=P8-CATTR: the program of run C has a CPU read of $44000, a word of the audio data block, behind its DONE write (prog_p8_c_attr); TG68K FAIL (7 passed/1 failed; also NOT GRADED P8-CTRL P8-RESTART), fx68k FAIL (7 passed/1 failed; also NOT GRADED P8-CTRL P8-RESTART).
// MUTATION P8-PRE-A stimulus red NMH_MUTATE=P8-A: run M writes DMACON <- $8200, without AUD0EN (prog_p8_noaud), so the channel fetches nothing; TG68K FAIL (7 passed/1 failed; also NOT GRADED P8-CTRL P8-RESTART), fx68k FAIL (7 passed/1 failed; also NOT GRADED P8-CTRL P8-RESTART).
// MUTATION P8-PRE-A stimulus red NMH_MUTATE=P8-CA: run C writes DMACON <- $8200, without AUD0EN (prog_p8_c_noaud), so the channel fetches nothing; TG68K FAIL (7 passed/1 failed; also NOT GRADED P8-CTRL P8-RESTART), fx68k FAIL (7 passed/1 failed; also NOT GRADED P8-CTRL P8-RESTART).
// MUTATION P8-PRE-B stimulus red NMH_MUTATE=P8-B: run M writes DMACON <- $8201 after the AUD0PER write and before marker 1 (prog_p8_early) so the channel fetches before marker 2; TG68K FAIL (7 passed/1 failed; also NOT GRADED P8-CTRL P8-RESTART), fx68k FAIL (7 passed/1 failed; also NOT GRADED P8-CTRL P8-RESTART).
// MUTATION P8-PRE-B stimulus red NMH_MUTATE=P8-CB: run C writes DMACON <- $8201 after the AUD0PER write and before marker 1 (prog_p8_c_early) so the channel fetches before marker 2; TG68K FAIL (7 passed/1 failed; also NOT GRADED P8-CTRL P8-RESTART), fx68k FAIL (7 passed/1 failed; also NOT GRADED P8-CTRL P8-RESTART).
// MUTATION P8-CTRL stimulus red NMH_MUTATE=P8-M: run M has AUD0LEN = 5 (prog_p8_len) so every run after the first has five words; TG68K FAIL (8 passed/2 failed; also FAIL P8-RESTART), fx68k FAIL (8 passed/2 failed; also FAIL P8-RESTART).
// MUTATION P8-CTRL stimulus red NMH_MUTATE=P8-C: run C omits the AUD0LC write and keeps marker W (prog_p8_c_nolc) so the runs after marker W start at $44000 and not at $44100; TG68K FAIL (8 passed/2 failed; also FAIL P8-RESTART), fx68k FAIL (8 passed/2 failed; also FAIL P8-RESTART).
// MUTATION P8-RESTART stimulus red NMH_MUTATE=P8-M: run M has AUD0LEN = 5 (prog_p8_len) so every run has five words, and the line is red in the unmutated run as well because the first run of each audio run has three words (the first run is graded, Section 8 A4), while the control P8-CTRL on runs after the first is green in the unmutated run and red here; TG68K FAIL (8 passed/2 failed; also FAIL P8-CTRL), fx68k FAIL (8 passed/2 failed; also FAIL P8-CTRL).
// MUTATION P8-RESTART stimulus red NMH_MUTATE=P8-C: run C omits the AUD0LC write and keeps marker W (prog_p8_c_nolc) so the runs after marker W start at $44000 and not at $44100; the line is red in the unmutated run as well, and the control P8-CTRL is green in the unmutated run and red here; TG68K FAIL (8 passed/2 failed; also FAIL P8-CTRL), fx68k FAIL (8 passed/2 failed; also FAIL P8-CTRL).
// MUTATION P3-MOVED none impossible clause (i), every Copper read of $3600A returns the last value the CPU wrote there: every write to $3600A that reaches the port is counted as a value write, whichever channel makes it, P3-RUN requires the four writes to carry the four values (whose two bytes differ, so none is a one-byte write), and a RAM word read returns the word last written at its address, so a read differs from the last port write only if the memory loses a written word; no stimulus changes that
// MUTATION P7-BLIT none impossible clause "after the Copper's read of $3A002": P7-PRE-A requires that no blitter access precedes that read, so the clause holds whenever the line is graded; a blit started before the read makes P7-PRE-A red and the line NOT GRADED (P7-A)

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "nanomig_harness.h"
#include "listing_scan.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

constexpr uint32_t kMarker    = 0x04F000;
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint32_t kResults   = 0x0C0000;
constexpr uint32_t kProgOrg   = 0x080400;
constexpr uint64_t kBootBudget = 6'000'000;
// clk_sys cycles per field are measured from the vertical sync of each run;
// this figure only sizes the run budgets (a field is 568,408 cycles in PAL).
constexpr uint64_t kFieldCycles = 620'000;

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

std::string hex(uint32_t v, int digits = 4) {
    char b[16];
    std::snprintf(b, sizeof b, "%0*X", digits, v);
    return std::string("$") + b;
}

std::string num(uint64_t v) { return std::to_string(v); }

using Poke = std::pair<uint32_t, uint16_t>; // byte address, word

// Appends words at consecutive word addresses starting at `base`.
void put(std::vector<Poke>& out, uint32_t base, std::initializer_list<uint16_t> words) {
    for (uint16_t w : words) { out.push_back({base, w}); base += 2; }
}

struct Setup {
    std::string bin;
    std::vector<Poke> pokes;
    nmh::Config cfg;
    uint64_t budget = 12'000'000;
    std::vector<uint32_t> peeks; // byte addresses read back after the run
};

struct Run {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log;
    std::vector<bool> held;
    std::vector<nmh::VideoSample> video;
    std::map<uint32_t, uint16_t> peeked;
};

Run run_program(const Setup& s) {
    Run r;
    nmh::NanomigHarness h(s.cfg);
    r.loaded = h.load_program(s.bin) > 0;
    if (!r.loaded) return r;
    for (const auto& p : s.pokes) h.poke_word(p.first / 2, p.second);
    r.released = h.boot(kBootBudget).cpu_released;
    r.done = h.run_until_write(kDoneAddr, kDoneValue, s.budget);
    r.log = h.ram_log();
    r.video = h.video();
    // The bus period is the smallest spacing between two port samples.
    uint64_t period = 0;
    for (size_t i = 1; i < r.log.size(); ++i) {
        uint64_t d = r.log[i].cycle - r.log[i - 1].cycle;
        if (d > 0 && (period == 0 || d < period)) period = d;
    }
    r.held = nmh::held_continuations(r.log, period);
    for (uint32_t a : s.peeks) r.peeked[a] = h.peek_word(a / 2);
    return r;
}

uint64_t budget_for(int fields) { return uint64_t(fields + 3) * kFieldCycles + 2'000'000; }

// ---- markers ---------------------------------------------------------------

struct Mark { uint64_t cycle; uint16_t value; };

std::vector<Mark> marks(const Run& r) {
    std::vector<Mark> out;
    for (size_t i = 0; i < r.log.size(); ++i) {
        const auto& a = r.log[i];
        if (a.is_write && a.word_addr == kMarker / 2 && !r.held[i]) out.push_back({a.cycle, a.data});
    }
    return out;
}

// The cycle of the first write of marker value `v`; -1 if there is none.
int64_t mark_cycle(const std::vector<Mark>& m, uint16_t v) {
    for (const auto& x : m) if (x.value == v) return int64_t(x.cycle);
    return -1;
}

bool marks_are(const std::vector<Mark>& m, std::initializer_list<uint16_t> want) {
    if (m.size() != want.size()) return false;
    size_t i = 0;
    for (uint16_t w : want) if (m[i++].value != w) return false;
    return true;
}

// ---- Chip-memory port accesses by region --------------------------------------

struct Region { uint32_t lo, hi; }; // byte addresses, inclusive

bool in_regions(uint32_t byte_addr, const std::vector<Region>& regs) {
    for (const auto& g : regs) if (byte_addr >= g.lo && byte_addr <= g.hi) return true;
    return false;
}

struct Acc { uint64_t t; uint32_t addr; uint16_t data; };

constexpr uint64_t kForever = ~0ull;

// Accesses at the port inside `regs` between cycles [t0,t1), a held bus cycle counted once.
std::vector<Acc> accesses_in(const Run& r, const std::vector<Region>& regs, bool writes, uint64_t t0, uint64_t t1) {
    std::vector<Acc> out;
    for (size_t i = 0; i < r.log.size(); ++i) {
        const auto& a = r.log[i];
        if (a.is_write != writes || r.held[i] || a.cycle < t0 || a.cycle >= t1) continue;
        if (in_regions(a.word_addr * 2, regs)) out.push_back({a.cycle, a.word_addr * 2, a.data});
    }
    return out;
}
std::vector<Acc> reads_in(const Run& r, const std::vector<Region>& regs, uint64_t t0 = 0, uint64_t t1 = kForever) {
    return accesses_in(r, regs, false, t0, t1);
}

// The Copper's reads in `regs` between cycles [t0,t1) with the extra read taken out (SPECIFICATION.md Section 3 and
// Section 8, A1): a read at the word directly after an end WAIT's second word ($FFFE), made right after that read.
// The extra reads go to `extra`, and belong to no pass. The extra read is found on the whole run, so a window
// that begins just after a $FFFE read does not mistake it for a restart access.
std::vector<Acc> cop_reads(const Run& r, const std::vector<Region>& regs, uint64_t t0 = 0, uint64_t t1 = kForever,
                           std::vector<Acc>* extra = nullptr) {
    auto all = accesses_in(r, regs, false, 0, kForever);
    std::vector<Acc> out;
    bool prev_extra = false;
    for (size_t i = 0; i < all.size(); ++i) {
        const bool ex = i > 0 && !prev_extra && all[i - 1].data == 0xFFFE && all[i].addr == all[i - 1].addr + 2;
        prev_extra = ex;
        if (all[i].t < t0 || all[i].t >= t1) continue;
        if (ex) { if (extra) extra->push_back(all[i]); }
        else out.push_back(all[i]);
    }
    return out;
}

// "3 at $30048" / "none": the extra reads, counted per address.
std::string extra_text(const std::vector<Acc>& v) {
    if (v.empty()) return "none";
    std::map<uint32_t, size_t> n;
    for (const auto& x : v) ++n[x.addr];
    std::string t;
    for (const auto& kv : n) t += (t.empty() ? "" : ", ") + num(kv.second) + " at " + hex(kv.first, 5);
    return t;
}
std::vector<Acc> writes_in(const Run& r, const std::vector<Region>& regs, uint64_t t0 = 0, uint64_t t1 = kForever) {
    return accesses_in(r, regs, true, t0, t1);
}

// A pass (SPECIFICATION.md Section 3): the run of Copper reads from one restart
// access up to and including the read of an end WAIT's second word ($FFFE). The
// restart access is the first Copper access after that read that is not the
// extra read (cop_reads takes the extra read out of the stream). `complete` is
// false for the run that the end of the observation interval cuts off.
struct Pass { size_t first, last; bool complete; };

std::vector<Pass> passes_of(const std::vector<Acc>& v) {
    std::vector<Pass> out;
    size_t start = 0;
    for (size_t i = 0; i < v.size(); ++i) {
        if (v[i].data == 0xFFFE) { out.push_back({start, i, true}); start = i + 1; }
    }
    if (start < v.size()) out.push_back({start, v.size() - 1, false});
    return out;
}

// ---- video -------------------------------------------------------------------

// A field is the output between two consecutive VSYNC leading edges; only the
// fields that begin at or after cycle `t` and end before the capture does.
struct Fld { size_t begin, end; };

std::vector<Fld> complete_fields_after(const Run& r, uint64_t t) {
    std::vector<Fld> out;
    auto fr = nmh::find_frames(r.video);
    for (size_t i = 1; i + 1 < fr.size(); ++i)
        if (fr[i].begin >= t) out.push_back({fr[i].begin, fr[i].end});
    return out;
}

// A row sample: the pixel halfway between two consecutive HSYNC leading edges
// inside the field.
struct Row { size_t idx; uint16_t rgb; }; // rgb = R<<8 | G<<4 | B

std::vector<Row> rows_of(const Run& r, const Fld& f) {
    std::vector<Row> out;
    long prev_edge = -1;
    bool prev = r.video[f.begin].hs;
    for (size_t i = f.begin + 1; i < f.end; ++i) {
        bool hs = r.video[i].hs;
        if (hs && !prev) {
            if (prev_edge >= 0) {
                size_t mid = (size_t(prev_edge) + i) / 2;
                const auto& v = r.video[mid];
                out.push_back({mid, uint16_t((v.r << 8) | (v.g << 4) | v.b)});
            }
            prev_edge = long(i);
        }
        prev = hs;
    }
    return out;
}

// The run sequence of a field: its non-black row samples, equal neighbours collapsed.
std::vector<uint16_t> run_sequence(const std::vector<Row>& rows) {
    std::vector<uint16_t> out;
    for (const auto& x : rows) {
        if (x.rgb == 0) continue;
        if (out.empty() || out.back() != x.rgb) out.push_back(x.rgb);
    }
    return out;
}

std::string seq_text(const std::vector<uint16_t>& s) {
    std::string t;
    for (uint16_t c : s) { char b[8]; std::snprintf(b, sizeof b, "%03X ", c); t += b; }
    return t;
}

// The length of a colour clock in clk_sys cycles, calibrated on the run's own video: the doubled
// output has two rows per Amiga line, and a PAL line is 227 colour clocks long (p.23). 0 if the
// spacing of the rows is not a whole number of cycles per colour clock.
uint64_t cck_cycles(const Run& r) {
    std::vector<uint64_t> gaps;
    long prev = -1;
    bool hs = false;
    for (size_t i = 0; i < r.video.size(); ++i) {
        const bool now = r.video[i].hs;
        if (now && !hs) {
            if (prev >= 0) gaps.push_back(uint64_t(long(i) - prev));
            prev = long(i);
        }
        hs = now;
    }
    if (gaps.size() < 100) return 0;
    std::sort(gaps.begin(), gaps.end());
    const uint64_t row = gaps[gaps.size() / 2];
    const uint64_t line = 2 * row;
    return line % 227 == 0 ? line / 227 : 0;
}

// ---- gating ------------------------------------------------------------------

std::string first_word(const std::string& label) {
    size_t e = label.find_first_of(": ");
    return label.substr(0, e);
}

// A precondition that fails makes every line that depends on it NOT GRADED.
struct Gate {
    bool ok = true;
    bool skipped = false; // the run itself failed: the preconditions that read its capture are NOT GRADED too
    std::string unmet;
    // The fixture: the program ran, wrote its markers and left a capture worth reading.
    void run(Checks& c, bool cond, const std::string& label) {
        c.expect(cond, label);
        if (!cond) { ok = false; skipped = true; unmet += " " + first_word(label); }
    }
    // A precondition that reads the static listing only.
    void stat(Checks& c, bool cond, const std::string& label) {
        c.expect(cond, label);
        if (!cond) { ok = false; unmet += " " + first_word(label); }
    }
    // A precondition that reads the capture.
    void pre(Checks& c, bool cond, const std::string& label) {
        if (skipped) {
            const size_t b = label.rfind(" [");
            c.record("NOT GRADED " + first_word(label) + (b == std::string::npos ? std::string() : label.substr(b)) +
                     ": the run did not produce a capture to read");
            ok = false;
            return;
        }
        c.expect(cond, label);
        if (!cond) { ok = false; unmet += " " + first_word(label); }
    }
    // The line `id` is behind this gate and one of its preconditions does not hold.
    void skip(Checks& c, const std::string& id) const {
        c.record("NOT GRADED " + id + ": the precondition(s)" + unmet + " do not hold");
    }
};

std::vector<lscan::Guard> parked_guards() {
    return {{0x4C000, 0x4CFFF}, {0x4D000, 0x4DFFF}, {0x4E000, 0x4E0FF}, {0x44200, 0x442FF}};
}

// The CPU program's listing shows no data access beyond what each guard allows.
bool listing_ok(const std::string& bin, std::vector<lscan::Guard> guards, std::string& why) {
    for (const auto& g : parked_guards()) guards.push_back(g);
    return lscan::guards_hold(bin, guards, kProgOrg, why);
}

bool dbg() { return std::getenv("NMH_DEBUG") != nullptr; }

// The claim ids of the three tables that print "bit n of IR2 carries RDn"
// (p.36 twice, p.274/275) and of the p.274/275 field list.
std::string rd_ids(int bit) {
    const std::string a = std::to_string(2 + 6 * (15 - bit));
    const std::string b = std::to_string(15 + 6 * (15 - bit));
    return "ch2-summary-of-copper-instructions.c" + a + " COP2-summary-of-copper-instructions.c" + a +
           " COP2-copins-encoding.c" + a + " ch2-appa-copins.c" + b;
}
std::string rd_ids(int hi, int lo) {
    std::string s;
    for (int b = hi; b >= lo; --b) s += (s.empty() ? "" : " ") + rd_ids(b);
    return s;
}

} // namespace


// =============================================================================
// P1. A Copper MOVE writes its second word into the register its first word
// names: for COLOR00, bits 11-0 appear bit for bit as the displayed colour.
// =============================================================================
namespace p1 {

constexpr uint32_t kList = 0x30000;
constexpr uint16_t kColours[9] = {0x0FFF, 0x0555, 0x0666, 0x0878, 0x0F80, 0x0AAA, 0x0999, 0x0787, 0x007F};

std::vector<Poke> list_words(const std::string& mv, uint32_t base = kList) {
    std::vector<Poke> pk;
    const uint16_t waits[8] = {0x4001, 0x5001, 0x6001, 0x7001, 0x8001, 0x9001, 0xA001, 0xB001};
    uint32_t a = base;
    for (int i = 0; i < 9; ++i) {
        uint16_t ir1 = 0x0180;
        uint16_t ir2 = kColours[i];
        if (mv == "P1-MAIN" && i == 4) ir2 = 0x0F00;
        if (mv == "P1-DA" && i == 4) ir1 = 0x0182;
        put(pk, a, {ir1, ir2});
        a += 4;
        if (i < 8) {
            uint16_t w = waits[i];
            if (mv == "P1-C" && i == 2) w = 0x5001;
            put(pk, a, {w, 0xFF00});
            a += 4;
        }
    }
    put(pk, a, {0xFFFF, 0xFFFE});
    return pk;
}

// The words of the list in address order, as the stimulus stores them.
std::vector<uint16_t> list_image(const std::string& mv) {
    std::vector<Poke> pk = list_words(mv);
    std::vector<uint16_t> w;
    for (const auto& p : pk) w.push_back(p.second);
    return w;
}

void run(Checks& checks, const std::string& dir, const std::string& mv) {
    std::string bin = dir + "/prog_p1.bin";
    if (mv == "P1-A") bin = dir + "/prog_p1_a.bin";
    if (mv == "P1-B") bin = dir + "/prog_p1_b.bin";
    if (mv == "P1-ATTR") bin = dir + "/prog_p1_attr.bin";
    if (mv == "P1-RUN") bin = dir + "/prog_nodone.bin";
    Setup s;
    s.bin = bin;
    s.pokes = list_words(mv);
    // P1-A starts the Copper at an identical copy of the list at $30080.
    if (mv == "P1-A") { auto cp = list_words(mv, kList + 0x80); s.pokes.insert(s.pokes.end(), cp.begin(), cp.end()); }
    s.budget = budget_for(9);
    Run r = run_program(s);
    auto m = marks(r);
    const int64_t en = mark_cycle(m, 3);
    auto cf = en >= 0 ? complete_fields_after(r, uint64_t(en)) : std::vector<Fld>{};
    const std::vector<Region> cop = {{kList, kList + 0xFF}};
    Gate g;

    g.run(checks, r.loaded && r.released && r.done && marks_are(m, {0, 1, 2, 3}) && cf.size() >= 6,
          "P1-RUN: the program ran to its DONE mark, wrote markers 0, 1, 2 and 3 once each in order, and the capture holds "
          "at least six complete fields after the enable marker (" + num(cf.size()) + ")");
    std::string why;
    const bool attr_ok = listing_ok(bin, {{kList, kList + 0xFF, 0, 0}}, why);
    g.stat(checks, attr_ok,
          "P1-ATTR: the program's listing shows no CPU data access to the Copper list region " + hex(kList, 5) + "-" +
          hex(kList + 0xFF, 5) + " or to a parked region of another channel" + (attr_ok ? std::string() : " (" + why + ")"));

    // The observed fields are the 3rd to 6th complete fields after the enable marker.
    std::vector<Acc> rd, extra;
    std::vector<Pass> ps;
    if (cf.size() >= 6) {
        rd = cop_reads(r, cop, uint64_t(en), cf[5].end, &extra);
        ps = passes_of(rd);
    }
    checks.note("P1 extra reads (the word after the end WAIT, in no pass) in the observed interval: " + extra_text(extra));
    // (a) one complete pass ends in each observed field, starts at the list and reads its words in order.
    const std::vector<uint16_t> image = list_image(mv);
    auto pass_is_list = [&](const std::vector<Acc>& v, const Pass& p) {
        if (p.last - p.first + 1 != image.size()) return false;
        for (size_t k = 0; k < image.size(); ++k)
            if (v[p.first + k].addr != kList + 2 * k || v[p.first + k].data != image[k]) return false;
        return true;
    };
    bool a_ok = cf.size() >= 6;
    std::string a_why;
    for (size_t f = 2; f < 6 && f < cf.size(); ++f) {
        int found = 0;
        for (const auto& p : ps) {
            if (!p.complete) continue;
            const uint64_t t = rd[p.last].t;
            if (t < cf[f].begin || t >= cf[f].end) continue;
            ++found;
            if (!pass_is_list(rd, p)) {
                a_ok = false;
                a_why += " field " + num(f + 1) + " pass starts at " + hex(rd[p.first].addr, 5) + " and holds " +
                         num(p.last - p.first + 1) + " reads;";
            }
        }
        if (found != 1) { a_ok = false; a_why += " field " + num(f + 1) + " has " + num(found) + " passes;"; }
    }
    g.pre(checks, a_ok,
          "P1-PRE-A: (a) in each observed field the Copper's pass starts at " + hex(kList, 5) + " and reads the 36 words of the list in "
          "address order with the data stored" + a_why);

    // (b) no sprite access and no bitplane access during the observed fields.
    bool b_ok = cf.size() >= 6;
    size_t spr = 0, bpl = 0;
    if (cf.size() >= 6) {
        spr = reads_in(r, {{0x4E000, 0x4E0FF}}, cf[2].begin, cf[5].end).size() + writes_in(r, {{0x4E000, 0x4E0FF}}, cf[2].begin, cf[5].end).size();
        bpl = reads_in(r, {{0x4C000, 0x4CFFF}}, cf[2].begin, cf[5].end).size() + writes_in(r, {{0x4C000, 0x4CFFF}}, cf[2].begin, cf[5].end).size();
        b_ok = spr == 0 && bpl == 0;
    }
    g.pre(checks, b_ok,
          "P1-PRE-B: (b) no sprite access and no bitplane access appears at the port during the observed fields (" +
          num(spr) + " in the sprite region, " + num(bpl) + " in the bitplane region)");

    // (c) each colour appears in at least one row of each observed field.
    bool c_ok = cf.size() >= 6;
    std::vector<std::vector<Row>> rows;
    std::string seqs;
    for (size_t f = 2; f < 6 && f < cf.size(); ++f) {
        rows.push_back(rows_of(r, cf[f]));
        seqs += "[" + seq_text(run_sequence(rows.back())) + "]";
        for (size_t i = 0; i < 9; ++i) {
            if (image[4 * i] != 0x0180) continue; // a MOVE to another register sets no band
            const uint16_t col = image[4 * i + 1] & 0xFFF;
            bool seen = false;
            for (const auto& x : rows.back()) if (x.rgb == col) seen = true;
            if (!seen) c_ok = false;
        }
    }
    g.pre(checks, c_ok,
          "P1-PRE-C: (c) each of the nine colours the list sets in COLOR00 appears in at least one row of each observed field");

    // Expected: the run sequence of every observed field.
    bool seq_ok = cf.size() >= 6;
    for (const auto& rw : rows) {
        auto seq = run_sequence(rw);
        auto it = std::find(seq.begin(), seq.end(), kColours[0]);
        if (it == seq.end()) { seq_ok = false; continue; }
        const size_t i = size_t(it - seq.begin());
        if (i > 1 || (i == 1 && seq[0] != kColours[8])) seq_ok = false;
        if (seq.size() != i + 9) seq_ok = false;
        for (size_t k = 0; seq_ok && k < 9; ++k) if (seq[i + k] != kColours[k]) seq_ok = false;
    }
    checks.note("P1 run sequences of the observed fields " + seqs);
    if (g.ok) checks.expect(seq_ok,
             "P1-COLOUR: [p19 p20 p21 p36 p274 p275] ch2-about-the-copper.c13.s2 ch2-what-is-a-copper-instruction.c4.s2 "
             "ch2-the-move-instruction.c1.s2 ch2-appa-copins.c8.s2 COP2-copins-fields-p275.c5 "
             "COP2-copins-fields-p275.c6 COP2-copins-fields-p275.c9 ch2-appa-copins.c116 " + rd_ids(11, 0) + " "
             "ch2-summary-of-copper-instructions.c98 ch2-summary-of-copper-instructions.c99 ch2-summary-of-copper-instructions.c100 "
             "ch2-summary-of-copper-instructions.c101: in every observed field the run sequence holds, consecutively and in order, the "
             "colours (F,F,F) (5,5,5) (6,6,6) (8,7,8) (F,8,0) (A,A,A) (9,9,9) (7,8,7) (0,7,F) set by the eight patterns of IR2 bits 11-0 "
             "moved to COLOR00, at most the previous field's last colour (0,7,F) comes before (F,F,F), and (0,7,F) is the last colour");
    else g.skip(checks, "P1-COLOUR");
}

} // namespace p1

// =============================================================================
// P2. IR2 bits 15-1 reach COP1LCL bits 15-1 and IR2 bits 2-0 reach COP1LCH
// bits 2-0; shown by the address where the Copper restarts.
// =============================================================================
namespace p2 {

struct Seg { uint32_t t; uint16_t l, h; };

// Segment k is at T_k and writes (L, H); the next restart is at T_{k+1}.
const std::vector<Seg> kSegs = {
    {0x34000, 0xAAAA, 0x0001}, {0x1AAAA, 0xCCCC, 0x0001}, {0x1CCCC, 0xF0F0, 0x0001}, {0x1F0F0, 0xFF00, 0x0001},
    {0x1FF00, 0x5554, 0x0001}, {0x15554, 0x3332, 0x0001}, {0x13332, 0x0F0E, 0x0001}, {0x10F0E, 0x00FE, 0x0001},
    {0x100FE, 0x2000, 0x0005}, {0x52000, 0x2000, 0x0006}, {0x62000, 0x2000, 0x0002}, {0x22000, 0x2000, 0x0001},
    {0x12000, 0x2000, 0x0001}};

uint32_t target(const Seg& s) { return (uint32_t(s.h & 7) << 16) | (s.l & 0xFFFE); }

void run(Checks& checks, const std::string& dir, const std::string& mv) {
    std::vector<Seg> segs = kSegs;
    std::vector<Poke> pk;
    // P2-MAIN: segment 3 writes $FF80 instead of $FF00 and a segment at $1FF80 continues the chain.
    if (mv == "P2-MAIN") segs[3].l = 0xFF80;
    for (const auto& s : segs) {
        if (mv == "P2-A" && s.t == 0x15554) {
            put(pk, s.t, {0xFFFF, 0xFFFE, 0x0082, s.l, 0x0080, s.h});
        } else if (mv == "P2-B" && s.t == 0x13332) {
            put(pk, s.t, {0x0082, s.l, 0x0080, s.h, 0xF001, 0xFF00, 0xFFFF, 0xFFFE});
        } else {
            put(pk, s.t, {0x0082, s.l, 0x0080, s.h, 0xFFFF, 0xFFFE});
        }
    }
    if (mv == "P2-MAIN") put(pk, 0x1FF80, {0x0082, 0x5554, 0x0080, 0x0001, 0xFFFF, 0xFFFE});
    std::map<uint32_t, uint16_t> mem(pk.begin(), pk.end());
    // The segment base each pass begins in, as the stimulus makes the Copper follow (pass 0 starts at the strobe address).
    std::vector<uint32_t> chain;
    for (size_t k = 0; k < 15; ++k) {
        if (mv == "P2-MAIN") chain.push_back(k == 4 ? 0x1FF80 : kSegs[std::min<size_t>(k, 12)].t);
        else if (mv == "P2-A") chain.push_back(kSegs[std::min<size_t>(k, 5)].t);
        else chain.push_back(kSegs[std::min<size_t>(k, 12)].t);
    }
    std::string bin = dir + "/prog_p2.bin";
    if (mv == "P2-ATTR") bin = dir + "/prog_p2_attr.bin";
    if (mv == "P2-RUN") bin = dir + "/prog_nodone.bin";
    Setup s;
    s.bin = bin;
    s.pokes = pk;
    s.budget = budget_for(18);
    Run r = run_program(s);
    auto m = marks(r);
    const int64_t en = mark_cycle(m, 3);
    std::vector<Region> cop;
    std::vector<lscan::Guard> guards;
    for (const auto& sg : kSegs) { cop.push_back({sg.t, sg.t + 0x1F}); guards.push_back({sg.t, sg.t + 0x1F, 0, 0}); }
    cop.push_back({0x1FF80, 0x1FF9F});
    guards.push_back({0x1FF80, 0x1FF9F, 0, 0});
    Gate g;

    // The observation: Copper accesses from the enable marker until 14 restart accesses have occurred.
    std::vector<Acc> rd, extra;
    std::vector<Pass> ps;
    bool have14 = false;
    if (en >= 0) {
        auto all = cop_reads(r, cop, uint64_t(en), kForever, &extra);
        auto p = passes_of(all);
        // pass n (n >= 1) begins with restart access n; the 14th restart access begins pass 14.
        if (p.size() >= 15) {
            have14 = true;
            rd.assign(all.begin(), all.begin() + long(p[14].first) + 1);
            ps = passes_of(rd);
            std::vector<Acc> in_interval;
            for (const auto& x : extra) if (x.t < rd.back().t) in_interval.push_back(x);
            extra = in_interval;
        }
    }
    checks.note("P2 extra reads (the word after the end WAIT, in no pass) before the 14th restart access: " + extra_text(extra));
    g.run(checks, r.loaded && r.released && r.done && marks_are(m, {0, 1, 2, 3}) && have14,
          "P2-RUN: the program ran to its DONE mark, wrote markers 0, 1, 2 and 3 once each in order, and 14 restart accesses occur after the enable marker");
    std::string why;
    const bool attr_ok = listing_ok(bin, guards, why);
    g.stat(checks, attr_ok,
          "P2-ATTR: the program's listing shows no CPU data access to a Copper segment region or to a parked region of another channel" +
          (attr_ok ? std::string() : " (" + why + ")"));

    // (a) pass k (k = 1..13, the passes that begin with a restart access) reads segment k's six words T_k..T_k+10 in address
    // order, with the data the segment holds.
    // (b) the Copper's first access after its read of T_k+10 is not at T_k+12 or T_k+14.
    bool a_ok = have14, b_ok = have14;
    std::string a_why, b_why;
    if (have14) {
        for (size_t k = 1; k < 14; ++k) {
            const uint32_t tk = chain[k];
            bool found = false;
            for (size_t i = ps[k].first; i + 5 <= ps[k].last && !found; ++i) {
                bool ok = true;
                for (int w = 0; w < 6 && ok; ++w) if (rd[i + w].addr != tk + 2 * w || rd[i + w].data != mem[tk + 2 * w]) ok = false;
                found = ok;
            }
            if (!found) { a_ok = false; a_why += " pass " + num(k) + " lacks the six words of " + hex(tk, 5) + ";"; }
            for (size_t i = ps[k].first; i <= ps[k].last; ++i) {
                if (rd[i].addr != tk + 10) continue;
                const uint32_t nxt = rd[i + 1].addr;
                if (nxt == tk + 12 || nxt == tk + 14) {
                    b_ok = false;
                    if (b_why.empty()) b_why = " (the first access after the read of " + hex(tk + 10, 5) + " in pass " + num(k) + " is " + hex(nxt, 5) + ")";
                }
            }
        }
    }
    g.pre(checks, a_ok,
          "P2-PRE-A: (a) between its restart access and the next, the Copper reads segment k's six words T_k to T_k+10 in address "
          "order, with the data the segment holds" + a_why);
    g.pre(checks, b_ok,
          "P2-PRE-B: (b) the Copper's first access after its read of T_k+10, not counting the extra read at T_k+12, is not at T_k+12 or "
          "T_k+14, so the access that follows is a restart" + b_why);

    // Expected: restart access k (k = 1..12) is at T_k, and every later one at $12000. The 14 restart accesses are graded.
    bool ok = have14;
    std::string got;
    for (size_t k = 1; k <= 14 && have14; ++k) {
        const uint32_t want = target(kSegs[std::min<size_t>(k - 1, 12)]);
        const uint32_t actual = rd[ps[k].first].addr;
        char b[16]; std::snprintf(b, sizeof b, "%05X ", actual); got += b;
        if (actual != want) ok = false;
    }
    checks.note("P2 restart accesses 1 to 14: " + got);
    if (g.ok) checks.expect(ok,
             "P2-RESTART: [p21 p25 p36 p274 p275] " + rd_ids(15, 12) + " " + rd_ids(11, 0) + " ch2-the-move-instruction.c7: the Copper restarts at the "
             "address IR2 bits 15-1 (COP1LCL) and IR2 bits 2-0 (COP1LCH) form: restart accesses 1 to 12 are at $1AAAA $1CCCC $1F0F0 $1FF00 "
             "$15554 $13332 $10F0E $100FE $52000 $62000 $22000 $12000 and the 13th and 14th at $12000");
    else g.skip(checks, "P2-RESTART");
}

} // namespace p2

// =============================================================================
// P3. The word a MOVE delivers is the content of RAM at its IR2 fetch.
// =============================================================================
namespace p3 {

constexpr uint32_t kList = 0x36000;
constexpr uint32_t kIr2 = 0x3600A;
constexpr uint16_t kValues[4] = {0x00FF, 0x0FF0, 0x0A5A, 0x05A5};
constexpr uint16_t kInitial = 0x0F0F;

void run(Checks& checks, const std::string& dir, const std::string& mv) {
    std::vector<Poke> pk;
    put(pk, kList, {0x0180, 0x0FFF, 0x8001, 0xFF00, 0x0180, kInitial, 0xFFFF, 0xFFFE});
    // P3-B: the Copper starts at a list at $36080 that reads no IR2 word and loads COP1LC with $36000.
    if (mv == "P3-B") put(pk, kList + 0x80, {0x0082, 0x6000, 0x0080, 0x0003, 0xFFFF, 0xFFFE});
    // P3-II: a later MOVE to COLOR00 behind a WAIT for line $C0 sets the colour of the rows after it.
    if (mv == "P3-II") put(pk, kList + 12, {0xC001, 0xFF00, 0x0180, 0x0F00, 0xFFFF, 0xFFFE});
    std::string bin = dir + "/prog_p3.bin";
    if (mv == "P3-A") bin = dir + "/prog_p3_fast.bin";
    if (mv == "P3-B") bin = dir + "/prog_p3_b.bin";
    if (mv == "P3-MAIN") bin = dir + "/prog_p3_dest.bin";
    if (mv == "P3-ATTR") bin = dir + "/prog_p3_attr.bin";
    if (mv == "P3-RUN") bin = dir + "/prog_nodone.bin";
    Setup s;
    s.bin = bin;
    s.pokes = pk;
    s.budget = budget_for(14);
    Run r = run_program(s);
    auto m = marks(r);
    const int64_t en = mark_cycle(m, 3);
    auto cf = en >= 0 ? complete_fields_after(r, uint64_t(en)) : std::vector<Fld>{};
    const std::vector<Region> cop = {{kList, kList + 0xFF}};
    const std::vector<Region> ir2 = {{kIr2, kIr2 + 1}};
    Gate g;

    // The CPU's own writes of the four values, whichever address they go to.
    auto dest_writes = writes_in(r, {{0x3600A, 0x3600B}, {0x36010, 0x36011}}, en >= 0 ? uint64_t(en) : 0);
    auto val_writes = writes_in(r, ir2, en >= 0 ? uint64_t(en) : 0);
    bool four = dest_writes.size() == 4;
    for (size_t i = 0; four && i < 4; ++i) if (dest_writes[i].data != kValues[i]) four = false;

    // The observation interval: from the 3rd complete field after the enable marker until
    // 2 complete fields after the last CPU write.
    uint64_t t0 = 0, t1 = 0;
    bool window = false;
    if (cf.size() >= 3 && four) {
        auto after = complete_fields_after(r, dest_writes.back().t);
        if (after.size() >= 2) { t0 = cf[2].begin; t1 = after[1].end; window = true; }
    }
    g.run(checks, r.loaded && r.released && r.done && marks_are(m, {0, 1, 2, 3}) && four && window,
          "P3-RUN: the program ran to its DONE mark, wrote markers 0, 1, 2 and 3 once each in order, the port shows its four value "
          "writes in order, and the capture holds two complete fields after the last of them");
    std::string why;
    const bool attr_ok = listing_ok(bin, {{kList, kList + 9, 0, 0}, {kIr2, kIr2 + 1, 0, 4}, {kList + 0xC, kList + 0xF, 0, 0},
                                          {0x36010, 0x36011, 0, 4}, {0x36012, kList + 0xFF, 0, 0}}, why);
    g.stat(checks, attr_ok,
          "P3-ATTR: the program's listing shows no CPU data access to the Copper list region " + hex(kList, 5) + "-" + hex(kList + 0xFF, 5) +
          " other than the four writes of the values, or to a parked region of another channel" + (attr_ok ? std::string() : " (" + why + ")"));

    std::vector<Acc> reads, all_reads, extra;
    std::vector<Pass> ps;
    if (window) {
        reads = reads_in(r, ir2, t0, t1);
        all_reads = cop_reads(r, cop, uint64_t(en), t1, &extra);
        ps = passes_of(all_reads);
    }
    checks.note("P3 extra reads (the word after the end WAIT, in no pass) in the interval: " + extra_text(extra));
    // (a) between two consecutive Copper reads of the IR2 word at most one value write, and every value write is followed by a read.
    bool a_ok = window && !reads.empty();
    for (size_t j = 0; a_ok && j + 1 < reads.size(); ++j) {
        int n = 0;
        for (const auto& w : dest_writes) if (w.t >= reads[j].t && w.t < reads[j + 1].t) ++n;
        if (n > 1) a_ok = false;
    }
    for (size_t i = 0; a_ok && i < dest_writes.size(); ++i) {
        bool followed = false;
        for (const auto& x : reads) if (x.t > dest_writes[i].t) followed = true;
        if (!followed) a_ok = false;
    }
    g.pre(checks, a_ok,
          "P3-PRE-A: (a) between two consecutive Copper reads of " + hex(kIr2, 5) + " there is at most one of the program's four value "
          "writes, and every value write is followed by at least one Copper read of " + hex(kIr2, 5));
    // (b) every pass contains a Copper read of the IR2 word.
    bool b_ok = window && !ps.empty();
    int complete = 0;
    for (const auto& p : ps) {
        if (!p.complete) continue;
        ++complete;
        bool has = false;
        for (size_t i = p.first; i <= p.last; ++i) if (all_reads[i].addr == kIr2) has = true;
        if (!has) b_ok = false;
    }
    g.pre(checks, b_ok && complete >= 4,
          "P3-PRE-B: (b) every pass contains a Copper read of " + hex(kIr2, 5) + " (" + num(complete) + " complete passes, at least 4 needed)");

    // (i) each d_j is the last value the CPU wrote before r_j, or the initial value before the first write.
    bool i_ok = window && !reads.empty();
    std::string dj;
    for (const auto& x : reads) {
        uint16_t want = kInitial;
        for (const auto& w : val_writes) if (w.t < x.t) want = w.data;
        if (x.data != want) i_ok = false;
        char b[16]; std::snprintf(b, sizeof b, "%04X ", x.data); dj += b;
    }
    // (iii) the values moved over the observed passes are the initial value and the four values, in this order.
    std::vector<uint16_t> moved;
    for (const auto& x : reads) if (moved.empty() || moved.back() != x.data) moved.push_back(x.data);
    const std::vector<uint16_t> want_moved = {kInitial, kValues[0], kValues[1], kValues[2], kValues[3]};
    bool iii_ok = window && moved == want_moved;
    // (ii) the colour of every band row is the low 12 bits of the word the latest Copper read of the IR2 word delivered.
    bool ii_ok = window;
    size_t band = 0;
    if (window) {
        for (const auto& f : complete_fields_after(r, t0)) {
            if (f.end > t1) break;
            auto rows = rows_of(r, f);
            long last_f = -1;
            for (size_t i = 0; i < rows.size(); ++i) if (rows[i].rgb == 0xFFF) last_f = long(i);
            if (last_f < 0) { ii_ok = false; continue; }
            for (size_t i = size_t(last_f) + 1; i < rows.size(); ++i) {
                if (rows[i].rgb == 0 || rows[i].rgb == 0xFFF) continue;
                ++band;
                const Acc* latest = nullptr;
                for (const auto& x : reads) if (x.t < rows[i].idx) latest = &x;
                if (!latest || (latest->data & 0xFFF) != rows[i].rgb) ii_ok = false;
            }
        }
    }
    ii_ok = ii_ok && band > 0;
    checks.note("P3 values delivered by the Copper reads of " + hex(kIr2, 5) + " in the interval: " + dj + "; band rows graded: " + num(band));
    if (g.ok) checks.expect(i_ok && ii_ok && iii_ok,
             "P3-MOVED: [p21 p275] COP2-copins-fields-p275.c8 ch2-appa-copins.c115 "
             "ch2-the-move-instruction.c1.s2: the value a MOVE delivers is the word RAM held at its IR2 fetch: every Copper read of " + hex(kIr2, 5) +
             " returns the last value the CPU wrote there, every band row shows the low 12 bits of the word the latest such read returned, "
             "and the values delivered over the observed passes are " + hex(kInitial) + " " + hex(kValues[0]) + " " + hex(kValues[1]) + " " +
             hex(kValues[2]) + " " + hex(kValues[3]) + " in this order");
    else g.skip(checks, "P3-MOVED");
}

} // namespace p3

// =============================================================================
// P4. A MOVE that follows a WAIT is performed only once the WAIT is satisfied.
// =============================================================================
namespace p4 {

constexpr uint32_t kList = 0x38000;

void run(Checks& checks, const std::string& dir, const std::string& mv) {
    std::vector<Poke> pk;
    put(pk, kList, {0x0180, 0x0FFF, 0x8001, 0xFF00, 0x0180, 0x00F0, 0xFFFF, 0xFFFE,
                    0x0180, 0x0F00, 0xFFFF, 0xFFFE});
    if (mv == "P4-MAIN") put(pk, kList + 12, {0xC001, 0xFF00});
    if (mv == "P4-A") put(pk, kList + 4, {0xFFFF, 0xFFFE});
    std::string bin = dir + "/prog_p4.bin";
    if (mv == "P4-B") bin = dir + "/prog_p4_b.bin";
    if (mv == "P4-ATTR") bin = dir + "/prog_p4_attr.bin";
    if (mv == "P4-RUN") bin = dir + "/prog_nodone.bin";
    Setup s;
    s.bin = bin;
    s.pokes = pk;
    s.budget = budget_for(9);
    Run r = run_program(s);
    auto m = marks(r);
    const int64_t en = mark_cycle(m, 3);
    const uint64_t m0 = mark_cycle(m, 0) >= 0 ? uint64_t(mark_cycle(m, 0)) : 0;
    auto cf = en >= 0 ? complete_fields_after(r, uint64_t(en)) : std::vector<Fld>{};
    const std::vector<Region> cop = {{kList, kList + 0xFF}};
    Gate g;

    g.run(checks, r.loaded && r.released && r.done && marks_are(m, {0, 1, 2, 3}) && cf.size() >= 6,
          "P4-RUN: the program ran to its DONE mark, wrote markers 0, 1, 2 and 3 once each in order, and the capture holds "
          "at least six complete fields after the enable marker (" + num(cf.size()) + ")");
    std::string why;
    const bool attr_ok = listing_ok(bin, {{kList, kList + 0xFF, 0, 0}}, why);
    g.stat(checks, attr_ok,
          "P4-ATTR: the program's listing shows no CPU data access to the Copper list region " + hex(kList, 5) + "-" +
          hex(kList + 0xFF, 5) + " or to a parked region of another channel" + (attr_ok ? std::string() : " (" + why + ")"));

    std::vector<Acc> rd, extra;
    std::vector<Pass> ps;
    if (cf.size() >= 6) {
        rd = cop_reads(r, cop, m0, cf[5].end, &extra); // from marker 0: the Copper's first reads precede the write of marker 3
        ps = passes_of(rd);
    }
    auto pass_reads = [&](const Pass& p, uint32_t a) {
        for (size_t i = p.first; i <= p.last; ++i) if (rd[i].addr == a) return true;
        return false;
    };
    int complete = 0;
    bool a_ok = true, b_ok = true;
    for (size_t k = 0; k < ps.size(); ++k) {
        if (ps[k].complete) {
            ++complete;
            if (!pass_reads(ps[k], kList + 0xC) || !pass_reads(ps[k], kList + 0xE)) a_ok = false;
        }
        if (k >= 1 && rd[ps[k].first].addr != kList) b_ok = false;
    }
    g.pre(checks, a_ok && complete >= 4,
          "P4-PRE-A: (a) in every pass of the interval the Copper reads " + hex(kList + 0xC, 5) + " and " + hex(kList + 0xE, 5) +
          " (the unsatisfiable WAIT) (" + num(complete) + " complete passes, at least 4 needed)");
    g.pre(checks, b_ok && ps.size() >= 2,
          "P4-PRE-B: (b) every pass after the first begins with a restart access at " + hex(kList, 5));

    // Expected (i): in every pass the Copper reads MOVE A after its read of $38006; in every observed field the run after (F,F,F) is (0,F,0).
    bool i_ok = complete >= 4;
    for (const auto& p : ps) {
        if (!p.complete) continue;
        int stage = 0;
        for (size_t i = p.first; i <= p.last; ++i) {
            if (stage == 0 && rd[i].addr == kList + 6) stage = 1;
            else if (stage == 1 && rd[i].addr == kList + 8) stage = 2;
            else if (stage == 2 && rd[i].addr == kList + 0xA) stage = 3;
        }
        if (stage != 3) i_ok = false;
    }
    bool fields_ok = cf.size() >= 6, never_red = true, last_ok = cf.size() >= 6;
    std::string seqs;
    std::string nrows;
    for (size_t f = 0; f < 6 && f < cf.size(); ++f) {
        auto rows = rows_of(r, cf[f]);
        auto seq = run_sequence(rows);
        nrows += (f ? " " : "") + num(rows.size());
        for (const auto& x : rows) if (x.rgb == 0xF00) never_red = false; // all six complete fields of the interval
        if (f < 2) continue; // the colour sequence is graded in fields 3 to 6 only
        seqs += "[" + seq_text(seq) + "]";
        auto it = std::find(seq.begin(), seq.end(), 0xFFF);
        if (it == seq.end() || it + 1 == seq.end() || *(it + 1) != 0x0F0) fields_ok = false;
        if (seq.empty() || seq.back() != 0x0F0) last_ok = false;
    }
    // Expected (ii): MOVE B is not performed: no row shows (F,0,0) and the last non-black run of every observed field, COLOR00's value
    // at the field boundary, is (0,F,0). The Copper's reads of MOVE B's words are measured and not graded (Section 8, A2).
    int move_b_reads = 0;
    for (const auto& a : rd) if (a.addr == kList + 0x10 || a.addr == kList + 0x12) ++move_b_reads;
    for (const auto& a : extra) if (a.addr == kList + 0x10 || a.addr == kList + 0x12) ++move_b_reads;
    checks.note("P4 sampled rows per field of the interval: " + nrows + "; run sequences of fields 3 to 6 " + seqs + "; reads of " + hex(kList + 0x10, 5) + " or " +
                hex(kList + 0x12, 5) + " in the interval: " + num(move_b_reads) + " (not graded); extra reads (the word after the end WAIT, "
                "in no pass): " + extra_text(extra));
    if (g.ok) checks.expect(i_ok && fields_ok && last_ok && never_red,
             "P4-MOVE-AFTER-WAIT: [p19] ch2-about-the-copper.c18.s2: the Copper performs MOVE A (reads " + hex(kList + 8, 5) +
             " and " + hex(kList + 0xA, 5) + " after " + hex(kList + 6, 5) + ", and each of fields 3 to 6 shows (0,F,0) after (F,F,F)) and "
             "never performs MOVE B behind the unsatisfiable WAIT (no (F,0,0) row in any of the six complete fields, and (0,F,0) is the last colour of each of fields 3 to 6)");
    else g.skip(checks, "P4-MOVE-AFTER-WAIT");
}

} // namespace p4

// =============================================================================
// P5. One 680x0 long-word write loads a location register pair, H from the high
// word and L from the low word; the Copper starts there after the pair's strobe,
// and restarts from COP1LC at every vertical blank.
// =============================================================================
namespace p5 {

constexpr uint32_t kA = 0x20004, kB = 0x25A5C, kC = 0x40002;

struct Leg {
    bool graded_ok = false; // the comparison holds
    bool pre_ok = false;    // every precondition of the leg holds
    std::string text;
};

enum Kind { kR1, kR2, kR2c };

// One run (R1, R2 or R2c) in one mode.
Leg leg(Checks& checks, const std::string& bin, Kind kind, bool ecs, const std::string& mv) {
    const bool r1 = kind == kR1;
    std::vector<Poke> pk;
    put(pk, kA, {0xFFFF, 0xFFFE});
    put(pk, kB, {0xFFFF, 0xFFFE});
    put(pk, kC, {0xFFFF, 0xFFFE});
    if (mv == "P5-C") put(pk, kA, {0x0180, 0x0F00, 0xFFFF, 0xFFFE});
    Setup s;
    s.bin = bin;
    s.pokes = pk;
    s.cfg.chipset_config = ecs ? 0x08 : 0x00;
    s.budget = budget_for(r1 ? 13 : 7);
    Run r = run_program(s);
    auto m = marks(r);
    const std::string tag = std::string(" [") + (kind == kR1 ? "R1" : kind == kR2 ? "R2" : "R2c") + ", " + (ecs ? "ECS" : "OCS") + "]";
    const std::vector<Region> cop = {{kA, kA + 0x1B}, {kB, kB + 0x23}, {kC, kC + 0x1D}};
    Gate g;
    const bool marks_ok = r1 ? marks_are(m, {0, 1, 2, 3, 4, 5}) : marks_are(m, {0, 1, 2, 3, 5});
    g.run(checks, r.loaded && r.released && r.done && marks_ok,
          "P5-RUN: the program ran to its DONE mark and wrote its markers once each in order" + tag);
    std::string why;
    const bool attr_ok = listing_ok(bin, {{kA, kA + 0x1B, 0, 0}, {kB, kB + 0x23, 0, 0}, {kC, kC + 0x1D, 0, 0}}, why);
    g.stat(checks, attr_ok,
          "P5-ATTR: the program's listing shows no CPU data access to a Copper list region or to a parked region of another channel" + tag +
          (attr_ok ? std::string() : " (" + why + ")"));
    Leg out;
    if (!marks_ok) {
        // The run produced no capture to read: its preconditions are NOT GRADED.
        g.pre(checks, false, "P5-PRE-A" + tag);
        g.pre(checks, false, "P5-PRE-B" + tag);
        g.pre(checks, false, "P5-PRE-C" + tag);
        return out;
    }
    // The Copper starts as soon as Copper DMA is on, so the boundary is marker 2, written before the DMACON write (Section 8, A3).
    const uint64_t m0 = uint64_t(mark_cycle(m, 0)), m2 = uint64_t(mark_cycle(m, 2)), m5 = uint64_t(mark_cycle(m, 5));
    const uint64_t m4 = r1 ? uint64_t(mark_cycle(m, 4)) : m5;
    std::vector<Acc> extra;
    auto rd = cop_reads(r, cop, m0, m5, &extra);
    auto ps = passes_of(rd);
    if (dbg()) {
        std::cout << "DEBUG P5" << tag << ": m0=" << m0 << " m2=" << m2 << " m4=" << m4 << " m5=" << m5 << std::endl;
        for (const auto& x : m) std::cout << "  mark " << x.value << " at " << x.cycle << std::endl;
        for (size_t i = 0; i < rd.size() && i < 10; ++i)
            std::cout << "  rd t=" << rd[i].t << " a=" << hex(rd[i].addr, 5) << " d=" << hex(rd[i].data) << std::endl;
    }
    // (a) no Copper access between marker 0 and marker 2 (the strobe, with Copper DMA still off).
    const size_t early = reads_in(r, cop, m0, m2).size();
    g.pre(checks, early == 0, "P5-PRE-A: (a) no Copper access occurs between marker 0 and marker 2, written before the DMACON write (" + num(early) + ")" + tag);
    // (b) at least 4 restart accesses in each phase.
    size_t n1 = 0, n2 = 0;
    for (size_t k = 1; k < ps.size(); ++k) {
        const uint64_t t = rd[ps[k].first].t;
        if (t > m2 && t < m4) ++n1;
        else if (t > m4) ++n2;
    }
    g.pre(checks, r1 ? (n1 >= 4 && n2 >= 4) : (n1 + n2 >= 4),
          "P5-PRE-B: (b) at least 4 restart accesses occur " + std::string(r1 ? "between marker 2 and marker 4 and at least 4 after marker 4"
          : "after marker 2") + " (" + num(n1) + ", " + num(n2) + ")" + tag);
    // (c) every pass reads exactly the two words of the end WAIT at its list address.
    bool c_ok = !ps.empty();
    for (const auto& p : ps) {
        if (!p.complete) continue;
        const bool two = p.last == p.first + 1 && rd[p.first].data == 0xFFFF && rd[p.last].data == 0xFFFE &&
                         rd[p.last].addr == rd[p.first].addr + 2 &&
                         (rd[p.first].addr == kA || rd[p.first].addr == kB || rd[p.first].addr == kC);
        if (!two) c_ok = false;
    }
    g.pre(checks, c_ok, "P5-PRE-C: (c) every pass reads exactly the two words of the end WAIT at its list address" + tag);
    out.pre_ok = g.ok;

    // Expected.
    const Acc* first = nullptr;
    for (const auto& x : rd) if (x.t >= m2) { first = &x; break; }
    bool ok = first != nullptr;
    std::string got;
    if (first) got = "first " + hex(first->addr, 5);
    if (r1) {
        if (!first || first->addr != kA) ok = false;
        for (size_t k = 1; k < ps.size(); ++k) {
            const uint64_t t = rd[ps[k].first].t;
            const uint32_t a = rd[ps[k].first].addr;
            if (t > m2 && t < m4 && a != kA) ok = false;
            if (t > m4 && a != kB) ok = false;
        }
    } else {
        if (!first || first->addr != kB) ok = false;
        for (size_t k = 1; k < ps.size(); ++k) if (rd[ps[k].first].t > m2 && rd[ps[k].first].addr != kA) ok = false;
    }
    std::string rs;
    for (size_t k = 1; k < ps.size() && k < 12; ++k) rs += " " + hex(rd[ps[k].first].addr, 5);
    checks.note("P5" + tag + " " + got + "; restart accesses:" + rs + "; extra reads (the word after the end WAIT, in no pass): " + extra_text(extra));
    out.graded_ok = ok;
    return out;
}

void run(Checks& checks, const std::string& dir, const std::string& mv) {
    bool all_pre = true, ctrl_pre = true, r1_ok = true, r2_ok = true, ctrl_ok = true;
    for (bool ecs : {true, false}) {
        std::string b1 = dir + "/prog_p5_r1.bin", b2 = dir + "/prog_p5_r2.bin", b3 = dir + "/prog_p5_r2c.bin";
        if (mv == "P5-R1") b1 = dir + "/prog_p5_r1x.bin";
        if (mv == "P5-R2") b2 = dir + "/prog_p5_r2x.bin";
        if (mv == "P5-R2C") b2 = dir + "/prog_p5_r2c.bin";
        if (mv == "P5-R2CX") b3 = dir + "/prog_p5_r2cx.bin";
        if (mv == "P5-A") b1 = dir + "/prog_p5_r1_a.bin";
        if (mv == "P5-B") b1 = dir + "/prog_p5_r1_b.bin";
        if (mv == "P5-ATTR") b1 = dir + "/prog_p5_r1_attr.bin";
        if (mv == "P5-A2") b2 = dir + "/prog_p5_r2_a.bin";
        if (mv == "P5-B2") b2 = dir + "/prog_p5_r2_b.bin";
        if (mv == "P5-ATTR2") b2 = dir + "/prog_p5_r2_attr.bin";
        if (mv == "P5-RUN") { b1 = dir + "/prog_nodone.bin"; b2 = dir + "/prog_nodone.bin"; b3 = dir + "/prog_nodone.bin"; }
        Leg l1 = leg(checks, b1, kR1, ecs, mv);
        Leg l2 = leg(checks, b2, kR2, ecs, mv);
        Leg l3 = leg(checks, b3, kR2c, ecs, mv);
        all_pre = all_pre && l1.pre_ok && l2.pre_ok;
        ctrl_pre = ctrl_pre && l3.pre_ok;
        r1_ok = r1_ok && l1.graded_ok;
        r2_ok = r2_ok && l2.graded_ok;
        ctrl_ok = ctrl_ok && l3.graded_ok;
    }
    if (!all_pre) {
        checks.record("NOT GRADED P5-LOCATION: a precondition of one of the runs R1 and R2 does not hold");
        checks.record("NOT GRADED P5-LOCATION-R2: a precondition of one of the runs R1 and R2 does not hold");
    } else {
        checks.expect(r1_ok,
                      "P5-LOCATION: [p25 p288] COP2-location-registers.c6 COP2-location-registers.c9 ch2-location-registers.c11 "
                      "COP2-location-registers.c13 appB-note.c9: a long-word write loads the high word into COPxLCH and the low word into COPxLCL; "
                      "after the matching strobe the Copper's first fetch is at that address, and at every vertical blank it restarts from COP1LC "
                      "(run R1 in ECS and OCS: $20004 after the COPJMP1 strobe, restarts at $20004, then $25A5C after the write without a strobe)");
        checks.expect(r2_ok,
                      "P5-LOCATION-R2: [p25 p26 p276] ch2-location-registers.c16 ch2-location-registers.c17 ch2-jump-address-strobes.c1 "
                      "ch2-jump-address-strobes.c7 ch2-appa-copins.c142: a jump strobe loads the program counter from its location register "
                      "(COPJMP2 restarts from COP2LC); same comparator as P5-LOCATION, run R2 in ECS and OCS: the Copper's first fetch after the "
                      "COPJMP2 strobe, written with Copper DMA off, is at $25A5C, and every restart is at $20004");
    }
    if (!ctrl_pre) {
        checks.record("NOT GRADED P5-LOCATION-R2-CTRL: a precondition of run R2c does not hold");
    } else {
        checks.expect(ctrl_ok,
                      "P5-LOCATION-R2-CTRL: control on the comparator of P5-LOCATION-R2, run R2c in ECS and OCS: with COP1LC and COP2LC both $25A5C at "
                      "the COPJMP2 strobe, written with Copper DMA off, the Copper's first fetch is at $25A5C, and after COP1LC is written $20004 (no "
                      "strobe) every restart is at $20004");
    }
}

} // namespace p5

// =============================================================================
// P6. A 680x0 long-word write puts the most significant word at the lower word address.
// =============================================================================
namespace p6 {

void run(Checks& checks, const std::string& dir, const std::string& mv, bool is_tg68k) {
    struct Mode { const char* name; uint8_t cfg; };
    std::vector<Mode> modes = {{"68000", 0x00}};
    if (is_tg68k) { modes.push_back({"68010", 0x01}); modes.push_back({"68020", 0x03}); }
    std::string bin = dir + "/prog_p6.bin";
    if (mv == "P6-MAIN") bin = dir + "/prog_p6_value.bin";
    if (mv == "P6-A") bin = dir + "/prog_p6_nopre.bin";
    if (mv == "P6-B") bin = dir + "/prog_p6_probe2.bin";
    if (mv == "P6-RUN") bin = dir + "/prog_nodone.bin";
    for (const auto& md : modes) {
        Setup s;
        s.bin = bin;
        s.cfg.cpu_config = md.cfg;
        s.budget = 3'000'000;
        s.peeks = {kResults, kResults + 2};
        Run r = run_program(s);
        auto m = marks(r);
        const std::string tag = std::string(" [") + NMH_XSTR(NANOMIG_CORE_NAME) + " core, " + md.name + " mode]";
        Gate g;
        g.run(checks, r.loaded && r.released && r.done && marks_are(m, {0, 1, 2}),
              "P6-RUN: the program ran to its DONE mark and wrote markers 0, 1 and 2 once each in order" + tag);
        const int64_t m1 = mark_cycle(m, 1), m2 = mark_cycle(m, 2);
        const uint64_t t1 = m1 >= 0 ? uint64_t(m1) : 0, t2 = m2 >= 0 ? uint64_t(m2) : 0;
        auto w0 = writes_in(r, {{0x46000, 0x46001}}, 0, t1);
        auto w2 = writes_in(r, {{0x46002, 0x46003}}, 0, t1);
        g.pre(checks, !w0.empty() && !w2.empty() && w0.back().data == 0xAAAA && w2.back().data == 0x5555,
              "P6-PRE-A: (a) the writes of $AAAA to $46000 and $5555 to $46002 occur before marker 1" + tag);
        // The CPU setting shows at the port through the program's own probe: MOVEC is a 68010 instruction and EXTB.L a
        // 68020 instruction, and a CPU without the instruction takes the illegal instruction exception instead.
        auto idw = writes_in(r, {{kResults + 4, kResults + 5}}, 0, t1);
        const uint16_t want_id = md.cfg == 0x00 ? 0 : md.cfg == 0x01 ? 1 : 2;
        g.pre(checks, idw.size() == 1 && idw[0].data == want_id,
              "P6-PRE-B: (b) the CPU identification the program stores at " + hex(kResults + 4, 6) + " before marker 1 is " + num(want_id) +
              ", the value for a " + md.name + " (stored: " + (idw.empty() ? std::string("none") : hex(idw.back().data)) + ", " + num(idw.size()) +
              " write(s))" + tag);
        auto a0 = writes_in(r, {{0x46000, 0x46001}}, t1, t2);
        auto a2 = writes_in(r, {{0x46002, 0x46003}}, t1, t2);
        const uint16_t res0 = r.peeked.count(kResults) ? r.peeked[kResults] : 0;
        const uint16_t res2 = r.peeked.count(kResults + 2) ? r.peeked[kResults + 2] : 0;
        checks.note("P6" + tag + " writes to $46000 between markers 1 and 2: " + num(a0.size()) + " (data " +
                    (a0.empty() ? std::string("none") : hex(a0[0].data)) + "), to $46002: " + num(a2.size()) + " (data " +
                    (a2.empty() ? std::string("none") : hex(a2[0].data)) + "), result words " + hex(res0) + " " + hex(res2));
        if (g.ok) checks.expect(a0.size() == 1 && a0[0].data == 0x1234 && a2.size() == 1 && a2[0].data == 0x5678 && res0 == 0x1234 && res2 == 0x5678,
                 "P6-LONG: [p25] ch2-location-registers.c11 COP2-location-registers.c13: MOVE.L #$12345678 to $46000 writes $1234 to $46000 "
                 "and $5678 to $46002, exactly one write each, and reads back as $1234 and $5678" + tag);
    else g.skip(checks, "P6-LONG");
    }
}

} // namespace p6

// =============================================================================
// P7. With COPCON bit 1 (CDANG) set, a Copper MOVE writes a register in $40-$7E:
// a MOVE to BLTSIZE starts the blitter.
// =============================================================================
namespace p7 {

constexpr uint32_t kList = 0x3A000, kSafe = 0x3A100, kDest = 0x48000;

struct Leg { bool pre_ok = false, ok = false; };

Leg leg(Checks& checks, const std::string& bin, bool ecs, const std::string& mv) {
    std::vector<Poke> pk;
    put(pk, kList, {mv == "P7-MAIN" ? uint16_t(0x0180) : uint16_t(0x0058), mv == "P7-W2" ? uint16_t(0x0042) : uint16_t(0x0041),
                    0x0082, 0xA100, 0x0080, 0x0003, 0xFFFF, 0xFFFE});
    put(pk, kSafe, {0xFFFF, 0xFFFE});
    put(pk, kDest, {0xA5A5});
    Setup s;
    s.bin = bin;
    s.pokes = pk;
    s.cfg.chipset_config = ecs ? 0x08 : 0x00;
    s.budget = budget_for(7);
    s.peeks = {kResults};
    Run r = run_program(s);
    auto m = marks(r);
    const std::string tag = std::string(" [") + (ecs ? "ECS" : "OCS") + "]";
    const std::vector<Region> cop = {{kList, kList + 0x1F}, {kSafe, kSafe + 0x1F}};
    Gate g;
    g.run(checks, r.loaded && r.released && r.done && marks_are(m, {0, 1, 2, 3}),
          "P7-RUN: the program ran to its DONE mark and wrote markers 0, 1, 2 and 3 once each in order" + tag);
    std::string why;
    const bool attr_ok = listing_ok(bin, {{kList, kList + 0x1F, 0, 0}, {kSafe, kSafe + 0x1F, 0, 0}, {kDest, kDest + 1, 1, 1},
                                          {kDest + 2, kDest + 0xFF, 0, 0}}, why);
    g.stat(checks, attr_ok,
          "P7-ATTR: the program's listing shows no CPU data access to a Copper list region or to the blitter destination word beyond its "
          "one clearing write and its one final read, or to a parked region of another channel" + tag + (attr_ok ? std::string() : " (" + why + ")"));
    Leg out;
    if (!marks_are(m, {0, 1, 2, 3})) {
        // The run produced no capture to read: its preconditions are NOT GRADED.
        g.pre(checks, false, "P7-PRE-A" + tag);
        g.pre(checks, false, "P7-PRE-B" + tag);
        g.pre(checks, false, "P7-PRE-C" + tag);
        return out;
    }
    const uint64_t m0 = uint64_t(mark_cycle(m, 0)), m3 = uint64_t(mark_cycle(m, 3));
    // The result is stored by the CPU after it reads the destination word.
    auto stores = writes_in(r, {{kResults, kResults + 1}}, m3);
    const uint64_t tres = stores.empty() ? kForever : stores[0].t;
    std::vector<Acc> extra;
    auto rd = cop_reads(r, cop, m0, tres, &extra);
    auto ps = passes_of(rd);
    auto w48 = writes_in(r, {{kDest, kDest + 0xFF}}, m0, tres);
    // The CPU's clearing write is the one that carries $0000 before the enable marker; every other write is the blitter's.
    std::vector<Acc> blit;
    bool cpu_write = false;
    for (const auto& w : w48) {
        if (!cpu_write && w.data == 0x0000 && w.t < m3) { cpu_write = true; continue; }
        blit.push_back(w);
    }
    const Acc* r3a002 = nullptr;
    for (const auto& x : rd) if (x.addr == kList + 2) { r3a002 = &x; break; }
    size_t before = 0;
    for (const auto& w : blit) if (!r3a002 || w.t < r3a002->t) ++before;
    // (a) no blitter access occurs between marker 0 and the Copper's read of $3A002.
    g.pre(checks, !r3a002 ? blit.empty() : before == 0,
          "P7-PRE-A: (a) no blitter access occurs between marker 0 and the Copper's read of " + hex(kList + 2, 5) + " (" + num(before) + ")" + tag);
    // (b) the CPU's write of $0000 to the destination word occurs before the enable marker.
    g.pre(checks, cpu_write, "P7-PRE-B: (b) the CPU's write of $0000 to " + hex(kDest, 5) + " occurs before the enable marker" + tag);
    // (c) the Copper reads $3A000 and $3A002 once, in the first pass, and every later restart access is at $3A100.
    int n0 = 0, n2 = 0;
    for (const auto& x : rd) { if (x.addr == kList) ++n0; if (x.addr == kList + 2) ++n2; }
    bool c_ok = n0 == 1 && n2 == 1 && !ps.empty();
    if (c_ok) {
        bool in_first = false;
        for (size_t i = ps[0].first; i <= ps[0].last; ++i) if (rd[i].addr == kList) in_first = true;
        c_ok = in_first;
    }
    for (size_t k = 1; k < ps.size(); ++k) if (rd[ps[k].first].addr != kSafe) c_ok = false;
    g.pre(checks, c_ok,
          "P7-PRE-C: (c) the Copper reads " + hex(kList, 5) + " and " + hex(kList + 2, 5) + " once, in the first pass, and every later restart "
          "access is at " + hex(kSafe, 5) + tag);
    out.pre_ok = g.ok;

    const uint16_t result = r.peeked.count(kResults) ? r.peeked[kResults] : 0;
    checks.note("P7" + tag + " blitter writes in " + hex(kDest, 5) + "-" + hex(kDest + 0xFF, 5) + ": " + num(blit.size()) + (blit.empty() ? std::string() : " first at " + hex(blit[0].addr, 5) +
                " data " + hex(blit[0].data)) + ", result word " + hex(result) +
                "; extra reads (the word after the end WAIT, in no pass): " + extra_text(extra));
    out.ok = blit.size() == 1 && blit[0].addr == kDest && blit[0].data == 0xFFFF && r3a002 && blit[0].t > r3a002->t && result == 0xFFFF;
    return out;
}

void run(Checks& checks, const std::string& dir, const std::string& mv) {
    bool all_pre = true, all_ok = true;
    for (bool ecs : {true, false}) {
        std::string bin = dir + "/prog_p7.bin";
        if (mv == "P7-A") bin = dir + "/prog_p7_a.bin";
        if (mv == "P7-B") bin = dir + "/prog_p7_b.bin";
        if (mv == "P7-C") bin = dir + "/prog_p7_c.bin";
        if (mv == "P7-ATTR") bin = dir + "/prog_p7_attr.bin";
        if (mv == "P7-LF") bin = dir + "/prog_p7_lf0.bin";
        if (mv == "P7-RUN") bin = dir + "/prog_nodone.bin";
        Leg l = leg(checks, bin, ecs, mv);
        all_pre = all_pre && l.pre_ok;
        all_ok = all_ok && l.ok;
    }
    if (!all_pre) {
        checks.record("NOT GRADED P7-BLIT: a precondition of one of the two runs does not hold");
        return;
    }
    checks.expect(all_ok,
                  "P7-BLIT: [p26 p269 p274] COP2-control-register.c11: with COPCON bit 1 set, a Copper MOVE to BLTSIZE ($058), a blitter control "
                  "register in the range $40-$7E, starts the prepared blit: exactly one blitter write in $48000-$480FF, at $48000 with data $FFFF, "
                  "after the Copper's read of $3A002, and the result word is $FFFF (ECS and OCS); of the blitter control registers this line "
                  "exercises BLTSIZE only");
}

} // namespace p7

// =============================================================================
// P8. Whenever an audio channel's length count finishes, its pointer restarts
// from the channel's location registers.
// =============================================================================
namespace p8 {

constexpr uint32_t kA = 0x44000, kB = 0x44100;
struct Fetch { uint64_t t; uint32_t addr; };
struct RunOf { size_t first, last; };

// A run: a maximal sequence of consecutive channel-0 fetches in which each address is the previous one + 2.
std::vector<RunOf> runs_of(const std::vector<Acc>& v) {
    std::vector<RunOf> out;
    size_t start = 0;
    for (size_t i = 1; i <= v.size(); ++i) {
        if (i == v.size() || v[i].addr != v[i - 1].addr + 2) {
            if (i > start) out.push_back({start, i - 1});
            start = i;
        }
    }
    return out;
}

struct Leg {
    bool pre_ok = false, ok = false, ok_rest = false; // ok_rest: the same comparator without the first run
};

// One run (M or C) of the stimulus.
Leg leg(Checks& checks, const std::string& bin, bool run_c, const std::string& mv) {
    std::vector<Poke> pk;
    put(pk, kA, {0x4000, 0xC000, 0x4000, 0xC000});
    put(pk, kB, {0x2000, 0xE000, 0x2000, 0xE000});
    Setup s;
    s.bin = bin;
    s.pokes = pk;
    s.budget = budget_for(4);
    Run r = run_program(s);
    auto m = marks(r);
    const std::string tag = std::string(" [run ") + (run_c ? "C" : "M") + "]";
    const bool marks_ok = run_c ? marks_are(m, {0, 1, 2, 4, 3}) : marks_are(m, {0, 1, 2, 3});
    const uint64_t cck = cck_cycles(r);
    Gate g;
    g.run(checks, r.loaded && r.released && r.done && marks_ok && cck > 0,
          "P8-RUN: the program ran to its DONE mark, wrote its markers once each in order, and the video shows lines of 227 colour clocks "
          "(a colour clock is " + num(cck) + " cycles)" + tag);
    std::string why;
    const bool attr_ok = listing_ok(bin, {{kA, kA + 0xFF, 0, 0}, {kB, kB + 0xFF, 0, 0}}, why);
    g.stat(checks, attr_ok,
          "P8-ATTR: the program's listing shows no CPU data access to an audio data block or to a parked region of another channel" + tag +
          (attr_ok ? std::string() : " (" + why + ")"));
    Leg out;
    if (!marks_ok) {
        // The run produced no capture to read: its preconditions are NOT GRADED.
        g.pre(checks, false, "P8-PRE-A" + tag);
        g.pre(checks, false, "P8-PRE-B" + tag);
        return out;
    }
    const uint64_t m2 = uint64_t(mark_cycle(m, 2)), m3 = uint64_t(mark_cycle(m, 3));
    const uint64_t mw = run_c ? uint64_t(mark_cycle(m, 4)) : 0;
    auto before = reads_in(r, {{kA, kA + 0xFF}, {kB, kB + 0xFF}}, 0, m2);
    auto f = reads_in(r, {{kA, kA + 0xFF}, {kB, kB + 0xFF}}, m2, m3);
    auto runs = runs_of(f);
        // Every run is graded; the last may be cut off by marker 3 and is graded on the fetches it contains.
    auto expect_run = [&](const RunOf& ru, uint32_t base, bool is_last) {
        const size_t n = ru.last - ru.first + 1;
        if (n > 4) return false;
        if (n < 4 && !is_last) return false;
        for (size_t i = 0; i < n; ++i) if (f[ru.first + i].addr != base + 2 * i) return false;
        return true;
    };
    bool ok = !runs.empty(), ok_rest = runs.size() > 1;
    std::string rs;
    size_t graded_before_w = 0, graded_after_w = 0;
    for (size_t k = 0; k < runs.size(); ++k) {
        const bool is_last = k + 1 == runs.size();
        const uint64_t t = f[runs[k].first].t;
        bool good;
        if (!run_c) {
            good = expect_run(runs[k], kA, is_last);
        } else if (t < mw) {
            good = expect_run(runs[k], kA, is_last);
            ++graded_before_w;
        } else if (t > mw + 454 * cck) {
            good = expect_run(runs[k], kB, is_last);
            ++graded_after_w;
        } else {
            good = expect_run(runs[k], kA, is_last) || expect_run(runs[k], kB, is_last);
        }
        if (!good) { ok = false; if (k > 0) ok_rest = false; }
        if (k < 14) {
            char b[64];
            std::snprintf(b, sizeof b, " [%u@%05X]", unsigned(runs[k].last - runs[k].first + 1), f[runs[k].first].addr);
            rs += b;
        }
    }
    g.pre(checks, run_c ? (graded_before_w >= 2 && graded_after_w >= 3) : runs.size() >= 8,
          "P8-PRE-A: (a) " + std::string(run_c ? "run C has at least 2 graded runs before marker W and at least 3 more than 2 lines after it"
          : "run M has at least 8 graded runs") + " (" + num(run_c ? graded_before_w : runs.size()) + (run_c ? ", " + num(graded_after_w) : std::string()) + ")" + tag);
    g.pre(checks, before.empty(), "P8-PRE-B: (b) no channel-0 fetch occurs before marker 2 (" + num(before.size()) + ")" + tag);
    out.pre_ok = g.ok;
    checks.note("P8" + tag + " runs (length@first address):" + rs);
    out.ok = ok;
    out.ok_rest = ok_rest;
    return out;
}

void run(Checks& checks, const std::string& dir, const std::string& mv) {
    std::string bm = dir + "/prog_p8.bin", bc = dir + "/prog_p8_c.bin";
    if (mv == "P8-M") bm = dir + "/prog_p8_len.bin";
    if (mv == "P8-C") bc = dir + "/prog_p8_c_nolc.bin";
    if (mv == "P8-A") bm = dir + "/prog_p8_noaud.bin";
    if (mv == "P8-B") bm = dir + "/prog_p8_early.bin";
    if (mv == "P8-ATTR") bm = dir + "/prog_p8_attr.bin";
    if (mv == "P8-CA") bc = dir + "/prog_p8_c_noaud.bin";
    if (mv == "P8-CB") bc = dir + "/prog_p8_c_early.bin";
    if (mv == "P8-CATTR") bc = dir + "/prog_p8_c_attr.bin";
    if (mv == "P8-RUN") { bm = dir + "/prog_nodone.bin"; bc = dir + "/prog_nodone.bin"; }
    Leg lm = leg(checks, bm, false, mv);
    Leg lc = leg(checks, bc, true, mv);
    if (!lm.pre_ok || !lc.pre_ok) {
        checks.record("NOT GRADED P8-CTRL: a precondition of run M or run C does not hold");
        checks.record("NOT GRADED P8-RESTART: a precondition of run M or run C does not hold");
        return;
    }
    checks.expect(lm.ok_rest && lc.ok_rest,
                  "P8-CTRL: control on the same measurement basis as P8-RESTART: every run after the first of run M and of run C, graded "
                  "by the same comparator, is the run of four words the AUD0LC in force prescribes");
    checks.expect(lm.ok && lc.ok,
                  "P8-RESTART: [p288] appB-note.c9 appB-note.c11: whenever the audio length count finishes the channel's pointer restarts from "
                  "AUD0LC: with AUD0LEN = 4 every run of fetches is $44000 $44002 $44004 $44006, and after the CPU writes AUD0LC = $44100 the "
                  "runs that start more than 2 lines later are $44100 $44102 $44104 $44106");
}

} // namespace p8

// =============================================================================
// Claims the package declines (SPECIFICATION.md Section 1), each with its reason. A claim id sits on a
// RECORDED line or on a graded line, never on both.
// =============================================================================
void declined(Checks& checks) {
    checks.record("COP2-the-move-instruction.c7 COP2-summary-of-copper-instructions.c98 are declined: "
                  "\"Bits 15 - 9 Not used, but should be set to 0\" tells the programmer what to write; it gives no expected value for any other "
                  "setting, so it only constrains the stimulus (every MOVE here has IR1 bits 15-9 = 0)");
    checks.record("ch2-the-wait-instruction.c12 ch2-using-the-copper-with-the-blitter.c4 COP2-using-the-copper-with-the-blitter.c4 are declined: "
                  "\"Normally, this bit is a 1\" describes usual use, not a behaviour (every WAIT here has BFD = 1)");
    checks.record("ch2-summary-of-copper-instructions.c102 ch2-summary-of-copper-instructions.c103 ch2-summary-of-copper-instructions.c104 "
                  "ch2-summary-of-copper-instructions.c105 ch2-summary-of-copper-instructions.c106 COP2-summary-of-copper-instructions.c105 "
                  "COP2-summary-of-copper-instructions.c106 COP2-summary-of-copper-instructions.c107 are declined: they only name the fields of "
                  "the WAIT and SKIP instructions (VP, HP, VE, HE, BFD); a name cannot be observed");
    checks.record("COP2-a-copper-loop-example.c4 COP2-a-copper-loop-example.c16 are declined: \"always greater than $0F\" and \"binary 1xxx1111\" "
                  "are true by arithmetic and state nothing about the hardware");
    checks.record("ch2-what-is-a-copper-instruction.c2 is declined: \"adds its own instruction set to the instructions already provided by the "
                  "680x0\" is descriptive; its only observable consequence, that the Copper executes its own MOVE, is graded by P1");
    checks.record("ch2-about-the-copper.c5 COP2-about-the-copper.c4 are declined: \"freeing the 680x0 to execute program logic\" states a purpose, "
                  "names no observable quantity, and the page predicts no stimulus under which the 680x0 would not be freed, so no contrasting case exists");
    checks.record("ch2-about-the-copper.c31 ch2-about-the-copper.c32 are declined: \"can retrieve its instructions only from the chip RAM area\": "
                  "every access a test sees is at the Chip-memory port, so a fetch from anywhere else would be invisible, and the pages give no "
                  "expected value for a location register that points outside Chip memory");
    checks.record("ch2-about-the-copper.c6.s2 is withdrawn from P1, P2 and P7: its existential part (\"can directly affect the contents of ... chip "
                  "control registers\") is what they use, and the quantifier \"most\" is not graded (it needs an observable Copper write effect for "
                  "more than half of the 256 register addresses, many of which are strobes, unused or read-only, and the pages disagree about which "
                  "ranges the Copper may write)");
    checks.record("COP2-control-register.c14 is declined: the protection it describes depends on CDANG = 0 blocking $40-$7E, and p.21 and p.310 "
                  "contradict p.26; no outcome for CDANG = 0 is shared by every page");
    checks.record("COP2-control-register.c9 is withdrawn from P7: only the direction every page shares is exercised (bit 1 set allows $40-$7E); "
                  "what bit 1 = 0 does in $010-$07E is not specified");
    checks.record("ch2-appa-copcon.c2.s2 ch2-appb-table.c4.s2 ch2-appc-ecs.c14.s2 are declined: COPCON is write-only, its only effect is CDANG, and "
                  "the pages share no outcome that differs between bit 1 = 0 and bit 1 = 1, so no test can show that a COPCON write took effect");
    checks.record("ch2-appa-copins.c2.s2 ch2-appb-table.c43.s2 are declined: COPINS is a dummy address the Copper generates on the register address "
                  "bus, which is not among the outputs a test can see, and the pages state no effect of a processor write and no read value");
    checks.record("ch2-appb-table.c10.s2 is declined: DENISEID is read-only; the \"R\" mark gives no value, the \"~\" mark rests on the disputed "
                  "CDANG rules, and the OCS value is unspecified, so there is no contrasting case");
    checks.record("ch2-location-registers.c3.s2 ch2-location-registers.c5.s2 are withdrawn from P5: \"Low 16 bits\" is graded for bits 15-1 only, "
                  "bit 0 of a word address is not visible at the port, and the pages disagree about whether the register holds 15 or 16 bits");
    checks.record("COP2-copins-fields-p275.c7 is withdrawn from P3: \"The RD transfer occurs during IR2 time\" says when the register receives the "
                  "word; P3 grades which word is moved (the RAM content at the IR2 fetch), and the page states no delay from that fetch to the "
                  "register write, so the time of the transfer is not an observable");
    checks.record("ch2-starting-the-copper-after-reset.c3 ch2-appa-copins.c146 are withdrawn from P5-LOCATION-R2: \"ensures a known start address and "
                  "known state\" and \"insures a determined startup address and state\" do not say which register's address the start address is, "
                  "and run R2 starts at a determined address, COP1LC's, so the run does not contradict either sentence");
    checks.record("ch2-appa-location-registers.c2.s2 ch2-appa-location-registers.c7.s2 ch2-appa-location-registers.c11.s2 "
                  "ch2-appa-location-registers.c16.s2 ch2-appb-table.c16.s2 ch2-appb-table.c22.s2 ch2-appb-table.c26.s2 ch2-appb-table.c32.s2 "
                  "ch2-appc-ecs.c3.s2 ch2-appc-ecs.c8.s2 ch2-appb-legend.c6.s2 are withdrawn from P2 and P5: the write half of \"write-only\" is "
                  "exercised, the \"only\" half gives no read value and a test must not read these addresses");
}

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;
    const bool is_tg68k = std::string(NMH_XSTR(NANOMIG_CORE_NAME)) == "tg68k";
    const char* env = std::getenv("NMH_MUTATE");
    const std::string mv = env ? env : "";
    const std::string group = mv.substr(0, 2);
    auto wanted = [&](const char* g) { return mv.empty() || group == g; };

    if (mv.empty()) declined(checks);
    if (wanted("P1")) p1::run(checks, dir, mv);
    if (wanted("P2")) p2::run(checks, dir, mv);
    if (wanted("P3")) p3::run(checks, dir, mv);
    if (wanted("P4")) p4::run(checks, dir, mv);
    if (wanted("P5")) p5::run(checks, dir, mv);
    if (wanted("P6")) p6::run(checks, dir, mv, is_tg68k);
    if (wanted("P7")) p7::run(checks, dir, mv);
    if (wanted("P8")) p8::run(checks, dir, mv);

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
