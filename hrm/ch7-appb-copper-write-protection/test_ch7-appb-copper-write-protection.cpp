// Test purpose: Verify Appendix B's Copper write-protection marks for this
// package's slice of the register list -- which addresses the Copper cannot
// write ("*"), and which it may write bearing no restriction mark at all --
// in both OCS and ECS as required.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
// p.287 (Appendix B legend) and pp.289-290 (Appendix B register list);
// committed specification 4647f19 (SPECIFICATION.md, this package).
// Requirement IDs: claims cited per-assertion by uid below; see
// SPECIFICATION.md for the full inventory of all 19.
// Non-goals: the "~" (danger-bit-required) mark is out of scope for this
// package -- its legend claims (sol:7-appB-legend.c5, opus:7-appB.c6) are
// routed to chapter 2's Copper packages, which already assert CDANG
// behaviour (see hrm/agnus_copper_move_range T5-T7, T9-T10). Whether
// VPOSW/VHPOSW are readable despite their "W" column is a different
// package's subject in this chapter and is not probed here, even though
// this package's stimulus writes to their addresses. COPCON, BLTCON0,
// BLTSIZE, the $096 boundary of DMACON, BPL1PTH and COLOR00 are already
// measured by hrm/agnus_copper_move_range; none of those addresses recurs
// in this package's own assertions.
//
// Originally one proposition scored, not eight: the eight starred claims in
// this package's slice ($002, $004, $006, $00E, $01C, $01E, $02A, $02C) are
// all decided by the single expression at agnus_copper.v:225,
// `ir2[8:6]==3'b000 && !ecs`, which reads only the destination address's
// top three bits (all eight fall inside byte range $000-$03E) and never
// distinguishes one named register from another. This file originally
// SCORED one representative case (T1, DMACONR $002) with its own control
// and mutation, and MEASURED (via `note`, never graded) the other seven --
// a forbidden write to any of those seven would have printed "was WRITTEN"
// while the package stayed green.
//
// REPAIR (2026-09-13, independent-validation finding 3, PROMOTE): "the same
// RTL expression decides all eight" is not an oracle reason to grade only
// one of eight printed "*" marks (p.287/p.289) -- each of the eight rows on
// p.289 carries its own mark, and the committed SPECIFICATION.md already
// specified all eight (PROP-1..PROP-8) as independent propositions before
// this file collapsed seven of them to `note()`. PROP-2 through PROP-8 are
// now their own expect() calls, same move_reaches() basis as T1, same
// shared control (T1's block + T2/T4/T6/T8's pass-through, per
// SPECIFICATION.md's "On a control case" section). Baseline moved 9 passed,
// 0 failed -> 16 passed, 0 failed; no new red -- the RTL's shared decode
// expression blocks all eight addresses, not only DMACONR.
//
// MUTATION T1 stimulus red: swapped move_reaches() target from DMACONR
// ($002, protected) to DMACON ($096, this package's own unmarked control
// register) while leaving the "not written" expectation unchanged -> FAIL
// as expected, because DMACON is writable. Reverted.
// MUTATION PROP-2 stimulus red: swapped move_reaches() target from VPOSR
// ($004, protected) to DMACON ($096) while leaving the "not written"
// expectation unchanged -> FAIL (15 passed, 1 failed), only PROP-2 newly
// red. Reverted (16 passed, 0 failed restored).
// MUTATION PROP-3 stimulus red: same swap, VHPOSR ($006) -> DMACON -> FAIL
// (15 passed, 1 failed), only PROP-3 newly red. Reverted.
// MUTATION PROP-4 stimulus red: same swap, CLXDAT ($00E) -> DMACON -> FAIL
// (15 passed, 1 failed), only PROP-4 newly red. Reverted.
// MUTATION PROP-5 stimulus red: same swap, INTENAR ($01C) -> DMACON -> FAIL
// (15 passed, 1 failed), only PROP-5 newly red. Reverted.
// MUTATION PROP-6 stimulus red: same swap, INTREQR ($01E) -> DMACON -> FAIL
// (15 passed, 1 failed), only PROP-6 newly red. Reverted.
// MUTATION PROP-7 stimulus red: same swap, VPOSW ($02A) -> DMACON -> FAIL
// (15 passed, 1 failed), only PROP-7 newly red. Reverted.
// MUTATION PROP-8 stimulus red: same swap, VHPOSW ($02C) -> DMACON -> FAIL
// (15 passed, 1 failed), only PROP-8 newly red. Reverted (16 passed, 0
// failed restored).
// MUTATION T2 stimulus red: swapped move_reaches()/install_two_moves()
// target from DMACON ($096) to DMACONR ($002, protected) while leaving the
// "written" expectation unchanged -> FAIL as expected. Reverted.
// MUTATION T3 stimulus red: same swap as T2, applied to the ECS run of
// DMACON -> FAIL as expected. Reverted.
// MUTATION T4 stimulus red: same swap as T2, applied to CLXCON OCS -> FAIL
// as expected. Reverted.
// MUTATION T5 stimulus red: same swap as T2, applied to CLXCON ECS -> FAIL
// as expected. Reverted.
// MUTATION T6 stimulus red: same swap as T2, applied to INTENA OCS -> FAIL
// as expected. Reverted.
// MUTATION T7 stimulus red: same swap as T2, applied to INTENA ECS -> FAIL
// as expected. Reverted.
// MUTATION T8 stimulus red: same swap as T2, applied to INTREQ OCS -> FAIL
// as expected. Reverted.
// MUTATION T9 stimulus red: same swap as T2, applied to INTREQ ECS -> FAIL
// as expected. Reverted.

#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <verilated.h>
#include "Vagnus_copper.h"

namespace {

// Register addresses are given as byte addresses in the HRM (Appendix B).
// The Copper instruction encodes DA8-1, i.e. the byte address shifted right
// by one, and that is also the width of the module's public register ports.
constexpr uint16_t to_reg(uint16_t byte_address) { return static_cast<uint16_t>(byte_address >> 1); }

constexpr uint16_t COP1LCH = to_reg(0x080);
constexpr uint16_t COP1LCL = to_reg(0x082);
constexpr uint16_t COPJMP1 = to_reg(0x088);
constexpr uint16_t COPINS = to_reg(0x08C);
constexpr uint16_t COPCON = to_reg(0x02E);
constexpr uint16_t IDLE_REG = 0x0FF;

// This package's eight starred (protected) addresses -- p.289. Only DMACONR
// is scored (T1); the other seven are measured only (see the header note).
constexpr uint16_t DMACONR = to_reg(0x002);
constexpr uint16_t VPOSR   = to_reg(0x004);
constexpr uint16_t VHPOSR  = to_reg(0x006);
constexpr uint16_t CLXDAT  = to_reg(0x00E);
constexpr uint16_t INTENAR = to_reg(0x01C);
constexpr uint16_t INTREQR = to_reg(0x01E);
constexpr uint16_t VPOSW   = to_reg(0x02A);
constexpr uint16_t VHPOSW  = to_reg(0x02C);

// This package's four unmarked (freely writable) addresses -- p.290.
constexpr uint16_t DMACON = to_reg(0x096);
constexpr uint16_t CLXCON = to_reg(0x098);
constexpr uint16_t INTENA = to_reg(0x09A);
constexpr uint16_t INTREQ = to_reg(0x09C);

constexpr uint32_t LIST_ADDRESS = 0x00002000;

struct MoveEvent {
    uint16_t reg;
    uint16_t data;
};

class CopperHarness {
public:
    explicit CopperHarness(bool ecs_mode) : dut(new Vagnus_copper) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->reset = 1;
        dut->ecs = ecs_mode;
        dut->enadma = 1;
        dut->ackdma = 0;
        dut->sof = 0;
        dut->blit_busy = 0;
        dut->hpos = 0;
        dut->vpos = 0;
        dut->data_in = 0;
        dut->reg_address_in = IDLE_REG;

        for (int i = 0; i < 12; ++i) {
            cycle();
        }
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) {
            cycle();
        }
    }

    ~CopperHarness() {
        dut->final();
        delete dut;
    }

    CopperHarness(const CopperHarness&) = delete;
    CopperHarness& operator=(const CopperHarness&) = delete;

    // A minimal list: one MOVE of `data` to `reg`, then a terminating WAIT.
    void install_single_move(uint16_t reg, uint16_t data) {
        memory[LIST_ADDRESS + 0] = static_cast<uint16_t>(reg << 1);
        memory[LIST_ADDRESS + 1] = data;
        memory[LIST_ADDRESS + 2] = 0xFFFF;
        memory[LIST_ADDRESS + 3] = 0xFFFE;
    }

    // Two sequential MOVEs, then a terminating WAIT -- used to prove the
    // fixture delivers distinct known words, in order, before any assertion
    // trusts a single write's outcome (BUILD-BRIEF's fixture-liveness
    // requirement).
    void install_two_moves(uint16_t reg1, uint16_t data1, uint16_t reg2, uint16_t data2) {
        memory[LIST_ADDRESS + 0] = static_cast<uint16_t>(reg1 << 1);
        memory[LIST_ADDRESS + 1] = data1;
        memory[LIST_ADDRESS + 2] = static_cast<uint16_t>(reg2 << 1);
        memory[LIST_ADDRESS + 3] = data2;
        memory[LIST_ADDRESS + 4] = 0xFFFF;
        memory[LIST_ADDRESS + 5] = 0xFFFE;
    }

    void set_cdang(bool value) {
        write_reg(COPCON, value ? 0x0002 : 0x0000);
        run_cycles(2);
    }

    void start_list() {
        const uint16_t high = static_cast<uint16_t>((LIST_ADDRESS >> 15) & 0x1F);
        const uint16_t low = static_cast<uint16_t>((LIST_ADDRESS & 0x7FFF) << 1);
        write_reg(COP1LCH, high);
        write_reg(COP1LCL, low);
        write_reg(COPJMP1, 0x0000);
        run_cycles(4);
        dma_allowed = true;
    }

    void run_cycles(int count) {
        for (int i = 0; i < count; ++i) {
            cycle();
        }
    }

    int move_count(uint16_t reg) const {
        int count = 0;
        for (const auto& event : moves) {
            if (event.reg == reg) {
                ++count;
            }
        }
        return count;
    }

    bool saw_move(uint16_t reg, uint16_t data) const {
        for (const auto& event : moves) {
            if (event.reg == reg && event.data == data) {
                return true;
            }
        }
        return false;
    }

    bool dma_allowed = false;
    std::vector<MoveEvent> moves;

private:
    Vagnus_copper* dut;
    std::unordered_map<uint32_t, uint16_t> memory;
    uint16_t hpos = 0;
    uint16_t vpos = 0;

    uint16_t read_memory(uint32_t word_address) const {
        const auto it = memory.find(word_address);
        return it == memory.end() ? 0xFFFF : it->second;
    }

    void write_reg(uint16_t word_reg, uint16_t data) {
        cycle(true, word_reg, data);
    }

    void cycle(bool cpu_write = false, uint16_t cpu_reg = IDLE_REG, uint16_t cpu_data = 0) {
        dut->eval();

        const bool accepted_dma = !cpu_write && dma_allowed && dut->reqdma;
        if (cpu_write) {
            dut->ackdma = 0;
            dut->reg_address_in = cpu_reg;
            dut->data_in = cpu_data;
        } else if (accepted_dma) {
            const uint32_t address = dut->address_out;
            const uint16_t reg = dut->reg_address_out;
            const uint16_t data = read_memory(address);
            dut->ackdma = 1;
            dut->reg_address_in = reg;
            dut->data_in = data;
            dut->eval();

            // A MOVE's second-word fetch presents the destination register on
            // the public register bus. COPINS is the Copper's own instruction
            // fetch and is not a MOVE to a destination.
            if (reg != COPINS && reg != IDLE_REG) {
                moves.push_back({reg, data});
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

        if (++hpos == 454) {
            hpos = 0;
            vpos = static_cast<uint16_t>((vpos + 1) & 0x00FF);
        }
        dut->hpos = hpos;
        dut->vpos = vpos;
    }
};

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

// Runs one MOVE of 0x0ABC to `reg` and reports whether the write appeared.
bool move_reaches(bool ecs, bool cdang, uint16_t reg, bool set_cdang_explicitly = true) {
    CopperHarness h(ecs);
    if (set_cdang_explicitly) {
        h.set_cdang(cdang);
    }
    h.install_single_move(reg, 0x0ABC);
    h.start_list();
    h.run_cycles(600);
    return h.move_count(reg) > 0;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ------------------------------------------------------------------
    // T1 -- the one scored protected-address proposition, at the strongest
    // configuration this package's own claims license: CDANG SET (proving
    // the "*" mark is unconditional, unlike the CDANG-gated "~" range that
    // is out of this package's scope) and ecs=0 (OCS, per this package's
    // claims' own chipset scope -- running this at ecs=1 would test a chip
    // nobody here is testing).
    // ------------------------------------------------------------------
    checks.expect(!move_reaches(/*ecs=*/false, /*cdang=*/true, DMACONR),
                  "T1 OCS CDANG=1: Copper MOVE to DMACONR ($002) is blocked, "
                  "per Appendix B's \"*\" mark "
                  "[claims: sol:7-appB-dmaconr.c5, opus:7-appB.c14, legend "
                  "sol:7-appB-legend.c4, opus:7-appB.c5]");

    // ------------------------------------------------------------------
    // PROP-2 through PROP-8 -- independent-validation finding 3 (PROMOTE,
    // 2026-09-13). The other seven addresses in this package's starred
    // slice are decided by the same RTL expression as T1
    // (agnus_copper.v:225, `ir2[8:6]==3'b000 && !ecs`, reading only the
    // destination address's top three bits) but were only MEASURED here
    // until this repair, never graded: a forbidden write would have
    // printed "was WRITTEN" while the package stayed green. p.287 defines
    // "*" as "Address not writable by the Copper," and p.289 gives each of
    // these seven rows its OWN "*", exactly like DMACONR's. The committed
    // SPECIFICATION.md (PROP-2..PROP-8) already specified these as eight
    // INDEPENDENT propositions; "they use the same RTL expression" is not
    // an oracle reason to collapse eight printed marks into one grade.
    // Each is now its own expect(), on the identical move_reaches() basis
    // as T1. T1 above (blocked) and T2/T4/T6/T8 below (freely-writable,
    // OCS) remain this family's shared control -- BUILD-BRIEF's "control
    // on the same measurement basis" is already satisfied file-wide, per
    // SPECIFICATION.md's own "On a control case" section: PROP-9..12 are
    // the natural controls for PROP-1..8, same harness, same
    // move_reaches() call, differing only in target address.
    // ------------------------------------------------------------------
    checks.expect(!move_reaches(/*ecs=*/false, /*cdang=*/true, VPOSR),
                  "PROP-2 OCS CDANG=1: Copper MOVE to VPOSR ($004) is blocked, "
                  "per Appendix B's \"*\" mark "
                  "[claims: sol:7-appB-vposr.c5, opus:7-appB.c16, legend "
                  "sol:7-appB-legend.c4, opus:7-appB.c5]");
    checks.expect(!move_reaches(/*ecs=*/false, /*cdang=*/true, VHPOSR),
                  "PROP-3 OCS CDANG=1: Copper MOVE to VHPOSR ($006) is blocked, "
                  "per Appendix B's \"*\" mark "
                  "[claims: sol:7-appB-vhposr.c5, opus:7-appB.c18, legend "
                  "sol:7-appB-legend.c4, opus:7-appB.c5]");
    checks.expect(!move_reaches(/*ecs=*/false, /*cdang=*/true, CLXDAT),
                  "PROP-4 OCS CDANG=1: Copper MOVE to CLXDAT ($00E) is blocked, "
                  "per Appendix B's \"*\" mark "
                  "[claims: sol:7-appB-clxdat.c5, opus:7-appB.c20, legend "
                  "sol:7-appB-legend.c4, opus:7-appB.c5]");
    checks.expect(!move_reaches(/*ecs=*/false, /*cdang=*/true, INTENAR),
                  "PROP-5 OCS CDANG=1: Copper MOVE to INTENAR ($01C) is blocked, "
                  "per Appendix B's \"*\" mark "
                  "[claims: sol:7-appB-intenar.c5, opus:7-appB.c22, legend "
                  "sol:7-appB-legend.c4, opus:7-appB.c5]");
    checks.expect(!move_reaches(/*ecs=*/false, /*cdang=*/true, INTREQR),
                  "PROP-6 OCS CDANG=1: Copper MOVE to INTREQR ($01E) is blocked, "
                  "per Appendix B's \"*\" mark "
                  "[claims: sol:7-appB-intreqr.c5, opus:7-appB.c24, legend "
                  "sol:7-appB-legend.c4, opus:7-appB.c5]");
    // VPOSW/VHPOSW get individual attention per the lead's note: another
    // package in this chapter measured these two ARE readable despite
    // their "W" column. That is a different proposition (readability);
    // this package only grades whether the Copper's WRITE takes effect.
    checks.expect(!move_reaches(/*ecs=*/false, /*cdang=*/true, VPOSW),
                  "PROP-7 OCS CDANG=1: Copper MOVE to VPOSW ($02A) is blocked, "
                  "per Appendix B's \"*\" mark -- write-blocking only; VPOSW's "
                  "separately-measured readability is out of this package's scope "
                  "[claims: sol:7-appB-vposw.c5, opus:7-appB.c26, legend "
                  "sol:7-appB-legend.c4, opus:7-appB.c5]");
    checks.expect(!move_reaches(/*ecs=*/false, /*cdang=*/true, VHPOSW),
                  "PROP-8 OCS CDANG=1: Copper MOVE to VHPOSW ($02C) is blocked, "
                  "per Appendix B's \"*\" mark -- write-blocking only; VHPOSW's "
                  "separately-measured readability is out of this package's scope "
                  "[claims: sol:7-appB-vhposw.c5, opus:7-appB.c28, legend "
                  "sol:7-appB-legend.c4, opus:7-appB.c5]");

    // ------------------------------------------------------------------
    // T2/T3 -- DMACON ($096): unmarked, so the Copper may write it, run at
    // BOTH chipset settings as this claim's own field demands. T2 also
    // proves the fixture delivers two DISTINCT known words, IN ORDER,
    // before any assertion in this file trusts a single write's outcome.
    // ------------------------------------------------------------------
    {
        CopperHarness h(/*ecs=*/false);
        h.install_two_moves(DMACON, 0x1111, DMACON, 0x2222);
        h.start_list();
        h.run_cycles(600);
        const bool ordered_and_distinct =
            h.move_count(DMACON) == 2 &&
            h.moves.size() >= 2 && h.moves[0].reg == DMACON && h.moves[0].data == 0x1111 &&
            h.moves[1].reg == DMACON && h.moves[1].data == 0x2222;
        checks.expect(ordered_and_distinct,
                      "T2 OCS: Copper MOVEs 0x1111 then 0x2222 to DMACON ($096) both arrive, "
                      "in order, unmodified (fixture-liveness proof; also: DMACON carries no "
                      "restriction mark so the Copper may write it) "
                      "[claims: opus:7-appB.c33, legend sol:7-appB-legend.c4, opus:7-appB.c5]");
    }
    checks.expect(move_reaches(/*ecs=*/true, /*cdang=*/false, DMACON),
                  "T3 ECS: Copper MOVE to DMACON ($096) is written -- run at ECS as this claim's own "
                  "chipset field requires (\"must be RUN at both, or it only half tests the "
                  "proposition\") [claims: opus:7-appB.c33]");

    // ------------------------------------------------------------------
    // T4/T5 -- CLXCON ($098): unmarked, both chipset settings.
    // ------------------------------------------------------------------
    checks.expect(move_reaches(/*ecs=*/false, /*cdang=*/false, CLXCON),
                  "T4 OCS: Copper MOVE to CLXCON ($098) is written [claims: opus:7-appB.c33]");
    checks.expect(move_reaches(/*ecs=*/true, /*cdang=*/false, CLXCON),
                  "T5 ECS: Copper MOVE to CLXCON ($098) is written [claims: opus:7-appB.c33]");

    // ------------------------------------------------------------------
    // T6/T7 -- INTENA ($09A): unmarked, both chipset settings.
    // ------------------------------------------------------------------
    checks.expect(move_reaches(/*ecs=*/false, /*cdang=*/false, INTENA),
                  "T6 OCS: Copper MOVE to INTENA ($09A) is written [claims: opus:7-appB.c33]");
    checks.expect(move_reaches(/*ecs=*/true, /*cdang=*/false, INTENA),
                  "T7 ECS: Copper MOVE to INTENA ($09A) is written [claims: opus:7-appB.c33]");

    // ------------------------------------------------------------------
    // T8/T9 -- INTREQ ($09C): unmarked, both chipset settings.
    // ------------------------------------------------------------------
    checks.expect(move_reaches(/*ecs=*/false, /*cdang=*/false, INTREQ),
                  "T8 OCS: Copper MOVE to INTREQ ($09C) is written [claims: opus:7-appB.c33]");
    checks.expect(move_reaches(/*ecs=*/true, /*cdang=*/false, INTREQ),
                  "T9 ECS: Copper MOVE to INTREQ ($09C) is written [claims: opus:7-appB.c33]");

    // ------------------------------------------------------------------
    // Claims considered for this package and left unbound, with reasons.
    // Neither line touches passed/failed -- these are not assertions.
    // ------------------------------------------------------------------
    checks.record("The \"~\" (danger-bit-required) mark is not asserted by this package. Its "
        "legend claims (sol:7-appB-legend.c5, opus:7-appB.c6) exist in this chapter's corpus "
        "but are routed to \"covered by an existing package\": chapter 2's Copper packages "
        "(hrm/agnus_copper_move_range T3-T7, T9-T10, and others) already assert CDANG/danger-bit "
        "behaviour on this same RTL expression. Confirmed by the lead after this package's "
        "specification was committed; nothing for this package to build.");
    checks.record("Whether VPOSW ($02A) or VHPOSW ($02C) are readable despite Appendix B's \"W\" "
        "column is a different package's subject in this chapter (already measured elsewhere). "
        "This package measured only whether the Copper's WRITE to those two addresses takes "
        "effect (see the notes above); both behaved consistent with every other address in "
        "this package's starred slice, with nothing to report as off.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
