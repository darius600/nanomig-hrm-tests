// Test purpose: HRM conformance package for the p.241 disk-controller
//   overview (plus two p.247 buffering/scheduling claims that build on it):
//   drive count, whole-track DMA capacity and its revolution-ratio,
//   DSKSYNC-gated synchronisation, DSKBYTR's byte-read field, and the
//   CIAAPRA/CIABPRB bit counts and functional categories seen from the
//   floppy side -- against `paula_floppy` (paula_floppy.v +
//   paula_floppy_fifo.v only; paula_floppy_ebr.v excluded, it declares a
//   second `module paula_floppy`). 22 claims, from
//   work/ch8-s2-build/ch8-disk-controller-overview.jsonl. Full derivation
//   in SPECIFICATION.md (committed as d8666c6, before this file or any RTL
//   was opened).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed p.241 (and
//   p.247 for the two dskdma claims); full quotations in SPECIFICATION.md.
//
// RTL READ, IN ORDER, ALL AFTER SPECIFICATION.md WAS COMMITTED (d8666c6):
//   1. nanomig/src/minimig-aga/paula_floppy.v -- full file (1041 lines),
//      read myself for the first time at this step (SPECIFICATION.md's own
//      process note discloses that I had already seen RTL-mechanism
//      comments inside three sibling packages' built .cpp files before
//      writing the spec -- register-address convention and the DSKLEN
//      double-write shape only, not any of THIS package's own oracle
//      values, which all come from my claims file's own evidence).
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
//     CORRECTION, recorded rather than silently fixed: an earlier revision
//     of this file graded a separate P-REVOLUTION-RATIO assertion by
//     comparing the whole-track transfer's measured clk7_en-tick duration
//     against an imported real-time constant (300 RPM = 200 ms/revolution;
//     clk7_en ~= 7.09379 MHz PAL, 2x the PAL colour clock -- ordinary,
//     well-known Amiga chipset facts). The coordinator checked both the s2
//     slice (pp.241-250) and the full 687-page extracted corpus for a
//     stated rotation speed or clock frequency: absent from both. This
//     suite's oracle is the manual, not general chipset knowledge, and a
//     duration compared against a real-time figure the manual never
//     states is an ABSOLUTE reading with no oracle here (the classify
//     brief's ratio-or-absolute rule: no port emits Hertz or milliseconds).
//     A green assertion resting on an imported number reads as the manual
//     being confirmed when it was not. P-REVOLUTION-RATIO is REMOVED, not
//     reworded to a smaller number -- see P-WHOLETRACK-DMA below, which now
//     also carries opus:8-dsk.c7/sol:8-dsk.c7's DIMENSIONLESS reading
//     (single continuous pass covers a whole track, 1:1, no real-time
//     calibration needed) in its place.
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
//     my own claims file's own MEASURED evidence (opus:8-dsk.c8/
//     sol:8-dsk.c8) exactly.
//   - dskbytr (:699): {1'b1,(trackrd|trackwr),dsklen[14],5'b1_0000,8'h00}
//     at DSKBYTR's own address, else 0. Bits 7:0 (the DATA field
//     opus:8-dsk.c9/sol:8-dsk.c9 grade) are a LITERAL CONSTANT 8'h00 --
//     structurally incapable of ever reflecting disk-stream content, no
//     matter what flows through DSKDATR/the fifo. This is C6 (finding
//     candidates doc), and matches my claims file's own MEASURED evidence
//     (two consecutive $DFF01A reads both returning 0x9000) exactly.
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
//   plain uppercase/P- id where an assertion grades several claims'
//   shared content at once (P-WHOLETRACK-DMA) or grades no claim at all
//   (FIXTURE-LIVE, DSKBYTR-CONTROL).
//   opus:8-dsk.c9/sol:8-dsk.c9 are RED at baseline (C6), so per this
//   suite's rule against mutating an already-failing check, their MUTATE
//   run substitutes DSKBYTR-CONTROL's mechanism (the WRITE-bit mirror,
//   proven live elsewhere in this run) through the IDENTICAL
//   data_out-varies comparator instead of a stimulus mutation.
//
// MUTATION RUN LOG: recorded in the commit message and build report after
//   the run, not predicted here.

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

// RIG HAZARD (SPECIFICATION.md, build brief): dsktrack has no reset term
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
    // FIXTURE-LIVE -- no claim id, no manual page: the build brief's own
    // requirement that the fixture prove it can deliver a known, distinct
    // word before any read-based assertion below is trusted.
    // ========================================================================
    {
        Rig r; r.do_reset();
        uint16_t v1 = r.read_reg(ADDR_DSKBYTR);
        bool mut = mutating("FIXTURE-LIVE");
        uint16_t word2 = mut ? 0x0000 : 0x4000;
        r.write_reg(ADDR_DSKLEN, word2);
        uint16_t v2 = r.read_reg(ADDR_DSKBYTR);
        checks.note("FIXTURE-LIVE: DSKLEN<=0x0000 -> DSKBYTR=0x" + hex4(v1) +
                    "; DSKLEN<=0x" + hex4(word2) + " -> DSKBYTR=0x" + hex4(v2));
        checks.expect(v1 == 0x9000 && v2 == (mut ? 0x9000 : 0xB000) && v1 != v2,
            "FIXTURE-LIVE: two distinct, known DSKLEN writes arrive, unmodified and mutually distinguishable, "
            "at the DSKBYTR mirror this package's own DSKBYTR-CONTROL and several other assertions also read -- "
            "the fixture's write-then-read data path is live, not stuck");
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
    // split), opus:8-dsk.c7 (p.241, dimensionless single-pass reading --
    // see note below), sol:8-dsk.c7 (p.241, both halves of its combined
    // c6+c7 claim), sol:8-dskdma.c6 (p.247, "read from the disk only once
    // for the entire track", graded on sol's own operative reading: one
    // DSKLEN double-write / one blckint covers a whole track). A single
    // arming delivers all 5984 words of a whole track's real content (11
    // sectors * 544 MFM words, paula_floppy.v:499,530-538) and raises
    // blckint exactly once, even though the SD-side ingest issues eleven
    // separate sdc_rd requests underneath -- the CPU/DMA-programming side
    // sees ONE operation.
    //
    // CORRECTED after this package's own build report was checked against
    // the manual corpus (RPM/clock frequency absent from both the s2 slice
    // and the whole 687-page corpus): this assertion previously ALSO
    // graded a separate P-REVOLUTION-RATIO check comparing this same
    // transfer's measured clk7_en-tick duration against an externally
    // supplied real-time constant (300 RPM, ~7.09379 MHz PAL clk7_en) --
    // neither number is stated anywhere in the manual, so that duration
    // comparison had no oracle in this suite and has been REMOVED, not
    // reworded. What survives, folded into THIS assertion instead of a
    // separate one, is the part of "in a single disk revolution" that
    // needs no real-time calibration at all: a whole track's worth of
    // data (5984 words, matching the track size 1:1) is delivered in ONE
    // continuous DMA session -- one arm, one blckint -- never needing a
    // second pass/re-arm to cover the same ground. That is exactly what
    // this assertion already measured for the capacity half; opus:8-dsk.c7
    // and sol:8-dsk.c7's ratio content is cited here under that SAME
    // single-pass measurement, and it grades the single-pass property
    // only, not duration -- no clk7_en-tick count is compared against any
    // real-time figure anywhere in this file.
    // ========================================================================
    {
        const int TRACK_WORDS = 5984;
        Rig r; r.do_reset();
        r.select(0); r.d->wordsync = 0;
        bool mut = mutating("P-WHOLETRACK-DMA");
        // MUTATION: request one word MORE than a whole track holds, while
        // the check still expects completion at exactly TRACK_WORDS.
        int programmed = mut ? TRACK_WORDS + 1 : TRACK_WORDS;
        r.arm_dma_read((uint16_t)programmed);
        auto res = run_read_transfer(r, programmed,
            [](int sector, int i){ return (uint8_t)(sector * 53 + i * 7 + 11); }, 400000);
        checks.note("P-WHOLETRACK-DMA: programmed=" + std::to_string(programmed) +
                    " words, delivered=" + std::to_string(res.delivered) +
                    ", blckint_seen=" + std::to_string(res.blckint_seen));
        checks.expect(res.delivered == TRACK_WORDS && res.blckint_seen,
            "P-WHOLETRACK-DMA (opus:8-dsk.c6, p.241, quote 'It can DMA an entire track of raw\\nMFM data into "
            "memory in a single disk revolution.'; opus:8-dsk.c7, p.241, same quote, graded here ONLY on its "
            "dimensionless single-pass reading -- data delivered to track size, 1:1, in one uninterrupted DMA "
            "session -- NOT on any duration/real-time reading, for which the manual supplies no rotation-speed "
            "or clock-frequency figure anywhere in the corpus; sol:8-dsk.c7, p.241, same quote, both halves of "
            "its combined claim, same dimensionless reading; sol:8-dskdma.c6, p.247, quote 'With this process "
            "you need to read from the disk only once for the\\nentire track.', graded on sol's own operative "
            "reading -- one programming, one blckint): a single DSKLEN double-write delivers all 5984 words of "
            "a whole track's real content (11 sectors * 544 MFM words, paula_floppy.v:499,530-538) and raises "
            "blckint exactly once, confirming a whole track is covered in ONE continuous DMA pass from the "
            "programming side, never a second pass over the same ground");
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
    // EXPECTED RED (C6): DSKBYTR's DATA field (bits 7:0) is a literal
    // constant in the RTL (paula_floppy.v:699, "...8'h00"), so it cannot
    // vary regardless of what streams through the disk data path.
    // ========================================================================
    {
        bool mut = mutating("opus:8-dsk.c9");
        if (!mut) {
            Rig r; r.do_reset();
            r.select(0); r.d->wordsync = 0;
            r.arm_dma_read(60);
            run_read_transfer(r, 5, [](int, int i){ return (uint8_t)i; }, 5000);
            uint16_t v1 = (uint16_t)(r.read_reg(ADDR_DSKBYTR) & 0x00FF);
            run_read_transfer(r, 60, [](int, int i){ return (uint8_t)(255 - i); }, 5000);
            uint16_t v2 = (uint16_t)(r.read_reg(ADDR_DSKBYTR) & 0x00FF);
            checks.note("opus:8-dsk.c9: DSKBYTR DATA field near word 5 = 0x" + hex4(v1) +
                        ", near word 60 (very different fed payload) = 0x" + hex4(v2));
            checks.expect(v1 != v2,
                "opus:8-dsk.c9 (p.241, quote 'Special registers allow\\nthe CPU to synchronize with specific "
                "data, or read input a byte at a time.'): if DSKBYTR's DATA field genuinely mirrored varying "
                "disk-stream content, two reads taken at very different points of a run streaming very "
                "different payload bytes would differ -- paula_floppy.v:699 hardwires bits 7:0 to the literal "
                "constant 8'h00, so this is expected to fail against the RTL (finding candidate C6; matches "
                "this package's own claims-file evidence: two consecutive $DFF01A reads both return 0x9000)");
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
        if (!mut) {
            Rig r; r.do_reset();
            r.select(0); r.d->wordsync = 0;
            r.arm_dma_read(60);
            run_read_transfer(r, 5, [](int, int i){ return (uint8_t)i; }, 5000);
            uint16_t v1 = (uint16_t)(r.read_reg(ADDR_DSKBYTR) & 0x00FF);
            run_read_transfer(r, 60, [](int, int i){ return (uint8_t)(255 - i); }, 5000);
            uint16_t v2 = (uint16_t)(r.read_reg(ADDR_DSKBYTR) & 0x00FF);
            checks.note("sol:8-dsk.c9: twin of opus:8-dsk.c9, same measurement -> v1=0x" + hex4(v1) + " v2=0x" + hex4(v2));
            checks.expect(v1 != v2,
                "sol:8-dsk.c9 (p.241, quote 'Special registers allow\\nthe CPU to synchronize with specific "
                "data, or read input a byte at a time.'): twin of opus:8-dsk.c9 (same printed quote, independent "
                "reader) -- same measurement, graded independently under its own uid, same expected RED (C6)");
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
    // ========================================================================
    {
        Rig r; r.do_reset();
        bool mut = mutating("DSKBYTR-CONTROL");
        uint16_t v1 = r.read_reg(ADDR_DSKBYTR);
        uint16_t stim = mut ? 0x0000 : 0x4000;
        r.write_reg(ADDR_DSKLEN, stim);
        uint16_t v2 = r.read_reg(ADDR_DSKBYTR);
        checks.note("DSKBYTR-CONTROL: before=0x" + hex4(v1) + ", DSKLEN<=0x" + hex4(stim) + " -> DSKBYTR=0x" + hex4(v2));
        checks.expect(v1 == 0x9000 && v2 == (mut ? 0x9000 : 0xB000) && v1 != v2,
            "DSKBYTR-CONTROL (control case, no manual citation, same $DFF01A data_out port as opus:8-dsk.c9/"
            "sol:8-dsk.c9): the WRITE-bit mirror (bit 13) DOES vary immediately on a DSKLEN write -- proving the "
            "DSKBYTR read port and this package's comparator are live, so the DATA-field red above is about that "
            "field specifically, not a stuck port");
    }

    // ========================================================================
    // opus:8-dsk.c21 / sol:8-reglist.c4 (p.241, twins) -- P-CIAAPRA-FOUR-BITS.
    // Four independently-controllable sense signals: _track0, _change,
    // _ready, _wprot -- toggling one condition moves only its own bit.
    // ========================================================================
    {
        Rig r; r.do_reset();
        bool mut = mutating("opus:8-dsk.c21");
        r.mount(0, 1000);
        r.select(0);
        r.do_step(false); r.saturate_step_ena(); r.do_step(true); r.saturate_step_ena();

        bool base_ready = r.ready(), base_track0 = r.track0(), base_wprot = r.wprot(), base_change = r.change();

        r.d->floppy_wrprot = 1; r.d->eval();
        bool t1_wprot = r.wprot();
        bool t1_others = (r.ready() == base_ready) && (r.track0() == base_track0) && (r.change() == base_change);
        r.d->floppy_wrprot = 0; r.d->eval();

        // MUTATION: skip the step -- track0 never leaves its base value.
        if (!mut) r.do_step(false);
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
        checks.note("opus:8-dsk.c21: wprot(" + std::to_string(t1_wprot) + "," + std::to_string(t1_others) +
                    ") track0(" + std::to_string(t2_track0) + "," + std::to_string(t2_others) + ") change(" +
                    std::to_string(t3_change) + "," + std::to_string(t3_others) + ") ready(" + std::to_string(t4_ready) + ")");
        checks.expect(ok,
            "opus:8-dsk.c21 (p.241, quote '   CIAAPRA    (\\$BFE001)    four input bits for disk sensing'): four "
            "independently-controllable sense signals -- _track0, _change, _ready, _wprot -- reach the CIAAPRA "
            "side of the boundary from paula_floppy; toggling write-protect, head position, media presence, or "
            "selection each moves exactly its own bit (deselecting is graded on _ready alone, since _sel also "
            "masks the other three at this port -- paula_floppy.v:662,664,666 -- so it is not double-counted "
            "against them). Grades the COUNT/directionality at paula_floppy's own ports, not the exact CIAAPRA "
            "bit position (that needs ciaa/minimig.v, outside this package's assigned module boundary)");
    }
    {
        Rig r; r.do_reset();
        bool mut = mutating("sol:8-reglist.c4");
        r.mount(0, 1000);
        r.select(0);
        r.do_step(false); r.saturate_step_ena(); r.do_step(true); r.saturate_step_ena();

        bool base_ready = r.ready(), base_track0 = r.track0(), base_wprot = r.wprot(), base_change = r.change();

        r.d->floppy_wrprot = 1; r.d->eval();
        bool t1_wprot = r.wprot();
        bool t1_others = (r.ready() == base_ready) && (r.track0() == base_track0) && (r.change() == base_change);
        r.d->floppy_wrprot = 0; r.d->eval();

        if (!mut) r.do_step(false);
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
        checks.note("sol:8-reglist.c4: twin of opus:8-dsk.c21, same measurement -> ok=" + std::to_string(ok));
        checks.expect(ok,
            "sol:8-reglist.c4 (p.241, quote '   CIAAPRA    (\\$BFE001)    four input bits for disk sensing'): "
            "twin of opus:8-dsk.c21 (same printed quote, independent reader) -- same measurement, graded "
            "independently under its own uid");
    }

    // ========================================================================
    // opus:8-dsk.c22 (p.241) -- P-CIAAPRA-SENSING. Reuses the same four
    // toggles' functional-direction half (each bit fires correctly in
    // response to its OWN real sensing condition), a weaker/different
    // emphasis than c21's cross-independence grading above.
    // ========================================================================
    {
        Rig r; r.do_reset();
        bool mut = mutating("opus:8-dsk.c22");
        r.mount(0, 1000);
        r.select(0);
        r.do_step(false); r.saturate_step_ena(); r.do_step(true); r.saturate_step_ena();

        r.d->floppy_wrprot = 1; r.d->eval();
        bool wprot_fires = r.wprot();
        r.d->floppy_wrprot = 0; r.d->eval();

        // MUTATION: skip the step -- track0's sensing-response is never
        // exercised.
        if (!mut) r.do_step(false);
        bool track0_fires = !r.track0();
        r.saturate_step_ena(); r.do_step(true);

        r.unmount(0);
        bool change_fires = r.change();
        r.mount(0, 1000); r.do_step(false); r.saturate_step_ena(); r.do_step(true);

        r.deselect_all();
        bool ready_fires = !r.ready();

        bool ok = wprot_fires && track0_fires && change_fires && ready_fires;
        checks.note("opus:8-dsk.c22: wprot_fires=" + std::to_string(wprot_fires) + " track0_fires=" +
                    std::to_string(track0_fires) + " change_fires=" + std::to_string(change_fires) +
                    " ready_fires=" + std::to_string(ready_fires));
        checks.expect(ok,
            "opus:8-dsk.c22 (p.241, quote '   CIAAPRA    (\\$BFE001)    four input bits for disk sensing'): "
            "each of the four floppy-side sense signals responds, in the functionally-correct direction, to a "
            "genuinely distinct disk-sensing condition -- write-protect, head position, media presence, "
            "selection -- confirming they are four real sensing signals, not four copies of one condition under "
            "different names");
    }

    // ========================================================================
    // opus:8-dsk.c24 / sol:8-reglist.c6 (p.241, twins) -- P-CIABPRB-EIGHT-BITS.
    // Eight independently-drivable control inputs reach paula_floppy from
    // the CIABPRB side: _sel[3:0] (4), _step, direc, side, _motor.
    // ========================================================================
    {
        bool mut = mutating("opus:8-dsk.c24");
        bool sel_ok = true;
        {
            Rig r; r.do_reset();
            for (int n = 0; n < 4; n++) {
                r.deselect_all(); r.settle_edges(2);
                bool desel = r.ready();
                if (!(mut && n == 3)) r.select(n);
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
        checks.note("opus:8-dsk.c24: sel_ok=" + std::to_string(sel_ok) + " step_direc_ok=" +
                    std::to_string(step_direc_ok) + " side_ok=" + std::to_string(side_ok) + " (sector " +
                    std::to_string(sec_side0) + " vs " + std::to_string(sec_side1) + ") motor_ok=" +
                    std::to_string(motor_ok));
        checks.expect(ok,
            "opus:8-dsk.c24 (p.241, quote '   CIABPRB    (\\$BFD100)    eight output bits for disk selection, "
            "control\\n                             and stepping'): eight independently-drivable control inputs "
            "reach paula_floppy from the CIABPRB side -- _sel[3:0] (selection, via _ready), _step/direc "
            "(stepping, via _track0), side (via the requested sdc_sector base, paula_floppy.v:669,283), _motor "
            "(via the motor_on latch read through index) -- each shown to produce a distinguishable effect. "
            "Grades the COUNT at paula_floppy's own ports, not the exact CIABPRB bit position (minimig.v, "
            "outside this package's assigned boundary)");
    }
    {
        bool mut = mutating("sol:8-reglist.c6");
        bool sel_ok = true;
        {
            Rig r; r.do_reset();
            for (int n = 0; n < 4; n++) {
                r.deselect_all(); r.settle_edges(2);
                bool desel = r.ready();
                if (!(mut && n == 3)) r.select(n);
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
        checks.note("sol:8-reglist.c6: twin of opus:8-dsk.c24, same measurement -> ok=" + std::to_string(ok));
        checks.expect(ok,
            "sol:8-reglist.c6 (p.241, quote '   CIABPRB    (\\$BFD100)    eight output bits'): twin of "
            "opus:8-dsk.c24 (same printed quote, independent reader) -- same measurement, graded independently "
            "under its own uid");
    }

    // ========================================================================
    // opus:8-dsk.c25 / sol:8-reglist.c7 (p.241, twins) -- P-CIABPRB-SELECTION.
    // _sel[3:0] targets a specific drive for DMA (paula_floppy.v:285,363,
    // sdc_rd<=~_sel).
    // ========================================================================
    {
        bool mut = mutating("opus:8-dsk.c25");
        Rig r; r.do_reset(); r.select(0);
        r.arm_dma_read(8);
        r.wait_edges([&]{ return r.d->sdc_rd != 0; }, 60);
        uint8_t rd0 = r.d->sdc_rd;

        Rig r2; r2.do_reset();
        int target = mut ? 0 : 2;   // MUTATION: select drive 0 again, checked against drive 2's bit
        r2.select(target);
        r2.arm_dma_read(8);
        r2.wait_edges([&]{ return r2.d->sdc_rd != 0; }, 60);
        uint8_t rd2 = r2.d->sdc_rd;

        checks.note("opus:8-dsk.c25: drive0 -> sdc_rd=" + hex4(rd0) + "; target drive " + std::to_string(target) +
                    " -> sdc_rd=" + hex4(rd2));
        checks.expect(rd0 == 0x1 && rd2 == 0x4,
            "opus:8-dsk.c25 (p.241, quote '   CIABPRB    (\\$BFD100)    eight output bits for disk selection, "
            "control\\n                             and stepping'): selecting drive 0 requests sdc_rd bit 0 "
            "only, and selecting drive 2 requests sdc_rd bit 2 only -- paula_floppy.v:285,363 (sdc_rd<=~_sel) -- "
            "confirming _sel targets a SPECIFIC drive, not a generic 'some drive' signal");
    }
    {
        bool mut = mutating("sol:8-reglist.c7");
        Rig r; r.do_reset(); r.select(0);
        r.arm_dma_read(8);
        r.wait_edges([&]{ return r.d->sdc_rd != 0; }, 60);
        uint8_t rd0 = r.d->sdc_rd;

        Rig r2; r2.do_reset();
        int target = mut ? 0 : 2;
        r2.select(target);
        r2.arm_dma_read(8);
        r2.wait_edges([&]{ return r2.d->sdc_rd != 0; }, 60);
        uint8_t rd2 = r2.d->sdc_rd;

        checks.note("sol:8-reglist.c7: twin of opus:8-dsk.c25, same measurement -> rd0=" + hex4(rd0) + " rd(target)=" + hex4(rd2));
        checks.expect(rd0 == 0x1 && rd2 == 0x4,
            "sol:8-reglist.c7 (p.241, quote '   CIABPRB    (\\$BFD100)    eight output bits'): twin of "
            "opus:8-dsk.c25 (same printed quote, independent reader) -- same measurement, graded independently "
            "under its own uid");
    }

    // ========================================================================
    // opus:8-dsk.c26 / sol:8-reglist.c8 (p.241, twins) -- P-CIABPRB-CONTROL.
    // _motor latches motor_on per drive; side selects the head surface
    // (paula_floppy.v:624-659, :669).
    // ========================================================================
    {
        bool mut = mutating("opus:8-dsk.c26");
        bool motor_ok;
        {
            Rig r; r.do_reset(); r.select_with_motor(0, true);
            bool on_reading = r.peek_index(0);
            Rig r2; r2.do_reset();
            // MUTATION: latch the SAME level (ON) instead of OFF.
            r2.select_with_motor(0, mut ? true : false);
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
        checks.note("opus:8-dsk.c26: motor_ok=" + std::to_string(motor_ok) + " side_ok=" + std::to_string(side_ok) +
                    " (sector " + std::to_string(sec0) + " vs " + std::to_string(sec1) + ")");
        checks.expect(motor_ok && side_ok,
            "opus:8-dsk.c26 (p.241, quote '   CIABPRB    (\\$BFD100)    eight output bits for disk selection, "
            "control\\n                             and stepping'): _motor latches a distinguishable per-drive "
            "state at its own select edge (read via index), and side selects a distinguishable head surface "
            "(the requested sdc_sector base) -- both are real, effective disk-CONTROL inputs");
    }
    {
        bool mut = mutating("sol:8-reglist.c8");
        bool motor_ok;
        {
            Rig r; r.do_reset(); r.select_with_motor(0, true);
            bool on_reading = r.peek_index(0);
            Rig r2; r2.do_reset();
            r2.select_with_motor(0, mut ? true : false);
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
        checks.note("sol:8-reglist.c8: twin of opus:8-dsk.c26, same measurement -> motor_ok=" +
                    std::to_string(motor_ok) + " side_ok=" + std::to_string(side_ok));
        checks.expect(motor_ok && side_ok,
            "sol:8-reglist.c8 (p.241, quote '   CIABPRB    (\\$BFD100)    eight output bits'): twin of "
            "opus:8-dsk.c26 (same printed quote, independent reader) -- same measurement, graded independently "
            "under its own uid");
    }

    // ========================================================================
    // opus:8-dsk.c27 / sol:8-reglist.c9 (p.241, twins) -- P-CIABPRB-STEPPING.
    // ========================================================================
    {
        Rig r; r.do_reset();
        bool mut = mutating("opus:8-dsk.c27");
        r.select(0); r.saturate_step_ena();
        bool at_zero = r.track0();
        r.do_step(false);
        bool left_zero = !r.track0();
        r.saturate_step_ena();
        r.do_step(mut ? false : true);   // MUTATION: step away again instead of back
        bool returned = r.track0();
        checks.note("opus:8-dsk.c27: at_zero=" + std::to_string(at_zero) + " left_zero=" + std::to_string(left_zero) +
                    " returned=" + std::to_string(returned));
        checks.expect(at_zero && left_zero && returned,
            "opus:8-dsk.c27 (p.241, quote '   CIABPRB    (\\$BFD100)    eight output bits for disk selection, "
            "control\\n                             and stepping'): _step/direc move the selected drive's track "
            "counter away from and back to track 0 (paula_floppy.v:671-680), confirming a real head-stepping "
            "function");
    }
    {
        Rig r; r.do_reset();
        bool mut = mutating("sol:8-reglist.c9");
        r.select(0); r.saturate_step_ena();
        bool at_zero = r.track0();
        r.do_step(false);
        bool left_zero = !r.track0();
        r.saturate_step_ena();
        r.do_step(mut ? false : true);
        bool returned = r.track0();
        checks.note("sol:8-reglist.c9: twin of opus:8-dsk.c27, same measurement -> at_zero=" + std::to_string(at_zero) +
                    " left_zero=" + std::to_string(left_zero) + " returned=" + std::to_string(returned));
        checks.expect(at_zero && left_zero && returned,
            "sol:8-reglist.c9 (p.241, quote '   CIABPRB    (\\$BFD100)    eight output bits'): twin of "
            "opus:8-dsk.c27 (same printed quote, independent reader) -- same measurement, graded independently "
            "under its own uid");
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

// MUTATION FIXTURE-LIVE stimulus red: word 2 driven identical to word 1 (0x0000) instead of 0x4000, so the two readings are indistinguishable (MUTATE=FIXTURE-LIVE).
// MUTATION opus:8-dsk.c1 stimulus red: drive 3 is never actually selected in the loop, so its "selected -> ready" half is checked against a state that never occurred (MUTATE=opus:8-dsk.c1).
// MUTATION sol:8-dsk.c1 stimulus red: same omission as opus:8-dsk.c1, independent uid (MUTATE=sol:8-dsk.c1).
// MUTATION P-WHOLETRACK-DMA stimulus red: DSKLEN is programmed for one word MORE than a whole track holds (5985 instead of 5984), while the check still expects completion at exactly 5984 -- an ordinary stimulus mutation, now covering opus:8-dsk.c7/sol:8-dsk.c7's dimensionless single-pass reading as well as the capacity content it always graded (MUTATE=P-WHOLETRACK-DMA).
// MUTATION opus:8-dsk.c8 stimulus red: the matching DSKSYNC value (0x4489) is used on the "should not match" probe too, while the check still expects the mismatch outcome (zero delivered) (MUTATE=opus:8-dsk.c8).
// MUTATION sol:8-dsk.c8 stimulus red: same substitution as opus:8-dsk.c8, independent uid (MUTATE=sol:8-dsk.c8).
// MUTATION opus:8-dsk.c9 stimulus green: this assertion is RED at baseline (C6), so per the rule against mutating an already-failing check, the stimulus substitutes DSKBYTR-CONTROL's WRITE-bit mechanism -- proven live elsewhere in this run -- through the identical data_out-varies comparator in place of the DATA field the RTL never varies (MUTATE=opus:8-dsk.c9).
// MUTATION sol:8-dsk.c9 stimulus green: same substitute mechanism as opus:8-dsk.c9, independent uid (MUTATE=sol:8-dsk.c9).
// MUTATION DSKBYTR-CONTROL stimulus red: the stimulus write is changed from 0x4000 (WRITE bit set) to 0x0000 (WRITE bit clear), so the two DSKBYTR readings are indistinguishable (MUTATE=DSKBYTR-CONTROL).
// MUTATION opus:8-dsk.c21 stimulus red: the step that isolates _track0 is skipped, so that toggle no longer isolates anything and the drive stays at track 0 throughout (MUTATE=opus:8-dsk.c21).
// MUTATION sol:8-reglist.c4 stimulus red: same omission as opus:8-dsk.c21, independent uid (MUTATE=sol:8-reglist.c4).
// MUTATION opus:8-dsk.c22 stimulus red: the step that exercises _track0's sensing response is skipped (MUTATE=opus:8-dsk.c22).
// MUTATION opus:8-dsk.c24 stimulus red: drive 3 is never actually selected in the _sel sub-check, so one of the eight bits' distinguishing effect is never exercised (MUTATE=opus:8-dsk.c24).
// MUTATION sol:8-reglist.c6 stimulus red: same omission as opus:8-dsk.c24, independent uid (MUTATE=sol:8-reglist.c6).
// MUTATION opus:8-dsk.c25 stimulus red: the second trial selects drive 0 again instead of drive 2, while the check still expects drive 2's sdc_rd bit pattern (MUTATE=opus:8-dsk.c25).
// MUTATION sol:8-reglist.c7 stimulus red: same redirection as opus:8-dsk.c25, independent uid (MUTATE=sol:8-reglist.c7).
// MUTATION opus:8-dsk.c26 stimulus red: the second motor trial latches the SAME level (ON) instead of OFF, so the two motor readings are indistinguishable (MUTATE=opus:8-dsk.c26).
// MUTATION sol:8-reglist.c8 stimulus red: same redirection as opus:8-dsk.c26, independent uid (MUTATE=sol:8-reglist.c8).
// MUTATION opus:8-dsk.c27 stimulus red: the second step is redirected to move away from zero AGAIN instead of back toward it, so the drive never returns to track 0 and the final read is checked against a state that never occurred (MUTATE=opus:8-dsk.c27).
// MUTATION sol:8-reglist.c9 stimulus red: same redirection as opus:8-dsk.c27, independent uid (MUTATE=sol:8-reglist.c9).
// MUTATION sol:8-dskdma.c7 stimulus red: DMA is never armed at all, so fdd_led stays 0 throughout and the "stays active while stepping" half of this claim is checked against a transfer that was never running (MUTATE=sol:8-dskdma.c7).
