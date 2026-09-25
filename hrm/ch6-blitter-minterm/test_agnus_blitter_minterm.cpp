// Test purpose: Verify the blitter's minterm function generator
//   (agnus_blitter_minterm) computes, for every one of the 256 possible LF
//   control-byte values and for both isolated selectors and mixed 16-bit
//   source words, exactly the Boolean function of A, B and C that the
//   manual's truth table, minterm numbering, Venn diagram and worked
//   examples describe -- including the two named line-mode functions and the
//   "cookie-cut" function used elsewhere in the chapter.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 169, 174, 175, 176, 177, 178, 179, 181, 191, 192, 198, 199 and 268.
// Non-goals: where the LF byte comes from (BLTCON0[7:0] decoding -
//   PKG-BLT-REGDECODE's territory; ch6-fig613.f27 below), what happens to
//   the minterm output afterwards (fill logic, PKG-BLT-FILL's territory;
//   ch6-fig613.f20/f21 below), and the A/B/C hold-register mechanism that
//   places values on ain/bin/cin before this module sees them
//   (ch6-fig613.f13 below) - none of that is inside this leaf module's port
//   list (lf, ain, bin, cin, out - agnus_blitter_minterm.v:7-14).
// Claim ids: ids such as `6-function-generator.c2` and `ch6-funcgen.c1` name
//   one sentence of the manual in this chapter's reading. They carry no reader
//   prefix, and they resolve in the claim files that
//   hrm-testpipeline/acceptance/ch6.json lists under `inventories`.
//
// EXTENDS PRIOR ART (hrm/ch6-blitter-minterm, unchanged in shape since its
// original commit; see docs/evidence/blitter-prior-art.md). What was already here and
// is kept, verbatim in mechanism: the `hrm_oracle()` function computing the
// expected output from the manual's bit-selection description rather than
// from the RTL; the exhaustive first stage driving A, B and C as all-zero or
// all-one words so each of the 256 LF bytes is checked against each of the 8
// selectors with every LF bit separately confirmed to reach the output
// (2048 cases); the second stage using 0xF0F0/0xCCCC/0xAAAA so the 16 bit
// positions of one word enumerate all eight selectors twice and the output
// must equal lf|(lf<<8) (256 cases); the independent hrm_oracle() sweep
// against three mixed 16-bit source-word patterns for every LF byte (768
// cases); and the ten-case run_hrm_examples() worked-example check ($80,
// $F0, $0F, $CC, $AA, $FC, $C0, $CA, $00, $FF) driven against each of those
// three mixed patterns.
//
// What the pipeline added:
//   - `Test class:` / `Primary oracle:` header (prior art had neither).
//   - the exhaustive/packed/mixed sweeps and the ten worked-example cases are
//     now reported through named, individually cited PASS/FAIL lines instead
//     of one collective "PASS: N cases" line with no per-claim citation -
//     the sweep totals are unchanged, only how they are reported.
//   - 8 TRUTHTABLE-POS-n assertions isolate one truth-table row each
//     (p.174's table, p.175's positional cross-reference and p.178's Venn
//     diagram all state the same 8 facts; folded together here rather than
//     asserted 3 times over).
//   - MECHANISM-ALLOW-INHIBIT and BIT-INDEPENDENCE isolate two structural
//     claims from figure 6-13 (p.199) that the sweeps prove but do not tell
//     legibly on their own: that each LF bit independently allows or
//     inhibits exactly its own minterm, and that the 16 output bits are 16
//     independent copies of the same logic ("16 BLOCKS" in the figure), not
//     one shared computation.
//   - 29 TABLE61-$xx assertions, one per distinct LF code in table 6-1
//     (p.177) other than $CA, each checking the code against its own named
//     Boolean equation - not against the generic hrm_oracle() bit-selector,
//     which cannot distinguish a right code from a wrong name for it.
//   - a COOKIE-CUT assertion ($CA, p.176/p.181/p.177/p.191-192) and two
//     LINE-MODE-4A assertions (p.191-192/p.268), separated from the general
//     table because they are named, cross-referenced functions in their own
//     right, not just another row.
//   - a stimulus `// MUTATION` record for every assertion group (prior art
//     had none).
//
// THE CROSS-READ FOUND 5 ARITHMETIC DEFECTS in restatements of the p.177
// LF-code table; 4 of the 5 are claims of this package and are named here:
// 6-function-generator.c62, c66, c67 and c70. Each restatement drops one
// overbar from the printed equation (verified independently below, not
// taken on the cross-read's word alone - see the arithmetic in the
// TABLE61-$3F/$5F/$FA/$77 assertions' comments):
//   c62 says $3F = ~A|B; the printed table's overbar sits over BOTH letters
//     on that line ("A-bar + B-bar"), so $3F = ~A|~B. ~A|B is $CF, a
//     different cell of the same table (kept correct in c65/ch6-t61.left.CF).
//   c66 says $5F = ~A|C; same defect, both letters carry the overbar, so
//     $5F = ~A|~C. ~A|C is $AF, a different cell (kept correct in
//     c69/ch6-t61.left.AF).
//   c67 says $FA = A|~B; the printed line for $FA carries NO overbar at all
//     ("D = A + C" plain), so $FA = A|C. A|~B is $F3, a different cell
//     (kept correct in c60/ch6-t61.right.F3).
//   c70 says $77 = ~B|C; same defect as c62/c66, so $77 = ~B|~C. ~B|C is
//     $BB, a different cell (kept correct in c73/ch6-t61.left.BB).
// Per the task brief, every TABLE61 assertion below is built from
// `extract.quote` (the hex code, verbatim and undisputed) plus the
// page-174-derived truth-table arithmetic (independently recomputed in this
// file, not copied from either reader), never from a restatement. Where a
// sol restatement disagrees with that arithmetic, the assertion message says
// so explicitly and the quote/arithmetic wins. Every opus ch6-t61.* claim
// covering the same 4 cells was independently checked the same way and found
// arithmetically correct, so the correct equation is also the one both
// readers converge on once cross-read. (The cross-read's fifth reported
// defect concerns a claim outside PKG-BLT-MINTERM's 182 testable claims and
// is not addressed by this file.)
//
// CLAIM COVERAGE, for the 5 of 182 claims with no assertion of their own
// (reported here, not silently dropped):
//   ch6-minterms.c10 (p.176, "This mask can be used in turn on [each
//     background bitplane]") describes a multi-bitplane looping convention
//     in software, not a hardware behaviour of this module - agnus_blitter_
//     minterm.v has no notion of "which bitplane" and applies the same
//     combinational function regardless. Not constructible at this
//     boundary. Routed record:ch6-blitter-minterm; printed as a RECORDED
//     line by main() below, not left in this comment alone.
//   ch6-fig613.f13 (p.199, figure 6-13: "A hold", "B hold", "C hold"
//     register the shifted/plain sources before this module sees them)
//     describes the mechanism that PLACES values on ain/bin/cin
//     (agnus_blitter.v, outside this module). What this module does with
//     WHATEVER is placed there is exhaustively covered by every sweep
//     below; the upstream hold-register mechanism itself is not
//     constructible at agnus_blitter_minterm's boundary.
//   ch6-fig613.f20 (p.199, figure 6-13: the minterm OR output XORed with the
//     fill carry) and ch6-fig613.f21 (p.199, figure 6-13: the fill-enable
//     switch between the unfilled and XORed paths) describe the FILL stage
//     that consumes this module's `out` - agnus_blitter_minterm has no fill
//     ports (ife, efe, fci, fco - none of them are in this module's port
//     list, agnus_blitter_minterm.v:7-14). PKG-BLT-FILL's territory.
//   ch6-fig613.f27 (p.199, figure 6-13: the "Blitter Control" block supplies
//     the multi-minterm select bits, i.e. `lf`, from BLTCON0) describes
//     where `lf` comes from, not what this module does with it once
//     supplied. PKG-BLT-REGDECODE's territory.
//
// Every mutation below was actually applied (edit -> rebuild -> run -> revert
// -> rebuild-clean) under WSL/Verilator and the PASS/FAIL lines quoted are
// what the run printed, not predictions. Clean baseline: "56 passed, 0
// failed".
//
// MUTATION SWEEP-EXHAUSTIVE-SELECTORS stimulus red: inside the exhaustive
//   selector sweep, the word actually driven onto ain/bin/cin was built from
//   `selector ^ 1` while `expected` stayed computed from the original
//   `selector` -> printed "FAIL: SWEEP-EXHAUSTIVE-SELECTORS ... first
//   mismatch: LF=$01 selector=0 expected D=$FFFF actual D=$0000", and because
//   CAPABILITY-256-FUNCTIONS reuses the same sweep result it FAILED too, with
//   the same first-mismatch text; every other assertion PASS. Totalled
//   "54 passed, 2 failed" for that run.
// MUTATION CAPABILITY-256-FUNCTIONS stimulus red: the same edit as the
//   SWEEP-EXHAUSTIVE-SELECTORS record immediately above (`selector ^ 1` driven
//   onto ain/bin/cin while `expected` stayed computed from the original
//   `selector`) -> CAPABILITY-256-FUNCTIONS reads the identical
//   `exhaustive.mismatches` counter as SWEEP-EXHAUSTIVE-SELECTORS, so the same
//   run printed "FAIL: CAPABILITY-256-FUNCTIONS ... first mismatch: LF=$01
//   selector=0 expected D=$FFFF actual D=$0000" alongside it; every other
//   assertion PASS. Same "54 passed, 2 failed" totals as above -- one edit,
//   two assertions, recorded once each per their own ids.
// MUTATION SWEEP-PACKED-SELECTOR-ORDER stimulus red: in the packed-selector-order
//   sweep, the LF value driven onto the DUT was `lf ^ 0x01` while `expected`
//   stayed computed as `lf | (lf << 8)` from the ORIGINAL lf -> printed
//   "FAIL: SWEEP-PACKED-SELECTOR-ORDER ... first mismatch: LF=$00 expected
//   D=$0000 actual D=$0101"; every other assertion PASS. Totalled
//   "55 passed, 1 failed".
// MUTATION SWEEP-MIXED-WORDS stimulus red: in the mixed-word sweep,
//   the word pattern actually driven was taken from `kPatterns[(idx+1) %
//   kPatterns.size()]` while hrm_oracle() was still called with `kPatterns
//   [idx]` -> printed "FAIL: SWEEP-MIXED-WORDS ... first mismatch: LF=$01
//   pattern=0 expected D=$8008 actual D=$4000"; every other assertion PASS.
//   Totalled "55 passed, 1 failed".
// MUTATION CASE-COUNT-EXHAUSTIVE stimulus red: a value-mutation cannot turn a
//   CASE-COUNT-* assertion red, since it recomputes its own expectation per
//   case and a wrong VALUE is not a wrong COUNT (confirmed by the three sweep
//   mutations above, none of which moved any CASE-COUNT-* assertion). The
//   correspondence a CASE-COUNT-* assertion actually checks is "the loop ran
//   as many times as it should have" -- so the stimulus mutation that can
//   break it is a skipped case: the exhaustive loop's first iteration
//   (LF=$00, selector=0) was skipped with `if (lf == 0 && selector == 0)
//   continue;` before the case is counted or compared -> printed "FAIL:
//   CASE-COUNT-EXHAUSTIVE: 2047 of 2048 expected cases actually ran"; because
//   the skipped case is never compared at all (not compared-and-wrong, simply
//   never run), SWEEP-EXHAUSTIVE-SELECTORS and CAPABILITY-256-FUNCTIONS -
//   which only count mismatches, not cases - stayed PASS; every other
//   assertion PASS. Totalled "55 passed, 1 failed".
// MUTATION CASE-COUNT-PACKED stimulus red: same reasoning and shape as
//   CASE-COUNT-EXHAUSTIVE immediately above, applied to the packed-selector-
//   order sweep: its first iteration (LF=$00) was skipped with `if (lf == 0)
//   continue;` before the case is counted or compared -> printed "FAIL:
//   CASE-COUNT-PACKED: 255 of 256 expected cases actually ran";
//   SWEEP-PACKED-SELECTOR-ORDER stayed PASS (the skipped case was never
//   compared, not compared-and-wrong); every other assertion PASS. Totalled
//   "55 passed, 1 failed".
// MUTATION CASE-COUNT-MIXED stimulus red: same reasoning and shape again,
//   applied to the mixed-word sweep: its first iteration (pattern idx=0,
//   LF=$00) was skipped with `if (idx == 0 && lf == 0) continue;` before the
//   case is counted or compared -> printed "FAIL: CASE-COUNT-MIXED: 767 of
//   768 expected cases actually ran"; SWEEP-MIXED-WORDS stayed PASS (the
//   skipped case was never compared, not compared-and-wrong); every other
//   assertion PASS. Totalled "55 passed, 1 failed".
// MUTATION TRUTHTABLE-POS-3-PROBE stimulus red: only entry k=3 in the
//   truth-table probe table had its driven (a,b,c) word swapped for k=2's
//   pattern (A=0,B=1,C=0) while `lf` stayed `1<<3` and the expected output
//   stayed 0xFFFF -> printed "FAIL: TRUTHTABLE-POS-3 ... actual D=$0000" and
//   no other TRUTHTABLE-POS-n assertion affected. Totalled
//   "55 passed, 1 failed".
// MUTATION TABLE61-$3F stimulus red: only the $3F entry in the
//   named-function table had its driven `lf` changed to $CF while the
//   equation lambda (~A|~B, the code proper to $3F) was left untouched ->
//   printed "FAIL: TABLE61-$3F ... first mismatch: A=$5AA5 B=$3CC3 C=$0FF0
//   expected D=$E77E actual D=$BDDB" and no other TABLE61/WORKED/COOKIE-CUT/
//   LINE-MODE assertion affected (in particular TABLE61-$CF, which drives
//   the genuine $CF row, stayed PASS since it is a separate table entry).
//   Totalled "55 passed, 1 failed".
// MUTATION COOKIE-CUT stimulus red: the COOKIE-CUT entry's driven
//   `lf` was changed from $CA to $CB while the equation ((A&B)|(~A&C))
//   stayed untouched -> printed "FAIL: COOKIE-CUT ... first mismatch: A=$5AA5
//   B=$3CC3 C=$0FF0 expected D=$1DD1 actual D=$9DD9"; every other assertion
//   PASS. Totalled "55 passed, 1 failed".
// MUTATION LINE-MODE-4A-EQUATION stimulus red: the LINE-MODE-4A-EQUATION
//   entry's driven `lf` was changed from $4A to $4B while the equation
//   ((~A&C)|(A&B&~C)) stayed untouched -> printed "FAIL:
//   LINE-MODE-4A-EQUATION ... first mismatch: A=$5AA5 B=$3CC3 C=$0FF0
//   expected D=$1551 actual D=$9559"; every other assertion PASS. Totalled
//   "55 passed, 1 failed".
// MUTATION LINE-MODE-TERM-PASSTHROUGH stimulus red: two attempts, both
//   recorded. First attempt: LINE-MODE-TERM-PASSTHROUGH's "A all-zero" case
//   had `ain` changed from 0x0000 to 0x0001 while the expected value stayed
//   `cin`=$5678 unchanged -> the run printed "56 passed, 0 failed", NOT red.
//   Diagnosis before treating that as a defect in the assertion: bit 0 of
//   the fixed bin=$1234 and cin=$5678 are both 0, so the mutated term
//   (b0 & ~c0 = 0) happened to equal cin's own bit 0 (0) - a vacuous
//   stimulus, not a broken assertion (the same caution the fill/status
//   packages record: a survivor, or here a non-survivor, is a hypothesis
//   until the mechanism is checked). Second attempt, kept: `ain` changed to
//   0x0004 (bit 2, where bin=$1234 has a 1 and cin=$5678 has a 0) -> printed
//   "FAIL: LINE-MODE-TERM-PASSTHROUGH ... actual D=$567C"; LINE-MODE-TERM-
//   INVERT (the A=all-one case, untouched) stayed PASS. Totalled
//   "55 passed, 1 failed".
// MUTATION LINE-MODE-TERM-INVERT stimulus red: the sibling of the
//   LINE-MODE-TERM-PASSTHROUGH mutation immediately above, applied to the
//   OTHER half of the $4A equation. In the "A entirely 1" case, the word
//   actually driven onto `bin` had bit 2 flipped (0x1234 ^ 0x0004 = 0x1230,
//   chosen because bin's bit 2 is 1 and cin's bit 2 is 0, so the flip is
//   non-vacuous the same way the PASSTHROUGH mutation's kept second attempt
//   was) while `expected` stayed computed from the ORIGINAL bin=$1234 ->
//   printed "FAIL: LINE-MODE-TERM-INVERT ... actual D=$0000" (expected was
//   $0004, i.e. bin & ~cin with bit 2 set); LINE-MODE-TERM-PASSTHROUGH (the
//   A=all-zero case, untouched) stayed PASS; every other assertion PASS.
//   Totalled "55 passed, 1 failed".
// MUTATION MECHANISM-ALLOW-INHIBIT stimulus red: in the allow/inhibit probe, the
//   driven `lf` for every k became `(1<<k) | (1<<((k+1)%8))` (two bits set
//   instead of one) while the expected pattern stayed "only selector k reads
//   1" -> printed "FAIL: MECHANISM-ALLOW-INHIBIT ... first mismatch: k=0
//   selector=1 expected D=$0000 actual D=$FFFF"; every other assertion PASS.
//   Totalled "55 passed, 1 failed".
// MUTATION BIT-INDEPENDENCE stimulus red: the second BIT-
//   INDEPENDENCE probe drove the differing input bit at position 6 (instead
//   of 0) while the comparison still checked that ONLY bit 0 of the output
//   differed -> printed "FAIL: BIT-INDEPENDENCE ... actual changed
//   bits=$0040"; every other assertion PASS. Totalled "55 passed, 1 failed".
//
// After every mutation above, the file was reverted and rebuilt clean; the
// final clean run prints "56 passed, 0 failed".

#include "Vagnus_blitter_minterm.h"
#include "verilated.h"

#include <array>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

// HRM: bit `selector` of the LF byte (selector = 4*A + 2*B + C, per the
// truth table on p.174) gates whether that (A,B,C) combination sets the
// destination bit. This function computes the expected output from that
// bit-selection rule alone, independent of the RTL (agnus_blitter_minterm.v
// implements the same rule with eight named minterm regs; every assertion
// below checks the RTL against an oracle, never the reverse).
std::uint16_t hrm_oracle(
    std::uint8_t lf,
    std::uint16_t a,
    std::uint16_t b,
    std::uint16_t c) {
    std::uint16_t expected = 0;
    for (unsigned bit = 0; bit < 16; ++bit) {
        const unsigned selector =
            (((a >> bit) & 1U) << 2U) |
            (((b >> bit) & 1U) << 1U) |
            ((c >> bit) & 1U);
        const std::uint16_t result_bit =
            static_cast<std::uint16_t>((lf >> selector) & 1U);
        expected |= static_cast<std::uint16_t>(result_bit << bit);
    }
    return expected;
}

std::string hex2(unsigned v) {
    std::ostringstream os;
    os << "$" << std::hex << std::uppercase << std::setw(2)
       << std::setfill('0') << (v & 0xFFU);
    return os.str();
}

std::string hex4(unsigned v) {
    std::ostringstream os;
    os << "$" << std::hex << std::uppercase << std::setw(4)
       << std::setfill('0') << (v & 0xFFFFU);
    return os.str();
}

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

struct WordPattern {
    std::uint16_t a;
    std::uint16_t b;
    std::uint16_t c;
};

// Three arbitrary, non-degenerate 16-bit source-word patterns, shared by the
// mixed-word sweep, the worked examples and the named-function checks below.
const std::array<WordPattern, 3> kPatterns{{
    {0x5AA5U, 0x3CC3U, 0x0FF0U},
    {0x1357U, 0x2468U, 0xAAAAU},
    {0xF00FU, 0x55AAU, 0xC33CU},
}};

using Equation = std::function<std::uint16_t(std::uint16_t, std::uint16_t, std::uint16_t)>;

struct NamedFunctionEntry {
    std::uint8_t lf;
    std::string label;      // e.g. "TABLE61-$3F"
    std::string equation_text;
    Equation fn;
    std::string page_and_ids;
    std::string note;       // usually empty
};

// Runs one named-function entry against all three kPatterns and reports one
// PASS/FAIL line citing every covering claim id. `lf_override`, when
// nonzero-signalled via `override_lf`, drives a DIFFERENT LF code than
// `entry.lf` while `entry.fn` (the expected computation) stays tied to the
// entry's own equation - this is the hook the stimulus mutations above use;
// in the clean baseline every call site passes `override_lf = -1` (no
// override).
bool run_named_function_check(
    Vagnus_blitter_minterm& dut,
    Checks& checks,
    const NamedFunctionEntry& entry,
    int override_lf = -1) {
    const std::uint8_t driven_lf =
        override_lf >= 0 ? static_cast<std::uint8_t>(override_lf) : entry.lf;
    bool ok = true;
    std::string mismatch;
    for (const auto& p : kPatterns) {
        dut.lf = driven_lf;
        dut.ain = p.a;
        dut.bin = p.b;
        dut.cin = p.c;
        dut.eval();
        const auto actual = static_cast<std::uint16_t>(dut.out);
        const std::uint16_t expected = entry.fn(p.a, p.b, p.c);
        if (actual != expected) {
            ok = false;
            std::ostringstream os;
            os << "first mismatch: A=" << hex4(p.a) << " B=" << hex4(p.b)
               << " C=" << hex4(p.c) << " expected D=" << hex4(expected)
               << " actual D=" << hex4(actual);
            mismatch = os.str();
            break;
        }
    }
    checks.expect(ok,
        entry.label + ": LF=" + hex2(entry.lf) + " implements " +
        entry.equation_text + " (" + entry.page_and_ids + ")" +
        (entry.note.empty() ? "" : " -- " + entry.note) +
        (ok ? "" : " -- " + mismatch));
    return ok;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus_blitter_minterm dut;
    Checks checks;

    // ---------------- exhaustive selector sweep (kept from prior art) ----------------
    //
    // Drives A, B and C as all-zero or all-one words for every one of the 8
    // selectors and every one of the 256 LF bytes (2048 cases), so each LF
    // bit is separately confirmed to reach the output. This is the strongest
    // single piece of evidence in the file and backs several claim clusters
    // below by citation.
    struct SweepResult {
        std::size_t cases = 0;
        std::size_t mismatches = 0;
        std::string first_failure;
    };

    SweepResult exhaustive;
    for (unsigned lf = 0; lf <= 0xFFU; ++lf) {
        for (unsigned selector = 0; selector < 8U; ++selector) {
            // MUTATION HOOK (normally inert): no case skipped here.
            // during MUTATION CASE-COUNT-EXHAUSTIVE: `if (lf == 0 && selector == 0) continue;`
            // MUTATION HOOK (normally inert): driven_selector == selector.
            const unsigned driven_selector = selector;  // ^ 1 during MUTATION SWEEP-SELECTOR-DECOUPLE
            const std::uint16_t a =
                (driven_selector & 0x4U) != 0U ? 0xFFFFU : 0x0000U;
            const std::uint16_t b =
                (driven_selector & 0x2U) != 0U ? 0xFFFFU : 0x0000U;
            const std::uint16_t c =
                (driven_selector & 0x1U) != 0U ? 0xFFFFU : 0x0000U;
            const std::uint16_t expected =
                ((lf >> selector) & 1U) != 0U ? 0xFFFFU : 0x0000U;

            dut.lf = static_cast<std::uint8_t>(lf);
            dut.ain = a;
            dut.bin = b;
            dut.cin = c;
            dut.eval();
            const auto actual = static_cast<std::uint16_t>(dut.out);
            ++exhaustive.cases;
            if (actual != expected) {
                ++exhaustive.mismatches;
                if (exhaustive.first_failure.empty()) {
                    std::ostringstream os;
                    os << "first mismatch: LF=" << hex2(lf)
                       << " selector=" << selector
                       << " expected D=" << hex4(expected)
                       << " actual D=" << hex4(actual);
                    exhaustive.first_failure = os.str();
                }
            }
        }
    }

    checks.expect(exhaustive.mismatches == 0,
        "SWEEP-EXHAUSTIVE-SELECTORS: exhaustively confirmed across all 256 LF "
        "bytes x 8 selectors (2048 cases) - each LF bit independently reaches "
        "the output exactly where its own (A,B,C) selector is presented "
        "(agnus_blitter_minterm.v:32-44) (p.174, 'up to 256 different ways to "
        "generate the values stored by the destination', 6-function-generator.c2, "
        "ch6-funcgen.c1)" +
        (exhaustive.mismatches ? " -- " + exhaustive.first_failure : ""));

    checks.expect(exhaustive.mismatches == 0,
        "CAPABILITY-256-FUNCTIONS: the same exhaustive sweep, read as a whole, "
        "demonstrates the LF byte selects one of 256 distinct logic functions "
        "of up to three one-bit sources (p.169, 'In block move mode, the "
        "blitter can perform any logical operation on up to [three source "
        "areas]', 6-intro.c8, 6-intro.c9, ch6-intro.c8; p.174, 'blitter to "
        "perform one of the 256 possible logic operations on three [sources]', "
        "6-function-generator.c23, ch6-funcgen.c5)" +
        (exhaustive.mismatches ? " -- " + exhaustive.first_failure : ""));

    checks.expect(exhaustive.cases == 2048U,
        "CASE-COUNT-EXHAUSTIVE: " + std::to_string(exhaustive.cases) +
        " of 2048 expected cases actually ran - a loop that silently stops "
        "early must fail this, not merely fail to increment a pass count");

    // ---------------- packed selector-order sweep (kept from prior art) ----------------
    //
    // 0xF0F0/0xCCCC/0xAAAA make the 16 bit positions of one word enumerate
    // all eight selectors twice (bits 0-7 = selectors 0-7, bits 8-15 =
    // selectors 0-7 again), so the output must equal lf | (lf << 8) for
    // every LF byte. This is also what figure 6-13 shows: eight minterm
    // outputs, one per selector, produced together (p.199).
    SweepResult packed;
    for (unsigned lf = 0; lf <= 0xFFU; ++lf) {
        // MUTATION HOOK (normally inert): no case skipped here.
        // during MUTATION CASE-COUNT-PACKED: `if (lf == 0) continue;`
        // MUTATION HOOK (normally inert): driven_lf == lf.
        const unsigned driven_lf = lf;  // ^ 0x01 during MUTATION SWEEP-PACKED-OFFSET
        const auto expected = static_cast<std::uint16_t>(lf | (lf << 8U));
        dut.lf = static_cast<std::uint8_t>(driven_lf);
        dut.ain = 0xF0F0U;
        dut.bin = 0xCCCCU;
        dut.cin = 0xAAAAU;
        dut.eval();
        const auto actual = static_cast<std::uint16_t>(dut.out);
        ++packed.cases;
        if (actual != expected) {
            ++packed.mismatches;
            if (packed.first_failure.empty()) {
                std::ostringstream os;
                os << "first mismatch: LF=" << hex2(lf)
                   << " expected D=" << hex4(expected)
                   << " actual D=" << hex4(actual);
                packed.first_failure = os.str();
            }
        }
    }

    checks.expect(packed.mismatches == 0,
        "SWEEP-PACKED-SELECTOR-ORDER: with A=$F0F0/B=$CCCC/C=$AAAA the 16 bit "
        "positions enumerate all eight selectors twice, and the output equals "
        "lf|(lf<<8) for all 256 LF bytes (256 cases) - matching figure 6-13's "
        "minterm generator producing all eight A/B/C minterms together and "
        "labelling them left to right in selector order (p.199, "
        "6-blitter-block-diagram.c6, ch6-fig613.f14, ch6-fig613.f15)" +
        (packed.mismatches ? " -- " + packed.first_failure : ""));

    checks.expect(packed.cases == 256U,
        "CASE-COUNT-PACKED: " + std::to_string(packed.cases) +
        " of 256 expected cases actually ran");

    // ---------------- mixed-word sweep against hrm_oracle() (kept from prior art) ----------------
    SweepResult mixed;
    for (std::size_t idx = 0; idx < kPatterns.size(); ++idx) {
        // MUTATION HOOK (normally inert): driven_idx == idx.
        const std::size_t driven_idx = idx;  // (idx+1)%kPatterns.size() during MUTATION SWEEP-MIXED-PATTERN-SWAP
        const auto& driven = kPatterns[driven_idx];
        const auto& oracle_pattern = kPatterns[idx];
        for (unsigned lf = 0; lf <= 0xFFU; ++lf) {
            // MUTATION HOOK (normally inert): no case skipped here.
            // during MUTATION CASE-COUNT-MIXED: `if (idx == 0 && lf == 0) continue;`
            const auto expected = hrm_oracle(
                static_cast<std::uint8_t>(lf),
                oracle_pattern.a, oracle_pattern.b, oracle_pattern.c);
            dut.lf = static_cast<std::uint8_t>(lf);
            dut.ain = driven.a;
            dut.bin = driven.b;
            dut.cin = driven.c;
            dut.eval();
            const auto actual = static_cast<std::uint16_t>(dut.out);
            ++mixed.cases;
            if (actual != expected) {
                ++mixed.mismatches;
                if (mixed.first_failure.empty()) {
                    std::ostringstream os;
                    os << "first mismatch: LF=" << hex2(lf)
                       << " pattern=" << idx << " expected D=" << hex4(expected)
                       << " actual D=" << hex4(actual);
                    mixed.first_failure = os.str();
                }
            }
        }
    }

    checks.expect(mixed.mismatches == 0,
        "SWEEP-MIXED-WORDS: an independent hrm_oracle() (bit-selection, not "
        "the RTL's minterm-register structure) checked against 3 arbitrary "
        "16-bit source-word patterns x 256 LF bytes (768 cases) - the "
        "selector rule holds for mixed, non-degenerate words, not only the "
        "all-zero/all-one probes above (p.174, 'There are eight possible "
        "combinations of values of the three bits', 6-function-generator.c5, "
        "ch6-funcgen.c3)" +
        (mixed.mismatches ? " -- " + mixed.first_failure : ""));

    checks.expect(mixed.cases == 768U,
        "CASE-COUNT-MIXED: " + std::to_string(mixed.cases) +
        " of 768 expected cases actually ran");

    // ---------------- truth table, 8 positions (pp. 174, 175, 178) ----------------
    //
    // Each row of the truth table (p.174) states that input combination
    // (A,B,C)=k selects BLTCON0 LF bit position k; the same fact is
    // restated as a Boolean minterm expression (p.174, overbars), located in
    // the Venn diagram (p.178, figure 6-3) and cross-referenced by a worked
    // example (p.175, positions 6,4,7,3,2). All of that is one proposition
    // per k, asserted once here with every covering id.
    struct TruthTableEntry {
        unsigned k;
        std::string ids;
    };
    const std::array<TruthTableEntry, 8> truth_table{{
        {0, "6-function-generator.c7, 6-function-generator.c8, "
            "ch6-funcgen.tt000.pos, ch6-funcgen.tt000.minterm, "
            "6-function-generator.c74, ch6-venn.f0"},
        {1, "6-function-generator.c9, 6-function-generator.c10, "
            "ch6-funcgen.tt001.pos, ch6-funcgen.tt001.minterm, "
            "6-function-generator.c75, ch6-venn.f1"},
        {2, "6-function-generator.c11, 6-function-generator.c12, "
            "ch6-funcgen.tt010.pos, ch6-funcgen.tt010.minterm, "
            "6-function-generator.c76, ch6-venn.f2, 6-function-generator.c39"},
        {3, "6-function-generator.c13, 6-function-generator.c14, "
            "ch6-funcgen.tt011.pos, ch6-funcgen.tt011.minterm, "
            "6-function-generator.c77, ch6-venn.f3, 6-function-generator.c38"},
        {4, "6-function-generator.c15, 6-function-generator.c16, "
            "ch6-funcgen.tt100.pos, ch6-funcgen.tt100.minterm, "
            "6-function-generator.c78, ch6-venn.f4, 6-function-generator.c36"},
        {5, "6-function-generator.c17, 6-function-generator.c18, "
            "ch6-funcgen.tt101.pos, ch6-funcgen.tt101.minterm, "
            "6-function-generator.c79, ch6-venn.f5"},
        {6, "6-function-generator.c19, 6-function-generator.c20, "
            "ch6-funcgen.tt110.pos, ch6-funcgen.tt110.minterm, "
            "6-function-generator.c80, ch6-venn.f6, 6-function-generator.c35"},
        {7, "6-function-generator.c21, 6-function-generator.c22, "
            "ch6-funcgen.tt111.pos, ch6-funcgen.tt111.minterm, "
            "6-function-generator.c81, ch6-venn.f7, 6-function-generator.c37"},
    }};

    for (const auto& entry : truth_table) {
        const unsigned k = entry.k;
        // MUTATION HOOK (normally inert): probe_k == k for every entry.
        // During MUTATION TRUTHTABLE-POS-3-PROBE this line reads
        // `(k == 3) ? 2U : k;` instead - baseline must be the plain `k`.
        const unsigned probe_k = k;
        const std::uint16_t a = (probe_k & 0x4U) != 0U ? 0xFFFFU : 0x0000U;
        const std::uint16_t b = (probe_k & 0x2U) != 0U ? 0xFFFFU : 0x0000U;
        const std::uint16_t c = (probe_k & 0x1U) != 0U ? 0xFFFFU : 0x0000U;
        dut.lf = static_cast<std::uint8_t>(1U << k);
        dut.ain = a;
        dut.bin = b;
        dut.cin = c;
        dut.eval();
        const auto actual = static_cast<std::uint16_t>(dut.out);
        checks.expect(actual == 0xFFFFU,
            "TRUTHTABLE-POS-" + std::to_string(k) + ": with only LF bit " +
            std::to_string(k) + " set, input (A,B,C)=" +
            std::string(1, static_cast<char>('0' + ((k >> 2) & 1))) +
            std::string(1, static_cast<char>('0' + ((k >> 1) & 1))) +
            std::string(1, static_cast<char>('0' + (k & 1))) +
            " (binary " + std::to_string(k) + ") drives the output high and "
            "no other selector does (p.174 table row, p.174 minterm "
            "expression, p.178 figure 6-3 Venn placement, p.175 positional "
            "cross-reference where applicable) (ids: " + entry.ids + ")" +
            (actual != 0xFFFFU ? " -- actual D=" + hex4(actual) : ""));
    }

    // ---------------- named LF functions: worked examples, table 6-1, cookie-cut, line mode ----------------
    //
    // One entry per distinct LF code with its own named Boolean equation,
    // checked with an independently written lambda (not hrm_oracle(), which
    // cannot tell a right code from a wrong name for it) against all three
    // kPatterns.
    std::vector<NamedFunctionEntry> named_functions;

    // -- p.174 worked examples ($80, and the earlier $FC reading) --
    named_functions.push_back({
        0x80U, "WORKED-P174-$80", "D = A & B & C",
        [](std::uint16_t a, std::uint16_t b, std::uint16_t c) { return a & b & c; },
        "p.174, worked example: 'an LF control byte of $80 (...) turns "
        "[D] on ... corresponding bits of A, B, and C sources were all on' / "
        "'are set to 0' for the rest (ids: 6-function-generator.c27, "
        "6-function-generator.c28, 6-function-generator.c29, "
        "ch6-funcgen.c8, ch6-funcgen.c9)", ""});
    named_functions.push_back({
        0xFCU, "WORKED-P174-$FC", "D = A | B",
        [](std::uint16_t a, std::uint16_t b, std::uint16_t) { return a | b; },
        "p.174, worked example: 'bottom up, reading 11111100, or $FC' for "
        "the function that sets D where A or B is 1 (ids: "
        "6-function-generator.c25, 6-function-generator.c26, "
        "ch6-funcgen.c7)", ""});

    // -- p.178 figure 6-3 / worked $F0 and $C0 --
    named_functions.push_back({
        0xF0U, "WORKED-P178-$F0", "D = A",
        [](std::uint16_t a, std::uint16_t, std::uint16_t) { return a; },
        "p.178, 'This is the set of minterms 7, 6, 5, and 4 ... equals $F0' "
        "and p.176's 'a value of $F0' for the standard copy function A "
        "(ids: 6-function-generator.c82, 6-function-generator.c83, "
        "ch6-minterms.c11, ch6-venn.c1, ch6-venn.c3, ch6-venn.c4)", ""});
    named_functions.push_back({
        0xC0U, "WORKED-P178-$C0", "D = A & B",
        [](std::uint16_t a, std::uint16_t b, std::uint16_t) { return a & b; },
        "p.178, '[bits] common to both the A and B circles, or minterms 7 "
        "and 6 ... equals $C0' (ids: 6-function-generator.c84, "
        "6-function-generator.c85, ch6-venn.c6, ch6-venn.c7)", ""});

    // -- p.179 worked $0F and $C8 --
    named_functions.push_back({
        0x0FU, "WORKED-P179-$0F", "D = ~A",
        [](std::uint16_t a, std::uint16_t, std::uint16_t) { return static_cast<std::uint16_t>(~a); },
        "p.179, '[minterms] 0, 1, 2, and 3 ... equals $0F' for ~A (ids: "
        "6-function-generator.c86, 6-function-generator.c87, ch6-venn.c9, "
        "ch6-venn.c10)", ""});
    named_functions.push_back({
        0xC8U, "WORKED-P179-$C8", "D = (A & B) | (B & C)",
        [](std::uint16_t a, std::uint16_t b, std::uint16_t c) { return (a & b) | (b & c); },
        "p.179, 'AB+BC ... equals $C8' (ids: 6-function-generator.c88, "
        "6-function-generator.c89, ch6-venn.c12, ch6-venn.c13, "
        "ch6-venn.c14, ch6-venn.c15)", ""});

    // -- table 6-1, p.177: 29 of its 30 covered cells (the 30th, $CA, is
    //    folded into COOKIE-CUT below since it is the same fact as p.176/
    //    p.181's cookie-cut function, not a separate proposition) --
    struct T61 {
        std::uint8_t lf; const char* eq; Equation fn; const char* ids; const char* note;
    };
    const std::vector<T61> t61{
        {0xF0U, "D = A", [](std::uint16_t a, std::uint16_t, std::uint16_t){ return a; },
         "6-function-generator.c43, ch6-t61.left.F0", ""},
        {0xC0U, "D = A & B", [](std::uint16_t a, std::uint16_t b, std::uint16_t){ return a & b; },
         "6-function-generator.c44, ch6-t61.right.C0", ""},
        {0x0FU, "D = ~A", [](std::uint16_t a, std::uint16_t, std::uint16_t){ return static_cast<std::uint16_t>(~a); },
         "6-function-generator.c45, ch6-t61.left.0F", ""},
        {0x30U, "D = A & ~B", [](std::uint16_t a, std::uint16_t b, std::uint16_t){ return static_cast<std::uint16_t>(a & ~b); },
         "6-function-generator.c46, ch6-t61.right.30", ""},
        {0xCCU, "D = B", [](std::uint16_t, std::uint16_t b, std::uint16_t){ return b; },
         "6-function-generator.c47, ch6-t61.left.CC", ""},
        {0x0CU, "D = ~A & B", [](std::uint16_t a, std::uint16_t b, std::uint16_t){ return static_cast<std::uint16_t>(~a) & b; },
         "6-function-generator.c48, ch6-t61.right.0C", ""},
        {0x33U, "D = ~B", [](std::uint16_t, std::uint16_t b, std::uint16_t){ return static_cast<std::uint16_t>(~b); },
         "6-function-generator.c49, ch6-t61.left.33", ""},
        {0x03U, "D = ~A & ~B", [](std::uint16_t a, std::uint16_t b, std::uint16_t){ return static_cast<std::uint16_t>(~a) & static_cast<std::uint16_t>(~b); },
         "6-function-generator.c50, ch6-t61.right.03", ""},
        {0xAAU, "D = C", [](std::uint16_t, std::uint16_t, std::uint16_t c){ return c; },
         "6-function-generator.c51, ch6-t61.left.AA", ""},
        {0x88U, "D = B & C", [](std::uint16_t, std::uint16_t b, std::uint16_t c){ return b & c; },
         "6-function-generator.c52, ch6-t61.right.88", ""},
        {0x55U, "D = ~C", [](std::uint16_t, std::uint16_t, std::uint16_t c){ return static_cast<std::uint16_t>(~c); },
         "6-function-generator.c53, ch6-t61.left.55", ""},
        {0x44U, "D = B & ~C", [](std::uint16_t, std::uint16_t b, std::uint16_t c){ return static_cast<std::uint16_t>(b & ~c); },
         "6-function-generator.c54, ch6-t61.right.44", ""},
        {0xA0U, "D = A & C", [](std::uint16_t a, std::uint16_t, std::uint16_t c){ return a & c; },
         "6-function-generator.c55, ch6-t61.left.A0", ""},
        {0x22U, "D = ~B & C", [](std::uint16_t, std::uint16_t b, std::uint16_t c){ return static_cast<std::uint16_t>(~b) & c; },
         "6-function-generator.c56, ch6-t61.right.22", ""},
        {0x50U, "D = A & ~C", [](std::uint16_t a, std::uint16_t, std::uint16_t c){ return static_cast<std::uint16_t>(a & ~c); },
         "6-function-generator.c57, ch6-t61.left.50", ""},
        {0x0AU, "D = ~A & C", [](std::uint16_t a, std::uint16_t, std::uint16_t c){ return static_cast<std::uint16_t>(~a) & c; },
         "6-function-generator.c59, ch6-t61.left.0A", ""},
        {0xF3U, "D = A | ~B", [](std::uint16_t a, std::uint16_t b, std::uint16_t){ return static_cast<std::uint16_t>(a | static_cast<std::uint16_t>(~b)); },
         "6-function-generator.c60, ch6-t61.right.F3", ""},
        {0x05U, "D = ~A & ~C", [](std::uint16_t a, std::uint16_t, std::uint16_t c){ return static_cast<std::uint16_t>(~a) & static_cast<std::uint16_t>(~c); },
         "6-function-generator.c61, ch6-t61.left.05", ""},
        {0x3FU, "D = ~A | ~B", [](std::uint16_t a, std::uint16_t b, std::uint16_t){ return static_cast<std::uint16_t>(static_cast<std::uint16_t>(~a) | static_cast<std::uint16_t>(~b)); },
         "6-function-generator.c62, ch6-t61.right.3F",
         "6-function-generator.c62's own restatement is WRONG (says ~A|B); "
         "the printed table's overbar sits over BOTH letters on this line, "
         "so the code is ~A|~B (~A|B is $CF, a different cell) - this "
         "assertion is built from the quote's hex code plus the verified "
         "table arithmetic, not from that restatement; see file header"},
        {0xFCU, "D = A | B", [](std::uint16_t a, std::uint16_t b, std::uint16_t){ return a | b; },
         "6-function-generator.c63, ch6-t61.left.FC", ""},
        {0xF5U, "D = A | ~C", [](std::uint16_t a, std::uint16_t, std::uint16_t c){ return static_cast<std::uint16_t>(a | static_cast<std::uint16_t>(~c)); },
         "6-function-generator.c64, ch6-t61.right.F5", ""},
        {0xCFU, "D = ~A | B", [](std::uint16_t a, std::uint16_t b, std::uint16_t){ return static_cast<std::uint16_t>(static_cast<std::uint16_t>(~a) | b); },
         "6-function-generator.c65, ch6-t61.left.CF", ""},
        {0x5FU, "D = ~A | ~C", [](std::uint16_t a, std::uint16_t, std::uint16_t c){ return static_cast<std::uint16_t>(static_cast<std::uint16_t>(~a) | static_cast<std::uint16_t>(~c)); },
         "6-function-generator.c66, ch6-t61.right.5F",
         "6-function-generator.c66's own restatement is WRONG (says ~A|C); "
         "same overbar-drop defect as c62, so the code is ~A|~C (~A|C is "
         "$AF, a different cell) - built from the quote plus verified "
         "arithmetic, not from that restatement; see file header"},
        {0xFAU, "D = A | C", [](std::uint16_t a, std::uint16_t, std::uint16_t c){ return a | c; },
         "6-function-generator.c67, ch6-t61.left.FA",
         "6-function-generator.c67's own restatement is WRONG (says A|~B); "
         "the printed line for $FA carries NO overbar at all, so the code "
         "is plain A|C (A|~B is $F3, a different cell) - built from the "
         "quote plus verified arithmetic, not from that restatement; see "
         "file header"},
        {0xDDU, "D = B | ~C", [](std::uint16_t, std::uint16_t b, std::uint16_t c){ return static_cast<std::uint16_t>(b | static_cast<std::uint16_t>(~c)); },
         "6-function-generator.c68, ch6-t61.right.DD", ""},
        {0xAFU, "D = ~A | C", [](std::uint16_t a, std::uint16_t, std::uint16_t c){ return static_cast<std::uint16_t>(static_cast<std::uint16_t>(~a) | c); },
         "6-function-generator.c69, ch6-t61.left.AF", ""},
        {0x77U, "D = ~B | ~C", [](std::uint16_t, std::uint16_t b, std::uint16_t c){ return static_cast<std::uint16_t>(static_cast<std::uint16_t>(~b) | static_cast<std::uint16_t>(~c)); },
         "6-function-generator.c70, ch6-t61.right.77",
         "6-function-generator.c70's own restatement is WRONG (says ~B|C); "
         "same overbar-drop defect as c62/c66, so the code is ~B|~C (~B|C "
         "is $BB, a different cell) - built from the quote plus verified "
         "arithmetic, not from that restatement; see file header"},
        {0xEEU, "D = B | C", [](std::uint16_t, std::uint16_t b, std::uint16_t c){ return b | c; },
         "6-function-generator.c71, ch6-t61.left.EE", ""},
        {0xBBU, "D = ~B | C", [](std::uint16_t, std::uint16_t b, std::uint16_t c){ return static_cast<std::uint16_t>(static_cast<std::uint16_t>(~b) | c); },
         "6-function-generator.c73, ch6-t61.left.BB", ""},
    };
    for (const auto& e : t61) {
        named_functions.push_back({
            e.lf, std::string("TABLE61-") + hex2(e.lf), e.eq, e.fn,
            std::string("p.177, Table 6-1 (ids: ") + e.ids + ")", e.note});
    }

    // -- cookie-cut function, $CA: p.176/p.177/p.181/p.191-192 --
    named_functions.push_back({
        0xCAU, "COOKIE-CUT", "D = (A & B) | (~A & C)",
        [](std::uint16_t a, std::uint16_t b, std::uint16_t c) {
            return static_cast<std::uint16_t>((a & b) | (static_cast<std::uint16_t>(~a) & c));
        },
        "p.176 ('commonly referred to as the cookie-cut function', "
        "'AB+A-bar C, which works out to an LF code value of $CA'), p.177 "
        "(table 6-1's own $CA cell), p.181 ('yielding our classic "
        "cookie-cut function of AB+A-bar C, or $CA'), p.191-192 ('the most "
        "common function for drawing a line is AB+A-bar C' / 'else "
        "BLTCON0 LF control byte = AB+A-bar C') (ids: "
        "6-function-generator.c41, 6-function-generator.c42, "
        "6-function-generator.c72, ch6-minterms.c14, ch6-minterms.c15, "
        "ch6-t61.right.CA, ch6-shifts.c31, ch6-shifts.c32, ch6-shifts.c33, "
        "ch6-line.c39, ch6-linesum.c16)", ""});

    // -- line-mode function, $4A: p.191-192/p.268 --
    named_functions.push_back({
        0x4AU, "LINE-MODE-4A-EQUATION", "D = (~A & C) | (A & B & ~C)",
        [](std::uint16_t a, std::uint16_t b, std::uint16_t c) {
            return static_cast<std::uint16_t>(
                (static_cast<std::uint16_t>(~a) & c) |
                (a & b & static_cast<std::uint16_t>(~c)));
        },
        "p.268 appendix A ('LINE DRAW ... with 4A to select the equation "
        "D=(A-bar C+ABC-bar)'), p.191-192 ('the exclusive-or line control "
        "byte = ABC-bar+A-bar C') (ids: 6-appa-control.c80, "
        "6-appa-control.c81, ch6-appa-lf.c1, ch6-appa-lf.c2, ch6-line.c40, "
        "ch6-linesum.c15)", ""});

    for (std::size_t i = 0; i < named_functions.size(); ++i) {
        int override_lf = -1;
        if (named_functions[i].label == "TABLE61-$3F") {
            override_lf = -1;  // 0xCF during MUTATION NAMEDFN-3F-CODE-SWAP
        } else if (named_functions[i].label == "COOKIE-CUT") {
            override_lf = -1;  // 0xCB during MUTATION COOKIE-CUT-CODE-SWAP
        } else if (named_functions[i].label == "LINE-MODE-4A-EQUATION") {
            override_lf = -1;  // 0x4B during MUTATION LINE-MODE-CODE-SWAP
        }
        run_named_function_check(dut, checks, named_functions[i], override_lf);
    }

    // ---------------- line-mode terms, p.268 ----------------
    //
    // The $4A equation's two terms, checked separately since each is its
    // own claim (the pass-through half and the inverting half), not a
    // restatement of the combined equation above.
    {
        // A entirely 0: D reduces to the pass-through term, C unchanged.
        const std::uint16_t driven_a = 0x0000U;  // 0x0004 during MUTATION LINE-MODE-TERM-A0-STIMULUS
        dut.lf = 0x4AU;
        dut.ain = driven_a;
        dut.bin = 0x1234U;
        dut.cin = 0x5678U;
        dut.eval();
        const auto actual = static_cast<std::uint16_t>(dut.out);
        checks.expect(actual == 0x5678U,
            "LINE-MODE-TERM-PASSTHROUGH: with A entirely 0, LF=$4A passes C "
            "unchanged to the output (p.268, 'LINE DRAW ... will pass the C "
            "field unchanged' / '(not A and C)') (ids: 6-appa-control.c83, "
            "ch6-appa-lf.c4)" +
            (actual != 0x5678U ? " -- actual D=" + hex4(actual) : ""));
    }
    {
        // A entirely 1: D reduces to the inverting term, B & ~C.
        // MUTATION HOOK (normally inert): driven_b == 0x1234.
        const std::uint16_t driven_b = 0x1234U;  // ^ 0x0004 during MUTATION LINE-MODE-TERM-INVERT
        dut.lf = 0x4AU;
        dut.ain = 0xFFFFU;
        dut.bin = driven_b;
        dut.cin = 0x5678U;
        dut.eval();
        const auto actual = static_cast<std::uint16_t>(dut.out);
        const std::uint16_t expected = static_cast<std::uint16_t>(0x1234U & static_cast<std::uint16_t>(~0x5678U));
        checks.expect(actual == expected,
            "LINE-MODE-TERM-INVERT: with A entirely 1, LF=$4A inverts C and "
            "ANDs it with B texture (p.268, 'LINE DRAW ... invert the C "
            "field and combine it [with texture (A and B and not C)]') "
            "(ids: 6-appa-control.c84, ch6-appa-lf.c6)" +
            (actual != expected ? " -- actual D=" + hex4(actual) : ""));
    }

    // ---------------- mechanism: allow/inhibit per minterm (pp. 198, 199) ----------------
    //
    // Distinct from TRUTHTABLE-POS-n above: that group checks only the
    // MATCHING selector. This checks that setting exactly one LF bit
    // produces output ONLY at its own selector and nowhere else - the
    // "allow or inhibit" claim and figure 6-13's one-AND-gate-per-minterm
    // structure.
    {
        bool ok = true;
        std::string mismatch;
        for (unsigned k = 0; k < 8 && ok; ++k) {
            // MUTATION HOOK (normally inert): driven_lf sets only bit k.
            const unsigned driven_lf = (1U << k);  // | (1U << ((k+1)%8)) during MUTATION MECHANISM-EXTRA-BIT
            for (unsigned selector = 0; selector < 8; ++selector) {
                const std::uint16_t a = (selector & 0x4U) != 0U ? 0xFFFFU : 0x0000U;
                const std::uint16_t b = (selector & 0x2U) != 0U ? 0xFFFFU : 0x0000U;
                const std::uint16_t c = (selector & 0x1U) != 0U ? 0xFFFFU : 0x0000U;
                dut.lf = static_cast<std::uint8_t>(driven_lf);
                dut.ain = a; dut.bin = b; dut.cin = c;
                dut.eval();
                const auto actual = static_cast<std::uint16_t>(dut.out);
                const std::uint16_t expected = (selector == k) ? 0xFFFFU : 0x0000U;
                if (actual != expected) {
                    ok = false;
                    std::ostringstream os;
                    os << "first mismatch: k=" << k << " selector=" << selector
                       << " expected D=" << hex4(expected) << " actual D=" << hex4(actual);
                    mismatch = os.str();
                    break;
                }
            }
        }
        checks.expect(ok,
            "MECHANISM-ALLOW-INHIBIT: for every k in 0..7, setting only LF "
            "bit k allows output at selector k and inhibits it at every one "
            "of the other seven selectors (p.198, 'select bits either allow "
            "or inhibit the use of a specific minterm'; p.199 figure 6-13, "
            "'Multi-minterm select bits (8)' with one AND gate per minterm "
            "output) (ids: ch6-blockdiag.c7, 6-blitter-block-diagram.c7, "
            "ch6-fig613.f16, ch6-fig613.f17)" +
            (ok ? "" : " -- " + mismatch));
    }

    // ---------------- mechanism: 16 independent bit-slices (p. 199, "16 BLOCKS") ----------------
    {
        // D = A (LF=$F0). Flipping bit 0 of A must change only bit 0 of the
        // output; every other bit is driven from B/C words with all other
        // bits held at a fixed, non-degenerate pattern.
        dut.lf = 0xF0U;
        dut.ain = 0x0000U;
        dut.bin = 0x1234U;
        dut.cin = 0x5678U;
        dut.eval();
        const auto base = static_cast<std::uint16_t>(dut.out);

        // MUTATION HOOK (normally inert): flips bit 0.
        const unsigned probe_bit = 0;  // changed to 6 during MUTATION BIT-INDEPENDENCE-WRONG-BIT
        dut.ain = static_cast<std::uint16_t>(1U << probe_bit);
        dut.eval();
        const auto flipped = static_cast<std::uint16_t>(dut.out);
        const auto diff = static_cast<std::uint16_t>(base ^ flipped);

        checks.expect(diff == (1U << 0),
            "BIT-INDEPENDENCE: flipping exactly bit 0 of source A (with "
            "LF=$F0, D=A) changes exactly bit 0 of the output and no other "
            "bit - the 16 output bits are 16 independent copies of the same "
            "logic, not one shared computation (p.199 figure 6-13, the "
            "minterm-and-fill logic annotated '16 BLOCKS') (ids: "
            "ch6-fig613.f24)" +
            (diff != (1U << 0) ? " -- actual changed bits=" + hex4(diff) : ""));
    }

    // ---------------- declined claim: ch6-minterms.c10 (routed record, not graded) ----------------
    //
    // Routed record:ch6-blitter-minterm rather than to an assertion (see file
    // header, CLAIM COVERAGE): the router's own reason is that reusing one
    // mask bitplane "in turn on" each background bitplane is a property of
    // the memory image the caller/testbench builds, not of any port on this
    // module -- the neighbouring claims from the same passage were discarded
    // for the identical reason (ch6-minterms.c9, 6-function-generator.c40).
    // Printed here, not left in the header comment alone.
    checks.record(
        "ch6-minterms.c10 (p.176, quote: 'depth of the background bitplane. "
        "This mask can be used in turn on' -- restated: 'The same "
        "single-bitplane mask can be used in turn on each of the background "
        "bitplanes.'): describes a multi-bitplane looping convention in "
        "software -- which bitplane the mask is currently applied to is a "
        "property of the memory image the caller builds, not of any port on "
        "agnus_blitter_minterm (agnus_blitter_minterm.v:7-14 has no notion "
        "of \"which bitplane\"; the module applies the same combinational "
        "function regardless). Not constructible at this module's boundary "
        "-- declined, not graded.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    dut.final();
    return checks.failed == 0 ? 0 : 1;
}
