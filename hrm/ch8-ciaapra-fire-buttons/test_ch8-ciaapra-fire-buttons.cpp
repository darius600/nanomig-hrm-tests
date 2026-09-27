// Test purpose: HRM conformance package for printed pages 232-233's CIAAPRA
//   controller-port fire-button bits (6 and 7), against the `ciaa` module
//   compiled alone. 10 claims, from
//   work/ch8-s1-build/ch8-ciaapra-fire-buttons.jsonl. Full derivation in
//   SPECIFICATION.md.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed p.232 ("Port 1
//   uses bit 6 and port 2 uses bit 7. A logic state of 1 means 'switch
//   open.' A logic state of 0 means 'switch closed.'") and p.233 ("The fire
//   buttons for ports 0 and 1 are connected to bits 6 and 7 of CIAAPRA
//   ($BFE001). A 0 here indicates the switch is closed.").
// Fixture boundary: `ciaa` (ciaa.v) plus the four sibling files Verilator
//   needs to elaborate it -- cia_int.v, cia_timera.v, cia_timerb.v,
//   cia_timerd.v -- confirmed by this file's own Makefile SRC list, the
//   only place a module could be added. That elaboration pattern (which
//   files, in which order) is hrm/ch8-ciaa-port-direction's Makefile's.
//   This file's own harness/mutation boilerplate (the Checks class, the
//   MUTATE env-var convention, the summary-line format) follows
//   hrm/ch8-appb-chip-column-paula/test_paula.cpp, an unrelated fixture
//   (paula, not ciaa), this suite's existing convention -- no
//   ciaa-specific fact comes from that file.
// Non-goals: anything outside the `ciaa` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/ch8-s1-build/ch8-ciaapra-fire-buttons.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// RTL FACT USED THROUGHOUT (ciaa.v): the address decoder gives CIAAPRA at rs=4'h0 (ciaa.v:136,
//   "pra = enable && rs==4'h0"). Reading it combinationally drives
//   pa_out[7:0] = {porta_in2[7:2], porta_out[1:0]} (ciaa.v:253-254), where
//   porta_in2[7:2] is porta_in[7:2] synchronized one clk7_en period earlier
//   (ciaa.v:227-230). Bits 7:2 are pass-through from the porta_in pins,
//   completely unmodified and ungated by DDRA -- there is no inverter and
//   no coupling between bit 6 and bit 7 anywhere in this path.
//
// WHAT IS GRADED AND WHAT IS DECLINED. All 10 claims in this package are
//   declined via record(), each naming the specific fact this ciaa-alone
//   fixture cannot supply (switch-to-pin wiring, or the disputed numeral).
//   What is graded is fixture liveness (A0) and the raw CIA port
//   pass-through / bit-independence facts (PROP-BIT6/7-PASSTHROUGH,
//   PROP-BIT6/7-INDEPENDENT-OF-*), all uid-free:
//
//   (1) opus:8-mbtn.c7/.c8 and sol:8-button.c6/.c7 name a switch state
//   ("switch open" / "switch closed"). Driving porta_in[6]/[7] and reading
//   the bit back shows only fact (i) of DECLINED BY MODULE BOUNDARY's own
//   two-fact chain, below -- it cannot show that a mechanically
//   open/closed switch actually drives the pin to that level (fact (ii)),
//   the identical fact this file declines for opus:8-joy2.c3/sol:8-joy.c19.
//   Grading fact (i) alone under a switch-labelled uid would claim more
//   than the fixture proves; the same module boundary cannot be outside
//   the fixture for one claim and inside it for four others. All four
//   uids are declined below with that identical boundary reason; the
//   pin-level fact they partially rest on is graded as two uid-free
//   propositions, PROP-BIT6-PASSTHROUGH and PROP-BIT7-PASSTHROUGH, worded
//   as a pass-through fact rather than a switch-state claim.
//
//   (2) opus:8-joy2.c1/.c2 and sol:8-joy.c17/.c18 each name p.233's
//   disputed port numeral. The coverage ledger counts a uid ASSERTED
//   whenever it appears on a graded line, regardless of what the label
//   says around it, so carrying these uids on a graded line would record
//   the numeral half of each claim as carried when nothing tests it. The
//   undisputed ordering proposition (bit 6 and bit 7 are distinct and
//   non-interacting, whichever numeral either page attaches) lives on two
//   uid-free graded lines, PROP-BIT6-INDEPENDENT-OF-BIT7 and
//   PROP-BIT7-INDEPENDENT-OF-BIT6; the four numeral uids sit on record()
//   lines beside them, naming the p.232-versus-p.233 conflict. This is
//   the shape hrm/ch8-controller-port-io-path uses for its own page-232
//   numeral uids (opus:8-mbtn.c5/.c6, sol:8-button.c4/.c5; its
//   PROP-PORT-LOW6/PROP-PORT-HIGH7 and their adjoining record() calls) --
//   the shape only, not any ciaa-specific fact.
//
// KNOWN CONFLICT 3 (port numbering).
//   p.233 numbers the controller ports 0 and 1; p.232 and the rest of the
//   chapter number the same two ports 1 and 2. opus:8-joy2.c1/c2 and
//   sol:8-joy.c17/c18 each name a specific decimal port label for a
//   specific bit. What is NOT disputed between the pages: there are
//   exactly two ports, each on its own CIAAPRA bit (6 or 7), and neither
//   page ever suggests the two bits interact. This package grades exactly
//   that undisputed core -- CIAAPRA bit 6 (resp. 7) responds only to its
//   own porta_in pin, never to the other port's -- on two uid-free graded
//   lines, and never grades which decimal label attaches to which bit; the
//   four uids that name a numeral are declined via record(), not kept on
//   either graded line (see above).
//
// DECLINED BY MODULE BOUNDARY: opus:8-joy2.c3 / sol:8-joy.c19 ("a 0 in the
//   fire-button bit means the switch is closed") chain two facts: (i) the
//   CIAAPRA bit equals its pin (proven below, PROP-BIT6/7-PASSTHROUGH), and
//   (ii) a mechanically-closed switch drives that pin low. Fact (ii) is a
//   property of the controller-port-to-chip wiring, not of ciaa itself --
//   ciaa.v has no switch, no connector and no polarity logic; grep over
//   ciaa.v, cia_int.v, cia_timera.v, cia_timerb.v, cia_timerd.v for
//   "switch" or "joy" or "button": zero hits. This fixture (ciaa alone)
//   cannot settle fact (ii), the same kind of boundary argument that
//   routes CIAAPRA *address* claims to ch8-ciaapra-address-window (a rig
//   over ciaa+gary). Declined below with record(), not re-measured with a
//   weaker guess. The same boundary reason covers opus:8-mbtn.c7/.c8 and
//   sol:8-button.c6/.c7 -- four claims that chain the identical two facts,
//   naming "open"/"closed" instead of restating "closed" alone.
//
//   Which package could carry the full opus:8-mbtn.c7/.c8,
//   sol:8-button.c6/.c7, opus:8-joy2.c3, sol:8-joy.c19 chain: only a rig
//   spanning `userio` (which models the switch-to-pin polarity --
//   userio.v's `_fire0 = _sjoy1[4] & _mleft`, `_mleft = ~mouse_btn[0]`) and
//   `ciaa` together can supply fact (ii). hrm/ch8-controller-port-io-path
//   already builds exactly that rig and its own P232-1 assertion
//   (opus:8-mbtn.c3) already measures the identical open=1/closed=0 fact
//   these six uids need -- but none of these six uids is in that
//   package's own claims file (ch8-controller-port-io-path.jsonl).
//
// NOT RE-MEASURED: hrm/ch5-audio-filter-bypass already asserts CIA-A PRA
//   bit 1 (the LED/audio-filter bit) on this same fixture. Nothing here
//   names bit 1.
//
// UIDS NAMED BUT NOT IN THIS CLAIMS FILE: ch8-ciaa-port-direction
//   declines opus:8-tbl8-1.c36, sol:8-pin.c40 (p.228 Table 8-1's
//   pin-6 footnote) and opus:8-dio.c42, sol:8-dio.c41 (p.241, fire buttons
//   configurable as outputs) to this package by name. None of the four
//   appear in ch8-ciaapra-fire-buttons.jsonl (whose 10 ids are 8-mbtn.c7,
//   8-mbtn.c8, 8-button.c6, 8-button.c7, 8-joy2.c1, 8-joy2.c2, 8-joy2.c3,
//   8-joy.c17, 8-joy.c18, 8-joy.c19) -- they are not built here.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup -- not a compile-time #ifdef -- so one build serves the real
//   run (MUTATE unset) and every mutation run (MUTATE=<id>, one id per
//   assertion). Each mutation changes what the module is ASKED TO DO (a
//   driven pin level) and leaves the `expect()` call's own expected-value
//   code untouched. Every one below was run with its own id,
//   confirmed red (each mutation reddens exactly and only
//   its own assertion), then reverted (MUTATE unset) and confirmed green
//   again. Baseline with MUTATE unset: 5 passed, 0 failed (A0 plus the
//   four PROP-* lines; all ten claim uids are record()s, not graded).
//
// MUTATION A0-LIVENESS-PORTA-TO-CIAAPRA stimulus red: the second liveness word driven identical to the first instead of its complement, so the two readings are indistinguishable (MUTATE=A0). RUN: baseline 5 passed 0 failed; mutated 4 passed 1 failed, only this label red; reverted 5/0.
// MUTATION PROP-BIT6-PASSTHROUGH stimulus red: porta_in[6] driven to the opposite level in both halves of the check (the "should read 1" half driven low, the "should read 0" half driven high), so neither expected level is met (MUTATE=PROP-BIT6-PASSTHROUGH). RUN: baseline 5 passed 0 failed; mutated 4 passed 1 failed, only this label red; reverted 5/0.
// MUTATION PROP-BIT7-PASSTHROUGH stimulus red: porta_in[7] driven to the opposite level in both halves of the check, symmetric to PROP-BIT6-PASSTHROUGH (MUTATE=PROP-BIT7-PASSTHROUGH). RUN: baseline 5 passed 0 failed; mutated 4 passed 1 failed, only this label red; reverted 5/0.
// MUTATION PROP-BIT6-INDEPENDENT-OF-BIT7 stimulus red: porta_in[6] stuck at 1, ignoring the intended alternating pattern, so a coupling bug would go undetected and the isolation check fails (MUTATE=PROP-BIT6-INDEPENDENT-OF-BIT7). RUN: baseline 5 passed 0 failed; mutated 4 passed 1 failed, only this label red; reverted 5/0.
// MUTATION PROP-BIT7-INDEPENDENT-OF-BIT6 stimulus red: porta_in[7] stuck at 1, ignoring the intended alternating pattern, symmetric to PROP-BIT6-INDEPENDENT-OF-BIT7 (MUTATE=PROP-BIT7-INDEPENDENT-OF-BIT6). RUN: baseline 5 passed 0 failed; mutated 4 passed 1 failed, only this label red; reverted 5/0.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include "Vciaa.h"
#include "verilated.h"

namespace {
const char* g_mutate = nullptr;
inline bool mutating(const char* id) {
    return g_mutate != nullptr && std::strcmp(g_mutate, id) == 0;
}
std::string hex2(uint8_t v) {
    char b[8];
    std::snprintf(b, sizeof b, "%02X", v);
    return std::string(b);
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

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Vciaa* dut = new Vciaa;
    Checks checks;

    int clk7_cycle = 0;
    auto tick = [&]() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        clk7_cycle = (clk7_cycle + 1) % 4;
        dut->clk7_en  = (clk7_cycle == 0);
        dut->clk7n_en = (clk7_cycle == 2);
    };

    auto do_reset = [&]() {
        clk7_cycle = 0;
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->clk7n_en = 0;
        dut->aen = 0;
        dut->rd = 0;
        dut->wr = 0;
        dut->reset = 1;
        dut->rs = 0;
        dut->data_in = 0;
        dut->tick = 0;
        dut->eclk = 0;
        dut->porta_in = 0x3F;   // all six pins "open" (1) at reset -- harmless default
        dut->portb_in = 0xFF;
        dut->kms_level = 0;
        dut->kbd_mouse_type = 0;
        dut->kbd_mouse_data = 0;
        dut->hrtmon_en = 0;
        for (int i = 0; i < 16; i++) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; i++) tick();
    };

    // porta_in is declared `input [7:2] porta_in` in ciaa.v -- a 6-bit
    // range. Verilator normalizes a partial range so bit 0 of the C++
    // value is the range's own LSB (Verilog bit 2) through bit 5 = Verilog
    // bit 7. set_porta_word takes a 6-bit value in that same LSB-aligned
    // convention (bit0=V2 ... bit5=V7) and drives it directly.
    auto set_porta_word = [&](uint8_t w6) {
        dut->porta_in = w6 & 0x3F;
    };
    // set_bit67 drives only the two claimed pins (Verilog bit 6 = C bit 4,
    // Verilog bit 7 = C bit 5) and holds the other four (unclaimed by any
    // uid in this package) at 1.
    auto set_bit67 = [&](bool b6, bool b7) {
        uint8_t w = 0x0F;
        if (b6) w |= (1 << 4);
        if (b7) w |= (1 << 5);
        set_porta_word(w);
    };
    auto settle = [&]() { for (int i = 0; i < 8; i++) tick(); };

    // CIAAPRA read: rs=0 selects pra (ciaa.v:136); the read mux is purely
    // combinational (ciaa.v:251-259), so no clock edge is needed for the
    // read itself -- only porta_in2 (already latched by settle() above)
    // needs to be current.
    auto read_pra = [&]() -> uint8_t {
        dut->aen = 1; dut->wr = 0; dut->rd = 1; dut->rs = 0x0;
        dut->eval();
        uint8_t val = dut->data_out;
        dut->aen = 0; dut->rd = 0;
        dut->eval();
        return val;
    };

    // ========================================================================
    // A0 -- FIXTURE DATA-PATH LIVENESS. No claim id, no manual page: the
    // fixture proves it can deliver a
    // known, distinct word before any read-based assertion is trusted.
    // A known 6-bit pattern and its bitwise complement are driven onto
    // porta_in[7:2] and read back at CIAAPRA's own address; the expected
    // value is derived algebraically from the very word driven (w6 << 2),
    // not from a second hard-coded assumption about bit layout, so this
    // check is also the empirical confirmation of the C-bit <-> Verilog-bit
    // mapping every other assertion below relies on.
    // ========================================================================
    do_reset();
    {
        auto expected_upper6 = [](uint8_t w6) -> uint8_t {
            return (uint8_t)((w6 & 0x3F) << 2);
        };
        const uint8_t word1 = 0b101010;
        // Stimulus mutation (MUTATE=A0): the second word is driven
        // IDENTICAL to the first instead of its complement, so the two
        // readings cannot be told apart and the liveness check fails.
        // Expected values untouched. A control that cannot fail proves
        // nothing.
        const uint8_t word2 = mutating("A0") ? word1
                                             : (uint8_t)0b010101;  // complement

        set_porta_word(word1);
        settle();
        uint8_t v1 = read_pra();
        checks.note("A0-LIVENESS-WORD1: porta_in[7:2] driven to 0b" + std::string("101010") +
                    " reads back CIAAPRA = 0x" + hex2(v1) + " (bits 7:2 = 0x" + hex2(v1 & 0xFC) + ")");

        set_porta_word(word2);
        settle();
        uint8_t v2 = read_pra();
        checks.note("A0-LIVENESS-WORD2: the complement 0b" + std::string("010101") +
                    " reads back CIAAPRA = 0x" + hex2(v2) + " (bits 7:2 = 0x" + hex2(v2 & 0xFC) + ")");

        bool w1ok = (v1 & 0xFC) == expected_upper6(word1);
        bool w2ok = (v2 & 0xFC) == expected_upper6(word2);
        bool distinct = (v1 & 0xFC) != (v2 & 0xFC);
        checks.expect(w1ok && w2ok && distinct,
            "A0-LIVENESS-PORTA-TO-CIAAPRA: a known word driven on porta_in[7:2] and its bitwise complement both "
            "arrive, unmodified and mutually distinguishable, at CIAAPRA bits 7:2 (ciaa.v:227-230 synchronizer, "
            "ciaa.v:253-254 pass-through mux) -- the fixture's data path is live, not stuck");
    }

    // ========================================================================
    // PROP-BIT6-PASSTHROUGH / PROP-BIT7-PASSTHROUGH (see the file header,
    // item (1)). opus:8-mbtn.c7, opus:8-mbtn.c8,
    // sol:8-button.c6 and sol:8-button.c7 (p.232) each chain two facts: (i)
    // the CIAAPRA bit equals its own porta_in pin, and (ii) a mechanically
    // open/closed switch actually drives that pin to the corresponding
    // level. This fixture (ciaa alone) can measure fact (i) only -- there
    // is no switch, connector or polarity logic anywhere in ciaa.v (see
    // DECLINED BY MODULE BOUNDARY in the header, and P4-DECLINED below,
    // which already makes this same argument for opus:8-joy2.c3 /
    // sol:8-joy.c19). Fact (i) is graded below on two uid-free lines,
    // worded as a pin-level pass-through fact rather than a switch-state
    // claim; all four uids are declined via record() beside the line that
    // measures their pin-level half.
    // ========================================================================
    do_reset();
    {
        bool b6_drive_hi = mutating("PROP-BIT6-PASSTHROUGH") ? false : true;  // MUTATION: drive low, expected value (1) untouched
        set_bit67(b6_drive_hi, true);
        settle();
        uint8_t v_hi = read_pra();
        bool bit6_hi = (v_hi >> 6) & 1;

        bool b6_drive_lo = mutating("PROP-BIT6-PASSTHROUGH") ? true : false;  // MUTATION: drive high, expected value (0) untouched
        set_bit67(b6_drive_lo, true);
        settle();
        uint8_t v_lo = read_pra();
        bool bit6_lo = (v_lo >> 6) & 1;

        checks.note("PROP-BIT6-PASSTHROUGH-STIMULUS: porta_in bit6 driven " +
                    std::string(b6_drive_hi ? "HIGH" : "LOW (mutated)") + " gives CIAAPRA=0x" + hex2(v_hi) +
                    "; driven " + std::string(b6_drive_lo ? "HIGH (mutated)" : "LOW") + " gives CIAAPRA=0x" + hex2(v_lo));
        checks.expect(bit6_hi == 1 && bit6_lo == 0,
            "PROP-BIT6-PASSTHROUGH (pin-level fact only, uid-free -- see record() declines below): CIAAPRA "
            "bit 6 reads back exactly the logic level driven onto porta_in[6], both directions (ciaa.v:253-254 "
            "pass-through via the porta_in2 synchronizer, ciaa.v:227-230) -- no switch-state or "
            "connector-wiring claim is graded here");

        checks.record("opus:8-mbtn.c7 (p.232, quote '...A logic state of 1 means \"switch open.\"...'): the "
            "pin-level half of this claim -- CIAAPRA bit 6 equals porta_in[6] -- is measured above by "
            "PROP-BIT6-PASSTHROUGH. The claim itself names the SWITCH's state (\"switch open\"), not the "
            "pin's, and additionally needs a mechanically-open fire-button switch to actually drive "
            "porta_in[6] high -- a fact about the controller-port-connector-to-chip wiring, not about ciaa "
            "itself (ciaa.v has no switch, connector or polarity logic of its own; see the grep evidence in "
            "P4-DECLINED below). Declined, not re-measured with a weaker guess -- the identical boundary "
            "reason this package already uses for opus:8-joy2.c3/sol:8-joy.c19.");
        checks.record("opus:8-mbtn.c8 (p.232, quote '...A logic state of 0 means \"switch closed.\"...'): "
            "same boundary reason as opus:8-mbtn.c7 above -- PROP-BIT6-PASSTHROUGH measures the pin-level "
            "half only; a mechanically-closed switch driving porta_in[6] low is outside ciaa's boundary.");
    }

    do_reset();
    {
        bool b7_drive_hi = mutating("PROP-BIT7-PASSTHROUGH") ? false : true;  // MUTATION: drive low, expected value (1) untouched
        set_bit67(true, b7_drive_hi);
        settle();
        uint8_t v_hi = read_pra();
        bool bit7_hi = (v_hi >> 7) & 1;

        bool b7_drive_lo = mutating("PROP-BIT7-PASSTHROUGH") ? true : false;  // MUTATION: drive high, expected value (0) untouched
        set_bit67(true, b7_drive_lo);
        settle();
        uint8_t v_lo = read_pra();
        bool bit7_lo = (v_lo >> 7) & 1;

        checks.note("PROP-BIT7-PASSTHROUGH-STIMULUS: porta_in bit7 driven " +
                    std::string(b7_drive_hi ? "HIGH" : "LOW (mutated)") + " gives CIAAPRA=0x" + hex2(v_hi) +
                    "; driven " + std::string(b7_drive_lo ? "HIGH (mutated)" : "LOW") + " gives CIAAPRA=0x" + hex2(v_lo));
        checks.expect(bit7_hi == 1 && bit7_lo == 0,
            "PROP-BIT7-PASSTHROUGH (pin-level fact only, uid-free -- see record() declines below; twin "
            "measurement of PROP-BIT6-PASSTHROUGH, on bit 7 instead of bit 6 so the two together cover both "
            "claimed bits rather than repeating one): CIAAPRA bit 7 reads back exactly the logic level "
            "driven onto porta_in[7], both directions -- same pass-through path as PROP-BIT6-PASSTHROUGH, no "
            "switch-state or connector-wiring claim is graded here");

        checks.record("sol:8-button.c6 (p.232, quote 'A logic state of 1 means \"switch open.\"'): twin of "
            "opus:8-mbtn.c7, measured on bit 7 via PROP-BIT7-PASSTHROUGH instead of bit 6 -- same boundary "
            "reason: the pin-level half is measured, the switch-to-pin wiring half is outside ciaa's "
            "boundary. Declined, not re-measured with a weaker guess.");
        checks.record("sol:8-button.c7 (p.232, quote 'A logic state of 0 means \"switch closed.\"'): twin of "
            "opus:8-mbtn.c8, same reasoning as sol:8-button.c6 above.");
    }

    // ========================================================================
    // PROP-BIT6-INDEPENDENT-OF-BIT7 / PROP-BIT7-INDEPENDENT-OF-BIT6 (see
    // the file header, item (2)). p.233's
    // KNOWN CONFLICT 3: the undisputed core -- bit 6 and bit 7 each track
    // only their own pin, whichever decimal port label either page
    // attaches to them -- is graded on these two uid-free lines. A
    // 4-combination matrix drives every (bit6,bit7) pairing; each
    // assertion checks its own bit against the matrix while the OTHER bit
    // is varied underneath it, so a coupling bug (e.g. a crossed wire)
    // would show up as a mismatch even though the disputed numeral is
    // never graded.
    // ========================================================================
    checks.note("KNOWN-CONFLICT-3: p.233 numbers the two controller ports 0 and 1; p.232 and the rest of the "
                "chapter number the same two ports 1 and 2. PROP-BIT6-INDEPENDENT-OF-BIT7 and "
                "PROP-BIT7-INDEPENDENT-OF-BIT6 below grade only that the two CIAAPRA bits are independent of "
                "each other; neither grades which decimal label attaches to which bit -- see the record() "
                "declines beside each for why the four page-233 numeral uids stay off these graded lines.");

    auto measure_bit6_independent = [&](const char* mutate_id) -> bool {
        const bool combo_b6[4] = {true, true, false, false};
        const bool combo_b7[4] = {false, true, true, false};
        bool ok = true;
        for (int i = 0; i < 4; i++) {
            bool b6_intended = combo_b6[i];
            bool b6_drive = mutating(mutate_id) ? true : b6_intended;  // MUTATION: bit6 stuck at 1
            set_bit67(b6_drive, combo_b7[i]);
            settle();
            uint8_t v = read_pra();
            bool bit6 = (v >> 6) & 1;
            if (bit6 != b6_intended) ok = false;
        }
        return ok;
    };
    auto measure_bit7_independent = [&](const char* mutate_id) -> bool {
        const bool combo_b7[4] = {true, true, false, false};
        const bool combo_b6[4] = {false, true, true, false};
        bool ok = true;
        for (int i = 0; i < 4; i++) {
            bool b7_intended = combo_b7[i];
            bool b7_drive = mutating(mutate_id) ? true : b7_intended;  // MUTATION: bit7 stuck at 1
            set_bit67(combo_b6[i], b7_drive);
            settle();
            uint8_t v = read_pra();
            bool bit7 = (v >> 7) & 1;
            if (bit7 != b7_intended) ok = false;
        }
        return ok;
    };

    do_reset();
    bool bit6_indep = measure_bit6_independent("PROP-BIT6-INDEPENDENT-OF-BIT7");
    checks.expect(bit6_indep,
        "PROP-BIT6-INDEPENDENT-OF-BIT7 (undisputed core of KNOWN CONFLICT 3, uid-free -- see record() "
        "declines below): CIAAPRA bit 6 tracks only its own porta_in pin across every combination of the "
        "other port's pin, regardless of which decimal label either port carries");
    checks.record("opus:8-joy2.c1 (p.233, quote 'The fire buttons for ports 0 and 1 are connected to bits 6 "
        "and 7 of CIAAPRA ($BFE001).', restated as 'The fire button of port 0 is connected to bit 6 of "
        "CIAAPRA.'): the bit-6/bit-7 distinctness is measured by PROP-BIT6-INDEPENDENT-OF-BIT7 above, but "
        "this claim's own restatement names the numeral \"port 0\", and p.233 numbers the two controller "
        "ports 0 and 1 where p.232 and the rest of the chapter number the same two ports 1 and 2 -- no RTL "
        "measurement settles a disputed page's own numbering. Not fully exercised by the graded line; "
        "conflict-recorded, not asserted.");
    checks.record("sol:8-joy.c17 (p.233, restated as 'Controller port 0's fire button is connected to "
        "CIAAPRA bit 6.'): twin of opus:8-joy2.c1, same p.232-versus-p.233 numeral conflict.");

    do_reset();
    bool bit7_indep = measure_bit7_independent("PROP-BIT7-INDEPENDENT-OF-BIT6");
    checks.expect(bit7_indep,
        "PROP-BIT7-INDEPENDENT-OF-BIT6 (undisputed core of KNOWN CONFLICT 3, uid-free -- see record() "
        "declines below): CIAAPRA bit 7 tracks only its own porta_in pin across every combination of the "
        "other port's pin, regardless of which decimal label either port carries");
    checks.record("opus:8-joy2.c2 (p.233, restated as 'The fire button of port 1 is connected to bit 7 of "
        "CIAAPRA.'): same p.232-versus-p.233 numeral conflict as opus:8-joy2.c1 above, measured instead by "
        "PROP-BIT7-INDEPENDENT-OF-BIT6 -- the numeral \"port 1\" is not settled by any RTL measurement.");
    checks.record("sol:8-joy.c18 (p.233, restated as 'Controller port 1's fire button is connected to "
        "CIAAPRA bit 7.'): twin of opus:8-joy2.c2, same reasoning.");

    // ========================================================================
    // P4-DECLINED -- opus:8-joy2.c3, sol:8-joy.c19 (p.233): see header.
    // ========================================================================
    checks.record(
        "P4-DECLINED (p.233, opus:8-joy2.c3 sol:8-joy.c19, quote 'A 0 here indicates the switch is closed.'): not "
        "asserted at this boundary -- the full chain needs two facts, only one of which ciaa can supply. "
        "ciaa.v:253-254 (via the porta_in2 synchronizer, ciaa.v:227-230) shows CIAAPRA bits 7:2 are an unmodified "
        "copy of porta_in[7:2] -- confirmed above by PROP-BIT6/7-PASSTHROUGH's pass-through measurements. But "
        "whether a mechanically-closed fire-button switch actually drives its porta_in pin LOW is a fact about "
        "the wiring between the controller-port connector and the chip, not about ciaa itself -- ciaa.v has no "
        "switch, no connector and no polarity logic of its own (grep over this fixture's five source files for "
        "\"switch\", \"joy\" or \"button\": zero hits in any of the five modules' executable bodies -- the sole "
        "hit anywhere is ciaa.v:71, a pre-module changelog comment reading \"2010-08-15 - added joystick "
        "emulation\", which names no signal and sits before the module keyword). That wiring lives outside this "
        "fixture's boundary (ciaa compiled alone), the same kind of boundary argument that routes CIAAPRA "
        "address claims to ch8-ciaapra-address-window. Declined, not re-measured with a weaker guess.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
