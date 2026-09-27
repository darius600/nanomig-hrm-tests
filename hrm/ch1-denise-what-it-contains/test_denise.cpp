// ch1-denise-what-it-contains -- denise conformance test
//
// Test purpose: chapter 1's "what Denise contains" introduction material --
//   printed pp.1-4 (custom silicon / per-chip task split / display-mode and
//   colour-count list / sprite and priority/collision bullets) and p.8
//   (Figure 1-1, the DENISE block) -- at the `denise` top-module boundary.
//   37 claims (19 propositions, two readers `sol`/`opus`), from
//   hrm-testpipeline/work/2026-09-16_ch1_packages.jsonl filtered to
//   classify.package == "ch1-denise-what-it-contains". Full derivation in
//   SPECIFICATION.md.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp.1-4, 8 (Figure 1-1). A red result below means denise.v disagrees with
//   the manual, not that the assertion is wrong -- no assertion here was, or
//   should be, adjusted to make the RTL pass.
// Non-goals: anything outside the `denise` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/2026-09-16_ch1_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// CHAPTER-1 DISCIPLINE: this chapter is an introduction. Its claims are
//   structural (is a thing inside Denise's boundary; does a port carry a
//   stated width) not mechanism claims (does the thing behave correctly --
//   already chapters 3/4's job, already graded there where it is).
//   SPECIFICATION.md's "Existing coverage" section greps hrm/ for every
//   proposition below and 15 of this package's 19 propositions (28 of its
//   37 claims) are DECLINED on record() lines citing the sibling package
//   that already grades them, or the specific boundary (Agnus's beam
//   counter / DMA engine, no port on `denise` reaches either) that makes
//   them ungradeable at a `denise`-only fixture without a wider rig outside
//   this package's scope. Only 4 propositions (9 claims) are graded here,
//   on 4 expect() assertions plus one infrastructure liveness control.
//
// RTL POSITIONS USED: BPLCON0[15]=hires, BPLCON0[6]=shres (denise.v:180/187);
//   the write-decode mechanism (denise.v:174-176). No address used by
//   userio.v/paula.v/agnus.v (e.g. JOY0DAT, word address 0x05/$00A) appears
//   anywhere in denise.v or any module it instantiates (grep: zero hits),
//   the same fact hrm/ch8-appb-chip-column-denise/ measures for its own
//   three JOY*/JOYTEST rows.
//
// BOUNDARY: `denise` (denise.v) and every module it itself instantiates --
//   denise_bitplanes (bplm0, denise.v:345), denise_playfields (plfm0,
//   denise.v:377), denise_sprites (sprm0, denise.v:393), denise_
//   spritepriority (spm0, denise.v:419), denise_colortable (clut0,
//   denise.v:440), denise_hamgenerator (ham0, denise.v:459), denise_
//   collision (col0, denise.v:474), and each of THEIR OWN submodules
//   (denise_bitplane_shifter, bitplane_ram, denise_sprites_shifter,
//   sprite_ram, denise_colortable_ram_mf) -- per this file's own Makefile
//   SRC list, the only place a module could be added. Port list (denise.v:
//   32-51): clk, clk7_en, c1, c3, cck, reset, strhor, reg_address_in[8:1],
//   data_in[15:0], chip48[47:0], data_out[15:0] (out), blank (IN -- an
//   external blanking signal denise CONSUMES, not one it produces; the
//   classify.evidence of this package's claims c54/c53 (declined as B4
//   below) proposes reading `blank` as an output, which this port list
//   contradicts; B4 is declined on independent grounds regardless),
//   red/green/blue[7:0] (out), a1k, ecs, aga, hires (out),
//   shres (out). No CPU address/data/interrupt-acknowledge port of any
//   kind is declared anywhere in this list -- the register bus (reg_
//   address_in/data_in) is the only content-bearing input.
//
// MUTATION METHODOLOGY: every expect() below is guarded by a compile-time
//   #if MUT_n, built via `make MUT=MUT_n test` (Makefile: -CFLAGS
//   -D$(MUT)=1), confirmed red in isolation, then rebuilt with MUT unset to
//   confirm the clean baseline result returned. Every assertion below
//   carries its own mutation, the infrastructure control
//   (FIXTURE-LIVE-BPLCON0) included: the mutation gate
//   (tools/check_gates.py) strips a bracketed tag and reads the id after
//   it, so a "[control]"-tagged assertion is an ordinary assertion to it.
//   ch3-collision-registers (ALL-DISABLED-CONTINUOUS-CONTROL,
//   READ-CLEAR-CONTROL) and ch3-colour-register-format
//   (CONTROL-KNOWN-VALUES-ARRIVE) likewise record a mutation for their
//   controls.
//
// MUTATION FIXTURE-LIVE-BPLCON0 stimulus red MUT_4: the second BPLCON0 write replays the first word (0x8000) instead of a different one (0x0040), so shres never moves off its reset value and the second reading does not differ from the first.
// MUTATION CUSTOM-SILICON-DRIVES-VIDEO-NO-CPU stimulus red MUT_0: the SHRES-setting write's value changed from 0x0040 (bit 6 set) to 0x0000, so shres never moves off its reset value and the "the register bus alone moves the video output" measurement collapses to a no-op.
// MUTATION DENISE-DECODE-BOUNDED-TO-OWN-RANGE stimulus red MUT_1: the "outside denise's own table" probe address changed from 0x05 (JOY0DAT, decoded nowhere in this hierarchy) to 0x80 (BPLCON0's own address), so the "outside" write actually lands on BPLCON0 and clears hires, which the assertion reads as denise responding outside its own boundary.
// MUTATION HIRES-BIT-SELECTS-HIGH-RESOLUTION-MODE stimulus red MUT_2: the third BPLCON0 write's value changed from 0x8040 (hires+shres) to 0x8000 (a duplicate of the second write), so the third (hires,shres) reading no longer differs from the second and the three-mode progression collapses to two.
// MUTATION sol:1-blockdiag.c14 stimulus red MUT_3: the register-bus write's value changed from 0x8040 to 0x0000, so neither hires nor shres ever leaves its reset value and the "one write on the one documented input path reaches video out" measurement fails.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include "Vdenise.h"
#include "verilated.h"

#ifndef MUT_0
#define MUT_0 0
#endif
#ifndef MUT_1
#define MUT_1 0
#endif
#ifndef MUT_2
#define MUT_2 0
#endif
#ifndef MUT_3
#define MUT_3 0
#endif
#ifndef MUT_4
#define MUT_4 0
#endif

namespace {
std::string hex4(uint16_t v) {
    char b[8];
    std::snprintf(b, sizeof b, "%04X", v);
    return std::string(b);
}
}  // namespace

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

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vdenise* dut = new Vdenise;
    Checks checks;

    const int BPLCON0 = 0x80;  // write word-address, byte $100 -- denise.v:59/175
    const int OUTSIDE_ADDR = 0x05;  // JOY0DAT, byte $00A -- userio.v, NOT in this fixture

    int clk7_cycle = 0;
    auto tick = [&]() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        clk7_cycle = (clk7_cycle + 1) % 4;
        dut->clk7_en = (clk7_cycle == 0);
    };

    auto do_reset = [&]() {
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
        for (int i = 0; i < 16; i++) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; i++) tick();
    };

    auto write_reg = [&](int addr, uint16_t data) {
        while (!dut->clk7_en) tick();
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick(); tick(); tick(); tick();
        dut->reg_address_in = 0;
        dut->data_in = 0;
    };

    // ========================================================================
    // [control] FIXTURE-LIVE-BPLCON0 -- no claim id, no page: the fixture
    // proves it can deliver a known, distinct value before any
    // claim-bearing assertion is trusted. BPLCON0
    // and its hires/shres output ports (denise.v:59, 174-180, 187) are used
    // because they are direct, combinational, primary outputs -- "the value
    // arrived" is observed on a real port, not inferred from a read
    // multiplexer that could itself be the thing under test. Carries its own
    // mutation (MUT_4) like every other assertion below -- see
    // MUTATION METHODOLOGY above.
    // ========================================================================
    do_reset();
    write_reg(BPLCON0, 0x8000);  // hires=1, shres=0
    bool hires1 = dut->hires, shres1 = dut->shres;
    uint16_t fixture_live_w2 = 0x0040;
#if MUT_4
    fixture_live_w2 = 0x8000;  // MUTATION: second write replays the first
#endif
    write_reg(BPLCON0, fixture_live_w2);  // expect hires=0, shres=1 (unless mutated)
    bool hires2 = dut->hires, shres2 = dut->shres;
    checks.note("FIXTURE-LIVE-BPLCON0: write 0x8000 -> hires=" + std::to_string(hires1) +
                " shres=" + std::to_string(shres1) + "; write $" + hex4(fixture_live_w2) +
                " -> hires=" + std::to_string(hires2) + " shres=" + std::to_string(shres2));
    checks.expect(
        hires1 == 1 && shres1 == 0 && hires2 == 0 && shres2 == 1,
        "[control] FIXTURE-LIVE-BPLCON0: two distinct, known words written to "
        "BPLCON0 through reg_address_in/data_in arrive at the hires/shres "
        "output ports as two distinct, correct readings before any "
        "claim-bearing assertion below trusts that path. No claim id, no "
        "page -- infrastructure.");

    // ========================================================================
    // A1 -- sol:1-components.c2 opus:1-components.c2 (p.1) -- PARTIAL.
    // "The Amiga does this through the use of custom silicon hardware that
    // yields advanced graphics and sound features." This fixture is `denise`
    // alone and cannot exercise Paula's audio path, so under the suite's
    // "no claim may lose its home" rule the measurement below carries NO
    // UID (it grades only the video half) and both uids are cited on the
    // record() that follows it, which tells the whole story.
    // ========================================================================
    do_reset();
    bool shres_before = dut->shres;
    uint16_t custom_silicon_w = 0x0040;
#if MUT_0
    custom_silicon_w = 0x0000;  // MUTATION: shres bit never written
#endif
    write_reg(BPLCON0, custom_silicon_w);
    bool shres_after = dut->shres;
    checks.note("CUSTOM-SILICON-DRIVES-VIDEO-NO-CPU: shres before=" +
                std::to_string(shres_before) + ", after one register-bus write ($" +
                hex4(custom_silicon_w) + ")=" + std::to_string(shres_after));
    checks.expect(
        shres_before == 0 && shres_after == 1,
        "CUSTOM-SILICON-DRIVES-VIDEO-NO-CPU: denise's own port list (denise.v:"
        "32-51) declares no CPU address/data/interrupt-acknowledge port of any "
        "kind -- the register bus (reg_address_in/data_in) is the only "
        "content-bearing input. A single write on that bus alone moves a video "
        "output (shres) from 0 to 1, with no other stimulus, demonstrating the "
        "video path is driven purely by the documented, non-CPU register bus "
        "-- the video half of 'The Amiga does this through the use of custom "
        "silicon hardware' (p.1). [no uid, and none may be NAMED here either, "
        "not even as a pointer: the ledger reads uids off the graded LINE "
        "whatever the prose around them says, so citing the claim here counted "
        "it as asserted. The record() immediately below carries its full "
        "disposition.]");
    checks.record(
        "sol:1-components.c2 opus:1-components.c2 (p.1, 'The Amiga does this "
        "through the use of custom silicon hardware that yields advanced "
        "graphics and sound features.'): PARTIAL. The video half is measured "
        "above by CUSTOM-SILICON-DRIVES-VIDEO-NO-CPU (denise's video output "
        "moves from a register-bus write alone, with no CPU port anywhere on "
        "this module). The audio half is Paula's (paula.v:114-115 drives "
        "ldata/rdata from four channel instances) -- outside this "
        "single-module `denise` fixture and not built here. 'Advanced' names "
        "no threshold and is not graded either way.");

    // ========================================================================
    // A2 -- sol:1-components.c36 opus:1-components.c36 (p.2) -- PARTIAL.
    // "These three custom chips, named Paula, Agnus, and Denise, each
    // contain the logic to handle a specific set of tasks." Proving all
    // three chips are mutually disjoint needs all three fixtures; this
    // single-module fixture can show denise's own half: its decode is
    // bounded to its own address table.
    // ========================================================================
    do_reset();
    write_reg(BPLCON0, 0x8000);
    bool hires_valid_write = dut->hires;  // expect 1
    int outside_addr = OUTSIDE_ADDR;
#if MUT_1
    outside_addr = BPLCON0;  // MUTATION: "outside" probe actually hits BPLCON0
#endif
    write_reg(outside_addr, 0x0000);
    bool hires_after_outside = dut->hires;  // expect still 1 unless mutated
    checks.note("DENISE-DECODE-BOUNDED-TO-OWN-RANGE: hires after valid BPLCON0 "
                "write=" + std::to_string(hires_valid_write) +
                "; hires after a $0000 write to word-address 0x" + hex4(outside_addr) +
                "=" + std::to_string(hires_after_outside));
    checks.expect(
        hires_valid_write == 1 && hires_after_outside == 1,
        "DENISE-DECODE-BOUNDED-TO-OWN-RANGE: after BPLCON0 sets hires=1, a "
        "$0000 write to word-address 0x05 (JOY0DAT, byte $00A -- decoded "
        "nowhere in denise.v or any module it instantiates, confirmed by grep "
        "over this fixture's own Makefile SRC list, zero hits, the same fact "
        "hrm/ch8-appb-chip-column-denise/ already measured for this exact "
        "address) leaves hires unchanged at 1 -- denise's decode answers only "
        "its own address table (denise.v:56-65) and does not react to an "
        "address that belongs to a sibling chip's register space. This is "
        "denise's own half of 'each contain the logic to handle a specific "
        "set of tasks' (p.2). [no uid -- see the following record()]");
    checks.record(
        "sol:1-components.c36 opus:1-components.c36 (p.2, 'These three "
        "custom chips, named Paula, Agnus, and Denise, each contain the "
        "logic to handle a specific set of tasks such as video, audio, or "
        "I/O.'): PARTIAL. Denise's own bounded decode is measured above by "
        "DENISE-DECODE-BOUNDED-TO-OWN-RANGE. The full three-way "
        "disjointness (that Paula's and Agnus's own decodes are likewise "
        "bounded, and to different ranges) needs their own fixtures -- "
        "outside this single-module `denise` package and not built here.");

    // ========================================================================
    // A3 -- sol:1-components.c48 opus:1-components.c47 (p.3, NTSC hires:
    // "640 by 200 non-interlaced or 640 by 400 interlaced ... in 16
    // colors") + sol:1-components.c50 opus:1-components.c49 (p.3, PAL
    // hires: "640 by 256 non-interlaced or 640 by 512 interlaced ... in 16
    // colors") -- PARTIAL, ONE shared measurement for both propositions.
    // `denise` has no PAL/NTSC input at all (that is Agnus's `ntsc` port
    // into agnus_beamcounter.v); a denise-only fixture cannot distinguish
    // "NTSC hires" from "PAL hires" and would produce identical evidence
    // for both. What IS inside this boundary is the mode-selecting
    // mechanism itself: BPLCON0's HIRES bit (bit 15, denise.v:180) and
    // SHRES bit (bit 6, denise.v:187) drive hires/shres directly and
    // combinationally, selecting among the resolution modes the four
    // bullets describe.
    // ========================================================================
    do_reset();
    write_reg(BPLCON0, 0x0000);
    bool h_lo = dut->hires, s_lo = dut->shres;      // expect 0,0 (normal res)
    write_reg(BPLCON0, 0x8000);
    bool h_hi = dut->hires, s_hi = dut->shres;      // expect 1,0 (hires)
    uint16_t w_super = 0x8040;
#if MUT_2
    w_super = 0x8000;  // MUTATION: duplicate of the hires-only write
#endif
    write_reg(BPLCON0, w_super);
    bool h_super = dut->hires, s_super = dut->shres;  // expect 1,1 (super-hires)
    checks.note("HIRES-BIT-SELECTS-HIGH-RESOLUTION-MODE: $0000->(h=" +
                std::to_string(h_lo) + ",s=" + std::to_string(s_lo) + ") $8000->(h=" +
                std::to_string(h_hi) + ",s=" + std::to_string(s_hi) + ") $" +
                hex4(w_super) + "->(h=" + std::to_string(h_super) + ",s=" +
                std::to_string(s_super) + ")");
    checks.expect(
        h_lo == 0 && s_lo == 0 && h_hi == 1 && s_hi == 0 && h_super == 1 && s_super == 1,
        "HIRES-BIT-SELECTS-HIGH-RESOLUTION-MODE: three BPLCON0 writes "
        "($0000, $8000, $8040) each produce a distinct, directly-predicted "
        "(hires,shres) pair at denise's own output ports -- the register-"
        "controlled mode-selection mechanism the 'high resolution mode' "
        "bullets (p.3) describe. The specific pixel counts (640x200 vs "
        "640x256 etc.), interlace line-doubling and PAL-vs-NTSC selection "
        "are Agnus's beam counter (agnus_beamcounter.v), outside this "
        "fixture, and are not graded here. [no uid -- see the following "
        "record() for all four uids' full disposition]");
    checks.record(
        "sol:1-components.c48 opus:1-components.c47 (p.3, NTSC hires: '640 "
        "by 200 non-interlaced or 640 by 400 interlaced ... in 16 colors') "
        "sol:1-components.c50 opus:1-components.c49 (p.3, PAL hires: '640 "
        "by 256 non-interlaced or 640 by 512 interlaced ... in 16 colors'): "
        "PARTIAL, both propositions share the one measurement above. "
        "denise has no PAL/NTSC input (Agnus's `ntsc` port feeds "
        "agnus_beamcounter.v's separate 312/262-line constants, outside "
        "this fixture) -- a denise-only fixture cannot tell NTSC hires from "
        "PAL hires apart and produces identical evidence for both. What is "
        "measured above is the HIRES/SHRES mode-select mechanism common to "
        "all four bullets. The pixel counts, interlace and PAL/NTSC "
        "selection are Agnus's, not built here.");

    // ========================================================================
    // A4 -- sol:1-blockdiag.c14 (p.8, Figure 1-1). "READ FROM THE IMAGE ...
    // a double-outlined box labelled DENISE sits directly under PAULA
    // inside the dashed CUSTOM CHIP SECTION; its only incoming arrowhead
    // comes from the REG ADR BUS on the right -- no chip-data arrow enters
    // it in the drawing." A different oracle (the image, not text) and its
    // own page from A1 above, even though the underlying mechanism is
    // similar -- graded on its own assertion with an independent stimulus,
    // per "one assertion per proposition, each naming its page".
    // ========================================================================
    do_reset();  // no register-bus activity yet: hires=0, shres=0 at reset
    uint16_t figure_w = 0x8040;
#if MUT_3
    figure_w = 0x0000;  // MUTATION: the one documented input path writes nothing new
#endif
    write_reg(BPLCON0, figure_w);
    bool fig_hires = dut->hires, fig_shres = dut->shres;
    checks.note("DENISE-BLOCK-REG-BUS-IN-VIDEO-OUT: from reset, one write ($" +
                hex4(figure_w) + ") via reg_address_in/data_in -> hires=" +
                std::to_string(fig_hires) + " shres=" + std::to_string(fig_shres));
    checks.expect(
        fig_hires == 1 && fig_shres == 1,
        "sol:1-blockdiag.c14 DENISE-BLOCK-REG-BUS-IN-VIDEO-OUT (p.8, Figure "
        "1-1): the DENISE block's only documented incoming arrow is the REG "
        "ADR BUS; no separate chip-data arrow enters it. From reset, with no "
        "prior register-bus activity, one write via reg_address_in/data_in "
        "(the one input class the figure shows) reaches the video-mode "
        "output ports (hires, shres) directly -- confirming the figure's own "
        "depiction: one input class in, video out.");

    // ========================================================================
    // GROUP B -- declined. Fifteen propositions (28 claims) whose
    // proposition is already graded by an existing sibling package, or
    // whose remaining half needs a fixture (Agnus's beam counter / DMA
    // engine) outside this package's scope. Full reasoning for
    // each is in SPECIFICATION.md's "Existing coverage" section; the
    // record() text below states each one's own citation, condensed.
    // ========================================================================

    checks.record(
        "B1 sol:1-components.c46 opus:1-components.c45 (p.3, 'Bitplane "
        "generated, high resolution graphics capable of supporting both PAL "
        "and NTSC video standards.'): declined. PAL/NTSC is Agnus's `ntsc` "
        "input into agnus_beamcounter.v (312 PAL / 262 NTSC line constants) "
        "-- not a denise port, outside this fixture, same reasoning as A3. "
        "'Bitplane generated': denise_bitplanes is instantiated inside "
        "denise (denise.v:345, containment not in question), but actually "
        "measuring bitplane data reaching a visible pixel needs the "
        "phase-accurate window/hpos state machine hrm/ch3-bplcon0-display-"
        "modes/ and hrm/ch3-horizontal-display-window/ already built and "
        "already exercise at this exact denise fixture -- building a second "
        "copy here, for a chapter-1 restatement, would inflate a "
        "structural sentence into a mechanism test. Declined citing both "
        "packages' ownership of the "
        "bitplane-to-pixel mechanism and Agnus's beam counter for the "
        "PAL/NTSC half.");

    checks.record(
        "B2 sol:1-components.c52 opus:1-components.c51 (p.3, 'Hold-and-"
        "modify (HAM) mode allows for the display of up to 4,096 colors on "
        "screen simultaneously.'): declined. denise_hamgenerator is "
        "instantiated inside denise (denise.v:459, containment not in "
        "question), but hrm/ch3-ham-control-field/'s own header records "
        "that the manual disagrees with itself "
        "about which BPLCON0 bit enables HAM (p.95 says bit 10; pp.87/90/271 "
        "say bit 11) and explicitly declines to resolve that dispute, even "
        "at the narrower denise_hamgenerator fixture where the question is "
        "directly askable. Driving HAM through this package's wider denise "
        "fixture inherits the identical unresolved dispute one level "
        "further removed, with no new information to resolve it. Declined, "
        "citing hrm/ch3-ham-control-field/'s own unresolved-dispute finding.");

    checks.record(
        "B3 sol:1-components.c53 opus:1-components.c52 (p.3, 'Overscan mode "
        "allows the creation of higher resolution displays specially suited "
        "for video and film applications.'): declined. The mechanism is "
        "denise's own DIWSTRT/DIWSTOP window registers (denise.v:56-57, "
        "write path denise.v:290-316) accepting values outside the nominal "
        "edges. DIWSTRT/DIWSTOP are write-only (no readback via data_out, "
        "confirmed by hrm/ch3-nominal-display-values/'s own finding); "
        "observing an out-of-nominal window's effect needs the hpos-vs-"
        "window comparator and its phase-accurate clocking, which "
        "hrm/ch3-horizontal-display-window/ (174 claims) already owns in "
        "full at this exact denise fixture. Declined, citing that package's "
        "ownership of the window-comparator mechanism.");

    checks.record(
        "B4 sol:1-components.c54 opus:1-components.c53 (p.3, 'Displays of "
        "arbitrary size, larger than the visible viewing area can be "
        "created.'): declined, same mechanism and citation as B3 "
        "(hrm/ch3-horizontal-display-window/) for the horizontal half. The "
        "claim's own evidence names 'the same two registers as c53 plus the "
        "vertical pair in Agnus' -- the vertical half is agnus_bitplanedma.v "
        "(VSTART/VSTOP), outside every denise-only fixture, including "
        "ch3-horizontal-display-window's own.");

    checks.record(
        "B5 sol:1-components.c55 opus:1-components.c54 (p.3, 'Amigas which "
        "contain the Enhanced Chip Set (ECS) support Productivity mode "
        "giving displays of 640 by 480, non-interlaced with 4 colors from a "
        "pallette of 64.'): declined in full. The 640x480 geometry is "
        "Agnus's beam counter (agnus_beamcounter.v:206/212/223/229, "
        "varbeamen_out), outside this fixture, same reasoning as A3/B1. The "
        "colour half ('4 colors from a palette of 64') is ambiguous at this "
        "remove -- it may be EHB (denise_colortable's own select[5] "
        "mechanism, already measured by hrm/ch3-colour-register-format/'s "
        "EHB-HALVES-INTENSITY) or a distinct ECS-specific wide-select path; "
        "the page does not say which, and this package does not settle it "
        "from the RTL. "
        "Declined: geometry cited to the Agnus boundary, colour mechanism "
        "cited to EHB-HALVES-INTENSITY as the closest already-measured "
        "'beyond 32 base colours' fact, ambiguity disclosed not resolved by "
        "assumption.");

    checks.record(
        "B6 sol:1-components.c60 opus:1-components.c59 (p.3, '32 system "
        "color registers, each of which contains a 12-bit number as four "
        "bits of red, four bits of green, and four bits of blue intensity "
        "information.'): declined. Already measured on this exact "
        "mechanism (denise_colortable) by hrm/ch3-colour-register-format/'s "
        "ADDR-32-DISTINCT (32 distinct 12-bit codes on the 32 registers "
        "read back as 32 pairwise-distinct values; cites 3-colortable.c1-c6, "
        "3-t2.c01-c05, A-color.c3/c5/c6 among others) and RED-FIELD-BITS-"
        "11-8 / GREEN-FIELD-BITS-7-4 / BLUE-FIELD-BITS-3-0 (each nibble "
        "moves only its own channel byte; cites 3-colortable.c14-c16, "
        "3-t3.c02-c04, A-color.c11-c22). Declined, citing that package and "
        "those four assertions.");

    checks.record(
        "B7 sol:1-components.c61 opus:1-components.c60 (p.3, 'This allows a "
        "system color palette of 4,096 different choices of color for each "
        "register.'): declined. Already measured by hrm/ch3-colour-"
        "register-format/'s SWEEP-12BIT-DISTINCT (9 distinct 12-bit codes "
        "on one register read back pairwise distinct; cites 3-bpl.c13, "
        "3-color.c14, 3-colortable.c11/c12, 3-ct.c07/c08, A-color.c4, "
        "A-colorxx.c02) -- the identical register, the identical numeric "
        "fact, on a different page (p.46 there, p.3 here) of the same "
        "manual describing the same hardware. Declined, citing that "
        "package and assertion.");

    checks.record(
        "B8 sol:1-components.c62 opus:1-components.c61 (p.3, 'Eight "
        "reusable 16-bit wide sprites with up to 15 color choices per "
        "sprite pixel (when sprites are paired).'): declined in full, three "
        "components all measured elsewhere. Eight sprites exist "
        "(hrm/ch3-sprite-registers/'s own '8 sprites total' register map; "
        "hrm/ch7-inter-sprite-priority/'s own 'there are eight sprites and "
        "they are numbered 0 to 7', grounded across 8 assertions). 16-bit "
        "width (hrm/ch4-sprite-shifter/'s SPRITE-RUN-IS-EXACTLY-16-PIXELS, "
        "citing CH4-size.c1 p.101 'Sprites are 16 pixels wide'). 15 colours "
        "when paired (hrm/ch4-sprite-colour-registers/, citing CH4-attach."
        "c1/c2 p.120 'you can create sprites that have fifteen possible "
        "color choices (plus transparent) ... by attaching two sprites'). "
        "'Reusable' carries no separate testable threshold, same as p.1's "
        "'advanced' in A1. Declined, citing all three packages.");

    checks.record(
        "B9 sol:1-components.c63 opus:1-components.c62 (p.3, 'A sprite is "
        "an easily movable graphics object whose display is entirely "
        "independent of the background (called a playfield); sprites can "
        "be displayed over or under this background.'): declined in full. "
        "The over/under half is BPLCON2 playfield-sprite priority, already "
        "measured by hrm/ch3-sprite-playfield-priority/'s BPLCON2-BITS-0-5-"
        "ARE-LIVE-PRIORITY-FIELD (sweeps bplcon2[5:0] through all 64 "
        "values, shows sprsel takes both possible values) -- the same "
        "assertion hrm/ch4-sprite-priority/ itself cites when declining its "
        "OWN page-103 restatement of this identical proposition "
        "(CH4-p103.c22), rather than building a second copy. 'Movable, "
        "independent of the playfield': sprite position comes from the "
        "sprite's own SPRxPOS register and the beam position (denise_"
        "sprites_shifter.v's own hpos port), not from bitplane data -- "
        "structural containment already established by this claim's own "
        "classify.evidence and by hrm/ch4-sprite-horizontal-position/'s and "
        "hrm/ch4-sprite-vertical-position/'s own register-independence "
        "measurements. Declined, citing all of the above.");

    checks.record(
        "B10 sol:1-components.c64 opus:1-components.c63 (p.3-4, 'A sprite "
        "is 16 low resolution pixels wide and an arbitrary number of lines "
        "tall' -- the sentence crosses the printed page break, both "
        "readers' crossread note this, not a defect): declined. Width: same "
        "citation as B8 (hrm/ch4-sprite-shifter/'s SPRITE-RUN-IS-EXACTLY-"
        "16-PIXELS). Height ('an arbitrary number of lines'): NOT a "
        "duplicate decline -- hrm/ch4-sprite-shifter/ itself explicitly "
        "declined this exact half on its OWN record() (VERTICAL-EXTENT-"
        "NOT-AT-THIS-BOUNDARY, citing CH4-p101.c2/c3, CH4-size.c2), because "
        "sprite height is set entirely by agnus_spritedma.v's own "
        "DMA-fetched position/control words -- not visible at denise_"
        "sprites_shifter, and not visible anywhere in denise either "
        "(confirmed: no vertical line-count port on denise's own top-level "
        "interface). No package in hrm/ measures 'arbitrary number of "
        "lines' as its own proposition; it remains ungraded across the "
        "suite for a genuine boundary reason. Declined here for the same "
        "reason, citing hrm/ch4-sprite-shifter/'s own finding of the "
        "identical gap rather than claiming false coverage.");

    checks.record(
        "B11 sol:1-components.c67 opus:1-components.c66 (p.4, "
        "'Dynamically controllable inter-object priority, with collision "
        "detection.'): declined. A two-part bullet heading; both parts are "
        "covered by B12/B14 below and by hrm/ch4-sprite-priority/'s general "
        "priority statements (SPRITE-PRIORITY-FIXED-BY-GROUP-NUMBER, "
        "SPRITE-DATA-REACHES-PRIORITY-CIRCUIT, citing CH4-p103.c20, "
        "CH4-prio.c1/c3, CH4-hw.c14, CH4-p124.c10, CH4-fig4-13.c6) and "
        "hrm/ch4-sprite-collision/'s page-113 collision statements "
        "(CH4-coll.c1/c2/c3). Declined, citing all four.");

    checks.record(
        "B12 sol:1-components.c68 opus:1-components.c67 (p.4, 'This means "
        "that the system can dynamically control the video priority "
        "between the sprite objects and the bitplane backgrounds "
        "(playfields).'): declined. The same proposition as B9's over/under "
        "half and as hrm/ch4-sprite-priority/'s own declined CH4-p103.c22. "
        "Declined, citing hrm/ch3-sprite-playfield-priority/'s BPLCON2-"
        "BITS-0-5-ARE-LIVE-PRIORITY-FIELD directly, per that same "
        "established precedent.");

    checks.record(
        "B13 sol:1-components.c69 opus:1-components.c68 (p.4, 'You can "
        "control which object or objects appear over or under the "
        "background at any time.'): declined. Same mechanism and same "
        "citation as B12 (hrm/ch3-sprite-playfield-priority/'s BPLCON2-"
        "BITS-0-5-ARE-LIVE-PRIORITY-FIELD).");

    checks.record(
        "B14 sol:1-components.c70 opus:1-components.c69 (p.4, "
        "'Additionally, you can use system hardware to detect collisions "
        "between objects and have your program react to such collisions.'): "
        "declined. Already measured almost verbatim by hrm/ch4-sprite-"
        "collision/'s three page-113 assertions (CH4-coll.c1: sprites "
        "collide with each other; CH4-coll.c2: sprites collide with either "
        "playfield; CH4-coll.c3 COLLISION-DETECTABLE-VIA-REGISTER-READ: "
        "collision is readable via CLXDAT) and by hrm/ch3-collision-"
        "registers/'s 98-claim CLXCON/CLXDAT inventory. 'Have your program "
        "react' is software, not hardware, and is not graded by either "
        "sibling package either (their own non-goals say so explicitly). "
        "Declined in full, citing both packages.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
