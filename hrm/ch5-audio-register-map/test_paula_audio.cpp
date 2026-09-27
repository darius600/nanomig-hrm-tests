// ch5-audio-register-map -- paula_audio conformance test
//
// Test purpose: the audio register map at the paula_audio boundary -- the
//   four identical per-channel base addresses (AUD0BASE..AUD3BASE) and,
//   within each channel, the four registers AUDxLEN/AUDxPER/AUDxVOL/AUDxDAT,
//   each shown to be its own distinctly decoded, distinctly functioning
//   location, all reached without leaving paula_audio's own instance tree.
//   Also: the "one state machine per channel" independence claim, the
//   dmal/dmas once-per-horizontal-line latch, and the two-volume-path
//   65-level claim (paula_audio exposes BOTH the multiply path (ldata/
//   rdata) and the PWM path (ldata_okk/rdata_okk), so this package can
//   measure the already-flagged divergence between them directly rather than
//   assume one).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition. Chapter 5
//   printed p.164 (one state machine per channel; DMA/restart requests
//   transferred to Agnus once per horizontal line); printed pp.265-266
//   (Appendix A register descriptions, AUDxDAT/AUDxLEN/AUDxPER/AUDxVOL,
//   generic x form); printed pp.290-291 (Appendix B per-channel register
//   list, AUD0..AUD3 instances); printed p.298 and p.310 (Appendix C's ECS
//   register map and its prose row for AUDxPER).
// Non-goals: anything outside the `paula_audio` fixture this package builds,
//   and any claim it does not name. A green here means those assertions held
//   on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: ids such as `5-p164.c1` and `5-sm.c1` name one sentence of the
//   manual in this chapter's reading. They carry no reader prefix, and they
//   resolve in the claim files that hrm-testpipeline/acceptance/ch5.json lists
//   under `inventories`.
//
// CLAIM SOURCE: this package's own claims file is
//   hrm-testpipeline/work/ch5_packages/
//   ch5-audio-register-map.jsonl -- 116 records, all cited below by id.
//
// EXPECTED VALUES AND MEASUREMENT METHOD: a register-map package cannot be
//   written from the claims' quotations alone once the claims assert facts
//   ("belongs to Paula", "is the length register") that are not obviously
//   observable at a black-box port list. The MEASUREMENT METHOD therefore
//   comes from paula_audio.v and paula_audio_channel.v's register-write,
//   volume-counter and state-machine sections: which of paula_audio's own
//   OUTPUT ports can serve as an observable proxy for each register's
//   claimed identity (see BOUNDARY / PROXY note below). Every EXPECTED
//   VALUE below comes from the claims file's own quotations (an address, a
//   register name, a level count, a bit-6 rule) and never from what the
//   RTL happens to compute.
//
// BOUNDARY / PROXY FOR "BELONGS TO / IS DECODED BY / IS ASSIGNED TO /
//   LIVES IN / IS IMPLEMENTED BY Paula": this fixture IS paula_audio.v,
//   with no paula.v register-decode layer above it in the compiled unit.
//   Any address-selective effect observed at paula_audio's own ports
//   (audint, dmal, dmas, ldata/rdata, ldata_okk/rdata_okk) is therefore, by
//   construction of this build, an effect produced entirely inside Paula's
//   audio subsystem -- paula.v (not built here) only wires reg_address_in/
//   data_in/dmaena straight through (see BOUNDARIES.md). That is the proxy
//   this file uses for every "belongs to Paula" style claim; it is not a
//   claim about paula.v's own wiring, which this package does not open.
//
// NON-GOALS: no ADKCON port exists on this fixture (BOUNDARIES.md), so
//   attach and modulation claims are not here and were not sought. dmaena
//   arrives pre-decoded (DMACON->AUDxEN decode is paula.v's, a different
//   package's boundary) -- this file drives dmaena directly and asserts
//   nothing about how it got that value. AUDxLEN values 1, 2 and 4 and the
//   exact interrupt-moment claim are known measured-not-asserted items
//   (CANDIDATES.md) and this file does not touch them: LEN is exercised
//   only at 3 and 6, chosen precisely because CANDIDATES.md names those as
//   behaving normally. denise.v's missing $096 decode and paula.v's missing
//   DMACONR decode are outside this module entirely and are not touched.
//   This file does not assert the exact tick-for-tick period-counter
//   formula or exact interrupt timing -- those belong to
//   paula_audio_channel's own boundary; AUDxPER's identity is shown
//   here only as "bigger AUDPER makes the first-half hold longer", which
//   is what "period register" means, not a specific formula.
//   audpen (intpen) is held low for the whole run: paula_intcontroller,
//   which would normally drive it, is chapter 7's boundary
//   (BOUNDARIES.md), not this one. Held at 0 it never blocks a fresh
//   direct-mode write and, measured, it also never gates the steady
//   DMA-mode loop (AUDxON=1 makes state 4's own exit condition true
//   regardless of AUDxIP) -- so no interrupt-pending model is needed.
//
// MEASUREMENT BASIS, per assertion (why the stimulus asserts what it does):
//
//  CHANNEL-INDEPENDENCE (HRM p.164, "There is one audio state machine for
//   each channel"): a direct-mode AUDxDAT write on channel c alone must
//   raise audint[c] and leave the other three audint bits at 0. Run once
//   per channel, all four required to hold.
//
//  DMA-REQUEST-LATCHED-ONCE-PER-LINE / DMA-RESTART-LATCHED-ONCE-PER-LINE
//   (HRM p.164, "DMA requests and restart requests are transferred to
//   Agnus once each horizontal line"): channel 0 run continuously in DMA
//   mode across many simulated lines (hpos 0..226, strhor pulsed once per
//   line). dmal[0]/
//   dmas[0] are sampled every clk7_en tick and must stay constant except
//   in a short window around each strhor pulse (paula_audio.v:171-179 only
//   updates them `if (strhor)`, so the window is a tolerance for where
///  exactly in that one clk7_en period the registered change becomes
//   visible, not a claim that it can land anywhere) -- and each must take
//   at least two distinct values over the run, proving the invariant is
//   not vacuously true of a signal that never moves.
//
//  LEN-REGISTER-IDENTITY: same DMA-mode run, compared at AUDxLEN=3 against
//   AUDxLEN=6 (CANDIDATES.md's own two normally-behaving values). The
//   restart flag dmas[c] (paula_audio_channel.v: `dmas <= dmasen | lenfin`)
//   must assert strictly more often over a fixed number of lines at the
//   smaller length -- the length register is, on the manual's own words,
//   what makes a channel "restart" sooner or later.
//
//  PER-REGISTER-IDENTITY: direct mode, AUDxDAT holds a two-byte marker so
//   the channel alternates high-byte/low-byte output every period. The
//   number of consecutive cck ticks the high byte holds must be strictly
//   longer at the larger of two AUDxPER values -- that is what "period
//   register" means on the manual's own words, independent of the exact
//   tick formula.
//
//  VOL-REGISTER-IDENTITY: direct mode, AUDxDAT and AUDxPER held fixed.
//   With AUDxVOL left at its reset value the PWM output (ldata_okk/
//   rdata_okk) must read zero at every sampled tick; with AUDxVOL written
//   to a mid-range value it must read nonzero at at least one sampled
//   tick. That is the volume register gating the channel's own output.
//
//  DAT-REGISTER-IDENTITY: direct mode. Writing a two-byte marker to the
//   channel's own AUDxDAT address must produce that marker's high byte
//   then its low byte at the channel's own PWM output; writing the
//   identical marker to the SAME channel's AUDxLEN address instead (a
//   built-in control case, not a mutation) must produce no output at all,
//   proving DAT's address is not aliased to LEN's.
//
//  VOLUME-65-LEVELS (PWM path, ldata_okk/rdata_okk): the PWM gate free-
//   runs a 6-bit down-counter reloaded exactly once (at the direct-mode
//   entry) from AUDxVOL and never again while looping between the two
//   output states, so any 64-consecutive-cck window sees the gate open
//   for exactly AUDxVOL of those 64 ticks (or all 64, for AUDxVOL=64,
//   since the gate compares against the full 7-bit register while the
//   counter itself only holds 6 bits). Measured over a 256-tick (4x64)
//   window at AUDxVOL = 0, 1, 32, 63, 64: open-tick counts must be exactly
//   4x the written value (256 for 64), strictly increasing -- appendix A's
//   65 linear levels, and its bit 6 "forces max, 64 ones no zeros".
//
//  VOLUME-64-VERSUS-63-ON-THE-MULTIPLY-PATH (ldata/rdata): the SAME
//   proposition (63 and 64 are two of the 65 DISTINCT levels the manual
//   describes) measured at paula_audio's OTHER volume output.
//   paula_audio_mixer.v:26-32 ORs vol[6] into every bit of the 6-bit input
//   it hands to paula_audio_volume, so AUDxVOL=63 and AUDxVOL=64 reach the
//   multiplier as the same six-bit pattern. The two paths differ, so the
//   path is named: measured here, at ldata/rdata, with a fixed nonzero
//   sample. THIS ASSERTION IS EXPECTED RED. It is not a units error --
//   VOLUME-65-LEVELS above is the control proving the harness's own
//   measurement basis is sound; only the boundary differs. No NM-HRM
//   finding key is assigned by this file.
//
// CLAIM COVERAGE -- all 116 records, cited by id inside the expect() calls
//   below:
//     CHANNEL-INDEPENDENCE                       2 ids  (5-p164.c1, 5-sm.c1)
//     DMA-REQUEST-LATCHED-ONCE-PER-LINE          2 ids  (5-p164.c29, 5-sm.c30)
//     DMA-RESTART-LATCHED-ONCE-PER-LINE          2 ids  (5-p164.c30, 5-sm.c30)
//     LEN-REGISTER-IDENTITY                     25 ids
//     PER-REGISTER-IDENTITY                     32 ids
//     VOL-REGISTER-IDENTITY                     25 ids
//     DAT-REGISTER-IDENTITY                     26 ids
//     VOLUME-65-LEVELS                           3 ids  (5-appA-p266.c32,
//                                                 5-appA-p266.c34, A-audxvol.c3)
//     VOLUME-64-VERSUS-63-ON-THE-MULTIPLY-PATH   2 ids  (5-appA-p266.c32,
//                                                 A-audxvol.c3 -- re-measured
//                                                 at the other path; c34's own
//                                                 "ones and zeros" wording is a
//                                                 PWM-specific description and
//                                                 is cited only on the PWM
//                                                 assertion above)
//   2+2+2+25+32+25+26+3+2 = 119 citations over 116 distinct ids (5-sm.c30
//   is cited on both DMA-latch assertions, and 5-appA-p266.c32/A-audxvol.c3
//   are each cited on both volume-path assertions -- 3 shared citations,
//   119-3=116).
//
// RESULTS: 8 passed, 1 failed. CHANNEL-INDEPENDENCE, DMA-REQUEST-LATCHED-ONCE-PER-LINE,
//   DMA-RESTART-LATCHED-ONCE-PER-LINE, LEN-REGISTER-IDENTITY (dmas
//   asserted 7 times over 20 lines at AUDxLEN=3 versus 4 at AUDxLEN=6),
//   PER-REGISTER-IDENTITY (high-byte hold 2 cck ticks at AUDxPER=4 versus
//   14 at AUDxPER=16), VOL-REGISTER-IDENTITY, DAT-REGISTER-IDENTITY and
//   VOLUME-65-LEVELS (measured open-tick counts over a 256-tick window:
//   0, 4, 128, 252, 256 for AUDxVOL = 0, 1, 32, 63, 64 -- exactly 4x each
//   written value) are all GREEN. VOLUME-64-VERSUS-63-ON-THE-MULTIPLY-PATH
//   is RED: ldata settles at 4032 for BOTH AUDxVOL=63 and AUDxVOL=64. No
//   NM-HRM finding key is assigned here.
//
// MUTATION LOG (WSL/Verilator; each entry run: rebuild with the named C++
//   macro via `-CFLAGS -DMUT_X=1` -> run -> confirm only the named
//   assertion(s) changed colour -> rebuild with no flags -> confirm the
//   baseline restored; the pass/fail totals quoted in an entry are those
//   of the run that recorded it):
//
// MUTATION CHANNEL-INDEPENDENCE stimulus red: built with -DMUT_ALIAS=1,
//   redirecting every channel's write to channel 0's own AUDxDAT address
//   -> FAIL (7 passed, 2 failed), only CHANNEL-INDEPENDENCE newly red
//   (channels 1-3 never received their own write, so their audint bits
//   never fired). Rebuilt with no flags: 8 passed, 1 failed restored.
//
// MUTATION DMA-REQUEST-LATCHED-ONCE-PER-LINE stimulus red /
// MUTATION DMA-RESTART-LATCHED-ONCE-PER-LINE stimulus red /
// MUTATION LEN-REGISTER-IDENTITY stimulus red: built with
//   -DMUT_NODMAENA=1 (channel 0's own dmaena is withheld for the entire
//   DMA-mode run) -> FAIL (5 passed, 4 failed), DMA-REQUEST-LATCHED-ONCE-
//   PER-LINE, DMA-RESTART-LATCHED-ONCE-PER-LINE and LEN-REGISTER-IDENTITY
//   all newly red together (dmal/dmas never left 0, so neither ever took a
//   second distinct value, and the length comparison had nothing to
//   compare), all three sharing the one honest cause; every other
//   assertion, measured on channels/paths this mutation does not touch,
//   stayed exactly as green or red as the baseline. Rebuilt with no
//   flags: 8 passed, 1 failed restored.
//
// MUTATION LEN-REGISTER-IDENTITY stimulus red (its own, isolated
//   mutation): built with -DMUT_LEN_STIM=1, which asks the comparison run
//   for AUDxLEN=3 as well (instead of 6) -> FAIL (7 passed, 2 failed),
//   only LEN-REGISTER-IDENTITY newly red (dmas asserted 7 times at
//   AUDxLEN=3 in both runs -- no difference left to detect). Rebuilt with
//   no flags: 8 passed, 1 failed restored.
//
// MUTATION PER-REGISTER-IDENTITY stimulus red: built with
//   -DMUT_PER_STIM=1, which asks the comparison run for AUDxPER=4 as well
//   (instead of 16) -> FAIL (7 passed, 2 failed), only PER-REGISTER-
//   IDENTITY newly red. Rebuilt with no flags: 8 passed, 1 failed
//   restored.
//
// MUTATION VOL-REGISTER-IDENTITY stimulus red: built with
//   -DMUT_VOL_STIM=1, which withholds the AUDxVOL=32 write in the "on"
//   half of the measurement -> FAIL (7 passed, 2 failed), only VOL-
//   REGISTER-IDENTITY newly red (both halves read silent); VOLUME-64-
//   VERSUS-63-ON-THE-MULTIPLY-PATH, already red at baseline, stayed red
//   for its own reason. Rebuilt with no flags: 8 passed, 1 failed
//   restored.
//
// MUTATION DAT-REGISTER-IDENTITY stimulus red: built with
//   -DMUT_DAT_STIM=1, redirecting the marker write to the channel's own
//   AUDxLEN address instead of its AUDxDAT address -> FAIL (7 passed, 2
//   failed), only DAT-REGISTER-IDENTITY newly red (the marker never
//   reached auddat, so neither the high nor the low byte was ever
//   observed). Rebuilt with no flags: 8 passed, 1 failed restored.
//
// MUTATION VOLUME-65-LEVELS stimulus red: built with -DMUT_VOLLVL_STIM=1,
//   which appends AUDxVOL=100 to the measured sequence -> FAIL (7 passed,
//   2 failed), only VOLUME-65-LEVELS newly red (100 aliases 64: both read
//   256/256 open ticks, breaking the required strict increase -- bit 6
//   forcing maximum for any value at or above 64, the same rule the
//   assertion's own green case demonstrates for 64 itself). Rebuilt with
//   no flags: 8 passed, 1 failed restored.
//
// MUTATION VOLUME-64-VERSUS-63-ON-THE-MULTIPLY-PATH stimulus green: built
//   with -DMUT_MIXER_STIM=1, which re-measures the comparison at
//   AUDxVOL=10 versus 20 instead of 63 versus 64 -> PASS (9 passed, 0
//   failed), only VOLUME-64-VERSUS-63-ON-THE-MULTIPLY-PATH newly green --
//   confirming the red is scoped to the bit-6 fold path
//   (paula_audio_mixer.v:26-32) and not a broken comparison: at volume
//   values below that fold, the SAME multiply path does distinguish two
//   different settings. Rebuilt with no flags: 8 passed, 1 failed
//   restored.

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vpaula_audio.h"

// Each MUT_ flag replays exactly one assertion's own mutation record (see
// the MUTATION LOG in this file's header). Passed as
// -CFLAGS -DMUT_X=1 (a C++ macro, not a Verilog define -- Verilator would
// silently accept -DFOO as a HDL define and do nothing to this file).
#ifndef MUT_ALIAS
#define MUT_ALIAS 0
#endif
#ifndef MUT_NODMAENA
#define MUT_NODMAENA 0
#endif
#ifndef MUT_LEN_STIM
#define MUT_LEN_STIM 0
#endif
#ifndef MUT_PER_STIM
#define MUT_PER_STIM 0
#endif
#ifndef MUT_VOL_STIM
#define MUT_VOL_STIM 0
#endif
#ifndef MUT_DAT_STIM
#define MUT_DAT_STIM 0
#endif
#ifndef MUT_VOLLVL_STIM
#define MUT_VOLLVL_STIM 0
#endif
#ifndef MUT_MIXER_STIM
#define MUT_MIXER_STIM 0
#endif

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Register map, from this package's own claims file (AUD0BASE=$0A0 ..
// AUD3BASE=$0D0, each +4/+6/+8/+0xA for LEN/PER/VOL/DAT); paula_audio.v:
// 131-134's own parameters decode the same addresses.
struct ChanAddrs { int base, len, per, vol, dat; };
constexpr ChanAddrs CH[4] = {
    {0xA0, 0xA4, 0xA6, 0xA8, 0xAA},
    {0xB0, 0xB4, 0xB6, 0xB8, 0xBA},
    {0xC0, 0xC4, 0xC6, 0xC8, 0xCA},
    {0xD0, 0xD4, 0xD6, 0xD8, 0xDA},
};

int sext9(int v) { v &= 0x1FF; return (v & 0x100) ? (v | ~0x1FF) : v; }
int sext15(int v) { v &= 0x7FFF; return (v & 0x4000) ? (v | ~0x7FFF) : v; }

// Clocking of the pre-pipeline hrm/paula_audio_channel/test_paula_audio_channel.cpp
// (removed): clk7_en every 4th clk edge, cck every 2nd clk7_en tick, strhor
// pulsed at hpos==13 of a 0..226 line.
class Harness {
public:
    Harness() : dut(new Vpaula_audio) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->cck = 0;
        dut->rst = 1;
        dut->strhor = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->dmaena = 0;
        dut->audpen = 0;
        for (int i = 0; i < 32; ++i) tick();
        dut->rst = 0;
        for (int i = 0; i < 32; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        clk7_cycle = (clk7_cycle + 1) % 4;
        dut->clk7_en = (clk7_cycle == 0);
        if (dut->clk7_en) {
            cck_cycle = (cck_cycle + 1) % 2;
            dut->cck = (cck_cycle == 0);
        } else {
            dut->cck = 0;
        }
    }

    void write_reg(int addr, uint16_t data) {
        while (!dut->clk7_en) tick();
        dut->reg_address_in = addr >> 1;
        dut->data_in = data;
        tick(); tick(); tick(); tick();
        dut->reg_address_in = 0;
        dut->data_in = 0;
        tick(); tick(); tick(); tick();
    }

    // Runs one cck-valid tick; returns true if this tick was cck-valid
    // (mirrors the sibling harness's own `tick(); if (dut->cck) {...}`
    // idiom rather than re-deriving the phase relationship).
    bool tick_and_report_cck() {
        tick();
        return dut->cck;
    }

    // AUDxIR (hence audint) is assigned in paula_audio_channel's own
    // COMBINATIONAL state-transition block, not a clocked register: it can
    // be true for a window as narrow as the gap between the AUDxDAT write
    // strobe (any clk7_en edge) and the next clk7_en&&cck edge (the only
    // edge that actually advances audio_state), which can align to fall
    // entirely BETWEEN two tick()-call boundaries at a fixed write_reg
    // cadence. Sampling only at
    // tick() boundaries can therefore miss a real, RTL-genuine pulse -- a
    // harness aliasing bug, not a finding about the design. This variant
    // samples audint at every one of tick()'s three internal eval() calls
    // and ORs them, so no delta-cycle-wide assertion of audint is missed.
    int tick_capture_audint() {
        int seen = 0;
        dut->eval();      seen |= dut->audint;
        dut->clk = 1; dut->eval(); seen |= dut->audint;
        dut->clk = 0; dut->eval(); seen |= dut->audint;
        clk7_cycle = (clk7_cycle + 1) % 4;
        dut->clk7_en = (clk7_cycle == 0);
        if (dut->clk7_en) {
            cck_cycle = (cck_cycle + 1) % 2;
            dut->cck = (cck_cycle == 0);
        } else {
            dut->cck = 0;
        }
        return seen;
    }

    int okk_of(int c) const {
        return (c == 0 || c == 3) ? sext9(dut->ldata_okk) : sext9(dut->rdata_okk);
    }

    // Pulses strhor for exactly one clk7_en-gated edge. paula_audio.v:171-179
    // latches dmal/dmas `if (clk7_en) if (strhor) ...` -- clk7_en alone,
    // not cck. Holding strhor high for a whole cck period (two clk7_en
    // edges), for instance by tying strhor's level to hpos and only
    // checking it at cck ticks, double-latches:
    // the first clk7_en edge correctly captures the channel's pending
    // dmareq into dmal, but the channel's OWN strhor-triggered clear
    // (paula_audio_channel.v's own dma-request-logic block) has by then
    // reset dmareq to 0, and a SECOND clk7_en edge under the same strhor
    // level re-latches that now-cleared 0 straight back into dmal -- dmal
    // pulses to 1 for a few ticks, then silently falls back to 0 every
    // line: a harness artefact, not an RTL finding.
    void pulse_strhor_one_tick() {
        while (!dut->clk7_en) tick();
        dut->strhor = 1;
        tick();
        dut->strhor = 0;
    }

    Vpaula_audio* dut;
    int clk7_cycle = 0;
    int cck_cycle = 0;
};

// ---------------------------------------------------------------------
// CHANNEL-INDEPENDENCE
// ---------------------------------------------------------------------
bool run_channel_independence_case(int c) {
    Harness h;
    // MUT_ALIAS: every write, regardless of which channel was asked for,
    // targets channel 0's own DAT address -- for c=1,2,3 the intended
    // channel never receives its write at all.
    int target = MUT_ALIAS ? CH[0].dat : CH[c].dat;
    h.write_reg(target, 0x1234);
    int audint_seen_mask = 0;
    for (int i = 0; i < 64; ++i) audint_seen_mask |= h.tick_capture_audint();
    bool this_channel_fired = (audint_seen_mask >> c) & 1;
    bool other_channel_fired = (audint_seen_mask & ~(1 << c) & 0xF) != 0;
    return this_channel_fired && !other_channel_fired;
}

// ---------------------------------------------------------------------
// DMA-mode run: channel c in continuous DMA mode over num_lines lines,
// answering each line's latched request with an incrementing data word.
// Returns the captured traces needed by the DMA-latch and LEN-identity
// assertions so one simulation run serves all three (they are the same
// proposition space, measured together, not three separate DUTs guessing
// at the same timing three times).
// ---------------------------------------------------------------------
struct DmaRun {
    bool dmal_invariant_ok = true;
    bool dmas_invariant_ok = true;
    int dmal_distinct = 1;
    int dmas_distinct = 1;
    int dmas_assert_count = 0;
};

DmaRun run_dma_lines(int c, int len_val, int per_val, int num_lines) {
    DmaRun result;
    Harness h;
    h.write_reg(CH[c].len, static_cast<uint16_t>(len_val));
    h.write_reg(CH[c].per, static_cast<uint16_t>(per_val));
    h.write_reg(CH[c].vol, 32);
    h.dut->dmaena = MUT_NODMAENA ? 0 : (1 << c);

    uint16_t next_word = 0x1000;
    int prev_dmal = (h.dut->dmal >> c) & 1;
    int prev_dmas = (h.dut->dmas >> c) & 1;

    for (int line = 0; line < num_lines; ++line) {
        bool answered_this_line = false;
        for (int hpos = 0; hpos < 227; ++hpos) {
            // hpos==13 is the ONLY place strhor is ever pulsed -- the one
            // clk7_en edge dmal/dmas are allowed to change on. Every other
            // hpos in the line is a plain cck-valid wait with strhor low.
            if (hpos == 13) {
                h.pulse_strhor_one_tick();
            } else {
                while (!h.tick_and_report_cck()) {}
            }
            int cur_dmal = (h.dut->dmal >> c) & 1;
            int cur_dmas = (h.dut->dmas >> c) & 1;
            bool tolerated = (hpos == 13);
            if (!tolerated) {
                if (cur_dmal != prev_dmal) result.dmal_invariant_ok = false;
                if (cur_dmas != prev_dmas) result.dmas_invariant_ok = false;
            }
            if (cur_dmal != prev_dmal) ++result.dmal_distinct;
            if (cur_dmas && !prev_dmas) ++result.dmas_assert_count;
            if (cur_dmas != prev_dmas) ++result.dmas_distinct;
            prev_dmal = cur_dmal;
            prev_dmas = cur_dmas;

            // Answer a latched request once per line, shortly after strhor,
            // before the next line's strhor comes around. write_reg's own
            // internal ticks never touch strhor, so dmal/dmas (which only
            // update `if (strhor)`) cannot change during it.
            if (hpos == 14 && !answered_this_line && cur_dmal) {
                h.write_reg(CH[c].dat, next_word++);
                answered_this_line = true;
            }
        }
    }
    return result;
}

// ---------------------------------------------------------------------
// Direct-mode helpers (PER/VOL/DAT identity, volume-level measurements)
// ---------------------------------------------------------------------

// Writes AUDxDAT directly (dmaena held 0) and ticks forward far enough to
// clear the one-cycle write->AUDxDAT-flag->state-transition pipeline
// documented in this file's own header, landing safely inside the first
// high-byte (state 3) phase.
void enter_direct_mode(Harness& h, int dat_addr, uint16_t marker) {
    h.write_reg(dat_addr, marker);
    for (int i = 0; i < 16; ++i) h.tick(); // settle past the write->state3 pipeline
}

// PER-REGISTER-IDENTITY: measures how many consecutive cck ticks the
// channel's own PWM output holds the high-byte value before switching to
// the low-byte value.
int measure_high_byte_hold_ticks(int c, int per_val) {
    Harness h;
    h.write_reg(CH[c].vol, 32);
    h.write_reg(CH[c].per, static_cast<uint16_t>(per_val));
    enter_direct_mode(h, CH[c].dat, 0x1234);
    int first = h.okk_of(c);
    int hold = 0;
    for (int i = 0; i < 4000; ++i) {
        bool is_cck = h.tick_and_report_cck();
        if (!is_cck) continue;
        if (h.okk_of(c) != first) break;
        ++hold;
    }
    return hold;
}

struct VolResult { bool off_silent; bool on_nonzero; };

VolResult measure_vol_gate(int c) {
    Harness h;
    h.write_reg(CH[c].per, 8);
    // AUDxVOL left at its reset value (0) for the "off" half of this
    // measurement -- MUT_VOL_STIM additionally skips the "on" write below,
    // so both halves become the silent case.
    enter_direct_mode(h, CH[c].dat, 0x4040);
    bool off_silent = true;
    for (int i = 0; i < 512; ++i) {
        bool is_cck = h.tick_and_report_cck();
        if (is_cck && h.okk_of(c) != 0) off_silent = false;
    }

    Harness h2;
    h2.write_reg(CH[c].per, 8);
    if (!MUT_VOL_STIM) h2.write_reg(CH[c].vol, 32);
    enter_direct_mode(h2, CH[c].dat, 0x4040);
    bool on_nonzero = false;
    for (int i = 0; i < 512; ++i) {
        bool is_cck = h2.tick_and_report_cck();
        if (is_cck && h2.okk_of(c) != 0) on_nonzero = true;
    }
    return {off_silent, on_nonzero};
}

struct DatResult { bool correct_shows_marker; bool wrong_addr_silent; };

DatResult measure_dat_identity(int c) {
    // Correct-address case (subject to MUT_DAT_STIM, which redirects this
    // write to LEN's own address instead).
    Harness h;
    h.write_reg(CH[c].per, 8);
    h.write_reg(CH[c].vol, 32);
    int target = MUT_DAT_STIM ? CH[c].len : CH[c].dat;
    h.write_reg(target, 0x1234);
    bool saw_hi = false, saw_lo = false;
    for (int i = 0; i < 4000; ++i) {
        bool is_cck = h.tick_and_report_cck();
        if (!is_cck) continue;
        int v = h.okk_of(c);
        if (v == 0x12) saw_hi = true;
        if (v == 0x34 && saw_hi) saw_lo = true;
        if (saw_hi && saw_lo) break;
    }

    // Built-in control case (not a mutation): the identical marker written
    // to this channel's own LEN address must produce nothing at all.
    Harness h3;
    h3.write_reg(CH[c].per, 8);
    h3.write_reg(CH[c].vol, 32);
    h3.write_reg(CH[c].len, 0x1234);
    bool wrong_addr_silent = true;
    for (int i = 0; i < 512; ++i) {
        bool is_cck = h3.tick_and_report_cck();
        if (is_cck && h3.okk_of(c) != 0) wrong_addr_silent = false;
    }
    return {saw_hi && saw_lo, wrong_addr_silent};
}

// VOLUME-65-LEVELS: counts, over a 256-cck (4x64) window, how many of
// those ticks the PWM gate is open, for a fixed nonzero sample.
int measure_open_ticks_pwm(int vol_val) {
    Harness h;
    h.write_reg(CH[0].vol, static_cast<uint16_t>(vol_val));
    h.write_reg(CH[0].per, 8);
    enter_direct_mode(h, CH[0].dat, 0x7F7F);
    // Clear the initial transient (a handful of period cycles) so the
    // measurement window sits inside the free-running 64-tick gate cycle.
    for (int i = 0; i < 400; ++i) h.tick_and_report_cck();
    int open = 0, counted = 0;
    while (counted < 256) {
        bool is_cck = h.tick_and_report_cck();
        if (!is_cck) continue;
        if (h.okk_of(0) != 0) ++open;
        ++counted;
    }
    return open;
}

// VOLUME-64-VERSUS-63-ON-THE-MULTIPLY-PATH: settled ldata reading for a
// fixed nonzero sample at the given AUDxVOL.
int measure_ldata_settled(int vol_val) {
    Harness h;
    h.write_reg(CH[0].vol, static_cast<uint16_t>(vol_val));
    h.write_reg(CH[0].per, 8);
    enter_direct_mode(h, CH[0].dat, 0x4040);
    for (int i = 0; i < 200; ++i) h.tick_and_report_cck();
    return sext15(h.dut->ldata);
}

} // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // --- CHANNEL-INDEPENDENCE -------------------------------------------------
    bool indep_ok = true;
    for (int c = 0; c < 4; ++c) indep_ok &= run_channel_independence_case(c);
    checks.expect(indep_ok,
        "CHANNEL-INDEPENDENCE (HRM p.164): a direct-mode AUDxDAT write on each "
        "channel alone raises only that channel's own audint bit, all four "
        "channels independently checked -- 5-p164.c1, 5-sm.c1");

    // --- DMA-mode run (channel 0), used by three assertions --------------------
    DmaRun dma_len3 = run_dma_lines(0, /*len=*/3, /*per=*/8, /*lines=*/20);
    // MUT_LEN_STIM: the comparison run is asked for AUDxLEN=3 as well (a
    // genuine stimulus change, not a hard-coded verdict), so both runs
    // measure the same length and the "strictly more often" claim below has
    // no real difference left to detect.
    DmaRun dma_len_cmp = run_dma_lines(0, /*len=*/MUT_LEN_STIM ? 3 : 6, /*per=*/8, /*lines=*/20);

    checks.expect(dma_len3.dmal_invariant_ok && dma_len3.dmal_distinct >= 2,
        "DMA-REQUEST-LATCHED-ONCE-PER-LINE (HRM p.164): channel 0's dmal bit "
        "changes value only in the window around a strhor pulse (never mid-"
        "line) across 20 simulated lines, and takes at least two distinct "
        "values (not vacuously constant) -- 5-p164.c29, 5-sm.c30");

    checks.expect(dma_len3.dmas_invariant_ok && dma_len3.dmas_distinct >= 2,
        "DMA-RESTART-LATCHED-ONCE-PER-LINE (HRM p.164): channel 0's dmas bit "
        "changes value only in the window around a strhor pulse across 20 "
        "simulated lines, and takes at least two distinct values -- "
        "5-p164.c30, 5-sm.c30");

    checks.note("dmas asserted " + std::to_string(dma_len3.dmas_assert_count) +
        " times over 20 lines at AUDxLEN=3, versus " +
        std::to_string(dma_len_cmp.dmas_assert_count) + " times at AUDxLEN=" +
        (MUT_LEN_STIM ? "3 (mutated: both runs used the same value)" : "6") + ".");

    bool len_ok = dma_len3.dmas_assert_count > dma_len_cmp.dmas_assert_count;
    checks.expect(len_ok,
        "LEN-REGISTER-IDENTITY: over a fixed 20 simulated lines of continuous "
        "DMA playback, channel 0's dmas (restart) flag asserts strictly more "
        "often at AUDxLEN=3 than at AUDxLEN=6 -- the length register sets how "
        "soon the channel restarts, on the manual's own words -- "
        "5-appA-p266.c14, 5-appA-p266.c16, 5-appA-p266.c17, A-audxlen.addr, "
        "A-audxlen.chip, 5-appB-p290.c33, 5-appB-p290.c35, 5-appB-p290.c36, "
        "B-aud0len.addr, B-aud0len.chip, 5-appB-p290.c68, 5-appB-p290.c70, "
        "5-appB-p290.c71, B-aud1len.addr, B-aud1len.chip, 5-appB-p290.c103, "
        "5-appB-p290.c105, 5-appB-p290.c106, B-aud2len.addr, B-aud2len.chip, "
        "5-appB-p290.c138, 5-appB-p290.c140, 5-appB-p290.c141, B-aud3len.addr, "
        "B-aud3len.chip (25 ids, one register instance measured functionally, "
        "the other three channels' own instances covered by the identical "
        "address-family pattern already exercised in CHANNEL-INDEPENDENCE and "
        "DAT-REGISTER-IDENTITY below)");

    // --- PER-REGISTER-IDENTITY --------------------------------------------------
    int hold_per4 = measure_high_byte_hold_ticks(0, 4);
    int hold_per16 = MUT_PER_STIM ? measure_high_byte_hold_ticks(0, 4) // same value: no difference to measure
                                  : measure_high_byte_hold_ticks(0, 16);
    checks.note("high-byte hold: " + std::to_string(hold_per4) + " cck ticks at "
        "AUDxPER=4, " + std::to_string(hold_per16) + " at AUDxPER=" +
        (MUT_PER_STIM ? std::string("4 (mutated: both runs used the same value)") : std::string("16")) + ".");
    checks.expect(hold_per4 < hold_per16,
        "PER-REGISTER-IDENTITY: the high-byte (state 3) output holds for "
        "strictly more cck ticks at a larger AUDxPER than at a smaller one, "
        "for all four channels' own identically-shaped register (channel 0 "
        "measured directly; channels 1-3 share the same per-channel address "
        "pattern already confirmed independent in CHANNEL-INDEPENDENCE) -- "
        "5-appA-p266.c19, 5-appA-p266.c21, 5-appA-p266.c22, A-audxper.addr, "
        "A-audxper.chip, 5-appB-p290.c38, 5-appB-p290.c40, 5-appB-p290.c41, "
        "B-aud0per.addr, B-aud0per.chip, 5-appB-p290.c73, 5-appB-p290.c75, "
        "5-appB-p290.c76, B-aud1per.addr, B-aud1per.chip, 5-appB-p290.c108, "
        "5-appB-p290.c110, 5-appB-p290.c111, B-aud2per.addr, B-aud2per.chip, "
        "5-appB-p290.c143, 5-appB-p290.c145, 5-appB-p290.c146, B-aud3per.addr, "
        "B-aud3per.chip, 5-appC-p298.c7, 5-appC-p298.c10, C-map.per.addr, "
        "C-map.per.chip, 5-appC-p310.c2, 5-appC-p310.c4, C-interp.per.row (32 ids)");

    // --- VOL-REGISTER-IDENTITY (looped over all four channels) -----------------
    bool vol_ok = true;
    for (int c = 0; c < 4; ++c) {
        VolResult r = measure_vol_gate(c);
        vol_ok &= (r.off_silent && r.on_nonzero);
    }
    checks.expect(vol_ok,
        "VOL-REGISTER-IDENTITY: with AUDxVOL at its reset value the channel's "
        "own PWM output reads zero at every sampled tick; written to a mid-"
        "range value the same channel's output reads nonzero at some sampled "
        "tick -- checked on all four channels' own instances -- "
        "5-appA-p266.c27, 5-appA-p266.c29, 5-appA-p266.c30, A-audxvol.addr, "
        "A-audxvol.chip, 5-appB-p290.c43, 5-appB-p290.c45, 5-appB-p290.c46, "
        "B-aud0vol.addr, B-aud0vol.chip, 5-appB-p290.c78, 5-appB-p290.c80, "
        "5-appB-p290.c81, B-aud1vol.addr, B-aud1vol.chip, 5-appB-p290.c113, "
        "5-appB-p290.c115, 5-appB-p290.c116, B-aud2vol.addr, B-aud2vol.chip, "
        "5-appB-p291.c1, 5-appB-p291.c3, 5-appB-p291.c4, B-aud3vol.addr, "
        "B-aud3vol.chip (25 ids)");

    // --- DAT-REGISTER-IDENTITY (looped over all four channels) -----------------
    bool dat_ok = true;
    for (int c = 0; c < 4; ++c) {
        DatResult r = measure_dat_identity(c);
        dat_ok &= (r.correct_shows_marker && r.wrong_addr_silent);
    }
    checks.expect(dat_ok,
        "DAT-REGISTER-IDENTITY: a two-byte marker written to the channel's "
        "own AUDxDAT address is observed high-byte-then-low-byte at that "
        "channel's own PWM output; the identical marker written to the SAME "
        "channel's AUDxLEN address instead (control case) produces no output "
        "at all -- checked on all four channels' own instances -- "
        "5-appA-p265.c33, 5-appA-p265.c35, 5-appA-p265.c36, 5-appA-p265.c37, "
        "A-audxdat.addr, A-audxdat.chip, 5-appB-p290.c48, 5-appB-p290.c50, "
        "5-appB-p290.c51, B-aud0dat.addr, B-aud0dat.chip, 5-appB-p290.c83, "
        "5-appB-p290.c85, 5-appB-p290.c86, B-aud1dat.addr, B-aud1dat.chip, "
        "5-appB-p290.c118, 5-appB-p290.c120, 5-appB-p290.c121, B-aud2dat.addr, "
        "B-aud2dat.chip, 5-appB-p291.c6, 5-appB-p291.c8, 5-appB-p291.c9, "
        "B-aud3dat.addr, B-aud3dat.chip (26 ids)");

    // --- VOLUME-65-LEVELS (PWM path) --------------------------------------------
    std::vector<int> vol_values = {0, 1, 32, 63, 64};
    if (MUT_VOLLVL_STIM) vol_values.push_back(100); // 100 aliases 64 (bit 6 set): breaks strict increase
    std::vector<int> opens;
    for (int v : vol_values) opens.push_back(measure_open_ticks_pwm(v));
    {
        std::string trace;
        for (size_t i = 0; i < vol_values.size(); ++i)
            trace += "AUDxVOL=" + std::to_string(vol_values[i]) + "->" + std::to_string(opens[i]) + "/256 ticks open; ";
        checks.note(trace);
    }
    bool exact_ok = (opens[0] == 0) && (opens[1] == 4) && (opens[2] == 128) &&
                    (opens[3] == 252) && (opens[4] == 256);
    bool increasing_ok = true;
    for (size_t i = 1; i < opens.size(); ++i)
        if (!(opens[i] > opens[i - 1])) increasing_ok = false;
    checks.expect(exact_ok && increasing_ok,
        "VOLUME-65-LEVELS (PWM path, ldata_okk/rdata_okk): open-tick count "
        "over a 256-cck window is exactly 4x the written AUDxVOL for 0, 1, "
        "32, 63, and strictly increasing throughout, with AUDxVOL=64 giving "
        "256/256 (all ones, no zeros, per any 64-tick slice) -- appendix A's "
        "65 linear levels and its bit-6-forces-max rule -- 5-appA-p266.c32, "
        "5-appA-p266.c34, A-audxvol.c3");

    // --- VOLUME-64-VERSUS-63-ON-THE-MULTIPLY-PATH (expected RED) ---------------
    int mix_a = MUT_MIXER_STIM ? measure_ldata_settled(10) : measure_ldata_settled(63);
    int mix_b = MUT_MIXER_STIM ? measure_ldata_settled(20) : measure_ldata_settled(64);
    checks.note("ldata settled value: " + std::to_string(mix_a) + " at AUDxVOL=" +
        (MUT_MIXER_STIM ? "10" : "63") + ", " + std::to_string(mix_b) + " at AUDxVOL=" +
        (MUT_MIXER_STIM ? "20" : "64") + ".");
    checks.expect(mix_a != mix_b,
        "VOLUME-64-VERSUS-63-ON-THE-MULTIPLY-PATH (ldata/rdata): the manual's "
        "own 65-distinct-levels claim requires AUDxVOL=63 and 64 to produce "
        "different output; measured here at paula_audio's multiply path they "
        "do not (paula_audio_mixer.v folds bit 6 into all six lower bits "
        "before the multiplier). EXPECTED RED -- see this file's own header; "
        "VOLUME-65-LEVELS above, measured on the identical harness at the "
        "OTHER path, is the control proving this is not a units error -- "
        "5-appA-p266.c32, A-audxvol.c3");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
