// aa-blitter-pointer-map -- agnus_blitter_adrgen conformance test
//
// Test purpose: the AA address list gives an address for the Blitter's four
//   pointer pairs and four modulo registers. This grades all twelve against
//   the decoder: a write at the printed address must reach the register that
//   address names, and the neighbouring register must not move.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF page 7, the address-ordered
//   register list. The document prints no page numbers of its own; the corpus
//   numbering is the PDF's.
// Non-goals: this package does not test the Blitter's operation, its minterms,
//   its fill logic, its line mode, or what the modulo is FOR. It grades where
//   twelve registers are decoded.
//
// WHY THESE TWELVE. Of 236 registers the AA list gives an address for, 93 are
//   also declared as an address in the RTL. agnus_blitter_adrgen holds twelve
//   of them -- the largest group left after the Copper package took eight.
//   The figures read 235, 92 and 49 until 22.09.2026. They were one
//   reading's, not the pipeline's -- address_yield's summary indexed the
//   first inventory on its command line whatever else was passed -- and
//   the row pattern held the mark column as the range `a-h`, which
//   excludes `p` and dropped six rows including CLXCON2 and FMODE. Over
//   the union of all 38 readings: 236 with an address, 93 in the RTL.
//
// HOW THE ADDRESSES ARE OBSERVED. `address_out[20:1]` is the pointer currently
//   selected by `ptrsel`, so each pointer can be written by address and then
//   read back by selection -- two independent routes to the same register,
//   which is what makes this an address measurement and not a register one. A
//   modulo has no read path, so it is reached through `addmod`: with the
//   modulo added to a known pointer, `address_out` moves by exactly the value
//   written (agnus_blitter_adrgen.v:115, sign-extended from bits [15:1]).
//
// THE DECODE IS NOT UNIFORM AND BOTH HALVES WERE READ BEFORE THIS WAS WRITTEN.
//   A pointer write is enabled by a full four-way address compare and the
//   register it lands in is chosen by {reg_address_in[4], reg_address_in[2]}
//   (:64). A modulo write is enabled by a SIX-bit compare,
//   `reg_address_in[8:3]==BLTAMOD[8:3]` (:94), with `reg_address_in[2:1]`
//   choosing among the four. Neither admits an address outside the eight and
//   four the list prints -- checked, because a partial compare is where an
//   alias would live, and finding none is a result worth having written down.

#include "Vagnus_blitter_adrgen.h"
#include "verilated.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace {
// Per-assertion mutation selector: MUTATE names ONE assertion's own label,
// and only that assertion's stimulus site changes.
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
constexpr uint16_t A_BLTCPTH = 0x048, A_BLTCPTL = 0x04A;
constexpr uint16_t A_BLTBPTH = 0x04C, A_BLTBPTL = 0x04E;
constexpr uint16_t A_BLTAPTH = 0x050, A_BLTAPTL = 0x052;
constexpr uint16_t A_BLTDPTH = 0x054, A_BLTDPTL = 0x056;
constexpr uint16_t A_BLTCMOD = 0x060, A_BLTBMOD = 0x062;
constexpr uint16_t A_BLTAMOD = 0x064, A_BLTDMOD = 0x066;

// Which slot each address selects, derived from the RTL and verified by the
// assertions below rather than assumed: {reg_address_in[4], reg_address_in[2]}
// for a pointer, reg_address_in[2:1] for a modulo. Both give C=0 B=1 A=2 D=3.
constexpr int SEL_C = 0, SEL_B = 1, SEL_A = 2, SEL_D = 3;

class Harness {
public:
    Harness() : dut(new Vagnus_blitter_adrgen) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->ptrsel = 0; dut->modsel = 0; dut->enaptr = 0;
        dut->incptr = 0; dut->decptr = 0; dut->addmod = 0; dut->submod = 0;
        dut->data_in = 0; dut->reg_address_in = 0;
        for (int i = 0; i < 8; ++i) step();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) step();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void step() { dut->clk = 1; dut->eval(); dut->clk = 0; dut->eval(); }

    void write(uint16_t word_addr, uint16_t value) {
        dut->reg_address_in = (word_addr >> 1) & 0xFF;
        dut->data_in = value;
        step();
        dut->reg_address_in = 0;
        dut->data_in = 0;
        step();
    }

    // Hold enaptr with every update input low: bltptr_sel becomes ptrsel and
    // the pointer is written back with its own value (newptr == bltptr_out
    // when nothing increments or adds), so selecting is not destructive.
    uint32_t pointer(int sel) {
        dut->enaptr = 1; dut->ptrsel = sel;
        dut->incptr = 0; dut->decptr = 0; dut->addmod = 0; dut->submod = 0;
        dut->eval();
        const uint32_t v = dut->address_out & 0xFFFFF;
        dut->enaptr = 0;
        dut->eval();
        return v;
    }

    // One addmod cycle on the selected pointer and modulo.
    uint32_t add_modulo(int ptr, int mod) {
        dut->enaptr = 1; dut->ptrsel = ptr; dut->modsel = mod;
        dut->addmod = 1;
        step();
        dut->addmod = 0; dut->enaptr = 0;
        dut->eval();
        return pointer(ptr);
    }

    Vagnus_blitter_adrgen* dut;
};

// A 20-bit pattern written as the pair the document describes: high five bits
// through PTH's data_in[4:0], low fifteen through PTL's data_in[15:1].
static void load_pointer(Harness& h, uint16_t hi_addr, uint16_t lo_addr,
                         uint32_t pattern) {
    h.write(hi_addr, static_cast<uint16_t>((pattern >> 15) & 0x1F));
    h.write(lo_addr, static_cast<uint16_t>((pattern & 0x7FFF) << 1));
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks c;
    g_mutate = std::getenv("MUTATE");

    // MUTATION. Each assertion moves ONE address it writes to the neighbouring
    // register of the same kind, so the register that should have received the
    // value keeps what it had. A symmetric swap would be no mutation at all --
    // the Copper package learned that the hard way, staying green on four of
    // six until each guard broke its own pairing on one side only.
    //
    // Each block below gates its own set of address swaps behind its OWN
    // assertion's id. The pointer assertion moves all eight addresses (four
    // pairs) at once and the modulo assertion moves all four -- that is not
    // several assertions sharing one switch, it is one compound assertion
    // (all four pointer pairs / all four modulos, checked together) selected
    // by its own single id; no swap here is reachable by any OTHER
    // assertion's id.
    if (g_mutate)
        std::cout << "MUTATE=" << g_mutate
                   << ": exactly the named assertion below must go red\n";

    c.record("oracle: AA specification, PDF p. 7, the address-ordered register "
             "list. BLTCPTH $048, BLTCPTL $04A, BLTBPTH $04C, BLTBPTL $04E, "
             "BLTAPTH $050, BLTAPTL $052, BLTDPTH $054, BLTDPTL $056, "
             "BLTCMOD $060, BLTBMOD $062, BLTAMOD $064, BLTDMOD $066 "
             "[AA-blitter-map.c1]");

    // Four distinct patterns, each with its top five bits set differently, so
    // a PTH written to the wrong pointer shows and a PTL written to the wrong
    // pointer shows too.
    constexpr uint32_t P_C = 0x0ACE1, P_B = 0x13572, P_A = 0x1BD93, P_D = 0x075E4;

    // --- FIXTURE LIVENESS, before any address assertion leans on it --------
    {
        Harness h;
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red: selecting this
        // id writes the high word at $054 instead of $050, so slot A keeps a
        // zero high word. It is mutated like every other assertion: an
        // unmutated liveness check would leave `make mutate` reporting a
        // pass, and a mutation run with any green in it is a gate that has
        // already failed.
        const uint16_t m_live_h = mutating("FIXTURE-DELIVERS-KNOWN-VALUE")
                                       ? A_BLTDPTH : A_BLTAPTH;
        load_pointer(h, m_live_h, A_BLTAPTL, P_A);
        const uint32_t got = h.pointer(SEL_A);
        c.note("pattern " + hex(P_A) + " written through $050/$052 reads back "
               "at ptrsel=" + std::to_string(SEL_A) + " as " + hex(got));
        c.expect(got == P_A,
                 "FIXTURE-DELIVERS-KNOWN-VALUE: a 20-bit pattern written by "
                 "address and read back by selection arrives intact, so a "
                 "later assertion that reads the wrong value means the decoder "
                 "and not the fixture [no page -- infrastructure]");
    }

    // --- 1. The four pointers, each written by address, each read by slot ---
    {
        Harness h;
        // Selecting BLT-POINTERS-AT-048-056 sends each of the four
        // pointer-pair writes to the next pointer's address, so every slot
        // ends up holding a neighbour's pattern (record and rationale are
        // below, next to the expect() this stimulus feeds). All eight
        // addresses here are gated on this ONE id; none of them moves under
        // any other assertion's id.
        const bool m_pointers = mutating("BLT-POINTERS-AT-048-056");
        const uint16_t m_cpth = m_pointers ? A_BLTBPTH : A_BLTCPTH;
        const uint16_t m_bpth = m_pointers ? A_BLTAPTH : A_BLTBPTH;
        const uint16_t m_apth = m_pointers ? A_BLTDPTH : A_BLTAPTH;
        const uint16_t m_dpth = m_pointers ? A_BLTCPTH : A_BLTDPTH;
        const uint16_t m_cptl = m_pointers ? A_BLTBPTL : A_BLTCPTL;
        const uint16_t m_bptl = m_pointers ? A_BLTAPTL : A_BLTBPTL;
        const uint16_t m_aptl = m_pointers ? A_BLTDPTL : A_BLTAPTL;
        const uint16_t m_dptl = m_pointers ? A_BLTCPTL : A_BLTDPTL;
        load_pointer(h, m_cpth, m_cptl, P_C);
        load_pointer(h, m_bpth, m_bptl, P_B);
        load_pointer(h, m_apth, m_aptl, P_A);
        load_pointer(h, m_dpth, m_dptl, P_D);
        const uint32_t gc = h.pointer(SEL_C), gb = h.pointer(SEL_B);
        const uint32_t ga = h.pointer(SEL_A), gd = h.pointer(SEL_D);
        c.note("C " + hex(gc) + " B " + hex(gb) + " A " + hex(ga) + " D "
               + hex(gd) + "; expected " + hex(P_C) + " " + hex(P_B) + " "
               + hex(P_A) + " " + hex(P_D));
        // MUTATION BLT-POINTERS-AT-048-056 stimulus red: selecting this id
        // sends each write to the next pointer's address, so every slot ends
        // up holding a neighbour's pattern.
        c.expect(gc == P_C && gb == P_B && ga == P_A && gd == P_D,
                 "BLT-POINTERS-AT-048-056: all four pointer pairs are decoded "
                 "where the address list puts them, each reaching its own slot "
                 "and no other [claims: opus:AA-bltapth.c1 opus:AA-bltaptl.c1 opus:AA-bltbpth.c1 opus:AA-bltbptl.c1 opus:AA-bltcpth.c1 opus:AA-bltcptl.c1 opus:AA-bltdpth.c1 opus:AA-bltdptl.c1 sol:AA-bltapth.c1 sol:AA-bltaptl.c1 sol:AA-bltbpth.c1 sol:AA-bltbptl.c1 sol:AA-bltcpth.c1 sol:AA-bltcptl.c1 sol:AA-bltdpth.c1 sol:AA-bltdptl.c1]");
    }

    // --- 2. The four modulos, reached through what adding one does ---------
    {
        Harness h;
        // A distinct modulo per channel, all positive and all different, so a
        // write landing in the wrong slot moves the wrong pointer by the wrong
        // amount. The RTL adds bltmod[15:1] sign-extended, so the value that
        // reaches the pointer is the written word shifted right once.
        constexpr uint16_t M_C = 0x0002, M_B = 0x0008, M_A = 0x0020, M_D = 0x0080;
        // Selecting BLT-MODULOS-AT-060-066 sends each modulo write to the
        // next modulo's address, so every pointer moves by a neighbour's
        // amount (record and rationale are below, next to the expect() this
        // stimulus feeds). All four addresses here are gated on this ONE id.
        const bool m_modulos = mutating("BLT-MODULOS-AT-060-066");
        const uint16_t m_cmod = m_modulos ? A_BLTBMOD : A_BLTCMOD;
        const uint16_t m_bmod = m_modulos ? A_BLTAMOD : A_BLTBMOD;
        const uint16_t m_amod = m_modulos ? A_BLTDMOD : A_BLTAMOD;
        const uint16_t m_dmod = m_modulos ? A_BLTCMOD : A_BLTDMOD;
        h.write(m_cmod, M_C);
        h.write(m_bmod, M_B);
        h.write(m_amod, M_A);
        h.write(m_dmod, M_D);
        load_pointer(h, A_BLTCPTH, A_BLTCPTL, P_C);
        load_pointer(h, A_BLTBPTH, A_BLTBPTL, P_B);
        load_pointer(h, A_BLTAPTH, A_BLTAPTL, P_A);
        load_pointer(h, A_BLTDPTH, A_BLTDPTL, P_D);
        const uint32_t gc = h.add_modulo(SEL_C, SEL_C);
        const uint32_t gb = h.add_modulo(SEL_B, SEL_B);
        const uint32_t ga = h.add_modulo(SEL_A, SEL_A);
        const uint32_t gd = h.add_modulo(SEL_D, SEL_D);
        const uint32_t wc = (P_C + (M_C >> 1)) & 0xFFFFF;
        const uint32_t wb = (P_B + (M_B >> 1)) & 0xFFFFF;
        const uint32_t wa = (P_A + (M_A >> 1)) & 0xFFFFF;
        const uint32_t wd = (P_D + (M_D >> 1)) & 0xFFFFF;
        c.note("after addmod: C " + hex(gc) + "/" + hex(wc) + " B " + hex(gb)
               + "/" + hex(wb) + " A " + hex(ga) + "/" + hex(wa) + " D "
               + hex(gd) + "/" + hex(wd) + "  (read/expected)");
        // MUTATION BLT-MODULOS-AT-060-066 stimulus red: selecting this id
        // sends each modulo write to the next modulo's address, so every
        // pointer moves by a neighbour's amount.
        c.expect(gc == wc && gb == wb && ga == wa && gd == wd,
                 "BLT-MODULOS-AT-060-066: all four modulo registers are decoded "
                 "where the address list puts them, each moving its own pointer "
                 "by its own value [claims: opus:AA-bltamod.c1 opus:AA-bltbmod.c1 opus:AA-bltcmod.c1 opus:AA-bltdmod.c1]");
    }

    // --- 3. What the list says about access, measured but not graded -------
    {
        c.record("the list marks all twelve `W` and `~`, and the eight pointer "
                 "rows also `+` -- an address register pair, low word "
                 "DB1-DB15, high word DB0-DB4. This module has no read path "
                 "for any of them, so `W` is not falsifiable here; the pair "
                 "shape is exercised by assertion 1 writing five bits high and "
                 "fifteen low, but is not itself graded. The `~` is the "
                 "Coprocessor restriction and belongs to agnus_copper, where "
                 "aa/aa-copper-register-map grades the range "
                 "[AA-blitter-map.n1]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary (insufficient): The claim is the list's "
             "access column -- W, R or S. A module answers reads at the "
             "addresses it decodes and zero everywhere else, so a write-only row "
             "reads as the same silence an undecoded address reads as, and a "
             "strobe has no read at all. [claims: opus:AA-bltapth.c2 "
             "opus:AA-bltaptl.c2 opus:AA-bltbpth.c2 opus:AA-bltbptl.c2 "
             "opus:AA-bltcpth.c2 opus:AA-bltcptl.c2 opus:AA-bltdpth.c2 "
             "opus:AA-bltdptl.c2 sol:AA-bltapth.c2 sol:AA-bltaptl.c2 "
             "sol:AA-bltbpth.c2 sol:AA-bltbptl.c2 sol:AA-bltcpth.c2 "
             "sol:AA-bltcptl.c2 sol:AA-bltdpth.c2 sol:AA-bltdptl.c2]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is the list's "
             "chip column, which says which 1985 silicon a register lives in. No "
             "port of any module in this design carries a chip identity. "
             "[claims: opus:AA-bltapth.c3 opus:AA-bltaptl.c3 opus:AA-bltbpth.c3 "
             "opus:AA-bltbptl.c3 opus:AA-bltcpth.c3 opus:AA-bltcptl.c3 "
             "opus:AA-bltdpth.c3 opus:AA-bltdptl.c3 sol:AA-bltapth.c3 "
             "sol:AA-bltaptl.c3 sol:AA-bltbpth.c3 sol:AA-bltbptl.c3 "
             "sol:AA-bltcpth.c3 sol:AA-bltcptl.c3 sol:AA-bltdpth.c3 "
             "sol:AA-bltdptl.c3]");
    c.record(
             "DECLINED at this boundary (insufficient): This package writes and "
             "reads the register -- that is how its address row is graded here "
             "-- but no assertion distinguishes what this row says about it: the "
             "function column's width or role, the read-and-clear, the restart. "
             "A stimulus that passes whether or not the row is true does not "
             "grade it. [claims: opus:AA-bltapth.c6 opus:AA-bltaptl.c6 "
             "opus:AA-bltbpth.c6 opus:AA-bltbptl.c6 opus:AA-bltcpth.c6 "
             "opus:AA-bltcptl.c6 opus:AA-bltdpth.c6 opus:AA-bltdptl.c6 "
             "sol:AA-bltapth.c6 sol:AA-bltaptl.c6 sol:AA-bltbpth.c6 "
             "sol:AA-bltbptl.c6 sol:AA-bltcpth.c6 sol:AA-bltcptl.c6 "
             "sol:AA-bltdpth.c6 sol:AA-bltdptl.c6]");
    c.record(
             "DECLINED at this boundary (not-hardware): The claim is a legend "
             "statement, a gap between two printed rows, or a reserved line -- "
             "the document describing its own notation and layout. The one "
             "legend mark that IS a hardware restriction, the `~` Coprocessor "
             "limit, is graded by this package's own assertion. [claims: "
             "opus:AA-bltapth.c4/c5 opus:AA-bltaptl.c4/c5 opus:AA-bltbpth.c4/c5 "
             "opus:AA-bltbptl.c4/c5 opus:AA-bltcpth.c4/c5 opus:AA-bltcptl.c4/c5 "
             "opus:AA-bltdpth.c4/c5 opus:AA-bltdptl.c4/c5 sol:AA-bltapth.c4/c5 "
             "sol:AA-bltaptl.c4/c5 sol:AA-bltbpth.c4/c5 sol:AA-bltbptl.c4/c5 "
             "sol:AA-bltcpth.c4/c5 sol:AA-bltcptl.c4/c5 sol:AA-bltdpth.c4/c5 "
             "sol:AA-bltdptl.c4/c5]");

    std::cout << "\naa-blitter-pointer-map: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed ? 1 : 0;
}
