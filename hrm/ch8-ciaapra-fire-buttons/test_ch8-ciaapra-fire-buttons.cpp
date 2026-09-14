// Test purpose: HRM conformance package for printed pages 232-233's CIAAPRA
//   controller-port fire-button bits (6 and 7), against the `ciaa` module
//   compiled alone. 10 claims, from
//   work/ch8-s1-build/ch8-ciaapra-fire-buttons.jsonl. Full derivation in
//   SPECIFICATION.md (committed as 9460ba1 before this file was written).
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
//   files, in which order) was read from hrm/ch8-ciaa-port-direction's
//   Makefile, per the build brief; nothing else in that sibling directory
//   was opened. This file's own harness/mutation boilerplate (the Checks
//   class, the MUTATE env-var convention, the summary-line format) was
//   read from hrm/ch8-appb-chip-column-paula/test_paula.cpp, an unrelated
//   fixture (paula, not ciaa), to match this suite's existing convention --
//   no ciaa-specific fact came from that file.
//
// RTL FACT USED THROUGHOUT (ciaa.v, read after SPECIFICATION.md was
//   committed): the address decoder gives CIAAPRA at rs=4'h0 (ciaa.v:136,
//   "pra = enable && rs==4'h0"). Reading it combinationally drives
//   pa_out[7:0] = {porta_in2[7:2], porta_out[1:0]} (ciaa.v:253-254), where
//   porta_in2[7:2] is porta_in[7:2] synchronized one clk7_en period earlier
//   (ciaa.v:227-230). Bits 7:2 are pass-through from the porta_in pins,
//   completely unmodified and ungated by DDRA -- there is no inverter and
//   no coupling between bit 6 and bit 7 anywhere in this path.
//
// KNOWN CONFLICT 3 (port numbering, carried forward from the build brief).
//   p.233 numbers the controller ports 0 and 1; p.232 and the rest of the
//   chapter number the same two ports 1 and 2. opus:8-joy2.c1/c2 and
//   sol:8-joy.c17/c18 each name a specific decimal port label for a
//   specific bit. What is NOT disputed between the pages: there are
//   exactly two ports, each on its own CIAAPRA bit (6 or 7), and neither
//   page ever suggests the two bits interact. This package grades exactly
//   that undisputed core -- CIAAPRA bit 6 (resp. 7) responds only to its
//   own porta_in pin, never to the other port's -- for all four uids, and
//   never grades which decimal label attaches to which bit.
//
// DECLINED BY MODULE BOUNDARY: opus:8-joy2.c3 / sol:8-joy.c19 ("a 0 in the
//   fire-button bit means the switch is closed") chain two facts: (i) the
//   CIAAPRA bit equals its pin (proven below, A0/A1-A4), and (ii) a
//   mechanically-closed switch drives that pin low. Fact (ii) is a
//   property of the controller-port-to-chip wiring, not of ciaa itself --
//   ciaa.v has no switch, no connector and no polarity logic; grep over
//   ciaa.v, cia_int.v, cia_timera.v, cia_timerb.v, cia_timerd.v for
//   "switch" or "joy" or "button": zero hits. This fixture (ciaa alone)
//   cannot settle fact (ii), the same kind of boundary argument that
//   routes CIAAPRA *address* claims to ch8-ciaapra-address-window (a rig
//   over ciaa+gary). Declined below with record(), not re-measured with a
//   weaker guess.
//
// NOT RE-MEASURED: hrm/ch5-audio-filter-bypass already asserts CIA-A PRA
//   bit 1 (the LED/audio-filter bit) on this same fixture. Nothing here
//   names bit 1.
//
// UIDS NAMED BUT NOT IN THIS CLAIMS FILE: ch8-ciaa-port-direction's own
//   build declined opus:8-tbl8-1.c36, sol:8-pin.c40 (p.228 Table 8-1's
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
//   code untouched. Every one below was run with its own id, confirmed red
//   (each mutation reddens exactly and only its own assertion -- 8 passed,
//   1 failed in every one of the 8 runs, never more), then reverted
//   (MUTATE unset) and confirmed green again. Baseline with MUTATE unset:
//   9 passed, 0 failed (A0 plus A1-A8; P4 is a record(), not graded).
//
// MUTATION A0-LIVENESS-PORTA-TO-CIAAPRA stimulus red: the second liveness word driven identical to the first instead of its complement, so the two readings are indistinguishable (MUTATE=A0). RUN: baseline 9 passed 0 failed; mutated 8 passed 1 failed, only this label red; reverted 9/0.
// MUTATION A1-BIT6-READS-1-WHEN-OPEN stimulus red: porta_in[6] driven low while the assertion still expects "open" (MUTATE=A1)
// MUTATION A2-BIT6-READS-0-WHEN-CLOSED stimulus red: porta_in[6] driven high while the assertion still expects "closed" (MUTATE=A2)
// MUTATION A3-BIT7-READS-1-WHEN-OPEN stimulus red: porta_in[7] driven low while the assertion still expects "open" (MUTATE=A3)
// MUTATION A4-BIT7-READS-0-WHEN-CLOSED stimulus red: porta_in[7] driven high while the assertion still expects "closed" (MUTATE=A4)
// MUTATION A5-BIT6-INDEPENDENT-OF-BIT7-OPUS stimulus red: porta_in[6] stuck at 1, ignoring the intended alternating pattern (MUTATE=A5)
// MUTATION A6-BIT6-INDEPENDENT-OF-BIT7-SOL stimulus red: porta_in[6] stuck at 1, ignoring the intended alternating pattern (MUTATE=A6)
// MUTATION A7-BIT7-INDEPENDENT-OF-BIT6-OPUS stimulus red: porta_in[7] stuck at 1, ignoring the intended alternating pattern (MUTATE=A7)
// MUTATION A8-BIT7-INDEPENDENT-OF-BIT6-SOL stimulus red: porta_in[7] stuck at 1, ignoring the intended alternating pattern (MUTATE=A8)

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
    // build brief's own requirement that the fixture prove it can deliver a
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
        // nothing, and this one carried no mutation at all.
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
    // A1/A2 -- opus:8-mbtn.c7, opus:8-mbtn.c8 (p.232): a CIAAPRA button bit
    // reads 1 when its pin is driven to the manual's "open" level and 0 when
    // driven to its "closed" level. Graded on bit 6.
    // ========================================================================
    do_reset();
    {
        bool b6_drive = mutating("A1") ? false : true;  // MUTATION: drive low, still call it "open"
        set_bit67(b6_drive, true);
        settle();
        uint8_t v = read_pra();
        bool bit6 = (v >> 6) & 1;
        checks.note("A1-STIMULUS: porta_in bit6 driven " + std::string(b6_drive ? "HIGH (open)" : "LOW (mutated)") +
                    ", CIAAPRA reads 0x" + hex2(v));
        checks.expect(bit6 == 1,
            "A1-BIT6-READS-1-WHEN-OPEN (p.232, opus:8-mbtn.c7, quote '...A logic state of 1 means \"switch "
            "open.\"...'): CIAAPRA bit 6, with porta_in[6] driven to the manual's own \"open\" level (1), reads "
            "back 1 (ciaa.v:253-254 pass-through via the porta_in2 synchronizer, ciaa.v:227-230)");
    }
    do_reset();
    {
        bool b6_drive = mutating("A2") ? true : false;  // MUTATION: drive high, still call it "closed"
        set_bit67(b6_drive, true);
        settle();
        uint8_t v = read_pra();
        bool bit6 = (v >> 6) & 1;
        checks.note("A2-STIMULUS: porta_in bit6 driven " + std::string(b6_drive ? "HIGH (mutated)" : "LOW (closed)") +
                    ", CIAAPRA reads 0x" + hex2(v));
        checks.expect(bit6 == 0,
            "A2-BIT6-READS-0-WHEN-CLOSED (p.232, opus:8-mbtn.c8, quote '...A logic state of 0 means \"switch "
            "closed.\"...'): CIAAPRA bit 6, with porta_in[6] driven to the manual's own \"closed\" level (0), "
            "reads back 0 (same pass-through path as A1)");
    }

    // ========================================================================
    // A3/A4 -- sol:8-button.c6, sol:8-button.c7 (p.232): twins of A1/A2,
    // graded independently under their own uids, and deliberately measured
    // on the OTHER bit (7) rather than replaying A1/A2's own bit, so the
    // twin pair also broadens coverage instead of repeating one bit twice.
    // ========================================================================
    do_reset();
    {
        bool b7_drive = mutating("A3") ? false : true;  // MUTATION: drive low, still call it "open"
        set_bit67(true, b7_drive);
        settle();
        uint8_t v = read_pra();
        bool bit7 = (v >> 7) & 1;
        checks.note("A3-STIMULUS: porta_in bit7 driven " + std::string(b7_drive ? "HIGH (open)" : "LOW (mutated)") +
                    ", CIAAPRA reads 0x" + hex2(v));
        checks.expect(bit7 == 1,
            "A3-BIT7-READS-1-WHEN-OPEN (p.232, sol:8-button.c6, quote 'A logic state of 1 means \"switch "
            "open.\"'): CIAAPRA bit 7, with porta_in[7] driven to the manual's own \"open\" level (1), reads back "
            "1 -- same pass-through path as A1, twin uid graded independently");
    }
    do_reset();
    {
        bool b7_drive = mutating("A4") ? true : false;  // MUTATION: drive high, still call it "closed"
        set_bit67(true, b7_drive);
        settle();
        uint8_t v = read_pra();
        bool bit7 = (v >> 7) & 1;
        checks.note("A4-STIMULUS: porta_in bit7 driven " + std::string(b7_drive ? "HIGH (mutated)" : "LOW (closed)") +
                    ", CIAAPRA reads 0x" + hex2(v));
        checks.expect(bit7 == 0,
            "A4-BIT7-READS-0-WHEN-CLOSED (p.232, sol:8-button.c7, quote 'A logic state of 0 means \"switch "
            "closed.\"'): CIAAPRA bit 7, with porta_in[7] driven to the manual's own \"closed\" level (0), reads "
            "back 0 -- twin uid of A2, graded independently");
    }

    // ========================================================================
    // A5-A8 -- opus:8-joy2.c1/c2, sol:8-joy.c17/c18 (p.233, KNOWN CONFLICT
    // 3): the undisputed core of the port-numbering conflict -- bit 6 and
    // bit 7 each track only their own pin, never the other's, whichever
    // decimal port label either page attaches to them. A 4-combination
    // matrix drives every (bit6,bit7) pairing; each assertion checks its
    // own bit against the matrix while the OTHER bit is varied underneath
    // it, so a coupling bug (e.g. a crossed wire) would show up as a
    // mismatch even though this package never grades which port is "0",
    // "1" or "2".
    // ========================================================================
    checks.note("KNOWN-CONFLICT-3: p.233 numbers the two controller ports 0 and 1; p.232 and the rest of the "
                "chapter number the same two ports 1 and 2. A5-A8 grade only that the two CIAAPRA bits are "
                "independent of each other; no assertion below grades which decimal label attaches to which bit.");

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
    bool a5 = measure_bit6_independent("A5");
    checks.expect(a5,
        "A5-BIT6-INDEPENDENT-OF-BIT7-OPUS (p.233, opus:8-joy2.c1, quote 'The fire buttons for ports 0 and 1 are "
        "connected to bits 6 and 7 of CIAAPRA ($BFE001).' -- KNOWN CONFLICT 3, port-number label dropped, "
        "undisputed core graded): CIAAPRA bit 6 tracks only its own porta_in pin across every combination of the "
        "other port's pin, regardless of which decimal label either port carries");

    do_reset();
    bool a6 = measure_bit6_independent("A6");
    checks.expect(a6,
        "A6-BIT6-INDEPENDENT-OF-BIT7-SOL (p.233, sol:8-joy.c17, quote 'The fire buttons for ports 0 and 1 are "
        "connected to bits 6 and 7 of CIAAPRA ($BFE001).' -- KNOWN CONFLICT 3, port-number label dropped, "
        "undisputed core graded): twin of A5, graded independently under its own uid");

    do_reset();
    bool a7 = measure_bit7_independent("A7");
    checks.expect(a7,
        "A7-BIT7-INDEPENDENT-OF-BIT6-OPUS (p.233, opus:8-joy2.c2, quote 'The fire buttons for ports 0 and 1 are "
        "connected to bits 6 and 7 of CIAAPRA ($BFE001).' -- KNOWN CONFLICT 3, port-number label dropped, "
        "undisputed core graded): CIAAPRA bit 7 tracks only its own porta_in pin across every combination of the "
        "other port's pin, regardless of which decimal label either port carries");

    do_reset();
    bool a8 = measure_bit7_independent("A8");
    checks.expect(a8,
        "A8-BIT7-INDEPENDENT-OF-BIT6-SOL (p.233, sol:8-joy.c18, quote 'The fire buttons for ports 0 and 1 are "
        "connected to bits 6 and 7 of CIAAPRA ($BFE001).' -- KNOWN CONFLICT 3, port-number label dropped, "
        "undisputed core graded): twin of A7, graded independently under its own uid");

    // ========================================================================
    // P4-DECLINED -- opus:8-joy2.c3, sol:8-joy.c19 (p.233): see header.
    // ========================================================================
    checks.record(
        "P4-DECLINED (p.233, opus:8-joy2.c3 sol:8-joy.c19, quote 'A 0 here indicates the switch is closed.'): not "
        "asserted at this boundary -- the full chain needs two facts, only one of which ciaa can supply. "
        "ciaa.v:253-254 (via the porta_in2 synchronizer, ciaa.v:227-230) shows CIAAPRA bits 7:2 are an unmodified "
        "copy of porta_in[7:2] -- confirmed above by A0-A4's pass-through measurements. But whether a "
        "mechanically-closed fire-button switch actually drives its porta_in pin LOW is a fact about the wiring "
        "between the controller-port connector and the chip, not about ciaa itself -- ciaa.v has no switch, no "
        "connector and no polarity logic of its own (grep over this fixture's five source files for \"switch\", "
        "\"joy\" or \"button\": zero hits in any of the five modules' executable bodies -- the sole hit anywhere "
        "is ciaa.v:71, a pre-module changelog comment reading \"2010-08-15 - added joystick emulation\", which "
        "names no signal and sits before the module keyword). That wiring lives outside this fixture's boundary "
        "(ciaa compiled alone), the same kind of boundary argument that routes CIAAPRA address claims to "
        "ch8-ciaapra-address-window. Declined, not re-measured with a weaker guess.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
