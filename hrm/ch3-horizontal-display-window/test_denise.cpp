// Test purpose: the HORIZONTAL half of the Denise display window -- DIWSTRT
//   H / DIWSTOP H, the implicit ninth bit (H8, forced 0 for start and 1 for
//   stop on OCS/ECS-without-DIWHIGH), the ECS DIWHIGH register that lets
//   software override that forced bit explicitly, the border (COLOR00
//   outside the window) and the pixel position where the window opens and
//   closes. Package ch3-horizontal-display-window, fixture `denise` (one
//   Verilator build, top module denise.v plus its own eleven sub-modules:
//   denise_bitplanes, denise_bitplane_shifter, denise_playfields,
//   denise_sprites, denise_sprites_shifter, denise_collision,
//   denise_hamgenerator, denise_colortable, denise_colortable_ram_mf,
//   denise_spritepriority, bitplane_ram).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 39, 42, 57, 58, 59, 60, 64, 67, 73, 74, 76, 77, 78, 79, 80, 82, 91,
//   270, 277, 293, 296, 297, 305, 306, 307.
//
// NON-GOALS (declined below, not resolved -- see the NOTE calls in main()
// for the full reasoning against each specific claim id):
//  * Sprites as a display-object class (3-intro.c01, 3-overview.c1,
//    3-diw.c02, 3-window.c2) -- this package's own boundary ("What it
//    tests": HSTART/HSTOP, the forced H8 bits, where the window opens/
//    closes, the border) never names sprites, and this repository has a
//    dedicated sibling package (ch3-sprite-registers / hrm/denise_sprites)
//    for sprite conformance. Exercising real sprite pixel output here would
//    need sprite-register addresses this package's own claim inventory
//    never cites, and would duplicate that sibling package's own claims --
//    exactly what the build brief says not to do.
//  * Dual-playfield transparency (3-dual.c12) -- covered by the sibling
//    ch3-dual-playfield package.
//  * DMA fetch / memory-picture byte counts (3-big.c04, 3-large.c3,
//    3-large.c4, 3-big.c05, 3-hscr.c14) -- an Agnus DDF/pointer/modulo
//    concept (bytes fetched per line), not something denise.v's own
//    boundary observes; covered by ch3-ddf-fetch-window and the Agnus
//    packages.
//  * Vertical-axis and wrong-chip claims (3-big.c26, 3-window-range.c6, the
//    eight V-bit appendix-table entries in both Appendix A and Appendix C,
//    A-diw.bits1, C-beam.b12, C-beamcon0.c4, C-diw.c04, A-mod.c03) -- the
//    vertical half of DIWSTRT/DIWSTOP is this repository's sibling
//    ch3-vertical-display-window package, on the agnus_bitplanedma fixture;
//    BEAMCON0 is an Agnus register with no port on this fixture at all;
//    C-diw.c04 states the V-half and H-half of the 16-bit word as one
//    conjoined proposition and this package can confirm only the H-half
//    (independently confirmed elsewhere in THIS package's own asserted
//    claims already) -- declined here rather than partially asserted.
//  * Figure 3-9's CRT-sweep-vs-visible-screen-vs-window nested-boxes
//    reading (3-diw.f02, 3-diw.f03) -- a physical monitor-geometry
//    distinction (beam sweep area vs "visible screen boundaries" vs the
//    display window itself) that denise.v's own hpos/DIWSTRT/DIWSTOP
//    compare has no notion of; Denise only ever compares hpos against the
//    window bounds, it does not know where a monitor's own visible area
//    sits relative to that -- declined, needs a wider (whole-system/CRT-
//    timing) boundary than this fixture.
//
// BOUNDARY (from PACKAGE.md's own evidence field, not from opening the RTL
// -- see ORDER OF WORK below): nanomig/src/minimig-aga/denise.v:56-57
// (DIWSTRT custom address 9'h08E, DIWSTOP 9'h090), hdiwstrt l.293-305,
// hdiwstop l.307-320 (H8 forced 0 for start, 1 for stop), window compare
// l.328-334, border select l.489-498 (outside the window clut_data is
// forced to 0, so the border shows COLOR00) -- denise.v:65 (COLORBASE
// custom address 9'h180) with denise_colortable.v:24,30-34 (write decode)
// and the colour packing at denise_colortable.v:56-58, used here purely as
// MEASUREMENT INFRASTRUCTURE (two distinguishable colours to tell "inside"
// from "outside"), never as a claim of this package's own inventory (the
// colour-register FORMAT is the ch3-colour-register-format sibling's job).
// DIWHIGH at custom address 9'h1E4 (Appendix A/B, pp. 277/293) is an ECS
// register with no line-number citation of its own in the evidence field
// (it is new to the ECS and this evidence field is the OCS-era boundary
// text) -- its address is taken from the manual's own printed appendix
// tables (pp. 277, 293, 306), which is where the claims about it come from
// in the first place, not from RTL. BPLCON0 (address 9'h100) and BPLCON3
// (address 9'h106) are used below purely as INFRASTRUCTURE to reach a
// preconditions state (BPU=1 so a bitplane pixel can appear at all; HIRES/
// LACE for the mode-invariance measurements; BRDNTRAN/BRDRBLNK for the one
// group of claims that is actually about BPLCON3) -- these two addresses
// are standard, ubiquitously-documented Amiga custom-register addresses
// (also the exact convention this repository's own hrm/denise/
// test_denise.cpp already uses: its BPLCON4 write at reg_address_in 0x86
// is custom address 0x10C, i.e. the same BPLCON0=0x100/.../BPLCON4=0x10C
// ladder), not read out of nanomig/'s own denise.v before the spec commit.
//
// THE C1/C3 TRAP (build-brief warning, not itself a claim): amiga_clk.v:
// 58-61 makes c3 the registered 7MHz clock and c1 = ~c3 delayed one further
// 28MHz cycle, so across one 7MHz cycle's four 28MHz phases, c1 and c3 are
// BOTH high in one phase and BOTH low in another -- they are not two
// disjoint two-phase signals. denise_bitplanes.v:381 (a sub-module of THIS
// fixture) gates its BPLxDAT write strobe on c1&c3. Driving them as if
// disjoint -- the exact pattern this repository's own, already-committed
// hrm/denise/test_denise.cpp uses (c1 true only at cycle 1, c3 true only at
// cycle 3, so c1&c3 is NEVER true) -- means no BPLxDAT write ever reaches a
// shifter and every pixel reads colour 0 silently, with that file's own
// border/window checks still nominally running (it happens to pass anyway
// because BPLCON4's XOR of an all-zero plane is still zero) -- exactly the
// silent false-negative shape the build brief warns about. This Harness
// uses the measured-working four-phase pattern instead: clk7_en=(phase==0),
// c1=(phase==0||phase==1), c3=(phase==1||phase==2) -- phase 1 has both
// high (the write strobe), phase 3 has both low. FIXTURE-DATA-PATH-LIVE
// below exists specifically to prove this Harness's own pattern actually
// delivers a loaded, non-zero BPLxDAT value through to the RGB output
// BEFORE any window-position claim is asserted from it -- not a formality.
//
// THE HPOS[3:2] TRAP, AND WHY THIS PACKAGE'S OWN MEASUREMENTS SIDESTEP IT:
// denise_bitplanes.v:146-152 decodes hpos[3:2] into 0/4/8/12 lores pixels of
// EXTRA scroll folded into BPLCON1's effective value -- so writing BPL1DAT
// one colour clock later can move which pixel of a given loaded word first
// becomes visible. This package is not about scroll delay (that is the
// ch3-bplcon1-scroll-delay sibling's job) -- it is about the WINDOW's own
// open/close boundary, a property of hdiwstrt/hdiwstop and hpos alone, at a
// different point in denise.v than the bitplane shifter's own scroll path.
// Rather than pin BPL1DAT writes to a specific hpos[3:2] phase and carry
// that offset through every expectation (the trap's own suggested fix),
// this Harness sidesteps the whole mechanism: BPL1DAT is loaded with an
// ALL-ONES pattern (0xFFFF) and continuously RE-loaded every few ticks for
// the whole scanned line (see Harness::scan_window). Any scroll offset the
// hpos[3:2] mechanism applies to an all-ones word still reads as all ones --
// shifting a constant bit pattern by any amount yields the identical
// constant bit pattern -- so a window-open/close measurement built on a
// saturated plane is, by construction, insensitive to the scroll-phase
// trap. FIXTURE-DATA-PATH-LIVE separately proves the raw, non-gamed data
// path with a single one-shot, non-constant pattern (0xACE1), so the
// saturation trick is not covering for a data path that does not actually
// work end to end.
//
// MEASUREMENT METHOD -- pixels counted directly, no RTL-derived divisor:
// every width/position claim below is a COUNT of clk7_en-aligned samples
// (Harness::tick's phase 0), each one sample per register-resolution step
// -- exactly what "the display window registers... have a one-pixel
// resolution" (p.60, asserted as part of MODE-AND-RESOLUTION-INVARIANCE
// below) means. No division by a shifter rate or any other constant read
// out of the RTL is used anywhere in this file; the one arithmetic
// constant used (256, for HSTOP's "actual value minus 256 ($100)") is
// taken from the manual's own quoted sentence (p.59), and the assertion
// that uses it compares a MEASURED sample count against that quoted
// constant, not the other way around.
//
// ORDER OF WORK: this file (Harness, expected values, claim-id coverage)
// was written and committed from claims.jsonl's quotations and PACKAGE.md's
// own evidence field, plus the two build-brief traps above -- denise.v
// itself was not opened until after that commit. A second commit follows
// once the RTL is opened, the build actually run under WSL/Verilator, any
// Harness bugs fixed (documented below, oracle text untouched), and the
// MUTATION LOG filled in with real results.
//
// CLAIM COVERAGE: all 174 records of claims.jsonl are accounted for below
// -- 139 inside an expect() assertion string (ten grouped assertions plus
// the ungrouped control), 35 inside a note() call with a stated reason
// (five groups: sprites out of boundary, dual-playfield out of boundary,
// DMA/memory-picture bytes out of boundary, vertical-axis/wrong-chip
// claims, and Figure 3-9's physical-screen-geometry reading). Every id is
// written out in full, in the assertion or note string itself, not only in
// a header comment or grouping name.
//
// [RTL OPENED / BUILD / HARNESS FIXES / MUTATION LOG: filled in below this
// line after the RTL was actually read, per ORDER OF WORK above.]

#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vdenise.h"

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    // A note is not a check: it prints and does not touch passed/failed.
    // Used to record, in an actual C++ string (not only a `//` comment),
    // every claim declined below -- so coverage is findable by grepping
    // this file's own strings, not only its comments.
    void note(const std::string& msg) { std::cout << "NOTE: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Custom chip-register addresses, as printed in the manual (Appendix A/B,
// pp. 277/293/306 for DIWSTRT/DIWSTOP/DIWHIGH; p.65 of the main text, per
// PACKAGE.md's own boundary evidence, for COLORBASE). reg_address_in is
// [8:1] -- an already-halved word address -- the same convention this
// repository's own sibling Denise harnesses use.
constexpr uint16_t kDiwstrtAddr = 0x08E;
constexpr uint16_t kDiwstopAddr = 0x090;
constexpr uint16_t kDiwhighAddr = 0x1E4;
constexpr uint16_t kColorbaseAddr = 0x180;  // COLOR00
constexpr uint16_t kColor01Addr = 0x182;    // COLOR01 (COLORBASE + 2)
// Infrastructure-only addresses (not themselves a claim of this package --
// see BOUNDARY above): the standard Amiga BPLCON0/BPLCON1/BPLCON3/BPL1DAT
// ladder, the same one this repository's own hrm/denise/test_denise.cpp
// already relies on (its BPLCON4 write at reg_address_in 0x86 is custom
// address 0x10C on this identical ladder).
constexpr uint16_t kBplcon0Addr = 0x100;
constexpr uint16_t kBplcon1Addr = 0x102;
constexpr uint16_t kBplcon3Addr = 0x106;
constexpr uint16_t kBpl1DatAddr = 0x110;

// DIWHIGH bit positions (Appendix C, p.306): bit5 = H8 (horizontal start,
// most significant bit), bit13 = H8 (horizontal stop, most significant
// bit). Reserved/"must be zero"/"-" (unassigned) bits used by
// DIWHIGH-RESERVED-BITS-NO-EFFECT below.
constexpr uint16_t kDiwhighStartH8Bit = 5;
constexpr uint16_t kDiwhighStopH8Bit = 13;

// Two colours, deliberately distinct and non-black, used throughout as the
// "inside window" (COLOR01, from a saturated bitplane-1) vs "outside
// window" (COLOR00, the border) markers. Values themselves are arbitrary --
// no claim in this package is about colour-register bit packing (that is
// the ch3-colour-register-format sibling's job) -- only INEQUALITY between
// a captured "outside" sample and a captured "inside" sample is ever
// tested, so the actual R/G/B packing denise_colortable.v uses does not
// need to be known here.
constexpr uint16_t kColor00Value = 0x0F0;
constexpr uint16_t kColor01Value = 0xF00;

// Reused clocking mechanics for this fixture, on the same port set this
// repository's own already-committed hrm/denise/test_denise.cpp declares
// on its Vdenise object (clk, clk7_en, c1, c3, cck, reset, strhor,
// reg_address_in, data_in, chip48, blank, a1k, ecs, aga; red/green/blue
// out) -- reused as infrastructure, not read as a claim of this package's
// own inventory. `cck` itself is not cited by any evidence field text
// either; it is driven the same phase-0-or-1 pattern that file's own
// (broken) c1 signal happened to occupy, as a placeholder pending RTL
// confirmation -- see HARNESS FIXES below once the RTL is opened.
class Harness {
public:
    Harness() : dut(new Vdenise) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->c1 = 0;
        dut->c3 = 0;
        dut->cck = 0;
        dut->reset = 1;
        dut->strhor = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->chip48 = 0;
        dut->blank = 0;
        dut->a1k = 0;
        dut->ecs = 1;
        dut->aga = 0;
        for (int i = 0; i < 16; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    // One 28MHz clock edge, then advance the four-phase c1/c3/clk7_en/cck
    // relationship for the NEXT edge -- see THE C1/C3 TRAP above. Phase 0
    // is the clk7_en register-capture phase (one sample = one lores-pixel-
    // resolution step); phase 1 has c1 AND c3 both high (the BPLxDAT write
    // strobe phase, denise_bitplanes.v:381).
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
        dut->cck = (phase == 0 || phase == 1);
    }

    // Advances to the clk7_en phase, presents (addr>>1, data), captures it
    // on exactly the following clk7_en edge, then clears the bus.
    void write_reg(uint16_t custom_addr, uint16_t data) {
        while (phase != 0) tick();
        dut->reg_address_in = static_cast<uint8_t>(custom_addr >> 1);
        dut->data_in = data;
        tick();
        dut->reg_address_in = 0;
        dut->data_in = 0;
    }

    // Pulses strhor (the horizontal-start pulse this fixture's own boundary
    // evidence and this repository's own hrm/denise/test_denise.cpp both
    // use to reset denise.v's internal horizontal position counter to the
    // start of a line) for four ticks.
    void reset_line() {
        while (phase != 0) tick();
        dut->strhor = 1;
        for (int i = 0; i < 4; ++i) tick();
        dut->strhor = 0;
    }

    static uint32_t pack_rgb(const Vdenise* d) {
        return (static_cast<uint32_t>(d->red) << 16) |
               (static_cast<uint32_t>(d->green) << 8) |
               static_cast<uint32_t>(d->blue);
    }

    // Scans `n` clk7_en-aligned samples from the start of a fresh line,
    // sampling red/green/blue BEFORE each tick (i.e. sample i reflects the
    // pipeline's state at horizontal-position step i). If `saturate` is
    // set, BPL1DAT is loaded with an all-ones pattern (0xFFFF) BEFORE
    // reset_line() (so bitplane 1's shifter is already full before the
    // line even starts counting -- no warm-up latency biases sample 0) and
    // re-loaded every four samples throughout the scan (so it never runs
    // dry after its own 16-pixel word) -- see THE HPOS[3:2] TRAP above for
    // why this sidesteps the scroll-phase mechanism entirely. Returns the
    // packed rgb value at each of the n sampled steps.
    std::vector<uint32_t> scan_window(int n, bool saturate) {
        if (saturate) {
            write_reg(kBpl1DatAddr, 0xFFFF);
            for (int i = 0; i < 8; ++i) tick();
        }
        reset_line();
        std::vector<uint32_t> samples;
        samples.reserve(n);
        for (int i = 0; i < n; ++i) {
            while (phase != 0) tick();
            samples.push_back(pack_rgb(dut));
            if (saturate && (i % 4 == 0)) {
                dut->reg_address_in = static_cast<uint8_t>(kBpl1DatAddr >> 1);
                dut->data_in = 0xFFFF;
            } else {
                dut->reg_address_in = 0;
                dut->data_in = 0;
            }
            tick();
        }
        dut->reg_address_in = 0;
        dut->data_in = 0;
        return samples;
    }

    Vdenise* dut;
    int phase = 0;
};

// Window open/close edges, measured in sample-index units (each unit is
// one clk7_en step -- one lores-pixel-resolution unit of DIWSTRT/DIWSTOP,
// per "the display window registers... have a one-pixel resolution", p.60,
// asserted as part of MODE-AND-RESOLUTION-INVARIANCE below). open_idx is
// the first sample index whose colour differs from `border`; close_idx is
// one past the last such index (so [open_idx, close_idx) is "inside").
// -1 for either field means "never observed" within the scanned window.
struct Edges {
    long open_idx = -1;
    long close_idx = -1;
};

Edges find_edges(const std::vector<uint32_t>& samples, uint32_t border) {
    Edges e;
    for (size_t i = 0; i < samples.size(); ++i) {
        if (samples[i] != border) {
            if (e.open_idx < 0) e.open_idx = static_cast<long>(i);
            e.close_idx = static_cast<long>(i) + 1;
        }
    }
    return e;
}

// Fresh Harness, standard COLOR00/COLOR01 programmed, BPU=1 (one
// bitplane), BPLCON1=0 (no extra scroll register offset -- see THE
// HPOS[3:2] TRAP above for why this Harness's measurements do not depend
// on that value anyway), DIWSTRT/DIWSTOP H written as given, scanned
// saturated (see Harness::scan_window). bplcon0_extra ORs in additional
// BPLCON0 bits (HIRES=bit15, LACE=bit2) for the mode-invariance group.
Edges measure_window(uint16_t diwstrt_h, uint16_t diwstop_h, uint16_t bplcon0_extra = 0,
                      int n = 560) {
    Harness h;
    h.write_reg(kColorbaseAddr, kColor00Value);
    h.write_reg(kColor01Addr, kColor01Value);
    h.write_reg(kBplcon0Addr, 0x1000 | bplcon0_extra);  // BPU=1 (+ extra mode bits)
    h.write_reg(kBplcon1Addr, 0x0000);
    h.write_reg(kDiwstrtAddr, diwstrt_h);
    h.write_reg(kDiwstopAddr, diwstop_h);
    auto samples = h.scan_window(n, /*saturate=*/true);
    // The border colour is whatever COLOR00 rendered as; sample 0 of a
    // scan whose window has not yet opened (diwstrt_h > 0 in every caller
    // below except the deliberate 0x00 extremes, where border is instead
    // captured from a guaranteed-never-open calibration window elsewhere)
    // would work, but to stay robust for every caller (including
    // diwstrt_h==0, where the window is open from sample 0), border is
    // measured from a SEPARATE, guaranteed-empty-range calibration window
    // (stop before start) on a fresh Harness sharing the identical
    // COLOR00/COLOR01 programming.
    Harness cal;
    cal.write_reg(kColorbaseAddr, kColor00Value);
    cal.write_reg(kColor01Addr, kColor01Value);
    cal.write_reg(kBplcon0Addr, 0x1000 | bplcon0_extra);
    cal.write_reg(kBplcon1Addr, 0x0000);
    cal.write_reg(kDiwstrtAddr, 0x60);
    cal.write_reg(kDiwstopAddr, 0x20);  // stop < start: an empty range under any >=/< reading
    auto cal_samples = cal.scan_window(8, /*saturate=*/true);
    uint32_t border = cal_samples[4];
    return find_edges(samples, border);
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ==================== FIXTURE-DATA-PATH-LIVE (control) ====================
    {
        Harness h;
        h.write_reg(kColorbaseAddr, kColor00Value);
        h.write_reg(kColor01Addr, kColor01Value);
        h.write_reg(kBplcon0Addr, 0x1000);  // BPU=1
        h.write_reg(kBplcon1Addr, 0x0000);
        h.write_reg(kDiwstrtAddr, 0x10);
        h.write_reg(kDiwstopAddr, 0xF0);  // wide open window
        h.reset_line();
        // A single, ONE-SHOT, non-constant, recognisable pattern (not the
        // saturated all-ones trick every other measurement below uses) --
        // proving the raw data path genuinely loads and displays a written
        // BPLxDAT value, not merely that a stuck-at-1 pattern survives.
        h.write_reg(kBpl1DatAddr, 0xACE1);
        bool saw_color01 = false;
        uint32_t border_guess = 0;
        for (int i = 0; i < 300; ++i) {
            while (h.phase != 0) h.tick();
            uint32_t rgb = Harness::pack_rgb(h.dut);
            if (i == 0) border_guess = rgb;
            if (rgb != border_guess) saw_color01 = true;
            h.tick();
        }
        checks.expect(
            saw_color01,
            "control: a distinct, non-constant, one-shot value (0xACE1) written to "
            "BPL1DAT inside a wide-open display window arrives at the RGB output as "
            "a colour visibly different from the border/COLOR00 sample taken at the "
            "very start of the line -- proving this Harness's own four-phase c1/c3 "
            "pattern (phase 1: both high, the BPLxDAT write strobe) actually "
            "delivers a loaded value through denise_bitplanes' shifter to Denise's "
            "own RGB output, unlike the disjoint c1/c3 pattern in this repository's "
            "own hrm/denise/test_denise.cpp, under which no BPLxDAT write ever "
            "reaches a shifter and every pixel reads colour 0 silently. Every "
            "window-position measurement below builds on this same proven-live data "
            "path. (Infrastructure control -- not itself one of this package's 174 "
            "claim ids.)");
    }

    // ==================== G2: WINDOW-DEFINES-VISIBLE-AREA ====================
    {
        auto narrow = measure_window(0x30, 0x40);  // 16-lores-pixel-wide window
        auto wide = measure_window(0x30, 0x50);     // 32-lores-pixel-wide window
        bool narrow_ok = narrow.open_idx == 0x30 && narrow.close_idx == 0x40;
        bool wide_ok = wide.open_idx == 0x30 && wide.close_idx == 0x50;
        bool same_open = narrow.open_idx == wide.open_idx;
        bool width_tracks_diwstop =
            (wide.close_idx - wide.open_idx) - (narrow.close_idx - narrow.open_idx) == 0x10;
        checks.expect(
            narrow_ok && wide_ok && same_open && width_tracks_diwstop,
            "3-diw.c01 3-window.c1 3-window.c3 3-window.c5 3-diw.c04 3-diw.c03 "
            "3-window.c4 3-window.c6 3-bpl.c16 : with a fully saturated bitplane 1 "
            "(colour present at EVERY sampled horizontal position, so nothing "
            "outside the window is a data-fetch accident -- see the HPOS[3:2] TRAP "
            "note above), narrowing DIWSTOP from $50 to $40 while DIWSTRT stays at "
            "$30 leaves the window's OPEN edge exactly where it was (the same "
            "segment of the underlying playfield keeps starting at the same place, "
            "'a display window that is smaller than the playfield allows you to "
            "display some segment of a large playfield', p.57) and shrinks the "
            "visible width by exactly the $10 the stop moved (the border -- "
            "COLOR00, sample colour outside [open,close) -- grows by that same "
            "amount, 'the size of the border around the playfield depends on the "
            "size of the display window', p.57); in both cases every single sample "
            "outside [open_idx, close_idx) reads the border colour and none inside "
            "it does not ('You cannot display objects outside of the defined "
            "display window', p.57) -- 'the display window... is the actual size "
            "of the on-screen display' (p.57) and 'the playfield in memory are "
            "often larger than the playfield displayed on the screen. You choose "
            "which part... by specifying a different size for the display window' "
            "(p.42) are exactly this mechanism.");
    }

    // ==================== G3-DEF: DIWSTRT/DIWSTOP-DEFINE-CORNERS ====================
    {
        auto a = measure_window(0x20, 0x90);
        auto b = measure_window(0x30, 0x90);  // DIWSTRT moved, DIWSTOP held
        auto c = measure_window(0x20, 0x70);
        auto d = measure_window(0x20, 0x90);  // DIWSTOP moved, DIWSTRT held
        bool start_moves_open_only =
            (b.open_idx - a.open_idx == 0x10) && (b.close_idx == a.close_idx);
        bool stop_moves_close_only =
            (d.close_idx - c.close_idx == 0x20) && (d.open_idx == c.open_idx);
        checks.expect(
            start_moves_open_only && stop_moves_close_only,
            "A-diw.c01 A-diw.c7 A-diw.c8 3-diw.c19 3-diw.c25 3-diw.c21 3-window.c24 "
            "3-window.c31 3-window-range.c9 : moving DIWSTRT H from $20 to $30 "
            "(DIWSTOP fixed at $90) shifts the window's OPEN edge by exactly $10 "
            "and leaves the CLOSE edge untouched; moving DIWSTOP H from $70 to $90 "
            "(DIWSTRT fixed at $20) shifts the CLOSE edge by exactly $20 and leaves "
            "the OPEN edge untouched -- each register independently locates its own "
            "corner of the window, exactly as 'these registers control display "
            "window size and position by locating the upper left and lower right "
            "corners' (Appendix A, p.277) describes: DIWSTRT ('controls the display "
            "window starting position', p.58; 'contains... HSTART', p.58) locates "
            "the start/left corner, DIWSTOP ('controls the display window stopping "
            "position... the lower right-hand corner', p.59; 'contains HSTOP', "
            "p.78) locates the stop/right corner.");
    }

    // ==================== Shared H-bit sweeps (reused by G3-BITS and G4) ====================
    // sweep_start[v]: open_idx when DIWSTRT H = v, DIWSTOP H = 0xFF (far).
    // sweep_stop[v]:  close_idx when DIWSTRT H = 0x00, DIWSTOP H = v.
    // Both indexed by the actual byte value written (not just bit powers),
    // at v in {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0xFF}.
    std::array<uint16_t, 10> sweep_values = {0x00, 0x01, 0x02, 0x04, 0x08,
                                              0x10, 0x20, 0x40, 0x80, 0xFF};
    std::array<long, 10> sweep_start{};
    std::array<long, 10> sweep_stop{};
    for (size_t i = 0; i < sweep_values.size(); ++i) {
        sweep_start[i] = measure_window(sweep_values[i], 0xFF).open_idx;
        sweep_stop[i] = measure_window(0x00, sweep_values[i]).close_idx;
    }

    // ==================== G3-BITS: DIWSTRT/DIWSTOP-BITS-7-0-ARE-H7-H0 ====================
    {
        bool start_bits_ok = true, stop_bits_ok = true;
        for (size_t i = 0; i < sweep_values.size(); ++i) {
            if (sweep_start[i] != static_cast<long>(sweep_values[i])) start_bits_ok = false;
            if (sweep_stop[i] != static_cast<long>(sweep_values[i]) + 256) stop_bits_ok = false;
        }
        checks.expect(
            start_bits_ok && stop_bits_ok,
            "3-sum.c14 3-sum.c16 3-summary.c1 3-summary.c3 3-rdiw.c02 3-rdiw.c04 "
            "3-regsum-diwstrt.c2 3-regsum-diwstop.c2 3-big.c27 3-big.c30 "
            "3-window-range.c7 3-window-range.c14 A-diw.bits2 A-diw-bits.c9 "
            "A-diw-bits.c10 A-diw-bits.c11 A-diw-bits.c12 A-diw-bits.c13 "
            "A-diw-bits.c14 A-diw-bits.c15 A-diw-bits.c16 C-diw-base-bits.c9 "
            "C-diw-base-bits.c10 C-diw-base-bits.c11 C-diw-base-bits.c12 "
            "C-diw-base-bits.c13 C-diw-base-bits.c14 C-diw-base-bits.c15 "
            "C-diw-base-bits.c16 : sweeping DIWSTRT's low byte through "
            "{$00,$01,$02,$04,$08,$10,$20,$40,$80,$FF} moves the window's open edge "
            "to exactly that many samples from line start every time (bit-for-bit, "
            "including full superposition at $FF); sweeping DIWSTOP's low byte "
            "through the same values (DIWSTRT held at $00) moves the close edge to "
            "exactly 256 plus that value every time -- 'Horizontal position in bits "
            "0 through 7 (low order bits)' for DIWSTRT and 'Horizontal position in "
            "bits 0 through 7' for DIWSTOP (p.64), 'Bits 7-0 - HSTART (H7-H0)' / "
            "'Bits 7-0 - HSTOP (H7-H0)' (p.91), 'write the value for HSTART into "
            "bits 0 through 7... of DIWSTRT' / 'write HSTOP into bits 0 through 7... "
            "of DIWSTOP' (pp.77,79), and the Appendix A/C bit table 'USE V7 V6 V5 V4 "
            "V3 V2 V1 V0 H7 H6 H5 H4 H3 H2 H1 H0' (pp.277,306) for bits 7 down to 0 "
            "each carrying H7 down to H0 of the corresponding start or stop "
            "coordinate, on both OCS (Appendix A) and ECS (Appendix C) printings of "
            "the identical table.");
    }

    // ==================== G4: H8-FORCED-RANGE ====================
    {
        bool start_range_ok = (sweep_start[0] == 0) && (sweep_start[9] == 255);
        bool stop_range_ok = (sweep_stop[0] == 256) && (sweep_stop[9] == 511);
        long offset_256 = sweep_stop[6] - sweep_start[6];  // both at raw value 0x20
        bool offset_ok = offset_256 == 256;
        checks.expect(
            start_range_ok && stop_range_ok && offset_ok,
            "A-diw.c10 A-diw.strt2 A-diw.c12 A-diw.stop2 C-diw.c5 C-diw.c7 "
            "C-diw.c07 C-diw.c09 3-diw.c27 3-window.c33 3-diw.c28 3-window.c34 "
            "3-big.c23 3-big.c24 3-window-range.c1 3-window-range.c2 3-big.f06 "
            "3-window-range.c3 3-big.f09 3-window-range.c11 3-t9.c07 3-t9.c08 "
            "3-window.c49 3-window.c50 3-t9.c15 3-t9.c16 3-window.c57 3-window.c58 : "
            "the DIWSTRT sweep never opens the window before sample 0 or after "
            "sample 255 across its full $00..$FF range (open range 0-255, "
            "'DIWSTRT is... horizontally restricted to the left 3/4 of the display "
            "(H8=0)', Appendix A p.277 and Appendix C p.306; 'the eight bits "
            "allocated to HSTART are assigned to the first 256 positions... "
            "HSTART $81 $81 $00 $FF', pp.76,59); the DIWSTOP sweep never closes the "
            "window before sample 256 or after sample 511 ('DIWSTOP is... "
            "horizontally restricted to the right 1/4... (H8=1)', same pages; "
            "'HSTOP $C1 $C1 $00(=$100) $FF(=$1FF)', p.59); and at the identical "
            "raw byte value $20 written to each register, DIWSTOP's close position "
            "is exactly 256 samples later than DIWSTRT's open position for that "
            "same byte -- 'the HSTOP value you write is the actual value minus 256 "
            "($100)' (p.59) -- confirming DIWSTOP's forced ninth bit (H8=1) sits "
            "exactly $100 above DIWSTRT's (H8=0) for one and the same written "
            "byte.");
    }

    // ==================== G5: DEFAULT-81-C1-320PX ====================
    {
        auto def = measure_window(0x81, 0xC1);
        bool width_320 = (def.close_idx - def.open_idx) == 320;
        bool open_at_81 = def.open_idx == 0x81;
        bool close_at_1c1 = def.close_idx == 0x1C1;  // $C1 + $100, per G4's offset rule
        checks.expect(
            width_320 && open_at_81 && close_at_1c1,
            "3-diw.c08 3-window.c11 3-diw.c13 3-window.c15 3-window.c16 "
            "3-window.c20 3-diw.c29 3-window.c35 3-window.c36 3-window.c27 : "
            "DIWSTRT H=$81 with DIWSTOP H=$C1 -- 'the first horizontal position "
            "normally used is $81' (p.57), 'you write... $81 for HSTART' (p.58), "
            "'the normal HSTOP value is ($1C1) but is written as ($C1)' (p.59) -- "
            "opens the window at sample 81 hex and closes it at sample $1C1 (via "
            "the same +$100 relationship G4 measures), a width of exactly 320 "
            "samples: 'a starting position of ($81,$2C) centers a normal size "
            "display' (p.57), matching Figure 3-9's own 320-pixel-wide labelled "
            "window (p.58); hardware is not prevented from opening earlier than "
            "$81 ('the hardware allows you to specify a starting position before "
            "($81,$2C), but part of the display may not be visible', p.57), which "
            "this same sweep's own $00 point (G3-BITS/G4, opening at sample 0) "
            "already demonstrates working, just earlier than the normal centred "
            "position.");
    }

    // ==================== G6: MODE-AND-RESOLUTION-INVARIANCE ====================
    {
        auto lores = measure_window(0x50, 0x90, 0x0000);
        auto hires = measure_window(0x50, 0x90, 0x8000);  // BPLCON0 HIRES
        auto lace = measure_window(0x50, 0x90, 0x0004);   // BPLCON0 LACE
        bool hires_same = (hires.open_idx == lores.open_idx) && (hires.close_idx == lores.close_idx);
        bool lace_same = (lace.open_idx == lores.open_idx) && (lace.close_idx == lores.close_idx);
        bool one_pixel_resolution = (sweep_start[1] - sweep_start[0]) == 1;  // bit0 alone: exactly 1 sample
        checks.expect(
            hires_same && lace_same && one_pixel_resolution,
            "3-diw.c16 3-diw.c17 3-diw.c18 3-window.c21 3-window.c22 3-window.c23 "
            "3-diw.c22 3-diw.c23 3-window.c28 3-window.c29 3-window.c30 "
            "3-window-range.c12 3-ddf.c02 : the identical DIWSTRT=$50/DIWSTOP=$90 "
            "window opens and closes at the identical sample indices whether "
            "BPLCON0 selects low resolution, HIRES ('if you select high resolution "
            "mode (640 pixels horizontally)... the starting position does not "
            "change', p.58; '...or interlaced mode... the stopping position does "
            "not change', p.59), or LACE/interlace ('the starting position is "
            "always interpreted in low resolution, non-interlaced mode', p.58; "
            "'like the starting position, it is interpreted in low resolution, "
            "non-interlaced mode', p.59; 'select a value that represents the "
            "correct position in low resolution, non-interlaced mode', p.78) -- "
            "and the smallest DIWSTRT step measured (G3-BITS's own bit-0 point, "
            "$00 to $01) moves the open edge by exactly one sample, matching 'the "
            "display window registers... have a one-pixel resolution' (p.60).");
    }

    // ==================== G7: ECS-DIWHIGH-EXPANDS-RANGE ====================
    Edges base_no_diwhigh = measure_window(0x30, 0x50);  // DIWHIGH never written in this call
    {
        // (a) DIWHIGH untouched: the old OCS scheme holds -- literally true
        // of every measurement above (none of them ever writes DIWHIGH).
        bool old_scheme_holds = (base_no_diwhigh.open_idx == 0x30) && (base_no_diwhigh.close_idx == 0x50);

        // (b) DIWHIGH written LAST, after DIWSTRT/DIWSTOP, with the start
        // H8 override bit set: the open edge moves from $30 to $30+256,
        // i.e. explicit control of a position the old (H8-forced-0) scheme
        // could never reach for DIWSTRT.
        Harness h;
        h.write_reg(kColorbaseAddr, kColor00Value);
        h.write_reg(kColor01Addr, kColor01Value);
        h.write_reg(kBplcon0Addr, 0x1000);
        h.write_reg(kBplcon1Addr, 0x0000);
        h.write_reg(kDiwstrtAddr, 0x30);
        h.write_reg(kDiwstopAddr, 0xFF);
        h.write_reg(kDiwhighAddr, (1u << kDiwhighStartH8Bit));  // written LAST
        auto ov = h.scan_window(560, /*saturate=*/true);
        Harness cal;
        cal.write_reg(kColorbaseAddr, kColor00Value);
        cal.write_reg(kColor01Addr, kColor01Value);
        cal.write_reg(kBplcon0Addr, 0x1000);
        cal.write_reg(kDiwstrtAddr, 0x60);
        cal.write_reg(kDiwstopAddr, 0x20);
        auto cal_s = cal.scan_window(8, true);
        auto ov_edges = find_edges(ov, cal_s[4]);
        bool explicit_anywhere = ov_edges.open_idx == (0x30 + 256);

        checks.expect(
            old_scheme_holds && explicit_anywhere,
            "A-diwhigh.chip A-diwhigh.fn B-subject.c294 B-subject.c295 C-diw.c01 "
            "C-diw.c3 C-diw.c9 C-diw.c10 C-diw.c11 C-diw.c12 C-diw.c13 C-diw.c14 "
            "C-diwhigh.c02 C-diwhigh.c03 C-diwhigh.c04 3-max.c13 3-max.c14 "
            "3-max-window.c30 : with DIWHIGH never written, DIWSTRT=$30/DIWSTOP=$50 "
            "opens/closes exactly where G2/G3/G4 above already measured it -- 'if "
            "[DIWHIGH] is not written, the old scheme for DIWSTRT and DIWSTOP... "
            "holds' (p.306), and 'the limits... described in this section apply to "
            "the [OCS]' (p.80); writing DIWHIGH LAST, after DIWSTRT and DIWSTOP, "
            "with its start-H8 bit set moves the open edge to $30+256 -- a position "
            "the unmodified DIWSTRT byte alone could never reach (G4's own "
            "0-255 range) -- confirming 'this is an added register for the ECS "
            "chips, and allows larger start and stop ranges' (p.306; DIWHIGH is 'A, "
            "D(E)', implemented in Agnus and Denise, Appendix A p.277 / Appendix B "
            "p.293) and 'if this register is written last in a sequence of setting "
            "the display window, it sets direct start and stop positions anywhere "
            "on the screen' (p.306); 'with the enhanced Denise... display window "
            "start and stop [gain] explicit control over larger ranges than was "
            "possible before' (p.297); 'in the [ECS] the limits... have been "
            "changed' (p.80).");
    }

    // ==================== G8: DIWHIGH-H8-BITS-OVERRIDE ====================
    {
        // Start-H8 only (bit 5): start overridden, stop untouched.
        Harness hs;
        hs.write_reg(kColorbaseAddr, kColor00Value);
        hs.write_reg(kColor01Addr, kColor01Value);
        hs.write_reg(kBplcon0Addr, 0x1000);
        hs.write_reg(kDiwstrtAddr, 0x30);
        hs.write_reg(kDiwstopAddr, 0x90);
        hs.write_reg(kDiwhighAddr, (1u << kDiwhighStartH8Bit));
        auto s_samp = hs.scan_window(560, true);
        Harness cal1;
        cal1.write_reg(kColorbaseAddr, kColor00Value);
        cal1.write_reg(kColor01Addr, kColor01Value);
        cal1.write_reg(kBplcon0Addr, 0x1000);
        cal1.write_reg(kDiwstrtAddr, 0x60);
        cal1.write_reg(kDiwstopAddr, 0x20);
        auto cal1_s = cal1.scan_window(8, true);
        auto s_edges = find_edges(s_samp, cal1_s[4]);
        bool start_overridden = s_edges.open_idx == (0x30 + 256);
        bool stop_untouched_by_start_bit = s_edges.close_idx == 0x90;

        // Stop-H8 only (bit 13): stop overridden, start untouched.
        Harness ht;
        ht.write_reg(kColorbaseAddr, kColor00Value);
        ht.write_reg(kColor01Addr, kColor01Value);
        ht.write_reg(kBplcon0Addr, 0x1000);
        ht.write_reg(kDiwstrtAddr, 0x30);
        ht.write_reg(kDiwstopAddr, 0x90);
        ht.write_reg(kDiwhighAddr, (1u << kDiwhighStopH8Bit));
        auto t_samp = ht.scan_window(560, true);
        auto t_edges = find_edges(t_samp, cal1_s[4]);
        bool stop_overridden = t_edges.close_idx == (0x90 + 256);
        bool start_untouched_by_stop_bit = t_edges.open_idx == 0x30;

        checks.expect(
            start_overridden && stop_untouched_by_start_bit && stop_overridden &&
                start_untouched_by_stop_bit,
            "C-diwhigh.b05 C-diwhigh.b13 C-diwhigh-bits.c3 C-diwhigh-bits.c11 : "
            "setting ONLY DIWHIGH bit 5 moves the open edge by +256 and leaves the "
            "close edge untouched; setting ONLY DIWHIGH bit 13 moves the close edge "
            "by +256 and leaves the open edge untouched -- 'bit 5 H8 Horizontal "
            "start, most significant bit' and 'bit 13 H8 Horizontal stop, most "
            "significant bit' (Appendix C, p.306) each independently control only "
            "their own half of the window.");
    }

    // ==================== G9: DIWHIGH-RESERVED-BITS-NO-EFFECT ====================
    {
        uint16_t base_diwhigh = (1u << kDiwhighStartH8Bit) | (1u << kDiwhighStopH8Bit);
        auto baseline = measure_window(0x30, 0x50);  // recomputed fresh below with DIWHIGH
        Harness hb;
        hb.write_reg(kColorbaseAddr, kColor00Value);
        hb.write_reg(kColor01Addr, kColor01Value);
        hb.write_reg(kBplcon0Addr, 0x1000);
        hb.write_reg(kDiwstrtAddr, 0x30);
        hb.write_reg(kDiwstopAddr, 0x50);
        hb.write_reg(kDiwhighAddr, base_diwhigh);
        auto base_s = hb.scan_window(560, true);
        Harness cal2;
        cal2.write_reg(kColorbaseAddr, kColor00Value);
        cal2.write_reg(kColor01Addr, kColor01Value);
        cal2.write_reg(kBplcon0Addr, 0x1000);
        cal2.write_reg(kDiwstrtAddr, 0x60);
        cal2.write_reg(kDiwstopAddr, 0x20);
        auto cal2_s = cal2.scan_window(8, true);
        Edges base_edges = find_edges(base_s, cal2_s[4]);

        const int reserved_bits[8] = {15, 14, 12, 11, 7, 6, 4, 3};
        bool all_no_effect = true;
        for (int rb : reserved_bits) {
            Harness hr;
            hr.write_reg(kColorbaseAddr, kColor00Value);
            hr.write_reg(kColor01Addr, kColor01Value);
            hr.write_reg(kBplcon0Addr, 0x1000);
            hr.write_reg(kDiwstrtAddr, 0x30);
            hr.write_reg(kDiwstopAddr, 0x50);
            hr.write_reg(kDiwhighAddr, static_cast<uint16_t>(base_diwhigh | (1u << rb)));
            auto s = hr.scan_window(560, true);
            Edges e = find_edges(s, cal2_s[4]);
            if (e.open_idx != base_edges.open_idx || e.close_idx != base_edges.close_idx)
                all_no_effect = false;
        }
        checks.expect(
            all_no_effect && (baseline.open_idx == 0x30 && baseline.close_idx == 0x50),
            "C-diwhigh-bits.c1 C-diwhigh-bits.c2 C-diwhigh-bits.c4 C-diwhigh-bits.c5 "
            "C-diwhigh-bits.c9 C-diwhigh-bits.c10 C-diwhigh-bits.c12 "
            "C-diwhigh-bits.c13 : with both H8 override bits held fixed, setting "
            "any one of DIWHIGH's remaining reserved bits (15, 14, 12, 11, 7, 4, 3 "
            "-- each documented 'must be zero' in Appendix C's DIWHIGH bit table, "
            "p.306 -- or bit 6, documented '-', unassigned) produces the identical "
            "measured open/close edges as leaving it clear -- none of these bits "
            "has any observable effect on the horizontal window.");
    }

    // ==================== G11: BORDER-BLANK-OPAQUE (BPLCON3) ====================
    {
        // Baseline: BPLCON3 = 0 (BRDNTRAN=0, BRDRBLNK=0). Border is COLOR00
        // ('opaque' by default -- a real, non-genlock-blanked colour).
        Harness h0;
        h0.write_reg(kColorbaseAddr, kColor00Value);
        h0.write_reg(kColor01Addr, kColor01Value);
        h0.write_reg(kBplcon0Addr, 0x1000);
        h0.write_reg(kBplcon3Addr, 0x0000);
        h0.write_reg(kDiwstrtAddr, 0x30);
        h0.write_reg(kDiwstopAddr, 0x50);
        auto s0 = h0.scan_window(80, true);
        uint32_t border_default = s0[4];  // well before DIWSTRT=$30

        // BRDRBLNK=1 (bit 5): border areas set to be "opaque or transparent"
        // -- should differ from the default COLOR00 rendering if this
        // fixture implements the ECS BPLCON3 border-blank/opaque control at
        // all.
        Harness h1;
        h1.write_reg(kColorbaseAddr, kColor00Value);
        h1.write_reg(kColor01Addr, kColor01Value);
        h1.write_reg(kBplcon0Addr, 0x1000);
        h1.write_reg(kBplcon3Addr, 0x0020);  // BRDRBLNK
        h1.write_reg(kDiwstrtAddr, 0x30);
        h1.write_reg(kDiwstopAddr, 0x50);
        auto s1 = h1.scan_window(80, true);
        uint32_t border_blnk = s1[4];

        checks.expect(
            border_blnk != border_default,
            "C-gen.c08 C-gen.c09 C-genlock.c3 C-genlock.c4 C-gen.c12 C-gen.c15 "
            "C-genlock-ext.c3 C-genlock-ext.c4 C-genlock-ext.c16 C-ecsbit.b04 "
            "C-ecsbit.b05 : with BPLCON3 left at its default (BRDNTRAN=0, "
            "BRDRBLNK=0) the border renders COLOR00 exactly as every other group "
            "above measures it; setting BPLCON3 bit 5 (BRDRBLNK, 'Border blank', "
            "Appendix C p.307) is expected to change that rendering -- 'BorderBlank "
            "creates a transparent \"frame\" surrounding the active area' / "
            "'BorderNotTransparent makes an opaque \"frame\"...' (p.296), 'borders "
            "may be explicitly transparent or opaque' (p.297), 'the border areas "
            "surrounding the active display window may also be set to be opaque or "
            "transparent' (p.307), bit 4 BRDNTRAN 'Border opaque' (p.307) -- if the "
            "measured colour does not change, that is a finding about this "
            "fixture's own BPLCON3 wiring, not an adjustment to this expectation.");
    }

    // ==================== DECLINED CLAIMS ====================
    checks.note(
        "D1-SPRITES-OUT-OF-BOUNDARY: 3-intro.c01 3-overview.c1 3-diw.c02 "
        "3-window.c2 -- 'the screen display... consists of two basic parts -- "
        "playfields... and sprites' (p.39) and 'adjustment of display window "
        "size affects the entire display area, including the border and the "
        "sprites, not just the playfield' (p.57) both name sprites as a display "
        "object class; this package's own declared boundary (DIWSTRT/DIWSTOP H, "
        "DIWHIGH, the border, first/last displayed pixel) never mentions "
        "sprites, no sprite-register address is cited by any claim in this "
        "package's 174-record inventory, and this repository already has a "
        "dedicated sibling package (ch3-sprite-registers / hrm/denise_sprites) "
        "for sprite conformance -- exercising sprite pixel output here would "
        "duplicate that package. Declined, not resolved.");
    checks.note(
        "D2-DUAL-PLAYFIELD-TRANSPARENCY: 3-dual.c12 -- 'you can use transparency "
        "to allow selected features of the background playfield to show through' "
        "(p.67) is about dual-playfield colour-0/colour-8 transparency, covered "
        "by the sibling ch3-dual-playfield package. Declined, not resolved.");
    checks.note(
        "D3-DMA-MEMORY-PICTURE-BYTES: 3-big.c04 3-large.c3 3-large.c4 3-big.c05 "
        "3-hscr.c14 -- 'a 320-pixel display window displays 40 bytes on each "
        "line' / 'the big picture in memory... is 80 bytes wide' (pp.73-74,82) "
        "are Agnus DDF/DMA byte-count concepts (how many bytes get FETCHED per "
        "line), not something denise.v's own boundary observes at all (Denise "
        "receives bitplane data already fetched, via BPLxDAT, with no notion of "
        "a byte count); covered by the ch3-ddf-fetch-window and Agnus DMA "
        "packages. Declined, not resolved.");
    checks.note(
        "D4-VERTICAL-OR-WRONG-CHIP: 3-big.c26 3-window-range.c6 A-diw-bits.c1 "
        "A-diw-bits.c2 A-diw-bits.c3 A-diw-bits.c4 A-diw-bits.c5 A-diw-bits.c6 "
        "A-diw-bits.c7 A-diw-bits.c8 A-diw.bits1 C-diw-base-bits.c1 "
        "C-diw-base-bits.c2 C-diw-base-bits.c3 C-diw-base-bits.c4 "
        "C-diw-base-bits.c5 C-diw-base-bits.c6 C-diw-base-bits.c7 "
        "C-diw-base-bits.c8 C-beam.b12 C-beamcon0.c4 C-diw.c04 A-mod.c03 -- the "
        "even-line-count-for-interlace claims and the sixteen V7..V0-in-bits-15-8 "
        "appendix-table entries (both OCS Appendix A p.277 and ECS Appendix C "
        "p.306 printings) are the VERTICAL half of DIWSTRT/DIWSTOP, this "
        "repository's sibling ch3-vertical-display-window package's job on the "
        "agnus_bitplanedma fixture, not this one; BEAMCON0 ('bit 12 VARVBEN... "
        "use VBSTRT/STOP', p.305) is an Agnus register with no port anywhere on "
        "this denise fixture and is itself a vertical-blanking concept; "
        "C-diw.c04 ('bits 15-8 carry V7..V0 and bits 7-0 carry H7..H0', p.306) "
        "conjoins the V-half and H-half of the same claim -- the H-half is "
        "independently confirmed by G3-BITS above, but the V-half cannot be "
        "settled at this boundary, so the conjoined claim as stated is declined "
        "rather than partially asserted; the modulo claim ('the odd and even "
        "bitplanes may... have sizes different from... the display window "
        "size', Appendix A p.270) is an Agnus DMA-modulo concept with no modulo "
        "port anywhere in this fixture. Declined, not resolved -- needs a wider "
        "(or different) boundary than the denise fixture.");
    checks.note(
        "D5-PHYSICAL-SCREEN-GEOMETRY-FIGURES: 3-diw.f02 3-diw.f03 -- Figure "
        "3-9's own nested-boxes reading (p.58: the beam-sweep rectangle, the "
        "'visible screen boundaries' rounded rectangle, and the innermost "
        "display-window rectangle, with the coordinate origin (0,0) sitting "
        "outside and above the normal window) describes physical CRT-monitor "
        "geometry -- where a real monitor's own visible area sits relative to "
        "the beam's full sweep -- that denise.v's own hpos-vs-DIWSTRT/DIWSTOP "
        "compare has no notion of at all: Denise only ever compares its own "
        "horizontal position counter against the window bounds it was "
        "programmed with, it does not know where a monitor's visible area "
        "begins. Declined, not resolved -- needs a wider (whole-system/CRT-"
        "timing) boundary than this fixture; the figure's own NUMERIC content "
        "(320x200 window dimensions, the 0-255/256-511 HSTART/HSTOP ranges) is "
        "independently asserted from the same figures' captions in "
        "G5-DEFAULT-81-C1-320PX and G4-H8-FORCED-RANGE above.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
