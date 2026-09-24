// Test purpose: pp.433-434 Appendix K "REGISTER BIT ASSIGNMENTS": register
//   00 bits 2:0 (board size code), register 08 bit 5 (the size-extension
//   selector), and register 08 bits 3:0 (sub-size).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 433, 434
// Non-goals: anything outside the `cpu_wrapper` fixture this package builds,
//   and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/appK-build/appK-autoconfig-size-encoding.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// appK-autoconfig-size-encoding -- pp.433-434 Appendix K "REGISTER BIT
// ASSIGNMENTS": register 00 bits 2:0 (board size code), register 08 bit 5
// (the size-extension selector), and register 08 bits 3:0 (sub-size). 21
// claims, filtered from
// hrm-testpipeline/work/appK-build/appK-autoconfig-size-encoding.jsonl.
//
// See SPECIFICATION.md for the full boundary investigation. In short: this
// package reuses hrm/appK-autoconfig-register-map's fixture (Harness
// mechanism, TG68KdotC_Kernel core, active-low reset, direct/complement
// arithmetic) and extends it with ONE new capability that fixture didn't
// need: writing an AUTOCONFIG register (MOVE.W #imm,(abs).L, opcode
// 0x33FC), because the 128MB Zorro III size code is never emitted until
// after the first board's base-address write (register 44) completes.
// -DENABLE_RAM32=1 (this package's own Makefile) is a real per-board
// `define (icepi-zero, icesugar-pro, two mistle targets) needed to reach
// the "8MB" Zorro-II size code at all in this design.
//
// 6 claims declined -- 4 because the printed row names two size
// interpretations (unextended/extended) and this ENABLE_RAM32 build only
// ever emits ONE of the two for that particular code (the other column is
// never produced by any case arm this build compiles), 2 because the
// "aligned on natural boundaries"
// claim needs a read-back or a live RAM-model fixture this boundary can't
// safely provide (write-only registers, no internal probe permitted). See
// SPECIFICATION.md, "Declined claims". 15 claims graded.

// MUTATION opus:K-0796 stimulus red: MUT=WRONGSIZE (see WRONGSIZE block below)
// MUTATION opus:K-0808 stimulus red: MUT=WRONGSIZE
// MUTATION sol:K-0779 stimulus red: MUT=WRONGSIZE (the "4MB" half; the
//   "reserved in extended mode" half is `none impossible`: cpu_wrapper.v's
//   Zorro III index-1 arm (:596) only ever assigns 4'b0011/4'b0100 -- code
//   111 is never assigned by any case arm, checked in full.)
// MUTATION opus:K-0810 stimulus red: MUT=WRONGSIZE
// MUTATION opus:K-0803 stimulus red: MUT=WRONGSTAGE (see WRONGSTAGE block below)
// MUTATION opus:K-0805 stimulus red: MUT=WRONGSTAGE
// MUTATION sol:K-0786 stimulus red: MUT=WRONGCONFIG (see WRONGCONFIG block below)
// MUTATION sol:K-0787 stimulus red: MUT=WRONGCONFIG
// MUTATION sol:K-0788 stimulus red: MUT=WRONGCONFIG
// MUTATION opus:K-0795 stimulus red: MUT=WRONGCONFIG
// MUTATION opus:K-0819 stimulus red: MUT=WRONGCONFIG
// MUTATION sol:K-0771 stimulus red: MUT=WRONGCONFIG
// MUTATION opus:K-0794 stimulus red: MUT=WRONGCONFIG
// MUTATION sol:K-0793 none impossible: cpu_wrapper.v:599 (Zorro III,
//   hardcoded 4'b1111) and the cpu_wrapper.v:547 default (Zorro II) are the
//   only two paths reaching chip_addr index 5 in this design, and both give
//   the same constant -- no input changes it.
// MUTATION opus:K-0827 none impossible: same as sol:K-0793.

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

// General-purpose Harness: a strictly-ordered program of READ (capture to a
// fresh scratch slot) and WRITE_IMM (MOVE.W #imm,(addr).L) operations,
// executed through cpu_wrapper's own ports only -- same mechanism as
// hrm/appK-autoconfig-register-map's Harness, generalised to also support
// writes (that fixture never needed to write anything).
struct Op {
    bool is_write;
    int  idx;      // chip_addr[6:1] index (both READ and WRITE_IMM)
    uint16_t imm;  // WRITE_IMM only
};
Op READ(int idx) { return Op{false, idx, 0}; }
Op WRITE_IMM(int idx, uint16_t imm) { return Op{true, idx, imm}; }

struct Harness {
    Vcpu_wrapper* top;
    std::map<uint32_t, uint16_t> mem;
    std::vector<uint32_t> read_scratch; // one per READ op, in program order
    std::vector<uint16_t> word;         // full captured 16-bit word per READ
    bool all_captured = false;

    void tick() { top->clk = 0; top->eval(); top->clk = 1; top->eval(); }

    Harness(uint8_t cpucfg_v, uint32_t fastramcfg_v, const std::vector<Op>& ops) {
        top = new Vcpu_wrapper;
        mem[0x000000] = 0x0001; mem[0x000002] = 0x0000; // SSP = 0x00010000
        mem[0x000004] = 0x0000; mem[0x000006] = 0x1000; // PC  = 0x00001000

        uint32_t pc = 0x1000;
        int nreads = 0;
        for (auto& op : ops) {
            uint32_t target = autocfg_addr(op.idx);
            if (!op.is_write) {
                uint32_t s = 0x002000 + 2u * (uint32_t)nreads++;
                read_scratch.push_back(s);
                // MOVE.W (target).L,D0
                mem[pc] = 0x3039; mem[pc+2] = (uint16_t)(target>>16); mem[pc+4] = (uint16_t)(target&0xFFFF);
                pc += 6;
                // MOVE.W D0,(s).L
                mem[pc] = 0x33C0; mem[pc+2] = (uint16_t)(s>>16); mem[pc+4] = (uint16_t)(s&0xFFFF);
                pc += 6;
            } else {
                // MOVE.W #imm,(target).L
                mem[pc] = 0x33FC; mem[pc+2] = op.imm;
                mem[pc+4] = (uint16_t)(target>>16); mem[pc+6] = (uint16_t)(target&0xFFFF);
                pc += 8;
            }
        }
        mem[pc] = 0x60FE; // BRA.S * -- infinite loop, end of program

        top->reset = 0; // active-low: held in reset
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

// idx <= 1 direct, everything else complemented -- see SPECIFICATION.md.
static uint8_t logical(int idx, uint8_t rawnib) {
    return (idx <= 1) ? rawnib : (uint8_t)((~rawnib) & 0xF);
}
static bool bit(uint8_t nibble, int n) { return ((nibble >> n) & 1) != 0; }

int main() {
    // H1: Zorro II, fastramcfg_d=1 -- "2MB" arm (cpu_wrapper.v:572).
    // reads: [0]=reg00(idx0,type,sanity) [1]=reg01(idx1,size) [2]=reg08hi(idx4,bit5) [3]=reg08lo(idx5,subsize)
    Harness h1(0, 1, {READ(0), READ(1), READ(4), READ(5)});
    // H2: Zorro II, fastramcfg_d=2 -- "4MB" arm (cpu_wrapper.v:573).
    Harness h2(0, 2, {READ(1)});
    // H3: Zorro II, fastramcfg_d=3 -- falls to `default`; ENABLE_RAM32 (this
    // package's Makefile) makes this "8MB" (cpu_wrapper.v:575).
    Harness h3(0, 3, {READ(1)});
    // H4: Zorro III, cpucfg=2 (cpucfg[1]=1), fastramcfg=5 (bit2=1 for
    // cfg_z3, bit0=1 so the post-write autocfg_card state stays nonzero
    // -- {fastramcfg_d[0],1'b0} = 2'b10 -- letting a second read observe
    // the "second board" 128MB code instead of being silenced).
    // reads: [0]=reg00(idx0,before) [1]=reg01(idx1,before,"256MB")
    //        [2]=reg08hi(idx4,bit5) ; then WRITE reg44 ; [3]=reg01(idx1,after,"128MB")
    Harness h4(2, 5, {READ(0), READ(1), READ(4), WRITE_IMM(34, 0x1000), READ(1)});
#ifdef MUT_WRONGCONFIG
    // MUTATION sol:K-0786/K-0787/K-0788/opus:K-0795/K-0819/sol:K-0771/
    // opus:K-0794 WRONGCONFIG stimulus red: swap which harness's bit5/size
    // readings feed the ZII-context vs ZIII-context assertions below.
    #define ZII_CTX h4
    #define ZIII_CTX h1
#else
    #define ZII_CTX h1
    #define ZIII_CTX h4
#endif

    note("fixture construction: h1.all_captured=" + std::string(h1.all_captured?"true":"false") +
         " h2=" + std::string(h2.all_captured?"true":"false") +
         " h3=" + std::string(h3.all_captured?"true":"false") +
         " h4=" + std::string(h4.all_captured?"true":"false") +
         " -- all must be true for any assertion below to mean anything.");

    // =========================================================================
    // Liveness -- carries no claim uid.
    // =========================================================================
    expect(h1.all_captured && h2.all_captured && h3.all_captured && h4.all_captured &&
           h1.nibble(1) != h2.nibble(0) && h2.nibble(0) != h3.nibble(0) &&
           h4.word[1] != h4.word[3],
           "fixture-liveness [not graded -- apparatus check, no claim uid]");
    note("liveness detail: h1 reg01 raw=0x" + hex1(h1.nibble(1)) +
         " h2 reg01 raw=0x" + hex1(h2.nibble(0)) +
         " h3 reg01 raw=0x" + hex1(h3.nibble(0)) +
         " h4 reg01 before=0x" + hex1((uint8_t)((h4.word[1]>>12)&0xF)) +
         " after=0x" + hex1((uint8_t)((h4.word[3]>>12)&0xF)) +
         " -- all distinct, read through real reset->fetch->execute->bus sequences.");

    // =========================================================================
    // Register 00 bits 2:0 -- size code (chip_addr index 1, direct).
    // =========================================================================
    uint8_t h1_size = logical(1, h1.nibble(1)) & 0x7; // ZII "2MB" (h1's 2nd read, position 1)
    uint8_t h2_size = logical(1, h2.nibble(0)) & 0x7; // ZII "4MB" (h2's only read, position 0)
    uint8_t h3_size = logical(1, h3.nibble(0)) & 0x7; // ZII "8MB" (RAM32) (h3's only read, position 0)
    uint8_t h4_size_before = logical(1, (uint8_t)((h4.word[1]>>12)&0xF)) & 0x7; // ZIII "256MB"
    uint8_t h4_size_after  = logical(1, (uint8_t)((h4.word[3]>>12)&0xF)) & 0x7; // ZIII "128MB" (2nd board)
    note("register00 bits2:0 size codes: ZII 2MB-arm=0b" + std::to_string((h1_size>>2)&1) + std::to_string((h1_size>>1)&1) + std::to_string(h1_size&1) +
         " ZII 4MB-arm=0b" + std::to_string((h2_size>>2)&1) + std::to_string((h2_size>>1)&1) + std::to_string(h2_size&1) +
         " ZII 8MB-arm(RAM32)=0b" + std::to_string((h3_size>>2)&1) + std::to_string((h3_size>>1)&1) + std::to_string(h3_size&1) +
         " ZIII before-write=0b" + std::to_string((h4_size_before>>2)&1) + std::to_string((h4_size_before>>1)&1) + std::to_string(h4_size_before&1) +
         " ZIII after-write=0b" + std::to_string((h4_size_after>>2)&1) + std::to_string((h4_size_after>>1)&1) + std::to_string(h4_size_after&1));

#ifdef MUT_WRONGSIZE
    // MUTATION opus:K-0796/K-0808/sol:K-0779/opus:K-0810 WRONGSIZE stimulus
    // red: rotate which harness's size code feeds which assertion.
    uint8_t code_2mb = h2_size, code_4mb = h3_size, code_8mb = h1_size;
#else
    uint8_t code_2mb = h1_size, code_4mb = h2_size, code_8mb = h3_size;
#endif
    bool is_2mb = (code_2mb == 0b110);
    bool is_4mb = (code_4mb == 0b111);
    bool is_8mb = (code_8mb == 0b000);

    expect(is_8mb, "opus:K-0796 register00 size code 000 means 8 megabytes when unextended");
    record("sol:K-0772 NOT GRADED (p433): 'Size code 000 denotes 8 MiB "
           "unextended or 16 MiB extended.' The unextended half is graded "
           "above (graded: opus:K-0796). The extended (16MB) half is never emitted "
           "by this design: cpu_wrapper.v:596's Zorro III index-1 arm only "
           "ever assigns 4'b0011 or 4'b0100 (codes 011/100) -- code 000 is "
           "never reached by any Zorro III configuration this fixture -- or "
           "any fixture built on cpu_wrapper's own ports -- can drive. "
           "reason_kind: partial-untested-half.");

    expect(is_2mb, "opus:K-0808 register00 size code 110 means 2 megabytes when unextended");
    record("sol:K-0778 NOT GRADED (p433): 'Size code 110 denotes 2 MiB "
           "unextended or 1 GiB extended.' The unextended half is graded "
           "above (graded: opus:K-0808). The extended (1GB) half is never emitted: "
           "cpu_wrapper.v:596 only ever assigns codes 011/100 in Zorro III "
           "mode, never 110. reason_kind: partial-untested-half.");

    bool reserved_never_emitted_111_ziii =
        (h4_size_before != 0b111) && (h4_size_after != 0b111);
    expect(is_4mb && reserved_never_emitted_111_ziii,
           "sol:K-0779 register00 size code 111 means 4MB unextended and is reserved (never emitted) in extended mode");
    expect(is_4mb, "opus:K-0810 register00 size code 111 means 4 megabytes when unextended");

    // =========================================================================
    // Register 00 bits 2:0 -- Zorro III 128MB/256MB, before/after the
    // register-44 write that advances autocfg_card. chip_addr index 1,
    // direct, Zorro III arm only.
    // =========================================================================
#ifdef MUT_WRONGSTAGE
    // MUTATION opus:K-0803/K-0805 stimulus red: MUT=WRONGSTAGE swap which read
    // (before/after the register-44 write) feeds which assertion.
    uint8_t code_256mb = h4_size_after, code_128mb = h4_size_before;
#else
    uint8_t code_256mb = h4_size_before, code_128mb = h4_size_after;
#endif
    expect(code_256mb == 0b100, "opus:K-0805 register00 size code 100 means 256 megabytes when extended");
    record("sol:K-0776 NOT GRADED (p433): 'Size code 100 denotes 512 KiB "
           "unextended or 256 MiB extended.' The extended half is graded "
           "above (graded: opus:K-0805). The unextended (512KB) half is never "
           "emitted: cpu_wrapper.v:572-575, the Zorro II fastramcfg_d cases this "
           "ENABLE_RAM32 build compiles, "
           "only ever assign codes 110/111/000, never 100. "
           "reason_kind: partial-untested-half.");

    expect(code_128mb == 0b011, "opus:K-0803 register00 size code 011 means 128 megabytes when extended");
    record("sol:K-0775 NOT GRADED (p433): 'Size code 011 denotes 256 KiB "
           "unextended or 128 MiB extended.' The extended half is graded "
           "above (graded: opus:K-0803). The unextended (256KB) half is never "
           "emitted: cpu_wrapper.v:572-575, the Zorro II fastramcfg_d cases this "
           "ENABLE_RAM32 build compiles, "
           "only ever assign codes 110/111/000, never 011. "
           "reason_kind: partial-untested-half.");

    // =========================================================================
    // Register 08 bit 5 -- size-extension selector. chip_addr index 4
    // (bit 5 of the byte = bit 1 of the nibble after complement).
    // Zorro II: unmapped -> default -> logical 0000 -> bit5=0.
    // Zorro III: cpu_wrapper.v:598 -> logical 1111 -> bit5=1.
    // =========================================================================
    uint8_t zii_reg08 = logical(4, ZII_CTX.nibble(2));
    uint8_t ziii_reg08 = logical(4, ZIII_CTX.nibble(2));
    bool zii_bit5 = bit(zii_reg08, 1);
    bool ziii_bit5 = bit(ziii_reg08, 1);
    note("register08 bit5 (size-extension selector), chip_addr index 4: "
         "Zorro-II-context logical nibble=0x" + hex1(zii_reg08) + " bit5=" + std::to_string(zii_bit5) +
         "; Zorro-III-context logical nibble=0x" + hex1(ziii_reg08) + " bit5=" + std::to_string(ziii_bit5));

    bool bit5_selects = (!zii_bit5) && ziii_bit5;
    expect(bit5_selects, "sol:K-0786 register08 bit5 is the size-extension selector (0=ZII table, 1=ZIII table observed)");
    expect(bit5_selects, "opus:K-0795 register08 bit5 selects Zorro III size interpretation, the new meaning of bit5");
    expect(!zii_bit5, "sol:K-0787 register08 bit5 clear -> register00 size bits use Zorro II meanings (Zorro II context observed)");
    expect(ziii_bit5, "sol:K-0788 register08 bit5 set -> register00 size bits indicate a new (Zorro III) size (Zorro III context observed)");
    expect(!zii_bit5 && ziii_bit5, "opus:K-0819 register08 bit5: clear->Zorro II meanings, set->new (Zorro III) size, both observed");

    bool both_interpretations_exercised = bit5_selects;
    expect(both_interpretations_exercised,
           "sol:K-0771 register00 bits2:0 encode board size, with register08 bit5 selecting unextended/extended interpretation");
    expect(both_interpretations_exercised,
           "opus:K-0794 register00 bits2-0 indicate the PIC's configuration size");

    // =========================================================================
    // Register 08 bits 3:0 -- sub-size, chip_addr index 5. Hardcoded 4'b1111
    // in both configs (cpu_wrapper.v:599 Zorro III, :547 default Zorro II).
    // =========================================================================
    uint8_t subsize = logical(5, h1.nibble(3));
    note("register08 bits3:0 (sub-size), chip_addr index 5: logical nibble=0x" + hex1(subsize));
    bool subsize_matches_physical = (subsize == 0x0);
    expect(subsize_matches_physical, "sol:K-0793 register08 sub-size code 0000 means logical size matches physical size");
    expect(subsize_matches_physical, "opus:K-0827 register08 sub-size code 0000 means logical size matches physical size");

    // =========================================================================
    // Declined: alignment (no read-back boundary for write-only base regs).
    // =========================================================================
    record("sol:K-0789 NOT GRADED (p433): 'A card using extended size "
           "encoding is aligned on its natural boundary.' Registers 44/48 "
           "are write-only with no read-back path (confirmed in "
           "hrm/appK-autoconfig-base-address's boundary investigation). "
           "Observing whether a written base is actually forced onto its "
           "natural alignment boundary needs either an internal probe of "
           "z3ram_base0/z3ram_base1 (forbidden) or a live RAM-model fixture "
           "driving ramsel/ramready, which this fixture's permanently-low "
           "ramready cannot safely exercise without hanging (two prior "
           "attempts at this fixture already hung on a related reset-"
           "polarity mistake; this is the same category of risk). "
           "reason_kind: unreachable-observable, boundary: an internal "
           "z3ram_base0/z3ram_base1 probe, or a RAM-model fixture.");
    record("opus:K-0820 NOT GRADED (p433, same claim as sol:K-0789): "
           "reason_kind: unreachable-observable, same boundary.");

    std::cout << "\n" << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}

// RESULTS (WSL, Verilator 5.052, built and run 2026-09-18): baseline
//   `make test` -> 16 passed, 0 failed (15 graded assertions + the
//   fixture-liveness apparatus check). 6 claims declined (record()).
//
// MUTATION WRONGSIZE (`make MUT=WRONGSIZE test`, after `make clean`):
//   12 passed, 4 failed -- turns red exactly the 4 assertions the header
//   names as WRONGSIZE-sensitive (opus:K-0796, opus:K-0808, sol:K-0779,
//   opus:K-0810). Rebuilt with no MUT afterward: 16/0 baseline restored.
//
// MUTATION WRONGSTAGE (`make MUT=WRONGSTAGE test`, after `make clean`):
//   14 passed, 2 failed -- turns red exactly opus:K-0805 and opus:K-0803.
//
// MUTATION WRONGCONFIG (`make MUT=WRONGCONFIG test`, after `make clean`):
//   9 passed, 7 failed -- turns red exactly the 7 assertions the header
//   names as WRONGCONFIG-sensitive (sol:K-0786, opus:K-0795, sol:K-0787,
//   sol:K-0788, opus:K-0819, sol:K-0771, opus:K-0794).
//
// A note on a bug this fixture caught in itself: the first `make test`
// run showed h2 (fastramcfg_d=2, expected size code 111/"4MB") and h3
// (fastramcfg_d=3, RAM32 default/"8MB") both reading raw nibble 0x0,
// which would have been misreported as a genuine RTL divergence. It
// was not one: h2/h3 each issue only ONE AUTOCONFIG read (unlike h1's
// four), so their captured value lives at word-vector position 0, not
// position 1 -- an off-by-one in this test file's own indexing
// (`h2.nibble(1)` instead of `h2.nibble(0)`), not a fact about
// cpu_wrapper. Caught by comparing against the RTL's own case-statement
// text before accepting the result, fixed, and re-measured -- kept here
// as the record of that check, per "measurement first, then the
// write-up," and because a wrong red is exactly the kind of result a
// oracle check is supposed to catch before it is reported as a finding.
