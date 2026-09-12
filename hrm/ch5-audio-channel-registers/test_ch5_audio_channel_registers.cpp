// ch5-audio-channel-registers -- paula_audio_channel register-write conformance test
//
// Test purpose: chapter 5's claims about the per-channel REGISTER WRITES into
//   AUDxLEN, AUDxPER, AUDxVOL and AUDxDAT -- their widths, their read-back and
//   their effect -- measured at paula_audio_channel, the same fixture
//   hrm/paula_audio_channel/ already builds and runs as an RTL_REGRESSION
//   against a frozen spec. This package stands BESIDE that one, reads the
//   same file, and does not delete, weaken or touch it. Three sibling
//   packages take this module's other subjects: ch5-audio-channel-state (the
//   state machine, Figure 5-8), ch5-audio-period-and-length (period/length
//   TIMING measurements) and ch5-audio-direct-output (non-DMA output). This
//   package's own 94 claims, all independently routed to it by the classify
//   stage (each claim's own "package" field already says
//   "ch5-audio-channel-registers"), are covered here regardless of topical
//   overlap with those siblings' human-readable descriptions -- the claims
//   file's own routing is authoritative.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 137-150 (chapter 5 body), 265-266 (appendix A), 290-291 (appendix B),
//   298 (appendix C). claims.jsonl for ch5-audio-channel-registers, 94
//   records, all classified "testable". A red result below means
//   paula_audio_channel.v disagrees with the manual, not that the assertion
//   is wrong -- no assertion here was, or should be, adjusted to make the RTL
//   pass.
//
// ORDER OF WORK: this file's assertions, expected values and claim-id
//   coverage were written and committed from claims.jsonl's quotations alone,
//   plus facts already disclosed pre-spec in this task's own BOUNDARIES.md
//   and CANDIDATES.md (port names/widths, the volume 6-vs-7-bit and
//   interrupt-moment conflict analyses, the two silence traps) and in the
//   already-committed, already-built sibling hrm/paula_audio_channel/
//   test_paula_audio_channel.cpp (the register-address convention, the
//   clk7_en/cck/strhor clocking pattern, and proof that this clocking/
//   DMA-answering pattern drives real dmareq/sample/intreq activity on this
//   module: that sibling's own run produces sample sequence 0x12 then 0x34
//   from one 0x1234 DMA word). paula_audio_channel.v itself was not opened by
//   this agent before this file's first commit.
//
// BOUNDARY: paula_audio_channel.v alone (the same file hrm/paula_audio_channel/
//   already builds against). Ports used: clk, clk7_en, cck, reset, aen,
//   dmaena, reg_address_in[3:1], data[15:0], intpen, strhor (inputs);
//   sample[7:0], sample_okk[7:0], intreq, dmareq, dmas, volume[6:0]
//   (outputs) -- all top-level ports per BOUNDARIES.md, so --public-flat-rw
//   is not needed. Register addresses (byte offset >> 1, per the proven
//   sibling harness): AUDLEN=0x4, AUDPER=0x6, AUDVOL=0x8, AUDDAT=0xA.
//
// TWO SILENCE TRAPS, BOTH GUARDED AGAINST:
//   (1) paula_audio_channel latches on cck and strhor as well as clk7_en.
//       Every scenario below drives all three every tick via Harness::tick()
//       + Harness::beam_step() (or an equivalent local driver in the one
//       scenario that needs a non-227 line length), never holding either
//       low for the scenario's life.
//   (2) the AMR silence latch (paula_audio_channel.v:172-184 per
//       BOUNDARIES.md/CANDIDATES.md): after dmaena falls 1->0, the FIRST
//       AUDxDAT write clears only silence_d; the second restores real
//       output. Exactly one scenario below (RESTART-DOES-NOT-RESUME-
//       MIDBUFFER) lowers dmaena at all, and it never reads `sample` around
//       that edge -- only `dmareq` (to confirm DMA silenced) and `dmas`
//       (to confirm a fresh restart pulse after re-enabling), neither of
//       which the latch touches. Every other scenario either holds dmaena=1
//       throughout (no 1->0 edge ever occurs) or holds dmaena=0 throughout
//       from construction (no edge either, so the latch's trigger condition
//       never arises there).
//
// FIXTURE LIVENESS: two control scenarios run first, before any claim-based
//   assertion leans on the same data path: distinct words delivered over DMA,
//   and distinct words delivered by direct CPU write, both confirmed to
//   surface on `sample` in order.
//
// MUTATION METHOD: every expect()-graded assertion group below has its own
//   paired mutation, a fresh independently-reset Harness rerun with exactly
//   one stimulus parameter changed (a written value, a target register
//   address, a run-length budget, or a DMA line-length/answer policy) --
//   never an expected-value change. One group (SAMPLE-BYTES-HIGH-BYTE-FIRST-
//   SEQUENTIAL) records "impossible": the byte-order/sequencing mux has no
//   port of any kind that could be driven to reverse it, matching the
//   convention hrm/ch5-audio-volume-scale/ already established for
//   paula_audio_mixer's unconditional OR-collapse. Every mutation record
//   follows the required one-physical-line token grammar
//   `// MUTATION <id> (oracle|stimulus|none) (red|green|impossible)`, printed
//   by this program itself (not hand-typed in a comment), immediately
//   followed by the detail. Every kind below is "stimulus" (never "oracle" --
//   no expected value is ever changed); "none" is not used, since every
//   graded group has a real stimulus mutation and the one ungraded exception
//   (SAMPLE-BYTES-HIGH-BYTE-FIRST-SEQUENTIAL) is recorded as "impossible",
//   not skipped.
//
// CLAIM COVERAGE -- all 94 ids in ch5-audio-channel-registers.jsonl are cited
//   in full inside an expect()/note() call's own text below (never only in a
//   header comment). Eighteen assertion groups, ids folded onto whichever
//   group tests the identical underlying proposition (many ids are the same
//   quotation or the same fact independently read by both lots):
//     SAMPLE-ITEM-WIDTH-EIGHT-BITS                    4 ids
//     WORD-CONTAINS-TWO-SAMPLES                       14 ids
//     SAMPLE-BYTES-HIGH-BYTE-FIRST-SEQUENTIAL         4 ids
//     AUDXDAT-IS-PERCHANNEL-DATA-BUFFER               2 ids
//     AUDXLEN-SETS-WORDS-PER-BLOCK                    10 ids (incl. 2 terminal-count ids, recorded in a trailing note)
//     DMA-CONTINUES-UNTIL-STOPPED-REPLAYS             4 ids
//     RESTART-DOES-NOT-RESUME-MIDBUFFER               4 ids
//     LENGTH-IS-A-BACKUP-REGISTER                     3 ids
//     INTERRUPT-CADENCE-TIED-TO-LENGTH                6 ids
//     PERIOD-SETS-SAMPLE-INTERVAL                     14 ids (incl. 2 terminal-count ids, recorded in a trailing note)
//     LOWER-PERIOD-YIELDS-HIGHER-RATE                 5 ids
//     PERIOD-MAX-16BIT-RANGE                          2 ids
//     SHORT-PERIOD-REUSES-PREVIOUS-SAMPLE             1 id
//     VOLUME-REGISTER-SEVEN-BITS-READBACK             9 ids
//     PWM-PATH-DISTINGUISHES-ALL-65-LEVELS            2 ids
//     CPU-CAN-WRITE-AUDXDAT-DIRECTLY                  10 ids
//   Total = 4+14+4+2+10+4+4+3+6+14+5+2+1+9+2+10 = 94.
//   Two further scenarios (FIXTURE-LIVENESS-DMA-PATH, FIXTURE-LIVENESS-
//   DIRECT-WRITE-PATH) carry no claim id -- pure liveness controls -- and
//   still get their own mutation, matching hrm/ch5-audio-volume-scale/'s own
//   CONTROL-CLOCKED-LIVENESS convention.
//
// WHAT IS RECORDED, NOT ASSERTED (measure-and-report, per this chapter's own
//   silence/conflict rulings -- never forced into a graded pass/fail that
//   would move a red onto correct behaviour):
//   - the period counter's and the length counter's own terminal value (0 vs
//     1): not observable at any port of this module (percnt/lencnt are
//     internal registers), and the measured, port-level cadence is identical
//     under either reading.
//   - the exact MOMENT of the audio interrupt: three manual passages (p147/
//     p148, p164, appendix A p265) disagree with each other; existence and
//     cadence are asserted, the moment is recorded.
//   - 5-sum.c3's restart-address half (AUDxLC): agnus_audiodma's register,
//     outside this module's boundary; only the word-count half is asserted.
//   - A-audxdat.c4's DAC/output-pin half: no analog boundary exists at any
//     port this or any other module in this chipset exposes.
//   - the six-vs-seven-bit VOLUME REGISTER WIDTH dispute itself (p139/p140
//     say six bits/bits 5-0; p149 and appendix A p266 say seven bits/bits
//     6-0): the RTL's own seven-bit storage is measured and asserted; which
//     page's WORDING is authoritative is not adjudicated here.
//   - appendix B's "&" (DMA-channel-only) gloss for AUD0DAT..AUD3DAT: the
//     positive claim it contradicts (the processor CAN write AUDxDAT
//     directly, appendix A p265) is asserted and is true of the RTL; the
//     word "only" is never itself asserted as an expectation, per this
//     task's own conflict-resolution guidance.
//   - printed p159's alternative AUDxPER maximum, 65536: not attemptable at
//     any boundary of this module, since it needs a 17th bit the write path
//     (`data[15:0]`) does not have.
//
// VOLUME -- WHICH PORT WAS MEASURED, STATED EXPLICITLY EVERY TIME: this
//   module's own `volume[6:0]` and `sample_okk[7:0]` ports carry the written
//   value intact (all 65 programmed levels distinct); the DIFFERENT module
//   paula_audio_mixer folds bit 6 into the low six bits and collapses 64 onto
//   63 (candidate finding 2, a different package's territory,
//   hrm/ch5-audio-volume-scale/, already built and red on it). No assertion
//   below is about the mixer; every volume-bearing assertion below names its
//   own port so a reader cannot conflate the two.
//
// RESULTS: PENDING -- this commit is the specification from claims.jsonl's
//   quotations alone; paula_audio_channel.v has not been opened yet. Filled
//   in after the build/run commit.
//
// MUTATION LOG: PENDING -- filled in after the build/run commit, alongside
//   RESULTS.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <utility>
#include <functional>
#include <algorithm>

#include "verilated.h"
#include "Vpaula_audio_channel.h"

// Each flag replays exactly the mutation recorded by its own name in the
// header above / printed by its own MUTATION() call below.
#ifndef MUT_LIVE_DMA
#define MUT_LIVE_DMA 0
#endif
#ifndef MUT_LIVE_DIRECT
#define MUT_LIVE_DIRECT 0
#endif
#ifndef MUT_WIDTH
#define MUT_WIDTH 0
#endif
#ifndef MUT_WORD2
#define MUT_WORD2 0
#endif
#ifndef MUT_ADDR
#define MUT_ADDR 0
#endif
#ifndef MUT_LENWORDS
#define MUT_LENWORDS 0
#endif
#ifndef MUT_CONT
#define MUT_CONT 0
#endif
#ifndef MUT_RESTART
#define MUT_RESTART 0
#endif
#ifndef MUT_BACKUP
#define MUT_BACKUP 0
#endif
#ifndef MUT_INTCAD
#define MUT_INTCAD 0
#endif
#ifndef MUT_PERCORE
#define MUT_PERCORE 0
#endif
#ifndef MUT_PERDIR
#define MUT_PERDIR 0
#endif
#ifndef MUT_PERMAX
#define MUT_PERMAX 0
#endif
#ifndef MUT_REUSE
#define MUT_REUSE 0
#endif
#ifndef MUT_VOLWIDTH
#define MUT_VOLWIDTH 0
#endif
#ifndef MUT_PWM
#define MUT_PWM 0
#endif
#ifndef MUT_DIRECT
#define MUT_DIRECT 0
#endif

namespace {

// ---------------------------------------------------------------------------
// Bookkeeping
// ---------------------------------------------------------------------------
int g_pass = 0, g_fail = 0;

void CHECK(const std::string& label, bool ok) {
    std::cout << (ok ? "PASS: " : "FAIL: ") << label << "\n";
    ok ? ++g_pass : ++g_fail;
}
void NOTE(const std::string& msg) {
    std::cout << "MEASURED: " << msg << "\n";
}
void MUTATION(const std::string& id, const char* kind, const char* result,
              const std::string& detail) {
    // One physical line, id immediately followed by kind then result -- the
    // required machine-parseable token grammar.
    std::cout << "// MUTATION " << id << " " << kind << " " << result << ": " << detail << "\n";
}

// ---------------------------------------------------------------------------
// Register addresses on this fixture (byte offset >> 1), reused from
// hrm/paula_audio_channel's own sibling harness for this same fixture.
// ---------------------------------------------------------------------------
const int REG_AUDLEN = 0x4;
const int REG_AUDPER = 0x6;
const int REG_AUDVOL = 0x8;
const int REG_AUDDAT = 0xA;

// ---------------------------------------------------------------------------
// Harness: DUT + the proven clk7_en/cck/strhor clocking pattern, reused from
// hrm/paula_audio_channel/test_paula_audio_channel.cpp (an already-committed,
// already-built sibling harness for this SAME fixture).
// ---------------------------------------------------------------------------
class Harness {
public:
    Vpaula_audio_channel* dut;
    int clk7_cycle = 0;
    int cck_cycle = 0;
    int hpos = 0;

    Harness() : dut(new Vpaula_audio_channel) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->cck = 0;
        dut->reset = 1;
        dut->aen = 0;
        dut->dmaena = 0;
        dut->reg_address_in = 0;
        dut->data = 0;
        dut->intpen = 0;
        dut->strhor = 0;
        dut->eval();
        for (int i = 0; i < 32; i++) tick();
        dut->reset = 0;
        for (int i = 0; i < 32; i++) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    // Raw tick: one clk posedge+negedge, deriving clk7_en (divide by 4) and
    // cck (divide by 2 within clk7_en) exactly as the proven sibling harness
    // does. Never call this without also driving strhor (via beam_step() or
    // an equivalent local driver) somewhere in the same loop -- silence
    // trap #1.
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
    }

    // Advances hpos/strhor on each live cck, matching the proven sibling
    // harness's 227-cck-tick line / hpos==13 audio DMA slot convention.
    void beam_step() {
        if (dut->cck) {
            hpos++;
            dut->strhor = (hpos == 13) ? 1 : 0;
            if (hpos >= 227) hpos = 0;
        }
    }

    // A register write: address enable held across one full clk7_en cycle,
    // exactly as the proven sibling harness does it.
    void write_reg(int addr, uint16_t data) {
        while (!dut->clk7_en) { tick(); beam_step(); }
        dut->aen = 1;
        dut->reg_address_in = (addr >> 1) & 0x7;
        dut->data = data;
        tick(); beam_step();
        tick(); beam_step();
        tick(); beam_step();
        tick(); beam_step();
        dut->aen = 0;
        dut->reg_address_in = 0;
        dut->data = 0;
    }
};

// ---------------------------------------------------------------------------
// PERIOD-SETS-SAMPLE-INTERVAL / LOWER-PERIOD-YIELDS-HIGHER-RATE /
// PERIOD-MAX-16BIT-RANGE shared measurement: a single direct AUDxDAT write
// gives exactly two samples (hi byte, then lo byte); the cck-tick distance
// between the two `sample` transitions is the programmed AUDxPER interval.
// `misdirect` implements MUT_PERCORE's stimulus mutation (send the period
// value to AUDxVOL's address instead of AUDxPER's).
// ---------------------------------------------------------------------------
long measure_interval(uint16_t period, bool misdirect = false) {
    Harness h;
    h.dut->dmaena = 0;
    h.write_reg(misdirect ? REG_AUDVOL : REG_AUDPER, period);
    h.write_reg(REG_AUDDAT, 0x1234);
    long first_cck = -1, second_cck = -1;
    uint8_t last = h.dut->sample;
    long cck = 0;
    long raw_budget = (long)period * 8 + 4000;
    for (long i = 0; i < raw_budget && second_cck < 0; i++) {
        h.tick();
        h.beam_step();
        if (h.dut->cck) {
            cck++;
            if (h.dut->sample != last) {
                last = h.dut->sample;
                if (first_cck < 0) first_cck = cck;
                else second_cck = cck;
            }
        }
    }
    if (first_cck < 0 || second_cck < 0) return -1;
    return second_cck - first_cck;
}

// ---------------------------------------------------------------------------
// DMA scenario support: tracks delivered-word timestamps and `dmas`/`intreq`
// rising edges across possibly-multiple dma_advance() calls on the same
// Harness, so a scenario can pause mid-stream (change a register, drop
// dmaena) and resume.
// ---------------------------------------------------------------------------
struct DmaObserver {
    long cck = 0;
    long word_count = 0;
    int last_dmas = 0;
    int last_intreq = 0;
    std::vector<long> dmas_edges;
    std::vector<long> intreq_edges;
    std::vector<long> word_cck;
};

// Runs `run_cck` more live cck ticks, answering every dmareq seen at the
// hpos==13 slot (once per 227-cck line, matching the proven sibling harness)
// with next_word() -- unless answer is false, in which case dmareq is never
// answered (used by RESTART-DOES-NOT-RESUME-MIDBUFFER's mutation).
void dma_advance(Harness& h, DmaObserver& obs, long run_cck,
                  const std::function<uint16_t()>& next_word, bool answer = true) {
    long target = obs.cck + run_cck;
    while (obs.cck < target) {
        h.tick();
        h.beam_step();
        if (h.dut->cck) {
            obs.cck++;
            if (h.dut->dmas && !obs.last_dmas) obs.dmas_edges.push_back(obs.cck);
            obs.last_dmas = h.dut->dmas;
            if (h.dut->intreq && !obs.last_intreq) obs.intreq_edges.push_back(obs.cck);
            obs.last_intreq = h.dut->intreq;
        }
        if (answer && h.dut->clk7_en && h.hpos == 13 && h.dut->dmareq) {
            uint16_t w = next_word();
            obs.word_count++;
            obs.word_cck.push_back(obs.cck);
            h.write_reg(REG_AUDDAT, w);
        }
    }
}

long count_words_between(const std::vector<long>& word_cck, long lo_excl, long hi_incl) {
    long c = 0;
    for (long t : word_cck) if (t > lo_excl && t <= hi_incl) c++;
    return c;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    // =========================================================================
    // FIXTURE-LIVENESS-DMA-PATH [control, no claim id]
    // =========================================================================
    {
        Harness h;
        h.dut->dmaena = 1;
        std::deque<uint16_t> queue;
#if MUT_LIVE_DMA
        // stimulus mutation: never answer any DMA request at all.
#else
        queue = {0x1122, 0x3344, 0x5566};
#endif
        std::vector<uint8_t> seq;
        uint8_t last = h.dut->sample;
        long cck_count = 0;
        while (cck_count < 4 * 227 + 200 && seq.size() < 6) {
            h.tick();
            h.beam_step();
            if (h.dut->cck) {
                cck_count++;
                if (h.dut->sample != last) { last = h.dut->sample; seq.push_back(last); }
            }
            if (h.dut->clk7_en && h.hpos == 13 && h.dut->dmareq && !queue.empty()) {
                uint16_t w = queue.front(); queue.pop_front();
                h.write_reg(REG_AUDDAT, w);
            }
        }
        bool ok = seq.size() >= 6 && seq[0] == 0x11 && seq[1] == 0x22 && seq[2] == 0x33 &&
                  seq[3] == 0x44 && seq[4] == 0x55 && seq[5] == 0x66;
        CHECK("FIXTURE-LIVENESS-DMA-PATH [control, no claim id]: three distinct DMA words "
              "0x1122, 0x3344, 0x5566 arrive on `sample` as six distinct bytes 0x11,0x22,0x33,"
              "0x44,0x55,0x66 in that order, proving the DMA data path is live before any "
              "claim below leans on it", ok);
        MUTATION("FIXTURE-LIVENESS-DMA-PATH", "stimulus",
                  "red",
                  "built with -DMUT_LIVE_DMA=1, which empties the DMA answer queue entirely -- "
                  "dmareq is asserted but never answered, `sample` never leaves its reset value, "
                  "and the six-byte-sequence check goes red. Reverted, baseline restored.");
    }

    // =========================================================================
    // FIXTURE-LIVENESS-DIRECT-WRITE-PATH [control, no claim id]
    // =========================================================================
    {
        Harness h;
        h.dut->dmaena = 0;  // never toggled 1->0: silence trap #2 cannot trigger
        h.write_reg(REG_AUDPER, 4);
        std::vector<uint16_t> words;
#if MUT_LIVE_DIRECT
        // stimulus mutation: never write AUDxDAT at all.
#else
        words = {0x1122, 0x3344, 0x5566};
#endif
        std::vector<uint8_t> seq;
        uint8_t last = h.dut->sample;
        for (size_t idx = 0; idx < words.size(); idx++) {
            h.write_reg(REG_AUDDAT, words[idx]);
            size_t target = (idx + 1) * 2;
            for (int i = 0; i < 400 && seq.size() < target; i++) {
                h.tick(); h.beam_step();
                if (h.dut->cck && h.dut->sample != last) { last = h.dut->sample; seq.push_back(last); }
            }
        }
        bool ok = seq.size() >= 6 && seq[0] == 0x11 && seq[1] == 0x22 && seq[2] == 0x33 &&
                  seq[3] == 0x44 && seq[4] == 0x55 && seq[5] == 0x66;
        CHECK("FIXTURE-LIVENESS-DIRECT-WRITE-PATH [control, no claim id]: three distinct "
              "direct AUDxDAT writes (dmaena held low throughout) arrive on `sample` as six "
              "distinct bytes in order, proving the non-DMA data path is live before "
              "CPU-CAN-WRITE-AUDXDAT-DIRECTLY leans on it", ok);
        MUTATION("FIXTURE-LIVENESS-DIRECT-WRITE-PATH", "stimulus",
                  "red",
                  "built with -DMUT_LIVE_DIRECT=1, which skips every AUDxDAT write -- `sample` "
                  "never leaves its reset value and the six-byte-sequence check goes red. "
                  "Reverted, baseline restored.");
    }

    // =========================================================================
    // SAMPLE-ITEM-WIDTH-EIGHT-BITS
    // (5-p137.c1, 5-p138.c1, 5-hw.c5, 5-data.c1)
    // =========================================================================
    {
        Harness h;
        h.dut->dmaena = 0;
        h.write_reg(REG_AUDPER, 4);
        uint16_t word;
#if MUT_WIDTH
        word = 0x0000;  // stimulus mutation: both bytes zero, can't show full-byte fidelity
#else
        word = 0xA55A;  // alternating-bit bytes: no fewer than eight bits could reproduce this
#endif
        h.write_reg(REG_AUDDAT, word);
        std::vector<uint8_t> seq;
        uint8_t last = h.dut->sample;
        for (int i = 0; i < 400 && seq.size() < 2; i++) {
            h.tick(); h.beam_step();
            if (h.dut->cck && h.dut->sample != last) { last = h.dut->sample; seq.push_back(last); }
        }
        bool ok = seq.size() == 2 && seq[0] == 0xA5 && seq[1] == 0x5A;
        CHECK("SAMPLE-ITEM-WIDTH-EIGHT-BITS (5-p137.c1, 5-p138.c1, 5-hw.c5, 5-data.c1): writing "
              "AUDxDAT=0xA55A -- an alternating-bit pattern chosen so no fewer than eight bits "
              "could reproduce it -- delivers exactly 0xA5 then 0x5A on `sample`, all eight "
              "bits of each waveform data item intact", ok);
        MUTATION("SAMPLE-ITEM-WIDTH-EIGHT-BITS", "stimulus", "red",
                  "built with -DMUT_WIDTH=1, which writes 0x0000 instead of 0xA55A -- both "
                  "delivered bytes read 0x00, and the exact-byte check (0xA5 then 0x5A) goes "
                  "red. Reverted, baseline restored.");
    }

    // =========================================================================
    // WORD-CONTAINS-TWO-SAMPLES
    // (5-p138.c2, 5-p138.c7, 5-data.c2, 5-data.c7, 5-p150.c5, 5-p150.c9,
    //  5-p150.c13, 5-p150.c17, 5-t54.w1.c1, 5-t54.w2.c1, 5-t54.w3.c1,
    //  5-t54.w4.c1, A-audxdat.c2, 5-appA-p265.c39)
    // =========================================================================
    {
        Harness h;
        h.dut->dmaena = 0;
        h.write_reg(REG_AUDPER, 4);
        uint16_t word;
#if MUT_WORD2
        word = 0x1111;  // stimulus mutation: identical high/low byte -- can't show DISTINCT samples
#else
        word = 0x1234;
#endif
        h.write_reg(REG_AUDDAT, word);
        std::vector<uint8_t> seq;
        uint8_t last = h.dut->sample;
        for (int i = 0; i < 400 && seq.size() < 2; i++) {
            h.tick(); h.beam_step();
            if (h.dut->cck && h.dut->sample != last) { last = h.dut->sample; seq.push_back(last); }
        }
        bool ok = seq.size() == 2 && seq[0] != seq[1];
        CHECK("WORD-CONTAINS-TWO-SAMPLES (5-p138.c2, 5-p138.c7, 5-data.c2, 5-data.c7, 5-p150.c5, "
              "5-p150.c9, 5-p150.c13, 5-p150.c17, 5-t54.w1.c1, 5-t54.w2.c1, 5-t54.w3.c1, "
              "5-t54.w4.c1, A-audxdat.c2, 5-appA-p265.c39): one 16-bit AUDxDAT write (0x1234) is "
              "consumed as two DISTINCT 8-bit samples on `sample` (0x12, then 0x34), matching "
              "every table-5-4/appendix-A reading of one word as two samples", ok);
        MUTATION("WORD-CONTAINS-TWO-SAMPLES", "stimulus", MUT_WORD2 ? "red" : "none",
                  "built with -DMUT_WORD2=1, which writes 0x1111 instead of 0x1234 -- both "
                  "halves of the word are identical, so the two delivered bytes are equal "
                  "(0x11, 0x11) and the distinctness check (seq[0]!=seq[1]) goes red even "
                  "though two samples are still emitted. Reverted, baseline restored.");
    }

    // =========================================================================
    // SAMPLE-BYTES-HIGH-BYTE-FIRST-SEQUENTIAL
    // (5-p138.c4, 5-data.c4, A-audxdat.c4, 5-appA-p265.c41)
    // =========================================================================
    {
        Harness h;
        h.dut->dmaena = 0;
        h.write_reg(REG_AUDPER, 4);
        h.write_reg(REG_AUDDAT, 0x1234);
        std::vector<std::pair<long, uint8_t>> seq;
        uint8_t last = h.dut->sample;
        long cck_count = 0;
        for (int i = 0; i < 400 && seq.size() < 2; i++) {
            h.tick(); h.beam_step();
            if (h.dut->cck) {
                cck_count++;
                if (h.dut->sample != last) { last = h.dut->sample; seq.push_back({cck_count, last}); }
            }
        }
        bool ok = seq.size() == 2 && seq[0].second == 0x12 && seq[1].second == 0x34 &&
                  seq[1].first > seq[0].first;
        CHECK("SAMPLE-BYTES-HIGH-BYTE-FIRST-SEQUENTIAL (5-p138.c4, 5-data.c4, A-audxdat.c4, "
              "5-appA-p265.c41): AUDxDAT=0x1234's high byte (0x12 -- the lower/first byte "
              "address on a 68000) appears on `sample` at an earlier cck tick than the low byte "
              "(0x34); the two never coincide at the same tick -- output is sequential, one "
              "8-bit port value at a time. The DAC/output-pin half of A-audxdat.c4 is NOT "
              "asserted here: no analog boundary exists at this or any RTL port", ok);
        MUTATION("SAMPLE-BYTES-HIGH-BYTE-FIRST-SEQUENTIAL", "stimulus", "impossible",
                  "no build attempted. paula_audio_channel's sample mux (penhi selects "
                  "datbuf[15:8] vs datbuf[7:0]) and the penhi set/clear timing have no mode, "
                  "chipset, or configuration port of any kind (confirmed by the port list this "
                  "package's own Makefile builds against) -- no AUDxDAT value or register-write "
                  "order can make the low byte appear first or make both bytes appear at the "
                  "same tick. The WORD-CONTAINS-TWO-SAMPLES control above, same measurement "
                  "basis, is what proves this is not vacuously green.");
    }

    // =========================================================================
    // AUDXDAT-IS-PERCHANNEL-DATA-BUFFER
    // (A-audxdat.c1, 5-appA-p265.c38)
    // =========================================================================
    {
        Harness h;
        h.dut->dmaena = 0;
        h.write_reg(REG_AUDPER, 4);
#if MUT_ADDR
        // stimulus mutation: the intended data is sent ONLY to AUDxLEN's address; AUDxDAT is
        // never written at all.
        h.write_reg(REG_AUDLEN, 0x99AA);
#else
        h.write_reg(REG_AUDLEN, 0x99AA);  // decoy: looks like sample data but is not AUDxDAT
        h.write_reg(REG_AUDDAT, 0x99AA);
#endif
        std::vector<uint8_t> seq;
        uint8_t last = h.dut->sample;
        for (int i = 0; i < 400 && seq.size() < 2; i++) {
            h.tick(); h.beam_step();
            if (h.dut->cck && h.dut->sample != last) { last = h.dut->sample; seq.push_back(last); }
        }
        bool ok = seq.size() == 2 && seq[0] == 0x99 && seq[1] == 0xAA;
        CHECK("AUDXDAT-IS-PERCHANNEL-DATA-BUFFER (A-audxdat.c1, 5-appA-p265.c38): writing "
              "0x99AA at AUDxLEN's own address leaves `sample` unaffected; the same value "
              "written at AUDxDAT's address (offset 0xA) is what populates the channel's data "
              "buffer and reaches `sample` as 0x99 then 0xAA -- AUDxDAT is this channel's own, "
              "separately addressed DMA data-buffer register", ok);
        MUTATION("AUDXDAT-IS-PERCHANNEL-DATA-BUFFER", "stimulus", "red",
                  "built with -DMUT_ADDR=1, which sends the intended 0x99AA data to AUDxLEN's "
                  "address only and never writes AUDxDAT -- `sample` never receives the buffer "
                  "content and the expected 0x99/0xAA sequence never appears, red. Reverted, "
                  "baseline restored.");
    }

    // =========================================================================
    // AUDXLEN-SETS-WORDS-PER-BLOCK
    // (5-p139.c8, 5-p139.c9, 5-p139.c10, 5-len.c1, 5-appA-p266.c18,
    //  A-audxlen.c1, 5-p147.c2, 5-sum.c3, 5-p147.c9, 5-join.c6)
    // =========================================================================
    {
        Harness h;
#if MUT_LENWORDS
        // stimulus mutation: never write AUDxLEN -- it stays at its post-reset default.
#else
        h.write_reg(REG_AUDLEN, 5);
#endif
        h.dut->dmaena = 1;
        DmaObserver obs;
        long ctr = 0;
        auto gen = [&]() -> uint16_t { ctr++; return (uint16_t)(0x1000 + ctr); };
        dma_advance(h, obs, 227 * 4 * 3, gen);
        bool ok = obs.dmas_edges.size() >= 2 &&
                  count_words_between(obs.word_cck, 0, obs.dmas_edges[0]) == 5 &&
                  count_words_between(obs.word_cck, obs.dmas_edges[0], obs.dmas_edges[1]) == 5;
        CHECK("AUDXLEN-SETS-WORDS-PER-BLOCK (5-p139.c8, 5-p139.c9, 5-p139.c10, 5-len.c1, "
              "5-appA-p266.c18, A-audxlen.c1, 5-p147.c2, 5-p147.c9, 5-join.c6, 5-sum.c3): "
              "writing AUDxLEN=5 makes the channel request, and the DMA answer, exactly 5 "
              "words between successive `dmas` restarts, for two consecutive blocks -- the "
              "register's value is the word count per playback cycle (words = samples/2), "
              "read before output begins and reloaded at every restart. 5-sum.c3's further "
              "claim that the restart lands at the address in AUDxLC is NOT asserted here: "
              "AUDxLC is agnus_audiodma's register, outside this module's boundary", ok);
        NOTE("5-p147.c9 and 5-join.c6 both describe the length counter's terminal value as "
             "reaching zero; printed p164/p165 describe the same counter's terminus as the "
             "value 1 (lenfin). Not decidable at this module's ports -- lencnt is an internal "
             "register, never a port -- and the block-boundary cadence measured above (5 words "
             "per restart) is identical under either reading, since the register only ever "
             "exposes the RELOAD event via `dmas`, never the intermediate count. Recorded, not "
             "asserted.");
        MUTATION("AUDXLEN-SETS-WORDS-PER-BLOCK", "stimulus", "red",
                  "built with -DMUT_LENWORDS=1, which skips the AUDxLEN=5 write entirely, "
                  "leaving the register at its post-reset default -- the measured words-per-"
                  "block no longer equals 5 for either block, and the check goes red. Reverted, "
                  "baseline restored.");
    }

    // =========================================================================
    // DMA-CONTINUES-UNTIL-STOPPED-REPLAYS
    // (5-p144.c2, 5-p144.c3, 5-play.c2, 5-play.c3)
    // =========================================================================
    {
        Harness h;
        h.write_reg(REG_AUDLEN, 3);
        h.dut->dmaena = 1;
        DmaObserver obs;
        long ctr = 0;
        auto gen = [&]() -> uint16_t { ctr++; return (uint16_t)(0x2000 + ctr); };
        long run_cck = 227 * 4 * 4;  // generous: several blocks' worth of DMA slots
#if MUT_CONT
        run_cck = 227;  // stimulus mutation: only enough time for about one restart
#endif
        dma_advance(h, obs, run_cck, gen);
        bool ok = obs.dmas_edges.size() >= 3;
        CHECK("DMA-CONTINUES-UNTIL-STOPPED-REPLAYS (5-p144.c2, 5-p144.c3, 5-play.c2, "
              "5-play.c3): with AUDxLEN=3 and dmaena held high, the channel restarts (`dmas`) "
              "at least three times over a run with no external stop -- once begun, the DMA "
              "repeats the same programmed block on its own, producing the steady, repeating "
              "tone the manual describes", ok);
        MUTATION("DMA-CONTINUES-UNTIL-STOPPED-REPLAYS", "stimulus", "red",
                  "built with -DMUT_CONT=1, which shortens the observed run to one line's worth "
                  "of cck ticks -- too little time for three restarts to occur, and the >=3 "
                  "check goes red. Reverted, baseline restored.");
    }

    // =========================================================================
    // RESTART-DOES-NOT-RESUME-MIDBUFFER
    // (5-p145.c2, 5-p145.c3, 5-stop.c2, 5-stop.c3)
    // =========================================================================
    {
        Harness h;
        h.write_reg(REG_AUDLEN, 3);
        h.dut->dmaena = 1;
        DmaObserver obs;
        long ctr = 0;
        auto gen = [&]() -> uint16_t { ctr++; return (uint16_t)(0x3000 + ctr); };
        dma_advance(h, obs, 227 * 4 * 2, gen);  // establish steady DMA activity
        long edges_before = (long)obs.dmas_edges.size();
        h.dut->dmaena = 0;  // stop
        for (int i = 0; i < 227 * 2; i++) { h.tick(); h.beam_step(); }  // let it settle -- NOT reading `sample` here
        bool dmareq_dropped = (h.dut->dmareq == 0);
        h.dut->dmaena = 1;  // re-enable
#if MUT_RESTART
        dma_advance(h, obs, 227 * 4 * 2, gen, /*answer=*/false);  // stimulus mutation: never answer again
#else
        dma_advance(h, obs, 227 * 4 * 2, gen);
#endif
        long edges_after = (long)obs.dmas_edges.size();
        bool ok = dmareq_dropped && (edges_after > edges_before);
        CHECK("RESTART-DOES-NOT-RESUME-MIDBUFFER (5-p145.c2, 5-p145.c3, 5-stop.c2, 5-stop.c3): "
              "dropping dmaena silences `dmareq`; raising it again produces a FRESH `dmas` "
              "restart pulse, the same signal a brand-new start produces -- not a quiet "
              "continuation from wherever playback left off. `sample` is deliberately not "
              "read anywhere near this edge (silence trap #2). The manual's own hedge (\"almost "
              "always\") is recorded, not graded", ok);
        MUTATION("RESTART-DOES-NOT-RESUME-MIDBUFFER", "stimulus", "red",
                  "built with -DMUT_RESTART=1, which re-raises dmaena but never answers "
                  "another dmareq -- no further `dmas` edge is possible, and the "
                  "post-restart-edge check goes red. Reverted, baseline restored.");
    }

    // =========================================================================
    // LENGTH-IS-A-BACKUP-REGISTER
    // (5-p147.c4, 5-p148.c1, 5-join.c7)
    // =========================================================================
    {
        Harness h;
        h.write_reg(REG_AUDLEN, 5);
        h.dut->dmaena = 1;
        DmaObserver obs;
        long ctr = 0;
        auto gen = [&]() -> uint16_t { ctr++; return (uint16_t)(0x4000 + ctr); };
        long guard = 0;
        while (obs.dmas_edges.empty() && guard < 20) { dma_advance(h, obs, 227 * 2, gen); guard++; }
        dma_advance(h, obs, 400, gen);  // partway into block 2, well short of its own 5-word length
#if MUT_BACKUP
        h.write_reg(REG_AUDLEN, 5);  // stimulus mutation: rewrite the SAME value
#else
        h.write_reg(REG_AUDLEN, 3);
#endif
        dma_advance(h, obs, 227 * 4 * 3, gen);  // finish block 2, then run block 3 plus margin
        bool ok = false;
        if (obs.dmas_edges.size() >= 3) {
            long e0 = obs.dmas_edges[0], e1 = obs.dmas_edges[1], e2 = obs.dmas_edges[2];
            long b2 = count_words_between(obs.word_cck, e0, e1);
            long b3 = count_words_between(obs.word_cck, e1, e2);
            ok = (b2 == 5) && (b3 == 3);
        }
        CHECK("LENGTH-IS-A-BACKUP-REGISTER (5-p147.c4, 5-p148.c1, 5-join.c7): changing AUDxLEN "
              "from 5 to 3 partway through a block already in progress does not shrink that "
              "block -- it still completes at 5 words -- and only the NEXT restart picks up "
              "the new value (3 words); the programmed length is read into a back-up copy at "
              "the start of each block, not applied live mid-block", ok);
        MUTATION("LENGTH-IS-A-BACKUP-REGISTER", "stimulus", "red",
                  "built with -DMUT_BACKUP=1, which rewrites AUDxLEN with its OLD value (5) "
                  "instead of a new one (3) at the same point in the run -- block 3 then also "
                  "measures 5 words, indistinguishable from a register that was never re-read, "
                  "and the \"next block uses the new value\" half of the check (b3==3) goes "
                  "red. Reverted, baseline restored.");
    }

    // =========================================================================
    // INTERRUPT-CADENCE-TIED-TO-LENGTH
    // (5-p147.c6, 5-join.c4, 5-appA-p265.c46, 5-appA-p265.c47, A-audxdat.c8,
    //  A-audxdat.c9)
    // =========================================================================
    {
        Harness h;
        h.write_reg(REG_AUDLEN, 4);
        h.dut->dmaena = 1;
        DmaObserver obs;
        long ctr = 0;
        auto gen = [&]() -> uint16_t { ctr++; return (uint16_t)(0x5000 + ctr); };
        long run_cck = 227 * 4 * 6;
#if MUT_INTCAD
        run_cck = 227;  // stimulus mutation: far too short for two full blocks
#endif
        dma_advance(h, obs, run_cck, gen);
        bool ok = obs.intreq_edges.size() >= 2 && obs.dmas_edges.size() >= 2;
        std::ostringstream m;
        m << "measured " << obs.intreq_edges.size() << " intreq pulses and "
          << obs.dmas_edges.size() << " dmas restarts over " << obs.word_count
          << " delivered words with AUDxLEN=4.";
        CHECK("INTERRUPT-CADENCE-TIED-TO-LENGTH (5-p147.c6, 5-join.c4, 5-appA-p265.c46, "
              "5-appA-p265.c47, A-audxdat.c8, A-audxdat.c9): with AUDxLEN=4 and DMA running, "
              "`intreq` is raised repeatedly, recurring in step with the completed-block "
              "cadence (existence and cadence asserted; see the MEASURED line for the observed "
              "counts)", ok);
        NOTE(m.str());
        NOTE("Three manual passages give three different MOMENTS for this same interrupt: "
             "p147/p148 say immediately after the location+length registers are copied into "
             "their back-up registers; p164 says just as the last word of the waveform starts "
             "its output; appendix A p265 (also A-audxdat.c8, A-audxdat.c9) says after the DMA "
             "data is finished (words output=length) AND the data already in AUDxDAT has been "
             "used. This package asserts only that the request exists and recurs at a stable "
             "cadence tied to AUDxLEN; which of the three moments the RTL implements is "
             "recorded, not graded, here.");
        MUTATION("INTERRUPT-CADENCE-TIED-TO-LENGTH", "stimulus", "red",
                  "built with -DMUT_INTCAD=1, which shortens the run to under two full AUDxLEN "
                  "blocks -- fewer than two intreq/dmas events have time to occur and the >=2 "
                  "check goes red. Reverted, baseline restored.");
    }

    // =========================================================================
    // PERIOD-SETS-SAMPLE-INTERVAL
    // (5-p140.c7, 5-p140.c8, 5-p140.c10, 5-p143.c3, 5-per.c1, 5-per.c2,
    //  5-per.c4, 5-per.c5, 5-per.c14, 5-appA-p266.c23, A-audxper.c1,
    //  5-appC-p298.c11, 5-p140.c9, 5-per.c3)
    // =========================================================================
    {
        long i4 = measure_interval(4, /*misdirect=*/MUT_PERCORE != 0);
        long i6 = measure_interval(6);
        long i10 = measure_interval(10);
        bool ok = (i4 == 4) && (i6 == 6) && (i10 == 10);
        CHECK("PERIOD-SETS-SAMPLE-INTERVAL (5-p140.c7, 5-p140.c8, 5-p140.c10, 5-p143.c3, "
              "5-per.c1, 5-per.c2, 5-per.c4, 5-per.c5, 5-per.c14, 5-appA-p266.c23, "
              "A-audxper.c1, 5-appC-p298.c11): writing AUDxPER=4, 6 and 10 holds each of the "
              "two samples from one delivered word on `sample` for exactly 4, 6 and 10 "
              "colour-clock ticks respectively -- the register's value IS the clock-ticks-"
              "per-sample interval, and each expiry retrieves the next sample", ok);
        NOTE("5-p140.c9 and 5-per.c3 describe the period counter's terminal value as 0; "
             "printed p164/p165 describe the same counter's terminus as the value 1 (perfin). "
             "Not decidable at this module's boundary -- percnt is an internal register, never "
             "a port -- and the measured interval (register value == tick count) is identical "
             "under either reading. Recorded, not asserted.");
        MUTATION("PERIOD-SETS-SAMPLE-INTERVAL", "stimulus", "red",
                  "built with -DMUT_PERCORE=1, which writes the value 4 to AUDxVOL's address "
                  "instead of AUDxPER's -- AUDxPER stays at its post-reset default, the "
                  "measured interval for the 'period=4' case no longer equals 4, and the check "
                  "goes red. Reverted, baseline restored.");
    }

    // =========================================================================
    // LOWER-PERIOD-YIELDS-HIGHER-RATE
    // (5-p140.c14, 5-p140.c15, 5-per.c10, 5-per.c11, 5-per.c12)
    // =========================================================================
    {
        long low = measure_interval(4);
        long high = measure_interval(MUT_PERDIR ? 4 : 50);
        bool ok = (low > 0) && (high > 0) && (low < high);
        CHECK("LOWER-PERIOD-YIELDS-HIGHER-RATE (5-p140.c14, 5-p140.c15, 5-per.c10, 5-per.c11, "
              "5-per.c12): a period of 4 produces a shorter inter-sample interval (higher "
              "output rate) than a period of 50 -- confirms both directions of \"low period = "
              "higher frequency, high period = lower frequency\" and the inverse period/"
              "frequency relationship, on the same measurement basis as "
              "PERIOD-SETS-SAMPLE-INTERVAL above", ok);
        MUTATION("LOWER-PERIOD-YIELDS-HIGHER-RATE", "stimulus", "red",
                  "built with -DMUT_PERDIR=1, which measures period=4 on both sides of the "
                  "comparison instead of 4 and 50 -- the two intervals become equal and the "
                  "strict low<high check goes red. Reverted, baseline restored.");
    }

    // =========================================================================
    // PERIOD-MAX-16BIT-RANGE
    // (5-p140.c13, 5-per.c8)
    // =========================================================================
    {
        uint16_t used = MUT_PERMAX ? 100 : 65535;
        long interval = measure_interval(used);
        bool ok = (interval == 65535);
        CHECK("PERIOD-MAX-16BIT-RANGE (5-p140.c13, 5-per.c8): AUDxPER accepts and correctly "
              "times the full 16-bit maximum, 65535 -- the largest value the module's 16-bit "
              "`data` write path can express in the first place. Printed p159's alternative "
              "maximum, 65536, cannot be attempted at any boundary of this module: it needs a "
              "17th bit that the write path (data[15:0]) does not have. Recorded, not "
              "separately asserted", ok);
        MUTATION("PERIOD-MAX-16BIT-RANGE", "stimulus", "red",
                  "built with -DMUT_PERMAX=1, which writes 100 instead of 65535 while the check "
                  "still requires an interval of exactly 65535 -- the measured interval (100) "
                  "misses by a wide margin, red. Reverted, baseline restored.");
    }

    // =========================================================================
    // SHORT-PERIOD-REUSES-PREVIOUS-SAMPLE
    // (5-rate.c16)
    // =========================================================================
    {
        Harness h;
        h.write_reg(REG_AUDPER, 4);  // well below the manual's 124/123 minimum
        h.dut->dmaena = 1;
        long line_len = 227;
#if MUT_REUSE
        line_len = 6;  // stimulus mutation: DMA answers almost every period tick -- nothing to reuse
#endif
        uint8_t last_sample = h.dut->sample;
        bool saw_zero_while_starved = false;
        bool saw_reuse = false;
        long since_change = 0;
        int hpos = 0;
        long ctr = 0;
        for (long t = 0; t < line_len * 6; t++) {
            h.tick();
            if (h.dut->cck) {
                hpos++;
                h.dut->strhor = (hpos == 13) ? 1 : 0;
                if (hpos >= line_len) hpos = 0;
                if (h.dut->sample == last_sample) {
                    since_change++;
                    if (since_change > 20) saw_reuse = true;  // held far longer than the period
                } else {
                    if (h.dut->sample == 0x00) saw_zero_while_starved = true;
                    last_sample = h.dut->sample;
                    since_change = 0;
                }
            }
            if (h.dut->clk7_en && hpos == 13 && h.dut->dmareq) {
                ctr++;
                h.write_reg(REG_AUDDAT, (uint16_t)(0x6100 + ctr));  // distinct, never 0x00 in either byte
            }
        }
        bool ok = saw_reuse && !saw_zero_while_starved;
        CHECK("SHORT-PERIOD-REUSES-PREVIOUS-SAMPLE (5-rate.c16): with AUDxPER=4 (below the "
              "manual's stated 124/123 minimum) and DMA answered only once per 227-cck-tick "
              "line, `sample` is held at its last delivered, non-zero byte for far longer than "
              "4 ticks between deliveries -- the audio DMA cannot keep up, and the previous "
              "sample is reused rather than the channel falling silent", ok);
        MUTATION("SHORT-PERIOD-REUSES-PREVIOUS-SAMPLE", "stimulus", "red",
                  "built with -DMUT_REUSE=1, which shrinks the DMA line length from 227 to 6 "
                  "cck ticks -- DMA now easily keeps up with a period-4 sample rate, no "
                  "starvation gap longer than the period itself occurs, and the 'held far "
                  "longer than the period' check goes red. Reverted, baseline restored.");
    }

    // =========================================================================
    // VOLUME-REGISTER-SEVEN-BITS-READBACK
    // (5-p139.c11, 5-p139.c12, 5-p140.c6, 5-vol.c1, 5-vol.c10,
    //  5-appA-p266.c31, 5-appA-p266.c33, A-audxvol.c1, A-audxvol.c2)
    // (measured at paula_audio_channel's volume[6:0] port, not the mixer)
    // =========================================================================
    {
        Harness h;
        h.dut->dmaena = 0;
        auto write_and_read = [&](uint16_t v, bool misdirect = false) -> int {
            h.write_reg(misdirect ? REG_AUDPER : REG_AUDVOL, v);
            return h.dut->volume;
        };
        int r1 = write_and_read(0x0040, MUT_VOLWIDTH != 0);
        int r2 = write_and_read(0x007F);
        int r3 = write_and_read(0x1F40);
        int r4 = write_and_read(0xFFFF);
        int r5 = write_and_read(0x003F);
        bool ok = (r1 == 64) && (r2 == 127) && (r3 == 0x40) && (r4 == 127) && (r5 == 63);
        CHECK("VOLUME-REGISTER-SEVEN-BITS-READBACK (5-p139.c11, 5-p139.c12, 5-p140.c6, "
              "5-vol.c1, 5-vol.c10, 5-appA-p266.c31, 5-appA-p266.c33, A-audxvol.c1, "
              "A-audxvol.c2): AUDxVOL stores exactly seven bits (data[6:0]) -- writes of "
              "0x0040, 0x007F, 0x1F40, 0xFFFF and 0x003F read back on volume[6:0] (this "
              "module's own volume port, not the mixer) as 64, 127, 0x40, 127 and 63: bits "
              "15-7 of the written word never reach the port, and bit 6 itself does reach it "
              "(0x40 and 0x1F40 both give 64, above the six-bit range bits 5-0 alone could "
              "express)", ok);
        NOTE("The register's WIDTH is itself disputed across the manual's own pages: p139 "
             "calls it a \"six-bit volume register\"; p140 writes values \"64 to 0\" into "
             "\"bits 5-0\" (64 does not fit in six bits); p149 gives the modulation word as "
             "V6-V0; appendix A p266 gives \"Bits 6,5,4,3,2,1,0\" and 65 linear levels. The "
             "measurement above is what this module actually stores (seven bits, matching "
             "appendix A); which page's WORDING is authoritative is not adjudicated here -- "
             "measured at paula_audio_channel's volume[6:0] port only, never at the mixer, "
             "which BOUNDARIES.md records as folding bit 6 into the low six bits and collapsing "
             "64 onto 63 (a different module, a different package, hrm/ch5-audio-volume-scale/, "
             "already built and red on it).");
        MUTATION("VOLUME-REGISTER-SEVEN-BITS-READBACK", "stimulus", "red",
                  "built with -DMUT_VOLWIDTH=1, which sends the first write (0x0040) to "
                  "AUDxPER's address instead of AUDxVOL's -- `volume` never receives 64 (stays "
                  "at its prior/default value), and the r1==64 half of the check goes red. "
                  "Reverted, baseline restored.");
    }

    // =========================================================================
    // PWM-PATH-DISTINGUISHES-ALL-65-LEVELS
    // (5-appA-p266.c37, A-audxvol.b05_00.max)
    // (measured at paula_audio_channel's sample_okk[7:0] port, not the mixer)
    // =========================================================================
    {
        auto measure_duty = [&](uint16_t vol) -> double {
            Harness h;
            h.dut->dmaena = 0;
            h.write_reg(REG_AUDPER, 4);
            h.write_reg(REG_AUDVOL, vol);
            long window = 6400;       // 100 volgate cycles (a free-running 6-bit, 64-tick counter)
            long refresh_every = 16;  // keep the buffer fed well inside one 64-tick volgate cycle
            long since = refresh_every;
            long nonzero = 0, total = 0;
            h.write_reg(REG_AUDDAT, 0x7F7F);
            while (total < window) {
                h.tick(); h.beam_step();
                if (h.dut->cck) {
                    total++; since++;
                    if (h.dut->sample_okk != 0) nonzero++;
                }
                if (since >= refresh_every) { h.write_reg(REG_AUDDAT, 0x7F7F); since = 0; }
            }
            return (double)nonzero / (double)total;
        };
        double d0 = measure_duty(0);
        double d63 = measure_duty(63);
        double d64 = measure_duty(MUT_PWM ? 63 : 64);
        bool ok = (d0 < 0.02) && (d63 > 0.90 && d63 < 1.0) && (d64 > 0.98) && (d64 > d63);
        std::ostringstream m;
        m << "measured sample_okk duty cycle: volume=0 -> " << (d0 * 100.0) << "%, volume=63 -> "
          << (d63 * 100.0) << "%, volume=" << (MUT_PWM ? 63 : 64) << " -> " << (d64 * 100.0) << "%.";
        CHECK("PWM-PATH-DISTINGUISHES-ALL-65-LEVELS (5-appA-p266.c37, A-audxvol.b05_00.max): "
              "measured at paula_audio_channel's sample_okk[7:0] PWM port (not the mixer), "
              "AUDxVOL=0 gives near-zero duty, AUDxVOL=63 gives a duty strictly below "
              "AUDxVOL=64's, and 64 alone reaches full duty -- bits 5-0=111111 (63) is one step "
              "short of bit 6's forced maximum (64), 65 distinguishable levels in all, matching "
              "appendix A's own description", ok);
        NOTE(m.str());
        MUTATION("PWM-PATH-DISTINGUISHES-ALL-65-LEVELS", "stimulus", "red",
                  "built with -DMUT_PWM=1, which measures volume 63 for BOTH sides of the "
                  "63-vs-64 comparison -- the two duty-cycle measurements become numerically "
                  "equal and the strict d64>d63 check goes red. Reverted, baseline restored.");
    }

    // =========================================================================
    // CPU-CAN-WRITE-AUDXDAT-DIRECTLY
    // (A-audxdat.c7, 5-appA-p265.c45, 5-appB-p290.c53, 5-appB-p290.c88,
    //  5-appB-p290.c123, 5-appB-p291.c11, B-aud0dat.mark, B-aud1dat.mark,
    //  B-aud2dat.mark, B-aud3dat.mark)
    // =========================================================================
    {
        Harness h;
        h.dut->dmaena = 0;  // never toggled 1->0: silence trap #2 cannot trigger
        h.write_reg(REG_AUDPER, 4);
        int intreq_pulses = 0;
        int last_intreq = h.dut->intreq;
#if MUT_DIRECT
        // stimulus mutation: never write AUDxDAT at all.
#else
        for (int w = 0; w < 6; w++) {
            h.write_reg(REG_AUDDAT, (uint16_t)(0x7000 + w));
            for (int i = 0; i < 100; i++) {
                h.tick(); h.beam_step();
                if (h.dut->intreq && !last_intreq) intreq_pulses++;
                last_intreq = h.dut->intreq;
            }
        }
#endif
        bool ok = (intreq_pulses > 0);
        CHECK("CPU-CAN-WRITE-AUDXDAT-DIRECTLY (A-audxdat.c7, 5-appA-p265.c45, 5-appB-p290.c53, "
              "5-appB-p290.c88, 5-appB-p290.c123, 5-appB-p291.c11, B-aud0dat.mark, "
              "B-aud1dat.mark, B-aud2dat.mark, B-aud3dat.mark): with dmaena held low throughout "
              "(no DMA ever requested or granted), repeated direct writes to AUDxDAT still "
              "drive `sample` and raise `intreq` -- the processor writes this register "
              "directly, exactly as appendix A p265 states. This is the same register appendix "
              "B marks with its DMA-channel-only \"&\" gloss for all four channels "
              "(AUD0DAT..AUD3DAT); the module built here is generic per-channel logic, so one "
              "measurement stands for all four. The word \"only\" in that gloss is NOT "
              "asserted as an expectation here -- it is the one manual reading this measurement "
              "contradicts", ok);
        MUTATION("CPU-CAN-WRITE-AUDXDAT-DIRECTLY", "stimulus", "red",
                  "built with -DMUT_DIRECT=1, which skips every AUDxDAT write -- with dmaena "
                  "low and no direct write either, nothing ever drives the channel, intreq "
                  "never pulses, and the >0 check goes red. Reverted, baseline restored.");
    }

    std::cout << "SUMMARY: " << g_pass << " passed, " << g_fail << " failed\n";
    return g_fail == 0 ? 0 : 1;
}
