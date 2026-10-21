// Test purpose: p.314's A1000/A500/A2000 Memory Map (Appendix D), the two
//   rows for the 8520-B ($BF D000-BF DF00) and 8520-A ($BF E001-BF EF01)
//   address ranges and the sentence beneath them -- "The underlined digit
//   chooses which of the 16 internal registers of the 8520 is to be
//   accessed. See Appendix F." -- at the `ciaa` module boundary, compiled
//   alone.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 314.
// Requirement IDs: none graded here -- see the RECORDED block in main() and
//   the "What this package found" note below. Claims file:
//   hrm-testpipeline/work/2026-09-17_appDE_packages.jsonl, filtered to
//   classify.package == "appDE-cia-register-select" (2 claims, both
//   restatements of the same p.314 sentence: sol:D-memmap-a1000.c9,
//   opus:D-memmap-a500.c10).
// Non-goals: anything outside the `ciaa` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the printed pages independently, and an id names
//   one reader's reading of one sentence. They resolve in
//   `work/2026-09-17_appDE_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// RTL read for step 2: ciaa.v (388 lines) in full; cia_int.v, cia_timera.v,
//   cia_timerb.v, cia_timerd.v (needed only for Verilator to elaborate the
//   top module, not read for this package's own reasoning). minimig.v was
//   read ONLY for the two cited line numbers below (:616, :641) -- it is not
//   compiled by this package's Makefile and is out of this fixture's own
//   boundary; every other minimig.v line is out of scope here.
//
// What this package found:
//   - ciaa.v:89, `input [3:0] rs` -- a 4-bit register-select input, already
//     the DECODED value. ciaa.v:136-150 (the address decoder) shows all 16
//     values of `rs` are distinguished (15 assert a named strobe, rs==4'hB
//     asserts none) -- "the underlined digit chooses which of the 16
//     internal registers is accessed" is exactly the proposition
//     hrm/appF-cia-register-map's REG-COUNT-16 and RS-SELECTS-DISTINCT
//     assertions already grade at this same module boundary, from
//     appendix F pp.339-340 (the Chip Register Map and CIAA address map).
//     Both claims here restate that identical mechanism from a different
//     page (appendix D's memory map, rather than appendix F's register
//     table) -- same proposition, not a narrower or different-axis
//     statement of it. Declined below.
//   - The row's own wording names WHICH digit does the choosing: "the
//     underlined digit" in `BF D000` / `BF E001`, i.e. address bits
//     A11-A8. `ciaa` itself has no A11-A8 input at all -- `rs` IS the
//     already-decoded nibble, not an address. The binding from a raw CPU
//     address's A11-A8 bits to ciaa's `rs` port is minimig.v's wiring, at
//     two sites: minimig.v:626 (`.rs(cpu_address_out[11:8])`, CIAA1's own
//     instantiation) and minimig.v:651 (the CIAB1 twin, same expression).
//     This is a FACT ABOUT MINIMIG.V, not about `ciaa` -- confirmed here by
//     reading ciaa.v's own port list (:80-104), which has no address input
//     of any width, only the pre-decoded `rs`. This package's Makefile does
//     not compile minimig.v and this package's top is `ciaa` alone, so
//     "the underlined digit IS A11-A8" cannot be constructed or observed at
//     this fixture's own boundary without adding a port `ciaa` does not
//     have -- a HALT-class finding, not worked around. It independently
//     confirms the same boundary hrm/appF-cia-register-map's own committed
//     decline already drew for the textually identical p.353 fact
//     ("Address bits A11, A10, A9, and A8 are used to specify which of the
//     16 internal registers you want to access", declined there as
//     opus:F-conn.c4/sol:F-signals.c61, "ciaa alone has no address
//     decoder") -- re-derived here from this package's own reading of
//     ciaa.v, not cited as license: neither hrm/appF-cia-register-map nor
//     hrm/ch8-ciaapra-address-window GRADES this address-bit binding (the
//     latter's own A1-A4-BFE001-SELECTS-PRA lines are record() calls, not
//     expect() calls, so they could not license a decline here even if they
//     tried -- checked directly in ch8-ciaapra-address-window's own
//     test_*.cpp, all four are checks.record(), none is checks.expect()).
//
// HALT, reported rather than worked around: this package builds a live
//   `ciaa` fixture (FIXTURE-LIVE below) and confirms both claims' behavioural
//   core is already graded elsewhere, but the address-bit half neither
//   claim's own restatement can be separated from ("the underlined digit"
//   IS the claim) is not observable at the boundary this package is
//   required to top at. Grading it would require either reaching into
//   minimig.v (out of this package's own scope, and the same scope
//   hrm/appF-cia-register-map already declined address-bit claims on) or
//   adding an address port `ciaa` does not have. Neither is done here.
//
// Mutation for the one assertion this file grades (infrastructure only):
//   built (WSL Verilator 5.052, `make MUT=MUT_FIXLIVE test`, full
//   `rm -rf obj_dir` rebuild) and the PASS/FAIL line quoted is what that run
//   printed.
//
// Baseline (no MUT): "1 passed, 0 failed" -- FIXTURE-LIVE PASS, both claims
//   declined, no HALT worked around.
//
// MUTATION FIXTURE-LIVE stimulus red: MUT_FIXLIVE repeats the first DDRA
//   byte (0x55) instead of a distinct second one -> "0 passed, 1 failed"
//   (FIXTURE-LIVE FAIL). Reverted.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vciaa.h"

#ifndef MUT_FIXLIVE
#define MUT_FIXLIVE 0
#endif

namespace {

constexpr uint8_t RS_DDRA = 0x2;

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

// Bus-timing protocol for ciaa.v, reused verbatim from hrm/appF-cia-
// register-map's own Harness (this exact fixture, same top module): regporta
// /ddrporta are posedge-clk, clk7_en-gated, so a write needs one tick to
// latch.
class Harness {
public:
    Harness() : dut(new Vciaa) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->clk7n_en = 1;
        dut->reset = 1;
        dut->aen = 0;
        dut->rd = 0;
        dut->wr = 0;
        dut->rs = 0;
        dut->data_in = 0;
        dut->tick = 0;
        dut->eclk = 0;
        dut->porta_in = 0;
        dut->portb_in = 0;
        dut->kms_level = 0;
        dut->kbd_mouse_type = 0;
        dut->kbd_mouse_data = 0;
        dut->hrtmon_en = 0;
        for (int i = 0; i < 8; ++i) step();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) step();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void write_reg(uint8_t rs, uint8_t data) {
        dut->aen = 1;
        dut->wr = 1;
        dut->rd = 0;
        dut->rs = rs;
        dut->data_in = data;
        step();
        dut->aen = 0;
        dut->wr = 0;
        dut->rs = 0;
        dut->data_in = 0;
        step();
    }

    uint8_t read_reg(uint8_t rs) {
        dut->aen = 1;
        dut->rd = 1;
        dut->wr = 0;
        dut->rs = rs;
        dut->eval();
        const uint8_t v = dut->data_out;
        dut->aen = 0;
        dut->rd = 0;
        dut->eval();
        return v;
    }

    void step() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    Vciaa* dut;
};

// ---------------------------------------------------------------------
// FIXTURE-LIVE: `ciaa` has never been measured by this package before. Two
// distinct, known bytes must both arrive at DDRA and read back correctly
// and distinctly before this package reports anything about it.
// ---------------------------------------------------------------------
bool fixture_live(Harness& h) {
    h.write_reg(RS_DDRA, 0x55);
    const uint8_t r1 = h.read_reg(RS_DDRA);
    h.write_reg(RS_DDRA, MUT_FIXLIVE ? 0x55 : 0xAA);
    const uint8_t r2 = h.read_reg(RS_DDRA);
    return r1 == 0x55 && r2 == 0xAA && r1 != r2;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    { Harness h; checks.expect(fixture_live(h),
        "FIXTURE-LIVE: two distinct, known DDRA bytes (0x55, 0xAA) both "
        "arrive and read back correctly and distinctly before this "
        "package reports anything about `ciaa` [no page -- infrastructure]"); }

    // ------------------------------------------------------------------
    // Both claims declined: same behavioural proposition already graded
    // at this module boundary; the address-bit half neither restatement
    // separates from that proposition is a HALT (see header), not a
    // duplicate-grading decline -- recorded, not folded into this line, so
    // the HALT is visible on its own.
    // ------------------------------------------------------------------
    checks.record(
        "sol:D-memmap-a1000.c9 opus:D-memmap-a500.c10 (p.314, 'The "
        "underlined digit chooses which of the 16 internal registers of "
        "the 8520 is to be accessed. See Appendix F.', both the 8520-B "
        "$BF D000 row and the 8520-A $BF E001 row): the 1-of-16-registers "
        "mechanism this sentence states is the identical proposition "
        "hrm/appF-cia-register-map's REG-COUNT-16 and RS-SELECTS-DISTINCT "
        "assertions already grade at this same `ciaa` boundary, from "
        "appendix F pp.339-340 -- this page's own text sends the reader "
        "there itself ('See Appendix F'). Not graded here as a duplicate.");

    checks.record(
        "sol:D-memmap-a1000.c9 opus:D-memmap-a500.c10 -- HALT, not a "
        "duplicate decline: 'the underlined digit' names address bits "
        "A11-A8, which `ciaa` does not have as a port at all (ciaa.v:89, "
        "`rs` is already the decoded 4-bit register-select value; ciaa.v's "
        "own port list, :80-104, has no address input of any width). The "
        "binding from a CPU address's A11-A8 to ciaa's `rs` is "
        "minimig.v's wiring (minimig.v:626,641, `.rs(cpu_address_out"
        "[11:8])`), a file this package's Makefile does not compile and "
        "outside a ciaa-only fixture's own boundary -- the same boundary "
        "hrm/appF-cia-register-map's own committed decline already drew "
        "for the textually identical p.353 fact (opus:F-conn.c4 "
        "sol:F-signals.c61). Neither hrm/appF-cia-register-map nor "
        "hrm/ch8-ciaapra-address-window grades this address-bit binding -- "
        "the latter's own A1-A4-BFE001-SELECTS-PRA lines are checks."
        "record() calls in its own test source, not checks.expect(), so "
        "they could not license a decline here even if cited. Grading "
        "'the underlined digit IS A11-A8' at this fixture's boundary would "
        "require adding an address port `ciaa` does not have, or reaching "
        "into minimig.v -- neither is done. HALT.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
