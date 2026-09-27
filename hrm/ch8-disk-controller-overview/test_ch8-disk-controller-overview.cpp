// Test purpose: HRM conformance package for the p.241 disk-controller
//   overview (plus two p.247 buffering/scheduling claims that build on it):
//   drive count, whole-track DMA in a single revolution (declined),
//   DSKSYNC-gated synchronisation, DSKBYTR's byte-read field, and the
//   CIAAPRA/CIABPRB bit counts and functional categories seen from the
//   floppy side -- against `paula_floppy` (paula_floppy.v +
//   paula_floppy_fifo.v only; paula_floppy_ebr.v excluded, it declares a
//   second `module paula_floppy`). 22 claims, from
//   work/ch8-s2-build/ch8-disk-controller-overview.jsonl. Full derivation in
//   SPECIFICATION.md.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed p.241 (and p.247
//   for the two dskdma claims); full quotations in SPECIFICATION.md.
// Non-goals: anything outside the `paula_floppy` fixture this package
//   builds, and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/ch8-s2-build/ch8-disk-controller-overview.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// RTL READ:
//   1. nanomig/src/minimig-aga/paula_floppy.v -- full file (1041 lines).
//      None of this package's own oracle values comes from it; they come
//      from the claims file's own evidence.
//   2. nanomig/src/minimig-aga/paula_floppy_fifo.v -- full file (60 lines):
//      a straightforward 2048-word-deep circular buffer.
//
// RTL FACTS THIS FILE RELIES ON (paula_floppy.v line numbers):
//   - floppy_drives[1:0] latches into `drives` only at reset (:554-559).
//     _ready (:691-694) reduces, for drives==2'b11 (max, 4 drives
//     configured), to _sel[3]&_sel[2]&_sel[1]&_sel[0] -- deasserted only
//     when ALL FOUR are deselected, i.e. selecting ANY ONE of the four
//     asserts it. Worked by hand for drives==2'b00/01/10 too: the pattern
//     is exactly "drives+1 independently-working _sel bits", confirming
//     floppy_drives is a genuine "how many of the (structurally fixed at)
//     four _sel/sdc_rd/sdc_wr bits are wired live" configuration -- this
//     is P-DRIVE-COUNT's mechanism.
//   - A track's real content is exactly 11 sectors * 544 MFM words = 5984
//     words (:499, :530-538 -- fifo_sector_counter cycles 0..10, each 544
//     words, before the separate 350-word gap that follows sector 11).
//     DSKLEN's length field is 14 bits (max 16383), comfortably more than
//     one track. sdc_rd (:285, :363) is re-issued once per 512-byte SD
//     sector as the internal buffer empties -- eleven separate `sdc_rd`
//     assertions carry one whole track's worth of DMA, all under a SINGLE
//     DSKLEN double-write / single blckint (data_out, dmal, blckint -- the
//     port-level basis sol:8-dskdma.c6's own evidence names).
//   - `index` (:576) = |(~_sel & motor_on) & ~|rpm_pulse_cnt & sof;
//     rpm_pulse_cnt (:563-573) is a 4-bit counter advanced once per
//     clk7_en tick WHERE `sof` is asserted that tick, wrapping at 9 (PAL,
//     ntsc=0) -- i.e. one revolution = 10 sof-qualified clk7_en ticks.
//     `sof` itself is agnus-generated, outside paula_floppy's boundary, so
//     ITS real-world rate cannot be read off this fixture, and this file
//     does not drive `sof`/`index` for any grading purpose.
//
//     NO REAL-TIME COMPARISON: neither the s2 slice (pp.241-250) nor the
//     full 687-page extracted corpus states a rotation speed or clock
//     frequency. This suite's oracle is the manual, not general chipset
//     knowledge (300 RPM = 200 ms/revolution; clk7_en ~= 7.09379 MHz PAL),
//     and a duration compared against a real-time figure the manual never
//     states is an ABSOLUTE reading with no oracle here (the
//     ratio-or-absolute rule: no port emits Hertz or milliseconds). A green
//     assertion resting on an imported number would read as the manual
//     being confirmed when it was not. No clk7_en-tick count is compared
//     against any real-time figure in this file; opus:8-dsk.c7/
//     sol:8-dsk.c7's "in a single disk revolution" is declined with
//     P-WHOLETRACK-DMA below.
//   - dsksync (:701-709) only ever feeds `sync_match` (:914-915,
//     dsksync==floppy_data && fifo_reading_sector && trackrd) -- it is
//     compared against WHATEVER the generator is currently emitting, not
//     substituted into the stream itself. The generator's own fixed sync
//     word is hardcoded 16'h4489 (:485), and its reset default for dsksync
//     (:705) is ALSO 16'h4489 -- so DSKSYNC<=0x4489 matches almost
//     immediately (within the first sector's header preamble), while an
//     arbitrary DSKSYNC (0xDEAD) essentially never appears in the
//     deterministic aaaa/4489/checksum-of-zeros stream this fixture
//     generates, so trackrdok (:920-926) never latches and nothing is ever
//     pushed to the fifo. This is P-DSKSYNC-MATCH's mechanism, and matches
//     the claims file's own MEASURED evidence (opus:8-dsk.c8/
//     sol:8-dsk.c8) exactly.
//   - dskbytr (:699): {1'b1,(trackrd|trackwr),dsklen[14],5'b1_0000,8'h00}
//     at DSKBYTR's own address, else 0. Bits 7:0 (the DATA field
//     opus:8-dsk.c9/sol:8-dsk.c9 grade) are a LITERAL CONSTANT 8'h00 --
//     structurally incapable of ever reflecting disk-stream content, no
//     matter what flows through DSKDATR/the fifo. This is C6, and matches
//     the claims file's own MEASURED evidence (two consecutive $DFF01A
//     reads both returning 0x9000) exactly.
//
// DSKBYTR DATA-FIELD SAMPLING (opus:8-dsk.c9/sol:8-dsk.c9). Two arbitrary
//   DSKBYTR snapshots with no independent ground truth for either cannot
//   show the DATA field -- both reading 0x00 is compatible with a
//   conforming implementation that simply had not caught up yet. So a
//   KNOWN incoming byte (0xFF) is bound to the DSKBYTR sampling instant
//   through the DSKDATR/FIFO, the technique ch8-mfm-cell-encoding uses.
//   THIS IS THE SAME UNDERLYING FACT as ch8-dskbytr-status-register/A9 and
//   ch8-disk-register-map-paula-floppy/B4 -- one fact, graded against a
//   known, bound byte in all three packages that carry it (A9 there finds
//   the payload index by search; B4 and this package use the fixed index).
//   The locator is the fixed, RTL-structural FIRST_PAYLOAD_POP_INDEX (=33)
//   defined below, taken from paula_floppy.v's own word-counter framing
//   thresholds (:484-497), a piece of logic structurally distinct from the
//   per-bit encoder (:467-477) -- so it cannot break when the encoder is
//   fixed. Every trial feeds a uniform 0xFF sector, so any payload word is
//   as good a sample as any other. Searching the drained stream for 0xFFFF
//   would not do: 0xFFFF is what THIS RTL's own non-conformant,
//   context-free MFM encoder produces for a uniform 0xFF sector, while
//   p.250 makes 0xFF page-encode to 0x5555, so against a conforming encoder
//   the search would find nothing and the assertion would report a
//   DATA-field defect that would not be there.
//   The peek at that fixed instant CONFIRMS the sampled word is the known
//   byte's own encoding (`bound_ok = (known_word == 0xFFFF)`); a
//   confirmation failure is a PRECONDITION that DECLINES via record(),
//   never a conjunct that grades -- the shape ch8-dskbytr-status-register/
//   A9 uses. Against the RTL under test here, bound_ok is true and both
//   assertions are RED: known_word=0xFFFF, DSKBYTR=0xD000, DATA
//   field=0x0000.
//   - dmal (:950): for a read (dsklen[14]=0), dmal = dmaon & ~fifo_empty --
//     a real port telling the host exactly when a word is ready to pop.
//     dmaon (:1011, ACTIVE state) = ~lenzero | ~dsklen[14], which for a
//     read is 1 throughout the ACTIVE state regardless of lenzero -- so
//     dmal tracks fifo-non-empty cleanly for the whole transfer. Used here
//     to interleave feeding (on sdc_rd) with draining (on dmal) without
//     guessing timing.
//   - `track` (:669) = {dsktrack[sel], ~side}; `side`'s LSB flips `track`,
//     which feeds `sdc_sector` (:283, `track*11+fd_dma_wr_sec_next`) -- a
//     REAL output port, giving `side` an observable effect without needing
//     to decode the MFM header stream.
//   - `sdc_rd <= ~_sel` (:285, :363) -- whichever drive is selected gets
//     exactly that bit of the 4-bit sdc_rd request, confirming _sel's
//     selection function directly (matches this package's own claims-file
//     evidence: "_sel=0xE selecting drive 0, sdc_rd=1 naming that drive").
//   - dsktrack (:129, :671-680) has NO reset term anywhere in the module.
//     Confirmed by reading the whole file. A FRESH Vpaula_floppy is
//     allocated for every trial below; no instance is reused across two
//     measurements.
//   - step_ena_cnt (:594-603), also no reset term: a 9-bit saturating
//     counter gating every accepted _step edge behind >=256 clk7_en ticks
//     since the last edge (accepted or not). saturate_step_ena() idles 300
//     ticks before every step this file needs accepted.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup -- one build serves the real run (MUTATE unset) and every
//   mutation run (MUTATE=<id>, one id per assertion). Ids are the claim's
//   own `classify.uid` where an assertion grades exactly one claim, and a
//   plain uppercase/P- id for measurements that grade no claim (FIXTURE-LIVE,
//   DSKBYTR-CONTROL, both note()s).
//   opus:8-dsk.c9/sol:8-dsk.c9 are RED at baseline (C6), so per this
//   suite's rule against mutating an already-failing check, their MUTATE
//   run substitutes DSKBYTR-CONTROL's mechanism (the WRITE-bit mirror,
//   proven live elsewhere in this run) through the IDENTICAL
//   data_out-varies comparator instead of a stimulus mutation.
//
// MUTATION records: at the bottom of this file.

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
std::string hex4(uint32_t v) {
    char b[16];
    std::snprintf(b, sizeof b, "%04X", (unsigned)(v & 0xFFFFu));
    return std::string(b);
}

// Register addresses as this fixture's own reg_address_in[8:1] values
// (paula_floppy.v's own parameters, each right-shifted by 1).
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

// RIG HAZARD (SPECIFICATION.md): dsktrack has no reset term
// anywhere in paula_floppy.v. A FRESH Vpaula_floppy is allocated per
// trial; no instance below is reused for a second measurement.
struct Rig {
    Vpaula_floppy* d;
    int clk_cycle = 0;
    long qedges = 0;   // count of qualifying (clk7_en-active) rising edges

    Rig() {
        d = new Vpaula_floppy;
        d->clk = 0; d->clk7_en = 0; d->clk7n_en = 0;
        d->reset = 1; d->ntsc = 0; d->sof = 0; d->enable = 1;
        d->reg_address_in = ADDR_NONE; d->data_in = 0;
        d->_step = 1; d->direc = 0; d->_sel = 0xF; d->side = 0; d->_motor = 1;
        d->wordsync = 0;
        d->floppy_drives = 3;   // 4 drives configured (max), unless a test overrides
        d->floppy_wrprot = 0;
        d->sdc_image_mounted = 0; d->sdc_image_size = 0;
        d->sdc_busy = 0; d->sdc_done = 0;
        d->sdc_byte_in_strobe = 0; d->sdc_byte_addr = 0; d->sdc_byte_in_data = 0;
        d->eval();
    }
    ~Rig() { d->final(); delete d; }
    Rig(const Rig&) = delete;
    Rig& operator=(const Rig&) = delete;

    void tick() {
        bool qualifying = d->clk7_en;
        d->eval();
        d->clk = 1; d->eval();
        d->clk = 0; d->eval();
        if (qualifying) qedges++;
        clk_cycle = (clk_cycle + 1) % 4;
        d->clk7_en  = (clk_cycle == 0);
        d->clk7n_en = (clk_cycle == 2);
    }
    void settle_edges(int n) {
        int seen = 0;
        while (seen < n) {
            bool q = d->clk7_en;
            tick();
            if (q) seen++;
        }
    }
    bool wait_edges(const std::function<bool()>& pred, int max_edges) {
        for (int i = 0; i < max_edges; i++) {
            if (pred()) return true;
            settle_edges(1);
        }
        return pred();
    }

    void do_reset() {
        clk_cycle = 0;
        d->clk = 0; d->clk7_en = 0; d->clk7n_en = 0;
        d->reset = 1;
        settle_edges(8);
        d->reset = 0;
        settle_edges(8);
    }

    void select(int n) { d->_sel = (uint8_t)(0xF & ~(1u << n)); d->eval(); }
    void deselect_all() { d->_sel = 0xF; d->eval(); }

    void write_reg(uint8_t addr, uint16_t data) {
        d->reg_address_in = addr;
        d->data_in = data;
        settle_edges(1);
        d->reg_address_in = ADDR_NONE;
        d->data_in = 0;
        settle_edges(1);
    }
    uint16_t read_reg(uint8_t addr) {
        d->reg_address_in = addr;
        d->eval();
        uint16_t v = (uint16_t)d->data_out;
        d->reg_address_in = ADDR_NONE;
        d->eval();
        return v;
    }
    uint16_t dskdatr_pop() {
        d->reg_address_in = ADDR_DSKDATR;
        d->eval();
        uint16_t v = (uint16_t)d->data_out;
        settle_edges(1);
        d->reg_address_in = ADDR_NONE;
        settle_edges(1);
        return v;
    }
    void arm_dma_read(uint16_t length_words) {
        uint16_t val = (uint16_t)(0x8000u | (length_words & 0x3FFFu));
        write_reg(ADDR_DSKLEN, val);
        write_reg(ADDR_DSKLEN, val);
    }

    // _step_del (paula_floppy.v:588-592) and the dsktrack/step_ena_cnt
    // rising-edge detectors it feeds only update on QUALIFYING (clk7_en)
    // edges, and this Rig's 4-phase tick() makes clk7_en high on only 1 of
    // every 4 raw tick() calls -- so each _step transition below must be
    // pinned to a genuine qualifying edge via settle_edges(1), not a fixed
    // count of raw tick() calls (which can land entirely on non-qualifying
    // phases and silently miss the rising edge).
    void do_step(bool direc_outward) {
        d->direc = direc_outward ? 1 : 0;
        d->_step = 0;
        settle_edges(1);
        d->_step = 1;
        settle_edges(1);
        settle_edges(1);
    }
    void saturate_step_ena() { settle_edges(300); }

    bool track0() { d->eval(); return d->_track0 == 0; }  // active low
    bool ready()  { d->eval(); return d->_ready == 0; }   // active low
    bool wprot()  { d->eval(); return d->_wprot == 0; }   // active low
    bool change() { d->eval(); return d->_change == 0; }  // active low

    void mount(int n, uint32_t size) {
        d->sdc_image_mounted = (uint8_t)(1u << n);
        d->sdc_image_size = size;
        settle_edges(4);
        d->sdc_image_mounted = 0;
    }
    void unmount(int n) {
        d->sdc_image_mounted = (uint8_t)(1u << n);
        d->sdc_image_size = 0;
        settle_edges(4);
        d->sdc_image_mounted = 0;
    }

    // Latch motor_on[n] via a genuine _sel[n] turn-on edge, always starting
    // from fully deselected so the edge is real (paula_floppy.v:624-659).
    void select_with_motor(int n, bool motor_level_on) {
        d->_sel = 0xF;
        settle_edges(4);
        d->_motor = motor_level_on ? 0 : 1;   // active low
        d->_sel = (uint8_t)(0xF & ~(1u << n));
        settle_edges(4);
    }
    // Peek motor_on[n] via `index`, driving `sof` for a single eval() with
    // NO clock edge, so rpm_pulse_cnt/motor_on/_sel_del are untouched by
    // the act of reading (paula_floppy.v:576, :563-573).
    bool peek_index(int n) {
        uint8_t saved_sel = d->_sel;
        d->_sel = (uint8_t)(0xF & ~(1u << n));
        d->sof = 1;
        d->eval();
        bool v = d->index;
        d->sof = 0;
        d->_sel = saved_sel;
        d->eval();
        return v;
    }

    // Stream one full 512-byte SD sector into the read-ingest path
    // (fd_dma_buf_wr_state 0->1->2). Call only once sdc_rd has asserted.
    void feed_sector(const uint8_t* bytes /* 512 */) {
        d->sdc_busy = 1;
        settle_edges(2);
        d->sdc_busy = 0;
        settle_edges(1);
        for (int i = 0; i < 512; i++) {
            d->sdc_byte_addr = i;
            d->sdc_byte_in_data = bytes[i];
            d->sdc_byte_in_strobe = 1;
            settle_edges(1);
            d->sdc_byte_in_strobe = 0;
            settle_edges(1);
        }
    }
};

// opus:8-dsk.c9/sol:8-dsk.c9's payload-word locator (see the file
// header's DSKBYTR DATA-FIELD SAMPLING note for why it is fixed). Payload
// words are fifo_word_counter 32-543; word-counter 0-31 is sync/preamble/
// header/label/checksum framing, fixed by paula_floppy.v's own
// state-machine thresholds (:484-497) and structurally independent of what
// any cell encodes to -- confirmed directly from the RTL, matching the
// reading the already-accepted ch8-mfm-cell-encoding package established
// for this identical fixture. The FIFO preserves push order, so the Nth
// DSKDATR pop is always fifo_word_counter=N-1, and the payload's own first
// word (fifo_word_counter=32) is therefore always pop #33 -- a locator
// taken from the generator's own word-counter thresholds, not from what any
// cell encodes to, so it cannot break when the per-bit encoder
// (paula_floppy.v:467-477) is
// fixed. Every opus:8-dsk.c9/sol:8-dsk.c9 trial feeds a uniform 0xFF
// sector, so this one fixed index is as good a payload sample as any other.
constexpr int FRAMING_WORDS = 32;
constexpr int FIRST_PAYLOAD_POP_INDEX = FRAMING_WORDS + 1;

struct TransferResult { int delivered = 0; bool blckint_seen = false; };

// Interleaves feeding SD sectors (whenever sdc_rd asserts) with draining
// DSKDATR (whenever dmal asserts) until target_words have been delivered,
// blckint fires, or max_qedges qualifying clk7_en ticks have elapsed.
template <typename PatternFn>
TransferResult run_read_transfer(Rig& r, int target_words, PatternFn pattern, int max_qedges) {
    TransferResult res;
    long start_q = r.qedges;
    int sector_counter = 0;
    while (r.qedges - start_q < max_qedges) {
        if (r.d->blckint) { res.blckint_seen = true; break; }
        if (res.delivered >= target_words) break;
        if (r.d->sdc_rd != 0) {
            uint8_t buf[512];
            for (int i = 0; i < 512; i++) buf[i] = pattern(sector_counter, i);
            r.feed_sector(buf);
            sector_counter++;
            continue;
        }
        if (r.d->dmal) {
            r.dskdatr_pop();
            res.delivered++;
            continue;
        }
        r.settle_edges(1);
    }
    return res;
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    std::cout << "Starting ch8-disk-controller-overview...\n";

    // ========================================================================
    // FIXTURE-LIVE -- no claim uid, no manual page: the fixture proves it
    // can deliver a known, distinct word before any read-based assertion
    // below is trusted.
    //
    // A note(), not an expect(): it carries no claim uid -- it is
    // apparatus, never conformance evidence -- and a note() is not graded,
    // so it carries no mutation record.
    // ========================================================================
    {
        Rig r; r.do_reset();
        uint16_t v1 = r.read_reg(ADDR_DSKBYTR);
        r.write_reg(ADDR_DSKLEN, 0x4000);
        uint16_t v2 = r.read_reg(ADDR_DSKBYTR);
        checks.note("FIXTURE-LIVE [not graded -- apparatus check, no claim uid]: DSKLEN<=0x0000 -> DSKBYTR=0x" +
                    hex4(v1) + "; DSKLEN<=0x4000 -> DSKBYTR=0x" + hex4(v2) + " -- two distinct, known DSKLEN "
                    "writes arrive, unmodified and mutually distinguishable, at the DSKBYTR mirror several other "
                    "assertions in this package also read -- the fixture's write-then-read data path is live, "
                    "not stuck");
    }

    // ========================================================================
    // opus:8-dsk.c1 / sol:8-dsk.c1 (p.241, twins) -- P-DRIVE-COUNT.
    // floppy_drives=3 (max): each of the four _sel bits independently
    // asserts _ready when selected alone (paula_floppy.v:691-694).
    // ========================================================================
    {
        Rig r; r.do_reset();
        bool mut = mutating("opus:8-dsk.c1");
        bool ok = true;
        std::string trace;
        for (int n = 0; n < 4; n++) {
            r.deselect_all(); r.settle_edges(2);
            bool desel_ready = r.ready();
            // MUTATION: for drive 3, never actually select it.
            if (!(mut && n == 3)) r.select(n);
            r.settle_edges(2);
            bool sel_ready = r.ready();
            ok = ok && !desel_ready && sel_ready;
            trace += " drive" + std::to_string(n) + "(desel=" + std::to_string(desel_ready) +
                     ",sel=" + std::to_string(sel_ready) + ")";
        }
        checks.note("opus:8-dsk.c1:" + trace);
        checks.expect(ok,
            "opus:8-dsk.c1 (p.241, quote 'The built-in disk controller in the system can handle up to four "
            "MFM-type\\ndevices.'): with floppy_drives configured to its maximum (3, four drives), each of the "
            "four _sel bits independently selects a working drive slot -- paula_floppy.v:691-694 -- grading the "
            "drive COUNT only, not the 'MFM-type' media qualifier (split off per this package's own classify "
            "evidence, carried by claims outside this package)");
    }
    {
        Rig r; r.do_reset();
        bool mut = mutating("sol:8-dsk.c1");
        bool ok = true;
        std::string trace;
        for (int n = 0; n < 4; n++) {
            r.deselect_all(); r.settle_edges(2);
            bool desel_ready = r.ready();
            if (!(mut && n == 3)) r.select(n);
            r.settle_edges(2);
            bool sel_ready = r.ready();
            ok = ok && !desel_ready && sel_ready;
            trace += " drive" + std::to_string(n) + "(desel=" + std::to_string(desel_ready) +
                     ",sel=" + std::to_string(sel_ready) + ")";
        }
        checks.note("sol:8-dsk.c1: twin of opus:8-dsk.c1, same measurement -> " + trace);
        checks.expect(ok,
            "sol:8-dsk.c1 (p.241, quote 'The built-in disk controller in the system can handle up to four "
            "MFM-type\\ndevices.'): twin of opus:8-dsk.c1 (same printed quote, independent reader) -- same "
            "measurement, graded independently under its own uid");
    }

    // ========================================================================
    // P-WHOLETRACK-DMA -- opus:8-dsk.c6 (p.241, capacity half of its own
    // split), opus:8-dsk.c7 (p.241, "in a single disk revolution"),
    // sol:8-dsk.c7 (p.241, both halves of its combined c6+c7 claim),
    // sol:8-dskdma.c6 (p.247, "read from the disk only once for the entire
    // track"). Measured, not graded: a single arming delivers all 5984
    // words of a whole track's real content (11 sectors * 544 MFM words,
    // paula_floppy.v:499,530-538) and raises blckint exactly once, even
    // though the SD-side ingest issues eleven separate sdc_rd requests
    // underneath -- the CPU/DMA-programming side sees ONE operation.
    // ========================================================================
    //
    // DECLINED via record(), for two independent reasons, either alone
    // sufficient. (1) TRACK_WORDS (5984) is an RTL-DERIVED constant
    // (paula_floppy.v:499,530-538 -- 11 sectors * 544 MFM words each), not
    // a figure the manual states anywhere in the s2 slice or the full
    // 687-page corpus (which state neither RPM nor clock frequency either,
    // see the header). Using an RTL-derived number as this assertion's own
    // delivery target would make the RTL under test the reason the
    // assertion expects that value.
    //
    // (2) This file never drives `sof` or observes an index-to-index
    // interval -- this package's own header says so directly ("sof itself
    // is agnus-generated, outside paula_floppy's boundary, so ITS
    // real-world rate cannot be read off this fixture, and this file does
    // not drive sof/index for any grading purpose"). So a transfer of the
    // implementation's own chosen word count, delivered at ANY rate, in ANY
    // number of revolutions, would pass -- it cannot distinguish "an entire
    // track ... in a single disk revolution" (p.241) or "read from the disk
    // only once for the entire track" (p.247, sol:8-dskdma.c6) from "some
    // implementation-defined-length blob moved in one uninterrupted DMA
    // session."
    //
    // What an assertion would need to observe: two consecutive `index`
    // pulses -- this fixture would have to drive `sof` itself, on a real
    // timebase -- bracketing the whole-track transfer, so an implementation
    // that took two revolutions to move the same word count is caught. No
    // package in this suite builds that; nothing in hrm/ grades these four
    // uids (opus:8-dsk.c6, opus:8-dsk.c7, sol:8-dsk.c7, sol:8-dskdma.c6).
    // ========================================================================
    {
        const int TRACK_WORDS = 5984;
        Rig r; r.do_reset();
        r.select(0); r.d->wordsync = 0;
        r.arm_dma_read((uint16_t)TRACK_WORDS);
        auto res = run_read_transfer(r, TRACK_WORDS,
            [](int sector, int i){ return (uint8_t)(sector * 53 + i * 7 + 11); }, 400000);
        checks.note("P-WHOLETRACK-DMA [not graded -- declined, see the record() below]: programmed=" +
                    std::to_string(TRACK_WORDS) + " words, delivered=" + std::to_string(res.delivered) +
                    ", blckint_seen=" + std::to_string(res.blckint_seen));
        checks.record(
            "opus:8-dsk.c6 opus:8-dsk.c7 sol:8-dsk.c7 sol:8-dskdma.c6 P-WHOLETRACK-DMA (p.241, quote 'It can DMA "
            "an entire track of raw\\nMFM data into memory in a single disk revolution.'; p.247, sol:8-dskdma.c6, "
            "quote 'With this process you need to read from the disk only once for the\\nentire track.'): CANNOT "
            "be graded at this boundary. TRACK_WORDS (5984) is derived from paula_floppy.v's own internal "
            "sector-counter structure (11*544), not from any figure the manual states -- an RTL-derived oracle "
            "value, which this suite's rule forbids. Independently, this session never drives `sof`/observes an "
            "index-to-index interval (paula_floppy.v's own revolution counter is explicitly not driven for "
            "grading purposes anywhere in this file), so a transfer of any implementation-chosen word count, "
            "completing in any number of revolutions, still passes -- it cannot establish 'in a single disk "
            "revolution' or 'only once for the entire track' against a page-given figure. A sound assertion "
            "would need to drive `sof` on a real timebase, observe two consecutive `index` pulses, and confirm "
            "the whole-track transfer starts and completes within that one interval -- not merely count words "
            "against a constant this file itself derived from the RTL under test.");
    }

    // ========================================================================
    // opus:8-dsk.c8 / sol:8-dsk.c8 (p.241, twins) -- P-DSKSYNC-MATCH.
    // ========================================================================
    {
        Rig r; r.do_reset();
        r.select(0); r.d->wordsync = 1;
        bool mut = mutating("opus:8-dsk.c8");
        const uint16_t MATCH = 0x4489, MISMATCH = 0xDEAD;
        // MUTATION: program the MATCHING value on the "should not match"
        // probe too, while the check still expects the mismatch outcome.
        r.write_reg(ADDR_DSKSYNC, mut ? MATCH : MISMATCH);
        r.arm_dma_read(16);
        auto res_mismatch = run_read_transfer(r, 16, [](int, int i){ return (uint8_t)(i * 7 + 3); }, 20000);

        Rig r2; r2.do_reset();
        r2.select(0); r2.d->wordsync = 1;
        r2.write_reg(ADDR_DSKSYNC, MATCH);
        r2.arm_dma_read(16);
        auto res_match = run_read_transfer(r2, 16, [](int, int i){ return (uint8_t)(i * 7 + 3); }, 20000);

        checks.note("opus:8-dsk.c8: DSKSYNC=" + hex4(mut ? MATCH : MISMATCH) + " -> delivered=" +
                    std::to_string(res_mismatch.delivered) + ",blckint=" + std::to_string(res_mismatch.blckint_seen) +
                    "; DSKSYNC=0x4489 -> delivered=" + std::to_string(res_match.delivered) + ",blckint=" +
                    std::to_string(res_match.blckint_seen));
        checks.expect(res_mismatch.delivered == 0 && !res_mismatch.blckint_seen &&
                      res_match.delivered == 16 && res_match.blckint_seen,
            "opus:8-dsk.c8 (p.241, quote 'Special registers allow\\nthe CPU to synchronize with specific data, "
            "or read input a byte at a time.'): with wordsync enabled, a DSKSYNC value that never occurs in the "
            "generated stream (0xDEAD) delivers zero words and no blckint, while the value that matches the "
            "generator's own fixed sync word (0x4489, also its reset default, paula_floppy.v:485,705) delivers "
            "the programmed count and raises blckint -- confirming DSKSYNC genuinely gates delivery on a "
            "specific-data match, not merely on wordsync being enabled");
    }
    {
        Rig r; r.do_reset();
        r.select(0); r.d->wordsync = 1;
        bool mut = mutating("sol:8-dsk.c8");
        const uint16_t MATCH = 0x4489, MISMATCH = 0xDEAD;
        r.write_reg(ADDR_DSKSYNC, mut ? MATCH : MISMATCH);
        r.arm_dma_read(16);
        auto res_mismatch = run_read_transfer(r, 16, [](int, int i){ return (uint8_t)(i * 7 + 3); }, 20000);

        Rig r2; r2.do_reset();
        r2.select(0); r2.d->wordsync = 1;
        r2.write_reg(ADDR_DSKSYNC, MATCH);
        r2.arm_dma_read(16);
        auto res_match = run_read_transfer(r2, 16, [](int, int i){ return (uint8_t)(i * 7 + 3); }, 20000);

        checks.note("sol:8-dsk.c8: twin of opus:8-dsk.c8, same measurement -> mismatch delivered=" +
                    std::to_string(res_mismatch.delivered) + ", match delivered=" + std::to_string(res_match.delivered));
        checks.expect(res_mismatch.delivered == 0 && !res_mismatch.blckint_seen &&
                      res_match.delivered == 16 && res_match.blckint_seen,
            "sol:8-dsk.c8 (p.241, quote 'Special registers allow\\nthe CPU to synchronize with specific data, or "
            "read input a byte at a time.'): twin of opus:8-dsk.c8 (same printed quote, independent reader) -- "
            "same measurement, graded independently under its own uid");
    }

    // ========================================================================
    // opus:8-dsk.c9 / sol:8-dsk.c9 (p.241, twins) -- P-DSKBYTR-BYTE-READ.
    //
    // A KNOWN incoming byte (0xFF) is bound to the DSKBYTR sampling instant
    // (see the file header's DSKBYTR DATA-FIELD SAMPLING note). THIS IS THE
    // SAME UNDERLYING FACT as ch8-dskbytr-status-register/A9 and
    // ch8-disk-register-map-paula-floppy/B4 -- one fact, graded against a
    // known, bound byte in all three packages that carry it (each
    // package's own header/SPECIFICATION.md says so).
    //
    // The trial feeds a uniform 0xFF sector, discards pops up to just
    // before the fixed payload-start index (FIRST_PAYLOAD_POP_INDEX, defined
    // above), PEEKS (does not pop) DSKDATR to confirm the known word is
    // present RIGHT NOW, then peeks DSKBYTR -- zero clock edges elapse
    // between the two peeks, so both are read at the SAME instant. Both
    // bytes of 0xFFFF are 0xFF, sidestepping any high/low-byte ordering
    // ambiguity in what "the current byte" means. If the peek does not
    // confirm the known byte's own encoding (bound_ok), the trial DECLINES
    // via record() rather than grading an unconfirmed sample.
    //
    // RED -- paula_floppy.v:699 hardwires bits 7:0 (DATA) to the literal
    // constant 8'h00; a known, bound, nonzero incoming byte never appears
    // there. Finding candidate C6.
    // ========================================================================
    {
        bool mut = mutating("opus:8-dsk.c9");
        int idx = FIRST_PAYLOAD_POP_INDEX;
        checks.note("opus:8-dsk.c9-CALIBRATION: payload's first word (fifo_word_counter=32, "
                    "paula_floppy.v:484-497) is DSKDATR pop #" + std::to_string(idx));
        if (!mut) {
            Rig r; r.do_reset();
            r.select(0); r.d->wordsync = 0;
            r.arm_dma_read(544);
            r.wait_edges([&]{ return r.d->sdc_rd != 0; }, 60);
            uint8_t sector[512];
            std::memset(sector, 0xFF, sizeof sector);
            r.feed_sector(sector);
            for (int i = 1; i < idx; i++) r.dskdatr_pop();  // discard up to just before the fixed payload-start index.
            uint16_t known_word = r.read_reg(ADDR_DSKDATR);  // peek, no pop -- the payload word itself, at this instant.
            uint16_t dskbytr_val = r.read_reg(ADDR_DSKBYTR); // SAME instant: zero clock edges elapsed since the peek above.
            bool bound_ok = (known_word == 0xFFFF);  // confirmation, not a locator.
            uint8_t data_field = (uint8_t)(dskbytr_val & 0x00FF);
            checks.note("opus:8-dsk.c9: known_word=0x" + hex4(known_word) + " (bound=" +
                        std::to_string(bound_ok) + "), DSKBYTR at same instant=0x" + hex4(dskbytr_val) +
                        ", DATA field=0x" + hex4(data_field));
            if (!bound_ok) {
                checks.record(
                    "opus:8-dsk.c9 (p.241, quote 'Special registers allow\\nthe CPU to synchronize with specific "
                    "data, or read input a byte at a time.'): could not confirm a known byte at the fixed "
                    "payload-start sampling instant (pop #" + std::to_string(idx) + ", fifo_word_counter=32) -- "
                    "the word read there was not this fixture's own known encoding of a uniform 0xFF byte "
                    "(0xFFFF). Declining rather than grading an unconfirmed sample: 0xFFFF is not itself a "
                    "page-legal MFM encoding of 0xFF (p.250 makes 0xFF -> 0x5555), so a mismatch here is not "
                    "evidence about the DATA field one way or the other.");
            } else {
                checks.expect(data_field == 0xFF,
                    "opus:8-dsk.c9 (p.241, quote 'Special registers allow\\nthe CPU to synchronize with specific "
                    "data, or read input a byte at a time.'): with a KNOWN incoming byte (0xFF) bound to the "
                    "DSKBYTR sampling instant via the fixed payload-start index (paula_floppy.v:484-497), "
                    "confirmed present by the peek above, DSKBYTR's DATA field should read that byte; "
                    "paula_floppy.v:699 hardwires bits 7:0 to the literal constant 8'h00, so it does not "
                    "(finding candidate C6)");
            }
        } else {
            // Substitute (already red at baseline): DSKBYTR-CONTROL's own
            // mechanism (the WRITE-bit mirror, proven live elsewhere in
            // this run) through the IDENTICAL data_out-varies comparator.
            Rig r; r.do_reset();
            uint16_t v1 = r.read_reg(ADDR_DSKBYTR);
            r.write_reg(ADDR_DSKLEN, 0x4000);
            uint16_t v2 = r.read_reg(ADDR_DSKBYTR);
            checks.note("opus:8-dsk.c9 [substitute]: DSKBYTR before=0x" + hex4(v1) + ", after WRITE-bit set=0x" + hex4(v2));
            checks.expect(v1 != v2,
                "opus:8-dsk.c9 [substitute mechanism, MUTATE run only]: the WRITE-bit mirror (bit 13, proven "
                "live by DSKBYTR-CONTROL) varies through the identical DSKBYTR data_out-varies comparator, in "
                "place of the DATA field the RTL never varies");
        }
    }
    {
        bool mut = mutating("sol:8-dsk.c9");
        int idx = FIRST_PAYLOAD_POP_INDEX;
        checks.note("sol:8-dsk.c9-CALIBRATION: twin of opus:8-dsk.c9, same fixed locator -> payload's first "
                    "word (fifo_word_counter=32, paula_floppy.v:484-497) is DSKDATR pop #" + std::to_string(idx));
        if (!mut) {
            Rig r; r.do_reset();
            r.select(0); r.d->wordsync = 0;
            r.arm_dma_read(544);
            r.wait_edges([&]{ return r.d->sdc_rd != 0; }, 60);
            uint8_t sector[512];
            std::memset(sector, 0xFF, sizeof sector);
            r.feed_sector(sector);
            for (int i = 1; i < idx; i++) r.dskdatr_pop();  // discard up to just before the fixed payload-start index.
            uint16_t known_word = r.read_reg(ADDR_DSKDATR);  // peek, no pop -- the payload word itself, at this instant.
            uint16_t dskbytr_val = r.read_reg(ADDR_DSKBYTR); // SAME instant: zero clock edges elapsed since the peek above.
            bool bound_ok = (known_word == 0xFFFF);  // confirmation, not a locator.
            uint8_t data_field = (uint8_t)(dskbytr_val & 0x00FF);
            checks.note("sol:8-dsk.c9: twin of opus:8-dsk.c9, same measurement -> known_word=0x" +
                        hex4(known_word) + " (bound=" + std::to_string(bound_ok) + ") DSKBYTR=0x" +
                        hex4(dskbytr_val) + " DATA field=0x" + hex4(data_field));
            if (!bound_ok) {
                checks.record(
                    "sol:8-dsk.c9 (p.241, quote 'Special registers allow\\nthe CPU to synchronize with specific "
                    "data, or read input a byte at a time.'): twin of opus:8-dsk.c9 (same printed quote, "
                    "independent reader) -- could not confirm a known byte at the fixed payload-start sampling "
                    "instant (pop #" + std::to_string(idx) + ", fifo_word_counter=32) -- the word read there was "
                    "not this fixture's own known encoding of a uniform 0xFF byte (0xFFFF). Declining rather "
                    "than grading an unconfirmed sample: 0xFFFF is not itself a page-legal MFM encoding of 0xFF "
                    "(p.250 makes 0xFF -> 0x5555), so a mismatch here is not evidence about the DATA field one "
                    "way or the other.");
            } else {
                checks.expect(data_field == 0xFF,
                    "sol:8-dsk.c9 (p.241, quote 'Special registers allow\\nthe CPU to synchronize with specific "
                    "data, or read input a byte at a time.'): twin of opus:8-dsk.c9 (same printed quote, independent "
                    "reader) -- same measurement, graded independently under its own uid, same underlying fact as "
                    "ch8-dskbytr-status-register/A9 and ch8-disk-register-map-paula-floppy/B4, same RED result "
                    "(finding candidate C6)");
            }
        } else {
            Rig r; r.do_reset();
            uint16_t v1 = r.read_reg(ADDR_DSKBYTR);
            r.write_reg(ADDR_DSKLEN, 0x4000);
            uint16_t v2 = r.read_reg(ADDR_DSKBYTR);
            checks.note("sol:8-dsk.c9 [substitute]: same mechanism as opus:8-dsk.c9's substitute -> v1=0x" +
                        hex4(v1) + " v2=0x" + hex4(v2));
            checks.expect(v1 != v2,
                "sol:8-dsk.c9 [substitute mechanism, MUTATE run only]: same substitute as opus:8-dsk.c9, "
                "independent uid");
        }
    }

    // ========================================================================
    // DSKBYTR-CONTROL -- control case, no manual citation, SAME $DFF01A
    // data_out port/read comparator as opus:8-dsk.c9/sol:8-dsk.c9 above:
    // proof the port is live, so their red is a genuine constant-DATA
    // finding, not a dead or stuck read path.
    //
    // A note(), not an expect(): it carries no claim uid -- it is
    // apparatus, never conformance evidence -- and a note() is not graded,
    // so it carries no mutation record. It compares two readings for
    // inequality only, never the WHOLE DSKBYTR word (0x9000/0xB000): p.278
    // marks bits of that word "X Not used" with no stated reset/idle value.
    // ========================================================================
    {
        Rig r; r.do_reset();
        uint16_t v1 = r.read_reg(ADDR_DSKBYTR);
        r.write_reg(ADDR_DSKLEN, 0x4000);
        uint16_t v2 = r.read_reg(ADDR_DSKBYTR);
        checks.note("DSKBYTR-CONTROL [not graded -- apparatus check, no claim uid]: before=0x" + hex4(v1) +
                    ", DSKLEN<=0x4000 -> DSKBYTR=0x" + hex4(v2) + " -- the WRITE-bit mirror (bit 13) varies "
                    "immediately on a DSKLEN write, showing the DSKBYTR read port and this package's comparator "
                    "are live, so the DATA-field red above (opus:8-dsk.c9/sol:8-dsk.c9) is about that field "
                    "specifically, not a stuck port");
    }

    // ========================================================================
    // opus:8-dsk.c21 / sol:8-reglist.c4 (p.241, twins) -- P-CIAAPRA-FOUR-BITS.
    // Four independently-controllable sense signals: _track0, _change,
    // _ready, _wprot -- toggling one condition moves only its own bit.
    // ========================================================================
    // DECLINED via record(): this
    // block only ever reaches paula_floppy's own generic sense ports
    // (_ready/_track0/_wprot/_change); it never touches CIAAPRA itself --
    // that join is one level up, at minimig.v:622, outside this package's
    // assigned fixture boundary (paula_floppy alone). A severed, swapped or
    // wrong-bit join at minimig.v:622 passes this assertion exactly as
    // written. hrm/ch8-cia-disk-signal-bindings carries the fact, at
    // TOP_MODULE=minimig, over a real CPU bus access to CIAAPRA ($BFE001):
    // its PA5-BINDING/PA4-BINDING/PA3-BINDING/PA2-BINDING assertions
    // (opus:8-cia.c6/sol:8-cia.c5, opus:8-cia.c11/sol:8-cia.c9,
    // opus:8-cia.c16/sol:8-cia.c16, opus:8-cia.c18/sol:8-cia.c17) each
    // compare the WHOLE (PA5,PA4,PA3,PA2) nibble read through a real CPU
    // access, catching exactly the swap/sever this block cannot -- and
    // together establish, at the real port, that four independently-
    // controllable disk-sensing bits reach CIAAPRA.
    {
        Rig r; r.do_reset();
        r.mount(0, 1000);
        r.select(0);
        r.do_step(false); r.saturate_step_ena(); r.do_step(true); r.saturate_step_ena();

        bool base_ready = r.ready(), base_track0 = r.track0(), base_wprot = r.wprot(), base_change = r.change();

        r.d->floppy_wrprot = 1; r.d->eval();
        bool t1_wprot = r.wprot();
        bool t1_others = (r.ready() == base_ready) && (r.track0() == base_track0) && (r.change() == base_change);
        r.d->floppy_wrprot = 0; r.d->eval();

        r.do_step(false);
        bool t2_track0 = !r.track0();
        bool t2_others = (r.ready() == base_ready) && (r.wprot() == base_wprot) && (r.change() == base_change);
        r.saturate_step_ena(); r.do_step(true);

        r.unmount(0);
        bool t3_change = r.change();
        bool t3_others = (r.ready() == base_ready) && (r.wprot() == base_wprot) && (r.track0() == base_track0);
        r.mount(0, 1000); r.do_step(false); r.saturate_step_ena(); r.do_step(true);

        r.deselect_all();
        bool t4_ready = !r.ready();

        bool ok = t1_wprot && t1_others && t2_track0 && t2_others && t3_change && t3_others && t4_ready;
        checks.note("opus:8-dsk.c21 [not graded -- declined, see the record() below]: wprot(" + std::to_string(t1_wprot) +
                    "," + std::to_string(t1_others) + ") track0(" + std::to_string(t2_track0) + "," +
                    std::to_string(t2_others) + ") change(" + std::to_string(t3_change) + "," +
                    std::to_string(t3_others) + ") ready(" + std::to_string(t4_ready) + ") -- measured " +
                    std::to_string(ok) + " at paula_floppy's own ports, the property this package's fixture "
                    "boundary can reach");
        checks.record(
            "opus:8-dsk.c21 sol:8-reglist.c4 P-CIAAPRA-FOUR-BITS (p.241, quote '   CIAAPRA    (\\$BFE001)    "
            "four input bits for disk sensing'): CANNOT be graded at this boundary. Four independently-"
            "controllable sense signals reach paula_floppy's own ports, but this package's fixture is "
            "paula_floppy alone -- it never reaches CIAAPRA, so it cannot confirm those four bits actually land "
            "there rather than being severed, swapped, or aliased at the minimig.v:622 join. That join is "
            "graded by hrm/ch8-cia-disk-signal-bindings's PA5/PA4/PA3/PA2-BINDING assertions, over a real CPU "
            "bus access at $BFE001 against the pinned system top -- 15 passed / 0 failed, 15 of 15 mutations "
            "killed.");
    }
    // DECLINED via record(): twin of
    // opus:8-dsk.c21 above -- same fault, the same package carrying the fact.
    {
        Rig r; r.do_reset();
        r.mount(0, 1000);
        r.select(0);
        r.do_step(false); r.saturate_step_ena(); r.do_step(true); r.saturate_step_ena();

        bool base_ready = r.ready(), base_track0 = r.track0(), base_wprot = r.wprot(), base_change = r.change();

        r.d->floppy_wrprot = 1; r.d->eval();
        bool t1_wprot = r.wprot();
        bool t1_others = (r.ready() == base_ready) && (r.track0() == base_track0) && (r.change() == base_change);
        r.d->floppy_wrprot = 0; r.d->eval();

        r.do_step(false);
        bool t2_track0 = !r.track0();
        bool t2_others = (r.ready() == base_ready) && (r.wprot() == base_wprot) && (r.change() == base_change);
        r.saturate_step_ena(); r.do_step(true);

        r.unmount(0);
        bool t3_change = r.change();
        bool t3_others = (r.ready() == base_ready) && (r.wprot() == base_wprot) && (r.track0() == base_track0);
        r.mount(0, 1000); r.do_step(false); r.saturate_step_ena(); r.do_step(true);

        r.deselect_all();
        bool t4_ready = !r.ready();

        bool ok = t1_wprot && t1_others && t2_track0 && t2_others && t3_change && t3_others && t4_ready;
        checks.note("sol:8-reglist.c4 [not graded -- declined, see the record() below]: twin of opus:8-dsk.c21, same "
                    "measurement -> ok=" + std::to_string(ok));
        checks.record(
            "sol:8-reglist.c4 P-CIAAPRA-FOUR-BITS (p.241, quote '   CIAAPRA    (\\$BFE001)    four input bits "
            "for disk sensing'): twin of opus:8-dsk.c21 (same printed quote, independent reader) -- same "
            "boundary fault (this package's fixture is paula_floppy alone; it never reaches CIAAPRA). The same "
            "underlying fact this uid names is carried by hrm/ch8-cia-disk-signal-bindings's "
            "PA5/PA4/PA3/PA2-BINDING assertions (cited there under opus:8-cia.c6/sol:8-cia.c5 et al., not this "
            "uid string -- the fact, not the uid, is carried there).");
    }

    // ========================================================================
    // opus:8-dsk.c22 (p.241) -- P-CIAAPRA-SENSING. Reuses the same four
    // toggles' functional-direction half (each bit fires correctly in
    // response to its OWN real sensing condition), a weaker/different
    // emphasis than c21's cross-independence grading above.
    //
    // DECLINED via record(): same
    // boundary fault as opus:8-dsk.c21/sol:8-reglist.c4 above -- this block
    // never reaches CIAAPRA either, only paula_floppy's own ports, so a
    // severed or wrong-bit minimig.v:622 join passes it too. Each of
    // hrm/ch8-cia-disk-signal-bindings's PA5/PA4/PA3/PA2-BINDING assertions
    // already establishes, through a real CIAAPRA CPU read, that its named
    // bit responds to its own real sensing condition (deselect vs select for
    // PA5/_ready, step-to-zero for PA4/_track0, floppy_wrprot for
    // PA3/_wprot, unmount for PA2/_change) and NOT the other three --
    // functional direction plus cross-isolation together, a strictly
    // stronger form of what this claim needs.
    // ========================================================================
    {
        Rig r; r.do_reset();
        r.mount(0, 1000);
        r.select(0);
        r.do_step(false); r.saturate_step_ena(); r.do_step(true); r.saturate_step_ena();

        r.d->floppy_wrprot = 1; r.d->eval();
        bool wprot_fires = r.wprot();
        r.d->floppy_wrprot = 0; r.d->eval();

        r.do_step(false);
        bool track0_fires = !r.track0();
        r.saturate_step_ena(); r.do_step(true);

        r.unmount(0);
        bool change_fires = r.change();
        r.mount(0, 1000); r.do_step(false); r.saturate_step_ena(); r.do_step(true);

        r.deselect_all();
        bool ready_fires = !r.ready();

        bool ok = wprot_fires && track0_fires && change_fires && ready_fires;
        checks.note("opus:8-dsk.c22 [not graded -- declined, see the record() below]: wprot_fires=" +
                    std::to_string(wprot_fires) + " track0_fires=" + std::to_string(track0_fires) +
                    " change_fires=" + std::to_string(change_fires) + " ready_fires=" + std::to_string(ready_fires));
        checks.record(
            "opus:8-dsk.c22 P-CIAAPRA-SENSING (p.241, quote '   CIAAPRA    (\\$BFE001)    four input bits for "
            "disk sensing'): CANNOT be graded at this boundary -- same fault as opus:8-dsk.c21/sol:8-reglist.c4 "
            "above (paula_floppy-only fixture, never reaches CIAAPRA). hrm/ch8-cia-disk-signal-bindings's "
            "PA5/PA4/PA3/PA2-BINDING assertions (opus:8-cia.c6/sol:8-cia.c5, opus:8-cia.c11/sol:8-cia.c9, "
            "opus:8-cia.c16/sol:8-cia.c16, opus:8-cia.c18/sol:8-cia.c17) carry the fact, each confirming its "
            "own bit responds to its own real sensing condition through a genuine CPU read of CIAAPRA.");
    }

    // ========================================================================
    // opus:8-dsk.c24 / sol:8-reglist.c6 (p.241, twins) -- P-CIABPRB-EIGHT-BITS.
    // Eight independently-drivable control inputs reach paula_floppy from
    // the CIABPRB side: _sel[3:0] (4), _step, direc, side, _motor.
    //
    // DECLINED via record(): same fault
    // family as the CIAAPRA blocks above -- this block never reaches
    // CIABPRB, only paula_floppy's own generic ports; the minimig.v:650
    // join is outside this package's fixture boundary and a severed/
    // swapped/wrong-bit join there passes this assertion unchanged.
    // hrm/ch8-cia-disk-signal-bindings's eight PB7..PB0-BINDING assertions
    // carry the fact, each writing a real byte through a real CPU bus
    // access to CIABPRB ($BFD100) and requiring the target join wire low
    // AND the other seven high -- catching exactly the swap this block
    // cannot, and together establishing the COUNT at the real port.
    // ========================================================================
    {
        bool sel_ok = true;
        {
            Rig r; r.do_reset();
            for (int n = 0; n < 4; n++) {
                r.deselect_all(); r.settle_edges(2);
                bool desel = r.ready();
                r.select(n);
                r.settle_edges(2);
                bool sel = r.ready();
                sel_ok = sel_ok && !desel && sel;
            }
        }
        bool step_direc_ok;
        {
            Rig r; r.do_reset(); r.select(0); r.saturate_step_ena();
            bool at_zero = r.track0();
            r.do_step(false);
            bool left = !r.track0();
            r.saturate_step_ena(); r.do_step(true);
            bool back = r.track0();
            step_direc_ok = at_zero && left && back;
        }
        uint32_t sec_side0, sec_side1;
        {
            Rig r; r.do_reset(); r.select(0); r.d->side = 0;
            r.arm_dma_read(8);
            r.wait_edges([&]{ return r.d->sdc_rd != 0; }, 60);
            sec_side0 = r.d->sdc_sector;
        }
        {
            Rig r; r.do_reset(); r.select(0); r.d->side = 1;
            r.arm_dma_read(8);
            r.wait_edges([&]{ return r.d->sdc_rd != 0; }, 60);
            sec_side1 = r.d->sdc_sector;
        }
        bool side_ok = (sec_side0 != sec_side1);
        bool motor_ok;
        {
            Rig r; r.do_reset(); r.select_with_motor(0, true);
            bool on_reading = r.peek_index(0);
            Rig r2; r2.do_reset(); r2.select_with_motor(0, false);
            bool off_reading = r2.peek_index(0);
            motor_ok = on_reading && !off_reading;
        }
        bool ok = sel_ok && step_direc_ok && side_ok && motor_ok;
        checks.note("opus:8-dsk.c24 [not graded -- declined, see the record() below]: sel_ok=" + std::to_string(sel_ok) +
                    " step_direc_ok=" + std::to_string(step_direc_ok) + " side_ok=" + std::to_string(side_ok) +
                    " (sector " + std::to_string(sec_side0) + " vs " + std::to_string(sec_side1) + ") motor_ok=" +
                    std::to_string(motor_ok));
        checks.record(
            "opus:8-dsk.c24 sol:8-reglist.c6 P-CIABPRB-EIGHT-BITS (p.241, quote '   CIABPRB    (\\$BFD100)    "
            "eight output bits for disk selection, control\\n                             and stepping'): CANNOT "
            "be graded at this boundary. Eight independently-drivable control inputs reach paula_floppy's own "
            "ports, but this package's fixture never reaches CIABPRB itself (minimig.v:650, outside paula_floppy "
            "alone), so it cannot confirm those eight bits actually land there. hrm/ch8-cia-disk-signal-bindings's "
            "PB7-BINDING through PB0-BINDING assertions carry the fact, over a real CPU bus write to "
            "$BFD100 against the pinned system top -- 15 passed / 0 failed, 15 of 15 mutations killed.");
    }
    // DECLINED via record(): twin of
    // opus:8-dsk.c24 above -- same fault, the same package carrying the fact.
    {
        bool sel_ok = true;
        {
            Rig r; r.do_reset();
            for (int n = 0; n < 4; n++) {
                r.deselect_all(); r.settle_edges(2);
                bool desel = r.ready();
                r.select(n);
                r.settle_edges(2);
                bool sel = r.ready();
                sel_ok = sel_ok && !desel && sel;
            }
        }
        bool step_direc_ok;
        {
            Rig r; r.do_reset(); r.select(0); r.saturate_step_ena();
            bool at_zero = r.track0();
            r.do_step(false);
            bool left = !r.track0();
            r.saturate_step_ena(); r.do_step(true);
            bool back = r.track0();
            step_direc_ok = at_zero && left && back;
        }
        uint32_t sec_side0, sec_side1;
        {
            Rig r; r.do_reset(); r.select(0); r.d->side = 0;
            r.arm_dma_read(8);
            r.wait_edges([&]{ return r.d->sdc_rd != 0; }, 60);
            sec_side0 = r.d->sdc_sector;
        }
        {
            Rig r; r.do_reset(); r.select(0); r.d->side = 1;
            r.arm_dma_read(8);
            r.wait_edges([&]{ return r.d->sdc_rd != 0; }, 60);
            sec_side1 = r.d->sdc_sector;
        }
        bool side_ok = (sec_side0 != sec_side1);
        bool motor_ok;
        {
            Rig r; r.do_reset(); r.select_with_motor(0, true);
            bool on_reading = r.peek_index(0);
            Rig r2; r2.do_reset(); r2.select_with_motor(0, false);
            bool off_reading = r2.peek_index(0);
            motor_ok = on_reading && !off_reading;
        }
        bool ok = sel_ok && step_direc_ok && side_ok && motor_ok;
        checks.note("sol:8-reglist.c6 [not graded -- declined, see the record() below]: twin of opus:8-dsk.c24, same "
                    "measurement -> ok=" + std::to_string(ok));
        checks.record(
            "sol:8-reglist.c6 P-CIABPRB-EIGHT-BITS (p.241, quote '   CIABPRB    (\\$BFD100)    eight output "
            "bits'): twin of opus:8-dsk.c24 (same printed quote, independent reader) -- same boundary fault. The "
            "same underlying fact this uid names is carried by hrm/ch8-cia-disk-signal-bindings's "
            "PB7-BINDING through PB0-BINDING assertions (cited there under their own opus:8-cia.*/sol:8-cia.* "
            "uids, not this uid string -- the fact, not the uid, is carried there).");
    }

    // ========================================================================
    // opus:8-dsk.c25 / sol:8-reglist.c7 (p.241, twins) -- P-CIABPRB-SELECTION.
    // _sel[3:0] targets a specific drive for DMA (paula_floppy.v:285,363,
    // sdc_rd<=~_sel).
    //
    // DECLINED via record(): same fault
    // family -- sdc_rd is paula_floppy's own internal request line, never
    // CIABPRB, so this cannot confirm the selection bits (PB6../DSKSEL3*..
    // PB3/DSKSEL0*) actually land at their named CIABPRB positions.
    // hrm/ch8-cia-disk-signal-bindings's PB6/PB5/PB4/PB3-BINDING assertions
    // (the four _sel3.._sel0 bindings) carry the selection half of this
    // fact, over a real CPU bus write to $BFD100.
    // ========================================================================
    {
        Rig r; r.do_reset(); r.select(0);
        r.arm_dma_read(8);
        r.wait_edges([&]{ return r.d->sdc_rd != 0; }, 60);
        uint8_t rd0 = r.d->sdc_rd;

        Rig r2; r2.do_reset();
        int target = 2;
        r2.select(target);
        r2.arm_dma_read(8);
        r2.wait_edges([&]{ return r2.d->sdc_rd != 0; }, 60);
        uint8_t rd2 = r2.d->sdc_rd;

        checks.note("opus:8-dsk.c25 [not graded -- declined, see the record() below]: drive0 -> sdc_rd=" + hex4(rd0) +
                    "; target drive " + std::to_string(target) + " -> sdc_rd=" + hex4(rd2));
        checks.record(
            "opus:8-dsk.c25 sol:8-reglist.c7 P-CIABPRB-SELECTION (p.241, quote '   CIABPRB    (\\$BFD100)    "
            "eight output bits for disk selection, control\\n                             and stepping'): CANNOT "
            "be graded at this boundary. _sel targets a specific drive at paula_floppy's own sdc_rd request "
            "line, but this package's fixture never reaches CIABPRB itself, so it cannot confirm the selection "
            "bits land at their named CIABPRB positions (PB6/DSKSEL3* .. PB3/DSKSEL0*). "
            "hrm/ch8-cia-disk-signal-bindings's PB6-BINDING/PB5-BINDING/PB4-BINDING/PB3-BINDING assertions "
            "carry the fact, over a real CPU bus write to $BFD100 against the pinned system top.");
    }
    // DECLINED via record(): twin of
    // opus:8-dsk.c25 above -- same fault, the same package carrying the fact.
    {
        Rig r; r.do_reset(); r.select(0);
        r.arm_dma_read(8);
        r.wait_edges([&]{ return r.d->sdc_rd != 0; }, 60);
        uint8_t rd0 = r.d->sdc_rd;

        Rig r2; r2.do_reset();
        int target = 2;
        r2.select(target);
        r2.arm_dma_read(8);
        r2.wait_edges([&]{ return r2.d->sdc_rd != 0; }, 60);
        uint8_t rd2 = r2.d->sdc_rd;

        checks.note("sol:8-reglist.c7 [not graded -- declined, see the record() below]: twin of opus:8-dsk.c25, same "
                    "measurement -> rd0=" + hex4(rd0) + " rd(target)=" + hex4(rd2));
        checks.record(
            "sol:8-reglist.c7 P-CIABPRB-SELECTION (p.241, quote '   CIABPRB    (\\$BFD100)    eight output "
            "bits'): twin of opus:8-dsk.c25 (same printed quote, independent reader) -- same boundary fault. The "
            "same underlying fact this uid names is carried by hrm/ch8-cia-disk-signal-bindings's "
            "PB6/PB5/PB4/PB3-BINDING assertions (cited there under their own opus:8-cia.*/sol:8-cia.* uids, not "
            "this uid string -- the fact, not the uid, is carried there).");
    }

    // ========================================================================
    // opus:8-dsk.c26 / sol:8-reglist.c8 (p.241, twins) -- P-CIABPRB-CONTROL.
    // _motor latches motor_on per drive; side selects the head surface
    // (paula_floppy.v:624-659, :669).
    //
    // DECLINED via record(): same fault
    // family -- motor_on/index and the requested sdc_sector base are
    // paula_floppy's own internal signals, never CIABPRB, so this cannot
    // confirm PB7/DSKMOTOR* and PB2/DSKSIDE land at their named CIABPRB
    // positions. hrm/ch8-cia-disk-signal-bindings's PB7-BINDING and
    // PB2-BINDING assertions carry this fact, over a real CPU bus write
    // to $BFD100.
    // ========================================================================
    {
        bool motor_ok;
        {
            Rig r; r.do_reset(); r.select_with_motor(0, true);
            bool on_reading = r.peek_index(0);
            Rig r2; r2.do_reset();
            r2.select_with_motor(0, false);
            bool off_reading = r2.peek_index(0);
            motor_ok = on_reading && !off_reading;
        }
        uint32_t sec0, sec1;
        {
            Rig r; r.do_reset(); r.select(0); r.d->side = 0;
            r.arm_dma_read(8);
            r.wait_edges([&]{ return r.d->sdc_rd != 0; }, 60);
            sec0 = r.d->sdc_sector;
        }
        {
            Rig r; r.do_reset(); r.select(0); r.d->side = 1;
            r.arm_dma_read(8);
            r.wait_edges([&]{ return r.d->sdc_rd != 0; }, 60);
            sec1 = r.d->sdc_sector;
        }
        bool side_ok = (sec0 != sec1);
        checks.note("opus:8-dsk.c26 [not graded -- declined, see the record() below]: motor_ok=" + std::to_string(motor_ok) +
                    " side_ok=" + std::to_string(side_ok) + " (sector " + std::to_string(sec0) + " vs " +
                    std::to_string(sec1) + ")");
        checks.record(
            "opus:8-dsk.c26 sol:8-reglist.c8 P-CIABPRB-CONTROL (p.241, quote '   CIABPRB    (\\$BFD100)    eight "
            "output bits for disk selection, control\\n                             and stepping'): CANNOT be "
            "graded at this boundary. _motor and side are real, effective inputs at paula_floppy's own ports, "
            "but this package's fixture never reaches CIABPRB itself, so it cannot confirm PB7/DSKMOTOR* and "
            "PB2/DSKSIDE land there. hrm/ch8-cia-disk-signal-bindings's PB7-BINDING and PB2-BINDING assertions "
            "carry the fact, over a real CPU bus write to $BFD100 against the pinned system top.");
    }
    // DECLINED via record(): twin of
    // opus:8-dsk.c26 above -- same fault, the same package carrying the fact.
    {
        bool motor_ok;
        {
            Rig r; r.do_reset(); r.select_with_motor(0, true);
            bool on_reading = r.peek_index(0);
            Rig r2; r2.do_reset();
            r2.select_with_motor(0, false);
            bool off_reading = r2.peek_index(0);
            motor_ok = on_reading && !off_reading;
        }
        uint32_t sec0, sec1;
        {
            Rig r; r.do_reset(); r.select(0); r.d->side = 0;
            r.arm_dma_read(8);
            r.wait_edges([&]{ return r.d->sdc_rd != 0; }, 60);
            sec0 = r.d->sdc_sector;
        }
        {
            Rig r; r.do_reset(); r.select(0); r.d->side = 1;
            r.arm_dma_read(8);
            r.wait_edges([&]{ return r.d->sdc_rd != 0; }, 60);
            sec1 = r.d->sdc_sector;
        }
        bool side_ok = (sec0 != sec1);
        checks.note("sol:8-reglist.c8 [not graded -- declined, see the record() below]: twin of opus:8-dsk.c26, same "
                    "measurement -> motor_ok=" + std::to_string(motor_ok) + " side_ok=" + std::to_string(side_ok));
        checks.record(
            "sol:8-reglist.c8 P-CIABPRB-CONTROL (p.241, quote '   CIABPRB    (\\$BFD100)    eight output "
            "bits'): twin of opus:8-dsk.c26 (same printed quote, independent reader) -- same boundary fault. The "
            "same underlying fact this uid names is carried by hrm/ch8-cia-disk-signal-bindings's "
            "PB7-BINDING and PB2-BINDING assertions (cited there under their own opus:8-cia.*/sol:8-cia.* uids, "
            "not this uid string -- the fact, not the uid, is carried there).");
    }

    // ========================================================================
    // opus:8-dsk.c27 / sol:8-reglist.c9 (p.241, twins) -- P-CIABPRB-STEPPING.
    //
    // DECLINED via record(): same fault
    // family -- _step/direc and dsktrack are paula_floppy's own internal
    // signals, never CIABPRB, so this cannot confirm PB1/DSKDIREC and
    // PB0/DSKSTEP* land at their named CIABPRB positions.
    // hrm/ch8-cia-disk-signal-bindings's PB1-BINDING and PB0-BINDING
    // assertions carry this fact, over a real CPU bus write to $BFD100.
    // ========================================================================
    {
        Rig r; r.do_reset();
        r.select(0); r.saturate_step_ena();
        bool at_zero = r.track0();
        r.do_step(false);
        bool left_zero = !r.track0();
        r.saturate_step_ena();
        r.do_step(true);
        bool returned = r.track0();
        checks.note("opus:8-dsk.c27 [not graded -- declined, see the record() below]: at_zero=" + std::to_string(at_zero) +
                    " left_zero=" + std::to_string(left_zero) + " returned=" + std::to_string(returned));
        checks.record(
            "opus:8-dsk.c27 sol:8-reglist.c9 P-CIABPRB-STEPPING (p.241, quote '   CIABPRB    (\\$BFD100)    "
            "eight output bits for disk selection, control\\n                             and stepping'): CANNOT "
            "be graded at this boundary. _step/direc move the selected drive's track counter, a real function at "
            "paula_floppy's own ports, but this package's fixture never reaches CIABPRB itself, so it cannot "
            "confirm PB1/DSKDIREC and PB0/DSKSTEP* land there. hrm/ch8-cia-disk-signal-bindings's PB1-BINDING "
            "and PB0-BINDING assertions carry the fact, over a real CPU bus write to $BFD100 against the "
            "pinned system top.");
    }
    // DECLINED via record(): twin of
    // opus:8-dsk.c27 above -- same fault, the same package carrying the fact.
    {
        Rig r; r.do_reset();
        r.select(0); r.saturate_step_ena();
        bool at_zero = r.track0();
        r.do_step(false);
        bool left_zero = !r.track0();
        r.saturate_step_ena();
        r.do_step(true);
        bool returned = r.track0();
        checks.note("sol:8-reglist.c9 [not graded -- declined, see the record() below]: twin of opus:8-dsk.c27, same "
                    "measurement -> at_zero=" + std::to_string(at_zero) + " left_zero=" + std::to_string(left_zero) +
                    " returned=" + std::to_string(returned));
        checks.record(
            "sol:8-reglist.c9 P-CIABPRB-STEPPING (p.241, quote '   CIABPRB    (\\$BFD100)    eight output "
            "bits'): twin of opus:8-dsk.c27 (same printed quote, independent reader) -- same boundary fault. The "
            "same underlying fact this uid names is carried by hrm/ch8-cia-disk-signal-bindings's "
            "PB1-BINDING and PB0-BINDING assertions (cited there under their own opus:8-cia.*/sol:8-cia.* uids, "
            "not this uid string -- the fact, not the uid, is carried there).");
    }

    // ========================================================================
    // sol:8-dskdma.c7 (p.247) -- P-STEP-DURING-DMA. Head stepping is
    // accepted while a DMA transfer is active, not gated on it being idle.
    // ========================================================================
    {
        Rig r; r.do_reset();
        bool mut = mutating("sol:8-dskdma.c7");
        r.select(0);
        r.saturate_step_ena();
        // MUTATION: never arm DMA at all -- fdd_led stays 0 throughout, so
        // the "stays active while stepping" half is checked against a
        // transfer that was never running.
        if (!mut) {
            r.arm_dma_read(40);
            r.wait_edges([&]{ return r.d->sdc_rd != 0; }, 60);
            uint8_t buf[512]; for (int i = 0; i < 512; i++) buf[i] = (uint8_t)i;
            r.feed_sector(buf);
            r.wait_edges([&]{ return r.d->fdd_led; }, 60);
        }
        bool active_before = r.d->fdd_led;
        bool at_zero = r.track0();
        r.do_step(false);
        bool left_zero = !r.track0();
        bool active_during = r.d->fdd_led;
        r.saturate_step_ena();
        r.do_step(true);
        bool back = r.track0();
        bool active_after = r.d->fdd_led;
        checks.note("sol:8-dskdma.c7: active_before=" + std::to_string(active_before) + " at_zero=" +
                    std::to_string(at_zero) + " left_zero=" + std::to_string(left_zero) + " active_during=" +
                    std::to_string(active_during) + " back=" + std::to_string(back) + " active_after=" +
                    std::to_string(active_after));
        checks.expect(active_before && at_zero && left_zero && active_during && back && active_after,
            "sol:8-dskdma.c7 (p.247, quote 'In a high speed loader, the step to the next head can occur\\nwhile "
            "the previous track is processed and checksummed.'): with disk DMA armed and active (fdd_led=1), "
            "step pulses move the track counter away from and back to track 0 while fdd_led stays 1 throughout "
            "-- stepping is accepted independently of the DMA state machine (paula_floppy.v:671-680 has no "
            "dependency on dskstate/dmaon), confirming the drive can step while a transfer is in progress");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}

// MUTATION opus:8-dsk.c1 stimulus red: drive 3 is never actually selected in the loop, so its "selected -> ready" half is checked against a state that never occurred (MUTATE=opus:8-dsk.c1).
// MUTATION sol:8-dsk.c1 stimulus red: same omission as opus:8-dsk.c1, independent uid (MUTATE=sol:8-dsk.c1).
// MUTATION opus:8-dsk.c8 stimulus red: the matching DSKSYNC value (0x4489) is used on the "should not match" probe too, while the check still expects the mismatch outcome (zero delivered) (MUTATE=opus:8-dsk.c8).
// MUTATION sol:8-dsk.c8 stimulus red: same substitution as opus:8-dsk.c8, independent uid (MUTATE=sol:8-dsk.c8).
// MUTATION opus:8-dsk.c9 stimulus green: this assertion is RED at baseline (C6), so per the rule against mutating an already-failing check, the stimulus substitutes DSKBYTR-CONTROL's WRITE-bit mechanism -- proven live elsewhere in this run -- through the identical data_out-varies comparator in place of the DATA field the RTL never varies (MUTATE=opus:8-dsk.c9).
// MUTATION sol:8-dsk.c9 stimulus green: same substitute mechanism as opus:8-dsk.c9, independent uid (MUTATE=sol:8-dsk.c9).
// MUTATION sol:8-dskdma.c7 stimulus red: DMA is never armed at all, so fdd_led stays 0 throughout and the "stays active while stepping" half of this claim is checked against a transfer that was never running (MUTATE=sol:8-dskdma.c7).
