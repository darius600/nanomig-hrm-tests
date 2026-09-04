// Test purpose: Prove PKG-SELFMOD - that the Copper can modify its own program
//   counter by writing to its own control registers (COP1LC/COP2LC/COPJMP1/
//   COPJMP2), as opposed to the already-refuted claim that it can rewrite its
//   own instruction words in RAM.
// Test class: HRM_CONFORMANCE (SM0 included: it re-measures an already-
//   uncontested fact - a subsequent externally-issued strobe reading back a
//   self-written location register - on the identical apparatus as SM1-SM4,
//   so a red SM1-SM4 result cannot be dismissed as "the harness is broken.")
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, Chapter 2,
//   print pages 26, 32 and 33. Frozen specification:
//   fpga-lab/work/2026-09-05_fpga-077_copper-selfmod_testspecification.md
// Requirement IDs: none of the frozen COP-01..COP-09 IDs owns this claim
//   group; see the specification's "Disputed prior finding" section, which
//   challenges docs/2026-08-21_hrm-coverage-matrix.md row COP-06.
// Non-goals: exact register-bus return latency (unspecified by module and
//   chapter alike); sustained/repeated looping beyond the few iterations
//   needed to demonstrate a loop closes; self-modification of instruction
//   words in RAM (confirmed not constructible - no data output port);
//   WAIT/SKIP/INTREQ semantics of the page-33 worked example; CDANG/illegal-
//   register interaction; the sof-triggered automatic COP1LC restart
//   (PKG-SOF-RESTART, separate package).
// Disclosure: agnus_copper.v was read beyond the port list (register-write
//   and strobe/jump logic, roughly lines 58-249 and 366-403) to determine
//   whether a register-bus loopback fixture was even buildable, and to
//   source the register addresses chapter 2 itself never gives numerically.
//   No expected value below came from that reading; every expected value is
//   a restatement of the quoted HRM text in the specification above. The
//   loopback technique itself (reg_address_out looped onto reg_address_in/
//   data_in) is not new: it is the same technique already used by
//   agnus_copper, agnus_copper_move, agnus_copper_skip, agnus_copper_wait
//   and agnus_copper_bus_behaviour, all of which depend on it for ordinary
//   instruction fetch (COPINS).

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
constexpr uint16_t COP2LCH = to_reg(0x084);
constexpr uint16_t COP2LCL = to_reg(0x086);
constexpr uint16_t COPJMP1 = to_reg(0x088);
constexpr uint16_t COPJMP2 = to_reg(0x08A);
constexpr uint16_t IDLE_REG = 0x0FF;

// Ordinary display registers, reused only as distinguishable markers. Chosen
// well clear of $080-$08A (the location/strobe registers under test) and
// already proven legal (not refused as illegal MOVE destinations) by the
// existing agnus_copper/-move/-bus_behaviour tests.
constexpr uint16_t MARK_SM0_TARGET = to_reg(0x180);
constexpr uint16_t MARK_SM1_TARGET = to_reg(0x182);
constexpr uint16_t MARK_SM2_TARGET = to_reg(0x184);
constexpr uint16_t MARK_SM3_LOOP   = to_reg(0x186);
constexpr uint16_t MARK_SM4_LOOP   = to_reg(0x188);

constexpr uint16_t WAIT_ALL = 0xFFFE;
constexpr uint16_t TERMINAL_IR1 = 0xFFFF;

constexpr uint16_t move_ir1(uint16_t reg) { return static_cast<uint16_t>(reg << 1); }

// Data-word halves for a location-register MOVE: cop*lch takes data_in[4:0],
// cop*lcl takes data_in[15:1] (agnus_copper.v lines 143-169). Word addresses
// here are in the same units as address_out - i.e. the units already used by
// every existing self-loop test (word index, not byte address).
constexpr uint16_t loc_hi(uint32_t word_addr) { return static_cast<uint16_t>((word_addr >> 15) & 0x1F); }
constexpr uint16_t loc_lo(uint32_t word_addr) { return static_cast<uint16_t>((word_addr & 0x7FFF) << 1); }

struct Grant {
    long tick;
    uint32_t address;
    uint16_t reg;
    uint16_t data;
};

// Register-bus loopback harness: identical technique to
// agnus_copper_bus_behaviour's CopperHarness. Whatever address the module
// itself presents on reg_address_out during an accepted DMA cycle is
// returned to it on reg_address_in, together with the word the memory model
// holds at the current address_out - which, during a MOVE's write cycle, is
// exactly the instruction's own data word. This is not a special case for
// this test: it is the same unconditional loop every existing Copper test
// already relies on for fetching COPINS.
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

    // External, CPU-style register write: drives reg_address_in/data_in
    // directly, never derived from the module's own reg_address_out. Used
    // (a) to bootstrap every scenario's entry point, and (b) in SM0/SM2 to
    // supply the half of the mechanism this package does not claim the
    // Copper performs itself.
    void write_reg(uint16_t reg, uint16_t data) { cycle(true, reg, data); }
    void preload_location(uint16_t hi_reg, uint16_t lo_reg, uint32_t word_addr) {
        write_reg(hi_reg, loc_hi(word_addr));
        write_reg(lo_reg, loc_lo(word_addr));
    }

    // Boot the Copper so its first fetch comes from `entry` (COP1LC +
    // COPJMP1, both written externally - ordinary test bootstrap, not part
    // of any PKG-SELFMOD claim itself).
    void start(uint32_t entry) {
        preload_location(COP1LCH, COP1LCL, entry);
        write_reg(COPJMP1, 0x0000);
        run_cycles(4);
        grants.clear();
        req_ticks.clear();
        tick = 0;
        dma_allowed = true;
    }
    void run_cycles(int n) { for (int i = 0; i < n; ++i) cycle(); }

    int count_grants_for_register(uint16_t reg) const {
        int n = 0;
        for (const auto& g : grants) if (g.reg == reg) ++n;
        return n;
    }

    std::vector<Grant> grants;
    std::vector<long> req_ticks;
    bool dma_allowed = true;

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
        if (dut->reqdma) req_ticks.push_back(tick);
        const bool accepted = !cpu_write && dma_allowed && dut->reqdma;
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
    int passed = 0, failed = 0;
};

constexpr uint32_t SRC = 0x00002000;
constexpr uint32_t DST = 0x00002100;

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ------------------------------------------------------------------
    // SM0 - control, expected PASS. Same loopback apparatus as SM1-SM4, but
    // only the "self-write the location register" half is self-issued; the
    // strobe that reads it back is issued externally (the already-covered,
    // already-passing PKG-STROBE mechanism). If this goes red, the fixture
    // itself is broken, not NanoMig; every other result in this file must
    // then be read in that light.
    // ------------------------------------------------------------------
    {
        CopperHarness h;
        h.install(SRC, {
            move_ir1(COP2LCH), loc_hi(DST),
            move_ir1(COP2LCL), loc_lo(DST),
            TERMINAL_IR1, WAIT_ALL   // deliberately no self-issued strobe here
        });
        h.install(DST, {
            move_ir1(MARK_SM0_TARGET), 0xC0DE,
            TERMINAL_IR1, WAIT_ALL
        });
        h.start(SRC);
        h.run_cycles(2000);   // let the two self-writes into COP2LC land, then halt at terminal WAIT
        h.write_reg(COPJMP2, 0x0000);  // externally-issued strobe, not looped from reg_address_out
        h.run_cycles(2000);

        checks.expect(h.count_grants_for_register(MARK_SM0_TARGET) >= 1,
            "SM0 [control] a self-written COP2LC value is correctly read back by a "
            "later externally-issued strobe (p.26 setup half; loopback fixture sanity check)");
    }

    // ------------------------------------------------------------------
    // SM1 - "The Copper can write its own location registers and strobe
    // addresses to perform programmed jumps" (p.26), worked as: MOVE a new
    // value into COP2LC, then MOVE COPJMP2 - both self-issued.
    // Covers: COP2-jump.c2, COP2-jump-strobe-address.c2/c3/c4,
    //         COP2-loops.c2, COP2-copper-loops-and-branches-and-comparison-enable.c2
    // ------------------------------------------------------------------
    {
        CopperHarness h;
        h.install(SRC, {
            move_ir1(COP2LCH), loc_hi(DST),
            move_ir1(COP2LCL), loc_lo(DST),
            move_ir1(COPJMP2), 0x0000,   // self-issued strobe, per the manual's own $0 data word
            move_ir1(MARK_SM0_TARGET), 0xDEAD,  // must not execute if the jump took effect
            TERMINAL_IR1, WAIT_ALL
        });
        h.install(DST, {
            move_ir1(MARK_SM1_TARGET), 0xBEEF,
            TERMINAL_IR1, WAIT_ALL
        });
        h.start(SRC);
        h.run_cycles(4000);

        checks.expect(h.count_grants_for_register(MARK_SM1_TARGET) >= 1,
            "SM1 [conformance] a self-issued MOVE into COP2LC followed by a "
            "self-issued MOVE to COPJMP2 reloads address_out with the "
            "self-written value (p.26)");
    }

    // ------------------------------------------------------------------
    // SM2 - "any MOVE instruction that addresses COPJMP2 strobes this
    // indirect address into the program counter" (p.26). COP2LC preloaded
    // externally (the claim does not require it be self-written); only the
    // COPJMP2 strobe itself is self-issued.
    // Covers: COP2-jump.c3
    // ------------------------------------------------------------------
    {
        CopperHarness h;
        h.install(SRC, {
            move_ir1(COPJMP2), 0x0000,   // self-issued strobe only
            TERMINAL_IR1, WAIT_ALL
        });
        h.install(DST, {
            move_ir1(MARK_SM2_TARGET), 0xF00D,
            TERMINAL_IR1, WAIT_ALL
        });
        h.start(SRC);
        h.preload_location(COP2LCH, COP2LCL, DST);   // external preload
        h.run_cycles(4000);

        checks.expect(h.count_grants_for_register(MARK_SM2_TARGET) >= 1,
            "SM2 [conformance] a self-issued MOVE addressing COPJMP2 strobes "
            "the externally-preloaded COP2LC address into the program "
            "counter (p.26)");
    }

    // ------------------------------------------------------------------
    // SM3 - "The loop is executed by writing to the COPJMP1 register. This
    // causes the Copper to jump to the address that was initialized in
    // COP1LC" (p.33). COP1LC is already the list's own head (ordinary
    // start() bootstrap); the list itself contains only a marker MOVE and a
    // self-issued MOVE to COPJMP1 - a genuine self-referencing loop.
    // Covers: COP2-loopex.c13/c14, COP2-a-copper-loop-example.c10/c11
    // ------------------------------------------------------------------
    {
        CopperHarness h;
        h.install(SRC, {
            move_ir1(MARK_SM3_LOOP), 0x1234,
            move_ir1(COPJMP1), 0x0000,   // self-issued strobe -> jump back to COP1LC (= SRC itself)
        });
        h.start(SRC);
        h.run_cycles(60000);

        checks.expect(h.count_grants_for_register(MARK_SM3_LOOP) >= 3,
            "SM3 [conformance] a self-issued MOVE to COPJMP1 repeatedly "
            "jumps to COP1LC, closing the loop (p.33 first worked loop)");
    }

    // ------------------------------------------------------------------
    // SM4 - "To cause the second loop, write to the COPJMP2 register"
    // (p.33), which the same page's listing comments "Force a jump to
    // COP2LC." Same self-referencing-loop construction as SM3.
    // Covers: COP2-loopex.c20, COP2-a-copper-loop-example.c19
    // ------------------------------------------------------------------
    {
        CopperHarness h;
        h.install(SRC, {
            move_ir1(MARK_SM4_LOOP), 0x5678,
            move_ir1(COPJMP2), 0x0000,   // self-issued strobe -> jump back to COP2LC (= SRC itself)
        });
        h.start(SRC);
        h.preload_location(COP2LCH, COP2LCL, SRC);   // COP2LC also points at the list's own head
        h.run_cycles(60000);

        checks.expect(h.count_grants_for_register(MARK_SM4_LOOP) >= 3,
            "SM4 [conformance] a self-issued MOVE to COPJMP2 repeatedly "
            "jumps to COP2LC, closing the second loop (p.33 second worked loop)");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
