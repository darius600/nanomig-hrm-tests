// Test purpose: pp.431,435-436 Appendix K "Base Address Registers" / "Shut
//   Up Register" / the prose that separates the Zorro II and Zorro III
//   configuration blocks.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 431, 435
// Non-goals: anything outside the `cpu_wrapper` fixture this package builds,
//   and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/appK-build/appK-autoconfig-base-address.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// appK-autoconfig-base-address -- pp.431,435-436 Appendix K "Base Address
// Registers" / "Shut Up Register" / the prose that separates the Zorro II
// and Zorro III configuration blocks. 23 claims, filtered from
// hrm-testpipeline/work/appK-build/appK-autoconfig-base-address.jsonl.
//
// See SPECIFICATION.md for the full boundary investigation and the
// candidate finding this package exists to grade. In short:
//
//   - THE FINDING: page 436 says writing register 48, in the Zorro II
//     configuration block, "actually configures the board for both
//     Zorro II and Zorro III boards." cpu_wrapper.v:629-665's write-
//     completion logic is `if (~cfg_z3) { if (addr==0x48) ... } else if
//     (addr==0x44) ...` -- when cfg_z3=1 (Zorro III), a write to register
//     48 matches NEITHER branch. Register 48 does nothing at all while
//     configured for Zorro III in this design. Graded as the page states
//     it (sol:K-0844, opus:K-0881) -- RED, and it stays red.
//   - Register 4C (shut-up) has no write handler anywhere in this design
//     (cpu_wrapper.v:629-665, read in full, recognizes only 6'b100100 and
//     6'b100010). The Zorro II card this design's own default
//     configuration presents advertises register08 bit6=0 ("can be shut
//     up", hrm/appK-autoconfig-register-map's own sol:K-0785(p433),
//     graded PASS there). Writing register 4C to a board that advertises
//     shut-up support should silence it, per the page -- it does not.
//     Graded RED (sol:K-0740/K-0849/K-0850/K-0848, opus:K-0889/K-0888).
//   - 11 claims whose entire content is the SPECIFIC bit range a write-
//     only, no-readback register latches (nybble vs byte width, A31-A24
//     vs A31-A28, etc.) are declined: no boundary on this module can
//     observe what was actually stored (same reasoning
//     hrm/appK-autoconfig-size-encoding already used to decline the
//     "aligned on natural boundaries" claim). See SPECIFICATION.md.
//   - 4 claims about the registers' IDENTITY/ROLE (write-only, and that
//     writing completes configuration in the register's own primary,
//     described context) remain gradeable and are graded PASS.
//
// MUTATION NOTE: 8 of this package's 23 graded assertions are expected to
// fail in the BASELINE (that is the finding). The build brief's MUTATION
// template ("stimulus change that turns it red") presumes a passing
// baseline; for these 8, the guarded mutation instead shows the INVERSE --
// a stimulus under which the same mechanism genuinely succeeds -- proving
// the check is a real, discriminating measurement and not a label that
// would print FAIL unconditionally. See SPECIFICATION.md, "A note on the
// MUTATION requirement for already-red assertions." Graded expected
// values are never touched by this.

// MUTATION sol:K-0844/opus:K-0881 baseline RED (the finding). GREENCHECK_REG48
//   stimulus GREEN (see GREENCHECK_REG48 block): the identical check,
//   built against Zorro II instead of Zorro III, reads green -- register
//   48 DOES complete configuration there (cpu_wrapper.v:640-642), proving
//   this is a real per-cfg_z3 divergence, not a stuck assertion.
// MUTATION sol:K-0844 stimulus green: built against Zorro II instead of
//   Zorro III (`make -B MUT=GREENCHECK_REG48 test`: "FAIL: sol:K-0844" at
//   baseline becomes "PASS: sol:K-0844", 7 passed, 6 failed).
// MUTATION opus:K-0881 stimulus green: same switch as sol:K-0844
//   (`make -B MUT=GREENCHECK_REG48 test`: "FAIL: opus:K-0881" at baseline
//   becomes "PASS: opus:K-0881", 7 passed, 6 failed).
// MUTATION sol:K-0740/K-0849/K-0850/opus:K-0889 baseline RED. GREENCHECK_4C
//   stimulus GREEN (see GREENCHECK_4C block): the identical "does writing
//   X silence the board" check, targeting register 48 instead of register
//   4C, reads green -- proving the check mechanism itself is sound.
// MUTATION sol:K-0740 stimulus green: targets register 48 instead of 4C
//   (`make -B MUT=GREENCHECK_4C test`: "FAIL: sol:K-0740" at baseline
//   becomes "PASS: sol:K-0740", 11 passed, 2 failed).
// MUTATION sol:K-0849 stimulus green: same switch as sol:K-0740
//   (`make -B MUT=GREENCHECK_4C test`: "FAIL: sol:K-0849" at baseline
//   becomes "PASS: sol:K-0849", 11 passed, 2 failed).
// MUTATION sol:K-0850 stimulus green: same switch as sol:K-0740
//   (`make -B MUT=GREENCHECK_4C test`: "FAIL: sol:K-0850" at baseline
//   becomes "PASS: sol:K-0850", 11 passed, 2 failed).
// MUTATION opus:K-0889 stimulus green: same switch as sol:K-0740
//   (`make -B MUT=GREENCHECK_4C test`: "FAIL: opus:K-0889" at baseline
//   becomes "PASS: opus:K-0889", 11 passed, 2 failed).
// MUTATION sol:K-0848/opus:K-0888 baseline RED. GREENCHECK_4C stimulus
//   GREEN: same swap as above (register 4C has literally no code path;
//   register 48 does).
// MUTATION sol:K-0848 stimulus green: same switch as sol:K-0740
//   (`make -B MUT=GREENCHECK_4C test`: "FAIL: sol:K-0848" at baseline
//   becomes "PASS: sol:K-0848", 11 passed, 2 failed).
// MUTATION opus:K-0888 stimulus green: same switch as sol:K-0740
//   (`make -B MUT=GREENCHECK_4C test`: "FAIL: opus:K-0888" at baseline
//   becomes "PASS: opus:K-0888", 11 passed, 2 failed).
// MUTATION sol:K-0834/opus:K-0866 stimulus red: MUT=WRONGREG (see WRONGREG
//   block below): tested against Zorro II instead of Zorro III, writing
//   register 44 does nothing (only register 48 is recognized when
//   cfg_z3=0) -- the "completes configuration in its own context"
//   assertion goes red.
// MUTATION sol:K-0834 stimulus red: tested against Zorro II instead of
//   Zorro III (`make -B MUT=WRONGREG test`: "PASS: sol:K-0834" at baseline
//   becomes "FAIL: sol:K-0834", 1 passed, 12 failed).
// MUTATION opus:K-0866 stimulus red: same switch as sol:K-0834
//   (`make -B MUT=WRONGREG test`: "PASS: opus:K-0866" at baseline becomes
//   "FAIL: opus:K-0866", 1 passed, 12 failed).
// MUTATION sol:K-0835/opus:K-0867 stimulus red: MUT=WRONGREG tested against
//   Zorro III instead of Zorro II, writing register 48 does nothing (this
//   IS the finding's own mechanism) -- goes red.
// MUTATION sol:K-0835 stimulus red: tested against Zorro III instead of
//   Zorro II (`make -B MUT=WRONGREG test`: "PASS: sol:K-0835" at baseline
//   becomes "FAIL: sol:K-0835", 1 passed, 12 failed).
// MUTATION opus:K-0867 stimulus red: same switch as sol:K-0835
//   (`make -B MUT=WRONGREG test`: "PASS: opus:K-0867" at baseline becomes
//   "FAIL: opus:K-0867", 1 passed, 12 failed).

#include "Vcpu_wrapper.h"
#include "verilated.h"
#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <vector>

static int passed = 0, failed = 0;

void expect(bool cond, const std::string& label) {
    if (cond) { std::cout << "PASS: " << label << "\n"; passed++; }
    else      { std::cout << "FAIL: " << label << "\n"; failed++; }
}
void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }

std::string hex1(uint8_t v) {
    static const char* d = "0123456789ABCDEF";
    return std::string(1, d[v & 0xF]);
}

static uint32_t autocfg_addr(int idx) { return 0xE80000u + 2u * (uint32_t)idx; }

// Same general-purpose Harness as hrm/appK-autoconfig-space's own.
struct Op {
    bool is_write;
    uint32_t addr;
    uint16_t imm;
};
Op READ(uint32_t addr) { return Op{false, addr, 0}; }
Op WRITE_IMM(uint32_t addr, uint16_t imm) { return Op{true, addr, imm}; }

struct Harness {
    Vcpu_wrapper* top;
    std::map<uint32_t, uint16_t> mem;
    std::vector<uint32_t> read_scratch;
    std::vector<uint16_t> word;
    bool all_captured = false;

    void tick() { top->clk = 0; top->eval(); top->clk = 1; top->eval(); }

    Harness(uint8_t cpucfg_v, uint32_t fastramcfg_v, const std::vector<Op>& ops) {
        top = new Vcpu_wrapper;
        mem[0x000000] = 0x0001; mem[0x000002] = 0x0000;
        mem[0x000004] = 0x0000; mem[0x000006] = 0x1000;

        uint32_t pc = 0x1000;
        int nreads = 0;
        for (auto& op : ops) {
            if (!op.is_write) {
                uint32_t s = 0x002000 + 2u * (uint32_t)nreads++;
                read_scratch.push_back(s);
                mem[pc] = 0x3039; mem[pc+2] = (uint16_t)(op.addr>>16); mem[pc+4] = (uint16_t)(op.addr&0xFFFF);
                pc += 6;
                mem[pc] = 0x33C0; mem[pc+2] = (uint16_t)(s>>16); mem[pc+4] = (uint16_t)(s&0xFFFF);
                pc += 6;
            } else {
                mem[pc] = 0x33FC; mem[pc+2] = op.imm;
                mem[pc+4] = (uint16_t)(op.addr>>16); mem[pc+6] = (uint16_t)(op.addr&0xFFFF);
                pc += 8;
            }
        }
        mem[pc] = 0x60FE;

        top->reset = 0;
        top->clk = 0; top->ph1 = 0; top->ph2 = 0;
        top->cpucfg = cpucfg_v;
        top->fastramcfg = fastramcfg_v;
        top->chipramcfg = 0;
        top->slowramcfg = 0;
        top->turbocfg = 0;
        top->bootrom = 0;
        top->chip_dout = 0;
        top->chip_dtack = 0;
        top->chip_ipl = 7;
        top->ramdout = 0;
        top->ramready = 0;
        top->eval();

        std::map<uint32_t, uint16_t> captured;
        bool last_as = true;
        const int RESET_CYCLES = 20;
        const int MAX_CYC = 200000;
        for (int cyc = 0; cyc < MAX_CYC && captured.size() < read_scratch.size(); cyc++) {
            if (cyc == RESET_CYCLES) top->reset = 1;

            uint32_t ba = ((uint32_t)top->chip_addr) << 1;
            auto it = mem.find(ba & ~1u);
            top->chip_dout = (it != mem.end()) ? it->second : 0;
            top->eval();

            int phase = cyc % 4;
            top->ph1 = (phase == 3);
            top->ph2 = (phase == 1);
            tick();

            if (cyc >= RESET_CYCLES) {
                bool as_now = top->chip_as;
                if (last_as && !as_now && !top->chip_rw) {
                    uint32_t a = ((uint32_t)top->chip_addr) << 1;
                    for (auto s : read_scratch)
                        if (s == a && !captured.count(a)) { captured[a] = top->chip_din; break; }
                }
                last_as = as_now;
            }
        }
        all_captured = (captured.size() == read_scratch.size());
        for (auto s : read_scratch)
            word.push_back(captured.count(s) ? captured[s] : 0);
    }
    ~Harness() { top->final(); delete top; }

    uint8_t nibble(size_t readIndex) const { return (uint8_t)((word[readIndex] >> 12) & 0xF); }
};

static uint8_t logical(int idx, uint8_t rawnib) {
    return (idx <= 1) ? rawnib : (uint8_t)((~rawnib) & 0xF);
}
static bool bit(uint8_t nibble, int n) { return ((nibble >> n) & 1) != 0; }

int main() {
    const int IDX_REG00 = 0, IDX_REG08 = 4, IDX_REG44 = 34, IDX_REG48 = 36, IDX_REG4C = 38;

    // =========================================================================
    // Group Finding -- page 436: "writing to register 48 actually
    // configures the board for both Zorro II and Zorro III boards in the
    // Zorro II configuration block." Zorro III harness, single write to
    // register 48 (fastramcfg=4: cfg_z3=1, fastramcfg_d[0]=0 so a
    // register-44 write WOULD complete in one step -- register 48 is
    // written instead, to grade exactly the page's claim).
    // =========================================================================
#ifdef MUT_GREENCHECK_REG48
    // MUTATION sol:K-0844/opus:K-0881 baseline RED; GREENCHECK_REG48
    // stimulus GREEN: Zorro II instead of Zorro III -- register 48 DOES
    // complete configuration there.
    Harness finding(0, 1, {READ(autocfg_addr(IDX_REG00)),
                            WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),
                            READ(autocfg_addr(IDX_REG00))});
#else
    Harness finding(2, 4, {READ(autocfg_addr(IDX_REG00)),
                            WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),
                            READ(autocfg_addr(IDX_REG00))});
#endif
    uint8_t finding_before = finding.nibble(0);
    uint8_t finding_after = finding.nibble(1);
    note("Group Finding: reg00 raw before=0x" + hex1(finding_before) +
         " after writing register 48=0x" + hex1(finding_after) +
         " -- page says this should now be silent (0x0, configured); "
         "nonzero means it is still responding (NOT configured).");
    bool reg48_completes_ziii_per_page = finding.all_captured && (finding_after == 0x0);
    expect(reg48_completes_ziii_per_page,
           "sol:K-0844 in the Zorro II configuration block, writing register 48 completes configuration for a Zorro II OR Zorro III board");
    expect(reg48_completes_ziii_per_page,
           "opus:K-0881 writing to register 48 configures the board for both Zorro II and Zorro III boards in the Zorro II configuration block");

    // =========================================================================
    // Group Shutup -- page 436: any write to register 4C shuts up a
    // board that advertises support (register 08 bit6=0). Zorro II
    // harness (this design's own default configuration; register 08
    // bit6=0 there, already established independently by
    // hrm/appK-autoconfig-register-map's sol:K-0785(p433)).
    // =========================================================================
    Harness shutup_check(0, 1, {READ(autocfg_addr(IDX_REG08))});
    uint8_t reg08_raw = shutup_check.nibble(0);
    uint8_t reg08_logical = logical(IDX_REG08, reg08_raw);
    bool bit6_clear_can_be_shut_up = !bit(reg08_logical, 2); // bit6 of byte = bit2 of nibble
    note("Group Shutup precondition: register08 logical nibble=0x" + hex1(reg08_logical) +
         " bit6=" + std::to_string(bit(reg08_logical,2)) + " (0 = 'can be shut up', matching "
         "hrm/appK-autoconfig-register-map's independently-graded sol:K-0785(p433)).");

#ifdef MUT_GREENCHECK_4C
    // MUTATION sol:K-0740/K-0849/K-0850/opus:K-0889/K-0848/opus:K-0888
    // baseline RED; GREENCHECK_4C stimulus GREEN: target register 48
    // (which DOES silence the board) instead of register 4C.
    Harness shutup(0, 1, {READ(autocfg_addr(IDX_REG00)),
                           WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),
                           READ(autocfg_addr(IDX_REG00))});
#else
    Harness shutup(0, 1, {READ(autocfg_addr(IDX_REG00)),
                           WRITE_IMM(autocfg_addr(IDX_REG4C), 0x0000),
                           READ(autocfg_addr(IDX_REG00))});
#endif
    uint8_t shutup_before = shutup.nibble(0);
    uint8_t shutup_after = shutup.nibble(1);
    note("Group Shutup: reg00 raw before=0x" + hex1(shutup_before) +
         " after writing register 4C=0x" + hex1(shutup_after) +
         " -- page says a bit6=0 board must now be silent (0x0); nonzero "
         "means it is still responding (NOT shut up).");
    bool shutup_silences_board = shutup.all_captured && bit6_clear_can_be_shut_up && (shutup_after == 0x0);
    bool reg4c_is_write_only = true; // established generically: 0x4C never appears in
                                      // either read case list (cpu_wrapper.v:565-608,
                                      // read in full), same as every other unmapped index.

    expect(shutup_silences_board, "sol:K-0740 any write to the shut-up register (4C) shuts up a supported (bit6=0) board");
    expect(shutup_silences_board, "sol:K-0849 any write to logical register 4C requests board shut-up, and the board complies");
    expect(shutup_silences_board, "sol:K-0850 a shut-up-capable board ceases responding until reset after a write to register 4C");
    expect(shutup_silences_board, "opus:K-0889 anything written to register 4C causes a board that supports shut-up to completely disappear until the next reset");
    expect(reg4c_is_write_only && shutup_silences_board,
           "sol:K-0848 logical register 4C is the write-only shut-up register (functions as one)");
    expect(reg4c_is_write_only && shutup_silences_board,
           "opus:K-0888 register 4C, named ec_Shutup, is the shut up register, write only (functions as one)");

    // =========================================================================
    // Group Identity -- register 44/48 are write-only, and each completes
    // configuration in its own primary, described context (Zorro III for
    // 44, Zorro II for 48). Graded on role, NOT on the specific bit range
    // latched (declined separately below -- no read-back boundary exists).
    // =========================================================================
#ifdef MUT_WRONGREG
    // MUTATION sol:K-0834/opus:K-0866 stimulus red: MUT=WRONGREG register 44
    // tested against Zorro II (wrong context) -- does nothing there.
    Harness id44(0, 1, {READ(autocfg_addr(IDX_REG44)),
                         READ(autocfg_addr(IDX_REG00)),
                         WRITE_IMM(autocfg_addr(IDX_REG44), 0x1000),
                         READ(autocfg_addr(IDX_REG00))});
    // MUTATION sol:K-0835/opus:K-0867 stimulus red: MUT=WRONGREG register 48
    // tested against Zorro III (wrong context) -- does nothing there (this
    // IS the finding's own mechanism).
    Harness id48(2, 4, {READ(autocfg_addr(IDX_REG48)),
                         READ(autocfg_addr(IDX_REG00)),
                         WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),
                         READ(autocfg_addr(IDX_REG00))});
#else
    Harness id44(2, 4, {READ(autocfg_addr(IDX_REG44)),
                         READ(autocfg_addr(IDX_REG00)),
                         WRITE_IMM(autocfg_addr(IDX_REG44), 0x1000),
                         READ(autocfg_addr(IDX_REG00))});
    Harness id48(0, 1, {READ(autocfg_addr(IDX_REG48)),
                         READ(autocfg_addr(IDX_REG00)),
                         WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),
                         READ(autocfg_addr(IDX_REG00))});
#endif
    uint8_t r44_readonly_raw = logical(IDX_REG44, id44.nibble(0));
    uint8_t r44_before = id44.nibble(1);
    uint8_t r44_after = id44.nibble(2);
    note("Group Identity, register44: read-before-any-write logical=0x" + hex1(r44_readonly_raw) +
         " (unmapped for reads -> default); reg00 before/after writing register44 in its own "
         "primary (Zorro III) context: 0x" + hex1(r44_before) + " / 0x" + hex1(r44_after));
    bool r44_write_only = (r44_readonly_raw == 0x0);
    bool r44_configures_in_own_context = id44.all_captured && (r44_before != 0x0) && (r44_after == 0x0);
    expect(r44_write_only && r44_configures_in_own_context,
           "sol:K-0834 logical register 44 is the write-only high-order base-address register (functions as the base-address-completing register for Zorro III)");
    expect(r44_write_only && r44_configures_in_own_context,
           "opus:K-0866 register 44 is the high order base address register, write only (functions as one for Zorro III)");

    uint8_t r48_readonly_raw = logical(IDX_REG48, id48.nibble(0));
    uint8_t r48_before = id48.nibble(1);
    uint8_t r48_after = id48.nibble(2);
    note("Group Identity, register48: read-before-any-write logical=0x" + hex1(r48_readonly_raw) +
         " (unmapped for reads -> default); reg00 before/after writing register48 in its own "
         "primary (Zorro II) context: 0x" + hex1(r48_before) + " / 0x" + hex1(r48_after));
    bool r48_write_only = (r48_readonly_raw == 0x0);
    bool r48_configures_in_own_context = id48.all_captured && (r48_before != 0x0) && (r48_after == 0x0);
    expect(r48_write_only && r48_configures_in_own_context,
           "sol:K-0835 logical register 48 is the write-only low-order base-address register (functions as the base-address-completing register for Zorro II)");
    expect(r48_write_only && r48_configures_in_own_context,
           "opus:K-0867 register 48 is the low order base address register, write only (functions as one for Zorro II)");

    // =========================================================================
    // Declined: specific bit-range/write-width content of registers 44/48
    // -- no read-back boundary exists on this module (see
    // SPECIFICATION.md).
    // =========================================================================
    const std::string bitrange_reason =
        "reason_kind: unreachable-observable. Registers 44/48 are write-only "
        "with no read-back path anywhere in this design (cpu_wrapper.v:565-608, "
        "the autocfg_data read case lists, read in full -- neither $E80044 nor "
        "$E80048 ever appears there). Observing what was actually latched "
        "(cpu_dout[15:12] into z3ram_base1, cpu_dout[15:11] into z3ram_base0, "
        "or nothing at all for register 48) needs either an internal probe "
        "(forbidden) or a live RAM-model fixture driving ramsel/ramready, "
        "which this fixture's permanently-low ramready cannot safely exercise "
        "without risking the hang hrm/appK-autoconfig-register-map's own "
        "SPECIFICATION.md records from this fixture's early development -- "
        "the same boundary hrm/appK-autoconfig-size-encoding already declined "
        "the 'natural boundary alignment' claim over.";

    record("sol:K-0836 NOT GRADED (p435): 'Register 44 receives configured address bits A31-A24.' " + bitrange_reason);
    record("opus:K-0868 NOT GRADED (p435, same claim as sol:K-0836): " + bitrange_reason);
    record("sol:K-0837 NOT GRADED (p435): 'Register 48 receives configured address bits A23-A16.' " + bitrange_reason);
    record("opus:K-0869 NOT GRADED (p435, same claim as sol:K-0837): " + bitrange_reason);
    record("sol:K-0839 NOT GRADED (p435): 'A nybble write to register 44 supplies A31-A28, while a byte write supplies A31-A24.' " + bitrange_reason);
    record("opus:K-0873 NOT GRADED (p435, nybble-wide half of sol:K-0839): " + bitrange_reason);
    record("opus:K-0874 NOT GRADED (p435, byte-wide half of sol:K-0839): " + bitrange_reason);
    record("sol:K-0841 NOT GRADED (p435): 'A nybble write to register 48 supplies A23-A20, while a byte write supplies A23-A16.' " + bitrange_reason);
    record("opus:K-0877 NOT GRADED (p435, nybble-wide half of sol:K-0841): " + bitrange_reason);
    record("opus:K-0878 NOT GRADED (p435, byte-wide half of sol:K-0841): " + bitrange_reason);
    record("opus:K-0870 NOT GRADED (p435): 'For Zorro III boards configured in the Zorro II "
           "space the configuration address is written both nybble and byte wide.' The "
           "write-detect trigger (cpu_wrapper.v:629, keyed only on ~chip_uds) does not itself "
           "distinguish a byte-wide from a word-wide write, but confirming that a byte-wide "
           "CPU write instruction latches address content identically requires observing the "
           "SAME latched-content boundary as the other bit-range claims. " + bitrange_reason);

    // =========================================================================
    // Liveness -- carries no claim uid.
    // =========================================================================
    expect(finding.all_captured && shutup.all_captured && id44.all_captured && id48.all_captured &&
           finding_before != 0x0 && shutup_before != 0x0,
           "fixture-liveness [not graded -- apparatus check, no claim uid]");

    std::cout << "\n" << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}

// RESULTS (WSL, Verilator 5.052, built and run 2026-09-18, TG68K build): baseline
//   `make test` -> 5 passed, 8 failed. Reproduced twice (once before, once
//   after the mutation runs below), byte-for-byte identical both times.
//   13 graded assertions (8 red, 4 green, 1 liveness) + 11 declined.
//
// MUTATION GREENCHECK_REG48 (`make MUT=GREENCHECK_REG48 test`, after
//   `make clean`): 7 passed, 6 failed -- sol:K-0844/opus:K-0881 flip to
//   PASS (register 48 genuinely completes configuration under Zorro II);
//   the 6 shut-up-group assertions stay red (untouched by this flag).
//
// MUTATION GREENCHECK_4C (`make MUT=GREENCHECK_4C test`, after `make
//   clean`): 11 passed, 2 failed -- all 6 shut-up-group assertions flip to
//   PASS (writing register 48 genuinely does silence the board); only
//   sol:K-0844/opus:K-0881 (untouched by this flag) stay red.
//
// MUTATION WRONGREG (`make MUT=WRONGREG test`, after `make clean`):
//   1 passed, 12 failed -- the 4 identity assertions (sol:K-0834/K-0835,
//   opus:K-0866/K-0867) join the 8 always-red ones. Every graded assertion
//   in this package has now been shown to flip under a real stimulus
//   change in at least one direction. Rebuilt with no MUT afterward:
//   5/8 baseline restored exactly.
//
// No expected value was adjusted after seeing a result, and no red was
// found to explain away: the 8 red assertions above (sol:K-0844,
// opus:K-0881, sol:K-0740, sol:K-0849, sol:K-0850, opus:K-0889,
// sol:K-0848, opus:K-0888) are graded exactly as the printed page states
// them, and NanoMig's cpu_wrapper.v disagrees with the page on both
// counts this package tests: register 48 does not complete configuration
// for a Zorro III board (only register 44 does, cpu_wrapper.v:629-665),
// and register 4C (shut-up) has no write handler at all, so a Zorro II
// board that itself advertises shut-up support (register08 bit6=0) can
// never actually be shut up by software in this design.
