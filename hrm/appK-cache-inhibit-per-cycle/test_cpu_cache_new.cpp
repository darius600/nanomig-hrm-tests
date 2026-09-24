// appK-cache-inhibit-per-cycle -- cpu_cache_new conformance test
//
// Test purpose: grade this package's one claim (Amiga Hardware Reference
//   Manual, 3rd edition, Appendix K, p.402, "Cache Support": "cards that
//   absolutely must not be cached [may] assert a cache inhibit line") --
//   sol:K-0295 -- against `cpu_cache_new` (cpu_cache_new.v). See
//   SPECIFICATION.md for the claim inventory (one claim), the boundary
//   argument, and the "checking the evidence myself" section this build
//   brief explicitly asked for.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 402.
// Non-goals: anything outside the `cpu_cache_new` fixture this package
//   builds, and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the id below carries a reader prefix -- `sol:` and `opus:` are the
//   two models that read the printed pages independently, and it names
//   one reader's reading of one printed sentence. They resolve in the chapter's
//   claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// MUTATION METHODOLOGY: every expect() below is guarded by a compile-time
//   `-D` flag (`MUT ?=` in the Makefile). Every mutation was actually
//   built and run (WSL Verilator, `make clean; make MUT=<NAME> test`).

#include <cstdint>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vcpu_cache_new.h"

#ifndef MUT_NEVER_INHIBIT_FIRST_FILL
#define MUT_NEVER_INHIBIT_FIRST_FILL 0
#endif
#ifndef MUT_INHIBIT_SECOND_FILL_TOO
#define MUT_INHIBIT_SECOND_FILL_TOO 0
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

class Harness {
public:
    Harness() : dut(new Vcpu_cache_new) {
        dut->clk = 0;
        dut->rst = 1;
        dut->cpu_cache_ctrl = 0x1; // bit0 = cache enable
        dut->cache_inhibit = 0;
        dut->cpu_cs = 0;
        dut->cpu_adr = 0;
        dut->cpu_bs = 0x3;
        dut->cpu_we = 0;
        dut->cpu_ir = 0;
        dut->cpu_dr = 0;
        dut->cpu_dat_w = 0;
        dut->sdr_dat_r = 0;
        dut->sdr_read_ack = 0;
        dut->snoop_act = 0;
        dut->snoop_adr = 0;
        dut->snoop_dat_w = 0;
        dut->snoop_bs = 0;
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();
    }

    // Cache-clear sweep: SDR_SM_INIT0/INIT1 count sdr_sm_adr by 4 until
    // sdr_sm_adr[9:2] (8 bits) are all set -- 256 steps -- before
    // cache_init_done. Settled generously (300 ticks), not the minimum.
    void reset_and_settle() {
        dut->rst = 1;
        for (int i = 0; i < 4; ++i) tick();
        dut->rst = 0;
        for (int i = 0; i < 300; ++i) tick();
    }

    // Services one 4-word SDRAM burst on the first sdr_read_req this
    // access raises (whether or not the DUT ends up consuming all four --
    // a cache_inhibit=1 fill only consumes the first; the stub does not
    // need to know which case it is in). Drives one read to completion
    // (cpu_ack) or up to `max_ticks`, whichever comes first.
    struct AccessResult {
        bool saw_req = false;
        uint16_t data = 0;
        bool acked = false;
        int ticks_used = 0;
    };
    // Feeds a full 4-word SDRAM burst to completion regardless of
    // cache_inhibit (a cache_inhibit=1 fill only consumes the first word
    // itself, per CPU_SM_FILL1's own branch -- but CPU_SM_FILL2/3/4 have
    // no `if (!cpu_cs)` escape at all, so this stub must not stop
    // early: doing so would leave the state machine wedged mid-burst
    // forever, and every access after it would silently hang instead of
    // being decided by cache_inhibit). Only after the whole scheduled
    // burst is delivered, plus a settle margin, does this drop cpu_cs and
    // let CPU_SM_FILLW's own `if (!cpu_ack) state <= CPU_SM_IDLE` fire.
    // `drive_cs`: when false, cpu_cs is never asserted, so the DUT never
    // raises sdr_read_req at all -- the request side never engages, so this
    // cannot wedge the fill state machine (see the "must not stop early"
    // warning above, which is about the SERVICE side, not the request side).
    AccessResult read(uint32_t byte_addr, bool inhibit, int max_ticks = 80, bool drive_cs = true) {
        static const uint16_t PATTERN[4] = {0x1111, 0x2222, 0x3333, 0x4444};
        dut->cache_inhibit = inhibit ? 1 : 0;
        dut->cpu_adr = byte_addr >> 1;
        dut->cpu_bs = 0x3;
        dut->cpu_we = 0;
        dut->cpu_ir = 0;
        dut->cpu_dr = 1;
        dut->cpu_cs = drive_cs ? 1 : 0;

        AccessResult r;
        bool burst_issued = false;
        int burst_remaining = 0;
        int word_idx = 0;
        int ticks_since_first_ack = -1;

        for (int i = 0; i < max_ticks; ++i) {
            if (dut->sdr_read_req) {
                r.saw_req = true;
                if (!burst_issued) { burst_issued = true; burst_remaining = 4; word_idx = 0; }
            }
            if (burst_remaining > 0) {
                dut->sdr_read_ack = 1;
                dut->sdr_dat_r = PATTERN[word_idx++];
                --burst_remaining;
            } else {
                dut->sdr_read_ack = 0;
            }

            tick();
            r.ticks_used = i + 1;

            if (dut->cpu_ack && !r.acked) {
                r.acked = true;
                r.data = dut->cpu_dat_r;
            }
            if (r.acked) {
                ticks_since_first_ack = (ticks_since_first_ack < 0) ? 0 : ticks_since_first_ack + 1;
                if (burst_remaining == 0 && ticks_since_first_ack >= 6) break;
            }
        }
        dut->sdr_read_ack = 0;
        dut->cpu_cs = 0;
        // Settle: cpu_ack clears one cycle after cpu_cs drops (the
        // unconditional `if (!cpu_cs) cpu_ack <= 0` rule), then
        // CPU_SM_FILLW's own `if (!cpu_ack) state <= IDLE` fires the
        // cycle after that -- 6 ticks is a generous margin over the 2
        // actually needed.
        for (int i = 0; i < 6; ++i) tick();
        return r;
    }

    Vcpu_cache_new* dut;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    Harness h;
    h.reset_and_settle();

    checks.note("cache_inhibit is tied to nanomig.v:423's literal 1'b0 in "
                "the assembled system (confirmed by reading the "
                "instantiation directly) -- this fixture drives it as a "
                "free input, since that is cpu_cache_new's own port, not "
                "the integration's choice. The capability graded below "
                "exists at this module and is simply unused by the "
                "current top-level wiring; both are true at once.");

    // =========================================================================
    // FIXTURE-LIVENESS [not graded -- apparatus check, no claim uid]
    // Both cold (first) reads below must themselves be genuine misses
    // (sdr_read_req observed, cpu_ack eventually asserts) -- otherwise a
    // "second read never misses" finding could just mean the fixture
    // never drove a real access at all.
    // =========================================================================
    // MUT_NEVER_INHIBIT_FIRST_FILL flips address A's own cold fill to
    // inhibit=true instead of false -- named for what it does to the fill
    // that sub-check 1 (below) depends on staying UNinhibited.
    const bool a_fill_inhibit = MUT_NEVER_INHIBIT_FIRST_FILL ? true : false;
#ifdef MUT_NO_COLD_REQUEST
    // MUTATION FIXTURE-LIVENESS stimulus red: cpu_cs is never asserted for
    // address A's cold fill, so the DUT never raises sdr_read_req --
    // coldA.saw_req and coldA.acked both stay false
    // (`make -B MUT=NO_COLD_REQUEST test`: "PASS: FIXTURE-LIVENESS [not
    // graded...]" becomes "FAIL: FIXTURE-LIVENESS [not graded...]", 1
    // passed, 2 failed). Also reddens sol:K-0295 sub-check 1/2: with A
    // never actually filled, the later "hot" re-read of A is itself a
    // genuine cold miss ("FAIL: sol:K-0295 (p.402, sub-check 1/2)");
    // sub-check 2/2 stays "PASS" (address B's own fill is untouched).
    auto coldA = h.read(0x100000, /*inhibit=*/a_fill_inhibit, /*max_ticks=*/80, /*drive_cs=*/false);
#else
    auto coldA = h.read(0x100000, /*inhibit=*/a_fill_inhibit);
#endif
    // MUT_INHIBIT_SECOND_FILL_TOO flips address B's own cold fill to
    // inhibit=false instead of true -- named for what it does to the fill
    // that sub-check 2 (below) depends on staying inhibited.
    const bool b_fill_inhibit = MUT_INHIBIT_SECOND_FILL_TOO ? false : true;
    auto coldB = h.read(0x200000, /*inhibit=*/b_fill_inhibit);
    checks.note("FIXTURE-LIVENESS: cold read of address A (inhibit=" +
                std::to_string(a_fill_inhibit) + "): saw_req=" +
                std::to_string(coldA.saw_req) + " acked=" +
                std::to_string(coldA.acked) + " ticks=" +
                std::to_string(coldA.ticks_used) + "; cold read of address "
                "B (inhibit=" + std::to_string(b_fill_inhibit) + "): "
                "saw_req=" + std::to_string(coldB.saw_req) +
                " acked=" + std::to_string(coldB.acked) + " ticks=" +
                std::to_string(coldB.ticks_used) + ".");
    checks.expect(coldA.saw_req && coldA.acked && coldB.saw_req && coldB.acked,
        "FIXTURE-LIVENESS [not graded -- apparatus check, no claim uid]: "
        "both cold reads are genuine misses that reach cpu_ack -- the "
        "fixture actually drives a real access, not a stuck one.");

    // =========================================================================
    // GRADED -- sol:K-0295. Two sub-checks, both citing the same uid (the
    // claim's own single proposition: caching can be suppressed for one
    // cycle at a time via cache_inhibit).
    // =========================================================================

    // Sub-check 1: re-read address A a second time (this second read's
    // own inhibit value is irrelevant -- only the COLD fill above decides
    // whether A got cached). Baseline: A's cold fill used inhibit=false,
    // so the tag SHOULD have been written -- expect a HIT (no new
    // sdr_read_req). Under MUT_NEVER_INHIBIT_FIRST_FILL, A's cold fill
    // above was inhibited instead, so this now expects (and gets) a miss.
    auto hotA = h.read(0x100000, /*inhibit=*/false);
    const bool a_hits_second_time = !hotA.saw_req && hotA.acked;

    // Sub-check 2: re-read address B a second time. Baseline: B's cold
    // fill above used inhibit=true, so the tag should NOT have been
    // written -- expect a MISS again (sdr_read_req reasserted). Under
    // MUT_INHIBIT_SECOND_FILL_TOO, B's cold fill above was NOT inhibited,
    // so this now expects (and gets) a hit instead.
    auto hotB = h.read(0x200000, /*inhibit=*/true);
    const bool b_misses_again = hotB.saw_req && hotB.acked;

    checks.note("CACHE-INHIBIT-PER-CYCLE: address A (filled with "
                "cache_inhibit=" + std::to_string(a_fill_inhibit) +
                ") re-read: saw_req=" + std::to_string(hotA.saw_req) +
                " acked=" + std::to_string(hotA.acked) + "; address B "
                "(filled with cache_inhibit=" + std::to_string(b_fill_inhibit) +
                ") re-read: saw_req=" + std::to_string(hotB.saw_req) +
                " acked=" + std::to_string(hotB.acked) + ".");
    checks.expect(a_hits_second_time,
        "sol:K-0295 (p.402, sub-check 1/2): \"...cards that absolutely "
        "must not be cached [may] assert a cache inhibit line...\" -- an "
        "address filled WITHOUT cache_inhibit is cached: its second read "
        "hits (no repeat sdr_read_req) -- the baseline caching path this "
        "package's mutation half will contrast against.");
    checks.expect(b_misses_again,
        "sol:K-0295 (p.402, sub-check 2/2): the same proposition's other "
        "half -- an address filled WITH cache_inhibit=1 "
        "(cpu_cache_new.v:363's CPU_SM_FILLW branch, skipping the tag "
        "write and the 3-word rest of the line fill) is NOT cached: its "
        "second read misses again, reissuing sdr_read_req -- caching was "
        "genuinely suppressed for that one cycle, not merely delayed.");
    // MUTATION sol:K-0295 stimulus red MUT_NEVER_INHIBIT_FIRST_FILL: address A's own cold fill above is driven with cache_inhibit=true instead of false -- A is then never cached either, its second read reissues sdr_read_req instead of hitting, and a_hits_second_time (sub-check 1/2) goes red.
    // MUTATION sol:K-0295 stimulus red MUT_INHIBIT_SECOND_FILL_TOO: address B's own cold fill above is driven with cache_inhibit=false instead of true -- B gets cached after all, its second read hits instead of reissuing sdr_read_req, and b_misses_again (sub-check 2/2) goes red.

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
