// Test purpose: Verify the blitter barrel shifter
//   (agnus_blitter_barrelshifter) produces the HRM-derived result for every
//   one of the 16 possible shift values (0 through 15) in both ascending and
//   descending mode, that ascending mode shifts right and descending mode
//   shifts left, and that the module's structure - a 16-bit output taken
//   from a position within a 32-bit combination of its two 16-bit inputs,
//   the position chosen by the `shift` input alone - matches the HRM's block
//   diagram and prose.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 169, 179, 182, 198, 199 and 200.
// Non-goals: - Shift-on-load of an immediate data register (p.182: "Be sure
//   to load the blitter immediate data registers only after setting the
//   shift count ... The act of loading one of the data registers 'draws' the
//   data through the machine and shifts it."). This module has no clock port
//   and no register at all - it is purely combinational (desc, shift,
//   new_val, old_val -> out, agnus_blitter_barrelshifter.v:6-13) - so there
//   is no "load" event to observe here. The register that is loaded
//   (bltbhold) and the moment its load samples the then-current shift value
//   live in agnus_blitter.v, outside this module's boundary. That is
//   PKG-BLT-SHIFT-CHAIN's territory and is already classified there:
//   6-blitter-key-points.c19, ch6-keypoints.c23, 6-shifts-and- masks.c43,
//   6-shifts-and-masks.c46, ch6-shifts.c40, ch6-shifts.c42. - Chaining
//   behaviour across words and rows of one blit - zeros shifted in for the
//   first word of a blit, the previous row's bits carried at a row boundary.
//   This module has no notion of "first word" or "row" at all: every call is
//   an independent, stateless function of whatever
//   old_val/new_val/shift/desc it is given. That sequencing lives in
//   agnus_blitter's bltahold/bltbhold registers and is PKG-BLT-SHIFT-
//   CHAIN's territory, a separate fixture that compiles agnus_blitter. -
//   Where the `shift` and `desc` port values originate (BLTCON0/BLTCON1
//   field decode) - PKG-BLT-REGDECODE's / PKG-BLT-BLTCON-FIELDS' territory;
//   this module receives them as plain input ports. - The AND-gate/mask
//   mechanics that form "A old" before it reaches this module
//   (ch6-fig613.f06's three AND gates and their OR) - PKG-BLT- MASKS'
//   territory; this module has no mask-related port at all. Only the half of
//   f06 that says the SHIFTER performs no further masking is asserted here
//   (MASK-UPSTREAM below). - The C channel's path around both mask and
//   shifter (ch6-fig613.f12) - not constructible at this boundary: the
//   module has no C-channel- related port whatsoever (absent_signal: a
//   C/bypass-select input; the port list is exactly desc, shift, new_val,
//   old_val, out, agnus_blitter_barrelshifter.v:6-13), so there is nothing
//   here that could confirm or contradict a claim about channel C bypassing
//   this module entirely.
// Claim ids: ids such as `6-intro.c11` and `ch6-intro.c10` name one sentence
//   of the manual in this chapter's reading. They carry no reader prefix, and
//   they resolve in the claim files that hrm-testpipeline/acceptance/ch6.json
//   lists under `inventories`.
//
// EXTENDS PRIOR ART (hrm/ch6-blitter-barrelshifter, unchanged in shape
// since its original commit; see docs/evidence/blitter-prior-art.md). What was
// already here and is kept, verbatim in mechanism: the `hrm_oracle()`
// function computing the expected output from the manual's shift
// description rather than from the RTL; the five-part exhaustive sweep
// (shift=0 passthrough, old_val=0x0000 across every current word and every
// nonzero shift, current=0x0000 across every old_val and every nonzero
// shift, a walk of all 32 abstract input bit positions, and an 8x8 pattern
// cross-product) driving all 16 shift values and both desc values
// throughout; and the self-checked case count (4,197,376) that fails the
// test if any section stops early. Read the exhaustive sweep sections below
// for one correction: the original comment on the second section ("First
// word: no predecessor, so zeros are shifted in") described a claim this
// module cannot make (see the note at that section) - the sweep code itself
// is untouched, only its comment.
//
// What the pipeline added:
//   - `Test class:` / `Primary oracle:` header (prior art had neither).
//   - the exhaustive sweep's completeness check is now a named, cited
//     RANGE assertion instead of a bare `if (checks != expected_checks)
//     return 1;` with no citation - the sweep and its total are unchanged,
//     only how the result is reported.
//   - RIGHT-ASCENDING and LEFT-DESCENDING: two claims overturned from
//     not-asserted discards by Prompt 5's attack stage on 08.09.2026, each
//     reproduced here as the exact stimulus a different model measured
//     against this exact module before this package existed.
//   - STRUCTURE-WINDOW and SHIFT-VALUE-SELECTS: two structural claims from
//     the block diagram (p.198 prose, Figure 6-13 on p.199) that the
//     exhaustive sweep proves but does not report legibly on its own -
//     that the output is a genuine 16-bit window straddling both 16-bit
//     input halves, and that the window's position tracks the numeric
//     value of `shift` alone, with no clock involved.
//   - MASK-UPSTREAM: the constructible half of a figure claim about masks
//     preceding the shifter (the other half, the AND-gate mechanics
//     themselves, is out of scope - see Non-goals).
//   - a stimulus `// MUTATION` record for every named assertion below
//     (prior art had none).
//
// CLAIM COVERAGE, all 24 of PKG-BLT-SHIFT's testable claims:
//   RANGE            (7 ids, asserted below)
//   RIGHT-ASCENDING  (3 ids, asserted below)
//   LEFT-DESCENDING  (3 ids, asserted below)
//   STRUCTURE-WINDOW (6 ids, asserted below)
//   SHIFT-VALUE-SELECTS (3 ids, asserted below)
//   MASK-UPSTREAM    (1 id, partially asserted - see the assertion itself)
//   ch6-fig613.f12   (1 id, NOT asserted - not constructible at this
//     boundary, see Non-goals above: no C-channel-related port exists here
//     at all)
// That is 7+3+3+6+3+1 = 23 ids asserted plus 1 id reported-not-asserted,
// covering the full 24.
//
// Every mutation below was actually applied (edit -> rebuild -> run ->
// revert -> rebuild-clean) under WSL/Verilator and the PASS/FAIL lines
// quoted are what the run printed, not predictions. Clean baseline:
// "6 passed, 0 failed (named, cited assertions)" plus the kept exhaustive
// sweep's count, "MEASURED: 4197376 HRM-derived blitter barrel shifter
// cases".
//
// Every named assertion's PASS/FAIL line is a fixed label (Checks::expect
// prints only the condition and the static message, not a computed actual
// value), so "what actually printed" below is the PASS->FAIL transition of
// that exact label plus the passed/failed tally; where a numeric result is
// given in parentheses it was independently derived from the oracle/RTL
// arithmetic to design the mutation, not echoed by the run itself, and is
// marked as such.
//
// MUTATION RANGE stimulus red: the patterns-loop shift bound was
//   temporarily shrunk from `shift < 16U` to `shift < 15U`, dropping
//   shift=15 from that section's 8x8x16x2 cross product (128 fewer cases)
//   while `expected_checks` (4197376) was left untouched -> the RANGE line
//   printed FAIL instead of PASS, and the run additionally printed
//   "MEASURED: exhaustive sweep completed 4197248 cases, expected 4197376"
//   (a real, dynamically computed diagnostic - this one line is printed
//   verbatim from the run). Every other named assertion still PASS.
//   Totalled "5 passed, 1 failed". Reverted; rebuild-clean reproduced
//   "MEASURED: 4197376 ..." and "6 passed, 0 failed" again.
// MUTATION RIGHT-ASCENDING stimulus red: both `dut.desc = 0` lines in the
//   RIGHT-ASCENDING block were changed to `dut.desc = 1` (asking for a
//   left/descending shift instead) while the expected constants 0x0800 and
//   0x0001 were left untouched -> the RIGHT-ASCENDING line printed FAIL.
//   (Independently derived from hrm_oracle(true, ...), not printed by the
//   run: with desc=1 the two stimuli actually produce out=0x0000 - shift=4,
//   new_val=0x8000: current<<4 truncates entirely out of the low 16 bits -
//   and out=0x0004 - shift=1, new_val=0x0002: current<<1=0x0004 - neither
//   matching the held-fixed expected constants 0x0800/0x0001.) Every other
//   named assertion PASS. Totalled "5 passed, 1 failed". Reverted.
// MUTATION LEFT-DESCENDING stimulus red: both `dut.desc = 1` lines in the
//   LEFT-DESCENDING block were changed to `dut.desc = 0` while the expected
//   constants 0x8000 and 0x0080 were left untouched -> the LEFT-DESCENDING
//   line printed FAIL. (Independently derived from hrm_oracle(false, ...):
//   with desc=0 the two stimuli actually produce out=0x2000 - shift=1,
//   new_val=0x4000: current>>1=0x2000 - and out=0x0000 - shift=4,
//   new_val=0x0008: current>>4=0x0000, since 0x0008 has no set bits above
//   position 3 - neither matching the held-fixed expected constants
//   0x8000/0x0080.) Every other named assertion PASS. Totalled
//   "5 passed, 1 failed". Reverted.
// MUTATION STRUCTURE-WINDOW stimulus red: `dut.desc = 0` in the
//   STRUCTURE-WINDOW block was changed to `dut.desc = 1` (descending
//   instead of ascending) while `expected` stayed computed from
//   `hrm_oracle(false, 8, 0x00AB, 0xCD00)` (i.e. still the ascending
//   answer, 0xABCD) -> the STRUCTURE-WINDOW line printed FAIL.
//   (Independently derived from hrm_oracle(true, 8, 0x00AB, 0xCD00): with
//   desc=1 the same stimulus actually produces out=0x0000 - current<<8
//   truncates entirely out of the low 16 bits and previous>>8 is 0 since
//   0x00AB fits in 8 bits - nowhere near the ascending answer 0xABCD.)
//   Every other named assertion PASS. Totalled "5 passed, 1 failed".
//   Reverted.
// MUTATION SHIFT-VALUE-SELECTS stimulus red: the second and third
//   `dut.shift = 7;` / `dut.shift = 15;` lines were both changed to
//   `dut.shift = 0;` (asking every sample for the same window position)
//   while the expected constants 0x0001 and 0x0000 were left untouched ->
//   the SHIFT-VALUE-SELECTS line printed FAIL. (Independently derived:
//   out_s7 and out_s15 both actually read 0x0080, identical to out_s0,
//   since shift never varied.) Every other named assertion PASS. Totalled
//   "5 passed, 1 failed". Reverted.
// MUTATION MASK-UPSTREAM stimulus red: immediately before the
//   `old_val = 0xFFFF` sample, `dut.new_val` was changed from the inherited
//   0x1234 to 0x1235, while the comparison still expected both samples to
//   equal 0x1234 -> the MASK-UPSTREAM line printed FAIL. (Independently
//   derived: out_hi actually reads 0x1235, matching the new new_val, not
//   the old comparison constant.) Every other named assertion PASS.
//   Totalled "5 passed, 1 failed". Reverted.
//
// After every mutation above the file was reverted and rebuilt clean; the
// final clean run reproduces "MEASURED: 4197376 HRM-derived blitter barrel
// shifter cases" and "6 passed, 0 failed (named, cited assertions)".

#include <Vagnus_blitter_barrelshifter.h>
#include <verilated.h>

#include <array>
#include <cstdio>
#include <cstdint>
#include <iostream>
#include <string>

#define HRM_STRINGIFY_IMPL(...) #__VA_ARGS__
#define HRM_STRINGIFY(...) HRM_STRINGIFY_IMPL(__VA_ARGS__)

namespace {

std::uint16_t hrm_oracle(
    bool descending,
    std::uint8_t shift,
    std::uint16_t previous,
    std::uint16_t current) {
    if (shift == 0U) {
        return current;
    }
    if (descending) {
        return static_cast<std::uint16_t>(
            (static_cast<std::uint32_t>(current) << shift) |
            (static_cast<std::uint32_t>(previous) >> (16U - shift)));
    }
    return static_cast<std::uint16_t>(
        (static_cast<std::uint32_t>(previous) << (16U - shift)) |
        (static_cast<std::uint32_t>(current) >> shift));
}

bool check_case(
    Vagnus_blitter_barrelshifter& dut,
    bool descending,
    std::uint8_t shift,
    std::uint16_t previous,
    std::uint16_t current,
    std::size_t& checks) {
    const auto expected =
        hrm_oracle(descending, shift, previous, current);
    dut.desc = descending;
    dut.shift = shift;
    dut.old_val = previous;
    dut.new_val = current;
    dut.eval();
    ++checks;
    const auto actual = static_cast<std::uint16_t>(dut.out);
    if (actual == expected) {
        return true;
    }
    std::fprintf(
        stderr,
        HRM_STRINGIFY(
            FAIL desc=%u shift=%u P=%04x C=%04x expected=%04x actual=%04x),
        static_cast<unsigned>(descending),
        static_cast<unsigned>(shift),
        static_cast<unsigned>(previous),
        static_cast<unsigned>(current),
        static_cast<unsigned>(expected),
        static_cast<unsigned>(actual));
    std::fputc(10, stderr);
    return false;
}

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    int passed = 0;
    int failed = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus_blitter_barrelshifter dut;
    std::size_t checks = 0;
    Checks named;

    const std::array<std::uint16_t, 2> zero_shift_previous{{
        0x0000U,
        0xFFFFU,
    }};

    // Shift zero selects the current word and ignores the previous word.
    for (unsigned current = 0; current <= 0xFFFFU; ++current) {
        for (const auto previous : zero_shift_previous) {
            for (unsigned desc = 0; desc < 2U; ++desc) {
                const bool ok = check_case(
                    dut, desc != 0U, 0U, previous,
                    static_cast<std::uint16_t>(current), checks);
                if (ok == false) {
                    return 1;
                }
            }
        }
    }

    // old_val fixed at 0x0000 across every current word and every nonzero
    // shift/desc combination. NOTE (added by the pipeline): the original
    // comment here read "First word: no predecessor, so zeros are shifted
    // in" - that framing overreaches what this module can state. This
    // module has no notion of "first word" or "predecessor" at all: it is
    // purely combinational (no clk port, no state, agnus_blitter_
    // barrelshifter.v:6-13), and old_val=0x0000 below is just one value
    // among many this loop happens to pass, not evidence about blit
    // sequencing. The genuine "the first word of a blit has no predecessor,
    // so zeros are shifted in" claim needs the agnus_blitter boundary,
    // where bltahold/bltbhold actually start at their reset value at the
    // start of a blit - that is PKG-BLT-SHIFT-CHAIN's territory. The sweep
    // itself is kept unchanged: it remains one more corner of the RANGE
    // claim below (every nonzero shift, both desc values, old_val=0x0000).
    for (unsigned current = 0; current <= 0xFFFFU; ++current) {
        for (unsigned shift = 1; shift < 16U; ++shift) {
            for (unsigned desc = 0; desc < 2U; ++desc) {
                const bool ok = check_case(
                    dut, desc != 0U, static_cast<std::uint8_t>(shift),
                    0x0000U, static_cast<std::uint16_t>(current), checks);
                if (ok == false) {
                    return 1;
                }
            }
        }
    }

    // Zero current word isolates bits crossing from the previous word.
    for (unsigned previous = 0; previous <= 0xFFFFU; ++previous) {
        for (unsigned shift = 1; shift < 16U; ++shift) {
            for (unsigned desc = 0; desc < 2U; ++desc) {
                const bool ok = check_case(
                    dut, desc != 0U, static_cast<std::uint8_t>(shift),
                    static_cast<std::uint16_t>(previous), 0x0000U, checks);
                if (ok == false) {
                    return 1;
                }
            }
        }
    }

    // Route every individual bit in the abstract 32-bit P:C input.
    for (unsigned bit = 0; bit < 32U; ++bit) {
        const auto previous = static_cast<std::uint16_t>(
            bit >= 16U ? 1U << (bit - 16U) : 0U);
        const auto current = static_cast<std::uint16_t>(
            bit < 16U ? 1U << bit : 0U);
        for (unsigned shift = 0; shift < 16U; ++shift) {
            for (unsigned desc = 0; desc < 2U; ++desc) {
                const bool ok = check_case(
                    dut, desc != 0U, static_cast<std::uint8_t>(shift),
                    previous, current, checks);
                if (ok == false) {
                    return 1;
                }
            }
        }
    }

    const std::array<std::uint16_t, 8> patterns{{
        0x0000U,
        0xFFFFU,
        0xAAAAU,
        0x5555U,
        0x8001U,
        0x7FFEU,
        0x00FFU,
        0xFF00U,
    }};
    for (const auto previous : patterns) {
        for (const auto current : patterns) {
            // MUTATION HOOK (normally inert): MUTATION RANGE temporarily
            // changed `shift < 16U` to `shift < 15U` here, dropping shift=15
            // from this section only.
            for (unsigned shift = 0; shift < 16U; ++shift) {
                for (unsigned desc = 0; desc < 2U; ++desc) {
                    const bool ok = check_case(
                        dut, desc != 0U, static_cast<std::uint8_t>(shift),
                        previous, current, checks);
                    if (ok == false) {
                        return 1;
                    }
                }
            }
        }
    }

    // ---------------- RANGE (kept from prior art; now named/cited) ----------------
    //
    // check_case() above already aborts the whole test (return 1) on the
    // FIRST mismatch anywhere in the sweep, so by the time this line runs
    // every one of the 4,197,376 cases has already matched the HRM-derived
    // oracle. This assertion checks the one thing that abort-on-mismatch
    // cannot: that the sweep actually ran to its full, expected count
    // rather than a loop silently exiting early with fewer cases checked.
    constexpr std::size_t expected_checks = 4197376U;
    named.expect(checks == expected_checks,
        "RANGE: every one of the 16 possible shift values 0 through 15 "
        "(`shift[3:0]`, agnus_blitter_barrelshifter.v:9) produces the "
        "HRM-derived result in both ascending and descending mode, "
        "confirmed by the exhaustive sweep above completing to its full "
        "asserted count (" + std::to_string(expected_checks) + " cases) "
        "rather than silently stopping early - 'a block-move source shift "
        "can be from one through fifteen bits' (6-intro.c11, p.169), 'The "
        "shift applied to a source area is from one to fifteen bits' "
        "(ch6-intro.c10, p.169), '[DMA channels] have a barrel shifter "
        "that can shift an image between 0 and [...] 15 bits' "
        "(6-shifts-and-masks.c1, 6-shifts-and-masks.c2, p.179), 'the "
        "barrel shifter permits pixel-boundary positioning despite "
        "word-addressed bitmap data' (6-shifts-and-masks.c5, p.179 - "
        "entailed by the same 0-to-15 range, not a separate mechanism), "
        "'both the A and B DMA channels have a barrel shifter' "
        "(ch6-shifts.c1, p.179) and 'the barrel shifter can shift an "
        "image by 0 to 15 bits' (ch6-shifts.c2, p.179) - the RTL's own "
        "header comment confirms this one generic module is what both "
        "channels instantiate ('This module can shift 0-15 positions', "
        "agnus_blitter_barrelshifter.v:2).");

    // ---------------- RIGHT-ASCENDING (measured, overturned from a discard) ----------------
    //
    // ch6-keypoints.c16 was originally discarded and overturned by Prompt 5
    // (attack, 08.09.2026, a different model), which built its own
    // throwaway Verilator harness against this exact module and measured
    // these two cases directly, before this package existed. Reproduced
    // here as named, cited assertions against the real fixture rather than
    // left as a discard overturned only in the classification record.
    {
        dut.desc = 0;
        dut.shift = 4;
        dut.old_val = 0x0000;
        dut.new_val = 0x8000;
        dut.eval();
        const auto out_a = static_cast<std::uint16_t>(dut.out);

        dut.desc = 0;
        dut.shift = 1;
        dut.old_val = 0x0000;
        dut.new_val = 0x0002;
        dut.eval();
        const auto out_b = static_cast<std::uint16_t>(dut.out);

        named.expect(out_a == 0x0800 && out_b == 0x0001,
            "RIGHT-ASCENDING: with desc=0, a marked bit moves toward the "
            "low end of the word - measured directly: desc=0, shift=4, "
            "new_val=0x8000 -> out=0x0800 (bit 15 to bit 11) and desc=0, "
            "shift=1, new_val=0x0002 -> out=0x0001 (bit 1 to bit 0) - "
            "confirming 'modulos, and shifts to the right' "
            "(ch6-keypoints.c16, p.200), 'The shift is normally towards "
            "the right' (6-shifts-and-masks.c4, ch6-shifts.c4, p.179). "
            "ch6-keypoints.c16 was overturned from a not-asserted discard "
            "by Prompt 5's attack stage on 08.09.2026.");
    }

    // ---------------- LEFT-DESCENDING (measured, overturned from a discard) ----------------
    {
        dut.desc = 1;
        dut.shift = 1;
        dut.old_val = 0x0000;
        dut.new_val = 0x4000;
        dut.eval();
        const auto out_a = static_cast<std::uint16_t>(dut.out);

        dut.desc = 1;
        dut.shift = 4;
        dut.old_val = 0x0000;
        dut.new_val = 0x0008;
        dut.eval();
        const auto out_b = static_cast<std::uint16_t>(dut.out);

        named.expect(out_a == 0x8000 && out_b == 0x0080,
            "LEFT-DESCENDING: with desc=1, a marked bit moves toward the "
            "high end of the word - measured directly: desc=1, shift=1, "
            "new_val=0x4000 -> out=0x8000 (bit 14 to bit 15) and desc=1, "
            "shift=4, new_val=0x0008 -> out=0x0080 (bit 3 to bit 7) - "
            "confirming 'the modulos, and shifts to the left' "
            "(ch6-keypoints.c19, p.200), 'Shifts are then towards the "
            "left, rather than the right' (6-descending-mode.c5, "
            "ch6-desc.c6, p.182). ch6-keypoints.c19 was overturned from a "
            "not-asserted discard by Prompt 5's attack stage on "
            "08.09.2026.");
    }

    // ---------------- STRUCTURE-WINDOW ----------------
    //
    // Figure 6-13 (p.199) draws the shifter as a 32-bit register split
    // into an "old" and a "new" 16-bit half, with a 16-bit output taken
    // from a position within that combination (also stated in prose on
    // p.198). This module's own port list is exactly that structure: a
    // 16-bit old_val, a 16-bit new_val, and a 16-bit out selected by shift
    // (agnus_blitter_barrelshifter.v:6-13). The case below picks a shift
    // where the output genuinely straddles both halves - not a degenerate
    // shift of 0, where old_val would not matter at all - to show both
    // halves are actually combined into the window, not that one merely
    // happens to pass through untouched.
    {
        dut.desc = 0;
        dut.shift = 8;
        dut.old_val = 0x00AB;
        dut.new_val = 0xCD00;
        dut.eval();
        const auto actual = static_cast<std::uint16_t>(dut.out);
        const auto expected = hrm_oracle(false, 8, 0x00AB, 0xCD00);

        named.expect(actual == expected && actual == 0xABCD,
            "STRUCTURE-WINDOW: old_val=0x00AB, new_val=0xCD00, shift=8, "
            "desc=0 -> out=0xABCD - the high byte (0xAB) comes from "
            "old_val's low byte and the low byte (0xCD) from new_val's "
            "high byte, i.e. the output is a genuine 16-bit window "
            "straddling both 16-bit halves of one 32-bit combination, "
            "positioned by shift - 'bits of data is taken from a "
            "specified position within a 32-bit [register]' "
            "(ch6-blockdiag.c5, p.198), Figure 6-13's 'A-old and the new "
            "masked A word as the two 16-bit halves of a 32-bit A "
            "shifter' (6-blitter-block-diagram.c3, p.199), 'B-old and "
            "B-new as the two 16-bit halves of a 32-bit B shifter' "
            "(6-blitter-block-diagram.c4, p.199), \"'B new' feeds a "
            "register 'B old', and both feed a 'SHIFTER (32-BIT)'\" "
            "(ch6-fig613.f08, p.199), \"the B shifter's output is "
            "dimensioned '16 bits' taken from the 32-bit register\" "
            "(ch6-fig613.f10, p.199) and \"'A old' feeds a second "
            "'SHIFTER (32-BIT)' driven by 'SHIFT A VALUE', also "
            "dimensioned '16 bits'\" (ch6-fig613.f11, p.199) - this one "
            "generic module is the box the figure draws once per "
            "channel.");
    }

    // ---------------- SHIFT-VALUE-SELECTS ----------------
    //
    // Figure 6-13 draws "SHIFT A VALUE" / "SHIFT B VALUE" entering the
    // shifter box from "Blitter Control" (i.e. BLTCON0/BLTCON1), and
    // p.198's prose says the position is "based on the A shift or B shift
    // values shown in BLTCON0 and BLTCON1". This module receives that
    // value as a plain input port (`shift`, agnus_blitter_barrelshifter.v:
    // 9) with no clock and no internal counter of its own - driving three
    // different values on the same dut instance, with no clock edge
    // between them, and observing the output move each time demonstrates
    // that `shift`'s numeric value alone (not any state this module keeps)
    // selects the window position, exactly as an externally-supplied
    // "Blitter Control" value would have to.
    {
        dut.desc = 0;
        dut.old_val = 0x0000;
        dut.new_val = 0x0080;

        dut.shift = 0;
        dut.eval();
        const auto out_s0 = static_cast<std::uint16_t>(dut.out);
        dut.shift = 7;
        dut.eval();
        const auto out_s7 = static_cast<std::uint16_t>(dut.out);
        dut.shift = 15;
        dut.eval();
        const auto out_s15 = static_cast<std::uint16_t>(dut.out);

        named.expect(out_s0 == 0x0080 && out_s7 == 0x0001 && out_s15 == 0x0000,
            "SHIFT-VALUE-SELECTS: the same new_val=0x0080 (bit 7) with "
            "old_val=0x0000, desc=0, driven at shift=0, then shift=7, "
            "then shift=15 with no clock edge between them, produces "
            "out=0x0080, then out=0x0001, then out=0x0000 in lockstep "
            "with shift's own value - 'the shifter position [is] based "
            "on the A shift or B shift values shown in BLTCON0 and "
            "BLTCON1' (ch6-blockdiag.c6, p.198), Figure 6-13 shows 'the "
            "A and B shift values entering their respective shifters "
            "from blitter control' (6-blitter-block-diagram.c5, p.199) "
            "and 'the B shifter is driven by \"SHIFT B VALUE\", "
            "annotated \"(Blitter Control)\"' (ch6-fig613.f09, p.199).");
    }

    // ---------------- MASK-UPSTREAM (partial claim - see Non-goals) ----------------
    //
    // Figure 6-13 shows three A-path AND gates (the masks) feeding an OR
    // gate whose output becomes "A old", i.e. the masking happens BEFORE
    // the value reaches the shifter box (ch6-fig613.f06, p.199). The
    // AND-gate mechanics themselves are not constructible here - this
    // module has no mask-related port at all (agnus_blitter_
    // barrelshifter.v:6-13) - but the other half of the same claim, that
    // the SHIFTER performs no further masking of what it is handed, is
    // directly testable: at shift=0 the module returns new_val exactly
    // (agnus_blitter_barrelshifter.v:76, shifted_new[31:16] with
    // shift_onehot's top bit set), so driving old_val to its two extremes
    // while new_val is held fixed must never change the result if old_val
    // is never combined with it. This is the same case the very first
    // section of this file's exhaustive sweep already runs
    // (zero_shift_previous = {0x0000, 0xFFFF}, all 65536 values of
    // `current`); reasserted here by name and tied to its citation.
    {
        dut.desc = 0;
        dut.shift = 0;
        dut.new_val = 0x1234;

        dut.old_val = 0x0000;
        dut.eval();
        const auto out_lo = static_cast<std::uint16_t>(dut.out);

        dut.old_val = 0xFFFF;
        dut.eval();
        const auto out_hi = static_cast<std::uint16_t>(dut.out);

        named.expect(out_lo == 0x1234 && out_hi == 0x1234,
            "MASK-UPSTREAM (partial claim): with shift=0, out=0x1234 "
            "regardless of whether old_val is 0x0000 or 0xFFFF - the "
            "shifter performs no ANDing or masking of new_val against "
            "old_val, consistent with Figure 6-13 showing the masks "
            "applied BEFORE the value reaches 'A old' and the shifter "
            "(ch6-fig613.f06, p.199). Only this half of f06 is asserted "
            "here; the AND-gate mechanics that produce 'A old' in the "
            "first place are upstream of this module's port list and are "
            "PKG-BLT-MASKS' territory, not asserted here.");
    }

    dut.final();

    // The sweep's count, reported as a measurement: RANGE above is its graded
    // form, and a second PASS:/FAIL: line for the same condition would count
    // it twice. Gated on the real count so a shrunk sweep - the exact thing
    // RANGE's own mutation drives - cannot leave the full figure on screen.
    if (checks == expected_checks) {
        std::puts(HRM_STRINGIFY(
            MEASURED: 4197376 HRM-derived blitter barrel shifter cases));
    } else {
        std::printf("MEASURED: exhaustive sweep completed %zu cases, expected %zu\n",
                    checks, expected_checks);
    }
    std::printf("%d passed, %d failed (named, cited assertions)\n",
                named.passed, named.failed);
    return named.failed == 0 ? 0 : 1;
}
