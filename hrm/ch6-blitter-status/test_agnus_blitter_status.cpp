// Test purpose: Verify the blitter's three status signals at the
//   agnus_blitter boundary - busy (set on the BLTSIZE write, cleared when
//   the blit finishes), zero/BZERO (set at init, cleared by the first
//   destination word with any bit set, never restored, computed even with
//   the D channel disabled, and valid only after the blit has completed) and
//   int3 (pulses once when the blit finishes).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 171, 186, 187 and 198.
// Non-goals: pre-Fat-Agnus busy timing ("gets its first DMA cycle") - the
//   chip generation is not a module input, so only the Fat-Agnus behaviour
//   is constructible here; whichever behaviour is measured this design
//   implements, the other is by construction not present (reported, not
//   asserted). DMACONR bit positions (DMAF_BLTDONE bit 14, DMAF_BLTNZERO bit
//   13) - that mapping needs the agnus top level / register bus, outside
//   this module's boundary; PKG-AGNUS-DMACON's territory. Minterm/fill
//   correctness, line mode, masking, address generation, ECS BLTSIZH/BLTSIZV
//   sizing - covered by their own packages (PKG-BLT-MINTERM, PKG-BLT-FILL,
//   PKG-BLT-LINE, PKG-BLT-MASKS, PKG-BLT-ADRGEN, PKG-BLT-ECS); this package
//   uses only OCS-style BLTSIZE sizing and the simplest minterm settings
//   (C-passthrough, plain AND) as plumbing to get words through the
//   pipeline, not as claims of their own. Exact fetch/store pipeline latency
//   is measured tooling, not itself an asserted claim.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the printed pages independently, and an id names
//   one reader's reading of one sentence. They resolve in the chapter's
//   claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// Supersedes hrm/agnus_blitter_zero, per the build register. That package
// compiles a local copy of agnus_blitter.v:481-487 pasted into its own .v file
// (see docs/evidence/blitter-prior-art.md, "Defect 1") - a change to agnus_blitter.v
// cannot make it go red. It is NOT deleted here: it stays in the tree, unchanged
// and still green, until this package is confirmed green in its place.
//
// CLAIM COVERAGE, for the three claims of the 28 with no assertion of their
// own (reported here, not silently dropped):
//   ch6-blockdiag.c11 (p.198, "Not shown on this diagram is the logic for
//     zero detection") states that a figure omits something, not a hardware
//     behaviour - it is preamble to the paragraph c12/ch6-blockdiag.c12 and
//     c13/ch6-blockdiag.c13 actually state, and its testable content is
//     exactly theirs (asserted as Z-D-CLEAR and Z-CLEAR/Z-NEVER-RESTORED
//     below).
//   6-blitter-done-flag.c7, 6-blitter-done-flag.c13, ch6-done.c14 (pp.186-187,
//     the pre-Fat-Agnus
//     busy timing: "gets its first DMA cycle") are NOT constructible at this
//     boundary - chip generation is not a module input, so there is no
//     stimulus that selects the pre-Fat-Agnus alternative. BUSY-NO-DMA-NEEDED
//     below measures the Fat-Agnus behaviour this design actually implements
//     and, by the same measurement, shows the pre-Fat-Agnus alternative is not
//     what is present.
//   ch6-done.c15 (p.187, "not all machines use the newer chips") is prose
//     about historical deployment, not a hardware behaviour with any signal
//     to name - not asserted, and not constructible.
//
// PROPOSITIONS ROUTED 2026-09-25 (work/2026-09-25_unit-backlog), spec-stage
// only -- quote, restatement and page, RTL not opened for these yet:
//   ch6-blockdiag.c11 (p.198) quote "Not shown on this diagram is the logic
//     for zero detection, which" -- restated: the logic for zero detection
//     is not shown on Figure 6-13.
//   6-blitter-done-flag.c4 (p.186) quote "A blitter done flag, also called
//     the blitter busy flag, is provided as" -- restated: the blitter-done
//     flag is also called the blitter-busy flag.
//   6-blitter-done-flag.c7 (p.186) quote "access because of, for instance,
//     display fetches, this bit may not" -- restated: on pre-Fat-Agnus
//     chips, a just-started blit that has not received memory access may
//     not yet assert busy; p.187 states Fat Agnus fixed this
//     generation-dependent behavior.
//   ch6-done.c4 (p.186) quote "A blitter done flag, also called the blitter
//     busy flag, is provided as" -- restated: the blitter done flag is also
//     called the blitter busy flag.
//   ch6-done.c7 (p.186) quote "DMAF_BLTDONE (bit 14) in DMACONR. This flag
//     is set when a blit is in" -- restated: the blitter done flag is set
//     when a blit is in progress.
//   6-blitter-done-flag.c13 (p.187) quote "when the blitter gets its first
//     DMA cycle. However, not all machines" -- restated: on pre-Fat-Agnus
//     chips, the busy bit becomes set when the blitter receives its first
//     DMA cycle.
//   ch6-done.c14 (p.187) quote "when the blitter gets its first DMA cycle.
//     However, not all machines" -- restated: before the Fat Agnus the
//     blitter busy bit was set when the blitter got its first DMA cycle.
//   ch6-done.c15 (p.187) quote "when the blitter gets its first DMA cycle.
//     However, not all machines" -- restated: not all machines use the
//     newer chips in which the busy bit is fixed.
// These eight are the same three groups the CLAIM COVERAGE note above
// already names by id (blockdiag.c11; the pre-Fat-Agnus trio c7/c13/c14;
// c15) plus the nomenclature pair (6-blitter-done-flag.c4/ch6-done.c4) and
// the in-progress restatement (ch6-done.c7) that the BUSY section's comment
// below discusses but had not put in any printed line. Grading vs recording
// each is decided after <package>.jsonl and the RTL are opened, per Prompt
// 7's order of work; see the RECORDED lines and BUSY-MIDBLIT's label below
// for the disposition actually reached.
//
// FIXTURE NOTE FOR THE NEXT NINE agnus_blitter PACKAGES.
//   The starting harness salvaged into scratchpad/salvaged-harness/diag.cpp had
//   correct busy/int3/we timing but never actually delivered data: every
//   data_out at every we came back 0. The cause, found by diffing against the
//   known-good hrm/ch2-copper-waitskip-range harness: on a DMA grant, that
//   harness sets ackdma/reg_address_in/data_in and then calls dut->eval() AGAIN
//   before toggling the clock. The salvaged diag.cpp skipped that second eval()
//   - it set the grant signals and went straight to `clk=1; eval();`. The fix
//   below (see Harness::cycle) restores that second eval(). TEST-DATA-DELIVERY
//   below is the assertion that proves the fix actually works - it must stay
//   green for any data-dependent assertion in this file (or a descendant
//   fixture) to mean anything.
//
//   Second trap, found while building this package: BLTAFWM/BLTALWM (the A
//   channel's first/last-word masks) default to 0 after reset. bltamask uses
//   them whenever first_word or last_word is true (agnus_blitter.v:311-319), so
//   any test that drives the A channel and does not explicitly write
//   BLTAFWM=BLTALWM=0xFFFF will see channel A silently masked to 0 on its first
//   and/or last word - for a 1-word blit, on its only word. This package avoids
//   the trap everywhere except the AND-overlap test, which writes both masks
//   before starting.
//
//   Third: zero's register update lags behind busy/done/int3 by however many
//   cycles the D-hold-register pipeline still has in flight. zero is driven by
//   `store_result`, which is `enable && next_word` registered one cycle, so a
//   destination word can still be clearing (or not clearing) zero one or more
//   cycles after busy has already fallen and int3 has already pulsed - see the
//   "Fourth" trap below, where the true last word's store trails busy/int3 by
//   two cycles for a plain multi-word blit. Every "sample zero after
//   completion" in this file runs a 10-cycle margin past the busy-falls edge
//   before reading zero, for exactly this reason.
//
//   Fourth: busy/int3 signal "the blit is done" before the pipeline has
//   actually drained the LAST word to memory. `done` (agnus_blitter.v:905) is
//   true while the main per-word loop is fetching the LAST source word (last_
//   word&&last_line, checked in BLT_D or BLT_C), but because the D-channel
//   store is itself one iteration behind the fetch, that same cycle is storing
//   the SECOND-TO-LAST word, not the last one. The true last word is flushed
//   afterwards, through the extra BLT_E/BLT_F states, one to two cycles after
//   busy has already fallen and int3 has already pulsed. Measured on a 4-word
//   C-passthrough blit: the four D-channel writes land at ticks 11, 14, 17 and
//   19; int3 pulses and busy falls at tick 17 - i.e. the fourth (truly last)
//   word is written 2 cycles AFTER completion is signalled. This package's
//   assertions sample busy/int3/zero either well before this final flush (mid-
//   blit) or with the 10-cycle settle margin after it, so it never depends on
//   exactly when within that trailing window the last store lands - but a
//   later package that wants to assert something about the LAST word's value
//   should not assume it is present yet at the tick busy/int3 report done.
//
//   Fifth: int3 (`assign int3 = done`) is combinational off the FSM's CURRENT
//   state, and that is also the value the busy register's non-blocking update
//   captures on this same clock edge - so int3 must be sampled BEFORE this
//   tick's own edge to land on the same tick index as the busy-registered
//   fall it causes. Sampled after the edge (the natural thing to do, and what
//   an early draft of this harness did), int3 reads one tick early relative to
//   when busy is observed to fall, because by then the FSM has already moved
//   past the state that made `done` true. See Harness::cycle's `last_int3`.
//
// Every mutation below was actually applied (edit -> rebuild -> run -> revert
// -> rebuild-clean) and the FAIL/PASS lines quoted are what the run printed,
// not predictions. Two were wrong on the first attempt and are recorded that
// way: BUSY-NO-DMA-NEEDED stayed green under BUSY-SET's mutation (a first
// draft of this comment claimed it would also fail - it does not, and that is
// the correct shape, not a defect); Z-CLEAR's first version passed under the
// Z-FINAL mutation for the wrong reason (see below) and was fixed before being
// counted as covered.
//
// MUTATION TEST-DATA-DELIVERY stimulus red: fed C word at index 2 changed from
//   0x9ABC to 0x9ABD (expected constants left untouched) -> FAIL, the third
//   captured data_out no longer matches; every other assertion PASS.
// MUTATION BUSY-SET stimulus red: the register written to start the blit
//   changed from BLTSIZE to BLTCON1 (same data, same timing) -> BUSY-SET and
//   BUSY-RELEASES FAIL (the blit never starts, busy never rises, so it also
//   never has anything to release). BUSY-NO-DMA-NEEDED stays PASS - it only
//   asserts "no DMA cycle occurred before busy was observed set", and with
//   busy never set that is vacuously still true; it is not a counterexample to
//   this mutation, just not the assertion this particular stimulus speaks to.
// MUTATION BUSY-MIDBLIT stimulus red: width shortened from 4 to 1 (fixture:
//   the assertion block covering BUSY-MIDBLIT, INT3-ON-COMPLETION and
//   BUSY-FLAG-ON-COMPLETION). All three FAIL: with only one word requested,
//   the fixture's "wait for the second of four stores, then sample" no longer
//   describes a point mid-blit - by the time a second store is observed
//   (whatever it is, for a 1-word blit) the blit has already run to
//   completion and back to idle, so busy reads 0 where BUSY-MIDBLIT expects 1,
//   and the subsequent completion search finds a stale/absent transition.
// MUTATION INT3-ON-COMPLETION stimulus red: width raised from 4 to 64 in the same
//   fixture as above, run budgets left at 60 cycles -> BUSY-MIDBLIT stays
//   PASS (2 of 64 words store well inside 60 cycles, so busy is genuinely
//   still 1 there) while INT3-ON-COMPLETION and BUSY-FLAG-ON-COMPLETION FAIL
//   (the blit cannot finish in 60 cycles, so no completion tick is found) -
//   a different mechanism than BUSY-MIDBLIT's mutation, confirming the
//   completion checks fail because completion itself is missing, not because
//   the wait logic broke.
// MUTATION Z-CTRL stimulus red: word index 1 (originally 0x0000) changed to
//   0x0001 -> only Z-CTRL FAILs (the mid-blit sample now finds zero already
//   cleared); Z0, Z-CLEAR and Z-FINAL stay PASS (correct shape - only the
//   claim about the mid-blit sample point depends on that particular word).
// MUTATION Z-FINAL stimulus red: the trailing nonzero word (0x8000) changed to
//   0x0000, making the whole run all-zero -> Z-FINAL and Z-CLEAR both FAIL
//   (zero never clears, so there is no clearing tick to find and the
//   post-completion sample stays 1); Z0 and Z-CTRL stay PASS (an all-zero run
//   still starts at 1 and is still 1 mid-blit). Z-CLEAR only failed here after
//   a fixture bug found BY this mutation was fixed first: its original search
//   for "the first tick zero reads 0" scanned the WHOLE log, including the
//   few cycles before the blit's own BLT_INIT sets zero to 1 for the first
//   time (zero has no reset branch and defaults to 0) - so it was finding
//   that pre-init tick and passing regardless of whether a real clearing word
//   was ever stored. Restricting the search to ticks after Z0's "zero is now
//   1" point (zero_set_tick) fixed it; re-run, this mutation then failed
//   Z-CLEAR as it should.
// MUTATION Z-NEVER-RESTORED stimulus red: the middle (nonzero) word changed
//   from 0x0001 to 0x0000, making the whole 3-word run all-zero -> both the
//   "[setup] after the second word zero has cleared" check and
//   Z-NEVER-RESTORED itself FAIL (zero is never cleared in the first place,
//   so there is nothing for the third word to fail to restore).
// MUTATION Z-D-PRECOND stimulus red: `used` added back (D channel enabled)
//   with everything else held fixed -> Z-D-PRECOND FAILs (`we` fires once);
//   Z-D-CLEAR stays PASS (correct shape - the underlying clearing behaviour
//   does not depend on D being enabled, only the "was it written" precondition
//   does).
// MUTATION Z-D-CLEAR stimulus red: the fed C word changed from 0x4321 to
//   0x0000 while `used` stayed 0 -> Z-D-CLEAR FAILs (zero stays 1);
//   Z-D-PRECOND stays PASS (`we` still correctly never fires).
// MUTATION Z-AND-RESULT stimulus red: b_words[1] changed from 0x2222 to 0x1111 (now
//   equal to a_words[1]), so the second word's AND is 0x1111, not 0 -> both
//   Z-AND-RESULT and Z-AND-STAYS FAIL as expected.
//
// SEVEN MORE, added after the gate's mutation check pointed out these seven
// assertions were only ever discussed inside another assertion's record above,
// never carrying one of their own under their own id. Each was re-run to get
// its own line; none of the ten records above were altered.
//
// MUTATION BUSY-RELEASES stimulus red: same edit as BUSY-SET above (the
//   register written to start the blit changed from BLTSIZE to BLTCON1, same
//   data, same timing) -> BUSY-RELEASES FAILs too: busy is never set in the
//   first place, so run_until_done(200) never sees it fall and returns -1 even
//   after enadma is restored. (BUSY-SET FAILs for the same root cause;
//   BUSY-NO-DMA-NEEDED stays PASS, vacuously, as already noted above.)
// MUTATION BUSY-FLAG-ON-COMPLETION stimulus red: same edit as BUSY-MIDBLIT
//   above (width shortened from 4 to 1 in that fixture) -> FAILs: the
//   completion search finds a stale/absent transition, done_at stays -1, so
//   "busy reads 0 after that same completion tick" has no real tick to check.
// MUTATION INT3-ON-COMPLETION stimulus red: same edit as BUSY-MIDBLIT above
//   (width shortened from 4 to 1) -> FAILs for the same reason as
//   BUSY-FLAG-ON-COMPLETION just above: done_at stays -1, so
//   "int3_tick == done_at" cannot hold.
// MUTATION Z-CLEAR stimulus red: same edit as Z-FINAL above (the trailing
//   nonzero word changed from 0x8000 to 0x0000, making the whole run
//   all-zero) -> Z-CLEAR FAILs alongside Z-FINAL: with nothing ever clearing
//   zero, the search for a post-zero_set_tick clearing tick finds none and
//   clear_tick stays -1.
// MUTATION Z0 stimulus red: enadma held low (h.set_enadma(false), the same
//   device BUSY-SET's block uses) added across the whole Z0/Z-CTRL/Z-CLEAR/
//   Z-FINAL fixture -> enable (= enadma & clkena) stays 0, the FSM cannot
//   leave BLT_IDLE, so BLT_INIT - the only state that sets zero <= 1
//   (agnus_blitter.v:483-484, gated on enable && init) - is never entered;
//   zero never becomes 1 within the 20-cycle budget, so run_until_zero_becomes
//   returns false and Z0 FAILs (zero_set is false). Z-CTRL and Z-CLEAR FAIL
//   too, same root cause. Z-FINAL stays PASS, but for the wrong reason: zero
//   defaults to 0 and, with BLT_INIT never reached, was simply never touched,
//   so "!zero()" reads true by accident rather than by the blit having
//   cleared it - not a defect in Z-FINAL, which already has its own
//   discriminating mutation above, just a reminder that this stimulus is not
//   the one that exercises it.
// MUTATION BUSY-NO-DMA-NEEDED stimulus red: enadma left true (the
//   h.set_enadma(false) call removed) across the BLTSIZE write. FIRST
//   ATTEMPT, wait left at h.run_cycles(1): MISS, stayed PASS (green) - one
//   cycle is not enough for the FSM to reach a DMA-requesting state (BLT_INIT
//   has no dma_req; BLT_A's dma_req is `usea`, and this fixture never sets
//   USEA), so no request had appeared yet at the point the check runs.
//   REPLACEMENT, same enadma change with the wait widened from 1 to 20
//   cycles: FAILs - reqdma is seen and the single word is actually delivered
//   before the check runs. Collateral in the replacement run: BUSY-SET and
//   BUSY-RELEASES FAIL too, because with 20 cycles and enadma on throughout,
//   this 1-word blit runs to completion and busy has already fallen back to 0
//   by the time BUSY-SET samples it - not a counterexample to their own
//   (separately recorded, narrower) mutations above, just this wider window
//   doing more than the one thing it was aimed at.
// MUTATION Z-AND-STAYS stimulus red: same edit as the "Z-AND-RESULT" record above
//   (b_words[1] changed from 0x2222 to 0x1111) -> Z-AND-STAYS FAILs alongside
//   Z-AND-RESULT. The existing record above used the id "Z-AND", which is not
//   the same token as either "Z-AND-RESULT" or "Z-AND-STAYS"; this line
//   closes that gap for Z-AND-STAYS under its own actual id.

#include <cstdint>
#include <deque>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vagnus_blitter.h"

namespace {

constexpr uint16_t to_reg(uint16_t byte_address) { return static_cast<uint16_t>(byte_address >> 1); }

constexpr uint16_t BLTCON0 = to_reg(0x040);
constexpr uint16_t BLTCON1 = to_reg(0x042);
constexpr uint16_t BLTAFWM = to_reg(0x044);
constexpr uint16_t BLTALWM = to_reg(0x046);
constexpr uint16_t BLTADAT = to_reg(0x074);
constexpr uint16_t BLTBDAT = to_reg(0x072);
constexpr uint16_t BLTCDAT = to_reg(0x070);
constexpr uint16_t BLTSIZE = to_reg(0x058);
constexpr uint16_t IDLE_REG = 0x0FF;

// BLTCON0 use-channel bits, per agnus_blitter.v:252
// {usea,useb,usec,used} = bltcon0[11:8] (with used further gated by an
// internal 'enad' register that becomes 1 once the blit is running).
constexpr uint16_t USEA = 1u << 11;
constexpr uint16_t USEB = 1u << 10;
constexpr uint16_t USEC = 1u << 9;
constexpr uint16_t USED = 1u << 8;

// Minterm LF settings (agnus_blitter_minterm.v): bit k of LF selects the
// (A,B,C) combination whose binary value is k, MSB-first as (A,B,C).
constexpr uint16_t LF_C_PASSTHROUGH = 0xAA;  // D = C, for every A/B value
constexpr uint16_t LF_A_AND_B = 0xC0;        // D = A & B, independent of C

struct Sample {
    int tick;
    bool busy;
    bool zero;
    bool int3;
    bool we;
    uint16_t data_out;
    bool reqdma;
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

    // Runs until `data_writes.size()` reaches `n`, or `max_ticks` is exhausted.
    // Returns true if the target was reached.
    bool run_until_writes(size_t n, int max_ticks = 500) {
        for (int i = 0; i < max_ticks && data_writes.size() < n; ++i) tick();
        return data_writes.size() >= n;
    }

    // Runs until zero is observed to equal `target`, or `max_ticks` is
    // exhausted. Returns true if reached.
    bool run_until_zero_becomes(bool target, int max_ticks = 100) {
        for (int i = 0; i < max_ticks; ++i) {
            tick();
            if (log.back().zero == target) return true;
        }
        return false;
    }

    // Runs until busy is observed to fall from 1 to 0 (a completed blit), or
    // `max_ticks` is exhausted. Returns the tick index of the fall, or -1.
    int run_until_done(int max_ticks = 500) {
        bool seen_busy = false;
        for (int i = 0; i < max_ticks; ++i) {
            tick();
            const Sample& s = log.back();
            if (s.busy) seen_busy = true;
            if (seen_busy && !s.busy) return s.tick;
        }
        return -1;
    }

    // Idle margin: zero's register update lags one clock behind busy/done (see
    // file header, "Third"). Run a few idle cycles before sampling zero after
    // completion.
    void settle(int n = 10) { run_cycles(n); }

    bool zero() const { return dut->zero; }
    bool busy() const { return dut->busy; }
    bool int3() const { return dut->int3; }

    std::deque<uint16_t> a_words, b_words, c_words;
    std::vector<Sample> log;
    std::vector<uint16_t> data_writes;
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

    void tick_bookkeeping() {
        Sample s{tick_count++, static_cast<bool>(dut->busy), static_cast<bool>(dut->zero),
                  last_int3, last_we, last_data_out, static_cast<bool>(dut->reqdma)};
        log.push_back(s);
        if (s.we) data_writes.push_back(s.data_out);
    }

    void tick() {
        cycle();
        tick_bookkeeping();
    }

    void cycle(bool cpu_write = false, uint16_t cpu_reg = IDLE_REG, uint16_t cpu_data = 0) {
        dut->eval();
        // int3 (=done, agnus_blitter.v:507) is purely combinational off the
        // CURRENT (pre-edge) FSM state, and that same pre-edge value of `done`
        // is what the busy register's non-blocking update captures at the
        // clock edge below. Sampling int3 here - before this tick's own edge -
        // is what makes it line up with `busy` (a registered output, correctly
        // read post-edge) at the SAME tick index. Sampling int3 after the edge
        // instead reads the NEXT state's `done`, which has already gone back
        // to 0 (the FSM has moved on) - that reads one tick late and never
        // lines up with the busy-falls tick. Measured, not assumed: an earlier
        // draft of this harness sampled post-edge and got int3 one tick ahead
        // of the busy-falls tick every time.
        last_int3 = dut->int3;
        last_we = false;
        last_data_out = 0;
        const bool granted = !cpu_write && dut->reqdma;
        if (cpu_write) {
            dut->ackdma = 0;
            dut->reg_address_in = cpu_reg;
            dut->data_in = cpu_data;
        } else if (granted) {
            dut->ackdma = 1;
            const uint16_t reg = dut->reg_address_out;
            uint16_t v = 0;
            if (reg == BLTADAT) v = pop_or_zero(a_words);
            else if (reg == BLTBDAT) v = pop_or_zero(b_words);
            else if (reg == BLTCDAT) v = pop_or_zero(c_words);
            // BLTDDAT (channel D) is the blitter writing OUT to the bus; there
            // is nothing to feed back for it.
            dut->reg_address_in = reg;
            dut->data_in = v;
            // Settle ackdma/reg_address_in/data_in against the combinational
            // network BEFORE the clock edge. This is the step the salvaged
            // diag.cpp harness skipped (see file header) - without it, every
            // data_out at every we reads 0.
            dut->eval();
            // `we`/`data_out` are combinational off ackdma&&chsel==CHD, and
            // chsel changes on the VERY SAME edge that completes this grant
            // whenever this happens to be the last cycle of a channel state
            // (which it always is here - each channel state lasts exactly one
            // cycle). Sampled after the edge below, chsel has already moved on
            // and `we` reads 0 again even though the write genuinely happened.
            // So capture it HERE, at the only moment it is valid - matching
            // how a real synchronous memory would sample write-enable/data
            // (settled ahead of the clock edge it is written on).
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

void start_blit(Harness& h, uint16_t bltcon0, uint16_t width, uint16_t height) {
    h.write_reg(BLTCON0, bltcon0);
    h.write_reg(BLTCON1, 0x0000);
    h.write_reg(BLTSIZE, bltsize_word(width, height));
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- TEST-DATA-DELIVERY (fixture self-check, no page) ----------------
    //
    // Before any assertion about zero/busy content can mean anything, the
    // fixture has to prove it can actually deliver a known word from the C
    // channel through to a D-channel memory write. Four distinct, non-trivial
    // words, C-passthrough (LF=0xAA), D enabled.
    {
        Harness h;
        h.c_words = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        start_blit(h, USEC | USED | LF_C_PASSTHROUGH, 4, 1);
        h.run_until_writes(4, 60);
        h.settle();

        const std::vector<uint16_t> expected = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        checks.expect(h.data_writes == expected,
            "[fixture check] TEST-DATA-DELIVERY: four distinct C words fed into the "
            "fixture arrive at data_out, in order, at the we cycles they were fetched on "
            "- proves the DMA grant handshake actually delivers data, which every "
            "data-dependent assertion below depends on");
    }

    // ---------------- BUSY / done / int3 (pp. 186-187, 171) ----------------
    //
    // busy is set on the BLTSIZE write and cleared when the blit finishes
    // (agnus_blitter.v:496-504). "Also called the blitter busy flag"
    // (6-blitter-done-flag.c4, ch6-done.c4, p.186) and "This flag is set when a
    // blit is in progress" (ch6-done.c7, p.186) are nomenclature/definition for
    // exactly this signal - not independently assertable (naming is not RTL
    // behaviour), covered by the busy assertions below rather than asserted on
    // their own.
    {
        // BUSY-SET / BUSY-NO-DMA-NEEDED / BUSY-RELEASES: hold enadma low across
        // the BLTSIZE write, so `enable` (= enadma & clkena) stays 0 and the FSM
        // cannot leave BLT_IDLE, let alone request or receive a DMA cycle
        // (agnus_blitter.v:605-606, 638-644). busy is nonetheless set the very
        // next clock, because agnus_blitter.v:500-501 sets it purely off
        // reg_address_in matching BLTSIZE, with no `enable` gate at all.
        Harness h;
        h.c_words = {0xABCD};
        h.set_enadma(false);
        h.write_reg(BLTCON0, USEC | USED | LF_C_PASSTHROUGH);
        h.write_reg(BLTCON1, 0x0000);
        h.write_reg(BLTSIZE, bltsize_word(1, 1));
        h.run_cycles(1);

        checks.expect(h.busy(),
            "BUSY-SET: busy reads 1 on the cycle immediately after the BLTSIZE write "
            "(6-blitter-done-flag.c11, 6-blitter-done-flag.c12, ch6-done.c13, p.187, "
            "'Starting with the Fat "
            "Agnus the blitter busy bit has been \"fixed\" to be set as soon as you write "
            "to BLTSIZE to start the blit rather than ... when the blitter gets its "
            "first DMA cycle')");

        bool any_reqdma_seen = false;
        for (const auto& s : h.log) if (s.reqdma) any_reqdma_seen = true;
        checks.expect(!any_reqdma_seen && h.data_writes.empty(),
            "BUSY-NO-DMA-NEEDED: busy became 1 while zero DMA cycles have been "
            "requested or granted to the blitter, so this design's busy bit does not "
            "wait for 'its first DMA cycle' the way pre-Fat-Agnus chips did (p.187) - "
            "the pre-Fat-Agnus alternative on the same page is therefore not what this "
            "design implements, and is not separately constructible here since chip "
            "generation is not a module input");

        // Control: release enadma and let the same blit actually run to
        // completion, so BUSY-SET above is not simply describing a stuck bit.
        h.set_enadma(true);
        const int done_at = h.run_until_done(200);
        checks.expect(done_at >= 0,
            "BUSY-RELEASES [control]: once enadma is released the same blit completes "
            "normally and busy returns to 0, so BUSY-SET above is a real transition and "
            "not a bit that never falls");
    }

    {
        // BUSY-MIDBLIT / INT3-ON-COMPLETION / BUSY-FLAG-ON-COMPLETION: a normal
        // 4-word blit, sampled mid-blit and at completion.
        Harness h;
        h.c_words = {0x0000, 0x0000, 0x0000, 0x8000};
        start_blit(h, USEC | USED | LF_C_PASSTHROUGH, 4, 1);

        // Sample after 2 of 4 words, safely before completion. Completion
        // (done/busy-fall/int3) coincides with the THIRD store, not after the
        // fourth - see the file header's "Fourth" trap: the D-channel store
        // trails the source fetch by one word, so done fires while fetching
        // the last (4th) word, which is the same cycle the 3rd word is stored.
        h.run_until_writes(2, 60);
        checks.expect(h.busy(),
            "BUSY-MIDBLIT: busy still reads 1 after two of four destination words "
            "have been stored - the blitter is 'set while a blit is in progress' "
            "(6-blitter-done-flag.c6, p.186)");

        const int done_at = h.run_until_done(60);
        h.settle();
        int int3_tick = -1;
        for (const auto& s : h.log) if (s.int3) { int3_tick = s.tick; break; }

        checks.expect(done_at >= 0 && int3_tick == done_at,
            "INT3-ON-COMPLETION: int3 pulses on exactly the tick busy falls from 1 to "
            "0 - 'the blitter also has an interrupt flag that is set whenever a blit "
            "[finishes]' (6-interrupt-flag.c1, ch6-int.c1, p.187), corroborated by "
            "p.171's 'and an interrupt' (6-dma-channels.c42, ch6-dma.c39). sol:7-interrupts.c51, opus:7-int.c49, opus:7-fig7-4.c21 .");

        checks.expect(done_at >= 0 && !h.busy(),
            "BUSY-FLAG-ON-COMPLETION: busy reads 0 after that same completion tick - "
            "'when the transfer is complete, the blitter will signal the processor "
            "with a flag' (6-dma-channels.c41, ch6-dma.c38, p.171), the same busy/done "
            "bit described on p.186-187. sol:7-interrupts.c52, opus:7-int.c49 .");
    }

    // ---------------- zero / BZERO (p.187, p.198) ----------------
    {
        // Z0 / Z-CTRL / Z-FINAL / Z-CLEAR: one 4-word blit whose first three
        // words are zero and whose last is not. Z0 is the baseline before any
        // word is stored; Z-CTRL is the deliberate mid-blit control sample the
        // task brief calls for; Z-FINAL is the post-completion sample the
        // sampling-rule claim is actually about; Z-CLEAR checks the clearing
        // tick itself, tying to p.198's "if there are any 1-bits... zero detect
        // is false".
        Harness h;
        h.c_words = {0x0000, 0x0000, 0x0000, 0x8000};

        h.write_reg(BLTCON0, USEC | USED | LF_C_PASSTHROUGH);
        h.write_reg(BLTCON1, 0x0000);
        h.write_reg(BLTSIZE, bltsize_word(4, 1));
        // zero has no reset branch of its own (agnus_blitter.v:480-487) - it
        // only becomes defined once the blit's own BLT_INIT cycle sets it to
        // 1. Wait for exactly that, before any word can have been stored.
        const bool zero_set = h.run_until_zero_becomes(true, 20);
        const int zero_set_tick = h.log.back().tick;
        checks.expect(zero_set && h.data_writes.empty(),
            "Z0 [control] zero reads 1 once the blit has started, before any "
            "destination word has been stored - establishing what 'no destination "
            "word has cleared it yet' looks like, which the mid-blit and "
            "post-completion samples below are contrasted against");

        h.run_until_writes(3, 60);
        checks.expect(h.zero(),
            "Z-CTRL [control]: after three zero words have been stored, zero still "
            "reads 1 - the deliberate mid-blit sample that the post-completion sample "
            "below is contrasted against (p.187, 'the zero flag reports whether the "
            "logic operation ... resulted in zero bits for all destination bits', "
            "6-zero-flag.c1/ch6-zero.c1)");

        // Find the tick the fourth (nonzero) word is stored, and the first tick
        // afterwards zero reads 0.
        h.run_until_writes(4, 60);
        const int fourth_write_tick = h.data_writes.size() == 4 ? h.log.back().tick : -1;
        int clear_tick = -1;
        for (const auto& s : h.log) {
            if (s.tick <= zero_set_tick) continue;  // ignore the pre-init 0 default
            if (!s.zero) { clear_tick = s.tick; break; }
        }
        checks.expect(clear_tick >= 0 && fourth_write_tick >= 0 && clear_tick <= fourth_write_tick + 1,
            "Z-CLEAR: zero drops to 0 within one cycle of the fourth word (which "
            "contains a 1-bit, 0x8000) being stored - 'if there are any 1-bits "
            "generated ... zero detect is false' (ch6-blockdiag.c13, p.198)");

        h.run_until_done(60);
        h.settle();
        checks.expect(!h.zero(),
            "Z-FINAL: sampled after the blit has completed (busy back to 0, with "
            "settling margin), zero reads 0 - 'the Zero flag is only valid after the "
            "blitter has completed its operation' (6-zero-flag.c4/ch6-zero.c4, p.187); "
            "Z-CTRL above is the mid-blit sample that this claim says is NOT "
            "authoritative, which is why it was taken deliberately as the control");
    }

    {
        // Z-NEVER-RESTORED: entailed, not quoted verbatim on any page. p.187
        // says the flag reports "zero bits for all destination bits"
        // (6-zero-flag.c1/ch6-zero.c1) and p.198 says the logic "looks at every
        // bit generated for the destination" (ch6-blockdiag.c12) - together
        // that describes an accumulator that only ever latches false, with no
        // stated (or plausible, given "for ALL destination bits" and "every
        // bit") path back to true within one blit. This is the prior art's
        // most interesting uncited claim (docs/evidence/blitter-prior-art.md); it is
        // asserted here against that entailment, not against a direct quote.
        Harness h;
        h.c_words = {0x0000, 0x0001, 0x0000};
        start_blit(h, USEC | USED | LF_C_PASSTHROUGH, 3, 1);

        h.run_until_writes(2, 60);
        checks.expect(!h.zero(),
            "[setup] after the second (nonzero) word, zero has cleared, as expected "
            "from Z-CLEAR above - the precondition for what follows");

        h.run_until_writes(3, 60);
        h.settle();
        checks.expect(!h.zero(),
            "Z-NEVER-RESTORED [entailed, not a verbatim quote]: a third, zero-valued "
            "word stored after the flag has cleared does not restore it - entailed by "
            "p.187 'zero bits for ALL destination bits' (6-zero-flag.c1/ch6-zero.c1) "
            "together with p.198 'looks at every bit generated for the destination' "
            "(ch6-blockdiag.c12): an accumulator over every generated bit, with no "
            "stated path back to true");
    }

    {
        // Z-D-PRECOND / Z-D-CLEAR: D channel disabled (used=0). The FSM then
        // never visits BLT_D/BLT_F (agnus_blitter.v:724-733), so `we` (which
        // requires chsel==CHD) never fires - genuinely nothing is written to
        // memory. store_result still pulses, because next_word is asserted at
        // BLT_C itself when !used (agnus_blitter.v:892), so the zero flag still
        // tracks the generated (but unwritten) word.
        Harness h;
        h.c_words = {0x4321};
        start_blit(h, USEC | LF_C_PASSTHROUGH, 1, 1);  // USED intentionally omitted
        h.run_cycles(30);
        h.settle();

        bool any_we = false;
        for (const auto& s : h.log) if (s.we) any_we = true;
        checks.expect(!any_we,
            "Z-D-PRECOND: with the D channel disabled, `we` never fires across the "
            "whole run - the precondition for Z-D-CLEAR below, so a green there cannot "
            "be a green-by-accident where D was silently enabled all along");

        checks.expect(!h.zero(),
            "Z-D-CLEAR: zero nonetheless clears to 0 - 'zero detection includes "
            "generated destination bits even when D DMA is disabled' / '...even if "
            "those destination bits are not written due to the D [channel being "
            "disabled]' (6-zero-flag.c2/ch6-zero.c2, p.187)");
    }

    {
        // Z-AND-RESULT / Z-AND-STAYS: the manual's own worked scenario - AND two
        // source images to test for overlap. Two non-overlapping word pairs
        // (0x0F0F/0xF0F0 and 0x1111/0x2222), LF=0xC0 (A AND B, independent of
        // C). BLTAFWM/BLTALWM must be written to 0xFFFF or channel A's first
        // and last word (both words, for this 2-word blit) are silently masked
        // to 0 by agnus_blitter.v:311-319 - see the file header's "Second"
        // trap note.
        Harness h;
        h.a_words = {0x0F0F, 0x1111};
        h.b_words = {0xF0F0, 0x2222};
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        start_blit(h, USEA | USEB | USED | LF_A_AND_B, 2, 1);
        h.run_until_writes(2, 60);
        h.settle();

        const std::vector<uint16_t> expected_and = {0x0000, 0x0000};
        checks.expect(h.data_writes == expected_and,
            "[setup] Z-AND-RESULT: 0x0F0F & 0xF0F0 and 0x1111 & 0x2222 both stored as "
            "0x0000 - confirms the two source words genuinely do not overlap, so "
            "Z-AND-STAYS below is testing a real non-trivial AND and not a "
            "vacuous case");

        checks.expect(h.zero(),
            "Z-AND-STAYS: with two non-overlapping source images ANDed together, zero "
            "stays 1 across the whole blit and after completion - 'to test for "
            "overlap. If the images do not overlap, the zero flag will stay [true]' "
            "(6-zero-flag.c3/ch6-zero.c3, p.187)");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
