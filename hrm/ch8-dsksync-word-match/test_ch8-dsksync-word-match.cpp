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
//   the claim's own `classify.uid` for the named assertions still graded and
//   a plain uppercase id for A0, which carries no manual citation.
//   `opus:8-dsksync.c4`/`sol:8-dsksync.c5` (P-RESYNC), `opus:8-dma.c29`/
//   `sol:8-dsksync.c1` (P-ALIGN), `opus:8-dsksync.c6`/`sol:8-dsksync.c7`
//   (P-INTREQSET), and `opus:8-dsksync.c7`/`sol:8-dsksync.c8` (P-INTREQINDEP)
//   carry NO `expect()` and NO mutation record: see the REPAIR NOTES below
//   and the DECLINE record() beside each block.
//
//   RUN LOG (measured, WSL/Verilator 5.052, this repair session). Baseline
//   (MUTATE unset): 15 checks.expect() calls, 15 passed, 0 failed (14
//   uid-carrying assertions plus A0). MUTATE=A0-LIVENESS-KNOWN-DISTINCT-WORDS
//   was rerun against the REPAIRED pair and confirmed 14 passed / 1 failed,
//   the ONE FAIL being A0 itself -- the new single-write stimulus (dmaen
//   never arms) correctly turns the repaired pair red. Spot-checked three
//   further, unrelated-to-this-repair mutations (opus:8-adkcon.c19,
//   opus:8-dsksync.c3, opus:8-appA-dsksync.c5) after the `Rig::arm()`
//   signature change (new defaulted `double_write` parameter, used only by
//   A0): each still confirmed 14 passed / 1 failed, exactly its own targeted
//   label, no cross-contamination -- the signature change did not disturb
//   any other trial. See the report accompanying this commit for the full
//   per-assertion table.
//
// REPAIR NOTE / LOT C (2026-09-15, against DOKTRIN.md/LOS-C.md). Three more
//   withdrawals beyond P-RESYNC (below), all shape 2 of the repair doctrine
//   (claim graded elsewhere): opus:8-dma.c29/sol:8-dsksync.c1 (P-ALIGN),
//   opus:8-dsksync.c6/sol:8-dsksync.c7 (P-INTREQSET), and opus:8-dsksync.c7/
//   sol:8-dsksync.c8 (P-INTREQINDEP) are withdrawn to record() -- the
//   INTREQSET/INTREQINDEP pair because hrm/ch8-disk-interrupt-path now grades
//   the real INTREQ bit and minimig.v's own _cpu_ipl in the same run, where
//   this package could only ever reach syncint; P-ALIGN because it is
//   unfalsifiable at this boundary (see that block's own record()), the same
//   limitation P-RESYNC already established for the identical word-indexed
//   generator. A0's own known-word pair is REPAIRED, not withdrawn: the
//   validation's stated reason for dropping A0 was half right -- 0xFF ->
//   0xFFFF was this RTL's own non-conformant encoder output, not a
//   page-legal value, but 0x00 -> 0xAAAA IS exactly what p.250 requires, and
//   withdrawing it by reflex would have thrown away a correct half. The
//   repaired A0 uses the fixture's own two fixed structural words (0xAAAA
//   preamble, 0x4489 sync mark) instead of an engineered payload byte -- see
//   the A0 block's own comment for the full reasoning. `tools/
//   check_uid_placement.py` and `tools/check_no_claim_lost.py` verdicts on
//   this package are in the report accompanying this commit; no RTL was
//   touched, no debug port was added, and no oracle was adjusted by any of
//   this.
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
    // task's own disclosed measurement. `double_write=false` performs only
    // the first write, deliberately leaving dmaen unarmed -- a mutation
    // stimulus A0 uses below (the same single/double-write technique
    // hrm/ch8-disk-interrupt-requests's own DSKBLK trials use).
    void arm(uint16_t len_words, bool double_write = true) {
        d->enable = 1;
        write_reg(0x024, 0x8000 | len_words);
        if (double_write) write_reg(0x024, 0x8000 | len_words);
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
// `double_write=false` leaves DMA unarmed -- A0's own mutation stimulus.
std::vector<uint16_t> trial(bool wordsync, uint16_t dsksync, int markerPair, bool double_write = true) {
    Rig r;
    r.d->wordsync = wordsync ? 1 : 0;
    r.write_reg(0x07E, dsksync);
    r.arm(DSKLEN_BUDGET, double_write);
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
    // A0 -- FIXTURE DATA-PATH LIVENESS. No claim id, no manual page.
    // REPAIRED 2026-09-15 (repair lot C): the previous pair used engineered
    // payload bytes 0x00 -> 0xAAAA (correct: p.250's own "0 -> 10 ;if
    // following a 0" rule, applied to a steady run of zero bits, IS 0xAAAA)
    // and 0xFF -> 0xFFFF (WRONG as a page-legal value: p.250's "1 -> 01" is
    // unconditional, so a uniform 0xFF byte page-encodes to 0x5555, not
    // 0xFFFF -- 0xFFFF is only what this RTL's own simplified {clock=1,
    // data=bit} encoder happens to produce for a run of one-bits, never what
    // the printed rule requires). Withdrawing the whole pair would also have
    // withdrawn the CORRECT 0xAAAA half by reflex, which DOKTRIN.md/LOS-C.md
    // both warn against. The repair instead drops the payload-derived word
    // entirely and uses the fixture's own two FIXED structural words instead
    // -- the 0xAAAA preamble and the 0x4489 sync mark (both hardwired by
    // paula_floppy's generator regardless of payload, paula_floppy.v:
    // 481-500, 'word0,1 the fixed 0xaaaa preamble, word2,3 the fixed 0x4489
    // sync pair') -- neither derived from this RTL's own (non-conformant)
    // per-bit encoder: 0xAAAA is independently page-legal as shown above, and
    // 0x4489 is the manual's own named 'usual' MFM sync-mark value (per this
    // chapter's own classify record, 8-dsksync.c5/c6: 'DSKSYNC is typically
    // set to the MFM sync mark value $4489'), not merely an RTL default. This
    // is the SAME ungated read (wordsync=0) every other assertion below
    // already relies on -- P-WRITETARGET/P-SYNCREG/P-ALIGNSTART already use
    // ungated[0]=0xAAAA and ungated[1]=0x4489 as their own reference points,
    // confirmed empirically by their own (unchanged, still-passing) trials.
    // ========================================================================
    {
        bool mut = mutating("A0-LIVENESS-KNOWN-DISTINCT-WORDS");
        // MUTATE=A0-LIVENESS-KNOWN-DISTINCT-WORDS: DSKLEN written only once
        // instead of twice, so dmaen never arms and neither structural word
        // is ever delivered -- both reads stay at DSKDATR's stale/idle value,
        // indistinguishable.
        auto w = trial(false, DSKSYNC_NONE, -1, /*double_write=*/!mut);
        uint16_t first = w[0];   // fixed preamble word (structural, not payload-derived)
        uint16_t second = w[1];  // fixed sync-mark word (structural, not payload-derived)
        checks.note("A0: ungated read of the fixture's own fixed preamble/sync words -> first=" + hex4(first) +
                    " second=" + hex4(second) + (mut ? " (DMA armed only once, mutated)" : ""));
        checks.expect(first == 0xAAAA && second == 0x4489 && first != second,
            "A0-LIVENESS-KNOWN-DISTINCT-WORDS: two distinct, page-legal known words -- the fixture's own fixed "
            "preamble word (0xAAAA, the steady-state all-zero encoding p.250's own rule produces, independent "
            "of this RTL's per-bit encoder) and its fixed sync-mark word (0x4489, the value the manual itself "
            "names as the usual MFM sync mark, not merely an RTL default) -- read back via DSKDATR with "
            "wordsync=0 (ungated), before any match-based claim below is trusted. Repairs the previous pair, "
            "whose second word (0xFF -> 0xFFFF) was this RTL's own non-conformant encoder output, not a "
            "page-legal value (p.250's 'the 1 -> 01' rule makes the page-correct encoding 0x5555); the first "
            "word (0x00 -> 0xAAAA) was already page-legal and is carried forward unchanged, now as a "
            "structural word rather than an engineered payload byte");
    }

    // ========================================================================
    // P-ALIGN -- p.247, "Using DSKSYNC ... will guarantee word alignment of
    // the data." WITHDRAWN 2026-09-15 (repair lot C) for BOTH uids, same
    // reasoning as the already-declined P-RESYNC below: "guarantee word
    // alignment" reads, against the printed sentence, as a guarantee against
    // BIT-misaligned delivery -- but this fixture's own generator
    // (`floppy_data`/`sync_match`, paula_floppy.v:481-500, 914-915) is a
    // purely WORD-INDEXED combinational multiplexer with no shift register
    // and no sub-word or bit-rotated view of the stream anywhere in the
    // module. Every word it can ever present is already word-aligned by
    // construction, so a "stays word-aligned" measurement taken here cannot
    // distinguish "the mechanism the sentence describes is present" from
    // "nothing here could ever be misaligned regardless of any mechanism" --
    // unfalsifiable at this boundary, not merely undemonstrated, the exact
    // limitation this package's own P-RESYNC decline already cites for the
    // identical generator. Kept as a note(): the previously-graded
    // word-for-word-shifted measurement, real and unchanged, but not
    // conformance evidence for this sentence.
    // ========================================================================
    {
        auto ungated = trial(false, DSKSYNC_NONE, -1);
        auto gated = trial(true, DSKSYNC_LEADING, -1);
        bool aligned = true;
        for (int k = 0; k < 6; k++) if (gated[k] != ungated[k + 1]) aligned = false;
        checks.note("P-ALIGN (not graded): gated[0..5] = " + std::to_string(gated[0]) + "," +
                    std::to_string(gated[1]) + ",... vs ungated[1..6] -- aligned=" + std::to_string(aligned) +
                    "; this fixture's generator can never present anything else, see the record() below");
        checks.record(
            "opus:8-dma.c29 / sol:8-dsksync.c1 (p.247, 'Using the DSKSYNC register ... will guarantee word "
            "alignment of the data.'): declined, not graded. The claim, read as a guarantee against "
            "BIT-misaligned delivery, needs a stimulus that could otherwise land off a word boundary; this "
            "fixture's `floppy_data` generator (paula_floppy.v:481-500) is a purely word-indexed combinational "
            "multiplexer -- no shift register, no sub-word or bit-rotated view of the MFM stream exists "
            "anywhere in this module, so `sync_match` (:914-915) can only ever compare whole, already "
            "word-aligned words. No stimulus in this fixture's input space can present a bit-misaligned "
            "candidate for the claim to guard against -- unfalsifiable here, not merely undemonstrated, the "
            "identical limitation this package's own P-RESYNC decline (below) already established for the "
            "same generator. A sound version of this claim needs a boundary that can present a bit-level or "
            "sub-word stream, which paula_floppy does not expose.");
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
    // input stream matches the DSKSYNC register." WITHDRAWN 2026-09-15
    // (repair lot C) for BOTH uids: this package can reach only syncint, one
    // paula_floppy output port, never the real INTREQ bit. hrm/
    // ch8-disk-interrupt-path's own PROP-DSKSYNC-FULLJOIN trial (dsksync_trial,
    // uid opus:8-dsksync.c6) grades this claim in full -- the real INTREQ
    // bit 12 AND minimig.v's own _cpu_ipl at level 5, observed in the same
    // cycle as the stimulus. Kept as a note(): the local syncint measurement,
    // unchanged, no longer mutation-guarded (nothing grades it as a claim).
    // ========================================================================
    {
        int pulses = trial_syncint(true, DSKSYNC_ALT, 300);
        checks.note("P-INTREQSET (local half only): syncint pulses = " + std::to_string(pulses) +
                    " -- paula_floppy's own syncint still pulses on a real stream match, unchanged from "
                    "before this repair");
        checks.record(
            "opus:8-dsksync.c6 / sol:8-dsksync.c7 (p.250, 'In addition, the DSKSYNC bit in INTREQ is set "
            "when the input stream matches the DSKSYNC register.'): declined here, not graded -- this package "
            "can only reach syncint, one paula_floppy output port, not the real INTREQ bit 12 or minimig.v's "
            "_cpu_ipl level. hrm/ch8-disk-interrupt-path's own PROP-DSKSYNC-FULLJOIN trial grades both uids "
            "fully: a real stream match against DSKSYNC sets the real INTREQ bit 12 AND drives _cpu_ipl to the "
            "level-5 encoding, observed in the same cycle, in the same run as the stimulus. That is where "
            "these two uids are ASSERTED; this package's own local syncint pulse (noted above, unchanged) is "
            "real but does not, by itself, establish INTREQ's own bit being set.");
    }

    // ========================================================================
    // P-INTREQINDEP -- p.250, "The DSKSYNC bit in INTREQ is independent of
    // the WORDSYNC enable." WITHDRAWN 2026-09-15 (repair lot C) for BOTH
    // uids, same reasoning as P-INTREQSET above: hrm/ch8-disk-interrupt-path's
    // own PROP-DSKSYNC-INTREQINDEP trial (intreqindep_trial, uid
    // opus:8-dsksync.c7 -- a DIFFERENT claim from sol:8-dsksync.c7 above,
    // despite the shared numeric suffix; confirmed against that package's
    // own header note before relying on it) grades this claim in full at
    // the real INTREQ bit and _cpu_ipl, under both WORDSYNC settings.
    // ========================================================================
    {
        int p0 = trial_syncint(false, DSKSYNC_ALT, 300);
        int p1 = trial_syncint(true, DSKSYNC_ALT, 300);
        checks.note("P-INTREQINDEP (local half only): wordsync=0 pulses=" + std::to_string(p0) +
                    " wordsync=1 pulses=" + std::to_string(p1) + " -- paula_floppy's own syncint still "
                    "pulses the same count either way, unchanged from before this repair");
        checks.record(
            "opus:8-dsksync.c7 / sol:8-dsksync.c8 (p.250, 'The DSKSYNC bit in INTREQ is independent of the "
            "WORDSYNC enable.'): declined here, not graded -- this package can only reach syncint, not the "
            "real INTREQ bit or minimig.v's _cpu_ipl level. hrm/ch8-disk-interrupt-path's own PROP-DSKSYNC-"
            "INTREQINDEP trial grades both uids fully: the same real stream match sets the real INTREQ bit 12 "
            "and drives _cpu_ipl to level 5, in the same cycle, whether WORDSYNC is 0 or 1. That is where "
            "these two uids are ASSERTED; this package's own local syncint-pulse-count equality (noted above, "
            "unchanged) is real but does not, by itself, establish INTREQ's own bit being set independently "
            "of WORDSYNC.");
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

// MUTATION A0-LIVENESS-KNOWN-DISTINCT-WORDS stimulus red: DSKLEN written only once instead of twice (repair 2026-09-15: the pair itself was replaced, see the header's A0 note above), so dmaen never arms and neither structural word is ever delivered, indistinguishable (MUTATE=A0-LIVENESS-KNOWN-DISTINCT-WORDS).
// opus:8-dma.c29 / sol:8-dsksync.c1 -- DECLINED via record(), not expect(); no mutation record (not graded through expect()). Repair 2026-09-15: withdrawn as unfalsifiable at this boundary -- see the header's REPAIR NOTE / P-ALIGN and the record() beside the P-ALIGN block.
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
// opus:8-dsksync.c6 / sol:8-dsksync.c7 (P-INTREQSET) -- DECLINED via record(), not expect(); no mutation record. Repair 2026-09-15 (repair lot C): the claim is now graded in full at hrm/ch8-disk-interrupt-path -- see the record() beside the P-INTREQSET block.
// opus:8-dsksync.c7 / sol:8-dsksync.c8 (P-INTREQINDEP) -- DECLINED via record(), not expect(); no mutation record, same reason -- see the record() beside the P-INTREQINDEP block. (opus:8-dsksync.c7 here is a DIFFERENT claim from sol:8-dsksync.c7 above, despite the shared numeric suffix.)
// MUTATION opus:8-appA-dsksync.c5 stimulus red: same redirection as opus:8-adkcon.c21 (MUTATE=opus:8-appA-dsksync.c5).
// MUTATION sol:8-appa-dsksync.c4 stimulus red: same redirection as opus:8-adkcon.c21, independent uid (MUTATE=sol:8-appa-dsksync.c4).
