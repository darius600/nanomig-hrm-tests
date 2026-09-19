// appK-zorro2-cycle-in-68000-terms -- minimig_m68k_bridge conformance test
//
// Test purpose: grade 27 of this package's 51 claims (Amiga Hardware
//   Reference Manual, 3rd edition, Appendix K, pp.387-398, "Zorro II Bus
//   Architecture" / "Addressing and Control Signals" / "Data Transfer
//   Acknowledge") against `minimig_m68k_bridge` (minimig_m68k_bridge.v). The
//   other 24 are declined with record() below -- see SPECIFICATION.md for
//   the full per-claim disposition table (groups G1-G12 graded, D1-D7
//   declined).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp.387, 391, 393, 397, 398.
// Non-goals: anything outside the `minimig_m68k_bridge` fixture this package
//   builds, and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// Clocking: the 4-phase scheme and Harness::acquire_enable() sequence are
//   reused verbatim from hrm/appF-cia-bus-and-clock/test_minimig_m68k_
//   bridge.cpp, this same module's own sibling fixture in this suite --
//   that file's own comment records it as "verified empirically against a
//   standalone probe build" before being written there. Four additional
//   RTL facts specific to THIS package's claims (AS-to-DTACK latency, the
//   read-side byte-select gap, the VPA/VMA two-path structure, the async
//   AS-rise reset) were measured here via a throwaway probe, discarded,
//   not part of this package -- see SPECIFICATION.md's "Measured before
//   writing" section for the numbers.
//
// MUTATION METHODOLOGY: every expect() below is guarded by a compile-time
//   `-D` flag (`MUT ?=` in the Makefile). Every mutation was actually
//   built and run (WSL Verilator, `make clean; make MUT=<NAME> test`).

#include <cstdint>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vminimig_m68k_bridge.h"

#ifndef MUT_NEVER_ASSERT_AS
#define MUT_NEVER_ASSERT_AS 0
#endif
#ifndef MUT_NEVER_NEGATE_AS
#define MUT_NEVER_NEGATE_AS 0
#endif
#ifndef MUT_FORCE_NRDY_FAST_PATH
#define MUT_FORCE_NRDY_FAST_PATH 0
#endif
#ifndef MUT_NEVER_RELEASE_NRDY
#define MUT_NEVER_RELEASE_NRDY 0
#endif
#ifndef MUT_FORCE_ENABLES_ON_DEAD_EDGES
#define MUT_FORCE_ENABLES_ON_DEAD_EDGES 0
#endif
#ifndef MUT_FORCE_RW_LOW_WHEN_HIGH_EXPECTED
#define MUT_FORCE_RW_LOW_WHEN_HIGH_EXPECTED 0
#endif
#ifndef MUT_FORCE_RW_HIGH_WHEN_LOW_EXPECTED
#define MUT_FORCE_RW_HIGH_WHEN_LOW_EXPECTED 0
#endif
#ifndef MUT_REPEAT_ADDRESS
#define MUT_REPEAT_ADDRESS 0
#endif
#ifndef MUT_REPEAT_DATA
#define MUT_REPEAT_DATA 0
#endif
#ifndef MUT_NEVER_ASSERT_STROBES
#define MUT_NEVER_ASSERT_STROBES 0
#endif
#ifndef MUT_SWAP_LDS_UDS_CHECK
#define MUT_SWAP_LDS_UDS_CHECK 0
#endif
#ifndef MUT_COLLAPSE_READS_VIA_RW_FLIP
#define MUT_COLLAPSE_READS_VIA_RW_FLIP 0
#endif
#ifndef MUT_BYPASS_VPA
#define MUT_BYPASS_VPA 0
#endif
#ifndef MUT_NEVER_START_EQUIV
#define MUT_NEVER_START_EQUIV 0
#endif

namespace {

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

// Same 4-phase scheme as appF-cia-bus-and-clock's own Harness for this
// module: clk7_en pulses at phase 0, clk7n_en at phase 2, of a free-running
// phase counter this testbench drives directly (normally amiga_clk's job,
// out of this package's own one-top-module fixture boundary).
class Harness {
public:
    Harness() : dut(new Vminimig_m68k_bridge) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->clk7n_en = 0;
        dut->c1 = 0;
        dut->c3 = 0;
        dut->eclk = 0x100;
        dut->vpa = 0;
        dut->dbr = 0;
        dut->dbs = 0;
        dut->xbs = 0;
        dut->nrdy = 0;
        dut->cck = 0;
        dut->memory_config = 0;
        dut->_as = 1;
        dut->_lds = 1;
        dut->_uds = 1;
        dut->r_w = 1;
        dut->address = 0;
        dut->cpudatain = 0;
        dut->data_in = 0;
        phase = 0;
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->clk7_en = (phase == 0);
        dut->clk7n_en = (phase == 2);
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        phase = (phase + 1) & 3;
    }

    // A clock edge that carries neither 7M-clock enable, bypassing the
    // phase counter entirely -- used only by CLOCK-BASIS-7M below to show
    // nothing in this module moves on such an edge. Does not touch `phase`
    // (so proper phase-managed tick()s can resume seamlessly after).
    void dead_edge() {
        dut->clk7_en = MUT_FORCE_ENABLES_ON_DEAD_EDGES ? 1 : 0;
        dut->clk7n_en = MUT_FORCE_ENABLES_ON_DEAD_EDGES ? 1 : 0;
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    // Proven acquisition sequence, reused verbatim from
    // hrm/appF-cia-bus-and-clock/test_minimig_m68k_bridge.cpp.
    void acquire_enable() {
        dut->_as = MUT_NEVER_ASSERT_AS ? 1 : 0;
        dut->cck = 1;
        for (int i = 0; i < 16; ++i) tick();
        dut->cck = 0;
        for (int i = 0; i < 4; ++i) tick();
    }

    Vminimig_m68k_bridge* dut;
    int phase;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =========================================================================
    // FIXTURE-LIVENESS [not graded -- apparatus check, no claim uid]
    // address_out is a plain registered copy of address, gated by nothing --
    // a path independent of the whole _as/cck/DTACK acquisition every other
    // assertion below depends on. Same check appF already built for this
    // module; reused rather than reinvented.
    // =========================================================================
    {
        Harness h;
        h.dut->address = 0x001000 >> 1;
        h.tick();
        const uint32_t r1 = h.dut->address_out;
        h.dut->address = 0x002000 >> 1;
        h.tick();
        const uint32_t r2 = h.dut->address_out;
        checks.note("FIXTURE-LIVENESS: address_out follows address: r1=0x" +
                    std::to_string(r1) + " r2=0x" + std::to_string(r2) + ".");
        checks.expect(r1 == (0x001000 >> 1) && r2 == (0x002000 >> 1) && r1 != r2,
            "FIXTURE-LIVENESS [not graded -- apparatus check, no claim uid]: "
            "two distinct, known address values both arrive at address_out "
            "correctly and distinctly.");
    }

    // =========================================================================
    // Declined claims (record() only -- 24 of this package's 51 uids).
    // Grouped exactly as SPECIFICATION.md's D1-D7; full rationale there.
    // =========================================================================

    checks.record(
        "D1 MEMORY-SPACE-QUALIFIER-BUNDLE (p.391, sol:K-0098, opus:K-0117): "
        "both restatements require a memory-space-qualifier conjunct "
        "alongside R/W and byte-select strobes. No signal in "
        "minimig_m68k_bridge.v decodes address ranges or memory spaces at "
        "all -- that decode is gary.v's (sel_chip/sel_kick/etc.), already "
        "graded at gary's own boundary by hrm/appK-zorro2-memory-space-decode "
        "and hrm/appDE-memory-map-decode. Declined in full per the build "
        "brief's 'states more than the package tests' rule -- the boundary "
        "that would answer the memory-space half is gary.v, not this module.");

    checks.record(
        "D2 BUS-ARBITRATION-NAMED (p.393, opus:K-0143): restatement bundles "
        "'...and for other system functions such as bus arbitration' onto "
        "the 7M-clock-basis proposition Group G4 (graded: sol:K-0136) already "
        "grades. No BR/BG/BGACK or any multi-master arbitration signal "
        "exists on minimig_m68k_bridge's port list, and this single-CPU "
        "pinned design implements no bus arbitration anywhere that could be "
        "checked at any boundary. Declined -- the arbitration conjunct is "
        "unobservable, not merely out of this one module's scope.");

    checks.record(
        "D3 CCS-NAMED (p.397, 'Address Strobe' section: sol:K-0199, "
        "sol:K-0200, sol:K-0201, sol:K-0202, opus:K-0211): each restatement "
        "commits to '/CCS' as the acting signal. p.397's own heading reads "
        "'Address Strobe (/AS)...(/CCS) for Zorro III' -- the physical "
        "signal in Zorro II mode (the only mode this pinned design "
        "implements) IS /AS; /CCS is only the Zorro III renaming of the "
        "same pin. minimig_m68k_bridge.v has no port named _ccs, and this "
        "design has no Zorro III mode-switching logic anywhere (no "
        "Fat-Buster-equivalent controller). The identical underlying "
        "sentences, read and labelled /AS instead by opus "
        "(graded: opus:K-0209, opus:K-0210) below, ARE graded -- this is a "
        "per-claim label "
        "distinction, not a coin flip: each claim is judged on the signal "
        "name its own restatement commits to. Declined -- the boundary that "
        "would answer a /CCS-named claim is a Zorro III-capable bus "
        "controller, absent from this design entirely.");

    checks.record(
        "D4 ZORRO-II-AND-III-BUNDLE (p.398, sol:K-0213, opus:K-0221): both "
        "restatements require /DTACK to terminate 'both Zorro II AND Zorro "
        "III cycles' as one conjunction. This design implements no Zorro "
        "III mode anywhere -- unlike (graded: sol:K-0097/opus:K-0210) below, "
        "which reduce to the one mode (Zorro II-electrical) this pinned "
        "tree actually has. Declined -- the Zorro III half of the required "
        "conjunction has no boundary anywhere in this design.");

    checks.record(
        "D5 MASTER-S-STATE (p.398, sol:K-0214/opus:K-0223 [S4 DTACK "
        "sampling], sol:K-0215/opus:K-0224 [S6 data latch, opus also "
        "bundles S7], sol:K-0216 [S7 AS-negation timing], sol:K-0231/"
        "opus:K-0239 [XRDY-gated S4-or-later, also D6]): no S0-S7-labelled "
        "state register or comment exists anywhere in "
        "minimig_m68k_bridge.v (read in full) -- its own header diagram is "
        "a different CIA/ECLK timing diagram. This module is the bus "
        "TARGET: it drives _dtack, it never samples it; it drives data, it "
        "never latches an incoming read value the way a CPU's own bus-state "
        "machine would. No 68000-compatible CPU core is instantiated in "
        "this fixture (none exists under minimig-aga/ by that role at all). "
        "sol:K-0216 additionally states more than this boundary tests: the "
        "general 'AS negation ends the cycle' half IS graded in Group G1 "
        "(graded: opus:K-0210), but the specific 'during S7' qualifier is "
        "not, so "
        "the whole claim is declined here rather than partially graded on "
        "the reachable half. Declined -- the boundary that would answer any "
        "S4/S6/S7-specific claim is an actual 68000-compatible CPU core "
        "(e.g. TG68/fx68k) implementing its own bus-state sequencer.");

    checks.record(
        "D6 XRDY-NAMED (p.398, 'External Ready' and surrounding prose: "
        "sol:K-0218, opus:K-0226, sol:K-0227, sol:K-0229, sol:K-0230, "
        "sol:K-0231 [also D5], opus:K-0237, opus:K-0239 [also D5]): XRDY, "
        "per its own page text, 'is a wired-OR from all PICs' -- a "
        "slot-level, multi-card electrical bus, not a single module's "
        "input. minimig_m68k_bridge.v's `nrdy` input plays an analogous "
        "ROLE (holding off _dtack, measured below under Group G3) but is "
        "not XRDY reconstructed: nrdy is single-source and active-high "
        "NOT-ready (asserting it BLOCKS dtack), the functional and naming "
        "opposite of XRDY's active-high READY convention (default "
        "asserted/high=ready; a slow slave NEGATES/drives it low to hold "
        "off DTACK), and has none of XRDY's wired-OR multi-source "
        "character. Grading these eight claims via nrdy would report a "
        "fact about a differently-named, differently-driven, single-source "
        "signal as if it answered a claim about XRDY specifically. Declined "
        "-- compare (graded: sol:K-0100) in Group G3, which names no specific "
        "mechanism and IS graded via nrdy as the generic fact it actually "
        "states. The boundary that would answer an XRDY-named claim is an "
        "actual Zorro II slot with wired-OR ready logic from multiple "
        "PICs, absent from this design.");

    checks.record(
        "D7 READ-VS-RW-TIMING-COMPARISON (p.397, opus:K-0206): restatement "
        "implies two physically distinct signals -- a Zorro-bus 'READ' pin "
        "generated independently of, and possibly earlier than, 'a 68000 "
        "R/W line'. minimig_m68k_bridge.v has exactly one such input, "
        "r_w; its own registered copy lr_w is strictly LATER (clocked, "
        "delayed by up to one clk7_en period), never earlier, and no "
        "separate, independently-timed 'READ' output is derived from it "
        "anywhere in this module. The differential-timing relationship "
        "this claim states has no second signal at this boundary to "
        "compare r_w against. Declined.");

    // =========================================================================
    // GROUP G1 -- CYCLE-START-ON-AS-FALL (sol:K-0096, opus:K-0209)
    // p.391: "The cycle is defined by an address strobe..."
    // p.397: "The falling edge of [/AS] indicates that addresses are
    //   valid, the READ line is valid, and a Zorro II cycle is starting."
    // =========================================================================
    {
        Harness h;
        h.dut->r_w = 1;
        for (int i = 0; i < 4; ++i) h.tick(); // idle baseline, AS never touched
        const bool idle_inactive = (h.dut->_dtack == 1) && (h.dut->rd == 0) &&
                                    (h.dut->hwr == 0) && (h.dut->lwr == 0);

        h.acquire_enable(); // drives _as low (unless MUT_NEVER_ASSERT_AS)
        const bool active_after_as = (h.dut->_dtack == 0) && (h.dut->rd == 1);

        checks.note("CYCLE-START-ON-AS-FALL: idle (AS high) gave dtack=" +
                    std::to_string((int)h.dut->_dtack) + " rd/hwr/lwr all 0 = " +
                    std::to_string(idle_inactive) + "; after AS falls and the "
                    "cycle is acquired, dtack=0 (asserted) and rd=1: " +
                    std::to_string(active_after_as) + ".");
        checks.expect(idle_inactive && active_after_as,
            "sol:K-0096 (p.391): \"The cycle is defined by an address "
            "strobe.\" -- with AS held high nothing here reads active; "
            "once AS falls and the cycle is acquired, DTACK asserts and "
            "the read-enable line goes valid -- AS falling is what moves "
            "this bridge from idle to an active cycle.");
        checks.expect(idle_inactive && active_after_as,
            "opus:K-0209 (p.397): \"The falling edge of /AS indicates "
            "that addresses are valid, that the READ line is valid, and "
            "that a Zorro II cycle is starting.\" -- same transition: AS "
            "falling is what this bridge's own outputs respond to.");
    }
    // MUTATION sol:K-0096 stimulus red MUT_NEVER_ASSERT_AS: _as is held at 1 throughout acquire_enable() instead of being driven low -- dtack never asserts and rd never reads 1, so active_after_as goes false and the assertion goes red.
    // MUTATION opus:K-0209 stimulus red MUT_NEVER_ASSERT_AS: same stimulus change as sol:K-0096 above -- same shared boolean, same red.

    // =========================================================================
    // GROUP G1 (cont.) -- CYCLE-END-ON-AS-RISE (sol:K-0097, opus:K-0210)
    // p.391: "...terminated by a data transfer strobe..."
    // p.397: "The rising edge [of /AS] signals the end of a Zorro II bus
    //   cycle, signalling the current slave to negate all slave-driven
    //   signals as quickly as possible."
    // =========================================================================
    {
        Harness h;
        h.acquire_enable();
        const bool active_before = (h.dut->_dtack == 0);

        if (!MUT_NEVER_NEGATE_AS) h.dut->_as = 1;
        h.dut->eval(); // no clock edge at all -- _ta_n's own async-reset
                        // sensitivity (`posedge clk or posedge _as_and_cs`)
                        // is what this checks.
        const bool negated_immediately = (h.dut->_dtack == 1);

        checks.note("CYCLE-END-ON-AS-RISE: dtack asserted during the cycle "
                    "(" + std::to_string(active_before) + "); immediately "
                    "after AS rises, with no intervening clock edge, dtack "
                    "reads " + std::to_string((int)h.dut->_dtack) + " (1 = "
                    "deasserted).");
        checks.expect(active_before && negated_immediately,
            "sol:K-0097 (p.391): \"...terminated by a data transfer "
            "strobe...\" -- DTACK (the data-transfer-acknowledge strobe) "
            "was the slave-driven signal active through the cycle, and it "
            "is what clears when the cycle ends.");
        checks.expect(active_before && negated_immediately,
            "opus:K-0210 (p.397): \"The rising edge [of /AS] signals the "
            "end of a Zorro II bus cycle...to negate all slave-driven "
            "signals as quickly as possible.\" -- DTACK negates on a bare "
            "eval() after AS rises, with no clock edge needed at all: as "
            "quickly as this synchronous model can represent \"immediately\".");
    }
    // MUTATION sol:K-0097 stimulus red MUT_NEVER_NEGATE_AS: _as is never driven back to 1 after acquisition -- dtack stays asserted (0) instead of reading 1, so negated_immediately goes false and the assertion goes red.
    // MUTATION opus:K-0210 stimulus red MUT_NEVER_NEGATE_AS: same stimulus change as sol:K-0097 above -- same shared boolean, same red.

    // =========================================================================
    // GROUP G2 -- DURATION-FAST-PATH (sol:K-0099, opus:K-0118, sol:K-0217,
    // opus:K-0225). p.391: "...runs for a total of four cycles of a
    // 7.16MHz clock..."; p.398: "...this /DTACK will be driven by the bus
    // controller with no wait states, making the bus essentially a 4-cycle
    // synchronous bus."
    // =========================================================================
    {
        Harness h;
        for (int i = 0; i < 8; ++i) h.tick(); // idle settle
        h.dut->_as = 0;
        h.dut->cck = 1;
        h.dut->nrdy = MUT_FORCE_NRDY_FAST_PATH ? 1 : 0;
        int latency = -1;
        for (int i = 1; i <= 20; ++i) {
            h.tick();
            if (h.dut->_dtack == 0 && latency == -1) latency = i;
        }
        const bool within_four_cycles = (latency != -1) && (latency <= 16);

        checks.note("DURATION-FAST-PATH: in the no-intervention path "
                    "(nrdy=0), dtack first asserts at tick " +
                    std::to_string(latency) + " of this fixture's 4-ticks-"
                    "per-7M-cycle scheme (16 ticks = 4 cycles budget).");
        checks.expect(within_four_cycles,
            "sol:K-0099 (p.391): \"A basic Zorro II cycle lasts four "
            "periods of the 7.16 MHz clock.\" -- measured minimum: this "
            "bridge's own DTACK-generation latency, the mechanism a real "
            "4-cycle no-wait-state transaction depends on, is well inside "
            "that budget.");
        checks.expect(within_four_cycles,
            "opus:K-0118 (p.391): \"The basic Zorro II bus cycle runs for "
            "a total of four cycles..., extendable with wait states when "
            "required.\" -- same measurement (extendability is Group G3).");
        checks.expect(within_four_cycles,
            "sol:K-0217 (p.398): \"If the slave does not intervene, the "
            "controller supplies zero-wait-state /DTACK.\" -- measured "
            "with no intervention (nrdy=0): DTACK asserts promptly, well "
            "under the 4-cycle budget.");
        checks.expect(within_four_cycles,
            "opus:K-0225 (p.398): \"If a Zorro II slave does nothing, "
            "/DTACK is driven by the bus controller with no wait states, "
            "making the bus essentially a 4-cycle synchronous bus.\" -- "
            "same measurement.");
    }
    // MUTATION sol:K-0099 stimulus red MUT_FORCE_NRDY_FAST_PATH: nrdy is forced to 1 during what should be the no-intervention path -- dtack never asserts within the 20-tick search window, latency stays -1, and within_four_cycles goes false.
    // MUTATION opus:K-0118 stimulus red MUT_FORCE_NRDY_FAST_PATH: same stimulus change as sol:K-0099 above -- same shared boolean, same red.
    // MUTATION sol:K-0217 stimulus red MUT_FORCE_NRDY_FAST_PATH: same stimulus change as sol:K-0099 above -- same shared boolean, same red.
    // MUTATION opus:K-0225 stimulus red MUT_FORCE_NRDY_FAST_PATH: same stimulus change as sol:K-0099 above -- same shared boolean, same red.

    // =========================================================================
    // GROUP G3 -- WAIT-STATE-EXTENSION (sol:K-0100). p.391: "...though it
    // can be extended to add wait states when required."
    // =========================================================================
    {
        Harness h;
        for (int i = 0; i < 8; ++i) h.tick();
        h.dut->_as = 0;
        h.dut->cck = 1;
        h.dut->nrdy = 1; // slave not ready -- holds DTACK off
        bool asserted_while_nrdy = false;
        for (int i = 0; i < 40; ++i) {
            h.tick();
            if (h.dut->_dtack == 0) asserted_while_nrdy = true;
        }
        const bool held_off = !asserted_while_nrdy;

        if (!MUT_NEVER_RELEASE_NRDY) h.dut->nrdy = 0;
        bool asserted_after_release = false;
        for (int i = 0; i < 20; ++i) {
            h.tick();
            if (h.dut->_dtack == 0) { asserted_after_release = true; break; }
        }

        checks.note("WAIT-STATE-EXTENSION: with nrdy=1 held for 40 ticks, "
                    "dtack " + std::string(held_off ? "never" : "still") +
                    " asserted; after releasing nrdy, dtack asserted=" +
                    std::to_string(asserted_after_release) + ".");
        checks.expect(held_off && asserted_after_release,
            "sol:K-0100 (p.391): \"...though it can be extended to add "
            "wait states when required.\" -- nrdy held high extends the "
            "cycle (DTACK stays off through 40 ticks it would otherwise "
            "have asserted within 3); releasing it lets the cycle "
            "complete -- the cycle is genuinely extensible, not fixed-"
            "length.");
    }
    // MUTATION sol:K-0100 stimulus red MUT_NEVER_RELEASE_NRDY: nrdy is never lowered back to 0 after the 40-tick hold -- dtack never asserts in the following 20-tick window either, so asserted_after_release goes false and the assertion goes red.

    // =========================================================================
    // GROUP G4 -- CLOCK-BASIS-7M (sol:K-0136). p.393: "This clock forms
    // the basis for all Zorro [II]/68000 compatible activity..."
    // =========================================================================
    {
        Harness h;
        for (int i = 0; i < 8; ++i) h.tick();
        h.dut->_as = 0;
        h.dut->cck = 1;
        const uint8_t before_dtack = h.dut->_dtack;
        const uint8_t before_rd = h.dut->rd;
        const uint8_t before_hwr = h.dut->hwr;
        const uint8_t before_lwr = h.dut->lwr;

        h.dead_edge();
        h.dead_edge();
        const bool unchanged_on_dead_edges =
            (h.dut->_dtack == before_dtack) && (h.dut->rd == before_rd) &&
            (h.dut->hwr == before_hwr) && (h.dut->lwr == before_lwr);

        bool moved_once_enabled = false;
        for (int i = 0; i < 8; ++i) {
            h.tick();
            if (h.dut->_dtack != before_dtack) { moved_once_enabled = true; break; }
        }

        checks.note("CLOCK-BASIS-7M: two clock edges with neither clk7_en "
                    "nor clk7n_en active leave dtack/rd/hwr/lwr unchanged (" +
                    std::to_string(unchanged_on_dead_edges) + "); resuming "
                    "normal clk7_en/clk7n_en-gated ticks then moves dtack "
                    "within a further 8 ticks (" +
                    std::to_string(moved_once_enabled) + ") -- confirming "
                    "the earlier stall was specifically the missing "
                    "enables, not a deadlocked fixture.");
        checks.expect(unchanged_on_dead_edges && moved_once_enabled,
            "sol:K-0136 (p.393, corpus_damage: OCR 'Zorro H' for 'Zorro "
            "II'): \"7M is the timing basis for Zorro II and 68000-"
            "compatible bus activity.\" -- within this module's own scope, "
            "every observable output is inert on a clock edge that carries "
            "neither clk7_en nor clk7n_en, and live again the moment a "
            "properly-enabled edge resumes.");
    }
    // MUTATION sol:K-0136 stimulus red MUT_FORCE_ENABLES_ON_DEAD_EDGES: dead_edge() drives clk7_en=clk7n_en=1 instead of 0 on the two edges that should carry neither -- l_as (and, one edge later, the DTACK-clearing condition) update on those edges after all, dtack changes before the "dead" window ends, and unchanged_on_dead_edges goes false.

    // =========================================================================
    // GROUP G5 -- READ direction (sol:K-0195, opus:K-0204, opus:K-0205 [high half])
    // p.397: "READ asserted during a bus cycle indicates a read cycle...";
    // "This is the read enable for the bus, which is equivalent to the
    // 68000's R/W output."
    // =========================================================================
    {
        Harness h;
        h.acquire_enable();
        h.dut->r_w = MUT_FORCE_RW_LOW_WHEN_HIGH_EXPECTED ? 0 : 1;
        for (int i = 0; i < 4; ++i) h.tick();
        const bool read_shape = (h.dut->rd == 1) && (h.dut->hwr == 0) && (h.dut->lwr == 0);

        checks.note("READ-HIGH-IS-READ: r_w=1 gives rd/hwr/lwr = " +
                    std::to_string((int)h.dut->rd) + "/" +
                    std::to_string((int)h.dut->hwr) + "/" +
                    std::to_string((int)h.dut->lwr) + ".");
        checks.expect(read_shape,
            "sol:K-0195 (p.397): \"READ asserted during a bus cycle "
            "indicates a read cycle...\" -- r_w high (this bridge's own "
            "READ-equivalent input) gives rd=1 and neither write strobe.");
        checks.expect(read_shape,
            "opus:K-0204 (p.397): \"This is the read enable for the bus, "
            "which is equivalent to the 68000's R/W output.\" -- rd tracks "
            "r_w exactly as a read-enable derived from R/W would.");
        checks.expect(read_shape,
            "opus:K-0205 (p.397, read half of \"READ asserted...indicates "
            "a read cycle, READ negated indicates a write cycle\"): same "
            "measurement as sol:K-0195 above.");
    }
    // MUTATION sol:K-0195 stimulus red MUT_FORCE_RW_LOW_WHEN_HIGH_EXPECTED: r_w is driven to 0 instead of 1 -- rd reads 0 and the write strobes assert instead, so read_shape goes false and the assertion goes red.
    // MUTATION opus:K-0204 stimulus red MUT_FORCE_RW_LOW_WHEN_HIGH_EXPECTED: same stimulus change as sol:K-0195 above -- same shared boolean, same red.
    // MUTATION opus:K-0205 stimulus red MUT_FORCE_RW_LOW_WHEN_HIGH_EXPECTED: same stimulus change as sol:K-0195 above -- same shared boolean, same red.

    // =========================================================================
    // GROUP G5 (cont.) -- WRITE direction (sol:K-0196, opus:K-0205 [write half])
    // =========================================================================
    {
        Harness h;
        h.acquire_enable();
        h.dut->r_w = MUT_FORCE_RW_HIGH_WHEN_LOW_EXPECTED ? 1 : 0;
        h.dut->_uds = 0;
        h.dut->_lds = 0;
        for (int i = 0; i < 4; ++i) h.tick();
        const bool write_shape = (h.dut->rd == 0) && (h.dut->hwr == 1) && (h.dut->lwr == 1);

        checks.note("READ-LOW-IS-WRITE: r_w=0 (both strobes asserted) "
                    "gives rd/hwr/lwr = " + std::to_string((int)h.dut->rd) +
                    "/" + std::to_string((int)h.dut->hwr) + "/" +
                    std::to_string((int)h.dut->lwr) + ".");
        checks.expect(write_shape,
            "sol:K-0196 (p.397): \"READ negated indicates a write cycle.\" "
            "-- r_w low gives both write strobes asserted and rd=0.");
        checks.expect(write_shape,
            "opus:K-0205 (p.397, write half): same measurement.");
    }
    // MUTATION sol:K-0196 stimulus red MUT_FORCE_RW_HIGH_WHEN_LOW_EXPECTED: r_w is driven to 1 instead of 0 -- both write-strobe conjuncts go red (hwr/lwr read 0, rd reads 1 instead), so write_shape goes false.
    // MUTATION opus:K-0205 stimulus red MUT_FORCE_RW_HIGH_WHEN_LOW_EXPECTED: same stimulus change as sol:K-0196 above -- same shared boolean, same red (this is opus's own second, write-half citation of the same uid, per this package's [claims:] convention of naming a uid on every graded line its own restatement genuinely covers).

    // =========================================================================
    // GROUP G6 -- ADDRESS-WIDTH (sol:K-0197, opus:K-0207). p.397: "...
    // logically equivalent to the 68000's address bus, providing 16
    // megabytes of address space..."
    // =========================================================================
    {
        Harness h;
        h.dut->address = 0x000000;
        h.tick();
        const uint32_t r1 = h.dut->address_out;
        h.dut->address = MUT_REPEAT_ADDRESS ? 0x000000 : 0x7FFFFF; // full 23-bit span
        h.tick();
        const uint32_t r2 = h.dut->address_out;

        checks.note("ADDRESS-WIDTH: address=0x000000 -> address_out=0x" +
                    std::to_string(r1) + "; address=0x" +
                    std::to_string(MUT_REPEAT_ADDRESS ? 0x000000 : 0x7FFFFF) +
                    " -> address_out=0x" + std::to_string(r2) + ".");
        checks.expect(r1 == 0x000000 && r2 == 0x7FFFFF && r1 != r2,
            "sol:K-0197 (p.397): \"...providing 16 megabytes of address\" "
            "-- driving the full 23-bit span (0x000000 and the all-ones "
            "0x7FFFFF, the widest value this [23:1] port can carry) both "
            "arrive correctly and distinctly at address_out -- no bit is "
            "silently tied off.");
        checks.expect(r1 == 0x000000 && r2 == 0x7FFFFF && r1 != r2,
            "opus:K-0207 (p.397): \"This is logically equivalent to the "
            "68000's address bus, providing 16 megabytes of address "
            "space...\" -- same measurement.");
    }
    // MUTATION sol:K-0197 stimulus red MUT_REPEAT_ADDRESS: the second address is redriven as 0x000000 instead of the all-ones 0x7FFFFF -- r1 and r2 become identical, so the r1 != r2 conjunct (and the r2 == 0x7FFFFF conjunct) goes red.
    // MUTATION opus:K-0207 stimulus red MUT_REPEAT_ADDRESS: same stimulus change as sol:K-0197 above -- same shared boolean, same red.

    // =========================================================================
    // GROUP G7 -- DATA-WIDTH (sol:K-0203, opus:K-0213). p.397: "...
    // providing 16 bits of data accessible by word or either byte."
    // =========================================================================
    {
        Harness h;
        h.acquire_enable(); // establishes `enable` so ldata_in will latch
        h.dut->c1 = 0;
        h.dut->c3 = 1;
        h.dut->data_in = 0xA5A5;
        h.tick();
        const uint32_t r1 = h.dut->data;
        h.dut->data_in = MUT_REPEAT_DATA ? 0xA5A5 : 0x5A5A;
        h.tick();
        const uint32_t r2 = h.dut->data;

        checks.note("DATA-WIDTH: data_in=0xA5A5 -> data=0x" +
                    std::to_string(r1) + "; data_in=0x" +
                    std::to_string(MUT_REPEAT_DATA ? 0xA5A5 : 0x5A5A) +
                    " -> data=0x" + std::to_string(r2) + ".");
        checks.expect(r1 == 0xA5A5 && r2 == 0x5A5A && r1 != r2,
            "sol:K-0203 (p.397): \"D0-D15 transfer a word or either "
            "byte.\" -- two distinct full 16-bit patterns both arrive "
            "correctly and distinctly on data -- the whole word moves, "
            "not a truncated subset.");
        checks.expect(r1 == 0xA5A5 && r2 == 0x5A5A && r1 != r2,
            "opus:K-0213 (p.397): \"...a buffered version of the 680x0 "
            "data bus, providing 16 bits of data accessible by word or "
            "either byte.\" -- same measurement.");
    }
    // MUTATION sol:K-0203 stimulus red MUT_REPEAT_DATA: the second data_in is redriven as 0xA5A5 instead of 0x5A5A -- r1 and r2 become identical, so the r1 != r2 conjunct goes red.
    // MUTATION opus:K-0213 stimulus red MUT_REPEAT_DATA: same stimulus change as sol:K-0203 above -- same shared boolean, same red.

    // =========================================================================
    // GROUP G8 -- WRITE-STROBE-VALIDITY (sol:K-0205, sol:K-0206). p.397:
    // "...the data strobes to determine when data is valid on writes";
    // "These strobes fall on data valid during writes..."
    // =========================================================================
    {
        Harness h;
        h.acquire_enable();
        h.dut->r_w = 0; // write cycle
        for (int i = 0; i < 4; ++i) h.tick(); // strobes still deasserted (1/1)
        const bool invalid_before = (h.dut->hwr == 0) && (h.dut->lwr == 0);

        if (!MUT_NEVER_ASSERT_STROBES) { h.dut->_uds = 0; h.dut->_lds = 0; }
        for (int i = 0; i < 4; ++i) h.tick();
        const bool valid_after = (h.dut->hwr == 1) && (h.dut->lwr == 1);

        checks.note("WRITE-STROBE-VALIDITY: with UDS/LDS deasserted, "
                    "hwr/lwr = " + std::to_string((int)h.dut->hwr) + "/" +
                    std::to_string((int)h.dut->lwr) + " before asserting "
                    "them (invalid_before=" + std::to_string(invalid_before) +
                    "); after asserting both, hwr/lwr read valid=" +
                    std::to_string(valid_after) + ".");
        checks.expect(invalid_before && valid_after,
            "sol:K-0205 (p.397): \"A PIC uses...the data strobes to "
            "determine when data is valid on writes.\" -- hwr/lwr (this "
            "bridge's own write-data-validity indicators, which any "
            "downstream device would consult) read invalid until UDS/LDS "
            "assert, then valid.");
        checks.expect(invalid_before && valid_after,
            "sol:K-0206 (p.397): \"These strobes fall on data valid "
            "during writes...\" -- same measurement: write validity "
            "tracks the strobes falling (asserting).");
    }
    // MUTATION sol:K-0205 stimulus red MUT_NEVER_ASSERT_STROBES: _uds/_lds are never driven low -- hwr/lwr stay 0 even after the second tick round, so valid_after goes false and the assertion goes red.
    // MUTATION sol:K-0206 stimulus red MUT_NEVER_ASSERT_STROBES: same stimulus change as sol:K-0205 above -- same shared boolean, same red.

    // =========================================================================
    // GROUP G9 -- BYTE-SELECT MAPPING (sol:K-0208, sol:K-0209, opus:K-0217)
    // p.397: "The lower strobe is used for the lower byte (even byte), the
    // upper strobe is used for the upper byte (odd byte)."
    // =========================================================================
    {
        Harness h;
        h.acquire_enable();
        h.dut->r_w = 0;
        // LDS only (UDS deasserted) -- MUT swaps which strobe is driven,
        // so the "only lwr" isolation this assertion checks breaks.
        if (!MUT_SWAP_LDS_UDS_CHECK) { h.dut->_lds = 0; h.dut->_uds = 1; }
        else                          { h.dut->_uds = 0; h.dut->_lds = 1; }
        for (int i = 0; i < 4; ++i) h.tick();
        const bool lds_only_gives_lwr_only = (h.dut->lwr == 1) && (h.dut->hwr == 0);

        h.dut->_lds = 1; h.dut->_uds = 1;
        for (int i = 0; i < 2; ++i) h.tick();
        // UDS only (LDS deasserted)
        if (!MUT_SWAP_LDS_UDS_CHECK) { h.dut->_uds = 0; h.dut->_lds = 1; }
        else                          { h.dut->_lds = 0; h.dut->_uds = 1; }
        for (int i = 0; i < 4; ++i) h.tick();
        const bool uds_only_gives_hwr_only = (h.dut->hwr == 1) && (h.dut->lwr == 0);

        checks.note("BYTE-SELECT-MAPPING: LDS-only gives hwr/lwr=" +
                    std::to_string((int)h.dut->hwr) + "/" +
                    std::to_string((int)h.dut->lwr) + " overall (first "
                    "probe lwr_only=" + std::to_string(lds_only_gives_lwr_only) +
                    "); UDS-only gives hwr_only=" +
                    std::to_string(uds_only_gives_hwr_only) + ".");
        checks.expect(lds_only_gives_lwr_only,
            "sol:K-0208 (p.397): \"The lower strobe is used for the lower "
            "byte (even byte)...\" -- LDS alone asserted gives lwr=1 and "
            "hwr=0, and no other combination.");
        checks.expect(uds_only_gives_hwr_only,
            "sol:K-0209 (p.397): \"...the upper strobe is used for the "
            "upper byte (odd byte).\" -- UDS alone asserted gives hwr=1 "
            "and lwr=0.");
        checks.expect(lds_only_gives_lwr_only && uds_only_gives_hwr_only,
            "opus:K-0217 (p.397): \"The lower data strobe is used for the "
            "lower byte, the even byte, and the upper strobe for the "
            "upper byte, the odd byte.\" -- both halves of the mapping.");
    }
    // MUTATION sol:K-0208 stimulus red MUT_SWAP_LDS_UDS_CHECK: the "LDS-only" probe drives UDS instead (and vice versa for the second probe) -- lwr no longer asserts alone on the first probe (hwr does instead), so lds_only_gives_lwr_only goes false.
    // MUTATION sol:K-0209 stimulus red MUT_SWAP_LDS_UDS_CHECK: same swap -- the second probe now drives LDS instead of UDS, so uds_only_gives_hwr_only goes false.
    // MUTATION opus:K-0217 stimulus red MUT_SWAP_LDS_UDS_CHECK: same swap as both above -- both conjuncts go false.

    // =========================================================================
    // GROUP G10 -- BYTE-SELECT CLAIMED FOR BOTH READS AND WRITES
    // (sol:K-0207, opus:K-0216). EXPECTED RED: measured before writing
    // (see SPECIFICATION.md) that `rd` (this bridge's read-enable) does
    // not depend on _uds/_lds at all -- the "writes" half of this claim
    // holds (Group G9 above), the "reads" half does not.
    // =========================================================================
    {
        Harness h;
        h.acquire_enable();
        h.dut->r_w = 0; // writes: confirm they DO byte-select (reuses the
                         // Group G9 mechanism independently, self-contained)
        h.dut->_lds = 0; h.dut->_uds = 1;
        for (int i = 0; i < 4; ++i) h.tick();
        const uint8_t hwr_lds_only = h.dut->hwr, lwr_lds_only = h.dut->lwr;
        h.dut->_lds = 1; h.dut->_uds = 0;
        for (int i = 0; i < 4; ++i) h.tick();
        const uint8_t hwr_uds_only = h.dut->hwr, lwr_uds_only = h.dut->lwr;
        const bool writes_vary = (hwr_lds_only != hwr_uds_only) || (lwr_lds_only != lwr_uds_only);

        h.dut->_lds = 1; h.dut->_uds = 1;
        for (int i = 0; i < 2; ++i) h.tick();
        h.dut->r_w = 1; // reads: does rd vary with the same two probes?
        h.dut->_lds = 0; h.dut->_uds = 1;
        for (int i = 0; i < 4; ++i) h.tick();
        const uint8_t rd_lds_only = h.dut->rd;
        h.dut->_lds = 1; h.dut->_uds = 0;
        for (int i = 0; i < 4; ++i) h.tick();
        uint8_t rd_uds_only = h.dut->rd;
#if MUT_COLLAPSE_READS_VIA_RW_FLIP
        // MUTATION (stimulus green -- baseline below is RED, a real
        // measurement, not a stuck assertion): flip r_w for the second
        // read probe instead of only the strobes, so rd differs for a
        // reason entirely unrelated to byte-select (direction, not
        // strobes) -- collapsing reads_vary to true and the whole
        // assertion to green, proving the baseline RED is a genuine
        // reading of the strobe-only difference and not a coding error.
        h.dut->r_w = 0;
        for (int i = 0; i < 4; ++i) h.tick(); // let lr_w (registered from
                                               // r_w on clk7_en) actually
                                               // update -- rd depends on
                                               // lr_w, not live r_w.
        rd_uds_only = h.dut->rd;
#endif
        const bool reads_vary = (rd_lds_only != rd_uds_only);

        checks.note("BYTE-SELECT-BOTH-READS-AND-WRITES: writes vary with "
                    "UDS/LDS (hwr/lwr differ between the two probes)=" +
                    std::to_string(writes_vary) + "; reads vary with UDS/"
                    "LDS (rd differs between the two probes)=" +
                    std::to_string(reads_vary) + " (measured baseline: rd="
                    + std::to_string((int)rd_lds_only) + " for both probes "
                    "regardless of which strobe is asserted).");
        // CORRECTED 18.09.2026: both of these were expect() and both were
        // RED. They are declined now. The measurement is unchanged and still
        // printed in the note() above; what was wrong was that HALF THE
        // PARAGRAPH had been thrown away before the claim reached this file.
        //
        // p.397 does not stop at "indicate byte select for both reads and
        // writes". Its next two sentences are the exception to it:
        //
        //   "There is one slight difference between these lines and the 68000
        //    data strobes. On reads of Zorro II memory space, both /DS3 and
        //    /DS2 will be asserted, no matter what the actual size of the
        //    requested transfer is. This is required to support caching of
        //    the Zorro II memory space. For Zorro II I/O space, these strobes
        //    indicate the actual, requested byte enables, just as would a
        //    68000 bus master."
        //
        // Both readers extracted those sentences and the classification put
        // them at not-asserted/absent (sol:K-0210, sol:K-0212, opus:K-0218),
        // while the sentence they qualify came through as testable. So a rule
        // was graded and its own exception, three lines below it on the same
        // page, was discarded.
        //
        // Read whole, the claim is: byte select on writes always; on reads
        // only in Zorro II I/O space, and NOT in Zorro II memory space, where
        // both strobes assert regardless of transfer size. That is a claim
        // WITH A MEMORY-SPACE QUALIFIER, and this package already has the
        // rule for those: Group D1 declines sol:K-0098 and opus:K-0117 for
        // exactly it, because no signal in minimig_m68k_bridge.v decodes an
        // address range or a memory space at all -- that is gary.v's decode,
        // graded in hrm/appK-zorro2-memory-space-decode. The same boundary
        // answers these two.
        //
        // Worth saying plainly: `rd = enable & lr_w` with no strobe term is
        // what "on reads both strobes are asserted no matter the size" looks
        // like from the slave's side. The measurement was never evidence of a
        // divergence; it is close to the page's own qualification. It is left
        // as a measurement and deliberately NOT re-graded into a green --
        // this module cannot tell memory space from I/O space, so it cannot
        // establish the half of the claim that would pass.
        checks.record(
            "sol:K-0207, opus:K-0216 (p.397) -- declined: read with the rest "
            "of its own paragraph, this claim bundles a memory-space "
            "qualifier. p.397 says the strobes indicate byte select for reads "
            "and writes, and then says that on reads of Zorro II MEMORY space "
            "both strobes assert no matter what size the transfer is, while "
            "for Zorro II I/O space they indicate the actual byte enables. "
            "Those qualifying sentences are sol:K-0210, sol:K-0212 and "
            "opus:K-0218, all classified not-asserted/absent, so the rule "
            "arrived here without its exception. No signal in "
            "minimig_m68k_bridge.v decodes an address range or a memory space "
            "-- that is gary.v's decode, the boundary Group D1 already names "
            "for sol:K-0098 and opus:K-0117 -- so the surviving claim is not "
            "gradable here either. Measured above and not graded: rd carries "
            "no _uds/_lds term at all (minimig_m68k_bridge.v:139, `rd = "
            "enable & lr_w`, against hwr/lwr at :140-141), which is what the "
            "page's own exception describes from the slave's side. These were "
            "graded red before this correction.");
        (void)reads_vary;
    }
    // sol:K-0207 and opus:K-0216 are declined and carry no MUTATION line.
    // MUT_COLLAPSE_READS_VIA_RW_FLIP is kept as a measurement variant: it
    // flips r_w on the second read probe so rd differs for a reason unrelated
    // to byte select, which still shows the probe reads rd correctly and that
    // rd is not stuck.

    // =========================================================================
    // GROUP G11 -- VPA/VMA PERIPHERAL-SUPPORT MECHANISM (opus:K-0068)
    // p.387: "...the signals /VPA and /VMA, which comprise the 6800/6502
    // peripheral support mechanism that's part of the 68000 bus interface."
    // =========================================================================
    {
        Harness h;
        for (int i = 0; i < 8; ++i) h.tick();
        h.dut->vpa = MUT_BYPASS_VPA ? 0 : 1; // holds the plain !vpa shortcut unavailable
        h.dut->_as = 0;
        h.dut->cck = 1;
        h.dut->eclk = 0;

        bool asserted_before_vma_sequence = false;
        for (int i = 0; i < 4; ++i) {
            h.tick();
            if (h.dut->_dtack == 0) asserted_before_vma_sequence = true;
        }
        const bool stayed_off_until_vma = !asserted_before_vma_sequence;

        h.dut->eclk = (1u << 3); // latch vma (needs a clk7_en-phase tick)
        h.tick();
        h.dut->eclk = 0;
        for (int i = 0; i < 14; ++i) h.tick();
        h.dut->eclk = (1u << 8); // now let the dtack condition see vma&&eclk[8]
        bool asserted_via_vma_path = false;
        for (int i = 0; i < 10; ++i) {
            h.tick();
            if (h.dut->_dtack == 0) { asserted_via_vma_path = true; break; }
        }

        checks.note("VPA-VMA-MECHANISM: with vpa held asserted throughout "
                    "(the plain !vpa shortcut unavailable), dtack stayed "
                    "deasserted through 4 ticks before the VMA/eclk[8] "
                    "sequence began (" +
                    std::to_string(stayed_off_until_vma) + "); once eclk[3] "
                    "latches vma and eclk[8] then arrives, dtack asserts (" +
                    std::to_string(asserted_via_vma_path) + ").");
        checks.expect(stayed_off_until_vma && asserted_via_vma_path,
            "opus:K-0068 (p.387): \"...the signals /VPA and /VMA, which "
            "comprise the 6800/6502 peripheral support mechanism that's "
            "part of the 68000 bus interface.\" -- with VPA asserted, "
            "DTACK generation genuinely routes through a distinct VMA/"
            "eclk[8]-gated path (minimig_m68k_bridge.v:99-106,131) rather "
            "than the plain path -- the mechanism is real and load-"
            "bearing here, not dead wiring.");
    }
    // MUTATION opus:K-0068 stimulus red MUT_BYPASS_VPA: vpa is driven to 0 instead of 1, so the plain (!vpa) DTACK path is available immediately -- dtack asserts within the first few ticks, before the VMA/eclk[8] sequence even runs, and stayed_off_until_vma goes false.

    // =========================================================================
    // GROUP G12 -- DTACK EQUIVALENT TO 68000 DTACK INPUT, ZORRO II MODES
    // ONLY (opus:K-0222). p.398: "For Zorro II modes, it is equivalent to
    // the 68000's Data Transfer Acknowledge input."
    // =========================================================================
    {
        Harness h;
        for (int i = 0; i < 8; ++i) h.tick();
        const bool idle_deasserted = (h.dut->_dtack == 1);

        h.dut->_as = MUT_NEVER_START_EQUIV ? 1 : 0;
        h.dut->cck = 1;
        bool asserted_during_cycle = false;
        for (int i = 0; i < 8; ++i) {
            h.tick();
            if (h.dut->_dtack == 0) asserted_during_cycle = true;
        }

        h.dut->_as = 1;
        h.dut->eval();
        const bool deasserted_after = (h.dut->_dtack == 1);

        checks.note("DTACK-EQUIV-ZORRO-II: idle-deasserted=" +
                    std::to_string(idle_deasserted) + "; asserted during "
                    "an acquired Zorro-II-electrical cycle=" +
                    std::to_string(asserted_during_cycle) +
                    "; deasserted again once AS negates=" +
                    std::to_string(deasserted_after) + ".");
        checks.expect(idle_deasserted && asserted_during_cycle && deasserted_after,
            "opus:K-0222 (p.398): \"For Zorro II modes, it is equivalent "
            "to the 68000's Data Transfer Acknowledge input.\" -- this "
            "bridge's own _dtack performs exactly the DTACK-generation "
            "role a 68000's own DTACK input would be driven by, for the "
            "one bus mode (Zorro II-electrical) this pinned design "
            "implements at all -- idle high, asserted low through an "
            "active cycle, released on AS negation.");
    }
    // MUTATION opus:K-0222 stimulus red MUT_NEVER_START_EQUIV: _as is held at 1 throughout instead of falling -- dtack never asserts during the 8-tick window, so asserted_during_cycle goes false and the assertion goes red.

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
