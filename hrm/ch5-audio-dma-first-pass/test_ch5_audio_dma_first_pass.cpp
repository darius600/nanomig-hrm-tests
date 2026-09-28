// Test purpose: how many words of an audio data set channel 0 fetches on
//   its first DMA pass after DMA-enable, before the first restart at
//   AUDxLC, and whether any fetch ever lies beyond the data set. Package
//   ch5-audio-dma-first-pass, fixture `audio_dma_first_pass_rig` -- this
//   package's own wrapper around `paula_audio` (with `paula_audio_channel`,
//   `paula_audio_mixer`, `paula_audio_volume`) and `agnus_audiodma`, both
//   from the pinned NanoMig submodule.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 139, 145, 147, 164-166 (Figure 5-8 on p166), 265. A red result below
//   means the fixture disagrees with the manual, not that the assertion is
//   wrong -- no assertion here was, or should be, adjusted to make the
//   fixture pass.
// Non-goals: anything outside `address_out`/`dma` (the two graded
//   observables) and the exact propositions SPECIFICATION.md states. The
//   output side (`sample`, `ldata`/`rdata`) is not graded here. Channels 1-3
//   are held permanently disabled and are not exercised.
// Claim ids: SPECIFICATION.md carries the full source-claims table (chapter
//   5 claim ids, one row per distinct quoted sentence or figure bracket,
//   with every id that names that same sentence). Every id listed there is
//   cited below, inside the CHECK text of the assertion it supports.
//
// HARNESS AND SIGNAL SOURCES: this package's own wrapper ports (clk,
//   clk7_en, cck, strhor, hpos[8:0], rst, en, pend,
//   cpu_reg_address[8:1], cpu_data[15:0], sim_len[15:0], address_out[20:1],
//   dma, dmal0, dmas0) are exactly SPECIFICATION.md's own "Fixture" section.
//   No internal signal is read here -- every graded observable is a
//   top-level port, so this package needs no --public-flat-rw.
//   Register addresses (byte; cpu_reg_address receives addr>>1) are the
//   real Amiga custom-chip offsets SPECIFICATION.md's own stimulus section
//   names: AUD0LCH=$A0, AUD0LCL=$A2, AUD0LEN=$A4, AUD0PER=$A6, AUD0VOL=$A8.
//   Idle bus address is $FF (SPECIFICATION.md's own "Fixture" section).
//
// CLOCKING: clk7_en once every 4 raw ticks; cck alternates once every 2
//   clk7_en ticks (so once every 8 raw ticks -- this package's own
//   "cck tick" unit, matching the manual's "colour clock"); hpos[8:1]
//   increments, wrapping 226->0, on the tick where cck becomes 1, and
//   hpos[0]=cck, exactly SPECIFICATION.md's own clocking rule. strhor =
//   (hpos==9'd13), computed the same way. A register write holds its
//   address/data for exactly one clk7_en tick, then returns to the idle
//   address $FF -- long enough for exactly one write, never two.
//
// OBSERVATION: `address_out`/`dma` are sampled once per clk7_en tick
//   (`clk7_en && dma`, checked after the tick that produced them) -- `dma`
//   and `address_out` are both held constant across the 4 raw ticks between
//   clk7_en edges (hpos itself only changes at a clk7_en boundary), so
//   sampling on every raw tick would record every fetch four times over;
//   gating on clk7_en records each fetch exactly once. `offset` is
//   `address_out - $8000` (word address), per SPECIFICATION.md's own
//   Observation section. Fetches are captured only from the moment `en` is
//   raised in the graded run's own stimulus step 3, through the end of the
//   run.
//
// MUTATION METHOD: every assertion group has at least one paired mutation,
//   built by rerunning the FULL stimulus recipe fresh (a new DUT, a new
//   reset, from scratch) with exactly one stimulus parameter changed --
//   never the expected value, never the RTL. `FIRST-PASS-LEN-<n>` carries
//   two: a stimulus mutation (shared with the other two families at the
//   same LEN, an AUD0LEN<-(n+1) run graded against LEN=n's own
//   expectation) and a `none`-kind mutation (no new stimulus at all: pass 2
//   of the SAME, unmutated capture fed through the SAME comparator pass 1
//   uses, expected to still pass). Every stimulus mutation reuses the exact
//   same grading function its own assertion uses, applied to the mutated
//   capture with the ORIGINAL LEN's own expectation -- never a relaxed or
//   different comparison.
//
// RESULTS: run under WSL against $(NANOMIG)/src/minimig-aga/paula_audio.v
//   (paula_audio_channel.v, paula_audio_mixer.v, paula_audio_volume.v) and
//   agnus_audiodma.v. 22 CHECKs: 15 PASS, 7 FAIL. The fixture-liveness
//   check and all 7 LEN values' `NO-FETCH-BEYOND-DATA-SET-LEN-<n>` and
//   `LATER-PASSES-LEN-<n>` are green, except
//   `NO-FETCH-BEYOND-DATA-SET-LEN-3`, which is red (its own pass 1, graded
//   under `FIRST-PASS-LEN-3` below, reaches into that LEN's forbidden
//   region). `FIRST-PASS-LEN-<n>` is green only at LEN=2; at LEN in
//   {3,4,5,7,8,16} the measured pass 1 does not equal 0..LEN-1 (LEN=3
//   measured 5 fetches, offsets 0..4; LEN=4,5,7,8,16 each measured exactly
//   one fewer fetch than LEN, offsets 0..LEN-2) -- these are the fixture's
//   own measurements against the graded reading, not adjusted.
//
// MUTATION LOG: 28 mutation records, 4 per LEN value (2 for
//   `FIRST-PASS-LEN-<n>`, 1 for `NO-FETCH-BEYOND-DATA-SET-LEN-<n>`, 1 for
//   `LATER-PASSES-LEN-<n>`) across the 7 LEN values {2,3,4,5,7,8,16}. Every
//   `none`-kind record (`FIRST-PASS-LEN-<n>`'s own pass-2-through-the-same-
//   comparator check) is green, at all 7 LEN values. Every
//   `NO-FETCH-BEYOND-DATA-SET-LEN-<n>` and `LATER-PASSES-LEN-<n>`
//   `stimulus` record is red, at all 7 LEN values. `FIRST-PASS-LEN-<n>`'s
//   own `stimulus` record is red only at LEN=2 (the one LEN where the
//   unmutated assertion is itself green); at LEN in {3,4,5,7,8,16} it is
//   green -- the mutated run's own pass-1 measurement, graded against the
//   ORIGINAL LEN's expectation, matches anyway, because
//   `FIRST-PASS-LEN-<n>` is already red on the UNMUTATED capture at every
//   one of those LEN values, so this stimulus change is not the thing
//   separating a passing check from a failing one there. Every record's
//   own printed line states the exact measured value the token was decided
//   from.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "verilated.h"
#include "Vaudio_dma_first_pass_rig.h"

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
// Register addresses on this fixture (byte address; cpu_reg_address
// receives addr>>1).
// ---------------------------------------------------------------------------
static const int AUD0LCH = 0xA0;
static const int AUD0LCL = 0xA2;
static const int AUD0LEN = 0xA4;
static const int AUD0PER = 0xA6;
static const int AUD0VOL = 0xA8;

static const uint32_t D_WORD = 0x8000; // word address of D, this run's own graded data set

// ---------------------------------------------------------------------------
// Harness: DUT + the clk7_en/cck/hpos/strhor clocking pattern
// SPECIFICATION.md's own "Fixture" section gives.
// ---------------------------------------------------------------------------
struct Harness {
    Vaudio_dma_first_pass_rig* dut;
    int clk7_cycle = 0, cck_cycle = 0;
    int hpos_coarse = 0;   // hpos[8:1], 0..226
    long cck_ticks = 0;    // count of cck-high edges: this package's own unit ("colour clocks")

    Harness() {
        dut = new Vaudio_dma_first_pass_rig;
        dut->clk = 0; dut->clk7_en = 0; dut->cck = 0; dut->rst = 1;
        dut->en = 0; dut->pend = 0;
        dut->cpu_reg_address = 0xFF; dut->cpu_data = 0;
        dut->sim_len = 0;
        dut->hpos = 0; dut->strhor = 0;
        dut->eval();
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
            int newCck = (cck_cycle == 0) ? 1 : 0;
            if (newCck) {
                hpos_coarse = (hpos_coarse >= 226) ? 0 : (hpos_coarse + 1);
                cck_ticks++;
            }
            dut->cck = newCck;
        } else {
            dut->cck = 0;
        }
        dut->hpos = (uint16_t)((hpos_coarse << 1) | (dut->cck & 1));
        dut->strhor = (dut->hpos == 13) ? 1 : 0;
        // Re-settle combinational logic against the NEW clk7_en/cck/hpos/
        // strhor we just set -- same reasoning as this suite's other
        // packages' own Harness::tick(): every purely-combinational output
        // (dma, address_out, dmal0, dmas0 among them) would otherwise still
        // reflect the values computed during the eval() calls above, which
        // used the OLD, pre-this-tick clk7_en/cck/hpos.
        dut->eval();
    }

    void write_reg(int addr, uint16_t data) {
        while (!dut->clk7_en) tick();
        dut->cpu_reg_address = (uint16_t)(addr >> 1);
        dut->cpu_data = data;
        tick(); tick(); tick(); tick(); // exactly one clk7_en tick's worth: one write
        dut->cpu_reg_address = 0xFF;
        dut->cpu_data = 0;
    }

    // Runs `n` cck ticks (colour clocks) forward from the current point.
    void runCck(long n) {
        long target = cck_ticks + n;
        while (cck_ticks < target) tick();
    }
};

// ---------------------------------------------------------------------------
// Scenario: the full stimulus recipe (SPECIFICATION.md's own "Stimulus"
// section), for a caller-supplied `actualLen` -- the value actually written
// to AUD0LEN (and to this fixture's own `sim_len`, so the RAM's data-set/
// sentinel boundary tracks it). A mutation calls this with an `actualLen`
// different from the LEN whose expectation the capture is later graded
// against; an unmutated run calls it with actualLen==the graded LEN.
// ---------------------------------------------------------------------------
struct DmaRun {
    std::vector<uint32_t> fetchAddr; // address_out at each observed dma=1 tick, in order
};

static DmaRun runScenario(uint16_t actualLen) {
    Harness h;

    // Step 1: rst high for 20 cycles, then 20 idle cycles.
    h.dut->rst = 1;
    for (int i = 0; i < 20; i++) h.tick();
    h.dut->rst = 0;
    for (int i = 0; i < 20; i++) h.tick();

    // Step 2: priming, so the Agnus pointer sits somewhere other than D.
    h.write_reg(AUD0LCH, 0x0000);
    h.write_reg(AUD0LCL, 0x8000);
    h.write_reg(AUD0LEN, 2);
    h.dut->sim_len = 2;
    h.write_reg(AUD0VOL, 64);
    h.write_reg(AUD0PER, 200);
    h.dut->en = 1;
    h.runCck(8L * 227);
    h.dut->en = 0;
    h.dut->pend = 1;
    h.runCck(4L * 200 + 1000);

    // Step 3: graded run, p.145's own order.
    h.write_reg(AUD0LCH, 0x0001);
    h.write_reg(AUD0LCL, 0x0000);
    h.write_reg(AUD0LEN, actualLen);
    h.dut->sim_len = actualLen;
    h.write_reg(AUD0VOL, 64);
    h.write_reg(AUD0PER, 200);
    // pend stays 1 (already set in step 2).

    DmaRun r;
    h.dut->en = 1;
    long budget = (4L * actualLen + 6) * 400 + 4L * 227;
    long target = h.cck_ticks + budget;
    while (h.cck_ticks < target) {
        h.tick();
        if (h.dut->clk7_en && h.dut->dma)
            r.fetchAddr.push_back((uint32_t)h.dut->address_out);
    }
    return r;
}

// ---------------------------------------------------------------------------
// Grading helpers -- pure functions over an already-captured fetch trace.
// ---------------------------------------------------------------------------
static long offsetOf(uint32_t addr) { return (long)addr - (long)D_WORD; }

static std::vector<int> offsetZeroIndices(const std::vector<uint32_t>& f) {
    std::vector<int> idx;
    for (size_t i = 0; i < f.size(); i++)
        if (offsetOf(f[i]) == 0) idx.push_back((int)i);
    return idx;
}

// Offsets for the fetches from index `from` (inclusive) to `to` (exclusive).
static std::vector<long> passOffsets(const std::vector<uint32_t>& f, int from, int to) {
    std::vector<long> out;
    for (int i = from; i < to && i >= 0 && i < (int)f.size(); i++)
        out.push_back(offsetOf(f[i]));
    return out;
}

static bool equalsRange(const std::vector<long>& offs, int len) {
    if ((int)offs.size() != len) return false;
    for (int i = 0; i < len; i++) if (offs[i] != i) return false;
    return true;
}

// FIRST-PASS-LEN-<n>'s own precondition + pass-1 check, against a capture
// graded at length `gradeLen` (which may differ from whatever `actualLen`
// produced the capture, for a stimulus mutation).
struct FirstPassResult {
    bool preconditionOk = false;
    bool pass1Ok = false;
    int j = -1;             // 1-based index of the first offset-zero fetch, or -1 if none
    std::vector<long> pass1Offsets;
};

static FirstPassResult gradeFirstPass(const std::vector<uint32_t>& f, int gradeLen) {
    FirstPassResult res;
    std::vector<int> zeros = offsetZeroIndices(f);
    if (zeros.empty()) return res;
    res.j = zeros[0] + 1;
    if (res.j == 1) {
        res.preconditionOk = true;
    } else if (res.j == 2) {
        long f1off = offsetOf(f[0]);
        bool inside = (f1off >= 0 && f1off < (long)gradeLen + 16);
        res.preconditionOk = !inside;
    }
    if (res.preconditionOk) {
        int startIdx = zeros[0];
        int endIdx = (zeros.size() >= 2) ? zeros[1] : (int)f.size();
        res.pass1Offsets = passOffsets(f, startIdx, endIdx);
        res.pass1Ok = equalsRange(res.pass1Offsets, gradeLen);
    }
    return res;
}

static bool gradeNoFetchBeyond(const std::vector<uint32_t>& f, int gradeLen) {
    for (uint32_t addr : f) {
        long off = offsetOf(addr);
        if (off >= gradeLen && off <= gradeLen + 15) return false;
    }
    return true;
}

struct LaterPassesResult {
    bool ok = false;
    bool haveBoundaries = false;
    std::vector<long> pass2Offsets, pass3Offsets;
};

static LaterPassesResult gradeLaterPasses(const std::vector<uint32_t>& f, int gradeLen) {
    LaterPassesResult res;
    std::vector<int> zeros = offsetZeroIndices(f);
    if (zeros.size() < 3) return res;
    res.haveBoundaries = true;
    res.pass2Offsets = passOffsets(f, zeros[1], zeros[2]);
    int pass3End = (zeros.size() >= 4) ? zeros[3] : (int)f.size();
    res.pass3Offsets = passOffsets(f, zeros[2], pass3End);
    res.ok = equalsRange(res.pass2Offsets, gradeLen) && equalsRange(res.pass3Offsets, gradeLen);
    return res;
}

static std::string offsetsToStr(const std::vector<long>& v, size_t cap = 20) {
    std::ostringstream d;
    d << "[";
    for (size_t i = 0; i < v.size() && i < cap; i++) d << v[i] << (i + 1 < v.size() && i + 1 < cap ? "," : "");
    if (v.size() > cap) d << ",...";
    d << "]";
    return d.str();
}

// =============================================================================
int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    // -------------------------------------------------------------------
    // FIXTURE LIVENESS -- confirm the whole DMA handshake genuinely reaches
    // the RAM port before any claim-based assertion leans on it: several
    // fetches occur, at more than one distinct address (never stuck at one
    // value, which would mean the grant/address path is not really moving).
    // This check does NOT constrain WHICH address the discard fetch or any
    // other individual fetch lands on -- SPECIFICATION.md's own "Reading"
    // section leaves that open and grades neither.
    // -------------------------------------------------------------------
    {
        DmaRun live = runScenario(2);
        std::vector<uint32_t>& fetches = live.fetchAddr;
        bool sawFetches = fetches.size() >= 4;
        bool varied = false;
        for (size_t i = 1; i < fetches.size(); i++)
            if (fetches[i] != fetches[0]) { varied = true; break; }
        std::ostringstream d;
        d << "[fixture] FIXTURE-LIVENESS: a full LEN=2 stimulus run -- " << fetches.size()
          << " fetches observed at agnus_audiodma.address_out while agnus_audiodma.dma=1, not "
             "all identical (first few: ";
        for (size_t i = 0; i < fetches.size() && i < 6; i++)
            d << "0x" << std::hex << fetches[i] << std::dec << (i + 1 < fetches.size() && i < 5 ? "," : "");
        d << ") -- the DMA-request/grant/fetch path genuinely reaches the RAM port before any "
             "claim-based assertion below leans on it";
        CHECK(d.str(), sawFetches && varied);
    }

    struct LenCase { int len; };
    std::vector<LenCase> cases = {{2}, {3}, {4}, {5}, {7}, {8}, {16}};

    static const char* FIRST_PASS_IDS =
        "5-sum.c3 5-p139.c8 5-p139.c9 5-len.c1 5-p164.c15 5-p164.c16 5-sm.c17 5-sm.c18 "
        "5-p164.c17 5-sm.c19 5-p164.c18 5-p164.c19 5-sm.c20 5-sm.c21 5-p164.c20 5-p164.c21 "
        "5-sm.c22 5-sm.c23 5-p164.c22 5-sm.c24 F58.t101_010";
    static const char* NO_FETCH_BEYOND_IDS =
        "5-p139.c7 5-loc.c7 5-p164.c23 5-p164.c24 5-sm.c25 5-p164.c25 5-sm.c26 5-p164.c26 "
        "5-p164.c27 5-sm.c27 5-sm.c28 F58.loop.lenfin 5-p147.c8 5-p147.c9 5-join.c6 "
        "5-appA-p265.c46 5-appA-p265.c47 A-audxdat.c8 A-audxdat.c9";
    static const char* LATER_PASSES_IDS = NO_FETCH_BEYOND_IDS; // same basis, per SPECIFICATION.md

    for (auto& c : cases) {
        int n = c.len;
        std::ostringstream lbl; lbl << n;
        std::string ls = lbl.str();

        // -----------------------------------------------------------------
        // The unmutated capture, shared by all three assertions at this LEN
        // and by the FIRST-PASS `none`-kind mutation.
        // -----------------------------------------------------------------
        DmaRun run = runScenario((uint16_t)n);

        // FIRST-PASS-LEN-<n>
        FirstPassResult fp = gradeFirstPass(run.fetchAddr, n);
        {
            std::ostringstream d;
            d << "FIRST-PASS-LEN-" << ls << " [" << FIRST_PASS_IDS << "] (pp.139,145,164,166) "
                 "pass 1 is exactly the offsets 0..LEN-1: LEN=" << n << ", j=" << fp.j
              << ", precondition " << (fp.preconditionOk ? "held" : "FAILED")
              << ", pass 1 measured " << offsetsToStr(fp.pass1Offsets) << ", expected [0.."
              << (n - 1) << "]";
            CHECK(d.str(), fp.preconditionOk && fp.pass1Ok);
        }

        // NO-FETCH-BEYOND-DATA-SET-LEN-<n>
        bool nfb = gradeNoFetchBeyond(run.fetchAddr, n);
        {
            std::ostringstream d;
            d << "NO-FETCH-BEYOND-DATA-SET-LEN-" << ls << " [" << NO_FETCH_BEYOND_IDS
              << "] (pp.139,147,164,166,265) no fetch anywhere in the run has offset in "
              << n << ".." << (n + 15) << ": " << run.fetchAddr.size()
              << " fetches observed, none in the forbidden region: " << (nfb ? "true" : "FALSE");
            CHECK(d.str(), nfb);
        }

        // LATER-PASSES-LEN-<n> (green control)
        LaterPassesResult lp = gradeLaterPasses(run.fetchAddr, n);
        {
            std::ostringstream d;
            d << "LATER-PASSES-LEN-" << ls << " [" << LATER_PASSES_IDS << "] (pp.147,164,166,265) "
                 "passes 2 and 3 each equal exactly 0..LEN-1: pass2=" << offsetsToStr(lp.pass2Offsets)
              << ", pass3=" << offsetsToStr(lp.pass3Offsets) << ", expected [0.." << (n - 1)
              << "] for both (boundaries " << (lp.haveBoundaries ? "found" : "NOT FOUND") << ")";
            CHECK(d.str(), lp.haveBoundaries && lp.ok);
        }

        // -----------------------------------------------------------------
        // MUTATION 1 (FIRST-PASS-LEN-<n>, none/green): no new stimulus --
        // pass 2 of the SAME capture above, through the SAME comparator
        // FIRST-PASS-LEN-<n> itself uses.
        // -----------------------------------------------------------------
        {
            bool pass2MatchesFirstPassShape = equalsRange(lp.pass2Offsets, n);
            MUTATION(std::string("FIRST-PASS-LEN-") + ls, "none",
                      pass2MatchesFirstPassShape ? "green" : "red",
                      "no stimulus change: pass 2 of the same capture (" +
                      offsetsToStr(lp.pass2Offsets) + ") fed through the identical "
                      "0..LEN-1 comparator FIRST-PASS-LEN-" + ls + " itself uses -- expected to "
                      "still pass, since LATER-PASSES-LEN-" + ls + " grades this same segment.");
        }

        // -----------------------------------------------------------------
        // MUTATION 2/3/4: one shared mutated capture (AUD0LEN<-(n+1) in the
        // graded run), graded against LEN=n's own expectation for all
        // three assertion families at this LEN.
        // -----------------------------------------------------------------
        DmaRun mut = runScenario((uint16_t)(n + 1));

        {
            FirstPassResult mfp = gradeFirstPass(mut.fetchAddr, n);
            bool mutRed = !(mfp.preconditionOk && mfp.pass1Ok);
            std::ostringstream det;
            det << "AUD0LEN stimulus changed from " << n << " to " << (n + 1)
                << " in the graded run; checked against the ORIGINAL LEN=" << n
                << " expectation (pass 1 == [0.." << (n - 1) << "]): j=" << mfp.j
                << ", precondition " << (mfp.preconditionOk ? "held" : "failed")
                << ", pass 1 measured " << offsetsToStr(mfp.pass1Offsets) << " -- "
                << (mutRed
                        ? "mismatched the ORIGINAL expectation, reverted"
                        : "matched the ORIGINAL expectation anyway, reverted -- at this LEN the "
                          "unmutated FIRST-PASS-LEN result above is itself already red, so this "
                          "record does not add a second, independent red result the way the "
                          "LEN=2 case does")
                << " to LEN=" << n << " above.";
            MUTATION(std::string("FIRST-PASS-LEN-") + ls, "stimulus", mutRed ? "red" : "green", det.str());
        }
        {
            bool mnfb = gradeNoFetchBeyond(mut.fetchAddr, n);
            bool mutRed = !mnfb;
            std::ostringstream det;
            det << "AUD0LEN stimulus changed from " << n << " to " << (n + 1)
                << " in the graded run; checked against the ORIGINAL LEN=" << n
                << " forbidden region (" << n << ".." << (n + 15) << "): "
                << (mnfb ? "no fetch found in the region (unexpected)" : "a fetch landed inside it")
                << " -- " << (mutRed ? "mismatched as expected, reverted" : "did NOT mismatch")
                << " to LEN=" << n << " above.";
            MUTATION(std::string("NO-FETCH-BEYOND-DATA-SET-LEN-") + ls, "stimulus",
                      mutRed ? "red" : "green", det.str());
        }
        {
            LaterPassesResult mlp = gradeLaterPasses(mut.fetchAddr, n);
            bool mutRed = !(mlp.haveBoundaries && mlp.ok);
            std::ostringstream det;
            det << "AUD0LEN stimulus changed from " << n << " to " << (n + 1)
                << " in the graded run (same mutated capture as the two records above); checked "
                   "against the ORIGINAL LEN=" << n << " expectation (passes 2,3 == [0.."
                << (n - 1) << "]): pass2=" << offsetsToStr(mlp.pass2Offsets)
                << ", pass3=" << offsetsToStr(mlp.pass3Offsets) << " -- "
                << (mutRed ? "mismatched the ORIGINAL expectation, reverted"
                           : "matched the ORIGINAL expectation anyway, reverted")
                << " to LEN=" << n << " above.";
            MUTATION(std::string("LATER-PASSES-LEN-") + ls, "stimulus", mutRed ? "red" : "green", det.str());
        }
    }

    // -------------------------------------------------------------------
    std::cout << "\n==== ch5-audio-dma-first-pass: " << g_pass << " PASS, " << g_fail
              << " FAIL ====\n";
    // The same two numbers in the spelling hrm/run_all.sh and the coverage
    // ledger grep for.
    std::cout << g_pass << " passed, " << g_fail << " failed.\n";
    return (g_fail == 0) ? 0 : 1;
}
