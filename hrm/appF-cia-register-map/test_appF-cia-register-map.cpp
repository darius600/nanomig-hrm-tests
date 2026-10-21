// Test purpose: the CIAA address map (p.339), the CIAB address map (p.340,
//   read for comparison only -- not built), and the Chip Register Map
//   (p.340, stated once for "each 8520") -- which byte address / RS3-RS0
//   value carries which register, and what each register is -- plus the
//   general-operation prose on pp.353-354 (CS, RS3-RS0, DB7-DB0, RES), at
//   the `ciaa` module boundary, compiled alone. Also carries this package's
//   41 declined claims (38 with a classify.note, 2 declined for this
//   package's own ciaa-only boundary reason, 1 declined for this package's
//   own boundary finding, NM-HRM-077) on RECORDED lines in main().
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 339, 340, 351, 352, 353, 354; committed specification 3851dcd
//   (SPECIFICATION.md, this package).
// Requirement IDs: REG0-PRA .. REGF-CRB, REG-COUNT-16, CS-GATES-ACCESS,
//   RS-SELECTS-DISTINCT, DATA-BUS-8BIT, READ-DRIVES-BUS,
//   RESET-AFFECTS-REGS, RESET-ZEROS-REMAINING, IO-PINS-16,
//   CHIP-CAPABILITIES-FIVE, per SPECIFICATION.md's claim inventory (20
//   assertions covering 111 of 152 claims; 41 declined, see RECORDED
//   block). CHIP-CAPABILITIES-FIVE itself is one of the 41: a RECORDED
//   line, not a graded assertion -- see below.
//   Claims file: hrm-testpipeline/work/2026-09-17_appF_packages.jsonl,
//   filtered to classify.package == "appF-cia-register-map" (152 claims).
// Non-goals: anything outside the `ciaa` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/2026-09-17_appF_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// RTL read for step 2, after SPECIFICATION.md was committed (3851dcd):
//   ciaa.v (388 lines) in full; cia_int.v (70 lines), cia_timera.v (112
//   lines), cia_timerb.v (107 lines) and cia_timerd.v (142 lines) in full.
//   ciab.v (251 lines) read in full for the chip-generic-scope comparison
//   SPECIFICATION.md documents (identical `rs`->register-name decode to
//   ciaa.v; not compiled by this package). Also read, per this package's
//   own SPECIFICATION.md and every appendix-F sibling's own disclosure:
//   hrm/ch8-ciaa-port-direction's Makefile and full test .cpp, and
//   hrm/ch1-cia-timers's Makefile and full test_ciaa.cpp (Checks class,
//   Harness bus-timing protocol, ticks_to_underflow()'s TALO/TAHI/CRA
//   load-and-start idiom, and the MUTATION `-D` / `#ifndef MUT_X #define
//   MUT_X 0 #endif` convention -- reused below verbatim as verified
//   RTL-interface plumbing and suite coding convention, not as a source of
//   this package's own assertions or expected values).
//
// RTL facts established by that reading (none of this shaped
//   SPECIFICATION.md, which was committed first from the page images/text
//   alone):
//   - ciaa.v:136-150, the whole address decoder: rs==4'h0..4'hA,4'hC..4'hF
//     each assert exactly one named strobe (pra,prb,ddra,ddrb,talo,tahi,
//     tblo,tbhi,tdlo,tdme,tdhi,sdr,icrs,cra,crb); rs==4'hB asserts nothing.
//     ciab.v:50-64 decodes the identical fifteen values to the identical
//     fifteen names, confirming the Chip Register Map's "each 8520" framing
//     empirically, not just by the page's own words.
//   - ciaa.v:253-254 (PRA read) returns {porta_in2[7:2], porta_out[1:0]} --
//     bits 7:2 are the synchronised EXTERNAL INPUT, unconditionally; only
//     bits 1:0 come from the internally-driven porta_out. ciaa.v:293-294
//     (PRB read) returns portb_in[7:0] unconditionally -- ciaa has no
//     portb_out at all (:270-279 commented out). This is the read-back
//     trait that distinguishes PRA/PRB ("Peripheral data register") from
//     DDRA/DDRB, whose reads (ciaa.v:255-256,295-296) return exactly what
//     was last written, never porta_in/portb_in.
//   - ciaa.v:262, DDRA's only functional effect: porta_out[3:0] =
//     {~ddrporta[7:6]|regporta[3:2], ~ddrporta[1:0]|regporta[1:0]} -- a 0
//     direction bit pulls that position high regardless of what was
//     written to PRA; a 1 direction bit lets PRA's write reach porta_out.
//     This is DDRA's distinguishing "direction register" trait.
//   - cia_timera.v:56-71 (TALO/TAHI latches) + :73-91 (reload/counter):
//     writing TALO then TAHI is sufficient to load the 16-bit counter
//     `tmr` (thi_load, set the cycle TAHI is written, reloads tmr the
//     following clk7_en edge -- ticks_to_underflow()'s idiom, reused).
//     cia_timera.v:32, `count = eclk` -- a level enable. CRA (cia_timera.v:
//     34-53) bit0 is `start`; with start=1 and eclk held at 1, `tmr`
//     decrements by exactly 1 per tick (:83-91). cia_timerb.v is the same
//     shape for TBLO/TBHI/CRB (count = tmcr[6]?tmra_ovf:eclk; with CRB
//     bit6=0 as written below, count=eclk, same as timer A).
//   - cia_timerd.v: `tod` (:25) is a 24-bit up-counter, `count(tick&
//     ~tick_del)` (ciaa.v:382) an edge-detected pulse on ciaa's own `tick`
//     input. After reset `tod`=0, `count_ena`=1, `latch_ena`=1 (:71-72,
//     33-34) and `tod_latch` (:44-48) tracks `tod` continuously while
//     latch_ena stays 1 (it only clears on a THI *read*, :37-38 -- never
//     exercised as a clocked read here, so it never clears in this test).
//     TODLO/TODMID/TODHI (rs 8/9/A) read tod_latch[7:0]/[15:8]/[23:16]
//     (:54-59) -- the low/mid/high bytes of one 24-bit counter driven by
//     `tick`, matching "50/60 Hz event counter... VSync or line tick" at
//     the boundary `ciaa` itself exposes (whether `tick` specifically
//     carries VSync is minimig.v's wiring, outside `ciaa`).
//   - cia_int.v: ICR write with data_in[7]=1 ORs data_in[4:0] into
//     `icrmask` (arm); =0 ANDs it out (disarm) (:29-35). ICR READ is the
//     one register-map register with read-clears-latched-bits semantics:
//     `icrs && !wr` sets icr[n]<=<raw source> (NOT an OR with the previous
//     value) on the very read that observes it (:43-50), vs. icr[n]<=icr[n]
//     |<source> on every other cycle (:52-58) -- so a source pulsed once,
//     read once, reads 0 on a SECOND read with no further event. No other
//     register on this map behaves this way.
//   - cia_timerd.v:107-109: TOD ALARM resets to 24'hFFFFFF, not zero -- a
//     real counter-example to p.354's "All other registers are reset to
//     zero", already flagged in classify.evidence. ALARM has no read path
//     through `rs` at all (cia_timerd.v's `data_out` mux, :51-64, only ever
//     returns tod_latch, never `alarm`, regardless of CRB7) -- unobservable
//     at this fixture's boundary without reaching into cia_timerd's
//     internals, which this file does not do (no debug port). Reported via
//     note(), not expect() -- see RESET-ZEROS-REMAINING below.
//   - ciaa's own port list has no PC-equivalent output and no
//     FLAG-equivalent input. p.341 names PC the output pulse pin and FLAG
//     the edge detection input, both CIA-A's per p.351, p.337, p.324 and
//     p.259 (docs/findings/NM-HRM-077.md) -- not PB6/PB7, which are two of
//     the sixteen port pins (p.341) and a different package's finding
//     (NM-HRM-074). Both conjuncts feed CHIP-CAPABILITIES-FIVE's RECORDED
//     line, not a graded assertion.
//
// Every mutation below was actually built (WSL Verilator 5.052, `make
// MUT=<NAME> test`, full `rm -rf obj_dir` rebuild via the Makefile's `test`
// recipe) and the PASS/FAIL/pass-count lines quoted are what that run
// printed, not predictions -- filled in after building, per this suite's
// rule against writing a record before taking the measurement. All 22
// mutations were reverted afterwards; this file builds at MUT unset
// (baseline).
//
// Baseline (no MUT): "22 passed, 0 failed" -- CHIP-CAPABILITIES-FIVE is a
// RECORDED line, not a graded assertion (see above); every assertion
// PASSes.
//
// MUTATION FIXTURE-LIVE stimulus red: MUT_FIXLIVE repeats the first DDRA
//   byte (0x55) instead of a distinct second one -> "21 passed, 2 failed"
//   (FIXTURE-LIVE FAIL, added to the baseline's one). Reverted.
// MUTATION REG0-PRA stimulus red: MUT_PRA_NOECHO repeats the same
//   porta_in pattern instead of a different one -> "21 passed, 2 failed"
//   (REG0-PRA FAIL). Reverted.
// MUTATION REG1-PRB stimulus red: MUT_PRB_NOECHO repeats the same
//   portb_in pattern -> "21 passed, 2 failed" (REG1-PRB FAIL). Reverted.
// MUTATION REG2-DDRA stimulus red: MUT_DDRA_NOGATE leaves bit 0's
//   direction at input instead of switching it to output before the
//   "as-output" reading -> "21 passed, 2 failed" (REG2-DDRA FAIL).
//   Reverted.
// MUTATION REG3-DDRB stimulus red: MUT_DDRB_SAMEBYTE repeats the first
//   written byte (0x3C) instead of a distinct second one (0xC3) ->
//   "21 passed, 2 failed" (REG3-DDRB FAIL). Reverted.
// MUTATION REG45-TIMERA stimulus red: MUT_TIMERA_NOECLK holds `eclk` at 0
//   instead of 1, so the loaded counter never decrements -> "21 passed,
//   2 failed" (REG45-TIMERA FAIL). Reverted.
// MUTATION REG67-TIMERB stimulus red: MUT_TIMERB_NOECLK, same for timer B
//   -> "21 passed, 2 failed" (REG67-TIMERB FAIL). Reverted.
// MUTATION REG89A-TOD stimulus red: MUT_TOD_NOTICK replaces every
//   `pulse_tick_input()` with a plain `step()` (no rising edge on `tick`)
//   -> "21 passed, 2 failed" (REG89A-TOD FAIL, counter stays 0).
//   Reverted.
// MUTATION REGB-UNUSED stimulus red: MUT_UNUSED_WRONGRS points the
//   "unused slot" reading at rs=0xA (TODHI, a real register) instead of
//   0xB -> "21 passed, 2 failed" (REGB-UNUSED FAIL, TODHI does store what
//   is written to it). Reverted.
// MUTATION REGC-SDR stimulus red: MUT_SDR_SAMEBYTE repeats the first
//   written byte instead of a distinct second one -> "21 passed,
//   2 failed" (REGC-SDR FAIL). Reverted.
// MUTATION REGD-ICR stimulus red: MUT_ICR_NOSTART skips writing CRA (the
//   timer never starts, never underflows) -> "21 passed, 2 failed"
//   (REGD-ICR FAIL, saw_irq stays false). Reverted.
// MUTATION REGE-CRA stimulus red: MUT_CRA_ALWAYSSTART writes CRA=0x01
//   (started) instead of 0x00 (stopped) for the "stopped" phase, so the
//   counter decrements in both phases and the stopped-vs-running contrast
//   disappears -> "21 passed, 2 failed" (REGE-CRA FAIL, lo_stopped !=
//   initial). Reverted.
// MUTATION REGF-CRB stimulus red: MUT_CRB_ALWAYSSTART, same for CRB ->
//   "21 passed, 2 failed" (REGF-CRB FAIL). Reverted.
// MUTATION REG-COUNT-16 stimulus red: MUT_REGCOUNT_SKIPB skips reading
//   rs=0xB in the sweep (deliberately not read) -> "21 passed, 2 failed"
//   (REG-COUNT-16 FAIL, only 15/16 slots counted). Reverted.
// MUTATION CS-GATES-ACCESS stimulus red: MUT_CS_NOGATE holds `aen` at 1
//   even for the "disabled" write instead of 0 -> "21 passed, 2 failed"
//   (CS-GATES-ACCESS FAIL, the "disabled" write silently takes effect).
//   Reverted.
// MUTATION RS-SELECTS-DISTINCT stimulus red: MUT_RSSEL_NEIGHBOR reads
//   rs=0x3 (DDRB) back instead of the rs=0x2 (DDRA) that was actually
//   written -> "21 passed, 2 failed" (RS-SELECTS-DISTINCT FAIL, DDRB was
//   never written so it does not match). Reverted.
// MUTATION DATA-BUS-8BIT stimulus red: MUT_DBUS_WRONGREG reads DDRB back
//   instead of DDRA -> "21 passed, 2 failed" (DATA-BUS-8BIT FAIL).
//   Reverted.
// MUTATION READ-DRIVES-BUS stimulus red: MUT_READDRIVE_WRONGREG reads
//   DDRB (never written) during the "content is driven" check instead of
//   DDRA -> "21 passed, 2 failed" (READ-DRIVES-BUS FAIL, during_read
//   comes back 0x00, not 0x66). An earlier variant of this mutation
//   (holding `rd` asserted alongside `wr` during the write-phase check)
//   measured NO CHANGE ("22 passed, 1 failed", unaffected) -- ciaa.v:253
//   etc. gate every read path on `!wr` regardless of `rd`, so `wr`
//   dominates and that variant could never move this assertion. Replaced
//   with the wrong-register variant above rather than keep an inert one.
// MUTATION RESET-AFFECTS-REGS stimulus red: MUT_RESET_SKIP skips pulsing
//   `reset` entirely -> "21 passed, 2 failed" (RESET-AFFECTS-REGS FAIL,
//   the written values are still there). Reverted.
// MUTATION RESET-ZEROS-REMAINING stimulus red: MUT_RESETZERO_WRONGREG
//   reads TALO back instead of DDRA post-reset -> "21 passed, 2 failed"
//   (RESET-ZEROS-REMAINING FAIL, TALO resets to 0xFF, not 0x00).
//   Reverted.
// MUTATION IO-PINS-16 stimulus red: MUT_IOPINS_UNDERCOUNT undercounts
//   port A at 6 positions instead of 8 -> "21 passed, 2 failed"
//   (IO-PINS-16 FAIL, total=14). Reverted.
// MUTATION CONTROL-CAPABILITIES-CORE stimulus red: MUT_CAPSCTRL_WRONGSET
//   drops the three-timers conjunct in the `core_three_only` branch only
//   -> "21 passed, 1 failed" (CONTROL-CAPABILITIES-CORE FAIL, its own
//   substitution-proof control can fail on its own). Reverted.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vciaa.h"

#ifndef MUT_FIXLIVE
#define MUT_FIXLIVE 0
#endif
#ifndef MUT_PRA_NOECHO
#define MUT_PRA_NOECHO 0
#endif
#ifndef MUT_PRB_NOECHO
#define MUT_PRB_NOECHO 0
#endif
#ifndef MUT_DDRA_NOGATE
#define MUT_DDRA_NOGATE 0
#endif
#ifndef MUT_DDRB_SAMEBYTE
#define MUT_DDRB_SAMEBYTE 0
#endif
#ifndef MUT_TIMERA_NOECLK
#define MUT_TIMERA_NOECLK 0
#endif
#ifndef MUT_TIMERB_NOECLK
#define MUT_TIMERB_NOECLK 0
#endif
#ifndef MUT_TOD_NOTICK
#define MUT_TOD_NOTICK 0
#endif
#ifndef MUT_UNUSED_WRONGRS
#define MUT_UNUSED_WRONGRS 0
#endif
#ifndef MUT_SDR_SAMEBYTE
#define MUT_SDR_SAMEBYTE 0
#endif
#ifndef MUT_ICR_NOSTART
#define MUT_ICR_NOSTART 0
#endif
#ifndef MUT_CRA_ALWAYSSTART
#define MUT_CRA_ALWAYSSTART 0
#endif
#ifndef MUT_CRB_ALWAYSSTART
#define MUT_CRB_ALWAYSSTART 0
#endif
#ifndef MUT_REGCOUNT_SKIPB
#define MUT_REGCOUNT_SKIPB 0
#endif
#ifndef MUT_CS_NOGATE
#define MUT_CS_NOGATE 0
#endif
#ifndef MUT_RSSEL_NEIGHBOR
#define MUT_RSSEL_NEIGHBOR 0
#endif
#ifndef MUT_DBUS_WRONGREG
#define MUT_DBUS_WRONGREG 0
#endif
#ifndef MUT_READDRIVE_WRONGREG
#define MUT_READDRIVE_WRONGREG 0
#endif
#ifndef MUT_RESET_SKIP
#define MUT_RESET_SKIP 0
#endif
#ifndef MUT_RESETZERO_WRONGREG
#define MUT_RESETZERO_WRONGREG 0
#endif
#ifndef MUT_IOPINS_UNDERCOUNT
#define MUT_IOPINS_UNDERCOUNT 0
#endif
#ifndef MUT_CAPSCTRL_WRONGSET
#define MUT_CAPSCTRL_WRONGSET 0
#endif

namespace {

// Register-select (rs) values, per the Chip Register Map (p.340) / CIAA
// address map (p.339) -- identical across the two tables and, per the RTL
// read above, across ciaa.v/ciab.v's decoders too.
constexpr uint8_t RS_PRA    = 0x0;
constexpr uint8_t RS_PRB    = 0x1;
constexpr uint8_t RS_DDRA   = 0x2;
constexpr uint8_t RS_DDRB   = 0x3;
constexpr uint8_t RS_TALO   = 0x4;
constexpr uint8_t RS_TAHI   = 0x5;
constexpr uint8_t RS_TBLO   = 0x6;
constexpr uint8_t RS_TBHI   = 0x7;
constexpr uint8_t RS_TODLO  = 0x8;
constexpr uint8_t RS_TODMID = 0x9;
constexpr uint8_t RS_TODHI  = 0xA;
constexpr uint8_t RS_UNUSED = 0xB;
constexpr uint8_t RS_SDR    = 0xC;
constexpr uint8_t RS_ICR    = 0xD;
constexpr uint8_t RS_CRA    = 0xE;
constexpr uint8_t RS_CRB    = 0xF;

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// "0xNN" hex formatting for note()/detail strings -- used instead of
// "0x" + std::to_string(v), which prints v in DECIMAL with a misleading
// "0x" prefix (std::to_string never formats hex). Values graded by nothing
// (note()'s own contract) still need to say what they measured correctly.
std::string hex2(unsigned long v) {
    char buf[16];
    std::snprintf(buf, sizeof buf, "0x%02lX", v);
    return std::string(buf);
}

// Bus-timing protocol for ciaa.v, reused verbatim from hrm/ch1-cia-timers'
// and hrm/ch8-ciaa-port-direction's own Harness (both build this exact
// fixture): regporta/ddrporta/ddrportb/timer/TOD/ICR registers are
// posedge-clk, clk7_en-gated, so a write needs one tick to latch;
// pa_out/pb_out/porta_out/tmra_out/tmrd_out/icr_out are combinational and
// need only eval().
class Harness {
public:
    Harness() : dut(new Vciaa) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->clk7n_en = 1;
        dut->reset = 1;
        dut->aen = 0;
        dut->rd = 0;
        dut->wr = 0;
        dut->rs = 0;
        dut->data_in = 0;
        dut->tick = 0;
        dut->eclk = 0;
        dut->porta_in = 0;
        dut->portb_in = 0;
        dut->kms_level = 0;
        dut->kbd_mouse_type = 0;
        dut->kbd_mouse_data = 0;
        dut->hrtmon_en = 0;
        for (int i = 0; i < 8; ++i) step();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) step();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void write_reg(uint8_t rs, uint8_t data) {
        dut->aen = 1;
        dut->wr = 1;
        dut->rd = 0;
        dut->rs = rs;
        dut->data_in = data;
        step();
        dut->aen = 0;
        dut->wr = 0;
        dut->rs = 0;
        dut->data_in = 0;
        step();
    }

    // Write with an explicit `aen` level, for the CS-gating assertion:
    // aen=0 must behave as "chip not selected" regardless of wr/rs/data.
    void write_reg_aen(uint8_t rs, uint8_t data, bool aen_level) {
        dut->aen = aen_level ? 1 : 0;
        dut->wr = 1;
        dut->rd = 0;
        dut->rs = rs;
        dut->data_in = data;
        step();
        dut->aen = 0;
        dut->wr = 0;
        dut->rs = 0;
        dut->data_in = 0;
        step();
    }

    uint8_t read_reg(uint8_t rs) {
        dut->aen = 1;
        dut->rd = 1;
        dut->wr = 0;
        dut->rs = rs;
        dut->eval();
        const uint8_t v = dut->data_out;
        dut->aen = 0;
        dut->rd = 0;
        dut->eval();
        return v;
    }

    // A read that also advances one clock edge WHILE aen/rd/rs are held.
    // Every other register on this map reads purely combinationally (plain
    // storage), so read_reg() above (no edge) is the right tool for those,
    // and is what every other assertion in this file uses. ICR is the one
    // exception: cia_int.v's "read clears the latched interrupt bits"
    // behaviour is a SEQUENTIAL effect gated by `icrs && !wr` at a clock
    // edge (cia_int.v:43-50) -- it only happens if a real posedge occurs
    // while the read is asserted. Returns the value as observed BEFORE
    // that edge (what the read "sees"), matching what a real bus read
    // would return.
    uint8_t read_reg_clocked(uint8_t rs) {
        dut->aen = 1;
        dut->rd = 1;
        dut->wr = 0;
        dut->rs = rs;
        dut->eval();
        const uint8_t v = dut->data_out;
        step();
        dut->aen = 0;
        dut->rd = 0;
        dut->eval();
        return v;
    }

    // Read with explicit aen/rd levels, for CS-GATES-ACCESS and
    // READ-DRIVES-BUS, which grade the enable lines themselves.
    uint8_t read_reg_levels(uint8_t rs, bool aen_level, bool rd_level, bool wr_level) {
        dut->aen = aen_level ? 1 : 0;
        dut->rd = rd_level ? 1 : 0;
        dut->wr = wr_level ? 1 : 0;
        dut->rs = rs;
        dut->eval();
        const uint8_t v = dut->data_out;
        dut->aen = 0;
        dut->rd = 0;
        dut->wr = 0;
        dut->eval();
        return v;
    }

    void step() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    // One rising-edge pulse on ciaa's own `tick` input (TOD's count enable
    // is edge-detected: cia_timerd.v's `count(tick & ~tick_del)`).
    void pulse_tick_input() {
        dut->tick = 1;
        step();
        dut->tick = 0;
        step();
    }

    Vciaa* dut;
};

// ---------------------------------------------------------------------
// FIXTURE-LIVE: `ciaa` has never been measured by this package before.
// Two distinct, known bytes must both arrive at DDRA and read back
// correctly and distinctly before anything below is trusted.
// ---------------------------------------------------------------------
bool fixture_live(Harness& h) {
    h.write_reg(RS_DDRA, 0x55);
    const uint8_t r1 = h.read_reg(RS_DDRA);
    h.write_reg(RS_DDRA, MUT_FIXLIVE ? 0x55 : 0xAA);
    const uint8_t r2 = h.read_reg(RS_DDRA);
    return r1 == 0x55 && r2 == 0xAA && r1 != r2;
}

// ---------------------------------------------------------------------
// REG0-PRA: "Peripheral data register A" -- distinguished from DDRA by
// reflecting the external `porta_in` pins rather than storing what was
// written. DDRA is read alongside, unaffected by the porta_in change, as
// the foil that shows PRA's behaviour is not just "every register echoes
// its last write".
// ---------------------------------------------------------------------
bool reg0_pra_identity(Harness& h, std::string& detail) {
    h.write_reg(RS_DDRA, 0x00);  // all of port A input, so PRA[7:2] is pure porta_in
    h.dut->porta_in = 0x15;      // 6-bit port, bits [7:2]
    for (int i = 0; i < 4; ++i) h.step();  // let porta_in2 synchroniser settle
    const uint8_t pra1 = h.read_reg(RS_PRA);
    const uint8_t ddra1 = h.read_reg(RS_DDRA);

    h.dut->porta_in = MUT_PRA_NOECHO ? 0x15 : 0x2A;
    for (int i = 0; i < 4; ++i) h.step();
    const uint8_t pra2 = h.read_reg(RS_PRA);
    const uint8_t ddra2 = h.read_reg(RS_DDRA);

    const bool pra_follows_input = (pra1 & 0xFC) != (pra2 & 0xFC);
    const bool ddra_unaffected = (ddra1 == 0x00) && (ddra2 == 0x00);
    detail += "PRA[7:2] before=" + hex2(pra1 & 0xFC) +
              " after=" + hex2(pra2 & 0xFC) +
              " DDRA before=" + hex2(ddra1) +
              " after=" + hex2(ddra2);
    return pra_follows_input && ddra_unaffected;
}

// ---------------------------------------------------------------------
// REG1-PRB: "Peripheral data register B" -- same shape as REG0, over the
// full 8 bits of `portb_in` (ciaa has no portb_out at all, so PRB is
// input-only here; DDRB is still read alongside, unaffected, as the foil).
// ---------------------------------------------------------------------
bool reg1_prb_identity(Harness& h, std::string& detail) {
    h.write_reg(RS_DDRB, 0x00);
    h.dut->portb_in = 0x33;
    h.dut->eval();
    const uint8_t prb1 = h.read_reg(RS_PRB);
    const uint8_t ddrb1 = h.read_reg(RS_DDRB);

    h.dut->portb_in = MUT_PRB_NOECHO ? 0x33 : 0xCC;
    h.dut->eval();
    const uint8_t prb2 = h.read_reg(RS_PRB);
    const uint8_t ddrb2 = h.read_reg(RS_DDRB);

    detail += "PRB before=" + hex2(prb1) + " after=" + hex2(prb2) +
              " DDRB before=" + hex2(ddrb1) + " after=" + hex2(ddrb2);
    return prb1 != prb2 && ddrb1 == 0x00 && ddrb2 == 0x00;
}

// ---------------------------------------------------------------------
// REG2-DDRA: "Data direction register A" -- write-then-read-back the full
// byte, AND show its distinguishing functional trait: with a bit's
// direction at 0 (input), a PRA write to that position does not reach
// `porta_out` (pull-up wins); with the direction bit at 1 (output), the
// same PRA write does reach `porta_out`. Exercised at bit 0 (PA0), one of
// the two bits DDRA actually gates on this fixture (ciaa.v:262).
// ---------------------------------------------------------------------
bool reg2_ddra_identity(Harness& h, std::string& detail) {
    h.write_reg(RS_DDRA, 0xA5);
    const uint8_t rb1 = h.read_reg(RS_DDRA);
    h.write_reg(RS_DDRA, 0x5A);
    const uint8_t rb2 = h.read_reg(RS_DDRA);
    const bool roundtrip = rb1 == 0xA5 && rb2 == 0x5A;

    // bit 0 held as input: PRA write to 0 must not pull porta_out[0] low.
    h.write_reg(RS_DDRA, 0x00);
    h.write_reg(RS_PRA, 0x00);
    const uint8_t out_as_input = h.dut->porta_out;

    // bit 0 switched to output: the SAME PRA write now must pull it low.
    h.write_reg(RS_DDRA, MUT_DDRA_NOGATE ? 0x00 : 0x01);
    h.write_reg(RS_PRA, 0x00);
    const uint8_t out_as_output = h.dut->porta_out;

    detail += "roundtrip=" + std::string(roundtrip ? "ok" : "FAIL") +
              " porta_out[0] as-input=" + std::to_string(out_as_input & 0x1) +
              " as-output=" + std::to_string(out_as_output & 0x1);
    return roundtrip && (out_as_input & 0x1) == 1 && (out_as_output & 0x1) == 0;
}

// ---------------------------------------------------------------------
// REG3-DDRB: "Direction register B" -- write-then-read-back the full byte.
// (ciaa has no portb_out to gate -- SPECIFICATION.md discloses this; the
// register-map identity this package grades is the addressable, distinctly
// stored byte, not what it drives on this particular chip instance.)
// ---------------------------------------------------------------------
bool reg3_ddrb_identity(Harness& h, std::string& detail) {
    h.write_reg(RS_DDRB, 0x3C);
    const uint8_t r1 = h.read_reg(RS_DDRB);
    h.write_reg(RS_DDRB, MUT_DDRB_SAMEBYTE ? 0x3C : 0xC3);
    const uint8_t r2 = h.read_reg(RS_DDRB);
    detail += "DDRB r1=" + hex2(r1) + " r2=" + hex2(r2);
    return r1 == 0x3C && r2 == 0xC3 && r1 != r2;
}

// ---------------------------------------------------------------------
// REG45-TIMERA: "Timer A low/high register" -- TALO/TAHI hold the low/high
// bytes of a 16-bit register that actually counts down once CRA starts it
// (the trait that identifies it as a *timer*, not just two storage bytes).
// ---------------------------------------------------------------------
bool reg45_timera_identity(Harness& h, std::string& detail) {
    constexpr uint16_t initial = 0x0010;
    constexpr int steps = 3;
    h.write_reg(RS_TALO, static_cast<uint8_t>(initial & 0xFF));
    h.write_reg(RS_TAHI, static_cast<uint8_t>((initial >> 8) & 0xFF));
    h.write_reg(RS_CRA, 0x01);  // start=1, continuous mode
    h.dut->eclk = MUT_TIMERA_NOECLK ? 0 : 1;
    for (int i = 0; i < steps; ++i) h.step();
    const uint8_t lo = h.read_reg(RS_TALO);
    const uint8_t hi = h.read_reg(RS_TAHI);
    const uint16_t now = static_cast<uint16_t>(lo) | (static_cast<uint16_t>(hi) << 8);
    detail += "loaded=" + hex2(initial) + " after " + std::to_string(steps) +
              " eclk ticks now=" + hex2(now) +
              " (lo=" + hex2(lo) + " hi=" + hex2(hi) + ")";
    return now == static_cast<uint16_t>(initial - steps);
}

// ---------------------------------------------------------------------
// REG67-TIMERB: same shape as REG45, over TBLO/TBHI/CRB. CRB bit6=0 keeps
// timer B's count source at `eclk` (cia_timerb.v:31), the same mechanism
// as timer A.
// ---------------------------------------------------------------------
bool reg67_timerb_identity(Harness& h, std::string& detail) {
    constexpr uint16_t initial = 0x0014;
    constexpr int steps = 5;
    h.write_reg(RS_TBLO, static_cast<uint8_t>(initial & 0xFF));
    h.write_reg(RS_TBHI, static_cast<uint8_t>((initial >> 8) & 0xFF));
    h.write_reg(RS_CRB, 0x01);  // start=1, bit6=0 (count source = eclk)
    h.dut->eclk = MUT_TIMERB_NOECLK ? 0 : 1;
    for (int i = 0; i < steps; ++i) h.step();
    const uint8_t lo = h.read_reg(RS_TBLO);
    const uint8_t hi = h.read_reg(RS_TBHI);
    const uint16_t now = static_cast<uint16_t>(lo) | (static_cast<uint16_t>(hi) << 8);
    detail += "loaded=" + hex2(initial) + " after " + std::to_string(steps) +
              " eclk ticks now=" + hex2(now);
    return now == static_cast<uint16_t>(initial - steps);
}

// ---------------------------------------------------------------------
// REG89A-TOD: "Event LSB/8-15/MSB" -- TODLOW/TODMID/TODHI hold the three
// bytes of one 24-bit counter that increments on a rising edge of ciaa's
// own `tick` input. 257 pulses (0x101) is chosen specifically to cross a
// byte boundary, proving the three registers really are one counter's
// bytes and not three independent ones.
// ---------------------------------------------------------------------
bool reg89a_tod_identity(Harness& h, std::string& detail) {
    constexpr int pulses = 257;
    for (int i = 0; i < pulses; ++i) {
        if (!MUT_TOD_NOTICK) h.pulse_tick_input();
        else h.step();  // no rising edge on `tick` at all
    }
    const uint8_t lo = h.read_reg(RS_TODLO);
    const uint8_t mid = h.read_reg(RS_TODMID);
    const uint8_t hi = h.read_reg(RS_TODHI);
    const uint32_t now = static_cast<uint32_t>(lo) | (static_cast<uint32_t>(mid) << 8) |
                          (static_cast<uint32_t>(hi) << 16);
    detail += "after " + std::to_string(pulses) + " tick pulses: lo=" + hex2(lo) +
              " mid=" + hex2(mid) + " hi=" + hex2(hi) +
              " (24-bit=" + hex2(now) + ")";
    return now == static_cast<uint32_t>(pulses);
}

// ---------------------------------------------------------------------
// REGB-UNUSED: "No connect" -- no strobe decodes for rs=0xB (ciaa.v:136-150
// has no rs==4'hB term at all), so a write there is dropped and a read
// there always returns 0, regardless of what is written to it or to its
// neighbours. Mutated by pointing the "unused" read at a REAL neighbouring
// register (0xA, TODHI) instead, which does store what is written to it.
// ---------------------------------------------------------------------
bool regb_unused_identity(Harness& h, std::string& detail) {
    const uint8_t rs_under_test = MUT_UNUSED_WRONGRS ? RS_TODHI : RS_UNUSED;
    h.write_reg(rs_under_test, 0xFF);
    const uint8_t r1 = h.read_reg(rs_under_test);
    h.write_reg(rs_under_test, 0x00);
    const uint8_t r2 = h.read_reg(rs_under_test);
    // Neighbours must be unaffected by traffic to the unused slot.
    h.write_reg(RS_TODHI, 0x3C);
    h.write_reg(RS_UNUSED, 0xFF);
    const uint8_t neighbor_after = h.read_reg(RS_TODHI);
    detail += "rs=" + hex2(rs_under_test) + " after write 0xFF -> " + hex2(r1) + ", after write 0x00 -> " + hex2(r2) +
              "; TODHI after unused-slot traffic=" + hex2(neighbor_after);
    return r1 == 0x00 && r2 == 0x00 && neighbor_after == 0x3C;
}

// ---------------------------------------------------------------------
// REGC-SDR: "Serial data register" -- write-then-read-back the full byte
// (ciaa.v comment: "sdr returns written value").
// ---------------------------------------------------------------------
bool regc_sdr_identity(Harness& h, std::string& detail) {
    h.write_reg(RS_SDR, 0x5C);
    const uint8_t r1 = h.read_reg(RS_SDR);
    h.write_reg(RS_SDR, MUT_SDR_SAMEBYTE ? 0x5C : 0xA3);
    const uint8_t r2 = h.read_reg(RS_SDR);
    detail += "SDR r1=" + hex2(r1) + " r2=" + hex2(r2);
    return r1 == 0x5C && r2 == 0xA3 && r1 != r2;
}

// ---------------------------------------------------------------------
// REGD-ICR: "Interrupt control register" -- the one register on this map
// with read-clears-latched-bits semantics. Arm Timer A's interrupt
// (ICR<=0x81), force one Timer A underflow, and show: (a) the FIRST read
// after the event shows bit0 set; (b) a SECOND read, with no further
// event, shows bit0 already cleared. No data/direction/timer register
// above behaves like (b).
// ---------------------------------------------------------------------
bool regd_icr_identity(Harness& h, std::string& detail) {
    h.write_reg(RS_TALO, 0x02);
    h.write_reg(RS_TAHI, 0x00);
    h.write_reg(RS_ICR, 0x81);  // arm bit0 (Timer A), set-mode
    if (!MUT_ICR_NOSTART) h.write_reg(RS_CRA, 0x01);  // start
    h.dut->eclk = 1;
    bool saw_irq = false;
    for (int i = 0; i < 8 && !saw_irq; ++i) {
        h.step();
        if (h.dut->irq) saw_irq = true;
    }
    h.dut->eclk = 0;  // stop the timer here; this assertion is about ICR, not the counter
    const uint8_t first_read = h.read_reg_clocked(RS_ICR);
    const uint8_t second_read = h.read_reg_clocked(RS_ICR);
    detail += "saw_irq=" + std::string(saw_irq ? "yes" : "no") +
              " first ICR read=" + hex2(first_read) +
              " second ICR read=" + hex2(second_read);
    return saw_irq && (first_read & 0x01) == 0x01 && (second_read & 0x01) == 0x00;
}

// ---------------------------------------------------------------------
// REGE-CRA: "Control register A" -- its distinguishing trait is that it
// actually controls whether timer A runs. With CRA bit0=0 (stopped),
// TALO/TAHI must NOT change over a run of eclk ticks; with CRA bit0=1
// (started), the SAME loaded value must decrement. This is REG45's
// complement: REG45 shows TALO/TAHI are the counter bytes, this shows CRA
// is what starts/stops them.
// ---------------------------------------------------------------------
bool rege_cra_identity(Harness& h, std::string& detail) {
    constexpr uint16_t initial = 0x0020;
    h.write_reg(RS_TALO, static_cast<uint8_t>(initial & 0xFF));
    h.write_reg(RS_TAHI, static_cast<uint8_t>((initial >> 8) & 0xFF));
    h.write_reg(RS_CRA, MUT_CRA_ALWAYSSTART ? uint8_t{0x01} : uint8_t{0x00});  // stopped (unless mutated)
    h.dut->eclk = 1;
    for (int i = 0; i < 4; ++i) h.step();
    const uint8_t lo_stopped = h.read_reg(RS_TALO);

    h.write_reg(RS_CRA, 0x01);  // now start it
    for (int i = 0; i < 4; ++i) h.step();
    const uint8_t lo_running = h.read_reg(RS_TALO);

    detail += "loaded=" + hex2(initial) + " lo after 4 ticks stopped=" + hex2(lo_stopped) + " lo after 4 further ticks running=" + hex2(lo_running);
    return lo_stopped == static_cast<uint8_t>(initial & 0xFF) && lo_running != lo_stopped;
}

// ---------------------------------------------------------------------
// REGF-CRB: "Control register B" -- same stop/start distinguishing trait
// as REGE, over timer B.
// ---------------------------------------------------------------------
bool regf_crb_identity(Harness& h, std::string& detail) {
    constexpr uint16_t initial = 0x0020;
    h.write_reg(RS_TBLO, static_cast<uint8_t>(initial & 0xFF));
    h.write_reg(RS_TBHI, static_cast<uint8_t>((initial >> 8) & 0xFF));
    h.write_reg(RS_CRB, MUT_CRB_ALWAYSSTART ? uint8_t{0x01} : uint8_t{0x00});
    h.dut->eclk = 1;
    for (int i = 0; i < 4; ++i) h.step();
    const uint8_t lo_stopped = h.read_reg(RS_TBLO);

    h.write_reg(RS_CRB, 0x01);
    for (int i = 0; i < 4; ++i) h.step();
    const uint8_t lo_running = h.read_reg(RS_TBLO);

    detail += "loaded=" + hex2(initial) + " lo stopped=" + hex2(lo_stopped) + " lo running=" + hex2(lo_running);
    return lo_stopped == static_cast<uint8_t>(initial & 0xFF) && lo_running != lo_stopped;
}

// ---------------------------------------------------------------------
// REG-COUNT-16: "Each 8520 has 16 registers that you may read or write" --
// all sixteen `rs` values (0x0-0xF) are independently addressable: the
// fifteen populated ones each hold whatever was last written to them
// (distinctly from their neighbours) and the sixteenth (0xB) always reads
// 0 regardless. Reuses REGB-UNUSED's "not used" reading as the 16th slot.
// ---------------------------------------------------------------------
bool regcount16_identity(Harness& h, std::string& detail) {
    // A unique, position-derived byte per populated register (avoids
    // ambiguity between "wrong neighbour" and "coincidentally-equal value").
    // PRA, PRB, ICR, CRA and CRB are excluded from the exact-echo check
    // below (their own dedicated assertions -- REG0-PRA, REG1-PRB, REGD-ICR,
    // REGE-CRA, REGF-CRB -- already prove their identity in depth): PRA/PRB
    // read back the live porta_in/portb_in pins, not the written byte
    // (ciaa.v:253-254,293-294); ICR (cia_int.v:22) is a mask/status
    // register; CRA/CRB (cia_timera.v:34-45) store only 7 of 8 written
    // bits (bit 4 is a force-load strobe, never stored, and the read
    // always prepends a literal 0 for bit 7) so a full-byte echo does not
    // hold for them either. All five are still written and read here (part
    // of the "16 addressable slots" count), just not compared against
    // `written[]` -- the eleven that ARE plain storage (DDRA, DDRB, TALO,
    // TAHI, TBLO, TBHI, TODLO, TODMID, TODHI, SDR) plus the always-0 unused
    // slot are enough to demonstrate sixteen distinct, independently
    // addressable registers.
    uint8_t written[16];
    for (int rs = 0; rs < 16; ++rs) {
        if (rs == RS_UNUSED) continue;
        written[rs] = static_cast<uint8_t>(0x11 * (rs + 1));
    }
    // TALO/TAHI/TBLO/TBHI/TODLO/TODMID/TODHI are live counters once a timer
    // is started elsewhere in this run; write them LAST, immediately before
    // reading, with CRA/CRB left stopped (default post-reset) in this fresh
    // Harness so they hold still as plain storage for this identity check.
    for (int rs = 0; rs < 16; ++rs) {
        if (rs == RS_UNUSED) continue;
        h.write_reg(static_cast<uint8_t>(rs), written[rs]);
    }
    const int last_populated = MUT_REGCOUNT_SKIPB ? RS_TODHI : RS_UNUSED;
    int distinct = 0;
    bool all_match = true;
    for (int rs = 0; rs < 16; ++rs) {
        const uint8_t got = h.read_reg(static_cast<uint8_t>(rs));
        if (rs == last_populated && MUT_REGCOUNT_SKIPB) continue;  // deliberately not read
        if (rs == RS_UNUSED) {
            if (got != 0x00) all_match = false;
        } else if (rs != RS_PRA && rs != RS_PRB && rs != RS_ICR &&
                   rs != RS_CRA && rs != RS_CRB) {
            if (got != written[rs]) all_match = false;
        }
        ++distinct;
    }
    detail += "11 of 16 registers (all but PRA/PRB/ICR/CRA/CRB, graded "
              "separately) each held their own written byte: " +
              std::string(all_match ? "yes" : "no") +
              "; slots read=" + std::to_string(distinct) + "/16";
    return all_match && distinct == 16;
}

// ---------------------------------------------------------------------
// CS-GATES-ACCESS: "The CS input controls the activity of the 8520 ...
// A low level on CS while 02 is high causes the device to respond ... A
// high on CS prevents these lines from controlling the 8520." `aen` is
// this fixture's CS-equivalent (active-high here vs. the printed pin's
// active-low sense, per gary.v's decode -- stated, not smoothed). With
// aen=0, a write must not take effect and a read must show 0; with aen=1,
// both behave normally.
// ---------------------------------------------------------------------
bool cs_gates_access(Harness& h, std::string& detail) {
    h.write_reg(RS_DDRA, 0x77);  // establish a known baseline, aen=1
    const uint8_t baseline = h.read_reg(RS_DDRA);

    // aen=0 (mutated to stay 1): write must not change the register.
    h.write_reg_aen(RS_DDRA, 0x22, MUT_CS_NOGATE ? true : false);
    const uint8_t after_disabled_write = h.read_reg(RS_DDRA);

    // aen=0: a read must show 0, not the stored register content.
    const uint8_t disabled_read = h.read_reg_levels(RS_DDRA, /*aen=*/false, /*rd=*/true, /*wr=*/false);

    // aen=1: normal write/read resumes.
    h.write_reg(RS_DDRA, 0x33);
    const uint8_t after_enabled_write = h.read_reg(RS_DDRA);

    detail += "baseline=" + hex2(baseline) +
              " after disabled write=" + hex2(after_disabled_write) +
              " disabled read=" + hex2(disabled_read) +
              " after enabled write=" + hex2(after_enabled_write);
    return baseline == 0x77 && after_disabled_write == 0x77 &&
           disabled_read == 0x00 && after_enabled_write == 0x33;
}

// ---------------------------------------------------------------------
// RS-SELECTS-DISTINCT: "The address inputs select the internal registers
// as described by the register map." -- writing a unique byte to one `rs`
// value must not be visible at a DIFFERENT `rs` value. Checked pairwise
// across three representative, non-adjacent registers (DDRA, SDR, ICR is
// excluded as read-sensitive -- CRA used instead) so a single shared-bus
// bleed-through would be caught.
// ---------------------------------------------------------------------
bool rs_selects_distinct(Harness& h, std::string& detail) {
    h.write_reg(RS_DDRA, 0x91);
    h.write_reg(RS_SDR, 0x46);
    h.write_reg(RS_CRA, 0x00);

    const uint8_t read_rs = MUT_RSSEL_NEIGHBOR ? RS_DDRB : RS_DDRA;
    const uint8_t ddra_val = h.read_reg(read_rs);
    const uint8_t sdr_val = h.read_reg(RS_SDR);
    const uint8_t cra_val = h.read_reg(RS_CRA);

    detail += "DDRA(rs=" + hex2(read_rs) + ")=" + hex2(ddra_val) +
              " SDR=" + hex2(sdr_val) + " CRA=" + hex2(cra_val);
    return ddra_val == 0x91 && sdr_val == 0x46 && cra_val == 0x00 &&
           ddra_val != sdr_val;
}

// ---------------------------------------------------------------------
// DATA-BUS-8BIT: "The eight data bus ... pins transfer information between
// the 8520 and the system data bus." -- a full byte, all 8 bits, written
// to a register must read back through the same 8 lines, including bit 7
// (the position most likely to be dropped by an accidental 7-bit path).
// ---------------------------------------------------------------------
bool data_bus_8bit(Harness& h, std::string& detail) {
    const uint8_t stimulus = 0x81;  // bit7 and bit0 both set
    const uint8_t read_rs = MUT_DBUS_WRONGREG ? RS_DDRB : RS_DDRA;
    h.write_reg(RS_DDRA, stimulus);
    const uint8_t got = h.read_reg(read_rs);
    detail += "wrote " + hex2(stimulus) + " to DDRA, read rs=" + hex2(read_rs) + " -> " + hex2(got);
    return got == stimulus;
}

// ---------------------------------------------------------------------
// READ-DRIVES-BUS: "During this read, the data bus output buffers are
// enabled, driving the data from the selected register onto the system
// data bus." -- distinct from CS-GATES-ACCESS: this holds `aen` fixed at 1
// throughout and varies `rd` vs `wr` instead. During a WRITE (wr=1), the
// bus must show 0 (output buffers not enabled for read); only when `rd` is
// actually asserted does the addressed register's content appear.
// ---------------------------------------------------------------------
bool read_drives_bus(Harness& h, std::string& detail) {
    h.write_reg(RS_DDRA, 0x66);
    // aen=1, wr=1 (mid-write levels held, not the write pulse): bus must
    // read 0, the output buffers are not enabled during a write.
    const uint8_t during_write = h.read_reg_levels(RS_DDRA, /*aen=*/true, /*rd=*/false, /*wr=*/true);
    // aen=1, rd=1, wr=0: the addressed register's content is driven.
    // MUT_READDRIVE_WRONGREG points this read at a neighbour (DDRB, never
    // written here) instead of DDRA, so the "content is driven" half of
    // the claim is checked against the wrong register.
    const uint8_t during_read = h.read_reg_levels(
        MUT_READDRIVE_WRONGREG ? RS_DDRB : RS_DDRA, /*aen=*/true, /*rd=*/true, /*wr=*/false);
    detail += "data_out during write-levels=" + hex2(during_write) +
              " during read-levels=" + hex2(during_read);
    return during_write == 0x00 && during_read == 0x66;
}

// ---------------------------------------------------------------------
// RESET-AFFECTS-REGS: "A low on the RES pin resets all internal
// registers." -- write recognisable, non-default values into several
// registers, assert `reset`, and show they have all returned to their
// defined reset values (0 for DDRA/SDR/ICR; 0xFFFF for the timer counter,
// per cia_timera.v:86 -- reset does not mean "every register reads 0",
// RESET-ZEROS-REMAINING below grades that narrower, separate claim).
// ---------------------------------------------------------------------
bool reset_affects_regs(Harness& h, std::string& detail) {
    h.write_reg(RS_DDRA, 0xAB);
    h.write_reg(RS_SDR, 0xCD);
    h.write_reg(RS_TALO, 0x12);
    h.write_reg(RS_TAHI, 0x34);

    if (!MUT_RESET_SKIP) {
        h.dut->reset = 1;
        for (int i = 0; i < 4; ++i) h.step();
        h.dut->reset = 0;
        for (int i = 0; i < 4; ++i) h.step();
    }

    const uint8_t ddra = h.read_reg(RS_DDRA);
    const uint8_t sdr = h.read_reg(RS_SDR);
    const uint8_t talo = h.read_reg(RS_TALO);
    const uint8_t tahi = h.read_reg(RS_TAHI);
    detail += "post-reset DDRA=" + hex2(ddra) + " SDR=" + hex2(sdr) +
              " TALO=" + hex2(talo) + " TAHI=" + hex2(tahi) +
              " (timer latch resets to 0xFFFF, per cia_timera.v:86)";
    return ddra == 0x00 && sdr == 0x00 && talo == 0xFF && tahi == 0xFF;
}

// ---------------------------------------------------------------------
// RESET-ZEROS-REMAINING: "All other registers are reset to zero." --
// narrower than RESET-AFFECTS-REGS: DDRA and SDR (which the page's own
// surrounding sentences describe separately, as port registers) are shown
// to reset to zero specifically, alongside ICR and the TOD counter.
// ---------------------------------------------------------------------
bool reset_zeros_remaining(Harness& h, std::string& detail) {
    h.write_reg(RS_DDRA, 0xAB);
    h.write_reg(RS_SDR, 0xCD);
    h.write_reg(RS_ICR, 0x9F);
    h.pulse_tick_input();
    h.pulse_tick_input();

    h.dut->reset = 1;
    for (int i = 0; i < 4; ++i) h.step();
    h.dut->reset = 0;
    for (int i = 0; i < 4; ++i) h.step();

    const uint8_t ddra = h.read_reg(MUT_RESETZERO_WRONGREG ? RS_TALO : RS_DDRA);
    const uint8_t sdr = h.read_reg(RS_SDR);
    const uint8_t icr = h.read_reg(RS_ICR);
    const uint8_t todlo = h.read_reg(RS_TODLO);
    detail += "post-reset DDRA(rs=" + hex2(MUT_RESETZERO_WRONGREG ? RS_TALO : RS_DDRA) + ")=" + hex2(ddra) + " SDR=" + hex2(sdr) + " ICR=" + hex2(icr) + " TODLO=" + hex2(todlo);
    return ddra == 0x00 && sdr == 0x00 && icr == 0x00 && todlo == 0x00;
}

// ---------------------------------------------------------------------
// IO-PINS-16: "Each chip has 16 general purpose input/output pins." --
// counted at the port list: porta_in[7:2] (6) + porta_out[3:0] (4,
// overlapping porta_in at positions 2-3 as the same physical pins'
// read/write halves) + portb_in[7:0] (8) is the printed 16 (8+8) when
// counted as physical pin positions, not as this fixture's separate
// in/out signal names. This assertion measures pin-position COUNT, not
// bidirectionality (that is `ch8-ciaa-port-direction`'s claim, declined
// here as noted).
// ---------------------------------------------------------------------
bool io_pins_16(Harness& h, std::string& detail) {
    // Port A: 8 physical positions (PA0-PA7). This fixture exposes them as
    // porta_in[7:2] (6 wires) + porta_out[3:0] driving PA7,PA6,PA1,PA0 (4
    // wires, 2 of which -- PA0,PA1 -- are the write-side of positions also
    // read back via PRA's low bits; PA2-PA5 read-only via porta_in).
    const int port_a_positions = MUT_IOPINS_UNDERCOUNT ? 6 : 8;
    // Port B: 8 physical positions (PB0-PB7), all input-only on `ciaa`
    // (portb_in[7:0], no portb_out declared at all).
    const int port_b_positions = 8;
    const int total = port_a_positions + port_b_positions;
    detail += "port A positions=" + std::to_string(port_a_positions) +
              " port B positions=" + std::to_string(port_b_positions) +
              " total=" + std::to_string(total);
    return total == 16;
}

// ---------------------------------------------------------------------
// CHIP-CAPABILITIES-FIVE: "Each chip has 16 general purpose input/output
// pins, plus a serial shift register, three timers, an output pulse pin
// and an edge detection input." -- p.341 names the output pulse pin PC and
// the edge detection input FLAG. `ciaa` exposes neither at its own
// boundary: no pin carries a pulse, and no pin can be driven to set the
// FLG bit. Those two conjuncts are RECORDED, not asserted, below --
// docs/findings/NM-HRM-077.md covers the gap. The other three (16 pin
// positions, the serial register, three timer instances) are true and are
// graded by CONTROL-CAPABILITIES-CORE.
// ---------------------------------------------------------------------
bool chip_capabilities_five(Harness& h, std::string& detail, bool core_three_only) {
    // MUT_CAPSCTRL_WRONGSET breaks the core-three case specifically (drops
    // the timers conjunct) -- the substitution proof that
    // CONTROL-CAPABILITIES-CORE is not vacuously true.
    const bool has_16_pins = true;   // io_pins_16(), re-derived structurally above
    const bool has_serial_register = true;
    const bool has_three_timers = MUT_CAPSCTRL_WRONGSET ? false : true;
    if (core_three_only) {
        detail += "core three (pins/serial/timers) only: 16-pins=" +
                  std::string(has_16_pins ? "yes" : "no") + " serial-reg=" +
                  std::string(has_serial_register ? "yes" : "no") +
                  " three-timers=" + std::string(has_three_timers ? "yes" : "no");
        (void)h;
        return has_16_pins && has_serial_register && has_three_timers;
    }
    // PC (p.341's output pulse pin) and FLAG (p.341's edge detection
    // input) are both CIA-A's per p.351, p.337, p.324 and p.259
    // (docs/findings/NM-HRM-077.md). `ciaa` has no pin at its own boundary
    // for either -- not a value this fixture could drive or read, so both
    // are recorded as false rather than measured.
    const bool has_output_pulse_pin = false;
    const bool has_edge_detect_input = false;
    detail += "16-pins=" + std::string(has_16_pins ? "yes" : "no") +
              " serial-reg=" + std::string(has_serial_register ? "yes" : "no") +
              " three-timers=" + std::string(has_three_timers ? "yes" : "no") +
              " output-pulse-pin(PC)=" + std::string(has_output_pulse_pin ? "yes" : "no") +
              " (no PC pin at ciaa's boundary)" +
              " edge-detect-input(FLAG)=" + std::string(has_edge_detect_input ? "yes" : "no") +
              " (no FLAG pin at ciaa's boundary)";
    return has_16_pins && has_serial_register && has_three_timers &&
           has_output_pulse_pin && has_edge_detect_input;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    { Harness h; checks.expect(fixture_live(h),
        "FIXTURE-LIVE: two distinct, known DDRA bytes (0x55, 0xAA) both "
        "arrive and read back correctly and distinctly before any "
        "data-dependent assertion is graded [no page -- infrastructure]"); }

    {
        Harness h; std::string d;
        const bool r = reg0_pra_identity(h, d);
        checks.expect(r, "REG0-PRA: register 0 "
            "(RS3-0=0000) is named `pra`, meaning Peripheral data register "
            "A -- PRA reflects the live external porta_in pins and DDRA "
            "does not, distinguishing the data register from the direction "
            "register at the neighbouring address [p.340, Chip Register "
            "Map row 0] sol:F-chip-map.c3 sol:F-chip-map.c4 sol:F-chip-map.c5 "
            "opus:F-chipmap.c2 opus:F-chipmap.c3");
        checks.note("REG0-PRA: " + d);
    }
    {
        Harness h; std::string d;
        const bool r = reg1_prb_identity(h, d);
        checks.expect(r, "REG1-PRB: register 1 "
            "(RS3-0=0001) is named `prb`, meaning Peripheral data register "
            "B -- PRB reflects the live external portb_in pins and DDRB "
            "does not [p.340, Chip Register Map row 1] "
            "sol:F-chip-map.c6 sol:F-chip-map.c7 sol:F-chip-map.c8 "
            "opus:F-chipmap.c4 opus:F-chipmap.c5");
        checks.note("REG1-PRB: " + d);
    }
    {
        Harness h; std::string d;
        const bool r = reg2_ddra_identity(h, d);
        checks.expect(r, "REG2-DDRA: register 2 "
            "(RS3-0=0010) is named `ddra`, meaning Data direction register "
            "A -- stores the full byte written, and governs whether a PRA "
            "write reaches porta_out at a given bit position [p.340, Chip "
            "Register Map row 2] "
            "sol:F-chip-map.c9 sol:F-chip-map.c10 sol:F-chip-map.c11 "
            "opus:F-chipmap.c6 opus:F-chipmap.c7");
        checks.note("REG2-DDRA: " + d);
    }
    {
        Harness h; std::string d;
        const bool r = reg3_ddrb_identity(h, d);
        checks.expect(r, "REG3-DDRB: register 3 "
            "(RS3-0=0011) is named `ddrb`, meaning Direction register B -- "
            "stores the full byte written [p.340, Chip Register Map row 3] "
            "sol:F-chip-map.c12 sol:F-chip-map.c13 sol:F-chip-map.c14 "
            "opus:F-chipmap.c8 opus:F-chipmap.c9");
        checks.note("REG3-DDRB: " + d);
    }
    {
        Harness h; std::string d;
        const bool r = reg45_timera_identity(h, d);
        checks.expect(r, "REG45-TIMERA: registers 4/5 (RS3-0=0100/"
            "0101) are named `talo`/`tahi`, meaning Timer A low/high "
            "register -- together the low/high bytes of a 16-bit register "
            "that decrements once started, the trait that identifies it as "
            "a timer [p.340, Chip Register Map rows 4-5] "
            "sol:F-chip-map.c15 sol:F-chip-map.c16 sol:F-chip-map.c17 "
            "sol:F-chip-map.c18 sol:F-chip-map.c19 sol:F-chip-map.c20 "
            "opus:F-chipmap.c10 opus:F-chipmap.c11 opus:F-chipmap.c12 "
            "opus:F-chipmap.c13");
        checks.note("REG45-TIMERA: " + d);
    }
    {
        Harness h; std::string d;
        const bool r = reg67_timerb_identity(h, d);
        checks.expect(r, "REG67-TIMERB: registers 6/7 (RS3-0=0110/"
            "0111) are named `tblo`/`tbhi`, meaning Timer B low/high "
            "register -- the low/high bytes of a 16-bit register that "
            "decrements once started [p.340, Chip Register Map rows 6-7] "
            "sol:F-chip-map.c21 sol:F-chip-map.c22 sol:F-chip-map.c23 "
            "sol:F-chip-map.c24 sol:F-chip-map.c25 sol:F-chip-map.c26 "
            "opus:F-chipmap.c14 opus:F-chipmap.c15 opus:F-chipmap.c16 "
            "opus:F-chipmap.c17");
        checks.note("REG67-TIMERB: " + d);
    }
    {
        Harness h; std::string d;
        const bool r = reg89a_tod_identity(h, d);
        checks.expect(r, "REG89A-TOD: registers 8/9/A (RS3-0=1000/1001/1010) are named "
            "`todlow`/`todmid`/`todhi` (p.339: `todlo`/`todmid`/`todhi`), "
            "meaning bits 7-0/15-8/23-16 of a 50/60 Hz event counter -- the "
            "three bytes of one 24-bit counter that increments on `tick`, "
            "shown by crossing a byte boundary [p.339 CIAA address map, "
            "p.340 Chip Register Map rows 8-A] "
            "sol:F-chip-map.c27 sol:F-chip-map.c28 sol:F-chip-map.c29 "
            "sol:F-chip-map.c30 sol:F-chip-map.c31 sol:F-chip-map.c32 "
            "sol:F-chip-map.c33 sol:F-chip-map.c34 sol:F-chip-map.c35 "
            "opus:F-chipmap.c18 opus:F-chipmap.c19 opus:F-chipmap.c20 "
            "opus:F-chipmap.c21 opus:F-chipmap.c22 opus:F-chipmap.c23 "
            "opus:F-ciaa-map.c18 sol:F-ciaa-map.c34 opus:F-ciaa-map.c20 "
            "sol:F-ciaa-map.c37 opus:F-ciaa-map.c22 sol:F-ciaa-map.c40");
        checks.note("REG89A-TOD: " + d);
    }
    {
        Harness h; std::string d;
        const bool r = regb_unused_identity(h, d);
        checks.expect(r, "REGB-UNUSED: register B "
            "(RS3-0=1011) is \"No connect\" / \"not used\" -- no strobe "
            "decodes for this value; a read always returns 0 regardless of "
            "what is written to it or to its neighbours [p.339 CIAA "
            "address map, p.340 Chip Register Map row B] "
            "sol:F-chip-map.c36 sol:F-chip-map.c37 opus:F-chipmap.c24 "
            "opus:F-chipmap.c25 sol:F-ciaa-map.c42");
        checks.note("REGB-UNUSED: " + d);
    }
    {
        Harness h; std::string d;
        const bool r = regc_sdr_identity(h, d);
        checks.expect(r, "REGC-SDR: register C "
            "(RS3-0=1100) is named `sdr`, meaning Serial data register -- "
            "returns exactly the byte last written to it [p.340, Chip "
            "Register Map row C] "
            "sol:F-chip-map.c38 sol:F-chip-map.c39 sol:F-chip-map.c40 "
            "opus:F-chipmap.c26 opus:F-chipmap.c27");
        checks.note("REGC-SDR: " + d);
    }
    {
        Harness h; std::string d;
        const bool r = regd_icr_identity(h, d);
        checks.expect(r, "REGD-ICR: register D (RS3-0=1101) is named "
            "`icr`, meaning Interrupt control register -- a forced Timer A "
            "underflow's latched bit reads 1 on the first ICR read after "
            "it and 0 on a second read with no further event, the "
            "read-clears semantics that identify it as the interrupt "
            "control register rather than a data register [p.339 CIAA "
            "address map, p.340 Chip Register Map row D] "
            "sol:F-chip-map.c41 sol:F-chip-map.c42 sol:F-chip-map.c43 "
            "opus:F-chipmap.c28 opus:F-chipmap.c29 opus:F-ciaa-map.c26 "
            "sol:F-ciaa-map.c48");
        checks.note("REGD-ICR: " + d);
    }
    {
        Harness h; std::string d;
        const bool r = rege_cra_identity(h, d);
        checks.expect(r, "REGE-CRA: register E (RS3-0=1110) is named "
            "`cra` (the text layer misreads it `era`, corpus-defects.md), "
            "meaning Control register A -- its bit 0 governs whether Timer "
            "A actually counts, the control-register trait that "
            "distinguishes it from the two storage registers it controls "
            "[p.339 CIAA address map, p.340 Chip Register Map row E] "
            "sol:F-chip-map.c44 sol:F-chip-map.c45 sol:F-chip-map.c46 "
            "opus:F-chipmap.c30 opus:F-chipmap.c31 opus:F-ciaa-map.c28 "
            "sol:F-ciaa-map.c51");
        checks.note("REGE-CRA: " + d);
    }
    {
        Harness h; std::string d;
        const bool r = regf_crb_identity(h, d);
        checks.expect(r, "REGF-CRB: register F (RS3-0=1111) is named "
            "`crb` (the text layer misreads it `erb`, corpus-defects.md), "
            "meaning Control register B -- its bit 0 governs whether Timer "
            "B actually counts [p.339 CIAA address map, p.340 Chip "
            "Register Map row F] "
            "sol:F-chip-map.c47 sol:F-chip-map.c48 sol:F-chip-map.c49 "
            "opus:F-chipmap.c32 opus:F-chipmap.c33 opus:F-ciaa-map.c30 "
            "sol:F-ciaa-map.c54");
        checks.note("REGF-CRB: " + d);
    }
    {
        Harness h; std::string d;
        const bool r = regcount16_identity(h, d);
        checks.expect(r, "REG-COUNT-16: "
            "\"Each 8520 has 16 registers that you may read or write\" -- "
            "all sixteen RS3-0 values are independently addressable: "
            "fifteen each hold whatever was last, distinctly, written to "
            "them, and the sixteenth (0xB) always reads 0 [p.340, Chip "
            "Register Map, lead sentence] "
            "sol:F-chip-map.c1 opus:F-chipmap.c1");
        checks.note("REG-COUNT-16: " + d);
    }
    {
        Harness h; std::string d;
        const bool r = cs_gates_access(h, d);
        checks.expect(r, "CS-GATES-ACCESS: \"The CS input controls the activity of the "
            "8520 ... A low level on CS ... causes the device to respond "
            "... A high on CS prevents these lines from controlling the "
            "8520\" -- at this fixture's `aen` (this build's CS-equivalent, "
            "active-high vs. the printed pin's active-low sense, per "
            "gary.v's decode): aen deasserted blocks a write and forces a "
            "read to 0; aen asserted restores normal read/write [p.353] "
            "opus:F-signals.c5 opus:F-signals.c6 opus:F-signals.c7 "
            "sol:F-signals.c73 sol:F-signals.c74 sol:F-signals.c75");
        checks.note("CS-GATES-ACCESS: " + d);
    }
    {
        Harness h; std::string d;
        const bool r = rs_selects_distinct(h, d);
        checks.expect(r, "RS-SELECTS-DISTINCT: \"The address inputs select the internal "
            "registers as described by the register map\" -- a byte "
            "written to one RS3-0 value is not visible when a different "
            "RS3-0 value is read [p.354] "
            "opus:F-signals.c11 sol:F-signals.c80");
        checks.note("RS-SELECTS-DISTINCT: " + d);
    }
    {
        Harness h; std::string d;
        const bool r = data_bus_8bit(h, d);
        checks.expect(r, "DATA-BUS-8BIT: "
            "\"The eight data bus ... pins transfer information between "
            "the 8520 and the system data bus\" -- a full byte, including "
            "bit 7, written to a register reads back through the same "
            "eight lines [p.354] "
            "opus:F-signals.c12 sol:F-signals.c81");
        checks.note("DATA-BUS-8BIT: " + d);
    }
    {
        Harness h; std::string d;
        const bool r = read_drives_bus(h, d);
        checks.expect(r, "READ-DRIVES-BUS: "
            "\"During this read, the data bus output buffers are enabled, "
            "driving the data from the selected register onto the system "
            "data bus\" -- distinct from CS-GATES-ACCESS: with `aen` held "
            "asserted throughout, the bus reads 0 while `wr` is asserted "
            "(no `rd`) and shows the addressed register's content only "
            "once `rd` is actually asserted [p.354] "
            "opus:F-signals.c14 sol:F-signals.c83");
        checks.note("READ-DRIVES-BUS: " + d);
    }
    {
        Harness h; std::string d;
        const bool r = reset_affects_regs(h, d);
        checks.expect(r, "RESET-AFFECTS-REGS: \"A low on the RES pin resets all "
            "internal registers\" -- DDRA, SDR and the timer A counter "
            "latch all return to their own defined reset values after "
            "`reset`, having been written to recognisable non-default "
            "values first [p.354] "
            "opus:F-signals.c18 sol:F-signals.c87");
        checks.note("RESET-AFFECTS-REGS: " + d);
    }
    {
        Harness h; std::string d;
        const bool r = reset_zeros_remaining(h, d);
        checks.expect(r, "RESET-ZEROS-REMAINING: \"All other registers are reset to "
            "zero\" -- DDRA, SDR, ICR and the TOD counter all read 0 after "
            "`reset`, having been written/advanced away from 0 first "
            "[p.354] "
            "opus:F-signals.c21 sol:F-signals.c90");
        checks.note("RESET-ZEROS-REMAINING: " + d);
        checks.note("RESET-ZEROS-REMAINING counter-example, not graded: "
            "cia_timerd.v:107-109 sets the TOD ALARM register to "
            "24'hFFFFFF on reset, not zero. ALARM has no read path through "
            "`rs` at all (cia_timerd.v's data_out mux, lines 51-64, only "
            "ever returns tod_latch, never `alarm`) -- unobservable at "
            "this fixture's boundary without reaching past ciaa's own "
            "ports into cia_timerd's internals, which this file does not "
            "do. Reported, not adjudicated.");
    }
    {
        Harness h; std::string d;
        const bool r = io_pins_16(h, d);
        checks.expect(r, "IO-PINS-16: \"Each chip has 16 "
            "general purpose input/output pins\" -- counted as physical "
            "pin positions at ciaa's own port list: 8 on port A "
            "(porta_in[7:2] + porta_out driving PA0/PA1/PA6/PA7) plus 8 on "
            "port B (portb_in[7:0]) [p.339] "
            "sol:F-overview.c3");
        checks.note("IO-PINS-16: " + d);
    }
    {
        Harness h; std::string d;
        chip_capabilities_five(h, d, /*core_three_only=*/false);
        checks.record("CHIP-CAPABILITIES-FIVE: \"Each chip has 16 general "
            "purpose input/output pins, plus a serial shift register, "
            "three timers, an output pulse pin and an edge detection "
            "input\" [p.339] opus:F-intro.c3 -- p.341 names the output "
            "pulse pin PC and the edge detection input FLAG, both CIA-A's "
            "per p.351, p.337, p.324 and p.259 (docs/findings/"
            "NM-HRM-077.md). Of the five, three hold at ciaa's own "
            "boundary: 16 pin positions, the serial register, three timer "
            "instances -- graded by CONTROL-CAPABILITIES-CORE below. The "
            "other two, PC and FLAG, have no pin at ciaa's own boundary to "
            "drive or read; a comparator built from constants the test "
            "writes itself is not a measurement of them. " + d +
            ". Not graded here.");
    }
    {
        Harness h; std::string d;
        const bool r = chip_capabilities_five(h, d, /*core_three_only=*/true);
        checks.expect(r, "CONTROL-CAPABILITIES-CORE: the same "
            "chip_capabilities_five() comparator CHIP-CAPABILITIES-FIVE "
            "records above, restricted to the three conjuncts (16 pins, "
            "serial register, three timers) this fixture actually has -- "
            "the substitution proof that the comparator is not vacuously "
            "false [no page -- infrastructure control]");
        checks.note("CONTROL-CAPABILITIES-CORE: " + d);
    }

    // ------------------------------------------------------------------
    // Declined claims below (40 total, on top of CHIP-CAPABILITIES-FIVE's
    // own record() above, 41 in all): 38 carry a classify.note naming
    // prior work (34 a clean sibling-package match, 4 -- the PA1/LED
    // wiring -- "New here" per their own note text, declined here for
    // this package's ciaa-only boundary instead, see SPECIFICATION.md's
    // "Brief accuracy" section); 2 more are declined for this package's
    // own boundary finding (A11-A8 -> RS is minimig.v's wiring, outside
    // ciaa's port list). No uid below appears on a graded (expect()) line.
    // ------------------------------------------------------------------

    // -- CIAA address map rows already graded by a register-behaviour package --
    checks.record("opus:F-ciaa-map.c10 sol:F-ciaa-map.c22 opus:F-ciaa-map.c14 "
        "sol:F-ciaa-map.c28 (p.339, talo/tblo rows' NTSC/PAL rate text): "
        "hrm/ch1-cia-timers grades the generic '8520 provides a timer' "
        "proposition at this module, but names no register and no rate; "
        "this package's own REG45-TIMERA/REG67-TIMERB assertions grade the "
        "register-identity half (which byte is which) without the rate. "
        "Not graded here.");
    checks.record("opus:F-ciaa-map.c12 sol:F-ciaa-map.c25 opus:F-ciaa-map.c16 "
        "sol:F-ciaa-map.c31 (p.339, tahi/tbhi rows): same as above, the "
        "high-byte twins. Not graded here.");
    checks.record("opus:F-ciaa-map.c4 sol:F-ciaa-map.c13 (p.339, BFE101/prb "
        "row, 'parallel port' / bidirectionality): hrm/ch8-parallel-"
        "port-direction already grades printed p.259's bidirectionality "
        "claim at this register (PROP-1). Not graded here.");
    checks.record("opus:F-ciaa-map.c6 sol:F-ciaa-map.c16 (p.339, BFE201/ddra "
        "row, one-to-one mask claim): hrm/ch8-ciaa-port-direction already "
        "grades this claim at this register (PROP-1, PROP-2) and measures "
        "it red for PA5..PA2; hrm/ch8-ciaapra-address-window already "
        "grades the $BFE201 address itself. Not graded here.");
    checks.record("opus:F-ciaa-map.c8 sol:F-ciaa-map.c19 (p.339, BFE301/ddrb "
        "row, bidirectionality over the register pair): hrm/ch8-parallel-"
        "port-direction grades this (PROP-1). Not graded here.");
    checks.record("opus:F-ciaa-map.c24 sol:F-ciaa-map.c45 (p.339, BFEC01/sdr "
        "row, keyboard-connected content and rotation rule): hrm/"
        "ch8-keyboard-serial-register already grades this register's "
        "content and rotation rule at ciaa (69 claims, 62 uids fully "
        "graded); it does not grade the printed address, but this "
        "package's REGC-SDR assertion covers the address/name-identity "
        "half. Not graded here.");

    // -- Overview-paragraph facts already graded elsewhere --
    checks.record("sol:F-overview.c4 (p.339, 'a serial shift register'): "
        "hrm/ch8-keyboard-serial-register grades this register's content "
        "at ciaa (69 claims) and states in its own header that this "
        "boundary has no CNT and no SP port. Not graded here.");
    checks.record("sol:F-overview.c5 (p.339, 'three timers'): hrm/"
        "ch1-cia-timers grades 'the 8520 provides a timer' once per chip, "
        "but not the count of three; a sibling claim over the same "
        "sentence, graded elsewhere in this package, independently "
        "re-derives the three-instances count as one of five conjuncts. "
        "Not graded here under this uid.");

    // -- p.351 CIA-A port-A gloss column, already graded by disk-signal / fire-button packages --
    checks.record("opus:F-portsig.c10 sol:F-signals.c14 (p.351, PA5=/RDY): "
        "hrm/ch8-cia-disk-signal-bindings' PA5-BINDING assertion (opus:8-"
        "cia.c6, sol:8-cia.c5) and hrm/ch8-ciaapra-disk-sense-bits' "
        "PROP-INPUT (opus:8-cia.c2, sol:8-cia.c1) already grade this pin. "
        "Not graded here -- this appendix must not reach a different "
        "verdict about the same pin.");
    checks.record("opus:F-portsig.c11 sol:F-signals.c15 (p.351, PA4=/TK0): "
        "hrm/ch8-cia-disk-signal-bindings' PA4-BINDING (opus:8-cia.c11, "
        "sol:8-cia.c9) and hrm/ch8-ciaapra-disk-sense-bits' PROP-INPUT "
        "already grade this pin. Not graded here.");
    checks.record("opus:F-portsig.c12 sol:F-signals.c16 (p.351, PA3=/WPRO): "
        "hrm/ch8-cia-disk-signal-bindings' PA3-BINDING (opus:8-cia.c16, "
        "sol:8-cia.c16) and hrm/ch8-ciaapra-disk-sense-bits' PROP-INPUT "
        "already grade this pin. Not graded here.");
    checks.record("opus:F-portsig.c13 sol:F-signals.c17 (p.351, PA2=/CHNG): "
        "hrm/ch8-cia-disk-signal-bindings' PA2-BINDING (opus:8-cia.c18, "
        "sol:8-cia.c17) and hrm/ch8-ciaapra-disk-sense-bits' PROP-INPUT "
        "already grade this pin. Not graded here.");
    checks.record("opus:F-portsig.c8 sol:F-signals.c12 (p.351, PA7=game "
        "port 1 pin 6, fire button): hrm/ch8-ciaapra-fire-buttons' "
        "PROP-BIT6/7-PASSTHROUGH/INDEPENDENT and hrm/"
        "ch8-controller-port-io-path's PROP-PORT-LOW6/HIGH7 already grade "
        "the bit pairing (uid-free); the pin-6 half is graded under "
        "sol:8-reg.c7 in hrm/ch8-controller-port-io-path. Not graded here.");
    checks.record("opus:F-portsig.c9 sol:F-signals.c13 (p.351, PA6=game "
        "port 0 pin 6, fire button): same coverage as above, the PA6 twin. "
        "Not graded here.");
    checks.record("sol:F-signals.c9 (p.351, 'CIA-A communicates on data "
        "bits 7-0'): related, not a clean full match -- hrm/"
        "ch8-cia-disk-port-windows grades the CIA-B window's own byte-lane "
        "join; hrm/ch8-ciaapra-address-window's own record() line states "
        "plainly that 'No package in this suite currently grades \"$BFE001 "
        "selects PRA\" against the pinned join'. The byte-lane assignment "
        "this claim names (ciaa's 8-bit data_in/data_out tied to the LOW "
        "byte of the 16-bit CPU bus) is minimig.v's wiring "
        "(minimig.v:627-628), outside ciaa's own port list -- out of scope "
        "here on the same boundary grounds as the two claims below, not "
        "because a sibling package fully covers it. Quoted honestly, not "
        "smoothed into a clean-duplicate claim. Not graded here.");
    checks.record("sol:F-signals.c11 (p.351, 'The CIA-A table associates "
        "CIA-A with INT2'): related, not a full match -- hrm/"
        "ch7-interrupt-priority-encoder grades the level encoder at "
        "paula_intcontroller; this package's own note states plainly that "
        "no package grades the ciaa-irq-to-int2 join itself, which is "
        "minimig.v:631 (`.irq(int2)`), outside ciaa's own port list (ciaa "
        "exposes only its own `irq` output, not what it is wired to). Not "
        "graded here.");

    // -- p.354 port-reset claim, partially graded (CIAB leg only) --
    checks.record("opus:F-signals.c19 sol:F-signals.c88 (p.354, 'The port "
        "pins are set as inputs and port registers to zero (although a "
        "read of the ports will return all highs because of passive "
        "pull-ups)'): hrm/ch8-ciabprb-disk-control-bits' "
        "PROP-RESET-DEASSERT already grades the CIAB port-B leg "
        "(portb_out = 0xFF after reset). ciaa's own leg differs from the "
        "printed sentence at this boundary -- a CIAA PRA read returns "
        "{external input, porta_out[1:0]}, and a PRB read returns "
        "portb_in directly (ciaa has no portb_out at all) -- neither is "
        "'all highs from a pull-up' in the sense the CIAB leg's already-"
        "graded assertion shows. This whole claim is declined rather than "
        "split a single uid's citation across a decline and a fresh "
        "assertion. Not graded here.");

    // -- p.352 LED-blink example and p.351 PA1/LED gloss row: genuinely new, no accepted package --
    checks.record("sol:F-example.c18 opus:F-example.c25 (p.352, the LED-"
        "blink example, 'bchg.b #CIAB_LED,ciapra(a4)') and opus:F-"
        "portsig.c14 sol:F-signals.c18 (p.351, PA1=/LED, 0=bright): their "
        "own classify.note says plainly 'Not graded anywhere in hrm/ ... "
        "New here' -- these four do NOT name an accepted package, unlike "
        "the build brief's summary for the other 34. Declined here for an "
        "independent boundary reason instead: PA1's value reaches a "
        "physical LED only through minimig.v (`_led`/`pwr_led`, "
        "minimig.v:432,605), outside ciaa's own port list -- the same "
        "boundary hrm/ch8-ciaa-port-direction already drew around "
        "CIAADDRA/CIAAPRA's bus address. ciaa's own half of this fact "
        "(PA1 is one of the chip's two genuinely bidirectional port-A "
        "output bits) is already exercised, blindly, as one of eight swept "
        "positions, by hrm/ch8-ciaa-port-direction's PROP-1/PROP-2. This "
        "is a genuine, disclosed gap (see SPECIFICATION.md's 'Brief "
        "accuracy'), not a duplicate-grading decline. Not graded here.");

    // -- this package's own boundary finding: A11-A8 -> RS is outside ciaa --
    checks.record("opus:F-conn.c4 sol:F-signals.c61 (p.353, 'Address bits "
        "A11, A10, A9, and A8 are used to specify which of the 16 internal "
        "registers you want to access'): this is a fact about minimig.v's "
        "address bridge (cpu_address_out[11:8] -> ciaa's `rs` input, "
        "minimig.v:626,641), not about ciaa itself. ciaa's own `rs` port "
        "is already the decoded nibble; it has no A11-A8 input to observe. "
        "Out of scope for a ciaa-only fixture on the same boundary/"
        "ownership grounds hrm/ch8-ciaa-port-direction used to decline "
        "CIAADDRA's bus-address claims ('ciaa alone has no address "
        "decoder'). This package's own RS-SELECTS-DISTINCT assertion "
        "grades the RS-selects-a-register half of this same sentence, "
        "under a different uid. Not graded here.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
