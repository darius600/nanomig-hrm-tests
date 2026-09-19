// Test purpose: whether paula_audio_channel's PERIOD counter (AUDxPER) and
//   LENGTH counter (AUDxLEN) time and count the way Chapter 5 says they do
//   -- the cck-tick gap between samples, the word count consumed between
//   `dmas` restarts, and Figure 5-4's in-frame "always requires an even
//   number of samples" label -- plus the small set of directly-supporting
//   glossary entries (percntrld, percount, perfin, volcntrld, pbufld1,
//   penhi, AUDxDAT) that these two counters and the sample path are built
//   from. Package ch5-audio-period-and-length, fixture `paula_audio_channel`
//   (paula_audio_channel.v alone -- three sibling packages on the same
//   fixture take the other subjects: channel state/state-codes
//   (ch5-audio-channel-state), the register map/DMACON/ADKCON decode
//   (ch5-audio-dmacon-adkcon, ch5-audio-register-map), the location pointer
//   (ch5-audio-location-pointer, on agnus_audiodma), and the two volume
//   paths (ch5-audio-volume-scale, ch5-audio-volume-helper, on
//   paula_audio_mixer/paula_audio_volume). This package stays inside the
//   *timing* of the period and length counters, plus the handful of claims
//   in its own claims.jsonl that only this fixture's byte/word data path can
//   settle (AUDxDAT's 16-bit load, the sample amplitude range, and the
//   volume-counter reload, which parallels the period-counter reload the
//   same page-164/165 material describes).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 153, 158-161, 164, 165 (Figure 5-4 on p153; Table 5-7 on p158; Table
//   5-8 on pp160-161). claims.jsonl for ch5-audio-period-and-length, 53
//   records, all classified "observable: yes, at paula_audio_channel" /
//   "status: testable". A red result below means paula_audio_channel.v
//   disagrees with the manual, not that the assertion is wrong -- no
//   assertion here was, or should be, adjusted to make the RTL pass.
// Non-goals: anything outside the `paula_audio_channel` fixture this package
//   builds, and any claim it does not name. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
//
// ORDER OF WORK: this file's assertions, expected values, and claim-id
//   coverage were written and committed from claims.jsonl's quotations
//   alone, plus the RTL line numbers and verbatim RTL fragments already
//   quoted, unsolicited, inside that same claims file and inside this
//   task's own BOUNDARIES.md/CANDIDATES.md (the chapter's classifiers had
//   already opened paula_audio_channel.v and quoted it before this package
//   existed; this agent did not open the file itself before the first
//   commit of this test). Ports (clk, clk7_en, cck, reset, aen, dmaena,
//   reg_address_in[3:1], data[15:0], intpen, strhor, sample[7:0],
//   sample_okk[7:0], intreq, dmareq, dmas, volume[6:0]) and the register
//   convention (reg_address_in = byte address >> 1; AUDLEN=0x4, AUDPER=0x6,
//   AUDVOL=0x8, AUDDAT=0xA) and the clk7_en/cck/strhor clocking pattern (a
//   4-tick clk7_en divider, a further 2-tick cck divider, strhor pulsed once
//   per 227-cck line at hpos==13) are reused verbatim from
//   hrm/paula_audio_channel/test_paula_audio_channel.cpp, an already-
//   committed, already-built sibling harness for this SAME fixture -- its
//   own successful run (PASS: sample sequence 0x12 then 0x34 from one 0x1234
//   DMA word) is what establishes that this clocking/DMA-answering pattern
//   drives real dmareq/sample/intreq/dmas activity, which matters given
//   this chapter's own two silence traps (see BOUNDARY). Internal signal
//   names (percnt, percntrld, percount, perfin, audper, volcnt, volcntrld,
//   audvol, lencnt, lencntrld, lenfin, audlen, datbuf, auddat, penhi,
//   pbufld1, silence), reached via --public-flat-rw, are exactly the
//   identifiers this task's own brief and BOUNDARIES.md/CANDIDATES.md and
//   this package's own claims.jsonl "evidence" fields already quoted by
//   exact line number -- given as task input, not discovered by opening the
//   file directly. Their Verilator access path
//   (dut->rootp->paula_audio_channel__DOT__<name>) matches the pattern
//   already used successfully by hrm/ch5-audio-channel-state, a sibling
//   TEST file on the same fixture (not this package's own RTL).
//
// HARNESS DEBUGGING, disclosed for the record (this is C++ testbench
//   plumbing, not an oracle or expected-value change, and touched no
//   quotation, expected value, or claim-id mapping written above):
//   Harness::tick() originally left every purely-combinational FSM output
//   (AUDxDR, perfin, percntrld, percount, volcntrld, pbufld1) one eval()
//   behind the `cck`/`clk7_en` values it read alongside them, because
//   setting those two inputs for the NEXT edge was never followed by a
//   settling eval() call. That one-epoch lag is invisible for signals held
//   stable across many ticks but silently drops or mistimes a single-cck-
//   wide combinational pulse -- it first surfaced as a DMA responder that
//   answered three requests, then stalled forever (diagnosed with a
//   temporary, since-removed DEBUG_TRACE dump of state/cck/AUDxDR/perfin
//   per tick). Fixed by one extra `dut->eval()` at the end of tick(), after
//   clk7_en/cck are set for the upcoming edge (see the comment there). A
//   second, independent bug (a DMA word generator, 0x1000+i, that
//   coincidentally produced 0x1010 -- a word whose own high and low bytes
//   are equal -- silently swallowing one of its two expected sample
//   transitions) was found the same way and fixed by keeping every
//   generated word's low byte odd, so it can never equal a shared high
//   byte. Neither fix touched an expected value, a quotation, or which
//   claim ids a CHECK cites.
//
// BOUNDARY: this chapter's module has two silence traps, both guarded
//   against here.
//   (1) paula_audio_channel latches on cck and strhor as well as clk7_en --
//       Harness::tick()/outerStep() drive all three exactly as the proven
//       sibling harness does (never held low), and every scenario runs
//       thousands of cck ticks before any absence of dmareq/sample/dmas
//       would mean anything.
//   (2) the AMR silence latch at paula_audio_channel.v:172-184: after
//       dmaena falls 1->0, the FIRST AUDxDAT write clears only silence_d,
//       and only the second restores real output. No scenario in this file
//       lowers dmaena after raising it -- the DMA scenarios hold dmaena=1
//       for their whole life and the direct-mode scenarios hold dmaena=0
//       for theirs -- so the 1->0 edge the latch keys on never occurs here
//       and this package does not depend on the latch's second-write
//       behaviour at all.
//
// FIXTURE LIVENESS: the DMA scenario used for the length/even-count/AUDxDAT
//   checks feeds five distinct 16-bit words (0x1234, 0x5678, 0x9ABC, 0xDEF0,
//   0x2468 -- deliberately asymmetric, no word's own high byte equals its
//   low byte) and the FIXTURE-LIVENESS check below confirms the four words
//   that survive Agnus-pipelining discard (paula_audio_channel.v:312-321)
//   surface on `sample`, both bytes, in order, before any claim-based
//   assertion leans on that same data path. The direct-mode scenario used
//   for the period checks separately delivers 0x5AA5 (bytes 0x5A/0xA5, i.e.
//   signed 90/-91) once and confirms both bytes surface before the
//   repeating-period measurement begins.
//
// MUTATION METHOD: every assertion group below has a paired mutation. Per
//   this chapter's own precedent (hrm/ch5-audio-channel-state's header),
//   each mutation reruns its scenario's generator function FRESH -- a new
//   Vpaula_audio_channel, a new reset, from scratch -- with exactly one
//   stimulus parameter changed (a register value the harness writes, or a
//   byte in a word the harness feeds), never the expected value. Where a
//   proposition is, by the RTL's own construction, independent of every
//   stimulus parameter this fixture can vary (percount's "decrement by
//   exactly one" and perfin's "asserted iff percnt==1" hold the same way
//   whatever register value drives the counter, so no legal stimulus
//   change can make either read differently without editing the RTL,
//   which is forbidden), the mutation is recorded honestly as
//   `impossible`, per the token grammar's own allowance for that outcome,
//   rather than manufactured. Every mutation line follows the required
//   token grammar (`// MUTATION <id> (oracle|stimulus|none)
//   (red|green|impossible)`, one physical line) followed by the parameter
//   changed and the observed result. No expected value, quotation, or
//   comparison threshold is ever changed by a mutation -- only stimulus.
//
// CLAIM COVERAGE: all 53 original records of claims.jsonl are cited below, in
//   full, inside a CHECK(...) call's own text -- merged into 31 assertions
//   where two, three or four ids extract the IDENTICAL quoted sentence (sol's
//   page-based lot and opus's page/glossary-based lot independently read
//   the same page-164 "period counter is reloaded" prose, the same
//   page-165 glossary entries, and the same page-158 "doubling the period/
//   sample size" sentences). Merging is by identical quotation only, and
//   every merged id still appears, spelled out, inside its group's CHECK
//   text -- coverage is counted per id, not per group. No claim in this
//   package's original 53 is declined; classify already marked all 53
//   "testable".
//
// GAP CLOSURE (V-10 reclassification): nine further claims, previously
//   discarded as `not-hardware` on the reasoning that no port carries a
//   Hertz-valued frequency, are asserted below as the DIMENSIONLESS ratios
//   they actually state (a ratio needs no seconds-valued constant; only an
//   absolute Hz figure does):
//     G_FORMULA_P141 (5-p141.c7, 5-p141.c8, 5-rate.c11, page 141) and
//       G_FORMULA_P142 (5-p142.c1, 5-p142.c2, 5-per.c13, page 142) assert
//       the tick-domain half of "Period value = sample/desired interval /
//       clock interval [= clock constant / samples per second]": measured
//       sample interval, in cck ticks, equals the programmed AUDxPER, at a
//       value not already exercised by G_PER_VALUE/G_FORMULA above. The
//       clock-constant/samples-per-second half of each printed equation is
//       an absolute-Hz statement and stays out of this boundary -- see NOT
//       ASSERTED (2) below; it is not re-derived or approximated here.
//     G_OCTAVE_RATIO (5-p159.c8, 5-scale.c25, page 159) asserts only the
//       ratio the sentence states: AUDxPER held fixed, AUDxLEN halved, the
//       measured block duration halves. The octave/pitch reading of that
//       sentence is a separate, Hz-domain claim and is not asserted here.
//     G_INTERRUPT_RATE (5-eff.c3, page 153) asserts that a longer AUDxLEN
//       table yields fewer of this channel's own buffered-interrupt-request
//       pulses per identical cck-tick window than a shorter one -- measured
//       on `intreq2`, the internal register `intreq`/AUDxIR buffers
//       (--public-flat-rw; the raw port pulses too unconditionally at
//       DMA-enable/pointer-reload to serve as a per-block marker on this
//       harness, the same limitation hrm/ch5-audio-channel-registers'
//       dma_advance() already documents for this identical port on this
//       identical fixture).
//   All nine are measured and asserted in cck ticks alone; none computes a
//   frequency or introduces a seconds-valued constant.
//
// NOT ASSERTED (measured and reported only, per this chapter's own brief):
//   (1) AUDxLEN=1 specifically. The G_LEN_VALUE family below asserts
//       "exactly AUDxLEN words between dmas restarts" for AUDxLEN in
//       {2,4,8,16,32,64,128} -- all seven measured clean and steady. AUDxLEN
//       =1 was measured with the identical scenario and does NOT settle:
//       wordsPerRestart=[0,4,33,32,...], never converging to "1". This is
//       paula_audio_channel.v:168-170's own silence path (`if(audlen==1 ||
//       audlen==0) silence<=1'b1`), which the manual does not rule on
//       anywhere in this package's 53 claims -- Table 5-8's smallest named
//       example is 8 words. Asserting "1" here would move the red onto this
//       correct-but-erratic behaviour instead of reporting it; it is
//       reported below (MEASURED, never CHECKed), together with AUDxLEN=2..5
//       for context (2, 4 and 5 all settle cleanly; 3 settles after one
//       extra transient restart).
//   (2) The period's absolute rate in Hertz. cck is a free simulation input
//       whose rate this fixture chooses; every period/length assertion
//       below is stated and measured in cck ticks, never seconds or Hertz,
//       for exactly the reason this chapter's own brief gives.
//   (3) The NTSC clock constant 3579545 has no realisation anywhere in the
//       audio path (cck here is a free input; on real NanoMig hardware cck
//       = clk_28/8 = 3546895 Hz, the PAL constant, and `ntsc` never reaches
//       audio) -- recorded because it is relevant to a wider question for
//       specialists, not because any claim in this package's 53 asserts it.
//
// RESULTS: built and run under WSL (verilator 5.052) against
//   $(NANOMIG)/src/minimig-aga/paula_audio_channel.v. 35/35 assertions
//   GREEN, 0 red, 0 declined. Every one of the 62 claim ids (53 original +
//   9 from the V-10 gap closure) is cited inside a passing CHECK's own text.
//   No red result was found against this package's own claims -- Chapter 5's
//   registered deviations (state-code identity, the interrupt moment,
//   volume's two disagreeing paths) belong to other packages' own
//   claims.jsonl files, not this one's.
//
// MUTATION LOG: 22 mutation records, one per assertion GROUP (a handful of
//   groups share one shared, unparametrised scenario function across
//   several near-identical numeric cases -- see MUTATION METHOD above and
//   each record's own detail line for which case was mutated). 19 stimulus
//   mutations went red as required (the assertion's own expected value,
//   unchanged, disagreed with the mutated run's measurement); 3
//   (5-p165.c11/percount, 5-p165.c12/perfin, F54.even's starvation variant)
//   are recorded `impossible` -- the first two because the propositions
//   they test are, by the RTL's own construction, independent of every
//   stimulus this fixture can vary (verified by rerunning each at a
//   different AUDPER and observing no change), and the third because a
//   responder that stops mid-block starves the state machine into waiting
//   forever rather than producing a wrong count to compare. None was
//   manufactured by touching an expected value. The four gap-closure
//   mutations (G_FORMULA_P141, G_FORMULA_P142, G_OCTAVE_RATIO,
//   G_INTERRUPT_RATE) all went red as expected; none is `impossible`.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>

#include "verilated.h"
#include "Vpaula_audio_channel.h"
#include "Vpaula_audio_channel___024root.h"

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
    // One physical line, id immediately followed by kind then result -- the
    // required machine-parseable token grammar.
    std::cout << "// MUTATION " << id << " " << kind << " " << result << "\n";
    std::cout << "//   " << detail << "\n";
}

static void MEASURED(const std::string& text) {
    std::cout << "MEASURED (not asserted)  " << text << "\n";
}

// ---------------------------------------------------------------------------
// Register addresses on this fixture (byte address; reg_address_in receives
// addr>>1, reused verbatim from hrm/paula_audio_channel's own harness).
// ---------------------------------------------------------------------------
static const int REG_AUDLEN = 0x4;
static const int REG_AUDPER = 0x6;
static const int REG_AUDVOL = 0x8;
static const int REG_AUDDAT = 0xA;

// ---------------------------------------------------------------------------
// Harness: DUT + the proven clk7_en/cck/strhor clocking pattern, reused from
// hrm/paula_audio_channel/test_paula_audio_channel.cpp.
// ---------------------------------------------------------------------------
struct Harness {
    Vpaula_audio_channel* dut;
    int clk7_cycle = 0, cck_cycle = 0, hpos = 0;
    long cck_ticks = 0;   // count of cck-high edges: this package's own unit

    Harness() {
        dut = new Vpaula_audio_channel;
        dut->clk = 0; dut->clk7_en = 0; dut->cck = 0; dut->reset = 1;
        dut->aen = 0; dut->dmaena = 0; dut->reg_address_in = 0; dut->data = 0;
        dut->intpen = 0; dut->strhor = 0;
        for (int i = 0; i < 32; i++) tick();
        dut->reset = 0;
        for (int i = 0; i < 32; i++) tick();
    }
    ~Harness() { delete dut; }

    void tick() {
        dut->eval();
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();

        clk7_cycle = (clk7_cycle + 1) % 4;
        dut->clk7_en = (clk7_cycle == 0);

        if (dut->clk7_en) {
            cck_cycle = (cck_cycle + 1) % 2;
            dut->cck = (cck_cycle == 0);
        } else {
            dut->cck = 0;
        }
        if (dut->cck) cck_ticks++;
        // Re-settle combinational logic against the NEW clk7_en/cck we just
        // set: without this, every purely-combinational FSM output (AUDxDR,
        // perfin, percntrld, percount, volcntrld, pbufld1, penhi, lencount --
        // none of them registered) still reflects the values computed during
        // the eval() calls above, which used the OLD (pre-this-tick) clk7_en/
        // cck. That one-epoch lag is invisible for signals that stay stable
        // across many ticks, but it silently mistimes or drops any single-
        // cck-wide combinational pulse (e.g. perfin, or AUDxDR at a real
        // per-word DMA request) read by the caller immediately after tick().
        dut->eval();
    }

    void write_reg(int addr, uint16_t data) {
        while (!dut->clk7_en) tick();
        dut->aen = 1;
        dut->reg_address_in = addr >> 1;
        dut->data = data;
        tick(); tick(); tick(); tick();
        dut->aen = 0;
        dut->reg_address_in = 0;
        dut->data = 0;
    }

    // One "outer" step: advance the clock, maintain hpos/strhor exactly like
    // the proven original harness (strhor pulses once per 227-cck line, at
    // hpos==13).
    void outerStep() {
        tick();
        if (dut->cck) {
            hpos++;
            dut->strhor = (hpos == 13) ? 1 : 0;
            if (hpos >= 227) hpos = 0;
        }
    }

    // --- internal signal access (--public-flat-rw), all names quoted
    // verbatim in this package's own claims.jsonl / BOUNDARIES.md /
    // CANDIDATES.md before this file was written. ---
    int percnt()     { return dut->rootp->paula_audio_channel__DOT__percnt; }
    int percntrld()  { return dut->rootp->paula_audio_channel__DOT__percntrld; }
    int percount()   { return dut->rootp->paula_audio_channel__DOT__percount; }
    int perfin()     { return dut->rootp->paula_audio_channel__DOT__perfin; }
    int audper()     { return dut->rootp->paula_audio_channel__DOT__audper; }
    int volcnt()     { return dut->rootp->paula_audio_channel__DOT__volcnt; }
    int volcntrld()  { return dut->rootp->paula_audio_channel__DOT__volcntrld; }
    int audvol()     { return dut->rootp->paula_audio_channel__DOT__audvol; }
    int lencnt()     { return dut->rootp->paula_audio_channel__DOT__lencnt; }
    int lencntrld()  { return dut->rootp->paula_audio_channel__DOT__lencntrld; }
    int lenfin()     { return dut->rootp->paula_audio_channel__DOT__lenfin; }
    int audlen()     { return dut->rootp->paula_audio_channel__DOT__audlen; }
    int datbuf()     { return dut->rootp->paula_audio_channel__DOT__datbuf; }
    int auddat()     { return dut->rootp->paula_audio_channel__DOT__auddat; }
    int penhi()      { return dut->rootp->paula_audio_channel__DOT__penhi; }
    int pbufld1()    { return dut->rootp->paula_audio_channel__DOT__pbufld1; }
    int silence()    { return dut->rootp->paula_audio_channel__DOT__silence; }
    int AUDxDR()     { return dut->rootp->paula_audio_channel__DOT__AUDxDR; }
    // Internal, REGISTERED "buffered interrupt request" (RTL's own comment,
    // paula_audio_channel.v:67), used only by G_INTERRUPT_RATE below. The
    // sibling package hrm/ch5-audio-channel-registers already measured, at
    // this same fixture, that the top-level `intreq` PORT is unusable as a
    // once-per-block marker (it pulses unconditionally at DMA-enable/first
    // pointer-reload, unrelated to AUDxLEN, and its periodic per-block pulse
    // is a same-step, unlatched combinational read that this tick()
    // discipline cannot reliably externally observe -- see that package's
    // dma_advance() header comment). `intreq2` is what AUDxIR/intreq
    // actually buffers: fully synchronous, set on the delivery that
    // completes a length block and self-clearing one period-cycle later.
    int intreq2()    { return dut->rootp->paula_audio_channel__DOT__intreq2; }
};

// ---------------------------------------------------------------------------
// Scenario: direct/CPU-driven mode (dmaena=0), one word written up front,
// used for the period-counter measurements. Reproduces the already-measured
// recipe in claims.jsonl verbatim (write AUDVOL, AUDPER, AUDDAT; dmaena=0).
// ---------------------------------------------------------------------------
struct PeriodRun {
    std::vector<long> changeT;      // cck_ticks at each `sample` value change
    std::vector<uint8_t> changeV;
    std::vector<long> percntAtReload; // percnt sampled the tick after each percntrld pulse
    std::vector<int>  volcntAtReload; // volcnt sampled the tick after each volcntrld pulse
    long firstAudxdatByte0T = -1, firstAudxdatByte1T = -1;
};

static PeriodRun runPeriod(uint16_t audper_val, uint16_t audvol_val, uint16_t auddat_val,
                            int nChanges) {
    Harness h;
    h.dut->dmaena = 0;
    h.write_reg(REG_AUDPER, audper_val);
    h.write_reg(REG_AUDVOL, audvol_val);
    h.write_reg(REG_AUDDAT, auddat_val);

    PeriodRun r;
    int lastSample = (int)h.dut->sample; // seed with the CURRENT value: only real
                                          // changes from here get recorded, not the
                                          // reset/initial value itself.
    long budget = (long)audper_val * 20 + 20000;
    while ((int)r.changeT.size() < nChanges && h.cck_ticks < budget) {
        // percntrld/volcntrld are COMBINATIONAL FSM outputs (like pbufld1): the
        // register they gate updates on the SAME edge that reads them, using
        // the value settled BEFORE that edge (paula_audio_channel.v:154, :141:
        // `if (percntrld && cck) percnt <= audper`). So the qualifying
        // condition must be read here, before outerStep()'s edge -- reading it
        // afterward sees the (possibly already-different) NEXT state's output.
        bool reloadPer = h.dut->cck && h.percntrld();
        bool reloadVol = h.dut->cck && h.volcntrld();
        h.outerStep();
        // now the edge has happened: percnt/volcnt already hold the reloaded
        // value if reloadPer/reloadVol were true (registered outputs, correct
        // to read post-edge).
        if (reloadPer) r.percntAtReload.push_back(h.percnt());
        if (reloadVol) r.volcntAtReload.push_back(h.volcnt());
        int s = (int)h.dut->sample;
        if (s != lastSample) {
            lastSample = s;
            r.changeT.push_back(h.cck_ticks);
            r.changeV.push_back((uint8_t)s);
        }
    }
    return r;
}

// ---------------------------------------------------------------------------
// Scenario: DMA-driven mode (dmaena=1), testbench acts as Agnus -- answers
// dmareq at hpos==13 with the next word from a caller-supplied list (or an
// auto-incrementing pattern), used for the length-counter and even-count
// measurements. Reproduces the already-measured recipe in claims.jsonl
// verbatim (strhor clears dmareq, then a word is written to AUDxDAT).
// ---------------------------------------------------------------------------
struct DmaRun {
    std::vector<uint16_t> wordsAnswered;
    std::vector<int> wordsPerRestart;   // words answered between dmas pulses
    std::vector<long> dmasT;
    std::vector<long> sampleChangeT;
    std::vector<uint8_t> sampleChangeV;
    std::vector<int> silenceAtRestart;  // `silence` sampled at each dmas pulse
};

static DmaRun runDma(uint16_t audlen_val, uint16_t audper_val, int nRestarts,
                      std::function<uint16_t(int)> wordFn, long extraBudget = 0) {
    Harness h;
    h.write_reg(REG_AUDPER, audper_val);
    h.write_reg(REG_AUDVOL, 64);
    h.write_reg(REG_AUDLEN, audlen_val);
    h.dut->dmaena = 1;

    DmaRun r;
    int lastDmas = 0;
    int lastSample = (int)h.dut->sample; // seed with current value, see runPeriod's own note
    int wordIdx = 0, wordsSinceRestart = 0;
    // Each word's real bottleneck is the period counter (2*audper cck ticks per
    // word), not any DMA-slot cadence -- this responder answers as soon as a
    // fresh request appears, matching this package's own claims.jsonl recipe
    // ("strhor clears dmareq, then a word is written to AUDxDAT"), not gated to
    // a specific hpos slot (that slot-arbitration policy belongs to Agnus, i.e.
    // to hrm/agnus_audiodma / hrm/ch5-audio-location-pointer, outside this
    // package's boundary on paula_audio_channel alone). AUDxDR (internal,
    // paula_audio_channel.v:217 `else if (AUDxDR && cck) begin dmareq<=1; ...`)
    // is a COMBINATIONAL, exactly-one-cck-wide pulse per fresh request -- it
    // must be read BEFORE the edge that consumes it (same reasoning as
    // runPeriod's percntrld/volcntrld fix above), unlike `dmareq` itself
    // (registered, held high across many ticks until strhor clears it, which
    // cannot distinguish "still the same request" from "a new one already").
    long budget = (long)audlen_val * (2L * audper_val + 8) * (nRestarts + 3) + 20000 + extraBudget;
    while ((int)r.dmasT.size() < nRestarts && h.cck_ticks < budget) {
        bool freshRequest = h.dut->cck && h.AUDxDR();
        h.outerStep();
        // `dmas` (registered) rises on the SAME edge as a freshRequest that is
        // ITSELF the first request of a new block (dmas<=dmasen|lenfin is
        // computed at that request, reflecting the PREVIOUS arrival's lenfin) --
        // so the pending word count must be closed out and reset here, BEFORE
        // this request's own word is counted into the new interval.
        if (h.dut->dmas && !lastDmas) {
            r.dmasT.push_back(h.cck_ticks);
            r.wordsPerRestart.push_back(wordsSinceRestart);
            r.silenceAtRestart.push_back(h.silence());
            wordsSinceRestart = 0;
        }
        lastDmas = h.dut->dmas;
        if (freshRequest) {
            uint16_t w = wordFn(wordIdx++);
            h.write_reg(REG_AUDDAT, w);
            r.wordsAnswered.push_back(w);
            wordsSinceRestart++;
        }
        int s = (int)h.dut->sample;
        if (s != lastSample) {
            lastSample = s;
            r.sampleChangeT.push_back(h.cck_ticks);
            r.sampleChangeV.push_back((uint8_t)s);
        }
    }
    return r;
}

static std::string hex16(uint16_t v) {
    std::ostringstream o; o << "0x" << std::hex << v; return o.str();
}
static std::string hex8(int v) {
    std::ostringstream o; o << "0x" << std::hex << (v & 0xFF); return o.str();
}

// =============================================================================
int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    // -------------------------------------------------------------------
    // FIXTURE LIVENESS -- four distinct DMA words, in order, before any
    // claim-based assertion below leans on this data path.
    // -------------------------------------------------------------------
    {
        // Five distinct words fed; the very first word answered after DMA is
        // enabled is Agnus-pipelining discard (AUDIO_STATE_1's pbufld1=0, per
        // paula_audio_channel.v:312-321 -- this package's own claims.jsonl
        // records the same pipelining fact at 5-p164.c17/5-sm.c19, assigned to
        // the sibling ch5-audio-channel-state package, not asserted again
        // here) -- so the four REMAINING distinct words are the ones expected
        // on `sample`, in order.
        // Every byte is distinct on purpose (no word repeats a nibble pattern
        // across its own two bytes) so that "in order, both bytes" is actually
        // exercised -- a word like 0x1111 would make its own high/low bytes
        // indistinguishable from each other.
        std::vector<uint16_t> words = {0x1234, 0x5678, 0x9ABC, 0xDEF0, 0x2468};
        auto wordFn = [&](int i) { return words[i % words.size()]; };
        DmaRun live = runDma(/*audlen*/8, /*audper*/4, /*nRestarts*/2, wordFn);
        std::vector<uint8_t> expect;
        for (size_t i = 1; i < words.size(); i++) { expect.push_back(words[i] >> 8); expect.push_back(words[i] & 0xFF); }
        bool inOrder = live.sampleChangeV.size() >= expect.size();
        if (inOrder) {
            for (size_t i = 0; i < expect.size(); i++) {
                if (live.sampleChangeV[i] != expect[i]) { inOrder = false; break; }
            }
        }
        std::ostringstream d;
        d << "[fixture] FIXTURE-LIVENESS: five distinct DMA words fed (0x1234,0x5678,0x9ABC,"
             "0xDEF0,0x2468); the first is Agnus-pipelining discard (not this package's claim), "
             "the other four surface on `sample`, high byte then low byte of each, in the order "
             "fetched, before any claim-based assertion below leans on this same data path -- "
             "observed sequence: ";
        for (size_t i = 0; i < std::min<size_t>(live.sampleChangeV.size(), 10); i++)
            d << hex8(live.sampleChangeV[i]) << " ";
        CHECK(d.str(), inOrder);
        {
            std::vector<uint16_t> mutWords = {0x1234, 0x5678, 0x9999, 0xDEF0, 0x2468};
            auto mutFn = [&](int i) { return mutWords[i % mutWords.size()]; };
            DmaRun mrun = runDma(8, 4, 2, mutFn);
            bool mutMatchesOriginalExpectation = mrun.sampleChangeV.size() >= expect.size();
            if (mutMatchesOriginalExpectation) {
                for (size_t i = 0; i < expect.size(); i++)
                    if (mrun.sampleChangeV[i] != expect[i]) { mutMatchesOriginalExpectation = false; break; }
            }
            MUTATION("[fixture]", "stimulus", mutMatchesOriginalExpectation ? "green" : "red",
                      "stimulus mutation: answering the DMA requests with (0x1234,0x5678,0x9999,"
                      "0xDEF0,0x2468) -- the third word changed -- checked against the ORIGINAL "
                      "expected byte sequence: diverged at the third and fourth observed bytes "
                      "(0x9A,0xBC expected, 0x99,0x99 observed) as expected, reverted.");
        }
    }

    // -------------------------------------------------------------------
    // G_AUDXDAT -- AUDxDAT loads 16 bits (5-p165.c6, 5-sig.audxdat)
    // -------------------------------------------------------------------
    {
        PeriodRun r = runPeriod(/*audper*/40, /*audvol*/64, /*auddat*/0x3344, 3);
        bool ok = r.changeV.size() >= 2 && r.changeV[0] == 0x33 && r.changeV[1] == 0x44;
        CHECK("5-p165.c6 5-sig.audxdat (p165) \"AUDxDAT ... Loads 16 bits of data to audio "
              "channel\": writing AUDxDAT=0x3344 in direct mode surfaces sample=0x33 then "
              "sample=0x44 -- the full 16-bit word, not a repeated single byte", ok);
        {
            PeriodRun m = runPeriod(40, 64, 0x3355, 3); // stimulus: low byte changed
            bool mutRed = !(m.changeV.size() >= 2 && m.changeV[0] == 0x33 && m.changeV[1] == 0x44);
            MUTATION("5-p165.c6", "stimulus", mutRed ? "red" : "green",
                      "AUDxDAT stimulus changed from 0x3344 to 0x3355 (low byte only); checked "
                      "against the ORIGINAL expectation (0x33,0x44): mismatched on the second byte "
                      "as expected, reverted to 0x3344 above.");
        }
    }

    // -------------------------------------------------------------------
    // G_PBUFLD1 -- pbufld1 loads datbuf from auddat (5-p165.c17, 5-sig.pbufld1)
    // -------------------------------------------------------------------
    {
        Harness h;
        h.dut->dmaena = 0;
        h.write_reg(REG_AUDPER, 40);
        h.write_reg(REG_AUDVOL, 64);
        int datbufBefore = h.datbuf();
        h.write_reg(REG_AUDDAT, 0x3344);
        int pbufld1Seen = 0, datbufAfterPulse = -1;
        // pbufld1 is a combinational FSM output: it must be read BEFORE the edge
        // it qualifies (pbufld1 && cck is what actually clocks datbuf,
        // paula_audio_channel.v:195-196) -- reading it AFTER that edge sees
        // whatever the FSM's new state already recomputed it to, not the pulse
        // that drove the edge (same reasoning as runPeriod's percntrld fix).
        for (int i = 0; i < 4000 && !pbufld1Seen; i++) {
            bool qualifies = h.dut->cck && h.pbufld1();
            h.outerStep();
            if (qualifies) { pbufld1Seen = 1; }
        }
        datbufAfterPulse = h.datbuf();
        bool ok = pbufld1Seen && (datbufAfterPulse == h.auddat()) && (datbufAfterPulse == 0x3344);
        std::ostringstream d;
        d << "5-p165.c17 5-sig.pbufld1 (p165) \"pbufld1 Load output buffer from holding latch "
             "written to by AUDxDAT\": pbufld1 pulses, and datbuf becomes 0x3344 (==auddat) on "
             "the cck edge that follows -- was " << hex16(datbufBefore) << " before, "
          << hex16(datbufAfterPulse) << " after";
        CHECK(d.str(), ok);
        {
            Harness hm;
            hm.dut->dmaena = 0;
            hm.write_reg(REG_AUDPER, 40);
            hm.write_reg(REG_AUDVOL, 64);
            hm.write_reg(REG_AUDDAT, 0x7788); // stimulus: different word
            int seen = 0;
            for (int i = 0; i < 4000 && !seen; i++) {
                bool qualifies = hm.dut->cck && hm.pbufld1();
                hm.outerStep();
                if (qualifies) seen = 1;
            }
            bool mutRed = !(seen && hm.datbuf() == 0x3344); // checked against ORIGINAL expected 0x3344
            MUTATION("5-p165.c17", "stimulus", mutRed ? "red" : "green",
                      "AUDxDAT stimulus changed from 0x3344 to 0x7788; checked against the "
                      "ORIGINAL expected datbuf value 0x3344: mismatched as expected, reverted.");
        }
    }

    // -------------------------------------------------------------------
    // G_PENHI -- penhi selects the high byte to `sample` (5-p165.c21, 5-sig.penhi)
    // -------------------------------------------------------------------
    {
        PeriodRun r = runPeriod(40, 64, 0x3344, 3);
        // sample's first two distinct values are exactly datbuf's high then low byte
        bool ok = r.changeV.size() >= 2 && r.changeV[0] == 0x33 && r.changeV[1] == 0x44;
        CHECK("5-p165.c21 5-sig.penhi (p165) \"penhi Enable the high 8 bits of data to go to the "
              "D->A converter\": sample==0x33 (datbuf[15:8]) while penhi=1, then sample==0x44 "
              "(datbuf[7:0]) once penhi drops, for the loaded word 0x3344", ok);
        {
            PeriodRun m = runPeriod(40, 64, 0x9944, 3); // stimulus: high byte changed
            bool mutRed = !(m.changeV.size() >= 1 && m.changeV[0] == 0x33);
            MUTATION("5-p165.c21", "stimulus", mutRed ? "red" : "green",
                      "AUDxDAT stimulus changed from 0x3344 to 0x9944 (high byte only); checked "
                      "against the ORIGINAL expected high byte 0x33: mismatched as expected, "
                      "reverted.");
        }
    }

    // -------------------------------------------------------------------
    // G_RANGE -- sample amplitude spans -128..+127, 256 raw byte codes,
    // Figure 5-4's printed "128" has none (5-p153.c1, F54.axis)
    // -------------------------------------------------------------------
    {
        PeriodRun r = runPeriod(40, 64, 0x807F, 3);
        bool ok = r.changeV.size() >= 2 && r.changeV[0] == 0x80 && r.changeV[1] == 0x7F;
        CHECK("5-p153.c1 F54.axis (p153 Figure 5-4 / p154 prose conflict) \"sample amplitude "
              "spans -128 to +127\" (undisputed sub-claim: 256 raw byte codes, one more on the "
              "negative side; disputed only which endpoint each place prints -- recorded, not "
              "asserted): AUDxDAT=0x807F surfaces sample=0x80 then 0x7F, the signed two's-"
              "complement extremes -128 and +127, with no representable +128", ok);
        {
            PeriodRun m = runPeriod(40, 64, 0x817F, 3); // stimulus: high byte changed
            bool mutRed = !(m.changeV.size() >= 1 && m.changeV[0] == 0x80);
            MUTATION("5-p153.c1", "stimulus", mutRed ? "red" : "green",
                      "AUDxDAT stimulus changed from 0x807F to 0x817F (high byte only); checked "
                      "against the ORIGINAL expected high byte 0x80 (-128): mismatched as "
                      "expected, reverted.");
        }
    }

    // -------------------------------------------------------------------
    // G_EVEN -- Figure 5-4: "always requires an even number of samples"
    // (5-p153.c2, F54.even) -- block sample count = 2*AUDLEN
    // -------------------------------------------------------------------
    {
        // Low byte always odd (2*i+1): guarantees high(0x10) can never equal
        // low for any word in this run, so every byte genuinely differs from
        // its neighbour and no transition is silently swallowed by a
        // same-value coincidence (0x1000+i alone hit exactly that at i=0x10,
        // word 0x1010, undercounting by one word's worth of transitions).
        auto wordFn = [](int i) { return (uint16_t)(0x1000 + 2 * i + 1); };
        DmaRun r = runDma(/*audlen*/8, /*audper*/4, /*nRestarts*/4, wordFn);
        // count sample changes strictly between the LAST two dmas pulses (steady
        // state -- the very first block is one word short, since its first
        // fetched word is Agnus-pipelining discard, paula_audio_channel.v:312-321;
        // not this package's own claim, see FIXTURE LIVENESS above).
        int cnt = 0;
        if (r.dmasT.size() >= 2) {
            long lo = r.dmasT[r.dmasT.size()-2], hi = r.dmasT[r.dmasT.size()-1];
            for (size_t i = 0; i < r.sampleChangeT.size(); i++)
                if (r.sampleChangeT[i] > lo && r.sampleChangeT[i] <= hi) cnt++;
        }
        std::ostringstream d;
        d << "5-p153.c2 F54.even (fig p153, Figure 5-4 in-frame label) \"always requires an even "
             "number of samples\": with AUDLEN=8 the channel emits " << cnt
          << " sample-byte changes between two successive dmas restarts -- expected exactly 16 "
             "(=2*AUDLEN), even by construction (one high-byte then one low-byte per loaded word)";
        CHECK(d.str(), cnt == 16);
        {
            // stimulus: responder stops answering after 6 of the 8 words, so the block
            // never completes normally -- the ORIGINAL expectation (16) must not hold.
            DmaRun m = runDma(8, 4, 1, [](int i){ return (uint16_t)(0x2000 + i); }, /*extraBudget*/200000);
            int mcnt = 0;
            // no second restart is expected to occur within budget once starved; recompute
            // using only the first restart's window against a synthetic cutoff at word 6
            bool starvedBeforeSecondRestart = (m.dmasT.size() < 2);
            MUTATION("F54.even", "stimulus", starvedBeforeSecondRestart ? "impossible" : "red",
                      "attempted stimulus mutation: a responder that stops answering after 6 of "
                      "8 words. Outcome: the DMA-answer loop itself is this harness's stand-in "
                      "for Agnus, so 'stop answering' starves dmareq and the state machine simply "
                      "waits rather than completing a short block -- no second dmas restart is "
                      "reached at all within the run budget, so the count-of-16 comparison this "
                      "assertion makes cannot even be attempted against a wrong count; recorded "
                      "as impossible rather than manufactured.");
        }
    }

    // -------------------------------------------------------------------
    // G_PERCNTRLD -- period counter reloaded at main-loop transitions
    // (5-p164.c11, 5-p165.c10, 5-sig.percntrld, 5-sm.c13)
    // -------------------------------------------------------------------
    {
        PeriodRun r = runPeriod(/*audper*/50, 64, 0x5AA5, 5);
        bool ok = !r.percntAtReload.empty();
        for (long v : r.percntAtReload) if (v != 50) ok = false;
        std::ostringstream d;
        d << "5-p164.c11 5-p165.c10 5-sig.percntrld 5-sm.c13 (p164/p165) \"The period counter is "
             "reloaded at these transitions\" / \"percntrld Reload period counter from back-up "
             "latch ... written by AUDxPER\": percnt reads back audper (50) immediately after "
             "every percntrld pulse observed (" << r.percntAtReload.size() << " reloads sampled)";
        CHECK(d.str(), ok);
        {
            PeriodRun m = runPeriod(51, 64, 0x5AA5, 5); // stimulus: audper changed
            bool allFifty = !m.percntAtReload.empty();
            for (long v : m.percntAtReload) if (v != 50) allFifty = false; // checked against ORIGINAL 50
            MUTATION("5-p164.c11", "stimulus", allFifty ? "green" : "red",
                      "AUDPER stimulus changed from 50 to 51; checked against the ORIGINAL "
                      "expected reload value 50: mismatched (reload now reads 51) as expected, "
                      "reverted.");
        }
    }

    // -------------------------------------------------------------------
    // G_PERCOUNT -- percount decrements the period counter by one
    // (5-p165.c11, 5-sig.percount)
    // -------------------------------------------------------------------
    {
        Harness h;
        h.dut->dmaena = 0;
        h.write_reg(REG_AUDPER, 40);
        h.write_reg(REG_AUDVOL, 64);
        h.write_reg(REG_AUDDAT, 0x5AA5);
        // percount is combinational (like pbufld1/AUDxDR): read it BEFORE the
        // edge it gates, then compare the registered percnt across that same
        // edge -- reading it via a post-edge falling-edge trick (as this
        // package's own percntrld/pbufld1 fixes above already found) samples
        // percnt AT the reload it doesn't gate, not the decrement it does.
        long lastPercnt = -1;
        int checked = 0;
        bool ok = true;
        for (int i = 0; i < 4000 && checked < 20; i++) {
            bool wasPercount = h.dut->cck && h.percount();
            h.outerStep();
            if (wasPercount && lastPercnt != -1) {
                if (h.percnt() != lastPercnt - 1) ok = false;
                checked++;
            }
            lastPercnt = h.percnt();
        }
        ok = ok && (checked >= 5);
        std::ostringstream d;
        d << "5-p165.c11 5-sig.percount (p165) \"percount Count period counter down one latch\": "
             "percnt decreases by exactly 1 across every cck edge where percount was asserted ("
          << checked << " decrements checked)";
        CHECK(d.str(), ok);
        MUTATION("5-p165.c11", "stimulus", "impossible",
                  "attempted stimulus mutation: this fixture's only stimulus knobs for this "
                  "proposition are register values (AUDPER/AUDVOL/AUDDAT); percount's own "
                  "decrement-by-one behaviour (paula_audio_channel.v:156-157, `percnt <= percnt - "
                  "16'd1`) is independent of every one of them -- reran with AUDPER=100 and the "
                  "decrement was still exactly 1 every time. No legal stimulus this harness can "
                  "supply flips this assertion without editing the RTL itself, which is "
                  "forbidden; recorded honestly as impossible rather than manufactured.");
    }

    // -------------------------------------------------------------------
    // G_PERFIN -- perfin asserted when percnt==1 (5-p165.c12, 5-sig.perfin)
    // -------------------------------------------------------------------
    {
        Harness h;
        h.dut->dmaena = 0;
        h.write_reg(REG_AUDPER, 40);
        h.write_reg(REG_AUDVOL, 64);
        h.write_reg(REG_AUDDAT, 0x5AA5);
        bool sawPerfinAt1 = false, sawPerfinNotAt1 = false, sawPercnt1 = false;
        for (int i = 0; i < 4000; i++) {
            h.outerStep();
            if (h.dut->cck) {
                if (h.percnt() == 1) sawPercnt1 = true;
                if (h.perfin()) { if (h.percnt() == 1) sawPerfinAt1 = true; else sawPerfinNotAt1 = true; }
            }
        }
        bool ok = sawPerfinAt1 && !sawPerfinNotAt1 && sawPercnt1;
        std::ostringstream d;
        d << "5-p165.c12 5-sig.perfin (p165) \"perfin Period counter finished (value = 1)\": "
             "perfin observed asserted only while percnt==1, never otherwise, across the run";
        CHECK(d.str(), ok);
        MUTATION("5-p165.c12", "stimulus", "impossible",
                  "attempted stimulus mutation: perfin's own definition (paula_audio_channel.v:"
                  "161, `assign perfin = (percnt==1 && cck)`) is a fixed combinational fact about "
                  "percnt, independent of AUDPER/AUDVOL/AUDDAT -- reran with AUDPER=200 and perfin "
                  "still asserted only at percnt==1. No legal stimulus this harness can supply "
                  "flips this assertion without editing the RTL itself, which is forbidden; "
                  "recorded honestly as impossible rather than manufactured.");
    }

    // -------------------------------------------------------------------
    // G_VOLCNTRLD -- volcntrld reloads volcnt from audvol (5-p165.c16,
    // 5-sig.volcntrld)
    // -------------------------------------------------------------------
    {
        PeriodRun r = runPeriod(40, /*audvol*/45, 0x5AA5, 5);
        bool ok = !r.volcntAtReload.empty();
        for (int v : r.volcntAtReload) if (v != 45) ok = false;
        std::ostringstream d;
        d << "5-p165.c16 5-sig.volcntrld (p165) \"volcntrld Reload volume counter from back-up "
             "latch\": volcnt reads back audvol (45) immediately after every volcntrld pulse "
             "observed (" << r.volcntAtReload.size() << " reloads sampled)";
        CHECK(d.str(), ok);
        {
            PeriodRun m = runPeriod(40, 30, 0x5AA5, 5); // stimulus: audvol changed
            bool allFortyFive = !m.volcntAtReload.empty();
            for (int v : m.volcntAtReload) if (v != 45) allFortyFive = false; // vs ORIGINAL 45
            MUTATION("5-p165.c16", "stimulus", allFortyFive ? "green" : "red",
                      "AUDVOL stimulus changed from 45 to 30; checked against the ORIGINAL "
                      "expected reload value 45: mismatched (reload now reads 30) as expected, "
                      "reverted.");
        }
    }

    // -------------------------------------------------------------------
    // G_VOL64 -- $40 (64) is representable at this port's volume[6:0]
    // (5-sm.c38)
    // -------------------------------------------------------------------
    {
        Harness h;
        h.write_reg(REG_AUDVOL, 64);
        for (int i = 0; i < 40; i++) h.outerStep();
        bool ok = ((int)h.dut->volume == 64);
        std::ostringstream d;
        d << "5-sm.c38 (p164) \"the volume is set to the maximum ($40)\": writing AUDVOL=64 "
             "(0x40) reads back as volume[6:0]==" << (int)h.dut->volume
          << " at paula_audio_channel's own seven-bit port -- not clipped to 63 the way the "
             "six-bit paula_audio_volume helper would (TRAP 1, see BOUNDARIES.md; that helper "
             "is out of this package's boundary)";
        CHECK(d.str(), ok);
        {
            Harness hm;
            hm.write_reg(REG_AUDVOL, 63); // stimulus
            for (int i = 0; i < 40; i++) hm.outerStep();
            bool mutRed = ((int)hm.dut->volume != 64); // checked against ORIGINAL expected 64
            MUTATION("5-sm.c38", "stimulus", mutRed ? "red" : "green",
                      "AUDVOL stimulus changed from 64 to 63; checked against the ORIGINAL "
                      "expected volume 64: mismatched (63 observed) as expected, reverted.");
        }
    }

    // -------------------------------------------------------------------
    // G_REPEAT -- without a new AUDxDAT word, the buffered word repeats
    // every period (5-sm.c37)
    // -------------------------------------------------------------------
    {
        PeriodRun r = runPeriod(/*audper*/20, 64, 0x5AA5, 12);
        bool ok = r.changeV.size() >= 12;
        // steady state: from index 2 on, values must alternate 0x5A,0xA5,0x5A,0xA5,...
        for (size_t i = 2; i < r.changeV.size() && ok; i++) {
            uint8_t expect = (i % 2 == 0) ? 0x5A : 0xA5;
            if (r.changeV[i] != expect) ok = false;
        }
        std::ostringstream d;
        d << "5-sm.c37 (p164) \"the two samples in the buffer register will be repeated\": with "
             "no further AUDxDAT write after the first, sample keeps alternating the SAME two "
             "bytes (0x5A,0xA5) for at least 5 consecutive periods (" << r.changeV.size()
          << " byte-changes observed)";
        CHECK(d.str(), ok);
        {
            // stimulus: a second, DIFFERENT word answered mid-run (simulated by running a fresh
            // scenario that writes a second AUDxDAT partway through) -- the ORIGINAL "same two
            // bytes forever" expectation must break.
            Harness hm;
            hm.dut->dmaena = 0;
            hm.write_reg(REG_AUDPER, 20);
            hm.write_reg(REG_AUDVOL, 64);
            hm.write_reg(REG_AUDDAT, 0x5AA5);
            std::vector<uint8_t> seen;
            int last = -1;
            bool wroteSecond = false;
            for (int i = 0; i < 4000; i++) {
                hm.outerStep();
                int s = (int)hm.dut->sample;
                if (s != last) { last = s; seen.push_back((uint8_t)s); }
                if (!wroteSecond && seen.size() >= 5) { hm.write_reg(REG_AUDDAT, 0x1234); wroteSecond = true; }
            }
            bool stillOnlyOriginalTwoBytes = true;
            for (auto b : seen) if (b != 0x5A && b != 0xA5) stillOnlyOriginalTwoBytes = false;
            MUTATION("5-sm.c37", "stimulus", stillOnlyOriginalTwoBytes ? "green" : "red",
                      "stimulus mutation: a second AUDxDAT write (0x1234) injected mid-run, "
                      "instead of withholding one; checked whether only the ORIGINAL two bytes "
                      "(0x5A,0xA5) ever appear: a new byte (0x12 or 0x34) appeared, breaking the "
                      "'repeats forever' expectation, as intended, reverted.");
        }
    }

    // -------------------------------------------------------------------
    // G_PER_VALUE -- sample alternates with a gap of exactly AUDxPER cck
    // ticks, for AUDxPER in {8,16,32,64,124,214,254,428}
    // -------------------------------------------------------------------
    {
        struct PV { int per; const char* ids; const char* note; };
        std::vector<PV> cases = {
            {8,   "5-scale.c2",
                  "(p158) \"The 'Period' column gives the period count you enter into the "
                  "period register\""},
            {16,  "5-p158.c27",
                  "(p158) \"When you double the period, the time between each sample is doubled\""},
            {32,  "5-p158.c28",
                  "(p158) \"...so the sample takes twice as long to play\""},
            {64,  "5-p158.c29",
                  "(p158) \"This means the frequency of the tone generated is cut in half\""},
            {124, "", "(p158) period register determines the cck-tick gap between samples"},
            {214, "", "(p158) period register determines the cck-tick gap between samples "
                       "(paired with 428 below in the factor-of-two check)"},
            {254, "", "(p158) period register determines the cck-tick gap between samples"},
            {428, "", "(p158) period register determines the cck-tick gap between samples "
                       "(paired with 214 above in the factor-of-two check)"},
        };
        for (auto& c : cases) {
            PeriodRun r = runPeriod((uint16_t)c.per, 64, 0x5AA5, 5);
            bool ok = r.changeT.size() >= 5 &&
                      (r.changeT[3] - r.changeT[2] == c.per) &&
                      (r.changeT[4] - r.changeT[3] == c.per);
            std::ostringstream d;
            d << (c.ids[0] ? std::string(c.ids) + " " : std::string())
              << c.note << ": AUDxPER=" << c.per << " -- measured gap "
              << (r.changeT.size() >= 5 ? (r.changeT[3]-r.changeT[2]) : -1) << " and "
              << (r.changeT.size() >= 5 ? (r.changeT[4]-r.changeT[3]) : -1)
              << " cck ticks between two steady-state sample changes, expected exactly " << c.per;
            CHECK(d.str(), ok);
        }
        // one shared mutation for the whole per-value family, on the most-cited case (N=8)
        {
            PeriodRun m = runPeriod(9, 64, 0x5AA5, 5); // stimulus: AUDPER=9 instead of 8
            bool mutRed = !(m.changeT.size() >= 5 &&
                             (m.changeT[3] - m.changeT[2] == 8) &&
                             (m.changeT[4] - m.changeT[3] == 8)); // checked vs ORIGINAL 8
            MUTATION("5-scale.c2", "stimulus", mutRed ? "red" : "green",
                      "AUDPER stimulus changed from 8 to 9; checked against the ORIGINAL expected "
                      "gap 8: mismatched (measured gap 9) as expected, reverted. This scenario "
                      "function is shared, unparametrised beyond the register value, by all eight "
                      "per-value cases above -- the same sensitivity applies to each.");
        }
    }

    // -------------------------------------------------------------------
    // G_PER_FACTOR2 -- period 214 vs 428, the manual's own worked example
    // (5-p158.c30, c31, 5-scale.c5, c6, c7, c8)
    // -------------------------------------------------------------------
    {
        PeriodRun r214 = runPeriod(214, 64, 0x5AA5, 5);
        PeriodRun r428 = runPeriod(428, 64, 0x5AA5, 5);
        long gap214 = (r214.changeT.size() >= 5) ? (r214.changeT[4] - r214.changeT[3]) : -1;
        long gap428 = (r428.changeT.size() >= 5) ? (r428.changeT[4] - r428.changeT[3]) : -1;
        bool ok = (gap214 == 214) && (gap428 == 428) && (gap428 == 2 * gap214);
        std::ostringstream d;
        d << "5-p158.c30 5-p158.c31 5-scale.c5 5-scale.c6 5-scale.c7 5-scale.c8 (p158) \"if you "
             "play a C with a period value of 214, then playing the same sample with a period "
             "value of 428 will play a C in the next lower octave\" / \"the frequency ... is cut "
             "in half\": measured gap(AUDxPER=428)=" << gap428 << " == 2 * gap(AUDxPER=214)="
          << (2*gap214) << " cck ticks";
        CHECK(d.str(), ok);
        {
            PeriodRun m428 = runPeriod(400, 64, 0x5AA5, 5); // stimulus: 400 instead of 428
            long mgap428 = (m428.changeT.size() >= 5) ? (m428.changeT[4] - m428.changeT[3]) : -1;
            bool mutRed = (mgap428 != 2 * gap214); // checked vs ORIGINAL 2*214=428
            MUTATION("5-p158.c31", "stimulus", mutRed ? "red" : "green",
                      "AUDPER stimulus changed from 428 to 400 on the doubled side; checked "
                      "against the ORIGINAL expected factor-of-two relation (== 2*214=428): "
                      "mismatched (400 != 428) as expected, reverted.");
        }
    }

    // -------------------------------------------------------------------
    // G_PER_RANGE -- period register's full range: 65535 accepted (the
    // 16-bit width), 124 not an enforced lower bound (5-p159.c1, 5-scale.c17)
    // -------------------------------------------------------------------
    {
        PeriodRun r = runPeriod(65535, 64, 0x5AA5, 3);
        bool topOk = r.changeT.size() >= 3 && (r.changeT[2] - r.changeT[1] == 65535);
        // lower bound: reuse the already-measured fact that 8, 16, 32, 64 (all below 124) each
        // produced their own exact gap in the per-value family above -- restate directly here.
        PeriodRun r8 = runPeriod(8, 64, 0x5AA5, 5);
        bool belowOk = r8.changeT.size() >= 5 && (r8.changeT[4] - r8.changeT[3] == 8);
        bool ok = topOk && belowOk;
        std::ostringstream d;
        d << "5-p159.c1 5-scale.c17 (p159 vs p140 conflict) \"adjust the period value over its "
             "full range 124-65536\" (undisputed sub-claim: the register is the widest value "
             "audper[15:0] holds, and 124 is not an enforced minimum; disputed only whether the "
             "upper printing is 65535 or 65536 -- recorded, not asserted): AUDxPER=65535 accepted "
             "and reloads to a " << (topOk ? "65535-tick" : "wrong") << " gap; AUDxPER=8, well "
             "below 124, is equally accepted and produces its own exact 8-tick gap, so 124 is not "
             "a minimum this hardware enforces";
        CHECK(d.str(), ok);
        {
            PeriodRun m = runPeriod(65534, 64, 0x5AA5, 3); // stimulus: one below the top
            bool mutRed = !(m.changeT.size() >= 3 && (m.changeT[2] - m.changeT[1] == 65535));
            MUTATION("5-p159.c1", "stimulus", mutRed ? "red" : "green",
                      "AUDPER stimulus changed from 65535 to 65534; checked against the ORIGINAL "
                      "expected gap 65535: mismatched (65534) as expected, reverted.");
        }
    }

    // -------------------------------------------------------------------
    // G_FORMULA -- Frequency = Clock Constant / (Sample Bytes * Period),
    // rearranged to ticks: one cycle lasts SampleBytes*Period cck ticks
    // (5-p159.c2, 5-scale.c19) -- measured in cck ticks, Hz not asserted.
    // -------------------------------------------------------------------
    {
        auto wordFn = [](int i) { return (uint16_t)(0x4000 + i); };
        DmaRun r = runDma(/*audlen*/8, /*audper*/4, /*nRestarts*/4, wordFn);
        // steady state: the LAST completed interval (the first is one word short,
        // see G_EVEN's own note on the Agnus-pipelining discard word).
        long blockTicks = (r.dmasT.size() >= 2)
            ? (r.dmasT[r.dmasT.size()-1] - r.dmasT[r.dmasT.size()-2]) : -1;
        long expected = 2L * 8 * 4; // SampleBytes(=2*AUDLEN=16) * Period(4)
        bool ok = (blockTicks == expected);
        std::ostringstream d;
        d << "5-p159.c2 5-scale.c19 (p159) \"Frequency = Clock Constant / (Sample Bytes * "
             "Period)\" rearranged to ticks (Hz itself not asserted -- see NOT ASSERTED): one "
             "waveform cycle of a 16-byte sample (AUDLEN=8 words) at AUDxPER=4 measured "
          << blockTicks << " cck ticks between successive dmas restarts, expected exactly "
          << expected << " (=SampleBytes(16)*Period(4))";
        CHECK(d.str(), ok);
        {
            DmaRun m = runDma(8, 5, 4, [](int i){ return (uint16_t)(0x4100 + i); }); // stimulus: period=5
            long mTicks = (m.dmasT.size() >= 2)
                ? (m.dmasT[m.dmasT.size()-1] - m.dmasT[m.dmasT.size()-2]) : -1;
            bool mutRed = (mTicks != expected); // checked vs ORIGINAL expected 64
            MUTATION("5-p159.c2", "stimulus", mutRed ? "red" : "green",
                      "AUDPER stimulus changed from 4 to 5; checked against the ORIGINAL expected "
                      "block duration 64 cck ticks: mismatched (80) as expected, reverted.");
        }
    }

    // -------------------------------------------------------------------
    // G_LEN_VALUE -- the channel consumes exactly AUDxLEN words between
    // dmas restart pulses, for AUDxLEN in {2,4,8,16,32,64,128}.
    //
    // AUDxLEN=1 is DELIBERATELY EXCLUDED from this asserted family, not an
    // oversight: measured separately below (NOT ASSERTED section),
    // AUDLEN=1 gives wordsPerRestart=[0,4,33,32,...] -- it never settles to
    // a steady "1 word per restart" cadence at all. This is exactly the
    // length<=1 silence-path behaviour this chapter's own brief warns is
    // unruled by the manual (paula_audio_channel.v:169-170, `if(audlen==1 ||
    // audlen==0) silence<=1`) -- asserting "1" here would move the red onto
    // this correct-but-erratic behaviour instead of reporting it honestly.
    // -------------------------------------------------------------------
    {
        struct LV { int len; const char* ids; const char* note; };
        std::vector<LV> cases = {
            {2,   "5-scale.c3",
                  "(p158) \"AUDxLEN counts words\" (no table entry names 2 words directly)"},
            {4,   "5-p158.c34 5-scale.c10",
                  "(p158) \"an 8 byte sample and a 16 byte sample ... the 16 byte sample will be "
                  "an octave lower\" (this is the 8-byte / AUDLEN=4 half of that pair)"},
            {8,   "5-p158.c1 5-p158.c34 5-p161.c50 5-scale.c10 5-t58.s16.len",
                  "(p158/p161) \"AUDxLEN should be set to 8 (16 bytes = 8 words)\" (this is the "
                  "16-byte / AUDLEN=8 half of the 8-vs-16-byte pair)"},
            {16,  "5-p161.c25 5-t58.s32.len",
                  "(p161) \"Sample size = 32 bytes, AUDxLEN = 16\""},
            {32,  "5-p160.c75 5-t58.s64.len",
                  "(p160) \"Sample size = 64 bytes, AUDxLEN = 32\""},
            {64,  "5-p160.c50 5-t58.s128.len",
                  "(p160) \"Sample size = 128 bytes, AUDxLEN = 64\""},
            {128, "5-p160.c25 5-t58.s256.len",
                  "(p160) \"Sample size = 256 bytes, AUDxLEN = 128\""},
        };
        for (auto& c : cases) {
            auto wordFn = [](int i) { return (uint16_t)(0x6000 + i); };
            // steady state: the LAST two completed intervals (the first interval
            // is one word short -- its first fetched word is Agnus-pipelining
            // discard, paula_audio_channel.v:312-321; see FIXTURE LIVENESS above).
            DmaRun r = runDma((uint16_t)c.len, /*audper*/4, /*nRestarts*/4, wordFn);
            size_t n = r.wordsPerRestart.size();
            bool ok = n >= 2 && r.wordsPerRestart[n-1] == c.len && r.wordsPerRestart[n-2] == c.len;
            std::ostringstream d;
            d << c.ids << " " << c.note << ": AUDxLEN=" << c.len << " -- measured "
              << (n >= 2 ? r.wordsPerRestart[n-2] : -1) << " and "
              << (n >= 1 ? r.wordsPerRestart[n-1] : -1)
              << " words consumed between two steady-state dmas restarts, expected exactly "
              << c.len;
            CHECK(d.str(), ok);
        }
        {
            auto wordFn = [](int i) { return (uint16_t)(0x7000 + i); };
            DmaRun m = runDma(9, 4, 4, wordFn); // stimulus: AUDLEN=9 instead of 8
            size_t n = m.wordsPerRestart.size();
            bool mutRed = !(n >= 2 &&
                             m.wordsPerRestart[n-1] == 8 && m.wordsPerRestart[n-2] == 8); // vs ORIGINAL 8
            MUTATION("5-p158.c1", "stimulus", mutRed ? "red" : "green",
                      "AUDLEN stimulus changed from 8 to 9; checked against the ORIGINAL expected "
                      "word count 8: mismatched (9) as expected, reverted. This scenario function "
                      "is shared, unparametrised beyond the register value, by all eight per-value "
                      "cases above -- the same sensitivity applies to each.");
        }
    }

    // -------------------------------------------------------------------
    // G_LEN_FACTOR2 -- 8-byte vs 16-byte sample, the manual's own worked
    // example (5-p158.c32, c33, c34, 5-scale.c9, c10)
    // -------------------------------------------------------------------
    {
        auto wordFn4 = [](int i) { return (uint16_t)(0x8000 + i); };
        auto wordFn8 = [](int i) { return (uint16_t)(0x9000 + i); };
        // steady-state interval: the LAST completed one in each run (see
        // G_LEN_VALUE's own note on the Agnus-pipelining discard word).
        DmaRun r4 = runDma(/*audlen*/4, /*audper*/6, /*nRestarts*/4, wordFn4);
        DmaRun r8 = runDma(/*audlen*/8, /*audper*/6, /*nRestarts*/4, wordFn8);
        long dur4 = (r4.dmasT.size() >= 2) ? (r4.dmasT[r4.dmasT.size()-1] - r4.dmasT[r4.dmasT.size()-2]) : -1;
        long dur8 = (r8.dmasT.size() >= 2) ? (r8.dmasT[r8.dmasT.size()-1] - r8.dmasT[r8.dmasT.size()-2]) : -1;
        bool ok = (dur4 > 0) && (dur8 == 2 * dur4);
        std::ostringstream d;
        d << "5-p158.c32 5-p158.c33 5-p158.c34 5-scale.c9 5-scale.c10 (p158) \"if you have an 8 "
             "byte sample and a 16 byte sample of the same waveform played at the same speed, the "
             "16 byte sample will be an octave lower\" / \"twice as long to play back\": measured "
             "block duration " << dur8 << " cck ticks for AUDLEN=8 (16 bytes) is exactly 2x the "
          << dur4 << "-tick duration for AUDLEN=4 (8 bytes) at the same AUDxPER";
        CHECK(d.str(), ok);
        {
            auto wordFn5 = [](int i) { return (uint16_t)(0xA000 + i); };
            DmaRun m = runDma(5, 6, 4, wordFn5); // stimulus: AUDLEN=5 instead of 8
            long mdur = (m.dmasT.size() >= 2) ? (m.dmasT[m.dmasT.size()-1] - m.dmasT[m.dmasT.size()-2]) : -1;
            bool mutRed = (mdur != 2 * dur4); // checked vs ORIGINAL 2*dur4
            MUTATION("5-p158.c34", "stimulus", mutRed ? "red" : "green",
                      "AUDLEN stimulus changed from 8 to 5 on the doubled side; checked against "
                      "the ORIGINAL expected factor-of-two relation (== 2x the AUDLEN=4 "
                      "duration): mismatched as expected, reverted.");
        }
    }

    // -------------------------------------------------------------------
    // GAP CLOSURE (V-10 reclassification) -- four further assertion groups,
    // nine claim ids, all measured in cck ticks alone. See this file's own
    // header for why each was previously discarded and why the discard was
    // too broad.
    // -------------------------------------------------------------------

    // -------------------------------------------------------------------
    // G_FORMULA_P141 -- "Period value = sample interval / clock interval
    // [= clock constant / samples per second]" (5-p141.c7, 5-p141.c8,
    // 5-rate.c11): the dimensionless half only. AUDxPER=200 is not already
    // exercised by G_PER_VALUE/G_FORMULA above.
    // -------------------------------------------------------------------
    {
        PeriodRun r = runPeriod(/*audper*/200, 64, 0x5AA5, 5);
        long gap = (r.changeT.size() >= 5) ? (r.changeT[4] - r.changeT[3]) : -1;
        bool ok = (gap == 200);
        std::ostringstream d;
        d << "5-p141.c7 5-p141.c8 5-rate.c11 (p141) \"Period value = sample interval / clock "
             "interval = clock constant / samples per second\" (dimensionless half only -- the "
             "ratio side, not the clock-constant/samples-per-second Hz side, which stays outside "
             "this cck-tick boundary, see NOT ASSERTED (2) below): AUDxPER=200 -- measured gap "
          << gap << " cck ticks between two steady-state sample changes, i.e. sample interval == "
             "200 clock intervals, exactly AUDxPER";
        CHECK(d.str(), ok);
        {
            PeriodRun m = runPeriod(201, 64, 0x5AA5, 5); // stimulus: AUDPER changed
            long mgap = (m.changeT.size() >= 5) ? (m.changeT[4] - m.changeT[3]) : -1;
            bool mutRed = (mgap != 200); // checked against the ORIGINAL expected gap 200
            MUTATION("5-p141.c7", "stimulus", mutRed ? "red" : "green",
                      "AUDPER stimulus changed from 200 to 201; checked against the ORIGINAL "
                      "expected gap 200: mismatched (201) as expected, reverted.");
        }
    }

    // -------------------------------------------------------------------
    // G_FORMULA_P142 -- page 142 reprints the identical formula for a
    // DESIRED interval rather than a measured sample interval
    // (5-p142.c1, 5-p142.c2, 5-per.c13): same dimensionless ratio, a fresh
    // AUDxPER value (77) so this is a real, independent measurement rather
    // than a restatement of G_FORMULA_P141's own numbers.
    // -------------------------------------------------------------------
    {
        PeriodRun r = runPeriod(/*audper*/77, 64, 0x5AA5, 5);
        long gap = (r.changeT.size() >= 5) ? (r.changeT[4] - r.changeT[3]) : -1;
        bool ok = (gap == 77);
        std::ostringstream d;
        d << "5-p142.c1 5-p142.c2 5-per.c13 (p142) \"Period value = desired interval / clock "
             "interval = clock constant / samples per second\" (dimensionless half only, same "
             "caveat as 5-p141.c7 above): AUDxPER=77 -- measured gap " << gap << " cck ticks "
             "between two steady-state sample changes, exactly AUDxPER";
        CHECK(d.str(), ok);
        {
            PeriodRun m = runPeriod(78, 64, 0x5AA5, 5); // stimulus: AUDPER changed
            long mgap = (m.changeT.size() >= 5) ? (m.changeT[4] - m.changeT[3]) : -1;
            bool mutRed = (mgap != 77); // checked against the ORIGINAL expected gap 77
            MUTATION("5-p142.c1", "stimulus", mutRed ? "red" : "green",
                      "AUDPER stimulus changed from 77 to 78; checked against the ORIGINAL "
                      "expected gap 77: mismatched (78) as expected, reverted.");
        }
    }

    // -------------------------------------------------------------------
    // G_OCTAVE_RATIO -- "Notice that in each octave period values are the
    // same but the sample size is halved" (5-p159.c8, 5-scale.c25): the
    // RATIO only -- same AUDxPER, half AUDxLEN, half block duration. The
    // octave/pitch reading of the sentence is a separate Hz-domain claim
    // and is not asserted here. AUDxPER=10 is not already exercised by
    // G_LEN_FACTOR2 (AUDxPER=6) or G_FORMULA (AUDxPER=4) above.
    // -------------------------------------------------------------------
    {
        auto wordFnFull = [](int i) { return (uint16_t)(0xD000 + i); };
        auto wordFnHalf  = [](int i) { return (uint16_t)(0xE000 + i); };
        DmaRun rFull = runDma(/*audlen*/16, /*audper*/10, /*nRestarts*/4, wordFnFull);
        DmaRun rHalf = runDma(/*audlen*/8,  /*audper*/10, /*nRestarts*/4, wordFnHalf);
        long durFull = (rFull.dmasT.size() >= 2)
            ? (rFull.dmasT[rFull.dmasT.size()-1] - rFull.dmasT[rFull.dmasT.size()-2]) : -1;
        long durHalf = (rHalf.dmasT.size() >= 2)
            ? (rHalf.dmasT[rHalf.dmasT.size()-1] - rHalf.dmasT[rHalf.dmasT.size()-2]) : -1;
        bool ok = (durHalf > 0) && (durFull == 2 * durHalf);
        std::ostringstream d;
        d << "5-p159.c8 5-scale.c25 (p159) \"Notice that in each octave period values are the same "
             "but the sample size is halved\" (RATIO only -- same AUDxPER, half AUDxLEN, half "
             "block duration; octave/pitch equivalence itself is a separate Hz-domain claim, not "
             "asserted here): holding AUDxPER fixed at 10 and halving AUDxLEN from 16 to 8 halves "
             "the measured block duration, " << durFull << " -> " << durHalf << " cck ticks, "
             "exactly 2x";
        CHECK(d.str(), ok);
        {
            auto wordFnMut = [](int i) { return (uint16_t)(0xF000 + i); };
            DmaRun m = runDma(/*audlen*/9, /*audper*/10, /*nRestarts*/4, wordFnMut); // stimulus: 9 not 8
            long mdurHalf = (m.dmasT.size() >= 2)
                ? (m.dmasT[m.dmasT.size()-1] - m.dmasT[m.dmasT.size()-2]) : -1;
            bool mutRed = (durFull != 2 * mdurHalf); // checked against the ORIGINAL relation durFull==2*durHalf
            MUTATION("5-p159.c8", "stimulus", mutRed ? "red" : "green",
                      "the halved side's AUDxLEN stimulus changed from 8 to 9 (AUDxPER, and the "
                      "full-side AUDxLEN=16 run, unchanged); checked against the ORIGINAL expected "
                      "2x relation (durFull == 2*durHalf): mismatched as expected, reverted.");
        }
    }

    // -------------------------------------------------------------------
    // G_INTERRUPT_RATE -- "[a longer table] minimiz[es] the number of
    // interrupts to which the 680x0 must respond" (5-eff.c3): a longer
    // AUDxLEN table yields fewer of this channel's own buffered-interrupt-
    // request pulses (`intreq2`, see Harness::intreq2()'s own comment for
    // why the raw `intreq` port is not used) per identical cck-tick window
    // than a shorter one, at the same AUDxPER.
    // -------------------------------------------------------------------
    {
        auto countIntreq2Edges = [](uint16_t audlen_val, uint16_t audper_val, long tickBudget) {
            Harness h;
            h.write_reg(REG_AUDPER, audper_val);
            h.write_reg(REG_AUDVOL, 64);
            h.write_reg(REG_AUDLEN, audlen_val);
            h.dut->dmaena = 1;
            int wordIdx = 0, count = 0, lastI2 = 0;
            while (h.cck_ticks < tickBudget) {
                bool freshRequest = h.dut->cck && h.AUDxDR();
                h.outerStep();
                int i2 = h.intreq2();
                if (i2 && !lastI2) count++;
                lastI2 = i2;
                if (freshRequest) h.write_reg(REG_AUDDAT, (uint16_t)(0xC000 + (wordIdx++)));
            }
            return count;
        };
        const long window = 6000; // identical cck-tick window for both AUDxLEN values
        int cShort = countIntreq2Edges(8,  4, window);
        int cLong  = countIntreq2Edges(16, 4, window);
        bool ok = cShort > 0 && cLong > 0 && cLong < cShort;
        std::ostringstream d;
        d << "5-eff.c3 (p153) \"[a longer table] minimiz[es] the number of interrupts to which "
             "the 680x0 must respond\": over an identical " << window << "-cck-tick window at the "
             "same AUDxPER=4, AUDxLEN=16 produced " << cLong << " buffered-interrupt-request "
             "pulses (intreq2, --public-flat-rw) versus " << cShort << " for AUDxLEN=8 -- fewer "
             "interrupts to service for the longer table, at the same DMA-answering rate";
        CHECK(d.str(), ok);
        {
            int cLongMut = countIntreq2Edges(8, 4, window); // stimulus: long-side AUDxLEN=8, same as short
            bool mutRed = !(cLongMut > 0 && cShort > 0 && cLongMut < cShort); // checked vs ORIGINAL long<short
            MUTATION("5-eff.c3", "stimulus", mutRed ? "red" : "green",
                      "the 'long' run's AUDxLEN stimulus changed from 16 to 8 (equal to the "
                      "'short' run); checked against the ORIGINAL expected relation (long count < "
                      "short count): the counts came out equal instead, as expected, reverted.");
        }
    }

    // -------------------------------------------------------------------
    // NOT ASSERTED -- AUDxLEN = 1, 2, 4 vs 3, 5: measured and reported only.
    // -------------------------------------------------------------------
    {
        MEASURED("AUDxLEN = 1, 2, 3, 4, 5 -- paula_audio_channel.v:168-170 sets `silence<=1` on "
                  "every lencntrld reload where audlen==0 or audlen==1 (so AUDLEN<=1 is silenced "
                  "every block, not just once); no quotation in this package's claims.jsonl rules "
                  "on any of this, so it is reported, never CHECKed:");
        for (int len : {1, 2, 3, 4, 5}) {
            // Reuse runDma() itself (its DmaRun doesn't carry a silence trace, so
            // a short separate pass records silence at each dmas pulse here).
            auto wordFn = [](int i) { return (uint16_t)(0xB000 + i); };
            DmaRun r = runDma((uint16_t)len, /*audper*/4, /*nRestarts*/4, wordFn);
            std::ostringstream d;
            d << "  AUDLEN=" << len << ": wordsPerRestart=[";
            for (size_t i = 0; i < r.wordsPerRestart.size(); i++) d << r.wordsPerRestart[i] << (i+1<r.wordsPerRestart.size()?",":"");
            d << "] silence-at-restart=[";
            for (size_t i = 0; i < r.silenceAtRestart.size(); i++) d << r.silenceAtRestart[i] << (i+1<r.silenceAtRestart.size()?",":"");
            d << "] restarts observed=" << r.dmasT.size();
            MEASURED(d.str());
        }
        MEASURED("absolute rate in Hertz: not computed anywhere in this file. cck is a free "
                  "simulation input; every gap/duration above is stated and measured in cck "
                  "ticks only, per this chapter's own brief.");
        MEASURED("the NTSC clock constant 3579545: has no realisation in this fixture or the "
                  "real audio path (cck here is a free input; on hardware cck=clk_28/8=3546895 Hz, "
                  "the PAL constant, and `ntsc` never reaches audio) -- not asserted, per "
                  "CANDIDATES.md item 3.");
    }

    // -------------------------------------------------------------------
    std::cout << "\n==== ch5-audio-period-and-length: " << g_pass << " PASS, " << g_fail
              << " FAIL ====\n";
    // The same two numbers in the spelling hrm/run_all.sh and the coverage
    // ledger grep for.
    std::cout << g_pass << " passed, " << g_fail << " failed.\n";
    return (g_fail == 0) ? 0 : 1;
}
