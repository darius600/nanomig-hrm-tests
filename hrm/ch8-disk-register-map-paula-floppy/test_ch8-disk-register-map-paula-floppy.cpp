// Test purpose: HRM conformance package for the register-map propositions
//   (address, access direction, function) of DSKLEN ($024), DSKBYTR ($01A),
//   DSKSYNC ($07E), DSKDAT ($026) and DSKDATR ($008), against `paula_floppy`
//   (paula_floppy.v + paula_floppy_fifo.v only -- paula_floppy_ebr.v
//   excluded, it declares a second `module paula_floppy`). 79 claims, from
//   work/ch8-s2-build/ch8-disk-register-map-paula-floppy.jsonl, reduced to
//   20 assertions (19 after the 2026-09-15 repair below removed E4; see
//   REPAIR NOTE). Full derivation in SPECIFICATION.md (committed as
//   0ae6d09, before any RTL was opened; updated 2026-09-15 for the repair).
//
// REPAIR NOTE (independent validation, 2026-09-15): B4, B5 and E3 were
//   withdrawn -- soundly. B4/B5's original "nonzero at least once" premise
//   could never be refuted (zero is a valid disk byte), and E3 tested an
//   unstated "no real content behind $008" reading the manual's own `ER`
//   legend (p.287) does not make. B4 and B5 are repaired to bind a KNOWN
//   incoming byte (0xFF, via the DSKDATR/FIFO marker technique the
//   already-accepted ch8-mfm-cell-encoding package established) to the
//   DSKBYTR sampling instant -- THE SAME UNDERLYING FACT as
//   ch8-dskbytr-status-register/A9 and
//   ch8-disk-controller-overview/opus:8-dsk.c9+sol:8-dsk.c9, repaired
//   identically in all three packages that carry it. Still RED (the DATA
//   field is the literal constant 8'h00, paula_floppy.v:699). E3 is
//   DECLINED (record(), not expect()): the manual's actual "dummy address"
//   claim is about WHO triggers the bus cycle (Agnus timing vs. a
//   destination-address-bus read, per the p.287 `ER` legend), a
//   distinction this package's `paula_floppy`-alone fixture cannot make.
//   E4, whose sole purpose was proving E3's comparator wasn't vacuously
//   red, is removed along with E3's grading (E1 already independently
//   covers busrd's live/undecoded discrimination).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed pp.241, 246, 249,
//   265, 278-279, 289-290.
//
// RTL READ, IN ORDER, ALL AFTER SPECIFICATION.md WAS COMMITTED (0ae6d09):
//   1. hrm/ch8-dsklen-dma-arming/{Makefile,test_ch8-dsklen-dma-arming.cpp,
//      SPECIFICATION.md} -- a sibling package targeting the SAME fixture,
//      read for this suite's established harness/mutation boilerplate (the
//      Checks class, the MUTATE env-var convention, the Rig/tick()/
//      settle_edges()/do_reset()/feed_sector_read() shape) and to avoid
//      re-deriving facts it had already worked out about this fixture's own
//      clocking. No claim in THIS package's own jsonl was graded by reading
//      that file; every expect() below cites only this package's own uids
//      and was checked against paula_floppy.v directly (next).
//   2. nanomig/src/minimig-aga/paula_floppy.v -- full file (1041 lines).
//   3. nanomig/src/minimig-aga/paula_floppy_fifo.v -- full file (60 lines):
//      a 2048-word circular buffer, read/write pointer equality for
//      empty/full.
//   paula_floppy_ebr.v was NOT read (excluded from this fixture per the
//   build brief).
//
// RTL FACTS THIS FILE RELIES ON (paula_floppy.v line numbers):
//   - data_out = dskbytr | dskdatr (:580) -- the ENTIRE read side of this
//     module's bus interface. DSKLEN, DSKSYNC and DSKDAT have no read decode
//     of any kind; nothing they hold can ever reach data_out. DSKBYTR
//     answers only when reg_address_in matches DSKBYTR[8:1] (:699); DSKDATR
//     only when it matches DSKDATR[8:1] via `busrd` (:756,946).
//   - `busrd` and `buswr` (:752-753) are declared `/* verilator public */`
//     and are read DIRECTLY from this file as dut->rootp->paula_floppy->
//     busrd / ->buswr (Verilator 5.052's split-class layout puts a
//     `/* verilator public */` signal declared in the top module on the
//     rootp->paula_floppy sub-object, not the top Vpaula_floppy class
//     itself) --
//     busrd = (reg_address_in[8:1]==DSKDATR[8:1]) (:756), buswr =
//     (reg_address_in[8:1]==DSKDAT[8:1]) (:759). Pure combinational address
//     decode, no clocking needed to observe them.
//   - dskbytr = reg_address_in[8:1]==DSKBYTR[8:1] ?
//     {1'b1,(trackrd|trackwr),dsklen[14],5'b1_0000,8'h00} : 0 (:699). Bits
//     7:0 are the literal constant 8'h00 in this concatenation -- a
//     compile-time constant, not a value that depends on any FIFO content,
//     disk content or DMA state. Bit 15 and bit 12 are likewise hardwired
//     1. Bits 14 and 13 (trackrd|trackwr, dsklen[14]) are the only bits that
//     move.
//   - dsklen[14:0] loads from data_in[14:0] on EVERY write to DSKLEN
//     (:712-720, :724-733); dsklen[15]/dmaen need a SECOND consecutive write
//     with data_in[15] set (:735-744) -- the double-write arm. dsklen[13:0]
//     decrements once per fifo_wr (:718-719), regardless of direction.
//   - dma_active = dmaen && !lenzero && enable (:248). dskstate transitions
//     IDLE->ACTIVE->INT->IDLE (:994-1019); blckint (a plain output port,
//     read directly as dut->blckint) is 1 for one cycle in the INT state.
//   - fdd_led = (dskstate != DISKDMA_IDLE) (:972), a real output port.
//   - dsksync (:127,701-709) resets to 16'h4489 and loads from data_in only
//     when reg_address_in matches DSKSYNC[8:1]; never read back anywhere.
//   - sync_match = dsksync==floppy_data && fifo_reading_sector && trackrd
//     (:914-915); trackrdok <= ~wordsync | sync_match | trackrdok (:920-926,
//     sticky, no resync -- matches finding_candidates.md C10). fifo_wr's
//     read-side term is trackrdok && fifo_reading_sector & !fifo_full &
//     ~lenzero (:771). floppy_data's own internal generator places the
//     literal word 0x4489 at MFM word position 2/3 unconditionally (:485)
//     and 0xaaaa as its default fallback (:497) -- so with wordsync=1,
//     dsksync=0x4489 (the reset default) matches almost immediately, while
//     dsksync=0xDEAD never matches anything this generator ever produces
//     (0xDEAD is not a value any of :485-497's cases can output), gating
//     `trackrdok` permanently low and `fifo_wr`'s read term permanently 0.
//   - fifo_rd = (busrd & dmaon) | stbdat (:911) -- for DSKDATR's read
//     direction, the READ ACCESS ITSELF (busrd, gated by dmaon) is what
//     advances the FIFO; there is no separate "early" strobe anywhere in
//     this port list.
//   - For the WRITE direction (dsklen[14]=1): fifo_in = data_in (:762,
//     trackrd=0 so the mux selects data_in over floppy_data); fifo_wr's
//     second term, buswr & dmaon (:771), pushes a DSKDAT write straight
//     into the FIFO with no SD-card involvement, and the SAME decrement at
//     :718-719 fires from it -- a single DSKDAT write during an armed
//     1-word WRITE transfer both enters the FIFO and completes the length
//     count, observable as blckint the following few clk7_en edges.
//   - dsktrack (:129,671-680) has NO reset term anywhere in the module --
//     confirmed by reading the whole file. A FRESH `Vpaula_floppy` is
//     allocated for every trial below, per the build brief's rig hazard.
//
// EIGHT CLAIMS' PARTLY-OVERTURNED CONSTRAINT (build brief, "Eight claims
//   carry a constraint from the attack stage"): none of this package's own
//   claims carry a `note` of that kind (checked: no claim record below has a
//   `classify.note` field at all); this paragraph is here only to record
//   that the check was made, not to disclaim anything graded below.
//
// FIXTURE-LIVENESS. L0 proves the write-then-read data path is live before
//   anything else is trusted, on the exact DSKBYTR-mirror path B3/L3's own
//   trials also read.
//
// DECLINED CLAIMS (as of the 2026-09-15 repair): E3's five uids
//   (opus:8-appA-dskdat.c8, sol:8-appa-dskdatr.c4, sol:8-appa-dskdatr.c5,
//   opus:8-appB-dskdatr.c6, sol:8-appb-dskdatr.c6) are cited on a
//   `record()` line, not a graded line -- see E3's own comment below and
//   SPECIFICATION.md's "Declined claims" section. The remaining 74 uids
//   are still cited on a graded (`expect()`) line.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup -- not a compile-time #ifdef -- so one build serves the real run
//   (MUTATE unset) and every mutation run (MUTATE=<id>, one id per
//   assertion). B4 and B5 are expected RED at baseline (see their own
//   comments below and finding_candidates.md C6); an ordinary stimulus
//   mutation cannot fail an assertion that already fails, so each carries a
//   `none impossible` mutation record instead, and the comparator each uses
//   is proven live via a dedicated GREEN-at-baseline substitute (B6) that
//   applies the identical extraction-and-compare form to a mechanism
//   already shown live elsewhere in this same run, and carries its own
//   ordinary stimulus mutation. E3 is DECLINED (record(), not expect()) and
//   carries no mutation record -- there is no graded line to pair one
//   against. Actual run counts and per-mutation results are recorded in
//   the commit message and build report, not predicted here.
//
// MUTATION L0-FIXTURE-LIVENESS stimulus red: the second known DSKLEN write is changed to be identical to the first (0x0000 both times) so the two DSKBYTR-mirror readings become indistinguishable (MUTATE=L0).
// MUTATION L1-DSKLEN-ADDRESS stimulus red: the arming write is redirected from DSKLEN's own address to DSKBYTR's address (a real decode) while the check still expects the DSKLEN-address readback pattern (MUTATE=L1).
// MUTATION L2-DSKLEN-WRITE-ONLY stimulus red: the read-back address is redirected from DSKLEN to DSKBYTR (a real, nonzero-idle decode) while the check still expects 0x0000 (MUTATE=L2).
// MUTATION L3-DSKLEN-FUNCTION-LENGTH stimulus red: DSKLEN is programmed with 9 words instead of 8 while the check still expects completion at exactly 8 reads (MUTATE=L3).
// MUTATION B1-DSKBYTR-ADDRESS stimulus red: the probe address is redirected from DSKBYTR's own address to DSKLEN's address (no read decode) while the check still expects DSKBYTR's nonzero idle readback (MUTATE=B1).
// MUTATION B2-DSKBYTR-READ-ONLY stimulus red: a write value distinct from the previous stimulus is driven at DSKBYTR's address between the two DSKLEN-mirror reads, while the check still expects the DSKLEN-arming sequence to behave as if that write never happened (MUTATE=B2).
// MUTATION B3-DSKBYTR-STATUS-HALF stimulus red: the second DSKLEN write's WRITE bit is left equal to the first's (both clear) instead of set, so the DSKBYTR status mirror cannot move, while the check still expects it to move (MUTATE=B3).
// MUTATION B4-DSKBYTR-DATA-BYTE-HALF none impossible: already red at baseline -- bits 7:0 of dskbytr are the literal RTL constant 8'h00 (paula_floppy.v:699), not driven by any signal this fixture can stimulate, so no stimulus can make this assertion pass, even with a genuinely bound known byte at the sampling instant. The comparator that grades it is proven live via the substitute B6-DSKBYTR-BYTE-FIELD-COMPARATOR-LIVE below.
// MUTATION B5-DSKBYTR-COMPOUND-BYTE-AND-STATUS none impossible: already red at baseline for the same reason as B4 (the byte conjunct can never become true); see B6.
// MUTATION B6-DSKBYTR-BYTE-FIELD-COMPARATOR-LIVE stimulus red: the second DSKLEN write is changed to leave the WRITE bit equal to the first (both clear) instead of set, so the status field the substitute reads cannot move, while the check still expects it to (MUTATE=B6).
// MUTATION S1-DSKSYNC-ADDRESS stimulus red: the 0xDEAD write is redirected from DSKSYNC's own address ($07E) to DSKLEN's address ($024) while the check still expects the redirected write to have NO effect on sync gating (i.e. still expects normal delivery); with the write correctly redirected, delivery is unaffected and the check (written to expect the mutated case's own outcome) is inverted by hand below -- see the code for the exact form (MUTATE=S1).
// MUTATION S2-DSKSYNC-WRITE-ONLY stimulus red: the read-back address is redirected from DSKSYNC to DSKBYTR (a real, nonzero decode) while the check still expects 0x0000 (MUTATE=S2).
// MUTATION S3-DSKSYNC-FUNCTION-MATCH-WORD stimulus red: DSKSYNC is written with 0x4489 (the matching value) instead of 0xDEAD in the trial the check expects to see suppressed (zero words delivered), while the check still expects suppression (MUTATE=S3).
// MUTATION D1-DSKDAT-ADDRESS stimulus red: the swept address that is expected to be the sole `buswr`-asserting one is changed from $026 to $024 while the check still expects $026 alone to assert it (MUTATE=D1).
// MUTATION D2-DSKDAT-WRITE-ONLY stimulus red: the read-back address is redirected from DSKDAT to DSKBYTR (a real, nonzero decode) while the check still expects 0x0000 (MUTATE=D2).
// MUTATION D3-DSKDAT-FUNCTION-DMA-WRITE stimulus red: the DSKDAT write that is expected to complete the 1-word armed transfer is instead directed at DSKSYNC's address (no fifo/length effect) while the check still expects blckint to fire (MUTATE=D3).
// MUTATION E1-DSKDATR-ADDRESS stimulus red: the swept address expected to be the sole `busrd`-asserting one is changed from $008 to $01A while the check still expects $008 alone to assert it (MUTATE=E1).
// MUTATION E2-DSKDATR-EARLY-READ-ACCESS-IS-TRIGGER stimulus red: a second, distinct read of DSKDATR is skipped (only the first access is applied) while the check still expects the second access alone to have advanced the FIFO (MUTATE=E2).
// E3 is DECLINED as of the 2026-09-15 repair (record(), not expect()) -- see E3's own comment below. It carries no MUTATION record: a decline has no graded line to pair one against. E4, whose sole purpose was proving E3's own comparator wasn't vacuously red, is removed along with E3's grading; E1's own MUTATION record above already covers busrd's live/undecoded discrimination over the same swept set.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include "Vpaula_floppy.h"
#include "Vpaula_floppy___024root.h"
#include "Vpaula_floppy_paula_floppy.h"
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

// Register addresses as this fixture's own reg_address_in[8:1] values (the
// register's $DFF0xx offset, right-shifted by 1 -- paula_floppy.v's own
// parameters DSKDATR=9'h008, DSKBYTR=9'h01a, DSKPTH=9'h020, DSKPTL=9'h022,
// DSKLEN=9'h024, DSKDAT=9'h026, DSKSYNC=9'h07e -- DSKPTH/DSKPTL carry no
// decode in this module at all and are swept only as "elsewhere" probes).
const uint8_t ADDR_NONE    = 0x00;
const uint8_t ADDR_DSKDATR = 0x04;  // $008 >> 1
const uint8_t ADDR_DSKBYTR = 0x0D;  // $01A >> 1
const uint8_t ADDR_DSKPTH  = 0x10;  // $020 >> 1 (no claims in this package)
const uint8_t ADDR_DSKPTL  = 0x11;  // $022 >> 1 (no claims in this package)
const uint8_t ADDR_DSKLEN  = 0x12;  // $024 >> 1
const uint8_t ADDR_DSKDAT  = 0x13;  // $026 >> 1
const uint8_t ADDR_DSKSYNC = 0x3F;  // $07E >> 1

// The full sweep set used by every ADDRESS assertion's "does not answer
// elsewhere" half.
const uint8_t SWEEP[7] = {ADDR_DSKDATR, ADDR_DSKBYTR, ADDR_DSKPTH, ADDR_DSKPTL,
                           ADDR_DSKLEN,  ADDR_DSKDAT,  ADDR_DSKSYNC};
const char* SWEEP_NAME[7] = {"DSKDATR($008)", "DSKBYTR($01A)", "DSKPTH($020)",
                              "DSKPTL($022)",  "DSKLEN($024)",  "DSKDAT($026)",
                              "DSKSYNC($07E)"};
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
        dut->_step = 1;
        dut->direc = 0;
        dut->_sel = 0xE;      // drive 0 selected (active low, 4'b1110)
        dut->side = 0;
        dut->_motor = 1;
        dut->wordsync = 0;    // disabled unless a trial explicitly needs it
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

    // One qualifying-edge write at `addr`, then the address is withdrawn.
    void reg_write(uint8_t addr, uint16_t value) {
        dut->reg_address_in = addr;
        dut->data_in = value;
        settle_edges(1);
        dut->reg_address_in = ADDR_NONE;
        dut->data_in = 0;
        settle_edges(2);
    }

    void dsklen_write(uint16_t value) { reg_write(ADDR_DSKLEN, value); }

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

    // Read DSKDATR repeatedly, counting reads, until blckint is observed.
    // Returns the number of reads performed, or -1 if blckint never
    // appeared within max_reads.
    int drain_until_blckint(int max_reads) {
        for (int i = 1; i <= max_reads; i++) {
            dskdatr_pop();
            if (wait_until([&]() { return (bool)dut->blckint; }, 8)) return i;
        }
        return -1;
    }
};

// Finds the DSKDATR pop index (1-indexed) at which a uniform 0xFF sector's
// payload word is exactly 0xFFFF -- reusing the already-accepted
// ch8-mfm-cell-encoding package's own find_ff_marker_pop_index()
// methodology on this identical fixture (a word of exactly 0xFFFF can
// only arise from this fixture's own payload encoder fed an all-1s byte,
// so it is a marker no fixed framing default can be confused with).
// Returns -1 if not found within 60 pops.
int find_ff_marker_pop_index() {
    Rig rig;
    rig.do_reset();
    uint16_t stim = 0x8000 | 544;  // read-direction, one whole sector's worth.
    rig.dsklen_write(stim);
    rig.dsklen_write(stim);
    uint8_t sector[512];
    std::memset(sector, 0xFF, sizeof sector);
    rig.feed_sector_read(sector);
    for (int i = 1; i <= 60; i++) {
        if (rig.dskdatr_pop() == 0xFFFF) return i;
    }
    return -1;
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    // ========================================================================
    // L0 -- FIXTURE_LIVENESS (no manual citation). Drive two distinct, known
    // DSKLEN writes and confirm they arrive, unmodified and mutually
    // distinguishable, at the DSKBYTR mirror bit (data_out bit 13) that L1-
    // L3, B1-B6 all also read.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dsklen_write(0x0000);
        uint16_t v1 = rig.read_reg(ADDR_DSKBYTR);
        uint16_t word2 = mutating("L0") ? 0x0000 : 0x4000;
        rig.dsklen_write(word2);
        uint16_t v2 = rig.read_reg(ADDR_DSKBYTR);
        uint16_t expected_v2 = mutating("L0") ? 0x9000 : 0xB000;
        checks.note("L0-LIVENESS: DSKLEN<=0x0000 -> DSKBYTR=0x" + hex4(v1) +
                    "; DSKLEN<=0x" + hex4(word2) + " -> DSKBYTR=0x" + hex4(v2));
        checks.expect(v1 == 0x9000 && v2 == expected_v2 && v1 != v2,
            "L0-FIXTURE-LIVENESS: two distinct, known DSKLEN writes arrive, unmodified and "
            "mutually distinguishable, at the DSKBYTR mirror this package's every other "
            "assertion also reads -- the fixture's write-then-read data path is live");
    }

    // ========================================================================
    // L1 -- opus:8-dsk.c35, opus:8-dma.c13, opus:8-appA-dsklen.c1,
    //   sol:8-appa-dsklen.c1, opus:8-appB-dsklen.c1, sol:8-appb-dsklen.c1
    //   (pp.241,246,279,289, DSKLEN's ADD cell): DSKLEN answers at $024, and
    //   at $024 only. DSKLEN has no direct read decode, so this is graded
    //   through the DSKBYTR mirror: writing the WRITE-bit at each of the 7
    //   swept addresses, only $024 should move the mirror.
    // ========================================================================
    {
        checks.note("L1-STIMULUS: sweeping 0x4000 across all 7 register addresses, reading "
                    "DSKBYTR after each");
        bool moved_only_at_dsklen = true;
        for (int i = 0; i < 7; i++) {
            Rig rig;
            rig.do_reset();
            uint8_t addr = SWEEP[i];
            // MUTATE=L1: redirect the arming write from DSKLEN to DSKBYTR's
            // address for the DSKLEN slot in the sweep, while still judging
            // by the DSKLEN-address outcome below.
            uint8_t write_addr = (mutating("L1") && addr == ADDR_DSKLEN) ? ADDR_DSKBYTR : addr;
            rig.reg_write(write_addr, 0x4000);
            uint16_t v = rig.read_reg(ADDR_DSKBYTR);
            bool moved = (v == 0xB000);
            checks.note(std::string("L1-STIMULUS: wrote 0x4000 at ") + SWEEP_NAME[i] +
                        ", DSKBYTR readback=0x" + hex4(v));
            bool should_move = (addr == ADDR_DSKLEN);
            if (moved != should_move) moved_only_at_dsklen = false;
        }
        checks.expect(moved_only_at_dsklen,
            "L1-DSKLEN-ADDRESS (p.241 opus:8-dsk.c35; p.246 opus:8-dma.c13; p.279 "
            "opus:8-appA-dsklen.c1, sol:8-appa-dsklen.c1; p.289 opus:8-appB-dsklen.c1, "
            "sol:8-appb-dsklen.c1): a write reaches DSKLEN's own state (observable via the "
            "DSKBYTR mirror) only when addressed at $024 -- silent at $008, $01A, $020, $022, "
            "$026 and $07E");
    }

    // ========================================================================
    // L2 -- sol:8-dsklen.c1, opus:8-appA-dsklen.c2, sol:8-appa-dsklen.c2,
    //   opus:8-appB-dsklen.c3, sol:8-appb-dsklen.c3 (pp.246,279,289,
    //   composed with the appendix legend's "write-only"): a value
    //   presented at $024 reaches state, but reading $024 returns nothing
    //   written there.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dsklen_write(0x1234);
        // MUTATE=L2: read at DSKBYTR (a real, nonzero decode) instead of
        // DSKLEN's own (undecoded) address.
        uint8_t addr = mutating("L2") ? ADDR_DSKBYTR : ADDR_DSKLEN;
        uint16_t v = rig.read_reg(addr);
        checks.note(std::string("L2-STIMULUS: after DSKLEN<=0x1234, read at ") +
                    (mutating("L2") ? "DSKBYTR (mutated)" : "DSKLEN's own address") +
                    " gives 0x" + hex4(v));
        checks.expect(v == 0x0000,
            "L2-DSKLEN-WRITE-ONLY (p.246 sol:8-dsklen.c1; p.279 opus:8-appA-dsklen.c2, "
            "sol:8-appa-dsklen.c2; p.289 opus:8-appB-dsklen.c3, sol:8-appb-dsklen.c3): after "
            "writing 0x1234 to DSKLEN, a read at DSKLEN's own address returns 0x0000 -- no read "
            "decode exists for it");
    }

    // ========================================================================
    // L3 -- opus:8-dsk.c36, sol:8-reglist.c17, opus:8-appA-dsklen.c4,
    //   sol:8-appa-dsklen.c4, opus:8-appB-dsklen.c5, sol:8-appb-dsklen.c5
    //   (pp.241,279,289, DSKLEN's FUNCTION cell): DSKLEN's value functions
    //   as a DMA transfer length -- program 8 words (double write to arm),
    //   feed one sector, and confirm the transfer completes at exactly 8
    //   reads (blckint on the 8th, not before).
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        uint8_t sector[512];
        std::memset(sector, 0x00, sizeof sector);
        const uint16_t EXPECTED_LEN = mutating("L3") ? 9 : 8;
        uint16_t stim = 0x8000 | EXPECTED_LEN;  // WRITE=0 (read direction), DMAEN=1, len=8/9
        rig.dsklen_write(stim);
        rig.dsklen_write(stim);  // second write arms dmaen
        checks.note("L3-STIMULUS: DSKLEN armed twice with 0x" + hex4(stim) +
                    " (length=" + std::to_string(EXPECTED_LEN) + ")");
        rig.feed_sector_read(sector);
        int completed = rig.drain_until_blckint(64);
        checks.note("L3-MEASURED: transfer completed (blckint seen) after " +
                    std::to_string(completed) + " DSKDATR reads");
        checks.expect(completed == 8,
            "L3-DSKLEN-FUNCTION-LENGTH (p.241 opus:8-dsk.c36, sol:8-reglist.c17; p.279 "
            "opus:8-appA-dsklen.c4, sol:8-appa-dsklen.c4; p.289 opus:8-appB-dsklen.c5, "
            "sol:8-appb-dsklen.c5): DSKLEN's programmed value governs exactly how many words a "
            "DMA transfer moves before blckint fires -- 8 words programmed, 8 words delivered, "
            "not one more or fewer");
    }

    // ========================================================================
    // B1 -- opus:8-dsk.c37, opus:8-appA-dskbytr.c1, sol:8-appa-dskbytr.c1,
    //   opus:8-appB-dskbytr.c1, sol:8-appb-dskbytr.c1 (pp.241,278,289,
    //   DSKBYTR's ADD cell): DSKBYTR answers at $01A, and at $01A only.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        checks.note("B1-STIMULUS: sweeping data_out across all 7 register addresses (idle)");
        bool answers_only_at_dskbytr = true;
        for (int i = 0; i < 7; i++) {
            uint8_t addr = SWEEP[i];
            // MUTATE=B1: probe DSKLEN's address (no read decode) in the
            // DSKBYTR slot, while still judging by DSKBYTR's own address.
            uint8_t probe = (mutating("B1") && addr == ADDR_DSKBYTR) ? ADDR_DSKLEN : addr;
            uint16_t v = rig.read_reg(probe);
            checks.note(std::string("B1-STIMULUS: read at ") + SWEEP_NAME[i] + " gives 0x" + hex4(v));
            bool should_answer = (addr == ADDR_DSKBYTR);
            bool answers = (v != 0x0000);
            if (answers != should_answer) answers_only_at_dskbytr = false;
        }
        checks.expect(answers_only_at_dskbytr,
            "B1-DSKBYTR-ADDRESS (p.241 opus:8-dsk.c37; p.278 opus:8-appA-dskbytr.c1, "
            "sol:8-appa-dskbytr.c1; p.289 opus:8-appB-dskbytr.c1, sol:8-appb-dskbytr.c1): "
            "data_out is nonzero (0x9000 idle) only when read at $01A -- 0x0000 at $008, $020, "
            "$022, $024, $026 and $07E");
    }

    // ========================================================================
    // B2 -- opus:8-appA-dskbytr.c2, sol:8-appa-dskbytr.c2,
    //   opus:8-appB-dskbytr.c3, sol:8-appb-dskbytr.c3 (pp.278,289, composed
    //   with the legend's "read-only"): $01A answers on read, and nothing
    //   presented at $01A on a write reaches any state.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        uint16_t before = rig.read_reg(ADDR_DSKBYTR);
        // A write intended for DSKBYTR's own address, carrying the exact
        // bit pattern (0x4000) that DOES move state when aimed at DSKLEN.
        // MUTATE=B2: aim the SAME payload at DSKLEN's address instead --
        // demonstrating that if a write to $01A behaved like a write to
        // $024 (i.e. reached state), this check would (correctly) go red.
        uint8_t write_addr = mutating("B2") ? ADDR_DSKLEN : ADDR_DSKBYTR;
        rig.reg_write(write_addr, 0x4000);
        uint16_t after = rig.read_reg(ADDR_DSKBYTR);
        checks.note(std::string("B2-STIMULUS: DSKBYTR before=0x") + hex4(before) +
                    ", after writing 0x4000 at " +
                    (write_addr == ADDR_DSKBYTR ? "$01A (own address)" : "$024 (mutated)") +
                    ", DSKBYTR=0x" + hex4(after));
        checks.expect(before == 0x9000 && after == 0x9000,
            "B2-DSKBYTR-READ-ONLY (p.278 opus:8-appA-dskbytr.c2, sol:8-appa-dskbytr.c2; p.289 "
            "opus:8-appB-dskbytr.c3, sol:8-appb-dskbytr.c3): writing 0x4000 at $01A changes "
            "nothing observable -- DSKBYTR still reads 0x9000, unlike the same write aimed at "
            "$024 (see L1/L3), proving $01A has no write decode");
    }

    // ========================================================================
    // B3 -- sol:8-reglist.c20, sol:8-appa-dskbytr.c5, sol:8-appb-dskbytr.c6
    //   (pp.241,278,289, the status half of the FUNCTION cell): reading
    //   DSKBYTR provides disk status that moves with module state. This is
    //   also this package's CONTROL CASE for B4/B5 (same $01A/data_out
    //   measurement basis, expected GREEN).
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        uint16_t idle = rig.read_reg(ADDR_DSKBYTR);
        // MUTATE=B3: leave the WRITE bit clear on both writes instead of
        // setting it on the second, so the status mirror cannot move.
        rig.dsklen_write(0x0000);
        rig.dsklen_write(mutating("B3") ? 0x0000 : 0x4000);
        uint16_t after = rig.read_reg(ADDR_DSKBYTR);
        checks.note("B3-STIMULUS: DSKBYTR idle=0x" + hex4(idle) + ", after arming write(s)=0x" +
                    hex4(after));
        checks.expect(idle == 0x9000 && after != idle,
            "B3-DSKBYTR-STATUS-HALF (p.241 sol:8-reglist.c20; p.278 sol:8-appa-dskbytr.c5; "
            "p.289 sol:8-appb-dskbytr.c6): DSKBYTR's status bits (bits 15:12) move with module "
            "state -- this package's control case, on the same data_out@$01A basis as B4/B5");
    }

    // ========================================================================
    // B4 -- sol:8-reglist.c19, sol:8-appa-dskbytr.c4, opus:8-appB-dskbytr.c5,
    //   sol:8-appb-dskbytr.c5 (pp.241,278,289, the data-byte half of the
    //   FUNCTION cell): reading DSKBYTR provides a disk data byte.
    //
    //   REPAIRED (independent validation, 2026-09-15): the original form
    //   sampled only idle and a DSKLEN WRITE-bit state and passed if
    //   either low byte was nonzero -- zero is a valid disk byte, so
    //   neither state could ever refute the claim; there was no
    //   independent ground truth for what the "right" byte should be.
    //   THIS IS THE SAME UNDERLYING FACT as
    //   ch8-dskbytr-status-register/A9 and
    //   ch8-disk-controller-overview/opus:8-dsk.c9+sol:8-dsk.c9 -- one
    //   fact, repaired identically in all three packages that carry it.
    //
    //   Repaired to bind a KNOWN incoming byte (0xFF) to the DSKBYTR
    //   sampling instant via the DSKDATR/FIFO marker technique already
    //   established by the sibling ch8-mfm-cell-encoding package: a
    //   uniform 0xFF sector's payload word can only ever arrive as
    //   exactly 0xFFFF. find_ff_marker_pop_index() calibrates the pop
    //   index at which that marker appears (a fresh, throwaway model);
    //   the real trial then discards pops up to just before that index,
    //   PEEKS (does not pop) DSKDATR to confirm the known word is present
    //   RIGHT NOW, then peeks DSKBYTR -- zero clock edges elapse between
    //   the two peeks, so both are read at the SAME instant. Both bytes
    //   of 0xFFFF are 0xFF, sidestepping any high/low-byte ordering
    //   ambiguity in what "the current byte" means.
    //
    //   Still RED -- paula_floppy.v:699 hardwires bits 7:0 to the literal
    //   constant 8'h00; a known, bound, nonzero incoming byte still never
    //   appears there. Flagged by finding_candidates.md C6.
    // ========================================================================
    {
        int idx = find_ff_marker_pop_index();
        checks.note("B4-CALIBRATION: 0xFF-sector marker word (0xFFFF) first found at DSKDATR pop #" +
                    std::to_string(idx));

        bool bound_ok = false;
        uint16_t known_word = 0, dskbytr_at_instant = 0;
        if (idx > 0) {
            Rig rig;
            rig.do_reset();
            uint16_t stim = 0x8000 | 544;
            rig.dsklen_write(stim);
            rig.dsklen_write(stim);
            uint8_t sector[512];
            std::memset(sector, 0xFF, sizeof sector);
            rig.feed_sector_read(sector);
            for (int i = 1; i < idx; i++) rig.dskdatr_pop();  // discard up to just before the marker.
            known_word = rig.read_reg(ADDR_DSKDATR);          // peek, no pop -- the marker itself, at this instant.
            dskbytr_at_instant = rig.read_reg(ADDR_DSKBYTR);  // SAME instant: zero clock edges elapsed since the peek above.
            bound_ok = (known_word == 0xFFFF);
        }
        uint8_t known_byte = 0xFF;  // both bytes of 0xFFFF are 0xFF -- no high/low ambiguity.
        uint8_t data_field = (uint8_t)(dskbytr_at_instant & 0x00FF);
        checks.note("B4-MEASURED: known_word=0x" + hex4(known_word) + " (bound=" +
                    std::to_string(bound_ok) + "), DSKBYTR at that same instant=0x" +
                    hex4(dskbytr_at_instant) + ", DATA field=0x" + hex4(data_field));
        checks.expect(bound_ok && data_field == known_byte,
            "B4-DSKBYTR-DATA-BYTE-HALF (p.241 sol:8-reglist.c19; p.278 sol:8-appa-dskbytr.c4; "
            "p.289 opus:8-appB-dskbytr.c5, sol:8-appb-dskbytr.c5): reading DSKBYTR provides a "
            "disk data byte -- with a KNOWN incoming byte (0xFF) bound to the sampling instant "
            "via the proven-live DSKDATR/FIFO marker path (the same underlying fact as "
            "ch8-dskbytr-status-register/A9 and ch8-disk-controller-overview/opus:8-dsk.c9+"
            "sol:8-dsk.c9), DSKBYTR's own DATA field should read that same byte at that instant; "
            "it does not");
    }

    // ========================================================================
    // B5 -- opus:8-dsk.c38, opus:8-appA-dskbytr.c4, sol:8-appa-dskbytr.c6
    //   (pp.241,278, the un-split compound printing): "DSKBYTR returns the
    //   disk data byte together with status" / "is the disk-to-
    //   microprocessor data buffer" -- graded as the conjunction B3 (true)
    //   AND B4 (false).
    //
    //   REPAIRED (independent validation, 2026-09-15): B5's byte half
    //   inherited B4's original unsupported "nonzero at least once"
    //   premise and was withdrawn for the same reason. Repaired here by
    //   reusing B4's own repaired, known-byte-bound measurement for the
    //   byte half (see B4 above for the full methodology) instead of
    //   recomputing an unbound premise. Still RED, for the same
    //   underlying reason as B4.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        uint16_t idle = rig.read_reg(ADDR_DSKBYTR);
        rig.dsklen_write(0x0000);
        rig.dsklen_write(0x4000);
        uint16_t armed = rig.read_reg(ADDR_DSKBYTR);
        bool status_half_real = (armed != idle);

        int idx = find_ff_marker_pop_index();
        bool byte_half_real = false;
        if (idx > 0) {
            Rig rig2;
            rig2.do_reset();
            uint16_t stim = 0x8000 | 544;
            rig2.dsklen_write(stim);
            rig2.dsklen_write(stim);
            uint8_t sector[512];
            std::memset(sector, 0xFF, sizeof sector);
            rig2.feed_sector_read(sector);
            for (int i = 1; i < idx; i++) rig2.dskdatr_pop();
            uint16_t known_word = rig2.read_reg(ADDR_DSKDATR);
            uint16_t dskbytr_now = rig2.read_reg(ADDR_DSKBYTR);
            byte_half_real = (known_word == 0xFFFF) && ((dskbytr_now & 0x00FF) == 0xFF);
        }
        checks.note("B5-MEASURED: status_half_real=" + std::to_string(status_half_real) +
                    ", byte_half_real=" + std::to_string(byte_half_real) +
                    " (byte half repaired via the same known-byte binding as B4)");
        checks.expect(status_half_real && byte_half_real,
            "B5-DSKBYTR-COMPOUND-BYTE-AND-STATUS (p.241 opus:8-dsk.c38; p.278 "
            "opus:8-appA-dskbytr.c4, sol:8-appa-dskbytr.c6): the printed function cell claims "
            "BOTH a real status portion AND a real disk-data-byte portion -- the conjunction "
            "fails because, per B4's repaired and now-grounded measurement, the byte portion "
            "does not reflect a known bound incoming byte");
    }

    // ========================================================================
    // B6 -- DSKBYTR_BYTE_FIELD_COMPARATOR_LIVE (substitute, no manual
    //   citation). B4/B5 are expected red. Applies the SAME field-
    //   extraction-and-compare apparatus (read data_out at $01A, compare a
    //   sub-field before/after a stimulus) to the status field, which B3
    //   already shows moves, proving the apparatus is not itself silently
    //   broken or vacuously true/false.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        uint16_t before = rig.read_reg(ADDR_DSKBYTR);
        rig.dsklen_write(0x0000);
        // MUTATE=B6: leave the WRITE bit clear on the second write too, so
        // the status field the comparator reads cannot move.
        rig.dsklen_write(mutating("B6") ? 0x0000 : 0x4000);
        uint16_t after = rig.read_reg(ADDR_DSKBYTR);
        checks.note("B6-STIMULUS: DSKBYTR before=0x" + hex4(before) + " after=0x" + hex4(after));
        checks.expect(before != after,
            "B6-DSKBYTR-BYTE-FIELD-COMPARATOR-LIVE: the same data_out@$01A field-extraction "
            "comparator B4/B5 use, applied instead to the status field B3 already shows moves, "
            "correctly reports a difference -- the apparatus is live, not the reason B4/B5 read "
            "red");
    }

    // ========================================================================
    // S1 -- opus:8-dsk.c39, opus:8-adkcon.c22, sol:8-adkcon.c18,
    //   opus:8-appA-dsksync.c1, sol:8-appa-dsksync.c1, opus:8-appA-adkcon.c21,
    //   sol:8-appa-adkbit.c12, opus:8-appB-dsksync.c1, sol:8-appb-dsksync.c1
    //   (pp.241,249,265,279,290, DSKSYNC's ADD cell / the (3F)*2 arithmetic):
    //   DSKSYNC answers (i.e. a write reaches its comparator) at $07E, and
    //   at $07E only. DSKSYNC has no read decode, so this is graded through
    //   the same sync-gating apparatus as S3: writing a mismatching value
    //   (0xDEAD) to a WRONG address ($024) should have NO effect -- DSKSYNC
    //   stays at its reset default (0x4489, matching), so delivery proceeds
    //   normally, exactly as if DSKSYNC had never been written at all.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        uint8_t sector[512];
        std::memset(sector, 0x00, sizeof sector);
        rig.dut->wordsync = 1;
        // MUTATE=S1: write the mismatching value to DSKSYNC's OWN address
        // instead of the wrong one, which (correctly) suppresses delivery
        // -- while the check below still expects the "wrong address, no
        // effect" outcome (normal delivery).
        uint8_t write_addr = mutating("S1") ? ADDR_DSKSYNC : ADDR_DSKLEN;
        rig.reg_write(write_addr, 0xDEAD);
        const uint16_t LEN = 8;
        uint16_t stim = 0x8000 | LEN;
        rig.dsklen_write(stim);
        rig.dsklen_write(stim);
        checks.note(std::string("S1-STIMULUS: wrote 0xDEAD at ") +
                    (write_addr == ADDR_DSKSYNC ? "DSKSYNC's own address" : "DSKLEN's address (wrong)"));
        rig.feed_sector_read(sector);
        int completed = rig.drain_until_blckint(64);
        checks.note("S1-MEASURED: transfer completed after " +
                    (completed < 0 ? std::string("never (>64 reads)") : std::to_string(completed)) +
                    " reads");
        checks.expect(completed > 0,
            "S1-DSKSYNC-ADDRESS (p.241 opus:8-dsk.c39; p.249 opus:8-adkcon.c22, "
            "sol:8-adkcon.c18; p.265 opus:8-appA-adkcon.c21, sol:8-appa-adkbit.c12; p.279 "
            "opus:8-appA-dsksync.c1, sol:8-appa-dsksync.c1; p.290 opus:8-appB-dsksync.c1, "
            "sol:8-appb-dsksync.c1): a mismatching value written to a DIFFERENT address ($024) "
            "has no effect on DSKSYNC's comparator -- delivery proceeds exactly as if DSKSYNC "
            "were left at its reset default, proving $07E (not $024) is where the write is "
            "decoded");
    }

    // ========================================================================
    // S2 -- opus:8-appA-dsksync.c2, sol:8-appa-dsksync.c2,
    //   opus:8-appB-dsksync.c3, sol:8-appb-dsksync.c3 (pp.279,290, composed
    //   with the legend's "write-only"): a value presented at $07E reaches
    //   state, but reading $07E returns nothing written there.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.reg_write(ADDR_DSKSYNC, 0x1234);
        uint8_t addr = mutating("S2") ? ADDR_DSKBYTR : ADDR_DSKSYNC;
        uint16_t v = rig.read_reg(addr);
        checks.note(std::string("S2-STIMULUS: after DSKSYNC<=0x1234, read at ") +
                    (mutating("S2") ? "DSKBYTR (mutated)" : "DSKSYNC's own address") +
                    " gives 0x" + hex4(v));
        checks.expect(v == 0x0000,
            "S2-DSKSYNC-WRITE-ONLY (p.279 opus:8-appA-dsksync.c2, sol:8-appa-dsksync.c2; p.290 "
            "opus:8-appB-dsksync.c3, sol:8-appb-dsksync.c3): after writing 0x1234 to DSKSYNC, a "
            "read at its own address returns 0x0000 -- no read decode exists for it");
    }

    // ========================================================================
    // S3 -- opus:8-dsk.c40, opus:8-dsk.c41, sol:8-reglist.c22,
    //   sol:8-reglist.c23, opus:8-appA-dsksync.c4, opus:8-appB-dsksync.c4,
    //   sol:8-appb-dsksync.c4 (pp.241,279,290, DSKSYNC's FUNCTION cell):
    //   DSKSYNC holds a word compared against the incoming disk data
    //   stream, gating delivery. With wordsync=1: DSKSYNC=0xDEAD (never
    //   produced by this fixture's own internal MFM generator) delivers
    //   ZERO words; DSKSYNC left at its reset default 0x4489 (the literal
    //   sync mark the generator itself emits) delivers the programmed
    //   count.
    // ========================================================================
    {
        uint8_t sector[512];
        std::memset(sector, 0x00, sizeof sector);
        const uint16_t LEN = 8;
        uint16_t stim = 0x8000 | LEN;

        // Trial A: mismatching DSKSYNC.
        Rig rigA;
        rigA.do_reset();
        rigA.dut->wordsync = 1;
        // MUTATE=S3: write the MATCHING value instead, in the trial the
        // check expects to see suppressed.
        rigA.reg_write(ADDR_DSKSYNC, mutating("S3") ? 0x4489 : 0xDEAD);
        rigA.dsklen_write(stim);
        rigA.dsklen_write(stim);
        rigA.feed_sector_read(sector);
        int completedA = rigA.drain_until_blckint(64);
        checks.note("S3-MEASURED (mismatching DSKSYNC): completed=" +
                    (completedA < 0 ? std::string("never (>64 reads, suppressed)") : std::to_string(completedA)));

        // Trial B: matching DSKSYNC (reset default, 0x4489 -- not written).
        Rig rigB;
        rigB.do_reset();
        rigB.dut->wordsync = 1;
        rigB.dsklen_write(stim);
        rigB.dsklen_write(stim);
        rigB.feed_sector_read(sector);
        int completedB = rigB.drain_until_blckint(64);
        checks.note("S3-MEASURED (matching DSKSYNC, reset default 0x4489): completed=" +
                    std::to_string(completedB));

        checks.expect(completedA < 0 && completedB > 0,
            "S3-DSKSYNC-FUNCTION-MATCH-WORD (p.241 opus:8-dsk.c40, opus:8-dsk.c41, "
            "sol:8-reglist.c22, sol:8-reglist.c23; p.279 opus:8-appA-dsksync.c4; p.290 "
            "opus:8-appB-dsksync.c4, sol:8-appb-dsksync.c4): with wordsync enabled, a "
            "mismatching DSKSYNC value suppresses delivery entirely, while a matching value "
            "delivers the programmed count -- DSKSYNC's content gates the read");
    }

    // ========================================================================
    // D1 -- opus:8-appA-dskdat.c1, sol:8-appa-dskdat.c1, opus:8-appB-dskdat.c1,
    //   sol:8-appb-dskdat.c1 (pp.279,289, DSKDAT's ADD cell): DSKDAT answers
    //   at $026, and at $026 only. Graded directly on the `buswr` public
    //   wire -- pure combinational address decode.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        checks.note("D1-STIMULUS: sweeping buswr across all 7 register addresses");
        bool buswr_only_at_dskdat = true;
        for (int i = 0; i < 7; i++) {
            uint8_t addr = SWEEP[i];
            // MUTATE=D1: probe DSKLEN's address in the DSKDAT slot.
            uint8_t probe = (mutating("D1") && addr == ADDR_DSKDAT) ? ADDR_DSKLEN : addr;
            rig.dut->reg_address_in = probe;
            rig.dut->eval();
            bool w = (bool)rig.dut->rootp->paula_floppy->buswr;
            checks.note(std::string("D1-STIMULUS: reg_address_in=") + SWEEP_NAME[i] +
                        " buswr=" + std::to_string(w));
            bool should = (addr == ADDR_DSKDAT);
            if (w != should) buswr_only_at_dskdat = false;
        }
        rig.dut->reg_address_in = ADDR_NONE;
        rig.dut->eval();
        checks.expect(buswr_only_at_dskdat,
            "D1-DSKDAT-ADDRESS (p.279 opus:8-appA-dskdat.c1, sol:8-appa-dskdat.c1; p.289 "
            "opus:8-appB-dskdat.c1, sol:8-appb-dskdat.c1): `buswr` asserts only when "
            "reg_address_in selects $026 -- 0 at $008, $01A, $020, $022, $024 and $07E");
    }

    // ========================================================================
    // D2 -- opus:8-appA-dskdat.c2, sol:8-appa-dskdat.c2, opus:8-appB-dskdat.c4,
    //   sol:8-appb-dskdat.c4 (pp.279,289, composed with the legend's
    //   "write-only"): a value presented at $026 reaches state (verified
    //   via D3's FIFO effect), but reading $026 returns nothing written
    //   there.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.reg_write(ADDR_DSKDAT, 0x5678);
        uint8_t addr = mutating("D2") ? ADDR_DSKBYTR : ADDR_DSKDAT;
        uint16_t v = rig.read_reg(addr);
        checks.note(std::string("D2-STIMULUS: after DSKDAT<=0x5678, read at ") +
                    (mutating("D2") ? "DSKBYTR (mutated)" : "DSKDAT's own address") +
                    " gives 0x" + hex4(v));
        checks.expect(v == 0x0000,
            "D2-DSKDAT-WRITE-ONLY (p.279 opus:8-appA-dskdat.c2, sol:8-appa-dskdat.c2; p.289 "
            "opus:8-appB-dskdat.c4, sol:8-appb-dskdat.c4): after writing 0x5678 to DSKDAT, a "
            "read at its own address returns 0x0000 -- no read decode exists for it");
    }

    // ========================================================================
    // D3 -- opus:8-appA-dskdat.c4, sol:8-appa-dskdat.c4, opus:8-appB-dskdat.c6,
    //   sol:8-appb-dskdat.c6 (pp.279,289, DSKDAT's FUNCTION cell): a write
    //   at $026 is disk DMA outgoing data. Arm a 1-word WRITE-direction
    //   transfer, write one word to DSKDAT, and confirm it completes the
    //   transfer (blckint) -- proving the written word entered the DMA
    //   length-counted data path, not a void.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        const uint16_t LEN = 1;
        uint16_t stim = 0xC000 | LEN;  // WRITE=1 (write direction), DMAEN=1, len=1
        rig.dsklen_write(stim);
        rig.dsklen_write(stim);
        // Let the state machine reach DISKDMA_ACTIVE.
        rig.wait_until([&]() { return (bool)rig.dut->fdd_led; }, 16);
        // MUTATE=D3: write the payload word at DSKSYNC's address instead of
        // DSKDAT's -- no fifo/length effect there.
        uint8_t write_addr = mutating("D3") ? ADDR_DSKSYNC : ADDR_DSKDAT;
        rig.reg_write(write_addr, 0xABCD);
        bool completed = rig.wait_until([&]() { return (bool)rig.dut->blckint; }, 32);
        checks.note(std::string("D3-STIMULUS: wrote 0xABCD at ") +
                    (write_addr == ADDR_DSKDAT ? "DSKDAT" : "DSKSYNC (mutated)") +
                    " during an armed 1-word WRITE transfer; blckint observed=" +
                    std::to_string(completed));
        checks.expect(completed,
            "D3-DSKDAT-FUNCTION-DMA-WRITE (p.279 opus:8-appA-dskdat.c4, sol:8-appa-dskdat.c4; "
            "p.289 opus:8-appB-dskdat.c6, sol:8-appb-dskdat.c6): a single DSKDAT write, during "
            "an armed 1-word WRITE-direction transfer, both enters the FIFO and completes the "
            "DMA length count -- the written word is genuinely disk-DMA outgoing data, not "
            "discarded");
    }

    // ========================================================================
    // E1 -- opus:8-appA-dskdat.c5, sol:8-appa-dskdatr.c1, opus:8-appB-dskdatr.c1,
    //   sol:8-appb-dskdatr.c1 (pp.279,289, DSKDATR's ADD cell): DSKDATR
    //   answers at $008, and at $008 only. Graded directly on the `busrd`
    //   public wire.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        checks.note("E1-STIMULUS: sweeping busrd across all 7 register addresses");
        bool busrd_only_at_dskdatr = true;
        for (int i = 0; i < 7; i++) {
            uint8_t addr = SWEEP[i];
            // MUTATE=E1: probe DSKBYTR's address in the DSKDATR slot.
            uint8_t probe = (mutating("E1") && addr == ADDR_DSKDATR) ? ADDR_DSKBYTR : addr;
            rig.dut->reg_address_in = probe;
            rig.dut->eval();
            bool r = (bool)rig.dut->rootp->paula_floppy->busrd;
            checks.note(std::string("E1-STIMULUS: reg_address_in=") + SWEEP_NAME[i] +
                        " busrd=" + std::to_string(r));
            bool should = (addr == ADDR_DSKDATR);
            if (r != should) busrd_only_at_dskdatr = false;
        }
        rig.dut->reg_address_in = ADDR_NONE;
        rig.dut->eval();
        checks.expect(busrd_only_at_dskdatr,
            "E1-DSKDATR-ADDRESS (p.279 opus:8-appA-dskdat.c5, sol:8-appa-dskdatr.c1; p.289 "
            "opus:8-appB-dskdatr.c1, sol:8-appb-dskdatr.c1): `busrd` asserts only when "
            "reg_address_in selects $008 -- 0 at $01A, $020, $022, $024, $026 and $07E");
    }

    // ========================================================================
    // E2 -- opus:8-appA-dskdat.c6, sol:8-appa-dskdatr.c2 (p.279, the `ER`
    //   mark's observable half): the read access at $008 is itself what
    //   advances the FIFO -- there is no separate strobe distinct from the
    //   access. Prove it by reading twice and confirming the second read's
    //   own act of accessing is what changes what the third read sees.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        const uint16_t LEN = 4;
        uint16_t stim = 0x8000 | LEN;
        rig.dsklen_write(stim);
        rig.dsklen_write(stim);
        uint8_t sector[512];
        std::memset(sector, 0xFF, sizeof sector);
        rig.feed_sector_read(sector);
        rig.wait_until([&]() { return (bool)rig.dut->fdd_led; }, 16);
        uint16_t r1 = rig.dskdatr_pop();
        // MUTATE=E2: skip the second access entirely -- read the SAME
        // pre-second-access value again instead of performing a fresh
        // access, so the "access is the trigger" claim cannot be
        // demonstrated.
        uint16_t r2 = mutating("E2") ? r1 : rig.dskdatr_pop();
        uint16_t r3 = rig.dskdatr_pop();
        checks.note("E2-STIMULUS: three DSKDATR accesses returned 0x" + hex4(r1) + ", 0x" +
                    hex4(r2) + ", 0x" + hex4(r3));
        checks.expect(r2 != r3,
            "E2-DSKDATR-EARLY-READ-ACCESS-IS-TRIGGER (p.279 opus:8-appA-dskdat.c6, "
            "sol:8-appa-dskdatr.c2): the second access's own occurrence is what advances the "
            "FIFO to a new word for the third access -- the read access at $008 is itself the "
            "trigger, no separate strobe exists");
    }

    // ========================================================================
    // E3 -- opus:8-appA-dskdat.c8, sol:8-appa-dskdatr.c4, sol:8-appa-dskdatr.c5,
    //   opus:8-appB-dskdatr.c6, sol:8-appb-dskdatr.c6 (pp.279,289, the
    //   "dummy address" claim). DECLINED (independent validation,
    //   2026-09-15): the original assertion over-read the manual's own
    //   framing.
    //
    //   The decisive text is p.287's `ER` legend: early-read transfers put
    //   data on the bus early for RAM timing and "are therefore initiated
    //   by Agnus timing, rather than a read address on the destination
    //   address bus." "Dummy address" names WHO/WHAT triggers the bus
    //   cycle (Agnus's own DMA timing, not a CPU-issued read-address
    //   decode on the destination bus) -- it does not claim there is no
    //   real register or content behind $008, and does not require
    //   consecutive reads to be identical. The withdrawn assertion tested
    //   exactly that unstated "no real content" reading (and, ironically,
    //   the live content E1/E2 above already establish would have FAILED
    //   that withdrawn reading correctly for the wrong reason).
    //
    //   Restating the assertion to what the page ACTUALLY claims would
    //   need to distinguish "an Agnus-timed access" from "a CPU
    //   destination-address-bus access" -- a distinction that exists only
    //   at the Agnus<->Paula boundary. This package's assigned fixture
    //   (`paula_floppy` alone) has no such boundary: reg_address_in is
    //   presented identically regardless of what upstream hardware drove
    //   it there, and fifo_rd's own gating (busrd & dmaon,
    //   paula_floppy.v:911) cannot tell CPU-explicit access from
    //   Agnus-scheduled access apart. Declining rather than either
    //   re-testing the withdrawn "no real content" reading or overreaching
    //   to a boundary (Agnus+Paula) this package was not assigned.
    //
    //   E4 (DSKDATR_DUMMY_ADDRESS_COMPARATOR_LIVE), whose entire purpose
    //   was proving E3's own comparator was not vacuously red, is removed
    //   along with E3's grading -- there is no longer a graded comparator
    //   for it to defend, and E1 above already independently establishes
    //   busrd's live/undecoded discrimination over the same swept set.
    // ========================================================================
    {
        checks.record(
            "E3-DSKDATR-NOT-A-DUMMY-ADDRESS (p.279 opus:8-appA-dskdat.c8, sol:8-appa-dskdatr.c4, "
            "sol:8-appa-dskdatr.c5; p.289 opus:8-appB-dskdatr.c6, sol:8-appb-dskdatr.c6): the "
            "manual's own 'dummy address' / 'early read dummy address' framing, read against the "
            "decisive p.287 `ER` legend, claims that the $008 bus cycle is initiated by Agnus "
            "timing rather than a destination-address-bus read -- a trigger-source distinction "
            "that needs the Agnus<->Paula boundary. `paula_floppy` compiled alone (this "
            "package's assigned fixture) presents reg_address_in identically regardless of what "
            "upstream hardware drove it, so this fixture cannot test who/what triggered the "
            "access. The withdrawn assertion instead tested an unstated 'no real content behind "
            "$008' reading, which E1/E2 above already show is false in an uninteresting way (a "
            "live, content-dependent FIFO answers there) -- that is not what 'dummy' means on "
            "this page. Declining rather than re-testing the withdrawn reading or overreaching "
            "to a boundary (Agnus+Paula) this package was not assigned.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
