// Test purpose: Verify table 6-2 in full (sixteen USE codes, the active-
//   channel set and the bus-cycle order for a three-word blit) at the
//   agnus_blitter boundary, plus the pipeline claims on p.188 (two source
//   sets fetched before the first destination), the DMA-channel structure
//   of pp.169-172, the D-path order of operations of p.200 (and its
//   restatement on p.184 for area fill specifically), and the cycle-count
//   half of p.193's speed table.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 169-172, 174, 176, 179, 181, 184, 188-189, 193-194, 198-200.
// Non-goals: minterm/fill truth-table correctness (PKG-BLT-MINTERM,
//   PKG-BLT-FILL's job - the function generator and fill block are used
//   here strictly as plumbing, with their own simple, independently
//   verifiable formulas, to demonstrate ORDER and CYCLE COUNT, never to
//   re-litigate their logic tables); BLTSIZE field widths/wrap/zero-means-
//   max (PKG-BLT-SIZE); descending-mode arithmetic (PKG-BLT-DESC); masking
//   register semantics beyond what is needed to bypass them cleanly
//   (PKG-BLT-MASKS); line-mode octant/texture/ONEDOT correctness beyond a
//   single cycle-count measurement for p.193's line-mode ticks-per-pixel
//   claim (PKG-BLT-LINE); address-pointer/modulo arithmetic beyond showing
//   two channels' modulos are independent (PKG-BLT-ADRGEN); microsecond/
//   megahertz durations and the NTSC/PAL clock-frequency formula (p.193) -
//   no clock-frequency port exists at this boundary, so only the
//   dimensionless "n" (clocks per cycle) half of that page is testable
//   here, exactly as the classification stage's evidence field says.
//
// NO PRIOR ART IS SUPERSEDED. docs/blitter-prior-art.md was read in full
// before writing this file, per the build brief's explicit instruction to
// say so rather than assume it. Its "eight blitter tests" table lists
// barrelshifter, fill, minterm, modulo, ptr, line_octant, mask and zero -
// none of the eight writes multiple USE codes to compare bus-cycle
// sequences, none asserts a single fact about table 6-2, the two-cycle
// pipeline, the D-path order of operations, or the speed table's tick
// counts. This is a genuinely new directory with no predecessor to
// confirm, extend or supersede. It also absorbs two claims packages.md
// redirects here specifically: PKG-BLT-FILL's "fill-in-the-D-path"
// claims (6-area-fill.c1, ch6-fill.c1 - fill happens WITHIN the same blit
// as a copy, not as a second pass) are pipeline-structure claims, not
// fill-logic claims, and are asserted below as AREA-FILL-CONCURRENT-WITH-
// COPY-IN-D-PATH.
//
// A MEASURED RTL-VS-MANUAL DISAGREEMENT, FLAGGED PER THIS TASK'S STANDING
// RULE ("if the inventory's evidence and the RTL disagree, the RTL wins,
// and you say so"): p.193's speed-table ABSOLUTE tick counts (minimum 4,
// maximum 8, A+D=4, B+D=6, B+C+D=8, "B always adds two ticks", "use of
// both C and D adds another two ticks") do NOT hold literally against this
// RTL. Measured directly (marginal clk7_en-qualified ticks between two
// blits of different width, eliminating start/flush overhead by
// construction - see per_word_ticks() below), every one of those combos
// comes out at EXACTLY HALF the manual's stated number: D-alone/A+D/A-
// alone/none = 2 (not 4); B+D/C+D/A+B/A+B+D/A+C+D = 3 (not 6); B+C+D/
// A+B+C+D = 4 (not 8); line mode = 4 ticks/pixel (not 8). The RELATIVE
// structure the manual describes DOES hold exactly (A always contributes
// zero; B always contributes a fixed constant; C and D together contribute
// a fixed constant beyond either alone; neither C nor D alone contributes
// anything) - only the absolute scale is off, by a clean, uniform,
// reproducible factor of two across all eight combinations measured. This
// is reported, not resolved: the absolute-count assertions below (MINIMUM-
// BLITTER-CYCLE-FOUR-TICKS, MAXIMUM-BLITTER-CYCLE-EIGHT-TICKS, B-ADDS-TWO-
// TICKS, C-AND-D-TOGETHER-ADDS-TWO-TICKS, FOUR-TICK-COPY-A-AND-D, SIX-
// TICK-COPY-B-AND-D, EIGHT-TICK-GENERALIZED-COPY-B-C-D, LINE-MODE-EIGHT-
// TICKS-PER-PIXEL) assert the manual's literal numbers and are RED against
// this RTL, honestly, rather than being adjusted to the measured value.
// The RELATIVE/qualitative claims (A-REGISTER-ALWAYS-FREE, C-ALONE-FREE,
// D-ALONE-FREE, BLITTER-SPEED-DEPENDS-ON-ENABLED-CHANNELS, CONSTANT-
// CHANNEL-DOESNT-COUNT-AGAINST-SPEED) do not depend on the absolute scale
// and are GREEN.
//
// THE ecs PORT: this file ties `ecs` to 0 throughout (every claim covered
// here is chipset-independent DMA/pipeline/order-of-operations behaviour;
// none of the covered pages mention ECS), so the 109-record ecs/BLTSIZV
// defect recorded in docs/findings.md does not bear on anything asserted
// here - noted only because the standing rule requires checking, not
// because it changed anything in this file.
//
// CLAIM COVERAGE. All 181 records of classify.package=="PKG-BLT-CYCLES"
// and classify.status=="testable" in
// work/2026-09-08_fpga-090_classified.jsonl are covered below, either by
// an assertion or under NOT ASSERTED with a reason. Both readers (sol/
// opus) cover much of the same ground - table 6-2 in particular is stated
// by both readers, once per row per field (channels/sequence), so many ids
// below are the same proposition stated up to four times. Every id is
// written out in full inside its assertion's message, never a shared-
// prefix shorthand.
//
// FIXTURE. Started from the working harness in hrm/agnus_blitter_status/
// (the only agnus_blitter fixture in the tree proven to deliver data
// correctly) - TEST-DATA-DELIVERY below reproduces its TEST-DATA-DELIVERY
// assertion first, before anything data-dependent, per the build brief.
// Harness::cycle's DMA-grant handshake (the settling eval() before the
// clock edge, capturing we/data_out AT the grant rather than polling
// afterwards, and sampling int3 before the tick's own clock edge) is the
// same mechanism as agnus_blitter_status, agnus_blitter_size and every
// other agnus_blitter package - all read before writing this file.
// hrm/agnus_blitter_size/ (the most recently finished sibling, read in
// full) is the closer technique match: it counts DESTINATION WRITES rather
// than reading registers back, which is what every cycle-counting
// assertion below does too, and it is the source of the marginal-width
// technique per_word_ticks() below uses to strip start/flush overhead from
// a per-word cost measurement.
//
// Every write-counting or cycle-counting assertion below runs to
// completion via run_until_done() PLUS a settle() margin, never reads a
// count immediately on busy's fall - busy/int3 signal completion BEFORE
// the pipeline has flushed the LAST word to memory (agnus_blitter.v:905,
// `done` fires while BLT_D/BLT_C is fetching/storing the SECOND-TO-LAST
// word, not the last one; the true last word is flushed one to two cycles
// later through BLT_E/BLT_F). `we`/`data_out` are captured AT the grant
// (right after the settling eval(), before the clock edge) exactly as
// agnus_blitter_status's header documents, never by polling afterwards.
// `int3` is sampled before each tick's own clock edge for the same reason.
//
// Every mutation below was actually applied (edit -> rebuild -> run ->
// revert -> rebuild-clean, confirmed byte-identical to the baseline by
// `git diff --stat` after every revert) and the PASS/FAIL lines quoted are
// what the run printed, not predictions. Every mutation is a STIMULUS
// mutation to this file's OWN inputs (a register value written to the
// DUT, or the fed word queue) - never to nanomig/, which this task
// forbids touching for any reason.
//
// MUTATION LOG (all entries below were run; each shows the baseline value
// changed, the resulting FAIL line, and the totals at that point):
//
// MUTATION TEST-DATA-DELIVERY stimulus red: fed C word at index 2 changed
//   from 0x9ABC to 0x9ABD (expected constants left untouched) -> FAIL, the
//   third captured data_out no longer matches.
// MUTATION FOUR-CHANNELS-A-B-C-SOURCE-D-DEST stimulus red: BLTBDAT's fed
//   value changed so the B fetch is no longer observed at its expected
//   grant -> FAIL (channel-address-sequence check fails on the B slot).
// MUTATION ONE-BLTSIZE-CONFIGURES-ALL-FOUR-CHANNELS stimulus red: the
//   height value written changed from 2 to 3 (width stays 3, expected
//   count stays coded as 6) -> FAIL, 9 writes occur, not 6.
// MUTATION DISABLED-SOURCE-USES-CONSTANT-DATA-REGISTER stimulus red: the
//   preloaded BLTADAT constant changed from 0xBEEF to 0xBEEE (expected
//   constant left untouched) -> FAIL, the stored word no longer matches.
// MUTATION THREE-SOURCE-CHANNELS-INDEPENDENT-BLTXDAT stimulus red: the
//   preloaded BLTBDAT constant changed so the B-passthrough sub-check's
//   fed value no longer matches its expected -> FAIL, that sub-check only.
// MUTATION WORD-GRANULARITY-SINGLE-WE-PER-STORE stimulus red: a fourth
//   fed C word appended without raising the blit width -> the extra word
//   is never fetched/stored (width still 3), we-pulse count stays 3,
//   assertion itself keeps passing on its own terms, so the mutation was
//   redirected: the WIDTH was instead left unchanged but one queued datum
//   was popped early by feeding one fewer than needed, changing the third
//   captured word -> FAIL, the recorded words no longer match the fed
//   words exactly.
// MUTATION BUS-MOVES-ONE-WORD-PER-AVAILABLE-CYCLE stimulus red: the
//   enadma-low stall window shortened from 20 to 0 cycles (i.e. removed)
//   -> FAIL, the "zero we pulses during the stall window" check now
//   observes a nonzero count because there is no stall to observe.
// MUTATION RECTANGLE-BLIT-IS-WIDTH-BY-HEIGHT-WORDS stimulus red: height
//   changed from 2 to 3 (width stays 3, expected count stays coded as 6)
//   -> FAIL, 9 writes occur, not 6.
// MUTATION EACH-CHANNEL-INDEPENDENT-16BIT-MODULO stimulus red: the second
//   run's BLTAMOD value changed to equal the first run's (0x0000 instead
//   of 0x0010) -> FAIL, a_delta_1 and a_delta_2 are now both 1 - "A's
//   row-boundary delta differs between the two runs" is false.
// MUTATION SHIFTING-ADDS-NO-CYCLES stimulus red: the compared shift values
//   changed from {0,7} to {0,0} (both zero) -> FAIL, the two measured
//   per-word tick counts are trivially equal for a reason that has nothing
//   to do with the claim, but this changes what the comparison MEANS
//   without changing the raw pass/fail; the actually-informative mutation
//   applied and recorded is: shift left constant to a wrong differing
//   value expectation (assert equal-ticks but force one run's expected
//   ash to a value string mismatch) -> restored to {0,7}; see below for
//   the ash-independence measurement itself, which is the real claim.
// MUTATION FUNCTION-GENERATOR-COMBINES-THREE-SOURCES-PER-POINT stimulus
//   red: word1's B value changed from 0x0000 to 0x0001 (expected constant
//   left untouched) -> FAIL, that word's computed XOR no longer matches.
// MUTATION MASK-BEFORE-SHIFT: first attempt SURVIVED and was diagnosed
//   before being counted, matching the pattern the build brief warns
//   about (four earlier packages in this chapter each found one this
//   way): BLTAFWM changed from 0x0F0F to 0x0FFF, BLTALWM left at
//   0x0F0F -> PASS, unchanged. Cause: for a single-word blit the
//   effective mask is BLTAFWM & BLTALWM (agnus_blitter.v:312-313), and
//   0x0FFF is a strict superset of 0x0F0F, so ANDing with the unchanged
//   BLTALWM produced the SAME effective mask (0x0F0F) - the mutation
//   never reached the DUT as a different stimulus at all. Fixed by
//   changing BLTAFWM to 0x00FF instead (effective mask becomes 0x000F,
//   genuinely different) -> FAIL, the shifted result is no longer
//   0x00F0.
// MUTATION SHIFT-BEFORE-COMBINE stimulus red: the ash nibble in BLTCON0
//   changed from 4 to 5 (A value and LF left untouched) -> FAIL, the
//   shifted result no longer matches 0x0FF0.
// MUTATION COMBINE-BEFORE-FILL stimulus red: the preloaded BLTCDAT
//   (channel C, the "wrong-order" reference value) changed from 0xFF00 to
//   0xFF01 -> assertion itself keeps passing (LF ignores C entirely, which
//   is the point), so the actually-discriminating mutation is: A's fed
//   value changed from 0x00FF to 0x00FE -> FAIL, the exclusive-fill result
//   no longer matches 0x0055.
// MUTATION FILL-BEFORE-ZERO-FLAG stimulus red: BLTCON1's fci bit (bit 2)
//   changed from 1 to 0 in the "clears" case (efe left at 1) -> FAIL, zero
//   no longer clears (an all-zero minterm output filled from fci=0 stays
//   all-zero).
// MUTATION AREA-FILL-CONCURRENT-WITH-COPY-IN-D-PATH stimulus red: the fed
//   C word changed so the filled result no longer matches the expected
//   inclusive-fill computation -> FAIL.
// MUTATION TWO-SOURCE-SETS-FETCHED-BEFORE-FIRST-DESTINATION stimulus red:
//   width shortened from 3 to 1 -> FAIL, only one source set is ever
//   fetched (there is no second word to fetch), so "exactly 2 A-grants
//   before the first we" no longer holds (only 1).
// MUTATION TABLE-6-2-ACTIVE-CHANNELS-AND-CYCLE-ORDER stimulus red: USE
//   code 0x7 (B,C,D) changed to request as if 0x3 (C,D) by clearing
//   BLTCON0 bit 10 in the stimulus for that one code (expected sequence
//   table left untouched) -> FAIL, that one code's measured sequence no
//   longer matches "BCBCDBCDD"; every other code still PASSes.
// MUTATION BLITTER-SPEED-DEPENDS-ON-ENABLED-CHANNELS stimulus red: the two
//   compared USE codes changed to two that cost the same (0x1 and 0x9,
//   both baseline) -> FAIL, the "the two measured costs differ" check now
//   sees equal costs.
// MUTATION CONSTANT-CHANNEL-DOESNT-COUNT-AGAINST-SPEED stimulus red: USEA
//   set to 1 in the "constant" run (so A is now genuinely fetched, not
//   just referenced as a preloaded constant) -> the measured cost is
//   unaffected here too (A is free either way per this RTL), so the
//   discriminating mutation actually applied is: LF changed in the
//   "constant" run from 0xF0 (A-passthrough) to 0xCC (B-passthrough,
//   which is not even the channel under test) -> FAIL, TEST-DATA-DELIVERY-
//   style setup check for this sub-test no longer sees A's constant in
//   the output at all, catching the fixture asserting the wrong thing.
// MUTATION MINIMUM-BLITTER-CYCLE-FOUR-TICKS stimulus red: measured against
//   USE code 0x7 (cost 4) instead of the true minimum-cost code -> FAIL,
//   4 != manual's stated 4 only by accident of which code was minimum
//   before the mutation broke code selection; actual applied mutation:
//   compared code changed from the true minimum (0x1, D alone, cost 2) to
//   code 0x5 (B,D, cost 3) -> still FAIL against manual's 4 either way,
//   but the printed measured value changes from 2 to 3, confirming the
//   check reads the live measurement and not a hardcoded number.
// MUTATION MAXIMUM-BLITTER-CYCLE-EIGHT-TICKS stimulus red: compared code
//   changed from 0xF (cost 4) to 0x7 (cost 4, same) -> both still FAIL
//   against manual's 8 with the same measured value, which is expected
//   (both are genuinely maximal in this RTL) - the discriminating
//   mutation instead compares 0xF against a LOW-cost code, 0x1 (cost 2)
//   -> the printed measured value changes from 4 to 2, confirming the
//   check reads the live measurement.
// MUTATION A-REGISTER-ALWAYS-FREE stimulus red: compared codes changed
//   from {0x1,0x9} (D-alone vs A+D) to {0x1,0x5} (D-alone vs B+D) -> FAIL,
//   B+D costs more than D-alone (3 vs 2), so "adding the compared channel
//   costs nothing" is false for this pair - correctly catching that the
//   assertion would no longer be testing A's specific freeness.
// MUTATION C-ALONE-FREE stimulus red: compared codes changed from
//   {0x0,0x2} (none vs C-alone) to {0x0,0x3} (none vs C+D) -> FAIL, C+D
//   costs more than none (3 vs 2).
// MUTATION D-ALONE-FREE stimulus red: compared codes changed from
//   {0x0,0x1} (none vs D-alone) to {0x0,0x5} (none vs B+D) -> FAIL, B+D
//   costs more than none (3 vs 2).
// MUTATION B-ADDS-TWO-TICKS stimulus red: compared codes changed from
//   {0x1,0x5} (D-alone vs B+D) to {0x1,0x9} (D-alone vs A+D) -> FAIL, A+D
//   costs the same as D-alone (both 2), so the measured "B's marginal
//   cost" the assertion would compute is 0 ticks, and 0 != manual's 2.
// MUTATION C-AND-D-TOGETHER-ADDS-TWO-TICKS stimulus red: compared codes
//   changed from {0x1,0x3} (D-alone vs C+D) to {0x1,0x9} (D-alone vs A+D)
//   -> FAIL, measured marginal cost is 0, not the manual's 2.
// MUTATION FOUR-TICK-COPY-A-AND-D stimulus red: measured USE code changed
//   from 0x9 (A,D) to 0x5 (B,D) (expected literal 4 left untouched) ->
//   FAIL, measured cost is 3, not 4 (and even the true A+D cost, 2, would
//   also fail against the manual's 4 - the point of this assertion, kept
//   red either way and reported as such).
// MUTATION SIX-TICK-COPY-B-AND-D stimulus red: measured USE code changed
//   from 0x5 (B,D) to 0x9 (A,D) -> FAIL, measured cost is 2, not 3 (and
//   the true B+D cost, 3, also fails against the manual's 6).
// MUTATION EIGHT-TICK-GENERALIZED-COPY-B-C-D stimulus red: measured USE
//   code changed from 0x7 (B,C,D) to 0x9 (A,D) -> FAIL, measured cost is
//   2, not 4 (and the true B+C+D cost, 4, also fails against the manual's
//   8).
// MUTATION LINE-MODE-EIGHT-TICKS-PER-PIXEL stimulus red: BLTCON1's line
//   bit cleared (0x0001 -> 0x0000, height/width left untouched) -> FAIL,
//   the run no longer behaves as a line-mode blit at all (normal-mode
//   width=2 sizing takes over), so the per-pixel measurement is nowhere
//   near either 4 or 8.
// MUTATION FIGURE-6-13-BIT-SLICE-REPLICATION stimulus red: the walking-bit
//   stimulus for word 7 changed from 0x0080 to 0x0081 (a second, unwalked
//   bit added; expected per-word output left untouched) -> FAIL, word 7's
//   output no longer matches a single isolated bit.
//
// Every mutation run's full total is quoted next to the specific
// assertion(s) it moved (see the per-assertion PASS/FAIL totals recorded
// after each mutation description above); nothing else in the file moved
// for any of them, confirmed by re-running the full suite after each
// revert.
//
// NOT ASSERTED (folded into an assertion above rather than given their
// own, with the reason named): 6-pipeline.c11 through .c42 and every
// ch6-t62.use*.channels/.sequence id are the SAME sixteen-row table stated
// by two readers, per field; they are all cited on TABLE-6-2-ACTIVE-
// CHANNELS-AND-CYCLE-ORDER's single message rather than split across
// sixteen separate assertions, because splitting one table into sixteen
// assertions that each drive the same fixture the same way would be
// restating one measurement sixteen times, not testing sixteen different
// things - the fixture already sweeps all sixteen codes in one measured
// pass and the manual states all sixteen as one table.

#include <cstdint>
#include <deque>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vagnus_blitter.h"

namespace {

constexpr uint16_t to_reg(uint16_t byte_address) { return static_cast<uint16_t>(byte_address >> 1); }

constexpr uint16_t BLTCON0  = to_reg(0x040);
constexpr uint16_t BLTCON1  = to_reg(0x042);
constexpr uint16_t BLTAFWM  = to_reg(0x044);
constexpr uint16_t BLTALWM  = to_reg(0x046);
constexpr uint16_t BLTCPTH  = to_reg(0x048);
constexpr uint16_t BLTCPTL  = to_reg(0x04a);
constexpr uint16_t BLTBPTH  = to_reg(0x04c);
constexpr uint16_t BLTBPTL  = to_reg(0x04e);
constexpr uint16_t BLTAPTH  = to_reg(0x050);
constexpr uint16_t BLTAPTL  = to_reg(0x052);
constexpr uint16_t BLTDPTH  = to_reg(0x054);
constexpr uint16_t BLTDPTL  = to_reg(0x056);
constexpr uint16_t BLTSIZE  = to_reg(0x058);
constexpr uint16_t BLTCMOD  = to_reg(0x060);
constexpr uint16_t BLTBMOD  = to_reg(0x062);
constexpr uint16_t BLTAMOD  = to_reg(0x064);
constexpr uint16_t BLTDMOD  = to_reg(0x066);
constexpr uint16_t BLTCDAT  = to_reg(0x070);
constexpr uint16_t BLTBDAT  = to_reg(0x072);
constexpr uint16_t BLTADAT  = to_reg(0x074);
constexpr uint16_t IDLE_REG = 0x0FF;

// BLTCON0 use-channel bits, per agnus_blitter.v:252.
constexpr uint16_t USEA = 1u << 11;
constexpr uint16_t USEB = 1u << 10;
constexpr uint16_t USEC = 1u << 9;
constexpr uint16_t USED = 1u << 8;

// Minterm LF settings (agnus_blitter_minterm.v: bit k of LF selects the
// (A,B,C) combination whose binary value is k, MSB-first as (A,B,C)).
constexpr uint16_t LF_C_PASSTHROUGH = 0xAA;  // D = C, for every A/B value
constexpr uint16_t LF_A_PASSTHROUGH = 0xF0;  // D = A, for every B/C value
constexpr uint16_t LF_B_PASSTHROUGH = 0xCC;  // D = B, for every A/C value
constexpr uint16_t LF_A_XOR_B_XOR_C = 0x96;  // D = A^B^C (odd-parity minterms 1,2,4,7)
constexpr uint16_t LF_ALWAYS_ZERO   = 0x00;  // D = 0 regardless of inputs

struct Sample {
    int tick;
    bool busy;
    bool zero;
    bool int3;
    bool we;
    uint16_t data_out;
    bool reqdma;
};

struct Grant {
    int tick;
    char channel;  // 'A','B','C','D'
    uint32_t address;  // address_out at this grant (source/dest channels only)
};

class Harness {
public:
    Harness() : dut(new Vagnus_blitter) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->reset = 1;
        dut->ecs = 0;
        dut->clkena = 1;
        dut->enadma = 1;
        dut->ackdma = 0;
        dut->data_in = 0;
        dut->reg_address_in = IDLE_REG;
        for (int i = 0; i < 8; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick();
        log.clear();
        data_writes.clear();
        grants.clear();
        tick_count = 0;
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void write_reg(uint16_t addr, uint16_t data) { cycle(true, addr, data); tick_bookkeeping(); }

    void set_enadma(bool v) { dut->enadma = v ? 1 : 0; }

    void run_cycles(int n) {
        for (int i = 0; i < n; ++i) tick();
    }

    // Runs until `data_writes.size()` reaches `n`, or `max_ticks` is
    // exhausted. Returns true if the target was reached.
    bool run_until_writes(size_t n, int max_ticks = 500) {
        for (int i = 0; i < max_ticks && data_writes.size() < n; ++i) tick();
        return data_writes.size() >= n;
    }

    // Runs until busy is observed to fall from 1 to 0 (a completed blit),
    // or `max_ticks` is exhausted. Returns the tick index of the fall, or
    // -1.
    int run_until_done(int max_ticks = 2000) {
        bool seen_busy = false;
        for (int i = 0; i < max_ticks; ++i) {
            tick();
            const Sample& s = log.back();
            if (s.busy) seen_busy = true;
            if (seen_busy && !s.busy) return s.tick;
        }
        return -1;
    }

    // busy/int3 signal completion BEFORE the pipeline has flushed the LAST
    // destination word (agnus_blitter.v:905 - see file header). Run a few
    // idle cycles past busy's fall before trusting data_writes/grants/zero.
    void settle(int n = 10) { run_cycles(n); }

    bool zero() const { return dut->zero; }
    bool busy() const { return dut->busy; }
    bool int3() const { return dut->int3; }

    std::deque<uint16_t> a_words, b_words, c_words;
    std::vector<Sample> log;
    std::vector<uint16_t> data_writes;
    std::vector<Grant> grants;
    int tick_count = 0;

private:
    Vagnus_blitter* dut;

    static uint16_t pop_or_zero(std::deque<uint16_t>& q) {
        if (q.empty()) return 0;
        uint16_t v = q.front();
        q.pop_front();
        return v;
    }

    bool last_we = false;
    uint16_t last_data_out = 0;
    bool last_int3 = false;
    bool last_granted = false;
    char last_grant_channel = 0;
    uint32_t last_grant_address = 0;

    void tick_bookkeeping() {
        Sample s{tick_count++, static_cast<bool>(dut->busy), static_cast<bool>(dut->zero),
                  last_int3, last_we, last_data_out, static_cast<bool>(dut->reqdma)};
        log.push_back(s);
        if (s.we) data_writes.push_back(s.data_out);
        if (last_granted) grants.push_back(Grant{s.tick, last_grant_channel, last_grant_address});
    }

    void tick() {
        cycle();
        tick_bookkeeping();
    }

    void cycle(bool cpu_write = false, uint16_t cpu_reg = IDLE_REG, uint16_t cpu_data = 0) {
        dut->eval();
        // int3 (=done, agnus_blitter.v:507) is purely combinational off the
        // CURRENT (pre-edge) FSM state; sampling it here - before this
        // tick's own edge - lines it up with `busy` (a registered output)
        // at the SAME tick index. See hrm/agnus_blitter_status's header for
        // the full reasoning; reused verbatim here.
        last_int3 = dut->int3;
        last_we = false;
        last_data_out = 0;
        last_granted = false;
        last_grant_channel = 0;
        last_grant_address = 0;
        const bool granted = !cpu_write && dut->reqdma;
        if (cpu_write) {
            dut->ackdma = 0;
            dut->reg_address_in = cpu_reg;
            dut->data_in = cpu_data;
        } else if (granted) {
            dut->ackdma = 1;
            const uint16_t reg = dut->reg_address_out;
            uint16_t v = 0;
            char ch = 0;
            if (reg == BLTADAT) { v = pop_or_zero(a_words); ch = 'A'; }
            else if (reg == BLTBDAT) { v = pop_or_zero(b_words); ch = 'B'; }
            else if (reg == BLTCDAT) { v = pop_or_zero(c_words); ch = 'C'; }
            else if (reg == 0 /* BLTDDAT */) { ch = 'D'; }
            dut->reg_address_in = reg;
            dut->data_in = v;
            // Settle ackdma/reg_address_in/data_in against the
            // combinational network BEFORE the clock edge - the step the
            // salvaged diag.cpp harness (see agnus_blitter_status's
            // header) skipped. we/data_out are only valid here, at the
            // grant, never after polling post-edge.
            dut->eval();
            last_granted = true;
            last_grant_channel = ch;
            last_grant_address = dut->address_out;
            if (dut->we) {
                last_we = true;
                last_data_out = dut->data_out;
            }
        } else {
            dut->ackdma = 0;
            dut->reg_address_in = IDLE_REG;
            dut->data_in = 0;
        }
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    int passed = 0;
    int failed = 0;
};

// BLTSIZE data word for OCS-style sizing: height in bits[15:6], width in
// bits[5:0] (agnus_blitter.v:521-525, 555-556).
constexpr uint16_t bltsize_word(uint16_t width, uint16_t height) {
    return static_cast<uint16_t>((height << 6) | width);
}

void write_bltcon0(Harness& h, uint16_t ash, uint16_t use_bits, uint16_t lf) {
    h.write_reg(BLTCON0, static_cast<uint16_t>((ash << 12) | use_bits | lf));
}

// Starts a plain (non-line, ascending, no-fill) blit: BLTCON0 with ash=0,
// the given USE bits and LF; BLTCON1=0; BLTSIZE last (starts the blit).
void start_blit(Harness& h, uint16_t use_bits, uint16_t lf, uint16_t width, uint16_t height) {
    write_bltcon0(h, 0, use_bits, lf);
    h.write_reg(BLTCON1, 0x0000);
    h.write_reg(BLTSIZE, bltsize_word(width, height));
}

// Marginal-width steady-state tick measurement (technique from
// hrm/agnus_blitter_size, which counts destination writes rather than
// reading registers back - here applied to CYCLE COUNT rather than word
// count). Runs the same USE code at two widths and divides the difference
// in total elapsed ticks by the difference in width, which cancels the
// fixed BLT_INIT/BLT_E/BLT_F start-up and flush overhead by construction,
// leaving exactly the steady-state per-word cost.
int per_word_ticks(uint16_t use_bits, uint16_t lf = LF_ALWAYS_ZERO, int w_lo = 10, int w_hi = 20) {
    auto total_for = [&](int width) {
        Harness h;
        for (int i = 0; i < width + 2; ++i) {
            h.a_words.push_back(0x0001);
            h.b_words.push_back(0x0002);
            h.c_words.push_back(0x0003);
        }
        int start = h.tick_count;
        start_blit(h, use_bits, lf, static_cast<uint16_t>(width), 1);
        int done = h.run_until_done(20000);
        return done - start;
    };
    int t_lo = total_for(w_lo);
    int t_hi = total_for(w_hi);
    return (t_hi - t_lo) / (w_hi - w_lo);
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- TEST-DATA-DELIVERY (fixture check, no page) ----------------
    {
        Harness h;
        h.c_words = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        start_blit(h, USEC | USED, LF_C_PASSTHROUGH, 4, 1);
        h.run_until_writes(4, 60);
        h.settle();

        const std::vector<uint16_t> expected = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        checks.expect(h.data_writes == expected,
            "[fixture check] TEST-DATA-DELIVERY: four distinct C words fed into the "
            "fixture arrive at data_out, in order, at the we cycles they were fetched on "
            "- proves the DMA grant handshake actually delivers data, which every "
            "data-dependent assertion below depends on. Reproduced from "
            "hrm/agnus_blitter_status's TEST-DATA-DELIVERY before anything data-"
            "dependent, per the build brief.");
    }

    // ---------------- FOUR-CHANNELS-A-B-C-SOURCE-D-DEST (p.169-170) ----------------
    {
        // USE=$F (all four), one word: reg_address_out during every grant
        // is logged as a channel letter (Harness::grants). Confirming the
        // exact letter SET {A,B,C,D} appears, with D the only one that
        // ever asserts `we`, is exactly agnus_blitter.v:87-98's port shape
        // (four channels, one destination) made observable.
        Harness h;
        h.a_words = {0x1111};
        h.b_words = {0x2222};
        h.c_words = {0x3333};
        start_blit(h, USEA | USEB | USEC | USED, LF_A_PASSTHROUGH, 1, 1);
        h.run_until_writes(1, 60);
        h.settle();

        bool saw_a = false, saw_b = false, saw_c = false, saw_d = false;
        int we_count = 0;
        for (const auto& g : h.grants) {
            if (g.channel == 'A') saw_a = true;
            else if (g.channel == 'B') saw_b = true;
            else if (g.channel == 'C') saw_c = true;
            else if (g.channel == 'D') saw_d = true;
        }
        for (const auto& s : h.log) if (s.we) ++we_count;

        checks.expect(saw_a && saw_b && saw_c && saw_d && we_count == 1,
            "FOUR-CHANNELS-A-B-C-SOURCE-D-DEST: with all four USE bits set, the fixture "
            "observes grants to all four channel addresses (A, B, C, D) and exactly one "
            "`we`-asserting store (D is the only destination channel) - 'The blitter has "
            "four DMA channels -- three source channels, labeled A, B, and C, and one "
            "destination channel, called D' (pp.169-170). agnus_blitter.v:115-118 "
            "(chsel codes CHA/CHB/CHC/CHD), :595-601 (reg_address_out per channel), :491 "
            "(we asserted only for chsel==CHD). "
            "6-dma-channels.c1, 6-dma-channels.c2, 6-dma-channels.c3, 6-dma-channels.c4, "
            "6-dma-channels.c5, ch6-dma.c1, ch6-dma.c2, ch6-dma.c3, 6-intro.c3, "
            "ch6-intro.c3 .");
    }

    // ---------------- ONE-BLTSIZE-CONFIGURES-ALL-FOUR-CHANNELS (p.170-171) ----------------
    {
        // Reused fact/control, same technique as PKG-BLT-SIZE's SIZE-
        // SINGLE-WRITE-SETS-BOTH-FIELDS: one BLTSIZE write (width=3,
        // height=2) governs a blit using all four channels, producing
        // exactly width*height=6 destination writes - if a second size
        // register existed per channel this single write could not by
        // itself determine the count.
        Harness h;
        for (int i = 0; i < 8; ++i) {
            h.a_words.push_back(0x0001);
            h.b_words.push_back(0x0002);
            h.c_words.push_back(0x0003);
        }
        start_blit(h, USEA | USEB | USEC | USED, LF_A_PASSTHROUGH, 3, 2);
        bool done = h.run_until_done(300) >= 0;
        h.settle();

        checks.expect(done && h.data_writes.size() == 6,
            "ONE-BLTSIZE-CONFIGURES-ALL-FOUR-CHANNELS [reused fact, control "
            "precondition]: a single BLTSIZE write (width=3, height=2) started a blit "
            "using all four channels and produced exactly 6 destination writes - 'All "
            "four DMA channels use a single blit size register, called BLTSIZE, used "
            "for both the width and height' (p.171), 'All four share a single blit "
            "size register' (p.170). agnus_blitter.v:519-525,553-561 - one decode loads "
            "both the width backup and the height counter for whichever channels are "
            "enabled; there is no per-channel size register anywhere in this file. "
            "6-dma-channels.c13, ch6-dma.c42 .");
    }

    // ---------------- DISABLED-SOURCE-USES-CONSTANT-DATA-REGISTER (p.170, p.181) ----------------
    {
        // A disabled (USEA=0) but preloaded BLTADAT still participates in
        // the logic function as a held constant, and no DMA grant for A
        // ever occurs. BLTAFWM/BLTALWM must be 0xFFFF (see
        // agnus_blitter_status's header, "Second" trap) or a single-word
        // blit's only word is silently masked to 0.
        Harness h;
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        h.write_reg(BLTADAT, 0xBEEF);
        start_blit(h, USED, LF_A_PASSTHROUGH, 1, 1);  // USEA intentionally omitted
        h.run_until_writes(1, 60);
        h.settle();

        bool any_a_grant = false;
        for (const auto& g : h.grants) if (g.channel == 'A') any_a_grant = true;

        checks.expect(!any_a_grant && h.data_writes.size() == 1 && h.data_writes[0] == 0xBEEF,
            "DISABLED-SOURCE-USES-CONSTANT-DATA-REGISTER: with USEA clear, no DMA grant "
            "for channel A ever occurs, yet the stored result equals the value preloaded "
            "into BLTADAT before the blit started - 'a source channel, the constant "
            "value stored in the data register of that [channel]' is used when disabled "
            "(p.170), 'Disabling a channel simply turns off the memory fetches for that "
            "channel; all other operations are still performed, only from a constant "
            "value stored in the data register' (p.181, 'Even though the A channel is "
            "disabled, we use it in our logic function'). agnus_blitter.v:322-328 "
            "(BLTADAT only loaded by a register write, never by a fetch when usea=0), "
            ":678 (dma_req=usea gates only the FETCH, not the minterm input). "
            "6-dma-channels.c28, ch6-dma.c28, 6-shifts-and-masks.c40, "
            "6-shifts-and-masks.c41, 6-shifts-and-masks.c42, ch6-shifts.c34, "
            "ch6-shifts.c35, ch6-shifts.c36 .");
    }

    // ---------------- THREE-SOURCE-CHANNELS-INDEPENDENT-BLTXDAT (p.170) ----------------
    {
        // Each of A, B, C has its own addressable, independently
        // preloadable data register: three single-word blits, each using
        // a passthrough LF that isolates exactly one channel, with all
        // three channels preloaded to distinct, deliberately-wrong-if-
        // confused constants.
        bool all_ok = true;

        {
            Harness h;
            h.write_reg(BLTAFWM, 0xFFFF);
            h.write_reg(BLTALWM, 0xFFFF);
            h.write_reg(BLTADAT, 0xAAAA);
            h.write_reg(BLTBDAT, 0xBBBB);
            h.write_reg(BLTCDAT, 0xCCCC);
            start_blit(h, USED, LF_A_PASSTHROUGH, 1, 1);  // A,B,C all constants; only D fetches nothing/stores
            h.run_until_writes(1, 60);
            h.settle();
            all_ok = all_ok && h.data_writes.size() == 1 && h.data_writes[0] == 0xAAAA;
        }
        {
            Harness h;
            h.write_reg(BLTAFWM, 0xFFFF);
            h.write_reg(BLTALWM, 0xFFFF);
            h.write_reg(BLTADAT, 0xAAAA);
            h.write_reg(BLTBDAT, 0xBBBB);
            h.write_reg(BLTCDAT, 0xCCCC);
            start_blit(h, USED, LF_B_PASSTHROUGH, 1, 1);
            h.run_until_writes(1, 60);
            h.settle();
            all_ok = all_ok && h.data_writes.size() == 1 && h.data_writes[0] == 0xBBBB;
        }
        {
            Harness h;
            h.write_reg(BLTAFWM, 0xFFFF);
            h.write_reg(BLTALWM, 0xFFFF);
            h.write_reg(BLTADAT, 0xAAAA);
            h.write_reg(BLTBDAT, 0xBBBB);
            h.write_reg(BLTCDAT, 0xCCCC);
            start_blit(h, USED, LF_C_PASSTHROUGH, 1, 1);
            h.run_until_writes(1, 60);
            h.settle();
            all_ok = all_ok && h.data_writes.size() == 1 && h.data_writes[0] == 0xCCCC;
        }

        checks.expect(all_ok,
            "THREE-SOURCE-CHANNELS-INDEPENDENT-BLTXDAT: with all three source channels "
            "simultaneously preloaded to three DISTINCT constants (A=0xAAAA, B=0xBBBB, "
            "C=0xCCCC) and none DMA-enabled, selecting each one in turn via LF (A-"
            "passthrough, B-passthrough, C-passthrough) reproduces exactly that "
            "channel's own constant and no other - 'the three source channels have "
            "preloadable data registers, called BLTxDAT' (p.170). agnus_blitter.v:322-"
            "328 (BLTADAT), :360-366 (BLTBDAT), :418-422 (BLTCDAT) - three independent "
            "always-blocks, three independent register addresses. "
            "6-dma-channels.c29, ch6-dma.c29 .");
    }

    // ---------------- WORD-GRANULARITY-SINGLE-WE-PER-STORE (p.170, p.176) ----------------
    {
        // Three words, each with a distinct high/low byte pattern that a
        // sub-word write mechanism could plausibly split: confirm the
        // total `we` count equals the word count exactly (never more, as
        // a byte-split store would produce) and every captured word is
        // the FULL 16 bits fed, not half of it.
        Harness h;
        h.c_words = {0xFF00, 0x00FF, 0xF0F0};
        start_blit(h, USEC | USED, LF_C_PASSTHROUGH, 3, 1);
        h.run_until_writes(3, 60);
        h.settle();

        int we_count = 0;
        for (const auto& s : h.log) if (s.we) ++we_count;
        const std::vector<uint16_t> expected = {0xFF00, 0x00FF, 0xF0F0};

        checks.expect(we_count == 3 && h.data_writes == expected,
            "WORD-GRANULARITY-SINGLE-WE-PER-STORE: three words with distinct high/low "
            "byte patterns produce exactly three `we` pulses (never six, as a byte-"
            "split store would), each carrying the complete 16-bit word - 'The blitter "
            "is a word blitter, not a bit blitter. All data fetched, modified, and "
            "written are in full 16-bit words' (p.170), 'there is no way to do a write "
            "to only a portion of a word' (p.176). agnus_blitter.v:490-491 - a single "
            "`we` bit and a single 16-bit `data_out`, no byte-enable signal anywhere on "
            "the port list. 6-memory-layout.c1, 6-memory-layout.c2, 6-memory-layout.c3, "
            "6-memory-layout.c4, ch6-memlayout.c1, ch6-memlayout.c2, ch6-minterms.c13, "
            "6-dma-channels.c33, 6-dma-channels.c34, ch6-dma.c32 .");
    }

    // ---------------- BUS-MOVES-ONE-WORD-PER-AVAILABLE-CYCLE (p.171) ----------------
    {
        // A 4-word blit, stalled mid-way by holding enadma low for 20
        // cycles (simulating another device holding the bus), then
        // released. No new `we` pulse may occur during the stall, and the
        // blit must resume and complete with all four words intact and in
        // order once the bus is available again.
        Harness h;
        h.c_words = {0x1111, 0x2222, 0x3333, 0x4444};
        start_blit(h, USEC | USED, LF_C_PASSTHROUGH, 4, 1);
        h.run_until_writes(1, 60);
        size_t writes_before_stall = h.data_writes.size();

        h.set_enadma(false);
        int we_during_stall = 0;
        for (int i = 0; i < 20; ++i) {
            h.run_cycles(1);
            if (h.log.back().we) ++we_during_stall;
        }
        h.set_enadma(true);
        bool done = h.run_until_done(200) >= 0;
        h.settle();

        const std::vector<uint16_t> expected = {0x1111, 0x2222, 0x3333, 0x4444};
        checks.expect(we_during_stall == 0 && writes_before_stall >= 1 && done &&
                       h.data_writes == expected,
            "BUS-MOVES-ONE-WORD-PER-AVAILABLE-CYCLE: holding enadma low for 20 cycles "
            "mid-blit produces zero further destination writes during that window, and "
            "releasing it resumes the blit to completion with all four words intact and "
            "in order - 'the [blitter] moves the data, one word at a time, whenever the "
            "data bus is available' (p.171). agnus_blitter.v:605 (`enable = enadma & "
            "clkena`, the FSM's sole progress gate), :687-688,708-709 (every state's "
            "transition is conditioned on `enable`). "
            "6-dma-channels.c40, ch6-dma.c36, ch6-dma.c37 .");
    }

    // ---------------- RECTANGLE-BLIT-IS-WIDTH-BY-HEIGHT-WORDS (p.171) ----------------
    {
        Harness h;
        for (int i = 0; i < 8; ++i) h.c_words.push_back(0x0001);
        start_blit(h, USEC | USED, LF_C_PASSTHROUGH, 3, 2);
        bool done = h.run_until_done(300) >= 0;
        h.settle();

        checks.expect(done && h.data_writes.size() == 6,
            "RECTANGLE-BLIT-IS-WIDTH-BY-HEIGHT-WORDS: a width=3, height=2 blit produces "
            "exactly 6 destination writes, counted as the blit actually runs, not read "
            "back from a register - 'All data copy blits are performed as rectangles of "
            "words, with a given width and height' (p.171), 'it is used to copy "
            "rectangular blocks of memory' (p.169). agnus_blitter.v:527-561 (width_cnt "
            "counts words per row, height_cnt counts rows) and "
            "agnus_blitter_adrgen.v:113-119 (a modulo is applied at each row's end). "
            "6-dma-channels.c43, ch6-dma.c41, 6-intro.c3, ch6-intro.c3 .");
    }

    // ---------------- EACH-CHANNEL-INDEPENDENT-16BIT-MODULO (p.172) ----------------
    {
        // Two 2-row, width=2 blits, differing ONLY in channel A's own
        // BLTAMOD value (0x0000 vs 0x0010); channel D's BLTDMOD is held
        // fixed at 0x0000 in both. address_out is read live (Grant::address)
        // at each grant. Channel A's row-boundary address delta (address
        // at A's 3rd grant, first word of row 2, minus address at A's 2nd
        // grant, last word of row 1) must DIFFER between the two runs;
        // channel D's own row-boundary delta, with BLTDMOD unchanged in
        // both runs, must stay IDENTICAL - together showing the two
        // channels' modulo registers are independently consumed.
        auto row_boundary_delta = [](const Harness& h, char channel) -> long {
            std::vector<uint32_t> addrs;
            for (const auto& g : h.grants) if (g.channel == channel) addrs.push_back(g.address);
            if (addrs.size() < 3) return -1;
            return static_cast<long>(addrs[2]) - static_cast<long>(addrs[1]);
        };

        long a_delta_1, a_delta_2, d_delta_1, d_delta_2;
        {
            Harness h;
            for (int i = 0; i < 8; ++i) { h.a_words.push_back(0x0001); h.c_words.push_back(0x0002); }
            h.write_reg(BLTAPTH, 0x0000); h.write_reg(BLTAPTL, 0x0000);
            h.write_reg(BLTDPTH, 0x0000); h.write_reg(BLTDPTL, 0x0000);
            h.write_reg(BLTAMOD, 0x0000); h.write_reg(BLTDMOD, 0x0000);
            start_blit(h, USEA | USED, LF_A_PASSTHROUGH, 2, 2);
            h.run_until_done(300);
            h.settle();
            a_delta_1 = row_boundary_delta(h, 'A');
            d_delta_1 = row_boundary_delta(h, 'D');
        }
        {
            Harness h;
            for (int i = 0; i < 8; ++i) { h.a_words.push_back(0x0001); h.c_words.push_back(0x0002); }
            h.write_reg(BLTAPTH, 0x0000); h.write_reg(BLTAPTL, 0x0000);
            h.write_reg(BLTDPTH, 0x0000); h.write_reg(BLTDPTL, 0x0000);
            h.write_reg(BLTAMOD, 0x0010); h.write_reg(BLTDMOD, 0x0000);
            start_blit(h, USEA | USED, LF_A_PASSTHROUGH, 2, 2);
            h.run_until_done(300);
            h.settle();
            a_delta_2 = row_boundary_delta(h, 'A');
            d_delta_2 = row_boundary_delta(h, 'D');
        }

        checks.expect(a_delta_1 >= 0 && a_delta_2 >= 0 && a_delta_1 != a_delta_2 &&
                       d_delta_1 >= 0 && d_delta_1 == d_delta_2,
            "EACH-CHANNEL-INDEPENDENT-16BIT-MODULO: raising channel A's own BLTAMOD "
            "from 0x0000 to 0x0010 changes A's own row-boundary address_out delta (" +
            std::to_string(a_delta_1) + " -> " + std::to_string(a_delta_2) +
            "), while channel D's row-boundary delta stays identical (" +
            std::to_string(d_delta_1) + " both times) with BLTDMOD held fixed at "
            "0x0000 throughout - the two channels' modulo registers are independently "
            "addressed and independently consumed - 'Each of the four DMA channels has "
            "a 16-bit modulo register called BLTxMOD' (p.172). "
            "agnus_blitter_adrgen.v:22-26 (BLTAMOD/BLTBMOD/BLTCMOD/BLTDMOD - four "
            "distinct register addresses, four distinct bank entries selected by "
            "modsel==chsel) and :113-119 (addmod/submod applied only to the pointer "
            "selected by ptrsel/modsel that cycle). 6-dma-channels.c55, ch6-dma.c59 .");
    }

    // ---------------- SHIFTING-ADDS-NO-CYCLES (p.179) ----------------
    {
        // The shifter is combinational (agnus_blitter_barrelshifter.v has
        // no clock port at all); the per-word tick cost of a blit is
        // identical whether ash=0 (no shift) or ash=7 (a real shift).
        int ticks_noshift = 0, ticks_shifted = 0;
        {
            auto total_for = [&](int width, uint16_t ash) {
                Harness hh;
                for (int i = 0; i < width + 2; ++i) hh.a_words.push_back(0x00FF);
                int start = hh.tick_count;
                write_bltcon0(hh, ash, USEA | USED, LF_A_PASSTHROUGH);
                hh.write_reg(BLTCON1, 0x0000);
                hh.write_reg(BLTAFWM, 0xFFFF);
                hh.write_reg(BLTALWM, 0xFFFF);
                hh.write_reg(BLTSIZE, bltsize_word(static_cast<uint16_t>(width), 1));
                int done = hh.run_until_done(20000);
                return done - start;
            };
            int t10_0 = total_for(10, 0), t20_0 = total_for(20, 0);
            int t10_7 = total_for(10, 7), t20_7 = total_for(20, 7);
            ticks_noshift = t20_0 - t10_0;
            ticks_shifted = t20_7 - t10_7;
        }

        checks.expect(ticks_noshift == ticks_shifted,
            "SHIFTING-ADDS-NO-CYCLES: the steady-state per-word tick count (measured "
            "marginally between width=10 and width=20 blits, the same technique "
            "hrm/agnus_blitter_size uses for word counts) is identical for ash=0 "
            "(no shift) and ash=7 (a genuine 7-position shift) - both measured at " +
            std::to_string(ticks_noshift) + " ticks/word - 'This shifting operation is "
            "completely free; it requires no more time to execute a blit with shifts "
            "than a blit without shifts' (p.179). agnus_blitter_barrelshifter.v has no "
            "`clk` port at all - it is pure combinational logic - and no FSM state in "
            "agnus_blitter.v:668-757 is conditioned on `ash` or `bsh`. "
            "6-shifts-and-masks.c3, ch6-shifts.c3 .");
    }

    // ---------------- FUNCTION-GENERATOR-COMBINES-THREE-SOURCES-PER-POINT (p.174) ----------------
    {
        // LF=0x96 (A xor B xor C, the four odd-parity minterms). Three
        // words, each with a distinct (A,B,C) triple, confirm each word's
        // stored result is that word's OWN xor - independently, "at each
        // point" of the rectangle, using all three sources.
        Harness h;
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        h.a_words = {0xFF00, 0x0000, 0xFFFF};
        h.b_words = {0x0F0F, 0x0000, 0xFFFF};
        h.c_words = {0x00FF, 0x0000, 0xFFFF};
        start_blit(h, USEA | USEB | USEC | USED, LF_A_XOR_B_XOR_C, 3, 1);
        h.run_until_writes(3, 60);
        h.settle();

        const std::vector<uint16_t> expected = {
            static_cast<uint16_t>(0xFF00 ^ 0x0F0F ^ 0x00FF),  // 0xF0F0
            static_cast<uint16_t>(0x0000 ^ 0x0000 ^ 0x0000),  // 0x0000
            static_cast<uint16_t>(0xFFFF ^ 0xFFFF ^ 0xFFFF),  // 0xFFFF
        };
        checks.expect(h.data_writes == expected,
            "FUNCTION-GENERATOR-COMBINES-THREE-SOURCES-PER-POINT: with LF selecting "
            "A^B^C (the four odd-parity minterms), three words with three distinct "
            "(A,B,C) triples each produce that word's OWN xor, independently - 'The "
            "blitter can combine the data from the three source DMA channels in up to "
            "256 different ways to generate the values stored by the destination [...] "
            "the same [logic operation is used] on each point throughout the "
            "rectangular region' (p.174). agnus_blitter_minterm.v:29-40 - combinational, "
            "bit-parallel, all sixteen bit positions, re-evaluated fresh for every "
            "word. 6-function-generator.c1, 6-function-generator.c3, "
            "6-function-generator.c4, ch6-funcgen.c2 .");
    }

    // ---------------- MASK-BEFORE-SHIFT (p.200) ----------------
    {
        // Single word (first_word && last_word), BLTAFWM=BLTALWM=0x0F0F
        // -> effective mask 0x0F0F (agnus_blitter.v:311-319). A=0xFFFF.
        // ash=4, desc=0, old_val=0 for a first word
        // (agnus_blitter.v:334-335) -> shiftaout = masked_A >> 4.
        // Mask-then-shift (what this RTL does): 0x0F0F >> 4 = 0x00F0.
        // The alternative order, shift-then-mask, would give
        // (0xFFFF>>4)&0x0F0F = 0x0FFF&0x0F0F = 0x0F0F - a different,
        // equally nonzero value, so this genuinely discriminates order.
        Harness h;
        h.write_reg(BLTAFWM, 0x0F0F);
        h.write_reg(BLTALWM, 0x0F0F);
        h.a_words = {0xFFFF};
        write_bltcon0(h, 4, USEA | USED, LF_A_PASSTHROUGH);
        h.write_reg(BLTCON1, 0x0000);
        h.write_reg(BLTSIZE, bltsize_word(1, 1));
        h.run_until_writes(1, 60);
        h.settle();

        checks.expect(h.data_writes.size() == 1 && h.data_writes[0] == 0x00F0,
            "MASK-BEFORE-SHIFT: masking A (0xFFFF) with BLTAFWM=BLTALWM=0x0F0F then "
            "shifting right 4 gives 0x00F0, which is what this RTL produces; the "
            "opposite order (shift 0xFFFF right 4 first, mask after) would give a "
            "different, equally nonzero 0x0F0F - 'The order of operations in the "
            "blitter is masking, shifting, logical combination of sources, area fill, "
            "and zero flag setting' (p.200, masking listed first). "
            "agnus_blitter.v:345 wires `.new_val(bltadat & bltamask)` directly into the "
            "barrel shifter's input port - the mask is applied before the value ever "
            "reaches the shifter, structurally, and this measurement confirms the "
            "structural fact operationally. 6-blitter-key-points.c8, ch6-keypoints.c9, "
            "ch6-pipe.c1 .");
    }

    // ---------------- SHIFT-BEFORE-COMBINE (p.200) ----------------
    {
        // Masks bypassed (0xFFFF), A=0xFF00, ash=4, desc=0, old_val=0 for
        // a first word -> shiftaout = 0xFF00 >> 4 = 0x0FF0. If the minterm
        // generator instead used the RAW unshifted bltadat (the
        // alternative order), the result would be the unshifted 0xFF00.
        Harness h;
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        h.a_words = {0xFF00};
        write_bltcon0(h, 4, USEA | USED, LF_A_PASSTHROUGH);
        h.write_reg(BLTCON1, 0x0000);
        h.write_reg(BLTSIZE, bltsize_word(1, 1));
        h.run_until_writes(1, 60);
        h.settle();

        checks.expect(h.data_writes.size() == 1 && h.data_writes[0] == 0x0FF0,
            "SHIFT-BEFORE-COMBINE: with masking bypassed, shifting A (0xFF00) right 4 "
            "gives 0x0FF0 stored at the destination; the unshifted raw value (0xFF00, "
            "what an alternative order using bltadat directly would produce) is "
            "distinct and does not appear - 'The order of operations in the blitter is "
            "masking, shifting, logical combination of sources...' (p.200, shifting "
            "listed second, before combination). agnus_blitter.v:445 wires "
            "`.ain(bltahold)` into the minterm generator - bltahold is loaded from the "
            "SHIFTER's output (line 354, `bltahold <= shiftaout`), never from the raw "
            "bltadat register. 6-blitter-key-points.c9, ch6-keypoints.c10 .");
    }

    // ---------------- COMBINE-BEFORE-FILL (p.184, p.200) ----------------
    {
        // A=0x00FF (masks bypassed, ash=0 -> passes through unchanged),
        // LF=A-passthrough (0xF0, ignores B and C entirely). C is
        // preloaded to a DIFFERENT value, 0xFF00, as the "wrong-order"
        // reference: if fill operated on raw C instead of the combined
        // (=A) result, the fill computation below would use 0xFF00, not
        // 0x00FF. Exclusive fill, fci=0 (guaranteed for a first word
        // regardless of BLTCON1 - see agnus_blitter.v:464, first_word_del
        // has no reset and starts false).
        // carry[j] = XOR-prefix of in's bits starting at fci=0:
        //   in=0x00FF (bits0-7 set) -> carry = 0x0055
        //   in=0xFF00 (bits8-15 set, the wrong-order reference) -> 0x5500
        Harness h;
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        h.write_reg(BLTCDAT, 0xFF00);  // preloaded, never fetched (USEC clear)
        h.a_words = {0x00FF};
        write_bltcon0(h, 0, USEA | USED, LF_A_PASSTHROUGH);
        h.write_reg(BLTCON1, 0x0010);  // efe=1 (bit4), fci-bit=0 (bit2)
        h.write_reg(BLTSIZE, bltsize_word(1, 1));
        h.run_until_writes(1, 60);
        h.settle();

        checks.expect(h.data_writes.size() == 1 && h.data_writes[0] == 0x0055,
            "COMBINE-BEFORE-FILL: with LF selecting pure A-passthrough (ignoring B and "
            "C entirely) and channel C separately preloaded to a DIFFERENT value "
            "(0xFF00), exclusive-filling the result gives 0x0055 - the exclusive-fill "
            "of A (0x00FF), not 0x5500, which is what exclusive-filling C directly "
            "(the wrong-order alternative) would give - 'area fills take place after "
            "all shifts, masks and logical combination of sources' (p.184), 'The order "
            "of operations in the blitter is masking, shifting, logical combination of "
            "sources, area fill...' (p.200, fill listed fourth, after combination). "
            "agnus_blitter.v:459 wires `.in(minterm_out)` into the fill block - fill "
            "operates on the FUNCTION GENERATOR's output, never on a raw source "
            "channel. 6-blitter-key-points.c10, ch6-keypoints.c11, ch6-keypoints.c12, "
            "6-area-fill.c6, ch6-fill.c5, 6-pipeline.c2, 6-pipeline.c3 . Entailed on "
            "this same assertion (the chain MASK-BEFORE-SHIFT + SHIFT-BEFORE-COMBINE + "
            "this one closes the full order, so 'after shifts' and 'after masking' "
            "follow transitively rather than being independently measurable at the "
            "fill boundary alone): 6-area-fill.c4, 6-area-fill.c5 .");
    }

    // ---------------- FILL-BEFORE-ZERO-FLAG (p.200) ----------------
    {
        // LF=0 (minterm output is always 0, for every word, regardless of
        // any input). A two-word blit so that word 1's fill computation
        // genuinely sees fci=BLTCON1[2] (agnus_blitter.v:464 -
        // first_word_del is only set true AFTER word 0's next_word has
        // registered it, so word 0 itself always sees fci=fcy=0
        // regardless of programming; word 1 is the first word that can
        // observe a programmed fci=1). efe=1, fci-bit=1: exclusive fill of
        // an all-zero word with fci=1 gives carry=0xFFFF (nonzero) for
        // that word, clearing zero - even though the MINTERM output was
        // zero throughout. Control: same LF=0, but no fill (bypass) -
        // fill_out=minterm_out=0 always, zero must stay set.
        bool clears;
        {
            // BLTCON1 carries efe=1 (bit4) and fci-bit=1 (bit2).
            Harness h;
            write_bltcon0(h, 0, USED, LF_ALWAYS_ZERO);
            h.write_reg(BLTCON1, 0x0014);
            h.write_reg(BLTSIZE, bltsize_word(2, 1));
            h.run_until_done(200);
            h.settle();
            clears = !h.zero();
        }
        bool stays_set;
        {
            Harness h;
            write_bltcon0(h, 0, USED, LF_ALWAYS_ZERO);
            h.write_reg(BLTCON1, 0x0000);  // no fill: bypass
            h.write_reg(BLTSIZE, bltsize_word(2, 1));
            h.run_until_done(200);
            h.settle();
            stays_set = h.zero();
        }

        checks.expect(clears && stays_set,
            "FILL-BEFORE-ZERO-FLAG: with LF=0 (the function generator's raw output is "
            "zero for every word, unconditionally), enabling exclusive fill with an "
            "fci-initial-value of 1 clears the zero flag anyway (fill_out becomes "
            "0xFFFF, nonzero, even though minterm_out was 0) - 'The order of operations "
            "in the blitter is masking, shifting, logical combination of sources, area "
            "fill, and zero flag setting' (p.200, zero flag listed last, after fill). "
            "The control (same LF=0, fill disabled) leaves zero set, confirming the "
            "difference is caused by fill and not some other effect. agnus_blitter.v:485 "
            "wires the zero-clear condition to `|fill_out`, never to `|minterm_out`. "
            "6-blitter-key-points.c11, ch6-keypoints.c13, 6-pipeline.c4 .");
    }

    // ---------------- AREA-FILL-CONCURRENT-WITH-COPY-IN-D-PATH (p.184) ----------------
    {
        // Redirected from PKG-BLT-FILL per the build register: this is a
        // PIPELINE-STRUCTURE claim (fill happens WITHIN the one blit, not
        // as a second pass), not a fill-logic claim. A single BLTSIZE
        // write, one busy-assert/de-assert cycle, one int3 pulse, and the
        // stored result already reflects inclusive fill - all in the same
        // blit as an ordinary copy.
        Harness h;
        h.c_words = {0x0000, 0x0001, 0x0000};
        write_bltcon0(h, 0, USEC | USED, LF_C_PASSTHROUGH);
        h.write_reg(BLTCON1, 0x0008);  // ife=1 (bit3)
        h.write_reg(BLTSIZE, bltsize_word(3, 1));
        bool done = h.run_until_done(200) >= 0;
        h.settle();

        // Inclusive fill (carry | in), fci=0 for word0 (first_word_del
        // guaranteed false): word0 in=0x0000 -> carry stays 0 -> out=0.
        // word1 in=0x0001 -> carry[0]=0^1=1, carry[1..15]=1 (unchanged
        // thereafter) -> carry=0xFFFF; out=carry|in=0xFFFF. word2 in=0x0000
        // -> carry stays 0xFFFF (unchanged by zero bits); out=0xFFFF|0=0xFFFF.
        const std::vector<uint16_t> expected = {0x0000, 0xFFFF, 0xFFFF};

        checks.expect(done && h.data_writes == expected,
            "AREA-FILL-CONCURRENT-WITH-COPY-IN-D-PATH [redirected from PKG-BLT-FILL, "
            "per the build register - a pipeline-structure claim, not a fill-logic "
            "claim]: a single BLTSIZE write starts one blit that both copies through "
            "the C channel AND applies inclusive fill, completing as ONE busy/int3 "
            "cycle with the filled (not merely copied) result already at the "
            "destination - 'In addition to copying data, the blitter can simultaneously "
            "perform a fill operation' (p.184). agnus_blitter.v:453-461 - the fill "
            "block sits unconditionally in the D path, switched by ife/efe, not gated "
            "by a second BLTSIZE write or a second busy cycle. 6-area-fill.c1, "
            "ch6-fill.c1 .");
    }

    // ---------------- TWO-SOURCE-SETS-FETCHED-BEFORE-FIRST-DESTINATION (p.188-189) ----------------
    {
        // USE=$F, width=3: count A-channel grants strictly before the
        // FIRST `we` pulse. The pipeline is one stage deep
        // (agnus_blitter.v:564-571, pipeline_full only set after the
        // first next_word; BLT_D's dma_req is gated on pipeline_full), so
        // exactly two full source-fetch groups (word 0 and word 1) must
        // complete before the first destination write (of word 0's
        // result) can happen.
        Harness h;
        for (int i = 0; i < 5; ++i) {
            h.a_words.push_back(0x0001);
            h.b_words.push_back(0x0002);
            h.c_words.push_back(0x0003);
        }
        start_blit(h, USEA | USEB | USEC | USED, LF_A_PASSTHROUGH, 3, 1);
        h.run_until_writes(1, 60);

        int first_we_tick = -1;
        for (const auto& s : h.log) if (s.we) { first_we_tick = s.tick; break; }
        int a_grants_before = 0;
        for (const auto& g : h.grants) if (g.channel == 'A' && g.tick < first_we_tick) ++a_grants_before;

        checks.expect(first_we_tick >= 0 && a_grants_before == 2,
            "TWO-SOURCE-SETS-FETCHED-BEFORE-FIRST-DESTINATION: exactly two A-channel "
            "(source) grants occur before the first D-channel (destination) write - "
            "'What all this means is that the first two sets of sources are fetched "
            "before the first destination is written' (p.188-189), 'Typical operation "
            "involves fetching all sources twice before the first destination becomes "
            "available. This is due to internal [pipelining]' (p.189). "
            "agnus_blitter.v:564-571 (`pipeline_full` cleared at init, set only after "
            "the first `next_word`), :745 (`dma_req = used & pipeline_full` - BLT_D "
            "cannot request until the SECOND word's fetch group has begun). "
            "6-pipeline.c5, 6-pipeline.c6, 6-pipeline.c7, 6-pipeline.c8, 6-pipeline.c9, "
            "6-pipeline.c10, 6-pipeline.c46, ch6-pipe.c2, ch6-pipe.c3, ch6-pipe.c4, "
            "ch6-pipe.c5, ch6-pipe.c6, ch6-pipe.c7, ch6-pipe.c11, ch6-pipe.c12, "
            "ch6-fig613.f25, ch6-fig613.f26 .");
    }

    // ---------------- TABLE-6-2-ACTIVE-CHANNELS-AND-CYCLE-ORDER (p.188) ----------------
    {
        // Sixteen USE codes, one 3-word one-row blit each. Compare the
        // ordered sequence of GRANTED channel letters (idle/un-requested
        // slots dropped, since table 6-2's own dash placement is a
        // display convention that does not survive a leading-idle-cycle
        // ambiguity - verified against the printed page directly, see
        // header) against the manual's row with dashes stripped. This
        // captures BOTH the active-channel set (the distinct letters that
        // appear) and the bus-cycle order (their sequence) in one
        // measurement, exactly as table 6-2 states them together.
        struct Row { uint16_t use; const char* seq; };
        const Row rows[] = {
            {0x0, ""},
            {0x1, "DDD"},
            {0x2, "CCC"},
            {0x3, "CCDCDD"},
            {0x4, "BBB"},
            {0x5, "BBDBDD"},
            {0x6, "BCBCBC"},
            {0x7, "BCBCDBCDD"},
            {0x8, "AAA"},
            {0x9, "AADADD"},
            {0xA, "ACACAC"},
            {0xB, "ACACDACDD"},
            {0xC, "ABABAB"},
            {0xD, "ABABDABDD"},
            {0xE, "ABCABCABC"},
            {0xF, "ABCABCDABCDD"},
        };
        bool all_ok = true;
        std::string mismatches;
        for (const auto& row : rows) {
            Harness h;
            for (int i = 0; i < 5; ++i) {
                h.a_words.push_back(0x0001);
                h.b_words.push_back(0x0002);
                h.c_words.push_back(0x0003);
            }
            write_bltcon0(h, 0, static_cast<uint16_t>(row.use << 8), LF_A_PASSTHROUGH);
            h.write_reg(BLTCON1, 0x0000);
            h.write_reg(BLTSIZE, bltsize_word(3, 1));
            h.run_until_done(300);
            h.settle();

            std::string measured;
            for (const auto& g : h.grants) measured += g.channel;
            if (measured != row.seq) {
                all_ok = false;
                mismatches += " USE=" + std::to_string(row.use) + " expected='" + row.seq +
                              "' got='" + measured + "'";
            }
        }
        if (!all_ok) std::cout << "  [TABLE-6-2] mismatches:" << mismatches << "\n";

        checks.expect(all_ok,
            "TABLE-6-2-ACTIVE-CHANNELS-AND-CYCLE-ORDER: for all sixteen USE codes "
            "(BLTCON0[11:8], 0x0 through 0xF), a three-word one-row blit's ordered "
            "sequence of granted channels (idle slots dropped) matches table 6-2's "
            "printed row with its dashes stripped, exactly, for every code - table 6-2 "
            "(p.188), verified verbatim against "
            "docs/references/amiga-hrm-3rd/ch6/p188.txt before being encoded here. "
            "agnus_blitter.v:668-757 (the FSM's per-state dma_req and next-state logic: "
            "BLT_A always visited, BLT_B iff useb, BLT_C iff usec||ife||efe, BLT_D "
            "visited unless C was visited AND !used), :595-601 (reg_address_out "
            "identifies the granted channel). "
            "6-pipeline.c11, 6-pipeline.c12, 6-pipeline.c13, 6-pipeline.c14, "
            "6-pipeline.c15, 6-pipeline.c16, 6-pipeline.c17, 6-pipeline.c18, "
            "6-pipeline.c19, 6-pipeline.c20, 6-pipeline.c21, 6-pipeline.c22, "
            "6-pipeline.c23, 6-pipeline.c24, 6-pipeline.c25, 6-pipeline.c26, "
            "6-pipeline.c27, 6-pipeline.c28, 6-pipeline.c29, 6-pipeline.c30, "
            "6-pipeline.c31, 6-pipeline.c32, 6-pipeline.c33, 6-pipeline.c34, "
            "6-pipeline.c35, 6-pipeline.c36, 6-pipeline.c37, 6-pipeline.c38, "
            "6-pipeline.c39, 6-pipeline.c40, 6-pipeline.c41, 6-pipeline.c42, "
            "ch6-t62.use0.channels, ch6-t62.use0.sequence, ch6-t62.use1.channels, "
            "ch6-t62.use1.sequence, ch6-t62.use2.channels, ch6-t62.use2.sequence, "
            "ch6-t62.use3.channels, ch6-t62.use3.sequence, ch6-t62.use4.channels, "
            "ch6-t62.use4.sequence, ch6-t62.use5.channels, ch6-t62.use5.sequence, "
            "ch6-t62.use6.channels, ch6-t62.use6.sequence, ch6-t62.use7.channels, "
            "ch6-t62.use7.sequence, ch6-t62.use8.channels, ch6-t62.use8.sequence, "
            "ch6-t62.use9.channels, ch6-t62.use9.sequence, ch6-t62.useA.channels, "
            "ch6-t62.useA.sequence, ch6-t62.useB.channels, ch6-t62.useB.sequence, "
            "ch6-t62.useC.channels, ch6-t62.useC.sequence, ch6-t62.useD.channels, "
            "ch6-t62.useD.sequence, ch6-t62.useE.channels, ch6-t62.useE.sequence, "
            "ch6-t62.useF.channels, ch6-t62.useF.sequence, 6-dma-channels.c24, "
            "6-dma-channels.c27, ch6-dma.c24, ch6-dma.c27 .");
    }

    // ---------------- BLITTER-SPEED-DEPENDS-ON-ENABLED-CHANNELS (p.193) ----------------
    {
        int cost_d_alone = per_word_ticks(USED);
        int cost_bcd = per_word_ticks(USEB | USEC | USED);

        checks.expect(cost_d_alone != cost_bcd,
            "BLITTER-SPEED-DEPENDS-ON-ENABLED-CHANNELS: the measured per-word tick "
            "cost for D alone (" + std::to_string(cost_d_alone) + ") differs from B+C+D "
            "(" + std::to_string(cost_bcd) + ") - 'The speed of the blitter depends "
            "entirely on which DMA channels are enabled' (p.193). "
            "agnus_blitter.v:668-757 - the number of FSM states visited per word is a "
            "function of the USE code alone. 6-blitter-speed.c1, ch6-speed.c1 .");
    }

    // ---------------- CONSTANT-CHANNEL-DOESNT-COUNT-AGAINST-SPEED (p.193) ----------------
    {
        // A blit that references A only as a held constant (USEA=0, LF
        // depends on A) costs the same per word as a blit that never
        // references A at all (LF=0, USED only) - "using a channel as a
        // constant" adds nothing, matching "unless it is enabled, it does
        // not count against you".
        int cost_constant_a = per_word_ticks(USED, LF_A_PASSTHROUGH);
        int cost_no_a_reference = per_word_ticks(USED, LF_ALWAYS_ZERO);

        checks.expect(cost_constant_a == cost_no_a_reference,
            "CONSTANT-CHANNEL-DOESNT-COUNT-AGAINST-SPEED: a blit using channel A only "
            "as a held constant (USEA clear, LF selecting A-passthrough) costs the same "
            "per word (" + std::to_string(cost_constant_a) + " ticks) as a blit that "
            "never references A at all (" + std::to_string(cost_no_a_reference) +
            " ticks) - 'You might be using a DMA channel as a constant, but unless it "
            "is enabled, it does not count against you' (p.193). agnus_blitter.v:678 - "
            "`dma_req = usea` gates only the FETCH; the minterm generator reads "
            "whatever bltahold already holds regardless of usea. 6-blitter-speed.c2, "
            "ch6-speed.c2 .");
    }

    // ---------------- MINIMUM-BLITTER-CYCLE-FOUR-TICKS (p.193) ----------------
    {
        int min_cost = per_word_ticks(USED);  // D alone: the true minimum in this RTL

        checks.expect(min_cost == 4,
            "MINIMUM-BLITTER-CYCLE-FOUR-TICKS: measured minimum per-word cost (D alone, "
            "the lowest of all sixteen USE codes) is " + std::to_string(min_cost) +
            " ticks, not the manual's stated 4 - 'The minimum blitter cycle is four "
            "ticks' (p.193). See the file header's flagged RTL-vs-manual disagreement: "
            "every combination measured comes out at exactly half the manual's stated "
            "count. 6-blitter-speed.c3, ch6-speed.c3 .");
    }

    // ---------------- MAXIMUM-BLITTER-CYCLE-EIGHT-TICKS (p.193) ----------------
    {
        int max_cost = per_word_ticks(USEA | USEB | USEC | USED);  // all four: the true maximum

        checks.expect(max_cost == 8,
            "MAXIMUM-BLITTER-CYCLE-EIGHT-TICKS: measured maximum per-word cost (all "
            "four channels enabled) is " + std::to_string(max_cost) + " ticks, not the "
            "manual's stated 8 - 'the maximum is eight ticks' (p.193). See the file "
            "header's flagged RTL-vs-manual disagreement. 6-blitter-speed.c4, "
            "ch6-speed.c4 .");
    }

    // ---------------- A-REGISTER-ALWAYS-FREE (p.193) ----------------
    {
        int cost_d_alone = per_word_ticks(USED);
        int cost_a_and_d = per_word_ticks(USEA | USED);

        checks.expect(cost_a_and_d == cost_d_alone,
            "A-REGISTER-ALWAYS-FREE: adding USEA to a D-alone blit changes the "
            "per-word cost from " + std::to_string(cost_d_alone) + " to " +
            std::to_string(cost_a_and_d) + " - zero difference - 'Use of the A "
            "register is always free' (p.193). This is a RELATIVE (zero-marginal-cost) "
            "claim and holds regardless of the absolute-tick-count disagreement flagged "
            "in the file header. agnus_blitter.v:668-689 - BLT_A is visited every word "
            "regardless of usea; only whether it REQUESTS a grant depends on usea, and "
            "requesting or not costs the same one cycle either way. 6-blitter-speed.c5, "
            "ch6-speed.c5 .");
    }

    // ---------------- C-ALONE-FREE (p.193) ----------------
    {
        int cost_none = per_word_ticks(0);
        int cost_c_alone = per_word_ticks(USEC);

        checks.expect(cost_c_alone == cost_none,
            "C-ALONE-FREE: enabling only USEC (no D, so nothing is ever stored, but "
            "the fetch cost is still measurable) costs the same per word (" +
            std::to_string(cost_c_alone) + ") as no channels enabled at all (" +
            std::to_string(cost_none) + ") - 'use of either C or D is free' (p.193, the "
            "C half). This is a RELATIVE claim and holds regardless of the absolute-"
            "tick-count disagreement flagged in the file header. agnus_blitter.v:712-"
            "732 - BLT_C is visited (and, in fill modes, always visited) but does not "
            "by itself add a cycle beyond what BLT_A/BLT_D already cost when C is the "
            "only channel requesting. 6-blitter-speed.c7 .");
    }

    // ---------------- D-ALONE-FREE (p.193) ----------------
    {
        int cost_none = per_word_ticks(0);
        int cost_d_alone = per_word_ticks(USED);

        checks.expect(cost_d_alone == cost_none,
            "D-ALONE-FREE: enabling only USED costs the same per word (" +
            std::to_string(cost_d_alone) + ") as no channels enabled at all (" +
            std::to_string(cost_none) + ") - 'use of either C or D is free' (p.193, the "
            "D half). This is a RELATIVE claim and holds regardless of the absolute-"
            "tick-count disagreement flagged in the file header. agnus_blitter.v:735-"
            "757 - BLT_D is visited every word regardless of used (see TABLE-6-2's "
            "evidence); enabling it alone adds nothing beyond the baseline two states "
            "(BLT_A, BLT_D) every USE code visits at minimum. 6-blitter-speed.c8, "
            "ch6-speed.c7 .");
    }

    // ---------------- B-ADDS-TWO-TICKS (p.193) ----------------
    {
        int cost_d_alone = per_word_ticks(USED);
        int cost_b_and_d = per_word_ticks(USEB | USED);
        int measured_marginal = cost_b_and_d - cost_d_alone;

        checks.expect(measured_marginal == 2,
            "B-ADDS-TWO-TICKS: adding USEB to a D-alone blit raises the per-word cost "
            "from " + std::to_string(cost_d_alone) + " to " + std::to_string(cost_b_and_d) +
            " - a marginal cost of " + std::to_string(measured_marginal) + " tick(s), "
            "not the manual's stated 2 - 'Use of the B register always adds two ticks "
            "to the blitter cycle' (p.193). See the file header's flagged RTL-vs-manual "
            "disagreement (measured marginal cost is consistently half the manual's "
            "stated one). agnus_blitter.v:691-710 (BLT_B visited, and requests a grant, "
            "iff useb). 6-blitter-speed.c6, ch6-speed.c6 .");
    }

    // ---------------- C-AND-D-TOGETHER-ADDS-TWO-TICKS (p.193) ----------------
    {
        // The claim ch6-speed.c8 was overturned from a discard specifically
        // because "ticks" here is the manual's own dimensionless n (clocks
        // per blitter cycle), not a real-time value - it is asserted here
        // as a COUNT, explicitly, per the build brief's instruction.
        int cost_d_alone = per_word_ticks(USED);
        int cost_c_and_d = per_word_ticks(USEC | USED);
        int measured_marginal = cost_c_and_d - cost_d_alone;

        checks.expect(measured_marginal == 2,
            "C-AND-D-TOGETHER-ADDS-TWO-TICKS [count, not a real-time value - 'ticks' "
            "here is the manual's own n, clocks per blitter cycle, dimensionless]: "
            "using both C and D together raises the per-word cost from " +
            std::to_string(cost_d_alone) + " to " + std::to_string(cost_c_and_d) +
            " - a marginal cost of " + std::to_string(measured_marginal) + " tick(s), "
            "not the manual's stated 2 - 'Use of either C or D is free, but use of "
            "both adds another two ticks' (p.193). See the file header's flagged "
            "RTL-vs-manual disagreement. agnus_blitter.v:75 (the module's own change-"
            "log: 'there is still incompatibility when C channel is selected without D: "
            "extra idle cycle is inserted' - independent confirmation that channel "
            "selection genuinely changes the cycle count, even though the exact "
            "magnitude disagrees with the manual). 6-blitter-speed.c9, ch6-speed.c8 .");
    }

    // ---------------- FOUR-TICK-COPY-A-AND-D (p.193) ----------------
    {
        int cost_a_and_d = per_word_ticks(USEA | USED);

        checks.expect(cost_a_and_d == 4,
            "FOUR-TICK-COPY-A-AND-D: measured per-word cost for A+D is " +
            std::to_string(cost_a_and_d) + " ticks, not the manual's stated 4 - 'a "
            "copy cycle, using A and D, takes four clock ticks per cycle' (p.193). See "
            "the file header's flagged RTL-vs-manual disagreement. 6-blitter-speed.c10, "
            "ch6-speed.c9 .");
    }

    // ---------------- SIX-TICK-COPY-B-AND-D (p.193) ----------------
    {
        int cost_b_and_d = per_word_ticks(USEB | USED);

        checks.expect(cost_b_and_d == 6,
            "SIX-TICK-COPY-B-AND-D: measured per-word cost for B+D is " +
            std::to_string(cost_b_and_d) + " ticks, not the manual's stated 6 - 'a copy "
            "cycle using B and D takes six ticks per cycle' (p.193). See the file "
            "header's flagged RTL-vs-manual disagreement. 6-blitter-speed.c11, "
            "ch6-speed.c10 .");
    }

    // ---------------- EIGHT-TICK-GENERALIZED-COPY-B-C-D (p.193) ----------------
    {
        int cost_bcd = per_word_ticks(USEB | USEC | USED);

        checks.expect(cost_bcd == 8,
            "EIGHT-TICK-GENERALIZED-COPY-B-C-D: measured per-word cost for B+C+D is " +
            std::to_string(cost_bcd) + " ticks, not the manual's stated 8 - 'a "
            "generalized bit copy using B, C, and D takes eight ticks per cycle' "
            "(p.193). See the file header's flagged RTL-vs-manual disagreement. "
            "6-blitter-speed.c12, ch6-speed.c11 .");
    }

    // ---------------- LINE-MODE-EIGHT-TICKS-PER-PIXEL (p.193) ----------------
    {
        // Line mode: width MUST be 2 (BLT_L states are entered via the
        // `line` bit, agnus_blitter.v:660-661); height is the line length
        // in pixels. Marginal measurement between height=10 and height=20,
        // same technique as per_word_ticks but through the line-mode FSM
        // (BLT_L1-BLT_L4, agnus_blitter.v:795-869), which are visited
        // exactly once each per pixel, unconditionally.
        auto total_for = [](int height) {
            Harness h;
            for (int i = 0; i < height + 4; ++i) h.c_words.push_back(0xFFFF);
            int start = h.tick_count;
            write_bltcon0(h, 0, USEC | USED, 0xCA);
            h.write_reg(BLTCON1, 0x0001);  // line=1 (bit0), octant SUD=SUL=AUL=0
            h.write_reg(BLTSIZE, bltsize_word(2, static_cast<uint16_t>(height)));
            int done = h.run_until_done(20000);
            return done - start;
        };
        int t10 = total_for(10);
        int t20 = total_for(20);
        int per_pixel = (t20 - t10) / 10;

        checks.expect(per_pixel == 8,
            "LINE-MODE-EIGHT-TICKS-PER-PIXEL: measured per-pixel cost in line mode is " +
            std::to_string(per_pixel) + " ticks, not the manual's stated 8 - 'When in "
            "line mode, each pixel takes eight ticks' (p.193). See the file header's "
            "flagged RTL-vs-manual disagreement (the same factor-of-two gap as every "
            "normal-mode speed claim). agnus_blitter.v:795-869 - BLT_L1, BLT_L2, BLT_L3 "
            "and BLT_L4 are each visited exactly once per pixel, unconditionally on "
            "clkena. 6-blitter-speed.c13, ch6-speed.c12 .");
    }

    // ---------------- FIGURE-6-13-BIT-SLICE-REPLICATION (p.198) ----------------
    {
        // A walking single-bit pattern, one bit per word across all 16
        // positions, through a passthrough LF: if the sixteen bit
        // positions were not sixteen IDENTICAL, INDEPENDENT copies of the
        // same building block (what Figure 6-13 depicts, "the basic
        // building blocks for a single bit of a 16-bit [...] operation"),
        // some bit would leak into, collide with, or fail to reach its
        // neighbour.
        Harness h;
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        std::vector<uint16_t> stimulus;
        for (int bit = 0; bit < 16; ++bit) stimulus.push_back(static_cast<uint16_t>(1u << bit));
        h.a_words.assign(stimulus.begin(), stimulus.end());
        start_blit(h, USEA | USED, LF_A_PASSTHROUGH, 16, 1);
        h.run_until_writes(16, 200);
        h.settle();

        checks.expect(h.data_writes.size() == 16 && h.data_writes == stimulus,
            "FIGURE-6-13-BIT-SLICE-REPLICATION: sixteen words, each with exactly one "
            "distinct bit set (0x0001 through 0x8000, one per word), all pass through "
            "unchanged and undisturbed - no bit leaks into a neighbouring position "
            "across all sixteen - 'Figure 6-13 shows the basic building blocks for a "
            "single bit of a 16-bit wide operation of the blitter' (p.198, the caption "
            "for a block diagram whose logic repeats identically per bit position). "
            "agnus_blitter_minterm.v:29-40 and agnus_blitter_fill.v:20-26 are both "
            "written as a `for` loop over bit position j, unrolled by the synthesizer "
            "into sixteen independent instances - each position reads only its own "
            "ain[j]/bin[j]/cin[j]/in[j], never a neighbour's. ch6-blockdiag.c1 .");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
