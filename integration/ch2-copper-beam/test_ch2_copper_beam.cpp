// test_ch2_copper_beam.cpp -- ch2-copper-beam. See SPECIFICATION.md for
// every assertion's proposition, program, observed value and expected
// value. This file implements what SPECIFICATION.md specifies: RF1-RF4,
// P1 to P9.
//
// NMH_MUTATE selects a mutated program, or the standard the harness is
// configured for, in place of the normal one, one assertion or precondition
// at a time; none is exercised by a plain `make test`. A selected mutation
// runs only the section that owns the mutated line, with the unchanged
// comparators of that section.
//
// The mutation records: one line per label, the value of NMH_MUTATE that selects the variant, and what it changes. SPECIFICATION.md 8.6 gives the verdicts of both cores. A record of kind implementation names a private copy of the chip set: the Makefile builds the ones that carry a patch file of this package (op-test-<core>, rn-test-<core>, lc-test-<core>, cs-test-<core>) in their own object directories, and the pinned submodule stays unchanged.
//
// MUTATION FIX-LOAD stimulus red NMH_MUTATE=A-NOBIN-PAL: the run names a program of which no binary exists, so the harness loads no program and releases no CPU; also moves (both cores): FIX-CONST, PRE-DONE, PRE-S1A, PRE-S1B, PRE-L1, PRE-T1E, PRE-T2, PRE-R1, PRE-FIELDS, PRE-STD, PRE-ANCHOR, PRE-P2WAITED, PRE-E0, PRE-P7L256, P1-ORIGIN, P2-PAL, PRE-P3SAMPLES, P3-PAL, P7, P7-WAITED PASS to NOT GRADED
// MUTATION FIX-CONST stimulus red NMH_MUTATE=A-NOFIX-PAL: the program leaves out the store of $A5C3 to $0C0002, so the port logs no such write and the dependent lines are NOT GRADED; also moves (both cores): PRE-DONE, PRE-S1A, PRE-S1B, PRE-L1, PRE-T1E, PRE-T2, PRE-R1, PRE-FIELDS, PRE-STD, PRE-ANCHOR, PRE-P2WAITED, PRE-E0, PRE-P7L256, P1-ORIGIN, P2-PAL, PRE-P3SAMPLES, P3-PAL, P7, P7-WAITED PASS to NOT GRADED
// MUTATION PRE-DONE stimulus red NMH_MUTATE=A-NODONE-PAL: the whole program runs and halts in place of storing the completion word, so the completion word is never written; also moves (both cores): PRE-T1E, PRE-T2, PRE-R1, PRE-FIELDS, PRE-STD, PRE-ANCHOR, PRE-P2WAITED, PRE-E0, PRE-P7L256, P1-ORIGIN, P2-PAL, PRE-P3SAMPLES, P3-PAL, P7, P7-WAITED PASS to NOT GRADED
// MUTATION PRE-S1A none impossible unfalsifiable at this boundary: no input of the programs changes the refresh slots (RF1)
// MUTATION PRE-S1B none impossible unfalsifiable at this boundary: no input of the programs changes the refresh slots, so no two accesses can be made to share one (RF1)
// MUTATION PRE-L1 stimulus red NMH_MUTATE=A-EARLYCOP-PAL: the Copper is started before the CPU writes the list, and the CPU writes it once V is in $20 .. $3F, so the Copper's first fetch of the list precedes the CPU's first write of it; the program reaches DONE and every later field runs the written list; also moves (both cores): PRE-T1E, PRE-ANCHOR, PRE-P2WAITED, PRE-E0, PRE-P7L256, P1-ORIGIN, P2-PAL, P3-PAL, P7, P7-WAITED PASS to NOT GRADED
// MUTATION PRE-T1E stimulus red NMH_MUTATE=A-SLOW-PAL: the flat run is cut into 50 blocks of 1000 samples with a loop of 20 dbra iterations between two blocks (more than 16 slots, less than a line), so events fall between samples more than 16 slots apart; also moves (both cores): PRE-ANCHOR, PRE-P7L256, P1-ORIGIN, P2-PAL, P7, P7-WAITED PASS to NOT GRADED
// MUTATION PRE-T2 stimulus red NMH_MUTATE=A-DUP-PAL: after every 5000 samples one read of $DFF004 is stored twice, so two consecutive samples have the same V and H and H does not increase; also moves (both cores): PRE-R1, PRE-FIELDS, PRE-ANCHOR, PRE-P7L256, P1-ORIGIN, P2-PAL, PRE-P3SAMPLES, P3-PAL, P7, P7-WAITED PASS to NOT GRADED
// MUTATION PRE-R1 stimulus red NMH_MUTATE=A-NOCOP-PAL: DMACON = $0080 (COPEN cleared) is written between two parts of the flat run, after the third field boundary the trace sees, so the later fields have no restart; also moves (both cores): P1-ORIGIN, P2-PAL, P7, P7-WAITED PASS to NOT GRADED
// MUTATION PRE-FIELDS stimulus red NMH_MUTATE=A-FEW-PAL: the trace buffer holds 35000 samples, fewer than 6 complete fields; also moves (both cores): P1-ORIGIN, P2-PAL, PRE-P3SAMPLES, P3-PAL, P7, P7-WAITED PASS to NOT GRADED
// MUTATION PRE-P3SAMPLES none impossible no isolated stimulus at this boundary: with PRE-T2 every line of a complete field holds a trusted sample, so fewer than 114 trusted samples needs a field of fewer than 114 lines; VTOTAL with VARBEAMEN (p.285, p.305) makes such fields, but a field of fewer than 114 lines has no line 256, so one of PRE-S1A, PRE-S1B, PRE-L1, PRE-T1E, PRE-R1, PRE-P7L256 fails with it; measured with VTOTAL = 4 (6-line fields, at most 106 trusted samples) on both cores: PRE-P3SAMPLES, PRE-T1E, PRE-P2WAITED, PRE-E0 FAIL
// MUTATION PRE-STD stimulus red NMH_MUTATE=A-STD-NTSC-PALCHIP: the NTSC program runs on a machine configured as PAL, so its chip identification names PAL while the line is graded as NTSC; also moves (both cores): P2-NTSC, PRE-P3COVER, P3-NTSC-SET FAIL to NOT GRADED; P1-ORIGIN, P7, P7-WAITED, P8-LINES PASS to NOT GRADED
// MUTATION PRE-ANCHOR stimulus red NMH_MUTATE=A-ANCHOR-PAL: every WAIT(v,$40) becomes WAIT(v+1,$40), so m_v lies on line v+1; also moves (both cores): P2-PAL, P7, P7-WAITED PASS to NOT GRADED
// MUTATION PRE-P2WAITED stimulus red NMH_MUTATE=A-P2WAITED-PAL: an extra WAIT($7F,$40) after MARK($7F) is fetched after its target has passed, so its delay equals lambda_min; also moves (both cores): P2-PAL PASS to NOT GRADED
// MUTATION PRE-E0 stimulus red NMH_MUTATE=A-NOE0-PAL: MARK($A0E0) after WAIT($FF,$E0) is another padding move, so m_E0 does not exist while the WAIT and the padding stay; also moves (both cores): P7, P7-WAITED PASS to NOT GRADED
// MUTATION PRE-P7L256 stimulus red NMH_MUTATE=A-P7EARLY-PAL: WAIT($00,$60) and its marker precede WAIT($FF,$E0), right after m_$FF, so its second word is fetched on line 255 while m_E0 and the other tail WAITs are as in the normal run; also moves (both cores): P2-PAL, P7, P7-WAITED PASS to NOT GRADED
// MUTATION P1-ORIGIN implementation red private chip-set copy, outside the repository (restart_nonzero.patch; NMH_ONLY=A make rn-test-fx68k, rn-test-tg68k), whose horizontal count starts each line at 1 in place of 0 and ends it at $E3 in place of $E2 (227 counts, as before), so H = 0 is never read while the program, the sampler and every precondition are those of the normal run: the normal run A through the unchanged comparator prints FAIL (PAL and NTSC) where the repository's RTL prints PASS; also moves (both cores): P3-PAL PASS to FAIL (p.23: the count has values "$0 to $E2"; $00 is not read and $E3 is)
// MUTATION P2-PAL implementation red private chip-set copy, outside the repository (odd_pal.patch; NMH_ONLY=A make op-test-fx68k, op-test-tg68k), whose odd PAL lines end one count later (228 counts, 0 ... $E3; p.23: "In PAL, they are all 227 long"): the normal PAL run A measures L(v) = 228 on every other line and the unchanged comparator prints FAIL where the repository's RTL prints PASS, every precondition holding; also moves (both cores): P3-PAL PASS to FAIL
// MUTATION P2-NTSC implementation green private chip-set copy, outside the repository (ntsc_long.patch; NMH_ONLY=A make lc-test-fx68k, lc-test-tg68k), whose NTSC lines alternate between 228 and 227 counts; the normal NTSC run A through the unchanged comparator prints PASS where the repository's RTL prints FAIL; also moves (both cores): PRE-P3COVER, P3-NTSC-SET FAIL to PASS; P3-NTSC-LONGONLY NOT GRADED to PASS
// MUTATION PRE-P3ODDEND stimulus red NMH_MUTATE=A-NOE1-NTSC: on each of 3000 lines the program compares every read of $DFF004 with $E1 and stores it unless equal until H is below $20, then stores 15 reads without compare, which end before $E1, so no sample whose H is $E1 is stored; PRE-T1E holds on both cores (0 events hit a step longer than 16 slots); also moves (both cores): P3-NTSC-SET FAIL to NOT GRADED
// MUTATION PRE-P3COVER implementation green the private chip-set copy of ntsc_long.patch (NMH_ONLY=A make lc-test-fx68k, lc-test-tg68k), whose NTSC lines have 228 and 227 counts: H >= $E0 is observed on more than 20 lines of each length, so the unchanged comparator prints PASS where the repository's RTL prints FAIL; also moves (both cores): P2-NTSC, P3-NTSC-SET FAIL to PASS; P3-NTSC-LONGONLY NOT GRADED to PASS
// MUTATION P3-PAL implementation red private chip-set copy, outside the repository (odd_pal.patch; NMH_ONLY=A make op-test-fx68k, op-test-tg68k), whose odd PAL lines end one count later (228 counts, 0 ... $E3; p.23: "In PAL, they are all 227 long", $E3 belongs to NTSC long lines): the normal PAL run A reads $E3 and the unchanged comparator prints FAIL where the repository's RTL prints PASS, every precondition holding; also moves (both cores): P2-PAL PASS to FAIL
// MUTATION P3-NTSC-SET implementation green the private chip-set copy of ntsc_long.patch (NMH_ONLY=A make lc-test-fx68k, lc-test-tg68k), whose NTSC lines have 228 and 227 counts: $E3 is read, so the unchanged comparator prints PASS where the repository's RTL prints FAIL; also moves (both cores): P2-NTSC, PRE-P3COVER FAIL to PASS; P3-NTSC-LONGONLY NOT GRADED to PASS
// MUTATION P3-NTSC-LONGONLY none impossible unfalsifiable at this boundary: a line whose counts run 0 ... $E3 has 228 counts, so no stimulus makes a line with L(v) = 227 show $E3; it is NOT GRADED on the repository's RTL (PRE-P3COVER fails) and graded, and PASS, on the private chip-set copy of ntsc_long.patch (NMH_ONLY=A make lc-test-fx68k, lc-test-tg68k), whose NTSC lines have 228 and 227 counts (normal NTSC run A)
// MUTATION P7 stimulus red NMH_MUTATE=A-TAILSHIFT-PAL: every tail WAIT(n,$60) becomes WAIT(n+1,$60), so m_(256+n) lies on line 257+n
// MUTATION P7-WAITED stimulus red NMH_MUTATE=A-LONGPAD-PAL: 16 further padding moves put the fetch of WAIT(0,$60) after count $60, so it is already satisfied; also moves (both cores): P2-PAL PASS to NOT GRADED
// MUTATION P8-LINES stimulus red NMH_MUTATE=A-TAILSHIFT-NTSC: every tail WAIT(n,$60) becomes WAIT(n+1,$60), so m_(256+n) lies on line 257+n; also moves (both cores): P7 PASS to FAIL
// MUTATION PRE-C-R1LINE stimulus red NMH_MUTATE=C-CR1LINE-PAL: WAIT(w_5,$C0) becomes WAIT(w_5-1,$C0), so r1_5 lies on line w_5-1, the line of r2_4, while every later WAIT still waits; also moves (both cores): P4, P4-A, P4-B, P4-C PASS to NOT GRADED
// MUTATION PRE-C-WAITED stimulus red NMH_MUTATE=C-CLATE-PAL: the reference of k = 90 is WAIT(u_90,$C4), so the following WAIT(u_90,$B4) is fetched after its target; also moves (both cores): P4, P4-A, P4-B, P4-C PASS to NOT GRADED
// MUTATION P4-A stimulus red NMH_MUTATE=C-CSHIFT-PAL: WAIT(u_50,$66) in place of WAIT(u_50,$64), so D_50 changes by 2 slots; also moves (both cores): P4 PASS to FAIL
// MUTATION P4-B stimulus red NMH_MUTATE=C-CSHIFTEND-PAL: WAIT(u_106,$D6) in place of WAIT(u_106,$D4), so D_106 is not below D_107; also moves (both cores): P4 PASS to FAIL
// MUTATION P4-C stimulus red NMH_MUTATE=C-CNEXT-PAL: q_5 is WAIT(w_5+1,$50), so the WAIT of r2_5 is fetched after its target and did not wait; also moves (both cores): P4 PASS to FAIL
// MUTATION P4 stimulus red NMH_MUTATE=C-CSHIFT-PAL: WAIT(u_50,$66) in place of WAIT(u_50,$64), so the aggregate fails through P4-A; also moves (both cores): P4-A PASS to FAIL
// MUTATION PRE-B-CHAIN stimulus red NMH_MUTATE=B-BCHAIN-PAL: the COP1LC moves of L_3 name L_5, so the restart after L_3 fetches L_5 and not L_4; also moves (both cores): P6-LARGEST FAIL to NOT GRADED; P6-LINE255, P6-NEVER, P6-CONTROL PASS to NOT GRADED
// MUTATION PRE-B-L255 stimulus red NMH_MUTATE=B-BLATE-PAL: in L_1 a WAIT($FF,$E0) and 16 padding moves precede WAIT($FF,h_1), so its second word is fetched on line 256; also moves (both cores): P6-LARGEST FAIL to NOT GRADED; P6-LINE255, P6-NEVER, P6-CONTROL PASS to NOT GRADED
// MUTATION PRE-B-WAITED stimulus red NMH_MUTATE=B-BWAITED-PAL: in L_4 a WAIT($FF,$80) precedes WAIT($FF,$40), so the latter is fetched after its target; also moves (both cores): P6-LARGEST FAIL to NOT GRADED; P6-LINE255, P6-NEVER, P6-CONTROL PASS to NOT GRADED
// MUTATION P6-LINE255 stimulus red NMH_MUTATE=B-B254-PAL: in L_2 the first WAIT is for line $FE and a WAIT($FF,$30) follows it, so a_2 lies on line 254 while the preconditions still hold
// MUTATION P6-CONTROL stimulus red NMH_MUTATE=B-BFEOFF-PAL: L_FE waits for ($FF,$FE), so $B300 is not fetched
// MUTATION P6-LARGEST implementation green private chip-set copy, outside the repository (wait_e2.patch; NMH_ONLY=B make cs-test-fx68k, cs-test-tg68k), whose WAIT for ($FF,$E2) ends in line $FF; the normal run B through the unchanged comparator prints PASS where the repository's RTL prints FAIL (x_0 is fetched 163 slots after a_0). NMH_MUTATE=B-BE4-PAL (L_0 waits for $E4, the specification's contrast for x_0 not fetched) leaves the line FAIL on both cores, as in the normal run: the line is red before the contrast changes anything, so the contrast discriminates nothing here
// MUTATION P6-NEVER stimulus red NMH_MUTATE=B-BEND-PAL: L_14 waits for ($FE,$FE) in place of ($FF,$FE), which ends when line $FF begins, so x_14 is fetched before line 0 of the next field while every precondition holds
// MUTATION PRE-P5-CALLO stimulus red NMH_MUTATE=D-DFLATLO-PAL: every plane word of D-lo is $FFFF, so there are no alternating runs; also moves (both cores): P5-LO, P9-A-E, P9-A-EW, P9-B, P9-A PASS to NOT GRADED
// MUTATION PRE-P5-CALHI stimulus red NMH_MUTATE=D-DFLATHI-PAL: every plane word of D-hi is $FFFF, so there are no alternating runs
// MUTATION PRE-P5-EDGES stimulus red NMH_MUTATE=D-DDROP-PAL: the second MOVE($180,B) of line $50 is left out, so line $50 has no A to B edge; also moves (both cores): P5-LO, P5-HI PASS to NOT GRADED
// MUTATION PRE-P5-WAITED stimulus red NMH_MUTATE=D-DLATE-PAL: the second WAIT of line $50 is at $3A, already satisfied when it is fetched; also moves (both cores): P5-LO, P5-HI PASS to NOT GRADED
// MUTATION P5-LO stimulus red NMH_MUTATE=D-DSHIFT-PAL: the second WAIT of line $50 is at $36+2*24+2, so w_$50 is 4 * 25 * P_lo while n_$50 is 24; also moves (both cores): P5-HI PASS to FAIL
// MUTATION P5-HI stimulus red NMH_MUTATE=D-DSHIFT-PAL: the second WAIT of line $50 is at $36+2*24+2, so w_$50 is 8 * 25 * P_hi while n_$50 is 24; also moves (both cores): P5-LO PASS to FAIL
// MUTATION PRE-P9-LINES stimulus red NMH_MUTATE=E-ENOBPL-PAL: bitplane DMA is left off, so no output line shows COLOR01; also moves (both cores): PRE-P9-ONE0, P9-A-E, P9-A-EW, P9-B, P9-A PASS to NOT GRADED
// MUTATION PRE-P9-ONE0 stimulus red NMH_MUTATE=E-EBLACKGAP-PAL: COLOR00 = $000 and the 10th plane word of every line is $0000, so each window line has a second run of 0; also moves (both cores): P9-A-E, P9-A-EW, P9-B, P9-A PASS to NOT GRADED
// MUTATION P9-A-E stimulus red NMH_MUTATE=E-EWIDEWIN-PAL: DIWSTRT $2C71 and DIWSTOP $2CD1 with 22 words of $FFFF per line, so the COLOR01 run is 352 * P_lo; also moves (both cores): P9-A PASS to FAIL
// MUTATION P9-A-EW stimulus red NMH_MUTATE=E-EWWIDEWIN-PAL: the E-widefetch capture is replaced by E-widewindow, so its COLOR01 run is 352 * P_lo; also moves (both cores): P9-A PASS to FAIL
// MUTATION P9-A stimulus red NMH_MUTATE=E-EWIDEWIN-PAL: DIWSTRT $2C71 and DIWSTOP $2CD1 with 22 words of $FFFF per line, so the aggregate fails through P9-A-E; also moves (both cores): P9-A-E PASS to FAIL
// MUTATION P9-B stimulus red NMH_MUTATE=E-EBRDBLNK-PAL: BPLCON0 bit 0 and BPLCON3 bit 5 are set with COLOR00 = $00F, so the unused portion shows pixels of value 0 in place of COLOR00 on every window line (2048 of 2048 lines differ); p.307 lists BRDRBLNK as "Border blank" and ENBPLCN3 as "Enable new BLPCON3 register." and no page says what a blanked border displays: the package expects pixels of value 0 there; also moves (both cores): P9-A PASS to FAIL

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

// ---- the fixed assertion call form --------------------------------------
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

// ---- memory layout (SPECIFICATION.md 3.6) --------------------------------
constexpr uint32_t kListBase    = 0x010000;
constexpr uint32_t kListBaseW   = kListBase / 2;
constexpr uint32_t kListWords   = 0x8000;
constexpr uint32_t kTraceBase   = 0x100000;
constexpr uint32_t kTraceBaseW  = kTraceBase / 2;
constexpr uint32_t kTraceMaxW   = 0x080000;
constexpr uint32_t kResultsW    = 0x0C0000 / 2;
constexpr uint32_t kFixtureW    = 0x0C0002 / 2;
constexpr uint32_t kDoneAddr    = 0x0C0FFE;
constexpr uint16_t kDoneValue   = 0x600D;
constexpr uint16_t kFixtureValue = 0xA5C3;

// chipset_config values: ECS in every run. The second value of each pair
// is the harness input that gives an NTSC chip identification; PRE-STD
// checks the identification of every run and nothing else here assumes it.
constexpr uint8_t kECS_PAL  = 0x08;
constexpr uint8_t kECS_NTSC = 0x0A;
constexpr uint8_t kOCS_PAL  = 0x00;
constexpr uint8_t kOCS_NTSC = 0x02;

constexpr uint64_t kBootBudget    = 6'000'000;
constexpr uint64_t kRunBudgetA    = 20'000'000;
constexpr uint64_t kRunBudgetB    = 30'000'000;
constexpr uint64_t kRunBudgetC    = 14'000'000;
constexpr uint64_t kRunBudgetDE   = 24'000'000;

enum Tag { PAL = 0, NTSC = 1 };
const char* tagname(Tag t) { return t == PAL ? "PAL" : "NTSC"; }
uint8_t cfg_ecs(Tag t) { return t == PAL ? kECS_PAL : kECS_NTSC; }
uint8_t cfg_ocs(Tag t) { return t == PAL ? kOCS_PAL : kOCS_NTSC; }

std::string num(long long v) { return std::to_string(v); }
std::string slots(long long v) { return std::to_string(v) + (v == 1 ? " slot" : " slots"); }
std::string hex2(unsigned v) { char b[16]; std::snprintf(b, sizeof b, "%02X", v & 0xFF); return b; }
std::string hex4(unsigned v) { char b[16]; std::snprintf(b, sizeof b, "%04X", v & 0xFFFF); return b; }

// ---- capture ---------------------------------------------------------------
struct Capture {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> raw;  // for S1/refresh (see measure_slots)
    std::vector<nmh::RamAccess> log;  // coalesced, for everything else
    std::vector<nmh::VideoSample> video;
};

Capture run_capture(const std::string& bin, uint8_t chipset_config, uint64_t run_budget, bool keep_video = false) {
    Capture c;
    nmh::Config cfg;
    cfg.chipset_config = chipset_config;
    nmh::NanomigHarness h(cfg);
    size_t loaded = h.load_program(bin);
    c.loaded = loaded > 0;
    if (!c.loaded) return c;
    auto boot = h.boot(kBootBudget);
    c.released = boot.cpu_released;
    c.done = h.run_until_write(kDoneAddr, kDoneValue, run_budget);
    c.raw = h.ram_log();
    c.log = nmh::coalesce(c.raw);
    if (keep_video) c.video = h.video();
    return c;
}

// ---- RF1: memory slots (SPECIFICATION.md 3.2) ----------------------------
// Refresh reads are RAM-port reads at word address 0, four per line, each two
// slots after the one before (Figure 6-9). They are identified from the raw
// (uncoalesced) log: coalesce() merges consecutive same-address, same-data
// entries regardless of how many real cycles separate them.
struct SlotRef {
    bool ok = false;
    uint64_t c_R = 0;
    int64_t S = 0;
    bool spacing_consistent = true;
    size_t groups_found = 0;
};

int64_t floordiv(int64_t a, int64_t b) {
    int64_t q = a / b, r = a % b;
    if (r != 0 && ((r < 0) != (b < 0))) --q;
    return q;
}

SlotRef measure_slots(const std::vector<nmh::RamAccess>& raw) {
    SlotRef r;
    std::vector<uint64_t> reads;
    for (auto& a : raw) if (!a.is_write && a.word_addr == 0) reads.push_back(a.cycle);
    if (reads.size() < 8) return r;
    std::vector<std::vector<uint64_t>> groups;
    groups.push_back({reads[0]});
    for (size_t i = 1; i < reads.size(); ++i) {
        if (reads[i] - reads[i - 1] > 100) groups.push_back({});
        groups.back().push_back(reads[i]);
    }
    std::vector<int64_t> spacings;
    std::vector<uint64_t> firsts;
    for (auto& g : groups) {
        if (g.size() != 4) continue;
        int64_t sp = (int64_t)g[1] - (int64_t)g[0];
        bool ok = sp > 0;
        for (size_t i = 1; ok && i < 4; ++i)
            if ((int64_t)g[i] - (int64_t)g[i - 1] != sp) ok = false;
        if (!ok) continue;
        spacings.push_back(sp);
        firsts.push_back(g[0]);
    }
    r.groups_found = spacings.size();
    if (spacings.empty()) return r;
    int64_t sp0 = spacings[0];
    for (auto sp : spacings) if (sp != sp0) r.spacing_consistent = false;
    r.S = sp0 / 2;
    r.c_R = firsts[0];
    r.ok = r.S > 0;
    return r;
}

int64_t slot_of(const SlotRef& ref, uint64_t cycle) { return floordiv((int64_t)cycle - (int64_t)ref.c_R, ref.S); }

// S1's second clause: no two logged accesses, merged per bus cycle, occupy
// the same slot. slot() is non-decreasing in cycle, so a same-slot pair
// anywhere in the log implies a same-slot ADJACENT pair too.
bool s1_no_collision(const SlotRef& ref, const std::vector<nmh::RamAccess>& log, size_t* violations) {
    *violations = 0;
    int64_t prev = INT64_MIN;
    bool ok = ref.ok;
    for (auto& a : log) {
        int64_t s = slot_of(ref, a.cycle);
        if (s == prev) { ok = false; ++*violations; }
        prev = s;
    }
    return ok;
}

// ---- RF2: beam trace -------------------------------------------------------
struct Sample { uint64_t cyc; int64_t w; int V, H, id; size_t idx; bool trusted; };

std::vector<Sample> decode_trace(const SlotRef& ref, const std::vector<nmh::RamAccess>& log) {
    std::vector<const nmh::RamAccess*> ws;
    for (auto& a : log)
        if (a.is_write && a.word_addr >= kTraceBaseW && a.word_addr < kTraceBaseW + kTraceMaxW) ws.push_back(&a);
    std::vector<Sample> out;
    out.reserve(ws.size() / 2);
    for (size_t i = 0; i + 1 < ws.size(); i += 2) {
        uint16_t hi = ws[i]->data, lo = ws[i + 1]->data;
        Sample s;
        s.cyc = std::max(ws[i]->cycle, ws[i + 1]->cycle);
        s.w = ref.ok ? slot_of(ref, s.cyc) : 0;
        int V8 = hi & 1;
        s.V = V8 * 256 + ((lo >> 8) & 0xFF);
        s.H = lo & 0xFF;
        s.id = (hi >> 8) & 0x7F;
        s.idx = out.size();
        s.trusted = s.H >= 16;
        out.push_back(s);
    }
    return out;
}

std::vector<Sample> trusted_of(const std::vector<Sample>& s) {
    std::vector<Sample> t;
    for (auto& x : s) if (x.trusted) t.push_back(x);
    return t;
}

// Chip identification (SPECIFICATION.md 3.1, p.299): 00 and 20 name PAL, 10
// and 30 name NTSC. Returns 0 for PAL, 1 for NTSC, -1 for neither or mixed.
int std_of_id(int id) {
    if (id == 0x00 || id == 0x20) return 0;
    if (id == 0x10 || id == 0x30) return 1;
    return -1;
}

struct RF2Precond {
    bool t1 = true; size_t t1_violations = 0, t1_total = 0, t1_at_boundary = 0;
    bool t2 = true; size_t t2_violations = 0, t2_total = 0;
};

RF2Precond check_rf2(const std::vector<Sample>& all, const std::vector<Sample>& trusted) {
    RF2Precond r;
    for (size_t i = 0; i + 1 < all.size(); ++i) {
        ++r.t1_total;
        if (all[i + 1].w - all[i].w > 16) {
            r.t1 = false; ++r.t1_violations;
            if (all[i + 1].V < all[i].V) ++r.t1_at_boundary;
        }
    }
    for (size_t i = 0; i + 1 < trusted.size(); ++i) {
        ++r.t2_total;
        int V0 = trusted[i].V, V1 = trusted[i + 1].V, H0 = trusted[i].H, H1 = trusted[i + 1].H;
        bool ok = (V0 == V1) ? (H1 > H0) : (V1 == V0 + 1 || V1 == 0);
        if (!ok) { r.t2 = false; ++r.t2_violations; }
    }
    return r;
}

// ---- RF3: fields and Copper restarts ----------------------------------------
struct Restart { uint64_t cycle; uint32_t base; };
struct RWin { uint64_t lo, hi; uint32_t base; };
struct FieldSpan { size_t lo, hi; }; // trusted[] index range, half-open

std::vector<size_t> trace_wraps(const std::vector<Sample>& trusted) {
    std::vector<size_t> w;
    for (size_t i = 1; i < trusted.size(); ++i) if (trusted[i].V < trusted[i - 1].V) w.push_back(i);
    return w;
}

std::vector<Restart> restart_events(const std::vector<nmh::RamAccess>& log, const std::set<uint32_t>& bases) {
    std::vector<Restart> v;
    for (auto& a : log) if (!a.is_write && bases.count(a.word_addr)) v.push_back({a.cycle, a.word_addr});
    return v;
}

// Complete fields begin after the first restart (restarts[1]; restarts[0]
// is the run's first fetch, not a restart).
std::vector<FieldSpan> complete_fields(const std::vector<Sample>& trusted, const std::vector<Restart>& restarts) {
    std::vector<FieldSpan> out;
    if (restarts.size() < 2) return out;
    uint64_t first_restart = restarts[1].cycle;
    auto wraps = trace_wraps(trusted);
    std::vector<size_t> kept;
    for (auto idx : wraps) if (trusted[idx].cyc >= first_restart) kept.push_back(idx);
    for (size_t k = 0; k + 1 < kept.size(); ++k) out.push_back({kept[k], kept[k + 1]});
    return out;
}

// Restart-bounded field windows: window k is [restarts[k+1], restarts[k+2]).
std::vector<RWin> restart_windows(const std::vector<Restart>& restarts) {
    std::vector<RWin> out;
    for (size_t k = 0; k + 2 < restarts.size(); ++k) out.push_back({restarts[k + 1].cycle, restarts[k + 2].cycle, restarts[k + 1].base});
    return out;
}

struct R1Check { bool ok = true; size_t violations = 0, spans = 0; };

R1Check check_r1(const std::vector<Sample>& trusted, const std::vector<Restart>& restarts) {
    R1Check r;
    auto wraps = trace_wraps(trusted);
    for (size_t k = 0; k + 1 < wraps.size(); ++k) {
        uint64_t lo = trusted[wraps[k]].cyc, hi = trusted[wraps[k + 1]].cyc;
        size_t cnt = 0;
        for (size_t ri = 1; ri < restarts.size(); ++ri) if (restarts[ri].cycle >= lo && restarts[ri].cycle < hi) ++cnt;
        ++r.spans;
        if (cnt != 1) { r.ok = false; ++r.violations; }
    }
    if (r.spans == 0) r.ok = false;
    return r;
}

// ---- L1: the Copper's first fetches read what the CPU wrote ----------------
bool check_l1(const std::vector<nmh::RamAccess>& log, uint32_t base_w, uint16_t e0, uint16_t e1) {
    std::optional<uint64_t> w0, w1, r0, r0d, r1, r1d, w0c, w1c;
    for (auto& a : log) {
        if (a.is_write && a.word_addr == base_w && !w0) { w0 = a.data; w0c = a.cycle; }
        if (a.is_write && a.word_addr == base_w + 1 && !w1) { w1 = a.data; w1c = a.cycle; }
        if (!a.is_write && a.word_addr == base_w && !r0) { r0 = a.cycle; r0d = a.data; }
        if (!a.is_write && a.word_addr == base_w + 1 && !r1) { r1 = a.cycle; r1d = a.data; }
    }
    return w0 && w1 && r0d && r1d && *r0 > *w0c && *r1 > *w1c && *w0 == *r0d && *w1 == *r1d && *w0 == e0 && *w1 == e1;
}

bool check_fixture(const std::vector<nmh::RamAccess>& log) {
    for (auto& a : log) if (a.is_write && a.word_addr == kFixtureW && a.data == kFixtureValue) return true;
    return false;
}

// ---- the lists as the CPU wrote them -----------------------------------------
struct Instr {
    uint32_t a; uint16_t w1, w2;
    bool wait() const { return (w1 & 1) != 0; }
    int v() const { return w1 >> 8; }
    int h() const { return w1 & 0xFE; }
};

std::map<uint32_t, uint16_t> list_memory(const std::vector<nmh::RamAccess>& raw) {
    std::map<uint32_t, uint16_t> m;
    for (auto& a : raw)
        if (a.is_write && a.word_addr >= kListBaseW && a.word_addr < kListBaseW + kListWords) m[a.word_addr] = a.data;
    return m;
}

// The words the CPU wrote from base on, two per instruction, up to the first word it did not write.
// WAIT($FF,$FE) has the words of END (p.23), so the decoding does not stop at them.
std::vector<Instr> decode_list(const std::map<uint32_t, uint16_t>& mem, uint32_t base) {
    std::vector<Instr> out;
    uint32_t a = base;
    for (int guard = 0; guard < 4000; ++guard, a += 2) {
        auto i1 = mem.find(a), i2 = mem.find(a + 1);
        if (i1 == mem.end() || i2 == mem.end()) break;
        out.push_back({a, i1->second, i2->second});
    }
    return out;
}

// marker data -> address of the marker's first word (a MOVE to $1FE)
std::unordered_map<uint16_t, uint32_t> marker_map(const std::vector<Instr>& list) {
    std::unordered_map<uint16_t, uint32_t> m;
    for (auto& i : list) if (i.w1 == 0x01FE && !m.count(i.w2)) m[i.w2] = i.a;
    return m;
}

// ---- Copper fetches ------------------------------------------------------------
using ReadIndex = std::unordered_map<uint32_t, std::vector<uint64_t>>;

ReadIndex build_read_index(const std::vector<nmh::RamAccess>& log) {
    ReadIndex idx;
    for (auto& a : log) {
        if (a.is_write) continue;
        if (a.word_addr < kListBaseW || a.word_addr >= kListBaseW + kListWords) continue;
        idx[a.word_addr].push_back(a.cycle);
    }
    return idx;
}

std::optional<uint64_t> fetch_in(const ReadIndex& idx, uint32_t addr, uint64_t lo, uint64_t hi) {
    auto it = idx.find(addr);
    if (it == idx.end()) return std::nullopt;
    auto lb = std::lower_bound(it->second.begin(), it->second.end(), lo);
    if (lb == it->second.end() || *lb >= hi) return std::nullopt;
    return *lb;
}

// ---- one analysed trace run ---------------------------------------------------------
struct Analysis {
    Capture cap;
    SlotRef sref;
    std::vector<Sample> all, trusted;
    RF2Precond rf2;
    std::vector<Restart> restarts;
    std::vector<FieldSpan> fields;
    std::vector<RWin> rwins;
    R1Check r1;
    bool l1 = false, fixture = false;
    ReadIndex ridx;
    bool s1_spacing = false, s1_collision = false;
    size_t s1_violations = 0;
    std::map<uint32_t, uint16_t> mem;
    int std_class = -1;      // 0 PAL, 1 NTSC, -1 neither or mixed
    bool std_known = false;
};

Analysis analyze(Capture&& cap, const std::set<uint32_t>& bases, uint16_t l1w0, uint16_t l1w1) {
    Analysis a;
    a.cap = std::move(cap);
    a.sref = measure_slots(a.cap.raw);
    a.all = decode_trace(a.sref, a.cap.log);
    a.trusted = trusted_of(a.all);
    a.rf2 = check_rf2(a.all, a.trusted);
    a.restarts = restart_events(a.cap.log, bases);
    a.fields = complete_fields(a.trusted, a.restarts);
    a.rwins = restart_windows(a.restarts);
    a.r1 = check_r1(a.trusted, a.restarts);
    a.l1 = check_l1(a.cap.log, *bases.begin(), l1w0, l1w1);
    a.fixture = check_fixture(a.cap.log);
    a.ridx = build_read_index(a.cap.log);
    a.s1_spacing = a.sref.ok && a.sref.spacing_consistent;
    a.s1_collision = s1_no_collision(a.sref, a.cap.log, &a.s1_violations);
    a.mem = list_memory(a.cap.raw);
    int cls = -2;
    for (auto& s : a.all) {
        int c = std_of_id(s.id);
        if (cls == -2) cls = c; else if (cls != c) { cls = -1; break; }
    }
    a.std_class = cls == -2 ? -1 : cls;
    a.std_known = a.std_class >= 0;
    return a;
}

// "Line of an event" (SPECIFICATION.md RF2). trusted is sorted by w. The
// sample before s+ in the full trace must be written at or after t: that is
// the clause of T1 that the identification rests on.
struct LineId { int line = -1; int status = 1; }; // 0 identified, 1 outside the trace, 2 samples disagree, 3 step too long

LineId line_of(const Analysis& a, int64_t t) {
    LineId r;
    auto it = std::upper_bound(a.trusted.begin(), a.trusted.end(), t,
        [](int64_t val, const Sample& s) { return val < s.w; });
    if (it == a.trusted.begin()) return r;
    const Sample& sminus = *(it - 1);
    auto it2 = std::lower_bound(a.trusted.begin(), a.trusted.end(), t + 16,
        [](const Sample& s, int64_t val) { return s.w < val; });
    if (it2 == a.trusted.end()) return r;
    const Sample& splus = *it2;
    if (splus.idx == 0) return r;
    const Sample& prev = a.all[splus.idx - 1];
    if (prev.w < t) { r.status = 3; return r; }
    if (sminus.V != splus.V) { r.status = 2; return r; }
    r.line = sminus.V; r.status = 0;
    return r;
}

std::optional<int64_t> fetch_slot(const Analysis& a, uint32_t addr, const RWin& w) {
    auto fc = fetch_in(a.ridx, addr, w.lo, w.hi);
    if (!fc) return std::nullopt;
    return slot_of(a.sref, *fc);
}

// The start of line 0 of the next field (SPECIFICATION.md RF3), placed by count
// arithmetic on the beam trace: one count per memory slot (p.24). a is the
// last trusted sample before the first fall of V after from_cyc, and p the
// sample before it in the trace. The read of a followed the writes of p and
// preceded the writes of a, so the count at slot t of a's line is
// H(a) + (t - r) with w(p) < r < w(a). A line ends at count $E2 at the
// earliest and at count max_count_late at the latest (p.23), and line 0
// starts one slot after the end of the last line. early is the earliest and
// late the latest slot at which line 0 can have started.
struct Line0 { int64_t early, late; };

std::optional<Line0> line0_after(const Analysis& a, uint64_t from_cyc, int max_count_late) {
    for (size_t b : trace_wraps(a.trusted)) {
        if (a.trusted[b].cyc <= from_cyc) continue;
        const Sample& sa = a.trusted[b - 1];
        if (sa.cyc <= from_cyc || sa.idx == 0) return std::nullopt;
        const Sample& sp = a.all[sa.idx - 1];
        Line0 r;
        r.early = sp.w + (0xE2 - sa.H) + 1;
        r.late = sa.w + (max_count_late - sa.H) + 1;
        return r;
    }
    return std::nullopt;
}

// delay(W) of SPECIFICATION.md 3.5 for the WAIT whose second word is at
// wait_a + 1: the slot of the fetch of the word after it, minus the slot of
// the fetch of that second word.
std::optional<int64_t> wait_delay(const Analysis& a, uint32_t wait_a, const RWin& w) {
    auto s2 = fetch_slot(a, wait_a + 1, w);
    auto nx = fetch_slot(a, wait_a + 2, w);
    if (!s2 || !nx) return std::nullopt;
    return *nx - *s2;
}

// lambda_min of a window: the delay of the WAIT($00,$00) at the head of the
// list that runs in it.
std::optional<int64_t> lambda_min(const Analysis& a, const RWin& w) {
    return wait_delay(a, w.base, w);
}

struct WaitedResult { bool all = true; size_t checked = 0, expected = 0; };

// Every WAIT of `list` that satisfies pred has a delay above lambda_min, in
// every window whose list base is list_base.
template <class Pred>
WaitedResult waited_all(const Analysis& a, const std::vector<Instr>& list, uint32_t list_base, Pred pred) {
    WaitedResult r;
    for (auto& w : a.rwins) {
        if (w.base != list_base) continue;
        auto lam = lambda_min(a, w);
        for (auto& i : list) {
            if (!i.wait() || i.a == list_base || (i.w1 == 0xFFFF && i.w2 == 0xFFFE) || !pred(i)) continue;
            ++r.expected;
            auto d = wait_delay(a, i.a, w);
            if (!d || !lam) { r.all = false; continue; }
            ++r.checked;
            if (*d <= *lam) r.all = false;
        }
    }
    if (r.expected == 0 || r.checked < r.expected) r.all = false;
    return r;
}

// ---- common preconditions of every trace run ------------------------------------
// the names in the given lists, each once, in order of first appearance
std::string union_why(std::initializer_list<std::string> lists) {
    std::string out;
    for (const auto& l : lists) {
        size_t i = 0;
        while (i < l.size()) {
            while (i < l.size() && l[i] == ' ') ++i;
            size_t j = i;
            while (j < l.size() && l[j] != ' ') ++j;
            if (j > i) {
                const std::string tok = l.substr(i, j - i);
                if ((out + " ").find(" " + tok + " ") == std::string::npos) out += " " + tok;
            }
            i = j;
        }
    }
    return out;
}

// Each common precondition is graded only when the lines it rests on hold;
// otherwise it prints NOT GRADED and names them. A line is false in the
// gate when it failed or was not graded.
//   cap()    the run completed on a loaded, fixture-checked capture
//   fetch()  the slots are defined and the Copper ran the list the CPU wrote
//   trace()  fetch() and the beam trace identifies lines (RF2)
struct Gate {
    bool fix = false, done = false, s1 = false, l1 = false, t1e = false, t2 = false, r1 = false, fields = false, stdid = false;
    bool cap() const { return fix && done; }
    bool fetch() const { return fix && done && s1 && l1; }
    bool core() const { return fix && done && s1 && l1 && t1e && t2 && r1 && fields && stdid; }
    bool trace() const { return fix && done && s1 && l1 && t1e && t2; }
    bool counts() const { return fix && done && s1 && l1 && t2 && stdid; }
    std::string why_not_cap() const {
        std::string s;
        if (!fix) s += " FIX";
        if (!done) s += " PRE-DONE";
        return s;
    }
    std::string why_not_fetch() const {
        std::string s = why_not_cap();
        if (!s1) s += " PRE-S1";
        if (!l1) s += " PRE-L1";
        return s;
    }
    std::string why_not_core() const {
        std::string s = why_not_fetch();
        if (!t1e) s += " PRE-T1E";
        if (!t2) s += " PRE-T2";
        if (!r1) s += " PRE-R1";
        if (!fields) s += " PRE-FIELDS";
        if (!stdid) s += " PRE-STD";
        return s;
    }
    std::string why_not_counts() const {
        std::string s = why_not_fetch();
        if (!t2) s += " PRE-T2";
        if (!stdid) s += " PRE-STD";
        return s;
    }
    std::string why_not_trace() const {
        std::string s = why_not_fetch();
        if (!t1e) s += " PRE-T1E";
        if (!t2) s += " PRE-T2";
        return s;
    }
};

// gap_hits: the number of events of this run whose line identification
// failed because the step before s+ ended before the event (the clause of
// T1 the identification rests on).
Gate report_common(Checks& checks, const Analysis& a, Tag tag, const std::string& run, size_t min_fields,
                   size_t gap_hits, size_t events_identified) {
    Gate g;
    const std::string tg = tagname(tag);
    const std::string rn = " run " + run + ": ";
    // a precondition whose prerequisites failed is printed NOT GRADED, naming them
    auto not_graded = [&](const std::string& id, const std::string& why) {
        checks.record(id + " " + tg + rn + "NOT GRADED: precondition failed:" + why);
    };
    const bool load = a.cap.loaded && a.cap.released;
    checks.expect(load, "FIX-LOAD " + tg + rn + "program loaded and CPU released [not graded -- apparatus check, no claim uid]");
    if (load) checks.expect(a.fixture, "FIX-CONST " + tg + rn + "the known word $A5C3 stored by the program to $0C0002 appears on the Chip-memory port with that data [not graded -- apparatus check, no claim uid]");
    else not_graded("FIX-CONST", " FIX-LOAD");
    g.fix = load && a.fixture;
    g.done = a.cap.done.has_value();
    if (g.fix) checks.expect(g.done, "PRE-DONE " + tg + rn + "the completion word was written");
    else not_graded("PRE-DONE", g.why_not_cap());
    if (g.fix) checks.expect(a.s1_spacing, "PRE-S1A " + tg + rn + "refresh spacing 2S is constant through the run (RF1)");
    else not_graded("PRE-S1A", g.why_not_cap());
    checks.note("S1 " + tg + rn + "S=" + num(a.sref.S) + " clk_sys cycles/slot, " + num((long long)a.sref.groups_found) + " refresh groups measured");
    if (g.fix && a.s1_spacing) checks.expect(a.s1_collision, "PRE-S1B " + tg + rn + "no two logged accesses occupy the same slot (RF1)");
    else not_graded("PRE-S1B", g.why_not_cap() + std::string(a.s1_spacing ? "" : " PRE-S1A"));
    g.s1 = g.fix && a.s1_spacing && a.s1_collision;
    g.l1 = g.fix && a.l1;
    if (g.fix) checks.expect(a.l1, "PRE-L1 " + tg + rn + "the Copper's first fetches read the words the CPU wrote in step 3");
    else not_graded("PRE-L1", g.why_not_cap());
    checks.note("PRE-T1 " + tg + rn + num((long long)a.rf2.t1_violations) + " of " + num((long long)a.rf2.t1_total) +
                " consecutive-sample gaps exceed 16 slots, " + num((long long)a.rf2.t1_at_boundary) + " of them across a field boundary (measured, not graded: no page gives the CPU's cycles per instruction)");
    const bool t1e = gap_hits == 0 && events_identified > 0;
    if (g.fetch()) checks.expect(t1e, "PRE-T1E " + tg + rn + "no event of this run is bracketed by a step that ended before it (the clause of T1 that RF2's line identification uses): " +
                                 num((long long)gap_hits) + " events hit such a step, " + num((long long)events_identified) + " events identified");
    else not_graded("PRE-T1E", g.why_not_fetch());
    g.t1e = g.fetch() && t1e;
    if (g.cap()) checks.expect(a.rf2.t2, "PRE-T2 " + tg + rn + "the beam counter is live between trusted samples (RF2)");
    else not_graded("PRE-T2", g.why_not_cap());
    g.t2 = g.cap() && a.rf2.t2;
    checks.note("T2 " + tg + rn + num((long long)a.rf2.t2_violations) + " of " + num((long long)a.rf2.t2_total) +
                " trusted-sample transitions violate T2");
    if (g.cap() && g.t2) checks.expect(a.r1.ok, "PRE-R1 " + tg + rn + "exactly one restart between consecutive field boundaries (RF3)");
    else not_graded("PRE-R1", g.why_not_cap() + std::string(g.t2 ? "" : " PRE-T2"));
    g.r1 = g.cap() && g.t2 && a.r1.ok;
    checks.note("R1 " + tg + rn + num((long long)a.r1.violations) + " of " + num((long long)a.r1.spans) +
                " inter-boundary spans have restart count != 1");
    const bool fields = a.fields.size() >= min_fields;
    if (g.cap() && g.t2) checks.expect(fields, "PRE-FIELDS " + tg + rn + "at least " + num((long long)min_fields) + " complete fields traced (measured " + num((long long)a.fields.size()) + ")");
    else not_graded("PRE-FIELDS", g.why_not_cap() + std::string(g.t2 ? "" : " PRE-T2"));
    g.fields = g.cap() && g.t2 && fields;
    const bool std_ok = a.std_class == (int)tag;
    if (g.cap()) checks.expect(std_ok, "PRE-STD " + tg + rn + "the chip identification in VPOSR bits 14-8 of every sample names " + tg + " (p.299)");
    else not_graded("PRE-STD", g.why_not_cap());
    g.stdid = g.cap() && std_ok;
    return g;
}

// ---- field length of a trace ---------------------------------------------------------
std::set<int> field_lengths(const Analysis& a) {
    std::set<int> lens;
    for (auto& f : a.fields) {
        std::set<int> vs;
        for (size_t i = f.lo; i < f.hi; ++i) vs.insert(a.trusted[i].V);
        lens.insert((int)vs.size());
    }
    return lens;
}

std::string setstr(const std::set<int>& s) {
    std::string o; for (int v : s) o += (o.empty() ? "" : ",") + std::to_string(v); return o.empty() ? "(none)" : o;
}
std::string setstr64(const std::set<int64_t>& s) {
    std::string o; for (auto v : s) o += (o.empty() ? "" : ",") + std::to_string(v); return o.empty() ? "(none)" : o;
}

// ==== SECTION A ===============================================================
struct APlan { bool run[2] = {true, true}; std::string bin[2]; uint8_t cfg[2]; };

struct ALists {
    std::vector<Instr> list;
    std::unordered_map<uint16_t, uint32_t> mk;
    std::optional<uint32_t> m(uint16_t d) const { auto it = mk.find(d); if (it == mk.end()) return std::nullopt; return it->second; }
};

struct AEvents {
    size_t identified = 0, gap_hits = 0, mismatches = 0, missing = 0;
};

// the line of marker data d in window w; counts the event in ev
LineId marker_line(const Analysis& a, const ALists& L, uint16_t d, const RWin& w, AEvents& ev) {
    LineId none;
    auto ma = L.m(d);
    if (!ma) { ++ev.missing; return none; }
    auto fs = fetch_slot(a, *ma, w);
    if (!fs) { ++ev.missing; return none; }
    LineId r = line_of(a, *fs);
    if (r.status == 0) ++ev.identified; else if (r.status == 3) ++ev.gap_hits; else ++ev.missing;
    return r;
}

struct P2Res {
    bool complete = true, all_227 = true, in_set = true, alt = true;
    size_t measured = 0, expected = 0;
    std::set<int64_t> values;
    std::vector<std::unordered_map<int, int64_t>> lines;   // per window: line -> L(line)
};

// L(v) = time(m_{v+1}) - time(m_v) for v = $30 ... $FE, and for the tail
// pairs of lines 256 + n, n = 0 ... tail_pairs-1 (SPECIFICATION.md P2).
P2Res grade_p2(const Analysis& a, const ALists& L, int tail_pairs) {
    P2Res r;
    for (auto& w : a.rwins) {
        std::unordered_map<int, int64_t> lm;
        std::vector<int64_t> main_part, tail_part;
        for (int v = 0x30; v < 0xFF; ++v) {
            ++r.expected;
            auto ma = L.m((uint16_t)v), mb = L.m((uint16_t)(v + 1));
            std::optional<int64_t> t0, t1;
            if (ma) t0 = fetch_slot(a, *ma, w);
            if (mb) t1 = fetch_slot(a, *mb, w);
            if (!t0 || !t1) { r.complete = false; main_part.push_back(INT64_MIN); continue; }
            int64_t l = *t1 - *t0;
            ++r.measured; r.values.insert(l); lm[v] = l; main_part.push_back(l);
        }
        for (int n = 0; n < tail_pairs; ++n) {
            ++r.expected;
            auto ma = L.m((uint16_t)(0x100 + n)), mb = L.m((uint16_t)(0x101 + n));
            std::optional<int64_t> t0, t1;
            if (ma) t0 = fetch_slot(a, *ma, w);
            if (mb) t1 = fetch_slot(a, *mb, w);
            if (!t0 || !t1) { r.complete = false; tail_part.push_back(INT64_MIN); continue; }
            int64_t l = *t1 - *t0;
            ++r.measured; r.values.insert(l); lm[256 + n] = l; tail_part.push_back(l);
        }
        for (auto* part : {&main_part, &tail_part}) {
            for (size_t i = 0; i < part->size(); ++i) {
                int64_t l = (*part)[i];
                if (l == INT64_MIN) continue;
                if (l != 227) r.all_227 = false;
                if (l != 227 && l != 228) r.in_set = false;
                if (i + 1 < part->size() && (*part)[i + 1] != INT64_MIN && (*part)[i + 1] == l) r.alt = false;
            }
        }
        r.lines.push_back(lm);
    }
    if (a.rwins.empty()) r.complete = false;
    return r;
}

// OCS cross-check of SPECIFICATION.md 3.1: recorded, not graded. It compares
// the set of field lengths and, when tail_pairs is not negative (Run A), the
// set of line lengths L(v) of P2.
void ocs_check(Checks& checks, const std::string& dir, const std::string& bin, Tag tag, const std::string& run,
               const Analysis& ecs, const std::set<uint32_t>& bases, uint16_t w0, uint16_t w1, uint64_t budget, int tail_pairs = -1) {
    Analysis ocs = analyze(run_capture(dir + "/" + bin + ".bin", cfg_ocs(tag), budget), bases, w0, w1);
    auto e = field_lengths(ecs), o = field_lengths(ocs);
    bool same = e == o;
    std::string lv;
    if (tail_pairs >= 0) {
        ALists le, lo;
        le.list = decode_list(ecs.mem, *bases.begin()); le.mk = marker_map(le.list);
        lo.list = decode_list(ocs.mem, *bases.begin()); lo.mk = marker_map(lo.list);
        P2Res pe = grade_p2(ecs, le, tail_pairs), po = grade_p2(ocs, lo, tail_pairs);
        same = same && pe.values == po.values;
        lv = "; line-length L(v) set {" + setstr64(pe.values) + "} over " + num((long long)pe.measured) + " pairs vs OCS {" + setstr64(po.values) +
             "} over " + num((long long)po.measured) + " pairs";
    }
    checks.note("OCS-CHECK " + std::string(tagname(tag)) + " run " + run + ": ECS field-length set {" + setstr(e) + "} vs OCS {" + setstr(o) + "}" + lv +
                " -- " + (same ? "match, assumption checked" : "MISMATCH, recorded not graded"));
}

int window_of(const Analysis& a, uint64_t cyc) {
    for (size_t k = 0; k < a.rwins.size(); ++k) if (cyc >= a.rwins[k].lo && cyc < a.rwins[k].hi) return (int)k;
    return -1;
}

struct P3Res {
    bool graded = true, ok = true, set_ok = true, e3_ok = true, odd_end_ok = false;
    bool cover = false;       // both parities of H are read (RF2, parity coverage)
    int parity = 0;           // the observed parity when cover is false
    bool e3_graded = true;    // the "$E3 only on long lines" clause can be graded (an odd count is readable)
    std::set<int> hs;
    size_t lines_long = 0, lines_short = 0;
    std::vector<int> missing_even, missing_odd;
};

// P3: the horizontal count range (SPECIFICATION.md P3). With parity coverage
// the graded set is the lowest count, every even count up to $E2, the highest
// count the page gives, and nothing above it; the odd counts are measured (the
// sampler reads no odd count in part of the line, see 8.4 item 2). Without
// parity coverage the graded set is every value of the one parity read, from
// the lowest to the highest not above the page's highest count, and nothing
// above it; "$E3 only on long lines" is graded only when the parity read is odd.
P3Res grade_p3(const Analysis& a, Tag expect, const P2Res& p2) {
    P3Res r;
    for (auto& s : a.all) r.hs.insert(s.H);
    const int maxH = expect == PAL ? 0xE2 : 0xE3;
    // line types near the end of a line (NTSC)
    std::set<std::pair<int, int>> long_lines, short_lines;
    bool e3_on_short = false;
    for (auto& s : a.trusted) {
        int k = window_of(a, s.cyc);
        if (k < 0 || k >= (int)p2.lines.size()) continue;
        auto it = p2.lines[k].find(s.V);
        if (it == p2.lines[k].end()) continue;
        if (s.H >= 0xE0) {
            if (it->second == 228) long_lines.insert({k, s.V});
            else if (it->second == 227) short_lines.insert({k, s.V});
        }
        if (s.H == 0xE3 && it->second == 227) e3_on_short = true;
    }
    r.lines_long = long_lines.size(); r.lines_short = short_lines.size();
    r.e3_ok = !e3_on_short;
    if (r.hs.empty()) { r.graded = false; r.ok = false; r.set_ok = false; return r; }
    bool within = true, any_even = false, any_odd = false;
    for (int h : r.hs) { if (h > maxH) within = false; if (h & 1) any_odd = true; else any_even = true; }
    r.cover = any_even && any_odd;
    r.parity = any_odd && !any_even ? 1 : 0;
    bool even_all = true;
    for (int x = 0; x <= 0xE2; x += 2) if (!r.hs.count(x)) { even_all = false; r.missing_even.push_back(x); }
    for (int x = 1; x <= 0xE1; x += 2) if (!r.hs.count(x)) r.missing_odd.push_back(x);
    r.odd_end_ok = r.hs.count(0xE1) > 0;
    if (r.cover) {
        r.set_ok = within && even_all && r.hs.count(maxH) > 0;
    } else {
        int highest = maxH;
        if ((highest & 1) != r.parity) --highest;
        bool all = true;
        for (int x = r.parity; x <= highest; x += 2) if (!r.hs.count(x)) all = false;
        r.set_ok = within && all;
        r.e3_graded = r.parity == 1;
    }
    r.ok = r.set_ok && (expect == PAL || !r.e3_graded || r.e3_ok);
    return r;
}

void section_A(Checks& checks, const std::string& dir, const std::string& mv, bool normal) {
    APlan plan;
    plan.bin[PAL] = "prog_a"; plan.bin[NTSC] = "prog_a_ntsc";
    plan.cfg[PAL] = kECS_PAL; plan.cfg[NTSC] = kECS_NTSC;
    if (!normal) {
        // mv is "A-<VARIANT>-<PAL|NTSC>" or A-STD-NTSC-PALCHIP
        plan.run[PAL] = plan.run[NTSC] = false;
        if (mv == "A-STD-NTSC-PALCHIP") {                 // the NTSC program on a machine configured PAL
            plan.run[NTSC] = true; plan.cfg[NTSC] = kECS_PAL;
        } else {
            std::string rest = mv.substr(2);              // "<VARIANT>-<PAL|NTSC>"
            size_t dash = rest.rfind('-');
            std::string var = rest.substr(0, dash), tg = rest.substr(dash + 1);
            for (auto& ch : var) ch = (char)std::tolower((unsigned char)ch);
            Tag t = tg == "NTSC" ? NTSC : PAL;
            plan.run[t] = true;
            plan.bin[t] = "prog_a_" + var + (t == NTSC ? "_ntsc" : "");
        }
    }
    const std::set<uint32_t> bases = {kListBaseW};
    for (int ti = 0; ti < 2; ++ti) {
        if (!plan.run[ti]) continue;
        const Tag tag = (Tag)ti;
        const std::string tg = tagname(tag);
        Analysis a = analyze(run_capture(dir + "/" + plan.bin[ti] + ".bin", plan.cfg[ti], kRunBudgetA), bases, 0x0001, 0xFFFE);
        ALists L; L.list = decode_list(a.mem, kListBaseW); L.mk = marker_map(L.list);

        // ---- events, computed once -----------------------------------------
        AEvents ev;
        size_t anchor_checked = 0, anchor_bad = 0;
        for (auto& w : a.rwins) {
            for (int v = 0x30; v <= 0xFF; ++v) {
                if (!L.m((uint16_t)v)) continue;
                LineId r = marker_line(a, L, (uint16_t)v, w, ev);
                if (r.status == 3) continue;
                ++anchor_checked;
                if (r.status != 0 || r.line != v) ++anchor_bad;
            }
        }
        const int tail_main = tag == PAL ? 55 : 5;            // P2 pairs of lines 256 + n
        const int p7_max = tag == PAL ? 55 : 5;               // P7: n = 0 ... p7_max
        size_t p7_expected = 0, p7_identified = 0, p7_bad = 0;
        for (auto& w : a.rwins) {
            for (int n = 0; n <= p7_max; ++n) {
                ++p7_expected;
                if (!L.m((uint16_t)(0x100 + n))) continue;
                LineId r = marker_line(a, L, (uint16_t)(0x100 + n), w, ev);
                if (r.status == 3) continue;
                if (r.status != 0) continue;
                ++p7_identified;
                if (r.line != 256 + n) ++p7_bad;
            }
        }
        // m_E0 reached, and the line of the second word of WAIT(0,$60)
        size_t e0_reached = 0, w256_ok = 0, w256_checked = 0;
        auto e0 = L.m(0xA0E0);
        auto t0m = L.m(0x100);
        for (auto& w : a.rwins) {
            if (e0 && fetch_slot(a, *e0, w)) ++e0_reached;
            if (t0m) {
                auto fs = fetch_slot(a, *t0m - 1, w);
                if (fs) {
                    LineId r = line_of(a, *fs);
                    if (r.status == 3) ++ev.gap_hits;
                    else { ++w256_checked; if (r.status == 0 && r.line == 256) ++w256_ok; }
                }
            }
        }
        // SPECIFICATION.md P2: WAIT(n,$60) for n = 1 .. $37 (PAL) or 1 .. 5 (NTSC); WAIT($00,$60) is measured apart (w0_waited)
        auto p2waited = waited_all(a, L.list, kListBaseW, [tag](const Instr& i) {
            return (i.h() == 0x40 && i.v() >= 0x30) || (i.h() == 0x60 && i.v() >= 1 && i.v() <= (tag == PAL ? 0x37 : 5));
        });
        // WAIT($00,$60), the one before m_256, waited (its delay exceeds lambda_min) in every field
        bool w0_waited = !a.rwins.empty(); size_t w0_checked = 0;
        for (auto& w : a.rwins) {
            auto lam0 = lambda_min(a, w);
            if (!t0m) { w0_waited = false; break; }
            auto dly0 = wait_delay(a, *t0m - 2, w);
            if (!dly0 || !lam0) { w0_waited = false; continue; }
            ++w0_checked;
            if (*dly0 <= *lam0) w0_waited = false;
        }

        // ---- preconditions ---------------------------------------------------
        Gate g = report_common(checks, a, tag, "A", 6, ev.gap_hits, ev.identified);
        auto not_graded = [&](const std::string& id, const std::string& why) {
            checks.record(id + " " + tg + " run A: NOT GRADED: precondition failed:" + why);
        };
        const bool anchor_ok = anchor_checked > 0 && anchor_bad == 0;
        if (g.trace()) checks.expect(anchor_ok, "PRE-ANCHOR " + tg + " run A: every m_v (v=$30..$FF) lies on line v (RF2), " +
                                     num((long long)(anchor_checked - anchor_bad)) + "/" + num((long long)anchor_checked) + " matched");
        else not_graded("PRE-ANCHOR", g.why_not_trace());
        if (g.fetch()) checks.expect(p2waited.all, "PRE-P2WAITED " + tg + " run A: every WAIT(v,$40) and every WAIT(n,$60) for n >= 1 waited (delay above lambda_min), " +
                                     num((long long)p2waited.checked) + "/" + num((long long)p2waited.expected) + " measured");
        else not_graded("PRE-P2WAITED", g.why_not_fetch());
        const bool e0_ok = e0 && !a.rwins.empty() && e0_reached == a.rwins.size();
        if (g.fetch()) checks.expect(e0_ok, "PRE-E0 " + tg + " run A: m_E0 (after WAIT($FF,$E0)) is fetched in every field (" +
                                     num((long long)e0_reached) + "/" + num((long long)a.rwins.size()) + ")");
        else not_graded("PRE-E0", g.why_not_fetch());
        const bool p7pre_ok = w256_checked > 0 && w256_ok == w256_checked && w256_checked == a.rwins.size();
        if (g.trace()) checks.expect(p7pre_ok, "PRE-P7L256 " + tg + " run A: the second word of the WAIT before m_256 is fetched on line 256 (RF2), " +
                                     num((long long)w256_ok) + "/" + num((long long)a.rwins.size()) + " fields");
        else not_graded("PRE-P7L256", g.why_not_trace());
        // L(256 + n) of P2 and P3 rests on m_256 lying on line 256 and on WAIT($00,$60) having waited
        const bool p2pre = anchor_ok && p2waited.all && p7pre_ok && w0_waited && w0_checked == a.rwins.size();
        const std::string p2pre_why = std::string(anchor_ok ? "" : " PRE-ANCHOR") + std::string(p2waited.all ? "" : " PRE-P2WAITED") +
                                      std::string(p7pre_ok ? "" : " PRE-P7L256") + std::string(w0_waited && w0_checked == a.rwins.size() ? "" : " P7-WAITED");
        const bool base_ok = g.core() && p2pre;

        // ---- P1: field length (NOT GRADED) and origin ----------------------------
        const std::string fl = setstr(field_lengths(a));
        checks.note("P1-COUNT " + tg + " run A: distinct V values per complete field {" + fl + "} over " + num((long long)a.fields.size()) + " complete fields");
        if (tag == PAL) {
            checks.record("P1-COUNT PAL NOT GRADED COP2-vertical-beam-position.c4 COP2-using-the-copper-in-interlaced-mode.c4: the pages give two "
                          "field lengths. p.24: \"There are actually 262 NTSC (312 PAL) possible vertical positions.\" p.34: \"PAL has 312 lines normally\". "
                          "p.27: \"to the end of the display (226,262) NTSC (or (226,312) PAL)\", positions counted from (0,0), which are 263 and 313. "
                          "p.216 Table 7-5: V8 \"Allows PAL line counts (313) to appear in PAL versions of the Amiga.\"");
        } else {
            checks.record("P1-COUNT NTSC NOT GRADED COP2-vertical-beam-position.c3 COP2-using-the-copper-in-interlaced-mode.c2: the pages give two "
                          "field lengths. p.24: \"There are actually 262 NTSC (312 PAL) possible vertical positions.\" p.34: \"a normal NTSC display has 262 lines\". "
                          "p.27: \"to the end of the display (226,262) NTSC (or (226,312) PAL)\", positions counted from (0,0), which are 263 and 313. "
                          "p.216 Table 7-5 gives PAL only: V8 \"Allows PAL line counts (313) to appear in PAL versions of the Amiga.\"");
        }
        {
            bool even = false, odd = false, h0 = false;
            for (auto& s : a.all) { if (s.H & 1) odd = true; else even = true; if (s.H == 0) h0 = true; }
            const bool cover = even && odd;
            bool first_v0 = !a.fields.empty();
            for (auto& f : a.fields) if (a.trusted[f.lo].V != 0) first_v0 = false;
            bool lowest_present = false;
            const int lowest = cover ? 0 : (odd ? 1 : 0);
            for (auto& s : a.all) if (s.H == lowest) lowest_present = true;
            checks.note("P1-ORIGIN " + tg + " run A: parity coverage " + std::string(cover ? "yes" : "no") + " (even " + std::string(even ? "seen" : "absent") +
                        ", odd " + std::string(odd ? "seen" : "absent") + ")");
            if (!g.core()) {
                checks.record("P1-ORIGIN " + tg + " NOT GRADED: precondition failed:" + g.why_not_core());
            } else if (cover || !odd) {
                checks.expect(first_v0 && (cover ? h0 : lowest_present),
                              "P1-ORIGIN " + tg + " [p.27]: every complete field's first trusted sample has V=0, and H=0 occurs in the trace");
            } else {
                checks.expect(first_v0 && lowest_present,
                              "P1-ORIGIN " + tg + " [p.27]: every complete field's first trusted sample has V=0, and the lowest H of the observed odd parity occurs in the trace");
            }
            checks.record("P1-ORIGIN " + tg + " COP2-putting-together-a-copper-instruction-list.c7 not asserted whole: p.27 starts the traversal at (0,0). "
                          "The P1-ORIGIN line grades two separate facts, that the first trusted sample of every complete field has V=0 and that H=0 (the lowest count of the parity read, when only odd counts are read) occurs somewhere in the trace; "
                          "no line grades V=0 and H=0 together at a field start, so the claim itself is neither established nor contradicted");
        }

        // ---- P2 -------------------------------------------------------------------
        P2Res p2 = grade_p2(a, L, tail_main);
        checks.record("P2 " + tg + " run A: L(v) values measured {" + setstr64(p2.values) + "} over " + num((long long)p2.measured) + " of " + num((long long)p2.expected) + " pairs");
        if (tag == PAL) {
            if (!base_ok || !p2.complete) {
                checks.record("P2-PAL NOT GRADED: precondition or population failed:" + g.why_not_core() +
                              p2pre_why + std::string(p2.complete ? "" : " population-incomplete"));
            } else {
                checks.expect(p2.all_227, "P2-PAL [p.23,p.24]: L(v)=227 for every measured v in every complete field");
            }
            checks.record("P2-PAL COP2-horizontal-beam-position.c15 not asserted whole: p.23 says every PAL line is 227 colour clocks long. "
                          "The P2-PAL line grades L(v) for v=$30 .. $FE and for the tail pairs of lines 256+n (the pairs counted on the P2 line above), in every complete field; "
                          "lines 0 .. $2F, line $FF, the lines at the end of the field after the tail pairs and the field end are not graded, so the claim for every line is neither established nor contradicted");
        } else {
            if (!base_ok || !p2.complete) {
                checks.record("P2-NTSC NOT GRADED COP2-horizontal-beam-position.c10 COP2-horizontal-beam-position.c11 COP2-horizontal-beam-position.c12 "
                              "COP2-vertical-beam-position.c11: precondition or population failed:" + g.why_not_core() +
                              p2pre_why + std::string(p2.complete ? "" : " population-incomplete"));
            } else {
                checks.expect(p2.in_set && p2.alt,
                              "P2-NTSC COP2-horizontal-beam-position.c10 COP2-horizontal-beam-position.c11 COP2-horizontal-beam-position.c12 "
                              "COP2-vertical-beam-position.c11 [p.23,p.24]: L(v) in {227,228} for every measured v, and "
                              "L(v)!=L(v+1) for every measured consecutive pair, in every complete field");
            }
            checks.record("P2-NTSC (half graded: COP2-horizontal-beam-position.c11 COP2-horizontal-beam-position.c12 COP2-vertical-beam-position.c11) a green P2-NTSC would establish the alternation of long and short lines and the 228-clock long lines "
                          "over the measured lines only, v=$30 .. $FE and the tail pairs of lines 256+n in every complete field (the pairs counted on the P2 line above): "
                          "lines 0 .. $2F, line $FF, the lines at the end of the field after the tail pairs, and the phase of the alternation across these gaps are not graded; "
                          "a witnessed pair of equal adjacent lines is a counterexample whatever the coverage");
            checks.record("P2-NTSC COP2-horizontal-beam-position.c14 not asserted: its clause (the remaining NTSC lines last 227 colour clocks) is about the short lines only; "
                          "P2-NTSC grades the alternation of long and short lines, which is where it is red, so the clause is neither established nor contradicted by that line; "
                          "the measured L(v) values are on the P2 line above");
        }

        // ---- P3 ---------------------------------------------------------------------
        {
            P3Res p3 = grade_p3(a, tag, p2);
            std::string hs; { int lo = 999, hi = -1; for (int h : p3.hs) { lo = std::min(lo, h); hi = std::max(hi, h); } hs = p3.hs.empty() ? "(none)" : hex2(lo) + ".." + hex2(hi) + ", " + num((long long)p3.hs.size()) + " distinct"; }
            std::string mo, me; { int c = 0; for (int x : p3.missing_odd) { if (c++ < 40) mo += " " + hex2(x); } c = 0; for (int x : p3.missing_even) { if (c++ < 40) me += " " + hex2(x); } }
            checks.note("P3 " + tg + " run A: H values observed " + hs + "; even values not observed (" + num((long long)p3.missing_even.size()) + "):" + (me.empty() ? std::string(" none") : me) +
                        "; odd values up to $E1 not observed (" + num((long long)p3.missing_odd.size()) + "):" + (mo.empty() ? std::string(" none") : mo) +
                        "; lines with H>=$E0 and L=228: " + num((long long)p3.lines_long) + ", with L=227: " + num((long long)p3.lines_short));
            const bool cov_ok = p3.lines_long >= 20 && p3.lines_short >= 20;
            size_t least = a.fields.empty() ? 0 : (size_t)-1;
            for (auto& f : a.fields) least = std::min(least, f.hi - f.lo);
            const bool p3s = a.fields.size() >= 6 && least >= 114;
            // PRE-P3SAMPLES rests on PRE-FIELDS (and so on PRE-T2)
            const bool p3s_graded = g.cap() && g.t2 && g.fields;
            if (p3s_graded) checks.expect(p3s, "PRE-P3SAMPLES " + tg + " run A: at least 6 complete fields, each with at least 114 trusted samples, the number of even counts $00 ... $E2 (p.23) (" +
                                          num((long long)a.fields.size()) + " fields, at least " + num((long long)least) + " trusted samples per field)");
            else not_graded("PRE-P3SAMPLES", g.why_not_cap() + std::string(g.t2 ? "" : " PRE-T2") + std::string(g.fields ? "" : " PRE-FIELDS"));
            if (tag == NTSC) {
                if (!(g.cap() && g.t2)) not_graded("PRE-P3ODDEND", g.why_not_cap() + std::string(g.t2 ? "" : " PRE-T2"));
                else if (p3.cover) checks.expect(p3.odd_end_ok, "PRE-P3ODDEND NTSC run A: the odd count $E1, next below the last even count, is read at the end of a line");
                else checks.record("PRE-P3ODDEND NTSC run A: not needed, only counts of one parity are read (" + std::string(p3.parity ? "odd" : "even") + ")");
                if (!(g.counts() && g.trace() && p2pre && p2.complete))
                    not_graded("PRE-P3COVER", union_why({g.why_not_counts(), g.why_not_trace(), p2pre_why, std::string(p2.complete ? "" : " population-incomplete")}));
                else if (p3.e3_graded) checks.expect(cov_ok, "PRE-P3COVER NTSC run A: H >= $E0 is observed on at least 20 distinct lines with L(v)=228 and at least 20 with L(v)=227 (" +
                                                     num((long long)p3.lines_long) + " and " + num((long long)p3.lines_short) + ")");
                else checks.record("PRE-P3COVER NTSC run A: not needed, $E3 cannot be read when only even counts are read");
            }
            if (!(g.counts() && p3s)) {
                checks.record("P3-" + tg + " NOT GRADED: precondition failed:" + g.why_not_counts() + std::string(p3s ? "" : " PRE-P3SAMPLES"));
            } else if (tag == PAL) {
                checks.expect(p3.set_ok, "P3-PAL [p.23,p.27]: every H is at most $E2, and every even value from $00 to $E2 is observed (when counts of one parity only are read: every value of that parity from the lowest to the highest not above $E2)");
            } else if (p3.cover && !p3.odd_end_ok) {
                checks.record("P3-NTSC-SET NOT GRADED: precondition failed: PRE-P3ODDEND");
            } else {
                checks.expect(p3.set_ok, "P3-NTSC-SET [p.23]: every H is at most $E3, and every even value from $00 to $E2 and the odd value $E3 are observed (when counts of one parity only are read: every value of that parity from the lowest to the highest not above $E3)");
                const bool pop_ok = p2.complete && p2pre;
                if (!p3.e3_graded) {
                    checks.record("P3-NTSC-LONGONLY NOT GRADED COP2-horizontal-beam-position.c13: only even counts are read, so $E3 cannot be read");
                } else if (!(cov_ok && pop_ok)) {
                    checks.record("P3-NTSC-LONGONLY NOT GRADED COP2-horizontal-beam-position.c13: precondition failed:" +
                                  std::string(cov_ok ? "" : " PRE-P3COVER") + std::string(pop_ok ? "" : " P2-population-or-anchor-or-waited"));
                } else {
                    checks.expect(p3.e3_ok, "P3-NTSC-LONGONLY [p.23]: no sample with H=$E3 lies on a line with L(v)=227");
                }
            }
            if (tag == NTSC) checks.record("P3-NTSC COP2-horizontal-beam-position.c13 not asserted whole: the sampler reads no odd count in part of the line (" + num((long long)p3.missing_odd.size()) +
                                           " odd values up to $E1 are never read), so \"0-$E3\" is graded over the even counts, the lowest count and $E3 only");
        }

        // ---- P7 -----------------------------------------------------------------------
        {
            const bool waited = w0_waited; const size_t wchecked = w0_checked;
            const bool pre = g.core() && anchor_ok && e0_ok && p7pre_ok;
            const bool pop = p7_identified == p7_expected;
            const std::string why = g.why_not_core() + std::string(anchor_ok ? "" : " PRE-ANCHOR") + std::string(e0_ok ? "" : " PRE-E0") + std::string(p7pre_ok ? "" : " PRE-P7L256");
            if (!pre || !pop) {
                checks.record("P7 " + tg + " NOT GRADED COP2-the-wait-instruction.c22 COP2-the-wait-instruction.c23 COP2-vertical-beam-position.c7: precondition or population failed:" + why +
                              std::string(pop ? "" : " population-incomplete (" + num((long long)p7_identified) + "/" + num((long long)p7_expected) + ")"));
                checks.record("P7-WAITED " + tg + " NOT GRADED: precondition failed:" + why);
            } else {
                checks.expect(p7_bad == 0, "P7 " + tg + " COP2-the-wait-instruction.c22 COP2-the-wait-instruction.c23 COP2-vertical-beam-position.c7 [p.23,p.24]: every m_(256+n) lies on line 256+n for n=0.." + num(p7_max));
                checks.expect(waited && wchecked == a.rwins.size(), "P7-WAITED " + tg + " [p.22]: WAIT($00,$60) waited (its delay exceeds lambda_min), " + num((long long)wchecked) + " fields");
            }
        }

        // ---- P8 (NTSC) --------------------------------------------------------------------
        if (tag == NTSC) {
            bool lines_ok = true; size_t lines_checked = 0;
            for (auto& w : a.rwins) {
                for (int n = 0; n <= 5; ++n) {
                    AEvents dummy;
                    LineId r = marker_line(a, L, (uint16_t)(0x100 + n), w, dummy);
                    ++lines_checked;
                    if (r.status != 0 || r.line != 256 + n) lines_ok = false;
                }
            }
            // measured: the line after line 261, and where m_262 is fetched against the start of line 0 of the next field
            std::map<int, int> after_v;
            size_t after_checked = 0;
            if (!a.fields.empty()) {
                for (size_t i = a.fields.front().lo; i + 1 < a.fields.back().hi && i + 1 < a.trusted.size(); ++i) {
                    if (a.trusted[i].V == 261 && a.trusted[i + 1].V != 261) { ++after_checked; after_v[a.trusted[i + 1].V]++; }
                }
            }
            std::map<std::string, size_t> m262;
            size_t m262_checked = 0;
            auto m261 = L.m(0x105); auto m262a = L.m(0x106);
            for (auto& w : a.rwins) {
                if (!m261) break;
                auto f261 = fetch_in(a.ridx, *m261, w.lo, w.hi);
                if (!f261) continue;
                ++m262_checked;
                auto l0 = line0_after(a, *f261, 0xE3);
                auto f262 = m262a ? fetch_in(a.ridx, *m262a, *f261, w.hi) : std::nullopt;
                std::string key;
                if (!f262) key = "m_262 not fetched before the next restart";
                else if (!l0) key = "m_262 fetched, the start of line 0 is not placed";
                else {
                    const int64_t s = slot_of(a.sref, *f262);
                    key = "m_262 fetched " + (s < l0->early ? slots(l0->early - s) + " before the earliest start of line 0" :
                                              s > l0->late ? slots(s - l0->late) + " after the latest start of line 0" : std::string("between the earliest and the latest start of line 0"));
                }
                ++m262[key];
            }
            std::string av; for (auto& kv : after_v) av += (av.empty() ? "" : ",") + std::to_string(kv.first) + "x" + std::to_string(kv.second);
            std::string mv262; for (auto& kv : m262) mv262 += " " + num((long long)kv.second) + " of " + num((long long)m262_checked) + " fields: " + kv.first + ";";
            checks.note("P8 " + tg + " run A: the trusted sample after the last sample of line 261 has V (count): " + (av.empty() ? std::string("(none)") : av) + " over " + num((long long)after_checked) + " field boundaries");
            checks.note("P8 " + tg + " run A: after m_261:" + (mv262.empty() ? std::string(" (none)") : mv262));
            const bool pre = g.core() && anchor_ok && e0_ok && p7pre_ok;
            const std::string why = g.why_not_core() + std::string(anchor_ok ? "" : " PRE-ANCHOR") + std::string(e0_ok ? "" : " PRE-E0") + std::string(p7pre_ok ? "" : " PRE-P7L256");
            if (!pre || lines_checked == 0) {
                checks.record("P8-LINES NTSC NOT GRADED: precondition or population failed:" + why);
            } else {
                checks.expect(lines_ok, "P8-LINES NTSC [p.24]: m_(256+n) lies on line 256+n for n=0..5, every complete field");
            }
            // the field-length dispute of P1-COUNT: the last line number and a line 262 are not graded
            const std::string dispute =
                "the pages give two field lengths. p.24: \"There are actually 262 NTSC (312 PAL) possible vertical positions.\" and, in the box, \"256 + 6 = 262\"; "
                "p.34: \"a normal NTSC display has 262 lines\"; against p.24: \"these last six or seven scan lines\", p.27: \"to the end of the display (226,262) NTSC "
                "(or (226,312) PAL)\" from \"(0,0)\", which are 263 and 313 positions, and p.100: \"line 0 to line 262\". p.216 Table 7-5 gives PAL only: V8 "
                "\"Allows PAL line counts (313) to appear in PAL versions of the Amiga.\"";
            checks.record("P8-AFTER NTSC NOT GRADED: whether line 0 follows line 261 depends on the field length; " + dispute);
            checks.record("P8-NO262 NTSC NOT GRADED: whether a line 262 exists depends on the field length; " + dispute);
            checks.record("P8 NTSC NOT GRADED COP2-vertical-beam-position.c9: \"covering the last 6 lines of the scan before vertical blanking occurs\" names the last six lines and so the field length; " + dispute);
        }

        if (normal) ocs_check(checks, dir, plan.bin[ti], tag, "A", a, bases, 0x0001, 0xFFFE, kRunBudgetA, tail_main);
    }
}

// ==== SECTION C: horizontal position sweep (P4) =========================================
struct CPlan { bool run[2] = {true, true}; std::string bin[2]; uint8_t cfg[2]; };

CPlan make_plan(const std::string& prefix, const std::string& mv, bool normal) {
    CPlan p;
    p.bin[PAL] = prefix; p.bin[NTSC] = prefix + "_ntsc";
    p.cfg[PAL] = kECS_PAL; p.cfg[NTSC] = kECS_NTSC;
    if (!normal) {
        p.run[PAL] = p.run[NTSC] = false;
        std::string rest = mv.substr(2);                  // "<VARIANT>-<PAL|NTSC>"
        size_t dash = rest.rfind('-');
        std::string var = rest.substr(0, dash), tg = rest.substr(dash + 1);
        for (auto& ch : var) ch = (char)std::tolower((unsigned char)ch);
        Tag t = tg == "NTSC" ? NTSC : PAL;
        p.run[t] = true;
        p.bin[t] = prefix + "_" + var + (t == NTSC ? "_ntsc" : "");
    }
    return p;
}

void section_C(Checks& checks, const std::string& dir, const std::string& mv, bool normal) {
    CPlan plan = make_plan("prog_c", mv, normal);
    const std::set<uint32_t> bases = {kListBaseW};
    for (int ti = 0; ti < 2; ++ti) {
        if (!plan.run[ti]) continue;
        const Tag tag = (Tag)ti;
        const std::string tg = tagname(tag);
        Analysis a = analyze(run_capture(dir + "/" + plan.bin[ti] + ".bin", plan.cfg[ti], kRunBudgetC), bases, 0x0001, 0xFFFE);
        ALists L; L.list = decode_list(a.mem, kListBaseW); L.mk = marker_map(L.list);

        // ---- events -----------------------------------------------------------------
        AEvents ev;
        size_t r1_checked = 0, r1_bad = 0;
        size_t a_expected = 0, a_measured = 0, a_bad = 0;
        size_t b_expected = 0, b_measured = 0;
        bool b_strict = true;
        size_t c_expected = 0, c_measured = 0, c_bad = 0;
        std::map<int, std::set<int64_t>> off_by_k;             // D_k - (h_k - $10), k = 105..113
        std::map<int, std::set<std::string>> cdet;             // j -> E_j and the line of r2_j
        for (auto& w : a.rwins) {
            std::map<int, int64_t> D;
            for (int k = 1; k <= 113; ++k) {
                auto mm = L.m((uint16_t)(0xC100 + k)), mr = L.m((uint16_t)(0xC200 + k));
                std::optional<int64_t> tm, tr;
                if (mm) tm = fetch_slot(a, *mm, w);
                if (mr) tr = fetch_slot(a, *mr, w);
                if (tm && tr) D[k] = *tm - *tr;
            }
            for (int k = 1; k <= 104; ++k) {
                ++a_expected;
                auto it = D.find(k);
                if (it == D.end()) continue;
                ++a_measured;
                const int h = 2 * k, href = h <= 0x80 ? 0xC0 : 0x10;
                if (it->second != h - href) ++a_bad;
            }
            for (int k = 105; k <= 113; ++k) {
                ++b_expected;
                auto it = D.find(k);
                if (it == D.end()) continue;
                ++b_measured;
                off_by_k[k].insert(it->second - (2 * k - 0x10));
                if (k > 105 && D.count(k - 1) && !(D[k - 1] < it->second)) b_strict = false;
            }
            for (int j = 0; j < 14; ++j) {
                auto m1 = L.m((uint16_t)(0xC300 + j)), mq = L.m((uint16_t)(0xC400 + j)), m2 = L.m((uint16_t)(0xC500 + j));
                if (m1) {
                    auto fs = fetch_slot(a, *m1, w);
                    if (fs) {
                        LineId r = line_of(a, *fs);
                        if (r.status == 3) ++ev.gap_hits; else { ++r1_checked; if (r.status == 0) ++ev.identified; if (r.status != 0 || r.line != 0xA4 + 2 * j) ++r1_bad; }
                    }
                }
                ++c_expected;
                if (!mq || !m2) continue;
                auto tq = fetch_slot(a, *mq, w), t2 = fetch_slot(a, *m2, w);
                if (!tq || !t2) continue;
                LineId r = line_of(a, *t2);
                if (r.status == 3) { ++ev.gap_hits; continue; }
                if (r.status == 0) ++ev.identified;
                ++c_measured;
                cdet[j].insert("E=" + num(*t2 - *tq) + " line " + (r.status == 0 ? num(r.line) : std::string("?")));
                auto lam2 = lambda_min(a, w);
                auto d2 = wait_delay(a, *m2 - 2, w);
                const bool r2waited = d2 && lam2 && *d2 > *lam2;
                if (r.status != 0 || r.line != 0xA4 + 2 * j + 1 || !r2waited) ++c_bad;
            }
        }
        // the WAITs under test: the first-word and second-word WAITs of each line (markers $C1xx and $C2xx) and WAIT(w_j,g_j) (marker $C4xx)
        std::unordered_map<uint32_t, uint16_t> mdata;
        for (auto& i : L.list) if (i.w1 == 0x01FE) mdata[i.a] = i.w2;
        auto waited = waited_all(a, L.list, kListBaseW, [&](const Instr& i) {
            auto it = mdata.find(i.a + 2);
            if (it == mdata.end()) return false;
            const int hi = it->second >> 8;
            return hi == 0xC1 || hi == 0xC2 || hi == 0xC4;
        });

        // ---- preconditions --------------------------------------------------------------
        Gate g = report_common(checks, a, tag, "C", 3, ev.gap_hits, ev.identified);
        const size_t r1_expected = a.rwins.size() * 14;
        const bool r1_ok = r1_checked == r1_expected && r1_expected > 0 && r1_bad == 0;
        if (g.trace()) checks.expect(r1_ok, "PRE-C-R1LINE " + tg + " run C: r1_j lies on line w_j for every j, every complete field (RF2), " +
                                     num((long long)(r1_checked - r1_bad)) + "/" + num((long long)r1_expected) + " matched");
        else checks.record("PRE-C-R1LINE " + tg + " run C: NOT GRADED: precondition failed:" + g.why_not_trace());
        if (g.fetch()) checks.expect(waited.all, "PRE-C-WAITED " + tg + " run C: every WAIT under test (the first-word and second-word WAITs of each line, and WAIT(w_j,g_j)) waited (delay above lambda_min), " +
                                     num((long long)waited.checked) + "/" + num((long long)waited.expected) + " measured");
        else checks.record("PRE-C-WAITED " + tg + " run C: NOT GRADED: precondition failed:" + g.why_not_fetch());
        const bool pre = g.core() && r1_ok && waited.all;
        const std::string why = g.why_not_core() + std::string(r1_ok ? "" : " PRE-C-R1LINE") + std::string(waited.all ? "" : " PRE-C-WAITED");

        // ---- P4 -------------------------------------------------------------------------------
        std::string offs;
        for (auto& kv : off_by_k) {
            offs += " k=" + num(kv.first) + ":";
            bool first = true;
            for (auto v : kv.second) { offs += (first ? "" : "/") + num(v); first = false; }
        }
        checks.note("P4 " + tg + " run C: D_k - (h_k - $10) for h_k = $D2..$E2 (reading A predicts 0 or 1, reading B -1 or 0):" + (offs.empty() ? std::string(" (none)") : offs));
        { std::string cd; for (auto& kv : cdet) { cd += " j=" + num(kv.first) + "(w=$" + hex2(0xA4 + 2 * kv.first) + ", expected line " + num(0xA4 + 2 * kv.first + 1) + "):"; bool f = true; for (auto& x : kv.second) { cd += (f ? "{" : "/") + x; f = false; } cd += "}"; }
          checks.note("P4-C " + tg + " run C: E_j and the line of r2_j per j:" + (cd.empty() ? std::string(" (none)") : cd)); }
        checks.record("P4-C " + tg + " run C: E_j = time(r2_j) - time(q_j) = $40 is not graded: where in the first counts of a line the Copper's next fetch falls is left open by the pages (p.20: the Copper's cycles do not collide with refresh, which occupies slots at the start of every line; SPECIFICATION.md 7, item 14); E_j is measured per j above");
        const bool pop = a_measured == a_expected && b_measured == b_expected && c_measured == c_expected && a_expected > 0;
        if (!pre || !pop) {
            checks.record("P4 " + tg + " NOT GRADED COP2-horizontal-beam-position.c3 COP2-vertical-beam-position.c23: precondition or population failed:" + why +
                          std::string(pop ? "" : " population-incomplete (a " + num((long long)a_measured) + "/" + num((long long)a_expected) + ", b " +
                                      num((long long)b_measured) + "/" + num((long long)b_expected) + ", c " + num((long long)c_measured) + "/" + num((long long)c_expected) + ")"));
        } else {
            const bool oka = a_bad == 0, okb = b_strict, okc = c_bad == 0;
            checks.expect(oka, "P4-A " + tg + " [p.22,p.23,p.24,p.275]: D_k = h_k - H_ref for every k with h_k <= $D0 (k=1..104), every complete field");
            checks.expect(okb, "P4-B " + tg + " [p.23,p.32]: D_k strictly increases with k for h_k = $D2..$E2 (k=105..113), every complete field");
            checks.expect(okc, "P4-C " + tg + " [p.23,p.30]: r2_j lies on line w_j+1 and its WAIT(w_j+1,$40) waited, so the WAIT for a position beyond $E2 on line w_j ended before count $40 of line w_j+1, for every j=0..13, every complete field");
            checks.expect(oka && okb && okc, "P4 " + tg + " COP2-horizontal-beam-position.c3 COP2-vertical-beam-position.c23 [p.23,p.24,p.275]: the Copper's horizontal "
                          "comparison advances one count per memory cycle, gives 113 positions $02..$E2, and has no position beyond $E2");
        }
        if (normal) ocs_check(checks, dir, plan.bin[ti], tag, "C", a, bases, 0x0001, 0xFFFE, kRunBudgetC);
    }
}

// ==== SECTION B: WAIT on line $FF beyond $E2 (P6) =====================================================
constexpr uint32_t kListInitW = kListBaseW;
constexpr uint32_t kListFEW   = 0x010200 / 2;
uint32_t list_b_base(int j) { return (0x010400 + 0x100 * j) / 2; }

void section_B(Checks& checks, const std::string& dir, const std::string& mv, bool normal) {
    CPlan plan = make_plan("prog_b", mv, normal);
    std::set<uint32_t> bases = {kListInitW, kListFEW};
    for (int j = 0; j < 16; ++j) bases.insert(list_b_base(j));
    for (int ti = 0; ti < 2; ++ti) {
        if (!plan.run[ti]) continue;
        const Tag tag = (Tag)ti;
        const std::string tg = tagname(tag);
        Analysis a = analyze(run_capture(dir + "/" + plan.bin[ti] + ".bin", plan.cfg[ti], kRunBudgetB), bases, 0x0080, 0x0001);
        std::map<uint32_t, ALists> lists;
        for (uint32_t b : bases) { ALists l; l.list = decode_list(a.mem, b); l.mk = marker_map(l.list); lists[b] = l; }

        // every Copper fetch in the list region, in time order
        std::vector<uint64_t> allf;
        for (auto& kv : a.ridx) for (auto c : kv.second) allf.push_back(c);
        std::sort(allf.begin(), allf.end());

        // window i runs the list L_FE (i = 0), L_0 (i = 1), ...; the chain the lists name
        std::vector<uint32_t> expect_base;
        expect_base.push_back(kListFEW);
        for (size_t i = 0; i < a.rwins.size(); ++i) {
            int j = (int)i - 1;
            if (j < 0) continue;
            expect_base.push_back(list_b_base(j > 15 ? 15 : j));
        }
        bool chain_ok = a.rwins.size() >= 19;
        for (size_t i = 0; i < a.rwins.size() && i < expect_base.size(); ++i) if (a.rwins[i].base != expect_base[i]) chain_ok = false;
        size_t l15_windows = 0;
        for (auto& w : a.rwins) if (w.base == list_b_base(15)) ++l15_windows;
        chain_ok = chain_ok && l15_windows >= 3;

        AEvents ev;
        size_t l255_expected = 0, l255_ok = 0;           // PRE-B-L255
        size_t a_expected = 0, a_checked = 0, a_bad = 0; // P6-LINE255
        size_t waited_expected = 0, waited_ok = 0;       // PRE-B-WAITED
        bool x0_reached = false, x0_seen = false;
        int64_t x0_delta = 0; int x0_line = -1;
        std::set<int64_t> x0_early, x0_late;                 // line 0 of the next field, in slots after a_0
        size_t l0_missing = 0;                               // fields in which the start of line 0 is not placed
        const int max_late = tag == PAL ? 0xE2 : 0xE3;
        bool larger_never = true; size_t larger_checked = 0, larger_expected = 0;
        bool fe_never = true; size_t fe_checked = 0, fe_windows = 0;
        std::string l255_detail;
        std::map<std::string, size_t> never_count;
        bool ctl_ok = false, ctl_seen = false; int64_t ctl_delta = 0;
        for (size_t wi = 0; wi < a.rwins.size(); ++wi) {
            const RWin& w = a.rwins[wi];
            const ALists& L = lists[w.base];
            int j = -1;
            if (w.base == kListFEW) j = -1;
            else for (int q = 0; q < 16; ++q) if (w.base == list_b_base(q)) j = q;
            if (w.base == kListFEW) {
                auto m0 = L.m(0xB300), m1 = L.m(0xB301);
                if (m0 && m1) {
                    auto t0 = fetch_slot(a, *m0, w), t1 = fetch_slot(a, *m1, w);
                    auto lamfe = lambda_min(a, w);
                    auto dfe = wait_delay(a, *m1 - 2, w);
                    ctl_seen = true;
                    if (t0 && t1) { ctl_delta = *t1 - *t0; ctl_ok = ctl_delta > 0 && dfe && lamfe && *dfe > *lamfe; }
                }
                continue;
            }
            if (j < 0) continue;
            const uint16_t da = (uint16_t)(0xB100 + (j == 15 ? 14 : j));        // L_15 is identical to L_14
            const uint16_t dx = (uint16_t)(0xB200 + (j == 15 ? 14 : j));
            auto ma = L.m(da), mx = L.m(dx);
            auto lam = lambda_min(a, w);
            ++l255_expected; ++a_expected; ++waited_expected;
            if (!ma || !mx) continue;
            auto tw = fetch_slot(a, *mx - 1, w);              // second word of the WAIT before x_j
            auto ta = fetch_slot(a, *ma, w);
            if (tw) {
                LineId r = line_of(a, *tw);
                if (r.status == 3) ++ev.gap_hits; else { if (r.status == 0) ++ev.identified; if (r.status == 0 && r.line == 255) ++l255_ok; }
                if (!(r.status == 0 && r.line == 255)) l255_detail += " window " + num((long long)wi) + " (L_" + num(j) + "): line " + (r.status == 0 ? num(r.line) : std::string("?(status ") + num(r.status) + ")");
            } else l255_detail += " window " + num((long long)wi) + " (L_" + num(j) + "): second word not fetched";
            if (ta) {
                LineId r = line_of(a, *ta);
                if (r.status == 3) ++ev.gap_hits; else { if (r.status == 0) ++ev.identified; ++a_checked; if (r.status != 0 || r.line != 255) ++a_bad; }
                auto d = wait_delay(a, *ma - 2, w);
                if (d && lam && *d > *lam) ++waited_ok;
            }
            // the fetch of x_j, and every Copper fetch after the second word of the WAIT and before the
            // earliest start of line 0 of the next field (SPECIFICATION.md P6: the Copper stops until then)
            auto fx = fetch_in(a.ridx, *mx, w.lo, w.hi);
            auto fs2 = fetch_in(a.ridx, *mx - 1, w.lo, w.hi);
            std::optional<Line0> l0;
            if (fs2) l0 = line0_after(a, *fs2, max_late);
            if (tw && !l0) ++l0_missing;
            const bool fx_before = fx && l0 && slot_of(a.sref, *fx) < l0->early;
            size_t after = 0;
            if (fs2 && l0)
                for (auto it = std::upper_bound(allf.begin(), allf.end(), *fs2); it != allf.end() && *it < w.hi; ++it)
                    if (slot_of(a.sref, *it) < l0->early) ++after;
            if (j == 0) {
                x0_seen = true;
                x0_reached = fx_before && ta && slot_of(a.sref, *fx) > *ta;
                if (fx && ta) { x0_delta = slot_of(a.sref, *fx) - *ta; auto lx = line_of(a, slot_of(a.sref, *fx)); x0_line = lx.status == 0 ? lx.line : -2; }
                if (l0 && ta) { x0_early.insert(l0->early - *ta); x0_late.insert(l0->late - *ta); }
            } else {
                ++larger_expected;
                if (tw) {
                    ++larger_checked;
                    if (fx_before || after > 0) larger_never = false;
                    std::string rel;
                    if (fx && l0) {
                        const int64_t s = slot_of(a.sref, *fx);
                        rel = s < l0->early ? slots(l0->early - s) + " before the earliest start of line 0" :
                              s > l0->late ? slots(s - l0->late) + " after the latest start of line 0" : std::string("between the earliest and the latest start of line 0");
                        rel += ", " + slots(slot_of(a.sref, w.hi) - s) + " before the next restart";
                    }
                    const std::string key = std::string(fx ? "x_j fetched " + (l0 ? rel : std::string("(the start of line 0 is not placed)")) : "x_j not fetched") +
                                            ", " + num((long long)after) + " Copper fetches between the second word of the WAIT and the earliest start of line 0";
                    ++never_count[key];
                }
            }
            if (j == 14 || j == 15) {
                ++fe_windows;
                if (tw) { ++fe_checked; if (fx_before || after > 0) fe_never = false; }
            }
        }

        // ---- preconditions -------------------------------------------------------------------------
        Gate g = report_common(checks, a, tag, "B", 19, ev.gap_hits, ev.identified);
        if (g.fetch()) checks.expect(chain_ok, "PRE-B-CHAIN " + tg + " run B: each restart fetches the list that the previous list named (L_FE, L_0 .. L_15, then L_15 again), at least 3 fields of L_15 (" +
                                     num((long long)a.rwins.size()) + " fields, " + num((long long)l15_windows) + " of L_15)");
        else checks.record("PRE-B-CHAIN " + tg + " run B: NOT GRADED: precondition failed:" + g.why_not_fetch());
        const bool l255 = l255_expected > 0 && l255_ok == l255_expected;
        if (g.trace()) checks.expect(l255, "PRE-B-L255 " + tg + " run B: the second word of each WAIT($FF,h_j) is fetched on line 255 (RF2), " + num((long long)l255_ok) + "/" + num((long long)l255_expected) + " fields");
        else checks.record("PRE-B-L255 " + tg + " run B: NOT GRADED: precondition failed:" + g.why_not_trace());
        if (!l255_detail.empty()) checks.note("PRE-B-L255 " + tg + " run B: fields not on line 255:" + l255_detail);
        const bool wt = waited_expected > 0 && waited_ok == waited_expected;
        if (g.fetch()) checks.expect(wt, "PRE-B-WAITED " + tg + " run B: every WAIT($FF,$40) waited (delay above lambda_min), " + num((long long)waited_ok) + "/" + num((long long)waited_expected) + " fields");
        else checks.record("PRE-B-WAITED " + tg + " run B: NOT GRADED: precondition failed:" + g.why_not_fetch());
        const bool pre = g.core() && chain_ok && l255 && wt;
        const std::string why = g.why_not_core() + std::string(chain_ok ? "" : " PRE-B-CHAIN") + std::string(l255 ? "" : " PRE-B-L255") + std::string(wt ? "" : " PRE-B-WAITED");

        // ---- P6 ----------------------------------------------------------------------------------------
        std::string rec = "j=0: x_0 " + std::string(x0_reached ? "fetched" : "not fetched") + " before line 0 of the next field";
        if (x0_seen && x0_delta != 0) rec += ", time(x_0)-time(a_0) = " + num(x0_delta) + " slots, " + (x0_line >= 0 ? "on line " + num(x0_line) : std::string("on a line the trace does not identify")) +
                                             "; line 0 of the next field starts " + setstr64(x0_early) + " (earliest) to " + setstr64(x0_late) + " (latest) slots after a_0" +
                                             " (reading A predicts time(x_0)-time(a_0) = $A2 or $A3 = 162 or 163, reading B $A1 or $A2 = 161 or 162, p.23 and P4(b))";
        std::string nd;
        for (auto& kv : never_count) nd += " " + num((long long)kv.second) + " of " + num((long long)larger_checked) + " fields: " + kv.first + ";";
        checks.note("P6 " + tg + " run B: " + rec + "; fields of h_j > $E2:" + (nd.empty() ? std::string(" none") : nd));
        if (!pre || a_checked != a_expected || l0_missing > 0) {
            checks.record("P6 " + tg + " NOT GRADED COP2-the-wait-instruction.c26 COP2-the-wait-instruction.c27 COP2-the-wait-instruction.c28 COP2-the-wait-instruction.c29: precondition or population failed:" + why +
                          std::string(a_checked == a_expected ? "" : " population-incomplete") + std::string(l0_missing == 0 ? "" : " start-of-line-0-not-placed"));
        } else {
            checks.expect(a_bad == 0, "P6-LINE255 " + tg + " COP2-the-wait-instruction.c27 [p.23]: a_j lies on line 255 in every field of every list L_0 .. L_15");
            checks.expect(x0_seen && x0_reached && larger_checked == larger_expected && larger_never,
                          "P6-LARGEST " + tg + " COP2-the-wait-instruction.c26 [p.23,p.32,p.33,p.219]: $FFE2 is reached (x_0 is fetched after a_0 and before line 0 of the next field) and no larger value is (x_j is not fetched before line 0 of the next field for h_j = $E4 .. $FE)");
            checks.expect(fe_never && fe_windows >= 4 && fe_checked == fe_windows,
                          "P6-NEVER " + tg + " COP2-the-wait-instruction.c28 COP2-the-wait-instruction.c29 [p.23,p.22,p.219]: for $FFFE, x_j is never fetched and no Copper fetch occurs between the second word of the WAIT and the start of line 0 of the next field, in every one of the at least 4 fields with h_j = $FE");
            if (ctl_seen) checks.expect(ctl_ok, "P6-CONTROL " + tg + " [p.23]: WAIT($FE,$FE) ends when line $FF begins: $B300 is fetched in the same field and the WAIT($FF,$40) after it waited (time($B301)-time($B300) = " + num(ctl_delta) + " slots; $40 is not graded, for the reason of P4-C)");
            else checks.record("P6-CONTROL " + tg + " NOT GRADED: the L_FE field was not measured");
        }
        if (normal) ocs_check(checks, dir, plan.bin[ti], tag, "B", a, bases, 0x0080, 0x0001, kRunBudgetB);
    }
}

// ==== SECTIONS D and E: video (P5, P9) ==========================================================
struct VRun { int col; int len; };

// RF4: the runs of one output line, taken cyclically (a run touching both the start and the end counts once)
std::vector<VRun> line_runs(const std::vector<nmh::VideoSample>& v, nmh::Span ln) {
    std::vector<VRun> r;
    for (size_t i = ln.begin; i < ln.end && i < v.size(); ++i) {
        int c = (v[i].r << 8) | (v[i].g << 4) | v[i].b;
        if (!r.empty() && r.back().col == c) ++r.back().len; else r.push_back({c, 1});
    }
    if (r.size() > 1 && r.front().col == r.back().col) { r.front().len += r.back().len; r.pop_back(); }
    return r;
}

struct VideoSet {
    std::vector<std::vector<std::vector<VRun>>> frames;   // [complete frame][output line] runs
    size_t complete = 0;
    bool r1_ok = true; size_t r1_bad = 0;
};

// complete output frames after the first restart; R1 per frame
VideoSet make_video_set(const Analysis& a) {
    VideoSet vs;
    const auto& v = a.cap.video;
    auto frames = nmh::find_frames(v);
    auto lines = nmh::find_lines(v);
    if (a.restarts.size() < 2) { vs.r1_ok = false; return vs; }
    const uint64_t first = a.restarts[1].cycle;
    size_t li = 0;
    for (size_t f = 0; f + 1 < frames.size(); ++f) {
        if (frames[f].begin < first) continue;
        size_t cnt = 0;
        for (size_t ri = 1; ri < a.restarts.size(); ++ri) if (a.restarts[ri].cycle >= frames[f].begin && a.restarts[ri].cycle < frames[f].end) ++cnt;
        if (cnt != 1) { vs.r1_ok = false; ++vs.r1_bad; }
        std::vector<std::vector<VRun>> fl;
        while (li < lines.size() && lines[li].begin < frames[f].begin) ++li;
        for (size_t k = li; k < lines.size() && lines[k].end <= frames[f].end; ++k) fl.push_back(line_runs(v, lines[k]));
        vs.frames.push_back(std::move(fl));
    }
    vs.complete = vs.frames.size();
    if (vs.complete == 0) vs.r1_ok = false;
    return vs;
}

// cap(): the capture is usable (everything but the chip identification); ok(): and it is the standard asked for
struct VGate { bool fix = false, done = false, s1 = false, l1 = false, r1 = false, fields = false, stdid = false;
    bool cap() const { return fix && done && s1 && l1 && r1 && fields; }
    bool ok() const { return cap() && stdid; }
    std::string why_not_done() const {
        std::string s;
        if (!fix) s += " FIX";
        if (!done) s += " PRE-DONE";
        return s;
    }
    std::string why_not_cap() const {
        std::string s = why_not_done();
        if (!s1) s += " PRE-S1";
        if (!l1) s += " PRE-L1";
        if (!r1) s += " PRE-R1";
        if (!fields) s += " PRE-FIELDS";
        return s;
    }
    std::string why() const {
        std::string s = why_not_cap();
        if (!stdid) s += " PRE-STD";
        return s;
    }
};

int results_id(const Analysis& a) {
    for (auto& x : a.cap.log) if (x.is_write && x.word_addr == kResultsW) return (x.data >> 8) & 0x7F;
    return -1;
}

VGate report_common_video(Checks& checks, const Analysis& a, const VideoSet& vs, Tag tag, const std::string& run) {
    VGate g;
    const std::string tg = tagname(tag);
    const std::string rn = " run " + run + ": ";
    auto not_graded = [&](const std::string& id, const std::string& why) {
        checks.record(id + " " + tg + rn + "NOT GRADED: precondition failed:" + why);
    };
    const bool load = a.cap.loaded && a.cap.released;
    checks.expect(load, "FIX-LOAD " + tg + rn + "program loaded and CPU released [not graded -- apparatus check, no claim uid]");
    if (load) checks.expect(a.fixture, "FIX-CONST " + tg + rn + "the known word $A5C3 stored by the program to $0C0002 appears on the Chip-memory port with that data [not graded -- apparatus check, no claim uid]");
    else not_graded("FIX-CONST", " FIX-LOAD");
    g.fix = load && a.fixture;
    g.done = a.cap.done.has_value();
    if (g.fix) checks.expect(g.done, "PRE-DONE " + tg + rn + "the completion word was written");
    else not_graded("PRE-DONE", g.why_not_done());
    if (g.fix) checks.expect(a.s1_spacing, "PRE-S1A " + tg + rn + "refresh spacing 2S is constant through the run (RF1)");
    else not_graded("PRE-S1A", g.why_not_done());
    checks.note("S1 " + tg + rn + "S=" + num(a.sref.S) + " clk_sys cycles/slot, " + num((long long)a.sref.groups_found) + " refresh groups measured");
    if (g.fix && a.s1_spacing) checks.expect(a.s1_collision, "PRE-S1B " + tg + rn + "no two logged accesses occupy the same slot (RF1)");
    else not_graded("PRE-S1B", g.why_not_done() + std::string(a.s1_spacing ? "" : " PRE-S1A"));
    g.s1 = g.fix && a.s1_spacing && a.s1_collision;
    g.l1 = g.fix && a.l1;
    if (g.fix) checks.expect(a.l1, "PRE-L1 " + tg + rn + "the Copper's first fetches read the words the CPU wrote in step 3");
    else not_graded("PRE-L1", g.why_not_done());
    if (g.fix && g.done) checks.expect(vs.r1_ok, "PRE-R1 " + tg + rn + "exactly one restart between consecutive field boundaries (RF3), " + num((long long)vs.r1_bad) + " of " + num((long long)vs.complete) + " output fields without");
    else not_graded("PRE-R1", g.why_not_done());
    g.r1 = g.fix && g.done && vs.r1_ok;
    const bool fields = vs.complete >= 3;
    if (g.fix && g.done) checks.expect(fields, "PRE-FIELDS " + tg + rn + "at least 3 complete fields traced (measured " + num((long long)vs.complete) + ")");
    else not_graded("PRE-FIELDS", g.why_not_done());
    g.fields = g.fix && g.done && fields;
    const int id = results_id(a);
    const bool std_ok = std_of_id(id) == (int)tag;
    if (g.fix && g.done) checks.expect(std_ok, "PRE-STD " + tg + rn + "the chip identification in VPOSR bits 14-8 names " + tg + " (p.299), measured $" + hex2(id < 0 ? 0xFF : id));
    else not_graded("PRE-STD", g.why_not_done());
    g.stdid = g.fix && g.done && std_ok;
    return g;
}

constexpr int kWhite = 0xFFF, kBlue = 0x00F, kYellow = 0xFF0, kGreen = 0x0F0;

struct Calib { bool valid = false; size_t lines = 0, bad = 0; std::set<int> lens; int plen = 0; };

// D-lo / D-hi: every output line that shows COLOR01 has nwhite COLOR01 runs, and all of them and the nwhite-1 COLOR00 runs
// between them have one common length. P is that length, common to all lines.
Calib calibrate(const VideoSet& vs, int white, int blue, size_t nwhite) {
    Calib c;
    for (auto& fr : vs.frames) for (auto& runs : fr) {
        size_t first = runs.size(), last = 0, nw = 0;
        for (size_t i = 0; i < runs.size(); ++i) if (runs[i].col == white) { if (first == runs.size()) first = i; last = i; ++nw; }
        if (nw == 0) continue;
        ++c.lines;
        bool ok = nw == nwhite && (last - first + 1) == 2 * nwhite - 1;
        int len = runs[first].len;
        for (size_t i = first; ok && i <= last; ++i) {
            const bool want_white = ((i - first) % 2) == 0;
            if (runs[i].col != (want_white ? white : blue)) ok = false;
            if (runs[i].len != len) ok = false;
        }
        if (!ok) ++c.bad; else c.lens.insert(len);
    }
    c.valid = c.lines > 0 && c.bad == 0 && c.lens.size() == 1;
    if (c.valid) c.plen = *c.lens.begin();
    return c;
}

struct SweepRes {
    bool edges_ok = true; size_t lines_with_a = 0, bad_lines = 0;
    std::set<int> present_all;                        // u present in every complete frame
    std::map<int, std::vector<int>> widths;           // u -> widths found
    size_t frames = 0;
};

SweepRes analyze_sweep(const VideoSet& vs) {
    SweepRes r;
    r.frames = vs.complete;
    std::map<int, size_t> frames_with;
    for (auto& fr : vs.frames) {
        std::set<int> seen;
        for (auto& runs : fr) {
            std::vector<size_t> ai;
            for (size_t i = 0; i < runs.size(); ++i) if ((runs[i].col >> 8) == 0xF && ((runs[i].col >> 4) & 0xF) <= 8) ai.push_back(i);
            if (ai.empty()) continue;
            ++r.lines_with_a;
            if (ai.size() != 1) { r.edges_ok = false; ++r.bad_lines; continue; }
            size_t i = ai[0];
            const int u = 0x40 + (runs[i].col & 0xFF);
            const bool nb = i > 0 && i + 1 < runs.size() && runs[i - 1].col == kGreen && runs[i + 1].col == kGreen;
            if (!nb) { r.edges_ok = false; ++r.bad_lines; continue; }
            seen.insert(u);
            r.widths[u].push_back(runs[i].len);
        }
        for (int u : seen) ++frames_with[u];
    }
    for (int u = 0x40; u <= 0xCB; ++u) if (frames_with[u] == vs.complete && vs.complete > 0) r.present_all.insert(u); else r.edges_ok = false;
    return r;
}

int sweep_n(int u) { return u <= 0x85 ? u - 0x38 : 77 - (u - 0x86); }

// one standard-screen output frame set: window lines (those that show COLOR01)
struct EGrade { size_t window_lines = 0; size_t bad_a = 0, bad_b = 0, bad_one0 = 0, windows_per_frame_min = 0; bool frames_have_lines = true; std::set<int> c1_lens; };

EGrade grade_e(const VideoSet& vs, int c1, int c0, int plo) {
    EGrade e;
    e.windows_per_frame_min = (size_t)-1;
    for (auto& fr : vs.frames) {
        size_t wl = 0;
        for (auto& runs : fr) {
            size_t n1 = 0, iw = 0;
            for (size_t i = 0; i < runs.size(); ++i) if (runs[i].col == c1) { ++n1; iw = i; }
            if (n1 == 0) continue;
            ++wl; ++e.window_lines;
            // (a): exactly one COLOR01 run of 320 * P_lo; every other run COLOR00 or 0
            bool a_ok = n1 == 1 && runs[iw].len == 320 * plo;
            e.c1_lens.insert(runs[iw].len);
            for (auto& r : runs) if (r.col != c1 && r.col != c0 && r.col != 0) a_ok = false;
            if (!a_ok) ++e.bad_a;
            // exactly one run of 0
            size_t nz = 0; for (auto& r : runs) if (r.col == 0) ++nz;
            if (nz != 1) ++e.bad_one0;
            // (b): after the COLOR01 run, cyclically: COLOR00, 0, COLOR00 and nothing else
            bool b_ok = false;
            if (n1 == 1 && runs.size() == 4 && c0 != 0) {
                const int a1 = runs[(iw + 1) % 4].col, a2 = runs[(iw + 2) % 4].col, a3 = runs[(iw + 3) % 4].col;
                b_ok = a1 == c0 && a2 == 0 && a3 == c0;
            }
            if (!b_ok) ++e.bad_b;
        }
        e.windows_per_frame_min = std::min(e.windows_per_frame_min, wl);
    }
    if (vs.frames.empty()) e.windows_per_frame_min = 0;
    return e;
}

struct DPlan { bool lo = true, hi = true, sweep = true, e = true, ew = true;
    std::string b_lo = "prog_dlo", b_hi = "prog_dhi", b_sw = "prog_dsweep", b_e = "prog_e", b_ew = "prog_ewidefetch"; };

struct DEData { Analysis a; VideoSet vs; };

DEData load_de(const std::string& dir, const std::string& bin, Tag tag, bool sweep, uint16_t w0, uint16_t w1) {
    DEData d;
    const std::set<uint32_t> bases = {kListBaseW};
    d.a = analyze(run_capture(dir + "/" + bin + (tag == NTSC ? "_ntsc" : "") + ".bin", cfg_ecs(tag), kRunBudgetDE, true), bases, w0, w1);
    (void)sweep;
    d.vs = make_video_set(d.a);
    return d;
}

void section_DE(Checks& checks, const std::string& dir, const std::string& mv, bool normal) {
    DPlan plan;
    Tag only = PAL; bool one_tag = false;
    if (!normal) {
        plan.lo = plan.hi = plan.sweep = plan.e = plan.ew = false;
        std::string rest = mv.substr(2);                  // "<VARIANT>-<PAL|NTSC>"
        size_t dash = rest.rfind('-');
        std::string var = rest.substr(0, dash), tg = rest.substr(dash + 1);
        for (auto& ch : var) ch = (char)std::tolower((unsigned char)ch);
        only = tg == "NTSC" ? NTSC : PAL; one_tag = true;
        if (var == "dshift" || var == "ddrop" || var == "dlate") { plan.lo = plan.hi = plan.sweep = true; plan.b_sw = "prog_" + var; }
        else if (var == "dflatlo") { plan.lo = plan.hi = plan.sweep = plan.e = plan.ew = true; plan.b_lo = "prog_dflatlo"; }
        else if (var == "dflathi") { plan.hi = true; plan.b_hi = "prog_dflathi"; }
        else if (var == "ewidewin" || var == "ebrdblnk" || var == "eblackgap" || var == "enobpl") { plan.lo = plan.e = plan.ew = true; plan.b_e = "prog_" + var; }
        else if (var == "ewwidewin") { plan.lo = plan.e = plan.ew = true; plan.b_ew = "prog_ewidewin"; }
        else { std::printf("unknown variant %s\n", var.c_str()); std::exit(2); }
    }
    for (int ti = 0; ti < 2; ++ti) {
        if (one_tag && (Tag)ti != only) continue;
        const Tag tag = (Tag)ti;
        const std::string tg = tagname(tag);
        Calib clo, chi; bool have_lo = false, have_hi = false, lo_cap = false;
        if (plan.lo) {
            DEData d = load_de(dir, plan.b_lo, tag, false, 0x00E0, 0x0002);
            VGate g = report_common_video(checks, d.a, d.vs, tag, "D-lo");
            clo = calibrate(d.vs, kWhite, kBlue, 160); have_lo = true; lo_cap = g.cap();
            checks.note("P5 " + tg + " D-lo: " + num((long long)clo.lines) + " window lines, " + num((long long)clo.bad) + " without 160 COLOR01 runs of one common length with the COLOR00 runs between; lengths {" + setstr(clo.lens) + "}");
            if (g.cap()) checks.expect(clo.valid, "PRE-P5-CALLO " + tg + " D-lo: every window line shows 160 COLOR01 runs, and all COLOR01 runs and the 159 COLOR00 runs between them have one common length P_lo (" + num(clo.plen) + ")");
            else checks.record("PRE-P5-CALLO " + tg + " D-lo: NOT GRADED: precondition failed:" + g.why_not_cap());
        }
        if (plan.hi) {
            DEData d = load_de(dir, plan.b_hi, tag, false, 0x00E0, 0x0002);
            VGate g = report_common_video(checks, d.a, d.vs, tag, "D-hi");
            chi = calibrate(d.vs, kWhite, kBlue, 320); have_hi = true;
            checks.note("P5 " + tg + " D-hi: " + num((long long)chi.lines) + " window lines, " + num((long long)chi.bad) + " without 320 COLOR01 runs of one common length with the COLOR00 runs between; lengths {" + setstr(chi.lens) + "}");
            if (g.cap()) checks.expect(chi.valid, "PRE-P5-CALHI " + tg + " D-hi: every window line shows 320 COLOR01 runs, and all COLOR01 runs and the 319 COLOR00 runs between them have one common length P_hi (" + num(chi.plen) + ")");
            else checks.record("PRE-P5-CALHI " + tg + " D-hi: NOT GRADED: precondition failed:" + g.why_not_cap());
        }
        if (plan.sweep) {
            DEData d = load_de(dir, plan.b_sw, tag, true, 0x0001, 0xFFFE);
            VGate g = report_common_video(checks, d.a, d.vs, tag, "D-sweep");
            std::vector<Instr> list = decode_list(d.a.mem, kListBaseW);
            auto waited = waited_all(d.a, list, kListBaseW, [](const Instr&) { return true; });
            SweepRes sw = analyze_sweep(d.vs);
            if (g.cap()) checks.expect(sw.edges_ok, "PRE-P5-EDGES " + tg + " D-sweep: A_u appears for every sweep line u in every complete field, and every output line that shows A_u shows exactly one run of it with COLOR00 ($0F0) before and after it (" +
                                       num((long long)sw.bad_lines) + " of " + num((long long)sw.lines_with_a) + " output lines not so, " + num((long long)sw.present_all.size()) + " of 140 sweep lines present in every field)");
            else checks.record("PRE-P5-EDGES " + tg + " D-sweep: NOT GRADED: precondition failed:" + g.why_not_cap());
            if (g.cap()) checks.expect(waited.all, "PRE-P5-WAITED " + tg + " D-sweep: every sweep WAIT waited (delay above lambda_min), " + num((long long)waited.checked) + "/" + num((long long)waited.expected) + " measured");
            else checks.record("PRE-P5-WAITED " + tg + " D-sweep: NOT GRADED: precondition failed:" + g.why_not_cap());
            const bool pre = g.ok() && sw.edges_ok && waited.all;
            const std::string why = g.why() + std::string(sw.edges_ok ? "" : " PRE-P5-EDGES") + std::string(waited.all ? "" : " PRE-P5-WAITED");
            std::string lo_w, hi_w;
            if (!pre) {
                checks.record("P5 " + tg + " NOT GRADED COP2-horizontal-beam-position.c4 ch2-horizontal-beam-position.c4 COP2-horizontal-beam-position.c5 ch2-horizontal-beam-position.c5: precondition failed:" + why);
            } else {
                size_t lo_bad = 0, hi_bad = 0, total = 0;
                std::set<int> wset;
                for (auto& kv : sw.widths) for (int wv : kv.second) {
                    ++total; wset.insert(wv);
                    if (have_lo && clo.valid && wv != 4 * sweep_n(kv.first) * clo.plen) ++lo_bad;
                    if (have_hi && chi.valid && wv != 8 * sweep_n(kv.first) * chi.plen) ++hi_bad;
                }
                checks.note("P5 " + tg + " D-sweep: " + num((long long)total) + " A_u runs measured; P_lo=" + num(have_lo ? clo.plen : -1) + ", P_hi=" + num(have_hi ? chi.plen : -1));
                if (have_lo && clo.valid) {
                    checks.expect(lo_bad == 0, "P5-LO " + tg + " COP2-horizontal-beam-position.c4 ch2-horizontal-beam-position.c4 [p.23]: w_u = 4 * n_u * P_lo for every sweep line u, every output line, every complete field (" + num((long long)lo_bad) + " of " + num((long long)total) + " differ)");
                } else checks.record("P5-LO " + tg + " NOT GRADED COP2-horizontal-beam-position.c4 ch2-horizontal-beam-position.c4: P_lo is not valid (PRE-P5-CALLO)");
                if (have_hi && chi.valid) {
                    checks.expect(hi_bad == 0, "P5-HI " + tg + " COP2-horizontal-beam-position.c5 ch2-horizontal-beam-position.c5 [p.23]: w_u = 8 * n_u * P_hi for every sweep line u, every output line, every complete field (" + num((long long)hi_bad) + " of " + num((long long)total) + " differ)");
                } else checks.record("P5-HI " + tg + " NOT GRADED COP2-horizontal-beam-position.c5 ch2-horizontal-beam-position.c5: P_hi is not valid (PRE-P5-CALHI)");
            }
        }
        if (plan.e) {
            DEData d = load_de(dir, plan.b_e, tag, false, 0x00E0, 0x0002);
            VGate g = report_common_video(checks, d.a, d.vs, tag, "E");
            const int c0 = kBlue;
            const int plo = have_lo && clo.valid ? clo.plen : 0;
            EGrade e = grade_e(d.vs, kYellow, c0, plo);
            checks.note("P9 " + tg + " E: " + num((long long)e.window_lines) + " window lines, COLOR01 run lengths {" + setstr(e.c1_lens) + "}, P_lo=" + num(plo));
            const bool plo_ok = have_lo && lo_cap && clo.valid;     // the verdict of PRE-P5-CALLO in the D-lo capture
            const bool lines_ok = e.window_lines > 0 && e.windows_per_frame_min >= 1;
            if (g.cap()) checks.expect(lines_ok, "PRE-P9-LINES " + tg + " E: every complete field has at least one window line (" + num((long long)e.window_lines) + " window lines, at least " + num((long long)(e.window_lines ? e.windows_per_frame_min : 0)) + " per field)");
            else checks.record("PRE-P9-LINES " + tg + " E: NOT GRADED: precondition failed:" + g.why_not_cap());
            // the window lines the line below grades exist only when PRE-P9-LINES holds
            const bool one0 = e.window_lines > 0 && e.bad_one0 == 0;
            if (g.cap() && lines_ok) checks.expect(one0, "PRE-P9-ONE0 " + tg + " E: each window line has exactly one run of pixels of value 0 (" + num((long long)e.bad_one0) + " of " + num((long long)e.window_lines) + " do not)");
            else checks.record("PRE-P9-ONE0 " + tg + " E: NOT GRADED: precondition failed:" + g.why_not_cap() + std::string(lines_ok ? "" : " PRE-P9-LINES"));
            const bool pre = g.ok() && plo_ok && lines_ok && one0;
            const std::string why = g.why() + std::string(plo_ok ? "" : " PRE-P5-CALLO") + std::string(lines_ok ? "" : " PRE-P9-LINES") + std::string(one0 ? "" : " PRE-P9-ONE0");
            EGrade ew; bool have_ew = false; bool pre_ew = false;
            if (plan.ew) {
                DEData dw = load_de(dir, plan.b_ew, tag, false, 0x00E0, 0x0002);
                VGate gw = report_common_video(checks, dw.a, dw.vs, tag, "E-widefetch");
                ew = grade_e(dw.vs, kYellow, kBlue, plo); have_ew = true;
                const bool wl = ew.window_lines > 0 && ew.bad_one0 == 0 && ew.windows_per_frame_min >= 1;
                if (gw.cap()) checks.expect(wl, "PRE-P9-ONE0 " + tg + " E-widefetch: each window line has exactly one run of pixels of value 0 (" + num((long long)ew.bad_one0) + " of " + num((long long)ew.window_lines) + " do not)");
                else checks.record("PRE-P9-ONE0 " + tg + " E-widefetch: NOT GRADED: precondition failed:" + gw.why_not_cap());
                pre_ew = gw.ok() && wl;
            }
            if (!pre || (plan.ew && !pre_ew)) {
                checks.record("P9 " + tg + " NOT GRADED COP2-horizontal-beam-position.c7 COP2-horizontal-beam-position.c9 ch2-horizontal-beam-position.c8: precondition failed:" + why + std::string(plan.ew && !pre_ew ? " E-widefetch-preconditions" : ""));
            } else {
                checks.expect(e.bad_a == 0, "P9-A-E " + tg + " [p.23,p.57,p.59]: every window line of run E is one run of COLOR01 of length 320 * P_lo, and otherwise only COLOR00 and 0 pixels (" + num((long long)e.bad_a) + " of " + num((long long)e.window_lines) + " lines differ)");
                if (have_ew) checks.expect(ew.bad_a == 0, "P9-A-EW " + tg + " [p.23,p.57,p.59]: every window line of E-widefetch is one run of COLOR01 of length 320 * P_lo, and otherwise only COLOR00 and 0 pixels (" + num((long long)ew.bad_a) + " of " + num((long long)ew.window_lines) + " lines differ)");
                checks.expect(e.bad_b == 0, "P9-B " + tg + " [p.23]: going cyclically from the end of the COLOR01 run, a window line shows COLOR00, one run of 0, COLOR00 and nothing else (" + num((long long)e.bad_b) + " of " + num((long long)e.window_lines) + " lines differ)");
                if (have_ew) checks.expect(e.bad_a == 0 && ew.bad_a == 0 && e.bad_b == 0,
                              "P9-A " + tg + " COP2-horizontal-beam-position.c7 COP2-horizontal-beam-position.c9 ch2-horizontal-beam-position.c8 [p.23,p.57,p.59]: the standard screen is 320 pixels wide and the rest of each of its lines shows only the background colour, in the standard stimulus and in E-widefetch");
            }
        }
    }
}


}  // namespace

// The claims SPECIFICATION.md section 1 does not turn into a proposition, declined with their reasons.
void record_declined(Checks& checks) {
    checks.record("DECLINED ch2-horizontal-beam-position.c14 COP2-horizontal-beam-position.c16 [p.23]: the display's line length of 227 1/2 colour clocks reaches the test only through a line "
                  "doubler, and the sentence is unclear for PAL (SPECIFICATION.md section 1 and 7 item 6)");
    checks.record("DECLINED ch2-vertical-beam-position.c21 COP2-vertical-beam-position.c24 [p.24]: \"processor is twice this\": the processor clock is internal to the CPU and no page "
                  "converts a bus access into processor clock cycles (SPECIFICATION.md section 1)");
    checks.record("DECLINED COP2-copins-timing.c6 [p.275]: a duration in nanoseconds is not observable, the harness states no clock period (SPECIFICATION.md section 1)");
    checks.record("DECLINED COP2-putting-together-a-copper-instruction-list.c9 COP2-putting-together-a-copper-instruction-list.c10 [p.27]: the end-of-display coordinate (226,262) NTSC and (226,312) PAL: "
                  "counted from line 0 the last line is 261 or 311 by p.24 and p.34, and no page says whether an NTSC field ends on a long line (SPECIFICATION.md section 1 and 7 item 1)");
    checks.record("DECLINED COP2-horizontal-beam-position.c8 ch2-horizontal-beam-position.c7 [p.23]: the unused portion $04 to $47 is a pair of counter positions, and no page gives the delay "
                  "from a counter value to a displayed pixel (SPECIFICATION.md section 1 and 7 item 5)");
    checks.record("DECLINED COP2-horizontal-beam-position.c6 ch2-horizontal-beam-position.c6 [p.23]: horizontal blanking $0F to $35 is a range of counter positions, not observable against the "
                  "displayed pixels for the same reason (SPECIFICATION.md section 1)");
    checks.record("DECLINED COP2-vertical-beam-position.c10 [p.24]: Copper execution over all 262 lines depends on where the Copper restarts, which the pages state two ways (SPECIFICATION.md section 1 and 7 item 8)");
}

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;
    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";
    if (mv.empty()) {
        const char* only_env = std::getenv("NMH_ONLY");     // development aid: run one section of the normal run
        const std::string only = only_env ? only_env : "ACBD";
        record_declined(checks);
        if (only.find('A') != std::string::npos) section_A(checks, dir, "", true);
        if (only.find('C') != std::string::npos) section_C(checks, dir, "", true);
        if (only.find('B') != std::string::npos) section_B(checks, dir, "", true);
        if (only.find('D') != std::string::npos) section_DE(checks, dir, "", true);
    } else if (mv.rfind("A-", 0) == 0) {
        section_A(checks, dir, mv, false);
    } else if (mv.rfind("C-", 0) == 0) {
        section_C(checks, dir, mv, false);
    } else if (mv.rfind("B-", 0) == 0) {
        section_B(checks, dir, mv, false);
    } else if (mv.rfind("D-", 0) == 0 || mv.rfind("E-", 0) == 0) {
        section_DE(checks, dir, mv, false);
    } else {
        std::printf("unknown NMH_MUTATE value %s\n", mv.c_str());
        return 2;
    }
    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
