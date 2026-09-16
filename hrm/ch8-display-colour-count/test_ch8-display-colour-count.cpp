// ch8-display-colour-count -- denise_colortable conformance test
//
// Test purpose: p.260's "up to 4,096 different colors on-screen
//   simultaneously" figure (RGB Monitors bullet, chapter 8 "Display Output
//   Connections"). Both claims in this package's claims file
//   (opus:8-display.c13, sol:8-display.c12) restate that same sentence.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 260; committed specification 79e9104 (SPECIFICATION.md, this
//   package).
// Requirement IDs: opus:8-display.c13, sol:8-display.c12 -- both graded by
//   the single PALETTE-RESOLVES-4096-CODES assertion below. Claims file:
//   hrm-testpipeline/work/ch8-s4-build/ch8-display-colour-count.jsonl
//   (2 claims).
//
// Disposition -- REPAIRED after coordinator review; recorded here because it
//   changed what this file asserts (change history belongs in the commit
//   message, not SPECIFICATION.md, but this paragraph explains the shape of
//   what follows, which the commit message alone does not):
//   this file's first version declined both claims with record(), citing
//   hrm/ch3-colour-register-format's SWEEP-12BIT-DISTINCT (same mechanism,
//   same number, p.46) rather than measuring them here. The coordinator
//   pointed out that record-and-cite is the rule for the 53 claims elsewhere
//   in this slice that carry a classify.note naming an accepted package's
//   own uid -- these two carry no such note (package list: "already graded
//   elsewhere: 0" for this package), and a citation is not a uid on a graded
//   line: the ledger read this package as grading neither claim. There is
//   also a reason of substance: this package's claims cite p.260, ch3's
//   assertion cites p.46 -- a claim is graded against the page it cites, and
//   if those two pages ever disagreed, citing ch3 would quietly substitute
//   its oracle for this one's. Both uids are graded below, against p.260
//   directly, by sweeping all 4,096 possible 12-bit codes into one register
//   and counting distinct readings -- not asserted from data_in's declared
//   width. ch3's result is carried as corroboration in that assertion's own
//   note(), not as its oracle.
//
//   Establishing which reading page 260 describes is not free, though: the
//   colour store here is 24 bits, written in two 12-bit halves selected by
//   `loct` (denise_colortable.v:17,33; denise_colortable_ram_mf.v:16-20) --
//   loct low writes both halves alike (the 4,096-code OCS/ECS reading page
//   260 describes); loct high writes only the low half, reaching colours
//   outside that 4,096-code space (the AGA path, which page 260's claims are
//   not about). ch3-colour-register-format's own Harness holds loct at 0
//   throughout but never says why, because none of ITS claims turn on the
//   distinction. This package's own LOCT-SELECTS-4096-READING control,
//   below, is what establishes -- measured, not assumed -- that
//   PALETTE-RESOLVES-4096-CODES is graded on the correct (loct-low) basis.
//
// BOUNDARY: top module `denise_colortable` only (one Verilator build,
//   sources denise_colortable.v + denise_colortable_ram_mf.v), per the
//   package list's top-module assignment. Ports: clk, clk7_en,
//   reg_address_in[8:1], data_in[11:0], rdram, select[7:0], bplxor[7:0],
//   bank[2:0], loct, ehb_en, rgb[23:0] (denise_colortable.v:9-19). The
//   Harness's tick/write/read shape and the reg_address_in write-address
//   convention (COLORBASE 9'h180 -> word address $C0..$DF, i.e.
//   reg_address_in = 0xC0 + register index) are carried over from
//   hrm/ch3-colour-register-format's own harness, read in full as the
//   sibling package PALETTE-RESOLVES-4096-CODES corroborates against --
//   disclosed here rather than silently reused, per that package's own
//   precedent for disclosing borrowed plumbing.
//
// Non-goal: whether "on-screen simultaneously" additionally requires HAM
//   (denise_hamgenerator, instantiated in denise.v, outside this boundary)
//   is addressed in SPECIFICATION.md's "What is testable at this boundary,
//   and what is not" -- the numeric content both claims carry (4,096 = 2^12
//   reachable, distinguishable colour codes) does not depend on it, and no
//   fixture built only from denise_colortable can reach a HAM claim anyway.
//
// Every mutation below was replayed as a `-D` build variant (MUT_n, see
// Makefile: `make MUT=MUT_n test`), confirmed red in isolation, then
// rebuilt with no flags to confirm the clean baseline result returned.
//
// MUTATION FIXTURE-LIVE stimulus red MUT_0: the second write's value changed from a distinct sentinel (0x0F0) to a repeat of the first (0x1F0), so the two reads no longer differ.
// MUTATION LOCT-SELECTS-4096-READING stimulus red MUT_1: the loct-high write's value changed from Y (0x3E1) to X (0xA5C) -- the value already resident in both halves from the preceding loct-low write -- so the low half no longer diverges from the high half; the second reading collapses back to the doubled/OCS pattern the first reading already produced, instead of the independently-blended value (hi nibble from X, lo nibble from Y, per channel) the assertion expects.
// MUTATION opus:8-display.c13 stimulus red MUT_2: the swept 12-bit stimulus was masked to 11 bits (v & 0x7FF) before being written, so bit 11 never varies; the sweep still presents 4,096 loop iterations but only 2,048 distinct codes ever reach data_in, and the distinct-reading count comes back 2,048, not 4,096.

#include <cstdio>
#include <iostream>
#include <set>
#include <string>
#include "Vdenise_colortable.h"
#include "verilated.h"

#ifndef MUT_0
#define MUT_0 0
#endif
#ifndef MUT_1
#define MUT_1 0
#endif
#ifndef MUT_2
#define MUT_2 0
#endif

namespace {

constexpr uint8_t COLOR_ADDR_BASE = 0xC0;  // reg_address_in word-address of COLOR00 ($180/2); COLORx = 0xC0+x.
constexpr uint8_t IDLE_ADDR = 0x00;        // outside 0xC0-0xDF: the write decode will not strobe any color register here.

std::string hex6(uint32_t v) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "$%06X", static_cast<unsigned>(v & 0xFFFFFF));
    return std::string(buf);
}

class Harness {
public:
    Harness() : dut(new Vdenise_colortable) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->rdram = 0;
        dut->bplxor = 0;
        dut->bank = 0;
        dut->loct = 0;
        dut->ehb_en = 0;
        dut->select = 0;
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        tick();
    }
    ~Harness() { delete dut; }

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    // Writes `val` (0..0xFFF; data_in is declared [11:0]) to color register
    // `idx` (0..31) via reg_address_in/data_in, with `loct` held at the
    // given level for the write -- loct selects which of the 24-bit store's
    // two 12-bit halves this write updates (denise_colortable.v:33,
    // denise_colortable_ram_mf.v:16-20). loct is returned to 0 (the
    // OCS/ECS baseline this package cares about) after every write so it is
    // never left in an unstated state between calls -- held deliberately,
    // not left floating.
    void write_reg(int idx, uint32_t val, bool loct_high = false) {
        dut->loct = loct_high ? 1 : 0;
        dut->reg_address_in = static_cast<uint8_t>(COLOR_ADDR_BASE + idx);
        dut->data_in = val;
        tick();
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        tick();
        dut->loct = 0;
    }

    // Selects color register `idx` (0..31) for output and returns rgb after
    // two ticks (mirrors hrm/ch3-colour-register-format's harness, which
    // reads twice after changing `select`).
    uint32_t read_reg(int idx) {
        dut->ehb_en = 0;
        dut->select = static_cast<uint8_t>(idx);
        tick();
        tick();
        return dut->rgb;
    }

    Vdenise_colortable* dut;
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =========================================================================
    // FIXTURE-LIVE -- denise_colortable has never been measured by this
    // package before. Two distinct, known 12-bit codes, written with loct
    // held low (the baseline this whole package is about), must both arrive
    // and read back distinctly before anything below is trusted.
    // =========================================================================
    {
        Harness h;
        h.write_reg(4, 0x1F0, /*loct_high=*/false);
        uint32_t a = h.read_reg(4);
#if MUT_0
        h.write_reg(4, 0x1F0, /*loct_high=*/false);
#else
        h.write_reg(4, 0x0F0, /*loct_high=*/false);
#endif
        uint32_t b = h.read_reg(4);
        checks.expect(
            a != b,
            "FIXTURE-LIVE: two distinct known 12-bit codes ($1F0 then $0F0), "
            "written with loct held low, read back through select/rgb as "
            "two distinct values before any assertion below trusts that "
            "path [no page -- infrastructure]");
        checks.note("FIXTURE-LIVE: " + hex6(a) + " -> " + hex6(b));
    }

    // =========================================================================
    // LOCT-SELECTS-4096-READING -- measurement-basis control, no claim uid.
    // The colour store is 24 bits in two 12-bit halves (ram0/ram1 in
    // denise_colortable_ram_mf.v), gated by loct's write-enable pair
    // (denise_colortable.v:33, wr_ws = loct ? 2'b01 : 2'b11). A loct-low
    // write enables both halves together with the same 12-bit value; a
    // loct-high write enables only the low half (ram0), leaving whatever
    // the high half (ram1) last held from an earlier loct-low write.
    //
    // Sequence, one register: (1) loct-low write of X=$A5C -- both halves
    // become X, so each rgb channel byte is X's own nibble doubled
    // (R=$AA,G=$55,B=$CC: R/G/B nibble order per denise_colortable.v:46/92's
    // own bit map, already established by ch3-colour-register-format's
    // RED/GREEN/BLUE-FIELD assertions). (2) loct-high write of Y=$3E1 to
    // the SAME register -- only the low half moves to Y; the high half
    // stays X, so each rgb channel byte becomes {X's own nibble, Y's own
    // nibble} -- R=$A3,G=$5E,B=$C1 -- a byte no loct-low-only write could
    // ever produce (its two nibbles differ), i.e. outside the 4,096-code
    // OCS/ECS space. (3) loct-low write of Z=$246 to the SAME register --
    // both halves move together again, back to a doubled byte
    // (R=$22,G=$44,B=$66), proving loct's control over which reading is in
    // effect is real and reversible, not a one-way artifact of reset state.
    // =========================================================================
    {
        Harness h;
        constexpr uint16_t X = 0xA5C;
        constexpr uint16_t Y = 0x3E1;
        constexpr uint16_t Z = 0x246;
        constexpr uint32_t EXPECT_1 = 0xAA55CC;  // X doubled, per channel nibble
        constexpr uint32_t EXPECT_2 = 0xA35EC1;  // hi nibble from X, lo nibble from Y
        constexpr uint32_t EXPECT_3 = 0x224466;  // Z doubled, per channel nibble

        h.write_reg(7, X, /*loct_high=*/false);
        uint32_t rgb1 = h.read_reg(7);

#if MUT_1
        h.write_reg(7, X, /*loct_high=*/true);
#else
        h.write_reg(7, Y, /*loct_high=*/true);
#endif
        uint32_t rgb2 = h.read_reg(7);

        h.write_reg(7, Z, /*loct_high=*/false);
        uint32_t rgb3 = h.read_reg(7);

        const bool ok = rgb1 == EXPECT_1 && rgb2 == EXPECT_2 && rgb3 == EXPECT_3;
        checks.expect(
            ok,
            "LOCT-SELECTS-4096-READING: with loct held low, a write updates "
            "both 12-bit halves of the 24-bit store alike, so the reading "
            "is exactly the doubled-nibble, 4,096-code OCS/ECS space page "
            "260 describes ($A5C -> $AA55CC); with loct held high on the "
            "next write to the SAME register, only the low half moves, "
            "producing a byte per channel no loct-low-only write could "
            "reach ($A5C then $3E1 -> $A35EC1, not $AA55CC and not "
            "$335511); a further loct-low write returns both halves to "
            "moving together ($246 -> $224466). Confirms, at this fixture, "
            "which reading PALETTE-RESOLVES-4096-CODES below is graded "
            "against and that loct is held low deliberately for it, not "
            "left floating [no page -- measurement-basis control for the "
            "assertion below]");
        checks.note("LOCT-SELECTS-4096-READING: loct-low " + hex6(rgb1) +
                     ", loct-high " + hex6(rgb2) + ", loct-low " + hex6(rgb3));
    }

    // =========================================================================
    // opus:8-display.c13 sol:8-display.c12 -- PALETTE-RESOLVES-4096-CODES
    // (p.260): both claims restate "up to 4,096 different colors on-screen
    // simultaneously". Graded here directly against p.260, on the loct-low
    // basis LOCT-SELECTS-4096-READING establishes above: sweep every one of
    // the 4,096 possible 12-bit codes into one colour register and count
    // how many pairwise-distinct rgb readings result -- measured by
    // sweeping and counting, not asserted from data_in's declared width.
    // hrm/ch3-colour-register-format's SWEEP-12BIT-DISTINCT graded the
    // identical mechanism against p.46's identical figure and passed;
    // that is corroboration, carried in the note() below, not this
    // assertion's oracle -- p.260 is.
    // =========================================================================
    {
        Harness h;
        std::set<uint32_t> distinct;
        for (uint32_t v = 0; v < 4096; ++v) {
#if MUT_2
            const uint32_t stim = v & 0x7FFu;  // bit 11 never varies -- 2,048 codes, not 4,096
#else
            const uint32_t stim = v;
#endif
            h.write_reg(9, stim, /*loct_high=*/false);
            distinct.insert(h.read_reg(9));
        }
        checks.expect(
            distinct.size() == 4096,
            "opus:8-display.c13 sol:8-display.c12 PALETTE-RESOLVES-4096-CODES: "
            "sweeping all 4,096 possible 12-bit codes ($000-$FFF) into one "
            "colour register, with loct held low, and counting the "
            "pairwise-distinct rgb readings yields exactly 4,096 -- the "
            "full 12-bit field selects among 4,096 distinct, "
            "distinguishable colours, measured by sweeping and counting "
            "rather than asserted from data_in's declared width, on the "
            "loct-low basis LOCT-SELECTS-4096-READING confirms above "
            "[p.260 \"They can generate up to 4,096 different colors "
            "on-screen simultaneously\"]");
        checks.note(
            "PALETTE-RESOLVES-4096-CODES: " + std::to_string(distinct.size()) +
            " distinct rgb readings out of 4,096 codes swept. "
            "Corroboration, not this assertion's oracle: "
            "hrm/ch3-colour-register-format's SWEEP-12BIT-DISTINCT grades "
            "the identical mechanism against p.46's identical figure and "
            "passed (55 passed, 0 failed, re-run 16.09.2026) -- this "
            "assertion is graded against p.260 directly and independently, "
            "since a claim is graded against the page it cites.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
