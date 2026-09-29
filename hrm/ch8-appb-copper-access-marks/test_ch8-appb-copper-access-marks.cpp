// Test purpose: whether ADKCONR/SERDATR/SERDAT/SERPER -- all four "*"-marked
// on p.289 -- are unconditionally unreachable by the Copper, as the p.287
// legend states, or only conditionally so, at ECS=1 (both CDANG states).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, 3rd ed., Appendix B,
// p.287 (the "*"/"~" legend) composed with p.289 (the four rows), and, at
// ecs=1/cdang=1, Appendix C p.310, which governs that corner for SERDAT,
// SERDATR and SERPER. See SPECIFICATION.md for the full reading, why this
// package is scoped to ECS (the OCS corners are already graded, PASS, in
// hrm/ch2-copper-move-sweep), and the five claims routed to record()
// instead of graded here.
// Non-goals: anything outside the `agnus_copper` fixture this package
//   builds, and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// MUTATION LIVENESS stimulus red MUT_LIVE retargeted the cdang=0 liveness
//   probe from DMACON ($096, always writable) to SERDAT ($030, blocked at
//   ecs=1/cdang=0) -> FAIL as expected.
// MUTATION CTRL-BLTCON0 stimulus red MUT_CTRL flipped the cdang argument on
//   BLTCON0's "admitted" probe from true to false -> FAIL as expected
//   (BLTCON0 is blocked, not admitted, at cdang=0).
// MUTATION SERDAT-CDANG0 stimulus red MUT_SD0 flipped the cdang argument
//   from false to true (still ecs=1) -> FAIL as expected (SERDAT becomes
//   reachable at cdang=1, the corner p.310 governs; SERDAT-CDANG1 records
//   that corner rather than grading it, so no mutation record is needed
//   for it).
// MUTATION SERPER-CDANG0 stimulus red MUT_SP0 flipped the cdang argument
//   from false to true (still ecs=1) -> FAIL as expected.
// MUTATION SERDATR-CDANG0 stimulus red MUT_SR0 flipped the cdang argument
//   from false to true (still ecs=1) -> FAIL as expected.

#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <verilated.h>
#include "Vagnus_copper.h"

#ifndef MUT_LIVE
#define MUT_LIVE 0
#endif
#ifndef MUT_CTRL
#define MUT_CTRL 0
#endif
#ifndef MUT_SD0
#define MUT_SD0 0
#endif
#ifndef MUT_SP0
#define MUT_SP0 0
#endif
#ifndef MUT_SR0
#define MUT_SR0 0
#endif

namespace {

// Register addresses are given as byte addresses in the HRM. A Copper
// instruction encodes DA8-1 (the byte address shifted right by one), which
// is also the width of agnus_copper's public register ports.
constexpr uint16_t to_reg(uint16_t byte_address) { return static_cast<uint16_t>(byte_address >> 1); }

constexpr uint16_t COP1LCH = to_reg(0x080);
constexpr uint16_t COP1LCL = to_reg(0x082);
constexpr uint16_t COPJMP1 = to_reg(0x088);
constexpr uint16_t COPINS  = to_reg(0x08C);
constexpr uint16_t COPCON  = to_reg(0x02E);
constexpr uint16_t IDLE_REG = 0x0FF;

constexpr uint16_t SERDATR = to_reg(0x018);  // *-marked, this package's P-SERDATR
constexpr uint16_t SERDAT  = to_reg(0x030);  // *-marked, this package's P-SERDAT
constexpr uint16_t SERPER  = to_reg(0x032);  // *-marked, this package's P-SERPER
constexpr uint16_t BLTCON0 = to_reg(0x040);  // ~-marked, this package's control
constexpr uint16_t DMACON  = to_reg(0x096);  // always writable, liveness precondition

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

        for (int i = 0; i < 12; ++i) cycle();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) cycle();
    }

    ~CopperHarness() {
        dut->final();
        delete dut;
    }

    CopperHarness(const CopperHarness&) = delete;
    CopperHarness& operator=(const CopperHarness&) = delete;

    void install_single_move(uint16_t reg, uint16_t data) {
        memory[LIST_ADDRESS + 0] = static_cast<uint16_t>(reg << 1);
        memory[LIST_ADDRESS + 1] = data;
        memory[LIST_ADDRESS + 2] = 0xFFFF;
        memory[LIST_ADDRESS + 3] = 0xFFFE;
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
        for (int i = 0; i < count; ++i) cycle();
    }

    int move_count(uint16_t reg) const {
        int count = 0;
        for (const auto& event : moves) {
            if (event.reg == reg) ++count;
        }
        return count;
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
        if (condition) {
            std::cout << "PASS: " << label << "\n";
            ++passed;
        } else {
            std::cout << "FAIL: " << label << "\n";
            ++failed;
        }
    }

    // Prints a measured number/fact graded by nothing.
    void note(const std::string& msg) {
        std::cout << "MEASURED: " << msg << "\n";
    }

    // Declines a claim, with the reason -- the uid's only home when it is
    // not on a graded expect() label.
    void record(const std::string& msg) {
        std::cout << "RECORDED: " << msg << "\n";
    }

    int passed = 0;
    int failed = 0;
};

// Installs a single MOVE of 0x0ABC to `reg`, runs the list, and reports
// whether the write reached the register.
bool move_reaches(bool ecs, bool cdang, uint16_t reg) {
    CopperHarness h(ecs);
    h.set_cdang(cdang);
    h.install_single_move(reg, 0x0ABC);
    h.start_list();
    h.run_cycles(600);
    return h.move_count(reg) > 0;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // --- Fixture liveness (precondition): the harness must be able to
    // register a Copper write at all, at the two corners this package
    // grades under, before any "blocked" result below is trustworthy.
    uint16_t liveness_target_cdang0 = DMACON;
#if MUT_LIVE
    liveness_target_cdang0 = SERDAT;  // retargeted from DMACON to a register this package expects blocked at cdang=0
#endif
    const bool live_cdang0 = move_reaches(true, false, liveness_target_cdang0);
    const bool live_cdang1 = move_reaches(true, true, DMACON);
    checks.expect(live_cdang0 && live_cdang1,
                  "LIVENESS ecs=1: a Copper MOVE reaches DMACON ($096, always writable) at cdang=0 and at "
                  "cdang=1 -- confirms the harness can register a write under both corners this package grades, "
                  "so a later \"blocked\" result is a real \"no\", not a dead bus");

    // --- Control: BLTCON0 ($040), ~-marked. Its own legend (p.287:24) names
    // one exception -- COPCON's danger bit -- and no chipset term. Measured
    // at ECS=1 against that rule: blocked while CDANG is clear, admitted
    // once it is set.
    const bool bltcon0_blocked_cdang0 = !move_reaches(true, false, BLTCON0);
    bool cdang_for_admitted_probe = true;
#if MUT_CTRL
    cdang_for_admitted_probe = false;  // was true; checks the "admitted" half still holds with cdang left clear (it must not)
#endif
    const bool bltcon0_admitted_cdang1 = move_reaches(true, cdang_for_admitted_probe, BLTCON0);
    checks.expect(bltcon0_blocked_cdang0 && bltcon0_admitted_cdang1,
                  "CTRL-BLTCON0 ecs=1: BLTCON0 ($040, ~-marked, p.289:41) is blocked at cdang=0 and admitted at "
                  "cdang=1, matching its own conditional legend (p.287:24) exactly -- the control that shows this "
                  "fixture and measurement basis can tell a conditional mark from an unconditional one, at the same "
                  "corners graded below");

    // --- P-SERDAT: SERDAT ($030), *-marked (p.289:33), composed with the
    // unconditional legend (p.287:22). Expected blocked in BOTH CDANG
    // states at ECS=1 -- the OCS corners are already graded PASS in
    // hrm/ch2-copper-move-sweep (SWEEP ecs=0 cdang=0/cdang=1 vs spec
    // Reading B; see SPECIFICATION.md).
    bool sd0_cdang = false;
#if MUT_SD0
    sd0_cdang = true;  // was false
#endif
    checks.expect(!move_reaches(true, sd0_cdang, SERDAT),
                  "SERDAT-CDANG0 ecs=1 cdang=0: a Copper MOVE to SERDAT ($030) is blocked, matching the "
                  "unconditional * legend [claims: sol:8-serdat.c78, opus:8-regsum.c13]");

    checks.record(
        "SERDAT-CDANG1 ecs=1 cdang=1 (SERDAT $030, p.289 '*' composed with p.287's legend) [claims: "
        "sol:8-serdat.c78, opus:8-regsum.c13] -- at this corner the governing text is p.310 (Appendix C, "
        "\"Interpretational Differences\"): \"Now, in the ECS, if this bit is set, the Copper can access "
        "all of the Amiga chip registers.\" Already graded: hrm/ch2-copper-move-sweep, assertion "
        "\"SWEEP ecs=1 cdang=1 vs Appendix C\" (SERDAT among the 256 destinations measured accepted, PASS), "
        "and \"APPBSTAR $18 SERDAT ecs=0\" (the same row, blocked at ecs=0, the corner p.310 leaves "
        "untouched, PASS). Not graded here against the unconditional '*' alone; SERDAT-CDANG0 above covers "
        "this proposition's other corner.");

    // --- P-SERPER: SERPER ($032), *-marked (p.289:34). Same shape as
    // P-SERDAT.
    bool sp0_cdang = false;
#if MUT_SP0
    sp0_cdang = true;  // was false
#endif
    checks.expect(!move_reaches(true, sp0_cdang, SERPER),
                  "SERPER-CDANG0 ecs=1 cdang=0: a Copper MOVE to SERPER ($032) is blocked, matching the "
                  "unconditional * legend [claims: sol:8-serper.c33, opus:8-regsum.c19]");

    checks.record(
        "SERPER-CDANG1 ecs=1 cdang=1 (SERPER $032, p.289 '*' composed with p.287's legend) [claims: "
        "sol:8-serper.c33, opus:8-regsum.c19] -- at this corner the governing text is p.310 (Appendix C, "
        "\"Interpretational Differences\"): \"Now, in the ECS, if this bit is set, the Copper can access "
        "all of the Amiga chip registers.\" Already graded: hrm/ch2-copper-move-sweep, assertion "
        "\"SWEEP ecs=1 cdang=1 vs Appendix C\" (SERPER among the 256 destinations measured accepted, PASS), "
        "and \"APPBSTAR $19 SERPER ecs=0\" (the same row, blocked at ecs=0, the corner p.310 leaves "
        "untouched, PASS). Not graded here against the unconditional '*' alone; SERPER-CDANG0 above covers "
        "this proposition's other corner.");

    // --- P-SERDATR: SERDATR ($018), *-marked (p.289:21). Same shape as
    // P-SERDAT. `opus:8-regsum.c7`'s own restatement is the narrower
    // glyph-identity form, but its classify.evidence composes it with the
    // same p.287 legend and the same agnus_copper.v:225 decode used for the
    // other four claims -- graded here as that composed proposition (see
    // SPECIFICATION.md, P-SERDATR).
    bool sr0_cdang = false;
#if MUT_SR0
    sr0_cdang = true;  // was false
#endif
    checks.expect(!move_reaches(true, sr0_cdang, SERDATR),
                  "SERDATR-CDANG0 ecs=1 cdang=0: a Copper MOVE to SERDATR ($018) is blocked, matching the "
                  "unconditional * legend [claims: opus:8-regsum.c7]");

    checks.record(
        "SERDATR-CDANG1 ecs=1 cdang=1 (SERDATR $018, p.289 '*' composed with p.287's legend) [claims: "
        "opus:8-regsum.c7] -- at this corner the governing text is p.310 (Appendix C, \"Interpretational "
        "Differences\"): \"Now, in the ECS, if this bit is set, the Copper can access all of the Amiga chip "
        "registers.\" Already graded: hrm/ch2-copper-move-sweep, assertion \"SWEEP ecs=1 cdang=1 vs "
        "Appendix C\" (SERDATR among the 256 destinations measured accepted, PASS), and \"APPBSTAR $0C "
        "SERDATR ecs=0\" (the same row, blocked at ecs=0, the corner p.310 leaves untouched, PASS). Not "
        "graded here against the unconditional '*' alone; SERDATR-CDANG0 above covers this proposition's "
        "other corner.");

    // ------------------------------------------------------------------
    // Claims already graded in an accepted slice -- not re-graded here.
    // See SPECIFICATION.md, "Already graded elsewhere", for how each
    // citation was verified (and, for c145, corrected) against the actual
    // accepted package rather than trusted from the claims file's own note.
    // ------------------------------------------------------------------
    checks.record(
        "sol:8-serdatr.c145 (SERDATR *018, p.289 composed with p.287's * legend) -- already graded: "
        "hrm/ch2-copper-move-sweep, assertions \"SWEEP ecs=0 cdang=0 vs spec Reading B\" and "
        "\"SWEEP ecs=0 cdang=1 vs spec Reading B\" (uid pair opus:8-appB.c37/sol:8-appB.c36), both PASS at "
        "ecs=0 (OCS) in that package's own accepted run. The claims file's own classify.note cited "
        "hrm/ch8-appb-serial-register-rows for this claim; checked directly, that package's SPECIFICATION.md "
        "lines 59-61 explicitly disclaim testing the * mark at all ('nothing here tests it'), so the note's "
        "package name was wrong even though its uid pair was real. Not re-graded here; this package's own "
        "P-SERDATR (see SERDATR-CDANG0/SERDATR-CDANG1 above) covers SERDATR's fresh ECS ground instead.");
    checks.record(
        "opus:8-regsum.c25 (ADKCONR *010, p.289 composed with p.287's * legend) -- already graded: "
        "hrm/ch2-copper-move-sweep, assertion \"SWEEP ecs=0 cdang=0 vs spec Reading B\" (uid pair "
        "opus:8-appB-adkconr.c2/sol:8-appb-adkconr.c2), PASS at ecs=0 (OCS) in that package's own accepted "
        "run. Not re-graded here; this package does not independently test ADKCONR at ECS, since no claim in "
        "this lot's uid set asks for it once c25 is accounted for this way.");

    checks.note(
        "SERDAT/SERPER/SERDATR's ecs=1/cdang=1 corner, recorded above rather than graded, comes from the "
        "same RTL line as two tracked findings on sibling registers: NM-HRM-044 (DSKPTH $020, same "
        "agnus_copper.v:225 decode, filed against the printed page -- a graded assertion, P1, fails against "
        "p.279 because p.279 explicitly permits Copper init of DSKPTH where p.289 forbids it) and NM-HRM-004 "
        "(the ~-marked $040-$07E range at ecs=1/cdang=0, a different corner of the same line). Unlike DSKPTH, "
        "p.265 and p.283 -- this slice's own Appendix-A pages for ADKCONR/SERDATR/SERDAT/SERPER -- state "
        "nothing about Copper access in either direction, so there is no second manual passage for these four "
        "registers playing p.279's role; see SPECIFICATION.md, \"Related history\", for why that difference "
        "means p.310, not a competing Appendix-A sentence, is what this package now records against at "
        "ecs=1/cdang=1 (NM-HRM-053, withdrawn).");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
