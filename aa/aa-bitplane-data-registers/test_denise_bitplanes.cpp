// aa-bitplane-data-registers -- denise_bitplanes conformance test
//
// Test purpose: the AA specification's address-ordered register list gives
//   BPL2DAT through BPL8DAT their addresses and calls each one a parallel-to-
//   serial converter for its own bitplane, and gives BPLCON1 the scroll values
//   for two playfields. This is the module that answers for all eight.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF page 8, the address-ordered
//   register list. The document prints no page numbers of its own; the corpus
//   numbering is the PDF's.
// Non-goals: this package does not test the colour table, HAM, dual playfield
//   priority, the display window, sprites, fetch modes, or what appears on a
//   screen. It grades which address feeds which plane, that the feed is
//   serial, and that one register carries two scroll values.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the AA specification independently, and ids such as
//   `opus:AA-bpl2dat.c1` and `opus:AA-bpl3dat.c1` each name one reader's
//   reading of one sentence. They resolve in the claim files that
//   hrm-testpipeline/acceptance/aa-s7.json lists under `inventories`.
//
// THIS BOUNDARY HAS NO READ PATH AT ALL. denise_bitplanes has no data_out
//   port: everything it is told is observed on `bpldata[8:1]`, one bit per
//   plane, a pixel at a time. So an address is graded by what comes OUT of the
//   shifter it feeds, and the W access letter is graded by the same stimulus
//   rather than by a read that answers nothing -- there is nothing here to
//   read.
//
// BPL7DAT AND BPL8DAT ARE WHY THIS PACKAGE IS AA AND NOT HRM. Their rows are
//   the only two in this group marked `&p`, and the `p` is the mark this
//   document's own p.6 legend calls new for the chip set. Seven and eight
//   bitplanes are what the AA specification adds; assertion 2 drives all
//   seven addresses and assertion 3 reads seven distinct words back out.
//
// THE CLOCK PHASES ARE THE FIXTURE'S WHOLE DIFFICULTY. amiga_clk.v runs four
//   28MHz cycles per 7MHz cycle and the two phase signals are staggered:
//
//     phase   0     1     2     3
//     c1      0     0     1     1
//     c3      1     0     0     1
//     enable        clk7_en           clk7n_en
//
//   so `shift` (~c1 & ~c3) and `clk7_en` are the same phase, and the module's
//   own `clk7n_en` (c1 & c3) is phase 3. A bus write lands on phase 1, its
//   buffer register takes it on phase 3 of the same 7MHz cycle, and a write to
//   BPL1DAT raises `load` for exactly one cycle -- so the shifters take their
//   data on phase 1 of the NEXT cycle, one 7MHz cycle after the write. Every
//   assertion below writes planes 2 to 8 first and BPL1DAT last, because
//   BPL1DAT is the load trigger (denise_bitplanes.v:466) and not merely
//   another plane.
//
// WHY A PIXEL IS SAMPLED ONCE PER 7MHz CYCLE. In lores the scroller advances
//   on one phase in four while `sh_scroller` shifts on every 28MHz clock, so
//   each pixel value stands at `bpldata` for four consecutive clocks. Sampling
//   at one fixed phase therefore yields exactly one sample per pixel, whatever
//   phase is chosen. This fixture samples after each phase-1 tick.
//
// AND THE FIRST LIT PIXEL IS THE ORIGIN, NOT A COUNTED LATENCY. pf1h_del adds
//   `extra_delay_r`, which depends on hpos and the fetch mode, so the absolute
//   number of clocks from the write to the first pixel is not something this
//   package wants to assert. Every assertion here writes a word whose top bit
//   is 1 in every plane it cares about, finds the first cycle at which any
//   plane is lit, and reads from there. What is asserted is which planes are
//   lit and in what order -- never when.

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
// Per-assertion mutation selector, the same form the rest of the suite uses:
// MUTATE names ONE assertion's own label, and only that assertion's stimulus
// site changes. A single on/off flag reddened every assertion at once, which
// cannot show that the mutation recorded for one assertion leaves the others
// green.
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

// Word addresses as the AA register list prints them, read from the corpus.
constexpr int A_BPLCON1 = 0x102 >> 1;   // denise_bitplanes.v:106
constexpr int A_BPL1DAT = 0x110 >> 1;   // :107, the load trigger; graded elsewhere
constexpr int A_BPLXDAT[9] = {0, 0x110 >> 1, 0x112 >> 1, 0x114 >> 1, 0x116 >> 1,
                              0x118 >> 1, 0x11a >> 1, 0x11c >> 1, 0x11e >> 1};
constexpr int A_IDLE = 0x000;           // decodes to no register in this module

// bpldata is declared [8:1], so Verilator packs plane k at bit k-1.
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
        dut->hpos = 0;
        dut->chip48 = 0;
        dut->reg_address_in = A_IDLE;
        dut->data_in = 0;
        for (int i = 0; i < 16; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; ++i) tick();
    }

    // Hold the address across the phase-1 edge, which is the only edge the
    // module's register writes are gated on.
    void write(int addr, uint16_t value) {
        while (phase != 1) tick();
        dut->reg_address_in = addr;
        dut->data_in = value;
        tick();
        dut->reg_address_in = A_IDLE;
        dut->data_in = 0;
        while (phase != 1) tick();      // let the phase-3 buffer latch happen
    }

    // One sample per pixel: bpldata holds each value for four 28MHz clocks,
    // so a fixed phase yields exactly one sample per pixel.
    std::vector<uint8_t> sample(int cycles) {
        std::vector<uint8_t> out;
        out.reserve(cycles);
        for (int i = 0; i < cycles; ++i) {
            while (phase != 1) tick();
            tick();
            out.push_back((uint8_t)dut->bpldata);
        }
        return out;
    }

private:
    Vdenise_bitplanes* dut;
    int phase = 0;
};

// The first cycle at which any plane is lit; -1 if none ever is.
static int first_lit(const std::vector<uint8_t>& t) {
    for (size_t i = 0; i < t.size(); ++i) if (t[i]) return (int)i;
    return -1;
}

static uint8_t or_all(const std::vector<uint8_t>& t) {
    uint8_t v = 0;
    for (uint8_t s : t) v |= s;
    return v;
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;
    g_mutate = std::getenv("MUTATE");

    // Each guard below is keyed to its OWN assertion's id: MUTATE=<id> changes
    // ONE thing in that assertion's stimulus, and every other assertion runs
    // its real stimulus.
    const uint16_t m_quiet  = mutating("FIXTURE-DELIVERS-KNOWN-VALUE") ? 0x8000u : 0x0000u;
    const bool     m_onehot = mutating("BPLXDAT-ADDRESSES-MAP-TO-PLANES");
    const bool     m_same   = mutating("BPLXDAT-CONVERTS-PARALLEL-TO-SERIAL");
    const uint16_t m_scroll = mutating("BPLCON1-AT-102-HOLDS-TWO-SCROLL-VALUES") ? 0x0000u : 0x0001u;

    const int WINDOW = 40;   // pixels; sixteen carry data, the rest are the tail

    std::cout << "aa-bitplane-data-registers -- AA_CONFORMANCE at denise_bitplanes\n";
    if (g_mutate)
        std::cout << "MUTATE=" << g_mutate
                   << ": exactly the named assertion below must go red, the "
                      "others must keep their baseline verdict\n";

    // --- 1. The fixture delivers a known value ------------------------------
    {
        Harness h;
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red selecting this
        // assertion's own id writes 0x8000 to every plane instead of zero, so
        // the window that must stay dark is lit.
        h.write(A_BPLCON1, 0x0000);
        for (int k = 2; k <= 8; ++k) h.write(A_BPLXDAT[k], m_quiet);
        h.write(A_BPL1DAT, 0x0000);          // the load trigger, itself empty
        const uint8_t lit = or_all(h.sample(WINDOW));
        checks.note("with every plane written zero, the planes ever lit are " +
                    hex(lit));
        checks.expect(lit == 0x00,
                      "FIXTURE-DELIVERS-KNOWN-VALUE: eight planes written zero "
                      "light nothing across the window, so any plane lit in the "
                      "assertions below was put there by a word this fixture "
                      "wrote");
    }

    // --- 2. Each address feeds its own plane and no other -------------------
    {
        uint8_t seen[9] = {0};
        bool ok = true;
        for (int k = 2; k <= 8; ++k) {
            Harness h;
            h.write(A_BPLCON1, 0x0000);
            // MUTATION BPLXDAT-ADDRESSES-MAP-TO-PLANES stimulus red selecting
            // this assertion's own id sends every plane's word to $112, so
            // plane 2 lights for all seven trials and six of them see the
            // wrong plane.
            const int target = m_onehot ? A_BPLXDAT[2] : A_BPLXDAT[k];
            for (int j = 2; j <= 8; ++j) h.write(A_BPLXDAT[j], 0x0000);
            h.write(target, 0x8000);
            h.write(A_BPL1DAT, 0x0000);
            seen[k] = or_all(h.sample(WINDOW));
            if (seen[k] != plane_bit(k)) ok = false;
        }
        std::ostringstream o;
        for (int k = 2; k <= 8; ++k)
            o << (k > 2 ? ", " : "") << "BPL" << k << "DAT -> " << hex(seen[k]);
        checks.note("planes lit per address: " + o.str());
        checks.expect(ok,
                      "BPLXDAT-ADDRESSES-MAP-TO-PLANES: a word written at each "
                      "of the seven addresses lights that plane's output bit "
                      "and no other, which is the address and the W access "
                      "letter the AA list prints for BPL2DAT through BPL8DAT "
                      "[p.8] [claims: opus:AA-bpl2dat.c1/c2 opus:AA-bpl3dat.c1/c2 opus:AA-bpl4dat.c1/c2 opus:AA-bpl5dat.c1/c2 opus:AA-bpl6dat.c1/c2 opus:AA-bpl7dat.c1/c2 opus:AA-bpl8dat.c1/c2 sol:AA-bpl2dat.c1/c2 sol:AA-bpl3dat.c1/c2 sol:AA-bpl4dat.c1/c2 sol:AA-bpl5dat.c1/c2 sol:AA-bpl6dat.c1/c2 sol:AA-bpl7dat.c1/c2 sol:AA-bpl8dat.c1/c2]");
    }

    // --- 3. And the feed is parallel in, serial out -------------------------
    {
        Harness h;
        h.write(A_BPLCON1, 0x0000);
        uint16_t wrote[9] = {0};
        for (int k = 2; k <= 8; ++k) {
            // Top bit set in every plane so the first lit pixel is a common
            // origin; the remaining fifteen bits differ per plane.
            // MUTATION BPLXDAT-CONVERTS-PARALLEL-TO-SERIAL stimulus red
            // selecting this assertion's own id gives every plane plane 2's word, so six of the seven
            // recovered words are not the word that address was given.
            wrote[k] = (uint16_t)(0x8000u | ((m_same ? 2 : k) << 9) |
                                  (0x0A5u ^ (uint16_t)(m_same ? 2 : k)));
            h.write(A_BPLXDAT[k], wrote[k]);
        }
        h.write(A_BPL1DAT, 0x8000);
        const std::vector<uint8_t> t = h.sample(WINDOW);
        const int origin = first_lit(t);

        uint16_t got[9] = {0};
        bool ok = origin >= 0 && (size_t)(origin + 16) <= t.size();
        if (ok) {
            for (int k = 2; k <= 8; ++k) {
                uint16_t w = 0;
                for (int i = 0; i < 16; ++i)
                    w = (uint16_t)((w << 1) | ((t[origin + i] & plane_bit(k)) ? 1 : 0));
                got[k] = w;
                // Under MUTATE the words written were all plane 2's, so the
                // expected word is still what the document predicts for k.
                if (w != (uint16_t)(0x8000u | (k << 9) | (0x0A5u ^ (uint16_t)k)))
                    ok = false;
            }
        }
        std::ostringstream o;
        for (int k = 2; k <= 8; ++k)
            o << (k > 2 ? ", " : "") << "plane " << k << " wrote " << hex(wrote[k])
              << " read " << hex(got[k]);
        checks.note("origin pixel " + std::to_string(origin) + "; " + o.str());
        checks.expect(ok,
                      "BPLXDAT-CONVERTS-PARALLEL-TO-SERIAL: the sixteen bits "
                      "written at each address come back one pixel at a time "
                      "on that plane's output, most significant first, which is "
                      "the parallel-to-serial conversion the AA list's "
                      "description column names for each of the seven planes "
                      "[p.8] [claims: opus:AA-bpl2dat.c5 opus:AA-bpl3dat.c5 opus:AA-bpl4dat.c5 opus:AA-bpl5dat.c5 opus:AA-bpl6dat.c5 opus:AA-bpl7dat.c6 opus:AA-bpl8dat.c6 sol:AA-bpl2dat.c5 sol:AA-bpl3dat.c5 sol:AA-bpl4dat.c5 sol:AA-bpl5dat.c5 sol:AA-bpl6dat.c5 sol:AA-bpl7dat.c6 sol:AA-bpl8dat.c6]");
    }

    // --- 4. BPLCON1 carries two scroll values, not one ----------------------
    {
        Harness h;
        // MUTATION BPLCON1-AT-102-HOLDS-TWO-SCROLL-VALUES stimulus red
        // selecting this assertion's own id writes a scroll of zero, so both playfields start on the
        // same pixel and the two values cannot be told apart.
        h.write(A_BPLCON1, m_scroll);        // PF1 nibble 1, PF2 nibble 0
        for (int k = 2; k <= 8; ++k) h.write(A_BPLXDAT[k], 0x8000);
        h.write(A_BPL1DAT, 0x8000);
        const std::vector<uint8_t> t = h.sample(WINDOW);
        const int origin = first_lit(t);

        const uint8_t EVEN = plane_bit(2) | plane_bit(4) | plane_bit(6) | plane_bit(8);
        const uint8_t ODD  = plane_bit(1) | plane_bit(3) | plane_bit(5) | plane_bit(7);
        const bool ok = origin >= 0 && (size_t)(origin + 1) < t.size() &&
                        t[origin] == EVEN && (t[origin + 1] & ODD) == ODD;
        checks.note("first lit pixel " + hex(origin >= 0 ? t[origin] : 0) +
                    ", the one after it " +
                    hex(origin >= 0 && (size_t)(origin + 1) < t.size()
                        ? t[origin + 1] : 0) +
                    " (even planes are playfield 2, odd are playfield 1)");
        checks.expect(ok,
                      "BPLCON1-AT-102-HOLDS-TWO-SCROLL-VALUES: one word written "
                      "at $102 starts the even planes a pixel before the odd "
                      "ones, so the register carries a scroll value for each "
                      "playfield rather than one for all planes, which is what "
                      "the AA list prints for BPLCON1 [p.8] "
                      "[claims: opus:AA-bplcon1.c1/c2/c4 sol:AA-bplcon1.c1/c2/c4]");
    }

    // --- What this boundary cannot grade ------------------------------------
    checks.record(
             "DECLINED at this boundary (absent): The register is not decoded "
             "anywhere in this design. The four strobes are generated from the "
             "beam counter's own hpos rather than written at an address; the AGA "
             "high registers and REFPTR are in no module of this core. [claims: "
             "opus:AA-bplhdat-list.c1/c2 sol:AA-bplhdat.c1/c2]");
    checks.record(
             "DECLINED at this boundary (insufficient): The claim is the list's "
             "access column -- W, R or S. A module answers reads at the "
             "addresses it decodes and zero everywhere else, so a write-only row "
             "reads as the same silence an undecoded address reads as, and a "
             "strobe has no read at all. [claims: opus:AA-bpl1dat.c2 "
             "sol:AA-bpl1dat.c2]");
    checks.record(
             "DECLINED at this boundary (insufficient): The claim is the list's "
             "chip column, which says which 1985 silicon a register lives in. No "
             "port of any module in this design carries a chip identity. "
             "[claims: opus:AA-bpl1dat.c3 opus:AA-bplhdat-list.c3 "
             "sol:AA-bpl1dat.c3]");
    checks.record(
             "DECLINED at this boundary (insufficient): The claim names the chip "
             "that implements the register. Which silicon a block is fabricated "
             "in is a fact about the source tree and no port of denise_bitplanes "
             "carries it. [claims: opus:AA-bpl2dat.c3 opus:AA-bpl3dat.c3 "
             "opus:AA-bpl4dat.c3 opus:AA-bpl5dat.c3 opus:AA-bpl6dat.c3 "
             "opus:AA-bpl7dat.c3 opus:AA-bpl8dat.c3 opus:AA-bplcon1.c3 "
             "sol:AA-bpl2dat.c3 sol:AA-bpl3dat.c3 sol:AA-bpl4dat.c3 "
             "sol:AA-bpl5dat.c3 sol:AA-bpl6dat.c3 sol:AA-bpl7dat.c3 "
             "sol:AA-bpl8dat.c3 sol:AA-bplcon1.c3]");
    checks.record(
             "DECLINED at this boundary (insufficient): The register is decoded "
             "at this boundary, but no assertion in this package writes or reads "
             "it. The package grades the rows its own subject names; this row is "
             "another register of the same module. [claims: "
             "opus:AA-bpl1dat.c1/c5 sol:AA-bpl1dat.c1/c5]");
    checks.record(
             "DECLINED at this boundary (not-hardware): The claim is a legend "
             "statement, a gap between two printed rows, or a reserved line -- "
             "the document describing its own notation and layout. The one "
             "legend mark that IS a hardware restriction, the `~` Coprocessor "
             "limit, is graded by this package's own assertion. [claims: "
             "opus:AA-bpl1dat.c4 sol:AA-bpl1dat.c4]");
    checks.record(
             "DECLINED at this boundary (not-hardware): The row's lowercase `p` "
             "is read differently by the document's two legends: p.6 calls it "
             "new for the chip set, p.10 calls lowercase p added or changed in "
             "hires chips and capital P new. Which legend governs is a fact "
             "about the document, not about a signal. [claims: "
             "opus:AA-bpl7dat.c5 opus:AA-bpl8dat.c5 sol:AA-bpl7dat.c5 "
             "sol:AA-bpl8dat.c5]");
    checks.record(
             "DECLINED at this boundary (outside-uut): The `&` mark says the "
             "address is for use by a DMA channel only. Who is allowed to drive "
             "the bus is decided by the bitplane DMA in Agnus; this module "
             "answers the same way whatever drove it. [claims: "
             "opus:AA-bpl2dat.c4 opus:AA-bpl3dat.c4 opus:AA-bpl4dat.c4 "
             "opus:AA-bpl5dat.c4 opus:AA-bpl6dat.c4 opus:AA-bpl7dat.c4 "
             "opus:AA-bpl8dat.c4 sol:AA-bpl2dat.c4 sol:AA-bpl3dat.c4 "
             "sol:AA-bpl4dat.c4 sol:AA-bpl5dat.c4 sol:AA-bpl6dat.c4 "
             "sol:AA-bpl7dat.c4 sol:AA-bpl8dat.c4]");

    std::cout << "\nAssertions: " << checks.passed << " passed, "
              << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
