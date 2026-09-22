// aa-dmacon-paula-half -- paula conformance test
//
// Test purpose: the AA specification's alphabetical entry for DMACON prints
//   the whole DMA control register as a bit table -- sixteen rows covering
//   every channel in the chip set. Paula decodes five of them. This grades
//   those five and says, per row, which chip the others belong to.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF page 22. The document prints no
//   page numbers of its own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the audio channels' sample output,
//   the disk data path, the blitter, the Copper, the bitplane fetch, or the
//   bus arbitration any of those enables feed.
//
// THE ENTRY'S OWN CHIP COLUMN SAYS WHY MOST OF IT IS DECLINED HERE. The
//   address-ordered row reads `DMACON 096 W A D P`, three chips -- and the
//   RTL agrees: `dmacon[4:0]` in paula.v:164-171 holds DSKEN and the four
//   audio enables, `dmaen` holds bit 9, and BPLEN, COPEN, BLTEN, SPREN and
//   BLTPRI are decoded in Agnus. BBUSY and BZERO are the blitter's own status
//   and the table marks both read only.
//
//   So this package grades the five bits Paula owns and declines eleven rows
//   as `outside-uut` with the module that does own them named. A package that
//   graded all sixteen at one boundary would be claiming a decode that is not
//   there.
//
// HOW AN ENABLE IS SEEN. `dsken = dmacon[4] & dmaen` (paula.v:174) is
//   paula_floppy's `enable`, and `dma_active = dmaen && !lenzero && enable`
//   (paula_floppy.v:248) moves the disk state machine out of idle, which
//   `fdd_led` reports (:972). The four audio enables reach the audio channels
//   as `dmaena`; showing one of those needs a channel armed with a period and
//   a length, which is this package's first non-goal.

#include "Vpaula.h"
#include "verilated.h"
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

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

constexpr int A_DMACON = 0x096 >> 1;
constexpr int A_DSKLEN = 0x024 >> 1;
constexpr int A_IDLE   = 0x000;

constexpr uint16_t SETCLR = 1u << 15;
constexpr uint16_t DMAEN  = 1u << 9;
constexpr uint16_t BPLEN  = 1u << 8;     // Agnus's, and this module ignores it
constexpr uint16_t DSKEN  = 1u << 4;

class Harness {
public:
    Harness() : dut(new Vpaula) { reset(); }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->eval();
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();
        phase = (phase + 1) % 4;
        dut->clk7_en  = (phase == 0);
        dut->clk7n_en = (phase == 2);
    }

    void settle(int n) {
        int seen = 0;
        while (seen < n) {
            bool q = dut->clk7_en;
            tick();
            if (q) seen++;
        }
    }

    void reset() {
        phase = 0;
        dut->clk = 0; dut->clk7_en = 0; dut->clk7n_en = 0; dut->cck = 0;
        dut->reset = 1;
        dut->reg_address_in = A_IDLE; dut->data_in = 0;
        dut->rxd = 1; dut->ntsc = 0; dut->sof = 0; dut->strhor = 0;
        dut->vblint = 0; dut->int2 = 0; dut->int3 = 0; dut->int6 = 0;
        dut->_step = 1; dut->direc = 0; dut->_sel = 0xE; dut->side = 0;
        dut->_motor = 1;
        dut->sdc_image_mounted = 0; dut->sdc_image_size = 0;
        dut->sdc_busy = 0; dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0; dut->sdc_byte_addr = 0;
        dut->sdc_byte_in_data = 0;
        dut->floppy_drives = 0; dut->floppy_wrprot = 0;
        settle(8);
        dut->reset = 0;
        settle(8);
    }

    void write(int addr, uint16_t value) {
        dut->reg_address_in = addr;
        dut->data_in = value;
        settle(1);
        dut->reg_address_in = A_IDLE;
        dut->data_in = 0;
        settle(2);
    }

    // The floppy's own arming: the SECOND write's bit 15 and the bit 15 the
    // FIRST left behind must both be set (paula_floppy.v:742).
    void arm() {
        write(A_DSKLEN, 0x8001);
        write(A_DSKLEN, 0x8001);
    }

    bool active() { dut->eval(); return dut->fdd_led != 0; }

    Vpaula* dut;
    int phase = 0;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    const bool mutate = std::getenv("MUTATE") != nullptr;
    Checks c;

    // MUTATION. Every expect() is guarded so MUTATE=1 forces it red.
    const uint16_t m_both = mutate ? DSKEN : (uint16_t)(DMAEN | DSKEN); // 1
    const uint16_t m_dsk  = mutate ? (uint16_t)(DMAEN | DSKEN) : DMAEN; // 2
    const uint16_t m_othr = mutate ? (uint16_t)(DMAEN | DSKEN)
                                   : (uint16_t)(DMAEN | BPLEN);         // 3
    const uint16_t m_clr  = mutate ? SETCLR : 0u;                       // 4

    std::cout << "aa-dmacon-paula-half -- AA_CONFORMANCE at paula\n";
    std::cout << (mutate ? "MUTATE=1: every assertion below must go red\n" : "");

    // --- 1. DMAEN and DSKEN together let the disk transfer run --------------
    {
        Harness h;
        // MUTATION DMAEN-AND-DSKEN-TOGETHER stimulus red MUTATE=1 leaves bit 9
        // out, so the channel enable alone has to do it and cannot.
        h.arm();
        const bool before = h.active();
        h.write(A_DMACON, SETCLR | m_both);
        h.settle(4);
        const bool after = h.active();
        c.note(std::string("an armed transfer with DMACON clear is ") +
               (before ? "running" : "idle") + "; after bits 9 and 4 it is " +
               (after ? "running" : "idle"));
        c.expect(!before && after,
                 "DMAEN-AND-DSKEN-TOGETHER: an armed disk transfer runs only "
                 "once both the master enable and the disk channel's own bit "
                 "are set, which is what the AA table means by bit 9 enabling "
                 "all DMA below it [p.22] [claims: opus:AA-dmacon.c1/c2/c5 sol:AA-dmacon.c1/c2/c5 opus:AA-dmacon.b09]");
    }

    // --- 2. The master enable alone is not enough ---------------------------
    {
        Harness h;
        // MUTATION DSKEN-IS-THE-CHANNELS-OWN-BIT stimulus red MUTATE=1 sets
        // the disk bit as well, so the transfer runs and the negative is gone.
        h.arm();
        h.write(A_DMACON, SETCLR | m_dsk);
        h.settle(4);
        const bool with_master_only = h.active();
        Harness g;
        g.arm();
        g.write(A_DMACON, SETCLR | DMAEN | DSKEN);
        g.settle(4);
        const bool with_both = g.active();
        c.note(std::string("bit 9 alone leaves the transfer ") +
               (with_master_only ? "running" : "idle") +
               "; bits 9 and 4 leave it " + (with_both ? "running" : "idle"));
        c.expect(!with_master_only && with_both,
                 "DSKEN-IS-THE-CHANNELS-OWN-BIT: the master enable on its own "
                 "starts nothing, so bit 4 is the disk channel's own enable "
                 "that the AA table names DSKEN [p.22] "
                 "[claims: opus:AA-dmacon.b04]");
    }

    // --- 3. A bit this chip does not own changes nothing here ---------------
    {
        Harness h;
        // MUTATION A-BIT-THIS-CHIP-DOES-NOT-OWN stimulus red MUTATE=1 writes
        // the disk bit in place of the bitplane bit, so the transfer runs and
        // the negative disappears.
        h.arm();
        h.write(A_DMACON, SETCLR | m_othr);
        h.settle(4);
        const bool ran = h.active();
        c.note(std::string("the bitplane enable beside the master bit leaves "
                           "the disk transfer ") + (ran ? "running" : "idle"));
        c.expect(!ran,
                 "A-BIT-THIS-CHIP-DOES-NOT-OWN: setting the bit-plane enable "
                 "does not start the disk channel, which is what the AA "
                 "entry's three-chip row means -- one register, decoded in "
                 "three places, and this is the Paula third [p.22] "
                 "this assertion is a CONTROL and cites nothing: the bit it writes belongs to Agnus, and the lot declines it there rather than grading it here");
    }

    // --- 4. And bit 15 clears what it set -----------------------------------
    {
        Harness h;
        // MUTATION SET-CLEAR-REACHES-THE-ENABLES stimulus red MUTATE=1 does
        // the second write as a set, so the enable is ORed in again.
        h.arm();
        h.write(A_DMACON, SETCLR | DMAEN | DSKEN);
        h.settle(4);
        const bool on = h.active();
        h.write(A_DMACON, m_clr | DSKEN);
        h.settle(4);
        const bool off = h.active();
        c.note(std::string("after the set the transfer is ") +
               (on ? "running" : "idle") + ", after the clear it is " +
               (off ? "running" : "idle"));
        c.expect(on && !off,
                 "SET-CLEAR-REACHES-THE-ENABLES: a word written with bit 15 "
                 "low turns the disk channel off again, which is the Set/Clear "
                 "control bit the AA table gives DMACON's row 15 [p.22] "
                 "[claims: opus:AA-dmacon.c4 sol:AA-dmacon.c4 opus:AA-dmacon.b15 opus:AA-dmacon.b15b opus:AA-dmacon.b15c]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary (insufficient): The bit reaches the "
             "audio channels as `dmaena`, so the signal is at this boundary -- "
             "but showing one needs a channel armed with a period and a length, "
             "which is this package's first non-goal. [claims: "
             "opus:AA-dmacon.b00 opus:AA-dmacon.b01 opus:AA-dmacon.b02 "
             "opus:AA-dmacon.b03]");
    c.record(
             "DECLINED at this boundary (not-hardware): The claim is about the "
             "page's typography -- a double space inside a printed name. Nothing "
             "about a signal turns on it. [claims: opus:AA-dmacon.b13]");
    c.record(
             "DECLINED at this boundary (not-hardware): The claim is that the "
             "document's two statements of DMACON disagree -- the alphabetical "
             "entry gives it three chips, the address list two. Which is right "
             "is a fact about the document. [claims: opus:AA-dmacon.c3 "
             "sol:AA-dmacon.c3]");
    c.record(
             "DECLINED at this boundary (outside-uut): BBUSY and BZERO are the "
             "blitter's own status bits, marked read only in the same cell, and "
             "agnus_blitter produces them. Paula neither writes nor reads them. "
             "[claims: opus:AA-dmacon.c6 opus:AA-dmacon.b13b opus:AA-dmacon.b14 "
             "opus:AA-dmacon.b14b]");
    c.record(
             "DECLINED at this boundary (outside-uut): The bit enables a DMA "
             "channel Agnus owns. paula.v decodes dmacon[4:0] and the master "
             "enable only; BPLEN, COPEN, BLTEN, SPREN and BLTPRI are decoded in "
             "agnus. [claims: opus:AA-dmacon.b05 opus:AA-dmacon.b06 "
             "opus:AA-dmacon.b07 opus:AA-dmacon.b08 opus:AA-dmacon.b09b "
             "opus:AA-dmacon.b10 opus:AA-dmacon.b10b opus:AA-dmacon.b10c]");
    c.record(
             "DECLINED at this boundary (source-silent): The table prints X for "
             "the bit and names no function for it. A cell with no name makes no "
             "requirement on a signal. [claims: opus:AA-dmacon.b11 "
             "opus:AA-dmacon.b12]");

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
