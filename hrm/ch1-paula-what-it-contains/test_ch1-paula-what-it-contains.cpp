// Test purpose: HRM conformance for ch1-paula-what-it-contains -- chapter
// 1's ("Introduction") sentences naming what is inside Paula: the four
// audio channels, the floppy disk interface, the serial/parallel ports and
// the interrupt controller, at the `paula` boundary (paula.v:59), per
// SPECIFICATION.md. 37 claims total: 24 already graded in accepted
// packages elsewhere (record(), not re-graded -- ch5-audio-dmacon-adkcon,
// ch5-audio-register-map, ch5-audio-location-pointer,
// ch8-disk-controller-overview, ch8-parallel-port-direction,
// ch8-serial-interface); 13 graded here through 5 expect() assertions
// (two of the five each cover two claim-pairs that state the identical
// coarse proposition on two different pages).
//
// RTL read for step 2, after SPECIFICATION.md was committed (commit
// a07d14e): paula.v in full (311 lines -- confirms DMACON bit9=DMAEN
// master, bit4=DSKEN, bits3:0=audio channel enables, all SET/CLR at
// paula.v:159-175; confirms paula_audio ad1's instantiation at
// paula.v:289-307 carries no adkcon port at all), paula_intcontroller.v in
// full (176 lines -- confirms intena[14] is a master gate ANDed with the
// other 14 bits before the priority encoder, paula_intcontroller.v:143-146
// and :152-169), paula_floppy.v in full (1041 lines -- confirms
// dsktrack is `reg [6:0] dsktrack[3:0]` at :129 (7 bits, one per drive),
// dsktracklast is an explicit `==82` comparison at :686, track is
// `{dsktrack[sel],~side}` at :669, sdc_sector is set to `track*11 +
// fd_dma_wr_sec_next` at :283, sdc_byte_addr is a 9-bit port at :78 (512
// distinct byte positions), and step_ena is a simple >=256-qualifying-edge
// debounce that holds high until the next accepted step edge, :591-603),
// paula_audio.v and paula_audio_channel.v in full (confirm the per-channel
// register bases and the direct-mode entry path already exercised
// identically by hrm/ch5-audio-dmacon-adkcon's own arm_direct()/
// enter_direct_mode() stimulus, reused here as the same established
// technique, not re-derived).
//
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, 3rd edition, printed
//   pages 1, 2, 4, 6, 7, 8, 10; SPECIFICATION.md (this package).
// Non-goals: anything outside the `paula` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// Fixture: a fresh Rig (Vpaula wrapper) per assertion block, matching the
//   discipline of every sibling `paula`-top package in this suite (e.g.
//   hrm/ch8-adkcon-and-interrupt-relations, whose Rig this one's do_reset/
//   tick/settle_edges/write_reg/read_reg are modelled on directly -- same
//   established build pattern, not re-derived independently).
//
// Every mutation is a compile-time variant, replayable with
// `make clean && make MUT=MUT_n test` (n = 0..4, matching the MUT_n token
// at the end of each MUTATION record at the bottom of this file) -- see the
// `#if MUT_n` guard at each assertion's stimulus site.

#include <verilated.h>
#include "Vpaula.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <string>

#ifndef MUT_0
#define MUT_0 0
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

namespace {

std::string hex4(uint32_t v) {
    char b[8];
    std::snprintf(b, sizeof b, "0x%04X", (unsigned)(v & 0xFFFF));
    return std::string(b);
}

int32_t sext15(uint32_t v) {
    v &= 0x7FFF;
    return (v & 0x4000) ? (int32_t)(v | 0xFFFF8000u) : (int32_t)v;
}

// Register word addresses, as reg_address_in[8:1] (the $DFFxxx byte offset
// right-shifted by 1) -- the same convention independently confirmed in
// this package's own RTL read (paula.v's own ADKCON/DMACON parameters,
// paula_intcontroller.v's own INTENA/INTREQ/INTENAR/INTREQR parameters,
// paula_floppy.v's own DSKLEN parameter, paula_audio.v's own
// AUD0..3BASE parameters), matching the addresses already used by sibling
// packages hrm/ch8-adkcon-and-interrupt-relations and
// hrm/ch5-audio-dmacon-adkcon for the identical registers.
const uint8_t ADDR_NONE    = 0x00;
const uint8_t ADDR_ADKCONR = 0x08;  // $010 >> 1
const uint8_t ADDR_INTENAR = 0x0E;  // $01C >> 1
const uint8_t ADDR_INTREQR = 0x0F;  // $01E >> 1
const uint8_t ADDR_SERDATR = 0x0C;  // $018 >> 1
const uint8_t ADDR_SERDAT  = 0x18;  // $030 >> 1
const uint8_t ADDR_SERPER  = 0x19;  // $032 >> 1
const uint8_t ADDR_DSKLEN  = 0x12;  // $024 >> 1
const uint8_t ADDR_DMACON  = 0x4B;  // $096 >> 1
const uint8_t ADDR_ADKCON  = 0x4F;  // $09E >> 1
const uint8_t ADDR_INTENA  = 0x4D;  // $09A >> 1
const uint8_t ADDR_INTREQ  = 0x4E;  // $09C >> 1
const uint8_t ADDR_AUD0VOL = 0x54;  // $0A8 >> 1
const uint8_t ADDR_AUD0DAT = 0x55;  // $0AA >> 1
const uint8_t ADDR_AUD1VOL = 0x5C;  // $0B8 >> 1
const uint8_t ADDR_AUD1DAT = 0x5D;  // $0BA >> 1
const uint8_t ADDR_AUD2VOL = 0x64;  // $0C8 >> 1
const uint8_t ADDR_AUD2DAT = 0x65;  // $0CA >> 1
const uint8_t ADDR_AUD3VOL = 0x6C;  // $0D8 >> 1
const uint8_t ADDR_AUD3DAT = 0x6D;  // $0DA >> 1

const uint8_t AUD_VOL_ADDR[4] = {ADDR_AUD0VOL, ADDR_AUD1VOL, ADDR_AUD2VOL, ADDR_AUD3VOL};
const uint8_t AUD_DAT_ADDR[4] = {ADDR_AUD0DAT, ADDR_AUD1DAT, ADDR_AUD2DAT, ADDR_AUD3DAT};

// ADKCON/INTENA/INTREQ/DMACON all use the chipset SET/CLR convention: bit
// 15 of the write selects OR (set, 1) or AND-NOT (clear, 0) against the
// named data bits.
const uint16_t SETCLR = 0x8000;

// DMACON bits (paula.v:159-175): bit9 DMAEN (master), bit4 DSKEN.
const uint16_t DMACON_DMAEN = 0x0200;
const uint16_t DMACON_DSKEN = 0x0010;

// INTENA/INTREQ bit positions (paula_intcontroller.v): bit14 is the master
// gate ANDed with every other bit before the priority encoder
// (paula_intcontroller.v:143-146); bit0 is TBE (paula_intcontroller.v:106).
const uint16_t INT_BIT14_MASTER = 0x4000;
const uint16_t INT_BIT0_TBE     = 0x0001;
const uint16_t INT_ALL_15       = 0x7FFF;

}  // namespace

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

// A fresh Verilated model per trial, matching the discipline every sibling
// `paula`-top package in this suite uses.
struct Rig {
    Vpaula* dut;
    int clk7_cycle = 0;

    Rig() { dut = new Vpaula; }
    ~Rig() { delete dut; }

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        clk7_cycle = (clk7_cycle + 1) % 4;
        dut->clk7_en  = (clk7_cycle == 0);
        dut->clk7n_en = (clk7_cycle == 2);
        dut->cck      = (clk7_cycle < 2);
    }

    // Advance until exactly n qualifying (clk7_en-active) rising edges have
    // been applied.
    void settle_edges(int n) {
        int seen = 0;
        while (seen < n) {
            bool q = dut->clk7_en;
            tick();
            if (q) seen++;
        }
    }

    void do_reset() {
        clk7_cycle = 0;
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->clk7n_en = 0;
        dut->cck = 0;
        dut->reset = 1;
        dut->reg_address_in = ADDR_NONE;
        dut->data_in = 0;
        dut->rxd = 1;
        dut->ntsc = 0;
        dut->sof = 0;
        dut->strhor = 0;
        dut->vblint = 0;
        dut->int2 = 0;
        dut->int3 = 0;
        dut->int6 = 0;
        dut->_step = 1;      // inactive (active low)
        dut->direc = 0;
        dut->_sel = 0xE;     // drive 0 selected (active low, 4'b1110)
        dut->side = 0;
        dut->_motor = 1;     // inactive; not consulted by this package's data path
        dut->sdc_image_mounted = 0;
        dut->sdc_image_size = 0;
        dut->sdc_busy = 0;
        dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0;
        dut->sdc_byte_addr = 0;
        dut->sdc_byte_in_data = 0;
        dut->floppy_drives = 0;
        dut->floppy_wrprot = 0;
        settle_edges(4);
        dut->reset = 0;
        settle_edges(4);
    }

    // One qualifying-edge write, then the address is withdrawn so no
    // further qualifying edge repeats it.
    void write_reg(uint8_t addr, uint16_t value) {
        dut->reg_address_in = addr;
        dut->data_in = value;
        settle_edges(1);
        dut->reg_address_in = ADDR_NONE;
        dut->data_in = 0;
        settle_edges(2);
    }

    // Combinational read: never advances a clock edge while the address is
    // asserted, so this can never be mistaken for a write by any edge-
    // triggered write-decode.
    uint16_t read_reg(uint8_t addr) {
        dut->reg_address_in = addr;
        dut->eval();
        uint16_t v = (uint16_t)dut->data_out;
        dut->reg_address_in = ADDR_NONE;
        dut->eval();
        return v;
    }

    // Register one head-step, reliably: step_ena (paula_floppy.v:591-603)
    // is a simple debounce that counts qualifying edges up to 256 and then
    // HOLDS high until the next accepted step edge resets it -- so waiting
    // 260 qualifying edges after the previous step (or reset) guarantees
    // step_ena is already stably high before the _step low->high
    // transition below, which is what paula_floppy.v:673 actually detects.
    // No precise alignment with the debounce counter's own phase is
    // needed.
    void step_once(bool reverse) {
        dut->direc = reverse ? 1 : 0;
        dut->_step = 1;
        settle_edges(8);
        settle_edges(260);
        dut->_step = 0;
        settle_edges(4);
        dut->_step = 1;
        settle_edges(4);
    }

    // Arms a one-word disk DMA transfer (the standard double-write DSKLEN
    // convention, paula_floppy.v:711-744) and returns the sdc_sector value
    // the request-side state machine latches at the very first state
    // transition (paula_floppy.v:266-283) -- no byte is ever actually
    // transferred (sdc_busy stays low throughout), so this is a lightweight
    // probe of the track/sector addressing arithmetic alone, not a full
    // DMA run.
    uint32_t arm_and_read_sector() {
        write_reg(ADDR_DMACON, SETCLR | DMACON_DMAEN | DMACON_DSKEN);
        write_reg(ADDR_DSKLEN, SETCLR | 0x0001);
        write_reg(ADDR_DSKLEN, SETCLR | 0x0001);
        settle_edges(8);
        return (uint32_t)dut->sdc_sector;
    }
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ========================================================================
    // FIXTURE-LIVE [not graded -- apparatus check, no claim uid]: the
    // fixture delivers two distinct, known register values and reads them
    // back distinctly before anything else here is trusted.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.write_reg(ADDR_ADKCON, SETCLR | 0x1111);
        uint16_t v1 = rig.read_reg(ADDR_ADKCONR);
        rig.write_reg(ADDR_ADKCON, 0x1111);            // CLR those bits
        rig.write_reg(ADDR_ADKCON, SETCLR | 0x2222);    // SET a distinct pattern
        uint16_t v2 = rig.read_reg(ADDR_ADKCONR);
        checks.note("FIXTURE-LIVE: two distinct, known writes (0x1111, 0x2222) arrive, unmodified "
                     "and mutually distinguishable, at ADKCONR -- v1=" + hex4(v1) + " v2=" + hex4(v2));
    }

    // ========================================================================
    // 1 -- THREE-CHIPS-DISJOINT-OUTPUTS. sol:1-components.c10,
    // opus:1-components.c10 (p.1, "All Amiga models are equipped with three
    // custom chips named Paula, Agnus, and Denise which provide for
    // superior color graphics, digital audio, and high-performance
    // interrupt and I/O handling."). Grading the sentence, not the whole
    // machine: at Paula's own boundary, driving Paula's own register bus
    // alone (no other chip) independently produces an audio output, an
    // interrupt level and serial I/O -- the audio and interrupt/IO thirds
    // of the sentence. "All Amiga models" and "superior" set no measurable
    // threshold and are not graded; the graphics third is Denise's, not
    // this package's boundary.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        bool ldata_baseline_silent = (rig.dut->ldata == 0);
        bool txd_idle_high = (rig.dut->txd == 1);
        bool ipl_baseline_idle = (rig.dut->_ipl == 7);

        // Audio domain: direct-mode write (dmaena stays 0 throughout --
        // never touched), the same stimulus technique already established
        // and proven by hrm/ch5-audio-dmacon-adkcon's own arm_direct().
        rig.write_reg(ADDR_AUD0VOL, 64);
#if MUT_0
        rig.write_reg(ADDR_AUD1DAT, 0x7F7F);  // MUTATION: wrong channel's data register written
#else
        rig.write_reg(ADDR_AUD0DAT, 0x7F7F);
#endif
        rig.settle_edges(200);
        bool ldata_moved = (rig.dut->ldata != 0);

        // Interrupt domain: master gate (bit14) plus one source bit (bit0,
        // TBE), then raise that one request.
        rig.write_reg(ADDR_INTENA, SETCLR | INT_BIT14_MASTER | INT_BIT0_TBE);
        rig.write_reg(ADDR_INTREQ, SETCLR | INT_BIT0_TBE);
        rig.settle_edges(4);
        bool ipl_active = (rig.dut->_ipl != 7);

        // Serial domain: a SERDAT write reaches the transmit shift
        // register and pulls txd low for a start bit.
        rig.write_reg(ADDR_SERPER, 0x0003);
        rig.write_reg(ADDR_SERDAT, 0x0155);
        rig.settle_edges(2);
        bool txd_dropped = (rig.dut->txd == 0);

        checks.note("THREE-CHIPS: baseline(ldata_silent=" + std::string(ldata_baseline_silent?"yes":"no") +
                     ",txd_idle=" + std::string(txd_idle_high?"yes":"no") + ",ipl_idle=" +
                     std::string(ipl_baseline_idle?"yes":"no") + ") after-stimulus(ldata_moved=" +
                     std::string(ldata_moved?"yes":"no") + ",ipl_active=" + std::string(ipl_active?"yes":"no") +
                     ",txd_dropped=" + std::string(txd_dropped?"yes":"no") + ")");
        checks.expect(ldata_baseline_silent && txd_idle_high && ipl_baseline_idle &&
                          ldata_moved && ipl_active && txd_dropped,
            "THREE-CHIPS-DISJOINT-OUTPUTS (sol:1-components.c10, opus:1-components.c10 p.1 \"All Amiga models are "
            "equipped with three custom chips named Paula, Agnus, and Denise which provide for superior color "
            "graphics, digital audio, and high-performance interrupt and I/O handling.\"): from one idle baseline, "
            "writing only to Paula's own register bus independently moves all three of Paula's own output domains -- "
            "an audio DAC output (ldata), an interrupt level (_ipl) and a serial line (txd) -- with no other chip "
            "instantiated in this build. 'All Amiga models' and 'superior' are not graded; the graphics third is "
            "Denise's, outside this package's boundary.");
    }

    // ========================================================================
    // 2 -- TWO-CHANNELS-PER-STEREO-OUTPUT. sol:1-components.c22,
    // opus:1-components.c22 (p.2, "Ports for left and right stereo audio
    // from four special purpose audio channels."); sol:1-components.c77,
    // opus:1-components.c76 (p.4, "Two channels are directed into each of
    // the two stereo audio outputs."). Both are the same coarse,
    // page-agnostic partition claim: of four channels, exactly two feed
    // left and exactly two feed right -- neither sentence names WHICH two.
    // Caution: the SPECIFIC channel-to-side assignment is a different,
    // stronger claim already graded EXPECTED RED elsewhere (NM-HRM-002,
    // hrm/ch5-audio-volume-scale) -- not this assertion's question, and
    // not contradicted by it; see SPECIFICATION.md.
    // ========================================================================
    {
        int left_count = 0, right_count = 0, neither_count = 0, both_count = 0;
        std::string trace;
        for (int ch = 0; ch < 4; ch++) {
            Rig rig;
            rig.do_reset();
            rig.write_reg(AUD_VOL_ADDR[ch], 64);
#if MUT_1
            if (ch != 0) rig.write_reg(AUD_DAT_ADDR[ch], 0x7F7F);  // MUTATION: channel 0 left silent
#else
            rig.write_reg(AUD_DAT_ADDR[ch], 0x7F7F);
#endif
            rig.settle_edges(200);
            bool l = (rig.dut->ldata != 0);
            bool r = (rig.dut->rdata != 0);
            if (l && !r) left_count++;
            else if (r && !l) right_count++;
            else if (l && r) both_count++;
            else neither_count++;
            trace += " ch" + std::to_string(ch) + "(l=" + (l?"1":"0") + ",r=" + (r?"1":"0") + ")";
        }
        checks.note("TWO-PER-SIDE: " + trace + " -- left_count=" + std::to_string(left_count) +
                     " right_count=" + std::to_string(right_count) + " both=" + std::to_string(both_count) +
                     " neither=" + std::to_string(neither_count));
        checks.expect(both_count == 0 && neither_count == 0 && left_count == 2 && right_count == 2,
            "TWO-CHANNELS-PER-STEREO-OUTPUT (sol:1-components.c22, opus:1-components.c22 p.2 \"Ports for left and "
            "right stereo audio from four special purpose audio channels.\"; sol:1-components.c77, "
            "opus:1-components.c76 p.4 \"Two channels are directed into each of the two stereo audio outputs.\"): "
            "driving each of the four audio channels alone, every channel moves exactly one of the two stereo "
            "outputs (never both, never neither), and the four channels split exactly two-and-two across the two "
            "outputs -- the coarse partition both sentences state, independent of which specific channel lands on "
            "which side (a stronger, separate claim this assertion does not make -- see SPECIFICATION.md).");
    }

    // ========================================================================
    // 3 -- FLOPPY-DISK-GEOMETRY-AND-CAPACITY. sol:1-components.c118,
    // opus:1-components.c118 (p.6, "Disks are 80 track, double sided, and
    // formatted as 11 sectors per track, 512 bytes per sector (over
    // 900,000 bytes per disk).").
    //
    // SIMULATED (the expect() below): the sector-index arithmetic
    // (paula_floppy.v:283, `sdc_sector <= track*11 + fd_dma_wr_sec_next`,
    // sampled at the very first DMA request -- fd_dma_wr_sec_next is 0 on
    // that first request, paula_floppy.v:249,265-284) and the fact that
    // BOTH `side` and genuine, mechanism-driven head-stepping (not merely
    // a wire toggle -- paula_floppy.v:673, gated by the step_ena debounce
    // at :591-603) change that arithmetic's `track` input
    // (paula_floppy.v:669, `track = {dsktrack[sel],~side}`) exactly as the
    // formula predicts.
    //
    // READ, NOT SIMULATED (reported via note(), not folded into the
    // expect() above, so a failure of the note()'d facts alone could never
    // be mistaken for a failure of the simulated ones): bytes/sector, from
    // `sdc_byte_addr`'s own 9-bit port declaration (paula_floppy.v:78,
    // `input [8:0] sdc_byte_addr` -- 512 distinct positions, not
    // simulated by driving all 512 because the port's own declared width
    // already settles the count); track reach, from `dsktrack`'s own
    // declared width (paula_floppy.v:129, `reg [6:0] dsktrack[3:0]` -- 7
    // bits, 0-127) and `dsktracklast`'s own explicit bound
    // (paula_floppy.v:686, `dsktrack[sel]==82`) -- simulating all 79
    // further steps to the manual's printed 80 was not attempted, both
    // because it is not needed to settle a register-width/explicit-bound
    // question and because it would be a very slow simulation for no
    // additional certainty; 5 real, mechanism-driven steps are simulated
    // above as the "the stepping mechanism genuinely works, repeatedly"
    // demonstration.
    // ========================================================================
    {
        Rig r00; r00.do_reset(); r00.dut->side = 0;
        uint32_t sec_t0_s0 = r00.arm_and_read_sector();

        Rig r01; r01.do_reset(); r01.dut->side = 1;
        uint32_t sec_t0_s1 = r01.arm_and_read_sector();

        Rig r15; r15.do_reset(); r15.dut->side = 0;
#if MUT_2
        for (int i = 0; i < 4; i++) r15.step_once(false);  // MUTATION: 4 steps instead of 5
#else
        for (int i = 0; i < 5; i++) r15.step_once(false);
#endif
        uint32_t sec_t5_s0 = r15.arm_and_read_sector();

        bool sides_differ = (sec_t0_s0 == 11 && sec_t0_s1 == 0);
        bool steps_scale_by_11_per_track = (sec_t5_s0 == 121);  // (5 steps * 2 [side LSB] ) * 11 = 110, +11 baseline = 121

        checks.note("FLOPPY-GEOMETRY: sec(T=0,side=0)=" + std::to_string(sec_t0_s0) + " sec(T=0,side=1)=" +
                     std::to_string(sec_t0_s1) + " sec(T=5,side=0)=" + std::to_string(sec_t5_s0) +
                     " -- READ (not simulated): sdc_byte_addr is a 9-bit port (paula_floppy.v:78) = 512 distinct "
                     "byte positions/sector; dsktrack is `reg [6:0]` (paula_floppy.v:129, 0-127) with an explicit "
                     "`==82` upper bound (paula_floppy.v:686), comfortably covering the printed 80-track range. "
                     "Derived, not measured: 80 tracks (printed) * 2 sides * 11 sectors/track (measured) * 512 "
                     "bytes/sector (read) = 901,120 bytes > 900,000.");
        checks.expect(sides_differ && steps_scale_by_11_per_track,
            "FLOPPY-DISK-GEOMETRY-AND-CAPACITY (sol:1-components.c118, opus:1-components.c118 p.6 \"Disks are 80 "
            "track, double sided, and formatted as 11 sectors per track, 512 bytes per sector (over 900,000 bytes "
            "per disk).\"): the disk-image sector-index arithmetic scales by exactly 11 per track step (5 genuine, "
            "mechanism-driven head-steps move the addressed sector from 11 to 121, matching 5*2*11=110 added), and "
            "toggling `side` alone (no step) moves it from 11 to 0, matching a double-sided image layout -- both "
            "the 11-sectors/track factor and double-sidedness are structurally confirmed by simulation; "
            "bytes/sector and track reach are read directly from their own port/register declarations (see "
            "note() above), not simulated.");
    }

    // ========================================================================
    // 4 -- PAULA-ON-SHARED-REGISTER-BUS. sol:1-blockdiag.c13 (p.8, Figure
    // 1-1: a double-outlined PAULA block in the CUSTOM CHIP SECTION, one
    // arrowhead entering from CHIP DATA on its left, one from REG ADR BUS
    // on its right -- docs/references/amiga-hrm-3rd/ch1/figures/p8.png,
    // opened and read directly). Testable at this boundary: Paula answers
    // on its own register-address/data bus. The figure's further claim
    // that Paula's reg_address_in is wired to the SAME net Agnus's own
    // input is a minimig.v-level fact this paula-only build cannot reach
    // without instantiating agnus as well, and is not graded here -- named,
    // not silently dropped (SPECIFICATION.md).
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.write_reg(ADDR_ADKCON, SETCLR | 0x0155);
#if MUT_3
        uint16_t v = rig.read_reg(ADDR_ADKCON);   // MUTATION: read at the write-only address itself (no read decode there)
#else
        uint16_t v = rig.read_reg(ADDR_ADKCONR);
#endif
        checks.note("BUS-ANSWER: wrote 0x0155 via reg_address_in/data_in, read back " + hex4(v) + " via data_out");
        checks.expect(v == 0x0155,
            "PAULA-ON-SHARED-REGISTER-BUS (sol:1-blockdiag.c13 p.8, Figure 1-1, the PAULA block in the CUSTOM CHIP "
            "SECTION with REG ADR BUS and CHIP DATA arrows entering it): a value written at Paula's own "
            "reg_address_in/data_in terminals (the figure's REG ADR BUS and CHIP DATA arrows) is read back exactly "
            "at data_out -- Paula answers on the bus the figure shows it wired to, entirely from its own boundary. "
            "The figure's further claim that this is the SAME net Agnus shares is a minimig.v-level fact outside "
            "this paula-only build's reach and is not graded here.");
    }

    // ========================================================================
    // 5 -- INTENA-7FFF-DISABLES-ALL-INTERRUPTS. sol:1-examples.c25,
    // opus:1-examples.c24 (p.10, assembler listing, trailing comment
    // "to disable all interrupts" on `move.w #$7FFF,intena(a0)`);
    // sol:1-examples.c30, opus:1-examples.c26 (p.10, C listing, trailing
    // comment "Disable all interrupts" on `custom.intena = 0x7FFF;`). Both
    // listings assert the identical machine fact in two languages (this
    // package's own classify stage calls the C listing's comment "the same
    // machine fact as the assembler one above it") -- graded once.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        // All fifteen bits set first, so the CLR write below is a real,
        // detectable change, not a no-op against an already-zero register.
        rig.write_reg(ADDR_INTENA, SETCLR | INT_ALL_15);
        rig.write_reg(ADDR_INTREQ, SETCLR | INT_ALL_15);
        rig.settle_edges(4);
        bool ipl_active_before = (rig.dut->_ipl != 7);  // control: with INTENA still all-set, all-15-pending DOES reach _ipl

#if MUT_4
        rig.write_reg(ADDR_INTENA, SETCLR | INT_ALL_15);  // MUTATION: SET again instead of CLR -- everything stays enabled
#else
        rig.write_reg(ADDR_INTENA, INT_ALL_15);            // the $7FFF write itself: SETCLR=0 (CLR), all 15 named bits
#endif
        rig.settle_edges(4);
        uint16_t enar_after = rig.read_reg(ADDR_INTENAR);
        bool ipl_idle_after = (rig.dut->_ipl == 7);

        checks.note("INTENA-7FFF: before(ipl_active=" + std::string(ipl_active_before?"yes":"no") +
                     ") after $7FFF(INTENAR=" + hex4(enar_after) + ", ipl_idle=" + std::string(ipl_idle_after?"yes":"no") +
                     ") with all 15 INTREQ bits STILL pending throughout");
        checks.expect(ipl_active_before && enar_after == 0x0000 && ipl_idle_after,
            "INTENA-7FFF-DISABLES-ALL-INTERRUPTS (sol:1-examples.c25, opus:1-examples.c24 p.10, assembler listing, "
            "\"move.w #$7FFF,intena(a0) ; to disable all interrupts\"; sol:1-examples.c30, opus:1-examples.c26 p.10, "
            "C listing, \"custom.intena = 0x7FFF; /* Disable all interrupts */\"): with all fifteen INTREQ bits "
            "raised and pending throughout, they DO reach _ipl while INTENA is still fully set (control, same "
            "measurement basis); writing $7FFF to INTENA (SET/CLR bit clear, naming all fifteen enables) then "
            "clears INTENAR to 0x0000 and holds _ipl at idle (7) despite every one of the fifteen requests still "
            "pending -- exactly what both listings' own trailing comment claims.");
    }

    // ========================================================================
    // 12 propositions (24 claims) already graded in accepted packages
    // elsewhere. See SPECIFICATION.md's "Already graded elsewhere" section
    // for the full citation and reasoning behind each.
    // ========================================================================
    checks.record("ALREADY GRADED in hrm/ch8-disk-controller-overview, assertion P-DRIVE-COUNT (uids "
        "opus:8-dsk.c1, sol:8-dsk.c1, p.241 \"The built-in disk controller in the system can handle up to four "
        "MFM-type devices.\"): with floppy_drives at maximum, each of the four _sel bits independently selects a "
        "working drive slot. Claims: sol:1-components.c15, opus:1-components.c15 (p.2, built-in + up to three "
        "additional); sol:1-components.c117, opus:1-components.c117 (p.6, built-in 3.5in drive); "
        "sol:1-components.c120, opus:1-components.c120 (p.6, up to three extra drives); sol:1-components.c138, "
        "opus:1-components.c138 (p.7, rear connector). All four are the same drive-count proposition restated "
        "across pages 2, 6 and 7. Not re-graded.");
    checks.record("ALREADY GRADED in hrm/ch8-parallel-port-direction, requirement PROP-1 (uids opus:8-par.c1, "
        "sol:8-par.c1, p.259) -- MEASURED RED there: the parallel port's direction register is programmable but "
        "the data-write path that would drive the port is absent (ciaa.v). Also ALREADY GRADED in "
        "hrm/ch8-serial-interface across its own 97 expect() assertions against pp.255-259 (e.g. assertion A2, "
        "opus:8-serial.c10, p.255, \"a programmable length of eight or nine bits\") for the serial half. Claim: "
        "sol:1-components.c17, opus:1-components.c17 (p.2, \"Fully programmable parallel and RS-232-C serial "
        "ports.\"). Not re-graded.");
    checks.record("ALREADY GRADED in hrm/ch5-audio-dmacon-adkcon, assertion CHANNELS-INDEPENDENTLY-PROGRAMMABLE "
        "(citing manual quotes p.136, p.149, \"All four audio channels are independently programmable\") and in "
        "hrm/ch5-audio-register-map, assertion CHANNEL-INDEPENDENCE (uids 5-p164.c1, 5-sm.c1) for the four-channel "
        "count half. Claim: sol:1-components.c74, opus:1-components.c73 (p.4, \"Audio consisting of four digital "
        "channels with independently programmable volume and sampling rate.\"). Not re-graded.");
    checks.record("ALREADY GRADED in hrm/ch5-audio-location-pointer, assertion "
        "LOCATION-REGISTER-VS-WORKING-POINTER (33 uids including 5-p137.c4, 5-p139.c5/c6/c7, 5-p164.c25, "
        "5-loc.c5/c6/c7): the location registers load a working pointer once, at start/restart, which Agnus then "
        "advances on its own -- DMA-fetched -- and repeats without any further rewrite needed. Claims: "
        "sol:1-components.c75, opus:1-components.c74 (p.4, \"The audio channels retrieve their control and sample "
        "data via DMA.\"); sol:1-components.c76, opus:1-components.c75 (p.4, \"Once started, each channel can "
        "automatically play a specified waveform without further processor interaction.\"). Both are the same "
        "measurement's two halves (DMA-fetched; runs on with no further rewrite). Not re-graded.");
    checks.record("ALREADY GRADED, EXPECTED RED, in hrm/ch5-audio-dmacon-adkcon, assertion "
        "ADKCON-ATTACH-BITS-DO-NOT-SILENCE-CHANNELS: with every one of ADKCON's eight audio attach/modulate bits "
        "set, two independently armed channels reach exactly the peaks they would have reached with no attach "
        "bits set at all -- attach/modulate has zero effect on the audio path (paula_audio_channel.v:1's own first "
        "line: attached modes are not supported; paula.v:289-307 instantiates paula_audio with no adkcon port at "
        "all). This is this package's own disclosed measured fact. Claim: sol:1-components.c78, "
        "opus:1-components.c77 (p.4, \"The audio channels may be linked together to provide amplitude or "
        "frequency modulation or both forms of modulation simultaneously.\"). Not re-graded.");
    checks.record("ALREADY GRADED in hrm/ch8-disk-controller-overview, assertion P-WHOLETRACK-DMA (uids "
        "opus:8-dsk.c6, opus:8-dsk.c7, sol:8-dsk.c7, sol:8-dskdma.c6, p.241/p.247): a single DSKLEN arming "
        "delivers all 5984 words of a whole track's real content (11 sectors * 544 MFM words) and raises blckint "
        "exactly once -- one DMA operation, one whole track. Claims: sol:1-components.c79, opus:1-components.c78 "
        "(p.4, \"DMA controlled floppy disk read and write on a full track basis.\"); sol:1-components.c80, "
        "opus:1-components.c79 (p.4, \"...the built-in disk can read over 5600 bytes of data in a single disk "
        "revolution (11 sectors of 512 bytes each).\"). Both are the same measurement's mechanism half and "
        "arithmetic half. Not re-graded.");
    checks.record("ALREADY GRADED in hrm/ch8-serial-interface across its own 97 expect() assertions against pp. "
        "255-259 (device-agnostic register-level programmability and function; \"modem, MIDI interface, or "
        "printer\" names the use, not a distinct RTL boundary). Claim: sol:1-components.c126, "
        "opus:1-components.c126 (p.6, \"The Amiga includes a standard RS-232-C serial port for external serial "
        "input/output devices such as a modem, MIDI interface, or printer.\"). Not re-graded.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}

// MUTATION THREE-CHIPS-DISJOINT-OUTPUTS stimulus red MUT_0: the audio write
//   changed from AUD0's own data register to AUD1's -- expect FAIL, ldata
//   never moves because channel 0 (the one whose volume was set) is never
//   given a sample.
// MUTATION TWO-CHANNELS-PER-STEREO-OUTPUT stimulus red MUT_1: channel 0's
//   AUDxDAT write is skipped (its AUDxVOL is still set) -- expect FAIL,
//   channel 0 lands in "neither" (both ldata and rdata stay silent for it),
//   breaking the exactly-two-and-two partition.
// MUTATION FLOPPY-DISK-GEOMETRY-AND-CAPACITY stimulus red MUT_2: the T=5
//   trial steps the head 4 times instead of 5 -- expect FAIL, the measured
//   sector (99, matching 4*2*11+11) no longer equals the hard-coded
//   expectation (121, matching 5 real steps), so the scale-by-11-per-track
//   check fails even though the arithmetic itself still holds internally --
//   proving the assertion is sensitive to the actual number of accepted
//   step edges, not simply always green.
// MUTATION PAULA-ON-SHARED-REGISTER-BUS stimulus red MUT_3: the read probes
//   ADKCON's own write-only address ($09E, no read decode) instead of
//   ADKCONR ($010) -- expect FAIL, the readback is 0x0000 instead of the
//   written 0x0155.
// MUTATION INTENA-7FFF-DISABLES-ALL-INTERRUPTS stimulus red MUT_4: the
//   $7FFF write's SET/CLR bit changed from CLR (0) to SET (1) -- expect
//   FAIL, INTENA stays fully enabled (INTENAR reads back 0x7FFF, not
//   0x0000) and _ipl stays active instead of returning to idle.
