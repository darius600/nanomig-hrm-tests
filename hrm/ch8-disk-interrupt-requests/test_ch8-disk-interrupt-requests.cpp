// Test purpose: HRM conformance package for the disk controller's interrupt
//   REQUEST LINES -- the p.250 "three kinds of interrupts" intro, the
//   DSKSYNC and DSKBLK bullet lines, and p.279's DMA-length-zero clause --
//   against `paula_floppy` (paula_floppy.v + paula_floppy_fifo.v only;
//   paula_floppy_ebr.v excluded, it declares a second `module
//   paula_floppy`). 8 claims, from
//   hrm-testpipeline/work/ch8-s2-build/ch8-disk-interrupt-requests.jsonl,
//   reduced to 4 propositions. Full derivation in SPECIFICATION.md
//   (committed as e6f93d0, before this file or paula_floppy.v was opened).
//
// REPAIR (2026-09-15, repair lot C, against DOKTRIN.md/LOS-C.md). Independent
//   validation of chapter 8 slice s2 found every claim in this package graded
//   only the LOCAL paula_floppy pin -- syncint, blckint, index -- for a
//   sentence that names a CPU interrupt (an INTREQ bit and a level), while
//   six of the eight uids sat on BOTH a graded line and a record() line: the
//   coverage ledger reads uids off graded lines only, so it counted every one
//   of these six as fully asserted despite the record() beside it honestly
//   saying the CPU half was missing. `tools/check_uid_placement.py` reported
//   this package as one of ten offenders in the suite, naming exactly
//   opus:8-dskint.c1/c4/c7 and sol:8-int.c1/c4/c7.
//
//   hrm/ch8-disk-interrupt-path was built to close the gap: it grades DSKSYNC
//   and DSKBLK end to end at TOP_MODULE=minimig, observing the real INTREQ
//   bit AND minimig.v's own top-level _cpu_ipl port in the SAME cycle as the
//   stimulus (15 checks.expect() calls, 15 passed / 0 failed at baseline, 0
//   uid-placement offenders, every mutation killed -- read, not modified,
//   confirmed by grep in this session before relying on it). This repair:
//     - withdraws FIXTURE-LIVE from expect() to note() -- it carries no claim
//       uid and was never conformance evidence (doctrine shape 1);
//     - withdraws opus:8-dskint.c4/sol:8-int.c4 (DSKSYNC bullet),
//       opus:8-dskint.c7/sol:8-int.c7 (DSKBLK bullet), and
//       opus:8-appA-dskdat.c15/sol:8-appa-dskdata.c8 (DSKBLK, Appendix A)
//       from expect() to record() (doctrine shape 2): each record() names
//       the hrm/ch8-disk-interrupt-path trial that now grades the claim in
//       full; each pair's own local syncint/blckint measurement is kept as
//       an un-mutated note() only, since nothing grades it as a claim
//       anymore and an orphaned mutation record travels with no expect();
//     - keeps opus:8-dskint.c1/sol:8-int.c1's own local "index is a real,
//       independently-triggerable third pulse" measurement GRADED, but moves
//       its label off the uid (P-THREE-KINDS-INDEX-PULSE-A/-B carry no uid),
//       per the mechanical placement rule: "if an assertion grades only part
//       of a claim, the measurement keeps its expect() under a label
//       carrying no uid, and the uid goes on a record() line alone." The two
//       uids now appear ONLY on the record() immediately after, which also
//       states what hrm/ch8-disk-interrupt-path and hrm/
//       ch8-disk-index-interrupt-path between them do and do not close for
//       INDEX -- matching hrm/ch8-disk-interrupt-path's own record() for
//       these same two uids, so the two declines agree rather than each
//       telling half a story.
//   After this repair `tools/check_uid_placement.py` no longer reports this
//   package as an offender (measured after rebuild -- see the report
//   accompanying this commit for the exact command and output). No RTL was
//   touched, no debug port was added, and no oracle was adjusted -- nothing
//   here changed what any assertion requires the DUT to do; only which line,
//   if any, each claim uid is graded or recorded on.
//
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed p.250 (disk
//   interrupt list) and p.279 (Appendix A, DSKDATR/DSKDAT register
//   description).
// Non-goals: anything outside the `paula_floppy` fixture this package
//   builds, and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/ch8-s2-build/ch8-disk-interrupt-requests.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// THE BOUNDARY, confirmed by reading paula_floppy.v myself (SPECIFICATION.md
//   had only the build task's disclosed, not-yet-verified citations):
//   paula_floppy has no INTREQ port and no _ipl port anywhere in its port
//   list (:67-117, read in full). INTREQ and _ipl are paula_intcontroller
//   signals (disclosed via classify.evidence, not opened here -- out of this
//   fixture's boundary). Every one of this package's claims names a CPU
//   interrupt in the same sentence as its request-line condition ("three
//   kinds of interrupts" for all three ports; "level 5, INTREQ bit 12" on
//   DSKSYNC; "level 1, INTREQ bit 1" on DSKBLK, twice, once from the chapter
//   body and once from Appendix A) -- this fixture can reach only the local
//   port (index/syncint/blckint), never INTREQ or _cpu_ipl. Per the REPAIR
//   note above: six of the eight uids are now withdrawn entirely to
//   record() (each claim graded in full at hrm/ch8-disk-interrupt-path), and
//   the remaining two (opus:8-dskint.c1/sol:8-int.c1) keep a PARTIAL local
//   measurement under an uid-less label, the uids themselves on record()
//   only, since no single package in this suite yet grades the whole
//   "three kinds" claim (index's own local-pin-to-_cpu_ipl join is still not
//   closed anywhere -- see that record() for the full picture).
//
// RTL READ, IN ORDER, ALL AFTER SPECIFICATION.md WAS COMMITTED (e6f93d0):
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
//     is why the P-DSKSYNC-MATCH local note() below (post-repair, no longer
//     graded) deliberately arms DMA (dmaen=1 clamps this term to 0) rather
//     than leaving the fixture idle.
//     `wordsync` (an input port) gates trackrdok's wait-for-sync LATCH
//     (:919-927), not sync_match/syncint themselves (:915 does not reference
//     wordsync) -- this package runs with wordsync=0 throughout, matching
//     ch8-dsklen-dma-arming's own choice.
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
// DECLINED. Post-repair: opus:8-dskint.c4/sol:8-int.c4, opus:8-dskint.c7/
//   sol:8-int.c7, and opus:8-appA-dskdat.c15/sol:8-appa-dskdata.c8 (six
//   uids) are recorded, not graded, in full -- each claim's CPU-visible half
//   was never gradeable at this paula_floppy-only boundary, and is now
//   graded in full at hrm/ch8-disk-interrupt-path (named in each record()).
//   opus:8-dskint.c1/sol:8-int.c1 (two uids) are recorded too, but this
//   package's own P-THREE-KINDS-INDEX-PULSE-A/-B stay graded under an
//   uid-less label for the part of the claim (index's local existence as a
//   genuine third pulse) they do close. See the REPAIR note at the top of
//   this file and the record() beside each withdrawn pair.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup -- not a compile-time #ifdef -- so one build serves the real run
//   (MUTATE unset) and every mutation run (MUTATE=<id>, one id per
//   assertion). `mutating(...)` is always called into a local `bool mut`
//   BEFORE the expect() it guards, never as a string literal inside the same
//   expect() call, per the build task's warning about the gate reading the
//   first string literal of a call as the assertion id. Converting an
//   expect() to record()/note() orphans its mutation record, so this repair
//   removed the record AND the runtime mutation guard for FIXTURE-LIVE,
//   opus:8-dskint.c4/sol:8-int.c4, opus:8-dskint.c7/sol:8-int.c7, and
//   opus:8-appA-dskdat.c15/sol:8-appa-dskdata.c8 -- none of those six is a
//   `mutating(...)` target any more, so a runtime-mutation replay script
//   correctly finds nothing to run for them. INTERRUPT-LINES-IDLE-CONTROL is
//   untouched by this repair (no uid before, none after, not named in
//   LOS-C.md's list). opus:8-dskint.c1/sol:8-int.c1 KEEP their runtime
//   MUTATE selectors ("opus:8-dskint.c1"/"sol:8-int.c1", internal ids, never
//   printed to stdout) unchanged, guarding the same stimulus (sof withheld)
//   under their new uid-less graded label -- the same convention hrm/
//   ch8-disk-interrupt-path itself uses for its own PROP-TWO-OF-THREE
//   pair.
//
// MUTATION RUN LOG (measured, WSL/Verilator 5.052, this repair session,
//   `make test` then one rerun per remaining `mutating(...)` id -- 4 runs
//   total from this one build; see the report accompanying this commit for
//   the full per-assertion table):
//   Baseline (MUTATE unset): 3 checks.expect() calls, 3 passed, 0 failed --
//   INTERRUPT-LINES-IDLE-CONTROL, P-THREE-KINDS-INDEX-PULSE-A, -B. Each of
//   MUTATE=INTERRUPT-LINES-IDLE-CONTROL, MUTATE=opus:8-dskint.c1 (targets
//   P-THREE-KINDS-INDEX-PULSE-A), MUTATE=sol:8-int.c1 (targets -B) was run
//   and confirmed 2 passed / 1 failed, with the ONE FAIL line in each run
//   being exactly that run's own targeted label and no other -- three runs,
//   three distinct single-line diffs from baseline, no cross-contamination.
//   Every run's process exit code was checked at the wsl.exe call (0 for
//   baseline, 1 for every MUTATE run), never via `echo $?` inside `bash -lc`.
//
// MUTATION INTERRUPT-LINES-IDLE-CONTROL stimulus red: instead of leaving the fixture genuinely idle, a drive is selected with its motor commanded on and an image mounted, then sof is pulsed -- exercising syncint's OWN separate idle-sof term (paula_floppy.v:917's second OR operand) so syncint reads asserted even though no DMA was ever armed (MUTATE=INTERRUPT-LINES-IDLE-CONTROL).
// MUTATION P-THREE-KINDS-INDEX-PULSE-A stimulus red: sof is never pulsed at all, so index's only trigger condition never occurs (MUTATE=opus:8-dskint.c1).
// MUTATION P-THREE-KINDS-INDEX-PULSE-B stimulus red: same omission as -A, independent reader (MUTATE=sol:8-int.c1).
// FIXTURE-LIVE, opus:8-dskint.c4/sol:8-int.c4, opus:8-dskint.c7/sol:8-int.c7, and opus:8-appA-dskdat.c15/sol:8-appa-dskdata.c8 carry NO mutation record any more: each was withdrawn from expect() to note()/record() in this repair (2026-09-15), and a mutation record travels with its assertion -- converting the assertion away removes the record with it. See the REPAIR note at the top of this file.

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
        // FIXTURE-LIVE carries no claim uid (build task's own precondition, not
        // conformance evidence) -- WITHDRAWN from expect() to note() per the
        // repair doctrine's shape 1. No mutation record travels with a note().
        Rig r;
        r.write_reg(ADDR_DSKLEN, 0x4000);  // WRITE bit set (bit14); bit15=0, DMA never arms
        uint16_t v1 = r.read_reg(ADDR_DSKBYTR);
        r.write_reg(ADDR_DSKLEN, 0x0000);
        uint16_t v2 = r.read_reg(ADDR_DSKBYTR);
        checks.note("FIXTURE-LIVE: DSKBYTR after DSKLEN=0x4000 -> " + hex4(v1) + "; after DSKLEN=0000 -> " +
                    hex4(v2) + " -- two distinct, known DSKLEN writes, driven through the WRITE-bit mirror in "
                    "DSKBYTR (paula_floppy.v:699, dskbytr bit13=dsklen[14], with :716-717), arrive unmodified "
                    "and mutually distinguishable when read back: " +
                    std::string((v1 == 0xB000 && v2 == 0x9000 && v1 != v2) ?
                        "the register write/read-back path is live, not stuck, before any claim below is trusted" :
                        "UNEXPECTED -- the register write/read-back path did not behave as the fixture requires"));
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
    // UID-PLACEMENT REPAIR (this package was a uid-placement offender: these
    // two uids sat on both a graded line and a record() line). This trial's
    // own measurement -- index is a real, independently-triggerable THIRD
    // local pulse, distinct from blckint/syncint -- grades only PART of the
    // "three kinds of interrupts" claim (port existence, not CPU-visibility),
    // so per the placement rule it now runs under an uid-less graded label
    // (P-THREE-KINDS-INDEX-PULSE-A/-B); the uids themselves appear ONLY on
    // the record() below, never on a graded line. The runtime MUTATE
    // selectors stay "opus:8-dskint.c1"/"sol:8-int.c1" (internal ids, never
    // printed to stdout), matching the convention hrm/ch8-disk-interrupt-path
    // itself uses for the identical situation.
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
        checks.note("P-THREE-KINDS-INDEX-PULSE-A: " +
                    std::string(mut ? "sof NEVER pulsed (mutated)" : "sof pulsed at rpm_pulse_cnt==0, drive0 selected, motor commanded on") +
                    " -> index=" + std::to_string(index_pulsed) +
                    ", blckint(before/during)=" + std::to_string(blckint_before) + "/" + std::to_string(blckint_during) +
                    ", syncint(before/during)=" + std::to_string(syncint_before) + "/" + std::to_string(syncint_during));
        checks.expect(index_pulsed && !blckint_before && !syncint_before && !blckint_during && !syncint_during,
            "P-THREE-KINDS-INDEX-PULSE-A (p.250, quote 'The disk controller can issue three kinds of "
            "interrupts:', reader one, PARTIAL closure of the port-existence half only -- see the record() "
            "immediately below for the two claim uids this measurement partially closes): paula_floppy's own "
            "port list names exactly three pulse-type disk-controller signals -- blckint (paula_floppy.v:96, "
            "'disk dma has finished interrupt'), syncint (:97, 'disk syncword found'), index (:93, 'disk index "
            "pulse') -- and this trial demonstrates index as a real, independently-triggerable THIRD pulse: "
            "with a drive selected and its motor commanded on, a sof pulse at rpm_pulse_cnt==0 asserts index "
            "(paula_floppy.v:576) while blckint and syncint (graded on their own trigger conditions below) "
            "both stay deasserted throughout -- confirming these are three distinct signals, not aliases of "
            "one wire");
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
        checks.note("P-THREE-KINDS-INDEX-PULSE-B: twin of -A, same measurement -> " +
                    std::string(mut ? "sof NEVER pulsed (mutated)" : "sof pulsed") +
                    " -> index=" + std::to_string(index_pulsed) +
                    ", blckint=" + std::to_string(blckint_before) + "/" + std::to_string(blckint_during) +
                    ", syncint=" + std::to_string(syncint_before) + "/" + std::to_string(syncint_during));
        checks.expect(index_pulsed && !blckint_before && !syncint_before && !blckint_during && !syncint_during,
            "P-THREE-KINDS-INDEX-PULSE-B (p.250, twin quote, reader two, PARTIAL closure of the port-existence "
            "half only): same three-distinct-signals measurement, graded independently as a second reader.");
        checks.record(
            "opus:8-dskint.c1 / sol:8-int.c1 boundary: P-THREE-KINDS-INDEX-PULSE-A/-B above measure, in this "
            "package, that paula_floppy exposes index as a real, independently-triggerable third local pulse, "
            "distinct from blckint and syncint -- the port-existence half of 'three kinds of interrupts'. "
            "Whether these three ports -- and no other paula_floppy output -- are exactly the three interrupts "
            "the CPU receives is NOT graded here: that needs paula_intcontroller (blckint/syncint's INTREQ/ "
            "_ipl) and, for index, additionally ciab (the FLAG-pin journey), both outside this fixture. "
            "hrm/ch8-disk-interrupt-path's own PROP-TWO-OF-THREE-PATHS-REACH-DISTINCT-CPU-LEVELS-A/-B closes "
            "TWO of the three CPU-visible paths (DSKSYNC, DSKBLK) under these same two uids, in one run, "
            "against minimig.v's real top-level _cpu_ipl port; its own record() declines the INDEX third for "
            "the identical reason stated here (needs ciab, not instantiated there). hrm/"
            "ch8-disk-index-interrupt-path further closes the 8520 Flag-pin journey and 'decodes to level 6' "
            "at paula_intcontroller's own boundary for INDEX specifically, but (per that package's own header, "
            "confirmed by ch8-disk-interrupt-path's read of it, not re-opened here) does not itself reach "
            "minimig.v's top-level _cpu_ipl in the same run. So: two of the three CPU-visible-interrupt paths "
            "are fully closed (hrm/ch8-disk-interrupt-path), INDEX's local existence is closed here, and "
            "INDEX's own local-pin-to-_cpu_ipl join in one run is still not graded anywhere in this suite -- "
            "these two declines and hrm/ch8-disk-interrupt-path's own agree on that gap, not each telling half "
            "a story.");
    }

    // ========================================================================
    // opus:8-dskint.c4 / sol:8-int.c4 (p.250, twins) -- P-DSKSYNC-MATCH.
    // WITHDRAWN (shape 2): this package's own local syncint measurement is
    // kept below as an un-mutated note() only -- the full claim ("level 5,
    // INTREQ bit 12 -- input stream matches the DSKSYNC register") is now
    // graded END TO END (real INTREQ bit 12 AND minimig.v's own _cpu_ipl at
    // level 5, in the same cycle as the stimulus) by hrm/ch8-disk-interrupt-
    // path's PROP-DSKSYNC-FULLJOIN, under these same two uids. No mutation
    // record travels with a record()-only decline.
    // ========================================================================
    {
        uint8_t sector[512];
        std::memset(sector, 0x00, sizeof sector);
        const int len = 8;
        Rig m;  // DSKSYNC left at its post-reset default 0x4489 -- matches
        m.arm_dma(len);
        m.feed_sector_read(sector);
        int match_pulses = m.count_syncint(80);
        checks.note("P-DSKSYNC-MATCH (local half only): DSKSYNC=0x4489, syncint pulses over 80 qualifying "
                    "edges=" + std::to_string(match_pulses) + " -- paula_floppy's own syncint still pulses on "
                    "a real stream match, unchanged from before this repair");
        checks.record(
            "opus:8-dskint.c4 / sol:8-int.c4 (p.250, quote '   *   DSKSYNC  (level 5,  INTREQ  bit 12) -- "
            "input stream matches the DSKSYNC register.'): declined here, not graded -- this package can only "
            "reach syncint, one paula_floppy output port, not the real INTREQ bit 12 or minimig.v's _cpu_ipl "
            "level. hrm/ch8-disk-interrupt-path's own PROP-DSKSYNC-FULLJOIN trial grades both uids fully: a "
            "real DSKSYNC stream match sets INTREQ bit 12 AND drives _cpu_ipl to the level-5 encoding, observed "
            "in the same cycle, in the same run as the stimulus. That is where these two uids are ASSERTED; "
            "this package's own local syncint pulse (noted above, unchanged) is real but does not, by itself, "
            "establish the claim's CPU-visible half.");
    }

    // ========================================================================
    // opus:8-dskint.c7 / sol:8-int.c7 (p.250, twins) -- P-DSKBLK-COMPLETE.
    // WITHDRAWN (shape 2): same reasoning as P-DSKSYNC-MATCH above -- the
    // full claim is now graded end to end by hrm/ch8-disk-interrupt-path's
    // PROP-DSKBLK-FULLJOIN, under these same two uids.
    // ========================================================================
    {
        const int len = 8;
        uint8_t sector[512];
        std::memset(sector, 0x00, sizeof sector);
        Rig r;
        r.write_reg(ADDR_DSKSYNC, 0x1234);  // keep this trial's log free of unrelated syncint pulses
        r.arm_dma(len);
        r.feed_sector_read(sector);
        bool blckint_seen_early = false;
        for (int i = 1; i < len; i++) {
            r.dskdatr_pop();
            if (r.dut->blckint) blckint_seen_early = true;
        }
        r.dskdatr_pop();  // the len-th pop
        bool blckint_after_final = r.wait_until([&]() { return (bool)r.dut->blckint; }, 8);
        checks.note("P-DSKBLK-COMPLETE (local half only): len=" + std::to_string(len) + ", blckint seen before "
                    "final pop=" + std::to_string(blckint_seen_early) + ", blckint asserted after final pop "
                    "(within settle margin)=" + std::to_string(blckint_after_final) + " -- paula_floppy's own "
                    "blckint still behaves as before this repair");
        checks.record(
            "opus:8-dskint.c7 / sol:8-int.c7 (p.250, quote '   *   DSKBLK  (level 1,  INTREQ  bit 1) -- disk "
            "DMA has completed.'): declined here, not graded -- this package can only reach blckint, one "
            "paula_floppy output port, not the real INTREQ bit 1 or minimig.v's _cpu_ipl level. hrm/"
            "ch8-disk-interrupt-path's own PROP-DSKBLK-FULLJOIN trial (dskblk_trial_singlewrite) grades both "
            "uids fully: a real disk-DMA completion sets INTREQ bit 1 AND drives _cpu_ipl to the level-1 "
            "encoding, observed in the same cycle, in the same run as the stimulus. That is where these two "
            "uids are ASSERTED; this package's own local blckint measurement (noted above, unchanged) is real "
            "but does not, by itself, establish the claim's CPU-visible half.");
    }

    // ========================================================================
    // opus:8-appA-dskdat.c15 / sol:8-appa-dskdata.c8 (p.279, twins) --
    // P-LENZERO-BLCKINT. WITHDRAWN (shape 2): the printed quote carries no
    // level/bit language of its own, but the claim it makes -- DMA
    // length-zero "causes a disk block interrupt" -- is the SAME DSKBLK
    // interrupt as opus:8-dskint.c7/sol:8-int.c7 above, and a local blckint
    // pulse no more establishes "causes an interrupt" (CPU-visible) here
    // than it did there. hrm/ch8-disk-interrupt-path's own PROP-DSKBLK-
    // FULLJOIN (dskblk_trial_dsken_gate) grades it fully, under these same
    // two uids.
    // ========================================================================
    {
        const int len = 4;
        uint8_t sector[512];
        std::memset(sector, 0x00, sizeof sector);
        Rig r;
        r.write_reg(ADDR_DSKSYNC, 0x1234);
        r.arm_dma(len);
        r.feed_sector_read(sector);
        bool blckint_seen_early = false;
        for (int i = 1; i < len; i++) {
            r.dskdatr_pop();
            if (r.dut->blckint) blckint_seen_early = true;
        }
        r.dskdatr_pop();  // the len-th pop
        bool blckint_after_final = r.wait_until([&]() { return (bool)r.dut->blckint; }, 8);
        checks.note("P-LENZERO-BLCKINT (local half only): len=" + std::to_string(len) + ", blckint seen early=" +
                    std::to_string(blckint_seen_early) + ", blckint after final pop=" +
                    std::to_string(blckint_after_final) + " -- paula_floppy's own blckint still behaves as "
                    "before this repair");
        checks.record(
            "opus:8-appA-dskdat.c15 / sol:8-appa-dskdata.c8 (p.279, quote 'This register is the disk DMA data "
            "buffer. ... and when the DMA data is finished (length=0) it causes a disk block interrupt. See "
            "interrupts below.'): declined here, not graded -- 'causes a disk block interrupt' names the same "
            "CPU-visible DSKBLK interrupt as opus:8-dskint.c7/sol:8-int.c7 above, and this package reaches only "
            "the local blckint pin, not INTREQ or _cpu_ipl. hrm/ch8-disk-interrupt-path's own PROP-DSKBLK-"
            "FULLJOIN trial (dskblk_trial_dsken_gate) grades both uids fully, cited from Appendix A's own "
            "DSKDATR description instead of the chapter-body bullet -- the same same-cycle INTREQ-bit1/level-1 "
            "measurement as opus:8-dskint.c7, now carried to INTREQ and _cpu_ipl. That is where these two uids "
            "are ASSERTED; this package's own local blckint measurement (noted above, unchanged) is real but "
            "does not, by itself, establish the claim's CPU-visible half.");
        checks.record(
            "opus:7-int.c54 (p.220, quote 'Bit 1, DSKBLK, indicates \"disk block finished.\"'), "
            "opus:7-fig7-4.c6 and opus:7-fig7-4.c39 (p.221, Figure 7-4): declined here, not graded. These "
            "three chapter-7 uids were routed to the pre-pipeline hrm/paula_floppy package, which declined "
            "them because its oracle was a frozen module specification rather than the manual; that package "
            "has been removed and its decline is carried here, on the conformance package that measures the "
            "same pins. The decline itself is unchanged in substance: 'indicates disk block finished' is a "
            "CPU-visible statement about INTREQ bit 1, and this package reaches only paula_floppy's local "
            "blckint and syncint pins -- P-DSKBLK-COMPLETE and P-DSKSYNC-MATCH above are both marked 'local "
            "half only' for exactly that reason. hrm/ch8-disk-interrupt-path is the package that carries a "
            "full join to INTREQ and _cpu_ipl, and it is where a grading of these three would belong; as of "
            "this build none of the three is cited there or anywhere else in the suite, so all three remain "
            "UNGRADED. That is a coverage gap in chapter 7, recorded rather than closed here, because "
            "closing it means deciding what Figure 7-4's two uids actually claim -- which is a reading of "
            "the figure, not a measurement this package can make.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
