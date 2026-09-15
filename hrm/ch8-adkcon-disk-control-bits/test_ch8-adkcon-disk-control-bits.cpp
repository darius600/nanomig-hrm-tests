// Test purpose: HRM conformance for ch8-adkcon-disk-control-bits -- ADKCON
// ($DFF09E, write) / ADKCONR ($DFF010, read), the disk half of every
// function-column row, PRECOMP 1-0 (bits 14-13) and WORDSYNC (bit 10), at
// the `paula` boundary, per SPECIFICATION.md. Thirteen claims, all cited
// there; none declined.
//
// Boundary correction (disclosed by the build brief, independently
// confirmed here by reading paula.v myself): ADKCON/ADKCONR decode inside
// `paula` (paula.v:126-127, the register-write block at :182-195, the
// read mux at :198). `paula` instantiates `paula_floppy` directly
// (paula.v:243-286); `adkcon` has exactly two fan-outs anywhere in
// paula.v: bit 11 to paula_uart's `uartbrk` (:211, out of this package's
// scope -- UART, not disk) and bit 10 to paula_floppy's `wordsync` port
// (:269). No other adkcon bit reaches any submodule at all -- confirmed by
// reading paula.v's instantiations in full. PRECOMP (14-13), and (not
// asserted by any claim in this package, reported via note() only) MFMPREC
// (12), MSBSYNC (9) and FAST (8), are stored and read back and drive
// nothing -- finding candidate C8.
//
// Primary oracle: the manual quotations cited inline (see
// SPECIFICATION.md), not the RTL. Every assertion in this package is
// expected GREEN: A1/A2 grade only that ADKCON's/ADKCONR's write/read
// functions reach real disk-side logic (true, via the wordsync fan-out),
// not that every disk-labelled bit has an effect; A3/A4/A5 grade PRECOMP's
// position and per-bit storage only, per their own claims' explicit split
// ("what the field DOES ... is not asserted"); A6 grades WORDSYNC's own
// specific, real mechanism. Not overreaching into C8's red half (PRECOMP/
// MFMPREC/MSBSYNC/FAST doing nothing) is deliberate: no claim in this
// package's thirteen asserts an effect for those bits, so none is graded
// through expect() -- inventing one would be exactly the overreach this
// suite's process exists to prevent. The two note()s below report that red
// half as MEASURED, graded by nothing.
//
// WORDSYNC mechanism, read from paula_floppy.v (trackrdok/sync_match,
// :508-509, :914-926) after SPECIFICATION.md was committed: `dsksync`
// resets to 0x4489 (:704-705) and the fixed value `floppy_data` emits at
// word-count 2/3 (the synthesised sync mark, :485) is also 0x4489, so
// `sync_match` (:914-915) fires as soon as a track read reaches that
// position, independent of any byte actually fed through the SD interface.
// `trackrdok` (:920-926) is `~wordsync | sync_match | trackrdok`, latched:
// with `wordsync`=0 it is 1 from the first qualifying tick (no match
// needed); with `wordsync`=1 it stays 0 until `sync_match` first fires.
// `fifo_wr`'s read-side term (:771) requires `trackrdok`, so the FIRST word
// actually pushed into the disk FIFO -- and therefore the first word
// DSKDATR delivers -- differs measurably: 0xAAAA (the raw, unfiltered
// preamble word at word-count 0) with wordsync=0, versus 0x4489 (the sync
// mark itself, the first position trackrdok has latched by) with
// wordsync=1. `disk_dmal` (paula's own top-level port, wired straight from
// paula_floppy's `dmal`, paula.v:255) telling `dmaon & ~fifo_empty` is used
// to wait for that first push before popping, so no fixed cycle count is
// guessed at.
//
// This mechanism was checked against the DMA-arming and SD-sector-feed
// pattern already built and running in `hrm/ch8-dsklen-dma-arming` and
// `hrm/ch8-drive-select-motor-led` (read for build/rig plumbing only, per
// the build brief and this package's own SPECIFICATION.md -- their
// specific claim/assertion content is not cited as an oracle here, only
// the mechanics of driving DSKLEN's double write and the sdc_byte_in_*
// sector-feed handshake, which are properties of this same RTL, not of
// those packages' own findings). `hrm/ch5-audio-dmacon-adkcon`'s Makefile
// was read for the `paula` submodule file list, also not cited as an
// oracle -- see the Makefile's own comment for the (superset, since it
// also compiled paula_audio's tree) list it used, and this package's
// Makefile for the corrected, paula_floppy_fifo-including list.
//
// Fixture: a fresh Vpaula per trial (paula_floppy's dsktrack has no reset
// term -- disclosed rig hazard, not touched by any assertion here, but the
// same fresh-model discipline is followed throughout regardless).
//
// Test class: HRM_CONFORMANCE
//
// Not turned into assertions: none. All thirteen claims map onto A1-A6
// (see SPECIFICATION.md "Every claim accounted for").
//
// MUTATION records: see the bottom of this header, filled in after each
// mutation was actually built and run (build brief: baseline colour must
// stand beside the mutation result).

#include <verilated.h>
#include "Vpaula.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <functional>
#include <iostream>
#include <string>

namespace {
const char* g_mutate = nullptr;
inline bool mutating(const char* id) {
    return g_mutate != nullptr && std::strcmp(g_mutate, id) == 0;
}
std::string hex4(uint32_t v) {
    char b[8];
    std::snprintf(b, sizeof b, "%04X", (unsigned)(v & 0xFFFF));
    return std::string(b);
}

// Register addresses, as reg_address_in[8:1] (the $DFFxxx offset right-
// shifted by 1) -- paula.v's own parameters DMACON=9'h096, ADKCON=9'h09e,
// ADKCONR=9'h010, and paula_floppy.v's DSKLEN=9'h024, DSKDATR=9'h008.
const uint8_t ADDR_NONE    = 0x00;
const uint8_t ADDR_ADKCONR = 0x08;  // $010 >> 1
const uint8_t ADDR_DSKDATR = 0x04;  // $008 >> 1
const uint8_t ADDR_DSKLEN  = 0x12;  // $024 >> 1
const uint8_t ADDR_DMACON  = 0x4B;  // $096 >> 1
const uint8_t ADDR_ADKCON  = 0x4F;  // $09E >> 1

// ADKCON/DMACON both use the chipset SET/CLR convention (paula.v:182-195,
// :159-171): bit 15 of the write selects OR (set, 1) or AND-NOT (clear, 0)
// against the named field bits.
const uint16_t SETCLR = 0x8000;
const uint16_t ADK_BIT10_WORDSYNC = 0x0400;
const uint16_t ADK_BIT14_PRECOMP1 = 0x4000;
const uint16_t ADK_BIT13_PRECOMP0 = 0x2000;
const uint16_t ADK_PRECOMP_PAIR   = ADK_BIT14_PRECOMP1 | ADK_BIT13_PRECOMP0;
const uint16_t ADK_BIT12_MFMPREC  = 0x1000;
const uint16_t ADK_BIT9_MSBSYNC   = 0x0200;
const uint16_t ADK_BIT8_FAST      = 0x0100;
const uint16_t DMACON_MASTER_DMAEN = 0x0200;  // data_in[9]
const uint16_t DMACON_DSKEN        = 0x0010;  // data_in[4]
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

// A fresh Verilated model per trial (paula_floppy's dsktrack has no reset
// term; the same discipline used by the sibling ch8 floppy packages).
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
    }

    // Advance until exactly n qualifying (clk7_en-active) rising edges have
    // been applied. clk7_en, as read here, is the value that will gate the
    // UPCOMING tick()'s rising edge (set by the previous tick's tail).
    void settle_edges(int n) {
        int seen = 0;
        while (seen < n) {
            bool q = dut->clk7_en;
            tick();
            if (q) seen++;
        }
    }

    bool wait_until(const std::function<bool()>& pred, int max_edges) {
        for (int i = 0; i < max_edges; i++) {
            if (pred()) return true;
            settle_edges(1);
        }
        return pred();
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
    // further qualifying edge repeats it (matters for DSKLEN's double-write
    // arming, where a lingering address would re-arm on every later edge).
    void write_reg(uint8_t addr, uint16_t value) {
        dut->reg_address_in = addr;
        dut->data_in = value;
        settle_edges(1);
        dut->reg_address_in = ADDR_NONE;
        dut->data_in = 0;
        settle_edges(2);
    }

    uint16_t read_reg(uint8_t addr) {
        dut->reg_address_in = addr;
        dut->eval();
        uint16_t v = (uint16_t)dut->data_out;
        dut->reg_address_in = ADDR_NONE;
        dut->eval();
        return v;
    }

    // Pop exactly one word from DSKDATR's FIFO (busrd & dmaon triggers
    // fifo_rd while held), returning the word that was at the head before
    // the pop.
    uint16_t dskdatr_pop() {
        dut->reg_address_in = ADDR_DSKDATR;
        dut->eval();
        uint16_t v = (uint16_t)dut->data_out;
        settle_edges(1);
        dut->reg_address_in = ADDR_NONE;
        settle_edges(1);
        return v;
    }

    // Stream one full 512-byte SD sector into the read-direction ingest
    // path: wait for sdc_rd to assert, ack via sdc_busy, then present
    // sdc_byte_in_strobe for all 512 byte addresses. Content is arbitrary --
    // the assertions in this package only ever look at the sync mark / raw
    // preamble words (word-count 0-3), which paula_floppy synthesises from
    // fixed constants, not from the fed bytes.
    void feed_sector_read(const uint8_t* bytes /* 512 */) {
        wait_until([&]() { return dut->sdc_rd != 0; }, 60);
        dut->sdc_busy = 1;
        settle_edges(2);
        dut->sdc_busy = 0;
        settle_edges(1);
        for (int i = 0; i < 512; i++) {
            dut->sdc_byte_addr = i;
            dut->sdc_byte_in_data = bytes[i];
            dut->sdc_byte_in_strobe = 1;
            settle_edges(1);
            dut->sdc_byte_in_strobe = 0;
            settle_edges(1);
        }
    }

    // Arm disk read DMA at the `paula` boundary: DMACON (master DMAEN +
    // DSKEN) once, then DSKLEN's double write (direction bit 14 clear =
    // read), length in words.
    void arm_disk_read_dma(uint16_t length_words) {
        write_reg(ADDR_DMACON, SETCLR | DMACON_MASTER_DMAEN | DMACON_DSKEN);
        uint16_t dsklen_val = SETCLR | (length_words & 0x3FFF);
        write_reg(ADDR_DSKLEN, dsklen_val);
        write_reg(ADDR_DSKLEN, dsklen_val);
    }
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    // ========================================================================
    // LIVENESS -- no claim id, no manual page: the build brief's own
    // requirement that the fixture prove it can deliver a known, distinct
    // word before any read-based assertion is trusted. Two distinct, known
    // ADKCON writes (set then clear the same recognisable pattern) must
    // arrive, unmodified and mutually distinguishable, at ADKCONR.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.write_reg(ADDR_ADKCON, SETCLR | 0x1234);
        uint16_t v1 = rig.read_reg(ADDR_ADKCONR);
        checks.note("LIVENESS-WORD1: ADKCON<=0x9234 (set 0x1234), ADKCONR readback=0x" + hex4(v1));

        // MUTATE=LIVENESS: drive the second write to SET the same pattern
        // again instead of clearing it, so the two readings collide.
        const bool mutLive = mutating("LIVENESS");
        uint16_t word2 = mutLive ? (SETCLR | 0x1234) : 0x1234;
        rig.write_reg(ADDR_ADKCON, word2);
        uint16_t v2 = rig.read_reg(ADDR_ADKCONR);
        checks.note("LIVENESS-WORD2: ADKCON<=0x" + hex4(word2) + ", ADKCONR readback=0x" + hex4(v2));

        checks.expect(v1 == 0x1234 && v2 == (mutLive ? 0x1234 : 0x0000) && v1 != v2,
            "LIVENESS (fixture delivers two distinct known words through ADKCON/ADKCONR before any proposition is "
            "graded): ADKCONR=0x" + hex4(v1) + " then 0x" + hex4(v2));
    }

    // ========================================================================
    // A2 -- opus:8-appA-adkcon.c8/sol:8-appa-adkr.c5 (p.265); opus:8-appB-
    // adkconr.c5/sol:8-appb-adkconr.c6 (p.289). ADKCONR's read function
    // reports real disk-control state: the one disk-relevant bit (10,
    // WORDSYNC) that ADKCON's write side can actually change is correctly
    // reported back through ADKCONR, set and cleared.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        // MUTATE=A2: write bit 11 (UARTBRK, not a disk bit) instead of bit
        // 10, while the check still looks at bit 10 and still expects it
        // set.
        uint16_t setbit = mutating("A2") ? 0x0800 : ADK_BIT10_WORDSYNC;
        rig.write_reg(ADDR_ADKCON, SETCLR | setbit);
        uint16_t v_set = rig.read_reg(ADDR_ADKCONR);
        rig.write_reg(ADDR_ADKCON, ADK_BIT10_WORDSYNC);  // clear bit 10 regardless
        uint16_t v_clr = rig.read_reg(ADDR_ADKCONR);
        checks.note("A2-STIMULUS: ADKCON bit written=0x" + hex4(setbit) + " -> ADKCONR=0x" + hex4(v_set) +
                    "; then ADKCON bit10 cleared -> ADKCONR=0x" + hex4(v_clr));
        checks.expect(((v_set & ADK_BIT10_WORDSYNC) != 0) && ((v_clr & ADK_BIT10_WORDSYNC) == 0),
            "A2-ADKCONR-DISK-READ (opus:8-appA-adkcon.c8/sol:8-appa-adkr.c5 p.265 'ADKCONR ... Audio, disk, control "
            "read'; opus:8-appB-adkconr.c5/sol:8-appb-adkconr.c6 p.289 'ADKCONR ... Audio, disk control register "
            "read'): ADKCONR reports the real disk-control bit (10, WORDSYNC) correctly set and cleared -- "
            "ADKCONR=0x" + hex4(v_set) + " with bit10 set, 0x" + hex4(v_clr) + " with it cleared");
    }

    // ========================================================================
    // A3 -- opus:8-appA-adkcon.c11 (p.265, "14-13  PRECOMP 1-0"). Position
    // and width only, per the claim's own split ("what the field DOES ...
    // is not asserted"): bits 14-13 store and read back as a pair, and
    // nothing outside that pair is disturbed.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        // MUTATE=A3: write only bit 13 of the pair instead of both, while
        // the check still expects the full pair (0x6000) to read back.
        uint16_t stim = mutating("A3") ? ADK_BIT13_PRECOMP0 : ADK_PRECOMP_PAIR;
        rig.write_reg(ADDR_ADKCON, SETCLR | stim);
        uint16_t v_set = rig.read_reg(ADDR_ADKCONR);
        rig.write_reg(ADDR_ADKCON, ADK_PRECOMP_PAIR);  // clear both, regardless
        uint16_t v_clr = rig.read_reg(ADDR_ADKCONR);
        checks.note("A3-STIMULUS: ADKCON<=set 0x" + hex4(stim) + " -> ADKCONR=0x" + hex4(v_set) +
                    "; then pair cleared -> ADKCONR=0x" + hex4(v_clr));
        checks.expect(v_set == ADK_PRECOMP_PAIR && v_clr == 0x0000,
            "A3-PRECOMP-FIELD-POSITION (opus:8-appA-adkcon.c11 p.265, quote '            14-13  PRECOMP 1-0'): "
            "bits 14-13 store and read back as a pair, exactly (ADKCONR=0x" + hex4(v_set) +
            " after setting both, nothing outside the pair set; 0x" + hex4(v_clr) + " after clearing both)");
    }

    // ========================================================================
    // A4 -- sol:8-appa-adkbit.c3 (p.265, PRECOMP1, bit 14). Individual
    // storage: bit 14 alone stores and reads back, with bit 13 (its
    // immediate neighbour, per A3) left undisturbed.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        // MUTATE=A4: also set bit 13 (the neighbour) while the check still
        // expects the register to read back as EXACTLY bit 14 alone.
        uint16_t stim = mutating("A4") ? ADK_PRECOMP_PAIR : ADK_BIT14_PRECOMP1;
        rig.write_reg(ADDR_ADKCON, SETCLR | stim);
        uint16_t v_set = rig.read_reg(ADDR_ADKCONR);
        rig.write_reg(ADDR_ADKCON, ADK_PRECOMP_PAIR);
        uint16_t v_clr = rig.read_reg(ADDR_ADKCONR);
        checks.note("A4-STIMULUS: ADKCON<=set 0x" + hex4(stim) + " -> ADKCONR=0x" + hex4(v_set) +
                    "; cleared -> ADKCONR=0x" + hex4(v_clr));
        checks.expect(v_set == ADK_BIT14_PRECOMP1 && v_clr == 0x0000,
            "A4-PRECOMP1-BIT14-STORES (sol:8-appa-adkbit.c3 p.265, quote '            14-13  PRECOMP 1-0', bit 14 "
            "half): bit 14 alone stores and reads back (ADKCONR=0x" + hex4(v_set) +
            ", bit 13 undisturbed) and clears (0x" + hex4(v_clr) + ")");
    }

    // ========================================================================
    // A5 -- sol:8-appa-adkbit.c33 (p.265, PRECOMP0, bit 13). Individual
    // storage: bit 13 alone stores and reads back, with bit 14 left
    // undisturbed.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        // MUTATE=A5: also set bit 14 (the neighbour) while the check still
        // expects EXACTLY bit 13 alone.
        uint16_t stim = mutating("A5") ? ADK_PRECOMP_PAIR : ADK_BIT13_PRECOMP0;
        rig.write_reg(ADDR_ADKCON, SETCLR | stim);
        uint16_t v_set = rig.read_reg(ADDR_ADKCONR);
        rig.write_reg(ADDR_ADKCON, ADK_PRECOMP_PAIR);
        uint16_t v_clr = rig.read_reg(ADDR_ADKCONR);
        checks.note("A5-STIMULUS: ADKCON<=set 0x" + hex4(stim) + " -> ADKCONR=0x" + hex4(v_set) +
                    "; cleared -> ADKCONR=0x" + hex4(v_clr));
        checks.expect(v_set == ADK_BIT13_PRECOMP0 && v_clr == 0x0000,
            "A5-PRECOMP0-BIT13-STORES (sol:8-appa-adkbit.c33 p.265, quote '            14-13  PRECOMP 1-0', bit 13 "
            "half): bit 13 alone stores and reads back (ADKCONR=0x" + hex4(v_set) +
            ", bit 14 undisturbed) and clears (0x" + hex4(v_clr) + ")");
    }

    // ========================================================================
    // A1 and A6 share one pair of trials: a disk read DMA transfer run once
    // with ADKCON bit 10 (WORDSYNC) left clear and once with it set,
    // reading the FIRST word DSKDATR delivers in each case. See the file
    // header for the mechanism read from paula_floppy.v.
    //
    // A1 -- opus:8-appA-adkcon.c4/sol:8-appa-adk.c5 (p.265, "ADKCON ...
    // Audio, disk, control write"); opus:8-appB-adkcon.c5/sol:8-appb-
    // adkcon.c5 (p.290, "ADKCON ... Audio, disk, UART control"). The write
    // function's disk half reaches real disk-side logic: whether WORDSYNC
    // is set changes what the disk data path actually delivers.
    //
    // A6 -- opus:8-appA-adkcon.c20/sol:8-appa-adkbit.c11 (p.265, "10
    // WORDSYNC ... Enables disk read synchronizing on a word equal to DISK
    // SYNC CODE"). The specific mechanism: with WORDSYNC clear, the raw,
    // unfiltered preamble word (0xAAAA) is delivered first; with WORDSYNC
    // set, delivery is held back until the sync word is found, so the
    // FIRST word delivered is the sync mark itself (0x4489), not the
    // preamble.
    // ========================================================================
    {
        uint8_t sector[512];
        std::memset(sector, 0, sizeof sector);  // content irrelevant -- see header

        // Trial 1: WORDSYNC clear (default after reset -- no ADKCON write).
        Rig rig0;
        rig0.do_reset();
        // MUTATE=A1 / MUTATE=A6: set WORDSYNC in the "should be clear" trial
        // too, collapsing the intended contrast between the two trials.
        if (mutating("A1") || mutating("A6")) {
            rig0.write_reg(ADDR_ADKCON, SETCLR | ADK_BIT10_WORDSYNC);
        }
        rig0.arm_disk_read_dma(8);
        rig0.feed_sector_read(sector);
        rig0.wait_until([&]() { return rig0.dut->disk_dmal != 0; }, 200);
        uint16_t word0_wordsync_clear = rig0.dskdatr_pop();

        // Trial 2: WORDSYNC set.
        Rig rig1;
        rig1.do_reset();
        rig1.write_reg(ADDR_ADKCON, SETCLR | ADK_BIT10_WORDSYNC);
        rig1.arm_disk_read_dma(8);
        rig1.feed_sector_read(sector);
        rig1.wait_until([&]() { return rig1.dut->disk_dmal != 0; }, 200);
        uint16_t word0_wordsync_set = rig1.dskdatr_pop();

        checks.note("A1/A6-STIMULUS: first DSKDATR word with WORDSYNC clear=0x" + hex4(word0_wordsync_clear) +
                    ", with WORDSYNC set=0x" + hex4(word0_wordsync_set));

        checks.expect(word0_wordsync_clear != word0_wordsync_set,
            "A1-ADKCON-DISK-WRITE (opus:8-appA-adkcon.c4/sol:8-appa-adk.c5 p.265 'ADKCON ... Audio, disk, control "
            "write'; opus:8-appB-adkcon.c5/sol:8-appb-adkcon.c5 p.290 'ADKCON ... Audio, disk, UART control'): "
            "writing ADKCON's disk-relevant bit changes what the disk data path actually delivers -- first word "
            "0x" + hex4(word0_wordsync_clear) + " (WORDSYNC clear) versus 0x" + hex4(word0_wordsync_set) +
            " (WORDSYNC set) -- the write function's disk half reaches real logic, not a dead bit");

        checks.expect(word0_wordsync_clear == 0xAAAA && word0_wordsync_set == 0x4489,
            "A6-WORDSYNC-BIT10-EFFECT (opus:8-appA-adkcon.c20/sol:8-appa-adkbit.c11 p.265, quote '            10   "
            "  WORDSYNC       Enables disk read synchronizing on a word\\nequal to DISK SYNC CODE, located in "
            "address (3F)*2.'): with WORDSYNC clear the raw, unfiltered preamble word (0x" +
            hex4(word0_wordsync_clear) + ", expected 0xAAAA) is delivered first; with WORDSYNC set, delivery is "
            "held back until the sync word is found, so the first word delivered is the sync mark itself (0x" +
            hex4(word0_wordsync_set) + ", expected 0x4489) -- WORDSYNC is a real synchronizing gate, matching the "
            "page, not a dead bit like PRECOMP/MFMPREC/MSBSYNC/FAST");
    }

    // ========================================================================
    // C8's red half, ungraded: no claim in this package's thirteen asserts
    // an effect for MFMPREC (12), MSBSYNC (9) or FAST (8), so none is
    // graded through expect(). Measured here and reported via note() only,
    // confirming the finding candidates document's own disclosed
    // measurement independently, at this package's own fixture.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        uint16_t bits = ADK_BIT12_MFMPREC | ADK_BIT9_MSBSYNC | ADK_BIT8_FAST;
        rig.write_reg(ADDR_ADKCON, SETCLR | bits);
        uint16_t v_set = rig.read_reg(ADDR_ADKCONR);
        rig.write_reg(ADDR_ADKCON, bits);
        uint16_t v_clr = rig.read_reg(ADDR_ADKCONR);
        checks.note("C8-MFMPREC-MSBSYNC-FAST: bits 12/9/8 store and read back individually (ADKCONR=0x" +
                    hex4(v_set) + " set, 0x" + hex4(v_clr) + " cleared) -- confirmed by reading paula.v in full: "
                    "adkcon's only two fan-outs anywhere in src/minimig-aga are bit 11 (paula.v:211, UART, out of "
                    "this package's scope) and bit 10 (paula.v:269, graded above as A1/A6). Bits 12, 9 and 8 drive "
                    "no submodule port and reach no logic; this is storage with no consumer, not asserted by any "
                    "claim in this package's thirteen and therefore not graded through expect().");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}

// MUTATION LIVENESS stimulus red: the second ADKCON write set the same
// pattern again instead of clearing it, so the two ADKCONR readings
// collide (0x1234 twice instead of 0x1234 then 0x0000) (MUTATE=LIVENESS).
// MUTATION A2-ADKCONR-DISK-READ stimulus red: bit 11 (UARTBRK) was written
// instead of bit 10 (WORDSYNC), while the check still reads bit 10 and
// still expects it set (MUTATE=A2).
// MUTATION A3-PRECOMP-FIELD-POSITION stimulus red: only bit 13 of the pair
// was written instead of both bits 14 and 13, while the check still
// expects the full pair (0x6000) to read back (MUTATE=A3).
// MUTATION A4-PRECOMP1-BIT14-STORES stimulus red: bit 13 (the neighbour)
// was also set alongside bit 14, while the check still expects the
// register to read back as exactly bit 14 alone (MUTATE=A4).
// MUTATION A5-PRECOMP0-BIT13-STORES stimulus red: bit 14 (the neighbour)
// was also set alongside bit 13, while the check still expects the
// register to read back as exactly bit 13 alone (MUTATE=A5).
// MUTATION A1-ADKCON-DISK-WRITE stimulus red: WORDSYNC was also set in the
// trial that should have left it clear, collapsing the two trials to the
// same stimulus, while the check still expects the two first-delivered
// words to differ (MUTATE=A1).
// MUTATION A6-WORDSYNC-BIT10-EFFECT stimulus red: WORDSYNC was also set in
// the trial that should have left it clear, so both trials deliver the
// sync mark first, while the check still expects the WORDSYNC-clear trial
// to deliver the raw preamble word (0xAAAA) (MUTATE=A6).
