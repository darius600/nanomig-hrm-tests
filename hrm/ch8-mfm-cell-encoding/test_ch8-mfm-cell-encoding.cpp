// Test purpose: HRM conformance package for the MFM cell-encoding table on
//   p.250 (1 -> 01; 0 -> 10 if following a 0; 0 -> 00 if following a 1) and
//   the "raw MFM data ... twice as large" sentence, same page, against
//   `paula_floppy` (paula_floppy.v + paula_floppy_fifo.v only --
//   paula_floppy_ebr.v excluded, it declares a second `module
//   paula_floppy`). 8 claims, from
//   work/ch8-s2-build/ch8-mfm-cell-encoding.jsonl, reduce to 4 assertions +
//   fixture liveness. Full derivation in SPECIFICATION.md (committed as
//   7adfeb4, before any RTL was opened).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed p.250.
// Non-goals: anything outside the `paula_floppy` fixture this package
//   builds, and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the printed pages independently, and an id names
//   one reader's reading of one sentence. They resolve in
//   `work/ch8-s2-build/ch8-mfm-cell-encoding.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// RTL READ, IN ORDER, ALL AFTER SPECIFICATION.md WAS COMMITTED (7adfeb4):
//   1. hrm/ch8-dsklen-dma-arming/Makefile, test_ch8-dsklen-dma-arming.cpp,
//      SPECIFICATION.md -- this suite's established harness/mutation
//      boilerplate against this SAME fixture (the Checks class, the MUTATE
//      env-var convention, the tick()/settle_edges()/do_reset() clocking
//      pattern, dsklen_write()/read_reg()/feed_sector_read()/dskdatr_pop(),
//      the summary-line format, the spec/commit-message format). This
//      package's Rig class and register-write/read/feed/pop helpers are
//      adapted directly from that file (same fixture, same register
//      addresses, same reset sequence, same 0x8000|LENGTH read-mode arming);
//      no NEW RTL fact was taken from it that this file does not also
//      confirm independently below.
//   2. hrm/ch8-dskbytr-status-register/test_paula_floppy_dskbytr.cpp
//      (header comment only, not the body) -- for this suite's precedent on
//      recording a red-baseline substitute mutation directly against the
//      red assertion's own id with outcome `green`, rather than inventing a
//      separate uid-less "apparatus" assertion.
//   3. nanomig/src/minimig-aga/paula_floppy.v -- full file (1041 lines).
//   4. nanomig/src/minimig-aga/paula_floppy_fifo.v -- full file (60 lines):
//      a straightforward 2048-word-deep, order-preserving circular buffer.
//      Confirms popped words come out in exactly push order, so the Nth
//      DSKDATR pop corresponds to the Nth word the internal generator wrote.
//   paula_floppy_ebr.v and paula_floppy_dma_buf.v were NOT read (not part of
//   this fixture; the build brief already gives the reason to exclude EBR,
//   and paula_floppy.v -- read in full -- instantiates no dma_buf module).
//
// RTL FACTS THIS FILE RELIES ON (paula_floppy.v line numbers):
//   - The encoder is context-free. Payload cells (:467-477): `data_odd` and
//     `data_even` each interleave a CONSTANT `1'b1` clock bit with each
//     payload data bit, so every payload bit d is emitted as the cell pair
//     "1d" -- "11" for d=1, "10" for d=0 -- regardless of what precedes it.
//     (Header cells, :453-462, use a constant `1'b0` clock instead; this
//     package does not test header cells, only payload/data cells, which is
//     what p.250's DATA-field table describes.)
//   - `data_odd` (:467-471) is built from `fd_dma_buf_out`'s ODD-indexed
//     bits, MSB first: cell 0 = bit[15], cell 1 = bit[13], cell 2 = bit[11],
//     ... cell 7 = bit[1]. `fd_dma_buf_out[15:8]` is the SD sector byte at
//     the EVEN address of the current byte pair ("B_even"); `[7:0]` is the
//     byte at the ODD address ("B_odd") (:221-222, :228-236, :315-316). So
//     the first TWO cells of a `data_odd` word are, in true bit-stream
//     order, the cell for B_even's bit 7 followed immediately by the cell
//     for B_even's bit 5 -- both from the SAME fed byte, so their
//     predecessor relationship is fully known from the byte value alone,
//     with no dependency on anything outside this one popped word. This
//     package reads ONLY that second cell (bits [13:12] of the popped word)
//     for its context-dependent assertions (A3, A4), and the first cell
//     (bits [15:14]) for its context-FREE assertion (A2) -- deliberately
//     avoiding any claim about the very first cell's own predecessor, which
//     is the last cell of the previous field and not needed by any
//     assertion here.
//   - Payload words are `fifo_word_counter` 32-543 (512 words: :494-495,
//     "word 32-287 data odd bits" / "word 288-544 data even bits" per the
//     RTL's own comment); words 0-31 are sync/preamble/header/label/
//     checksum framing, fixed by the state machine's word-counter
//     thresholds, never touched by data_odd/data_even (:484-493). The FIFO
//     (paula_floppy_fifo.v) preserves push order, so the Nth DSKDATR pop
//     after arming is the word generated when `fifo_word_counter` was N-1 --
//     so the payload's own first word (fifo_word_counter=32) is always
//     DSKDATR pop #33. Every locator in this file now takes that index
//     directly from the RTL's own word-counter threshold (see
//     `FIRST_PAYLOAD_POP_INDEX` below and the Lot G note further down this
//     header), not from a runtime content search.
//   - `dsklen` LENGTH (bits 13-0) counts words the internal generator PUSHES
//     into the FIFO (:718-719, decrementing on `fifo_wr`, not on the CPU's
//     own pop), and `blckint` fires once the count reaches zero AND the
//     FIFO has been fully drained (:994-1019). Programming LENGTH=544 and
//     draining exactly 544 DSKDATR reads therefore covers exactly one whole
//     sector's encoded stream (framing + payload), matching this package's
//     own claims file's own disclosed "600-word read of one sector"
//     methodology (a superset that also works, this file uses the tighter
//     544 bound since the exact word count is now RTL-confirmed).
//   - `dsktrack` (:129, :671-680) has NO reset term anywhere in the module
//     -- confirmed by reading the whole file. This package never drives
//     `_step`, so `dsktrack` never changes from its Verilator
//     construction-time zero within a trial, but per the build brief a
//     FRESH `Vpaula_floppy` is still allocated for every trial below
//     (never one reused instance), matching the rig-hazard mandate
//     literally.
//   - DSKLEN needs the double write (:735-744) to arm `dmaen`; a single
//     write loads LENGTH/WRITE but not the DMA-enable latch. This package
//     always writes its arming value twice, exactly as
//     hrm/ch8-dsklen-dma-arming's own A6 established.
//
// WHY A "PAYLOAD START" INDEX IS NOW A FIXED CONSTANT, NOT A CONTENT SEARCH
//   (Lot G repair, 2026-09-16 -- supersedes this section as it stood
//   through Lot F). The original form of every payload-word locator in
//   this file -- A0's, A2's, A3's and A4's -- called
//   the withdrawn search helper, which fed a uniform 0xFF sector on its
//   own throwaway rig and searched up to 60 DSKDATR pops for the FIRST
//   exact 0xFFFF match. That word is not page-legal: p.250's "1 -> 01" is
//   unconditional, so a uniform 0xFF byte page-encodes to 0x5555, never
//   0xFFFF -- 0xFFFF is only what THIS fixture's own non-conformant,
//   context-free encoder (candidate C9, graded by A2/A4 below) happens to
//   produce for a run of one-bits. Against a conforming encoder the search
//   finds nothing and returns -1, and every caller's own read loop,
//   `for (int i = 1; i <= idx; i++) w = rig.dskdatr_pop();`, then never
//   executes (the loop bound is below its start): `w` stays its
//   initializer 0, and A2/A3/A4 would each grade a cell extracted from a
//   word that was never actually popped -- a verdict for a reason other
//   than the one it names.
//
//   A1 hit this same failure mode against itself in the Lot F repair above
//   and switched to trusting the generator's own `fifo_word_counter`
//   word-counter threshold selection directly (paula_floppy.v:484-497)
//   instead of searching for content. This Lot G repair applies the
//   identical fix to the three remaining callers -- the withdrawn search
//   helper is removed from this file entirely, replaced by the single
//   constant `FIRST_PAYLOAD_POP_INDEX` defined below, next to the register-
//   address constants:
//
//   Every trial in this file feeds a UNIFORM 512-byte sector (one repeated
//   byte value), so every payload word encodes that SAME byte -- which
//   payload word is read carries no information A0/A2/A3/A4 need. The
//   payload's own FIRST word is therefore always fifo_word_counter=32 (a
//   data_odd word, :494, "word 32-287 data odd bits"), i.e. DSKDATR pop
//   #33 (the FIFO preserves push order, confirmed paula_floppy_fifo.v, so
//   the Nth pop is fifo_word_counter=N-1). This index comes from the same
//   structurally-distinct word-counter threshold logic A1 already relies
//   on (:484-497), unrelated to the per-bit encoder logic (:467-477) that
//   is the actual site of candidate C9 -- fixing that bug would never move
//   this threshold. It is scheme-neutral: it holds against a conforming
//   encoder exactly as it does against this one, since it never asks what
//   any cell encodes to.
//
// FIXTURE-LIVENESS. A0 proves two distinct, KNOWN, non-default words (0xFF
//   sector -> 0xFFFF; 0x88 sector -> 0xEEEE, both hand-derived below and
//   neither reachable from any fixed framing default) arrive unmodified at
//   the exact DSKDATR read path every other assertion in this package also
//   uses. WITHDRAWN TO note() (Lot F, 2026-09-15): A0 carries no claim uid,
//   so it was never conformance evidence -- see its own comment below.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup -- not a compile-time #ifdef -- so one build serves the real
//   run (MUTATE unset) and every mutation run (MUTATE=<id>, one id per
//   assertion). Baseline with MUTATE unset (re-measured 2026-09-15, Lot
//   F): 2 passed, 2 failed, plus A0 now MEASURED (note(), not graded) --
//   A1 (doubling, repaired to a page-legal 0x00/0xAAAA marker scoped to
//   the known payload index range) and A3 (0-after-0) green, A2 (1->01)
//   and A4 (0-after-1) red. A2 and A4's red is the deliverable (finding
//   candidate C9): actual run counts and per-mutation results are recorded
//   in the commit message and build report, not predicted here.
//
// A2 and A4 are RED at baseline (candidate C9: the clock cell is a
//   constant 1 for every payload bit, so context never changes the
//   result). Per the build brief, a stimulus mutation on an assertion
//   already red cannot demonstrate anything further (it cannot fail
//   harder). The substitute rule applies: A3's own live, green
//   0-after-0 reading uses the IDENTICAL mechanism (feed a uniform payload
//   byte, locate the payload word by the same fixed payload-start index, extract
//   the same bits[13:12] cell, compare by the same == comparator) and is
//   already proven live and green elsewhere in this same run. Per the
//   suite's ch8-dskbytr-status-register precedent, A2's and A4's own
//   `// MUTATION` records point at that substitute directly, with outcome
//   `green`, recorded against their OWN assertion id -- not a separate
//   uid-less id -- so the record pairs against each assertion's own (red)
//   baseline correctly.
//
// A0-DISTINCT-KNOWN-WORDS-ARRIVE -- no claim uid; withdrawn to note() (Lot F, 2026-09-15). No mutation record (not graded through expect()).
// MUTATION A1-MFM-DOUBLES-SIZE stimulus red (repaired stimulus, Lot F, 2026-09-15): the armed LENGTH is reduced from 544 to 300 (a shorter transfer), while the check still expects exactly 512 payload words within the payload range carrying the 0xAAAA marker (MUTATE=A1).
// MUTATION A2-MFM-ONE-ENCODES-01 stimulus green: substitute -- A3-MFM-ZERO-AFTER-ZERO-ENCODES-10's own live, already-graded reading (uniform 0x00 sector, identical payload-word-search and bits[13:12]-extraction mechanism, identical 2-bit equality comparator) is a different mechanism, proven live elsewhere in this same run, put through the same comparator, and it grades green -- showing the comparator apparatus is not vacuously red. A2's own red above is a genuine divergence (finding candidate C9), not a stuck check.
// MUTATION A3-MFM-ZERO-AFTER-ZERO-ENCODES-10 stimulus red: the real trial's sector is changed from uniform 0x00 to uniform 0xFF (the same byte value A0/A2 feed), so the checked cell (bits[13:12], still compared against 0b10) now reads 0b11 instead (MUTATE=A3).
// MUTATION A4-MFM-ZERO-AFTER-ONE-ENCODES-00 stimulus green: substitute -- A3-MFM-ZERO-AFTER-ZERO-ENCODES-10's own live, already-graded reading is the same different-mechanism-through-the-same-comparator demonstration as A2's record above (see that record for the full reasoning; not repeated here to avoid two conflicting explanations of one fact). A4's own red above is a genuine divergence (finding candidate C9), not a stuck check.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
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
std::string bin2(uint16_t v) {
    char b[3];
    b[0] = ((v >> 1) & 1) ? '1' : '0';
    b[1] = (v & 1) ? '1' : '0';
    b[2] = '\0';
    return std::string(b);
}

// Register addresses, as this fixture's own reg_address_in[8:1] values
// (paula_floppy.v's own parameters DSKLEN=9'h024, DSKDATR=9'h008, each
// compared via [8:1] -- confirmed directly from the RTL, matching
// hrm/ch8-dsklen-dma-arming's own independently-derived constants).
const uint8_t ADDR_NONE    = 0x00;
const uint8_t ADDR_DSKDATR = 0x04;  // $008 >> 1
const uint8_t ADDR_DSKLEN  = 0x12;  // $024 >> 1

// Payload words are fifo_word_counter 32-543; word-counter 0-31 is
// sync/preamble/header/label/checksum framing, fixed by the state
// machine's own thresholds and never touched by data_odd/data_even --
// confirmed directly from paula_floppy.v:484-497 (RTL FACTS above, the
// same reading A1 relies on). The FIFO preserves push order
// (paula_floppy_fifo.v), so the Nth DSKDATR pop after arming is always
// fifo_word_counter=N-1, and the payload's own first word
// (fifo_word_counter=32) is therefore always pop #33 -- a locator taken
// from the generator's own word-counter thresholds, not from what any
// cell encodes to (Lot G repair, 2026-09-16; see the header note above for
// why the withdrawn content-search locator could not be trusted against a
// conforming encoder). Every trial below feeds a
// UNIFORM sector, so this one fixed index is as good a payload sample as
// any other for A0/A2/A3/A4.
constexpr int FRAMING_WORDS = 32;
constexpr int FIRST_PAYLOAD_POP_INDEX = FRAMING_WORDS + 1;
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
// would carry head position across trials. Every calibration search and
// every real measurement below constructs its own Rig.
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
        dut->_step = 1;      // inactive (active low), never driven low: no head stepping
        dut->direc = 0;
        dut->_sel = 0xE;     // drive 0 selected (active low, 4'b1110)
        dut->side = 0;
        dut->_motor = 1;     // inactive; not consulted by the data path this package tests
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

    // Arm a read-direction transfer (WRITE bit clear, DMAEN double-written)
    // for `length` words and feed it a uniform 512-byte sector of `byteval`.
    void arm_and_feed_uniform(uint16_t length, uint8_t byteval) {
        uint16_t dsklen_val = 0x8000 | length;  // bit15=DMAEN, bit14=0 (read direction)
        dsklen_write(dsklen_val);
        dsklen_write(dsklen_val);
        uint8_t sector[512];
        std::memset(sector, byteval, 512);
        feed_sector_read(sector);
    }
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    // ========================================================================
    // A0 -- FIXTURE DATA-PATH LIVENESS. No claim id, no manual page.
    // WITHDRAWN TO note() (independent validation, 2026-09-15, Lot F): this
    // assertion carries no claim uid -- it was never conformance evidence,
    // only a fixture control, so grading it through expect() cost no
    // coverage to remove. It also used 0xFF -> 0xFFFF as one of its two
    // known words, which is this RTL's own non-conformant encoder output,
    // not a page-legal value (p.250's "1 -> 01" is unconditional, so a
    // uniform 0xFF byte page-encodes to 0x5555, not 0xFFFF) -- printing it
    // as a measurement rather than a grade removes any conformance weight
    // that choice carried. Its mutation record is removed with it.
    // ========================================================================
    {
        int idx = FIRST_PAYLOAD_POP_INDEX;
        checks.note("A0-CALIBRATION: payload's first word (fifo_word_counter=32, paula_floppy.v:484-497) is DSKDATR pop #" + std::to_string(idx));

        Rig rig1;
        rig1.do_reset();
        rig1.arm_and_feed_uniform(544, 0xFF);
        uint16_t v1 = 0;
        for (int i = 1; i <= idx; i++) v1 = rig1.dskdatr_pop();

        Rig rig2;
        rig2.do_reset();
        rig2.arm_and_feed_uniform(544, 0x88);
        uint16_t v2 = 0;
        for (int i = 1; i <= idx; i++) v2 = rig2.dskdatr_pop();

        checks.note("A0-DISTINCT-KNOWN-WORDS-ARRIVE (no claim uid, MEASURED not GRADED): two distinct, known "
                    "sector patterns (uniform 0xFF, uniform 0x88) arrive, unmodified and mutually distinguishable, "
                    "at the exact same DSKDATR/data_out payload-word read this package's every other assertion "
                    "also uses -- the fixture's SD-sector-in -> stream-out data path is live, not stuck. Measured "
                    "pop #" + std::to_string(idx) + " reads 0x" + hex4(v1) + " for a uniform 0xFF sector and 0x" +
                    hex4(v2) + " for a uniform 0x88 sector.");
    }

    // ========================================================================
    // A1 -- opus:8-mfm.c1 / sol:8-mfm.c1 (p.250, "The raw MFM data that must
    // be presented to the disk controller will be twice as large as the
    // unencoded data.").
    //
    // REPAIRED (independent validation, 2026-09-15, Lot F): the withdrawn
    // form searched for a content marker of exactly 0xFFFF (a uniform 0xFF
    // sector's payload word) and counted its occurrences across the whole
    // drained stream. 0xFFFF is NOT a page-legal MFM encoding of 0xFF --
    // p.250's "1 -> 01" is unconditional, so 0xFF page-encodes to 0x5555 --
    // it is only what THIS RTL's own non-conformant, context-free encoder
    // (candidate C9, graded by A2/A4 below) produces for a run of one-bits.
    // Against a conforming encoder the search would never find it, and this
    // assertion would count zero and fail, reporting a doubling defect that
    // is not there.
    //
    // Repaired to a uniform 0x00 sector instead: p.250's own rule makes
    // 0x00 page-legally encode to 0xAAAA (0 -> 10 "if following a 0", the
    // steady state for a run of zero bits) -- true of BOTH a conforming
    // encoder and this RTL's own context-free one (confirmed: A3 below
    // measures exactly this), so counting 0xAAAA payload words is safe
    // either way. Counting is scoped to the PAYLOAD index range alone
    // (fifo_word_counter 32-543, 512 words: DSKDATR pops 33-544),
    // confirmed by this agent directly reading the generator's own
    // word-counter THRESHOLD selection (paula_floppy.v:484-497) -- a
    // structurally distinct piece of logic from, and unrelated to, the
    // per-bit encoder (:467-477) that is the actual site of the C9
    // conformance bug; nothing about fixing that bug would move this
    // threshold. This also sidesteps a risk a whole-stream content search
    // would have carried: framing words 30/31 (paula_floppy.v:492-493,
    // the PAYLOAD checksum) are content-derived and, unlike every other
    // framing word, DO depend on the fed bytes -- a fixed index range
    // excludes them by construction; a content search across the whole
    // stream would not have.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        // MUTATE=A1: arm a shorter transfer (300 words instead of 544), so
        // fewer than 512 payload words are ever delivered, while the check
        // still expects exactly 512.
        uint16_t length = mutating("A1") ? 300 : 544;
        rig.arm_and_feed_uniform(length, 0x00);

        // FRAMING_WORDS (fifo_word_counter 0-31, confirmed paula_floppy.v:484-497)
        // is now defined once, file-scope, above -- shared with A0/A2/A3/A4's
        // FIRST_PAYLOAD_POP_INDEX (Lot G repair).
        int to_discard = (length < FRAMING_WORDS) ? length : FRAMING_WORDS;
        for (int i = 0; i < to_discard; i++) rig.dskdatr_pop();
        int payload_words = length - to_discard;

        int marker_count = 0;
        for (int i = 0; i < payload_words; i++) {
            if (rig.dskdatr_pop() == 0xAAAA) marker_count++;
        }
        int encoded_bytes = marker_count * 2;
        checks.note("A1-STIMULUS: LENGTH armed=" + std::to_string(length) +
                    "; payload words available=" + std::to_string(payload_words) +
                    "; 0xAAAA payload words counted=" + std::to_string(marker_count) +
                    " (" + std::to_string(encoded_bytes) + " encoded bytes) against 512 unencoded payload bytes fed");

        checks.expect(marker_count == 512,
            "A1-MFM-DOUBLES-SIZE (p.250, opus:8-mfm.c1/sol:8-mfm.c1, quote 'The raw MFM data that must be "
            "presented to the disk controller will be\\ntwice as large as the unencoded data.'): a uniform 512-byte "
            "SD sector fed on sdc_byte_in_data (uniform 0x00, whose page-legal MFM encoding is 0xAAAA per p.250) "
            "produces exactly 512 payload words within the known payload index range (fifo_word_counter 32-543) "
            "at data_out -- 1024 encoded bytes for 512 unencoded payload bytes, exactly twice, matching the page");
    }

    // ========================================================================
    // A2 -- opus:8-mfm.c2 / sol:8-mfm.c2 (p.250, "               1 -> 01").
    // Context-free rule, no predecessor needed: read the FIRST cell
    // (bits[15:14]) of the payload word at the fixed payload-start index
    // (FIRST_PAYLOAD_POP_INDEX), fed from a byte whose bit 7 is 1.
    // ========================================================================
    {
        int idx = FIRST_PAYLOAD_POP_INDEX;

        Rig rig;
        rig.do_reset();
        // MUTATE=A2: real trial substituted below (see the file header's
        // MUTATION record: an already-red assertion gets the substitute
        // rule, not a stimulus mutation of its own -- this block still runs
        // its ordinary, unmutated stimulus every time).
        rig.arm_and_feed_uniform(544, 0xFF);  // bit7=1 on every fed byte
        uint16_t w = 0;
        for (int i = 1; i <= idx; i++) w = rig.dskdatr_pop();
        uint16_t cell = (w >> 14) & 0x3;

        checks.note("A2-STIMULUS: payload word (pop #" + std::to_string(idx) + ") = 0x" + hex4(w) +
                    "; first cell (bit 7 of the fed byte, value 1) = " + bin2(cell) + "b");

        checks.expect(cell == 0b01,
            "A2-MFM-ONE-ENCODES-01 (p.250, opus:8-mfm.c2/sol:8-mfm.c2, quote '               1 -> 01'): an "
            "unencoded data bit of 1 should encode, unconditionally, as the cell pair 01. Measured on a byte whose "
            "bit 7 is 1: this fixture's encoder emits " + bin2(cell) + " instead -- the clock cell is a hardwired "
            "1 (paula_floppy.v:467-477) rather than being set from context, so every 1 comes out 11. This is the "
            "deliverable (finding candidate C9), not something to explain away by loosening the assertion to "
            "match what NanoMig does.");
    }

    // ========================================================================
    // A3 -- opus:8-mfm.c3 / sol:8-mfm.c3 (p.250, "               0 -> 10   "
    // ";if following a 0"). Read the SECOND cell (bits[13:12]) of the
    // payload word, fed from a byte whose bits 7 and 5 are both 0 -- the
    // predecessor relationship (bit 5 follows bit 7, same word, same fed
    // byte) is fully known, no dependency outside this one popped word.
    // CONTROL CASE and SUBSTITUTE SOURCE for A2/A4 (see file header): this
    // assertion's own live, green result is what A2's and A4's `//
    // MUTATION` records point to as the "different mechanism, proven live
    // elsewhere in the same run" the build brief's substitute rule asks
    // for, since it exercises the IDENTICAL payload-word locator,
    // bits[13:12]-extraction, and 2-bit equality comparator those two
    // assertions use.
    // ========================================================================
    {
        int idx = FIRST_PAYLOAD_POP_INDEX;

        Rig rig;
        rig.do_reset();
        // MUTATE=A3: the real trial's sector is changed from uniform 0x00
        // to uniform 0xFF (the same byte value A0/A2 feed), while the
        // check still expects the 0-after-0 reading (0b10).
        uint8_t byteval = mutating("A3") ? 0xFF : 0x00;  // bits 7 and 5 both 0 at baseline
        rig.arm_and_feed_uniform(544, byteval);
        uint16_t w = 0;
        for (int i = 1; i <= idx; i++) w = rig.dskdatr_pop();
        uint16_t cell = (w >> 12) & 0x3;

        checks.note("A3-STIMULUS: sector byte=0x" + hex4(byteval).substr(2) + "; payload word (pop #" +
                    std::to_string(idx) + ") = 0x" + hex4(w) + "; second cell (bit 5, following bit 7, both 0) = " +
                    bin2(cell) + "b");

        checks.expect(cell == 0b10,
            "A3-MFM-ZERO-AFTER-ZERO-ENCODES-10 (p.250, opus:8-mfm.c3/sol:8-mfm.c3, quote '               0 -> 10   "
            ";if following a 0'): an unencoded data bit of 0, whose immediately preceding unencoded data bit is "
            "also 0, should encode as the cell pair 10. Measured on a byte whose bits 7 and 5 are both 0: this "
            "fixture's encoder emits " + bin2(cell) + " -- matching the page, but (per finding candidate C9) for "
            "the wrong reason: the clock cell is a hardwired 1 regardless of context "
            "(paula_floppy.v:467-477), so this row happens to agree with a context-blind encoder. A green here "
            "does not by itself certify context-sensitivity -- only A2 and A4, which the same hardwired clock "
            "makes disagree with the page, settle that.");
    }

    // ========================================================================
    // A4 -- opus:8-mfm.c4 / sol:8-mfm.c4 (p.250, "               0 -> 00   "
    // ";if following a 1"). Same mechanism as A3, fed from a byte whose bit
    // 7 is 1 and bit 5 is 0.
    // ========================================================================
    {
        int idx = FIRST_PAYLOAD_POP_INDEX;

        Rig rig;
        rig.do_reset();
        rig.arm_and_feed_uniform(544, 0x80);  // 1000_0000: bit7=1, bit5=0
        uint16_t w = 0;
        for (int i = 1; i <= idx; i++) w = rig.dskdatr_pop();
        uint16_t cell = (w >> 12) & 0x3;

        checks.note("A4-STIMULUS: payload word (pop #" + std::to_string(idx) + ") = 0x" + hex4(w) +
                    "; second cell (bit 5, following bit 7=1) = " + bin2(cell) + "b");

        checks.expect(cell == 0b00,
            "A4-MFM-ZERO-AFTER-ONE-ENCODES-00 (p.250, opus:8-mfm.c4/sol:8-mfm.c4, quote '               0 -> 00   "
            ";if following a 1'): an unencoded data bit of 0, whose immediately preceding unencoded data bit is 1, "
            "should encode as the cell pair 00. Measured on a byte whose bit 7 is 1 and bit 5 is 0: this fixture's "
            "encoder emits " + bin2(cell) + " instead -- the clock cell is a hardwired 1 (paula_floppy.v:467-477) "
            "rather than being cleared when the previous bit was 1, so this 0 comes out 10, the same as A3's "
            "0-after-0 case. This is the deliverable (finding candidate C9), not something to explain away by "
            "loosening the assertion to match what NanoMig does.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
