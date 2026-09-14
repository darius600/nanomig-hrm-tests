// Test purpose: HRM conformance package for Appendix B's printed p.289
//   register-map rows JOY0DAT, JOY1DAT and JOYTEST, composed with p.287's
//   chip-letter legend ("D=Denise chip"), against the `denise` module
//   compiled alone. 6 claims (3 propositions, each claimed independently by
//   readers opus and sol as twins), from
//   work/ch8-s1-build/ch8-appb-chip-column-denise.jsonl. Full derivation in
//   SPECIFICATION.md (committed as feaf659 before this file was written).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   p.289 (Appendix B row for each register), composed with printed p.287
//   (the chip-letter legend).
// Fixture boundary: `denise` (denise.v) and every module it itself
//   instantiates -- denise_bitplanes (bplm0, denise.v:345, which itself
//   instantiates denise_bitplane_shifter, denise_bitplanes.v:346/472-585,
//   which itself instantiates bitplane_ram), denise_playfields (plfm0,
//   denise.v:377), denise_sprites (sprm0, denise.v:393, which itself
//   instantiates denise_sprites_shifter, denise_sprites.v:170-321, which
//   itself instantiates sprite_ram), denise_spritepriority (spm0,
//   denise.v:419), denise_colortable (clut0, denise.v:440, which itself
//   instantiates denise_colortable_ram_mf, denise_colortable.v:39), and
//   denise_hamgenerator (ham0, denise.v:459, which also instantiates
//   denise_colortable_ram_mf, denise_hamgenerator.v:38), and
//   denise_collision (col0, denise.v:474). No other module is compiled into
//   this package -- confirmed by this file's own Makefile SRC list, the only
//   place a module could be added.
//
// THE CHIP-COLUMN BOUNDARY SPLIT -- the point this package exists to make,
//   and the whole reason this package has zero expect()-graded claims.
//   Measured directly against this RTL (grep over denise.v and every module
//   listed above, zero hits for "00a", "00c" or "036" as an address):
//   JOY0DAT ($00A), JOY1DAT ($00C) and JOYTEST ($036) are decoded nowhere in
//   `denise`'s own hierarchy. denise.v's own address table (denise.v:56-65)
//   is DIWSTRT $08E, DIWSTOP $090, DIWHIGH $1E4, BPLCON0 $100, BPLCON2 $104,
//   BPLCON3 $106, BPLCON4 $10C, DENISEID $07C, BPL1DAT $110 and COLORBASE
//   $180 (base of a range, denise.v:221/434). None of the seven submodules
//   listed above decode any address at all except denise_bitplanes,
//   denise_sprites, denise_colortable, denise_hamgenerator and
//   denise_collision, and grep over all of them for "00a"/"00c"/"036" is
//   also zero hits.
//   JOY0DAT/JOY1DAT/JOYTEST ARE decoded in userio.v (parameters JOY0DAT =
//   9'h00a, JOY1DAT = 9'h00c at userio.v:54-55, JOYTEST = 9'h036 at
//   userio.v:61, used at userio.v:290-317), a SIBLING module instantiated as
//   USERIO1 in minimig.v:551 -- never inside denise.v (instantiated
//   separately as DENISE1 at minimig.v:579), and not part of this package's
//   Makefile SRC list. Both USERIO1 and DENISE1 are wired to the same
//   `reg_address`/`custom_data_in` buses in minimig.v (minimig.v:556-557,
//   588-589), confirming they are address-bus siblings, not a
//   parent/submodule pair.
//   A CHIP-column letter is a claim about which chip answers at an address;
//   a fixture that never receives userio.v's decode logic can return neither
//   a yes nor a no about it -- denise.v's own data_out is an unconditional
//   OR of its own sub-signals (denise.v:102/130, `col_out | deniseid_out |
//   rgb_out`) with no chip-select/acknowledge output of any kind, so driving
//   an address absent from denise's own table simply reads back 0x0000,
//   which is indistinguishable from "claimed and returns zero". All three
//   rows are DECLINED with record(), naming userio.v, per this task's
//   carried-forward rule for CHIP-column claims (the accepted shape used by
//   ch8-appb-chip-column-paula on this same manual page, for its own
//   userio.v-decoded rows) -- the fixture is not widened to force an answer,
//   and this is not graded as a manual-vs-RTL disagreement.
//
// FIXTURE LIVENESS (A0). Independent of any claim: denise.v's OWN internal
//   BPLCON0 register ($100, denise.v:59/175-176) is round-tripped with two
//   distinct words before any claim-bearing register is trusted, proving the
//   reg_address_in -> internal register -> observable-output path is alive
//   at all. BPLCON0 is used rather than a register with a read-back path
//   because two of its bits are wired straight to denise's own top-level
//   output ports -- bit 15 to `hires` (denise.v:180) and bit 6 to `shres`
//   (denise.v:187) -- so "the value arrived" is observed on a primary
//   output, not inferred from a read multiplexer that could itself be the
//   thing under test.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup -- not a compile-time #ifdef -- so one build serves the real run
//   (MUTATE unset) and every mutation run (MUTATE=<id>, one id per
//   assertion), following the same technique disclosed as read from
//   ch8-appb-chip-column-paula's test_paula.cpp. Each mutation changes what
//   the module is ASKED TO DO (a written value) and leaves the `expect()`
//   call's own expected-value code untouched. Both were run with their own
//   id, confirmed red, then reverted (MUTATE unset) and confirmed green
//   again; results are logged next to each assertion and summarised in the
//   build report. Baseline with MUTATE unset: 2 passed, 0 failed.
//
// MUTATION A0-LIVENESS-BPLCON0-HIRES stimulus red: the written word's bit 15 is cleared (0x0000 instead of 0x8000), so hires never goes high (MUTATE=A0_1)
// MUTATION A0-LIVENESS-BPLCON0-SHRES stimulus red: the second word replays the first (0x8000) instead of a different one (0x0040), so shres never goes high and the observation does not change from the first (MUTATE=A0_2)

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include "Vdenise.h"
#include "verilated.h"

namespace {
const char* g_mutate = nullptr;
inline bool mutating(const char* id) {
    return g_mutate != nullptr && std::strcmp(g_mutate, id) == 0;
}
}  // namespace

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

namespace {
std::string hex4(uint16_t v) {
    char b[8];
    std::snprintf(b, sizeof b, "%04X", v);
    return std::string(b);
}
}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Vdenise* dut = new Vdenise;
    Checks checks;

    int clk7_cycle = 0;
    auto tick = [&]() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        clk7_cycle = (clk7_cycle + 1) % 4;
        dut->clk7_en = (clk7_cycle == 0);
    };

    auto do_reset = [&]() {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->c1 = 0;
        dut->c3 = 0;
        dut->cck = 0;
        dut->reset = 1;
        dut->strhor = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->chip48 = 0;
        dut->blank = 0;
        dut->a1k = 0;
        dut->ecs = 1;
        dut->aga = 1;
        for (int i = 0; i < 16; i++) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; i++) tick();
    };

    auto write_reg = [&](int addr, uint16_t data) {
        while (!dut->clk7_en) tick();
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick(); tick(); tick(); tick();
        dut->reg_address_in = 0;
        dut->data_in = 0;
    };

    auto read_reg = [&](int addr) -> uint16_t {
        while (!dut->clk7_en) tick();
        dut->reg_address_in = addr;
        tick();
        uint16_t val = dut->data_out;
        tick(); tick(); tick();
        dut->reg_address_in = 0;
        return val;
    };

    // Register addresses: documented Amiga custom-chip byte offsets (printed
    // Appendix B), presented as word indices (byte address >> 1) on
    // reg_address_in[8:1] -- confirmed at denise.v:39 (`input wire [9-1:1]
    // reg_address_in`) and denise.v:56-65's own address table, and used the
    // same way by every package in this tree (e.g.
    // ch8-appb-chip-column-paula's own ADKCON/SERDAT addresses).
    const int BPLCON0 = 0x80;  // write, byte $100 -- denise.v:59
    const int JOY0DAT = 0x05;  // byte $00A -- userio.v:54, NOT in this fixture
    const int JOY1DAT = 0x06;  // byte $00C -- userio.v:55, NOT in this fixture
    const int JOYTEST = 0x1B;  // byte $036 -- userio.v:61, NOT in this fixture

    // ========================================================================
    // A0 -- FIXTURE DATA-PATH LIVENESS. No claim id, no manual page: the
    // BUILD-BRIEF's own requirement that the fixture prove it can deliver a
    // known, distinct word before any claim-bearing assertion is trusted.
    // Uses denise.v's own BPLCON0 register and its own hires/shres output
    // ports (denise.v:59, 175-176, 180, 187) -- none of the three claim
    // rows below -- so it cannot be confused with any of them.
    // ========================================================================
    do_reset();
    {
        uint16_t w1 = mutating("A0_1") ? 0x0000 : 0x8000;  // MUTATION: bit 15 never set
        write_reg(BPLCON0, w1);
        bool hires1 = dut->hires;
        bool shres1 = dut->shres;
        checks.note("BPLCON0-FIRST-WRITE: BPLCON0=0x" + hex4(w1) + " gives hires=" +
                    std::to_string(hires1) + " shres=" + std::to_string(shres1));
        checks.expect(hires1 == 1 && shres1 == 0,
            "A0-LIVENESS-BPLCON0-HIRES: a known word (0x8000) written to BPLCON0 arrives at the hires output");

        uint16_t w2 = mutating("A0_2") ? 0x8000 : 0x0040;  // MUTATION: replay the stale first word
        write_reg(BPLCON0, w2);
        bool hires2 = dut->hires;
        bool shres2 = dut->shres;
        checks.note("BPLCON0-SECOND-WRITE: BPLCON0=0x" + hex4(w2) + " gives hires=" +
                    std::to_string(hires2) + " shres=" + std::to_string(shres2));
        checks.expect(hires2 == 0 && shres2 == 1,
            "A0-LIVENESS-BPLCON0-SHRES: a second, different known word (0x0040) also arrives, so the path is not stuck at the first value");
    }

    // ========================================================================
    // P1/P2/P3 -- JOY0DAT, JOY1DAT, JOYTEST: DECLINED. See the header for the
    // measurement (grep over this fixture's own compiled hierarchy, zero
    // hits) that established this.
    // ========================================================================
    do_reset();
    {
        uint16_t v = read_reg(JOY0DAT);
        checks.note("JOY0DAT-ADDRESS-0x00A-ON-DENISE-ALONE: reading word address 0x05 ($00A) on the isolated denise fixture returns 0x" + hex4(v));
        checks.record(
            "P1-JOY0DAT (p.289, opus:8-appB.c10 sol:8-appB.c9, quote 'JOY0DAT      *00A   R        D       Joystick-mouse "
            "0 data (vert,horiz)', composed with p.287's legend 'A=Agnus chip, D=Denise chip, P=Paula chip.'): not "
            "asserted -- JOY0DAT's address ($00A) is decoded nowhere in denise.v or any module it instantiates "
            "(measured: grep over denise.v, denise_bitplanes.v, denise_bitplane_shifter.v, bitplane_ram.v, "
            "denise_playfields.v, denise_sprites.v, denise_sprites_shifter.v, sprite_ram.v, denise_spritepriority.v, "
            "denise_colortable.v, denise_colortable_ram_mf.v, denise_hamgenerator.v and denise_collision.v, zero hits). "
            "It IS decoded in userio.v (parameter JOY0DAT = 9'h00a, userio.v:54, used at userio.v:290-292), a sibling "
            "module instantiated as USERIO1 in minimig.v:551 -- never inside denise.v, which is instantiated separately "
            "as DENISE1 at minimig.v:579, and not in this package's Makefile SRC list. A CHIP-column letter names which "
            "chip answers at an address; this fixture never receives userio.v's decode logic and so can return neither "
            "a yes nor a no about it. userio.v is the module that would decide this claim.");
    }
    do_reset();
    {
        uint16_t v = read_reg(JOY1DAT);
        checks.note("JOY1DAT-ADDRESS-0x00C-ON-DENISE-ALONE: reading word address 0x06 ($00C) on the isolated denise fixture returns 0x" + hex4(v));
        checks.record(
            "P2-JOY1DAT (p.289, opus:8-appB.c15 sol:8-appB.c14, quote 'JOY1DAT      *00C   R        D       Joystick-mouse "
            "1 data (vert,horiz)', composed with p.287's legend 'A=Agnus chip, D=Denise chip, P=Paula chip.'): not "
            "asserted -- JOY1DAT's address ($00C) is decoded nowhere in denise.v or any module it instantiates "
            "(measured: grep, zero hits, same file list as P1). It IS decoded in userio.v (parameter JOY1DAT = 9'h00c, "
            "userio.v:55, used at userio.v:294), instantiated as USERIO1 in minimig.v:551, never inside denise.v "
            "(DENISE1, minimig.v:579) and not in this package's SRC list. userio.v is the module that would decide "
            "this claim.");
    }
    do_reset();
    {
        uint16_t v = read_reg(JOYTEST);
        checks.note("JOYTEST-ADDRESS-0x036-ON-DENISE-ALONE: reading word address 0x1B ($036) on the isolated denise fixture returns 0x" + hex4(v));
        checks.record(
            "P3-JOYTEST (p.289, opus:8-appB.c55 sol:8-appB.c54, quote 'JOYTEST      *036   W        D       Write to all "
            "four joystick-mouse counters at once', composed with p.287's legend 'A=Agnus chip, D=Denise chip, P=Paula "
            "chip.'): not asserted -- JOYTEST's address ($036) is decoded nowhere in denise.v or any module it "
            "instantiates (measured: grep, zero hits, same file list as P1). It IS decoded in userio.v (parameter "
            "JOYTEST = 9'h036, userio.v:61, used at userio.v:317), instantiated as USERIO1 in minimig.v:551, never "
            "inside denise.v (DENISE1, minimig.v:579) and not in this package's SRC list. userio.v is the module that "
            "would decide this claim.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
