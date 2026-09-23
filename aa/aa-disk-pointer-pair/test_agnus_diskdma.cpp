// aa-disk-pointer-pair -- agnus_diskdma conformance test
//
// Test purpose: the AA specification's address-ordered register list gives the
//   four disk DMA registers an address each, and marks the two pointer halves
//   `+`. The p. 6 legend defines that mark as an address register pair whose
//   LOW word uses DB1-DB15 and whose HIGH word uses DB0-DB4. This grades the
//   four addresses against the decoder and, for the first time in this suite,
//   grades the `+` legend itself: which data bits each half of a pointer pair
//   actually takes.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF page 6, the address-ordered
//   register list and its legend. The document prints no page numbers of its
//   own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the disk DMA slot allocation, the
//   `speed` input, the pointer's auto-increment during DMA, or anything Paula
//   does with the data. It grades four addresses and one legend.
//
// WHY THE `+` LEGEND MATTERS MORE THAN FOUR ADDRESSES. Thirty register pairs
//   in this document carry the mark -- the blitter's four, the Copper's two,
//   the four audio, the eight bit-plane, the eight sprite, plus SPRHPT and
//   BPLHPT and this one. Every one of them inherits the same sentence about
//   DB0-DB4 and DB1-DB15. aa-blitter-pointer-map grades twelve of those
//   addresses and does not grade the bit split; this package grades the split
//   once, where the RTL states it in one line:
//
//     address_outnew[20:1] = dma ? address_out+1 : {data_in[4:0],data_in[15:1]}
//
//   High word takes data_in[4:0] -- DB0 to DB4. Low word takes data_in[15:1]
//   -- DB1 to DB15. The legend and the line say the same thing, and saying so
//   is not the same as showing it, which is what assertions 4 and 5 are for.
//
// HOW THE ADDRESSES ARE OBSERVED. `address_out[20:1]` is the pointer itself,
//   and the two halves latch separately:
//
//     address_out[20:16] <= ... when dma || reg_address_in == DSKPTH   (:58)
//     address_out[15:1]  <= ... when dma || reg_address_in == DSKPTL   (:65)
//     reg_address_out    =  wr ? DSKDATR : DSKDAT                      (:71)
//     wr                 =  ~dmas                                      (:50)
//
//   so each pointer half is reachable at exactly one address and leaves the
//   other half alone, and the two data addresses are selected by one input
//   rather than by a write.
//
// WHY dmal IS HELD LOW. `dma = dmal & dmaslot & hpos[0]`, and a live `dma`
//   latches address_out+1 into BOTH halves on every enabled edge -- which
//   would overwrite the very thing each assertion is about. With dmal low the
//   pointer moves only when written. The DMA increment is a real behaviour and
//   it is this package's first non-goal, not an oversight.
//
// THE POINTER HAS NO RESET. `address_out` is a reg with no reset clause, so
//   its initial value is Verilator's zero-fill and not the module's. Every
//   assertion writes both halves before reading, and the liveness check below
//   is what establishes that a written value arrives at all.

#include "Vagnus_diskdma.h"
#include "verilated.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace {
// Per-assertion mutation selector, the same form the rest of the suite uses:
// MUTATE names ONE assertion's own label, and only that assertion's stimulus
// site changes. Every OTHER assertion stays on its real stimulus and is a
// live control on each run.
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

// Word addresses as the AA address list prints them, read from the corpus.
constexpr uint16_t A_DSKPTH  = 0x020;
constexpr uint16_t A_DSKPTL  = 0x022;
constexpr uint16_t A_DSKDAT  = 0x026;
constexpr uint16_t A_DSKDATR = 0x008;

class Harness {
public:
    Harness() : dut(new Vagnus_diskdma) {
        dut->clk = 0; dut->clk7_en = 1;
        dut->dmal = 0; dut->dmas = 0; dut->speed = 0;
        dut->hpos = 0x40; dut->reg_address_in = 0; dut->data_in = 0;
        for (int i = 0; i < 4; ++i) step();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void step() {
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();
    }

    void write(uint16_t word_addr, uint16_t value) {
        dut->reg_address_in = (word_addr >> 1) & 0xFF;
        dut->data_in = value;
        step();
        dut->reg_address_in = 0;
        dut->data_in = 0;
        step();
    }

    // address_out is declared [20:1], so the C value's bit 0 IS address_out[1].
    // High half = bits 19..15 of the C value, low half = bits 14..0. Reading it
    // as though bit 0 were address_out[0] shifts every expected value by one
    // and makes a correct decoder look broken.
    uint32_t pointer() const { return dut->address_out & 0xFFFFF; }
    uint32_t high5()   const { return (pointer() >> 15) & 0x1F; }
    uint32_t low15()   const { return pointer() & 0x7FFF; }

    uint8_t reg_addr_out(bool dmas) {
        dut->dmas = dmas ? 1 : 0;
        dut->eval();
        return dut->reg_address_out;
    }

    Vagnus_diskdma* dut;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks c;
    g_mutate = std::getenv("MUTATE");

    // MUTATION. Each guard below tests its own assertion's label and moves
    // ONE thing: the address written, the data pattern, or the input that
    // selects which data address is driven out. MUTATE=<label> selects
    // exactly one.
    //
    // NO GUARD SWAPS THE PAIR SYMMETRICALLY. Exchanging DSKPTH with DSKPTL in
    // both directions writes both halves and the pointer ends up holding the
    // same twenty bits by the mirrored route, which is how the Copper package
    // once kept four of six assertions green under mutation.
    const uint16_t m_pth   = mutating("DSKPTH-AT-020") ? A_DSKPTL : A_DSKPTH;   // 2: high half never written
    const uint16_t m_ptl   = mutating("DSKPTL-AT-022") ? A_DSKPTH : A_DSKPTL;   // 3: low half never written
    // 4 and 5 mutate the DATA, not the address: the proposition under test is
    // which bits of the word each half takes, so the mutation has to move bits.
    const uint16_t m_hi_pat = mutating("DSKPT-HIGH-TAKES-DB0-DB4") ? 0x001F : 0xFFE0;      // 4: the ignored bits stop being ignored
    const uint16_t m_lo_pat = mutating("DSKPT-LOW-TAKES-DB1-DB15") ? 0xFFFF : 0x0001;      // 5: likewise for DB0
    const bool     m_dmas   = mutating("DSKDATR-AT-008") ? true : false;         // 6: wr inverted
    const bool     m_dmas2  = mutating("DSKDAT-AT-026") ? false : true;         // 7: wr inverted
    const uint16_t m_live   = mutating("FIXTURE-DELIVERS-KNOWN-VALUE") ? A_DSKPTL : A_DSKPTH;  // liveness, same shape

    c.record("oracle: AA specification, PDF p.6, the address-ordered register "
             "list and its legend. DSKDATR $008 ER P, DSKPTH $020 W A, DSKPTL "
             "$022 W A, DSKDAT $026 W P; DSKPTH and DSKPTL both marked + "
             "[no page -- the package's own oracle line]");

    c.record("DECLINED at this boundary: the list's chip column gives DSKPTH "
             "and DSKPTL to Agnus/Alice and DSKDAT and DSKDATR to Paula. Which "
             "silicon a block is fabricated in is a fact about the source tree "
             "and no port of this module carries it [claims: sol:AA-dskpth.c3 "
             "sol:AA-dskptl.c3 sol:AA-dskdat.c3 sol:AA-dskdatr.c3]");
    c.record("DECLINED at this boundary: the list marks all four registers `~`, "
             "which the p.6 legend defines as not writable by the Coprocessor "
             "unless COPCON bit 1 is set. That restriction is enforced in "
             "agnus_copper and graded by aa-copper-register-map [claims: "
             "sol:AA-dskpth.c5 sol:AA-dskptl.c5 sol:AA-dskdat.c5 "
             "sol:AA-dskdatr.c5]");
    c.record("DECLINED at this boundary: the list marks DSKDATR `&`, which the "
             "p.6 legend defines as used by a DMA channel only, and gives it "
             "the access letters ER for early read. This module drives the "
             "register address out; whether a processor read of $008 is "
             "refused is decided elsewhere [claims: sol:AA-dskdatr.c2 "
             "sol:AA-dskdatr.c4]");

    // --- FIXTURE LIVENESS ---------------------------------------------------
    {
        Harness h;
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red: selecting this
        // assertion's own id writes the high half at $022, so the top five
        // bits stay zero.
        h.write(m_live, 0x0015);
        h.write(A_DSKPTL, 0xACE4);
        c.note("high half written $0015 at $020 and low half $ACE4 at $022; "
               "address_out = " + hex(h.pointer()) + ", high5 = "
               + hex(h.high5()) + ", low15 = " + hex(h.low15()));
        c.expect(h.high5() == 0x15 && h.low15() == (0xACE4 >> 1),
                 "FIXTURE-DELIVERS-KNOWN-VALUE: a distinct pattern written "
                 "through both pointer halves arrives at address_out, so a "
                 "later assertion that reads nothing means the decoder and not "
                 "the fixture [no page -- infrastructure]");
    }

    // --- 2. DSKPTH is decoded at $020, and touches only the high half -------
    {
        Harness h;
        h.write(A_DSKPTL, 0x2222);
        const uint32_t low_before = h.low15();
        // MUTATION DSKPTH-AT-020 stimulus red: selecting this assertion's
        // own id writes at $022, so the high half keeps its zero and the
        // low half is overwritten.
        h.write(m_pth, 0x001B);
        c.note("after $001B at $020, high5 = " + hex(h.high5())
               + ", low15 = " + hex(h.low15()) + " (was " + hex(low_before) + ")");
        c.expect(h.high5() == 0x1B && h.low15() == low_before,
                 "DSKPTH-AT-020: a write at $020 sets the disk pointer's high "
                 "five bits and leaves the low fifteen alone, which is the "
                 "address and the half the AA list prints for DSKPTH [p.6] "
                 "[claims: sol:AA-dskpth.c1 opus:AA-dskpth.c1 sol:AA-dskpth.c2 "
                 "opus:AA-dskpth.c2 sol:AA-dskpth.c6 opus:AA-dskpth.c6]");
    }

    // --- 3. DSKPTL is decoded at $022, and touches only the low half --------
    {
        Harness h;
        h.write(A_DSKPTH, 0x000D);
        const uint32_t high_before = h.high5();
        // MUTATION DSKPTL-AT-022 stimulus red: selecting this assertion's
        // own id writes at $020, so the low half keeps its zero.
        h.write(m_ptl, 0x5556);
        c.note("after $5556 at $022, low15 = " + hex(h.low15())
               + ", high5 = " + hex(h.high5()) + " (was " + hex(high_before) + ")");
        c.expect(h.low15() == (0x5556 >> 1) && h.high5() == high_before,
                 "DSKPTL-AT-022: a write at $022 sets the disk pointer's low "
                 "fifteen bits and leaves the high five alone, which is the "
                 "address and the half the AA list prints for DSKPTL [p.6] "
                 "[claims: sol:AA-dskptl.c1 opus:AA-dskptl.c1 sol:AA-dskptl.c2 "
                 "opus:AA-dskptl.c2 sol:AA-dskptl.c6 opus:AA-dskptl.c6]");
    }

    // --- 4. the HIGH word of a `+` pair uses DB0-DB4 and nothing above ------
    {
        Harness h;
        // MUTATION DSKPT-HIGH-TAKES-DB0-DB4 oracle red: selecting this
        // assertion's own id writes $001F where the assertion needs a word
        // whose low five bits are clear, so the "bits above DB4 are
        // ignored" reading becomes 0x1F.
        h.write(A_DSKPTH, m_hi_pat);
        const uint32_t ignored = h.high5();
        h.write(A_DSKPTH, 0x001F);
        const uint32_t taken = h.high5();
        c.note("writing $FFE0 at $020 gives high5 = " + hex(ignored)
               + "; writing $001F gives high5 = " + hex(taken));
        c.expect(ignored == 0x00 && taken == 0x1F,
                 "DSKPT-HIGH-TAKES-DB0-DB4: the high half of the pointer takes "
                 "data bits 0 to 4 and no bit above them, which is what the "
                 "p.6 legend states for the high word of a `+` address register "
                 "pair [p.6] [claims: sol:AA-legends.c5 opus:AA-legend6-plus.c3 "
                 "sol:AA-dskpth.c4 opus:AA-dskpth.c4]");
    }

    // --- 5. the LOW word of a `+` pair uses DB1-DB15, so DB0 is ignored -----
    {
        Harness h;
        // MUTATION DSKPT-LOW-TAKES-DB1-DB15 oracle red: selecting this
        // assertion's own id writes $FFFF where the assertion needs a word
        // with only DB0 set.
        h.write(A_DSKPTL, m_lo_pat);
        const uint32_t ignored = h.low15();
        h.write(A_DSKPTL, 0xFFFF);
        const uint32_t taken = h.low15();
        c.note("writing $0001 at $022 gives low15 = " + hex(ignored)
               + "; writing $FFFF gives low15 = " + hex(taken));
        c.expect(ignored == 0x0000 && taken == 0x7FFF,
                 "DSKPT-LOW-TAKES-DB1-DB15: the low half of the pointer takes "
                 "data bits 1 to 15 and ignores bit 0, which is what the p.6 "
                 "legend states for the low word of a `+` address register "
                 "pair [p.6] [claims: sol:AA-legends.c4 opus:AA-legend6-plus.c2 "
                 "sol:AA-dskptl.c4 opus:AA-dskptl.c4]");
    }

    // --- 6. DSKDATR is the address driven out for a read --------------------
    {
        Harness h;
        // MUTATION DSKDATR-AT-008 stimulus red: selecting this assertion's
        // own id raises dmas, which clears wr and drives DSKDAT instead.
        const uint8_t got = h.reg_addr_out(m_dmas);
        c.note("with dmas low, reg_address_out = " + hex(got) + ", expected "
               + hex(A_DSKDATR >> 1));
        c.expect(got == ((A_DSKDATR >> 1) & 0xFF),
                 "DSKDATR-AT-008: the address this module drives out for a disk "
                 "read is $008, which is the address the AA list prints for "
                 "DSKDATR and the dummy address its function column describes "
                 "[p.6] [claims: sol:AA-dskdatr.c1 opus:AA-dskdatr.c1 "
                 "sol:AA-dskdatr.c6 opus:AA-dskdatr.c6]");
    }

    // --- 7. DSKDAT is the address driven out for a write --------------------
    {
        Harness h;
        // MUTATION DSKDAT-AT-026 stimulus red: selecting this assertion's
        // own id lowers dmas, which raises wr and drives DSKDATR instead.
        const uint8_t got = h.reg_addr_out(m_dmas2);
        c.note("with dmas high, reg_address_out = " + hex(got) + ", expected "
               + hex(A_DSKDAT >> 1));
        c.expect(got == ((A_DSKDAT >> 1) & 0xFF),
                 "DSKDAT-AT-026: the address this module drives out for a disk "
                 "write is $026, which is the address the AA list prints for "
                 "DSKDAT [p.6] [claims: sol:AA-dskdat.c1 opus:AA-dskdat.c1 "
                 "sol:AA-dskdat.c2 opus:AA-dskdat.c2]");
    }

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
