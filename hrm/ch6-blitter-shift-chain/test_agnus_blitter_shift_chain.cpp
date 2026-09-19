// Test purpose: Verify the barrel shifter AS WIRED INTO agnus_blitter - what
//   the leaf module agnus_blitter_barrelshifter cannot show because it has
//   no clock and no register (hrm/ch6-blitter-barrelshifter,
//   PKG-BLT-SHIFT, confirmed and unchanged). Specifically: channel A's mask
//   is applied to the source word BEFORE that word enters the shift chain;
//   the first word of the first row of a fresh blit shifts in zeros and no
//   other word does; each later word shifts in the bits shifted OUT of the
//   previous word, including across a row boundary; the B channel's
//   immediate data register (BLTBDAT) is shifted at the moment it is loaded,
//   using whatever shift count is in effect then, and is not reshifted if
//   the shift count changes afterward; and the "shifting is free" claim is
//   measured here strictly as a word/DMA-cycle count, never as a time
//   duration.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 169-171, 179-182, 200.
// Non-goals: the leaf barrel shifter's own combinational correctness across
//   shift/value space - exhaustively swept already by PKG-BLT-SHIFT
//   (hrm/ch6-blitter-barrelshifter, 4,197,376 cases), reused here only as
//   the already-validated hrm_oracle() formula, not re-swept.
//   BLTCON0/BLTCON1 field decode in isolation (which nibble is ash/bsh) -
//   PKG-BLT-BLTCON-FIELDS. Full BLTAFWM/BLTALWM masking semantics beyond the
//   one "mask precedes the chain" ordering fact - PKG-BLT-MASKS (pp.180-182,
//   198-199, 266). Minterm/LF decode correctness - PKG-BLT-MINTERM.
//   Descending mode's left-shift direction (BLTCON1 bit 1, p.182 "Descending
//   Mode") - not exercised; every claim covered here is stated for ascending
//   mode. Line-mode texturing (the `line`-gated branch of bltbhold that
//   replicates a single bit) - PKG-BLT-LINE. ECS BLTCON0L/BLTSIZH/BLTSIZV -
//   PKG-BLT-ECS. No microsecond or wall-clock timing value is measured or
//   asserted anywhere in this file: agnus_blitter carries no clock-frequency
//   port, so any such number would be invented, not observed - p.179's
//   "completely free... no more time" claim and p.171's "extra word" claim
//   are both treated here purely as word/DMA-cycle counts.
//
// PRIOR ART / RECONCILIATION (docs/blitter-prior-art.md and
// work/2026-09-08_fpga-090_build_register.md, both read before writing this
// file, as required). Nothing here supersedes anything: hrm/agnus_blitter_
// barrelshifter compiles agnus_blitter_barrelshifter.v directly (not a local
// copy - none of prior-art's "Defect 1" applies to it) and its 4.2M-case sweep
// stands unchanged and still green. PKG-BLT-SHIFT-CHAIN ADDS to that prior
// art rather than replacing it, per packages.md's reconciliation table and
// the build register's own build note: "PKG-BLT-SHIFT's brief told the
// builder to assert 'shift-on-load of an immediate data register (p.182)'.
// That claim is not in PKG-BLT-SHIFT ... The builder caught it ... and
// declined: the module has no clock and no register, so shift-on-load is not
// constructible there whatever the brief says." That builder was right; this
// package is where shift-on-load, and the rest of the chaining behaviour,
// actually lives, because it needs agnus_blitter's registers (bltaold,
// bltbhold, bltbdat_wrtn) and clock, which the leaf module does not have.
//
// FIXTURE. Started from the working harness in hrm/ch6-blitter-status/ (the
// only agnus_blitter fixture in the tree proven to deliver data correctly),
// not from scratch and not from the salvaged diagnostic. Harness::cycle,
// Harness::tick_bookkeeping and the TEST-DATA-DELIVERY assertion immediately
// below are copied from hrm/ch6-blitter-status/test_agnus_blitter_status.cpp
// essentially verbatim (added: LF_A_PASSTHROUGH and LF_B_PASSTHROUGH constants,
// needed here because this file routes data through A and B, not only C). All
// five timing traps documented in that file's header apply here too and are
// restated briefly against where this file actually hits them:
//   1. `we`/`data_out` are transient, sampled at the grant (Harness::cycle),
//      never polled after the tick.
//   2. BLTAFWM/BLTALWM default to 0 after reset and silently mask channel A's
//      first/last word. Every A-channel scenario below writes both to 0xFFFF
//      before starting, EXCEPT MASK-BEFORE-SHIFT, which deliberately writes
//      BLTAFWM to a partial mask - that is the claim under test there - and
//      still writes BLTALWM=0xFFFF so the *other* mask trap does not also
//      fire and confound the result.
//   3. `zero` is not read anywhere in this file - not needed for any claim
//      here - so its settle-margin trap does not apply, but is noted for
//      completeness.
//   4/5. busy/int3 completion timing is not read anywhere in this file either
//      - every assertion below reads data_out/data_writes, established via
//      run_until_writes, never via busy/int3. The DMA-grant second-eval()
//      fix (trap the whole harness exists to carry forward) is what every
//      assertion in this file depends on; TEST-DATA-DELIVERY proves it still
//      holds in this build.
//
// A second, NEW fixture fact found while building this package, not in the
// status file's list because agnus_blitter_status never exercises the B
// channel's immediate-load path: bltbdat_wrtn (agnus_blitter.v:387-394) is a
// ONE-CYCLE-WIDE pulse, registered true on the cycle immediately AFTER a
// BLTBDAT write, and false again the cycle after that. bltbhold
// (agnus_blitter.v:407-413) captures the shifter output on the clock edge
// where bltbdat_wrtn reads 1 - i.e. one cycle after the write, not on the
// write's own cycle. Writing BLTCON1 (to change bsh) on that exact following
// cycle races the bltbhold capture: bsh's own register update and the
// bltbhold capture happen on the SAME clock edge, so the capture still uses
// the PRE-edge (old) bsh value. This is not a bug to route around - it is
// the mechanism SHIFT-ON-LOAD below is built to demonstrate, and the harness
// exploits it deliberately: two write_reg() calls back-to-back land exactly
// on the write cycle and the following cycle, which is exactly the "load,
// then immediately change the shift count" scenario p.182 describes.
//
// CLAIM COVERAGE. All 35 records of classify.package=="PKG-BLT-SHIFT-CHAIN"
// and classify.status=="testable" in
// work/2026-09-08_fpga-090_classified.jsonl are covered below; several are
// the same proposition stated by both readers (sol/opus) or by the same
// reader in a general and a worked-example form, so they are asserted once
// and every covering id is cited in that assertion's message. No claim in
// this package has zero assertion.
//
// Every mutation below was actually applied (edit -> rebuild -> run -> revert
// -> rebuild-clean) and the FAIL/PASS lines quoted are what the run printed,
// not predictions.
//
// MUTATION TEST-DATA-DELIVERY stimulus red: fed C word at index 2 changed from
//   0x9ABC to 0x9ABD (expected constants left untouched) -> FAIL, the third
//   captured data_out no longer matches; every other assertion PASS.
// MUTATION NO-SHIFT-ON-C stimulus red: the same four words routed through
//   channel A instead of channel C (LF changed from C-passthrough 0xAA to
//   A-passthrough 0xF0, fed via a_words instead of c_words, BLTAFWM/BLTALWM
//   set to 0xFFFF, ash left at 15) -> FAIL: A's shifter is real, so at
//   ash=15 the captured words no longer equal the raw fed words the way
//   C's did. Confirms the assertion actually distinguishes "has a shifter"
//   from "doesn't", rather than being true of any channel.
// MUTATION ZEROS-FIRST-WORD-ONLY stimulus red: ash changed from 4 to 0 (word
//   fed unchanged, 0xFFFF) -> FAIL: with no shift the word passes through
//   unchanged (0xFFFF), not the hardcoded expected shifted-with-zero-fill
//   value 0x0FFF.
// MUTATION PREV-WORD-BITS stimulus red: a_words[0] changed from 0xABCD to
//   0xABC0 (only the low nibble - the exact bits the assertion is about -
//   zeroed) -> PREV-WORD-BITS FAILs (word 2 becomes 0x0000, not the
//   hardcoded expected 0xD000). The [setup] PREV-WORD1-BASELINE check in the
//   same block stays PASS under this same mutation - not a counterexample,
//   just the correct shape: word 1 is (word0 >> 4) with zero fill, and >>4
//   discards exactly the nibble this mutation changed, so word 1's own value
//   is unchanged by it (0xABCD>>4 == 0xABC0>>4 == 0x0ABC) while word 2, which
//   depends on that nibble via the old_val chain, is not.
// MUTATION ROW-BOUNDARY-CHAIN stimulus red: a_words[1] (row 1's LAST word,
//   0xABCD) changed to 0xABC0 -> FAIL: the second row's first word becomes
//   0x0000, not the hardcoded expected 0xD000 - the low nibble carried
//   across the row boundary is exactly what changed.
// MUTATION MASK-BEFORE-SHIFT stimulus red: BLTAFWM changed from 0xFFF0 (masks
//   off word 0's low nibble) to 0xFFFF (masks nothing) -> FAIL: word 2 becomes
//   0xF000, not the hardcoded expected 0x0000 - the unmasked low nibble of
//   word 0 now survives into word 2's shift-in, showing the assertion is
//   actually sensitive to whether the mask took effect before the value was
//   captured into the chain, not just to shift arithmetic in general.
// MUTATION SHIFT-ON-LOAD stimulus red: in the TEST harness only, the write
//   order reversed - BLTCON1(bsh=2) written BEFORE BLTBDAT instead of after
//   (so the scenario becomes "set the shift, then load", the normal/expected
//   order, not "load, then change the shift") -> SHIFT-ON-LOAD FAILs (the
//   result now equals R2, matching the reversed-order control, not R4). The
//   [control] SHIFT-CONTROLS-DIFFER check (built from two entirely separate
//   harnesses) is untouched by this edit and stays PASS.
// MUTATION PREV-WORD1-BASELINE stimulus red: the PREV-WORD-BITS fixture's
//   `h.a_words = {0xABCD, 0x0000}` changed to `{0xBBCD, 0x0000}`, expectations
//   untouched -> 10 passed 1 failed, PREV-WORD1-BASELINE the only assertion
//   that moves (word 1 becomes 0x0BBC against its expected 0x0ABC; word 2 is
//   still 0xD000, since only the low nibble reaches it). Reverted: 11/0.
//   Worth recording because the first attempt did NOT work: changing 0xABCD to
//   0xABCE left PREV-WORD1-BASELINE green and reddened PREV-WORD-BITS instead
//   -- an ash=4 shift discards the low nibble out of word 1 and delivers it
//   into word 2, so the two assertions read genuinely different bits. The
//   failed attempt is the better evidence that they are independent.
// MUTATION SHIFT-CONTROLS-DIFFER stimulus red: control B's
//   `h.write_reg(BLTCON1, static_cast<uint16_t>(2u << 12))` changed to
//   `4u << 12`, making both reference shifts identical -> 9 passed 2 failed:
//   SHIFT-CONTROLS-DIFFER (r_bsh4 == r_bsh2, so the two shifts are no longer
//   distinguishable) and SHIFT-ON-LOAD with it. Both were expected: this
//   control exists precisely to establish that SHIFT-ON-LOAD's comparison
//   measures something, so collapsing it must take the assertion it supports
//   down as well. Reverted: 11/0.
// MUTATION UNSHIFTED-WORD-COUNT stimulus red: the control's
//   `h.a_words = {0x1111, 0x2222, 0x3333}` changed at index 1 to `0x2223`,
//   the expected vector left as it was -> 10 passed 1 failed, and
//   UNSHIFTED-WORD-COUNT is the only assertion that moves (its harness is
//   its own block). Reverted: 11/0.
// MUTATION EXTRA-WORD-COST-IN-CYCLES stimulus red: the shifted case's width
//   reduced from 4 back to 3 (i.e. the "extra" word is not requested) ->
//   FAIL: exactly 3 words are fetched/stored, not the hardcoded expected 4.
//   The [control] UNSHIFTED-WORD-COUNT check (a separate harness, ash=0,
//   width=3) is untouched and stays PASS.

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
constexpr uint16_t USEA = 1u << 11;
constexpr uint16_t USEB = 1u << 10;
constexpr uint16_t USEC = 1u << 9;
constexpr uint16_t USED = 1u << 8;

// Minterm LF settings (agnus_blitter_minterm.v): bit k of LF selects the
// (A,B,C) combination whose binary value is k, MSB-first as (A,B,C). Each
// of the three "passthrough" constants below is derived the same way: set
// bit k wherever the wanted channel's bit of index k is 1, which reproduces
// that channel's value regardless of what the OTHER two channels carry -
// not merely "correct when the others happen to be zero".
constexpr uint16_t LF_C_PASSTHROUGH = 0xAA;  // D = C, for every A/B value
constexpr uint16_t LF_A_PASSTHROUGH = 0xF0;  // D = A, for every B/C value
constexpr uint16_t LF_B_PASSTHROUGH = 0xCC;  // D = B, for every A/C value

// The ascending-mode shift formula, independently validated in
// hrm/ch6-blitter-barrelshifter/test_agnus_blitter_barrelshifter.cpp
// (hrm_oracle(), 4.2M-case sweep against the leaf module, PKG-BLT-SHIFT,
// confirmed and unchanged). Reused here as a fixed reference, not re-derived
// and not re-swept - this file only checks that agnus_blitter WIRES this
// already-proven shifter the way the manual describes (mask ordering, zero
// fill, word-to-word and row-to-row chaining, load-time capture), not that
// the shifter's own arithmetic is correct.
constexpr uint16_t ascending_shift(uint16_t shift, uint16_t previous, uint16_t current) {
    return shift == 0
        ? current
        : static_cast<uint16_t>((static_cast<uint32_t>(previous) << (16u - shift)) |
                                 (static_cast<uint32_t>(current) >> shift));
}

struct Sample {
    int tick;
    bool busy;
    bool zero;
    bool int3;
    bool we;
    uint16_t data_out;
    bool reqdma;
};

// Copied from hrm/ch6-blitter-status/test_agnus_blitter_status.cpp,
// unchanged in mechanism - see that file's header for why every step here
// is where it is (the second eval() on a DMA grant, capturing we/data_out
// at the grant rather than polling, int3 sampled before the tick's own edge).
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

    void run_cycles(int n) {
        for (int i = 0; i < n; ++i) tick();
    }

    // Runs until `data_writes.size()` reaches `n`, or `max_ticks` is exhausted.
    bool run_until_writes(size_t n, int max_ticks = 500) {
        for (int i = 0; i < max_ticks && data_writes.size() < n; ++i) tick();
        return data_writes.size() >= n;
    }

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
            dut->reg_address_in = reg;
            dut->data_in = v;
            // Settle ackdma/reg_address_in/data_in against the combinational
            // network BEFORE the clock edge - the step the salvaged harness
            // skipped (see hrm/ch6-blitter-status's header).
            dut->eval();
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

// Starts a blit with BLTCON1 zeroed (no shift needed on the B side, no
// line/descending/fill bits) - fine for every scenario below that only needs
// channel A's shift (ash lives in BLTCON0, untouched by this) or needs no
// shift at all. The SHIFT-ON-LOAD scenarios do NOT use this helper for their
// BLTCON1 writes (see below) because they need a specific, surviving bsh
// value at a specific cycle.
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
    {
        Harness h;
        h.c_words = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        start_blit(h, USEC | USED | LF_C_PASSTHROUGH, 4, 1);
        h.run_until_writes(4, 60);

        const std::vector<uint16_t> expected = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        checks.expect(h.data_writes == expected,
            "[fixture check] TEST-DATA-DELIVERY: four distinct C words fed into the "
            "fixture arrive at data_out, in order - proves the DMA grant handshake "
            "actually delivers data (copied from hrm/ch6-blitter-status), which "
            "every data-dependent assertion below depends on");
    }

    // ---------------- NO-SHIFT-ON-C (pp. 169-170) ----------------
    {
        // ash and bsh both driven to their maximum (15) while only channel C
        // is used; C reaches the minterm generator unshifted
        // (agnus_blitter.v:418-422,442-449 - no barrel shifter instance on the
        // C path at all, unlike A (line 341) and B (line 397)), so the
        // extreme shift settings must have no effect whatsoever on a
        // C-passthrough blit.
        Harness h;
        h.c_words = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        h.write_reg(BLTCON0, static_cast<uint16_t>((15u << 12) | USEC | USED | LF_C_PASSTHROUGH));
        h.write_reg(BLTCON1, static_cast<uint16_t>(15u << 12));
        h.write_reg(BLTSIZE, bltsize_word(4, 1));
        h.run_until_writes(4, 60);

        const std::vector<uint16_t> expected = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        checks.expect(h.data_writes == expected,
            "NO-SHIFT-ON-C: with ash=bsh=15 (the maximum shift), a C-passthrough blit "
            "still delivers the four fed words completely unshifted - 'both the A and "
            "B DMA channels have a barrel shifter' (ch6-shifts.c5's neighbour context, "
            "p.179) by omission names only two, matching '[two channels] have shift "
            "registers' (6-dma-channels.c10, ch6-dma.c8, p.170) and 'a block-move "
            "operation can shift up to two source areas' (6-intro.c10, ch6-intro.c9, "
            "p.169) - the complementary half of those four claims (that A and B DO "
            "shift) is independently established by ZEROS-FIRST-WORD-ONLY, "
            "PREV-WORD-BITS, ROW-BOUNDARY-CHAIN and SHIFT-ON-LOAD below, all of which "
            "show A or B's output changing with the shift value");
    }

    // ---------------- ZEROS-FIRST-WORD-ONLY (pp. 179-180) ----------------
    {
        // Fresh Harness: bltaold is only cleared on `init` (agnus_blitter.v:
        // 334-335), which happens once, at the true start of THIS blit. A
        // single-word row (width=1) makes that word simultaneously the first
        // AND only word, so old_val=0 (zeros) is guaranteed to be what feeds
        // the shifter for it.
        Harness h;
        h.a_words = {0xFFFF};
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        start_blit(h, static_cast<uint16_t>((4u << 12) | USEA | USED | LF_A_PASSTHROUGH), 1, 1);
        h.run_until_writes(1, 60);

        const uint16_t expected = ascending_shift(4, 0x0000, 0xFFFF);  // 0x0FFF
        checks.expect(h.data_writes.size() == 1 && h.data_writes[0] == expected,
            "ZEROS-FIRST-WORD-ONLY: the first word of a fresh ascending blit, shifted "
            "right four bits, reads 0x0FFF - top nibble genuinely zero, not garbage - "
            "'the first destination word is the first word fetched from A, shifted "
            "right four bits' (6-shifts-and-masks.c15, p.180) 'with zeros shifted in "
            "from the left' (covering ids 6-shifts-and-masks.c6, 6-shifts-and-masks.c16, "
            "ch6-shifts.c6, ch6-shifts.c13, pp.179-180) and 'on shifted blits... we "
            "only get zeros shifted in for the first word of the first row' (covering "
            "ids 6-shifts-and-masks.c21, ch6-shifts.c16, p.180) - the 'only' is "
            "corroborated by PREV-WORD-BITS and ROW-BOUNDARY-CHAIN below, which show "
            "every other word position getting real chained bits instead of zeros");
    }

    // ---------------- PREV-WORD-BITS (pp. 179-180) ----------------
    {
        Harness h;
        h.a_words = {0xABCD, 0x0000};
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        start_blit(h, static_cast<uint16_t>((4u << 12) | USEA | USED | LF_A_PASSTHROUGH), 2, 1);
        h.run_until_writes(2, 60);

        const uint16_t expected_word1 = ascending_shift(4, 0x0000, 0xABCD);  // 0x0ABC
        checks.expect(h.data_writes.size() >= 1 && h.data_writes[0] == expected_word1,
            "[setup] PREV-WORD1-BASELINE: word 1 (0x0ABC) is the first-word/zero-fill "
            "case already covered by ZEROS-FIRST-WORD-ONLY - precondition for the "
            "word-2 check below, so a green there is not a green-by-accident where "
            "word 1 already carried the wrong data");

        const uint16_t expected_word2 = ascending_shift(4, 0xABCD, 0x0000);  // 0xD000
        checks.expect(h.data_writes.size() == 2 && h.data_writes[1] == expected_word2,
            "PREV-WORD-BITS: the second word (0xD000) carries the low nibble of the "
            "first word's masked value (0xD, from 0xABCD) in its top nibble, exactly "
            "the bits the first word's own shift discarded - 'each subsequent word... "
            "the data shifted out from the previous word is shifted in' (covering ids "
            "6-shifts-and-masks.c7, ch6-shifts.c7, p.179), 'the second word... shifted "
            "right, with the least significant (rightmost) four bits of the first word "
            "shifted in' (covering ids 6-shifts-and-masks.c17, 6-shifts-and-masks.c18, "
            "ch6-shifts.c14, p.180); the underlying 32-bit window this chaining relies "
            "on is 'pixels... addressed 16 at a time by each word address' (covering id "
            "ch6-shifts.c5, p.179) - the shifter spans two such 16-bit words to produce "
            "one pixel-aligned 16-bit result, which is exactly what is measured here");
    }

    // ---------------- ROW-BOUNDARY-CHAIN (p. 180) ----------------
    {
        Harness h;
        h.a_words = {0x1111, 0xABCD, 0x000F, 0x0000};  // row 1: w0,w1 ; row 2: w0,w1
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        start_blit(h, static_cast<uint16_t>((4u << 12) | USEA | USED | LF_A_PASSTHROUGH), 2, 2);
        h.run_until_writes(3, 200);  // need the 3rd write: row 2's first word

        const uint16_t expected = ascending_shift(4, 0xABCD, 0x000F);  // 0xD000
        checks.expect(h.data_writes.size() >= 3 && h.data_writes[2] == expected,
            "ROW-BOUNDARY-CHAIN: the second row's first destination word (0xD000) "
            "carries the low nibble of row 1's LAST word (0xD, from 0xABCD) - bltaold "
            "is reloaded on every next_word transition including a row boundary and is "
            "reset only by `init`, the very first word of the whole blit "
            "(agnus_blitter.v:331-337) - matching 'we will write the first word of the "
            "second row... with the least significant four bits of the last word from "
            "the first row shifted in' (covering ids 6-shifts-and-masks.c19, "
            "6-shifts-and-masks.c20, ch6-shifts.c15, p.180) and the general rule 'on "
            "all other rows the blitter will shift in the bits that it shifted out of "
            "the previous row' (covering ids 6-shifts-and-masks.c22, ch6-shifts.c17, "
            "p.180)");
    }

    // ---------------- MASK-BEFORE-SHIFT (p. 181, Figure 6-4) ----------------
    {
        // BLTAFWM masks off only word 0's low nibble (0xFFF0, not 0xFFFF).
        // bltaold is loaded from bltadat & bltamask (agnus_blitter.v:336-337)
        // - the AND happens BEFORE the value is captured for the next word's
        // shift-in, not after. If masking happened after the shift instead,
        // the masked-off bits would still leak into word 2's shift-in via an
        // unmasked old_val. BLTALWM is left at 0xFFFF so the OTHER mask trap
        // (word 2 is also this 2-word blit's last word) does not confound it.
        Harness h;
        h.a_words = {0xFFFF, 0x0000};
        h.write_reg(BLTAFWM, 0xFFF0);
        h.write_reg(BLTALWM, 0xFFFF);
        start_blit(h, static_cast<uint16_t>((4u << 12) | USEA | USED | LF_A_PASSTHROUGH), 2, 1);
        h.run_until_writes(2, 60);

        const uint16_t masked_word0 = 0xFFFF & 0xFFF0;                    // 0xFFF0
        const uint16_t expected = ascending_shift(4, masked_word0, 0x0000);  // 0x0000
        checks.expect(h.data_writes.size() == 2 && h.data_writes[1] == expected,
            "MASK-BEFORE-SHIFT: word 2 reads 0x0000, not 0xF000 - the low nibble "
            "BLTAFWM masked off word 0's raw value (0xFFFF -> 0xFFF0) does NOT survive "
            "into word 2's shift-in, proving the mask is applied to the source data "
            "before it enters the shift chain, not after. Figure 6-4 (p.181) shows "
            "this same ordering: 'these masks are... applied' to DMA A before the "
            "masked result is combined with B and C (ch6-shifts.f10, the final "
            "destination's masked columns matching the mask box, not the raw source) "
            "and the specific mask value the figure labels ('second word mask' box, "
            "11111110 00000000, ch6-shifts.f5) is itself only meaningful as a pre-shift "
            "AND, exactly the mechanism measured here");
    }

    // ---------------- SHIFT-ON-LOAD (pp. 182, 200) ----------------
    uint16_t r_bsh4, r_bsh2, r_test;
    {
        // [control A]: BSH held at 4 throughout. BDATA (BLTBDAT) written once,
        // to V=0x00F0; bltbold is 0 (fresh harness, bltbold_init=1 from
        // reset), so bltbhold = ascending_shift(4, 0, 0x00F0) = 0x000F.
        Harness h;
        h.write_reg(BLTCON1, static_cast<uint16_t>(4u << 12));
        h.write_reg(BLTBDAT, 0x00F0);
        h.c_words = {0x0000};
        start_blit(h, static_cast<uint16_t>(USEC | USED | LF_B_PASSTHROUGH), 1, 1);
        h.run_until_writes(1, 60);
        r_bsh4 = h.data_writes.empty() ? 0xFFFF /* sentinel: will fail below */ : h.data_writes[0];
    }
    {
        // [control B]: BSH held at 2 throughout, same BDATA value.
        Harness h;
        h.write_reg(BLTCON1, static_cast<uint16_t>(2u << 12));
        h.write_reg(BLTBDAT, 0x00F0);
        h.c_words = {0x0000};
        start_blit(h, static_cast<uint16_t>(USEC | USED | LF_B_PASSTHROUGH), 1, 1);
        h.run_until_writes(1, 60);
        r_bsh2 = h.data_writes.empty() ? 0xFFFF : h.data_writes[0];
    }
    checks.expect(r_bsh4 != r_bsh2,
        "[control] SHIFT-CONTROLS-DIFFER: loading BDATA=0x00F0 under a constant "
        "BSH=4 (0x000F) and under a constant BSH=2 (0x003C) produce different "
        "results - establishes that these two reference shifts are actually "
        "distinguishable for the value used below, so the SHIFT-ON-LOAD comparison "
        "that follows is measuring something real");
    {
        // TEST: BSH=4, then BDATA=0x00F0 written (this is the write whose
        // FOLLOWING cycle captures bltbhold), then BSH changed to 2 on that
        // very following cycle (write_reg calls land back-to-back, one tick
        // apart) - "the last person left BSHIFT to be 4, and I load BDATA
        // with [0x00F0] and then change BSHIFT to 2" (p.182).
        Harness h;
        h.write_reg(BLTCON1, static_cast<uint16_t>(4u << 12));
        h.write_reg(BLTBDAT, 0x00F0);
        h.write_reg(BLTCON1, static_cast<uint16_t>(2u << 12));  // "immediately after"
        h.c_words = {0x0000};
        start_blit(h, static_cast<uint16_t>(USEC | USED | LF_B_PASSTHROUGH), 1, 1);
        h.run_until_writes(1, 60);
        r_test = h.data_writes.empty() ? 0xFFFF : h.data_writes[0];
    }
    checks.expect(r_test == r_bsh4 && r_test != r_bsh2,
        "SHIFT-ON-LOAD: BDATA loaded while BSH=4 and then BSH changed to 2 still "
        "produces the BSH=4 result, not the BSH=2 result - 'shifts are done on "
        "immediate data as soon as it is loaded' (6-blitter-key-points.c19, "
        "ch6-keypoints.c23, p.200); 'be sure to load the blitter immediate data "
        "registers only after setting the shift count..., as loading the data "
        "registers first will lead to unpredictable results... the resulting BDATA "
        "that is used is [the one shifted with the shift count in effect at load "
        "time], not [the later one]. The act of loading one of the data registers "
        "\"draws\" the data through the machine and shifts it' (covering ids "
        "6-shifts-and-masks.c43, 6-shifts-and-masks.c44, 6-shifts-and-masks.c45, "
        "6-shifts-and-masks.c46, ch6-shifts.c40, ch6-shifts.c41, ch6-shifts.c42, "
        "p.182) - bltbdat_wrtn "
        "(agnus_blitter.v:387-394) pulses exactly one cycle after the BLTBDAT write "
        "and captures bltbhold from the shifter output using whatever bsh is in "
        "effect on THAT cycle (agnus_blitter.v:274,407-413), which is what is "
        "measured here; the exact numeric outcome ('1<<4' on p.182) is notation for "
        "which shift count got baked in, not asserted here as a literal left-shift, "
        "since the design's own already-validated ascending formula is a right shift "
        "(matching p.179's 'the shift is normally towards the right' and the worked "
        "example on p.180) - the structural claim (fixed at load time, not "
        "recomputed) is what is tested, using that right-shift formula throughout");

    // ---------------- EXTRA-WORD-COST-IN-CYCLES (p. 171) ----------------
    {
        // [control]: no shift, width=3 - exactly 3 A-fetches/D-stores, values
        // passed through unchanged (shift==0 is the passthrough branch of
        // ascending_shift, matching the RTL's own shift_onehot==0x10000 case).
        Harness h;
        h.a_words = {0x1111, 0x2222, 0x3333};
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        start_blit(h, static_cast<uint16_t>(USEA | USED | LF_A_PASSTHROUGH), 3, 1);  // ash=0
        h.run_until_writes(3, 60);

        const std::vector<uint16_t> expected = {0x1111, 0x2222, 0x3333};
        checks.expect(h.data_writes.size() == 3 && h.data_writes == expected,
            "[control] UNSHIFTED-WORD-COUNT: an unshifted 3-word row performs exactly "
            "3 A-channel fetches and delivers all 3 words unchanged - the baseline "
            "word/cycle count the shifted case below is measured against");
    }
    {
        // TEST: shift applied, width=4 (one MORE than the control's 3) - the
        // extra source word p.171 describes, realised as exactly +1 to the
        // width field, i.e. one extra DMA cycle/word, nothing hidden or
        // automatic in the hardware beyond what the width register says.
        Harness h;
        h.a_words = {0x1111, 0x2222, 0x3333, 0x0000};
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        start_blit(h, static_cast<uint16_t>((4u << 12) | USEA | USED | LF_A_PASSTHROUGH), 4, 1);
        h.run_until_writes(4, 60);

        checks.expect(h.data_writes.size() == 4,
            "EXTRA-WORD-COST-IN-CYCLES: a shifted row configured with width=4 (one "
            "more than the equivalent unshifted row's width=3, control above) "
            "performs exactly 4 A-channel fetches - one MORE than the unshifted case, "
            "matching 'shifting... operations may require an extra word be fetched' "
            "(6-dma-channels.c53, ch6-dma.c53, p.171). Measured here strictly as a "
            "word/DMA-cycle count (4 vs 3), which is what 'an extra word' literally "
            "names - deliberately NOT converted to or compared against any "
            "microsecond/time figure, since agnus_blitter has no clock-frequency "
            "input from which a real-time duration could be derived; p.179's separate "
            "'this shifting operation is completely free... no more time' claim is "
            "about the shift ARITHMETIC costing no extra cycles once a word has been "
            "fetched, a different claim from this one about DMA word count, and is "
            "not itself one of this package's 35 covered ids");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
