// Test purpose: HRM conformance package for the disk controller's interrupt
//   REQUEST LINES -- the p.250 "three kinds of interrupts" intro, the
//   DSKSYNC and DSKBLK bullet lines, and p.279's DMA-length-zero clause --
//   against `paula_floppy` (paula_floppy.v + paula_floppy_fifo.v only;
//   paula_floppy_ebr.v excluded, it declares a second `module paula_floppy`).
//   8 claims, from
//   hrm-testpipeline/work/ch8-s2-build/ch8-disk-interrupt-requests.jsonl,
//   reduced to 4 propositions / 8 assertions (each printed quotation cited by
//   two independent readers gets its own expect() under its own uid). Full
//   derivation in SPECIFICATION.md (committed as ee1ba33, before this file or
//   paula_floppy.v was opened).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed p.250 (disk
//   interrupt list) and p.279 (Appendix A, DSKDATR/DSKDAT register
//   description).
//
// THE BOUNDARY, confirmed by reading paula_floppy.v myself (SPECIFICATION.md
//   had only the build task's disclosed, not-yet-verified citations):
//   paula_floppy has no INTREQ port and no _ipl port anywhere in its port
//   list (:67-117, read in full). INTREQ and _ipl are paula_intcontroller
//   signals (disclosed via classify.evidence, not opened here -- out of this
//   fixture's boundary). Two of this package's four propositions print the
//   interrupt number/level in the same sentence as the request-line
//   condition ("level 5, INTREQ bit 12" on DSKSYNC; "level 1, INTREQ bit 1"
//   on DSKBLK) -- those two assertions grade the request-line half only and
//   DECLINE the number/level half via record(). The "three kinds" count
//   assertion is declined on the same principle for all three ports: whether
//   they reach the CPU needs paula_intcontroller (blckint/syncint) and,
//   for `index`, additionally `ciab` -- confirmed to be exactly what
//   hrm/ch8-disk-index-interrupt-path (its own SPECIFICATION.md, committed
//   as aeca195, read-only, not modified) is a dedicated three-module rig
//   for. p.279's DMA-length-zero quote carries no such language and needs no
//   split.
//
// RTL READ, IN ORDER, ALL AFTER SPECIFICATION.md WAS COMMITTED (ee1ba33):
//   1. hrm/ch8-drive-sense-lines/test_ch8-drive-sense-lines.cpp (committed)
//      -- for this suite's Checks class, mutating(id)/MUTATE convention, the
//      fixed // MUTATION comment form, and the twin-claim pattern (two
//      readers citing one printed sentence get two independent expect()
//      calls under their own classify.uid). No paula_floppy-specific fact
//      used below came from this; only harness mechanics.
//   2. hrm/ch8-dsklen-dma-arming/test_ch8-dsklen-dma-arming.cpp (committed)
//      -- for the DSKLEN double-write arming sequence, the 4-phase
//      clk7_en/clk7n_en tick()/settle_edges()/wait_until() clocking pattern,
//      and the feed_sector_read() SD-ingest technique this package's
//      blckint/syncint trials both reuse (with my own reg-address table and
//      my own arm_dma()/dskdatr_pop() wrappers, not copied verbatim).
//   3. nanomig/src/minimig-aga/paula_floppy.v -- full file (1041 lines).
//   4. nanomig/src/minimig-aga/paula_floppy_fifo.v -- full file (60 lines):
//      confirms the 2048-word-deep FIFO and that paula_floppy instantiates
//      nothing else, so the two-file build closes.
//
// MECHANISM, confirmed by reading paula_floppy.v myself:
//
//   blckint (:994-1036, output reg, a combinational function of dskstate):
//     DISKDMA_IDLE: blckint=0; on dma_active (=dmaen&&!lenzero&&enable),
//       nextstate=ACTIVE.
//     DISKDMA_ACTIVE: blckint=0; trackrd=~lenzero&~dsklen[14]; if
//       lenzero&&fifo_empty&&!fifo_wr_del, nextstate=INT.
//     DISKDMA_INT: blckint=1 for exactly one qualifying edge; nextstate=IDLE
//       unconditionally.
//   dsklen[15]/dmaen (:723-745): dsklen[15] loads from data_in[15] on every
//     DSKLEN write; dmaen becomes 1 only when the CURRENT write's
//     data_in[15] AND the PREVIOUS (already-latched) dsklen[15] are both 1 --
//     the double-write arming mechanism. dsklen[13:0] (the length) decrements
//     once per fifo_wr (the GENERATOR pushing a word into the FIFO, not the
//     CPU popping one) -- so lenzero is reached by the internal generator's
//     own pace, independent of whether DSKDATR is ever read. blckint's
//     `fifo_empty` term is what actually needs the CPU (DSKDATR reads) to
//     have drained everything pushed -- confirmed by the A7/A8 mutation
//     design below (popping one word short of the programmed length leaves
//     blckint silent, because fifo_empty never becomes true, not because
//     lenzero doesn't).
//   dskbytr (:699): {1'b1,(trackrd|trackwr),dsklen[14],5'b1_0000,8'h00} --
//     bit15=1 hardwired, bit14=(trackrd|trackwr), bit13=dsklen[14] (mirrors
//     the WRITE bit immediately, armed or not), bit12=1 hardwired. Idle
//     0x9000; DSKLEN written with bit14 set (0x4000) mirrors to 0xB000 --
//     this package's FIXTURE-LIVE proof.
//
//   syncint (:913-917):
//     wire sync_match = dsksync[15:0]==floppy_data && fifo_reading_sector
//                        && trackrd;
//     assign syncint = sync_match
//                       | ~dmaen & |(~_sel & motor_on & disk_present) & sof;
//     `floppy_data` (:481-500) is paula_floppy's own internally generated
//     MFM-encoded stream mux, NOT a pass-through of the injected SD bytes:
//     word 2/3 of every sector is hardwired to 16'h4489 (the standard sync
//     mark, comment at :482-485), independent of payload content. dsksync's
//     post-reset default (:704-705) is ALSO 16'h4489. So an ordinary DMA
//     read run, with DSKSYNC left untouched, produces a genuine sync_match
//     without needing to know anything about MFM-encoding the payload
//     (correcting SPECIFICATION.md's more roundabout plan, which had not
//     read this multiplexer yet and proposed steering the match through the
//     payload's own MFM encoding instead -- that path exists too, per
//     finding candidate C9, but is unnecessary once the structural sync word
//     is visible directly). The SECOND term (`~dmaen & ...`) is a genuine,
//     separate NanoMig mechanism -- a spurious syncint on `sof` whenever DMA
//     is not armed, a selected drive's motor is on, and a disk is present --
//     unrelated to DSKSYNC matching and not described on p.250 at all; it is
//     what INTERRUPT-LINES-IDLE-CONTROL's mutation exercises below, and it
//     is why A3/A4's mismatch trial deliberately arms DMA (dmaen=1 clamps
//     this term to 0) rather than leaving the fixture idle.
//     `wordsync` (an input port) gates trackrdok's wait-for-sync LATCH
//     (:919-927), not sync_match/syncint themselves (:915 does not reference
//     wordsync) -- this package runs with wordsync=0 throughout, matching
//     ch8-dsklen-dma-arming's own choice, and does not depend on the exact
//     pulse count the build task's disclosed prior measurement reports (this
//     package only needs match-vs-no-match, not a specific count).
//
//   index (:562-576):
//     reg [3:0] rpm_pulse_cnt; on every sof, wraps 0..11 (or 0..9 if ntsc).
//     assign index = |(~_sel & motor_on) & ~|rpm_pulse_cnt & sof;
//     A pure function of a *selected drive's* motor_on state, the rotation
//     counter being at 0, and sof itself -- combinational in sof, so no
//     clock edge is needed to observe it once the registered half (motor_on,
//     rpm_pulse_cnt) is settled. `motor_on[n]` (:624-659) latches ~_motor on
//     the rising (becomes-selected) edge of _sel[n], so selecting a drive
//     with _motor already driven low (commanded on) latches motor_on on that
//     same settle_edges(1).
//
// RIG HAZARD (build task, SPECIFICATION.md): dsktrack (paula_floppy.v:671-
//   680) has no reset term. This package never drives _step, so dsktrack is
//   never touched by any trial below, but per the build task's unconditional
//   mandate, a FRESH Vpaula_floppy is still allocated for every trial --
//   nothing here relies on trials never happening to need head position.
//
// DECLINED (all recorded via record(), never expect()): the number/level
//   half of A3/A4 ("level 5, INTREQ bit 12") and A5/A6 ("level 1, INTREQ bit
//   1"); the CPU-visible-interrupt half of A1/A2 for all three ports,
//   including the whole `index` journey. See the boundary paragraph above.
//   No claim in this package's 8 is fully declined -- all eight get an
//   expect()-graded assertion, four of them boundary-limited to part of
//   their printed sentence.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup -- not a compile-time #ifdef -- so one build serves the real run
//   (MUTATE unset) and every mutation run (MUTATE=<id>, one id per
//   assertion). `mutating(...)` is always called into a local `bool mut`
//   BEFORE the expect() it guards, never as a string literal inside the same
//   expect() call, per the build task's warning about the gate reading the
//   first string literal of a call as the assertion id. All ten assertions
//   below are expected GREEN at baseline -- this is a CONFIRMING package
//   (none of the 8 claims is named among the sixteen finding candidates in
//   work/2026-09-15_ch8_s2_finding_candidates.md, and the build task states
//   this subject was already measured green on a throwaway rig) -- so every
//   one gets an ordinary `stimulus red` record.
//
// MUTATION RUN LOG (measured, WSL/Verilator 5.052, 11 runs from this one
//   build -- baseline plus one run per MUTATE id; see the report accompanying
//   this commit for the full per-assertion table):
//   Baseline (MUTATE unset): 10 checks.expect() calls, 10 passed, 0 failed --
//   this package is a confirming package, and it confirmed clean. Each of
//   MUTATE=FIXTURE-LIVE, INTERRUPT-LINES-IDLE-CONTROL, opus:8-dskint.c1,
//   sol:8-int.c1, opus:8-dskint.c4, sol:8-int.c4, opus:8-dskint.c7,
//   sol:8-int.c7, opus:8-appA-dskdat.c15, sol:8-appa-dskdata.c8 was run and
//   confirmed 9 passed / 1 failed, with the ONE FAIL line in each run being
//   exactly that run's own targeted label and no other -- ten runs, ten
//   distinct single-line diffs from baseline, no cross-contamination between
//   assertions. Every run's process exit code was checked at the wsl.exe
//   call (0 for baseline, 1 for every MUTATE run), never via `echo $?`
//   inside `bash -lc`.
//
// MUTATION FIXTURE-LIVE stimulus red: second DSKLEN write driven identical to the first (0x4000 both times) instead of its WRITE-bit-clear complement, so the two DSKBYTR readings are indistinguishable (MUTATE=FIXTURE-LIVE).
// MUTATION INTERRUPT-LINES-IDLE-CONTROL stimulus red: instead of leaving the fixture genuinely idle, a drive is selected with its motor commanded on and an image mounted, then sof is pulsed -- exercising syncint's OWN separate idle-sof term (paula_floppy.v:917's second OR operand) so syncint reads asserted even though no DMA was ever armed (MUTATE=INTERRUPT-LINES-IDLE-CONTROL).
// MUTATION opus:8-dskint.c1 stimulus red: sof is never pulsed at all, so index's only trigger condition never occurs (MUTATE=opus:8-dskint.c1).
// MUTATION sol:8-int.c1 stimulus red: same omission as opus:8-dskint.c1, independent uid (MUTATE=sol:8-int.c1).
// MUTATION opus:8-dskint.c4 stimulus red: the "match" arm's DSKSYNC is ALSO written to the deliberately mismatching 0x1234 (instead of being left at its matching post-reset default 0x4489), so the match trial's pulse count drops to zero too (MUTATE=opus:8-dskint.c4).
// MUTATION sol:8-int.c4 stimulus red: same mismatch as opus:8-dskint.c4, independent uid (MUTATE=sol:8-int.c4).
// MUTATION opus:8-dskint.c7 stimulus red: DSKLEN is written only ONCE instead of twice, so dmaen never arms and the DMA this assertion needs never runs (MUTATE=opus:8-dskint.c7).
// MUTATION sol:8-int.c7 stimulus red: same single-write omission as opus:8-dskint.c7, independent uid (MUTATE=sol:8-int.c7).
// MUTATION opus:8-appA-dskdat.c15 stimulus red: DSKDATR is popped only (len-1) times instead of len, so the FIFO never actually empties and blckint stays silent within the observation window (MUTATE=opus:8-appA-dskdat.c15).
// MUTATION sol:8-appa-dskdata.c8 stimulus red: same one-pop-short omission as opus:8-appA-dskdat.c15, independent uid (MUTATE=sol:8-appa-dskdata.c8).

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
// (paula_floppy.v's own parameters DSKBYTR=9'h01a, DSKDATR=9'h008,
// DSKSYNC=9'h07e, DSKLEN=9'h024, each compared via [8:1]).
const uint8_t ADDR_NONE    = 0x00;
const uint8_t ADDR_DSKDATR = 0x04;  // $008 >> 1
const uint8_t ADDR_DSKBYTR = 0x0D;  // $01A >> 1
const uint8_t ADDR_DSKLEN  = 0x12;  // $024 >> 1
const uint8_t ADDR_DSKSYNC = 0x3F;  // $07E >> 1
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

// A fresh Verilated model per trial, per the build task's rig hazard mandate
// (dsktrack has no reset term -- see the file header). Every assertion block
// below constructs its own Rig; none is reused for a second trial.
struct Rig {
    Vpaula_floppy* dut;
    int clk7_cycle = 0;

    Rig() { dut = new Vpaula_floppy; do_reset(); }
    ~Rig() { dut->final(); delete dut; }

    Rig(const Rig&) = delete;
    Rig& operator=(const Rig&) = delete;

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
    // been applied.
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
        dut->_step = 1;      // inactive (active low): no head stepping in this package
        dut->direc = 0;
        dut->_sel = 0xF;     // all four drives deselected (active low)
        dut->side = 0;
        dut->_motor = 1;     // inactive (active low): motor commanded off
        dut->wordsync = 0;   // disabled: trackrdok latches immediately, not needed for
                              // sync_match/syncint themselves (see file header)
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

    // One qualifying-edge register write, address withdrawn afterward.
    void write_reg(uint8_t addr, uint16_t value) {
        dut->reg_address_in = addr;
        dut->data_in = value;
        settle_edges(1);
        dut->reg_address_in = ADDR_NONE;
        dut->data_in = 0;
        settle_edges(1);
    }

    // Side-effect-free combinational read (safe for DSKBYTR; NOT used for
    // DSKDATR, which pops the FIFO -- see dskdatr_pop()).
    uint16_t read_reg(uint8_t addr) {
        dut->reg_address_in = addr;
        dut->eval();
        uint16_t v = (uint16_t)dut->data_out;
        dut->reg_address_in = ADDR_NONE;
        dut->eval();
        return v;
    }

    // DSKLEN's double-write DMA-arming sequence (paula_floppy.v:723-745):
    // dmaen only becomes 1 when the CURRENT write's bit 15 AND the PREVIOUS
    // write's (already-latched) bit 15 are both set. `double_write=false`
    // performs only the first write -- deliberately leaving DMA unarmed, a
    // mutation stimulus used below.
    void arm_dma(int len, bool double_write = true) {
        write_reg(ADDR_DSKLEN, (uint16_t)(0x8000 | (len & 0x3FFF)));
        if (double_write)
            write_reg(ADDR_DSKLEN, (uint16_t)(0x8000 | (len & 0x3FFF)));
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

    // Pop exactly one word from DSKDATR's FIFO (busrd & dmaon triggers
    // fifo_rd while the address is held across a qualifying edge).
    void dskdatr_pop() {
        dut->reg_address_in = ADDR_DSKDATR;
        settle_edges(1);
        dut->reg_address_in = ADDR_NONE;
        settle_edges(1);
    }

    // Count qualifying edges, over the next n, where syncint reads high.
    int count_syncint(int n_edges) {
        int c = 0;
        for (int i = 0; i < n_edges; i++) {
            settle_edges(1);
            if (dut->syncint) c++;
        }
        return c;
    }
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    std::cout << "Starting ch8-disk-interrupt-requests...\n";

    // ========================================================================
    // FIXTURE-LIVE -- no claim id, no manual page: the build task's own
    // requirement that the fixture prove it can deliver a known, distinct
    // word before any claim-based read below is trusted. Measured on the
    // DSKBYTR WRITE-bit mirror (dsklen[14]), the simplest single-cycle case
    // available on this fixture that does not itself arm DMA.
    // ========================================================================
    {
        bool mut = mutating("FIXTURE-LIVE");
        Rig r;
        r.write_reg(ADDR_DSKLEN, 0x4000);  // WRITE bit set (bit14); bit15=0, DMA never arms
        uint16_t v1 = r.read_reg(ADDR_DSKBYTR);
        uint16_t second = mut ? (uint16_t)0x4000 : (uint16_t)0x0000;
        r.write_reg(ADDR_DSKLEN, second);
        uint16_t v2 = r.read_reg(ADDR_DSKBYTR);
        checks.note("FIXTURE-LIVE: DSKBYTR after DSKLEN=0x4000 -> " + hex4(v1) + "; after DSKLEN=" + hex4(second) +
                    (mut ? " (mutated, identical to word 1)" : "") + " -> " + hex4(v2));
        checks.expect(v1 == 0xB000 && v2 == 0x9000 && v1 != v2,
            "FIXTURE-LIVE: two distinct, known DSKLEN writes, driven through the WRITE-bit mirror in DSKBYTR, "
            "arrive unmodified and mutually distinguishable when read back -- paula_floppy.v:699 "
            "(dskbytr bit13=dsklen[14]) with :716-717 -- the register write/read-back path is live, not stuck, "
            "before any claim below is trusted");
    }

    // ========================================================================
    // INTERRUPT-LINES-IDLE-CONTROL -- this package's control case, on the
    // SAME two ports (blckint, syncint) the claims below grade: with the
    // fixture freshly reset and nothing armed or matched, both request
    // lines are expected to read deasserted -- proof that a "pulses when
    // triggered" reading in the assertions below is a real transition, not
    // a line that is simply always asserted.
    // ========================================================================
    {
        bool mut = mutating("INTERRUPT-LINES-IDLE-CONTROL");
        Rig r;
        r.settle_edges(4);
        bool blckint_idle = r.dut->blckint;
        bool syncint_idle = r.dut->syncint;
        if (mut) {
            // MUTATION: exercise syncint's OWN separate idle-sof term
            // (paula_floppy.v:917, ~dmaen & |(~_sel&motor_on&disk_present) &
            // sof) instead of leaving the fixture genuinely idle -- select a
            // drive with its motor commanded on, mount an image so
            // disk_present latches, then pulse sof. DMA is still never
            // armed (dmaen stays 0), so this is a DIFFERENT mechanism from
            // sync_match, not a contradiction of it.
            r.dut->_motor = 0;
            r.dut->_sel = 0xE;
            r.settle_edges(1);
            r.dut->sdc_image_mounted = 0x1;
            r.dut->sdc_image_size = 1000;
            r.settle_edges(4);
            r.dut->sdc_image_mounted = 0;
            r.dut->sof = 1;
            r.dut->eval();
        }
        bool blckint_after = r.dut->blckint;
        bool syncint_after = r.dut->syncint;
        checks.note("INTERRUPT-LINES-IDLE-CONTROL: idle blckint=" + std::to_string(blckint_idle) +
                    ", syncint=" + std::to_string(syncint_idle) + "; after " +
                    std::string(mut ? "exercising the idle-sof term (mutated)" : "further idle settling") +
                    " blckint=" + std::to_string(blckint_after) + ", syncint=" + std::to_string(syncint_after));
        checks.expect(!blckint_idle && !syncint_idle && !blckint_after && !syncint_after,
            "INTERRUPT-LINES-IDLE-CONTROL (control case, no manual citation, same blckint/syncint ports as the "
            "assertions below): with the fixture freshly reset and no DMA ever armed, both request lines read "
            "deasserted -- proving they are live, meaningful comparators, not lines stuck permanently asserted, "
            "before any 'pulses when triggered' claim below is trusted");
    }

    // ========================================================================
    // opus:8-dskint.c1 / sol:8-int.c1 (p.250, twins) -- P-THREE-KINDS.
    // Grades the port-count half only (see file header boundary paragraph).
    // ========================================================================
    {
        bool mut = mutating("opus:8-dskint.c1");
        Rig r;
        r.dut->_motor = 0;   // commanded on, while still deselected
        r.dut->_sel = 0xE;   // select drive 0 -- this edge latches motor_on[0]
        r.settle_edges(1);
        bool blckint_before = r.dut->blckint;
        bool syncint_before = r.dut->syncint;
        if (!mut) r.dut->sof = 1;  // MUTATION: sof never pulsed -- index's only trigger never occurs
        r.dut->eval();
        bool index_pulsed = r.dut->index;
        bool blckint_during = r.dut->blckint;
        bool syncint_during = r.dut->syncint;
        r.settle_edges(1);
        r.dut->sof = 0;
        r.dut->eval();
        checks.note("opus:8-dskint.c1: " +
                    std::string(mut ? "sof NEVER pulsed (mutated)" : "sof pulsed at rpm_pulse_cnt==0, drive0 selected, motor commanded on") +
                    " -> index=" + std::to_string(index_pulsed) +
                    ", blckint(before/during)=" + std::to_string(blckint_before) + "/" + std::to_string(blckint_during) +
                    ", syncint(before/during)=" + std::to_string(syncint_before) + "/" + std::to_string(syncint_during));
        checks.expect(index_pulsed && !blckint_before && !syncint_before && !blckint_during && !syncint_during,
            "opus:8-dskint.c1 (p.250, quote 'The disk controller can issue three kinds of interrupts:'): "
            "paula_floppy's own port list names exactly three pulse-type disk-controller signals -- blckint "
            "(paula_floppy.v:96, 'disk dma has finished interrupt'), syncint (:97, 'disk syncword found'), index "
            "(:93, 'disk index pulse') -- and this trial demonstrates index as a real, independently-triggerable "
            "THIRD pulse: with a drive selected and its motor commanded on, a sof pulse at rpm_pulse_cnt==0 "
            "asserts index (paula_floppy.v:576) while blckint and syncint (graded on their own trigger conditions "
            "below) both stay deasserted throughout -- confirming these are three distinct signals, not aliases "
            "of one wire");
        checks.record("opus:8-dskint.c1 boundary: whether these three ports -- and no other paula_floppy output "
            "-- are exactly the three interrupts the CPU receives needs paula_intcontroller (blckint/syncint's "
            "INTREQ/_ipl) and, for index, additionally ciab (the FLAG-pin journey); both outside this fixture. "
            "The index journey specifically is hrm/ch8-disk-index-interrupt-path's dedicated three-module rig "
            "(confirmed by reading its own committed SPECIFICATION.md, not modified here); not graded here.");
    }
    {
        bool mut = mutating("sol:8-int.c1");
        Rig r;
        r.dut->_motor = 0;
        r.dut->_sel = 0xE;
        r.settle_edges(1);
        bool blckint_before = r.dut->blckint;
        bool syncint_before = r.dut->syncint;
        if (!mut) r.dut->sof = 1;
        r.dut->eval();
        bool index_pulsed = r.dut->index;
        bool blckint_during = r.dut->blckint;
        bool syncint_during = r.dut->syncint;
        r.settle_edges(1);
        r.dut->sof = 0;
        r.dut->eval();
        checks.note("sol:8-int.c1: twin of opus:8-dskint.c1, same measurement -> " +
                    std::string(mut ? "sof NEVER pulsed (mutated)" : "sof pulsed") +
                    " -> index=" + std::to_string(index_pulsed) +
                    ", blckint=" + std::to_string(blckint_before) + "/" + std::to_string(blckint_during) +
                    ", syncint=" + std::to_string(syncint_before) + "/" + std::to_string(syncint_during));
        checks.expect(index_pulsed && !blckint_before && !syncint_before && !blckint_during && !syncint_during,
            "sol:8-int.c1 (p.250, quote 'The disk controller can issue three kinds of interrupts:'): twin of "
            "opus:8-dskint.c1 (same printed quote, independent reader) -- same measurement, graded independently "
            "under its own uid");
        checks.record("sol:8-int.c1 boundary: same as opus:8-dskint.c1 -- the CPU-visible-interrupt half for all "
            "three ports needs paula_intcontroller and, for index, additionally ciab; not graded here.");
    }

    // ========================================================================
    // opus:8-dskint.c4 / sol:8-int.c4 (p.250, twins) -- P-DSKSYNC-MATCH.
    // Grades the request-line half only; declines "level 5, INTREQ bit 12".
    // ========================================================================
    {
        bool mut = mutating("opus:8-dskint.c4");
        uint8_t sector[512];
        std::memset(sector, 0x00, sizeof sector);
        const int len = 8;

        // "Match" arm: DSKSYNC left at its post-reset default, 0x4489
        // (paula_floppy.v:704-705) -- the same word paula_floppy's own
        // generator injects into every sector as the structural sync mark
        // (paula_floppy.v:485), independent of the payload bytes fed below.
        Rig m;
        if (mut) m.write_reg(ADDR_DSKSYNC, 0x1234);  // MUTATION: mismatch the "match" arm too
        m.arm_dma(len);
        m.feed_sector_read(sector);
        int match_pulses = m.count_syncint(80);

        // "No-match" arm: DSKSYNC deliberately written to a value nothing in
        // this stream produces.
        Rig nm;
        nm.write_reg(ADDR_DSKSYNC, 0x1234);
        nm.arm_dma(len);
        nm.feed_sector_read(sector);
        int nomatch_pulses = nm.count_syncint(80);

        checks.note("opus:8-dskint.c4: match arm (" +
                    std::string(mut ? "mutated to 0x1234, no match expected" : "DSKSYNC=0x4489") +
                    ") syncint pulses over 80 qualifying edges=" + std::to_string(match_pulses) +
                    "; no-match arm (DSKSYNC=0x1234) syncint pulses=" + std::to_string(nomatch_pulses));
        checks.expect(match_pulses > 0 && nomatch_pulses == 0,
            "opus:8-dskint.c4 (p.250, quote '   *   DSKSYNC  (level 5,  INTREQ  bit 12) -- input stream matches "
            "the DSKSYNC register.'): with an active DMA read stream, syncint pulses when the internally "
            "generated data stream matches the programmed DSKSYNC register (paula_floppy.v:915, sync_match) and "
            "does not pulse over an identical run with a deliberately mismatching DSKSYNC value");
        checks.record("opus:8-dskint.c4 boundary: 'level 5, INTREQ bit 12' is not gradeable at paula_floppy -- "
            "INTREQ and _ipl are paula_intcontroller ports, one level up, outside this fixture; not graded here.");
    }
    {
        bool mut = mutating("sol:8-int.c4");
        uint8_t sector[512];
        std::memset(sector, 0x00, sizeof sector);
        const int len = 8;

        Rig m;
        if (mut) m.write_reg(ADDR_DSKSYNC, 0x1234);
        m.arm_dma(len);
        m.feed_sector_read(sector);
        int match_pulses = m.count_syncint(80);

        Rig nm;
        nm.write_reg(ADDR_DSKSYNC, 0x1234);
        nm.arm_dma(len);
        nm.feed_sector_read(sector);
        int nomatch_pulses = nm.count_syncint(80);

        checks.note("sol:8-int.c4: twin of opus:8-dskint.c4, same measurement -> match=" +
                    std::to_string(match_pulses) + ", no-match=" + std::to_string(nomatch_pulses));
        checks.expect(match_pulses > 0 && nomatch_pulses == 0,
            "sol:8-int.c4 (p.250, quote '   *   DSKSYNC  (level 5,  INTREQ  bit 12) -- input stream matches the "
            "DSKSYNC register.'): twin of opus:8-dskint.c4 (same printed quote, independent reader) -- same "
            "measurement, graded independently under its own uid");
        checks.record("sol:8-int.c4 boundary: same as opus:8-dskint.c4 -- 'level 5, INTREQ bit 12' needs "
            "paula_intcontroller; not graded here.");
    }

    // ========================================================================
    // opus:8-dskint.c7 / sol:8-int.c7 (p.250, twins) -- P-DSKBLK-COMPLETE.
    // Grades the request-line half only; declines "level 1, INTREQ bit 1".
    // ========================================================================
    {
        bool mut = mutating("opus:8-dskint.c7");
        const int len = 8;
        uint8_t sector[512];
        std::memset(sector, 0x00, sizeof sector);
        Rig r;
        r.write_reg(ADDR_DSKSYNC, 0x1234);  // keep this trial's log free of unrelated syncint pulses
        r.arm_dma(len, /*double_write=*/!mut);  // MUTATION: single write only -- dmaen never arms
        r.feed_sector_read(sector);
        bool blckint_seen_early = false;
        for (int i = 1; i < len; i++) {
            r.dskdatr_pop();
            if (r.dut->blckint) blckint_seen_early = true;
        }
        r.dskdatr_pop();  // the len-th pop
        bool blckint_after_final = r.wait_until([&]() { return (bool)r.dut->blckint; }, 8);
        checks.note("opus:8-dskint.c7: len=" + std::to_string(len) + ", double_write=" +
                    std::string(mut ? "NO (mutated)" : "yes") + ", blckint seen before final pop=" +
                    std::to_string(blckint_seen_early) + ", blckint asserted after final pop (within settle "
                    "margin)=" + std::to_string(blckint_after_final));
        checks.expect(!blckint_seen_early && blckint_after_final,
            "opus:8-dskint.c7 (p.250, quote '   *   DSKBLK  (level 1,  INTREQ  bit 1) -- disk DMA has "
            "completed.'): blckint stays deasserted through the first len-1 DSKDATR pops and asserts once the "
            "len-th pop drains the last word -- paula_floppy.v:994-1036 (blckint=1 only in DISKDMA_INT, reached "
            "when lenzero&&fifo_empty&&!fifo_wr_del)");
        checks.record("opus:8-dskint.c7 boundary: 'level 1, INTREQ bit 1' is not gradeable at paula_floppy -- "
            "INTREQ and _ipl are paula_intcontroller ports, one level up, outside this fixture; not graded here.");
    }
    {
        bool mut = mutating("sol:8-int.c7");
        const int len = 8;
        uint8_t sector[512];
        std::memset(sector, 0x00, sizeof sector);
        Rig r;
        r.write_reg(ADDR_DSKSYNC, 0x1234);
        r.arm_dma(len, /*double_write=*/!mut);
        r.feed_sector_read(sector);
        bool blckint_seen_early = false;
        for (int i = 1; i < len; i++) {
            r.dskdatr_pop();
            if (r.dut->blckint) blckint_seen_early = true;
        }
        r.dskdatr_pop();
        bool blckint_after_final = r.wait_until([&]() { return (bool)r.dut->blckint; }, 8);
        checks.note("sol:8-int.c7: twin of opus:8-dskint.c7, same measurement -> seen_early=" +
                    std::to_string(blckint_seen_early) + ", after_final=" + std::to_string(blckint_after_final));
        checks.expect(!blckint_seen_early && blckint_after_final,
            "sol:8-int.c7 (p.250, quote '   *   DSKBLK  (level 1,  INTREQ  bit 1) -- disk DMA has completed.'): "
            "twin of opus:8-dskint.c7 (same printed quote, independent reader) -- same measurement, graded "
            "independently under its own uid");
        checks.record("sol:8-int.c7 boundary: same as opus:8-dskint.c7 -- 'level 1, INTREQ bit 1' needs "
            "paula_intcontroller; not graded here.");
    }

    // ========================================================================
    // opus:8-appA-dskdat.c15 / sol:8-appa-dskdata.c8 (p.279, twins) --
    // P-LENZERO-BLCKINT. No boundary split needed (no level/bit language).
    // ========================================================================
    {
        bool mut = mutating("opus:8-appA-dskdat.c15");
        const int len = 4;
        uint8_t sector[512];
        std::memset(sector, 0x00, sizeof sector);
        Rig r;
        r.write_reg(ADDR_DSKSYNC, 0x1234);
        r.arm_dma(len);  // always double-write here -- the mutation is downstream, in the pop count
        r.feed_sector_read(sector);
        int pops = mut ? (len - 1) : len;  // MUTATION: pop one word short of the programmed length
        bool blckint_seen_early = false;
        for (int i = 1; i < pops; i++) {
            r.dskdatr_pop();
            if (r.dut->blckint) blckint_seen_early = true;
        }
        r.dskdatr_pop();  // the "pops"-th pop
        bool blckint_after_final = r.wait_until([&]() { return (bool)r.dut->blckint; }, 8);
        checks.note("opus:8-appA-dskdat.c15: len=" + std::to_string(len) + ", pops=" + std::to_string(pops) +
                    std::string(mut ? " (mutated, one short)" : "") + ", blckint seen early=" +
                    std::to_string(blckint_seen_early) + ", blckint after final pop=" +
                    std::to_string(blckint_after_final));
        checks.expect(!blckint_seen_early && blckint_after_final,
            "opus:8-appA-dskdat.c15 (p.279, quote 'This register is the disk DMA data buffer. It contains two "
            "bytes of data that are either sent (written) to or received (read) from the disk. The write mode is "
            "enabled by bit 14 of the LENGTH register. The DMA controller automatically transfers data to or "
            "from this register and RAM, and when the DMA data is finished (length=0) it causes a disk block "
            "interrupt. See interrupts below.'): blckint stays deasserted while any programmed word remains "
            "undrained and asserts once the DMA length reaches zero AND the last word has been drained -- "
            "paula_floppy.v:748 (lenzero), :994-1036 (blckint=1 only in DISKDMA_INT, reached when "
            "lenzero&&fifo_empty&&!fifo_wr_del)");
    }
    {
        bool mut = mutating("sol:8-appa-dskdata.c8");
        const int len = 4;
        uint8_t sector[512];
        std::memset(sector, 0x00, sizeof sector);
        Rig r;
        r.write_reg(ADDR_DSKSYNC, 0x1234);
        r.arm_dma(len);
        r.feed_sector_read(sector);
        int pops = mut ? (len - 1) : len;
        bool blckint_seen_early = false;
        for (int i = 1; i < pops; i++) {
            r.dskdatr_pop();
            if (r.dut->blckint) blckint_seen_early = true;
        }
        r.dskdatr_pop();
        bool blckint_after_final = r.wait_until([&]() { return (bool)r.dut->blckint; }, 8);
        checks.note("sol:8-appa-dskdata.c8: twin of opus:8-appA-dskdat.c15, same measurement -> pops=" +
                    std::to_string(pops) + ", seen_early=" + std::to_string(blckint_seen_early) +
                    ", after_final=" + std::to_string(blckint_after_final));
        checks.expect(!blckint_seen_early && blckint_after_final,
            "sol:8-appa-dskdata.c8 (p.279, quote 'and when the DMA data is finished (length=0) it causes a disk "
            "block interrupt.'): twin of opus:8-appA-dskdat.c15 (same printed register description, independent "
            "reader, narrower quote) -- same measurement, graded independently under its own uid");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
