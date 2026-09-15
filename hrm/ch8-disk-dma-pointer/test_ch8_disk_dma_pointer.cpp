// Test purpose: HRM conformance for ch8-disk-dma-pointer -- DSKPTH ($DFF020)
// / DSKPTL ($DFF022), the disk DMA pointer, at the `agnus_diskdma` boundary,
// per SPECIFICATION.md. Headline: P4A/P4B grade finding candidates C7 and
// C14 -- the pointer's high field is five bits wide and loads
// unconditionally (no `ecs` port exists on this module at all), so both the
// printed "high 3 bits" (OCS) reading and the "(E)"/"5 bits if ECS"
// (conditional-difference) reading fail, for the same single cause.
// Test class: HRM_CONFORMANCE
// Primary oracle: the manual quotations cited inline (see SPECIFICATION.md
// for full citations and page numbers), not the RTL. Red results at P4A,
// P4B, P11A and P11B are valid, finished outcomes -- they are not adjusted
// to pass.
// Non-goals (see SPECIFICATION.md "Not turned into assertions"): the
// CPU-access-size half of "a single long word" (not observable at
// reg_address_in, which sees two ordinary 16-bit writes regardless of the
// originating instruction); the "by a processor/Copper" requester-identity
// half of the reload claims (reg_address_in carries an address and a value,
// no requester identity); the "pointing to chip memory" half of the `+`
// mark (a `gary`-level fact, out of this boundary). This package also does
// not grade the exclusivity ("only") of DSKDATR/DSKDAT to the disk DMA
// channel -- that pair of claims belongs to the sibling package
// ch8-disk-dma-data-channel, not to this one; P10 here grades only the
// directional fact its own two claims state.
//
// Fixture: the unmodified agnus_diskdma.v, compiled standalone. No reset
// port exists on this module (confirmed by its port list) -- relevant to
// P9.
//
// address_out is declared [20:1] (20 bits). As a plain Verilated integer,
// named bit 1 is integer bit 0 and named bit 20 is integer bit 19, so the
// DSKPTH-loaded field (address_out[20:16]) is (address_out>>15)&0x1F and
// the DSKPTL-loaded field (address_out[15:1]) is address_out&0x7FFF. This
// was cross-checked against the classify-stage measurement before any code
// was written: DSKPTH<=0x0007, DSKPTL<=0x2468 packs to 0x39234 by this
// formula, matching opus:8-dsk.c32's MEASURED value exactly.
//
// MUTATION records: see the bottom of this header, filled in after each
// mutation was actually built and run (BUILD-BRIEF: baseline colour must
// stand beside the mutation result).
//
// MUTATION LIVENESS stimulus red: both DSKPTH/DSKPTL writes changed to
// 0x0000 (from 0x0015/0x2C6A), expected value left computed from the
// original constants. Baseline green. Went red (address_out read 0 against
// a nonzero expected). Reverted, rebuilt, back to green.
// MUTATION P1 stimulus red: the probe value was written to $022 instead of
// $020. Baseline green. Went red (address_out's high field stayed at its
// established prior value instead of the probe's low 5 bits). Reverted,
// rebuilt, back to green.
// MUTATION P2 stimulus red: the probe value was written to $020 instead of
// $022. Baseline green. Went red (address_out's low field stayed at its
// established prior value). Reverted, rebuilt, back to green.
// MUTATION P3 none impossible: unfalsifiable at this boundary. The compiled
// module's port list (reg_address_in, data_in, address_out, reg_address_out,
// dmal, dmas, dma, wr, hpos, speed) has no data-output signal at all; no
// reachable stimulus can manufacture one to read a write back through.
// MUTATION P4A oracle red->green: the compared relationship was substituted
// from "predicts equality" (the page's 3-bit model: bits 3/4 of the write
// have no effect, so writing 0x1F and 0x07 to DSKPTH should read back
// identically) to "predicts inequality" (the classify-stage 5-bit model).
// Baseline red (the equality the 3-bit model predicts does not hold: 0x1F
// and 0x07 read back different fields). Substituted oracle went green (the
// inequality the 5-bit model predicts does hold), showing the comparator
// itself discriminates and P4A's red is a genuine page/RTL mismatch, not a
// broken harness. Reverted, rebuilt, back to red baseline.
// MUTATION P4B oracle red->green: the compared relationship was substituted
// from "predicts a difference across dmas/speed/hpos states" (what a real
// chipset-conditioning input would allow) to "predicts no difference"
// (the classify-stage unconditional model). Baseline red (no difference is
// observed, so the conditional-difference prediction fails). Substituted
// oracle went green (the no-difference prediction holds). Reverted,
// rebuilt, back to red baseline.
// MUTATION P5 stimulus red: one bit of the value written to DSKPTL was
// flipped from the value the expected computation still uses. Baseline
// green. Went red. Reverted, rebuilt, back to green.
// MUTATION P6 stimulus red: the second DSKPTL write was changed to differ
// from the first in more than just bit 0 (0x246B instead of 0x2469).
// Baseline green (the two fields the fixed pair 0x2468/0x2469 produce are
// equal, since only bit 0 differs and bit 0 is discarded). Went red (0x246B
// also differs in bit 1, which is not discarded, so the fields differ).
// Reverted, rebuilt, back to green.
// MUTATION P7 stimulus red: a DSKPTL write was added immediately after the
// DSKPTH-alone write this check depends on, contaminating what "alone"
// means. Baseline green. Went red (the low field was no longer preserved).
// Reverted, rebuilt, back to green.
// MUTATION P8 stimulus red: dmal held low throughout instead of high, so no
// DMA cycle is ever granted. Baseline green (3 of 3 granted cycles observed,
// each address_out matching the running next-word counter). Went red (0 of
// 3 grants observed). Reverted, rebuilt, back to green.
// MUTATION P9 stimulus red: dmal was left asserted through the
// post-DMA-stop observation window instead of withdrawn. Baseline green
// (address_out held constant across 45 idle ticks, more than one full slot
// period). Went red (address_out kept advancing, because a grant recurred
// once hpos wrapped back through 0x19). Reverted, rebuilt, back to green.
// MUTATION P10 stimulus red: the `dmas` values used for the two trials were
// swapped (1 then 0 instead of 0 then 1), expected constants left
// unchanged. Baseline green. Went red (each trial now read the other
// trial's expected reg_address_out). Reverted, rebuilt, back to green.
// MUTATION P11A oracle red->green: the compared width was substituted from
// 32 (the page's claim) to 20 (the classify-stage measured width). Baseline
// red (32 bits are not reachable; the all-ones value tops out at 20 bits).
// Substituted oracle went green (20 bits matches). Reverted, rebuilt, back
// to red baseline.
// MUTATION P11B oracle red->green: the compared width was substituted from
// 18 (the appendix's claim) to 20 (5+15, the classify-stage measured
// width). Baseline red. Substituted oracle went green. Reverted, rebuilt,
// back to red baseline.
//
// Run tally, baseline (no -D flags): 10 passed, 4 failed (P4A, P4B, P11A,
// P11B red by design; all others green).

#include <verilated.h>
#include "Vagnus_diskdma.h"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

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

// register addresses, as [8:1] -- see agnus_diskdma.v:21-24
static const uint8_t DSKPTH_ADDR  = 0x10; // 9'h020[8:1]
static const uint8_t DSKPTL_ADDR  = 0x11; // 9'h022[8:1]
static const uint8_t DSKDAT_ADDR  = 0x13; // 9'h026[8:1]
static const uint8_t DSKDATR_ADDR = 0x04; // 9'h008[8:1]
static const uint8_t IDLE_ADDR    = 0xFF; // unused, matches neither register

// Returns the hex digits only (no "0x" prefix) -- callers prepend "0x"
// themselves so a literal "0x" is never duplicated in a message.
static std::string hex32(uint32_t v) {
    char buf[16];
    snprintf(buf, sizeof buf, "%05X", v);
    return std::string(buf);
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus_diskdma* top = new Vagnus_diskdma;
    Checks checks;

    top->clk = 0;
    top->clk7_en = 1;
    top->dmal = 0;
    top->dmas = 0;
    top->speed = 0;
    top->hpos = 0;
    top->reg_address_in = IDLE_ADDR;
    top->data_in = 0;

    auto tick = [&]() {
        top->eval();
        top->clk = 1; top->eval();
        top->clk = 0; top->eval();
    };

    // A register write happens on the next posedge if reg_address_in/data_in
    // are held stable across it. dmal is forced 0 here so `dma` cannot be
    // asserted during a write (agnus_diskdma.v:56: a granted cycle steals
    // the load path for the increment instead of the write value).
    auto write_reg = [&](uint8_t addr, uint16_t data) {
        top->dmal = 0;
        top->reg_address_in = addr;
        top->data_in = data;
        tick();
        top->reg_address_in = IDLE_ADDR;
        top->data_in = 0;
    };

    auto high_field = [&]() -> uint32_t { return (top->address_out >> 15) & 0x1F; };
    auto low_field  = [&]() -> uint32_t { return top->address_out & 0x7FFF; };

    // --- LIVENESS: two distinct, recognisable values into the two fields,
    // confirm both arrive, neither stuck, neither swapped. ---
    {
        const uint16_t dskpth_live = 0x0015; // low 5 bits = 10101
        const uint16_t dskptl_live = 0x2C6A; // arbitrary nonzero pattern
        uint32_t expected = ((dskpth_live & 0x1F) << 15) | ((dskptl_live >> 1) & 0x7FFF);

#ifdef MUT_LIVENESS
        write_reg(DSKPTH_ADDR, 0x0000);
        write_reg(DSKPTL_ADDR, 0x0000);
#else
        write_reg(DSKPTH_ADDR, dskpth_live);
        write_reg(DSKPTL_ADDR, dskptl_live);
#endif
        checks.note("LIVENESS: DSKPTH<=0x" + hex32(dskpth_live) + ", DSKPTL<=0x" +
                     hex32(dskptl_live) + " -> address_out=0x" + hex32(top->address_out) +
                     " (expected 0x" + hex32(expected) + ")");
        checks.expect(top->address_out == expected,
            "LIVENESS (fixture delivers two distinct known words before any proposition is graded): "
            "address_out=0x" + hex32(top->address_out) + " expected 0x" + hex32(expected));
    }

    // --- P1: DSKPTH decodes at $DFF020 -- a write there, and only there,
    // reaches address_out[20:16]. ---
    {
        write_reg(DSKPTH_ADDR, 0x0000); // clean the high field first
        uint32_t before = high_field();
        const uint16_t probe = 0x0015; // low 5 bits nonzero, recognisable
#ifdef MUT_P1
        write_reg(DSKPTL_ADDR, probe); // wrong address on purpose
#else
        write_reg(DSKPTH_ADDR, probe);
#endif
        uint32_t after = high_field();
        checks.expect(before == 0x00 && after == (probe & 0x1F),
            "P1 (opus:8-dsk.c32 p.241, opus:8-dma.c11/sol:8-dskpth.c7 p.246, "
            "opus:8-appA-dskpt.c1/sol:8-appa-dskpth.c1 p.279, "
            "opus:8-appB-dskpth.c1/sol:8-appb-dskpth.c1 p.289): a write at $DFF020 reaches "
            "address_out[20:16] -- before=0x" + hex32(before) +
            " after=0x" + hex32(after) + " expected 0x" + hex32(probe & 0x1F));
    }

    // --- P2: DSKPTL decodes at $DFF022 -- a write there, and only there,
    // reaches address_out[15:1]. ---
    {
        write_reg(DSKPTL_ADDR, 0x0000); // clean the low field first
        uint32_t before = low_field();
        const uint16_t probe = 0x3AC4; // arbitrary nonzero pattern in bits[15:1]
#ifdef MUT_P2
        write_reg(DSKPTH_ADDR, probe); // wrong address on purpose
#else
        write_reg(DSKPTL_ADDR, probe);
#endif
        uint32_t after = low_field();
        uint32_t expected_low = (probe >> 1) & 0x7FFF;
        checks.expect(before == 0x0000 && after == expected_low,
            "P2 (opus:8-appA-dskpt.c6/sol:8-appa-dskptl.c1 p.279, "
            "opus:8-appB-dskptl.c1/sol:8-appb-dskptl.c1 p.289): a write at $DFF022 reaches "
            "address_out[15:1] -- before=0x" + hex32(before) + " after=0x" + hex32(after) +
            " expected 0x" + hex32(expected_low));
    }

    // --- P3: write-only -- structural, no stimulus can falsify it (no
    // data-output port exists on this module at all). ---
    checks.expect(true,
        "P3 (opus:8-appA-dskpt.c2/sol:8-appa-dskpth.c2 p.279 DSKPTH; "
        "opus:8-appA-dskpt.c7/sol:8-appa-dskptl.c2 p.279 DSKPTL; "
        "opus:8-appB-dskpth.c4/sol:8-appb-dskpth.c6 p.289; "
        "opus:8-appB-dskptl.c4/sol:8-appb-dskptl.c6 p.289): agnus_diskdma's compiled port "
        "list (reg_address_in, data_in, address_out, reg_address_out, dmal, dmas, dma, wr, "
        "hpos, speed) has no data-output port -- DSKPTH/DSKPTL cannot be read back through "
        "this boundary, structurally, not as a result of any stimulus applied here");

    // --- P4A/P4B (headline, C7/C14): both readings of the appendix row
    // fail, for the same cause -- the load is unconditionally five bits. ---
    {
        write_reg(DSKPTH_ADDR, 0x1F); // all 5 low bits set
        uint32_t field_1F = high_field();
        write_reg(DSKPTH_ADDR, 0x07); // only the low 3 bits set (bits 3,4 clear)
        uint32_t field_07 = high_field();
        checks.note("P4A: DSKPTH<=0x1F -> high field=0x" + hex32(field_1F) +
                     "; DSKPTH<=0x07 -> high field=0x" + hex32(field_07));
#ifdef MUT_P4A
        checks.expect(field_1F != field_07,
            "P4A oracle-substituted (5-bit model: bits 3/4 of the write DO reach "
            "address_out) -- field(0x1F)=0x" + hex32(field_1F) + " field(0x07)=0x" + hex32(field_07));
#else
        checks.expect(field_1F == field_07,
            "P4A (opus:8-appA-dskpt.c4/sol:8-appa-dskpth.c4 p.279, OCS 'high 3 bits' "
            "reading [C7]): if only 3 of 5 written bits reached address_out, bits 3 and 4 "
            "of the write would have no effect, so writing 0x1F and 0x07 to DSKPTH should "
            "read back identically -- field(0x1F)=0x" + hex32(field_1F) + " field(0x07)=0x" +
            hex32(field_07) + " (they differ)");
#endif

        // P4B: no chipset-conditioning input exists; the same write gives the
        // same field regardless of every other real input on this boundary.
        top->dmas = 0; top->speed = 0; top->hpos = 0;
        write_reg(DSKPTH_ADDR, 0x1F);
        uint32_t field_stateA = high_field();
        top->dmas = 1; top->speed = 1; top->hpos = 0x1FF;
        write_reg(DSKPTH_ADDR, 0x1F);
        uint32_t field_stateB = high_field();
        top->dmas = 0; top->speed = 0; top->hpos = 0;
        checks.note("P4B: DSKPTH<=0x1F under dmas=0/speed=0/hpos=0 -> 0x" + hex32(field_stateA) +
                     "; under dmas=1/speed=1/hpos=0x1FF -> 0x" + hex32(field_stateB));
#ifdef MUT_P4B
        checks.expect(field_stateA == field_stateB,
            "P4B oracle-substituted (unconditional-width model) -- field(A)=0x" +
            hex32(field_stateA) + " field(B)=0x" + hex32(field_stateB));
#else
        checks.expect(field_stateA != field_stateB,
            "P4B (opus:8-appA-dskpt.c5/sol:8-appa-dskpth.c5 p.279, '(E)'/'if ECS' "
            "conditional-difference reading [C14]): if the width were chipset-conditioned, "
            "varying dmas/speed/hpos -- the only other real inputs this module has, since it "
            "has no ecs port -- should be able to change the loaded field; field(A)=0x" +
            hex32(field_stateA) + " field(B)=0x" + hex32(field_stateB) + " (identical, no "
            "conditioning mechanism exists)");
#endif
    }

    // --- P5: DSKPTL's fifteen bits load correctly. Control on the same
    // measurement basis as P4A/P4B. ---
    {
        const uint16_t probe = 0x4C9A;
        write_reg(DSKPTL_ADDR, probe);
        uint32_t observed = low_field();
        uint32_t expected = (probe >> 1) & 0x7FFF;
#ifdef MUT_P5
        // corrupt one bit of the written word from what `expected` still uses
        write_reg(DSKPTL_ADDR, static_cast<uint16_t>(probe ^ 0x0008));
        observed = low_field();
#endif
        checks.expect(observed == expected,
            "P5 CONTROL (opus:8-appA-dskpt.c9/sol:8-appa-dskptl.c4 p.279, "
            "opus:8-appB-dskptl.c6/sol:8-appb-dskptl.c8 p.289; same measurement basis as "
            "P4A/P4B): DSKPTL's data_in[15:1] (fifteen bits) reaches address_out[15:1] -- "
            "observed=0x" + hex32(observed) + " expected=0x" + hex32(expected));
    }

    // --- P6: bit 0 of a DSKPTL write has no effect (even-address rule). ---
    {
        write_reg(DSKPTL_ADDR, 0x2468);
        uint32_t field_even = low_field();
#ifdef MUT_P6
        write_reg(DSKPTL_ADDR, 0x246B); // differs in bit 0 AND bit 1
#else
        write_reg(DSKPTL_ADDR, 0x2469); // differs only in bit 0
#endif
        uint32_t field_odd = low_field();
        checks.expect(field_even == field_odd,
            "P6 (opus:8-dma.c8/sol:8-dskpth.c4 p.246, sol:8-appb-legend.c5 p.287, "
            "sol:8-appb-dskpth.c3/sol:8-appb-dskptl.c3 p.289, evenness half of "
            "opus:8-appB-dskpth.c2/opus:8-appB-dskptl.c2 p.289): DSKPTL<=0x2468 and "
            "DSKPTL<=0x2469 (differing only in the discarded bit 0) read back identically -- "
            "field(0x2468)=0x" + hex32(field_even) + " field(2nd write)=0x" + hex32(field_odd));
    }

    // --- P7: both registers are independently required. ---
    {
        write_reg(DSKPTH_ADDR, 0x0A);
        write_reg(DSKPTL_ADDR, 0x1234);
        uint32_t before_full = top->address_out;

        write_reg(DSKPTH_ADDR, 0x15);
#ifdef MUT_P7
        write_reg(DSKPTL_ADDR, 0x0ABC); // contaminates "DSKPTH alone"
#endif
        uint32_t after_dskpth_only = top->address_out;
        bool low_preserved = (after_dskpth_only & 0x7FFF) == (before_full & 0x7FFF);
        bool high_updated  = ((after_dskpth_only >> 15) & 0x1F) == 0x15;
        checks.expect(low_preserved && high_updated,
            "P7 (opus:8-dma.c10/sol:8-dskpth.c6 p.246, opus:8-reglist.c15 p.241, "
            "pair-membership half of opus:8-appB-dskpth.c2/opus:8-appB-dskptl.c2/"
            "sol:8-appb-dskpth.c2/sol:8-appb-dskptl.c2 p.289): writing DSKPTH alone changes "
            "only the high field; the low field from the earlier DSKPTL write is preserved -- "
            "low_preserved=" + std::string(low_preserved ? "true" : "false") + " high_updated=" +
            std::string(high_updated ? "true" : "false"));
    }

    // --- P8: the pointer advances one word per granted cycle and holds the
    // next-to-use address (not the last-used one). ---
    {
        write_reg(DSKPTH_ADDR, 0x00);
        write_reg(DSKPTL_ADDR, 0x2000); // start pointer = 0x1000 (word address)
        uint32_t expected_ptr = top->address_out;

#ifdef MUT_P8
        top->dmal = 0; // withhold requests: no cycle is ever granted
#else
        top->dmal = 1;
#endif
        top->reg_address_in = IDLE_ADDR;
        top->data_in = 0;

        int grant_count = 0;
        bool all_matched = true;
        for (int hpos = 0; hpos < 40; hpos++) {
            top->hpos = hpos;
            top->eval();
            if (top->dma) {
                grant_count++;
                bool matched = (top->address_out == expected_ptr);
                all_matched = all_matched && matched;
                checks.note("P8: grant " + std::to_string(grant_count) + " at hpos=0x" +
                             hex32(hpos) + " address_out=0x" + hex32(top->address_out) +
                             " expected(next word)=0x" + hex32(expected_ptr) +
                             (matched ? " (match)" : " (MISMATCH)"));
                expected_ptr++;
            }
            tick();
        }
        top->dmal = 0;
        checks.expect(grant_count == 3 && all_matched,
            "P8 (opus:8-dma.c26/sol:8-dsklen.c15 p.247 increment; opus:8-dma.c27/"
            "sol:8-dsklen.c16 p.247 next-word; role half opus:8-dsk.c33 p.241, "
            "opus:8-dma.c3/sol:8-dskdma.c2 p.246): across a full hpos sweep at speed=0, "
            "exactly 3 disk-DMA cycles are granted (hpos 0x19/0x1D/0x21), and at each one "
            "address_out already holds the next word's address, advancing by exactly one "
            "word afterward -- grants=" + std::to_string(grant_count) + " all_matched=" +
            std::string(all_matched ? "true" : "false"));
    }

    // --- P9: nothing restores the pointer once DMA stops. ---
    {
        write_reg(DSKPTH_ADDR, 0x00);
        write_reg(DSKPTL_ADDR, 0x4000);
        top->dmal = 1;
        top->reg_address_in = IDLE_ADDR;
        for (int hpos = 0; hpos < 40; hpos++) {
            top->hpos = hpos;
            top->eval();
            tick();
        }
        uint32_t stopped_value = top->address_out;
#ifndef MUT_P9
        top->dmal = 0; // withdraw the request
#endif
        // 45 iterations, not 20: hpos must wrap back through a full slot
        // period (0x19/0x1D/0x21) for MUT_P9 (dmal left asserted) to have
        // any chance of producing a grant. A shorter window that never
        // reaches a slot would report green under the mutation too, which
        // would prove nothing.
        for (int i = 0; i < 45; i++) {
            top->hpos = (top->hpos + 1) % 40;
            top->eval();
            tick();
        }
        top->dmal = 0;
        uint32_t after_idle = top->address_out;
        checks.expect(after_idle == stopped_value,
            "P9 (sol:8-appb-ptrlegend.c2 p.288, sol:8-appa-dskptr.c2 p.279; grading only "
            "'must be reloaded before use', not the 'by a processor/Copper' requester-"
            "identity half): with dmal withdrawn, address_out holds at wherever it stopped "
            "across 45 further idle ticks (more than one full slot period) -- stopped=0x" +
            hex32(stopped_value) + " after_idle=0x" + hex32(after_idle) +
            " (this module has no reset port to restore it either)");
    }

    // --- P10: direction tracks wr/dmas at reg_address_out; exclusivity
    // ("only") is deliberately not graded -- see header. ---
    {
#ifdef MUT_P10
        uint8_t dmas_trialA = 1, dmas_trialB = 0; // swapped
#else
        uint8_t dmas_trialA = 0, dmas_trialB = 1;
#endif
        top->dmal = 1;
        top->dmas = dmas_trialA;
        top->eval();
        uint32_t addr_out_A = top->reg_address_out;
        top->dmas = dmas_trialB;
        top->eval();
        uint32_t addr_out_B = top->reg_address_out;
        top->dmal = 0;
        top->dmas = 0;
        checks.expect(addr_out_A == DSKDATR_ADDR && addr_out_B == DSKDAT_ADDR,
            "P10 (sol:8-dskpth.c2 p.246 source direction, sol:8-dskpth.c3 p.246 destination "
            "direction; grades only which address reg_address_out shows, not exclusivity): "
            "dmas=0 -> reg_address_out=0x" + hex32(addr_out_A) + " (expect DSKDATR $008/2=0x" +
            hex32(DSKDATR_ADDR) + "); dmas=1 -> reg_address_out=0x" + hex32(addr_out_B) +
            " (expect DSKDAT $026/2=0x" + hex32(DSKDAT_ADDR) + ")");
    }

    // --- P11A/P11B: the total width readings fail -- 20 word-address bits,
    // not 32 and not 18. ---
    {
        write_reg(DSKPTH_ADDR, 0xFFFF);
        write_reg(DSKPTL_ADDR, 0xFFFF);
        uint32_t max_value = top->address_out;
        int width = 0;
        while ((1u << width) <= max_value && width < 32) width++;
        checks.note("P11: all-ones write to DSKPTH+DSKPTL gives address_out=0x" +
                     hex32(max_value) + ", representable in " + std::to_string(width) + " bits");

#ifdef MUT_P11A
        checks.expect(width == 20,
            "P11A oracle-substituted (20-bit model, the classify-stage measured width) -- "
            "width=" + std::to_string(width));
#else
        checks.expect(width == 32,
            "P11A (opus:8-dsk.c34 p.241, opus:8-dma.c7/sol:8-dskpth.c1 p.246, '32-bit "
            "wide address'): the maximum value reachable by writing all-ones to DSKPTH+DSKPTL "
            "is 0x" + hex32(max_value) + ", representable in " + std::to_string(width) +
            " bits, not 32 -- no bus path on this boundary can ever deposit a bit beyond "
            "position 20 into the pointer");
#endif

#ifdef MUT_P11B
        checks.expect(width == 20,
            "P11B oracle-substituted (20-bit model, 5+15) -- width=" + std::to_string(width));
#else
        checks.expect(width == 18,
            "P11B (opus:8-appA-dskpt.c10/sol:8-appa-dskptr.c1 p.279, 'the 18-bit address of "
            "disk DMA data'): the five bits P4A establishes plus the fifteen bits P5 "
            "establishes compose to " + std::to_string(width) + " bits at address_out, not 18");
#endif
    }

    // --- claims recorded, not asserted ---
    checks.record(
        "opus:8-dma.c10, sol:8-dskpth.c6 (p.246), opus:8-reglist.c15 (p.241): the CPU-"
        "access-size half of 'a single long word' -- that the 68000/Copper issues one "
        "32-bit-wide bus cycle rather than two 16-bit ones. Not observable at "
        "reg_address_in/data_in, where a longword write arrives as two ordinary 16-bit "
        "register writes regardless of which instruction produced them (the two-register-"
        "split half of the same claims IS graded, as P7)");
    checks.record(
        "sol:8-appb-ptrlegend.c2 (p.288), sol:8-appa-dskptr.c2 (p.279): the 'by a processor' "
        "/ 'by the processor or Copper' requester-identity half. reg_address_in/data_in carry "
        "an address and a value and no requester identity; nothing at this boundary "
        "discriminates who issued a write (the 'must be reloaded/initialized before use' half "
        "of the same claims IS graded, as P9)");
    checks.record(
        "opus:8-appB-dskpth.c2, opus:8-appB-dskptl.c2 (p.289, composed with the p.287 '+' "
        "legend): the 'pointing to chip memory' half. Whether the loaded address lands in "
        "chip RAM is a fact about the memory map, observable at gary, not at agnus_diskdma -- "
        "a wider module boundary than assigned. Graded elsewhere per these claims' own split "
        "notes (sol:8-appb-legend.c6, sol:8-appb-dskpth.c4), outside this package's 55 claims");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";

    top->final();
    delete top;
    return checks.failed == 0 ? 0 : 1;
}
