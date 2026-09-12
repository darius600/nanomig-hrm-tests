// Test purpose: Assert the register-level half of the audio low-pass
//   filter's bypass control - that CIA-A PRA bit 1, the same output bit that
//   drives the red "power" LED, exists, is independently writable and reads
//   back - at the ciaa module boundary. This does NOT and CANNOT assert any
//   audio consequence of that bit (see Non-goals).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, chapter 5,
//   printed p.157.
// Non-goals: THERE IS NO FILTER ANYWHERE IN NANOMIG'S AUDIO PATH, AND THAT IS
//   WHY THIS PACKAGE STOPS AT THE REGISTER BIT. A grep over all five audio
//   modules (paula_audio.v, paula_audio_channel.v, paula_audio_volume.v,
//   paula_audio_mixer.v, paula_intcontroller.v) and paula.v itself returns
//   only sigma-delta modulator comments, no low-pass filter, no cutoff
//   constant, nothing this bit could plausibly gate. So the only thing
//   testable at either end of this claim is the CIA-A side: that the bit
//   the manual names ($BFE001 bit 1, /LED) is a real, independently
//   addressed, writable and readable register bit on ciaa. Whether writing
//   it changes anything about the audio signal is NOT asserted here,
//   because there is nothing downstream in this design to observe - a
//   claim about "the filter engages/bypasses" would have no fixture that
//   could ever go anything but vacuously green or vacuously red. Per
//   BOUNDARIES.md's framing for this chapter, this package is measured
//   entirely at ciaa and does not reach into minimig.v's _led/pwr_led wiring
//   or any audio module.
//
//   CONFLICT 6, recorded and NOT resolved (out of scope for a register-bit
//   fixture; specialists' question, not this package's): the manual gives
//   the filter's cutoff frequency three different values - Figure 5-5's
//   label "about 5 kHz", printed p.156's "becomes active around 4 KHz", and
//   printed p.158's "the Amiga's 7 KHz cut-off filter". None of the three is
//   chosen as an oracle here; all three are simply recorded, because there
//   is no filter cutoff signal anywhere in this design to measure any of
//   them against.
//
// RTL read before this spec was written: ciaa.v (388 lines, read in full,
//   including its four instantiated submodules cia_int.v, cia_timera.v,
//   cia_timerb.v, cia_timerd.v needed only so Verilator can elaborate the
//   top module) and this chapter's BOUNDARIES.md/CANDIDATES.md - per
//   BRIEF.md's own note that the chapter's literal "write the spec, commit
//   it, then open the RTL" order cannot be met here. What the rule protects
//   is still honoured: the expected values below (bit 1 of PRA, at register
//   select 0, following data_in bit 1) come from the chapter's printed
//   p.157 quotation, not from tuning to whatever ciaa.v happens to do.
//
// Claims covered (work/ch5_packages/ch5-audio-filter-bypass.jsonl, both):
//   CIA-A-PRA-BIT1-WRITABLE  : 5-p157.c8, 5-filt.c9
//   CIA-A-PRA-BIT1-READS-BACK: 5-p157.c8, 5-filt.c9
//   (Both ids quote the identical sentence from the two independent readers
//   who found it; there is exactly one proposition here, restated once as
//   "writable" and once as "reads back" because those are two separable,
//   separately falsifiable facts about the same bit.)
//
// FIXTURE. New harness for this package, following the write_reg/tick
// pattern already established in hrm/agnus_audiodma and hrm/agnus_blitter_*
// (both read before this file was written): reset, then one clk7_en tick to
// latch a register write (ciaa.v's regporta/ddrporta are posedge-clk,
// clk7_en-gated - ciaa.v:227-239), while reads (pa_out, ciaa.v:251-259, and
// porta_out, ciaa.v:262, a continuous assign) are purely combinational and
// need only eval(), not another clock edge. TEST-DATA-DELIVERY below proves
// this fixture's write/read path actually carries a distinct value through
// ciaa.v before either PRA-bit assertion is trusted, per BRIEF.md's fixture-
// liveness requirement.
//
// Every mutation below was actually applied (edit -> rebuild -> run ->
// revert -> rebuild-clean) and the FAIL/PASS lines quoted are what the run
// printed, not predictions.
//
// MUTATION TEST-DATA-DELIVERY stimulus red: the sentinel written to DDRA
//   changed from 0xA5 to 0xA6 (the expected 0xA5 comparison left untouched)
//   -> FAIL, read-back 0xA6 != expected 0xA5; every other assertion PASS.
// MUTATION CIA-A-PRA-BIT1-WRITABLE stimulus red: the PRA "bit high" write
//   changed from 0b00000010 to 0b00000000 (the expected ==1 comparison left
//   untouched) -> FAIL, porta_out bit 1 reads 0, not the expected 1; every
//   other assertion PASS.
// MUTATION CIA-A-PRA-BIT1-READS-BACK stimulus red: the PRA "bit high" write
//   changed from 0b00000010 to 0b00000000 (the expected ==1 comparison left
//   untouched) -> FAIL, the PRA read-back bit 1 reads 0, not the expected 1;
//   every other assertion PASS.

#include <cstdint>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vciaa.h"

namespace {

constexpr uint8_t RS_PRA = 0x0;
constexpr uint8_t RS_DDRA = 0x2;

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    int passed = 0;
    int failed = 0;
};

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
        for (int i = 0; i < 8; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    // One register write, latched on the next posedge clk while clk7_en is
    // high (ciaa.v's regporta/ddrporta always-blocks, e.g. ciaa.v:233-239).
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

    // Combinational read: pa_out (ciaa.v:251-259) is an always @(*) block
    // driven by rd/wr/rs alone, no clock edge required - settle with
    // eval() only, the same discipline agnus_blitter_masks's Harness
    // documents for its own combinational reads.
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

    // porta_out (ciaa.v:262) is a continuous assign off ddrporta/regporta,
    // valid immediately after the write that changed them settles.
    uint8_t porta_out_now() {
        dut->eval();
        return dut->porta_out;
    }

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    Vciaa* dut;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- TEST-DATA-DELIVERY (fixture self-check, no page) ----------------
    {
        Harness h;
        h.write_reg(RS_DDRA, 0xA5);
        const uint8_t back = h.read_reg(RS_DDRA);
        checks.expect(back == 0xA5,
            "[fixture check] TEST-DATA-DELIVERY: a distinct sentinel byte (0xA5) "
            "written to DDRA and read back unchanged proves this fixture's register "
            "write/read path actually carries data through ciaa.v, which the "
            "CIA-A-PRA-BIT1 assertions below depend on.");
    }

    // ---------------- CIA-A-PRA-BIT1-WRITABLE (chapter 5, p.157) ----------------
    {
        // DDRA bit 1 configured as output only (all other DDRA bits left at
        // their reset default 0, i.e. input/pulled-up) so porta_out[1] =
        // ~ddrporta[1] | regporta[1] collapses to exactly regporta[1]
        // (ciaa.v:262), with no pull-up ambiguity confounding this one bit.
        Harness h;
        h.write_reg(RS_DDRA, 0b00000010);
        h.write_reg(RS_PRA, 0b00000010);  // bit 1 = 1, all else 0
        const uint8_t out_hi = h.porta_out_now();
        h.write_reg(RS_PRA, 0b00000000);  // bit 1 = 0
        const uint8_t out_lo = h.porta_out_now();
        checks.expect(((out_hi >> 1) & 1) == 1 && ((out_lo >> 1) & 1) == 0,
            "CIA-A-PRA-BIT1-WRITABLE: writing CIA-A PRA (register select 0, the "
            "register the memory map places at $BFE001) bit 1 to 1, then to 0, "
            "drives porta_out bit 1 high then low under software control - 'This "
            "control bit is the same output bit of the 8520 CIA that controls the "
            "brightness of the red \"power\" LED (CIA A $BFE001 - Bit 1: /LED)' "
            "(5-p157.c8, 5-filt.c9, chapter 5, printed p.157) - the identified bit "
            "is a real, independently writable output bit at this boundary; "
            "ciaa.v:262 `porta_out[3:0] = {(~ddrporta[7:6] | regporta[3:2]), "
            "(~ddrporta[1:0] | regporta[1:0])}` with regporta[1] loaded from "
            "data_in[1] (ciaa.v:238).");
    }

    // ---------------- CIA-A-PRA-BIT1-READS-BACK (chapter 5, p.157) ----------------
    {
        Harness h;
        h.write_reg(RS_DDRA, 0b00000010);
        h.write_reg(RS_PRA, 0b00000010);
        const uint8_t read_hi = h.read_reg(RS_PRA);
        h.write_reg(RS_PRA, 0b00000000);
        const uint8_t read_lo = h.read_reg(RS_PRA);
        checks.expect(((read_hi >> 1) & 1) == 1 && ((read_lo >> 1) & 1) == 0,
            "CIA-A-PRA-BIT1-READS-BACK: reading CIA-A PRA back after writing bit 1 "
            "to 1, then again after writing it to 0, returns bit 1 = 1 and then "
            "bit 1 = 0 - the same 'CIA A $BFE001 - Bit 1: /LED' bit (5-p157.c8, "
            "5-filt.c9, chapter 5, printed p.157) is not write-only: software can "
            "observe the value it set, over the same PRA register - ciaa.v:253-254 "
            "`pa_out[7:0] = {porta_in2[7:2],porta_out[1:0]}` on a PRA read.");
    }

    std::cout << "Checked " << (checks.passed + checks.failed)
              << " assertions; passed=" << checks.passed
              << " failed=" << checks.failed << ".\n";

    return checks.failed == 0 ? 0 : 1;
}
