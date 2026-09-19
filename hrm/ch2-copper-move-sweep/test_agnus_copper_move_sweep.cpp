// Test purpose: Exhaustively sweep DA8-1 (all 256 values of the MOVE first
// word's destination field) on a Copper MOVE and enumerate the full accepted
// set on reg_address_out, per chipset mode (ecs) and CDANG state.
// Test class: HRM_CONFORMANCE
// Primary oracle: work/2026-09-05_fpga-079_copper-move-sweep_testspecification.md
//   (private lab workspace, not published; the claim ids below are the
//   public record)
// (fpga-lab, committed before this file), itself built from the Amiga
// Hardware Reference Manual, 3rd ed., Chapter 2, printed pp. 19-21, 25, 26,
// 29, 35, 36. OCS (ecs=0) accepted sets are asserted against the
// specification's chosen "Reading B" (page 26 + register map: never $00-$3E,
// CDANG-gated $40-$7E, always >=$80, in byte addresses / DA8-1 = byte>>1).
// ECS (ecs=1): this header said until 06.09.2026 that the ECS sets were
// asserted against the SAME chapter-2-derived sets, "because chapter 2 states
// no ecs dependency (p36:49-52 defers ECS behaviour to Appendix C, outside this
// task's source set)". **That is no longer what the file does.** Appendix C was
// brought into the corpus on 05.09.2026, and the ECS rows below are asserted
// against p310's own statement of the CDANG ranges - which is what makes the
// ecs=1/cdang=0 row a valid red finding (NM-HRM-004) rather than a literal-reading
// choice. Stale header found by independent review; the assertions were already
// right, only this paragraph described an older version of them.
// Requirement IDs: COP-03 (covers PKG-MOVE-DECODE, PKG-CDANG from
// work/2026-08-29_fpga-074_klassifikation.md)
// Non-goals: MOVE cycle timing/count (agnus_copper_cycles,
// agnus_copper_cycle_times); IR1 bit-0 MOVE/WAIT/SKIP type decoding
// (PKG-DECODE); IR2 data passthrough; boundary-only coverage -- that is
// sim/hrm/ch2-copper-move-range (COP-03, frozen, not modified here). This
// test is the exhaustive complement the frozen test explicitly declines to
// be.
// Disclosure: RTL (src/minimig-aga/agnus_copper.v) was read only after the
// specification above was committed to fpga-lab (commit e0b2d56). The
// module's illegal-access block (around line 220) computes, on the MOVE
// first word held in `ir2[8:1]` at FETCH2 (a one-word pipeline delay
// intrinsic to this module's ir1/ir2 shift, see comment at line 211):
//   illegal = (DA8-1 <= 8'h3F && !cdang) || (DA8-1 <= 8'h1F && !ecs)
// which the module's own comment restates as: CDANG=0 (OCS/ECS): $080-$1FE
// allowed; CDANG=1 (OCS): $040-$1FE allowed; CDANG=1 (ECS): $000-$1FE
// allowed. This matches the specification's Reading B thresholds for ecs=0
// exactly (accepted = DA8-1 in [0x40,0xFF] when CDANG=0, [0x20,0xFF] when
// CDANG=1), predicts no ecs=0/ecs=1 difference when CDANG=0, and predicts
// that ecs=1,CDANG=1 accepts everything (DA8-1 [0x00,0xFF]) -- 32 addresses
// (DA8-1 0x00-0x1F) beyond the chapter-2-blind expected set, matching the
// known Appendix-C-sourced ECS extension already exercised by the frozen
// test's T10. The sweep below measures this empirically rather than taking
// the comment's word for it, and reports any further deviation as a finding.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// Discrimination proof (mutation, reverted before commit): each assertion
// category was independently mutated, rebuilt and rerun, confirmed red, then
// reverted and rerun, confirmed green again.
//   - control case: CONTROL_REG -> $00 in the first CTRL check -> that
//     check went red (others unaffected).
//   - set-equality: expected_cdang0 lower bound $40 -> $41 -> the ecs=0/
//     ecs=1 cdang=0 checks went red, reporting "falsely accepted = {$40}".
//   - bits15-9 independence: candidate CONTROL_REG -> $00 in the accepted-
//     candidate loop -> that check went red on the first iteration.
//   - reset-clears-CDANG: assert_reset() call removed -> that check went
//     red (CDANG stayed 1, $20 was accepted instead of rejected).
// Machine-readable records for tools/check_gates.py --tests. Only mutations
// actually performed appear here.
// MUTATION DECODE stimulus red probe_emitted returned the emitted register xor 1, simulating a misrouted destination
// MUTATION CTRL stimulus red the control destination moved from COLOR00 ($180, always writable) to BLTCON0 ($040, CDANG-gated), so the case that must be accepted under every combination is one that is legitimately refused without CDANG
// MUTATION RESET stimulus red the reset removed after CDANG is set, so CDANG survives and the MOVE the assertion expects to be refused is accepted
// MUTATION SWEEP stimulus red: the OCS CDANG=0 sweep run with CDANG SET --
//   `sweep_accepted_set(false, false)` changed to `sweep_accepted_set(false, true)`,
//   the expected set left as {$40..$FF} -> 10 passed, 2 failed, and the newly red
//   row is SWEEP ecs=0 cdang=0 exactly (the accepted set becomes {$20..$FF}).
//   SWEEP ecs=1 cdang=0 is red at baseline as well and stays so: that is NM-HRM-004,
//   not this mutation. Reverted: 11 passed, 1 failed.
//   The four assertions here all begin their label with the word SWEEP, so the
//   gate reads one id for the family; this record covers it.
// MUTATION BITS15-9 stimulus red the ecs=0/cdang=0 hi-bits loop's probed register changed from CONTROL_REG ($C0, always accepted) to $00 (never accepted under cdang=0), so all_stable goes false on the first iteration (hi=0) instead of holding true across the 128-value sweep
//
// The CTRL mutation turned the two cdang=0 rows red and left the two cdang=1
// rows green, which is the correct discrimination and not a partial result:
// BLTCON0 really is writable once CDANG is set. It also took the BITS15-9
// sweep down, which shares CONTROL_REG - a dependency, not a second defect.
//
// Baseline before and after each mutation: 11 passed, 1 failed (the known,
// documented ecs=1/cdang=0 ECS-extension divergence, see above; corrected
// 10.09.2026 -- this sentence said cdang=1 and undercounted by one check
// (DECODE was added after it was written), verified by rerunning `make test`).

#include <cstdint>
#include <functional>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <verilated.h>
#include "Vagnus_copper.h"

namespace {

// Register addresses are given as byte addresses in the HRM. The Copper
// instruction encodes DA8-1, i.e. the byte address shifted right by one,
// which is also the width of the module's public register ports.
constexpr uint16_t to_reg(uint16_t byte_address) { return static_cast<uint16_t>(byte_address >> 1); }

constexpr uint16_t COP1LCH = to_reg(0x080);
constexpr uint16_t COP1LCL = to_reg(0x082);
constexpr uint16_t COPJMP1 = to_reg(0x088);
constexpr uint16_t COPINS = to_reg(0x08C);
constexpr uint16_t COPCON = to_reg(0x02E);

// A fixed control destination, always in the "always writable" group under
// every reading and every (ecs, CDANG) combination this test exercises
// (COLOR00, $180 byte -> DA8-1 $C0).
constexpr uint16_t CONTROL_REG = to_reg(0x180);

constexpr uint32_t LIST_ADDRESS = 0x00002000;

struct DmaEvent {
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
        dut->reg_address_in = 0x0FF;

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
    // hi_bits carries data_in[15:9] of the MOVE's first word (normally
    // don't-care per the HRM; used to test that independence).
    void install_single_move(uint16_t reg, uint16_t data, uint16_t hi_bits = 0) {
        const uint16_t word0 = static_cast<uint16_t>((reg << 1) | (hi_bits << 9));
        memory[LIST_ADDRESS + 0] = word0;
        memory[LIST_ADDRESS + 1] = data;
        memory[LIST_ADDRESS + 2] = 0xFFFF;
        memory[LIST_ADDRESS + 3] = 0xFFFE;
    }

    void set_cdang(bool value) {
        write_reg(COPCON, value ? 0x0002 : 0x0000);
        run_cycles(2);
    }

    void assert_reset() {
        dut->reset = 1;
        run_cycles(4);
        dut->reset = 0;
        run_cycles(4);
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

    // Every accepted DMA cycle observed since construction, in order. For a
    // freshly started single-MOVE list this is expected to be:
    //   [0] pointer-load dummy cycle (reg == 0x0FF)
    //   [1] instruction fetch of the first word (reg == COPINS)
    //   [2] present only if the MOVE was accepted; reg is the destination
    //       actually placed on reg_address_out for that cycle.
    // Counting by position (rather than filtering by value, as the frozen
    // agnus_copper_move_range test does) is required here because DA8-1
    // $FF and DA8-1 == COPINS's own field value are legitimate sweep
    // candidates and must not be silently confused with the idle/fetch
    // markers that happen to carry the same numeric value.
    //
    // What that costs, raised by the independent validation on 11.09.2026 and
    // recorded here rather than repaired: reading position [2] encodes THIS
    // implementation's cycle sequence, dummy cycle included. A conforming
    // Copper that fetched or strobed in a different order would be classified
    // wrongly by every assertion in this package, and the expected register
    // sets -- which are well sourced from the manual -- would not save it.
    //
    // The alternative the validator proposed, identifying the destination
    // event by what it is rather than where it sits, is the one the paragraph
    // above already rejects, and for a reason that still holds: it misreads
    // exactly the two sweep candidates that collide with the markers. So the
    // positional read stays, and what is added is the disclosure that the
    // measurement is implementation-shaped even though the oracle is not.
    std::vector<DmaEvent> events;

    bool dma_allowed = false;

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

    void cycle(bool cpu_write = false, uint16_t cpu_reg = 0x0FF, uint16_t cpu_data = 0) {
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
            events.push_back({reg, data});
        } else {
            dut->ackdma = 0;
            dut->reg_address_in = 0x0FF;
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

// Runs a single MOVE of candidate `reg` and returns the destination register
// the module ACTUALLY emitted on reg_address_out for it, or -1 if the MOVE was
// refused.
//
// The first revision of this file returned `events.size() >= 3` and the sweep
// then recorded the *injected* candidate as accepted. That measured "did a
// third DMA cycle occur", not "which register was addressed": a regression
// routing every MOVE to one constant wrong destination would have passed all
// four set comparisons unchanged. Found in cross-lineage validation,
// 05.09.2026. Returning the emitted value is what makes the sweep a
// measurement of the decoder rather than of the FSM's cycle count.
int probe_emitted(bool ecs, bool cdang, uint16_t reg, uint16_t hi_bits = 0,
                  bool set_cdang_explicitly = true) {
    CopperHarness h(ecs);
    if (set_cdang_explicitly) {
        h.set_cdang(cdang);
    }
    h.install_single_move(reg, 0x0ABC, hi_bits);
    h.start_list();
    h.run_cycles(600);
    if (h.events.size() < 3) return -1;
    return static_cast<int>(h.events[2].reg);
}

bool probe(bool ecs, bool cdang, uint16_t reg, uint16_t hi_bits = 0, bool set_cdang_explicitly = true) {
    return probe_emitted(ecs, cdang, reg, hi_bits, set_cdang_explicitly) >= 0;
}

// Sweeps all 256 DA8-1 values for one (ecs, cdang) combination and returns
// the accepted set.
std::set<int> sweep_accepted_set(bool ecs, bool cdang) {
    std::set<int> accepted;
    for (int reg = 0; reg <= 0xFF; ++reg) {
        if (probe(ecs, cdang, static_cast<uint16_t>(reg))) {
            accepted.insert(reg);
        }
    }
    return accepted;
}

// Every accepted candidate must be emitted as itself. Collects the ones that
// are not: {injected, emitted} pairs. An empty result is the claim "the
// decoder is bit-accurate across the whole accepted set", which the acceptance
// sweep above cannot make on its own.
std::vector<std::pair<int, int>> sweep_misrouted(bool ecs, bool cdang) {
    std::vector<std::pair<int, int>> wrong;
    for (int reg = 0; reg <= 0xFF; ++reg) {
        const int emitted = probe_emitted(ecs, cdang, static_cast<uint16_t>(reg));
        if (emitted >= 0 && emitted != reg) {
            wrong.emplace_back(reg, emitted);
        }
    }
    return wrong;
}

std::set<int> expected_set(int lo_inclusive) {
    std::set<int> s;
    for (int v = lo_inclusive; v <= 0xFF; ++v) {
        s.insert(v);
    }
    return s;
}

std::string format_ranges(const std::set<int>& values) {
    if (values.empty()) {
        return "{}";
    }
    std::ostringstream out;
    out << "{";
    bool first = true;
    int range_start = *values.begin();
    int prev = range_start;
    auto flush = [&](int end) {
        if (!first) {
            out << ", ";
        }
        first = false;
        if (range_start == end) {
            out << "$" << std::hex << range_start;
        } else {
            out << "$" << std::hex << range_start << "-$" << std::hex << end;
        }
    };
    for (auto it = std::next(values.begin()); it != values.end(); ++it) {
        if (*it != prev + 1) {
            flush(prev);
            range_start = *it;
        }
        prev = *it;
    }
    flush(prev);
    out << "}";
    return out.str();
}

// Reports a set-equality check against `expected`, printing the concrete
// difference (falsely accepted / falsely rejected addresses, as ranges) on
// mismatch instead of failing 256 individual assertions.
void expect_set_equals(Checks& checks, const std::set<int>& measured, const std::set<int>& expected,
                        const std::string& label) {
    if (measured == expected) {
        checks.expect(true, label + " (accepted = " + format_ranges(measured) + ")");
        return;
    }
    std::set<int> falsely_accepted;
    std::set<int> falsely_rejected;
    for (int v : measured) {
        if (!expected.count(v)) falsely_accepted.insert(v);
    }
    for (int v : expected) {
        if (!measured.count(v)) falsely_rejected.insert(v);
    }
    std::ostringstream detail;
    detail << label << " -- MISMATCH: measured = " << format_ranges(measured)
           << ", expected = " << format_ranges(expected)
           << "; falsely accepted = " << format_ranges(falsely_accepted)
           << " (" << falsely_accepted.size() << "), falsely rejected = " << format_ranges(falsely_rejected)
           << " (" << falsely_rejected.size() << ")";
    checks.expect(false, detail.str());
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // --- Control cases: the measurement itself must be trustworthy before a
    // rejection anywhere else can be believed. CONTROL_REG ($180/COLOR00,
    // DA8-1 $C0) is in the "always writable" group under both HRM readings
    // and under every (ecs, CDANG) combination.
    checks.expect(probe(false, false, CONTROL_REG), "CTRL ecs=0 cdang=0: CONTROL_REG ($C0) accepted");
    checks.expect(probe(false, true, CONTROL_REG), "CTRL ecs=0 cdang=1: CONTROL_REG ($C0) accepted");
    checks.expect(probe(true, false, CONTROL_REG), "CTRL ecs=1 cdang=0: CONTROL_REG ($C0) accepted");
    checks.expect(probe(true, true, CONTROL_REG), "CTRL ecs=1 cdang=1: CONTROL_REG ($C0) accepted");

    // --- Exhaustive sweeps: the core of this task. Full accepted set per
    // (ecs, CDANG), asserted against the specification's expected sets
    // (spec sections 4 and 5).
    const std::set<int> expected_cdang0 = expected_set(0x40);  // {$40..$FF}, 192 values
    const std::set<int> expected_cdang1 = expected_set(0x20);  // {$20..$FF}, 224 values

    const auto ocs_cdang0 = sweep_accepted_set(false, false);
    const auto ocs_cdang1 = sweep_accepted_set(false, true);
    const auto ecs_cdang0 = sweep_accepted_set(true, false);
    const auto ecs_cdang1 = sweep_accepted_set(true, true);

    expect_set_equals(checks, ocs_cdang0, expected_cdang0,
                       "SWEEP ecs=0 cdang=0 vs spec Reading B (HRM_CONFORMANCE, primary oracle) "
                       "[claims: COP2-about-the-copper.c5, COP2-control-register.c4, ch2-about-the-copper.c6, ch2-appb-table.c10, ch2-appb-table.c12, ch2-appb-table.c13, ch2-appb-table.c46, ch2-appb-table.c8, ch2-appb-table.c9, appB-p289-068.c2, appB-p289-06a.c2, appB-p289-06c.c2, appB-p289-06e.c2, appB-p289-adkconr.c7, appB-p289-bltafwm.c6, appB-p289-bltalwm.c6, appB-p289-bltamod.c6, appB-p289-bltapth.c11, appB-p289-bltaptl.c10, appB-p289-bltbdat.c8, appB-p289-bltbmod.c6, appB-p289-bltbpth.c10, appB-p289-bltbptl.c10, appB-p289-bltcdat.c8, appB-p289-bltcmod.c6, appB-p289-bltcon0l.c8, appB-p289-bltcon1.c7, appB-p289-bltcpth.c10, appB-p289-bltcptl.c10, appB-p289-bltdmod.c6, appB-p289-bltdpth.c10, appB-p289-bltdptl.c10, appB-p289-bltsize.c8, appB-p289-bltsizh.c9, appB-p289-bltsizv.c8, appB-p289-clxdat.c7, appB-p289-dmaconr.c8, appB-p289-dskbytr.c7, appB-p289-dskdat.c7, appB-p289-dskdatr.c8, appB-p289-dsklen.c6, appB-p289-dskpth.c12, appB-p289-dskptl.c10, appB-p289-intenar.c6, appB-p289-intreqr.c6, appB-p289-joy0dat.c7, appB-p289-joy1dat.c7, appB-p289-joytest.c7, appB-p289-pot0dat.c8, appB-p289-pot1dat.c8, appB-p289-potgo.c7, appB-p289-potgor.c7, appB-p289-refptr.c7, appB-p289-serdat.c7, appB-p289-serdatr.c7, appB-p289-serper.c7, appB-p289-strequ.c9, appB-p289-strhor.c8, appB-p289-strlong.c8, appB-p289-strvbl.c8, appB-p289-vhposr.c7, appB-p289-vhposw.c7, appB-p289-vposr.c8, appB-p289-vposw.c7, appB-p290-076.c2, appB-p290-07a.c2, appB-p290-bltadat.c7, appB-p290-sprhdat.c8, opus:8-appB.c12, sol:8-appB.c11, opus:8-appB.c17, sol:8-appB.c16, opus:8-appB.c22, sol:8-appB.c21, opus:8-appB.c27, sol:8-appB.c26, opus:8-appB.c32, sol:8-appB.c31, opus:8-appB.c37, sol:8-appB.c36, opus:8-appB.c42, sol:8-appB.c41, opus:8-appB.c47, sol:8-appB.c46, opus:8-appB.c52, sol:8-appB.c51, opus:8-appB.c57, sol:8-appB.c56, opus:8-appB-dskdatr.c3, sol:8-appb-dskdatr.c3, opus:8-appB-adkconr.c2, sol:8-appb-adkconr.c2, opus:8-appB-dskbytr.c2, sol:8-appb-dskbytr.c2, opus:8-appB-dskpth.c3, sol:8-appb-dskpth.c5, opus:8-appB-dskptl.c3, sol:8-appb-dskptl.c5, opus:8-appB-dsklen.c2, sol:8-appb-dsklen.c2, opus:8-appB-dskdat.c3, sol:8-appb-dskdat.c3, opus:8-appB-dsksync.c2, sol:8-appb-dsksync.c2]");
    expect_set_equals(checks, ocs_cdang1, expected_cdang1,
                       "SWEEP ecs=0 cdang=1 vs spec Reading B (HRM_CONFORMANCE, primary oracle) "
                       "[claims: COP2-about-the-copper.c5, COP2-control-register.c13, appB-p289-068.c2, appB-p289-06a.c2, appB-p289-06c.c2, appB-p289-06e.c2, appB-p289-adkconr.c7, appB-p289-bltafwm.c6, appB-p289-bltalwm.c6, appB-p289-bltamod.c6, appB-p289-bltapth.c11, appB-p289-bltaptl.c10, appB-p289-bltbdat.c8, appB-p289-bltbmod.c6, appB-p289-bltbpth.c10, appB-p289-bltbptl.c10, appB-p289-bltcdat.c8, appB-p289-bltcmod.c6, appB-p289-bltcon0l.c8, appB-p289-bltcon1.c7, appB-p289-bltcpth.c10, appB-p289-bltcptl.c10, appB-p289-bltdmod.c6, appB-p289-bltdpth.c10, appB-p289-bltdptl.c10, appB-p289-bltsize.c8, appB-p289-bltsizh.c9, appB-p289-bltsizv.c8, appB-p289-clxdat.c7, appB-p289-dmaconr.c8, appB-p289-dskbytr.c7, appB-p289-dskdat.c7, appB-p289-dskdatr.c8, appB-p289-dsklen.c6, appB-p289-dskpth.c12, appB-p289-dskptl.c10, appB-p289-intenar.c6, appB-p289-intreqr.c6, appB-p289-joy0dat.c7, appB-p289-joy1dat.c7, appB-p289-joytest.c7, appB-p289-pot0dat.c8, appB-p289-pot1dat.c8, appB-p289-potgo.c7, appB-p289-potgor.c7, appB-p289-refptr.c7, appB-p289-serdat.c7, appB-p289-serdatr.c7, appB-p289-serper.c7, appB-p289-strequ.c9, appB-p289-strhor.c8, appB-p289-strlong.c8, appB-p289-strvbl.c8, appB-p289-vhposr.c7, appB-p289-vhposw.c7, appB-p289-vposr.c8, appB-p289-vposw.c7, appB-p290-076.c2, appB-p290-07a.c2, appB-p290-bltadat.c7, appB-p290-sprhdat.c8, opus:8-appB.c12, sol:8-appB.c11, opus:8-appB.c17, sol:8-appB.c16, opus:8-appB.c22, sol:8-appB.c21, opus:8-appB.c27, sol:8-appB.c26, opus:8-appB.c32, sol:8-appB.c31, opus:8-appB.c37, sol:8-appB.c36, opus:8-appB.c42, sol:8-appB.c41, opus:8-appB.c47, sol:8-appB.c46, opus:8-appB.c52, sol:8-appB.c51, opus:8-appB.c57, sol:8-appB.c56, opus:8-appB-dsksync.c2, sol:8-appb-dsksync.c2]");
    // The two ECS rows are asserted against APPENDIX C, not against a
    // chapter-2 extrapolation. Chapter 2 states no ECS rule at all, so
    // asserting one was oracle invention - and it put the red in the wrong
    // row: it reported NanoMig as violating a rule the manual does not make,
    // while the row where NanoMig really does deviate passed. Found in
    // cross-lineage validation, 05.09.2026; corrected here.
    //
    // Appendix C, on COPCON, in the manual's own words:
    //   "Another register where the interpretation has been extended for the
    //    ECS is COPCON. ... Now, in the ECS, if this bit is set, the Copper
    //    can access all of the Amiga chip registers. If this bit is clear,
    //    the Copper can access the address range from $DFF03E through
    //    $DFF07E, THE SAME RANGE AS WHEN THE DANGER BIT IS SET IN THE
    //    STANDARD CHIP SET."
    //
    // ECS + CDANG=1 is therefore every candidate. ECS + CDANG=0 is expressed
    // through the equivalence the appendix itself states - the OCS CDANG=1
    // range - rather than by re-deriving $03E/$07E into DA8-1 here. The
    // conversion is byte-offset to word index and a one-off in it would
    // silently produce a wrong oracle, so the manual's own equality is the
    // safer formulation.
    const std::set<int> expected_ecs_cdang1 = expected_set(0x00);   // "all of the chip registers"
    const std::set<int>& expected_ecs_cdang0 = expected_cdang1;     // "the same range as OCS with CDANG set"

    expect_set_equals(checks, ecs_cdang0, expected_ecs_cdang0,
                       "SWEEP ecs=1 cdang=0 vs Appendix C (HRM_CONFORMANCE; the appendix gives the ECS "
                       "CDANG=0 range as equal to the OCS CDANG=1 range). Expected to be RED: this is "
                       "the deviation already tracked as NM-HRM-004 "
                       "[claims: COP2-copcon-ecs.c14, ch2-appc-ecs.c22]");
    expect_set_equals(checks, ecs_cdang1, expected_ecs_cdang1,
                       "SWEEP ecs=1 cdang=1 vs Appendix C (HRM_CONFORMANCE; \"the Copper can access all "
                       "of the Amiga chip registers\") "
                       "[claims: COP2-copcon-ecs.c11, ch2-appc-ecs.c20]");

    // --- Bit independence (data_in[15:9] of the MOVE first word must not
    // affect the decode; PKG-MOVE-DECODE, Table 2-2 "X" bits / COP2-summary-
    // of-copper-instructions.c1,c7,c13,c19,c25,c31,c37,c97 / COP2-summary.c1).
    // Swept over the full 7-bit field (128 values), not just one-hot, for
    // an accepted and a rejected candidate.
    {
        bool all_stable = true;
        for (int hi = 0; hi <= 0x7F && all_stable; ++hi) {
            if (!probe(false, false, CONTROL_REG, static_cast<uint16_t>(hi))) {
                all_stable = false;
            }
        }
        checks.expect(all_stable,
                       "BITS15-9 ecs=0 cdang=0: CONTROL_REG stays accepted for all 128 values of data_in[15:9] "
                       "[claims: COP2-copins-encoding.c1, COP2-copins-encoding.c7, COP2-copins-encoding.c13, "
                       "COP2-copins-encoding.c19, COP2-copins-encoding.c25, COP2-copins-encoding.c31, "
                       "COP2-copins-encoding.c37, COP2-the-move-instruction.c6, "
                       "COP2-summary-of-copper-instructions.c1, COP2-summary-of-copper-instructions.c7, "
                       "COP2-summary-of-copper-instructions.c13, COP2-summary-of-copper-instructions.c19, "
                       "COP2-summary-of-copper-instructions.c25, COP2-summary-of-copper-instructions.c31, "
                       "COP2-summary-of-copper-instructions.c37, COP2-summary-of-copper-instructions.c97, ch2-appa-copins.c14, ch2-appa-copins.c20, ch2-appa-copins.c26, ch2-appa-copins.c32, ch2-appa-copins.c38, ch2-appa-copins.c44, ch2-appa-copins.c50, ch2-summary-of-copper-instructions.c1, ch2-summary-of-copper-instructions.c13, ch2-summary-of-copper-instructions.c19, ch2-summary-of-copper-instructions.c25, ch2-summary-of-copper-instructions.c31, ch2-summary-of-copper-instructions.c37, ch2-summary-of-copper-instructions.c7, ch2-the-move-instruction.c6]");
    }
    {
        constexpr uint16_t NEVER_REG = 0x00;  // never-writable in both readings, both CDANG states
        bool all_stable = true;
        for (int hi = 0; hi <= 0x7F && all_stable; ++hi) {
            if (probe(false, true, NEVER_REG, static_cast<uint16_t>(hi))) {
                all_stable = false;
            }
        }
        checks.expect(all_stable,
                       "BITS15-9 ecs=0 cdang=1: NEVER_REG ($00) stays rejected for all 128 values of data_in[15:9] "
                       "[claims: COP2-copins-encoding.c1, COP2-copins-encoding.c7, COP2-copins-encoding.c13, "
                       "COP2-copins-encoding.c19, COP2-copins-encoding.c25, COP2-copins-encoding.c31, "
                       "COP2-copins-encoding.c37, COP2-the-move-instruction.c6, "
                       "COP2-summary-of-copper-instructions.c1, COP2-summary-of-copper-instructions.c7, "
                       "COP2-summary-of-copper-instructions.c13, COP2-summary-of-copper-instructions.c19, "
                       "COP2-summary-of-copper-instructions.c25, COP2-summary-of-copper-instructions.c31, "
                       "COP2-summary-of-copper-instructions.c37, COP2-summary-of-copper-instructions.c97]");
    }

    // --- Reset clears CDANG (COP2-ctrl.c14 / COP2-control-register.c15):
    // set CDANG, reset, and confirm the gated address ($20, in the CDANG-
    // gated group under Reading B) is rejected again without a fresh COPCON
    // write.
    {
        CopperHarness h(false);
        h.set_cdang(true);
        h.assert_reset();
        h.install_single_move(0x20, 0x0ABC);
        h.start_list();
        h.run_cycles(600);
        checks.expect(h.events.size() < 3,
                       "RESET ecs=0: CDANG=1 set, then reset, then MOVE to $20 (CDANG-gated) is rejected -- "
                       "reset clears CDANG "
                       "[claims: COP2-copcon-appa.c7, COP2-copcon-appa.c8, COP2-control-register.c15, ch2-appa-copcon.c7, ch2-appa-copcon.c8]");
    }

    // ------------------------------------------------------------------
    // DECODE - the accepted candidate must also be the destination emitted.
    //
    // Every set comparison above only asks whether a MOVE got through. None of
    // them looks at reg_address_out, so a decoder that let the right addresses
    // through but emitted the wrong one would pass them all. This closes that,
    // across all four modes and all 256 candidates.
    // ------------------------------------------------------------------
    {
        std::vector<std::pair<int, int>> all_wrong;
        for (bool ecs : {false, true}) {
            for (bool cdang : {false, true}) {
                for (const auto& w : sweep_misrouted(ecs, cdang)) all_wrong.push_back(w);
            }
        }
        std::ostringstream detail;
        detail << "DECODE every accepted MOVE is emitted as the register it named, "
               << "all four modes, 256 candidates each "
               << "[claims: COP2-copins-encoding.c43, COP2-copins-encoding.c49, "
               << "COP2-copins-encoding.c55, COP2-copins-encoding.c61, COP2-copins-encoding.c67, "
               << "COP2-copins-encoding.c73, COP2-copins-encoding.c79, COP2-copins-encoding.c85, "
               << "COP2-the-move-instruction.c3, COP2-the-move-instruction.c5, "
               << "COP2-summary-of-copper-instructions.c43, COP2-summary-of-copper-instructions.c49, "
               << "COP2-summary-of-copper-instructions.c55, COP2-summary-of-copper-instructions.c61, "
               << "COP2-summary-of-copper-instructions.c67, COP2-summary-of-copper-instructions.c73, "
               << "COP2-summary-of-copper-instructions.c79, COP2-summary-of-copper-instructions.c85, ch2-about-the-copper.c7, ch2-appa-copins.c56, ch2-appa-copins.c62, ch2-appa-copins.c68, ch2-appa-copins.c74, ch2-appa-copins.c80, ch2-appa-copins.c86, ch2-appa-copins.c92, ch2-appa-copins.c98, ch2-summary-of-copper-instructions.c43, ch2-summary-of-copper-instructions.c49, ch2-summary-of-copper-instructions.c55, ch2-summary-of-copper-instructions.c61, ch2-summary-of-copper-instructions.c67, ch2-summary-of-copper-instructions.c73, ch2-summary-of-copper-instructions.c79, ch2-summary-of-copper-instructions.c85, ch2-the-move-instruction.c3, ch2-the-move-instruction.c5]";
        if (!all_wrong.empty()) {
            detail << " - MISROUTED:";
            for (size_t i = 0; i < all_wrong.size() && i < 8; ++i) {
                detail << " $" << std::hex << all_wrong[i].first
                       << "->$" << all_wrong[i].second << std::dec;
            }
            if (all_wrong.size() > 8) detail << " and " << (all_wrong.size() - 8) << " more";
        }
        checks.expect(all_wrong.empty(), detail.str());
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
