// Test purpose: DSKBYTR ($DFF01A) -- the disk data-and-status read
//   register: its identity as a real, addressable register (p.247, p.248),
//   the DSKBYT bit set-on-receipt/cleared-on-read behaviour (p.248, p.278),
//   DMAON (p.248, p.278), DISKWRITE (p.248, p.278), WORDEQUAL and the
//   "sector-searching facilities" it is printed to expose (p.247, p.248,
//   p.278), the currently-unused bits (p.248, p.278), the DATA byte
//   (p.248, p.278), and the "synchronize to rotation" framing (p.248).
//   Package ch8-dskbytr-status-register, fixture paula_floppy.v +
//   paula_floppy_fifo.v (paula_floppy_ebr.v deliberately excluded -- it
//   declares a second `module paula_floppy`, a rig hazard given by the
//   build brief).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 247, 248, 278.
// Non-goals: DSKSYNC's own interrupt-timing precision (the "as little as
//   two microseconds" clause -- no port here carries real time units, per
//   the claims file's own split, recorded outside this package's 48
//   claims); the "don't depend on read value" programmer-advice clause of
//   the unused-bits row (also outside this package's 48); DMACON's own
//   DSKEN/DMAEN half of the DMAON condition (this fixture has no DMACON
//   port -- declined below, needs the wider `paula` boundary); DSKDATR
//   ($008) and the FIFO/MFM-encoder pipeline behind it (a different
//   register, a different package's subject -- driven here only as an
//   SD-byte delivery mechanism in A9, never asserted on).
//
// CLAIM SOURCE:
//   C:\code4nanomig\code\hrm-testpipeline\work\ch8-s2-build\
//   ch8-dskbytr-status-register.jsonl -- 48 claim records.
//
// BOUNDARY: nanomig/src/minimig-aga/paula_floppy.v module header (read
//   directly by this agent AFTER SPECIFICATION.md was committed, per the
//   build brief's order of work -- no port name, register address or RTL
//   line appears anywhere in that file) -- ports clk, clk7_en, clk7n_en,
//   reset, ntsc, sof, enable, reg_address_in[8:1], data_in[15:0],
//   data_out[15:0], dmal, dmas, _step, direc, _sel[3:0], side, _motor,
//   _track0, _change, _ready, _wprot, index, blckint, syncint, wordsync,
//   fdd_led, floppy_drives[1:0], floppy_wrprot, and the sdc_* SD-card
//   interface (sdc_image_mounted, sdc_image_size, sdc_rd, sdc_wr,
//   sdc_sector, sdc_busy, sdc_done, sdc_byte_in_strobe, sdc_byte_addr,
//   sdc_byte_in_data, sdc_byte_out_data).
//
// METHODOLOGY, established by reading paula_floppy.v directly:
//
// (1) DSKBYTR ($01A, reg_address_in==DSKBYTR>>1) is a pure combinational
//   mux (paula_floppy.v:699): data_out = {1'b1 (DSKBYT), (trackrd|trackwr)
//   (DMAON), dsklen[14] (DISKWRITE), 1'b1 (WORDEQUAL), 4'b0000 (bits 11-8),
//   8'h00 (DATA)} when addressed, else 16'h0000. Bits 15, 12, 11-8 and 7-0
//   are literal constants in that expression -- no signal anywhere in the
//   module can move them, confirming the build brief's disclosed
//   background (candidate C6) directly against the source. Only bits 14
//   and 13 are ever anything other than their fixed value.
//
// (2) DISKWRITE (bit 13) is dsklen[14], and DSKLEN's low 15 bits latch on
//   ANY single write while clk7_en is high (paula_floppy.v:711-721, no
//   double-write gate on this half) -- so DISKWRITE responds to ONE
//   DSKLEN write, unlike DMAON below.
//
// (3) DMAON (bit 14) is (trackrd|trackwr), which the main disk state
//   machine (paula_floppy.v:994-1037) holds at 0 in DISKDMA_IDLE and only
//   raises once dskstate has actually transitioned to DISKDMA_ACTIVE.
//   That transition needs dma_active = dmaen && !lenzero && enable
//   (paula_floppy.v:248) to be true for one clk7_en edge before the NEXT
//   edge moves dskstate. `dmaen` itself needs the double DSKLEN write the
//   build brief's own rig-hazard note already named
//   (paula_floppy.v:735-743: "start disk dma if second write in a row with
//   dsklen[15] set") -- confirmed directly against the source, not
//   re-derived. `enable` is this module's own external port; at the
//   paula_floppy boundary it is simply held asserted throughout (it
//   represents DMACON's own product at the wider `paula` boundary this
//   package was not assigned -- see the DMAON-DMACON-HALF decline below).
//
// (4) The SD-byte-feed protocol reused for A9's DATA-field measurement
//   (request a sector on `dma_active` entering state 0 with the read
//   direction, wait for `sdc_busy`, then drive 512 bytes on
//   sdc_byte_in_strobe/sdc_byte_addr/sdc_byte_in_data, paula_floppy.v:
//   263-336) is the same protocol the sibling package hrm/paula_floppy's
//   own accepted test already exercises end-to-end on this exact RTL --
//   its timing (poll for sdc_rd, assert sdc_busy, drive 512 strobed
//   bytes) is reused here, not re-derived from this package's own reading,
//   for exactly the byte-delivery mechanics; DSKDATR/the FIFO/the MFM
//   encoder downstream of that feed are that sibling's own subject and are
//   never read by this file's own assertions -- only DSKBYTR is.
//
// RIG HAZARDS, both already disclosed by the build brief and confirmed
//   directly against the source now that it is open:
//   - `dsktrack` (paula_floppy.v:129) carries no reset term
//     (paula_floppy.v:671-680 only ever increments/decrements it, never
//     clears it) -- every assertion below allocates its own fresh
//     Vpaula_floppy, never reusing one across assertions or even across
//     the two/three trials inside one assertion.
//   - DSKLEN needs the double write (paula_floppy.v:743) to arm dmaen; a
//     single write leaves DISKWRITE (bit 13, dsklen[14]) already following
//     the write, but DMAON (bit 14) clear -- confirmed directly, matching
//     the build brief's own disclosed measurement (0x9000/fdd_led=0 after
//     one write, 0xD000/fdd_led=1/sdc_rd=1 after two).
//
// CONTROL CASES: A2 (register genuinely address-decoded), A3 (DISKWRITE
//   genuinely mirrors DSKLEN), A4 (DMAON genuinely responds to DSKLEN's own
//   double-write arming) and A8 (the unused bits genuinely carry no
//   function) are this package's PASS-shaped measurements, all on the
//   exact same register and the exact same read_reg() this file's
//   suspect assertions (A5, A6, A7, A9, A10) use -- proving the harness's
//   read path is live on the identical basis the suspect assertions rely
//   on to mean anything.
//
// ON RED-BASELINE MUTATION, per the build brief: "a stimulus mutation
//   proves an assertion CAN fail, and one already failing cannot fail
//   harder." A5, A6, A7, A9 and A10 are all expected RED (DSKBYTR's own
//   constant bits per candidate C6). Each one's mutation record instead
//   substitutes a DIFFERENT, PROVEN-LIVE mechanism from elsewhere in this
//   same file -- DISKWRITE (bit 13, proven live by A3) or DMAON (bit 14,
//   proven live by A4) -- through the SAME "extract a bit, compare it
//   against what the state genuinely requires" comparator, and shows THAT
//   goes GREEN: proof the comparator is not vacuously red regardless of
//   what it reads, exactly the shape ch5-audio-dmacon-adkcon's own
//   DMACONR-NOT-DECODED-BY-PAULA mutation record already established as
//   this suite's precedent for a red-baseline assertion.
//
// MUTATION LOG (every entry actually run by this agent under WSL/
//   Verilator: `make clean && make MUT=<NAME>` -> run -> confirm the named
//   assertion, and only that one, newly flips colour against the baseline
//   -> `make clean && make` -> confirm the baseline exactly restored,
//   before moving to the next one):
//
// MUTATION FIXTURE-LIVENESS-KNOWN-DISTINCT-DSKLEN-STATES stimulus red: the
//   first trial's DSKLEN write changed from 0x4000 to 0x0000 (the same
//   value the second trial already writes), so both trials now write the
//   identical value -> the two readings collapse to the same 0x9000
//   instead of staying distinct -> only this assertion newly red.
//   Reverted.
// MUTATION DSKBYTR-IS-A-REAL-READABLE-REGISTER stimulus red: the "read at
//   the decoded DSKBYTR address" trial's address changed to the unmapped
//   address used by the OTHER half of this same assertion -> that half's
//   own decoded-nonzero expectation now reads 0x0000 -> only this
//   assertion newly red. Reverted.
// MUTATION DISKWRITE-BIT13-TRACKS-DSKLEN-WRITE-BIT stimulus red: the
//   "should read 1" trial's DSKLEN write changed from 0x4000 to 0x0000 (no
//   WRITE bit) -> bit 13 stays 0 where 1 was expected -> only this
//   assertion newly red. Reverted.
// MUTATION DMAON-BIT14-TRACKS-DSKLEN-DMA-ENABLE stimulus red: the second,
//   arming DSKLEN write was skipped, leaving only the single first write
//   -> dmaen never latches, dskstate never leaves DISKDMA_IDLE, DMAON
//   stays 0 where 1 was expected -> only this assertion newly red.
//   Reverted.
// MUTATION DSKBYT-NOT-SET-ONLY-ON-RECEIPT stimulus green: with nothing
//   else touched, the SAME "compare an extracted bit to what the state
//   genuinely demands" comparator was pointed at bit 13 (DISKWRITE) after
//   a DSKLEN=0x4000 write, expecting 1, INSTEAD of bit 15 (DSKBYT) after a
//   fresh reset with zero bytes ever received, expecting 0 -> bit 13
//   genuinely reads 1 (DISKWRITE responds to a real write, proven live by
//   A3) -> only this assertion newly green. Reverted.
// MUTATION DSKBYT-NOT-CLEARED-ON-READ stimulus green: a genuine DSKLEN=
//   0x4000 write was inserted between the two reads, and the comparator
//   was pointed at bit 13 instead of bit 15 -> the two reads now genuinely
//   differ (0 then 1) where the real assertion's two reads never do ->
//   only this assertion newly green. Reverted.
// MUTATION WORDEQUAL-DOES-NOT-TRACK-DSKSYNC-MATCH stimulus green: the
//   comparator was pointed at bit 14 (DMAON) instead of bit 12 (WORDEQUAL)
//   for the same idle-vs-active-transfer pair -> bit 14 genuinely differs
//   (0 idle, 1 once DMA is armed, proven live by A4) -> only this
//   assertion newly green. Reverted.
// MUTATION UNUSED-BITS-11-8-CARRY-NO-FUNCTION none impossible: bits 11-8
//   of dskbytr are the literal constant 4'b0000 inside paula_floppy.v:699's
//   own ternary expression -- no combination of any port this module
//   exposes can reach that literal and change it, confirmed by reading the
//   line directly. Unfalsifiable at this boundary; there is no stimulus to
//   run.
// MUTATION DATA-FIELD-DOES-NOT-REFLECT-INCOMING-BYTES stimulus green: the
//   comparator was pointed at bit 13, comparing its value after a DSKLEN=
//   0x4000 write against its value after a DSKLEN=0x0000 write (two
//   genuinely different stimuli), INSTEAD of DATA (bits 7-0) after feeding
//   two genuinely different 512-byte SD sector patterns -> bit 13 reads
//   two genuinely different values (1 then 0) where the real assertion's
//   two SD feeds never move DATA at all -> only this assertion newly
//   green. Reverted.
// MUTATION DSKBYTR-DOES-NOT-VARY-WITH-ROTATION-OR-DIRECTION stimulus green:
//   the comparator was pointed at bit 13, sampled before and after a
//   genuine DSKLEN=0x4000 write interposed during the same elapsed-idle
//   window, INSTEAD of DSKBYTR's own value sampled before and after ~2000
//   idle clk7_en cycles with the drive selected and the motor running ->
//   bit 13 genuinely differs -> only this assertion newly green. Reverted.
//
// RESULTS (WSL/Verilator 5.052): 5 passed, 5 failed. A1 (fixture
// liveness), A2 (DSKBYTR-IS-A-REAL-READABLE-REGISTER), A3
// (DISKWRITE-BIT13-TRACKS-DSKLEN-WRITE-BIT), A4
// (DMAON-BIT14-TRACKS-DSKLEN-DMA-ENABLE) and A8
// (UNUSED-BITS-11-8-CARRY-NO-FUNCTION) are GREEN. A5
// (DSKBYT-NOT-SET-ONLY-ON-RECEIPT), A6 (DSKBYT-NOT-CLEARED-ON-READ), A7
// (WORDEQUAL-DOES-NOT-TRACK-DSKSYNC-MATCH), A9
// (DATA-FIELD-DOES-NOT-REFLECT-INCOMING-BYTES) and A10
// (DSKBYTR-DOES-NOT-VARY-WITH-ROTATION-OR-DIRECTION) are RED -- all five
// genuine findings, matching the build brief's disclosed background
// (candidate C6) exactly, none adjusted to pass. Final rebuild after the
// last mutation's revert restored this exact 5/5 baseline (see out.txt).

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vpaula_floppy.h"

namespace {

// Fixed per the build brief: expect() grades, note() measures without
// grading, record() declines with a reason and is never a measurement.
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

// Register word addresses (reg_address_in carries bits [8:1] of the
// manual's own byte address), confirmed directly against
// paula_floppy.v:120-124's own parameter declarations.
constexpr int A_DSKDATR  = 0x008 >> 1;  // read path, not this package's subject -- used only to drive the SD-feed protocol in A9, never read by an assertion.
constexpr int A_DSKLEN   = 0x024 >> 1;
constexpr int A_DSKDAT   = 0x026 >> 1;  // write-only; used as a plainly-not-DSKBYTR unmapped-shaped address in A2's control half.
constexpr int A_DSKBYTR  = 0x01a >> 1;
constexpr int A_DSKSYNC  = 0x07e >> 1;
constexpr int A_UNMAPPED = 0x000;       // decodes to none of the above.

std::string hex16(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "0x%04x", v);
    return std::string(buf);
}

int bitn(uint16_t v, int n) { return (v >> n) & 1; }

class Harness {
public:
    Harness() : dut(new Vpaula_floppy) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->clk7n_en = 0;
        dut->reset = 1;
        dut->ntsc = 0;
        dut->sof = 0;
        dut->enable = 1;  // represents DMACON's own product, held asserted at this boundary -- see METHODOLOGY (3).
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->_step = 1;
        dut->direc = 1;
        dut->_sel = 0xF;      // no drive selected
        dut->side = 0;
        dut->_motor = 1;      // motor off (active low)
        dut->wordsync = 0;
        dut->floppy_drives = 1;
        dut->floppy_wrprot = 0;
        dut->sdc_image_mounted = 0;
        dut->sdc_image_size = 0;
        dut->sdc_busy = 0;
        dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0;
        dut->sdc_byte_addr = 0;
        dut->sdc_byte_in_data = 0;
        for (int i = 0; i < 32; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 32; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        clk_phase = (clk_phase + 1) % 4;
        dut->clk7_en = (clk_phase == 0);
        dut->clk7n_en = (clk_phase == 2);
    }
    void ticks(int n) { for (int i = 0; i < n; ++i) tick(); }

    // Holds addr/data for one full clk7_en period (>=1 qualified edge),
    // matching every register write in paula_floppy.v being gated on
    // `if (clk7_en)` (METHODOLOGY, confirmed at paula_floppy.v:703-743).
    void write_reg(int addr_w, uint16_t data) {
        dut->reg_address_in = static_cast<uint16_t>(addr_w & 0x1FF);
        dut->data_in = data;
        ticks(4);
        dut->reg_address_in = 0;
        dut->data_in = 0;
        ticks(4);
    }

    // data_out is a plain combinational mux (paula_floppy.v:580,699) --
    // no clock edge required.
    uint16_t read_reg(int addr_w) {
        dut->reg_address_in = static_cast<uint16_t>(addr_w & 0x1FF);
        dut->eval();
        uint16_t v = static_cast<uint16_t>(dut->data_out);
        dut->reg_address_in = 0;
        dut->eval();
        return v;
    }

    // Selects drive 0 and turns its motor on, needed for the SD-feed
    // protocol in A9 (sdc_rd <= ~_sel, paula_floppy.v:285 -- with no
    // drive selected, ~_sel is all zero and no sector is ever requested)
    // and for A10's "motor running" scenario. motor_on[0] latches on the
    // falling edge of _sel[0] (paula_floppy.v:625-632), so this must run
    // for at least two clk7_en cycles after the change.
    void select_drive0_motor_on() {
        dut->_sel = 0xF;
        ticks(8);
        dut->_sel = 0xE;
        dut->_motor = 0;
        ticks(8);
    }

    // Arms DMA in the given direction (write_dir=false: read, true: write)
    // with the given 14-bit length, via the double-write DSKLEN needs
    // (paula_floppy.v:735-743, a rig hazard the build brief already
    // disclosed and this METHODOLOGY confirms directly). Leaves enough
    // margin (settle_ticks) for dskstate to actually transition into
    // DISKDMA_ACTIVE afterward.
    void arm_dma(bool write_dir, uint16_t length, int settle_ticks = 40) {
        uint16_t word = 0x8000 | (write_dir ? 0x4000 : 0x0000) | (length & 0x3FFF);
        write_reg(A_DSKLEN, word);
        write_reg(A_DSKLEN, word);
        ticks(settle_ticks);
    }

    Vpaula_floppy* dut;
    int clk_phase = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ================= A1: FIXTURE-LIVENESS =================
    // Mandatory precondition, no claim uid: two distinct, known DSKLEN
    // states must arrive at DSKBYTR distinctly before anything else here
    // is graded.
    {
        Harness h1;
#ifdef MUT_FIXTURE_LIVENESS
        h1.write_reg(A_DSKLEN, 0x0000);  // MUTATION: same value the second trial already writes.
#else
        h1.write_reg(A_DSKLEN, 0x4000);
#endif
        uint16_t v1 = h1.read_reg(A_DSKBYTR);

        Harness h2;  // fresh model -- dsktrack carries no reset term.
        h2.write_reg(A_DSKLEN, 0x0000);
        uint16_t v2 = h2.read_reg(A_DSKBYTR);

        bool ok = (v1 == 0xB000) && (v2 == 0x9000) && (v1 != v2);
        checks.expect(ok,
            "FIXTURE-LIVENESS-KNOWN-DISTINCT-DSKLEN-STATES: a DSKLEN write "
            "with the WRITE bit set (0x4000) makes DSKBYTR read 0xB000, and "
            "a distinct, separately-reset trial writing DSKLEN=0x0000 makes "
            "it read 0x9000 -- two distinct, known register states arrive "
            "at DSKBYTR distinctly, proving this fixture's read/write bus "
            "is live before anything else in this file is trusted. "
            "Measured v1=" + hex16(v1) + " v2=" + hex16(v2) + ".");
    }

    // ================= A2: DSKBYTR-IS-A-REAL-READABLE-REGISTER =================
    // (p.247 opus:8-dma.c31, sol:8-dskdma.c9; p.248 opus:8-dskbytr.c1,
    //  sol:8-dskbytr.c1; p.278 opus:8-appA-dskbytr.c5)
    {
        Harness h;
#ifdef MUT_DSKBYTR_IS_A_REAL_READABLE_REGISTER
        uint16_t idle_at_dskbytr = h.read_reg(A_UNMAPPED);  // MUTATION: read the wrong (unmapped) address instead of DSKBYTR.
#else
        uint16_t idle_at_dskbytr = h.read_reg(A_DSKBYTR);
#endif
        uint16_t idle_at_unmapped = h.read_reg(A_UNMAPPED);
        h.arm_dma(/*write_dir=*/false, /*length=*/10);
        uint16_t during_active = h.read_reg(A_DSKBYTR);

        bool decoded_idle = bitn(idle_at_dskbytr, 15) == 1;
        bool not_decoded_elsewhere = idle_at_unmapped == 0x0000;
        bool decoded_active = bitn(during_active, 15) == 1;
        checks.expect(decoded_idle && not_decoded_elsewhere && decoded_active,
            "DSKBYTR-IS-A-REAL-READABLE-REGISTER (p.247 opus:8-dma.c31, "
            "sol:8-dskdma.c9; p.248 opus:8-dskbytr.c1, sol:8-dskbytr.c1; "
            "p.278 opus:8-appA-dskbytr.c5): DSKBYTR is a genuinely decoded "
            "register, not a stuck wire -- reading its own address returns "
            "a defined value (bit 15 set) both idle and mid-transfer, while "
            "reading an address this module does not decode returns "
            "0x0000, proving the mux discriminates by address rather than "
            "returning the same thing regardless of what is read; this is "
            "also the 'register that may be polled to examine the disk "
            "input stream' (p.247) and 'this register is the "
            "disk-microprocessor data buffer' (p.248/p.278) identity half. "
            "Measured idle_at_dskbytr=" + hex16(idle_at_dskbytr) +
            " idle_at_unmapped=" + hex16(idle_at_unmapped) +
            " during_active=" + hex16(during_active) + ".");
    }

    // ================= A3: DISKWRITE-BIT13-TRACKS-DSKLEN-WRITE-BIT =================
    // (p.248 opus:8-dskbytr.c10, sol:8-dskbytr.c14; p.278
    //  opus:8-appA-dskbytr.c12, sol:8-appa-dskbytr.c13)
    {
        Harness h1;
#ifdef MUT_DISKWRITE_BIT13_TRACKS_DSKLEN_WRITE_BIT
        h1.write_reg(A_DSKLEN, 0x0000);  // MUTATION: no WRITE bit, where the assertion expects DISKWRITE=1.
#else
        h1.write_reg(A_DSKLEN, 0x4000);
#endif
        uint16_t v_set = h1.read_reg(A_DSKBYTR);

        Harness h2;
        h2.write_reg(A_DSKLEN, 0x0000);
        uint16_t v_clear = h2.read_reg(A_DSKBYTR);

        bool ok = (bitn(v_set, 13) == 1) && (bitn(v_clear, 13) == 0);
        checks.expect(ok,
            "DISKWRITE-BIT13-TRACKS-DSKLEN-WRITE-BIT (p.248 "
            "opus:8-dskbytr.c10, sol:8-dskbytr.c14; p.278 "
            "opus:8-appA-dskbytr.c12, sol:8-appa-dskbytr.c13): 'The disk "
            "write bit (in DSKLEN) is enabled' / 'Mirror of bit 14 (WRITE) "
            "in DSKLEN' -- writing DSKLEN with the WRITE bit set makes bit "
            "13 (DISKWRITE) read 1, and writing it clear makes bit 13 read "
            "0. Measured v_set=" + hex16(v_set) + " v_clear=" +
            hex16(v_clear) + ".");
    }

    // ================= A4: DMAON-BIT14-TRACKS-DSKLEN-DMA-ENABLE (DSKLEN half) =================
    // (p.248 opus:8-dskbytr.c8 [DSKLEN clause], sol:8-dskbytr.c9 [DSKLEN
    //  clause]; p.278 opus:8-appA-dskbytr.c10 [DSKLEN clause])
    {
        Harness h;
        uint16_t word = 0x8000 | 0x000A;  // read-direction, length=10.
        h.write_reg(A_DSKLEN, word);
#ifdef MUT_DMAON_BIT14_TRACKS_DSKLEN_DMA_ENABLE
        // MUTATION: the arming second write is skipped entirely.
#else
        h.write_reg(A_DSKLEN, word);
#endif
        h.ticks(40);
        uint16_t v = h.read_reg(A_DSKBYTR);

        bool ok = bitn(v, 14) == 1;
        checks.expect(ok,
            "DMAON-BIT14-TRACKS-DSKLEN-DMA-ENABLE (p.248 opus:8-dskbytr.c8 "
            "DSKLEN clause, sol:8-dskbytr.c9 DSKLEN clause; p.278 "
            "opus:8-appA-dskbytr.c10 DSKLEN clause): 'Indicates when DMA is "
            "actually enabled ... This means the DMAEN bit in DSKLEN' / "
            "'Mirror of bit 15 (DMAEN) in DSKLEN' -- graded here only for "
            "the DSKLEN half both pages state identically; the DMACON half "
            "is declined below, out of this fixture's boundary. Two "
            "identical DSKLEN writes with the DMA-enable bit set (the "
            "double write the build brief's own rig-hazard note already "
            "named) bring DMAON (bit 14) up; this package's own measured "
            "DSKLEN<<800A double-write value (0xD000, matching the build "
            "brief's disclosed background) confirms bit 14 set here too. "
            "Measured v=" + hex16(v) + ".");

        checks.record(
            "DMAON-DMACON-HALF, out of this package's boundary (p.248 "
            "opus:8-dskbytr.c8 DMACON clause, sol:8-dskbytr.c9 DMACON "
            "clause; p.278 opus:8-appA-dskbytr.c10 DMACON clause): p.248 "
            "prints 'the DSKEN & DMAEN bits in DMACON' as an additional "
            "requirement for DMAON; p.278 prints only 'ANDed with Bit09 "
            "(DMAEN) in DMACON' -- a page-versus-page difference (p.248 "
            "names DSKEN as well as DMAEN, p.278 names only DMAEN), not a "
            "red against this RTL. This package's assigned fixture is "
            "paula_floppy compiled alone, which has no DMACON port at all "
            "-- DMACON's bits reach this module only pre-combined into the "
            "single `enable` input this file holds asserted throughout "
            "(see the Harness constructor). Grading DMACON's own bit "
            "combination needs the wider `paula` boundary, already "
            "measured by finding candidate C11 in "
            "work/2026-09-15_ch8_s2_finding_candidates.md (DMACON bit 9 "
            "alone leaves DSKBYTR bit 14 clear; bit 9 AND bit 4 are both "
            "required) -- not re-measured here, and not asserted on a "
            "boundary this package does not have.");
    }

    // ================= A5: DSKBYT-BIT15-IS-NOT-SET-ONLY-ON-BYTE-RECEIPT =================
    // (p.248 opus:8-dskbytr.c3, opus:8-dskbytr.c6, sol:8-dskbytr.c3,
    //  sol:8-dskbytr.c7; p.278 opus:8-appA-dskbytr.c7,
    //  opus:8-appA-dskbytr.c8, sol:8-appa-dskbytr.c8, sol:8-appa-dskbytr.c9)
    {
        Harness h;
        // Fresh reset, no DSKLEN ever written, no transfer ever armed --
        // zero bytes have ever been received.
        uint16_t v = h.read_reg(A_DSKBYTR);
        int measured_bit = bitn(v, 15);
        bool ok = measured_bit == 0;

#ifdef MUT_DSKBYT_NOT_SET_ONLY_ON_RECEIPT
        // MUTATION (already-red baseline; substitute a proven-live
        // mechanism through the same comparator, see file header): bit 13
        // (DISKWRITE, proven live by A3) after a genuine DSKLEN=0x4000
        // write, expecting 1, instead of bit 15 after a fresh reset,
        // expecting 0.
        h.write_reg(A_DSKLEN, 0x4000);
        uint16_t v2 = h.read_reg(A_DSKBYTR);
        ok = bitn(v2, 13) == 1;
        measured_bit = bitn(v2, 13);
        v = v2;
#endif

        checks.expect(ok,
            "DSKBYT-NOT-SET-ONLY-ON-RECEIPT (p.248 opus:8-dskbytr.c3, "
            "opus:8-dskbytr.c6, sol:8-dskbytr.c3, sol:8-dskbytr.c7; p.278 "
            "opus:8-appA-dskbytr.c7, opus:8-appA-dskbytr.c8, "
            "sol:8-appa-dskbytr.c8, sol:8-appa-dskbytr.c9): 'As each byte "
            "is received into the register, the DSKBYT bit is set true' / "
            "'indicates that this register contains a valid byte of data' "
            "-- with zero bytes ever received (fresh reset, no transfer "
            "ever armed), bit 15 (DSKBYT) should read 0. Measured "
            "dskbytr=" + hex16(v) + " bit=" + std::to_string(measured_bit) +
            ".");
    }

    // ================= A6: DSKBYT-BIT15-IS-NOT-CLEARED-ON-READ =================
    // (p.248 opus:8-dskbytr.c4, opus:8-dskbytr.c7, sol:8-dskbytr.c8; p.278
    //  opus:8-appA-dskbytr.c9, sol:8-appa-dskbytr.c10)
    {
        Harness h;
        uint16_t read1 = h.read_reg(A_DSKBYTR);
#ifndef MUT_DSKBYT_NOT_CLEARED_ON_READ
        uint16_t read2 = h.read_reg(A_DSKBYTR);
        bool ok = bitn(read1, 15) == 1 && bitn(read2, 15) == 0;
        checks.expect(ok,
            "DSKBYT-NOT-CLEARED-ON-READ (p.248 opus:8-dskbytr.c4, "
            "opus:8-dskbytr.c7, sol:8-dskbytr.c8; p.278 "
            "opus:8-appA-dskbytr.c9, sol:8-appa-dskbytr.c10): 'DSKBYT is "
            "cleared when the DSKBYTR register is read' / '(reset by "
            "reading this register)' -- a second, immediately consecutive "
            "read should show bit 15 cleared relative to the first. "
            "Measured read1=" + hex16(read1) + " read2=" + hex16(read2) +
            ".");
#else
        // MUTATION (already-red baseline; substitute a proven-live
        // mechanism through the same comparator): a genuine DSKLEN=0x4000
        // write inserted between the two reads, comparator pointed at bit
        // 13 instead of bit 15.
        h.write_reg(A_DSKLEN, 0x4000);
        uint16_t read2 = h.read_reg(A_DSKBYTR);
        bool ok = bitn(read1, 13) == 0 && bitn(read2, 13) == 1;
        checks.expect(ok,
            "DSKBYT-NOT-CLEARED-ON-READ (p.248 opus:8-dskbytr.c4, "
            "opus:8-dskbytr.c7, sol:8-dskbytr.c8; p.278 "
            "opus:8-appA-dskbytr.c9, sol:8-appa-dskbytr.c10): "
            "[mutation substitute] Measured read1=" + hex16(read1) +
            " read2=" + hex16(read2) + ".");
#endif
    }

    // ================= A7: WORDEQUAL-DOES-NOT-TRACK-DSKSYNC-MATCH =================
    // (p.247 opus:8-dma.c30, sol:8-dskdma.c8; p.248 opus:8-dskbytr.c11,
    //  sol:8-dskbytr.c15, opus:8-dskbytr.c12, sol:8-dskbytr.c16; p.278
    //  opus:8-appA-dskbytr.c13, sol:8-appa-dskbytr.c14)
    {
        Harness h;
        h.write_reg(A_DSKSYNC, 0xDEAD);  // deliberately mismatching value.
        uint16_t idle = h.read_reg(A_DSKBYTR);
        h.arm_dma(/*write_dir=*/false, /*length=*/10);
        uint16_t active = h.read_reg(A_DSKBYTR);

#ifndef MUT_WORDEQUAL_DOES_NOT_TRACK_DSKSYNC_MATCH
        int idle_bit = bitn(idle, 12);
        int active_bit = bitn(active, 12);
        bool ok = idle_bit != active_bit;
        checks.expect(ok,
            "WORDEQUAL-DOES-NOT-TRACK-DSKSYNC-MATCH (p.247 opus:8-dma.c30, "
            "sol:8-dskdma.c8; p.248 opus:8-dskbytr.c11, sol:8-dskbytr.c15, "
            "opus:8-dskbytr.c12, sol:8-dskbytr.c16; p.278 "
            "opus:8-appA-dskbytr.c13, sol:8-appa-dskbytr.c14): 'Indicates "
            "the DSKSYNC register equals the disk input stream. This bit "
            "is true only while the input stream matches the sync "
            "register' -- and, p.247, 'the disk hardware supports a "
            "limited set of sector-searching facilities' / 'a register "
            "that may be polled to examine the disk input stream', the "
            "concrete mechanism behind both being exactly this bit. "
            "WORDEQUAL (bit 12) should read differently between a state "
            "with no transfer at all and one with a real transfer active "
            "against a deliberately-mismatching DSKSYNC. Measured idle=" +
            hex16(idle) + " active=" + hex16(active) + ".");
#else
        // MUTATION (already-red baseline; substitute a proven-live
        // mechanism through the same comparator): bit 14 (DMAON, proven
        // live by A4) instead of bit 12, for the identical idle-vs-active
        // pair.
        int idle_bit = bitn(idle, 14);
        int active_bit = bitn(active, 14);
        bool ok = idle_bit != active_bit;
        checks.expect(ok,
            "WORDEQUAL-DOES-NOT-TRACK-DSKSYNC-MATCH (p.247 opus:8-dma.c30, "
            "sol:8-dskdma.c8; p.248 opus:8-dskbytr.c11, sol:8-dskbytr.c15, "
            "opus:8-dskbytr.c12, sol:8-dskbytr.c16; p.278 "
            "opus:8-appA-dskbytr.c13, sol:8-appa-dskbytr.c14): "
            "[mutation substitute] Measured idle=" + hex16(idle) +
            " active=" + hex16(active) + ".");
#endif
    }

    // ================= A8: UNUSED-BITS-11-8-CARRY-NO-FUNCTION =================
    // (p.248 opus:8-dskbytr.c13, sol:8-dskbytr.c18; p.278
    //  opus:8-appA-dskbytr.c14, sol:8-appa-dskbytr.c15)
    {
        Harness h;
        uint16_t idle = h.read_reg(A_DSKBYTR);
        h.write_reg(A_DSKLEN, 0x4000);
        uint16_t after_write = h.read_reg(A_DSKBYTR);
        h.arm_dma(/*write_dir=*/false, /*length=*/10);
        uint16_t during_active = h.read_reg(A_DSKBYTR);

        auto bits118 = [](uint16_t v) { return (v >> 8) & 0xF; };
        bool ok = bits118(idle) == 0 && bits118(after_write) == 0 &&
                  bits118(during_active) == 0;
        checks.expect(ok,
            "UNUSED-BITS-11-8-CARRY-NO-FUNCTION (p.248 opus:8-dskbytr.c13, "
            "sol:8-dskbytr.c18; p.278 opus:8-appA-dskbytr.c14, "
            "sol:8-appa-dskbytr.c15): 'Currently unused' / 'Not used' -- "
            "bits 11-8 do not encode any of DSKBYTR's other defined "
            "semantics: they read identically across idle, a DISKWRITE-set "
            "state, and an active DMA-armed transfer. Measured idle=" +
            hex16(idle) + " after_write=" + hex16(after_write) +
            " during_active=" + hex16(during_active) + ".");
    }

    // ================= A9: DATA-FIELD-DOES-NOT-REFLECT-INCOMING-DISK-BYTES =================
    // (p.248 opus:8-dskbytr.c2, opus:8-dskbytr.c14, sol:8-dskbytr.c2,
    //  sol:8-dskbytr.c20; p.278 opus:8-appA-dskbytr.c6,
    //  opus:8-appA-dskbytr.c15, sol:8-appa-dskbytr.c7,
    //  sol:8-appa-dskbytr.c16)
    {
#ifndef MUT_DATA_FIELD_DOES_NOT_REFLECT_INCOMING_BYTES
        auto feed_and_sample = [](uint8_t pattern) -> uint8_t {
            Harness h;  // fresh model -- dsktrack carries no reset term.
            h.select_drive0_motor_on();
            uint16_t word = 0x8000 | 10;  // read-direction, length=10.
            h.write_reg(A_DSKLEN, word);
            h.write_reg(A_DSKLEN, word);
            // Wait for the sector-fetch state machine to request and be
            // acknowledged (paula_floppy.v:263-309: dma_active raises
            // sdc_rd; this testbench answers with sdc_busy, as the
            // sibling paula_floppy package's own accepted test does).
            bool requested = false;
            for (int i = 0; i < 200 && !requested; ++i) {
                h.tick();
                if (h.dut->sdc_rd != 0) requested = true;
            }
            h.dut->sdc_busy = 1;
            h.ticks(20);
            // Drive 512 distinct, known bytes of the chosen pattern.
            for (int i = 0; i < 512; ++i) {
                h.dut->sdc_byte_addr = static_cast<uint16_t>(i);
                h.dut->sdc_byte_in_data = pattern;
                h.dut->sdc_byte_in_strobe = 1;
                h.ticks(4);
                h.dut->sdc_byte_in_strobe = 0;
                h.ticks(4);
            }
            h.dut->sdc_busy = 0;
            h.ticks(40);
            uint16_t v = h.read_reg(A_DSKBYTR);
            return static_cast<uint8_t>(v & 0xFF);
        };

        uint8_t data_after_a = feed_and_sample(0x11);
        uint8_t data_after_b = feed_and_sample(0xEE);
        // The proposition under test is that DATA tracks what was fed --
        // two genuinely distinct known patterns must produce two
        // genuinely distinct DATA readings (NOT "both read 0x00": that
        // would be grading agreement with the RTL's own hardwired
        // behaviour instead of the manual's claim).
        bool ok = (data_after_a != data_after_b);
        checks.expect(ok,
            "DATA-FIELD-DOES-NOT-REFLECT-INCOMING-BYTES (p.248 "
            "opus:8-dskbytr.c2, opus:8-dskbytr.c14, sol:8-dskbytr.c2, "
            "sol:8-dskbytr.c20; p.278 opus:8-appA-dskbytr.c6, "
            "opus:8-appA-dskbytr.c15, sol:8-appa-dskbytr.c7, "
            "sol:8-appa-dskbytr.c16): 'In read mode, data from the disk is "
            "placed into this register one byte at a time' / '7-0 DATA "
            "Disk byte data' -- feeding a full 512-byte sector of a known "
            "pattern (0x11, then, from a fresh model, a distinct 0xEE) "
            "through an armed read-direction transfer should make DATA "
            "(bits 7-0) reflect what was fed, i.e. the two genuinely "
            "distinct feeds should produce two genuinely distinct DATA "
            "readings. Measured data_after_0x11=" + hex16(data_after_a) +
            " data_after_0xEE=" + hex16(data_after_b) + ".");
#else
        // MUTATION (already-red baseline; substitute a proven-live
        // mechanism through the same comparator): compare bit 13 after
        // two genuinely different DSKLEN writes instead of DATA after two
        // genuinely different SD sector feeds.
        Harness h1;
        h1.write_reg(A_DSKLEN, 0x4000);
        uint16_t v1 = h1.read_reg(A_DSKBYTR);
        Harness h2;
        h2.write_reg(A_DSKLEN, 0x0000);
        uint16_t v2 = h2.read_reg(A_DSKBYTR);
        bool ok = (bitn(v1, 13) == 1) && (bitn(v2, 13) == 0);
        checks.expect(ok,
            "DATA-FIELD-DOES-NOT-REFLECT-INCOMING-BYTES (p.248 "
            "opus:8-dskbytr.c2, opus:8-dskbytr.c14, sol:8-dskbytr.c2, "
            "sol:8-dskbytr.c20; p.278 opus:8-appA-dskbytr.c6, "
            "opus:8-appA-dskbytr.c15, sol:8-appa-dskbytr.c7, "
            "sol:8-appa-dskbytr.c16): [mutation substitute] Measured v1=" +
            hex16(v1) + " v2=" + hex16(v2) + ".");
#endif
    }

    // ================= A10: DSKBYTR-DOES-NOT-VARY-WITH-ROTATION-OR-DIRECTION =================
    // (p.248 opus:8-dskbytr.c5, sol:8-dskbytr.c5, sol:8-dskbytr.c6)
    {
        Harness h;
        h.select_drive0_motor_on();
        uint16_t before = h.read_reg(A_DSKBYTR);
#ifndef MUT_DSKBYTR_DOES_NOT_VARY_WITH_ROTATION_OR_DIRECTION
        h.ticks(2000);  // elapsed time with the motor running, no DMA armed.
        uint16_t after = h.read_reg(A_DSKBYTR);
        bool ok = before != after;
        checks.expect(ok,
            "DSKBYTR-DOES-NOT-VARY-WITH-ROTATION-OR-DIRECTION (p.248 "
            "opus:8-dskbytr.c5, sol:8-dskbytr.c5, sol:8-dskbytr.c6): "
            "'DSKBYTR may be used to synchronize the processor to the disk "
            "rotation before issuing a read or write under DMA control' -- "
            "the hardware proposition under the advice (per each claim's "
            "own split note) is that DSKBYTR's state tracks the incoming "
            "byte stream and hence the passage of rotation; sampled with "
            "the drive selected and the motor genuinely running (an edge "
            "this fixture drives, per METHODOLOGY) across 2000 elapsed "
            "clk7_en cycles with no DMA ever armed, it should differ. "
            "Measured before=" + hex16(before) + " after=" + hex16(after) +
            ".");
#else
        // MUTATION (already-red baseline; substitute a proven-live
        // mechanism through the same comparator): bit 13, sampled before
        // and after a genuine DSKLEN=0x4000 write interposed during the
        // same elapsed-idle window, instead of DSKBYTR's own value before
        // and after 2000 idle clk7_en cycles.
        h.ticks(1000);
        h.write_reg(A_DSKLEN, 0x4000);
        h.ticks(1000);
        uint16_t after = h.read_reg(A_DSKBYTR);
        bool ok = bitn(before, 13) != bitn(after, 13);
        checks.expect(ok,
            "DSKBYTR-DOES-NOT-VARY-WITH-ROTATION-OR-DIRECTION (p.248 "
            "opus:8-dskbytr.c5, sol:8-dskbytr.c5, sol:8-dskbytr.c6): "
            "[mutation substitute] Measured before=" + hex16(before) +
            " after=" + hex16(after) + ".");
#endif
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed
              << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
