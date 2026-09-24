// aa-beam-position-words -- agnus_beamcounter conformance test
//
// Test purpose: the AA specification's alphabetical entries for VPOSR and
//   VHPOSR print each register as a sixteen-row bit table, and VPOSR's table
//   carries a seven-bit chip identifier with a list of the values each Agnus
//   reports. This grades the two words at the module that produces them. It
//   is the complement of aa-beam-counter-registers, which grades where the
//   seventeen beam registers are decoded.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 28 and 29. The document
//   prints no page numbers of its own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the light pen, interlace field
//   alternation, the sync pulses, or the vertical blanking window.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the AA specification independently, and ids such as
//   `opus:AA-vhpos.c22` and `opus:AA-vhposr.c1` each name one reader's reading
//   of one sentence. They resolve in the claim files that
//   hrm-testpipeline/acceptance/aa-s3.json lists under `inventories`.
//
// ONE LINE OF RTL ANSWERS BOTH REGISTERS. agnus_beamcounter.v:107-114:
//
//     VPOSR/VPOSW  -> {long_frame, 1'b0, ecs, ntsc, 2'b00, {2{aga}},
//                      long_line, 4'b0000, vpos[10:8]}
//     VHPOSR       -> {vpos[7:0], hpos[8:1]}
//
//   so VHPOSR is a straight pack of the two counters and VPOSR is a status
//   word with the three highest vertical bits at the bottom.
//
// THE CHIP IDENTIFIER IS THE INTERESTING HALF. VPOSR's table gives bits 14 to
//   8 as I6 down to I0 and lists what each part reports:
//
//     8361(regular) or 8370(fat) (agnus-ntsc)   = 10
//     8367(pal) or 8371(fat-pal) (agnus-pal)    = 00
//     8372(fat-hr) (agnushr), thru rev. 4       = 20 PAL, 30 NTSC
//     8372(fat-hr) (agnushr), rev. 5            = 21 PAL, 31 NTSC
//     8374(alice)                               = 22 PAL, 32 NTSC
//
//   This RTL is an Alice: it is the AA chip set, `aga` is its own input, and
//   the register list this document prints is the one it decodes. So a fixture
//   built with aga and ecs high and ntsc low should read 22 in those seven
//   bits, and one with ntsc high should read 32.
//
//   Assertion 4 measures that and reports the value it finds rather than
//   asserting a bit pattern taken from the RTL. Whatever it finds is the
//   measurement; the document's list is the oracle.

#include "Vagnus_beamcounter.h"
#include "verilated.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
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

constexpr uint16_t A_VPOSR  = 0x004;
constexpr uint16_t A_VPOSW  = 0x02A;
constexpr uint16_t A_VHPOSR = 0x006;
constexpr uint16_t A_VHPOSW = 0x02C;

class Harness {
public:
    Harness(bool aga = true, bool ecs = true, bool ntsc = false)
        : dut(new Vagnus_beamcounter) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->cck = 0; dut->ntsc = ntsc ? 1 : 0;
        dut->aga = aga ? 1 : 0; dut->ecs = ecs ? 1 : 0; dut->a1k = 0;
        dut->reg_address_in = 0; dut->data_in = 0;
        for (int i = 0; i < 8; ++i) step();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) step();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void step() { dut->clk = 1; dut->eval(); dut->clk = 0; dut->eval(); }
    void cck_tick() { dut->cck = 1; step(); dut->cck = 0; step(); }

    void write(uint16_t word_addr, uint16_t value) {
        dut->reg_address_in = (word_addr >> 1) & 0xFF;
        dut->data_in = value;
        step();
        dut->reg_address_in = 0;
        dut->data_in = 0;
        step();
    }

    uint16_t read(uint16_t word_addr) {
        dut->reg_address_in = (word_addr >> 1) & 0xFF;
        dut->eval();
        const uint16_t got = (uint16_t)dut->data_out;
        dut->reg_address_in = 0;
        dut->eval();
        return got;
    }

    Vagnus_beamcounter* dut;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks c;
    g_mutate = std::getenv("MUTATE");

    // The document's own list, for an Alice.
    constexpr uint16_t ID_ALICE_PAL  = 0x22;
    constexpr uint16_t ID_ALICE_NTSC = 0x32;

    // Each guard below is keyed to its OWN assertion's id: MUTATE=<id> moves
    // ONE thing in that assertion's stimulus, and every other assertion runs
    // its real stimulus. m_vpos1 and m_vpos2 used to be the SAME variable --
    // one line number fed both assertion 1 and assertion 2's stimulus -- which
    // meant selecting one assertion's mutation also disturbed the other's.
    // Split so each has its own id and its own default.
    const uint16_t m_vpos1 = mutating("FIXTURE-DELIVERS-KNOWN-VALUE") ? 0x0055u : 0x00AAu;
    const uint16_t m_vpos2 = mutating("VHPOSR-PACKS-VERTICAL-AND-HORIZONTAL") ? 0x0055u : 0x00AAu;
    const uint16_t m_high = mutating("VPOSR-CARRIES-THE-HIGH-BITS") ? 0x0000u : 0x0005u;
    // 4: see below -- this assertion's baseline may itself be red, so its
    // mutation direction is the opposite of every other assertion's.
    const bool     m_id   = mutating("VPOSR-REPORTS-THE-CHIP-IDENTIFIER");

    std::cout << "aa-beam-position-words -- AA_CONFORMANCE at agnus_beamcounter\n";
    if (g_mutate)
        std::cout << "MUTATE=" << g_mutate
                   << ": exactly the named assertion below must go red (or, "
                      "for VPOSR-REPORTS-THE-CHIP-IDENTIFIER, green), the "
                      "others must keep their baseline verdict\n";

    // --- 1. The fixture delivers a known value ------------------------------
    {
        Harness h;
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red selecting this
        // assertion's own id writes a different line number than the one
        // compared against.
        h.write(A_VPOSW, 0x0000);
        h.write(A_VHPOSW, (uint16_t)(m_vpos1 << 8));
        const uint16_t got = h.read(A_VHPOSR);
        c.note("after writing line 0xAA the packed word reads " + hex(got));
        c.expect(((got >> 8) & 0xFF) == 0xAA,
                 "FIXTURE-DELIVERS-KNOWN-VALUE: a line number written through "
                 "VHPOSW comes back in the high byte of VHPOSR, so this "
                 "fixture can put the beam where it wants it");
    }

    // --- 2. VHPOSR packs the vertical and horizontal counters ---------------
    {
        Harness h;
        // MUTATION VHPOSR-PACKS-VERTICAL-AND-HORIZONTAL stimulus red
        // selecting this assertion's own id writes a different line number
        // than the one the assertion compares against, so the high byte does
        // not match.
        h.write(A_VPOSW, 0x0000);
        // hpos is put MID-LINE, not at zero: a horizontal counter of zero is
        // the end-of-line condition, and the first version of this assertion
        // wrote it there and watched the line number step from 0xAA to 0xAB
        // while it measured.
        h.write(A_VHPOSW, (uint16_t)((m_vpos2 << 8) | 0x20));
        const uint16_t before = h.read(A_VHPOSR);
        for (int i = 0; i < 6; ++i) h.cck_tick();
        const uint16_t after = h.read(A_VHPOSR);
        c.note("the packed word reads " + hex(before) + ", and after six "
               "colour clocks " + hex(after));
        c.expect(((before >> 8) & 0xFF) == 0xAA &&
                 ((after >> 8) & 0xFF) == 0xAA &&
                 (after & 0xFF) != (before & 0xFF),
                 "VHPOSR-PACKS-VERTICAL-AND-HORIZONTAL: the high byte holds a "
                 "line number that does not move while the low byte follows "
                 "the beam across it, which is the two fields the AA table "
                 "prints for VHPOSR [p.28] [claims: opus:AA-vhpos.c22 opus:AA-vhposr.c1/c2/c3 opus:AA-vhposw.c1/c2 sol:AA-vhpos.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18 sol:AA-vhposr.c2/c3/c4/c5 sol:AA-vhposw.c3/c4/c5]");
    }

    // --- 3. VPOSR carries the vertical counter's highest bits ---------------
    {
        Harness h;
        // MUTATION VPOSR-CARRIES-THE-HIGH-BITS stimulus red selecting this assertion's own id writes no
        // high bits at all, so the three bits stay zero.
        h.write(A_VPOSW, m_high);
        const uint16_t got = h.read(A_VPOSR);
        c.note("after writing 5 into the high vertical bits, VPOSR reads " +
               hex(got) + ", whose low three bits are " +
               std::to_string(got & 7));
        c.expect((got & 7) == 5,
                 "VPOSR-CARRIES-THE-HIGH-BITS: the three highest bits of the "
                 "vertical counter come back in the three lowest bits of "
                 "VPOSR, which is the V10 to V8 the AA table prints at the "
                 "bottom of that word [p.29] [claims: opus:AA-vpos.c19/c26/c27 opus:AA-vposr.c1/c2/c3/c5 opus:AA-vposw.c1/c2/c3/c5 sol:AA-vpos.c1/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c24/c25 sol:AA-vposr.c3/c4/c5/c6 sol:AA-vposw.c2/c3/c4/c5]");
    }

    // --- 4. VPOSR's seven identifier bits -----------------------------------
    {
        Harness pal(true, true, false);
        Harness ntsc(true, true, true);
        const uint16_t id_pal  = (uint16_t)((pal.read(A_VPOSR) >> 8) & 0x7F);
        const uint16_t id_ntsc = (uint16_t)((ntsc.read(A_VPOSR) >> 8) & 0x7F);
        // MUTATION VPOSR-REPORTS-THE-CHIP-IDENTIFIER oracle green This
        // assertion's baseline may be red, and if it is, no stimulus mutation
        // can make it redder. selecting this assertion's own id compares against the value measured
        // instead of the value the document lists, which must make it GREEN --
        // proving the comparison is reading the register and not a constant.
        const uint16_t want_pal  = m_id ? id_pal  : ID_ALICE_PAL;
        const uint16_t want_ntsc = m_id ? id_ntsc : ID_ALICE_NTSC;
        c.note("VPOSR bits 14-8 read " + hex(id_pal) + " with PAL and " +
               hex(id_ntsc) + " with NTSC; the AA list gives an Alice " +
               hex(ID_ALICE_PAL) + " and " + hex(ID_ALICE_NTSC));
        c.expect(id_pal == want_pal && id_ntsc == want_ntsc,
                 "VPOSR-REPORTS-THE-CHIP-IDENTIFIER: the seven identifier bits "
                 "read 22 with PAL and 32 with NTSC, which is what the AA "
                 "table's own list gives for 8374(alice) -- the part this "
                 "specification describes [p.29] "
                 "[claims: opus:AA-vhpos.c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c21 opus:AA-vpos.c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c20/c25 sol:AA-vpos.c2/c3/c4/c5/c6/c7/c8/c19/c26]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary (insufficient): The claim names the chip "
             "that implements the register. No port of agnus_beamcounter carries "
             "a chip identity. [claims: opus:AA-bplhdat.c2 opus:AA-bplhpth.c3 "
             "opus:AA-bplhptl.c3 opus:AA-bplhstop.c3 opus:AA-bplhstrt.c3 "
             "opus:AA-bplxpth.c3 opus:AA-bplxptl.c3 opus:AA-clxcon.c3 "
             "opus:AA-clxcon2.c3 opus:AA-sprhstop.c3 opus:AA-sprhstrt.c3 "
             "opus:AA-sprxctl.c3 opus:AA-sprxdata.c3 opus:AA-sprxdatb.c3 "
             "opus:AA-sprxpos.c3 opus:AA-sprxpth.c3 opus:AA-sprxptl.c3 "
             "opus:AA-strequ.c3 opus:AA-strhor.c3 opus:AA-strlong.c3 "
             "opus:AA-strvbl.c3 opus:AA-vbstrt.c3 opus:AA-vsstop.c3 "
             "opus:AA-vsstrt.c3 opus:AA-vtotal.c3 sol:AA-bplhmod.c4 "
             "sol:AA-bplhpth.c5 sol:AA-bplhptl.c5 sol:AA-bplhstop.c4 "
             "sol:AA-bplhstrt.c4 sol:AA-bplxdat.c10 sol:AA-bplxpth.c10 "
             "sol:AA-bplxptl.c10 sol:AA-clxcon.c3 sol:AA-clxcon2.c4 "
             "sol:AA-sprhstop.c4/c23 sol:AA-sprhstrt.c4 sol:AA-sprxctl.c3 "
             "sol:AA-sprxdata.c3 sol:AA-sprxdatb.c3 sol:AA-sprxpos.c3 "
             "sol:AA-sprxpth.c3 sol:AA-sprxptl.c3 sol:AA-strequ.c5 "
             "sol:AA-strlong.c5 sol:AA-strobes.c3 sol:AA-strvbl.c5 "
             "sol:AA-vbstop.c4 sol:AA-vbstrt.c4 sol:AA-vsstop.c4 "
             "sol:AA-vsstrt.c4 sol:AA-vtotal.c4]");
    c.record(
             "DECLINED at this boundary (insufficient): The register is decoded "
             "somewhere in this design but not at the boundary this package "
             "drives, and no other slice 3 lot claims it. Recorded so the slice "
             "closes with nothing unclassified. [claims: opus:AA-bplamx.c1/c2 "
             "opus:AA-bplhdat.c1/c5 opus:AA-bplhmod.c1/c2/c5/c6 "
             "opus:AA-bplhpth.c1/c2/c6 opus:AA-bplhptl.c1/c2/c6/c7/c8/c9/c10/c11 "
             "opus:AA-bplhstop.c1/c2/c5/c6/c7/c8/c9/c10 "
             "opus:AA-bplhstrt.c1/c2/c5/c6 opus:AA-brdntran.c1/c2 "
             "opus:AA-brdrblnk.c1 opus:AA-esprmx.c1/c2/c3 opus:AA-extblken.c1/c2 "
             "opus:AA-osprmx.c1/c2/c3 opus:AA-shresrow.c1/c2 "
             "opus:AA-sprpixel.c1/c2/c3/c4/c5 opus:AA-strequ.c1/c6 "
             "opus:AA-strhor.c1 opus:AA-strlong.c1/c7/c8 "
             "opus:AA-strobes.c1/c2/c3/c4/c5 opus:AA-strvbl.c1/c6 "
             "opus:AA-vbstop.c1/c2 opus:AA-vbstrt.c1/c2/c5/c6/c7 "
             "opus:AA-vsstop.c1/c2/c5 opus:AA-vsstrt.c1/c2 "
             "opus:AA-vtotal.c1/c2/c5/c6/c7/c8 opus:AA-zdclken.c1/c2/c3 "
             "sol:AA-bplcon3.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12 "
             "sol:AA-bplcon4.c1/c2/c3/c4/c5/c6/c7/c8 sol:AA-bplhdat.c3/c4/c5 "
             "sol:AA-bplhmod.c2/c3/c5/c6 sol:AA-bplhpt.c1/c2/c3/c4/c5/c6 "
             "sol:AA-bplhpth.c3/c4/c6 sol:AA-bplhptl.c3/c4/c6 "
             "sol:AA-bplhstop.c2/c3/c5/c6/c7/c8/c9/c10 "
             "sol:AA-bplhstrt.c2/c3/c5/c6 sol:AA-sprite-display.c1/c2/c3/c4/c5 "
             "sol:AA-sprxpt.c1/c2 sol:AA-strequ.c3/c4/c6 sol:AA-strhor.c3/c4/c6 "
             "sol:AA-strlong.c3/c4/c6/c8 sol:AA-strobes.c1/c2/c4/c5 "
             "sol:AA-strvbl.c3/c4/c6 sol:AA-vb-window.c1/c2/c3 "
             "sol:AA-vbstop.c2/c3/c5 sol:AA-vbstrt.c2/c3/c5 "
             "sol:AA-vsstop.c2/c3/c5/c6 sol:AA-vsstrt.c2/c3/c5 "
             "sol:AA-vtotal.c2/c3/c5/c6/c7/c8]");
    c.record(
             "DECLINED at this boundary (insufficient): VHPOSR doubles as the "
             "light pen latch. Showing that needs a light pen input and a "
             "capture, which this package does not drive. [claims: "
             "opus:AA-vhposr.c5 opus:AA-vhposw.c5]");
    c.record(
             "DECLINED at this boundary (not-hardware): The claim is about the "
             "document -- a mark, a legend, or a disagreement between its two "
             "statements of a register. [claims: opus:AA-bplhdat.c3/c4 "
             "opus:AA-bplhmod.c3/c4 opus:AA-bplhpth.c4/c5 opus:AA-bplhptl.c4/c5 "
             "opus:AA-bplhstop.c4 opus:AA-bplhstrt.c4 "
             "opus:AA-bplxdat.c3/c5/c12/c13 opus:AA-bplxpth.c5/c7/c8 "
             "opus:AA-bplxptl.c5/c8/c9 opus:AA-clxcon2.c1/c4 "
             "opus:AA-legend.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18 "
             "opus:AA-sprhstop.c4 opus:AA-sprhstrt.c4 opus:AA-sprxctl.c4/c5 "
             "opus:AA-sprxdata.c4 opus:AA-sprxdatb.c4 opus:AA-sprxpos.c4 "
             "opus:AA-sprxpth.c5 opus:AA-sprxptl.c5 opus:AA-strequ.c2/c4/c5 "
             "opus:AA-strhor.c2/c4/c5 opus:AA-strlong.c2/c4/c5/c6 "
             "opus:AA-strvbl.c2/c4/c5 opus:AA-vbstop.c3/c4 opus:AA-vbstrt.c4 "
             "opus:AA-vhposr.c4 opus:AA-vhposw.c3/c4 opus:AA-vposr.c4 "
             "opus:AA-vposw.c4 opus:AA-vsstop.c4 opus:AA-vsstrt.c4 "
             "opus:AA-vtotal.c4 sol:AA-bplhdat.c1/c2 sol:AA-bplhmod.c1 "
             "sol:AA-bplhpth.c1/c2 sol:AA-bplhptl.c1/c2 sol:AA-bplhstop.c1 "
             "sol:AA-bplhstrt.c1 "
             "sol:AA-bplxdat.c11/c12/c13/c14/c15/c16/c17/c18/c19/c20 "
             "sol:AA-bplxpth.c12/c13/c14/c15/c16/c17/c18/c19/c20/c21 "
             "sol:AA-bplxptl.c13/c14/c15/c16/c17/c18/c19/c20/c21/c22 "
             "sol:AA-clxcon2.c1/c2 sol:AA-sprhstop.c1 sol:AA-sprhstrt.c1 "
             "sol:AA-sprxctl.c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19 "
             "sol:AA-sprxdata.c4/c5/c6/c7/c8/c9/c10/c11 "
             "sol:AA-sprxdatb.c4/c5/c6/c7/c8/c9/c10/c11 "
             "sol:AA-sprxpos.c5/c6/c7/c8/c9/c10/c11/c12 "
             "sol:AA-sprxpth.c4/c5/c6/c7/c8/c9/c10/c11 "
             "sol:AA-sprxptl.c4/c5/c6/c7/c8/c9/c10/c11 sol:AA-strequ.c1/c2 "
             "sol:AA-strhor.c1/c2/c5 sol:AA-strlong.c1/c2/c7 sol:AA-strvbl.c1/c2 "
             "sol:AA-vbstop.c1 sol:AA-vbstrt.c1 sol:AA-vhposr.c1 "
             "sol:AA-vhposw.c1/c2 sol:AA-vposr.c1/c2 sol:AA-vposw.c1 "
             "sol:AA-vsstop.c1 sol:AA-vsstrt.c1 sol:AA-vtotal.c1]");
    c.record(
             "DECLINED at this boundary (not-hardware): The line names the "
             "identifier some OTHER Agnus reports -- an 8361, an 8367, an 8372. "
             "This RTL is an Alice; what a different part returns is a fact "
             "about that part. [claims: opus:AA-vpos.c21/c22/c23/c24/c28 "
             "sol:AA-vpos.c20/c21/c22/c23]");
    c.record(
             "DECLINED at this boundary (outside-uut): CLXCON and CLXCON2 are "
             "decoded in denise_collision, where aa-clxcon2-address and "
             "aa-collision-data-register grade them. [claims: "
             "opus:AA-clxcon.c1/c2/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c21/c22 "
             "opus:AA-clxcon2.c2/c5/c6/c7/c8/c9/c10/c11/c12/c13 "
             "sol:AA-clxcon.c1/c2/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c21/c22 "
             "sol:AA-clxcon2.c3/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18]");
    c.record(
             "DECLINED at this boundary (outside-uut): The bitplane data "
             "registers are decoded in denise_bitplanes and graded by "
             "aa-bitplane-data-registers at that boundary. [claims: "
             "opus:AA-bplxdat.c1/c2/c4/c6/c7/c8/c9/c10/c11/c14 "
             "sol:AA-bplxdat.c1/c2/c3/c4/c5/c6/c7/c8/c9/c21/c22/c23/c24/c25/c26/c27]");
    c.record(
             "DECLINED at this boundary (outside-uut): The bitplane pointers and "
             "modulos are decoded in agnus_bitplanedma, a module of its own. "
             "[claims: opus:AA-bplxpth.c1/c2/c4/c6/c9 "
             "opus:AA-bplxptl.c1/c2/c4/c6/c7/c10 "
             "sol:AA-bplxpth.c1/c2/c3/c4/c5/c6/c7/c8/c9/c11/c22 "
             "sol:AA-bplxptl.c1/c2/c3/c4/c5/c6/c7/c8/c9/c11/c12/c23]");
    c.record(
             "DECLINED at this boundary (outside-uut): The sprite registers are "
             "decoded in agnus_spritedma and denise_sprites. agnus_beamcounter "
             "produces the beam position they are compared against and nothing "
             "else. [claims: opus:AA-sprhstop.c1/c2/c5/c6/c7/c8/c9 "
             "opus:AA-sprhstrt.c1/c2/c5/c6 "
             "opus:AA-sprxctl.c1/c2/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20 "
             "opus:AA-sprxdata.c1/c2 "
             "opus:AA-sprxdatb.c1/c2/c5/c6/c7/c8/c9/c10/c11/c12 "
             "opus:AA-sprxpos.c1/c2/c5/c6/c7/c8/c9/c10 opus:AA-sprxpth.c1/c2/c4 "
             "opus:AA-sprxptl.c1/c2/c4/c6/c7 "
             "sol:AA-sprhstop.c2/c3/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c21/c22 "
             "sol:AA-sprhstrt.c2/c3/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c21 "
             "sol:AA-sprxctl.c1/c2/c20/c21/c22/c23/c24/c25/c26/c27/c28/c29/c30/c31/c32/c33/c34/c35 "
             "sol:AA-sprxdata.c1/c2/c12/c13/c14/c15/c16/c17/c18/c19 "
             "sol:AA-sprxdatb.c1/c2 sol:AA-sprxpos.c1/c2/c4/c13/c14/c15/c16 "
             "sol:AA-sprxpth.c1/c2/c12 sol:AA-sprxptl.c1/c2/c12]");

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
