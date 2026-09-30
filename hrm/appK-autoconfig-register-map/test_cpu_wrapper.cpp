// Test purpose: pp.432-434 Appendix K "REGISTER BIT ASSIGNMENTS" (the
//   Zorro-II/Zorro-III AUTOCONFIG register table), against cpu_wrapper's own
//   real AUTOCONFIG responder.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 432, 434
// Non-goals: anything outside the `cpu_wrapper` fixture this package builds,
//   and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/appK-build/appK-autoconfig-register-map.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// appK-autoconfig-register-map -- pp.432-434 Appendix K "REGISTER BIT
// ASSIGNMENTS" (the Zorro-II/Zorro-III AUTOCONFIG register table), against
// cpu_wrapper's own real AUTOCONFIG responder. 47 claims, filtered from
// hrm-testpipeline/work/appK-build/appK-autoconfig-register-map.jsonl.
//
// See SPECIFICATION.md for the full boundary investigation, the claim
// inventory, and the direct/complement arithmetic this file relies on. In
// short:
//
//   - cpu_wrapper's bus ports (chip_addr/chip_din/chip_rw/chip_uds/chip_lds/
//     chip_as) are all outputs, generated internally by whichever CPU core
//     the module instantiates. The default core (fx68k) fails this
//     project's pinned Verilator under its default checks (measured:
//     %Error-BLKANDNBLK, fx68k.sv:293); it builds with -Wno-BLKANDNBLK and
//     runs with --no-assert (SPECIFICATION.md, item 1). The other core
//     cpu_wrapper.v can select -- TG68KdotC_Kernel, built from nanomig/sim/
//     (the Verilator-compatible translation cpu_wrapper.v's own `ifdef
//     VERILATOR branch already names) -- elaborates, and six of NanoMig's
//     twelve board tops select it, among them all four that define
//     ENABLE_RAM32. The core is chosen by the Makefile; see
//     SPECIFICATION.md. This fixture drives cpu_wrapper with a
//     real, hand-assembled 68000 program executed through the module's own
//     ports only -- no internal signal is read or forced anywhere below.
//   - Every AUTOCONFIG register nibble lives at chip_addr[6:1] indices 0 and
//     1 *directly* (uninverted); every other index is the one's *complement*
//     of its logical value. Both halves of this rule are re-derived in
//     SPECIFICATION.md directly from the RTL's own case-statement comments
//     (manufacturer ID 0x139c, product ID 0x10, serial=1, register 08's
//     Zorro-III flags, register 00's board-size codes), not copied from
//     classify.evidence.
//   - 3 claims (opus:K-0784, opus:K-0812, opus:K-0815) assert nothing but a
//     literal Zorro-III physical address (100/104/108) that this RTL's own
//     sel_autoconfig decode never reaches (it reuses the $E80000-E8007E
//     window for its "Zorro III" case branch, selected by cfg_z3, not a
//     distinct $E80100+ decode) -- declined. 2 claims (sol:K-0768,
//     opus:K-0791 p432) assert a causal "register 08 modifies computed size"
//     relationship this RTL does not implement (register 08 has no write
//     handler at all, and the "logical size" register is an independent
//     static constant) -- declined. 42 claims graded, 5 declined.
//
// MUTATION METHODOLOGY. Every register-content value graded below (PIC type
// aside) is a hardcoded case-statement literal with no input port that can
// perturb it -- cpu_wrapper's only externally-driven configuration inputs
// affecting the AUTOCONFIG read path are cpucfg/fastramcfg (which select
// *which* whole case branch -- Zorro II vs Zorro III -- applies), and (for
// register 00's low nibble only) fastramcfg's board-size sub-selection. For
// every such fixed-constant assertion the mutation is `none impossible`,
// naming the exact cpu_wrapper.v line(s) holding the literal -- confirmed by
// reading the case statement, not asserted from habit. Where switching
// cpucfg[1]/fastramcfg[2] (Zorro II vs Zorro III) genuinely does change the
// value under test -- register 00's PIC-type-code assertions -- a real
// guarded mutation exists (`make MUT=WRONGCONFIG test`): it feeds the
// Zorro-III harness's captured nibble into the Zorro-II-context assertions
// and vice versa, which does turn those assertions red (see MUTATION lines
// below and the RESULTS section at the end of this file).
//
// MUTATION sol:K-0766 stimulus red: MUT=WRONGCONFIG (see WRONGCONFIG block below)
// MUTATION opus:K-0789 stimulus red: MUT=WRONGCONFIG
// MUTATION sol:K-0765 stimulus red: MUT=WRONGCONFIG
// MUTATION opus:K-0788 stimulus red: MUT=WRONGCONFIG
// MUTATION sol:K-0762 stimulus red: MUT=WRONGCONFIG
// MUTATION opus:K-0785 stimulus red: MUT=WRONGCONFIG
// MUTATION sol:K-0763 none impossible: cpu_wrapper.v:569 (4'b1110) and :595
//   (4'b1010) are the only two literal assignments to chip_addr index 0 in
//   this design, in either reachable configuration; no third value (00) is
//   ever written by any code path, so no stimulus can produce it.
// MUTATION opus:K-0786 none impossible: same as sol:K-0763.
// MUTATION sol:K-0764 none impossible: same reasoning as sol:K-0763, for
//   code 01 -- cpu_wrapper.v:569/:595 are the only two constants.
// MUTATION opus:K-0787 none impossible: same as sol:K-0764.
// MUTATION sol:K-0767 none impossible: bit5=1 in both cpu_wrapper.v:569
//   (4'b1110) and :595 (4'b1010) -- hardcoded in both reachable branches.
// MUTATION opus:K-0790(p432) none impossible: same as sol:K-0767.
// MUTATION opus:K-0790 none impossible: bit5=1 in both cpu_wrapper.v:569
//   (4'b1110) and :595 (4'b1010), the only two literal assignments to
//   chip_addr index 0.
// MUTATION sol:K-0769 none impossible: bit4=0 in both cpu_wrapper.v:569 and
//   :595 -- hardcoded in both reachable branches.
// MUTATION opus:K-0792 none impossible: same as sol:K-0769.
// MUTATION sol:K-0770 none impossible: bit3=0 in every case arm that can
//   reach chip_addr index 1 -- cpu_wrapper.v:572,573,575 (Zorro II, all
//   three fastramcfg_d arms) and :596 (Zorro III, both autocfg_card[1] arms)
//   -- five hardcoded constants, none sets bit3.
// MUTATION opus:K-0793 none impossible: same as sol:K-0770.
// MUTATION sol:K-0780 none impossible: cpu_wrapper.v:597, 4'b1110, is the
//   only assignment to chip_addr index 2 in this design.
// MUTATION opus:K-0813 none impossible: same as sol:K-0780.
// MUTATION sol:K-0782 none impossible: cpu_wrapper.v:598, 4'b0000, is the
//   only Zorro-III assignment to chip_addr index 4; bit7 is hardcoded 1
//   after complement.
// MUTATION sol:K-0783 none impossible: chip_addr index 4 has no Zorro-II
//   case arm at all (cpu_wrapper.v:568-590's case list has no 6'b000100
//   entry), so it falls to the unconditional default at :547 (4'b1111) in
//   every Zorro-II run; bit7 is hardcoded 0 after complement.
// MUTATION opus:K-0817 none impossible: same two constants as sol:K-0782/
//   sol:K-0783 (cpu_wrapper.v:598 and the :547 default).
// MUTATION sol:K-0784(p433) none impossible: same as sol:K-0782 (bit6 of
//   the same cpu_wrapper.v:598 constant).
// MUTATION sol:K-0784 none impossible: bit6 of cpu_wrapper.v:598 (4'b0000,
//   the only Zorro-III assignment to chip_addr index 4) is hardcoded.
// MUTATION sol:K-0785(p433) none impossible: same as sol:K-0783 (bit6 of
//   the :547 default).
// MUTATION sol:K-0785 none impossible: chip_addr index 4 has no Zorro-II
//   case arm (cpu_wrapper.v:568-590, no 6'b000100 entry) so it falls to
//   the unconditional :547 default (4'b1111), hardcoded.
// MUTATION opus:K-0818(p433) none impossible: same two constants.
// MUTATION opus:K-0818 none impossible: same two constants as sol:K-0784/
//   sol:K-0785 above: cpu_wrapper.v:598 (4'b0000) and the :547 default
//   (4'b1111), both hardcoded.
// MUTATION sol:K-0790(p433) none impossible: bit4 of cpu_wrapper.v:598,
//   hardcoded 1 after complement; no Zorro-II case arm at index 4 exists to
//   compare against (the claim is scoped to Zorro III on the page itself).
// MUTATION sol:K-0790 none impossible: bit4 of cpu_wrapper.v:598 (4'b0000)
//   is hardcoded.
// MUTATION opus:K-0821(p433) none impossible: same as sol:K-0790(p433).
// MUTATION opus:K-0821 none impossible: same constant as sol:K-0790 above:
//   bit4 of cpu_wrapper.v:598 (4'b0000), hardcoded.
// MUTATION sol:K-0791(p433) none impossible: bits3:0 of cpu_wrapper.v:598
//   (Zorro III, hardcoded 1111 after complement) and the :547 default
//   (Zorro II, hardcoded 0000 after complement).
// MUTATION sol:K-0791 none impossible: bits3:0 of cpu_wrapper.v:598
//   (Zorro III) and the :547 default (Zorro II) are both hardcoded
//   literals.
// MUTATION opus:K-0822 none impossible: same as sol:K-0791(p433).
// MUTATION sol:K-0814 none impossible: chip_addr index 6 (and 7) never
//   appears in any case list in this file (cpu_wrapper.v:568-606 checked in
//   full) -- it falls to the unconditional :547 default in every
//   configuration this design offers.
// MUTATION opus:K-0848 none impossible: same as sol:K-0814.
// MUTATION sol:K-0815 none impossible: cpu_wrapper.v:584-585 (Zorro II)
//   and :600-601 (Zorro III) both assign 4'b1110/4'b1100 to chip_addr
//   indices 8/9 -- no other case arm ever does.
// MUTATION opus:K-0849 none impossible: same as sol:K-0815.
// MUTATION sol:K-0816 none impossible: cpu_wrapper.v:586-587 (Zorro II)
//   and :602-603 (Zorro III) both assign 4'b0110/4'b0011 to chip_addr
//   indices 10/11 -- no other case arm ever does.
// MUTATION opus:K-0850 none impossible: same as sol:K-0816.
// MUTATION sol:K-0818 none impossible: chip_addr index 12 never appears in
//   any case list -- unconditional :547 default in every configuration.
// MUTATION sol:K-0819 none impossible: chip_addr index 14 never appears in
//   any case list -- unconditional :547 default.
// MUTATION sol:K-0820 none impossible: chip_addr index 16 never appears in
//   any case list -- unconditional :547 default.
// MUTATION sol:K-0821 none impossible: cpu_wrapper.v:588 (Zorro II,
//   4'b1110) is a hardcoded literal; cpu_wrapper.v:604's Zorro-III formula
//   ({2'b11,~autocfg_card}) depends only on autocfg_card, itself a hardcoded
//   reset constant (cpu_wrapper.v:618, `autocfg_card <= 2'b01`) this fixture
//   never writes -- no exposed input reaches either path.
// MUTATION opus:K-0852 none impossible: same as sol:K-0818.
// MUTATION opus:K-0853 none impossible: same as sol:K-0819.
// MUTATION opus:K-0854 none impossible: same as sol:K-0820.
// MUTATION opus:K-0855 none impossible: same as sol:K-0821.

#include "Vcpu_wrapper.h"
#include "verilated.h"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <vector>

static int passed = 0, failed = 0;

// ---- the fixed harness verbs (character-identical to
// hrm/appDE-fire-buttons-userio/test_userio.cpp) ----
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

// One configuration's fixture: builds cpu_wrapper (+ its own embedded CPU
// core -- which one is chosen by the Makefile; see SPECIFICATION.md),
// assembles a tiny 68000 program that reads each
// requested AUTOCONFIG index in turn and writes the raw word straight back
// out to a scratch address, runs it to completion, and captures the raw
// nibble (bits 15:12 of the write) for each requested index -- entirely
// through cpu_wrapper's own ports (chip_dout/chip_dtack driven in;
// chip_addr/chip_rw/chip_din/chip_as observed out). No internal signal of
// cpu_wrapper or of the CPU core it instantiates is read or forced anywhere
// in this file.
struct Harness {
    Vcpu_wrapper* top;
    std::map<uint32_t, uint16_t> mem;
    std::vector<int> indices;
    std::vector<uint8_t> raw;   // one nibble per index, in `indices` order
    bool all_captured = false;

    // tick()/settle(): same bodies as test_userio.cpp's tick()/settle(), as
    // Harness members rather than free functions on one global `top` --
    // this package, unlike userio, needs two independent DUT instances (one
    // per Zorro configuration), each owning its own embedded CPU core and
    // memory model. settle() is kept for parity with the suite's harness
    // shape even though the main run loop below cannot use it directly: it
    // must update chip_dout (this fixture's memory-model response) and the
    // ph1/ph2 phase every single cycle, which a bare N-tick settle cannot
    // do -- unlike userio, cpu_wrapper's fixture contains a real, free-
    // running CPU core that must be served a live bus on every cycle.
    void tick() { top->clk = 0; top->eval(); top->clk = 1; top->eval(); }
    void settle(int n) { for (int i = 0; i < n; i++) tick(); }

    Harness(uint8_t cpucfg_v, uint32_t fastramcfg_v, const std::vector<int>& idxs)
        : indices(idxs) {
        top = new Vcpu_wrapper;

        mem[0x000000] = 0x0001; mem[0x000002] = 0x0000; // initial SSP = 0x00010000
        mem[0x000004] = 0x0000; mem[0x000006] = 0x1000; // initial PC  = 0x00001000

        uint32_t pc = 0x1000;
        std::vector<uint32_t> scratch;
        for (size_t i = 0; i < indices.size(); i++) {
            uint32_t target = autocfg_addr(indices[i]);
            uint32_t s = 0x002000 + 2u * (uint32_t)i;
            scratch.push_back(s);
            // MOVE.W (target).L,D0
            mem[pc]     = 0x3039;
            mem[pc + 2] = (uint16_t)(target >> 16);
            mem[pc + 4] = (uint16_t)(target & 0xFFFF);
            pc += 6;
            // MOVE.W D0,(s).L
            mem[pc]     = 0x33C0;
            mem[pc + 2] = (uint16_t)(s >> 16);
            mem[pc + 4] = (uint16_t)(s & 0xFFFF);
            pc += 6;
        }
        mem[pc] = 0x60FE; // BRA.S * -- infinite loop, end of program

        top->reset = 0;        // active-low here: held in reset
        top->clk = 0; top->ph1 = 0; top->ph2 = 0;
        top->cpucfg = cpucfg_v;
        top->fastramcfg = fastramcfg_v;
        top->chipramcfg = 0;
        top->slowramcfg = 0;
        top->turbocfg = 0;     // ramsel stays permanently false -- every
                                // access (vectors, fetches, AUTOCONFIG
                                // reads, scratch writes) goes through the
                                // chip_dtack/chip_dout path this fixture
                                // services directly
        top->bootrom = 0;
        top->chip_dout = 0;
        top->chip_dtack = 0;   // always "ready" (active low)
        top->chip_ipl = 7;     // no interrupt (IPL2:0 = 111)
        top->ramdout = 0;
        top->ramready = 0;
        top->eval();

        std::map<uint32_t, uint16_t> captured;
        bool last_as = true;
        const int RESET_CYCLES = 20;
        const int MAX_CYC = 60000;
        for (int cyc = 0; cyc < MAX_CYC && captured.size() < scratch.size(); cyc++) {
            if (cyc == RESET_CYCLES) top->reset = 1; // release reset (active-low)

            uint32_t ba = ((uint32_t)top->chip_addr) << 1;
            auto it = mem.find(ba & ~1u);
            top->chip_dout = (it != mem.end()) ? it->second : 0;
            top->eval();

            // Real amiga ph1/ph2 cadence (nanomig.v:144-179's own timing
            // table: ph2 once, then ph1 once, every 4 clk edges) driven
            // directly on cpu_wrapper's own ph1/ph2 ports.
            int phase = cyc % 4;
            top->ph1 = (phase == 3);
            top->ph2 = (phase == 1);
            tick();

            if (cyc >= RESET_CYCLES) {
                bool as_now = top->chip_as;
                if (last_as && !as_now && !top->chip_rw) {
                    uint32_t a = ((uint32_t)top->chip_addr) << 1;
                    if (std::find(scratch.begin(), scratch.end(), a) != scratch.end())
                        captured[a] = top->chip_din;
                }
                last_as = as_now;
            }
        }
        all_captured = (captured.size() == scratch.size());
        for (auto s : scratch)
            raw.push_back((uint8_t)((captured.count(s) ? captured[s] : 0) >> 12) & 0xF);
    }
    ~Harness() { top->final(); delete top; }

    uint8_t at(int idx) const {
        for (size_t i = 0; i < indices.size(); i++)
            if (indices[i] == idx) return raw[i];
        return 0xFF; // never returned for an index this Harness was asked to read
    }
};

// chip_addr index 0 and 1 carry the direct (uninverted) logical value;
// every other index carries the one's complement. See SPECIFICATION.md,
// "The register-map arithmetic this package relies on".
static uint8_t logical(int idx, uint8_t rawnib) {
    return (idx <= 1) ? rawnib : (uint8_t)((~rawnib) & 0xF);
}
static bool bit(uint8_t nibble, int n) { return ((nibble >> n) & 1) != 0; }

int main() {
    // Zorro II: cpucfg=0 (68000, cpucfg[1]=0 -> cfg_z3 forced 0 regardless
    // of fastramcfg), fastramcfg=1 (nonzero, required for sel_autoconfig to
    // ever assert; selects the "2MB" board-size arm at index 1).
    std::vector<int> zii_idx = {0, 1, 4, 6, 8, 9, 10, 11, 12, 14, 16, 18, 19};
    Harness zii(0, 1, zii_idx);

    // Zorro III: cpucfg[1]=1 and fastramcfg[2]=1 both required for
    // cfg_z3 = fastramcfg_d[2] & cpucfg[1] (cpu_wrapper.v:536) to assert.
    std::vector<int> ziii_idx = {0, 1, 2, 3, 4, 19};
    Harness ziii(2, 4, ziii_idx);

    note("fixture construction: Zorro-II harness all_captured=" +
         std::string(zii.all_captured ? "true" : "false") +
         ", Zorro-III harness all_captured=" +
         std::string(ziii.all_captured ? "true" : "false") +
         " -- both must be true for any assertion below to mean anything.");

    // =========================================================================
    // Liveness -- carries no claim uid, proves the round trip is not stuck.
    // =========================================================================
    uint8_t zii_idx0_raw = zii.at(0);
    uint8_t zii_idx4_raw = zii.at(4);
    note("liveness: Zorro-II raw nibble at index 0 (register 00 high) = 0x" +
         hex1(zii_idx0_raw) + ", at index 4 (register 08, unmapped in Zorro "
         "II) = 0x" + hex1(zii_idx4_raw) + " -- read through a real reset -> "
         "vector fetch -> instruction fetch -> decode -> execute -> bus "
         "read -> bus write sequence, no internal signal touched.");
    expect(zii.all_captured && ziii.all_captured && zii_idx0_raw != zii_idx4_raw,
           "fixture-liveness [not graded -- apparatus check, no claim uid]");

    // =========================================================================
    // GROUP A -- register 00 bits 7:6, PIC type code. chip_addr index 0,
    // direct (no complement). Graded with the correctly-configured harness;
    // MUT_WRONGCONFIG swaps which harness's nibble feeds these assertions.
    // =========================================================================
#ifdef MUT_WRONGCONFIG
    uint8_t a_zii_raw = ziii.at(0);   // MUTATION stimulus red: wrong harness's nibble
    uint8_t a_ziii_raw = zii.at(0);   // MUTATION stimulus red: wrong harness's nibble
#else
    uint8_t a_zii_raw = zii.at(0);
    uint8_t a_ziii_raw = ziii.at(0);
#endif
    uint8_t a_zii_type = (a_zii_raw >> 2) & 0x3;    // bits 7:6 of the nibble (nibble = byte bits 7:4)
    uint8_t a_ziii_type = (a_ziii_raw >> 2) & 0x3;
    note("register 00 bits7:6 (PIC type): Zorro-II raw nibble=0x" + hex1(a_zii_raw) +
         " -> type=" + std::to_string(a_zii_type) + "b" +
         "; Zorro-III raw nibble=0x" + hex1(a_ziii_raw) +
         " -> type=" + std::to_string(a_ziii_type) + "b (both direct, no complement).");

    bool type_is_11 = (a_zii_type == 0b11);
    expect(type_is_11, "sol:K-0766 register00 type=11 reads as Zorro II");
    expect(type_is_11, "opus:K-0789 register00 type=11 reads as Zorro II");

    bool type_is_10 = (a_ziii_type == 0b10);
    expect(type_is_10, "sol:K-0765 register00 type=10 reads as Zorro III");
    expect(type_is_10, "opus:K-0788 register00 type=10 reads as Zorro III");

    // Reserved codes 00/01: this design only ever writes 4'b1110 (ZII) or
    // 4'b1010 (ZIII) to this field (cpu_wrapper.v:569,595) -- exhaustive
    // over both of this design's reachable configurations, not a sample.
    bool never_00 = (a_zii_type != 0b00) && (a_ziii_type != 0b00);
    expect(never_00, "sol:K-0763 register00 type=00 (Reserved) never emitted, either config");
    expect(never_00, "opus:K-0786 register00 type=00 (Reserved) never emitted, either config");
    bool never_01 = (a_zii_type != 0b01) && (a_ziii_type != 0b01);
    expect(never_01, "sol:K-0764 register00 type=01 (Reserved) never emitted, either config");
    expect(never_01, "opus:K-0787 register00 type=01 (Reserved) never emitted, either config");

    bool type_field_correct = type_is_11 && type_is_10;
    expect(type_field_correct, "sol:K-0762 register00 bits7-6 encode PIC type (11=ZII,10=ZIII observed)");
    expect(type_field_correct, "opus:K-0785 register00 bits7-6 named er_Type encode PIC type (11=ZII,10=ZIII observed)");

    record("opus:K-0784 NOT GRADED (p432): 'Register 00 is register 02 in "
           "the Zorro II mapping and register 100 in the Zorro III mapping.' "
           "This claim's entire content is the literal address triple, with "
           "no accompanying bit-content to grade instead (contrast "
           "(graded: sol:K-0762/opus:K-0785) above, which state the same "
           "triple but also state the bit7:6 content, graded there). The quoted "
           "Zorro-III address ($E80100) is never reached by this design's "
           "own sel_autoconfig decode (cpu_wrapper.v:611 only ever matches "
           "chip_addr[23:16]==8'b11101000, and chip_addr[6:1] is 6 bits wide "
           "-- max byte offset 0x7E, never 0x100) at any input this fixture "
           "-- or any fixture built on cpu_wrapper's own ports -- can drive. "
           "reason_kind: unreachable-address.");

    // =========================================================================
    // GROUP B -- register 00 bit 5 (link memory into free pool), chip_addr
    // index 0, direct. Graded fact only; the causal "register 08 modifies
    // the size" claim is declined separately below.
    // =========================================================================
    bool bit5_zii = bit(a_zii_raw, 1);   // nibble bit1 == byte bit5
    bool bit5_ziii = bit(a_ziii_raw, 1);
    note("register00 bit5 (link memory): Zorro-II=" + std::to_string(bit5_zii) +
         " Zorro-III=" + std::to_string(bit5_ziii));
    bool bit5_set_both = bit5_zii && bit5_ziii;
    expect(bit5_set_both, "sol:K-0767 register00 bit5 set -> PIC memory linked into free pool");
    expect(bit5_set_both, "opus:K-0790 register00 bit5 set -> PIC memory linked into free pool");

    record("sol:K-0768 NOT GRADED (p432): 'The Zorro III register 08 may "
           "modify the size of the linked memory.' This is a causal/"
           "reactive claim (varying register 08's content changes a "
           "computed logical size). This RTL implements register 08 "
           "(chip_addr index 4, cpu_wrapper.v:598) and the 'logical size' "
           "register (index 5, cpu_wrapper.v:599) as independent static "
           "case-statement constants -- neither is a function of the other "
           "-- and register 08 has no write handler at all "
           "(cpu_wrapper.v:629-665 only handles writes to indices 0x24/0x22, "
           "the RAM-activation registers, never index 4): there is no live "
           "'modify' mechanism in this design to observe. "
           "reason_kind: unreachable-behaviour.");
    record("opus:K-0791 NOT GRADED (p432, duplicate of sol:K-0768): same "
           "claim, same reason -- reason_kind: unreachable-behaviour.");

    // =========================================================================
    // GROUP C -- register 00 bit 4 (autoboot ROM), chip_addr index 0, direct.
    // =========================================================================
    bool bit4_zii = bit(a_zii_raw, 0);
    bool bit4_ziii = bit(a_ziii_raw, 0);
    note("register00 bit4 (autoboot ROM): Zorro-II=" + std::to_string(bit4_zii) +
         " Zorro-III=" + std::to_string(bit4_ziii));
    bool bit4_clear_both = !bit4_zii && !bit4_ziii;
    expect(bit4_clear_both, "sol:K-0769 register00 bit4 clear -> no autoboot ROM, either config");
    expect(bit4_clear_both, "opus:K-0792 register00 bit4 clear -> no autoboot ROM, either config");

    // =========================================================================
    // GROUP D -- register 00 bit 3 (next board related), chip_addr index 1,
    // direct.
    // =========================================================================
    uint8_t zii_idx1_raw = zii.at(1);
    uint8_t ziii_idx1_raw = ziii.at(1);
    bool bit3_zii = bit(zii_idx1_raw, 3);
    bool bit3_ziii = bit(ziii_idx1_raw, 3);
    note("register00 bit3 (next board related), chip_addr index 1: Zorro-II "
         "raw nibble=0x" + hex1(zii_idx1_raw) + " bit3=" + std::to_string(bit3_zii) +
         "; Zorro-III raw nibble=0x" + hex1(ziii_idx1_raw) + " bit3=" + std::to_string(bit3_ziii));
    bool bit3_clear_both = !bit3_zii && !bit3_ziii;
    expect(bit3_clear_both, "sol:K-0770 register00 bit3 clear -> next board never declared related");
    expect(bit3_clear_both, "opus:K-0793 register00 bit3 clear -> next board never declared related");

    // =========================================================================
    // GROUP E -- register 04, product number. chip_addr index 2 (high
    // nibble, implemented, complemented) / index 3 (low nibble, unmapped
    // default, complemented). Zorro III only in this RTL.
    // =========================================================================
    uint8_t prod_hi = logical(2, ziii.at(2));
    uint8_t prod_lo = logical(3, ziii.at(3));
    note("register04 product number (Zorro III only in this RTL): high "
         "nibble=0x" + hex1(prod_hi) + " (raw=0x" + hex1(ziii.at(2)) +
         ", complemented), low nibble=0x" + hex1(prod_lo) +
         " (unimplemented default, complemented) -> product byte=0x" +
         hex1(prod_hi) + hex1(prod_lo) + " (RTL's own comment: 'ProductID=0x10').");
    bool product_matches = (prod_hi == 0x1) && (prod_lo == 0x0);
    expect(product_matches, "sol:K-0780 register04 holds product number (0x10 observed)");
    expect(product_matches, "opus:K-0813 register04 er_Product holds product number (0x10 observed)");

    record("opus:K-0812 NOT GRADED (p433): 'Register 04 is register 06 in "
           "the Zorro II mapping and register 104 in the Zorro III "
           "mapping.' Pure address-triple claim, no bit-content -- same "
           "reason as opus:K-0784: $E80104 is never reached by this "
           "design's sel_autoconfig decode. reason_kind: unreachable-address.");

    // =========================================================================
    // GROUP F -- register 08 flags. chip_addr index 4: Zorro III
    // (cpu_wrapper.v:598, implemented) vs Zorro II (unmapped -> the :547
    // default, complemented in both cases).
    // =========================================================================
    uint8_t reg08_ziii = logical(4, ziii.at(4));
    uint8_t reg08_zii = logical(4, zii.at(4));
    note("register08 (chip_addr index 4): Zorro-III logical nibble=0x" +
         hex1(reg08_ziii) + " (raw=0x" + hex1(ziii.at(4)) +
         ", complemented; RTL comment 'Memory card, not silenceable, "
         "Extended size, reserved.'), Zorro-II logical nibble=0x" +
         hex1(reg08_zii) + " (unmapped -> default, complemented).");

    bool f_bit7_ziii = bit(reg08_ziii, 3);   // bit7 of the byte = bit3 of this nibble
    bool f_bit7_zii = bit(reg08_zii, 3);
    expect(f_bit7_ziii, "sol:K-0782 register08 bit7 set -> memory device (Zorro III)");
    expect(!f_bit7_zii, "sol:K-0783 register08 bit7 clear -> I/O device (Zorro II default)");
    expect(f_bit7_ziii && !f_bit7_zii, "opus:K-0817 register08 er_Flags bit7: memory (ZIII) vs I/O (ZII default)");

    bool f_bit6_ziii = bit(reg08_ziii, 2);
    bool f_bit6_zii = bit(reg08_zii, 2);
    expect(f_bit6_ziii, "sol:K-0784 register08 bit6 set -> cannot be shut up by software (Zorro III)");
    expect(!f_bit6_zii, "sol:K-0785 register08 bit6 clear -> can be shut up by software (Zorro II default)");
    expect(f_bit6_ziii && !f_bit6_zii, "opus:K-0818 register08 bit6: not-silenceable (ZIII) vs silenceable (ZII default)");

    bool f_bit4_ziii = bit(reg08_ziii, 0);
    expect(f_bit4_ziii, "sol:K-0790 register08 bit4 reserved, reads 1 on Zorro III");
    expect(f_bit4_ziii, "opus:K-0821 register08 bit4 reserved, reads 1 on Zorro III");

    uint8_t subsize_ziii = reg08_ziii & 0xF;
    uint8_t subsize_zii = reg08_zii & 0xF;
    note("register08 bits3:0 (sub-size): Zorro-III=0x" + hex1(subsize_ziii) +
         " Zorro-II(default)=0x" + hex1(subsize_zii));
    bool subsize_present = (subsize_ziii == 0xF) && (subsize_zii == 0x0);
    expect(subsize_present, "sol:K-0791 register08 bits3-0 carry the board sub-size field");
    expect(subsize_present, "opus:K-0822 register08 bits3-0 carry the board sub-size field");

    record("opus:K-0815 NOT GRADED (p433): 'Register 08 is register 0A in "
           "the Zorro II mapping and register 108 in the Zorro III "
           "mapping.' Pure address-triple claim -- same reason as "
           "opus:K-0784/opus:K-0812: $E80108 is never reached by this "
           "design's sel_autoconfig decode. reason_kind: unreachable-address.");

    // =========================================================================
    // GROUP G -- register 0C, reserved, must read 0. chip_addr index 6,
    // unmapped in both configs -> the :547 default, complemented.
    // =========================================================================
    uint8_t reg0c_zii = logical(6, zii.at(6));
    note("register0C (chip_addr index 6, unmapped in both configs): logical nibble=0x" +
         hex1(reg0c_zii));
    bool reserved_0c = (reg0c_zii == 0x0);
    expect(reserved_0c, "sol:K-0814 register0C bits7-0 reserved, read 0");
    expect(reserved_0c, "opus:K-0848 register0C er_Reserved03 bits7-0 reserved, read 0");

    // =========================================================================
    // GROUP H -- manufacturer number. chip_addr indices 8/9 (high byte),
    // 10/11 (low byte); identical case values in both Zorro-II and
    // Zorro-III blocks, so tested once (Zorro-II harness).
    // =========================================================================
    uint8_t manu_hi_hi = logical(8, zii.at(8));
    uint8_t manu_hi_lo = logical(9, zii.at(9));
    uint16_t manu_hi_byte = ((uint16_t)manu_hi_hi << 4) | manu_hi_lo;
    note("register10 manufacturer high byte: nibble8=0x" + hex1(manu_hi_hi) +
         " nibble9=0x" + hex1(manu_hi_lo) + " -> byte=0x" + hex4(manu_hi_byte).substr(2));
    bool manu_hi_ok = (manu_hi_byte == 0x13);
    expect(manu_hi_ok, "sol:K-0815 register10 holds manufacturer number high byte (0x13 observed)");
    expect(manu_hi_ok, "opus:K-0849 register10 holds manufacturer number high byte (0x13 observed)");

    uint8_t manu_lo_hi = logical(10, zii.at(10));
    uint8_t manu_lo_lo = logical(11, zii.at(11));
    uint16_t manu_lo_byte = ((uint16_t)manu_lo_hi << 4) | manu_lo_lo;
    note("register14 manufacturer low byte: nibble10=0x" + hex1(manu_lo_hi) +
         " nibble11=0x" + hex1(manu_lo_lo) + " -> byte=0x" + hex4(manu_lo_byte).substr(2) +
         " -- together with the high byte: manufacturer=0x" + hex1(manu_hi_hi) +
         hex1(manu_hi_lo) + hex1(manu_lo_hi) + hex1(manu_lo_lo) +
         " (RTL's own comment: 'Manufacturer ID: 0x139c').");
    bool manu_lo_ok = (manu_lo_byte == 0x9C);
    expect(manu_lo_ok, "sol:K-0816 register14 holds manufacturer number low byte (0x9C observed)");
    expect(manu_lo_ok, "opus:K-0850 register14 holds manufacturer number low byte (0x9C observed)");

    // =========================================================================
    // GROUP I -- serial number, 4 bytes. Only byte 3 (LSB, chip_addr index
    // 19/0x13) is implemented in this design; bytes 0-2 (indices 12,14,16)
    // and byte3's own high nibble (index 18) fall to the unmapped default.
    // =========================================================================
    uint8_t ser18 = logical(12, zii.at(12));
    uint8_t ser1c = logical(14, zii.at(14));
    uint8_t ser20 = logical(16, zii.at(16));
    uint8_t ser24_hi = logical(18, zii.at(18));
    uint8_t ser24_lo = logical(19, zii.at(19));
    note("serial number bytes: reg18(byte0/msb)=0x" + hex1(ser18) +
         " reg1C(byte1)=0x" + hex1(ser1c) + " reg20(byte2)=0x" + hex1(ser20) +
         " reg24-hi=0x" + hex1(ser24_hi) + " reg24-lo(byte3/lsb)=0x" + hex1(ser24_lo) +
         " -- only byte3's low nibble is implemented in this design "
         "(cpu_wrapper.v:588 'serial=1'); everything else falls to the "
         "unmapped default.");

    bool ser18_unimpl = (ser18 == 0x0);
    expect(ser18_unimpl, "sol:K-0818 register18 serial byte0 (msb), unimplemented in this design, reads 0");
    expect(ser18_unimpl, "opus:K-0852 register18 serial byte0 (msb), unimplemented in this design, reads 0");

    bool ser1c_unimpl = (ser1c == 0x0);
    expect(ser1c_unimpl, "sol:K-0819 register1C serial byte1, unimplemented in this design, reads 0");
    expect(ser1c_unimpl, "opus:K-0853 register1C serial byte1, unimplemented in this design, reads 0");

    bool ser20_unimpl = (ser20 == 0x0);
    expect(ser20_unimpl, "sol:K-0820 register20 serial byte2, unimplemented in this design, reads 0");
    expect(ser20_unimpl, "opus:K-0854 register20 serial byte2, unimplemented in this design, reads 0");

    bool ser24_lsb_is_1 = (ser24_hi == 0x0) && (ser24_lo == 0x1);
    expect(ser24_lsb_is_1, "sol:K-0821 register24 serial byte3 (lsb) holds the serial number (1 observed)");
    expect(ser24_lsb_is_1, "opus:K-0855 register24 serial byte3 (lsb) holds the serial number (1 observed)");

    std::cout << "\n" << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}

// RESULTS (TG68K build): baseline
//   `make test` -> 43 passed, 0 failed (42 graded assertions + the
//   fixture-liveness apparatus check). Every graded claim's observed raw
//   nibble matched the value hand-derived from the RTL's own case-statement
//   comments in SPECIFICATION.md before this run -- no expected value was
//   adjusted after seeing a result, and no red was found to explain away.
//
// MUTATION WRONGCONFIG (`make MUT=WRONGCONFIG test`, after `make clean`):
//   37 passed, 6 failed -- turns red exactly, and only, the 6 assertions
//   the header block above names as WRONGCONFIG-sensitive
//   (sol:K-0766, opus:K-0789, sol:K-0765, opus:K-0788, sol:K-0762,
//   opus:K-0785), by feeding each Zorro configuration's own harness the
//   other configuration's captured nibble. Every other graded assertion
//   (36 of 42) stayed green under this mutation, including the two
//   "Reserved code never emitted" assertions (sol:K-0763/opus:K-0786,
//   sol:K-0764/opus:K-0787) -- confirmed rather than assumed: swapping
//   which of the two real, non-reserved codes (0b11/0b10) each context sees
//   still never produces 0b00 or 0b01, exactly as the "only two literal
//   assignments exist" argument predicts. Rebuilt with no MUT afterward:
//   43 passed / 0 failed baseline restored exactly.
//
// `reset` on this module is active-LOW (`always @(posedge clk, negedge
// reset)`, `if (~reset | ~reset_out)` throughout) despite its plain name --
// wired `.nreset(reset)` direct to the TG68K instance (:424), and
// `.extReset(~reset)` / `.pwrUp(~reset)` to the fx68k instance (:482-483).
// Driving it the conventional active-high way holds the design permanently
// in reset once "released" (cpustate stuck at 2'b01 "no memaccess",
// cpu_clkena stuck low, chip_addr never leaving $000000, for tens of
// thousands of cycles). This file's Harness constructor drives it
// active-low (`top->reset = 0` during the hold, `= 1` to release),
// confirmed working by the end-to-end round trip these 42 assertions
// depend on.
