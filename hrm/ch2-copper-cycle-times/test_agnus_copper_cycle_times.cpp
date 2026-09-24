// Test purpose: Assert how many memory cycle *times* each Copper instruction
//   form occupies, and that the Copper takes only every other memory cycle.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, print
//   pages 19, 20, 23 and 194; frozen specification 7177fce.
// Requirement IDs: COP-01, COP-02
// Non-goals: Granted accesses per instruction (FPGA-063 covers those).
//   Contended timing when a higher-priority channel takes the slot
//   (FPGA-066). Absolute slot parity, whose numbering origin the HRM does
//   not fix at this module boundary. NTSC long/short lines. A blocked WAIT,
//   whose span is set by the beam rather than by the instruction.
// Claim ids: ids such as `COP2-about-the-copper.c20` and
//   `COP2-copins-timing.c1` name one sentence of the manual in this chapter's
//   reading. They carry no reader prefix, and they resolve in the claim files
//   that hrm-testpipeline/acceptance/ch2.json lists under `inventories`.
//   `FPGA-nnn` is a task number in the private workspace where this work was
//   planned; it resolves nowhere public, and docs/legacy-names.md gives the
//   finding keys that replaced the ones that were findings.
//   A `frozen specification` named above was the suite's first oracle, a note
//   written from the Verilog before the pipeline existed; docs/legacy-names.md
//   describes it.
//   The `COP-nn` Requirement IDs, COP-01 to COP-09, number the requirements of
//   the frozen specification the suite's first version was written against
//   (docs/legacy-names.md); no claim file carries them.
//
// Disclosure required by docs/model-collaboration-guide.md: the RTL was already
// inspected during FPGA-063 and FPGA-065 before this specification was written.
// Every expected figure below is a verbatim printed-page quotation, and the unit
// definition is derived from the HRM (p.194, p.23) rather than from the RTL.
//
// MUTATION P1 stimulus red: run_cycles after start() cut from 600 to 10 (too short to observe five
//   consecutive grant pairs) -> FAIL as expected. Per the mutation campaign's file scope, only P1 was
//   mutation-tested here; P2-P6 are out of scope for this pass.
//
// AC1-AC7 added 06.09.2026 (FPGA-084, package PKG-APPX-TIMING). Machine-readable
// records for tools/check_gates.py --tests:
// MUTATION AC1 stimulus red a WAIT placed before the measured MOVE, so the window from word 0 to word 2 spans the WAIT instead
// MUTATION AC2 stimulus red the same WAIT placement; the span measured is then the WAIT's, not a MOVE's
// MUTATION AC3 stimulus red the same WAIT placement; the ratio measured is then the WAIT's
// MUTATION AC7 stimulus red the run cut from 600 cycles to 6, too short to observe three grants, so the control correctly refuses to vouch for the fixture
//
// AC4 CANNOT BE STIMULUS-MUTATED AT THIS BOUNDARY, and that is worth stating
// rather than hiding. It asserts that a SKIP is granted two bus cycles. No
// stimulus produces a different grant count, because this module grants every
// instruction form exactly two -- replacing the SKIP with a MOVE or a WAIT
// leaves AC4 green. The assertion still discriminates against a real
// alternative behaviour (a module granting three), but that behaviour cannot
// be induced from outside; only an RTL change would show it. The playbook's
// mutation rule assumes a stimulus mutation always exists. Here one does not.
//
// MUTATION AC5 stimulus green the SKIP in the window replaced by a plain MOVE -> AC5 and AC6 turn GREEN
// MUTATION AC6 stimulus green the same replacement -> the ratio measures 2, not 4
// MUTATION P2 stimulus red: the first instruction of P2's own list changed from
//   a MOVE (move_ir1(COLOR00), 0x0444) to a WAIT already satisfied on arrival
//   (ir1_pos(0x10, 0x00), ir2_mask(true, 0x7F, 0x00, false)) with the beam at
//   0x70, so the span from word 0 to word 2 is a WAIT's and not a MOVE's ->
//   6 passed, 7 failed; P2 is the only assertion that moves. Reverted: 7
//   passed, 6 failed, the baseline exactly.
// MUTATION AC4 stimulus red: the SKIP in AC4's own list replaced by a WAIT for
//   a line the beam has not reached (ir1_pos(0xF0, 0x00) with skip cleared, the
//   beam at 0x70), so the Copper blocks instead of stepping over the skipped
//   MOVE and fewer grants fall inside word offsets 0..2 -> 6 passed, 7 failed;
//   AC4 is the only assertion that moves. Reverted: 7 passed, 6 failed.
// MUTATION P3 none impossible: P3 is RED at baseline and is one of NM-HRM-005's
//   assertions - the manual gives a SKIP four memory cycle times and the module
//   takes eight. A stimulus mutation has to take a green assertion red, and
//   there is nothing green here to break. Recorded rather than left out:
//   an unrecorded assertion and a forgotten one look the same.
// MUTATION P4 none impossible: same, the SKIP-not-taken half of NM-HRM-005.
// MUTATION P5 none impossible: same, the satisfied-WAIT half of NM-HRM-005 - six
//   memory cycle times in the manual, eight measured.
// MUTATION P6 none impossible: same. P6 asserts the DIFFERENCE between a WAIT
//   and a MOVE and is red for the same reason; with P5 red there is no green
//   baseline for it either.
//
// Those last two are POSITIVE CONTROLS, and they are what makes AC5/AC6's red
// mean something. With a SKIP in the measurement window the module grants two
// bus cycles and spans eight memory cycle times, ratio 4. With a MOVE in the
// same window, on the same fixture and the same counters, it spans four, ratio
// 2. The instrument is therefore not miscounting and the units are not
// confused: the cost belongs to the SKIP.

#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <verilated.h>
#include "Vagnus_copper.h"

namespace {

constexpr uint16_t to_reg(uint16_t byte_address) { return static_cast<uint16_t>(byte_address >> 1); }

constexpr uint16_t COP1LCH = to_reg(0x080);
constexpr uint16_t COP1LCL = to_reg(0x082);
constexpr uint16_t COPJMP1 = to_reg(0x088);
constexpr uint16_t IDLE_REG = 0x0FF;
constexpr uint16_t COLOR00 = to_reg(0x180);
constexpr uint16_t COLOR01 = to_reg(0x182);
constexpr uint16_t COLOR02 = to_reg(0x184);

constexpr uint32_t LIST = 0x00002000;

// HRM p.23: "In PAL, they are all 227 long." The fixture wraps hpos at 454, so
// one memory cycle time (= one color clock, p.194) is two hpos steps.
constexpr int PAL_LINE_COLOR_CLOCKS = 227;
constexpr int HPOS_WRAP = 2 * PAL_LINE_COLOR_CLOCKS;
constexpr int TICKS_PER_MEMORY_CYCLE_TIME = HPOS_WRAP / PAL_LINE_COLOR_CLOCKS;

constexpr uint16_t ir1_pos(uint8_t vp, uint8_t hp) {
    return static_cast<uint16_t>((vp << 8) | ((hp & 0x7F) << 1) | 1);
}
constexpr uint16_t ir2_mask(bool bfd, uint8_t ve, uint8_t he, bool skip) {
    return static_cast<uint16_t>((bfd ? 0x8000 : 0) | ((ve & 0x7F) << 8) | ((he & 0x7F) << 1) | (skip ? 1 : 0));
}
constexpr uint16_t move_ir1(uint16_t reg) { return static_cast<uint16_t>(reg << 1); }

constexpr uint16_t WAIT_ALL = 0xFFFE;
constexpr uint16_t TERMINAL_IR1 = 0xFFFF;

struct Grant {
    long tick;
    uint32_t word_offset;
    uint16_t reg;
};

class CopperHarness {
public:
    CopperHarness() : dut(new Vagnus_copper) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1; dut->ecs = 0;
        dut->enadma = 1; dut->ackdma = 0; dut->sof = 0; dut->blit_busy = 0;
        dut->hpos = 0; dut->vpos = 0; dut->data_in = 0; dut->reg_address_in = IDLE_REG;
        for (int i = 0; i < 12; ++i) cycle();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) cycle();
    }
    ~CopperHarness() { dut->final(); delete dut; }
    CopperHarness(const CopperHarness&) = delete;
    CopperHarness& operator=(const CopperHarness&) = delete;

    void install(const std::vector<uint16_t>& words) {
        for (size_t i = 0; i < words.size(); ++i) memory[LIST + i] = words[i];
    }
    void seek_beam(uint16_t v) { vpos = v; hpos = 0; dut->vpos = vpos; dut->hpos = hpos; }

    void start() {
        write_reg(COP1LCH, static_cast<uint16_t>((LIST >> 15) & 0x1F));
        write_reg(COP1LCL, static_cast<uint16_t>((LIST & 0x7FFF) << 1));
        write_reg(COPJMP1, 0x0000);
        run_cycles(4);
        grants.clear();          // ignore the start handshake itself
        tick = 0;
        dma_allowed = true;
    }
    void run_cycles(int n) { for (int i = 0; i < n; ++i) cycle(); }

    // Tick of the first granted cycle that fetched the given word offset.
    long first_tick_at(uint32_t word_offset) const {
        for (const auto& g : grants) if (g.word_offset == word_offset) return g.tick;
        return -1;
    }

    // Span in memory cycle times from an instruction's own first word to the
    // first word of the instruction that follows it. This is the quantity
    // HRM p.20 calls "memory cycle times ... required per instruction".
    int span_in_memory_cycle_times(uint32_t own_first_word, uint32_t next_first_word) const {
        const long a = first_tick_at(own_first_word);
        const long b = first_tick_at(next_first_word);
        if (a < 0 || b < 0) return -1;
        return static_cast<int>((b - a) / TICKS_PER_MEMORY_CYCLE_TIME);
    }

    // Granted bus cycles belonging to one instruction: the grants that fall
    // between its own first word and the first word of the next instruction.
    //
    // The other unit. Appendix A p.275 names both and relates them: "All
    // instructions (except for WAIT) require two bus cycles (and two
    // instruction words).  Since only the odd bus cycles are requested, four
    // memory cycle times are required per instruction". Counting them in the
    // same run is what makes the ratio assertable -- two numbers from two
    // fixtures would not share a time base.
    int granted_bus_cycles(uint32_t own_first_word, uint32_t next_first_word) const {
        const long a = first_tick_at(own_first_word);
        const long b = first_tick_at(next_first_word);
        if (a < 0 || b < 0) return -1;
        int n = 0;
        for (const auto& g : grants) if (g.tick >= a && g.tick < b) ++n;
        return n;
    }

    std::vector<Grant> grants;
    bool dma_allowed = false;

private:
    Vagnus_copper* dut;
    std::unordered_map<uint32_t, uint16_t> memory;
    uint16_t hpos = 0, vpos = 0;
    long tick = 0;

    uint16_t read_memory(uint32_t a) const {
        const auto it = memory.find(a);
        return it == memory.end() ? 0xFFFF : it->second;
    }
    void write_reg(uint16_t r, uint16_t d) { cycle(true, r, d); }

    void cycle(bool cpu_write = false, uint16_t cpu_reg = IDLE_REG, uint16_t cpu_data = 0) {
        dut->eval();
        const bool accepted = !cpu_write && dma_allowed && dut->reqdma;
        if (cpu_write) {
            dut->ackdma = 0; dut->reg_address_in = cpu_reg; dut->data_in = cpu_data;
        } else if (accepted) {
            const uint32_t address = dut->address_out;
            const uint16_t reg = dut->reg_address_out;
            dut->ackdma = 1; dut->reg_address_in = reg; dut->data_in = read_memory(address);
            dut->eval();
            if (reg != IDLE_REG) grants.push_back({tick, address - LIST, reg});
        } else {
            dut->ackdma = 0; dut->reg_address_in = IDLE_REG; dut->data_in = 0;
        }
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();
        ++tick;
        if (++hpos == HPOS_WRAP) { hpos = 0; vpos = static_cast<uint16_t>((vpos + 1) & 0x00FF); }
        dut->hpos = hpos; dut->vpos = vpos;
    }
};

class Checks {
public:
    void expect(bool c, const std::string& l) {
        std::cout << (c ? "PASS: " : "FAIL: ") << l << "\n";
        c ? ++passed : ++failed;
    }
    void expect_eq(int actual, int expected, const std::string& l) {
        const bool ok = actual == expected;
        std::cout << (ok ? "PASS: " : "FAIL: ") << l
                  << "  [expected " << expected << ", observed " << actual << "]\n";
        ok ? ++passed : ++failed;
    }
    // Records an unbound-claim reason without checking anything: does not
    // touch passed/failed, so it cannot change the suite's pass/fail counts.
    void note(const std::string& l) { std::cout << "NOTE: " << l << "\n"; }
    int passed = 0, failed = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // P1 - p.19/p.20: the Copper takes only every other memory cycle, which is
    // why two memory cycles cost four memory cycle times. Asserted as spacing
    // rather than absolute parity: the numbering origin is a property of the
    // surrounding beam counter, which the HRM does not fix at this boundary.
    {
        CopperHarness h;
        h.seek_beam(0x20);
        h.install({move_ir1(COLOR00), 0x0111,
                   move_ir1(COLOR01), 0x0222,
                   move_ir1(COLOR02), 0x0333,
                   TERMINAL_IR1, WAIT_ALL});
        h.start();
        h.run_cycles(600);

        int pairs = 0, wrong = 0;
        long prev = -1;
        for (const auto& g : h.grants) {
            if (g.word_offset > 5) break;          // stay inside the MOVE run
            if (prev >= 0) {
                ++pairs;
                if ((g.tick - prev) / TICKS_PER_MEMORY_CYCLE_TIME != 2) ++wrong;
            }
            prev = g.tick;
        }
        checks.expect(pairs >= 5 && wrong == 0,
                      "P1 consecutive Copper bus grants are two memory cycle times apart (p.19, p.20) "
                      "[claims: COP2-about-the-copper.c20, COP2-copins-timing.c1, ch2-about-the-copper.c19, ch2-appa-copins.c126]");

        // COP2-about-the-copper.c21 and COP2-copins-timing.c2 ("the Copper
        // requests the bus only during
        // odd-numbered memory cycles") is UNBOUND. P1 above tests the same
        // sentence's SPACING (every other cycle), by this test's own design
        // and comment ("Asserted as spacing rather than absolute parity: the
        // numbering origin is a property of the surrounding beam counter,
        // which the HRM does not fix at this boundary."). c21 requires the
        // parity itself (why "odd" rather than "even"), which is a labelling
        // convention this fixture explicitly declines to pin down - no
        // stimulus here could distinguish "odd" from "even" without also
        // fixing an origin the HRM does not give. Reason: no assertion
        // measures it.
        checks.note("UNBOUND COP2-about-the-copper.c21 -- no assertion measures it "
                     "(odd/even is a numbering-origin convention this fixture does not fix, see P1's own comment) [claims: ch2-about-the-copper.c20, ch2-appa-copins.c127, ch2-appa-copins.c129, ch2-what-is-a-copper-instruction.c14]");
    }

    // P2 - p.20: "The MOVE and SKIP instructions require two memory cycles ...
    // four memory cycle times are required per instruction."
    int move_span = -1;
    {
        CopperHarness h;
        h.seek_beam(0x20);
        h.install({move_ir1(COLOR00), 0x0444,
                   move_ir1(COLOR01), 0x0555,
                   TERMINAL_IR1, WAIT_ALL});
        h.start();
        h.run_cycles(600);
        move_span = h.span_in_memory_cycle_times(0, 2);
        checks.expect_eq(move_span, 4, "P2 a MOVE spans four memory cycle times (p.20) "
                         "[claims: COP2-what-is-a-copper-instruction.c16, COP2-copins-timing.c4, "
                         "COP2-copins-timing.c5]");
    }

    // P3 - p.20, same sentence: SKIP is stated at four as well. Condition true,
    // so the following MOVE is skipped.
    {
        CopperHarness h;
        h.seek_beam(0x70);
        h.install({ir1_pos(0x10, 0x00), ir2_mask(true, 0x7F, 0x00, true),
                   move_ir1(COLOR00), 0x0666,      // skipped
                   move_ir1(COLOR01), 0x0777,
                   TERMINAL_IR1, WAIT_ALL});
        h.start();
        h.run_cycles(4000);
        checks.expect_eq(h.span_in_memory_cycle_times(0, 2), 4,
                         "P3 a SKIP whose condition is true spans four memory cycle times (p.20) [NM-HRM-005, expected red] "
                         "[claims: COP2-what-is-a-copper-instruction.c17]");
    }

    // P4 - the same claim with the condition false, so the following MOVE runs.
    {
        CopperHarness h;
        h.seek_beam(0x10);
        h.install({ir1_pos(0x70, 0x00), ir2_mask(true, 0x7F, 0x00, true),
                   move_ir1(COLOR00), 0x0888,
                   TERMINAL_IR1, WAIT_ALL});
        h.start();
        h.run_cycles(4000);
        checks.expect_eq(h.span_in_memory_cycle_times(0, 2), 4,
                         "P4 a SKIP whose condition is false spans four memory cycle times (p.20) [NM-HRM-005, expected red] "
                         "[claims: COP2-what-is-a-copper-instruction.c17]");
    }

    // P5/P6 - p.20: "The WAIT instruction requires three memory cycles and six
    // memory cycle times." Measured on a WAIT that is already satisfied when it
    // is decoded: a blocked WAIT's span is set by when the beam arrives, which
    // the page does not describe. See the specification for the recorded
    // tension with FPGA-063 over which scenario carries this figure.
    {
        CopperHarness h;
        h.seek_beam(0x70);
        h.install({ir1_pos(0x10, 0x00), ir2_mask(true, 0x7F, 0x00, false),   // satisfied on arrival
                   move_ir1(COLOR00), 0x0999,
                   TERMINAL_IR1, WAIT_ALL});
        h.start();
        h.run_cycles(4000);
        const int wait_span = h.span_in_memory_cycle_times(0, 2);
        checks.expect_eq(wait_span, 6,
                         "P5 a satisfied WAIT spans six memory cycle times (p.20) [NM-HRM-005, expected red] "
                         "[claims: COP2-what-is-a-copper-instruction.c19, ch2-what-is-a-copper-instruction.c17]");
        checks.expect_eq(wait_span - move_span, 2,
                         "P6 a WAIT spans two memory cycle times more than a MOVE (p.20: three cycles versus two) [NM-HRM-005, expected red] "
                         "[claims: COP2-what-is-a-copper-instruction.c20]");
    }

    // ------------------------------------------------------------------
    // AC1-AC7 - appendix A p.275, the CONVERSION between the two units.
    // Specification: work/2026-09-06_fpga-084_appendix-cycle-conversion_
    // testspecification.md. Package PKG-APPX-TIMING.
    //
    //   "All instructions (except for WAIT) require two bus cycles (and two
    //    instruction words).  Since only the odd bus cycles are requested,
    //    four memory cycle times are required per instruction (memory cycles
    //    are 280 ns.)"
    //
    // P2-P4 above assert the memory-cycle-time counts on their own. These state
    // the sentence's other half: the two units stand in a fixed ratio of two,
    // because only every other slot is asked for.
    //
    // WHAT THESE DO NOT DO, corrected 06.09.2026 after independent review.
    // An earlier version of this comment claimed "a module can satisfy both
    // individual counts and still break the relation between them". **That is
    // mathematically false.** If AC4 holds that a SKIP is granted two bus
    // cycles and AC5 holds that it spans four memory cycle times, the ratio is
    // two and AC6 cannot fail. The ratio assertions are EXPOSITORY - they state
    // the manual's own relation in the manual's terms - and they are not an
    // independent check. AC5 also repeats what P3 above already measures on the
    // same fixture.
    //
    // The finding is therefore AC5/P3's, not AC6's: a SKIP spans eight memory
    // cycle times where the appendix and page 20 both say four. AC6's value is
    // that it says so in the unit relation the appendix uses.
    //
    // FPGA-064 has been blocked since 21.08.2026 on "first establish what
    // 'memory cycle' means here". This paragraph establishes it: a bus cycle is
    // a granted slot, a memory cycle time is 280 ns of elapsed time, and one
    // costs two of the other. That is what the appendix contributes; the count
    // itself was already measured.
    // ------------------------------------------------------------------
    {
        CopperHarness h;
        h.seek_beam(0x20);
        h.install({move_ir1(COLOR00), 0x0444,
                   move_ir1(COLOR01), 0x0555,
                   TERMINAL_IR1, WAIT_ALL});
        h.start();
        h.run_cycles(600);

        const int bus = h.granted_bus_cycles(0, 2);
        const int span = h.span_in_memory_cycle_times(0, 2);

        // AC7 first: the control. If grants are not two memory cycle times
        // apart on this very fixture, the two counters do not share a time base
        // and neither ratio below means anything. Without it a red AC6 cannot
        // survive the objection that the instruments simply disagree.
        bool spacing_ok = h.grants.size() >= 3;
        for (size_t i = 1; i < h.grants.size() && spacing_ok; ++i) {
            const long d = h.grants[i].tick - h.grants[i - 1].tick;
            spacing_ok = (d / TICKS_PER_MEMORY_CYCLE_TIME) == 2;
        }
        checks.expect(spacing_ok,
            "AC7 [control, MOVE fixture only] on a list of two MOVEs and a terminal WAIT, "
            "consecutive grants are two memory cycle times apart - the two calculations "
            "are calibrated against each other here (p.275 \"only the odd bus cycles are "
            "requested\"). They read the same `grants` vector, so this is agreement "
            "between two readings of one record, not between two instruments ");

        checks.expect_eq(bus, 2,
            "AC1 a MOVE is granted two bus cycles (p.275 \"All instructions (except for "
            "WAIT) require two bus cycles\") "
            "[claims: COP2-copins-instructions.c5, ch2-appa-copins.c135]");
        checks.expect_eq(span, 4,
            "AC2 a MOVE spans four memory cycle times (p.275 \"four memory cycle times are "
            "required per instruction\") "
            "[claims: COP2-copins-timing.c4, COP2-copins-timing.c5]");
        // Multiply rather than divide. `span / bus == 2` is integer division and
        // would accept span=5, bus=2 - which is not "exactly twice" and not what
        // the sentence says. Caught by independent review on 06.09.2026.
        checks.expect_eq(span, 2 * bus,
            "AC3 for a MOVE, memory cycle times are exactly twice the granted bus cycles "
            "(p.275 \"Since only the odd bus cycles are requested\")");
    }

    {
        // The SKIP scenario of P3, measured with both instruments at once.
        CopperHarness h;
        h.seek_beam(0x70);
        h.install({ir1_pos(0x10, 0x00), ir2_mask(true, 0x7F, 0x00, true),
                   move_ir1(COLOR00), 0x0666,      // skipped
                   move_ir1(COLOR01), 0x0777,
                   TERMINAL_IR1, WAIT_ALL});
        h.start();
        h.run_cycles(4000);

        const int bus = h.granted_bus_cycles(0, 2);
        const int span = h.span_in_memory_cycle_times(0, 2);

        // Diagnostic, not an assertion, and deliberately not given an
        // assertion-style name - a previous revision called it "AC8", which
        // independent review rightly objected to.
        //
        // AC7 runs only on the MOVE fixture and cannot vouch for this one. It
        // cannot, and the reason is the finding: here the grants are NOT evenly
        // spaced. What follows measures the gap ACROSS THE SKIP specifically -
        // from its second word to the next instruction's first - rather than
        // the largest gap anywhere in the stream, which an earlier revision
        // printed and which could have come from somewhere else entirely.
        //
        // The MOVE comparison value is measured in the MOVE block above and
        // carried here, rather than written into the text as a constant.
        const long skip_second_word = h.first_tick_at(1);
        const long next_instruction = h.first_tick_at(2);
        const long idle_across_skip =
            (skip_second_word >= 0 && next_instruction > skip_second_word)
                ? (next_instruction - skip_second_word) / TICKS_PER_MEMORY_CYCLE_TIME
                : -1;
        std::cout << "[measured] from the SKIP's second word to the next instruction: "
                  << idle_across_skip << " memory cycle times; the MOVE fixture above "
                  << "measured " << move_span << " for a whole instruction\n";

        checks.expect_eq(bus, 2,
            "AC4 a SKIP is granted two bus cycles (p.275, SKIP is not the sentence's "
            "exception) "
            "[claims: COP2-copins-instructions.c6, COP2-copins-instructions.c8, ch2-appa-copins.c135]");
        checks.expect_eq(span, 4,
            "AC5 a SKIP spans four memory cycle times (p.275, same sentence)");
        checks.expect_eq(span, 2 * bus,
            "AC6 for a SKIP, memory cycle times are exactly twice the granted bus cycles "
            "(p.275). EXPOSITORY, not independent: given AC4 and AC5 this cannot fail on "
            "its own - it states the manual's relation in the manual's own units");
    }

    // ------------------------------------------------------------------
    // Appendix claims considered for this package and left UNBOUND, with
    // reasons. None of these change any assertion above; they are a printed
    // record only.
    // ------------------------------------------------------------------
    checks.note("UNBOUND (COP2-copins-timing.c3: the claim's own restatement flags it "
        "as a cycle-COUNT claim distinct from elapsed-cycle-TIME. Every assertion in "
        "this file - P1/P2/AC1-AC7 - measures elapsed time via tick differences "
        "converted through TICKS_PER_MEMORY_CYCLE_TIME, not an independent count; no "
        "instrument here tests a 'memory cycles' notion separate from that time base) [claims: ch2-appa-copins.c128]");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
