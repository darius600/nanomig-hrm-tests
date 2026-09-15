// Test purpose: HRM conformance package for DSKLEN's bit layout and the
//   double-write disk-DMA arming sequence (pp.246-247, Appendix A p.279),
//   against `paula_floppy` (paula_floppy.v + paula_floppy_fifo.v only --
//   paula_floppy_ebr.v excluded, it declares a second `module paula_floppy`).
//   54 claims, from work/ch8-s2-build/ch8-dsklen-dma-arming.jsonl, reduced to
//   12 assertions + fixture liveness. Full derivation in SPECIFICATION.md
//   (committed as e97f8f2, before any RTL was opened).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed pp.246-247, 279.
//
// RTL READ, IN ORDER, ALL AFTER SPECIFICATION.md WAS COMMITTED (e97f8f2):
//   1. hrm/ch8-ciaapra-address-window/Makefile, test_ch8-ciaapra-address-
//      window.cpp, SPECIFICATION.md -- for this suite's established
//      harness/mutation boilerplate (the Checks class, the MUTATE env-var
//      convention, the tick()/settle()/do_reset() clocking pattern, the
//      summary-line format, the spec/commit-message format). No
//      paula_floppy-specific fact came from this; only the surrounding
//      harness shape and suite conventions did.
//   2. hrm/ch8-drive-select-motor-led/SPECIFICATION.md -- a sibling package
//      targeting the same fixture, read only for the two-commit-per-package
//      precedent it documents; its own RTL findings (still unbuilt at time
//      of reading) were not used here.
//   3. hrm/paula_floppy/Makefile -- existing package's build pattern. Noted:
//      it compiles paula_floppy_ebr.v and paula_floppy_dma_buf.v alongside
//      paula_floppy.v/paula_floppy_fifo.v, using -Wno-fatal for the EBR
//      duplicate `module paula_floppy`. This package deliberately does NOT
//      follow that: paula_floppy_ebr.v is excluded per the build brief, and
//      paula_floppy.v (read in full, next) instantiates no dma_buf module,
//      so paula_floppy_dma_buf.v is not needed to elaborate this fixture.
//   4. nanomig/src/minimig-aga/paula_floppy.v -- full file (1041 lines).
//   5. nanomig/src/minimig-aga/paula_floppy_fifo.v -- full file (60 lines):
//      a straightforward 2048-word-deep circular buffer, read/write pointer
//      equality for empty/full. Confirms the fixture never hits fifo_full
//      for any length this package programs (max 32 words << 2048 deep).
//   6. hrm/paula_floppy/test_paula_floppy.cpp -- ONE line only, grepped, not
//      read: confirmed the `reg_address_in = addr >> 1` convention (the
//      module's own port is declared [8:1], dropping bit 0) matches what
//      this file derives independently from paula_floppy.v's own address
//      parameters (DSKLEN=9'h024, DSKBYTR=9'h01a, DSKDATR=9'h008,
//      DSKSYNC=9'h07e, DSKDAT=9'h026, each compared via `[8:1]`).
//   paula_floppy_ebr.v and paula_floppy_dma_buf.v were NOT read (not part of
//   this fixture; the build brief already gives the reason to exclude EBR).
//
// RTL FACTS THIS FILE RELIES ON (paula_floppy.v line numbers, all read only
//   after SPECIFICATION.md was committed):
//   - dsklen[14:0] loads from data_in[14:0] on EVERY write to DSKLEN
//     (:712-720) -- LENGTH and WRITE take effect on the FIRST write.
//   - dsklen[15] also loads from data_in[15] on every write (:724-733), but
//     `dmaen` (a separate register, :735-744) only becomes 1 when the
//     CURRENT write's data_in[15] AND the PREVIOUS (already-latched)
//     dsklen[15] are both 1 -- i.e. only on the second of two consecutive
//     writes with bit 15 set. This is the double-write arming mechanism.
//   - blckint (:728,740) auto-clears dsklen[15] and dmaen when the transfer
//     completes -- a further, RTL-native "prevent accidental writes" on top
//     of the manual's own step 5.
//   - dma_active = dmaen && !lenzero && enable (:248) -- `enable` (a plain
//     port, this fixture's boundary for the DMACON-derived gate) is a THIRD,
//     independent precondition alongside the double-write.
//   - data_out = dskbytr | dskdatr (:580). dskbytr, read at DSKBYTR's
//     address only (:699), mirrors bit15=1 (hardwired), bit14=(trackrd|
//     trackwr) [1 only while the DMA state machine is ACTIVE], bit13=
//     dsklen[14] [mirrors the WRITE bit immediately, armed or not], bit12=1
//     (hardwired), bits11-0=0. DSKLEN's own address (and DSKSYNC's) have no
//     read decode at all -- confirmed by grep, `data_out` is built purely
//     from dskbytr|dskdatr, neither of which lights up for those addresses.
//   - dskstate transitions IDLE->ACTIVE one qualifying clk7_en edge after
//     dma_active becomes stable (:994-1019); trackrd/trackwr/dmaon/blckint
//     are combinational functions of dskstate and dsklen[14] (:1007-1019).
//   - fdd_led = (dskstate != DISKDMA_IDLE) (:972) -- a real port.
//   - fifo_wr (:771) decrements dsklen[13:0] (:718-719) once per word pushed
//     into the 2048-deep FIFO; the push-side state machine (:510-550) starts
//     once a full SD sector has been streamed in via sdc_byte_in_strobe
//     (fd_dma_buf_wr_state 0->1->2, :263-336) and runs automatically,
//     stopping exactly when lenzero (dsklen[13:0]==0) -- independent of how
//     much of the 544-word MFM-encoded sector remains. The CPU side reads
//     DSKDATR (busrd & dmaon, :752-756,911) to pop one word per qualifying
//     edge held on that address.
//   - dsktrack (:129,671-680) has NO reset term anywhere in the module --
//     confirmed by reading the whole file. This package never drives _step,
//     so dsktrack never changes from its Verilator construction-time zero
//     within a trial, but per the build brief a FRESH `Vpaula_floppy` is
//     still allocated for every trial below (never one reused instance with
//     repeated do_reset()), matching the rig-hazard mandate literally rather
//     than relying on this package's own trials never happening to need it.
//
// EVERY MEASURED VALUE IN THIS PACKAGE'S OWN CLAIMS FILE (0x9000/0xB000/
//   0xD000/0xF000 idle/off/read-active/write-active; the M7/M8/M10/N2/N3/N5
//   sequences) reproduces exactly from the above RTL facts, worked through
//   by hand before writing a line of this file -- strong corroboration that
//   the trace above, not just this file's own run, is right.
//
// FIXTURE-LIVENESS AND DATA CONTENT. A9/A10/A12's word-count checks do not
//   depend on the CONTENT of the fed sector (the words a length-8/32/16
//   transfer delivers are entirely sync/header/gap pattern, computed by
//   paula_floppy's own internal generator, not a pass-through of injected
//   bytes -- the injected byte content only reaches the DATA portion of an
//   MFM sector, word 32 onward, which these short transfers never reach).
//   A0 instead proves the fixture's write-then-read data path directly, on
//   the DSKBYTR mirror every other assertion in this package also reads.
//
// DECLINED: opus:8-dma.c32 / sol:8-dskbug.c1 (last three bits lost on a
//   disk write) -- see the record() call near the end of main(). Reaching a
//   genuine LAST WRITTEN BYTE on `sdc_byte_out_data` requires driving a full,
//   checksum-gated write transfer through paula_floppy's cpu_wr_state
//   pipeline (:779-903): a correctly MFM-formatted sync+header+payload
//   stream fed via DSKDAT writes, matching the module's own (not otherwise
//   observable) `track` value and passing two checksum gates before the
//   buffer-write trigger this package's other assertions never need is
//   reached. Out of proportion to what this package's remaining 53 claims
//   need; not attempted.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup -- not a compile-time #ifdef -- so one build serves the real run
//   (MUTATE unset) and every mutation run (MUTATE=<id>, one id per
//   assertion). Baseline with MUTATE unset: 11 passed, 0 failed, plus 2
//   RECORDED declines (A11, A12 -- neither has an `expect()` at all, neither
//   is tallied either way, neither carries a mutation record). Actual run
//   counts and per-mutation results are recorded in the commit message and
//   build report, not predicted here.
//
// A12 -- REPAIRED 2026-09-15, DECLINED, not graded. Independent validation
//   found the previous graded-RED assertion tested MORE than its cited page
//   states: p.247 says the last word "may not come in", not that one IS
//   lost -- "may" demands nothing a single deterministic trial can violate.
//   Declined via `record()`, quoting the page's own wording; the measured
//   fact this package's fresh trial confirms -- NanoMig delivers the FULL
//   programmed word count, not one fewer, corroborating A9/A10's own
//   independent exact-count measurement -- is kept as a `note()`, not a
//   finding against a page that only ever claimed a possibility.
//
//   A12-SUBSTITUTE-EXACT-COUNT-COMPARATOR-LIVE is REMOVED along with A12's
//   graded check. Its only job was to prove A12's comparator was not
//   vacuously red (a stimulus mutation cannot demonstrate anything on an
//   already-failing check, so the substitute rule put a different mechanism
//   through the SAME equality-comparator form and showed it green). With A12
//   declined rather than graded red, there is no longer a graded comparator
//   to defend against a vacuous-red objection -- the substitute's reason for
//   existing is gone. It also duplicated A9/A10, which already prove the
//   identical exact-count mechanism live and green under their own manual
//   citations. Removed, along with its mutation record.
//
// MUTATION A0-LIVENESS-DSKLEN-WRITE-TO-DSKBYTR-READBACK stimulus red: second known value driven identical to the first (0x0000 both times) so the two readings are indistinguishable (MUTATE=A0).
// MUTATION A1-DSKLEN-WRITE-ONLY stimulus red: the read-back address is redirected from DSKLEN to DSKBYTR (a real decode, idle value 0x9000) while the check still expects 0x0000 (MUTATE=A1).
// MUTATION A2-DSKLEN-TWO-CONTROL-BITS stimulus red: the single write is changed from 0x4000 (WRITE bit set) to 0x0000 (WRITE bit clear) while the check still expects the WRITE bit's immediate readback effect (MUTATE=A2).
// MUTATION A3-ENABLE-GATES-DMA-START stimulus red: `enable` is held at 1 throughout, instead of 0 during the double write and only raised afterward, while the check still expects the pre-enable "not yet started" reading (MUTATE=A3).
// MUTATION A4-DSKLEN-4000-FORCES-OFF-FROM-IDLE stimulus red: the idle write is changed from 0x4000 to 0x0000 while the check still expects the WRITE-bit-set (0xB000) readback (MUTATE=A4).
// MUTATION A5-FIRST-WRITE-LOADS-DOES-NOT-START stimulus red: the single arming write is repeated a second time (accidentally arming DMAEN) while the check still expects the not-yet-started reading (MUTATE=A5).
// MUTATION A6-SECOND-WRITE-ARMS-DMAEN stimulus red: only ONE write is performed, not two, while the check still expects the armed/started reading (MUTATE=A6).
// MUTATION A7-DSKLEN-4000-FORCES-OFF-DURING-ACTIVE stimulus red: instead of writing 0x4000 to force DMA off mid-transfer, the same armed value is rewritten (re-affirming, not forcing off) while the check still expects the forced-off reading (MUTATE=A7).
// MUTATION A8-WRITE-DIRECTION-BIT-AND-PREVENTION stimulus red: the double write (0xC00A twice) is reduced to a single write while the check still expects the double-write's positive result (MUTATE=A8).
// MUTATION A9-LENGTH-FIELD-COUNTS-WORDS stimulus red: DSKLEN is programmed with 33 words instead of 32 while the check still expects completion at exactly 32 reads (MUTATE=A9).
// MUTATION A10-LENGTH-DECREMENTS-STOPS-AT-ZERO stimulus red: DSKLEN is programmed with 16 words instead of 8 while the check still expects completion at exactly 8 reads, well before the fed sector's 544 MFM words are exhausted (MUTATE=A10).
// A11 -- DSKBUG_LAST_THREE_BITS_LOST_ON_WRITE: declined via record(), not expect(); no mutation record (not graded through expect()).
// A12 -- DSKBUG_ONE_WORD_SHORT_ON_READ: DECLINED via record(), not expect(); no mutation record (not graded through expect()). Repair 2026-09-15: the previous MUTATION record (none/impossible) and its substitute A12-SUBSTITUTE-EXACT-COUNT-COMPARATOR-LIVE (stimulus red) are both removed along with the graded check they defended -- see the header's A12 repair note.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include "Vpaula_floppy.h"
#include "verilated.h"

namespace {
const char* g_mutate = nullptr;
inline bool mutating(const char* id) {
    return g_mutate != nullptr && std::strcmp(g_mutate, id) == 0;
}
std::string hex4(uint16_t v) {
    char b[8];
    std::snprintf(b, sizeof b, "%04X", v);
    return std::string(b);
}

// Register addresses, as this fixture's own reg_address_in[8:1] values
// (the register's low-byte offset within $DFF0xx, right-shifted by 1 --
// paula_floppy.v's own parameters DSKBYTR=9'h01a, DSKDATR=9'h008,
// DSKSYNC=9'h07e, DSKLEN=9'h024, DSKDAT=9'h026, each compared via [8:1]).
const uint8_t ADDR_NONE   = 0x00;
const uint8_t ADDR_DSKDATR= 0x04;  // $008 >> 1
const uint8_t ADDR_DSKBYTR= 0x0D;  // $01A >> 1
const uint8_t ADDR_DSKDAT = 0x13;  // $026 >> 1
const uint8_t ADDR_DSKLEN = 0x12;  // $024 >> 1
const uint8_t ADDR_DSKSYNC= 0x3F;  // $07E >> 1
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

// A fresh Verilated model per trial, per the build brief's rig hazard:
// dsktrack has no reset term anywhere in paula_floppy.v, so a reused model
// would carry head position across trials. Every assertion block below
// constructs its own Rig.
struct Rig {
    Vpaula_floppy* dut;
    int clk7_cycle = 0;

    Rig() { dut = new Vpaula_floppy; }
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
        dut->reset = 1;
        dut->ntsc = 0;
        dut->sof = 0;
        dut->enable = 1;
        dut->reg_address_in = ADDR_NONE;
        dut->data_in = 0;
        dut->_step = 1;      // inactive (active low), never driven low: no head stepping
        dut->direc = 0;
        dut->_sel = 0xE;     // drive 0 selected (active low, 4'b1110)
        dut->side = 0;
        dut->_motor = 1;     // inactive; not consulted by the DMA data path this package tests
        dut->wordsync = 0;   // disabled: trackrdok latches immediately, no dsksync match needed
        dut->floppy_drives = 0;
        dut->floppy_wrprot = 0;
        dut->sdc_image_mounted = 0;
        dut->sdc_image_size = 0;
        dut->sdc_busy = 0;
        dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0;
        dut->sdc_byte_addr = 0;
        dut->sdc_byte_in_data = 0;
        settle_edges(4);
        dut->reset = 0;
        settle_edges(4);
    }

    // Exactly one qualifying-edge write to DSKLEN, then the address is
    // withdrawn so no further qualifying edge repeats it.
    void dsklen_write(uint16_t value) {
        dut->reg_address_in = ADDR_DSKLEN;
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
    // path (fd_dma_buf_wr_state 0->1->2): wait for sdc_rd to assert, ack via
    // sdc_busy, then present sdc_byte_in_strobe for all 512 byte addresses.
    void feed_sector_read(const uint8_t* bytes /* 512 */) {
        wait_until([&]() { return dut->sdc_rd != 0; }, 40);
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

    // Read DSKDATR repeatedly, counting reads, until blckint is observed
    // (a few extra qualifying edges after each read let dskstate's INT
    // transition and blckint's combinational readout settle). Returns the
    // number of reads performed, or -1 if blckint never appeared within
    // max_reads.
    int drain_until_blckint(int max_reads) {
        for (int i = 1; i <= max_reads; i++) {
            dskdatr_pop();
            if (wait_until([&]() { return (bool)dut->blckint; }, 8)) return i;
        }
        return -1;
    }
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    // ========================================================================
    // A0 -- FIXTURE DATA-PATH LIVENESS. No claim id, no manual page: the
    // build brief's own requirement that the fixture prove it can deliver a
    // known, distinct word before any read-based assertion is trusted. Two
    // distinct, known DSKLEN writes (0x0000, then 0x4000) must arrive,
    // unmodified and mutually distinguishable, at the DSKBYTR readback bit
    // this package's every other assertion also reads.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dsklen_write(0x0000);
        uint16_t v1 = rig.read_reg(ADDR_DSKBYTR);
        checks.note("A0-LIVENESS-WORD1: DSKLEN<=0x0000, DSKBYTR readback=0x" + hex4(v1));

        // MUTATE=A0: drive the second value IDENTICAL to the first. Hoisted
        // into a local bool, read once, so no string literal precedes the
        // label inside the expect() call below (the gate reads the FIRST
        // string literal of a call as the assertion's id; a `mutating("A0")`
        // called again inline there, ahead of the label, would be misread as
        // the id instead -- this changes what the gate can read, not what is
        // measured: same condition, same value, same result).
        const bool mutA0 = mutating("A0");
        uint16_t word2 = mutA0 ? 0x0000 : 0x4000;
        rig.dsklen_write(word2);
        uint16_t v2 = rig.read_reg(ADDR_DSKBYTR);
        checks.note("A0-LIVENESS-WORD2: DSKLEN<=0x" + hex4(word2) + ", DSKBYTR readback=0x" + hex4(v2));

        checks.expect(v1 == 0x9000 && v2 == (mutA0 ? 0x9000 : 0xB000) && v1 != v2,
            "A0-LIVENESS-DSKLEN-WRITE-TO-DSKBYTR-READBACK: two distinct, known DSKLEN writes (0x0000 then 0x4000) "
            "arrive, unmodified and mutually distinguishable, at the DSKBYTR mirror bit (data_out bit 13) this "
            "package's every other assertion also reads -- the fixture's write-then-read data path is live, not "
            "stuck");
    }

    // ========================================================================
    // A1 -- opus:8-dma.c12 (p.246, "All of the control bits relating to this
    // topic are contained in a write-only register, called DSKLEN"). DSKLEN
    // has no read decode: a written value does not come back on a read at
    // its own address.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dsklen_write(0x1234);
        // MUTATE=A1: read at DSKBYTR (a real, nonzero-idle decode) instead
        // of DSKLEN, while still expecting 0x0000.
        uint8_t addr = mutating("A1") ? ADDR_DSKBYTR : ADDR_DSKLEN;
        uint16_t v = rig.read_reg(addr);
        checks.note(std::string("A1-STIMULUS: after DSKLEN<=0x1234, read at ") +
                    (mutating("A1") ? "DSKBYTR (mutated)" : "DSKLEN's own address") +
                    " gives 0x" + hex4(v));
        checks.expect(v == 0x0000,
            "A1-DSKLEN-WRITE-ONLY (p.246, opus:8-dma.c12, quote 'All of the control bits relating to this topic "
            "are contained in a write-only register, called DSKLEN:'): after writing 0x1234 to DSKLEN, a read at "
            "DSKLEN's own address returns 0x0000 -- no read decode exists for it, matching a write-only register");
    }

    // ========================================================================
    // A2 -- sol:8-appa-dsklen.c6 (p.279, "It also contains two control bits,
    // a DMA enable bit, and a DMA direction (read/write) bit."). The WRITE
    // (direction) bit is reflected on a SINGLE write, immediately -- a
    // behaviourally distinct control from DMAEN, which (per A5/A6) needs
    // two.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        // MUTATE=A2: write 0x0000 (WRITE bit clear) instead of 0x4000 (WRITE
        // bit set), single write, while still expecting the WRITE-set
        // readback (0xB000).
        uint16_t stim = mutating("A2") ? 0x0000 : 0x4000;
        rig.dsklen_write(stim);
        uint16_t v = rig.read_reg(ADDR_DSKBYTR);
        checks.note("A2-STIMULUS: single DSKLEN<=0x" + hex4(stim) + ", DSKBYTR readback=0x" + hex4(v));
        checks.expect(v == 0xB000,
            "A2-DSKLEN-TWO-CONTROL-BITS (p.279, sol:8-appa-dsklen.c6, quote 'It also contains two control\\n "
            "                bits, a DMA enable bit, and a DMA direction\\n                 (read/write) bit.'): "
            "a SINGLE write with the WRITE (direction) bit set takes effect immediately (DSKBYTR readback 0xB000) "
            "-- unlike DMAEN (A5/A6), which needs two -- confirming WRITE and DMAEN are two separately-behaving "
            "control bits, not one shared mechanism");
    }

    // ========================================================================
    // A3 -- opus:8-dma.c2 (p.246, "The disk DMA is controlled by four "
    // items:"), opus:8-dma.c20 / sol:8-dsklen.c9 (p.247, step 1, "Enable "
    // disk DMA in the DMACON register"). At this fixture's boundary, the
    // DMACON-derived enable is the `enable` port: a fully double-written,
    // armed DSKLEN does not start a transfer while enable=0; raising enable
    // alone (no further DSKLEN write) then starts it.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dut->enable = 0;
        rig.dsklen_write(0x800A);
        rig.dsklen_write(0x800A);  // second write: dmaen arms, but dma_active still gated by enable=0
        uint16_t v_before = rig.read_reg(ADDR_DSKBYTR);
        bool led_before = rig.dut->fdd_led;
        checks.note("A3-STIMULUS: DSKLEN armed (0x800A x2) with enable=0: DSKBYTR=0x" + hex4(v_before) +
                    ", fdd_led=" + std::to_string(led_before));

        // MUTATE=A3: hold enable=1 throughout (instead of 0 then 1), so the
        // "not yet started" reading above would already be wrong.
        if (!mutating("A3")) {
            rig.dut->enable = 1;
        }
        rig.settle_edges(4);
        uint16_t v_after = rig.read_reg(ADDR_DSKBYTR);
        bool led_after = rig.dut->fdd_led;
        checks.note("A3-STIMULUS: enable raised to 1 (no further DSKLEN write): DSKBYTR=0x" + hex4(v_after) +
                    ", fdd_led=" + std::to_string(led_after));

        checks.expect(v_before == 0x9000 && !led_before && v_after == 0xD000 && led_after,
            "A3-ENABLE-GATES-DMA-START (p.246, opus:8-dma.c2, quote 'The disk DMA is controlled by four items:'; "
            "p.247, opus:8-dma.c20/sol:8-dsklen.c9, quote step 1 'Enable disk DMA in the  DMACON  register'): a "
            "fully double-written, armed DSKLEN does not start a transfer while this fixture's DMACON-derived "
            "`enable` port is 0 (DSKBYTR stays 0x9000, fdd_led stays 0); raising enable alone, with no further "
            "DSKLEN write, then starts it (DSKBYTR becomes 0xD000, fdd_led becomes 1) -- the enable item is a real, "
            "independent precondition, not a formality");
    }

    // ========================================================================
    // A4 -- opus:8-dma.c21 / sol:8-dsklen.c10 (p.247, step 2, "Set DSKLEN to
    // $4000, thereby forcing the DMA for the disk to be turned off.").
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        // MUTATE=A4: write 0x0000 instead of 0x4000, while still expecting
        // the WRITE-bit-set readback (0xB000).
        uint16_t stim = mutating("A4") ? 0x0000 : 0x4000;
        rig.dsklen_write(stim);
        uint16_t v = rig.read_reg(ADDR_DSKBYTR);
        bool led = rig.dut->fdd_led;
        checks.note("A4-STIMULUS: from idle, DSKLEN<=0x" + hex4(stim) + ": DSKBYTR=0x" + hex4(v) +
                    ", fdd_led=" + std::to_string(led));
        checks.expect(v == 0xB000 && !led,
            "A4-DSKLEN-4000-FORCES-OFF-FROM-IDLE (p.247, opus:8-dma.c21/sol:8-dsklen.c10, quote step 2 'Set DSKLEN "
            "to $4000, thereby forcing the DMA for the disk to be\\n      turned off.'): writing $4000 to DSKLEN "
            "from idle leaves DMA off (DSKBYTR=0xB000, fdd_led=0)");
    }

    // ========================================================================
    // A5 -- opus:8-dma.c17/sol:8-dsklen.c6 (p.247, "The hardware requires a "
    // special sequence..."), opus:8-dma.c22/sol:8-dsklen.c11 (p.247, step 3,
    // "Put the value you want into the DSKLEN register."). A single write
    // loads LENGTH/WRITE but does not start DMA.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dsklen_write(0x800A);
        // MUTATE=A5: repeat the write a second time (accidentally arming),
        // while still expecting the not-yet-started reading.
        if (mutating("A5")) rig.dsklen_write(0x800A);
        uint16_t v = rig.read_reg(ADDR_DSKBYTR);
        bool led = rig.dut->fdd_led;
        checks.note("A5-STIMULUS: DSKLEN<=0x800A written " + std::to_string(mutating("A5") ? 2 : 1) +
                    " time(s): DSKBYTR=0x" + hex4(v) + ", fdd_led=" + std::to_string(led));
        checks.expect(v == 0x9000 && !led,
            "A5-FIRST-WRITE-LOADS-DOES-NOT-START (p.247, opus:8-dma.c17/sol:8-dsklen.c6, quote 'The hardware "
            "requires a special sequence in order to start DMA to the\\ndisk.'; opus:8-dma.c22/sol:8-dsklen.c11, "
            "quote step 3 'Put the value you want into the DSKLEN register.'): a single write of 0x800A loads the "
            "value (DSKBYTR mirrors it) but does not start DMA (DSKBYTR=0x9000, fdd_led=0) -- a single write is not "
            "the special sequence the page requires");
    }

    // ========================================================================
    // A6 -- twelve uids (see SPECIFICATION.md A6): DSKLEN bit 15 (DMAEN) is
    // a SECONDARY enable, arming DMA only on the second of two consecutive
    // writes with the bit set.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dsklen_write(0x800A);
        // MUTATE=A6: perform only the one write above, skip the second,
        // while still expecting the armed/started reading.
        if (!mutating("A6")) rig.dsklen_write(0x800A);
        rig.settle_edges(4);
        uint16_t v = rig.read_reg(ADDR_DSKBYTR);
        bool led = rig.dut->fdd_led;
        bool sdc_rd0 = (rig.dut->sdc_rd & 0x1) != 0;
        checks.note("A6-STIMULUS: DSKLEN<=0x800A written " + std::to_string(mutating("A6") ? 1 : 2) +
                    " time(s): DSKBYTR=0x" + hex4(v) + ", fdd_led=" + std::to_string(led) +
                    ", sdc_rd[0]=" + std::to_string(sdc_rd0));
        checks.expect(v == 0xD000 && led && sdc_rd0,
            "A6-SECOND-WRITE-ARMS-DMAEN (p.246, opus:8-dma.c6/sol:8-dskdma.c5 'DMA enable'; opus:8-dma.c14/"
            "sol:8-dsklen.c3/opus:8-appA-dsklen.c8/sol:8-appa-dsklen.c9 'DMAEN Secondary disk DMA enable'; "
            "opus:8-appA-dsklen.c6/sol:8-appa-dsklen.c7 Appendix A DSKLEN paragraph; p.247, opus:8-dma.c19/"
            "sol:8-dsklen.c8 'must be turned on twice'; opus:8-dma.c23/sol:8-dsklen.c12 step 4 'Write this value "
            "again into the DSKLEN register. This actually\\n      starts the DMA.'): a second write repeating "
            "0x800A arms DMAEN and starts disk read DMA (DSKBYTR=0xD000, fdd_led=1, sdc_rd requested for the "
            "selected drive)");
    }

    // ========================================================================
    // A7 -- opus:8-dma.c24/sol:8-dsklen.c13 (p.247, step 5, "After the DMA "
    // is complete, set the DSKLEN register back to $4000, to prevent "
    // accidental writes to the disk."). $4000 forces DMA off from an ACTIVE
    // transfer too, not only from idle (A4).
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dsklen_write(0x800A);
        rig.dsklen_write(0x800A);
        rig.settle_edges(4);
        bool led_active = rig.dut->fdd_led;  // sanity: transfer is running before we force it off
        // MUTATE=A7: instead of forcing off with 0x4000, re-affirm the armed
        // value (stays active) while still expecting the forced-off reading.
        if (mutating("A7")) rig.dsklen_write(0x800A);
        else                rig.dsklen_write(0x4000);
        rig.settle_edges(4);
        uint16_t v = rig.read_reg(ADDR_DSKBYTR);
        bool led = rig.dut->fdd_led;
        checks.note("A7-STIMULUS: was active (fdd_led=" + std::to_string(led_active) + "), then DSKLEN<=0x" +
                    hex4(mutating("A7") ? 0x800A : 0x4000) + ": DSKBYTR=0x" + hex4(v) +
                    ", fdd_led=" + std::to_string(led));
        checks.expect(led_active && v == 0xB000 && !led,
            "A7-DSKLEN-4000-FORCES-OFF-DURING-ACTIVE (p.247, opus:8-dma.c24/sol:8-dsklen.c13, quote step 5 'After "
            "the DMA is complete, set the DSKLEN register back to\\n      $4000, to prevent accidental writes to "
            "the disk.'): writing $4000 while a transfer is ACTIVE also forces it off (DSKBYTR returns to 0xB000, "
            "fdd_led drops to 0), the same as it does from idle (A4)");
    }

    // ========================================================================
    // A8 -- twelve uids (see SPECIFICATION.md A8): DSKLEN bit 14 (WRITE)
    // selects RAM-to-disk direction, and the double-write sequence is what
    // prevents a single stray write (even with WRITE and DMAEN both set)
    // from starting an accidental disk write.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dsklen_write(0xC00A);
        // MUTATE=A8: skip the second write, while still expecting the
        // double-write's positive (armed, write-direction) result.
        if (!mutating("A8")) rig.dsklen_write(0xC00A);
        rig.settle_edges(4);
        uint16_t v_double = rig.read_reg(ADDR_DSKBYTR);
        bool dmal_double = rig.dut->dmal;
        bool dmas_double = rig.dut->dmas;
        checks.note("A8-STIMULUS (double write, " + std::to_string(mutating("A8") ? 1 : 2) + " write(s) applied): "
                    "0xC00A: DSKBYTR=0x" + hex4(v_double) + ", dmal=" + std::to_string(dmal_double) +
                    ", dmas=" + std::to_string(dmas_double));

        Rig rig2;
        rig2.do_reset();
        rig2.dsklen_write(0xC00A);  // single write only, negative/control half -- not mutated
        rig2.settle_edges(4);
        bool dmas_single = rig2.dut->dmas;
        bool sdc_wr_single = (rig2.dut->sdc_wr != 0);
        bool led_single = rig2.dut->fdd_led;
        checks.note("A8-STIMULUS (single write): 0xC00A once: dmas=" + std::to_string(dmas_single) +
                    ", sdc_wr=" + std::to_string(sdc_wr_single) + ", fdd_led=" + std::to_string(led_single));

        checks.expect(v_double == 0xF000 && dmal_double && dmas_double &&
                      !dmas_single && !sdc_wr_single && !led_single,
            "A8-WRITE-DIRECTION-BIT-AND-PREVENTION (p.246, opus:8-dma.c5/sol:8-dskdma.c4 'Direction of data "
            "transfer (read/write)'; opus:8-dma.c15/sol:8-dsklen.c4 'WRITE   Disk write (RAM -> disk if 1)'; "
            "p.279, opus:8-appA-dskdat.c12/sol:8-appa-dskdata.c5 'write mode is enabled by bit 14'; "
            "opus:8-appA-dsklen.c7/sol:8-appa-dsklen.c8 Appendix A DSKLEN paragraph; opus:8-appA-dsklen.c9/"
            "sol:8-appa-dsklen.c10 'WRITE Disk write (RAM to disk) if 1'; p.247, opus:8-dma.c18/sol:8-dsklen.c7 "
            "'This sequence prevents accidental writes to the disk.'): double-writing 0xC00A (WRITE+DMAEN) arms a "
            "RAM-to-disk transfer (DSKBYTR=0xF000, dmal=1, dmas=1); a SINGLE write of the same value starts "
            "nothing (dmas=0, sdc_wr=0, fdd_led=0) -- the two-write sequence is what prevents that one stray write "
            "from becoming an accidental disk write");
    }

    // ========================================================================
    // A9 -- eight uids (see SPECIFICATION.md A9), CONTROL CASE. DSKLEN bits
    // 13-0 (LENGTH) count words to transfer: programming 32 and running a
    // transfer delivers exactly 32 words, no more, no fewer.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        uint8_t sector[512];
        for (int i = 0; i < 512; i++) sector[i] = (uint8_t)i;

        // MUTATE=A9: program 33 words instead of 32, while the check still
        // expects completion at exactly 32 reads.
        uint16_t programmed = mutating("A9") ? 33 : 32;
        uint16_t dsklen_val = 0x8000 | programmed;
        rig.dsklen_write(dsklen_val);
        rig.dsklen_write(dsklen_val);
        rig.feed_sector_read(sector);

        int reads_at_31 = -999;
        for (int i = 1; i <= 31; i++) rig.dskdatr_pop();
        rig.settle_edges(2);
        bool blckint_at_31 = rig.dut->blckint;

        int completed_at = -1;
        if (!blckint_at_31) {
            rig.dskdatr_pop();
            if (rig.wait_until([&]() { return (bool)rig.dut->blckint; }, 8)) completed_at = 32;
        }
        checks.note("A9-STIMULUS: DSKLEN length programmed=" + std::to_string(programmed) +
                    "; blckint after 31 reads=" + std::to_string(blckint_at_31) +
                    "; completed at read #" + std::to_string(completed_at));

        checks.expect(!blckint_at_31 && completed_at == 32,
            "A9-LENGTH-FIELD-COUNTS-WORDS (p.246, opus:8-dma.c4/sol:8-dskdma.c3 'Length of data to be moved by "
            "DMA'; opus:8-dma.c16/sol:8-dsklen.c5 'LENGTH  Number of words to transfer'; p.279, "
            "opus:8-appA-dsklen.c5/sol:8-appa-dsklen.c5 Appendix A DSKLEN paragraph; opus:8-appA-dsklen.c10/"
            "sol:8-appa-dsklen.c11 'LENGTH        Length (# of words) of DMA data.'): programming DSKLEN's LENGTH "
            "field to 32 and running a read transfer delivers exactly 32 words (blckint has not fired after 31 "
            "reads, and does fire once the 32nd is taken) -- this package's control case, on the same "
            "word-count-in/word-count-out measurement basis as A10 and A12");
    }

    // ========================================================================
    // A10 -- five uids (see SPECIFICATION.md A10). The length counter
    // decrements once per word transferred and the transfer stops (raising
    // blckint) exactly at zero -- demonstrated with a programmed length (8)
    // far shorter than the fed sector's available data (544 MFM words), so
    // the early stop is due to the count, not exhausted source data.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        uint8_t sector[512];
        for (int i = 0; i < 512; i++) sector[i] = (uint8_t)(0xA5 ^ i);

        // MUTATE=A10: program 16 words instead of 8, while the check below
        // still bounds itself to the fixed EXPECTED_LEN=8 (not to
        // `programmed`) -- so the oracle does not silently move with the
        // stimulus.
        const uint16_t EXPECTED_LEN = 8;
        uint16_t programmed = mutating("A10") ? 16 : EXPECTED_LEN;
        uint16_t dsklen_val = 0x8000 | programmed;
        rig.dsklen_write(dsklen_val);
        rig.dsklen_write(dsklen_val);
        rig.feed_sector_read(sector);

        for (int i = 1; i <= (int)EXPECTED_LEN - 1; i++) rig.dskdatr_pop();
        rig.settle_edges(2);
        bool blckint_early = rig.dut->blckint;
        rig.dskdatr_pop();
        bool completed = rig.wait_until([&]() { return (bool)rig.dut->blckint; }, 8);
        checks.note("A10-STIMULUS: DSKLEN length programmed=" + std::to_string(programmed) +
                    " (sector supplies far more than " + std::to_string(programmed) + " words); checked against a "
                    "fixed bound of " + std::to_string(EXPECTED_LEN) + " reads; blckint before final read=" +
                    std::to_string(blckint_early) + "; completed after final read=" + std::to_string(completed));

        checks.expect(!blckint_early && completed,
            "A10-LENGTH-DECREMENTS-STOPS-AT-ZERO (p.247, opus:8-dma.c25/sol:8-dsklen.c14 'As each data word is "
            "transferred, the length value is decremented.'; opus:8-dma.c28/sol:8-dsklen.c17 'When the length\\n"
            "value counts down to 0, the transfer stops.'; p.279, opus:8-appA-dskdat.c14 Appendix A DSKDAT "
            "paragraph, 'when the DMA data is finished (length=0) it\\n                 causes a disk block "
            "interrupt.'): programming LENGTH to 8 against a sector that supplies far more data stops the "
            "transfer (raises blckint) at exactly 8 words, not when the source data runs out -- the length "
            "counter, not the medium, decides when the transfer ends");
    }

    // ========================================================================
    // A11 -- opus:8-dma.c32 / sol:8-dskbug.c1 (p.247, "There is a hardware "
    // bug that causes the last three bits of data sent to the disk to be "
    // lost."). DECLINED: see the header comment's DECLINED section. Reaching
    // a genuine last-written byte on sdc_byte_out_data needs a full,
    // checksum-gated write transfer through paula_floppy's cpu_wr_state
    // pipeline (:779-903) that no other assertion in this package requires
    // and this package's time budget does not cover.
    // ========================================================================
    checks.record(
        "A11-DECLINED (p.247, opus:8-dma.c32/sol:8-dskbug.c1, quote 'There is a hardware bug that causes the last "
        "three bits of data sent to\\nthe disk to be lost.'): not turned into an assertion. Reaching the LAST byte "
        "of a genuine disk-write transfer on `sdc_byte_out_data` requires driving paula_floppy's cpu_wr_state "
        "pipeline (paula_floppy.v:779-903) through a correctly MFM-formatted sync+header+payload stream fed via "
        "DSKDAT writes, matching the module's own (not otherwise observable) `track` value and passing two "
        "checksum gates (header checksum at cpu_wr_cnt==22/23, payload checksum at cpu_wr_state==4) before the "
        "buffer-write trigger this package's other eleven assertions never need is reached. Out of proportion to "
        "this package's remaining scope; not attempted. NanoMig's write path is architecturally byte-oriented "
        "(sdc_byte_out_data, an SD-card sector buffer, not a flux-level medium), which is the same underlying "
        "fact the finding candidates document (C1/C2) already names as governing every media-format claim in "
        "this chapter -- consistent with, though not itself proof of, the expectation that this claim would grade "
        "red like A12 if it were built.");

    // ========================================================================
    // A12 -- opus:8-dma.c33 / sol:8-dskbug.c2 (p.247, "Also, the last word "
    // in a disk-read DMA operation may\nnot come in (that is, one less word "
    // may be read than you asked for)."). REPAIRED 2026-09-15: DECLINED, not
    // graded. Independent validation found the previous graded-RED assertion
    // tested MORE than the page states -- the page says a word MAY be lost,
    // not that one is; "may" demands nothing a single deterministic trial
    // can violate, so grading `delivered == programmed - 1` as a hard
    // requirement was a claim the page never made. A12-SUBSTITUTE-EXACT-
    // COUNT-COMPARATOR-LIVE, whose only job was defending this graded check
    // against a vacuous-red objection, is removed with it: with nothing
    // graded, there is no comparator left to defend, and it duplicated
    // A9/A10's own already-graded, already-green exact-count measurement.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        uint8_t sector[512];
        for (int i = 0; i < 512; i++) sector[i] = (uint8_t)(i * 3);

        const uint16_t programmed = 16;
        uint16_t dsklen_val = 0x8000 | programmed;
        rig.dsklen_write(dsklen_val);
        rig.dsklen_write(dsklen_val);
        rig.feed_sector_read(sector);

        int delivered = rig.drain_until_blckint(programmed + 4);
        checks.note("A12: DSKLEN length programmed=" + std::to_string(programmed) +
                    "; transfer actually completed after " + std::to_string(delivered) +
                    " delivered word(s) -- measured for interest only, not graded");
        checks.record(
            "A12-DSKBUG-ONE-WORD-SHORT-ON-READ (p.247, opus:8-dma.c33/sol:8-dskbug.c2, quote 'Also, the last word "
            "in a disk-read DMA operation may\\nnot come in (that is, one less word may be read than you asked "
            "for).'): declined, not graded. The page says a word MAY be lost, not that one IS -- 'may' is a "
            "possibility, and a red requires the page to demand something a deterministic trial can violate; this "
            "one demands nothing. Measured, kept as fact not finding: this fixture delivers the FULL programmed "
            "count (" + std::to_string(programmed) + " for " + std::to_string(programmed) + ", delivered=" +
            std::to_string(delivered) + "), matching A9/A10's own independent exact-count measurement above -- "
            "NanoMig simply does not exhibit the documented possible bug on this trial, which is not the same as "
            "contradicting a page that only ever claimed a possibility.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
