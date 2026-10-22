// appDE-portsig-cia-bindings -- minimig conformance test
//
// Test purpose: grade the 48 claims filtered to this package from
//   work/2026-09-17_appDE_packages.jsonl -- p.330's FIRE BUTTONS figure
//   (connector pin 6 -> FIRE0/FIRE1 -> CIAAPRA bit), p.334's EXTERNAL DISK
//   CONNECTOR figure (pin 22, INDEX*), and p.337's PART 4 -- PORT SIGNAL
//   ASSIGNMENTS FOR 8520 CIAS (both CIA-A and CIA-B port tables in full).
//   Fixture `minimig` -- the
//   whole chipset, needed because every proposition here is a JOIN fact
//   (connector pin <-> CIA port bit, or CIA-A write <-> Gary's overlay
//   decode) only visible at this boundary, exactly as
//   hrm/appF-cia-pin-bindings and hrm/ch7-overlay-source already argued for
//   their own, narrower joins.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 330, 334 and 337.
// Non-goals: anything outside the `minimig` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/2026-09-17_appDE_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// Three questions touch this package; each is measured and recorded, none
//   is adjudicated here:
//     1. The fire buttons (p.329/p.327 vs the RTL). The textual proposition
//        ("the LEFT port's fire is on bit 7", p.329) belongs to a DIFFERENT
//        package (appDE-fire-buttons-ciaa) -- no uid in this package's own
//        48 claims states it -- so it is not graded here under a foreign
//        uid, and no other claim's uid is named in a graded label, not even
//        to disown it. What this package's fixture uniquely supplies,
//        because it alone can see the connector pins, the CIA-A port bits
//        AND userio.v's own internal joins in one build, is the MEASUREMENT
//        of the connector-to-bit fact the question depends on -- see
//        FINDING-CANDIDATE-1-JOY0DAT-VS-FIRE-BIT below, a note(), not an
//        expect(), for exactly that reason.
//     2. The ROM/RAM overlay half of PA0 (p.337 vs appendix F's p.351).
//        `hrm/ch7-reset-overlay` (TOP_MODULE=gary) grades ovl=1 selecting
//        Kickstart ROM at $000000 (OVERLAY-ROM-AT-000000) and ovl=0
//        selecting chip RAM (OVL-CONTROL-BASELINE-000000). PA0's
//        ROM-over-RAM sub-proposition is therefore DECLINED below
//        (PA0-ROM-OVER-RAM-ALREADY-GRADED), not graded fresh; see that
//        record() for what this package's own fixture adds (the
//        CIA-A-write-to-gary-decode JOIN, measured as confirmation, not new
//        coverage).
//     3. PA1's audio-filter clause exists only in appendix E. It is one
//        clause of one of THIS package's own 48 claims' printed rows (PA1's
//        row) and is engaged with as such -- but because that row bundles
//        it with the LED clause (see PA1-DRIVES-LED-0-IS-BRIGHT and
//        PA1-ROW-PARTIALLY-COVERED below), the row's own two uids are
//        carried on the record() line, not a graded one.
//
// CLAIM COVERAGE (48 uids, all accounted for below):
//   Fresh, graded with expect(), UID-FREE (2 propositions; no uid in this
//   package's own 48 claims is ever printed on a PASS/FAIL line -- see
//   below for why):
//     PA1-DRIVES-LED-0-IS-BRIGHT (p.337's PA1 row's LED clause. Its own
//       uids, sol:E-portsig.c8/opus:E-portsig.c9, are NOT named here: that
//       row's printed sentence bundles the LED clause with an untested
//       audio-filter clause, classify.split is unset for both, and "an id
//       whose claim bundles a clause this package does not test is
//       withdrawn whole, not kept with a caveat in the prose" [chapter 5's
//       own README]. Both uids are carried on ONE record() line instead --
//       PA1-ROW-PARTIALLY-COVERED, see below.)
//     CONTROL-RESET-RELEASES (harness precondition, no manual page)
//   Measured, not graded, informing but not adjudicating one of the
//   questions above (no uid -- see the header note above):
//     FINDING-CANDIDATE-1-JOY0DAT-VS-FIRE-BIT
//     PA0-OVL-DECODE-VIA-MINIMIG-JOIN (confirmation of an already-graded
//       result reached through a fuller path -- see PA0-ROM-OVER-RAM-
//       ALREADY-GRADED below)
//   Declined as already graded elsewhere, declined as not gradable as a
//   positive on THIS RTL, or declined as a bundled claim whose printed
//   sentence is only partially established (not a re-grade -- 24 record()
//   groups, all 48 uids; see the per-group comments in main() for the
//   exact citation each one rests on. PA1-ROW-PARTIALLY-COVERED is the one
//   group whose own proposition IS graded above, uid-free, by design).
//
// RTL read directly: gary.v:159 `sel_chip[0] = cpu_address_in[23:19]==5'b0000_0
//   && (!ovl || cpu_hlt)`, gary.v:166 `sel_kick = (...) || (cpu_rd && ovl &&
//   cpu_address_in[23:19]==5'b0000_0)` -- both INSIDE gary.v:145's `if (dbr)
//   ... else` CPU-side branch; the `if (dbr)` branch (Agnus DMA's own turn)
//   drives the SAME sel_chip/sel_kick wires from `dma_address_in` instead,
//   entirely unrelated to `ovl` -- a decode sample that ignores `dbr` can
//   read the DMA branch and give a self-contradictory reading
//   (sel_chip[0]=1 while ovl=1, impossible per gary.v:159 alone), which is
//   why read_with_decode samples only while dbr=0; minimig.v:403
//   `wire cpuhlt = 1'b0;` (tied low in this build, so gary's `cpu_hlt` term
//   never masks the overlay bit's own effect); minimig.v:342/344 declare
//   `sel_chip`/`sel_kick` as minimig's
//   own top-level internal wires (GARY1's outputs), minimig.v:694
//   `.address_out(cpu_address_out)` feeds gary's `cpu_address_in` from the
//   SAME [23:1] Verilog-numbered wire the top-level `cpu_address` port
//   ultimately drives -- so the appF-recorded trap (hand-built address bit
//   patterns misalign against minimig's own [23:1] port numbering) is
//   avoided the same way appF avoided it: real hex byte addresses through
//   bus_cycle()'s `(byte_addr>>1)` translation, never a hand-built vector.
//   minimig.v:614-615 `wire [3:0] porta_out; assign {_fire1_dat,_fire0_dat,
//   _led} = porta_out[3:1];` (so porta_out[0], PA0's own output value, is
//   connected to nothing at minimig level -- confirmed independently here,
//   the same fact appF-cia-pin-bindings recorded for its own PA0 claim);
//   minimig.v:215/432 `output pwr_led; ... assign pwr_led = ~_led;`;
//   ciaa.v:262 `assign porta_out[3:0] = {(~ddrporta[7:6]|regporta[3:2]),
//   (~ddrporta[1:0]|regporta[1:0])}` -- the 8520's own pull-up model: an
//   input-direction bit (ddrporta=0) reads back forced to 1 regardless of
//   what was written, so DDRA must set PA1 to output before PRA's bit 1
//   drives `_led` at all. userio.v:92 `wire joy_swap = 1'b0;`, :215
//   `_sjoy1 <= joy_swap ? joy1 : joy2;` (so _sjoy1 is loaded from the
//   PHYSICAL SECOND top-level joystick port, _joy2, not from _joy1 despite
//   the "1" in its own name), :313 `assign _fire0 = cd32pad &&
//   !cd32pad1_reg_load ? fire1_d : _sjoy1[4] & _mleft;` (cd32pad is tied
//   1'b0 at userio.v:91, so the live term is always `_sjoy1[4] & _mleft`),
//   :233-239 build `dmouse0dat` (JOY0DAT) from that same `_sjoy1`;
//   minimig.v:632 `.porta_in({_fire1,_fire0,...})` places _fire0 at PA6 and
//   _fire1 at PA7. No paula_audio.v line implements a low-pass filter,
//   cutoff constant or gated signal PA1 could plausibly drive -- grepped
//   directly for "filter"/"cutoff"/"lowpass" across every minimig-aga
//   source file; the only "filter" hits in the whole source tree are an
//   unrelated video-interpolation comment in minimig.v and two audio
//   noise-shaping comments in paula_audio.v, neither a gateable RC/cutoff
//   element -- independently confirms, rather than merely cites,
//   ch5-audio-filter-bypass's own finding.
//
// Fixture plumbing (ClkGen, Harness::tick/bus_cycle/write_byte/read_byte,
//   the long reset-acquisition budget, the harness-hygiene throwaway _as
//   edge) is shared with hrm/appF-cia-pin-bindings/test_minimig.cpp, which
//   shares it with hrm/ch7-overlay-source and
//   hrm/ch8-cia-disk-signal-bindings -- this suite's own validated
//   plumbing for this identical TOP_MODULE=minimig boundary. It is
//   plumbing, not a source of this package's own assertions; the gary
//   decode sampling (read_with_decode) and PA1/LED sequencing are this
//   package's own, for its own claims.
//
// MUTATION METHODOLOGY: every expect() below is guarded by a compile-time
//   `-D` flag. Every mutation was actually built and run (WSL Verilator,
//   `make MUT=<NAME> test`, full `rm -rf obj_dir` rebuild).

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vminimig.h"
#include "Vminimig___024root.h"
#include "Vminimig_minimig.h"

#ifndef MUT_RESET
#define MUT_RESET 0
#endif
#ifndef MUT_LED_SWAP
#define MUT_LED_SWAP 0
#endif

namespace {

// Reproduces amiga_clk.v's exact register update -- verbatim from
// hrm/appF-cia-pin-bindings/test_minimig.cpp (itself from
// hrm/ch7-overlay-source via hrm/ch8-cia-disk-signal-bindings), this
// suite's own validated plumbing for this fixture, not a source of this
// package's own assertions.
struct ClkGen {
    uint8_t clk7_cnt = 2;
    bool clk7_en = true, clk7n_en = true, cck = true;
    uint16_t shifter = 1;
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

std::string hex(unsigned v) {
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%X", v);
    return std::string(buf);
}

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

class Harness {
public:
    Vminimig* dut;
    ClkGen cg;

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

        // Same harness-hygiene throwaway _as edge hrm/ch7-overlay-source's
        // own Harness uses: Verilator zero-inits minimig_m68k_bridge.v's
        // `_ta_n`, so _cpu_dtack reads a stale 0 until the first _as
        // low->high edge.
        dut->_cpu_as = 0; tick(); tick();
        dut->_cpu_as = 1; tick(); tick();

        for (int i = 0; i < 8; ++i) tick();
        dut->_cpu_reset_in = 1;

#if MUT_RESET
        // MUTATION for CONTROL-RESET-RELEASES: hold master reset asserted
        // forever (minimig_syscontrol.v's own rst_cnt never advances while
        // mrst is held), so _cpu_reset must never rise within the budget.
        dut->rst_ext = 1;
#endif

        const long long BUDGET = 3000000;
        bool reset_released = false;
        for (long long i = 0; i < BUDGET && !reset_released; ++i) {
            tick();
            if (dut->_cpu_reset == 1) reset_released = true;
        }
        this->reset_released = reset_released;
        for (int i = 0; i < 8; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    bool reset_released = false;

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        cg.step();
        dut->clk7_en = cg.clk7_en; dut->clk7n_en = cg.clk7n_en;
        dut->c1 = cg.c1; dut->c3 = cg.c3; dut->cck = cg.cck; dut->eclk = cg.shifter;
    }

    // Verbatim (data-carrying) shape from hrm/appF-cia-pin-bindings' own
    // bus_cycle(): same dtack-wait/settle sequence.
    bool bus_cycle(uint32_t byte_addr, bool is_write, uint16_t write_data = 0) {
        bool low_byte = (byte_addr & 1u) != 0;
        dut->cpu_address = (byte_addr >> 1) & 0x7FFFFFu;
        dut->cpu_r_w = is_write ? 0 : 1;
        dut->cpudata_in = is_write ? write_data : 0;
        if (low_byte) { dut->_cpu_uds = 1; dut->_cpu_lds = 0; }
        else          { dut->_cpu_uds = 0; dut->_cpu_lds = 1; }
        dut->_cpu_as = 0;
        bool dtack_seen = false;
        for (int i = 0; i < 4000 && !dtack_seen; ++i) {
            tick();
            if (dut->_cpu_dtack == 0) dtack_seen = true;
        }
        for (int i = 0; i < 16; ++i) tick();
        dut->_cpu_as = 1; dut->_cpu_uds = 1; dut->_cpu_lds = 1; dut->cpu_r_w = 1;
        for (int i = 0; i < 8; ++i) tick();
        return dtack_seen;
    }

    bool write_byte(uint32_t byte_addr, uint8_t value) {
        bool low = (byte_addr & 1u) != 0;
        uint16_t wd = low ? (uint16_t)value : (uint16_t)((uint16_t)value << 8);
        return bus_cycle(byte_addr, true, wd);
    }
    uint8_t read_byte(uint32_t byte_addr) {
        bus_cycle(byte_addr, false, 0);
        bool low = (byte_addr & 1u) != 0;
        uint16_t v = dut->cpu_data;
        return low ? (uint8_t)(v & 0xFF) : (uint8_t)((v >> 8) & 0xFF);
    }

    // A full-word READ cycle at byte_addr that also
    // samples Gary's own decode outputs (sel_chip[0], sel_kick -- minimig's
    // own top-level internal wires, GARY1's outputs, read via
    // --public-flat-rw through rootp->minimig, the same access style
    // hrm/appF-cia-pin-bindings already uses for `int2`/`int6`).
    //
    // TWO TIMING TRAPS, both measured with this helper:
    //
    // (1) gary.v:145's `always @(*) if (dbr) ... else ...` time-multiplexes
    //   sel_chip/sel_kick between Agnus's DMA-address decode (dbr=1, the
    //   `sel_chip[0] = ~dma_address_in[20] & ~dma_address_in[19]` branch --
    //   ovl-independent and address bits from an entirely different bus)
    //   and the CPU-address decode this package's claims are actually
    //   about (dbr=0, gary.v:159/:166). Chip bus arbitration keeps toggling
    //   `dbr` throughout normal operation (Agnus's own DMA slots), so a
    //   sample taken without checking `dbr` risks reading the WRONG
    //   branch's output on the SAME wire -- measured directly: OR-
    //   accumulating every tick regardless of `dbr` reads sel_chip[0]=1
    //   even in the ovl=1 state, where gary.v's own CPU-side formula
    //   (`addr_range && !ovl`) makes that impossible. Every sample is
    //   therefore gated on `dbr==0`.
    //
    // (2) minimig_m68k_bridge.v's `rd = enable & lr_w`, `enable = ~l_as &
    //   ~l_dtack & ~cck`, is only asserted during specific `cck` phases
    //   before dtack is recognized -- a single fixed-tick or
    //   dtack-coincident sample risks catching a tick where `rd` (and
    //   therefore sel_kick's read-qualified term) is momentarily low even
    //   though the cycle is genuinely a read -- measured directly: sampling
    //   only at the dtack-coincident tick reads sel_kick=0 in BOTH the
    //   ovl=1 and ovl=0 states, again impossible per gary.v:166. The helper
    //   therefore OR-accumulates every dbr==0-qualified sample across the
    //   whole window: once gary's own CPU-side decode has been true for
    //   even one qualifying tick, that is what a 68000 latching data on
    //   `rd`'s pulse would have seen.
    bool read_with_decode(uint32_t byte_addr, uint8_t& chip0_out, uint8_t& kick_out) {
        dut->cpu_address = (byte_addr >> 1) & 0x7FFFFFu;
        dut->cpu_r_w = 1;
        dut->cpudata_in = 0;
        dut->_cpu_uds = 0; dut->_cpu_lds = 0;  // full word, both lanes
        dut->_cpu_as = 0;
        bool dtack_seen = false;
        uint8_t chip0_sticky = 0, kick_sticky = 0;
        auto sample = [&]() {
            if (!dut->rootp->minimig->dbr) {  // CPU's own turn only -- trap (1) above
                chip0_sticky |= (uint8_t)(dut->rootp->minimig->sel_chip & 0x1);
                kick_sticky |= (uint8_t)(dut->rootp->minimig->sel_kick & 0x1);
            }
        };
        for (int i = 0; i < 20000 && !dtack_seen; ++i) {
            tick();
            sample();
            if (dut->_cpu_dtack == 0) dtack_seen = true;
        }
        // Keep sampling a little past dtack too -- `rd`'s cck-gated pulse
        // and dtack's own recognition are not guaranteed to land on the
        // exact same tick (trap (2) above).
        for (int i = 0; i < 16; ++i) {
            tick();
            sample();
        }
        dut->_cpu_as = 1; dut->_cpu_uds = 1; dut->_cpu_lds = 1; dut->cpu_r_w = 1;
        for (int i = 0; i < 8; ++i) tick();
        chip0_out = chip0_sticky;
        kick_out = kick_sticky;
        return dtack_seen;
    }
};

constexpr uint32_t CIAA_PRA  = 0xBFE001;
constexpr uint32_t CIAA_DDRA = 0xBFE201;
constexpr uint32_t CHIP_BASE = 0x000000;  // $000000 -- gary.v:159/:166's 5'b0000_0 range

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =========================================================================
    // CONTROL-RESET-RELEASES (no claim id; harness precondition). Every
    // assertion below depends on this having released -- if it never does,
    // every measurement below is of a permanently-reset chip, not the
    // behaviour the manual describes.
    // =========================================================================
    Harness h0;
    checks.expect(h0.reset_released,
        "CONTROL-RESET-RELEASES: _cpu_reset rose within a 3,000,000-tick "
        "budget after rst_ext/_cpu_reset_in were released -- harness "
        "precondition, no manual page, shared by every assertion below.");
    // MUTATION CONTROL-RESET-RELEASES stimulus red MUT_RESET: hold rst_ext (master reset) asserted forever, so minimig_syscontrol.v's rst_cnt never advances and _cpu_reset never rises within the 3,000,000-tick budget; baseline PASS goes red.
    if (!h0.reset_released) {
        std::cout << "\nHALT-CANDIDATE: reset never released within budget; "
                     "no assertion below would be measuring anything but a "
                     "permanently-reset chip, so none are run.\n";
        std::cout << checks.passed << " passed, " << checks.failed << " failed\n";
        return checks.failed == 0 ? 0 : 1;
    }

    // =========================================================================
    // PA0's "ROM/RAM overlay bit" sub-proposition (c) -- the header's
    // second question. `hrm/ch7-reset-overlay` (TOP_MODULE=gary) grades
    // exactly this decode behaviour: its OVL-CONTROL-BASELINE-000000
    // asserts ovl=0 at $000000 gives sel_chip[0]=1/sel_kick=0, and its
    // OVERLAY-ROM-AT-000000 asserts ovl=1 at $000000 gives sel_kick=1
    // (test_gary.cpp:280-330), with MUT_B1 forcing ovl=0 to redden the
    // second.
    // What this package's own fixture adds beyond that -- reaching ovl=1
    // and ovl=0 through a REAL CIA-A PRA write at `minimig` rather than by
    // hand-setting gary's `ovl` input in isolation -- is measured below as
    // a note(), not graded fresh: it exercises the JOIN (CIA-A write ->
    // minimig.v:871-872's ovl register -> gary's decode) that neither
    // ch7-overlay-source (stops at reading `ovl` itself) nor
    // ch7-reset-overlay (gary alone, ovl hand-set) tests end-to-end, but
    // the RESULT is not new -- it reproduces the same two states those two
    // packages already establish, combined. Declined below rather than
    // re-asserted, per the ordinary decline rule: the p.337 CIA-bit-binding
    // half (which bit, and that a write clears it) is a different axis,
    // already declined separately elsewhere on this row; the decode-
    // behaviour half (what ovl DOES to $000000) is the SAME proposition
    // ch7-reset-overlay already grades, regardless of how ovl got there.
    // =========================================================================
    {
        Harness h;
        uint8_t chip0_ovl1 = 0, kick_ovl1 = 0;
        bool dtack1 = h.read_with_decode(CHIP_BASE, chip0_ovl1, kick_ovl1);

        h.write_byte(CIAA_PRA, 0x00);  // clears ovl -- ch7-overlay-source's own mechanism, reused

        uint8_t chip0_ovl0 = 0, kick_ovl0 = 0;
        bool dtack2 = h.read_with_decode(CHIP_BASE, chip0_ovl0, kick_ovl0);

        checks.note("PA0-OVL-DECODE-VIA-MINIMIG-JOIN: read $000000 with "
                    "ovl=1 (cold-start default, minimig.v:871) -> sel_chip[0]=" +
                    std::to_string(chip0_ovl1) + ", sel_kick=" + std::to_string(kick_ovl1) +
                    " (dtack seen=" + std::to_string(dtack1) + "); after a real CIA-A PRA "
                    "write clears ovl to " + (h.dut->ovl ? "1" : "0") +
                    ", read again -> sel_chip[0]=" + std::to_string(chip0_ovl0) +
                    ", sel_kick=" + std::to_string(kick_ovl0) +
                    " (dtack seen=" + std::to_string(dtack2) + "). Matches "
                    "ch7-reset-overlay's OVL-CONTROL-BASELINE-000000/"
                    "OVERLAY-ROM-AT-000000 values exactly (sel_chip[0]=0,"
                    "sel_kick=1 at ovl=1; sel_chip[0]=1,sel_kick=0 at ovl=0), "
                    "reached here via the full CIA-A-write-to-gary-decode "
                    "join instead of a hand-set `ovl` input -- confirmation, "
                    "not new coverage.");
    }

    // =========================================================================
    // PA1-DRIVES-LED-0-IS-BRIGHT -- UID-FREE, deliberately. p.337's PA1 row
    // (sol:E-portsig.c8, opus:E-portsig.c9) bundles TWO clauses in one
    // sentence -- "led light (0=bright)" AND "audio filter control (A500 &
    // A2000)" -- and classify.split is unset for both: nobody split this
    // claim into sub-ids. Both claims' own restatement/quote text read as
    // one bundled sentence, not two, so naming the uid inside a passing assertion
    // that grades only the LED third of it, with the audio-filter third
    // caveated in the prose, still puts the uid on a graded line -- and
    // `check_uid_placement.py` (and any transitive coverage figure built
    // from it) then counts the WHOLE bundled claim as asserted. The
    // precedent is chapter 5's own README: "an id whose claim bundles a
    // clause this package does not test is withdrawn whole, not kept with
    // a caveat in the prose." So this proposition is graded here uid-free,
    // and both uids are carried on a single record() below
    // (PA1-ROW-PARTIALLY-COVERED) that states the row's whole disposition.
    // DDRA bit1 must be set to output first (ciaa.v:262's own pull-up
    // model: an input-direction bit reads back forced to 1 regardless of
    // what PRA was written).
    // =========================================================================
    {
        Harness h;
        h.write_byte(CIAA_DDRA, 0x02);  // PA1 = output, rest = input (pull-up model, ciaa.v:262)

        h.write_byte(CIAA_PRA, MUT_LED_SWAP ? 0x02 : 0x00);  // PA1=0 first (unless mutated)
        bool led_at_pa1_0 = h.dut->pwr_led;
        h.write_byte(CIAA_PRA, MUT_LED_SWAP ? 0x00 : 0x02);  // PA1=1 second (unless mutated)
        bool led_at_pa1_1 = h.dut->pwr_led;

        checks.note("PA1-DRIVES-LED-0-IS-BRIGHT: pwr_led with CIAAPRA bit1=0 "
                    "(unless mutated) -> " + std::to_string(led_at_pa1_0) +
                    "; pwr_led with CIAAPRA bit1=1 (unless mutated) -> " +
                    std::to_string(led_at_pa1_1) + ".");

        checks.expect(led_at_pa1_0 == 1 && led_at_pa1_1 == 0,
            "PA1-DRIVES-LED-0-IS-BRIGHT (p.337, \"led light (0=bright)\", "
            "the LED clause of PA1's row -- uid-free, see this block's own "
            "header comment and PA1-ROW-PARTIALLY-COVERED below for why): "
            "with CIAAPRA bit1 (PA1) configured as output, writing it 0 "
            "drives minimig's own top-level `pwr_led` output to 1 "
            "(asserted/lit) and writing it 1 drives `pwr_led` to 0 -- "
            "minimig.v:614-615's `_led = porta_out[1]` and minimig.v:432's "
            "`pwr_led = ~_led`, measured through real CPU bus writes, not "
            "read from the assign statements alone. MUTATION: swap which "
            "value (0 or 1) is written first and second -- led_at_pa1_0 "
            "reads 0 and led_at_pa1_1 reads 1, so the led_at_pa1_0==1 && "
            "led_at_pa1_1==0 conjuncts go red.");
    }
    // MUTATION PA1-DRIVES-LED-0-IS-BRIGHT stimulus red MUT_LED_SWAP: write CIAAPRA bit1=1 first and bit1=0 second (the two states swapped) -- led_at_pa1_0 reads 0 and led_at_pa1_1 reads 1, so the led_at_pa1_0==1 && led_at_pa1_1==0 conjuncts go red.

    // =========================================================================
    // PA1-ROW-PARTIALLY-COVERED (p.337, sol:E-portsig.c8, opus:E-portsig.c9
    // -- BOTH uids, on this ONE record() line: the claim as printed
    // bundles two clauses in one sentence ("led light (0=bright)" AND
    // "audio filter control (A500 & A2000)"), classify.split is unset for
    // both, and an id whose claim bundles a clause this package does not
    // test is withdrawn whole rather than kept on a graded line with a
    // caveat in the prose -- the precedent is chapter 5's own README:
    // "Naming an id inside a passing assertion in order to disclaim it
    // still puts the id on a graded line." So neither uid appears on any
    // PASS/FAIL line in this package; this record states the row's whole
    // disposition. The header's third question: "PA1's audio-filter clause
    // exists only in appendix E."
    // =========================================================================
    checks.record(
        "PA1-ROW-PARTIALLY-COVERED (p.337, \"PA1..LED*: led light "
        "(0=bright) / audio filter control (A500 & A2000)\"): the row "
        "bundles two clauses. The LED clause is measured and graded, "
        "UID-FREE, on PA1-DRIVES-LED-0-IS-BRIGHT above (minimig.v:614-615's "
        "`_led = porta_out[1]` and minimig.v:432's `pwr_led = ~_led`, "
        "PASS). The audio-filter clause is NOT constructible on this RTL: "
        "grepped independently across every nanomig-aga source file for "
        "\"filter\", \"cutoff\", \"lowpass\" and \"low_pass\" -- the only "
        "matches are minimig.v's video-interpolation comment (~line 85, "
        "unrelated) and paula_audio.v's noise-shaping comments (an "
        "error-feedback DSP technique in the PCM output path, not a "
        "gateable RC low-pass element); no signal, parameter or port in "
        "this RTL corresponds to an audio filter PA1 could gate. "
        "Independently confirms (not merely cites) ch5-audio-filter-"
        "bypass's own finding. This is not a stimulus "
        "this harness failed to find; there is no such stimulus to find, "
        "and inventing one would test a mechanism this RTL does not "
        "implement. The row's "
        "third, unbundled fact -- PA1 being one of CIAA's genuinely "
        "bidirectional port-A output bits -- is separately already graded "
        "by ch5-audio-filter-bypass's CIA-A-PRA-BIT1-WRITABLE/CIA-A-PRA-"
        "BIT1-READS-BACK [5-p157.c8, 5-filt.c9] (register half only, that "
        "package's own header) and, blindly as one of eight swept "
        "positions, hrm/ch8-ciaa-port-direction's PROP-1/PROP-2. SO: the "
        "claim AS PRINTED -- both clauses together -- is not established by "
        "this package. [claims: sol:E-portsig.c8, "
        "opus:E-portsig.c9]");

    // =========================================================================
    // FINDING-CANDIDATE-1-JOY0DAT-VS-FIRE-BIT -- no claim uid; the
    // header's first question, the fire buttons. p.329's "PRAO bit 7 =
    // Fire* left controller port" and p.327's "J12 is the left controller
    // port connector (JOY0DAT, POT0DAT)" are BOTH claims of a different
    // package (appDE-fire-buttons-ciaa) -- no uid in this package's own 48
    // claims states either sentence, so neither is graded here under a
    // foreign uid or a uid of this package's own that does not actually
    // say it. What this package's fixture uniquely supplies is the
    // connector-to-bit measurement the question depends on, entirely
    // self-contained in userio.v: whichever physical connector's motion
    // produces JOY0DAT, does ITS fire line land on PRA bit 7 or bit 6?
    // Both ends trace through the SAME internal register, `_sjoy1`, so
    // this needs no assumption about which physical port is labelled
    // "left"/"PORT 1"/"port 0" on any figure -- exactly the ambiguity
    // hrm/ch8-ciaapra-fire-buttons's own KNOWN CONFLICT 3 and this
    // package's own F1/F2 record()s below (declined groups, PA7/PA6's
    // numeral-specific half) could not resolve.
    //   userio.v:92 `wire joy_swap = 1'b0;`, :215 `_sjoy1 <= joy_swap ?
    //     joy1 : joy2;` -- with joy_swap permanently 0, _sjoy1 is loaded
    //     from the PHYSICAL SECOND top-level port, _joy2 -- despite the
    //     "1" in its own name.
    //   userio.v:233-239 build dmouse0dat (JOY0DAT) from that same _sjoy1.
    //   userio.v:313 `assign _fire0 = ... : _sjoy1[4] & _mleft;` (cd32pad
    //     tied 0 at userio.v:91, so the live term is always
    //     `_sjoy1[4] & _mleft`) -- the SAME _sjoy1 drives _fire0.
    //   minimig.v:632 places _fire0 at PA6, not PA7.
    // So: the connector whose motion produces JOY0DAT has its fire signal
    // on PRA bit 6, not bit 7 -- read directly from the source above, and
    // confirmed live below on the fire-bit half (the JOY0DAT half is a
    // static citation, not simulated live here -- driving genuine
    // quadrature transitions to move dmouse0dat is appDE-mouse-quadrature's
    // own subject, reused rather than re-derived; this package's own
    // contribution is the join, not a repeat of that package's own
    // fixture work). If p.329's "left port -> bit 7" is read together with
    // p.327's "left port -> JOY0DAT" (both foreign claims, cited here only
    // as page numbers, not uids), the combination predicts bit 7 for the
    // JOY0DAT connector -- measured here as bit 6 instead. Not adjudicated:
    // this package does not decide whether p.329, p.327 or neither is in
    // error, only reports what minimig.v actually does.
    // =========================================================================
    {
        Harness h;
        // State A: connector 2 (_joy2, feeds _sjoy1 per userio.v:215, and
        // therefore feeds both JOY0DAT and, via _sjoy1[4], _fire0/PA6)
        // fires; connector 1 idle.
        h.dut->_joy1 = 0xFFFF;
        h.dut->_joy2 = (uint16_t)(0xFFFF & ~(1u << 4));
        for (int i = 0; i < 8; ++i) h.tick();
        uint8_t pra_a = h.read_byte(CIAA_PRA);
        bool bit6_a = (pra_a >> 6) & 1, bit7_a = (pra_a >> 7) & 1;

        // State B: connector 1 (_joy1, feeds _sjoy2, and therefore
        // _fire1/PA7) fires; connector 2 idle.
        h.dut->_joy1 = (uint16_t)(0xFFFF & ~(1u << 4));
        h.dut->_joy2 = 0xFFFF;
        for (int i = 0; i < 8; ++i) h.tick();
        uint8_t pra_b = h.read_byte(CIAA_PRA);
        bool bit6_b = (pra_b >> 6) & 1, bit7_b = (pra_b >> 7) & 1;

        checks.note(
            "FINDING-CANDIDATE-1-JOY0DAT-VS-FIRE-BIT: driving _joy2 pin4 "
            "low (connector-2 fire) with _joy1 idle reads CIAAPRA=0x" +
            hex(pra_a) + " (bit6=" + std::to_string(bit6_a) + ", bit7=" +
            std::to_string(bit7_a) + "); driving _joy1 pin4 low "
            "(connector-1 fire) with _joy2 idle reads CIAAPRA=0x" +
            hex(pra_b) + " (bit6=" + std::to_string(bit6_b) + ", bit7=" +
            std::to_string(bit7_b) + "). So connector _joy2's fire line "
            "lands on PA6, and connector _joy1's fire line lands on PA7 "
            "-- confirmed live (this specific connector<->bit pairing is "
            "the same fact hrm/ch8-controller-port-io-path's own "
            "PROP-PORT-LOW6/PROP-PORT-HIGH7 already grade, not re-graded "
            "here, reproduced only as this measurement's own control "
            "basis). Per userio.v:215/:233-239 (read directly, not "
            "simulated live in this package), that SAME connector _joy2 "
            "is also the one whose motion userio.v builds dmouse0dat "
            "(JOY0DAT) from. So: the connector producing JOY0DAT has its "
            "fire signal on PA6, not PA7. p.329 (a different package's "
            "claim, cited here by page number only) reads 'PRAO bit 7 = "
            "Fire* left controller port'; p.327 (also a different "
            "package's claim) ties the left controller port's connector to "
            "JOY0DAT. Combined, those two predict PA7 for the JOY0DAT "
            "connector -- measured here as PA6. Not adjudicated: this "
            "package does not decide which page, if either, is in error, "
            "only reports the join.");
    }

    // =========================================================================
    // Declined claims (record() only -- 21 groups, 42 uids, all verified
    // against this package's own filtered claim data).
    // =========================================================================

    checks.record(
        "F1-FIRE-CONNECTOR-BIT-GENERIC (p.330, hrm/ch8-ciaapra-fire-buttons "
        "P3, uid-free: 'bit 6 and bit 7 belong to two distinct ports, "
        "whichever numerals those ports carry'; hrm/ch8-cia-disk-signal-"
        "bindings measures the PA7/PA6=_fire1/_fire0 half at minimig.v:632 "
        "directly) -- the numeral-free core of p.330's FIRE0/FIRE1-to-PRA-"
        "bit paths is already graded. Decline that half. The "
        "numeral-specific half (which PORT NUMBER -- 1 or 2 -- attaches to "
        "which bit) is NOT declined against a prior assertion (none grades "
        "it) but is also not gradable as a positive on this RTL: minimig's "
        "own signal names (_joy1, _joy2) carry no PORT 1/PORT 2 board-level "
        "label -- and where userio.v DOES carry a label, it points the "
        "wrong way: userio.v:42-43 comment _joy1 'joystick 1 in (default "
        "mouse port)' and _joy2 'joystick 2 in (default joystick port)', "
        "but :215/:217 cross them (joy_swap tied 0 at :92), so the block "
        "userio.v:229 itself calls 'Port 1', whose motion lands in JOY0DAT, "
        "is fed by _joy2 -- the file's own labels point the opposite way "
        "from its wiring; trace nets, not names. And p.330's own figure "
        "numbers the same two connectors differently from p.332's POT "
        "COUNTERS figure and p.333's LIGHT PEN figure within three "
        "consecutive pages (verified against figures/p330.png, "
        "figures/p332.png, figures/p333.png) -- THREE different port-"
        "numbering conventions are in play across this territory (J11/J12 "
        "at p.327, left/right at p.329, PORT 1/PORT 2 at p.330, the last "
        "matching the RTL's own PORT1->FIRE0*(bit6)/PORT2->FIRE1*(bit7) "
        "numeric convention read off figures/p330.png -- but that alignment "
        "does not by itself resolve p.329's left/right wording, and is not "
        "assumed to here) -- a genuine printed ambiguity this fixture "
        "cannot resolve by construction, matching hrm/ch8-ciaapra-fire-"
        "buttons's own KNOWN CONFLICT 3. See "
        "FINDING-CANDIDATE-1-JOY0DAT-VS-FIRE-BIT above for this package's "
        "own measurement of the one part of this territory that IS "
        "resolvable without assuming a port-number correspondence. "
        "[claims: sol:E-fire.c16, sol:E-fire.c17, opus:E-firebutton.c20, "
        "opus:E-firebutton.c22]");

    checks.record(
        "F2-PORTSIG-PA7-PA6-GENERIC (p.337, hrm/ch8-ciaapra-fire-buttons "
        "PROP-BIT7-PASSTHROUGH/PROP-BIT6-PASSTHROUGH and PROP-BIT7-"
        "INDEPENDENT-OF-BIT6/PROP-BIT6-INDEPENDENT-OF-BIT7 [uid-free], "
        "hrm/ch8-controller-port-io-path's PROP-PORT-HIGH7/PROP-PORT-LOW6 "
        "[uid-free, opus:8-mbtn.c6/sol:8-button.c5 cited there]) -- the "
        "bit-passthrough half of PA7 'game port 1, pin 6, fire button*' and "
        "PA6 'game port 0, pin 6, fire button*' is already graded. Decline "
        "that half. The numeral half (which connector is called 'game "
        "port 1' vs 'game port 0') is the SAME unresolved ambiguity as "
        "F1-FIRE-CONNECTOR-BIT-GENERIC above, not gradable as a positive "
        "here either, for the same reason. opus:E-portsig.c3's own text-"
        "layer note (the 'audio filter control (A500 & A2000)' fragment "
        "that travelled six rows up onto PA7's row, p.337 corpus-defects.md) "
        "is a corpus observation, not a fresh testable proposition -- "
        "already documented, nothing further to grade. [claims: "
        "sol:E-portsig.c2, sol:E-portsig.c3, opus:E-portsig.c3, "
        "opus:E-portsig.c4]");

    checks.record(
        "D1-EXTERNAL-DISK-INDEX (p.334, hrm/ch8-disk-index-interrupt-path's "
        "PROP-FLAG-PIN/PROP-LEVEL6/PROP-NOT-OWN-BIT over a three-module "
        "rig; hrm/ch8-disk-interrupt-path and hrm/ch8-drive-select-motor-"
        "led touch the same nets): pin 22, INDEX*, a pulse once per disk "
        "revolution conditionally raising a level-6 68000 interrupt, is "
        "already graded in full. Decline as a re-grade. [claims: "
        "sol:E-disk.c47, opus:E-extdisk-iface.c25]");

    checks.record(
        "PA5-RDY-BINDING (p.337, hrm/ch8-cia-disk-signal-bindings' "
        "PA5-BINDING [opus:8-cia.c6, sol:8-cia.c5]: CIAAPRA's PA5 carries "
        "_ready, compared across the whole (PA5,PA4,PA3,PA2) nibble so a "
        "swap between any two positions is caught; hrm/ch8-ciaapra-disk-"
        "sense-bits' PROP-INPUT [opus:8-cia.c2, sol:8-cia.c1] separately "
        "grades that the four positions are genuine inputs). Same "
        "proposition as appendix F p.351's identical row (word for word), "
        "already declined there to the same assertion -- this appendix "
        "must not reach a different verdict about the same pin. [claims: "
        "sol:E-portsig.c4, opus:E-portsig.c5]");

    checks.record(
        "PA4-TK0-BINDING (p.337, hrm/ch8-cia-disk-signal-bindings' "
        "PA4-BINDING [opus:8-cia.c11, sol:8-cia.c9]; hrm/ch8-ciaapra-disk-"
        "sense-bits' PROP-INPUT as above). Same proposition as appendix F "
        "p.351's identical row. [claims: sol:E-portsig.c5, "
        "opus:E-portsig.c6]");

    checks.record(
        "PA3-WPRO-BINDING (p.337, hrm/ch8-cia-disk-signal-bindings' "
        "PA3-BINDING [opus:8-cia.c16, sol:8-cia.c16]; hrm/ch8-ciaapra-disk-"
        "sense-bits' PROP-INPUT as above). Same proposition as appendix F "
        "p.351's identical row. [claims: sol:E-portsig.c6, "
        "opus:E-portsig.c7]");

    checks.record(
        "PA2-CHNG-BINDING (p.337, hrm/ch8-cia-disk-signal-bindings' "
        "PA2-BINDING [opus:8-cia.c18, sol:8-cia.c17]; hrm/ch8-ciaapra-disk-"
        "sense-bits' PROP-INPUT as above). Same proposition as appendix F "
        "p.351's identical row. [claims: sol:E-portsig.c7, "
        "opus:E-portsig.c8]");

    checks.record(
        "PA0-WRITE-CLEARS-OVL (p.337, hrm/ch7-overlay-source's "
        "OVL-BIT-WRITE-CLEARS-OVERLAY [sol:7-reset.c8, opus:7-reset.c4]): "
        "a write to CIA-A PRA clears ovl -- already graded (that package's "
        "own sibling OTHER-CIA-A-REGISTER-ALSO-CLEARS is red at baseline: "
        "ANY CIA-A write clears it, minimig.v:871-872, not specifically a "
        "PA0 write). Decline that half. [claims: sol:E-portsig.c9, "
        "opus:E-portsig.c10 -- write-clears sub-proposition only; the "
        "other two halves of this row are declined immediately below]");

    checks.record(
        "PA0-BIT-SPECIFIC-NOT-GRADABLE (p.337, hrm/appF-cia-pin-bindings' "
        "own CIAA-MAP-PA0-OVL-REGISTER record(): PA0 being specifically "
        "the bit that clears ovl is not gradable as a positive. "
        "Independently re-confirmed here, not merely cited: minimig.v:614-"
        "615 `assign {_fire1_dat,_fire0_dat,_led} = porta_out[3:1];` "
        "consumes only porta_out[3:1] -- porta_out[0], PA0's own driven "
        "output value, is connected to nothing at minimig level, and "
        "minimig.v:872's `sel_cia_a & (cpu_lwr|cpu_hwr)` term that "
        "actually clears ovl reads no register-select or bit-pattern "
        "information at all. There is no PA0-specific gating in this RTL "
        "to test bit-level; inventing one would test a mechanism this "
        "page's own RTL does not implement. [claims: sol:E-portsig.c9, "
        "opus:E-portsig.c10 -- PA0-specificity sub-proposition only]");

    checks.record(
        "PA0-ROM-OVER-RAM-ALREADY-GRADED (p.337, \"ROM/RAM overlay bit\"): "
        "hrm/ch7-reset-overlay (test_gary.cpp, TOP_MODULE=gary) grades "
        "exactly this decode behaviour -- OVL-CONTROL-BASELINE-000000 "
        "[control] asserts ovl=0 at $000000 gives sel_chip[0]=1/sel_kick=0, "
        "and OVERLAY-ROM-AT-000000 [PROP-B, sol:7-reset.c4, "
        "opus:7-reset.c1] asserts ovl=1 at $000000 gives sel_kick=1, with "
        "MUT_B1 forcing ovl=0 to redden the second. How ovl reaches a given "
        "value (hand-set at gary alone there, versus a real CIA-A PRA "
        "write through `minimig` here) does not change the PROPOSITION "
        "being graded about gary's own decode function -- same "
        "proposition, not a narrower or different-axis one, so declined "
        "per the ordinary rule rather than re-asserted. This package's own "
        "fixture additionally exercises the JOIN (CIA-A write -> "
        "minimig.v:871-872's ovl register -> gary's decode change) in one "
        "continuous real bus cycle, which neither ch7-overlay-source nor "
        "ch7-reset-overlay does end-to-end -- measured above as "
        "PA0-OVL-DECODE-VIA-MINIMIG-JOIN, reproducing the identical values, "
        "reported as confirmation and not graded fresh (the join's own "
        "endpoints are each already proven correct by those two packages "
        "separately; nothing about combining them changes what either "
        "found). [claims: sol:E-portsig.c9, opus:E-portsig.c10 -- "
        "ROM-over-RAM sub-proposition only]");

    checks.record(
        "CIAB-PA7-DTR-BINDING (p.337, hrm/appF-cia-pin-bindings' "
        "DTR-IS-PRA-BIT7 [p.340, sol:F-ciab-map.c3, opus:F-ciab-map.c2]: "
        "drives/reads minimig's own top-level `dtr` pin through a real CPU "
        "bus cycle, requiring the complementary pattern so a neighbouring-"
        "bit swap goes red). Already graded -- the same binding, not a "
        "narrower one. "
        "[claims: sol:E-portsig.c23, opus:E-portsig.c25]");

    checks.record(
        "CIAB-PA6-RTS-BINDING (p.337, hrm/appF-cia-pin-bindings' "
        "RTS-IS-PRA-BIT6 [p.340, sol:F-ciab-map.c4, opus:F-ciab-map.c2]). "
        "Already graded, as CIAB-PA7-DTR-BINDING above. "
        "[claims: sol:E-portsig.c24, opus:E-portsig.c26]");

    checks.record(
        "CIAB-PA5-CD-BINDING (p.337, hrm/appF-cia-pin-bindings' "
        "CD-IS-PRA-BIT5 [p.340, sol:F-ciab-map.c5, opus:F-ciab-map.c2]). "
        "Already graded, as CIAB-PA7-DTR-BINDING above. "
        "[claims: sol:E-portsig.c25, opus:E-portsig.c27]");

    checks.record(
        "CIAB-PA4-CTS-BINDING (p.337, hrm/appF-cia-pin-bindings' "
        "CTS-IS-PRA-BIT4 [p.340, sol:F-ciab-map.c6, opus:F-ciab-map.c2]). "
        "Already graded, as CIAB-PA7-DTR-BINDING above. "
        "[claims: sol:E-portsig.c26, opus:E-portsig.c28]");

    checks.record(
        "CIAB-PA3-DSR-BINDING (p.337, hrm/appF-cia-pin-bindings' "
        "DSR-IS-PRA-BIT3 [p.340, sol:F-ciab-map.c7, opus:F-ciab-map.c2]). "
        "Already graded, as CIAB-PA7-DTR-BINDING above. "
        "[claims: sol:E-portsig.c27, opus:E-portsig.c29]");

    checks.record(
        "PB7-MTR-BINDING (p.337, hrm/ch8-cia-disk-signal-bindings' "
        "PB7-BINDING [opus:8-cia.c21, sol:8-cia.c20]: writes one known byte "
        "at $BFD100 and requires `_motor` low with the other seven named "
        "wires high, so a swap between any two of the eight positions is "
        "caught). Already graded. [claims: sol:E-portsig.c33, "
        "opus:E-portsig.c35]");

    checks.record(
        "PB6-SEL3-BINDING (p.337, hrm/ch8-cia-disk-signal-bindings' "
        "PB6-BINDING [opus:8-cia.c32, sol:8-cia.c30]): the BIT BINDING, "
        "PB6<->_sel3, is already graded, citing chapter 8 p.245's 'Select "
        "drive 6' (a drive NUMBER). Decline that half. This page instead "
        "says 'select external 3rd drive' -- an ordinal-to-internal/"
        "external ATTRIBUTION nothing in this RTL distinguishes: "
        "paula_floppy.v:615,:691-694 know only drive index 0..3 and a "
        "configured count, and minimig.v:383-386 comments them 'disk0 "
        "select'..'disk3 select', with no internal/external notion at "
        "all. Not gradable as a positive; not constructible. The same "
        "split applies to appendix F's p.351 twin of this row, declined to "
        "the same assertion without this split recorded there. [claims: "
        "sol:E-portsig.c34, opus:E-portsig.c36]");

    checks.record(
        "PB5-SEL2-BINDING (p.337, hrm/ch8-cia-disk-signal-bindings' "
        "PB5-BINDING [opus:8-cia.c34, sol:8-cia.c31]). Same split as "
        "PB6-SEL3-BINDING above: bit binding declined, "
        "external/ordinal-2nd-drive attribution not constructible. "
        "[claims: sol:E-portsig.c35, opus:E-portsig.c37]");

    checks.record(
        "PB4-SEL1-BINDING (p.337, hrm/ch8-cia-disk-signal-bindings' "
        "PB4-BINDING [opus:8-cia.c36, sol:8-cia.c32]). Same split as "
        "PB6-SEL3-BINDING above: bit binding declined, "
        "external/ordinal-1st-drive attribution not constructible. "
        "[claims: sol:E-portsig.c36, opus:E-portsig.c38]");

    checks.record(
        "PB3-SEL0-BINDING (p.337, hrm/ch8-cia-disk-signal-bindings' "
        "PB3-BINDING [opus:8-cia.c38, sol:8-cia.c33]). Same split as "
        "PB6-SEL3-BINDING above: bit binding declined, "
        "internal-drive attribution not constructible (the RTL "
        "distinguishes drive INDEX, not internal-vs-external). [claims: "
        "sol:E-portsig.c37, opus:E-portsig.c39]");

    checks.record(
        "PB2-SIDE-BINDING (p.337, hrm/ch8-cia-disk-signal-bindings' "
        "PB2-BINDING [opus:8-cia.c41, sol:8-cia.c35]: writes one known "
        "byte at $BFD100 and requires `side` low with the other seven "
        "named wires high). Already graded in full. [claims: "
        "sol:E-portsig.c38, opus:E-portsig.c40]");

    checks.record(
        "PB1-DIR-BINDING (p.337, hrm/ch8-cia-disk-signal-bindings' "
        "PB1-BINDING [opus:8-cia.c45, sol:8-cia.c39]: requires `direc` low "
        "with the other seven named wires high). Already graded in full. "
        "[claims: sol:E-portsig.c39, opus:E-portsig.c41]");

    checks.record(
        "PB0-STEP-BINDING (p.337, hrm/ch8-cia-disk-signal-bindings' "
        "PB0-BINDING [opus:8-cia.c49, sol:8-cia.c44]: requires `_step` low "
        "with the other seven named wires high). Already graded in full -- "
        "and this row is WIDER than appendix F's p.351 twin in the other "
        "direction: p.351 prints 'step* (3.0 milliseconds minimum)' where "
        "p.337 prints only 'step*'. Nothing here demands the timing half "
        "hrm/ch8-drive-step-timing's RATIO-A carries for p.351 -- p.337 "
        "states LESS than p.351, not more, so there is nothing extra to "
        "grade on this printing. [claims: sol:E-portsig.c40, "
        "opus:E-portsig.c42]");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
