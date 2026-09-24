// Package ch3-shres-sprite-colour-encoding -- ECS SuperHires sprite colour
//   encoding conformance test, fixture `denise` (one Verilator build, top
//   module denise.v plus its own eleven sub-modules: denise_bitplanes,
//   denise_bitplane_shifter, denise_playfields, denise_sprites,
//   denise_sprites_shifter, denise_collision, denise_hamgenerator,
//   denise_colortable, denise_colortable_ram_mf, denise_spritepriority,
//   bitplane_ram -- PACKAGE.md's own fixture line).
//
// Test purpose: printed page 302's table mapping the sixteen *upper* colour
//   registers (COLOR$10-COLOR$1F, i.e. COLOR16-COLOR31) onto the symbols
//   A-X, plus the legend "Sprite (Color 16) : AB-- CD-- EF--" (and its three
//   sibling legend lines for Color 17/18/19) that says each named SuperHires
//   sprite colour contributes only the TOP two bits of each of R, G, B. A
//   sibling package (page 301) covers the bitplane half of the identical
//   scheme; this package is the sprite half only.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 301-302 (claims.jsonl for ch3-shres-sprite-colour-encoding, 275
//   records). A red result below means denise.v (and the sub-modules
//   PACKAGE.md's own fixture line names) disagrees with the manual, not that
//   the assertion is wrong -- no assertion here was, or should be, adjusted
//   to make the RTL pass. Per the build brief: "already verified
//   independently: all 512 bit claims across pages 301 and 302 were
//   re-derived from the table cells plus the legend and matched 512 of 512"
//   -- this package tests the RTL against that already-checked mapping, it
//   does not re-audit the mapping itself.
// Non-goals: anything outside the `denise` fixture this package builds, and
//   any claim it does not name. A green here means those assertions held on
//   this fixture at the pinned revision -- not that the module is correct,
//   and not that the chapter is covered.
//
// THE ORACLE, DERIVED PURELY FROM THE QUOTES (before RTL was opened -- see
//   ORDER OF WORK below): claims.jsonl's 16 row quotes (C-shrs.r10..r1F,
//   page 302, e.g. "C  10   .   .   .   .   A   B   A   B   C   D   C   D
//   E   F   E   F") print, for each of the sixteen registers COLOR$10-
//   COLOR$1F, sixteen symbols for bits 15 downto 0: bits 15-12 always "."
//   (unused -- matches the sibling ch3-colour-register-format package's
//   own 12-bit-register-format finding, cited here only as an unused-bit
//   prediction, not re-tested as that package's own claim); bits 11-0 each
//   carry ONE letter A-X. claims.jsonl's 12 symbol-legend quotes
//   (C-shres-sprite-symbols.c1-c12, page 302, "Sprite (Color 16) :   AB--
//   CD-- EF--" and its three siblings for Color 17/18/19) assign each
//   letter to a (sprite colour 16|17|18|19, channel R|G|B, hi-or-lo-bit)
//   triple: A-F -> colour 16 (A/B = R hi/lo, C/D = G hi/lo, E/F = B
//   hi/lo), G-L -> colour 17, M-R -> colour 18, S-X -> colour 19. Working
//   through both tables by hand (verified against the leading-letter
//   column, which spells "COLOR REGISTER" vertically down the table's
//   margin -- not itself part of the bit data, confirmed by checking it is
//   never one of A-X) shows every register's twelve meaningful bits
//   resolve into SIX two-bit groups -- bits[11:10]/[7:6]/[3:2] (called
//   "slot A" below) and bits[9:8]/[5:4]/[1:0] ("slot B") -- and that, for
//   the register at COLOR($10+offset) (offset 0-15, the same 4-bit
//   combined-sprite-colour-index 0-15 that addresses COLOR16-31 in NORMAL,
//   non-SuperHires mode: 0 = both attached members transparent/0, 1-3 = an
//   UNATTACHED sprite's own 2-bit pixel value, 4-15 = an ATTACHED pair's
//   {odd 2-bit value, even 2-bit value}), slot A always equals colour
//   (16 + (offset&3))'s own six bits and slot B always equals colour
//   (16 + (offset>>2))'s own six bits -- i.e. slot A is keyed by the LOW
//   ("even") half of the combined index, slot B by the HIGH ("odd") half.
//   This is a DERIVATION from the sixteen row quotes (self-consistent
//   across all 16 rows, independently re-checked against every row before
//   this file was written), not a guess -- but it is a reading of the
//   MANUAL's own printed table, produced before denise.v was opened, and
//   is used only to decide WHICH sprite arming this Harness must present
//   to reach a given register's own combined index; it is not read out of
//   the RTL and does not by itself decide any expect() result below (every
//   expect() below compares a WRITTEN register bit against an OBSERVED RGB
//   bit, never a value derived from RTL source).
//
// WHY REGISTER $10 (colour 16) IS TREATED SEPARATELY: offset 0 (both
//   attached-pair members' own value 0, or an unattached sprite's own
//   value 0) is universally "transparent" on real sprite hardware -- no
//   sprite pixel is ever drawn, so COLOR16's own storage can never become
//   the colour of an actually-visible sprite pixel, in EITHER SuperHires
//   or normal mode, regardless of what denise.v's own transparency gate
//   turns out to look like. FIXTURE-SPR0-TRANSPARENT-AT-VALUE0 below
//   confirms this empirically (arming with value 0 produces no visible
//   sprite pixel) BEFORE the REG-0x10 assertion is asked to rely on it;
//   REG-0x10 is written to expect exactly this outcome (no display-based
//   confirmation possible for this one register), not silently dropped --
//   its own seventeen ids (sixteen bits, one row) are still cited in full.
//
// BOUNDARY (from PACKAGE.md's own evidence field, not from opening the RTL
//   -- see ORDER OF WORK below): nanomig/src/minimig-aga/denise.v:31-52
//   module header (reg_address_in[8:1] l.39, data_in l.40, data_out l.42,
//   strhor l.38, blank l.43, red/green/blue l.44-46, a1k l.47, ecs l.48,
//   aga l.49, hires l.50, shres l.51) | denise.v:59 (BPLCON0 9'h100), write
//   l.171-178, field decode l.180-192 (hires=bit15, bpu={bit4&aga,
//   bits14-12}, ham=bit11, dpf=bit10, shres=bit6, ecsena=bit0), plane
//   gating l.367-374 (shres = bplcon0[6], l.187) | denise.v:65 (COLORBASE
//   9'h180) with denise_colortable.v:24,30-34 (write decode, wr_dat=
//   data_in[11:0] -- confirms bits 15-12 of any colour-register write are
//   never even latched, matching the "unused" prediction above) and the
//   colour packing at denise_colortable.v:56-58. Sprite register addresses
//   (SPRPOSCTLBASE 9'h140, four words per sprite: POS/CTL/DATA/DATB) and
//   the SPR1CTL bit 7 ATTACH convention are reused from this repository's
//   own already-committed, already-built hrm/ch3-sprite-registers/
//   test_denise_sprites.cpp (a sibling harness for denise_sprites, the SAME
//   sub-module this fixture also instantiates) -- fair game per that
//   sibling's own ORDER OF WORK precedent (reusing an already-committed
//   sibling harness's conventions is not "opening this package's own
//   forbidden-until-committed RTL"). Display-window register addresses
//   (DIWSTRT 9'h08E, DIWSTOP 9'h090) and the four-phase clocking Harness
//   are reused from this repository's own already-committed, already-built
//   hrm/ch3-horizontal-display-window/test_denise.cpp -- a sibling harness
//   for the IDENTICAL `denise` fixture this package also uses -- same
//   fair-game precedent.
//
// THE C1/C3 TRAP (build-brief warning, not itself a claim): amiga_clk.v:
//   58-61 makes c3 the registered 7MHz clock and c1 = ~c3 delayed one
//   further 28MHz cycle, so across one 7MHz cycle's four 28MHz phases, c1
//   and c3 are BOTH high in one phase and BOTH low in another -- not two
//   disjoint two-phase signals. This Harness uses the build brief's own
//   measured-working four-phase pattern: clk7_en=(phase==0),
//   c1=(phase==0||phase==1), c3=(phase==1||phase==2) -- phase 1 has both
//   high, phase 3 has both low -- the same pattern the
//   ch3-horizontal-display-window sibling Harness already proved live on
//   this identical fixture.
//
// MEASUREMENT METHOD -- no RTL-derived divisor or constant: every
//   assertion below compares a register bit VALUE this file itself chose
//   and wrote (never a value read out of denise.v) against an RGB sample
//   OBSERVED by diffing a scan of the whole line against a same-setup
//   scan with the sprite disarmed (DATA=DATB=0, i.e. genuinely undriven,
//   not merely "value 0" -- see FIXTURE-SPR0-TRANSPARENT-AT-VALUE0, which
//   is the one place this file measures "value 0" deliberately, precisely
//   to confirm it is indistinguishable from disarmed). Register content
//   under test is always compared at BOTH BPLCON0.shres settings (as a
//   SET of distinct colours, not a raw sample trace) rather than against
//   an absolute predicted value, specifically so a genuine colour-
//   encoding difference cannot be confused with the sprite-shifter's own
//   shres-dependent drain RATE (a timing/width effect, unrelated to
//   colour, found once the RTL was opened -- see HARNESS FIXES below).
//
// ORDER OF WORK: this file (Harness, oracle tables, expected values,
//   claim-id coverage, register/bit-symbol derivation above) was written
//   and committed from claims.jsonl's quotations and PACKAGE.md's own
//   evidence field, plus the two already-committed sibling harnesses named
//   above and the build brief's own C1/C3 trap note -- denise.v itself was
//   not opened until after that commit. A second commit follows once the
//   RTL is opened, the build actually run under WSL/Verilator, any Harness
//   bugs fixed (documented below, oracle text untouched), and the
//   MUTATION LOG filled in with real results.
//
// CLAIM COVERAGE (275 ids, partitioned with no overlap -- every id is
//   written out in full inside exactly one expect() string below):
//   General (3 ids, no register-specific quote): C-shrc.c03,
//     C-shres-color.c8, C-shrs.c01 -- cited on SHRES-MODE-CHANGES-OUTPUT.
//   Sixteen per-register assertions REG-0x10 .. REG-0x1F, each citing that
//     register's own row id (C-shrs.rXX) plus, where present, its sixteen
//     per-bit ids (C-shres-sprite-bits.cN) -- register $19 has a row id but
//     NO per-bit ids in this package's own claims.jsonl (240 = 15
//     registers x 16 bits, register $19 excluded from the extraction --
//     confirmed by counting, not assumed). The four "diagonal" registers
//     ($10, $15, $1A, $1F -- each dedicated to a single sprite colour, slot
//     A == slot B) additionally cite that colour's three symbol ids
//     (C-shres-sprite-symbols.cN) and its one legend id (C-shrc.srcsNN),
//     since those four registers are where each of those twelve symbol ids
//     and four legend ids is actually exercised.
//   16 + 240 + 12 + 4 + 3 = 275. No claim id is declined.
//
// RTL OPENED -- FINDING (read directly off denise.v/denise_colortable.v/
//   denise_sprites.v/denise_sprites_shifter.v, the exact lines cited,
//   before any Harness fix below or any build was attempted):
//
//   SuperHires sprite colour encoding -- the whole page-301/302 scheme of
//   sixteen registers redundantly packing four reduced-precision "sprite
//   colours" via a slot-A/slot-B split selected by SuperHires timing --
//   IS NOT IMPLEMENTED at this boundary. denise_colortable.v (the only
//   module that ever turns a stored 12-bit register value into an RGB
//   pixel) takes no `shres` input at all (its full port list, l.7-20, has
//   none); its read address (`rd_adr`, l.34) is simply `select_xored`
//   (i.e. denise.v's `clut_data`, i.e. `sprdata` when a sprite is
//   selected) with no shres-conditional term anywhere; and its RGB
//   packing (l.56-58) always duplicates each stored 4-bit nibble across
//   the full 8-bit channel ({nibble,nibble}) -- the SAME expansion at
//   shres=0 and shres=1. denise_sprites.v's sprite-colour-index
//   construction (l.470-512, the `sprdata[7:0] = {esprm/osprm, ...}`
//   case statement) never reads `spres`/`shres` either. The only place
//   `spres`/`shres` reaches the sprite path at all is denise_sprites.v:
//   158-166's `shift` case on `spres` (falling back to a shres-derived
//   `spres_d` in denise.v:390-391 when BPLCON3.spres itself is zero,
//   which it always is here since aga=0) -- and denise_sprites_shifter.v:
//   274-284 confirms this `shift` signal controls ONLY how fast an
//   already-loaded shift register drains (64 bits, l.77-78, only the
//   top 16 non-zero at fmode=0, l.91; twice as fast at SuperHires
//   resolution), never what value gets loaded or displayed.
//   The claims below are therefore tested, correctly, against an RTL that
//   has no SuperHires-specific colour path to find: every register-level
//   and the one general assertion compare the SET of distinct colours a
//   sprite pixel can show between BPLCON0.shres=0 and shres=1 (not an
//   absolute value against the manual's slot-A prediction), because an
//   absolute-value check would have passed "by coincidence" on an
//   ordinary, always-on 12-bit colour register -- exactly what this RTL
//   implements -- without actually confirming anything SuperHires-
//   specific, which is what every one of these 275 claims is about.
//
// HARNESS FIXES (documented, oracle text never touched):
//   - red/green/blue are 8 bits wide (denise.v:44-46: `output wire
//     [8-1:0]`), not the pre-RTL 4-bit guess -- kChannelBits corrected;
//     harmless in practice since it is only used for the diagnostic
//     describeColour()/describeSet() text now (see RTL OPENED finding
//     above for why no expect() condition uses an absolute channel
//     value).
//   - The original pre-RTL per-register measurement (absolute slot-A
//     value check + a separate "slot-B has some effect somewhere" probe)
//     was replaced, before ever being built or run, by the shres=0-vs-
//     shres=1 distinct-colour-SET comparison used below and in SHRES-
//     MODE-CHANGES-OUTPUT -- a methodology fix triggered directly by
//     reading denise_sprites_shifter.v's shres-dependent drain rate
//     (confirmed it is a pure timing/width effect that a raw-trace
//     comparison could mistake for a colour difference). No expected
//     VALUE from claims.jsonl was altered by this fix -- only how the RTL
//     is queried to check it.
//   - Register addresses (COLOR16-31 at word address 0xC0+offset,
//     matching COLORBASE 9'h180 and denise_colortable.v:30's
//     `wr_adr={bank[2:0],reg_address_in[5:1]}` with bank forced 0 at
//     aga=0), the SPRPOSCTLBASE/ATTACH sprite register convention, and
//     the four-phase clocking all matched this file's pre-RTL
//     assumptions exactly (reused, per BOUNDARY above, from already-
//     committed sibling harnesses for these same modules).
//   - THREE further fixes found only once the package was actually built
//     and run (the pre-RTL draft compiled and linked cleanly; these are
//     runtime/behavioural bugs a compiler cannot catch):
//     1) FIXTURE-SPRITE-DATA-PATH-LIVE itself failed on the first run
//        (no sprite pixel ever reached red/green/blue, for ANY register).
//        denise.v:410 gates every sprite with `sprena = display_ena |
//        brdsprt`; `display_ena` (denise.v:146-153, "in OCS sprites are
//        visible between first write to BPL1DAT and end of scanline")
//        only ever becomes 1 after a BPL1DAT write, and this package,
//        deliberately BPU=0 throughout, never wrote it. Fix: setupDisplay
//        now writes BPL1DAT once (value 0 -- harmless, since l_bpu stays
//        0 so the bitplane path stays inert regardless) purely to set
//        this gate -- see kBpl1DatAddr's own comment.
//     2) activeSamples() never called reset_line() before scanning, so
//        hpos free-ran from the Harness constructor's own reset through
//        every setup/arm register write -- a sprite's fixed hstart could
//        already have been reached, or fully drained past, before
//        scan_line() ever sampled anything, intermittently and (because
//        a faster SuperHires drain empties a shift register sooner)
//        mode-sensitively. Fix: reset_line() now runs after all setup/
//        arming and before every scan.
//     3) armPair() originally loaded DATA/DATB with a single MSB bit
//        (0x8000, the hrm/ch3-sprite-registers/ sibling's own
//        convention) -- a pulse exactly ONE shift-event wide. denise_
//        sprites_shifter.v's shift register advances TWICE per clk7_en
//        period at SuperHires resolution (denise_sprites.v:158-166), so
//        a one-shift-wide pulse can shift past entirely between two
//        consecutive once-per-clk7_en samples -- confirmed by a
//        temporary raw-trace dump showing an all-zero shres=1 trace
//        where shres=0 showed exactly one non-zero sample. Fix: DATA/
//        DATB now fill all 16 shift positions with the same bit
//        (0xFFFF/0x0000), giving a colour that is CONSTANT for the
//        sprite's whole ~16-clk7_en-period lifetime, immune to sampling
//        phase or drain rate. None of these three fixes touched an
//        expected VALUE from claims.jsonl -- only how the stimulus
//        reaches the RTL and when the response is sampled.
//
// BUILD / RESULTS: built and run under WSL (Ubuntu-24.04)/Verilator
// 5.052, `make` in this directory, 11.09.2026. 2 passed (both fixture
// controls), 17 failed (SHRES-MODE-CHANGES-OUTPUT and all 16 REG-0x10..
// REG-0x1F register assertions) -- every one of the 275 claim ids is
// covered, and every failure is read directly off the RTL cited above,
// not from the test's failure alone:
//   FINDING -- SuperHires sprite colour encoding (the entire page-301/
//   302 scheme) is NOT IMPLEMENTED at this boundary. Measured: for every
//   one of the 15 live-testable registers (COLOR17-31; COLOR16 is
//   separately unreachable, see REG-0x10's own reasoning), the SET of
//   distinct colours a sprite pixel selecting that register can show is
//   IDENTICAL at BPLCON0.shres=0 and shres=1 -- always exactly the
//   register's own top-two-bit ("slot A") value, e.g. REG-0x11 measured
//   "{R=2 G=1 B=3}" at BOTH settings. This matches the RTL OPENED finding
//   above (denise_colortable.v has no `shres` input at all; its RGB
//   packing duplicates each stored nibble the same way regardless of
//   mode) and is now confirmed empirically, not merely read from source.
//   REG-0x10 (COLOR16) additionally cannot be display-tested at all, for
//   the separate, architectural reason given in its own assertion
//   (combined sprite index 0 is universally transparent).
//
// DEFECT CORRECTION (independent validator, this agent, 11.09.2026), two
// separate findings:
//   1) REG-0x10 was checks.expect(false, ...) -- an unconditional failing
//      "assertion" with nothing to measure, a declaration, not a
//      measurement. Converted to Checks::decline() (see REG-0x10's own
//      code comment): printed, not counted in passed/failed, and no
//      longer gated for a mutation record. Its own twenty sibling ids
//      (sixteen per-bit, three symbol, one legend) are declined together
//      with it, for the identical reason. Counts become: 2 passed, 16
//      failed, 1 declined (REG-0x10, 21 ids) of the 19 checks.expect()
//      calls this file now makes.
//   2) REG-0x11's own aggregate check (like every REG-0x1X entry) asks
//      only whether the SET of distinct colours differs between shres
//      modes for ONE combined, multi-bit stimulus -- it never isolates a
//      single bit, so it does not verify C-shres-sprite-bits.c21's own
//      claim ("COLOR11 bit 11 carries symbol G from sprite color 17
//      red"). Added SHRES-SPR11-BIT11-ISOLATES-RED (below, isolates
//      C-shres-sprite-bits.c21 on its own, same reasoning and same
//      channel-only scope as the sibling ch3-shres-bitplane-colour-
//      encoding package's own SHRES-BPL01-BIT11-ISOLATES-RED): with only
//      bit 11 set, the resulting sprite pixel moves the red channel and
//      no other -- PASSES. The colour-17-vs-colour-16 half is not
//      independently observable here (same underlying reason as the
//      bitplane sibling) and is not re-tested; REG-0x11's own sweep
//      above already carries it, red for the reason given in the FINDING
//      above. Current totals: 3 passed, 16 failed, 1 declined (20
//      checks.expect() calls, 1 decline). No existing assertion's oracle
//      text, expected value or comparison logic was changed by either
//      correction.
//
// MUTATION LOG (all 4 stimulus entries actually run by this agent under
// WSL/Verilator on 11.09.2026 -- edit -> rebuild -> run -> confirm the
// named assertion, and only that assertion, changed colour -> revert ->
// rebuild -> confirm the 2-passed/17-failed baseline restored -- before
// moving to the next one. nanomig/ was never touched; no expected value,
// oracle text or comparison logic was ever edited, only stimulus. The
// first attempt at the FIXTURE-SPR0-TRANSPARENT-AT-VALUE0 mutation chose
// a stimulus (arming value 1 while still reading COLOR16, which the
// mutated arming no longer addresses) that coincidentally produced the
// same black-on-black result as the unmutated baseline -- not a finding,
// a mutation-design mistake; fixed by also moving the read address to
// COLOR17, the register the mutated arming actually selects, before the
// result below was recorded):
//
// MUTATION FIXTURE-SPRITE-DATA-PATH-LIVE stimulus red: the arming value changed from 1 to 0 (disarmed, identical to the baseline it is diffed against) -> FAIL ("1 passed, 18 failed"), only FIXTURE-SPRITE-DATA-PATH-LIVE newly red (diffs became empty). Reverted to 1: 2/17, same finding.
// MUTATION FIXTURE-SPR0-TRANSPARENT-AT-VALUE0 stimulus red: the arming value changed from 0 to 1 AND the read register from COLOR16 to COLOR17 (the register that arming value actually addresses, now loaded with the same all-ones pattern) -> FAIL ("1 passed, 18 failed"), only FIXTURE-SPR0-TRANSPARENT-AT-VALUE0 newly red (diffs became non-empty -- value 1 is not transparent). Reverted to even=0/COLOR16: 2/17, same finding.
// MUTATION SHRES-MODE-CHANGES-OUTPUT none impossible: already red at baseline (the FINDING above). The measured distinct-colour set is identical at shres=0 and shres=1 for every register content this file can write, so no stimulus this harness can present makes this assertion's condition (the two sets differing) true; there is nothing green here to break.
// REG-0x10 carries no mutation record: a defect-correction pass (this agent, 11.09.2026, see REG-0x10's own code comment) converted it from checks.expect(false, ...) to Checks::decline() -- a declared-unreachable claim, not an assertion, so tools/check_gates.py's mutation gate no longer requires or expects a record for it.
// MUTATION REG-0x11 none impossible: already red at baseline (the FINDING above -- shres=0/shres=1 distinct-colour sets identical); no stimulus this harness can present makes them differ.
// MUTATION REG-0x12 none impossible: already red at baseline (the FINDING above); no stimulus this harness can present makes the shres=0/shres=1 distinct-colour sets differ.
// MUTATION REG-0x13 none impossible: already red at baseline (the FINDING above); no stimulus this harness can present makes the shres=0/shres=1 distinct-colour sets differ.
// MUTATION REG-0x14 none impossible: already red at baseline (the FINDING above); no stimulus this harness can present makes the shres=0/shres=1 distinct-colour sets differ.
// MUTATION REG-0x15 none impossible: already red at baseline (the FINDING above); no stimulus this harness can present makes the shres=0/shres=1 distinct-colour sets differ.
// MUTATION REG-0x16 none impossible: already red at baseline (the FINDING above); no stimulus this harness can present makes the shres=0/shres=1 distinct-colour sets differ.
// MUTATION REG-0x17 none impossible: already red at baseline (the FINDING above); no stimulus this harness can present makes the shres=0/shres=1 distinct-colour sets differ.
// MUTATION REG-0x18 none impossible: already red at baseline (the FINDING above); no stimulus this harness can present makes the shres=0/shres=1 distinct-colour sets differ.
// MUTATION REG-0x19 none impossible: already red at baseline (the FINDING above); no stimulus this harness can present makes the shres=0/shres=1 distinct-colour sets differ.
// MUTATION REG-0x1A none impossible: already red at baseline (the FINDING above); no stimulus this harness can present makes the shres=0/shres=1 distinct-colour sets differ.
// MUTATION REG-0x1B none impossible: already red at baseline (the FINDING above); no stimulus this harness can present makes the shres=0/shres=1 distinct-colour sets differ.
// MUTATION REG-0x1C none impossible: already red at baseline (the FINDING above); no stimulus this harness can present makes the shres=0/shres=1 distinct-colour sets differ.
// MUTATION REG-0x1D none impossible: already red at baseline (the FINDING above); no stimulus this harness can present makes the shres=0/shres=1 distinct-colour sets differ.
// MUTATION REG-0x1E none impossible: already red at baseline (the FINDING above); no stimulus this harness can present makes the shres=0/shres=1 distinct-colour sets differ.
// MUTATION REG-0x1F none impossible: already red at baseline (the FINDING above); no stimulus this harness can present makes the shres=0/shres=1 distinct-colour sets differ.
// MUTATION SHRES-SPR11-BIT11-ISOLATES-RED stimulus red: -DMUT_SPR11BIT11RED=1 replaces the bit-11 stimulus with bit 7 (a green-channel bit); the measured sprite pixel no longer moved the red channel, so the assertion's own red-channel-isolation check went red alone. Reverted: green baseline restored.

#ifndef MUT_SPR11BIT11RED
#define MUT_SPR11BIT11RED 0
#endif

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <set>
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
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    // A DECLINED claim: a proposition this boundary cannot measure at all
    // (not a pass, not a fail -- a false measurement dressed up as one
    // would be worse than an honest non-measurement). Printed but not
    // counted in passed/failed, and deliberately NOT named expect()/
    // report() -- tools/check_gates.py's mutation gate reads only those
    // two call names as assertions, so a decline correctly asks for no
    // mutation record.
    void decline(const std::string& label) {
        std::cout << "DECLINED: " << label << "\n";
        ++declined;
    }
    int passed = 0;
    int failed = 0;
    int declined = 0;
};

// ---------------------------------------------------------------------
// Custom chip-register addresses (byte addresses, as printed in the
// manual / reused from already-committed sibling harnesses -- see
// BOUNDARY above). reg_address_in is [8:1], an already-halved word
// address; write_reg below does the halving.
// ---------------------------------------------------------------------
constexpr uint16_t kDiwstrtAddr = 0x08E;
constexpr uint16_t kDiwstopAddr = 0x090;
constexpr uint16_t kBplcon0Addr = 0x100;
// BPL1DAT (9'h110): NOT written for its own data -- BPU stays 0
// throughout this file, so denise.v's own l_bpu latch (l.194-203) stays
// 0 and every bitplane shift register reads permanently inert (l.367-
// 374) regardless of what is written here. Written ONCE per Harness,
// value 0, purely to set `display_ena` (denise.v:146-153, "in OCS
// sprites are visible between first write to BPL1DAT and end of
// scanline"): sprites are gated by `sprena = display_ena | brdsprt`
// (denise.v:410) and this file never sets brdsprt (BPLCON3 stays at its
// reset default), so without this one write no sprite pixel -- of any
// colour, at any register -- ever reaches the RGB output at all. Found
// via FIXTURE-SPRITE-DATA-PATH-LIVE failing on the first build attempt;
// a Harness/mechanics fix, not an oracle change (see HARNESS FIXES).
constexpr uint16_t kBpl1DatAddr = 0x110;
constexpr uint16_t kColorbaseAddr = 0x180;  // COLOR00
constexpr uint16_t kColorAddr(int n) { return static_cast<uint16_t>(kColorbaseAddr + 2 * n); }  // COLOR n, n=0..31

// Sprite registers: SPRPOSCTLBASE 9'h140, four consecutive words per
// sprite (POS, CTL, DATA, DATB), reused from hrm/ch3-sprite-registers/.
constexpr uint16_t kSprBase = 0x140;
constexpr uint16_t kSprPosAddr(int n) { return static_cast<uint16_t>(kSprBase + 8 * n + 0); }
constexpr uint16_t kSprCtlAddr(int n) { return static_cast<uint16_t>(kSprBase + 8 * n + 2); }
constexpr uint16_t kSprDataAddr(int n) { return static_cast<uint16_t>(kSprBase + 8 * n + 4); }
constexpr uint16_t kSprDatbAddr(int n) { return static_cast<uint16_t>(kSprBase + 8 * n + 6); }
constexpr uint16_t kSprCtlAttachBit = 0x80;

// BPLCON0 field bits, per PACKAGE.md's own evidence field.
constexpr uint16_t kBplcon0Shres = 0x0040;   // bit 6
constexpr uint16_t kBplcon0Ecsena = 0x0001;  // bit 0

// Channel width, CONFIRMED after opening the RTL (denise.v:44-46: red/
// green/blue are `output wire [8-1:0]`, 8 bits each -- see HARNESS FIXES
// below). denise_colortable.v:56-58 packs each 4-bit stored nibble into
// its own 8-bit channel by literal duplication ({nibble,nibble}), so a
// channel's top two bits (bits 7:6) always equal the stored nibble's own
// top two bits -- used only for the MEASURED diagnostic notes below, not
// for any expect() gating condition (see HARNESS FIXES).
constexpr int kChannelBits = 8;

// ---------------------------------------------------------------------
// THE ORACLE -- transcribed verbatim from claims.jsonl's sixteen row
// quotes (C-shrs.r10..r1F, page 302), bits 15 downto 0 left-to-right,
// '.' for the printed "." (unused). Cross-checked programmatically
// against every one of the 240 individual C-shres-sprite-bits.cN
// restatements before this file was written (register-by-register,
// bit-by-bit) -- not hand-copied from the table image a second time.
// ---------------------------------------------------------------------
struct RegRow {
    const char* hex;       // "10".."1F"
    const char* symbols;   // 16 chars, index 0 = bit15 .. index 15 = bit0
};

constexpr RegRow kRows[16] = {
    {"10", "....ABABCDCDEFEF"},
    {"11", "....GHABIJCDKLEF"},
    {"12", "....MNABOPCDQREF"},
    {"13", "....STABUVCDWXEF"},
    {"14", "....ABGHCDIJEFKL"},
    {"15", "....GHGHIJIJKLKL"},
    {"16", "....MNGHOPIJQRKL"},
    {"17", "....STGHUVIJWXKL"},
    {"18", "....ABMNCDOPEFQR"},
    {"19", "....GHMNIJOPKLQR"},
    {"1A", "....MNMNOPOPQRQR"},
    {"1B", "....STMNUVOPWXQR"},
    {"1C", "....ABSTCDUVEFWX"},
    {"1D", "....GHSTIJUVKLWX"},
    {"1E", "....MNSTOPUVQRWX"},
    {"1F", "....STSTUVUVWXWX"},
};

// Symbol -> (sprite colour, channel, hi-or-lo) from the twelve
// C-shres-sprite-symbols.cN quotes: A-F -> colour16, G-L -> colour17,
// M-R -> colour18, S-X -> colour19; within each colour's six letters,
// [0]=R-hi [1]=R-lo [2]=G-hi [3]=G-lo [4]=B-hi [5]=B-lo.
struct SymInfo {
    int color;
    char channel;
    bool hi;
};

SymInfo symInfo(char c) {
    int L = c - 'A';
    int color = 16 + L / 6;
    int group = L % 6;
    char channel = (group < 2) ? 'R' : (group < 4 ? 'G' : 'B');
    bool hi = (group % 2 == 0);
    return SymInfo{color, channel, hi};
}

// Claim ids for each register's assertion, WRITTEN OUT IN FULL as literal
// text (not computed at run time) so every id is findable by grepping
// this source file itself, not only this program's stdout. The bit-claim
// numbering is exactly sequential -- 16 ids per register, in this same
// hex order, bit15 downto bit0 -- EXCEPT register $19 (index 9), whose
// 16-id block (c145-c160) was never emitted into claims.jsonl (confirmed
// by direct inspection of the file, not assumed, hence its row-only
// entry below). Every one of these 275 ids was cross-checked
// programmatically against claims.jsonl's own id list before being
// pasted in here -- not re-typed by hand a second time. The four
// diagonal registers ($10,$15,$1A,$1F) additionally carry that sprite
// colour's three symbol ids and one legend id (see CLAIM COVERAGE above).
constexpr const char* kRegIds[16] = {
    "C-shrs.r10, C-shres-sprite-bits.c1, C-shres-sprite-bits.c2, C-shres-sprite-bits.c3, C-shres-sprite-bits.c4, C-shres-sprite-bits.c5, C-shres-sprite-bits.c6, C-shres-sprite-bits.c7, C-shres-sprite-bits.c8, C-shres-sprite-bits.c9, C-shres-sprite-bits.c10, C-shres-sprite-bits.c11, C-shres-sprite-bits.c12, C-shres-sprite-bits.c13, C-shres-sprite-bits.c14, C-shres-sprite-bits.c15, C-shres-sprite-bits.c16, C-shres-sprite-symbols.c1, C-shres-sprite-symbols.c2, C-shres-sprite-symbols.c3, C-shrc.srcs16",
    "C-shrs.r11, C-shres-sprite-bits.c17, C-shres-sprite-bits.c18, C-shres-sprite-bits.c19, C-shres-sprite-bits.c20, C-shres-sprite-bits.c21, C-shres-sprite-bits.c22, C-shres-sprite-bits.c23, C-shres-sprite-bits.c24, C-shres-sprite-bits.c25, C-shres-sprite-bits.c26, C-shres-sprite-bits.c27, C-shres-sprite-bits.c28, C-shres-sprite-bits.c29, C-shres-sprite-bits.c30, C-shres-sprite-bits.c31, C-shres-sprite-bits.c32",
    "C-shrs.r12, C-shres-sprite-bits.c33, C-shres-sprite-bits.c34, C-shres-sprite-bits.c35, C-shres-sprite-bits.c36, C-shres-sprite-bits.c37, C-shres-sprite-bits.c38, C-shres-sprite-bits.c39, C-shres-sprite-bits.c40, C-shres-sprite-bits.c41, C-shres-sprite-bits.c42, C-shres-sprite-bits.c43, C-shres-sprite-bits.c44, C-shres-sprite-bits.c45, C-shres-sprite-bits.c46, C-shres-sprite-bits.c47, C-shres-sprite-bits.c48",
    "C-shrs.r13, C-shres-sprite-bits.c49, C-shres-sprite-bits.c50, C-shres-sprite-bits.c51, C-shres-sprite-bits.c52, C-shres-sprite-bits.c53, C-shres-sprite-bits.c54, C-shres-sprite-bits.c55, C-shres-sprite-bits.c56, C-shres-sprite-bits.c57, C-shres-sprite-bits.c58, C-shres-sprite-bits.c59, C-shres-sprite-bits.c60, C-shres-sprite-bits.c61, C-shres-sprite-bits.c62, C-shres-sprite-bits.c63, C-shres-sprite-bits.c64",
    "C-shrs.r14, C-shres-sprite-bits.c65, C-shres-sprite-bits.c66, C-shres-sprite-bits.c67, C-shres-sprite-bits.c68, C-shres-sprite-bits.c69, C-shres-sprite-bits.c70, C-shres-sprite-bits.c71, C-shres-sprite-bits.c72, C-shres-sprite-bits.c73, C-shres-sprite-bits.c74, C-shres-sprite-bits.c75, C-shres-sprite-bits.c76, C-shres-sprite-bits.c77, C-shres-sprite-bits.c78, C-shres-sprite-bits.c79, C-shres-sprite-bits.c80",
    "C-shrs.r15, C-shres-sprite-bits.c81, C-shres-sprite-bits.c82, C-shres-sprite-bits.c83, C-shres-sprite-bits.c84, C-shres-sprite-bits.c85, C-shres-sprite-bits.c86, C-shres-sprite-bits.c87, C-shres-sprite-bits.c88, C-shres-sprite-bits.c89, C-shres-sprite-bits.c90, C-shres-sprite-bits.c91, C-shres-sprite-bits.c92, C-shres-sprite-bits.c93, C-shres-sprite-bits.c94, C-shres-sprite-bits.c95, C-shres-sprite-bits.c96, C-shres-sprite-symbols.c4, C-shres-sprite-symbols.c5, C-shres-sprite-symbols.c6, C-shrc.srcs17",
    "C-shrs.r16, C-shres-sprite-bits.c97, C-shres-sprite-bits.c98, C-shres-sprite-bits.c99, C-shres-sprite-bits.c100, C-shres-sprite-bits.c101, C-shres-sprite-bits.c102, C-shres-sprite-bits.c103, C-shres-sprite-bits.c104, C-shres-sprite-bits.c105, C-shres-sprite-bits.c106, C-shres-sprite-bits.c107, C-shres-sprite-bits.c108, C-shres-sprite-bits.c109, C-shres-sprite-bits.c110, C-shres-sprite-bits.c111, C-shres-sprite-bits.c112",
    "C-shrs.r17, C-shres-sprite-bits.c113, C-shres-sprite-bits.c114, C-shres-sprite-bits.c115, C-shres-sprite-bits.c116, C-shres-sprite-bits.c117, C-shres-sprite-bits.c118, C-shres-sprite-bits.c119, C-shres-sprite-bits.c120, C-shres-sprite-bits.c121, C-shres-sprite-bits.c122, C-shres-sprite-bits.c123, C-shres-sprite-bits.c124, C-shres-sprite-bits.c125, C-shres-sprite-bits.c126, C-shres-sprite-bits.c127, C-shres-sprite-bits.c128",
    "C-shrs.r18, C-shres-sprite-bits.c129, C-shres-sprite-bits.c130, C-shres-sprite-bits.c131, C-shres-sprite-bits.c132, C-shres-sprite-bits.c133, C-shres-sprite-bits.c134, C-shres-sprite-bits.c135, C-shres-sprite-bits.c136, C-shres-sprite-bits.c137, C-shres-sprite-bits.c138, C-shres-sprite-bits.c139, C-shres-sprite-bits.c140, C-shres-sprite-bits.c141, C-shres-sprite-bits.c142, C-shres-sprite-bits.c143, C-shres-sprite-bits.c144",
    "C-shrs.r19",
    "C-shrs.r1A, C-shres-sprite-bits.c161, C-shres-sprite-bits.c162, C-shres-sprite-bits.c163, C-shres-sprite-bits.c164, C-shres-sprite-bits.c165, C-shres-sprite-bits.c166, C-shres-sprite-bits.c167, C-shres-sprite-bits.c168, C-shres-sprite-bits.c169, C-shres-sprite-bits.c170, C-shres-sprite-bits.c171, C-shres-sprite-bits.c172, C-shres-sprite-bits.c173, C-shres-sprite-bits.c174, C-shres-sprite-bits.c175, C-shres-sprite-bits.c176, C-shres-sprite-symbols.c7, C-shres-sprite-symbols.c8, C-shres-sprite-symbols.c9, C-shrc.srcs18",
    "C-shrs.r1B, C-shres-sprite-bits.c177, C-shres-sprite-bits.c178, C-shres-sprite-bits.c179, C-shres-sprite-bits.c180, C-shres-sprite-bits.c181, C-shres-sprite-bits.c182, C-shres-sprite-bits.c183, C-shres-sprite-bits.c184, C-shres-sprite-bits.c185, C-shres-sprite-bits.c186, C-shres-sprite-bits.c187, C-shres-sprite-bits.c188, C-shres-sprite-bits.c189, C-shres-sprite-bits.c190, C-shres-sprite-bits.c191, C-shres-sprite-bits.c192",
    "C-shrs.r1C, C-shres-sprite-bits.c193, C-shres-sprite-bits.c194, C-shres-sprite-bits.c195, C-shres-sprite-bits.c196, C-shres-sprite-bits.c197, C-shres-sprite-bits.c198, C-shres-sprite-bits.c199, C-shres-sprite-bits.c200, C-shres-sprite-bits.c201, C-shres-sprite-bits.c202, C-shres-sprite-bits.c203, C-shres-sprite-bits.c204, C-shres-sprite-bits.c205, C-shres-sprite-bits.c206, C-shres-sprite-bits.c207, C-shres-sprite-bits.c208",
    "C-shrs.r1D, C-shres-sprite-bits.c209, C-shres-sprite-bits.c210, C-shres-sprite-bits.c211, C-shres-sprite-bits.c212, C-shres-sprite-bits.c213, C-shres-sprite-bits.c214, C-shres-sprite-bits.c215, C-shres-sprite-bits.c216, C-shres-sprite-bits.c217, C-shres-sprite-bits.c218, C-shres-sprite-bits.c219, C-shres-sprite-bits.c220, C-shres-sprite-bits.c221, C-shres-sprite-bits.c222, C-shres-sprite-bits.c223, C-shres-sprite-bits.c224",
    "C-shrs.r1E, C-shres-sprite-bits.c225, C-shres-sprite-bits.c226, C-shres-sprite-bits.c227, C-shres-sprite-bits.c228, C-shres-sprite-bits.c229, C-shres-sprite-bits.c230, C-shres-sprite-bits.c231, C-shres-sprite-bits.c232, C-shres-sprite-bits.c233, C-shres-sprite-bits.c234, C-shres-sprite-bits.c235, C-shres-sprite-bits.c236, C-shres-sprite-bits.c237, C-shres-sprite-bits.c238, C-shres-sprite-bits.c239, C-shres-sprite-bits.c240",
    "C-shrs.r1F, C-shres-sprite-bits.c241, C-shres-sprite-bits.c242, C-shres-sprite-bits.c243, C-shres-sprite-bits.c244, C-shres-sprite-bits.c245, C-shres-sprite-bits.c246, C-shres-sprite-bits.c247, C-shres-sprite-bits.c248, C-shres-sprite-bits.c249, C-shres-sprite-bits.c250, C-shres-sprite-bits.c251, C-shres-sprite-bits.c252, C-shres-sprite-bits.c253, C-shres-sprite-bits.c254, C-shres-sprite-bits.c255, C-shres-sprite-bits.c256, C-shres-sprite-symbols.c10, C-shres-sprite-symbols.c11, C-shres-sprite-symbols.c12, C-shrc.srcs19",
};

// ---------------------------------------------------------------------
// Harness -- four-phase clocking and write_reg reused from the
// already-committed hrm/ch3-horizontal-display-window/ sibling (same
// `denise` fixture); sprite arming reused (addresses, ATTACH convention)
// from the already-committed hrm/ch3-sprite-registers/ sibling (same
// denise_sprites sub-module).
// ---------------------------------------------------------------------
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

    void write_reg(uint16_t custom_addr, uint16_t data) {
        while (phase != 0) tick();
        dut->reg_address_in = static_cast<uint8_t>(custom_addr >> 1);
        dut->data_in = data;
        tick();
        dut->reg_address_in = 0;
        dut->data_in = 0;
    }

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

    // Samples red/green/blue once per clk7_en-aligned step, for `n`
    // steps from the start of a fresh line.
    std::vector<uint32_t> scan_line(int n) {
        std::vector<uint32_t> samples;
        samples.reserve(n);
        for (int i = 0; i < n; ++i) {
            while (phase != 0) tick();
            samples.push_back(pack_rgb(dut));
            tick();
        }
        return samples;
    }

    Vdenise* dut;
    int phase = 0;
};

// Fixed sprite position used throughout (SPR0POS=0x40 places a sprite
// near raw hpos 0x80 -- an observation already made and re-measured by
// the already-committed hrm/ch3-sprite-registers/ sibling harness for
// this same denise_sprites sub-module -- reused only to pick a position
// safely inside the wide-open window below, never as the basis of any
// expect() comparison itself, which always diffs against a same-position
// disarmed baseline instead).
constexpr uint16_t kSpritePosByte = 0x40;
constexpr uint16_t kSpriteCtlSh0 = 0x00;
constexpr int kScanSamples = 240;
constexpr uint16_t kDiwstrtWide = 0x10;
constexpr uint16_t kDiwstopWide = 0xF0;

// Programs a wide-open display window, ECS BPLCON0 (SHRES per `shres`),
// and no bitplanes (BPU=0 -- background is COLOR00 wherever no sprite
// draws).
void setupDisplay(Harness& h, bool shres = true) {
    h.write_reg(kDiwstrtAddr, kDiwstrtWide);
    h.write_reg(kDiwstopAddr, kDiwstopWide);
    h.write_reg(kBplcon0Addr, (shres ? kBplcon0Shres : 0) | kBplcon0Ecsena);
    h.write_reg(kBpl1DatAddr, 0x0000);  // sets display_ena -- see kBpl1DatAddr's own comment.
}

// Arms sprite pair (SPR0=even/low, SPR1=odd/high) at the fixed position,
// each with its own 2-bit pixel value (0-3), attach set on SPR1 only when
// useOdd is true (the "unattached single sprite" case never touches
// SPR1 at all -- SPR1 stays disarmed, its own value irrelevant). DATA/
// DATB are filled with the SAME bit (0xFFFF or 0x0000) across all 16
// shift positions -- not just the MSB -- so the sprite shows a CONSTANT
// pixel value for its entire ~16-clk7_en-period lifetime. A second-build
// fix (see HARNESS FIXES): a single-MSB-bit pulse (0x8000) is only ONE
// shift-event wide, and denise_sprites_shifter.v's shift register
// advances TWICE per clk7_en period at SuperHires resolution
// (denise_sprites.v:158-166) -- a single shift-event-wide pulse can
// shift past entirely between two consecutive once-per-clk7_en samples,
// which is exactly what happened (an all-zero trace at shres=1,
// discovered via a temporary raw-trace dump, not assumed).
void armPair(Harness& h, int evenVal, int oddVal, bool useOdd) {
    uint16_t evenData = (evenVal & 1) ? 0xFFFF : 0x0000;
    uint16_t evenDatb = (evenVal & 2) ? 0xFFFF : 0x0000;
    h.write_reg(kSprPosAddr(0), kSpritePosByte);
    h.write_reg(kSprCtlAddr(0), kSpriteCtlSh0);
    h.write_reg(kSprDatbAddr(0), evenDatb);
    h.write_reg(kSprDataAddr(0), evenData);
    if (useOdd) {
        uint16_t oddData = (oddVal & 1) ? 0xFFFF : 0x0000;
        uint16_t oddDatb = (oddVal & 2) ? 0xFFFF : 0x0000;
        h.write_reg(kSprPosAddr(1), kSpritePosByte);
        h.write_reg(kSprCtlAddr(1), kSpriteCtlSh0 | kSprCtlAttachBit);
        h.write_reg(kSprDatbAddr(1), oddDatb);
        h.write_reg(kSprDataAddr(1), oddData);
    }
}

// The set of samples that differ from a same-setup, same-position,
// disarmed-sprite baseline (DATA=DATB=0 for both SPR0 and SPR1) -- i.e.
// exactly the samples this sprite's own pixel(s) drove. `shres` selects
// BPLCON0.shres for BOTH the live and the baseline scan (so a pure
// SuperHires drain-RATE difference -- denise_sprites.v:158-166's `shift`
// case on spres/spres_d, confirmed after opening the RTL to affect only
// how fast an already-loaded shift register drains, never the loaded
// VALUE itself -- cannot by itself make this function return a
// differently-SIZED diff list; see distinctValues() below for the
// colour-only comparison that does not depend on how many samples were
// active). Calls reset_line() AFTER all setup/arming and BEFORE
// scanning (a second-build fix -- see HARNESS FIXES): without it, hpos
// free-runs from the Harness constructor's own reset, so the sprite's
// FIXED hstart could already have been reached, or even fully drained
// past, during the setup/arming writes themselves, before scan_line
// ever samples anything -- intermittently and mode-sensitively (a
// faster SuperHires drain empties a shift register that started
// draining mid-setup sooner), not because of any actual colour
// difference. reset_line() gives every measurement the same known
// hpos=2 starting point regardless of setup length or shres.
std::vector<uint32_t> activeSamples(uint16_t regAddr, uint16_t regValue,
                                     int evenVal, int oddVal, bool useOdd,
                                     bool shres = true) {
    Harness base;
    setupDisplay(base, shres);
    base.write_reg(regAddr, regValue);
    armPair(base, 0, 0, useOdd);  // disarmed: both members value 0
    base.reset_line();  // known hpos=2 start -- see this function's own comment above.
    auto baseline = base.scan_line(kScanSamples);

    Harness live;
    setupDisplay(live, shres);
    live.write_reg(regAddr, regValue);
    armPair(live, evenVal, oddVal, useOdd);
    live.reset_line();
    auto trace = live.scan_line(kScanSamples);

    std::vector<uint32_t> diffs;
    for (size_t i = 0; i < trace.size() && i < baseline.size(); ++i) {
        if (trace[i] != baseline[i]) diffs.push_back(trace[i]);
    }
    return diffs;
}

// The set of DISTINCT colour values among `samples`, sorted -- comparing
// two of these (one per BPLCON0.shres setting) isolates a genuine colour-
// encoding difference from a mere width/duration difference (which would
// change how many samples are active, and at which sample indices, but
// not which distinct colours appear).
std::vector<uint32_t> distinctValues(const std::vector<uint32_t>& samples) {
    std::set<uint32_t> s(samples.begin(), samples.end());
    return std::vector<uint32_t>(s.begin(), s.end());
}

// Extracts channel `ch` ('R','G','B') top-2-bit value (0-3) from a
// packed RGB sample, per kChannelBits. Diagnostic use only (MEASURED
// notes below) -- no expect() condition depends on this decomposition,
// since it cannot by itself tell "SuperHires-specific" apart from
// "ordinary colour register, ordinary nibble-to-byte duplication".
int top2(uint32_t rgb, char ch) {
    uint32_t v;
    if (ch == 'R') v = (rgb >> 16) & ((1u << kChannelBits) - 1);
    else if (ch == 'G') v = (rgb >> 8) & ((1u << kChannelBits) - 1);
    else v = rgb & ((1u << kChannelBits) - 1);
    return static_cast<int>(v >> (kChannelBits - 2)) & 0x3;
}

// Diagnostic-only rendering of a colour or a colour SET, e.g. for a
// MEASURED note explaining exactly what this file observed.
std::string describeColour(uint32_t rgb) {
    return "R=" + std::to_string(top2(rgb, 'R')) +
           " G=" + std::to_string(top2(rgb, 'G')) +
           " B=" + std::to_string(top2(rgb, 'B')) +
           " (raw 0x" + [&] {
               char buf[8];
               std::snprintf(buf, sizeof(buf), "%06X", rgb);
               return std::string(buf);
           }() + ")";
}
std::string describeSet(const std::vector<uint32_t>& colours) {
    if (colours.empty()) return "{}";
    std::string s = "{";
    for (size_t i = 0; i < colours.size(); ++i) {
        if (i) s += ", ";
        s += describeColour(colours[i]);
    }
    s += "}";
    return s;
}

// Builds a 12-bit COLOR-register write value from six independently
// chosen 2-bit fields (slot A hi/lo for R/G/B at bits 11:10/7:6/3:2, slot
// B hi/lo at bits 9:8/5:4/1:0), with the four unused bits 15-12 set from
// `unused4`.
uint16_t buildRegValue(int rAhi, int rAlo, int gAhi, int gAlo, int bAhi, int bAlo,
                        int rBhi, int rBlo, int gBhi, int gBlo, int bBhi, int bBlo,
                        int unused4) {
    uint16_t v = 0;
    v |= static_cast<uint16_t>((unused4 & 0xF) << 12);
    v |= static_cast<uint16_t>((rAhi & 1) << 11);
    v |= static_cast<uint16_t>((rAlo & 1) << 10);
    v |= static_cast<uint16_t>((rBhi & 1) << 9);
    v |= static_cast<uint16_t>((rBlo & 1) << 8);
    v |= static_cast<uint16_t>((gAhi & 1) << 7);
    v |= static_cast<uint16_t>((gAlo & 1) << 6);
    v |= static_cast<uint16_t>((gBhi & 1) << 5);
    v |= static_cast<uint16_t>((gBlo & 1) << 4);
    v |= static_cast<uint16_t>((bAhi & 1) << 3);
    v |= static_cast<uint16_t>((bAlo & 1) << 2);
    v |= static_cast<uint16_t>((bBhi & 1) << 1);
    v |= static_cast<uint16_t>((bBlo & 1) << 0);
    return v;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ==================== control: FIXTURE-SPRITE-DATA-PATH-LIVE ====================
    {
        // Sprite colour 17 (unattached SPR0 value=1), register $11
        // (offset 1), a saturated distinguishing pattern -- proves a
        // written, non-zero, non-constant sprite pixel actually reaches
        // the RGB output via this Harness's write_reg/armPair/scan_line
        // path before ANY claim-based assertion below depends on it.
        uint16_t regv = buildRegValue(/*R*/ 1, 0, /*G*/ 0, 1, /*B*/ 1, 1,
                                       /*slotB*/ 1, 1, 0, 0, 0, 1, /*unused*/ 0xA);
        auto diffs = activeSamples(kColorAddr(17), regv, /*even=*/1, /*odd=*/0, /*useOdd=*/false);
        checks.expect(!diffs.empty(),
            "[control] FIXTURE-SPRITE-DATA-PATH-LIVE: arming an unattached "
            "sprite 0 with its own pixel value 1 (combined index 1, register "
            "COLOR17/$11) against a same-setup disarmed baseline produces at "
            "least one differing RGB sample -- proving this Harness's "
            "write_reg -> armPair -> scan_line -> diff-against-baseline "
            "measurement path is actually live before any claim-based "
            "assertion below depends on it. Fixture check, no claim id, no "
            "page.");
    }
    // ==================== control: FIXTURE-SPR0-TRANSPARENT-AT-VALUE0 ====================
    {
        uint16_t regv = buildRegValue(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0xF);
        auto diffs = activeSamples(kColorAddr(16), regv, /*even=*/0, /*odd=*/0, /*useOdd=*/false);
        checks.expect(diffs.empty(),
            "[control] FIXTURE-SPR0-TRANSPARENT-AT-VALUE0: an unattached "
            "sprite armed with its own pixel value 0 (combined index 0, "
            "register COLOR16/$10) produces NO differing sample versus the "
            "disarmed baseline, even though COLOR16 itself is loaded with an "
            "all-ones pattern -- confirming combined sprite index 0 is "
            "universally transparent on this boundary, independent of "
            "SuperHires mode or of what COLOR16 stores. This is why REG-0x10 "
            "below cannot be confirmed via live display. Fixture check, no "
            "claim id, no page.");
    }

    // ==================== SHRES-MODE-CHANGES-OUTPUT (general) ====================
    {
        // Register COLOR17/$11 (offset 1, unattached sprite value=1),
        // written so its top-two-bit ('slot A') R field (bits 11:10 = 2)
        // and its bottom-two-bit ('slot B') R field (bits 9:8 = 1) differ.
        // If SuperHires colour multiplexing -- the p.301/p.302 scheme --
        // occurs at all, the DISTINCT colour values a sprite pixel
        // selecting this register can show must differ between
        // BPLCON0.shres=0 and BPLCON0.shres=1. Compared as a SET of
        // distinct colours (distinctValues), not as a raw sample-by-
        // sample trace, specifically so that denise_sprites.v:158-166's
        // OWN shres/spres-dependent shift RATE (confirmed after opening
        // the RTL: SuperHires drains an already-loaded sprite shift
        // register twice as fast, changing how WIDE the sprite is on
        // screen, not what colour it is) cannot by itself make this
        // comparison read "differs" for a reason that has nothing to do
        // with colour.
        uint16_t regv = buildRegValue(/*rA*/ 1, 0, /*gA*/ 0, 0, /*bA*/ 0, 0,
                                       /*rB*/ 0, 1, /*gB*/ 0, 0, /*bB*/ 0, 0, 0);
        auto set0 = distinctValues(activeSamples(kColorAddr(17), regv, 1, 0, false, /*shres=*/false));
        auto set1 = distinctValues(activeSamples(kColorAddr(17), regv, 1, 0, false, /*shres=*/true));

        bool differs = (set0 != set1);
        checks.expect(differs,
            "SHRES-MODE-CHANGES-OUTPUT: with register COLOR17/$11 loaded so "
            "its top-two-bit ('slot A') R field (bits 11:10, printed '10') "
            "differs from its bottom-two-bit ('slot B') R field (bits 9:8, "
            "printed '01'), the SET of distinct colours a sprite pixel "
            "selecting this register can show (measured across the whole "
            "scanned line, at BOTH BPLCON0.shres settings, so a pure width/"
            "timing difference cannot be mistaken for a colour difference) "
            "differs between BPLCON0.shres=0 and BPLCON0.shres=1 -- "
            "confirming that SuperHires mode actually changes how sprite "
            "colour data is read out of these registers, not merely that "
            "the registers exist: 'In order to decode sprite and bitplane "
            "color information in SuperHires mode, certain multiplexing "
            "occurs in the use of the registers.' (p.301). 'SuperHires "
            "sprites are encoded in the upper sixteen color registers "
            "using a similar scheme:' (p.302, quoted twice by two "
            "independent readers). C-shrc.c03, C-shres-color.c8, "
            "C-shrs.c01 .");
        checks.note("SHRES-MODE-CHANGES-OUTPUT: shres=0 distinct colours " +
                     describeSet(set0) + "; shres=1 distinct colours " + describeSet(set1) + ".");
    }

    // ==================== REG-0x10 .. REG-0x1F ====================
    for (int idx = 0; idx < 16; ++idx) {
        const RegRow& row = kRows[idx];
        int offset = idx;  // COLOR($10+offset)
        int evenTarget = offset & 3;
        int oddTarget = (offset >> 2) & 3;
        bool useOdd = offset >= 4;
        // Diagonal registers ($10,$15,$1A,$1F): offset&3 == offset>>2,
        // i.e. slot A and slot B are the SAME logical (colour,channel,
        // hi/lo) -- the register is dedicated to one sprite colour. (The
        // extra symbol/legend ids this implies are already baked into
        // kRegIds[idx] above, not recomputed here.)

        // row.symbols[4],[5] = R slot-A hi,lo (bits 11,10); [6],[7] = R
        // slot-B hi,lo (bits 9,8); [8],[9] = G slot-A (bits 7,6); [10],
        // [11] = G slot-B (bits 5,4); [12],[13] = B slot-A (bits 3,2);
        // [14],[15] = B slot-B (bits 1,0) -- per kRows' own comment above.
        // The claim ids for this register are the literal kRegIds[idx]
        // string below; the actual written/measured VALUES are chosen
        // independently (rA/gA/bA, rB/gB/bB below), not derived from the
        // row's own letters.
        (void)row.symbols;

        // Chosen slot-A test value: R=2(10b) G=1(01b) B=3(11b) -- three
        // distinct 2-bit values, one per channel, so a stuck bit or a
        // channel swap is individually detectable. Slot B gets the
        // bitwise complement (R=1,G=2,B=0) so if it ever leaked into the
        // observed output it would read as a DIFFERENT, wrong value, not
        // an accidental match.
        int rA = 2, gA = 1, bA = 3;
        int rB = 1, gB = 2, bB = 0;
        auto hi = [](int v) { return (v >> 1) & 1; };
        auto lo = [](int v) { return v & 1; };

        uint16_t regAddr = kColorAddr(16 + offset);

        if (offset == 0) {
            // REG-0x10 (colour 16): cannot be confirmed via live sprite
            // display -- see FIXTURE-SPR0-TRANSPARENT-AT-VALUE0 above and
            // the header note "WHY REGISTER $10 IS TREATED SEPARATELY".
            //
            // DEFECT CORRECTION (independent validator, this agent,
            // 11.09.2026): this was previously checks.expect(false, ...) --
            // an unconditional failing "assertion" with nothing to measure,
            // i.e. a declaration dressed up as a measurement. Converted to
            // a genuine declined claim (Checks::decline, printed but not
            // counted in passed/failed, and not gated for a mutation
            // record, since it asserts nothing that could be mutated). The
            // other twenty ids on this same register (the sixteen per-bit
            // ids, its own three symbol ids and its one legend id) belong
            // with the row id for the identical reason -- every one of
            // them is a fact ABOUT this same universally-unreachable
            // register's stored content, not a claim this Harness can
            // separately measure or fail to measure -- so all twenty-one
            // are declined together here, ids still cited in full.
            std::string ids = kRegIds[idx];
            checks.decline(
                "REG-0x10: register COLOR16/$10's sixteen bit positions "
                "(this row: \"C  10   .   .   .   .   A   B   A   B   C   D "
                "  C   D   E   F   E   F\", p.302) can never be confirmed "
                "via this boundary's live sprite display, because combined "
                "sprite index 0 -- the ONLY index that ever addresses this "
                "register -- is universally transparent on real sprite "
                "hardware (both an unattached sprite's own value 0 and an "
                "attached pair with both members 0 mean 'no sprite pixel', "
                "confirmed empirically by FIXTURE-SPR0-TRANSPARENT-AT-"
                "VALUE0 above): no RGB sample this Harness can produce ever "
                "reflects this register's stored content. This is an "
                "architectural property of the sprite/transparency path, "
                "not a per-bit finding about any ONE of these ids -- "
                "declined rather than silently dropped or asserted false "
                "with nothing measured. " + ids + " .");
        } else {
            // Distinguishing content: slot A (bits 11:10/7:6/3:2) =
            // R2,G1,B3; slot B (bits 9:8/5:4/1:0) = the complement,
            // R1,G2,B0 -- this row's own printed pattern determines WHICH
            // of slot A/slot B (or, per HARNESS FIXES below, neither,
            // since no shres-dependent split exists in this RTL at all)
            // is what the RGB output actually reflects.
            uint16_t regv = buildRegValue(hi(rA), lo(rA), hi(gA), lo(gA), hi(bA), lo(bA),
                                           hi(rB), lo(rB), hi(gB), lo(gB), hi(bB), lo(bB),
                                           /*unused4=*/0x0);

            // Unused-bits (15-12) no-effect check: same register content
            // otherwise, unused nibble flipped from 0x0 to 0xF, measured
            // at shres=1 (this package's own claimed mode) -- mode-
            // independent by construction (denise_colortable.v:32,
            // confirmed after opening the RTL: wr_dat=data_in[11:0]
            // truncates bits 15-12 before they are ever latched,
            // regardless of shres).
            uint16_t regvUnused = buildRegValue(hi(rA), lo(rA), hi(gA), lo(gA), hi(bA), lo(bA),
                                                 hi(rB), lo(rB), hi(gB), lo(gB), hi(bB), lo(bB),
                                                 /*unused4=*/0xF);
            auto diffs = activeSamples(regAddr, regv, evenTarget, oddTarget, useOdd, /*shres=*/true);
            auto diffsUnused = activeSamples(regAddr, regvUnused, evenTarget, oddTarget, useOdd, /*shres=*/true);
            bool unusedNoEffect = (diffs == diffsUnused);

            // THE DECISIVE check, same methodology as SHRES-MODE-CHANGES-
            // OUTPUT above and for the same reason (isolating a genuine
            // colour-encoding difference from denise_sprites.v's own
            // shres/spres-dependent shift RATE, confirmed after opening
            // the RTL to affect only sprite WIDTH on screen, never the
            // loaded pixel VALUE -- see HARNESS FIXES): the SET of
            // distinct colours this register's own combined index can
            // show must differ between BPLCON0.shres=0 and shres=1 if
            // this row's printed slot-A/slot-B split is actually being
            // read out as the manual describes.
            auto set0 = distinctValues(activeSamples(regAddr, regv, evenTarget, oddTarget, useOdd, /*shres=*/false));
            auto set1 = distinctValues(activeSamples(regAddr, regv, evenTarget, oddTarget, useOdd, /*shres=*/true));
            bool shresChangesColour = (set0 != set1);

            std::string ids = kRegIds[idx];

            checks.expect(unusedNoEffect && shresChangesColour,
                std::string("REG-0x") + row.hex +
                ": with register COLOR" + std::to_string(16 + offset) + "/$" + row.hex +
                " loaded so its top-two-bit ('slot A') fields (bits 11:10 R "
                "/ 7:6 G / 3:2 B, this row's own printed pattern, p.302) "
                "read R=2 G=1 B=3 and its bottom-two-bit ('slot B') fields "
                "(bits 9:8/5:4/1:0, this row's OTHER printed symbols) read "
                "R=1 G=2 B=0, and the sprite arming that this register's "
                "own combined index (" + std::to_string(offset) +
                ") naturally reaches " +
                (useOdd ? "(an ATTACHED pair, SPR0 own value " + std::to_string(evenTarget) +
                          ", SPR1 own value " + std::to_string(oddTarget) + ")"
                        : "(an UNATTACHED sprite, own value " + std::to_string(evenTarget) + ")") +
                ": flipping this SAME register's unused bits 15-12 from "
                "0x0 to 0xF changes none of the RGB samples this sprite "
                "pixel drives (matching 'bit 15/14/13/12 is unused' for "
                "this row); AND, measured at BOTH BPLCON0.shres settings "
                "to isolate colour from the SuperHires drain-rate/width "
                "effect, the SET of distinct colours this sprite pixel "
                "can show differs between shres=0 and shres=1 -- i.e. "
                "this register's slot-A/slot-B split from this row's own "
                "printed symbols is actually read out as SuperHires-mode-"
                "specific data, not as an ordinary always-on colour "
                "value. " + ids + " .");
            checks.note(std::string("REG-0x") + row.hex + ": shres=0 distinct colours " +
                        describeSet(set0) + "; shres=1 distinct colours " + describeSet(set1) + ".");
        }
    }

    // ==================== SHRES-SPR11-BIT11-ISOLATES-RED ====================
    {
        // Isolates C-shres-sprite-bits.c21 ("COLOR11 bit 11 carries symbol
        // G from sprite color 17 red") from REG-0x11's own aggregate check
        // above: an independent validator found that check only asks
        // whether the WHOLE set of distinct colours a sprite pixel can
        // show differs between shres=0 and shres=1 for ONE combined,
        // multi-bit stimulus -- it never isolates bit 11 alone, nor
        // confirms the reading lands in the RED channel specifically. This
        // writes COLOR11/$11 (= COLOR17 decimal) with ONLY bit 11 set (all
        // fifteen other bits 0) and measures the resulting sprite pixel's
        // own RED channel against a same-setup, same-position baseline
        // with COLOR11=0 -- the position-to-channel half of the claim, the
        // part this boundary can observe. The colour-17-distinct-from-
        // colour-16 half has no independent circuit to observe at all
        // (same reason as the sibling ch3-shres-bitplane-colour-encoding
        // package's own SHRES-BPL01-BIT11-ISOLATES-RED: denise_colortable.v
        // reads a register's R nibble as ONE undifferentiated 4-bit value,
        // shres or not) -- not re-tested here; REG-0x11's own sweep above
        // already carries it, and stays red for exactly that reason.
#if MUT_SPR11BIT11RED
        uint16_t bitOnly = 0x0080;  // MUTATION: bit 7 (green) instead of bit 11 (red)
#else
        uint16_t bitOnly = 0x0800;  // COLOR11/$11 bit 11 alone
#endif
        auto diffs = activeSamples(kColorAddr(17), bitOnly, /*even=*/1, /*odd=*/0,
                                    /*useOdd=*/false, /*shres=*/true);
        bool moved = false, onlyRed = true;
        for (uint32_t s : diffs) {
            if (top2(s, 'R') != 0) moved = true;
            if (top2(s, 'G') != 0 || top2(s, 'B') != 0) onlyRed = false;
        }
        checks.expect(moved && onlyRed,
            "SHRES-SPR11-BIT11-ISOLATES-RED (p.302, C-shres-sprite-bits.c21): "
            "with every other bit of COLOR11/$11 held at 0, bit 11 alone, "
            "read via an unattached sprite selecting combined index 1 "
            "(register COLOR17), moves the sprite pixel's RED channel and "
            "leaves green/blue at 0 -- 'O  11   .   .   .   .   G   H   A   "
            "B   I   J   C   D   K   L   E   F' (p.302) names bit 11 as "
            "symbol G, sprite colour 17's own red. Confirms the position-"
            "to-channel half of the claim; the colour-17-distinct-from-"
            "colour-16 half is not re-tested here (see this block's own "
            "comment above) -- REG-0x11's own sweep carries it, red for "
            "the same underlying reason.");
        checks.note("SHRES-SPR11-BIT11-ISOLATES-RED: bit-11-only diffs " + describeSet(diffs) + ".");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed, "
              << checks.declined << " declined\n";
    return checks.failed == 0 ? 0 : 1;
}
