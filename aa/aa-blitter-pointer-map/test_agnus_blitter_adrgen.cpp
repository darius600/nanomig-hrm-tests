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
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

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
    const bool mutate = getenv("MUTATE") != nullptr;

    // MUTATION. Each assertion moves ONE address it writes to the neighbouring
    // register of the same kind, so the register that should have received the
    // value keeps what it had. A symmetric swap would be no mutation at all --
    // the Copper package learned that the hard way, staying green on four of
    // six until each guard broke its own pairing on one side only.
    const uint16_t m_live_h = mutate ? A_BLTDPTH : A_BLTAPTH;
    const uint16_t m_cpth = mutate ? A_BLTBPTH : A_BLTCPTH;
    const uint16_t m_bpth = mutate ? A_BLTAPTH : A_BLTBPTH;
    const uint16_t m_apth = mutate ? A_BLTDPTH : A_BLTAPTH;
    const uint16_t m_dpth = mutate ? A_BLTCPTH : A_BLTDPTH;
    const uint16_t m_cptl = mutate ? A_BLTBPTL : A_BLTCPTL;
    const uint16_t m_bptl = mutate ? A_BLTAPTL : A_BLTBPTL;
    const uint16_t m_aptl = mutate ? A_BLTDPTL : A_BLTAPTL;
    const uint16_t m_dptl = mutate ? A_BLTCPTL : A_BLTDPTL;
    const uint16_t m_cmod = mutate ? A_BLTBMOD : A_BLTCMOD;
    const uint16_t m_bmod = mutate ? A_BLTAMOD : A_BLTBMOD;
    const uint16_t m_amod = mutate ? A_BLTDMOD : A_BLTAMOD;
    const uint16_t m_dmod = mutate ? A_BLTCMOD : A_BLTDMOD;

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
        load_pointer(h, m_live_h, A_BLTAPTL, P_A);
        const uint32_t got = h.pointer(SEL_A);
        c.note("pattern " + hex(P_A) + " written through $050/$052 reads back "
               "at ptrsel=" + std::to_string(SEL_A) + " as " + hex(got));
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red MUTATE=1 writes
        // the high word at $054 instead of $050, so slot A keeps a zero high
        // word. It is mutated like every other assertion: an unmutated
        // liveness check would leave `make mutate` reporting a pass, and a
        // mutation run with any green in it is a gate that has already failed.
        c.expect(got == P_A,
                 "FIXTURE-DELIVERS-KNOWN-VALUE: a 20-bit pattern written by "
                 "address and read back by selection arrives intact, so a "
                 "later assertion that reads the wrong value means the decoder "
                 "and not the fixture [no page -- infrastructure]");
    }

    // --- 1. The four pointers, each written by address, each read by slot ---
    {
        Harness h;
        load_pointer(h, m_cpth, m_cptl, P_C);
        load_pointer(h, m_bpth, m_bptl, P_B);
        load_pointer(h, m_apth, m_aptl, P_A);
        load_pointer(h, m_dpth, m_dptl, P_D);
        const uint32_t gc = h.pointer(SEL_C), gb = h.pointer(SEL_B);
        const uint32_t ga = h.pointer(SEL_A), gd = h.pointer(SEL_D);
        c.note("C " + hex(gc) + " B " + hex(gb) + " A " + hex(ga) + " D "
               + hex(gd) + "; expected " + hex(P_C) + " " + hex(P_B) + " "
               + hex(P_A) + " " + hex(P_D));
        // MUTATION BLT-POINTERS-AT-048-056 stimulus red MUTATE=1 sends each
        // write to the next pointer's address, so every slot ends up holding a
        // neighbour's pattern.
        c.expect(gc == P_C && gb == P_B && ga == P_A && gd == P_D,
                 "BLT-POINTERS-AT-048-056: all four pointer pairs are decoded "
                 "where the address list puts them, each reaching its own slot "
                 "and no other [claims: opus:AA-bltcpth.c1, "
                 "opus:AA-bltcptl.c1, opus:AA-bltbpth.c1, opus:AA-bltbptl.c1, "
                 "opus:AA-bltapth.c1, opus:AA-bltaptl.c1, opus:AA-bltdpth.c1, "
                 "opus:AA-bltdptl.c1]");
    }

    // --- 2. The four modulos, reached through what adding one does ---------
    {
        Harness h;
        // A distinct modulo per channel, all positive and all different, so a
        // write landing in the wrong slot moves the wrong pointer by the wrong
        // amount. The RTL adds bltmod[15:1] sign-extended, so the value that
        // reaches the pointer is the written word shifted right once.
        constexpr uint16_t M_C = 0x0002, M_B = 0x0008, M_A = 0x0020, M_D = 0x0080;
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
        // MUTATION BLT-MODULOS-AT-060-066 stimulus red MUTATE=1 sends each
        // modulo write to the next modulo's address, so every pointer moves by
        // a neighbour's amount.
        c.expect(gc == wc && gb == wb && ga == wa && gd == wd,
                 "BLT-MODULOS-AT-060-066: all four modulo registers are decoded "
                 "where the address list puts them, each moving its own pointer "
                 "by its own value [claims: opus:AA-bltcmod.c1, "
                 "opus:AA-bltbmod.c1, opus:AA-bltamod.c1, opus:AA-bltdmod.c1]");
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

    std::cout << "\naa-blitter-pointer-map: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed ? 1 : 0;
}
