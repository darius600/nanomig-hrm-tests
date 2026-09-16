// Test purpose: HRM conformance for ch8-disk-dma-pointer -- DSKPTH ($DFF020)
// / DSKPTL ($DFF022), the disk DMA pointer, at the `agnus_diskdma` boundary,
// per SPECIFICATION.md. Headline: P11A -- the pointer is printed "32 bits"
// (p.241, p.246) but stores only 20 word-address bits ([20:1]); that is the
// printed-width inconsistency this package still asserts, distinct from the
// OCS-18/ECS-20 conditioning question declined below.
//
// REPAIRED 2026-09-15 (independent stage-8 validation,
// hrm-testpipeline/work/2026-09-15_ch8_s2_stage8_validation.md and
// _sol_validation_A.md sections 6/9/10): P4A, P4B and P11B were withdrawn as
// red findings because each restates the SAME underlying fact -- DSKPTH's
// high field is five bits wide and loads unconditionally -- that is kept
// once, properly, at the sibling package `ch8-disk-pointer-ecs-and-copper`'s
// `agnus` boundary, where `ecs` is a real, drivable port. `agnus_diskdma`
// (this fixture) has no `ecs` port at all (agnus.v:326-340 connects none),
// so it can only observe the unconditional width; it cannot put the OCS/ECS
// conditional question the sibling's P4/P5 ask and answer. All three are
// declined below via record(), each naming the sibling assertion that
// carries the fact. P11A is unaffected by this repair: it is the
// p.241/p.246 "32 bits" wording against the stored 20 bits, not the
// OCS-18/ECS-20 conditioning fact, and stays RED.
//
// REPAIRED 2026-09-15 (repair lot D, LOS-D.md, following DOKTRIN.md): P3,
// P6, P7 and P10 were withdrawn (shape 3: nothing grades their claims
// elsewhere in the suite, checked before declining). P3's expect(true, ...)
// was an assertion no design behaviour could fail -- converted to record().
// P6 required address_out to discard DSKPTL bit 0 -- the manual's
// even-address rule is a precondition on software (p.246, p.287, p.289); it
// does not specify hardware masking behaviour for a misaligned write, so
// grading that RTL behaviour made the RTL its own oracle. Declined. P7's
// testable form (a DSKPTH-alone write leaves the low field untouched) shows
// only that the two registers decode and write independently -- already
// established by P1/P2 -- and does not distinguish this from any two
// unrelated write-only registers; the "single long word" claims it also
// cited are a CPU bus-width fact already excluded as a non-goal, and the
// "+  pair" claims' remaining (pair-membership) half is not actually put to
// the question by this measurement. Declined. P10 observes only which
// Paula register address `dmas` selects; agnus_diskdma's port list carries
// no RAM data bus at all (only reg_address_out, wr, address_out), so the
// transfer itself -- the source/destination claims' actual subject -- is
// not observable here. Declined. P8 and P9 were narrowed rather than
// declined: P8's expect() no longer requires exactly 3 grants at the
// implementation's own hpos slots (a correct implementation scheduling
// differently would have failed it); it now grades the page's own
// requirement directly -- every granted cycle observes address_out already
// holding the next word and advancing by exactly one word afterward, with
// at least one grant observed. P9 no longer grades mere holding (which the
// cited claims do not describe); it now performs an explicit reload of
// DSKPTH/DSKPTL after DMA stops and confirms the FIRST subsequent granted
// cycle uses exactly the freshly reloaded pointer, not the address where
// the previous transfer stopped -- genuinely exercising "must be reloaded
// before use". This repair also clears this package's uid-placement
// offenses (tools/check_uid_placement.py): 7 uids previously sat on both a
// graded line and a record() line (P7's opus:8-dma.c10/opus:8-reglist.c15/
// sol:8-dskpth.c6/opus:8-appB-dskpth.c2/opus:8-appB-dskptl.c2, P9's
// sol:8-appb-ptrlegend.c2/sol:8-appa-dskptr.c2); declining P7 and stripping
// P9's label of its literal uid tokens (kept only on the existing record()
// lines, which already name P7/P9 as the assertion carrying the graded
// half) resolves all 7.
//
// Test class: HRM_CONFORMANCE
// Primary oracle: the manual quotations cited inline (see SPECIFICATION.md
// for full citations and page numbers), not the RTL. The red result at
// P11A is a valid, finished outcome -- it is not adjusted to pass.
// Non-goals (see SPECIFICATION.md "Not turned into assertions"): the
// CPU-access-size half of "a single long word" (not observable at
// reg_address_in, which sees two ordinary 16-bit writes regardless of the
// originating instruction); the "by a processor/Copper" requester-identity
// half of the reload claims (reg_address_in carries an address and a value,
// no requester identity); the "pointing to chip memory" half of the `+`
// mark (a `gary`-level fact, out of this boundary); the OCS/ECS conditioning
// of DSKPTH's high-field width (P4A/P4B/P11B, declined -- graded at the
// sibling package ch8-disk-pointer-ecs-and-copper's P4/P5). This package
// also does not grade the exclusivity ("only") of DSKDATR/DSKDAT to the
// disk DMA channel -- that pair of claims belongs to the sibling package
// ch8-disk-dma-data-channel, not to this one. P10 is fully declined
// (2026-09-15, repair lot D): it observed only which Paula register
// address `dmas` selects, never the RAM transfer itself the
// source/destination claims describe -- see record().
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
// MUTATION P3: REMOVED 2026-09-15 (repair lot D). Declined via record(),
// not graded through expect() any more -- no mutation record remains with
// its assertion.
// MUTATION P4A, P4B: REMOVED 2026-09-15 (repair). Both assertions were
// declined, not merely mutated -- see the REPAIRED note above. Per
// BUILD-BRIEF, a mutation record is removed with its assertion; these two
// no longer grade anything through expect(), so no record remains.
// MUTATION P5 stimulus red: one bit of the value written to DSKPTL was
// flipped from the value the expected computation still uses. Baseline
// green. Went red. Reverted, rebuilt, back to green.
// MUTATION P6: REMOVED 2026-09-15 (repair lot D). Declined via record() --
// the even-address rule is a software precondition, not a hardware masking
// behaviour either citing page specifies; no mutation record remains.
// MUTATION P7: REMOVED 2026-09-15 (repair lot D). Declined via record() --
// see the REPAIRED note above; no mutation record remains.
// MUTATION P8 stimulus red: dmal held low throughout instead of high, so no
// DMA cycle is ever granted. Baseline green (grant_count>=1 && all_matched:
// 3 granted cycles observed, each address_out matching the running
// next-word counter). Went red (0 grants observed, failing grant_count>=1).
// Reverted, rebuilt, back to green.
// MUTATION P9 stimulus red (REPAIRED 2026-09-15, repair lot D -- retargeted
// to the new reload-and-use mechanism): the post-stop DSKPTH/DSKPTL reload
// writes are skipped instead of performed, so the pointer is never actually
// reloaded before DMA restarts. Baseline green (first_use == reloaded_ptr).
// Went red (with no reload, the first subsequent grant's address_out equals
// `stopped`, not `reloaded_ptr`, failing first_use==reloaded_ptr). Reverted,
// rebuilt, back to green.
// MUTATION P10: REMOVED 2026-09-15 (repair lot D). Declined via record() --
// see the REPAIRED note above; no mutation record remains.
// MUTATION P11A oracle red->green: the compared width was substituted from
// 32 (the page's claim) to 20 (the classify-stage measured width). Baseline
// red (32 bits are not reachable; the all-ones value tops out at 20 bits).
// Substituted oracle went green (20 bits matches). Reverted, rebuilt, back
// to red baseline.
// MUTATION P11B: REMOVED 2026-09-15 (repair). Declined, not merely mutated
// -- see the REPAIRED note above; no record remains with its assertion.
//
// Run tally, baseline (no -D flags), before the 2026-09-15 repair (P4A,
// P4B, P11B decline): 10 passed, 4 failed. After that repair, before repair
// lot D (this repair): 10 passed, 1 failed (P11A red). After repair lot D:
// see the build report below -- P3, P6, P7 and P10 are no longer graded
// (declined via record()); P8 and P9 remain graded, with their oracles
// narrowed to what the pages actually require; P11A is unchanged, red by
// design.

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

    // --- P3 (DECLINED, REPAIRED 2026-09-15, repair lot D): write-only was
    // graded via expect(true, ...), an assertion no design behaviour can
    // fail -- it restated the module's lack of a data-output port as its
    // own result, proving neither register write-only rather than testing
    // it. Converted to record(): the fixture has no rd/hwr/lwr strobe and,
    // per its evidence-recorded port list, no data-output port of any kind,
    // so no stimulus can ever construct a read-back path to falsify
    // against -- unfalsifiable by construction, at this boundary. ---
    checks.note("P3: agnus_diskdma's compiled port list (reg_address_in, data_in, address_out, "
                 "reg_address_out, dmal, dmas, dma, wr, hpos, speed) has no data-output port of "
                 "any kind -- DSKPTH/DSKPTL cannot be read back through this boundary");
    checks.record(
        "opus:8-appA-dskpt.c2, sol:8-appa-dskpth.c2 (p.279, DSKPTH W column), "
        "opus:8-appA-dskpt.c7, sol:8-appa-dskptl.c2 (p.279, DSKPTL W column), "
        "opus:8-appB-dskpth.c4, sol:8-appb-dskpth.c6 (p.289, composed), "
        "opus:8-appB-dskptl.c4, sol:8-appb-dskptl.c6 (p.289, composed): declined here, not "
        "asserted. This boundary's compiled port list carries no rd/hwr/lwr strobe and no "
        "data-output port of any kind, so no stimulus can ever construct a read-back path to "
        "falsify against -- the previous expect(true, ...) form was unfalsifiable by "
        "construction (the literal condition true), proving neither register write-only rather "
        "than testing it. Not re-asserted here.");

    // --- P4A/P4B (declined, REPAIRED 2026-09-15): both readings of the
    // appendix row restate the SAME unconditional-five-bit DSKPTH fact
    // graded properly, once, at the sibling package
    // ch8-disk-pointer-ecs-and-copper's `agnus` boundary, where a real,
    // drivable `ecs` port exists. `agnus_diskdma` has none at all
    // (agnus.v:326-340 connects none). The measurements below feed the
    // record() calls that decline these claims; neither is graded through
    // expect() any more. ---
    {
        write_reg(DSKPTH_ADDR, 0x1F); // all 5 low bits set
        uint32_t field_1F = high_field();
        write_reg(DSKPTH_ADDR, 0x07); // only the low 3 bits set (bits 3,4 clear)
        uint32_t field_07 = high_field();
        checks.note("P4A: DSKPTH<=0x1F -> high field=0x" + hex32(field_1F) +
                     "; DSKPTH<=0x07 -> high field=0x" + hex32(field_07) +
                     " (bits 3/4 land either way -- this boundary has no ecs input to "
                     "condition that on)");
        checks.record(
            "opus:8-appA-dskpt.c4, sol:8-appa-dskpth.c4 (p.279, OCS 'high 3 bits' reading): "
            "declined here, not asserted. This is the same unconditional-five-bit DSKPTH "
            "fact graded at the sibling package ch8-disk-pointer-ecs-and-copper's P5 ('on "
            "OCS, DSKPTH holds only the high three bits'), which drives the real ecs=0 port "
            "at the agnus boundary to put the OCS reading to the test. agnus_diskdma has no "
            "ecs port at all (agnus.v:326-340 connects none), so this boundary can only "
            "observe the unconditional width measured above (field(0x1F)=0x" + hex32(field_1F) +
            " field(0x07)=0x" + hex32(field_07) + ") -- it cannot put the OCS condition P5 "
            "tests to the question. Not re-asserted here.");

        // no chipset-conditioning input exists; the same write gives the
        // same field regardless of every other real input on this boundary.
        top->dmas = 0; top->speed = 0; top->hpos = 0;
        write_reg(DSKPTH_ADDR, 0x1F);
        uint32_t field_stateA = high_field();
        top->dmas = 1; top->speed = 1; top->hpos = 0x1FF;
        write_reg(DSKPTH_ADDR, 0x1F);
        uint32_t field_stateB = high_field();
        top->dmas = 0; top->speed = 0; top->hpos = 0;
        checks.note("P4B: DSKPTH<=0x1F under dmas=0/speed=0/hpos=0 -> 0x" + hex32(field_stateA) +
                     "; under dmas=1/speed=1/hpos=0x1FF -> 0x" + hex32(field_stateB) +
                     " (dmas/speed/hpos are not a stand-in for chipset identity)");
        checks.record(
            "opus:8-appA-dskpt.c5, sol:8-appa-dskpth.c5 (p.279, '(E)'/'if ECS' conditional-"
            "difference reading): declined here, not asserted. This is the same fact graded "
            "at the sibling package ch8-disk-pointer-ecs-and-copper's P4 ('the (E) mark: ECS "
            "changes DSKPTH'), which drives the real ecs port at the agnus boundary (ecs=0 "
            "vs ecs=1, the actual chipset-identity input) and finds no difference -- the "
            "genuine test of the (E) conditioning. This boundary has no ecs port to drive; "
            "varying dmas/speed/hpos (measured above: field(A)=0x" + hex32(field_stateA) +
            " field(B)=0x" + hex32(field_stateB) + ", identical) was never a valid proxy for "
            "chipset identity and cannot put the question P4 asks. Not re-asserted here.");
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

    // --- P6 (DECLINED, REPAIRED 2026-09-15, repair lot D): the printed
    // even-address rule (p.246 "The lowest bit of the address must be
    // zero..."; p.287/p.289 "+" mark composed) is a precondition ON
    // SOFTWARE -- what a programmer must write -- not a description of what
    // hardware does with a misaligned write. Neither citing page specifies
    // a masking/discard behaviour for bit 0; grading "does the RTL discard
    // bit 0" would make the RTL its own oracle. The only fact this boundary
    // can measure (no storage exists for bit 0 at all) is already the
    // content of this package's own green P5 control (DSKPTL's
    // data_in[15:1], fifteen bits, feeds address_out[15:1]) -- P6 added
    // nothing beyond restating P5 under an "evenness" label. Converted to
    // record(). ---
    {
        write_reg(DSKPTL_ADDR, 0x2468);
        uint32_t field_even = low_field();
        write_reg(DSKPTL_ADDR, 0x2469); // differs only in bit 0
        uint32_t field_odd = low_field();
        checks.note("P6: DSKPTL<=0x2468 -> field=0x" + hex32(field_even) +
                     "; DSKPTL<=0x2469 (differing only in bit 0) -> field=0x" + hex32(field_odd) +
                     " (no storage for bit 0 exists at this boundary at all -- the same fact "
                     "already kept as this package's green P5 control, not a masking behaviour "
                     "the manual specifies)");
        checks.record(
            "opus:8-dma.c8, sol:8-dskpth.c4 (p.246, 'lowest bit ... must be zero'), "
            "sol:8-appb-legend.c5 (p.287, '+' mark, 'even address'), sol:8-appb-dskpth.c3, "
            "sol:8-appb-dskptl.c3 (p.289, composed), evenness half of opus:8-appB-dskpth.c2, "
            "opus:8-appB-dskptl.c2 (p.289): declined here, not asserted. This is a precondition "
            "on the value software presents, not a specified hardware masking behaviour for a "
            "misaligned write -- neither citing page says what happens if bit 0 is set. The only "
            "fact this boundary can measure (field(0x2468)=0x" + hex32(field_even) +
            " field(0x2469)=0x" + hex32(field_odd) + ", identical) is that DSKPTL has no storage "
            "for bit 0 at all, which is the same fact already kept as this package's green P5 "
            "control (DSKPTL's fifteen-bit load). Not re-asserted here.");
    }

    // --- P7 (DECLINED, REPAIRED 2026-09-15, repair lot D): the testable
    // form here (a DSKPTH-alone write leaves the low field at its prior
    // value) shows only that DSKPTH and DSKPTL decode and load
    // independently -- already established by this package's own green
    // P1/P2 -- not that they are specifically the "+  Address register
    // pair" the p.287 legend and p.289 rows name, nor anything about "a
    // single long word" (already excluded above as a non-goal, the CPU
    // bus-width half of the same opus:8-dma.c10/sol:8-dskpth.c6/
    // opus:8-reglist.c15 claims). Any two unrelated write-only registers
    // feeding independent bit ranges would pass this identical measurement;
    // it does not put the manual's pairing claim to a real test. Converted
    // to record(). ---
    {
        write_reg(DSKPTH_ADDR, 0x0A);
        write_reg(DSKPTL_ADDR, 0x1234);
        uint32_t before_full = top->address_out;

        write_reg(DSKPTH_ADDR, 0x15);
        uint32_t after_dskpth_only = top->address_out;
        bool low_preserved = (after_dskpth_only & 0x7FFF) == (before_full & 0x7FFF);
        bool high_updated  = ((after_dskpth_only >> 15) & 0x1F) == 0x15;
        checks.note("P7: writing DSKPTH alone -- low_preserved=" +
                     std::string(low_preserved ? "true" : "false") + " high_updated=" +
                     std::string(high_updated ? "true" : "false") + " (independent decode, "
                     "already covered by P1/P2 -- not a test of pair-membership)");
        checks.record(
            "opus:8-dma.c10, sol:8-dskpth.c6 (p.246), opus:8-reglist.c15 (p.241), "
            "pair-membership half of opus:8-appB-dskpth.c2, opus:8-appB-dskptl.c2, "
            "sol:8-appb-dskpth.c2, sol:8-appb-dskptl.c2 (p.289, '+' mark): declined here, not "
            "asserted. The measurement (low_preserved=" +
            std::string(low_preserved ? "true" : "false") + " high_updated=" +
            std::string(high_updated ? "true" : "false") + ") shows only that DSKPTH and DSKPTL "
            "decode and load independently -- the same fact already kept as this package's "
            "green P1/P2 -- not that they are specifically the manual's address-register pair; "
            "a design with two entirely unrelated write-only registers would pass identically. "
            "The 'single long word' claims' remaining, CPU-bus-width half is a fact about "
            "instruction encoding, not observable at reg_address_in/data_in (see 'Not turned "
            "into assertions' above). Not re-asserted here.");
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
        checks.expect(grant_count >= 1 && all_matched,
            "P8 (REPAIRED 2026-09-15, repair lot D -- narrowed from requiring exactly 3 grants "
            "at this implementation's own hpos slots, which a correctly-scheduled but "
            "differently-timed implementation could fail, to the page's own requirement: "
            "opus:8-dma.c26/sol:8-dsklen.c15 p.247 increment; opus:8-dma.c27/sol:8-dsklen.c16 "
            "p.247 next-word; role half opus:8-dsk.c33 p.241, opus:8-dma.c3/sol:8-dskdma.c2 "
            "p.246): across a full hpos sweep at speed=0, at least one disk-DMA cycle is "
            "granted, and at EVERY granted cycle observed, address_out already holds the next "
            "word's address, advancing by exactly one word afterward -- grants=" +
            std::to_string(grant_count) + " all_matched=" +
            std::string(all_matched ? "true" : "false") + " (measured at hpos 0x19/0x1D/0x21 "
            "in this build, not required to be exactly those slots)");
    }

    // --- P9 (REPAIRED 2026-09-15, repair lot D): narrowed from grading mere
    // holding -- which the cited claims do not describe -- to an explicit
    // reload-and-use sequence. "Must be reloaded before use" is a
    // cause-and-effect claim: after DMA stops, an explicit DSKPTH/DSKPTL
    // write is what the NEXT transfer actually uses, not whatever the
    // pointer happened to hold. Uids cited only via record() below, per
    // uid-placement (the same two claims' other half -- requester identity
    // -- is already recorded there). ---
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
        top->dmal = 0; // withdraw the request; nothing restores the pointer meanwhile

        // reload: write a fresh pointer value, distinct from wherever the
        // previous transfer stopped, before re-enabling DMA.
        const uint16_t reload_h = 0x03, reload_l = 0x6000;
        uint32_t reloaded_ptr = ((reload_h & 0x1F) << 15) | ((reload_l >> 1) & 0x7FFF);
#ifndef MUT_P9
        write_reg(DSKPTH_ADDR, reload_h);
        write_reg(DSKPTL_ADDR, reload_l);
#endif

        top->dmal = 1;
        top->reg_address_in = IDLE_ADDR;
        bool got_grant = false;
        uint32_t first_use_value = 0;
        for (int hpos = 0; hpos < 40 && !got_grant; hpos++) {
            top->hpos = hpos;
            top->eval();
            if (top->dma) {
                got_grant = true;
                first_use_value = top->address_out;
            }
            tick();
        }
        top->dmal = 0;
        checks.note("P9: stopped=0x" + hex32(stopped_value) + " reload DSKPTH=0x" +
                     hex32(reload_h) + "/DSKPTL=0x" + hex32(reload_l) + " (reloaded_ptr=0x" +
                     hex32(reloaded_ptr) + ") -> first subsequent granted cycle's address_out=0x" +
                     hex32(first_use_value) + (got_grant ? "" : " (NO GRANT OBSERVED)"));
        checks.expect(got_grant && first_use_value == reloaded_ptr && first_use_value != stopped_value,
            "P9 (grading only the 'must be reloaded before use' half of the DSKPTH/DSKPTL "
            "reload claims -- see record() below for their uids and for the requester-identity "
            "half not graded here): after DMA stops, DSKPTH/DSKPTL are written to a fresh, "
            "distinct pointer value and DMA is re-enabled -- the FIRST subsequent granted cycle "
            "uses exactly that reloaded value, not the address where the previous transfer "
            "stopped, demonstrating the pointer must be reloaded before use, not merely that it "
            "holds meanwhile -- got_grant=" + std::string(got_grant ? "true" : "false") +
            " first_use=0x" + hex32(first_use_value) + " reloaded=0x" + hex32(reloaded_ptr) +
            " stopped=0x" + hex32(stopped_value));
    }

    // --- P10 (DECLINED, REPAIRED 2026-09-15, repair lot D): this only
    // observes which Paula register address dmas selects (reg_address_out).
    // agnus_diskdma's own port list carries no RAM data bus at all -- only
    // reg_address_out, wr and address_out -- so the actual transfer the
    // source/destination claims describe (data moving between DSKDATR/
    // DSKDAT and RAM) is not observable here; a reversed or entirely
    // broken RAM transfer direction would pass this identical measurement.
    // Converted to record(). ---
    {
        uint8_t dmas_trialA = 0, dmas_trialB = 1;
        top->dmal = 1;
        top->dmas = dmas_trialA;
        top->eval();
        uint32_t addr_out_A = top->reg_address_out;
        top->dmas = dmas_trialB;
        top->eval();
        uint32_t addr_out_B = top->reg_address_out;
        top->dmal = 0;
        top->dmas = 0;
        checks.note("P10: dmas=0 -> reg_address_out=0x" + hex32(addr_out_A) +
                     "; dmas=1 -> reg_address_out=0x" + hex32(addr_out_B) +
                     " (register-address selection only -- no RAM data bus exists at this "
                     "boundary to observe the transfer itself)");
        checks.record(
            "sol:8-dskpth.c2 (p.246, source direction), sol:8-dskpth.c3 (p.246, destination "
            "direction): declined here, not asserted. reg_address_out=0x" + hex32(addr_out_A) +
            " for dmas=0 (DSKDATR $008/2=0x" + hex32(DSKDATR_ADDR) + ") and 0x" +
            hex32(addr_out_B) + " for dmas=1 (DSKDAT $026/2=0x" + hex32(DSKDAT_ADDR) +
            ") shows only which Paula register address is selected; agnus_diskdma's port list "
            "(reg_address_in, data_in, address_out, reg_address_out, dmal, dmas, dma, wr, hpos, "
            "speed) carries no RAM data bus at all, so a reversed or entirely broken RAM "
            "transfer direction would pass this identical measurement -- the claims' actual "
            "subject, data moving to/from RAM, is not observable at this boundary. Not "
            "re-asserted here.");
    }

    // --- P11A: the "32 bits" width reading fails -- 20 word-address bits,
    // not 32. This is the p.241/p.246 wording against the stored width,
    // NOT the OCS-18/ECS-20 conditioning fact below -- keep them apart.
    // P11B (declined, REPAIRED 2026-09-15): see record() below. ---
    {
        write_reg(DSKPTH_ADDR, 0xFFFF);
        write_reg(DSKPTL_ADDR, 0xFFFF);
        uint32_t max_value = top->address_out;
        int width = 0;
        while ((1u << width) <= max_value && width < 32) width++;
        checks.note("P11A: all-ones write to DSKPTH+DSKPTL gives address_out=0x" +
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
            "position 20 into the pointer. This is the p.241/p.246 '32 bits' wording against "
            "the stored 20 word-address bits, a printed-width inconsistency distinct from "
            "the OCS-18/ECS-20 conditioning fact declined below and graded at the sibling "
            "package's P4/P5 -- the two are not merged");
#endif

        checks.record(
            "opus:8-appA-dskpt.c10, sol:8-appa-dskptr.c1 (p.279, 'the 18-bit address of disk "
            "DMA data', on OCS): declined here, not asserted. The composed total depends on "
            "the same unconditional-five-bit DSKPTH high field P4A/P4B decline above, plus "
            "the uncontested fifteen-bit low field already kept as this package's own green "
            "P5 control. The measured total at this boundary, 0x" + hex32(max_value) + " (" +
            std::to_string(width) + " bits), is not an OCS result -- read in its table "
            "context (p.289) 18 is the base/OCS width and the ECS row explicitly extends it, "
            "and the fact that decides which width applies is graded at the sibling package "
            "ch8-disk-pointer-ecs-and-copper's P4/P5, where a real, drivable ecs port exists. "
            "Not to be confused with P11A above, which is the distinct p.241/p.246 '32 bits' "
            "wording, not the OCS/ECS conditioning question. Not re-asserted here.");
    }

    // --- claims recorded, not asserted ---
    checks.record(
        "opus:8-dma.c10, sol:8-dskpth.c6 (p.246), opus:8-reglist.c15 (p.241): the CPU-"
        "access-size half of 'a single long word' -- that the 68000/Copper issues one "
        "32-bit-wide bus cycle rather than two 16-bit ones. Not observable at "
        "reg_address_in/data_in, where a longword write arrives as two ordinary 16-bit "
        "register writes regardless of which instruction produced them. (The two-register-"
        "split half of the same claims was graded as P7 until the 2026-09-15 repair lot D; "
        "P7 is now also declined -- see its own record() above for why the split measurement "
        "does not actually put this claim's pairing to the test. Both halves of these three "
        "claims are declined in this package now.)");
    checks.record(
        "sol:8-appb-ptrlegend.c2 (p.288), sol:8-appa-dskptr.c2 (p.279): the 'by a processor' "
        "/ 'by the processor or Copper' requester-identity half. reg_address_in/data_in carry "
        "an address and a value and no requester identity; nothing at this boundary "
        "discriminates who issued a write (the 'must be reloaded/initialized before use' half "
        "of the same claims IS graded, as P9, via an explicit reload-and-use measurement since "
        "the 2026-09-15 repair lot D)");
    checks.record(
        "opus:8-appB-dskpth.c2, opus:8-appB-dskptl.c2 (p.289, composed with the p.287 '+' "
        "legend): the 'pointing to chip memory' half. Whether the loaded address lands in "
        "chip RAM is a fact about the memory map, observable at gary, not at agnus_diskdma -- "
        "a wider module boundary than assigned. Graded elsewhere per these claims' own split "
        "notes (sol:8-appb-legend.c6, sol:8-appb-dskpth.c4), outside this package's 55 claims. "
        "(These same two claims' pair-membership half is also declined in this package, at P7 "
        "above, since the 2026-09-15 repair lot D -- see P7's own record() for why.)");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";

    top->final();
    delete top;
    return checks.failed == 0 ? 0 : 1;
}
