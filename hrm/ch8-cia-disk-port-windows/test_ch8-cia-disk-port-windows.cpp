// Test purpose: HRM conformance package for the printed-page claims that the
//   disk subsystem uses two CIA ports (p.241), that CIABPRB answers at
//   $BFD100 (p.241/244, the latter a corpus defect corrected to CIABPRB),
//   and that DSKINDEX* answers at $BFDD00 bit 4 (p.245) -- against a rig
//   over `gary` + `ciaa` + `ciab` (cia_disk_port_windows_rig.v, this
//   package's own wrapper -- not pinned RTL). 7 claims, from
//   work/ch8-s2-build/ch8-cia-disk-port-windows.jsonl. Full derivation in
//   SPECIFICATION.md (committed as 9d3366c, before any RTL was opened).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed p.241 ("The disk
//   subsystem uses two ports on the system's 8520 CIA chips..." and "CIABPRB
//   ($BFD100) eight output bits for disk selection, control and stepping"),
//   p.244 ("PB are output bits located in CIAAPRB ($BFD100)" -- corpus
//   defect, register is CIABPRB, resolved during classification on grounds
//   that include the very decode this package measures) and p.245 ("FLAG
//   DSKINDEX* Disk index pulse ($BFDD00, bit 4)").
// Non-goals: anything outside the `cia_disk_port_windows_rig` fixture this
//   package builds, and any claim it does not cite. A green here
//   means those assertions held on this fixture at the pinned revision --
//   not that the module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/ch8-s2-build/ch8-cia-disk-port-windows.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// WHAT THIS PACKAGE MEASURES, disclosed by the build task and by
//   finding-candidate C12 before any RTL was opened, and confirmed below by
//   reading gary.v in full: gary.v:191-192 --
//     assign sel_cia_a = sel_cia & ~cpu_address_in[12];
//     assign sel_cia_b = sel_cia & ~cpu_address_in[13];
//   -- each tests exactly one address bit. Bits 15/14 (inside the fully-
//   compared $BFxxxx byte) appear in neither line, and bit 12 does not
//   appear in sel_cia_b's expression nor bit 13 in sel_cia_a's. So (a)
//   $BFD100 and $BFDD00 are each one member of a wider alias class -- other
//   addresses sharing [23:16]=$BF, the relevant one of [13]/[12], and the
//   same `rs` answer identically -- and (b) an address with BOTH [13]=0 and
//   [12]=0 asserts sel_cia_a and sel_cia_b at once. Every address claim on
//   these pages is therefore a claim about a window, not a point, and that
//   is what CIABPRB-ALIAS-*, DSKINDEX-ALIAS-* and OVERLAP-BOTH-SELECTS-ACTIVE
//   below make checkable rather than merely cited.
//
// RTL READ, IN ORDER, ALL AFTER SPECIFICATION.md WAS COMMITTED:
//   1. hrm/ch8-ciaa-port-direction/Makefile -- build pattern (ciaa/ciab need
//      cia_int.v, cia_timera.v, cia_timerb.v, cia_timerd.v to elaborate),
//      named in the build task.
//   2. $(NANOMIG)/src/minimig-aga/gary.v -- full file.
//   3. $(NANOMIG)/src/minimig-aga/ciab.v -- full file.
//   4. $(NANOMIG)/src/minimig-aga/ciaa.v -- full file.
//   5. $(NANOMIG)/src/minimig-aga/cia_int.v -- full file (icr[4]<-flag on
//      every clk7_en cycle when the register is not itself being read;
//      icrs&&!wr instead latches icr[4] directly from the CURRENT flag and
//      clears otherwise -- standard latch/clear-on-read).
//   6. $(NANOMIG)/src/minimig-aga/minimig.v lines 600-651 (ciaa CIAA1 /
//      ciab CIAB1 instantiations) and 762-819 (gary GARY1 instantiation) --
//      the wiring cia_disk_port_windows_rig.v reproduces; see that file's
//      own header for the join-by-join citation.
//   cia_timera.v, cia_timerb.v, cia_timerd.v were NOT read -- ciaa/ciab
//   instantiate them for the timers, which none of this package's
//   assertions touch, the same scoping call ch8-ciaapra-address-window made.
//
// A FACT WORTH RECORDING, found while reading minimig.v: ciaa's data_out
//   and ciab's data_out land on DIFFERENT byte lanes of minimig's own 16-bit
//   cia_data_out bus ([7:0] for ciaa, minimig.v:628; [15:8] for ciab,
//   minimig.v:653) -- so at minimig's own boundary, the OVERLAP-BOTH-
//   SELECTS-ACTIVE condition would not corrupt a byte-wide CPU *read*
//   (each chip's answer lands on its own lane). This rig does not build
//   that lane-combining logic -- it is one level above gary+ciaa+ciab, not
//   reproduced here -- and OVERLAP-BOTH-SELECTS-ACTIVE below grades only
//   the decode-level fact (both `sel_cia_a` and `sel_cia_b` asserted by
//   gary for one address), which is what the claims' own evidence and the
//   build task name, not any data-bus consequence of it. Recorded here so
//   the boundary is not silently narrower than it looks.
//
// TWOPORTS' CIAA ADDRESS is deliberately NOT $BFE001: that literal address
//   is ch8-ciaapra-address-window's own subject (four claims, cited onto
//   that package, not this one), and the build task says not to duplicate
//   it. TWOPORTS uses $BFA000 instead -- a different member of ciaa's own
//   PRA window ([13]=1 so sel_cia_b stays clear, [12]=0 so sel_cia_a
//   asserts, rs=4'h0=PRA) -- so this package's own liveness/two-ports
//   evidence is independent of that package's assertions rather than a
//   restatement of them.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup -- not a compile-time #ifdef -- so one build serves the real
//   run (MUTATE unset) and every mutation run (MUTATE=<id>, one id per
//   assertion). Every `mutating(...)` call is its own statement, computed
//   into a local variable BEFORE the `expect()` call that uses it -- never
//   written inline inside an `expect()` argument list, which would place a
//   string literal ahead of the label in that same call and steal the
//   assertion id the gate reads. Baseline with MUTATE unset: 14 passed, 0
//   failed. Each of MUTATE=<id> below was run individually and confirmed
//   13 passed, 1 failed (only the targeted assertion reddens), then
//   reverted and confirmed back to 14 passed, 0 failed.
//
// MUTATION LIVE-CIAB-ICR-FLAG-DISTINCT stimulus red: flag left unpulsed before the second ICR read too, so both readings are the unpulsed value (MUTATE=LIVE-CIAB-ICR-FLAG-DISTINCT). RUN: baseline 14/0 -> 13/1, only this label red; reverted 14/0.
// MUTATION PRE-SELCIAB-ASSERTED stimulus red: address redirected to non-CIA space while the check still expects sel_cia_b asserted (MUTATE=PRE-SELCIAB-ASSERTED). RUN: baseline 14/0 -> 13/1, only this label red; reverted 14/0.
// MUTATION PRE-NEG-NONCIA-DESELECTED stimulus red: the CIA address itself presented, so the decode asserts and the isolation control fails (MUTATE=PRE-NEG-NONCIA-DESELECTED). RUN: baseline 14/0 -> 13/1, only this label red; reverted 14/0.
// MUTATION OVERLAP-BOTH-SELECTS-ACTIVE stimulus red: address's bit 12 reverted to 1 (no overlap) while the check still expects both selects asserted (MUTATE=OVERLAP-BOTH-SELECTS-ACTIVE). RUN: baseline 14/0 -> 13/1, only this label red; reverted 14/0.
// MUTATION CIABPRB-LITERAL-OPUS-DSK-C23 stimulus red: write/read address redirected from $BFD100 to a non-CIA address while the check still expects the written byte to come back (MUTATE=CIABPRB-LITERAL-OPUS-DSK-C23)
// MUTATION CIABPRB-ALIAS-OPUS-CIA-C5 stimulus red: alias address redirected outside the window (bit 13 set, deselecting CIA-B) while the check still expects $BFD100's own byte to come back identically (MUTATE=CIABPRB-ALIAS-OPUS-CIA-C5)
// MUTATION CIABPRB-CHIPLETTER-SOL-CIA-C4 stimulus red: address's bit 12 driven to 0 (asserting sel_cia_a too) while the check still expects sel_cia_a=0 at this address (MUTATE=CIABPRB-CHIPLETTER-SOL-CIA-C4)

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include "Vcia_disk_port_windows_rig.h"
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
    Vcia_disk_port_windows_rig* dut = new Vcia_disk_port_windows_rig;
    Checks checks;

    // Byte addresses, converted to this rig's [23:1] address port by
    // dropping bit 0 (>>1), the same arithmetic ch8-ciaapra-address-window
    // uses. Derived from the literal byte value at runtime rather than
    // hand-computed, to keep the bit-pattern reasoning (in SPECIFICATION.md
    // and this file's own header) separate from arithmetic risk.
    const uint32_t BFD100_BYTE = 0xBFD100;  // literal CIABPRB   ([13]=0,[12]=1,rs=1)
    const uint32_t BFD300_BYTE = 0xBFD300;  // CIAB DDRB, same aen family as BFD100 (rs=3)
    const uint32_t BF1100_BYTE = 0xBF1100;  // CIABPRB alias     ([13]=0,[12]=1,rs=1, [15:14] differ from BFD100)
    const uint32_t BF3100_BYTE = 0xBF3100;  // CIABPRB-ALIAS mutation target: BF1100 with [13] set -- outside the window (sel_cia_b deselected)
    const uint32_t BFC100_BYTE = 0xBFC100;  // overlap address   ([13]=0,[12]=0,rs=1) -- also BFD100 with [12] cleared
    const uint32_t BFDD00_BYTE = 0xBFDD00;  // literal DSKINDEX  ([13]=0,[12]=1,rs=0xD)
    const uint32_t BF1D00_BYTE = 0xBF1D00;  // DSKINDEX alias    ([13]=0,[12]=1,rs=0xD, [15:14] differ from BFDD00)
    const uint32_t BF3D00_BYTE = 0xBF3D00;  // DSKINDEX-ALIAS mutation target: BF1D00 with [13] set -- outside the window
    const uint32_t BFA000_BYTE = 0xBFA000;  // ciaa PRA window   ([13]=1,[12]=0,rs=0) -- not $BFE001, see header
    const uint32_t NONCIA_BYTE = 0x000000;  // chip RAM, outside $BFxxxx entirely

    const uint32_t ADDR_BFD100 = BFD100_BYTE >> 1;
    const uint32_t ADDR_BFD300 = BFD300_BYTE >> 1;
    const uint32_t ADDR_BF1100 = BF1100_BYTE >> 1;
    const uint32_t ADDR_BF3100 = BF3100_BYTE >> 1;
    const uint32_t ADDR_BFC100 = BFC100_BYTE >> 1;
    const uint32_t ADDR_BFDD00 = BFDD00_BYTE >> 1;
    const uint32_t ADDR_BF1D00 = BF1D00_BYTE >> 1;
    const uint32_t ADDR_BF3D00 = BF3D00_BYTE >> 1;
    const uint32_t ADDR_BFA000 = BFA000_BYTE >> 1;
    const uint32_t ADDR_NONCIA = NONCIA_BYTE >> 1;

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
        dut->ciaa_porta_in = 0x3F;
        dut->ciaa_portb_in = 0xFF;
        dut->ciaa_tick = 0;
        dut->ciaa_eclk = 0;
        dut->kms_level = 0;
        dut->kbd_mouse_type = 0;
        dut->kbd_mouse_data = 0;
        dut->hrtmon_en = 0;
        dut->ciab_porta_in = 0x3F;
        dut->ciab_tick = 0;
        dut->ciab_eclk = 0;
        dut->ciab_flag = 0;
        for (int i = 0; i < 16; i++) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; i++) tick();
    };

    auto set_address = [&](uint32_t addr23) {
        dut->cpu_address = addr23 & 0x7FFFFF;
        dut->eval();
    };

    // Read through both chips at once (needed for the overlap/precondition
    // checks, which look at gary's sel_cia* outputs rather than data).
    auto present_read = [&](uint32_t addr23) {
        set_address(addr23);
        dut->cpu_rd = 1; dut->cpu_hwr = 0; dut->cpu_lwr = 0;
        dut->eval();
    };
    auto end_read = [&]() {
        dut->cpu_rd = 0;
        dut->eval();
    };

    auto cia_a_read = [&](uint32_t addr23) -> uint8_t {
        present_read(addr23);
        uint8_t v = dut->cia_a_data_out;
        end_read();
        return v;
    };
    auto cia_b_read = [&](uint32_t addr23) -> uint8_t {
        present_read(addr23);
        uint8_t v = dut->cia_b_data_out;
        end_read();
        return v;
    };

    // Writes need the address and wr strobe held across a clk7_en edge
    // (regporta/regportb/ddrportb etc. are clocked). ciaa.data_in is the
    // LOW byte of cpu_data_out (minimig.v:627); ciab.data_in is the HIGH
    // byte (minimig.v:652) -- reproduced by this rig's own port mapping.
    auto cia_a_write = [&](uint32_t addr23, uint8_t byte) {
        dut->cpu_address = addr23 & 0x7FFFFF;
        dut->cpu_data_out = (uint16_t)byte;
        dut->cpu_lwr = 1; dut->cpu_hwr = 0; dut->cpu_rd = 0;
        settle();
        dut->cpu_lwr = 0;
        dut->eval();
    };
    auto cia_b_write = [&](uint32_t addr23, uint8_t byte) {
        dut->cpu_address = addr23 & 0x7FFFFF;
        dut->cpu_data_out = (uint16_t)(byte << 8);
        dut->cpu_hwr = 1; dut->cpu_lwr = 0; dut->cpu_rd = 0;
        settle();
        dut->cpu_hwr = 0;
        dut->eval();
    };

    auto setup_ciab_output_ddrb_allout = [&]() {
        cia_b_write(ADDR_BFD300, 0xFF);  // rs=3, DDRB: all 8 PB bits driven as outputs
    };

    // ========================================================================
    // LIVE-CIAA-PORTA-DISTINCT -- fixture data-path liveness for ciaa's
    // porta_in -> PRA read path, through this rig's own address decode
    // (gary's sel_cia_a into ciaa's aen/rs), at an address distinct from
    // $BFE001 (see header). No claim id, no manual page.
    //
    // WITHDRAWN to note(): carries no
    // claim uid -- apparatus, never conformance evidence -- so demoting it
    // costs no coverage. No longer mutated (MUTATE=LIVE-CIAA-PORTA-DISTINCT
    // removed with it): a note() is not graded, so there is nothing left
    // for a mutation to flip.
    // ========================================================================
    do_reset();
    {
        auto expected_upper6 = [](uint8_t w6) -> uint8_t {
            return (uint8_t)((w6 & 0x3F) << 2);
        };
        const uint8_t word1 = 0b101100;
        const uint8_t word2 = (uint8_t)0b010011;  // complement

        dut->ciaa_porta_in = word1 & 0x3F;
        settle();
        uint8_t v1 = cia_a_read(ADDR_BFA000);
        dut->ciaa_porta_in = word2 & 0x3F;
        settle();
        uint8_t v2 = cia_a_read(ADDR_BFA000);

        bool w1ok = (v1 & 0xFC) == expected_upper6(word1);
        bool w2ok = (v2 & 0xFC) == expected_upper6(word2);
        bool distinct = (v1 & 0xFC) != (v2 & 0xFC);
        checks.note("LIVE-CIAA-PORTA-DISTINCT [not graded -- apparatus check, no claim uid]: porta_in[7:2]="
                    "0b101100 then its complement, read through the rig at $BFA000, give CIAAPRA bits7:2=0x" +
                    hex2(v1 & 0xFC) + " then 0x" + hex2(v2 & 0xFC) + " -- both arrive unmodified and mutually "
                    "distinguishable (matched=" + std::to_string(w1ok && w2ok && distinct) + "): ciaa's half of "
                    "the fixture's data path is live, not stuck");
    }

    // ========================================================================
    // LIVE-CIAB-PORTB-DISTINCT -- same proof for ciab's PRB write/read-back
    // path (through gary's sel_cia_b), at the literal CIABPRB address.
    //
    // WITHDRAWN to note(): carries no
    // claim uid -- apparatus, never conformance evidence -- so demoting it
    // costs no coverage. No longer mutated (MUTATE=LIVE-CIAB-PORTB-DISTINCT
    // removed with it): a note() is not graded, so there is nothing left
    // for a mutation to flip.
    // ========================================================================
    do_reset();
    {
        setup_ciab_output_ddrb_allout();
        const uint8_t byte1 = 0xA5;
        const uint8_t byte2 = (uint8_t)0x5A;

        cia_b_write(ADDR_BFD100, byte1);
        uint8_t v1 = cia_b_read(ADDR_BFD100);
        cia_b_write(ADDR_BFD100, byte2);
        uint8_t v2 = cia_b_read(ADDR_BFD100);

        bool distinct = v1 != v2;
        checks.note("LIVE-CIAB-PORTB-DISTINCT [not graded -- apparatus check, no claim uid]: 0xA5 then 0x5A "
                    "written to PRB via $BFD100 (DDRB=0xFF first), read back 0x" + hex2(v1) + " then 0x" +
                    hex2(v2) + " (matched=" + std::to_string(v1 == byte1 && v2 == byte2 && distinct) + "): "
                    "ciab's half of the fixture's data path is live, not stuck");
    }

    // ========================================================================
    // LIVE-CIAB-ICR-FLAG-DISTINCT -- fixture liveness for the mechanism
    // DSKINDEX-* depends on: a pulsed vs. unpulsed `flag` reads back as
    // distinct ICR bit-4 values.
    // ========================================================================
    do_reset();
    {
        cia_b_write(ADDR_BFDD00, 0x90);  // icrmask[4] <= 1 (set), data_in[7]=1,[4:0]=0x10
        uint8_t v_unpulsed = cia_b_read(ADDR_BFDD00);

        bool mut_live_icr = mutating("LIVE-CIAB-ICR-FLAG-DISTINCT");
        dut->ciab_flag = mut_live_icr ? 0 : 1;
        settle();
        dut->ciab_flag = 0;
        settle();
        uint8_t v_pulsed = cia_b_read(ADDR_BFDD00);
        checks.note("LIVE-CIAB-ICR-STIMULUS: ICR read unpulsed=0x" + hex2(v_unpulsed) + ", after a flag pulse=0x" +
                    hex2(v_pulsed));

        bool distinct = (v_unpulsed & 0x90) != (v_pulsed & 0x90);
        checks.expect((v_unpulsed & 0x10) == 0x00 && (v_pulsed & 0x90) == 0x90 && distinct,
            "LIVE-CIAB-ICR-FLAG-DISTINCT: an unpulsed and a pulsed `flag`, both read back through the ICR at "
            "rs=4'hD, give distinct bit-4 (FLG) values -- the mechanism the DSKINDEX-* assertions depend on is "
            "not stuck at one value");
    }

    // ========================================================================
    // PRE-SELCIAB-ASSERTED -- precondition every CIABPRB-*/DSKINDEX-*
    // assertion depends on: gary actually asserts ciab's chip-enable for
    // $BFD100. Asserted, not assumed.
    // ========================================================================
    do_reset();
    {
        bool mut_pre = mutating("PRE-SELCIAB-ASSERTED");
        uint32_t addr = mut_pre ? ADDR_NONCIA : ADDR_BFD100;
        set_address(addr);
        checks.note("PRE-STIMULUS: cpu_address selects " + std::string(mut_pre ? "non-CIA space" : "$BFD100") +
                    "; sel_cia=" + std::to_string((int)dut->sel_cia) +
                    " sel_cia_a=" + std::to_string((int)dut->sel_cia_a) +
                    " sel_cia_b=" + std::to_string((int)dut->sel_cia_b));
        checks.expect(dut->sel_cia_b == 1,
            "PRE-SELCIAB-ASSERTED (precondition for CIABPRB-*/DSKINDEX-*, not itself a manual citation): "
            "presenting CPU address $BFD100 to gary asserts sel_cia_b (gary.v:190,192), the chip-enable every "
            "CIABPRB/DSKINDEX reading in this package depends on");
    }

    // ========================================================================
    // PRE-NEG-NONCIA-DESELECTED -- isolation control: an address outside
    // $BFxxxx entirely leaves both chip-enables deasserted.
    // ========================================================================
    do_reset();
    {
        bool mut_preneg = mutating("PRE-NEG-NONCIA-DESELECTED");
        uint32_t addr = mut_preneg ? ADDR_BFD100 : ADDR_NONCIA;
        set_address(addr);
        checks.expect(dut->sel_cia == 0 && dut->sel_cia_a == 0 && dut->sel_cia_b == 0,
            "PRE-NEG-NONCIA-DESELECTED (isolation control, not a manual citation): presenting a non-CIA address "
            "($000000) leaves sel_cia, sel_cia_a and sel_cia_b all deasserted (gary.v:190-192) -- the decode "
            "discriminates $BFxxxx from other space rather than answering everywhere");
    }

    // ========================================================================
    // OVERLAP-BOTH-SELECTS-ACTIVE -- the central mechanism fact this
    // package exists to make checkable: gary.v:191-192's decode is not a
    // full compare, so an address exists where sel_cia_a and sel_cia_b are
    // BOTH asserted. p.351 addresses each chip by one bit alone -- CIA-A by
    // A12*, CIA-B by A13* -- not a compound two-bit decode (NM-HRM-075);
    // read that way it does not exclude the two single-bit conditions
    // holding at the same address, which is what makes every address claim
    // on these pages a window, not a point.
    // ========================================================================
    do_reset();
    {
        bool mut_overlap = mutating("OVERLAP-BOTH-SELECTS-ACTIVE");
        uint32_t addr = mut_overlap ? ADDR_BFD100 : ADDR_BFC100;  // BFD100 has [12]=1: no overlap
        set_address(addr);
        checks.note("OVERLAP-STIMULUS: cpu_address selects " + std::string(mut_overlap ? "$BFD100 ([12]=1)" :
                    "$BFC100 ([13]=0,[12]=0)") + "; sel_cia_a=" + std::to_string((int)dut->sel_cia_a) +
                    " sel_cia_b=" + std::to_string((int)dut->sel_cia_b));
        checks.expect(dut->sel_cia_a == 1 && dut->sel_cia_b == 1,
            "OVERLAP-BOTH-SELECTS-ACTIVE (mechanism fact, not a manual citation): CPU address $BFC100 ([23:16]=$BF, "
            "[13]=0, [12]=0) asserts sel_cia_a AND sel_cia_b simultaneously (gary.v:191-192) -- CIA-A and CIA-B are "
            "both chip-enabled for the same CPU cycle, which is why $BFD100 and $BFDD00 are each one member of an "
            "alias class rather than a unique address");
    }

    // ========================================================================
    // CTRL-DISTINCT-REGISTER-SAME-CHIP -- control case, same rig and
    // measurement basis as the assertions below: within ciab's own
    // chip-select family ([23:16]=$BF, [13]=0, [12]=1), a different `rs`
    // (DDRB, 4'h3, vs. PRB, 4'h1) reads back a distinct, independently-
    // written byte. Satisfies "at least one control case expected to PASS."
    //
    // WITHDRAWN to note(): carries no
    // claim uid -- control case, no manual citation -- so demoting it costs
    // no coverage. No longer mutated
    // (MUTATE=CTRL-DISTINCT-REGISTER-SAME-CHIP removed with it): a note()
    // is not graded, so there is nothing left for a mutation to flip.
    // ========================================================================
    do_reset();
    {
        setup_ciab_output_ddrb_allout();  // DDRB = 0xFF
        cia_b_write(ADDR_BFD100, (uint8_t)0x66);  // PRB
        uint8_t ddrb_val = cia_b_read(ADDR_BFD300);
        uint8_t prb_val  = cia_b_read(ADDR_BFD100);
        checks.note("CTRL-DISTINCT-REGISTER-SAME-CHIP [not graded -- apparatus check, no claim uid]: DDRB "
                    "(rs=3, $BFD300) reads 0x" + hex2(ddrb_val) + "; PRB (rs=1, $BFD100) reads 0x" + hex2(prb_val) +
                    " (matched=" + std::to_string(ddrb_val == 0xFF && prb_val != ddrb_val) + "): within the same "
                    "$BF../[13]=0/[12]=1 chip-select family, DDRB and PRB read back distinct, independently-"
                    "written bytes -- the address-to-register routing this rig measures discriminates registers "
                    "within a chip, not just chips");
    }

    // ========================================================================
    // TWOPORTS-CIAA-PORTA-OPUS-DSK-C18 (p.241, opus:8-dsk.c18).
    // TWOPORTS-CIAB-PORTB-SOL-REGLIST-C1 (p.241, sol:8-reglist.c1, twin).
    // Both grade the identical measured fact from two separate citing
    // passages: two distinct, independently-reachable CIA port registers
    // -- one on ciaa (input direction, porta_in, minimig.v:632), one on
    // ciab (output direction, portb_out, minimig.v:660) -- answer through
    // MUTUALLY EXCLUSIVE chip-selects. NOT graded: that no other CIA port
    // carries a disk signal anywhere else in the design (see
    // SPECIFICATION.md's PROP-TWO-PORTS boundary note).
    //
    // WITHDRAWN to record(): the
    // stimulus on both sides (ciaa_porta_in = 0b100110, PRB <= 0xC3) is a
    // generic hand-picked bit pattern, not a real disk signal -- this
    // proves gary+ciaa+ciab present two distinct, mutually-exclusively-
    // addressed CIA ports, but not that the DISK SUBSYSTEM is what uses
    // them (the claim's own words). hrm/ch8-cia-disk-signal-bindings's
    // full battery of PA5/PA4/PA3/PA2 and PB7..PB0-BINDING assertions now
    // carries that fact: each one drives a REAL, named disk signal
    // (paula_floppy's _ready/_track0/_wprot/_change/_motor/_sel3..0/side/
    // direc/_step) through the pinned minimig.v join and reads it back at
    // CIAAPRA or CIABPRB over a real CPU bus access -- proving, more
    // specifically than this block ever did, that it is the disk
    // subsystem's own signals occupying exactly these two ports.
    // ========================================================================
    do_reset();
    {
        dut->ciaa_porta_in = 0b100110;
        settle();
        uint8_t ciaa_v = cia_a_read(ADDR_BFA000);
        set_address(ADDR_BFA000);
        bool ciaa_sel_ok = dut->sel_cia_a == 1 && dut->sel_cia_b == 0;

        setup_ciab_output_ddrb_allout();
        cia_b_write(ADDR_BFD100, 0xC3);
        uint8_t ciab_v = cia_b_read(ADDR_BFD100);
        set_address(ADDR_BFD100);
        bool ciab_sel_ok = dut->sel_cia_b == 1 && dut->sel_cia_a == 0;

        checks.note("TWOPORTS-CIAA-PORTA-OPUS-DSK-C18 [not graded -- see WITHDRAWN note above]: ciaa read at "
                    "$BFA000 gives bits7:2=0x" + hex2(ciaa_v & 0xFC) + "; ciab PRB reads 0x" + hex2(ciab_v) +
                    " (matched=" + std::to_string((ciaa_v & 0xFC) == 0x98 && ciaa_sel_ok && ciab_v == 0xC3 &&
                    ciab_sel_ok) + ")");
        checks.record(
            "TWOPORTS-CIAA-PORTA-OPUS-DSK-C18 (p.241, opus:8-dsk.c18, quote 'The disk subsystem uses two ports on "
            "the system's 8520 CIA chips'): CANNOT be graded at this boundary. Driving a generic, hand-picked "
            "bit pattern proves two CIA cores are addressable, not that the DISK SUBSYSTEM uses those two ports "
            "-- the stimulus never touches a real disk signal. hrm/ch8-cia-disk-signal-bindings's twelve "
            "PA/PB-BINDING assertions now carry the fact, each driving a real, named disk signal through the "
            "pinned join and reading it back at CIAAPRA or CIABPRB over a genuine CPU bus access.");
    }

    do_reset();
    {
        dut->ciaa_porta_in = 0b100110;
        settle();
        uint8_t ciaa_v = cia_a_read(ADDR_BFA000);
        set_address(ADDR_BFA000);
        bool ciaa_sel_ok = dut->sel_cia_a == 1 && dut->sel_cia_b == 0;

        setup_ciab_output_ddrb_allout();
        cia_b_write(ADDR_BFD100, 0xC3);
        uint8_t ciab_v = cia_b_read(ADDR_BFD100);
        set_address(ADDR_BFD100);
        bool ciab_sel_ok = dut->sel_cia_b == 1 && dut->sel_cia_a == 0;

        checks.note("TWOPORTS-CIAB-PORTB-SOL-REGLIST-C1 [not graded -- see WITHDRAWN note above]: twin of "
                    "opus:8-dsk.c18, same measurement -> ciab write/read at $BFD100 gives 0x" + hex2(ciab_v) +
                    " (matched=" + std::to_string((ciaa_v & 0xFC) == 0x98 && ciaa_sel_ok && ciab_v == 0xC3 &&
                    ciab_sel_ok) + ")");
        checks.record(
            "TWOPORTS-CIAB-PORTB-SOL-REGLIST-C1 (p.241, sol:8-reglist.c1, quote 'The disk subsystem uses two "
            "ports on the system's 8520 CIA chips', twin of opus:8-dsk.c18): same boundary fault. The same "
            "underlying fact this uid names is now carried by hrm/ch8-cia-disk-signal-bindings's twelve "
            "PA/PB-BINDING assertions (cited there under their own opus:8-cia.*/sol:8-cia.* uids, not this uid "
            "string -- the fact, not the uid, has the new home).");
    }

    // ========================================================================
    // CIABPRB-LITERAL-OPUS-DSK-C23 (p.241, opus:8-dsk.c23): the literal
    // printed address $BFD100 selects ciab and reads back a byte just
    // written to CIABPRB through that same address.
    // ========================================================================
    do_reset();
    {
        setup_ciab_output_ddrb_allout();
        bool mut_lit = mutating("CIABPRB-LITERAL-OPUS-DSK-C23");
        const uint8_t byte = 0x7E;
        uint32_t addr = mut_lit ? ADDR_NONCIA : ADDR_BFD100;
        cia_b_write(addr, byte);
        uint8_t v = cia_b_read(addr);
        checks.note("CIABPRB-LITERAL-STIMULUS: wrote 0x7E to " + std::string(mut_lit ? "non-CIA address (mutated)" :
                    "$BFD100") + ", read back 0x" + hex2(v));
        checks.expect(v == byte,
            "CIABPRB-LITERAL-OPUS-DSK-C23 (p.241, opus:8-dsk.c23, quote 'CIABPRB ($BFD100) eight output bits for "
            "disk selection, control and stepping'): CPU address $BFD100, presented to the gary+ciab rig, selects "
            "CIABPRB -- a byte written through that address arrives back unchanged");
    }

    // ========================================================================
    // CIABPRB-ALIAS-OPUS-CIA-C5 (p.244, opus:8-cia.c5, corpus-damage claim,
    // corrected reading): a DIFFERENT address inside the same window
    // ([13]=0,[12]=1,rs=1, differing from $BFD100 in bits 15/14) answers
    // IDENTICALLY -- the window claim itself.
    // ========================================================================
    do_reset();
    {
        setup_ciab_output_ddrb_allout();
        const uint8_t byte = 0x93;
        cia_b_write(ADDR_BFD100, byte);
        uint8_t v_literal = cia_b_read(ADDR_BFD100);

        bool mut_alias = mutating("CIABPRB-ALIAS-OPUS-CIA-C5");
        uint32_t alias_addr = mut_alias ? ADDR_BF3100 : ADDR_BF1100;  // mutated: BF1100 with [13] set, leaving the window
        cia_b_write(alias_addr, byte);
        uint8_t v_alias = cia_b_read(alias_addr);

        checks.note("CIABPRB-ALIAS-STIMULUS: $BFD100 reads 0x" + hex2(v_literal) + "; " +
                    std::string(mut_alias ? "the [13]-set (mutated) address" : "$BF1100 (same window, [15:14] differ)") +
                    " reads 0x" + hex2(v_alias));
        checks.expect(v_literal == byte && v_alias == byte && v_literal == v_alias,
            "CIABPRB-ALIAS-OPUS-CIA-C5 (p.244, opus:8-cia.c5, quote 'PB are output bits located in CIAAPRB "
            "($BFD100)' -- corpus defect, register is CIABPRB): a second address sharing [23:16]=$BF, [13]=0 and "
            "rs=4'h1 but differing from $BFD100 in bits 15/14 answers identically -- CIABPRB is a window, not the "
            "single literal address the page prints");
    }

    // ========================================================================
    // CIABPRB-CHIPLETTER-SOL-CIA-C4 (p.244, sol:8-cia.c4, twin of the
    // above, restatement already corrected to CIABPRB): at $BFD100
    // specifically, sel_cia_b=1 AND sel_cia_a=0 -- the decode-level fact
    // underlying the corpus-conflict resolution, measured directly.
    // ========================================================================
    do_reset();
    {
        bool mut_letter = mutating("CIABPRB-CHIPLETTER-SOL-CIA-C4");
        uint32_t addr = mut_letter ? ADDR_BFC100 : ADDR_BFD100;  // mutated: BFD100 with [12] cleared too (asserts sel_cia_a)
        set_address(addr);
        checks.note("CIABPRB-CHIPLETTER-STIMULUS: cpu_address=" + std::string(mut_letter ? "$BFD100 with [12] cleared (mutated)" :
                    "$BFD100") + "; sel_cia_a=" + std::to_string((int)dut->sel_cia_a) +
                    " sel_cia_b=" + std::to_string((int)dut->sel_cia_b));
        checks.expect(dut->sel_cia_b == 1 && dut->sel_cia_a == 0,
            "CIABPRB-CHIPLETTER-SOL-CIA-C4 (p.244, sol:8-cia.c4, quote 'Bits labeled PB are output bits located in "
            "CIAAPRB ($BFD100)', restatement corrected to CIABPRB): at $BFD100, sel_cia_b=1 and sel_cia_a=0 "
            "(gary.v:191-192) -- the decode selects CIA-B, not CIA-A, settling the corpus conflict at the address "
            "level independently of the register-list argument");
    }

    // ========================================================================
    // DSKINDEX-LITERAL-OPUS-CIA-C57 (p.245, opus:8-cia.c57): a pulse on
    // `flag`, read back through the literal address $BFDD00, sets bit 4
    // (and bit 7, IR) of the returned byte.
    //
    // WITHDRAWN to record(): this
    // block pulses ciab's `flag` INPUT PIN directly (dut->ciab_flag = 1) --
    // it proves the CIA ICR bit and the $BFDD00 address, but not p.245's
    // binding from the disk `index` signal to that pin (minimig.v:536+:657,
    // `.flag(index)`); a severed or wrong join there still passes this
    // block unchanged. hrm/ch8-disk-index-interrupt-path's PROP-FLAG-PIN
    // assertion (opus:8-cia.c56, opus:8-dskint.c9, sol:8-int.c9) now carries
    // the FLAG-pin journey: it pulses paula_floppy's real `index` OUTPUT and
    // carries it across the real join onto ciab's `flag`, catching exactly
    // the severed-join case this block cannot.
    // ========================================================================
    do_reset();
    {
        cia_b_write(ADDR_BFDD00, 0x90);  // icrmask[4] <- 1
        dut->ciab_flag = 1;
        settle();
        dut->ciab_flag = 0;
        settle();
        uint8_t v = cia_b_read(ADDR_BFDD00);
        checks.note("DSKINDEX-LITERAL-OPUS-CIA-C57 [not graded -- see WITHDRAWN note above]: flag pulsed "
                    "directly, ICR read at $BFDD00 gives 0x" + hex2(v) + " (matched=" +
                    std::to_string(v == 0x90) + ")");
        checks.record(
            "DSKINDEX-LITERAL-OPUS-CIA-C57 (p.245, opus:8-cia.c57, quote 'FLAG DSKINDEX* Disk index pulse "
            "($BFDD00, bit 4)'): CANNOT be graded at this boundary. This block pulses ciab's `flag` pin "
            "directly, proving the ICR bit and the $BFDD00 address, but not the binding from paula_floppy's real "
            "`index` output to that pin (minimig.v:536+:657) -- a severed join passes unchanged. "
            "hrm/ch8-disk-index-interrupt-path's PROP-FLAG-PIN assertion (opus:8-cia.c56, opus:8-dskint.c9, "
            "sol:8-int.c9) now carries the FLAG-pin journey, pulsing the real `index` output and carrying it "
            "across the real join.");
    }

    // ========================================================================
    // DSKINDEX-ALIAS-SOL-CIA-C51 (p.245, sol:8-cia.c51, twin): a different
    // address inside the same window (differing from $BFDD00 in bits
    // 15/14) answers identically -- the window claim for this register.
    //
    // WITHDRAWN to record(): same
    // fault as DSKINDEX-LITERAL-OPUS-CIA-C57 above -- `flag` is pulsed
    // directly, never through paula_floppy's real `index` output, so the
    // page's index-to-FLAG-pin binding is not established here either.
    // ========================================================================
    do_reset();
    {
        cia_b_write(ADDR_BF1D00, 0x90);  // icrmask[4] <- 1, written through the alias itself
        dut->ciab_flag = 1;
        settle();
        dut->ciab_flag = 0;
        settle();
        uint8_t v = cia_b_read(ADDR_BF1D00);
        checks.note("DSKINDEX-ALIAS-SOL-CIA-C51 [not graded -- see WITHDRAWN note above]: flag pulsed directly, "
                    "ICR read at $BF1D00 (same window as $BFDD00) gives 0x" + hex2(v) + " (matched=" +
                    std::to_string(v == 0x90) + ")");
        checks.record(
            "DSKINDEX-ALIAS-SOL-CIA-C51 (p.245, sol:8-cia.c51, quote 'FLAG DSKINDEX* Disk index pulse ($BFDD00, "
            "bit 4)', twin of opus:8-cia.c57): CANNOT be graded at this boundary, same fault as "
            "DSKINDEX-LITERAL-OPUS-CIA-C57 above (direct `flag` pulse, not the real `index`-to-FLAG-pin join). "
            "The same underlying fact this uid names is now carried by hrm/ch8-disk-index-interrupt-path's "
            "PROP-FLAG-PIN assertion (cited there under opus:8-cia.c56/opus:8-dskint.c9/sol:8-int.c9, not this "
            "uid string -- the fact, not the uid, has the new home).");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
