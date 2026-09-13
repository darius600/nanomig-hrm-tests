// ch7-overlay-source -- minimig conformance test
//
// Test purpose: where the overlay signal (`ovl`) comes from -- set by reset,
//   and specifically what write clears it. Package ch7-overlay-source,
//   fixture `minimig` (the whole minimig.v chipset module, per
//   AUFTRAG-ch7-overlay-source.md -- gary.v alone cannot carry this claim,
//   `ovl` is a free input there; minimig.v:165 is where it is a register).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   p.223. Claims file ch7-overlay-source.jsonl, 5 records (3 propositions
//   after twins are grouped). A red result below means minimig.v disagrees
//   with the manual, not that the assertion is wrong -- no assertion here
//   was, or should be, adjusted to make the RTL pass.
// Non-goals: this package does not model CIA-A's internal register file at
//   all. minimig.v:860-863's `ovl` mux does not read `rs` (register select)
//   or `data_out` -- only `sel_cia_a` (an address-range decode) and
//   `cpu_lwr|cpu_hwr` (a write strobe) -- so the specific bit pattern
//   written, and which CIA-A register is addressed, are provably
//   irrelevant to this signal and are not exercised beyond proving that.
//
// BOUNDARY: minimig.v ports cpu_address/cpu_r_w/_cpu_as/_cpu_uds/_cpu_lds/
//   cpudata_in/_cpu_dtack (the external 68000 bus), _cpu_reset_in/rst_ext/
//   _cpu_reset (the two reset inputs and the one derived CPU-reset output),
//   ovl (the signal under test, minimig.v:165), clk/clk7_en/clk7n_en/c1/c3/
//   cck/eclk (the clock-enable family minimig.v takes as plain inputs --
//   the real amiga_clk.v generator lives one level up and is not part of
//   this module's hierarchy; this harness reproduces its exact state
//   machine from amiga_clk.v, read for that purpose only, never modified).
//   Internally: minimig.v:861-862 `if(~_cpu_reset | ~_cpu_reset_in) ovl<=1;
//   else if(sel_cia_a & (cpu_lwr|cpu_hwr)) ovl<=0;`; gary.v:190-192
//   `sel_cia = cpu_address_in[23:16]==8'hBF; sel_cia_a = sel_cia &
//   ~cpu_address_in[12]; sel_cia_b = sel_cia & ~cpu_address_in[13];`;
//   minimig_syscontrol.v (the rst_ext/sof-counted `sys_reset` -> `_cpu_reset`
//   chain); minimig_m68k_bridge.v (the CPU bus protocol: `vpa`/`vma`/`eclk`
//   gate a CIA-space DTACK, `lwr`/`hwr` require `~l_dtack` -- the write
//   commit strobe happens in the brief window once DTACK is recognized, not
//   before, discovered empirically while building this harness below).
//
// ORDER OF WORK: SPECIFICATION.md (this package's 3 propositions, PROP-A/
// PROP-B/PROP-C) was written and committed from ch7-overlay-source-spec-
// stage.jsonl's quotations alone -- commit e258c9e -- before minimig.v,
// gary.v, minimig_syscontrol.v or minimig_m68k_bridge.v were opened.
//
// FIXTURE SIZE, MEASURED: this build Verilates 53 modules (11.1MB source)
// in ~4s. The `_cpu_reset` release this harness needs (see
// CONTROL-RESET-RELEASES below) needs on the order of 2.27M clk28 ticks --
// STEP3-NOTES-ch7-overlay-source.md reported 2,271,634 measured on its own
// diag_minimig harness; this harness's own independent measurement is
// printed at run time (see CONTROL-RESET-RELEASES's label) and is *not*
// hardcoded from that note. In wall-clock terms this cost about 2 seconds
// per full run in the environment this was built in -- large in tick count,
// cheap in wall time, so budgeting one long-lived Harness per binary
// (constructed once in main(), reused for every assertion below) rather
// than one per assertion was a real design choice, not an afterthought: a
// naive "fresh Harness per assertion" design in the style of some sibling
// packages' small, fast fixtures would have re-paid the reset cost 6+
// times per build and made the mutation matrix (7 separate builds)
// noticeably slower for no benefit, since nothing about the CIA-write
// assertions requires a fresh power-on.
//
// A HARNESS-CONSTRUCTION SURPRISE, recorded because it shaped the bus_cycle
// helper below: minimig_m68k_bridge.v's `_ta_n` (the register `_dtack`
// reads from) has no explicit initial value, so Verilator zero-inits it --
// `_cpu_dtack` reads falsely "already acknowledged" (0) from tick 0 until
// the first `_cpu_as` low->high edge forces the async `_ta_n<=1` in its
// `always @(posedge clk or posedge _as)` block. Without a deliberate throw-
// away edge at harness construction, a caller polling "wait for dtack==0"
// on its very first bus cycle would see a stale, meaningless 0 immediately
// and believe the cycle completed before the vpa/vma/eclk-gated CIA
// protocol ever ran -- exactly the kind of silently-dead-looking-alive
// harness path Chapter 6's DMA-grant lesson warns about, just on the
// control path instead of the data path. Caught here by comparing wait-tick
// counts across addresses before trusting any of them (see
// CONTROL-RESET-RELEASES / bus_cycle comments below), not assumed away.
//
// CLAIM COVERAGE: all 5 records of ch7-overlay-source.jsonl are accounted
// for below.
//   sol:7-reset.c3 (p.223, "the memory map is in a special state" on power-
//     on/external reset) -- NOT graded with expect(). See the note()/
//     record() pair before the graded assertions: no stimulus this harness
//     controls can make `ovl` read anything but 1 at cold start, because
//     minimig.v:861's `~_cpu_reset` term is already true from tick 0
//     regardless of `_cpu_reset_in` (minimig_syscontrol's own `sys_reset`
//     starts in the asserted state unconditionally) -- the claim is true
//     but not *isolable* from the internal reset chain at this boundary,
//     so it cannot carry a stimulus mutation of its own. Measured, not
//     graded, for that reason.
//   sol:7-reset.c8 + opus:7-reset.c4 (twins; p.223, "The OVL bit in one of
//     the 8520 Chips disables the overlay") -- carries TWO graded
//     assertions, deliberately not collapsed into one (the lead's
//     instruction): OVL-BIT-WRITE-CLEARS-OVERLAY (the sentence's plain
//     reading: a write that reaches the OVL bit clears the overlay) and
//     OTHER-CIA-A-REGISTER-ALSO-CLEARS (the sentence's other, narrower
//     half -- "THE OVL bit", not "any CIA-A register" -- reproducing
//     STEP3-NOTES-ch7-overlay-source.md's candidate finding independently
//     rather than trusting it).
//   sol:7-reset.c12 + opus:7-reset.c9 (twins; p.223, "The 68000 RESET
//     instruction works much like external reset or power on") -- worded,
//     per both claims' own `split`, as the reset LINE rather than the CPU
//     instruction (no fixture here decodes a 680x0 opcode). Graded as
//     RESET-LINE-RESTORES-OVERLAY.
//
// CONTROL CASES: CONTROL-RESET-RELEASES (no claim id; the harness's own
// reset chain must genuinely release within a bounded budget, or every
// later assertion is measuring a permanently-reset chip, not the CIA-write
// behaviour the manual describes). CIAB-WRITE-DOES-NOT-CLEAR and CHIPRAM-
// WRITE-DOES-NOT-CLEAR are the required "at least one control expected to
// PASS on the same measurement basis" for OTHER-CIA-A-REGISTER-ALSO-CLEARS
// (the suspect case, expected red): all three assertions read the same
// `ovl` signal after a bus write, through the same bus_cycle() path, with
// the same before/after comparison -- only the address differs.
//
// PRECONDITION: every "does this write/pulse change ovl" assertion below is
// preceded by its own graded expect() confirming ovl's value beforehand
// (ovl==1 before each of the four CIA-write assertions, ovl==0 before the
// reset-line assertion) -- per the lead's explicit correction, this is an
// expect(), not a comment, since a green "cleared/restored" reading is
// meaningless if the signal was never in the opposite state to begin with.
//
// RESULTS: baseline (no MUT flag) is 10 passed, 1 failed -- the DDRA
// finding below is the one red. Full pass/fail-by-assertion listing is in
// the report already sent to the lead.
//
// PER-ASSERTION MUTATION RECORD. Six variants, each its own full build+run
// (`make MUT=<switch>`, e.g. `make MUT=MUT_1`), already built, run,
// confirmed and reverted -- not re-run for this header update. Format
// (tools/check_gates.py): // MUTATION <assertion-label> <oracle|stimulus|
// none> <red|green|impossible> <what changed>. Every mutation below is
// `stimulus` -- what the module is asked to do -- never `oracle`; the
// expected value in the matching expect() is never touched, per
// BUILD-BRIEF.md's "flipping an expected value proves nothing".
//
// MUTATION CONTROL-RESET-RELEASES stimulus red MUT_RESET holds rst_ext (master reset) asserted forever, so minimig_syscontrol.v's rst_cnt never advances and _cpu_reset never rises within the 3,000,000-tick budget; baseline PASS (measured tick=2,273,642) goes red.
// MUTATION OVL-BIT-WRITE-CLEARS-OVERLAY stimulus red MUT_1 issues a read instead of a write to the same CIA-A PRA address ($BFE001), so cpu_lwr/cpu_hwr never assert; baseline PASS (ovl clears) goes red (ovl stays 1).
// MUTATION OTHER-CIA-A-REGISTER-ALSO-CLEARS stimulus green MUT_2 skips the CIA-A DDRA write ($BFE201) entirely -- no bus cycle at all. This is the one assertion that is ALREADY red at baseline (the manual/RTL disagreement this package exists to report), so its mutation runs the opposite direction from every other assertion here: a red claim's mutation must go GREEN under a genuinely different stimulus, or the red would be unfalsifiable/vacuous instead of a real measurement. With no CIA-A access at all, ovl stays at its precondition value (1), so "ovl==1 after" reads green -- proof the baseline FAIL is attributable to the write happening, not to an always-red assertion. (An earlier draft of this mutation swapped DDRA for PRA instead; that was rejected before being recorded here because both addresses already clear ovl identically on this RTL, so the swap changes nothing observable and would not have been a real mutation -- see the inline comment at MUT_2 in the body.)
// MUTATION CIAB-WRITE-DOES-NOT-CLEAR stimulus red MUT_3 redirects the same write from CIA-B ($BFD000) to CIA-A PRA ($BFE001); baseline PASS (ovl stays set) goes red (ovl clears).
// MUTATION CHIPRAM-WRITE-DOES-NOT-CLEAR stimulus red MUT_4 redirects the same write from chip RAM ($000100) to CIA-A PRA ($BFE001); baseline PASS (ovl stays set) goes red (ovl clears).
// MUTATION RESET-LINE-RESTORES-OVERLAY stimulus red MUT_5 skips the _cpu_reset_in low/high pulse entirely; baseline PASS (ovl restored to 1) goes red (ovl stays at 0).

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include "Vminimig.h"
#include "verilated.h"

#ifndef MUT_RESET
#define MUT_RESET 0
#endif
#ifndef MUT_1
#define MUT_1 0
#endif
#ifndef MUT_2
#define MUT_2 0
#endif
#ifndef MUT_3
#define MUT_3 0
#endif
#ifndef MUT_4
#define MUT_4 0
#endif
#ifndef MUT_5
#define MUT_5 0
#endif

namespace {

// Reproduces amiga_clk.v's exact register update (read for this purpose
// only; amiga_clk.v itself is excluded from this build -- it, and the
// actual 68000 core, live one level above minimig.v in the real SoC and
// are not part of this module's hierarchy). Values computed here are
// presented to the DUT's clock-enable input pins starting the tick AFTER
// the edge that produced them, matching amiga_clk.v's own registered
// (non-blocking) semantics: consumers on the same clk edge see the
// PREVIOUS cycle's clock-enable state, never the one just computed.
struct ClkGen {
    uint8_t clk7_cnt = 2;   // amiga_clk.v reset value 2'b10
    bool clk7_en = true, clk7n_en = true, cck = true;
    uint16_t shifter = 1;   // one-hot, amiga_clk.v reset value
    bool c1 = false, c3 = false;
    void step() {
        uint8_t old_cnt = clk7_cnt;
        bool old_c3 = c3;
        bool old_cck = cck;
        uint16_t old_shifter = shifter;
        bool old_clk7 = (old_cnt >> 1) & 1;
        clk7_cnt = (old_cnt + 1) & 0x3;
        clk7_en = (old_cnt == 0);
        clk7n_en = (old_cnt == 2);
        if (old_cnt == 1) {
            cck = !old_cck;
            uint16_t rotated = (uint16_t)(((old_shifter << 1) | (old_shifter >> 9)) & 0x3FF);
            shifter = rotated ? rotated : 1;
        } else {
            cck = old_cck;
            shifter = old_shifter;
        }
        c3 = old_clk7;
        c1 = !old_c3;
    }
};

// CIA-A responds on the low byte lane (odd byte addresses, LDS) and CIA-B
// on the high byte lane (even, UDS) -- minimig.v:617-618 vs :642-643
// (ciaa.v data_out feeds cpu_data_out[7:0], ciab.v feeds [15:8]).
constexpr uint32_t CIAA_PRA      = 0xBFE001; // CIA-A PRA -- the OVL bit lives here (manual p.223 + Appendix F)
constexpr uint32_t CIAA_DDRA     = 0xBFE201; // CIA-A DDRA -- CIA-A, but NOT the OVL bit
constexpr uint32_t CIAB_PRA      = 0xBFD000; // CIA-B -- a different chip entirely
constexpr uint32_t CHIPRAM_ADDR  = 0x000100; // not CIA space at all

class Harness {
public:
    Vminimig* dut;
    ClkGen cg;
    long long tickcount = 0;
    bool reset_released = false;
    long long reset_release_tick = -1;

    Harness() : dut(new Vminimig) {
        dut->clk = 0;
        dut->clk7_en = cg.clk7_en; dut->clk7n_en = cg.clk7n_en;
        dut->c1 = cg.c1; dut->c3 = cg.c3; dut->cck = cg.cck; dut->eclk = cg.shifter;
        dut->rst_ext = 0;
        dut->_cpu_reset_in = 0;
        dut->cpu_address = 0; dut->cpu_r_w = 1;
        dut->_cpu_as = 1; dut->_cpu_uds = 1; dut->_cpu_lds = 1;
        dut->cpudata_in = 0;
        dut->memory_config = 0; dut->chipset_config = 0; dut->floppy_config = 0;
        dut->nmi_addr = 0; dut->chip48 = 0; dut->ramdata_in = 0;
        dut->rxd = 1; dut->cts = 1; dut->dsr = 1; dut->cd = 1; dut->ri = 1;
        dut->_joy1 = 0xFFFF; dut->_joy2 = 0xFFFF; dut->_joy3 = 0xFFFF; dut->_joy4 = 0xFFFF;
        dut->joya1 = 0; dut->joya2 = 0; dut->mouse_btn = 0;
        dut->kms_level = 0; dut->kbd_mouse_type = 0; dut->kbd_mouse_data = 0;
        dut->sdc_img_mounted = 0; dut->sdc_img_size = 0; dut->sdc_busy = 0; dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0; dut->sdc_byte_addr = 0; dut->sdc_byte_in_data = 0;
        dut->cpucfg = 0;

        // Harness hygiene (see file header): force one clean _as low->high
        // edge before anything relies on _cpu_dtack, so bus_cycle()'s
        // wait-for-dtack loops measure the real protocol, not Verilator's
        // zero-init of minimig_m68k_bridge.v's un-reset `_ta_n` latch.
        dut->_cpu_as = 0; tick(); tick();
        dut->_cpu_as = 1; tick(); tick();

#if MUT_RESET
        // MUTATION for CONTROL-RESET-RELEASES: hold master reset asserted
        // forever. minimig_syscontrol.v:33 "if(mrst) rst_cnt<=0" then never
        // lets rst_cnt advance, so sys_reset (and therefore _cpu_reset)
        // must never release within the budget below.
        dut->rst_ext = 1;
#endif
        // _cpu_reset_in released early (matches STEP3-NOTES' own harness
        // procedure) so Agnus's free-running beam counters (agnus_beam-
        // counter.v's hpos/vpos -- NOT gated by `reset` at all, verified by
        // reading the file: no `if(reset)` clause touches either counter)
        // can produce the `sof` pulses minimig_syscontrol.v needs to count
        // down. Holding it low first models power-on/external reset.
        for (int i = 0; i < 8; ++i) tick();
        dut->_cpu_reset_in = 1;

        const long long BUDGET = 3000000; // STEP3 notes measured 2,271,634; budget with headroom
        for (long long i = 0; i < BUDGET && !reset_released; ++i) {
            tick();
            if (dut->_cpu_reset == 1) {
                reset_released = true;
                reset_release_tick = tickcount;
            }
        }
        for (int i = 0; i < 8; ++i) tick();
    }
    ~Harness() { delete dut; }

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        cg.step();
        dut->clk7_en = cg.clk7_en; dut->clk7n_en = cg.clk7n_en;
        dut->c1 = cg.c1; dut->c3 = cg.c3; dut->cck = cg.cck; dut->eclk = cg.shifter;
        ++tickcount;
    }

    // Drives one 68000-style bus cycle to byte_addr. is_write selects a
    // write (r_w=0) or read (r_w=1) cycle -- MUT_1's hook. Byte lane
    // (uds/lds) follows the address's own parity, matching CIA-A(odd)/
    // CIA-B(even) convention above; irrelevant for non-CIA addresses.
    // minimig_m68k_bridge.v's `lwr`/`hwr` require `~l_dtack` (the write-
    // commit strobe happens in the window right after DTACK is
    // recognized, not before -- discovered empirically while building
    // this harness: an earlier version dropped _as the instant dtack was
    // first seen and every CIA-A write silently failed to clear `ovl`),
    // so the cycle is held open a little longer after dtack before
    // releasing _as.
    bool bus_cycle(uint32_t byte_addr, bool is_write) {
        bool low_byte = (byte_addr & 1u) != 0;
        dut->cpu_address = (byte_addr >> 1) & 0x7FFFFFu;
        dut->cpu_r_w = is_write ? 0 : 1;
        dut->cpudata_in = 0;
        if (low_byte) { dut->_cpu_uds = 1; dut->_cpu_lds = 0; }
        else          { dut->_cpu_uds = 0; dut->_cpu_lds = 1; }
        dut->_cpu_as = 0;
        bool dtack_seen = false;
        for (int i = 0; i < 4000 && !dtack_seen; ++i) {
            tick();
            if (dut->_cpu_dtack == 0) dtack_seen = true;
        }
        for (int i = 0; i < 16; ++i) tick();
        dut->_cpu_as = 1; dut->_cpu_uds = 1; dut->_cpu_lds = 1; dut->cpu_r_w = 1;
        for (int i = 0; i < 8; ++i) tick();
        return dtack_seen;
    }
    bool write_word(uint32_t byte_addr) { return bus_cycle(byte_addr, true); }
    bool read_word(uint32_t byte_addr)  { return bus_cycle(byte_addr, false); }

    // Sets ovl=1 via the reset line alone, AFTER the long power-on reset
    // has already released (_cpu_reset==1) -- the one point where
    // _cpu_reset_in's own contribution is isolated from minimig_syscontrol's
    // internal chain (see the sol:7-reset.c3 note()/record() in main()).
    // Used both as RESET-LINE-RESTORES-OVERLAY's own stimulus and as the
    // ovl==1 precondition-setter for the CIA-write assertions.
    void pulse_reset_line() {
        dut->_cpu_reset_in = 0;
        tick(); tick();
        dut->_cpu_reset_in = 1;
        for (int i = 0; i < 8; ++i) tick();
    }
};

// The fixed assertion harness (build brief): expect() grades, note()
// measures without grading, record() declines with a reason.
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

} // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;
    Harness h;

    // =========================================================================
    // CONTROL-RESET-RELEASES (no claim id; harness precondition). If this
    // never goes green, `~_cpu_reset` (minimig.v:861) permanently forces
    // ovl<=1 regardless of any CIA write, and every assertion below would
    // silently be measuring a permanently-reset chip -- the control-path
    // analogue of Chapter 6's dead-data-path lesson.
    // =========================================================================
    {
        char tickbuf[24];
        std::snprintf(tickbuf, sizeof tickbuf, "%lld", h.reset_release_tick);
        checks.expect(
            h.reset_released,
            std::string("CONTROL-RESET-RELEASES: _cpu_reset rose within a 3,000,000-tick "
                        "budget after rst_ext/_cpu_reset_in were released (measured "
                        "tick=") + tickbuf + ", or -1 if the budget was exhausted). "
                        "STEP3-NOTES-ch7-overlay-source.md reported 2,271,634 for the "
                        "same transition under its own harness; this is this harness's "
                        "own independent measurement, not that number adopted. No "
                        "manual page (harness precondition).");
    }

    if (!h.reset_released) {
        std::cout << "\nHALT-CANDIDATE: reset never released within budget; no "
                     "CIA-write assertion below would be measuring anything but a "
                     "permanently-reset chip, so none are run.\n";
        std::cout << checks.passed << " passed, " << checks.failed << " failed\n";
        return checks.failed == 0 ? 0 : 1;
    }

    // =========================================================================
    // sol:7-reset.c3 (p.223) -- measured, not graded. See file header and
    // the reasoning below: no stimulus this harness controls can make ovl
    // read anything but 1 at cold start, because minimig.v:861's
    // `~_cpu_reset` term is already true from tick 0 regardless of
    // `_cpu_reset_in` (verified separately: driving `_cpu_reset_in` high
    // from tick 0 onward, never asserting it at all, still gives ovl=1 at
    // every one of the first 10 ticks, because `_cpu_reset` itself starts
    // deasserted-as-in-reset independent of that pin). The claim is TRUE on
    // this fixture; it just cannot be paired with a stimulus mutation that
    // would make it read otherwise, so it is not asserted with expect().
    // =========================================================================
    checks.note(
        "OVL-AT-COLD-START (p.223, sol:7-reset.c3): ovl reads 1 from "
        "simulation start regardless of _cpu_reset_in -- verified by a "
        "separate run that never asserts _cpu_reset_in low at all, where "
        "ovl still reads 1 for the first 10 ticks, because minimig.v:861's "
        "`~_cpu_reset` term is already true from tick 0 (minimig_syscontrol's "
        "own sys_reset starts in the asserted state unconditionally).");
    checks.record(
        "sol:7-reset.c3 (\"When the Amiga is turned on or externally reset, "
        "the memory map is in a special state.\", p.223) is not graded with "
        "expect() here: on this fixture, ovl's cold-start value is confounded "
        "with minimig_syscontrol's internal reset chain and is not isolable "
        "from it by any stimulus this harness controls, so no mutation of "
        "_cpu_reset_in (or rst_ext) can make this specific reading disagree. "
        "The isolated, mutation-graded test of \"a reset condition sets/"
        "restores ovl\" is RESET-LINE-RESTORES-OVERLAY below, using the same "
        "_cpu_reset_in pin once _cpu_reset has already released and the two "
        "paths are no longer confounded.");

    // =========================================================================
    // OVL-BIT-WRITE-CLEARS-OVERLAY (p.223) -- sol:7-reset.c8, opus:7-reset.c4.
    // "The OVL bit in one of the 8520 Chips disables the overlay." A write
    // that reaches CIA-A's PRA (where the OVL bit lives, manual + Appendix F)
    // clears ovl.
    // =========================================================================
    {
        h.pulse_reset_line();
        checks.expect(h.dut->ovl == 1,
            "OVL-BIT-WRITE-CLEARS-OVERLAY precondition: ovl==1 before the "
            "CIA-A PRA write (established via the reset line) -- a green "
            "\"cleared\" reading below would mean nothing if ovl were never "
            "set to begin with.");

#if MUT_1
        // MUTATION: read instead of write. Same address (still selects
        // CIA-A/the OVL-bit register), cpu_r_w left at 1 (read) so
        // minimig_m68k_bridge.v's `lwr`/`hwr` never assert -- the module is
        // asked to READ the OVL-bit register, not write it. Expected value
        // (ovl==0 after) is left alone.
        h.read_word(CIAA_PRA);
#else
        h.write_word(CIAA_PRA);
#endif
        checks.expect(h.dut->ovl == 0,
            "OVL-BIT-WRITE-CLEARS-OVERLAY (p.223): a write to CIA-A PRA "
            "($BFE001, the OVL bit's register) clears ovl -- \"The OVL bit "
            "in one of the 8520 Chips disables the overlay\" (p.223, "
            "restored quote adds \"(See Appendix F for the bit location)\"). "
            "sol:7-reset.c8, opus:7-reset.c4. "
            "MUTATION: read instead of write at the same address -- ovl "
            "must stay 1.");
    }

    // =========================================================================
    // OTHER-CIA-A-REGISTER-ALSO-CLEARS (p.223) -- same two claims' other,
    // narrower half: "THE OVL bit" (not "any CIA-A register") disables the
    // overlay. Reproducing STEP3-NOTES-ch7-overlay-source.md's candidate
    // finding independently: minimig.v:862 keys on `sel_cia_a &
    // (cpu_lwr|cpu_hwr)` alone -- it never reads `cpu_address_out[11:8]`
    // (register select) or the data written -- so a write to CIA-A DDRA
    // (not the OVL bit at all) is expected, by the manual's specific
    // wording, NOT to clear ovl. A red result here is the RTL disagreeing
    // with the manual, not a wrong assertion.
    // =========================================================================
    {
        h.pulse_reset_line();
        checks.expect(h.dut->ovl == 1,
            "OTHER-CIA-A-REGISTER-ALSO-CLEARS precondition: ovl==1 before "
            "the CIA-A DDRA write.");

#if MUT_2
        // MUTATION: do not perform the DDRA write at all. Baseline on this
        // RTL is already red (ovl clears on ANY CIA-A write, DDRA
        // included -- that is the finding), so swapping DDRA for PRA
        // would not move this assertion: both addresses clear ovl
        // identically on this RTL, which would make that swap a non-
        // mutation (same outcome, not a real stimulus change as far as
        // this assertion can tell). Skipping the write instead is the
        // change that actually moves the needle: with no CIA-A access at
        // all, ovl must stay at its precondition value (1), flipping this
        // specific assertion from red (baseline) to green -- proof the
        // FAIL above is attributable to the write happening, not to a
        // vacuous/always-red assertion.
#else
        h.write_word(CIAA_DDRA);
#endif
        checks.expect(h.dut->ovl == 1,
            "OTHER-CIA-A-REGISTER-ALSO-CLEARS (p.223): a write to CIA-A "
            "DDRA ($BFE201 -- CIA-A, but NOT the OVL bit) leaves ovl set, "
            "per \"THE OVL bit ... disables the overlay\" (p.223) naming "
            "one specific bit, not the whole chip's address range. "
            "sol:7-reset.c8, opus:7-reset.c4 (same sentence's narrower "
            "half). MUTATION: do not perform the DDRA write at all -- ovl "
            "must then stay at 1 (see the MUT_2 comment above for why "
            "swapping DDRA for PRA would not have been a real mutation "
            "on this RTL).");
    }

    // =========================================================================
    // CIAB-WRITE-DOES-NOT-CLEAR (control, no claim id beyond the same
    // sentence's implicit chip-identity scoping) -- same measurement basis
    // as OTHER-CIA-A-REGISTER-ALSO-CLEARS: a write to CIA-B, a different
    // chip entirely (gary.v:192 sel_cia_b, gated on address bit 13 instead
    // of 12), must not clear ovl.
    // =========================================================================
    {
        h.pulse_reset_line();
        checks.expect(h.dut->ovl == 1,
            "CIAB-WRITE-DOES-NOT-CLEAR precondition: ovl==1 before the "
            "CIA-B write.");

#if MUT_3
        // MUTATION: redirect to CIA-A PRA instead of CIA-B -- same "expect
        // ovl==1 after" check, now asked to hit the chip that SHOULD clear
        // it.
        h.write_word(CIAA_PRA);
#else
        h.write_word(CIAB_PRA);
#endif
        checks.expect(h.dut->ovl == 1,
            "CIAB-WRITE-DOES-NOT-CLEAR (control, p.223): a write to CIA-B "
            "($BFD000, a different chip -- gary.v's sel_cia_b, not "
            "sel_cia_a) leaves ovl set. Same measurement basis (bus write, "
            "then read ovl) as OTHER-CIA-A-REGISTER-ALSO-CLEARS, the "
            "suspect case above. MUTATION: redirect the same write to "
            "CIA-A PRA -- ovl must then clear.");
    }

    // =========================================================================
    // CHIPRAM-WRITE-DOES-NOT-CLEAR (control) -- a write nowhere near any
    // CIA at all must not clear ovl.
    // =========================================================================
    {
        h.pulse_reset_line();
        checks.expect(h.dut->ovl == 1,
            "CHIPRAM-WRITE-DOES-NOT-CLEAR precondition: ovl==1 before the "
            "chip-RAM write.");

#if MUT_4
        h.write_word(CIAA_PRA);
#else
        h.write_word(CHIPRAM_ADDR);
#endif
        checks.expect(h.dut->ovl == 1,
            "CHIPRAM-WRITE-DOES-NOT-CLEAR (control, p.223): a write to "
            "chip RAM ($000100, no CIA selected at all) leaves ovl set. "
            "Same measurement basis as OTHER-CIA-A-REGISTER-ALSO-CLEARS. "
            "MUTATION: redirect the same write to CIA-A PRA -- ovl must "
            "then clear.");
    }

    // =========================================================================
    // RESET-LINE-RESTORES-OVERLAY (p.223) -- sol:7-reset.c12, opus:7-reset.c9.
    // "The 68000 RESET instruction works much like external reset or power
    // on." Worded, per both claims' own split, as the reset LINE: this
    // fixture exposes exactly one such line, _cpu_reset_in, and does not
    // distinguish "power-on", "external reset" or "CPU RESET instruction"
    // as separate sources -- all three are, at this boundary, the same
    // pin. Tested here in true isolation from minimig_syscontrol's own
    // reset chain: _cpu_reset has already released (CONTROL-RESET-RELEASES,
    // above), so only _cpu_reset_in is doing the work, unlike sol:7-reset.c3
    // at cold start.
    // =========================================================================
    {
        h.write_word(CIAA_PRA); // clear ovl first (reuses the already-proven mechanism)
        checks.expect(h.dut->ovl == 0,
            "RESET-LINE-RESTORES-OVERLAY precondition: ovl==0 before the "
            "reset-line pulse (established via a CIA-A PRA write) -- a "
            "green \"restored\" reading below would mean nothing if ovl "
            "were never cleared to begin with.");

#if MUT_5
        // MUTATION: do not pulse the reset line at all. Expected value
        // (ovl==1 after) is left alone -- ovl must then stay at 0.
#else
        h.pulse_reset_line();
#endif
        checks.expect(h.dut->ovl == 1,
            "RESET-LINE-RESTORES-OVERLAY (p.223): pulsing _cpu_reset_in "
            "low then high again, AFTER _cpu_reset has already released, "
            "restores ovl to 1 from a running state where a CIA-A write "
            "had cleared it -- \"The 68000 RESET instruction works much "
            "like external reset or power on\" (p.223), worded as the "
            "reset line per both claims' own split. sol:7-reset.c12, "
            "opus:7-reset.c9. MUTATION: do not pulse the reset line -- ovl "
            "must stay at 0.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
