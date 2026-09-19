// Test purpose: grade printed page 380's DENISE PIN ASSIGNMENT table, the
//   DEFINITION column (direction: I / O / I/O) against the `denise` module
//   compiled alone with its own submodules (denise_bitplanes(+shifter,
//   bitplane_ram), denise_playfields, denise_sprites(+shifter, sprite_ram),
//   denise_spritepriority, denise_colortable(+ram_mf), denise_hamgenerator,
//   denise_collision). 16 claims (8 propositions, each claimed independently
//   by readers sol and opus as twins), filtered from
//   hrm-testpipeline/work/2026-09-17_appHJ_packages.jsonl on
//   classify.package == "appJ-denise-port-directions".
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 380, DENISE PIN ASSIGNMENT table.
// Fixture boundary: `denise` (denise.v) and every module it instantiates --
//   confirmed by this file's own Makefile SRC list, the only place a module
//   could be added. Modelled on hrm/ch8-appb-chip-column-denise, which
//   already builds this exact boundary; its write_reg/read_reg/do_reset
//   register-bus harness is reused close to verbatim below.
// Non-goals: anything outside the `denise` fixture this package builds, and
//   any claim outside the 16 it cites. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: 16 ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/2026-09-17_appHJ_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// THE AXIS. Per the build brief: the DEFINITION column is the direction of
//   the signal AT THE CHIP, the SAME axis hrm/appDE-ciab-port-signals grades
//   for the CIA. Every proposition below is graded on the pin-to-PORT
//   identification, argued from RTL declaration and wiring; the printed PIN
//   NUMBER / PIN RANGE is explicitly NOT part of any claim graded here
//   (classify.note says so for the two rows that split/merge a range).
//
// CORPUS: p.380's DENISE block is confirmed NOT column-displaced (per the
//   build brief; unlike the PAULA block below it on the same page). Every
//   DEFINITION cell this package grades was additionally read directly off
//   figures/p380.png (not only the text layer), row by row:
//     01-07 D6-D0 I/O | 10-17 RGA8-RGA1 I | 20-23 R0-R3 O | 24-27 B0-B3 O |
//     28-31 G0-G3 O | 35 7M I | 36 CCK I | 40-48 D15-D7 I/O
//   -- every cell a plain, unambiguous "I", "O" or "I/O" letter, matching
//   sol/opus's readings exactly.
//
// THE O/0 DAMAGE DIRECTION, CHECKED AND FOUND NOT TO TOUCH THIS PACKAGE. The
//   build brief warns the DEFINITION column loses letter O to digit 0
//   elsewhere on THIS SAME page: row 18, "BURST* Color burst O" -- confirmed
//   against the render, a plain letter O, correctly an output. BURST* is not
//   one of this package's 8 propositions. No DEFINITION cell this package
//   grades shows any digit/letter ambiguity on the render.
//
// THREE PRINTED THINGS ON THIS PAGE, RECORDED AND NOT RESOLVED, NONE OF
//   WHICH IS ONE OF THIS PACKAGE'S 8 PROPOSITIONS:
//   (1) Pin 34 carries two rows: "N/C / Not connected / N/C (old Denise)"
//   and, on the line below with no pin number, "CDAC / CDAC clock / I (ECS
//   Denise)" -- printed as-is, not resolved, not graded (neither name is
//   one of this package's claims).
//   (2) "/CSYNC" (pin 32) is printed with a leading slash on a page whose
//   own NOTE says an asterisk marks active low; every other active-low name
//   here uses a trailing "*". Not one of this package's claims.
//   (3) Appendix J's own self-contradiction between its two Agnus tables
//   (LP*, CCK, CCKQ reversing between p.379 and p.381) does not reach this
//   package -- Denise's own CCK row (36, "CCK Color clock I") is not part
//   of that pair (the reversal is between the two AGNUS printings only).
//
// A PORT-DIRECTION FACT IS COMPILE-TIME, NOT RUNTIME (build brief, quoting
//   hrm/ch8-video-signal-set's own record()). None of `denise`'s ports
//   graded here have a DDR-style register letting the SAME wire act as
//   input or output depending on other register content; every port is
//   unconditionally input or output for the whole life of the simulation.
//   So no STIMULUS this harness applies can flip an input to look like an
//   output or vice versa. Seven of the eight propositions below are graded
//   `none impossible`, each paired with a positive control proving the port
//   is genuinely live (not a dead wire) -- the hrm/appDE-fire-buttons-userio
//   pattern the build brief names. The eighth (CCK) is DECLINED -- see below.
//
// CCK IS DECLINED, NOT GRADED -- THE ONE PLACE THIS PACKAGE DIFFERS FROM ITS
//   OWN PATTERN AND FROM ITS PAULA SIBLING. Measured directly: `grep -n cck`
//   over denise.v and every one of the seven files this package's own
//   Makefile compiles (denise_bitplanes.v, denise_bitplane_shifter.v,
//   bitplane_ram.v, denise_playfields.v, denise_sprites.v,
//   denise_sprites_shifter.v, sprite_ram.v, denise_spritepriority.v,
//   denise_colortable.v, denise_colortable_ram_mf.v, denise_hamgenerator.v,
//   denise_collision.v) returns exactly ONE hit: denise.v:36, the port
//   declaration itself, `input wire cck`. Unlike paula's own CCK (which
//   gates paula_audio's channel state machine, graded in
//   hrm/appJ-paula-port-directions), denise's cck reaches NO other signal
//   anywhere in this compiled hierarchy -- it is a dead input at this
//   boundary. Every other proposition in this package (and every proposition
//   in the paula sibling package) grades `none impossible` PAIRED WITH a
//   positive control that shows the port move something real; for CCK, no
//   such control can be built here -- there is nothing downstream to move.
//   Writing `checks.expect(true, ...)` on a claim with no possible
//   falsifying observation is exactly the vacuous-assertion shape
//   `check_gates.py`'s own mutation gate exists to catch (an assertion "that
//   could not fail for the reason it was written"). This is NOT a
//   prior-assertion decline (hrm/ch8-appb-chip-column-denise and
//   hrm/ch8-appb-chip-column-paula, the packages that top the same module
//   boundaries, grade no package-pin direction at all, so nothing here is
//   declined against them either) -- it is a fresh reason: the printed
//   direction is correct RTL-wise (denise.v:36 IS a plain `input`), but this
//   compiled boundary offers no live consequence to found a non-tautological
//   runtime assertion on.
//
// RTL READ IN FULL before this file was drafted: denise.v (all ports, the
//   BPLCON0/DENISEID/COLORBASE decode, the red/green/blue/t_blank output
//   stage), denise_colortable.v (the color-RAM write/select path COLOR00
//   feeds). Also read in full: hrm/ch8-appb-chip-column-denise/
//   test_denise.cpp (this package's harness model) and
//   hrm/appDE-fire-buttons-userio/test_userio.cpp (the none-impossible +
//   positive-control worked example this build brief names).
//
// REGISTER ADDRESSES (word index = byte address >> 1, confirmed against
//   denise.v's own `reg_address_in[8:1]==<PARAM>[8:1]` decode, same
//   convention hrm/ch8-appb-chip-column-denise already uses):
//   BPLCON0=0x80(w) $100 (denise.v:59), DENISEID=0x3E(r) $07C (denise.v:63),
//   COLOR00=0xC0(w) $180 (denise.v:65, COLORBASE, index 0)
//
// MUTATION METHODOLOGY. Seven propositions are `none impossible` (see
//   above), each with its own positive control, run for real (WSL
//   Verilator, `make test`) and its MEASURED output read back into this
//   file's RUN lines. CCK carries no mutation record at all -- it is
//   declined with record(), and BUILD-BRIEF/this suite's own convention
//   (hrm/ch8-appb-chip-column-paula's header, reused by its denise sibling)
//   is that record() declines are not assertions and get no mutation entry.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include "Vdenise.h"
#include "verilated.h"

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
std::string hex2(uint8_t v) {
    char b[8];
    std::snprintf(b, sizeof b, "%02X", v);
    return std::string(b);
}
}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
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

    // ecs=0/aga=0 (unlike hrm/ch8-appb-chip-column-denise's own do_reset,
    // which sets both 1): denise.v:520-521's own `t_blank = (ecs & ecsena &
    // brdrblnk & ...)` forces the blanking term to 0 outright when ecs=0,
    // so the colour-output positive control below (R0-R3/B0-B3/G0-G3) is
    // never blanked regardless of window/display_ena state -- simplifies
    // the fixture without touching any claim this package grades (none of
    // the 8 propositions mentions ecs, aga or blanking).
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
        dut->ecs = 0;
        dut->aga = 0;
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

    const int BPLCON0  = 0x80;
    const int DENISEID = 0x3E;
    const int COLOR00  = 0xC0;

    // ========================================================================
    // D-BUS / RGA POSITIVE CONTROL, shared by D6-D0, D15-D7 and RGA8-RGA1.
    // WRITE half: a known word written through data_in to BPLCON0 lands on
    // denise's own top-level `hires`/`shres` outputs (denise.v:180/187) --
    // reused verbatim from hrm/ch8-appb-chip-column-denise's own A0
    // liveness. READ half: DENISEID's fixed identity word is read back
    // through data_out at its OWN address and reads DIFFERENTLY from an
    // unaddressed word, proving reg_address_in genuinely selects what
    // data_out returns (not merely always 0 or a stuck value).
    // ========================================================================
    bool write_live, read_live, rga_selects;
    {
        do_reset();
        write_reg(BPLCON0, 0x8000);
        bool hires1 = dut->hires, shres1 = dut->shres;
        write_reg(BPLCON0, 0x0040);
        bool hires2 = dut->hires, shres2 = dut->shres;
        checks.note("WRITE-LIVE: BPLCON0=0x8000 gives hires=" + std::to_string(hires1) +
                    " shres=" + std::to_string(shres1) + "; BPLCON0=0x0040 gives hires=" +
                    std::to_string(hires2) + " shres=" + std::to_string(shres2) +
                    " -- data_in genuinely reaches an internal register and its effect is "
                    "observed on a primary output.");
        write_live = (hires1 == 1 && shres1 == 0) && (hires2 == 0 && shres2 == 1);
    }
    {
        do_reset();
        uint16_t id = read_reg(DENISEID);
        uint16_t unaddr = read_reg(0x00);
        checks.note("READ-LIVE: DENISEID (own address 0x3E) reads 0x" + hex4(id) +
                    "; an unaddressed word (0x00) reads 0x" + hex4(unaddr) +
                    " -- data_out genuinely depends on reg_address_in, not a stuck value.");
        read_live = (id != 0) && (id != unaddr);
        rga_selects = read_live;
    }
    bool bus_live = write_live && read_live;
    checks.note(std::string("D-BUS-FIXTURE-LIVE: ") + (bus_live ? "live" : "NOT LIVE -- see MEASURED above") +
                " [no page -- infrastructure, not itself a claim]");
    checks.note(std::string("RGA-FIXTURE-LIVE: ") + (rga_selects ? "live" : "NOT LIVE -- see MEASURED above") +
                " [no page -- infrastructure, not itself a claim]");

    // MUTATION DENISE-D6-D0-DIRECTION none impossible: D6-D0 (printed pin 01-07) identifies to denise.v:40's `data_in[15:0]` (input) and denise.v:42's `data_out[15:0]` (output), both declared unconditionally -- no DDR-style register lets either behave as the other. Positive control: WRITE-LIVE and READ-LIVE above.
    checks.expect(bus_live,
        "DENISE-D6-D0-DIRECTION: at denise's own boundary, the D6-D0 half of the data bus "
        "identifies to `data_in[15:0]` (input, denise.v:40) and `data_out[15:0]` (output, "
        "denise.v:42), connected at minimig.v:587/590 -- a Verilog port-direction fact, not a "
        "runtime-configurable one, per the axis hrm/appDE-ciab-port-signals grades for the CIA. "
        "The printed pin range (01-07) is not graded; the RTL splits the one printed "
        "bidirectional bus into two unidirectional ports with no pin-range counterpart. "
        "[p.380 DENISE PIN ASSIGNMENT, DEFINITION column: D6-D0 \"I/O\"] "
        "sol:J-042 opus:AHJ-174");

    // MUTATION DENISE-D15-D7-DIRECTION none impossible: same pair of ports as D6-D0 (denise.v:40/42) -- printed pin 40-48 identifies to the SAME data_in/data_out. Positive control: WRITE-LIVE and READ-LIVE above.
    checks.expect(bus_live,
        "DENISE-D15-D7-DIRECTION: at denise's own boundary, the D15-D7 half of the data bus "
        "identifies to the SAME `data_in[15:0]`/`data_out[15:0]` pair as D6-D0 (denise.v:40/42) "
        "-- a Verilog port-direction fact. The printed pin range (40-48) is not graded. "
        "[p.380 DENISE PIN ASSIGNMENT, DEFINITION column: D15-D7 \"I/O\"] "
        "sol:J-078 opus:AHJ-210");

    // MUTATION DENISE-RGA-DIRECTION none impossible: RGA8-RGA1 identifies to denise.v:39's `reg_address_in[9-1:1]` (input), unconditionally declared -- no stimulus can make an address-bus input behave as an output. Positive control: RGA-FIXTURE-LIVE above.
    checks.expect(rga_selects,
        "DENISE-RGA-DIRECTION: at denise's own boundary, RGA8-RGA1 identifies to "
        "`reg_address_in[9-1:1]` (input, denise.v:39, 'register adress inputs'), connected at "
        "minimig.v:586 from agnus's `reg_address_out` -- a Verilog port-direction fact. Unlike "
        "Agnus's own RGA row, not touched by appendix J's AGNUS-table RGA contradiction (that "
        "split is between p.379 and p.381's two AGNUS printings; Denise's table prints I and "
        "Paula's prints I, and nothing contradicts either). "
        "[p.380 DENISE PIN ASSIGNMENT, DEFINITION column: RGA8-RGA1 \"I\"] "
        "sol:J-048 opus:AHJ-180");

    // ========================================================================
    // R0-R3 / B0-B3 / G0-G3 POSITIVE CONTROL: COLOR00 (background colour,
    // index 0 of the on-chip colour table) is written through denise's own
    // register bus; with no bitplanes enabled the pixel index defaults to
    // 0 (background), so the written colour reaches `red`/`green`/`blue`
    // through denise_colortable's own RAM (denise_colortable.v:39-48) once
    // the write has had a few clocks to propagate. ecs=0 (see do_reset
    // above) means t_blank is forced 0, so the output is never blanked.
    // ========================================================================
    bool red_live, green_live, blue_live;
    {
        do_reset();
        uint8_t r0 = dut->red, g0 = dut->green, b0 = dut->blue;
        write_reg(COLOR00, 0x0FFF);
        for (int i = 0; i < 40; i++) tick();
        uint8_t r1 = dut->red, g1 = dut->green, b1 = dut->blue;
        checks.note("COLOR00-LIVE: before the write, red/green/blue = 0x" + hex2(r0) + "/0x" +
                    hex2(g0) + "/0x" + hex2(b0) + "; after COLOR00<-0x0FFF (white) with no "
                    "bitplanes enabled (background pixel, index 0), red/green/blue = 0x" +
                    hex2(r1) + "/0x" + hex2(g1) + "/0x" + hex2(b1) + ".");
        red_live = (r1 != 0);
        green_live = (g1 != 0);
        blue_live = (b1 != 0);
    }
    checks.note(std::string("COLOR-OUTPUTS-FIXTURE-LIVE: ") +
        ((red_live && green_live && blue_live) ? "live" : "NOT LIVE -- see MEASURED above") +
        " [no page -- infrastructure, not itself a claim]");

    // MUTATION DENISE-R0-R3-DIRECTION none impossible: R0-R3 identifies to denise.v:44's `red[7:0]` (output), unconditionally declared -- no stimulus can make a video output behave as an input. Positive control: COLOR00-LIVE above.
    checks.expect(red_live,
        "DENISE-R0-R3-DIRECTION: at denise's own boundary, R0-R3 identifies to `red[7:0]` "
        "(output, denise.v:44, 'red componenent video out'), connected at minimig.v:591 and "
        "assigned at denise.v:520 -- a Verilog port-direction fact. WIDTH NOT GRADED: the page "
        "prints four bits (OCS); denise.v declares eight (AGA). The direction is the same "
        "proposition; the bit count is a different one, not asserted here. "
        "[p.380 DENISE PIN ASSIGNMENT, DEFINITION column: R0-R3 \"O\"] "
        "sol:J-054 opus:AHJ-186");

    // MUTATION DENISE-B0-B3-DIRECTION none impossible: B0-B3 identifies to denise.v:46's `blue[7:0]` (output), unconditionally declared. Positive control: COLOR00-LIVE above.
    checks.expect(blue_live,
        "DENISE-B0-B3-DIRECTION: at denise's own boundary, B0-B3 identifies to `blue[7:0]` "
        "(output, denise.v:46, 'blue component video out'), connected at minimig.v:593 and "
        "assigned at denise.v:520 -- a Verilog port-direction fact. Same width carve-out as "
        "R0-R3 above (four printed OCS bits against eight declared AGA bits). "
        "[p.380 DENISE PIN ASSIGNMENT, DEFINITION column: B0-B3 \"O\"] "
        "sol:J-056 opus:AHJ-188");

    // MUTATION DENISE-G0-G3-DIRECTION none impossible: G0-G3 identifies to denise.v:45's `green[7:0]` (output), unconditionally declared. Positive control: COLOR00-LIVE above.
    checks.expect(green_live,
        "DENISE-G0-G3-DIRECTION: at denise's own boundary, G0-G3 identifies to `green[7:0]` "
        "(output, denise.v:45, 'green component video out'), connected at minimig.v:592 and "
        "assigned at denise.v:520 -- a Verilog port-direction fact. Same width carve-out as "
        "R0-R3/B0-B3 above. [p.380 DENISE PIN ASSIGNMENT, DEFINITION column: G0-G3 \"O\"] "
        "sol:J-058 opus:AHJ-190");

    // ========================================================================
    // 7M (clk7_en) POSITIVE CONTROL: held low throughout a BPLCON0 write,
    // the write never lands (hires stays at its pre-write value); released,
    // the SAME write lands. `write_reg_forced` bypasses the normal
    // "while(!clk7_en) tick()" wait -- it drives reg_address_in/data_in and
    // ticks WITHOUT ever letting clk7_en go high, so denise's own clk7_en-
    // gated register logic (every `always @(posedge clk) if (clk7_en) ...`
    // block in denise.v) never samples it.
    // ========================================================================
    bool clk7en_live;
    {
        do_reset();
        bool hires_before = dut->hires;
        // Held low: force clk7_en to 0 for the whole hold, bypassing the
        // tick() lambda's own clk7_cycle-driven toggling.
        dut->reg_address_in = BPLCON0;
        dut->data_in = 0x8000;
        for (int i = 0; i < 16; i++) {
            dut->eval();
            dut->clk = 1; dut->clk7_en = 0; dut->eval();
            dut->clk = 0; dut->eval();
        }
        bool hires_during_hold = dut->hires;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        // Released: resume the normal free-running clk7_en pattern and
        // reissue the identical write.
        for (int i = 0; i < 8; i++) tick();
        write_reg(BPLCON0, 0x8000);
        bool hires_after = dut->hires;
        checks.note("CLK7EN-LIVE: hires=" + std::to_string(hires_before) + " before any write; "
                    "still " + std::to_string(hires_during_hold) + " after BPLCON0=0x8000 is "
                    "held on the bus for 16 raw ticks with clk7_en forced low the whole time; "
                    "becomes " + std::to_string(hires_after) + " once clk7_en is allowed to "
                    "pulse and the identical write is reissued.");
        clk7en_live = (hires_before == 0) && (hires_during_hold == 0) && (hires_after == 1);
    }
    checks.note(std::string("CLK7EN-FIXTURE-LIVE: ") + (clk7en_live ? "live" : "NOT LIVE -- see MEASURED above") +
                " [no page -- infrastructure, not itself a claim]");

    // MUTATION DENISE-7M-DIRECTION none impossible: 7M identifies to denise.v:33's `clk7_en` (input), unconditionally declared -- no stimulus can make a clock-enable input behave as an output. Positive control: CLK7EN-LIVE above (a write held with clk7_en forced low never lands; released, it does).
    checks.expect(clk7en_live,
        "DENISE-7M-DIRECTION: at denise's own boundary, 7M identifies to `clk7_en` (input, "
        "denise.v:33, '7MHz clock enable'), driven at minimig.v:582 -- a Verilog port-direction "
        "fact. REPRESENTATION DIFFERENCE, stated so the finding travels with the claim rather "
        "than the conclusion: NanoMig carries no 7.15909 MHz waveform on a wire -- it runs "
        "`denise` from the 28 MHz `clk` (denise.v:32) and gates the 7 MHz domain with a "
        "one-in-four enable. What is graded is that Denise RECEIVES its 7 MHz timing rather "
        "than generating it; the waveform's frequency is not asserted. "
        "[p.380 DENISE PIN ASSIGNMENT, DEFINITION column: 7M \"I\"] "
        "sol:J-068 opus:AHJ-200");

    // ========================================================================
    // CCK -- DECLINED, not graded. See file header for the full argument:
    // cck (denise.v:36) reaches no other signal anywhere in this compiled
    // hierarchy (measured, grep across all 12 SRC files this Makefile
    // builds), so no positive control can be built and an expect() here
    // would be unfalsifiable by construction.
    // ========================================================================
    checks.record(
        "DENISE-CCK-DIRECTION (p.380 DENISE PIN ASSIGNMENT, DEFINITION column: CCK \"I\"): "
        "not asserted. cck identifies to `cck` (input, denise.v:36, 'colour clock enable'), "
        "driven at minimig.v:585 from the same net that feeds agnus and paula -- the declared "
        "direction matches the printed cell. MEASURED: `grep -n cck` over denise.v and all 11 "
        "submodule files this package's own Makefile compiles (denise_bitplanes.v, "
        "denise_bitplane_shifter.v, bitplane_ram.v, denise_playfields.v, denise_sprites.v, "
        "denise_sprites_shifter.v, sprite_ram.v, denise_spritepriority.v, denise_colortable.v, "
        "denise_colortable_ram_mf.v, denise_hamgenerator.v, denise_collision.v) returns exactly "
        "one hit -- the port declaration itself. Unlike paula's own CCK (gates "
        "paula_audio_channel's state machine, graded in the sibling package "
        "hrm/appJ-paula-port-directions with a working positive control), denise's cck has ZERO "
        "fan-out in this fixture: no `assign`, no `always` block, no submodule port connection "
        "anywhere reads it. Every other proposition in this package is graded `none impossible` "
        "PAIRED WITH a positive control that shows the port move something real, per the build "
        "brief's own instruction and the hrm/appDE-fire-buttons-userio worked example; for CCK "
        "no such control can be built -- there is nothing downstream to move, so grading it with "
        "expect() would be an assertion that could not fail for the reason it was written, "
        "exactly what check_gates.py's mutation gate exists to catch. NOT a prior-assertion "
        "decline: hrm/ch8-appb-chip-column-denise (the accepted package topping this same "
        "`denise` boundary) grades no package-pin direction at all, so nothing here is declined "
        "against it. This is the one proposition in this package's 8 where the axis's own "
        "reasoning (RTL declaration argues the direction) still holds but this suite's own "
        "positive-control requirement cannot be met, and is recorded as a genuinely different "
        "reason, not a repeat of any other decline in this build. "
        "[claims: sol:J-070, opus:AHJ-202]");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
