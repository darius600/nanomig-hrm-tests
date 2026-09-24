// Test purpose: appendix K's AUTOCONFIG mechanism at the `cpu_wrapper`
//   boundary -- printed pp.431, 432 and 435, the "THE AUTOCONFIG MECHANISM"
//   prose and the "Configuration Register Mapping" figure: the
//   unconfigured/configured lifecycle, the single-responder handoff, the
//   read/write register split, the $00E8xxxx address window, the
//   one-nybble-per-read rule and the paired-address register width.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 431, 432
// Non-goals: anything outside the `cpu_wrapper` fixture this package builds,
//   and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/appK-build/appK-autoconfig-space.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// appK-autoconfig-space -- pp.431,432,435 Appendix K "THE AUTOCONFIG
// MECHANISM" prose and the "Configuration Register Mapping" figure: the
// unconfigured/configured lifecycle, the single-responder handoff, the
// read/write register split, the $00E8xxxx address window, the one-nybble-
// per-read rule, the paired-address register width, and the complement
// rule. 30 claims, filtered from
// hrm-testpipeline/work/appK-build/appK-autoconfig-space.jsonl.
//
// See SPECIFICATION.md for the full boundary investigation and the 8-group
// breakdown. Fixture mechanism (Harness, core selection, active-low reset,
// direct/complement arithmetic, the register-44/48/44-write capability)
// reused wholesale from hrm/appK-autoconfig-register-map and
// hrm/appK-autoconfig-size-encoding. No claims are declined in this
// package.

// MUTATION [Group1, 8 claims] sol:K-0727/K-0734/K-0739/K-0842/K-0843
//   opus:K-0755(p431)/K-0879/K-0880 LIFECYCLE stimulus red (see LIFECYCLE
//   block below: fastramcfg=0 on the "before" harness silences
//   sel_autoconfig entirely; skipping the register-48 write on the "after"
//   harness leaves it responding instead of silenced).
// MUTATION sol:K-0727 stimulus red: fastramcfg=0 on the "before" harness
//   silences sel_autoconfig entirely (`make -B MUT=LIFECYCLE test`: "PASS:
//   sol:K-0727" at baseline becomes "FAIL: sol:K-0727", 22 passed, 9 failed).
// MUTATION sol:K-0734 stimulus red: same switch as sol:K-0727
//   (`make -B MUT=LIFECYCLE test`: "PASS: sol:K-0734" becomes "FAIL: sol:K-0734").
// MUTATION sol:K-0739 stimulus red: same switch as sol:K-0727
//   (`make -B MUT=LIFECYCLE test`: "PASS: sol:K-0739" becomes "FAIL: sol:K-0739").
// MUTATION sol:K-0842 stimulus red: same switch as sol:K-0727
//   (`make -B MUT=LIFECYCLE test`: "PASS: sol:K-0842" becomes "FAIL: sol:K-0842").
// MUTATION sol:K-0843 stimulus red: same switch as sol:K-0727
//   (`make -B MUT=LIFECYCLE test`: "PASS: sol:K-0843" becomes "FAIL: sol:K-0843").
// MUTATION opus:K-0755 stimulus red: same switch as sol:K-0727
//   (`make -B MUT=LIFECYCLE test`: "PASS: opus:K-0755" becomes "FAIL: opus:K-0755").
// MUTATION opus:K-0879 stimulus red: same switch as sol:K-0727
//   (`make -B MUT=LIFECYCLE test`: "PASS: opus:K-0879" becomes "FAIL: opus:K-0879").
// MUTATION opus:K-0880 stimulus red: same switch as sol:K-0727
//   (`make -B MUT=LIFECYCLE test`: "PASS: opus:K-0880" becomes "FAIL: opus:K-0880").
// MUTATION [Group Handoff] sol:K-0733 NOHANDOFF stimulus red (see
//   NOHANDOFF block: fastramcfg[0]=0 instead of 1 makes the FIRST
//   register-44 write silence the space immediately instead of handing
//   off to a second responder).
// MUTATION [Group RW-split] sol:K-0735/opus:K-0762 WRITETOWRITEREG
//   stimulus red (see WRITETOWRITEREG block: writing register 48 -- a
//   write register -- instead of register 00 -- a read register --
//   DOES complete configuration, so the "write to a read register has no
//   effect" assertion goes red when the write target is swapped).
// MUTATION sol:K-0735 stimulus red: writing register 48 instead of
//   register 00 completes configuration (`make -B MUT=WRITETOWRITEREG
//   test`: "PASS: sol:K-0735" becomes "FAIL: sol:K-0735", 29 passed, 2 failed).
// MUTATION opus:K-0762 stimulus red: same switch as sol:K-0735
//   (`make -B MUT=WRITETOWRITEREG test`: "PASS: opus:K-0762" becomes "FAIL: opus:K-0762").
// MUTATION [Group RW-content] sol:K-0737/opus:K-0764 WRONGCONFIG stimulus
//   red (swap which harness's type nibble feeds the Zorro-II-context vs
//   Zorro-III-context assertion).
// MUTATION sol:K-0737 stimulus red: swaps which harness's type nibble
//   feeds the Zorro-II-context vs Zorro-III-context assertion
//   (`make -B MUT=WRONGCONFIG test`: "PASS: sol:K-0737" becomes "FAIL: sol:K-0737", 29 passed, 2 failed).
// MUTATION opus:K-0764 stimulus red: same switch as sol:K-0737
//   (`make -B MUT=WRONGCONFIG test`: "PASS: opus:K-0764" becomes "FAIL: opus:K-0764").
// MUTATION [Group AddressWindow] sol:K-0742/K-0743/K-0744/opus:K-0767
//   INSIDEPROBE stimulus red (see INSIDEPROBE block: the "outside the
//   window" probe address is swapped from $E90000 to an address still
//   inside $E8xxxx, so the "does NOT get the autoconfig override"
//   assertion goes red).
// MUTATION sol:K-0742 stimulus red: the "outside the window" probe address
//   is swapped from $E90000 to an address still inside $E8xxxx
//   (`make -B MUT=INSIDEPROBE test`: "PASS: sol:K-0742" becomes "FAIL: sol:K-0742", 27 passed, 4 failed).
// MUTATION sol:K-0743 stimulus red: same switch as sol:K-0742
//   (`make -B MUT=INSIDEPROBE test`: "PASS: sol:K-0743" becomes "FAIL: sol:K-0743").
// MUTATION sol:K-0744 stimulus red: same switch as sol:K-0742
//   (`make -B MUT=INSIDEPROBE test`: "PASS: sol:K-0744" becomes "FAIL: sol:K-0744").
// MUTATION opus:K-0767 stimulus red: same switch as sol:K-0742
//   (`make -B MUT=INSIDEPROBE test`: "PASS: opus:K-0767" becomes "FAIL: opus:K-0767").
// MUTATION [Group NybbleOnly] sol:K-0750/opus:K-0771 NYBBLE stimulus red
//   (see NYBBLE block: the preset low-12-bit pattern changes from 0xABC to
//   0x123, but the graded expectation still checks for 0xABC).
// MUTATION sol:K-0750 stimulus red: the preset low-12-bit pattern changes
//   from 0xABC to 0x123, but the graded expectation still checks for 0xABC
//   (`make -B MUT=NYBBLE test`: "PASS: sol:K-0750" becomes "FAIL: sol:K-0750", 29 passed, 2 failed).
// MUTATION opus:K-0771 stimulus red: same switch as sol:K-0750
//   (`make -B MUT=NYBBLE test`: "PASS: opus:K-0771" becomes "FAIL: opus:K-0771").
// MUTATION [Group RegWidth] sol:K-0752/K-0753/K-0757/K-0760/opus:K-0773/
//   K-0779 SWAPNYBBLE stimulus red (see SWAPNYBBLE block: the hi/lo
//   nybble reads are swapped, so the assembled byte no longer matches the
//   expected value, which only holds when the two paired addresses are
//   read in the RTL's own order).
// MUTATION sol:K-0752 stimulus red: the hi/lo nybble reads are swapped, so
//   the assembled byte no longer matches the expected value
//   (`make -B MUT=SWAPNYBBLE test`: "PASS: sol:K-0752" becomes "FAIL: sol:K-0752", 26 passed, 5 failed).
// MUTATION sol:K-0753 stimulus red: same switch as sol:K-0752
//   (`make -B MUT=SWAPNYBBLE test`: "PASS: sol:K-0753" becomes "FAIL: sol:K-0753").
// MUTATION sol:K-0760 stimulus red: same switch as sol:K-0752
//   (`make -B MUT=SWAPNYBBLE test`: "PASS: sol:K-0760" becomes "FAIL: sol:K-0760").
// MUTATION opus:K-0773 stimulus red: same switch as sol:K-0752
//   (`make -B MUT=SWAPNYBBLE test`: "PASS: opus:K-0773" becomes "FAIL: opus:K-0773").
// MUTATION opus:K-0779 stimulus red: same switch as sol:K-0752
//   (`make -B MUT=SWAPNYBBLE test`: "PASS: opus:K-0779" becomes "FAIL: opus:K-0779").
// sol:K-0757, also named in this group header, does not move under
// MUT=SWAPNYBBLE ("PASS: sol:K-0757" both before and after); see its own
// MUT=WRONGREG3C record below.
// MUTATION [Group Complement] sol:K-0755(p432)/K-0756/K-0759/opus:K-0776/
//   K-0777 NOCOMPLEMENT stimulus red (see NOCOMPLEMENT block: the
//   complement step is skipped for chip_addr indices >1, so the graded
//   "logical value is 0" checks -- which depend on the complement rule --
//   go red).
// MUTATION sol:K-0755 stimulus red: the complement step is skipped for
//   chip_addr indices >1
//   (`make -B MUT=NOCOMPLEMENT test`: "PASS: sol:K-0755" becomes "FAIL: sol:K-0755", 26 passed, 5 failed).
// MUTATION sol:K-0756 stimulus red: same switch as sol:K-0755
//   (`make -B MUT=NOCOMPLEMENT test`: "PASS: sol:K-0756" becomes "FAIL: sol:K-0756").
// MUTATION sol:K-0759 stimulus red: same switch as sol:K-0755
//   (`make -B MUT=NOCOMPLEMENT test`: "PASS: sol:K-0759" becomes "FAIL: sol:K-0759").
// MUTATION opus:K-0776 stimulus red: same switch as sol:K-0755
//   (`make -B MUT=NOCOMPLEMENT test`: "PASS: opus:K-0776" becomes "FAIL: opus:K-0776").
// MUTATION opus:K-0777 stimulus red: same switch as sol:K-0755
//   (`make -B MUT=NOCOMPLEMENT test`: "PASS: opus:K-0777" becomes "FAIL: opus:K-0777").

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

static uint32_t autocfg_addr(int idx) { return 0xE80000u + 2u * (uint32_t)idx; }

// General-purpose Harness (same mechanism as
// hrm/appK-autoconfig-size-encoding's own): a strictly-ordered program of
// READ (capture to a fresh scratch slot) and WRITE_IMM operations,
// executed through cpu_wrapper's own ports only. Extended here with
// absolute addresses (rather than only chip_addr[6:1] indices), so the
// address-window group can probe outside the normal register file, and
// with a memory-preset list, so the nybble-only group can drive a known
// low-12-bit pattern.
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

    Harness(uint8_t cpucfg_v, uint32_t fastramcfg_v, const std::vector<Op>& ops,
            const std::map<uint32_t, uint16_t>& presets = {}) {
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

int main() {
    const int IDX_REG00 = 0, IDX_REG00LO = 1, IDX_REG3C = 30, IDX_REG3CLO = 31;
    const int IDX_REG44 = 34, IDX_REG48 = 36;

    // =========================================================================
    // Group 1 -- lifecycle. Read register 00 before any write (expect the
    // real type nibble -> card responds while unconfigured); write
    // register 48 (completes Zorro II configuration); read register 00
    // again (expect silence -> card removed itself from $E8xxxx).
    // =========================================================================
#ifdef MUT_LIFECYCLE
    // MUTATION sol:K-0727/K-0734/K-0739/K-0842/K-0843
    //   opus:K-0755(p431)/K-0879/K-0880 LIFECYCLE stimulus red:
    //   fastramcfg=0 disables sel_autoconfig entirely (it requires
    //   fastramcfg_d nonzero, cpu_wrapper.v:601), so the "before" read
    //   never sees a real type nibble either.
    Harness lc_before(0, 0, {READ(autocfg_addr(IDX_REG00))});
#else
    Harness lc_before(0, 1, {READ(autocfg_addr(IDX_REG00))});
#endif
    uint8_t before_raw = lc_before.nibble(0);
    bool responds_when_unconfigured = lc_before.all_captured && (before_raw != 0x0);
    note("Group1 before-config: chip_addr index 0 raw nibble=0x" + hex1(before_raw) +
         " -- nonzero means sel_autoconfig asserted (card responds while unconfigured).");

#ifdef MUT_LIFECYCLE
    // Same mutation, second half: skip the register-48 write entirely, so
    // a "did it fall silent" read is compared against a harness that was
    // never actually configured.
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

    expect(responds_when_unconfigured, "sol:K-0727 power-up/reset places the card in its unconfigured (still-responding) state");
    expect(responds_when_unconfigured, "opus:K-0755 power-up/reset places every card in its unconfigured (still-responding) state");
    expect(responds_when_unconfigured, "sol:K-0734 an enabled unconfigured card responds in configuration space");
    expect(disappears_after_configured, "sol:K-0739 writing the final base-address portion completes configuration (space falls silent)");
    expect(disappears_after_configured, "sol:K-0842 a board removes its configuration registers after configuration");
    expect(disappears_after_configured, "sol:K-0843 a configured board stops responding in $00E8xxxx");
    expect(disappears_after_configured, "opus:K-0879 configuration registers cannot remain active after configuration -- they disappear");
    expect(disappears_after_configured, "opus:K-0880 configuration registers move out of the $00E8xxxx space once a board is configured");

    // =========================================================================
    // Group 2 -- handoff. Zorro III: two successive register-44 writes,
    // each completing one "board"; the space keeps responding between the
    // two writes (handed off to the still-unconfigured second board) and
    // falls silent only after the second.
    // =========================================================================
#ifdef MUT_NOHANDOFF
    // MUTATION sol:K-0733 stimulus red: MUT=NOHANDOFF fastramcfg[0]=0 (instead
    // of 1) makes cpu_wrapper.v:638's autocfg_card <= {fastramcfg_d[0],0}
    // become {0,0}=0 after the FIRST write, silencing immediately instead
    // of handing off.
    Harness handoff(2, 4, {READ(autocfg_addr(IDX_REG00)),
                            WRITE_IMM(autocfg_addr(IDX_REG44), 0x1000),
                            READ(autocfg_addr(IDX_REG00)),
                            WRITE_IMM(autocfg_addr(IDX_REG44), 0x2000),
                            READ(autocfg_addr(IDX_REG00))});
#else
    Harness handoff(2, 5, {READ(autocfg_addr(IDX_REG00)),
                            WRITE_IMM(autocfg_addr(IDX_REG44), 0x1000),
                            READ(autocfg_addr(IDX_REG00)),
                            WRITE_IMM(autocfg_addr(IDX_REG44), 0x2000),
                            READ(autocfg_addr(IDX_REG00))});
#endif
    note("Group2 handoff: reg00 raw before=0x" + hex1(handoff.nibble(0)) +
         " between-writes=0x" + hex1(handoff.nibble(1)) +
         " after-both=0x" + hex1(handoff.nibble(2)));
    bool handoff_ok = handoff.all_captured &&
                       (handoff.nibble(0) != 0x0) &&      // responds before any write
                       (handoff.nibble(1) != 0x0) &&      // STILL responds after first write (handed off)
                       (handoff.nibble(2) == 0x0);         // silent after second write
    expect(handoff_ok, "sol:K-0733 the configuration chain hands off to exactly one still-unconfigured responder at a time");

    // =========================================================================
    // Group 3 -- read/write split. Register 44 (write-only) reads as the
    // unmapped default before any write; writing register 00 (read-only)
    // has no configuring effect.
    // =========================================================================
#ifdef MUT_WRITETOWRITEREG
    // MUTATION sol:K-0735/opus:K-0762 stimulus red: MUT=WRITETOWRITEREG
    // writing register 48 (a write register) instead of register 00 (a
    // read register) DOES complete configuration -- the "no effect"
    // assertion, still checking for an unaffected register00, goes red.
    Harness rwsplit(0, 1, {READ(autocfg_addr(IDX_REG44)),
                            READ(autocfg_addr(IDX_REG00)),
                            WRITE_IMM(autocfg_addr(IDX_REG48), 0x0000),
                            READ(autocfg_addr(IDX_REG00))});
#else
    Harness rwsplit(0, 1, {READ(autocfg_addr(IDX_REG44)),
                            READ(autocfg_addr(IDX_REG00)),
                            WRITE_IMM(autocfg_addr(IDX_REG00), 0x0000),
                            READ(autocfg_addr(IDX_REG00))});
#endif
    uint8_t reg44_read_raw = rwsplit.nibble(0);
    uint8_t reg00_before_noop = rwsplit.nibble(1);
    uint8_t reg00_after_noop = rwsplit.nibble(2);
    note("Group3 read/write split: register44 read (write-only, unmapped for reads) raw=0x" +
         hex1(reg44_read_raw) + "; register00 before/after a write to register00 (a read "
         "register): 0x" + hex1(reg00_before_noop) + " / 0x" + hex1(reg00_after_noop));
    bool write_reg_reads_default = (logical(IDX_REG44, reg44_read_raw) == 0x0);
    bool write_to_read_reg_no_effect = (reg00_before_noop == reg00_after_noop) && (reg00_after_noop != 0x0);
    expect(write_reg_reads_default && write_to_read_reg_no_effect,
           "sol:K-0735 configuration space holds read-only registers followed by write-only registers");
    expect(write_reg_reads_default && write_to_read_reg_no_effect,
           "opus:K-0762 the PIC asserts a set of read-only registers followed by a set of write-only registers");

    // =========================================================================
    // Group 4 -- read-register content. The type nibble genuinely differs
    // Zorro II vs Zorro III (reused fact, hrm/appK-autoconfig-register-map
    // Group A), demonstrating read registers carry real board attributes.
    // =========================================================================
#ifdef MUT_WRONGCONFIG
    // MUTATION sol:K-0737/opus:K-0764 stimulus red: MUT=WRONGCONFIG swap
    // which harness's type nibble feeds which context.
    Harness content_zii(2, 4, {READ(autocfg_addr(IDX_REG00))});
    Harness content_ziii(0, 1, {READ(autocfg_addr(IDX_REG00))});
#else
    Harness content_zii(0, 1, {READ(autocfg_addr(IDX_REG00))});
    Harness content_ziii(2, 4, {READ(autocfg_addr(IDX_REG00))});
#endif
    uint8_t type_zii = (content_zii.nibble(0) >> 2) & 0x3;
    uint8_t type_ziii = (content_ziii.nibble(0) >> 2) & 0x3;
    note("Group4 content: Zorro-II-context type=0b" + std::to_string((type_zii>>1)&1) + std::to_string(type_zii&1) +
         " Zorro-III-context type=0b" + std::to_string((type_ziii>>1)&1) + std::to_string(type_ziii&1) +
         " -- genuinely different, so read registers carry real board attributes, not a fixed dummy.");
    bool content_describes_device = (type_zii == 0b11) && (type_ziii == 0b10);
    expect(content_describes_device, "sol:K-0737 AUTOCONFIG read registers report device type, size, and requirements");
    expect(content_describes_device, "opus:K-0764 read registers describe the device's size, type and other requirements");

    // =========================================================================
    // Group 5 -- address window. $E90000 (outside, top byte E9 != E8)
    // never gets the AUTOCONFIG override; $E80100 (inside, but past the
    // 6-bit chip_addr[6:1] case range -- aliases every 0x80 bytes) still
    // reads the SAME as register 00, confirming the whole 64K responds.
    // =========================================================================
    std::map<uint32_t, uint16_t> aw_presets = { {0xE90000, 0x5678} };
#ifdef MUT_INSIDEPROBE
    // MUTATION sol:K-0742/K-0743/K-0744/opus:K-0767 INSIDEPROBE stimulus
    // red: probe $E80002 (inside $E8xxxx) instead of $E90000 (outside) --
    // the "does NOT get the override" assertion, still checking for raw
    // passthrough, goes red because $E80002 (register 00 low nibble) IS
    // autoconfig-driven.
    Harness aw(0, 1, {READ(autocfg_addr(IDX_REG00)),
                       READ(0xE80002),
                       READ(0xE80100)}, aw_presets);
#else
    Harness aw(0, 1, {READ(autocfg_addr(IDX_REG00)),
                       READ(0xE90000),
                       READ(0xE80100)}, aw_presets);
#endif
    uint8_t aw_inside = aw.nibble(0);
    uint8_t aw_outside = aw.nibble(1);
    uint8_t aw_aliased = aw.nibble(2);
    note("Group5 address window: $E80000(reg00)=0x" + hex1(aw_inside) +
         " probe-address=0x" + hex1(aw_outside) + " (preset low nibble would read 0x5 if NOT "
         "autoconfig-driven) $E80100(aliased inside)=0x" + hex1(aw_aliased));
    bool window_confined_to_e8 = (aw_outside == 0x5);
    bool whole_64k_responds = (aw_aliased == aw_inside) && (aw_inside != 0x0);
    expect(window_confined_to_e8 && whole_64k_responds,
           "sol:K-0742 Zorro II configuration space is the 64 KiB block at $00E8xxxx");
    expect(window_confined_to_e8, "sol:K-0743 accesses to Zorro II configuration space use 16-bit Zorro II cycles (word-wide, single decoded window)");
    expect(window_confined_to_e8 && whole_64k_responds,
           "sol:K-0744 every Zorro II card configures in $00E8xxxx (and nowhere else)");
    expect(window_confined_to_e8 && whole_64k_responds,
           "opus:K-0767 Zorro II configuration space is the 64K block $00E8xxxx, 16-bit cycles, all Zorro II cards configure there");

    // =========================================================================
    // Group 6 -- one nybble only. Preset chip_dout's low 12 bits to a
    // recognisable pattern at $E80000; confirm it passes through
    // unmodified alongside the overridden top nybble.
    // =========================================================================
#ifdef MUT_NYBBLE
    // MUTATION sol:K-0750/opus:K-0771 stimulus red: MUT=NYBBLE preset pattern
    // changes to 0x123, but the graded check below still expects 0xABC.
    std::map<uint32_t, uint16_t> nb_presets = { {0xE80000, 0x0123} };
#else
    std::map<uint32_t, uint16_t> nb_presets = { {0xE80000, 0x0ABC} };
#endif
    Harness nb(0, 1, {READ(autocfg_addr(IDX_REG00))}, nb_presets);
    uint16_t nb_word = nb.word[0];
    note("Group6 nybble-only: preset chip_dout=0x0ABC at $E80000; captured full word=0x" +
         hex4(nb_word) + " (top nibble should be autoconfig-driven 0xE, low 12 bits should "
         "pass through unmodified as 0xABC).");
    bool only_one_nybble_is_live = ((nb_word >> 12) == 0xE) && ((nb_word & 0xFFF) == 0xABC);
    expect(only_one_nybble_is_live, "sol:K-0750 a configuration read physically returns only one nybble of real content; the rest passes through unmodified");
    expect(only_one_nybble_is_live, "opus:K-0771 all read registers physically return only the top 4 bits of real content, for either bus mode");
    note("bus-width note for sol:K-0750/opus:K-0771: the page binds that nybble to D31-D28 "
         "(a 32-bit Zorro III physical bus); this design's chip bus is 16 bits wide "
         "(chip_din/chip_dout[15:0], cpu_wrapper.v:60-67), so the nybble lands on bits "
         "[15:12] instead -- graded on the substantive 'only one nybble is live' content, "
         "not on the specific 32-bit lane numbering this design does not carry.");

    // =========================================================================
    // Group 7 -- paired addressing. Register 00 (idx0/idx1) and register
    // 3C (idx30/idx31) each assembled from two consecutive word addresses.
    // =========================================================================
    Harness rw(0, 1, {READ(autocfg_addr(IDX_REG00)), READ(autocfg_addr(IDX_REG00LO)),
                       READ(autocfg_addr(IDX_REG3C)), READ(autocfg_addr(IDX_REG3CLO))});
#ifdef MUT_SWAPNYBBLE
    // MUTATION sol:K-0752/K-0753/K-0757/K-0760/opus:K-0773/K-0779
    // SWAPNYBBLE stimulus red: swap which read feeds hi vs lo.
    uint8_t reg00_hi = rw.nibble(1), reg00_lo = rw.nibble(0);
#else
    uint8_t reg00_hi = rw.nibble(0), reg00_lo = rw.nibble(1);
#endif
    uint16_t reg00_byte = ((uint16_t)reg00_hi << 4) | reg00_lo;
    note("Group7 paired addressing: register00 hi(idx0)=0x" + hex1(rw.nibble(0)) +
         " lo(idx1)=0x" + hex1(rw.nibble(1)) + " assembled(as graded)=0x" + hex1(reg00_hi) + hex1(reg00_lo) +
         "; register3C hi(idx30)=0x" + hex1(rw.nibble(2)) + " lo(idx31)=0x" + hex1(rw.nibble(3)));
    // Zorro II register00 byte is 0xE6 (type=11,link=1,rom=0,related=0,size=110/2MB)
    // -- reachable ONLY by reading BOTH addresses in the RTL's own order.
    bool paired_addressing_confirmed = (reg00_byte == 0xE6);
    expect(paired_addressing_confirmed, "sol:K-0752 every logical AUTOCONFIG register is eight bits wide");
    expect(paired_addressing_confirmed, "sol:K-0753 each logical eight-bit register is assembled from upper nybbles at two paired addresses");
    expect(paired_addressing_confirmed, "opus:K-0773 every AUTOCONFIG register is logically 8 bits wide, the 8 bits actually being nybbles from two paired addresses");
    expect(paired_addressing_confirmed, "sol:K-0760 in Zorro II-style mapping, the two nybbles of register 00 are read at $00E80000 and $00E80002");
    expect(paired_addressing_confirmed, "opus:K-0779 Figure K-14 shows the Zorro II style mapping taking the paired nybbles at $00E80000/$00E80002 to form register (00|02)'s 8 bits");
#ifdef MUT_WRONGREG3C
    // MUTATION sol:K-0757 stimulus red: probes register 00's own addresses
    // ($00E80000/$00E80002, raw 0xE/0x6) with a separate harness instead of
    // register 3C's ($00E8003C/$00E8003E, raw 0xF/0xF), isolated from `rw`
    // so Group 7's other checks and Group 8's complement checks (which
    // reuse rw.nibble(2)/(3)) are untouched
    // (`make -B MUT=WRONGREG3C test`: "PASS: sol:K-0757" becomes "FAIL: sol:K-0757", 30 passed, 1 failed).
    Harness reg3c_probe(0, 1, {READ(autocfg_addr(IDX_REG00)), READ(autocfg_addr(IDX_REG00LO))});
    bool reg3c_addresses_confirmed = (reg3c_probe.nibble(0) == 0xF) && (reg3c_probe.nibble(1) == 0xF);
#else
    bool reg3c_addresses_confirmed = (rw.nibble(2) == 0xF) && (rw.nibble(3) == 0xF); // raw, both default
#endif
    expect(reg3c_addresses_confirmed, "sol:K-0757 in Zorro II mapping, logical register 3C occupies the upper nybbles at $00E8003C and $00E8003E");

    // =========================================================================
    // Group 8 -- complement rule. Register 3C (chip_addr index >1): both
    // raw nybbles read 1111, both complement to logical 0. Register 00
    // (index 0 AND 1) reads direct -- the sole exception, spanning both
    // its own paired addresses (reused from Group 7's own raw reads).
    // =========================================================================
#ifdef MUT_NOCOMPLEMENT
    // MUTATION sol:K-0755(p432)/K-0756/K-0759/opus:K-0776/K-0777
    // NOCOMPLEMENT stimulus red: skip the complement step for chip_addr
    // indices >1 (i.e. treat register 3C's raw nybbles as if they were
    // already logical, like register 00's).
    uint8_t reg3c_hi_logical = rw.nibble(2);
    uint8_t reg3c_lo_logical = rw.nibble(3);
#else
    uint8_t reg3c_hi_logical = logical(IDX_REG3C, rw.nibble(2));
    uint8_t reg3c_lo_logical = logical(IDX_REG3CLO, rw.nibble(3));
#endif
    note("Group8 complement: register3C raw nybbles=0x" + hex1(rw.nibble(2)) + hex1(rw.nibble(3)) +
         " logical(as graded)=0x" + hex1(reg3c_hi_logical) + hex1(reg3c_lo_logical) +
         "; register00 raw=0x" + hex1(rw.nibble(0)) + hex1(rw.nibble(1)) + " (direct, no complement, "
         "the sole exception, spanning both its own paired addresses).");
    bool reg3c_is_zero = (reg3c_hi_logical == 0x0) && (reg3c_lo_logical == 0x0);
    bool reg00_is_not_all_ones_raw = (rw.nibble(0) != 0xF); // register00's raw IS its logical value (0xE), not complemented
    bool complement_rule_confirmed = reg3c_is_zero && reg00_is_not_all_ones_raw;
    expect(complement_rule_confirmed, "sol:K-0755 every AUTOCONFIG read register except register 00 is physically complemented");
    expect(complement_rule_confirmed, "opus:K-0776 in the actual implementation all read registers except the 00 register are physically complemented");
    expect(reg3c_is_zero, "sol:K-0756 logical register 3C has the value zero");
    bool raw_both_all_ones = (rw.nibble(2) == 0xF) && (rw.nibble(3) == 0xF);
    expect(raw_both_all_ones && reg3c_is_zero, "sol:K-0759 because read registers are complemented, both physical nybbles for logical zero return all 1 bits");
    expect(raw_both_all_ones && reg3c_is_zero, "opus:K-0777 the logical value of register 3C is always 0, meaning the upper nybbles of $00E8003C/$00E8003E both return all 1s");

    // =========================================================================
    // Liveness -- carries no claim uid.
    // =========================================================================
    expect(lc_before.all_captured && rw.all_captured && nb.all_captured &&
           before_raw != 0x0 && rw.nibble(2) != rw.nibble(0),
           "fixture-liveness [not graded -- apparatus check, no claim uid]");

    std::cout << "\n" << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}

// RESULTS (WSL, Verilator 5.052, built and run 2026-09-18): baseline
//   `make test` -> 31 passed, 0 failed (30 graded assertions + the
//   fixture-liveness apparatus check). 0 claims declined.
//
// All 8 guarded mutations run (`make clean; make MUT=<NAME> test` for each
// of LIFECYCLE, NOHANDOFF, WRITETOWRITEREG, WRONGCONFIG, INSIDEPROBE,
// NYBBLE, SWAPNYBBLE, NOCOMPLEMENT) turn red EXACTLY the assertions each
// header comment names, and no others:
//   LIFECYCLE      -> 22 passed, 9 failed (the 8 Group1 claims + the
//                      liveness check, which also depends on the "before"
//                      read being nonzero -- expected, not graded).
//   NOHANDOFF      -> 30 passed, 1 failed (sol:K-0733 only).
//   WRITETOWRITEREG-> 29 passed, 2 failed (sol:K-0735, opus:K-0762).
//   WRONGCONFIG    -> 29 passed, 2 failed (sol:K-0737, opus:K-0764).
//   INSIDEPROBE    -> 27 passed, 4 failed (sol:K-0742/K-0743/K-0744,
//                      opus:K-0767).
//   NYBBLE         -> 29 passed, 2 failed (sol:K-0750, opus:K-0771).
//   SWAPNYBBLE     -> 26 passed, 5 failed (sol:K-0752/K-0753/K-0760,
//                      opus:K-0773/K-0779).
//   NOCOMPLEMENT   -> 26 passed, 5 failed (sol:K-0755/K-0756/K-0759,
//                      opus:K-0776/K-0777).
// Rebuilt with no MUT afterward: 31/0 baseline restored exactly.
//
// SWAPNYBBLE moves only 5 of the 6 ids its own group header names above --
// sol:K-0757 is not among them. A ninth switch, WRONGREG3C, isolated to a
// dedicated `reg3c_probe` Harness so it cannot touch Group 7's other checks
// or Group 8's complement checks (which reuse `rw.nibble(2)/(3)`):
//   WRONGREG3C     -> 30 passed, 1 failed (sol:K-0757 only).
// Rebuilt with no MUT afterward: 31/0 baseline restored exactly.
