// Test purpose: HRM conformance package for the printed-page claim that
//   CIAAPRA answers at CPU bus address $BFE001 (pp.229, 232), against a rig
//   over `ciaa` + `gary` (ciaapra_address_rig.v, this package's own
//   wrapper -- not pinned RTL). 4 claims, from
//   work/ch8-s1-build/ch8-ciaapra-address-window.jsonl. Full derivation in
//   SPECIFICATION.md (committed as 09a9ba3, before any RTL was opened).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed p.229
//   ("CIAAPRA ($BFE001) Input and output for pin 6 (port 1 & 2 fire
//   buttons)") and p.232 ("The left button on the Amiga mouse is connected
//   to CIAAPRA ($BFE001). Port 1 uses bit 6 and port 2 uses bit 7.").
//
// Fixture boundary and why it is wider than `ciaa` alone: see
//   SPECIFICATION.md, written and committed before ciaa.v, gary.v or
//   minimig.v was opened. In short -- ciaa.v exposes only an internal
//   register-select input (`rs`) and an address-enable pin (`aen`); it has
//   no CPU address bus and cannot itself be asked "does $BFE001 select
//   you." That decode is gary's: gary.v:190-192 (read after the
//   specification commit) --
//     assign sel_cia   = cpu_address_in[23:16]==8'hBF;
//     assign sel_cia_a = sel_cia & ~cpu_address_in[12];
//     assign sel_cia_b = sel_cia & ~cpu_address_in[13];
//   -- computed directly from gary's own `cpu_address_in` port, with no
//   third module pre-resolving any part of the address first. So a
//   ciaa+gary rig reaches the literal value $BFE001; no wider boundary was
//   needed, and none was added.
//
// RTL READ, IN ORDER, ALL AFTER SPECIFICATION.md WAS COMMITTED (09a9ba3):
//   1. hrm/ch8-ciaa-port-direction/Makefile -- build pattern (ciaa.v needs
//      cia_int.v, cia_timera.v, cia_timerb.v, cia_timerd.v to elaborate),
//      named in the build brief.
//   2. $(NANOMIG)/src/minimig-aga/gary.v -- full file.
//   3. $(NANOMIG)/src/minimig-aga/ciaa.v -- full file.
//   4. $(NANOMIG)/src/minimig-aga/minimig.v -- lines 600-710 and 760-830
//      (the ciaa and gary instantiations: ciaa at 607-630, gary at
//      762-821), for the wiring this package's own rig copies. Nothing
//      else in minimig.v was read.
//   5. hrm/ch8-ciaapra-fire-buttons/test_ch8-ciaapra-address-window.cpp
//      [sic: hrm/ch8-ciaapra-fire-buttons/test_ch8-ciaapra-fire-buttons.cpp]
//      -- full file, to match this suite's established harness/mutation
//      boilerplate (the Checks class, the MUTATE env-var convention, the
//      settle()/tick()/do_reset() clocking pattern, the summary-line
//      format). That file's own header discloses it took the same
//      boilerplate from hrm/ch8-appb-chip-column-paula/test_paula.cpp, an
//      unrelated fixture. No ciaa+gary-specific fact used below came from
//      reading it; only the surrounding harness shape did. That sibling
//      file's own header also states it is what routes CIAAPRA address
//      claims to this package.
//   cia_int.v, cia_timera.v, cia_timerb.v, cia_timerd.v were NOT read.
//   ciaa.v instantiates them for the interrupt controller and the three
//   timers, none of which this package's assertions touch; the sibling
//   ch8-ciaapra-fire-buttons package already reads ciaa's data_out (an
//   OR-reduction across icr_out/tmra_out/tmrb_out/tmrd_out/sdr_out/pb_out/
//   pa_out, ciaa.v:155) through the very same mechanism this file uses and
//   got clean results matching the manual with no contribution from those
//   four modules leaking through -- taken as corroboration, not reread
//   independently.
//
// FIXTURE ADDRESS ARITHMETIC. gary's cpu_address_in (and this rig's own
//   cpu_address port) is declared [23:1] -- the m68k's real A23..A1 lines,
//   A0 not present on this bus (byte lanes are cpu_hwr/cpu_lwr instead).
//   So driving this port with a plain byte address X uses X>>1 (bit 0 is
//   simply not represented): $BFE001>>1 = 0x5FF000, $BFE201>>1 = 0x5FF100,
//   $BFD001>>1 = 0x5FE800, $000000>>1 = 0x000000. Verified against gary.v's
//   own decode: 0x5FF000's real address bits [23:16]=$BF, [12]=0 (sel_cia_a
//   path), [13]=1 (sel_cia_b stays clear); 0x5FF000's bits [11:8]=0 match
//   ciaa.v:136's rs==4'h0 (PRA); 0x5FF100's bits [11:8]=2 match ciaa.v:138's
//   rs==4'h2 (DDRA).
//
// UIDS NAMED BUT NOT IN THIS CLAIMS FILE: the build brief names
//   opus:8-dio.c43 ("CIAADDRA is at $BFE201"), opus:8-dio.c45 ("CIAAPRA
//   ($BFE001) is the data read register") and the address half of
//   sol:8-dio.c42 as address claims two sibling builders declined to this
//   package by name. Checked: none of the three appear in
//   ch8-ciaapra-address-window.jsonl, whose four ids are opus:8-regs.c5,
//   sol:8-reg.c5, opus:8-mbtn.c4, sol:8-button.c3. Not built here; not
//   invented.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup -- not a compile-time #ifdef -- so one build serves the real
//   run (MUTATE unset) and every mutation run (MUTATE=<id>, one id per
//   cited assertion). A1-A4 all grade the identical measured fact ($BFE001
//   selects PRA) from four separate citing passages; each one's own
//   mutation independently redirects THAT assertion's read address to
//   $BFE201 (DDRA, still in the same $BFE0xx CIA-A window) while leaving
//   the expected value (PRA's pattern) untouched -- asking the rig a
//   different question without touching the oracle. A0 (fixture-liveness),
//   PRE, PRE-NEG and CTRL carry no manual citation and are not mutated,
//   matching the precedent in ch8-ciaapra-fire-buttons (its own A0 is
//   likewise unmutated). Baseline with MUTATE unset: 8 passed, 0 failed.
//   Each of MUTATE=A1..A4 was run and confirmed 7 passed, 1 failed (only
//   the targeted assertion reddens), then reverted and confirmed back to
//   8 passed, 0 failed.
//
// MUTATION A0-LIVENESS-PORTA-TO-CIAAPRA-VIA-RIG stimulus red: second liveness word driven identical to the first, so the two readings are indistinguishable (MUTATE=A0). RUN: baseline 8 passed 0 failed -> 7 passed 1 failed, only this label red; reverted 8/0.
// MUTATION PRE-AEN-ASSERTED-AT-BFE001 stimulus red: a non-CIA address presented while the check still expects sel_cia_a asserted (MUTATE=PRE). RUN: baseline 8 passed 0 failed -> 7 passed 1 failed, only this label red; reverted 8/0.
// MUTATION PRE-NEG-AEN-DEASSERTED-OUTSIDE-CIA-WINDOW stimulus red: the CIA address itself presented, so the decode asserts and the isolation control fails (MUTATE=PRE-NEG). RUN: baseline 8 passed 0 failed -> 7 passed 1 failed, only this label red; reverted 8/0.
// MUTATION CTRL-DDRA-DISTINCT-FROM-PRA stimulus red: 0x00 written to DDRA while the read-back is still compared against 0xA5 (MUTATE=CTRL). RUN: baseline 8 passed 0 failed -> 7 passed 1 failed, only this label red; reverted 8/0.
// MUTATION A1-BFE001-SELECTS-PRA-OPUS-REGS stimulus red: read address redirected from $BFE001 to $BFE201 (DDRA) while the check still expects PRA's pattern (MUTATE=A1)
// MUTATION A2-BFE001-SELECTS-PRA-SOL-REG stimulus red: read address redirected from $BFE001 to $BFE201 (DDRA) while the check still expects PRA's pattern (MUTATE=A2)
// MUTATION A3-BFE001-SELECTS-PRA-OPUS-MBTN stimulus red: read address redirected from $BFE001 to $BFE201 (DDRA) while the check still expects PRA's pattern (MUTATE=A3)
// MUTATION A4-BFE001-SELECTS-PRA-SOL-BUTTON stimulus red: read address redirected from $BFE001 to $BFE201 (DDRA) while the check still expects PRA's pattern (MUTATE=A4)

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include "Vciaapra_address_rig.h"
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
    Vciaapra_address_rig* dut = new Vciaapra_address_rig;
    Checks checks;

    // Byte addresses, converted to this rig's [23:1] address port by
    // dropping bit 0 (see FIXTURE ADDRESS ARITHMETIC above).
    const uint32_t ADDR_BFE001_PRA  = 0x5FF000;  // CIA-A, rs=0 (PRA)
    const uint32_t ADDR_BFE201_DDRA = 0x5FF100;  // CIA-A, rs=2 (DDRA), same $BFE0xx window
    const uint32_t ADDR_BFD001_CIAB = 0x5FE800;  // CIA-B window, not CIA-A
    const uint32_t ADDR_NONCIA      = 0x000000;  // chip RAM, outside $BFxxxx entirely

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
    auto settle = [&]() { for (int i = 0; i < 8; i++) tick(); };

    auto do_reset = [&]() {
        clk7_cycle = 0;
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->clk7n_en = 0;
        dut->reset = 1;
        dut->cpu_address = 0;
        dut->cpu_rd = 0;
        dut->cpu_hwr = 0;
        dut->cpu_lwr = 0;
        dut->cpu_data_out = 0;
        dut->porta_in = 0x3F;   // all six pins "open" (1) at reset -- harmless default
        dut->portb_in = 0xFF;
        dut->tick = 0;
        dut->eclk = 0;
        dut->kms_level = 0;
        dut->kbd_mouse_type = 0;
        dut->kbd_mouse_data = 0;
        dut->hrtmon_en = 0;
        for (int i = 0; i < 16; i++) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; i++) tick();
    };

    // porta_in is [7:2] on ciaa (6 bits); this rig passes it straight
    // through. bit0(C)=V2 ... bit5(C)=V7, same convention already
    // established (and re-derived, not assumed) in
    // ch8-ciaapra-fire-buttons.
    auto set_porta_word = [&](uint8_t w6) { dut->porta_in = w6 & 0x3F; };

    // Present an address to the rig and let gary's (combinational) decode
    // settle before ciaa's own (combinational) read mux is sampled.
    auto set_address = [&](uint32_t addr23) {
        dut->cpu_address = addr23 & 0x7FFFFF;
        dut->eval();
    };

    auto cia_read = [&](uint32_t addr23) -> uint8_t {
        set_address(addr23);
        dut->cpu_rd = 1; dut->cpu_hwr = 0; dut->cpu_lwr = 0;
        dut->eval();
        uint8_t val = dut->cia_data_out;
        dut->cpu_rd = 0;
        dut->eval();
        return val;
    };

    // A write needs the address and wr strobe held across a clk7_en edge
    // (ddrporta/regporta are clocked, ciaa.v:242-248).
    auto cia_write = [&](uint32_t addr23, uint8_t byte) {
        dut->cpu_address = addr23 & 0x7FFFFF;
        dut->cpu_data_out = byte;
        dut->cpu_lwr = 1; dut->cpu_hwr = 0; dut->cpu_rd = 0;
        settle();
        dut->cpu_lwr = 0;
        dut->eval();
    };

    // ========================================================================
    // A0 -- FIXTURE DATA-PATH LIVENESS. No claim id, no manual page: the
    // build brief's own requirement that the fixture prove it can deliver a
    // known, distinct word before any read-based assertion is trusted --
    // through THIS rig's own address-driven read (gary's decode into
    // ciaa's aen/rs), not a hand-set rs.
    // ========================================================================
    do_reset();
    {
        auto expected_upper6 = [](uint8_t w6) -> uint8_t {
            return (uint8_t)((w6 & 0x3F) << 2);
        };
        const uint8_t word1 = 0b101010;
        // MUTATE=A0: drive the second word IDENTICAL to the first, so the
        // two readings cannot be told apart and `distinct` is false.
        const uint8_t word2 = mutating("A0") ? word1
                                             : (uint8_t)0b010101;  // complement

        set_porta_word(word1);
        settle();
        uint8_t v1 = cia_read(ADDR_BFE001_PRA);
        checks.note("A0-LIVENESS-WORD1: porta_in[7:2]=0b101010, read through the rig at $BFE001, gives CIAAPRA=0x" +
                    hex2(v1) + " (bits 7:2=0x" + hex2(v1 & 0xFC) + ")");

        set_porta_word(word2);
        settle();
        uint8_t v2 = cia_read(ADDR_BFE001_PRA);
        checks.note("A0-LIVENESS-WORD2: the complement 0b010101, read the same way, gives CIAAPRA=0x" +
                    hex2(v2) + " (bits 7:2=0x" + hex2(v2 & 0xFC) + ")");

        bool w1ok = (v1 & 0xFC) == expected_upper6(word1);
        bool w2ok = (v2 & 0xFC) == expected_upper6(word2);
        bool distinct = (v1 & 0xFC) != (v2 & 0xFC);
        checks.expect(w1ok && w2ok && distinct,
            "A0-LIVENESS-PORTA-TO-CIAAPRA-VIA-RIG: a known word driven on porta_in[7:2] and its bitwise complement "
            "both arrive, unmodified and mutually distinguishable, at CIAAPRA's bits 7:2 when reached through this "
            "rig's address-driven read at $BFE001 -- the fixture's full data path (gary's decode plus ciaa's own "
            "porta pass-through) is live, not stuck");
    }

    // ========================================================================
    // PRE -- precondition every A1-A4 reading depends on: gary actually
    // asserts ciaa's chip-enable for $BFE001. Asserted, not assumed.
    // ========================================================================
    do_reset();
    {
        // MUTATE=PRE: present a non-CIA address while the check still
        // expects sel_cia_a asserted.
        set_address(mutating("PRE") ? ADDR_NONCIA : ADDR_BFE001_PRA);
        checks.note("PRE-STIMULUS: cpu_address=0x" + hex2((uint8_t)(ADDR_BFE001_PRA >> 16)) +
                    hex2((uint8_t)(ADDR_BFE001_PRA >> 8)) + hex2((uint8_t)ADDR_BFE001_PRA) +
                    " (real address $BFE001); sel_cia=" + std::to_string((int)dut->sel_cia) +
                    " sel_cia_a=" + std::to_string((int)dut->sel_cia_a) +
                    " sel_cia_b=" + std::to_string((int)dut->sel_cia_b));
        checks.expect(dut->sel_cia_a == 1,
            "PRE-AEN-ASSERTED-AT-BFE001 (precondition for A1-A4, not itself a manual citation): presenting CPU "
            "address $BFE001 to gary asserts sel_cia_a (gary.v:190-191), the chip-enable every one of A1-A4's PRA "
            "readings depends on");
    }

    // ========================================================================
    // PRE-NEG -- isolation control: an address outside $BFxxxx entirely
    // leaves the CIA-A chip-enable deasserted, proving the decode
    // discriminates rather than floating/tied high.
    // ========================================================================
    do_reset();
    {
        // MUTATE=PRE-NEG: present the CIA address itself, so the decode
        // asserts and the isolation control fails.
        set_address(mutating("PRE-NEG") ? ADDR_BFE001_PRA : ADDR_NONCIA);
        checks.expect(dut->sel_cia == 0 && dut->sel_cia_a == 0,
            "PRE-NEG-AEN-DEASSERTED-OUTSIDE-CIA-WINDOW (isolation control, not a manual citation): presenting a "
            "non-CIA address ($000000) leaves sel_cia and sel_cia_a deasserted (gary.v:190) -- the decode "
            "discriminates $BFxxxx from other space rather than answering everywhere");
    }

    // ========================================================================
    // CTRL -- register-discrimination control, same rig and measurement
    // basis as A1-A4: a DIFFERENT address inside the very same $BFE0xx
    // CIA-A window (rs=4'h2, DDRA) delivers a distinct, known word,
    // separate from PRA's. Satisfies "at least one control case expected
    // to PASS on the same measurement basis."
    // ========================================================================
    do_reset();
    {
        // MUTATE=CTRL: write a different byte to DDRA while the check
        // still compares the read-back against 0xA5.
        cia_write(ADDR_BFE201_DDRA, mutating("CTRL") ? 0x00 : 0xA5);
        uint8_t ddra_val = cia_read(ADDR_BFE201_DDRA);
        set_porta_word(0b110011);
        settle();
        uint8_t pra_val = cia_read(ADDR_BFE001_PRA);
        checks.note("CTRL-STIMULUS: wrote 0xA5 to $BFE201 (DDRA, rs=4'h2); read back DDRA=0x" + hex2(ddra_val) +
                    ", read back PRA at $BFE001=0x" + hex2(pra_val));
        checks.expect(ddra_val == 0xA5 && ddra_val != pra_val,
            "CTRL-DDRA-DISTINCT-FROM-PRA (control case, no manual citation, same rig/measurement basis as A1-A4): "
            "writing 0xA5 to $BFE201 (rs=4'h2, DDRA, ciaa.v:138) inside the same $BFE0xx CIA-A window and reading "
            "it back gets that byte, distinct from PRA's own reading at $BFE001 -- the address-to-register routing "
            "this rig measures is alive and discriminating, not stuck answering one register for every address");
    }

    // ========================================================================
    // A1 -- opus:8-regs.c5 (p.229). A2 -- sol:8-reg.c5 (p.229, twin of A1).
    // A3 -- opus:8-mbtn.c4 (p.232). A4 -- sol:8-button.c3 (p.232). All four
    // grade the identical measured fact from four separate citing
    // passages: CPU address $BFE001, presented to this rig, selects
    // ciaa's PRA register. Data-content diversity was already proven by
    // A0; these four reuse one recognizable word and focus on the address.
    // ========================================================================
    do_reset();
    {
        set_porta_word(0b100110);
        settle();
        uint32_t addr = mutating("A1") ? ADDR_BFE201_DDRA : ADDR_BFE001_PRA;  // MUTATION: ask DDRA, still expect PRA
        uint8_t v = cia_read(addr);
        checks.note("A1-STIMULUS: read at " + std::string(mutating("A1") ? "$BFE201 (mutated)" : "$BFE001") +
                    " gives 0x" + hex2(v));
        checks.expect((v & 0xFC) == 0b10011000,
            "A1-BFE001-SELECTS-PRA-OPUS-REGS (p.229, opus:8-regs.c5, quote 'CIAAPRA ($BFE001) Input and output "
            "for pin 6 (port 1 & 2 fire buttons)'): CPU address $BFE001, presented to the ciaa+gary rig, drives "
            "gary's sel_cia_a into ciaa's aen and address bits [11:8]=0 into ciaa's rs -- the register that "
            "answers is PRA (rs=4'h0, ciaa.v:136), confirmed by the known porta_in word 0b100110 arriving "
            "unmodified at data_out bits 7:2 [claims: opus:8-dsk.c20, opus:8-cia.c3]");
    }

    do_reset();
    {
        set_porta_word(0b100110);
        settle();
        uint32_t addr = mutating("A2") ? ADDR_BFE201_DDRA : ADDR_BFE001_PRA;  // MUTATION: ask DDRA, still expect PRA
        uint8_t v = cia_read(addr);
        checks.note("A2-STIMULUS: read at " + std::string(mutating("A2") ? "$BFE201 (mutated)" : "$BFE001") +
                    " gives 0x" + hex2(v));
        checks.expect((v & 0xFC) == 0b10011000,
            "A2-BFE001-SELECTS-PRA-SOL-REG (p.229, sol:8-reg.c5, quote 'CIAAPRA ($BFE001) Input and output for "
            "pin 6 (port 1 & 2 fire buttons)'): twin of A1 (same printed quote, independent reader) -- same "
            "measurement, graded independently under its own uid [claims: opus:8-dsk.c20, opus:8-cia.c3]");
    }

    do_reset();
    {
        set_porta_word(0b100110);
        settle();
        uint32_t addr = mutating("A3") ? ADDR_BFE201_DDRA : ADDR_BFE001_PRA;  // MUTATION: ask DDRA, still expect PRA
        uint8_t v = cia_read(addr);
        checks.note("A3-STIMULUS: read at " + std::string(mutating("A3") ? "$BFE201 (mutated)" : "$BFE001") +
                    " gives 0x" + hex2(v));
        checks.expect((v & 0xFC) == 0b10011000,
            "A3-BFE001-SELECTS-PRA-OPUS-MBTN (p.232, opus:8-mbtn.c4, quote 'The left button on the Amiga mouse is "
            "connected to CIAAPRA ($BFE001). Port 1 uses bit 6 and port 2 uses bit 7. A logic'): CPU address "
            "$BFE001 selects PRA at this rig, same measurement as A1, graded independently under its own uid and "
            "page [claims: opus:8-dsk.c20, opus:8-cia.c3]");
    }

    do_reset();
    {
        set_porta_word(0b100110);
        settle();
        uint32_t addr = mutating("A4") ? ADDR_BFE201_DDRA : ADDR_BFE001_PRA;  // MUTATION: ask DDRA, still expect PRA
        uint8_t v = cia_read(addr);
        checks.note("A4-STIMULUS: read at " + std::string(mutating("A4") ? "$BFE201 (mutated)" : "$BFE001") +
                    " gives 0x" + hex2(v));
        checks.expect((v & 0xFC) == 0b10011000,
            "A4-BFE001-SELECTS-PRA-SOL-BUTTON (p.232, sol:8-button.c3, quote 'The left button on the Amiga mouse "
            "is connected to CIAAPRA ($BFE001). Port 1 uses bit 6 and port 2 uses bit 7.'): CPU address $BFE001 "
            "selects PRA at this rig, same measurement as A1, graded independently under its own uid and page "
            "[claims: opus:8-dsk.c20, opus:8-cia.c3]");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
