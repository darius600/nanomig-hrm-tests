// ch3-shres-bitplane-colour-encoding -- denise conformance test
//
// Test purpose: the ECS SuperHires bitplane colour encoding printed on
//   p.301 -- the table that maps each of the sixteen lower colour
//   registers' bits onto symbols a-x, and the legend "Bitplane (Color 0):
//   ab-- cd-- ef--" (and its siblings for colours 1-3) that names which
//   symbol is which colour component -- plus p.297's "new color register
//   interpretations" and p.300's two-bitplane SuperHires depth cap.
//   Package ch3-shres-bitplane-colour-encoding, fixture denise (one
//   Verilator build, top module denise, per PACKAGE.md). A sibling
//   package covers the sprite half of the same scheme on page 302.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 297, 300, 301, 302 (claims.jsonl for
//   ch3-shres-bitplane-colour-encoding, 301 records).
// Non-goals: AGA's separate SuperHires colour path (aga=1) is out of scope
//   -- every claim in this package's inventory quotes the ECS text (p.297,
//   300, 301, 302), and aga is held at 0 throughout. ham and dpf are
//   non-goal defaults (0): no claim in this inventory combines SuperHires
//   with hold-and-modify or dual-playfield.
// Claim ids: ids such as `C-shres-bpl-bits.c1` and `C-shres-bpl-bits.c2`
//   name one sentence of the manual in this chapter's reading. They carry
//   no reader prefix, and they resolve in the claim files that
//   hrm-testpipeline/acceptance/ch3.json lists under `inventories`.
//
// WHAT THIS PACKAGE MEASURES. p.301's key gives each bitplane colour (0-3)
// six letters -- two bits of red, two of green, two of blue -- and its
// table places those letters into the sixteen lower colour registers: row
// n carries colour (n mod 4)'s own letters in bits 11-10, 7-6, 3-2 and
// colour (n div 4)'s own letters in bits 9-8, 5-4, 1-0. The page does not
// say which register, or which half of it, a given 35 ns SuperHires pixel
// reads -- p.297 only promises "new color register interpretations". This
// package fixes ONE concrete, documented addressing convention to turn
// that promise into a runnable measurement (ADDRESSING CONVENTION below);
// the convention is the package's own stimulus choice, not a page claim,
// and no assertion below treats it as one.
//
// ADDRESSING CONVENTION (this package's own choice, used to build stimuli
// and to classify which register a given run can legitimately move -- not
// itself graded): a SuperHires pixel pair is two consecutive 35 ns ticks;
// the first tick of a pair shows the register's own bits 11-10/7-6/3-2,
// the second tick shows bits 9-8/5-4/1-0; the pair's register index is
// 4*(first tick's raw two-bitplane value) + (second tick's raw value).
// Under this convention: a SOLID run of colour v (both bitplanes constant)
// addresses register 5*v on every pair. Two MIXED runs are also driven,
// each loading a 16-bit shift pattern into the two live bitplanes and
// sampling every 35 ns tick as it shifts out: pat1 = BPL1DAT/BPL2DAT =
// 0xAAAA/0xCCCC (adjacent pairs cycle registers 4 and 14) and pat2 =
// 0xF0F0/0xFF00 (adjacent pairs cycle registers 0, 5, 10 and 15). Six runs
// total: SOLID-0..SOLID-3, MIXED-1, MIXED-2.
//
// A register's own bit is graded by running the six-run battery once with
// a fixed base palette (four colours, pairwise-distinct two-bit R/G/B
// fields) written into all sixteen registers via the row formula above,
// and once more with that ONE bit of that ONE register inverted: FAIL if
// any run this convention does NOT tie to that register changes (an
// untied run/component moving is the RTL disagreeing with the table, or
// with this package's own addressing choice, either way a red result);
// PASS if a tied run changes and no untied run does; otherwise the
// register/bit is recorded, not graded, as not observable -- this
// package's own six-run battery never reaches it. bpu is held at 2
// throughout every SuperHires stimulus (p.300: "The maximum plane depth
// for a SuperHires viewport is 2 bitplanes").
//
// Register bus addressing: BPLCON0 = $100, COLORxx = $180 + xx,
// BPL1DAT/BPL2DAT/BPL3DAT/BPL4DAT = $110/$112/$114/$116, DIWSTRT/DIWSTOP =
// $08e/$090 (the manual's own register map, Appendix C); this fixture's
// own register-address input is the byte address halved. BPLCON0's own
// field layout (hires, bpu, shres, ecsena) is this fixture's wiring, not a
// claim tested here.
//
// CLAIM COVERAGE: all 301 records of claims.jsonl are accounted for below
// -- 298 inside an expect()/record() assertion string (ids written out in
// full), 3 declined (RECORDED, NOT GRADED block near the end of main()).
// Ids restating the identical proposition, or the same table row from a
// different angle, are grouped and cited together on the single assertion
// (or loop) that tests that proposition:
//
//   CONTROL-KNOWN-VALUES-ARRIVE (0 ids, no page)
//   FIXTURE-DATA-PATH-LIVE (0 ids, no page)
//   SELECT-REGISTER-ADDRESSES-CORRECTLY (0 ids, no page)
//   OCS-BASELINE-0RGB-FORMAT (4 ids)               -- C-shres-color.c4-c7
//   SHRES-REINTERPRETS-SAME-REGISTERS (2 ids)      -- C-shr.c06,
//                                                      C-superhires-color.c1
//   SHRES-BITPLANE-TABLE-COLOR-00 .. -0F (16 assertions, 288 ids total):
//     each cites its own row id (C-shrc.rXX) plus its own twelve
//     significant-bit C-shres-bpl-bits.cNNN ids; COLOR-00, -05, -0A and
//     -0F (the four diagonal rows) additionally cite that source colour's
//     own legend row and its three symbol ids.
//   SHRES-USES-SIXTEEN-LOWER-REGISTERS (2 ids)     -- C-shrc.c04, c05
//   SHRES-TWO-BITS-RESOLUTION (2 ids)              -- C-shrc.c01, c02
//   Tally: 4 + 2 + 288 + 2 + 2 = 298 distinct ids. Declined: 3. 298+3=301.
//
// RESULTS: 5 passed, 7 failed (12 graded assertions; 15 more recorded,
// not graded -- 12 declined-by-construction COLOR-04..-0F entries plus 3
// declined claims). Passing: CONTROL-KNOWN-VALUES-ARRIVE, FIXTURE-DATA-
// PATH-LIVE, OCS-BASELINE-0RGB-FORMAT, SELECT-REGISTER-ADDRESSES-
// CORRECTLY, SHRES-USES-SIXTEEN-LOWER-REGISTERS's own Hires control.
// Failing: SHRES-REINTERPRETS-SAME-REGISTERS, SHRES-BITPLANE-TABLE-
// COLOR-00 through -03, SHRES-USES-SIXTEEN-LOWER-REGISTERS, SHRES-TWO-
// BITS-RESOLUTION. Every assertion's own label states what it measured.
//
// MUTATION LOG (every changed or new assertion, actually built with its
// own -DMUT_*=1 flag via EXTRA_CFLAGS, rerun, and reverted):
//   MUT_REINTERP: forces h1's own register content to differ from h0's
//     unrelated to shres -- baseline FAIL (0x4499EE both) -> PASS
//     (0x4499EE vs 0xBB6611). Confirms the equality check is not
//     vacuously false.
//   MUT_COLORTABLE: misclassifies every run as tied for register 1 only
//     -- COLOR-01 baseline FAIL (12 of 12 bits untied) -> PASS (12 of 12
//     tied). Confirms the untied/tied classification drives the verdict.
//   MUT_SIXTEEN: probes COLOR01 (directly reachable) instead of COLOR0A
//     -- the main assertion FAIL (no run changed) -> PASS (a run
//     changed), and its own Hires control PASS (no run changed) -> FAIL
//     (a run changed) in lockstep, both for the same reason (COLOR01 is
//     reachable, unlike COLOR0A, regardless of mode).
//   MUT_TWOBITS: substitutes colour 1 (the tested run's own colour) for
//     the "other colours" loop -- 6 of 6 tested variants differ (versus
//     6 of 18 at baseline), confirming the per-variant tick comparison
//     is not vacuous; the aggregate verdict stays FAIL either way, for a
//     different, genuine reason (colour 0's own presence in COLOR01's
//     low half at baseline).
//   MUT_CONTROL, MUT_FIXTURE, MUT_OCSBASELINE, MUT_SELECTREG (pre-
//     existing controls, unchanged claims, re-verified after this
//     package's harness timing was retuned): each flips its own control
//     PASS to FAIL as before.

#include <algorithm>
#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vdenise.h"

#ifndef MUT_CONTROL
#define MUT_CONTROL 0
#endif
#ifndef MUT_FIXTURE
#define MUT_FIXTURE 0
#endif
#ifndef MUT_OCSBASELINE
#define MUT_OCSBASELINE 0
#endif
#ifndef MUT_SELECTREG
#define MUT_SELECTREG 0
#endif
#ifndef MUT_REINTERP
#define MUT_REINTERP 0
#endif
#ifndef MUT_COLORTABLE
#define MUT_COLORTABLE 0
#endif
#ifndef MUT_SIXTEEN
#define MUT_SIXTEEN 0
#endif
#ifndef MUT_TWOBITS
#define MUT_TWOBITS 0
#endif

namespace {

// ---------------------------------------------------------------------
// Oracle: the ECS SuperHires bitplane colour encoding table, p.301, and
// its legend. Derived mechanically from claims.jsonl's own restatement
// text (256 C-shres-bpl-bits.cNNN records), independently cross-checked
// against the sixteen C-shrc.rXX row quotations.
// ---------------------------------------------------------------------

static const char* const kRowIds[16] = {
    "C-shrc.r00", "C-shrc.r01", "C-shrc.r02", "C-shrc.r03",
    "C-shrc.r04", "C-shrc.r05", "C-shrc.r06", "C-shrc.r07",
    "C-shrc.r08", "C-shrc.r09", "C-shrc.r0A", "C-shrc.r0B",
    "C-shrc.r0C", "C-shrc.r0D", "C-shrc.r0E", "C-shrc.r0F",
};

// Per-register bit-claim id lists, bit 11 down to bit 0 (the twelve
// significant bits; bits 15-12 are printed unused and carry no id).
static const char* const kBitIds[16][12] = {
    { "C-shres-bpl-bits.c5", "C-shres-bpl-bits.c6", "C-shres-bpl-bits.c7", "C-shres-bpl-bits.c8", "C-shres-bpl-bits.c9", "C-shres-bpl-bits.c10", "C-shres-bpl-bits.c11", "C-shres-bpl-bits.c12", "C-shres-bpl-bits.c13", "C-shres-bpl-bits.c14", "C-shres-bpl-bits.c15", "C-shres-bpl-bits.c16" }, // COLOR00
    { "C-shres-bpl-bits.c21", "C-shres-bpl-bits.c22", "C-shres-bpl-bits.c23", "C-shres-bpl-bits.c24", "C-shres-bpl-bits.c25", "C-shres-bpl-bits.c26", "C-shres-bpl-bits.c27", "C-shres-bpl-bits.c28", "C-shres-bpl-bits.c29", "C-shres-bpl-bits.c30", "C-shres-bpl-bits.c31", "C-shres-bpl-bits.c32" }, // COLOR01
    { "C-shres-bpl-bits.c37", "C-shres-bpl-bits.c38", "C-shres-bpl-bits.c39", "C-shres-bpl-bits.c40", "C-shres-bpl-bits.c41", "C-shres-bpl-bits.c42", "C-shres-bpl-bits.c43", "C-shres-bpl-bits.c44", "C-shres-bpl-bits.c45", "C-shres-bpl-bits.c46", "C-shres-bpl-bits.c47", "C-shres-bpl-bits.c48" }, // COLOR02
    { "C-shres-bpl-bits.c53", "C-shres-bpl-bits.c54", "C-shres-bpl-bits.c55", "C-shres-bpl-bits.c56", "C-shres-bpl-bits.c57", "C-shres-bpl-bits.c58", "C-shres-bpl-bits.c59", "C-shres-bpl-bits.c60", "C-shres-bpl-bits.c61", "C-shres-bpl-bits.c62", "C-shres-bpl-bits.c63", "C-shres-bpl-bits.c64" }, // COLOR03
    { "C-shres-bpl-bits.c133", "C-shres-bpl-bits.c134", "C-shres-bpl-bits.c135", "C-shres-bpl-bits.c136", "C-shres-bpl-bits.c137", "C-shres-bpl-bits.c138", "C-shres-bpl-bits.c139", "C-shres-bpl-bits.c140", "C-shres-bpl-bits.c141", "C-shres-bpl-bits.c142", "C-shres-bpl-bits.c143", "C-shres-bpl-bits.c144" }, // COLOR04
    { "C-shres-bpl-bits.c149", "C-shres-bpl-bits.c150", "C-shres-bpl-bits.c151", "C-shres-bpl-bits.c152", "C-shres-bpl-bits.c153", "C-shres-bpl-bits.c154", "C-shres-bpl-bits.c155", "C-shres-bpl-bits.c156", "C-shres-bpl-bits.c157", "C-shres-bpl-bits.c158", "C-shres-bpl-bits.c159", "C-shres-bpl-bits.c160" }, // COLOR05
    { "C-shres-bpl-bits.c165", "C-shres-bpl-bits.c166", "C-shres-bpl-bits.c167", "C-shres-bpl-bits.c168", "C-shres-bpl-bits.c169", "C-shres-bpl-bits.c170", "C-shres-bpl-bits.c171", "C-shres-bpl-bits.c172", "C-shres-bpl-bits.c173", "C-shres-bpl-bits.c174", "C-shres-bpl-bits.c175", "C-shres-bpl-bits.c176" }, // COLOR06
    { "C-shres-bpl-bits.c181", "C-shres-bpl-bits.c182", "C-shres-bpl-bits.c183", "C-shres-bpl-bits.c184", "C-shres-bpl-bits.c185", "C-shres-bpl-bits.c186", "C-shres-bpl-bits.c187", "C-shres-bpl-bits.c188", "C-shres-bpl-bits.c189", "C-shres-bpl-bits.c190", "C-shres-bpl-bits.c191", "C-shres-bpl-bits.c192" }, // COLOR07
    { "C-shres-bpl-bits.c69", "C-shres-bpl-bits.c70", "C-shres-bpl-bits.c71", "C-shres-bpl-bits.c72", "C-shres-bpl-bits.c73", "C-shres-bpl-bits.c74", "C-shres-bpl-bits.c75", "C-shres-bpl-bits.c76", "C-shres-bpl-bits.c77", "C-shres-bpl-bits.c78", "C-shres-bpl-bits.c79", "C-shres-bpl-bits.c80" }, // COLOR08
    { "C-shres-bpl-bits.c197", "C-shres-bpl-bits.c198", "C-shres-bpl-bits.c199", "C-shres-bpl-bits.c200", "C-shres-bpl-bits.c201", "C-shres-bpl-bits.c202", "C-shres-bpl-bits.c203", "C-shres-bpl-bits.c204", "C-shres-bpl-bits.c205", "C-shres-bpl-bits.c206", "C-shres-bpl-bits.c207", "C-shres-bpl-bits.c208" }, // COLOR09
    { "C-shres-bpl-bits.c85", "C-shres-bpl-bits.c86", "C-shres-bpl-bits.c87", "C-shres-bpl-bits.c88", "C-shres-bpl-bits.c89", "C-shres-bpl-bits.c90", "C-shres-bpl-bits.c91", "C-shres-bpl-bits.c92", "C-shres-bpl-bits.c93", "C-shres-bpl-bits.c94", "C-shres-bpl-bits.c95", "C-shres-bpl-bits.c96" }, // COLOR0A
    { "C-shres-bpl-bits.c101", "C-shres-bpl-bits.c102", "C-shres-bpl-bits.c103", "C-shres-bpl-bits.c104", "C-shres-bpl-bits.c105", "C-shres-bpl-bits.c106", "C-shres-bpl-bits.c107", "C-shres-bpl-bits.c108", "C-shres-bpl-bits.c109", "C-shres-bpl-bits.c110", "C-shres-bpl-bits.c111", "C-shres-bpl-bits.c112" }, // COLOR0B
    { "C-shres-bpl-bits.c213", "C-shres-bpl-bits.c214", "C-shres-bpl-bits.c215", "C-shres-bpl-bits.c216", "C-shres-bpl-bits.c217", "C-shres-bpl-bits.c218", "C-shres-bpl-bits.c219", "C-shres-bpl-bits.c220", "C-shres-bpl-bits.c221", "C-shres-bpl-bits.c222", "C-shres-bpl-bits.c223", "C-shres-bpl-bits.c224" }, // COLOR0C
    { "C-shres-bpl-bits.c229", "C-shres-bpl-bits.c230", "C-shres-bpl-bits.c231", "C-shres-bpl-bits.c232", "C-shres-bpl-bits.c233", "C-shres-bpl-bits.c234", "C-shres-bpl-bits.c235", "C-shres-bpl-bits.c236", "C-shres-bpl-bits.c237", "C-shres-bpl-bits.c238", "C-shres-bpl-bits.c239", "C-shres-bpl-bits.c240" }, // COLOR0D
    { "C-shres-bpl-bits.c117", "C-shres-bpl-bits.c118", "C-shres-bpl-bits.c119", "C-shres-bpl-bits.c120", "C-shres-bpl-bits.c121", "C-shres-bpl-bits.c122", "C-shres-bpl-bits.c123", "C-shres-bpl-bits.c124", "C-shres-bpl-bits.c125", "C-shres-bpl-bits.c126", "C-shres-bpl-bits.c127", "C-shres-bpl-bits.c128" }, // COLOR0E
    { "C-shres-bpl-bits.c245", "C-shres-bpl-bits.c246", "C-shres-bpl-bits.c247", "C-shres-bpl-bits.c248", "C-shres-bpl-bits.c249", "C-shres-bpl-bits.c250", "C-shres-bpl-bits.c251", "C-shres-bpl-bits.c252", "C-shres-bpl-bits.c253", "C-shres-bpl-bits.c254", "C-shres-bpl-bits.c255", "C-shres-bpl-bits.c256" }, // COLOR0F
};

// The four diagonal (single-source-colour) registers additionally cite
// their source colour's own legend row (p.301, "Bitplane (Color N) :
// ..") and its three symbol ids.
struct LegendCite { int reg; const char* srcb; const char* sym[3]; };
static const LegendCite kLegend[4] = {
    { 0x0, "C-shrc.srcb0", { "C-shres-symbols.c1", "C-shres-symbols.c2", "C-shres-symbols.c3" } },
    { 0x5, "C-shrc.srcb1", { "C-shres-symbols.c4", "C-shres-symbols.c5", "C-shres-symbols.c6" } },
    { 0xA, "C-shrc.srcb2", { "C-shres-symbols.c7", "C-shres-symbols.c8", "C-shres-symbols.c9" } },
    { 0xF, "C-shrc.srcb3", { "C-shres-symbols.c10", "C-shres-symbols.c11", "C-shres-symbols.c12" } },
};

// ---------------------------------------------------------------------
// Register bus addressing -- the manual's own Appendix C register map.
// ---------------------------------------------------------------------
constexpr uint8_t kAddrBplcon0 = 0x80; // $100
constexpr uint8_t kAddrColor0 = 0xC0;  // $180, COLORxx = kAddrColor0 + xx
constexpr uint8_t kAddrBpl1Dat = 0x88; // $110
constexpr uint8_t kAddrBpl2Dat = 0x89; // $112
constexpr uint8_t kAddrBpl3Dat = 0x8A; // $114
constexpr uint8_t kAddrBpl4Dat = 0x8B; // $116
constexpr uint8_t kAddrDiwstrt = 0x47; // $08e
constexpr uint8_t kAddrDiwstop = 0x48; // $090

constexpr uint16_t kBplcon0Hires = 0x8000;  // bit15
constexpr uint16_t kBplcon0Shres = 0x0040;  // bit6
constexpr uint16_t kBplcon0Ecsena = 0x0001; // bit0

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

std::string hex6(uint32_t v) {
    std::ostringstream os;
    os << "0x" << std::hex << std::uppercase << std::setw(6) << std::setfill('0')
       << (v & 0xFFFFFFu);
    return os.str();
}

class Harness {
public:
    Harness() : dut(new Vdenise) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->c1 = 0;
        dut->c3 = 0;
        dut->reset = 1;
        dut->strhor = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->blank = 0;
        dut->a1k = 0;
        dut->ecs = 1;
        dut->aga = 0;
        for (int i = 0; i < 16; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; ++i) tick();

        // Opens denise's own display window once, wide enough that hpos
        // (free-running, advancing one per clk7_en tick) stays inside it
        // for this Harness's entire, short lifetime -- otherwise every
        // pixel below reads border colour regardless of what is written.
        write_reg(kAddrDiwstrt, 0x0004);
        write_reg(kAddrDiwstop, 0x00F0);
        while (phase != 0) tick();
        dut->strhor = 1;
        tick();
        dut->strhor = 0;
        tick(32);
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    // One 28MHz edge, then advance the four-phase c1/c3/clk7_en pattern
    // this fixture's own bitplane-load timing needs for the next edge.
    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        phase = (phase + 1) % 4;
        dut->clk7_en = (phase == 0);
        dut->c1 = (phase == 0 || phase == 1);
        dut->c3 = (phase == 1 || phase == 2);
    }
    void tick(int n) { for (int i = 0; i < n; ++i) tick(); }

    // Ticks n times, sampling the RGB output every 35 ns tick (a
    // SuperHires pixel), returning one sample per tick.
    std::vector<uint32_t> sample(int n) {
        std::vector<uint32_t> out;
        out.reserve(static_cast<size_t>(n));
        for (int i = 0; i < n; ++i) { tick(); out.push_back(rgb()); }
        return out;
    }

    void begin_write_reg(uint16_t word_addr, uint16_t data) {
        while (phase != 0) tick();
        dut->reg_address_in = word_addr;
        dut->data_in = data;
        tick();
    }
    void clear_reg() {
        dut->reg_address_in = 0;
        dut->data_in = 0;
    }
    void write_reg(uint16_t word_addr, uint16_t data) {
        begin_write_reg(word_addr, data);
        clear_reg();
        tick(4); // settle margin
    }

    void write_color(int idx, uint16_t val12) {
        write_reg(static_cast<uint16_t>(kAddrColor0 + idx), val12 & 0x0FFF);
    }

    void set_mode(bool shres_on, int bpu, bool hires_on = false) {
        write_reg(kAddrBplcon0,
                   static_cast<uint16_t>((hires_on ? kBplcon0Hires : 0) |
                                          (shres_on ? kBplcon0Shres : 0) |
                                          ((bpu & 7) << 12) | kBplcon0Ecsena));
        tick(32);
    }

    // Refreshes the two live bitplanes (bpu=2) to a constant two-bit
    // value v (0-3), reloading both shifters from a fresh write, then
    // settles a few extra ticks so the multi-stage output pipeline this
    // fixture's own colour path uses has caught up to the new load before
    // any caller samples it.
    // settleTicks defaults to a budget safe for SHRES's own full-rate
    // shift (every tick); a caller driving this under a slower shift
    // rate (plain Hires or Lores, shres=0) passes a longer settle.
    void drive_value2(int v, int settleTicks = 16) {
        write_reg(kAddrBpl2Dat, (v & 2) ? 0xFFFF : 0x0000);
        write_reg(kAddrBpl1Dat, (v & 1) ? 0xFFFF : 0x0000); // triggers load, both planes
        tick(settleTicks);
    }

    // Drives all four bitplanes directly as a four-bit register index
    // (plane N -> index bit N-1) -- used only by the plain-Hires control,
    // not by any SuperHires stimulus (SuperHires stimuli hold bpu=2, the
    // manual's own plane-depth ceiling, p.300). Settles longer than
    // drive_value2: plain Hires shifts at half the SuperHires rate.
    void select_register4(int reg) {
        write_reg(kAddrBpl2Dat, (reg & 2) ? 0xFFFF : 0x0000);
        write_reg(kAddrBpl3Dat, (reg & 4) ? 0xFFFF : 0x0000);
        write_reg(kAddrBpl4Dat, (reg & 8) ? 0xFFFF : 0x0000);
        write_reg(kAddrBpl1Dat, (reg & 1) ? 0xFFFF : 0x0000);
        tick(24);
    }

    // Drives one bitplane high or low, held (repeated writes) -- used by
    // the two non-SuperHires controls (single plane, bpu=1).
    void drive_plane1(bool high) {
        uint16_t p1 = high ? 0xFFFF : 0x0000;
        for (int i = 0; i < 8; ++i) write_reg(kAddrBpl1Dat, p1);
    }

    uint32_t rgb() const {
        return (static_cast<uint32_t>(dut->red) << 16) |
               (static_cast<uint32_t>(dut->green) << 8) |
               static_cast<uint32_t>(dut->blue);
    }

    Vdenise* dut;
    int phase = 0;
};

// ---------------------------------------------------------------------
// The p.301 row formula and the six-run stimulus battery.
// ---------------------------------------------------------------------

struct Colour2 { int r, g, b; }; // each field two bits (0-3)

// Register n's twelve-bit content under the row formula: colour (n&3)'s
// own two-bit R/G/B fields in the high half of each nibble, colour (n>>2)
// in the low half (p.301: "Row n carries Color (n mod 4) ... and Color
// (n div 4) ...").
uint16_t regval12(const Colour2& hi, const Colour2& lo) {
    uint16_t rNib = static_cast<uint16_t>((hi.r << 2) | lo.r);
    uint16_t gNib = static_cast<uint16_t>((hi.g << 2) | lo.g);
    uint16_t bNib = static_cast<uint16_t>((hi.b << 2) | lo.b);
    return static_cast<uint16_t>((rNib << 8) | (gNib << 4) | bNib);
}

uint16_t registerValue(const Colour2 A[4], int n) {
    return regval12(A[n & 3], A[n >> 2]);
}

void write_all_registers(Harness& h, const Colour2 A[4]) {
    for (int n = 0; n < 16; ++n) h.write_color(n, registerValue(A, n));
}

// A solid run of colour v: both live bitplanes held at v, sampled every
// tick, refreshed before it can decay.
// settleTicks/chunkTicks default to a budget safe for SHRES's own
// full-rate shift (every tick, real content decays after ~16 ticks); a
// caller driving this under a slower shift rate (plain Hires, shres=0)
// passes a longer settle and chunk, safe under Hires's own (roughly
// doubled) decay budget.
std::vector<uint32_t> solid_run(Harness& h, int v, int total_ticks,
                                 int settleTicks = 16, int chunkTicks = 6) {
    std::vector<uint32_t> out;
    while (static_cast<int>(out.size()) < total_ticks) {
        h.drive_value2(v, settleTicks);
        int chunk = std::min(chunkTicks, total_ticks - static_cast<int>(out.size()));
        auto s = h.sample(chunk);
        out.insert(out.end(), s.begin(), s.end());
    }
    return out;
}

// A mixed run: loads a 16-bit pattern into each of the two live
// bitplanes once, settles a few ticks for the output pipeline to catch
// up, then samples every tick as the rest of the pattern shifts out.
std::vector<uint32_t> mixed_run(Harness& h, uint16_t pat1, uint16_t pat2, int total_ticks,
                                  int settleTicks = 3) {
    h.write_reg(kAddrBpl2Dat, pat2);
    h.write_reg(kAddrBpl1Dat, pat1); // loads both shifters; natural shift begins
    h.tick(settleTicks);
    return h.sample(total_ticks);
}

// The six-run battery: SOLID-0..SOLID-3, MIXED-1 (0xAAAA/0xCCCC), MIXED-2
// (0xF0F0/0xFF00). invertReg/invertBit >= 0 flips one bit of one
// register's row-formula content before driving; invertReg < 0 runs the
// base palette unperturbed.
using Battery = std::array<std::vector<uint32_t>, 6>;

Battery run_battery(const Colour2 A[4], int invertReg, int invertBit) {
    Battery out;
    auto setup = [&](Harness& h) {
        h.set_mode(true, 2);
        write_all_registers(h, A);
        if (invertReg >= 0) {
            uint16_t cur = registerValue(A, invertReg);
            h.write_color(invertReg, static_cast<uint16_t>(cur ^ (1u << invertBit)));
        }
    };
    for (int v = 0; v < 4; ++v) {
        Harness h; setup(h);
        out[static_cast<size_t>(v)] = solid_run(h, v, 24);
    }
    { Harness h; setup(h); out[4] = mixed_run(h, 0xAAAA, 0xCCCC, 40); }
    { Harness h; setup(h); out[5] = mixed_run(h, 0xF0F0, 0xFF00, 40); }
    return out;
}

// The steady value a solid run settled to: its most frequent sample,
// robust to a harness-timing artefact this package's own settle-and-
// refresh procedure cannot fully avoid (a handful of samples at either
// end of a run occasionally show a transient value while the pipeline
// this fixture's own colour path uses catches up or a fresh reload's own
// settle count happens to straddle a boundary) -- see FIXTURE-DATA-PATH-
// LIVE, the control this package establishes the same measurement basis
// against. The steady value dominates every run this package drives.
uint32_t steadyValue(const std::vector<uint32_t>& samples) {
    std::map<uint32_t, int> counts;
    for (auto v : samples) ++counts[v];
    uint32_t best = samples.empty() ? 0 : samples.front();
    int bestCount = -1;
    for (auto& kv : counts) {
        if (kv.second > bestCount) { bestCount = kv.second; best = kv.first; }
    }
    return best;
}

// A run has "changed" if its steady value differs (solid runs, runs 0-3)
// or if the set of distinct RGB values it produced differs (mixed runs,
// runs 4-5 -- a mixed run's own tick-to-bit alignment is not this
// package's to pin down, only which colours it produced).
bool runDiffers(int runIdx, const std::vector<uint32_t>& a, const std::vector<uint32_t>& b) {
    if (runIdx < 4) return steadyValue(a) != steadyValue(b);
    std::set<uint32_t> sa(a.begin(), a.end()), sb(b.begin(), b.end());
    return sa != sb;
}

// Which of the six runs this package's ADDRESSING CONVENTION ties to
// register nn (see header). Diagonal registers (nn a multiple of 5) tie
// to their own solid run and to MIXED-2; registers 4 and 14 tie to
// MIXED-1; every other register ties to nothing this battery reaches.
// selfTieMutation additionally, and wrongly, ties EVERY run to register
// 1 specifically -- MUT_COLORTABLE's controlled misclassification,
// blinding the untied-contamination check for that one register only, to
// show the FAIL path is reachable (see main()).
std::array<bool, 6> tiedRuns(int nn, bool selfTieMutation) {
    std::array<bool, 6> tied{};
    if (nn % 5 == 0) { tied[static_cast<size_t>(nn / 5)] = true; tied[5] = true; }
    if (nn == 4 || nn == 14) tied[4] = true;
    if (selfTieMutation && nn == 1) tied.fill(true);
    return tied;
}

std::string join(std::initializer_list<const char*> ids) {
    std::ostringstream os;
    bool first = true;
    for (auto id : ids) {
        if (!first) os << ", ";
        os << id;
        first = false;
    }
    return os.str();
}
std::string join12(const char* const* ids) {
    std::ostringstream os;
    for (int i = 0; i < 12; ++i) {
        if (i) os << ", ";
        os << ids[i];
    }
    return os.str();
}
std::string hexReg(int reg) {
    std::ostringstream os;
    os << std::hex << std::uppercase;
    os.width(2); os.fill('0');
    os << reg;
    return os.str();
}

} // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =====================================================================
    // CONTROL -- the fixture can deliver a known value before anything
    // below is trusted to read one back. (bpu=1, single plane -- colour
    // index 0 or 1 directly.)
    // =====================================================================
    {
        Harness h;
        h.set_mode(false, 1);
        h.write_color(0, 0x0F00); // COLOR00 R-nibble=0xF -> doubled 0xFF0000
#if MUT_CONTROL
        h.write_color(1, 0x0F00); // MUTATION: same as COLOR00, no longer distinct
#else
        h.write_color(1, 0x00F0); // COLOR01 G-nibble=0xF -> doubled 0x00FF00
#endif
        uint32_t v0 = h.rgb();
        h.drive_plane1(true);
        uint32_t v1 = h.rgb();
        checks.expect(v0 != v1 && v0 == 0xFF0000 && v1 == 0x00FF00,
            "[control] CONTROL-KNOWN-VALUES-ARRIVE: two distinct, known "
            "colour-register writes (COLOR00=0x0F00, COLOR01=0x00F0) "
            "produce two distinct, correctly-positioned RGB readings (" +
            hex6(v0) + " then " + hex6(v1) + ") at denise's own "
            "red/green/blue pins as the displayed colour index changes, "
            "before any assertion below trusts it. No manual page: the "
            "control for every assertion in this file.");
    }

    // =====================================================================
    // FIXTURE-DATA-PATH-LIVE (no claim id) -- proves this fixture's own
    // four-phase clock pattern delivers a loaded, non-zero bitplane
    // sample, not silently-dead colour 0.
    // =====================================================================
    {
        Harness h;
        h.set_mode(false, 1);
        h.write_color(0, 0x0000);
        h.write_color(1, 0x0FFF); // COLOR01 = white, unmistakable if reached
        uint32_t baseline = h.rgb();
#if MUT_FIXTURE
        h.drive_plane1(false); // MUTATION: plane 1 left low, never reaches colour index 1
#else
        h.drive_plane1(true);
#endif
        uint32_t driven = h.rgb();
        checks.expect(baseline == 0 && driven == 0xFFFFFF,
            "[control] FIXTURE-DATA-PATH-LIVE: a BPL1DAT write of an "
            "all-ones plane reaches the bitplane shifter and is displayed "
            "as COLOR01 (" + hex6(driven) + ") within this fixture's own "
            "load timing -- not starved. No manual page: harness "
            "self-check.");
    }

    // =====================================================================
    // OCS-BASELINE-0RGB-FORMAT (p.301) -- the plain, non-SuperHires 0x0RGB
    // format, on the same write/read basis as every SuperHires measurement
    // below. Asserts only the top nibble of each 8-bit component: this
    // boundary's ports are 8 bits wide, and no page cited by this package
    // states how a stored 4-bit value fills the other four.
    // =====================================================================
    {
        Harness h;
        h.set_mode(false, 1);
        h.write_color(0, 0x0000);
#if MUT_OCSBASELINE
        h.write_color(1, 0x0000); // MUTATION: no longer a distinguishable pattern
#else
        h.write_color(1, 0x0A5C); // R=0xA, G=0x5, B=0xC -- distinguishable nibbles
#endif
        h.drive_plane1(true);
        uint32_t v = h.rgb();
        uint32_t rTop = (v >> 20) & 0xF, gTop = (v >> 12) & 0xF, bTop = (v >> 4) & 0xF;
        std::ostringstream topNibbles;
        topNibbles << std::hex << std::uppercase << rTop << gTop << bTop;
        checks.expect(rTop == 0xA && gTop == 0x5 && bTop == 0xC,
            "OCS-BASELINE-0RGB-FORMAT (p.301, " +
            join({"C-shres-color.c4", "C-shres-color.c5", "C-shres-color.c6",
                  "C-shres-color.c7"}) +
            "): with shres=0, COLOR01 written as 0x0A5C (R=0xA, G=0x5, "
            "B=0xC) reproduces the plain 4-bit-per-component 0x0RGB format "
            "-- each written nibble appears, unaltered, as the top four "
            "bits of denise's red/green/blue pins (measured top nibbles 0x" +
            topNibbles.str() + ", expected 0xA5C; full reading " + hex6(v) +
            "). 'Instead of 4 bits of red, green, and blue for bitplane "
            "registers 0-3 stored as 0x0RGB in four color registers, "
            "SuperHires bitplane colors are specially encoded...' read as "
            "describing what happens without SuperHires active.");
    }

    // =====================================================================
    // SELECT-REGISTER-ADDRESSES-CORRECTLY (no claim id) -- a plain-Hires
    // control on the sixteen-register address bus itself, independent of
    // any SuperHires measurement: three distinct registers, three
    // distinct plain 0x0RGB patterns, addressed via a four-bitplane
    // index, each read back its own value and no other's.
    // =====================================================================
    {
        Harness h;
        h.set_mode(false, 4);
        h.write_color(3, 0x0F00);
#if MUT_SELECTREG
        h.write_color(0xC, 0x0F00); // MUTATION: same as COLOR03, no longer distinct
#else
        h.write_color(0xC, 0x00F0);
#endif
        h.write_color(0xF, 0x000F);
        h.select_register4(3);
        uint32_t v3 = h.rgb();
        h.select_register4(0xC);
        uint32_t vC = h.rgb();
        h.select_register4(0xF);
        uint32_t vF = h.rgb();
        checks.expect(v3 == 0xFF0000 && vC == 0x00FF00 && vF == 0x0000FF &&
                          v3 != vC && vC != vF && v3 != vF,
            "[control] SELECT-REGISTER-ADDRESSES-CORRECTLY: three distinct "
            "registers (COLOR03=0x0F00, COLOR0C=0x00F0, COLOR0F=0x000F), "
            "addressed via a four-bitplane index under plain Hires "
            "(shres=0), each read back its own distinct, "
            "correctly-positioned value (" + hex6(v3) + ", " + hex6(vC) +
            ", " + hex6(vF) + ") and no other's. No manual page: harness "
            "self-check on the register bus, independent of SuperHires.");
    }

    // Base palette: four colours, each field a distinct two-bit value
    // (0-3), and no two colours share a component value -- so any bit
    // this package moves or swaps produces a detectably different
    // reading, never an accidental match.
    static const Colour2 kPaletteA[4] = {
        {0, 1, 2}, {1, 2, 3}, {2, 3, 0}, {3, 0, 1},
    };

    // =====================================================================
    // SHRES-REINTERPRETS-SAME-REGISTERS (p.297) -- all sixteen registers
    // written from the same base palette via the row formula; a solid run
    // of colour-index 1 (bpu=2, planes 1-2) sampled with shres=0 and again
    // with shres=1 -- same register-bus addressing, only the SuperHires
    // flag differs. PASS requires the two readings to differ.
    // =====================================================================
    {
        Harness h0;
        h0.set_mode(false, 2);
        write_all_registers(h0, kPaletteA);
        // shres=0 shifts slower; refresh repeatedly (the same proven
        // pattern FIXTURE-DATA-PATH-LIVE and OCS-BASELINE-0RGB-FORMAT use
        // for their own single-plane, shres=0 stimulus) rather than one
        // long settle.
        for (int i = 0; i < 6; ++i) h0.drive_value2(1);
        uint32_t ocs = h0.rgb();

        Harness h1;
        write_all_registers(h1, kPaletteA);
        h1.set_mode(true, 2);
#if MUT_REINTERP
        h1.write_color(1, static_cast<uint16_t>(registerValue(kPaletteA, 1) ^ 0x0FFF)); // MUTATION: force a difference unrelated to shres
#endif
        h1.drive_value2(1);
        uint32_t shres = h1.rgb();

        checks.expect(ocs != shres,
            "SHRES-REINTERPRETS-SAME-REGISTERS (p.297, " +
            join({"C-shr.c06", "C-superhires-color.c1"}) +
            "): all sixteen lower colour registers written from one base "
            "palette (p.301's own row formula), same colour-index-1 "
            "stimulus, same register-bus addressing -- shres=0 reads " +
            hex6(ocs) + ", shres=1 reads " + hex6(shres) + ". 'There are "
            "new color register interpretations as part of the SuperHires "
            "mode.'");
    }

    // =====================================================================
    // SHRES-BITPLANE-TABLE-COLOR-00 .. -0F -- the main sweep. All sixteen
    // registers written from the base palette (row formula); for each
    // register, each of its twelve significant bits is inverted in turn
    // and the six-run battery (see header, ADDRESSING CONVENTION) is
    // re-measured against the unperturbed baseline. FAIL if a run this
    // package's own convention does not tie to that register changes;
    // PASS if a tied run changes and no untied run does; otherwise the
    // bit is not observable through this battery.
    // =====================================================================
    Battery baseline = run_battery(kPaletteA, -1, 0);

    for (int nn = 0; nn < 16; ++nn) {
        int failCount = 0, passCount = 0, notObsCount = 0;
        int firstFailBit = -1, firstFailRun = -1, firstPassBit = -1;
        for (int bit = 0; bit < 12; ++bit) {
            Battery modified = run_battery(kPaletteA, nn, bit);
            auto tied = tiedRuns(nn, MUT_COLORTABLE != 0);
            bool changedTied = false, changedUntied = false;
            int untiedRun = -1;
            for (int r = 0; r < 6; ++r) {
                if (runDiffers(r, baseline[static_cast<size_t>(r)], modified[static_cast<size_t>(r)])) {
                    if (tied[static_cast<size_t>(r)]) changedTied = true;
                    else { changedUntied = true; if (untiedRun < 0) untiedRun = r; }
                }
            }
            if (changedUntied) {
                ++failCount;
                if (firstFailBit < 0) { firstFailBit = bit; firstFailRun = untiedRun; }
            } else if (changedTied) {
                ++passCount;
                if (firstPassBit < 0) firstPassBit = bit;
            } else {
                ++notObsCount;
            }
        }

        static const char* const kRunNames[6] = {
            "SOLID-0", "SOLID-1", "SOLID-2", "SOLID-3", "MIXED-1(0xAAAA/0xCCCC)", "MIXED-2(0xF0F0/0xFF00)",
        };

        std::string label = "SHRES-BITPLANE-TABLE-COLOR-" + hexReg(nn) +
            " (p.301, " + std::string(kRowIds[nn]) + ", " + join12(kBitIds[nn]) + ")";
        for (auto& lc : kLegend) {
            if (lc.reg == nn) {
                label += ", legend " + std::string(lc.srcb) + ", " +
                         join({lc.sym[0], lc.sym[1], lc.sym[2]});
            }
        }
        label += ": of 12 significant bits, " + std::to_string(failCount) +
            " moved a run/component this package's own addressing "
            "convention does not tie to this register (bit " +
            std::to_string(firstFailBit) + " moved " +
            (firstFailRun >= 0 ? std::string(kRunNames[firstFailRun]) : std::string("-")) +
            "), " + std::to_string(passCount) + " moved only the tied "
            "run(s) (first: bit " + std::to_string(firstPassBit) + "), " +
            std::to_string(notObsCount) + " moved nothing this package's "
            "six-run battery samples.";

        if (failCount > 0) {
            checks.expect(false, label);
        } else if (passCount > 0) {
            checks.expect(true, label);
        } else {
            checks.record(label);
        }
    }

    // =====================================================================
    // SHRES-USES-SIXTEEN-LOWER-REGISTERS (p.301) -- XORs $0FFF into
    // COLOR0A (one of the twelve registers outside COLOR00-03), and
    // re-measures the four solid runs (SOLID-0..SOLID-3, the two live
    // bitplanes' own raw addressing -- the two mixed runs never move
    // register 0xA under this package's own addressing convention either,
    // see tiedRuns, so the solid runs alone already give this check its
    // best chance to observe something): green if any of the four
    // changes at all. Control: the identical XOR under plain Hires
    // (BPLCON0 = hires=1, bpu=2, shres=0), where none should ever change.
    // =====================================================================
    {
#if MUT_SIXTEEN
        const int probeReg = 0x1; // MUTATION: a register COLOR00-03 reaches directly
#else
        const int probeReg = 0xA;
#endif
        auto setupXor = [&](Harness& h, bool hiresMode) {
            h.set_mode(!hiresMode, 2, hiresMode);
            write_all_registers(h, kPaletteA);
            uint16_t cur = registerValue(kPaletteA, probeReg);
            h.write_color(probeReg, static_cast<uint16_t>(cur ^ 0x0FFF));
        };
        auto setupPlain = [&](Harness& h, bool hiresMode) {
            h.set_mode(!hiresMode, 2, hiresMode);
            write_all_registers(h, kPaletteA);
        };

        bool anyChange = false;
        for (int v = 0; v < 4; ++v) {
            Harness hb; setupPlain(hb, false);
            Harness hx; setupXor(hx, false);
            if (steadyValue(solid_run(hb, v, 24)) != steadyValue(solid_run(hx, v, 24))) anyChange = true;
        }
        checks.expect(anyChange,
            "SHRES-USES-SIXTEEN-LOWER-REGISTERS (p.301, " +
            join({"C-shrc.c04", "C-shrc.c05"}) +
            "): COLOR" + hexReg(probeReg) + " (one of the twelve registers "
            "outside COLOR00-03) XORed with $0FFF against the base "
            "palette, re-measured across all four solid runs -- " +
            (anyChange ? std::string("at least one run changed.")
                       : std::string("no run changed.")) +
            " 'SuperHires bitplane colors are specially encoded in the "
            "sixteen lower color registers.'");

        // Control: identical XOR of the same register under plain Hires.
        bool hiresChanged = false;
        for (int v = 0; v < 4; ++v) {
            Harness hb; setupPlain(hb, true);
            Harness hx; setupXor(hx, true);
            if (steadyValue(solid_run(hb, v, 24)) != steadyValue(solid_run(hx, v, 24))) hiresChanged = true;
        }
        checks.expect(!hiresChanged,
            "[control] SHRES-USES-SIXTEEN-LOWER-REGISTERS, HIRES BPU=2 "
            "($A001): the identical XOR of COLOR" + hexReg(probeReg) +
            " under plain Hires (shres=0), re-measured across all four "
            "solid runs -- " +
            (hiresChanged ? std::string("some run changed.") : std::string("no run changed.")) +
            " Control basis for the SuperHires measurement above.");
    }

    // =====================================================================
    // SHRES-TWO-BITS-RESOLUTION (p.301) -- a solid run of colour-index 1
    // against the base palette, then against 18 one-bit changes of the
    // OTHER three colours (3 colours x 3 components x 2 bits): "only two
    // bits of red, green and blue color resolution per hires pixel"
    // requires the run's own ticks to stay identical, since a solid run
    // of one colour should never depend on any other colour's own bits.
    // =====================================================================
    {
        Harness hb;
        hb.set_mode(true, 2);
        write_all_registers(hb, kPaletteA);
        std::vector<uint32_t> baselineTicks = solid_run(hb, 1, 32);

        int identical = 0, differing = 0;
        std::string firstDiffDesc;
#if MUT_TWOBITS
        const int otherColours[1] = {1}; // MUTATION: perturb colour 1 itself, not an "other" colour
        const int numOther = 1;
#else
        const int otherColours[3] = {0, 2, 3};
        const int numOther = 3;
#endif
        for (int oi = 0; oi < numOther; ++oi) {
            int c = otherColours[oi];
            for (int comp = 0; comp < 3; ++comp) {
                for (int bitIdx = 0; bitIdx < 2; ++bitIdx) {
                    Colour2 modified[4];
                    for (int i = 0; i < 4; ++i) modified[i] = kPaletteA[i];
                    int mask = 1 << bitIdx;
                    switch (comp) {
                        case 0: modified[c].r ^= mask; break;
                        case 1: modified[c].g ^= mask; break;
                        default: modified[c].b ^= mask; break;
                    }
                    Harness hv;
                    hv.set_mode(true, 2);
                    write_all_registers(hv, modified);
                    std::vector<uint32_t> ticks = solid_run(hv, 1, 32);
                    if (ticks == baselineTicks) {
                        ++identical;
                    } else {
                        ++differing;
                        if (firstDiffDesc.empty()) {
                            std::ostringstream os;
                            os << "colour " << c << " component " << "RGB"[comp]
                               << " bit " << bitIdx;
                            firstDiffDesc = os.str();
                        }
                    }
                }
            }
        }
        int totalTested = identical + differing;
        checks.expect(differing == 0,
            "SHRES-TWO-BITS-RESOLUTION (p.301, " +
            join({"C-shrc.c01", "C-shrc.c02"}) +
            "): a solid run of colour-index 1 against " + std::to_string(totalTested) +
            " one-bit changes of the other three colours -- " + std::to_string(identical) +
            " of " + std::to_string(totalTested) + " left the run's own ticks identical, " +
            std::to_string(differing) + " changed it" +
            (differing > 0 ? (" (first: " + firstDiffDesc + ")") : std::string()) +
            ". 'SuperHires mode has a coarser granularity of color control "
            "than either Hires or Lores modes. ... There are only two "
            "bits of red, green and blue color resolution per hires "
            "pixel.'");
    }

    // =========================================================================
    // RECORDED, NOT GRADED. Three claims routed to this package.
    // =========================================================================
    checks.record("C-shrc.c06 (p.302): 'SuperHires color encryption is not "
        "reflected in the ColorTable' names a graphics.library structure "
        "(the manual's own next clause: reflected instead in copper lists "
        "via MakeVPort()/SetRGB4()), not a chip register.");
    checks.record("C-shres-color.c9 (p.302, same quote as C-shrc.c06): "
        "same reason -- the software ColorTable is not a hardware concept "
        "at this boundary.");
    checks.record("C-shrc.c08 (p.301, p.302): its causal premise, loss of "
        "lower bits of precision in SuperHires colours, is already "
        "established (graded: C-shrc.c01 C-shrc.c02) by SHRES-TWO-BITS-"
        "RESOLUTION above; the remaining content, 'may be visually "
        "difficult to distinguish', is a human-perception consequence not "
        "observable at this boundary -- withdrawn whole: an id whose "
        "claim bundles a clause this package does not test is not graded, "
        "rather than attached to that assertion with a caveat.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
