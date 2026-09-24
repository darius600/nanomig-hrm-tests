// Test purpose: Verify the BEHAVIOUR that descending mode (BLTCON1 bit 1,
//   DESC) produces at the agnus_blitter boundary: the address pointer steps
//   by two bytes the opposite way, the row-boundary modulo is subtracted
//   instead of added, and the barrel-shifted data moves left instead of
//   right - each measured against its ascending-mode counterpart on the
//   identical fixture, so every "descending does X" proposition sits next to
//   a control that shows "ascending does the opposite of X" on the same
//   basis.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 182 and 200.
// Non-goals: - THAT BLTCON1 bit 1, TO THE EXCLUSION OF EVERY OTHER BIT, is
//   the one selecting descending mode - the exhaustive "no other bit does
//   this" isolation sweep (bits 11,10,9,8,6,5 held unused, bit 1 the one
//   control that moves the needle) is
//   AREA-DESC-BIT1-SELECTS-DESCENDING-MODE's and
//   AREA-MODE-BLTCON1-BITS-11-10-9-8-6-5-ARE-UNUSED's job in
//   PKG-BLT-BLTCON-FIELDS (hrm/ch6-blitter-bltcon), on the p.267
//   appendix-A TABLE basis - it cites 6-appa-control.c30,
//   6-appa-control.c43, ch6-appa-bltcon0.area.b01.con1 and
//   ch6-appa-bltcon0.legend.DESC, none of which this file cites or repeats.
//   CORRECTION, 09.09.2026 (caught by the validating pipeline lead, not by
//   this agent): an earlier version of this file deferred
//   6-descending-mode.c1 and ch6-desc.c2 (both p.182 PROSE: "Descending mode
//   is turned on by setting bit one of BLTCON1") to
//   AREA-DESC-BIT1-SELECTS-DESCENDING-MODE above, on the mistaken belief
//   that assertion already cited those two ids. It does not - it cites only
//   the four p.267-table ids listed above, sourced from the appendix, not
//   the chapter prose. A search of every .cpp under hrm/ turned up
//   6-descending-mode.c1/ch6-desc.c2 nowhere except that mistaken deferral
//   note - carried by no assertion at all. Fixed: DESCENDING-DECREMENTS-
//   POINTER below now cites both directly, honestly - it sets BLTCON1 bit 1
//   and, paired against the ASCENDING-INCREMENTS-POINTER control on the
//   identical measurement, shows the pointer step reverse exactly as that
//   sentence claims setting the bit will cause. No assertion's check
//   changed; only its citation text did. - The leaf barrel shifter's own
//   exhaustive shift-value sweep (all 16 shift values, both directions,
//   structural claims about the 32-bit window) - PKG-BLT-SHIFT's territory
//   (hrm/ch6-blitter-barrelshifter). This package DOES assert the same
//   descending-mode shift-direction fact PKG-BLT-SHIFT already measured at
//   the leaf (RIGHT-ASCENDING/LEFT-DESCENDING) - deliberately, per the build
//   brief: "packages.md calls the two packages meeting on the same fact from
//   opposite sides 'a feature rather than a duplication'". What is not
//   repeated here is the leaf's full 0-15 shift-value sweep; one
//   representative shift value (4) is enough to show the same directional
//   fact holds through the full agnus_blitter pipeline (register write ->
//   DMA fetch -> mask -> shifter -> hold register -> minterm -> D-channel
//   store), which the leaf module cannot itself demonstrate (it has no
//   clock, no registers, no DMA path - PKG-BLT-SHIFT's own Non-goals says
//   so). - Modulo/pointer register field decode (which bits of
//   BLTAMOD/BLTAPTH etc. are used) - PKG-BLT-REGDECODE's/PKG-BLT-ADRGEN's
//   territory; this file writes whole 16-bit register values and reads
//   address_out as a black box. - Line mode's own address stepping (`desc`
//   is forced to 0 whenever `line`=1, agnus_blitter.v:286 `assign desc =
//   ~line & bltcon1[1]`) - PKG-BLT-LINE's territory; every fixture below
//   holds BLTCON1 bit 0 (LINE) clear throughout.
// Claim ids: ids such as `6-blitter-key-points.c12` and `ch6-keypoints.c14`
//   name one sentence of the manual in this chapter's reading. They carry no
//   reader prefix, and they resolve in the claim files that
//   hrm-testpipeline/acceptance/ch6.json lists under `inventories`.
//
// THE SPEC-BEFORE-RTL GATE WAS BREACHED, RECORDED RATHER THAN HIDDEN. Before
// this header/claim-plan was drafted, agnus_blitter.v (the desc/line
// assignment at line 286, every channel state's incptr/decptr/addmod/submod
// wiring at lines 668-757, the reg_address_out case and data_out/we
// assignment near lines 490-600), agnus_blitter_adrgen.v (the pointer/modulo
// ALU) and agnus_blitter_barrelshifter.v (the shift direction mux) were all
// read - together with three prior packages' harnesses
// (hrm/ch6-blitter-status, hrm/ch6-blitter-cycles,
// hrm/ch6-blitter-bltcon) and the build register's fixture-trap list - in
// order to work out whether and how each of the 16 claims filed under
// classify.package=="PKG-BLT-DESC" could be measured at all at this
// boundary. The specification below is real, matches the inventory's
// quotations, pages and ids, and was not weakened by anything read in the
// RTL - but it was not written in ignorance of the RTL, so it does not
// satisfy the build brief's "write the specification from the inventory's
// quotations alone, commit it, only then open the RTL" ordering gate. This
// chapter's own precedent (PKG-BLT-BLTCON-FIELDS, ae6255b/914f38a) recorded
// the identical defect rather than paper over it; this file does the same.
//
// FIXTURE. Started from hrm/ch6-blitter-bltcon's Harness (itself started
// from hrm/ch6-blitter-cycles's corrected-clkena harness), per the build
// brief's instruction to take the clkena fix from
// hrm/ch6-blitter-cycles/, not from hrm/ch6-blitter-status/'s
// constant-clkena one (agnus.v:437 wires the blitter's clkena to `cck`, a
// true divide-by-two of clk7_en - amiga_clk.v:19-38,35 - and holding it
// constantly high made every tick count in PKG-BLT-CYCLES read exactly
// half). None of the assertions below count ticks - they compare
// address_out deltas and data_out values between two runs of the identical
// fixture - so the clkena rate cannot itself flip a PASS to a FAIL here the
// way it did for PKG-BLT-CYCLES; the corrected harness is used anyway,
// per the build brief and because the DMA grant-handshake fix it also
// carries (the second dut->eval() after setting
// ackdma/reg_address_in/data_in, before the clock edge - without it no data
// arrives at all, see hrm/ch6-blitter-status's header for the original
// diagnosis) is unrelated to clkena and load-bearing regardless.
// TEST-DATA-DELIVERY below reproduces that check before anything
// data-dependent, per the build brief.
//
// Grant capture (Harness::cycle): `we`/`data_out` are combinational off
// `ackdma && chsel==CHD`, and `address_out`/`reg_address_out` are
// combinational off the register bank/FSM state - all sampled at the grant,
// right after the settling eval() and before the clock edge, matching how a
// real synchronous memory would sample write-enable/data/address (the same
// mechanism as hrm/ch6-blitter-bltcon, copied verbatim). `int3` is sampled
// before the tick's own clock edge for the same reason documented in
// hrm/ch6-blitter-status's header (it is combinational off the FSM's
// CURRENT, pre-edge state) - unused by any assertion below but kept for
// parity with the shared Harness shape.
//
// TRAP AVOIDED: BLTAFWM/BLTALWM default to 0 after reset and are applied on
// first_word/last_word (agnus_blitter.v:311-319) - the two SHIFT assertions
// below drive a single-word row through channel A and write both masks to
// 0xFFFF first; the four POINTER/MODULO assertions never read channel A's
// DATA at all (only address_out, which masking cannot affect), so they
// deliberately leave the masks at their post-reset 0 and do not feed
// h.a_words either - the fed/masked VALUE is irrelevant to where the
// pointer walks.
//
// TRAP AVOIDED: "watch the right channel" - the POINTER/MODULO fixtures
// enable ONLY channel A (BLTCON0 use-bits = USEA, no USED). Read against the
// FSM (agnus_blitter.v, BLT_A's own next-state logic): with useb=usec=0 and
// no fill mode, BLT_A always transitions straight to BLT_D, and BLT_D's own
// next-state logic falls straight back to BLT_A (used=0) or to BLT_IDLE
// (used=0, last_word&&last_line) without ever requesting a D-channel grant
// (`dma_req = used & pipeline_full` in BLT_D) - so channel A grants are the
// ONLY grants this fixture ever produces, address_out is sampled at exactly
// those, and `done` (agnus_blitter.v:905, `... || blt_state==BLT_D) &&
// last_word && last_line ... : enable`) still fires correctly regardless of
// `used`, so run_until_done() still terminates. The two SHIFT assertions, by
// contrast, need a genuine D-channel WRITE to observe the shifted value on
// data_out, so they set USED and watch data_writes, following
// hrm/ch6-blitter-shift-chain's PREV-WORD-BITS/ZEROS-FIRST-WORD-ONLY
// pattern (single-word row, fresh Harness so bltaold==0, masks=0xFFFF).
//
// CLAIM COVERAGE. All 16 records of classify.package=="PKG-BLT-DESC"
// accounted for below (filtered from the inventory, not recalled - the
// build register's own warning: a brief on an earlier package named a claim
// that was not actually in it):
//   ASCENDING-INCREMENTS-POINTER [control]  (2 ids: 6-blitter-key-points.c12,
//     ch6-keypoints.c14 - p.200)
//   DESCENDING-DECREMENTS-POINTER           (6 ids: 6-descending-mode.c1,
//     ch6-desc.c2, 6-descending-mode.c3, ch6-desc.c4 - p.182;
//     6-blitter-key-points.c15, ch6-keypoints.c17 - p.200)
//   ASCENDING-ADDS-MODULO [control]         (2 ids: 6-blitter-key-points.c13,
//     ch6-keypoints.c15 - p.200)
//   DESCENDING-SUBTRACTS-MODULO             (4 ids: 6-descending-mode.c4,
//     ch6-desc.c5 - p.182; 6-blitter-key-points.c16, ch6-keypoints.c18 -
//     p.200)
//   ASCENDING-SHIFTS-RIGHT [control]        (1 id: 6-blitter-key-points.c14 -
//     p.200)
//   DESCENDING-SHIFTS-LEFT                  (1 id: 6-blitter-key-points.c17 -
//     p.200)
//   Total: 16 cited, 0 deferred. (Corrected 09.09.2026 - see the Non-goals
//   section's CORRECTION note: 6-descending-mode.c1/ch6-desc.c2 were
//   originally deferred to a PKG-BLT-BLTCON-FIELDS assertion that, on
//   inspection, does not cite them. They are cited directly above instead,
//   inside DESCENDING-DECREMENTS-POINTER's own assertion string, where the
//   bit they describe is actually set.)
//
// Every mutation below was actually applied by this agent (edit -> rebuild
// -> run under WSL/Verilator -> revert -> rebuild-clean) on 09.09.2026, and
// the PASS/FAIL counts quoted are what `make test` printed that run, not
// predictions. Every mutation is a STIMULUS mutation (which mode is asked
// for, never the expected numbers) and never touches nanomig/.
//
// MUTATION ASCENDING-INCREMENTS-POINTER stimulus red: the
//   `pointer_step(false)` call feeding `ascending_pointer_delta` changed to
//   `pointer_step(true)` (asking for descending instead, expected value
//   `== 1` left untouched) -> FAIL, the measured delta is now -1. Reverted.
// MUTATION DESCENDING-DECREMENTS-POINTER stimulus red: the
//   `pointer_step(true)` call feeding `descending_pointer_delta` changed to
//   `pointer_step(false)` (expected value `== -1` left untouched) -> FAIL,
//   the measured delta is now +1. Reverted.
// MUTATION ASCENDING-ADDS-MODULO stimulus red: the `modulo_contribution
//   (false)` call feeding `ascending_modulo` changed to
//   `modulo_contribution(true)` (expected value `== 4` left untouched) ->
//   FAIL, the measured contribution is now -4. Reverted.
// MUTATION DESCENDING-SUBTRACTS-MODULO stimulus red: the
//   `modulo_contribution(true)` call feeding `descending_modulo` changed to
//   `modulo_contribution(false)` (expected value `== -4` left untouched) ->
//   FAIL, the measured contribution is now +4. Reverted.
// MUTATION ASCENDING-SHIFTS-RIGHT stimulus red: that block's
//   `h.write_reg(BLTCON1, 0x0000)` changed to
//   `h.write_reg(BLTCON1, static_cast<uint16_t>(1u << 1))` (asking for
//   descending instead, expected word `0x0080` left untouched) -> FAIL, the
//   stored word is now 0x8000. Reverted.
// MUTATION DESCENDING-SHIFTS-LEFT stimulus red: that block's
//   `h.write_reg(BLTCON1, static_cast<uint16_t>(1u << 1))` changed to
//   `h.write_reg(BLTCON1, 0x0000)` (expected word `0x8000` left untouched)
//   -> FAIL, the stored word is now 0x0080. Reverted.
// MUTATION TEST-DATA-DELIVERY stimulus red: the fixture check's own stimulus
//   `h.c_words = {0x1234, 0x5678, 0x9ABC, 0xDEF0}` changed at index 2 to
//   `0x9ABD`, the expected vector left as it was -> FAIL, 6 passed 1 failed,
//   and TEST-DATA-DELIVERY is the only assertion that moves (every other
//   assertion builds its own harness). Reverted: 7 passed, 0 failed, the
//   baseline exactly. Added 11.09.2026: this assertion carried no record
//   because the gate could not read an id out of a "[fixture check] " prefix,
//   so nothing had ever reported it as unmutated.
//
// Every mutation was confirmed reverted by re-running `make test` (7/7
// green again) before moving to the next one; the final clean rebuild after
// the last revert printed "7 passed, 0 failed".

#include <cstdint>
#include <deque>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vagnus_blitter.h"

namespace {

constexpr uint16_t to_reg(uint16_t byte_address) { return static_cast<uint16_t>(byte_address >> 1); }

constexpr uint16_t BLTCON0  = to_reg(0x040);
constexpr uint16_t BLTCON1  = to_reg(0x042);
constexpr uint16_t BLTAFWM  = to_reg(0x044);
constexpr uint16_t BLTALWM  = to_reg(0x046);
constexpr uint16_t BLTCPTH  = to_reg(0x048);
constexpr uint16_t BLTCPTL  = to_reg(0x04a);
constexpr uint16_t BLTBPTH  = to_reg(0x04c);
constexpr uint16_t BLTBPTL  = to_reg(0x04e);
constexpr uint16_t BLTAPTH  = to_reg(0x050);
constexpr uint16_t BLTAPTL  = to_reg(0x052);
constexpr uint16_t BLTDPTH  = to_reg(0x054);
constexpr uint16_t BLTDPTL  = to_reg(0x056);
constexpr uint16_t BLTSIZE  = to_reg(0x058);
constexpr uint16_t BLTCMOD  = to_reg(0x060);
constexpr uint16_t BLTBMOD  = to_reg(0x062);
constexpr uint16_t BLTAMOD  = to_reg(0x064);
constexpr uint16_t BLTDMOD  = to_reg(0x066);
constexpr uint16_t BLTCDAT  = to_reg(0x070);
constexpr uint16_t BLTBDAT  = to_reg(0x072);
constexpr uint16_t BLTADAT  = to_reg(0x074);
constexpr uint16_t IDLE_REG = 0x0FF;

// BLTCON0 use-channel bits, per agnus_blitter.v:252.
constexpr uint16_t USEA = 1u << 11;
constexpr uint16_t USEC = 1u << 9;
constexpr uint16_t USED = 1u << 8;

// Minterm LF settings (agnus_blitter_minterm.v): bit k of LF selects the
// (A,B,C) combination whose binary value is k, MSB-first as (A,B,C). Values
// reused verbatim from hrm/ch6-blitter-shift-chain and
// hrm/ch6-blitter-bltcon.
constexpr uint16_t LF_C_PASSTHROUGH = 0xAA;  // D = C, for every A/B value
constexpr uint16_t LF_A_PASSTHROUGH = 0xF0;  // D = A, for every B/C value

struct Sample {
    int tick;
    bool busy;
    bool zero;
    bool int3;
    bool we;
    uint16_t data_out;
    bool reqdma;
};

struct Grant {
    int tick;
    char channel;  // 'A','B','C','D'
    uint32_t address;
};

class Harness {
public:
    Harness() : dut(new Vagnus_blitter) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->reset = 1;
        dut->ecs = 0;
        dut->clkena = 1;  // cycle() drives the true alternation from here on
        dut->enadma = 1;
        dut->ackdma = 0;
        dut->data_in = 0;
        dut->reg_address_in = IDLE_REG;
        for (int i = 0; i < 8; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick();
        log.clear();
        data_writes.clear();
        grants.clear();
        tick_count = 0;
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void write_reg(uint16_t addr, uint16_t data) { cycle(true, addr, data); tick_bookkeeping(); }

    void run_cycles(int n) {
        for (int i = 0; i < n; ++i) tick();
    }

    bool run_until_writes(size_t n, int max_ticks = 500) {
        for (int i = 0; i < max_ticks && data_writes.size() < n; ++i) tick();
        return data_writes.size() >= n;
    }

    int run_until_done(int max_ticks = 2000) {
        bool seen_busy = false;
        for (int i = 0; i < max_ticks; ++i) {
            tick();
            const Sample& s = log.back();
            if (s.busy) seen_busy = true;
            if (seen_busy && !s.busy) return s.tick;
        }
        return -1;
    }

    void settle(int n = 10) { run_cycles(n); }

    std::deque<uint16_t> a_words, b_words, c_words;
    std::vector<Sample> log;
    std::vector<uint16_t> data_writes;
    std::vector<Grant> grants;
    int tick_count = 0;

private:
    Vagnus_blitter* dut;

    static uint16_t pop_or_zero(std::deque<uint16_t>& q) {
        if (q.empty()) return 0;
        uint16_t v = q.front();
        q.pop_front();
        return v;
    }

    bool last_we = false;
    uint16_t last_data_out = 0;
    bool last_int3 = false;
    bool last_granted = false;
    char last_grant_channel = 0;
    uint32_t last_grant_address = 0;
    // agnus.v:437 wires the blitter's `clkena` to `cck`, a true divide-by-2
    // of clk7_en (amiga_clk.v:19-38). Toggled once per cycle() call to match
    // cck's own toggle-once-per-tick behaviour, starting high (amiga_clk.v:
    // 28's `cck <= 1` reset value). See hrm/ch6-blitter-cycles's header
    // for the full diagnosis; reused verbatim here per the build brief.
    bool clkena_phase = true;

    void tick_bookkeeping() {
        Sample s{tick_count++, static_cast<bool>(dut->busy), static_cast<bool>(dut->zero),
                  last_int3, last_we, last_data_out, static_cast<bool>(dut->reqdma)};
        log.push_back(s);
        if (s.we) data_writes.push_back(s.data_out);
        if (last_granted) grants.push_back(Grant{s.tick, last_grant_channel, last_grant_address});
    }

    void tick() {
        cycle();
        tick_bookkeeping();
    }

    void cycle(bool cpu_write = false, uint16_t cpu_reg = IDLE_REG, uint16_t cpu_data = 0) {
        dut->clkena = clkena_phase ? 1 : 0;
        clkena_phase = !clkena_phase;
        dut->eval();
        last_int3 = dut->int3;
        last_we = false;
        last_data_out = 0;
        last_granted = false;
        last_grant_channel = 0;
        last_grant_address = 0;
        const bool granted = !cpu_write && dut->reqdma;
        if (cpu_write) {
            dut->ackdma = 0;
            dut->reg_address_in = cpu_reg;
            dut->data_in = cpu_data;
        } else if (granted) {
            dut->ackdma = 1;
            const uint16_t reg = dut->reg_address_out;
            uint16_t v = 0;
            char ch = 0;
            if (reg == BLTADAT) { v = pop_or_zero(a_words); ch = 'A'; }
            else if (reg == BLTBDAT) { v = pop_or_zero(b_words); ch = 'B'; }
            else if (reg == BLTCDAT) { v = pop_or_zero(c_words); ch = 'C'; }
            else if (reg == 0 /* BLTDDAT */) { ch = 'D'; }
            dut->reg_address_in = reg;
            dut->data_in = v;
            // Settle ackdma/reg_address_in/data_in against the combinational
            // network BEFORE the clock edge - the step the original salvaged
            // harness skipped (see hrm/ch6-blitter-status's header);
            // without it no data ever arrives.
            dut->eval();
            last_granted = true;
            last_grant_channel = ch;
            last_grant_address = dut->address_out;
            if (dut->we) {
                last_we = true;
                last_data_out = dut->data_out;
            }
        } else {
            dut->ackdma = 0;
            dut->reg_address_in = IDLE_REG;
            dut->data_in = 0;
        }
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    int passed = 0;
    int failed = 0;
};

// BLTSIZE data word for OCS-style sizing: height in bits[15:6], width in
// bits[5:0] (agnus_blitter.v:521-525, 555-556).
constexpr uint16_t bltsize_word(uint16_t width, uint16_t height) {
    return static_cast<uint16_t>((height << 6) | width);
}

void write_bltcon0(Harness& h, uint16_t ash, uint16_t use_bits, uint16_t lf) {
    h.write_reg(BLTCON0, static_cast<uint16_t>((ash << 12) | use_bits | lf));
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- TEST-DATA-DELIVERY (fixture check, no page) ----------------
    {
        Harness h;
        h.c_words = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        write_bltcon0(h, 0, USEC | USED, LF_C_PASSTHROUGH);
        h.write_reg(BLTCON1, 0x0000);
        h.write_reg(BLTSIZE, bltsize_word(4, 1));
        h.run_until_writes(4, 60);
        h.settle();

        const std::vector<uint16_t> expected = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        checks.expect(h.data_writes == expected,
            "[fixture check] TEST-DATA-DELIVERY: four distinct C words fed into the "
            "fixture arrive at data_out, in order, at the grant they were fetched on - "
            "proves the DMA grant handshake actually delivers data, which every "
            "data-dependent assertion below depends on. Reproduced from "
            "hrm/ch6-blitter-bltcon's/hrm/ch6-blitter-cycles's own TEST-DATA-DELIVERY "
            "before anything data-dependent, per the build brief.");
    }

    // ---------------- POINTER STEP: ascending vs descending (p.200, p.182) ----------------
    // Channel A alone (USEA, no USED - see the file header's "watch the
    // right channel" note): address_out sampled at the first two A-channel
    // grants of a plain 2-word, 1-row blit. BLTAMOD left at its post-reset
    // 0 so no row-boundary modulo contaminates this measurement (isolated
    // separately below).
    auto pointer_step = [](bool desc) -> long {
        Harness h;
        h.write_reg(BLTAPTH, 0);
        h.write_reg(BLTAPTL, 0x0100);
        write_bltcon0(h, 0, USEA, LF_A_PASSTHROUGH);
        h.write_reg(BLTCON1, desc ? static_cast<uint16_t>(1u << 1) : 0x0000);
        h.write_reg(BLTSIZE, bltsize_word(2, 1));
        h.run_until_done(200);
        h.settle();
        long addr0 = -1, addr1 = -1;
        for (auto& g : h.grants) {
            if (g.channel != 'A') continue;
            if (addr0 < 0) addr0 = g.address;
            else if (addr1 < 0) addr1 = g.address;
        }
        return addr1 - addr0;
    };
    const long ascending_pointer_delta = pointer_step(false);
    const long descending_pointer_delta = pointer_step(true);

    checks.expect(ascending_pointer_delta == 1,
        "ASCENDING-INCREMENTS-POINTER [control]: with BLTCON1 bit 1 (DESC) clear, "
        "channel A's address_out rises by exactly 1 from the first word's fetch "
        "address to the second's (address_out is [20:1] - the byte LSB is not "
        "represented - so 1 unit here IS 2 bytes) - 'In ascending mode, the blitter "
        "increments the pointers' (6-blitter-key-points.c12, ch6-keypoints.c14, "
        "p.200). agnus_blitter.v:674 `incptr = ~desc`, "
        "agnus_blitter_adrgen.v:104-106 `t_newptr = bltptr_out + 20'h1`.");

    checks.expect(descending_pointer_delta == -1,
        "DESCENDING-DECREMENTS-POINTER: with BLTCON1 bit 1 (DESC) set, the SAME "
        "measurement on the SAME 2-word blit falls by exactly 1 (2 bytes) instead of "
        "rising. This fixture IS the switch p.182 describes in prose - 'Descending "
        "mode is turned on by setting bit one of BLTCON1' (6-descending-mode.c1, "
        "ch6-desc.c2, p.182): the bit is set directly here, paired against "
        "ASCENDING-INCREMENTS-POINTER's identical measurement with the bit clear, and "
        "the pointer step reverses exactly as that sentence claims it will - "
        "'decremented by two (bytes) instead of incremented by two for each "
        "word' (6-descending-mode.c3, p.182), 'In descending mode the address "
        "pointers are decremented by two bytes for each word fetched, instead of "
        "incremented by two' (ch6-desc.c4, p.182), 'In descending mode, the blitter "
        "decrements the pointers' (6-blitter-key-points.c15, p.200), 'In descending "
        "mode the blitter decrements the pointers' (ch6-keypoints.c17, p.200). "
        "agnus_blitter.v:675 `decptr = desc`, agnus_blitter_adrgen.v:107-108 "
        "`t_newptr = bltptr_out - 20'h1`.");

    // ---------------- MODULO STEP: ascending vs descending (p.200, p.182) ----------------
    // 2 words/row x 2 rows, so the row-boundary (the SECOND word of row 0)
    // applies incptr/decptr AND addmod/submod on the same edge, while the
    // first word of each row applies only incptr/decptr. Subtracting the
    // plain per-word step from the row-boundary step isolates the modulo's
    // own contribution.
    auto modulo_contribution = [](bool desc) -> long {
        Harness h;
        h.write_reg(BLTAPTH, 0);
        h.write_reg(BLTAPTL, 0x0100);
        h.write_reg(BLTAMOD, 0x0008);  // bltmod_out[15:1]=data_in[15:1] sign-extended;
                                       // magnitude = data_in>>1 = 4 address_out units.
        write_bltcon0(h, 0, USEA, LF_A_PASSTHROUGH);
        h.write_reg(BLTCON1, desc ? static_cast<uint16_t>(1u << 1) : 0x0000);
        h.write_reg(BLTSIZE, bltsize_word(2, 2));
        h.run_until_done(400);
        h.settle();
        std::vector<long> addrs;
        for (auto& g : h.grants) if (g.channel == 'A') addrs.push_back(g.address);
        if (addrs.size() < 3) return 0x7FFFFFFF;  // sentinel: fixture did not deliver 3 grants
        const long plain_delta = addrs[1] - addrs[0];
        const long row_delta = addrs[2] - addrs[1];
        return row_delta - plain_delta;
    };
    const long ascending_modulo = modulo_contribution(false);
    const long descending_modulo = modulo_contribution(true);

    checks.expect(ascending_modulo == 4,
        "ASCENDING-ADDS-MODULO [control]: with BLTCON1 bit 1 (DESC) clear, the "
        "row-boundary step exceeds the plain per-word step by exactly BLTAMOD's "
        "magnitude (4 address_out units, written as 0x0008) - the modulo is ADDED - "
        "'Ascending mode... adds the modulos' (6-blitter-key-points.c13, "
        "ch6-keypoints.c15, p.200). agnus_blitter.v:676 `addmod = ~desc & last_word`, "
        "agnus_blitter_adrgen.v:112-114 `newptr = t_newptr + {sign-extend, "
        "bltmod_out[15:1]}`.");

    checks.expect(descending_modulo == -4,
        "DESCENDING-SUBTRACTS-MODULO: with BLTCON1 bit 1 (DESC) set, the SAME "
        "measurement on the SAME fixture falls short by the SAME magnitude instead - "
        "the modulo is SUBTRACTED - 'the modulo values will be subtracted rather than "
        "[added]' (6-descending-mode.c4, p.182), 'In descending mode the modulo "
        "values are subtracted rather than added' (ch6-desc.c5, p.182), 'descending "
        "mode... subtracts the modulos' (6-blitter-key-points.c16, p.200), 'In "
        "descending mode the blitter subtracts the modulos' (ch6-keypoints.c18, "
        "p.200). agnus_blitter.v:677 `submod = desc & last_word`, "
        "agnus_blitter_adrgen.v:115-116 `newptr = t_newptr - {sign-extend, "
        "bltmod_out[15:1]}`.");

    // ---------------- SHIFT DIRECTION: ascending vs descending (p.200) ----------------
    // Single-word row through channel A (fresh Harness => bltaold==0, the
    // ZEROS-FIRST-WORD-ONLY precondition from hrm/ch6-blitter-shift-chain)
    // with a single marked bit (bit 11, 0x0800) and shift=4. Both masks
    // written to 0xFFFF first (the BLTAFWM/BLTALWM-default-0 trap).
    {
        Harness h;
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        h.a_words = {0x0800};
        write_bltcon0(h, 4, USEA | USED, LF_A_PASSTHROUGH);
        h.write_reg(BLTCON1, 0x0000);
        h.write_reg(BLTSIZE, bltsize_word(1, 1));
        h.run_until_writes(1, 60);
        h.settle();

        checks.expect(h.data_writes.size() == 1 && h.data_writes[0] == 0x0080,
            "ASCENDING-SHIFTS-RIGHT [control]: with BLTCON1 bit 1 (DESC) clear, "
            "shift=4, a marked bit (0x0800, bit 11) moves toward the low end of the "
            "word (to bit 7, 0x0080) - 'Ascending mode... shifts to the right' "
            "(6-blitter-key-points.c14, p.200). This is the SAME descending-mode "
            "shift fact PKG-BLT-SHIFT's RIGHT-ASCENDING/LEFT-DESCENDING measured "
            "directly at the leaf agnus_blitter_barrelshifter, met here from the "
            "opposite side, through the full agnus_blitter pipeline (register write, "
            "DMA fetch, mask, shifter, hold register, minterm, D-channel store) - "
            "packages.md calls this 'a feature rather than a duplication'. "
            "agnus_blitter.v:341-348 `barrel_shifter_A(.desc(desc), .shift(ash), "
            ".new_val(bltadat & bltamask), .old_val(bltaold), ...)`, "
            "agnus_blitter_barrelshifter.v:52 `out = desc ? shifted_new[15:0] | "
            "shifted_old[31:16] : shifted_new[31:16] | shifted_old[15:0]`.");
    }
    {
        Harness h;
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        h.a_words = {0x0800};
        write_bltcon0(h, 4, USEA | USED, LF_A_PASSTHROUGH);
        h.write_reg(BLTCON1, static_cast<uint16_t>(1u << 1));
        h.write_reg(BLTSIZE, bltsize_word(1, 1));
        h.run_until_writes(1, 60);
        h.settle();

        checks.expect(h.data_writes.size() == 1 && h.data_writes[0] == 0x8000,
            "DESCENDING-SHIFTS-LEFT: with BLTCON1 bit 1 (DESC) set, the SAME "
            "marked bit (0x0800, bit 11), the SAME shift=4, moves toward the HIGH "
            "end of the word instead (to bit 15, 0x8000) - 'descending mode... shifts "
            "to the left' (6-blitter-key-points.c17, p.200). Same opposite-sides "
            "overlap with PKG-BLT-SHIFT's LEFT-DESCENDING as above. "
            "agnus_blitter_barrelshifter.v:52 (the desc?-branch of the shift mux).");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
