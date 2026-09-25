// Test purpose: Verify which destination registers a Copper MOVE may write,
//   in OCS and ECS, against the HRM register-protection rules.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 21 and 25, Appendix B register list, and Appendix C p. 310; frozen
//   specification 19b2eb2.
// Requirement IDs: COP-03
// Non-goals: What the Copper does after a rejected MOVE is not asserted -
//   the HRM does not define it. IR1 bits 15-9 non-zero, an exhaustive
//   address sweep, AGA extensions and MOVE timing are out of scope.
// Claim ids: ids such as `COP2-control-register.c1` and
//   `ch2-control-register.c1` name one sentence of the manual in this
//   chapter's reading. They carry no reader prefix, and they resolve in the
//   claim files that hrm-testpipeline/acceptance/ch2.json lists under
//   `inventories`.
//   The `COP-nn` Requirement IDs, COP-01 to COP-09, number the requirements of
//   the frozen specification the suite's first version was written against
//   (docs/legacy-names.md); no claim file carries them.
//
// Source conflict, resolved in the specification and repeated here so the code
// is readable on its own: print page 21 states the writable ranges as
// $20/$10/below-$10 while print page 25 and Appendix B state them as
// $80/$40-$7E/$00-$3E. The two are incompatible as byte addresses. Page 25 and
// Appendix B are used as the oracle because they agree with each other and are
// unambiguous full byte addresses.
//
// PROPOSITIONS ROUTED 2026-09-25 (work/2026-09-25_unit-backlog), spec-stage
// only -- quote, restatement and page, RTL not opened for these yet:
//   ch2-the-move-instruction.c9 (p.21) quote "Any register whose address is
//     between $10 and $20 if the Copper danger bit is a 1..." -- restated:
//     the Copper can store data into any register whose address is between
//     $10 and $20 if the Copper danger bit is a 1.
//   ch2-the-move-instruction.c11 (p.21) quote "The Copper cannot write into
//     any register whose address is lower than $10." -- restated: the
//     Copper cannot write into any register whose address is lower than
//     $10.
//   ch2-control-register.c3 (p.26) quote "Those it cannot affect at all are
//     numbered $00 to $3E inclusive. The Copper control register is within
//     this group ($00 to $3E)." -- restated: the registers the Copper
//     cannot affect at all are numbered $00 to $3E inclusive.
//   ch2-control-register.c4 (p.26) quote "inclusive. The Copper control
//     register is within this group ($00 to $3E)." -- restated: the Copper
//     control register lies inside the $00 to $3E group the Copper cannot
//     affect.
//   ch2-control-register.c5 (p.26) quote "The rest of the registers, from
//     $40 to $7E, are protected by a bit in the Copper control register."
//     -- restated: the registers from $40 to $7E are protected by a bit in
//     the Copper control register.
//   ch2-control-register.c8 (p.26) quote "This bit, called CDANG (for
//     Copper Danger Bit) protects all registers numbered between $40 and
//     $7E inclusive." -- restated: CDANG protects all registers numbered
//     between $40 and $7E inclusive.
//   ch2-control-register.c10 (p.26) quote "When CDANG is 0, these registers
//     cannot be written by the Copper." -- restated: when CDANG is 0 the
//     registers from $40 to $7E cannot be written by the Copper.
//   ch2-control-register.c11 (p.26) quote "When CDANG is 1, these registers
//     can be written by the Copper." -- restated: when CDANG is 1 the
//     registers from $40 to $7E can be written by the Copper.
//   These eight restate the same page-21-versus-page-26 protected-range
//   numbers this file already resolved editorially above (by favouring
//   page 25/Appendix B), but the classifier's own conflict_resolution for
//   these eight specific ids says plainly: "NOT RESOLVED HERE: the numbers
//   are recorded, not asserted." Grading either side of a manual conflict
//   the classifier explicitly declined to resolve is not this file's call
//   to make unilaterally, however it happens to line up with the existing
//   T3-T10 measurements below -- so these eight are expected to land on
//   record() lines, not expect() lines, pending confirmation once
//   <unit>.jsonl's routing is read.
//   ch2-appa-copcon.c9 (p.274) quote "01 CDANG Copper danger mode. Allows
//     Copper access to blitter if true." -- restated: bit 1 of COPCON is
//     named CDANG.
//   ch2-appa-copcon.c10 (p.274) same quote -- restated: CDANG is the
//     Copper danger mode bit.
//   B-copcon.addr (p.289) quote "COPCON *02E W A(E) Coprocessor control
//     register (CDANG)" -- restated: Appendix B, address order: COPCON is
//     listed at address $02E. Its own reader flagged it print-only,
//     not-named-in-chapter, added by opus's own completion audit and not
//     cross-read.
//   3-scroll-copper.c2 (p.80, chapter 3, reader sol) quote "For either type
//     of scrolling, resetting of pointers or data-fetch registers can be
//     handled by the Copper during the vertical blanking interval." --
//     restated: for vertical or horizontal scrolling, the Copper can reset
//     data-fetch registers during vertical blanking.
//   Grading versus recording each of these four is decided after
//   <unit>.jsonl and the RTL are opened, per Prompt 7's order of work; see
//   the RECORDED and PASS/FAIL lines below for the disposition actually
//   reached.
//
// MUTATION T1 stimulus red: swapped move_reaches() target from COLOR00 to BLTCON0 (still CDANG=0) -> FAIL as expected.
// MUTATION T2 stimulus red: swapped move_reaches() target from BPL1PTH to BLTCON0 (still CDANG=0) -> FAIL as expected.
// MUTATION T3 stimulus red: flipped cdang argument false->true on the BLTCON0 call -> FAIL as expected (write now succeeds, unblocked assertion fails).
// MUTATION T4 stimulus red: flipped cdang argument false->true on the BLTSIZE call -> FAIL as expected.
// MUTATION T5 stimulus red: flipped cdang argument true->false on the BLTCON0 call -> FAIL as expected (write now blocked).
// MUTATION T6 stimulus red: flipped cdang argument true->false on the BLTSIZE call -> FAIL as expected.
// MUTATION T7 stimulus red: flipped ecs argument false->true on the COPCON call -> FAIL as expected (ECS+CDANG unlocks COPCON, per T10).
// MUTATION T8 stimulus red: swapped move_reaches() target from DMACON to COPCON (still CDANG=1) -> FAIL as expected.
// MUTATION T9 stimulus green: swapped move_reaches() target from BLTCON0 to COLOR00 (ecs=1,cdang=0, unprotected register) -> PASS as expected;
//   confirms the harness/check mechanism does register a write in ECS mode, so T9's baseline FAIL is a genuine
//   DUT non-conformance (Appendix C p.310 range scaling not implemented for ECS+CDANG=0), not a broken assertion.
// MUTATION T9 stimulus red: a second probe for the same assertion - swapped
//   move_reaches() target from BLTCON0 to BLTSIZE (still ecs=1,cdang=0) -> still FAIL;
//   the gap is range-wide (both CDANG-range boundaries), not isolated to one register constant, reinforcing it is a real RTL gap.
// MUTATION T10 stimulus red: flipped ecs argument true->false on the COPCON call -> FAIL as expected (OCS+CDANG=1 still blocks COPCON, per T7).
// MUTATION T11 stimulus red: forced cdang=true and set_cdang_explicitly=true (simulating "reset fails to clear CDANG") -> FAIL as expected.
// MUTATION T12 stimulus red: changed the data written by install_single_move from 0xA55A to 0xA55B, oracle data left at 0xA55A -> FAIL as expected.

#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <verilated.h>
#include "Vagnus_copper.h"

namespace {

// Register addresses are given as byte addresses in the HRM. The Copper
// instruction encodes DA8-1, i.e. the byte address shifted right by one, and
// that is also the width of the module's public register ports.
constexpr uint16_t to_reg(uint16_t byte_address) { return static_cast<uint16_t>(byte_address >> 1); }

constexpr uint16_t COP1LCH = to_reg(0x080);
constexpr uint16_t COP1LCL = to_reg(0x082);
constexpr uint16_t COPJMP1 = to_reg(0x088);
constexpr uint16_t COPINS = to_reg(0x08C);
constexpr uint16_t COPCON = to_reg(0x02E);
constexpr uint16_t IDLE_REG = 0x0FF;

// Destinations probed by the test, chosen at the range boundaries named on
// print page 25 and in Appendix B.
constexpr uint16_t COLOR00 = to_reg(0x180);   // always writable
constexpr uint16_t BPL1PTH = to_reg(0x0E0);   // always writable
constexpr uint16_t DMACON  = to_reg(0x096);   // always writable, just above $80
constexpr uint16_t BLTCON0 = to_reg(0x040);   // CDANG-protected, low boundary
constexpr uint16_t BLTSIZE = to_reg(0x07E);   // CDANG-protected, high boundary

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
        if (condition) {
            std::cout << "PASS: " << label << "\n";
            ++passed;
        } else {
            std::cout << "FAIL: " << label << "\n";
            ++failed;
        }
    }

    // Prints a record for a claim this file does NOT bind to any assertion
    // here. Never touches passed/failed - this is not an assertion.
    void note(const std::string& label) {
        std::cout << "NOTE: " << label << "\n";
    }

    int passed = 0;
    int failed = 0;
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

    // T1, T2, T8 - permitted destinations must actually work, otherwise a
    // negative result below would not be meaningful.
    checks.expect(move_reaches(false, false, COLOR00), "T1 OCS CDANG=0: MOVE to COLOR00 ($180) is written "
                  "[claims: COP2-control-register.c1, ch2-control-register.c1]");
    checks.expect(move_reaches(false, false, BPL1PTH), "T2 OCS CDANG=0: MOVE to BPL1PTH ($0E0) is written "
                  "[claims: COP2-control-register.c1]");
    checks.expect(move_reaches(false, true, DMACON), "T8 OCS CDANG=1: MOVE to DMACON ($096) is written "
                  "[claims: COP2-control-register.c1]");

    // T3, T4 - the CDANG-protected range is blocked while CDANG is clear.
    checks.expect(!move_reaches(false, false, BLTCON0), "T3 OCS CDANG=0: MOVE to BLTCON0 ($040) is blocked "
                  "[claims: COP2-control-register.c2]");
    checks.expect(!move_reaches(false, false, BLTSIZE), "T4 OCS CDANG=0: MOVE to BLTSIZE ($07E) is blocked "
                  "[claims: COP2-control-register.c2, ch2-appb-table.c11]");

    // T5, T6 - and permitted once CDANG is set.
    checks.expect(move_reaches(false, true, BLTCON0),
                  "T5 OCS CDANG=1: MOVE to BLTCON0 ($040) is written "
                  "[claims: COP2-copcon-appa.c6, COP2-copcon-appa.c9, "
                  "COP2-copcon-appa.c11, COP2-copcon-ecs.c8, "
                  "COP2-control-register.c2, COP2-control-register.c13, COP2-the-move-instruction.c13, ch2-appa-copcon.c11, ch2-appa-copcon.c6, ch2-appb-legend.c2, ch2-appb-table.c7, ch2-appc-ecs.c18, ch2-control-register.c7]");
    checks.expect(move_reaches(false, true, BLTSIZE),
                  "T6 OCS CDANG=1: MOVE to BLTSIZE ($07E) is written "
                  "[claims: COP2-copcon-appa.c6, COP2-copcon-appa.c9, "
                  "COP2-copcon-appa.c11, COP2-copcon-ecs.c8, "
                  "COP2-control-register.c2, COP2-control-register.c13]");

    // T7 - below the protected range nothing is writable in OCS, not even with
    // CDANG set. COPCON itself lives here, so the Copper cannot set its own
    // danger bit.
    checks.expect(!move_reaches(false, true, COPCON), "T7 OCS CDANG=1: MOVE to COPCON ($02E) is blocked "
                  "[claims: COP2-control-register.c3, COP2-using-the-copper-registers.c3, COP2-control-register.c6, ch2-appb-table.c2]");

    // T9, T10 - Appendix C p. 310: ECS shifts the scale by one step. With
    // CDANG clear the ECS Copper already reaches the blitter range, and with
    // CDANG set it reaches every chip register.
    // T9 is expected RED: this is NM-HRM-004, not a broken assertion. See
    // docs/findings.md - with ECS and CDANG clear appendix C p.310 gives the
    // Copper the byte range $DFF03E-$DFF07E, the RTL accepts only $080 and above
    // at either chipset, and the 32 byte addresses $040-$07E are falsely
    // rejected. Making this assertion pass would delete the finding.
    //
    // Challenged 11.09.2026 by the chapter-2 appendix classification, which
    // preferred appendix B's marker legend to appendix C. Appendix B p.288 sends
    // the reader the other way: registers marked "(E)" in the chip column "have
    // been changed in the Enhanced Chip Set", and for what changed it points at
    // appendix C. COPCON is one of them ($02E, chip column A(E)). Neither that
    // legend nor the chapter body on p.26 mentions ECS at all, so neither can
    // govern the ECS case. Oracle unchanged.
    checks.expect(move_reaches(true, false, BLTCON0), "T9 ECS CDANG=0: MOVE to BLTCON0 ($040) is written (Appendix C p.310) [NM-HRM-004, expected red], ch2-appc-ecs.c21");
    checks.expect(move_reaches(true, true, COPCON),
                  "T10 ECS CDANG=1: MOVE to COPCON ($02E) is written (Appendix C p.310) "
                  "[claims: COP2-copcon-ecs.c11, ch2-appa-copcon.c3.s2, ch2-appb-legend.c9, ch2-appb-table.c5.s2, ch2-appc-ecs.c12]");

    // T11 - print page 25 warns CDANG is cleared by reset. Without any COPCON
    // write the Copper must behave as CDANG = 0.
    checks.expect(!move_reaches(false, false, BLTCON0, /*set_cdang_explicitly=*/false),
                  "T11 OCS after reset, no COPCON write: MOVE to BLTCON0 ($040) is blocked "
                  "[claims: COP2-control-register.c15]");

    // T12 - the 16 bits of IR2 reach the destination unmodified.
    {
        CopperHarness h(false);
        h.set_cdang(false);
        h.install_single_move(COLOR00, 0xA55A);
        h.start_list();
        h.run_cycles(600);
        checks.expect(h.saw_move(COLOR00, 0xA55A),
                      "T12 OCS: IR2 data $A55A arrives at COLOR00 unmodified "
                      "[claims: COP2-copins-kinds.c2, COP2-what-is-a-copper-instruction.c9, ch2-about-the-copper.c13, ch2-appa-copins.c8, ch2-what-is-a-copper-instruction.c4, ch2-what-is-a-copper-instruction.c7]");
    }

    // ------------------------------------------------------------------
    // Appendix claims considered for this package and left UNBOUND, with
    // reasons. None of these change any assertion above; they are a printed
    // record only.
    // ------------------------------------------------------------------
    checks.note("UNBOUND (COPCON address/access-type exercised as stimulus only via "
        "set_cdang(), not independently asserted - no bit-arithmetic walk like "
        "agnus_copper_locreg_width's RM1/RM3 isolates $02E as COPCON's own address; "
        "T5/T6's contrast with T3/T4 shows writing there has *some* gating effect, "
        "but not specifically that $02E is the address doing it): "
        "COP2-copcon-appa.c1, COP2-copcon-appa.c2, COP2-copcon-appa.c4, "
        "COP2-copcon-ecs.c1, COP2-copcon-ecs.c2, COP2-copcon-ecs.c3, "
        "COP2-copcon-ecs.c5, COP2-copcon-ecs.c6, ch2-appa-copcon.c1, ch2-appa-copcon.c2, ch2-appa-copcon.c4, ch2-appb-table.c3, ch2-appb-table.c4, ch2-appb-table.c6, ch2-appc-ecs.c13, ch2-appc-ecs.c14, ch2-appc-ecs.c16, ch2-appc-ecs.c17, ch2-the-move-instruction.c10");
    checks.note("UNBOUND (COP2-copcon-ecs.c7: the one COPCON bit \"is the danger bit "
        "named CDANG\" - the name again, and a second thing besides: that no OTHER "
        "COPCON bit exists. T5/T6 show the driven bit changes MOVE access, which is "
        "the separate claim .c8; the other-bit sweep this would need is itself unbound "
        "a few lines below)");
    checks.note("UNBOUND (COP2-copcon-appa.c10: \"Bit 01 is named CDANG\" - a name, not "
        "a requirement, and nothing in the module can disagree with one. T5/T6 listed "
        "this claim until 11.09.2026; they drive set_cdang(), which writes the VALUE "
        "0x0002, and observe the range unlock - which happens identically whatever the "
        "bit is called, so neither assertion could ever have failed on it. The sibling "
        "c9, that the control occupies bit 01, stays bound: writing 0x0002 unlocks "
        "nothing if the module reads a different bit)");
    checks.note("UNBOUND (COP2-copcon-appa.c5: \"COPCON has one implemented control "
        "bit\" - set_cdang() only ever writes 0x0000 or 0x0002; no assertion sweeps "
        "COPCON's other bits to show they are unimplemented/inert) [claims: ch2-appa-copcon.c5]");
    checks.note("UNBOUND COP2-control-register.c8 (\"COPCON bit 1 is the only bit "
        "currently used by the system\") -- no assertion measures it. Same gap as "
        "COP2-copcon-appa.c5 above, independently found: set_cdang() only ever writes "
        "0x0000 or 0x0002 (bit 1 alone); no assertion sweeps COPCON's other 15 bits to "
        "show they have no effect on the accepted-register set., ch2-control-register.c6");
    checks.note("UNBOUND (exact byte-address floor/ceiling not discriminated: "
        "COP2-copcon-ecs.c9 and COP2-copcon-ecs.c10 state the OCS+CDANG=1 range as "
        "$DFF03E through $DFF07E, and COP2-copcon-ecs.c12, COP2-copcon-ecs.c13 restate "
        "the same two figures for the ECS CDANG=0 case. T3-T7 above probe BLTCON0 "
        "($040) and BLTSIZE ($07E) and COPCON "
        "($02E), but never probe $03E itself or anything between $02E and $040 "
        "exclusive - so a RTL that actually gates at $040, not $03E, would pass every "
        "assertion here unchanged. See agnus_copper_move_sweep, which uses the same "
        "$040 floor under the name \"Reading B\" rather than $03E, and whose own "
        "ecs=1/cdang=0 sweep has the identical gap: it asserts the whole accepted SET "
        "against a $040-based expectation, never against $03E specifically) [claims: ch2-appb-table.c1, ch2-appc-ecs.c19]");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
