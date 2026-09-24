// Test purpose: appendix K's AUTOCONFIG mechanism at the `cpu_wrapper`
//   boundary -- printed pp.431, 432, 434, 435 and 438, the "THE AUTOCONFIG
//   MECHANISM" prose, the "Configuration Register Mapping" figure and the
//   pin table -- measured in the build without ENABLE_RAM32 (every board
//   target but the four 32 MiB ones, the Tang Nano 20K among them): the
//   unconfigured/configured lifecycle, the single-responder handoff (a
//   chain of up to three Zorro II boards, since this build has no Zorro
//   III board at all), the read/write register split, the $00E8xxxx
//   address window, the one-nybble-per-read rule, the paired-address
//   register width and the complement rule.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 431, 432, 435, 438
// Non-goals: anything outside the `cpu_wrapper` fixture this package
//   builds, and any claim it does not cite. A green here means those
//   assertions held on this fixture, in this build configuration -- not
//   that the module is correct, and not that the chapter is covered. This
//   build hard-wires cfg_z3 to 0 (cpu_wrapper.v:539): no Zorro III board
//   exists here, so no assertion in this file exercises one.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/appK-build/appK-autoconfig-space.jsonl` in the sibling repository
//   (hrm-testpipeline), the same 30-claim file hrm/appK-autoconfig-space
//   cites. docs/legacy-names.md explains the vocabulary.
//
// appK-autoconfig-card-chain -- pp.431,432,434,435,438 Appendix K "THE
// AUTOCONFIG MECHANISM" prose, the "Configuration Register Mapping"
// figure and the pin table, measured on the build without ENABLE_RAM32.
// 30 claims, filtered from
// hrm-testpipeline/work/appK-build/appK-autoconfig-space.jsonl. 25 are
// graded; sol:K-0737, opus:K-0764, opus:K-0771, opus:K-0777 and sol:K-0757
// are NOT GRADED (each bundles a clause, or names a specific address, this
// configuration cannot decide -- see the NOT GRADED records at each one's
// grading point and SPECIFICATION.md).
//
// See SPECIFICATION.md for the full boundary investigation and the group
// breakdown. Fixture mechanism (Harness, core selection, active-low reset,
// direct/complement arithmetic, the register-48-write capability) reused
// from hrm/appK-autoconfig-space.
//
// Mutation mechanism, plain summary (the individual, parseable
// `// MUTATION <label> <kind> <result>` records live next to each
// assertion below, one label per line). Thirteen `-DMUT_<NAME>=1` build
// flags exist: LIFECYCLE, NORESET, NOHANDOFF, WRITETOWRITEREG,
// REG48NOEFFECT, INSIDEPROBE, NOOVERRIDE, CHAINONEBOARD, NYBBLE,
// SWAPNYBBLE, NOCOMPLEMENT, SWEEPWRONGREG, REG00SILENT. Some assertions
// carry a second record of kind `none impossible`: a part of their claim
// is a structural fact (a fixed port width, a fixed address decode, or a
// specific undriven address indistinguishable by value from any other
// undriven address) that no stimulus in this module's input space can
// falsify, or a specific corroborating signal that cannot observe past
// its first instance -- both are reported rather than hidden behind a
// green mutation.

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
static bool type_bits_11(uint8_t nibble) { return ((nibble >> 2) & 0x3) == 0b11; }

static uint32_t autocfg_addr(int idx) { return 0xE80000u + 2u * (uint32_t)idx; }

// General-purpose Harness -- the same mechanism as hrm/appK-autoconfig-space
// and hrm/appK-autoconfig-size-encoding's own: a strictly-ordered program of
// READ (capture to a fresh scratch slot) and WRITE_IMM operations, executed
// through cpu_wrapper's own ports only, with absolute addresses and a
// memory-preset list. Extended here with an optional cycle budget (so a
// probe that never completes, such as an attempted real-RAM access with no
// dtack, does not force every harness in the file to spend the full default
// budget) and with `ramreq_seen`, a passive sample of the `ramreq` output
// port (cpu_wrapper.v:81, `assign ramreq = ramsel & ram_dtack;`,
// cpu_wrapper.v:168) -- an existing module port, not a debug port added for
// this package.
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

// Dedicated probe for the "...or RESETS..." half of sol:K-0727/opus:K-0755:
// configure ALL THREE boards of the chain (fastramcfg=3), confirm the space
// falls silent, pulse `reset` mid-run (not the power-up reset every Harness
// above already performs), then WALK THE CHAIN AGAIN: read register 00,
// and twice write register 48 and read register 00 -- requiring all three
// boards to answer again post-reset, so "every card" (opus:K-0755) is
// observed re-arming rather than inferred from board 1's own return. The very first post-reset reading is also compared
// against register 00's own FULL BYTE (both nybbles, measured, not an
// assumed constant) from before any write. Built by hand rather than
// through Harness because it needs a capability Harness does not have:
// driving `reset` low a second time after some of the program has already
// run, then continuing the SAME simulation.
struct ResetProbe {
    bool before_responds = false;
    bool after_config_silent = false;
    bool after_reset_responds = false;
    bool after_reset_point1_responds = false;
    bool after_reset_point2_responds = false;
    bool byte_matches_before = false;
    uint8_t before_hi = 0, before_lo = 0, after_config_hi = 0;
    uint8_t after_reset_hi = 0, after_reset_lo = 0, after_reset_point1_hi = 0, after_reset_point2_hi = 0;
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
        emit_read(pc, 0xE80000, 0x2000);   // before: register 00 hi
        emit_read(pc, 0xE80002, 0x2002);   // before: register 00 lo
        emit_write(pc, 0xE80048, 0x0000);  // configure board 1 -> board 2
        emit_write(pc, 0xE80048, 0x0000);  // configure board 2 -> board 3
        emit_write(pc, 0xE80048, 0x0000);  // configure board 3 -> silent
        emit_read(pc, 0xE80000, 0x2004);   // after configuring all three, before reset: expect silence
        uint32_t orig_end_pc = pc;         // where an UN-reset CPU is stuck looping
        mem[pc] = 0x60FE;

        top->reset = 0; top->clk = 0; top->ph1 = 0; top->ph2 = 0;
        top->cpucfg = 0; top->fastramcfg = 3; top->chipramcfg = 0; top->slowramcfg = 0;
        top->turbocfg = 0; top->bootrom = 0; top->chip_dout = 0; top->chip_dtack = 0;
        top->chip_ipl = 7; top->ramdout = 0; top->ramready = 0;
        top->eval();

        std::map<uint32_t, uint16_t> captured;
        bool last_as = true, pulsed = false;
        const int RESET_CYCLES = 20, MAX_CYC = 14000;
        for (int cyc = 0; cyc < MAX_CYC && !captured.count(0x2010); cyc++) {
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
                        // The whole chain is configured and silent. Splice
                        // a "walk the chain again" program at wherever the
                        // CPU will next fetch from: $1000, if a reset sends
                        // it there, or its OWN current self-loop location,
                        // if not -- so the no-pulse case reads what the
                        // un-reset CPU actually executes next, not an
                        // unreachable splice at a vector it never returns
                        // to.
                        uint32_t ppc = skip_pulse ? orig_end_pc : 0x1000u;
                        emit_read(ppc, 0xE80000, 0x2006);   // post-reset point 0 hi
                        emit_read(ppc, 0xE80002, 0x2008);   // post-reset point 0 lo
                        emit_write(ppc, 0xE80048, 0x0000);  // post-reset write: re-configure board 1
                        emit_read(ppc, 0xE80000, 0x200A);   // post-reset point 1 hi
                        emit_read(ppc, 0xE80002, 0x200C);   // post-reset point 1 lo
                        emit_write(ppc, 0xE80048, 0x0000);  // post-reset write: re-configure board 2
                        emit_read(ppc, 0xE80000, 0x200E);   // post-reset point 2 hi
                        emit_read(ppc, 0xE80002, 0x2010);   // post-reset point 2 lo
                        mem[ppc] = 0x60FE;
                        // MUTATION sol:K-0727 stimulus red: MUT=NORESET
                        //   skips driving `reset` low again. The splice
                        //   above lands where the CPU actually fetches from
                        //   next either way, so without the pulse it reads
                        //   the chain exactly as the three configuring
                        //   writes left it -- silent at all three post-splice
                        //   points, and the two post-splice writes (already
                        //   configured, sel_autoconfig off) have no effect
                        //   either -- instead of the chain responding again.
                        // MUTATION opus:K-0755 stimulus red: same switch
                        //   as sol:K-0727 (MUT=NORESET).
                        if (!skip_pulse) {
                            top->reset = 0;
                            for (int r = 0; r < RESET_CYCLES; r++) { top->clk = 0; top->eval(); top->clk = 1; top->eval(); }
                            top->reset = 1;
                        }
                        pulsed = true;
                    } else if (pulsed && (a == 0x2006 || a == 0x2008 || a == 0x200A || a == 0x200C ||
                                          a == 0x200E || a == 0x2010) && !captured.count(a)) {
                        captured[a] = top->chip_din;
                    }
                }
                last_as = as_now;
            }
        }
        completed = captured.count(0x2000) && captured.count(0x2002) && captured.count(0x2004) &&
                    captured.count(0x2006) && captured.count(0x2008) &&
                    captured.count(0x200A) && captured.count(0x200C) &&
                    captured.count(0x200E) && captured.count(0x2010);
        before_hi             = (uint8_t)((captured.count(0x2000) ? captured[0x2000] : 0) >> 12 & 0xF);
        before_lo              = (uint8_t)((captured.count(0x2002) ? captured[0x2002] : 0) >> 12 & 0xF);
        after_config_hi        = (uint8_t)((captured.count(0x2004) ? captured[0x2004] : 0) >> 12 & 0xF);
        after_reset_hi          = (uint8_t)((captured.count(0x2006) ? captured[0x2006] : 0) >> 12 & 0xF);
        after_reset_lo          = (uint8_t)((captured.count(0x2008) ? captured[0x2008] : 0) >> 12 & 0xF);
        after_reset_point1_hi = (uint8_t)((captured.count(0x200A) ? captured[0x200A] : 0) >> 12 & 0xF);
        after_reset_point2_hi = (uint8_t)((captured.count(0x200E) ? captured[0x200E] : 0) >> 12 & 0xF);
        before_responds             = before_hi != 0x0;
        after_config_silent         = after_config_hi == 0x0;
        after_reset_responds        = completed && (after_reset_hi != 0x0);
        after_reset_point1_responds = completed && (after_reset_point1_hi != 0x0);
        after_reset_point2_responds = completed && (after_reset_point2_hi != 0x0);
        byte_matches_before  = completed && (before_hi == after_reset_hi) && (before_lo == after_reset_lo);
        top->final(); delete top;
    }
};

// Dedicated probe for sol:K-0743/opus:K-0767's "16-bit Zorro II cycles":
// one MOVE.W read of register 00 ($E80000), sampling chip_uds and chip_lds
// (module output ports, cpu_wrapper.v ports list) at every cycle chip_as
// is asserted AND the address on chip_addr is inside $E8xxxx -- excluding
// the reset vector fetch, the instruction fetches, and the write-back to
// scratch, none of which are the configuration-space access this claim is
// about. A word-wide access asserts both strobes together for the whole
// cycle.
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

int main() {
    const int IDX_REG00 = 0, IDX_REG00LO = 1, IDX_REG3C = 30, IDX_REG3CLO = 31;
    const int IDX_REG48 = 36;

    // =========================================================================
    // Group 1 -- lifecycle. Read register 00 before any write (expect the
    // real type nibble -> board responds while unconfigured); write
    // register 48 (completes Zorro II configuration, cpu_wrapper.v:640-642);
    // read register 00 again (expect silence -> board removed itself from
    // $E8xxxx). fastramcfg=1: fastramcfg_d[1:0]==2'b01 never matches the
    // chain-extension guard at cpu_wrapper.v:644, so this is a plain
    // single-board configuration.
    //
    // sol:K-0727 and opus:K-0755 also name RESET, not only power-up
    // ("When an Amiga powers up OR RESETS, every card..."). ResetProbe
    // configures all three boards of the chain, confirms silence, pulses
    // `reset` again mid-run, and confirms register 00's own full byte
    // (both nybbles, measured against what was read before any write, not
    // an assumed constant) matches again -- proving a mid-operation reset
    // re-arms the whole chain, exercising "every card" on more than one
    // board.
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
         " -- nonzero means sel_autoconfig asserted (board responds while unconfigured).");

#ifdef MUT_LIFECYCLE
    // MUTATION sol:K-0739 stimulus red: MUT=LIFECYCLE skips the
    //   register-48 write on the "after" harness, so the post-configuration
    //   read is compared against a board that was never actually
    //   configured and is still in its unconfigured (responding) state.
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
         " -- zero means sel_autoconfig deasserted (board silent after configuring).");

#ifdef MUT_NORESET
    ResetProbe rp(true);
#else
    ResetProbe rp(false);
#endif
    note("Group1 reset-mid-run (three-board chain): before=0x" + hex1(rp.before_hi) + hex1(rp.before_lo) +
         " after-config(pre-reset)=0x" + hex1(rp.after_config_hi) +
         " after-reset point0=0x" + hex1(rp.after_reset_hi) + hex1(rp.after_reset_lo) +
         " after-reset point1 (post-reset write, walking the chain again)=0x" + hex1(rp.after_reset_point1_hi) +
         " after-reset point2=0x" + hex1(rp.after_reset_point2_hi) +
         " (point0's byte compared against before's byte for equality, not against an assumed constant; "
         "points 1 and 2 responding too means all three boards re-armed, not only board 1).");
    bool reset_rearms_chain = rp.completed && rp.before_responds &&
                               rp.after_config_silent && rp.after_reset_responds &&
                               rp.after_reset_point1_responds && rp.after_reset_point2_responds &&
                               rp.byte_matches_before;

    expect(responds_when_unconfigured && reset_rearms_chain,
           "sol:K-0727 power-up or reset places the board in its unconfigured (still-responding) state");
    expect(responds_when_unconfigured && reset_rearms_chain,
           "opus:K-0755 power-up or reset places every card in its unconfigured (still-responding) state");
    expect(responds_when_unconfigured, "sol:K-0734 an enabled unconfigured board responds in configuration space");
    expect(disappears_after_configured, "sol:K-0739 writing the final base-address portion completes configuration (space falls silent)");
    expect(disappears_after_configured, "sol:K-0842 a board removes its configuration registers after configuration");
    expect(disappears_after_configured, "sol:K-0843 a configured board stops responding in $00E8xxxx");
    expect(disappears_after_configured, "opus:K-0879 configuration registers cannot remain active after configuration -- they disappear");
    expect(disappears_after_configured, "opus:K-0880 configuration registers move out of the $00E8xxxx space once a board is configured");

    // =========================================================================
    // Group 2 -- handoff, the central claim (sol:K-0733). This build has no
    // Zorro III board (cfg_z3 hard-wired 0, cpu_wrapper.v:539). With
    // fastramcfg[1:0] == 2'b11 it instead chains up to three Zorro II
    // boards: cpu_wrapper.v:643-651 (compiled only when ENABLE_RAM32 is
    // undefined) re-arms autocfg_card (2'b01 -> 2'b10 -> 2'b11) on each
    // register-48 write before the unconditional autocfg_card <= 2'b00 at
    // cpu_wrapper.v:642 finally sticks.
    //
    // The claim is graded on the FIRST hand-off: after the configuring
    // write to register 48, (a) the `ramreq` output port (cpu_wrapper.v:
    // 81/168) is asserted for an access into board 1's own Zorro II RAM
    // range ($200000, cpu_wrapper.v:117's sel_z2ram), proving board 1 is
    // now configured and has left configuration space for real memory, and
    // (b) register 00 still answers, proving a still-unconfigured board
    // remains visible in $00E8xxxx. Neither probe's own read/write
    // completes (ramready is held low throughout this fixture, so DTACK
    // never arrives) -- only whether `ramreq` was ever asserted is
    // measured, and a trailing unreachable read keeps each probe's
    // simulation running long enough to observe it.
    // =========================================================================
#ifdef MUT_NOHANDOFF
    // MUTATION sol:K-0733 stimulus red: MUT=NOHANDOFF diverts the FIRST
    //   register-48 write to register 00 (a read-only register with no
    //   configuring effect, cpu_wrapper.v:640/653 -- neither case matches
    //   chip_addr[6:1]==0). autocfg_card does not advance and z2ram_ena
    //   never latches (cpu_wrapper.v:641 never fires), so `ramreq` never
    //   asserts for board 1's range -- the assertion goes red even though
    //   register 00 still (trivially) answers, since board 1 never left.
    Harness handoff_reg00(0, 3, {
        READ(autocfg_addr(IDX_REG00)), READ(autocfg_addr(IDX_REG00LO)),
        WRITE_IMM(autocfg_addr(IDX_REG00), 0x0000),
        READ(autocfg_addr(IDX_REG00)), READ(autocfg_addr(IDX_REG00LO)),
    });
    Harness handoff_ramreq_before(0, 3, {WRITE_IMM(0x200000, 0x0000), READ(autocfg_addr(IDX_REG00))}, {}, 3000);
    Harness handoff_ramreq_after(0, 3, {
        WRITE_IMM(autocfg_addr(IDX_REG00), 0x0000),
        WRITE_IMM(0x200000, 0x0000),
        READ(autocfg_addr(IDX_REG00))}, {}, 3000);
#else
    Harness handoff_reg00(0, 3, {
        READ(autocfg_addr(IDX_REG00)), READ(autocfg_addr(IDX_REG00LO)),
        WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),
        READ(autocfg_addr(IDX_REG00)), READ(autocfg_addr(IDX_REG00LO)),
    });
    Harness handoff_ramreq_before(0, 3, {WRITE_IMM(0x200000, 0x0000), READ(autocfg_addr(IDX_REG00))}, {}, 3000);
    Harness handoff_ramreq_after(0, 3, {
        WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),
        WRITE_IMM(0x200000, 0x0000),
        READ(autocfg_addr(IDX_REG00))}, {}, 3000);
#endif
    uint8_t handoff_before_hi = handoff_reg00.nibble(0), handoff_before_lo = handoff_reg00.nibble(1);
    uint8_t handoff_after_hi  = handoff_reg00.nibble(2), handoff_after_lo  = handoff_reg00.nibble(3);
    bool handoff_space_still_answers = handoff_reg00.all_captured && (handoff_after_hi != 0x0);
    bool handoff_ramreq_silent_before = !handoff_ramreq_before.ramreq_seen;
    bool handoff_ramreq_active_after  = handoff_ramreq_after.ramreq_seen;
    note("Group2 handoff (first write): reg00 before=0x" + hex1(handoff_before_hi) + hex1(handoff_before_lo) +
         " after=0x" + hex1(handoff_after_hi) + hex1(handoff_after_lo) +
         "; ramreq before=" + std::string(handoff_ramreq_before.ramreq_seen ? "yes" : "no") +
         " after=" + std::string(handoff_ramreq_after.ramreq_seen ? "yes" : "no") +
         " -- \"board 1 has left configuration space\" is INFERRED from the lifecycle claims "
         "(Group1, disappears_after_configured, measured at fastramcfg=1), not independently "
         "re-observed here: this check cannot by itself distinguish \"a different, still-unconfigured "
         "board now answers\" from \"board 1 is technically configured yet happens to still answer\" -- "
         "`ramreq` establishes board 1 is live real memory, and Group1 establishes a configured board "
         "stops answering in $E8xxxx; together, not separately, they support \"hands off\".");
    bool handoff_ok = handoff_reg00.all_captured && handoff_space_still_answers &&
                      handoff_ramreq_silent_before && handoff_ramreq_active_after;
    expect(handoff_ok, "sol:K-0733 the configuration chain hands off to exactly one still-unconfigured responder at a time");
    // MUTATION sol:K-0733 none impossible: unfalsifiable at this boundary --
    //   autocfg_data is one 4-bit register (cpu_wrapper.v:230) driven by
    //   one case statement (cpu_wrapper.v:546-608) selected by one two-bit
    //   autocfg_card register (cpu_wrapper.v:229) and gated onto the chip
    //   bus by one wire, sel_autoconfig (cpu_wrapper.v:611). No stimulus in
    //   cpu_wrapper's own input space can make it drive two boards' worth
    //   of read data at once; "exactly one at a time" is a property of
    //   this design's structure, not something a mutation demonstrates.
    // MUTATION sol:K-0733 none impossible: unfalsifiable at this boundary --
    //   the second hand-off step (board 2 to board 3) cannot be
    //   corroborated by `ramreq` specifically: z2ram_ena
    //   (cpu_wrapper.v:117/641) is a single sticky enable covering the
    //   whole $200000-$9FFFFF Zorro II RAM range, latched by the FIRST
    //   configuring write and never cleared by any stimulus this module
    //   accepts, so no input can make a `ramreq` probe after the SECOND
    //   write distinguish "board 2 just configured" from "board 1 is
    //   still the only one live" -- an observability limit of this one
    //   corroborating signal, not a choice of what to test.

    // =========================================================================
    // Same read-register-00 apparatus, run across the WHOLE four-write
    // chain (fastramcfg=3) to measure how many points respond before the
    // space falls silent, and to confirm an address outside the window
    // stays unaffected WHILE each board is actually the responder -- read
    // before write 1 (board 1 responding), between writes 1 and 2 (board
    // 2), and between writes 2 and 3 (board 3), not after the chain has
    // already gone silent. Used by sol:K-0744/opus:K-0767 below, and
    // reused for K-0737/K-0764's own NOT GRADED record. Sizes are reported
    // as MEASURED lines only -- see K-0737's record for why they are not
    // graded against.
    // =========================================================================
#ifdef MUT_CHAINONEBOARD
    // MUTATION sol:K-0744 stimulus red: MUT=CHAINONEBOARD sets fastramcfg=1
    //   instead of 3 -- fastramcfg_d[1:0] never matches 2'b11
    //   (cpu_wrapper.v:644), so the first register-48 write silences the
    //   space immediately: one responding point instead of the three
    //   boards this configuration presents.
    // MUTATION opus:K-0767 stimulus red: same switch as sol:K-0744.
    Harness chain(0, 1, {
        READ(autocfg_addr(IDX_REG00)),   READ(autocfg_addr(IDX_REG00LO)),   // point 0: before any write
        READ(0xE90000),                                                      // outside probe while board 1 is the responder
        WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),                          // write 1
        READ(autocfg_addr(IDX_REG00)),   READ(autocfg_addr(IDX_REG00LO)),   // point 1
        READ(0xE90000),                                                      // outside probe while board 2 is the responder
        WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),                          // write 2
        READ(autocfg_addr(IDX_REG00)),   READ(autocfg_addr(IDX_REG00LO)),   // point 2
        READ(0xE90000),                                                      // outside probe while board 3 is the responder
        WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),                          // write 3
        READ(autocfg_addr(IDX_REG00)),   READ(autocfg_addr(IDX_REG00LO)),   // point 3
        WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),                          // write 4: extra
        READ(autocfg_addr(IDX_REG00)),   READ(autocfg_addr(IDX_REG00LO)),   // point 4
    }, {{0xE90000, 0x5678}});
#else
    Harness chain(0, 3, {
        READ(autocfg_addr(IDX_REG00)),   READ(autocfg_addr(IDX_REG00LO)),   // point 0: before any write
        READ(0xE90000),                                                      // outside probe while board 1 is the responder
        WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),                          // write 1
        READ(autocfg_addr(IDX_REG00)),   READ(autocfg_addr(IDX_REG00LO)),   // point 1
        READ(0xE90000),                                                      // outside probe while board 2 is the responder
        WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),                          // write 2
        READ(autocfg_addr(IDX_REG00)),   READ(autocfg_addr(IDX_REG00LO)),   // point 2
        READ(0xE90000),                                                      // outside probe while board 3 is the responder
        WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),                          // write 3
        READ(autocfg_addr(IDX_REG00)),   READ(autocfg_addr(IDX_REG00LO)),   // point 3
        WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),                          // write 4: extra
        READ(autocfg_addr(IDX_REG00)),   READ(autocfg_addr(IDX_REG00LO)),   // point 4
    }, {{0xE90000, 0x5678}});
#endif
    const int NPTS = 5;
    const int hi_idx[NPTS] = {0, 3, 6, 9, 11};
    const int lo_idx[NPTS] = {1, 4, 7, 10, 12};
    bool chain_responds[NPTS]; uint8_t chain_hi[NPTS], chain_lo[NPTS];
    for (int p = 0; p < NPTS; p++) {
        chain_hi[p] = chain.nibble((size_t)hi_idx[p]);
        chain_lo[p] = chain.nibble((size_t)lo_idx[p]);
        chain_responds[p] = chain.all_captured && (chain_hi[p] != 0x0);
    }
    int silent_at = -1;
    for (int p = 0; p < NPTS; p++) if (!chain_responds[p]) { silent_at = p; break; }
    bool falls_silent = (silent_at != -1);
    // Stimulus precondition, stated once: fastramcfg=3 presents three
    // Zorro II boards in this build (cpu_wrapper.v:643-651). "Every card"
    // therefore means all three answer before the space falls silent.
    bool every_board_answers = falls_silent && (silent_at == 3);
    bool stays_silent_after = true;
    for (int p = (falls_silent ? silent_at : NPTS); p < NPTS; p++) if (chain_responds[p]) stays_silent_after = false;
    uint8_t chain_outside_1 = chain.nibble(2), chain_outside_2 = chain.nibble(5), chain_outside_3 = chain.nibble(8);
    bool chain_outside_confined = (chain_outside_1 == 0x5) && (chain_outside_2 == 0x5) && (chain_outside_3 == 0x5);
    {
        std::string s = "Group2 chain: reg00 hi/lo at points 0..4 (before, after each of 4 register-48 writes) =";
        for (int p = 0; p < NPTS; p++) s += " 0x" + hex1(chain_hi[p]) + hex1(chain_lo[p]);
        s += "; outside-window probes while board 1/2/3 respond = 0x" + hex1(chain_outside_1) +
             " 0x" + hex1(chain_outside_2) + " 0x" + hex1(chain_outside_3) +
             " -- measured chain length (writes needed to reach silence) = " +
             (falls_silent ? std::to_string(silent_at) : std::string("never within 4 writes")) + ".";
        note(s);
    }

    // =========================================================================
    // Control -- same read-register-00-after-each-write apparatus as
    // Group 2, run with fastramcfg values the RTL trace says should NOT
    // produce a multi-board chain: fastramcfg=0 (no board at all) and
    // fastramcfg=2 (fastramcfg_d[1:0]==2'b10, still not 2'b11, so the
    // chain-extension guard at cpu_wrapper.v:644 does not match and the
    // chain measures as exactly ONE board). Not a claim; a basis check for
    // Group 2's apparatus.
    // =========================================================================
    Harness ctrl_none(0, 0, {READ(autocfg_addr(IDX_REG00))});
    Harness ctrl_one(0, 2, {READ(autocfg_addr(IDX_REG00)),
                             WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),
                             READ(autocfg_addr(IDX_REG00))});
    bool control_no_board = ctrl_none.all_captured && (ctrl_none.nibble(0) == 0x0);
    bool control_one_board = ctrl_one.all_captured &&
        (ctrl_one.nibble(0) != 0x0) && (ctrl_one.nibble(1) == 0x0);
    note("Control: fastramcfg=0 (no board) reg00 raw=0x" + hex1(ctrl_none.nibble(0)) +
         "; fastramcfg=2 (one board) reg00 raw before/after = 0x" +
         hex1(ctrl_one.nibble(0)) + "/0x" + hex1(ctrl_one.nibble(1)) +
         " -- confirms the Group2 apparatus reports the fastramcfg-dependent chain length it actually measures.");
    expect(control_no_board && control_one_board,
           "control [not graded -- no claim uid] fastramcfg=0 yields no responding board and fastramcfg=2 yields exactly one");

    // =========================================================================
    // Group 3 -- read/write split (sol:K-0735, opus:K-0762). Register 00
    // (word index 0, read-only) ignores a write: neither the register-48
    // case (cpu_wrapper.v:640) nor the register-44 case (cpu_wrapper.v:653,
    // unreachable here since cfg_z3 is always 0) matches
    // chip_addr[6:1]==0. Register 48 (word index 36, write-only) DOES act
    // on a write -- it completes the board's configuration. That contrast
    // -- a register a write cannot touch, and a register a write does --
    // is the read-only/write-only split this page describes.
    // =========================================================================
#if defined(MUT_WRITETOWRITEREG)
    // MUTATION sol:K-0735 stimulus red: MUT=WRITETOWRITEREG writes register
    //   48 instead of register 00 for the "no effect" check, so the
    //   "before"/"after" comparison shows the board configuring (silent)
    //   instead of staying unaffected.
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
    // type, size and other requirements into one claim. Only the type
    // field's manual-defined coding (bits 3-2 of the type byte are '11'
    // for a Zorro-II add-memory board, Appendix K's register map) is
    // checkable against an independent reference here. The declared SIZE
    // for boards 2 and 3 (1 MiB / 0.5 MiB stolen from SlowRAM,
    // cpu_wrapper.v:579-580) is NanoMig's own implementation choice, not a
    // value the manual prescribes, so "size" cannot be checked against an
    // independent reference, and "other requirements" is not observable at
    // this boundary at all. A claim bundling an untested clause is
    // withdrawn whole rather than kept with a caveat.
    // =========================================================================
    {
        std::string type_bits_summary;
        for (int p = 0; p < NPTS; p++) {
            type_bits_summary += " point" + std::to_string(p) + "=";
            if (chain_responds[p]) {
                uint8_t b32 = (chain_hi[p] >> 2) & 0x3;
                type_bits_summary += "0b" + std::to_string((b32>>1)&1) + std::to_string(b32&1);
            } else {
                type_bits_summary += "silent";
            }
        }
        const std::string content_reason =
            "reason_kind: source-silent (observable at this boundary -- "
            "register 00's size nybble reads directly -- but the printed "
            "page states nothing to assert the SIZE or OTHER REQUIREMENTS "
            "clauses against). Bundles device type, size and other "
            "requirements. Only the type field's manual-defined coding "
            "(bits 3-2 of the type byte == '11' for a Zorro-II add-memory "
            "board) is checkable against an independent reference here -- "
            "measured at each point of the Group2 chain (bits 3-2 of the hi "
            "nibble):" + type_bits_summary + ". The declared SIZE for "
            "boards 2 and 3 (1 MiB / 0.5 MiB stolen from SlowRAM, "
            "cpu_wrapper.v:579-580) is NanoMig's own implementation choice; "
            "the page prints no expected value to check it against, and "
            "'other requirements' is not observable at this boundary at "
            "all.";
        record("sol:K-0737 NOT GRADED (p431): 'read registers describe the "
               "device's size, type, and other requirements' " + content_reason);
        record("opus:K-0764 NOT GRADED (p431, same claim as sol:K-0737): " + content_reason);
    }

    // =========================================================================
    // Group 5 -- address window (sol:K-0742, sol:K-0743, sol:K-0744,
    // opus:K-0767). $E90000 (one byte outside the window) and $E7FFFE
    // (one word below it) never get the AUTOCONFIG override; $E8FFFE (the
    // very top of the 64K window) still does, and $E80100 (inside, but
    // past the 6-bit chip_addr[6:1] case range) also gets it -- together
    // these establish the window is the WHOLE 64 KiB, not merely near its
    // base. Each in-window probe is graded against ITS OWN preset (an
    // override reads something OTHER than what was presented on
    // chip_dout), not against an assumed RTL constant.
    // =========================================================================
#ifdef MUT_INSIDEPROBE
    // MUTATION sol:K-0742 stimulus red: MUT=INSIDEPROBE moves the "outside"
    //   probe (and its own preset) to $E80002 (inside $E8xxxx, register 00's
    //   own low address) instead of $E90000 -- the "does NOT get the
    //   override" check, still comparing against the moved preset's own top
    //   nibble (0x5), goes red because $E80002 IS autoconfig-driven
    //   (measured 0x6, the real size nibble, not the preset).
    // MUTATION sol:K-0744 stimulus red: same switch as sol:K-0742 (the
    //   window-confinement conjunct of its own AND).
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
    //   presets back unmodified instead of being overridden, so "the
    //   override still applies at the top of the window / at an aliased
    //   in-window address" goes red.
    // MUTATION sol:K-0744 stimulus red: same switch as sol:K-0742 (the
    //   addr_0x100_overridden conjunct of its own AND).
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
    //   byte below the window respond, so the below_window_passthrough
    //   conjunct has no breaking stimulus in this module's input space.

    // StrobeProbe: a configuration read is one 16-bit chip-bus cycle,
    // graded on the bus strobes themselves during a read of register 00
    // (only the AS-low samples whose address is inside $E8xxxx count),
    // not on an address outside configuration space.
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
    //   chip_dout/chip_din are declared [15:0] (cpu_wrapper.v:61-62); the
    //   port is 16 bits wide by declaration and no stimulus this module
    //   accepts can vary it.

    expect(window_confined_to_e8 && addr_0x100_overridden && every_board_answers && chain_outside_confined,
           "sol:K-0744 every Zorro II card configures in $00E8xxxx (and nowhere else)");
    expect(window_confined_to_e8 && addr_0x100_overridden && window_spans_full_64k &&
           word_cycle_confirmed && every_board_answers && chain_outside_confined,
           "opus:K-0767 Zorro II configuration space is the 64K block $00E8xxxx, 16-bit cycles, all Zorro II cards configure there");
    // MUTATION opus:K-0767 none impossible: unfalsifiable at this boundary --
    //   the below-window-passthrough clause has no breaking stimulus; see
    //   sol:K-0742's own record.

    // =========================================================================
    // Group 6 -- one nybble only (sol:K-0750). Preset chip_dout's low 12
    // bits to a recognisable pattern at $E80000; the top nybble is
    // overridden with autocfg_data, the low 12 bits pass through
    // unconditionally (cpu_wrapper.v:236, `chip_data[11:0]` is used in
    // BOTH arms of the ternary). That unconditional use is why the
    // pass-through half cannot be broken by any stimulus: it is graded on
    // whatever the preset happens to be, computed rather than hard-wired,
    // and its own mutation record says so. The override half CAN be
    // broken: disable sel_autoconfig (fastramcfg=0) and the top nybble
    // stops being overridden; graded on the manual-defined type-code bits
    // (3-2 == '11'), not on the specific 0xE constant.
    // =========================================================================
    const uint16_t nb_preset_value = 0x0ABC;
    std::map<uint32_t, uint16_t> nb_presets = { {0xE80000, nb_preset_value} };
#ifdef MUT_NYBBLE
    // MUTATION sol:K-0750 stimulus red: MUT=NYBBLE sets fastramcfg=0,
    //   disabling sel_autoconfig entirely (cpu_wrapper.v:611) -- the top
    //   nybble is no longer overridden and reads the preset's own top
    //   nybble (bits 3-2 == '00'), not the manual-defined type code.
    Harness nb(0, 0, {READ(autocfg_addr(IDX_REG00))}, nb_presets);
#else
    Harness nb(0, 1, {READ(autocfg_addr(IDX_REG00))}, nb_presets);
#endif
    uint16_t nb_word = nb.word[0];
    uint16_t nb_expected_low12 = nb_preset_value & 0x0FFF;
    note("Group6 nybble-only: preset chip_dout=0x" + hex4(nb_preset_value) + " at $E80000; captured full word=0x" +
         hex4(nb_word) + " (low 12 bits expected 0x" + hex4(nb_expected_low12).substr(1) +
         ", pass through unconditionally; top nibble's bits 3-2 expected '11' only when sel_autoconfig is enabled).");
    bool low12_passes_through = (nb_word & 0xFFF) == nb_expected_low12;
    bool top_nybble_overridden = type_bits_11((uint8_t)(nb_word >> 12));
    expect(low12_passes_through && top_nybble_overridden,
           "sol:K-0750 a configuration read physically returns only one nybble of real content; the rest passes through unmodified");
    // MUTATION sol:K-0750 none impossible: unfalsifiable at this boundary --
    //   cpu_wrapper.v:236's mux uses chip_data[11:0] in BOTH arms of the
    //   ternary (`{sel_autoconfig ? autocfg_data : chip_data[15:12],
    //   chip_data[11:0]}`), so the low 12 bits pass through regardless of
    //   sel_autoconfig's value. The expected low 12 bits here are computed
    //   from the preset itself, so no stimulus change to the preset can
    //   break the comparison; only a genuine RTL defect in the passthrough
    //   path could. The top-nybble-override half of this same assertion is
    //   exercised by MUT_NYBBLE above.
    note("p.438's pin table (physical pins 63/65/67/69, AD31/AD30/AD29/AD28) "
         "shows the SAME physical lines carry Zorro II's D15-D12 and Zorro "
         "III's D31-D28 (data phase) -- the top nybble of this design's "
         "16-bit chip bus (chip_din/chip_dout[15:0], cpu_wrapper.v:61-62) "
         "IS the page's 'D31-D28' for a Zorro II board. The bits[15:12] "
         "check above tests exactly the clause the page names for this "
         "bus, not a different lane numbering this design lacks.");
    {
        const std::string bus_mode_reason =
            "reason_kind: absent (the second of the two bus modes this "
            "claim names has no signal path in this build at all -- cfg_z3 "
            "is a compiled-out constant, not a wire any stimulus drives). "
            "Its own quote claims the one-nybble rule holds 'physically "
            "return only the top 4 bits of data, on D31-D28 for either bus "
            "mode' -- both Zorro II's 16-bit bus AND Zorro III's 32-bit "
            "bus. This build hard-wires cfg_z3 to 0 (cpu_wrapper.v:539): "
            "only the Zorro II bus mode exists here, so 'either... mode' "
            "cannot be decided. The Zorro-II-only equivalent is graded "
            "(p431, graded: sol:K-0750).";
        record("opus:K-0771 NOT GRADED (p431): 'physically return only the "
               "top 4 bits of data, on D31-D28 for either bus mode' " + bus_mode_reason);
    }

    // =========================================================================
    // Group 7 -- paired addressing (sol:K-0752, sol:K-0753, sol:K-0760,
    // opus:K-0773, opus:K-0779). Every logical 8-bit register is two
    // nybbles at two consecutive word addresses. Graded structurally
    // rather than against a hard-coded expected byte: register 00's TYPE
    // address ($E80000) must carry the SAME nibble whether fastramcfg is 1
    // or 2, while its SIZE address ($E80002) must carry a DIFFERENT nibble
    // -- proving both addresses are genuinely, independently read, rather
    // than one address supplying an assumed constant.
    // =========================================================================
#ifdef MUT_SWAPNYBBLE
    // MUTATION sol:K-0752 stimulus red: MUT=SWAPNYBBLE reads $E80002
    //   before $E80000 (reversing the CPU's OWN physical read order), so
    //   the slot the test treats as "the type address" actually captured
    //   the size address's content and vice versa -- the "type is stable,
    //   size varies" structural check goes red because it is now checking
    //   the wrong address for each property.
    // MUTATION sol:K-0753 stimulus red: same switch as sol:K-0752.
    // MUTATION sol:K-0760 stimulus red: same switch as sol:K-0752.
    // MUTATION opus:K-0773 stimulus red: same switch as sol:K-0752.
    // MUTATION opus:K-0779 stimulus red: same switch as sol:K-0752.
    // Read order reversed ($E80002 before $E80000), but the test still
    // treats word index 0 as "hi" and index 1 as "lo" -- exactly as the
    // baseline does -- so each now holds the OTHER address's content.
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
            "reason_kind: insufficient (the port is not missing -- "
            "$E8003C/$E8003E is readable -- but its own evidence, a raw "
            "value every other undriven address in the window also "
            "produces, is not enough to establish that THIS address is "
            "register 3C's location). Register 3C's logical value is 0, "
            "and this design's complement rule (cpu_wrapper.v:547's "
            "default 4'b1111) makes EVERY undriven read register -- at "
            "$E8003C/$E8003E and at any other reserved index -- read the "
            "identical raw 0xF/0xF, logical 0/0. No stimulus in this "
            "module's input space can distinguish $E8003C/$E8003E from "
            "$E8007C/$E8007E or $E8000C/$E8000E by value, so this specific "
            "address cannot be established as register 3C's location at "
            "this boundary, even though the manual's own worked example "
            "(p.432) names exactly this pair and this value.";
        record("sol:K-0757 NOT GRADED (p432): 'upper nybbles of locations "
               "$00E8003C and $00E8003E' " + reg3c_reason);
    }

    // =========================================================================
    // Group 8 -- complement rule (sol:K-0755(p432), sol:K-0756, sol:K-0759,
    // opus:K-0776). Register 3C, and a sweep of the manual's OWN "Reserved,
    // must be 0" registers -- $0C (p.434), $30, $34, $38, $40 (p.435):
    // unlike $04 (product number, "completely up to the manufacturer",
    // p.433) or $08 (er_Flags, board-specific choices), the manual states
    // what these must read, so a raw 0xF here is checkable against the
    // page, not just against the RTL's own default. Every one reads raw
    // 1111, complementing to logical 0, evidence for "EVERY read register
    // except 00" rather than register 3C alone. Register 00 is the sole
    // exception and reads DIRECT: graded on the manual-defined type-code
    // bit pattern (bits 3-2 == '11') rather than on "not all ones", since a
    // complemented register could just as easily fail to read all-ones by
    // coincidence.
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
    //   0" registers -- "every swept register reads 0xF" goes red.
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
        uint8_t lg = logical(reserved_idx[i], raw); // idx>1 for all ten, complement applies
        if (raw != 0xF || lg != 0x0) reserved_all_zero = false;
        reserved_summary += " 0x" + hex1(raw);
    }
    note("Group8 reserved-register sweep (bytes $0C, $30, $34, $38, $40, the manual's own "
         "\"Reserved, must be 0\" registers, pp.434-435): raw nybbles =" +
         reserved_summary + " (0xF logical 0 expected at each, same complement rule as register 3C).");

    // register 00's own type-code bits (3-2 of the hi nibble) read DIRECT,
    // never complemented -- the manual-defined Zorro-II code is '11'. A
    // dedicated, independently-mutated harness (not Group2's `chain` or
    // Group7's `reg00_f1`/`reg00_f2`, so this conjunct is not silently
    // riding on a mutation aimed elsewhere).
#ifdef MUT_REG00SILENT
    // MUTATION sol:K-0755 stimulus red: MUT=REG00SILENT sets fastramcfg=0,
    //   disabling sel_autoconfig entirely (cpu_wrapper.v:611) -- register
    //   00 no longer responds at all (raw 0x0, confirmed by this
    //   package's own control at fastramcfg=0), so its type-code bits are
    //   no longer '11'.
    // MUTATION opus:K-0776 stimulus red: same switch as sol:K-0755.
    Harness reg00type(0, 0, {READ(autocfg_addr(IDX_REG00))});
#else
    Harness reg00type(0, 1, {READ(autocfg_addr(IDX_REG00))});
#endif
    uint8_t reg00_hi_stable = reg00type.nibble(0);
    bool reg00_type_direct = type_bits_11(reg00_hi_stable);
    note("Group8 complement: register3C raw nybbles=0x" + hex1(cpl_hi_raw) + hex1(cpl_lo_raw) +
         " logical=0x" + hex1(reg3c_hi_logical) + hex1(reg3c_lo_logical) +
         "; register00 type-code bits[3:2]=0b" + std::to_string((reg00_hi_stable>>3)&1) + std::to_string((reg00_hi_stable>>2)&1) +
         " (11 expected, uncomplemented -- the sole exception).");
    bool complement_rule_confirmed = reg3c_is_zero && reserved_all_zero && reg00_type_direct;
    expect(complement_rule_confirmed, "sol:K-0755 every AUTOCONFIG read register except register 00 is physically complemented");
    expect(complement_rule_confirmed, "opus:K-0776 in the actual implementation all read registers except the 00 register are physically complemented");
    // Three independent stimulus mutations cover this assertion's three
    // conjuncts: MUT_NOCOMPLEMENT (register 3C), MUT_SWEEPWRONGREG (the
    // reserved-register sweep, both above) and MUT_REG00SILENT (register
    // 00's own type-code bits, above) -- no conjunct here is unfalsifiable.
    expect(reg3c_is_zero, "sol:K-0756 logical register 3C has the value zero");
    expect(reg3c_raw_all_ones && reg3c_is_zero, "sol:K-0759 because read registers are complemented, both physical nybbles for logical zero return all 1 bits");
    {
        const std::string zorro3_addr_reason =
            "reason_kind: absent (the Zorro III address decode this claim "
            "names has no signal path in this build at all -- cfg_z3 is a "
            "compiled-out constant, not a wire any stimulus drives). Its "
            "own quote names $FF00003C and $FF00013C -- Zorro III's own "
            "addressing form for register 3C. This build hard-wires cfg_z3 "
            "to 0 (cpu_wrapper.v:539): no Zorro III board exists, so no "
            "address in Zorro III's own space is ever decoded here. The "
            "Zorro-II addressing and value are graded (p432, graded: "
            "sol:K-0756, sol:K-0759).";
        record("opus:K-0777 NOT GRADED (p432): 'or $FF00003C and "
               "$FF00013C, both return all Is' " + zorro3_addr_reason);
    }

    // =========================================================================
    // Liveness -- carries no claim uid.
    // =========================================================================
    expect(lc_before.all_captured && rw.all_captured && nb.all_captured &&
           before_raw != 0x0 && reg3c_hi_raw != reg00_hi_stable,
           "fixture-liveness [not graded -- apparatus check, no claim uid]");

    std::cout << "\n" << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
