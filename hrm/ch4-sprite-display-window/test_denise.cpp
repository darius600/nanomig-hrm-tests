// Test purpose: whether a sprite is shown, hidden, or partly shown depending
//   on where its horizontal start position sits relative to the playfield
//   DISPLAY WINDOW (DIWSTRT/DIWSTOP) -- fully inside is visible, fully
//   outside is invisible ("clipped"), and straddling an edge leaves "part"
//   of it visible, per Chapter 4's own wording. Package
//   ch4-sprite-display-window, fixture `denise` (one Verilator build, top
//   module denise.v plus its ten sub-modules: denise_bitplanes,
//   denise_bitplane_shifter, denise_playfields, denise_sprites,
//   denise_sprites_shifter, denise_collision, denise_hamgenerator,
//   denise_colortable, denise_colortable_ram_mf, denise_spritepriority,
//   bitplane_ram) -- the same fixture hrm/denise/ and
//   hrm/ch3-horizontal-display-window/ already build and run.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 99, 100 (claims.jsonl for ch4-sprite-display-window, 9 records). A
//   red result below means denise.v disagrees with the manual, not that the
//   assertion is wrong -- no assertion here was, or should be, adjusted to
//   make the RTL pass.
// Non-goals: anything outside the `denise` fixture this package builds, and
//   any claim it does not name. A green here means those assertions held on
//   this fixture at the pinned revision -- not that the module is correct,
//   and not that the chapter is covered.
// Claim ids: ids such as `CH4-vpos.c6` and `CH4-hpos.c2` name one sentence of
//   the manual in this chapter's reading. They carry no reader prefix, and
//   they resolve in the claim files that hrm-testpipeline/acceptance/ch4.json
//   lists under `inventories`.
//
// BOUNDARY: every one of this package's 9 claim records carries the same
//   evidence field (classify step, PACKAGE.md's own claims.jsonl): denise.v
//   forces clut_data to the border colour (8'b0) outside the display window
//   unless brdsprt is set (denise.v:536-544); the window itself is set at
//   hpos==hdiwstrt and cleared at hpos==hdiwstop (denise.v:344-349) from
//   DIWSTRT/DIWSTOP (denise.v:308-333); that gate is decided one level above
//   denise_sprites (a bare `sprena` input there), so this package's fixture
//   is the denise TOP level, not denise_sprites alone.
//
// ORDER OF WORK: this file's assertions, expected values, and claim-id
//   coverage were written and committed from claims.jsonl's quotations
//   alone -- denise.v was not opened for this package before that commit.
//   The register addresses (DIWSTRT=$08E->0x47, DIWSTOP=$090->0x48,
//   COLOR00=$180->0xC0), the OCS/ECS implicit-ninth-bit convention (H8
//   forced 0 on a DIWSTRT write and 1 on a DIWSTOP write, so DIWSTRT's
//   raw byte reads 0-255 and DIWSTOP's reads 256-511 in the same hpos
//   units), and an initial clk7_en/c1/c3/cck clocking/write_reg pattern
//   were reused from hrm/denise/test_denise.cpp, an already-committed,
//   already-built sibling harness for this SAME top-level fixture -- not
//   learned by opening this package's own forbidden-until-committed file.
//   The sprite register addresses (SPR0POS=$140->0xA0, SPR0CTL->0xA1,
//   SPR0DATB->0xA3, SPR0DATA->0xA2, DATA written last to arm) and
//   COLOR17's address ($1A2->0xD1, the first non-transparent sprite-pair
//   colour) are reused from hrm/ch3-sprite-registers/test_denise_sprites.cpp,
//   an already-committed sibling harness for the same chip register
//   space. None of the four values this file predicts before opening the
//   RTL -- 16/16 pixels when fully inside, 0/16 when fully outside on
//   either side, and strictly between 0 and 16 when straddling an edge --
//   are taken from that sibling's own measurements or from PACKAGE.md's
//   evidence field; every one of them is derived from this package's own
//   claim quotations against window/sprite geometry this file chooses
//   itself (80/432 and 96/400 as the two display windows' raw hpos
//   bounds, 256/40/440/72/424/480/392 as the sprite start positions --
//   none of them equal to PACKAGE.md's own 129/449/320/64/460/124
//   measurement numbers, deliberately).
//
//   AFTER the spec commit, opening denise.v to wire the harness surfaced
//   two build/run issues, both fixed as harness bugs, neither as oracle
//   adjustments: (1) sprite output is gated by `sprena(display_ena |
//   brdsprt)`, and display_ena only latches after a write to BPL1DAT
//   (denise.v's own comment: "sprites are visible after the first write
//   to the BPL1DAT register in a scanline") -- a precondition this file's
//   claims don't test but sprite visibility depends on regardless, so one
//   BPL1DAT write was added per measurement (see its own comment above);
//   (2) the Harness's tick() drives the c1/c3 phase pattern
//   hrm/ch3-sprite-registers/test_denise_sprites.cpp uses for this same
//   sprite hardware, matching real c1/c3 timing: the sprite shift enable
//   is `~c1 & ~c3` (denise_sprites.v:435), true on one phase in four
//   under this pattern and on two under a pattern whose c1 and c3 are
//   never high together, which would shift at twice the lores rate.
//   FIXTURE-KNOWN-WORD-IN-ORDER exists to catch a silently dead data
//   path regardless (see RESULTS below). No expected value, page
//   quotation, or pass/fail threshold depends on either.
//
// DECLINED (2 of the 9 records, needs a wider boundary than this package's
//   own): CH4-p100.c6 ("If you specify a vertical position (Y value) of
//   less than 44 (i.e., above the top of the display window) the top edge
//   of the sprite may not appear on screen.") and CH4-vpos.c6 ("display
//   window) the top edge of the sprite may not appear on screen." -- the
//   same sentence, a second extraction) both require a VERTICAL
//   (line-position) display-window clip. Two already-committed sibling
//   packages on this exact RTL already establish, in their own disclosed
//   BOUNDARY/NON-GOALS text, that denise.v has no such mechanism:
//   hrm/ch3-horizontal-display-window/test_denise.cpp's own NON-GOALS
//   states "the vertical half of DIWSTRT/DIWSTOP is this repository's
//   sibling ch3-vertical-display-window package, on the agnus_bitplanedma
//   fixture"; hrm/ch3-vertical-display-window/test_agnus_bitplanedma.cpp's
//   own BOUNDARY confirms vdiwstrt/vdiwstop/vdiwena live in
//   agnus_bitplanedma.v, gating bitplane DMA fetch (ddfrun), not denise's
//   pixel output. A third already-committed sibling,
//   hrm/ch3-sprite-registers/test_denise_sprites.cpp, separately found
//   that denise_sprites_shifter.v's CTL-register write only ever captures
//   bits 7 (ATTACH) and 0 (SH0) of SPRxCTL -- SPRxPOS/SPRxCTL's VSTART/
//   VSTOP fields are not read there either. Between them: no vertical
//   comparator this package's own boundary (denise top level) can observe
//   decides whether a sprite's Y position clips it. Testing CH4-p100.c6/
//   CH4-vpos.c6 honestly needs at least agnus_spritedma (which decides
//   when a sprite's data is fetched, line by line) added to the boundary --
//   out of scope for this package. These two ids are recorded below via
//   checks.record(), not checks.expect(), and are not silently dropped.
//
// CLAIM COVERAGE: all 9 records of claims.jsonl are cited below, in full,
//   inside a checks.expect() or checks.record() label -- 7 asserted, 2
//   declined (see DECLINED above).
//
//   CH4-hpos.c2 -- fully-inside control (also doubles as the fixture's
//     known-word/ordering proof, see CONTROL CASE below)
//   CH4-hpos.c6 -- fully-outside-left, window 1
//   CH4-p99.c2  -- fully-outside-right, window 1
//   CH4-hpos.c5 -- straddle-left, window 1 (the "part or all" claim this
//     package cares most about getting right)
//   CH4-p99.c5  -- straddle-right, window 1 (the same "part or all"
//     proposition, independent geometry)
//   CH4-clip.c1 -- fully-outside (total clip), window 2 -- a second,
//     independently-configured display window, showing the general
//     "boundaries of the display window" statement is not an artefact of
//     window 1's own specific bounds
//   CH4-p100.c8 -- straddle, window 2 -- the same general statement's
//     partial-clip half, same second window
//
// CONTROL CASE: CH4-hpos.c2 makes two measurements, both with sprite 0
//   fully inside window 1. The first, armed with a non-uniform bit
//   pattern (0xACE1/0x0000) instead of a solid one, is read twice: once as
//   [fixture] FIXTURE-KNOWN-WORD-IN-ORDER (no claim id, no page -- proves
//   the write -> arm -> line-sample -> read-rgb path this whole file
//   shares actually delivers 16 DISTINCT, correctly ORDERED pixel values,
//   not a single stuck colour, before any claim-based assertion depends on
//   it -- matching this package's own note that "a harness whose data
//   path is silently dead reports every data-dependent assertion as a
//   finding about the design"), and once inside CH4-hpos.c2 itself (all 16
//   pattern bits present in order == nothing clipped). The second, a solid
//   image, gives count(RED)==16 on the exact same count-of-RED-samples
//   basis CH4-hpos.c6/CH4-p99.c2/CH4-hpos.c5/CH4-p99.c5/CH4-clip.c1/
//   CH4-p100.c8 below all use -- so a straddle's fractional count, or a
//   fully-outside 0, cannot be explained away as a units or off-by-one
//   artefact of the counting method itself; the SAME method is shown here
//   reaching both 0 (outside) and 16 (inside).
//
// MEASUREMENT BASIS: each measurement resets a fresh Vdenise, writes
//   COLOR00 (black, the border) and COLOR17 (red, this sprite's only
//   non-transparent colour index) once, writes DIWSTRT/DIWSTOP for the
//   window under test, arms sprite 0 at a chosen SPR0POS byte (SPR0CTL
//   held at 0 -- no SuperHires low bit, no ATTACH) with a chosen 16-bit
//   image, runs one full line to let the window state machine sync up
//   (mirroring hrm/denise/test_denise.cpp's own comment to that effect),
//   then samples red/green/blue at every pixel-clock boundary across a
//   second full line (520 positions -- DIWSTOP's raw byte alone already
//   reaches 511 under the implicit-H8-is-1 convention, so 520 comfortably
//   covers the whole line). "Visible" means the sample equals RED
//   (0xFF0000); every other sample in this file's own measurements is
//   BLACK (0x000000, the border) or, for the [fixture] check only, the
//   0xACE1 pattern's own alternation of the two.
//
// RESULTS (after both harness fixes above): 8 passed, 0 failed -- the fixture
//   check plus all 7 asserted claims are GREEN. denise.v's sprite/
//   display-window behaviour matches every one of this package's 7
//   testable claims: a sprite entirely inside a display window shows in
//   full, a sprite entirely outside (on either side, on either of two
//   independently-configured windows) shows nothing, and a sprite
//   straddling an edge (on either side, on either window) shows a
//   strictly partial count -- never the full 16, never 0. This is a
//   legitimate, fully green result: nothing here was adjusted to reach
//   it (see the two harness fixes recorded in ORDER OF WORK, both
//   mechanism-only). 2 claims declined (see DECLINED above), not counted
//   in the 8.
//
// MUTATION LOG: all 8 entries below were run against the 8-passed/0-failed
//   baseline -- edit -> rebuild -> run -> confirm the named check, and
//   only that check, went red -> revert -> rebuild -> confirm the 8/0
//   baseline restored -- before moving to the next one. nanomig/ was never touched; no expected
//   value, quotation, or comparison threshold was ever edited, only
//   stimulus (a pos_byte or an image word).
//
// MUTATION FIXTURE-KNOWN-WORD-IN-ORDER stimulus red: inside_pattern's SPR0DATA changed from 0xACE1 to 0x5312 -> FAIL ("7 passed, 1 failed"), only FIXTURE-KNOWN-WORD-IN-ORDER newly red (the sampled bits no longer matched the hardcoded 0xACE1 pattern); CH4-hpos.c2, which now reads an independent solid-pattern measurement of its own, stayed green, confirming the isolation fix held. Replayable as MUTATE=FIXTURE-KNOWN-WORD-IN-ORDER. Reverted to 0xACE1: 8/0.
// MUTATION CH4-hpos.c2 stimulus red: its own inside_solid measurement's SPR0DATA changed from 0xFFFF to 0x0000 -> FAIL ("7 passed, 1 failed"), only CH4-hpos.c2 newly red (count(RED) became 0, not the asserted 16). Replayable as MUTATE=CH4-hpos.c2. Reverted to 0xFFFF: 8/0.
// MUTATION CH4-hpos.c6 stimulus red: pos_byte changed from 0x14 (hstart=40, wholly outside) to 0x80 (hstart=256, fully inside) -> FAIL ("7 passed, 1 failed"), only CH4-hpos.c6 newly red (count(RED) became 16, not the asserted 0). Replayable as MUTATE=CH4-hpos.c6. Reverted to 0x14: 8/0.
// MUTATION CH4-p99.c2 stimulus red: pos_byte changed from 0xDC (hstart=440, wholly outside) to 0x80 (fully inside) -> FAIL ("7 passed, 1 failed"), only CH4-p99.c2 newly red. Replayable as MUTATE=CH4-p99.c2. Reverted to 0xDC: 8/0.
// MUTATION CH4-hpos.c5 stimulus red: pos_byte changed from 0x24 (hstart=72, straddling) to 0x14 (hstart=40, wholly outside) -> FAIL ("7 passed, 1 failed"), only CH4-hpos.c5 newly red (count(RED) became 0, failing the >0 half). Replayable as MUTATE=CH4-hpos.c5. Reverted to 0x24: 8/0.
// MUTATION CH4-p99.c5 stimulus red: pos_byte changed from 0xD4 (hstart=424, straddling) to 0xDC (hstart=440, wholly outside) -> FAIL ("7 passed, 1 failed"), only CH4-p99.c5 newly red. Replayable as MUTATE=CH4-p99.c5. Reverted to 0xD4: 8/0.
// MUTATION CH4-clip.c1 stimulus red: pos_byte changed from 0xF0 (hstart=480, wholly outside window 2) to 0x64 (hstart=200, fully inside window 2) -> FAIL ("7 passed, 1 failed"), only CH4-clip.c1 newly red (count(RED) became 16, not the asserted 0). Replayable as MUTATE=CH4-clip.c1. Reverted to 0xF0: 8/0.
// MUTATION CH4-p100.c8 stimulus red: pos_byte changed from 0xC4 (hstart=392, straddling window 2's stop) to 0xF0 (hstart=480, wholly outside) -> FAIL ("7 passed, 1 failed"), only CH4-p100.c8 newly red (count(RED) became 0, failing the >0 half). Replayable as MUTATE=CH4-p100.c8. Reverted to 0xC4: 8/0.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vdenise.h"

namespace {

// Runtime mutation switch: one build serves both the real run (MUTATE
// unset) and every mutation run (MUTATE=<assertion label>), read once at
// startup. mutating(id) is true only while replaying that one id.
const char* g_mutate = nullptr;
bool mutating(const std::string& id) { return g_mutate != nullptr && id == g_mutate; }

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    // For records that state what the source says, or why a claim is
    // declined. Not measurements; must not read like one.
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Word-address register map. DIWSTRT/DIWSTOP/COLOR00 reused from
// hrm/denise/test_denise.cpp (same top-level fixture); SPR0POS/CTL/DATA/
// DATB and the arm-with-DATA-last convention reused from
// hrm/ch3-sprite-registers/test_denise_sprites.cpp (same chip register
// space, sibling package for this same module family).
constexpr int DIWSTRT = 0x47;
constexpr int DIWSTOP = 0x48;
constexpr int COLOR00 = 0xC0;
constexpr int COLOR17 = 0xD1;
constexpr int SPR0POS = 0xA0;
constexpr int SPR0CTL = 0xA1;
constexpr int SPR0DATA = 0xA2;
constexpr int SPR0DATB = 0xA3;
// BPL1DAT ($110->0x88). Opened in denise.v after the spec commit (see
// ORDER OF WORK): "sprena(display_ena | brdsprt)" wires sprite output
// entirely off denise_sprites' own boundary -- display_ena is a sticky
// latch, set the first time reg_address_in matches BPL1DAT and cleared
// only by reset or blank (denise.v: "sprite display enable signal -
// sprites are visible after the first write to the BPL1DAT register in a
// scanline"). This file never sets blank, so one BPL1DAT write per fresh
// Harness keeps display_ena latched for that whole measurement. Not a
// register this package's own claims are about; used only as the
// precondition sprite visibility here needs to exist at all, same
// footing as the write-registers-before-measuring step every sibling
// harness on this fixture already takes.
constexpr int BPL1DAT = 0x88;

constexpr uint32_t RED = 0xFF0000;
constexpr uint32_t BLACK = 0x000000;

// DIWSTOP's raw byte alone reaches 511 under the OCS/ECS implicit-H8=1
// convention (see ORDER OF WORK above), so 520 pixel-clock samples
// comfortably cover one full line with margin for pipeline latency.
constexpr int SAMPLE_N = 520;

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
        dut->aga = 1;
        for (int i = 0; i < 16; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    // Phase pattern reused from hrm/ch3-sprite-registers/test_denise_sprites.cpp
    // (same register bus/sprite hardware): c1 and c3 overlap, so the
    // sprite shift enable `~c1 & ~c3` (denise_sprites.v:435) fires once
    // per 7MHz cycle, the lores rate.
    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        cyc = (cyc + 1) % 4;
        switch (cyc) {
            case 0: dut->clk7_en = 1; dut->c1 = 0; dut->c3 = 0; break;
            case 1: dut->clk7_en = 0; dut->c1 = 1; dut->c3 = 0; break;
            case 2: dut->clk7_en = 0; dut->c1 = 1; dut->c3 = 1; break;
            case 3: dut->clk7_en = 0; dut->c1 = 0; dut->c3 = 1; break;
        }
        dut->cck = (cyc == 0 || cyc == 1);
    }

    void write_reg(int addr, uint16_t data) {
        while (!dut->clk7_en) tick();
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick(); tick(); tick(); tick();
        dut->reg_address_in = 0;
        dut->data_in = 0;
    }

    // Matches hrm/denise/test_denise.cpp's own start-of-line convention.
    void start_line() {
        while (!dut->clk7_en) tick();
        dut->strhor = 1;
        tick(); tick(); tick(); tick();
        dut->strhor = 0;
    }

    std::vector<uint32_t> sample_line(int n) {
        std::vector<uint32_t> out;
        out.reserve(n);
        for (int h = 0; h < n; ++h) {
            while (!dut->clk7_en) tick();
            uint32_t rgb = (static_cast<uint32_t>(dut->red) << 16) |
                           (static_cast<uint32_t>(dut->green) << 8) |
                           static_cast<uint32_t>(dut->blue);
            out.push_back(rgb);
            tick();
        }
        return out;
    }

    Vdenise* dut;
    int cyc = 0;
};

struct Meas {
    std::vector<uint32_t> trace;
    int count(uint32_t colour) const {
        int c = 0;
        for (uint32_t v : trace) if (v == colour) ++c;
        return c;
    }
};

// One isolated measurement: fresh DUT, reset, set colours, configure the
// display window, arm sprite 0 with a 16-bit image at pos_byte, run one
// full line to sync the window state machine, sample a second full line.
Meas measure(uint8_t diwstrt_byte, uint8_t diwstop_byte, uint8_t pos_byte,
             uint16_t data, uint16_t datb) {
    Harness h;
    h.write_reg(BPL1DAT, 0x0000);  // latches display_ena (see BPL1DAT comment above).
    h.write_reg(COLOR00, 0x0000);  // border: black
    h.write_reg(COLOR17, 0x0F00);  // sprite colour index 1: red
    h.write_reg(DIWSTRT, diwstrt_byte);
    h.write_reg(DIWSTOP, diwstop_byte);
    h.write_reg(SPR0POS, pos_byte);
    h.write_reg(SPR0CTL, 0x00);
    h.write_reg(SPR0DATB, datb);
    h.write_reg(SPR0DATA, data);  // arms the sprite; written last.
    h.start_line();
    h.sample_line(SAMPLE_N);  // flush line, discarded -- lets the window sync.
    Meas m;
    m.trace = h.sample_line(SAMPLE_N);
    return m;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    // Window 1: raw hpos [80, 432). DIWSTRT byte 0x50 -> 80 (H8 forced 0).
    // DIWSTOP byte 0xB0 -> 256+176=432 (H8 forced 1). Chosen by this file,
    // not reused from PACKAGE.md's own 129/449 measurement.
    constexpr uint8_t W1_STRT = 0x50, W1_STOP = 0xB0;
    // Window 2: raw hpos [96, 400). A second, independently-chosen window,
    // used only by CH4-clip.c1 / CH4-p100.c8 below.
    constexpr uint8_t W2_STRT = 0x60, W2_STOP = 0x90;

    // ---------------- [fixture] FIXTURE-KNOWN-WORD-IN-ORDER ----------------
    // Also the measurement CH4-hpos.c2 reads below (fully inside window 1,
    // hstart=256 -> pos_byte 0x80). 0xACE1/0x0000 is a non-uniform,
    // non-symmetric 16-bit pattern (bits, MSB first: 1010 1100 1110 0001)
    // so an ordering bug (a reversed or rotated shift) would not pass by
    // accident. The pattern's first non-zero sample marks where it starts
    // in the sampled trace (a fixed pipeline latency separates "hstart"
    // from the sample index it first appears at -- this file never
    // depends on that latency's own value, only on what appears relative
    // to it, the same discipline hrm/ch3-sprite-registers/ already
    // documents for this same sprite hardware).
    // MUTATION FIXTURE-KNOWN-WORD-IN-ORDER stimulus red: MUTATE=FIXTURE-
    //   KNOWN-WORD-IN-ORDER changes inside_pattern's SPR0DATA from 0xACE1
    //   to 0x5312, so the sampled bits no longer match the hardcoded
    //   pattern.
    Meas inside_pattern = measure(W1_STRT, W1_STOP, /*pos=*/0x80,
                                   /*data=*/mutating("FIXTURE-KNOWN-WORD-IN-ORDER") ? 0x5312 : 0xACE1,
                                   /*datb=*/0x0000);
    bool inside_pattern_ordered = false;
    {
        static const int bit[16] = {1,0,1,0,1,1,0,0,1,1,1,0,0,0,0,1};
        int first = -1;
        for (size_t i = 0; i < inside_pattern.trace.size(); ++i) {
            if (inside_pattern.trace[i] == RED) { first = static_cast<int>(i); break; }
        }
        bool ordered = first != -1 &&
                       first + 16 <= static_cast<int>(inside_pattern.trace.size());
        int red_count = 0, black_count = 0;
        for (int i = 0; ordered && i < 16; ++i) {
            uint32_t got = inside_pattern.trace[first + i];
            uint32_t want = bit[i] ? RED : BLACK;
            if (got != want) ordered = false;
            if (got == RED) ++red_count; else ++black_count;
        }
        bool took_both = ordered && red_count > 0 && black_count > 0;
        inside_pattern_ordered = ordered && took_both;
        checks.expect(inside_pattern_ordered,
            "[fixture] FIXTURE-KNOWN-WORD-IN-ORDER: arming sprite 0 fully "
            "inside window 1 with the non-uniform image SPR0DATA=0xACE1, "
            "SPR0DATB=0x0000 and sampling a full line, the 16 samples "
            "starting at the first non-transparent one reproduce that "
            "exact bit pattern, RED where the bit is 1 and BLACK where it "
            "is 0, IN ORDER -- proving the write -> arm -> line-sample -> "
            "read-rgb measurement basis every assertion below shares "
            "actually delivers a known word, in order, at the cycle it "
            "was fetched on, before any claim-based assertion depends on "
            "it. Fixture check, no claim id, no page.");
    }

    // ---------------- CH4-hpos.c2 ----------------
    {
        // Same position, solid pattern, count(RED) basis -- the exact
        // measurement method CH4-hpos.c6/CH4-p99.c2/CH4-hpos.c5/CH4-p99.c5/
        // CH4-clip.c1/CH4-p100.c8 below all use. Without this, those
        // straddle/outside findings would have no green result on their
        // OWN counting method to be read against -- only the differently-
        // measured pattern-order check above.
        // MUTATION CH4-hpos.c2 stimulus red: MUTATE=CH4-hpos.c2 changes
        //   inside_solid's own SPR0DATA from 0xFFFF to 0x0000.
        Meas inside_solid = measure(W1_STRT, W1_STOP, /*pos=*/0x80,
                                     /*data=*/mutating("CH4-hpos.c2") ? 0x0000 : 0xFFFF,
                                     /*datb=*/0x0000);
        int red16 = inside_solid.count(RED);
        // Independent of inside_pattern/inside_pattern_ordered above (its
        // own fresh Harness, its own measurement) so a mutation aimed at
        // the fixture check's pattern cannot also move this one -- the
        // same isolation discipline hrm/ch3-sprite-registers/ documents
        // needing to add for its own JOINTLY-LOAD-BEARING assertion.
        checks.expect(red16 == 16,
            "CH4-hpos.c2: with sprite 0 positioned entirely inside window "
            "1's raw hpos bounds [80,432) (hstart=256, width 16 -> "
            "256..271), a solid image reads all 16 of its pixels as RED, "
            "none forced to the border, on the count(RED) basis every "
            "outside/straddle assertion below shares -- the control those "
            "findings are read against, so a straddle's fractional count "
            "cannot be dismissed as a quirk of the counting method itself: "
            "'To be visible, however, an object must be within the [...] "
            "boundaries of the playfield display window.' (p.99, "
            "CH4-hpos.c2 / CH4-p99.c2).");
    }

    // ---------------- CH4-hpos.c6 ----------------
    {
        // MUTATION CH4-hpos.c6 stimulus red: MUTATE=CH4-hpos.c6 changes
        //   pos_byte from 0x14 (hstart=40, wholly outside) to 0x80
        //   (hstart=256, fully inside).
        Meas m = measure(W1_STRT, W1_STOP, /*pos=*/mutating("CH4-hpos.c6") ? 0x80 : 0x14,  // hstart=40 -> 40..55, wholly < 80
                          /*data=*/0xFFFF, /*datb=*/0x0000);
        int red = m.count(RED);
        checks.expect(red == 0,
            "CH4-hpos.c6: sprite 0 positioned wholly to the LEFT of window "
            "1 (hstart=40, occupying 40..55, entirely below the window's "
            "raw-hpos start of 80) shows zero RED pixels across the whole "
            "line -- wholly outside means wholly clipped: 'is sometimes "
            "desirable; such a sprite is said to be \"clipped.\"' (p.99, "
            "CH4-hpos.c6).");
    }

    // ---------------- CH4-p99.c2 ----------------
    {
        // MUTATION CH4-p99.c2 stimulus red: MUTATE=CH4-p99.c2 changes
        //   pos_byte from 0xDC (hstart=440, wholly outside) to 0x80 (fully
        //   inside).
        Meas m = measure(W1_STRT, W1_STOP, /*pos=*/mutating("CH4-p99.c2") ? 0x80 : 0xDC,  // hstart=440 -> 440..455, wholly >= 432
                          /*data=*/0xFFFF, /*datb=*/0x0000);
        int red = m.count(RED);
        checks.expect(red == 0,
            "CH4-p99.c2: sprite 0 positioned wholly to the RIGHT of window "
            "1 (hstart=440, occupying 440..455, entirely at or beyond the "
            "window's raw-hpos stop of 432) shows zero RED pixels -- "
            "'To be visible, however, an object must be [...] boundaries "
            "of the playfield display window.' (p.99, CH4-p99.c2).");
    }

    // ---------------- CH4-hpos.c5 ----------------
    {
        // MUTATION CH4-hpos.c5 stimulus red: MUTATE=CH4-hpos.c5 changes
        //   pos_byte from 0x24 (hstart=72, straddling) to 0x14 (hstart=40,
        //   wholly outside).
        Meas m = measure(W1_STRT, W1_STOP, /*pos=*/mutating("CH4-hpos.c5") ? 0x14 : 0x24,  // hstart=72 -> 72..87, straddles 80
                          /*data=*/0xFFFF, /*datb=*/0x0000);
        int red = m.count(RED);
        checks.expect(red > 0 && red < 16,
            "CH4-hpos.c5: sprite 0 straddling window 1's LEFT edge "
            "(hstart=72, occupying 72..87, window starts at raw hpos 80 -- "
            "8 of its 16 columns fall outside) shows more than 0 but "
            "fewer than all 16 of its RED pixels: neither the "
            "fully-inside 16/16 above nor the fully-outside 0/16 above, "
            "on the exact same RED-count measurement basis -- 'If you "
            "specify an X value for a sprite that takes it outside the "
            "display window, then part or all of the sprite may not "
            "appear on the screen.' (p.99, CH4-hpos.c5).");
    }

    // ---------------- CH4-p99.c5 ----------------
    {
        // MUTATION CH4-p99.c5 stimulus red: MUTATE=CH4-p99.c5 changes
        //   pos_byte from 0xD4 (hstart=424, straddling) to 0xDC (hstart=440,
        //   wholly outside).
        Meas m = measure(W1_STRT, W1_STOP, /*pos=*/mutating("CH4-p99.c5") ? 0xDC : 0xD4,  // hstart=424 -> 424..439, straddles 432
                          /*data=*/0xFFFF, /*datb=*/0x0000);
        int red = m.count(RED);
        checks.expect(red > 0 && red < 16,
            "CH4-p99.c5: sprite 0 straddling window 1's RIGHT edge "
            "(hstart=424, occupying 424..439, window stops at raw hpos "
            "432 -- 8 of its 16 columns fall outside) shows more than 0 "
            "but fewer than all 16 of its RED pixels, same measurement "
            "basis as CH4-hpos.c5 above, independent geometry -- 'If you "
            "specify an X value for a sprite that takes it outside the "
            "display window, then part or all of the sprite may not "
            "appear on the screen.' (p.99, CH4-p99.c5).");
    }

    // ---------------- CH4-clip.c1 ----------------
    {
        // MUTATION CH4-clip.c1 stimulus red: MUTATE=CH4-clip.c1 changes
        //   pos_byte from 0xF0 (hstart=480, wholly outside window 2) to
        //   0x64 (hstart=200, fully inside window 2).
        Meas m = measure(W2_STRT, W2_STOP, /*pos=*/mutating("CH4-clip.c1") ? 0x64 : 0xF0,  // hstart=480 -> 480..495, wholly >= 400
                          /*data=*/0xFFFF, /*datb=*/0x0000);
        int red = m.count(RED);
        checks.expect(red == 0,
            "CH4-clip.c1: on a SECOND, independently-configured display "
            "window (raw hpos [96,400), different bounds from window 1 "
            "above), a sprite positioned wholly beyond it (hstart=480, "
            "occupying 480..495) shows zero RED pixels -- the general "
            "'totally clipped ... beyond the boundaries' statement holds "
            "on a window this file did not use for any other assertion: "
            "'As noted above, sprites will be partially or totally "
            "clipped if they pass [across or beyond the boundaries of "
            "the display window].' (p.100, CH4-clip.c1).");
    }

    // ---------------- CH4-p100.c8 ----------------
    {
        // MUTATION CH4-p100.c8 stimulus red: MUTATE=CH4-p100.c8 changes
        //   pos_byte from 0xC4 (hstart=392, straddling window 2's stop) to
        //   0xF0 (hstart=480, wholly outside).
        Meas m = measure(W2_STRT, W2_STOP, /*pos=*/mutating("CH4-p100.c8") ? 0xF0 : 0xC4,  // hstart=392 -> 392..407, straddles 400
                          /*data=*/0xFFFF, /*datb=*/0x0000);
        int red = m.count(RED);
        checks.expect(red > 0 && red < 16,
            "CH4-p100.c8: on the same second display window as "
            "CH4-clip.c1 above, a sprite straddling its stop edge "
            "(hstart=392, occupying 392..407, window stops at raw hpos "
            "400) shows more than 0 but fewer than all 16 of its RED "
            "pixels -- the general statement's partial-clip half, same "
            "window, independent geometry from every straddle above: "
            "'As noted above, sprites will be partially or totally "
            "clipped if they pass across or beyond the boundaries of "
            "the display window.' (p.100, CH4-p100.c8).");
    }

    // ---------------- DECLINED ----------------
    checks.record(
        "CH4-p100.c6 DECLINED (needs a wider boundary than this package's "
        "denise top level -- see DECLINED above): 'If you specify a "
        "vertical position (Y value) of less than 44 (i.e., above the top "
        "of the display window) the top edge of the sprite may not appear "
        "on screen.' (p.100, CH4-p100.c6). No vertical display-window "
        "comparator is observable at this boundary: vdiwstrt/vdiwstop/"
        "vdiwena live in agnus_bitplanedma.v (hrm/ch3-vertical-display-"
        "window/'s own disclosed BOUNDARY), denise.v's own window compare "
        "is horizontal-only (hrm/ch3-horizontal-display-window/'s own "
        "disclosed NON-GOALS), and SPRxPOS/SPRxCTL's VSTART/VSTOP fields "
        "are not captured by denise_sprites_shifter.v either "
        "(hrm/ch3-sprite-registers/'s own disclosed FINDING).");
    checks.record(
        "CH4-vpos.c6 DECLINED (same reason as CH4-p100.c6 above -- the "
        "same manual sentence, a second extraction): 'display window) the "
        "top edge of the sprite may not appear on screen.' (p.100, "
        "CH4-vpos.c6).");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed
              << " failed (2 declined, not counted).\n";
    return checks.failed == 0 ? 0 : 1;
}
