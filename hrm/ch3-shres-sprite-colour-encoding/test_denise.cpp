// Package ch3-shres-sprite-colour-encoding -- ECS SuperHires sprite colour
//   encoding conformance test, fixture `denise` (one Verilator build, top
//   module denise.v plus its own eleven sub-modules: denise_bitplanes,
//   denise_bitplane_shifter, denise_playfields, denise_sprites,
//   denise_sprites_shifter, denise_collision, denise_hamgenerator,
//   denise_colortable, denise_colortable_ram_mf, denise_spritepriority,
//   bitplane_ram -- PACKAGE.md's own fixture line).
//
// Test purpose: printed page 302's table mapping the sixteen *upper*
//   colour registers (COLOR$10-COLOR$1F, i.e. COLOR16-COLOR31) onto the
//   symbols A-X, plus the legend "Sprite (Color 16) : AB-- CD-- EF--" (and
//   its three sibling legend lines for Color 17/18/19) that says each
//   named SuperHires sprite colour contributes only the TOP two bits of
//   each of R, G, B. A sibling package (page 301) covers the bitplane half
//   of the identical scheme; this package is the sprite half only.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 301-302 (claims.jsonl for ch3-shres-sprite-colour-encoding, 275
//   records). A red result below means denise.v (and the sub-modules
//   PACKAGE.md's own fixture line names) disagrees with the manual, not
//   that the assertion is wrong -- no assertion here was, or should be,
//   adjusted to make the RTL pass. Per the build brief: "already verified
//   independently: all 512 bit claims across pages 301 and 302 were
//   re-derived from the table cells plus the legend and matched 512 of
//   512" -- this package tests the RTL against that already-checked
//   mapping, it does not re-audit the mapping itself.
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
//   disjoint two-phase signals. denise_bitplanes.v:381 gates its BPLxDAT
//   write strobe on c1&c3; this fixture's sprite registers are written
//   through the SAME reg_address_in/data_in bus, so a broken phase pattern
//   would silently drop those writes too. This Harness uses the build
//   brief's own measured-working four-phase pattern: clk7_en=(phase==0),
//   c1=(phase==0||phase==1), c3=(phase==1||phase==2) -- phase 1 has both
//   high (the write strobe), phase 3 has both low -- the same pattern the
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
//   to confirm it is indistinguishable from disarmed). The one arithmetic
//   assumption this file makes about denise.v itself -- the channel width
//   red/green/blue expose (assumed 4 bits, the standard OCS/ECS DAC width
//   this whole repository's other Denise packages already use, e.g.
//   kColor00Value=0x0F0 in the ch3-horizontal-display-window sibling) -- is
//   confirmed or corrected as a HARNESS FIX after the RTL is opened, never
//   silently assumed into a passing result.
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
// [RTL OPENED / BUILD / HARNESS FIXES / MUTATION LOG: filled in below this
// line after the RTL was actually read, per ORDER OF WORK above.]

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
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
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

// Best-guess channel width, pending confirmation once the RTL is opened
// (see MEASUREMENT METHOD above) -- standard OCS/ECS DAC nibble width.
constexpr int kChannelBits = 4;

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

// Programs a wide-open display window, ECS+SHRES BPLCON0, and no
// bitplanes (BPU=0 -- background is COLOR00 wherever no sprite draws).
void setupDisplay(Harness& h) {
    h.write_reg(kDiwstrtAddr, kDiwstrtWide);
    h.write_reg(kDiwstopAddr, kDiwstopWide);
    h.write_reg(kBplcon0Addr, kBplcon0Shres | kBplcon0Ecsena);
}

// Arms sprite pair (SPR0=even/low, SPR1=odd/high) at the fixed position,
// each with its own 2-bit pixel value (0-3), attach set on SPR1 only when
// useOdd is true (the "unattached single sprite" case never touches
// SPR1 at all -- SPR1 stays disarmed, its own value irrelevant).
void armPair(Harness& h, int evenVal, int oddVal, bool useOdd) {
    uint16_t evenData = (evenVal & 1) ? 0x8000 : 0x0000;
    uint16_t evenDatb = (evenVal & 2) ? 0x8000 : 0x0000;
    h.write_reg(kSprPosAddr(0), kSpritePosByte);
    h.write_reg(kSprCtlAddr(0), kSpriteCtlSh0);
    h.write_reg(kSprDatbAddr(0), evenDatb);
    h.write_reg(kSprDataAddr(0), evenData);
    if (useOdd) {
        uint16_t oddData = (oddVal & 1) ? 0x8000 : 0x0000;
        uint16_t oddDatb = (oddVal & 2) ? 0x8000 : 0x0000;
        h.write_reg(kSprPosAddr(1), kSpritePosByte);
        h.write_reg(kSprCtlAddr(1), kSpriteCtlSh0 | kSprCtlAttachBit);
        h.write_reg(kSprDatbAddr(1), oddDatb);
        h.write_reg(kSprDataAddr(1), oddData);
    }
}

// The set of samples that differ from a same-setup, same-position,
// disarmed-sprite baseline (DATA=DATB=0 for both SPR0 and SPR1) -- i.e.
// exactly the samples this sprite's own pixel(s) drove.
std::vector<uint32_t> activeSamples(uint16_t regAddr, uint16_t regValue,
                                     int evenVal, int oddVal, bool useOdd) {
    Harness base;
    setupDisplay(base);
    base.write_reg(regAddr, regValue);
    armPair(base, 0, 0, useOdd);  // disarmed: both members value 0
    auto baseline = base.scan_line(kScanSamples);

    Harness live;
    setupDisplay(live);
    live.write_reg(regAddr, regValue);
    armPair(live, evenVal, oddVal, useOdd);
    auto trace = live.scan_line(kScanSamples);

    std::vector<uint32_t> diffs;
    for (size_t i = 0; i < trace.size() && i < baseline.size(); ++i) {
        if (trace[i] != baseline[i]) diffs.push_back(trace[i]);
    }
    return diffs;
}

// Extracts channel `ch` ('R','G','B') top-2-bit value (0-3) from a
// packed RGB sample, per kChannelBits.
int top2(uint32_t rgb, char ch) {
    uint32_t v;
    if (ch == 'R') v = (rgb >> 16) & ((1u << kChannelBits) - 1);
    else if (ch == 'G') v = (rgb >> 8) & ((1u << kChannelBits) - 1);
    else v = rgb & ((1u << kChannelBits) - 1);
    return static_cast<int>(v >> (kChannelBits - 2)) & 0x3;
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
        // written so slot A's R field (bits 11:10) and the FULL 4-bit R
        // nibble (bits 11:8) differ: slot A R = 10b (2), full nibble =
        // 1011b (0xB=11). If SuperHires colour multiplexing occurs at
        // all, the displayed R value with shres=1 must differ from the
        // displayed R value with shres=0 for this SAME register content.
        uint16_t regv = buildRegValue(/*rA*/ 1, 0, /*gA*/ 0, 0, /*bA*/ 0, 0,
                                       /*rB*/ 1, 1, /*gB*/ 0, 0, /*bB*/ 0, 0, 0);
        Harness h0;
        h0.write_reg(kDiwstrtAddr, kDiwstrtWide);
        h0.write_reg(kDiwstopAddr, kDiwstopWide);
        h0.write_reg(kBplcon0Addr, kBplcon0Ecsena);  // shres=0
        h0.write_reg(kColorAddr(17), regv);
        armPair(h0, 1, 0, false);
        auto t0 = h0.scan_line(kScanSamples);

        Harness h1;
        h1.write_reg(kDiwstrtAddr, kDiwstrtWide);
        h1.write_reg(kDiwstopAddr, kDiwstopWide);
        h1.write_reg(kBplcon0Addr, kBplcon0Shres | kBplcon0Ecsena);  // shres=1
        h1.write_reg(kColorAddr(17), regv);
        armPair(h1, 1, 0, false);
        auto t1 = h1.scan_line(kScanSamples);

        bool differs = (t0 != t1);
        checks.expect(differs,
            "SHRES-MODE-CHANGES-OUTPUT: with register COLOR17/$11 loaded so "
            "its full 4-bit R nibble (1011b) differs from its own top-two-"
            "bit slot-A R field (10b), the whole-line RGB trace for a "
            "sprite pixel selecting this register differs between "
            "BPLCON0.shres=0 and BPLCON0.shres=1 -- confirming that "
            "SuperHires mode actually changes how sprite colour data is "
            "read out of these registers, not merely that the registers "
            "exist: 'In order to decode sprite and bitplane color "
            "information in SuperHires mode, certain multiplexing occurs "
            "in the use of the registers.' (p.301). 'SuperHires sprites are "
            "encoded in the upper sixteen color registers using a similar "
            "scheme:' (p.302, quoted twice by two independent readers). "
            "C-shrc.c03, C-shres-color.c8, C-shrs.c01 .");
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
            std::string ids = kRegIds[idx];
            checks.expect(false,
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
                "not a per-bit finding about any ONE of these ids -- kept "
                "red rather than silently dropped. " + ids + " .");
        } else {
            uint16_t regv = buildRegValue(hi(rA), lo(rA), hi(gA), lo(gA), hi(bA), lo(bA),
                                           hi(rB), lo(rB), hi(gB), lo(gB), hi(bB), lo(bB),
                                           /*unused4=*/0x0);
            auto diffs = activeSamples(regAddr, regv, evenTarget, oddTarget, useOdd);

            bool slotAOk = !diffs.empty();
            for (uint32_t s : diffs) {
                if (top2(s, 'R') != rA || top2(s, 'G') != gA || top2(s, 'B') != bA) slotAOk = false;
            }

            // Unused-bits (15-12) no-effect check: same slot-A/slot-B
            // content, unused nibble flipped from 0x0 to 0xF.
            uint16_t regvUnused = buildRegValue(hi(rA), lo(rA), hi(gA), lo(gA), hi(bA), lo(bA),
                                                 hi(rB), lo(rB), hi(gB), lo(gB), hi(bB), lo(bB),
                                                 /*unused4=*/0xF);
            auto diffsUnused = activeSamples(regAddr, regvUnused, evenTarget, oddTarget, useOdd);
            bool unusedNoEffect = (diffs == diffsUnused);

            // Slot-B (bits 9,8 / 5,4 / 1,0) effect probe: two WHOLE-LINE
            // traces (not diffed against a baseline, so a slot-B effect
            // showing up anywhere -- including outside the samples slot-A
            // already marked "active", e.g. a separate SuperHires
            // sub-pixel sample point -- is still caught), sharing the
            // SAME slot-A content and the SAME sprite arming, differing
            // ONLY in slot B (000000 vs 111111).
            Harness pb0;
            setupDisplay(pb0);
            pb0.write_reg(regAddr, buildRegValue(hi(rA), lo(rA), hi(gA), lo(gA), hi(bA), lo(bA),
                                                  0, 0, 0, 0, 0, 0, 0x0));
            armPair(pb0, evenTarget, oddTarget, useOdd);
            auto slotBLow = pb0.scan_line(kScanSamples);
            Harness pb1;
            setupDisplay(pb1);
            pb1.write_reg(regAddr, buildRegValue(hi(rA), lo(rA), hi(gA), lo(gA), hi(bA), lo(bA),
                                                  1, 1, 1, 1, 1, 1, 0x0));
            armPair(pb1, evenTarget, oddTarget, useOdd);
            auto slotBHigh = pb1.scan_line(kScanSamples);
            bool slotBHasEffect = (slotBLow != slotBHigh);

            std::string ids = kRegIds[idx];

            checks.expect(slotAOk && unusedNoEffect && slotBHasEffect,
                std::string("REG-0x") + row.hex +
                ": with register COLOR" + std::to_string(16 + offset) + "/$" + row.hex +
                " loaded so its top-two-bit ('slot A') fields (bits 11:10 R "
                "/ 7:6 G / 3:2 B, this row's own printed pattern, p.302) "
                "read R=2 G=1 B=3, and the sprite arming that this "
                "register's own combined index (" + std::to_string(offset) +
                ") naturally reaches " +
                (useOdd ? "(an ATTACHED pair, SPR0 own value " + std::to_string(evenTarget) +
                          ", SPR1 own value " + std::to_string(oddTarget) + ")"
                        : "(an UNATTACHED sprite, own value " + std::to_string(evenTarget) + ")") +
                ": every RGB sample this sprite pixel drives reads back "
                "exactly R=2 G=1 B=3 (bits 11:10/7:6/3:2 carry what this "
                "row's printed symbols say they carry); flipping this SAME "
                "register's unused bits 15-12 from 0x0 to 0xF changes none "
                "of those samples (matching 'bit 15/14/13/12 is unused' for "
                "this row); and flipping this SAME register's 'slot B' "
                "bits (9,8/5,4/1,0, this row's OTHER printed symbols) from "
                "000000 to 111111, with slot A and the sprite arming both "
                "held fixed, changes at least one sample somewhere across "
                "the whole scanned line -- showing bits 9,8,5,4,1,0 are "
                "not simply inert padding either, matching this row's own "
                "printed symbol assignment for those six bit positions "
                "too. " + ids + " .");
        }
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
