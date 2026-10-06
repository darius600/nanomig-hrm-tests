// test_ch4_sprite_registers.cpp
//
// ch4-sprite-registers: see SPECIFICATION.md.
//
// Independent 68000 programs (MANUAL, AUTO/AUTO-ODD, CPU-PTR, JUMP, BLIT,
// COPBLT-v, BEAM, SHR-SWEEP/SHR-LORES/BITS-OCS), each run in its own
// NanomigHarness instance, grade P1-P14. With NMH_MUTATE unset, every
// baseline binary runs and every proposition, precondition and fixture check
// prints. With NMH_MUTATE set to one of the tags below, the program family of
// that tag runs with the changed binary in the named slot and the other
// slots of the family at baseline; a family's lines that depend on the
// changed run print too, so a precondition that fails prints its dependent
// line as NOT GRADED.
//
// The COPBLT, BEAM and SHR families grade a line only when its preconditions
// hold: a failing precondition prints FAIL on its own line and the dependent
// line prints RECORDED ... NOT GRADED, never PASS or FAIL.
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
//   P12-PROP, P12-ECSREG, P12-PRE1, P12-PRE2, P12-PRE3, P12-PRE3-QUIET, P12-PRE3-SIZH,
//   P13-PAIR1, P13-PAIR2, P13-PRE1, P13-PRE2, P13-PRE2-SIZH, P13-PRE3, FIXTURE-CB
//                                                -> prog_cb_<o|e><2|F|0|D>[_*].bin
//   P14-PROP-ECS, P14-PROP-OCS, P14-PRE1 (the ECS slot runs the OCS chip set),
//   P14-PRE2 (both slots PAL), P14-PRE3, P14-POP, R5-ANCHOR-BEAM, FIXTURE-BEAM
//                                                -> prog_beam_*.bin
//   P1-CONTROL, P1-PRE1, P1-PRE2, P1-PRE3, P2-PROP, P2-PRE1, P2-PRE2,
//   P2-DEP (the SHR-SWEEP slot has SHSH1 cleared everywhere), P2-PRE4, P2-PRE4-NOSPR, P3-PROP, P3-PRE1 (the BITS-OCS slot runs
//   the ECS chip set), P3-PRE2 (the same, with BPLCON0 = $1240), P3-PRE3,
//   P3-PRE4, P3-PRE5, R5-ANCHOR-SHR, FIXTURE-SHR  -> prog_shr*_*.bin
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
// MUTATION P10 stimulus red NMH_MUTATE=P10-PROP: field 4 writes COLOR01 (not a strobe) instead of COPJMP2 -- fx68k/TG68K: FAIL (in field 4 L2's first word is not read after M_4 and L1's MOVE data word $020006 is; fields 2 and 6 read L2 and not $020006)
// MUTATION P10-PRE1 stimulus red NMH_MUTATE=P10-PRE1: DMACON without COPEN -- fx68k/TG68K: FAIL (0 reads of $20000 at all)
// MUTATION P10-PRE2 stimulus red NMH_MUTATE=P10-PRE2: omits field 2's marker write -- fx68k/TG68K: FAIL (M_2 missing)
// MUTATION P10-PRE3 stimulus red NMH_MUTATE=P10-PRE3: waits until V7-0=$F4 (instead of $80) before every marker -- fx68k/TG68K: FAIL (markers fall after the $20004 read, not before)
// MUTATION FIXTURE-JUMP stimulus red NMH_MUTATE=FIXTURE-JUMP: $DEAD stored instead of $BEEF -- fx68k/TG68K: FAIL
// MUTATION P11 stimulus red NMH_MUTATE=P11-PROP: BLTCON0=$08F0 (USED cleared) -- fx68k/TG68K: FAIL (0 writes into DST1)
// MUTATION P11-PRE1 stimulus red NMH_MUTATE=P11-PRE1: DMACON without BLTEN -- fx68k/TG68K: FAIL (no A reads; same mutation also fails P11-PRE2 below)
// MUTATION P11-PRE2 stimulus red NMH_MUTATE=P11-PRE1: the same DMACON-without-BLTEN change -- fx68k/TG68K: FAIL (DMACONR bit 6=0)
// MUTATION P11-PRE3 stimulus red NMH_MUTATE=P11-PRE3: an extra CPU write to Chip memory ($30300) inside the M1-M2 window -- fx68k/TG68K: FAIL (window flagged not quiet)
// MUTATION FIXTURE-BLIT stimulus red NMH_MUTATE=FIXTURE-BLIT: $DEAD stored instead of $BEEF -- fx68k/TG68K: FAIL
// MUTATION FIXTURE-CB stimulus red NMH_MUTATE=FIXTURE-CB: the OCS $0002 run stores $DEAD instead of $BEEF -- fx68k/TG68K: FAIL (read back o2=DEAD); P12 is NOT GRADED
// MUTATION P12-PRE1 stimulus red NMH_MUTATE=P12-PRE1: DMACON without COPEN in both $0002 runs -- fx68k/TG68K: FAIL (no L1 read in any graded field); P12-PRE2 moves with it (DMACONR bit 7 clear) and P12 is NOT GRADED
// MUTATION P12-PRE2 stimulus red NMH_MUTATE=P12-PRE2: DMACON without BLTEN in both $0002 runs -- fx68k/TG68K: FAIL (DMACONR=0280); P12 is NOT GRADED
// MUTATION P12-PRE3 stimulus red NMH_MUTATE=P12-PRE3: the CPU writes BLTSIZE = $0082 before M1 (structural clause: one BLTSIZE store in each binary) -- fx68k/TG68K: FAIL; P12 is NOT GRADED
// MUTATION P12-PRE3 stimulus red NMH_MUTATE=P12-PRE3-QUIET: the CPU writes $1111 to $30200 inside M1-M2 (quiet-window clause) -- fx68k/TG68K: FAIL; P12 is NOT GRADED
// MUTATION P12-PRE3 stimulus red NMH_MUTATE=P12-PRE3-SIZH: the ECS run's CPU writes BLTSIZH = $0001 before M1 (one MOVE.W #imm,BLTSIZH($05E) in the binary) -- fx68k/TG68K: FAIL (BLTSIZH stores OCS=0 ECS=1; the quiet-window clause moves with it, 4 unexplained writes in M1-M2 on ECS); P12 is NOT GRADED
// MUTATION P12 stimulus red NMH_MUTATE=P12-PROP:the first BLTSIZE MOVE carries $0081 in both $0002 runs -- fx68k/TG68K: FAIL (DST1 gets two words instead of four); every precondition holds
// MUTATION P12 stimulus red NMH_MUTATE=P12-ECSREG: the ECS BLTSIZH MOVE carries $0003 -- fx68k/TG68K: FAIL (DST3 gets three words instead of four); every precondition holds
// MUTATION P13-PRE1 stimulus red NMH_MUTATE=P13-PRE1: DMACON without COPEN in the OCS and ECS $0002 runs -- fx68k/TG68K: FAIL; P13 is NOT GRADED
// MUTATION P13-PRE2 stimulus red NMH_MUTATE=P13-PRE2: the CPU writes BLTSIZE = $0082 at V7-0 >= $80 in the $0000 runs, where none is declared -- fx68k/TG68K: FAIL; P13 is NOT GRADED
// MUTATION P13-PRE2 stimulus red NMH_MUTATE=P13-PRE2-SIZH: the ECS $0000 run's CPU writes BLTSIZH = $0001 before M1 (one BLTSIZH store in the binary, none declared) -- fx68k/TG68K: FAIL (BLTSIZH stores ECS=1; no other precondition moves); P13 is NOT GRADED
// MUTATION P13-PRE3 stimulus red NMH_MUTATE=P13-PRE3: the CPU writes $1111 to $30200 inside M1-M2 of the $0000 runs -- fx68k/TG68K: FAIL; P13 is NOT GRADED
// MUTATION P13 stimulus red NMH_MUTATE=P13-PAIR1: the first BLTSIZE MOVE carries $0081 in the $FFFF runs -- fx68k/TG68K: FAIL (the $0002 and $FFFF outcomes differ on both chip sets); every precondition holds
// MUTATION P13 stimulus red NMH_MUTATE=P13-PAIR2: the CPU writes BLTSIZE = $0082 at V7-0 >= $80 of the first field in the $FFFD runs, one store declared for them -- fx68k/TG68K: FAIL (the $0000 and $FFFD outcomes differ on both chip sets); every precondition holds
// MUTATION FIXTURE-BEAM stimulus red NMH_MUTATE=FIXTURE-BEAM: the ECS run stores $DEAD instead of $BEEF -- fx68k/TG68K: FAIL (read back ECS=DEAD); P14 is NOT GRADED
// MUTATION P14-PRE1 stimulus red NMH_MUTATE=P14-PRE1: the run declared ECS is configured as OCS -- fx68k/TG68K: FAIL (VPOSR identification $10, expected $30); P14 is NOT GRADED
// MUTATION P14-PRE2 stimulus red NMH_MUTATE=P14-PRE2: both runs configured PAL -- fx68k/TG68K: FAIL (N=625 before M1); P14-PRE1 moves with it (identification $20 and $00) and P14 is NOT GRADED
// MUTATION P14-PRE3 stimulus red NMH_MUTATE=P14-PRE3: BPLCON0 = $0200 (LACE clear) in step 1 of both runs -- fx68k/TG68K: FAIL (the two fields of a frame have the same length); P14 is NOT GRADED
// MUTATION P14-POP stimulus red NMH_MUTATE=P14-POP: three VERTB after M1 instead of eight -- fx68k/TG68K: FAIL (two and three frames after M1 instead of four); P14 is NOT GRADED
// MUTATION R5-ANCHOR-BEAM stimulus red NMH_MUTATE=R5-ANCHOR-BEAM: the VERTB wait loop reads Chip word $0100 on every pass, a non-refresh access at a low address -- fx68k/TG68K: FAIL (every refresh group malformed); P14-PRE2 and P14-PRE3 move with it (their scan-line counts use the same groups) and P14 is NOT GRADED
// MUTATION P14 stimulus red NMH_MUTATE=P14-PROP-ECS: BEAMCON0 = $0000 in the ECS run -- fx68k/TG68K: FAIL (ECS frames after M1 stay 525); every precondition holds
// MUTATION P14 stimulus red NMH_MUTATE=P14-PROP-OCS: BPLCON0 = $0200 (LACE clear) is written at M1 in the OCS run -- fx68k/TG68K: FAIL (OCS frames after M1 are 526, not 525); every precondition holds
// MUTATION FIXTURE-SHR stimulus red NMH_MUTATE=FIXTURE-SHR: the SHR-SWEEP run stores $DEAD instead of $BEEF -- fx68k/TG68K: FAIL (read back SHR-SWEEP=DEAD); P1 and P1-CONTROL are NOT GRADED
// MUTATION R5-ANCHOR-SHR stimulus red NMH_MUTATE=R5-ANCHOR-SHR: the VERTB wait loop of the SHR-SWEEP run reads Chip word $0100 on every pass -- fx68k/TG68K: FAIL (every refresh group malformed); P1-PRE2 and P1-PRE3 move with it (their heights use the same groups), P1 and P1-CONTROL are NOT GRADED
// MUTATION P1-PRE1 stimulus red NMH_MUTATE=P1-PRE1: the SHR-SWEEP slot runs the Lores program (BPLCON0 = $1200) -- fx68k/TG68K: FAIL (ruler period 32, not 8); P1 and P1-CONTROL are NOT GRADED
// MUTATION P1-PRE2 stimulus red NMH_MUTATE=P1-PRE2: DMACON without SPREN in the SHR-SWEEP run -- fx68k/TG68K: FAIL (no sprite lines); P1-PRE3 moves with it, P1 and P1-CONTROL are NOT GRADED
// MUTATION P1-PRE3 stimulus red NMH_MUTATE=P1-PRE3: usage 0 of the SHR-SWEEP run has VSTOP = VSTART + 5 (a fifth data pair keeps the next usages aligned) -- fx68k/TG68K: FAIL (group 0 has 5 scan lines); P1 and P1-CONTROL are NOT GRADED
// MUTATION P1-CONTROL stimulus red NMH_MUTATE=P1-CONTROL: usage 2 of the SHR-SWEEP run has CTL $5400 (SH0 cleared) -- fx68k/TG68K: FAIL (group 2 has d=80, not 82); every precondition holds
// MUTATION P1 implementation green a private copy of the RTL outside the repository in which SPRxCTL bit 4 (SHSH1) delays the sprite's serial output by two 28 MHz clocks (70 ns) while the sprite resolution is SuperHires (denise_sprites_shifter.v and denise_sprites.v; the EBR shifter build of TG68K and the non-EBR build of fx68k), built with make NANOMIG=<copy> and run on both cores: every precondition holds, P1-CONTROL passes and P1 passes with d = 80 81 82 ... 89 in both graded fields; with the repository's RTL P1 is red on both cores (d = 80 80 82 82 84 84 86 86 88 88). The control through the same comparator on the same run is P1-CONTROL (even k, SHSH1 clear)
// MUTATION P2-PRE1 stimulus red NMH_MUTATE=P2-PRE1: the SHR-LORES slot runs the SuperHires program (BPLCON0 = $1240) -- fx68k/TG68K: FAIL (ruler period 8, not 32); P2 is NOT GRADED
// MUTATION P2-PRE2 stimulus red NMH_MUTATE=P2-PRE2: usage 2 of the Lores run has CTL $5400 (SH0 cleared) -- fx68k/TG68K: FAIL (group 2 has d=80, not 82); P2 is NOT GRADED
// MUTATION P2-PRE4 stimulus red NMH_MUTATE=P2-PRE4: usage 0 of the Lores run has VSTOP = VSTART + 5 -- fx68k/TG68K: FAIL (group 0 has 5 scan lines); P2 is NOT GRADED
// MUTATION P2-PRE4 stimulus red NMH_MUTATE=P2-PRE4-NOSPR: DMACON without SPREN in the Lores run -- fx68k/TG68K: FAIL (no sprite lines); P2-PRE2 moves with it and P2 is NOT GRADED
// MUTATION P2 implementation green the private copy of the RTL described at P1 (SPRxCTL bit 4 acts in SuperHires), both cores: every precondition holds, P1 passes and P2 passes (d even on every line of every group in the Lores run); with the repository's RTL P2 prints RECORDED NOT GRADED, depends on P1, because P1 is red
// MUTATION P2 stimulus red NMH_MUTATE=P2-PROP: the Copper switches to SuperHires after the ruler lines of the Lores run (prog_shrl_sw) -- on the private copy of the RTL described at P1, both cores: P2 FAIL with every precondition holding (the sprite lines are drawn in SuperHires, odd k give odd d); with the repository's RTL P2 is NOT GRADED, depends on P1
// MUTATION P2 stimulus red NMH_MUTATE=P2-DEP: the SHR-SWEEP slot has SHSH1 cleared in every usage (prog_shr_nosh1) -- on the private copy of the RTL described at P1, both cores: P1 FAIL and P2 NOT GRADED, depends on P1, with P2-PRE1, P2-PRE2 and P2-PRE4 holding
// MUTATION P3-PRE1 stimulus red NMH_MUTATE=P3-PRE1: the BITS-OCS slot runs the ECS chip set -- fx68k/TG68K: FAIL (VPOSR identification $20, expected $00); P3 is NOT GRADED
// MUTATION P3-PRE2 stimulus red NMH_MUTATE=P3-PRE2: the BITS-OCS slot runs BPLCON0 = $1240 on the ECS chip set -- fx68k/TG68K: FAIL (ruler period 8, not 32); P3-PRE1 moves with it (identification $20) and P3 is NOT GRADED
// MUTATION P3-PRE3 stimulus red NMH_MUTATE=P3-PRE3: usage 4 has CTL $6400 (SH0 cleared) -- fx68k/TG68K: FAIL (group 4 has d=80, not 82); P3 is NOT GRADED
// MUTATION P3-PRE4 stimulus red NMH_MUTATE=P3-PRE4: usage 0 has VSTOP = VSTART + 5 -- fx68k/TG68K: FAIL (group 0 has 5 scan lines); P3 is NOT GRADED
// MUTATION P3-PRE5 stimulus red NMH_MUTATE=P3-PRE5: DMACON without SPREN -- fx68k/TG68K: FAIL (no sprite lines); P3-PRE3 and P3-PRE4 move with it and P3 is NOT GRADED
// MUTATION P3 stimulus red NMH_MUTATE=P3-PROP: usage 1 has CTL $4C01 (SH0 instead of bit 4) -- fx68k/TG68K: FAIL (group 1 has d=82, not 80); every precondition holds

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <array>
#include <optional>
#include <algorithm>
#include <memory>
#include <map>
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
            // Fixed, unchanged comparator: in fields 2/4/6 L2's first word is
            // read after M_f and L1's MOVE data word is not read in the rest
            // of the field. Whether the jump first touches the old program
            // counter (the read of L1's MOVE register word, $20004) is open
            // on p.25 and is not graded. Under MUT_P10PROP field 4 writes
            // COLOR01 (not a strobe) instead of COPJMP2, so no restart happens
            // there and this naturally clashes with the same, unchanged
            // expectation.
            bool expect_l2 = (fnum == 2 || fnum == 4 || fnum == 6);
            bool l2_read = false, l1_data_read = false;
            for (auto& a : log) if (!a.is_write && a.cycle >= *m[i] && a.cycle < fi->second) {
                if (a.word_addr == kL2Base) l2_read = true;
                if (a.word_addr == kL1Base + 3) l1_data_read = true;
            }
            c.note("P10 field " + std::to_string(fnum) + ": first read after M_" + std::to_string(fnum) +
                   " in either list is at $" + hex6(first_after_addr * 2) +
                   (first_after_cyc ? (" (cycle " + std::to_string(*first_after_cyc) + ")") : " (none)") +
                   "; after M_" + std::to_string(fnum) + " in the field: L2's first word " +
                   (l2_read ? "read" : "not read") + ", L1's MOVE data word $020006 " +
                   (l1_data_read ? "read" : "not read"));
            if (expect_l2) {
                if (!l2_read || l1_data_read) ok = false;
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
                 "(p.287,25,26) -- in fields 2/4/6 L2's first word is read after M_f and L1's MOVE data word is not; "
                 "fields 1/3/5 read L1's MOVE first and never L2");
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
// Shared by COPBLT, BEAM and SHR
// ============================================================================
namespace ct {

nmh::Config make_cfg(bool ecs, bool ntsc) {
    nmh::Config c;
    c.chipset_config = uint8_t((ecs ? 0x08 : 0x00) | (ntsc ? 0x02 : 0x00));
    return c;
}

// R5: a refresh group is the four memory-refresh cycles of one scan line as
// the RAM port shows them (p.194): reads below word address $1000 that are
// the same number of clk_sys cycles apart. That spacing is measured, not
// assumed: it is the smallest gap among the first sixteen low reads of the
// capture, which precede the program (nothing but refresh reads there). Reads
// farther apart than twice that spacing belong to different groups. A group of
// any other shape is kept and marked, so a scan-line count never silently
// drops one. The last group of the capture may be cut by the end of the
// capture and is left out.
struct RefGroups {
    std::vector<uint64_t> firsts;
    std::vector<bool> well;
    uint64_t spacing = 0;
};

RefGroups refresh_groups(const std::vector<nmh::RamAccess>& log) {
    RefGroups rg;
    std::vector<uint64_t> low;
    for (const auto& a : log) if (!a.is_write && a.word_addr < 0x1000) low.push_back(a.cycle);
    std::sort(low.begin(), low.end());
    if (low.size() < 17) return rg;
    uint64_t s0 = low[1] - low[0];
    for (size_t k = 1; k < 16; ++k) s0 = std::min<uint64_t>(s0, low[k] - low[k - 1]);
    rg.spacing = s0;
    size_t i = 0;
    while (i < low.size()) {
        size_t j = i + 1;
        while (j < low.size() && low[j] - low[j - 1] <= 2 * s0) ++j;
        bool well = (j - i == 4);
        for (size_t k = i + 1; well && k < j; ++k)
            if (low[k] - low[k - 1] != s0) well = false;
        if (j < low.size()) { rg.firsts.push_back(low[i]); rg.well.push_back(well); }
        i = j;
    }
    return rg;
}

// R5: the number of scan lines between two times is the number of refresh
// groups that begin between them.
long groups_between(const RefGroups& rg, uint64_t lo, uint64_t hi) {
    if (hi <= lo) return 0;
    auto a = std::lower_bound(rg.firsts.begin(), rg.firsts.end(), lo);
    auto b = std::lower_bound(rg.firsts.begin(), rg.firsts.end(), hi);
    return (long)(b - a);
}

// number of refresh groups beginning in [lo,hi) that are not four cycles two
// slots apart, and the number that begin there at all
struct AnchorCount { size_t total = 0, malformed = 0; };
AnchorCount anchor_between(const RefGroups& rg, uint64_t lo, uint64_t hi) {
    AnchorCount n;
    for (size_t i = 0; i < rg.firsts.size(); ++i)
        if (rg.firsts[i] >= lo && rg.firsts[i] < hi) { ++n.total; if (!rg.well[i]) ++n.malformed; }
    return n;
}

std::string num(long v) { return std::to_string(v); }

std::string opt4(const std::optional<uint16_t>& v) { return v ? hex4(*v) : std::string("--"); }

// the byte offsets in a binary at which a MOVE.W #imm,d16(a0) to the given
// custom-register displacement occurs: 317C imm disp
int count_move_imm_a0(const std::string& path, uint16_t disp, bool* read_ok) {
    FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) { *read_ok = false; return 0; }
    std::vector<uint8_t> b;
    uint8_t buf[4096];
    size_t n;
    while ((n = std::fread(buf, 1, sizeof buf, f)) > 0) b.insert(b.end(), buf, buf + n);
    std::fclose(f);
    *read_ok = !b.empty();
    int cnt = 0;
    for (size_t i = 0; i + 6 <= b.size(); i += 2)
        if (b[i] == 0x31 && b[i + 1] == 0x7C && b[i + 4] == (disp >> 8) && b[i + 5] == (disp & 0xFF)) ++cnt;
    return cnt;
}

} // namespace ct

// ============================================================================
// COPBLT: P12, P13
// ============================================================================
namespace cb {

using ct::num;
using ct::opt4;

constexpr uint32_t kL1 = 0x020000 / 2;
constexpr uint32_t kRegLo = 0x030000 / 2, kRegHi = 0x030400 / 2;
constexpr int kL1Words = 72;

struct Acc {
    uint32_t a; bool w; uint16_t d;
    bool operator==(const Acc& o) const { return a == o.a && w == o.w && d == o.d; }
};

// SPECIFICATION.md P12 step 4: the words the blitter must write, in order
std::vector<Acc> expected_writes(bool ecs) {
    static const uint16_t d1[4] = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
    static const uint16_t d2[4] = {0xF000, 0x00F0, 0x0F00, 0x00F0};
    static const uint16_t d3[4] = {0xF0F0, 0x0FF0, 0xFF00, 0x00FF};
    std::vector<Acc> e;
    for (int i = 0; i < 4; ++i) e.push_back({0x030200u / 2 + (uint32_t)i, true, d1[i]});
    for (int i = 0; i < 4; ++i) e.push_back({0x030210u / 2 + (uint32_t)i, true, d2[i]});
    if (ecs) for (int i = 0; i < 4; ++i) e.push_back({0x030220u / 2 + (uint32_t)i, true, d3[i]});
    return e;
}

struct Run {
    std::string key, tag;
    bool ecs = false, done = false;
    int exp_cpu_bltsize = 0;
    std::optional<uint16_t> fixture, dmaconr;
    std::optional<uint64_t> m1, m2;
    std::vector<std::pair<uint64_t, uint64_t>> fields;
    std::vector<std::vector<Acc>> out;
    std::vector<std::vector<bool>> l1;
    int bltsize_writes = 0, bltsizh_writes = 0;
    bool bin_read = false;
    size_t stray_writes = 0;
};

Run do_run(const std::string& dir, const std::string& key, const std::string& bin, int exp_cpu) {
    Run r;
    r.key = key;
    r.ecs = key[0] == 'e';
    r.exp_cpu_bltsize = exp_cpu;
    r.tag = std::string(r.ecs ? "ECS" : "OCS") + " COPCON=$" +
            (key[1] == '2' ? "0002" : key[1] == 'F' ? "FFFF" : key[1] == '0' ? "0000" : "FFFD");
    bool ok1 = false, ok2 = false;
    r.bltsize_writes = ct::count_move_imm_a0(dir + "/" + bin, 0x0058, &ok1);
    r.bltsizh_writes = ct::count_move_imm_a0(dir + "/" + bin, 0x005E, &ok2);
    r.bin_read = ok1 && ok2;
    auto o = run_program(ct::make_cfg(r.ecs, false), dir + "/" + bin);
    r.done = o.loaded > 0 && o.released && o.done_seen;
    if (!r.done) return r;
    const auto& log = o.h->ram_log();
    r.fixture = o.h->peek_logical(kResults + 0);
    r.dmaconr = o.h->peek_logical(kResults + 2);
    r.m1 = find_write(log, kMark / 2 + 0, 1);
    r.m2 = find_write(log, kMark / 2 + 1, 2);
    if (!r.m1 || !r.m2) return r;
    auto fetches = find_reads(log, kL1);
    std::sort(fetches.begin(), fetches.end());
    for (size_t i = 0; i + 1 < fetches.size(); ++i)
        if (fetches[i] >= *r.m1 && fetches[i + 1] <= *r.m2) r.fields.push_back({fetches[i], fetches[i + 1]});
    for (auto& f : r.fields) {
        std::vector<Acc> seq;
        std::vector<bool> seen(kL1Words, false);
        for (auto& a : log) {
            if (a.cycle < f.first || a.cycle >= f.second) continue;
            if (!a.is_write && a.word_addr >= kL1 && a.word_addr < kL1 + kL1Words) seen[a.word_addr - kL1] = true;
            if (a.word_addr >= kRegLo && a.word_addr < kRegHi) {
                Acc x{a.word_addr, a.is_write, a.data};
                if (seq.empty() || !(seq.back() == x)) seq.push_back(x);
            }
        }
        r.out.push_back(seq);
        r.l1.push_back(seen);
    }
    // writes inside (M1,M2) that are not a blitter write the specification expects, and
    // not a write into the decoy destination (written only by a blit the CPU started)
    auto exp = expected_writes(r.ecs);
    for (auto& a : log) {
        if (!a.is_write || a.cycle <= *r.m1 || a.cycle >= *r.m2) continue;
        bool known = false;
        for (auto& e : exp) if (e.a == a.word_addr && e.d == a.data) known = true;
        if (a.word_addr >= 0x030300u / 2 && a.word_addr < 0x030400u / 2) known = true;
        if (!known) ++r.stray_writes;
    }
    return r;
}

// P12's comparator on one graded field
bool field_p12(bool ecs, const std::vector<Acc>& f) {
    std::vector<Acc> w;
    for (auto& a : f) if (a.w) w.push_back(a);
    if (!(w == expected_writes(ecs))) return false;
    static const uint32_t srcs[3] = {0x030000, 0x030010, 0x030020};
    for (uint32_t s : srcs)
        for (uint32_t i = 0; i < 4; ++i) {
            bool seen = false;
            for (auto& a : f) if (!a.w && a.a == s / 2 + i) seen = true;
            if (!seen) return false;
        }
    for (auto& a : f) {
        if (a.a >= 0x030100u / 2 && a.a < 0x030200u / 2) return false;
        if (a.a >= 0x030300u / 2 && a.a < 0x030400u / 2) return false;
    }
    return true;
}

bool l1_read_through(const Run& r, size_t f) {
    int last = r.ecs ? 69 : 49;
    for (int i = 0; i <= last; ++i) if (!r.l1[f][i]) return false;
    return true;
}

bool p12_pre1(const Run& r) {
    if (r.fields.empty()) return false;
    for (size_t f = 0; f < r.fields.size(); ++f) if (!l1_read_through(r, f)) return false;
    return true;
}
bool p12_pre2(const Run& r) {
    return r.dmaconr && ((*r.dmaconr >> 9) & 1) && ((*r.dmaconr >> 7) & 1) && ((*r.dmaconr >> 6) & 1);
}
bool pre_structure(const Run& r) {
    return r.bin_read && r.bltsize_writes == r.exp_cpu_bltsize && r.bltsizh_writes == 0;
}
bool pre_quiet(const Run& r) { return r.m1 && r.m2 && r.stray_writes == 0; }

struct Ovr { const char* mv; const char* key; const char* suffix; int exp_cpu; };
const Ovr kOvr[] = {
    {"P12-PROP", "o2", "s81", 0},   {"P12-PROP", "e2", "s81", 0},
    {"P12-ECSREG", "e2", "sizh3", 0},
    {"P12-PRE1", "o2", "nocop", 0}, {"P12-PRE1", "e2", "nocop", 0},
    {"P12-PRE2", "o2", "noblt", 0}, {"P12-PRE2", "e2", "noblt", 0},
    {"P12-PRE3", "o2", "cpuearly", 0}, {"P12-PRE3", "e2", "cpuearly", 0},
    {"P12-PRE3-QUIET", "o2", "cpuwr", 0}, {"P12-PRE3-QUIET", "e2", "cpuwr", 0},
    {"P12-PRE3-SIZH", "e2", "cpusizh", 0},
    {"P13-PAIR1", "oF", "s81", 0},  {"P13-PAIR1", "eF", "s81", 0},
    {"P13-PAIR2", "oD", "cpublt80", 1}, {"P13-PAIR2", "eD", "cpublt80", 1},
    {"P13-PRE1", "o2", "nocop", 0}, {"P13-PRE1", "e2", "nocop", 0},
    {"P13-PRE2", "o0", "cpublt80", 0}, {"P13-PRE2", "e0", "cpublt80", 0},
    {"P13-PRE2-SIZH", "e0", "cpusizh", 0},
    {"P13-PRE3", "o0", "cpuwr", 0}, {"P13-PRE3", "e0", "cpuwr", 0},
    {"FIXTURE-CB", "o2", "fix", 0},
};

bool owns(const std::string& mv) {
    return mv.empty() || mv.rfind("P12", 0) == 0 || mv.rfind("P13", 0) == 0 || mv == "FIXTURE-CB";
}

void run(const std::string& dir, const std::string& mv, Checks& c) {
    std::vector<std::string> keys;
    bool want12 = mv.empty() || mv.rfind("P12", 0) == 0 || mv == "FIXTURE-CB";
    bool want13 = mv.empty() || mv.rfind("P13", 0) == 0;
    if (want13) keys = {"o2", "oF", "o0", "oD", "e2", "eF", "e0", "eD"};
    else keys = {"o2", "e2"};
    std::map<std::string, Run> runs;
    for (auto& k : keys) {
        std::string bin = "prog_cb_" + k;
        int exp_cpu = 0;
        for (auto& o : kOvr)
            if (mv == o.mv && k == o.key) { bin += std::string("_") + o.suffix; exp_cpu = o.exp_cpu; }
        runs[k] = do_run(dir, k, bin + ".bin", exp_cpu);
    }

    // FIXTURE-CB: every run reaches DONE and delivers the known constant
    bool fx = true;
    std::string fxs;
    for (auto& k : keys) {
        auto& r = runs[k];
        bool ok = r.done && r.fixture && *r.fixture == 0xBEEF;
        fx = fx && ok;
        fxs += " " + k + "=" + (r.done ? opt4(r.fixture) : std::string("no-DONE"));
    }
    c.expect(fx, "FIXTURE-CB: every COPBLT run reaches DONE and stores a known constant (BEEF) at RESULTS"
                 " before anything else is trusted (read back:" + fxs + ")");

    // P12 preconditions (the $0002 run of each chip set)
    auto& o2 = runs["o2"]; auto& e2 = runs["e2"];
    bool pre1 = p12_pre1(o2) && p12_pre1(e2);
    bool pre2 = p12_pre2(o2) && p12_pre2(e2);
    bool pre3 = pre_structure(o2) && pre_structure(e2) && pre_quiet(o2) && pre_quiet(e2);
    c.note("COPBLT $0002 runs: OCS " + num((long)o2.fields.size()) + " graded fields, ECS " +
           num((long)e2.fields.size()) + " graded fields; DMACONR OCS=" + opt4(o2.dmaconr) +
           " ECS=" + opt4(e2.dmaconr) + "; CPU BLTSIZE stores in the binary OCS=" + num(o2.bltsize_writes) +
           " ECS=" + num(e2.bltsize_writes) + ", BLTSIZH stores OCS=" + num(o2.bltsizh_writes) + " ECS=" +
           num(e2.bltsizh_writes) + "; unexplained writes in M1-M2 OCS=" + num((long)o2.stray_writes) + " ECS=" +
           num((long)e2.stray_writes));
    if (want12) {
        c.expect(pre1, "P12-PRE1: in each graded field the Copper reads every word of L1 up to and including the "
                       "second BLTSIZE MOVE (OCS) and up to and including the BLTSIZH MOVE (ECS)");
        c.expect(pre2, "P12-PRE2: RESULT+2 (DMACONR) has bits 9, 7 and 6 set (DMAEN, COPEN, BLTEN) on both chip sets"
                       " (OCS=" + opt4(o2.dmaconr) + " ECS=" + opt4(e2.dmaconr) + ")");
        c.expect(pre3, "P12-PRE3: the CPU never writes BLTSIZE or BLTSIZH (no such store in the program binary) and"
                       " M1-M2 is a quiet window (every write at the port in M1-M2 is a blitter write)");
        bool gate = fx && pre1 && pre2 && pre3;
        if (!gate) {
            c.record("P12: NOT GRADED, a precondition does not hold (FIXTURE-CB, P12-PRE1, P12-PRE2 or P12-PRE3)");
        } else {
            bool ok = true;
            std::string detail;
            for (auto* r : {&o2, &e2}) {
                size_t good = 0;
                for (auto& f : r->out) if (field_p12(r->ecs, f)) ++good;
                if (good != r->out.size()) ok = false;
                detail += " " + std::string(r->ecs ? "ECS" : "OCS") + " " + num((long)good) + "/" +
                          num((long)r->out.size()) + " fields";
            }
            c.expect(ok, "P12 (APPC2-copcon.c2): with CDANG set the Copper can write the blitter registers -- a blit set "
                         "up and started entirely by the Copper runs as its values specify, on OCS and on ECS (p.310,"
                         "p.26,p.287): DST1/DST2 (and on ECS DST3) written as specified, SRCA/SRCB/SRCC read, nothing in "
                         "the decoy regions (fields matching:" + detail + ")");
        }
    }

    if (want13) {
        auto& oF = runs["oF"]; auto& o0 = runs["o0"]; auto& oD = runs["oD"];
        auto& eF = runs["eF"]; auto& e0 = runs["e0"]; auto& eD = runs["eD"];
        std::vector<Run*> all = {&o2, &oF, &o0, &oD, &e2, &eF, &e0, &eD};
        bool q1 = true;
        std::string q1s;
        for (auto* r : all) {
            bool ok = p12_pre2(*r);
            if (r->key[1] == '2' || r->key[1] == 'F') ok = ok && p12_pre1(*r);
            else {
                ok = ok && !r->fields.empty();
                for (size_t f = 0; f < r->fields.size(); ++f) if (!r->l1[f][0]) ok = false;
            }
            q1 = q1 && ok;
        }
        // a population to compare: the pair-1 runs show blitter accesses in every graded field
        for (auto* r : {&o2, &oF, &e2, &eF}) {
            if (r->out.empty()) q1 = false;
            for (auto& f : r->out) if (f.empty()) q1 = false;
        }
        bool q2 = true, q3 = true;
        for (auto* r : all) { q2 = q2 && pre_structure(*r); q3 = q3 && pre_quiet(*r); }
        c.expect(q1, "P13-PRE1: in every run RESULT+2 (DMACONR) has bits 9, 7 and 6 set; in the $0002 and $FFFF runs"
                     " the Copper reads L1 through the last BLTSIZE/BLTSIZH MOVE in each graded field, in the $0000 and"
                     " $FFFD runs it fetches L1's first word in each graded field; the $0002 and $FFFF runs show blitter"
                     " accesses in every graded field");
        c.expect(q2, "P13-PRE2: the CPU stores BLTSIZE only where a contrasting case says so (store count in each binary"
                     " equals the count declared for it) and never BLTSIZH");
        c.expect(q3, "P13-PRE3: M1-M2 is a quiet window in every run (every write at the port in M1-M2 is a blitter write)");
        std::string fl, sz;
        for (auto* r : all) {
            fl += " " + r->tag + ":" + num((long)r->fields.size());
            sz += " " + r->tag + ":" + (r->out.empty() ? std::string("-") : num((long)r->out[0].size()));
        }
        c.note("COPBLT graded fields per run:" + fl);
        c.note("COPBLT blitter accesses in $30000-$303FF in the first graded field of each run:" + sz);
        auto same = [&](const Run& a, const Run& b) {
            if (a.out.empty() || a.out.size() != b.out.size()) return false;
            for (size_t i = 0; i < a.out.size(); ++i) if (!(a.out[i] == b.out[i])) return false;
            return true;
        };
        bool gate = fx && q1 && q2 && q3;
        if (!gate) {
            c.record("P13: NOT GRADED, a precondition does not hold (FIXTURE-CB, P13-PRE1, P13-PRE2 or P13-PRE3)");
        } else {
            bool pair1_o = same(o2, oF), pair1_e = same(e2, eF);
            bool pair2_o = o0.out.size() == oD.out.size() && !o0.out.empty();
            bool pair2_e = e0.out.size() == eD.out.size() && !e0.out.empty();
            for (size_t i = 0; pair2_o && i < o0.out.size(); ++i) if (!(o0.out[i] == oD.out[i])) pair2_o = false;
            for (size_t i = 0; pair2_e && i < e0.out.size(); ++i) if (!(e0.out[i] == eD.out[i])) pair2_e = false;
            c.note("P13 equal outcomes: OCS $0002=$FFFF " + std::string(pair1_o ? "yes" : "no") + ", ECS $0002=$FFFF " +
                   (pair1_e ? "yes" : "no") + ", OCS $0000=$FFFD " + (pair2_o ? "yes" : "no") + ", ECS $0000=$FFFD " +
                   (pair2_e ? "yes" : "no"));
            c.expect(pair1_o && pair1_e && pair2_o && pair2_e,
                     "P13 (APPC2-copcon.c1): COPCON is a 1-bit register, only bit 1 matters -- on each chip set the "
                     "blitter outcome with COPCON=$FFFF equals that with $0002 and with $FFFD equals that with $0000 "
                     "(p.310,p.26,p.274)");
        }
    }
}

} // namespace cb

// ============================================================================
// BEAM: P14
// ============================================================================
namespace beam {

using ct::num;
using ct::opt4;

struct Run {
    std::string tag;
    bool ecs = false, ntsc = true, done = false;
    std::optional<uint16_t> vposr, fixture;
    std::optional<uint64_t> m1, m2, first_fetch;
    std::vector<uint64_t> vs;
    ct::RefGroups rg;
    std::vector<size_t> before, after;
    std::vector<long> nb, na;
    bool fields_differ_by_one = false;
    ct::AnchorCount anc_before, anc_after;
};

// bin: the program; cfg_ecs/cfg_ntsc: what the harness is configured to (the declared
// chip set of the slot is the caller's)
Run do_run(const std::string& dir, const std::string& tag, const std::string& bin, bool cfg_ecs, bool cfg_ntsc) {
    Run r;
    r.tag = tag; r.ecs = cfg_ecs; r.ntsc = cfg_ntsc;
    auto o = run_program(ct::make_cfg(cfg_ecs, cfg_ntsc), dir + "/" + bin, 1'500'000);
    r.done = o.loaded > 0 && o.released && o.done_seen;
    if (!r.done) return r;
    const auto& log = o.h->ram_log();
    const auto& video = o.h->video();
    r.vposr = o.h->peek_logical(kResults + 0);
    r.fixture = o.h->peek_logical(kResults + 2);
    r.m1 = find_write(log, kMark / 2 + 0, 1);
    r.m2 = find_write(log, kMark / 2 + 1, 2);
    r.first_fetch = find_read(log, 0x080400 / 2, 0);
    if (!r.m1 || !r.m2 || !r.first_fetch) return r;
    for (size_t i = 1; i < video.size(); ++i)
        if (video[i].vs && !video[i - 1].vs) r.vs.push_back(i);
    r.rg = ct::refresh_groups(log);
    // the frames before M1: from the second VSYNC activation after the program's first
    // instruction fetch, each frame ending (two fields on) before M1
    size_t j0 = 0;
    while (j0 < r.vs.size() && r.vs[j0] <= *r.first_fetch) ++j0;
    for (size_t i = j0 + 1; i + 2 < r.vs.size() && r.vs[i + 2] < *r.m1; ++i) r.before.push_back(i);
    size_t k0 = 0;
    while (k0 < r.vs.size() && r.vs[k0] <= *r.m1) ++k0;
    size_t a = k0 + 2;
    for (size_t i = a; i < a + 4 && i + 2 < r.vs.size(); ++i) r.after.push_back(i);
    r.fields_differ_by_one = !r.before.empty();
    for (size_t i : r.before) {
        r.nb.push_back(ct::groups_between(r.rg, r.vs[i], r.vs[i + 2]));
        long f1 = ct::groups_between(r.rg, r.vs[i], r.vs[i + 1]);
        long f2 = ct::groups_between(r.rg, r.vs[i + 1], r.vs[i + 2]);
        if (std::labs(f1 - f2) != 1) r.fields_differ_by_one = false;
    }
    for (size_t i : r.after) r.na.push_back(ct::groups_between(r.rg, r.vs[i], r.vs[i + 2]));
    if (!r.before.empty()) r.anc_before = ct::anchor_between(r.rg, r.vs[r.before.front()], r.vs[r.before.back() + 2]);
    if (!r.after.empty()) r.anc_after = ct::anchor_between(r.rg, r.vs[r.after.front()], r.vs[r.after.back() + 2]);
    return r;
}

std::string list(const std::vector<long>& v) {
    std::string s;
    for (size_t i = 0; i < v.size(); ++i) s += (i ? "," : "") + num(v[i]);
    return v.empty() ? std::string("none") : s;
}

bool owns(const std::string& mv) { return mv.empty() || mv.rfind("P14", 0) == 0 || mv == "FIXTURE-BEAM" || mv == "R5-ANCHOR-BEAM"; }

void run(const std::string& dir, const std::string& mv, Checks& c) {
    // slot E: the ECS run; slot O: the OCS run; both NTSC
    std::string binE = "prog_beam.bin", binO = "prog_beam.bin";
    bool cfgE_ecs = true, cfgE_ntsc = true, cfgO_ntsc = true;
    bool runE = true, runO = true;
    if (mv == "P14-PROP-ECS") { binE = "prog_beam_b0.bin"; runO = false; }
    if (mv == "P14-PROP-OCS") { binO = "prog_beam_lace1.bin"; runE = false; }
    if (mv == "P14-PRE1") { cfgE_ecs = false; runO = false; }
    if (mv == "P14-PRE2") { cfgE_ntsc = false; cfgO_ntsc = false; }
    if (mv == "P14-PRE3") { binE = "prog_beam_nolace.bin"; binO = "prog_beam_nolace.bin"; }
    if (mv == "P14-POP") { binE = "prog_beam_short.bin"; binO = "prog_beam_short.bin"; }
    if (mv == "R5-ANCHOR-BEAM") { binE = "prog_beam_lowrd.bin"; binO = "prog_beam_lowrd.bin"; }
    if (mv == "FIXTURE-BEAM") { binE = "prog_beam_fix.bin"; runO = false; }
    // a single-half proposition contrast and a single-slot precondition contrast still
    // need the other slot's baseline run for the dependent line
    if (mv == "P14-PROP-ECS" || mv == "P14-PROP-OCS") { runE = true; runO = true; }
    if (mv == "P14-PRE1" || mv == "FIXTURE-BEAM") { runE = true; runO = true; }

    Run E, O;
    if (runE) E = do_run(dir, "ECS NTSC", binE, cfgE_ecs, cfgE_ntsc);
    if (runO) O = do_run(dir, "OCS NTSC", binO, false, cfgO_ntsc);
    E.tag = "ECS NTSC"; O.tag = "OCS NTSC";
    bool fx = E.done && E.fixture && *E.fixture == 0xBEEF && O.done && O.fixture && *O.fixture == 0xBEEF;
    c.expect(fx, "FIXTURE-BEAM: both BEAM runs reach DONE and store a known constant (BEEF) at RESULT+2 before anything"
                 " else is trusted (read back ECS=" + (E.done ? opt4(E.fixture) : std::string("no-DONE")) +
                 " OCS=" + (O.done ? opt4(O.fixture) : std::string("no-DONE")) + ")");

    auto idv = [](const Run& r) { return r.vposr ? (long)((*r.vposr >> 8) & 0x7F) : -1L; };
    c.note("BEAM VPOSR chip identification: ECS slot $" + hex4((uint16_t)std::max(0L, idv(E))) + ", OCS slot $" +
           hex4((uint16_t)std::max(0L, idv(O))) + "; VSYNC activations ECS " + num((long)E.vs.size()) + " OCS " +
           num((long)O.vs.size()) + "; refresh-group spacing measured from the first group: ECS " +
           num((long)E.rg.spacing) + " OCS " + num((long)O.rg.spacing) + " clk_sys cycles");
    c.note("BEAM frames before M1 (N): ECS " + list(E.nb) + " | OCS " + list(O.nb) +
           "; after M1: ECS " + list(E.na) + " | OCS " + list(O.na));

    bool pre1 = idv(E) == 0x30 && idv(O) == 0x10;
    c.expect(pre1, "P14-PRE1: VPOSR bits 14-8 identify the NTSC Agnus of each kind -- $30 on the ECS run, $10 on the OCS run"
                   " (read ECS=$" + hex4((uint16_t)std::max(0L, idv(E))) + " OCS=$" + hex4((uint16_t)std::max(0L, idv(O))) + ")");
    auto same_not625 = [](const Run& r) {
        if (r.nb.empty()) return false;
        for (long n : r.nb) if (n != r.nb[0]) return false;
        return r.nb[0] != 625;
    };
    bool pre2 = same_not625(E) && same_not625(O);
    c.expect(pre2, "P14-PRE2: the graded frames before M1 all have the same N and it is not 625, so NTSC timing is in force"
                   " (ECS " + list(E.nb) + ", OCS " + list(O.nb) + ")");
    bool pre3 = E.fields_differ_by_one && O.fields_differ_by_one;
    c.expect(pre3, "P14-PRE3: in every graded frame before M1 the two fields differ in length by one scan line, so interlace is on");
    bool pop = !E.before.empty() && !O.before.empty() && E.after.size() == 4 && O.after.size() == 4;
    c.expect(pop, "P14-POP: the graded populations exist -- at least one frame before M1 and four after, on both runs (ECS "
                  + num((long)E.before.size()) + "+" + num((long)E.after.size()) + ", OCS " + num((long)O.before.size()) +
                  "+" + num((long)O.after.size()) + ")");
    bool anc = true;
    for (const Run* r : {&E, &O}) {
        if (r->anc_before.total == 0 || r->anc_after.total == 0) anc = false;
        if (r->anc_before.malformed || r->anc_after.malformed) anc = false;
    }
    c.expect(anc, "R5-ANCHOR-BEAM: in every graded frame, every refresh group has four refresh cycles (p.194) (malformed groups: ECS "
                  + num((long)(E.anc_before.malformed + E.anc_after.malformed)) + " of " +
                  num((long)(E.anc_before.total + E.anc_after.total)) + ", OCS " +
                  num((long)(O.anc_before.malformed + O.anc_after.malformed)) + " of " +
                  num((long)(O.anc_before.total + O.anc_after.total)) + ")");

    if (!(fx && pre1 && pre2 && pre3 && pop && anc)) {
        c.record("P14: NOT GRADED, a precondition does not hold (FIXTURE-BEAM, P14-PRE1, P14-PRE2, P14-PRE3, P14-POP or R5-ANCHOR-BEAM)");
        return;
    }
    bool ecs_ok = true;
    for (long n : E.na) if (n != 625) ecs_ok = false;
    bool ocs_ok = true;
    for (long n : O.na) if (n != O.nb[0]) ocs_ok = false;
    c.expect(ecs_ok && ocs_ok,
             "P14 (APPC2-BEAMCON0.uhres1): BEAMCON0 is new with ECS -- writing its PAL bit makes the interlaced frame 625 scan lines"
             " on ECS and changes nothing on OCS (p.298,p.305,p.304,p.24) (ECS frames after M1 " + list(E.na) +
             "; OCS frames after M1 " + list(O.na) + " against " + num(O.nb.empty() ? -1 : O.nb[0]) + " before)");
}

} // namespace beam

// ============================================================================
// SHR: P1, P2, P3
// ============================================================================
namespace shr {

using ct::num;
using ct::opt4;

// R4: a sprite pixel is a sample with G != 0 (the SuperHires-safe set makes every nonzero
// sprite colour green only and every nonzero playfield colour red only); a ruler pixel has R != 0.
struct LineInfo {
    size_t b = 0, e = 0;
    bool ruler = false;
    std::vector<long> rstarts;
    long ea = -1, eb = -1, runb = 0;
};

LineInfo analyse_line(const std::vector<nmh::VideoSample>& v, size_t b, size_t e) {
    LineInfo li;
    li.b = b; li.e = e;
    bool prev = false;
    for (size_t i = b; i < e && i < v.size(); ++i) {
        bool r = v[i].r != 0;
        if (r) li.ruler = true;
        if (r && !prev) li.rstarts.push_back((long)(i - b));
        prev = r;
    }
    size_t i = b;
    while (i < e && i < v.size() && v[i].g == 0) ++i;
    if (i >= e || i >= v.size()) return li;
    li.ea = (long)(i - b);
    while (i < e && i < v.size() && v[i].g != 0) ++i;
    while (i < e && i < v.size() && v[i].g == 0) ++i;
    if (i >= e || i >= v.size()) return li;
    li.eb = (long)(i - b);
    size_t j = i;
    while (j < e && j < v.size() && v[j].g != 0) ++j;
    li.runb = (long)(j - i);
    return li;
}

struct Group {
    long height = -1;
    std::vector<long> d, runb;
};

struct Field {
    uint64_t b = 0, e = 0;
    std::vector<long> rperiod;
    bool sref_block = false, sref_col_same = false;
    long sref_height = -1;
    std::vector<Group> groups;
    ct::AnchorCount anc;
};

// the period of the R != 0 pattern on a ruler line: the common distance between consecutive
// run starts, the first and the last run start (cut by the display window) left out
long ruler_period(const LineInfo& li) {
    const auto& s = li.rstarts;
    if (s.size() < 5) return -1;
    long p = s[2] - s[1];
    for (size_t i = 1; i + 2 < s.size(); ++i) if (s[i + 1] - s[i] != p) return -1;
    return p;
}

Field analyse_field(const std::vector<nmh::VideoSample>& video, const ct::RefGroups& rg, uint64_t b, uint64_t e) {
    Field f;
    f.b = b; f.e = e;
    std::vector<nmh::VideoSample> slice(video.begin() + (long)b, video.begin() + (long)std::min<uint64_t>(e, video.size()));
    auto lines = nmh::find_lines(slice);
    std::vector<LineInfo> li;
    for (auto& ln : lines) li.push_back(analyse_line(slice, ln.begin, ln.end));
    f.anc = ct::anchor_between(rg, b, e);

    // R3: the first three output lines on which R != 0 appears
    for (auto& l : li) {
        if (!l.ruler) continue;
        f.rperiod.push_back(ruler_period(l));
        if (f.rperiod.size() == 3) break;
    }

    // SREF: the lines with an e_A form one block, at one column
    long first = -1, last = -1;
    for (size_t i = 0; i < li.size(); ++i)
        if (li[i].ea >= 0) { if (first < 0) first = (long)i; last = (long)i; }
    if (first >= 0) {
        f.sref_block = true;
        f.sref_col_same = true;
        for (long i = first; i <= last; ++i) {
            if (li[i].ea < 0) f.sref_block = false;
            if (li[i].ea != li[first].ea) f.sref_col_same = false;
        }
        if ((size_t)last + 1 < li.size())
            f.sref_height = ct::groups_between(rg, b + li[first].b, b + li[last + 1].b);
    }

    // SWP: the groups of consecutive output lines that show an e_B
    for (size_t i = 0; i < li.size();) {
        if (li[i].eb < 0) { ++i; continue; }
        size_t j = i;
        Group g;
        while (j < li.size() && li[j].eb >= 0) {
            g.d.push_back(li[j].eb - li[j].ea);
            g.runb.push_back(li[j].runb);
            ++j;
        }
        if (j < li.size()) g.height = ct::groups_between(rg, b + li[i].b, b + li[j].b);
        f.groups.push_back(g);
        i = j;
    }
    return f;
}

struct Run {
    std::string tag;
    bool ecs = false, done = false;
    std::optional<uint16_t> vposr, fixture;
    std::vector<Field> fields;
    uint64_t spacing = 0;
};

Run do_run(const std::string& dir, const std::string& tag, const std::string& bin, bool ecs) {
    Run r;
    r.tag = tag; r.ecs = ecs;
    auto o = run_program(ct::make_cfg(ecs, false), dir + "/" + bin);
    r.done = o.loaded > 0 && o.released && o.done_seen;
    if (!r.done) return r;
    const auto& log = o.h->ram_log();
    const auto& video = o.h->video();
    r.vposr = o.h->peek_logical(kResults + 0);
    r.fixture = o.h->peek_logical(kResults + 2);
    auto m1 = find_write(log, kMark / 2 + 0, 1);
    auto m2 = find_write(log, kMark / 2 + 1, 2);
    if (!m1 || !m2) return r;
    auto rg = ct::refresh_groups(log);
    r.spacing = rg.spacing;
    for (auto& sp : nmh::find_frames(video))
        if (sp.begin > *m1 && sp.end < *m2) r.fields.push_back(analyse_field(video, rg, sp.begin, sp.end));
    return r;
}

// ---- predicates over a run's graded fields -----------------------------------------------
bool all_fields(const Run& r, bool (*pred)(const Field&)) {
    if (r.fields.empty()) return false;
    for (auto& f : r.fields) if (!pred(f)) return false;
    return true;
}

bool ruler_is(const Run& r, long p) {
    if (r.fields.empty()) return false;
    for (auto& f : r.fields) {
        if (f.rperiod.size() != 3) return false;
        for (long x : f.rperiod) if (x != p) return false;
    }
    return true;
}
bool sref_ok(const Field& f) { return f.sref_block && f.sref_col_same && f.sref_height == 80; }
bool g0_four(const Field& f) { return !f.groups.empty() && f.groups[0].height == 4; }

bool group_all_d(const Group& g, long want) {
    if (g.d.empty()) return false;
    for (long d : g.d) if (d != want) return false;
    return true;
}
bool group_all_even(const Group& g) {
    if (g.d.empty()) return false;
    for (long d : g.d) if (d % 2 != 0) return false;
    return true;
}

bool p1_field(const Field& f) {
    if (f.groups.size() != 10) return false;
    for (size_t k = 0; k < 10; ++k) if (!group_all_d(f.groups[k], 80 + (long)k)) return false;
    return true;
}
bool p2_field(const Field& f) {
    if (f.groups.size() != 10) return false;
    for (auto& g : f.groups) if (!group_all_even(g)) return false;
    return true;
}
bool p2pre2_field(const Field& f) {
    if (f.groups.size() != 10) return false;
    for (size_t k = 0; k < 10; k += 2) if (!group_all_d(f.groups[k], 80 + (long)k)) return false;
    return true;
}
bool p3_field(const Field& f) {
    if (f.groups.size() != 5) return false;
    for (size_t k = 0; k < 4; ++k) {
        if (!group_all_d(f.groups[k], 80)) return false;
        if (f.groups[k].height != 4) return false;
    }
    long run0 = f.groups[0].runb.empty() ? -1 : f.groups[0].runb[0];
    for (size_t k = 0; k < 4; ++k) for (long r : f.groups[k].runb) if (r != run0) return false;
    return true;
}
bool p3pre3_field(const Field& f) { return f.groups.size() == 5 && group_all_d(f.groups[4], 82); }

std::string field_summary(const Run& r) {
    std::string s = "refresh-group spacing " + num((long)r.spacing) + " clk_sys cycles, " +
                    num((long)r.fields.size()) + " graded fields";
    for (size_t i = 0; i < r.fields.size() && i < 2; ++i) {
        const Field& f = r.fields[i];
        s += "; field " + num((long)i) + ": ruler periods";
        for (long p : f.rperiod) s += " " + num(p);
        s += ", SREF block=" + std::string(f.sref_block ? "1" : "0") + " one-column=" + (f.sref_col_same ? "1" : "0") +
             " height=" + num(f.sref_height) + ", " + num((long)f.groups.size()) + " groups (height:d of the first line)";
        for (auto& g : f.groups) s += " " + num(g.height) + ":" + (g.d.empty() ? std::string("-") : num(g.d[0]));
    }
    return s;
}

bool owns(const std::string& mv) {
    return mv.empty() || mv.rfind("P1-", 0) == 0 || mv.rfind("P2-", 0) == 0 || mv.rfind("P3-", 0) == 0 ||
           mv == "FIXTURE-SHR" || mv == "R5-ANCHOR-SHR";
}

void run(const std::string& dir, const std::string& mv, Checks& c) {
    std::string binS = "prog_shr.bin", binL = "prog_shrl.bin", binO = "prog_shro.bin";
    bool ecsO = false;
    bool wantS = true, wantL = true, wantO = true;
    if (mv.rfind("P1-", 0) == 0 || mv == "FIXTURE-SHR" || mv == "R5-ANCHOR-SHR") { wantL = false; wantO = false; }
    if (mv.rfind("P2-", 0) == 0) wantO = false;
    if (mv.rfind("P3-", 0) == 0) { wantS = false; wantL = false; }
    if (mv == "P1-CONTROL") binS = "prog_shr_u2cl.bin";
    if (mv == "P1-PRE1") binS = "prog_shrl.bin";
    if (mv == "P1-PRE2") binS = "prog_shr_nospr.bin";
    if (mv == "P1-PRE3") binS = "prog_shr_g0h5.bin";
    if (mv == "P2-PROP") binL = "prog_shrl_sw.bin";
    if (mv == "P2-PRE1") binL = "prog_shr.bin";
    if (mv == "P2-PRE2") binL = "prog_shrl_u2cl.bin";
    if (mv == "P2-DEP") binS = "prog_shr_nosh1.bin";
    if (mv == "P2-PRE4") binL = "prog_shrl_g0h5.bin";
    if (mv == "P2-PRE4-NOSPR") binL = "prog_shrl_nospr.bin";
    if (mv == "P3-PROP") binO = "prog_shro_u1sh0.bin";
    if (mv == "P3-PRE1") ecsO = true;
    if (mv == "P3-PRE2") { binO = "prog_shro_shres.bin"; ecsO = true; }
    if (mv == "P3-PRE3") binO = "prog_shro_u4cl.bin";
    if (mv == "P3-PRE4") binO = "prog_shro_g0h5.bin";
    if (mv == "P3-PRE5") binO = "prog_shro_nospr.bin";
    if (mv == "R5-ANCHOR-SHR") binS = "prog_shr_lowrd.bin";
    if (mv == "FIXTURE-SHR") binS = "prog_shr_fix.bin";

    Run S, L, O;
    std::vector<Run*> ran;
    if (wantS) { S = do_run(dir, "SHR-SWEEP ECS", binS, true); ran.push_back(&S); }
    if (wantL) { L = do_run(dir, "SHR-LORES ECS", binL, true); ran.push_back(&L); }
    if (wantO) { O = do_run(dir, std::string("BITS-OCS ") + (ecsO ? "ECS" : "OCS"), binO, ecsO); ran.push_back(&O); }

    bool fx = true;
    std::string fxs;
    for (auto* r : ran) {
        fx = fx && r->done && r->fixture && *r->fixture == 0xBEEF;
        fxs += " " + r->tag + "=" + (r->done ? opt4(r->fixture) : std::string("no-DONE"));
    }
    c.expect(fx, "FIXTURE-SHR: every SHR run reaches DONE and stores a known constant (BEEF) at RESULT+2 before anything"
                 " else is trusted (read back:" + fxs + ")");

    bool anc = true;
    std::string ancs;
    for (auto* r : ran) {
        size_t tot = 0, bad = 0;
        for (auto& f : r->fields) { tot += f.anc.total; bad += f.anc.malformed; }
        if (r->fields.empty() || tot == 0 || bad != 0) anc = false;
        ancs += " " + r->tag + " " + num((long)bad) + " of " + num((long)tot);
    }
    c.expect(anc, "R5-ANCHOR-SHR: in every graded field, every refresh group has four refresh cycles (p.194) (malformed groups:" + ancs + ")");
    for (auto* r : ran) c.note("SHR " + r->tag + ": " + field_summary(*r));

    // ---------------------------------------------------------------- P1
    bool p1_pass = false;
    if (wantS) {
        bool pre1 = ruler_is(S, 8);
        bool pre2 = all_fields(S, sref_ok);
        bool pre3 = all_fields(S, g0_four);
        c.expect(pre1, "P1-PRE1: the R != 0 pattern of the three ruler lines has a period of 8 output pixels, so SuperHires is active in every graded field");
        c.expect(pre2, "P1-PRE2: SREF shows an e_A at one column on every output line of its 80 scan lines in every graded field");
        c.expect(pre3, "P1-PRE3: group 0 has exactly 4 scan lines in every graded field");
        if (!(fx && anc && pre1 && pre2 && pre3)) {
            c.record("P1-CONTROL: NOT GRADED, a precondition does not hold (FIXTURE-SHR, R5-ANCHOR-SHR, P1-PRE1, P1-PRE2 or P1-PRE3)");
            c.record("P1: NOT GRADED, a precondition does not hold (FIXTURE-SHR, R5-ANCHOR-SHR, P1-PRE1, P1-PRE2 or P1-PRE3)");
        } else {
            c.expect(all_fields(S, p2pre2_field),
                     "P1-CONTROL: the groups with SHSH1 clear (even k) show d = 80 + k on every line, through the comparator P1 applies "
                     "to every group, so P1's measurement basis is live on the same run (p.303: SH8-SH1 and SH0 steps)");
            p1_pass = all_fields(S, p1_field);
            c.expect(p1_pass, "P1 (APPC-pos.c2 CH4-appC-position.c18 CH4-appC-position.c4): in SuperHires on ECS, SPRxCTL bit 4 moves "
                              "the sprite start by 70 ns, two SuperHires pixels, and every 70 ns position in between is reachable (p.303) "
                              "-- ten groups, d = 80 + k on every line of group k");
        }
    }

    // ---------------------------------------------------------------- P2
    if (wantL) {
        bool pre1 = ruler_is(L, 32);
        bool pre2 = all_fields(L, p2pre2_field);
        bool pre4 = all_fields(L, sref_ok) && all_fields(L, g0_four);
        c.expect(pre1, "P2-PRE1: the R != 0 pattern of the three ruler lines has a period of 32 output pixels, so Lores is active in every graded field");
        c.expect(pre2, "P2-PRE2: for even k the Lores run shows d = 80 + k, so the measurement resolves 140 ns steps in Lores");
        c.expect(pre4, "P2-PRE4: SREF shows its e_A on every one of its 80 scan lines and group 0 has 4 scan lines in the Lores run (P1's preconditions 2 and 3)");
        if (!(fx && anc && pre1 && pre2 && pre4)) {
            c.record("P2: NOT GRADED, a precondition does not hold (FIXTURE-SHR, R5-ANCHOR-SHR, P2-PRE1, P2-PRE2 or P2-PRE4)");
        } else if (!p1_pass) {
            // the paired SHR-SWEEP run must pass P1: bit 4 must be live in this configuration, or a
            // parity pass could come from a bit that is ignored everywhere (SPECIFICATION.md P2, item 3 of its preconditions)
            c.record("P2: NOT GRADED, depends on P1");
        } else {
            c.expect(all_fields(L, p2_field), "P2: on ECS in Lores, a sprite with SPRxCTL bit 4 set starts at a whole multiple of 140 ns, never at an odd "
                                              "multiple of 70 ns (p.303) -- ten groups, d even on every line of every group");
        }
    }

    // ---------------------------------------------------------------- P3
    if (wantO) {
        bool pre1 = O.vposr && ((*O.vposr >> 8) & 0x7F) == 0x00;
        bool pre2 = ruler_is(O, 32);
        bool pre3 = all_fields(O, p3pre3_field);
        bool pre4 = all_fields(O, g0_four);
        bool pre5 = all_fields(O, sref_ok);
        c.expect(pre1, "P3-PRE1: VPOSR bits 14-8 = $00, the original PAL Agnus (read $" +
                       (O.vposr ? hex4((uint16_t)((*O.vposr >> 8) & 0x7F)) : std::string("--")) + ")");
        c.expect(pre2, "P3-PRE2: the R != 0 pattern of the three ruler lines has a period of 32 output pixels, so Lores is active in every graded field");
        c.expect(pre3, "P3-PRE3: group 4 has d = 82 on every line, so a 140 ns change is visible in this run");
        c.expect(pre4, "P3-PRE4: group 0 has exactly 4 scan lines in every graded field");
        c.expect(pre5, "P3-PRE5: SREF shows an e_A at one column on every output line of its 80 scan lines in every graded field");
        if (!(fx && anc && pre1 && pre2 && pre3 && pre4 && pre5)) {
            c.record("P3: NOT GRADED, a precondition does not hold (FIXTURE-SHR, R5-ANCHOR-SHR or P3-PRE1 to P3-PRE5)");
        } else {
            c.expect(all_fields(O, p3_field), "P3: on OCS, setting SPRxCTL bit 3, bit 4 or both leaves the sprite's position and image as with both bits "
                                              "clear (p.303) -- five groups, d = 80 on every line of groups 0 to 3, the same sprite-2 run length, "
                                              "each of groups 0 to 3 four scan lines");
        }
    }
}

} // namespace shr

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
        cb::run(dir, mv, c);
        beam::run(dir, mv, c);
        shr::run(dir, mv, c);
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
    } else if (cb::owns(mv)) {
        cb::run(dir, mv, c);
    } else if (beam::owns(mv)) {
        beam::run(dir, mv, c);
    } else if (shr::owns(mv)) {
        shr::run(dir, mv, c);
    } else {
        c.expect(false, "NMH_MUTATE=" + mv + ": unknown tag");
    }

    std::printf("%d passed, %d failed\n", c.passed, c.failed);
    return c.failed == 0 ? 0 : 1;
}
