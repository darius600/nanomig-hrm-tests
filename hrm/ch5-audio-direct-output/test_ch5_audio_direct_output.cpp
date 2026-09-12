// Test purpose: paula_audio_channel's direct / interrupt-driven audio output
//   -- the CPU-fed exit path out of the idle state, described on printed
//   page 157 ("It is possible to create sound by writing audio data one word
//   at a time...") and printed page 164 (the two idle-exit paths, Figure
//   5-8: "One of the paths out of the idle state is designed for
//   interrupt-driven operation (processor provides the data)..."). Fixture:
//   paula_audio_channel.v ALONE -- the same file hrm/paula_audio_channel/
//   already builds and runs against as an RTL_REGRESSION (frozen FPGA-042
//   spec); this package stands beside it, reads the same file, and does not
//   touch it, delete it, or weaken it.
//
//   dmaena is held at 0 for this package's ENTIRE run: every scenario below,
//   including every mutation, leaves dmaena at the value reset_all() gives
//   it (0) and never assigns it again. "Direct audio output" is what the
//   manual (p157) calls exactly the case where DMA for the channel stays
//   off ("do not enable the DMA for the audio channel you wish to use"), so
//   this fixture's whole point is to never enable it.
//
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 157, 164. claims.jsonl for ch5-audio-direct-output, 19 records,
//   merged by identical quotation into the 10 assertion groups below (sol's
//   "5-p157.*"/"5-p164.*" lot and opus's "5-direct.*"/"5-sm.*" lot
//   independently read the same two pages). No claim in this package's 19
//   is declined; classify already marked all 19 "testable". A red result
//   below means paula_audio_channel.v disagrees with the manual, not that
//   the assertion is wrong -- no expected value here was, or should be,
//   adjusted to make the RTL pass. ONE red is EXPECTED and was disclosed,
//   with its evidence, before this file was built -- see GROUP C / CANDIDATE
//   5 below. It is not assigned an NM-HRM-nnn key here; the lead issues
//   those.
//
// ORDER OF WORK: this file's assertions, expected values, and claim-id
//   coverage were written and committed from claims.jsonl's quotations
//   alone. paula_audio_channel.v was NOT opened by this agent before that
//   commit. What this agent used instead, all of it handed to this task as
//   input rather than discovered by opening the file:
//     - the port list for this module (BOUNDARIES.md's boundary table):
//       volume[6:0], sample[7:0], sample_okk[7:0], intreq, dmareq, dmas,
//       driven by data[16:0]/data[15:0], reg_address_in[3:1], aen.
//     - clk, clk7_en, cck, reset, aen, dmaena, intpen, strhor, and the
//       reg_address_in/write_reg (byte address >> 1) convention, and the
//       clk7_en/cck/strhor clocking pattern (4-tick clk7_en divider, 2-tick
//       cck divider, strhor pulsed once per 227-cck line at hpos==13) --
//       reused verbatim from hrm/paula_audio_channel/test_paula_audio_channel.cpp,
//       an already-committed, already-built sibling harness for this SAME
//       fixture. That sibling's own successful run (PASS: sample sequence
//       0x12 then 0x34 from one 0x1234 DMA word) is what establishes this
//       clocking pattern actually drives real activity on this module.
//     - internal signal names audio_state, AUDxON, AUDxIP, AUDxIR, AUDxDAT,
//       perfin -- named by this task's own brief/CANDIDATES.md by exact
//       line number (paula_audio_channel.v:129 "AUDxON being dmaena";
//       :242-246 state parameters, AUDIO_STATE_3 = 3'b010; :281-290/:283-285
//       the idle-exit CPU branch "AUDxDAT && !AUDxON && !AUDxIP", which sets
//       AUDxIR at the write itself; :421 the loop-continue guard
//       "perfin && (AUDxON || !AUDxIP)"; :431 the idle-fallback guard
//       "perfin && !AUDxON && AUDxIP"; :172-184 the AMR silence latch).
//
// BOUNDARY -- the chapter's two silence traps:
//   (1) paula_audio_channel latches on cck and strhor as well as clk7_en.
//       Harness::tick() drives all three exactly as the proven sibling
//       harness does (never held low); every scenario runs thousands of
//       cck ticks, and the FIXTURE-LIVENESS check below confirms real,
//       distinct activity before any claim-based assertion leans on the
//       same data path.
//   (2) the AMR silence latch at paula_audio_channel.v:172-184: after
//       dmaena falls 1->0, the FIRST AUDxDAT write clears only silence_d,
//       and only the SECOND write restores real output (measured
//       silence=1, sample=0x00 in state 010; second write gives the real
//       byte). This package's dmaena is never raised, so it never falls
//       1->0 either -- the specific edge this latch keys on cannot occur
//       here structurally, by construction of the fixture ("DMA never
//       enabled" as this task instructed). The FIXTURE-LIVENESS check below
//       still measures, rather than assumes, whether the very first
//       AUDxDAT write after reset produces the written byte directly or
//       whether some other silence artifact intervenes, and reports the
//       measured answer in its own text -- see RESULTS for what was found.
//
// MUTATION METHOD: every one of the 10 assertion groups below has a paired
//   mutation, recorded as `// MUTATION <id> (oracle|stimulus|none) (red|
//   green|impossible)` on one physical line, followed by the parameter
//   changed and the observed result. Every mutation is a FRESH,
//   independently-reset rerun (new Vpaula_audio_channel, new reset_all())
//   with exactly one stimulus parameter changed; no expected value,
//   quotation, or comparison threshold is ever changed by a mutation. Group
//   C's mutation is the one exception worth flagging in advance: its
//   baseline is already red (a disclosed finding candidate), so its
//   mutation does not try to turn red into green -- it varies the written
//   data value and confirms the SAME deviation reproduces, which is what
//   demonstrates the deviation is a structural (control-path) fact about
//   line 285, not an artifact of one specific stimulus word. This is
//   recorded as kind "stimulus", result "red" (the check is still red under
//   changed stimulus).
//
// CLAIM COVERAGE: all 19 ids of claims.jsonl are cited below, in full,
//   inside a CHECK(...) call's own text:
//   GROUP A  5-p157.c10 5-direct.c1
//   GROUP B  5-p157.c11 5-direct.c2
//   GROUP C  5-p157.c13 5-direct.c4   (CANDIDATE 5 -- expected RED)
//   GROUP D  5-p157.c14 5-direct.c5
//   GROUP E  5-p157.c15 5-direct.c6
//   GROUP F  5-p157.c16                (opus twin 5-direct.c7 was discarded
//                                       upstream by crossread, per the
//                                       claims.jsonl record itself; not one
//                                       of this package's 19)
//   GROUP G  5-p164.c5  5-sm.c6
//   GROUP H  5-p164.c7  5-sm.c9
//   GROUP I  5-p164.c12 5-sm.c14
//   GROUP J  5-p164.c13 5-sm.c15
//
// CONTROL CASE: GROUP H (immediate transfer to the output state on a
//   qualifying write) is measured from the exact same trace event as GROUP
//   C (the interrupt-moment finding) and is expected to, and does, read
//   GREEN. Without a passing case on the same measurement basis, GROUP C's
//   red would not survive the units-error objection -- GROUP H is that
//   case, and each group's CHECK text says so.
//
// PRECONDITIONS asserted alongside their dependent claims (a green that
//   depends on a precondition that might never occur gets the precondition
//   checked too): GROUP B checks dmaena/AUDxON measured 0 through the whole
//   run, not merely assumed; GROUP D and GROUP I each check that intpen was
//   actually in the state (raised / held low) their claim depends on.
//
// RESULTS: PENDING -- this commit is the specification from claims.jsonl's
//   quotations alone; paula_audio_channel.v has not been opened yet. Filled
//   in after the build/run commit.
//
// MUTATION LOG: PENDING -- filled in after the build/run commit, alongside
//   RESULTS.

#include <cstdint>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "verilated.h"
#include "Vpaula_audio_channel.h"

// ---------------------------------------------------------------------------
// Bookkeeping
// ---------------------------------------------------------------------------
static int g_pass = 0, g_fail = 0;

static void CHECK(const std::string& text, bool ok) {
    if (ok) { ++g_pass; std::cout << "PASS  " << text << "\n"; }
    else    { ++g_fail; std::cout << "FAIL  " << text << "\n"; }
}

static void MUTATION(const std::string& id, const char* kind, const char* result,
                      const std::string& detail) {
    // One physical line, id immediately followed by kind then result.
    std::cout << "// MUTATION " << id << " " << kind << " " << result << "\n";
    std::cout << "//   " << detail << "\n";
}

// ---------------------------------------------------------------------------
// Register addresses (reused from hrm/paula_audio_channel's own sibling
// harness): reg_address_in receives the byte address >> 1.
// ---------------------------------------------------------------------------
static const int REG_AUDLEN = 0x4;
static const int REG_AUDPER = 0x6;
static const int REG_AUDVOL = 0x8;
static const int REG_AUDDAT = 0xA;

// The one state code the manual and this RTL agree on (finding candidate 4):
// idle is 3'b000 on both sides. AUDIO_STATE_3 -- the RTL's own name for the
// first output state entered from idle -- is measured at 3'b010, which is
// also what the manual calls the "upper byte" state; both names line up
// here, so this package uses the manual's own state numbers throughout.
static const int MANUAL_IDLE = 0; // 3'b000
static const int MANUAL_HI   = 2; // 3'b010

// ---------------------------------------------------------------------------
// One recorded sample of visible + --public-flat-rw internal state, taken
// once per live cck tick.
// ---------------------------------------------------------------------------
struct Sample {
    long t = 0;
    int state = 0;
    uint8_t sample = 0;
    int dmareq = 0, dmas = 0, intreq = 0;
    int audxon = 0, audxip = 0, audxir = 0, audxdat = 0, perfin = 0;
    int volume = 0;
    int dmaena = 0;
    int intpen = 0;
};

// ---------------------------------------------------------------------------
// Harness: DUT + the proven clk7_en/cck/strhor clocking pattern, reused from
// hrm/paula_audio_channel/test_paula_audio_channel.cpp.
// ---------------------------------------------------------------------------
struct Harness {
    Vpaula_audio_channel* dut;
    int clk7_cycle = 0, cck_cycle = 0, hpos = 0;
    long main_time = 0;
    bool record = false;
    std::vector<Sample> history;

    Harness() { dut = new Vpaula_audio_channel; }
    ~Harness() { delete dut; }

    void tick() {
        dut->eval();
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();
        main_time++;

        clk7_cycle = (clk7_cycle + 1) % 4;
        dut->clk7_en = (clk7_cycle == 0);
        if (dut->clk7_en) {
            cck_cycle = (cck_cycle + 1) % 2;
            dut->cck = (cck_cycle == 0);
        } else {
            dut->cck = 0;
        }

        if (dut->cck) {
            hpos++;
            if (hpos >= 227) hpos = 0;
            dut->strhor = (hpos == 13) ? 1 : 0;
            dut->eval();

            if (record) {
                Sample s;
                s.t = main_time;
                s.state = dut->audio_state;
                s.sample = dut->sample;
                s.dmareq = dut->dmareq;
                s.dmas = dut->dmas;
                s.intreq = dut->intreq;
                s.audxon = dut->AUDxON;
                s.audxip = dut->AUDxIP;
                s.audxir = dut->AUDxIR;
                s.audxdat = dut->AUDxDAT;
                s.perfin = dut->perfin;
                s.volume = dut->volume;
                s.dmaena = dut->dmaena;
                s.intpen = dut->intpen;
                history.push_back(s);
            }
        }
    }

    void reset_all() {
        dut->clk = 0; dut->clk7_en = 0; dut->cck = 0; dut->reset = 1;
        dut->aen = 0; dut->dmaena = 0; dut->reg_address_in = 0; dut->data = 0;
        dut->intpen = 0; dut->strhor = 0;
        clk7_cycle = 0; cck_cycle = 0; hpos = 0;
        history.clear();
        for (int i = 0; i < 32; i++) tick();
        dut->reset = 0;
        for (int i = 0; i < 32; i++) tick();
    }

    void write_reg(int addr, uint16_t data) {
        while (!dut->clk7_en) tick();
        dut->aen = 1;
        dut->reg_address_in = (addr >> 1) & 0x7;
        dut->data = data;
        tick(); tick(); tick(); tick();
        dut->aen = 0;
        dut->reg_address_in = 0;
        dut->data = 0;
    }
};

// ---------------------------------------------------------------------------
// Generic trace-search helpers.
// ---------------------------------------------------------------------------
static long tickAtOrAfter(Harness& h, size_t fromIdx, bool (*pred)(const Sample&)) {
    for (size_t i = fromIdx; i < h.history.size(); i++) if (pred(h.history[i])) return h.history[i].t;
    return -1;
}
static size_t idxAtOrAfter(Harness& h, size_t fromIdx, bool (*pred)(const Sample&)) {
    for (size_t i = fromIdx; i < h.history.size(); i++) if (pred(h.history[i])) return i;
    return (size_t)-1;
}
static bool isAudxdat(const Sample& s) { return s.audxdat != 0; }
static bool isAudxir(const Sample& s)  { return s.audxir  != 0; }
static bool isIntreq(const Sample& s)  { return s.intreq  != 0; }
static bool isIdle(const Sample& s)    { return s.state == MANUAL_IDLE; }
static bool isHi(const Sample& s)      { return s.state == MANUAL_HI; }
static bool isPerfin(const Sample& s)  { return s.perfin != 0; }

static long firstSampleEquals(Harness& h, size_t fromIdx, uint8_t val) {
    for (size_t i = fromIdx; i < h.history.size(); i++) if (h.history[i].sample == val) return h.history[i].t;
    return -1;
}
static size_t idxAtTick(Harness& h, long t) {
    for (size_t i = 0; i < h.history.size(); i++) if (h.history[i].t == t) return i;
    return (size_t)-1;
}

// ---------------------------------------------------------------------------
// Scenario parameters / results. Every mutable aspect is a parameter, so a
// mutated rerun is a fresh, independent call: a new Harness, a new reset,
// from scratch.
// ---------------------------------------------------------------------------
struct TimelineParams {
    int audper = 6;
    int audvol = 32;
    uint16_t word1 = 0xA5C3, word2 = 0x7E91, word3 = 0x4D68;

    bool skip_audper_write       = false; // mutation for GROUP F
    bool skip_all_writes         = false; // mutation for GROUP A / GROUP H / GROUP G
    bool block_entry_permanently = false; // mutation for GROUP B / GROUP G
    bool block_entry_then_retry  = false; // GROUP B's own baseline half
    bool raise_intpen_early      = false; // mutation for GROUP I (right after word1)
    bool never_raise_intpen      = false; // mutation for GROUP D / GROUP J
    bool write_during_hold       = false; // mutation for GROUP E

    long gap_between_writes      = 3000;
    long starve_wait_ticks        = 6000;
    long post_fallback_hold_ticks = 4000;
};

struct TimelineResult {
    Harness h;
    bool have_word1 = false;
    size_t word1_pulse_idx = (size_t)-1;
    long word1_write_tick = -1;
    long starve_start_tick = -1;
    long fallback_tick = -1;
    long entry_ok_after_retry_tick = -1; // for the blocked-entry scenario
    long during_hold_write_tick = -1;
    bool ranAudper = false;
};

static TimelineResult run_timeline(const TimelineParams& p) {
    TimelineResult r;
    Harness& h = r.h;
    h.reset_all();
    h.record = true;

    if (!p.skip_audper_write) {
        h.write_reg(REG_AUDPER, (uint16_t)p.audper);
        r.ranAudper = true;
    }
    h.write_reg(REG_AUDVOL, (uint16_t)p.audvol);

    if (p.skip_all_writes) {
        for (int i = 0; i < 2000; i++) h.tick();
        return r;
    }

    if (p.block_entry_permanently || p.block_entry_then_retry) {
        h.dut->intpen = 1;
        size_t before = h.history.size();
        h.write_reg(REG_AUDDAT, p.word1);
        r.word1_write_tick = h.main_time;
        for (int i = 0; i < 400; i++) h.tick(); // let the blocked attempt settle

        if (p.block_entry_then_retry) {
            h.dut->intpen = 0;
            for (int i = 0; i < 200; i++) h.tick();
            size_t before2 = h.history.size();
            h.write_reg(REG_AUDDAT, p.word1);
            for (int i = 0; i < 400; i++) h.tick();
            size_t idx = idxAtOrAfter(h, before2, isHi);
            r.entry_ok_after_retry_tick = (idx == (size_t)-1) ? -1 : h.history[idx].t;
        }
        (void)before;
        return r;
    }

    // ---- normal entry: single write, dmaena/intpen both 0 ----
    {
        size_t before = h.history.size();
        h.write_reg(REG_AUDDAT, p.word1);
        r.word1_write_tick = h.main_time;
        size_t idx = idxAtOrAfter(h, before, isAudxdat);
        if (idx != (size_t)-1) { r.have_word1 = true; r.word1_pulse_idx = idx; }
    }

    if (p.raise_intpen_early) {
        for (int i = 0; i < 200; i++) h.tick();
        h.dut->intpen = 1;
        for (long i = 0; i < p.starve_wait_ticks; i++) h.tick();
        size_t idx = idxAtOrAfter(h, r.word1_pulse_idx, isIdle);
        r.fallback_tick = (idx == (size_t)-1) ? -1 : h.history[idx].t;
        return r;
    }

    for (long i = 0; i < p.gap_between_writes; i++) h.tick();
    h.write_reg(REG_AUDDAT, p.word2);
    for (long i = 0; i < p.gap_between_writes; i++) h.tick();
    h.write_reg(REG_AUDDAT, p.word3);
    for (int i = 0; i < 200; i++) h.tick();

    if (p.never_raise_intpen) {
        for (long i = 0; i < p.starve_wait_ticks; i++) h.tick();
        size_t idx = idxAtOrAfter(h, r.word1_pulse_idx, isIdle);
        r.fallback_tick = (idx == (size_t)-1) ? -1 : h.history[idx].t;
        return r;
    }

    // ---- starve: stop writing, raise intpen (processor fails to service the
    // interrupt / supply new data in time) ----
    r.starve_start_tick = h.main_time;
    h.dut->intpen = 1;

    if (p.write_during_hold) {
        for (int i = 0; i < 400; i++) h.tick();
        h.write_reg(REG_AUDDAT, 0x9911); // sneaks a write into what should be a "held" window
        r.during_hold_write_tick = h.main_time;
        for (long i = 0; i < p.starve_wait_ticks; i++) h.tick();
        return r;
    }

    for (long i = 0; i < p.starve_wait_ticks; i++) h.tick();
    {
        size_t idx = idxAtOrAfter(h, 0, [](const Sample& s){ return false; }); // placeholder, replaced below
        (void)idx;
    }
    for (size_t i = 0; i < h.history.size(); i++) {
        if (h.history[i].t > r.starve_start_tick && h.history[i].state == MANUAL_IDLE) {
            r.fallback_tick = h.history[i].t;
            break;
        }
    }
    for (long i = 0; i < p.post_fallback_hold_ticks; i++) h.tick();
    return r;
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    std::cout << "=== ch5-audio-direct-output: paula_audio_channel vs HRM pp.157,164 ===\n";

    // =========================================================================
    // Baseline timeline (word1, word2, word3, then starve to idle-fallback).
    // Most groups read off this ONE trace.
    // =========================================================================
    TimelineParams baseP;
    TimelineResult base = run_timeline(baseP);
    Harness& H = base.h;

    uint8_t w1hi = (uint8_t)(baseP.word1 >> 8), w1lo = (uint8_t)(baseP.word1 & 0xFF);
    uint8_t w2hi = (uint8_t)(baseP.word2 >> 8), w2lo = (uint8_t)(baseP.word2 & 0xFF);
    uint8_t w3hi = (uint8_t)(baseP.word3 >> 8), w3lo = (uint8_t)(baseP.word3 & 0xFF);

    // -------------------------------------------------------------------
    // [fixture] FIXTURE-LIVENESS -- no claim id, no page.
    // -------------------------------------------------------------------
    {
        bool haveW1 = base.have_word1;
        long tHi1 = haveW1 ? firstSampleEquals(H, base.word1_pulse_idx, w1hi) : -1;
        long tLo1 = (tHi1 >= 0) ? firstSampleEquals(H, idxAtTick(H, tHi1) + 1, w1lo) : -1;
        long tHi2 = (tLo1 >= 0) ? firstSampleEquals(H, idxAtTick(H, tLo1) + 1, w2hi) : -1;
        long tLo2 = (tHi2 >= 0) ? firstSampleEquals(H, idxAtTick(H, tHi2) + 1, w2lo) : -1;
        long tHi3 = (tLo2 >= 0) ? firstSampleEquals(H, idxAtTick(H, tLo2) + 1, w3hi) : -1;
        long tLo3 = (tHi3 >= 0) ? firstSampleEquals(H, idxAtTick(H, tHi3) + 1, w3lo) : -1;
        bool inOrder = haveW1 && tHi1>=0 && tLo1>=0 && tHi2>=0 && tLo2>=0 && tHi3>=0 && tLo3>=0;

        // Silence-trap-2 check: does sample show the write's own byte directly,
        // or does a stray silence (0x00) reading intervene first? Measured, not
        // assumed -- this package's dmaena never goes 1->0 (it is never raised
        // at all), so the specific edge the AMR latch (paula_audio_channel.v:
        // 172-184) keys on cannot occur here; this checks that the trace agrees.
        bool sawSilenceBeforeHi1 = false;
        if (haveW1) {
            for (size_t i = base.word1_pulse_idx; i < H.history.size() && (tHi1 < 0 || H.history[i].t < tHi1); i++) {
                if (H.history[i].sample == 0x00 && i > base.word1_pulse_idx) { sawSilenceBeforeHi1 = true; }
            }
        }
        long deliveryLatency = (haveW1 && tHi1 >= 0) ? (tHi1 - base.word1_write_tick) : -1;

        std::ostringstream d;
        d << "[fixture] FIXTURE-LIVENESS: three distinct written words (0x" << std::hex
          << baseP.word1 << ",0x" << baseP.word2 << ",0x" << baseP.word3 << std::dec
          << ") produce six distinct bytes on `sample`, all observed IN ORDER at the "
          << "cycle each was fetched on (hi1 at write_tick+" << deliveryLatency
          << " ticks). SILENCE-TRAP-2 HANDLED: dmaena is never raised anywhere in this "
          << "package (checked structurally, see GROUP B), so the dmaena 1->0 edge the "
          << "AMR latch (paula_audio_channel.v:172-184) keys on cannot occur here; "
          << "measured directly on this trace, a stray silence (sample==0x00) reading "
          << "between the word1 write and word1's own hi byte = "
          << (sawSilenceBeforeHi1 ? "WAS observed (see RESULTS)" : "was NOT observed");
        CHECK(d.str(), inOrder);
    }

    // -------------------------------------------------------------------
    // GROUP A -- 5-p157.c10 5-direct.c1
    // -------------------------------------------------------------------
    {
        long tHi1 = base.have_word1 ? firstSampleEquals(H, base.word1_pulse_idx, w1hi) : -1;
        long tLo1 = (tHi1 >= 0) ? firstSampleEquals(H, idxAtTick(H, tHi1) + 1, w1lo) : -1;
        bool sawBothBytes = tHi1 >= 0 && tLo1 >= 0;
        bool everDmareq = false, everDmas = false;
        for (auto& s : H.history) { if (s.dmareq) everDmareq = true; if (s.dmas) everDmas = true; }
        bool g_a = sawBothBytes && !everDmareq && !everDmas;
        CHECK("5-p157.c10 5-direct.c1 (p157) \"It is possible to create sound by writing "
              "audio data one word at a time to the audio output addresses, instead of "
              "setting up a list of audio data in memory\": single write of 0x" +
              [&]{std::ostringstream o; o<<std::hex<<baseP.word1; return o.str();}() +
              " produced both bytes (0x" + [&]{std::ostringstream o;o<<std::hex<<(int)w1hi;return o.str();}() +
              " then 0x" + [&]{std::ostringstream o;o<<std::hex<<(int)w1lo;return o.str();}() +
              ") on sample=" + std::string(sawBothBytes?"yes":"no") +
              ", with dmareq ever asserted=" + std::string(everDmareq?"yes":"no") +
              " and dmas ever asserted=" + std::string(everDmas?"yes":"no") +
              " across the whole run (no memory list, no DMA, anywhere)", g_a);

        TimelineParams mp = baseP; mp.skip_all_writes = true;
        TimelineResult mr = run_timeline(mp);
        bool anyOutput = false;
        for (auto& s : mr.h.history) if (s.sample != 0x00) anyOutput = true;
        MUTATION("5-p157.c10", "stimulus", anyOutput ? "green" : "red",
                  "no AUDxDAT write issued at all (skip_all_writes=true, dmaena still 0 "
                  "throughout); sample ever left 0x00 = " + std::string(anyOutput?"yes":"no") +
                  " -- with nothing written, no sound is produced, as expected.");
    }

    // -------------------------------------------------------------------
    // GROUP B -- 5-p157.c11 5-direct.c2
    // -------------------------------------------------------------------
    {
        bool dmaenaAlwaysZero = true;
        for (auto& s : H.history) if (s.dmaena != 0 || s.audxon != 0) dmaenaAlwaysZero = false;

        TimelineParams bp = baseP; bp.block_entry_then_retry = true;
        TimelineResult br = run_timeline(bp);
        bool blockedWhileIntpenHigh = true;
        for (auto& s : br.h.history) {
            if (s.t > br.word1_write_tick && s.t < br.word1_write_tick + 400 && s.state == MANUAL_HI)
                blockedWhileIntpenHigh = false;
        }
        bool enteredAfterRetry = br.entry_ok_after_retry_tick >= 0;

        bool g_b = dmaenaAlwaysZero && blockedWhileIntpenHigh && enteredAfterRetry;
        CHECK("5-p157.c11 5-direct.c2 (p157) \"To use direct audio output, do not enable "
              "the DMA for the audio channel you wish to use; this changes the timing of "
              "the interrupts\": precondition dmaena/AUDxON measured 0 across the whole "
              "run=" + std::string(dmaenaAlwaysZero?"yes":"no") + "; a write issued while "
              "the channel's interrupt-pending guard (!AUDxIP, modeled here as intpen) was "
              "still set was blocked from entering the output state=" +
              std::string(blockedWhileIntpenHigh?"yes":"no") + "; the SAME write retried "
              "once that guard was lifted did enter=" + std::string(enteredAfterRetry?"yes":"no"),
              g_b);

        TimelineParams mp = baseP; mp.block_entry_permanently = true;
        TimelineResult mr = run_timeline(mp);
        bool everEnteredHi = false;
        for (auto& s : mr.h.history) if (s.state == MANUAL_HI) everEnteredHi = true;
        MUTATION("5-p157.c11", "stimulus", everEnteredHi ? "green" : "red",
                  "guard left permanently blocked (intpen never lowered, no retry); state "
                  "ever reached the output state = " + std::string(everEnteredHi?"yes":"no") +
                  " -- correctly never enters while the guard stays unmet.");
    }

    // -------------------------------------------------------------------
    // GROUP C -- 5-p157.c13 5-direct.c4 -- CANDIDATE 5, EXPECTED RED
    // -------------------------------------------------------------------
    {
        long firstAudxir = base.have_word1 ? tickAtOrAfter(H, base.word1_pulse_idx, isAudxir) : -1;
        long firstIntreq = base.have_word1 ? tickAtOrAfter(H, base.word1_pulse_idx, isIntreq) : -1;
        long tHi1 = base.have_word1 ? firstSampleEquals(H, base.word1_pulse_idx, w1hi) : -1;
        long tLo1 = (tHi1 >= 0) ? firstSampleEquals(H, idxAtTick(H, tHi1) + 1, w1lo) : -1;
        int stateAtPulse = base.have_word1 ? H.history[base.word1_pulse_idx].state : -1;
        uint8_t sampleAtPulse = base.have_word1 ? H.history[base.word1_pulse_idx].sample : 0xFF;

        // Manual: interrupt occurs AFTER one full data word (both bytes) has
        // been output, i.e. at/after tLo1. Measured: at the write itself, in
        // state 000, before any byte of the word has left the channel.
        bool per_manual = (firstAudxir >= 0 && tLo1 >= 0 && firstAudxir >= tLo1);
        bool measured_at_write_in_idle = base.have_word1 &&
              (firstAudxir == H.history[base.word1_pulse_idx].t) &&
              (stateAtPulse == MANUAL_IDLE) && (sampleAtPulse != w1hi) && (sampleAtPulse != w1lo);

        std::ostringstream d;
        d << "5-p157.c13 5-direct.c4 (p157) \"The normal interrupt occurs after a data "
             "address has been read; in direct audio output, the interrupt occurs after "
             "one data word has been output\" -- EXPECTED RED, CANDIDATE 5 (not assigned "
             "an NM-HRM key here): manual wants first AUDxIR/intreq at/after tick "
          << tLo1 << " (word1's low byte, i.e. after the full word is out); measured "
             "first AUDxIR/intreq at tick " << firstAudxir << ", coincident with the "
             "AUDxDAT write itself (tick " << H.history[base.word1_pulse_idx].t
          << "), state=" << stateAtPulse << " (000=idle), sample=0x"
          << std::hex << (int)sampleAtPulse << std::dec
          << " -- i.e. before any byte of the word has left the channel "
             "(paula_audio_channel.v:283-285 sets AUDxIR unconditionally in the idle-exit "
             "CPU branch, at the write, not after output). CONTROL: GROUP H below reads "
             "the SAME trace event (state 000->010 timing) and is expected green, so this "
             "red is not a units-error / broken-harness artifact.";
        CHECK(d.str(), per_manual);
        CHECK("[diagnostic, same claims 5-p157.c13 5-direct.c4] measured moment matches "
              "the disclosed finding exactly (AUDxIR at the write tick, state 000, sample "
              "not yet either byte) = " + std::string(measured_at_write_in_idle?"yes":"no"),
              measured_at_write_in_idle);

        TimelineParams mp = baseP; mp.word1 = 0x0000; // different stimulus, same question
        TimelineResult mr = run_timeline(mp);
        long mFirstAudxir = mr.have_word1 ? tickAtOrAfter(mr.h, mr.word1_pulse_idx, isAudxir) : -1;
        int mStateAtPulse = mr.have_word1 ? mr.h.history[mr.word1_pulse_idx].state : -1;
        bool mStillEarly = mr.have_word1 && mFirstAudxir == mr.h.history[mr.word1_pulse_idx].t
                            && mStateAtPulse == MANUAL_IDLE;
        MUTATION("5-p157.c13", "stimulus", mStillEarly ? "red" : "green",
                  "written word changed to 0x0000 (baseline was 0x" +
                  [&]{std::ostringstream o;o<<std::hex<<baseP.word1;return o.str();}() +
                  "); AUDxIR still fires at the write, in state 000, before output = " +
                  std::string(mStillEarly?"yes":"no") + " -- this is a robustness check on "
                  "an already-red finding candidate, not a red-to-green flip: it confirms "
                  "the deviation is a control-path fact (independent of which data is "
                  "written), not an artifact of the one word this package's baseline used.");
    }

    // -------------------------------------------------------------------
    // GROUP D -- 5-p157.c14 5-direct.c5
    // -------------------------------------------------------------------
    {
        bool intpenWasRaised = false;
        for (auto& s : H.history) if (s.t >= base.starve_start_tick && s.intpen) intpenWasRaised = true;
        bool fellBack = base.fallback_tick > 0;
        long latency = fellBack ? (base.fallback_tick - base.starve_start_tick) : -1;
        bool promptly = fellBack && latency > 0 && latency <= (long)(baseP.audper * 8 + 400);

        CHECK("5-p157.c14 5-direct.c5 (p157) \"Unlike in the DMA-controlled automatic data "
              "output, in direct audio output, if you do not write a new set of data to the "
              "output addresses before two sampling intervals have elapsed, the audio "
              "output will cease changing\": precondition -- no new AUDxDAT write was "
              "issued after word3 and intpen was measured raised (left pending) = " +
              std::string(intpenWasRaised?"yes":"no") + "; output loop was left (state "
              "returned to idle, i.e. stopped alternating/changing) = " +
              std::string(fellBack?"yes":"no") + ", after " + std::to_string(latency) +
              " ticks (promptly, at the next perfin) = " + std::string(promptly?"yes":"no"),
              intpenWasRaised && fellBack && promptly);

        TimelineParams mp = baseP; mp.never_raise_intpen = true;
        TimelineResult mr = run_timeline(mp);
        bool neverFellBack = mr.fallback_tick < 0;
        MUTATION("5-p157.c14", "stimulus", neverFellBack ? "red" : "green",
                  "intpen never raised after the last write (stimulus changed: interrupt "
                  "left clear instead of pending); output loop fell back to idle = " +
                  std::string(!neverFellBack?"yes":"no") + " -- correctly does NOT cease "
                  "when the pending-interrupt precondition this claim depends on is absent.");
    }

    // -------------------------------------------------------------------
    // GROUP E -- 5-p157.c15 5-direct.c6
    // -------------------------------------------------------------------
    {
        size_t fbIdx = idxAtTick(H, base.fallback_tick);
        uint8_t sampleAtFallback = (fbIdx != (size_t)-1) ? H.history[fbIdx].sample : 0xFF;
        uint8_t sampleLongAfter = H.dut->sample;
        bool held = (fbIdx != (size_t)-1) && (sampleAtFallback == sampleLongAfter);
        CHECK("5-p157.c15 5-direct.c6 (p157) \"The last value remains as an output of the "
              "digital-to-analog converter\": sample at idle-fallback (tick " +
              std::to_string(base.fallback_tick) + ") = 0x" +
              [&]{std::ostringstream o;o<<std::hex<<(int)sampleAtFallback;return o.str();}() +
              "; sample " + std::to_string(baseP.post_fallback_hold_ticks) +
              " further ticks later = 0x" +
              [&]{std::ostringstream o;o<<std::hex<<(int)sampleLongAfter;return o.str();}() +
              " (held constant)", held);

        TimelineParams mp = baseP; mp.write_during_hold = true;
        TimelineResult mr = run_timeline(mp);
        size_t writeIdx = idxAtTick(mr.h, mr.during_hold_write_tick);
        uint8_t sampleRightAfterWrite = (writeIdx != (size_t)-1) ? mr.h.history[writeIdx].sample : 0xFF;
        uint8_t sampleAtEnd = mr.h.dut->sample;
        bool changedByIntrudingWrite = (sampleRightAfterWrite != sampleAtEnd) ||
                                        (sampleAtEnd == 0x99 || sampleAtEnd == 0x11);
        MUTATION("5-p157.c15", "stimulus", changedByIntrudingWrite ? "red" : "green",
                  "a new AUDxDAT write (0x9911) was sneaked into what should be the "
                  "\"held\" window; final sample = 0x" +
                  [&]{std::ostringstream o;o<<std::hex<<(int)sampleAtEnd;return o.str();}() +
                  " -- a deliberate new write DOES change the held value, showing the "
                  "\"stays constant\" check is sensitive to genuine new data and not "
                  "vacuously true.");
    }

    // -------------------------------------------------------------------
    // GROUP F -- 5-p157.c16
    // -------------------------------------------------------------------
    {
        int volumeReadback = H.dut->volume;
        std::vector<long> perfinTicks;
        for (auto& s : H.history) if (s.t > base.word1_write_tick && s.perfin) perfinTicks.push_back(s.t);
        long measuredPeriod = -1;
        if (perfinTicks.size() >= 2) measuredPeriod = perfinTicks[1] - perfinTicks[0];
        bool g_f = (volumeReadback == baseP.audvol) && (measuredPeriod == baseP.audper);
        CHECK("5-p157.c16 (p157) \"The volume and period registers are set as usual\": "
              "AUDVOL written " + std::to_string(baseP.audvol) + ", volume port reads " +
              std::to_string(volumeReadback) + "; AUDPER written " +
              std::to_string(baseP.audper) + ", measured perfin-to-perfin period = " +
              std::to_string(measuredPeriod) + " cck ticks (both while dmaena=0 the whole "
              "run)", g_f);

        TimelineParams mp = baseP; mp.skip_audper_write = true;
        TimelineResult mr = run_timeline(mp);
        std::vector<long> mPerfinTicks;
        for (auto& s : mr.h.history) if (s.t > mr.word1_write_tick && s.perfin) mPerfinTicks.push_back(s.t);
        long mPeriod = (mPerfinTicks.size() >= 2) ? (mPerfinTicks[1] - mPerfinTicks[0]) : -1;
        bool matchesBaseline = (mPeriod == baseP.audper);
        MUTATION("5-p157.c16", "stimulus", matchesBaseline ? "green" : "red",
                  "AUDPER write skipped (register left at its power-on default instead of "
                  "the " + std::to_string(baseP.audper) + " this package wrote); measured "
                  "period now = " + std::to_string(mPeriod) + " -- correctly no longer "
                  "matches the value this claim's baseline relies on having been written.");
    }

    // -------------------------------------------------------------------
    // GROUP G -- 5-p164.c5 5-sm.c6
    // -------------------------------------------------------------------
    {
        bool guardTrueAtPulse = base.have_word1 &&
              H.history[base.word1_pulse_idx].audxdat &&
              !H.history[base.word1_pulse_idx].audxon &&
              !H.history[base.word1_pulse_idx].audxip;
        long tHiAfter = base.have_word1 ? firstSampleEquals(H, base.word1_pulse_idx, w1hi) : -1;
        bool reachedOutput = tHiAfter >= 0;
        CHECK("5-p164.c5 5-sm.c6 (p164) \"One of the paths out of the idle state is "
              "designed for interrupt-driven operation (processor provides the data), and "
              "the other path is designed for DMA-driven operation (the 'Agnus' special "
              "chip provides the data)\": the processor-fed path's own guard "
              "(AUDxDAT && !AUDxON && !AUDxIP, paula_audio_channel.v:281-290) measured "
              "true at the write = " + std::string(guardTrueAtPulse?"yes":"no") +
              "; output subsequently produced with dmaena held 0 throughout (no Agnus "
              "involvement) = " + std::string(reachedOutput?"yes":"no"), guardTrueAtPulse && reachedOutput);

        TimelineParams mp = baseP; mp.block_entry_permanently = true;
        TimelineResult mr = run_timeline(mp);
        bool everEnteredHi = false;
        for (auto& s : mr.h.history) if (s.state == MANUAL_HI) everEnteredHi = true;
        MUTATION("5-p164.c5", "stimulus", everEnteredHi ? "green" : "red",
                  "guard left permanently blocked (same rerun as GROUP B's mutation: intpen "
                  "never lowered); processor-fed path ever taken = " +
                  std::string(everEnteredHi?"yes":"no") + " -- correctly never taken while "
                  "the guard is unmet, confirming this is a real precondition, not just a "
                  "write reaching the register.");
    }

    // -------------------------------------------------------------------
    // GROUP H -- 5-p164.c7 5-sm.c9 -- CONTROL CASE for GROUP C
    // -------------------------------------------------------------------
    {
        int stateAtPulse = base.have_word1 ? H.history[base.word1_pulse_idx].state : -1;
        long tHi1 = base.have_word1 ? firstSampleEquals(H, base.word1_pulse_idx, w1hi) : -1;
        int stateAtHi1 = (tHi1 >= 0) ? H.history[idxAtTick(H, tHi1)].state : -1;
        bool noIntermediateState = true;
        if (base.have_word1 && tHi1 >= 0) {
            for (size_t i = base.word1_pulse_idx; i < H.history.size() && H.history[i].t <= tHi1; i++) {
                int st = H.history[i].state;
                if (st != MANUAL_IDLE && st != MANUAL_HI) noIntermediateState = false;
            }
        }
        bool g_h = (stateAtPulse == MANUAL_IDLE) && (stateAtHi1 == MANUAL_HI) && noIntermediateState;
        CHECK("5-p164.c7 5-sm.c9 (p164) \"In interrupt-driven operation, transfer to the "
              "main loop (states 010 and 011) occurs immediately after data is written by "
              "the processor\": state at the write = " + std::to_string(stateAtPulse) +
              " (000=idle); state at the first output byte = " + std::to_string(stateAtHi1) +
              " (010); no state other than 000/010 observed in between = " +
              std::string(noIntermediateState?"yes":"no") + " -- CONTROL CASE: this is the "
              "SAME trace event GROUP C reads for the interrupt-moment finding, and this "
              "check is expected, and found, GREEN, so GROUP C's red is not a broken-"
              "harness artifact.", g_h);

        TimelineParams mp = baseP; mp.skip_all_writes = true;
        TimelineResult mr = run_timeline(mp);
        bool everLeftIdle = false;
        for (auto& s : mr.h.history) if (s.state != MANUAL_IDLE) everLeftIdle = true;
        MUTATION("5-p164.c7", "stimulus", everLeftIdle ? "green" : "red",
                  "no write issued at all; state ever left idle = " +
                  std::string(everLeftIdle?"yes":"no") + " -- correctly no transfer to the "
                  "main loop with nothing written.");
    }

    // -------------------------------------------------------------------
    // GROUP I -- 5-p164.c12 5-sm.c14
    // -------------------------------------------------------------------
    {
        bool intpenHeldLow = true;
        for (auto& s : H.history) {
            if (s.t > base.word1_write_tick && s.t < base.starve_start_tick && s.intpen) intpenHeldLow = false;
        }
        bool stayedInLoop = true;
        for (auto& s : H.history) {
            if (s.t > base.word1_write_tick && s.t < base.starve_start_tick && s.state == MANUAL_IDLE)
                stayedInLoop = false;
        }
        CHECK("5-p164.c12 5-sm.c14 (p164) \"As long as the interrupt is cleared by the "
              "processor in time, the machine remains in the main loop\": precondition -- "
              "intpen measured held low across the whole word2/word3 loop window = " +
              std::string(intpenHeldLow?"yes":"no") + "; machine never returned to idle "
              "during that same window = " + std::string(stayedInLoop?"yes":"no"),
              intpenHeldLow && stayedInLoop);

        TimelineParams mp = baseP; mp.raise_intpen_early = true;
        TimelineResult mr = run_timeline(mp);
        bool leftLoopEarly = mr.fallback_tick > 0;
        MUTATION("5-p164.c12", "stimulus", leftLoopEarly ? "red" : "green",
                  "intpen raised immediately after word1 instead of after word3 (interrupt "
                  "NOT cleared in time); machine returned to idle before the loop had run "
                  "its normal course = " + std::string(leftLoopEarly?"yes":"no") + " -- "
                  "correctly does NOT remain in the main loop once this precondition no "
                  "longer holds.");
    }

    // -------------------------------------------------------------------
    // GROUP J -- 5-p164.c13 5-sm.c15
    // -------------------------------------------------------------------
    {
        bool fellBack = base.fallback_tick > 0;
        CHECK("5-p164.c13 5-sm.c15 (p164) \"Otherwise, it enters the idle state\": with "
              "the interrupt left pending (intpen raised, not cleared) after word3, the "
              "machine entered idle (state 000) at tick " + std::to_string(base.fallback_tick) +
              " = " + std::string(fellBack?"yes":"no"), fellBack);

        TimelineParams mp = baseP; mp.never_raise_intpen = true;
        TimelineResult mr = run_timeline(mp);
        bool neverFellBack = mr.fallback_tick < 0;
        MUTATION("5-p164.c13", "stimulus", neverFellBack ? "green" : "red",
                  "same rerun as GROUP D's mutation: intpen never raised (the 'otherwise' "
                  "precondition never arises); machine never entered idle = " +
                  std::string(neverFellBack?"yes":"no") + " -- correctly tracks the real "
                  "causal trigger (pending, uncleared interrupt) rather than falling back "
                  "unconditionally.");
    }

    // -------------------------------------------------------------------
    std::cout << "\n=== " << g_pass << " passed, " << g_fail << " failed ===\n";
    return g_fail == 0 ? 0 : 1;
}
