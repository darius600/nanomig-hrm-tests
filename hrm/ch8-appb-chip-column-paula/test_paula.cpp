// Test purpose: HRM conformance package for Appendix B's printed p.289
//   register-map rows POT0DAT, POT1DAT, POTGOR, SERDATR, SERDAT, SERPER and
//   POTGO, composed with p.287's chip-letter legend ("P=Paula chip"), against
//   the `paula` module compiled alone. 14 claims, from
//   work/ch8-s1-build/ch8-appb-chip-column-paula.jsonl. Full derivation in
//   SPECIFICATION.md (committed as a387dd3 before this file was written).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   p.289 (Appendix B row for each register), composed with printed p.287
//   (the chip-letter legend) and, for scope only, printed p.288 (the "(E)"
//   definition, already graded by chapter 7's accepted opus:7-appB.c11).
// Fixture boundary: `paula` (paula.v) and whatever it itself instantiates --
//   paula_uart.v, paula_intcontroller.v, paula_floppy.v (and its own
//   paula_floppy_fifo.v), paula_audio.v (and its own paula_audio_channel.v,
//   paula_audio_mixer.v, paula_audio_volume.v). No other module is compiled
//   into this package -- confirmed by this file's own Makefile SRC list, the
//   only place a module could be added.
//
// THE (E) MARK -- out of scope on this page. POT0DAT and POT1DAT print
//   "P (E)" in the CHIP column, not bare "P". Printed p.287 (the legend these
//   two claims cite) does not define "(E)" -- MEASURED: 0 occurrences on
//   p.287 (both claims' own classify.conflict_resolution). "(E)" IS defined
//   on printed p.288 ("...those registers have been changed in the Enhanced
//   Chip Set (ECS)"), already graded testable by chapter 7's accepted
//   opus:7-appB.c11. That definition is not re-derived or re-graded here, and
//   "(E)" is NOT read as "ECS" anywhere in this file. Moot in this package
//   regardless, since both rows carrying it (POT0DAT, POT1DAT) are declined
//   below for the boundary reason next.
//
// THE CHIP-COLUMN BOUNDARY SPLIT -- the point this package exists to make.
//   Measured directly against this RTL (grep over paula.v and every module it
//   instantiates, zero hits for "POT" anywhere): POT0DAT ($012), POT1DAT
//   ($014), POTGOR/"POTINP" ($016) and POTGO ($034) are decoded nowhere in
//   `paula`'s own hierarchy. They ARE decoded in userio.v (parameters
//   POT0DAT/POT1DAT/POTINP/POTGO, userio.v:57-60), a SIBLING module
//   instantiated as USERIO1 in minimig.v:551 -- never inside paula.v, and not
//   part of this package's SRC list. A CHIP-column letter is a claim about
//   which chip answers at an address; a fixture that never receives that
//   chip's decode logic can return neither a yes nor a no about it. These
//   four rows are DECLINED with record(), naming userio.v, per BUILD-BRIEF's
//   carried-forward rule for CHIP-column claims -- the fixture is not widened
//   to force an answer.
//   SERDATR ($018), SERDAT ($030) and SERPER ($032) ARE decoded inside
//   paula_uart.v (paula_uart.v:21-23), instantiated as `pu1` inside paula.v
//   at paula.v:203. These three are ASSERTED. Chip ownership is structural
//   here exactly as chapter 7's accepted paula_intcontroller package argues
//   for INTENAR/INTREQR: this file's only DUT is `paula` and what it itself
//   instantiates, with no other chip's decode logic present anywhere in the
//   build, so a content-carrying round trip through the register's own
//   address is simultaneously its functional check and its CHIP=P check.
//
// FIXTURE LIVENESS (A0). Independent of any claim: paula.v's OWN internal
//   ADKCON (write, $09E) / ADKCONR (read, $010) pair is round-tripped with
//   two distinct SET/CLR-encoded words before any claim-bearing register is
//   trusted, proving the reg_address_in -> internal register -> data_out
//   path is alive at all (the same technique, same registers' SET/CLR
//   encoding, as chapter 7's accepted paula_intcontroller A0 section).
//
// SERDATR/SERDAT/SERPER measurement technique. SERDAT and SERPER are
//   documented write-only (p.289's "W" column) and have no read-back path in
//   this RTL for their OWN stored content (serdat/serper are read only by
//   paula_uart's internal TX logic, never muxed onto data_o) -- confirmed by
//   reading paula_uart.v in full. So each of the three rows is tested through
//   the observable behaviour its own write-only status still allows:
//     - SERDAT ($030): a write to SERDAT's own address is what starts a
//       transmission at all (observed on the top-level `txd` pin).
//     - SERPER ($032): SERPER's content sets the bit period, observed as the
//       WIDTH (in ticks) of the very same transmission's start-bit pulse on
//       `txd` -- a larger SERPER value producing a measurably wider pulse.
//     - SERDATR ($018): an external loopback (this testbench wires
//       dut->rxd = dut->txd every tick, outside the RTL) lets a real
//       transmission be received back through paula_uart's own RX state
//       machine. This package does not hand-derive the exact bit layout
//       paula_uart's RX assembly produces (LONG_BIT framing, sample timing) --
//       doing that by hand risks a wrong expected constant that would fail
//       for a reason that has nothing to do with the RTL. Instead SERDATR is
//       read (MEASURED, printed) before and after the loopback, and the
//       assertion is that content genuinely CHANGED and is non-zero -- the
//       register's own read address is live and reflects something that only
//       a real reception can produce, not merely "a write occurred elsewhere".
//   SERDAT's stimulus for the SERDAT/SERPER checks is 0x0001 (a single data
//   bit set at position 0): paula_uart's TX shifts tx_shift right, sending
//   the LSB of that value as the sole data bit, and its transmission ends the
//   moment the shift register reaches 0 -- with 0x0001 that is exactly one
//   bit period after the start bit, giving a single, unambiguous LOW pulse on
//   `txd` whose width is governed purely by SERPER. The SERDATR loopback uses
//   a separate, richer stimulus (0x00AA) so the received content is
//   distinguishable from the all-zero/idle case.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime check,
//   `mutating("ID")`, read from the MUTATE environment variable at startup --
//   not a compile-time #ifdef -- so one build serves the real run (MUTATE
//   unset) and every mutation run (MUTATE=<id>, one id per assertion). Each
//   mutation changes what the module is ASKED TO DO (a written value, a
//   driven address) and leaves the `expect()` call's own expected-value code
//   untouched, per BUILD-BRIEF. Every one below was run with its own id,
//   confirmed red, then reverted (MUTATE unset) and confirmed green again;
//   results are logged next to each assertion and summarised in the build
//   report. Baseline with MUTATE unset: 5 passed, 0 failed.
//
// MUTATION A0-LIVENESS-ADKCON-FIRST stimulus red: a different first word (0x00AA) is written to ADKCON (MUTATE=A0_1)
// MUTATION A0-LIVENESS-ADKCON-SECOND stimulus red: the stale first word is replayed as the second (MUTATE=A0_2)
// MUTATION T4-SERDATR-CONTENT-ARRIVES stimulus red: the read is issued at word address 0x00 (decoded nowhere in this fixture) instead of SERDATR's own $018 (MUTATE=T4)
// MUTATION T5-SERDAT-WRITE-TRIGGERS-TX stimulus red: the write is issued at word address 0x00 (decoded nowhere in this fixture) instead of SERDAT's own $030 (MUTATE=T5)
// MUTATION T6-SERPER-CONTROLS-BIT-PERIOD stimulus red: the "slow" run secretly reuses the fast SERPER value instead of the large one (MUTATE=T6)
//
// MUTATE=T5 also reddens T6-SERPER-CONTROLS-BIT-PERIOD (measured, not just
// expected): T6's comparison reuses the fast-SERPER pulse width the T5 block
// itself measures, so a T5 mutation that stops that pulse from ever
// occurring leaves T6 with no valid "fast" width to compare against either.
// T5 remains the sole record for T5-SERDAT-WRITE-TRIGGERS-TX; this is named
// here rather than tidied away, the same convention chapter 7's accepted
// paula_intcontroller package uses for its own cross-reddening ids.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include "Vpaula.h"
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

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Vpaula* dut = new Vpaula;
    Checks checks;

    int clk7_cycle = 0;
    // External serial loopback, wired entirely in this testbench (never in
    // the RTL): rxd always tracks the DUT's own txd, exactly as a physical
    // loopback cable would. Harmless to every non-serial assertion below,
    // since idle txd (1) keeps rxd idle (1) too.
    auto tick = [&]() {
        dut->rxd = dut->txd;
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
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->clk7n_en = 0;
        dut->cck = 0;
        dut->reset = 1;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->rxd = 1;              // idle-high, matches real RS232 idle
        dut->ntsc = 0;
        dut->sof = 0;
        dut->strhor = 0;
        dut->vblint = 0;
        dut->int2 = 0;
        dut->int3 = 0;
        dut->int6 = 0;
        dut->_step = 1;
        dut->direc = 0;
        dut->_sel = 0xF;           // all drives deselected (active low)
        dut->side = 0;
        dut->_motor = 1;           // motor off (active low)
        dut->sdc_image_mounted = 0;
        dut->sdc_image_size = 0;
        dut->sdc_busy = 0;
        dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0;
        dut->sdc_byte_addr = 0;
        dut->sdc_byte_in_data = 0;
        dut->floppy_drives = 0;
        dut->floppy_wrprot = 0;
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
    // reg_address_in[8:1] -- confirmed at paula.v:67 and used by every
    // package in this tree the same way (e.g. paula_intcontroller's own
    // INTENA/INTENAR addresses).
    const int ADKCON  = 0x4F;  // write, byte $09E -- paula.v:126
    const int ADKCONR = 0x08;  // read,  byte $010 -- paula.v:127
    const int SERDAT  = 0x18;  // write, byte $030 -- paula_uart.v:21
    const int SERDATR = 0x0C;  // read,  byte $018 -- paula_uart.v:22
    const int SERPER  = 0x19;  // write, byte $032 -- paula_uart.v:23
    const int POT0DAT = 0x09;  // byte $012 -- userio.v:57, NOT in this fixture
    const int POT1DAT = 0x0A;  // byte $014 -- userio.v:58, NOT in this fixture
    const int POTGOR  = 0x0B;  // byte $016 -- userio.v:59 ("POTINP"), NOT in this fixture
    const int POTGO   = 0x1A;  // byte $034 -- userio.v:60, NOT in this fixture
    const int UNUSED_ADDR = 0x00;  // decoded nowhere in this fixture's hierarchy (measured: grep, zero hits)
    const uint16_t SET = 0x8000;
    const uint16_t CLR = 0x0000;

    // ========================================================================
    // A0 -- FIXTURE DATA-PATH LIVENESS. No claim id, no manual page: the
    // BUILD-BRIEF's own requirement that the fixture prove it can deliver a
    // known, distinct word before any read-based assertion is trusted. Uses
    // paula.v's own ADKCON/ADKCONR pair (paula.v:126-127, 182-198), not any
    // of the seven claim rows, so it cannot be confused with any of them.
    // ========================================================================
    do_reset();
    {
        write_reg(ADKCON, CLR | 0x7FFF);
        uint16_t first = mutating("A0_1") ? 0x00AA : 0x0055;  // MUTATION: wrong first word
        write_reg(ADKCON, SET | first);
        uint16_t a1 = read_reg(ADKCONR);
        checks.expect(a1 == 0x0055,
            "A0-LIVENESS-ADKCON-FIRST: a known word (0x0055) written to ADKCON arrives at ADKCONR");

        write_reg(ADKCON, CLR | 0x0055);
        uint16_t second = mutating("A0_2") ? 0x0055 : 0x00AA;  // MUTATION: replay the stale word
        write_reg(ADKCON, SET | second);
        uint16_t a2 = read_reg(ADKCONR);
        checks.expect(a2 == 0x00AA,
            "A0-LIVENESS-ADKCON-SECOND: a second, different known word (0x00AA) also arrives, so the path is not stuck at the first value");
    }

    // ========================================================================
    // T1/T2/T3/T7 -- POT0DAT, POT1DAT, POTGOR, POTGO: DECLINED. See the
    // header for the measurement (grep over this fixture's own compiled
    // hierarchy, zero hits) that established this.
    // ========================================================================
    do_reset();
    {
        uint16_t v = read_reg(POT0DAT);
        checks.note("POT0DAT-ADDRESS-0x012-ON-PAULA-ALONE: reading word address 0x09 ($012) on the isolated paula fixture returns 0x" +
                    [](uint16_t x){ char b[8]; std::snprintf(b, sizeof b, "%04X", x); return std::string(b); }(v));
        checks.record(
            "T1-POT0DAT (p.289, opus:8-appB.c20 sol:8-appB.c19, quote 'POT0DAT      *012   R           P (E)Pot counter "
            "pair 0 data (vert,horiz)'): not asserted -- POT0DAT's address ($012) is decoded nowhere in paula.v or any "
            "module it instantiates (measured: grep over paula.v, paula_uart.v, paula_intcontroller.v, paula_floppy.v, "
            "paula_audio.v and their own children, zero hits for POT anything). It IS decoded in userio.v (parameter "
            "POT0DAT = 9'h012, userio.v:57), a sibling module instantiated as USERIO1 in minimig.v:551 -- never inside "
            "paula.v, and not in this package's Makefile SRC list. A CHIP-column letter names which chip answers at an "
            "address; this fixture never receives userio.v's decode logic and so can return neither a yes nor a no "
            "about it. userio.v is the module that would decide this claim. The row's '(E)' mark is additionally out "
            "of scope here regardless (see header) -- moot, since the P half itself is declined for this boundary "
            "reason, not for the (E) reason.");
    }
    do_reset();
    {
        uint16_t v = read_reg(POT1DAT);
        checks.note("POT1DAT-ADDRESS-0x014-ON-PAULA-ALONE: reading word address 0x0A ($014) on the isolated paula fixture returns 0x" +
                    [](uint16_t x){ char b[8]; std::snprintf(b, sizeof b, "%04X", x); return std::string(b); }(v));
        checks.record(
            "T2-POT1DAT (p.289, opus:8-appB.c25 sol:8-appB.c24, quote 'POT1DAT      *014   R           P (E)Pot counter "
            "pair 1 data (vertfhoriz)'): not asserted -- POT1DAT's address ($014) is decoded nowhere in paula.v or any "
            "module it instantiates (measured: grep, zero hits). It IS decoded in userio.v (parameter POT1DAT = "
            "9'h014, userio.v:58), instantiated as USERIO1 in minimig.v:551, never inside paula.v and not in this "
            "package's SRC list. userio.v is the module that would decide this claim. The '(E)' mark is additionally "
            "out of scope here regardless (see header) -- moot, since the P half itself is declined for this boundary "
            "reason, not for the (E) reason.");
    }
    do_reset();
    {
        uint16_t v = read_reg(POTGOR);
        checks.note("POTGOR-ADDRESS-0x016-ON-PAULA-ALONE: reading word address 0x0B ($016) on the isolated paula fixture returns 0x" +
                    [](uint16_t x){ char b[8]; std::snprintf(b, sizeof b, "%04X", x); return std::string(b); }(v));
        checks.record(
            "T3-POTGOR (p.289, opus:8-appB.c30 sol:8-appB.c29, quote 'POTGOR       *016   R           P    Pot port "
            "data read (formerly POTINP)'): not asserted -- POTGOR's address ($016) is decoded nowhere in paula.v or "
            "any module it instantiates (measured: grep, zero hits). It IS decoded in userio.v under the RTL's own "
            "name for the same address, parameter POTINP = 9'h016 (userio.v:59, matching the manual's own parenthetical "
            "'formerly POTINP'), instantiated as USERIO1 in minimig.v:551, never inside paula.v and not in this "
            "package's SRC list. userio.v is the module that would decide this claim.");
    }
    do_reset();
    {
        uint16_t v = read_reg(POTGO);
        checks.note("POTGO-ADDRESS-0x034-ON-PAULA-ALONE: reading word address 0x1A ($034) on the isolated paula fixture returns 0x" +
                    [](uint16_t x){ char b[8]; std::snprintf(b, sizeof b, "%04X", x); return std::string(b); }(v));
        checks.record(
            "T7-POTGO (p.289, opus:8-appB.c50 sol:8-appB.c49, quote 'POTGO        *034   W           P    Pot port "
            "data write and start'): not asserted -- POTGO's address ($034) is decoded nowhere in paula.v or any "
            "module it instantiates (measured: grep, zero hits). It IS decoded in userio.v (parameter POTGO = "
            "9'h034, userio.v:60), instantiated as USERIO1 in minimig.v:551, never inside paula.v and not in this "
            "package's SRC list. userio.v is the module that would decide this claim.");
    }

    // ========================================================================
    // T5/T6 -- SERDAT, SERPER: a write to SERDAT's own address starts a
    // transmission (observed on txd); SERPER's content sets that
    // transmission's bit period (observed as the start-bit pulse's width).
    // Stimulus: SERDAT=0x0001 -- one data bit set at position 0, so TX shifts
    // exactly once before the shift register reaches 0 and stops: a single,
    // unambiguous LOW pulse on txd whose width is governed purely by SERPER
    // (paula_uart.v:48-64, 77-142).
    // ========================================================================
    do_reset();
    int width_fast;
    {
        const int SERPER_FAST = 0x0002;
        write_reg(SERPER, SERPER_FAST);
        int wr_addr = mutating("T5") ? UNUSED_ADDR : SERDAT;  // MUTATION: write lands nowhere this fixture decodes
        write_reg(wr_addr, 0x0001);

        int low_at = -1, high_at = -1;
        for (int i = 0; i < 400 && high_at < 0; i++) {
            tick();
            if (low_at < 0 && dut->txd == 0) low_at = i;
            else if (low_at >= 0 && dut->txd == 1) high_at = i;
        }
        checks.note("TXD-PULSE-FAST-SERPER: with SERPER=0x0002, txd went low at tick " + std::to_string(low_at) +
                    " and back high at tick " + std::to_string(high_at));
        checks.expect(low_at >= 0 && high_at > low_at,
            "T5-SERDAT-WRITE-TRIGGERS-TX (p.289, opus:8-appB.c40 sol:8-appB.c39, quote 'SERDAT       *030   W           "
            "P    Serial port data and stop bits write'): a write at SERDAT's own address ($030) is what starts a "
            "transmission -- txd leaves its idle-high state and returns to it, which only paula_uart's TX logic can "
            "produce, and only paula_uart is instantiated inside this fixture (pu1, paula.v:203)");
        width_fast = (low_at >= 0 && high_at > low_at) ? (high_at - low_at) : -1;
    }

    do_reset();
    int width_slow;
    {
        const uint16_t SERPER_SLOW_REAL = 0x0064;   // 100 -- much longer bit period than 0x0002
        uint16_t serper_slow = mutating("T6") ? 0x0002 : SERPER_SLOW_REAL;  // MUTATION: secretly reuse the fast period
        write_reg(SERPER, serper_slow);
        write_reg(SERDAT, 0x0001);

        int low_at = -1, high_at = -1;
        for (int i = 0; i < 4000 && high_at < 0; i++) {
            tick();
            if (low_at < 0 && dut->txd == 0) low_at = i;
            else if (low_at >= 0 && dut->txd == 1) high_at = i;
        }
        checks.note("TXD-PULSE-SLOW-SERPER: with SERPER=0x0064 (or the mutated value), txd went low at tick " +
                    std::to_string(low_at) + " and back high at tick " + std::to_string(high_at));
        width_slow = (low_at >= 0 && high_at > low_at) ? (high_at - low_at) : -1;
        checks.expect(width_fast > 0 && width_slow > 0 && width_slow > width_fast * 4,
            "T6-SERPER-CONTROLS-BIT-PERIOD (p.289, opus:8-appB.c45 sol:8-appB.c44, quote 'SERPER       *032   W        "
            "   P    Serial port period and control'): SERPER's own content, written at its own address ($032), sets "
            "the width of the very transmission SERDAT starts -- a far larger SERPER value (0x0064 vs 0x0002) produces "
            "a measurably wider start-bit pulse on txd (measured fast=" + std::to_string(width_fast) + " ticks, slow=" +
            std::to_string(width_slow) + " ticks)");
    }

    // ========================================================================
    // T4 -- SERDATR: an external loopback (dut->rxd = dut->txd, wired only in
    // this testbench, every tick) lets a real transmission be received back
    // through paula_uart's own RX state machine, and content that differs
    // from both the reset baseline and zero is read back at SERDATR's own
    // address ($018) -- content genuinely arrives at the register this row
    // names, not merely "a write occurred".
    // Stimulus: SERPER=0x0000 (fastest bit period, for a short simulation),
    // SERDAT=0x00AA (a richer, distinguishable-from-idle bit pattern).
    // ========================================================================
    do_reset();
    {
        uint16_t baseline = read_reg(SERDATR);
        checks.note("SERDATR-RESET-BASELINE: SERDATR reads 0x" +
                    [](uint16_t x){ char b[8]; std::snprintf(b, sizeof b, "%04X", x); return std::string(b); }(baseline) +
                    " immediately after reset, before any transmission");

        write_reg(SERPER, 0x0000);
        write_reg(SERDAT, 0x00AA);
        for (int i = 0; i < 400; i++) tick();

        int rd_addr = mutating("T4") ? UNUSED_ADDR : SERDATR;  // MUTATION: read lands nowhere this fixture decodes
        uint16_t after = read_reg(rd_addr);
        checks.note("SERDATR-AFTER-LOOPBACK: SERDATR reads 0x" +
                    [](uint16_t x){ char b[8]; std::snprintf(b, sizeof b, "%04X", x); return std::string(b); }(after) +
                    " after a 0x00AA transmission has looped back through rxd");

        checks.expect(after != baseline && after != 0x0000,
            "T4-SERDATR-CONTENT-ARRIVES (p.289, opus:8-appB.c35 sol:8-appB.c34, quote 'SERDATR      *018   R           "
            "P    Serial port data and status read'): reading SERDATR's own address ($018) after a real transmission "
            "has looped back through paula_uart's RX state machine returns content that differs from both the reset "
            "baseline (0x" + [](uint16_t x){ char b[8]; std::snprintf(b, sizeof b, "%04X", x); return std::string(b); }(baseline) +
            ") and an unaddressed 0x0000 -- the data genuinely arrived at this register's own read address, which "
            "only paula_uart (instantiated inside this fixture, pu1, paula.v:203) can produce");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
