// Test purpose: HRM conformance package for the register-map propositions
//   (address, access direction, function) of DSKLEN ($024), DSKBYTR ($01A),
//   DSKSYNC ($07E), DSKDAT ($026) and DSKDATR ($008), against `paula_floppy`
//   (paula_floppy.v + paula_floppy_fifo.v only -- paula_floppy_ebr.v
//   excluded, it declares a second `module paula_floppy`). 79 claims, from
//   work/ch8-s2-build/ch8-disk-register-map-paula-floppy.jsonl, reduced to
//   19 propositions. Full derivation in SPECIFICATION.md.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 241, 246, 250, 263, 264, 278, and more
// Non-goals: anything outside the `paula_floppy` fixture this package
//   builds, and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/ch8-s2-build/ch8-disk-register-map-paula-floppy.jsonl` in the
//   sibling repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// WHAT IS GRADED AND WHAT IS DECLINED.
//   - B4 and B5 bind a KNOWN incoming byte (0xFF, via the DSKDATR/FIFO
//     technique ch8-mfm-cell-encoding uses) to the DSKBYTR sampling instant:
//     a "nonzero at least once" premise could never be refuted (zero is a
//     valid disk byte). THE SAME UNDERLYING FACT as
//     ch8-dskbytr-status-register/A9 and
//     ch8-disk-controller-overview/opus:8-dsk.c9+sol:8-dsk.c9, graded
//     identically in all three packages that carry it. RED (the DATA field
//     is the literal constant 8'h00, paula_floppy.v:699).
//   - E3 is DECLINED (record(), not expect()): the manual's actual "dummy
//     address" claim is about WHO triggers the bus cycle (Agnus timing vs.
//     a destination-address-bus read, per the p.287 `ER` legend), a
//     distinction this package's `paula_floppy`-alone fixture cannot make;
//     a "no real content behind $008" reading is one the legend does not
//     make. E1 independently covers busrd's live/undecoded discrimination.
//   - E2 is DECLINED for the SAME reason: it would grade the mechanism
//     p.287's `ER` legend explicitly says is NOT what triggers an
//     early-read transfer.
//   - L2, S2, D2 (the three WRITE-ONLY propositions) are DECLINED: they
//     would grade a read the manual (p.263-264) forbids outright and never
//     gives a value for. D3 is DECLINED: blckint/length-completion would
//     pass identically for a controller that discarded every written word.
//   - L0 (no uid) is a note(): a fixture-liveness check, not conformance
//     evidence.
//   - B1, B2 and B3 read DISKWRITE (bit 13, p.278-defined) or a
//     before/after equality rather than DSKBYTR's hardwired bits (15, 12),
//     since p.278 gives DSKBYTR no reset/idle word. S1 has a genuine
//     positive-write trial and S3's matching trial explicitly writes
//     0x4489, so neither leans on the RTL's reset default (0x4489)
//     coinciding with the manual's own "usual" sync word as its only
//     positive evidence.
//
// B4/B5 SAMPLING. The locator is the fixed, RTL-structural
//   FIRST_PAYLOAD_POP_INDEX (=33) defined below, taken from paula_floppy.v's
//   own word-counter framing thresholds (:484-497), a piece of logic
//   structurally distinct from the per-bit encoder (:467-477) -- a locator
//   that cannot break when the encoder is fixed. Every B4/B5 trial feeds a
//   uniform 0xFF sector, so any payload word is as good a sample as any
//   other. Searching the drained DSKDATR stream for 0xFFFF would not do:
//   0xFFFF is what THIS RTL's own non-conformant, context-free MFM encoder
//   produces for a uniform 0xFF sector, while p.250 makes 0xFF page-encode
//   to 0x5555, so against a conforming encoder the search would find
//   nothing and the assertion would report a DATA-field defect that would
//   not be there. The peek at the fixed instant CONFIRMS the sampled word
//   is the known byte's own encoding (`bound_ok = (known_word == 0xFFFF)`);
//   a confirmation failure is a PRECONDITION that DECLINES via record(),
//   never a conjunct that grades -- the shape ch8-dskbytr-status-register/
//   A9 uses. Against the RTL under test here, bound_ok is true and B4/B5
//   are RED: known_word=0xFFFF, DSKBYTR=0xD000, DATA field=0x0000.
// Oracle, in detail: Amiga Hardware Reference Manual, printed pp.241, 246,
//   249, 265, 278-279, 289-290 -- the per-proposition pages behind the range
//   given at the top of this file.
//
// SOURCES READ:
//   1. hrm/ch8-dsklen-dma-arming/{Makefile,test_ch8-dsklen-dma-arming.cpp,
//      SPECIFICATION.md} -- a sibling package targeting the SAME fixture,
//      for this suite's established harness/mutation boilerplate (the
//      Checks class, the MUTATE env-var convention, the Rig/tick()/
//      settle_edges()/do_reset()/feed_sector_read() shape) and this
//      fixture's own clocking. No claim in THIS package's own jsonl is
//      graded from that file; every expect() below cites only this
//      package's own uids and is checked against paula_floppy.v directly.
//   2. nanomig/src/minimig-aga/paula_floppy.v -- full file (1041 lines).
//   3. nanomig/src/minimig-aga/paula_floppy_fifo.v -- full file (60 lines):
//      a 2048-word circular buffer, read/write pointer equality for
//      empty/full.
//   paula_floppy_ebr.v is not part of this fixture.
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
//     allocated for every trial below (rig hazard).
//
// PARTLY-OVERTURNED CONSTRAINTS: no claim record in this package has a
//   `classify.note` field at all, so no claim is restricted to half of its
//   evidence by the attack stage.
//
// FIXTURE-LIVENESS. L0 (note(), no uid) proves the write-then-read data
//   path is live before anything else is trusted, on the exact
//   DSKBYTR-mirror path B3/L3's own trials also read.
//
// DECLINED CLAIMS: 24 uids sit
//   on `record()` lines, not graded lines --
//     E3 (5): opus:8-appA-dskdat.c8, sol:8-appa-dskdatr.c4,
//       sol:8-appa-dskdatr.c5, opus:8-appB-dskdatr.c6, sol:8-appb-dskdatr.c6
//     L2 (5): sol:8-dsklen.c1, opus:8-appA-dsklen.c2, sol:8-appa-dsklen.c2,
//       opus:8-appB-dsklen.c3, sol:8-appb-dsklen.c3
//     S2 (4): opus:8-appA-dsksync.c2, sol:8-appa-dsksync.c2,
//       opus:8-appB-dsksync.c3, sol:8-appb-dsksync.c3
//     D2 (4): opus:8-appA-dskdat.c2, sol:8-appa-dskdat.c2,
//       opus:8-appB-dskdat.c4, sol:8-appb-dskdat.c4
//     D3 (4): opus:8-appA-dskdat.c4, sol:8-appa-dskdat.c4,
//       opus:8-appB-dskdat.c6, sol:8-appb-dskdat.c6
//     E2 (2): opus:8-appA-dskdat.c6, sol:8-appa-dskdatr.c2
//   -- see each one's own comment below and SPECIFICATION.md's "Declined
//   claims" section. The remaining 55 uids are cited on a graded
//   (`expect()`) line.
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
//   ordinary stimulus mutation. L0 (note()) and the six declined claims (L2,
//   S2, D2, D3, E2, E3; all record()) carry no mutation record -- neither a
//   note() nor a record() is a graded line for a mutation to pair against.
//
// MUTATION L1-DSKLEN-ADDRESS stimulus red: the arming write is redirected from DSKLEN's own address to DSKBYTR's address (a real decode) while the check still expects the DSKLEN-address readback pattern (MUTATE=L1).
// L0-FIXTURE-LIVENESS is a note() (no manual citation, no uid) -- see its own comment below. It carries no MUTATION record: a note has no graded line to pair one against.
// L2-DSKLEN-WRITE-ONLY is DECLINED (record(), not expect()) -- see its own comment below. p.263-264 forbids reading a write-only register outright and gives no value for what such a read returns; there is no graded line to pair a mutation record against.
// MUTATION L3-DSKLEN-FUNCTION-LENGTH stimulus red: DSKLEN is programmed with 9 words instead of 8 while the check still expects completion at exactly 8 reads (MUTATE=L3).
// MUTATION B1-DSKBYTR-ADDRESS stimulus red: the probe address is redirected from DSKBYTR's own address to DSKLEN's address (no read decode) while the check still expects DSKBYTR's DISKWRITE-bit (bit 13) readback (MUTATE=B1). The check reads bit 13 (DISKWRITE, p.278-defined) alone, not the whole idle word (0x9000), which p.278 does not define -- see the code.
// MUTATION B2-DSKBYTR-READ-ONLY stimulus red: the same 0x4000 write is redirected from DSKBYTR's own address to DSKLEN's (a real decode) while the check still expects DSKBYTR's own readback to be unchanged (before==after) (MUTATE=B2). The check is a before/after equality, not a fixed idle value, since p.278 gives DSKBYTR no required reset/idle word.
// MUTATION B3-DSKBYTR-STATUS-HALF stimulus red: the second DSKLEN write's WRITE bit is left equal to the first's (both clear) instead of set, so DISKWRITE (bit 13) cannot move, while the check still expects it to move (MUTATE=B3). The check reads bit 13 (DISKWRITE, p.278-defined) alone, not the whole idle word.
// MUTATION B4-DSKBYTR-DATA-BYTE-HALF none impossible: already red at baseline -- bits 7:0 of dskbytr are the literal RTL constant 8'h00 (paula_floppy.v:699), not driven by any signal this fixture can stimulate, so no stimulus can make this assertion pass, even with a genuinely bound known byte at the sampling instant. The comparator that grades it is proven live via the substitute B6-DSKBYTR-BYTE-FIELD-COMPARATOR-LIVE below.
// MUTATION B5-DSKBYTR-COMPOUND-BYTE-AND-STATUS none impossible: already red at baseline for the same reason as B4 (the byte conjunct can never become true); see B6.
// MUTATION B6-DSKBYTR-BYTE-FIELD-COMPARATOR-LIVE stimulus red: the second DSKLEN write is changed to leave the WRITE bit equal to the first (both clear) instead of set, so the status field the substitute reads cannot move, while the check still expects it to (MUTATE=B6).
// MUTATION S1-DSKSYNC-ADDRESS stimulus red: the wrong-address (negative) half's 0xDEAD write is redirected from DSKLEN's address to DSKSYNC's own address instead, which (correctly) suppresses delivery, while the check still expects the wrong-address trial's normal-delivery outcome (MUTATE=S1). The assertion also has a POSITIVE half -- a fresh trial that writes 0xDEAD AT DSKSYNC's own address and confirms suppression -- so it does not lean on the RTL's reset default (0x4489) coinciding with the generator's own emitted sync word as its only evidence that $07E is real; see the code.
// MUTATION S2-DSKSYNC-WRITE-ONLY is DECLINED (record(), not expect()) -- same fault as L2 above; see its own comment below. It carries no MUTATION record.
// MUTATION S3-DSKSYNC-FUNCTION-MATCH-WORD stimulus red: DSKSYNC is written with 0x4489 (the matching value) instead of 0xDEAD in the trial the check expects to see suppressed (zero words delivered), while the check still expects suppression (MUTATE=S3). The matching trial (Trial B) explicitly WRITES 0x4489 to DSKSYNC rather than relying on the RTL's reset default; the mutation touches only Trial A.
// MUTATION D1-DSKDAT-ADDRESS stimulus red: the swept address that is expected to be the sole `buswr`-asserting one is changed from $026 to $024 while the check still expects $026 alone to assert it (MUTATE=D1).
// D2-DSKDAT-WRITE-ONLY is DECLINED (record(), not expect()) -- same fault as L2 above; see its own comment below. It carries no MUTATION record.
// D3-DSKDAT-FUNCTION-DMA-WRITE is DECLINED (record(), not expect()) -- see its own comment below: blckint and the length counter reaching zero do not show the written word became genuine outgoing disk data. It carries no MUTATION record.
// MUTATION E1-DSKDATR-ADDRESS stimulus red: the swept address expected to be the sole `busrd`-asserting one is changed from $008 to $01A while the check still expects $008 alone to assert it (MUTATE=E1).
// E2-DSKDATR-EARLY-READ-ACCESS-IS-TRIGGER is DECLINED (record(), not expect()) -- same fault as E3 below, over the same p.287 `ER` legend; see its own comment below. It carries no MUTATION record.
// E3 is DECLINED (record(), not expect()) -- see E3's own comment below. It carries no MUTATION record: a decline has no graded line to pair one against. No comparator-liveness substitute stands beside it; E1's own MUTATION record above covers busrd's live/undecoded discrimination over the same swept set.

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

// A fresh Verilated model per trial (rig hazard):
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

// B4/B5's payload-word locator (see the file header's B4/B5 SAMPLING note
// for why it is fixed). Payload words are
// fifo_word_counter 32-543; word-counter 0-31 is sync/preamble/header/
// label/checksum framing, fixed by paula_floppy.v's own state-machine
// thresholds (:484-497) and structurally independent of what any cell
// encodes to -- confirmed directly from the RTL, matching the reading the
// already-accepted ch8-mfm-cell-encoding package established for this
// identical fixture. The FIFO preserves push order, so the Nth DSKDATR pop
// is always fifo_word_counter=N-1, and the payload's own first word
// (fifo_word_counter=32) is therefore always pop #33 -- a locator taken
// from the generator's own word-counter thresholds, not from what any cell
// encodes to, so it cannot break when the per-bit encoder (paula_floppy.v
// :467-477) is fixed. Every B4/
// B5 trial feeds a uniform 0xFF sector, so this one fixed index is as good
// a payload sample as any other.
constexpr int FRAMING_WORDS = 32;
constexpr int FIRST_PAYLOAD_POP_INDEX = FRAMING_WORDS + 1;

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    // ========================================================================
    // L0 -- FIXTURE_LIVENESS (no manual citation, no claim uid). Drive two
    // distinct, known DSKLEN writes and confirm they arrive, unmodified and
    // mutually distinguishable, at the DSKBYTR mirror bit (data_out bit 13)
    // that L1-L3, B1-B6 all also read.
    //
    // A note(), not an expect(): no uid, so this is a fixture-liveness
    // check, not conformance evidence, and a check with no claim uid must
    // not be counted by the coverage ledger. It carries no mutation record:
    // a note() has no graded line to pair one against.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dsklen_write(0x0000);
        uint16_t v1 = rig.read_reg(ADDR_DSKBYTR);
        rig.dsklen_write(0x4000);
        uint16_t v2 = rig.read_reg(ADDR_DSKBYTR);
        checks.note("L0-FIXTURE-LIVENESS: DSKLEN<=0x0000 -> DSKBYTR=0x" + hex4(v1) +
                    "; DSKLEN<=0x4000 -> DSKBYTR=0x" + hex4(v2) + " -- two distinct, known "
                    "DSKLEN writes arrive, unmodified and mutually distinguishable, at the "
                    "DSKBYTR mirror this package's every other assertion also reads (v1==0x9000 "
                    "&& v2==0xB000 && v1!=v2: " +
                    std::to_string(v1 == 0x9000 && v2 == 0xB000 && v1 != v2) +
                    ") -- the fixture's write-then-read data path is live");
    }

    // ========================================================================
    // L1 -- opus:8-dsk.c35, opus:8-dma.c13, opus:8-appA-dsklen.c1,
    //   sol:8-appa-dsklen.c1, opus:8-appB-dsklen.c1, sol:8-appb-dsklen.c1,
    //   opus:B-dsklen.addr (pp.241,246,279,289, DSKLEN's ADD cell): DSKLEN
    //   answers at $024, and at $024 only. DSKLEN has no direct read decode,
    //   so this is graded through the DSKBYTR mirror: writing the WRITE-bit
    //   at each of the 7 swept addresses, only $024 should move the mirror.
    //   opus:B-dsklen.addr restates the same p.289 Appendix B row already
    //   cited above; cited here rather than graded separately.
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
            "sol:8-appb-dsklen.c1, opus:B-dsklen.addr): a write reaches DSKLEN's own state "
            "(observable via the DSKBYTR mirror) only when addressed at $024 -- silent at "
            "$008, $01A, $020, $022, $026 and $07E");
    }

    // ========================================================================
    // L2 -- sol:8-dsklen.c1, opus:8-appA-dsklen.c2, sol:8-appa-dsklen.c2,
    //   opus:8-appB-dsklen.c3, sol:8-appb-dsklen.c3 (pp.246,279,289,
    //   composed with the appendix legend's "write-only"): a value
    //   presented at $024 reaches state, but reading $024 returns nothing
    //   written there.
    //
    // DECLINED: p.263-264 does
    //   not leave a read of a write-only register undefined, it forbids the
    //   operation outright -- "If a register is marked as a write-only
    //   register, do not attempt to read from it, as this may trash the
    //   register and crash the system" -- and gives no value for what such a
    //   read returns. Grading "reads 0x0000" performs the very operation the
    //   manual prohibits and checks it against a value the manual never
    //   gives: a conforming open-bus or nonzero response would fail this
    //   check, and a readable register that happens to return zero would
    //   pass it, for reasons the page states neither way. The measurement is
    //   kept as a note() (matching the precedent in
    //   ch7-beam-position-word-layout's VPOSW-READBACK); the claim itself is
    //   declined, not graded. Nothing in this package, or in
    //   ch8-dsklen-dma-arming/A1 (the same fault, declined there too), grades
    //   this claim -- there is no boundary at which the page supplies an
    //   oracle for it.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dsklen_write(0x1234);
        uint16_t v = rig.read_reg(ADDR_DSKLEN);
        checks.note("L2-STIMULUS: after DSKLEN<=0x1234, read at DSKLEN's own address gives 0x" +
                    hex4(v) + " (measured for interest only -- see the decline below)");
        checks.record(
            "L2-DSKLEN-WRITE-ONLY (p.246 sol:8-dsklen.c1; p.279 opus:8-appA-dsklen.c2, "
            "sol:8-appa-dsklen.c2; p.289 opus:8-appB-dsklen.c3, sol:8-appb-dsklen.c3): declined, "
            "not graded. p.263-264 forbids reading a write-only register outright ('do not "
            "attempt to read from it, as this may trash the register and crash the system') and "
            "gives no value for what such a read returns -- grading 'reads 0x0000' checks a "
            "forbidden operation against a value the page never gives. No boundary in this suite "
            "grades this claim (ch8-dsklen-dma-arming/A1 carries the same fault and is declined "
            "identically).");
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
    //   opus:8-appB-dskbytr.c1, sol:8-appb-dskbytr.c1, opus:B-dskbytr.addr
    //   (pp.241,278,289, DSKBYTR's ADD cell): DSKBYTR answers at $01A, and
    //   at $01A only. opus:B-dskbytr.addr restates the same p.289 Appendix
    //   B row already cited above; cited here rather than graded separately.
    //
    // The decode marker is DISKWRITE (bit 13, dsklen[14] -- a bit p.278
    //   DOES define, mirroring DSKLEN's own WRITE bit), set independently
    //   via a DSKLEN write, not "data_out != 0x0000": at idle that
    //   nonzero-ness comes from bit 15 and bit 12 of {1'b1,(trackrd|trackwr),
    //   dsklen[14],5'b1_0000,8'h00} (paula_floppy.v:699) -- both HARDWIRED
    //   1'b1 constants in this concatenation, not values p.278 documents as
    //   DSKBYTR's reset or idle state (p.278 gives DSKBYTR no reset/idle
    //   word at all, and marks bits 11-08 unused), so it would be the
    //   implementation's own idle encoding doing the decode work, not $01A's
    //   address decode itself. Address decode is graded on a bit the page
    //   actually specifies, set by a stimulus independent of this package's
    //   own read.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dsklen_write(0x4000);  // sets DISKWRITE (DSKBYTR bit 13) via DSKLEN's own WRITE bit.
        checks.note("B1-STIMULUS: after DSKLEN<=0x4000 (DISKWRITE set), sweeping DSKBYTR bit 13 "
                    "across all 7 register addresses");
        bool answers_only_at_dskbytr = true;
        for (int i = 0; i < 7; i++) {
            uint8_t addr = SWEEP[i];
            // MUTATE=B1: probe DSKLEN's address (no read decode) in the
            // DSKBYTR slot, while still judging by DSKBYTR's own address.
            uint8_t probe = (mutating("B1") && addr == ADDR_DSKBYTR) ? ADDR_DSKLEN : addr;
            uint16_t v = rig.read_reg(probe);
            bool diskwrite_set = (v & 0x2000) != 0;
            checks.note(std::string("B1-STIMULUS: read at ") + SWEEP_NAME[i] + " gives 0x" +
                        hex4(v) + " (bit13/DISKWRITE=" + std::to_string(diskwrite_set) + ")");
            bool should_answer = (addr == ADDR_DSKBYTR);
            if (diskwrite_set != should_answer) answers_only_at_dskbytr = false;
        }
        checks.expect(answers_only_at_dskbytr,
            "B1-DSKBYTR-ADDRESS (p.241 opus:8-dsk.c37; p.278 opus:8-appA-dskbytr.c1, "
            "sol:8-appa-dskbytr.c1; p.289 opus:8-appB-dskbytr.c1, sol:8-appb-dskbytr.c1, "
            "opus:B-dskbytr.addr): with "
            "DISKWRITE (bit 13, p.278-defined mirror of DSKLEN's WRITE bit) set via an "
            "independent DSKLEN write, that bit reads 1 only when read at $01A -- 0 at $008, "
            "$020, $022, $024, $026 and $07E -- narrowed from the whole idle word (0x9000) since "
            "p.278 gives DSKBYTR no reset/idle value and bits 15/12 are hardwired constants, not "
            "documented ones");
    }

    // ========================================================================
    // B2 -- opus:8-appA-dskbytr.c2, sol:8-appa-dskbytr.c2,
    //   opus:8-appB-dskbytr.c3, sol:8-appb-dskbytr.c3 (pp.278,289, composed
    //   with the legend's "read-only"): $01A answers on read, and nothing
    //   presented at $01A on a write reaches any state.
    //
    // The check is a before/after EQUALITY, not a specific idle word
    //   (0x9000 is a value p.278 never documents as DSKBYTR's reset/idle
    //   state -- see B1's own note above): whatever DSKBYTR happens to read
    //   before the write, it must read identically after -- this catches
    //   the regression that matters (the same 0x4000 payload moving
    //   DISKWRITE, bit 13, if it leaked through to DSKLEN's decode) without
    //   requiring a specific numeric idle value the page does not state.
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
        checks.expect(before == after,
            "B2-DSKBYTR-READ-ONLY (p.278 opus:8-appA-dskbytr.c2, sol:8-appa-dskbytr.c2; p.289 "
            "opus:8-appB-dskbytr.c3, sol:8-appb-dskbytr.c3): writing 0x4000 at $01A changes "
            "nothing observable in DSKBYTR's own readback (before==after) -- unlike the "
            "identical payload aimed at $024 (see L1/L3), which does move DISKWRITE (bit 13) -- "
            "proving $01A has no write decode; narrowed to a before/after equality rather than a "
            "fixed idle word (0x9000), since p.278 gives DSKBYTR no required reset/idle value");
    }

    // ========================================================================
    // B3 -- sol:8-reglist.c20, sol:8-appa-dskbytr.c5, sol:8-appb-dskbytr.c6
    //   (pp.241,278,289, the status half of the FUNCTION cell): reading
    //   DSKBYTR provides disk status that moves with module state. This is
    //   also this package's CONTROL CASE for B4/B5 (same $01A/data_out
    //   measurement basis, expected GREEN).
    //
    // The check reads DISKWRITE (bit 13, p.278-defined) alone, not the
    //   whole idle word: p.278 gives DSKBYTR no reset/idle word (see B1's
    //   note above), so requiring the idle reading to equal a specific
    //   hardwired-bit-dependent constant would over-specify the claim. The
    //   stimulus here only ever sets DSKLEN's
    //   WRITE bit (not DMAEN, which needs the double-write this trial does
    //   not perform), so DISKWRITE is the one status bit this exact stimulus
    //   can move.
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
        bool diskwrite_moved = ((after ^ idle) & 0x2000) != 0;
        checks.note("B3-STIMULUS: DSKBYTR idle=0x" + hex4(idle) + ", after arming write(s)=0x" +
                    hex4(after) + " (bit13/DISKWRITE moved=" + std::to_string(diskwrite_moved) + ")");
        checks.expect(diskwrite_moved,
            "B3-DSKBYTR-STATUS-HALF (p.241 sol:8-reglist.c20; p.278 sol:8-appa-dskbytr.c5; "
            "p.289 sol:8-appb-dskbytr.c6): DSKBYTR's DISKWRITE status bit (bit 13, "
            "p.278-defined) moves with module state -- this package's control case, on the same "
            "data_out@$01A basis as B4/B5, narrowed to bit 13 alone since p.278 gives DSKBYTR no "
            "reset/idle word for its other bits");
    }

    // ========================================================================
    // B4 -- sol:8-reglist.c19, sol:8-appa-dskbytr.c4, opus:8-appB-dskbytr.c5,
    //   sol:8-appb-dskbytr.c5 (pp.241,278,289, the data-byte half of the
    //   FUNCTION cell): reading DSKBYTR provides a disk data byte.
    //
    //   Sampling idle and a DSKLEN WRITE-bit state and passing if either
    //   low byte were nonzero could never refute the claim -- zero is a
    //   valid disk byte, and there would be no independent ground truth for
    //   what the "right" byte should be. THIS IS THE SAME UNDERLYING FACT as
    //   ch8-dskbytr-status-register/A9 and
    //   ch8-disk-controller-overview/opus:8-dsk.c9+sol:8-dsk.c9 -- one
    //   fact, graded identically in all three packages that carry it.
    //
    //   A KNOWN incoming byte (0xFF) is bound to the DSKBYTR sampling
    //   instant (see the file header's B4/B5 SAMPLING note): the trial
    //   feeds a uniform 0xFF sector, discards pops up to just before the
    //   fixed payload-start index (FIRST_PAYLOAD_POP_INDEX, defined above),
    //   PEEKS (does not pop) DSKDATR to confirm the known word is present
    //   RIGHT NOW, then peeks DSKBYTR -- zero clock edges elapse between
    //   the two peeks, so both are read at the SAME instant. Both bytes
    //   of 0xFFFF are 0xFF, sidestepping any high/low-byte ordering
    //   ambiguity in what "the current byte" means. A confirmation failure
    //   (bound_ok false) DECLINES via record() rather than grading.
    //
    //   RED -- paula_floppy.v:699 hardwires bits 7:0 to the literal
    //   constant 8'h00; a known, bound, nonzero incoming byte never
    //   appears there. Flagged by finding_candidates.md C6.
    // ========================================================================
    {
        int idx = FIRST_PAYLOAD_POP_INDEX;
        checks.note("B4-CALIBRATION: payload's first word (fifo_word_counter=32, "
                    "paula_floppy.v:484-497) is DSKDATR pop #" + std::to_string(idx));

        Rig rig;
        rig.do_reset();
        uint16_t stim = 0x8000 | 544;
        rig.dsklen_write(stim);
        rig.dsklen_write(stim);
        uint8_t sector[512];
        std::memset(sector, 0xFF, sizeof sector);
        rig.feed_sector_read(sector);
        for (int i = 1; i < idx; i++) rig.dskdatr_pop();  // discard up to just before the fixed payload-start index.
        uint16_t known_word = rig.read_reg(ADDR_DSKDATR);          // peek, no pop -- the payload word itself, at this instant.
        uint16_t dskbytr_at_instant = rig.read_reg(ADDR_DSKBYTR);  // SAME instant: zero clock edges elapsed since the peek above.
        bool bound_ok = (known_word == 0xFFFF);  // confirmation, not a locator: does the sampled word match this fixture's own known-byte encoding.
        uint8_t known_byte = 0xFF;  // both bytes of 0xFFFF are 0xFF -- no high/low ambiguity.
        uint8_t data_field = (uint8_t)(dskbytr_at_instant & 0x00FF);
        checks.note("B4-MEASURED: known_word=0x" + hex4(known_word) + " (bound=" +
                    std::to_string(bound_ok) + "), DSKBYTR at that same instant=0x" +
                    hex4(dskbytr_at_instant) + ", DATA field=0x" + hex4(data_field));
        if (!bound_ok) {
            // PRECONDITION FAILED -- decline, do not grade. This is the
            // branch a conforming encoder would always take (p.250 makes
            // 0xFF -> 0x5555, never 0xFFFF), and it must say nothing about
            // the DATA field either way.
            checks.record(
                "B4-DSKBYTR-DATA-BYTE-HALF (p.241 sol:8-reglist.c19; p.278 sol:8-appa-dskbytr.c4; "
                "p.289 opus:8-appB-dskbytr.c5, sol:8-appb-dskbytr.c5): could not confirm a known "
                "byte at the fixed payload-start sampling instant (pop #" + std::to_string(idx) +
                ", fifo_word_counter=32) -- the word read there was not this fixture's own known "
                "encoding of a uniform 0xFF byte (0xFFFF). Declining rather than grading an "
                "unconfirmed sample: 0xFFFF is not itself a page-legal MFM encoding of 0xFF (p.250 "
                "makes 0xFF -> 0x5555), so a mismatch here is not evidence about the DATA field one "
                "way or the other.");
        } else {
            checks.expect(data_field == known_byte,
                "B4-DSKBYTR-DATA-BYTE-HALF (p.241 sol:8-reglist.c19; p.278 sol:8-appa-dskbytr.c4; "
                "p.289 opus:8-appB-dskbytr.c5, sol:8-appb-dskbytr.c5): reading DSKBYTR provides a "
                "disk data byte -- with a KNOWN incoming byte (0xFF) bound to the sampling instant "
                "via the fixed payload-start index (paula_floppy.v:484-497), confirmed present by "
                "the peek above, DSKBYTR's own DATA field should read that same byte at that "
                "instant; it does not");
        }
    }

    // ========================================================================
    // B5 -- opus:8-dsk.c38, opus:8-appA-dskbytr.c4, sol:8-appa-dskbytr.c6
    //   (pp.241,278, the un-split compound printing): "DSKBYTR returns the
    //   disk data byte together with status" / "is the disk-to-
    //   microprocessor data buffer" -- graded as the conjunction B3 (true)
    //   AND B4 (false).
    //
    //   The byte half uses B4's known-byte-bound measurement (see B4 above
    //   for the full methodology) rather than an unbound "nonzero at least
    //   once" premise. It locates via the fixed FIRST_PAYLOAD_POP_INDEX and
    //   treats the marker match as a confirmation precondition, not a
    //   graded conjunct -- folding `known_word == 0xFFFF` into the
    //   conjunction would make the compound FAIL against a conforming
    //   encoder regardless of DSKBYTR's actual DATA field. An unconfirmed
    //   sample DECLINES the whole compound claim via record() (the status
    //   half is separately measured and reported, but is not itself the
    //   claim); a confirmed sample grades the ordinary conjunction. RED,
    //   for the same underlying reason as B4.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        uint16_t idle = rig.read_reg(ADDR_DSKBYTR);
        rig.dsklen_write(0x0000);
        rig.dsklen_write(0x4000);
        uint16_t armed = rig.read_reg(ADDR_DSKBYTR);
        bool status_half_real = (armed != idle);

        int idx = FIRST_PAYLOAD_POP_INDEX;
        Rig rig2;
        rig2.do_reset();
        uint16_t stim = 0x8000 | 544;
        rig2.dsklen_write(stim);
        rig2.dsklen_write(stim);
        uint8_t sector[512];
        std::memset(sector, 0xFF, sizeof sector);
        rig2.feed_sector_read(sector);
        for (int i = 1; i < idx; i++) rig2.dskdatr_pop();  // discard up to just before the fixed payload-start index.
        uint16_t known_word = rig2.read_reg(ADDR_DSKDATR);          // peek, no pop -- the payload word itself, at this instant.
        uint16_t dskbytr_now = rig2.read_reg(ADDR_DSKBYTR);         // SAME instant: zero clock edges elapsed since the peek above.
        bool bound_ok = (known_word == 0xFFFF);  // confirmation, not a locator.
        bool data_matches = ((dskbytr_now & 0x00FF) == 0xFF);

        checks.note("B5-MEASURED: status_half_real=" + std::to_string(status_half_real) +
                    ", bound=" + std::to_string(bound_ok) + ", data_matches=" +
                    std::to_string(data_matches) + " (byte half via the same known-byte "
                    "binding as B4)");
        if (!bound_ok) {
            checks.record(
                "B5-DSKBYTR-COMPOUND-BYTE-AND-STATUS (p.241 opus:8-dsk.c38; p.278 "
                "opus:8-appA-dskbytr.c4, sol:8-appa-dskbytr.c6): could not confirm a known byte at "
                "the fixed payload-start sampling instant used for the byte half (see B4) -- "
                "declining the compound claim rather than grading an unconfirmed byte half. The "
                "status half alone measured " + std::to_string(status_half_real) + "; a partial "
                "measurement is not the printed compound claim, which requires both halves.");
        } else {
            checks.expect(status_half_real && data_matches,
                "B5-DSKBYTR-COMPOUND-BYTE-AND-STATUS (p.241 opus:8-dsk.c38; p.278 "
                "opus:8-appA-dskbytr.c4, sol:8-appa-dskbytr.c6): the printed function cell claims "
                "BOTH a real status portion AND a real disk-data-byte portion -- the conjunction "
                "fails because, per B4's repaired and now-grounded measurement, the byte portion "
                "does not reflect a known bound incoming byte");
        }
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
    //   sol:8-appa-adkbit.c12, opus:8-appB-dsksync.c1, sol:8-appb-dsksync.c1,
    //   opus:B-dsksync.addr (pp.241,249,265,279,290, DSKSYNC's ADD cell / the
    //   (3F)*2 arithmetic): DSKSYNC answers (i.e. a write reaches its
    //   comparator) at $07E, and at $07E only. opus:B-dsksync.addr restates
    //   the same p.290 Appendix B row already cited above; cited here rather
    //   than graded separately.
    //
    // Why two halves: a wrong-address write ($024) with "no effect" alone
    //   is equally true of a DSKSYNC with NO write decode at all -- the RTL's
    //   own reset default (0x4489) happens to match the generator's own
    //   emitted sync word regardless of whether any write ever reaches the
    //   register, so that trial alone cannot tell "the write never reaches
    //   DSKSYNC" apart from "the write correctly reaches $07E, which
    //   happened to already hold a matching value." The POSITIVE half writes
    //   the SAME mismatching value (0xDEAD) AT DSKSYNC's own address and
    //   confirms it suppresses delivery -- proving the write genuinely
    //   reaches the comparator at $07E, not merely that $07E started out
    //   matching. The wrong-address trial is the negative half.
    // ========================================================================
    {
        uint8_t sector[512];
        std::memset(sector, 0x00, sizeof sector);
        const uint16_t LEN = 8;
        uint16_t stim = 0x8000 | LEN;

        // Positive half: write the mismatching value AT DSKSYNC's own address.
        Rig rigOwn;
        rigOwn.do_reset();
        rigOwn.dut->wordsync = 1;
        rigOwn.reg_write(ADDR_DSKSYNC, 0xDEAD);
        rigOwn.dsklen_write(stim);
        rigOwn.dsklen_write(stim);
        rigOwn.feed_sector_read(sector);
        int completed_own = rigOwn.drain_until_blckint(64);
        checks.note("S1-STIMULUS (positive half): wrote 0xDEAD at DSKSYNC's own address ($07E); "
                    "transfer completed after " +
                    (completed_own < 0 ? std::string("never (>64 reads, suppressed)") : std::to_string(completed_own)) +
                    " reads");

        // Negative half: the SAME write redirected to a wrong address ($024).
        Rig rigWrong;
        rigWrong.do_reset();
        rigWrong.dut->wordsync = 1;
        // MUTATE=S1: redirect the wrong-address write to DSKSYNC's own
        // address instead, which (correctly) suppresses delivery, while the
        // check below still expects the wrong-address trial's own
        // normal-delivery outcome.
        uint8_t write_addr = mutating("S1") ? ADDR_DSKSYNC : ADDR_DSKLEN;
        rigWrong.reg_write(write_addr, 0xDEAD);
        rigWrong.dsklen_write(stim);
        rigWrong.dsklen_write(stim);
        checks.note(std::string("S1-STIMULUS (negative half): wrote 0xDEAD at ") +
                    (write_addr == ADDR_DSKSYNC ? "DSKSYNC's own address (mutated)" : "DSKLEN's address (wrong)"));
        rigWrong.feed_sector_read(sector);
        int completed_wrong = rigWrong.drain_until_blckint(64);
        checks.note("S1-MEASURED (negative half): transfer completed after " +
                    (completed_wrong < 0 ? std::string("never (>64 reads)") : std::to_string(completed_wrong)) +
                    " reads");
        checks.expect(completed_own < 0 && completed_wrong > 0,
            "S1-DSKSYNC-ADDRESS (p.241 opus:8-dsk.c39; p.249 opus:8-adkcon.c22, "
            "sol:8-adkcon.c18; p.265 opus:8-appA-adkcon.c21, sol:8-appa-adkbit.c12; p.279 "
            "opus:8-appA-dsksync.c1, sol:8-appa-dsksync.c1; p.290 opus:8-appB-dsksync.c1, "
            "sol:8-appb-dsksync.c1, opus:B-dsksync.addr): a mismatching value written AT "
            "DSKSYNC's own address ($07E) "
            "suppresses delivery entirely -- the write genuinely reaches the comparator, not "
            "merely coincides with an already-matching reset default -- while the identical "
            "write redirected to a DIFFERENT address ($024) has no effect and delivery proceeds "
            "normally, together showing $07E (not $024) is where the write is decoded");
    }

    // ========================================================================
    // S2 -- opus:8-appA-dsksync.c2, sol:8-appa-dsksync.c2,
    //   opus:8-appB-dsksync.c3, sol:8-appb-dsksync.c3 (pp.279,290, composed
    //   with the legend's "write-only"): a value presented at $07E reaches
    //   state, but reading $07E returns nothing written there.
    //
    // DECLINED: same fault as L2 above -- p.263-264 forbids reading a
    //   write-only register outright and gives no value for what such a read
    //   returns. See L2's own comment for the full reasoning.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.reg_write(ADDR_DSKSYNC, 0x1234);
        uint16_t v = rig.read_reg(ADDR_DSKSYNC);
        checks.note("S2-STIMULUS: after DSKSYNC<=0x1234, read at DSKSYNC's own address gives 0x" +
                    hex4(v) + " (measured for interest only -- see the decline below)");
        checks.record(
            "S2-DSKSYNC-WRITE-ONLY (p.279 opus:8-appA-dsksync.c2, sol:8-appa-dsksync.c2; p.290 "
            "opus:8-appB-dsksync.c3, sol:8-appb-dsksync.c3): declined, not graded -- same fault "
            "as L2 above. p.263-264 forbids reading a write-only register outright ('do not "
            "attempt to read from it, as this may trash the register and crash the system') and "
            "gives no value for what such a read returns; grading 'reads 0x0000' checks a "
            "forbidden operation against a value the page never gives. No boundary in this suite "
            "grades this claim.");
    }

    // ========================================================================
    // S3 -- opus:8-dsk.c40, opus:8-dsk.c41, sol:8-reglist.c22,
    //   sol:8-reglist.c23, opus:8-appA-dsksync.c4, opus:8-appB-dsksync.c4,
    //   sol:8-appb-dsksync.c4 (pp.241,279,290, DSKSYNC's FUNCTION cell):
    //   DSKSYNC holds a word compared against the incoming disk data
    //   stream, gating delivery. With wordsync=1: DSKSYNC=0xDEAD (never
    //   produced by this fixture's own internal MFM generator) delivers
    //   ZERO words; DSKSYNC explicitly written with 0x4489 (the literal
    //   sync mark the generator itself emits) delivers the programmed
    //   count.
    //
    // Trial B explicitly WRITES 0x4489 to DSKSYNC's own address rather than
    //   leaving DSKSYNC at its reset default (0x4489): the RTL's reset value
    //   coincides with the manual's own stated "usual" sync word (p.250), and
    //   leaning on that coincidence would not be a genuine positive write.
    //   Trial A is the mismatching half and carries the MUTATE=S3 mutation.
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

        // Trial B: matching DSKSYNC, explicitly WRITTEN (not left at reset default).
        Rig rigB;
        rigB.do_reset();
        rigB.dut->wordsync = 1;
        rigB.reg_write(ADDR_DSKSYNC, 0x4489);
        rigB.dsklen_write(stim);
        rigB.dsklen_write(stim);
        rigB.feed_sector_read(sector);
        int completedB = rigB.drain_until_blckint(64);
        checks.note("S3-MEASURED (matching DSKSYNC, explicitly written 0x4489): completed=" +
                    std::to_string(completedB));

        checks.expect(completedA < 0 && completedB > 0,
            "S3-DSKSYNC-FUNCTION-MATCH-WORD (p.241 opus:8-dsk.c40, opus:8-dsk.c41, "
            "sol:8-reglist.c22, sol:8-reglist.c23; p.279 opus:8-appA-dsksync.c4; p.290 "
            "opus:8-appB-dsksync.c4, sol:8-appb-dsksync.c4): with wordsync enabled, a "
            "mismatching DSKSYNC value suppresses delivery entirely, while a matching value "
            "explicitly written to DSKSYNC's own address delivers the programmed count -- "
            "DSKSYNC's content, not merely its reset default, gates the read");
    }

    // ========================================================================
    // D1 -- opus:8-appA-dskdat.c1, sol:8-appa-dskdat.c1, opus:8-appB-dskdat.c1,
    //   sol:8-appb-dskdat.c1, opus:B-dskdat.addr (pp.279,289, DSKDAT's ADD
    //   cell): DSKDAT answers at $026, and at $026 only. Graded directly on
    //   the `buswr` public wire -- pure combinational address decode.
    //   opus:B-dskdat.addr restates the same p.289 Appendix B row already
    //   cited above; cited here rather than graded separately.
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
            "opus:8-appB-dskdat.c1, sol:8-appb-dskdat.c1, opus:B-dskdat.addr): `buswr` asserts "
            "only when reg_address_in selects $026 -- 0 at $008, $01A, $020, $022, $024 and "
            "$07E");
    }

    // ========================================================================
    // D2 -- opus:8-appA-dskdat.c2, sol:8-appa-dskdat.c2, opus:8-appB-dskdat.c4,
    //   sol:8-appb-dskdat.c4 (pp.279,289, composed with the legend's
    //   "write-only"): a value presented at $026 reaches state (verified
    //   via D3's FIFO effect), but reading $026 returns nothing written
    //   there.
    //
    // DECLINED: same fault as L2 above -- p.263-264 forbids reading a
    //   write-only register outright and gives no value for what such a read
    //   returns. See L2's own comment for the full reasoning.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.reg_write(ADDR_DSKDAT, 0x5678);
        uint16_t v = rig.read_reg(ADDR_DSKDAT);
        checks.note("D2-STIMULUS: after DSKDAT<=0x5678, read at DSKDAT's own address gives 0x" +
                    hex4(v) + " (measured for interest only -- see the decline below)");
        checks.record(
            "D2-DSKDAT-WRITE-ONLY (p.279 opus:8-appA-dskdat.c2, sol:8-appa-dskdat.c2; p.289 "
            "opus:8-appB-dskdat.c4, sol:8-appb-dskdat.c4): declined, not graded -- same fault as "
            "L2 above. p.263-264 forbids reading a write-only register outright ('do not attempt "
            "to read from it, as this may trash the register and crash the system') and gives no "
            "value for what such a read returns; grading 'reads 0x0000' checks a forbidden "
            "operation against a value the page never gives. No boundary in this suite grades "
            "this claim.");
    }

    // ========================================================================
    // D3 -- opus:8-appA-dskdat.c4, sol:8-appa-dskdat.c4, opus:8-appB-dskdat.c6,
    //   sol:8-appb-dskdat.c6 (pp.279,289, DSKDAT's FUNCTION cell): a write
    //   at $026 is disk DMA outgoing data -- the written value enters the
    //   disk-write data path.
    //
    // DECLINED: arming a 1-word WRITE transfer, writing one arbitrary word to
    //   DSKDAT, and grading blckint firing would not show the word was
    //   "genuinely disk-DMA outgoing data." blckint firing and the length
    //   counter reaching zero only show the written word entered the FIFO and was popped by the
    //   length-decrement path (paula_floppy.v:718-719, fifo_wr's buswr&dmaon
    //   term at :771) -- a controller that discarded every written word
    //   while still decrementing length would pass this exact check
    //   identically. The manual's actual claim is that the written value
    //   becomes genuine OUTGOING DATA; reaching that on `sdc_byte_out_data`
    //   requires the word to survive paula_floppy's separate cpu_wr_state
    //   parser (:781-901), which only forwards bytes once it has found a
    //   DSKSYNC-matching sync marker (state 0, :793-800), decoded and
    //   checksum-verified a 24-word header against the module's own `track`
    //   value (state 1, :802-847), and stepped through a payload-checksum
    //   preload (state 2, :848-863) -- fd_dma_buffer_data_wr, the only path
    //   into the buffer that `sdc_byte_out_data` reads back from, is driven
    //   solely from state 3 (:865-882, specifically :876). This trial's
    //   single arbitrary word (0xABCD) never matches dsksync and so never
    //   leaves state 0 of that parser; the blckint this check grades is
    //   driven entirely by the length-counter path, independent of it.
    //   Reaching a genuine outgoing byte needs the full sync+header+payload
    //   stream this package's other claims do not otherwise require -- see
    //   ch8-dsklen-dma-arming/A11, which attempts exactly that construction
    //   under its own citation.
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
        rig.reg_write(ADDR_DSKDAT, 0xABCD);
        bool completed = rig.wait_until([&]() { return (bool)rig.dut->blckint; }, 32);
        checks.note(std::string("D3-STIMULUS: wrote 0xABCD at DSKDAT during an armed 1-word ") +
                    "WRITE transfer; blckint observed=" + std::to_string(completed) +
                    " (measured for interest only -- see the decline below)");
        checks.record(
            "D3-DSKDAT-FUNCTION-DMA-WRITE (p.279 opus:8-appA-dskdat.c4, sol:8-appa-dskdat.c4; "
            "p.289 opus:8-appB-dskdat.c6, sol:8-appb-dskdat.c6): declined, not graded. blckint "
            "firing and the length counter reaching zero do not show the written word became "
            "genuine outgoing disk data -- a controller that discarded every written word while "
            "still decrementing length would pass this exact check identically. Reaching a "
            "genuine outgoing byte on `sdc_byte_out_data` requires the word to survive "
            "paula_floppy's separate cpu_wr_state parser (paula_floppy.v:781-901), which forwards "
            "bytes only from state 3 (:865-882) after a DSKSYNC-matching sync marker and a "
            "checksum- and track-verified header (states 0-2, :793-863); this trial's single "
            "arbitrary word never matches dsksync and never leaves state 0, so the blckint graded "
            "here is driven entirely by the independent length-counter path. See "
            "ch8-dsklen-dma-arming/A11, which builds exactly that full construction under its own citation and "
            "measures RED (the written byte survives fully intact -- no bits are lost in NanoMig's write path).");
    }

    // ========================================================================
    // E1 -- opus:8-appA-dskdat.c5, sol:8-appa-dskdatr.c1, opus:8-appB-dskdatr.c1,
    //   sol:8-appb-dskdatr.c1, opus:B-dskdatr.addr (pp.279,289, DSKDATR's ADD
    //   cell): DSKDATR answers at $008, and at $008 only. Graded directly on
    //   the `busrd` public wire. opus:B-dskdatr.addr restates the same
    //   p.289 Appendix B row already cited above; cited here rather than
    //   graded separately.
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
            "opus:8-appB-dskdatr.c1, sol:8-appb-dskdatr.c1, opus:B-dskdatr.addr): `busrd` "
            "asserts only when reg_address_in selects $008 -- 0 at $01A, $020, $022, $024, "
            "$026 and $07E");
    }

    // ========================================================================
    // E2 -- opus:8-appA-dskdat.c6, sol:8-appa-dskdatr.c2 (p.279, the `ER`
    //   mark's observable half): the read access at $008 is itself what
    //   advances the FIFO -- there is no separate strobe distinct from the
    //   access.
    //
    // DECLINED: same fault as E3 below, over the SAME p.287 `ER` legend. The
    //   legend states early-read transfers "are therefore initiated by Agnus
    //   timing, rather than a read address on the destination address bus"
    //   -- i.e. the manual explicitly says the CPU's own read ACCESS is NOT
    //   what triggers the transfer. Grading this claim would grade the
    //   opposite: that presenting/accessing $008 IS itself what advances the
    //   FIFO. That reads true on this fixture only because paula_floppy's own
    //   `fifo_rd` gating (busrd & dmaon, paula_floppy.v:911) is a plain
    //   address-decoded access strobe with no separate Agnus-timing signal
    //   behind it at all -- grading it as a pass would make the RTL's own
    //   implementation choice the oracle, which this suite's doctrine
    //   forbids. Distinguishing an Agnus-timed early transfer from a CPU
    //   destination-address-bus read needs the Agnus<->Paula boundary;
    //   `paula_floppy` compiled alone (this package's assigned fixture)
    //   presents reg_address_in identically regardless of what upstream
    //   hardware drove it, so it cannot make that distinction. Declining
    //   rather than grading the mechanism the page names as NOT what
    //   happens; the raw measurement is kept as a note().
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
        uint16_t r2 = rig.dskdatr_pop();
        uint16_t r3 = rig.dskdatr_pop();
        checks.note("E2-STIMULUS: three DSKDATR accesses returned 0x" + hex4(r1) + ", 0x" +
                    hex4(r2) + ", 0x" + hex4(r3) + " (measured for interest only -- see the "
                    "decline below)");
        checks.record(
            "E2-DSKDATR-EARLY-READ-ACCESS-IS-TRIGGER (p.279 opus:8-appA-dskdat.c6, "
            "sol:8-appa-dskdatr.c2): declined, not graded -- same fault as E3 below, over the "
            "same p.287 `ER` legend. The legend says early-read transfers are triggered by Agnus "
            "timing, 'rather than a read address on the destination address bus' -- the opposite "
            "of what this claim would grade. Distinguishing an Agnus-timed access from a CPU "
            "destination-address-bus access needs the Agnus<->Paula boundary; `paula_floppy` "
            "compiled alone (this package's assigned fixture) presents reg_address_in "
            "identically regardless of what upstream hardware drove it, so it cannot make that "
            "distinction. See E3's own comment below for the full reasoning.");
    }

    // ========================================================================
    // E3 -- opus:8-appA-dskdat.c8, sol:8-appa-dskdatr.c4, sol:8-appa-dskdatr.c5,
    //   opus:8-appB-dskdatr.c6, sol:8-appb-dskdatr.c6 (pp.279,289, the
    //   "dummy address" claim). DECLINED.
    //
    //   The decisive text is p.287's `ER` legend: early-read transfers put
    //   data on the bus early for RAM timing and "are therefore initiated
    //   by Agnus timing, rather than a read address on the destination
    //   address bus." "Dummy address" names WHO/WHAT triggers the bus
    //   cycle (Agnus's own DMA timing, not a CPU-issued read-address
    //   decode on the destination bus) -- it does not claim there is no
    //   real register or content behind $008, and does not require
    //   consecutive reads to be identical. A "no real content" reading is
    //   one the page does not make (and the live content E1/E2 above
    //   establish would fail it for the wrong reason).
    //
    //   Grading what the page ACTUALLY claims would need to distinguish "an Agnus-timed access" from "a CPU
    //   destination-address-bus access" -- a distinction that exists only
    //   at the Agnus<->Paula boundary. This package's assigned fixture
    //   (`paula_floppy` alone) has no such boundary: reg_address_in is
    //   presented identically regardless of what upstream hardware drove
    //   it there, and fifo_rd's own gating (busrd & dmaon,
    //   paula_floppy.v:911) cannot tell CPU-explicit access from
    //   Agnus-scheduled access apart. Declining rather than either grading
    //   the "no real content" reading or overreaching to a boundary
    //   (Agnus+Paula) this package's fixture does not include.
    //
    //   With E3 declined there is no graded comparator to defend against
    //   vacuous red; E1 above independently establishes busrd's
    //   live/undecoded discrimination over the same swept set.
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
            "access. An unstated 'no real content behind $008' reading, which E1/E2 above "
            "already show is false in an uninteresting way (a live, content-dependent FIFO "
            "answers there), is not what 'dummy' means on this page. Declining rather than "
            "grading that reading or overreaching to a boundary (Agnus+Paula) this package's "
            "fixture does not include.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
