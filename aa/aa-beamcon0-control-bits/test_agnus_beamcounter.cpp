// aa-beamcon0-control-bits -- agnus_beamcounter conformance test
//
// Test purpose: the AA specification's alphabetical entry for BEAMCON0 prints
//   a bit table and a paragraph per bit. This grades the bits, at the module
//   that holds the register. It is the complement of aa-beam-counter-registers,
//   which grades where seventeen registers are decoded and names the sync pulse
//   shapes as a non-goal.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 12 and 13, the BEAMCON0
//   entry and its bit descriptions. The document prints no page numbers of its
//   own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the display window, interlace field
//   alternation, light-pen capture, genlock, the UHRES pointer chain, or what
//   a monitor makes of any of it. It grades what BEAMCON0's bits reach.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the AA specification independently, and ids such as
//   `sol:AA-beamcon0.c7` and `opus:AA-beamcon0.c6` each name one reader's
//   reading of one sentence. They resolve in the claim files that
//   hrm-testpipeline/acceptance/aa-s2.json lists under `inventories`.
//
// SIX OF SIXTEEN BITS ARE WIRED AND THE RTL SAYS SO IN ITS OWN COMMENTS.
//   agnus_beamcounter.v:128-142 declares the whole field and comments out nine
//   of the wires:
//
//     wire harddis      = beamcon0_reg[14];      // live
//     //wire lpendis    = beamcon0_reg[13];
//     wire varvben      = beamcon0_reg[12];      // live
//     wire loldis       = beamcon0_reg[11];      // live
//     //wire cscben     = beamcon0_reg[10];
//     wire varvsyen     = beamcon0_reg[ 9];      // live
//     wire varhsyen     = beamcon0_reg[ 8];      // live
//     wire varbeamen    = beamcon0_reg[ 7];      // live
//     //wire displaydual= beamcon0_reg[ 6];
//     //wire displaypal = beamcon0_reg[ 5];
//     //wire varcsyen   = beamcon0_reg[ 4];
//     //wire blanken    = beamcon0_reg[ 3];
//     //wire csynctrue  = beamcon0_reg[ 2];
//     //wire vsynctrue  = beamcon0_reg[ 1];
//     //wire hsynctrue  = beamcon0_reg[ 0];
//
//   So a claim about bit 14 and a claim about bit 2 fail differently here, and
//   the lot says which is which rather than declining them together.
//
// TWO PINS CARRY THE LIVE BITS OUT, AND ONE OF THEM IS AN OR. `varbeamen_out`
//   is bit 7 alone, but
//
//     assign harddis_out = harddis || varbeamen || varvben;      (:236)
//
//   so the hard-stop pin answers for bits 14, 12 and 7 together. The first
//   version of assertion 3 asserted that writing bit 7 leaves the other pin
//   alone; that is false by construction, and it took a red to notice. Each
//   assertion now pairs its positive with a bit of the SAME register that must
//   not move that pin.
//
// THE WRITE IS GATED ON ecs AND THE ROW IS MARKED H. agnus_beamcounter.v:123
//   is `else if ((reg_address_in[8:1] == BEAMCON0[8:1]) && ecs)`, and the
//   address list marks BEAMCON0's row `H`, which p.6 defines as new for the
//   HiRes chip set. Assertion 4 writes the same word with ecs low and shows the
//   pin does not move -- the register and its condition grade each other.
//
// AND THE SYNC POLARITY BITS ARE THE ONE PLACE THIS PACKAGE GOES RED. The
//   document says of bits 2, 1 and 0:
//
//     HSYTRUE,VSYTRUE,CSYTRUE= These change the polarity of the sync
//
//   `_hsync`, `_vsync` and `_csync` are ports of this module, so the signal the
//   sentence is about is right here. The three wires that would read the bits
//   are commented out, so the pins cannot move. Assertion 5 runs a whole line
//   twice, once with the three bits clear and once with them set, and compares
//   the two waveforms sample for sample.
//
//   This is the same defect as NM-HRM-032, reached from a different document.
//   It is recorded as corroboration on that finding's page rather than as a
//   new key: a second oracle agreeing does more for a finding than a second
//   number would.

#include "Vagnus_beamcounter.h"
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
constexpr uint16_t A_BEAMCON0 = 0x1DC;
constexpr uint16_t A_HTOTAL   = 0x1C0;

constexpr uint16_t HSYTRUE   = 1u << 0;
constexpr uint16_t VSYTRUE   = 1u << 1;
constexpr uint16_t CSYTRUE   = 1u << 2;
constexpr uint16_t VARBEAMEN = 1u << 7;
constexpr uint16_t VARVBEN   = 1u << 12;
constexpr uint16_t LPENDIS   = 1u << 13;
constexpr uint16_t HARDDIS   = 1u << 14;

class Harness {
public:
    explicit Harness(bool ecs = true) : dut(new Vagnus_beamcounter) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->cck = 0; dut->ntsc = 0; dut->aga = 1;
        dut->ecs = ecs ? 1 : 0; dut->a1k = 0;
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

    // One colour clock: the horizontal counter advances on cck's high half.
    void cck_tick() {
        dut->cck = 1; step();
        dut->cck = 0; step();
    }

    void write(uint16_t word_addr, uint16_t value) {
        dut->reg_address_in = (word_addr >> 1) & 0xFF;
        dut->data_in = value;
        step();
        dut->reg_address_in = 0;
        dut->data_in = 0;
        step();
    }

    bool harddis()   { dut->eval(); return dut->harddis_out != 0; }
    bool varbeamen() { dut->eval(); return dut->varbeamen_out != 0; }

    // The three sync pins sampled once per colour clock, for a whole PAL line
    // and a little more, so a pulse anywhere in the line is in the trace.
    std::vector<uint8_t> sync_trace(int ticks = 260) {
        std::vector<uint8_t> t;
        t.reserve(ticks);
        for (int i = 0; i < ticks; ++i) {
            cck_tick();
            dut->eval();
            t.push_back((uint8_t)((dut->_hsync ? 1 : 0) |
                                  (dut->_vsync ? 2 : 0) |
                                  (dut->_csync ? 4 : 0)));
        }
        return t;
    }

    Vagnus_beamcounter* dut;
};

// How many distinct values a trace takes, so a flat pin can be told from a
// pulsing one without asserting where the pulse is.
static int levels(const std::vector<uint8_t>& t, uint8_t mask) {
    bool seen[8] = {false};
    for (uint8_t v : t) seen[v & mask ? 1 : 0] = true;
    return (seen[0] ? 1 : 0) + (seen[1] ? 1 : 0);
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks c;
    g_mutate = std::getenv("MUTATE");

    // Each guard below is keyed to its OWN assertion's id: MUTATE=<id> changes
    // ONE thing in that assertion's stimulus, and every other assertion runs
    // its real stimulus.
    const uint16_t m_hard = mutating("HARDDIS-AT-BIT-14") ? LPENDIS : HARDDIS;
    const uint16_t m_beam = mutating("VARBEAMEN-AT-BIT-7") ? HARDDIS : VARBEAMEN;
    const bool     m_ecs  = mutating("BEAMCON0-WRITE-IS-CHIPSET-GATED") ? true : false;
    const uint16_t m_pol  = HSYTRUE | VSYTRUE | CSYTRUE;

    std::cout << "aa-beamcon0-control-bits -- AA_CONFORMANCE at agnus_beamcounter\n";
    if (g_mutate)
        std::cout << "MUTATE=" << g_mutate
                   << ": exactly the named assertion below must go red (or, "
                      "for SYNC-POLARITY-BITS-CHANGE-THE-PINS, whose baseline "
                      "is already red, green), the others must keep their "
                      "baseline verdict\n";

    // --- 1. The fixture delivers a known value ------------------------------
    {
        Harness h;
        const bool hard0 = h.harddis(), beam0 = h.varbeamen();
        const std::vector<uint8_t> t = h.sync_trace();
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red selecting this
        // assertion's own id samples a single colour clock instead of a line,
        // so no pin can be seen to move and the trace has one level rather
        // than two.
        const std::vector<uint8_t> t2 = mutating("FIXTURE-DELIVERS-KNOWN-VALUE")
                                             ? std::vector<uint8_t>(t.begin(), t.begin() + 1)
                                             : t;
        c.note(std::string("out of reset harddis_out is ") + (hard0 ? "1" : "0") +
               ", varbeamen_out is " + (beam0 ? "1" : "0") +
               "; over a line _hsync takes " + std::to_string(levels(t2, 1)) +
               " level(s)");
        c.expect(!hard0 && !beam0 && levels(t2, 1) == 2,
                 "FIXTURE-DELIVERS-KNOWN-VALUE: both control pins are low out "
                 "of reset and the horizontal sync pin pulses within one line, "
                 "so this fixture drives a beam and starts from a state it can "
                 "account for");
    }

    // --- 2. HARDDIS at bit 14 reaches the hard-stop pin ---------------------
    {
        Harness h;
        // MUTATION HARDDIS-AT-BIT-14 stimulus red selecting this assertion's own id writes bit 13
        // instead. That is LPENDIS, whose wire this RTL comments out, so the
        // pin does not move.
        const bool before = h.harddis();
        h.write(A_BEAMCON0, m_hard);
        const bool after = h.harddis();
        Harness g;
        g.write(A_BEAMCON0, LPENDIS);        // a bit of the same register
        const bool dead = g.harddis();
        c.note(std::string("harddis_out: none of the three set ") +
               (before ? "1" : "0") + ", bit 14 set " + (after ? "1" : "0") +
               ", bit 13 set " + (dead ? "1" : "0"));
        c.expect(!before && after && !dead,
                 "HARDDIS-AT-BIT-14: writing bit 14 of BEAMCON0 raises the "
                 "module's hard-stop output while another bit of the same "
                 "register does not, which is the bit the AA table names "
                 "HARDDIS and describes as disabling the hardwired stops, "
                 "cleared on reset [p.12,13] "
                 "[claims: sol:AA-beamcon0.c7/c22/c23 opus:AA-beamcon0.b14 "
                 "opus:AA-beamcon0.c6/c7]");
    }

    // --- 3. VARBEAMEN at bit 7 reaches its own pin --------------------------
    {
        Harness h;
        // MUTATION VARBEAMEN-AT-BIT-7 stimulus red selecting this assertion's own id writes bit 14
        // instead, which is HARDDIS and does not reach this pin.
        const bool before = h.varbeamen();
        h.write(A_BEAMCON0, m_beam);
        const bool after = h.varbeamen();
        Harness g;
        g.write(A_BEAMCON0, HARDDIS);
        const bool other = g.varbeamen();
        c.note(std::string("varbeamen_out: out of reset ") +
               (before ? "1" : "0") + ", bit 7 set " + (after ? "1" : "0") +
               ", bit 14 set " + (other ? "1" : "0"));
        c.expect(!before && after && !other,
                 "VARBEAMEN-AT-BIT-7: writing bit 7 of BEAMCON0 raises the "
                 "module's varbeamen output while bit 14 does not, which is "
                 "the bit the AA table names VARBEAMEN and describes as "
                 "enabling the variable beam counter comparators [p.12,13] "
                 "[claims: sol:AA-beamcon0.c14/c45/c46 opus:AA-beamcon0.b7 "
                 "opus:AA-beamcon0.c28/c29]");
    }

    // --- 4. And the register only takes the write on the newer chip set -----
    {
        Harness h(m_ecs ? true : false);
        // MUTATION BEAMCON0-WRITE-IS-CHIPSET-GATED stimulus red selecting this assertion's own id builds
        // the fixture with ecs high, so the write lands and the pin moves.
        h.write(A_BEAMCON0, HARDDIS);
        const bool after = h.harddis();
        Harness g(true);
        g.write(A_BEAMCON0, HARDDIS);
        const bool with_ecs = g.harddis();
        c.note(std::string("the same write with the older chip set gives ") +
               (after ? "1" : "0") + ", with the newer one " +
               (with_ecs ? "1" : "0"));
        c.expect(!after && with_ecs,
                 "BEAMCON0-WRITE-IS-CHIPSET-GATED: the same word written at "
                 "$1DC moves the pin on the newer chip set and not on the "
                 "older one, which is what the AA list's `H` mark on "
                 "BEAMCON0's row means by new for the HiRes chip set [p.6,12] "
                 "[claims: sol:AA-beamcon0.c1 opus:AA-beamcon0.c1]");
    }

    // --- 5. The sync polarity bits ------------------------------------------
    {
        Harness a;
        const std::vector<uint8_t> plain = a.sync_trace();
        Harness b;
        // MUTATION SYNC-POLARITY-BITS-CHANGE-THE-PINS stimulus green This
        // assertion's baseline is RED: the three wires that would read the
        // bits are commented out, so the pins cannot move and no stimulus
        // mutation can make it redder. The mutation goes the other way and
        // must make it GREEN -- selecting this assertion's own id starts the
        // second trace three colour clocks later, which does move the pins
        // relative to the first, so the comparison is shown able to see a
        // difference when there is one. (Writing HTOTAL was tried first and
        // changed nothing, because the line length is only consulted once
        // VARBEAMEN is set.)
        if (mutating("SYNC-POLARITY-BITS-CHANGE-THE-PINS"))
            for (int i = 0; i < 3; ++i) b.cck_tick();
        b.write(A_BEAMCON0, m_pol);
        const std::vector<uint8_t> flipped = b.sync_trace();

        int differ = 0;
        for (size_t i = 0; i < plain.size() && i < flipped.size(); ++i)
            if (plain[i] != flipped[i]) ++differ;
        c.note("over a line the three sync pins differ in " +
               std::to_string(differ) + " of " + std::to_string(plain.size()) +
               " samples between bits clear and bits set; _hsync takes " +
               std::to_string(levels(plain, 1)) + " level(s) either way");
        c.expect(differ > 0,
                 "SYNC-POLARITY-BITS-CHANGE-THE-PINS: setting HSYTRUE, VSYTRUE "
                 "and CSYTRUE changes what comes out of the module's three "
                 "sync pins, which is what the AA table means by `These change "
                 "the polarity of the sync` [p.13] "
                 "[claims: sol:AA-beamcon0.c19/c20/c21/c62/c63/c64 opus:AA-beamcon0.b2 opus:AA-beamcon0.b1 opus:AA-beamcon0.b0 opus:AA-beamcon0.c43]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary: nine of BEAMCON0's sixteen bits are "
             "stored and reach nothing. MEASURED at agnus_beamcounter.v:128-142, "
             "where the wire that would read each one is declared and COMMENTED "
             "OUT, and beamcon0_reg is read nowhere else in the tree. LPENDIS, "
             "CSCBEN, DUAL, PAL, VARCSYEN and the formerly-BLANKEN bit are "
             "declined here; the three sync-polarity bits are graded instead, "
             "because their pins are ports of this module [claims: "
             "opus:AA-beamcon0.c8/c9/c10/c15/c16/c17/c18/c19/c30/c31/c32/c33/c34/c35/c37/c38 "
             "sol:AA-beamcon0.c6/c8/c11/c15/c16/c17/c18/c24/c25/c26/c33/c34/c35/c47/c48/c49/c50/c51/c52/c53/c54 "
             "opus:AA-beamcon0.b10 opus:AA-beamcon0.b13 opus:AA-beamcon0.b15 "
             "opus:AA-beamcon0.b3 opus:AA-beamcon0.b3formerly "
             "opus:AA-beamcon0.b4 opus:AA-beamcon0.b5 opus:AA-beamcon0.b6]");
    c.record(
             "DECLINED at this boundary: VARVBEN, LOLDIS, VARVSYEN and VARHSYEN "
             "are wired in this RTL, but what each changes is a display timing "
             "this package does not drive [claims: "
             "opus:AA-beamcon0.c11/c12/c13/c14/c20/c26/c27/c36 "
             "sol:AA-beamcon0.c9/c10/c12/c13/c27/c28/c29/c30/c31/c32/c36/c37/c42/c43/c44 "
             "opus:AA-beamcon0.b11 opus:AA-beamcon0.b12 opus:AA-beamcon0.b8 "
             "opus:AA-beamcon0.b9]");
    c.record(
             "DECLINED at this boundary: the sentences that say when the "
             "variable sync and blank signals are set and reset, against HSSTRT, "
             "HSSTOP, HCENTER, VSSTRT, VSSTOP and the field length. Those "
             "comparators run inside this module; showing one needs a display "
             "window programmed and a field walked [claims: "
             "opus:AA-beamcon0.c21/c22/c23/c24/c25/c39/c40/c41/c42 "
             "sol:AA-beamcon0.c38/c39/c40/c41/c55/c56/c57/c58/c59/c60/c61]");
    c.record(
             "DECLINED at this boundary: the claims that the document's two "
             "statements of BEAMCON0 differ from each other. Which of them is "
             "right is a fact about the document [claims: opus:AA-beamcon0.c4/c5 "
             "sol:AA-beamcon0.c4/c5]");
    c.record(
             "DECLINED at this boundary: the chip column. No port of "
             "agnus_beamcounter carries a chip identity [claims: "
             "opus:AA-beamcon0.c3 sol:AA-beamcon0.c3]");
    c.record(
             "DECLINED at this boundary: the W access letter, which the chip-set "
             "assertion's own write already measures. A second record here would "
             "put a second home on one measurement [claims: opus:AA-beamcon0.c2 "
             "sol:AA-beamcon0.c2]");

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
