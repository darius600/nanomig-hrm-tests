// ch3-dual-playfield -- denise_playfields conformance test
//
// Test purpose: dual-playfield (DBLPF) mode as chapter 3 describes it: two
//   independent playfields sharing one bitplane bus, split by plane parity,
//   each with its own colour-register range and its own transparent code,
//   and the PF2PRI bit that decides which one is drawn in front. Package
//   ch3-dual-playfield, fixture denise_playfields (one Verilator build,
//   this module only, per PACKAGE.md). A sibling package,
//   ch3-playfield-colour-select, covers single-playfield colour selection
//   on the same fixture; this package is dual-playfield only.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 44, 48, 67, 68, 69, 70, 71, 72, 90, 271 (claims.jsonl for
//   ch3-dual-playfield, 98 records). A red result below means
//   denise_playfields.v disagrees with the manual, not that the assertion
//   is wrong -- no assertion here was, or should be, adjusted to make the
//   RTL pass.
// Non-goals: `aga` and `pf2of` are held at their non-goal default (0)
//   throughout -- no claim in this package's 98-record inventory names
//   either one (AGA extensions and the undocumented OCS priority-offset
//   behaviour some games rely on are out of scope here). `nplayfield` is
//   never read: no claim in this inventory describes its semantics, and
//   every proposition this package does assert is independently observable
//   through `plfdata` alone, so relying on `nplayfield` was not needed and
//   would have meant guessing at behaviour no quotation states. Six claims
//   naming independent bitplane scrolling (3-dual-mode.c2, 3-dual.c46) and
//   the high-resolution two-bitplane-per-playfield cap (3-dual-planes.c4/
//   c5/c6, 3-dual.c18/c19/c20) are declined -- see DECLINED CLAIMS below;
//   this module has no scroll/position or resolution port. One claim
//   naming sprite visibility through a transparent playfield
//   (3-dual-color.c24) is declined for the same kind of reason -- no
//   sprite port here. One claim about a plane-6/HAM DMA restriction
//   (3-bplcon0.c20) is declined as a system-level DMA policy question, not
//   something this pure per-pixel composition module enforces.
//
// BOUNDARY (from PACKAGE.md's own evidence field, carried into every claim
// record): nanomig/src/minimig-aga/denise_playfields.v:8-17 module header
//   (aga l.10, `bpldata`[8:1] l.11, dblpf l.12, pf2of l.13, `bplcon2`[6:0]
//   l.14, `nplayfield`[2:1] l.15, `plfdata`[7:0] l.16) | l.43-63 (nplayfield
//   from the odd/even planes) and l.68-93 (playfield 1 from planes 1,3,5 ->
//   colours 0-7; playfield 2 from planes 2,4,6 -> colours 8-15 on OCS) |
//   l.24 (pf2pri = bplcon2[6]).
//
// ORDER OF WORK: this file (assertions, expected values, claim-id coverage)
// was written and committed from claims.jsonl's quotations and PACKAGE.md's
// own evidence field ONLY -- denise_playfields.v itself was not opened
// until after that commit, per the build brief's explicit gate. The
// Harness's port names (aga, bpldata, dblpf, pf2of, bplcon2, plfdata) come
// from PACKAGE.md's own evidence field, which quotes the module header
// directly; the eval()-only, no-clock usage pattern is reused from
// hrm/denise_playfields/test_denise_playfields.cpp -- an already-built,
// already-committed sibling harness for the SAME module -- exactly as the
// build brief instructs ("copy the Makefile pattern... use it for its
// Makefile and as a working stimulus example"). The bit-packing convention
// assumed for bpldata (C++ bit (n-1) is Verilog plane n's bit, n=1..8) is
// likewise carried from that sibling's own comments and stimulus values,
// not learned by opening this package's own forbidden-until-committed
// file.
//
// THE CENTRAL DERIVED RULE below (used to build every priority/
// transparency stimulus, not read out of any single quote): "the visible
// code is playfield P's own code if P has priority and P's code is
// nonzero; otherwise it is the other playfield's code (offset +8 if that
// other playfield is playfield 2) if that code is nonzero; otherwise it is
// 0 (colour register 0, the background register)." This is the literal,
// most direct reading of five quoted sentences taken together -- the
// colour-register-range statements (p.67, p.72), the code-0-is-transparent
// statements (p.67), the PF2PRI priority statements (p.71, p.90, p.271),
// and the transparency-reveals-what-is-behind statement (p.71) -- and it
// is what makes "normally has priority" (PF2PRI's reset value, 0) and the
// double-transparent case (colour register 0, matching Figure 3-12's
// caption) both fall out of the same rule rather than needing a separate
// special case. Every stimulus in TWO-PLAYFIELDS-CAPACITY,
// COLOUR-REGISTER-RANGES, CODE-ZERO-TRANSPARENT, PF2PRI-PRIORITY,
// TRANSPARENCY-FALLTHROUGH, ODD-EVEN-GROUPING, FIG313-ROW-COUNTS,
// DBLPF-ACTIVATION, MSB-BIT-IDENTITY and PF2-LSB-BIT-IDENTITY below is
// built from this rule and from nothing else. If denise_playfields.v
// implements a different combination (a different reset priority, a
// different transparent-code value, no fall-through), the mismatch is a
// finding, not a mistake in the harness.
//
// PAGE 70'S SELF-CONTRADICTION (the one thing specific to this package,
// per the build brief): the prose says playfield 1's colour index comes
// from planes 1, 3 and 5, with plane 5 the most significant bit -- then,
// mid-sentence, names "PLANE 0" as the plane with the lowest significance.
// Table 3-10's three-bit mapping agrees with the first reading (planes 1,
// 3, 5; no plane 0 belongs to playfield 1 at all). This is not resolved
// here. No claim in this package's 98-record inventory asserts which plane
// supplies playfield 1's least-significant bit -- the extraction readers
// captured the undisputed most-significant fact (3-dual.c23, quoting the
// PLANE-5 sentence) but never split out a "PLANE 0 is playfield 1's LSB"
// claim, so there is nothing here that requires picking a side. MSB-BIT-
// IDENTITY below asserts only the undisputed half (plane 5 = playfield 1's
// MSB, plane 6 = playfield 2's MSB); a MEASURED note right after it records
// what denise_playfields.v actually does with planes 1 and 3 in isolation,
// without asserting either reading as correct.
//
// CLAIM COVERAGE: 88 of the 98 records in claims.jsonl are cited below, in
// full, inside an expect() label; 10 are declined (see DECLINED CLAIMS).
// Ids restating the identical proposition under a different manual
// wording, page or reader are grouped and cited together on the single
// assertion (or single shared-mechanism loop) that tests that proposition
// -- the convention hrm/ch3-collision-registers/ and
// hrm/ch3-ham-control-field/ both document and use.
//
//   TWO-PLAYFIELDS-CAPACITY (6 ids)     -- two playfields, each up to 3 planes.
//   COLOUR-REGISTER-RANGES (8 ids)      -- 0-7 / 8-15, disjoint sets.
//   CODE-ZERO-TRANSPARENT (8 ids)       -- code 0 is transparent, per playfield.
//   PF2PRI-PRIORITY (22 ids)            -- BPLCON2 bit 6 decides front/back.
//   TRANSPARENCY-FALLTHROUGH (6 ids)    -- transparent reveals what is behind.
//   ODD-EVEN-GROUPING (12 ids, loop x6) -- each plane's playfield membership.
//   FIG313-ROW-COUNTS (14 ids, loop x7) -- Figure 3-13's per-row split.
//   DBLPF-ACTIVATION (6 ids)            -- BPLCON0 bit 10 enables the mode.
//   MSB-BIT-IDENTITY (4 ids)            -- the undisputed high bit, each side.
//   PF2-LSB-BIT-IDENTITY (2 ids)        -- playfield 2's undisputed low bit.
//   6+8+8+22+6+12+14+6+4+2 = 88.
//
// DECLINED CLAIMS (10 ids, not observable at this module's ports):
//   3-dual-mode.c2 (p.67) -- "You can also move the two playfields
//     independently." Bitplane scrolling/positioning; this module has no
//     scroll/position port (aga, bpldata, dblpf, pf2of, bplcon2 in;
//     nplayfield, plfdata out only). That capability is
//     ch3-bplcon1-scroll-delay's boundary, not this one.
//   3-dual.c46 (p.72) -- "...the way hardware can move the bitplanes on
//     the screen." Same reasoning as 3-dual-mode.c2.
//   3-dual-planes.c4, 3-dual-planes.c5, 3-dual-planes.c6 (p.68) and
//   3-dual.c18, 3-dual.c19, 3-dual.c20 (p.68) --
//     the high-resolution two-bitplane-per-playfield cap. Resolution-
//     dependent plane-count restriction is bitplane-DMA/BPLCON0 policy
//     (which BPLxDAT registers get fetched at all); this module has no
//     resolution port and composes whatever bpldata it is handed,
//     regardless of how many planes the system chose to present.
//   3-bplcon0.c20 (p.48) -- "Sixth bitplane is used only in dual-playfield
//     mode and in hold-and-modify mode." A DMA/mode-enable policy question
//     (how many BPLxDAT registers a non-dual, non-HAM display fetches),
//     not something this pure per-pixel combinational module enforces --
//     it accepts bit 5 (plane 6) whenever it is presented.
//   3-dual-color.c24 (p.71) -- transparent playfield reveals "a sprite".
//     No sprite port exists on this module; sprite compositing is
//     ch3-sprite-playfield-priority's boundary. (The same quote's other
//     two named cases -- the other playfield, the background colour -- are
//     tested by TRANSPARENCY-FALLTHROUGH via 3-dual.c31, with the sprite
//     portion of that compound claim explicitly not covered; see that
//     assertion's own label.)
//
// CONTROL CASE: CONTROL-KNOWN-VALUES-ARRIVE (no claim id, no manual page)
// is the control every page-cited assertion below shares: the fixture is
// shown to deliver two distinct, correctly-positioned, known plfdata
// values through the same eval() path used everywhere else, before
// anything below trusts it. It carries no claim id and is prefixed
// "[control]" so the mutation gate does not require a mutation record for
// it -- the convention hrm/ch3-collision-registers/'s
// CONTROL-KNOWN-VALUES-ARRIVE and hrm/ch3-ham-control-field/'s equivalent
// both use.
//
// RESULTS (after opening the RTL and building): 22 passed, 0 failed, plus
// the 1 MEASURED note (printed, not counted as pass or fail).
// denise_playfields.v turned out to implement the manual's dual-playfield
// description exactly as every assertion above expected -- no finding in
// this package. Two points worth recording precisely because they were
// checked, not assumed:
//   - bpldata is declared `[8:1]` (Verilator packs the declared LSB, index
//     1, to C++ bit 0, exactly as assumed from the sibling harness -- see
//     ORDER OF WORK). The RTL's own playfield-1/2 code concatenations
//     additionally reference bpldata[7] and bpldata[8] (AGA-only
//     extension planes, ll.78/74); this package holds those bits at 0
//     throughout (aga=0, never set), so they never influence a result
//     here, and no claim in this inventory concerns them.
//   - Playfield 2's colour-register offset (+8) is not computed by a
//     separate adder -- denise_playfields.v:76 hardwires it as a constant
//     `1` at bit 3 of a 4-bit concatenation (`{1'b1,bpldata[6],bpldata[4],
//     bpldata[2]}`), which only executes when nplayfield[2] is already
//     known nonzero (l.50/72/86). This is exactly the "8 + playfield 2's
//     own code" arithmetic COLOUR-REGISTER-RANGES, PF2PRI-PRIORITY,
//     MSB-BIT-IDENTITY and PF2-LSB-BIT-IDENTITY expected, reached by a
//     different circuit shape than a literal adder -- not a discrepancy,
//     just how the multiplexer is built.
//   - PF1-LSB-DISPUTED's measurement (plane 1 -> code 1, plane 3 -> code
//     2, at denise_playfields.v:78's bit positions bpldata[1]->bit0,
//     bpldata[3]->bit1) shows the RTL resolves page 70's self-
//     contradiction the same way the "PLANE 0" phrase would read if it
//     were a typo for "PLANE 1" -- plane 1 as playfield 1's LSB, matching
//     the natural reading of Table 3-10's ordering. This is reported for
//     traceability only; the note does not assert it as conformant, since
//     no claim in this package's inventory makes that assertion either.
//
// One build-only fix was needed, and it was in the harness, not the
// oracle: none. The Harness written in the spec-only commit compiled and
// linked against Vdenise_playfields.h without modification -- PACKAGE.md's
// port list (aga, bpldata, dblpf, pf2of, bplcon2, plfdata) matched the
// actual module header exactly, and the eval()-only usage needed no clock
// or reset handling, confirming the sibling harness's pattern transferred
// cleanly. No expected value anywhere in this file was changed after the
// RTL was opened.
//
// MUTATION LOG (every line below was actually built with its own flag
// defined to 1, e.g. `-CFLAGS -DMUT_TWOPF=1`, confirmed to turn only its
// own assertion red -- 21 passed/1 failed in every case, the other 21
// assertions unaffected -- then rebuilt with no flags and reconfirmed the
// clean 22-passed/0-failed baseline):
//
// MUTATION CONTROL-KNOWN-VALUES-ARRIVE stimulus red: the second reading's own bpldata stimulus was changed from PF2_ALL (playfield 2's three planes) to PF1_ALL (playfield 1's three planes), so it collapsed onto the first reading's value (7) instead of producing its own distinct known value (15) -- confirmed 21 passed/1 failed with only CONTROL-KNOWN-VALUES-ARRIVE red, then reverted, restoring the clean 22-passed/0-failed baseline.
// MUTATION TWO-PLAYFIELDS-CAPACITY stimulus red: playfield 1's "all three planes" stimulus was changed to drop plane 5, so playfield 1's code can no longer reach its 3-bitplane maximum of 7.
// MUTATION COLOUR-REGISTER-RANGES stimulus red: playfield 2's maximum-code stimulus was changed to drop plane 6, so playfield 2's code can no longer reach register 15.
// MUTATION CODE-ZERO-TRANSPARENT stimulus red: playfield 1's "fully transparent" stimulus was changed to leave a stray plane-1 bit on, so playfield 1 is no longer transparent and its own code shows instead of falling through to playfield 2's.
// MUTATION PF2PRI-PRIORITY stimulus red: the PF2-priority read's BPLCON2 value was changed to the same all-clear value as the PF1-priority read, so the two reads stopped differing.
// MUTATION TRANSPARENCY-FALLTHROUGH stimulus red: playfield 1's stimulus in the fall-through case was changed to all-off, so there is nothing left behind playfield 2 to fall through to.
// MUTATION ODD-EVEN-GROUPING stimulus red: plane 3's own loop case was changed to present plane 2's bit instead of plane 3's own, so the case now measures an even plane while still being labelled and checked as playfield 1's odd plane 3.
// MUTATION FIG313-ROW-COUNTS stimulus red: row 1's single active-plane stimulus was changed from plane 1 to plane 2, so that row now shows playfield 2 present and playfield 1 empty instead of the reverse.
// MUTATION DBLPF-ACTIVATION stimulus red: the dblpf=1 stimulus was changed from playfield 2's plane 2 to playfield 1's plane 1, so the dual-mode read no longer lands on the playfield-2 8-15 split value (9) the assertion checks for.
// MUTATION MSB-BIT-IDENTITY stimulus red: playfield 1's plane-5-alone stimulus was changed to also present plane 1, so the reading is no longer the clean, isolated MSB value of 4.
// MUTATION PF2-LSB-BIT-IDENTITY stimulus red: playfield 2's plane-2-alone stimulus was changed to also present plane 4, so the reading is no longer the clean, isolated LSB value of 9.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include "Vdenise_playfields.h"
#include "verilated.h"

#ifndef MUT_TWOPF
#define MUT_TWOPF 0
#endif
#ifndef MUT_RANGES
#define MUT_RANGES 0
#endif
#ifndef MUT_ZEROTRANS
#define MUT_ZEROTRANS 0
#endif
#ifndef MUT_PRI
#define MUT_PRI 0
#endif
#ifndef MUT_FALLTHROUGH
#define MUT_FALLTHROUGH 0
#endif
#ifndef MUT_GROUPING
#define MUT_GROUPING 0
#endif
#ifndef MUT_FIGROWS
#define MUT_FIGROWS 0
#endif
#ifndef MUT_ACTIVATE
#define MUT_ACTIVATE 0
#endif
#ifndef MUT_MSB
#define MUT_MSB 0
#endif
#ifndef MUT_LSB
#define MUT_LSB 0
#endif
#ifndef MUT_CONTROL
#define MUT_CONTROL 0
#endif

namespace {

// bpldata bit (n-1) is bitplane n's live data bit, n=1..8 -- carried from
// hrm/denise_playfields/test_denise_playfields.cpp's own stimulus usage
// (its Test 1 sets bpldata=1 and calls that "bpldata[1]=1"; its Test 2
// sets bpldata=3 and calls that "bpldata[1]=1, bpldata[2]=1"), not learned
// by opening this package's own file.
constexpr uint8_t PLANE(int n) { return static_cast<uint8_t>(1u << (n - 1)); }

constexpr uint8_t PF1_ALL = PLANE(1) | PLANE(3) | PLANE(5);  // all three of playfield 1's planes
constexpr uint8_t PF2_ALL = PLANE(2) | PLANE(4) | PLANE(6);  // all three of playfield 2's planes
constexpr uint8_t PF2PRI_BIT = 0x40;  // BPLCON2 bit 6 (p.71/90/271)

class Harness {
public:
    Harness() : dut(new Vdenise_playfields) {
        // aga and pf2of are non-goals in this package (see header) -- set
        // once here and never touched again.
        dut->aga = 0;
        dut->pf2of = 0;
        dut->bpldata = 0;
        dut->dblpf = 0;
        dut->bplcon2 = 0;
        dut->eval();
    }
    ~Harness() { delete dut; }

    // Presents bpldata/dblpf/bplcon2 for one combinational evaluation
    // (denise_playfields has no clock -- see BOUNDARY) and returns plfdata.
    uint8_t sample(uint8_t bpldata_v, bool dblpf_v, uint8_t bplcon2_v) {
        dut->bpldata = bpldata_v;
        dut->dblpf = dblpf_v ? 1 : 0;
        dut->bplcon2 = bplcon2_v;
        dut->eval();
        return dut->plfdata;
    }

    Vdenise_playfields* dut;
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =========================================================================
    // CONTROL -- the fixture can deliver a known value before anything below
    // is trusted to read one back (package brief, "the fixture proves it can
    // deliver a known value before anything reads one").
    // =========================================================================
    {
        Harness h;
        uint8_t v2_stim = PF2_ALL;
#if MUT_CONTROL
        v2_stim = PF1_ALL;  // second reading driven with playfield 1's data instead of playfield 2's
#endif
        uint8_t v1 = h.sample(PF1_ALL, true, 0x00);         // PF1 priority, PF1 fully opaque
        uint8_t v2 = h.sample(v2_stim, true, PF2PRI_BIT);   // PF2 priority, PF2 fully opaque
        checks.expect(
            v1 == 7 && v2 == 15 && v1 != v2,
            "[control] CONTROL-KNOWN-VALUES-ARRIVE: two distinct, known "
            "dual-playfield stimuli (all three playfield-1 planes active "
            "under PF1 priority; all three playfield-2 planes active under "
            "PF2 priority) produce two distinct, correctly-positioned "
            "plfdata readings (7, then 15) through the fixture's own "
            "eval() path before any assertion below trusts it. No manual "
            "page: the control for every assertion in this file, on the "
            "same measurement basis.");
    }

    // =========================================================================
    // TWO-PLAYFIELDS-CAPACITY (p.44/67/68) -- two independent playfields,
    // each formed from up to three bitplanes.
    // =========================================================================
    {
        Harness h;
        uint8_t pf1_stim = PLANE(1) | PLANE(3) | PLANE(5);
#if MUT_TWOPF
        pf1_stim = PLANE(1) | PLANE(3);  // drop plane 5: PF1 can no longer reach its 3-plane maximum
#endif
        uint8_t both = pf1_stim | PF2_ALL;
        uint8_t out_pf1 = h.sample(both, true, 0x00);
        uint8_t out_pf2 = h.sample(both, true, PF2PRI_BIT);
        checks.expect(
            out_pf1 == 7 && out_pf2 == 15,
            "TWO-PLAYFIELDS-CAPACITY (p.44/67/68): with all three of "
            "playfield 1's planes (1,3,5) and all three of playfield 2's "
            "planes (2,4,6) active at once, playfield 1's own code still "
            "reaches its 3-bitplane maximum (7, read under PF1 priority) "
            "and playfield 2's reaches its own maximum independently (15, "
            "read under PF2 priority) -- 'two separate playfields, each "
            "formed from up to three bitplanes' (p.44), 'Each playfield in "
            "a dual display is formed from one, two or three bitplanes' "
            "(p.67), Figure 3-12's '(1, 2 or 3 bitplanes)' caption (p.68). "
            "3-dual.c02, 3-dual.c1, 3-dual.c2, 3-dual-mode.c3, 3-dual.c13, "
            "3-dual.f02.");
    }

    // =========================================================================
    // COLOUR-REGISTER-RANGES (p.44/67/72) -- playfield 1 stays inside
    // registers 0-7, playfield 2 inside 8-15: separate, disjoint sets.
    // =========================================================================
    {
        Harness h;
        uint8_t pf2_max_stim = PF2_ALL;
#if MUT_RANGES
        pf2_max_stim = PLANE(2) | PLANE(4);  // drop plane 6: PF2 can no longer reach register 15
#endif
        uint8_t pf1_max = h.sample(PF1_ALL, true, 0x00);
        uint8_t pf2_max = h.sample(pf2_max_stim, true, PF2PRI_BIT);
        checks.expect(
            pf1_max == 7 && pf2_max == 15 && pf1_max < 8 && pf2_max >= 8,
            "COLOUR-REGISTER-RANGES (p.44/67/72): playfield 1's maximum "
            "code (7) stays inside colour registers 0-7 and playfield 2's "
            "maximum (15) stays inside 8-15 -- disjoint sets -- 'Color "
            "registers 0 through 7 are assigned to playfield 1... Color "
            "registers 8 through 15 are assigned to playfield 2' (p.67), "
            "'Each of the two playfields uses a separate set of eight "
            "different colors' (p.44), 'colors in each playfield...are "
            "taken from different sets of color registers' (p.67), 'color "
            "registers 0-7...used by playfield 1...registers 8 through 15 "
            "are used by playfield 2' (p.72). 3-dual.c3, 3-dual-mode.c10, "
            "3-dual-mode.c11, 3-dual-mode.c6, 3-dual.c14, 3-dual.c15, "
            "3-dual.c47, 3-dual.c48.");
    }

    // =========================================================================
    // CODE-ZERO-TRANSPARENT (p.67) -- code 0 is transparent in each
    // playfield, not an opaque colour, even when that playfield has
    // priority.
    // =========================================================================
    {
        Harness h;
        uint8_t pf1_off_stim = 0;
#if MUT_ZEROTRANS
        pf1_off_stim = PLANE(1);  // PF1 is no longer fully transparent: stray plane-1 bit left on
#endif
        uint8_t case_a = h.sample(static_cast<uint8_t>(pf1_off_stim | PF2_ALL), true, 0x00);
        uint8_t case_b = h.sample(PF1_ALL, true, PF2PRI_BIT);
        checks.expect(
            case_a == 15 && case_b == 7,
            "CODE-ZERO-TRANSPARENT (p.67): playfield 1's code 0 does not "
            "behave as an opaque colour -- with PF1 given priority but "
            "held fully transparent (all its planes 0) while playfield 2 "
            "is opaque, playfield 2's code shows through unchanged (15); "
            "playfield 2's code 0 behaves the same way with the roles "
            "reversed (7) -- 'one of the colors in each playfield is "
            "transparent (color 0 in playfield 1 and color 8 in playfield "
            "2)' (p.67), 'up to seven [colours] plus transparent' per "
            "playfield (p.67). 3-dual-mode.c4, 3-dual-mode.c5, "
            "3-dual-mode.c7, 3-dual-mode.c8, 3-dual.c06, 3-dual.c09, "
            "3-dual.c10, 3-dual.c11.");
    }

    // =========================================================================
    // PF2PRI-PRIORITY (p.67/71/72/90/271) -- BPLCON2 bit 6 decides which
    // opaque playfield is drawn in front when both are opaque at once.
    // =========================================================================
    {
        Harness h;
        uint8_t both = PF1_ALL | PF2_ALL;
        uint8_t pf2pri_bit = PF2PRI_BIT;
#if MUT_PRI
        pf2pri_bit = 0x00;  // the PF2-priority request is silently dropped: same bplcon2 as the PF1 read
#endif
        uint8_t out_pri0 = h.sample(both, true, 0x00);
        uint8_t out_pri1 = h.sample(both, true, pf2pri_bit);
        checks.expect(
            out_pri0 == 7 && out_pri1 == 15,
            "PF2PRI-PRIORITY (p.67/71/72/90/271): with both playfields "
            "simultaneously opaque, BPLCON2 bit 6 (PF2PRI) decides which "
            "one wins -- clear (0), the reset/'normal' value, gives "
            "playfield 1 priority (7); set (1) gives playfield 2 priority "
            "(15) -- 'Playfield 1 normally has priority... When PF2PRI=1, "
            "playfield 2 has priority over playfield 1. When PF2PRI=0, "
            "playfield 1 has priority' (p.71), 'Bit 6 - PF2PRI... 1 = "
            "Playfield 2 has priority... 0 = Playfield 1 has priority' "
            "(p.90), Appendix A's bit table and legend (p.271). "
            "3-dual-mode.c1, 3-dual.c04, 3-pri.c02, 3-pri.c03, 3-pri.c04, "
            "3-pri.c05, 3-priority.c1, 3-priority.c2, 3-priority.c3, "
            "3-priority.c4, 3-priority.c5, 3-pri.c06, 3-r2.c01, 3-r2.c02, "
            "3-r2.c03, 3-regsum-bplcon2.c1, 3-regsum-bplcon2.c2, "
            "A-bpc.b06.c2, A-bplcon.c42, A-bplcon.c79, A-bplcon.c80, "
            "A-leg.pf2pri, sol:7-video.c15, sol:7-video.c16, sol:7-video.c17, sol:7-video.c19, sol:7-video.c29, sol:7-appA-bplcon2.c7, sol:7-appA-bplcon2.c10, opus:7-vp.c13, opus:7-vp.c14, opus:7-vp.c15, opus:7-vp.c18, opus:7-vp.c31, opus:7-appA-bplcon2.c14, opus:7-appA-bplcon2.c21, opus:7-fig7-2.c12.");
    }

    // =========================================================================
    // TRANSPARENCY-FALLTHROUGH (p.67/68/71) -- a transparent playfield
    // reveals whatever lies behind it: the other playfield, or (when both
    // are transparent) the background colour register.
    // =========================================================================
    {
        Harness h;
        uint8_t pf1_stim = PF1_ALL;
#if MUT_FALLTHROUGH
        pf1_stim = 0;  // PF1 is left transparent too: nothing left to fall through to
#endif
        uint8_t fallthrough_case = h.sample(pf1_stim, true, PF2PRI_BIT);  // PF2 priority, PF2 transparent
        uint8_t both_transparent_case = h.sample(0, true, 0x00);          // both playfields transparent
        checks.expect(
            fallthrough_case == 7 && both_transparent_case == 0,
            "TRANSPARENCY-FALLTHROUGH (p.67/68/71): a transparent "
            "playfield reveals whatever is behind it -- here, the other "
            "playfield (7: PF2 given priority but held transparent falls "
            "through to PF1) or, when both are transparent, colour "
            "register 0, the standard background register (0) -- 'You can "
            "use transparency to allow selected features of the "
            "background playfield to show through' (p.67), Figure 3-12's "
            "transparent-region caption (p.68), 'Combination 000 selects "
            "transparent mode, to show the color of whatever object (the "
            "other playfield...or the background color) may be behind the "
            "playfield' (p.71). 3-dual-mode.c9, 3-dual.f05, "
            "3-dual-color.c23, 3-dual-color.c25, 3-dual.c31, "
            "3-dual-mode.c14. (3-dual.c31's third named case -- a sprite "
            "behind a transparent playfield -- is outside "
            "denise_playfields' ports; there is no sprite input here, and "
            "this assertion does not test it; see 3-dual-color.c24 among "
            "the declined claims.)");
    }

    // =========================================================================
    // ODD-EVEN-GROUPING (p.68/70/72/271) -- each bitplane belongs to
    // exactly one playfield, by parity: odd planes (1,3,5) to playfield 1,
    // even planes (2,4,6) to playfield 2.
    // =========================================================================
    {
        struct PlaneCase { int plane; bool odd; const char* name; };
        const PlaneCase cases[6] = {
            {1, true, "plane 1"}, {2, false, "plane 2"}, {3, true, "plane 3"},
            {4, false, "plane 4"}, {5, true, "plane 5"}, {6, false, "plane 6"},
        };
        for (const auto& c : cases) {
            Harness h;
            uint8_t stim = PLANE(c.plane);
#if MUT_GROUPING
            if (c.plane == 3) stim = PLANE(2);  // plane 3's own case presents plane 2's bit instead
#endif
            uint8_t via_pf1 = h.sample(stim, true, 0x00);        // PF1 priority
            uint8_t via_pf2 = h.sample(stim, true, PF2PRI_BIT);  // PF2 priority
            bool in_pf1 = (via_pf2 > 0 && via_pf2 < 8);  // PF2 transparent -> fell through to PF1
            bool in_pf2 = (via_pf1 >= 8);                 // PF1 transparent -> fell through to PF2
            bool ok = c.odd ? (in_pf1 && !in_pf2) : (in_pf2 && !in_pf1);
            checks.expect(
                ok,
                std::string("ODD-EVEN-GROUPING (p.68/70/72/271): with "
                "only ") + c.name + " active, it registers in playfield " +
                (c.odd ? "1 (odd planes)" : "2 (even planes)") +
                " and nowhere else -- 'the three odd-numbered bitplanes "
                "(1, 3, and 5) are grouped together...and may be used in "
                "playfield 1...the three even-numbered bitplanes (2, 4, "
                "and 6)...may be used in playfield 2' (p.68), 'the "
                "hardware interprets color numbers for playfield 1 "
                "from...bitplanes 1, 3, and 5' / '...for playfield 2 "
                "from...bitplanes 2, 4, and 6' (p.70), 'all odd-numbered "
                "bitplanes are grouped together and all even-numbered "
                "bitplanes are grouped together' (p.72), 'DBLPF=Double "
                "playfield (PF1=odd PF2=even bitplanes)' (p.271). "
                "3-dual-planes.c1, 3-dual-planes.c2, 3-dual-planes.c3, "
                "3-dual.c16, 3-dual.c17, 3-dual.c44, 3-dual.c45, "
                "A-leg.dblpf, 3-dual-color.c1, 3-dual-color.c12, "
                "3-dual.c49, 3-dual.c50, opus:7-appA-bplcon2.c24.");
        }
    }

    // =========================================================================
    // FIG313-ROW-COUNTS (p.69) -- Figure 3-13's rows: with the lowest N
    // consecutive bitplanes active (N=0..6), playfield 1 is present iff
    // N>=1 and playfield 2 is present iff N>=2 (each row's own plane
    // membership is exactly the odd/even split ODD-EVEN-GROUPING already
    // measures; presence, not the disputed exact pf1 code value, is what
    // is checked here -- see the page-70 note on playfield 1's LSB).
    // =========================================================================
    {
        const char* row_ids[7] = {
            "3-dual-planes.c7, 3-dual-planes.c8",
            "3-dual-planes.c9, 3-dual-planes.c10",
            "3-dual-planes.c11, 3-dual-planes.c12",
            "3-dual-planes.c13, 3-dual-planes.c14",
            "3-dual-planes.c15, 3-dual-planes.c16",
            "3-dual-planes.c17, 3-dual-planes.c18",
            "3-dual-planes.c19, 3-dual-planes.c20",
        };
        for (int n = 0; n <= 6; ++n) {
            Harness h;
            uint8_t stim = 0;
            for (int k = 1; k <= n; ++k) stim = static_cast<uint8_t>(stim | PLANE(k));
#if MUT_FIGROWS
            if (n == 1) stim = PLANE(2);  // row 1's single active plane is presented as plane 2 instead of plane 1
#endif
            uint8_t via_pf1 = h.sample(stim, true, 0x00);
            uint8_t via_pf2 = h.sample(stim, true, PF2PRI_BIT);
            bool pf1_present = (via_pf1 >= 1 && via_pf1 <= 7);
            bool pf2_present = (via_pf2 >= 9);
            bool expect_pf1 = (n >= 1);
            bool expect_pf2 = (n >= 2);
            checks.expect(
                pf1_present == expect_pf1 && pf2_present == expect_pf2,
                "FIG313-ROW-COUNTS (p.69): Figure 3-13's row for " +
                std::to_string(n) + " active bitplane(s) (the lowest " +
                std::to_string(n) + " consecutively) shows playfield 1 " +
                std::string(expect_pf1 ? "present" : "empty") +
                " and playfield 2 " +
                std::string(expect_pf2 ? "present" : "empty") + ". " +
                row_ids[n] + ".");
        }
    }

    // =========================================================================
    // DBLPF-ACTIVATION (p.67/72/90) -- BPLCON0 bit 10 (DBLPF) is what turns
    // the dual-playfield colour-register split on.
    // =========================================================================
    {
        Harness h;
        uint8_t stim = PLANE(2);  // playfield 2's own LSB plane (undisputed mapping, see PF2-LSB-BIT-IDENTITY)
#if MUT_ACTIVATE
        stim = PLANE(1);  // presents PF1's plane instead of PF2's, under dblpf=1
#endif
        uint8_t out_dblpf1 = h.sample(stim, true, PF2PRI_BIT);
        uint8_t out_dblpf0 = h.sample(stim, false, PF2PRI_BIT);
        checks.expect(
            out_dblpf1 == 9 && out_dblpf0 != 9,
            "DBLPF-ACTIVATION (p.67/72/90): the same bitplane data (plane "
            "2 alone) produces the dual-playfield 8-15 split (9 = "
            "register 8+1) only when BPLCON0 bit 10 (DBLPF) is set; "
            "clearing it changes the result away from that dual-mode-"
            "specific value -- 'Writing a 1 to bit 10 (called DBLPF) of "
            "the bitplane control register BPLCON0 selects dual-playfield "
            "mode' (p.72), 'You must set a bit to activate dual-playfield "
            "mode' (p.67), '1 = dual playfields enabled' (p.90). "
            "3-dual-activate.c1, 3-dual.c43, 3-dual.c52, 3-dual.c08, "
            "3-r0.c19, 3-regsum-bplcon0.c14.");
    }

    // =========================================================================
    // MSB-BIT-IDENTITY (p.70) -- the undisputed high bit of each
    // playfield's 3-bit code: plane 5 for playfield 1, plane 6 for
    // playfield 2.
    // =========================================================================
    {
        Harness h;
        uint8_t pf1_stim = PLANE(5);
#if MUT_MSB
        pf1_stim = static_cast<uint8_t>(PLANE(5) | PLANE(1));  // an extra, uninvolved plane is left on alongside the MSB plane
#endif
        uint8_t pf1_msb = h.sample(pf1_stim, true, 0x00);
        uint8_t pf2_msb = h.sample(PLANE(6), true, PF2PRI_BIT);
        checks.expect(
            pf1_msb == 4 && pf2_msb == 12,
            "MSB-BIT-IDENTITY (p.70): playfield 1's plane 5, alone, "
            "produces code 4 -- the most-significant bit of a 3-bit code "
            "-- and playfield 2's plane 6, alone, does the same for its "
            "own code (register 12 = 8+4) -- 'Bits from PLANE 5 have the "
            "highest significance and form the most significant digit of "
            "the color register number' (playfield 1) and 'Bits from "
            "PLANE 6 have the highest significance' (playfield 2), both "
            "p.70. This is the undisputed half of page 70's plane "
            "numbering -- see the MEASURED note below for the disputed "
            "half (playfield 1's least-significant plane), which this "
            "package does not assert either way. 3-dual-color.c2, "
            "3-dual-color.c13, 3-dual.c23, 3-dual.c27.");
    }

    // =========================================================================
    // PF2-LSB-BIT-IDENTITY (p.70) -- playfield 2's undisputed low bit:
    // plane 2.
    // =========================================================================
    {
        Harness h;
        uint8_t pf2_stim = PLANE(2);
#if MUT_LSB
        pf2_stim = static_cast<uint8_t>(PLANE(2) | PLANE(4));  // an extra, uninvolved plane (PF2's own middle plane) is left on
#endif
        uint8_t pf2_lsb = h.sample(pf2_stim, true, PF2PRI_BIT);
        checks.expect(
            pf2_lsb == 9,
            "PF2-LSB-BIT-IDENTITY (p.70): playfield 2's plane 2, alone, "
            "produces code 1 -- the least-significant bit (register 9 = "
            "8+1) -- 'Bits from PLANE 2 have the lowest significance' "
            "(p.70). Unlike playfield 1 (see MSB-BIT-IDENTITY's note on "
            "the disputed page-70 text), playfield 2's least-significant "
            "plane is not in dispute anywhere in this package's "
            "inventory. 3-dual-color.c14, 3-dual.c28.");
    }

    // =========================================================================
    // PF1-LSB-DISPUTED -- no claim id, not asserted either way. See "PAGE
    // 70'S SELF-CONTRADICTION" in the header.
    // =========================================================================
    {
        Harness h;
        uint8_t via_plane1 = h.sample(PLANE(1), true, 0x00);
        uint8_t via_plane3 = h.sample(PLANE(3), true, 0x00);
        checks.note(
            "PF1-LSB-DISPUTED (p.70, no claim id -- not asserted either "
            "way): printed page 70 contradicts itself on playfield 1's "
            "least-significant colour-index bit. The prose reads "
            "'...Bits from PLANE 5 have the highest significance and form "
            "the most significant digit of the color register number. "
            "Bits from PLANE 0 have the lowest [significance]...' (quoted "
            "in full, including the disputed clause, by 3-dual.c23) -- "
            "but playfield 1 is elsewhere said, in the very same "
            "paragraph and in Table 3-10, to be formed from planes 1, 3 "
            "and 5 only; there is no 'plane 0' in that grouping. Table "
            "3-10's three-bit mapping agrees with the 1/3/5 reading, not "
            "with 'PLANE 0'. No claim in this package's 98-record "
            "inventory names a specific plane as playfield 1's "
            "least-significant bit -- only the undisputed most-"
            "significant fact (plane 5, MSB-BIT-IDENTITY) was extracted "
            "as a claim. This package does not pick a side. Measured "
            "only, for traceability: plane 1 alone reads as code " +
            std::to_string(static_cast<int>(via_plane1)) + " (register " +
            std::to_string(static_cast<int>(via_plane1)) + "), plane 3 "
            "alone reads as code " +
            std::to_string(static_cast<int>(via_plane3)) + " (register " +
            std::to_string(static_cast<int>(via_plane3)) +
            ") under denise_playfields.v as built -- neither value is "
            "asserted as conformant or nonconformant to either reading "
            "of page 70.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
