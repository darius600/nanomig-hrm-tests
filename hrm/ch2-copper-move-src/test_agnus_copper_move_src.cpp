// Test purpose: Prove PKG-MOVE-SRC - that on the granted MOVE cycle,
//   address_out carries the address of the MOVE instruction's own SECOND
//   word (the first word's address plus one), while reg_address_out carries
//   the destination register. In all thirteen pre-existing Copper tests,
//   address_out is read only by the harness to fetch simulated memory
//   (`const uint32_t address = top->address_out;`); it is never asserted
//   against an expected value. This test adds that missing assertion.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, Chapter 2,
//   print pages 19 (p19:41-43), 20 (p20:26-27), 21 (p21:8-11, p21:26-27).
//   Frozen specification:
//   fpga-lab/work/2026-09-05_fpga-078_move-src_testspecification.md (private
//   lab workspace, not published; the claim ids below are the public record)
// Requirement IDs: COP-01 (About the Copper, p.19), COP-02 (What is a
//   Copper Instruction?, p.20), COP-03 (The MOVE Instruction, p.21). None of
//   the three rows' existing evidence covers this claim - their cited tests
//   read address_out only to fetch harness memory.
// Non-goals: destination-register legality/CDANG range gating (PKG-CDANG /
//   PKG-CDANG-REGR territory); the data VALUE carried by the second word
//   once decoded into the destination register (PKG-MOVE-DECODE); DMA
//   arbitration among channels for a contested slot (needs the Agnus top
//   level); exact inter-cycle timing (COP-02's cycle-time territory, already
//   covered and already carrying an unrelated valid red finding); rejected/
//   illegal-destination MOVE behaviour (closed elsewhere, FPGA-068);
//   WAIT/SKIP instructions mixed into the same list.
// Claim ids: ids such as `COP2-copins-fields-p275.c1` and
//   `COP2-copins-fields-p275.c2` name one sentence of the manual in this
//   chapter's reading. They carry no reader prefix, and they resolve in the
//   claim files that hrm-testpipeline/acceptance/ch2.json lists under
//   `inventories`.
//   `FPGA-nnn` is a task number in the private workspace where this work was
//   planned; it resolves nowhere public, and docs/legacy-names.md gives the
//   finding keys that replaced the ones that were findings.
//   The `COP-nn` Requirement IDs, COP-01 to COP-09, number the requirements of
//   a frozen specification (docs/legacy-names.md); no claim file carries them.
// Disclosure: agnus_copper.v was read beyond the port list (the copper
//   state machine and reg_address_out/address_out select logic, roughly
//   lines 192-220 and 384-467) to confirm that the granted MOVE cycle
//   (FETCH2 state, selreg=1, dma_ack=1) presents reg_address_out = the
//   destination register decoded from the first word while address_out
//   still holds the address used to fetch the CURRENT (second) instruction
//   word - i.e. that the classification's claimed "single granted cycle
//   carries both" is structurally what this RTL does, before writing the
//   fixture. No expected value below came from that reading: every expected
//   value is the direct arithmetic restatement "second word's address =
//   first word's address + 1" already given by the classification's own
//   evidence column for COP2-move.c2 / COP2-the-move-instruction.c2. The
//   register-bus loopback technique itself is the same one already used by
//   agnus_copper, agnus_copper_move, agnus_copper_skip, agnus_copper_wait,
//   agnus_copper_bus_behaviour and agnus_copper_selfmod.
// Mutation check: performed against the built, passing test (documented
//   here, not re-run on every future rebuild). Three separate one-line
//   mutations, each reverted immediately after observing red:
//     - MS1: `g.address == SRC + 1` changed to `g.address == SRC + 2` ->
//       MS1 turned FAIL (2 passed, 1 failed), MS2/MS3 unaffected.
//     - MS2: `gB.address == SRC + 3` changed to `gB.address == SRC + 1`
//       (same value as gA) -> MS2 turned FAIL, together with the
//       independent MS3 mutation below (1 passed, 2 failed that run).
//     - MS3: `g.address == FAR + 1` changed to `g.address == FAR` -> MS3
//       turned FAIL in the same run as the MS2 mutation above.
//   All three assertions discriminate; none is vacuously true. Reverted to
//   the values shown in this file, which reproducibly pass (3 passed, 0
//   failed).
//
//   Those three are ORACLE mutations: they change the expected value and so
//   show only that the comparison can go false. Each assertion is also
//   mutated at the STIMULUS - the scenario moves, the expected value
//   untouched - which is what asks whether the assertion binds to the
//   behaviour it names. Machine-readable records for tools/check_gates.py
//   --tests:
// MUTATION MS1 stimulus red a no-op MOVE prepended to the list, so the measured instruction sits at SRC+2 and its second word at SRC+3 while the oracle still names SRC+1
// MUTATION MS2 stimulus red the two MOVEs swapped, so each marker is fetched from the other's address and the advancing relationship no longer matches the marker it is checked against
// MUTATION MS3 stimulus red the same no-op prepend at the high address FAR, so the control case's second word moves to FAR+3 while its oracle still names FAR+1

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

// Ordinary display registers, reused only as distinguishable markers -
// well clear of $080-$08E (location/strobe/COPINS) and $02E (COPCON), and
// inside the always-writable $100-$1FE range already proven legal by every
// existing Copper test (CDANG=0, ECS=0, the state every test boots into).
constexpr uint16_t MARK_MS1  = to_reg(0x180);
constexpr uint16_t MARK_MS2A = to_reg(0x182);
constexpr uint16_t MARK_MS2B = to_reg(0x184);
constexpr uint16_t MARK_MS3  = to_reg(0x186);

constexpr uint16_t WAIT_ALL = 0xFFFE;
constexpr uint16_t TERMINAL_IR1 = 0xFFFF;

constexpr uint16_t move_ir1(uint16_t reg) { return static_cast<uint16_t>(reg << 1); }

struct Grant {
    long tick;
    uint32_t address;
    uint16_t reg;
    uint16_t data;
};

// Register-bus loopback harness, identical technique to every existing
// Copper test: whatever address/register the module presents during an
// accepted DMA cycle is returned to it on reg_address_in/data_in together
// with the memory model's word at that address.
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

    void install(uint32_t base, const std::vector<uint16_t>& words) {
        for (size_t i = 0; i < words.size(); ++i) memory[base + static_cast<uint32_t>(i)] = words[i];
    }

    void write_reg(uint16_t reg, uint16_t data) { cycle(true, reg, data); }

    // Boot the Copper so its first fetch comes from `entry` (COP1LC +
    // COPJMP1, both written externally - ordinary bootstrap, not a claim of
    // this package).
    void start(uint32_t entry) {
        write_reg(COP1LCH, static_cast<uint16_t>((entry >> 15) & 0x1F));
        write_reg(COP1LCL, static_cast<uint16_t>((entry & 0x7FFF) << 1));
        write_reg(COPJMP1, 0x0000);
        run_cycles(4);
        grants.clear();
        tick = 0;
    }
    void run_cycles(int n) { for (int i = 0; i < n; ++i) cycle(); }

    bool find_grant(uint16_t reg, Grant& out) const {
        for (const auto& g : grants) if (g.reg == reg) { out = g; return true; }
        return false;
    }

    std::vector<Grant> grants;

private:
    Vagnus_copper* dut;
    std::unordered_map<uint32_t, uint16_t> memory;
    uint16_t hpos = 0, vpos = 0;
    long tick = 0;

    uint16_t read_memory(uint32_t a) const {
        const auto it = memory.find(a);
        return it == memory.end() ? 0xFFFF : it->second;
    }

    void cycle(bool cpu_write = false, uint16_t cpu_reg = IDLE_REG, uint16_t cpu_data = 0) {
        dut->eval();
        const bool accepted = !cpu_write && dut->reqdma;
        if (cpu_write) {
            dut->ackdma = 0; dut->reg_address_in = cpu_reg; dut->data_in = cpu_data;
        } else if (accepted) {
            const uint32_t address = dut->address_out;
            const uint16_t reg = dut->reg_address_out;
            const uint16_t data = read_memory(address);
            dut->ackdma = 1; dut->reg_address_in = reg; dut->data_in = data;
            dut->eval();
            if (reg != IDLE_REG) grants.push_back({tick, address, reg, data});
        } else {
            dut->ackdma = 0; dut->reg_address_in = IDLE_REG; dut->data_in = 0;
        }
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();
        ++tick;
        if (++hpos == 454) { hpos = 0; vpos = static_cast<uint16_t>((vpos + 1) & 0x00FF); }
        dut->hpos = hpos; dut->vpos = vpos;
    }
};

class Checks {
public:
    void expect(bool c, const std::string& l) {
        std::cout << (c ? "PASS: " : "FAIL: ") << l << "\n";
        c ? ++passed : ++failed;
    }
    // Prints a record for a claim this file does NOT bind to any assertion
    // here. Never touches passed/failed - this is not an assertion.
    void note(const std::string& l) {
        std::cout << "NOTE: " << l << "\n";
    }
    int passed = 0, failed = 0;
};

constexpr uint32_t SRC = 0x00002000;
// Deliberately above word address $8000, so the second-word-address
// relationship is exercised where address_out's high bits are non-zero -
// the required control case, same measurement basis as MS1/MS2.
constexpr uint32_t FAR = 0x00018000;

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ------------------------------------------------------------------
    // MS1 - baseline case, all 9 PKG-MOVE-SRC rows. A single MOVE at a known
    // address: on the granted cycle, address_out must be that instruction's
    // own first-word address plus one (the second word), while
    // reg_address_out carries the destination.
    // ------------------------------------------------------------------
    {
        CopperHarness h;
        h.install(SRC, {
            move_ir1(MARK_MS1), 0xABCD,
            TERMINAL_IR1, WAIT_ALL
        });
        h.start(SRC);
        h.run_cycles(400);

        Grant g{};
        const bool found = h.find_grant(MARK_MS1, g);
        checks.expect(found && g.address == SRC + 1,
            "MS1 [conformance] on the granted MOVE cycle, address_out carries "
            "the instruction's second-word address (first word's address + "
            "1) while reg_address_out carries the destination register "
            "(p19:41-43, p20:26-27, p21:8-11) "
            "[claims: COP2-copins-fields-p275.c1, COP2-copins-fields-p275.c2, "
            "COP2-copins-fields-p275.c3, COP2-copins-fields-p275.c4, "
            "COP2-the-move-instruction.c2, COP2-what-is-a-copper-instruction.c8, "
            "COP2-what-is-a-copper-instruction.c14, COP2-what-is-a-copper-instruction.c10, ch2-what-is-a-copper-instruction.c8, ch2-appa-copins.c112, ch2-appa-copins.c113, ch2-appa-copins.c114, ch2-the-move-instruction.c1, ch2-the-move-instruction.c2]");
    }

    // ------------------------------------------------------------------
    // MS2 - the relationship must hold generically as the list advances,
    // not only for the very first instruction after a jump (which could
    // pass MS1 by an incidental reset-state coincidence). Two MOVEs back to
    // back, each targeting a distinct register.
    // ------------------------------------------------------------------
    {
        CopperHarness h;
        h.install(SRC, {
            move_ir1(MARK_MS2A), 0x1111,
            move_ir1(MARK_MS2B), 0x2222,
            TERMINAL_IR1, WAIT_ALL
        });
        h.start(SRC);
        h.run_cycles(400);

        Grant gA{}, gB{};
        const bool foundA = h.find_grant(MARK_MS2A, gA);
        const bool foundB = h.find_grant(MARK_MS2B, gB);
        checks.expect(foundA && gA.address == SRC + 1 && foundB && gB.address == SRC + 3,
            "MS2 [conformance] the second-word-address relationship holds "
            "for each instruction as the list advances (second MOVE's "
            "granted cycle shows its own first-word address + 1 = SRC+3, "
            "not SRC+1 again and not a fixed offset) "
            "[claims: COP2-the-move-instruction.c2, COP2-what-is-a-copper-instruction.c10]");
    }

    // ------------------------------------------------------------------
    // MS3 - control, same measurement basis, at an address whose word value
    // exceeds $8000 (non-zero high bits of address_out). Defends a red
    // result elsewhere in this package against "the rig truncates or wraps
    // addresses" - if this case fails while MS1/MS2 pass, the defect is
    // width/wraparound-specific, not a general fixture failure.
    // ------------------------------------------------------------------
    {
        CopperHarness h;
        h.install(FAR, {
            move_ir1(MARK_MS3), 0x3333,
            TERMINAL_IR1, WAIT_ALL
        });
        h.start(FAR);
        h.run_cycles(400);

        Grant g{};
        const bool found = h.find_grant(MARK_MS3, g);
        checks.expect(found && g.address == FAR + 1,
            "MS3 [control, same measurement basis] the same relationship "
            "holds at a base address above word $8000, ruling out "
            "truncation or wraparound in the measurement rig itself "
            "[claims: COP2-the-move-instruction.c2, COP2-what-is-a-copper-instruction.c10]");
    }

    // COP2-what-is-a-copper-instruction.c21 ("the Copper can directly affect
    // only machine registers") is UNBOUND. It is a claim about the ABSENCE of
    // a memory-write mechanism (no data-out port, no write strobe, in the
    // module's port list). There is nothing to drive or observe for a write
    // that cannot happen in the first place: MS1-MS3 above show address_out
    // and reg_address_out on the granted MOVE cycle, but a fixture can only
    // fail to find a memory-write port, it cannot behaviourally demonstrate
    // that one does not exist anywhere. Reason: observable only at a boundary
    // none of these fixtures drives.
    checks.note("UNBOUND COP2-what-is-a-copper-instruction.c21 -- observable only at a "
                 "boundary none of these fixtures drives (absence of a memory-write port; "
                 "not a behaviour any stimulus here could vary) [claims: ch2-what-is-a-copper-instruction.c19]");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
