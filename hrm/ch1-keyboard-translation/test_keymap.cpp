// ch1-keyboard-translation -- keymap conformance test
//
// Test purpose: grade page 2's "A professional keyboard with numeric
//   keypad, 10 function keys, and cursor keys." against `keymap`
//   (amiga_keymap.v:10-13). SPECIFICATION.md (committed before this file,
//   or amiga_keymap.v, were opened -- commit 76bacde) records the
//   reasoning; this file carries it out.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 2.
//
// SCOPE / TRANSFERABILITY (required by 2026-09-16_ch1_build_brief.md, and
//   repeated per SPECIFICATION.md): every red this package could produce
//   would be a statement about NanoMig, not about the Minimig family.
//   `keymap` lives in src/misc, which holds NanoMig's own substitutes for
//   Amiga peripherals, not the shared chipset lineage `src/minimig-aga`
//   is -- a finding here does not transfer.
//
// BOUNDARY: keymap(input [6:0] code, output [6:0] amiga) -- two ports,
//   purely combinational (amiga_keymap.v:15-142, one `assign`). No clock
//   is needed: dut->eval() after setting `code` is a complete stimulus.
//
// CLAIM COVERAGE: this package's two claims (sol:1-components.c19 and
//   opus:1-components.c19, both the page-2 sentence above) are BOTH
//   declined below, on record() lines -- not graded here. Neither uid
//   appears on any expect() line in this file.
//
//   Why: the build brief requires checking whether an accepted package
//   already grades the same printed proposition before writing an
//   assertion. `grep -rn "numeric keypad|cursor keys|10 function|ten
//   function|F1-F10" ../` (run over nanomig-hrm-tests/hrm) matches exactly
//   one package, ch8-keycode-table -- same top, same boundary, built from
//   the chapter-8 keycode-table claims (pages 251/252/254). Its
//   test_keymap.cpp already grades, at a finer grain than page 2 asks for:
//     - 10 function keys: its FUNCTION-RANGE assertion tests "the ten
//       function-key codes are exactly the contiguous range $50 (F1) to
//       $59 (F10)" (citing opus:8-keycode.c30, p.252 "50-59 Function keys
//       F1-F10") -- the identical block/count shape as this page's claim.
//     - cursor keys: its KEY-$4C/$4D/$4E/$4F rows grade up/down/right/left
//       individually, all PASS at baseline in that package.
//     - numeric keypad: its KEY-$0F, $1D-$1F, $2D-$2F, $3C-$3F, $43, $4A,
//       $5A-$5E rows grade every individual keypad key, all PASS at
//       baseline in that package.
//   Re-measuring the same structural facts here, even worded at page 2's
//   coarser "does the group exist" level, would be pure duplication of an
//   accepted package's measurement with no new information -- exactly the
//   risk the build brief names: this chapter reaching a different verdict
//   from an accepted one about the same underlying proposition.
//
// FIXTURE-SELFCHECK below carries no claim uid -- it is a harness
//   precondition (per the build brief's "the fixture proves it can
//   deliver a known value before anything reads one"), included so this
//   package is a real, running build against `keymap` rather than an
//   empty shell, even though neither of its two claims is graded here.
//
// A CLAIM THIS PACKAGE DOES NOT CARRY: the task sheet frames this
//   package's oracle as "page 2's sentence that a variety of
//   international keyboards are supported" -- that is 1-components.c20,
//   immediately following c19 in the source text, and it is NOT one of
//   this package's two claims. classify.package is unset for c20; its own
//   classification is status: not-asserted (reason: absent) -- keymap's
//   whole interface is one unconditional comparison chain over one 7-bit
//   input with no layout/locale selector anywhere in src/misc, so "a
//   variety ... are supported" is not constructible at this boundary.
//   Confirmed directly against amiga_keymap.v:9-142 while reading it for
//   this file: no selector input of any kind exists. Not graded, not
//   declined here (it carries no uid assigned to this package) -- reported
//   as a task-sheet inaccuracy.

#include <cstdint>
#include <iostream>
#include <string>
#include "Vkeymap.h"
#include "verilated.h"

#ifndef MUT_FIXTURE_ZERO
#define MUT_FIXTURE_ZERO 0
#endif

namespace {

// Purely combinational: driving `code` and calling eval() is a complete
// stimulus. No clock, no reset, no internal state.
struct Harness {
    Vkeymap* dut;
    Harness() : dut(new Vkeymap) { dut->code = 0; dut->eval(); }
    ~Harness() { delete dut; }
    uint8_t sample(uint8_t code) {
        dut->code = code & 0x7f;
        dut->eval();
        return dut->amiga & 0x7f;
    }
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

} // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;
    Harness h;

    // =========================================================================
    // FIXTURE-SELFCHECK (no uid; harness precondition). Stimulates the 'a'
    // key's own HID code, confirmed directly against amiga_keymap.v:21
    // while reading the file for this package (code $04 -> amiga $20),
    // before any claim-bearing line runs. Same value ch8-keycode-table's
    // own fixture self-check uses, for the same reason: it is the
    // boundary's simplest known-good pair.
    // =========================================================================
    {
#if MUT_FIXTURE_ZERO
        uint8_t got = h.sample(0x00);
#else
        uint8_t got = h.sample(0x04);
#endif
        checks.expect(got == 0x20,
            "FIXTURE-SELFCHECK: driving code=$04 (the 'a' key's own HID "
            "source code, amiga_keymap.v:21) returns amiga=$20 -- the "
            "fixture can deliver a known, specific value before any claim "
            "below reads one. No manual page (harness precondition).");
    }
// MUTATION FIXTURE-SELFCHECK stimulus red MUT_FIXTURE_ZERO: the harness drives code=$00 instead of the 'a' key's own code ($04); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $20 -- baseline PASS goes red.

    // =========================================================================
    // Declined claims (record() only -- no expect(), no MUTATION line, per
    // the build brief: a MUTATION line's label is the leading token of a
    // label passed to expect(), and neither of these two is).
    // =========================================================================

    checks.record(
        "PAGE2-KEYBOARD-GROUPS (p.2): \"A professional keyboard with "
        "numeric keypad, 10 function keys, and cursor keys.\" All three "
        "named groups -- the ten function keys (contiguous $50-$59), the "
        "cursor keys, and the numeric keypad -- are already graded, at a "
        "finer grain than this sentence asks, by the accepted package "
        "ch8-keycode-table: its FUNCTION-RANGE assertion (citing "
        "opus:8-keycode.c30, p.252 \"50-59 Function keys F1-F10\") tests "
        "the identical 10-contiguous-codes proposition directly; its "
        "KEY-$4C/$4D/$4E/$4F rows grade the four cursor keys individually; "
        "its KEY-$0F/$1D-$1F/$2D-$2F/$3C-$3F/$43/$4A/$5A-$5E rows grade "
        "every numeric-pad key individually. All PASS at baseline in that "
        "package (confirmed by reading its test_keymap.cpp before this "
        "file was written). Re-grading the same structural facts here "
        "would duplicate an accepted package's measurement and risk this "
        "chapter reaching a different verdict about the same proposition -- "
        "declined per the build brief, not re-asserted. 'Professional' "
        "sets no threshold anywhere and is not gradable. "
        "[claims: sol:1-components.c19, opus:1-components.c19]");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
