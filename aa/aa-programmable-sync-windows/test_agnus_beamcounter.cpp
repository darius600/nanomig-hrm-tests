// aa-programmable-sync-windows -- agnus_beamcounter conformance test
//
// Test purpose: the AA specification's alphabetical entries give the
//   programmable horizontal timing registers a bit layout and a meaning each --
//   HTOTAL the line length in colour clocks, HSSTRT and HSSTOP the two edges of
//   horizontal sync, HBSTRT and HBSTOP the two edges of horizontal blanking --
//   and put the value in the written word's bits 7 to 0. This runs whole lines
//   and grades where the edges actually land.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF page 24, the alphabetical
//   entries HBSTOP, HBSTRT, HCENTER, HSSTOP, HSSTRT and HTOTAL and the bit
//   table HBSTOP and HBSTRT share; LACE on page 17; and the sentence on page
//   4 that says the blanking registers were added to LISA. The document
//   prints no page numbers of its own; the corpus numbering is the PDF's.
// Non-goals: vertical timing, interlace field alternation, HCENTER's vsync
//   position on the long field, the secondary beam counter HHPOS, and the
//   35 ns fine-position bits. The first three need whole frames; the last two
//   are below or outside this boundary and are recorded as such.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the AA specification independently, and ids such as
//   `opus:AA-htotal.c1` and `sol:AA-htotal.c1` each name one reader's reading
//   of one sentence. They resolve in the claim files that
//   hrm-testpipeline/acceptance/aa-s1.json lists under `inventories`.
//
// WHY THIS PACKAGE EXISTS SEPARATELY FROM aa-beam-counter-registers. That
//   package grades where seventeen beam registers are decoded, and says in its
//   own header that HSSTRT, HSSTOP, HBSTRT, HBSTOP and HCENTER were left out of
//   its lot because grading them means running whole sync windows rather than
//   watching a counter wrap. This is that package. Same module, same fixture
//   shape, a different question: not "is the address decoded" but "where does
//   the edge land".
//
// HOW THE MODULE COUNTS. `hpos` is nine bits of which the lowest IS the cck
//   phase -- `assign hpos = {hpos_r, hpos_0}` with `hpos_0 = cck` (:258) -- so
//   hpos counts 140 ns steps and hpos[8:1] counts colour clocks. Every
//   programmable limit is written as `{data_in[7:0], 1'b0}` (:206-211), which
//   is the written byte shifted into the colour-clock field. The two halves
//   agree: a value V in bits 7 to 0 of the word names colour clock V, and the
//   comparison `hpos == hsstrt` (:390) fires there.
//
// WHICH BIT GATES WHICH REGISTER. The module reads
//
//     htotal  =             varbeamen ? htotal_reg  : HTOTAL_VAL << 1   (:223)
//     hsstrt  = varhsyen && varbeamen ? hsstrt_reg  : HSSTRT_VAL        (:224)
//     hbstrt  =             varbeamen ? hbstrt_reg  : HBSTRT_VAL        (:227)
//
//   so the sync pair needs TWO BEAMCON0 bits and the blanking pair needs one.
//   The AA entries print `(VARBEAMEN=1)` against these rows and say of HSSTRT
//   and HSSTOP only that "when they are active is governed by BEAMCON0"; the
//   assertions below measure both halves of that rather than assume either.
//
// WHAT IS NOT GRADED AND WHY, in one line each: HBSTRT and HBSTOP bits 10 to 8
//   are a fine position control in 35 ns steps, and this boundary advances in
//   140 ns steps, so no reading here can tell an implemented fine bit from a
//   dropped one. The module drops them -- `{data_in[7:0], 1'b0}` keeps eight --
//   and carries its own `// TODO fix this` at :210. That is a source
//   observation, recorded with record(), not an assertion.

#include "Vagnus_beamcounter.h"
#include "verilated.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace {
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

static std::string dec(int v) {
    std::ostringstream o;
    o << v;
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

// Word addresses as the AA alphabetical entries print them.
constexpr uint16_t A_BPLCON0  = 0x100;
constexpr uint16_t A_BEAMCON0 = 0x1DC;
constexpr uint16_t A_HTOTAL   = 0x1C0;
constexpr uint16_t A_HSSTOP   = 0x1C2;
constexpr uint16_t A_HBSTRT   = 0x1C4;
constexpr uint16_t A_HBSTOP   = 0x1C6;
constexpr uint16_t A_HSSTRT   = 0x1DE;

constexpr uint16_t VARHSYEN  = 1u << 8;   // BEAMCON0 bit 8
constexpr uint16_t VARBEAMEN = 1u << 7;   // BEAMCON0 bit 7
constexpr uint16_t LACE      = 1u << 2;   // BPLCON0 bit 2
constexpr uint16_t PALBIT    = 1u << 5;   // BEAMCON0 bit 5, the PAL bit
constexpr uint16_t LONG_LINE = 1u << 7;   // VPOSR bit 7
constexpr uint16_t A_VPOSR   = 0x004;

class Harness {
public:
    // `ntsc` decides the PAL bit out of reset -- `pal <= ~ntsc` (:127) --
    // and the PAL bit is half of the condition the HTOTAL entry gives for
    // long-line/short-line toggling, so one run below needs it low.
    explicit Harness(bool ntsc = false) : dut(new Vagnus_beamcounter) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->cck = 0; dut->ntsc = ntsc ? 1 : 0; dut->aga = 1; dut->ecs = 1;
        dut->a1k = 0;
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

    // One colour clock. hpos[8:1] advances on cck's high half and hpos[0] is
    // the cck phase itself, so a tick moves hpos by exactly two.
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

    int colour_clock() const { return (dut->hpos >> 1) & 0xFF; }

    // Run to the start of a line, so a measurement never straddles a wrap.
    void to_line_start(int limit = 2000) {
        int prev = colour_clock();
        for (int i = 0; i < limit; ++i) {
            cck_tick();
            const int now = colour_clock();
            if (now < prev) return;
            prev = now;
        }
    }

    // The highest colour clock reached before the counter wrapped. -1 if it
    // did not wrap within the limit.
    int line_top(int limit = 2000) {
        int prev = colour_clock();
        for (int i = 0; i < limit; ++i) {
            cck_tick();
            const int now = colour_clock();
            if (now < prev) return prev;
            prev = now;
        }
        return -1;
    }

    // How many colour clocks one whole line takes, from wherever the beam
    // stands to the next wrap.
    int line_length(int limit = 2000) {
        int prev = colour_clock();
        for (int i = 1; i <= limit; ++i) {
            cck_tick();
            const int now = colour_clock();
            if (now < prev) return i;
            prev = now;
        }
        return -1;
    }

    uint16_t read(uint16_t word_addr) {
        dut->reg_address_in = (word_addr >> 1) & 0xFF;
        dut->eval();
        const uint16_t got = dut->data_out;
        dut->reg_address_in = 0;
        dut->eval();
        return got;
    }

    // Where a one-bit output changes, in colour clocks, watched over one whole
    // line. Returns -1 if it never takes that value.
    int edge_of(const uint8_t& signal, bool want, int limit = 2000) {
        for (int i = 0; i < limit; ++i) {
            const bool before = signal != 0;
            cck_tick();
            if (!before && (signal != 0) == want && want) return colour_clock();
            if (before && (signal != 0) == want && !want) return colour_clock();
        }
        return -1;
    }

    Vagnus_beamcounter* dut;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks c;

    // Per-assertion mutation selector: MUTATE names ONE assertion's own
    // label, and only that assertion's stimulus site changes.
    //
    // FOUR ASSERTIONS USED TO SHARE ONE m_varb FLAG (leaving VARBEAMEN clear
    // under the boolean MUTATE=1 switch). Split into m_varb_fixture,
    // m_varb_htotal, m_varb_xcells and m_varb_blanking so each assertion's
    // own id clears VARBEAMEN for its OWN write only, and cannot disturb the
    // write any other of the four assertions makes to the same register.
    const uint16_t m_varb_fixture  = mutating("FIXTURE-RUNS-A-PROGRAMMED-LINE")
                                          ? (uint16_t)0u : VARBEAMEN;   // 1
    const uint16_t m_varb_htotal   = mutating("HTOTAL-IS-THE-HIGHEST-COUNT")
                                          ? (uint16_t)0u : VARBEAMEN;   // 2
    const uint16_t m_varb_xcells   = mutating("HTOTAL-IGNORES-THE-X-CELLS")
                                          ? (uint16_t)0u : VARBEAMEN;   // 4
    const uint16_t m_varb_blanking = mutating("HBSTRT-AND-HBSTOP-BOUND-BLANKING")
                                          ? (uint16_t)0u : VARBEAMEN;   // 7
    const uint16_t m_xcell = mutating("HTOTAL-IGNORES-THE-X-CELLS")
                                  ? (uint16_t)0x0000u : (uint16_t)0xFF00u; // 4
    const uint16_t m_gate  = mutating("HSSTRT-NEEDS-VARHSYEN-TOO")
                                  ? (uint16_t)(VARBEAMEN | VARHSYEN)
                                  : (uint16_t)VARBEAMEN;                   // 6
    const uint16_t m_hss   = mutating("HSSTRT-AND-HSSTOP-BOUND-HSYNC")
                                  ? A_HSSTOP : A_HSSTRT;                   // 5
    const uint16_t m_lace  = mutating("LACE-IS-BPLCON0-BIT-2-AND-RESET-CLEARS-IT")
                                  ? (uint16_t)0u : LACE;                   // 9
    const uint16_t m_pal   = mutating("LONG-LINE-TOGGLE-RUNS-UNLESS-PAL-OR-LOLDIS")
                                  ? (uint16_t)0u : PALBIT;                 // 10

    std::cout << "aa-programmable-sync-windows -- AA_CONFORMANCE at "
                 "agnus_beamcounter\n";
    if (g_mutate)
        std::cout << "MUTATE=" << g_mutate
                   << ": exactly the named assertion below must go red, "
                      "every other assertion must keep its baseline verdict "
                      "(LONG-LINES-ARE-ONE-CLOCK-LONGER excepted -- it is red "
                      "at baseline and its own mutation is inverted, see "
                      "below)\n";

    constexpr int LINE = 80;    // a short line, so a whole one fits in a run
    // Far from the module's own defaults, which put the sync edges at colour
    // clocks 19 and 35. An assertion that a programmed edge is not the default
    // edge should not rest on the two being one apart.
    constexpr int SYNC_ON = 50, SYNC_OFF = 62;
    constexpr int BLANK_ON = 8, BLANK_OFF = 60;

    // --- 1. The fixture runs a line the programmed registers decide ---------
    {
        Harness h;
        // MUTATION FIXTURE-RUNS-A-PROGRAMMED-LINE stimulus red: MUTATE=<id>
        // leaves VARBEAMEN clear, so the module keeps its own line length
        // and the wrap is not where HTOTAL put it. MEASURED (WSL/Verilator
        // 5.052, run alone against this id): FAIL FIXTURE-RUNS-A-
        // PROGRAMMED-LINE, FAIL LONG-LINES-ARE-ONE-CLOCK-LONGER (already
        // red at baseline, unaffected by this id), all eight other
        // assertions PASS, summary "8 passed, 2 failed" (baseline "9
        // passed, 1 failed"). Only the named assertion moved.
        h.write(A_BEAMCON0, m_varb_fixture);
        h.write(A_HTOTAL, LINE);
        h.to_line_start();
        const int top = h.line_top();
        c.note("with VARBEAMEN set and HTOTAL=" + dec(LINE) +
               ", the line runs to colour clock " + dec(top));
        c.expect(top == LINE,
                 "FIXTURE-RUNS-A-PROGRAMMED-LINE: the beam wraps at the colour "
                 "clock HTOTAL names, so every edge measured below is measured "
                 "against a line this fixture set the length of "
                 "[no page -- infrastructure]");
    }

    // --- 2. HTOTAL is the highest count, so the line is HTOTAL+1 long -------
    {
        Harness h;
        // MUTATION HTOTAL-IS-THE-HIGHEST-COUNT stimulus red: MUTATE=<id>
        // leaves VARBEAMEN clear, so the line is the module's own 227 and
        // not HTOTAL+1. MEASURED (WSL/Verilator 5.052, run alone against
        // this id): FAIL HTOTAL-IS-THE-HIGHEST-COUNT, FAIL LONG-LINES-ARE-
        // ONE-CLOCK-LONGER (already red at baseline, unaffected by this
        // id), all eight other assertions PASS, summary "8 passed, 2
        // failed". Only the named assertion moved.
        h.write(A_BEAMCON0, m_varb_htotal);
        h.write(A_HTOTAL, LINE);
        h.to_line_start();
        int clocks = 0;
        int prev = h.colour_clock();
        for (int i = 0; i < 2000; ++i) {
            h.cck_tick();
            ++clocks;
            const int now = h.colour_clock();
            if (now < prev) break;
            prev = now;
        }
        c.note("one whole line takes " + dec(clocks) + " colour clocks with "
               "HTOTAL=" + dec(LINE));
        c.expect(clocks == LINE + 1,
                 "HTOTAL-IS-THE-HIGHEST-COUNT: a line lasts HTOTAL+1 colour "
                 "clocks, which is what the entry means by calling HTOTAL the "
                 "highest count in a horizontal line rather than its length "
                 "[p.24] [claims: opus:AA-htotal.c1/c4/c16 sol:AA-htotal.c1/c5/c6]");
    }

    // --- 3. And it is not consulted until VARBEAMEN says so -----------------
    {
        Harness with, without;
        with.write(A_BEAMCON0, VARBEAMEN);
        with.write(A_HTOTAL, LINE);
        with.to_line_start();
        const int on = with.line_top();

        // MUTATION VARBEAMEN-GATES-THE-LIMIT-REGISTERS stimulus red:
        // MUTATE=<id> sets VARBEAMEN in the run that is supposed to be
        // without it, so both runs read HTOTAL and the gate distinguishes
        // nothing. MEASURED (WSL/Verilator 5.052, run alone against this
        // id): FAIL VARBEAMEN-GATES-THE-LIMIT-REGISTERS, FAIL LONG-LINES-
        // ARE-ONE-CLOCK-LONGER (already red at baseline, unaffected by
        // this id), all eight other assertions PASS, summary "8 passed, 2
        // failed". Only the named assertion moved.
        without.write(A_BEAMCON0,
                      mutating("VARBEAMEN-GATES-THE-LIMIT-REGISTERS")
                          ? VARBEAMEN : 0u);
        without.write(A_HTOTAL, LINE);
        without.to_line_start();
        const int off = without.line_top();

        c.note("the same HTOTAL write gives colour clock " + dec(on) +
               " with VARBEAMEN set and " + dec(off) + " with it clear");
        c.expect(on == LINE && off != LINE && off > 0,
                 "VARBEAMEN-GATES-THE-LIMIT-REGISTERS: the same write to "
                 "HTOTAL decides the line only when BEAMCON0 bit 7 is set, "
                 "which is the condition the AA entry prints against these "
                 "rows [p.24] [claims: opus:AA-htotal.c18 sol:AA-htotal.c9]");
    }

    // --- 4. The x cells above bit 7 are not part of the value ---------------
    {
        Harness h;
        h.write(A_BEAMCON0, m_varb_xcells);
        // MUTATION HTOTAL-IGNORES-THE-X-CELLS stimulus red: MUTATE=<id>
        // leaves the x cells clear, so the write is an ordinary one and says
        // nothing about the eight bits above the value. MEASURED (WSL/
        // Verilator 5.052, run alone against this id): FAIL HTOTAL-
        // IGNORES-THE-X-CELLS, FAIL LONG-LINES-ARE-ONE-CLOCK-LONGER
        // (already red at baseline, unaffected by this id), all eight
        // other assertions PASS, summary "8 passed, 2 failed". Only the
        // named assertion moved.
        h.write(A_HTOTAL, (uint16_t)(LINE | m_xcell));
        h.to_line_start();
        const int top = h.line_top();
        c.note("HTOTAL written as " + hex(LINE | 0xFF00) + " runs to colour "
               "clock " + dec(top));
        c.expect(top == LINE,
                 "HTOTAL-IGNORES-THE-X-CELLS: bits 15 to 8 of the written word "
                 "change nothing, so H8 to H1 are the eight bits the AA table "
                 "prints at 07 to 00 and the cells above them are the unused x "
                 "the same table marks [p.24] [claims: opus:AA-hsstop.c6 opus:AA-hsstrt.c6 sol:AA-hsstop.c1 sol:AA-hsstrt.c1 sol:AA-htotal.c10/c12/c14/c16/c18/c20/c22/c24/c26/c27/c28/c29/c30/c31/c33]");
    }

    // --- 5. HSSTRT and HSSTOP are the two edges of horizontal sync ----------
    {
        Harness h;
        h.write(A_BEAMCON0, VARBEAMEN | VARHSYEN);
        h.write(A_HTOTAL, LINE);
        // MUTATION HSSTRT-AND-HSSTOP-BOUND-HSYNC stimulus red: MUTATE=<id>
        // writes the start position into HSSTOP, so both edges come from
        // one register and neither lands where the entry says. MEASURED
        // (WSL/Verilator 5.052, run alone against this id): FAIL HSSTRT-
        // AND-HSSTOP-BOUND-HSYNC, FAIL LONG-LINES-ARE-ONE-CLOCK-LONGER
        // (already red at baseline, unaffected by this id), all eight
        // other assertions PASS, summary "8 passed, 2 failed". Only the
        // named assertion moved.
        h.write(m_hss, SYNC_ON);
        h.write(A_HSSTOP, SYNC_OFF);
        h.to_line_start();
        const int falls = h.edge_of(h.dut->_hsync, false, 200);
        const int rises = h.edge_of(h.dut->_hsync, true, 200);
        c.note("_hsync goes low at colour clock " + dec(falls) +
               " and high again at " + dec(rises));
        c.expect(falls == SYNC_ON && rises == SYNC_OFF,
                 "HSSTRT-AND-HSSTOP-BOUND-HSYNC: sync starts at the colour "
                 "clock HSSTRT names and stops at the one HSSTOP names, which "
                 "is the pair of numbers the two AA entries give -- and the "
                 "HSSTRT header's own text, which describes a stop position, "
                 "is the misprint its neighbour makes visible [p.24] "
                 "[claims: opus:AA-hsstop.c1/c4 opus:AA-hsstrt.c1 sol:AA-hsstop.c5 sol:AA-hsstrt.c6]");
    }

    // --- 6. The sync pair needs the second BEAMCON0 bit as well -------------
    {
        Harness h;
        // MUTATION HSSTRT-NEEDS-VARHSYEN-TOO stimulus red: MUTATE=<id> sets
        // VARHSYEN as well, so the sync pair IS read and the edge lands on
        // HSSTRT after all. MEASURED (WSL/Verilator 5.052, run alone
        // against this id): FAIL HSSTRT-NEEDS-VARHSYEN-TOO, FAIL LONG-
        // LINES-ARE-ONE-CLOCK-LONGER (already red at baseline, unaffected
        // by this id), all eight other assertions PASS, summary "8
        // passed, 2 failed". Only the named assertion moved.
        h.write(A_BEAMCON0, m_gate);      // VARBEAMEN only, unless mutating
        h.write(A_HTOTAL, LINE);
        h.write(A_HSSTRT, SYNC_ON);
        h.write(A_HSSTOP, SYNC_OFF);
        h.to_line_start();
        const int falls = h.edge_of(h.dut->_hsync, false, 200);
        c.note("with VARBEAMEN alone, _hsync goes low at colour clock " +
               dec(falls) + " rather than at HSSTRT=" + dec(SYNC_ON));
        c.expect(falls != SYNC_ON,
                 "HSSTRT-NEEDS-VARHSYEN-TOO: VARBEAMEN alone leaves the sync "
                 "pair unread, so BEAMCON0 governs when HSSTRT and HSSTOP are "
                 "active in a stronger sense than the entries' cross-reference "
                 "states -- two bits, not one [p.24] "
                 "[claims: opus:AA-hsstrt.c7]");
    }

    // --- 7. HBSTRT and HBSTOP are the two edges of blanking -----------------
    {
        Harness h;
        h.write(A_BEAMCON0, m_varb_blanking);
        h.write(A_HTOTAL, LINE);
        // MUTATION HBSTRT-AND-HBSTOP-BOUND-BLANKING stimulus red:
        // MUTATE=<id> leaves VARBEAMEN clear through m_varb_blanking above,
        // so the module keeps its own blanking window. MEASURED (WSL/
        // Verilator 5.052, run alone against this id): FAIL HBSTRT-AND-
        // HBSTOP-BOUND-BLANKING, FAIL LONG-LINES-ARE-ONE-CLOCK-LONGER
        // (already red at baseline, unaffected by this id), all eight
        // other assertions PASS, summary "8 passed, 2 failed". Only the
        // named assertion moved.
        h.write(A_HBSTRT, BLANK_ON);
        h.write(A_HBSTOP, BLANK_OFF);
        h.to_line_start();
        const int on = h.edge_of(h.dut->hblank, true, 200);
        const int off = h.edge_of(h.dut->hblank, false, 200);
        c.note("hblank rises at colour clock " + dec(on) + " and falls at " +
               dec(off));
        c.expect(on == BLANK_ON && off == BLANK_OFF,
                 "HBSTRT-AND-HBSTOP-BOUND-BLANKING: blanking starts at the "
                 "colour clock HBSTRT names and stops at the one HBSTOP names, "
                 "in the 280 ns increments the page's bits 7 to 0 carry "
                 "[p.24] [claims: opus:AA-hbstop.c1/c3 opus:AA-hbstrt.c2/c4/c7/c9/c10/c11/c13/c14/c15/c16/c17/c18/c19/c20 sol:AA-hbstop.c1 sol:AA-hbstrt.c1 sol:AA-horizontal-blanking.c1/c2/c3/c5/c9/c10/c11/c12/c13/c14/c15/c16]");
    }

    // --- 8. The two pairs are not gated alike, which no claim says -------
    //
    // NOT AN ASSERTION. The module reads the blanking pair with VARBEAMEN
    // alone and the sync pair only with VARHSYEN as well, and that contrast
    // is worth recording -- but the AA entries for HBSTRT and HBSTOP say
    // nothing about when blanking is active, where the HSSTRT and HSSTOP
    // entries do say BEAMCON0 governs theirs. An expect() here would grade
    // the module against a sentence the page does not contain.
    {
        Harness h;
        h.write(A_BEAMCON0, VARBEAMEN);
        h.write(A_HTOTAL, LINE);
        h.write(A_HBSTRT, BLANK_ON);
        h.write(A_HBSTOP, BLANK_OFF);
        h.to_line_start();
        const int on = h.edge_of(h.dut->hblank, true, 200);
        c.note("with VARBEAMEN alone and no VARHSYEN, hblank still rises at "
               "the programmed colour clock " + dec(on) + ", where the sync "
               "pair measured above does not move until VARHSYEN is set too. "
               "The page states the condition for the sync pair and not for "
               "this one");
    }

    // --- 9. LACE is bit 2 of BPLCON0 and clear after reset ------------------
    {
        Harness h;
        const bool after_reset = h.dut->lace != 0;
        // MUTATION LACE-IS-BPLCON0-BIT-2-AND-RESET-CLEARS-IT stimulus red:
        // MUTATE=<id> writes BPLCON0 with bit 2 clear, so the output never
        // rises and the write proves nothing. MEASURED (WSL/Verilator
        // 5.052, run alone against this id): FAIL LACE-IS-BPLCON0-BIT-2-
        // AND-RESET-CLEARS-IT, FAIL LONG-LINES-ARE-ONE-CLOCK-LONGER
        // (already red at baseline, unaffected by this id), all eight
        // other assertions PASS, summary "8 passed, 2 failed". Only the
        // named assertion moved.
        h.write(A_BPLCON0, m_lace);
        const bool after_write = h.dut->lace != 0;
        c.note(std::string("lace reads ") + (after_reset ? "1" : "0") +
               " after reset and " + (after_write ? "1" : "0") +
               " after BPLCON0 bit 2 is written");
        c.expect(!after_reset && after_write,
                 "LACE-IS-BPLCON0-BIT-2-AND-RESET-CLEARS-IT: the interlace "
                 "enable reaches this module from bit 2 of BPLCON0 and powers "
                 "up clear, both halves of what the AA entry says about it "
                 "[p.17] [claims: opus:AA-lace.c1]");
    }

    // --- 10. When the long-line/short-line toggle runs at all --------------
    //
    // The entry's own sentence: "If the PAL bit & LOLDIS are not high, long
    // line/short line toggle will occur, and there will be this many + 2 every
    // other line." Two claims in one line, and the module answers them
    // differently -- which is why they are two assertions.
    {
        Harness ntsc(true), pal(false);
        ntsc.write(A_BEAMCON0, VARBEAMEN);           // PAL bit low, LOLDIS low
        // MUTATION LONG-LINE-TOGGLE-RUNS-UNLESS-PAL-OR-LOLDIS stimulus red:
        // MUTATE=<id> leaves the PAL bit clear in the second run as well, so
        // both runs toggle and the condition distinguishes nothing.
        // MEASURED (WSL/Verilator 5.052, run alone against this id): FAIL
        // LONG-LINE-TOGGLE-RUNS-UNLESS-PAL-OR-LOLDIS, FAIL LONG-LINES-ARE-
        // ONE-CLOCK-LONGER (already red at baseline, unaffected by this
        // id), all eight other assertions PASS, summary "8 passed, 2
        // failed". Only the named assertion moved.
        pal.write(A_BEAMCON0, (uint16_t)(VARBEAMEN | m_pal));
        int toggles_ntsc = 0, toggles_pal = 0;
        bool was_n = (ntsc.read(A_VPOSR) & LONG_LINE) != 0;
        bool was_p = (pal.read(A_VPOSR) & LONG_LINE) != 0;
        for (int line = 0; line < 4; ++line) {
            ntsc.line_length();
            pal.line_length();
            const bool now_n = (ntsc.read(A_VPOSR) & LONG_LINE) != 0;
            const bool now_p = (pal.read(A_VPOSR) & LONG_LINE) != 0;
            if (now_n != was_n) ++toggles_ntsc;
            if (now_p != was_p) ++toggles_pal;
            was_n = now_n;
            was_p = now_p;
        }
        c.note("over four lines the long-line bit changes " +
               dec(toggles_ntsc) + " times with the PAL bit low and " +
               dec(toggles_pal) + " times with it high");
        c.expect(toggles_ntsc > 0 && toggles_pal == 0,
                 "LONG-LINE-TOGGLE-RUNS-UNLESS-PAL-OR-LOLDIS: the toggle runs "
                 "when the PAL bit and LOLDIS are not high and stops when one "
                 "of them is, which is the condition the HTOTAL entry states "
                 "[p.24] [claims: opus:AA-htotal.c17 sol:AA-htotal.c7]");
    }

    // --- 11. And what the toggle is supposed to do to the line -------------
    {
        Harness h(true);                 // PAL bit low, so the toggle runs
        h.write(A_BEAMCON0, VARBEAMEN);
        h.write(A_HTOTAL, LINE);
        h.to_line_start();
        const int first = h.line_length();
        const int second = h.line_length();
        // MUTATION LONG-LINES-ARE-ONE-CLOCK-LONGER oracle green: this
        // assertion is RED against the tree at baseline (a genuine
        // published finding), so its mutation is inverted, the same
        // convention this suite already uses for a baseline red (see
        // ch3-beam-counters-and-blanking's VBLANK-DURATION-PAL-DISAGREES-
        // WITH-MANUAL and this repository's acceptance record
        // hrm-testpipeline/acceptance/aa-s1.json, "mutation_gate": "...
        // aa-programmable-sync-windows and aa-disk-status-bits carry red
        // baselines, whose mutations are inverted and go green"): selecting
        // this assertion's own id asks the module for the length it
        // actually produces instead of the one the entry promises, and the
        // assertion goes green. A mutation that made a red redder would
        // prove nothing about the measurement. MEASURED (WSL/Verilator
        // 5.052, run alone against this id): PASS LONG-LINES-ARE-ONE-CLOCK-
        // LONGER (the only assertion whose verdict FLIPS -- from red to
        // green -- since it starts red at baseline), all nine other
        // assertions keep their baseline PASS, summary "10 passed, 0
        // failed" (baseline "9 passed, 1 failed"). Only the named assertion
        // moved.
        const int want = mutating("LONG-LINES-ARE-ONE-CLOCK-LONGER")
                              ? second : first + 1;
        c.note("with the toggle running, two consecutive lines take " +
               dec(first) + " and " + dec(second) + " colour clocks; the "
               "entry says every other line is HTOTAL+2, which is " +
               dec(first + 1));
        c.expect(second == want,
                 "LONG-LINES-ARE-ONE-CLOCK-LONGER: under long-line/short-line "
                 "toggling every other line lasts HTOTAL+2 increments. This "
                 "module toggles the long-line bit VPOSR reports but ends "
                 "every line at the same count, so the two lines measure the "
                 "same [p.24] [claims: sol:AA-htotal.c8]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary (absent): The claim is about the "
             "secondary beam counter HHPOS. This module has one horizontal "
             "counter and decodes neither $1D8 nor $1DA; the UHRES pointers the "
             "counter is said to trigger are absent from this design. [claims: "
             "opus:AA-hhposr.c1/c2 opus:AA-hhposw.c1/c2/c5/c6 "
             "sol:AA-hhpos.c1/c2/c3/c4/c5/c6/c7 sol:AA-hhposr.c1/c2 "
             "sol:AA-hhposw.c1/c2]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is about ERSY "
             "making the HSYNC and VSYNC pads inputs. This module has no sync "
             "input pads; ERSY reaches it as a BPLCON0 bit that holds the "
             "counter at zero. [claims: opus:AA-ersy.c1]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is about a "
             "position on the long field of an interlaced frame. This package "
             "runs lines; the field alternation and the vsync position need "
             "whole frames and a vertical fixture. [claims: "
             "opus:AA-hcenter.c1/c2/c4/c6 sol:AA-hcenter.c1/c2/c5/c6]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is about the "
             "35 ns fine-position bits, HBSTRT and HBSTOP bits 10 to 8. hpos "
             "advances in 140 ns steps, so no reading at this boundary separates "
             "an implemented fine bit from a dropped one. The module drops them "
             "at the write and carries its own TODO there. [claims: "
             "opus:AA-hbstrt.c8/c12 sol:AA-horizontal-blanking.c4/c6/c7/c8 "
             "sol:AA-horizontal-comparators.c2/c3/c4/c5/c6]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is that the "
             "register is write-only. data_out answers VPOSR and VHPOSR and "
             "nothing else, so a read of a limit register's address returns the "
             "same silence as a read of an address the document never mentions, "
             "and the two are not distinguishable here. [claims: "
             "opus:AA-hbstop.c2 opus:AA-hbstrt.c3 opus:AA-hsstop.c2 "
             "opus:AA-hsstrt.c2 opus:AA-htotal.c2 sol:AA-hbstop.c2 "
             "sol:AA-hbstrt.c2 sol:AA-hsstop.c2 sol:AA-hsstrt.c2 "
             "sol:AA-htotal.c2]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim names the chip "
             "that implements the register. No port of agnus_beamcounter carries "
             "a chip identity. [claims: opus:AA-hcenter.c3/c7 opus:AA-hhposr.c3 "
             "opus:AA-hhposw.c3 opus:AA-hsstop.c3 opus:AA-hsstrt.c3 "
             "opus:AA-htotal.c3 sol:AA-hcenter.c3 sol:AA-hhposr.c3 "
             "sol:AA-hhposw.c3 sol:AA-hsstop.c3 sol:AA-hsstrt.c3 "
             "sol:AA-htotal.c3]");
    c.record(
             "DECLINED at this boundary (not-hardware): The claim is about the "
             "document -- a misprint, a conflict between two of its statements, "
             "a legend mark, or a recommendation to software about how to write "
             "an unused cell. [claims: opus:AA-hbstop.c4/c5 "
             "opus:AA-hbstrt.c1/c5/c6 opus:AA-hcenter.c5 opus:AA-hhposr.c4 "
             "opus:AA-hhposw.c4 opus:AA-hsstop.c5 opus:AA-hsstrt.c4/c5 "
             "opus:AA-htotal.c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15 "
             "sol:AA-hbstop.c3/c4 sol:AA-hbstrt.c3/c4 sol:AA-hcenter.c4 "
             "sol:AA-hhposr.c4 sol:AA-hhposw.c4 sol:AA-hsstop.c4 "
             "sol:AA-hsstrt.c4/c5 "
             "sol:AA-htotal.c4/c11/c13/c15/c17/c19/c21/c23/c25/c32]");
    c.record(
             "DECLINED at this boundary (outside-uut): The register named is "
             "decoded in denise or one of its sub-modules, not in the beam "
             "counter, and is graded there by its own AA package. [claims: "
             "sol:AA-horizontal-comparators.c1]");

    std::cout << "\nAssertions: " << c.passed << " passed, " << c.failed
              << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
