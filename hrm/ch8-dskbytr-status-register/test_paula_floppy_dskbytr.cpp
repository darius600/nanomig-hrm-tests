// Test purpose: DSKBYTR ($DFF01A) -- the disk data-and-status read register:
//   its identity as a real, addressable register (p.247, p.248), the DSKBYT
//   bit set-on-receipt/cleared-on-read behaviour (p.248, p.278), DMAON
//   (p.248, p.278), DISKWRITE (p.248, p.278), WORDEQUAL and the
//   "sector-searching facilities" it is printed to expose (p.247, p.248,
//   p.278), the currently-unused bits (p.248, p.278), the DATA byte (p.248,
//   p.278), and the "synchronize to rotation" framing (p.248). Package
//   ch8-dskbytr-status-register, fixture paula_floppy.v +
//   paula_floppy_fifo.v (paula_floppy_ebr.v deliberately excluded -- it
//   declares a second `module paula_floppy`, a rig hazard given by the build
//   brief).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 247, 248, 278.
// Non-goals: DSKSYNC's own interrupt-timing precision (the "as little as two
//   microseconds" clause -- no port here carries real time units, per the
//   claims file's own split, recorded outside this package's 48 claims); the
//   "don't depend on read value" programmer-advice clause of the unused-bits
//   row (also outside this package's 48); DMACON's own DSKEN/DMAEN half of
//   the DMAON condition (this fixture has no DMACON port -- declined below,
//   needs the wider `paula` boundary).
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// REPAIR NOTE (independent validation, 2026-09-15): a Stage-8 independent
//   read withdrew all five of this package's original A5/A6/A7/A9/A10 --
//   sound in every case (see below), because the shared fixture defect
//   named by that validation was real: read_reg() spans no clock edge at
//   all (two eval()-only combinational peeks), so a "cleared on read"
//   claim could never have been exercised regardless of what the RTL
//   implements, and A9's original DATA-field check compared two arbitrary
//   snapshots with no independent ground truth for either. This file adds
//   read_reg_clocked() (a genuine, address-held, clk7_en-qualified read
//   cycle) for A6, a known-byte-binding technique via the DSKDATR/FIFO
//   marker path (reused from the already-accepted sibling
//   ch8-mfm-cell-encoding package) for A9, and a real-output-port
//   (`syncint`) match/mismatch oracle for A7. A5 and A10 are declined --
//   see each assertion's own comment for why a sound repair is not
//   available at this boundary. DSKDATR ($008) and the FIFO/MFM-encoder
//   pipeline behind it remain a different register and a different
//   package's subject in general, but A9 now reads DSKDATR (via
//   non-destructive peeks and real pops) to bind a known byte to the
//   DSKBYTR sampling instant -- this is grading DSKBYTR only, using
//   DSKDATR solely as an independently-live ground-truth source, exactly
//   as the task instructions for this repair direct.
//
// REPAIR NOTE / LOT F (independent validation, 2026-09-15, against
//   DOKTRIN.md/LOS-F.md). Four more items and a defect inside the previous
//   repair itself:
//   - A1 (FIXTURE-LIVENESS-KNOWN-DISTINCT-DSKLEN-STATES) and A6-CONTROL
//     (REAL-READ-CYCLE-IS-LIVE) carry no claim uid -- withdrawn to note(),
//     per the mechanical rule that a no-uid assertion was never
//     conformance evidence. A6-CONTROL's own note also now says plainly
//     what its measurement does and does not establish: it proves
//     read_reg_clocked() samples correctly, not that a real bus cycle
//     reaches a synchronous read-side-effect mechanism, since the two
//     bits it reads (DMAON, DISKWRITE) are combinational and would move
//     the same way through the old eval()-only helper too.
//   - A2 (DSKBYTR-IS-A-REAL-READABLE-REGISTER) used idle bit 15 (DSKBYT)
//     = 1 as its decode marker, but p.278 gives DSKBYTR no reset or idle
//     word at all. Repaired, not withdrawn: narrowed to bits 13
//     (DISKWRITE) and 14 (DMAON), both page-defined and both proven to
//     move by A3/A4.
//   - A8 (UNUSED-BITS-11-8-CARRY-NO-FUNCTION) required bits 11-8 to read
//     zero; p.264 says outright not to rely on unused bits' values at
//     all, and any value is page-compatible. Declined (record()).
//   - A4's own graded label repeated three uids (opus:8-dskbytr.c8,
//     sol:8-dskbytr.c9, opus:8-appA-dskbytr.c10) that ALSO sit on this
//     same block's DMAON-DMACON-HALF record() line -- a uid-placement
//     defect (the ledger reads uids, not the "DSKLEN clause"/"DMACON
//     clause" qualifiers beside them). Repaired: the graded label no
//     longer carries these three uids; they are recorded, once, on the
//     record() line, which now also names the graded assertion where
//     their DSKLEN half is empirically measured.
//   - A9 (DATA-FIELD-DOES-NOT-REFLECT-INCOMING-BYTES) is RED and IS a
//     genuine finding (bound=1, DSKBYTR=0xd000, DATA=0x00) -- but its own
//     guard was unsound: it searched DSKDATR for the word 0xFFFF as a
//     "known 0xFF byte" marker, and 0xFFFF is NOT a page-legal MFM
//     encoding of 0xFF (p.250 makes 0xFF -> 0x5555; 0xFFFF is only what
//     this RTL's own non-conformant, context-free encoder -- candidate
//     C9 -- produces for a run of one-bits). Because the old guard was a
//     CONJUNCT of the grading expression, a CORRECT encoder would never
//     produce the marker, bound_ok would be false, and the assertion
//     would fail anyway -- reporting a DATA-field defect that would not
//     be there, for the wrong reason. Repaired so the binding is a
//     PRECONDITION that declines (record()) when the marker cannot be
//     located, and grades (expect()) the DATA field only when it can --
//     against the RTL actually under test here, the marker is reliably
//     found and the red finding is unchanged.
//   No RTL was touched, no debug port was added, and no oracle was
//   adjusted by any of this.
//
// CLAIM SOURCE:
//   hrm-testpipeline/work/ch8-s2-build/
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
// CONTROL CASES: A2 (register genuinely address-decoded, narrowed to bits
//   13/14 -- Lot F), A3 (DISKWRITE genuinely mirrors DSKLEN) and A4 (DMAON
//   genuinely responds to DSKLEN's own double-write arming) are this
//   package's PASS-shaped, GRADED measurements on read_reg(), proving the
//   harness's combinational read path is live. A1 (fixture liveness),
//   A6-CONTROL (read_reg_clocked() liveness) and A8 (the unused bits'
//   measured, ungraded value) are the same kind of proof but carry no
//   claim uid or, for A8, grade against a value the page does not require
//   -- all three are MEASURED via note()/record(), not GRADED, per Lot F.
//
// ON RED-BASELINE MUTATION, per the build brief: "a stimulus mutation
//   proves an assertion CAN fail, and one already failing cannot fail
//   harder." A6, A7 and A9 are all expected RED (DSKBYTR's own constant
//   bits per candidate C6). Each one's mutation record instead substitutes
//   a DIFFERENT, PROVEN-LIVE mechanism from elsewhere in this same file --
//   DISKWRITE (bit 13, proven live by A3) or DMAON (bit 14, proven live by
//   A4) -- through the SAME "extract a bit, compare it against what the
//   state genuinely requires" comparator, and shows THAT goes GREEN: proof
//   the comparator is not vacuously red regardless of what it reads,
//   exactly the shape ch5-audio-dmacon-adkcon's own
//   DMACONR-NOT-DECODED-BY-PAULA mutation record already established as
//   this suite's precedent for a red-baseline assertion. A5 and A10 are
//   DECLINED (record(), not expect()) and carry no mutation record: per
//   the build brief, "a uid on a graded line counts as ASSERTED... Declines
//   go on record() lines and nowhere else" -- there is no graded line to
//   pair a mutation against. A1, A6-CONTROL and A8 lost their mutation
//   records for the same reason when Lot F withdrew them from expect() --
//   converting an expect() to note()/record() orphans its mutation record.
//
// MUTATION LOG (every entry actually run by this agent under WSL/
//   Verilator: `make clean && make MUT=<NAME>` -> run -> confirm the named
//   assertion, and only that one, newly flips colour against the baseline
//   -> `make clean && make` -> confirm the baseline exactly restored,
//   before moving to the next one). Only assertions still graded via
//   expect() carry one -- A1, A6-CONTROL and A8's entries were removed
//   with their conversion to note()/record() (Lot F):
//
// MUTATION DSKBYTR-IS-A-REAL-READABLE-REGISTER stimulus red (repaired
//   stimulus, Lot F): the "read at the decoded DSKBYTR address" trial's
//   address changed to the unmapped address used by the OTHER half of
//   this same assertion -> that half's own DISKWRITE-set expectation
//   (bit 13) now reads 0 instead of 1 (the unmapped address always
//   decodes to 0x0000) -> only this assertion newly red. Reverted.
// MUTATION DISKWRITE-BIT13-TRACKS-DSKLEN-WRITE-BIT stimulus red: the
//   "should read 1" trial's DSKLEN write changed from 0x4000 to 0x0000 (no
//   WRITE bit) -> bit 13 stays 0 where 1 was expected -> only this
//   assertion newly red. Reverted.
// MUTATION DMAON-BIT14-TRACKS-DSKLEN-DMA-ENABLE stimulus red: the second,
//   arming DSKLEN write was skipped, leaving only the single first write
//   -> dmaen never latches, dskstate never leaves DISKDMA_IDLE, DMAON
//   stays 0 where 1 was expected -> only this assertion newly red.
//   Reverted.
// MUTATION DSKBYT-NOT-CLEARED-ON-READ stimulus green: a genuine DSKLEN=
//   0x4000 write was inserted between the two CLOCKED reads, and the
//   comparator was pointed at bit 13 instead of bit 15 -> the two reads
//   now genuinely differ (0 then 1) where the real assertion's two reads
//   never do -> only this assertion newly green. Reverted.
// MUTATION WORDEQUAL-DOES-NOT-TRACK-DSKSYNC-MATCH stimulus green: the
//   whole proved-match/proved-mismatch apparatus is bypassed; the
//   comparator was pointed at bit 14 (DMAON) instead of bit 12 (WORDEQUAL)
//   for an idle-vs-armed pair -> bit 14 genuinely differs (0 idle, 1 once
//   DMA is armed, proven live by A4) -> only this assertion newly green.
//   Reverted.
// MUTATION DATA-FIELD-DOES-NOT-REFLECT-INCOMING-BYTES stimulus green: the
//   comparator was pointed at bit 13, comparing its value after a DSKLEN=
//   0x4000 write against its value after a DSKLEN=0x0000 write (two
//   genuinely different stimuli), INSTEAD of DATA (bits 7-0) at a
//   known-bound marker instant -> bit 13 reads two genuinely different
//   values (1 then 0) where the real assertion's known-byte comparison
//   never moves DATA at all -> only this assertion newly green. Reverted.
//   (Lot F's guard repair to the unmutated branch does not touch this
//   branch; re-run and confirmed still isolates correctly after the
//   repair.)
//
// RESULTS (WSL/Verilator, re-measured 2026-09-15, Lot F): 3 passed, 3
//   failed, 3 declined, 3 measured-not-graded (plus A9's own internal
//   decline branch, not taken against this RTL). A2
//   (DSKBYTR-IS-A-REAL-READABLE-REGISTER, repaired oracle), A3
//   (DISKWRITE-BIT13-TRACKS-DSKLEN-WRITE-BIT) and A4
//   (DMAON-BIT14-TRACKS-DSKLEN-DMA-ENABLE, uid-placement repaired) are
//   GREEN. A6 (DSKBYT-NOT-CLEARED-ON-READ), A7
//   (WORDEQUAL-DOES-NOT-TRACK-DSKSYNC-MATCH) and A9
//   (DATA-FIELD-DOES-NOT-REFLECT-INCOMING-BYTES, guard repaired) are RED
//   -- sound measurements, unchanged in substance by Lot F's repairs. A5,
//   A8 and A10 are DECLINED (RECORDED). A1 and A6-CONTROL are MEASURED
//   (note(), not graded). See out.txt for the run this header describes.

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
    // no clock edge required. Also serves as a non-destructive PEEK at
    // DSKDATR: sampled via eval() only, before any clock edge, it never
    // triggers fifo_rd's own synchronous pop (paula_floppy.v:911).
    uint16_t read_reg(int addr_w) {
        dut->reg_address_in = static_cast<uint16_t>(addr_w & 0x1FF);
        dut->eval();
        uint16_t v = static_cast<uint16_t>(dut->data_out);
        dut->reg_address_in = 0;
        dut->eval();
        return v;
    }

    // Holds addr across a genuine QUALIFYING (clk7_en) rising edge -- the
    // same bus-cycle shape every register WRITE in this module uses
    // (paula_floppy.v:703-743's own `if (clk7_en)` gate) -- so a
    // synchronous "clear on read" implementation would have a real chance
    // to observe this as an actual read cycle. Repairs the fixture defect
    // named by the independent validation (2026-09-15): the plain
    // read_reg() above spans no clock edge at all and is a bare
    // combinational peek, incapable of ever exercising a synchronous
    // read-side-effect regardless of whether the RTL implements one.
    uint16_t read_reg_clocked(int addr_w) {
        dut->reg_address_in = static_cast<uint16_t>(addr_w & 0x1FF);
        dut->data_in = 0;
        while (!dut->clk7_en) tick();  // advance to just before a qualifying edge
        tick();                        // take the edge with the address held
        uint16_t v = static_cast<uint16_t>(dut->data_out);
        dut->reg_address_in = 0;
        ticks(4);
        return v;
    }

    // Pops exactly one word from DSKDATR's FIFO: busrd (combinational
    // address decode) ANDed with dmaon gates fifo_rd (paula_floppy.v:911);
    // the pop itself happens on the qualifying clk7_en edge that follows,
    // held for a full period matching write_reg's own established shape.
    // Returns the word that was at the head BEFORE the pop (a
    // non-destructive peek at that instant, via read_reg's own eval()-only
    // convention above).
    uint16_t dskdatr_pop() {
        uint16_t v = read_reg(A_DSKDATR);
        dut->reg_address_in = A_DSKDATR;
        ticks(4);
        dut->reg_address_in = 0;
        ticks(4);
        return v;
    }

    // Waits (bounded) for the module to request a sector via sdc_rd, then
    // acks it and streams the given 512 bytes in through the strobed SD
    // interface (paula_floppy.v:263-336) -- the same protocol the sibling
    // paula_floppy package's own accepted test exercises end-to-end on
    // this exact RTL (METHODOLOGY (4)).
    void feed_sector(const uint8_t* bytes /* 512 */) {
        bool requested = false;
        for (int i = 0; i < 200 && !requested; ++i) {
            tick();
            if (dut->sdc_rd != 0) requested = true;
        }
        dut->sdc_busy = 1;
        ticks(20);
        for (int i = 0; i < 512; ++i) {
            dut->sdc_byte_addr = static_cast<uint16_t>(i);
            dut->sdc_byte_in_data = bytes[i];
            dut->sdc_byte_in_strobe = 1;
            ticks(4);
            dut->sdc_byte_in_strobe = 0;
            ticks(4);
        }
        dut->sdc_busy = 0;
        ticks(40);
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

// Result of one WORDEQUAL trial: whether the intended match/mismatch state
// was actually PROVED (not merely intended), and what bit 12 of DSKBYTR
// read at/through that proved instant.
struct WordequalTrial {
    int bit12 = -1;
    bool proved = false;
};

// Runs one genuine read transfer against a chosen DSKSYNC value and uses
// `syncint` -- a real output port, not an internal wire -- as the oracle
// for "a match is happening right now". syncint = sync_match | ~dmaen &
// |(~_sel & motor_on & disk_present) & sof (paula_floppy.v:917); with a
// read transfer armed, dmaen is latched 1 for the whole run
// (paula_floppy.v:735-743), permanently zeroing that second disjunct, so
// syncint becomes EXACTLY sync_match = dsksync==floppy_data &&
// fifo_reading_sector && trackrd (paula_floppy.v:915) throughout -- a
// decisive, RTL-confirmed proof of match or non-match, not an inference
// from delivery counts or an idle-vs-active guess (the defect the
// independent validation, 2026-09-15, found in the withdrawn assertion).
// want_match=true: DSKSYNC is set to 0x4489, the generator's own fixed
// sync word AND its own reset default (paula_floppy.v:485,705) -- proved
// the instant `syncint` first pulses. want_match=false: DSKSYNC is set to
// 0xDEAD, a value the generator's deterministic sequence never produces --
// proved only if `syncint` never pulses across the whole poll (a
// SUSTAINED non-match while genuinely, continuously reading, not idle).
WordequalTrial run_wordequal_trial(uint16_t dsksync_value, bool want_match) {
    WordequalTrial r;
    Harness h;
    h.select_drive0_motor_on();
    h.write_reg(A_DSKSYNC, dsksync_value);
    h.arm_dma(/*write_dir=*/false, /*length=*/16, /*settle_ticks=*/4);

    // Request and answer exactly one sector, stopping the instant the last
    // byte strobe is issued -- deliberately NO trailing settle here, so
    // the syncint poll below cannot silently run past the sync-mark word
    // position before it starts watching.
    bool requested = false;
    for (int i = 0; i < 200 && !requested; ++i) {
        h.tick();
        if (h.dut->sdc_rd != 0) requested = true;
    }
    h.dut->sdc_busy = 1;
    h.ticks(8);
    h.dut->sdc_busy = 0;
    h.ticks(4);
    for (int i = 0; i < 512; ++i) {
        h.dut->sdc_byte_addr = static_cast<uint16_t>(i);
        h.dut->sdc_byte_in_data = static_cast<uint8_t>(i);
        h.dut->sdc_byte_in_strobe = 1;
        h.ticks(4);
        h.dut->sdc_byte_in_strobe = 0;
        h.ticks(4);
    }

    // reg_address_in held at DSKBYTR throughout the poll: side-effect-free
    // (busrd/fifo_rd need DSKDATR's own address, not DSKBYTR's -- holding
    // this address disturbs nothing), and lets data_out track bit 12
    // continuously without a separate read call that could itself miss
    // the exact qualifying edge.
    h.dut->reg_address_in = A_DSKBYTR;
    h.dut->eval();
    if (want_match) {
        for (int i = 0; i < 2000 && !r.proved; ++i) {
            h.tick();
            if (h.dut->syncint) {
                r.proved = true;
                r.bit12 = bitn(static_cast<uint16_t>(h.dut->data_out), 12);
            }
        }
    } else {
        bool any_sync = false;
        for (int i = 0; i < 2000; ++i) {
            h.tick();
            if (h.dut->syncint) any_sync = true;
        }
        r.proved = !any_sync;
        r.bit12 = bitn(static_cast<uint16_t>(h.dut->data_out), 12);
    }
    h.dut->reg_address_in = 0;
    h.dut->eval();
    return r;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ================= A1: FIXTURE-LIVENESS =================
    // Mandatory precondition, no claim uid: two distinct, known DSKLEN
    // states must arrive at DSKBYTR distinctly before anything else here
    // is trusted.
    //
    // WITHDRAWN TO note() (independent validation, 2026-09-15, Lot F): this
    // assertion carries no claim uid -- it was never conformance evidence,
    // only a fixture control, so grading it through expect() cost no
    // coverage to remove and only ever risked a false conformance reading.
    // Printed as a measurement, not graded. Its mutation record is removed
    // with it (a note() has no baseline colour to defend).
    {
        Harness h1;
        h1.write_reg(A_DSKLEN, 0x4000);
        uint16_t v1 = h1.read_reg(A_DSKBYTR);

        Harness h2;  // fresh model -- dsktrack carries no reset term.
        h2.write_reg(A_DSKLEN, 0x0000);
        uint16_t v2 = h2.read_reg(A_DSKBYTR);

        checks.note(
            "FIXTURE-LIVENESS-KNOWN-DISTINCT-DSKLEN-STATES (no claim uid, "
            "MEASURED not GRADED): a DSKLEN write with the WRITE bit set "
            "(0x4000) makes DSKBYTR read 0xB000, and a distinct, "
            "separately-reset trial writing DSKLEN=0x0000 makes it read "
            "0x9000 -- two distinct, known register states arrive at "
            "DSKBYTR distinctly, showing this fixture's read/write bus is "
            "live before anything else in this file is trusted. Measured "
            "v1=" + hex16(v1) + " v2=" + hex16(v2) + ".");
    }

    // ================= A2: DSKBYTR-IS-A-REAL-READABLE-REGISTER =================
    // (p.247 opus:8-dma.c31, sol:8-dskdma.c9; p.248 opus:8-dskbytr.c1,
    //  sol:8-dskbytr.c1; p.278 opus:8-appA-dskbytr.c5)
    //
    // REPAIRED (independent validation, 2026-09-15, Lot F): the withdrawn
    // form used idle bit 15 (DSKBYT) = 1 as its decode marker. p.278 gives
    // DSKBYTR NO reset or idle word at all -- requiring any specific idle
    // value, on any bit, is requiring something the page never states.
    // Narrowed to two bits the page DOES define and that MOVE: DISKWRITE
    // (bit 13, proven live by A3) after a plain DSKLEN write with the
    // WRITE bit set, and DMAON (bit 14, proven live by A4) once DMA is
    // armed -- both are page-cited, state-dependent values, not an
    // unstated reset default.
    {
        Harness h;
        h.write_reg(A_DSKLEN, 0x4000);  // WRITE bit set, no DMA arm -- DISKWRITE (bit 13) should read 1 (A3).
#ifdef MUT_DSKBYTR_IS_A_REAL_READABLE_REGISTER
        uint16_t after_write = h.read_reg(A_UNMAPPED);  // MUTATION: read the wrong (unmapped) address instead of DSKBYTR.
#else
        uint16_t after_write = h.read_reg(A_DSKBYTR);
#endif
        uint16_t idle_at_unmapped = h.read_reg(A_UNMAPPED);
        h.arm_dma(/*write_dir=*/false, /*length=*/10);
        uint16_t during_active = h.read_reg(A_DSKBYTR);

        bool decoded_write_state = bitn(after_write, 13) == 1;   // DISKWRITE, page-defined (A3).
        bool not_decoded_elsewhere = idle_at_unmapped == 0x0000;
        bool decoded_active = bitn(during_active, 14) == 1;      // DMAON, page-defined (A4).
        checks.expect(decoded_write_state && not_decoded_elsewhere && decoded_active,
            "DSKBYTR-IS-A-REAL-READABLE-REGISTER (p.247 opus:8-dma.c31, "
            "sol:8-dskdma.c9; p.248 opus:8-dskbytr.c1, sol:8-dskbytr.c1; "
            "p.278 opus:8-appA-dskbytr.c5): DSKBYTR is a genuinely decoded "
            "register, not a stuck wire -- reading its own address returns "
            "a DEFINED value that differs correctly by state (DISKWRITE set "
            "after a WRITE-bit DSKLEN write; DMAON set mid-transfer), while "
            "reading an address this module does not decode returns "
            "0x0000, proving the mux discriminates by address AND by state "
            "rather than returning a fixed pattern regardless of what or "
            "when it is read; narrowed to bits 13/14 rather than bit 15 "
            "(DSKBYT), whose own idle value p.278 never defines (DSKBYTR "
            "has no printed reset or idle word at all). This is also the "
            "'register that may be polled to examine the disk input "
            "stream' (p.247) and 'this register is the disk-microprocessor "
            "data buffer' (p.248/p.278) identity half. Measured "
            "after_write=" + hex16(after_write) +
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
            "DMAON-BIT14-TRACKS-DSKLEN-DMA-ENABLE (p.248, p.278; see the "
            "DMAON-DMACON-HALF record() below for this claim's own uids "
            "and citation): 'Indicates when DMA is actually enabled ... "
            "This means the DMAEN bit in DSKLEN' / 'Mirror of bit 15 "
            "(DMAEN) in DSKLEN' -- graded here only for the DSKLEN half "
            "both pages state identically; the DMACON half is declined "
            "below, out of this fixture's boundary. Two identical DSKLEN "
            "writes with the DMA-enable bit set (the double write the "
            "build brief's own rig-hazard note already named) bring DMAON "
            "(bit 14) up; this package's own measured DSKLEN<<800A "
            "double-write value (0xD000, matching the build brief's "
            "disclosed background) confirms bit 14 set here too. Measured "
            "v=" + hex16(v) + ".");

        // Repaired (independent validation, 2026-09-15, Lot F --
        // uid-placement defect): opus:8-dskbytr.c8, sol:8-dskbytr.c9 and
        // opus:8-appA-dskbytr.c10 previously appeared on BOTH the graded
        // expect() label above (as its "DSKLEN clause") AND this record()
        // line (as its "DMACON clause") -- the coverage ledger reads uids,
        // not the "clause" qualifier beside them, so it counted these
        // three as fully asserted while this record() simultaneously
        // declined them. Per the uid-placement rule, an assertion that
        // grades only part of a claim keeps its expect() label uid-free
        // and carries the uid on a record() line alone -- moved here.
        checks.record(
            "opus:8-dskbytr.c8, sol:8-dskbytr.c9, opus:8-appA-dskbytr.c10 "
            "(p.248, p.278; DMAON's own printed definition is a compound "
            "condition: 'All the various DMA bits must be true. This means "
            "the DMAEN bit in DSKLEN, and the DSKEN & DMAEN bits in "
            "DMACON.' / 'Mirror of bit 15 (DMAEN) in DSKLEN, ANDed with "
            "Bit09 (DMAEN) in DMACON'): the DSKLEN half of this same "
            "compound condition is empirically measured, GREEN, by the "
            "DMAON-BIT14-TRACKS-DSKLEN-DMA-ENABLE assertion immediately "
            "above (not graded under these three uids, per the "
            "uid-placement rule -- see that assertion's own label); the "
            "DMACON half cannot be measured here at all. p.248 prints 'the "
            "DSKEN & DMAEN bits in DMACON' as an additional requirement; "
            "p.278 prints only 'ANDed with Bit09 (DMAEN) in DMACON' -- a "
            "page-versus-page difference, not a red against this RTL. "
            "This package's assigned fixture is paula_floppy compiled "
            "alone, which has no DMACON port at all -- DMACON's bits reach "
            "this module only pre-combined into the single `enable` input "
            "this file holds asserted throughout (see the Harness "
            "constructor). Grading DMACON's own bit combination needs the "
            "wider `paula` boundary, already measured by finding candidate "
            "C11 in work/2026-09-15_ch8_s2_finding_candidates.md (DMACON "
            "bit 9 alone leaves DSKBYTR bit 14 clear; bit 9 AND bit 4 are "
            "both required) -- not re-measured here, and not asserted on a "
            "boundary this package does not have. Because this claim is "
            "compound and only half-measurable at this boundary, it is "
            "recorded as a whole rather than split across a graded line "
            "and a declined line for the SAME uids.");
    }

    // ================= A5: DSKBYT-NOT-SET-ONLY-ON-RECEIPT -- DECLINED =================
    // (p.248 opus:8-dskbytr.c3, opus:8-dskbytr.c6, sol:8-dskbytr.c3,
    //  sol:8-dskbytr.c7; p.278 opus:8-appA-dskbytr.c7,
    //  opus:8-appA-dskbytr.c8, sol:8-appa-dskbytr.c8, sol:8-appa-dskbytr.c9)
    //
    // DECLINED (independent validation, 2026-09-15): neither p.248 nor
    // p.278 defines DSKBYT's RESET value. "As each byte is received...
    // DSKBYT is set true" states what RECEIPT does; it says nothing about
    // what a fresh, byte-never-received reset state reads. The withdrawn
    // assertion silently required "receipt sets it" to also mean "reset
    // clears it", which is not printed anywhere.
    //
    // A sound receipt test needs a genuinely CLEARED starting state,
    // reached through the manual's own documented clearing action (p.248
    // "cleared when the DSKBYTR register is read"; p.278 "(reset by
    // reading this register)"), followed by a delivered known byte, then
    // an observed 0->1 transition. A6 below -- now driven through a
    // genuine, address-held, clk7_en-qualified read cycle, not a bare
    // combinational peek -- PROVES that documented clearing action does
    // not exist in this RTL: DSKBYT reads 1 both before and after a real
    // read cycle (paula_floppy.v:699's literal `1'b1`). There is therefore
    // no reachable cleared precondition anywhere in this fixture from
    // which a 0->1 receipt transition could ever be observed -- attempting
    // to restructure this assertion would only restate A6's own finding
    // under a different name, not test receipt.
    //
    // Per the build brief: "an assertion that tests more than its page
    // states produces a red finding about nothing." Declining.
    {
        checks.record(
            "DSKBYT-NOT-SET-ONLY-ON-RECEIPT (p.248 opus:8-dskbytr.c3, "
            "opus:8-dskbytr.c6, sol:8-dskbytr.c3, sol:8-dskbytr.c7; p.278 "
            "opus:8-appA-dskbytr.c7, opus:8-appA-dskbytr.c8, "
            "sol:8-appa-dskbytr.c8, sol:8-appa-dskbytr.c9): neither cited "
            "page defines DSKBYT's reset value -- 'receipt sets it' is not "
            "'reset clears it'. A sound receipt test needs a genuinely "
            "cleared precondition reached through the page's own "
            "documented clearing action (the read this file's own A6 "
            "grades), but A6's real, clk7_en-qualified read cycle proves "
            "that clearing action does not exist in this RTL (DSKBYT is "
            "the literal constant 1'b1, paula_floppy.v:699) -- so no "
            "cleared state is ever reachable at this boundary from which a "
            "0->1 receipt transition could be observed. Declining rather "
            "than restating A6's own finding under this claim's uids.");
    }

    // ================= A6-CONTROL: REAL-READ-CYCLE-IS-LIVE =================
    // No claim uid. WITHDRAWN TO note() (independent validation,
    // 2026-09-15, Lot F): this control reads bit 14 (DMAON) and bit 13
    // (DISKWRITE) through read_reg_clocked() and watches them move as
    // DSKLEN is driven -- but both bits are COMBINATIONAL mirrors of
    // DSKLEN's own state (paula_floppy.v:699): they would move exactly
    // the same way through the OLD eval()-only read_reg() helper too,
    // since nothing about their value depends on the read itself being a
    // genuine, qualifying bus cycle. This measurement therefore shows
    // read_reg_clocked() samples data_out correctly at the point it
    // reads -- it does NOT show that holding the address across a real
    // clk7_en edge reaches any SYNCHRONOUS read-side-effect mechanism
    // (the thing "cleared when read" would need). Carries no claim uid,
    // so note() -- printed with that scope stated explicitly, so a later
    // reader does not take this as proof that A6's own read cycle reaches
    // a clocked side effect it may not. Its mutation record is removed
    // with it.
    {
        Harness h1;
        uint16_t before14 = h1.read_reg_clocked(A_DSKBYTR);
        h1.arm_dma(/*write_dir=*/false, /*length=*/10);
        uint16_t after14 = h1.read_reg_clocked(A_DSKBYTR);

        Harness h2;
        uint16_t before13 = h2.read_reg_clocked(A_DSKBYTR);
        h2.write_reg(A_DSKLEN, 0x4000);
        uint16_t after13 = h2.read_reg_clocked(A_DSKBYTR);

        checks.note(
            "A6-CONTROL-REAL-READ-CYCLE-IS-LIVE (no claim uid, MEASURED "
            "not GRADED): read_reg_clocked() -- holding the DSKBYTR "
            "address across a genuine clk7_en-qualified rising edge -- "
            "correctly observes DMAON (bit 14) rise once DMA is armed "
            "(A4) and DISKWRITE (bit 13) rise once DSKLEN's WRITE bit is "
            "set (A3). Both are COMBINATIONAL mirrors of DSKLEN and would "
            "move identically through the old eval()-only read_reg() "
            "helper -- this shows the read function samples correctly, "
            "NOT that a real bus cycle reaches a synchronous read-side-"
            "effect mechanism, which nothing read here has. A6 below "
            "reads bit 15 through the identical cycle; it does not "
            "inherit stronger proof than this note actually establishes. "
            "Measured before14=" + hex16(before14) + " after14=" +
            hex16(after14) + " before13=" + hex16(before13) + " after13=" +
            hex16(after13) + ".");
    }

    // ================= A6: DSKBYT-BIT15-IS-NOT-CLEARED-ON-READ =================
    // (p.248 opus:8-dskbytr.c4, opus:8-dskbytr.c7, sol:8-dskbytr.c8; p.278
    //  opus:8-appA-dskbytr.c9, sol:8-appa-dskbytr.c10)
    //
    // REPAIRED (independent validation, 2026-09-15): the withdrawn form's
    // read_reg() spanned no clock edge at all (two eval()-only
    // combinational peeks), so it could never have observed a synchronous
    // read-side-effect even if this RTL implemented one. Repaired to use
    // read_reg_clocked() -- holding the address across a genuine
    // clk7_en-qualified rising edge, the SAME cycle shape A6-CONTROL just
    // proved faithfully observes bits that do move -- for both reads
    // below. Still RED: paula_floppy.v:699 hardwires bit 15 to the literal
    // constant 1'b1, so it cannot clear regardless of how genuine the read
    // cycle is; this is now a sound measurement of that fact, not an
    // artifact of a read that was never really a read.
    {
        Harness h;
        uint16_t read1 = h.read_reg_clocked(A_DSKBYTR);
#ifndef MUT_DSKBYT_NOT_CLEARED_ON_READ
        uint16_t read2 = h.read_reg_clocked(A_DSKBYTR);
        bool ok = bitn(read1, 15) == 1 && bitn(read2, 15) == 0;
        checks.expect(ok,
            "DSKBYT-NOT-CLEARED-ON-READ (p.248 opus:8-dskbytr.c4, "
            "opus:8-dskbytr.c7, sol:8-dskbytr.c8; p.278 "
            "opus:8-appA-dskbytr.c9, sol:8-appa-dskbytr.c10): 'DSKBYT is "
            "cleared when the DSKBYTR register is read' / '(reset by "
            "reading this register)' -- two consecutive GENUINE bus read "
            "cycles (each holding the address across a real, "
            "clk7_en-qualified edge, per A6-CONTROL) should show bit 15 "
            "cleared on the second relative to the first. Measured read1=" +
            hex16(read1) + " read2=" + hex16(read2) + ".");
#else
        // MUTATION (already-red baseline; substitute a proven-live
        // mechanism through the same comparator and the same genuine
        // read-cycle apparatus): a real DSKLEN=0x4000 write inserted
        // between the two clocked reads, comparator pointed at bit 13
        // (DISKWRITE, proven live by A6-CONTROL/A3) instead of bit 15.
        h.write_reg(A_DSKLEN, 0x4000);
        uint16_t read2 = h.read_reg_clocked(A_DSKBYTR);
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
    //
    // REPAIRED (independent validation, 2026-09-15): the withdrawn form
    // compared an IDLE state against an ACTIVE-BUT-MISMATCHING state and
    // required them to differ -- but p.248 requires WORDEQUAL false on a
    // non-match; it never requires idle and active-mismatching to differ
    // from each other. Repaired to sample bit 12 during a PROVED MATCH and
    // during a PROVED MISMATCH, using `syncint` (see run_wordequal_trial
    // above) -- a real output port that, with dmaen held 1 throughout an
    // armed transfer, is EXACTLY sync_match for the whole run -- as the
    // oracle for "matching right now", not an inference from idle-vs-busy.
#ifndef MUT_WORDEQUAL_DOES_NOT_TRACK_DSKSYNC_MATCH
    {
        WordequalTrial match = run_wordequal_trial(0x4489, /*want_match=*/true);
        WordequalTrial mismatch = run_wordequal_trial(0xDEAD, /*want_match=*/false);
        bool ok = match.proved && mismatch.proved &&
                  match.bit12 == 1 && mismatch.bit12 == 0;
        checks.note("A7-MEASURED: match trial proved=" + std::to_string(match.proved) +
                    " bit12=" + std::to_string(match.bit12) + "; mismatch trial proved=" +
                    std::to_string(mismatch.proved) + " bit12=" + std::to_string(mismatch.bit12) + ".");
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
            "Sampled at a PROVED match instant (syncint pulsed, DSKSYNC == "
            "the generator's own fixed sync word) and across a PROVED, "
            "SUSTAINED mismatch (syncint never pulsed while genuinely, "
            "continuously reading), WORDEQUAL (bit 12) should read 1 at "
            "the match and 0 through the mismatch; it reads 1 at both "
            "(paula_floppy.v:699's literal constant), so it cannot be "
            "distinguishing them.");
    }
#else
    {
        // MUTATION (already-red baseline; substitute a proven-live
        // mechanism through the same comparator): bit 14 (DMAON, proven
        // live by A4) instead of bit 12, over an idle-vs-armed pair.
        Harness h;
        uint16_t idle = h.read_reg(A_DSKBYTR);
        h.arm_dma(/*write_dir=*/false, /*length=*/10);
        uint16_t active = h.read_reg(A_DSKBYTR);
        bool ok = bitn(idle, 14) != bitn(active, 14);
        checks.expect(ok,
            "WORDEQUAL-DOES-NOT-TRACK-DSKSYNC-MATCH (p.247 opus:8-dma.c30, "
            "sol:8-dskdma.c8; p.248 opus:8-dskbytr.c11, sol:8-dskbytr.c15, "
            "opus:8-dskbytr.c12, sol:8-dskbytr.c16; p.278 "
            "opus:8-appA-dskbytr.c13, sol:8-appa-dskbytr.c14): "
            "[mutation substitute] Measured idle=" + hex16(idle) +
            " active=" + hex16(active) + ".");
    }
#endif

    // ================= A8: UNUSED-BITS-11-8-CARRY-NO-DEFINED-FUNCTION -- DECLINED =================
    //
    // DECLINED (independent validation, 2026-09-15, Lot F): p.278 marks
    // bits 11-8 "X  Not used", and p.264 says outright: "do not rely on
    // the values of unused bits when reading from a read only register."
    // The withdrawn form required these bits to read exactly zero in
    // three states -- but the page does not require zero, it requires
    // that a reader not depend on ANY particular value. Any value on
    // these bits is compatible with the page; requiring zero grades
    // against something the manual never states. The withdrawn
    // assertion's own mutation record justified "unfalsifiable, no
    // stimulus" by pointing at this RTL's own literal 4'b0000
    // (paula_floppy.v:699) -- implementation-derived reasoning, exactly
    // what the build brief and DOKTRIN.md both rule out as a basis for an
    // oracle. Declining rather than grading a value the page leaves
    // unspecified.
    {
        Harness h;
        uint16_t idle = h.read_reg(A_DSKBYTR);
        h.write_reg(A_DSKLEN, 0x4000);
        uint16_t after_write = h.read_reg(A_DSKBYTR);
        h.arm_dma(/*write_dir=*/false, /*length=*/10);
        uint16_t during_active = h.read_reg(A_DSKBYTR);

        auto bits118 = [](uint16_t v) { return (v >> 8) & 0xF; };
        checks.note(
            "A8-MEASURED (not graded): bits 11-8 read " +
            std::to_string(bits118(idle)) + " idle, " +
            std::to_string(bits118(after_write)) + " after a DISKWRITE-set "
            "write, " + std::to_string(bits118(during_active)) +
            " during an active DMA-armed transfer (idle=" + hex16(idle) +
            " after_write=" + hex16(after_write) + " during_active=" +
            hex16(during_active) + ") -- a real measurement, not evidence "
            "that any other value would be non-conformant.");
        checks.record(
            "UNUSED-BITS-11-8-CARRY-NO-DEFINED-FUNCTION (p.248 "
            "opus:8-dskbytr.c13, sol:8-dskbytr.c18; p.278 "
            "opus:8-appA-dskbytr.c14, sol:8-appa-dskbytr.c15): 'Currently "
            "unused' / 'Not used', and p.264, 'do not rely on the values "
            "of unused bits when reading from a read only register.' Any "
            "value on bits 11-8 is compatible with the page -- requiring "
            "zero (the withdrawn form's own oracle) grades against a value "
            "the manual never states, and the withdrawn form's own "
            "justification for treating that requirement as unfalsifiable "
            "cited this RTL's own literal constant, not the page. "
            "Declining rather than grading an unstated value.");
    }

    // ================= A9: DATA-FIELD-DOES-NOT-REFLECT-INCOMING-DISK-BYTES =================
    // (p.248 opus:8-dskbytr.c2, opus:8-dskbytr.c14, sol:8-dskbytr.c2,
    //  sol:8-dskbytr.c20; p.278 opus:8-appA-dskbytr.c6,
    //  opus:8-appA-dskbytr.c15, sol:8-appa-dskbytr.c7,
    //  sol:8-appa-dskbytr.c16)
    //
    // REPAIRED (independent validation, 2026-09-15): the withdrawn form
    // compared two arbitrary DSKBYTR snapshots (after uniform 0x11 and
    // 0xEE feeds) for inequality, with no independent ground truth for
    // EITHER sample -- both reading 0x00 is compatible with a conforming
    // implementation whose DATA field simply had not yet caught up. This
    // is THE SAME UNDERLYING FACT as ch8-disk-register-map-paula-floppy/B4
    // and ch8-disk-controller-overview/opus:8-dsk.c9+sol:8-dsk.c9 -- one
    // fact, repaired identically in all three packages that carry it.
    //
    // Repaired to bind a KNOWN incoming byte to the DSKBYTR sampling
    // instant via the DSKDATR/FIFO marker technique already established
    // and accepted by the sibling ch8-mfm-cell-encoding package: a uniform
    // 0xFF sector's payload word can only ever arrive as exactly 0xFFFF
    // (its clock cell is a constant 1 for every payload bit, so an all-1s
    // fed byte is the one pattern this fixture's own encoder cannot
    // confuse with any fixed framing default). Calibrate the pop index at
    // which that marker word appears (a fresh, throwaway model), then on
    // the real trial's model, discard pops up to just before that index,
    // PEEK (not pop) DSKDATR to confirm the known word is present RIGHT
    // NOW, then peek DSKBYTR -- zero clock edges elapse between the two
    // peeks, so both are read at the SAME instant. Both bytes of 0xFFFF
    // are 0xFF, sidestepping any high/low-byte ordering ambiguity in what
    // "the current byte" means.
    //
    // DEFECT FOUND IN THIS REPAIR, AND FIXED (independent validation,
    // 2026-09-15, Lot F): the red below IS a genuine finding -- bound=1,
    // DSKBYTR=0xd000, DATA=0x00 -- and it must survive. But 0xFFFF is NOT
    // a page-legal MFM encoding of 0xFF: p.250's own table makes "1 -> 01"
    // unconditional, so a uniform 0xFF byte page-encodes to 0x5555, not
    // 0xFFFF. 0xFFFF is only what THIS RTL's own non-conformant,
    // context-free encoder (candidate C9, graded in
    // ch8-mfm-cell-encoding's A2/A4) happens to produce for a run of
    // one-bits. The withdrawn guard used `bound_ok` -- "was the 0xFFFF
    // marker found" -- as a CONJUNCT of the grading expression
    // (`ok = bound_ok && (data_field == known_byte)`), so against a
    // CORRECT encoder the marker would never be found, bound_ok would be
    // false, and this assertion would FAIL anyway -- reporting a
    // DATA-field defect that would not be there, for the wrong reason.
    // Repaired so the binding is a PRECONDITION that DECLINES, not a
    // conjunct that grades: when the marker cannot be located, record()
    // states plainly that no known byte could be bound, and nothing about
    // the DATA field is claimed either way; when it CAN be located (as it
    // reliably is, against this exact RTL), the DATA-field comparison is
    // graded via expect() exactly as before. Against the RTL actually
    // under test here, bound_ok is true and the finding is unchanged.
    {
        int marker_index = -1;
        {
            Harness cal;  // throwaway calibration model.
            cal.select_drive0_motor_on();
            uint16_t word = 0x8000 | 544;  // read-direction, one whole sector's worth.
            cal.write_reg(A_DSKLEN, word);
            cal.write_reg(A_DSKLEN, word);
            uint8_t sector[512];
            std::memset(sector, 0xFF, sizeof sector);
            cal.feed_sector(sector);
            for (int i = 1; i <= 60; ++i) {
                if (cal.dskdatr_pop() == 0xFFFF) { marker_index = i; break; }
            }
        }
        checks.note("A9-CALIBRATION: 0xFF-sector marker word (0xFFFF) first found at DSKDATR pop #" +
                    std::to_string(marker_index) + ".");

#ifndef MUT_DATA_FIELD_DOES_NOT_REFLECT_INCOMING_BYTES
        bool bound_ok = false;
        uint16_t known_word = 0, dskbytr_val = 0;
        if (marker_index > 0) {
            Harness h;  // fresh model -- dsktrack carries no reset term.
            h.select_drive0_motor_on();
            uint16_t word = 0x8000 | 544;
            h.write_reg(A_DSKLEN, word);
            h.write_reg(A_DSKLEN, word);
            uint8_t sector[512];
            std::memset(sector, 0xFF, sizeof sector);
            h.feed_sector(sector);
            for (int i = 1; i < marker_index; ++i) h.dskdatr_pop();  // discard up to just before the marker.
            known_word = h.read_reg(A_DSKDATR);    // peek, no pop -- the marker word itself, at this instant.
            dskbytr_val = h.read_reg(A_DSKBYTR);   // SAME instant: zero clock edges elapsed since the peek above.
            bound_ok = (known_word == 0xFFFF);
        }
        uint8_t known_byte = 0xFF;  // both bytes of 0xFFFF are 0xFF -- no high/low ambiguity.
        uint8_t data_field = static_cast<uint8_t>(dskbytr_val & 0x00FF);
        checks.note("A9-MEASURED: known_word=" + hex16(known_word) + " (bound=" +
                    std::to_string(bound_ok) + "), DSKBYTR at that same instant=" +
                    hex16(dskbytr_val) + ", DATA field=" +
                    hex16(data_field) + ".");
        if (!bound_ok) {
            // PRECONDITION FAILED -- decline, do not grade. This is the
            // branch a conforming encoder would always take (p.250 makes
            // 0xFF -> 0x5555, never 0xFFFF), and it must say nothing about
            // the DATA field either way.
            checks.record(
                "DATA-FIELD-DOES-NOT-REFLECT-INCOMING-BYTES (p.248 "
                "opus:8-dskbytr.c2, opus:8-dskbytr.c14, sol:8-dskbytr.c2, "
                "sol:8-dskbytr.c20; p.278 opus:8-appA-dskbytr.c6, "
                "opus:8-appA-dskbytr.c15, sol:8-appa-dskbytr.c7, "
                "sol:8-appa-dskbytr.c16): could not bind a known byte to "
                "the DSKBYTR sampling instant -- the calibration search "
                "never located the 0xFFFF marker word this technique looks "
                "for in the DSKDATR stream (marker_index=" +
                std::to_string(marker_index) + "). Declining rather than "
                "grading an unbound sample: 0xFFFF is not itself a page-"
                "legal MFM encoding of 0xFF (p.250 makes 0xFF -> 0x5555), "
                "so a marker search failing here is not evidence about the "
                "DATA field one way or the other.");
        } else {
            bool ok = (data_field == known_byte);
            checks.expect(ok,
                "DATA-FIELD-DOES-NOT-REFLECT-INCOMING-BYTES (p.248 "
                "opus:8-dskbytr.c2, opus:8-dskbytr.c14, sol:8-dskbytr.c2, "
                "sol:8-dskbytr.c20; p.278 opus:8-appA-dskbytr.c6, "
                "opus:8-appA-dskbytr.c15, sol:8-appa-dskbytr.c7, "
                "sol:8-appa-dskbytr.c16): 'In read mode, data from the disk is "
                "placed into this register one byte at a time' / '7-0 DATA "
                "Disk byte data' -- with a KNOWN incoming byte (0xFF) bound to "
                "the DSKBYTR sampling instant via the proven-live DSKDATR/FIFO "
                "marker path, DSKBYTR's own DATA field should read that same "
                "byte at that instant; paula_floppy.v:699 hardwires bits 7:0 "
                "to the literal constant 8'h00, so it does not. Measured "
                "known_word=" + hex16(known_word) + " dskbytr=" + hex16(dskbytr_val) +
                ".");
        }
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

    // ================= A10: DSKBYTR-DOES-NOT-VARY-WITH-ROTATION-OR-DIRECTION -- DECLINED =================
    // (p.248 opus:8-dskbytr.c5, sol:8-dskbytr.c5, sol:8-dskbytr.c6)
    //
    // DECLINED (independent validation, 2026-09-15): the cited sentence,
    // "DSKBYTR may be used to synchronize the processor to the disk
    // rotation before issuing a read or write under DMA control", is
    // programmer ADVICE about a USE of the register. It promises neither
    // that any two arbitrarily-chosen sampled instants differ, nor any
    // minimum separation, nor that idle sampling (no DMA armed -- what the
    // withdrawn form did) exercises the rotation-tracking mechanism it is
    // advising about at all: two points in a periodic byte stream may
    // legitimately repeat, and "before issuing a read or write" describes
    // WHEN a programmer would consult it, not a state this fixture can
    // hold indefinitely while nothing streams.
    //
    // The hardware proposition actually underneath this advice -- that
    // DSKBYTR's own state tracks a genuinely known incoming byte -- is
    // exactly what A9 above already drives and grades directly, against a
    // KNOWN byte bound to a KNOWN instant, not two arbitrary points 2000
    // cycles apart. Restating this claim as an inequality over two
    // unspecified samples would only produce a second, weaker, unfalsifiable
    // (in the paired direction: two genuinely different bytes COULD
    // legitimately coincide) restatement of A9's own finding. Declining.
    {
        checks.record(
            "DSKBYTR-DOES-NOT-VARY-WITH-ROTATION-OR-DIRECTION (p.248 "
            "opus:8-dskbytr.c5, sol:8-dskbytr.c5, sol:8-dskbytr.c6): the "
            "cited sentence is programmer advice about a USE of DSKBYTR "
            "('may be used to synchronize... before issuing a read or "
            "write') -- it names no required sample separation and no "
            "promise that any two arbitrarily-chosen instants must differ "
            "(two points in a periodic byte stream may legitimately "
            "repeat). The hardware fact actually underneath this advice -- "
            "that DSKBYTR tracks a genuinely known incoming byte -- is "
            "exactly what A9 above already drives and grades directly "
            "against a bound, known byte; a two-arbitrary-samples "
            "inequality test here would only restate A9's own finding "
            "under a weaker, unfalsifiable predicate. Declining.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed
              << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
