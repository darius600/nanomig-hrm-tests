// Test purpose: grade printed page 326's SERIAL INTERFACE CONNECTOR PIN
//   ASSIGNMENT (J6) direction column (DIR: O/I) for the five RS-232 modem-
//   control lines plus RI, and printed page 327's "Modem control signals
//   (CTS, RTS, DTR, DSR, CD) are completely under software control" -- both
//   at the `ciab` module boundary, compiled alone with its four CIA
//   submodules (cia_int.v, cia_timera.v, cia_timerb.v, cia_timerd.v). This
//   is the SAME boundary hrm/appF-ciab-port-assignments and
//   hrm/ch8-ciabprb-disk-control-bits use (TOP := ciab, not TOP_MODULE =
//   minimig).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 326, 327 and 337.
// Requirement IDs: PROP-PA-OUTPUT-BITS, PROP-PA-INPUT-BITS,
//   PROP-MODEM-SOFTWARE-CONTROL. Claims file:
//   hrm-testpipeline/work/2026-09-17_appDE_packages.jsonl, filtered
//   classify.package == "appDE-ciab-port-signals" (16 claims; 14 uids
//   graded across the three propositions above, 2 declined -- see the
//   RECORDED line in main() and SPECIFICATION.md's claim inventory).
// Non-goals: anything outside the `ciab` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/2026-09-17_appDE_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// THE BOUNDARY THIS PACKAGE IS BUILT AROUND (per the build brief's own
//   "STALE DECLINE" note): `ciab` compiled alone carries no signal named
//   DTR, RTS, CTS, DSR, CD or RI -- only generic `porta_out[7:6]` (2 output
//   bits) and `porta_in[5:0]` (6 input bits), exactly the boundary
//   hrm/appF-ciab-port-assignments already documented in its own
//   SPECIFICATION.md for the identical p.351 PA-name claims. Two different
//   accepted packages sit on either side of that boundary for the SAME five
//   bindings (DTR=PA7, RTS=PA6, CD=PA5, CTS=PA4, DSR=PA3):
//     - hrm/appF-ciab-port-assignments (`ciab` top, like this package)
//       DECLINED the p.351 PA-name claims, arguing the name binding is not
//       constructible at this boundary, and its own record() text says (of
//       PA7=/DTR) "no accepted package currently grades this binding at all
//       (verified)". CHECKED HERE: that sentence is STALE, not live -- see
//       below.
//     - hrm/appF-cia-pin-bindings (`minimig` top, ONE LEVEL ABOVE this
//       boundary) now grades all five bindings fresh, as DTR-IS-PRA-BIT7,
//       RTS-IS-PRA-BIT6, CD-IS-PRA-BIT5, CTS-IS-PRA-BIT4, DSR-IS-PRA-BIT3
//       (test_minimig.cpp:367-401), by driving minimig's own top-level
//       `dtr`/`rts`/`cd`/`cts`/`dsr` pins and reading PRA through a real CPU
//       bus cycle -- i.e. it grades the NAME-to-BIT binding itself, at a
//       boundary this package's own top module (`ciab`) does not have.
//
//   This package's own 12 RS-232 claims (6 signals x 2 readers) do NOT name
//   a bit position at all -- each restates only "<signal> is an
//   output"/"<signal> is an input" (p.326's DIR column). That is decidable
//   at `ciab` alone, on a DIFFERENT AXIS from both of the above: not the
//   DDRA register's controllability (p.340, appF-ciab-port-assignments's
//   own PROP-DDRA-POSITION) and not the minimig-level name binding
//   (appF-cia-pin-bindings), but the plain hardware-wiring fact that
//   `porta_out` is DECLARED as a 2-bit OUTPUT port (`output [7:6]
//   porta_out`, ciab.v:20) and `porta_in` as a 6-bit INPUT port (`input
//   [5:0] porta_in`, ciab.v:19) -- a Verilog port-direction fact, not a
//   runtime-configurable one. Per the build brief's own instruction
//   ("grade it when this page states something narrower, or states it on a
//   different axis, and say in the label what the difference is"): GRADED
//   here, not declined, restricted to the direction/position half; the
//   name-to-bit identity is cited to appF-cia-pin-bindings (for the five it
//   covers) as established, not re-derived.
//
//   RI (bit 2, `ri & _joy3[4]` at minimig.v:658) is NOT one of the five
//   appF-cia-pin-bindings grades -- classify.evidence's own note says "No
//   accepted package grades a ring indicator at all" and that remains true;
//   RI's direction-only claim here is fresh coverage, not a duplicate of
//   anything.
//
// THE STALE-DECLINE CHECK, done against the assertion, not the decline
//   text: hrm/appF-ciab-port-assignments/test_appF-ciab-port-assignments.cpp
//   was opened and its five PA-name record() lines (sol:F-signals.c36..c40,
//   opus:F-portsig.c29..c33) read in full; each says, verbatim or in the
//   longer first-instance form, "no accepted package currently grades this
//   binding at all (verified: classify.note's own \"Not graded anywhere in
//   hrm/\" is correct)". hrm/appF-cia-pin-bindings/test_minimig.cpp was then
//   opened and read in full: DTR-IS-PRA-BIT7/RTS-IS-PRA-BIT6/
//   CD-IS-PRA-BIT5/CTS-IS-PRA-BIT4/DSR-IS-PRA-BIT3 are graded with expect()
//   at lines 367-401, driving minimig's real top-level dtr/rts/cd/cts/dsr
//   pins. CONFIRMED: that decline text is now false for those five bindings
//   -- but it is a decline written by a DIFFERENT package about DIFFERENT
//   claims (appendix F's p.351 PA-name claims), not about this package's
//   own appendix E p.326 direction-only claims, so nothing here needed
//   correcting -- this package never repeats or relies on that sentence.
//
// RTL read in full before this file was drafted: ciab.v (251 lines),
//   cia_int.v (70 lines) -- cia_timera.v/cia_timerb.v/cia_timerd.v present
//   only so Verilator can elaborate `ciab`, not read for this package's own
//   claims (no CRA/CRB/timer/TOD claim is owned here). Also read in full:
//   hrm/appF-ciab-port-assignments/test_appF-ciab-port-assignments.cpp and
//   SPECIFICATION.md (Harness bus-timing protocol and MUTATION-comment
//   convention, reused verbatim below as verified RTL-interface plumbing;
//   also the source of the stale-decline check above) and
//   hrm/appF-cia-pin-bindings/test_minimig.cpp (confirms the name-to-bit
//   citations this package leans on: DTR=PA7, RTS=PA6, CD=PA5, CTS=PA4,
//   DSR=PA3) and hrm/ch8-disk-index-interrupt-path/
//   test_ch8-disk-index-interrupt-path.cpp (PROP-FLAG-PIN, cited on the
//   decline below).
//
// RTL facts established by that reading:
//   - ciab.v:19-20: `input [5:0] porta_in` / `output [7:6] porta_out` --
//     `ciab` implements PA7/PA6 as driven outputs and PA5..PA0 as inputs
//     only; it never drives PA5..PA0.
//   - ciab.v:126: `porta_out[7:6] = (~ddrporta[7:6]) | regporta[7:6]` --
//     regporta/ddrporta are written ONLY at ciab.v:97-103 (`wr && pra`) and
//     ciab.v:106-112 (`wr && ddra`) respectively; grep of `regporta` and
//     `ddrporta` across ciab.v (the whole file) shows no other assignment
//     anywhere -- no timer, no ICR output and no other submodule's signal
//     ever feeds either register. This is what makes PROP-MODEM-SOFTWARE-
//     CONTROL's negative control (below) a genuine RTL fact, not an
//     assumption.
//   - ciab.v:86-94: `porta_in2 <= porta_in` on `posedge clk` when
//     `clk7_en` -- a one-cycle registered synchronizer, same as
//     appF-ciab-port-assignments's own `set_porta_in()` documents.
//   - ciab.v:118: `pa_out[7:0] = {porta_out[7:6],porta_in2[5:0]}` on a PRA
//     read -- the read mux a PRA read returns.
//   - minimig.v:658-659 (read for citation only, not this package's own
//     boundary): `.porta_in({cd,cts,dsr,ri&_joy3[4],1'b1,_joy4[4]})` over
//     ciab's 6-bit `porta_in[5:0]` places cd=bit5, cts=bit4, dsr=bit3,
//     ri&_joy3[4]=bit2; `.porta_out({dtr,rts})` over ciab's `porta_out[7:6]`
//     places dtr=bit7, rts=bit6 -- confirmed against
//     hrm/appF-cia-pin-bindings's own measured DTR-IS-PRA-BIT7/
//     RTS-IS-PRA-BIT6/CD-IS-PRA-BIT5/CTS-IS-PRA-BIT4/DSR-IS-PRA-BIT3, cited
//     rather than re-measured (that package already drives the real minimig
//     pins through a real CPU bus cycle; this package cannot reach `dtr`,
//     `rts`, `cd`, `cts` or `dsr` by name at all, only their `ciab`-level
//     bit positions).
//
// Non-goals / scope carve-outs:
//   - which named RS-232 signal sits at which PA bit position -- a
//     minimig.v:658-659 wiring fact, out of `ciab` alone's boundary; graded
//     for DTR/RTS/CD/CTS/DSR at hrm/appF-cia-pin-bindings (minimig top);
//     not re-graded here, cited only.
//   - CIA-B FLAG carrying INDEX*, the disk index pulse (p.337) -- already
//     graded by hrm/ch8-disk-index-interrupt-path's PROP-FLAG-PIN; declined
//     below (see main()), NOT because of the appendix-F "no accepted
//     package grades this at all" boundary argument, but because a prior
//     assertion grades the SAME proposition p.337 states: PROP-FLAG-PIN's
//     stimulus is a PULSE, and p.337's own gloss is "disk index PULSE*" (a
//     word appendix F's p.351 printing of the same table does not carry --
//     see SPECIFICATION.md's cross-printing note). p.337 makes no
//     edge-polarity claim of the kind p.341 does (the NM-HRM-063 trap
//     hrm/appF-ciab-port-assignments's own PROP-FLAG-EDGE-POLARITY exists
//     to grade), so nothing of that kind is owed here.
//   - p.337's CIA-A PA5..PA0 block and CIA-B PB7..PB0 block -- not this
//     package's claims (classify.package routed them elsewhere); not read
//     for content beyond confirming the FLAG/INDEX* row's position in the
//     shuffled text layer (see below).
//
// CORPUS DEFECT, read from the render per the build brief and
//   appE/corpus-defects.md's own p.337 section: p.337's text layer is
//   shuffled in three blocks, the third being the CIA-B footer, where
//   "disk index pulse*" is displaced from `F....INDEX*` onto
//   `PC...not used`. VERIFIED against figures/p337.png directly (not taken
//   from the note): the render's CIA-B block prints, in order, PA7..com
//   line DTR*, driven output / PA6..com line RTS*, driven output / PA5..com
//   line carrier detect* / PA4..com line CTS* / PA3..com line DSR* /
//   PA2..SEL Centronics control / ... / PC...not used / F....INDEX* disk
//   index pulse*. p.326's own text layer carries the OTHER, independent
//   defect the build brief and corpus-defects.md both describe (the NOTES
//   column shifted one row, inverting which prose sentence sits next to
//   which NAME/DIR row) -- VERIFIED against p326.txt directly: the NAME/DIR
//   pairing itself (TXD O, RXD I, RTS O, CTS I, DSR I, CD I, DTR O, RI I) is
//   NOT shifted, only the trailing NOTES sentence is (e.g. "Signal ground"
//   sits on RTS's row, "50 ma maximum *** WARNING -5V ***" on CTS's row).
//   Both readers' restatements below use the DIR column only, which this
//   package independently confirms is undisplaced.
//
// MUTATION METHODOLOGY: every expect() below is guarded by a compile-time
//   `-D` flag. Every mutation was actually built and run (WSL Verilator,
//   `make test MUT=<NAME>`, full `rm -rf obj_dir` rebuild) -- see the RUN
//   lines after each MUTATION comment, filled in from what that run
//   actually printed.

#include <cstdint>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vciab.h"

#ifndef MUT_A1_SKIP_DDRA
#define MUT_A1_SKIP_DDRA 0
#endif
#ifndef MUT_A2_SKIP_SYNC_TICK
#define MUT_A2_SKIP_SYNC_TICK 0
#endif
#ifndef MUT_A3_INJECT_WRITE
#define MUT_A3_INJECT_WRITE 0
#endif

namespace {

constexpr uint8_t RS_PRA  = 0x0;
constexpr uint8_t RS_DDRA = 0x2;
constexpr uint8_t RS_ICR  = 0xD;
constexpr uint8_t RS_TALO = 0x4;
constexpr uint8_t RS_TAHI = 0x5;
constexpr uint8_t RS_CRA  = 0xE;

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

// Bus-timing protocol for ciab.v, reused verbatim from
// hrm/appF-ciab-port-assignments's own Harness (this package's model, per
// the build task): regporta/ddrporta/tmcr/icr are posedge-clk,
// clk7_en-gated, so a write needs one tick to latch; the read mux
// (data_out, porta_out) is combinational and needs only eval().
class Harness {
public:
    Harness() : dut(new Vciab) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->aen = 0;
        dut->rd = 0;
        dut->wr = 0;
        dut->reset = 1;
        dut->rs = 0;
        dut->data_in = 0;
        dut->tick = 0;
        dut->eclk = 0;
        dut->flag = 0;
        dut->porta_in = 0;
        for (int i = 0; i < 8; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick();
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
        tick();
        dut->aen = 0;
        dut->wr = 0;
        dut->rs = 0;
        dut->data_in = 0;
        tick();
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

    uint8_t porta_out_now() {
        dut->eval();
        return dut->porta_out & 0x3;
    }

    // porta_in passes through a registered synchronizer (ciab.v:86-94,
    // `porta_in2 <= porta_in` on posedge clk) before a PRA read can see it,
    // so this drives the pin AND ticks the clock once to let it land.
    // `skip_tick`, when true, is PROP-PA-INPUT-BITS's stimulus mutation:
    // the pin is driven but the synchronizing tick is omitted, so a PRA
    // read still sees the STALE porta_in2 value.
    void set_porta_in(uint8_t v, bool skip_tick) {
        dut->porta_in = v & 0x3F;
        if (!skip_tick) tick();
    }

    void set_flag(bool v) {
        dut->flag = v ? 1 : 0;
        dut->eval();
    }

    // One count pulse for Timer A's `eclk`: high for one full tick, then
    // low for one full tick, so exactly one decrement occurs per call --
    // same shape as write_reg's own assert-then-deassert (reused from
    // hrm/appF-ciab-port-assignments's own Harness::pulse_eclk).
    void pulse_eclk(int n = 1) {
        for (int i = 0; i < n; ++i) {
            dut->eclk = 1;
            tick();
            dut->eclk = 0;
            tick();
        }
    }

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    Vciab* dut;
};

// ---------------------------------------------------------------------
// FIXTURE-LIVE (porta): a basic liveness proof, before any claim-grading
// assertion is trusted -- two distinct, known PRA/DDRA writes must both
// arrive and read back correctly on porta_out, and a porta_in sweep must
// both arrive and read back correctly through a PRA read. Not itself a
// page claim -- no uid, not graded.
// ---------------------------------------------------------------------
bool fixture_live_porta(Harness& h) {
    h.write_reg(RS_DDRA, 0xFF);
    h.write_reg(RS_PRA, 0xC0);
    const uint8_t out1 = h.porta_out_now();
    h.set_porta_in(0x15, false);
    const uint8_t pra1 = h.read_reg(RS_PRA);
    h.set_porta_in(0x2A, false);
    const uint8_t pra2 = h.read_reg(RS_PRA);
    return out1 == 0x3 && pra1 == 0xD5 && pra2 == 0xEA;
}

// ---------------------------------------------------------------------
// PROP-PA-OUTPUT-BITS (p.326, J6 DIR column: DTR="O", RTS="O"): PA7 (DTR's
// bit, per appF-cia-pin-bindings' DTR-IS-PRA-BIT7, cited not re-derived)
// and PA6 (RTS's bit, RTS-IS-PRA-BIT6) are both DRIVEN OUTPUT bits at
// `ciab`'s own boundary -- `porta_out` is declared `output [7:6]`
// (ciab.v:20), and with DDRA set to output mode, a PRA write reaches
// porta_out on BOTH bits independently. This is the direction/position
// half of "DTR is an output" / "RTS is an output"; the name-to-bit
// identity half is minimig.v:659, graded at appF-cia-pin-bindings, cited
// above, not re-graded here.
// `omit_ddra` is the stimulus mutation: the DDRA=0xFF write is skipped, so
// PRA writes cannot reach porta_out (pulled high instead).
// ---------------------------------------------------------------------
bool prop_pa_output_bits(Harness& h, bool omit_ddra) {
    if (!omit_ddra) h.write_reg(RS_DDRA, 0xFF);
    h.write_reg(RS_PRA, 0x80);  // bit7=1 (DTR high), bit6=0 (RTS low)
    const uint8_t out1 = h.porta_out_now();
    h.write_reg(RS_PRA, 0x40);  // bit7=0 (DTR low), bit6=1 (RTS high)
    const uint8_t out2 = h.porta_out_now();
    const bool pa7_driven = ((out1 >> 1) & 1) == 1 && ((out2 >> 1) & 1) == 0;
    const bool pa6_driven = (out1 & 1) == 0 && (out2 & 1) == 1;
    return pa7_driven && pa6_driven;
}

// ---------------------------------------------------------------------
// PROP-PA-INPUT-BITS (p.326, J6 DIR column: CD="I", CTS="I", DSR="I",
// RI="I"): PA5 (CD, per CD-IS-PRA-BIT5), PA4 (CTS, CTS-IS-PRA-BIT4), PA3
// (DSR, DSR-IS-PRA-BIT3, all three cited from appF-cia-pin-bindings) and
// PA2 (RI, minimig.v:658 `ri & _joy3[4]` -- not covered by any accepted
// package, fresh here) are plain, read-only PASS-THROUGH input bits at
// `ciab`'s own boundary -- `porta_in` is declared `input [5:0]`
// (ciab.v:19), and a PRA read returns exactly what was driven onto
// porta_in (through the one-cycle synchronizer), independent of any
// register write. This is the direction/position half of "CD/CTS/DSR/RI
// is an input"; the name-to-bit identity half for CD/CTS/DSR is graded at
// appF-cia-pin-bindings, cited above; RI has no accepted-package citation
// at all (classify.evidence's own note: "No accepted package grades a ring
// indicator at all").
// `skip_second_sync_tick` is the stimulus mutation: the SECOND porta_in
// drive omits its synchronizing tick, so the PRA read sees the STALE first
// pattern instead of the second one.
// ---------------------------------------------------------------------
bool prop_pa_input_bits(Harness& h, bool skip_second_sync_tick) {
    h.write_reg(RS_DDRA, 0xFF);
    // Pattern 1: bit5=0, bit4=1, bit3=0, bit2=1 (0b picking out 5..2 = 0101)
    h.set_porta_in(0x15, false);  // 0b010101 -> bit5=0,bit4=1,bit3=0,bit2=1,bit1=0,bit0=1
    const uint8_t pra1 = h.read_reg(RS_PRA);
    // Pattern 2: bit5=1, bit4=0, bit3=1, bit2=0 -- complementary pattern.
    h.set_porta_in(0x2A, skip_second_sync_tick);  // 0b101010
    const uint8_t pra2 = h.read_reg(RS_PRA);

    const bool pa5_ok = (((pra1 >> 5) & 1) == 0) && (((pra2 >> 5) & 1) == 1);
    const bool pa4_ok = (((pra1 >> 4) & 1) == 1) && (((pra2 >> 4) & 1) == 0);
    const bool pa3_ok = (((pra1 >> 3) & 1) == 0) && (((pra2 >> 3) & 1) == 1);
    const bool pa2_ok = (((pra1 >> 2) & 1) == 1) && (((pra2 >> 2) & 1) == 0);
    return pa5_ok && pa4_ok && pa3_ok && pa2_ok;
}

// ---------------------------------------------------------------------
// PROP-MODEM-SOFTWARE-CONTROL (p.327, "Modem control signals (CTS, RTS,
// DTR, DSR, CD) are completely under software control"): at `ciab`'s own
// boundary, this is decidable for the two OUTPUT bits (DTR=PA7, RTS=PA6):
// porta_out[7:6] must be moved ONLY by a direct PRA/DDRA write, and by
// NOTHING else in the module -- not a Timer A/B underflow, not an ICR
// event, not the mere passage of clock ticks. Graded as a negative control
// (hands-off window: run Timer A to two underflows and pulse/read ICR's
// FLG bit repeatedly, with NO PRA/DDRA write anywhere in that window --
// porta_out[7:6] must stay byte-for-byte unchanged) paired with a positive
// control on the SAME Harness (a direct PRA write immediately afterward
// DOES move porta_out[7:6] to match) -- proving the "stayed unchanged"
// result is not a dead/stuck read channel.
// MEASURED (ciab.v, full-file grep): `regporta`/`ddrporta` (the only state
// feeding porta_out[7:6]) are assigned ONLY at ciab.v:97-103 (`wr && pra`)
// and ciab.v:106-112 (`wr && ddra`) -- no timer output, no ICR signal and
// no other submodule's port reaches either register anywhere in the file.
// `inject_write` is the stimulus mutation: a real PRA write (a DIFFERENT
// value than the hands-off window started with) is issued PARTWAY THROUGH
// that window -- the "stayed unchanged" conjunct must then read FALSE,
// proving the check is watching the RTL and not simply always true.
// ---------------------------------------------------------------------
bool prop_modem_software_control(Harness& h, bool inject_write) {
    h.write_reg(RS_DDRA, 0xFF);
    h.write_reg(RS_PRA, 0x80);  // DTR=1 (PA7), RTS=0 (PA6)
    const uint8_t before = h.porta_out_now();

    // Hands-off window: Timer A armed and run to two underflows, ICR's FLG
    // bit armed and pulsed twice -- real hardware activity elsewhere in the
    // same module, with no PRA/DDRA access at all (unless mutated).
    h.write_reg(RS_TALO, 0x01);
    h.write_reg(RS_TAHI, 0x00);
    h.write_reg(RS_CRA, 0x01);   // START=1, continuous mode.
    h.pulse_eclk(2);
    h.pulse_eclk(2);
    h.write_reg(RS_ICR, 0x90);   // set-mode, unmask FLG.
    h.set_flag(true);
    h.tick();
    h.tick();
    if (inject_write) h.write_reg(RS_PRA, 0x40);  // spurious write mid-window.
    h.set_flag(false);
    h.read_reg(RS_ICR);
    const uint8_t mid = h.porta_out_now();
    const bool stayed_unchanged = (mid == before);

    // Positive control: a genuine write now DOES move it, on the SAME
    // Harness -- proving the read channel above was not simply stuck.
    h.write_reg(RS_PRA, 0x40);  // DTR=0, RTS=1 -- the opposite pattern.
    const uint8_t after = h.porta_out_now();
    const bool write_moves_it = (after != before) && (((after >> 1) & 1) == 0) && ((after & 1) == 1);

    return stayed_unchanged && write_moves_it;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    {
        Harness h;
        const bool live = fixture_live_porta(h);
        checks.note(std::string("FIXTURE-LIVE (porta): DDRA=0xFF, PRA<-0xC0 gives porta_out=0x3; a PRA "
            "read then returns {porta_out[7:6],porta_in[5:0]} -- 0xD5 with porta_in=0x15, 0xEA with "
            "porta_in=0x2A -- measured ") + (live ? "correctly" : "INCORRECTLY") + " -- liveness "
            "measurement only, not itself a page claim, not graded [no page -- infrastructure]");
    }

    {
        Harness h;
        const bool result = prop_pa_output_bits(h, MUT_A1_SKIP_DDRA);
        checks.expect(result,
            "PROP-PA-OUTPUT-BITS: at ciab's own boundary, PA7 and PA6 are both DRIVEN OUTPUT bits -- "
            "`porta_out` is declared `output [7:6]` (ciab.v:20), and with DDRA set to output mode a PRA "
            "write reaches porta_out on both bits independently (0x80 -> bit7=1,bit6=0; 0x40 -> "
            "bit7=0,bit6=1). This is the direction/position half of \"DTR is an output\"/\"RTS is an "
            "output\" -- narrower than the name-to-bit binding (DTR-IS-PRA-BIT7/RTS-IS-PRA-BIT6), which "
            "is graded at the `minimig` boundary by hrm/appF-cia-pin-bindings (cited, not re-derived) "
            "[p.326 J6, DIR column: DTR \"O\", RTS \"O\"] sol:E-rs232.c157 sol:E-rs232.c165 "
            "opus:E-serial-iface.c10 opus:E-serial-iface.c18");
    }
    // MUTATION PROP-PA-OUTPUT-BITS stimulus red MUT_A1_SKIP_DDRA: the DDRA=0xFF write is skipped, leaving DDRA at its reset value 0 (input/pulled-high mode for PA7/PA6) -- both PRA writes leave porta_out pulled high (0x3) instead of following PRA, so both the pa7_driven and pa6_driven conjuncts go red.
    // RUN (make test MUT=MUT_A1_SKIP_DDRA): 1 failed (PROP-PA-OUTPUT-BITS), all others unaffected. Reverted.

    {
        Harness h;
        const bool result = prop_pa_input_bits(h, MUT_A2_SKIP_SYNC_TICK);
        checks.expect(result,
            "PROP-PA-INPUT-BITS: at ciab's own boundary, PA5, PA4, PA3 and PA2 are plain, read-only "
            "PASS-THROUGH input bits -- `porta_in` is declared `input [5:0]` (ciab.v:19), and a PRA read "
            "returns exactly what was driven onto porta_in through its one-cycle synchronizer (0x15 -> "
            "bits 5/4/3/2 = 0/1/0/1; 0x2A -> the complementary 1/0/1/0), independent of any register "
            "write. This is the direction/position half of \"CD/CTS/DSR/RI is an input\" -- narrower than "
            "the name-to-bit binding, graded at `minimig` for CD/CTS/DSR by hrm/appF-cia-pin-bindings "
            "(cited, not re-derived); RI (PA2, minimig.v:658 `ri & _joy3[4]`) has no accepted-package "
            "citation at all, fresh here [p.326 J6, DIR column: CD \"I\", CTS \"I\", DSR \"I\", RI \"I\"] "
            "sol:E-rs232.c158 sol:E-rs232.c159 sol:E-rs232.c161 sol:E-rs232.c166 opus:E-serial-iface.c11 "
            "opus:E-serial-iface.c12 opus:E-serial-iface.c14 opus:E-serial-iface.c19");
    }
    // MUTATION PROP-PA-INPUT-BITS stimulus red MUT_A2_SKIP_SYNC_TICK: the second porta_in drive (0x2A) omits its synchronizing tick -- the PRA read still sees pattern 1's stale, already-latched value (0x15's bits) instead of pattern 2's, so all four bit-toggle conjuncts (pa5_ok, pa4_ok, pa3_ok, pa2_ok) go red.
    // RUN (make test MUT=MUT_A2_SKIP_SYNC_TICK): 1 failed (PROP-PA-INPUT-BITS), all others unaffected. Reverted.

    {
        Harness h;
        const bool result = prop_modem_software_control(h, MUT_A3_INJECT_WRITE);
        checks.expect(result,
            "PROP-MODEM-SOFTWARE-CONTROL: porta_out[7:6] (DTR/RTS's bits) moves ONLY on a direct PRA/DDRA "
            "write -- held through a hands-off window containing real Timer A activity (armed, run to two "
            "underflows) and a real ICR FLG pulse/read, with no PRA/DDRA access at all, it stays "
            "byte-for-byte unchanged (negative control); a direct PRA write immediately afterward, on the "
            "same Harness, DOES move it to the expected opposite pattern (positive control, proving the "
            "negative result is not a dead read channel). MEASURED: ciab.v's `regporta`/`ddrporta` are "
            "assigned only at ciab.v:97-103 (wr&&pra) and ciab.v:106-112 (wr&&ddra) -- no timer, ICR or "
            "other submodule output reaches either register anywhere in the file (full-file grep) [p.327 "
            "\"Modem control signals (CTS, RTS, DTR, DSR, CD) are completely under software control\"] "
            "sol:E-rs232.c170 opus:E-serial-timing.c5");
    }
    // MUTATION PROP-MODEM-SOFTWARE-CONTROL stimulus red MUT_A3_INJECT_WRITE: a real PRA write (0x40, the opposite pattern) is issued PARTWAY THROUGH the hands-off window -- porta_out changes mid-window, so the stayed_unchanged conjunct compares the post-window value against the pre-window `before` and finds them different, going red.
    // RUN (make test MUT=MUT_A3_INJECT_WRITE): 1 failed (PROP-MODEM-SOFTWARE-CONTROL), all others unaffected. Reverted.

    // ------------------------------------------------------------------
    // Declined claim (2 of 16): a prior assertion already grades the SAME
    // proposition p.337 states -- not the appendix-F "no accepted package
    // grades this at all" boundary argument (checked above and found
    // inapplicable to this package's own claims), but a genuine same-
    // proposition decline, per the build brief's own lot-H finding: p.337
    // makes no edge-polarity claim (unlike p.341, appF-ciab-port-
    // assignments's PROP-FLAG-EDGE-POLARITY / NM-HRM-063), and its own
    // gloss -- "disk index PULSE*" -- is exactly the stimulus shape
    // hrm/ch8-disk-index-interrupt-path's PROP-FLAG-PIN already measures.
    // ------------------------------------------------------------------
    checks.record(
        "p.337, CIA-B footer, \"F....INDEX*\" glossed \"disk index pulse*\" (text layer displaces this "
        "gloss onto \"PC...not used\" -- corpus-defects.md, verified against figures/p337.png directly for "
        "this package): hrm/ch8-disk-index-interrupt-path's PROP-FLAG-PIN already grades that a pulse on "
        "paula_floppy's real `index` output, carried across minimig.v:536+:657 onto ciab's `flag` input, "
        "is what ciab treats as the FLAG pin's DSKINDEX* pulse -- ICR bit 4 (FLG) latches and, with FLG "
        "unmasked, `irq` rises (a non-clocked ICR read returns 0x90). PROP-FLAG-PIN's stimulus IS a pulse "
        "(rise then fall), and p.337's own wording states only that the FLAG input carries a PULSE -- no "
        "edge-polarity requirement of the kind p.341 states (\"a NEGATIVE TRANSITION... will set the FLAG "
        "interrupt bit\", the NM-HRM-063 trap hrm/appF-ciab-port-assignments's own PROP-FLAG-EDGE-POLARITY "
        "exists to grade separately). So here, unlike appendix F's p.341 case, the accepted assertion DOES "
        "grade the same proposition this page states, and this is a genuine decline, not a re-grade "
        "avoided by a stale citation. Not graded here. [claims: sol:E-portsig.c42, opus:E-portsig.c44 -- "
        "citing hrm/ch8-disk-index-interrupt-path's PROP-FLAG-PIN, uids opus:8-cia.c56, opus:8-dskint.c9, "
        "sol:8-int.c9]");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
