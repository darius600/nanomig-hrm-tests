// ch3-colour-register-format -- denise_colortable conformance test
//
// Test purpose: the colour registers themselves as chapter 3 of the Amiga
//   Hardware Reference Manual (3rd ed.) describes them: 32 registers, each
//   holding a 12-bit code split into 4-bit Red/Green/Blue nibbles (the top 4
//   bits of the 16-bit word unused), write-only, individually addressed by
//   the 5-bit code assembled from bitplanes 5..1 (Figures 3-3/3-4), and the
//   extra-half-brite shift that bitplane 6 applies to that code's output.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, 3rd ed., printed pages
//   42, 43, 44, 46, 47, 48, 55, 56, 67, 83, 86, 90, 92, 95, 271, 273, 292,
//   293, 301 (claims.jsonl for ch3-colour-register-format, 163 records). A red
//   result below means denise_colortable.v disagrees with the manual, not
//   that the assertion is wrong -- no assertion here was, or should be,
//   adjusted to make the RTL pass.
// Non-goals: anything that needs a port this module does not have. See "OUT
//   OF SCOPE" below for the 27 claims that name a wider mechanism (BPLCON0,
//   the bitplane-to-select assembly, the display-window border, or a 16-bit
//   bus this 12-bit port cannot receive) than this fixture's boundary
//   reaches.
// Claim ids: ids such as `B-subject.c167` and `B-subject.c171` name one
//   sentence of the manual in this chapter's reading. They carry no reader
//   prefix, and they resolve in the claim files that
//   hrm-testpipeline/acceptance/ch3.json lists under `inventories`.
//
// BOUNDARY: top module `denise_colortable` only (one Verilator build,
//   sources denise_colortable.v + denise_colortable_ram_mf.v), per
//   PACKAGE.md. Ports used, exactly as PACKAGE.md's own evidence names
//   them (module header lines 11-19): reg_address_in, data_in[11:0],
//   rdram, select[7:0], bank, loct, ehb_en, rgb[23:0]. clk/clk7_en/bplxor
//   and the reg_address_in write-address convention (COLORBASE 9'h180 ->
//   word address $C0..$DF, i.e. reg_address_in = 0xC0 + register index)
//   are taken from the pre-pipeline hrm/denise_colortable/ harness
//   (removed), along with its Makefile shape.
//
// ASSUMPTION carried into the byte-extraction helpers R_of/G_of/B_of
//   below: a port literally named `rgb[23:0]` is assumed to pack Red in
//   the high byte, Green in the middle, Blue in the low byte, in that
//   order -- the same order the manual gives the nibbles in the 12-bit
//   word (bits 11-8 Red, 7-4 Green, 3-0 Blue). This is a naming-convention
//   inference, not something read out of the RTL body. If it is wrong the
//   symptom is every RGB-field assertion failing identically (R and B
//   swapped), which is a plumbing fix (swap the extraction), not an
//   oracle change.
//
// CLAIM GROUPS -- every one of the 163 claim ids in claims.jsonl is cited,
//   in full, in exactly one group below (in the assertion string that
//   tests it, not only here). Ids restating the identical proposition
//   under a different manual wording, page, or reader are grouped and
//   cited together on the single assertion that tests that proposition --
//   the same convention hrm/ch7-dmacon-register-bits/ documents and uses. Ids
//   naming genuinely different propositions (32 different registers, 14
//   different named colour samples) get their own assertion each, inside
//   a shared loop (one call site, like that package's WALKING-BIT loop).
//
//   REG-00..REG-31   (41 ids: 32 B-subject + 9 figure/example ids folded
//                      onto the 5 registers those figures name by number)
//     -- each of the 32 registers answers its own write and changes again
//        on a second, different write, independent of the other 31.
//   ADDR-32-DISTINCT (31 ids)
//     -- 32 distinct 12-bit codes written to the 32 registers read back
//        as 32 pairwise-distinct rgb values (there really are 32
//        independent registers, not fewer, and `select`/BPLxDAT is the
//        addressing mechanism).
//   SWEEP-12BIT-DISTINCT (8 ids)
//     -- 9 distinct 12-bit codes on one register read back pairwise
//        distinct (the 12-bit field is used, not truncated to a handful
//        of levels).
//   RED-FIELD-BITS-11-8, GREEN-FIELD-BITS-7-4, BLUE-FIELD-BITS-3-0
//                     (12 + 11 + 11 = 34 ids)
//     -- each of the four bits in a nibble moves only its own channel
//        byte, MSB-heaviest first, the other two channels unmoved.
//        RED-FIELD also anchors on the manual's own worked sample $D00
//        "Brick red" (3-color-samples.c3).
//   SAMPLE-nn        (14 ids, one assertion per named sample, shared loop)
//     -- each of Table 3-4's/3-13's remaining named samples ($6FE Sky
//        blue .. $999 Medium grey) is live and distinguishable from an
//        all-zero baseline on the same register. The manual names a
//        colour, not a numeric RGB target, so "distinguishable from
//        black" is the honest ceiling of what is testable here -- see
//        the header note on 3-t4.c02/c03 below for why exact colour-name
//        matching is not attempted.
//   EHB-HALVES-INTENSITY (5 ids)
//     -- selecting a register with the select[5] (bitplane-6) bit set
//        halves each of the three component bytes versus the same
//        register selected without it. The exact "shifted right by one"
//        mechanism is PACKAGE.md's own disclosed evidence for these ids
//        (denise_colortable.v:61-66 per PACKAGE.md), not something opened
//        here -- it is what turns the manual's qualitative "half
//        intensity" into a checkable relationship.
//   NO-READBACK-PORT (3 ids)
//     -- the only port that reflects a written value is rgb, gated by
//        select ("look at the screen"); PACKAGE.md's own port list for
//        this boundary (reg_address_in/data_in/rdram/select/bank/loct/
//        ehb_en in, rgb out) has no separate register-content read bus,
//        and this assertion confirms the one path that does exist behaves
//        as such.
//   APPC-SHR-REG0-3-BASELINE-NIBBLES (1 id)
//     -- Appendix C's SuperHires section (p.301) states the baseline it is
//        contrasting SuperHires against: outside SuperHires mode, each of
//        the four registers COLOR0-COLOR3 holds its own 12-bit code split
//        into three 4-bit R/G/B nibbles stored as 0x0RGB - the same
//        nibble format RED-FIELD-BITS-11-8/GREEN-FIELD-BITS-7-4/
//        BLUE-FIELD-BITS-3-0 above establish, but this claim names all
//        three channels together on one sentence, so - per this file's own
//        "cited in exactly one group" rule above - it gets one assertion
//        of its own, run on all four named registers (COLOR0-COLOR3),
//        rather than a citation split across those three single-channel
//        groups. This
//        module has no SuperHires-mode port at all (PACKAGE.md's port
//        list: reg_address_in/data_in/rdram/select/bank/loct/ehb_en in,
//        rgb out - no shres/hires-select input), so every measurement this
//        fixture can make already sits outside SuperHires by construction;
//        nothing here asserts anything about the sixteen-lower-registers
//        SuperHires encoding the same sentence goes on to describe, which
//        is a different mechanism this boundary cannot reach.
//
//   Two additional checks carry no claim id: CONTROL-KNOWN-VALUES-ARRIVE
//   (the write/select/rgb data path is alive before anything above trusts
//   it) and the EHB group's own reproducibility control, both the control
//   basis for the suspect assertions.
//
// NO HIGH-NIBBLE ASSERTION (bits 15-12 unused): data_in is declared
//   `[12-1:0]` in denise_colortable.v -- there are only 12 wires here, none
//   for bits 15-12 at all, so presenting $F456 (top nibble set) is not a
//   real 16-bit bus value landing on a 12-bit port; it pokes Verilator's
//   wider C++ storage for that port past the width the generated model's
//   own masking is written for. Measured directly: data_in reads back as 0
//   immediately after such a write (the $F456 value does not survive even
//   as raw storage), and a $0456 write and a $F456 write, identical in
//   their low 12 bits, read back differently anyway -- $445566 vs $4455F6,
//   blue only, not the all-or-nothing result a genuine masking question
//   would produce. That is a Verilator/testbench boundary artifact, not a
//   measurement of denise_colortable, so no assertion is made on it. The 9
//   "bits 15-12 unused" ids sit in OUT OF SCOPE below with this as their
//   reason -- PACKAGE.md's own "MEASURED (d_denise)" note for these same
//   ids establishes the fact one level up, at a boundary that actually has
//   a 16-bit-wide bus to truncate, and it cannot be re-derived from a
//   12-bit port that never receives the extra bits.
//
// OUT OF SCOPE -- not asserted anywhere below; each names a mechanism
//   this fixture's ports cannot reach (denise_colortable only sees
//   `select`; nothing decides what `select` is fed to it):
//   3-colortable.c13, 3-t3.c01, 3-colorsum.c6, 3-colorsum.c7, A-color.c10,
//     A-color.c7, A-color.c8, A-color.c9, A-colorxx.bits1 (p.46/92/273):
//     "bits 15-12 unused" -- data_in is declared exactly [11:0] at this
//     boundary; see NO HIGH-NIBBLE ASSERTION above. Settling this needs the
//     wider boundary (denise.v / d_denise) where a real 16-bit bus exists
//     and is truncated before reaching this module - PACKAGE.md's own
//     evidence for these ids already places that measurement there, not
//     here.
//   3-colortable.c7, 3-colortable.c8, 3-colortable.c9, 3-ct.c03,
//     3-ct.c04, 3-ct.c05 (p.46): "COLOR00 is reserved for background /
//     shows wherever no object is present / in the border" -- which
//     `select` value reaches this module when nothing else is on screen
//     is a decision made upstream (denise.v or later), not by
//     denise_colortable, which only ever answers whatever `select` it is
//     given.
//   3-bplcon0.c18, 3-t5.c02, 3-r0.bpu000, 3-regsum-bplcon0.c19 (p.48/90):
//     BPLCON0.BPU=000 -> background only -- BPLCON0 is not a port of this
//     module.
//   3-code.c02, 3-code.c03, 3-coding.c2, 3-coding.c3, 3-coding.c4,
//     3-code.f03 (p.55/56): "an all-zero/two-colour bitplane selects
//     COLOR00/COLOR01" -- the antecedent (which bitplane pattern produces
//     which `select` code) is the bitplane-to-select assembly, upstream
//     of this boundary.
//   3-dual.c07 (p.67): "the two playfields take colours from different
//     sets of registers" -- there is no playfield-id port here; which
//     `select` values a playfield uses is decided upstream.
//   3-hscr.f07 (p.83): figure shows the display-window border, a
//     geometry/timing concern with no port on this boundary.
//
// MUTATION REG-09 stimulus red: register 9's two write calls were redirected to register 10 (write_idx = idx+1) while the read stayed at register 9; register 9 was never touched, so after_a == after_b instead of differing.
// MUTATION ADDR-32-DISTINCT stimulus red: register 15 was written with register 14's 12-bit pattern instead of its own distinct one (register 15 still correctly targeted - only the value changed), so two of the 32 registers held identical content and the pairwise-distinct check found a collision. (Redirecting the address instead of the value - idx=15's write landing on register 14 - stays green: register 14 ends up holding pattern(15), a value still unique among the 32, and register 15's untouched reset/black value is also unique, so nothing collides. That is why this mutation is value-based, not address-based.)
// MUTATION SWEEP-12BIT-DISTINCT stimulus red: sweep entry 4 (0x555) was replaced with entry 3's pattern (0x3F0) instead of its own value, producing a duplicate among the 9 writes and a collision in the distinctness check.
// MUTATION RED-FIELD-BITS-11-8 stimulus red: the bit probed for R3 (bit 11) was moved to bit 7 (G3) instead, so the green byte changes when the check expects only the red byte to move.
// MUTATION GREEN-FIELD-BITS-7-4 stimulus red: the bit probed for G3 (bit 7) was moved to bit 11 (R3) instead, so the red byte changes when the check expects only the green byte to move.
// MUTATION BLUE-FIELD-BITS-3-0 stimulus red: the bit probed for B3 (bit 3) was moved to bit 7 (G3) instead, so the green byte changes when the check expects only the blue byte to move.
// MUTATION SAMPLE stimulus red: the Light green entry ($8E0) was replaced with $000 (black) instead of its own code, so its readback equals the black baseline and the liveness check fails.
// MUTATION EHB-HALVES-INTENSITY stimulus red: the EHB read was moved from register 15 (the register just loaded and read in normal mode) to register 16 (loaded with a different, unrelated pattern), so halved(normal) no longer matches the EHB reading.
// MUTATION NO-READBACK-PORT stimulus red: the read after each write moved from register 20 (the register actually written) to register 21 (never written), so the two reads no longer differ.
// MUTATION APPC-SHR-REG0-3-BASELINE-NIBBLES stimulus red: on COLOR0 only (the other three registers in the loop unchanged), the bit probed for R3 (bit 11) was moved to bit 7 (G3) instead, so COLOR0's green byte changes when the check expects only the red byte to move, failing the all-four-registers AND.
// MUTATION CONTROL-KNOWN-VALUES-ARRIVE stimulus red: the control's second write (originally to register 2, the same register the two reads target) was redirected to register 3 instead, so register 2 was never rewritten and the second read came back equal to the first ($1F0 both times, a == b) instead of distinct; reverted to writing register 2 again and confirmed the 55-passed/0-failed baseline returned.
// MUTATION EHB-CONTROL-NORMAL-REPRODUCIBLE stimulus red: the post-EHB normal-mode re-read was redirected from register 15 (the register loaded and read earlier in the same block) to register 16 (never written, still at its power-on value), so the re-read no longer matched the value recorded before the EHB reading; reverted to reading register 15 again and confirmed the 55-passed/0-failed baseline returned.
//
// Every MUTATION above was actually built and run (see the commit history
// for this package for the per-mutation pass/fail counts) with the flag
// named after it defined to 1 (e.g. -DMUT_REG=1), confirmed to turn only
// its own assertion red, then rebuilt with no flags to confirm a clean
// return to the baseline result. No oracle/expected value was changed by
// any of them; only what each check asks the module to do.

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include "Vdenise_colortable.h"
#include "verilated.h"

#ifndef MUT_REG
#define MUT_REG 0
#endif
#ifndef MUT_ADDR32
#define MUT_ADDR32 0
#endif
#ifndef MUT_BITS12
#define MUT_BITS12 0
#endif
#ifndef MUT_RED
#define MUT_RED 0
#endif
#ifndef MUT_GREEN
#define MUT_GREEN 0
#endif
#ifndef MUT_BLUE
#define MUT_BLUE 0
#endif
#ifndef MUT_SAMPLE
#define MUT_SAMPLE 0
#endif
#ifndef MUT_EHB
#define MUT_EHB 0
#endif
#ifndef MUT_WO
#define MUT_WO 0
#endif
#ifndef MUT_CTRL
#define MUT_CTRL 0
#endif
#ifndef MUT_EHBCTRL
#define MUT_EHBCTRL 0
#endif
#ifndef MUT_APPCSHR
#define MUT_APPCSHR 0
#endif

namespace {

constexpr uint8_t COLOR_ADDR_BASE = 0xC0;  // reg_address_in word-address of COLOR00 ($180/2); COLORx = 0xC0+x.
constexpr uint8_t IDLE_ADDR = 0x00;        // outside 0xC0-0xDF: the write decode will not strobe any color register here.

uint8_t R_of(uint32_t rgb) { return static_cast<uint8_t>((rgb >> 16) & 0xFF); }
uint8_t G_of(uint32_t rgb) { return static_cast<uint8_t>((rgb >> 8) & 0xFF); }
uint8_t B_of(uint32_t rgb) { return static_cast<uint8_t>(rgb & 0xFF); }

std::string hex3(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "$%03X", static_cast<unsigned>(v & 0xFFF));
    return std::string(buf);
}

class Harness {
public:
    Harness() : dut(new Vdenise_colortable) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->rdram = 0;
        dut->bplxor = 0;
        dut->bank = 0;
        dut->loct = 0;
        dut->ehb_en = 0;
        dut->select = 0;
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        tick();
    }
    ~Harness() { delete dut; }

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    // Writes `val` (0..0xFFF; data_in is declared [11:0] in the RTL, so
    // any caller-supplied bits above that are not meaningful here - see
    // NO HIGH-NIBBLE ASSERTION near the top of this file) to color register
    // `idx` (0..31) via reg_address_in/data_in, then parks reg_address_in
    // on an address the write decode does not recognise so the value is
    // not re-latched on later ticks (mirrors hrm/denise_colortable/'s
    // harness).
    void write_reg(int idx, uint32_t val) {
        dut->reg_address_in = static_cast<uint8_t>(COLOR_ADDR_BASE + idx);
        dut->data_in = val;
        tick();
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        tick();
    }

    // Selects color register `idx` (0..31) for output, optionally with the
    // extra-half-brite bit (select bit 5) set, and returns rgb after two
    // ticks (matches hrm/denise_colortable/'s harness, which reads twice
    // after changing `select`).
    uint32_t read_reg(int idx, bool ehb = false) {
        dut->ehb_en = ehb ? 1 : 0;
        dut->select = static_cast<uint8_t>(idx + (ehb ? 32 : 0));
        tick();
        tick();
        return dut->rgb;
    }

    Vdenise_colortable* dut;
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    int passed = 0;
    int failed = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =========================================================================
    // CONTROL -- the fixture can deliver a known value before anything below
    // is trusted to read one back.
    // =========================================================================
    {
        Harness h;
        h.write_reg(2, 0x1F0);
        uint32_t a = h.read_reg(2);
        int second_write_idx = 2;
#if MUT_CTRL
        second_write_idx = 3;
#endif
        h.write_reg(second_write_idx, 0x00F);
        uint32_t b = h.read_reg(2);
        checks.expect(
            a != b,
            "[control] CONTROL-KNOWN-VALUES-ARRIVE: two distinct known 12-bit "
            "codes ($1F0 then $00F) written to the same register, back to "
            "back, read back through select/rgb as two distinct values before "
            "any assertion below trusts that path. No manual page: the "
            "control for every write-then-read assertion in this file, on "
            "the same measurement basis.");
    }

    // =========================================================================
    // REG-00 .. REG-31 -- each of the 32 registers (p.42/43/44/46/292/293)
    // answers its own write and changes again on a second, different write,
    // independent of the other 31. One call site, 32 dynamic labels (mirrors
    // hrm/ch7-dmacon-register-bits/'s WALKING-BIT loop).
    // =========================================================================
    {
        Harness h;
        struct RegCase {
            int idx;
            int page;
            const char* id;
            const char* extra;
        };
        static const RegCase cases[32] = {
            {0, 292, "B-subject.c167", ""},
            {1, 292, "B-subject.c171", ""},
            {2, 292, "B-subject.c175", ""},
            {3, 292, "B-subject.c179", ""},
            {4, 292, "B-subject.c183", ""},
            {5, 292, "B-subject.c187", ""},
            {6, 292, "B-subject.c191",
             " Also Figure 3-4's worked example (00110 -> COLOR6, p.44): 3-bpl.f07."},
            {7, 292, "B-subject.c195", ""},
            {8, 292, "B-subject.c199", ""},
            {9, 292, "B-subject.c203", ""},
            {10, 292, "B-subject.c207", ""},
            {11, 292, "B-subject.c211",
             " Also Figure 3-4 (01011 -> COLOR11, p.44): 3-bpl.f08, 3-color.c22."},
            {12, 292, "B-subject.c215", ""},
            {13, 292, "B-subject.c219", ""},
            {14, 292, "B-subject.c223", ""},
            {15, 292, "B-subject.c227", ""},
            {16, 292, "B-subject.c231", ""},
            {17, 292, "B-subject.c235", ""},
            {18, 292, "B-subject.c239",
             " Also Figure 3-4 (10010 -> COLOR18, p.44): 3-bpl.f09, 3-color.c21."},
            {19, 292, "B-subject.c243", ""},
            {20, 292, "B-subject.c247", ""},
            {21, 293, "B-subject.c251", ""},
            {22, 293, "B-subject.c255", ""},
            {23, 293, "B-subject.c259", ""},
            {24, 293, "B-subject.c263",
             " Also Figure 3-3 (11000 -> COLOR24, p.43): 3-bpl.f03, 3-color.c16."},
            {25, 293, "B-subject.c267", ""},
            {26, 293, "B-subject.c271", ""},
            {27, 293, "B-subject.c275", ""},
            {28, 293, "B-subject.c279",
             " Also Figure 3-4 (11100 -> COLOR28, p.44): 3-bpl.f10, 3-color.c20."},
            {29, 293, "B-subject.c283", ""},
            {30, 293, "B-subject.c287", ""},
            {31, 293, "B-subject.c291", ""},
        };
        for (const auto& rc : cases) {
            int write_idx = rc.idx;
            int read_idx = rc.idx;
#if MUT_REG
            if (rc.idx == 9) write_idx = rc.idx + 1;
#endif
            h.write_reg(write_idx, 0x123);
            uint32_t after_a = h.read_reg(read_idx);
            h.write_reg(write_idx, 0xFFF ^ 0x123);
            uint32_t after_b = h.read_reg(read_idx);

            char idxbuf[4];
            std::snprintf(idxbuf, sizeof(idxbuf), "%02d", rc.idx);
            checks.expect(
                after_a != after_b,
                std::string("REG-") + idxbuf + " (p." + std::to_string(rc.page) +
                    "): COLOR" + idxbuf +
                    " answers its own write and changes again when "
                    "rewritten with a distinct 12-bit code ($123 then its "
                    "12-bit complement), independent of every other "
                    "register - 'COLOR" + idxbuf + " ... Color table " +
                    idxbuf + "'. " + rc.id + "." + rc.extra);
        }
    }

    // =========================================================================
    // ADDR-32-DISTINCT (p.42/43/44/46/86/92/273) -- 32 distinct 12-bit codes
    // on the 32 registers read back as 32 pairwise-distinct values: there
    // really are 32 independent registers, and select/BPLxDAT is the
    // addressing mechanism the figures and running text describe.
    // =========================================================================
    {
        Harness h;
        auto pattern = [](int idx) -> uint16_t { return static_cast<uint16_t>(((idx << 4) ^ 0x0A5) & 0xFFF); };
        for (int idx = 0; idx < 32; ++idx) {
            uint16_t val = pattern(idx);
#if MUT_ADDR32
            if (idx == 15) val = pattern(14);
#endif
            h.write_reg(idx, val);
        }
        uint32_t results[32];
        for (int idx = 0; idx < 32; ++idx) results[idx] = h.read_reg(idx);
        bool all_distinct = true;
        for (int i = 0; i < 32 && all_distinct; ++i)
            for (int j = i + 1; j < 32; ++j)
                if (results[i] == results[j]) { all_distinct = false; break; }
        checks.expect(
            all_distinct,
            "ADDR-32-DISTINCT: 32 distinct 12-bit codes written one per "
            "register, read back through select/rgb as 32 pairwise-distinct "
            "values - 'The Amiga has thirty-two color registers' (p.42), "
            "'The color table contains 32 registers' (p.46), the Figure "
            "3-3/3-4 5-bit selection code (p.43/44), and 'each pixel is "
            "colored by the contents of the selected color register' "
            "(p.86). 3-bpl.c12, 3-color.c12, 3-color.c13, 3-bpl.f01, "
            "3-bpl.f02, 3-color.c15, 3-colortable.c1, 3-colortable.c2, "
            "3-colortable.c3, 3-colortable.c4, 3-colortable.c5, "
            "3-colortable.c6, 3-ct.c01, 3-ct.c02, 3-t2.c01, 3-t2.c02, "
            "3-t2.c03, 3-t2.c04, 3-t2.c05, 3-ham.c02, 3-ham.c2, 3-ham.c3, "
            "3-colorsum.c1, 3-csum.c01, A-color.c3, A-color.c5, A-color.c6, "
            "A-colorxx.c01, A-colorxx.c03, A-colorxx.c04, A-colorxx.fn.");
    }

    // =========================================================================
    // SWEEP-12BIT-DISTINCT (p.42/46/273) -- 9 distinct 12-bit codes on one
    // register read back pairwise distinct: the 12-bit field is used, not
    // collapsed to a handful of levels (up to 4096 possible colors).
    // =========================================================================
    {
        Harness h;
        static const uint16_t patterns[9] = {0x000, 0x111, 0x248, 0x3F0, 0x555, 0x6A6, 0x789, 0xABC, 0xFFF};
        uint32_t results[9];
        for (int i = 0; i < 9; ++i) {
            uint16_t p = patterns[i];
#if MUT_BITS12
            if (i == 4) p = patterns[3];
#endif
            h.write_reg(5, p);
            results[i] = h.read_reg(5);
        }
        bool all_distinct = true;
        for (int i = 0; i < 9 && all_distinct; ++i)
            for (int j = i + 1; j < 9; ++j)
                if (results[i] == results[j]) { all_distinct = false; break; }
        checks.expect(
            all_distinct,
            "SWEEP-12BIT-DISTINCT: 9 distinct 12-bit codes on one register "
            "read back as 9 pairwise-distinct values - 'Twelve bits of "
            "color selection allow you to define ... one of 4,096 possible "
            "colors' (p.46). 3-bpl.c13, 3-color.c14, 3-colortable.c11, "
            "3-colortable.c12, 3-ct.c07, 3-ct.c08, A-color.c4, "
            "A-colorxx.c02.");
    }

    // =========================================================================
    // RED-FIELD-BITS-11-8 / GREEN-FIELD-BITS-7-4 / BLUE-FIELD-BITS-3-0
    // (p.46/92/273) -- each of the four bits in a nibble moves only its own
    // channel byte, most-significant bit heaviest, the other two channels
    // held at the all-zero baseline.
    // =========================================================================
    {
        Harness h;
        h.write_reg(10, 0x000);
        uint32_t base = h.read_reg(10);

        int bit_for_msb = 11;
#if MUT_RED
        bit_for_msb = 7;
#endif
        h.write_reg(10, static_cast<uint16_t>(1u << bit_for_msb));
        uint32_t v11 = h.read_reg(10);
        h.write_reg(10, 1u << 10);
        uint32_t v10 = h.read_reg(10);
        h.write_reg(10, 1u << 9);
        uint32_t v9 = h.read_reg(10);
        h.write_reg(10, 1u << 8);
        uint32_t v8 = h.read_reg(10);
        h.write_reg(10, 0xD00);  // Table's own sample: 'Brick red' (p.92)
        uint32_t vSample = h.read_reg(10);

        int dR11 = static_cast<int>(R_of(v11)) - static_cast<int>(R_of(base));
        int dR10 = static_cast<int>(R_of(v10)) - static_cast<int>(R_of(base));
        int dR9 = static_cast<int>(R_of(v9)) - static_cast<int>(R_of(base));
        int dR8 = static_cast<int>(R_of(v8)) - static_cast<int>(R_of(base));
        bool ok =
            G_of(v11) == G_of(base) && B_of(v11) == B_of(base) && dR11 > 0 &&
            G_of(v10) == G_of(base) && B_of(v10) == B_of(base) && dR10 > 0 &&
            G_of(v9) == G_of(base) && B_of(v9) == B_of(base) && dR9 > 0 &&
            G_of(v8) == G_of(base) && B_of(v8) == B_of(base) && dR8 > 0 &&
            dR11 > dR10 && dR10 > dR9 && dR9 > dR8 &&
            G_of(vSample) == G_of(base) && B_of(vSample) == B_of(base) &&
            R_of(vSample) > R_of(base);
        checks.expect(
            ok,
            "RED-FIELD-BITS-11-8: bits 11,10,9,8 set alone each move only "
            "the red byte of rgb, MSB heaviest, green/blue byte unchanged; "
            "the manual's own $D00 'Brick red' sample (p.92) does the same "
            "- 'Bits 11-8 Red' (p.46/92), Appendix A's R3..R0 bit map "
            "(p.273). 3-colortable.c14, 3-t3.c02, 3-colorsum.c2, "
            "3-colorsum.c8, 3-csum.c02, 3-t15.c02, A-color.c11, A-color.c12, "
            "A-color.c13, A-color.c14, A-colorxx.bits2, "
            "3-color-samples.c3.");
    }
    {
        Harness h;
        h.write_reg(11, 0x000);
        uint32_t base = h.read_reg(11);

        int bit_for_msb = 7;
#if MUT_GREEN
        bit_for_msb = 11;
#endif
        h.write_reg(11, static_cast<uint16_t>(1u << bit_for_msb));
        uint32_t v7 = h.read_reg(11);
        h.write_reg(11, 1u << 6);
        uint32_t v6 = h.read_reg(11);
        h.write_reg(11, 1u << 5);
        uint32_t v5 = h.read_reg(11);
        h.write_reg(11, 1u << 4);
        uint32_t v4 = h.read_reg(11);

        int dG7 = static_cast<int>(G_of(v7)) - static_cast<int>(G_of(base));
        int dG6 = static_cast<int>(G_of(v6)) - static_cast<int>(G_of(base));
        int dG5 = static_cast<int>(G_of(v5)) - static_cast<int>(G_of(base));
        int dG4 = static_cast<int>(G_of(v4)) - static_cast<int>(G_of(base));
        bool ok =
            R_of(v7) == R_of(base) && B_of(v7) == B_of(base) && dG7 > 0 &&
            R_of(v6) == R_of(base) && B_of(v6) == B_of(base) && dG6 > 0 &&
            R_of(v5) == R_of(base) && B_of(v5) == B_of(base) && dG5 > 0 &&
            R_of(v4) == R_of(base) && B_of(v4) == B_of(base) && dG4 > 0 &&
            dG7 > dG6 && dG6 > dG5 && dG5 > dG4;
        checks.expect(
            ok,
            "GREEN-FIELD-BITS-7-4: bits 7,6,5,4 set alone each move only "
            "the green byte of rgb, MSB heaviest, red/blue byte unchanged - "
            "'Bits 7-4 Green' (p.46/92), Appendix A's G3..G0 bit map "
            "(p.273). 3-colortable.c15, 3-t3.c03, 3-colorsum.c3, "
            "3-colorsum.c9, 3-csum.c03, 3-t15.c03, A-color.c15, A-color.c16, "
            "A-color.c17, A-color.c18, A-colorxx.bits3.");
    }
    {
        Harness h;
        h.write_reg(12, 0x000);
        uint32_t base = h.read_reg(12);

        int bit_for_msb = 3;
#if MUT_BLUE
        bit_for_msb = 7;
#endif
        h.write_reg(12, static_cast<uint16_t>(1u << bit_for_msb));
        uint32_t v3 = h.read_reg(12);
        h.write_reg(12, 1u << 2);
        uint32_t v2 = h.read_reg(12);
        h.write_reg(12, 1u << 1);
        uint32_t v1 = h.read_reg(12);
        h.write_reg(12, 1u << 0);
        uint32_t v0 = h.read_reg(12);

        int dB3 = static_cast<int>(B_of(v3)) - static_cast<int>(B_of(base));
        int dB2 = static_cast<int>(B_of(v2)) - static_cast<int>(B_of(base));
        int dB1 = static_cast<int>(B_of(v1)) - static_cast<int>(B_of(base));
        int dB0 = static_cast<int>(B_of(v0)) - static_cast<int>(B_of(base));
        bool ok =
            R_of(v3) == R_of(base) && G_of(v3) == G_of(base) && dB3 > 0 &&
            R_of(v2) == R_of(base) && G_of(v2) == G_of(base) && dB2 > 0 &&
            R_of(v1) == R_of(base) && G_of(v1) == G_of(base) && dB1 > 0 &&
            R_of(v0) == R_of(base) && G_of(v0) == G_of(base) && dB0 > 0 &&
            dB3 > dB2 && dB2 > dB1 && dB1 > dB0;
        checks.expect(
            ok,
            "BLUE-FIELD-BITS-3-0: bits 3,2,1,0 set alone each move only "
            "the blue byte of rgb, MSB heaviest, red/green byte unchanged - "
            "'Bits 3-0 Blue' (p.46/92), Appendix A's B3..B0 bit map "
            "(p.273). 3-colortable.c16, 3-t3.c04, 3-colorsum.c4, "
            "3-colorsum.c10, 3-csum.c04, 3-t15.c04, A-color.c19, "
            "A-color.c20, A-color.c21, A-color.c22, A-colorxx.bits4.");
    }

    // =========================================================================
    // SAMPLE-nn (p.47/92) -- each of Table 3-4's/3-13's named samples is
    // live: distinguishable from an all-zero baseline on the same register.
    // The manual names a colour, not a numeric RGB target (no printed byte
    // values for "Sky blue" etc.), so distinctness-from-black is the honest
    // ceiling here; the underlying nibble-to-channel mapping these samples
    // instantiate is what RED/GREEN/BLUE-FIELD above actually measure.
    // One call site, 14 dynamic labels (mirrors the WALKING-BIT convention).
    // =========================================================================
    {
        Harness h;
        h.write_reg(1, 0x000);
        uint32_t black = h.read_reg(1);

        struct Sample {
            const char* id;
            int page;
            uint16_t code;
            const char* name;
        };
        static const Sample samples[14] = {
            {"3-t4.c02", 47, 0x6FE, "Sky blue"},
            {"3-t4.c03", 47, 0xDB9, "Tan"},
            {"3-color-samples.c2", 92, 0x1FB, "Light aqua"},
            {"3-color-samples.c6", 92, 0x6CE, "Light blue"},
            {"3-color-samples.c10", 92, 0x61F, "Bright blue"},
            {"3-color-samples.c12", 92, 0x06D, "Dark blue"},
            {"3-color-samples.c13", 92, 0xFD0, "Cadmium yellow"},
            {"3-color-samples.c15", 92, 0xFF0, "Lemon yellow"},
            {"3-color-samples.c19", 92, 0x8E0, "Light green"},
            {"3-color-samples.c23", 92, 0x2C0, "Dark green"},
            {"3-color-samples.c25", 92, 0x0B1, "Forest green"},
            {"3-color-samples.c26", 92, 0xA87, "Dark brown"},
            {"3-color-samples.c28", 92, 0xCCC, "Light grey"},
            {"3-color-samples.c30", 92, 0x999, "Medium grey"},
        };
        for (const auto& s : samples) {
            uint16_t stim = s.code;
#if MUT_SAMPLE
            if (std::strcmp(s.id, "3-color-samples.c19") == 0) stim = 0x000;
#endif
            h.write_reg(1, stim);
            uint32_t val = h.read_reg(1);
            checks.expect(
                val != black,
                std::string("SAMPLE ") + s.name + " " + hex3(s.code) + " (p." + std::to_string(s.page) +
                    "): the table's own code reads back different from an "
                    "all-zero baseline on the same register - live, not "
                    "degenerate to black. " + s.id + ".");
        }
    }

    // =========================================================================
    // EHB-HALVES-INTENSITY (p.95/271) -- selecting a register with select[5]
    // (bitplane 6) set halves each of the three rgb component bytes versus
    // the same register selected without it.
    // =========================================================================
    {
        Harness h;
        h.write_reg(15, 0xC84);
        uint32_t normal = h.read_reg(15, false);

        int ehb_read_idx = 15;
#if MUT_EHB
        h.write_reg(16, 0x248);
        ehb_read_idx = 16;
#endif
        uint32_t ehb = h.read_reg(ehb_read_idx, true);

        bool ok = R_of(ehb) == static_cast<uint8_t>(R_of(normal) / 2) &&
                  G_of(ehb) == static_cast<uint8_t>(G_of(normal) / 2) &&
                  B_of(ehb) == static_cast<uint8_t>(B_of(normal) / 2);
        checks.expect(
            ok,
            "EHB-HALVES-INTENSITY: selecting register 15 with select[5] "
            "(bitplane 6) set reads back each component byte as the "
            "normal reading's component halved - 'the information in "
            "bitplane 6 controls an intensity reduction ... shifted to "
            "half-intensity by the sixth bitplane' (p.95), restated "
            "identically in Appendix A's HOMOD legend (p.271). PACKAGE.md's "
            "own disclosed evidence for these ids (denise_colortable.v:52,"
            "61-66) is what turns 'half-intensity' into 'component >> 1' "
            "here; nothing else about the RTL body was read to write this "
            "assertion. 3-ehb.c06, 3-ehb.c07, 3-ehb.c6, A-bplcon.c66, "
            "A-leg.homod4.");

        int normal2_read_idx = 15;
#if MUT_EHBCTRL
        normal2_read_idx = 16;
#endif
        uint32_t normal2 = h.read_reg(normal2_read_idx, false);
        checks.expect(
            normal2 == normal,
            "[control] EHB-CONTROL-NORMAL-REPRODUCIBLE: re-reading register "
            "15 in normal mode after the EHB reading reproduces the same "
            "value recorded before it - rules out a combinational glitch "
            "or ehb_en leaking into the non-EHB reading. No manual page: "
            "the control for EHB-HALVES-INTENSITY, on the same measurement "
            "basis.");
    }

    // =========================================================================
    // NO-READBACK-PORT (p.47/92) -- the only port that reflects a written
    // value is rgb, gated by select ("look at the screen"): confirms the
    // one path that exists behaves as such (no separate register-content
    // read bus is in PACKAGE.md's own port list for this boundary).
    // =========================================================================
    {
        Harness h;
        h.write_reg(20, 0x357);
        int read_idx = 20;
#if MUT_WO
        read_idx = 21;
#endif
        uint32_t seen = h.read_reg(read_idx);
        h.write_reg(20, 0x357 ^ 0xFFF);
        uint32_t seen2 = h.read_reg(read_idx);
        checks.expect(
            seen != seen2,
            "NO-READBACK-PORT: the value seen at rgb through select=20 "
            "tracks a fresh write to register 20 - the only way this "
            "boundary makes a written colour observable is by selecting it "
            "and reading rgb; PACKAGE.md's port list for denise_colortable "
            "(reg_address_in/data_in/rdram/select/bank/loct/ehb_en in, rgb "
            "out) has no separate register-content read bus - 'The color "
            "registers are write-only ... Only by looking at the screen can "
            "you find out the contents' (p.47), 'All color registers are "
            "write-only' (p.92). 3-colortable.c21, 3-ct.c10, "
            "3-colorsum.c5.");
    }

    // =========================================================================
    // APPC-SHR-REG0-3-BASELINE-NIBBLES (p.301) -- Appendix C's SuperHires
    // section states the baseline it contrasts SuperHires against: outside
    // SuperHires mode, each of the four registers COLOR0-COLOR3 the claim
    // names holds a 12-bit code split into three independent 4-bit R/G/B
    // fields (bits 11-8/7-4/3-0), the same field positions RED-FIELD-BITS-
    // 11-8/GREEN-FIELD-BITS-7-4/BLUE-FIELD-BITS-3-0 establish above on a
    // different register (10/11/12). Tested here on all four registers the
    // claim names, not one, because "four color registers" is the claim -
    // one register passing would not establish it. Each of the twelve bits
    // (four per channel) is walked alone, MSB heaviest -- the same control
    // RED-FIELD-BITS-11-8 uses on register 10 above -- rather than a
    // whole-nibble stimulus: a register that stored only some of a
    // nibble's bits would still pass a whole-nibble ($F00/$0F0/$00F)
    // stimulus, because that only checks that the byte moved at all, not
    // that all four bits of the nibble each move it. This claim bundles all
    // three channels in one
    // sentence, so it gets one assertion of its own rather than a citation
    // split across those three single-channel groups. This module has no
    // SuperHires-mode port at all, so this measurement already sits
    // outside SuperHires by construction.
    // =========================================================================
    {
        Harness h;
        bool ok = true;
        for (int idx = 0; idx < 4; ++idx) {
            h.write_reg(idx, 0x000);
            uint32_t base = h.read_reg(idx);

            int red_msb = 11;
#if MUT_APPCSHR
            if (idx == 0) red_msb = 7;
#endif
            h.write_reg(idx, static_cast<uint16_t>(1u << red_msb));
            uint32_t r11 = h.read_reg(idx);
            h.write_reg(idx, 1u << 10);
            uint32_t r10 = h.read_reg(idx);
            h.write_reg(idx, 1u << 9);
            uint32_t r9 = h.read_reg(idx);
            h.write_reg(idx, 1u << 8);
            uint32_t r8 = h.read_reg(idx);

            h.write_reg(idx, 1u << 7);
            uint32_t g7 = h.read_reg(idx);
            h.write_reg(idx, 1u << 6);
            uint32_t g6 = h.read_reg(idx);
            h.write_reg(idx, 1u << 5);
            uint32_t g5 = h.read_reg(idx);
            h.write_reg(idx, 1u << 4);
            uint32_t g4 = h.read_reg(idx);

            h.write_reg(idx, 1u << 3);
            uint32_t b3 = h.read_reg(idx);
            h.write_reg(idx, 1u << 2);
            uint32_t b2 = h.read_reg(idx);
            h.write_reg(idx, 1u << 1);
            uint32_t b1 = h.read_reg(idx);
            h.write_reg(idx, 1u << 0);
            uint32_t b0 = h.read_reg(idx);

            int dR11 = static_cast<int>(R_of(r11)) - static_cast<int>(R_of(base));
            int dR10 = static_cast<int>(R_of(r10)) - static_cast<int>(R_of(base));
            int dR9  = static_cast<int>(R_of(r9))  - static_cast<int>(R_of(base));
            int dR8  = static_cast<int>(R_of(r8))  - static_cast<int>(R_of(base));
            int dG7 = static_cast<int>(G_of(g7)) - static_cast<int>(G_of(base));
            int dG6 = static_cast<int>(G_of(g6)) - static_cast<int>(G_of(base));
            int dG5 = static_cast<int>(G_of(g5)) - static_cast<int>(G_of(base));
            int dG4 = static_cast<int>(G_of(g4)) - static_cast<int>(G_of(base));
            int dB3 = static_cast<int>(B_of(b3)) - static_cast<int>(B_of(base));
            int dB2 = static_cast<int>(B_of(b2)) - static_cast<int>(B_of(base));
            int dB1 = static_cast<int>(B_of(b1)) - static_cast<int>(B_of(base));
            int dB0 = static_cast<int>(B_of(b0)) - static_cast<int>(B_of(base));

            bool reg_ok =
                G_of(r11) == G_of(base) && B_of(r11) == B_of(base) && dR11 > 0 &&
                G_of(r10) == G_of(base) && B_of(r10) == B_of(base) && dR10 > 0 &&
                G_of(r9)  == G_of(base) && B_of(r9)  == B_of(base) && dR9  > 0 &&
                G_of(r8)  == G_of(base) && B_of(r8)  == B_of(base) && dR8  > 0 &&
                dR11 > dR10 && dR10 > dR9 && dR9 > dR8 &&
                R_of(g7) == R_of(base) && B_of(g7) == B_of(base) && dG7 > 0 &&
                R_of(g6) == R_of(base) && B_of(g6) == B_of(base) && dG6 > 0 &&
                R_of(g5) == R_of(base) && B_of(g5) == B_of(base) && dG5 > 0 &&
                R_of(g4) == R_of(base) && B_of(g4) == B_of(base) && dG4 > 0 &&
                dG7 > dG6 && dG6 > dG5 && dG5 > dG4 &&
                R_of(b3) == R_of(base) && G_of(b3) == G_of(base) && dB3 > 0 &&
                R_of(b2) == R_of(base) && G_of(b2) == G_of(base) && dB2 > 0 &&
                R_of(b1) == R_of(base) && G_of(b1) == G_of(base) && dB1 > 0 &&
                R_of(b0) == R_of(base) && G_of(b0) == G_of(base) && dB0 > 0 &&
                dB3 > dB2 && dB2 > dB1 && dB1 > dB0;
            ok = ok && reg_ok;
        }
        checks.expect(
            ok,
            "APPC-SHR-REG0-3-BASELINE-NIBBLES: on each of COLOR0, COLOR1, "
            "COLOR2 and COLOR3 in turn, each of the twelve bits (four per "
            "channel, MSB heaviest) set alone moves only its own byte of "
            "rgb versus an all-zero baseline on that same register, the "
            "other two bytes unchanged, holding on all four registers - "
            "outside SuperHires mode, COLOR0-COLOR3 each hold 4 bits each "
            "of red, green and blue stored as 0x0RGB in four color "
            "registers (p.301). APPC-shr.c7.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
