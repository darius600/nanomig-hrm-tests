// Test purpose: appendix K's AUTOCONFIG mechanism at the `cpu_wrapper`
//   boundary -- printed pp.431, 432, 434, 435 and 438, the "THE AUTOCONFIG
//   MECHANISM" prose, the "Configuration Register Mapping" figure and the
//   pin table -- measured in the build with ENABLE_RAM32 (NanoMig's 32 MiB
//   boards), the only configuration in which cpu_wrapper.v compiles a
//   Zorro III path: the unconfigured/configured lifecycle, the
//   single-responder handoff, the read/write register split, the
//   $00E8xxxx address window, the one-nybble-per-read rule, the
//   paired-address register width and the complement rule.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 431, 432, 434, 435, 438
// Non-goals: anything outside the `cpu_wrapper` fixture this package
//   builds, and any claim it does not cite. A green here means those
//   assertions held on this fixture, in this build configuration -- not
//   that the module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/appK-build/appK-autoconfig-space.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// appK-autoconfig-space -- pp.431,432,434,435,438 Appendix K "THE AUTOCONFIG
// MECHANISM" prose, the "Configuration Register Mapping" figure and the
// pin table. 30 claims, filtered from
// hrm-testpipeline/work/appK-build/appK-autoconfig-space.jsonl. 25 are
// graded; sol:K-0737, opus:K-0764, opus:K-0771, sol:K-0757 and opus:K-0777
// are NOT GRADED (each bundles a clause, names an address, or names a bus
// mode, this configuration cannot decide -- see the NOT GRADED records at
// each one's grading point and SPECIFICATION.md).
//
// See SPECIFICATION.md for the full boundary investigation and the group
// breakdown. Fixture mechanism (Harness, core selection, active-low reset,
// direct/complement arithmetic, the register-44/48-write capability)
// reused from hrm/appK-autoconfig-register-map and
// hrm/appK-autoconfig-size-encoding.
//
// Mutation mechanism, plain summary (the individual, parseable
// `// MUTATION <label> <kind> <result>` records live next to each
// assertion below, one label per line). Twelve `-DMUT_<NAME>=1` build
// flags exist: LIFECYCLE, NORESET, NOHANDOFF, WRITETOWRITEREG,
// REG48NOEFFECT, INSIDEPROBE, NOOVERRIDE, NYBBLE, SWAPNYBBLE, NOCOMPLEMENT,
// SWEEPWRONGREG, REG00SILENT. Some assertions carry a second record of
// kind `none impossible`: a part of their claim is a structural fact (a
// fixed port width, a fixed address decode, an unassigned case entry, or
// a specific undriven address indistinguishable by value from any other
// undriven address) that no stimulus in this module's input space can
// falsify, reported rather than hidden behind a green mutation.

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
std::string hex4(uint16_t v) {
    std::string s(4, '0');
    static const char* d = "0123456789ABCDEF";
    for (int i = 3; i >= 0; i--) { s[i] = d[v & 0xF]; v >>= 4; }
    return s;
}
static bool type_bits(uint8_t nibble, uint8_t pattern2) { return ((nibble >> 2) & 0x3) == pattern2; }

static uint32_t autocfg_addr(int idx) { return 0xE80000u + 2u * (uint32_t)idx; }

// General-purpose Harness (same mechanism as
// hrm/appK-autoconfig-size-encoding's own): a strictly-ordered program of
// READ (capture to a fresh scratch slot) and WRITE_IMM operations,
// executed through cpu_wrapper's own ports only, with absolute addresses
// and a memory-preset list. Extended here with an optional cycle budget
// (so a probe that never completes does not force every harness in the
// file to spend the full default budget) and with `ramreq_seen`, a
// passive sample of the `ramreq` output port (cpu_wrapper.v:81, `assign
// ramreq = ramsel & ram_dtack;`, cpu_wrapper.v:168) -- an existing module
// port, not a debug port added for this package.
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
    bool ramreq_seen = false;

    void tick() { top->clk = 0; top->eval(); top->clk = 1; top->eval(); }

    Harness(uint8_t cpucfg_v, uint32_t fastramcfg_v, const std::vector<Op>& ops,
            const std::map<uint32_t, uint16_t>& presets = {}, int max_cyc_override = -1) {
        top = new Vcpu_wrapper;
        mem[0x000000] = 0x0001; mem[0x000002] = 0x0000; // SSP = 0x00010000
        mem[0x000004] = 0x0000; mem[0x000006] = 0x1000; // PC  = 0x00001000
        for (auto& p : presets) mem[p.first] = p.second;

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
        const int MAX_CYC = (max_cyc_override > 0) ? max_cyc_override : 200000;
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
            if (top->ramreq) ramreq_seen = true;

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

// Dedicated probe for sol:K-0743/opus:K-0767's "16-bit Zorro II cycles":
// one MOVE.W read of register 00 ($E80000), sampling chip_uds and chip_lds
// at every cycle chip_as is asserted AND the address is inside $E8xxxx --
// excluding the reset-vector fetch, instruction fetches and the
// write-back to scratch. A word-wide access asserts both strobes together
// for the whole cycle.
struct StrobeProbe {
    int as_low_samples = 0;
    int uds_high_samples = 0;
    int lds_high_samples = 0;
    bool completed = false;

    StrobeProbe() {
        Vcpu_wrapper* top = new Vcpu_wrapper;
        std::map<uint32_t, uint16_t> mem;
        mem[0x000000] = 0x0001; mem[0x000002] = 0x0000;
        mem[0x000004] = 0x0000; mem[0x000006] = 0x1000;
        uint32_t pc = 0x1000;
        mem[pc] = 0x3039; mem[pc+2] = (uint16_t)(0xE80000>>16); mem[pc+4] = (uint16_t)(0xE80000&0xFFFF); pc += 6;
        mem[pc] = 0x33C0; mem[pc+2] = 0x0000; mem[pc+4] = 0x2000; pc += 6;
        mem[pc] = 0x60FE;

        top->reset = 0; top->clk = 0; top->ph1 = 0; top->ph2 = 0;
        top->cpucfg = 0; top->fastramcfg = 1; top->chipramcfg = 0; top->slowramcfg = 0;
        top->turbocfg = 0; top->bootrom = 0; top->chip_dout = 0; top->chip_dtack = 0;
        top->chip_ipl = 7; top->ramdout = 0; top->ramready = 0;
        top->eval();

        std::map<uint32_t, uint16_t> captured;
        bool last_as = true, still_watching = true;
        const int RESET_CYCLES = 20, MAX_CYC = 3000;
        for (int cyc = 0; cyc < MAX_CYC && !captured.count(0x2000); cyc++) {
            if (cyc == RESET_CYCLES) top->reset = 1;
            uint32_t ba = ((uint32_t)top->chip_addr) << 1;
            auto it = mem.find(ba & ~1u);
            top->chip_dout = (it != mem.end()) ? it->second : 0;
            top->eval();
            int phase = cyc % 4;
            top->ph1 = (phase == 3); top->ph2 = (phase == 1);
            top->clk = 0; top->eval(); top->clk = 1; top->eval();

            if (cyc >= RESET_CYCLES) {
                bool as_now = top->chip_as;
                if (!as_now && still_watching && (ba & 0xFF0000u) == 0xE80000u) {
                    as_low_samples++;
                    if (top->chip_uds) uds_high_samples++;
                    if (top->chip_lds) lds_high_samples++;
                }
                if (last_as && !as_now && !top->chip_rw) {
                    uint32_t a = ((uint32_t)top->chip_addr) << 1;
                    if (a == 0x2000 && !captured.count(0x2000)) {
                        captured[0x2000] = top->chip_din;
                        still_watching = false;
                    }
                }
                last_as = as_now;
            }
        }
        completed = captured.count(0x2000) > 0;
        top->final(); delete top;
    }
};

// Dedicated probe for the "...or RESETS..." half of sol:K-0727/opus:K-0755:
// configure both boards of the Zorro III chain (cpucfg=2, fastramcfg=5),
// confirm the space falls silent, pulse `reset` mid-run, then WALK THE
// CHAIN AGAIN (read register 00, write register 44, read register 00 once
// more), requiring at least a second responding point post-reset, so
// "every card" is observed re-arming rather than inferred from board 1's
// own return. The first post-reset reading is also compared, byte for
// byte, against register 00's reading from before any write. The mutation
// splices its probe program at wherever the CPU will actually fetch from
// next -- $1000 if reset genuinely redirects it there, or its own current
// self-loop location if the pulse is skipped -- so the no-pulse case is a
// genuine observation of the chain staying silent, not an apparatus
// failure that happens to read as red.
struct ResetProbe {
    bool before_responds = false;
    bool after_config_silent = false;
    bool after_reset_responds = false;
    bool after_reset_point1_responds = false;
    bool byte_matches_before = false;
    uint8_t before_hi = 0, before_lo = 0, after_config_hi = 0;
    uint8_t after_reset_hi = 0, after_reset_lo = 0, after_reset_point1_hi = 0;
    bool completed = false;

    explicit ResetProbe(bool skip_pulse) {
        Vcpu_wrapper* top = new Vcpu_wrapper;
        std::map<uint32_t, uint16_t> mem;
        mem[0x000000] = 0x0001; mem[0x000002] = 0x0000;
        mem[0x000004] = 0x0000; mem[0x000006] = 0x1000;

        auto emit_read = [&](uint32_t& pc, uint32_t addr, uint32_t scratch) {
            mem[pc] = 0x3039; mem[pc+2] = (uint16_t)(addr>>16); mem[pc+4] = (uint16_t)(addr&0xFFFF); pc += 6;
            mem[pc] = 0x33C0; mem[pc+2] = (uint16_t)(scratch>>16); mem[pc+4] = (uint16_t)(scratch&0xFFFF); pc += 6;
        };
        auto emit_write = [&](uint32_t& pc, uint32_t addr, uint16_t imm) {
            mem[pc] = 0x33FC; mem[pc+2] = imm;
            mem[pc+4] = (uint16_t)(addr>>16); mem[pc+6] = (uint16_t)(addr&0xFFFF); pc += 8;
        };

        uint32_t pc = 0x1000;
        emit_read(pc, 0xE80000, 0x2000);   // before: register 00 hi (board 1)
        emit_read(pc, 0xE80002, 0x2002);   // before: register 00 lo (board 1)
        emit_write(pc, 0xE80044, 0x1000);  // configure board 1 -> board 2
        emit_write(pc, 0xE80044, 0x2000);  // configure board 2 -> silent
        emit_read(pc, 0xE80000, 0x2004);   // after configuring both, before reset: expect silence
        uint32_t orig_end_pc = pc;         // where an UN-reset CPU is stuck looping
        mem[pc] = 0x60FE;

        top->reset = 0; top->clk = 0; top->ph1 = 0; top->ph2 = 0;
        top->cpucfg = 2; top->fastramcfg = 5; top->chipramcfg = 0; top->slowramcfg = 0;
        top->turbocfg = 0; top->bootrom = 0; top->chip_dout = 0; top->chip_dtack = 0;
        top->chip_ipl = 7; top->ramdout = 0; top->ramready = 0;
        top->eval();

        std::map<uint32_t, uint16_t> captured;
        bool last_as = true, pulsed = false;
        const int RESET_CYCLES = 20, MAX_CYC = 14000;
        for (int cyc = 0; cyc < MAX_CYC && !captured.count(0x200A); cyc++) {
            if (cyc == RESET_CYCLES) top->reset = 1;
            uint32_t ba = ((uint32_t)top->chip_addr) << 1;
            auto it = mem.find(ba & ~1u);
            top->chip_dout = (it != mem.end()) ? it->second : 0;
            top->eval();
            int phase = cyc % 4;
            top->ph1 = (phase == 3); top->ph2 = (phase == 1);
            top->clk = 0; top->eval(); top->clk = 1; top->eval();

            if (cyc >= RESET_CYCLES) {
                bool as_now = top->chip_as;
                if (last_as && !as_now && !top->chip_rw) {
                    uint32_t a = ((uint32_t)top->chip_addr) << 1;
                    if ((a == 0x2000 || a == 0x2002) && !captured.count(a)) captured[a] = top->chip_din;
                    else if (a == 0x2004 && !captured.count(0x2004)) {
                        captured[0x2004] = top->chip_din;
                        // Both boards are configured and silent. Splice a
                        // "walk the chain again" program at wherever the
                        // CPU will next fetch from: $1000 if a reset sends
                        // it there, or its OWN current self-loop location
                        // if not.
                        uint32_t ppc = skip_pulse ? orig_end_pc : 0x1000u;
                        emit_read(ppc, 0xE80000, 0x2006);   // post-reset point 0 hi
                        emit_read(ppc, 0xE80002, 0x2008);   // post-reset point 0 lo
                        emit_write(ppc, 0xE80044, 0x1000);  // post-reset write: re-configure board 1
                        emit_read(ppc, 0xE80000, 0x200A);   // post-reset point 1 hi
                        mem[ppc] = 0x60FE;
                        // MUTATION sol:K-0727 stimulus red: MUT=NORESET
                        //   skips driving `reset` low again. The splice
                        //   above lands where the CPU actually fetches
                        //   from next either way, so without the pulse it
                        //   reads the chain exactly as the two configuring
                        //   writes left it -- silent at both post-splice
                        //   points, and the post-splice write (already
                        //   configured, sel_autoconfig off) has no effect
                        //   either -- instead of the chain responding
                        //   again.
                        // MUTATION opus:K-0755 stimulus red: same switch
                        //   as sol:K-0727 (MUT=NORESET).
                        if (!skip_pulse) {
                            top->reset = 0;
                            for (int r = 0; r < RESET_CYCLES; r++) { top->clk = 0; top->eval(); top->clk = 1; top->eval(); }
                            top->reset = 1;
                        }
                        pulsed = true;
                    } else if (pulsed && (a == 0x2006 || a == 0x2008 || a == 0x200A) && !captured.count(a)) {
                        captured[a] = top->chip_din;
                    }
                }
                last_as = as_now;
            }
        }
        completed = captured.count(0x2000) && captured.count(0x2002) && captured.count(0x2004) &&
                    captured.count(0x2006) && captured.count(0x2008) && captured.count(0x200A);
        before_hi             = (uint8_t)((captured.count(0x2000) ? captured[0x2000] : 0) >> 12 & 0xF);
        before_lo              = (uint8_t)((captured.count(0x2002) ? captured[0x2002] : 0) >> 12 & 0xF);
        after_config_hi        = (uint8_t)((captured.count(0x2004) ? captured[0x2004] : 0) >> 12 & 0xF);
        after_reset_hi          = (uint8_t)((captured.count(0x2006) ? captured[0x2006] : 0) >> 12 & 0xF);
        after_reset_lo          = (uint8_t)((captured.count(0x2008) ? captured[0x2008] : 0) >> 12 & 0xF);
        after_reset_point1_hi = (uint8_t)((captured.count(0x200A) ? captured[0x200A] : 0) >> 12 & 0xF);
        before_responds             = before_hi != 0x0;
        after_config_silent         = after_config_hi == 0x0;
        after_reset_responds        = completed && (after_reset_hi != 0x0);
        after_reset_point1_responds = completed && (after_reset_point1_hi != 0x0);
        byte_matches_before  = completed && (before_hi == after_reset_hi) && (before_lo == after_reset_lo);
        top->final(); delete top;
    }
};

int main() {
    const int IDX_REG00 = 0, IDX_REG00LO = 1, IDX_REG3C = 30, IDX_REG3CLO = 31;
    const int IDX_REG44 = 34, IDX_REG48 = 36;

    // =========================================================================
    // Group 1 -- lifecycle. Read register 00 before any write (expect the
    // real type nibble -> card responds while unconfigured); write
    // register 48 (completes Zorro II configuration); read register 00
    // again (expect silence -> card removed itself from $E8xxxx).
    // sol:K-0727 and opus:K-0755 also name RESET, not only power-up
    // ("When an Amiga powers up OR RESETS..."); ResetProbe configures both
    // boards of the Zorro III chain, confirms silence, pulses `reset`
    // again, then WALKS THE CHAIN AGAIN post-reset, requiring a second
    // responding point so "every card" is observed re-arming.
    // =========================================================================
#ifdef MUT_LIFECYCLE
    // MUTATION sol:K-0727 stimulus red: MUT=LIFECYCLE sets fastramcfg=0 on
    //   the "before" harness. sel_autoconfig requires fastramcfg_d nonzero
    //   (cpu_wrapper.v:611), so the pre-configuration read never sees a
    //   real type nibble either.
    // MUTATION opus:K-0755 stimulus red: same switch as sol:K-0727 (the
    //   fastramcfg=0 half of MUT=LIFECYCLE).
    // MUTATION sol:K-0734 stimulus red: same switch as sol:K-0727 (the
    //   fastramcfg=0 half of MUT=LIFECYCLE).
    Harness lc_before(0, 0, {READ(autocfg_addr(IDX_REG00))});
#else
    Harness lc_before(0, 1, {READ(autocfg_addr(IDX_REG00))});
#endif
    uint8_t before_raw = lc_before.nibble(0);
    bool responds_when_unconfigured = lc_before.all_captured && (before_raw != 0x0);
    note("Group1 before-config: chip_addr index 0 raw nibble=0x" + hex1(before_raw) +
         " -- nonzero means sel_autoconfig asserted (card responds while unconfigured).");

#ifdef MUT_LIFECYCLE
    // MUTATION sol:K-0739 stimulus red: MUT=LIFECYCLE skips the
    //   register-48 write on the "after" harness, so the post-configuration
    //   read is compared against a card that was never actually configured
    //   and is still in its unconfigured (responding) state.
    // MUTATION sol:K-0842 stimulus red: same switch as sol:K-0739 (the
    //   skipped-write half of MUT=LIFECYCLE).
    // MUTATION sol:K-0843 stimulus red: same switch as sol:K-0739 (the
    //   skipped-write half of MUT=LIFECYCLE).
    // MUTATION opus:K-0879 stimulus red: same switch as sol:K-0739 (the
    //   skipped-write half of MUT=LIFECYCLE).
    // MUTATION opus:K-0880 stimulus red: same switch as sol:K-0739 (the
    //   skipped-write half of MUT=LIFECYCLE).
    Harness lc_after(0, 1, {READ(autocfg_addr(IDX_REG00))});
#else
    Harness lc_after(0, 1, {READ(autocfg_addr(IDX_REG00)),
                             WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),
                             READ(autocfg_addr(IDX_REG00))});
#endif
    uint8_t after_raw = lc_after.nibble(lc_after.word.size() - 1);
    bool disappears_after_configured = lc_after.all_captured && (after_raw == 0x0);
    note("Group1 after-config: chip_addr index 0 raw nibble=0x" + hex1(after_raw) +
         " -- zero means sel_autoconfig deasserted (card silent after configuring).");

#ifdef MUT_NORESET
    ResetProbe rp(true);
#else
    ResetProbe rp(false);
#endif
    note("Group1 reset-mid-run (two-board Zorro III chain): before=0x" + hex1(rp.before_hi) + hex1(rp.before_lo) +
         " after-config(pre-reset)=0x" + hex1(rp.after_config_hi) +
         " after-reset point0=0x" + hex1(rp.after_reset_hi) + hex1(rp.after_reset_lo) +
         " after-reset point1 (post-reset write, walking the chain again)=0x" + hex1(rp.after_reset_point1_hi) +
         " (point0's byte compared against before's byte for equality, not against an assumed constant; "
         "point1 responding too means the chain re-armed, not only board 1).");
    bool reset_rearms_chain = rp.completed && rp.before_responds &&
                               rp.after_config_silent && rp.after_reset_responds &&
                               rp.after_reset_point1_responds && rp.byte_matches_before;

    expect(responds_when_unconfigured && reset_rearms_chain, "sol:K-0727 power-up/reset places the card in its unconfigured (still-responding) state");
    expect(responds_when_unconfigured && reset_rearms_chain, "opus:K-0755 power-up/reset places every card in its unconfigured (still-responding) state");
    expect(responds_when_unconfigured, "sol:K-0734 an enabled unconfigured card responds in configuration space");
    expect(disappears_after_configured, "sol:K-0739 writing the final base-address portion completes configuration (space falls silent)");
    expect(disappears_after_configured, "sol:K-0842 a board removes its configuration registers after configuration");
    expect(disappears_after_configured, "sol:K-0843 a configured board stops responding in $00E8xxxx");
    expect(disappears_after_configured, "opus:K-0879 configuration registers cannot remain active after configuration -- they disappear");
    expect(disappears_after_configured, "opus:K-0880 configuration registers move out of the $00E8xxxx space once a board is configured");

    // =========================================================================
    // Group 2 -- handoff (sol:K-0733). Zorro III: two successive
    // register-44 writes, each completing one board. `ramreq`
    // (cpu_wrapper.v:81/168, an existing port) IS reachable here:
    // sel_z3ram1 compares cpu_addr[31:28] -- always 0 on this fixture's
    // 24-bit core (`TG68K_A24`, cpu_wrapper.v:34, cpu_wrapper.v:279's
    // `cpu_addr = {8'h00, cpu_addr_p[23:0]}`) -- against z3ram_base1,
    // which is set from the configuring write's OWN data
    // (cpu_dout[15:12], cpu_wrapper.v:655). Writing base 0x0000 makes
    // z3ram_base1 also 0000, which matches cpu_addr[31:28]; a base of
    // 0x1000 (upper nibble 0x1) does not. Graded on: `ramreq` asserted after a write with
    // base 0x0000 (board 1 now live real memory) in a harness that has no
    // further chip-bus traffic to be intercepted by that same decode;
    // register 00 still answering after a SEPARATE write with base 0x1000
    // (safe, does not redirect the following read); the space silent
    // after a second write; board 2's own completion corroborated the
    // same way (base 0x0000 on write 2). Size nibbles are reported as
    // MEASURED lines only -- NanoMig's own board sizes are not a value
    // graded against, per sol:K-0737's own withdrawal reasoning.
    // =========================================================================
#ifdef MUT_NOHANDOFF
    // MUTATION sol:K-0733 stimulus red: MUT=NOHANDOFF diverts the write
    //   meant to configure board 1 with base 0x0000 to register 00 (a
    //   read-only register with no configuring effect) instead of
    //   register 44 -- z3ram_ena1 never latches (cpu_wrapper.v:656), so
    //   `ramreq` never asserts for board 1's range even though base
    //   0x0000 would otherwise match cpu_addr[31:28].
    Harness handoff_ramreq_w1(2, 5, {WRITE_IMM(autocfg_addr(IDX_REG00), 0x0000),
                                      READ(autocfg_addr(IDX_REG00))}, {}, 3000);
#else
    Harness handoff_ramreq_w1(2, 5, {WRITE_IMM(autocfg_addr(IDX_REG44), 0x0000),
                                      READ(autocfg_addr(IDX_REG00))}, {}, 3000);
#endif
    Harness handoff_stillresponds(2, 5, {READ(autocfg_addr(IDX_REG00)),
                                          WRITE_IMM(autocfg_addr(IDX_REG44), 0x1000),
                                          READ(autocfg_addr(IDX_REG00))});
    Harness handoff_silent(2, 5, {READ(autocfg_addr(IDX_REG00)), READ(autocfg_addr(IDX_REG00LO)),
                                   WRITE_IMM(autocfg_addr(IDX_REG44), 0x1000),
                                   READ(autocfg_addr(IDX_REG00)), READ(autocfg_addr(IDX_REG00LO)),
                                   WRITE_IMM(autocfg_addr(IDX_REG44), 0x2000),
                                   READ(autocfg_addr(IDX_REG00))});
    Harness handoff_ramreq_w2(2, 5, {WRITE_IMM(autocfg_addr(IDX_REG44), 0x1000),
                                      WRITE_IMM(autocfg_addr(IDX_REG44), 0x0000),
                                      READ(autocfg_addr(IDX_REG00))}, {}, 3000);

    uint8_t hf_hi0 = handoff_silent.nibble(0), hf_lo0 = handoff_silent.nibble(1);
    uint8_t hf_hi1 = handoff_silent.nibble(2), hf_lo1 = handoff_silent.nibble(3);
    uint8_t hf_hi2 = handoff_silent.nibble(4);
    note("Group2 handoff: reg00 hi/lo before=0x" + hex1(hf_hi0) + hex1(hf_lo0) +
         " after-write1=0x" + hex1(hf_hi1) + hex1(hf_lo1) + " after-write2=0x" + hex1(hf_hi2) +
         " (size nibbles measured only, not graded against -- see sol:K-0737's own record). "
         "ramreq after a base-0x0000 write completing board 1 = " +
         std::string(handoff_ramreq_w1.ramreq_seen ? "yes" : "no") +
         "; after board 2 = " + std::string(handoff_ramreq_w2.ramreq_seen ? "yes" : "no") +
         ". Space still answers after a base-0x1000 write: before=0x" +
         hex1(handoff_stillresponds.nibble(0)) + " after=0x" + hex1(handoff_stillresponds.nibble(1)));
    bool handoff_board1_ramreq = handoff_ramreq_w1.ramreq_seen;
    bool handoff_board2_ramreq = handoff_ramreq_w2.ramreq_seen;
    bool handoff_responds_before = handoff_stillresponds.all_captured && (handoff_stillresponds.nibble(0) != 0x0);
    bool handoff_responds_after_write1 = handoff_stillresponds.all_captured && (handoff_stillresponds.nibble(1) != 0x0);
    bool handoff_silent_after_write2 = handoff_silent.all_captured && (hf_hi2 == 0x0);
    bool handoff_ok = handoff_board1_ramreq && handoff_board2_ramreq && handoff_responds_before &&
                       handoff_responds_after_write1 && handoff_silent_after_write2;
    expect(handoff_ok, "sol:K-0733 the configuration chain hands off to exactly one still-unconfigured responder at a time");
    // MUTATION sol:K-0733 none impossible: unfalsifiable at this boundary --
    //   autocfg_data is one 4-bit register (cpu_wrapper.v:230) driven by
    //   one case statement (cpu_wrapper.v:546-608) selected by one two-bit
    //   autocfg_card register (cpu_wrapper.v:229) and gated onto the chip
    //   bus by one wire, sel_autoconfig (cpu_wrapper.v:611). No stimulus in
    //   cpu_wrapper's own input space can make it drive two boards' worth
    //   of read data at once; "exactly one at a time" is a property of
    //   this design's structure, not something a mutation demonstrates.

    // =========================================================================
    // Group 3 -- read/write split (sol:K-0735, opus:K-0762). In this
    // harness (cpucfg=0, Zorro II context) register 44 is inert: nothing
    // is mid-configuration to interpret chip_addr[6:1]==6'b100010, so it
    // reads whatever every other reserved register reads (the unassigned
    // default, cpu_wrapper.v:547) -- that raw value does not, by itself,
    // tell a write-only register apart from a read register, so it is not
    // graded. Graded instead on: register 00 (read-only) ignores a write;
    // register 48 (write-only, the one write register this harness's
    // Zorro II path acts on) completes configuration on a write.
    // =========================================================================
#if defined(MUT_WRITETOWRITEREG)
    // MUTATION sol:K-0735 stimulus red: MUT=WRITETOWRITEREG writes
    //   register 48 instead of register 00 for the "no effect" check, so
    //   the "before"/"after" comparison shows the board configuring
    //   (silent) instead of staying unaffected.
    // MUTATION opus:K-0762 stimulus red: same switch as sol:K-0735.
    Harness rwsplit(0, 1, {READ(autocfg_addr(IDX_REG00)),
                            WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),
                            READ(autocfg_addr(IDX_REG00)),
                            WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),
                            READ(autocfg_addr(IDX_REG00))});
#elif defined(MUT_REG48NOEFFECT)
    // MUTATION sol:K-0735 stimulus red: MUT=REG48NOEFFECT diverts the
    //   SECOND write (meant to complete configuration via register 48) to
    //   $E90000, outside the decoded window -- register 48 no longer has
    //   any effect, so the "write-only register DOES act" half goes red.
    // MUTATION opus:K-0762 stimulus red: same switch as sol:K-0735.
    Harness rwsplit(0, 1, {READ(autocfg_addr(IDX_REG00)),
                            WRITE_IMM(autocfg_addr(IDX_REG00), 0x0000),
                            READ(autocfg_addr(IDX_REG00)),
                            WRITE_IMM(0xE90000, 0x0000),
                            READ(autocfg_addr(IDX_REG00))});
#else
    Harness rwsplit(0, 1, {READ(autocfg_addr(IDX_REG00)),
                            WRITE_IMM(autocfg_addr(IDX_REG00), 0x0000),
                            READ(autocfg_addr(IDX_REG00)),
                            WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),
                            READ(autocfg_addr(IDX_REG00))});
#endif
    uint8_t rws_before      = rwsplit.nibble(0);
    uint8_t rws_after_reg00 = rwsplit.nibble(1);
    uint8_t rws_after_reg48 = rwsplit.nibble(2);
    note("Group3 read/write split: register00 before=0x" + hex1(rws_before) +
         "; after a write to register00 (read-only)=0x" + hex1(rws_after_reg00) +
         "; after a write to register48 (write-only)=0x" + hex1(rws_after_reg48));
    bool write_to_read_reg_no_effect = (rws_before == rws_after_reg00) && (rws_after_reg00 != 0x0);
    bool write_to_write_reg_has_effect = (rws_after_reg48 == 0x0);
    expect(write_to_read_reg_no_effect && write_to_write_reg_has_effect,
           "sol:K-0735 configuration space holds read-only registers followed by write-only registers");
    expect(write_to_read_reg_no_effect && write_to_write_reg_has_effect,
           "opus:K-0762 the PIC asserts a set of read-only registers followed by a set of write-only registers");

    // =========================================================================
    // sol:K-0737 / opus:K-0764 -- NOT GRADED. The sentence bundles device
    // type, size and other requirements into one claim. This configuration
    // CAN contrast the type field against an independent reference for
    // BOTH devices this design implements -- Zorro II reads bits 3-2 =='11'
    // (cpu_wrapper.v:569) and Zorro III reads '10' (cpu_wrapper.v:595),
    // matching Appendix K's own register-map table -- but the declared
    // SIZE (2/4/8 MiB for Zorro II, 128/256 MiB for Zorro III,
    // cpu_wrapper.v:572-575/596) is NanoMig's own implementation choice,
    // not a value the manual prescribes, and "other requirements" is not
    // observable at this boundary at all. The stronger type evidence this
    // configuration offers narrows what is undecided; it does not remove
    // it. A claim bundling an untested clause is withdrawn whole rather
    // than kept with a caveat.
    // =========================================================================
    {
        Harness content_zii(0, 1, {READ(autocfg_addr(IDX_REG00))});
        Harness content_ziii(2, 4, {READ(autocfg_addr(IDX_REG00))});
        uint8_t type_zii = content_zii.nibble(0);
        uint8_t type_ziii = content_ziii.nibble(0);
        bool type_matches_table = type_bits(type_zii, 0b11) && type_bits(type_ziii, 0b10);
        note("Group4 content (context for K-0737/K-0764's withdrawal, not graded): Zorro-II-context "
             "type raw=0x" + hex1(type_zii) + " Zorro-III-context type raw=0x" + hex1(type_ziii) +
             " -- bits 3-2 match Appendix K's register-map table (11/10): " +
             std::string(type_matches_table ? "yes" : "no") + ".");
        const std::string content_reason =
            "reason_kind: source-silent (the type sub-clause is observable and independently "
            "checkable here -- register 00 reads bits 3-2 == '11' for a Zorro II board and '10' "
            "for a Zorro III board, matching Appendix K's own register-map table -- but the "
            "printed page states nothing to assert the SIZE or OTHER REQUIREMENTS clauses "
            "against). Bundles device type, size and other requirements. The declared SIZE (2/4/8 "
            "MiB Zorro II, 128/256 MiB Zorro III, cpu_wrapper.v:572-575/596) is NanoMig's own "
            "implementation choice; the page prints no expected value to check it against, and "
            "'other requirements' is not observable at this boundary at all.";
        record("sol:K-0737 NOT GRADED (p431): 'read registers describe the "
               "device's size, type, and other requirements' " + content_reason);
        record("opus:K-0764 NOT GRADED (p431, same claim as sol:K-0737): " + content_reason);
    }

    // =========================================================================
    // Group 5 -- address window (sol:K-0742, sol:K-0743, sol:K-0744,
    // opus:K-0767). $E90000 (one byte outside the window) and $E7FFFE (one
    // word below it) never get the AUTOCONFIG override; $E8FFFE (the top
    // of the window) and $E80100 (past the 6-bit chip_addr[6:1] case
    // range) each carry their OWN preset and are graded on "reads
    // something OTHER than the preset" (an override happened), not
    // against an assumed RTL constant.
    // =========================================================================
#ifdef MUT_INSIDEPROBE
    // MUTATION sol:K-0742 stimulus red: MUT=INSIDEPROBE moves the "outside"
    //   probe (and its own preset) to $E80002 (inside $E8xxxx, register 00's
    //   own low address) instead of $E90000 -- the "does NOT get the
    //   override" check, still comparing against the moved preset's own top
    //   nibble (0x5), goes red because $E80002 IS autoconfig-driven.
    // MUTATION sol:K-0744 stimulus red: same switch as sol:K-0742.
    // MUTATION opus:K-0767 stimulus red: same switch as sol:K-0742 (the
    //   window-confinement clause of its bundled claim).
    std::map<uint32_t, uint16_t> aw_presets = {
        {0xE7FFFE, 0x5678}, {0xE80002, 0x5678}, {0xE8FFFE, 0x5FFF}, {0xE80100, 0x5678} };
    Harness aw(0, 1, {READ(autocfg_addr(IDX_REG00)),
                       READ(0xE80002),
                       READ(0xE80100),
                       READ(0xE7FFFE),
                       READ(0xE8FFFE)}, aw_presets);
#elif defined(MUT_NOOVERRIDE)
    // MUTATION sol:K-0742 stimulus red: MUT=NOOVERRIDE sets fastramcfg=0
    //   for this harness, disabling sel_autoconfig entirely
    //   (cpu_wrapper.v:611) -- $E80100 and $E8FFFE now read their own
    //   presets back unmodified instead of being overridden.
    // MUTATION sol:K-0744 stimulus red: same switch as sol:K-0742.
    // MUTATION opus:K-0767 stimulus red: same switch as sol:K-0742 (the
    //   window-extent clauses of its bundled claim).
    std::map<uint32_t, uint16_t> aw_presets = {
        {0xE7FFFE, 0x5678}, {0xE90000, 0x5678}, {0xE8FFFE, 0x5FFF}, {0xE80100, 0x5678} };
    Harness aw(0, 0, {READ(autocfg_addr(IDX_REG00)),
                       READ(0xE90000),
                       READ(0xE80100),
                       READ(0xE7FFFE),
                       READ(0xE8FFFE)}, aw_presets);
#else
    std::map<uint32_t, uint16_t> aw_presets = {
        {0xE7FFFE, 0x5678}, {0xE90000, 0x5678}, {0xE8FFFE, 0x5FFF}, {0xE80100, 0x5678} };
    Harness aw(0, 1, {READ(autocfg_addr(IDX_REG00)),
                       READ(0xE90000),
                       READ(0xE80100),
                       READ(0xE7FFFE),
                       READ(0xE8FFFE)}, aw_presets);
#endif
    uint8_t aw_inside        = aw.nibble(0);
    uint8_t aw_outside       = aw.nibble(1);
    uint8_t aw_aliased       = aw.nibble(2);
    uint16_t aw_below_word   = aw.word[3];
    uint8_t aw_top_of_window = aw.nibble(4);
    note("Group5 address window: $E80000(reg00)=0x" + hex1(aw_inside) +
         " probe(outside)=0x" + hex1(aw_outside) +
         " $E80100(aliased inside, preset top nibble 0x5)=0x" + hex1(aw_aliased) +
         " $E7FFFE(below, full word)=0x" + hex4(aw_below_word) +
         " (preset 0x5678) $E8FFFE(top of window, preset top nibble 0x5)=0x" +
         hex1(aw_top_of_window));
    bool window_confined_to_e8 = (aw_outside == 0x5);
    bool addr_0x100_overridden = (aw_aliased != 0x5);
    bool below_window_passthrough = (aw_below_word == 0x5678);
    bool top_of_window_overridden = (aw_top_of_window != 0x5);
    bool window_spans_full_64k = below_window_passthrough && top_of_window_overridden;

    expect(window_confined_to_e8 && addr_0x100_overridden && window_spans_full_64k,
           "sol:K-0742 Zorro II configuration space is the 64 KiB block at $00E8xxxx");
    // MUTATION sol:K-0742 none impossible: unfalsifiable at this boundary --
    //   $E7FFFE is outside the decoded window by construction
    //   (chip_addr[23:16] must equal exactly 8'b11101000, cpu_wrapper.v:
    //   611); no fastramcfg or autocfg_card state can make an address one
    //   byte below the window respond.

    StrobeProbe sp;
    note("Group5 word-cycle: a configuration read is one 16-bit chip-bus cycle -- " +
         std::to_string(sp.as_low_samples) + " AS-low sample(s) at $E8xxxx during the register-00 "
         "read, chip_uds high in " + std::to_string(sp.uds_high_samples) + ", chip_lds high in " +
         std::to_string(sp.lds_high_samples) + " (0/0 expected for a word cycle).");
    bool word_cycle_confirmed = sp.completed && sp.as_low_samples > 0 &&
                                 sp.uds_high_samples == 0 && sp.lds_high_samples == 0;
    expect(word_cycle_confirmed,
           "sol:K-0743 accesses to Zorro II configuration space use 16-bit Zorro II cycles");
    // MUTATION sol:K-0743 none impossible: unfalsifiable at this boundary --
    //   chip_dout/chip_din are declared [15:0] (cpu_wrapper.v ports list);
    //   the port is 16 bits wide by declaration and no stimulus this
    //   module accepts can vary it.

    // sol:K-0744's "every Zorro II card" is graded over this build's own
    // population: under ENABLE_RAM32, the Zorro II chain-extension path is
    // compiled out (cpu_wrapper.v:643-651), so at most one Zorro II card
    // (this fixture's own) exists here -- "every" ranges over that set.
    expect(window_confined_to_e8 && addr_0x100_overridden,
           "sol:K-0744 every Zorro II card configures in $00E8xxxx (and nowhere else)");
    expect(window_confined_to_e8 && addr_0x100_overridden && window_spans_full_64k && word_cycle_confirmed,
           "opus:K-0767 Zorro II configuration space is the 64K block $00E8xxxx, 16-bit cycles, all Zorro II cards configure there");
    // MUTATION opus:K-0767 none impossible: unfalsifiable at this boundary --
    //   the below-window-passthrough clause has no breaking stimulus; see
    //   sol:K-0742's own record. The 16-bit-cycles clause is likewise
    //   fixed by the port declaration; see sol:K-0743's own record.

    // =========================================================================
    // Group 6 -- one nybble only (sol:K-0750 graded; opus:K-0771 NOT
    // GRADED). Preset chip_dout's low 12 bits to a recognisable pattern at
    // $E80000; the top nybble is overridden with autocfg_data, the low 12
    // bits pass through unconditionally (cpu_wrapper.v:236,
    // `chip_data[11:0]` is used in BOTH arms of the ternary) --
    // unfalsifiable, since the expected value is computed from the preset
    // itself. The override half is graded on the manual-defined type-code
    // bits, not a specific constant, and broken by disabling
    // sel_autoconfig (fastramcfg=0). A Zorro III probe is also measured
    // (cfg_z3=1) for context, but opus:K-0771's own "bus mode" clause (p.431:
    // "A Zorro III PIC can configure in Zorro II or Zorro III configuration
    // space ... on D31-D28 for either bus mode") names the CONFIGURATION
    // SPACE and cycle type a board uses, not which board type it is --
    // NanoMig's Zorro III board still configures in the $E8xxxx Zorro II
    // configuration space over this design's one 16-bit chip bus (see
    // sol:K-0743's own StrobeProbe and opus:K-0777's own withdrawal: no
    // Zorro III configuration space is reachable at all), so both probes
    // below measure the SAME (Zorro II) bus mode and cannot decide "either".
    // =========================================================================
    const uint16_t nb_preset_value = 0x0ABC;
    std::map<uint32_t, uint16_t> nb_presets = { {0xE80000, nb_preset_value} };
#ifdef MUT_NYBBLE
    // MUTATION sol:K-0750 stimulus red: MUT=NYBBLE sets fastramcfg=0,
    //   disabling sel_autoconfig entirely (cpu_wrapper.v:611) -- the top
    //   nybble is no longer overridden.
    Harness nb_zii(0, 0, {READ(autocfg_addr(IDX_REG00))}, nb_presets);
    Harness nb_ziii(2, 0, {READ(autocfg_addr(IDX_REG00))}, nb_presets);
#else
    Harness nb_zii(0, 1, {READ(autocfg_addr(IDX_REG00))}, nb_presets);
    Harness nb_ziii(2, 4, {READ(autocfg_addr(IDX_REG00))}, nb_presets);
#endif
    uint16_t nb_word_zii = nb_zii.word[0];
    uint16_t nb_word_ziii = nb_ziii.word[0];
    uint16_t nb_expected_low12 = nb_preset_value & 0x0FFF;
    note("Group6 nybble-only: preset chip_dout=0x" + hex4(nb_preset_value) + " at $E80000; Zorro II "
         "captured full word=0x" + hex4(nb_word_zii) + "; Zorro III-board (still Zorro II bus mode, "
         "context for opus:K-0771's own record) captured full word=0x" + hex4(nb_word_ziii) +
         " (low 12 bits expected 0x" + hex4(nb_expected_low12).substr(1) +
         " in both, pass through unconditionally; top nibble's bits 3-2 expected '11' "
         "only when sel_autoconfig is enabled).");
    bool low12_pass_zii = (nb_word_zii & 0xFFF) == nb_expected_low12;
    bool override_zii = type_bits((uint8_t)(nb_word_zii >> 12), 0b11);
    bool only_one_nybble_zii = low12_pass_zii && override_zii;
    expect(only_one_nybble_zii,
           "sol:K-0750 a configuration read physically returns only one nybble of real content; the rest passes through unmodified");
    // MUTATION sol:K-0750 none impossible: unfalsifiable at this boundary --
    //   cpu_wrapper.v:236's mux uses chip_data[11:0] in BOTH arms of the
    //   ternary, so the low 12 bits pass through regardless of
    //   sel_autoconfig's value. The expected low 12 bits here are computed
    //   from the preset itself, so no stimulus change to the preset can
    //   break the comparison. The top-nybble-override half of this same
    //   assertion is exercised by MUT_NYBBLE above.
    note("p.438's pin table (physical pins 63/65/67/69, AD31/AD30/AD29/AD28) "
         "shows the SAME physical lines carry Zorro II's D15-D12 and Zorro "
         "III's D31-D28 (data phase) -- the top nybble of this design's "
         "16-bit chip bus (chip_din/chip_dout[15:0]) IS the page's "
         "'D31-D28' for either card, not a different lane numbering.");
    {
        const std::string bus_mode_reason =
            "reason_kind: absent (this fixture never reaches a genuine second bus mode: no Zorro "
            "III configuration space exists to be decoded at all -- see opus:K-0777's own withdrawal "
            "-- so the second of the two modes this claim names has no signal path here). Its own "
            "quote (p431) reads 'A Zorro III PIC can configure in Zorro II or Zorro III "
            "configuration space ... on D31-D28 for either bus mode' -- bus mode names the "
            "CONFIGURATION SPACE and cycle type a board configures in, not which board type answers. "
            "NanoMig's Zorro III board still configures in the $E8xxxx Zorro II configuration space "
            "over this design's one 16-bit chip bus (measured above: both the Zorro II and the "
            "Zorro III-board probes read the SAME bus mode), so 'either' cannot be decided. The "
            "single reachable mode is graded (p431, graded: sol:K-0750).";
        record("opus:K-0771 NOT GRADED (p431): 'A Zorro III PIC can configure in Zorro II or Zorro "
               "III configuration space ... on D31-D28 for either bus mode' " + bus_mode_reason);
    }

    // =========================================================================
    // Group 7 -- paired addressing (sol:K-0752, sol:K-0753, sol:K-0760,
    // opus:K-0773, opus:K-0779). Graded structurally rather than against a
    // hard-coded expected byte: register 00's TYPE address ($E80000) must
    // carry the SAME nibble whether fastramcfg is 1 or 2, while its SIZE
    // address ($E80002) must carry a DIFFERENT nibble (2 MiB vs 4 MiB,
    // cpu_wrapper.v:572-573) -- proving both addresses are genuinely,
    // independently read, rather than one supplying an assumed constant.
    // =========================================================================
#ifdef MUT_SWAPNYBBLE
    // MUTATION sol:K-0752 stimulus red: MUT=SWAPNYBBLE reads $E80002
    //   before $E80000 (reversing the CPU's OWN physical read order), so
    //   the slot the test treats as "the type address" actually captured
    //   the size address's content and vice versa.
    // MUTATION sol:K-0753 stimulus red: same switch as sol:K-0752.
    // MUTATION sol:K-0760 stimulus red: same switch as sol:K-0752.
    // MUTATION opus:K-0773 stimulus red: same switch as sol:K-0752.
    // MUTATION opus:K-0779 stimulus red: same switch as sol:K-0752.
    Harness reg00_f1(0, 1, {READ(autocfg_addr(IDX_REG00LO)), READ(autocfg_addr(IDX_REG00))});
    Harness reg00_f2(0, 2, {READ(autocfg_addr(IDX_REG00LO)), READ(autocfg_addr(IDX_REG00))});
    uint8_t hi_f1 = reg00_f1.nibble(0), lo_f1 = reg00_f1.nibble(1);
    uint8_t hi_f2 = reg00_f2.nibble(0), lo_f2 = reg00_f2.nibble(1);
#else
    Harness reg00_f1(0, 1, {READ(autocfg_addr(IDX_REG00)), READ(autocfg_addr(IDX_REG00LO))});
    Harness reg00_f2(0, 2, {READ(autocfg_addr(IDX_REG00)), READ(autocfg_addr(IDX_REG00LO))});
    uint8_t hi_f1 = reg00_f1.nibble(0), lo_f1 = reg00_f1.nibble(1);
    uint8_t hi_f2 = reg00_f2.nibble(0), lo_f2 = reg00_f2.nibble(1);
#endif
    note("Group7 paired addressing: fastramcfg=1 -> $E80000(hi)=0x" + hex1(hi_f1) + " $E80002(lo)=0x" + hex1(lo_f1) +
         "; fastramcfg=2 -> $E80000(hi)=0x" + hex1(hi_f2) + " $E80002(lo)=0x" + hex1(lo_f2) +
         " (as read -- under a swap mutation these are the OTHER address's content).");
    bool hi_address_carries_type = reg00_f1.all_captured && reg00_f2.all_captured &&
                                    (hi_f1 == hi_f2) && (hi_f1 != 0x0);
    bool lo_address_carries_size = (lo_f1 != lo_f2);
    bool paired_addressing_confirmed = hi_address_carries_type && lo_address_carries_size;
    expect(paired_addressing_confirmed, "sol:K-0752 every logical AUTOCONFIG register is eight bits wide");
    expect(paired_addressing_confirmed, "sol:K-0753 each logical eight-bit register is assembled from upper nybbles at two paired addresses");
    expect(paired_addressing_confirmed, "opus:K-0773 every AUTOCONFIG register is logically 8 bits wide, the 8 bits actually being nybbles from two paired addresses");
    expect(paired_addressing_confirmed, "sol:K-0760 in Zorro II-style mapping, the two nybbles of register 00 are read at $00E80000 and $00E80002");
    expect(paired_addressing_confirmed, "opus:K-0779 Figure K-14 shows the Zorro II style mapping taking the paired nybbles at $00E80000/$00E80002 to form register (00|02)'s 8 bits");

    // Register 3C -- a stable (never mutated) reading of both its
    // addresses, reused by Group 8's complement checks.
    Harness rw(0, 1, {READ(autocfg_addr(IDX_REG3C)), READ(autocfg_addr(IDX_REG3CLO))});
    uint8_t reg3c_hi_raw = rw.nibble(0), reg3c_lo_raw = rw.nibble(1);
    note("Group7 register 3C: raw nybbles at $E8003C/$E8003E = 0x" + hex1(reg3c_hi_raw) + hex1(reg3c_lo_raw) +
         " (undriven default; see sol:K-0757's NOT GRADED record below -- every undriven register in this "
         "design reads this same value, so no stimulus can establish that THIS address specifically is "
         "register 3C's location).");
    {
        const std::string reg3c_reason =
            "reason_kind: insufficient (the port is not missing -- $E8003C/$E8003E is readable -- "
            "but its own evidence, a raw value every other undriven address in the window also "
            "produces, is not enough to establish that THIS address is register 3C's location). "
            "Register 3C's logical value is 0, and this design's complement rule "
            "(cpu_wrapper.v:547's default 4'b1111) makes EVERY undriven read register -- at "
            "$E8003C/$E8003E and at any other reserved index -- read the identical raw 0xF/0xF, "
            "logical 0/0. No stimulus in this module's input space can distinguish $E8003C/$E8003E "
            "from $E8007C/$E8007E or $E8000C/$E8000E by value, so this specific address cannot be "
            "established as register 3C's location at this boundary, even though the manual's own "
            "worked example (p.432) names exactly this pair and this value.";
        record("sol:K-0757 NOT GRADED (p432): 'upper nybbles of locations "
               "$00E8003C and $00E8003E' " + reg3c_reason);
    }

    // =========================================================================
    // Group 8 -- complement rule (sol:K-0755(p432), sol:K-0756, sol:K-0759,
    // opus:K-0776). Register 3C, plus a sweep of the manual's own
    // "Reserved, must be 0" registers -- $0C (p.434), $30, $34, $38, $40
    // (p.435), not $04 (product number, "completely up to the
    // manufacturer") or $08 (er_Flags, board-specific choices), whose
    // logical values the manual does not fix: all read raw `1111`,
    // complementing to logical 0, evidence for "EVERY read register
    // except 00" rather than register 3C alone. Register 00 is the sole
    // exception, graded on the manual-defined type-code bit pattern (bits
    // 3-2 == '11' in this Zorro II context) rather than "not all ones",
    // since a complemented register could fail to read all-ones by
    // coincidence without actually being direct. All three conjuncts
    // carry their own stimulus mutation.
    // =========================================================================
#ifdef MUT_NOCOMPLEMENT
    // MUTATION sol:K-0755 stimulus red: MUT=NOCOMPLEMENT reads register
    //   00's own addresses ($E80000/$E80002, raw 0xE/0x6) into the slots
    //   the complement check treats as "register 3C", isolated in a
    //   dedicated harness so Group 7's own checks (which use `rw`) are
    //   untouched. Applying the SAME (correct) complement step to those
    //   raw values yields 0x1/0x9, not 0x0/0x0, so "logical value is zero"
    //   goes red.
    // MUTATION sol:K-0756 stimulus red: same switch as sol:K-0755.
    // MUTATION sol:K-0759 stimulus red: same switch as sol:K-0755.
    // MUTATION opus:K-0776 stimulus red: same switch as sol:K-0755.
    Harness cpl(0, 1, {READ(autocfg_addr(IDX_REG00)), READ(autocfg_addr(IDX_REG00LO))});
    uint8_t cpl_hi_raw = cpl.nibble(0), cpl_lo_raw = cpl.nibble(1);
#else
    uint8_t cpl_hi_raw = reg3c_hi_raw, cpl_lo_raw = reg3c_lo_raw;
#endif
    uint8_t reg3c_hi_logical = logical(IDX_REG3C, cpl_hi_raw);
    uint8_t reg3c_lo_logical = logical(IDX_REG3CLO, cpl_lo_raw);
    bool reg3c_is_zero = (reg3c_hi_logical == 0x0) && (reg3c_lo_logical == 0x0);
    bool reg3c_raw_all_ones = (cpl_hi_raw == 0xF) && (cpl_lo_raw == 0xF);

    // Sweep of the manual's "Reserved, must be 0" registers: byte $0C
    // (word indices 6,7), $30 (24,25), $34 (26,27), $38 (28,29), $40
    // (32,33).
    const int reserved_idx[10] = {6,7, 24,25, 26,27, 28,29, 32,33};
#ifdef MUT_SWEEPWRONGREG
    // MUTATION sol:K-0755 stimulus red: MUT=SWEEPWRONGREG redirects the
    //   sweep to read register 00's own two addresses five times over
    //   (raw 0xE/0x6, not 0xF) instead of the manual's "Reserved, must be
    //   0" registers.
    // MUTATION opus:K-0776 stimulus red: same switch as sol:K-0755.
    Harness reserved_sweep(0, 1, {READ(autocfg_addr(IDX_REG00)), READ(autocfg_addr(IDX_REG00LO)),
                                   READ(autocfg_addr(IDX_REG00)), READ(autocfg_addr(IDX_REG00LO)),
                                   READ(autocfg_addr(IDX_REG00)), READ(autocfg_addr(IDX_REG00LO)),
                                   READ(autocfg_addr(IDX_REG00)), READ(autocfg_addr(IDX_REG00LO)),
                                   READ(autocfg_addr(IDX_REG00)), READ(autocfg_addr(IDX_REG00LO))});
#else
    Harness reserved_sweep(0, 1, {READ(autocfg_addr(reserved_idx[0])), READ(autocfg_addr(reserved_idx[1])),
                                   READ(autocfg_addr(reserved_idx[2])), READ(autocfg_addr(reserved_idx[3])),
                                   READ(autocfg_addr(reserved_idx[4])), READ(autocfg_addr(reserved_idx[5])),
                                   READ(autocfg_addr(reserved_idx[6])), READ(autocfg_addr(reserved_idx[7])),
                                   READ(autocfg_addr(reserved_idx[8])), READ(autocfg_addr(reserved_idx[9]))});
#endif
    bool reserved_all_zero = reserved_sweep.all_captured;
    std::string reserved_summary;
    for (int i = 0; i < 10; i++) {
        uint8_t raw = reserved_sweep.nibble((size_t)i);
        uint8_t lg = logical(reserved_idx[i], raw);
        if (raw != 0xF || lg != 0x0) reserved_all_zero = false;
        reserved_summary += " 0x" + hex1(raw);
    }
    note("Group8 reserved-register sweep (bytes $0C, $30, $34, $38, $40, the manual's own "
         "\"Reserved, must be 0\" registers, pp.434-435): raw nybbles =" +
         reserved_summary + " (0xF logical 0 expected at each, same complement rule as register 3C).");

    // register 00's own type-code bits (3-2 of the hi nibble) read DIRECT,
    // never complemented -- the manual-defined Zorro-II code is '11'. A
    // dedicated, independently-mutated harness, not Group2's `handoff` or
    // Group7's `reg00_f1`/`reg00_f2`.
#ifdef MUT_REG00SILENT
    // MUTATION sol:K-0755 stimulus red: MUT=REG00SILENT sets fastramcfg=0,
    //   disabling sel_autoconfig entirely (cpu_wrapper.v:611) -- register
    //   00 no longer responds at all (raw 0x0), so its type-code bits are
    //   no longer '11'.
    // MUTATION opus:K-0776 stimulus red: same switch as sol:K-0755.
    Harness reg00type(0, 0, {READ(autocfg_addr(IDX_REG00))});
#else
    Harness reg00type(0, 1, {READ(autocfg_addr(IDX_REG00))});
#endif
    uint8_t reg00_hi_stable = reg00type.nibble(0);
    bool reg00_type_direct = type_bits(reg00_hi_stable, 0b11);
    note("Group8 complement: register3C raw nybbles=0x" + hex1(cpl_hi_raw) + hex1(cpl_lo_raw) +
         " logical=0x" + hex1(reg3c_hi_logical) + hex1(reg3c_lo_logical) +
         "; register00 type-code bits[3:2]=0b" + std::to_string((reg00_hi_stable>>3)&1) + std::to_string((reg00_hi_stable>>2)&1) +
         " (11 expected, uncomplemented -- the sole exception).");
    bool complement_rule_confirmed = reg3c_is_zero && reserved_all_zero && reg00_type_direct;
    expect(complement_rule_confirmed, "sol:K-0755 every AUTOCONFIG read register except register 00 is physically complemented");
    expect(complement_rule_confirmed, "opus:K-0776 in the actual implementation all read registers except the 00 register are physically complemented");
    expect(reg3c_is_zero, "sol:K-0756 logical register 3C has the value zero");
    expect(reg3c_raw_all_ones && reg3c_is_zero, "sol:K-0759 because read registers are complemented, both physical nybbles for logical zero return all 1 bits");
    {
        const std::string zorro3_addr_reason =
            "reason_kind: absent (the Zorro III address decode this claim names has no signal path "
            "in this fixture at all -- chip_addr is declared [23:1], cpu_wrapper.v:34/60, a 24-bit "
            "byte address space; $FF00003C and $FF00013C need address bits beyond 23 that this port "
            "cannot carry, whatever fastramcfg or cpucfg state selects). Its own quote names "
            "$FF00003C and $FF00013C -- Zorro III's own addressing form for register 3C, using the "
            "physical connector's full address width, not the 64 KiB configuration window this "
            "fixture's chip bus decodes. The Zorro-II addressing and value are graded (p432, graded: "
            "sol:K-0756, sol:K-0759).";
        record("opus:K-0777 NOT GRADED (p432): 'or $FF00003C and "
               "$FF00013C, both return all Is' " + zorro3_addr_reason);
    }

    // =========================================================================
    // Liveness -- carries no claim uid.
    // =========================================================================
    expect(lc_before.all_captured && rw.all_captured && nb_zii.all_captured &&
           before_raw != 0x0 && reg3c_hi_raw != reg00_hi_stable,
           "fixture-liveness [not graded -- apparatus check, no claim uid]");

    std::cout << "\n" << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
