// Test purpose: Verify the blitter fill logic's inclusive/exclusive area-fill
//   semantics (agnus_blitter_fill) - the toggle-on-every-source-1 carry chain
//   that begins at the rightmost (bit 0) source bit and runs to the left edge
//   (bit 15), the inclusive OR that leaves boundary lines intact versus the
//   exclusive pass-through that trims one edge and produces a shape one pixel
//   narrower, the FCI-selects-inside-or-outside behaviour shown in figures
//   6-5/6-6, the fill-enable bypass path shown in figure 6-13, and the
//   manual's own worked words on printed pages 184 and 185.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 169, 184, 185, 186, 198 and 199.
// Non-goals: minterm generation, register decoding (BLTCON0/BLTCON1 bit
//   layout, BLTAFWM/BLTALWM), address generation, line mode, masking and the
//   surrounding agnus_blitter pipeline - covered by PKG-BLT-MINTERM,
//   PKG-BLT-REGDECODE, PKG-BLT-ADRGEN, PKG-BLT-LINE and PKG-BLT-MASKS. The
//   AND/OR word-position gating that turns this module's `fco` into the NEXT
//   word's `fci` (agnus_blitter.v:464, agnus_blitter.v:470) is not
//   constructible at this leaf module's boundary - see CLAIM COVERAGE below.
//
// EXTENDS PRIOR ART (hrm/agnus_blitter_fill, unchanged in shape since its
// original commit; see docs/blitter-prior-art.md "Defect 3" and the README).
// What was already here and is kept: the `hrm_oracle()` function computing
// expected fill behaviour from the manual's description rather than the RTL,
// the exhaustive sweep of all 65536 possible 16-bit inputs x 2 FCI values x 2
// modes (262144 cases) plus the separate 131072-case bypass sweep, and the
// case-count assertion that fails a loop that silently stops early. What the
// pipeline added:
//   - `Test class:` / `Primary oracle:` header (prior art had neither - the
//     defect docs/blitter-prior-art.md records as "what puts all eight in
//     layer C").
//   - every assertion now carries its own printed page and every covering
//     claim id, instead of one collective PASS line with no per-assertion
//     citation.
//   - the four hardcoded "HRM examples" are now individually cited: pages 184
//     (FCI=0) and 185 (FCI=1) - previously present but, per
//     docs/blitter-prior-art.md "Defect 3", carrying no page of their own.
//   - two new single-case assertions, MECH-START and MECH-TOGGLE, isolate the
//     carry chain's starting point/stopping point (p.186) and its per-bit
//     toggle rule (p.186, and figure 6-13's "to next block" carry routing,
//     p.199) from the exhaustive sweep, which proves the same rule but not
//     legibly as one bit's story.
//   - a new WHOLE-WORD assertion demonstrates the fill domain is one
//     indivisible 16-bit chain, not two 8-bit halves (p.184, "fills are
//     always done on full 16-bit words").
//   - the bypass sweep already existed; it is now tied explicitly to figure
//     6-13's Fill Enable line (p.199), which it previously was not.
//   - a stimulus `// MUTATION` record for every assertion (prior art had
//     none).
//   - the six arbitrary "boundary_patterns" cases (0x0000, 0xFFFF, 0x0001,
//     0x8000, 0x5555, 0xAAAA) were removed: all six are inside the exhaustive
//     0-0xFFFF sweep already run below and carry no HRM citation of their
//     own, unlike the four HRM examples kept above - they added coverage the
//     exhaustive sweep does not already have zero of, which is none.
//
// THE OVERTURNED CLAIM. 6-blitter-block-diagram.c9 (p.199, figure 6-13) was
// initially discarded and then overturned by the attack stage (Prompt 5,
// 08.09.2026): the figure labels the fill carry as going to two places, "to
// next block" (the next bit's XOR, entirely inside this module -
// agnus_blitter_fill.v:24-26) and "to next word" (`fco`, agnus_blitter_fill.v
// line 29, an output port of this module). A throwaway Verilator harness built
// during the attack stage reproduced the manual's own p.184/185 worked
// example bit-for-bit: input 0010010000011000, inclusive fill ->
// 0011110000011000 (exact match), exclusive fill -> 0001110000001000 (exact
// match), and separately confirmed the bit-to-bit ripple (in=0, out=0; only
// in[0]=1 set, and every one of the 16 output bits flips, fco=1). MECH-TOGGLE
// and the four WORKED-P18x assertions below turn that throwaway measurement
// into real, cited assertions in this file.
//
// THE WORD-TO-WORD HALF IS NOT VISIBLE HERE. `fco` (agnus_blitter_fill.v:29)
// is an output port of this module and is asserted below wherever a case
// carries a defined carry-out. What is NOT visible at this boundary is what
// LATCHES that `fco` into the next word's `fci`: agnus_blitter.v:464
// (`assign fci = first_word_del ? bltcon1[2] : fcy;`) and agnus_blitter.v:470
// (`fcy <= fco;`, inside the `posedge clk` block starting at line 467) both
// live in agnus_blitter.v, outside agnus_blitter_fill's port list (ife, efe,
// fci, fco, in, out - agnus_blitter_fill.v:7-14). This module has no clock
// port at all; it is purely combinational. Every assertion below that reads
// `fco` says explicitly that it is reading this module's own carry-out state,
// not the cross-word latch.
//
// CLAIM COVERAGE, for the 5 of 51 claims with no assertion of their own
// (reported here, not silently dropped):
//   ch6-blockdiag.c8 (p.198, "the fill operation works on the data generated
//     by the minterm combinations") and ch6-blockdiag.c9 (p.198, "fill
//     operations can be performed simultaneously with other complex logic
//     operations") describe this module's place in the wider agnus_blitter
//     pipeline (agnus_blitter.v:442-461: `minterm_out` feeds this module's
//     `in`) and whether fill overlaps other operations in time. Neither is
//     constructible at agnus_blitter_fill's own boundary: this module has no
//     minterm ports and no notion of concurrency with anything else - it is a
//     pure combinational function of `in`. The nearest fact this boundary can
//     state is that SWEEP-INCLUSIVE-SHAPE and SWEEP-EXCLUSIVE-SHAPE below
//     exhaustively cover every possible 16-bit `in`, so nothing about this
//     module's behaviour is special-cased to minterm-shaped patterns - not
//     the same claim, but what is visible here.
//   6-blitter-block-diagram.c8 and ch6-fig613.f19 (p.199, figure 6-13: FCI
//     routed through an AND gate gated by "first word time", a second AND
//     gate gated by "2nd word time", then OR'ed) describe the word-position
//     gating that produces this module's `fci` INPUT. That gating is
//     agnus_blitter.v:464 (`first_word_del ? bltcon1[2] : fcy`), entirely
//     outside this module. Not constructible here. What IS asserted: this
//     module correctly folds WHATEVER fci value it is handed into its carry
//     chain (MECH-START below, and every FCI=0/FCI=1 pair in the exhaustive
//     sweep).
//   6-area-fill.c3 (p.184, "only one bit set per horizontal row") is a
//     precondition on the CALLER's source data (how an outline is drawn), not
//     a hardware behaviour this module enforces, checks or could violate -
//     agnus_blitter_fill.v applies the same toggle-on-every-1 rule to any
//     16-bit pattern regardless of how many bits are set. SWEEP-INCLUSIVE-
//     SHAPE and SWEEP-EXCLUSIVE-SHAPE below exhaustively include inputs that
//     violate this precondition too (e.g. all bits set), so nothing about
//     this module's own behaviour depends on it holding.
//
// Every mutation below was actually applied (edit -> rebuild -> run -> revert
// -> rebuild-clean) under WSL/Verilator and the PASS/FAIL lines quoted are
// what the run printed, not predictions.
//
// MUTATION SWEEP-INCLUSIVE-SHAPE stimulus red: inside run_fill_sweep(), the
//   mode passed to drive_fill() was flipped to its opposite while
//   hrm_oracle() still computed the expectation for the ORIGINAL mode - this
//   is the "break the correspondence" shape a sweep needs, since the sweep
//   recomputes its own expectation per case and a plain input change would
//   not reliably go red -> printed "FAIL: SWEEP-INCLUSIVE-SHAPE ... first
//   mismatch: mode=0 FCI=1 IN=0001 expected OUT=0001 FCO=0 actual OUT=0000
//   FCO=0" - run totalled "9 passed, 4 failed".
// MUTATION SWEEP-INCLUSIVE-FCI stimulus red: same run as
//   SWEEP-INCLUSIVE-SHAPE immediately above - one edit to run_fill_sweep()
//   turns both assertions red together because they read the same
//   inclusive_sweep result -> printed the identical "FAIL:
//   SWEEP-INCLUSIVE-FCI ... first mismatch: mode=0 FCI=1 IN=0001 expected
//   OUT=0001 FCO=0 actual OUT=0000 FCO=0" - run totalled "9 passed, 4
//   failed".
// MUTATION SWEEP-EXCLUSIVE-SHAPE stimulus red: same run as
//   SWEEP-INCLUSIVE-SHAPE above (one mode-flip edit in run_fill_sweep()
//   affects both the inclusive and exclusive sweep calls) -> printed "FAIL:
//   SWEEP-EXCLUSIVE-SHAPE ... first mismatch: mode=1 FCI=1 IN=0001 expected
//   OUT=0000 FCO=0 actual OUT=0001 FCO=0" - run totalled "9 passed, 4
//   failed".
// MUTATION SWEEP-EXCLUSIVE-VERTEX stimulus red: same run as
//   SWEEP-EXCLUSIVE-SHAPE immediately above - shares the exclusive_sweep
//   result -> printed the identical "FAIL: SWEEP-EXCLUSIVE-VERTEX ... first
//   mismatch: mode=1 FCI=1 IN=0001 expected OUT=0000 FCO=0 actual OUT=0001
//   FCO=0" - run totalled "9 passed, 4 failed".
// MUTATION SWEEP-BYPASS stimulus red: in run_bypass_sweep(), the word driven
//   onto `dut.in` was changed to `word + 1` while the expected value stayed
//   `word` -> printed "FAIL: SWEEP-BYPASS ... first mismatch: FCI=0 IN=0000
//   expected OUT=0000 actual OUT=0001", every other assertion PASS - run
//   totalled "12 passed, 1 failed".
// MUTATION MECH-START stimulus red: the "no set source bit" input changed
//   from 0x0000 to 0x0001 for both FCI cases, while the expected words
//   (0x0000/false for FCI=0, 0xFFFF/true for FCI=1) were left untouched ->
//   printed "FAIL: MECH-START ..." with no other assertion affected - run
//   totalled "12 passed, 1 failed".
// MUTATION MECH-TOGGLE stimulus red: the single set source bit's input
//   changed from 0x0001 to 0x0000 while the expected 0xFFFF/true was left
//   untouched -> printed "FAIL: MECH-TOGGLE ..." with no other assertion
//   affected - run totalled "12 passed, 1 failed".
// MUTATION WHOLE-WORD stimulus red: the single set source bit's input changed
//   from 0x0080 (bit 7) to 0x0040 (bit 6) while the expected 0xFF80/true was
//   left untouched -> printed "FAIL: WHOLE-WORD ..." with no other assertion
//   affected - run totalled "12 passed, 1 failed".
// MUTATION WORKED-P184-INCLUSIVE stimulus red: all four hrm_examples[]
//   entries had their driven `input` incremented by 1 (0x2418 -> 0x2419)
//   while every expected output/carry-out field was left untouched -> this
//   entry printed "FAIL: WORKED-P184-INCLUSIVE ...", every non-worked-example
//   assertion PASS - run totalled "9 passed, 4 failed".
// MUTATION WORKED-P184-EXCLUSIVE stimulus red: same run as
//   WORKED-P184-INCLUSIVE immediately above (one edit to all four
//   hrm_examples[] entries) -> this entry printed "FAIL:
//   WORKED-P184-EXCLUSIVE ..." - run totalled "9 passed, 4 failed".
// MUTATION WORKED-P185-INCLUSIVE stimulus red: same run as
//   WORKED-P184-INCLUSIVE above -> this entry printed "FAIL:
//   WORKED-P185-INCLUSIVE ..." - run totalled "9 passed, 4 failed".
// MUTATION WORKED-P185-EXCLUSIVE stimulus red: same run as
//   WORKED-P184-INCLUSIVE above -> this entry printed "FAIL:
//   WORKED-P185-EXCLUSIVE ..." - run totalled "9 passed, 4 failed".
// MUTATION CASE-COUNT stimulus red: the exclusive sweep's upper bound was
//   lowered to 0x7FFFU (half the domain) while the inclusive sweep's bound
//   stayed 0xFFFFU -> printed "FAIL: CASE-COUNT: 327688 of 393224 expected
//   cases actually ran ..."; SWEEP-EXCLUSIVE-SHAPE and SWEEP-EXCLUSIVE-VERTEX
//   both stayed PASS in this same run (the half of the domain that was still
//   swept still matched hrm_oracle() exactly) - run totalled "12 passed, 1
//   failed". This is exactly the gap CASE-COUNT exists to close: a per-case
//   comparison alone cannot tell "every case checked" from "every case
//   checked stopped early".
//
// After every mutation above, the file was reverted and rebuilt clean; the
// final clean run prints "13 passed, 0 failed".

#include <array>
#include <cstdint>
#include <cstdio>
#include <string>

#include <verilated.h>
#include "Vagnus_blitter_fill.h"

namespace {

enum class FillMode {
    inclusive,
    exclusive,
};

struct ExpectedFill {
    std::uint16_t output;
    bool carry_out;
};

struct HrmExample {
    FillMode mode;
    bool carry_in;
    std::uint16_t input;
    std::uint16_t expected_output;
    bool expected_carry_out;
};

// HRM: start at the rightmost (bit 0) source bit and flip the running fill
// state at every source 1, continuing to bit 15 (the left edge). Inclusive
// fill ORs that state back with the source; exclusive fill is the state
// alone. This function computes the expected result from that description,
// not from the RTL (agnus_blitter_fill.v:22-38 implements the same rule, and
// every assertion below checks the RTL against THIS, never the reverse).
ExpectedFill hrm_oracle(
    FillMode mode,
    bool carry_in,
    std::uint16_t input) {
    bool fill_state = carry_in;
    std::uint16_t output = 0;

    for (unsigned bit = 0; bit < 16U; ++bit) {
        const bool input_bit = ((input >> bit) & 1U) != 0U;
        const bool output_bit = mode == FillMode::inclusive
            ? input_bit || fill_state
            : input_bit != fill_state;
        if (output_bit) {
            output = static_cast<std::uint16_t>(output | (1U << bit));
        }
        fill_state = fill_state != input_bit;
    }

    return ExpectedFill{output, fill_state};
}

// Drives one fill case onto the DUT and reads back what it produced. Not a
// pass/fail check by itself - callers compare the result against an
// expectation and report through Checks::expect().
ExpectedFill drive_fill(
    Vagnus_blitter_fill& dut,
    FillMode mode,
    bool carry_in,
    std::uint16_t input) {
    dut.ife = mode == FillMode::inclusive ? 1 : 0;
    dut.efe = mode == FillMode::exclusive ? 1 : 0;
    dut.fci = carry_in ? 1 : 0;
    dut.in = input;
    dut.eval();
    return ExpectedFill{
        static_cast<std::uint16_t>(dut.out),
        dut.fco != 0U};
}

std::uint16_t drive_bypass(
    Vagnus_blitter_fill& dut,
    bool carry_in,
    std::uint16_t input) {
    dut.ife = 0;
    dut.efe = 0;
    dut.fci = carry_in ? 1 : 0;
    dut.in = input;
    dut.eval();
    return static_cast<std::uint16_t>(dut.out);
}

struct SweepResult {
    std::size_t cases = 0;
    std::size_t mismatches = 0;
    std::string first_failure;
};

// Runs every one of the 65536 possible 16-bit inputs x both FCI values
// against hrm_oracle(), for one fixed mode. Does not stop at the first
// mismatch - CASE-COUNT below depends on every case actually running.
SweepResult run_fill_sweep(Vagnus_blitter_fill& dut, FillMode mode) {
    SweepResult result;
    for (unsigned input = 0; input <= 0xFFFFU; ++input) {
        for (unsigned carry_in = 0; carry_in < 2U; ++carry_in) {
            const auto word = static_cast<std::uint16_t>(input);
            const auto expected = hrm_oracle(mode, carry_in != 0U, word);
            const auto actual = drive_fill(dut, mode, carry_in != 0U, word);
            ++result.cases;
            if (actual.output != expected.output ||
                actual.carry_out != expected.carry_out) {
                ++result.mismatches;
                if (result.first_failure.empty()) {
                    char buf[160];
                    std::snprintf(buf, sizeof(buf),
                        "first mismatch: mode=%u FCI=%u IN=%04x expected OUT=%04x FCO=%u actual OUT=%04x FCO=%u",
                        mode == FillMode::inclusive ? 0U : 1U,
                        carry_in,
                        static_cast<unsigned>(word),
                        static_cast<unsigned>(expected.output),
                        static_cast<unsigned>(expected.carry_out),
                        static_cast<unsigned>(actual.output),
                        static_cast<unsigned>(actual.carry_out));
                    result.first_failure = buf;
                }
            }
        }
    }
    return result;
}

// Runs every one of the 65536 possible 16-bit inputs x both FCI values with
// fill disabled (ife=efe=0) and checks the bypass path returns the input
// unchanged.
SweepResult run_bypass_sweep(Vagnus_blitter_fill& dut) {
    SweepResult result;
    for (unsigned input = 0; input <= 0xFFFFU; ++input) {
        for (unsigned carry_in = 0; carry_in < 2U; ++carry_in) {
            const auto word = static_cast<std::uint16_t>(input);
            const auto actual = drive_bypass(dut, carry_in != 0U, word);
            ++result.cases;
            if (actual != word) {
                ++result.mismatches;
                if (result.first_failure.empty()) {
                    char buf[96];
                    std::snprintf(buf, sizeof(buf),
                        "first mismatch: FCI=%u IN=%04x expected OUT=%04x actual OUT=%04x",
                        carry_in, static_cast<unsigned>(word),
                        static_cast<unsigned>(word),
                        static_cast<unsigned>(actual));
                    result.first_failure = buf;
                }
            }
        }
    }
    return result;
}

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::printf("%s: %s\n", condition ? "PASS" : "FAIL", label.c_str());
        condition ? ++passed : ++failed;
    }
    int passed = 0;
    int failed = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus_blitter_fill dut;
    Checks checks;
    std::size_t total_cases = 0;

    // ---------------- exhaustive sweeps (pp. 184-186, 199) ----------------

    const SweepResult inclusive_sweep = run_fill_sweep(dut, FillMode::inclusive);
    const SweepResult exclusive_sweep = run_fill_sweep(dut, FillMode::exclusive);
    const SweepResult bypass_sweep = run_bypass_sweep(dut);
    total_cases += inclusive_sweep.cases + exclusive_sweep.cases + bypass_sweep.cases;

    checks.expect(inclusive_sweep.mismatches == 0,
        "SWEEP-INCLUSIVE-SHAPE: exhaustively confirmed across all 65536 possible "
        "16-bit source words x 2 FCI values (131072 cases) - inclusive fill (out = "
        "carry | in, agnus_blitter_fill.v:36) fills the interior between paired "
        "boundary lines while leaving the boundary lines themselves intact/set "
        "(p.184, 'leaving the lines intact', 6-area-fill.c10, 6-area-fill.c11, "
        "ch6-fill.c9)" +
        (inclusive_sweep.mismatches ? " -- " + inclusive_sweep.first_failure : ""));

    checks.expect(inclusive_sweep.mismatches == 0,
        "SWEEP-INCLUSIVE-FCI: the same exhaustive inclusive-mode sweep shows FCI "
        "selects which side of a closed outline gets filled - with FCI=0 the area "
        "outside a pair of boundary lines stays 0 while the interior between them "
        "fills with 1s (p.185, figure 6-5, 6-area-fill.c23), and with FCI=1 the "
        "outside fills with 1s while the interior stays 0 (p.185, 'If the FCI bit "
        "is a 1 instead of a 0, the area outside the lines is filled', figure 6-6, "
        "6-area-fill.c24, 6-area-fill.c25, 6-area-fill.c26, ch6-fill.c18, "
        "ch6-fill.c19, ch6-fill.f1, ch6-fill.f2, ch6-fill.f3)" +
        (inclusive_sweep.mismatches ? " -- " + inclusive_sweep.first_failure : ""));

    checks.expect(exclusive_sweep.mismatches == 0,
        "SWEEP-EXCLUSIVE-SHAPE: exhaustively confirmed across all 65536 possible "
        "16-bit source words x 2 FCI values (131072 cases) - exclusive fill (out = "
        "carry, agnus_blitter_fill.v:34, no OR with the source) fills the interior "
        "between paired boundary lines but preserves only the right-edge line "
        "while deleting the left-edge line, producing a shape one pixel narrower "
        "than the same pattern filled with inclusive fill (p.184, 'leaving the "
        "lines bordering the right edge of filled regions but deleting the lines "
        "bordering the left edge', 'yields filled shapes one pixel narrower', "
        "6-area-fill.c12, 6-area-fill.c13, 6-area-fill.c14, 6-area-fill.c15, "
        "ch6-fill.c10, ch6-fill.c11, ch6-fill.c12)" +
        (exclusive_sweep.mismatches ? " -- " + exclusive_sweep.first_failure : ""));

    checks.expect(exclusive_sweep.mismatches == 0,
        "SWEEP-EXCLUSIVE-VERTEX: the same exhaustive exclusive-mode sweep is what "
        "produces sharp single-point vertices where inclusive fill would leave a "
        "two-line-wide seam (p.185, 'If you wish to produce very sharp, "
        "single-point vertices, exclusive-fill', 6-area-fill.c27, ch6-fill.c20), "
        "matching figure 6-7's taper to a single set bit (p.186, 6-area-fill.c28, "
        "ch6-fill.f4, ch6-fill.f5)" +
        (exclusive_sweep.mismatches ? " -- " + exclusive_sweep.first_failure : ""));

    checks.expect(bypass_sweep.mismatches == 0,
        "SWEEP-BYPASS: exhaustively confirmed across all 65536 possible 16-bit "
        "source words x 2 FCI values (131072 cases) - with fill disabled "
        "(ife=efe=0), out=in unchanged (agnus_blitter_fill.v:38), matching figure "
        "6-13's Fill Enable line selecting whether the minterm result passes "
        "through the fill logic before the D hold register or bypasses it (p.199, "
        "6-blitter-block-diagram.c10)" +
        (bypass_sweep.mismatches ? " -- " + bypass_sweep.first_failure : ""));

    // ---------------- carry-chain mechanism, isolated (pp. 186, 199) ----------------
    //
    // The exhaustive sweeps above prove the toggle rule for all 131072 cases at
    // once but do not tell a legible single-bit story. These two cases isolate
    // it: MECH-START asks "where does the chain begin and where does it stop",
    // MECH-TOGGLE asks "what makes it flip". Both drive exclusive mode so `out`
    // reads the raw carry chain (agnus_blitter_fill.v:34, out=carry, no OR).
    {
        const auto no_bits_fci0 = drive_fill(dut, FillMode::exclusive, false, 0x0000U);
        const auto no_bits_fci1 = drive_fill(dut, FillMode::exclusive, true, 0x0000U);
        total_cases += 2;
        checks.expect(
            no_bits_fci0.output == 0x0000U && !no_bits_fci0.carry_out &&
            no_bits_fci1.output == 0xFFFFU && no_bits_fci1.carry_out,
            "MECH-START: with no source bit set, the running fill state begins at "
            "bit 0 (the rightmost/least-significant bit) equal to FCI "
            "(agnus_blitter_fill.v:22, 'carry[0] = fci ^ in[0]') and, with nothing "
            "to flip it, propagates that same value unchanged all the way to bit "
            "15 (the left edge), where it stops and becomes FCO "
            "(agnus_blitter_fill.v:26,29) - FCI=0 leaves the whole word 0, FCI=1 "
            "leaves the whole word 1 (p.186, 'beginning at the rightmost edge of "
            "each line' / 'until the left edge of the blit is reached, at which "
            "point the filling stops', 6-area-fill.c30, 6-area-fill.c33, "
            "ch6-fill.c22, ch6-fill.c24)");

        const auto one_bit = drive_fill(dut, FillMode::exclusive, false, 0x0001U);
        total_cases += 1;
        checks.expect(one_bit.output == 0xFFFFU && one_bit.carry_out,
            "MECH-TOGGLE: a single set source bit (here bit 0) flips the running "
            "fill state for every bit position from there to the left edge, so one "
            "set bit turns every one of the 16 output bits on "
            "(agnus_blitter_fill.v:22-26) - 'for each \"1\" bit in the source area, "
            "the blitter flips the fill state' and the current state 'either "
            "fill[s] or not fill[s] the space with ones' (p.186, 6-area-fill.c31, "
            "6-area-fill.c32, ch6-fill.c23); this is also the bit-to-bit half of "
            "figure 6-13's fill carry, drawn going 'to next block' as well as 'to "
            "next word' (p.199, 6-blitter-block-diagram.c9, ch6-fig613.f22) - the "
            "next-block half is entirely inside this module and is what this "
            "assertion measures; the next-word half (fco latched into the next "
            "word's fci) is not, see the file header");
    }

    // ---------------- whole-word domain (p. 184) ----------------
    {
        const auto bit7_only = drive_fill(dut, FillMode::exclusive, false, 0x0080U);
        total_cases += 1;
        checks.expect(bit7_only.output == 0xFF80U && bit7_only.carry_out,
            "WHOLE-WORD: a source bit set only at bit 7 (the middle of the two "
            "bytes that make up the word) flips the fill state there and the state "
            "carries straight into bit 8 with no reset at the byte boundary - the "
            "16 bits form one indivisible fill domain, not two independent 8-bit "
            "halves (p.184, '(Of course, fills are always done on full 16-bit "
            "words.)', 6-area-fill.c18, ch6-fill.c13d)");
    }

    // ---------------- the manual's own worked words (pp. 184-185, 199) ----------------
    //
    // Kept from prior art (docs/blitter-prior-art.md "Defect 3" noted these ran
    // but carried no page); each is now its own cited assertion. Source pattern
    // 0010010000011000 (0x2418) is the manual's own outline example - filling it
    // is a direct instance of 'the blitter can fill outlined shapes' (p.169,
    // 6-intro.c12, ch6-intro.c11).
    const std::array<HrmExample, 4> hrm_examples{{
        {FillMode::inclusive, false, 0x2418U, 0x3C18U, false},
        {FillMode::exclusive, false, 0x2418U, 0x1C08U, false},
        {FillMode::inclusive, true, 0x2418U, 0xE7FFU, true},
        {FillMode::exclusive, true, 0x2418U, 0xE3F7U, true},
    }};

    {
        const auto& ex = hrm_examples[0];
        const auto actual = drive_fill(dut, ex.mode, ex.carry_in, ex.input);
        ++total_cases;
        checks.expect(actual.output == ex.expected_output && actual.carry_out == ex.expected_carry_out,
            "WORKED-P184-INCLUSIVE: the manual's own worked example, FCI=0, source "
            "0010010000011000, inclusive fill -> 0011110000011000 (p.184, "
            "'00100100-00011000', '00111100-00011000', 6-area-fill.c16, "
            "ch6-fill.c13, ch6-fill.c13b) - the blitter filling an outlined shape "
            "(p.169, 6-intro.c12, ch6-intro.c11)");
    }
    {
        const auto& ex = hrm_examples[1];
        const auto actual = drive_fill(dut, ex.mode, ex.carry_in, ex.input);
        ++total_cases;
        checks.expect(actual.output == ex.expected_output && actual.carry_out == ex.expected_carry_out,
            "WORKED-P184-EXCLUSIVE: the same source, FCI=0, exclusive fill -> "
            "0001110000001000 (p.184, '00011100-00001000', 6-area-fill.c17, "
            "ch6-fill.c13c)");
    }
    {
        const auto& ex = hrm_examples[2];
        const auto actual = drive_fill(dut, ex.mode, ex.carry_in, ex.input);
        ++total_cases;
        checks.expect(actual.output == ex.expected_output && actual.carry_out == ex.expected_carry_out,
            "WORKED-P185-INCLUSIVE: the same source with FCI=1, inclusive fill -> "
            "11100111-11111111 (p.185, 6-area-fill.c21); the expected carry-out "
            "(true) is this module's OWN fco (agnus_blitter_fill.v:29) - the "
            "figure 6-13 claim that this value is what latches into the NEXT "
            "word's fci (p.199, 6-blitter-block-diagram.c9, ch6-fig613.f23) is not "
            "checked here, since that latch (agnus_blitter.v:464,470) is outside "
            "this leaf module's boundary");
    }
    {
        const auto& ex = hrm_examples[3];
        const auto actual = drive_fill(dut, ex.mode, ex.carry_in, ex.input);
        ++total_cases;
        checks.expect(actual.output == ex.expected_output && actual.carry_out == ex.expected_carry_out,
            "WORKED-P185-EXCLUSIVE: the same source with FCI=1, exclusive fill -> "
            "11100011-11110111 (p.185, 6-area-fill.c22)");
    }

    // ---------------- fixture completeness (no page: self-check) ----------------
    constexpr std::size_t expected_cases = 393224U;
    checks.expect(total_cases == expected_cases,
        "CASE-COUNT: " + std::to_string(total_cases) + " of " +
        std::to_string(expected_cases) + " expected cases actually ran (131072 "
        "inclusive-sweep + 131072 exclusive-sweep + 131072 bypass-sweep + 2 "
        "MECH-START + 1 MECH-TOGGLE + 1 WHOLE-WORD + 4 worked-example cases) - a "
        "loop that silently stops early must fail this, not merely fail to "
        "increment a pass count");

    std::printf("\n%d passed, %d failed\n", checks.passed, checks.failed);
    dut.final();
    return checks.failed == 0 ? 0 : 1;
}
