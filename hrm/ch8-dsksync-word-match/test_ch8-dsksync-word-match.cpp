// Test purpose: HRM conformance package for DSKSYNC's match word, the
//   ADKCON WORDSYNC gate, the "resync every time" claim, word alignment, and
//   the paula_floppy-side half of the INTREQ DSKSYNC signal (pp.247, 249,
//   250, 279). 22 claims, from
//   work/ch8-s2-build/ch8-dsksync-word-match.jsonl. Full derivation in
//   SPECIFICATION.md (committed as 86b4935, before paula_floppy.v was
//   opened).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual pp.247, 249, 250, 279 --
//   see SPECIFICATION.md for the full quotation table.
//
// RTL READ, IN ORDER, ALL AFTER SPECIFICATION.md WAS COMMITTED (86b4935):
//   1. hrm/ch8-ciaapra-address-window and hrm/ch8-drive-select-motor-led's
//      test sources -- already disclosed in SPECIFICATION.md as read BEFORE
//      the spec, for this suite's Checks/mutating()/MUTATION boilerplate.
//   2. hrm/paula_floppy/test_paula_floppy.cpp (RTL_REGRESSION, not HRM) --
//      for the tick()/write_reg()/read_reg()/SD-byte-feed idiom and the
//      DSKLEN double-write, all reused verbatim below.
//   3. paula_floppy.v -- full file (1041 lines).
//   4. paula_floppy_fifo.v -- full file (60 lines): a 2048-word pointer FIFO
//      whose `out` register prefetches `mem[out_ptr+1]` on every `rd`
//      pulse. Consequence, confirmed empirically below before any assertion
//      was written: reading DSKDATR N times in a row returns
//      true_delivered[1..N], never true_delivered[0] -- the very first word
//      paula_floppy ever writes into the FIFO is consumed by the FIFO's own
//      look-ahead prefetch on the first read and is never observable at
//      `data_out`. This is a real, unavoidable property of this fixture at
//      this boundary (confirmed against paula_floppy_fifo.v's own read
//      logic, not a bug in this harness), and every assertion below is
//      built to hold regardless of it -- none reads the very first
//      delivered word directly; each compares whole SHIFTED sequences
//      instead, which cancels the offset out. It also explains why an
//      earlier classify-stage dump (`work/ch8-s2-build/...jsonl`,
//      `opus:8-dma.c29` evidence) reported "the preamble and both 0x4489
//      words are gone" for DSKSYNC=0x4489: word2 (the first 0x4489) is
//      genuinely excluded by trackrdok's latch, and word3 (the second) is
//      the true first DELIVERED word and is the one this FIFO quirk hides
//      -- two different causes producing what reads as one fact.
//
// MECHANISM, read from paula_floppy.v myself (SPECIFICATION.md had only
//   classify's disclosed, not-yet-verified citations):
//   - `floppy_data` (paula_floppy.v:481-500) is a WORD-INDEXED multiplexer
//     on `fifo_word_counter`, not a bit-serial shift register: word0,1 are
//     the fixed 0xaaaa preamble, word2,3 the fixed 0x4489 sync pair, words
//     4-29 the header/checksum-gap area (mostly 0xaaaa, with a
//     content-derived checksum at 26,27), words 30,31 the payload checksum,
//     and words 32-543 the fed SD-sector payload, MFM-"encoded"
//     (paula_floppy.v:467-477, candidate C9's context-free encoder: output
//     bit pairs are {1,src_bit}, clock forced to 1). Each 16-bit output
//     word is entirely determined by ONE data byte pair (`fd_dma_buf_even`/
//     `_odd` at the same index) with no dependency on neighbouring bytes.
//   - `sync_match` (paula_floppy.v:914-915) compares `floppy_data` against
//     `dsksync` EVERY `clk7_en` tick, continuously, for as long as
//     `fifo_reading_sector && trackrd` hold -- not just once. A DSKSYNC
//     value that recurs later in the stream (by construction, not by
//     accident) makes `sync_match` pulse again.
//   - `trackrdok` (paula_floppy.v:920-926) is `~wordsync | sync_match |
//     trackrdok`, registered. On the cycle of the FIRST match, `fifo_wr`
//     (paula_floppy.v:771, gated on the OLD value of trackrdok) is still 0
//     -- that word is excluded. From the NEXT cycle on, trackrdok is
//     permanently 1 (an OR with itself), so `fifo_wr` is 1 on EVERY
//     subsequent cycle regardless of further `sync_match` pulses --
//     including the very next word, which is why a second consecutive
//     occurrence of the same value (the natural 0x4489,0x4489 pair, or a
//     DSKSYNC=0xaaaa preamble immediately followed by another 0xaaaa word)
//     is delivered, not excluded. This is candidate C10 read directly from
//     the register-transfer logic, not inferred.
//   - CRITICAL PIPELINE FACT for constructing a controllable "recurring
//     match": `fd_dma_buf_out` (paula_floppy.v:216-223), which supplies the
//     payload region's `floppy_data`, is loaded only `if(clk7_en &&
//     fifo_wr)` -- i.e. only once real delivery is already under way. A
//     payload byte can therefore NEVER be the module's first-ever match
//     (confirmed empirically: an attempted DSKSYNC value reachable only
//     from a payload byte, with nothing else in the stream matching it,
//     produced zero deliveries in an exploratory run -- trackrdok never
//     opens, because the payload pipeline that would produce the match
//     never starts without a match already having opened it). Every
//     assertion below opens `trackrdok` with a NATURAL fixed-position match
//     (0xaaaa preamble or 0x4489 sync) and only uses an engineered payload
//     byte (fed identically to both the even and odd sub-byte of one pair
//     index, giving a clean, predictable word by the encoder's own
//     context-free rule) for a RECURRING match once delivery is already
//     open.
//   - `syncint` (paula_floppy.v:917) is `sync_match | (a motor/disk_present
//     term this rig never drives, since it is held at 0)` -- in this
//     fixture it tracks `sync_match` exactly, independent of `wordsync`
//     (wordsync's only consumer is the trackrdok latch, not this line).
//     `syncint`'s OWN observable is graded (P-INTREQSET, P-INTREQINDEP);
//     INTREQ's bit 12 itself is paula_intcontroller's, outside this
//     package's boundary, per SPECIFICATION.md.
//
// MEASUREMENT BASIS. Every content-based assertion compares two or three
//   fresh trials' DELIVERED-WORD SEQUENCES (read via DSKDATR, `$008`) or
//   SYNCINT PULSE COUNTS, never a single raw value against a hand-derived
//   constant -- confirmed empirically before writing any assertion (see the
//   exploratory measurements in the report accompanying this commit).
//
// RIG HAZARD (task-disclosed, confirmed by reading paula_floppy.v:671-680
//   myself): `dsktrack` carries no reset term. Every trial below allocates
//   a fresh Vpaula_floppy.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup, exactly as this suite's chapter-8 packages already do. Ids are
//   the claim's own `classify.uid` for the 20 remaining named assertions and
//   a plain uppercase id for A0, which carries no manual citation.
//   `opus:8-dsksync.c4` and `sol:8-dsksync.c5` (P-RESYNC, candidate C10)
//   carry NO `expect()` and NO mutation record: REPAIRED 2026-09-15, see the
//   P-RESYNC block below and the DECLINE note that follows this paragraph.
//   RUN LOG: see the report accompanying this commit for the full
//   baseline/mutated pass-fail table; summarised in the records below.
//
// REPAIR NOTE / P-RESYNC (2026-09-15). Independent validation found the
//   previous graded-RED P-RESYNC assertion tested MORE than p.250 states.
//   "During disk read DMA, the controller will resync every time the word
//   match is found" does not say a later, already-word-aligned matching word
//   is OMITTED from delivery -- a controller can re-align its word boundary
//   at a later match while still delivering that already-aligned word. A
//   sound version needs a deliberately BIT-MISALIGNED recurring match and an
//   observable showing the subsequent word boundary re-aligns.
//
//   Checked directly against this fixture's own generator (paula_floppy.v:
//   479-500) before deciding: `floppy_sector_data`/`floppy_data` is a purely
//   WORD-INDEXED combinational multiplexer keyed on `fifo_word_counter`/
//   `fifo_sector_counter`. Every position it can ever present is one of a
//   small, fixed set of whole 16-bit values (0xaaaa preamble/gap, 0x4489
//   sync, `mfm_encoder_odd/even`, the two checksum words, `data_odd`/
//   `data_even` for the payload) selected by a discrete word-counter value
//   -- there is no shift register, no sub-word or bit-rotated view of the
//   stream anywhere in this module. `sync_match` (:914-915) compares this
//   WHOLE-WORD value against `dsksync` once per `fifo_word_counter` tick.
//   Because `floppy_data` never presents anything but a word-aligned value,
//   there is no stimulus this fixture's input space (SD-sector bytes fed via
//   `sdc_byte_in_strobe`) can produce that would make a match occur at a
//   bit-shifted (non-word-aligned) position for `sync_match` to compare
//   against -- the concept the repaired assertion would need to test does
//   not exist at this boundary. This is "unfalsifiable at this boundary" in
//   the build brief's own sense, not merely difficult: no RTL knob a
//   Verilated testbench can turn presents a bit-misaligned word to compare.
//   DECLINED via `record()` for both uids, stating exactly this. The
//   original engineered "recurring match" (MARKER_PAIR feeding 0x00 at
//   payload pair-index 10, producing floppy_data 0xAAAA, the SAME value as
//   the DSKSYNC_LEADING preamble match) was itself already word-aligned by
//   construction -- consistent with, and explained by, this finding: nothing
//   built from this generator could have been anything else.

#include <cstdint>
#include <cstdlib>
#include <cstring>
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
    std::snprintf(b, sizeof b, "0x%04X", v);
    return std::string(b);
}

constexpr uint16_t DSKSYNC_LEADING = 0xAAAA;  // matches the fixed preamble word0
constexpr uint16_t DSKSYNC_ALT     = 0x4489;  // matches the fixed sync word2
constexpr uint16_t DSKSYNC_NONE    = 0x1234;  // occurs nowhere in this fixture's stream
constexpr uint8_t  MARKER_PAIR     = 10;      // payload pair-index of the engineered recurring match
constexpr uint8_t  MARKER_BYTE     = 0x00;    // -> floppy_data 0xAAAA (matches DSKSYNC_LEADING)
constexpr uint8_t  FILLER_BYTE     = 0xFF;    // -> floppy_data 0xFFFF, everywhere else
constexpr uint16_t FILLER_WORD     = 0xFFFF;
constexpr uint16_t MARKER_WORD     = 0xAAAA;
constexpr int      READ_N          = 45;
constexpr int      DSKLEN_BUDGET   = 80;

// One Verilated model per trial -- dsktrack has no reset term
// (paula_floppy.v:671-680).
struct Rig {
    Vpaula_floppy* d;
    int clk_cycle = 0;

    Rig() {
        d = new Vpaula_floppy;
        d->clk = 0; d->clk7_en = 0; d->clk7n_en = 0;
        d->reset = 1;
        d->ntsc = 0; d->sof = 0; d->enable = 0;
        d->reg_address_in = 0; d->data_in = 0;
        d->_step = 1; d->direc = 1; d->_sel = 0xF; d->side = 0; d->_motor = 1;
        d->wordsync = 0;
        d->floppy_drives = 1; d->floppy_wrprot = 0;
        d->sdc_image_mounted = 0; d->sdc_image_size = 0;
        d->sdc_busy = 0; d->sdc_done = 0;
        d->sdc_byte_in_strobe = 0; d->sdc_byte_addr = 0; d->sdc_byte_in_data = 0;
        for (int i = 0; i < 32; i++) tick();
        d->reset = 0;
        for (int i = 0; i < 32; i++) tick();
        d->_sel = 0xE;   // drive 0 selected (active low)
        d->_motor = 0;
        settle(32);
    }
    ~Rig() { delete d; }
    Rig(const Rig&) = delete;

    void tick() {
        d->eval();
        d->clk = 1; d->eval();
        d->clk = 0; d->eval();
        clk_cycle = (clk_cycle + 1) % 4;
        d->clk7_en  = (clk_cycle == 0);
        d->clk7n_en = (clk_cycle == 2);
    }
    void settle(int n = 8) { for (int i = 0; i < n; i++) tick(); }

    void write_reg(int addr, uint16_t data) {
        while (!d->clk7_en) tick();
        d->reg_address_in = addr >> 1;
        d->data_in = data;
        tick(); tick(); tick(); tick();
        d->reg_address_in = 0;
        d->data_in = 0;
    }
    uint16_t read_reg(int addr) {
        while (!d->clk7_en) tick();
        d->reg_address_in = addr >> 1;
        tick(); tick(); tick(); tick();
        uint16_t val = d->data_out;
        d->reg_address_in = 0;
        return val;
    }

    // Arm a read DMA: wordsync must already be set by the caller before
    // this, DSKSYNC likewise. DSKLEN needs the double write
    // (paula_floppy.v:743) -- confirmed against the RTL, matching the
    // task's own disclosed measurement.
    void arm(uint16_t len_words) {
        d->enable = 1;
        write_reg(0x024, 0x8000 | len_words);
        write_reg(0x024, 0x8000 | len_words);
        for (int i = 0; i < 200 && d->sdc_rd == 0; i++) tick();
    }

    // Feed one 512-byte sector. `filler` everywhere, except pair-index
    // `markerPair` (both its even and odd sub-byte) gets `markerByte` --
    // markerPair<0 means no marker at all (pure filler sector).
    void feed(int markerPair, uint8_t markerByte, uint8_t filler) {
        d->sdc_busy = 1;
        settle(20);
        for (int i = 0; i < 512; i++) {
            int p = i / 2;
            uint8_t b = (p == markerPair) ? markerByte : filler;
            d->sdc_byte_addr = i;
            d->sdc_byte_in_data = b;
            d->sdc_byte_in_strobe = 1;
            tick(); tick(); tick(); tick();
            d->sdc_byte_in_strobe = 0;
            tick(); tick(); tick(); tick();
        }
        d->sdc_busy = 0;
        settle(100);
    }

    std::vector<uint16_t> read_words(int n) {
        std::vector<uint16_t> out;
        out.reserve(n);
        for (int i = 0; i < n; i++) out.push_back(read_reg(0x008));
        return out;
    }
};

// A full content trial: arm+feed+read, on a fresh Rig. `markerPair<0`
// disables the engineered recurring match (pure filler payload).
std::vector<uint16_t> trial(bool wordsync, uint16_t dsksync, int markerPair) {
    Rig r;
    r.d->wordsync = wordsync ? 1 : 0;
    r.write_reg(0x07E, dsksync);
    r.arm(DSKLEN_BUDGET);
    r.feed(markerPair, MARKER_BYTE, FILLER_BYTE);
    return r.read_words(READ_N);
}

// A syncint pulse-count trial: arm, then feed while counting rising edges
// of `syncint` THROUGHOUT (feed()'s own internal settle would otherwise
// hide a pulse that fires within it -- the match happens within a handful
// of clk7_en ticks of the sector becoming ready, near the START of that
// settle, confirmed empirically), plus `settleTicks` further ticks after.
int trial_syncint(bool wordsync, uint16_t dsksync, int settleTicks) {
    Rig r;
    r.d->wordsync = wordsync ? 1 : 0;
    r.write_reg(0x07E, dsksync);
    r.arm(DSKLEN_BUDGET);
    int pulses = 0;
    bool prev = false;
    auto watch = [&](int n) { for (int i = 0; i < n; i++) { r.tick();
        bool cur = r.d->syncint; if (cur && !prev) pulses++; prev = cur; } };
    r.d->sdc_busy = 1;
    watch(20);
    for (int i = 0; i < 512; i++) {
        r.d->sdc_byte_addr = i;
        r.d->sdc_byte_in_data = FILLER_BYTE;
        r.d->sdc_byte_in_strobe = 1;
        watch(4);
        r.d->sdc_byte_in_strobe = 0;
        watch(4);
    }
    r.d->sdc_busy = 0;
    watch(100 + settleTicks);
    return pulses;
}

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

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    // ========================================================================
    // A0 -- FIXTURE DATA-PATH LIVENESS. No claim id, no manual page. Two
    // distinct known payload bytes (0x00 -> 0xAAAA, 0xFF -> 0xFFFF, both
    // confirmed by the encoder's own context-free rule, paula_floppy.v:
    // 467-477), read via DSKDATR through the SAME rig every other assertion
    // uses (wordsync=0, so nothing is gated), before anything match-based
    // is trusted.
    // ========================================================================
    {
        bool mut = mutating("A0-LIVENESS-KNOWN-DISTINCT-WORDS");
        // MUTATE=A0-LIVENESS-KNOWN-DISTINCT-WORDS: marker pair index moved so the two adjacent
        // payload words are BOTH filler (0xFFFF, 0xFFFF) -- indistinguishable.
        int markerPair = mut ? -1 : 0;
        auto w = trial(false, DSKSYNC_NONE, markerPair);
        uint16_t first = w[31];   // pair-index 0 -> floppy_data word 32
        uint16_t second = w[32];  // pair-index 1 -> floppy_data word 33
        checks.note("A0: ungated read at payload pair-index 0 = 0x" + std::to_string(first) +
                    ", pair-index 1 = 0x" + std::to_string(second));
        checks.expect(first == MARKER_WORD && second == FILLER_WORD && first != second,
            "A0-LIVENESS-KNOWN-DISTINCT-WORDS: two distinct known payload bytes (0x00, 0xFF), fed at "
            "adjacent pair-indices and read back via DSKDATR with wordsync=0 (ungated), arrive as the "
            "distinct words the encoder's own rule predicts (0xAAAA, 0xFFFF) -- the fixture's data path "
            "is live, not stuck");
    }

    // ========================================================================
    // P-ALIGN -- p.247, "Using DSKSYNC ... will guarantee word alignment of
    // the data." Graded as: the delivered sequence under a leading DSKSYNC
    // match is a clean word-for-word continuation of the ungated sequence,
    // shifted by exactly the one excluded match word -- over SEVERAL words,
    // not one.
    // ========================================================================
    {
        bool mutOpus = mutating("opus:8-dma.c29");
        uint16_t dsA = mutOpus ? DSKSYNC_NONE : DSKSYNC_LEADING;
        auto ungated = trial(false, DSKSYNC_NONE, -1);
        auto gated = trial(true, dsA, -1);
        bool aligned = true;
        for (int k = 0; k < 6; k++) if (gated[k] != ungated[k + 1]) aligned = false;
        checks.note("P-ALIGN/opus:8-dma.c29: gated[0..5] = " + std::to_string(gated[0]) + "," +
                    std::to_string(gated[1]) + ",... vs ungated[1..6]");
        checks.expect(aligned,
            "opus:8-dma.c29 (p.247, 'Using the DSKSYNC register ... will guarantee word alignment of the "
            "data.'): six consecutive words delivered with WORDSYNC gating match, word for word, the "
            "ungated stream shifted by exactly the one excluded match word -- the data stays word-aligned "
            "across the whole run, not just at the match point");
    }
    {
        bool mutSol = mutating("sol:8-dsksync.c1");
        uint16_t dsA = mutSol ? DSKSYNC_NONE : DSKSYNC_LEADING;
        auto ungated = trial(false, DSKSYNC_NONE, -1);
        auto gated = trial(true, dsA, -1);
        bool aligned = true;
        for (int k = 0; k < 6; k++) if (gated[k] != ungated[k + 1]) aligned = false;
        checks.expect(aligned,
            "sol:8-dsksync.c1 (p.247, twin of opus:8-dma.c29, same printed quote, independent reader): "
            "same six-word alignment measurement, graded independently under its own uid");
    }

    // ========================================================================
    // P-ENABLE -- p.249, "WORDSYNC ... enables synchronizing ... on disk
    // read of a word." Graded as: gating changes what is delivered first,
    // relative to the same stream read ungated.
    // ========================================================================
    {
        bool mut = mutating("opus:8-adkcon.c19");
        // MUTATE: the "gated" trial is run with wordsync=0 too, so gating
        // has no effect to detect.
        auto ungated = trial(false, DSKSYNC_NONE, -1);
        auto gated = trial(!mut, DSKSYNC_LEADING, -1);
        checks.note("P-ENABLE/opus:8-adkcon.c19: ungated[0]=0x" + std::to_string(ungated[0]) +
                    " gated[0]=0x" + std::to_string(gated[0]));
        checks.expect(gated[0] != ungated[0],
            "opus:8-adkcon.c19 (p.249, 'WORDSYNC Value of 1 enables synchronizing ... on disk read of a "
            "word.'): with WORDSYNC=1 and a DSKSYNC value the stream contains, the first word delivered "
            "differs from the ungated (WORDSYNC=0) reading -- the enable measurably changes what the "
            "module delivers");
    }
    {
        bool mut = mutating("sol:8-adkcon.c15");
        auto ungated = trial(false, DSKSYNC_NONE, -1);
        auto gated = trial(!mut, DSKSYNC_LEADING, -1);
        checks.expect(gated[0] != ungated[0],
            "sol:8-adkcon.c15 (p.249, twin of opus:8-adkcon.c19): same enable-effect measurement, graded "
            "independently under its own uid");
    }

    // ========================================================================
    // P-DMASTART -- p.249, "... and starting of DMA on disk read of a
    // word." Graded as: with WORDSYNC=1 and a DSKSYNC value that never
    // occurs, the transfer never starts at all (not merely misaligned);
    // with a value that does occur, it does.
    // ========================================================================
    {
        bool mut = mutating("opus:8-adkcon.c20");
        // MUTATE: the "never occurs" trial is given a DSKSYNC value that DOES occur.
        uint16_t dsNone = mut ? DSKSYNC_LEADING : DSKSYNC_NONE;
        auto none = trial(true, dsNone, -1);
        auto some = trial(true, DSKSYNC_LEADING, -1);
        bool allStale = true;
        for (auto w : none) if (w != 0x0000) allStale = false;
        checks.note("P-DMASTART/opus:8-adkcon.c20: none[0..2]=" + std::to_string(none[0]) + "," +
                    std::to_string(none[1]) + "," + std::to_string(none[2]) +
                    " some[0]=0x" + std::to_string(some[0]));
        checks.expect(allStale && some[0] != 0x0000,
            "opus:8-adkcon.c20 (p.249, 'WORDSYNC Value of 1 enables synchronizing and starting of DMA on "
            "disk read of a word.'): with WORDSYNC=1 and a DSKSYNC value absent from the whole stream, "
            "not one word is ever delivered (the transfer itself never starts); with a value present, it "
            "does -- the match is what starts DMA, not merely what aligns it");
    }
    {
        bool mut = mutating("sol:8-adkcon.c16");
        uint16_t dsNone = mut ? DSKSYNC_LEADING : DSKSYNC_NONE;
        auto none = trial(true, dsNone, -1);
        auto some = trial(true, DSKSYNC_LEADING, -1);
        bool allStale = true;
        for (auto w : none) if (w != 0x0000) allStale = false;
        checks.expect(allStale && some[0] != 0x0000,
            "sol:8-adkcon.c16 (p.249, twin of opus:8-adkcon.c20): same DMA-start measurement, graded "
            "independently under its own uid");
    }

    // ========================================================================
    // P-WRITETARGET -- p.249, "The word on which to synchronize must be
    // written into the DSKSYNC address ($DFF07E)." Graded as: two
    // different written values produce two different, specific match
    // points (not a fixed built-in constant).
    // ========================================================================
    {
        bool mut = mutating("opus:8-adkcon.c21");
        auto ungated = trial(false, DSKSYNC_NONE, -1);
        uint16_t ds2 = mut ? DSKSYNC_LEADING : DSKSYNC_ALT;  // MUTATE: same value as the first trial
        auto leading = trial(true, DSKSYNC_LEADING, -1);
        auto alt = trial(true, ds2, -1);
        checks.note("P-WRITETARGET/opus:8-adkcon.c21: leading[0]=0x" + std::to_string(leading[0]) +
                    " alt[0]=0x" + std::to_string(alt[0]) + " ungated[3]=0x" + std::to_string(ungated[3]));
        checks.expect(leading[0] == ungated[1] && alt[0] == ungated[3] && leading[0] != alt[0],
            "opus:8-adkcon.c21 (p.249, 'The word on which to synchronize must be written into the DSKSYNC "
            "address ($DFF07E).'): writing 0xAAAA moves the match to the preamble (delivery starts one "
            "word in); writing 0x4489 moves it to the sync mark (delivery starts three words in) -- the "
            "match point is exactly where the write sent it");
    }
    {
        bool mut = mutating("sol:8-adkcon.c17");
        auto ungated = trial(false, DSKSYNC_NONE, -1);
        uint16_t ds2 = mut ? DSKSYNC_LEADING : DSKSYNC_ALT;
        auto leading = trial(true, DSKSYNC_LEADING, -1);
        auto alt = trial(true, ds2, -1);
        checks.expect(leading[0] == ungated[1] && alt[0] == ungated[3] && leading[0] != alt[0],
            "sol:8-adkcon.c17 (p.249, twin of opus:8-adkcon.c21): same write-target measurement, graded "
            "independently under its own uid");
    }

    // ========================================================================
    // P-SYNCREG -- p.250, "The DSKSYNC register is used to synchronize the
    // input stream." Same underlying fact as P-WRITETARGET, cited from a
    // different page, graded independently.
    // ========================================================================
    {
        bool mut = mutating("opus:8-dsksync.c1");
        auto ungated = trial(false, DSKSYNC_NONE, -1);
        uint16_t ds2 = mut ? DSKSYNC_LEADING : DSKSYNC_ALT;
        auto leading = trial(true, DSKSYNC_LEADING, -1);
        auto alt = trial(true, ds2, -1);
        checks.expect(leading[0] == ungated[1] && alt[0] == ungated[3] && leading[0] != alt[0],
            "opus:8-dsksync.c1 (p.250, 'The DSKSYNC register is used to synchronize the input stream.'): "
            "the register's own written content is the sole comparand that determines the synchronization "
            "point -- proven by moving it and watching the point move with it");
    }
    {
        bool mut = mutating("sol:8-dsksync.c2");
        auto ungated = trial(false, DSKSYNC_NONE, -1);
        uint16_t ds2 = mut ? DSKSYNC_LEADING : DSKSYNC_ALT;
        auto leading = trial(true, DSKSYNC_LEADING, -1);
        auto alt = trial(true, ds2, -1);
        checks.expect(leading[0] == ungated[1] && alt[0] == ungated[3] && leading[0] != alt[0],
            "sol:8-dsksync.c2 (p.250, twin of opus:8-dsksync.c1): same register-content measurement, "
            "graded independently under its own uid");
    }

    // ========================================================================
    // P-GATE -- p.250, "If the WORDSYNC bit is enabled ... no data is
    // transferred until a word is found in the input stream that matches
    // the word in the DSKSYNC register." Graded as: absent a match, zero
    // words transferred, period.
    // ========================================================================
    {
        bool mut = mutating("opus:8-dsksync.c2");
        uint16_t ds = mut ? DSKSYNC_LEADING : DSKSYNC_NONE;
        auto none = trial(true, ds, -1);
        bool allStale = true;
        for (auto w : none) if (w != 0x0000) allStale = false;
        checks.expect(allStale,
            "opus:8-dsksync.c2 (p.250, 'If the WORDSYNC bit is enabled in ADKCON, no data is transferred "
            "until a word is found in the input stream that matches the word in the DSKSYNC register.'): "
            "with a DSKSYNC value absent from the whole stream, every one of " + std::to_string(READ_N) +
            " consecutive DSKDATR reads returns the same unchanged value -- no data is ever transferred");
    }
    {
        bool mut = mutating("sol:8-dsksync.c3");
        uint16_t ds = mut ? DSKSYNC_LEADING : DSKSYNC_NONE;
        auto none = trial(true, ds, -1);
        bool allStale = true;
        for (auto w : none) if (w != 0x0000) allStale = false;
        checks.expect(allStale,
            "sol:8-dsksync.c3 (p.250, twin of opus:8-dsksync.c2): same no-transfer measurement, graded "
            "independently under its own uid");
    }

    // ========================================================================
    // P-ALIGNSTART -- p.250, "On read, DMA will start with the following
    // word from the disk." Graded as: the first delivered word under a
    // gated match equals the successor of the match word in the ungated
    // stream, not the match word itself.
    // ========================================================================
    {
        bool mut = mutating("opus:8-dsksync.c3");
        auto ungated = trial(false, DSKSYNC_NONE, -1);
        auto gated = trial(true, DSKSYNC_LEADING, -1);
        // MUTATE: compare against the MATCH word itself instead of its successor.
        uint16_t ref = mut ? ungated[0] : ungated[1];
        checks.note("P-ALIGNSTART/opus:8-dsksync.c3: gated[0]=0x" + std::to_string(gated[0]) +
                    " ref=0x" + std::to_string(ref));
        checks.expect(gated[0] == ref,
            "opus:8-dsksync.c3 (p.250, 'On read, DMA will start with the following word from the disk.'): "
            "the first word delivered once WORDSYNC opens is the word immediately AFTER the matched sync "
            "word, not the sync word itself");
    }
    {
        bool mut = mutating("sol:8-dsksync.c4");
        auto ungated = trial(false, DSKSYNC_NONE, -1);
        auto gated = trial(true, DSKSYNC_LEADING, -1);
        uint16_t ref = mut ? ungated[0] : ungated[1];
        checks.expect(gated[0] == ref,
            "sol:8-dsksync.c4 (p.250, twin of opus:8-dsksync.c3): same following-word measurement, graded "
            "independently under its own uid");
    }

    // ========================================================================
    // P-RESYNC -- p.250, "During disk read DMA, the controller will resync
    // every time the word match is found." Candidate C10. DECLINED
    // 2026-09-15 for BOTH uids, not graded. Repair: independent validation
    // found the previous graded-RED pair tested MORE than the cited
    // sentence states -- it treated "the later, already-word-aligned match
    // word is OMITTED from delivery" as the definition of "resync", but the
    // sentence does not say that; a controller can re-align its word
    // boundary at a later match while still delivering that already-aligned
    // word. A sound version needs a deliberately BIT-MISALIGNED recurring
    // match with an observable showing the subsequent word boundary
    // re-aligns -- and this fixture cannot present one: `floppy_sector_data`
    // (paula_floppy.v:479-500) is a purely WORD-INDEXED combinational
    // multiplexer on `fifo_word_counter`, presenting one of a small fixed
    // set of whole 16-bit values per counter position, with no shift
    // register or sub-word/bit-rotated view of the stream anywhere in the
    // module -- see the header's REPAIR NOTE for the full reasoning. Kept as
    // a `note()`: the RTL's actual (word-aligned) recurring-match behaviour,
    // real and measured but not a conformance failure against this
    // sentence, since it is not the case the sentence rules out.
    // ========================================================================
    {
        // Measured for interest only, not graded: the fixture's own
        // behaviour at a recurring, already-word-aligned match (the only
        // kind this generator can ever produce -- see above). MARKER_PAIR
        // feeds 0x00 at payload pair-index 10, which the context-free
        // encoder turns into floppy_data 0xAAAA, the SAME value as the
        // DSKSYNC_LEADING preamble match, so it recurs by construction.
        // gated[30+MARKER_PAIR] is the marker's own delivered slot.
        int markerIdx = 30 + MARKER_PAIR;
        auto gated = trial(true, DSKSYNC_LEADING, MARKER_PAIR);
        checks.note("P-RESYNC: [recurring, word-aligned match] gated[" + std::to_string(markerIdx) +
                    "]=" + hex4(gated[markerIdx]) + " (MARKER_WORD=" + hex4(MARKER_WORD) +
                    ", FILLER_WORD=" + hex4(FILLER_WORD) + ") -- delivered as MARKER_WORD (ordinary data, "
                    "not excluded); trackrdok (paula_floppy.v:920-926) latches on the first match and cannot "
                    "reopen. This is NOT evidence against the cited sentence: the sentence never requires this "
                    "already-aligned recurring word to be omitted, and this fixture has no way to present the "
                    "bit-misaligned case that would actually test 'resync'.");
        checks.record(
            "opus:8-dsksync.c4 (p.250, 'During disk read DMA, the controller will resync every time the word "
            "match is found.'): declined, not graded. The sentence does not say a later, already-word-aligned "
            "matching word is omitted from delivery -- a controller can re-align its word boundary at a later "
            "match while still delivering that already-aligned word. A sound test needs a deliberately "
            "BIT-MISALIGNED recurring match with an observable showing the subsequent word boundary re-aligns. "
            "Checked directly against this fixture's own generator (paula_floppy.v:479-500) before declining: "
            "`floppy_sector_data` is a purely word-indexed combinational multiplexer on `fifo_word_counter`, "
            "presenting one of a small fixed set of whole 16-bit values per counter position -- no shift "
            "register, no sub-word or bit-rotated view of the MFM stream exists anywhere in this module, so no "
            "stimulus in this fixture's input space (SD-sector bytes fed via sdc_byte_in_strobe) can ever "
            "present a match at a bit-shifted, non-word-aligned position for sync_match to compare against. The "
            "case this claim, read as the validation demands, would need to test does not exist at this "
            "boundary -- unfalsifiable here, not merely undemonstrated.");
    }
    {
        checks.record(
            "sol:8-dsksync.c5 (p.250, twin of opus:8-dsksync.c4, candidate C10): declined, not graded, for the "
            "identical reason under its own uid -- same sentence, same word-indexed generator, same absence of "
            "any bit-misaligned recurring-match stimulus at this fixture's boundary.");
    }

    // ========================================================================
    // P-INTREQSET -- p.250, "the DSKSYNC bit in INTREQ is set when the
    // input stream matches the DSKSYNC register." Graded ONLY at this
    // package's boundary: `syncint`, the paula_floppy-side signal that
    // (per paula_intcontroller.v:130, disclosed by classify, not opened
    // here -- out of this package's assigned boundary) feeds that INTREQ
    // bit. Not a claim that INTREQ's own bit is set, which this fixture
    // cannot observe.
    // ========================================================================
    {
        bool mut = mutating("opus:8-dsksync.c6");
        uint16_t ds = mut ? DSKSYNC_NONE : DSKSYNC_ALT;
        int pulses = trial_syncint(true, ds, 300);
        checks.note("P-INTREQSET/opus:8-dsksync.c6: syncint pulses = " + std::to_string(pulses));
        checks.expect(pulses > 0,
            "opus:8-dsksync.c6 (p.250, 'In addition, the DSKSYNC bit in INTREQ is set when the input "
            "stream matches the DSKSYNC register.', graded at its paula_floppy half only -- syncint, the "
            "signal paula_intcontroller.v:130 OR's into INTREQ bit 12, outside this package's boundary): "
            "syncint pulses when the stream matches DSKSYNC");
    }
    {
        bool mut = mutating("sol:8-dsksync.c7");
        uint16_t ds = mut ? DSKSYNC_NONE : DSKSYNC_ALT;
        int pulses = trial_syncint(true, ds, 300);
        checks.expect(pulses > 0,
            "sol:8-dsksync.c7 (p.250, twin of opus:8-dsksync.c6, flagged out_of_slice by its own reader "
            "but crossread ok and named in this package's own claims file): same syncint-pulses "
            "measurement, graded independently under its own uid, at the same paula_floppy-only boundary");
    }

    // ========================================================================
    // P-INTREQINDEP -- p.250, "The DSKSYNC bit in INTREQ is independent of
    // the WORDSYNC enable." Graded as: syncint's pulse count for the same
    // matching stream is the same whether WORDSYNC is 0 or 1.
    // ========================================================================
    {
        bool mut = mutating("opus:8-dsksync.c7");
        // MUTATE: the wordsync=1 trial is given a non-matching DSKSYNC,
        // so the counts would differ for an unrelated reason (transfer
        // never runs long enough / never matches at all).
        uint16_t dsWs1 = mut ? DSKSYNC_NONE : DSKSYNC_ALT;
        int p0 = trial_syncint(false, DSKSYNC_ALT, 300);
        int p1 = trial_syncint(true, dsWs1, 300);
        checks.note("P-INTREQINDEP/opus:8-dsksync.c7: wordsync=0 pulses=" + std::to_string(p0) +
                    " wordsync=1 pulses=" + std::to_string(p1));
        checks.expect(p0 == p1 && p0 > 0,
            "opus:8-dsksync.c7 (p.250, 'The DSKSYNC bit in INTREQ is independent of the WORDSYNC enable.', "
            "graded at its paula_floppy half only -- syncint): the same matching stream produces the same "
            "nonzero syncint pulse count whether WORDSYNC is 0 or 1");
    }
    {
        bool mut = mutating("sol:8-dsksync.c8");
        uint16_t dsWs1 = mut ? DSKSYNC_NONE : DSKSYNC_ALT;
        int p0 = trial_syncint(false, DSKSYNC_ALT, 300);
        int p1 = trial_syncint(true, dsWs1, 300);
        checks.expect(p0 == p1 && p0 > 0,
            "sol:8-dsksync.c8 (p.250, twin of opus:8-dsksync.c7, flagged out_of_slice by its own reader "
            "but crossread ok and named in this package's own claims file): same independence measurement, "
            "graded independently under its own uid, at the same paula_floppy-only boundary");
    }

    // ========================================================================
    // P-APPA -- p.279, "[DSKSYNC] holds the match code for disk read "
    // "synchronization." Same underlying fact as P-WRITETARGET/P-SYNCREG,
    // cited from the appendix A register table.
    // ========================================================================
    {
        bool mut = mutating("opus:8-appA-dsksync.c5");
        auto ungated = trial(false, DSKSYNC_NONE, -1);
        uint16_t ds2 = mut ? DSKSYNC_LEADING : DSKSYNC_ALT;
        auto leading = trial(true, DSKSYNC_LEADING, -1);
        auto alt = trial(true, ds2, -1);
        checks.expect(leading[0] == ungated[1] && alt[0] == ungated[3] && leading[0] != alt[0],
            "opus:8-appA-dsksync.c5 (p.279, appendix A, '[DSKSYNC] holds the match code for disk read "
            "synchronization.'): the register's written content is the match code -- moving it moves the "
            "synchronization point, the same measurement as the chapter-body claims from a third citing "
            "page");
    }
    {
        bool mut = mutating("sol:8-appa-dsksync.c4");
        auto ungated = trial(false, DSKSYNC_NONE, -1);
        uint16_t ds2 = mut ? DSKSYNC_LEADING : DSKSYNC_ALT;
        auto leading = trial(true, DSKSYNC_LEADING, -1);
        auto alt = trial(true, ds2, -1);
        checks.expect(leading[0] == ungated[1] && alt[0] == ungated[3] && leading[0] != alt[0],
            "sol:8-appa-dsksync.c4 (p.279, twin of opus:8-appA-dsksync.c5; classify's own uid_repair note "
            "corrects the reader prefix on this record to sol, matching the ledger): same appendix-A "
            "match-code measurement, graded independently under its own uid");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}

// MUTATION A0-LIVENESS-KNOWN-DISTINCT-WORDS stimulus red: marker pair-index removed, so the two checked payload words are both filler (0xFFFF, 0xFFFF), indistinguishable (MUTATE=A0-LIVENESS-KNOWN-DISTINCT-WORDS).
// MUTATION opus:8-dma.c29 stimulus red: the gated trial's DSKSYNC redirected to a value absent from the stream, so nothing is delivered to compare (MUTATE=opus:8-dma.c29).
// MUTATION sol:8-dsksync.c1 stimulus red: same redirection as opus:8-dma.c29, independent uid (MUTATE=sol:8-dsksync.c1).
// MUTATION opus:8-adkcon.c19 stimulus red: the "gated" trial is run with wordsync=0 too, so there is no enable effect to detect (MUTATE=opus:8-adkcon.c19).
// MUTATION sol:8-adkcon.c15 stimulus red: same redirection as opus:8-adkcon.c19, independent uid (MUTATE=sol:8-adkcon.c15).
// MUTATION opus:8-adkcon.c20 stimulus red: the "never occurs" trial's DSKSYNC redirected to a value that DOES occur, so data is delivered where none was expected (MUTATE=opus:8-adkcon.c20).
// MUTATION sol:8-adkcon.c16 stimulus red: same redirection as opus:8-adkcon.c20, independent uid (MUTATE=sol:8-adkcon.c16).
// MUTATION opus:8-adkcon.c21 stimulus red: the second trial's written DSKSYNC redirected to equal the first trial's, so the two match points collapse to one (MUTATE=opus:8-adkcon.c21).
// MUTATION sol:8-adkcon.c17 stimulus red: same redirection as opus:8-adkcon.c21, independent uid (MUTATE=sol:8-adkcon.c17).
// MUTATION opus:8-dsksync.c1 stimulus red: same redirection as opus:8-adkcon.c21 (MUTATE=opus:8-dsksync.c1).
// MUTATION sol:8-dsksync.c2 stimulus red: same redirection as opus:8-adkcon.c21, independent uid (MUTATE=sol:8-dsksync.c2).
// MUTATION opus:8-dsksync.c2 stimulus red: the "never occurs" trial's DSKSYNC redirected to a value that DOES occur, so data is delivered where none was expected (MUTATE=opus:8-dsksync.c2).
// MUTATION sol:8-dsksync.c3 stimulus red: same redirection as opus:8-dsksync.c2, independent uid (MUTATE=sol:8-dsksync.c3).
// MUTATION opus:8-dsksync.c3 stimulus red: the reference switched from the match word's successor to the match word itself, so the "starts with the following word" check compares against the wrong word (MUTATE=opus:8-dsksync.c3).
// MUTATION sol:8-dsksync.c4 stimulus red: same redirection as opus:8-dsksync.c3, independent uid (MUTATE=sol:8-dsksync.c4).
// opus:8-dsksync.c4 -- DECLINED via record(), not expect(); no mutation record (not graded through expect()). Repair 2026-09-15: the previous stimulus-green substitute record is removed along with the graded check it defended -- see the header's REPAIR NOTE / P-RESYNC.
// sol:8-dsksync.c5 -- DECLINED via record(), not expect(); no mutation record, same reason as opus:8-dsksync.c4.
// MUTATION opus:8-dsksync.c6 stimulus red: DSKSYNC redirected to a value absent from the stream, so syncint never pulses (MUTATE=opus:8-dsksync.c6).
// MUTATION sol:8-dsksync.c7 stimulus red: same redirection as opus:8-dsksync.c6, independent uid (MUTATE=sol:8-dsksync.c7).
// MUTATION opus:8-dsksync.c7 stimulus red: the wordsync=1 trial's DSKSYNC redirected to a value absent from the stream, so its pulse count drops to zero while the wordsync=0 reference stays nonzero (MUTATE=opus:8-dsksync.c7).
// MUTATION sol:8-dsksync.c8 stimulus red: same redirection as opus:8-dsksync.c7, independent uid (MUTATE=sol:8-dsksync.c8).
// MUTATION opus:8-appA-dsksync.c5 stimulus red: same redirection as opus:8-adkcon.c21 (MUTATE=opus:8-appA-dsksync.c5).
// MUTATION sol:8-appa-dsksync.c4 stimulus red: same redirection as opus:8-adkcon.c21, independent uid (MUTATE=sol:8-appa-dsksync.c4).
