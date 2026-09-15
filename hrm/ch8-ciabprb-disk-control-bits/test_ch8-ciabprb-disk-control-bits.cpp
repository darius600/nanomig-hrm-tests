// Test purpose: assert what printed pp.244-245 claim about CIABPRB
//   ($BFD100, ciab's PRB register, rs=4'h1) at the `ciab` module boundary,
//   compiled alone with its CIA submodules -- the eight PB disk-control
//   bits' independent bit-position transfer, their reset/pull-up deasserted
//   (HIGH) default, the DSKDIREC/DSKSTEP* separate-write ordering rule, and
//   DSKSTEP*'s high-momentarily-low-high pulse shape. Per SPECIFICATION.md
//   (committed 8125735, before this file was opened for the RTL): `ciab`
//   alone carries no signal named DSKMOTOR*, DSKSEL3*-DSKSEL0*, DSKSIDE,
//   DSKDIREC or DSKSTEP* -- only a generic `portb_out[7:0]`. The binding of
//   a bit POSITION to one of those NAMES is a one-line structural fact at
//   minimig.v:650, one level above this fixture's boundary, and is not
//   re-derived or exercised here. What is graded is exactly the four
//   propositions SPECIFICATION.md assigns: PROP-POSITION, PROP-RESET-
//   DEASSERT, PROP-ORDER, PROP-STEP-SHAPE.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 244-245; committed specification 8125735 (SPECIFICATION.md, this
//   package).
// Requirement IDs: PROP-POSITION, PROP-RESET-DEASSERT, PROP-ORDER,
//   PROP-STEP-SHAPE, per SPECIFICATION.md's claim inventory. Claims file:
//   hrm-testpipeline/work/ch8-s2-build/ch8-ciabprb-disk-control-bits.jsonl
//   (30 claims; 14 scored across the four propositions above, 16 declined
//   -- see RECORDED lines in main() and SPECIFICATION.md's "Claims not
//   assigned an assertion").
// Non-goals / scope carve-outs, per the build task and SPECIFICATION.md:
//   - which named drive signal (DSKMOTOR*, DSKSEL3*..0*, DSKSIDE, DSKDIREC,
//     DSKSTEP*) sits at which PB bit position is a minimig.v:650 wiring
//     fact, out of `ciab` alone's boundary -- declined, not graded.
//   - what a drive does with an asserted (LOW) level -- runs its motor,
//     selects itself -- is a paula_floppy.v / physical-drive fact, also out
//     of boundary and not graded, even where a claim's own evidence cites
//     paula_floppy.v as corroboration.
//   - how short "momentarily" (DSKSTEP*'s low phase) must be: a duration
//     with no companion interval on this page to form a ratio with, left to
//     lot c4's drive-timing package.
//
// RTL read for step 2, after SPECIFICATION.md was committed (8125735): read
//   ciab.v (224 lines) in full, plus its four instantiated submodules
//   (cia_int.v, cia_timera.v, cia_timerb.v, cia_timerd.v -- needed only for
//   Verilator to elaborate the top module, not opened/read for content).
//   Also read hrm/ch8-ciaa-port-direction/Makefile and its full test .cpp
//   (Checks class, Harness bus-timing protocol, main()-layout, and
//   MUTATION-comment convention -- the suite's own coding convention, which
//   the top-level build brief mandates character-identical, and the only
//   other package in the suite already building a `cia*` fixture), reused
//   verbatim below as verified RTL-interface plumbing, not as a source of
//   this package's own assertions or expected values, which come only from
//   SPECIFICATION.md and the claims file.
//
// RTL facts established by that reading (none of this shaped
//   SPECIFICATION.md, which was committed first; ciab.v itself was in fact
//   opened once before that commit too, during toolchain confirmation --
//   disclosed in SPECIFICATION.md's own closing section):
//   - ciab.v's address decoder: rs=4'h1 -> prb, rs=4'h3 -> ddrb, selected by
//     `enable = aen & (rd | wr)` together with the rs value -- never by
//     name or bus address (`ciab` alone has no address decoder for
//     $BFD100; that is gary.v's job, out of this boundary).
//   - PRB write: `if (reset) regportb<=8'd0; else if (wr&&prb)
//     regportb<=data_in;` (posedge clk, clk7_en-gated).
//   - DDRB write: same shape, `ddrportb<=8'd0` on reset, `data_in` on
//     `wr&&ddrb`.
//   - Output assignment, combinational: `assign portb_out[7:0] =
//     (~ddrportb[7:0]) | regportb[7:0];` -- with DDRB=0xFF (all outputs),
//     portb_out == regportb exactly, byte for byte, with no path from one
//     bit position to another; with DDRB=0x00 (all inputs, un-driven),
//     portb_out reads all-1s regardless of regportb -- the 8520 pull-up.
//   - PRB read mux: `if (!wr && prb) pb_out[7:0] = portb_out[7:0];` --
//     reading PRB mirrors the output pins exactly, so the register really
//     is bidirectionally observable at this boundary, not a write-only
//     shadow store.
//   - Both regportb and ddrportb clear to 0 on `reset` (posedge-clk,
//     clk7_en-gated), so after reset, DDRB=0 and PRB=0, and the pull-up OR
//     leaves portb_out = 0xFF -- every PB bit deasserted-high, unconditionally
//     of what was written before the reset pulse.
//
// Every mutation below was actually applied (edit -> rebuild -> run ->
// revert -> rebuild) and the FAIL/PASS lines quoted are what the run
// printed, not predictions.
//
// MUTATION FIXTURE-LIVE stimulus red: the second PRB write changed from the
//   distinct sentinel 0x5A to a repeat of the first (0xA5) -> FAIL, second
//   reading no longer differs from the first (both 0xA5). Reverted.
// MUTATION opus:8-cia.c4 stimulus red: PROP-POSITION's stimulus, at bit 3,
//   changed from clearing that bit (PRB<-0xF7) to leaving it set
//   (PRB<-0xFF, the omit-clear bug) -> FAIL, bit 3 of `portb_out` never
//   moved off 1, so bit_moved was false for PB3. Reverted.
// MUTATION opus:8-cia.c22 stimulus red: PROP-RESET-DEASSERT's stimulus
//   omitted the `reset` pulse after PRB<-0x00 -> FAIL, `portb_out` read
//   0x00, not the expected 0xFF. Reverted.
// MUTATION opus:8-cia.c48 stimulus red: PROP-ORDER's third write (meant to
//   restore STEP high while leaving DIREC alone) changed from 0xFD to 0xFF
//   -> FAIL, DIREC (bit 1) read back 1 after that write, not the 0 written
//   by the first, separate write. Reverted.
// MUTATION opus:8-cia.c50 stimulus red: PROP-STEP-SHAPE's middle write
//   changed from the dip 0xFE to a repeat of 0xFF (the no-dip bug) -> FAIL,
//   `portb_out`[0] never went low, so the demanded high/low/high shape did
//   not occur. Reverted.
// MUTATION CONTROL-BIT0 stimulus red: the same omit-clear bug PROP-POSITION
//   uses, applied to bit 0 alone (the control's own bit) -> FAIL, bit 0
//   never moved off 1. Reverted.

#include <cstdint>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vciab.h"

namespace {

constexpr uint8_t RS_PRB = 0x1;
constexpr uint8_t RS_DDRB = 0x3;

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

// Bus-timing protocol for ciab.v, adapted from hrm/ch8-ciaa-port-direction's
// own Harness (the only other package in this suite building a `cia*`
// fixture): regportb/ddrportb are posedge-clk, clk7_en-gated, so a write
// needs one tick to latch; portb_out is combinational and needs only
// eval(). ciab additionally exposes tick/eclk/flag/irq/porta_in/porta_out
// (timers, TOD and CIA-A-shaped port A) -- held at fixed, inactive values
// throughout, since no claim in this package concerns them.
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

    uint8_t portb_out_now() {
        dut->eval();
        return dut->portb_out;
    }

    // A mid-test reset pulse, distinct from the constructor's own -- used by
    // PROP-RESET-DEASSERT to show the deasserted default is reached FROM an
    // asserted (written-low) state, not merely inherited from power-on.
    void pulse_reset() {
        dut->reset = 1;
        for (int i = 0; i < 4; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick();
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
// FIXTURE-LIVE: `ciab` has never been measured by this package before.
// Two distinct, known bytes must both arrive at PRB and read back
// correctly and distinctly, via `portb_out`, before anything else this
// package depends on is trusted.
// ---------------------------------------------------------------------
bool fixture_live(Harness& h, bool repeat_bug = false) {
    h.write_reg(RS_DDRB, 0xFF);
    h.write_reg(RS_PRB, 0xA5);
    const uint8_t r1 = h.portb_out_now();
    h.write_reg(RS_PRB, repeat_bug ? uint8_t{0xA5} : uint8_t{0x5A});
    const uint8_t r2 = h.portb_out_now();
    return r1 == 0xA5 && r2 == 0x5A && r1 != r2;
}

// ---------------------------------------------------------------------
// PROP-POSITION / CONTROL shared comparator: with DDRB=0xFF, clearing one
// PRB bit must move exactly that bit of `portb_out` to 0, leave every other
// bit at 1, and restoring the write must bring the whole byte back to
// 0xFF. `omit_clear_bug`, when true, is the stimulus mutation: the "clear"
// write never actually clears the bit under test, so a position that
// really is independent and really does transfer will (correctly) fail to
// move -- the comparator itself is unchanged.
// ---------------------------------------------------------------------
bool bit_transfers_independently(Harness& h, int bit, bool omit_clear_bug = false) {
    const uint8_t mask = static_cast<uint8_t>(1u << bit);
    h.write_reg(RS_DDRB, 0xFF);
    h.write_reg(RS_PRB, 0xFF);
    const uint8_t baseline = h.portb_out_now();
    h.write_reg(RS_PRB, omit_clear_bug ? uint8_t{0xFF}
                                        : static_cast<uint8_t>(0xFF & ~mask));
    const uint8_t cleared = h.portb_out_now();
    h.write_reg(RS_PRB, 0xFF);
    const uint8_t restored = h.portb_out_now();

    const bool bit_moved = (baseline & mask) == mask && (cleared & mask) == 0x00;
    const bool others_untouched = static_cast<uint8_t>(cleared | mask) == 0xFF;
    const bool restores = restored == 0xFF;
    return bit_moved && others_untouched && restores;
}

// ---------------------------------------------------------------------
// PROP-POSITION: PB is eight independently writable AND readable output
// bits (opus:8-cia.c4, sol:8-cia.c3) -- graded across all eight positions,
// plus a CPU read-back of PRB itself confirming it mirrors `portb_out`
// rather than being a write-only shadow.
// ---------------------------------------------------------------------
bool prop_position(Harness& h, std::string& detail, int omit_clear_bug_at_bit = -1) {
    bool all_ok = true;
    for (int i = 0; i < 8; ++i) {
        const bool ok = bit_transfers_independently(h, i, i == omit_clear_bug_at_bit);
        detail += "PB" + std::to_string(i) + "=" + (ok ? "PASS " : "FAIL ");
        all_ok = all_ok && ok;
    }
    h.write_reg(RS_DDRB, 0xFF);
    h.write_reg(RS_PRB, 0x3C);
    const uint8_t written_view = h.portb_out_now();
    const uint8_t read_back = h.read_reg(RS_PRB);
    const bool readback_ok = read_back == written_view && written_view == 0x3C;
    detail += "readback=" + std::string(readback_ok ? "PASS" : "FAIL");
    return all_ok && readback_ok;
}

// ---------------------------------------------------------------------
// PROP-RESET-DEASSERT: PB7 (opus:8-cia.c22, "DSKMOTOR* ... active low"),
// PB6-PB3 (opus:8-cia.c33/c35/c37/c40, "DSKSEL3*..0* ... active low"), and
// opus:8-cia.c29/sol:8-cia.c27 ("all drive motors turn off after system
// reset") all reduce, at this boundary, to one measurement: after `reset`,
// every PB bit is HIGH -- deasserted -- unconditionally of what PRB held
// beforehand. `omit_reset_bug`, when true, is the stimulus mutation: PRB is
// written low but `reset` is never pulsed.
// ---------------------------------------------------------------------
bool prop_reset_deassert(Harness& h, bool omit_reset_bug = false) {
    h.write_reg(RS_DDRB, 0xFF);
    h.write_reg(RS_PRB, 0x00);  // assert every PB bit low first.
    if (!omit_reset_bug) h.pulse_reset();
    return h.portb_out_now() == 0xFF;
}

// ---------------------------------------------------------------------
// PROP-ORDER: DSKDIREC (PB1) and DSKSTEP* (PB0) share one PRB byte
// (opus:8-cia.c48, sol:8-cia.c42/c43) -- a value written to PB1 by one PRB
// write must survive two further writes that touch only PB0, since no
// single write can present one bit "before" the other; only a second,
// independent write can. `corrupt_third_write_bug`, when true, is the
// stimulus mutation: the third write (meant to restore PB0 high while
// leaving PB1 alone) also flips PB1.
// ---------------------------------------------------------------------
bool prop_order(Harness& h, bool corrupt_third_write_bug = false) {
    h.write_reg(RS_DDRB, 0xFF);
    h.write_reg(RS_PRB, 0xFD);  // PB1 (DIREC) = 0, PB0 (STEP) = 1.
    const uint8_t after_direc_write = h.portb_out_now();
    h.write_reg(RS_PRB, 0xFC);  // STEP-only write: PB0 -> 0, PB1 untouched.
    const uint8_t after_step_low = h.portb_out_now();
    h.write_reg(RS_PRB, corrupt_third_write_bug ? uint8_t{0xFF} : uint8_t{0xFD});
    const uint8_t after_step_high = h.portb_out_now();

    const auto direc_bit = [](uint8_t v) { return (v >> 1) & 1u; };
    return direc_bit(after_direc_write) == 0 &&
           direc_bit(after_step_low) == 0 &&
           direc_bit(after_step_high) == 0;
}

// ---------------------------------------------------------------------
// PROP-STEP-SHAPE: DSKSTEP* (PB0) must show high, momentarily low, then
// high across three successive PRB writes (opus:8-cia.c50, sol:8-cia.c45).
// `no_dip_bug`, when true, is the stimulus mutation: the middle write never
// actually drives PB0 low.
// ---------------------------------------------------------------------
bool prop_step_shape(Harness& h, bool no_dip_bug = false) {
    h.write_reg(RS_DDRB, 0xFF);
    h.write_reg(RS_PRB, 0xFF);
    const uint8_t s1 = h.portb_out_now();
    h.write_reg(RS_PRB, no_dip_bug ? uint8_t{0xFF} : uint8_t{0xFE});
    const uint8_t s2 = h.portb_out_now();
    h.write_reg(RS_PRB, 0xFF);
    const uint8_t s3 = h.portb_out_now();
    return (s1 & 1u) == 1 && (s2 & 1u) == 0 && (s3 & 1u) == 1;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    {
        Harness h;
        checks.expect(fixture_live(h),
            "FIXTURE-LIVE: two distinct, known PRB bytes (0xA5, 0x5A), with "
            "DDRB=0xFF, both arrive and read back correctly and distinctly "
            "on portb_out before any data-dependent assertion is graded "
            "[no page -- infrastructure]");
    }

    {
        Harness h;
        std::string detail;
        const bool result = prop_position(h, detail);
        checks.expect(result,
            "opus:8-cia.c4 sol:8-cia.c3 PROP-POSITION: CIABPRB's eight PB "
            "bits are independently writable AND readable output bits -- "
            "graded across all eight positions (PB0-PB7): clearing one bit "
            "must move only that bit of portb_out, restoring it must return "
            "the whole byte to 0xFF, and a CPU read of PRB must mirror "
            "portb_out exactly (0x3C written and read back). Not graded: "
            "which drive signal (DSKMOTOR* etc.) each position is named for "
            "-- that binding is minimig.v:650, one level above this "
            "boundary [p.244 \"Bits labeled PB are output bits located in "
            "CIABPRB ($BFD100)\"]");
        checks.note("PROP-POSITION per-bit independence: " + detail);
    }

    {
        Harness h;
        checks.expect(prop_reset_deassert(h),
            "opus:8-cia.c22 opus:8-cia.c33 opus:8-cia.c35 opus:8-cia.c37 "
            "opus:8-cia.c40 opus:8-cia.c29 sol:8-cia.c27 "
            "PROP-RESET-DEASSERT: PB7 (DSKMOTOR*), PB6-PB3 (DSKSEL3*..0*) "
            "and, byte-wide, every PB bit read HIGH -- deasserted -- "
            "immediately after reset, even though PRB was written all-low "
            "just before the reset pulse. This is the CIA-side precondition "
            "for 'all drive motors turn off after system reset' and for "
            "each active-low bit's default being its OFF state. Not graded: "
            "that LOW is the physically asserted level for a specific drive "
            "line (paula_floppy.v, out of this boundary), or that each "
            "drive individually latches and remembers motor state at its "
            "own select edge (drive-external, unmodelled here) [p.244 "
            "\"DSKMOTOR* ... active low\", \"All drive motors turn off "
            "after system reset\"; p.245 \"DSKSEL3*/DSKSEL2*/DSKSEL1*/"
            "DSKSEL0* ... active low\"]");
    }

    {
        Harness h;
        checks.expect(prop_order(h),
            "opus:8-cia.c48 sol:8-cia.c42 sol:8-cia.c43 PROP-ORDER: DSKDIREC "
            "(PB1) and DSKSTEP* (PB0) are bits of the same PRB byte, so no "
            "single write can present one before the other -- a value "
            "written to PB1 by one PRB write must survive two further "
            "writes that touch only PB0. Measured: PRB<-0xFD (DIREC=0, "
            "STEP=1), then PRB<-0xFC (STEP=0, DIREC untouched), then "
            "PRB<-0xFD (STEP=1, DIREC untouched) -- DIREC reads 0 at all "
            "three points [p.245 \"This line must be set up before the "
            "actual step pulse, with a separate write to the register\"]");
    }

    {
        Harness h;
        checks.expect(prop_step_shape(h),
            "opus:8-cia.c50 sol:8-cia.c45 PROP-STEP-SHAPE: DSKSTEP* (PB0) "
            "shows high, momentarily low, then high across three successive "
            "PRB writes (0xFF, 0xFE, 0xFF) -- a level merely held low is a "
            "different, unclaimed shape. Not graded: how short "
            "'momentarily' is -- a duration with no companion interval on "
            "this page, left to lot c4's drive-timing package [p.245 "
            "\"This signal must always be used as a quick pulse (high, "
            "momentarily low, then high)\"]");
    }

    { Harness h; checks.expect(
        bit_transfers_independently(h, /*bit=*/0),
        "CONTROL-BIT0: the same bit_transfers_independently() comparator "
        "PROP-POSITION sweeps above, instantiated at bit 0 alone -- the "
        "measurement-basis control every graded assertion in this package "
        "is implicitly compared against. Not itself a page claim [no page "
        "-- infrastructure control]"); }

    // ------------------------------------------------------------------
    // Declined claims. Each names a proposition this package does not
    // score, and the single reason -- per SPECIFICATION.md's "Claims not
    // assigned an assertion". No uid below appears on a graded (expect())
    // line.
    // ------------------------------------------------------------------
    checks.record(
        "opus:8-cia.c21 (p.244, \"PB7 DSKMOTOR* ... Disk motor control\"): "
        "names PB7's drive-signal binding. `ciab` alone has no signal "
        "named DSKMOTOR*; the binding of PB7 to that name is minimig.v:650, "
        "one level above this boundary. PB7's bit-position transfer is "
        "still exercised as one of PROP-POSITION's eight data points -- "
        "that is a different claim (byte-wide position independence) and "
        "does not re-grade this one under a different label. Not graded "
        "here.");
    checks.record(
        "opus:8-cia.c32 (p.245, \"PB6 DSKSEL3* ... Select drive 3\"): same "
        "reason as opus:8-cia.c21, for PB6/DSKSEL3*. Not graded here.");
    checks.record(
        "opus:8-cia.c34 (p.245, \"PB5 DSKSEL2* ... Select drive 2\"): same "
        "reason, for PB5/DSKSEL2*. Not graded here.");
    checks.record(
        "opus:8-cia.c36 (p.245, \"PB4 DSKSEL1* ... Select drive 1\"): same "
        "reason, for PB4/DSKSEL1*. Not graded here.");
    checks.record(
        "opus:8-cia.c38 (p.245, \"PB3 DSKSEL0* ... Select drive 0\"): same "
        "reason, for PB3/DSKSEL0*. Not graded here.");
    checks.record(
        "opus:8-cia.c41 (p.245, \"PB2 DSKSIDE ... which disk head to "
        "use\"): same reason, for PB2/DSKSIDE -- and DSKSIDE's MEANING (0 = "
        "upper head) is additionally a paula_floppy.v:669 fact "
        "(`track={dsktrack[sel],~side}`), also out of this boundary. Not "
        "graded here.");
    checks.record(
        "opus:8-cia.c45 (p.245, \"PB1 DSKDIREC ... direction to seek the "
        "heads\"): same reason, for PB1/DSKDIREC -- and DSKDIREC's MEANING "
        "(0 = towards center) is additionally a paula_floppy.v:673-677 "
        "fact. The register-structure half of this bit (that it is bit 1 "
        "of the same PRB byte as STEP, and survives a STEP-only write) IS "
        "graded, as opus:8-cia.c48/sol:8-cia.c42/sol:8-cia.c43's "
        "PROP-ORDER. Not graded here.");
    checks.record(
        "opus:8-cia.c49 (p.245, \"PB0 DSKSTEP* ... Step the heads\"): same "
        "reason, for PB0/DSKSTEP*. Not graded here.");
    checks.record(
        "sol:8-cia.c20 (p.244, composed: PB7 position + DSKMOTOR* name + "
        "active-low polarity): the polarity content is not lost -- it is "
        "the identical reset/pull-up measurement already graded above "
        "under its twin opus:8-cia.c22, in PROP-RESET-DEASSERT. Not cited "
        "there under this uid as well, because a uid cited on a graded "
        "line is counted asserted for the whole claim it names, including "
        "the DSKMOTOR* name-binding half this package does not test. Not "
        "graded here.");
    checks.record(
        "sol:8-cia.c30 (p.245, composed: PB6 + DSKSEL3* + active-low): same "
        "reason as sol:8-cia.c20; polarity half graded under "
        "opus:8-cia.c33. Not graded here.");
    checks.record(
        "sol:8-cia.c31 (p.245, composed: PB5 + DSKSEL2* + active-low): same "
        "reason; polarity half graded under opus:8-cia.c35. Not graded "
        "here.");
    checks.record(
        "sol:8-cia.c32 (p.245, composed: PB4 + DSKSEL1* + active-low): same "
        "reason; polarity half graded under opus:8-cia.c37. Not graded "
        "here.");
    checks.record(
        "sol:8-cia.c33 (p.245, composed: PB3 + DSKSEL0* + active-low): same "
        "reason; polarity half graded under opus:8-cia.c40. Not graded "
        "here.");
    checks.record(
        "sol:8-cia.c35 (p.245, \"PB2 DSKSIDE ... which disk head to use\", "
        "twin of opus:8-cia.c41): same reason as opus:8-cia.c41. Not "
        "graded here.");
    checks.record(
        "sol:8-cia.c39 (p.245, \"PB1 DSKDIREC ... direction to seek the "
        "heads\", twin of opus:8-cia.c45): same reason as opus:8-cia.c45; "
        "register-structure half graded as PROP-ORDER. Not graded here.");
    checks.record(
        "sol:8-cia.c44 (p.245, \"PB0 DSKSTEP* ... Step the heads\", twin of "
        "opus:8-cia.c49): same reason as opus:8-cia.c49. Not graded here.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
