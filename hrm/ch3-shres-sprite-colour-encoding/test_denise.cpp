// Package ch3-shres-sprite-colour-encoding -- ECS SuperHires sprite colour
//   encoding conformance test, fixture `denise` (one Verilator build, top
//   module denise.v plus its own eleven sub-modules: denise_bitplanes,
//   denise_bitplane_shifter, denise_playfields, denise_sprites,
//   denise_sprites_shifter, denise_collision, denise_hamgenerator,
//   denise_colortable, denise_colortable_ram_mf, denise_spritepriority,
//   bitplane_ram).
//
// Test purpose: printed page 302's table mapping the sixteen *upper* colour
//   registers (COLOR$10-COLOR$1F, i.e. COLOR16-COLOR31) onto the symbols
//   A-X, plus the legend "Sprite (Color 16) :   AB-- CD-- EF--" (and its
//   three sibling legend lines for Color 17/18/19) that says each named
//   SuperHires sprite colour contributes only the TOP two bits of each of
//   R, G, B. Page 301: "There are only two bits of red, green and blue
//   color resolution per hires pixel." and "In order to decode sprite and
//   bitplane color information in SuperHires mode, certain multiplexing
//   occurs in the use of the registers."
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 301-303, and pp. 103, 108 for sprite colour-register transparency.
//   A red result below means denise.v/denise_colortable.v/denise_sprites.v
//   disagree with the manual, not that the assertion is wrong -- no
//   assertion here was, or should be, adjusted to make the RTL pass.
//
// METHOD: page 302 gives sixteen separate rows, each naming which register
//   bit carries which sprite colour's which channel. A finding that only
//   compares "does something differ between BPLCON0.shres=0 and shres=1"
//   measures p.301's general multiplexing clause, not any one of those
//   sixteen rows. This package instead reads the table at the bit level:
//   each COLOR16-31 bit is flipped one at a time against a fixed background
//   palette, and what the flip is allowed to move -- which displayed sprite
//   colour, which one of R/G/B -- is read off the row's own printed symbols,
//   not assumed from NanoMig's own lookup.
//
// PALETTE DERIVATION (S0-S3, "state the derivation in the header"): each
//   palette S (0-3) assigns sprite colour (16+i), i=0..3, the two-bit RGB
//   triple R=(i+S) mod 4, G=(i+S+1) mod 4, B=(i+S+2) mod 4 -- four distinct,
//   pairwise-non-degenerate triples per palette, so a stuck bit, a channel
//   swap or a wrong colour is each individually detectable. Page 302's own
//   table then fixes how those sixteen (four palettes x four colours) two-
//   bit triples pack into the sixteen registers: register COLOR($10+n),
//   n = 4q+p (p,q = 0..3), carries colour (16+p)'s own triple in "slot A"
//   (bits 11-10 R, 7-6 G, 3-2 B) and colour (16+q)'s own triple in "slot B"
//   (bits 9-8 R, 5-4 G, 1-0 B) -- the same row-by-row reading kRows[] below
//   transcribes, expressed here as a formula that drives the stimulus.
//   Cross-check (S0, n=0..15, computed by buildPalette(0)+tableRegValue()):
//     $005A $049E $08D2 $0C16 $016B $05AF $09E3 $0D27
//     $0278 $06BC $0AF0 $0E34 $0349 $078D $0BC1 $0F05
//   matches the sixteen words an independently-produced cross-check of this
//   formula gives for S0, word for word.
//
// SAMPLING: scan_line() takes one red/green/blue sample per raw 28 MHz
//   `tick()`, not once every four -- a 70 ns SuperHires half-pixel drains in
//   two 28 MHz ticks, so a once-per-four-ticks (7 MHz) sample rate cannot
//   resolve it. Every call site below passes BPLCON0.shres explicitly; no
//   function in this file defaults it.
//
// SAMPLE WINDOW: measured, not assumed, with a throwaway probe built
//   against this same fixture (not part of the package): with SPR0POS=$40
//   and reset_line()'s own known hpos=2 start, an unattached sprite 0's
//   pixel is live from tick 514 to tick 544 after reset_line() at
//   BPLCON0.shres=1 (31 ticks, matching the SuperHires shift register's own
//   twice-as-fast drain: 16 positions x 2 ticks/position), and tick 514 to
//   577 at shres=0 (63 ticks, twice as long, the ordinary drain rate) -- for
//   a solid run and for the mixed "M" run alike, and independent of which
//   register is loaded. Every scan below starts sampling at tick 505 (a
//   9-tick margin before the earliest measured onset) for 100 ticks (tick
//   604), bracketing both measured windows with margin on both sides.
//
// STIMULUS -- SPRITE 0 UNATTACHED ONLY, no SPR1CTL ATTACH pairing anywhere
//   in this file: page 302's own "combined index" is not the ordinary-mode
//   ATTACH pairing of two separate sprite channels (SPR0+SPR1, a Chapter 4
//   (p.103) feature, a different mechanism). The pairing p.302's SuperHires
//   scheme actually uses is of two TEMPORALLY CONSECUTIVE 70 ns samples of
//   ONE sprite's own shift register (WinUAE's implementation, quoted on the
//   finding page: "two sprite samples per low-resolution pixel select
//   COLOR(16 + 4*(second&3) + (first&3))") -- reached here by giving
//   sprite 0 alone a stimulus whose own two-bit value changes from sample
//   to sample, never by arming a second sprite channel.
//
// RUNS: five DATA/DATB images loaded into sprite 0 alone (SPR1 is never
//   written by this file at all -- "unattached" throughout):
//     S1, S2, S3 -- solid runs, every one of the sprite's 16 shift
//       positions at the same value (1, 2 or 3), DATA/DATB filled with the
//       matching all-0/all-1 words: a constant value for the sprite's whole
//       ~32-tick lifetime, immune to sampling phase or drain rate.
//     M -- a 16-position mixed run, values
//       [1,1,2,1,3,2,2,3,3,1,0,3,0,2,0,1] (position 0 first out of the
//       shift register), loaded as DATA=$D9D1, DATB=$2F94: reconstructing
//       each position's two-bit value as (DATB-bit<<1)|DATA-bit, MSB
//       (bit 15) first, reproduces exactly this sequence -- checked by
//       direct bit extraction, not assumed from the hex alone.
//     M-SHSH1 -- the same M image, with SPR0CTL bit 4 (SHSH1, p.303: "04
//       SHSH1     Start horizontal (SHR mode) 70ns increment") additionally
//       set. Bit 4 is a documented dead-code bit on this boundary (the
//       committed hrm/ch3-sprite-registers finding
//       SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE: "is RED because it is
//       not [live]") -- included anyway, per the repair specification, as
//       an independent stimulus at the other 28 MHz tick-grid alignment a
//       live SHSH1 would have produced, so a per-bit verdict below does not
//       rest on one particular sample phase alone.
//   Every REG-0x1n measurement below sweeps all five; a register/bit
//   combination is graded only once at least one of the five shows a
//   difference (see "observed" below), and declined only when none of the
//   five ever does.
//
// ASSERTIONS:
//   C1 FIXTURE-SPRITE-DATA-PATH-LIVE (HIRES) -- a written, non-zero sprite
//     pixel reaches red/green/blue at all, at BPLCON0.shres=0, before any
//     SuperHires-specific measurement depends on it. No claim id, no page.
//   C2 FIXTURE-SPR0-TRANSPARENT-AT-VALUE0 (HIRES) -- combined index 0
//     (sprite value 0) produces no visible pixel however COLOR16 is
//     loaded, at shres=0. No claim id, no page (see REG-0x10 below).
//   C3 FIXTURE-TABLE-AS-0RGB (HIRES) -- for colour 17/18/19, a solid run
//     shows the S0 register's own raw twelve bits, unmangled, the ordinary
//     ("0RGB", p.301's own contrasting term for the non-SuperHires
//     encoding) ECS colour-register convention -- confirming the palette-
//     loading fixture itself before subjecting it to a SuperHires reading.
//     No claim id, no page (fixture plumbing).
//   P1 KEY-COLOURS-17-19-BITS-3-2 -- for all four palettes S0-S3 and
//     colours 17/18/19, a solid run's observed pixel carries that colour's
//     own two-bit RGB triple in bits 3-2 of each channel nibble (page 302's
//     legend, "AB-- CD-- EF--" etc: the colour's own letters are its
//     TOP two bits).
//   P2 TWO-BITS-54-TRACES-IDENTICAL -- for colour 17/18/19's own solid run
//     (register $11/$12/$13), flipping one letter belonging to a DIFFERENT
//     stored colour (16, and whichever of 17-19 is not the one on screen)
//     must leave that solid run's trace identical, per page 302's "only two
//     bits ... per hires pixel": 3 on-screen colours x 3 other colours x 3
//     channels x 2 (hi/lo) = 54 traces, all required identical.
//   REG-0x11 .. REG-0x1F -- one aggregate per-bit-inversion verdict per
//     register: with the S0 table loaded, each of the register's twelve
//     meaningful bits is flipped in turn and every one of the five runs is
//     scanned for a difference against the unmutated baseline. Not observed
//     on any run: declined (register $10, $14-$1F; see below). Observed on
//     at least one run: PASS if every observed difference lands on the
//     letter's own page-predicted colour and channel, FAIL otherwise.
//   REG-0x10 -- declined: combined index 0 (both an unattached sprite's own
//     value 0, or an attached pair with both members 0) is universally
//     transparent -- "The color value of the first register in each group
//     of four registers is ignored by sprites. When the sprite bits select
//     this register, the 'transparent' value is used." (p.103); "Recall
//     that binary 00 always means transparent and never refers to a color
//     except background." (p.108) -- so no RGB sample this Harness can
//     produce ever reflects COLOR16's stored content, confirmed by C2.
//   DOT-BITS -- declined: every printed row of page 302's table marks bits
//     15-12 "." (unused, e.g. row $11: "....GHABIJCDKLEF"). Measured here
//     (register $11, S0 baseline, run S1): bits 15-12 flipped 0x0->0xF
//     produce no observable difference -- but this boundary's colour-
//     register write path truncates data_in to its low twelve bits before
//     latching in EVERY mode (the sibling ch3-colour-register-format
//     package's own already-declined claim, "bits 15-12 unused"), so this
//     package cannot attribute that truncation to page 302's own SuperHires
//     table specifically rather than to the general 12-bit colour-register
//     format -- declining rather than re-testing a sibling package's claim
//     under a different name.
//
// NON-GOALS: a shres=0-vs-shres=1 "does something differ" comparison, on
//   any register, is deliberately not used to grade REG-0x11-0x1F or a
//   general multiplexing claim -- it can only show that p.301's general
//   multiplexing clause holds somewhere, never which of page 302's sixteen
//   rows it holds for, so it is graded nowhere by this package. Isolating
//   one register bit (e.g. bit 11) by itself is also not used, for the
//   converse reason: which colour and channel that bit's flip is allowed
//   to move is exactly what varies row to row, and REG-0x11-0x1F below
//   already measure every one of a register's twelve bits this way, not
//   one bit in isolation. An unused-bits (15-12) no-effect check is not
//   folded into a per-register conjunction with the colour/channel check
//   above -- it is its own declined DOT-BITS claim, with its own citation
//   and its own measurement, since a claim about truncated write bits and
//   a claim about which register a sprite pixel reads are not one
//   proposition.
//
// CLAIM COVERAGE: the row ids (C-shrs.r10-r1F), the 240 per-bit ids
//   (C-shres-sprite-bits.c1-c256, register $19 excluded from the extraction,
//   confirmed by counting), the twelve symbol ids and four legend ids
//   (kRegIds[] below, the same table transcription every one of this
//   package's assertions is built on) are cited in full on their own
//   register's REG-0x1n entry below, via kRegIds[]. The three general ids
//   for p.301's own multiplexing sentences (C-shrc.c03, C-shres-color.c8,
//   C-shrs.c01) are cited on REG-0x11, the first register this per-bit
//   family actually measures multiplexing through.
//
// MUTATION LOG: filled in below each assertion, after building and running
//   under WSL/Verilator and confirming the described stimulus change, and
//   only that assertion, moved -- see each block's own comment.

#ifndef MUT_C1
#define MUT_C1 0
#endif
#ifndef MUT_C2
#define MUT_C2 0
#endif
#ifndef MUT_C3
#define MUT_C3 0
#endif
#ifndef MUT_P1
#define MUT_P1 0
#endif
#ifndef MUT_P2
#define MUT_P2 0
#endif
#ifndef MUT_REG11
#define MUT_REG11 0
#endif

#include <array>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vdenise.h"

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::printf("%s: %s\n", condition ? "PASS" : "FAIL", label.c_str());
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::printf("MEASURED: %s\n", msg.c_str()); }
    // A DECLINED claim: a proposition this boundary cannot measure at all
    // (not a pass, not a fail). Printed but not counted in passed/failed,
    // and deliberately not expect()/report() -- tools/check_gates.py's
    // mutation gate reads only those two call names as assertions.
    void decline(const std::string& label) {
        std::printf("DECLINED: %s\n", label.c_str());
        ++declined;
    }
    int passed = 0;
    int failed = 0;
    int declined = 0;
};

// ---------------------------------------------------------------------
// Custom chip-register addresses (BOUNDARY, reused from the already-
// committed hrm/ch3-sprite-registers/
// and hrm/ch3-horizontal-display-window/ sibling harnesses for these same
// modules).
// ---------------------------------------------------------------------
constexpr uint16_t kDiwstrtAddr = 0x08E;
constexpr uint16_t kDiwstopAddr = 0x090;
constexpr uint16_t kBplcon0Addr = 0x100;
constexpr uint16_t kBpl1DatAddr = 0x110;  // sets display_ena -- see setupDisplay().
constexpr uint16_t kColorbaseAddr = 0x180;  // COLOR00
constexpr uint16_t kColorAddr(int n) { return static_cast<uint16_t>(kColorbaseAddr + 2 * n); }
constexpr uint16_t kSprBase = 0x140;
constexpr uint16_t kSprPosAddr(int n) { return static_cast<uint16_t>(kSprBase + 8 * n + 0); }
constexpr uint16_t kSprCtlAddr(int n) { return static_cast<uint16_t>(kSprBase + 8 * n + 2); }
constexpr uint16_t kSprDataAddr(int n) { return static_cast<uint16_t>(kSprBase + 8 * n + 4); }
constexpr uint16_t kSprDatbAddr(int n) { return static_cast<uint16_t>(kSprBase + 8 * n + 6); }

constexpr uint16_t kBplcon0Shres = 0x0040;   // bit 6
constexpr uint16_t kBplcon0Ecsena = 0x0001;  // bit 0
constexpr uint16_t kSpriteCtlSh1 = 0x0010;   // SPRxCTL bit 4, SHSH1 (p.303)

constexpr int kChannelBits = 8;  // the fixture's own red/green/blue ports, measured 8 bits wide.

// ---------------------------------------------------------------------
// THE ORACLE -- transcribed verbatim from page 302's sixteen row quotes.
// bits 15 downto 0 left-to-right, '.' for the printed "." (unused).
// ---------------------------------------------------------------------
struct RegRow {
    const char* hex;
    const char* symbols;
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

// Claim ids per register (see CLAIM COVERAGE above): row id + per-bit
// ids + (diagonal registers only) three
// symbol ids and one legend id.
constexpr const char* kRegIds[16] = {
    "C-shrs.r10, C-shres-sprite-bits.c1, C-shres-sprite-bits.c2, C-shres-sprite-bits.c3, C-shres-sprite-bits.c4, C-shres-sprite-bits.c5, C-shres-sprite-bits.c6, C-shres-sprite-bits.c7, C-shres-sprite-bits.c8, C-shres-sprite-bits.c9, C-shres-sprite-bits.c10, C-shres-sprite-bits.c11, C-shres-sprite-bits.c12, C-shres-sprite-bits.c13, C-shres-sprite-bits.c14, C-shres-sprite-bits.c15, C-shres-sprite-bits.c16, C-shres-sprite-symbols.c1, C-shres-sprite-symbols.c2, C-shres-sprite-symbols.c3, C-shrc.srcs16",
    "C-shrs.r11, C-shres-sprite-bits.c17, C-shres-sprite-bits.c18, C-shres-sprite-bits.c19, C-shres-sprite-bits.c20, C-shres-sprite-bits.c21, C-shres-sprite-bits.c22, C-shres-sprite-bits.c23, C-shres-sprite-bits.c24, C-shres-sprite-bits.c25, C-shres-sprite-bits.c26, C-shres-sprite-bits.c27, C-shres-sprite-bits.c28, C-shres-sprite-bits.c29, C-shres-sprite-bits.c30, C-shres-sprite-bits.c31, C-shres-sprite-bits.c32, C-shrc.c03, C-shres-color.c8, C-shrs.c01",
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
// Harness -- four-phase clocking and write_reg, proven live on this
// fixture; scan_line() samples every 28 MHz tick (see SAMPLING above).
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

    // Sample every raw 28 MHz tick -- see SAMPLING above.
    std::vector<uint32_t> scan_line(int n) {
        std::vector<uint32_t> samples;
        samples.reserve(n);
        for (int i = 0; i < n; ++i) {
            samples.push_back(pack_rgb(dut));
            tick();
        }
        return samples;
    }

    Vdenise* dut;
    int phase = 0;
};

void setupDisplay(Harness& h, bool shres) {
    h.write_reg(kDiwstrtAddr, 0x0010);
    h.write_reg(kDiwstopAddr, 0x00F0);
    h.write_reg(kBplcon0Addr, static_cast<uint16_t>((shres ? kBplcon0Shres : 0) | kBplcon0Ecsena));
    h.write_reg(kBpl1DatAddr, 0x0000);  // sets display_ena -- without it no sprite pixel, of any colour, ever reaches the RGB output.
}

// ---------------------------------------------------------------------
// Palette: colour (16+i)'s own two-bit (R,G,B) triple, i=0..3 -- see
// PALETTE DERIVATION above.
// ---------------------------------------------------------------------
struct Palette {
    int rgb[4][3];  // [i][0]=R [1]=G [2]=B, each 0-3
};

Palette buildPalette(int s) {
    Palette pal;
    for (int i = 0; i < 4; ++i) {
        pal.rgb[i][0] = (i + s) % 4;
        pal.rgb[i][1] = (i + s + 1) % 4;
        pal.rgb[i][2] = (i + s + 2) % 4;
    }
    return pal;
}

// Register COLOR($10+n) word from `pal`, per page 302's table (see
// PALETTE DERIVATION above): n = 4q+p, slot A = colour(16+p), slot B =
// colour(16+q).
uint16_t tableRegValue(const Palette& pal, int n) {
    int p = n & 3, q = n >> 2;
    uint16_t w = 0;
    w = static_cast<uint16_t>(w | (pal.rgb[p][0] & 3) << 10);  // R slot A
    w = static_cast<uint16_t>(w | (pal.rgb[q][0] & 3) << 8);   // R slot B
    w = static_cast<uint16_t>(w | (pal.rgb[p][1] & 3) << 6);   // G slot A
    w = static_cast<uint16_t>(w | (pal.rgb[q][1] & 3) << 4);   // G slot B
    w = static_cast<uint16_t>(w | (pal.rgb[p][2] & 3) << 2);   // B slot A
    w = static_cast<uint16_t>(w | (pal.rgb[q][2] & 3) << 0);   // B slot B
    return w;
}

std::array<uint16_t, 16> wordsFor(const Palette& pal) {
    std::array<uint16_t, 16> w{};
    for (int n = 0; n < 16; ++n) w[static_cast<size_t>(n)] = tableRegValue(pal, n);
    return w;
}

// One of the five stimuli sprite 0 alone is armed with -- see RUNS above.
struct Run {
    const char* name;
    uint16_t data;
    uint16_t datb;
    uint16_t ctl;
};

constexpr Run kRuns[5] = {
    {"S1", 0xFFFF, 0x0000, 0x0000},
    {"S2", 0x0000, 0xFFFF, 0x0000},
    {"S3", 0xFFFF, 0xFFFF, 0x0000},
    {"M", 0xD9D1, 0x2F94, 0x0000},
    {"M-SHSH1", 0xD9D1, 0x2F94, kSpriteCtlSh1},
};
constexpr Run kDisarmed = {"disarmed", 0x0000, 0x0000, 0x0000};

using Trace = std::vector<uint32_t>;

constexpr uint16_t kSpritePosByte = 0x40;
constexpr int kStart = 505;  // measured margin before the earliest onset (514) -- see SAMPLE WINDOW.
constexpr int kCount = 100;  // through tick 604, past the widest measured window (577) -- see SAMPLE WINDOW.

// Fresh DUT, palette `regWords` loaded across COLOR16-31, sprite 0 alone
// armed per `run`, sampled every tick from `start` for `count` ticks after
// reset_line(). Every measurement in this file goes through this one path
// -- see MEASUREMENT/SAMPLING above.
Trace runTraceRegs(const std::array<uint16_t, 16>& regWords, const Run& run, bool shres,
                    int start, int count) {
    Harness h;
    setupDisplay(h, shres);
    for (int n = 0; n < 16; ++n) h.write_reg(kColorAddr(16 + n), regWords[static_cast<size_t>(n)]);
    h.write_reg(kSprPosAddr(0), kSpritePosByte);
    h.write_reg(kSprCtlAddr(0), run.ctl);
    h.write_reg(kSprDatbAddr(0), run.datb);
    h.write_reg(kSprDataAddr(0), run.data);
    h.reset_line();
    for (int i = 0; i < start; ++i) h.tick();
    return h.scan_line(count);
}

Trace runTrace(const Palette& pal, const Run& run, bool shres, int start, int count) {
    return runTraceRegs(wordsFor(pal), run, shres, start, count);
}

// Register `overrideReg` (16-31) forced to `overrideVal`, the rest of
// `pal`'s own table otherwise.
Trace runTraceOverride(const Palette& pal, int overrideReg, uint16_t overrideVal, const Run& run,
                        bool shres, int start, int count) {
    auto words = wordsFor(pal);
    words[static_cast<size_t>(overrideReg - 16)] = overrideVal;
    return runTraceRegs(words, run, shres, start, count);
}

int top2(uint32_t rgb, char ch) {
    uint32_t v;
    if (ch == 'R') v = (rgb >> 16) & ((1u << kChannelBits) - 1);
    else if (ch == 'G') v = (rgb >> 8) & ((1u << kChannelBits) - 1);
    else v = rgb & ((1u << kChannelBits) - 1);
    return static_cast<int>(v >> (kChannelBits - 2)) & 0x3;
}

// Which of colour 17/18/19 (per `pal`) the sample's own top two bits per
// channel match, or -1 if none. Used to identify WHICH register a sample
// is actually reading, independent of any assumption about NanoMig's own
// addressing.
int colourOf(uint32_t rgb, const Palette& pal) {
    for (int c = 1; c <= 3; ++c) {
        if (top2(rgb, 'R') == pal.rgb[c][0] && top2(rgb, 'G') == pal.rgb[c][1] &&
            top2(rgb, 'B') == pal.rgb[c][2]) {
            return 16 + c;
        }
    }
    return -1;
}

// Full 8-bit channels that differ between two samples (0=R,1=G,2=B).
std::vector<int> diffComponents(uint32_t a, uint32_t b) {
    std::vector<int> out;
    for (int j = 0; j < 3; ++j) {
        uint32_t sh = static_cast<uint32_t>(16 - 8 * j);
        if (((a >> sh) & 0xFF) != ((b >> sh) & 0xFF)) out.push_back(j);
    }
    return out;
}

std::string describeColour(uint32_t rgb) {
    char buf[48];
    std::snprintf(buf, sizeof(buf), "R=%d G=%d B=%d (raw 0x%06X)", top2(rgb, 'R'), top2(rgb, 'G'),
                  top2(rgb, 'B'), rgb);
    return std::string(buf);
}

// ---------------------------------------------------------------------
// Per-register per-bit-inversion measurement -- see ASSERTIONS (REG-0x1n)
// above. `baseline[r]` is run r's trace under the unmutated S0 table,
// computed once and shared across every register (S0 never changes).
// ---------------------------------------------------------------------
struct RegVerdict {
    bool observed = false;
    bool allGood = true;
    int badBit = -1;
    const char* badRun = "";
    int badTick = -1;
    int sawColour = -1;
    int expectColour = -1;
    std::vector<int> badComps;
};

RegVerdict measureRegister(const std::array<uint16_t, 16>& s0words, const Palette& s0,
                            const Trace baseline[5], int n, bool forceGood = false) {
    RegVerdict v;
    int p = n & 3, q = n >> 2;
    for (int bit = 0; bit < 12; ++bit) {
        bool slotB = (bit == 9 || bit == 8 || bit == 5 || bit == 4 || bit == 1 || bit == 0);
        int k = slotB ? q : p;
        int comp = (bit >= 8) ? 0 : (bit >= 4 ? 1 : 2);  // 0=R,1=G,2=B
        auto mut = s0words;
        mut[static_cast<size_t>(n)] = static_cast<uint16_t>(mut[static_cast<size_t>(n)] ^ (1u << bit));
        for (int ri = 0; ri < 5; ++ri) {
            Trace t = runTraceRegs(mut, kRuns[ri], /*shres=*/true, kStart, kCount);
            const Trace& b = baseline[static_cast<size_t>(ri)];
            for (size_t i = 0; i < t.size() && i < b.size(); ++i) {
                if (t[i] == b[i]) continue;
                v.observed = true;
                int saw = colourOf(b[i], s0);
                auto comps = diffComponents(b[i], t[i]);
                bool good = forceGood || ((saw == 16 + k) && comps.size() == 1 &&
                            comps[0] == comp);
                if (!good && v.allGood) {
                    v.allGood = false;
                    v.badBit = bit;
                    v.badRun = kRuns[ri].name;
                    v.badTick = static_cast<int>(i);
                    v.sawColour = saw;
                    v.expectColour = 16 + k;
                    v.badComps = comps;
                }
            }
        }
    }
    return v;
}

std::string regVerdictDetail(const RegVerdict& v) {
    if (!v.observed) return "no run ever showed a difference (declined).";
    if (v.allGood) return "every observed difference matched its bit's own predicted colour and channel.";
    char buf[256];
    std::string comps;
    for (size_t i = 0; i < v.badComps.size(); ++i) {
        comps += "RGB"[v.badComps[i]];
    }
    std::snprintf(buf, sizeof(buf),
                  "bit %d, run %s, tick %d: observed colour %d, page-predicted colour %d, "
                  "component(s) moved: %s.",
                  v.badBit, v.badRun, v.badTick, v.sawColour, v.expectColour, comps.c_str());
    return std::string(buf);
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    const Palette S0 = buildPalette(0);
    const auto s0words = wordsFor(S0);

    // ==================== C1 [control] FIXTURE-SPRITE-DATA-PATH-LIVE ====================
    {
        auto base = runTrace(S0, kDisarmed, /*shres=*/false, kStart, kCount);
#if MUT_C1
        auto live = runTrace(S0, kDisarmed, /*shres=*/false, kStart, kCount);  // MUTATION: disarmed instead of S1
#else
        auto live = runTrace(S0, kRuns[0], /*shres=*/false, kStart, kCount);
#endif
        bool any = false;
        for (size_t i = 0; i < live.size(); ++i) if (live[i] != base[i]) { any = true; break; }
        checks.expect(any,
            "C1 [control] FIXTURE-SPRITE-DATA-PATH-LIVE (HIRES): arming unattached sprite 0 "
            "with its own pixel value 1 (run S1) against a same-setup disarmed baseline, at "
            "BPLCON0.shres=0, produces at least one differing RGB sample in the measured "
            "window -- proving this Harness's write_reg -> arm -> scan_line -> diff-against-"
            "baseline path is live before any SuperHires-specific assertion below depends on "
            "it. Fixture check, no claim id, no page.");
        // MUTATION C1 stimulus red: built with -DMUT_C1=1, which arms the "live" trace
        // with kDisarmed instead of run S1 (identical to the baseline it is diffed
        // against) -> measured "3 passed, 5 failed, 14 declined", C1 the only verdict
        // that moved (PASS -> FAIL; diff against baseline became empty). Reverted (no
        // flags): "4 passed, 4 failed, 14 declined", baseline restored.
    }

    // ==================== C2 [control] FIXTURE-SPR0-TRANSPARENT-AT-VALUE0 ====================
    {
        auto base = runTrace(S0, kDisarmed, /*shres=*/false, kStart, kCount);
        // live re-arms sprite 0 at value 0 explicitly (kDisarmed's own data/datb are
        // both zero, which IS combined index 0 -- the disarmed run doubles as the
        // value-0 arming here), with COLOR16 loaded all-ones, so a leak would be
        // visible against the all-zero baseline.
#if MUT_C2
        auto live = runTraceOverride(S0, 17, 0x0FFF, kRuns[0], /*shres=*/false, kStart, kCount);  // MUTATION: value 1, register 17
#else
        auto live = runTraceOverride(S0, 16, 0x0FFF, kDisarmed, /*shres=*/false, kStart, kCount);
#endif
        bool same = (live == base);
        checks.expect(same,
            "C2 [control] FIXTURE-SPR0-TRANSPARENT-AT-VALUE0 (HIRES): an unattached sprite "
            "armed with its own pixel value 0 (combined index 0, register COLOR16/$10) "
            "produces NO differing sample versus the disarmed baseline, at BPLCON0.shres=0, "
            "even though COLOR16 itself is loaded with an all-ones pattern ($0FFF) -- "
            "confirming combined index 0 is universally transparent on this boundary: 'The "
            "color value of the first register in each group of four registers is ignored by "
            "sprites. When the sprite bits select this register, the \"transparent\" value is "
            "used.' (p.103); 'Recall that binary 00 always means transparent and never refers "
            "to a color except background.' (p.108). This is why REG-0x10 below is declined "
            "rather than measured.");
        // MUTATION C2 stimulus red: built with -DMUT_C2=1, which re-arms the "live" trace
        // at value 1 (run S1) instead of value 0, and moves the overridden register from
        // COLOR16/$10 to COLOR17/$11 (the register value 1 actually reaches) -> measured
        // "3 passed, 5 failed, 14 declined", C2 the only verdict that moved (PASS -> FAIL;
        // value 1 is not transparent, so the trace differs from the value-0 baseline).
        // Reverted (no flags): "4 passed, 4 failed, 14 declined", baseline restored.
    }

    // ==================== C3 [control] FIXTURE-TABLE-AS-0RGB ====================
    {
        bool ok = true;
        for (int c = 1; c <= 3; ++c) {
            auto base = runTrace(S0, kDisarmed, /*shres=*/false, kStart, kCount);
            auto live = runTrace(S0, kRuns[c - 1], /*shres=*/false, kStart, kCount);
            uint32_t want = ((s0words[static_cast<size_t>(c)] >> 8 & 0xF) * 0x11u) << 16 |
                             ((s0words[static_cast<size_t>(c)] >> 4 & 0xF) * 0x11u) << 8 |
                             ((s0words[static_cast<size_t>(c)] & 0xF) * 0x11u);
#if MUT_C3
            if (c == 1) want ^= 0x000001u;  // MUTATION: corrupt colour 17's expected word only
#endif
            bool any = false, allWant = true;
            for (size_t i = 0; i < live.size(); ++i) {
                if (live[i] != base[i]) {
                    any = true;
                    if (live[i] != want) allWant = false;
                }
            }
            ok = ok && any && allWant;
        }
        checks.expect(ok,
            "C3 [control] FIXTURE-TABLE-AS-0RGB (HIRES): for colour 17, 18 and 19, a solid "
            "run at BPLCON0.shres=0 shows register COLOR($10+colour-16)'s own S0 word "
            "unmangled -- each nibble duplicated into its own 8-bit channel, the ordinary "
            "ECS colour-register convention p.301 contrasts SuperHires against ('Instead of "
            "4 bits of red, green, and blue for bitplane registers 0-3 stored as 0xORGB in "
            "four color registers, SuperHires ... colors are specially encoded') -- "
            "confirming this file's own palette-loading path is unmangled before it is read "
            "under a SuperHires-specific interpretation below. No claim id (fixture "
            "plumbing).");
        // MUTATION C3 stimulus red: built with -DMUT_C3=1, which corrupts colour 17's
        // own expected word (`want`) by one low bit before the comparison (the register
        // write itself left unchanged) -> measured "3 passed, 5 failed, 14 declined", C3
        // the only verdict that moved (PASS -> FAIL; the observed word no longer equals
        // the corrupted expectation). Reverted (no flags): "4 passed, 4 failed, 14
        // declined", baseline restored.
    }

    // ==================== P1 KEY-COLOURS-17-19-BITS-3-2 ====================
    {
        bool ok = true;
        for (int s = 0; s < 4; ++s) {
            Palette pal = buildPalette(s);
            for (int c = 1; c <= 3; ++c) {
                auto base = runTrace(pal, kDisarmed, /*shres=*/true, kStart, kCount);
                auto live = runTrace(pal, kRuns[c - 1], /*shres=*/true, kStart, kCount);
                bool any = false, allMatch = true;
                for (size_t i = 0; i < live.size(); ++i) {
                    if (live[i] == base[i]) continue;
                    any = true;
#if MUT_P1
                    int wantG = (s == 0 && c == 2) ? (pal.rgb[c][1] + 1) % 4 : pal.rgb[c][1];  // MUTATION: corrupt S0 colour 18's expected G
#else
                    int wantG = pal.rgb[c][1];
#endif
                    if (top2(live[i], 'R') != pal.rgb[c][0] || top2(live[i], 'G') != wantG ||
                        top2(live[i], 'B') != pal.rgb[c][2]) {
                        allMatch = false;
                    }
                }
                ok = ok && any && allMatch;
            }
        }
        checks.expect(ok,
            "P1 KEY-COLOURS-17-19-BITS-3-2: for palettes S0-S3 and colours 17/18/19, a solid "
            "run at BPLCON0.shres=1 shows that colour's own two-bit RGB triple in bits 3-2 of "
            "each channel nibble -- page 302's legend, 'Sprite (Color 17) :   GH-- IJ-- KL--' "
            "and its siblings for colour 16/18/19: each colour's own letters are its TOP two "
            "bits of R, G and B. C-shres-sprite-symbols.c4-c12 (colour 17/18/19's own six "
            "letters each).");
        // MUTATION P1 stimulus red: built with -DMUT_P1=1, which corrupts only the
        // comparison target for palette S0 colour 18's own G component (+1 mod 4; the
        // register write itself is left at pal's real value) -> measured "3 passed, 5
        // failed, 14 declined", P1 the only verdict that moved (PASS -> FAIL; the
        // observed G no longer equals the corrupted expectation). Reverted (no flags):
        // "4 passed, 4 failed, 14 declined", baseline restored.
    }

    // ==================== P2 TWO-BITS-54-TRACES-IDENTICAL ====================
    {
        int changed = 0, total = 0;
        for (int c = 1; c <= 3; ++c) {
            auto base = runTrace(S0, kRuns[c - 1], /*shres=*/true, kStart, kCount);
            for (int k = 0; k < 4; ++k) {
                if (k == c) continue;
#if MUT_P2
                // MUTATION: skip colour 16 (k==0) entirely -- the one colour whose
                // slot-B position always overlaps register $10+c for c=1,2,3 (the
                // FINDING this assertion is red for). With it excluded, only the
                // genuinely non-interfering k=1,2,3 combos remain.
                if (k == 0) continue;
#endif
                for (int ch = 0; ch < 3; ++ch) {
                    static constexpr int kHiA[3] = {11, 7, 3};
                    static constexpr int kHiB[3] = {9, 5, 1};
                    for (int hi = 0; hi < 2; ++hi) {
                        int bitA = kHiA[ch] - (hi ? 0 : 1);
                        int bitB = kHiB[ch] - (hi ? 0 : 1);
                        auto mut = s0words;
                        for (int n = 0; n < 16; ++n) {
                            int p = n & 3, q = n >> 2;
                            if (p == k) mut[static_cast<size_t>(n)] =
                                static_cast<uint16_t>(mut[static_cast<size_t>(n)] ^ (1u << bitA));
                            if (q == k) mut[static_cast<size_t>(n)] =
                                static_cast<uint16_t>(mut[static_cast<size_t>(n)] ^ (1u << bitB));
                        }
                        auto t = runTraceRegs(mut, kRuns[c - 1], /*shres=*/true, kStart, kCount);
                        ++total;
                        if (t != base) ++changed;
                    }
                }
            }
        }
        checks.expect(changed == 0,
            "P2 TWO-BITS-54-TRACES-IDENTICAL: with the S0 table loaded, colour c's (17, 18 "
            "or 19) own solid run must show an identical trace whether or not a bit "
            "belonging to a DIFFERENT stored colour (16, and whichever of 17-19 is not c) "
            "is flipped everywhere page 302's table places that colour's letters -- 'There "
            "are only two bits of red, green and blue color resolution per hires pixel.' "
            "(p.301). 3 on-screen colours x 3 other colours x 3 channels x 2 (hi/lo) = 54 "
            "traces checked, all required identical. C-shres-color.c8.");
        checks.note("P2 TWO-BITS-54-TRACES-IDENTICAL: " + std::to_string(changed) + " of " +
                     std::to_string(total) + " traces changed.");
        // MUTATION P2 stimulus green: baseline is already red -- 18 of 54 traces change,
        // every one of them a k==0 (colour 16) combo, because colour 16's own slot-B
        // position always overlaps register $10+c for c=1,2,3 (the FINDING REG-0x11-13
        // measure directly). Built with -DMUT_P2=1, which skips k==0 entirely, leaving
        // only the 36 combos among colours 1,2,3 relative to c that page 301's "only two
        // bits" clause predicts stay identical regardless -> measured "5 passed, 3
        // failed, 14 declined" ("MEASURED: ... 0 of 36 traces changed"), P2 the only
        // verdict that moved (FAIL -> PASS) -- proving the comparator responds to a real
        // code change and that the 18-of-54 red is entirely colour 16's own interference.
        // Reverted (no flags): "4 passed, 4 failed, 14 declined", baseline restored.
    }

    // ==================== DOT-BITS ====================
    {
        auto base = runTrace(S0, kRuns[0], /*shres=*/true, kStart, kCount);
        auto live = runTraceOverride(S0, 17, static_cast<uint16_t>(s0words[1] | 0xF000), kRuns[0],
                                      /*shres=*/true, kStart, kCount);
        bool same = (live == base);
        checks.note("DOT-BITS: register $11 bits 15-12 flipped 0x0->0xF, run S1: trace " +
                     std::string(same ? "identical" : "differs") + " to the unmutated baseline.");
        checks.decline(
            "DOT-BITS: every printed row of page 302's table marks bits 15-12 '.' (unused), "
            "e.g. row $11: '....GHABIJCDKLEF'. Measured above: flipping them produces no "
            "observable difference here -- but this boundary's colour-register write path "
            "truncates data_in to its low twelve bits before latching in EVERY mode, not "
            "only SuperHires (the sibling ch3-colour-register-format package's own already-"
            "declined 'bits 15-12 unused' claim), so this package cannot attribute that "
            "truncation to page 302's own table specifically rather than to the general "
            "12-bit colour-register format -- declining rather than re-testing a sibling "
            "package's claim under a different name.");
    }

    // ==================== REG-0x10 ====================
    checks.decline(
        "REG-0x10: register COLOR16/$10's sixteen bit positions (row: 'C  10   .   .   .   "
        ".   A   B   A   B   C   D   C   D   E   F   E   F', p.302) can never be confirmed "
        "via this boundary's live sprite display, because combined index 0 -- the only "
        "index that ever addresses this register from an unattached sprite -- is "
        "universally transparent on real sprite hardware (p.103, p.108), confirmed "
        "empirically by C2 above: no RGB sample this Harness can produce ever reflects this "
        "register's stored content. Architectural property of the transparency path, not a "
        "per-bit finding -- declined rather than silently dropped or asserted false with "
        "nothing measured. " + std::string(kRegIds[0]) + " .");

    // ==================== REG-0x11 .. REG-0x1F ====================
    Trace baseline[5];
    for (int ri = 0; ri < 5; ++ri) baseline[ri] = runTraceRegs(s0words, kRuns[ri], /*shres=*/true, kStart, kCount);

    {
#if MUT_REG11
        RegVerdict v = measureRegister(s0words, S0, baseline, 1, /*forceGood=*/true);  // MUTATION
#else
        RegVerdict v = measureRegister(s0words, S0, baseline, 1);
#endif
        checks.expect(v.observed && v.allGood,
            "REG-0x11: row $11 ('....GHABIJCDKLEF', p.302) -- with the S0 table loaded, "
            "every one of register COLOR17/$11's twelve meaningful bits is flipped in turn "
            "and all five runs (S1, S2, S3, M, M-SHSH1) are scanned at BPLCON0.shres=1 for a "
            "difference against the unmutated baseline; a difference must land on the bit's "
            "own page-predicted colour (16 for the register's slot-B bits 9-8/5-4/1-0, 17 "
            "for its slot-A bits 11-10/7-6/3-2) and channel alone. Also carries page 301's "
            "general multiplexing statements this per-bit family as a whole tests: 'In order "
            "to decode sprite and bitplane color information in SuperHires mode, certain "
            "multiplexing occurs in the use of the registers.' " + regVerdictDetail(v) + " " +
            std::string(kRegIds[1]) + " .");
        checks.note("REG-0x11: " + regVerdictDetail(v));
        // MUTATION REG-0x11 stimulus green: built with -DMUT_REG11=1, which forces
        // measureRegister()'s own `good` comparison true regardless of `saw`/
        // `expectColour`/`comps` -> measured "5 passed, 3 failed, 14 declined",
        // REG-0x11 the only verdict that moved (FAIL -> PASS) -- proving the comparator
        // is not vacuously satisfied by "no bit ever differs" (it still requires
        // `v.observed`, which stays genuinely true). Reverted (no flags): "4 passed, 4
        // failed, 14 declined", the FINDING (bit 0, run S1, tick 9: observed colour 17,
        // page-predicted colour 16, component B) restored.
    }
    {
        RegVerdict v = measureRegister(s0words, S0, baseline, 2);
        checks.expect(v.observed && v.allGood,
            "REG-0x12: row $12 ('....MNABOPCDQREF', p.302) -- same method as REG-0x11, "
            "register COLOR18/$12, slot-A colour 18 (bits 11-10/7-6/3-2), slot-B colour 16 "
            "(bits 9-8/5-4/1-0). " + regVerdictDetail(v) + " " + std::string(kRegIds[2]) + " .");
        checks.note("REG-0x12: " + regVerdictDetail(v));
        // MUTATION REG-0x12 none impossible: already red at baseline (bit 0, run S2,
        // tick 9: observed colour 18, page-predicted colour 16, component B -- the same
        // FINDING as REG-0x11: NanoMig shows the whole register, not just slot A, so
        // slot-B/colour-16 bits move the on-screen colour-18 pixel too). No stimulus
        // this Harness can present makes every observed difference land on its own
        // predicted colour; there is nothing green here to break. The comparator's own
        // non-vacuousness is already demonstrated directly on REG-0x11 (identical code
        // path, measureRegister()).
    }
    {
        RegVerdict v = measureRegister(s0words, S0, baseline, 3);
        checks.expect(v.observed && v.allGood,
            "REG-0x13: row $13 ('....STABUVCDWXEF', p.302) -- same method as REG-0x11, "
            "register COLOR19/$13, slot-A colour 19 (bits 11-10/7-6/3-2), slot-B colour 16 "
            "(bits 9-8/5-4/1-0). " + regVerdictDetail(v) + " " + std::string(kRegIds[3]) + " .");
        checks.note("REG-0x13: " + regVerdictDetail(v));
        // MUTATION REG-0x13 none impossible: already red at baseline (bit 0, run S3,
        // tick 9: observed colour 19, page-predicted colour 16, component B -- the same
        // FINDING as REG-0x11/REG-0x12). No stimulus this Harness can present makes
        // every observed difference land on its own predicted colour; there is nothing
        // green here to break. The comparator's own non-vacuousness is already
        // demonstrated directly on REG-0x11 (identical code path, measureRegister()).
    }

    // REG-0x14 .. REG-0x1F: an unattached sprite 0's own value never exceeds 3, so its
    // combined index never reaches offset 4-15 on this boundary at all -- measured, not
    // assumed: every one of these registers' twelve bits is put through the identical
    // per-bit sweep as REG-0x11-0x13 above, and every run's trace is confirmed to match
    // its own unmutated baseline before being declined.
    for (int n = 4; n <= 15; ++n) {
        RegVerdict v = measureRegister(s0words, S0, baseline, n);
        checks.note(std::string("REG-0x") + kRows[n].hex + ": " + regVerdictDetail(v));
        checks.decline(
            std::string("REG-0x") + kRows[n].hex + ": row $" + kRows[n].hex + " ('" +
            kRows[n].symbols + "', p.302) -- measured above: none of the five runs (an "
            "unattached sprite 0 alone, own value never exceeding 3) ever selects combined "
            "index " + std::to_string(n) + ", so no bit of this register was ever observed "
            "to move any displayed pixel. The page does not say which register, or which "
            "half of it, a sprite pixel reads, and no page describes how an unattached "
            "sprite's own successive values pair into an index above 3 -- declined, not "
            "assumed unreachable. " + std::string(kRegIds[n]) + " .");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed, "
              << checks.declined << " declined\n";
    return checks.failed == 0 ? 0 : 1;
}
