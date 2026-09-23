// aa-bitplane-pointer-map -- agnus_bitplanedma conformance test
//
// Test purpose: the AA specification's address-ordered register list gives the
//   eight bitplane pointers sixteen consecutive addresses from $0E0, marks each
//   pair with `+` -- the legend's address-register pair -- and says of every
//   such pair that the low word takes DB1 to DB15 and the high word DB0 to DB4.
//   This grades that at the module that holds the pointer bank.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 7 and 8, the
//   address-ordered register list, and p.6, the legend that defines `+`. The
//   document prints no page numbers of its own; the corpus numbering is the
//   PDF's.
// Non-goals: the fetch itself. What the pointers are FOR -- the display window,
//   the data-fetch sequence, the modulo added at end of line, the plane count --
//   needs a beam running inside a display window, and this package presents bus
//   cycles only.
//
// WHICH POINTER THE BOUNDARY SHOWS, AND WHY IT IS THE LAST ONE. The module
//   writes the bank through the address on the bus and reads it through the
//   plane the fetch sequencer has reached:
//
//     assign bplptr_sel = dma ? plane[2:0] : reg_address_in[4:2];     (:217)
//     assign address_out[20:16] = bplpth[plane[2:0]];                 (:231)
//     assign address_out[15: 1] = bplptl[plane[2:0]];                 (:242)
//
//   and `plane` is combinational from the sequencer: in lores with FMODE
//   clear, `plane = {2'b00, ~ddfseq[0], ~ddfseq[1], ~ddfseq[2]}` (:463). With
//   the sequencer idle at zero that is plane SEVEN, so what address_out shows
//   while no fetch runs is BPL8PT -- the eighth pointer, at $0F4 and $0F6.
//
//   That is the whole of what this boundary offers without a display window,
//   and it is enough for three questions: is the bank selected by the address
//   written, is BPL8PT's pair where the list puts it, and does each half take
//   the data bits the legend's `+` note gives it.

#include "Vagnus_bitplanedma.h"
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

// Word addresses as the AA address-ordered list prints them.
constexpr uint16_t A_BPLPTBASE = 0x0E0;      // BPL1PTH; +4 per plane, +2 for PTL

static uint16_t bplpth_addr(int p) { return (uint16_t)(A_BPLPTBASE + 4 * p); }
static uint16_t bplptl_addr(int p) { return (uint16_t)(A_BPLPTBASE + 4 * p + 2); }

class Harness {
public:
    Harness() : dut(new Vagnus_bitplanedma) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->harddis = 0; dut->aga = 1; dut->ecs = 1; dut->a1k = 0;
        dut->sof = 0; dut->dmaena = 0;
        dut->vpos = 0; dut->hpos = 0;
        dut->reg_address_in = 0; dut->data_in = 0;
        for (int i = 0; i < 8; ++i) step();
        dut->reset = 0;
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

    // The word address_out carries, which is the pointer of whichever plane
    // the sequencer has reached -- plane 7 while it is idle.
    uint32_t pointer() {
        dut->eval();
        return (uint32_t)(dut->address_out & 0xFFFFF);
    }
    static uint32_t high5(uint32_t p) { return (p >> 15) & 0x1F; }
    static uint32_t low15(uint32_t p) { return p & 0x7FFF; }

    void set_pointer(int plane, uint16_t high, uint16_t low) {
        write(bplpth_addr(plane), high);
        write(bplptl_addr(plane), low);
    }

    Vagnus_bitplanedma* dut;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks c;
    g_mutate = std::getenv("MUTATE");

    // Each guard below is keyed to its OWN assertion's id: MUTATE=<id> moves
    // ONE thing in that assertion's stimulus -- which plane's address a word
    // is written to, or which bits of the written word are looked at -- and
    // every other assertion runs its real stimulus.
    //
    // m_plane2 and m_plane3 used to be the SAME variable (m_plane, mutate ? 6
    // : 7) feeding both assertion 2 and assertion 3's stimulus, so selecting
    // one assertion's mutation also disturbed the other's. Split so each has
    // its own id, its own default, and its own wrong value -- assertion 2
    // needs a DIFFERENT plane written (6 instead of 7), assertion 3 needs the
    // SAME plane (7) written a second time instead of a different one (0).
    const int      m_plane2 = mutating("BPLPT-BANK-IS-ADDRESS-SELECTED") ? 6 : 7;
    const int      m_plane3 = mutating("BPL8PT-IS-AT-0F4-AND-0F6") ? 7 : 0;
    const uint16_t m_high   = mutating("BPLPT-HALVES-TAKE-DB0-DB4-AND-DB1-DB15")
                                   ? (uint16_t)0x0000u : (uint16_t)0xFFFFu;
    const bool     m_live1  = mutating("FIXTURE-REACHES-THE-POINTER-BANK");
    const uint16_t m_live   = m_live1 ? (uint16_t)0x0000u : (uint16_t)0x001Fu;

    std::cout << "aa-bitplane-pointer-map -- AA_CONFORMANCE at "
                 "agnus_bitplanedma\n";
    if (g_mutate)
        std::cout << "MUTATE=" << g_mutate
                   << ": exactly the named assertion below must go red, the "
                      "others must keep their baseline verdict\n";

    // --- 1. The fixture reaches the pointer the boundary shows --------------
    {
        Harness h;
        const uint32_t before = h.pointer();
        // MUTATION FIXTURE-REACHES-THE-POINTER-BANK stimulus red selecting
        // this assertion's own id writes zero into BOTH halves, which is
        // what reset left there, so the pointer cannot be seen to move.
        // Writing zero to one half only left the other one moving and the
        // assertion green.
        h.set_pointer(7, m_live, (uint16_t)(m_live1 ? 0x0000 : 0x2468));
        const uint32_t after = h.pointer();
        c.note("address_out reads " + hex(before) + " after reset and " +
               hex(after) + " once plane 7's pointer pair is written");
        c.expect(before != after,
                 "FIXTURE-REACHES-THE-POINTER-BANK: a write at $0F4 and $0F6 "
                 "changes the address this module puts out, so every assertion "
                 "below is against a bank this fixture wrote "
                 "[no page -- infrastructure]");
    }

    // --- 2. The bank is selected by the address written ---------------------
    {
        Harness h;
        for (int p = 0; p < 8; ++p)
            h.set_pointer(p, (uint16_t)(0x10 | p), (uint16_t)((p + 1) << 8));
        const uint32_t got = h.pointer();
        // MUTATION BPLPT-BANK-IS-ADDRESS-SELECTED stimulus red selecting
        // this assertion's own id overwrites plane 6's pair with plane 7's
        // value, so the two planes hold the same word and the bank cannot be
        // shown to be per plane.
        h.set_pointer(m_plane2, 0x0015, 0x8800);
        const uint32_t moved = h.pointer();
        c.note("with all eight pairs written, address_out reads " + hex(got) +
               "; after a further write to plane " +
               std::to_string(m_plane2) + " it reads " + hex(moved));
        c.expect(Harness::high5(got) == 0x17 && got != moved,
                 "BPLPT-BANK-IS-ADDRESS-SELECTED: each of the eight pointer "
                 "pairs is a bank entry of its own, chosen by the address the "
                 "write carries, which is what the AA list means by giving "
                 "BPL1PTH to BPL8PTL sixteen consecutive addresses [p.7] "
                 "[claims: opus:AA-bpl1pth.c1 opus:AA-bpl1ptl.c1 opus:AA-bpl2pth.c1 opus:AA-bpl2ptl.c1 opus:AA-bpl3pth.c1 opus:AA-bpl3ptl.c1 opus:AA-bpl4pth.c1 opus:AA-bpl4ptl.c1 opus:AA-bpl5pth.c1 opus:AA-bpl5ptl.c1 opus:AA-bpl6pth.c1 opus:AA-bpl6ptl.c1 opus:AA-bpl7pth.c1 opus:AA-bpl7ptl.c1 sol:AA-bpl1pth.c1 sol:AA-bpl1ptl.c1 sol:AA-bpl2pth.c1 sol:AA-bpl2ptl.c1 sol:AA-bpl3pth.c1 sol:AA-bpl3ptl.c1 sol:AA-bpl4pth.c1 sol:AA-bpl4ptl.c1 sol:AA-bpl5pth.c1 sol:AA-bpl5ptl.c1 sol:AA-bpl6pth.c1 sol:AA-bpl6ptl.c1 sol:AA-bpl7pth.c1 sol:AA-bpl7ptl.c1]");
    }

    // --- 3. And the eighth pair is at $0F4 and $0F6 -------------------------
    {
        Harness h;
        // MUTATION BPL8PT-IS-AT-0F4-AND-0F6 stimulus red selecting this
        // assertion's own id aims the second write at plane 7 as well, so
        // the pointer the boundary shows moves and the pair is not shown to
        // be its own.
        h.set_pointer(7, 0x0009, 0x1234);
        const uint32_t eighth = h.pointer();
        h.set_pointer(m_plane3, 0x001E, 0x7654);
        const uint32_t other = h.pointer();
        c.note("plane 7 reads " + hex(eighth) + " and is left at " +
               hex(other) + " by a write to another plane's address");
        c.expect(Harness::high5(eighth) == 0x9 && eighth == other,
                 "BPL8PT-IS-AT-0F4-AND-0F6: the pointer this boundary shows "
                 "while no fetch runs is the eighth, and it answers the "
                 "addresses the AA list gives BPL8PTH and BPL8PTL and no "
                 "others [p.7] [claims: opus:AA-bpl8pth.c1 opus:AA-bpl8ptl.c1 sol:AA-bpl8pth.c1 sol:AA-bpl8ptl.c1]");
    }

    // --- 4. The two halves take the data bits the legend gives them ---------
    {
        Harness h;
        // MUTATION BPLPT-HALVES-TAKE-DB0-DB4-AND-DB1-DB15 stimulus red
        // selecting this assertion's own id writes zero into the high half,
        // so the five bits it is supposed to keep are not there to be
        // counted.
        h.set_pointer(7, m_high, 0xFFFF);
        const uint32_t got = h.pointer();
        c.note("with $FFFF written to both halves, the pointer reads " +
               hex(got) + " -- high five bits " + hex(Harness::high5(got)) +
               ", low fifteen " + hex(Harness::low15(got)));
        c.expect(Harness::high5(got) == 0x1F && Harness::low15(got) == 0x7FFF,
                 "BPLPT-HALVES-TAKE-DB0-DB4-AND-DB1-DB15: the high word keeps "
                 "five bits and the low word fifteen, which is the split the "
                 "AA legend prints for every address-register pair it marks "
                 "with + [p.6] [claims: opus:AA-bpl1pth.c4 opus:AA-bpl1ptl.c4 opus:AA-bpl2pth.c4 opus:AA-bpl2ptl.c4 opus:AA-bpl3pth.c4 opus:AA-bpl3ptl.c4 opus:AA-bpl4pth.c4 opus:AA-bpl4ptl.c4 opus:AA-bpl5pth.c4 opus:AA-bpl5ptl.c4 opus:AA-bpl6pth.c4 opus:AA-bpl6ptl.c4 opus:AA-bpl7pth.c4 opus:AA-bpl7ptl.c4 opus:AA-bpl8pth.c4 opus:AA-bpl8ptl.c4 sol:AA-bpl1pth.c4 sol:AA-bpl1ptl.c4 sol:AA-bpl2pth.c4 sol:AA-bpl2ptl.c4 sol:AA-bpl3pth.c4 sol:AA-bpl3ptl.c4 sol:AA-bpl4pth.c4 sol:AA-bpl4ptl.c4 sol:AA-bpl5pth.c4 sol:AA-bpl5ptl.c4 sol:AA-bpl6pth.c4 sol:AA-bpl6ptl.c4 sol:AA-bpl7pth.c4 sol:AA-bpl7ptl.c4 sol:AA-bpl8pth.c4 sol:AA-bpl8ptl.c4]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary (absent): The register is not decoded "
             "anywhere in this design. The four strobes are generated from the "
             "beam counter's own hpos rather than written at an address; the AGA "
             "high registers and REFPTR are in no module of this core. [claims: "
             "opus:AA-bplhmod.c1/c2/c5 opus:AA-bplhpth.c1/c2/c4/c6 "
             "opus:AA-bplhptl.c1/c2/c4/c6 opus:AA-bplhstop.c1/c2/c5 "
             "opus:AA-bplhstrt.c1/c2/c5 sol:AA-bplhmod.c1/c2 "
             "sol:AA-bplhpth.c1/c2/c4/c6 sol:AA-bplhptl.c1/c2/c4/c6 "
             "sol:AA-bplhstop.c1/c2 sol:AA-bplhstrt.c1/c2]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is the list's "
             "access column -- W, R or S. A module answers reads at the "
             "addresses it decodes and zero everywhere else, so a write-only row "
             "reads as the same silence an undecoded address reads as, and a "
             "strobe has no read at all. [claims: opus:AA-bpl1mod.c2 "
             "opus:AA-bpl1pth.c2 opus:AA-bpl1ptl.c2 opus:AA-bpl2mod.c2 "
             "opus:AA-bpl2pth.c2 opus:AA-bpl2ptl.c2 opus:AA-bpl3pth.c2 "
             "opus:AA-bpl3ptl.c2 opus:AA-bpl4pth.c2 opus:AA-bpl4ptl.c2 "
             "opus:AA-bpl5pth.c2 opus:AA-bpl5ptl.c2 opus:AA-bpl6pth.c2 "
             "opus:AA-bpl6ptl.c2 opus:AA-bpl7pth.c2 opus:AA-bpl7ptl.c2 "
             "opus:AA-bpl8pth.c2 opus:AA-bpl8ptl.c2 opus:AA-ddfstop.c2 "
             "opus:AA-ddfstrt.c2 opus:AA-diwhigh.c2 opus:AA-diwstop.c2 "
             "opus:AA-diwstrt.c2 sol:AA-bpl1mod.c2 sol:AA-bpl1pth.c2 "
             "sol:AA-bpl1ptl.c2 sol:AA-bpl2mod.c2 sol:AA-bpl2pth.c2 "
             "sol:AA-bpl2ptl.c2 sol:AA-bpl3pth.c2 sol:AA-bpl3ptl.c2 "
             "sol:AA-bpl4pth.c2 sol:AA-bpl4ptl.c2 sol:AA-bpl5pth.c2 "
             "sol:AA-bpl5ptl.c2 sol:AA-bpl6pth.c2 sol:AA-bpl6ptl.c2 "
             "sol:AA-bpl7pth.c2 sol:AA-bpl7ptl.c2 sol:AA-bpl8pth.c2 "
             "sol:AA-bpl8ptl.c2 sol:AA-ddfstop.c2 sol:AA-ddfstrt.c2 "
             "sol:AA-diwhigh.c2 sol:AA-diwstop.c2 sol:AA-diwstrt.c2]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is the list's "
             "chip column, which says which 1985 silicon a register lives in. No "
             "port of any module in this design carries a chip identity. "
             "[claims: opus:AA-bpl1mod.c3 opus:AA-bpl1pth.c3 opus:AA-bpl1ptl.c3 "
             "opus:AA-bpl2mod.c3 opus:AA-bpl2pth.c3 opus:AA-bpl2ptl.c3 "
             "opus:AA-bpl3pth.c3 opus:AA-bpl3ptl.c3 opus:AA-bpl4pth.c3 "
             "opus:AA-bpl4ptl.c3 opus:AA-bpl5pth.c3 opus:AA-bpl5ptl.c3 "
             "opus:AA-bpl6pth.c3 opus:AA-bpl6ptl.c3 opus:AA-bpl7pth.c3 "
             "opus:AA-bpl7ptl.c3 opus:AA-bpl8pth.c3 opus:AA-bpl8ptl.c3 "
             "opus:AA-bplhmod.c3/c4 opus:AA-bplhpth.c3/c5 opus:AA-bplhptl.c3/c5 "
             "opus:AA-bplhstop.c3/c4 opus:AA-bplhstrt.c3/c4 opus:AA-ddfstop.c3 "
             "opus:AA-ddfstrt.c3 opus:AA-diwhigh.c3/c4 opus:AA-diwstop.c3 "
             "opus:AA-diwstrt.c3 sol:AA-bpl1mod.c3 sol:AA-bpl1pth.c3 "
             "sol:AA-bpl1ptl.c3 sol:AA-bpl2mod.c3 sol:AA-bpl2pth.c3 "
             "sol:AA-bpl2ptl.c3 sol:AA-bpl3pth.c3 sol:AA-bpl3ptl.c3 "
             "sol:AA-bpl4pth.c3 sol:AA-bpl4ptl.c3 sol:AA-bpl5pth.c3 "
             "sol:AA-bpl5ptl.c3 sol:AA-bpl6pth.c3 sol:AA-bpl6ptl.c3 "
             "sol:AA-bpl7pth.c3 sol:AA-bpl7ptl.c3 sol:AA-bpl8pth.c3 "
             "sol:AA-bpl8ptl.c3 sol:AA-bplhmod.c3/c4 sol:AA-bplhpth.c3/c5 "
             "sol:AA-bplhptl.c3/c5 sol:AA-bplhstop.c3/c4 sol:AA-bplhstrt.c3/c4 "
             "sol:AA-ddfstop.c3 sol:AA-ddfstrt.c3 sol:AA-diwhigh.c3/c4 "
             "sol:AA-diwstop.c3 sol:AA-diwstrt.c3]");
    c.record(
             "DECLINED at this boundary (insufficient): The register is decoded "
             "at this boundary, but no assertion in this package writes or reads "
             "it. The package grades the rows its own subject names; this row is "
             "another register of the same module. [claims: "
             "opus:AA-bpl1mod.c1/c4 opus:AA-bpl2mod.c1/c4 "
             "opus:AA-bplmod-symmetry.c1 opus:AA-ddfstop.c1/c4 "
             "opus:AA-ddfstrt.c1/c4 opus:AA-diwhigh.c1/c5 opus:AA-diwstop.c1/c4 "
             "opus:AA-diwstrt.c1/c4 sol:AA-bpl1mod.c1/c4 sol:AA-bpl2mod.c1/c4 "
             "sol:AA-ddfstop.c1/c4 sol:AA-ddfstrt.c1/c4 sol:AA-diwhigh.c1/c5 "
             "sol:AA-diwstop.c1/c4 sol:AA-diwstrt.c1/c4]");
    c.record(
             "DECLINED at this boundary (insufficient): This package writes and "
             "reads the register -- that is how its address row is graded here "
             "-- but no assertion distinguishes what this row says about it: the "
             "function column's width or role, the read-and-clear, the restart. "
             "A stimulus that passes whether or not the row is true does not "
             "grade it. [claims: opus:AA-bpl1pth.c5 opus:AA-bpl1ptl.c5 "
             "opus:AA-bpl2pth.c5 opus:AA-bpl2ptl.c5 opus:AA-bpl3pth.c5 "
             "opus:AA-bpl3ptl.c5 opus:AA-bpl4pth.c5 opus:AA-bpl4ptl.c5 "
             "opus:AA-bpl5pth.c5 opus:AA-bpl5ptl.c5 opus:AA-bpl6pth.c5 "
             "opus:AA-bpl6ptl.c5 opus:AA-bpl7pth.c5 opus:AA-bpl7ptl.c5 "
             "opus:AA-bpl8pth.c5 opus:AA-bpl8ptl.c5 sol:AA-bpl1pth.c5 "
             "sol:AA-bpl1ptl.c5 sol:AA-bpl2pth.c5 sol:AA-bpl2ptl.c5 "
             "sol:AA-bpl3pth.c5 sol:AA-bpl3ptl.c5 sol:AA-bpl4pth.c5 "
             "sol:AA-bpl4ptl.c5 sol:AA-bpl5pth.c5 sol:AA-bpl5ptl.c5 "
             "sol:AA-bpl6pth.c5 sol:AA-bpl6ptl.c5 sol:AA-bpl7pth.c5 "
             "sol:AA-bpl7ptl.c5 sol:AA-bpl8pth.c5 sol:AA-bpl8ptl.c5]");

    std::cout << "\nAssertions: " << c.passed << " passed, " << c.failed
              << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
