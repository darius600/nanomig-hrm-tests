// Test purpose: pp.435-436 Appendix K "REGISTER BIT ASSIGNMENTS", the run of
//   17 reserved registers between the serial-number block (register 24) and
//   the base-address/shut-up registers (44/48/4C, covered by
//   hrm/appK-autoconfig-base-address).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 435, 436
// Non-goals: anything outside the `cpu_wrapper` fixture this package builds,
//   and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/appK-build/appK-autoconfig-reserved-registers.jsonl` in the
//   sibling repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// appK-autoconfig-reserved-registers -- pp.435-436 Appendix K "REGISTER BIT
// ASSIGNMENTS", the run of 17 reserved registers between the serial-number
// block (register 24) and the base-address/shut-up registers (44/48/4C,
// covered by hrm/appK-autoconfig-base-address). 34 claims (two readers per
// register), filtered from
// hrm-testpipeline/work/appK-build/appK-autoconfig-reserved-registers.jsonl.
//
// See SPECIFICATION.md for the full boundary investigation. In short: this
// package reuses hrm/appK-autoconfig-register-map's fixture wholesale (same
// Harness shape, same TG68KdotC_Kernel core selection, same active-low
// reset, same direct/complement register-arithmetic rule). Every one of the
// 34 chip_addr[6:1] indices this package reads (24,25,...,63 -- see the
// table in SPECIFICATION.md) falls outside BOTH case lists in
// cpu_wrapper.v's autocfg_data process (Zorro II: 0,1,8,9,10,11,19; Zorro
// III: 0,1,2,4,5,8,9,10,11,19 -- read in full, cpu_wrapper.v:568-606), so
// every one of them takes the unconditional cpu_wrapper.v:547 default
// (4'b1111), which the complement rule (idx>1) turns into logical 0000 --
// exactly "reserved, must be 0". This holds regardless of cpucfg/
// fastramcfg, so one Zorro-II-configured harness suffices for all 34
// claims.
//
// MUTATION METHODOLOGY. Every assertion below grades a design constant with
// no input port that can perturb it: the default is unconditional
// (cpu_wrapper.v:547, evaluated before either case statement and never
// overridden for these indices in either branch). For every assertion the
// mutation is `none impossible`, naming the exact index and the fact that
// cpu_wrapper.v:568-606 (both case lists, read in full) never mentions it.

// MUTATION sol:K-0829 none impossible: chip_addr indices 24/25 (register 30)
//   never appear in either case list (cpu_wrapper.v:568-590 Zorro II,
//   cpu_wrapper.v:594-606 Zorro III) -- unconditional cpu_wrapper.v:547
//   default in every configuration this design offers.
// MUTATION opus:K-0861 none impossible: same as sol:K-0829 (register 30,
//   page 435).
// MUTATION sol:K-0830 none impossible: chip_addr indices 26/27 (register
//   34) never appear in either case list -- unconditional default.
// MUTATION opus:K-0862 none impossible: same as sol:K-0830 (register 34,
//   page 435).
// MUTATION sol:K-0831 none impossible: chip_addr indices 28/29 (register
//   38) never appear in either case list -- unconditional default.
// MUTATION opus:K-0863 none impossible: same as sol:K-0831 (register 38,
//   page 435).
// MUTATION sol:K-0832 none impossible: chip_addr indices 30/31 (register
//   3C) never appear in either case list -- unconditional default.
// MUTATION opus:K-0864 none impossible: same as sol:K-0832 (register 3C,
//   page 435).
// MUTATION sol:K-0833 none impossible: chip_addr indices 32/33 (register
//   40) never appear in either case list -- unconditional default.
// MUTATION opus:K-0865 none impossible: same as sol:K-0833 (register 40,
//   page 435).
// MUTATION sol:K-0851 none impossible: chip_addr indices 40/41 (register
//   50) never appear in either case list -- unconditional default.
// MUTATION opus:K-0890 none impossible: same as sol:K-0851 (register 50,
//   page 436).
// MUTATION sol:K-0852 none impossible: chip_addr indices 42/43 (register
//   54) never appear in either case list -- unconditional default.
// MUTATION opus:K-0891 none impossible: same as sol:K-0852 (register 54,
//   page 436).
// MUTATION sol:K-0853 none impossible: chip_addr indices 44/45 (register
//   58) never appear in either case list -- unconditional default.
// MUTATION opus:K-0892 none impossible: same as sol:K-0853 (register 58,
//   page 436).
// MUTATION sol:K-0854 none impossible: chip_addr indices 46/47 (register
//   5C) never appear in either case list -- unconditional default.
// MUTATION opus:K-0893 none impossible: same as sol:K-0854 (register 5C,
//   page 436).
// MUTATION sol:K-0855 none impossible: chip_addr indices 48/49 (register
//   60) never appear in either case list -- unconditional default.
// MUTATION opus:K-0894 none impossible: same as sol:K-0855 (register 60,
//   page 436).
// MUTATION sol:K-0856 none impossible: chip_addr indices 50/51 (register
//   64) never appear in either case list -- unconditional default.
// MUTATION opus:K-0895 none impossible: same as sol:K-0856 (register 64,
//   page 436).
// MUTATION sol:K-0857 none impossible: chip_addr indices 52/53 (register
//   68) never appear in either case list -- unconditional default.
// MUTATION opus:K-0896 none impossible: same as sol:K-0857 (register 68,
//   page 436).
// MUTATION sol:K-0858 none impossible: chip_addr indices 54/55 (register
//   6C) never appear in either case list -- unconditional default.
// MUTATION opus:K-0897 none impossible: same as sol:K-0858 (register 6C,
//   page 436).
// MUTATION sol:K-0859 none impossible: chip_addr indices 56/57 (register
//   70) never appear in either case list -- unconditional default.
// MUTATION opus:K-0898 none impossible: same as sol:K-0859 (register 70,
//   page 436).
// MUTATION sol:K-0860 none impossible: chip_addr indices 58/59 (register
//   74) never appear in either case list -- unconditional default.
// MUTATION opus:K-0899 none impossible: same as sol:K-0860 (register 74,
//   page 436).
// MUTATION sol:K-0861 none impossible: chip_addr indices 60/61 (register
//   78) never appear in either case list -- unconditional default. (Note:
//   this is sol:K-0861, page 436, register 78 -- a different uid from
//   opus:K-0861, page 435, register 30, above; the uid is the reader:id
//   pair, never the bare id.)
// MUTATION opus:K-0900 none impossible: same as sol:K-0861 above (register
//   78, page 436).
// MUTATION sol:K-0862 none impossible: chip_addr indices 62/63 (register
//   7C) never appear in either case list -- unconditional default. (Note:
//   this is sol:K-0862, page 436, register 7C -- a different uid from
//   opus:K-0862, page 435, register 34, above.)
// MUTATION opus:K-0901 none impossible: same as sol:K-0862 above (register
//   7C, page 436).

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
// hrm/appK-autoconfig-register-map/test_cpu_wrapper.cpp) ----
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

// Harness: identical mechanism to hrm/appK-autoconfig-register-map's own
// (reset -> vector fetch -> instruction fetch -> decode -> execute -> bus
// read -> bus write, entirely through cpu_wrapper's own ports). Reused
// wholesale rather than re-derived; see SPECIFICATION.md's disclosure.
struct Harness {
    Vcpu_wrapper* top;
    std::map<uint32_t, uint16_t> mem;
    std::vector<int> indices;
    std::vector<uint8_t> raw;
    bool all_captured = false;

    void tick() { top->clk = 0; top->eval(); top->clk = 1; top->eval(); }

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
        const int MAX_CYC = 200000; // 34 indices -- more headroom than the
                                     // 13-index register-map fixture needed
        for (int cyc = 0; cyc < MAX_CYC && captured.size() < scratch.size(); cyc++) {
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
        return 0xFF;
    }
};

// idx <= 1 direct, everything else complemented -- see SPECIFICATION.md.
static uint8_t logical(int idx, uint8_t rawnib) {
    return (idx <= 1) ? rawnib : (uint8_t)((~rawnib) & 0xF);
}

int main() {
    // Zorro II (cpucfg=0, fastramcfg=1): config-independent for these 34
    // indices (see SPECIFICATION.md), so one harness suffices.
    std::vector<int> idx = {
        24,25, 26,27, 28,29, 30,31, 32,33,             // 30,34,38,3C,40 (p435)
        40,41, 42,43, 44,45, 46,47, 48,49, 50,51,       // 50,54,58,5C,60,64 (p436)
        52,53, 54,55, 56,57, 58,59, 60,61, 62,63        // 68,6C,70,74,78,7C (p436)
    };
    Harness h(0, 1, idx);

    note("fixture construction: all_captured=" +
         std::string(h.all_captured ? "true" : "false") +
         " -- must be true for any assertion below to mean anything.");

    // =========================================================================
    // Liveness -- carries no claim uid, proves the round trip is not stuck
    // (distinguishing "genuinely read as 0" from "the harness never ran").
    // Register 24 (serial byte3 lsb, known non-zero content = 1, chip_addr
    // index 19) is read as an extra, unlisted probe purely for this check.
    // =========================================================================
    Harness liveness_probe(0, 1, std::vector<int>{19});
    uint8_t serial_lsb_raw = liveness_probe.at(19);
    note("liveness: chip_addr index 19 (register 24 low nibble, known "
         "non-zero content elsewhere in this design) raw nibble = 0x" +
         hex1(serial_lsb_raw) + " -- read through the same real reset -> "
         "vector fetch -> instruction fetch -> decode -> execute -> bus "
         "read -> bus write sequence as the 34 reserved-register reads "
         "below, no internal signal touched.");
    expect(h.all_captured && liveness_probe.all_captured && serial_lsb_raw != 0x0,
           "fixture-liveness [not graded -- apparatus check, no claim uid]");

    // =========================================================================
    // The 17 reserved registers, each graded twice (sol, opus).
    // =========================================================================
    struct Reg { const char* name; int idx_hi; int idx_lo;
                 const char* sol_uid; const char* opus_uid; };
    std::vector<Reg> regs = {
        {"30", 24,25, "sol:K-0829", "opus:K-0861"},
        {"34", 26,27, "sol:K-0830", "opus:K-0862"},
        {"38", 28,29, "sol:K-0831", "opus:K-0863"},
        {"3C", 30,31, "sol:K-0832", "opus:K-0864"},
        {"40", 32,33, "sol:K-0833", "opus:K-0865"},
        {"50", 40,41, "sol:K-0851", "opus:K-0890"},
        {"54", 42,43, "sol:K-0852", "opus:K-0891"},
        {"58", 44,45, "sol:K-0853", "opus:K-0892"},
        {"5C", 46,47, "sol:K-0854", "opus:K-0893"},
        {"60", 48,49, "sol:K-0855", "opus:K-0894"},
        {"64", 50,51, "sol:K-0856", "opus:K-0895"},
        {"68", 52,53, "sol:K-0857", "opus:K-0896"},
        {"6C", 54,55, "sol:K-0858", "opus:K-0897"},
        {"70", 56,57, "sol:K-0859", "opus:K-0898"},
        {"74", 58,59, "sol:K-0860", "opus:K-0899"},
        {"78", 60,61, "sol:K-0861", "opus:K-0900"},
        {"7C", 62,63, "sol:K-0862", "opus:K-0901"},
    };

    for (auto& r : regs) {
        uint8_t hi = logical(r.idx_hi, h.at(r.idx_hi));
        uint8_t lo = logical(r.idx_lo, h.at(r.idx_lo));
        note(std::string("register") + r.name + " (chip_addr indices " +
             std::to_string(r.idx_hi) + "/" + std::to_string(r.idx_lo) +
             "): raw=0x" + hex1(h.at(r.idx_hi)) + hex1(h.at(r.idx_lo)) +
             " -> logical byte=0x" + hex1(hi) + hex1(lo));
        bool is_zero = (hi == 0x0) && (lo == 0x0);
        expect(is_zero, std::string(r.sol_uid) + " register" + r.name +
               " bits7-0 reserved, reads 0");
        expect(is_zero, std::string(r.opus_uid) + " register" + r.name +
               " bits7-0 reserved, reads 0");
    }

    std::cout << "\n" << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}

// RESULTS: to be filled in after `make test` is run (WSL, Verilator 5.052).
