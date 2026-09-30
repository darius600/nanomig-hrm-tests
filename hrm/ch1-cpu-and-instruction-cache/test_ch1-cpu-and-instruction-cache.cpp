// ch1-cpu-and-instruction-cache -- cpu_cache_new conformance test, plus a
//   documented HALT for the claims this package cannot reach.
//
// Test purpose: page 13's instruction-cache guideline (store vs. monitor
//   writes; the CACR $0008 clear-instruction-cache bit).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 7, 8 and 13.
// Non-goals: anything outside the `cpu_cache_new` fixture this package
//   builds, and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// CLAIM SOURCE: hrm-testpipeline/work\
//   2026-09-16_ch1_packages.jsonl, filtered on classify.package ==
//   "ch1-cpu-and-instruction-cache" -- 15 claim records.
//
// ORDER OF WORK, AND ITS ONE DISCLOSED BREAK: SPECIFICATION.md was written
//   from the 15 claims' quote/restatement fields alone and committed
//   before cpu_wrapper.v or cpu_cache_new.v was opened for this package's
//   own purposes. That file also discloses that the claims' own
//   classify.evidence text already carried file:line RTL citations before
//   this package's work started -- the blind order was not fully intact,
//   and nothing here pretends otherwise. This file was written after
//   cpu_wrapper.v and cpu_cache_new.v were read in full.
//
// ============================================================================
// BOUNDARY ACTUALLY USED -- cpu_cache_new, not cpu_wrapper, and why
// ============================================================================
//
// This package's assigned top module is cpu_wrapper (task sheet, and the
// ch1 build brief's package table). SPECIFICATION.md named 8 propositions
// (PROP-1..PROP-8) before any RTL was open. Once cpu_wrapper.v was read in
// full, it became clear that PROP-1 through PROP-6 (12 of the 15 claims)
// cannot be graded at cpu_wrapper's own external port boundary:
//
//   cpu_wrapper.v:60-66 (chip bus) and :81-88 (ram bus) declare
//     chip_addr, chip_din, chip_as, chip_uds, chip_lds, chip_rw,
//     ramreq, ramaddr, ramdin, ramlds, ramuds, ramshared
//   ALL as `output`/`output reg`. cpu_wrapper.v:95-97 declares cpustate,
//   cacr and nmi_addr `output reg` too. There is NO address-input port
//   anywhere on this module -- every one of those signals is generated
//   internally by whichever CPU core cpu_wrapper instantiates (fx68k at
//   cpu_wrapper.v:473, TG68KdotC_Kernel at :404) from cpu_addr_o /
//   cpu_addr_p, themselves the CPU core's own output, driven by whatever
//   instructions that core fetches and executes.
//
//   To observe chip_addr==$E80000 (AUTOCONFIG, PROP-1..PROP-3), or
//   sel_z2ram at $200000 (PROP-4's RAM half, PROP-5, PROP-6), the embedded
//   CPU core must actually fetch and execute a real 68000 instruction
//   stream that performs that access. That is exactly what the task's own
//   rule calls a HALT: "No port of cpu_wrapper carries an opcode. If a
//   claim needs one, that is a HALT, not something to solve by reaching
//   outside." Building a small hand-assembled 68000 program and a stand-in
//   instruction-fetch memory model to drive the embedded core to a chosen
//   address IS reaching outside this fixture's natural boundary -- it is
//   not a port cpu_wrapper offers, it is a CPU test harness we would be
//   inventing on top of it.
//
//   A second obstacle is only a build option: the real fx68k core
//   cpu_wrapper.v:473 instantiates fails this project's own Verilator
//   (5.052) under its default checks. Measured directly (not assumed), from
//   this package's own directory tree:
//     $ verilator --lint-only --top-module cpu_wrapper \
//         $(NANOMIG)/src/minimig-aga/cpu_wrapper.v \
//         $(NANOMIG)/src/fx68k/fx68k.sv $(NANOMIG)/src/fx68k/fx68kAlu.sv \
//         $(NANOMIG)/src/fx68k/uaddrPla.sv
//     %Error-BLKANDNBLK: fx68k.sv:293:10: Unsupported: Blocking and
//       non-blocking assignments to same non-packed variable:
//       'cpu_wrapper.cpu_inst_o.Nanod'
//   NanoMig's own sim/Makefile waives that error with -Wno-BLKANDNBLK; so
//   built, fx68k runs with --no-assert (its unique-case checks stop it at
//   time 0 otherwise), and hrm/appK-autoconfig-register-map's hand-assembled
//   program gives the same result lines on it as on TG68K. The HALT rule
//   above is the reason that stands.
//   A Verilator-adapted fork exists at $(NANOMIG)/sim/fx68x_verilator/
//   (fx68k_flat.sv etc.), but a diff against src/fx68k/fx68k.sv shows a
//   near-total rewrite (1,838 vs. 1,841 lines, essentially no shared
//   text), not a drop-in patch -- using it would measure a materially
//   different core than the one cpu_wrapper.v:473 actually instantiates,
//   so this package does not substitute it.
//
//   PROP-4's other half (hard disk controllers) is out of scope for a
//   second, independent reason: its own counterpart (gary.v:181-182
//   sel_ide/sel_gayle, per the claim's own classify.evidence) lives in
//   gary, a sibling top-level module cpu_wrapper does not instantiate --
//   the same "needs a different top, not covered by this package" call
//   hrm/ch7-custom-address-space made for its own PROP-D even-address
//   half.
//
// PROP-7 and PROP-8 (the 3 remaining claims) are different: the claims
//   file's own classify.observable field already says "yes, at
//   cpu_cache_new" for both -- and cpu_cache_new.v:28 declares cpu_adr as
//   a genuine `input`, no embedded CPU core anywhere in its own hierarchy
//   (cpu_cache_new.v instantiates only dpram/dpram_be_1024x16 RAM
//   primitives -- confirmed by reading the whole 681-line file). This
//   package therefore builds cpu_cache_new directly as its actual
//   Verilator top, and grades PROP-7/PROP-8 there.
//
// A FACTUAL CORRECTION TO THE TASK SHEET AND THE CLAIMS' OWN
//   classify.evidence: both describe a fixture that "reads $E80000..
//   $E80043 through chip_addr/chip_rw", "writes a base through chip_din",
//   or "issues one access at $200000 ... and shows ... chip_as" -- all of
//   which treat chip_addr/chip_din/chip_rw/chip_as as inputs a testbench
//   can drive. They are outputs (cited above). This is very likely one of
//   the build brief's own warned-about errors ("Assume this one is wrong
//   somewhere too, and report where"); reported here, not corrected by
//   inventing a workaround.
//
// CLAIM COVERAGE -- all 15 ids:
//   PROP-1 AUTOCONFIG-EXISTS (2, HALT):     sol/opus:1-components.c133
//   PROP-2 AUTOCONFIG-DYNAMIC (2, HALT):    sol/opus:1-components.c134
//   PROP-3 AUTOCONFIG-NO-DIP (2, HALT):     sol/opus:1-components.c135
//   PROP-4 EXPANSION-EXAMPLES (2, HALT):    sol/opus:1-components.c137
//   PROP-5 BUS-TOPOLOGY (2, HALT):          sol/opus:1-blockdiag.c2
//   PROP-6 FIGURE-FAST-RAM (2, HALT):       sol/opus:1-blockdiag.c8
//   PROP-7 CACHE-STORES/WRITES (2, graded): sol/opus:1-guidelines.c70
//   PROP-8 CACR-CLEARI-0008 (1, graded):    opus:1-guidelines.c75
//   12 + 3 = 15. All 15 ids appear below, each exactly once as a HALT
//   record() or on a graded expect() label.
//
// CONFIGURATION CHOSEN, AND WHY: ADDR_WIDTH is left at its module default
//   (24, cpu_cache_new.v:18) -- not overridden -- because this test drives
//   the module directly and has no reason to pick a narrower width nanomig
//   .v might use for a particular board config. cpu_bs is held 2'b11
//   (both bytes) throughout; no test here depends on byte-lane splitting.
//   sdr_read_ack is tied high for the whole run (see WHY NO SDRAM MODEL
//   below).
//
// WHY NO SDRAM TIMING MODEL: cpu_cache_new's own state machine
//   (CPU_SM_FILL1..FILL4, cpu_cache_new.v:353-435) advances one fill word
//   per sdr_read_ack pulse; holding sdr_read_ack constantly high collapses
//   a miss to the minimum number of cycles the RTL's own state machine
//   allows, with sdr_dat_r captured at the exact cycle this test controls.
//   This fixture does not use fill timing to distinguish a hit from a
//   miss anywhere -- every hit/miss distinction below is made on DATA
//   content (whether a stale, previously-cached word or a freshly-fed one
//   comes back), which is timing-independent and unaffected by this
//   simplification.
//
// WHY NO DATA-DELIVERY PROOF IS A SEPARATE ITEM: it IS the first graded
//   assertion below (CACHE-FIXTURE-DELIVERS-KNOWN-VALUE) -- every other
//   assertion depends on the fixture's ability to feed a chosen word
//   through sdr_dat_r and read it back correctly, so that control comes
//   first and is graded like any other assertion (its own mutation
//   included), not asserted for free.
//
// WRITES NEVER ASSERT cpu_ack IN THIS RTL: cpu_cache_new.v's
//   CPU_SM_WRITE (:298-309) and CPU_SM_WB (:310-313) states never set
//   cpu_ack -- only the READ/FILL path does (:314-435). Confirmed by
//   reading the whole always block; not assumed. The Harness::access()
//   write path below does not wait for an acknowledge that never comes;
//   it holds cpu_cs for a fixed number of cycles instead (comment at the
//   call site).
//
// RESULTS: filled in below, after WSL/Verilator run(s) -- see the bottom
//   of this file for the final passed/failed counts and the mutation log,
//   each entry an actually-run build+run pair.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vcpu_cache_new.h"

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

static std::string hexw(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "$%04X", v);
    return std::string(buf);
}

struct Harness {
    Vcpu_cache_new *top;

    Harness() {
        top = new Vcpu_cache_new;
        top->clk = 0;
        top->rst = 1;
        top->cpu_cache_ctrl = 0b0001; // enable set, clear bit low
        top->cache_inhibit = 0;
        top->cpu_cs = 0;
        top->cpu_adr = 0;
        top->cpu_bs = 0b11;
        top->cpu_we = 0;
        top->cpu_ir = 0;
        top->cpu_dr = 0;
        top->cpu_dat_w = 0;
        top->sdr_dat_r = 0;
        top->sdr_read_ack = 1;  // see header, "WHY NO SDRAM TIMING MODEL"
        top->snoop_act = 0;
        top->snoop_adr = 0;
        top->snoop_dat_w = 0;
        top->snoop_bs = 0;
        for (int i = 0; i < 4; ++i) tick();
        top->rst = 0;
        // SDR_SM_INIT1 (cpu_cache_new.v:481-491) increments the 10-bit
        // sdr_sm_adr by 4 each cycle and exits when sdr_sm_adr[9:2] is all
        // ones -- i.e. after sdr_sm_adr/4 has counted 0..255, 256
        // iterations, before cache_init_done reasserts and CPU_SM_INIT
        // (:275-282) admits the first access. A first attempt at this
        // fixture used 150 here (a miscount -- 64, not 256) and every
        // first access after construction silently timed out; 320 is a
        // measured margin over the corrected 256-iteration count.
        for (int i = 0; i < 320; ++i) tick();
    }
    ~Harness() { top->final(); delete top; }

    void tick() { top->clk = 0; top->eval(); top->clk = 1; top->eval(); }

    // One CPU-side access, read or write, to completion.
    // Returns ticks-to-ack for a read (>=1), or -1 if a read never
    // acknowledged within timeout. Writes never ack in this RTL (see
    // header) -- the write path just holds cs for a fixed span instead
    // and always returns 4.
    int access(uint32_t adr, bool we, bool ir, bool dr, uint16_t din,
               uint16_t &dout, int timeout = 80) {
        top->cpu_adr = adr;
        top->cpu_bs = 0b11;
        top->cpu_we = we;
        top->cpu_ir = ir;
        top->cpu_dr = dr;
        top->cpu_dat_w = din;
        top->cpu_cs = 1;
        int cycles = -1;
        dout = 0;
        if (we) {
            for (int i = 0; i < 4; ++i) tick();
            cycles = 4;
        } else {
            for (int i = 0; i < timeout; ++i) {
                tick();
                if (top->cpu_ack) { cycles = i + 1; dout = top->cpu_dat_r; break; }
            }
        }
        top->cpu_cs = 0;
        top->cpu_we = 0;
        top->cpu_ir = 0;
        top->cpu_dr = 0;
        for (int i = 0; i < 6; ++i) tick();
        return cycles;
    }

    int read_instr(uint32_t adr, uint16_t &dout, int timeout = 80) {
        return access(adr, false, true, false, 0, dout, timeout);
    }
    void write_cpu(uint32_t adr, uint16_t data) {
        uint16_t d;
        access(adr, true, false, false, data, d);
    }

    // Pulses CACR bit 3 ($0008, CACRF_ClearI) for two idle cycles -- enough
    // for the edge-detect at cpu_cache_new.v:227 to latch cc_clr -- then
    // waits out the same 256-iteration re-init sweep the constructor
    // waits out (cpu_cache_new.v:493-499 SDR_SM_IDLE -> :472-480
    // SDR_SM_INIT0/1; see the constructor's own comment for the count).
    void pulse_clear() {
        top->cpu_cs = 0;
        top->cpu_cache_ctrl = 0b1001; // enable + clear
        tick();
        tick();
        top->cpu_cache_ctrl = 0b0001; // drop clear, keep enable
        for (int i = 0; i < 320; ++i) tick();
    }
};

int main() {
    Checks checks;

    // ========================================================================
    // HALT -- PROP-1 through PROP-6 (12 claims), not graded at this
    // package's assigned top (cpu_wrapper). See the file header for the
    // full argument (port directions, fx68k's Verilator build options, and
    // PROP-4's second, separate reason).
    // ========================================================================
    checks.record(
        "PROP-1-AUTOCONFIG-EXISTS-NOT-GRADED (p.7, sol:1-components.c133, "
        "opus:1-components.c133): 'These devices are automatically "
        "recognized and used by system software through a well defined, "
        "well documented linking procedure called AUTOCONFIG.' HALT: "
        "observing this needs the embedded CPU core to actually fetch/"
        "execute a program that performs an AUTOCONFIG read at $E80000 -- "
        "cpu_wrapper has no address-input port (chip_addr is `output reg`, "
        "cpu_wrapper.v:60), only the internal CPU core (fx68k.v:473 / "
        "TG68KdotC_Kernel.v:404) generates one, by executing instructions. "
        "No port of cpu_wrapper carries an opcode; per the task's own rule "
        "that is a HALT, not something to solve by reaching outside. "
        "reason_kind: unreachable-boundary.");
    checks.record(
        "PROP-2-AUTOCONFIG-DYNAMIC-NOT-GRADED (p.7, sol:1-components.c134, "
        "opus:1-components.c134): 'AUTOCONFIG is short for automatic "
        "configuration and is the process which allows memory or I/O "
        "space for an expansion board to be dynamically allocated by the "
        "system at boot time.' HALT: same reason as PROP-1 -- the write "
        "that sets z2ram_ena/z3ram_base (cpu_wrapper.v:629-664) fires only "
        "on a real bus write cycle the embedded CPU core must generate; "
        "chip_din/chip_rw/chip_uds are all `output reg` (cpu_wrapper.v:"
        "62-66), not inputs this fixture can drive. reason_kind: "
        "unreachable-boundary.");
    checks.record(
        "PROP-3-AUTOCONFIG-NO-DIP-NOT-GRADED (p.7, sol:1-components.c135, "
        "opus:1-components.c135): 'Unlike some other systems, there is no "
        "need to set DIP switches to select an address space from a fixed "
        "range reserved for expansion devices.' HALT: this claim's own "
        "observable substitute (SPECIFICATION.md PROP-3) is that the "
        "board's address comes only from the dynamic AUTOCONFIG write, "
        "which is PROP-2's own mechanism -- same unreachable-boundary "
        "reason as PROP-2, inherited rather than re-argued.");
    checks.record(
        "PROP-4-EXPANSION-EXAMPLES-NOT-GRADED (p.7, sol:1-components.c137, "
        "opus:1-components.c137): 'Peripherals that can be added include "
        "hard disk controllers and drives, or additional external RAM.' "
        "HALT, two separate reasons for the two halves. External RAM: "
        "same unreachable-boundary reason as PROP-1/2 -- sel_z2ram/"
        "sel_z3ram* (cpu_wrapper.v:115-118) depend on cpu_addr, internally "
        "CPU-generated only. Hard disk controller: its own counterpart "
        "(gary.v:181-182 sel_ide/sel_gayle, per this claim's own "
        "classify.evidence) lives in gary, a sibling top-level module "
        "cpu_wrapper does not instantiate -- needs a different top, not "
        "covered by this package (the same call hrm/ch7-custom-address-"
        "space made for its own PTR-PAIR-EVEN-ADDRESS-HALF). 'Drives' "
        "themselves are a mechanical peripheral with no register-level RTL "
        "counterpart to test. reason_kind: unreachable-boundary (RAM half), "
        "wrong-top (HDD half).");
    checks.record(
        "PROP-5-BUS-TOPOLOGY-NOT-GRADED (p.8, sol:1-blockdiag.c2, "
        "opus:1-blockdiag.c2): 'Notice that there are two separate buses, "
        "one that only the CPU can access (Fast memory) and another one "
        "that the custom chips share with the CPU (Chip memory).' HALT: "
        "the discriminating signals (ramsel/ramreq at cpu_wrapper.v:167-"
        "168, chip_as at :313) are computed from cpu_addr, the same "
        "internally-CPU-generated address PROP-1 through PROP-4 depend "
        "on. Demonstrating 'a Fast-RAM access at $200000 never asserts "
        "chip_as' needs the embedded core to actually generate a $200000 "
        "cycle. reason_kind: unreachable-boundary.");
    checks.record(
        "PROP-6-FIGURE-FAST-RAM-NOT-GRADED (p.8, Figure 1-1, "
        "sol:1-blockdiag.c8, opus:1-blockdiag.c8): the figure (opened and "
        "read directly for this package -- see SPECIFICATION.md, 'Figure "
        "1-1 checked independently') shows Fast RAM reached only via the "
        "CPU DATA/CPU ADDRESS buses, never via BUFFERS AND ARBITRATION "
        "LOGIC or the CUSTOM CHIP SECTION -- the same structural fact "
        "PROP-5 states in words. HALT for the same reason as PROP-5, "
        "inherited rather than re-argued.");

    // ========================================================================
    // GRADED -- cpu_cache_new, PROP-7 and PROP-8 (3 claims).
    // ========================================================================

    const uint16_t V1 = 0xAAAA;  // original / cached content
    const uint16_t V2 = 0xBBBB;  // "memory changed after caching" content
    const uint16_t V3 = 0xCCCC;  // content a CPU write tries to install
    const uint32_t ADDR_A = 0x000100;  // idx=0x20 blk=0 tag=0
    const uint32_t ADDR_B = 0x000200;  // idx=0x40 blk=0 tag=0 -- different row, no aliasing with A

    // ---------------- CACHE-FIXTURE-DELIVERS-KNOWN-VALUE ----------------
    // [control, no claim id] -- proves this fixture's sdr_dat_r stub
    // genuinely reaches cpu_dat_r through a real fill before anything
    // else in this file depends on that path.
    {
        Harness h;
        uint16_t fed =
#if MUT_DELIVER
            0x9999;  // MUTATION: fixture feeds a different word than the still-V1-expecting comparison below.
#else
            V1;
#endif
        h.top->sdr_dat_r = fed;
        uint16_t got;
        int cyc = h.read_instr(ADDR_A, got);
        bool ok = (cyc > 0) && (got == V1);
        checks.expect(ok,
            "CACHE-FIXTURE-DELIVERS-KNOWN-VALUE: an instruction read on a "
            "cold cache (a miss, since the constructor's reset leaves every "
            "tag invalid) returns the exact word this fixture placed on "
            "sdr_dat_r during the fill. [control, no claim id] measured "
            "ack_cycles=" + std::to_string(cyc) + " got=" + hexw(got) +
            " fed=" + hexw(fed) + ".");
    }

    // ---------------- CACHE-HIT-RETAINS-INSTRUCTION ----------------
    {
        Harness h;
        h.top->sdr_dat_r = V1;
        uint16_t first;
        int cyc1 = h.read_instr(ADDR_A, first);   // miss -> fills at V1
        h.top->sdr_dat_r = V2;                    // "memory" changes behind the cache's back
        uint32_t recheck =
#if MUT_HITMISS
            ADDR_B;  // MUTATION: re-read a different, never-cached address instead of the same one, forcing a fresh (miss) fetch that picks up the new background value.
#else
            ADDR_A;
#endif
        uint16_t second;
        int cyc2 = h.read_instr(recheck, second);
        bool ok = (cyc1 > 0) && (cyc2 > 0) && (first == V1) && (second == V1);
        checks.expect(ok,
            "CACHE-HIT-RETAINS-INSTRUCTION: an instruction address filled "
            "once at " + hexw(V1) + " is read again after the underlying "
            "memory model changed to " + hexw(V2) + ", and still returns "
            "the original " + hexw(V1) + " -- served from the cache, not "
            "re-fetched. 'These caches store recently used instructions...' "
            "(p.13, sol:1-guidelines.c70, opus:1-guidelines.c70). measured "
            "first=" + hexw(first) + " second=" + hexw(second) + " (ack "
            "cycles " + std::to_string(cyc1) + "/" + std::to_string(cyc2) + ").");
    }

    // ---------------- CACHE-WRITE-VISIBILITY ----------------
    // Measures the sentence's other half -- "...but do not monitor
    // writes." Whether this design's answer agrees with the page is what
    // this assertion reports, not something decided in advance (per the
    // build brief's own instruction for exactly this kind of claim).
    {
        Harness h;
        h.top->sdr_dat_r = V1;
        uint16_t filled;
        h.read_instr(ADDR_A, filled);              // establish the cached line at V1
        uint32_t write_target =
#if MUT_WRITEVIS
            ADDR_B;  // MUTATION: redirect the CPU write to a different address, so it cannot touch A's cached line.
#else
            ADDR_A;
#endif
        h.write_cpu(write_target, V3);
        uint16_t after;
        int cyc = h.read_instr(ADDR_A, after);      // re-read the SAME address
        bool matches_manual = (filled == V1) && (cyc > 0) && (after == V1);
        checks.expect(matches_manual,
            "CACHE-WRITE-VISIBILITY: after a CPU write of " + hexw(V3) +
            " to the address a cached instruction line came from, "
            "re-reading that instruction address returns " + hexw(after) +
            "; the manual's '...do not monitor writes' (p.13, "
            "sol:1-guidelines.c70, opus:1-guidelines.c70) predicts the "
            "original " + hexw(V1) + ". measured filled=" + hexw(filled) +
            " write_target=" + std::to_string((int)write_target) +
            " after=" + hexw(after) + ".");
    }

    // ---------------- CACR-CLEAR-INVALIDATES-CACHE ----------------
    {
        Harness h;
        h.top->sdr_dat_r = V1;
        uint16_t filled;
        h.read_instr(ADDR_A, filled);   // establish the cached line at V1
        h.top->sdr_dat_r = V2;          // what a fresh fetch will see if the entry is really gone
#if MUT_CLEAR
        // MUTATION: skip the CACR clear pulse entirely.
#else
        h.pulse_clear();
#endif
        uint16_t after;
        int cyc = h.read_instr(ADDR_A, after);
        bool ok = (filled == V1) && (cyc > 0) && (after == V2);
        checks.expect(ok,
            "CACR-CLEAR-INVALIDATES-CACHE: after CACR bit 3 ($0008) is "
            "pulsed, re-reading a previously-cached instruction address "
            "returns a freshly-fetched " + hexw(V2) + " rather than the "
            "stale cached " + hexw(V1) + ". 'CACRF_ClearI EQU $0008 ;Bit "
            "for clear instruction cache' (p.13, opus:1-guidelines.c75). "
            "measured filled=" + hexw(filled) + " after=" + hexw(after) +
            " ack_cycles=" + std::to_string(cyc) + ".");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}

// RESULTS: 3 passed, 1 failed. CACHE-WRITE-VISIBILITY is RED -- a genuine finding, not
//   adjusted to pass: this design's cpu_cache_new updates a matching
//   cached instruction line on an ordinary CPU write (cpu_cache_new.v:
//   298-309, CPU_SM_WRITE sets cpu_sm_iram0_we/iram1_we from
//   itag0_match/itag1_match alone, with no cpu_ir/cpu_dr distinction),
//   contradicting page 13's plain "...but do not monitor writes." Whether
//   that is a defect or a deliberate coherency feature (nanomig.v:316-328's
//   own comment: "All chip bus writes (cpu or dma) are snooped to keep
//   cached chip ram / kickstart data coherent") is outside this test's own
//   question -- see BUILD-BRIEF.md, "measure it, report it." Every other
//   assertion is GREEN, including a positive, working CACR-clear result
//   (the manual's own $0008 bit does exactly what page 13 says).
//
// The Harness's post-reset settle margin is 320 cycles:
//   cpu_cache_new.v:481-491's own re-init sweep runs sdr_sm_adr/4 = 256
//   iterations (0..255), and 320 cycles gives clean margin above that count
//   so no first access after construction times out (ack_cycles=-1)
//   before any claim here is measured.
//
// MUTATION LOG (each entry actually run:
//   rebuild with `make MUT=X test` -> read the PASS/FAIL lines -> confirm
//   only the named assertion changed colour relative to the 3-passed/
//   1-failed baseline -> rebuild with no MUT -> confirm the baseline was
//   restored):
//
// MUTATION CACHE-FIXTURE-DELIVERS-KNOWN-VALUE stimulus red: built with
//   `make MUT=MUT_DELIVER test`, which feeds $9999 through sdr_dat_r while
//   the comparison still checks for $AAAA -> 2 passed, 2 failed, only this
//   assertion newly red (measured got=$9999 fed=$9999). Rebuilt with no
//   MUT: 3/1 restored.
// MUTATION CACHE-HIT-RETAINS-INSTRUCTION stimulus red: built with
//   `make MUT=MUT_HITMISS test`, which re-reads a different, never-cached
//   address instead of the original one -> 2 passed, 2 failed, only this
//   assertion newly red (measured second=$BBBB, the freshly-fed
//   background value, instead of the cached $AAAA). Rebuilt with no MUT:
//   3/1 restored.
// MUTATION CACHE-WRITE-VISIBILITY stimulus green: built with
//   `make MUT=MUT_WRITEVIS test`, which redirects the CPU write to a
//   different address than the cached one -> 4 passed, 0 failed, only
//   this assertion newly green (measured after=$AAAA, unaffected, since
//   the write no longer targets the cached line). Rebuilt with no MUT:
//   3/1 restored -- confirming the red baseline is a live reading of a
//   real write-visibility effect, not a stuck-at value.
// MUTATION CACR-CLEAR-INVALIDATES-CACHE stimulus red: built with
//   `make MUT=MUT_CLEAR test`, which skips the CACR clear pulse entirely
//   -> 2 passed, 2 failed, only this assertion newly red (measured
//   after=$AAAA, the stale cached value, instead of the freshly-fetched
//   $BBBB). Rebuilt with no MUT: 3/1 restored.
//
// Four assertions, four independent mutations, each changing exactly one
// assertion's colour and none other; every one reverted cleanly.
