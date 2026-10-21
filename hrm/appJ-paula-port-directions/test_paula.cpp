// Test purpose: grade printed page 380's PAULA PIN ASSIGNMENT table, the
//   DEFINITION column (direction: I / O / I/O) against the `paula` module
//   compiled alone with its own submodules (paula_uart, paula_intcontroller,
//   paula_floppy(+fifo), paula_audio(+channel/mixer/volume)). 30 claims (15
//   propositions, each claimed independently by readers sol and opus as
//   twins), filtered from
//   hrm-testpipeline/work/2026-09-17_appHJ_packages.jsonl on
//   classify.package == "appJ-paula-port-directions".
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 380, PAULA PIN ASSIGNMENT table.
// Fixture boundary: `paula` (paula.v) and every module it instantiates --
//   confirmed by this file's own Makefile SRC list, the only place a module
//   could be added. Modelled on hrm/ch8-appb-chip-column-paula, which already
//   builds this exact boundary; its write_reg/read_reg/do_reset register-bus
//   harness is reused close to verbatim below (disclosed inline).
// Non-goals: anything outside the `paula` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/2026-09-17_appHJ_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// THE AXIS. Per the build brief: the DEFINITION column is the direction of
//   the signal AT THE CHIP, and this is the SAME axis hrm/appDE-ciab-port-
//   signals grades for the CIA -- its own expect() label reads "at ciab's
//   own boundary, PA7 and PA6 are both DRIVEN OUTPUT bits -- `porta_out` is
//   declared `output [7:6]` (ciab.v:20)" and calls it "a Verilog
//   port-direction fact, not a runtime-configurable one." As there, every
//   proposition below is graded on the pin-to-PORT identification, argued
//   from RTL declaration and wiring; the printed PIN NUMBER / PIN RANGE is
//   explicitly NOT part of any claim graded here (classify.note says so for
//   every claim that maps to a split or merged pin range).
//
// CORPUS DEFECT, PAULA BLOCK -- read from the render, not the text layer,
//   per the build brief. p380.txt emits the PAULA table's own column
//   headings as a fake first data row, "01-07 DESIGNATION ... DEFINITION" --
//   there is no such pin and no claim exists for it. figures/p380.png was
//   opened directly (not just the corpus-defects.md note) and its PAULA PIN
//   ASSIGNMENT block reads cleanly, in order, pin 01 through 42-48, with
//   every DEFINITION cell a plain "I", "O" or "I/O" -- no digit/letter
//   confusion in any cell this package grades (checked cell by cell against
//   the render, not assumed from the note; see the O/0 note below). The
//   DENISE block above it on the same page is separately confirmed NOT
//   displaced and is not this package's concern.
//
// THE O/0 DAMAGE DIRECTION, CHECKED AND FOUND NOT TO TOUCH THIS PACKAGE. The
//   build brief warns that the DEFINITION column loses the letter O to the
//   digit 0 elsewhere on this page (the DENISE block's row 18, "BURST*
//   Color burst O" misread as "...0", making a real output look like a
//   stray zero). That row is on the DENISE table, not PAULA, and BURST* is
//   not one of this package's 15 propositions regardless. Every DEFINITION
//   cell this package actually grades was read directly off figures/p380.png
//   as a plain letter (I/O), not inferred from the text layer or the note.
//
// TWO PRINTED THINGS ON THIS PAGE, RECORDED AND NOT RESOLVED, NEITHER OF
//   WHICH BINDS ANY CLAIM GRADED BELOW:
//   (1) PIN 13-15 "IPL0*-IPL2" -- the range's FIRST endpoint (IPL0) carries
//   the active-low asterisk and the LAST (IPL2) does not. Both readers
//   carried this verbatim (classify.note on sol:J-090/opus:AHJ-223). The
//   DIRECTION ("O") is graded below; the asterisk placement is not, and no
//   assertion here depends on which endpoint is active-low.
//   (2) Appendix J's own self-contradiction between its two Agnus tables
//   (LP*, CCK, CCKQ reversing between p.379 and p.381) does not reach this
//   package -- Paula's own CCK row (28, "CCK Color clock I") is not one of
//   the contradicting pair (that is between the two AGNUS printings only)
//   and nothing here cites p.381.
//
// A PORT-DIRECTION FACT IS COMPILE-TIME, NOT RUNTIME (per the build brief,
//   quoting hrm/ch8-video-signal-set's own record(): a port declaration is
//   "a structural, compile-time fact no runtime stimulus can change"). None
//   of `paula`'s ports graded here have a DDR-style register that lets the
//   SAME physical wire act as input or output depending on some other
//   register's content -- unlike ciab's PA7/PA6 (which hrm/appDE-ciab-port-
//   signals mutates via DDRA), every port below is unconditionally input or
//   output by its own `input`/`output` keyword, for the whole life of the
//   simulation. So no STIMULUS this harness can apply ever makes an input
//   port behave like an output or vice versa -- there is nothing for a
//   stimulus mutation to flip. Every proposition below is therefore recorded
//   `none impossible`, with the argument, PAIRED WITH A POSITIVE CONTROL
//   proving the port is genuinely live in this fixture (not a dead/
//   unconnected wire the direction claim happens to describe correctly by
//   accident) -- exactly the worked pattern in hrm/appDE-fire-buttons-userio
//   (its own header: "Recorded below as `none impossible` for every graded
//   uid... together with a positive-control measurement for each excluded
//   signal, proving it moves something else real in this fixture").
//
// RTL READ IN FULL before this file was drafted: paula.v (all ports and the
//   DMACON/ADKCON/reset/cck wiring), paula_intcontroller.v (INTENA/INTREQ/
//   INTENAR/INTREQR decode and the IPL priority encoder), paula_floppy.v
//   (DSKLEN arm sequence and the disk_dmal/dmaon combinational logic),
//   paula_audio.v and paula_audio_channel.v (the CPU-driven, non-DMA
//   AUDxDAT playback path used below) and paula_audio_mixer.v (channel 0/3
//   -> ldata "AUDA", channel 1/2 -> rdata "AUDB"). Also read in full:
//   hrm/ch8-appb-chip-column-paula/test_paula.cpp (this package's harness
//   model) and hrm/appDE-fire-buttons-userio/test_userio.cpp (the
//   none-impossible + positive-control worked example this build brief
//   names).
//
// REGISTER ADDRESSES (word index = byte address >> 1, confirmed against
//   paula.v's own `reg_address_in[8:1]==<PARAM>[8:1]` decode, same
//   convention hrm/ch8-appb-chip-column-paula already uses):
//   ADKCON=0x4F(w) $09E, ADKCONR=0x08(r) $010 (paula.v:131-132)
//   DMACON=0x4B(w) $096 (paula.v:130)
//   INTENA=0x4D(w) $09A, INTENAR=0x0E(r) $01C,
//   INTREQ=0x4E(w) $09C, INTREQR=0x0F(r) $01E (paula_intcontroller.v:27-30)
//   DSKLEN=0x12(w) $024 (paula_floppy.v:124)
//   SERDAT=0x18(w) $030, SERDATR=0x0C(r) $018, SERPER=0x19(w) $032
//     (paula_uart.v:26-28, values reused from ch8-appb-chip-column-paula)
//   AUD0VOL=0x54, AUD0DAT=0x55 (base $0A0>>1=0x50, +4/+5 word;
//     paula_audio.v:131, paula_audio_channel.v:24-26)
//   AUD1VOL=0x5C, AUD1DAT=0x5D (base $0B0>>1=0x58, +4/+5 word)
//
// MUTATION METHODOLOGY. Every proposition below is `none impossible` (see
//   above) with its own positive control, run for real (WSL Verilator,
//   `make test`) and its MEASURED output read back into this file's RUN
//   lines, not asserted from expectation. No compile-time #ifdef mutation
//   variants exist in this file -- there is no MUT= build for this package,
//   because nothing here claims a stimulus CAN flip these assertions; that
//   is the whole content of `none impossible`.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include "Vpaula.h"
#include "verilated.h"

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

namespace {
std::string hex4(uint16_t v) {
    char b[8];
    std::snprintf(b, sizeof b, "%04X", v);
    return std::string(b);
}
}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vpaula* dut = new Vpaula;
    Checks checks;

    int clk7_cycle = 0;
    // External serial loopback, wired only in this testbench, exactly as
    // hrm/ch8-appb-chip-column-paula's own tick() does -- harmless to every
    // non-serial assertion (idle txd=1 keeps rxd idle=1 too).
    auto tick = [&]() {
        dut->rxd = dut->txd;
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        clk7_cycle = (clk7_cycle + 1) % 4;
        dut->clk7_en  = (clk7_cycle == 0);
        dut->clk7n_en = (clk7_cycle == 2);
    };

    // do_reset leaves cck at 0 (as hrm/ch8-appb-chip-column-paula's own
    // do_reset does); sections that need paula_audio's cck-gated state to
    // advance set dut->cck explicitly afterward, and say so.
    auto do_reset = [&]() {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->clk7n_en = 0;
        dut->cck = 0;
        dut->reset = 1;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->rxd = 1;
        dut->ntsc = 0;
        dut->sof = 0;
        dut->strhor = 0;
        dut->vblint = 0;
        dut->int2 = 0;
        dut->int3 = 0;
        dut->int6 = 0;
        dut->_step = 1;
        dut->direc = 0;
        dut->_sel = 0xF;
        dut->side = 0;
        dut->_motor = 1;
        dut->sdc_image_mounted = 0;
        dut->sdc_image_size = 0;
        dut->sdc_busy = 0;
        dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0;
        dut->sdc_byte_addr = 0;
        dut->sdc_byte_in_data = 0;
        dut->floppy_drives = 0;
        dut->floppy_wrprot = 0;
        for (int i = 0; i < 16; i++) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; i++) tick();
    };

    auto write_reg = [&](int addr, uint16_t data) {
        while (!dut->clk7_en) tick();
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick(); tick(); tick(); tick();
        dut->reg_address_in = 0;
        dut->data_in = 0;
    };

    auto read_reg = [&](int addr) -> uint16_t {
        while (!dut->clk7_en) tick();
        dut->reg_address_in = addr;
        tick();
        uint16_t val = dut->data_out;
        tick(); tick(); tick();
        dut->reg_address_in = 0;
        return val;
    };

    const int ADKCON  = 0x4F, ADKCONR = 0x08;
    const int DMACON  = 0x4B;
    const int INTENA  = 0x4D, INTENAR = 0x0E;
    const int INTREQ  = 0x4E, INTREQR = 0x0F;
    const int DSKLEN  = 0x12;
    const int SERDAT  = 0x18, SERDATR = 0x0C, SERPER = 0x19;
    const int AUD0VOL = 0x54, AUD0DAT = 0x55;
    const int AUD1VOL = 0x5C, AUD1DAT = 0x5D;
    const uint16_t SET = 0x8000;
    const uint16_t CLR = 0x0000;

    // ========================================================================
    // D-BUS POSITIVE CONTROL, shared by the three printed D-bus rows below
    // (01-07 D8-D2, 09-10 D1-D0, 42-48 D15-D9 -- all three land on the SAME
    // pair of RTL ports, data_in[15:0]/data_out[15:0], paula.v:67-68; the
    // printed pin-range split has no RTL counterpart -- classify.note on
    // each of the three claim pairs). Two distinct known words written
    // through data_in and read back through data_out via ADKCON/ADKCONR,
    // exactly as hrm/ch8-appb-chip-column-paula's own A0 liveness does.
    // ========================================================================
    bool bus_live;
    {
        do_reset();
        write_reg(ADKCON, CLR | 0x7FFF);
        write_reg(ADKCON, SET | 0x0055);
        uint16_t a1 = read_reg(ADKCONR);
        write_reg(ADKCON, CLR | 0x0055);
        write_reg(ADKCON, SET | 0x00AA);
        uint16_t a2 = read_reg(ADKCONR);
        checks.note("D-BUS-LIVE: 0x0055 written through data_in arrives at ADKCONR (0x" +
                    hex4(a1) + "), a second distinct word 0x00AA also arrives (0x" + hex4(a2) +
                    ") read back through data_out -- both directions of the pair carry real, "
                    "distinct content, not a stuck value.");
        bus_live = (a1 == 0x0055) && (a2 == 0x00AA);
        checks.note(std::string("D-BUS-FIXTURE-LIVE: ") + (bus_live ? "live" : "NOT LIVE -- see MEASURED above") + " [no page -- infrastructure, not itself a claim]");
    }

    // MUTATION PAULA-D8-D2-DIRECTION none impossible: D8-D2 (printed pin 01-07) identifies to paula.v:68's `data_in[15:0]` (input) and paula.v:69's `data_out[15:0]` (output), both declared unconditionally -- no DDR-style register lets either behave as the other, so no stimulus this harness applies can flip which one is which. Positive control: D-BUS-LIVE above, two distinct words written through data_in and read back through data_out.
    checks.expect(bus_live,
        "PAULA-D8-D2-DIRECTION: at paula's own boundary, the D8-D2 half of the data bus "
        "identifies to `data_in[15:0]` (input, paula.v:68) and `data_out[15:0]` (output, "
        "paula.v:69), both connected at minimig.v:501-502 -- a Verilog port-direction fact, "
        "not a runtime-configurable one, per the axis hrm/appDE-ciab-port-signals grades for "
        "the CIA. The printed pin range (01-07) is not graded; the RTL splits the one printed "
        "bidirectional bus into two unidirectional ports with no pin-range counterpart. "
        "[p.380 PAULA PIN ASSIGNMENT, DEFINITION column: D8-D2 \"I/O\"] sol:J-080 opus:AHJ-213");

    // MUTATION PAULA-D1-D0-DIRECTION none impossible: same pair of ports as D8-D2 (paula.v:68-69) -- printed pin 09-10 identifies to the SAME data_in/data_out, no DDR-style register exists to flip either direction under stimulus. Positive control: D-BUS-LIVE above.
    checks.expect(bus_live,
        "PAULA-D1-D0-DIRECTION: at paula's own boundary, the D1-D0 half of the data bus "
        "identifies to the SAME `data_in[15:0]`/`data_out[15:0]` pair as D8-D2 and D15-D9 "
        "(paula.v:68-69) -- a Verilog port-direction fact. The printed pin range (09-10) is "
        "not graded. [p.380 PAULA PIN ASSIGNMENT, DEFINITION column: D1-D0 \"I/O\"] "
        "sol:J-084 opus:AHJ-217");

    // MUTATION PAULA-D15-D9-DIRECTION none impossible: same pair of ports as D8-D2/D1-D0 (paula.v:68-69) -- printed pin 42-48 identifies to the SAME data_in/data_out. Positive control: D-BUS-LIVE above.
    checks.expect(bus_live,
        "PAULA-D15-D9-DIRECTION: at paula's own boundary, the D15-D9 half of the data bus "
        "identifies to the SAME `data_in[15:0]`/`data_out[15:0]` pair as D8-D2 and D1-D0 "
        "(paula.v:68-69) -- a Verilog port-direction fact. The printed pin range (42-48) is "
        "not graded. [p.380 PAULA PIN ASSIGNMENT, DEFINITION column: D15-D9 \"I/O\"] "
        "sol:J-130 opus:AHJ-263");

    // ========================================================================
    // RGA8-RGA1 POSITIVE CONTROL: reg_address_in genuinely discriminates
    // between two different registers -- writing DMACON with a distinct
    // value does not corrupt an already-written ADKCON, and reading back
    // ADKCONR still returns exactly the ADKCON word.
    // ========================================================================
    bool rga_live;
    {
        do_reset();
        write_reg(ADKCON, CLR | 0x7FFF);
        write_reg(ADKCON, SET | 0x0033);
        write_reg(DMACON, CLR | 0x03FF);
        write_reg(DMACON, SET | 0x0210);
        uint16_t adk = read_reg(ADKCONR);
        checks.note("RGA-LIVE: after ADKCON<-0x0033 then a DIFFERENT register (DMACON<-0x0210) "
                    "is written at its own address, ADKCONR still reads 0x" + hex4(adk) +
                    " -- reg_address_in genuinely selects which register a write reaches.");
        rga_live = (adk == 0x0033);
        checks.note(std::string("RGA-FIXTURE-LIVE: ") + (rga_live ? "live" : "NOT LIVE -- see MEASURED above") + " [no page -- infrastructure, not itself a claim]");
    }

    // MUTATION PAULA-RGA-DIRECTION none impossible: RGA8-RGA1 identifies to paula.v:67's `reg_address_in[8:1]` (input), unconditionally declared -- no stimulus can make an address-bus input behave as an output. Positive control: RGA-LIVE above.
    checks.expect(rga_live,
        "PAULA-RGA-DIRECTION: at paula's own boundary, RGA8-RGA1 identifies to "
        "`reg_address_in[8:1]` (input, paula.v:67, 'register address inputs'), connected at "
        "minimig.v:500 from agnus's `reg_address_out` -- a Verilog port-direction fact. Not "
        "touched by appendix J's own AGNUS-table RGA contradiction (that split is between "
        "p.379 and p.381's two AGNUS printings, not this page's PAULA row). "
        "[p.380 PAULA PIN ASSIGNMENT, DEFINITION column: RGA8-RGA1 \"I\"] "
        "sol:J-098 opus:AHJ-231");

    // ========================================================================
    // RES* POSITIVE CONTROL: a nonzero ADKCON is written, then reset is
    // asserted (do_reset() itself), and ADKCONR is confirmed to read back to
    // its all-zero reset value -- reset genuinely reaches and clears
    // paula's internal state.
    // ========================================================================
    bool reset_live;
    {
        do_reset();
        write_reg(ADKCON, CLR | 0x7FFF);
        write_reg(ADKCON, SET | 0x7FFF);
        uint16_t before = read_reg(ADKCONR);
        do_reset();  // asserts dut->reset for 16 ticks, exactly as every section above
        uint16_t after = read_reg(ADKCONR);
        checks.note("RESET-LIVE: ADKCON=0x7FFF gives ADKCONR=0x" + hex4(before) +
                    " before reset; after `reset` is asserted, ADKCONR reads 0x" + hex4(after) +
                    " -- the reset input genuinely clears internal register state.");
        reset_live = (before != 0) && (after == 0);
        checks.note(std::string("RESET-FIXTURE-LIVE: ") + (reset_live ? "live" : "NOT LIVE -- see MEASURED above") + " [no page -- infrastructure, not itself a claim]");
    }

    // MUTATION PAULA-RES-DIRECTION none impossible: RES* identifies to paula.v:66's `reset` (input), unconditionally declared -- no stimulus can make a reset input behave as an output. Positive control: RESET-LIVE above.
    checks.expect(reset_live,
        "PAULA-RES-DIRECTION: at paula's own boundary, RES* identifies to `reset` (input, "
        "paula.v:66), driven at minimig.v:499 -- a Verilog port-direction fact. POLARITY NOT "
        "GRADED: the page prints RES* (active low by its own NOTE) and paula.v's `reset` is "
        "active high; that is a different axis, not asserted here. "
        "[p.380 PAULA PIN ASSIGNMENT, DEFINITION column: RES* \"I\"] "
        "sol:J-086 opus:AHJ-219");

    // ========================================================================
    // DMAL POSITIVE CONTROL: disk DMA is armed for a WRITE-direction
    // transfer (DMACON's master+DSKEN bits, then DSKLEN written twice in a
    // row with its top bit set -- paula_floppy.v:730-743's own "start disk
    // dma if second write in a row with dsklen[15] set"). Once dskstate
    // reaches DISKDMA_ACTIVE, `dmaon = ~lenzero | ~dsklen[14]` and, for a
    // write (dsklen[14]=1) with a fresh, empty fifo (~fifo_full), `dmal =
    // dmaon & (dsklen[14] & ~fifo_full)` -- asserted without needing any SD
    // card interaction (paula_floppy.v:950).
    // ========================================================================
    bool dmal_live;
    {
        do_reset();
        write_reg(DMACON, CLR | 0x03FF);
        write_reg(DMACON, SET | 0x0210);   // bit9 DMAEN=1, bit4 DSKEN=1
        bool before = dut->disk_dmal;
        write_reg(DSKLEN, 0xC001);         // bit15=1 arm, bit14=1 write dir, len=1
        write_reg(DSKLEN, 0xC001);         // second write in a row -> dmaen (floppy-local) <= 1
        bool after = false;
        for (int i = 0; i < 40 && !after; i++) { tick(); after = dut->disk_dmal; }
        checks.note(std::string("DMAL-LIVE: disk_dmal reads ") + (before ? "1" : "0") +
                    " before DMA is armed, and " + (after ? "1" : "0") +
                    " after DMACON (master+DSKEN) and two consecutive DSKLEN writes arm a "
                    "write-direction transfer -- the output genuinely moves in response to "
                    "real register writes, not a dead/tied pin.");
        dmal_live = (!before) && after;
        checks.note(std::string("DMAL-FIXTURE-LIVE: ") + (dmal_live ? "live" : "NOT LIVE -- see MEASURED above") + " [no page -- infrastructure, not itself a claim]");
    }

    // MUTATION PAULA-DMAL-DIRECTION none impossible: DMAL identifies to paula.v:89's `disk_dmal` (output), unconditionally declared -- no stimulus can make a DMA-request output behave as an input. Positive control: DMAL-LIVE above.
    checks.expect(dmal_live,
        "PAULA-DMAL-DIRECTION: at paula's own boundary, DMAL identifies to `disk_dmal` "
        "(output, paula.v:89, 'disk dma data transfer request (to Agnus)'), wired to agnus's "
        "matching input at minimig.v:521-524 -- a Verilog port-direction fact, and the exact "
        "complement of p.379's Agnus DMAL row (printed I there). NanoMig splits the one "
        "printed DMAL pin into five dedicated request wires (disk + 4 audio channels); the "
        "time-multiplexing of one physical pin is not graded, only the direction. "
        "[p.380 PAULA PIN ASSIGNMENT, DEFINITION column: DMAL \"O\"] "
        "sol:J-088 opus:AHJ-221");

    // ========================================================================
    // INTERRUPT-BUS POSITIVE CONTROL (shared groundwork for INT2*/INT3*/
    // INT6*/IPL0*-IPL2): INTENA's master bit (14) and the three bits int2
    // (3), int3 (6), int6 (13) route to (paula_intcontroller.v:103-129) are
    // all enabled once; each of the three inputs is then pulsed ALONE (a
    // fresh do_reset() between each) and its own INTREQR bit, plus the
    // shared `_ipl` output leaving its idle value 7, is measured.
    // ========================================================================
    bool int2_live, int3_live, int6_live;
    bool ipl_int2_moved, ipl_int3_moved, ipl_int6_moved;
    {
        do_reset();
        write_reg(INTENA, SET | 0x4000 | 0x0008);  // master(14) + int2's bit(3)
        uint16_t ipl_idle = dut->_ipl;
        dut->int2 = 1;
        for (int i = 0; i < 8; i++) tick();
        dut->int2 = 0;
        uint16_t req = read_reg(INTREQR);
        uint16_t ipl_now = dut->_ipl;
        checks.note("INT2-LIVE: INTENA=master+bit3, int2 pulsed -- INTREQR=0x" + hex4(req) +
                    " (bit3=" + std::to_string((req >> 3) & 1) + "), _ipl idle=" +
                    std::to_string(ipl_idle) + " now=" + std::to_string(ipl_now));
        int2_live = ((req >> 3) & 1) == 1;
        ipl_int2_moved = (ipl_idle == 7) && (ipl_now != 7);
    }
    {
        do_reset();
        write_reg(INTENA, SET | 0x4000 | 0x0040);  // master(14) + int3's bit(6)
        uint16_t ipl_idle = dut->_ipl;
        dut->int3 = 1;
        for (int i = 0; i < 8; i++) tick();
        dut->int3 = 0;
        uint16_t req = read_reg(INTREQR);
        uint16_t ipl_now = dut->_ipl;
        checks.note("INT3-LIVE: INTENA=master+bit6, int3 pulsed -- INTREQR=0x" + hex4(req) +
                    " (bit6=" + std::to_string((req >> 6) & 1) + "), _ipl idle=" +
                    std::to_string(ipl_idle) + " now=" + std::to_string(ipl_now));
        int3_live = ((req >> 6) & 1) == 1;
        ipl_int3_moved = (ipl_idle == 7) && (ipl_now != 7);
    }
    {
        do_reset();
        write_reg(INTENA, SET | 0x4000 | 0x2000);  // master(14) + int6's bit(13)
        uint16_t ipl_idle = dut->_ipl;
        dut->int6 = 1;
        for (int i = 0; i < 8; i++) tick();
        dut->int6 = 0;
        uint16_t req = read_reg(INTREQR);
        uint16_t ipl_now = dut->_ipl;
        checks.note("INT6-LIVE: INTENA=master+bit13, int6 pulsed -- INTREQR=0x" + hex4(req) +
                    " (bit13=" + std::to_string((req >> 13) & 1) + "), _ipl idle=" +
                    std::to_string(ipl_idle) + " now=" + std::to_string(ipl_now));
        int6_live = ((req >> 13) & 1) == 1;
        ipl_int6_moved = (ipl_idle == 7) && (ipl_now != 7);
    }
    checks.note(std::string("INTERRUPT-INPUTS-FIXTURE-LIVE: ") +
        ((int2_live && int3_live && int6_live) ? "live" : "NOT LIVE -- see MEASURED above") +
        " [no page -- infrastructure, not itself a claim]");
    bool ipl_live = ipl_int2_moved && ipl_int3_moved && ipl_int6_moved;
    checks.note(std::string("IPL-FIXTURE-LIVE: ") + (ipl_live ? "live" : "NOT LIVE -- see MEASURED above") + " [no page -- infrastructure, not itself a claim]");

    // MUTATION PAULA-INT2-DIRECTION none impossible: INT2* identifies to paula.v:83's `int2` (input), unconditionally declared -- no stimulus can make it behave as an output. Positive control: INT2-LIVE above (INTREQR bit3 set).
    checks.expect(int2_live,
        "PAULA-INT2-DIRECTION: at paula's own boundary, INT2* identifies to `int2` (input, "
        "paula.v:83, 'level 2 interrupt'), driven at minimig.v:516-519 -- a Verilog "
        "port-direction fact. hrm/appF-cia-pin-bindings' CIAA-IRQ-REACHES-INT2 grades that "
        "CIA-A's interrupt REACHES int2 at the `minimig` boundary, a routing proposition at a "
        "different boundary; cited, not declined against. "
        "[p.380 PAULA PIN ASSIGNMENT, DEFINITION column: INT2* \"I\"] "
        "sol:J-092 opus:AHJ-225");

    // MUTATION PAULA-INT3-DIRECTION none impossible: INT3* identifies to paula.v:84's `int3` (input), unconditionally declared. Positive control: INT3-LIVE above (INTREQR bit6 set).
    checks.expect(int3_live,
        "PAULA-INT3-DIRECTION: at paula's own boundary, INT3* identifies to `int3` (input, "
        "paula.v:84, 'level 3 interrupt'), driven at minimig.v:521 from agnus's `int3` output "
        "-- a Verilog port-direction fact, and the receiving end of p.379's Agnus INT3* row "
        "(printed O there; both ends graded, one per chip, at their own boundaries). "
        "[p.380 PAULA PIN ASSIGNMENT, DEFINITION column: INT3* \"I\"] "
        "sol:J-094 opus:AHJ-227");

    // MUTATION PAULA-INT6-DIRECTION none impossible: INT6* identifies to paula.v:85's `int6` (input), unconditionally declared. Positive control: INT6-LIVE above (INTREQR bit13 set).
    checks.expect(int6_live,
        "PAULA-INT6-DIRECTION: at paula's own boundary, INT6* identifies to `int6` (input, "
        "paula.v:85, 'level 6 interrupt'), driven at minimig.v:522 -- a Verilog port-direction "
        "fact. hrm/ch8-disk-index-interrupt-path's PROP-LEVEL6 traces an index pulse through "
        "to level 6, a path proposition at a different axis; cited, not declined against. "
        "[p.380 PAULA PIN ASSIGNMENT, DEFINITION column: INT6* \"I\"] "
        "sol:J-096 opus:AHJ-229");

    // MUTATION PAULA-IPL-DIRECTION none impossible: IPL0*-IPL2 identifies to paula.v:86's `_ipl[2:0]` (output), unconditionally declared -- no stimulus can make an interrupt-request output behave as an input. Positive control: IPL-FIXTURE-LIVE above (_ipl leaves its idle value 7 for each of int2/int3/int6, one at a time).
    checks.expect(ipl_live,
        "PAULA-IPL-DIRECTION: at paula's own boundary, IPL0*-IPL2 identifies to `_ipl[2:0]` "
        "(output, paula.v:86, 'm68k interrupt request'), connected at minimig.v:520 -- a "
        "Verilog port-direction fact. hrm/ch1-paula-what-it-contains already drives this exact "
        "channel, naming `_ipl` one of Paula's three output domains. PRINTED DEFECT RECORDED, "
        "NOT RESOLVED: the range's first endpoint (IPL0) carries the active-low asterisk and "
        "the last (IPL2) does not; the direction is graded, the asterisk placement is not. "
        "[p.380 PAULA PIN ASSIGNMENT, DEFINITION column: IPL0*-IPL2 \"O\"] "
        "sol:J-090 opus:AHJ-223");

    // ========================================================================
    // CCK POSITIVE CONTROL / DEFINITION: cck is a colour-clock ENABLE gating
    // paula_audio's own channel state machine (paula_audio_channel.v: every
    // register transition there is `if (clk7_en) ... if (cck) ...`). Held
    // low throughout a channel-0 write sequence, ldata must stay at its
    // reset baseline; released (held high, statically, for this liveness
    // demonstration -- see file header, a faster-than-real but functionally
    // valid cck waveform for this purpose), the SAME sequence moves ldata.
    // This doubles as this package's own CCK-DIRECTION positive control AND
    // (via ldata moving) is reused, unmodified, as the AUDA positive control
    // below -- rdata/AUDB gets its own channel-1 run.
    //
    // ORDER MATTERS AND IS DELIBERATE: the cck-high (released) run is
    // captured FIRST, the cck-held-low run SECOND. paula_audio_channel.v's
    // own `AUDxDAT` register (distinct from the combinational `datwrite`
    // pulse) has NO `if (reset)` clause at all -- it latches to 1 on a
    // register write and clears only `else if (cck)`. Measured directly:
    // running the held-low sequence FIRST leaves AUDxDAT stuck at 1 (cck
    // never pulses to clear it), and that residue survives a later
    // do_reset() (reset does not touch this register) and corrupts the
    // NEXT channel-0 run's very first state-machine transition, before its
    // own write ever lands. Capturing the released (ldata_cck_high) run
    // first, while channel 0 has never yet been touched, and the held-low
    // run second (nothing after it reuses channel 0) sidesteps this
    // uninitialized-adjacent RTL quirk entirely without editing paula.v.
    // ========================================================================
    uint16_t ldata_cck_low, ldata_cck_high;
    {
        do_reset();
        dut->cck = 1;  // released: paula_audio's cck-gated state can advance
        write_reg(AUD0VOL, 0x0040);
        write_reg(AUD0DAT, 0x7F00);
        for (int i = 0; i < 60; i++) tick();
        ldata_cck_high = dut->ldata;
    }
    {
        do_reset();
        dut->cck = 0;  // held low for the whole sequence
        write_reg(AUD0VOL, 0x0040);
        write_reg(AUD0DAT, 0x7F00);
        for (int i = 0; i < 60; i++) tick();
        ldata_cck_low = dut->ldata;
    }
    checks.note("CCK-GATES-AUDIO: identical AUD0VOL/AUD0DAT writes, ldata reads 0x" +
                hex4(ldata_cck_low) + " with cck held low for the whole sequence, and 0x" +
                hex4(ldata_cck_high) + " with cck released -- Paula's colour-clock-paced "
                "audio state advances only when cck is asserted.");
    bool cck_live = (ldata_cck_low == 0) && (ldata_cck_high != 0);
    checks.note(std::string("CCK-FIXTURE-LIVE: ") + (cck_live ? "live" : "NOT LIVE -- see MEASURED above") + " [no page -- infrastructure, not itself a claim]");

    // MUTATION PAULA-CCK-DIRECTION none impossible: CCK identifies to paula.v:65's `cck` (input), unconditionally declared -- no stimulus can make a colour-clock-enable input behave as an output. Positive control: CCK-GATES-AUDIO above (ldata frozen at 0 with cck held low, moves once cck is asserted).
    checks.expect(cck_live,
        "PAULA-CCK-DIRECTION: at paula's own boundary, CCK identifies to `cck` (input, "
        "paula.v:65, 'colour clock enable'), driven at minimig.v:498 from the same net that "
        "feeds agnus and denise -- a Verilog port-direction fact. REPRESENTATION DIFFERENCE: "
        "a clock ENABLE on a 28 MHz master, not a clock pin; not touched by appendix J's own "
        "AGNUS-table CCK contradiction (that is between p.379 and p.381's two AGNUS printings, "
        "not this PAULA row). [p.380 PAULA PIN ASSIGNMENT, DEFINITION column: CCK \"I\"] "
        "sol:J-102 opus:AHJ-235");

    // ========================================================================
    // AUDA (ldata, channel 0/3 -> LEFT per paula_audio_mixer.v's own
    // "channel 0&3 --> left") -- reuses the cck-released run above.
    // AUDB (rdata, channel 1/2 -> RIGHT) -- its own channel-1 run, same
    // recipe (AUD1VOL/AUD1DAT), cck held high throughout.
    // ========================================================================
    bool auda_live = (ldata_cck_high != 0);
    checks.note("AUDA-LIVE: ldata (channel 0/3, mixed LEFT per paula_audio_mixer.v) reads 0x" +
                hex4(ldata_cck_high) + " after a channel-0 AUDxVOL/AUDxDAT write with cck live "
                "-- reused from CCK-GATES-AUDIO above.");
    checks.note(std::string("AUDA-FIXTURE-LIVE: ") + (auda_live ? "live" : "NOT LIVE -- see MEASURED above") + " [no page -- infrastructure, not itself a claim]");

    bool audb_live;
    {
        do_reset();
        dut->cck = 1;
        write_reg(AUD1VOL, 0x0040);
        write_reg(AUD1DAT, 0x7F00);
        for (int i = 0; i < 60; i++) tick();
        uint16_t rdata_now = dut->rdata;
        checks.note("AUDB-LIVE: rdata (channel 1/2, mixed RIGHT per paula_audio_mixer.v) reads "
                    "0x" + hex4(rdata_now) + " after a channel-1 AUD1VOL/AUD1DAT write with cck "
                    "live, against a 0x0000 reset baseline.");
        audb_live = (rdata_now != 0);
        checks.note(std::string("AUDB-FIXTURE-LIVE: ") + (audb_live ? "live" : "NOT LIVE -- see MEASURED above") + " [no page -- infrastructure, not itself a claim]");
    }

    // MUTATION PAULA-AUDA-DIRECTION none impossible: AUDA identifies to paula.v:119's `ldata[14:0]` (output), unconditionally declared -- no stimulus can make an audio DAC output behave as an input. Positive control: AUDA-LIVE above.
    checks.expect(auda_live,
        "PAULA-AUDA-DIRECTION: at paula's own boundary, AUDA identifies to `ldata[14:0]` "
        "(output, paula.v:119, 'left DAC data'), connected at minimig.v:533 -- a Verilog "
        "port-direction fact. hrm/ch1-paula-what-it-contains names `ldata` an audio DAC output "
        "in its own THREE-CHIPS-DISJOINT-OUTPUTS label. REPRESENTATION DIFFERENCE: the printed "
        "pin is an analogue output, `ldata` a 15-bit digital DAC word -- that audio LEAVES "
        "Paula is graded; the analogue nature and WHICH CHANNELS feed this side (graded "
        "elsewhere, hrm/ch5-audio-*) are not restated here. "
        "[p.380 PAULA PIN ASSIGNMENT, DEFINITION column: AUDA \"O\"] "
        "sol:J-108 opus:AHJ-241");

    // MUTATION PAULA-AUDB-DIRECTION none impossible: AUDB identifies to paula.v:120's `rdata[14:0]` (output), unconditionally declared. Positive control: AUDB-LIVE above.
    checks.expect(audb_live,
        "PAULA-AUDB-DIRECTION: at paula's own boundary, AUDB identifies to `rdata[14:0]` "
        "(output, paula.v:120, 'right DAC data'), connected at minimig.v:534 -- a Verilog "
        "port-direction fact. Same representation-difference and channel-routing carve-out as "
        "AUDA above. [p.380 PAULA PIN ASSIGNMENT, DEFINITION column: AUDB \"O\"] "
        "sol:J-106 opus:AHJ-239");

    // ========================================================================
    // TXD/RXD -- reused close to verbatim from hrm/ch8-appb-chip-column-
    // paula's own T5/SERDATR technique (already proven working there): a
    // SERDAT write starts a transmission observed on txd; the testbench's
    // own rxd<-txd loopback (in tick(), above) lets that same transmission
    // be received back through paula_uart's RX state machine and read at
    // SERDATR.
    // ========================================================================
    bool txd_live;
    {
        do_reset();
        write_reg(SERPER, 0x0002);
        write_reg(SERDAT, 0x0001);
        int low_at = -1, high_at = -1;
        for (int i = 0; i < 400 && high_at < 0; i++) {
            tick();
            if (low_at < 0 && dut->txd == 0) low_at = i;
            else if (low_at >= 0 && dut->txd == 1) high_at = i;
        }
        checks.note("TXD-LIVE: SERDAT=0x0001 with SERPER=0x0002 -- txd went low at tick " +
                    std::to_string(low_at) + " and back high at tick " + std::to_string(high_at) +
                    " -- a real transmission observed on the output pin.");
        txd_live = (low_at >= 0) && (high_at > low_at);
        checks.note(std::string("TXD-FIXTURE-LIVE: ") + (txd_live ? "live" : "NOT LIVE -- see MEASURED above") + " [no page -- infrastructure, not itself a claim]");
    }

    bool rxd_live;
    {
        do_reset();
        uint16_t baseline = read_reg(SERDATR);
        write_reg(SERPER, 0x0000);
        write_reg(SERDAT, 0x00AA);
        for (int i = 0; i < 400; i++) tick();
        uint16_t after = read_reg(SERDATR);
        checks.note("RXD-LIVE: SERDATR reads 0x" + hex4(baseline) + " at reset baseline and 0x" +
                    hex4(after) + " after a 0x00AA transmission loops back through the "
                    "testbench's own rxd<-txd wiring and paula_uart's RX state machine -- rxd "
                    "genuinely carries content into paula, not a stuck value.");
        rxd_live = (after != baseline) && (after != 0);
        checks.note(std::string("RXD-FIXTURE-LIVE: ") + (rxd_live ? "live" : "NOT LIVE -- see MEASURED above") + " [no page -- infrastructure, not itself a claim]");
    }

    // MUTATION PAULA-TXD-DIRECTION none impossible: TXD identifies to paula.v:71's `txd` (output), unconditionally declared -- no stimulus can make a serial-transmit output behave as an input. Positive control: TXD-LIVE above.
    checks.expect(txd_live,
        "PAULA-TXD-DIRECTION: at paula's own boundary, TXD identifies to `txd` (output, "
        "paula.v:71, 'serial port transmitted data'), connected at minimig.v:503 -- a Verilog "
        "port-direction fact. hrm/ch1-paula-what-it-contains drives exactly this path at "
        "test_ch1-paula-what-it-contains.cpp:338 (`txd_dropped = (rig.dut->txd == 0)`), so the "
        "path is independently known live. [p.380 PAULA PIN ASSIGNMENT, DEFINITION column: "
        "TXD \"O\"] sol:J-126 opus:AHJ-259");

    // MUTATION PAULA-RXD-DIRECTION none impossible: RXD identifies to paula.v:72's `rxd` (input), unconditionally declared. Positive control: RXD-LIVE above.
    checks.expect(rxd_live,
        "PAULA-RXD-DIRECTION: at paula's own boundary, RXD identifies to `rxd` (input, "
        "paula.v:72, 'serial port received data'), connected at minimig.v:504 -- a Verilog "
        "port-direction fact. hrm/appDE-serial-uart builds at `paula_uart`, one level below, "
        "and drives the receive path there, so the channel is independently known live. "
        "[p.380 PAULA PIN ASSIGNMENT, DEFINITION column: RXD \"I\"] "
        "sol:J-128 opus:AHJ-261");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
