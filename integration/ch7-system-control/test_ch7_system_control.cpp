// test_ch7_system_control.cpp -- ch7-system-control. See SPECIFICATION.md
// for every assertion's proposition, program, observed value and expected
// value. This file only implements what SPECIFICATION.md already
// committed to: P1 (display priority), P2 (bitplane and sprite data in one
// output), P3 (light-pen read) and P4 (LPENDIS).
//
// NMH_MUTATE selects a mutated program or drive schedule in place of its
// normal counterpart; none is exercised by a plain `make test`. A selector
// starting P1- or P2- runs only that proposition's ECS-PAL case with
// prog_pri_pal_<name>.bin or prog_p2_pal_<name>.bin; the other selectors
// act on the P3 and P4 runs.
//
// P1 and P2 observe the video output. A row is identified by the tag drawn
// in bitplane 1; k (output samples per lores pixel) and s0 (the first sample
// of tag block 0) are measured from the output; the evaluated field is the
// video frame holding the middle of the field between the second and third
// VERTB tick of its configuration; every expected colour follows the
// printed priority rules from the sprite and playfield positions measured
// in the run's own reference bands. The preconditions are evaluated and
// printed before the graded lines; a graded line whose preconditions fail
// prints RECORDED ... NOT GRADED.
//
// MUTATION P1-FIXTURE-RUN stimulus red the marker of every configuration stored after the tick that begins the evaluated field instead of before it, via NMH_MUTATE=P1-LATEMARK (measured: P1-FIXTURE-RUN FAIL, every dependent line NOT GRADED, both cores)
// MUTATION P1-PRE-FRAME stimulus red no COP1LC write for configuration 4 (B3), via NMH_MUTATE=P1-NOCOP (measured: P1-PRE-FRAME FAIL, P1-GROUP-B NOT GRADED, both cores)
// MUTATION P1-PRE-TAGS stimulus red one row of the image of A1 and A2 whose tag block 2 is half set, so it does not decode, via NMH_MUTATE=P1-TAGBAD (measured: P1-PRE-TAGS FAIL, P1-GROUP-A NOT GRADED, both cores)
// MUTATION P1-PRE-TAGS stimulus red the first row of band X of A1 and A2 carries tag id 0, so every row decodes and band X has one row fewer than band E, via NMH_MUTATE=P1-TAGCOUNT (measured: P1-PRE-TAGS FAIL and no other line moves except P1-GROUP-A and P1-COLOUR-OF-HIGHEST-PRIORITY NOT GRADED, both cores)
// MUTATION P1-PRE-WINDOW stimulus red the CPU writes BPLCON1 $0011 at line 215 of every evaluated field and clears it after the field, so the rows below line 215 start one lores pixel later than the rows above it, via NMH_MUTATE=P1-SCROLL (measured: P1-PRE-WINDOW FAIL, both cores)
// MUTATION P1-PRE-SPRITES stimulus red DMACON written without SPREN, via NMH_MUTATE=P1-SPRENCLR (measured: P1-PRE-SPRITES FAIL, P1-PRE-VEXTENT NOT GRADED because no sprite reference exists, every group NOT GRADED, both cores)
// MUTATION P1-PRE-PLAYFIELDS stimulus red BPL2PT of B1-B8 at the all-zero image, via NMH_MUTATE=P1-BPL2ZERO (measured: P1-PRE-PLAYFIELDS FAIL, P1-GROUP-B NOT GRADED, both cores)
// MUTATION P1-PRE-VEXTENT stimulus red the sprites of B1-B8 end at line 130 instead of 140, via NMH_MUTATE=P1-VSTOP130 (measured: P1-PRE-VEXTENT FAIL, P1-GROUP-B NOT GRADED, both cores)
// MUTATION P1-PRE-VEXTENT stimulus red the odd slots of A1 and A2 end at line 130 instead of 120, so ten rows after band O show sprites, via NMH_MUTATE=P1-AVSTOP130 (measured: P1-PRE-VEXTENT FAIL, P1-GROUP-A NOT GRADED, both cores)
// MUTATION P1-GROUP-A stimulus red A1 with slot j holding sprite 7-j, graded against the table of A1, via NMH_MUTATE=P1-AMIRROR (measured: P1-GROUP-A FAIL on A1 only, every precondition PASS, both cores)
// MUTATION P1-GROUP-B stimulus red PF1P and PF2P exchanged in B1-B8, via NMH_MUTATE=P1-BSWAP (measured: P1-GROUP-B FAIL on B1-B5 and B8, every precondition PASS, both cores)
// MUTATION P1-GROUP-B stimulus red PF2PRI inverted in B1-B8, via NMH_MUTATE=P1-PRIFLIP (measured: P1-GROUP-B FAIL on B1-B8, every precondition PASS, both cores)
// MUTATION P1-GROUP-E stimulus red v replaced by 4-v in E0-E4, via NMH_MUTATE=P1-EREV (measured: P1-GROUP-E FAIL on every configuration except E2, every precondition PASS, both cores)
// MUTATION P1-COLOUR-OF-HIGHEST-PRIORITY stimulus red A1 with slot j holding sprite 7-j, via NMH_MUTATE=P1-AMIRROR (measured: FAIL together with P1-GROUP-A, both cores)
// MUTATION P2-FIXTURE-RUN stimulus red the marker stored after the tick that begins the evaluated field instead of before it, via NMH_MUTATE=P2-LATEMARK (measured: P2-FIXTURE-RUN FAIL, the dependent lines NOT GRADED, both cores)
// MUTATION P2-PRE-FRAME stimulus red the Copper starts from a copy of the list at another address and no COP1LC write follows, via NMH_MUTATE=P2-NOCOP (measured: P2-PRE-FRAME FAIL, P2-COMBINE NOT GRADED, both cores)
// MUTATION P2-PRE-TAGS stimulus red the tag id of the band is 2 instead of 1, via NMH_MUTATE=P2-TAGID (measured: P2-PRE-TAGS FAIL, P2-COMBINE NOT GRADED, both cores)
// MUTATION P2-PRE-WINDOW stimulus red the CPU writes BPLCON1 $0011 at line 215 of the evaluated field and clears it after the field, via NMH_MUTATE=P2-SCROLL (measured: P2-PRE-WINDOW FAIL, both cores)
// MUTATION P2-PRE-PLANE-FETCH stimulus red BPL4PT at the image of plane 2, via NMH_MUTATE=P2-BPL4PLANE2 (measured: P2-PRE-PLANE-FETCH FAIL, P2-COMBINE NOT GRADED, both cores)
// MUTATION P2-COMBINE stimulus red DMACON written without SPREN, via NMH_MUTATE=P2-SPRENCLR (measured: P2-COMBINE FAIL, every precondition PASS, both cores)
// MUTATION P2-COMBINE stimulus red BPL4PT at the all-zero image inside the area of plane 4, via NMH_MUTATE=P2-BPL4ZERO (measured: P2-COMBINE FAIL, every precondition PASS, both cores)
//
// MUTATION P3-FIXTURE-LOADED stimulus red the program file does not exist, via NMH_MUTATE=P3-NOBIN (measured: P3-FIXTURE-LOADED FAIL, the four P3 lines NOT GRADED, both cores)
// MUTATION P3-FIXTURE-TRIGGERS stimulus red DMACON written without COPEN so the Copper never fetches, via NMH_MUTATE=P3-NOCOPEN (measured: P3-FIXTURE-TRIGGERS FAIL, the four P3 lines NOT GRADED, both cores)
// MUTATION P3-FIXTURE-DRIVEN stimulus red the program never stores the DONE mark, via NMH_MUTATE=P3-NODONE (measured: P3-FIXTURE-DRIVEN FAIL, the four P3 lines NOT GRADED, both cores)
// MUTATION P3-PRE-RECORDS stimulus red the V1 record of every field carries event id 5 instead of 6, via NMH_MUTATE=P3-EVMIS (measured: P3-PRE-RECORDS FAIL, the four P3 lines NOT GRADED, both cores)
// MUTATION P3-PRE-ANCHOR stimulus red second A anchor WAIT moved to line 101 instead of 100, via NMH_MUTATE=P3-ANCHOR (measured: P3-PRE-ANCHOR FAIL, the four P3 lines NOT GRADED, both cores)
// MUTATION P3-PRE-READTIME stimulus red the V1 wait of every field is about 30 lines (three halves of the calibrated count) instead of about 2, via NMH_MUTATE=P3-READTIME (measured: P3-PRE-READTIME FAIL, the four P3 lines NOT GRADED, both cores)
// MUTATION P3-PRE-DDRA stimulus red CIAADDRA bits 6-7 forced to outputs instead of fixed to inputs, via NMH_MUTATE=P3-DDRA (measured: the value read after the branch has bits 6 and 7 set, P3-PRE-DDRA FAIL, the four P3 lines NOT GRADED, both cores)
// MUTATION P3-PRE-PULSE stimulus red the F1 trigger pulse omitted from the drive schedule, via NMH_MUTATE=P3-NOPULSE (measured: P3-PRE-PULSE FAIL on both cores, graded against the nominal 3-pulse schedule, the four P3 lines NOT GRADED)
// MUTATION P3-PRE-LIVE stimulus red the E40 WAIT at line 44 instead of line 40, via NMH_MUTATE=P3-E40LINE44 (measured: E40 reads V=44 or 45, P3-PRE-LIVE FAIL, the four P3 lines NOT GRADED, both cores)
// MUTATION P3-PRE-PERIOD stimulus red the ECS-NTSC build BEAMCON0 write ($0000) run against a PAL-configured harness (bit 5 opposite the selected standard), via NMH_MUTATE=P3-BEAMCON0 (measured: the field period after the write is the NTSC one, P3-PRE-PERIOD FAIL on both cores and on both RTLs, every other precondition PASS, the four P3 lines NOT GRADED)
// MUTATION P3-HELD stimulus red the LPEN-clear build substituted for the LPEN-set build under the same trigger schedule, via NMH_MUTATE=P3-LPEN, run against the private copy described at P3-HELD through the unchanged comparators (measured: every precondition PASS, P3-HELD FAIL on both cores; on the repository's RTL the line is red in the primary run already)
// MUTATION P3-PEN-POSITION stimulus red the LPEN-clear build, via NMH_MUTATE=P3-LPEN, run against the private copy described at P3-HELD through the unchanged comparators (measured: every precondition PASS, P3-PEN-POSITION FAIL on both cores)
// MUTATION P3-LATCHED-PEN-POSITION stimulus red the LPEN-clear build, via NMH_MUTATE=P3-LPEN, run against the private copy described at P3-HELD through the unchanged comparators (measured: every precondition PASS, P3-LATCHED-PEN-POSITION FAIL on both cores)
// MUTATION P3-FIELD-END-LATCH stimulus red the LPEN-clear build, via NMH_MUTATE=P3-LPEN, run against the private copy described at P3-HELD through the unchanged comparators (measured: every precondition PASS, P3-FIELD-END-LATCH FAIL on both cores)
// MUTATION P3-HELD implementation green a private copy of the RTL outside the repository in which agnus_beamcounter.v stores BPLCON0 bit 3, latches vpos and hpos on a falling edge of pin 6 after line 20 or at the end of the field, releases the latch at vpos 20 and returns it on VPOSR and VHPOSR unless ECS LPENDIS is set (pp. 238-239, p. 305), and agnus.v and minimig.v carry the AND of both fire inputs to it, built with make NANOMIG=<copy>: the line is PASS in OCS-PAL, ECS-PAL, OCS-NTSC and ECS-NTSC on both cores, and the repository's RTL is red there because VPOSR and VHPOSR read the live beam
// MUTATION P3-PEN-POSITION implementation green the private copy described at P3-HELD: V(L1)=100, V(L3)=100, V(L5)=180 and H(L3)-H(L1)=40 in all four cases on both cores, where the repository's RTL reads V=110, 110, 190
// MUTATION P3-PRE-PULSE stimulus red the F1 trigger at t(A1)+30 colour clocks instead of +16, via NMH_MUTATE=P3-OFFSET (measured: the drive log differs from the nominal schedule plan_triggers(dry.log,16), P3-PRE-PULSE FAIL, the four P3 lines NOT GRADED, both cores and both RTLs)
// MUTATION P3-LATCHED-PEN-POSITION implementation green the private copy described at P3-HELD: PASS in all four cases on both cores, where the repository's RTL is red
// MUTATION P3-FIELD-END-LATCH implementation green the private copy described at P3-HELD: V1 and V2 of F1, F3, F5 and F7 are $13801 in PAL and $10601 in NTSC, PASS in all four cases on both cores, where the repository's RTL reads the live beam (V1 at line 2, V2 at line 11)
//
// MUTATION P4-FIXTURE-LOADED stimulus red the program file does not exist, via NMH_MUTATE=P4-NOBIN (measured: P4-FIXTURE-LOADED FAIL, P4-LPENDIS-READ NOT GRADED, both cores)
// MUTATION P4-FIXTURE-A1 stimulus red DMACON written without COPEN so the Copper never fetches, via NMH_MUTATE=P4-NOCOPEN (measured: P4-FIXTURE-A1 FAIL, P4-LPENDIS-READ NOT GRADED, both cores)
// MUTATION P4-FIXTURE-DRIVEN stimulus red the program never stores the DONE mark, via NMH_MUTATE=P4-NODONE (measured: P4-FIXTURE-DRIVEN FAIL, P4-LPENDIS-READ NOT GRADED, both cores)
// MUTATION P4-PRE-DDRA stimulus red CIAADDRA bits 6-7 forced to outputs instead of fixed to inputs, via NMH_MUTATE=P4-DDRA (measured: the value read after the branch has bits 6 and 7 set, P4-PRE-DDRA FAIL, P4-LPENDIS-READ NOT GRADED, both cores)
// MUTATION P4-PRE-ANCHOR stimulus red the second A anchor WAITs for line 101 instead of 100, via NMH_MUTATE=P4-ANCHOR (measured: P4-PRE-ANCHOR FAIL on both cores, every other precondition as in the primary run)
// MUTATION P4-PRE-READTIME stimulus red 40 poll iterations between each BEAMCON0 write and the second read, via NMH_MUTATE=P4-YDELAY (measured: P4-PRE-READTIME FAIL on both cores, every other precondition as in the primary run)
// MUTATION P4-PRE-PULSE stimulus red a third pulse in F2, 16 colour clocks after F2's own A1 fetch and after Y's store, via NMH_MUTATE=P4-EXTRAPULSE-F2 (measured: P4-PRE-PULSE FAIL on both cores, graded against the nominal 2-pulse schedule; on the private copy described at P3-HELD no other precondition moves)
// MUTATION P4-PRE-PERIOD stimulus red every BEAMCON0 write has bit 5 clear (the other standard), via NMH_MUTATE=P4-PERIOD (measured: the field period changes and P4-PRE-PERIOD FAIL on both cores; P4-PRE-REFFRAME prints NOT GRADED)
// MUTATION P4-PRE-REFFRAME stimulus red the delay after E250 is 10 poll iterations instead of the calibrated count, via NMH_MUTATE=P4-SHORT (measured: P4-PRE-REFFRAME FAIL on both cores, every other precondition as in the primary run)
// MUTATION P4-PRE-LATCHF1 stimulus red BPLCON0 LPEN clear from E60 of F1 until Y is stored (set again before F2), the nominal pulses unchanged, via NMH_MUTATE=P4-F1NOLPEN, run against the private copy described at P3-HELD through the unchanged comparators (measured: Y reads the live beam, V=270, P4-PRE-LATCHF1 FAIL on both cores, every other precondition PASS, P4-LPENDIS-READ NOT GRADED)
// MUTATION P4-PRE-LATCHF1 implementation green the private copy described at P3-HELD: V(Y)=100 and H(Y)=H(Y') on both cores, where the repository's RTL reads V=270
// MUTATION P4-PRE-CONTROL stimulus red the E60 write of F3 changed to $2020 instead of $0020 (LPENDIS set in the control field too), via NMH_MUTATE=P4-F3LPENDIS, run against the private copy described at P3-HELD through the unchanged comparators (measured: X' reads V8=1 and differs from Y', P4-PRE-CONTROL FAIL on both cores, every other precondition PASS, P4-LPENDIS-READ NOT GRADED)
// MUTATION P4-PRE-CONTROL implementation green the private copy described at P3-HELD: X'=Y' and V(X')=100 on both cores, where the repository's RTL reads V=270 and X' differs from Y'
// MUTATION P4-LPENDIS-READ implementation green the private copy described at P3-HELD: every precondition holds and the line is PASS on both cores (X reads V8=1), where the repository's RTL leaves it NOT GRADED because P4-PRE-LATCHF1 and P4-PRE-CONTROL are red
// MUTATION P4-LPENDIS-READ stimulus red the E60 write of F1 changed to $0020 instead of $2020 (LPENDIS never set), via NMH_MUTATE=P4-NOLPENDIS, run against the private copy described at P3-HELD through the unchanged comparator (measured: every precondition holds, X is (0,100) and the line is FAIL on both cores); against the repository's RTL the line is NOT GRADED in the primary run and in this run
//
// The repository's RTL reads FAIL on P3-HELD, P3-PEN-POSITION, P3-LATCHED-PEN-POSITION and P3-FIELD-END-LATCH in all four cases and on P4-PRE-LATCHF1 and P4-PRE-CONTROL: VPOSR and VHPOSR return the live beam counter, never a value held between a trigger and the end of internal vertical blanking (p. 239). P4-LPENDIS-READ therefore prints NOT GRADED. Each of these lines has an implementation record that turns it green on a private copy of the RTL.

#include <algorithm>
#include <cstdint>
#include <cctype>
#include <cstdio>
#include <iostream>
#include <map>
#include <set>
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

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

// ---- program-side addresses (prog_lp.s) --------------------------------

constexpr uint32_t kResults  = 0x0C0000;
constexpr uint32_t kDoneAddr = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;

constexpr uint32_t kCopList  = 0x0C1000;
constexpr uint32_t kCopA1    = kCopList + 20; // register-offset word of the A1 MOVE (COLOR00<-$F00)
constexpr uint32_t kCopA2    = kCopList + 28; // A2 MOVE (COLOR00<-$0000)
constexpr uint32_t kCopB1    = kCopList + 52; // B1 MOVE (COLOR00<-$00F)
constexpr uint32_t kCopB2    = kCopList + 60; // B2 MOVE (COLOR00<-$0000)
// register-offset word of each COPER-signalling MOVE (INTREQ<-$8010)
constexpr uint32_t kCopE40  = kCopList + 4;
constexpr uint32_t kCopE60  = kCopList + 12;
constexpr uint32_t kCopE110 = kCopList + 36;
constexpr uint32_t kCopE130 = kCopList + 44;
constexpr uint32_t kCopE190 = kCopList + 68;
constexpr uint32_t kCopE210 = kCopList + 76;

constexpr uint32_t kPreTick0 = 0x0C1100;
constexpr uint32_t kPreTick1 = 0x0C1102;
constexpr uint32_t kMainTick = 0x0C1104; // 9 words, one per field (F-1..F7)
constexpr uint32_t kRecords  = 0x0C2000; // 5 words/record, see record_index()
constexpr uint32_t kCalib    = 0x0C1180; // long: poll iterations over the 20 lines E110..E130 (prog_lp.s, prog_p4.s)

// The programs start the Copper right after a VERTB, so its first pass through
// the whole list (E40..E210, including A1/A2/B1/B2) runs before the CPU's own
// main_loop sees its first VERTB. That first pass carries no field number
// (it stores 6 records with field word $FFFF, ahead of field 0's own block;
// P3-PRE-RECORDS asserts both the count and the words). Every array built
// directly from a Copper-fetch address (A1/B1/E40../E210's own
// read_cycles_at()) therefore has this one extra leading entry that
// MAINTICK/RECORDS (CPU-authored, only touched from inside main_loop) do
// not: kPhantomPasses skips it.
constexpr size_t kPhantomPasses = 1;

// One colour clock (one memory slot) equals 8 clk_sys cycles at the harness
// boundary (ch6-blitter-cpu-sharing's own kCck, reused here). The ratio is
// confirmed by P3-PRE-ANCHOR and P3-PRE-READTIME below: both compare a
// page-stated colour-clock interval against a clk_sys cycle count through
// this constant, and a wrong ratio would put every field's measurement far
// outside their small tolerances, not just occasionally.
constexpr uint64_t kCck = 8;
constexpr uint64_t kLineCycles = 227 * kCck; // p.23/p.304's own colour-clocks-per-line, in clk_sys
constexpr uint64_t kLongLineCycles = 228 * kCck; // p.23: one line is 227 or 228 colour clocks; the bound of the period checks

constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudget  = 9'000'000; // > 9 PAL fields + spin margin

uint32_t word_addr(uint32_t byte_addr) { return byte_addr / 2; }

// ---- capture/analysis helpers -------------------------------------------

std::vector<uint64_t> read_cycles_at(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr) {
    uint32_t w = word_addr(byte_addr);
    std::vector<uint64_t> out;
    for (const auto& a : log) if (!a.is_write && a.word_addr == w) out.push_back(a.cycle);
    return out;
}

std::vector<uint64_t> write_cycles_at(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr) {
    uint32_t w = word_addr(byte_addr);
    std::vector<uint64_t> out;
    for (const auto& a : log) if (a.is_write && a.word_addr == w) out.push_back(a.cycle);
    return out;
}

struct Rec {
    bool present = false;
    uint16_t field = 0xFFFF, event = 0xFFFF;
    uint16_t vposr_long = 0, vhposr_long = 0, vhposr_word = 0;
};

// Record layout (see prog_lp.s's own header comment, and kPhantomPasses
// above): a leading 6-record phantom block (field==-1, no V1/V2, events
// E40..E210 only) precedes 8 records per field for field index 0-7
// (F-1..F6), in store order V1,V2,E40,E60,E110,E130,E190,E210; field index
// 8 (F7) stores only V1,V2 (2 records) and then calls DONE. Total: 6 + 8*8
// + 2 = 72 records.
constexpr int kPhantomRecords = 6;
constexpr int kTotalRecords = 72;
int record_index(int field, int event) {
    // event: 0=E40,1=E60,2=E110,3=E130,4=E190,5=E210,6=V1,7=V2
    static const int pos[8] = {2, 3, 4, 5, 6, 7, 0, 1}; // pos[event] within a field's 8-record block
    if (field < 0 || field > 8) return -1;
    if (field == 8 && event != 6 && event != 7) return -1;
    int p = (event == 6) ? 0 : (event == 7) ? 1 : pos[event];
    return kPhantomRecords + field * 8 + p;
}

struct Records {
    std::vector<Rec> recs; // indexed by record_index()
    std::vector<uint64_t> store_cycle; // same indexing: cycle of the write to that record's first word
};

Records read_records(const nmh::NanomigHarness& /*unused, kept for symmetry*/) { return {}; }

Records parse_records(const std::vector<std::optional<uint16_t>>& mem_words /* kTotalRecords*5 words from kRecords */,
                       const std::vector<nmh::RamAccess>& log) {
    Records r;
    r.recs.resize(kTotalRecords);
    r.store_cycle.resize(kTotalRecords, 0);
    for (int i = 0; i < kTotalRecords; ++i) {
        size_t base = (size_t)i * 5;
        if (base + 4 >= mem_words.size()) continue;
        if (!mem_words[base] || !mem_words[base+1] || !mem_words[base+2] || !mem_words[base+3] || !mem_words[base+4]) continue;
        Rec rec;
        rec.present = true;
        // words: field, event, vposr_long, vhposr_long, vhposr_word
        rec.field = *mem_words[base+0];
        rec.event = *mem_words[base+1];
        rec.vposr_long  = *mem_words[base+2];
        rec.vhposr_long = *mem_words[base+3];
        rec.vhposr_word = *mem_words[base+4];
        r.recs[i] = rec;
        auto w = write_cycles_at(log, kRecords + 10u * (uint32_t)i);
        r.store_cycle[i] = w.empty() ? 0 : w.front();
    }
    return r;
}

uint32_t M_of(const Rec& r) { return ((uint32_t)(r.vposr_long & 1) << 16) | r.vhposr_long; }
uint32_t V_of(const Rec& r) { return M_of(r) >> 8; }
uint32_t H_of(const Rec& r) { return M_of(r) & 0xFF; }

// ---- Config combinations --------------------------------------------------

nmh::Config cfg_for(bool ecs, bool ntsc) {
    nmh::Config c;
    c.chipset_config = (ecs ? 0x08 : 0x00) | (ntsc ? 0x02 : 0x00);
    return c;
}

// ---- dry run: never drives pin 6, gives fetch/tick timing ----------------

struct DryResult {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log; // coalesced
};

DryResult dry_run(const std::string& bin, nmh::Config cfg) {
    DryResult r;
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;
    r.done = h.run_until_write(kDoneAddr, kDoneValue, kRunBudget);
    r.log = nmh::coalesce(h.ram_log());
    return r;
}

// ---- trigger schedule, derived from a dry run ----------------------------

struct TriggerPlan {
    bool ok = false;
    uint64_t f1 = 0, f3 = 0, f5 = 0; // absolute clk_sys cycle to assert pin 6
};

// f1_offset_cck lets P3-OFFSET reuse this with a different F1 placement.
TriggerPlan plan_triggers(const std::vector<nmh::RamAccess>& log, uint64_t f1_offset_cck = 16) {
    TriggerPlan p;
    auto a1 = read_cycles_at(log, kCopA1);
    auto b1 = read_cycles_at(log, kCopB1);
    if (a1.size() < 2 + kPhantomPasses || b1.size() < 6 + kPhantomPasses) return p;
    p.f1 = a1[kPhantomPasses + 2] + f1_offset_cck * kCck; // field index 2 = F1
    p.f3 = a1[kPhantomPasses + 4] + 56 * kCck;            // field index 4 = F3
    p.f5 = b1[kPhantomPasses + 6] + 16 * kCck;            // field index 6 = F5
    p.ok = true;
    return p;
}

std::vector<uint64_t> field_tick_cycles(const std::vector<nmh::RamAccess>& log) {
    std::vector<uint64_t> out;
    for (int i = 0; i < 9; ++i) {
        auto w = write_cycles_at(log, kMainTick + 2 * (uint32_t)i);
        out.push_back(w.empty() ? 0 : w.front());
    }
    return out;
}

// ---- driven run: replays the same program, driving pin 6 on schedule ----

struct DrivenResult {
    bool loaded = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log; // coalesced
    std::vector<std::pair<uint64_t,uint64_t>> pulses; // actual (assert,release) cycles achieved
    std::vector<std::optional<uint16_t>> mem; // kTotalRecords*5 words from kRecords
    std::optional<uint16_t> ddra_stored;
};

struct HoldWindow { uint64_t begin, end; };

DrivenResult driven_run(const std::string& bin, nmh::Config cfg,
                         std::vector<uint64_t> trigger_cycles, uint64_t pulse_len_cycles,
                         std::optional<HoldWindow> hold = std::nullopt,
                         size_t record_words = (size_t)kTotalRecords * 5) {
    DrivenResult r;
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    h.boot(kBootBudget);
    std::sort(trigger_cycles.begin(), trigger_cycles.end());
    size_t next = 0;
    bool active = false;
    uint64_t assert_at = 0, release_at = 0;
    size_t seen = 0;
    const uint64_t start = h.cycles();
    bool done_found = false;
    while (h.cycles() - start < kRunBudget) {
        uint64_t c = h.cycles();
        bool forced = hold && c >= hold->begin && c < hold->end;
        if (forced) {
            h.ports()->joystick0 |= 0x10;
            h.ports()->joystick1 |= 0x10;
            if (!active) { active = true; assert_at = c; }
        } else if (!active && next < trigger_cycles.size() && c >= trigger_cycles[next]) {
            h.ports()->joystick0 |= 0x10;
            h.ports()->joystick1 |= 0x10;
            active = true;
            assert_at = c;
            release_at = c + pulse_len_cycles;
            ++next;
        } else if (active && !(hold && c < hold->end) && c >= release_at) {
            h.ports()->joystick0 &= ~0x10;
            h.ports()->joystick1 &= ~0x10;
            active = false;
            r.pulses.push_back({assert_at, release_at});
        } else if (active && hold && c >= hold->end) {
            h.ports()->joystick0 &= ~0x10;
            h.ports()->joystick1 &= ~0x10;
            active = false;
            r.pulses.push_back({assert_at, c});
        }
        h.run_cycles(1);
        const auto& log = h.ram_log();
        for (; seen < log.size(); ++seen) {
            if (log[seen].is_write && log[seen].word_addr == word_addr(kDoneAddr) && log[seen].data == kDoneValue) {
                r.done = log[seen].cycle;
                done_found = true;
            }
        }
        if (done_found) break;
    }
    if (active) {
        h.ports()->joystick0 &= ~0x10;
        h.ports()->joystick1 &= ~0x10;
        r.pulses.push_back({assert_at, h.cycles()});
    }
    r.log = nmh::coalesce(h.ram_log());
    r.ddra_stored = h.peek_logical(kResults);
    for (size_t w = 0; w < record_words; ++w)
        r.mem.push_back(h.peek_logical(kRecords + 2u * (uint32_t)w));
    return r;
}

} // namespace

// ======================================================================
// P1 / P2: video-output analysis (SPECIFICATION.md, "Playfield geometry
// and row tags", "Reference frames and how the test checks them")
// ======================================================================
namespace pri {

struct Col { uint8_t r, g, b; };
inline bool same(const Col& a, const Col& b) { return a.r == b.r && a.g == b.g && a.b == b.b; }
constexpr Col BG{0, 0, 0}, PF1C{0xF, 0, 0}, PF2C{0, 0xF, 0};
// SC0..SC7: the colours of sprites 0..7 (COLOR19, 17/18, 23, 21/22, 27, 25/26, 31, 29/30)
constexpr Col SC[8] = {{0xF,0xF,0}, {0xF,0,0xF}, {0,0xF,0xF}, {0xF,8,0}, {0,8,0xF}, {8,0xF,0}, {0xF,0xF,0xF}, {8,8,8}};
// P2: the colours of the planes 1..6 alone (COLOR01, 09, 02, 10, 04, 12)
constexpr Col P2C[6] = {{0xF,0,0}, {0,0xF,0}, {0xA,0,0}, {0,0xA,0}, {5,0,0}, {0,5,0}};

std::string cs(const Col& c) {
    char b[8];
    snprintf(b, sizeof b, "$%X%X%X", c.r, c.g, c.b);
    return b;
}

// ---- memory map of prog_pri.s / prog_p2.s ----
constexpr uint32_t kTicks = 0x0C1000, kMarks = 0x0C1100;
constexpr uint32_t kListBase = 0x30000, kListStep = 0x100;
constexpr uint32_t kImgLo = 0x10000, kImgHi = 0x1F000;      // P1: every bitplane image
constexpr uint32_t kP2Img = 0x40000, kP2Step = 0x6000;      // P2: plane n image region

struct Reads { std::vector<uint64_t> cyc; std::vector<uint32_t> addr; };

struct Trace {
    const std::vector<nmh::VideoSample>* video = nullptr;
    std::vector<nmh::Span> frames, lines;
    std::vector<uint64_t> tick, mark;     // cycle of the first write at each tick / marker address; 0 = never
    Reads list_reads, plane_reads;        // reads inside the Copper-list area / the bitplane image areas
};

Trace make_trace(const nmh::NanomigHarness& h, int ncfg, bool p2) {
    Trace t;
    t.video = &h.video();
    t.frames = nmh::find_frames(h.video());
    t.lines = nmh::find_lines(h.video());
    t.tick.assign((size_t)ncfg * 3, 0);
    t.mark.assign((size_t)ncfg, 0);
    auto log = nmh::coalesce(h.ram_log());
    for (const auto& a : log) {
        uint32_t byte = a.word_addr * 2;
        if (a.is_write) {
            if (byte >= kTicks && byte < kTicks + 2 * t.tick.size()) {
                uint64_t& c = t.tick[(byte - kTicks) / 2];
                if (!c) c = a.cycle;
            } else if (byte >= kMarks && byte < kMarks + 2 * t.mark.size()) {
                uint64_t& c = t.mark[(byte - kMarks) / 2];
                if (!c) c = a.cycle;
            }
            continue;
        }
        if (byte >= kListBase && byte < kListBase + 0x1000) {
            t.list_reads.cyc.push_back(a.cycle);
            t.list_reads.addr.push_back(byte);
        } else if (p2 ? (byte >= kP2Img && byte < kP2Img + 6 * kP2Step) : (byte >= kImgLo && byte < kImgHi)) {
            t.plane_reads.cyc.push_back(a.cycle);
            t.plane_reads.addr.push_back(byte);
        }
    }
    return t;
}

// index of the video frame (vs rising edge to the next) holding a cycle; -1 if none
int frame_at(const Trace& t, uint64_t cycle) {
    int best = -1;
    for (size_t i = 0; i < t.frames.size(); ++i)
        if (t.frames[i].begin <= cycle && cycle < t.frames[i].end) best = (int)i;
    return best;
}

inline Col at(const Trace& t, size_t i) {
    const auto& v = (*t.video)[i];
    return {v.r, v.g, v.b};
}

// One output row that shows tag block 0: the first sample of PF1C, and the
// width of the PF1C run starting there.
struct Row { size_t b = 0, e = 0; long j0 = 0; int w0 = 0; int id = -1; };

std::vector<Row> raw_rows(const Trace& t, int fi) {
    std::vector<Row> rows;
    const nmh::Span fr = t.frames[(size_t)fi];
    auto it = std::lower_bound(t.lines.begin(), t.lines.end(), fr.begin,
                               [](const nmh::Span& s, size_t v) { return s.begin < v; });
    for (; it != t.lines.end() && it->end <= fr.end; ++it) {
        Row r;
        r.b = it->begin; r.e = it->end;
        long j0 = -1;
        for (size_t i = r.b; i < r.e; ++i)
            if (same(at(t, i), PF1C)) { j0 = (long)(i - r.b); break; }
        if (j0 < 0) continue;
        int w = 0;
        for (size_t i = r.b + (size_t)j0; i < r.e && same(at(t, i), PF1C); ++i) ++w;
        r.j0 = j0; r.w0 = w;
        rows.push_back(r);
    }
    return rows;
}

// Decodes a row's tag with k samples per lores pixel: block 0 must be PF1C,
// blocks 1-4 each wholly PF1C (1) or wholly BG (0).
int decode_id(const Trace& t, const Row& r, int k) {
    if (k <= 0) return -1;
    if (r.j0 + 40L * k > (long)(r.e - r.b)) return -1;
    int id = 0;
    for (int blk = 0; blk < 5; ++blk) {
        int ones = 0, zeros = 0;
        for (int s = 0; s < 8 * k; ++s) {
            Col c = at(t, r.b + (size_t)(r.j0 + (long)blk * 8 * k + s));
            if (same(c, PF1C)) ++ones; else if (same(c, BG)) ++zeros;
        }
        if (blk == 0) { if (ones != 8 * k) return -1; continue; }
        if (ones == 8 * k) id = id * 2 + 1;
        else if (zeros == 8 * k) id = id * 2;
        else return -1;
    }
    return id;
}

// the colour at lores-relative sample rel of a row (nullopt outside the line)
inline std::optional<Col> sample(const Trace& t, const Row& r, long rel) {
    long off = r.j0 + rel;
    if (off < 0 || off >= (long)(r.e - r.b)) return std::nullopt;
    return at(t, r.b + (size_t)off);
}

struct Run { long start = 0, len = 0; Col col{0,0,0}; };

// maximal runs of one non-BG colour between x = 40 and x = 320 of a row
std::vector<Run> runs_of(const Trace& t, const Row& r, int k) {
    std::vector<Run> out;
    for (long rel = 40L * k; rel < 320L * k; ++rel) {
        auto c = sample(t, r, rel);
        if (!c || same(*c, BG)) continue;
        if (!out.empty() && out.back().start + out.back().len == rel && same(out.back().col, *c)) ++out.back().len;
        else out.push_back({rel, 1, *c});
    }
    return out;
}

bool runs_equal(const std::vector<Run>& a, const std::vector<Run>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (a[i].start != b[i].start || a[i].len != b[i].len || !same(a[i].col, b[i].col)) return false;
    return true;
}

} // namespace pri

namespace pri {

struct Cfg { char group; const char* name; int v, w, pri; int slot_spr[8]; };
// SPECIFICATION.md "Configurations": A1, A2, B1-B8, E0-E4. slot_spr[j] is the sprite in slot j.
const Cfg kCfgs[15] = {
    {'A', "A1", 0, 0, 0, {0,1,2,3,4,5,6,7}},
    {'A', "A2", 0, 0, 0, {7,0,6,1,5,2,4,3}},
    {'B', "B1", 0, 4, 0, {}}, {'B', "B2", 1, 0, 1, {}}, {'B', "B3", 2, 1, 1, {}}, {'B', "B4", 3, 2, 1, {}},
    {'B', "B5", 4, 3, 1, {}}, {'B', "B6", 2, 2, 0, {}}, {'B', "B7", 2, 2, 1, {}}, {'B', "B8", 0, 2, 1, {}},
    {'E', "E0", 0, 0, 0, {}}, {'E', "E1", 1, 1, 0, {}}, {'E', "E2", 2, 2, 0, {}},
    {'E', "E3", 3, 3, 0, {}}, {'E', "E4", 4, 4, 0, {}},
};
constexpr int kB1 = 2, kB7 = 8;

struct Refs {
    bool ok = false;
    long R[8] = {0,0,0,0,0,0,0,0};          // B/E: start of sprite i's run
    long slot[8] = {0,0,0,0,0,0,0,0};       // A: start of slot j's run
};

struct CfgRes {
    bool frame_found = false;
    bool pre_frame = false, pre_tags = false, pre_sprites = false, pre_play = true, pre_vext = true;
    bool vext_eval = false;   // the vertical-extent check ran (a group-A configuration or B7 with its sprite references)
    std::string d_frame, d_tags, d_sprites, d_play, d_vext;
    long compared = 0, wrong = 0, ungraded = 0, outside = 0;
    std::string first_wrong, ungraded_seen;
    std::map<int, int> rows_by_id;
    size_t rows_total = 0;
};

// expected colour at lores-relative sample rel of a row with tag id (nullopt = the pages leave it open)
std::optional<Col> expected(const Cfg& c, const Refs& rf, int id, long rel, int k) {
    if (rel < 0 || rel >= 320L * k) return BG;                  // border margins
    long x = rel / k;
    if (x < 40) {                                               // tag region
        int blk = (int)(x / 8);
        if (blk == 0) return PF1C;
        return ((id >> (4 - blk)) & 1) ? PF1C : BG;
    }
    auto in_run = [&](long s) { return rel >= s && rel < s + 16L * k; };
    if (id == 0) return BG;
    if (c.group == 'A') {
        int best = -1;
        for (int j = 0; j < 8; ++j) {
            bool even = (j % 2) == 0;
            if (id == 1 && !even) continue;                     // band E: only the even slots
            if (id == 3 && even) continue;                      // band O: only the odd slots
            if (in_run(rf.slot[j])) { int s = c.slot_spr[j]; if (best < 0 || s < best) best = s; }
        }
        if (id < 1 || id > 3) return std::nullopt;
        return best >= 0 ? SC[best] : BG;
    }
    int sp = -1;
    for (int i = 0; i < 8; ++i) if (in_run(rf.R[i])) sp = i;
    if (c.group == 'E') {
        if (id == 2) { if (sp >= 0 && sp / 2 < c.v) return SC[sp]; return PF1C; }
        if (id == 6) return PF1C;
        return std::nullopt;
    }
    // group B
    if (sp >= 0 && id >= 1 && id <= 4) {
        int g = sp / 2;
        switch (id) {
        case 1: return SC[sp];
        case 2: return g < c.v ? SC[sp] : PF1C;
        case 3: return g < c.w ? SC[sp] : PF2C;
        default:
            if (g < c.v && g < c.w) return SC[sp];
            if (c.pri == 0 && g >= c.v) return PF1C;
            if (c.pri == 1 && g >= c.w) return PF2C;
            return std::nullopt;
        }
    }
    switch (id) {
    case 1: case 5: return BG;
    case 2: case 6: return PF1C;
    case 3: case 7: return PF2C;
    case 4: case 8: return c.pri == 0 ? PF1C : PF2C;
    default: return std::nullopt;
    }
}

struct P1Out {
    bool fixture_ok = false; std::string d_fixture;
    int k = 0; bool k_ok = false;
    bool window_ok = false; std::string d_window;
    CfgRes cfg[15];
    Refs refs[15];
};

P1Out analyze_p1(const Trace& t) {
    P1Out o;
    // ---- fixture: ticks and markers in order, one marker between t0 and t1 of each configuration
    {
        bool ok = true; std::string d;
        for (size_t i = 0; i < t.tick.size(); ++i) {
            if (!t.tick[i]) { ok = false; d += " tick" + std::to_string(i) + "=missing"; }
            else if (i && t.tick[i-1] && t.tick[i] <= t.tick[i-1]) { ok = false; d += " tick" + std::to_string(i) + "=out-of-order"; }
        }
        for (int k = 0; k < 15; ++k) {
            uint64_t m = t.mark[(size_t)k], t0 = t.tick[(size_t)(3*k)], t1 = t.tick[(size_t)(3*k+1)];
            if (!m || !(t0 < m && m < t1)) { ok = false; d += " marker" + std::to_string(k) + "=not-between-t0-and-t1"; }
        }
        o.fixture_ok = ok; o.d_fixture = d.empty() ? "45 ticks in order, 15 markers each between its t0 and t1" : d;
    }
    std::vector<Row> raw[15];
    int fidx[15];
    int kmin = 0;
    for (int k = 0; k < 15; ++k) {
        fidx[k] = -1;
        uint64_t t1 = t.tick[(size_t)(3*k+1)], t2 = t.tick[(size_t)(3*k+2)];
        if (!t1 || !t2) continue;
        fidx[k] = frame_at(t, (t1 + t2) / 2);
        o.cfg[k].frame_found = fidx[k] >= 0;
        if (fidx[k] < 0) continue;
        raw[k] = raw_rows(t, fidx[k]);
        for (const auto& r : raw[k]) if (!kmin || r.w0 < kmin) kmin = r.w0;
    }
    o.k = (kmin > 0 && kmin % 8 == 0) ? kmin / 8 : 0;
    o.k_ok = o.k > 0;
    // ---- window columns: block 0 is 8k wide in every row without a set tag bit, and s0 is one index everywhere
    {
        std::set<long> j0s; bool w_ok = o.k_ok; size_t n0 = 0, nrows = 0; std::string d;
        for (int k = 0; k < 15; ++k)
            for (auto& r : raw[k]) {
                r.id = decode_id(t, r, o.k);
                if (r.id < 0) continue;
                ++nrows; j0s.insert(r.j0);
                if (r.id == 0) { ++n0; if (r.w0 != 8 * o.k) { w_ok = false; d = "a row with id 0 has block 0 of " + std::to_string(r.w0) + " samples"; } }
            }
        if (n0 == 0) { w_ok = false; d += " no row with id 0"; }
        if (j0s.size() != 1) { w_ok = false; d += " s0 takes " + std::to_string(j0s.size()) + " different values"; }
        o.window_ok = w_ok;
        o.d_window = "k=" + std::to_string(o.k) + " (kmin=" + std::to_string(kmin) + "), " + std::to_string(nrows) +
                     " decoded rows, s0 values: " + std::to_string(j0s.size()) +
                     (j0s.empty() ? "" : " first=" + std::to_string(*j0s.begin())) + (d.empty() ? "" : " --" + d);
    }
    // ---- per configuration
    for (int c = 0; c < 15; ++c) {
        CfgRes& R = o.cfg[c];
        const Cfg& cf = kCfgs[c];
        if (!R.frame_found) { R.d_frame = "no video frame holds the evaluated field"; continue; }
        const int k = o.k;
        // frame association: Copper fetches in the field, list c only, the list's end before the first bitplane fetch
        {
            uint64_t t1 = t.tick[(size_t)(3*c+1)], t2 = t.tick[(size_t)(3*c+2)];
            uint32_t lo = kListBase + kListStep * (uint32_t)c, hi = lo + kListStep;
            int planes = cf.group == 'E' ? 1 : 2;
            uint32_t end_word = lo + 2u * (uint32_t)(4 + 4 * planes + 32);
            size_t n = 0, outside = 0; uint64_t first_end = 0, first_plane = 0;
            for (size_t i = 0; i < t.list_reads.cyc.size(); ++i) {
                uint64_t cy = t.list_reads.cyc[i];
                if (cy < t1 || cy >= t2) continue;
                ++n;
                uint32_t a = t.list_reads.addr[i];
                if (a < lo || a >= hi) ++outside;
                else if (a == end_word && !first_end) first_end = cy;
            }
            for (size_t i = 0; i < t.plane_reads.cyc.size(); ++i) {
                uint64_t cy = t.plane_reads.cyc[i];
                if (cy >= t1 && cy < t2) { first_plane = cy; break; }
            }
            R.pre_frame = n > 0 && outside == 0 && first_end && first_plane && first_end < first_plane;
            R.d_frame = std::to_string(n) + " list reads in the field, " + std::to_string(outside) + " outside list " +
                        std::to_string(c) + ", end WAIT first fetched at +" + (first_end ? std::to_string(first_end - t1) : "never") +
                        ", first bitplane fetch at +" + (first_plane ? std::to_string(first_plane - t1) : "never");
        }
        // tags: every row decodes, bands in table order with the table's relative counts
        std::vector<Row>& rows = raw[c];
        std::vector<int> seq;
        size_t undecoded = 0;
        for (auto& r : rows) {
            if (r.id < 0) ++undecoded;
            else { R.rows_by_id[r.id]++; if (r.id != 0) seq.push_back(r.id); }
        }
        R.rows_total = rows.size();
        {
            std::vector<int> bands = cf.group == 'A' ? std::vector<int>{1,2,3}
                                   : cf.group == 'B' ? std::vector<int>{1,2,3,4,5,6,7,8} : std::vector<int>{2,6};
            bool ok = undecoded == 0 && !seq.empty() && std::is_sorted(seq.begin(), seq.end());
            int n = R.rows_by_id.count(bands[0]) ? R.rows_by_id[bands[0]] : 0;
            if (cf.group == 'E') n = R.rows_by_id.count(6) ? R.rows_by_id[6] : 0;
            for (int b : bands) {
                int cnt = R.rows_by_id.count(b) ? R.rows_by_id[b] : 0;
                int want = n;
                if (cf.group == 'B' && b == 5) want = n / 2;
                if (cf.group == 'E' && b == 2) want = 2 * n;
                if (cnt == 0 || cnt != want || (cf.group == 'B' && b == 5 && n % 2)) ok = false;
            }
            for (auto& kv : R.rows_by_id) if (kv.first != 0 && std::find(bands.begin(), bands.end(), kv.first) == bands.end()) ok = false;
            R.pre_tags = ok;
            std::string d = std::to_string(undecoded) + " undecodable rows; rows per id:";
            for (auto& kv : R.rows_by_id) d += " " + std::to_string(kv.first) + ":" + std::to_string(kv.second);
            R.d_tags = d;
        }
        // sprite references
        Refs& rf = o.refs[c];
        auto band_runs = [&](int id, bool& same_all, bool& any) {
            std::vector<Run> first; same_all = true; any = false;
            for (auto& r : rows) {
                if (r.id != id) continue;
                auto ru = runs_of(t, r, k);
                if (!any) { first = ru; any = true; }
                else if (!runs_equal(first, ru)) same_all = false;
            }
            return first;
        };
        if (k > 0) {
            if (cf.group == 'A') {
                bool s1, a1, s3, a3;
                auto re = band_runs(1, s1, a1), ro = band_runs(3, s3, a3);
                bool ok = a1 && a3 && s1 && s3 && re.size() == 4 && ro.size() == 4;
                auto okrun = [&](const std::vector<Run>& v) {
                    for (size_t i = 0; i < v.size(); ++i) {
                        if (v[i].len != 16L * k) return false;
                        if (i && v[i].start != v[i-1].start + 16L * k) return false;
                        bool isc = false; for (auto& s : SC) if (same(s, v[i].col)) isc = true;
                        if (!isc) return false;
                    }
                    return true;
                };
                if (ok) ok = okrun(re) && okrun(ro);
                if (ok) for (int m = 0; m < 4; ++m) { rf.slot[2*m] = re[(size_t)m].start; rf.slot[2*m+1] = ro[(size_t)m].start; }
                rf.ok = ok; R.pre_sprites = ok;
                R.d_sprites = "band E: " + std::to_string(re.size()) + " runs" + (s1 ? "" : " (rows differ)") +
                              ", band O: " + std::to_string(ro.size()) + " runs" + (s3 ? "" : " (rows differ)");
            } else if (cf.group == 'B') {
                bool s, a;
                auto ru = band_runs(1, s, a);
                bool ok = a && s && ru.size() == 8;
                if (ok) for (int i = 0; i < 8; ++i) {
                    if (ru[(size_t)i].len != 16L * k || !same(ru[(size_t)i].col, SC[i])) ok = false;
                    if (i && ru[(size_t)i].start <= ru[(size_t)i-1].start) ok = false;
                    rf.R[i] = ru[(size_t)i].start;
                }
                rf.ok = ok; R.pre_sprites = ok;
                std::string d = "band S: " + std::to_string(ru.size()) + " runs" + (s ? "" : " (rows differ)") + ":";
                for (auto& r : ru) d += " " + cs(r.col) + "@" + std::to_string(r.start) + "+" + std::to_string(r.len);
                R.d_sprites = d;
            } else {
                R.pre_sprites = o.refs[kB1].ok;     // judged on B1's reference band
                if (o.refs[kB1].ok) for (int i = 0; i < 8; ++i) rf.R[i] = o.refs[kB1].R[i];
                rf.ok = o.refs[kB1].ok;
                R.d_sprites = "reference runs of B1 reused";
            }
        }
        // playfields present
        if (cf.group == 'B' || cf.group == 'E') {
            bool ok = k > 0; std::string d; int checked = 0;
            for (auto& r : rows) {
                int want = -1;
                if (r.id == 6) want = 1; else if (cf.group == 'B' && r.id == 7) want = 2;
                if (want < 0) continue;
                ++checked;
                for (long rel = 40L * k; rel < 320L * k && ok; ++rel) {
                    auto cc = sample(t, r, rel);
                    if (!cc || !same(*cc, want == 1 ? PF1C : PF2C)) { ok = false; d = "a row of id " + std::to_string(r.id) + " shows another colour at x=" + std::to_string(rel / k); }
                }
            }
            if (checked == 0) ok = false;
            R.pre_play = ok; R.d_play = std::to_string(checked) + " rows checked" + (d.empty() ? "" : "; " + d);
        }
        // vertical extent: Group B (B7: the sprites of groups 0 and 1 show in every row of the bands inside
        // their lines and in no row of band B0) and Group A (the sprites of every slot show in the rows of
        // the bands E, X, O that hold them and in no row after band O)
        if (c == kB7 && k > 0 && rf.ok) {
            bool ok = true; int rows_in = 0, rows_after = 0; std::string d;
            for (auto& r : rows) {
                if (r.id >= 1 && r.id <= 4) {
                    ++rows_in;
                    for (int i = 0; i < 4 && ok; ++i)
                        for (long rel = rf.R[i]; rel < rf.R[i] + 16L * k; ++rel) {
                            auto cc = sample(t, r, rel);
                            if (!cc || !same(*cc, SC[i])) { ok = false; d = "a row of id " + std::to_string(r.id) + " lacks sprite " + std::to_string(i); break; }
                        }
                } else if (r.id == 5) {
                    ++rows_after;
                    for (long rel = 40L * k; rel < 320L * k && ok; ++rel) {
                        auto cc = sample(t, r, rel);
                        if (!cc || !same(*cc, BG)) { ok = false; d = "a row of id 5 shows a colour"; }
                    }
                }
            }
            if (rows_in == 0 || rows_after == 0) ok = false;
            R.vext_eval = true;
            R.pre_vext = ok; R.d_vext = std::to_string(rows_in) + " rows inside the sprites' lines, " + std::to_string(rows_after) + " rows of the band after VSTOP" + (d.empty() ? "" : "; " + d);
        } else if (cf.group == 'A' && k > 0 && rf.ok) {
            bool ok = true; int rows_in = 0, rows_after = 0; std::string d;
            int last_o = -1;
            for (size_t i = 0; i < rows.size(); ++i) if (rows[i].id == 3) last_o = (int)i;
            for (size_t i = 0; i < rows.size(); ++i) {
                const Row& r = rows[i];
                if (r.id >= 1 && r.id <= 3) {
                    ++rows_in;
                    for (int j = 0; j < 8 && ok; ++j) {
                        bool even = (j % 2) == 0;
                        if ((r.id == 1 && !even) || (r.id == 3 && even)) continue;   // slots absent in that band
                        for (long rel = rf.slot[j]; rel < rf.slot[j] + 16L * k; ++rel) {
                            auto cc = sample(t, r, rel);
                            if (!cc || same(*cc, BG)) { ok = false; d = "a row of id " + std::to_string(r.id) + " shows no sprite at slot " + std::to_string(j); break; }
                        }
                    }
                } else if (r.id == 0 && (int)i > last_o && last_o >= 0) {
                    ++rows_after;
                    for (long rel = 40L * k; rel < 320L * k && ok; ++rel) {
                        auto cc = sample(t, r, rel);
                        if (!cc || !same(*cc, BG)) { ok = false; d = "a row after band O shows a colour"; }
                    }
                }
            }
            if (rows_in == 0 || rows_after == 0) ok = false;
            R.vext_eval = true;
            R.pre_vext = ok; R.d_vext = std::to_string(rows_in) + " rows inside the sprites' lines, " + std::to_string(rows_after) + " rows after band O" + (d.empty() ? "" : "; " + d);
        } else if (c == kB7 || cf.group == 'A') {
            R.vext_eval = false; R.pre_vext = false; R.d_vext = "no sprite references";
        }
        // ---- the comparison itself, every sample of every decoded row
        if (rf.ok && k > 0) {
            for (auto& r : rows) {
                if (r.id < 0) continue;
                for (long rel = -8L * k; rel < 328L * k; ++rel) {
                    auto got = sample(t, r, rel);
                    if (!got) { ++R.outside; continue; }
                    auto want = expected(cf, rf, r.id, rel, k);
                    if (!want) {
                        ++R.ungraded;
                        if (R.ungraded_seen.empty()) R.ungraded_seen = "row id " + std::to_string(r.id) + " x=" + std::to_string(rel / k) + " shows " + cs(*got);
                        continue;
                    }
                    ++R.compared;
                    if (!same(*got, *want)) {
                        ++R.wrong;
                        if (R.first_wrong.empty())
                            R.first_wrong = "row id " + std::to_string(r.id) + " x=" + std::to_string(rel / k) + " (sample " + std::to_string(rel) + ") shows " + cs(*got) + ", page says " + cs(*want);
                    }
                }
            }
        }
    }
    return o;
}

} // namespace pri

namespace pri {

// ---- P2: six planes and eight sprites in one row -------------------------------
struct P2Out {
    bool fixture_ok = false; std::string d_fixture;
    int k = 0;
    bool window_ok = false; std::string d_window;
    bool pre_frame = false; std::string d_frame;
    bool pre_tags = false; std::string d_tags;
    bool pre_fetch = false; std::string d_fetch;
    bool have_rows = false;
    long compared = 0, wrong = 0;
    std::string first_wrong;
    size_t rows_band = 0;
};

P2Out analyze_p2(const Trace& t) {
    P2Out o;
    {
        bool ok = t.tick.size() == 3 && t.mark.size() == 1;
        for (size_t i = 0; ok && i < 3; ++i) if (!t.tick[i] || (i && t.tick[i] <= t.tick[i-1])) ok = false;
        if (ok && !(t.tick[0] < t.mark[0] && t.mark[0] < t.tick[1])) ok = false;
        o.fixture_ok = ok; o.d_fixture = ok ? "3 ticks in order, the marker between t0 and t1" : "ticks or marker missing or out of order";
        if (!ok) return o;
    }
    const uint64_t t1 = t.tick[1], t2 = t.tick[2];
    int fi = frame_at(t, (t1 + t2) / 2);
    if (fi < 0) { o.d_frame = "no video frame holds the evaluated field"; return o; }
    std::vector<Row> rows = raw_rows(t, fi);
    int kmin = 0;
    for (const auto& r : rows) if (!kmin || r.w0 < kmin) kmin = r.w0;
    o.k = (kmin > 0 && kmin % 8 == 0) ? kmin / 8 : 0;
    const int k = o.k;
    {
        std::set<long> j0s; bool ok = k > 0; size_t n0 = 0; std::string d;
        for (auto& r : rows) {
            r.id = decode_id(t, r, k);
            if (r.id < 0) continue;
            j0s.insert(r.j0);
            if (r.id == 0) { ++n0; if (r.w0 != 8 * k) { ok = false; d = "a row with id 0 has block 0 of " + std::to_string(r.w0) + " samples"; } }
        }
        if (n0 == 0) { ok = false; d += " no row with id 0"; }
        if (j0s.size() != 1) { ok = false; d += " s0 takes " + std::to_string(j0s.size()) + " different values"; }
        o.window_ok = ok;
        o.d_window = "k=" + std::to_string(k) + " (kmin=" + std::to_string(kmin) + "), " + std::to_string(rows.size()) + " rows" + (d.empty() ? "" : " --" + d);
    }
    // frame association and plane fetches
    {
        uint32_t lo = kListBase, hi = lo + kListStep, end_word = lo + 2u * (4 + 4 * 6 + 32);
        size_t n = 0, outside = 0; uint64_t first_end = 0, first_plane = 0;
        for (size_t i = 0; i < t.list_reads.cyc.size(); ++i) {
            uint64_t cy = t.list_reads.cyc[i];
            if (cy < t1 || cy >= t2) continue;
            ++n;
            uint32_t a = t.list_reads.addr[i];
            if (a < lo || a >= hi) ++outside;
            else if (a == end_word && !first_end) first_end = cy;
        }
        // The band lies on lines 60-79; the fetches counted are those of lines 62-77 of the evaluated
        // field (VERTB marks line 0, p.219; one line is 227 colour clocks, p.23), two lines in from each end.
        const uint64_t band_lo = t1 + 62 * kLineCycles, band_hi = t1 + 78 * kLineCycles;
        size_t per_plane[6] = {0,0,0,0,0,0};
        for (size_t i = 0; i < t.plane_reads.cyc.size(); ++i) {
            uint64_t cy = t.plane_reads.cyc[i];
            if (cy < t1 || cy >= t2) continue;
            if (!first_plane) first_plane = cy;
            if (cy >= band_lo && cy < band_hi) per_plane[(t.plane_reads.addr[i] - kP2Img) / kP2Step]++;
        }
        o.pre_frame = n > 0 && outside == 0 && first_end && first_plane && first_end < first_plane;
        o.d_frame = std::to_string(n) + " list reads in the field, " + std::to_string(outside) + " outside the list, end WAIT first fetched at +" +
                    (first_end ? std::to_string(first_end - t1) : "never") + ", first bitplane fetch at +" + (first_plane ? std::to_string(first_plane - t1) : "never");
        bool all = true; std::string d = "reads per plane image in lines 62-77:";
        for (int p = 0; p < 6; ++p) { if (!per_plane[p]) all = false; d += " " + std::to_string(per_plane[p]); }
        o.pre_fetch = all; o.d_fetch = d;
    }
    // tags: rows of id 1 present, no other non-zero id, nothing undecodable
    size_t undecoded = 0; std::vector<int> seq;
    for (auto& r : rows) { if (r.id < 0) ++undecoded; else if (r.id) seq.push_back(r.id); }
    for (int s : seq) if (s != 1) undecoded += 1000;
    o.rows_band = seq.size();
    o.pre_tags = undecoded == 0 && !seq.empty();
    o.d_tags = std::to_string(seq.size()) + " rows of id 1, " + std::to_string(undecoded) + " undecodable or foreign rows";
    // ---- the comparison: sprite runs found by colour in the first row of the band
    if (k <= 0 || seq.empty()) return o;
    const Row* first = nullptr;
    for (auto& r : rows) if (r.id == 1) { first = &r; break; }
    if (!first) return o;
    o.have_rows = true;
    auto fr = runs_of(t, *first, k);
    long R[8]; bool found[8];
    for (int i = 0; i < 8; ++i) {
        found[i] = false; R[i] = -1000000000L;
        for (auto& ru : fr) if (same(ru.col, SC[i]) && ru.start < 272L * k) { R[i] = ru.start; found[i] = true; }
    }
    auto note_wrong = [&](const std::string& s) { ++o.wrong; if (o.first_wrong.empty()) o.first_wrong = s; };
    for (int i = 0; i < 8; ++i) {
        ++o.compared;
        if (!found[i]) note_wrong("sprite " + std::to_string(i) + " (" + cs(SC[i]) + ") has no run in the band");
        else {
            bool len_ok = false; for (auto& ru : fr) if (ru.start == R[i] && same(ru.col, SC[i]) && ru.len == 16L * k) len_ok = true;
            if (!len_ok) note_wrong("sprite " + std::to_string(i) + " is not one run of 16 lores pixels");
            if (i && found[i-1] && R[i] <= R[i-1]) note_wrong("sprite " + std::to_string(i) + " is not to the right of sprite " + std::to_string(i-1));
        }
    }
    for (auto& r : rows) {
        if (r.id < 0) continue;
        for (long rel = -8L * k; rel < 328L * k; ++rel) {
            auto got = sample(t, r, rel);
            if (!got) continue;
            Col want = BG;
            if (rel < 0 || rel >= 320L * k) want = BG;
            else {
                long x = rel / k;
                if (x < 40) { int blk = (int)(x / 8); want = blk == 0 ? PF1C : (((r.id >> (4 - blk)) & 1) ? PF1C : BG); }
                else if (r.id == 1) {
                    if (x >= 272) want = P2C[(x - 272) / 8];
                    else for (int i = 0; i < 8; ++i) if (found[i] && rel >= R[i] && rel < R[i] + 16L * k) want = SC[i];
                }
            }
            ++o.compared;
            if (!same(*got, want)) note_wrong("row id " + std::to_string(r.id) + " x=" + std::to_string(rel / k) + " (sample " + std::to_string(rel) + ") shows " + cs(*got) + ", page says " + cs(want));
        }
    }
    return o;
}

// ---- running one program --------------------------------------------------------
struct RunOut {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    P1Out p1;
    P2Out p2;
};

RunOut run_p1(const std::string& bin, const nmh::Config& cfg, uint64_t budget) {
    RunOut r;
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;
    r.done = h.run_until_write(kDoneAddr, kDoneValue, budget);
    if (!r.done) return r;
    Trace t = make_trace(h, 15, false);
    r.p1 = analyze_p1(t);
    return r;
}

RunOut run_p2(const std::string& bin, const nmh::Config& cfg, uint64_t budget) {
    RunOut r;
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;
    r.done = h.run_until_write(kDoneAddr, kDoneValue, budget);
    if (!r.done) return r;
    Trace t = make_trace(h, 1, true);
    r.p2 = analyze_p2(t);
    return r;
}

} // namespace pri

// ---- P1 and P2: the graded lines ------------------------------------------------
static void grade_p1(Checks& checks, const std::string& tag, const pri::RunOut& r) {
    using namespace pri;
    checks.expect(r.loaded && r.released && r.done.has_value() && r.p1.fixture_ok,
        "P1-FIXTURE-RUN " + tag + ": the program loaded, the CPU was released, the program reached DONE, and "
        "the field ticks and configuration markers are in order [not graded -- apparatus check] (" + r.p1.d_fixture + ")");
    if (!(r.loaded && r.released && r.done && r.p1.fixture_ok)) {
        checks.record("P1 " + tag + ": NOT GRADED -- the fixture did not deliver the schedule");
        return;
    }
    const P1Out& o = r.p1;
    checks.note("P1 " + tag + " reference frame: k=" + std::to_string(o.k) + " output samples per lores pixel; " + o.d_window);
    bool all_frame = true, all_tags = true, all_sprites = true, all_play = true;
    std::string bad_frame, bad_tags, bad_sprites, bad_play;
    for (int c = 0; c < 15; ++c) {
        const CfgRes& R = o.cfg[c];
        if (!R.pre_frame) { all_frame = false; bad_frame += std::string(" ") + kCfgs[c].name + "[" + R.d_frame + "]"; }
        if (!R.pre_tags) { all_tags = false; bad_tags += std::string(" ") + kCfgs[c].name + "[" + R.d_tags + "]"; }
        if (!R.pre_sprites) { all_sprites = false; bad_sprites += std::string(" ") + kCfgs[c].name + "[" + R.d_sprites + "]"; }
        if (!R.pre_play) { all_play = false; bad_play += std::string(" ") + kCfgs[c].name + "[" + R.d_play + "]"; }
        checks.note("P1 " + tag + " " + kCfgs[c].name + ": " + std::to_string(R.rows_total) + " rows, " + std::to_string(R.compared) +
                    " samples compared, " + std::to_string(R.wrong) + " different, " + std::to_string(R.ungraded) + " left open by the pages" +
                    (R.first_wrong.empty() ? "" : "; first difference: " + R.first_wrong) +
                    (R.ungraded_seen.empty() ? "" : "; open sample: " + R.ungraded_seen));
    }
    checks.expect(all_frame,
        "P1-PRE-FRAME " + tag + " [p.25]: in the evaluated field of every configuration every Copper fetch lies inside that "
        "configuration's own list, and the list's ending WAIT is fetched before the first bitplane fetch" + (all_frame ? "" : " --" + bad_frame));
    checks.expect(all_tags,
        "P1-PRE-TAGS " + tag + " [p.213]: every output row with a tag decodes, and the bands appear in the order and with the "
        "relative row counts of the band table in every configuration" + (all_tags ? "" : " --" + bad_tags));
    checks.expect(o.window_ok,
        "P1-PRE-WINDOW " + tag + " [p.59]: tag block 0 is exactly 8k samples wide and starts at one sample index in every "
        "row of every evaluated field (" + o.d_window + ")");
    checks.expect(all_sprites,
        "P1-PRE-SPRITES " + tag + " [p.126]: in every reference band the sprite runs are present, 16k samples wide, "
        "identical in every row and ordered left to right" + (all_sprites ? "" : " --" + bad_sprites));
    checks.expect(all_play,
        "P1-PRE-PLAYFIELDS " + tag + " [p.213]: in the bands with sprites absent, every column from x = 40 shows the playfield "
        "the band sets" + (all_play ? "" : " --" + bad_play));
    const CfgRes& B7 = o.cfg[kB7];
    const CfgRes& A1 = o.cfg[0];
    const CfgRes& A2 = o.cfg[1];
    const bool vext_eval = B7.vext_eval && A1.vext_eval && A2.vext_eval;
    const bool vext_ok = vext_eval && B7.pre_vext && A1.pre_vext && A2.pre_vext;
    if (!vext_eval)
        checks.record("P1-PRE-VEXTENT " + tag + ": NOT GRADED -- the sprite references of A1, A2 or B7 are missing (see P1-PRE-SPRITES)");
    else
        checks.expect(vext_ok,
            "P1-PRE-VEXTENT " + tag + " [p.107]: the sprites appear in every row of the bands inside their VSTART-VSTOP lines and in no row of "
            "the first band after VSTOP -- Group B (B7, groups 0 and 1; band B0): " + B7.d_vext + "; Group A (A1; the rows after band O): " + A1.d_vext +
            "; A2: " + A2.d_vext);

    auto gate = [&](char group, std::string& why) {
        why.clear();
        if (!r.p1.k_ok) why += " k";
        if (!o.window_ok) why += " window";
        for (int c = 0; c < 15; ++c) {
            if (kCfgs[c].group != group) continue;
            const CfgRes& R = o.cfg[c];
            if (!R.pre_frame) why += std::string(" frame:") + kCfgs[c].name;
            if (!R.pre_tags) why += std::string(" tags:") + kCfgs[c].name;
            if (!R.pre_sprites) why += std::string(" sprites:") + kCfgs[c].name;
            if (!R.pre_play) why += std::string(" playfields:") + kCfgs[c].name;
        }
        if (group == 'B' && !(B7.vext_eval && B7.pre_vext)) why += " vextent";
        if (group == 'A' && !(A1.vext_eval && A1.pre_vext && A2.vext_eval && A2.pre_vext)) why += " vextent";
        if (group == 'E' && !o.cfg[kB1].pre_sprites) why += " B1-sprites";
        return why.empty();
    };
    auto counts = [&](char group, long& cmp, long& wrong, std::string& detail) {
        cmp = 0; wrong = 0; detail.clear();
        for (int c = 0; c < 15; ++c) {
            if (kCfgs[c].group != group) continue;
            cmp += o.cfg[c].compared; wrong += o.cfg[c].wrong;
            if (o.cfg[c].wrong) detail += std::string(" ") + kCfgs[c].name + ":" + std::to_string(o.cfg[c].wrong);
        }
    };
    bool all_gated = true; long all_cmp = 0, all_wrong = 0;
    struct Grp { bool graded; long cmp, wrong; std::string why, detail; };
    auto grp = [&](char g) {
        Grp x;
        bool ok = gate(g, x.why);
        counts(g, x.cmp, x.wrong, x.detail);
        x.graded = ok && x.cmp > 0;
        if (!x.graded) { x.why = "NOT GRADED -- precondition(s) failed:" + x.why + (x.cmp == 0 ? " (no sample compared)" : ""); all_gated = false; }
        else { all_cmp += x.cmp; all_wrong += x.wrong; }
        return x;
    };
    auto tail = [&](const Grp& x) {
        return std::to_string(x.cmp) + " samples compared, " + std::to_string(x.wrong) + " different" + (x.detail.empty() ? "" : " (" + x.detail + " )");
    };
    const Grp ga = grp('A'), gb = grp('B'), ge = grp('E');
    if (!ga.graded) checks.record("P1-GROUP-A " + tag + ": " + ga.why);
    else checks.expect(ga.wrong == 0,
        "P1-GROUP-A " + tag + ": sprite against sprite: in every sample of the configurations A1 and A2 the lower-numbered of the "
        "overlapping sprites shows [p.207] -- " + tail(ga));
    if (!gb.graded) checks.record("P1-GROUP-B " + tag + ": " + gb.why);
    else checks.expect(gb.wrong == 0,
        "P1-GROUP-B " + tag + ": sprites against dual playfields: in every sample of B1-B8 the colour of the object in front shows, "
        "as Table 7-2 and PF2PRI order them [p.210, p.211] -- " + tail(gb));
    if (!ge.graded) checks.record("P1-GROUP-E " + tag + ": " + ge.why);
    else checks.expect(ge.wrong == 0,
        "P1-GROUP-E " + tag + ": sprites against a single playfield: in every sample of E0-E4 the colour of the object in front shows, "
        "as Table 7-2 orders them [p.210] -- " + tail(ge));
    if (!all_gated) {
        checks.record("P1-COLOUR-OF-HIGHEST-PRIORITY " + tag + ": NOT GRADED -- a group above was not graded");
    } else {
        checks.expect(all_wrong == 0,
            "P1-COLOUR-OF-HIGHEST-PRIORITY " + tag + " opus:7-cd.c3 sol:7-collision.c5 [p.213]: at every pixel of every row of the evaluated field "
            "of all fifteen configurations the colour of the highest-priority object is displayed, and the background colour where there is none -- " +
            std::to_string(all_cmp) + " samples compared, " + std::to_string(all_wrong) + " different");
    }
}

static void grade_p2(Checks& checks, const std::string& tag, const pri::RunOut& r) {
    checks.expect(r.loaded && r.released && r.done.has_value() && r.p2.fixture_ok,
        "P2-FIXTURE-RUN " + tag + ": the program loaded, the CPU was released, the program reached DONE, and the field ticks "
        "and the marker are in order [not graded -- apparatus check] (" + r.p2.d_fixture + ")");
    if (!(r.loaded && r.released && r.done && r.p2.fixture_ok)) {
        checks.record("P2 " + tag + ": NOT GRADED -- the fixture did not deliver the schedule");
        return;
    }
    const pri::P2Out& o = r.p2;
    checks.expect(o.pre_frame,
        "P2-PRE-FRAME " + tag + " [p.25]: in the evaluated field every Copper fetch lies inside the configuration's own list, and the "
        "list's ending WAIT is fetched before the first bitplane fetch (" + o.d_frame + ")");
    checks.expect(o.pre_tags,
        "P2-PRE-TAGS " + tag + " [p.213]: the rows of the band decode to tag id 1 and every other row to id 0 (" + o.d_tags + ")");
    checks.expect(o.window_ok,
        "P2-PRE-WINDOW " + tag + " [p.59]: tag block 0 is exactly 8k samples wide and starts at one sample index in every row (" + o.d_window + ")");
    checks.expect(o.pre_fetch,
        "P2-PRE-PLANE-FETCH " + tag + " [p.213]: the bitplane fetches of the band's lines (lines 62-77 of the evaluated field) read from the image of each of the six planes (" + o.d_fetch + ")");
    if (!(o.pre_frame && o.pre_tags && o.window_ok && o.pre_fetch && o.have_rows)) {
        checks.record("P2-COMBINE " + tag + ": NOT GRADED -- a precondition failed");
        return;
    }
    checks.note("P2 " + tag + ": " + std::to_string(o.rows_band) + " rows of the band, " + std::to_string(o.compared) + " samples compared, " +
                std::to_string(o.wrong) + " different" + (o.first_wrong.empty() ? "" : "; first difference: " + o.first_wrong));
    checks.expect(o.wrong == 0,
        "P2-COMBINE " + tag + " opus:7-cd.c2 [p.213]: every row of the band shows the contributions of all eight sprites (SC0-SC7, 16 lores pixels "
        "each, left to right) and of all six bitplanes (one block each, in the colour of that plane alone), and background elsewhere -- " +
        std::to_string(o.compared) + " samples compared, " + std::to_string(o.wrong) + " different");
}

static void run_pri_section(Checks& checks, const std::string& dir, const std::string& mv) {
    struct Combo { const char* label; bool ecs, ntsc; };
    std::vector<Combo> combos = {{"OCS-PAL", false, false}, {"ECS-PAL", true, false}, {"OCS-NTSC", false, true}, {"ECS-NTSC", true, true}};
    const bool mutated = !mv.empty();
    for (const auto& combo : combos) {
        if (mutated && !(combo.ecs && !combo.ntsc)) continue;
        const std::string std_name = combo.ntsc ? "ntsc" : "pal";
        const std::string tag = std::string(combo.label) + (mutated ? " [" + mv + "]" : "");
        if (!mutated || mv.rfind("P1-", 0) == 0) {
            std::string bin = dir + "/prog_pri_" + std_name + (mutated ? "_" + mv.substr(3) : "") + ".bin";
            auto lower = [](std::string s) { for (auto& ch : s) ch = (char)std::tolower((unsigned char)ch); return s; };
            if (mutated) bin = dir + "/prog_pri_pal_" + lower(mv.substr(3)) + ".bin";
            pri::RunOut r = pri::run_p1(bin, cfg_for(combo.ecs, combo.ntsc), 34'000'000);
            grade_p1(checks, tag, r);
        }
        if (!mutated || mv.rfind("P2-", 0) == 0) {
            auto lower = [](std::string s) { for (auto& ch : s) ch = (char)std::tolower((unsigned char)ch); return s; };
            std::string bin = dir + "/prog_p2_" + std_name + ".bin";
            if (mutated) bin = dir + "/prog_p2_pal_" + lower(mv.substr(3)) + ".bin";
            pri::RunOut r = pri::run_p2(bin, cfg_for(combo.ecs, combo.ntsc), 12'000'000);
            grade_p2(checks, tag, r);
        }
    }
}

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";
    const bool pri_mut = mv.rfind("P1-", 0) == 0 || mv.rfind("P2-", 0) == 0;

    // ================= Claims that give no proposition (SPECIFICATION.md) =================
    if (mv.empty()) {
        checks.record("opus:7-reset.c3 [p.223]: declined -- the page makes the outcome model-dependent (\"On some Amiga models, "
            "portions of the RAM still respond. On other models, no RAM responds. Software must assume that memory is not "
            "available.\"); both outcomes conform, so no expected value exists");
        checks.record("sol:7-reset.c6 [p.223]: declined -- the first half of the same sentence says that some models behave this way; "
            "one system cannot confirm or refute a statement about some models, so there is no expected value");
        checks.record("sol:7-reset.c7 [p.223]: declined -- the second half of the same sentence says that other models behave this way; "
            "as for sol:7-reset.c6, there is no expected value");
        checks.record("opus:7-appC-beamcon0.c7 [p.305]: declined -- BEAMCON0 bit 10 CSCBEN \"Composite sync redirection\": the page gives "
            "only this name, neither where composite sync is redirected to nor from where, and no pin, polarity or timing; "
            "composite sync is on a pin of its own (p.379, p.381) and a test sees only horizontal and vertical sync, so there "
            "is no stated effect and no observable output");
        checks.record("sol:7-appC-beamcon0.c7 [p.305]: declined -- the same row of the same table as opus:7-appC-beamcon0.c7, "
            "so the same reason applies");
    }

    // ================= P3 =================
    // Runs OCS and ECS, PAL and NTSC (the run matrix). NTSC is selected
    // through chipset_config (cfg_for); every mutation selector applies to
    // the ECS-PAL combination only, so a mutated run skips the NTSC ones.
    struct Combo { const char* label; bool ecs; bool ntsc; const char* bin; };
    Combo combos[] = {
        {"OCS-PAL",  false, false, "/prog_lp_ocs_pal.bin"},
        {"ECS-PAL",  true,  false, "/prog_lp_ecs_pal.bin"},
        {"OCS-NTSC", false, true,  "/prog_lp_ocs_ntsc.bin"},
        {"ECS-NTSC", true,  true,  "/prog_lp_ecs_ntsc.bin"},
    };

    for (const auto& combo : combos) {
        if (pri_mut || mv.rfind("P4-", 0) == 0 || (combo.ntsc && !mv.empty())) continue;
        std::string bin = dir + combo.bin;
        uint64_t f1_offset = 16;

        // Program-level mutations only apply to the ECS-PAL combo (the
        // representative base for P3's preconditions -- see the header
        // comment's own scope note).
        bool is_ecs_pal = combo.ecs && !combo.ntsc;
        std::string used_bin = bin;

        if (is_ecs_pal && mv == "P3-ANCHOR") used_bin = dir + "/prog_lp_ecs_pal_anchormut.bin";
        if (is_ecs_pal && mv == "P3-READTIME") used_bin = dir + "/prog_lp_ecs_pal_v1delay.bin";
        if (is_ecs_pal && mv == "P3-DDRA") used_bin = dir + "/prog_lp_ecs_pal_ddraforce.bin";
        if (is_ecs_pal && mv == "P3-LPEN") used_bin = dir + "/prog_lp_ecs_pal_lpenclear.bin";
        if (is_ecs_pal && mv == "P3-E40LINE44") used_bin = dir + "/prog_lp_ecs_pal_e40line44.bin";
        if (is_ecs_pal && mv == "P3-NOCOPEN") used_bin = dir + "/prog_lp_ecs_pal_nocopen.bin";
        if (is_ecs_pal && mv == "P3-NODONE") used_bin = dir + "/prog_lp_ecs_pal_nodone.bin";
        if (is_ecs_pal && mv == "P3-EVMIS") used_bin = dir + "/prog_lp_ecs_pal_evmis.bin";
        if (is_ecs_pal && mv == "P3-NOBIN") used_bin = dir + "/prog_lp_missing.bin";
        if (is_ecs_pal && mv == "P3-OFFSET") f1_offset = 30;

        DryResult dry = dry_run(used_bin, cfg_for(combo.ecs, combo.ntsc));
        std::string tag = std::string(combo.label) + ((is_ecs_pal && !mv.empty()) ? " [" + mv + "]" : "");
        // A failed fixture leaves the graded lines of this run NOT GRADED, never absent.
        auto fixture_failed = [&](const std::string& which) {
            for (const char* l : {"P3-HELD", "P3-PEN-POSITION", "P3-LATCHED-PEN-POSITION", "P3-FIELD-END-LATCH"})
                checks.record(std::string(l) + " " + tag + ": NOT GRADED -- the fixture check " + which + " failed");
        };

        checks.expect(dry.loaded && dry.released,
            "P3-FIXTURE-LOADED " + tag + ": program loaded and CPU released within the boot budget "
            "[not graded -- apparatus check]");
        if (!dry.loaded) { fixture_failed("P3-FIXTURE-LOADED"); continue; }

        TriggerPlan plan = plan_triggers(dry.log, is_ecs_pal ? f1_offset : 16);
        checks.expect(plan.ok, "P3-FIXTURE-TRIGGERS " + tag + ": the Copper's own A1/B1 fetch occurs once per "
            "field, at least 5 (A1) and 7 (B1) times, in the dry run [not graded -- apparatus check]");
        if (!plan.ok) { fixture_failed("P3-FIXTURE-TRIGGERS"); continue; }

        std::vector<uint64_t> triggers = {plan.f1, plan.f3, plan.f5};
        if (is_ecs_pal && mv == "P3-NOPULSE") triggers = {plan.f3, plan.f5};

        if (mv == "P3-BEAMCON0") {
            // reuse the ECS-NTSC build (BEAMCON0<-$0000) against this
            // combo's own PAL-configured harness -- bit 5 opposite the
            // selected standard.
            if (!combo.ecs) continue; // OCS has no BEAMCON0; run only once (under ECS-PAL)
            used_bin = dir + "/prog_lp_ecs_ntsc.bin";
            dry = dry_run(used_bin, cfg_for(combo.ecs, combo.ntsc));
            plan = plan_triggers(dry.log, 16);
            triggers = {plan.f1, plan.f3, plan.f5};
            tag = std::string(combo.label) + " [P3-BEAMCON0]";
        }

        DrivenResult run = driven_run(used_bin, cfg_for(combo.ecs, combo.ntsc), triggers, 4 * kCck);
        checks.expect(run.loaded && run.done.has_value(),
            "P3-FIXTURE-DRIVEN " + tag + ": the driven run reached DONE [not graded -- apparatus check]");
        if (!run.loaded || !run.done) { fixture_failed("P3-FIXTURE-DRIVEN"); continue; }

        Records recs = parse_records(run.mem, run.log);
        auto get = [&](int field, int event) -> const Rec& {
            static Rec empty;
            int idx = record_index(field, event);
            if (idx < 0) return empty;
            return recs.recs[idx];
        };
        auto storec = [&](int field, int event) -> uint64_t {
            int idx = record_index(field, event);
            return idx < 0 ? 0 : recs.store_cycle[idx];
        };

        // The preconditions of the graded lines, in print order.
        std::vector<std::pair<std::string, bool>> pre;

        // ---- Preconditions ----

        // 0. Record layout: every record carries the field and event words
        // that record_index() assumes for its position, the first
        // kPhantomRecords records being the first Copper pass (field word
        // $FFFF, events E40..E210 in order).
        {
            int bad = 0, missing = 0;
            std::string first_bad;
            for (int i = 0; i < kTotalRecords; ++i) {
                const Rec& r = recs.recs[(size_t)i];
                if (!r.present) { ++missing; continue; }
                int exp_field, exp_event;
                if (i < kPhantomRecords) { exp_field = 0xFFFF; exp_event = i; }
                else {
                    int j = i - kPhantomRecords;
                    exp_field = j / 8;
                    int p = j % 8;
                    exp_event = p == 0 ? 6 : p == 1 ? 7 : p - 2;
                }
                if (r.field != exp_field || r.event != exp_event) {
                    ++bad;
                    if (first_bad.empty())
                        first_bad = "record " + std::to_string(i) + " carries field " + std::to_string(r.field) +
                                    " event " + std::to_string(r.event) + ", its position implies field " +
                                    std::to_string(exp_field) + " event " + std::to_string(exp_event);
                }
            }
            bool ok = missing == 0 && bad == 0;
            checks.note("P3 " + tag + " records: " + std::to_string(kTotalRecords - missing) + " of " +
                        std::to_string(kTotalRecords) + " present, " + std::to_string(bad) + " with a field or event word " +
                        "other than the one their position implies" + (first_bad.empty() ? "" : "; first: " + first_bad));
            checks.expect(ok,
                "P3-PRE-RECORDS " + tag + " [not graded -- apparatus check]: all 72 records are present and each carries the field "
                "and event words that its position in the record area implies");
            pre.push_back({"P3-PRE-RECORDS", ok});
        }

        // 1. Anchor check.
        {
            auto a1 = read_cycles_at(dry.log, kCopA1), a2 = read_cycles_at(dry.log, kCopA2);
            auto b1 = read_cycles_at(dry.log, kCopB1), b2 = read_cycles_at(dry.log, kCopB2);
            // Skip the leading first pass (kPhantomPasses). DONE fires
            // right after F7's own V2 (before F7 ever reaches its own A1),
            // so only 8 real fields' worth of A1/B1 pairs (F-1..F6) are
            // ever logged -- 9 entries total with the first pass.
            constexpr size_t kRealFields = 8;
            bool sizes_ok = a1.size() >= kRealFields + kPhantomPasses && a2.size() >= kRealFields + kPhantomPasses &&
                             b1.size() >= kRealFields + kPhantomPasses && b2.size() >= kRealFields + kPhantomPasses;
            bool spacing_ok = sizes_ok, period_ok = sizes_ok;
            int64_t worst_spacing = 0, worst_period = 0;
            if (sizes_ok) {
                for (size_t i = 0; i < kRealFields; ++i) {
                    size_t j = i + kPhantomPasses;
                    int64_t da = (int64_t)a2[j] - (int64_t)a1[j] - 768;
                    int64_t db = (int64_t)b2[j] - (int64_t)b1[j] - 768;
                    if (std::abs(da) > 16) spacing_ok = false;
                    if (std::abs(db) > 16) spacing_ok = false;
                    worst_spacing = std::max({worst_spacing, std::abs(da), std::abs(db)});
                }
                int64_t p0 = (int64_t)a1[kPhantomPasses + 1] - (int64_t)a1[kPhantomPasses];
                for (size_t i = 1; i < kRealFields - 1; ++i) {
                    size_t j = i + kPhantomPasses;
                    int64_t p = (int64_t)a1[j+1] - (int64_t)a1[j];
                    if (std::abs(p - p0) > 8) period_ok = false;
                    worst_period = std::max(worst_period, std::abs(p - p0));
                }
            }
            checks.note("P3 " + tag + " anchor: worst |spacing-768| = " + std::to_string(worst_spacing) +
                        " cycles, worst period deviation = " + std::to_string(worst_period) + " cycles");
            bool ok = sizes_ok && spacing_ok && period_ok;
            checks.expect(ok,
                "P3-PRE-ANCHOR " + tag + " [p.22]: t(A2)-t(A1) and t(B2)-t(B1) each equal 96 colour "
                "clocks within 2 in every field, and the field period (from t(A1)) is the same for "
                "all fields");
            pre.push_back({"P3-PRE-ANCHOR", ok});
        }

        // 2. Read-time check.
        {
            // CALIB: poll iterations over the 20 lines E110..E130 of the first
            // Copper pass (prog_lp.s), the unit of the V1/V2 waits.
            {
                uint32_t hi = 0, lo = 0;
                bool have_hi = false, have_lo = false;
                for (const auto& a : run.log) {
                    if (!a.is_write) continue;
                    if (a.word_addr == word_addr(kCalib))     { hi = a.data; have_hi = true; }
                    if (a.word_addr == word_addr(kCalib + 2)) { lo = a.data; have_lo = true; }
                }
                auto e110 = read_cycles_at(run.log, kCopE110);
                auto e130 = read_cycles_at(run.log, kCopE130);
                std::string span = "?";
                if (!e110.empty() && !e130.empty()) span = std::to_string(e130.front() - e110.front());
                checks.note("P3 " + tag + " delay calibration: " +
                            (have_hi && have_lo ? std::to_string((hi << 16) | lo) : std::string("?")) +
                            " poll iterations measured over the 20 lines E110..E130 of the first Copper pass, "
                            "which the Copper fetches " + span + " cycles apart");
            }
            struct Sig { int field, event; uint32_t signal_addr; };
            std::vector<Sig> sigs = {
                {0,0,kCopE40},{0,1,kCopE60},{0,2,kCopE110},{0,3,kCopE130},{0,4,kCopE190},{0,5,kCopE210},
            };
            bool ok = true;
            int checked = 0;
            for (int f = 0; f <= 7; ++f) {
                for (auto s : sigs) {
                    auto fetches = read_cycles_at(dry.log, s.signal_addr);
                    size_t j = (size_t)f + kPhantomPasses; // skip the leading first pass
                    if (j >= fetches.size()) { ok = false; continue; }
                    uint64_t store = storec(f, s.event);
                    if (store == 0 || store <= fetches[j] || store - fetches[j] > kLineCycles) ok = false;
                    ++checked;
                }
            }
            auto mticks = field_tick_cycles(run.log);
            for (int f = 0; f <= 8; ++f) {
                uint64_t v1 = storec(f, 6), v2 = storec(f, 7);
                if (mticks[f] == 0 || v1 == 0 || v2 == 0) { ok = false; continue; }
                if (v1 <= mticks[f] || v1 - mticks[f] >= 18 * kLineCycles) ok = false;
                if (v2 <= mticks[f] || v2 - mticks[f] >= 18 * kLineCycles) ok = false;
                checked += 2;
            }
            checks.note("P3 " + tag + " read-time: " + std::to_string(checked) + " reads checked");
            checks.expect(ok,
                "P3-PRE-READTIME " + tag + " [p.22]: every Copper-signalled read is stored within "
                "one line of its signalling MOVE's own fetch, and every V1/V2 is stored less than "
                "18 lines after its field's own VERTB tick");
            pre.push_back({"P3-PRE-READTIME", ok});
        }

        // 3. Pin 6 not driven by the CIA (the CIAADDRA value read after the fix-or-force step).
        {
            bool ok = run.ddra_stored.has_value() && (*run.ddra_stored & 0xC0) == 0;
            checks.note("P3 " + tag + " DDRA: value read after the fix-or-force step = " +
                        (run.ddra_stored ? std::to_string(*run.ddra_stored) : std::string("(missing)")));
            checks.expect(ok,
                "P3-PRE-DDRA " + tag + " [p.228,p.229,p.351]: the CIAADDRA value read after the fix-or-force "
                "step has bits 6 and 7 clear (pin 6 of both ports is an input)");
            pre.push_back({"P3-PRE-DDRA", ok});
        }

        // 4. The pulse was driven. Graded against the NOMINAL schedule (3
        // pulses: t(A1)+16 in F1, t(A1)+56 in F3, t(B1)+16 in F5, in
        // colour clocks: plan_triggers(dry.log, 16)), whatever the selector
        // fed the driver -- a reduced or shifted `triggers` must still show
        // up here as different from the nominal schedule.
        {
            size_t nominal = 3;
            TriggerPlan nplan = plan_triggers(dry.log, 16);
            const std::vector<uint64_t> nominal_triggers = {nplan.f1, nplan.f3, nplan.f5};
            bool ok = nplan.ok && run.pulses.size() == nominal;
            for (size_t i = 0; ok && i < nominal_triggers.size() && i < run.pulses.size(); ++i) {
                uint64_t a = run.pulses[i].first;
                if (a < nominal_triggers[i] || a > nominal_triggers[i] + 4) ok = false;
            }
            checks.note("P3 " + tag + " drive log: " + std::to_string(run.pulses.size()) +
                        " pulse(s) driven, " + std::to_string(nominal) + " scheduled (nominal)");
            checks.expect(ok,
                "P3-PRE-PULSE " + tag + ": the test's own drive log shows each scheduled pulse at "
                "its scheduled time and nothing else on pin 6");
            pre.push_back({"P3-PRE-PULSE", ok});
        }

        {
            // 5 & item4: beam reads live before/after every trigger, every field.
            // The same comparator runs under every selector.
            bool ok = true;
            int checked = 0;
            for (int f = 0; f <= 7; ++f) {
                const Rec& e40 = get(f, 0);
                const Rec& e60 = get(f, 1);
                if (!e40.present || !e60.present) { ok = false; continue; }
                uint32_t v40 = V_of(e40), v60 = V_of(e60);
                if (!(v40 == 40 || v40 == 41)) ok = false;
                if (!(v60 == 60 || v60 == 61)) ok = false;
                checked += 2;
            }
            // fields without a trigger: F-1,F0,F2,F4,F6 = indices 0,1,3,5,7
            for (int f : {0,1,3,5,7}) {
                struct { int event; uint32_t line; } evs[] = {{2,110},{3,130},{4,190},{5,210}};
                for (auto e : evs) {
                    const Rec& r = get(f, e.event);
                    if (!r.present) { ok = false; continue; }
                    uint32_t v = V_of(r);
                    if (!(v == e.line || v == e.line + 1)) ok = false;
                    ++checked;
                }
            }
            checks.note("P3 " + tag + " item4/pre5: " + std::to_string(checked) + " live-beam reads checked");
            checks.expect(ok,
                "P3-PRE-LIVE " + tag + " [p.216]: before a trigger and "
                "after its release, VPOSR/VHPOSR read the live beam (E40/E60 of every field; "
                "E110-E210 of every field without a trigger)");
            pre.push_back({"P3-PRE-LIVE", ok});
        }

        // 6. ECS-only: field period unaffected by the BEAMCON0 write.
        if (combo.ecs) {
            auto pre0 = write_cycles_at(dry.log, kPreTick0);
            auto pre1 = write_cycles_at(dry.log, kPreTick1);
            auto mticks = field_tick_cycles(run.log);
            bool have = !pre0.empty() && !pre1.empty() && mticks[1] != 0 && mticks[0] != 0;
            int64_t pre_period = have ? (int64_t)pre1.front() - (int64_t)pre0.front() : 0;
            int64_t post_period = have ? (int64_t)mticks[1] - (int64_t)mticks[0] : 0;
            // The pre-write tick and the post-write tick are each stored by
            // a different code path (wait_vertb's own bsr/rts vs
            // main_loop's inline handling), so a small, fixed software
            // latency difference between the two is expected. The bound is
            // one line (p.23: 227 or 228 colour clocks, taken as 228): p.24
            // gives the PAL field 312 lines and the NTSC field 262, fifty
            // lines apart, so a change of standard is fifty times the bound.
            bool ok = have && std::abs(pre_period - post_period) <= (int64_t)kLongLineCycles;
            checks.note("P3 " + tag + " BEAMCON0 period: pre=" + std::to_string(pre_period) +
                        " post=" + std::to_string(post_period) + " cycles, bound one line = " +
                        std::to_string(kLongLineCycles) + " cycles");
            checks.expect(ok,
                "P3-PRE-PERIOD " + tag + " [p.23,p.24]: the field period measured before the BEAMCON0 "
                "write equals the period measured after it, within one line");
            pre.push_back({"P3-PRE-PERIOD", ok});
        } else {
            checks.record("P3-PRE-PERIOD " + tag + ": not applicable -- BEAMCON0 does not exist "
                "under OCS (p.305)");
        }

        {
            std::string why;
            for (const auto& p : pre) if (!p.second) why += " " + p.first;
            const std::string not_graded = ": NOT GRADED -- precondition(s) failed:" + why;
            if (!why.empty()) {
                checks.record("P3-HELD " + tag + not_graded);
                checks.record("P3-PEN-POSITION " + tag + not_graded);
                checks.record("P3-LATCHED-PEN-POSITION " + tag + not_graded);
                checks.record("P3-FIELD-END-LATCH " + tag + not_graded);
            } else {
                // ---- Items 1, 2, 5 (the latching claim). ----
                auto m_group_ok = [&](int f_events, std::vector<int> events, int f_tail) {
                    std::vector<uint32_t> ms;
                    bool word_ok = true, present = true;
                    for (int e : events) {
                        const Rec& r = get(f_events, e);
                        if (!r.present) { present = false; continue; }
                        ms.push_back(M_of(r));
                        if (r.vhposr_word != r.vhposr_long) word_ok = false;
                    }
                    for (int e : {6,7}) {
                        const Rec& r = get(f_tail, e);
                        if (!r.present) { present = false; continue; }
                        ms.push_back(M_of(r));
                        if (r.vhposr_word != r.vhposr_long) word_ok = false;
                    }
                    bool same = present && !ms.empty();
                    for (auto m : ms) if (m != ms[0]) same = false;
                    return present && same && word_ok;
                };
                bool g1 = m_group_ok(2, {2,3,4,5}, 3); // F1 -> F2
                bool g3 = m_group_ok(4, {2,3,4,5}, 5); // F3 -> F4
                bool g5 = m_group_ok(6, {4,5}, 7);     // F5 -> F6 (E190,E210 only)
                const bool item1 = g1 && g3 && g5;
                checks.expect(item1,
                    "P3-HELD " + tag + " [p.216,p.239]: once "
                    "triggered, VPOSR/VHPOSR stay at one constant value (the word read equalling the "
                    "long read's low word) through E110-E210 of the field and V1/V2 of the field after "
                    "it -- F1 (E110-E210,V1/V2 of F2): " + std::string(g1?"held":"changed") +
                    "; F3: " + std::string(g3?"held":"changed") + "; F5 (E190/E210,V1/V2 of F6): " +
                    std::string(g5?"held":"changed"));

                const Rec& l1 = get(2, 2), &l3 = get(4, 2), &l5 = get(6, 4);
                const bool item2 = l1.present && l3.present && l5.present &&
                             V_of(l1) == 100 && V_of(l3) == 100 && V_of(l5) == 180 &&
                             (int64_t)H_of(l3) - (int64_t)H_of(l1) == 40;
                checks.note("P3 " + tag + " item2: V(L1)=" + (l1.present?std::to_string(V_of(l1)):"?") +
                            " V(L3)=" + (l3.present?std::to_string(V_of(l3)):"?") +
                            " V(L5)=" + (l5.present?std::to_string(V_of(l5)):"?") +
                            " H(L3)-H(L1)=" + (l1.present&&l3.present?std::to_string((int64_t)H_of(l3)-(int64_t)H_of(l1)):"?"));
                checks.expect(item2,
                    "P3-PEN-POSITION " + tag + " [p.238,p.239]: the latched value "
                    "is the light-pen position -- V(L1)=100, V(L3)=100, V(L5)=180, H(L3)-H(L1)=40");

                checks.expect(item1 && item2,
                    "P3-LATCHED-PEN-POSITION " + tag + " opus:7-bp.c3 sol:7-beam.c3 opus:7-bp.c11 [p.216,p.238,p.239]: once "
                    "the pen has triggered, VPOSR/VHPOSR return the position of the pen, not the beam position, "
                    "until the end of internal vertical blanking of the next field (P3-HELD and P3-PEN-POSITION both hold)");

                bool ok5 = true;
                std::string v12;
                uint32_t threshold = 0x10500;
                for (int f : {2,4,6,8}) {
                    const Rec& v1 = get(f,6), &v2 = get(f,7);
                    if (!v1.present || !v2.present) { ok5 = false; continue; }
                    char b[64];
                    snprintf(b, sizeof b, " F%d V1=$%05X V2=$%05X", f - 1, M_of(v1), M_of(v2));
                    v12 += b;
                    if (M_of(v1) != M_of(v2)) ok5 = false;
                    bool above = combo.ntsc ? (M_of(v1) >= threshold) : (M_of(v1) > threshold);
                    if (!above) ok5 = false;
                }
                checks.note("P3 " + tag + " item5: M of V1 and V2 after a field without a trigger:" + v12);
                checks.expect(ok5,
                    "P3-FIELD-END-LATCH " + tag + " [p.239]: with no trigger in "
                    "the preceding field, V1 and V2 agree and their M is " +
                    std::string(combo.ntsc ? "not less than" : "greater than") + " $10500 (F1,F3,F5,F7)");
            }
        }
    }

    // ================= P4 (ECS, PAL) =================
    if (!pri_mut && mv.rfind("P3-", 0) != 0) {
        constexpr uint32_t kP4CopE250 = kCopList + 84;
        constexpr uint32_t kP4MainTick = 0x0C1100; // 5 words: fields F-1,F0,F1,F2,F3
        nmh::Config cfg = cfg_for(/*ecs=*/true, /*ntsc=*/false);

        std::string base_bin = "/prog_p4.bin";
        std::string used_bin = dir + base_bin;
        if (mv == "P4-NOLPENDIS") used_bin = dir + "/prog_p4_nolpendis.bin";
        if (mv == "P4-F3LPENDIS") used_bin = dir + "/prog_p4_f3lpendis.bin";
        if (mv == "P4-F1NOLPEN") used_bin = dir + "/prog_p4_f1nolpen.bin";
        if (mv == "P4-DDRA") used_bin = dir + "/prog_p4_ddraforce.bin";
        if (mv == "P4-ANCHOR") used_bin = dir + "/prog_p4_anchormut.bin";
        if (mv == "P4-PERIOD") used_bin = dir + "/prog_p4_periodmut.bin";
        if (mv == "P4-SHORT") used_bin = dir + "/prog_p4_shortspin.bin";
        if (mv == "P4-YDELAY") used_bin = dir + "/prog_p4_ydelay.bin";
        if (mv == "P4-NOCOPEN") used_bin = dir + "/prog_p4_nocopen.bin";
        if (mv == "P4-NODONE") used_bin = dir + "/prog_p4_nodone.bin";
        if (mv == "P4-NOBIN") used_bin = dir + "/prog_p4_missing.bin";
        std::string tag = std::string("ECS-PAL") + (mv.empty() ? "" : " [" + mv + "]");

        DryResult dry = dry_run(used_bin, cfg);
        checks.expect(dry.loaded && dry.released,
            "P4-FIXTURE-LOADED " + tag + ": program loaded and CPU released within the boot budget "
            "[not graded -- apparatus check]");

        auto a1 = read_cycles_at(dry.log, kCopA1);
        bool have_a1 = a1.size() >= kPhantomPasses + 5; // first pass + F-1,F0,F1,F2,F3
        checks.expect(have_a1,
            "P4-FIXTURE-A1 " + tag + ": the Copper's own A1 fetch occurs at least 5 times (plus the "
            "leading first pass) in the dry run [not graded -- apparatus check]");
        if (!dry.loaded || !have_a1) {
            checks.record("P4-LPENDIS-READ " + tag + ": NOT GRADED -- the fixture check " + std::string(!dry.loaded ? "P4-FIXTURE-LOADED" : "P4-FIXTURE-A1") + " failed");
        }
        else {
            uint64_t f1_trig = a1[kPhantomPasses + 2] + 16 * kCck;
            uint64_t f3_trig = a1[kPhantomPasses + 4] + 16 * kCck;
            std::vector<uint64_t> triggers = {f1_trig, f3_trig};
            // A third pulse in F2, 16 colour clocks after F2's own A1 fetch, which lies
            // after Y's store (P4-PRE-REFFRAME): the drive log then shows one pulse more
            // than the nominal schedule and nothing before it moves.
            if (mv == "P4-EXTRAPULSE-F2") triggers = {f1_trig, a1[kPhantomPasses + 3] + 16 * kCck, f3_trig};

            DrivenResult run = driven_run(used_bin, cfg, triggers, 4 * kCck, std::nullopt, 12);
            checks.expect(run.loaded && run.done.has_value(),
                "P4-FIXTURE-DRIVEN " + tag + ": the driven run reached DONE [not graded -- apparatus check]");
            if (run.loaded && run.done) {
                auto make_rec = [&](size_t base) -> Rec {
                    Rec r;
                    if (base + 2 >= run.mem.size()) return r;
                    if (!run.mem[base] || !run.mem[base+1] || !run.mem[base+2]) return r;
                    r.present = true;
                    r.vposr_long = *run.mem[base];
                    r.vhposr_long = *run.mem[base+1];
                    r.vhposr_word = *run.mem[base+2];
                    return r;
                };
                Rec X = make_rec(0), Y = make_rec(3), Xp = make_rec(6), Yp = make_rec(9);

                // The preconditions of the graded line, in print order.
                std::vector<std::pair<std::string, bool>> pre;

                // Precondition: DDRA (as in P3).
                bool ddra_ok = run.ddra_stored.has_value() && (*run.ddra_stored & 0xC0) == 0;
                checks.expect(ddra_ok,
                    "P4-PRE-DDRA " + tag + " [p.228,p.229,p.351]: the CIAADDRA value read after the "
                    "fix-or-force step has bits 6 and 7 clear");
                pre.push_back({"P4-PRE-DDRA", ddra_ok});

                // Precondition: anchor check (A2-A1 spacing), as in P3.
                auto a2 = read_cycles_at(dry.log, kCopA2);
                bool anchor_ok = a1.size() >= kPhantomPasses + 5 && a2.size() >= kPhantomPasses + 5;
                if (anchor_ok) {
                    for (size_t i = 0; i < 5; ++i) {
                        size_t j = i + kPhantomPasses;
                        if (std::abs((int64_t)a2[j] - (int64_t)a1[j] - 768) > 16) anchor_ok = false;
                    }
                }
                checks.expect(anchor_ok,
                    "P4-PRE-ANCHOR " + tag + " [p.22]: t(A2)-t(A1) equals 96 colour clocks within 2 "
                    "in every field (F-1..F3)");
                pre.push_back({"P4-PRE-ANCHOR", anchor_ok});

                auto find_store = [&](uint32_t byte_addr) -> uint64_t {
                    auto w = write_cycles_at(run.log, byte_addr);
                    return w.empty() ? 0 : w.front();
                };
                uint64_t x_store = find_store(kRecords);       // first word of X
                uint64_t y_store = find_store(kRecords + 6);   // first word of Y
                uint64_t xp_store = find_store(kRecords + 12);
                uint64_t yp_store = find_store(kRecords + 18);

                // Precondition: read time. Y is read at once after the BEAMCON0
                // write that follows X, and Y' after X': each store lies within
                // one line (p.23) of the one before it.
                {
                    bool ok = x_store && y_store && xp_store && yp_store &&
                              y_store > x_store && y_store - x_store <= kLongLineCycles &&
                              yp_store > xp_store && yp_store - xp_store <= kLongLineCycles;
                    checks.note("P4 " + tag + " read time: Y stored " + std::to_string(y_store - x_store) +
                                " cycles after X, Y' " + std::to_string(yp_store - xp_store) + " cycles after X', bound one line = " +
                                std::to_string(kLongLineCycles) + " cycles");
                    checks.expect(ok,
                        "P4-PRE-READTIME " + tag + " [p.23]: Y is stored within one line after X and Y' within one line "
                        "after X' (each second read follows its BEAMCON0 write at once)");
                    pre.push_back({"P4-PRE-READTIME", ok});
                }

                // Precondition: the drive log shows the scheduled pulses and nothing else on pin 6.
                {
                    size_t nominal = 2;
                    bool ok = run.pulses.size() == nominal;
                    std::vector<uint64_t> nominal_triggers = {f1_trig, f3_trig};
                    for (size_t i = 0; ok && i < nominal_triggers.size() && i < run.pulses.size(); ++i) {
                        uint64_t a = run.pulses[i].first;
                        if (a < nominal_triggers[i] || a > nominal_triggers[i] + 4) ok = false;
                    }
                    checks.note("P4 " + tag + " drive log: " + std::to_string(run.pulses.size()) +
                                " pulse(s) driven, " + std::to_string(nominal) + " scheduled (nominal)");
                    checks.expect(ok,
                        "P4-PRE-PULSE " + tag + ": the test's own drive log shows each scheduled pulse at "
                        "its scheduled time and nothing else on pin 6");
                    pre.push_back({"P4-PRE-PULSE", ok});
                }

                // Precondition: field period unchanged across every BEAMCON0 write.
                auto mticks_p4 = [&]() {
                    std::vector<uint64_t> t;
                    for (int i = 0; i < 5; ++i) {
                        auto w = write_cycles_at(run.log, kP4MainTick + 2 * (uint32_t)i);
                        t.push_back(w.empty() ? 0 : w.front());
                    }
                    return t;
                }();
                // main_loop's own VERTB poll is a tight but non-zero-length
                // instruction loop, so the tick store lands a few
                // instructions after the true hardware edge, by an amount
                // that varies slightly field to field -- polling quantization
                // jitter, not a period change. The bound is one line (p.23:
                // 227 or 228 colour clocks, taken as 228); p.24 gives the PAL
                // field 312 lines and the NTSC field 262, fifty lines apart.
                bool period_ok = true;
                int64_t p0 = (int64_t)mticks_p4[1] - (int64_t)mticks_p4[0];
                std::string periods_dump = std::to_string(p0);
                for (int i = 1; i < 4; ++i) {
                    int64_t p = (int64_t)mticks_p4[i+1] - (int64_t)mticks_p4[i];
                    periods_dump += "," + std::to_string(p);
                    if (std::abs(p - p0) > (int64_t)kLongLineCycles) period_ok = false;
                }
                checks.note("P4 " + tag + " ticks: " + std::to_string(mticks_p4[0]) + " " +
                            std::to_string(mticks_p4[1]) + " " + std::to_string(mticks_p4[2]) + " " +
                            std::to_string(mticks_p4[3]) + " " + std::to_string(mticks_p4[4]) +
                            "; periods: " + periods_dump + "; bound one line = " + std::to_string(kLongLineCycles) + " cycles");
                checks.expect(period_ok,
                    "P4-PRE-PERIOD " + tag + " [p.23,p.24,p.305]: the field period is unchanged across "
                    "every BEAMCON0 write (measured from the VERTB ticks), within one line");
                pre.push_back({"P4-PRE-PERIOD", period_ok});

                // Precondition: reference-frame bound on X/Y (also the guard of the
                // delay that prog_p4.s calibrates in the run). It depends on the
                // period precondition: a shorter field ends before the delay does.
                uint64_t f1_a1 = a1[kPhantomPasses + 2], f3_a1 = a1[kPhantomPasses + 4];
                uint64_t f2_a1 = a1[kPhantomPasses + 3]; // next field after F1
                {
                    // CALIB (prog_p4.s): spin iterations over the 20 lines between
                    // the E110 and E130 COPER signals, measured in F0 and used as
                    // the delay after E250. Read back from the program's own stores.
                    uint32_t hi = 0, lo = 0;
                    bool have_hi = false, have_lo = false;
                    for (const auto& a : run.log) {
                        if (!a.is_write) continue;
                        if (a.word_addr == word_addr(kCalib))     { hi = a.data; have_hi = true; }
                        if (a.word_addr == word_addr(kCalib + 2)) { lo = a.data; have_lo = true; }
                    }
                    auto e110 = read_cycles_at(run.log, kCopList + 36);
                    auto e130 = read_cycles_at(run.log, kCopList + 44);
                    auto e250 = read_cycles_at(run.log, kP4CopE250);
                    std::string span = "?";
                    if (e110.size() > kPhantomPasses + 1 && e130.size() > kPhantomPasses + 1)
                        span = std::to_string(e130[kPhantomPasses + 1] - e110[kPhantomPasses + 1]);
                    std::string delay = "?";
                    if (e250.size() > kPhantomPasses + 2 && x_store > e250[kPhantomPasses + 2])
                        delay = std::to_string(x_store - e250[kPhantomPasses + 2]);
                    checks.note("P4 " + tag + " delay calibration: " +
                                (have_hi && have_lo ? std::to_string((hi << 16) | lo) : std::string("?")) +
                                " poll iterations measured over the 20 lines E110..E130 (F0), which the "
                                "Copper fetches " + span + " cycles apart; the delay after E250 in F1 "
                                "lasted " + delay + " cycles to X's store");
                }
                checks.note("P4 " + tag + " reference frame: x_store-f1_a1=" +
                            std::to_string(x_store - f1_a1) + " cycles, needs > " +
                            std::to_string(156u*228u*kCck));
                if (!period_ok) {
                    checks.record("P4-PRE-REFFRAME " + tag + ": NOT GRADED -- precondition(s) failed: P4-PRE-PERIOD");
                } else {
                    bool ref_ok = x_store > f1_a1 + 156u * 228u * kCck &&
                                  y_store > f1_a1 + 156u * 228u * kCck &&
                                  (f2_a1 == 0 || y_store < f2_a1 - 100u * 228u * kCck) &&
                                  xp_store > f3_a1 + 156u * 228u * kCck;
                    checks.expect(ref_ok,
                        "P4-PRE-REFFRAME " + tag + " [p.239, build note]: X (and X') is stored at least "
                        "156*228 colour clocks after its own field's t(A1), placing it at line 256 or "
                        "later, and Y is stored before the next field's own t(A1) minus 100*228 colour "
                        "clocks");
                    pre.push_back({"P4-PRE-REFFRAME", ref_ok});
                }

                // Precondition: latch present in F1 -- V(Y)=100, and the H of the latch is the
                // same in F1 (Y) and F3 (Y'), whose triggers start at the same phase; X'=Y' is P4-PRE-CONTROL.
                bool latch_f1 = Y.present && Xp.present && Yp.present && V_of(Y) == 100 &&
                                H_of(Y) == H_of(Yp);
                checks.note("P4 " + tag + " Y: V=" + (Y.present?std::to_string(V_of(Y)):"?") +
                            " H=" + (Y.present?std::to_string(H_of(Y)):"?") +
                            "; H(Y')=" + (Yp.present?std::to_string(H_of(Yp)):"?"));
                checks.expect(latch_f1,
                    "P4-PRE-LATCHF1 " + tag + " [p.239]: the latch is present in F1 -- V(Y)=100 and "
                    "H(Y)=H(Y')");
                pre.push_back({"P4-PRE-LATCHF1", latch_f1});

                // Precondition: control field shows the latched value with LPENDIS clear.
                bool control_ok = Xp.present && Yp.present && M_of(Xp) == M_of(Yp) && V_of(Xp) == 100;
                checks.note("P4 " + tag + " X'/Y': V(X')=" + (Xp.present?std::to_string(V_of(Xp)):"?") +
                            " V(Y')=" + (Yp.present?std::to_string(V_of(Yp)):"?") +
                            " equal=" + (Xp.present&&Yp.present ? std::to_string(M_of(Xp)==M_of(Yp)) : "?"));
                checks.expect(control_ok,
                    "P4-PRE-CONTROL " + tag + " [p.239]: F3 (LPENDIS clear throughout) shows X'=Y', "
                    "V(X')=100");
                pre.push_back({"P4-PRE-CONTROL", control_ok});

                // ---- Main assertion: the graded relation, only when every precondition holds.
                bool x8 = X.present && (X.vposr_long & 1) != 0;
                uint32_t x70 = X.present ? (X.vhposr_long >> 8) : 0xFFFF;
                checks.note("P4 " + tag + " X: V8=" + (X.present?std::to_string((int)x8):"?") +
                            " V7-0=" + (X.present?std::to_string(x70):"?"));
                std::string why;
                for (const auto& p : pre) if (!p.second) why += " " + p.first;
                if (!why.empty()) {
                    checks.record("P4-LPENDIS-READ " + tag + ": NOT GRADED -- precondition(s) failed:" + why);
                } else {
                    bool graded = X.present && !((!x8) && x70 == 100);
                    checks.expect(graded,
                        "P4-LPENDIS-READ " + tag + " opus:7-appC-beamcon0.c4 sol:7-appC-beamcon0.c4 [p.305]: with "
                        "LPENDIS set, the pair (V8,V7-0) of X is not (0,100)");
                }

                if (mv == "P4-NOLPENDIS" && !why.empty()) {
                    checks.record("P4-LPENDIS-READ-MUTATION " + tag + ": P4-LPENDIS-READ is NOT GRADED on this RTL in the "
                        "primary run (P4-PRE-LATCHF1 and P4-PRE-CONTROL are already red: no latched pair exists for X to "
                        "differ from), so this selector, which sets LPENDIS never, cannot move it "
                        "(see the implementation record at P4-LPENDIS-READ)");
                }
            } else {
                checks.record("P4-LPENDIS-READ " + tag + ": NOT GRADED -- the fixture check P4-FIXTURE-DRIVEN failed");
            }
        }
    }

    // ================= P1 and P2 =================
    if (mv.empty() || pri_mut) run_pri_section(checks, dir, mv);

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
