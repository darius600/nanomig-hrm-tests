// Test purpose: The hold-and-modify control field -- planes 6 and 5 select
//   table/blue/red/green, planes 4-1 supply the value, and the untouched
//   components are held from the previous pixel (Table 3-19). Package
//   ch3-ham-control-field, fixture denise_hamgenerator (one Verilator build,
//   this module only).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 86, 87, 93, 95, 271 (chapter 3's hold-and-modify prose, Table 3-17's
//   low-resolution colour-selection table, Table 3-19's plane-6/plane-5
//   control-field table, and appendix A's BPLCON0/HOMOD legend entry).
// Non-goals: AGA HAM8 (the `ham8` port's alternate decode path) is out of
//   scope -- every stimulus below holds ham8=0; the Third Edition HRM's
//   chapter-3 HAM text is an OCS/ECS 6-bitplane oracle, not an AGA HAM8 one
//   (same non-goal already recorded by hrm/denise_hamgenerator's own
//   header). `bank`, `loct` and `bplxor` are held at their power-up default
//   (0) throughout -- no claim in this package's 77-record inventory names
//   them, and nothing asserted below depends on them.
//
// BOUNDARY (from the classifier's own evidence, not from opening the RTL --
// see "ORDER OF WORK" below): denise_hamgenerator.v:6-18 module header --
// ports reg_address_in, data_in[11:0], select[7:0], bplxor, bank, loct,
// ham8, rgb[23:0]. denise_hamgenerator.v:80-92 -- OCS HAM: select_r[5:4] is
// the control field (00 colour table, 01 modify blue, 10 modify red, 11
// modify green), select_r[3:0] is the data field, and the held components
// come from rgb_prev, registered at l.55-57.
//
// ORDER OF WORK: this file (assertions, expected values, claim-id coverage)
// was written and committed from claims.jsonl's quotations and the
// classifier's evidence field ONLY -- denise_hamgenerator.v itself was not
// opened until after that commit, per the build brief's explicit gate. The
// Harness's clocking mechanics (clk/clk7_en toggling, COLORBASE write
// sequencing, ticks-per-pixel) are reused from hrm/denise_hamgenerator/
// test_denise_hamgenerator.cpp -- an already-committed, already-built
// sibling harness for the SAME module -- not learned by opening this
// package's own forbidden-until-committed file.
//
// THE BPLCON0 ENABLE BIT: NOT ASSERTED, RECORDED AS A MEASUREMENT, NOT
// RESOLVED. The manual disagrees with itself about which bit of BPLCON0
// turns hold-and-modify on:
//   p.95: "This mode is in effect only if bit 10 of BPLCON0 = 1."
//   p.87: "Bit 11 of register BPLCON0 selects hold-and-modify mode. ...
//     Bit HOMOD, bit 11, is 1. Bit DBLPF, bit 10, is 0 (single-playfield
//     mode specified)."
//   p.90 (BPLCON0 legend): "Bit 11 - HOMOD (hold-and-modify enable) ... Bit
//     10 - DBLPF (double-playfield enable)."
//   appendix p.271 (register table + legend): bit 11 listed as HOMOD, bit
//     10 as DBLPF; "HOMOD=Hold-and-modify mode (1 = Hold-and-modify mode
//     (HAM); ...)".
// Three of the four printed citations (87, 90, 271) agree the enable bit is
// 11 (HOMOD), with bit 10 (DBLPF) required to be 0; one (95) disagrees and
// calls bit 10 the enable. NOT resolved here, per the build brief -- and it
// could not be settled at THIS boundary regardless: denise_hamgenerator's
// own port list (above) has no BPLCON0, HOMOD or DBLPF port at all.
// Whichever bit real hardware keys on, the decision is made upstream (by
// whatever decodes BPLCON0 and gates what reaches this module's `select`
// input) and is not observable here. A-leg.homod1 is declined below for
// exactly this boundary reason, independent of the page dispute; the
// dispute itself is recorded, not asserted, in the NOTE call that declines
// it. What IS undisputed and IS asserted below (fully within this module's
// boundary): the modify control comes from planes 6 and 5 -- see
// HAM-CONTROL-00/01/10/11-... below.
//
// NOT ASSERTED (2 of 77 claims, both declined for boundary reasons, neither
// resolved by picking a side): 3-ham.c11 (sol, p.86) and A-leg.homod1
// (opus, p.271) -- see the two checks.note() calls in main() for the full
// reasoning against each.
//
// CLAIM COVERAGE: all 77 records of claims.jsonl (both readers, every page)
// are accounted for below -- 75 inside an expect() message, 2 inside a
// note() call with a stated reason. Every id is written out in full; none
// compressed to a shared prefix. c10 and c11 (page 86) and c14/c15/c16/c17/
// c18/c19 (pages 86-87) and t19.c*/table3-19.c* (page 95) each name TWO
// records (opus and sol independently assigned the same numeric id to
// different sentences); citing the bare id once covers both, and where both
// readers' sentences land in different assertions the id is cited in both
// places, honestly, rather than picked once and dropped from the other.
//
// CONTROL CASE: HAM-CONTROL-00-NORMAL-SELECTION is the control every
// HAM-CONTROL-01/10/11 modify assertion is measured against, on the
// identical 24-bit rgb-readback basis, in the identical Harness session.
//
// RTL OPENED, BUILT AND RUN, 10.09.2026 (all after the spec-only commit
// above). denise_hamgenerator.v:80-92's three OCS modify-path expressions
// ({rgb_prev[23:8],select_r[3:0],select_r[3:0]} etc.) and
// denise_colortable_ram_mf.v's two parallel 12-bit RAMs (ram0/ram1, written
// identically when loct=0, per denise_hamgenerator.v:31's `wr_ws = loct ?
// 2'b01 : 2'b11`) match every numeric prediction this file's assertions
// were written against before either file was opened -- no expected value,
// stimulus shape or Harness mechanic needed changing. First build: 9
// passed, 0 failed, exactly as committed. `bank` is 3 bits wide (not
// checked before opening the file, held at 0 regardless, no effect) and
// `reg_address_in` is declared `[9-1:1]` rather than `[8:0]` (an index-
// naming detail that does not change the plain-integer C++ assignment
// already in write_color()).
//
// MUTATION LOG (all 9 entries actually run by this agent under WSL/
// Verilator on 10.09.2026: edit -> rebuild -> run -> confirm the named
// assertion(s) red -> revert -> rebuild -> confirm the full suite green
// again -- before moving to the next one. Every mutation is a STIMULUS
// change to this file's own inputs; no expected value or oracle text was
// ever touched, and nanomig/ was never touched. Final rebuild after the
// last revert: 9 passed, 0 failed, confirmed again immediately before this
// commit.):
//
// MUTATION TEST-DATA-DELIVERY stimulus red: register 7's fed colour
//   changed from 0xC91 to 0xC92 -> FAIL ("8 passed, 1 failed"), only
//   TEST-DATA-DELIVERY red, its own readback no longer matches. Reverted:
//   9/0.
// MUTATION HAM-CONTROL-00-NORMAL-SELECTION stimulus red: COLOR00's loaded
//   value changed from 0xABC to 0xABD -> FAIL ("8 passed, 1 failed"), only
//   HAM-CONTROL-00-NORMAL-SELECTION red (its own check hardcodes
//   0xAABBCCu; every other assertion compares rgb0/rgb1/.../rgb4 to each
//   other, not to a literal, so the whole chain shifted consistently and
//   stayed green). Reverted: 9/0.
// MUTATION HAM-HOLDS-PREVIOUS-VALUE-AND-MODIFIES-ONE-COMPONENT stimulus red: step 2's control code changed from 0x1 (modify blue) to 0x0
//   (normal selection), data left at 0x5 -> FAIL ("7 passed, 2 failed"):
//   HAM-HOLDS-... red (register 5, never loaded in this Harness, defaults
//   to 0 in Verilator, so all three components changed, not one) AND
//   HAM-CONTROL-01-MODIFIES-BLUE-HOLDS-RED-GREEN also red, as expected
//   collateral of changing away from control=01 -- recorded here rather
//   than hidden, per the standing rule against reporting a result not
//   actually produced. Reverted: 9/0.
// MUTATION HAM-CONTROL-01-MODIFIES-BLUE-HOLDS-RED-GREEN stimulus red: step
//   2's data field changed from 0x5 to 0x6 (control left at 0x1) -> FAIL
//   ("8 passed, 1 failed"), only this assertion red (blue now 0x66 against
//   the still-0x55 expectation; HAM-HOLDS-... stayed green because exactly
//   one component still changed, just to a different value). Reverted:
//   9/0.
// MUTATION HAM-CONTROL-10-MODIFIES-RED-HOLDS-GREEN-BLUE stimulus red: step
//   3's data field changed from 0x3 to 0x4 (control left at 0x2) -> FAIL
//   ("8 passed, 1 failed"), only this assertion red. Reverted: 9/0.
// MUTATION HAM-CONTROL-11-MODIFIES-GREEN-HOLDS-RED-BLUE stimulus red: step
//   4's data field changed from 0x9 to 0x8 (control left at 0x3) -> FAIL
//   ("8 passed, 1 failed"), only this assertion red. Reverted: 9/0.
// MUTATION HAM-SINGLE-REGISTER-TECHNIQUE-COLOR00 stimulus red: a stray
//   `h.write_color(0, 0x000);` inserted between step 4 and the final
//   re-select, rewriting COLOR00 -> FAIL ("8 passed, 1 failed"), only this
//   assertion red (rgb4 now reads the rewritten black, not the original
//   0xAABBCC; rgb0-rgb3 were already captured before the stray write, so
//   nothing else moved). Reverted: 9/0.
// MUTATION HAM-CONTROL-00-TABLE-MAPS-VALUES-TO-REGISTERS-0-15 stimulus red: a stray `h.write_color(5, 0x999);` inserted right after the
//   15-register load loop, overwriting register 5's value -> FAIL ("8
//   passed, 1 failed"), only this assertion red (register 5's readback no
//   longer matches its own N*0x111 expectation). Reverted: 9/0.
// MUTATION HAM-DATA-FIELD-SPANS-16-LEVELS stimulus red: the sweep's fed
//   data value changed from `data` to `(data==7 ? 8 : data)` for one point
//   in the loop, want_blue left computed from the unmutated `data` -> FAIL
//   ("8 passed, 1 failed"), only this assertion red (data point 7 now
//   measures 0x88 against a 0x77 expectation). Reverted: 9/0.

#include <cstdint>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vdenise_hamgenerator.h"

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    // A note is not a check: it prints and does not touch passed/failed.
    // Used only to record, in an actual C++ string (not a `//` comment), the
    // two claims declined below -- so their ids are found by a coverage
    // search of the file's strings, not only by reading its comments.
    void note(const std::string& msg) { std::cout << "NOTE: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Reused from hrm/denise_hamgenerator/test_denise_hamgenerator.cpp's
// already-committed harness for this same module: clk/clk7_en toggling, a
// COLORBASE=0x180 (word address 0xC0) CPU-style register write, and 4 ticks
// to settle one pixel's `select` through to `rgb`. Nothing about this
// mechanism is a claim of this package's own inventory; it is
// infrastructure, not specification.
class Harness {
public:
    Harness() : dut(new Vdenise_hamgenerator) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->select = 0;
        dut->bplxor = 0;
        dut->bank = 0;
        dut->loct = 0;
        dut->ham8 = 0;
        for (int i = 0; i < 10; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    void write_color(int index, uint16_t color12) {
        dut->reg_address_in = static_cast<uint16_t>(0xC0 + index);
        dut->data_in = color12;
        dut->clk7_en = 1;
        tick();
        dut->clk7_en = 0;
        dut->reg_address_in = 0;
        for (int i = 0; i < 4; ++i) tick();
    }

    // select = (control << 4) | data, per the classifier's evidence for
    // denise_hamgenerator.v:80-92 (select_r[5:4] control field, select_r
    // [3:0] data field) -- the evidence cited by every claim in this
    // package's inventory.
    uint32_t set_pixel(uint8_t control, uint8_t data) {
        dut->select = static_cast<uint8_t>((control << 4) | (data & 0xF));
        for (int i = 0; i < 4; ++i) tick();
        return dut->rgb;
    }

    Vdenise_hamgenerator* dut;
};

uint32_t R(uint32_t rgb) { return (rgb >> 16) & 0xFF; }
uint32_t G(uint32_t rgb) { return (rgb >> 8) & 0xFF; }
uint32_t B(uint32_t rgb) { return rgb & 0xFF; }

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- TEST-DATA-DELIVERY (fixture check, no page) ----------------
    {
        Harness h;
        h.write_color(3, 0x5A3);
        h.write_color(7, 0xC91);
        h.write_color(12, 0x246);
        uint32_t r3 = h.set_pixel(0x0, 3);
        uint32_t r7 = h.set_pixel(0x0, 7);
        uint32_t r12 = h.set_pixel(0x0, 12);
        checks.expect(
            r3 == 0x55AA33u && r7 == 0xCC9911u && r12 == 0x224466u,
            "TEST-DATA-DELIVERY: three distinct 12-bit colours (0x5A3 at "
            "register 3, 0xC91 at register 7, 0x246 at register 12) written "
            "into the fixture's colour RAM arrive at rgb, distinctly and in "
            "the order written, when selected back with control=00 (normal "
            "colour selection) -- proves the CPU-write-then-select data "
            "path is actually live before any claim-based assertion below "
            "depends on it.");
    }

    // ---------------- Modify chain: one COLOR00 register, four control codes in sequence ----------------
    // All four HAM-CONTROL-* assertions, HAM-HOLDS-PREVIOUS-VALUE-AND-
    // MODIFIES-ONE-COMPONENT and HAM-SINGLE-REGISTER-TECHNIQUE-COLOR00 read
    // off ONE continuous sequence below, matching the manual's own worked
    // technique: "it is possible to get by with defining only one color
    // register, which is COLOR00 ... You treat the entire screen as a
    // modification of that original color" (p.87).
    uint32_t rgb0, rgb1, rgb2, rgb3, rgb4;
    {
        Harness h;
        h.write_color(0, 0xABC);      // COLOR00 = R:A G:B B:C -- the only register this sequence ever defines
        rgb0 = h.set_pixel(0x0, 0x0); // control 00, data 0: normal selection of COLOR00 itself
        rgb1 = h.set_pixel(0x1, 0x5); // control 01: modify blue, data 0x5
        rgb2 = h.set_pixel(0x2, 0x3); // control 10: modify red, data 0x3
        rgb3 = h.set_pixel(0x3, 0x9); // control 11: modify green, data 0x9
        rgb4 = h.set_pixel(0x0, 0x0); // control 00, data 0 again: re-select COLOR00, unmodified
    }

    // ---------------- HAM-CONTROL-00-NORMAL-SELECTION ----------------
    checks.expect(
        rgb0 == 0xAABBCCu,
        "HAM-CONTROL-00-NORMAL-SELECTION: with the 6-5 control field "
        "(planes 6,5) at 00, the pixel reads COLOR00's own stored content "
        "verbatim (0xABC -> 0xAABBCC) -- the CONTROL CASE every "
        "HAM-CONTROL-01/10/11 assertion below is measured against, on the "
        "identical rgb-readback basis. 'If the 6-5 bit combination ... is "
        "00, normal color selection procedure is followed' (p.86), 'the "
        "bit combinations from planes 4-1 ... are used to choose one of 16 "
        "color registers' (p.86), 'Planes 5 and 6 are used to modify the "
        "way bits from planes 1-4 are treated' (p.86), '0 0 Normal "
        "operation (use color register itself)' (Table 3-19, p.95). "
        "3-ham.c09, opus:3-ham.c10, sol:3-ham.c10, opus:3-ham.c11, 3-ham.c8, 3-ham.c9, 3-t19.c00, "
        "3-table3-19.c1 .");

    // ---------------- HAM-HOLDS-PREVIOUS-VALUE-AND-MODIFIES-ONE-COMPONENT ----------------
    bool step2_one_change = (R(rgb1) == R(rgb0)) && (G(rgb1) == G(rgb0)) && (B(rgb1) != B(rgb0));
    bool step3_one_change = (R(rgb2) != R(rgb1)) && (G(rgb2) == G(rgb1)) && (B(rgb2) == B(rgb1));
    bool step4_one_change = (R(rgb3) == R(rgb2)) && (G(rgb3) != G(rgb2)) && (B(rgb3) == B(rgb2));
    checks.expect(
        step2_one_change && step3_one_change && step4_one_change,
        "HAM-HOLDS-PREVIOUS-VALUE-AND-MODIFIES-ONE-COMPONENT: across all "
        "three modify steps (01 -> 10 -> 11) chained from the same COLOR00 "
        "base, exactly one of the three 8-bit colour components changes at "
        "each step and the other two are held identical to the immediately "
        "preceding pixel's own output -- 'the value in the color output "
        "circuitry is held, and one of the three components of the color "
        "(red, green, or blue) is modified by bits coming from certain "
        "preselected bitplanes' (p.86), 'Note that a particular "
        "hold-and-modify pixel can only change one of the three color "
        "values at a time' (p.86). "
        "3-ham.c03, 3-ham.c04, 3-ham.c06, 3-ham.c4, 3-ham.c5, 3-ham.c6 .");

    // ---------------- HAM-CONTROL-01-MODIFIES-BLUE-HOLDS-RED-GREEN ----------------
    checks.expect(
        R(rgb1) == R(rgb0) && G(rgb1) == G(rgb0) && B(rgb1) == 0x55u,
        "HAM-CONTROL-01-MODIFIES-BLUE-HOLDS-RED-GREEN: with the 6-5 "
        "control field at 01, red and green are held from the immediately "
        "preceding pixel (0xAA, 0xBB unchanged) and blue is replaced by "
        "the 4-bit data field (planes 4-1 = 0x5, duplicated to 0x55) -- "
        "'If the 6-5 bit combination is 01, the color of the pixel "
        "immediately to the left of this pixel is duplicated and then "
        "modified. The bit combinations from planes 4-1 are used to "
        "replace the four \"blue\" bits' (pp.86-87), '0 1 Hold green and "
        "red  B = Bitplane 4-1 contents' (Table 3-19, p.95). "
        "sol:3-ham.c12, opus:3-ham.c13, sol:3-ham.c13, opus:3-ham.c14, 3-t19.c01, 3-table3-19.c2, "
        "3-table3-19.c3, 3-table3-19.c4 .");

    // ---------------- HAM-CONTROL-10-MODIFIES-RED-HOLDS-GREEN-BLUE ----------------
    checks.expect(
        G(rgb2) == G(rgb1) && B(rgb2) == B(rgb1) && R(rgb2) == 0x33u,
        "HAM-CONTROL-10-MODIFIES-RED-HOLDS-GREEN-BLUE: with the 6-5 "
        "control field at 10, green and blue are held from the immediately "
        "preceding pixel (0xBB, 0x55 unchanged) and red is replaced by the "
        "4-bit data field (planes 4-1 = 0x3, duplicated to 0x33) -- 'If "
        "the 6-5 bit combination is 10, the color of the pixel immediately "
        "to the left of this pixel is duplicated and then modified. The "
        "bit combinations from planes 4-1 are used to replace the four "
        "\"red\" bits' (p.87), '1 0 Hold green and blue  R = Bitplane 4-1 "
        "contents' (Table 3-19, p.95). "
        "sol:3-ham.c14, opus:3-ham.c15, sol:3-ham.c15, opus:3-ham.c18, 3-t19.c10, 3-table3-19.c5, "
        "3-table3-19.c6, 3-table3-19.c7 .");

    // ---------------- HAM-CONTROL-11-MODIFIES-GREEN-HOLDS-RED-BLUE ----------------
    checks.expect(
        R(rgb3) == R(rgb2) && B(rgb3) == B(rgb2) && G(rgb3) == 0x99u,
        "HAM-CONTROL-11-MODIFIES-GREEN-HOLDS-RED-BLUE: with the 6-5 "
        "control field at 11, red and blue are held from the immediately "
        "preceding pixel (0x33, 0x55 unchanged) and green is replaced by "
        "the 4-bit data field (planes 4-1 = 0x9, duplicated to 0x99) -- "
        "'If the 6-5 bit combination is 11, the color of the pixel "
        "immediately to the left of this pixel is duplicated and then "
        "modified. The bit combinations from planes 4-1 are used to "
        "replace the four \"green\" bits' (p.87), '1 1 Hold blue and red  "
        "G = Bitplane 4-1 contents' (Table 3-19, p.95). "
        "opus:3-ham.c16, sol:3-ham.c16, opus:3-ham.c17, sol:3-ham.c17, 3-t19.c11, 3-table3-19.c10, 3-table3-19.c8, "
        "3-table3-19.c9 .");

    // ---------------- HAM-SINGLE-REGISTER-TECHNIQUE-COLOR00 ----------------
    checks.expect(
        rgb4 == rgb0,
        "HAM-SINGLE-REGISTER-TECHNIQUE-COLOR00: after three modify steps "
        "(01, 10, 11) that never write any colour register, re-selecting "
        "control=00 data=0 still reads COLOR00 exactly as originally "
        "loaded (0xAABBCC) -- the one register the whole four-pixel "
        "sequence above was built from, confirming it was never itself "
        "touched by the modify path, as the manual's own technique "
        "assumes -- 'it is possible to get by with defining only one "
        "color register, which is COLOR00, the color of the background. "
        "You treat the entire screen as a modification of that original "
        "color, according to the scheme above' (p.87). "
        "sol:3-ham.c18, opus:3-ham.c19, sol:3-ham.c19 .");

    // ---------------- HAM-CONTROL-00-TABLE-MAPS-VALUES-TO-REGISTERS-0-15 ----------------
    {
        Harness h;
        // Registers 0-5, 7-15 each loaded with a distinct value N*0x111
        // (repeated nibble N in R, G and B) so selecting register N and no
        // other reads back (N*0x11) in every byte. Register 6 is
        // deliberately skipped: printed Table 3-17's own row for register 6
        // reads "00110  0100  110  6" -- the hold-and-modify column repeats
        // register 4's code (0100) instead of a distinct one, a defect in
        // the printed table itself. Neither reader created a claim about
        // register 6 (grepped the 77-record inventory: no such id exists),
        // so there is nothing to assert or decline for it specifically.
        const int regs[] = {0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        for (int n : regs) h.write_color(n, static_cast<uint16_t>(n * 0x111));

        bool all_ok = true;
        for (int n : regs) {
            uint32_t got = h.set_pixel(0x0, static_cast<uint8_t>(n));
            uint32_t want = static_cast<uint32_t>(((n * 0x11) << 16) | ((n * 0x11) << 8) | (n * 0x11));
            if (got != want) all_ok = false;
        }

        checks.expect(all_ok,
            "HAM-CONTROL-00-TABLE-MAPS-VALUES-TO-REGISTERS-0-15: with the "
            "6-5 control field held at 00 (normal selection), each of the "
            "15 distinctly-loaded data-field values 0,1,2,3,4,5,7,8,9,10,"
            "11,12,13,14,15 (planes 4-1) reads back exactly its own colour "
            "register's content and no other's -- Table 3-17's "
            "low-resolution single-playfield hold-and-modify column, row "
            "by row: '00000  0000 ... 0', '00001  0001 ... 1', '00010  "
            "0010 ... 2', '00011  0011 ... 3', '00100  0100 ... 4', "
            "'00101  0101 ... 5', '00111  0111 ... 7', '01000  1000 ... "
            "8' through '01111  1111 ... 15' (p.93), heading '(Bitplanes "
            "4,3,2,1) ... Number' (p.93), and 'Thus, the bit combinations "
            "from planes 4-1, in that order of significance, are used to "
            "choose one of 16 color registers (registers 0-15)' (p.86). "
            "The data field (select bits [3:0]) is 4 bits wide by "
            "construction, so it cannot represent 16 through 31 at all -- "
            "the mechanism Table 3-17's own rows 16-31 describe as 'NOT "
            "USED IN THIS MODE' (p.93). "
            "3-t17.h00, 3-t17.h01, 3-t17.h02, 3-t17.h03, 3-t17.h04, "
            "3-t17.h05, 3-t17.h07, 3-t17.h08, 3-t17.h09, 3-t17.h10, "
            "3-t17.h11, 3-t17.h12, 3-t17.h13, 3-t17.h14, 3-t17.h15, "
            "3-t17.hdr2, 3-t17.notused1, "
            "3-table3-17.c2, 3-table3-17.c5, 3-table3-17.c8, "
            "3-table3-17.c11, 3-table3-17.c14, 3-table3-17.c17, "
            "3-table3-17.c23, 3-table3-17.c26, 3-table3-17.c29, "
            "3-table3-17.c32, 3-table3-17.c35, 3-table3-17.c38, "
            "3-table3-17.c41, 3-table3-17.c44, 3-table3-17.c47 .");
    }

    // ---------------- HAM-DATA-FIELD-SPANS-16-LEVELS ----------------
    {
        Harness h;
        h.write_color(0, 0x000);  // COLOR00 = black; red/green start held at 0x00
        bool all_ok = true;
        std::vector<uint32_t> seen;
        for (int data = 0; data <= 15; ++data) {
            h.set_pixel(0x0, 0x0);  // re-select COLOR00 (control=00) so every measurement starts from the same held red/green
            uint32_t got = h.set_pixel(0x1, static_cast<uint8_t>(data));  // control=01: modify blue
            uint32_t want_blue = static_cast<uint32_t>(data * 0x11);
            if ((got & 0xFFu) != want_blue) all_ok = false;
            seen.push_back(got & 0xFFu);
        }
        bool all_distinct = std::set<uint32_t>(seen.begin(), seen.end()).size() == 16;

        checks.expect(all_ok && all_distinct,
            "HAM-DATA-FIELD-SPANS-16-LEVELS: sweeping the 4-bit data field "
            "(planes 4-1) through all 16 of its values under control=01 "
            "(modify blue) produces 16 distinct blue values, each exactly "
            "the data value duplicated to a byte -- the resolution basis "
            "for 'This is a special mode that allows you to produce up to "
            "4,096 colors on the screen at the same time' (p.86): 16 "
            "data-field levels per modified component (measured here), "
            "times the 3 independently-selectable components "
            "(HAM-CONTROL-01/10/11-... above), times the 16-entry base "
            "palette (HAM-CONTROL-00-TABLE-... above), is the 16x16x16 = "
            "4,096 the manual describes; this assertion measures only the "
            "first factor, the 16 achievable data-field levels. "
            "3-ham.c01, 3-ham.c1 .");
    }

    // ---------------- NOT ASSERTED ----------------
    checks.note(
        "NOT ASSERTED 3-ham.c11 (sol, p.86): \"If only five bitplanes are "
        "used, the data from the sixth plane is automatically supplied "
        "with the value as 0.\" This describes how the bitplane "
        "fetch/shift-register logic assembles the 6-bit HAM sample BEFORE "
        "it reaches this module: denise_hamgenerator's own ports "
        "(reg_address_in, data_in, select, bplxor, bank, loct, ham8, rgb "
        "-- module header ll.6-18) receive `select` only after that "
        "assembly, with no way to tell 'plane 6 forced to 0 because only "
        "5 planes are active' apart from 'plane 6 genuinely samples 0'. "
        "Settling it needs a wider boundary (the bitplane shifter / BPU "
        "decode), not this fixture.");

    checks.note(
        "NOT ASSERTED A-leg.homod1 (opus, p.271): \"HOMOD=Hold-and-modify "
        "mode (1 = Hold-and-modify mode (HAM);\" denise_hamgenerator has "
        "no BPLCON0, HOMOD or DBLPF port at all (same port list as "
        "above), so which bit of BPLCON0 enables HAM is not observable at "
        "this boundary, disputed or not. Recorded as a MEASUREMENT, not "
        "an assertion, per the build brief: printed p.95 says 'This mode "
        "is in effect only if bit 10 of BPLCON0 = 1'; printed p.87 says "
        "'Bit 11 of register BPLCON0 selects hold-and-modify mode ... Bit "
        "HOMOD, bit 11, is 1. Bit DBLPF, bit 10, is 0'; printed p.90's "
        "BPLCON0 legend lists 'Bit 11 - HOMOD (hold-and-modify enable)' "
        "and 'Bit 10 - DBLPF (double-playfield enable)'; printed appendix "
        "p.271 lists bit 11 as HOMOD and bit 10 as DBLPF in both the "
        "register table and its legend. Three of four printed citations "
        "(87, 90, 271) agree the enable bit is 11 (HOMOD), with bit 10 "
        "(DBLPF) required to be 0; one (95) disagrees and calls bit 10 "
        "the enable. Not resolved here. Undisputed and asserted nowhere "
        "because it is unobservable at THIS boundary regardless of who is "
        "right: some BPLCON0 bit gates hold-and-modify, and the modify "
        "control itself comes from planes 6 and 5 -- that undisputed half "
        "is asserted by HAM-CONTROL-00/01/10/11-... above, fully within "
        "this module's boundary.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
