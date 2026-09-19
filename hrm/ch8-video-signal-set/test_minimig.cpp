// ch8-video-signal-set -- minimig conformance test
//
// Test purpose: whether this design's video-output boundary carries an
//   identifiable Red, Green, Blue and Sync role, matching the four-member
//   set p.260 names for the analog-RGB ("RGB Monitors") interface. Package
//   ch8-video-signal-set, fixture `minimig` (the whole minimig.v chipset
//   module -- Denise, which owns red/green/blue, is a submodule of it and
//   cannot be exercised through the CPU bus at any smaller boundary this
//   suite already has).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   p.260. Claims file ch8-video-signal-set.jsonl, 10 records (5
//   propositions after twins are grouped, per SPECIFICATION.md, committed at
//   8d29f9b before this file was written).
// Non-goals: this package does not test the 23-pin connector's existence
//   (NanoMig has none; that is why 129 of p.260's other claims are
//   not-asserted elsewhere in this slice) or the "4,096 different colors"
//   clause in the same sentence (no claim in this claims file restates it).
//   It does not test colour depth/count (that is ch8-display-colour-count, a
//   different top). It does not decide whether three sync outputs where the
//   page names one is a divergence -- the task brief that commissioned this
//   build records that as NOT a divergence, so this file does not raise it;
//   SYNC-OUTPUT-TOGGLES below grades only that a Sync-role output exists and
//   is live, sampling `_csync` (the composite, the one of the three the
//   page's singular "Sync" means), never the 3-vs-1 count.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// BOUNDARY: minimig.v ports cpu_address/cpu_r_w/_cpu_as/_cpu_uds/_cpu_lds/
//   cpudata_in/_cpu_dtack (the external 68000 bus, used to write Denise's
//   colour table and, for SYNC-OUTPUT-TOGGLES's mutation only, Agnus's
//   BEAMCON0/HSSTRT/HSSTOP registers, through the custom-chip register
//   window at $DFF000-$DFF1FE); _cpu_reset_in/rst_ext/_cpu_reset (reset,
//   needed before the bus will accept any cycle); clk/clk7_en/clk7n_en/
//   c1/c3/cck/eclk (the clock-enable family minimig.v takes as plain
//   inputs, reproduced here exactly as ch7-overlay-source's harness does
//   -- amiga_clk.v itself is excluded from this build); chipset_config
//   (held at 0 -- OCS behaviour, ecs=0 -- for every assertion except
//   SYNC-OUTPUT-TOGGLES's own mutation, which raises it only after the
//   colour assertions have already run and been graded, so it cannot
//   couple back into them); red/green/blue/_hsync/_vsync/_csync (the
//   signals under test).
//
//   Internally, traced by reading the RTL (never modified): minimig.v:
//   247-249 declares red/green/blue as denise.v's own output ports,
//   wired straight through (minimig.v:592-595, "denise DENISE1 (...
//   .red(red), .green(green), .blue(blue) ...)"); minimig.v:240-242/
//   459-461 does the same for _hsync/_vsync/_csync from agnus.v, which
//   itself gets _csync from agnus_beamcounter.v:421 (`assign _csync =
//   _hsync & _vsync | vser;`). Inside denise.v: COLORBASE = 9'h180
//   (denise.v:65) is the base of the 32-entry colour table at
//   $DFF180-$DFF1BE; denise_colortable.v's wr_en fires whenever a CPU
//   write lands with reg_address_in in that range and clk7_en is true
//   (denise_colortable.v:32); with aga=0 (this fixture never sets the
//   AGA chipset-config bit), loct=0 (denise.v:251, `loct = bplcon3[9] &
//   aga`) so wr_ws=2'b11 (denise_colortable.v:33), which writes the same
//   12-bit value into both the "hi" and "lo" halves of the addressed
//   24-bit colour-table word -- so a plain COLOR00 write of {R,G,B}
//   nibbles reads back nibble-replicated: red[7:0]={R,R}, green[7:0]=
//   {G,G}, blue[7:0]={B,B} (denise_colortable.v:56-57's packing). With
//   no bitplane DMA and no sprites configured, denise.v:491-497's
//   priority mux (`clut_data = ... plfdata ^ bplxor` when no sprite and
//   window disabled forces `clut_data = 0`) selects colour-table index 0
//   (COLOR00) for every sampled pixel, and denise.v:515-518's `t_blank =
//   ecs & ecsena & brdrblnk & (...)` is unconditionally false whenever
//   ecs=0 -- so with chipset_config held at 0, red/green/blue continuously
//   present whatever is written to COLOR00, with no display-window or
//   DMA setup required to observe it.
//
//   For SYNC-OUTPUT-TOGGLES's mutation: agnus_beamcounter.v:388-395
//   registers `_hsync` -- cleared to 0 at `hpos==hsstrt`, set to 1 at
//   `hpos==hsstop`, and held (unchanged) every other clk7_en tick; `vser`
//   (agnus_beamcounter.v:410-418, the term `_csync` ORs in) is generated
//   the same way, off `hsstrt`/`hsstop` again. `hsstrt`/`hsstop`
//   (agnus_beamcounter.v:224-225) read from CPU-writable registers only
//   when `varhsyen && varbeamen` (BEAMCON0 bits 8 and 7, :134-135) are
//   both set, and BEAMCON0 itself only accepts a write when `ecs`
//   (:123,172) is true -- gated by chipset_config[4:3] (minimig.v:483).
//   So: raise chipset_config to set ecs, write BEAMCON0=$01A0 (bits
//   8,7,5 -- varhsyen, varbeamen, and pal, the last only to avoid also
//   flipping agnus_beamcounter.v:167-175's separate `pal` register,
//   which the same BEAMCON0 write's bit 5 controls independent of
//   varbeamen), then write HSSTRT and HSSTOP (agnus_beamcounter.v:73,66,
//   $DFF1DE/$DFF1C2) to $00FF -- `{data_in[7:0],1'b0}` (:207-208) makes
//   hsstrt_reg=hsstop_reg=510, beyond htotal's ~454-tick default (never
//   itself written, so it keeps its reset value even with varbeamen=1)
//   -- and `hpos` (0..~453) can never reach either again. `_hsync` and
//   `vser` then hold their last value for good, and `_csync` freezes
//   with them.
//
// ORDER OF WORK: SPECIFICATION.md (this package's 5 propositions,
// PROP-COUNT/PROP-RED/PROP-GREEN/PROP-BLUE/PROP-SYNC) was written and
// committed -- commit 8d29f9b -- from the claims file's quote/restatement
// fields, before minimig.v, denise.v, denise_colortable.v or
// agnus_beamcounter.v were opened for this build. SPECIFICATION.md
// discloses one exception to that order: the claims file's own
// classify.evidence field, and the task brief that commissioned this
// build, already named minimig.v's port lines before the specification
// was drafted -- an order-of-work compromise this build did not create
// but is not pretending away either.
//
// FIXTURE SIZE, MEASURED: this build Verilates the same 53 modules as
// ch7-overlay-source (11.1MB source) since it uses the identical
// Makefile/source-list pattern (SRC_DIR=nanomig/src/minimig-aga, same
// EXCLUDE_SOURCES). The `_cpu_reset` release this harness needs is
// reproduced from ch7-overlay-source's own independent measurement
// (~2.27M clk28 ticks); this file's own run prints its own measured tick
// count at CONTROL-RESET-RELEASES below, not a hardcoded number.
//
// CLAIM COVERAGE: all 10 records of ch8-video-signal-set.jsonl are
// accounted for below.
//   opus:8-display.c9 + sol:8-display.c8 (twins; p.260, "One of the
//     analog RGB outputs is Red, abbreviated R.") -- graded as
//     RED-OUTPUT-CARRIES-COLOR00.
//   opus:8-display.c10 + sol:8-display.c9 (twins; Green) -- graded as
//     GREEN-OUTPUT-CARRIES-COLOR00.
//   opus:8-display.c11 + sol:8-display.c10 (twins; Blue) -- graded as
//     BLUE-OUTPUT-CARRIES-COLOR00.
//   opus:8-display.c12 + sol:8-display.c11 (twins; p.260, "One of the
//     analog RGB outputs is Sync, abbreviated S.") -- graded as
//     SYNC-OUTPUT-TOGGLES. First declined with a record() (the vacuity
//     objection: no stimulus this harness controlled could suppress
//     _hsync/_csync, so it looked unfalsifiable); coordinator review
//     (O65 discussion) pointed out that vacuity has its own vocabulary
//     in this suite -- `none impossible`, per hrm/ch8-keycode-table's
//     CUTOUT-30 -- and that a record() instead makes the claim invisible
//     to the ledger. Re-examined: a stimulus mutation DOES exist (see the
//     BOUNDARY section above), so this is graded `stimulus red`, not
//     `none impossible`.
//   opus:8-display.c8 + sol:8-display.c7 (twins; p.260, "Provides four
//     outputs; Red (R), Green (G), Blue (B), and Sync (S).") -- NOT
//     graded with a fifth expect(). The count is a structural fact of
//     minimig.v's port declaration that no runtime stimulus can add to or
//     subtract from; grading it as a conjunction of the other four would
//     mean one label that can fail for four different reasons, which
//     BUILD-BRIEF.md's rule 2 forbids. Recorded as corroborated by the
//     four graded assertions (RED/GREEN/BLUE-OUTPUT-CARRIES-COLOR00,
//     SYNC-OUTPUT-TOGGLES) instead -- naming them, not only explaining
//     why this claim itself carries no expect().
//
// CONTROL CASES: CONTROL-RESET-RELEASES (no claim id; the harness's own
// reset chain must genuinely release within budget, exactly ch7-overlay-
// source's rationale, reused because this harness drives the same CPU
// bus). CONTROL-VIDEO-BASELINE (no claim id) is this package's "fixture
// proves it can deliver a known value before anything reads one": red,
// green and blue are checked ==0 at cold start, before any COLOR00 write,
// so the three colour assertions below are readings against a fixture
// already shown to report a specific value correctly, not an unverified
// one. SYNC-OUTPUT-TOGGLES carries its own analogous basis inline: the
// baseline toggle count it reads (printed by the preceding note()) is
// itself the demonstration that the fixture can tell a live signal from
// a stuck one, on the same measurement basis its mutation then flips.
//
// RESULTS: baseline (no MUT flag) is 6 passed, 0 failed. No claim in this
// package is expected red on this RTL -- unlike ch7-overlay-source, there
// is no known S4 finding-candidate against any of these ten claims (the
// four/five naming defect the finding-candidates file records for p.260,
// S4-C11, is the DIGITAL-RGB bullet's sentence, not the ANALOG-RGB
// sentence this package's ten claims all cite).
//
// PER-ASSERTION MUTATION RECORD. Six variants, each its own full
// build+run (`make MUT=<switch>`), built, run, confirmed and reverted --
// not re-run for this header update. Format (tools/check_gates.py):
// // MUTATION <assertion-label> <oracle|stimulus|none> <red|green|
// impossible> <what changed>. Every mutation below is `stimulus` -- what
// the harness asks the module to do -- never `oracle`.
//
// MUTATION CONTROL-RESET-RELEASES stimulus red MUT_RESET holds rst_ext (master reset) asserted forever, so minimig_syscontrol.v's rst_cnt never advances and _cpu_reset never rises within the 3,000,000-tick budget; baseline PASS (measured tick, printed at run time) goes red.
// MUTATION CONTROL-VIDEO-BASELINE stimulus red MUT_BASELINE issues an extra COLOR00 write of $FFF immediately after reset releases, before the baseline check -- red/green/blue read $FF/$FF/$FF instead of the expected $00/$00/$00; baseline PASS goes red.
// MUTATION RED-OUTPUT-CARRIES-COLOR00 stimulus red MUT_RED writes COLOR00=$000 instead of $F00 (the red nibble is never set); baseline PASS (red==$FF) goes red (red stays $00).
// MUTATION GREEN-OUTPUT-CARRIES-COLOR00 stimulus red MUT_GREEN writes COLOR00=$000 instead of $0F0; baseline PASS (green==$FF) goes red (green stays $00).
// MUTATION BLUE-OUTPUT-CARRIES-COLOR00 stimulus red MUT_BLUE writes COLOR00=$000 instead of $00F; baseline PASS (blue==$FF) goes red (blue stays $00).
// MUTATION SYNC-OUTPUT-TOGGLES stimulus red MUT_SYNC raises chipset_config to set ecs, writes BEAMCON0=$01A0 (varhsyen+varbeamen+pal) then HSSTRT=HSSTOP=$00FF (hsstrt_reg=hsstop_reg=510, beyond htotal's ~454) instead of leaving BEAMCON0/HSSTRT/HSSTOP unwritten; baseline PASS (_csync toggles) goes red (_csync frozen constant for the whole sampled window).

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include "Vminimig.h"
#include "verilated.h"

#ifndef MUT_RESET
#define MUT_RESET 0
#endif
#ifndef MUT_BASELINE
#define MUT_BASELINE 0
#endif
#ifndef MUT_RED
#define MUT_RED 0
#endif
#ifndef MUT_GREEN
#define MUT_GREEN 0
#endif
#ifndef MUT_BLUE
#define MUT_BLUE 0
#endif
#ifndef MUT_SYNC
#define MUT_SYNC 0
#endif

namespace {

// Reproduces amiga_clk.v's exact register update (read for this purpose
// only; amiga_clk.v itself is excluded from this build -- it, and the
// actual 68000 core, live one level above minimig.v in the real SoC and
// are not part of this module's hierarchy). Identical to
// ch7-overlay-source's ClkGen; copied rather than shared because this
// suite's packages do not share code between builds.
struct ClkGen {
    uint8_t clk7_cnt = 2;   // amiga_clk.v reset value 2'b10
    bool clk7_en = true, clk7n_en = true, cck = true;
    uint16_t shifter = 1;   // one-hot, amiga_clk.v reset value
    bool c1 = false, c3 = false;
    void step() {
        uint8_t old_cnt = clk7_cnt;
        bool old_c3 = c3;
        bool old_cck = cck;
        uint16_t old_shifter = shifter;
        bool old_clk7 = (old_cnt >> 1) & 1;
        clk7_cnt = (old_cnt + 1) & 0x3;
        clk7_en = (old_cnt == 0);
        clk7n_en = (old_cnt == 2);
        if (old_cnt == 1) {
            cck = !old_cck;
            uint16_t rotated = (uint16_t)(((old_shifter << 1) | (old_shifter >> 9)) & 0x3FF);
            shifter = rotated ? rotated : 1;
        } else {
            cck = old_cck;
            shifter = old_shifter;
        }
        c3 = old_clk7;
        c1 = !old_c3;
    }
};

// $DFF180 -- COLOR00, the background/border colour-table entry. With no
// bitplane DMA and no sprites configured (this fixture sets up neither),
// denise.v's priority mux selects colour-table index 0 for every sampled
// pixel (denise.v:487-497), so COLOR00 is what red/green/blue carry.
constexpr uint32_t COLOR00_ADDR = 0xDFF180;

// Agnus's variable-beam registers, used only by SYNC-OUTPUT-TOGGLES's
// mutation (see the file header's BOUNDARY section for the full trace).
// $DFF1DC BEAMCON0 (agnus_beamcounter.v:74), $DFF1DE HSSTRT (:73),
// $DFF1C2 HSSTOP (:66).
constexpr uint32_t BEAMCON0_ADDR = 0xDFF1DC;
constexpr uint32_t HSSTRT_ADDR   = 0xDFF1DE;
constexpr uint32_t HSSTOP_ADDR   = 0xDFF1C2;

class Harness {
public:
    Vminimig* dut;
    ClkGen cg;
    long long tickcount = 0;
    bool reset_released = false;
    long long reset_release_tick = -1;

    Harness() : dut(new Vminimig) {
        dut->clk = 0;
        dut->clk7_en = cg.clk7_en; dut->clk7n_en = cg.clk7n_en;
        dut->c1 = cg.c1; dut->c3 = cg.c3; dut->cck = cg.cck; dut->eclk = cg.shifter;
        dut->rst_ext = 0;
        dut->_cpu_reset_in = 0;
        dut->cpu_address = 0; dut->cpu_r_w = 1;
        dut->_cpu_as = 1; dut->_cpu_uds = 1; dut->_cpu_lds = 1;
        dut->cpudata_in = 0;
        dut->memory_config = 0; dut->chipset_config = 0; dut->floppy_config = 0;
        dut->nmi_addr = 0; dut->chip48 = 0; dut->ramdata_in = 0;
        dut->rxd = 1; dut->cts = 1; dut->dsr = 1; dut->cd = 1; dut->ri = 1;
        dut->_joy1 = 0xFFFF; dut->_joy2 = 0xFFFF; dut->_joy3 = 0xFFFF; dut->_joy4 = 0xFFFF;
        dut->joya1 = 0; dut->joya2 = 0; dut->mouse_btn = 0;
        dut->kms_level = 0; dut->kbd_mouse_type = 0; dut->kbd_mouse_data = 0;
        dut->sdc_img_mounted = 0; dut->sdc_img_size = 0; dut->sdc_busy = 0; dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0; dut->sdc_byte_addr = 0; dut->sdc_byte_in_data = 0;
        dut->cpucfg = 0;

        // Harness hygiene (see ch7-overlay-source, same reasoning): force
        // one clean _as low->high edge before anything relies on
        // _cpu_dtack, so bus_cycle()'s wait-for-dtack loops measure the
        // real protocol, not Verilator's zero-init of
        // minimig_m68k_bridge.v's un-reset `_ta_n` latch.
        dut->_cpu_as = 0; tick(); tick();
        dut->_cpu_as = 1; tick(); tick();

#if MUT_RESET
        // MUTATION for CONTROL-RESET-RELEASES: hold master reset asserted
        // forever. minimig_syscontrol.v never lets rst_cnt advance, so
        // sys_reset (and therefore _cpu_reset) must never release within
        // the budget below.
        dut->rst_ext = 1;
#endif
        // _cpu_reset_in released early so Agnus's free-running beam
        // counters (agnus_beamcounter.v's hpos/vpos -- not gated by
        // `reset` at all) can produce the `sof` pulses
        // minimig_syscontrol.v needs to count down. Holding it low first
        // models power-on/external reset.
        for (int i = 0; i < 8; ++i) tick();
        dut->_cpu_reset_in = 1;

        const long long BUDGET = 3000000; // ch7-overlay-source measured 2,271,634; budget with headroom
        for (long long i = 0; i < BUDGET && !reset_released; ++i) {
            tick();
            if (dut->_cpu_reset == 1) {
                reset_released = true;
                reset_release_tick = tickcount;
            }
        }
        for (int i = 0; i < 8; ++i) tick();
    }
    ~Harness() { delete dut; }

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        cg.step();
        dut->clk7_en = cg.clk7_en; dut->clk7n_en = cg.clk7n_en;
        dut->c1 = cg.c1; dut->c3 = cg.c3; dut->cck = cg.cck; dut->eclk = cg.shifter;
        ++tickcount;
    }

    // Drives one full-word 68000-style write cycle to a custom-chip
    // register. Custom registers ($DFF000-$DFF1FE) are word-only, unlike
    // CIA space (ch7-overlay-source's byte-lane bus_cycle) -- both _uds
    // and _lds go low together, carrying `data` on the bus for the whole
    // cycle. Same dtack-wait/settle-tail shape as ch7-overlay-source's
    // bus_cycle, generalised from a byte write to a word write.
    void write_word(uint32_t byte_addr, uint16_t data) {
        dut->cpu_address = (byte_addr >> 1) & 0x7FFFFFu;
        dut->cpu_r_w = 0; // write
        dut->cpudata_in = data;
        dut->_cpu_uds = 0; dut->_cpu_lds = 0;
        dut->_cpu_as = 0;
        bool dtack_seen = false;
        for (int i = 0; i < 4000 && !dtack_seen; ++i) {
            tick();
            if (dut->_cpu_dtack == 0) dtack_seen = true;
        }
        for (int i = 0; i < 16; ++i) tick();
        dut->_cpu_as = 1; dut->_cpu_uds = 1; dut->_cpu_lds = 1; dut->cpu_r_w = 1;
        for (int i = 0; i < 16; ++i) tick(); // let the colour-table RAM's registered read settle
    }
};

// The fixed assertion harness (build brief): expect() grades, note()
// measures without grading, record() declines with a reason.
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
    // CONTROL-RESET-RELEASES (no claim id; harness precondition, same
    // rationale as ch7-overlay-source: the CPU bus this harness uses for
    // every COLOR00 write depends on _cpu_reset having released, or every
    // write below would be talking to a permanently-reset chip).
    // =========================================================================
    {
        char tickbuf[24];
        std::snprintf(tickbuf, sizeof tickbuf, "%lld", h.reset_release_tick);
        checks.expect(
            h.reset_released,
            std::string("CONTROL-RESET-RELEASES: _cpu_reset rose within a 3,000,000-tick "
                        "budget after rst_ext/_cpu_reset_in were released (measured "
                        "tick=") + tickbuf + ", or -1 if the budget was exhausted). "
                        "No manual page (harness precondition).");
    }

    if (!h.reset_released) {
        std::cout << "\nHALT-CANDIDATE: reset never released within budget; no "
                     "COLOR00 write below would be measuring anything but a "
                     "permanently-reset chip, so none are run.\n";
        std::cout << checks.passed << " passed, " << checks.failed << " failed\n";
        return checks.failed == 0 ? 0 : 1;
    }

#if MUT_BASELINE
    // MUTATION for CONTROL-VIDEO-BASELINE: write a value into COLOR00
    // before the baseline is checked, so the "known value" this fixture
    // is about to prove it can deliver is $FFF (white) instead of the
    // reset default -- red/green/blue read $FF/$FF/$FF, not $00/$00/$00.
    h.write_word(COLOR00_ADDR, 0xFFF);
#endif

    // =========================================================================
    // CONTROL-VIDEO-BASELINE (no claim id). BUILD-BRIEF.md: "The fixture
    // proves it can deliver a known value before anything reads one."
    // Checked here, before any deliberate COLOR00 write: red, green and
    // blue all read 0 at this point (Verilator zero-inits the colour-
    // table RAM; nothing in this harness has written to it yet), so the
    // three graded assertions below are readings against a fixture
    // already demonstrated to report a specific, known value correctly.
    // =========================================================================
    checks.expect(
        h.dut->red == 0 && h.dut->green == 0 && h.dut->blue == 0,
        "CONTROL-VIDEO-BASELINE: red==0, green==0 and blue==0 before any "
        "COLOR00 write -- the fixture's known-value baseline. No manual "
        "page (harness precondition).");

    // =========================================================================
    // RED-OUTPUT-CARRIES-COLOR00 (p.260) -- opus:8-display.c9, sol:8-display.c8.
    // "One of the analog RGB outputs is Red, abbreviated R." Writing
    // COLOR00 with only the red nibble set drives a known, distinct value
    // onto minimig's `red` output.
    // =========================================================================
    {
#if MUT_RED
        // MUTATION: write $000 instead of $F00 -- the red nibble is never
        // set. Expected value (red==$FF after) is left alone.
        h.write_word(COLOR00_ADDR, 0x000);
#else
        h.write_word(COLOR00_ADDR, 0xF00);
#endif
        checks.expect(h.dut->red == 0xFF,
            "RED-OUTPUT-CARRIES-COLOR00 (p.260): writing COLOR00 ($DFF180) "
            "with its red nibble set (bits [11:8]) drives minimig's `red` "
            "output to $FF (nibble-replicated) -- \"One of the analog RGB "
            "outputs is Red, abbreviated R.\" (p.260). "
            "opus:8-display.c9, sol:8-display.c8. "
            "MUTATION: write $000 instead of $F00 -- red must stay $00.");
    }

    // =========================================================================
    // GREEN-OUTPUT-CARRIES-COLOR00 (p.260) -- opus:8-display.c10, sol:8-display.c9.
    // Same shape as RED, green nibble (bits [7:4]).
    // =========================================================================
    {
#if MUT_GREEN
        h.write_word(COLOR00_ADDR, 0x000);
#else
        h.write_word(COLOR00_ADDR, 0x0F0);
#endif
        checks.expect(h.dut->green == 0xFF,
            "GREEN-OUTPUT-CARRIES-COLOR00 (p.260): writing COLOR00 with its "
            "green nibble set (bits [7:4]) drives minimig's `green` output "
            "to $FF (nibble-replicated) -- \"One of the analog RGB outputs "
            "is Green, abbreviated G.\" (p.260). "
            "opus:8-display.c10, sol:8-display.c9. "
            "MUTATION: write $000 instead of $0F0 -- green must stay $00.");
    }

    // =========================================================================
    // BLUE-OUTPUT-CARRIES-COLOR00 (p.260) -- opus:8-display.c11, sol:8-display.c10.
    // Same shape as RED, blue nibble (bits [3:0]).
    // =========================================================================
    {
#if MUT_BLUE
        h.write_word(COLOR00_ADDR, 0x000);
#else
        h.write_word(COLOR00_ADDR, 0x00F);
#endif
        checks.expect(h.dut->blue == 0xFF,
            "BLUE-OUTPUT-CARRIES-COLOR00 (p.260): writing COLOR00 with its "
            "blue nibble set (bits [3:0]) drives minimig's `blue` output "
            "to $FF (nibble-replicated) -- \"One of the analog RGB outputs "
            "is Blue, abbreviated B.\" (p.260). "
            "opus:8-display.c11, sol:8-display.c10. "
            "MUTATION: write $000 instead of $00F -- blue must stay $00.");
    }

    // =========================================================================
    // SYNC-OUTPUT-TOGGLES (p.260) -- opus:8-display.c12, sol:8-display.c11.
    // "One of the analog RGB outputs is Sync, abbreviated S." Graded: the
    // composite sync output (_csync -- the one of the three sync ports
    // the page's singular "Sync" means, per SPECIFICATION.md and the file
    // header) reads low for part of the sampled window and high for the
    // rest, i.e. it is live, not a design that tied Sync to a constant.
    // Sampled over one horizontal line's worth of ticks (a PAL line is
    // 227 CCKs; clk7_en/cck run at 1/4 and 1/8 of clk28, so ~2000 clk28
    // ticks comfortably covers one full line), starting fresh after the
    // COLOR00 writes above.
    //
    // First declined here with a record() -- the vacuity objection: no
    // stimulus this harness controlled could suppress _hsync/_csync at
    // chipset_config=0, so the check looked unfalsifiable. Coordinator
    // review (recorded as O65 against BUILD-BRIEF.md, not against this
    // build) pointed out this suite's actual vocabulary for that case is
    // a `none impossible` mutation record (hrm/ch8-keycode-table's
    // CUTOUT-30), not a decline -- and that a genuine stimulus mutation
    // does exist here if chipset_config is allowed to change for this
    // block alone, after the colour assertions have already run and been
    // graded, so it cannot couple back into them (see MUT_SYNC below and
    // the file header's BOUNDARY section for the exact mechanism).
    // Re-examined and reversed: graded `stimulus red`, carrying both uids.
    // =========================================================================
    {
#if MUT_SYNC
        // MUTATION: push HSSTRT/HSSTOP beyond any reachable beam position
        // so agnus_beamcounter.v:390-393's `_hsync <= 0` (at hpos==hsstrt)
        // / `_hsync <= 1` (at hpos==hsstop) never fires again -- _hsync
        // (and `vser`, hence `_csync`) freezes at whatever it last held
        // instead of continuing to toggle. This changes what the harness
        // drives (chipset_config, then two register writes), never what
        // SYNC-OUTPUT-TOGGLES expects. Runs only in this block, after
        // RED/GREEN/BLUE-OUTPUT-CARRIES-COLOR00 have already been graded
        // above, so it cannot couple back into their ecs=0 measurement
        // basis.
        h.dut->chipset_config = 0x08; // chipset_config[4:3]!=0 -> ecs=1; aga (bit 4) stays 0
        h.write_word(BEAMCON0_ADDR, 0x01A0); // varhsyen(8)=1, varbeamen(7)=1, pal(5)=1 (unchanged)
        h.write_word(HSSTRT_ADDR, 0x00FF);   // hsstrt_reg = {0xFF,1'b0} = 510, beyond htotal's ~454
        h.write_word(HSSTOP_ADDR, 0x00FF);   // hsstop_reg likewise unreachable
#endif
        int hsync_low_ticks = 0, csync_low_ticks = 0;
        bool csync_matches_hsync_and_vsync = true;
        const int SAMPLE_TICKS = 2000;
        for (int i = 0; i < SAMPLE_TICKS; ++i) {
            h.tick();
            bool hsync = h.dut->_hsync != 0;
            bool vsync = h.dut->_vsync != 0;
            bool csync = h.dut->_csync != 0;
            if (!hsync) ++hsync_low_ticks;
            if (!csync) ++csync_low_ticks;
            // agnus_beamcounter.v:421 `assign _csync = _hsync & _vsync | vser;`
            // -- outside a vsync serration interval (vser==0, the expected
            // case for most of a frame and certainly reachable within one
            // line), _csync must equal _hsync & _vsync exactly.
            if (vsync && (csync != hsync)) csync_matches_hsync_and_vsync = false;
        }
        char buf[320];
        std::snprintf(buf, sizeof buf,
            "SYNC-ROLE-OBSERVED (p.260): over %d clk28 ticks (>1 PAL line), "
            "_hsync read low %d ticks and _csync read low %d ticks; while "
            "_vsync was high, _csync tracked (_hsync & _vsync) on every "
            "sampled tick: %s.",
            SAMPLE_TICKS, hsync_low_ticks, csync_low_ticks,
            csync_matches_hsync_and_vsync ? "yes" : "NO -- see the graded assertion below");
        checks.note(buf);

        bool csync_toggles = (csync_low_ticks > 0) && (csync_low_ticks < SAMPLE_TICKS);
        checks.expect(csync_toggles,
            "SYNC-OUTPUT-TOGGLES (p.260): minimig's composite sync output "
            "(_csync) reads low for part of the sampled window and high "
            "for the rest, rather than a constant -- \"One of the analog "
            "RGB outputs is Sync, abbreviated S.\" (p.260). "
            "opus:8-display.c12, sol:8-display.c11. "
            "MUTATION: with chipset_config's ECS bit set and BEAMCON0's "
            "varhsyen/varbeamen bits written, HSSTRT/HSSTOP are pushed to "
            "$1FE (510), beyond any reachable beam position -- _csync must "
            "then read constant for the whole sampled window instead.");
    }

    // =========================================================================
    // COUNT ("four outputs") -- opus:8-display.c8, sol:8-display.c7
    // (p.260, "Provides four outputs; Red (R), Green (G), Blue (B), and "
    // "Sync (S)."). NOT graded with a fifth, independent expect(). See
    // the file header's CLAIM COVERAGE section for the full reasoning:
    // the four-member count is a structural fact of minimig.v's port
    // declaration, not something any stimulus this harness applies can
    // add to or remove, and grading it as one label spanning four
    // different underlying reasons is exactly what BUILD-BRIEF.md's rule
    // 2 forbids. Unlike SYNC-OUTPUT-TOGGLES above, this is a genuine
    // conjunction over four independently-meaningful signals with no
    // single stimulus that isolates "the count" from "each member" --
    // kept as a decline, per the coordinator's review, strengthened to
    // name where the content actually lives rather than only explain why
    // it is not here.
    // =========================================================================
    checks.record(
        "opus:8-display.c8, sol:8-display.c7 (p.260, \"Provides four "
        "outputs; Red (R), Green (G), Blue (B), and Sync (S).\"): not "
        "graded with a fifth expect() here. The four-member count is "
        "graded by, and only by, the four assertions above that each carry "
        "one member's own content: RED-OUTPUT-CARRIES-COLOR00, "
        "GREEN-OUTPUT-CARRIES-COLOR00, BLUE-OUTPUT-CARRIES-COLOR00 and "
        "SYNC-OUTPUT-TOGGLES. Port declaration is a structural, "
        "compile-time fact no runtime stimulus in this harness can "
        "change, so a standalone COUNT expect() could only ever be a "
        "conjunction of those same four reasons, which would fail for "
        "whichever of the four happened to be true that run -- exactly "
        "what BUILD-BRIEF.md's rule 2 forbids. See SPECIFICATION.md's "
        "PROP-COUNT section and this file's header for the full "
        "reasoning.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
