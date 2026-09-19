// Machine-readable mutation records for tools/check_gates.py --tests. Only
// mutations actually performed are listed; assertions absent from this list
// have never been mutation-tested and the gate reports them as such.
// MUTATION SM4 stimulus red COP2LC pointed back at SRC, where COP1LC already points, restoring the original defect
// MUTATION SM6 stimulus red COP1LCH written with a wrong high half
// MUTATION SM0 stimulus red the externally-issued COPJMP2 strobe removed, so nothing ever reads the self-written COP2LC back
// MUTATION SM1 stimulus red the self-issued strobe retargeted from COPJMP2 to COPJMP1, so the jump follows COP1LC instead of the self-written COP2LC
// MUTATION SM2 stimulus red the external COP2LC preload removed entirely, so the self-issued COPJMP2 has no address of its own to jump to
// MUTATION SM2 stimulus red the self-issued strobe retargeted from COPJMP2 to COPJMP1 while COP2LC still holds DST, so reaching DST would have to come from the wrong register
// MUTATION SM3 stimulus red the loop's self-issued strobe changed from COPJMP1 to COPJMP2, which takes the road the assertion says is never taken
// MUTATION SM5 stimulus red the self-written low half pointed back at SRC instead of ALT, so the Copper never reaches the marked list
// MUTATION SM7 stimulus red the $FFFF-carrying MOVE retargeted from COPJMP2 to an ordinary register, so no strobe occurs at all
//
// One mutation SURVIVED and is recorded here because the survivor was the
// mutation, not the assertion. Moving SM2's preload from COP2LC to COP1LC
// (rather than removing it) left SM2 green: with DST in COP1LC there is a
// second road to DST that does not pass through COPJMP2 at all, so the
// scenario stopped discriminating. A mutation that installs the target address
// in the *other* location register cannot test a strobe; it has to remove the
// address or redirect the strobe. Both of those do turn SM2 red, above.
//
// Test purpose: Prove PKG-SELFMOD - that the Copper can modify its own
//   program counter by writing to its own control registers
//   (COP1LC/COP2LC/COPJMP1/ COPJMP2), as opposed to the already-refuted
//   claim that it can rewrite its own instruction words in RAM.
// Test class: HRM_CONFORMANCE (SM0 included as a control: it re-measures an
//   already-uncontested fact - a subsequent externally-issued strobe reading
//   back a self-written location register - on the same apparatus as the
//   others. Two limits on how far that carries, both established in
//   independent validation on 05.09.2026: - SM0 exercises the loopback for
//   *location register* writes only. A rig fault specific to the *strobe*
//   path leaves it green. - A red SM0 does NOT mean "the harness is broken
//   rather than NanoMig". It also goes red if a self-issued MOVE to COP2LCL
//   never reaches the register - a module fault. SM0 narrows where to look;
//   it does not decide between rig and module, and an earlier revision of
//   this header claimed that it did.)
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, Chapter 2,
//   print pages 26, 32 and 33. Frozen specification:
//   fpga-lab/work/2026-09-05_fpga-077_copper-selfmod_testspecification.md
//   (private lab workspace, not published; the claim ids below are the
//   public record)
// Requirement IDs: none of the frozen COP-01..COP-09 IDs owns this claim
//   group; see the specification's "Disputed prior finding" section, which
//   challenges docs/2026-08-21_hrm-coverage-matrix.md row COP-06.
// Why the loopback is legitimate: it is not a fixture invention. In
//   src/minimig-aga/agnus.v:235-239 the Copper's own register address is
//   selected onto the shared bus when its cycle is acknowledged
//   (ack_cop -> reg_address = reg_address_cop), and line 422 feeds that same
//   reg_address back into this module as reg_address_in. The harness
//   reproduces the wiring of the surrounding chip, which is why the module
//   cannot distinguish its own writes from the CPU's - and why this package
//   is measurable at all.
// Non-goals: exact register-bus return latency (unspecified by module and
//   chapter alike); the p.32 requirement that COP1LC be repointed before the
//   next vertical blanking (COP2-loops.c3 - a constraint on the programmer,
//   not on this module); self-modification of instruction words in RAM
//   (confirmed not constructible - no data output port); WAIT/SKIP/INTREQ
//   semantics of the page-33 worked example; CDANG/illegal- register
//   interaction; the sof-triggered automatic COP1LC restart
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
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the printed pages independently, and an id names
//   one reader's reading of one sentence. They resolve in the chapter's
//   claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.

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
constexpr uint16_t MARK_SM5_LOOP   = to_reg(0x18A);
constexpr uint16_t MARK_SM6_LOOP   = to_reg(0x18E);
constexpr uint16_t MARK_SM7_TARGET = to_reg(0x190);
// Placed only on the list a *wrong* strobe would reach. It must never be
// granted; if it is, the strobe read the other location register.
constexpr uint16_t MARK_WRONG_PATH = to_reg(0x18C);

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
    // Prints a record for a claim this file does NOT bind to any assertion
    // here. Never touches passed/failed - this is not an assertion.
    void note(const std::string& l) { std::cout << "NOTE: " << l << "\n"; }
    int passed = 0, failed = 0;
};

constexpr uint32_t SRC = 0x00002000;
constexpr uint32_t DST = 0x00002100;
// A third list head, used where a scenario must show that the *self-written*
// pointer value is the one taken - it is never written externally.
constexpr uint32_t ALT = 0x00002200;
// A fourth, deliberately above word address $8000 so that loc_hi() is NON-zero.
// SRC/DST/ALT all sit low enough that their high halves are 0, which means a
// COP1LCH/COP2LCH that did nothing at all would still pass every scenario built
// on them. Independent validation, 05.09.2026.
constexpr uint32_t FAR = 0x00018000;

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
            "self-written value (p.26) "
            "[claims: COP2-copjmp-strobes.c7, COP2-copjmp-strobes.c8, "
            "COP2-jump-strobe-address.c2, COP2-jump-strobe-address.c3, COP2-jump-strobe-address.c4, ch2-jump-address-strobes.c2, ch2-jump-address-strobes.c3]");
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
            "counter (p.26) "
            "[claims: COP2-copjmp-strobes.c7, COP2-copjmp-strobes.c8, COP2-jump-strobe-address.c3, ch2-jump-address-strobes.c4]");
    }

    // ------------------------------------------------------------------
    // SM3 - "The loop is executed by writing to the COPJMP1 register. This
    // causes the Copper to jump to the address that was initialized in
    // COP1LC" (p.33). COP1LC is already the list's own head (ordinary
    // start() bootstrap); the list itself contains only a marker MOVE and a
    // self-issued MOVE to COPJMP1 - a genuine self-referencing loop.
    //
    // COP2LC is deliberately pointed at a *different* list carrying
    // MARK_WRONG_PATH. Without that, COP2LC would sit at its reset value and
    // the assertion would discriminate only by accident: a COPJMP1 that
    // wrongly read COP2LC could not be told apart from one that read COP1LC.
    // Covers: COP2-loopex.c13/c14, COP2-a-copper-loop-example.c10/c11
    // ------------------------------------------------------------------
    {
        CopperHarness h;
        h.install(SRC, {
            move_ir1(MARK_SM3_LOOP), 0x1234,
            move_ir1(COPJMP1), 0x0000,   // self-issued strobe -> jump back to COP1LC (= SRC itself)
        });
        h.install(DST, {
            move_ir1(MARK_WRONG_PATH), 0xBAD1,
            TERMINAL_IR1, WAIT_ALL
        });
        h.start(SRC);
        h.preload_location(COP2LCH, COP2LCL, DST);   // the road not to be taken
        h.run_cycles(60000);

        checks.expect(h.count_grants_for_register(MARK_SM3_LOOP) >= 3
                      && h.count_grants_for_register(MARK_WRONG_PATH) == 0,
            "SM3 [conformance] a self-issued MOVE to COPJMP1 repeatedly "
            "jumps to COP1LC and never to the distinct COP2LC list "
            "(p.33 first worked loop) "
            "[claims: COP2-copjmp-strobes.c5, COP2-copjmp-strobes.c6, "
            "COP2-a-copper-loop-example.c10, COP2-a-copper-loop-example.c11, ch2-a-copper-loop-example.c10, ch2-a-copper-loop-example.c11, ch2-appa-copjmp.c10]");
    }

    // ------------------------------------------------------------------
    // SM4 - "To cause the second loop, write to the COPJMP2 register"
    // (p.33), which the same page's listing comments "Force a jump to
    // COP2LC."
    //
    // The pointers must differ, and an earlier revision of this test got that
    // wrong: it set COP2LC to SRC, where COP1LC already pointed, so the
    // assertion could not tell "COPJMP2 loaded COP2LC" from "COPJMP2 loaded
    // COP1LC" - precisely the claim it carries. Found in independent
    // validation, 05.09.2026. Now COP1LC = SRC and COP2LC = DST, and the
    // looping marker lives only on the DST list: if the strobe took COP1LC,
    // control would loop at SRC and MARK_SM4_LOOP would never be granted.
    // Covers: COP2-loopex.c20, COP2-a-copper-loop-example.c19
    // ------------------------------------------------------------------
    {
        CopperHarness h;
        h.install(SRC, {
            move_ir1(MARK_WRONG_PATH), 0xBAD2,  // granted once on entry, never again
            move_ir1(COPJMP2), 0x0000,          // self-issued strobe -> COP2LC (= DST)
        });
        h.install(DST, {
            move_ir1(MARK_SM4_LOOP), 0x5678,
            move_ir1(COPJMP2), 0x0000,          // self-loop, again via COP2LC
        });
        h.start(SRC);                                // COP1LC = SRC
        h.preload_location(COP2LCH, COP2LCL, DST);   // COP2LC = DST, deliberately distinct
        h.run_cycles(60000);

        checks.expect(h.count_grants_for_register(MARK_SM4_LOOP) >= 3
                      && h.count_grants_for_register(MARK_WRONG_PATH) <= 1,
            "SM4 [conformance] a self-issued MOVE to COPJMP2 repeatedly "
            "jumps to COP2LC, which points elsewhere than COP1LC "
            "(p.33 second worked loop) "
            "[claims: COP2-a-copper-loop-example.c19, ch2-a-copper-loop-example.c16]");
    }

    // ------------------------------------------------------------------
    // SM5 - the two claims that ask for both halves at once: the Copper
    // *changes* a location register itself, and the loop then runs on the
    // value it wrote. SM1 changes a pointer but jumps once; SM3 and SM4 loop
    // on pointers that were preloaded from outside. Neither covers this.
    //
    // COP2LC is never written externally here. The list at SRC writes it to
    // ALT and strobes; the list at ALT re-strobes COPJMP2 and so loops on the
    // self-written value.
    // Covers: COP2-loops.c2, COP2-copper-loops-and-branches-and-comparison-enable.c2
    // ------------------------------------------------------------------
    {
        CopperHarness h;
        h.install(SRC, {
            move_ir1(COP2LCH), loc_hi(ALT),   // self-written pointer, high
            move_ir1(COP2LCL), loc_lo(ALT),   // self-written pointer, low
            move_ir1(COPJMP2), 0x0000,        // jump to what was just written
        });
        h.install(ALT, {
            move_ir1(MARK_SM5_LOOP), 0x9ABC,
            move_ir1(COPJMP2), 0x0000,        // loop on the self-written value
        });
        h.start(SRC);
        h.run_cycles(60000);

        checks.expect(h.count_grants_for_register(MARK_SM5_LOOP) >= 3,
            "SM5 [conformance] the Copper writes its own COP2LC and then loops "
            "on that self-written address, with no external write to COP2LC at "
            "any point (p.26 with p.33) "
            "[claims: COP2-copper-loops-and-branches-and-comparison-enable.c2, COP2-jump-strobe-address.c2]");
    }

    // ------------------------------------------------------------------
    // SM6 - the same as SM5 but through COP1LC, and to an address whose HIGH
    // half is non-zero. Two blind spots at once, both found in independent
    // validation on 05.09.2026:
    //   - every scenario above self-writes COP2LC only, while the file header
    //     claims the Copper writes "its own control registers
    //     (COP1LC/COP2LC/COPJMP1/COPJMP2)". COP1LC was never self-written.
    //   - SRC/DST/ALT are all below word address $8000, so loc_hi() is 0 for
    //     all of them. A COP1LCH/COP2LCH write that did nothing whatsoever
    //     would have passed every assertion in this file.
    // FAR is above $8000, so its high half carries actual bits.
    // ------------------------------------------------------------------
    {
        CopperHarness h;
        h.install(SRC, {
            move_ir1(COP1LCH), loc_hi(FAR),   // self-written, and NON-zero
            move_ir1(COP1LCL), loc_lo(FAR),
            move_ir1(COPJMP1), 0x0000,
        });
        h.install(FAR, {
            move_ir1(MARK_SM6_LOOP), 0x0F1E,
            move_ir1(COPJMP1), 0x0000,        // loop on the self-written COP1LC
        });
        h.start(SRC);
        h.run_cycles(60000);

        checks.expect(h.count_grants_for_register(MARK_SM6_LOOP) >= 3,
            "SM6 [conformance] the Copper writes its own COP1LC - including a "
            "non-zero high half - and then loops on that self-written address "
            "(p.26 with p.33) "
            "[claims: COP2-jump-strobe-address.c2]");
    }

    // ------------------------------------------------------------------
    // SM7 - COP2-jump.c3 says "*any* MOVE instruction that addresses COPJMP2
    // strobes this indirect address into the program counter". Every strobe
    // above carries the data word $0000, copied from the manual's own
    // listing, so a module that strobed only on a zero data word would have
    // passed all of them. The strobe is address-triggered; the data word is
    // irrelevant, and that is what "any MOVE" asserts.
    // Covers the "any" in: COP2-jump.c3
    // ------------------------------------------------------------------
    {
        CopperHarness h;
        h.install(SRC, {
            move_ir1(COPJMP2), 0xFFFF,   // deliberately NOT $0000
            TERMINAL_IR1, WAIT_ALL
        });
        h.install(DST, {
            move_ir1(MARK_SM7_TARGET), 0x2468,
            TERMINAL_IR1, WAIT_ALL
        });
        h.start(SRC);
        h.preload_location(COP2LCH, COP2LCL, DST);
        h.run_cycles(4000);

        checks.expect(h.count_grants_for_register(MARK_SM7_TARGET) >= 1,
            "SM7 [conformance] a self-issued MOVE to COPJMP2 strobes "
            "regardless of its data word - $FFFF strobes as $0000 does "
            "(p.26, \"any MOVE\") - the same address-triggered, "
            "data-independent effect appendix B's own legend names: "
            "\"S Strobe (write address with no register bits). Writing "
            "the register causes the effect.\" (p.287) - COPJMP2 IS such "
            "a strobe register, demonstrated here directly: an arbitrary, "
            "non-zero data word ($FFFF) causes the identical effect a "
            "zero data word causes, because no register bits are ever "
            "read from the write at all "
            "[claims: COP2-copjmp-strobes.c2, COP2-jump-strobe-address.c3, opus:7-appB.c10, "
            "sol:8-appb-legend.c19]");
    }

    // ------------------------------------------------------------------
    // Appendix claims considered for this package and left UNBOUND, with
    // reasons. None of these change any assertion above; they are a printed
    // record only.
    // ------------------------------------------------------------------
    checks.note("UNBOUND (COP2-copjmp-strobes.c1: \"COPJMP1 is a strobe address\" - "
        "SM7 above tests data-word independence specifically for COPJMP2; no "
        "equivalent scenario here varies COPJMP1's own strobe data word, so the same "
        "data-independence fact is not separately established for COPJMP1)");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
