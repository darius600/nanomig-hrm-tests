// Test purpose: HRM conformance package for DSKLEN's bit layout and the
//   double-write disk-DMA arming sequence (pp.246-247, Appendix A p.279),
//   against `paula_floppy` (paula_floppy.v + paula_floppy_fifo.v only --
//   paula_floppy_ebr.v excluded, it declares a second `module
//   paula_floppy`). 54 claims, from
//   work/ch8-s2-build/ch8-dsklen-dma-arming.jsonl, reduced to 12
//   propositions (A1-A12) plus fixture liveness (A0) and the A13
//   comparator substitute. Full derivation in SPECIFICATION.md.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed pp.246-247, 279.
// Non-goals: anything outside the `paula_floppy` fixture this package
//   builds, and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/ch8-s2-build/ch8-dsklen-dma-arming.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// SOURCES READ:
//   1. hrm/ch8-ciaapra-address-window/Makefile, test_ch8-ciaapra-address-
//      window.cpp, SPECIFICATION.md -- for this suite's established
//      harness/mutation boilerplate (the Checks class, the MUTATE env-var
//      convention, the tick()/settle()/do_reset() clocking pattern, the
//      summary-line format). No paula_floppy-specific fact came from this;
//      only the surrounding harness shape and suite conventions did.
//   2. nanomig/src/minimig-aga/paula_floppy.v -- full file (1041 lines). It
//      instantiates no dma_buf module, so paula_floppy_dma_buf.v is not
//      needed to elaborate this fixture; paula_floppy_ebr.v is excluded
//      because it declares a second `module paula_floppy`.
//   3. nanomig/src/minimig-aga/paula_floppy_fifo.v -- full file (60 lines):
//      a straightforward 2048-word-deep circular buffer, read/write pointer
//      equality for empty/full. Confirms the fixture never hits fifo_full
//      for any length this package programs (max 32 words << 2048 deep).
//   The `reg_address_in = addr >> 1` convention (the module's own port is
//   declared [8:1], dropping bit 0) follows from paula_floppy.v's own
//   address parameters (DSKLEN=9'h024, DSKBYTR=9'h01a, DSKDATR=9'h008,
//   DSKSYNC=9'h07e, DSKDAT=9'h026, each compared via `[8:1]`).
//   paula_floppy_ebr.v and paula_floppy_dma_buf.v were NOT read (not part of
//   this fixture).
//
// RTL FACTS THIS FILE RELIES ON (paula_floppy.v line numbers):
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
//     within a trial, but a FRESH `Vpaula_floppy` is allocated for every
//     trial below anyway (never one reused instance with repeated
//     do_reset()), rather than relying on this package's own trials never
//     happening to need it.
//
// EVERY MEASURED VALUE IN THIS PACKAGE'S OWN CLAIMS FILE (0x9000/0xB000/
//   0xD000/0xF000 idle/off/read-active/write-active; the M7/M8/M10/N2/N3/N5
//   sequences) reproduces exactly from the above RTL facts, worked through
//   by hand -- corroboration that the trace above, not just this file's
//   own run, is right.
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
// A11 -- opus:8-dma.c32 / sol:8-dskbug.c1 (last three bits lost on a disk
//   write) is GRADED (expect()): the claims file classifies this
//   proposition as observable at `sdc_byte_out_data`, and the cost of
//   reaching it is no reason to decline (cost is not unobservability).
//   drive_write_transfer_and_read() (defined right after the Rig struct)
//   drives a full, checksum-gated write transfer through paula_floppy's
//   cpu_wr_state pipeline (:781-901): a DSKSYNC-matching sync word, a
//   24-word header whose track field matches this fixture's own (not
//   otherwise observable) `track` value and whose 2-word checksum is
//   computed to match, a 4-word payload-checksum preload, and a uniform
//   512-byte payload -- then reads the corresponding byte back via
//   `sdc_byte_out_data`. MEASURED RED: the byte's last three bits survive
//   intact, same as the rest of it -- see A11's own comment for why (no
//   bit-serial shift-register stage exists anywhere in this write path for
//   the documented timing race to occur in). A13 is a substitute, proving the same apparatus is live and content-dependent (A11 cannot,
//   being itself expected red -- see MUTATION METHODOLOGY below).
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup -- not a compile-time #ifdef -- so one build serves the real run
//   (MUTATE unset) and every mutation run (MUTATE=<id>, one id per
//   assertion). A11 is expected RED at baseline (see its own comment): the
//   byte survives fully intact, so no ordinary stimulus mutation can make
//   "the byte does not survive" pass; it carries a `none impossible`
//   mutation record instead, and its comparator is proven live via the
//   dedicated GREEN-at-baseline substitute A13, which applies the identical
//   extraction-and-compare apparatus to a different known payload and
//   carries its own ordinary stimulus mutation. A1 (record(), declined) and
//   A0 (note(), no uid) carry no mutation record -- neither is a graded
//   line for one to pair against.
//
// A12 -- DECLINED, not graded. p.247 says the last word "may not come in",
//   not that one IS lost -- "may" demands nothing a single deterministic
//   trial can violate, so a graded-RED assertion would test MORE than the
//   page states. Declined via `record()`, quoting the page's own wording;
//   the measured fact this package's trial confirms -- NanoMig delivers
//   the FULL programmed word count, not one fewer, corroborating A9/A10's
//   own independent exact-count measurement -- is kept as a `note()`, not
//   a finding against a page that only ever claimed a possibility. With
//   A12 declined there is no graded comparator to defend against a
//   vacuous-red objection, so no comparator-liveness substitute stands
//   beside it; A9/A10 already prove the identical exact-count mechanism
//   live and green under their own manual citations.
//
// A0-LIVENESS-DSKLEN-WRITE-TO-DSKBYTR-READBACK is a note() (no manual citation, no uid) -- see its own comment below. It carries no MUTATION record: a note has no graded line to pair one against.
// A1-DSKLEN-WRITE-ONLY is DECLINED (record(), not expect()) -- same fault as ch8-disk-register-map-paula-floppy/L2; see its own comment below. It carries no MUTATION record.
// MUTATION A2-DSKLEN-TWO-CONTROL-BITS stimulus red: the single write is changed from 0x4000 (WRITE bit set) to 0x0000 (WRITE bit clear) while the check still expects the WRITE bit's immediate readback effect (MUTATE=A2). The check reads DISKWRITE (bit 13, p.278-defined) alone, not the whole readback word (0xB000).
// MUTATION A3-ENABLE-GATES-DMA-START stimulus red: `enable` is held at 1 throughout, instead of 0 during the double write and only raised afterward, while the check still expects the pre-enable "not yet started" reading (MUTATE=A3). The check reads DMAON (bit 14, p.278-defined) alone, not the whole readback word (0x9000/0xD000).
// MUTATION A4-DSKLEN-4000-FORCES-OFF-FROM-IDLE stimulus red: the forcing-off write is changed from 0x4000 to 0x0000 while the check still expects the forced-off (DMAON clear, DISKWRITE set) reading (MUTATE=A4). The scenario arms and starts a REAL transfer before forcing it off (a $4000 write from idle, where dmaen is already 0, would force nothing observable off), and the check reads DMAON/DISKWRITE (p.278-defined), not the whole readback word (0xB000). See the code's own comment.
// MUTATION A5-FIRST-WRITE-LOADS-DOES-NOT-START stimulus red: the single arming write is repeated a second time (accidentally arming DMAEN) while the check still expects the not-yet-started reading (MUTATE=A5). The check reads DMAON (bit 14, p.278-defined) alone, not the whole idle word (0x9000).
// MUTATION A6-SECOND-WRITE-ARMS-DMAEN stimulus red: only ONE write is performed, not two, while the check still expects the armed/started reading (MUTATE=A6). The check reads DMAON (bit 14, p.278-defined) alone, not the whole readback word (0xD000).
// MUTATION A7-DSKLEN-4000-FORCES-OFF-DURING-ACTIVE stimulus red: instead of writing 0x4000 to force DMA off mid-transfer, the same armed value is rewritten (re-affirming, not forcing off) while the check still expects the forced-off reading (MUTATE=A7). The check reads DMAON/DISKWRITE (p.278-defined) alone, not the whole readback word (0xB000).
// MUTATION A8-WRITE-DIRECTION-BIT-AND-PREVENTION stimulus red: the double write (0xC00A twice) is reduced to a single write while the check still expects the double-write's positive result (MUTATE=A8). The double-write half reads DMAON/DISKWRITE (p.278-defined) alone, not the whole readback word (0xF000).
// MUTATION A9-LENGTH-FIELD-COUNTS-WORDS stimulus red: DSKLEN is programmed with 33 words instead of 32 while the check still expects completion at exactly 32 reads (MUTATE=A9).
// MUTATION A10-LENGTH-DECREMENTS-STOPS-AT-ZERO stimulus red: DSKLEN is programmed with 16 words instead of 8 while the check still expects completion at exactly 8 reads, well before the fed sector's 544 MFM words are exhausted (MUTATE=A10).
// MUTATION A11-DSKBUG-LAST-THREE-BITS-LOST-ON-WRITE none impossible: graded and already RED at baseline (the written byte's last three bits survive intact; see A11's own comment). No stimulus this package can choose changes that outcome -- the write path has no bit-serial shift-register stage for the documented timing race to occur in, for any payload. The comparator that grades it is proven live via the substitute A13-DSKDAT-WRITE-BYTE-COMPARATOR-LIVE below.
// MUTATION A13-DSKDAT-WRITE-BYTE-COMPARATOR-LIVE stimulus red: the substitute's own payload is changed from 0x00 to 0xAA while the check still expects the 0x00 payload's own readback (0x00) (MUTATE=A13).
// A12 -- DSKBUG_ONE_WORD_SHORT_ON_READ: DECLINED via record(), not expect(); no mutation record (not graded through expect()) and no comparator-liveness substitute (nothing graded to defend) -- see the header's A12 note.

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

// A fresh Verilated model per trial, a rig hazard:
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

    // One qualifying-edge write at `addr`, then the address is withdrawn
    // (A11's own DSKDAT feed uses this; mirrors the identical helper
    // already validated in the sibling ch8-disk-register-map-paula-floppy
    // package's D3).
    void reg_write(uint8_t addr, uint16_t value) {
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

// Encodes byte `b` into a 16-bit word whose EVEN bit positions (0,2,4,...,14)
// carry b's bits 0..7 -- the exact positions paula_floppy.v's own `dbyte`
// extraction reads (dbyte = {fifo_out[14],fifo_out[12],...,fifo_out[0]},
// paula_floppy.v:~792 area -- a standard MFM data/clock demux that reads only
// the data positions and never inspects the odd "clock" positions at all).
// Odd positions are left 0 -- state1/state2/state3 never look at them.
static uint16_t encode_dbyte(uint8_t b) {
    uint16_t w = 0;
    for (int i = 0; i < 8; i++)
        if (b & (1 << i)) w |= (uint16_t)(1 << (2 * i));
    return w;
}

// A11's own apparatus: drives a full disk-write DMA transfer through
// paula_floppy's cpu_wr_state parser (paula_floppy.v:781-901) -- a
// DSKSYNC-matching sync word, a 24-word header whose track field matches
// this fixture's own (dsktrack has no reset term and starts at Verilator's
// construction-time zero, side=0 from do_reset -> track={dsktrack,~side}=
// 0x01) and whose 2-word checksum is computed to match, a 4-word
// payload-checksum preload, and a UNIFORM 512-byte payload of
// `payload_byte` -- then reads back the byte paula_floppy reports at SD-card
// sector offset `sdc_addr` via `sdc_byte_out_data`. A uniform payload
// sidesteps needing to reverse-engineer the odd/even bitplane interleave
// sdc_byte_out_data performs (paula_floppy.v:221-236, recombining
// fd_dma_buf_odd/fd_dma_buf_even): interleaving bits from two IDENTICAL
// source bytes yields that same byte back, whichever bit of which source
// each output bit happens to be wired from.
static uint8_t drive_write_transfer_and_read(uint8_t payload_byte, uint16_t sdc_addr) {
    Rig rig;
    rig.do_reset();
    const uint16_t LEN = 543;  // 1 sync + 24 header + 4 payload-csum-preload + 512 payload + 2 finalize
    uint16_t stim = 0xC000 | LEN;  // WRITE=1 (write direction), DMAEN=1
    rig.dsklen_write(stim);
    rig.dsklen_write(stim);
    rig.wait_until([&]() { return (bool)rig.dut->fdd_led; }, 16);

    // Sync word: must be the LITERAL 16-bit value dsksync holds (reset
    // default 0x4489) -- cpu_wr_state 0 compares fifo_out against it
    // directly, not through the dbyte extraction.
    rig.reg_write(ADDR_DSKDAT, 0x4489);

    // Header (24 words, cpu_wr_cnt 0..23). Only cnt=2's dbyte matters for
    // the track check (header_word[23:16] must equal track=0x01):
    // paula_floppy.v's own field-fill index is
    // wr_sector_header_word[{~cnt[0],i[2:0],~cnt[1]}] <= fifo_out[2*i] (i.e.
    // <= dbyte bit i). For cnt=2 that index is 16+2*i (i=0..7), so dbyte bit
    // 0 of word cnt=2 lands on header bit 16 and bits 1-3 land on 18/20/22;
    // for cnt=0 the index is 17+2*i, landing bits 0-3 on header bits
    // 17/19/21/23. header_word[23:16] must equal track=0x01 (bit16=1,
    // bits17-23=0), so header_bytes[2]=0x01 and header_bytes[0]=0x00 (and
    // every other header byte is unconstrained padding, left 0x00).
    uint8_t header_bytes[22];
    for (int c = 0; c < 22; c++) header_bytes[c] = 0x00;
    header_bytes[2] = 0x01;
    uint8_t csum_lo = 0, csum_hi = 0;
    for (int c = 0; c <= 21; c++) {
        if (c & 1) csum_hi ^= header_bytes[c];
        else       csum_lo ^= header_bytes[c];
    }
    for (int c = 0; c < 22; c++) rig.reg_write(ADDR_DSKDAT, encode_dbyte(header_bytes[c]));
    rig.reg_write(ADDR_DSKDAT, encode_dbyte(csum_lo));  // cnt=22: verified against the running checksum.
    rig.reg_write(ADDR_DSKDAT, encode_dbyte(csum_hi));  // cnt=23: verified, then the track check, then -> state 2.

    // Payload-checksum preload (state 2, cnt 0..3): the first two words are
    // unused by the RTL; the next two seed wr_sector_csum. A zero preload
    // against a uniform payload XORs to zero over an even byte count (each
    // value cancels itself out pairwise), so the later "data checksum
    // failed" gate at fd_dma_buf_wr_state==6 -- which this measurement does
    // not depend on either way, see the comment at the call site -- also
    // passes cleanly.
    for (int i = 0; i < 4; i++) rig.reg_write(ADDR_DSKDAT, encode_dbyte(0x00));

    // Payload (state 3, 512 words): uniform `payload_byte`.
    uint16_t payload_word = encode_dbyte(payload_byte);
    for (int i = 0; i < 512; i++) rig.reg_write(ADDR_DSKDAT, payload_word);

    // Finalize (state 4, 2 words): unused by the RTL beyond advancing cnt.
    for (int i = 0; i < 2; i++) rig.reg_write(ADDR_DSKDAT, encode_dbyte(0x00));

    rig.settle_edges(4);  // let fd_dma_buf_wr_state's own byte-copy loop catch up.

    rig.dut->sdc_byte_addr = sdc_addr;
    rig.tick();
    rig.tick();
    return (uint8_t)rig.dut->sdc_byte_out_data;
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    // ========================================================================
    // A0 -- FIXTURE DATA-PATH LIVENESS. No claim id, no manual page: the
    // fixture proves it can deliver a known, distinct word before any
    // read-based assertion is trusted. Two distinct, known DSKLEN writes
    // (0x0000, then 0x4000) must arrive, unmodified and mutually
    // distinguishable, at the DSKBYTR readback bit this package's every
    // other assertion also reads.
    //
    // A note(): no uid, so this is a fixture-liveness check, not
    // conformance evidence. It carries no mutation record: a note() has no
    // graded line to pair one against.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dsklen_write(0x0000);
        uint16_t v1 = rig.read_reg(ADDR_DSKBYTR);
        checks.note("A0-LIVENESS-WORD1: DSKLEN<=0x0000, DSKBYTR readback=0x" + hex4(v1));

        rig.dsklen_write(0x4000);
        uint16_t v2 = rig.read_reg(ADDR_DSKBYTR);
        checks.note("A0-LIVENESS-WORD2: DSKLEN<=0x4000, DSKBYTR readback=0x" + hex4(v2));

        checks.note("A0-LIVENESS-DSKLEN-WRITE-TO-DSKBYTR-READBACK: two distinct, known DSKLEN writes (0x0000 then "
            "0x4000) arrive, unmodified and mutually distinguishable, at the DSKBYTR mirror bit (data_out bit 13) "
            "this package's every other assertion also reads -- the fixture's write-then-read data path is live, "
            "not stuck (v1==0x9000 && v2==0xB000 && v1!=v2: " +
            std::to_string(v1 == 0x9000 && v2 == 0xB000 && v1 != v2) + ")");
    }

    // ========================================================================
    // A1 -- opus:8-dma.c12 (p.246, "All of the control bits relating to this
    // topic are contained in a write-only register, called DSKLEN"). DSKLEN
    // has no read decode: a written value does not come back on a read at
    // its own address.
    //
    // DECLINED: same fault as
    //   ch8-disk-register-map-paula-floppy/L2 -- p.263-264 does not leave a
    //   read of a write-only register undefined, it forbids the operation
    //   outright ("If a register is marked as a write-only register, do not
    //   attempt to read from it, as this may trash the register and crash
    //   the system") and gives no value for what such a read returns.
    //   Grading "reads 0x0000" performs the very operation the manual
    //   prohibits and checks it against a value the manual never gives. The
    //   measurement is kept as a note(); the claim itself is declined, not
    //   graded. Nothing in this suite grades this claim -- L2 in the sibling
    //   package carries the identical fault and is declined identically.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dsklen_write(0x1234);
        uint16_t v = rig.read_reg(ADDR_DSKLEN);
        checks.note("A1-STIMULUS: after DSKLEN<=0x1234, read at DSKLEN's own address gives 0x" + hex4(v) +
                    " (measured for interest only -- see the decline below)");
        checks.record(
            "A1-DSKLEN-WRITE-ONLY (p.246, opus:8-dma.c12, quote 'All of the control bits relating to this topic "
            "are contained in a write-only register, called DSKLEN:'): declined, not graded. p.263-264 forbids "
            "reading a write-only register outright ('do not attempt to read from it, as this may trash the "
            "register and crash the system') and gives no value for what such a read returns -- grading 'reads "
            "0x0000' checks a forbidden operation against a value the page never gives. No boundary in this suite "
            "grades this claim (ch8-disk-register-map-paula-floppy/L2 carries the same fault and is declined "
            "identically).");
    }

    // ========================================================================
    // A2 -- sol:8-appa-dsklen.c6 (p.279, "It also contains two control bits,
    // a DMA enable bit, and a DMA direction (read/write) bit."). The WRITE
    // (direction) bit is reflected on a SINGLE write, immediately -- a
    // behaviourally distinct control from DMAEN, which (per A5/A6) needs
    // two.
    //
    // The check reads DISKWRITE (bit 13, p.278-defined mirror of DSKLEN's
    //   WRITE bit) alone, not the whole readback word (0xB000): p.278 gives
    //   DSKBYTR no reset/idle word, and bits 15/12 of that word are
    //   hardwired constants (paula_floppy.v:699), not values p.278
    //   documents.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        // MUTATE=A2: write 0x0000 (WRITE bit clear) instead of 0x4000 (WRITE
        // bit set), single write, while still expecting the WRITE-set
        // readback (DISKWRITE=1).
        uint16_t stim = mutating("A2") ? 0x0000 : 0x4000;
        rig.dsklen_write(stim);
        uint16_t v = rig.read_reg(ADDR_DSKBYTR);
        bool diskwrite_set = (v & 0x2000) != 0;
        checks.note("A2-STIMULUS: single DSKLEN<=0x" + hex4(stim) + ", DSKBYTR readback=0x" + hex4(v) +
                    " (bit13/DISKWRITE=" + std::to_string(diskwrite_set) + ")");
        checks.expect(diskwrite_set,
            "A2-DSKLEN-TWO-CONTROL-BITS (p.279, sol:8-appa-dsklen.c6, quote 'It also contains two control\\n "
            "                bits, a DMA enable bit, and a DMA direction\\n                 (read/write) bit.'): "
            "a SINGLE write with the WRITE (direction) bit set takes effect immediately -- DISKWRITE (DSKBYTR bit "
            "13, p.278-defined) reads 1 -- unlike DMAEN (A5/A6), which needs two -- confirming WRITE and DMAEN are "
            "two separately-behaving control bits, not one shared mechanism; graded on bit 13 alone, not the whole "
            "readback word (0xB000), since p.278 gives DSKBYTR no required reset/idle value");
    }

    // ========================================================================
    // A3 -- opus:8-dma.c2 (p.246, "The disk DMA is controlled by four "
    // items:"), opus:8-dma.c20 / sol:8-dsklen.c9 (p.247, step 1, "Enable "
    // disk DMA in the DMACON register"). At this fixture's boundary, the
    // DMACON-derived enable is the `enable` port: a fully double-written,
    // armed DSKLEN does not start a transfer while enable=0; raising enable
    // alone (no further DSKLEN write) then starts it.
    //
    // The check reads DMAON (bit 14, p.278-defined mirror of dsklen's
    //   DMAEN, ANDed with `enable`), not the whole readback word before
    //   (0x9000) and after (0xD000): p.278 gives DSKBYTR no reset/idle word,
    //   and bits 15/12 are hardwired constants (paula_floppy.v:699), not
    //   documented ones. fdd_led is read as-is -- a genuine RTL output port,
    //   not a masked DSKBYTR bit.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dut->enable = 0;
        rig.dsklen_write(0x800A);
        rig.dsklen_write(0x800A);  // second write: dmaen arms, but dma_active still gated by enable=0
        uint16_t v_before = rig.read_reg(ADDR_DSKBYTR);
        bool led_before = rig.dut->fdd_led;
        bool dmaon_before = (v_before & 0x4000) != 0;
        checks.note("A3-STIMULUS: DSKLEN armed (0x800A x2) with enable=0: DSKBYTR=0x" + hex4(v_before) +
                    " (bit14/DMAON=" + std::to_string(dmaon_before) + "), fdd_led=" + std::to_string(led_before));

        // MUTATE=A3: hold enable=1 throughout (instead of 0 then 1), so the
        // "not yet started" reading above would already be wrong.
        if (!mutating("A3")) {
            rig.dut->enable = 1;
        }
        rig.settle_edges(4);
        uint16_t v_after = rig.read_reg(ADDR_DSKBYTR);
        bool led_after = rig.dut->fdd_led;
        bool dmaon_after = (v_after & 0x4000) != 0;
        checks.note("A3-STIMULUS: enable raised to 1 (no further DSKLEN write): DSKBYTR=0x" + hex4(v_after) +
                    " (bit14/DMAON=" + std::to_string(dmaon_after) + "), fdd_led=" + std::to_string(led_after));

        checks.expect(!dmaon_before && !led_before && dmaon_after && led_after,
            "A3-ENABLE-GATES-DMA-START (p.246, opus:8-dma.c2, quote 'The disk DMA is controlled by four items:'; "
            "p.247, opus:8-dma.c20/sol:8-dsklen.c9, quote step 1 'Enable disk DMA in the  DMACON  register'): a "
            "fully double-written, armed DSKLEN does not start a transfer while this fixture's DMACON-derived "
            "`enable` port is 0 (DMAON, bit 14, stays 0; fdd_led stays 0); raising enable alone, with no further "
            "DSKLEN write, then starts it (DMAON sets, fdd_led becomes 1) -- the enable item is a real, "
            "independent precondition, not a formality; graded on DMAON (bit 14, p.278-defined) alone, not the "
            "whole readback word (0x9000/0xD000), since p.278 gives DSKBYTR no required reset/idle value");
    }

    // ========================================================================
    // A4 -- opus:8-dma.c21 / sol:8-dsklen.c10 (p.247, step 2, "Set DSKLEN to
    // $4000, thereby forcing the DMA for the disk to be turned off.").
    //
    //   (1) ORACLE: the check reads DMAON (bit 14) and DISKWRITE (bit 13,
    //       the bit $4000's own payload sets), both p.278-defined, not the
    //       whole readback word (0xB000) -- p.278 gives DSKBYTR no
    //       reset/idle word, and bits 15/12 are hardwired constants
    //       (paula_floppy.v:699), not documented ones.
    //   (2) SCENARIO: the trial arms and starts a real transfer first (as
    //       A6/A7 do), confirms it is genuinely active, THEN writes $4000
    //       and confirms DMA actually turns off -- reaching the page's own
    //       "forcing... off" mechanism. From an already-IDLE state `dmaen`
    //       is already 0: writing $4000 sets dsklen[15]<=data_in[15]=0 and
    //       dmaen<=data_in[15]&dsklen[15]_old = 0&0 = 0 -- a value dmaen
    //       already held (paula_floppy.v:735-744) -- so the write would
    //       observably force nothing OFF and the trial would only grade a
    //       WRITE-bit readback (DISKWRITE), which A2 already grades under
    //       its own citation. (A7 grades the same active-to-off transition
    //       under its own, different citation -- p.247 step 5, not step 2;
    //       both are real, independent printings of the same underlying
    //       fact.)
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dsklen_write(0x800A);
        rig.dsklen_write(0x800A);  // second write arms and starts DMA
        rig.settle_edges(4);
        bool led_active = rig.dut->fdd_led;
        uint16_t v_active = rig.read_reg(ADDR_DSKBYTR);
        bool dmaon_active = (v_active & 0x4000) != 0;

        // MUTATE=A4: write 0x0000 instead of 0x4000, while still expecting
        // the forced-off reading.
        uint16_t stim = mutating("A4") ? 0x0000 : 0x4000;
        rig.dsklen_write(stim);
        rig.settle_edges(4);
        uint16_t v = rig.read_reg(ADDR_DSKBYTR);
        bool led = rig.dut->fdd_led;
        bool dmaon = (v & 0x4000) != 0;
        bool diskwrite = (v & 0x2000) != 0;
        checks.note("A4-STIMULUS: armed and active (fdd_led=" + std::to_string(led_active) + ", DMAON=" +
                    std::to_string(dmaon_active) + "), then DSKLEN<=0x" + hex4(stim) + ": DMAON=" +
                    std::to_string(dmaon) + " DISKWRITE=" + std::to_string(diskwrite) + " fdd_led=" +
                    std::to_string(led));
        checks.expect(led_active && dmaon_active && diskwrite && !dmaon && !led,
            "A4-DSKLEN-4000-FORCES-OFF-FROM-IDLE (p.247, opus:8-dma.c21/sol:8-dsklen.c10, quote step 2 'Set DSKLEN "
            "to $4000, thereby forcing the DMA for the disk to be\\n      turned off.'): starting from a genuinely "
            "ACTIVE transfer (fdd_led and DMAON both set beforehand), writing $4000 forces it off -- DMAON (bit "
            "14) drops, fdd_led drops, DISKWRITE (bit 13, the bit $4000 itself sets) reads 1 -- graded on DMAON "
            "and DISKWRITE, both p.278-defined, rather than the whole readback word; and started from an ACTIVE "
            "transfer rather than idle, whatever the name says, since from idle `dmaen` is already 0 and the "
            "write forces nothing observable off");
    }

    // ========================================================================
    // A5 -- opus:8-dma.c17/sol:8-dsklen.c6 (p.247, "The hardware requires a "
    // special sequence..."), opus:8-dma.c22/sol:8-dsklen.c11 (p.247, step 3,
    // "Put the value you want into the DSKLEN register."). A single write
    // loads LENGTH/WRITE but does not start DMA.
    //
    // The check reads DMAON (bit 14, p.278-defined), not the whole idle
    //   readback word (0x9000) -- p.278 gives DSKBYTR no reset/idle word,
    //   and bits 15/12 are hardwired constants (paula_floppy.v:699): a
    //   single write of 0x800A (WRITE=0, DMAEN's bit set but not yet armed)
    //   must leave DMAON clear.
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
        bool dmaon = (v & 0x4000) != 0;
        checks.note("A5-STIMULUS: DSKLEN<=0x800A written " + std::to_string(mutating("A5") ? 2 : 1) +
                    " time(s): DSKBYTR=0x" + hex4(v) + " (bit14/DMAON=" + std::to_string(dmaon) + "), fdd_led=" +
                    std::to_string(led));
        checks.expect(!dmaon && !led,
            "A5-FIRST-WRITE-LOADS-DOES-NOT-START (p.247, opus:8-dma.c17/sol:8-dsklen.c6, quote 'The hardware "
            "requires a special sequence in order to start DMA to the\\ndisk.'; opus:8-dma.c22/sol:8-dsklen.c11, "
            "quote step 3 'Put the value you want into the DSKLEN register.'): a single write of 0x800A does not "
            "start DMA (DMAON, bit 14, stays clear; fdd_led=0) -- a single write is not the special sequence the "
            "page requires; graded on DMAON alone, not the whole idle word (0x9000), since p.278 gives DSKBYTR "
            "no required reset/idle value");
    }

    // ========================================================================
    // A6 -- twelve uids (see SPECIFICATION.md A6): DSKLEN bit 15 (DMAEN) is
    // a SECONDARY enable, arming DMA only on the second of two consecutive
    // writes with the bit set.
    //
    // The check reads DMAON (bit 14, p.278-defined mirror of DMAEN), not
    //   the whole readback word (0xD000) -- p.278 gives DSKBYTR no
    //   reset/idle word, and bits 15/12 are hardwired constants
    //   (paula_floppy.v:699). fdd_led and sdc_rd are read as-is -- genuine
    //   RTL output ports, not masked DSKBYTR bits.
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
        bool dmaon = (v & 0x4000) != 0;
        checks.note("A6-STIMULUS: DSKLEN<=0x800A written " + std::to_string(mutating("A6") ? 1 : 2) +
                    " time(s): DSKBYTR=0x" + hex4(v) + " (bit14/DMAON=" + std::to_string(dmaon) + "), fdd_led=" +
                    std::to_string(led) + ", sdc_rd[0]=" + std::to_string(sdc_rd0));
        checks.expect(dmaon && led && sdc_rd0,
            "A6-SECOND-WRITE-ARMS-DMAEN (p.246, opus:8-dma.c6/sol:8-dskdma.c5 'DMA enable'; opus:8-dma.c14/"
            "sol:8-dsklen.c3/opus:8-appA-dsklen.c8/sol:8-appa-dsklen.c9 'DMAEN Secondary disk DMA enable'; "
            "opus:8-appA-dsklen.c6/sol:8-appa-dsklen.c7 Appendix A DSKLEN paragraph; p.247, opus:8-dma.c19/"
            "sol:8-dsklen.c8 'must be turned on twice'; opus:8-dma.c23/sol:8-dsklen.c12 step 4 'Write this value "
            "again into the DSKLEN register. This actually\\n      starts the DMA.'): a second write repeating "
            "0x800A arms DMAEN and starts disk read DMA (DMAON, bit 14, sets; fdd_led=1, sdc_rd requested for the "
            "selected drive) -- graded on DMAON alone, not the whole readback word (0xD000), since p.278 gives "
            "DSKBYTR no required reset/idle value");
    }

    // ========================================================================
    // A7 -- opus:8-dma.c24/sol:8-dsklen.c13 (p.247, step 5, "After the DMA "
    // is complete, set the DSKLEN register back to $4000, to prevent "
    // accidental writes to the disk."). $4000 forces DMA off from an ACTIVE
    // transfer (A4 grades the same transition under p.247 step 2).
    //
    // The check reads DMAON (bit 14) clearing and DISKWRITE (bit 13, the
    //   bit $4000 itself sets), both p.278-defined, not the whole readback
    //   word after (0xB000) -- p.278 gives DSKBYTR no reset/idle word, and
    //   bits 15/12 are hardwired constants (paula_floppy.v:699).
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dsklen_write(0x800A);
        rig.dsklen_write(0x800A);
        rig.settle_edges(4);
        bool led_active = rig.dut->fdd_led;  // sanity: transfer is running before we force it off
        bool dmaon_active = (rig.read_reg(ADDR_DSKBYTR) & 0x4000) != 0;
        // MUTATE=A7: instead of forcing off with 0x4000, re-affirm the armed
        // value (stays active) while still expecting the forced-off reading.
        if (mutating("A7")) rig.dsklen_write(0x800A);
        else                rig.dsklen_write(0x4000);
        rig.settle_edges(4);
        uint16_t v = rig.read_reg(ADDR_DSKBYTR);
        bool led = rig.dut->fdd_led;
        bool dmaon = (v & 0x4000) != 0;
        bool diskwrite = (v & 0x2000) != 0;
        checks.note("A7-STIMULUS: was active (fdd_led=" + std::to_string(led_active) + ", DMAON=" +
                    std::to_string(dmaon_active) + "), then DSKLEN<=0x" + hex4(mutating("A7") ? 0x800A : 0x4000) +
                    ": DMAON=" + std::to_string(dmaon) + " DISKWRITE=" + std::to_string(diskwrite) + " fdd_led=" +
                    std::to_string(led));
        checks.expect(led_active && dmaon_active && !dmaon && diskwrite && !led,
            "A7-DSKLEN-4000-FORCES-OFF-DURING-ACTIVE (p.247, opus:8-dma.c24/sol:8-dsklen.c13, quote step 5 'After "
            "the DMA is complete, set the DSKLEN register back to\\n      $4000, to prevent accidental writes to "
            "the disk.'): writing $4000 while a transfer is ACTIVE also forces it off (DMAON, bit 14, drops; "
            "DISKWRITE, bit 13, reads 1; fdd_led drops to 0), the same transition A4 grades under p.247 step 2 -- "
            "graded on DMAON/DISKWRITE, not the whole readback word (0xB000), since p.278 gives DSKBYTR no "
            "required reset/idle value");
    }

    // ========================================================================
    // A8 -- twelve uids (see SPECIFICATION.md A8): DSKLEN bit 14 (WRITE)
    // selects RAM-to-disk direction, and the double-write sequence is what
    // prevents a single stray write (even with WRITE and DMAEN both set)
    // from starting an accidental disk write.
    //
    // The double-write half reads DMAON (bit 14) and DISKWRITE (bit 13),
    //   both p.278-defined, not the whole readback word (0xF000) -- p.278
    //   gives DSKBYTR no reset/idle word, and bits 15/12 are hardwired
    //   constants (paula_floppy.v:699). dmal/dmas/sdc_wr/fdd_led are read
    //   as-is -- genuine RTL output ports, not masked DSKBYTR bits.
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
        bool dmaon_double = (v_double & 0x4000) != 0;
        bool diskwrite_double = (v_double & 0x2000) != 0;
        checks.note("A8-STIMULUS (double write, " + std::to_string(mutating("A8") ? 1 : 2) + " write(s) applied): "
                    "0xC00A: DSKBYTR=0x" + hex4(v_double) + " (bit14/DMAON=" + std::to_string(dmaon_double) +
                    ", bit13/DISKWRITE=" + std::to_string(diskwrite_double) + "), dmal=" +
                    std::to_string(dmal_double) + ", dmas=" + std::to_string(dmas_double));

        Rig rig2;
        rig2.do_reset();
        rig2.dsklen_write(0xC00A);  // single write only, negative/control half -- not mutated
        rig2.settle_edges(4);
        bool dmas_single = rig2.dut->dmas;
        bool sdc_wr_single = (rig2.dut->sdc_wr != 0);
        bool led_single = rig2.dut->fdd_led;
        checks.note("A8-STIMULUS (single write): 0xC00A once: dmas=" + std::to_string(dmas_single) +
                    ", sdc_wr=" + std::to_string(sdc_wr_single) + ", fdd_led=" + std::to_string(led_single));

        checks.expect(dmaon_double && diskwrite_double && dmal_double && dmas_double &&
                      !dmas_single && !sdc_wr_single && !led_single,
            "A8-WRITE-DIRECTION-BIT-AND-PREVENTION (p.246, opus:8-dma.c5/sol:8-dskdma.c4 'Direction of data "
            "transfer (read/write)'; opus:8-dma.c15/sol:8-dsklen.c4 'WRITE   Disk write (RAM -> disk if 1)'; "
            "p.279, opus:8-appA-dskdat.c12/sol:8-appa-dskdata.c5 'write mode is enabled by bit 14'; "
            "opus:8-appA-dsklen.c7/sol:8-appa-dsklen.c8 Appendix A DSKLEN paragraph; opus:8-appA-dsklen.c9/"
            "sol:8-appa-dsklen.c10 'WRITE Disk write (RAM to disk) if 1'; p.247, opus:8-dma.c18/sol:8-dsklen.c7 "
            "'This sequence prevents accidental writes to the disk.'): double-writing 0xC00A (WRITE+DMAEN) arms a "
            "RAM-to-disk transfer (DMAON and DISKWRITE both set; dmal=1, dmas=1); a SINGLE write of the same value "
            "starts nothing (dmas=0, sdc_wr=0, fdd_led=0) -- the two-write sequence is what prevents that one "
            "stray write from becoming an accidental disk write; graded on DMAON/DISKWRITE, not the whole "
            "readback word (0xF000), since p.278 gives DSKBYTR no required reset/idle value");
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
    // A11 -- opus:8-dma.c32 / sol:8-dskbug.c1 (p.247, "There is a hardware
    // bug that causes the last three bits of data sent to the disk to be
    // lost.").
    //
    // Graded: the claims file classifies this proposition as observable at
    //   `sdc_byte_out_data`, and the cost of reaching it is no reason to
    //   decline (cost is not unobservability).
    //   drive_write_transfer_and_read() (defined above, right after the Rig
    //   struct) constructs the full sync+header+payload stream the write
    //   path needs (paula_floppy.v:781-901) and reads back the byte
    //   paula_floppy reports at the LAST offset of the 512-byte SD sector
    //   (address 511) via `sdc_byte_out_data`. A uniform 0xFF payload is
    //   used throughout so that offset's intended value is known and
    //   unambiguous without needing to reverse-engineer the odd/even
    //   bitplane interleave `sdc_byte_out_data` performs (see the helper's
    //   own comment).
    //
    //   MEASURED RED: the RTL's write path copies every payload byte
    //   through whole -- `fd_dma_buffer_data_wr <= dbyte` (paula_floppy.v:
    //   876, state 3) is a full 8-bit assignment, and both the copy into
    //   fd_dma_buf_even/odd (:378-384) and the recombination into
    //   sdc_byte_out_data (:221-236) are likewise bit-for-bit, with no
    //   conditional truncation of any bit range anywhere in either stage.
    //   Real Paula's documented bug is a timing race at the tail of a
    //   bit-serial disk-write shift sequence; this reimplementation has no
    //   bit-serial shift-register stage in its write path at all -- it is
    //   fully byte-buffered from the FIFO onward -- so there is no mechanism
    //   present for a byte's low bits to be dropped in transit, and the
    //   measurement below confirms it: the low three bits survive
    //   unchanged, same as the rest of the byte.
    // ========================================================================
    {
        uint8_t observed_ff = drive_write_transfer_and_read(0xFF, 511);
        bool upper5_intact = (observed_ff & 0xF8) == 0xF8;
        bool low3_lost = (observed_ff & 0x07) != 0x07;
        checks.note("A11-MEASURED: full write transfer, uniform payload 0xFF, sdc_byte_out_data at offset 511 "
                    "(the last byte of the sector) = 0x" + hex4(observed_ff) + " (upper 5 bits intact=" +
                    std::to_string(upper5_intact) + ", low 3 bits lost=" + std::to_string(low3_lost) + ")");
        checks.expect(low3_lost,
            "A11-DSKBUG-LAST-THREE-BITS-LOST-ON-WRITE (p.247, opus:8-dma.c32/sol:8-dskbug.c1, quote 'There is a "
            "hardware bug that causes the last three bits of data sent to\\nthe disk to be lost.'): a byte written "
            "as the LAST word of a disk-write DMA transfer should have its last three bits lost by the time it "
            "reaches the disk -- writing 0xFF and reading back the corresponding SD-sector byte "
            "(sdc_byte_out_data, offset 511) should NOT show 0xFF; it does (upper 5 bits and low 3 bits both "
            "survive intact) -- NanoMig's write path is fully byte-buffered from the CPU's DSKDAT write through "
            "to sdc_byte_out_data, with no bit-serial shift-register stage anywhere for the documented timing "
            "race to occur in, so this claim does not hold here");
    }

    // ========================================================================
    // A13 -- DSKDAT-WRITE-BYTE-COMPARATOR-LIVE (substitute, no manual
    // citation). A11 is expected RED
    // (see above) -- the byte survives fully intact, so no ordinary
    // stimulus mutation can make "the byte does not survive" pass; A11
    // carries a `none impossible` mutation record instead (see the top of
    // file). This substitute applies the SAME apparatus
    // (drive_write_transfer_and_read + sdc_byte_out_data readback) to a
    // DIFFERENT known payload (0x00 instead of 0xFF) and confirms the
    // reading tracks it -- proving the apparatus reports real,
    // content-dependent data, not a stuck or vacuous constant, and is not
    // the reason A11 reads red.
    // ========================================================================
    {
        // MUTATE=A13: feed a different, nonzero payload while the check
        // still expects the 0x00 payload's own readback (0x00).
        uint8_t payload = mutating("A13") ? 0xAA : 0x00;
        uint8_t observed = drive_write_transfer_and_read(payload, 511);
        checks.note("A13-STIMULUS: full write transfer, uniform payload 0x" + hex4(payload) +
                    ", sdc_byte_out_data at offset 511 = 0x" + hex4(observed));
        checks.expect(observed == 0x00,
            "A13-DSKDAT-WRITE-BYTE-COMPARATOR-LIVE: the same write-transfer-and-readback apparatus A11 uses, fed "
            "a DIFFERENT known payload (0x00 instead of 0xFF), correctly reports that different content at "
            "sdc_byte_out_data -- the apparatus is live and content-dependent, not the reason A11 reads red");
    }

    // ========================================================================
    // A12 -- opus:8-dma.c33 / sol:8-dskbug.c2 (p.247, "Also, the last word "
    // in a disk-read DMA operation may\nnot come in (that is, one less word "
    // may be read than you asked for)."). DECLINED, not graded: the page
    // says a word MAY be lost, not that one is; "may" demands nothing a
    // single deterministic trial can violate, so grading `delivered ==
    // programmed - 1` as a hard requirement would test a claim the page
    // never made. With nothing graded there is no comparator to defend
    // against a vacuous-red objection, so no comparator-liveness substitute
    // stands beside it; A9/A10 already grade the exact-count mechanism
    // green.
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
