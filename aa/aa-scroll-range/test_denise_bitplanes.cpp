// aa-scroll-range -- denise_bitplanes conformance test
//
// Test purpose: p.31 of the AA specification states that all playfield
//   scrolling is now in 35ns increments, then prints a table, "SCROLL RANGE
//   AS PROGRAMMED IN BPLCON1", whose 1X LORES row reads "0-15" lores pixels
//   and "0-63" 35ns steps for the same programmable range. This grades both:
//   that BPLCON1's scroll field moves the displayed bit by no more than one
//   28MHz clock at a time, and that the field LORES mode actually reaches
//   spans 16 lores-pixel positions expressed one way and 64 35ns-step
//   positions expressed the other way -- the same six bits of range, read at
//   two different granularities.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF page 31, the paragraph "All
//   playfield scrolling is now in 35ns increments" and the LORES row of the
//   "SCROLL RANGE AS PROGRAMMED IN BPLCON1" table. The document prints no
//   page numbers of its own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the HIRES or SHRES rows of the same
//   table, the 2X/4X blocks, sprites, what a display shows once bitplane
//   data reaches denise's colour table, or anything about the document's
//   own title page or revision list. See the DECLINED records at the bottom
//   for the full accounting of the lot's other 244 records.
//
// WHY LORES AND NOT SHRES OR HIRES, EVEN FOR THE GENERAL 35NS CLAIM. The
//   classified lot declines 33 claims -- the HIRES and SHRES table rows --
//   with this note: "Measured at one sample per clock, the scroll nibble
//   moves the origin not at all with `hires` or `shres` raised -- which is
//   this fixture's limit, not a reading of the module". That is exactly
//   right, and worth explaining once rather than rediscovering: pf1h latches
//   from bplcon1 the instant `load` pulses (denise_bitplanes.v:207-209), but
//   pf1h_del -- the value the shifter actually reads -- is a SEPARATE
//   register that only catches up one 7MHz cycle later (:211-214). Between
//   those two edges, `select` inside denise_bitplane_shifter briefly reads
//   the OLD scroll value while the shift register is already carrying the
//   NEW word, and in shres mode (shift held at 1'b1 every clock,
//   denise_bitplane_shifter.v:208) that stale window is long enough relative
//   to the one-tap-per-clock transit that a naive single read of `select`
//   can land on either the old tap or a spurious extra one -- probed by hand
//   outside this package (a --public Verilator build of denise_bitplanes,
//   not committed here) and confirmed: without settling, denise_bitplanes'
//   own bpldata pulse for two different scroll values can land at the SAME
//   first-lit clock, or even show up twice. A fixture that waited out that
//   settle window could probably read shres too, but that is a claim about a
//   DIFFERENT table row than the eight this package's lot marks testable,
//   and every one of those eight is gradable in lores alone: the general
//   35ns claim (c1) names no mode, and the LORES row (c3/c4/c5) names lores
//   by definition. So every assertion below drives hires=0, shres=0, and
//   never needs to characterise the settle window at all.
//
// THE SIX BITS THAT MOVE THE ORIGIN IN LORES MODE. pf1h is formed as
//   {bplcon1[11:10], bplcon1[3:0], bplcon1[9:8]} (denise_bitplanes.v:208).
//   In lores mode (fmode fixed at 0 throughout -- this package never writes
//   FMODE) denise_bitplane_shifter computes
//     select[5:0] = scroll[7:2] & 6'b00_1111        (:217, lores branch)
//     sh_select   = {aga, scroll[1:0]}               (:254, lores branch)
//   which keeps pf1h[7:6] (bplcon1[11:10]) out of the picture entirely and
//   leaves exactly six live bits: bplcon1[3:0] picks one of 16 taps in the
//   64-deep `scroller` (:238, shifts once per lores pixel via `shift` at
//   :216), and bplcon1[9:8] picks one of 4 taps in the 8-deep `sh_scroller`
//   (:259-261, which shifts every 28MHz clock regardless of mode). One full
//   turn of the fine 2 bits is one coarse tap, so the combined 6-bit field
//   F = (bplcon1[3:0] << 2) | bplcon1[9:8] is a single linear count from 0
//   to 63, one 28MHz clock (35ns) per unit -- measured directly below, not
//   assumed.
//
// WHY EVERY WRITE ZEROES PLANES 2 TO 8. Only plane 1's scroll (pf1h_del) is
//   under test; the even planes use pf2h_del, formed from a different slice
//   of the same register. Writing them to 0x0000 keeps `or_all`-style
//   confusion out of a single-plane read, the same discipline
//   aa-bitplane-data-registers uses for the same UUT.
//
// SAMPLING. The evidence this lot gives every one of its eight testable
//   records is the same sentence: "sample bpldata once per 28 MHz clock to
//   find where the first lit pixel lands". first_lit_plane1() below does
//   exactly that -- one dut->eval()-driving tick() per sample, no skipping
//   to a fixed phase -- which is coarser-grained than
//   aa-bitplane-data-registers' one-sample-per-pixel helper on purpose: this
//   package's claims are ABOUT the sub-pixel step, so the pixel-rate
//   sampling that package uses would hide the very thing under test.

#include "Vdenise_bitplanes.h"
#include "verilated.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {
// Per-assertion mutation selector, the same form the rest of the suite uses
// (e.g. hrm/ch8-adkcon-disk-control-bits): MUTATE names ONE assertion's own
// label, and only that assertion's stimulus site changes. Unlike a single
// on/off flag, this makes every OTHER assertion a live control on each run:
// a mutation record for one label is only good evidence if the other two
// assertions stayed green while it was selected, and a boolean flag cannot
// show that because it reddens all three at once.
const char* g_mutate = nullptr;
inline bool mutating(const char* id) {
    return g_mutate != nullptr && std::strcmp(g_mutate, id) == 0;
}
}  // namespace

static std::string hex(uint32_t v) {
    std::ostringstream o;
    o << "0x" << std::hex << std::uppercase << v;
    return o.str();
}

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

// Word addresses as denise_bitplanes.v declares them.
constexpr int A_BPLCON1 = 0x102 >> 1;
constexpr int A_BPL1DAT = 0x110 >> 1;
constexpr int A_BPLXDAT[9] = {0, 0x110 >> 1, 0x112 >> 1, 0x114 >> 1, 0x116 >> 1,
                              0x118 >> 1, 0x11a >> 1, 0x11c >> 1, 0x11e >> 1};
constexpr int A_IDLE = 0x000;

static inline uint8_t plane_bit(int k) { return (uint8_t)(1u << (k - 1)); }

class Harness {
public:
    Harness() : dut(new Vdenise_bitplanes) { reset(); }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->c1 = (phase == 2 || phase == 3) ? 1 : 0;
        dut->c3 = (phase == 0 || phase == 3) ? 1 : 0;
        dut->clk7_en = (phase == 1) ? 1 : 0;
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        phase = (phase + 1) % 4;
    }

    void reset() {
        phase = 0;
        dut->clk = 0;
        dut->reset = 1;
        dut->aga = 1;
        dut->hires = 0;
        dut->shres = 0;
        dut->hpos = 0;          // fixes extra_delay_r's hpos-dependent tables
        dut->chip48 = 0;        // fmode stays 0 after reset -> chip48_fmode = 0
        dut->reg_address_in = A_IDLE;
        dut->data_in = 0;
        for (int i = 0; i < 16; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; ++i) tick();
    }

    // Hold the address across the phase-1 edge, the only edge this module's
    // register writes are gated on.
    void write(int addr, uint16_t value) {
        while (phase != 1) tick();
        dut->reg_address_in = addr;
        dut->data_in = value;
        tick();
        dut->reg_address_in = A_IDLE;
        dut->data_in = 0;
        while (phase != 1) tick();
    }

    // The evidence, verbatim: write BPLCON1, then load a word at BPL1DAT.
    // Planes 2-8 are silenced first so only plane 1's scroll is live.
    void prime(uint16_t bplcon1_value, uint16_t bpl1dat_value) {
        write(A_BPLCON1, bplcon1_value);
        for (int k = 2; k <= 8; ++k) write(A_BPLXDAT[k], 0x0000);
        write(A_BPL1DAT, bpl1dat_value);
    }

    // Sample bpldata once per 28MHz clock, starting from whatever phase
    // prime() left the harness in, and return the index of the first sample
    // where plane 1 is lit -- the sub-pixel origin the evidence asks for.
    // -1 if it never lights within the window.
    int first_lit_plane1(int window) {
        for (int i = 0; i < window; ++i) {
            tick();
            if (dut->bpldata & plane_bit(1)) return i;
        }
        return -1;
    }

    Vdenise_bitplanes* dut;
    int phase = 0;
};

// pf1h's fine field is bplcon1[9:8]; its coarse field is bplcon1[3:0].
// bplcon1[11:10] never reaches lores-mode select or sh_select and is left 0.
static inline uint16_t bplcon1_for_coarse_fine(int coarse4, int fine2) {
    return (uint16_t)((coarse4 & 0xF) | ((fine2 & 0x3) << 8));
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks c;

    std::cout << "aa-scroll-range -- AA_CONFORMANCE at denise_bitplanes\n";
    if (g_mutate)
        std::cout << "MUTATE=" << g_mutate
                   << ": exactly the named assertion below must go red, the "
                      "other two must stay green\n";

    constexpr uint16_t BPL1DAT_PULSE = 0x8000;  // MSB only: a single 1 among
                                                 // 64 shifted bits (data16 is
                                                 // the top 16, chip48_fmode
                                                 // is fixed 0 -- see header)

    // --- 1. The finest step the field has is one 28MHz clock ---------------
    // F=0 and F=1 differ only in bplcon1[9:8], the fine field's own LSB: the
    // smallest unit the whole 6-bit scroll field can be moved by. If "all
    // playfield scrolling is now in 35ns increments" is true, that smallest
    // unit must move the sampled origin by exactly one clock -- not zero
    // (unreachable resolution) and not two or more (a coarser floor).
    {
        Harness h0;
        h0.prime(bplcon1_for_coarse_fine(0, 0), BPL1DAT_PULSE);
        const int origin0 = h0.first_lit_plane1(40);

        Harness h1;
        // MUTATION SCROLLING-IS-IN-35NS-STEPS stimulus red: selecting this
        // assertion's own id primes the second trial with the SAME scroll
        // field (0) instead of the next one up, so the two origins are
        // identical and the step size this assertion measures collapses to
        // zero. MEASURED (WSL/Verilator 5.052, run alone against this id):
        // origin 0 -> 9, origin 1 -> 9, step 0 (baseline: 9 and 10, step 1)
        // -- FAIL SCROLLING-IS-IN-35NS-STEPS, PASS LORES-SCROLL-SPANS-
        // SIXTEEN-LORES-PIXELS, PASS LORES-SCROLL-SPANS-SIXTY-FOUR-35NS-
        // STEPS, summary "2 passed, 1 failed". Only the named assertion
        // moved; the other two stayed on their real stimulus and green.
        const int fine1 = mutating("SCROLLING-IS-IN-35NS-STEPS") ? 0 : 1;
        h1.prime(bplcon1_for_coarse_fine(0, fine1), BPL1DAT_PULSE);
        const int origin1 = h1.first_lit_plane1(40);

        const int step = (origin0 >= 0 && origin1 >= 0) ? (origin1 - origin0) : -999;
        c.note("BPLCON1 scroll field 0 -> origin " + std::to_string(origin0) +
               "; field 1 -> origin " + std::to_string(origin1) +
               "; step " + std::to_string(step) + " clock(s)");
        c.expect(step == 1,
                 "SCROLLING-IS-IN-35NS-STEPS: the smallest step BPLCON1's "
                 "scroll field can be moved by shifts the first lit pixel by "
                 "exactly one 28MHz clock, which is the 35ns the page names "
                 "[p.31] [claims: sol:AA-scroll.c1 opus:AA-scroll.c1]");
    }

    // --- 2. LORES spans 16 lores-pixel positions ----------------------------
    // Sweep bplcon1[3:0] (the coarse field alone, fine field held at 0)
    // across its full 0-15 range and confirm 16 DISTINCT origins spaced
    // exactly one lores pixel (4 clocks) apart -- the table's "0-15" column
    // for the 1X LORES row, read in lores-pixel units.
    {
        int origin[16];
        bool ok = true;
        for (int v = 0; v <= 15; ++v) {
            // MUTATION LORES-SCROLL-SPANS-SIXTEEN-LORES-PIXELS stimulus red:
            // selecting this assertion's own id primes every trial with
            // coarse field 0 instead of the sweep index, so all sixteen
            // writes converge on the same tap and the range this assertion
            // measures collapses to a single position. MEASURED (WSL/
            // Verilator 5.052, run alone against this id): all sixteen
            // origins read 9 (baseline: 9, 13, 17, ..., 69, step 4 apart)
            // -- FAIL LORES-SCROLL-SPANS-SIXTEEN-LORES-PIXELS, PASS
            // SCROLLING-IS-IN-35NS-STEPS, PASS LORES-SCROLL-SPANS-SIXTY-
            // FOUR-35NS-STEPS, summary "2 passed, 1 failed". Only the named
            // assertion moved; the other two stayed on their real stimulus
            // and green.
            const int coarse = mutating("LORES-SCROLL-SPANS-SIXTEEN-LORES-PIXELS") ? 0 : v;
            Harness h;
            h.prime(bplcon1_for_coarse_fine(coarse, 0), BPL1DAT_PULSE);
            origin[v] = h.first_lit_plane1(90);
            if (v > 0) {
                if (origin[v] < 0 || origin[v - 1] < 0 ||
                    origin[v] - origin[v - 1] != 4)
                    ok = false;
            }
        }
        std::ostringstream o;
        for (int v = 0; v <= 15; ++v) o << (v ? ", " : "") << origin[v];
        c.note("origin per coarse field value 0..15: " + o.str());
        c.expect(ok,
                 "LORES-SCROLL-SPANS-SIXTEEN-LORES-PIXELS: BPLCON1's coarse "
                 "scroll field takes all 16 values 0 to 15, and each one "
                 "moves the first lit pixel by exactly one more lores pixel "
                 "(4 clocks) than the value below it, which is the LORES "
                 "row's \"0-15\" column of \"SCROLL RANGE AS PROGRAMMED IN "
                 "BPLCON1\" [p.31] [claims: sol:AA-scroll.c3 sol:AA-scroll.c4 "
                 "opus:AA-scroll.c3 opus:AA-scroll.c4]");
    }

    // --- 3. The same range, read in 35ns steps, spans 0 to 63 --------------
    // Sweep the full combined field F = (coarse<<2)|fine across 0-63 and
    // confirm 64 DISTINCT origins spaced exactly one clock (35ns) apart --
    // the same LORES row's "0-63" column, read at the finer granularity.
    // Reaching all 64 needs the fine field: assertion 2 already shows the
    // coarse field alone reaches only 16 positions.
    {
        int origin[64];
        bool ok = true;
        for (int f = 0; f <= 63; ++f) {
            const int coarse = (f >> 2) & 0xF;
            // MUTATION LORES-SCROLL-SPANS-SIXTY-FOUR-35NS-STEPS stimulus red:
            // selecting this assertion's own id zeroes the fine field
            // on every trial instead of taking it from F, so only the
            // coarse 16 positions are ever reached and four consecutive F
            // values in every group of four collapse onto the same origin
            // instead of advancing one clock at a time. MEASURED (WSL/
            // Verilator 5.052, run alone against this id): origin at F=0 ->
            // 9, at F=63 -> 69, span 60 over 64 values (baseline: 9 and 72,
            // span 63) -- FAIL LORES-SCROLL-SPANS-SIXTY-FOUR-35NS-STEPS,
            // PASS SCROLLING-IS-IN-35NS-STEPS, PASS LORES-SCROLL-SPANS-
            // SIXTEEN-LORES-PIXELS, summary "2 passed, 1 failed". Only the
            // named assertion moved; the other two stayed on their real
            // stimulus and green.
            const int fine = mutating("LORES-SCROLL-SPANS-SIXTY-FOUR-35NS-STEPS") ? 0 : (f & 0x3);
            Harness h;
            h.prime(bplcon1_for_coarse_fine(coarse, fine), BPL1DAT_PULSE);
            origin[f] = h.first_lit_plane1(100);
            if (f > 0) {
                if (origin[f] < 0 || origin[f - 1] < 0 ||
                    origin[f] - origin[f - 1] != 1)
                    ok = false;
            }
        }
        c.note("origin at F=0: " + std::to_string(origin[0]) + "; at F=63: " +
               std::to_string(origin[63]) + "; span " +
               std::to_string(origin[63] - origin[0]) + " clock(s) over 64 values");
        c.expect(ok,
                 "LORES-SCROLL-SPANS-SIXTY-FOUR-35NS-STEPS: the combined "
                 "6-bit scroll field BPLCON1 programs for 1X LORES takes all "
                 "64 values 0 to 63, each one moving the first lit pixel by "
                 "exactly one more 28MHz clock (35ns) than the value below "
                 "it, which is the LORES row's \"0-63\" column of \"SCROLL "
                 "RANGE AS PROGRAMMED IN BPLCON1\" [p.31] "
                 "[claims: sol:AA-scroll.c5 opus:AA-scroll.c5]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary (not-asserted): The claim is about "
             "what a display shows -- a resolution, how many colours a "
             "bitplane count yields, what bandwidth that needs, "
             "hold-and-modify, extra-half-brite, dual playfield or the "
             "palette. This boundary puts one plane's bits out in series; "
             "the colours are chosen in denise's colour table and the "
             "bandwidth is a memory-system property no port here carries. "
             "[claims: opus:AA-hires.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/"
             "c13/c14/c15/c16/c17/c18 opus:AA-key.c1/c2/c3 "
             "opus:AA-lores.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12 "
             "opus:AA-palette.c1 "
             "opus:AA-superhires.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/"
             "c14/c15/c16/c17/c18/c19/c20/c21/c22/c23/c24 "
             "opus:AA-vga.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/"
             "c16/c17/c18/c19/c20/c21/c22/c23/c24 "
             "sol:AA-bandwidth.c1/c2/c3/c4/c5/c6 "
             "sol:AA-hires.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/"
             "c15/c16/c17/c18/c19 sol:AA-lisa-modes.c1/c2 "
             "sol:AA-lores.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13 "
             "sol:AA-superhires.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/"
             "c14/c15/c16/c17/c18/c19/c20/c21/c22/c23/c24/c25/c26 "
             "sol:AA-vga.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/"
             "c16/c17/c18/c19/c20/c21/c22/c23/c24/c25/c26/c27]");
    c.record(
             "DECLINED at this boundary (not-asserted): The claim is a row "
             "of the scroll table this package does not reach: the HIRES "
             "and SHRES rows, or the 2X and 4X blocks. Measured at one "
             "sample per clock, the scroll nibble moves the origin not at "
             "all with `hires` or `shres` raised -- which is this fixture's "
             "limit, not a reading of the module: in those modes the "
             "serialiser consumes a word faster and the extra delay depends "
             "on the data-fetch alignment, which this package drives "
             "nowhere. A red here would be a statement about the fixture. "
             "See the header for the full explanation and how assertion 1 "
             "still grades the page's general 35ns claim without touching "
             "either mode. [claims: opus:AA-scroll.c6/c7/c8/c9/c10/c11/c12/"
             "c13/c14/c15/c16/c17/c18/c19/c20/c21 "
             "sol:AA-scroll.c2/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/"
             "c18/c19/c20/c21]");
    c.record(
             "DECLINED at this boundary (not-asserted): The claim is about "
             "sprites -- their widths, the modes they can be displayed in, "
             "attaching them, or which bank of colours they take. Sprites "
             "are fetched in agnus_spritedma and shifted in denise_sprites; "
             "this module handles bitplanes. [claims: "
             "opus:AA-sprites.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10 "
             "sol:AA-sprites.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13]");
    c.record(
             "DECLINED at this boundary (not-asserted): The claim is about "
             "the document itself: its title, its date, the word "
             "PRELIMINARY, or a line of its revision list describing what "
             "an earlier version said. [claims: opus:AA-title.c1/c2/c3 "
             "sol:AA-title.c1/c2/c3]");
    c.record(
             "DECLINED at this boundary (not-asserted): The claim is about "
             "a register this module does not decode. BPLCON4's "
             "sprite-mask fields, which the revision line gives a reset "
             "value, are held in denise itself. [claims: "
             "opus:AA-revlist.c1/c2 sol:AA-revision.c1/c2]");
    c.record(
             "DECLINED at this boundary (not-asserted): The claim is about "
             "the chips this document replaces -- what scrolling was "
             "before AA, or what could not be done in an ECS mode. This "
             "design is the AA side; a statement about the older silicon "
             "has nothing here to fail against. [claims: opus:AA-scroll.c2 "
             "opus:AA-sprites.c9 sol:AA-sprites.c12]");
    c.record(
             "DECLINED at this boundary (not-asserted): two single-claim "
             "records. sol:AA-legends.c1 defines what the register list's "
             "`h`/`p` marks mean, and the pages this slice owns carry no "
             "marked register for it to apply to. sol:AA-legends.c2 is the "
             "`~` mark: an address the Copper may not write unless COPCON "
             "bit 1 is set, which is agnus_copper's gating of the copper "
             "write path -- denise_bitplanes neither decodes COPCON nor "
             "sees the Copper. [claims: sol:AA-legends.c1 sol:AA-legends.c2]");

    std::cout << "\nAssertions: " << c.passed << " passed, " << c.failed
              << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
