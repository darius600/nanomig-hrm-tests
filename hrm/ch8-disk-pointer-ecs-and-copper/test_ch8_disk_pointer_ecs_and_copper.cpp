// Test purpose: Grade the Appendix B DSKPTH OCS/ECS bit-width split and the
//   (E) write-protect mark, and p.279's Copper-initialisation claim, through
//   the real Agnus top-level bus -- the boundary where `ecs` is an actual,
//   drivable port and a Copper-issued write can be told apart from a
//   CPU-issued one. Neither is true at the narrower `agnus_diskdma`
//   boundary the sibling package `ch8-disk-dma-pointer` used.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 279, 288-289.
// Non-goals: This test does not re-grade the OCS "high 3 bits" or the "(E)"
//   readings already graded RED at `agnus_diskdma` by `ch8-disk-dma-pointer`
//   (candidates C7/C14). It does not build a Copper-write assertion at
//   ECS/cdang=1 for the protected register range -- that case is
//   `hrm/agnus_copper_move_range`'s existing red NM-HRM-004, and grading it
//   again here would silently fold it into a new number, which the build
//   assignment forbids. It does not cite `agnus_copper_move_range`'s T4/T6:
//   those exercise $07E under the printed name BLTSIZE, not a disk register.
//
// Machine-readable mutation records for tools/check_gates.py --tests. Every
// mutation below was run by temporarily editing this file, rebuilding,
// observing the printed result, and reverting before this commit; the
// baseline colour is named beside each result.
// MUTATION LIVENESS stimulus red baseline green: fed 0x0000 to DSKPTH instead of the 0x0015 the hardcoded expected constant was built from -> FAIL as expected. Reverted.
// MUTATION COPLIVE stimulus red baseline green: retargeted the MOVE from DMACON ($096) to DSKPTH ($020) under ecs=0/cdang=0 -> FAIL as expected (write no longer reaches, DMACONR bit 5 never sets). Reverted.
// MUTATION P1 stimulus green baseline red: substituted a CPU write of the same operand for the Copper MOVE, through the same disk-slot readback comparator -> PASS as expected (proves the comparator and the readback path work). Reverted.
// MUTATION P2 stimulus red baseline green: changed the written low-three-bit pattern from 0x5 to 0x2 while the hardcoded expected constant (0x5) stayed put -> FAIL as expected. Reverted.
// MUTATION P3 stimulus red baseline green: changed the written five-bit pattern from 0x15 to 0x0A while the hardcoded expected constant (0x15) stayed put -> FAIL as expected. Reverted.
// MUTATION P4 stimulus green baseline red: kept ecs fixed at 0 for both reads and varied the written data (0x0018 then 0x0007) instead -> PASS as expected (the same inequality comparator detects a real difference). Reverted.
// MUTATION P5 stimulus green baseline red: applied the same "does this input bit fail to reach this output position" comparator to DSKPTL bit 0 against address_out[0] (no such storage exists, address_out is [20:1]) instead of DSKPTH bits 4:3 -> PASS as expected. Reverted.

#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>

#include <verilated.h>
#include "Vagnus.h"

namespace {

constexpr uint16_t to_reg(uint16_t byte_addr) { return byte_addr >> 1; }

constexpr uint16_t DSKPTH  = to_reg(0x020);
constexpr uint16_t DSKPTL  = to_reg(0x022);
constexpr uint16_t DSKDATR = to_reg(0x008);
constexpr uint16_t COPCON  = to_reg(0x02E);
constexpr uint16_t DMACON  = to_reg(0x096);
constexpr uint16_t DMACONR = to_reg(0x002);
constexpr uint16_t COP1LCH = to_reg(0x080);
constexpr uint16_t COP1LCL = to_reg(0x082);
constexpr uint16_t COPJMP1 = to_reg(0x088);

// Fixture, built from the given classify evidence (agnus_diskdma's port
// list -- reg_address_in, data_in, address_out, dmal, dmas, dma, wr, hpos,
// speed; agnus's disk_dmal/disk_dmas/ecs/floppy_speed top-level ports) and
// from reading hrm/agnus_copper_control/test_agnus_top_copper_dma.cpp for
// how to bring the Agnus top level up (its DUT init list and CPU
// write/fetch-serving pattern are reused here almost verbatim; nothing is
// cited from it as an oracle).
class AgnusHarness {
public:
    AgnusHarness() : dut(new Vagnus) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->cck = 0;
        dut->reset = 1;
        dut->aen = 0;
        dut->rd = 0;
        dut->hwr = 0;
        dut->lwr = 0;
        dut->data_in = 0;
        dut->address_in = 0;
        dut->audio_dmal = 0;
        dut->audio_dmas = 0;
        dut->disk_dmal = 0;
        dut->disk_dmas = 0;
        dut->bls = 0;
        dut->ntsc = 0;
        dut->a1k = 0;
        dut->ecs = 0;
        dut->aga = 0;
        dut->floppy_speed = 0;

        run_cycles(24);
        dut->reset = 0;
        run_cycles(12);
    }

    ~AgnusHarness() {
        dut->final();
        delete dut;
    }

    AgnusHarness(const AgnusHarness&) = delete;
    AgnusHarness& operator=(const AgnusHarness&) = delete;

    void set_ecs(bool value) { dut->ecs = value ? 1 : 0; }

    void write_reg(uint16_t word_reg, uint16_t data) {
        for (int timeout = 0; timeout < 4000; ++timeout) {
            if (cycle(Op::Write, word_reg, data)) {
                cycle(Op::Idle, 0, 0);
                return;
            }
        }
        std::cerr << "FAIL: CPU write timed out for register 0x"
                  << std::hex << word_reg << std::dec << "\n";
    }

    uint16_t read_reg(uint16_t word_reg) {
        uint16_t value = 0;
        for (int timeout = 0; timeout < 4000; ++timeout) {
            if (cycle(Op::Read, word_reg, 0, &value)) {
                cycle(Op::Idle, 0, 0);
                return value;
            }
        }
        std::cerr << "FAIL: CPU read timed out for register 0x"
                  << std::hex << word_reg << std::dec << "\n";
        return 0;
    }

    // A single MOVE instruction followed by a terminating WAIT that can
    // never match, so a rejected MOVE leaves the Copper stalled rather than
    // running on into unrelated memory.
    void install_move(uint32_t word_address, uint16_t dest_reg, uint16_t value) {
        memory[word_address + 0] = static_cast<uint16_t>(dest_reg << 1);
        memory[word_address + 1] = value;
        memory[word_address + 2] = 0xFFFF;
        memory[word_address + 3] = 0xFFFE;
    }

    void program_cop1(uint32_t word_address) {
        write_reg(COP1LCH, static_cast<uint16_t>((word_address >> 15) & 0x1F));
        write_reg(COP1LCL, static_cast<uint16_t>((word_address & 0x7FFF) << 1));
        write_reg(COPJMP1, 0x0000);
        // Strobe before DMA is enabled, same order the HRM sequence and the
        // reference harness both use.
        run_cycles(12);
    }

    void set_dmacon(uint16_t value) { write_reg(DMACON, value); }
    uint16_t read_dmaconr() { return read_reg(DMACONR); }

    void run_cycles(int count) {
        for (int i = 0; i < count; ++i) {
            cycle(Op::Idle, 0, 0);
        }
    }

    // Runs the disk DMA channel until its internal pointer register is
    // muxed onto the shared address_out bus, and captures it there --
    // agnus_diskdma's own address_out only reaches this module's top-level
    // port during a granted disk-DMA cycle. Returns 0xFFFFFFFF if no such
    // cycle occurred inside the budget.
    uint32_t read_pointer_via_diskslot(int max_cycles = 8000) {
        dut->disk_dmal = 1;
        dut->disk_dmas = 0;
        uint32_t result = 0xFFFFFFFFu;
        for (int i = 0; i < max_cycles; ++i) {
            cycle(Op::Idle, 0, 0);
            if (dsk_capture_valid) {
                result = dsk_capture_value;
                break;
            }
        }
        dut->disk_dmal = 0;
        dsk_capture_valid = false;
        return result;
    }

private:
    enum class Op { Idle, Write, Read };

    Vagnus* dut;
    std::unordered_map<uint32_t, uint16_t> memory;
    bool dsk_capture_valid = false;
    uint32_t dsk_capture_value = 0;

    uint16_t read_memory(uint32_t word_address) const {
        const auto it = memory.find(word_address);
        return it == memory.end() ? 0xFFFF : it->second;
    }

    bool cycle(Op op, uint16_t reg, uint16_t data, uint16_t* read_out = nullptr) {
        dut->cck = !dut->cck;
        const bool is_write = (op == Op::Write);
        const bool is_read  = (op == Op::Read);
        dut->aen = (is_write || is_read) ? 1 : 0;
        dut->rd  = is_read ? 1 : 0;
        dut->hwr = is_write ? 1 : 0;
        dut->lwr = is_write ? 1 : 0;
        dut->address_in = (is_write || is_read) ? reg : 0;
        dut->data_in = is_write ? data : 0;
        dut->eval();

        const bool granted = (is_write || is_read)
                           && dut->cpu_custom
                           && dut->reg_address_out == reg;
        if (granted && is_read && read_out) {
            *read_out = static_cast<uint16_t>(dut->data_out);
        }

        // Serve any in-flight fetch (Copper instruction words, etc.) from
        // the simulated memory map.
        if (dut->dbr && !dut->dbwe) {
            dut->data_in = read_memory(dut->address_out);
            dut->eval();
        }

        dsk_capture_valid = false;
        if (dut->dbr && !dut->refresh && dut->reg_address_out == DSKDATR) {
            dsk_capture_valid = true;
            dsk_capture_value = dut->address_out;
        }

        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();

        if (is_write) {
            dut->aen = 0;
            dut->hwr = 0;
            dut->lwr = 0;
            dut->data_in = 0;
        } else if (is_read) {
            dut->aen = 0;
            dut->rd = 0;
        }

        return granted;
    }
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        if (condition) {
            std::cout << "PASS: " << label << "\n";
            ++pass_count;
        } else {
            std::cerr << "FAIL: " << label << "\n";
            ++fail_count;
        }
    }

    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }

    int passed() const { return pass_count; }
    int failed() const { return fail_count; }
    int result() const { return fail_count == 0 ? 0 : 1; }

private:
    int pass_count = 0;
    int fail_count = 0;
};

} // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // LIVENESS -- the fixture must be shown to deliver two distinct known
    // words to DSKPTH/DSKPTL and have both arrive, before anything above is
    // trusted. Same measurement basis (a register write, address_out then
    // inspected) as P2/P3/P5, so it is also this package's control on that
    // basis, alongside P2/P3.
    {
        AgnusHarness h;
        h.set_ecs(false);
        constexpr uint16_t kHighStim = 0x0015; // DSKPTH data_in written
        constexpr uint16_t kLowStim  = 0x2A2A; // DSKPTL data_in written
        h.write_reg(DSKPTH, kHighStim);
        h.write_reg(DSKPTL, kLowStim);
        const uint32_t got = h.read_pointer_via_diskslot();
        // Hand-derived from the two stimulus patterns above, kept as an
        // independent literal rather than computed from them.
        constexpr uint32_t kExpected = 0xA9515u;
        checks.expect(got == kExpected,
                      "LIVENESS the fixture delivers two distinct known words to DSKPTH and DSKPTL, and both arrive intact at address_out, before any claim is graded");
    }

    // COPLIVE -- P1 depends on a Copper MOVE being able to reach some
    // register through this harness at all. A MOVE to DMACON ($096, outside
    // $008-$026 at any cdang/ecs) is issued and its effect confirmed by a
    // CPU read of DMACONR, so a red P1 cannot be mistaken for a harness that
    // cannot make the Copper write anything.
    {
        AgnusHarness h;
        h.set_ecs(false);
        h.write_reg(COPCON, 0x0000); // cdang = 0
        constexpr uint32_t A = 0x03000;
        constexpr uint16_t kMoveData = 0x8020; // SET bit + SPREN (bit 5)
        h.install_move(A, DMACON, kMoveData);
        h.program_cop1(A);
        h.set_dmacon(0x8280); // SET + DMAEN + COPEN: let the Copper actually run
        h.run_cycles(4000);
        const uint16_t after = h.read_dmaconr();
        checks.expect((after & 0x0020) != 0,
                      "COPLIVE a Copper MOVE reaches an always-unprotected register (DMACON, outside $008-$026), confirmed by a CPU read of DMACONR");
    }

    // P1 -- sol:8-appa-dskptr.c3, p.279: the Copper may initialise DSKPTH
    // before disk DMA is enabled. Graded at ecs=0, per the assignment's hard
    // instruction to grade the printed write-protect mark that way.
    {
        AgnusHarness h;
        h.set_ecs(false);
        h.write_reg(COPCON, 0x0000); // cdang = 0

        constexpr uint16_t kSentinelHigh = 0x0007;
        constexpr uint16_t kSentinelLow  = 0x1234;
        h.write_reg(DSKPTH, kSentinelHigh);
        h.write_reg(DSKPTL, kSentinelLow);

        checks.record("DSKPTL ($022) sits inside the same $008-$026 range C13 "
            "measured as blocked at OCS under either cdang value, but a "
            "Copper program halts on its first illegal MOVE (does not "
            "advance to a next instruction) rather than continuing, so a "
            "second MOVE in the same list to $022 could never be reached to "
            "demonstrate anything once the first is blocked. Not "
            "independently asserted.");

        constexpr uint32_t A = 0x04000;
        constexpr uint16_t kMoveOperand = 0x0015; // distinct from the sentinel above
        h.install_move(A, DSKPTH, kMoveOperand);
        h.program_cop1(A);
        h.set_dmacon(0x8280);
        h.run_cycles(4000);

        const uint32_t after = h.read_pointer_via_diskslot();
        const uint32_t moved_target =
            (static_cast<uint32_t>(kMoveOperand & 0x1F) << 15) | (static_cast<uint32_t>(kSentinelLow) >> 1);
        checks.expect(after == moved_target,
                      "P1 the Copper may initialise DSKPTH before disk DMA is enabled [sol:8-appa-dskptr.c3, p.279]");
    }

    // P2 -- opus:8-appB-dskpth.c6, p.289: the high three pointer bits ARE
    // carried by DSKPTH (existence reading, not the "only three" reading).
    {
        AgnusHarness h;
        h.set_ecs(false);
        constexpr uint16_t kStimLow3 = 0x0005; // bits[2:0] = 101, bits[4:3] = 00
        h.write_reg(DSKPTH, kStimLow3);
        h.write_reg(DSKPTL, 0x0000);
        const uint32_t after = h.read_pointer_via_diskslot();
        constexpr uint32_t kExpectedLow3 = 0x5u; // independent of kStimLow3 above
        // address_out is declared [20:1]: Verilator packs it with C++ bit 0
        // == Verilog bit 1, so Verilog bits [18:16] sit at C++ bits [17:15].
        checks.expect(((after >> 15) & 0x7) == kExpectedLow3,
                      "P2 the high three pointer bits ARE carried by DSKPTH [opus:8-appB-dskpth.c6, p.289]");
    }

    // P3 -- opus:8-appB-dskpth.c7 + sol:8-appb-dskpth.c10, p.289: mutual
    // twins stating the identical proposition (under ECS, DSKPTH holds five
    // bits), one assertion citing both.
    {
        AgnusHarness h;
        h.set_ecs(true);
        constexpr uint16_t kStim5 = 0x0015; // bits[4:0] = 10101
        h.write_reg(DSKPTH, kStim5);
        h.write_reg(DSKPTL, 0x0000);
        const uint32_t after = h.read_pointer_via_diskslot();
        constexpr uint32_t kExpected5 = 0x15u; // independent of kStim5 above
        // Same [20:1]-packing note as P2: Verilog bits [20:16] sit at C++
        // bits [19:15].
        checks.expect(((after >> 15) & 0x1F) == kExpected5,
                      "P3 under ECS, DSKPTH holds five bits [opus:8-appB-dskpth.c7, sol:8-appb-dskpth.c10, p.289]");
    }

    // P4 -- sol:8-appb-dskpth.c8, p.289 composed with p.288: the (E) mark.
    // The same $020 write, presented once at a real ecs=0 and once at a
    // real ecs=1 driven at this boundary, is expected by the printed mark
    // to differ.
    {
        AgnusHarness h;
        constexpr uint16_t kStim = 0x0018; // bits[4:3] = 11, the "ECS-only" bits

        h.set_ecs(false);
        h.write_reg(DSKPTH, kStim);
        h.write_reg(DSKPTL, 0x0000);
        const uint32_t r_ocs = h.read_pointer_via_diskslot();

        h.set_ecs(true);
        h.write_reg(DSKPTH, kStim);
        h.write_reg(DSKPTL, 0x0000);
        const uint32_t r_ecs = h.read_pointer_via_diskslot();

        checks.expect(r_ocs != r_ecs,
                      "P4 the (E) mark: driving the real ecs port at the agnus boundary changes what DSKPTH loads [sol:8-appb-dskpth.c8, p.289 composed with p.288]");
    }

    // P5 -- sol:8-appb-dskpth.c9, p.289: on OCS, DSKPTH holds ONLY the high
    // three bits (the exclusivity reading, distinct from P2's existence
    // reading). Graded at ecs=0.
    {
        AgnusHarness h;
        h.set_ecs(false);
        constexpr uint16_t kStim = 0x001F; // all five bits set
        h.write_reg(DSKPTH, kStim);
        h.write_reg(DSKPTL, 0x0000);
        const uint32_t after = h.read_pointer_via_diskslot();
        // Verilog bits [20:19] (the two "ECS-only" bits) sit at C++ bits
        // [19:18], same [20:1]-packing note as P2/P3.
        checks.expect(((after >> 18) & 0x3) == 0,
                      "P5 on OCS, DSKPTH holds only the high three bits [sol:8-appb-dskpth.c9, p.289]");
    }

    checks.note("NM-HRM-004 check: this package never drives a Copper MOVE "
        "at ecs=1/cdang=1 against the $008-$026 range (the reach case C13 "
        "assigns to that existing finding). P4's ecs comparison above uses "
        "plain CPU writes, which are never gated by the Copper's "
        "write-protect logic at all, so it does not touch NM-HRM-004's "
        "territory either. Nothing in this package's build fell into that "
        "territory; there is nothing further to point at NM-HRM-004 beyond "
        "this record.");

    std::cout << checks.passed() << " passed, " << checks.failed() << " failed\n";
    return checks.result();
}
