// ch8-keycode-table -- keymap conformance test
//
// Test purpose: exhaustively check `keymap`'s combinational lookup table
//   (amiga_keymap.v:10-13) against every "key X carries code Y" claim in
//   ch8-keycode-table.jsonl (446 claims, page 252's two text tables and page
//   254's two keyboard diagrams -- Figure 8-9 the Amiga 1000, Figure 8-10
//   the Amiga 500/2000/3000). SPECIFICATION.md (committed before this file,
//   or amiga_keymap.v/hid.v, were opened -- see its own "Order of work"
//   section) groups the 446 raw claims into 229 propositions; this file
//   grades or declines every one of them. A red result below means
//   amiga_keymap.v disagrees with the manual, not that the assertion is
//   wrong -- no assertion here was, or should be, adjusted to make the RTL
//   pass.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp.251, 252, 254.
// Non-goals: anything outside the `keymap` fixture this package builds, and
//   any claim outside the 446 it cites. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: 446 ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// SCOPE / TRANSFERABILITY (required by 2026-09-16_ch8_s3_build_brief.md):
//   every red this package can produce is a statement about NanoMig, not
//   about the Minimig family. Upstream Minimig-AGA_MiSTer at da7632e has
//   no amiga_keymap.v, no module `keymap` and no hid.v at all --
//   rtl/minimig.v:207 takes kbd_mouse_data as a top-level input and the
//   HID-to-Amiga translation happens above the chipset, in software.
//
// BOUNDARY: keymap(input [6:0] code, output [6:0] amiga) -- two ports,
//   purely combinational (amiga_keymap.v:15-142, one `assign`). No clock
//   is needed: dut->eval() after setting `code` is a complete stimulus.
//
// ORDER OF WORK: SPECIFICATION.md (446 claims -> 229 propositions, from
//   extract.quote/extract.restatement alone) was written and committed --
//   commit 8972201 -- before amiga_keymap.v or hid.v were opened.
//
// CLAIM COVERAGE: every one of the 446 claim uids in the claims file is
//   cited on exactly one line below (graded expect() or declined
//   record()) -- verified mechanically against the claims file before
//   this source was written (a script diffed the full uid set against
//   every citation list used here: 0 missing, 0 spurious).
//
// CANDIDATE S3-C1 (the two international cutout keys, p.252): the key
//   cut out beside left Shift is claimed at $30 by both p.252 text and
//   both p.254 figures; amiga_keymap.v never emits $30 anywhere in the
//   table (confirmed: a script checked every one of the table's 98
//   distinct output values against $30 and found no match) -- graded
//   red below as CUTOUT-30. The key cut out of Return is claimed at $2B
//   (p.252 text; p.254's Figure 8-10 only -- Figure 8-9 does not draw
//   this key) and amiga_keymap.v's EUR-1 entry (:72) does emit $2B --
//   graded green below as CUTOUT-2B, the required control on the same
//   measurement basis (same table, same two-key mechanism).
//
//   WHICH STIMULUS DRIVES WHICH PHYSICAL KEY, WITH ITS OWN ORACLE:
//   amiga_keymap.v calls its two cutout inputs only "EUR-1" (:72, code
//   7'h32) and "EUR-2" (:130, code 7'h64), naming neither key, and the
//   manual does not describe HID scancodes at all -- the USB HID Usage
//   Tables spec is the oracle for that binding, external to the manual,
//   independently readable at
//   _oracle/Denise/driver/input/iokit/keyNames.cpp: usage $32 (line 149)
//   is "Non-US Hash and Tilde", the ISO key beside RETURN; usage $64
//   (line 199) is "Keyboard Non-US Backslash and Pipe", the ISO key
//   beside LEFT SHIFT. So CUTOUT-30 (the left-Shift key) drives $64 and
//   CUTOUT-2B (the Return key) drives $32 -- see each assertion's own
//   comment below; NM-HRM-050 (coordinator review) caught an earlier
//   draft that had the two swapped, which made CUTOUT-30 red for the
//   wrong key and left CUTOUT-2B green while sitting on the actual
//   defect. Corrected before commit.
//
// CANDIDATE S3-C2 (right Alt, p.252 vs p.254): p.252's qualifier table
//   reads $64 Left Alt / $65 Right Alt. Both p.254 figures print $64 on
//   BOTH Alt keys in both keyboards; $65 appears on no key in either
//   figure. Measured directly against the RTL for this file (not just
//   read off the candidates note): amiga_keymap.v's own right-alt input
//   (:139, code 7'h6e) emits $65, matching p.252 and contradicting both
//   figures. This is a disagreement between two printed pages, not an
//   RTL divergence -- graded neither way below (RIGHTALT-CONFLICT,
//   record()+note() only, no expect()).
//
// A CORRECTION belonging to this file, not to SPECIFICATION.md (that
//   file was committed before the RTL was read, so it could not contain
//   this check): 2026-09-16_ch8_s3_finding_candidates.md states "Five
//   claim records carry source_defines: conflict: page 252 versus page
//   254." The claims file itself carries that flag on only FOUR records
//   (opus:8-fig.c75, sol:8-layout.c77, opus:8-fig.c164, sol:8-layout.c168
//   -- the four page-254 right-Alt figure claims). The two page-252
//   right-Alt claims (opus:8-keycode.c42, sol:8-keycode.c67) are each
//   marked plainly `source_defines: yes, page 252`, not `conflict`, in
//   the claims file. This does not change the handling: all six claims
//   are declined together below, for the same reason.
//
// THREE THINGS NOT RAISED (2026-09-16_ch8_s3_finding_candidates.md):
//   PageDown also produces $67 (alongside right-meta); $5F is reachable
//   from both Insert and End; there is no right Control key. None of the
//   446 claims in this package is of the shape these three would speak
//   to (every claim here says "this key's own code produces this
//   output", never "this output is reachable only from this input"), so
//   none of the three is asserted against below -- this file just picks
//   one of the two available inputs where the table offers more than one
//   path to a claimed output ($5F via End's own code 7'h4d; $67 via
//   right-meta's own code 7'h6f, the semantically primary path for each).
//
// PER-ASSERTION MUTATION RECORD. Three build variants beyond baseline
//   (`make`, `make MUT=MUT_ZERO`, `make MUT=MUT_ASCII_COLLIDE`,
//   `make MUT=MUT_FIXTURE_ZERO`), each a full build+run. Every KEY-$xx row
//   below (94 of them, one per distinct Amiga output code the
//   ordinary/control rows cover) and FUNCTION-RANGE share MUT_ZERO: it
//   redirects the harness's stimulus for every key to code 7'h00, which
//   amiga_keymap.v maps to nothing (falls through to the 7'h7f sentinel,
//   amiga_keymap.v:142) -- since no real claim anywhere in this package
//   expects $7F, MUT_ZERO turns every one of those assertions red
//   simultaneously, independently proving each one actually depends on
//   its own specific stimulus and not just on the fixture being alive.
//   FIXTURE-SELFCHECK -- that fixture-liveness precondition itself -- has
//   its own dedicated MUT_FIXTURE_ZERO (same $00->$7F substitution,
//   applied only to its own single, standalone sample() call, never to
//   the ordinary_rows loop MUT_ZERO covers, so the two mutations cannot
//   be confused for one another): a control's whole job is proving it
//   CAN go red, and until now nothing did. GEN-NOT-ASCII has its own
//   dedicated MUT_ASCII_COLLIDE (substitutes backspace's own code for the
//   letter-a slot, landing on $41 -- ASCII 'A' -- which the baseline
//   condition explicitly checks against). CUTOUT-30 carries `none
//   impossible`: it is red at baseline and, per the S3-C1 measurement
//   above, no stimulus in the table produces $30 at all, so no
//   substitution can turn it green.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include "Vkeymap.h"
#include "verilated.h"

#ifndef MUT_ZERO
#define MUT_ZERO 0
#endif
#ifndef MUT_ASCII_COLLIDE
#define MUT_ASCII_COLLIDE 0
#endif
#ifndef MUT_FIXTURE_ZERO
#define MUT_FIXTURE_ZERO 0
#endif

namespace {

// Purely combinational: driving `code` and calling eval() is a complete
// stimulus. No clock, no reset, no internal state.
struct Harness {
    Vkeymap* dut;
    Harness() : dut(new Vkeymap) { dut->code = 0; dut->eval(); }
    ~Harness() { delete dut; }
    uint8_t sample(uint8_t code) {
        dut->code = code & 0x7f;
        dut->eval();
        return dut->amiga & 0x7f;
    }
};

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

std::string hex2(uint8_t v) {
    char buf[4];
    std::snprintf(buf, sizeof buf, "%02X", v);
    return std::string(buf);
}

struct KeyRow {
    const char* token;       // e.g. "KEY-$10" -- unique, matches the MUTATION comment's label field exactly
    uint8_t stim;            // HID-side `code` stimulus (amiga_keymap.v's own input-side comment identifies the key)
    uint8_t expected;        // Amiga-side code the manual claims for this key (from SPECIFICATION.md, not from the RTL)
    const char* text;        // human-readable label, cites its page(s)
    const char* claims;      // trailing [claims: ...] suffix
};

} // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;
    Harness h;

    // =====================================================================
    // FIXTURE-SELFCHECK (no uid; harness precondition, per the build brief's
    // "the fixture proves it can deliver a known value before anything
    // reads one"). Stimulates the 'a' key's own HID code independently of
    // the data-driven loop below, before any claim-bearing assertion runs.
    // =====================================================================
    {
#if MUT_FIXTURE_ZERO
        uint8_t got = h.sample(0x00);
#else
        uint8_t got = h.sample(0x04);
#endif
        checks.expect(got == 0x20,
            "FIXTURE-SELFCHECK: driving code=$04 (the 'a' key's own HID "
            "source code, amiga_keymap.v:21) returns amiga=$20 -- the "
            "fixture can deliver a known, specific value before any claim "
            "below reads one. No manual page (harness precondition).");
    }
// MUTATION FIXTURE-SELFCHECK stimulus red MUT_FIXTURE_ZERO: the harness drives code=$00 instead of the 'a' key's own code ($04); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $20 -- baseline PASS goes red.

    // =====================================================================
    // KEY-$xx rows: every ordinary "key X carries code Y" proposition plus
    // the S3-C1 control (CUTOUT-2B, the international Return cutout).
    // All rows below are expected PASS at baseline. Every row shares one
    // MUTATION (...-stimulus red MUT_ZERO); the full per-row list of
    // MUTATION comment lines follows immediately after the loop.
    // =====================================================================
    static const KeyRow ordinary_rows[] = {
        { "KEY-$00", 0x35, 0x00, "grave/tilde (keyboard(s): A1000, A500/2000/3000; p.254) carries code $00", "[claims: opus:8-fig.c15, sol:8-layout.c13, opus:8-fig.c104, sol:8-layout.c102]" },
        { "KEY-$01", 0x1E, 0x01, "1/exclamation (keyboard(s): A1000, A500/2000/3000; p.254) carries code $01", "[claims: opus:8-fig.c16, sol:8-layout.c14, opus:8-fig.c105, sol:8-layout.c103]" },
        { "KEY-$02", 0x1F, 0x02, "2/at (keyboard(s): A1000, A500/2000/3000; p.254) carries code $02", "[claims: opus:8-fig.c17, sol:8-layout.c15, opus:8-fig.c106, sol:8-layout.c104]" },
        { "KEY-$03", 0x20, 0x03, "3/hash (keyboard(s): A1000, A500/2000/3000; p.254) carries code $03", "[claims: opus:8-fig.c18, sol:8-layout.c16, opus:8-fig.c107, sol:8-layout.c105]" },
        { "KEY-$04", 0x21, 0x04, "4/dollar (keyboard(s): A1000, A500/2000/3000; p.254) carries code $04", "[claims: opus:8-fig.c19, sol:8-layout.c17, opus:8-fig.c108, sol:8-layout.c106]" },
        { "KEY-$05", 0x22, 0x05, "5/percent (keyboard(s): A1000, A500/2000/3000; p.254) carries code $05", "[claims: opus:8-fig.c20, sol:8-layout.c18, opus:8-fig.c109, sol:8-layout.c107]" },
        { "KEY-$06", 0x23, 0x06, "6/caret (keyboard(s): A1000, A500/2000/3000; p.254) carries code $06", "[claims: opus:8-fig.c21, sol:8-layout.c19, opus:8-fig.c110, sol:8-layout.c108]" },
        { "KEY-$07", 0x24, 0x07, "7/ampersand (keyboard(s): A1000, A500/2000/3000; p.254) carries code $07", "[claims: opus:8-fig.c22, sol:8-layout.c20, opus:8-fig.c111, sol:8-layout.c109]" },
        { "KEY-$08", 0x25, 0x08, "8/asterisk (keyboard(s): A1000, A500/2000/3000; p.254) carries code $08", "[claims: opus:8-fig.c23, sol:8-layout.c21, opus:8-fig.c112, sol:8-layout.c110]" },
        { "KEY-$09", 0x26, 0x09, "9/left-parenthesis (keyboard(s): A1000, A500/2000/3000; p.254) carries code $09", "[claims: opus:8-fig.c24, sol:8-layout.c22, opus:8-fig.c113, sol:8-layout.c111]" },
        { "KEY-$0A", 0x27, 0x0A, "0/right-parenthesis (keyboard(s): A1000, A500/2000/3000; p.254) carries code $0A", "[claims: opus:8-fig.c25, sol:8-layout.c23, opus:8-fig.c114, sol:8-layout.c112]" },
        { "KEY-$0B", 0x2D, 0x0B, "minus/underscore (keyboard(s): A1000, A500/2000/3000; p.254) carries code $0B", "[claims: opus:8-fig.c26, sol:8-layout.c24, opus:8-fig.c115, sol:8-layout.c113]" },
        { "KEY-$0C", 0x2E, 0x0C, "equals/plus (keyboard(s): A1000, A500/2000/3000; p.254) carries code $0C", "[claims: opus:8-fig.c27, sol:8-layout.c25, opus:8-fig.c116, sol:8-layout.c114]" },
        { "KEY-$0D", 0x31, 0x0D, "backslash/vertical-bar (keyboard(s): A1000, A500/2000/3000; p.254) carries code $0D", "[claims: opus:8-fig.c28, sol:8-layout.c26, opus:8-fig.c117, sol:8-layout.c115]" },
        { "KEY-$0F", 0x62, 0x0F, "numeric-pad 0 (keyboard(s): A1000, A500/2000/3000; p.254) carries code $0F", "[claims: opus:8-fig.c89, sol:8-layout.c87, opus:8-fig.c187, sol:8-layout.c185]" },
        { "KEY-$10", 0x14, 0x10, "Q (keyboard(s): A1000, A500/2000/3000; p.254) carries code $10", "[claims: opus:8-fig.c31, sol:8-layout.c29, opus:8-fig.c120, sol:8-layout.c124]" },
        { "KEY-$11", 0x1A, 0x11, "W (keyboard(s): A1000, A500/2000/3000; p.254) carries code $11", "[claims: opus:8-fig.c32, sol:8-layout.c30, opus:8-fig.c121, sol:8-layout.c125]" },
        { "KEY-$12", 0x08, 0x12, "E (keyboard(s): A1000, A500/2000/3000; p.254) carries code $12", "[claims: opus:8-fig.c33, sol:8-layout.c31, opus:8-fig.c122, sol:8-layout.c126]" },
        { "KEY-$13", 0x15, 0x13, "R (keyboard(s): A1000, A500/2000/3000; p.254) carries code $13", "[claims: opus:8-fig.c34, sol:8-layout.c32, opus:8-fig.c123, sol:8-layout.c127]" },
        { "KEY-$14", 0x17, 0x14, "T (keyboard(s): A1000, A500/2000/3000; p.254) carries code $14", "[claims: opus:8-fig.c35, sol:8-layout.c33, opus:8-fig.c124, sol:8-layout.c128]" },
        { "KEY-$15", 0x1C, 0x15, "Y (keyboard(s): A1000, A500/2000/3000; p.254) carries code $15", "[claims: opus:8-fig.c36, sol:8-layout.c34, opus:8-fig.c125, sol:8-layout.c129]" },
        { "KEY-$16", 0x18, 0x16, "U (keyboard(s): A1000, A500/2000/3000; p.254) carries code $16", "[claims: opus:8-fig.c37, sol:8-layout.c35, opus:8-fig.c126, sol:8-layout.c130]" },
        { "KEY-$17", 0x0C, 0x17, "I (keyboard(s): A1000, A500/2000/3000; p.254) carries code $17", "[claims: opus:8-fig.c38, sol:8-layout.c36, opus:8-fig.c127, sol:8-layout.c131]" },
        { "KEY-$18", 0x12, 0x18, "O (keyboard(s): A1000, A500/2000/3000; p.254) carries code $18", "[claims: opus:8-fig.c39, sol:8-layout.c37, opus:8-fig.c128, sol:8-layout.c132]" },
        { "KEY-$19", 0x13, 0x19, "P (keyboard(s): A1000, A500/2000/3000; p.254) carries code $19", "[claims: opus:8-fig.c40, sol:8-layout.c38, opus:8-fig.c129, sol:8-layout.c133]" },
        { "KEY-$1A", 0x2F, 0x1A, "left-bracket/left-brace (keyboard(s): A1000, A500/2000/3000; p.254) carries code $1A", "[claims: opus:8-fig.c41, sol:8-layout.c39, opus:8-fig.c130, sol:8-layout.c134]" },
        { "KEY-$1B", 0x30, 0x1B, "right-bracket/right-brace (keyboard(s): A1000, A500/2000/3000; p.254) carries code $1B", "[claims: opus:8-fig.c42, sol:8-layout.c40, opus:8-fig.c131, sol:8-layout.c135]" },
        { "KEY-$1D", 0x59, 0x1D, "numeric-pad 1 (keyboard(s): A1000, A500/2000/3000; p.254) carries code $1D", "[claims: opus:8-fig.c86, sol:8-layout.c84, opus:8-fig.c183, sol:8-layout.c181]" },
        { "KEY-$1E", 0x5A, 0x1E, "numeric-pad 2 (keyboard(s): A1000, A500/2000/3000; p.254) carries code $1E", "[claims: opus:8-fig.c87, sol:8-layout.c85, opus:8-fig.c184, sol:8-layout.c182]" },
        { "KEY-$1F", 0x5B, 0x1F, "numeric-pad 3 (keyboard(s): A1000, A500/2000/3000; p.254) carries code $1F", "[claims: opus:8-fig.c88, sol:8-layout.c86, opus:8-fig.c185, sol:8-layout.c183]" },
        { "KEY-$20", 0x04, 0x20, "A (keyboard(s): A1000, A500/2000/3000; p.254) carries code $20", "[claims: opus:8-fig.c46, sol:8-layout.c44, opus:8-fig.c135, sol:8-layout.c139]" },
        { "KEY-$21", 0x16, 0x21, "S (keyboard(s): A1000, A500/2000/3000; p.254) carries code $21", "[claims: opus:8-fig.c47, sol:8-layout.c45, opus:8-fig.c136, sol:8-layout.c140]" },
        { "KEY-$22", 0x07, 0x22, "D (keyboard(s): A1000, A500/2000/3000; p.254) carries code $22", "[claims: opus:8-fig.c48, sol:8-layout.c46, opus:8-fig.c137, sol:8-layout.c141]" },
        { "KEY-$23", 0x09, 0x23, "F (keyboard(s): A1000, A500/2000/3000; p.254) carries code $23", "[claims: opus:8-fig.c49, sol:8-layout.c47, opus:8-fig.c138, sol:8-layout.c142]" },
        { "KEY-$24", 0x0A, 0x24, "G (keyboard(s): A1000, A500/2000/3000; p.254) carries code $24", "[claims: opus:8-fig.c50, sol:8-layout.c48, opus:8-fig.c139, sol:8-layout.c143]" },
        { "KEY-$25", 0x0B, 0x25, "H (keyboard(s): A1000, A500/2000/3000; p.254) carries code $25", "[claims: opus:8-fig.c51, sol:8-layout.c49, opus:8-fig.c140, sol:8-layout.c144]" },
        { "KEY-$26", 0x0D, 0x26, "J (keyboard(s): A1000, A500/2000/3000; p.254) carries code $26", "[claims: opus:8-fig.c52, sol:8-layout.c50, opus:8-fig.c141, sol:8-layout.c145]" },
        { "KEY-$27", 0x0E, 0x27, "K (keyboard(s): A1000, A500/2000/3000; p.254) carries code $27", "[claims: opus:8-fig.c53, sol:8-layout.c51, opus:8-fig.c142, sol:8-layout.c146]" },
        { "KEY-$28", 0x0F, 0x28, "L (keyboard(s): A1000, A500/2000/3000; p.254) carries code $28", "[claims: opus:8-fig.c54, sol:8-layout.c52, opus:8-fig.c143, sol:8-layout.c147]" },
        { "KEY-$29", 0x33, 0x29, "semicolon/colon (keyboard(s): A1000, A500/2000/3000; p.254) carries code $29", "[claims: opus:8-fig.c55, sol:8-layout.c53, opus:8-fig.c144, sol:8-layout.c148]" },
        { "KEY-$2A", 0x34, 0x2A, "apostrophe/quote (keyboard(s): A1000, A500/2000/3000; p.254) carries code $2A", "[claims: opus:8-fig.c56, sol:8-layout.c54, opus:8-fig.c145, sol:8-layout.c149]" },
        { "KEY-$2B", 0x32, 0x2B, "international Return cutout (S3-C1 control; HID usage $32, Non-US Hash and Tilde, beside Return -- keyNames.cpp:149) (keyboard(s): A500/2000/3000; p.252+254) carries code $2B", "[claims: opus:8-fig.c146, sol:8-layout.c150, opus:8-keycode.c15, sol:8-keycode.c14]" },
        { "KEY-$2D", 0x5C, 0x2D, "numeric-pad 4 (keyboard(s): A1000, A500/2000/3000; p.254) carries code $2D", "[claims: opus:8-fig.c83, sol:8-layout.c81, opus:8-fig.c179, sol:8-layout.c177]" },
        { "KEY-$2E", 0x5D, 0x2E, "numeric-pad 5 (keyboard(s): A1000, A500/2000/3000; p.254) carries code $2E", "[claims: opus:8-fig.c84, sol:8-layout.c82, opus:8-fig.c180, sol:8-layout.c178]" },
        { "KEY-$2F", 0x5E, 0x2F, "numeric-pad 6 (keyboard(s): A1000, A500/2000/3000; p.254) carries code $2F", "[claims: opus:8-fig.c85, sol:8-layout.c83, opus:8-fig.c181, sol:8-layout.c179]" },
        { "KEY-$31", 0x1D, 0x31, "Z (keyboard(s): A1000, A500/2000/3000; p.254) carries code $31", "[claims: opus:8-fig.c60, sol:8-layout.c59, opus:8-fig.c149, sol:8-layout.c153]" },
        { "KEY-$32", 0x1B, 0x32, "X (keyboard(s): A1000, A500/2000/3000; p.254) carries code $32", "[claims: opus:8-fig.c61, sol:8-layout.c60, opus:8-fig.c150, sol:8-layout.c154]" },
        { "KEY-$33", 0x06, 0x33, "C (keyboard(s): A1000, A500/2000/3000; p.254) carries code $33", "[claims: opus:8-fig.c62, sol:8-layout.c61, opus:8-fig.c151, sol:8-layout.c155]" },
        { "KEY-$34", 0x19, 0x34, "V (keyboard(s): A1000, A500/2000/3000; p.254) carries code $34", "[claims: opus:8-fig.c63, sol:8-layout.c62, opus:8-fig.c152, sol:8-layout.c156]" },
        { "KEY-$35", 0x05, 0x35, "B (keyboard(s): A1000, A500/2000/3000; p.254) carries code $35", "[claims: opus:8-fig.c64, sol:8-layout.c63, opus:8-fig.c153, sol:8-layout.c157]" },
        { "KEY-$36", 0x11, 0x36, "N (keyboard(s): A1000, A500/2000/3000; p.254) carries code $36", "[claims: opus:8-fig.c65, sol:8-layout.c64, opus:8-fig.c154, sol:8-layout.c158]" },
        { "KEY-$37", 0x10, 0x37, "M (keyboard(s): A1000, A500/2000/3000; p.254) carries code $37", "[claims: opus:8-fig.c66, sol:8-layout.c65, opus:8-fig.c155, sol:8-layout.c159]" },
        { "KEY-$38", 0x36, 0x38, "comma/less-than (keyboard(s): A1000, A500/2000/3000; p.254) carries code $38", "[claims: opus:8-fig.c67, sol:8-layout.c66, opus:8-fig.c156, sol:8-layout.c160]" },
        { "KEY-$39", 0x37, 0x39, "period/greater-than (keyboard(s): A1000, A500/2000/3000; p.254) carries code $39", "[claims: opus:8-fig.c68, sol:8-layout.c67, opus:8-fig.c157, sol:8-layout.c161]" },
        { "KEY-$3A", 0x38, 0x3A, "slash/question (keyboard(s): A1000, A500/2000/3000; p.254) carries code $3A", "[claims: opus:8-fig.c69, sol:8-layout.c68, opus:8-fig.c158, sol:8-layout.c162]" },
        { "KEY-$3C", 0x63, 0x3C, "numeric-pad period (keyboard(s): A1000, A500/2000/3000; p.254) carries code $3C", "[claims: opus:8-fig.c90, sol:8-layout.c88, opus:8-fig.c188, sol:8-layout.c186]" },
        { "KEY-$3D", 0x5F, 0x3D, "numeric-pad 7 (keyboard(s): A1000, A500/2000/3000; p.254) carries code $3D", "[claims: opus:8-fig.c80, sol:8-layout.c78, opus:8-fig.c175, sol:8-layout.c173]" },
        { "KEY-$3E", 0x60, 0x3E, "numeric-pad 8 (keyboard(s): A1000, A500/2000/3000; p.254) carries code $3E", "[claims: opus:8-fig.c81, sol:8-layout.c79, opus:8-fig.c176, sol:8-layout.c174]" },
        { "KEY-$3F", 0x61, 0x3F, "numeric-pad 9 (keyboard(s): A1000, A500/2000/3000; p.254) carries code $3F", "[claims: opus:8-fig.c82, sol:8-layout.c80, opus:8-fig.c177, sol:8-layout.c175]" },
        { "KEY-$40", 0x2C, 0x40, "Space (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $40", "[claims: opus:8-fig.c73, sol:8-layout.c75, opus:8-fig.c162, sol:8-layout.c166, opus:8-keycode.c18, sol:8-keycode.c34]" },
        { "KEY-$41", 0x2A, 0x41, "Backspace (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $41", "[claims: opus:8-fig.c29, sol:8-layout.c27, opus:8-fig.c118, sol:8-layout.c116, opus:8-keycode.c19, sol:8-keycode.c35]" },
        { "KEY-$42", 0x2B, 0x42, "Tab (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $42", "[claims: opus:8-fig.c30, sol:8-layout.c28, opus:8-fig.c119, sol:8-layout.c123, opus:8-keycode.c20, sol:8-keycode.c36]" },
        { "KEY-$43", 0x58, 0x43, "numeric-pad Enter (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $43", "[claims: opus:8-fig.c92, sol:8-layout.c90, opus:8-fig.c186, sol:8-layout.c184, opus:8-keycode.c21, sol:8-keycode.c37]" },
        { "KEY-$44", 0x28, 0x44, "Return (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $44", "[claims: opus:8-fig.c57, sol:8-layout.c55, opus:8-fig.c132, sol:8-layout.c136, opus:8-keycode.c22, sol:8-keycode.c38]" },
        { "KEY-$45", 0x29, 0x45, "Escape (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $45", "[claims: opus:8-fig.c3, sol:8-layout.c1, opus:8-fig.c93, sol:8-layout.c91, opus:8-keycode.c23, sol:8-keycode.c39]" },
        { "KEY-$46", 0x4C, 0x46, "Delete (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $46", "[claims: opus:8-fig.c14, sol:8-layout.c12, opus:8-fig.c165, sol:8-layout.c117, opus:8-keycode.c24, sol:8-keycode.c40]" },
        { "KEY-$4A", 0x56, 0x4A, "numeric-pad minus (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $4A", "[claims: opus:8-fig.c91, sol:8-layout.c89, opus:8-fig.c178, sol:8-layout.c176, opus:8-keycode.c25, sol:8-keycode.c41]" },
        { "KEY-$4C", 0x52, 0x4C, "cursor up (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $4C", "[claims: opus:8-fig.c76, sol:8-layout.c56, opus:8-fig.c167, sol:8-layout.c169, opus:8-keycode.c26, sol:8-keycode.c42]" },
        { "KEY-$4D", 0x51, 0x4D, "cursor down (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $4D", "[claims: opus:8-fig.c79, sol:8-layout.c72, opus:8-fig.c169, sol:8-layout.c171, opus:8-keycode.c27, sol:8-keycode.c43]" },
        { "KEY-$4E", 0x4F, 0x4E, "cursor right (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $4E", "[claims: opus:8-fig.c78, sol:8-layout.c71, opus:8-fig.c170, sol:8-layout.c172, opus:8-keycode.c28, sol:8-keycode.c44]" },
        { "KEY-$4F", 0x50, 0x4F, "cursor left (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $4F", "[claims: opus:8-fig.c77, sol:8-layout.c70, opus:8-fig.c168, sol:8-layout.c170, opus:8-keycode.c29, sol:8-keycode.c45]" },
        { "KEY-$50", 0x3A, 0x50, "F1 (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $50", "[claims: opus:8-fig.c4, sol:8-layout.c2, opus:8-fig.c94, sol:8-layout.c92, sol:8-keycode.c46]" },
        { "KEY-$51", 0x3B, 0x51, "F2 (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $51", "[claims: opus:8-fig.c5, sol:8-layout.c3, opus:8-fig.c95, sol:8-layout.c93, sol:8-keycode.c47]" },
        { "KEY-$52", 0x3C, 0x52, "F3 (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $52", "[claims: opus:8-fig.c6, sol:8-layout.c4, opus:8-fig.c96, sol:8-layout.c94, sol:8-keycode.c48]" },
        { "KEY-$53", 0x3D, 0x53, "F4 (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $53", "[claims: opus:8-fig.c7, sol:8-layout.c5, opus:8-fig.c97, sol:8-layout.c95, sol:8-keycode.c49]" },
        { "KEY-$54", 0x3E, 0x54, "F5 (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $54", "[claims: opus:8-fig.c8, sol:8-layout.c6, opus:8-fig.c98, sol:8-layout.c96, sol:8-keycode.c50]" },
        { "KEY-$55", 0x3F, 0x55, "F6 (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $55", "[claims: opus:8-fig.c9, sol:8-layout.c7, opus:8-fig.c99, sol:8-layout.c97, sol:8-keycode.c51]" },
        { "KEY-$56", 0x40, 0x56, "F7 (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $56", "[claims: opus:8-fig.c10, sol:8-layout.c8, opus:8-fig.c100, sol:8-layout.c98, sol:8-keycode.c52]" },
        { "KEY-$57", 0x41, 0x57, "F8 (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $57", "[claims: opus:8-fig.c11, sol:8-layout.c9, opus:8-fig.c101, sol:8-layout.c99, sol:8-keycode.c53]" },
        { "KEY-$58", 0x42, 0x58, "F9 (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $58", "[claims: opus:8-fig.c12, sol:8-layout.c10, opus:8-fig.c102, sol:8-layout.c100, sol:8-keycode.c54]" },
        { "KEY-$59", 0x43, 0x59, "F10 (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $59", "[claims: opus:8-fig.c13, sol:8-layout.c11, opus:8-fig.c103, sol:8-layout.c101, sol:8-keycode.c55]" },
        { "KEY-$5A", 0x4A, 0x5A, "numeric-pad left parenthesis (keyboard(s): A500/2000/3000; p.252+254) carries code $5A", "[claims: opus:8-fig.c171, sol:8-layout.c119, opus:8-keycode.c31, sol:8-keycode.c56]" },
        { "KEY-$5B", 0x4B, 0x5B, "numeric-pad right parenthesis (keyboard(s): A500/2000/3000; p.252+254) carries code $5B", "[claims: opus:8-fig.c172, sol:8-layout.c120, opus:8-keycode.c32, sol:8-keycode.c57]" },
        { "KEY-$5C", 0x54, 0x5C, "numeric-pad slash (keyboard(s): A500/2000/3000; p.252+254) carries code $5C", "[claims: opus:8-fig.c173, sol:8-layout.c121, opus:8-keycode.c33, sol:8-keycode.c58]" },
        { "KEY-$5D", 0x55, 0x5D, "numeric-pad asterisk (keyboard(s): A500/2000/3000; p.252+254) carries code $5D", "[claims: opus:8-fig.c174, sol:8-layout.c122, opus:8-keycode.c34, sol:8-keycode.c59]" },
        { "KEY-$5E", 0x57, 0x5E, "numeric-pad plus (keyboard(s): A500/2000/3000; p.252+254) carries code $5E", "[claims: opus:8-fig.c182, sol:8-layout.c180, opus:8-keycode.c35, sol:8-keycode.c60]" },
        { "KEY-$5F", 0x4D, 0x5F, "Help (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $5F", "[claims: opus:8-fig.c43, sol:8-layout.c41, opus:8-fig.c166, sol:8-layout.c118, opus:8-keycode.c36, sol:8-keycode.c61]" },
        { "KEY-$60", 0x69, 0x60, "left Shift (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $60", "[claims: opus:8-fig.c58, sol:8-layout.c57, opus:8-fig.c147, sol:8-layout.c151, opus:8-keycode.c37, sol:8-keycode.c62]" },
        { "KEY-$61", 0x6D, 0x61, "right Shift (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $61", "[claims: opus:8-fig.c70, sol:8-layout.c69, opus:8-fig.c159, sol:8-layout.c163, opus:8-keycode.c38, sol:8-keycode.c63]" },
        { "KEY-$62", 0x39, 0x62, "Caps Lock (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $62", "[claims: opus:8-fig.c45, sol:8-layout.c43, opus:8-fig.c134, sol:8-layout.c138, opus:8-keycode.c39, sol:8-keycode.c64]" },
        { "KEY-$63", 0x68, 0x63, "Control (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $63", "[claims: opus:8-fig.c44, sol:8-layout.c42, opus:8-fig.c133, sol:8-layout.c137, opus:8-keycode.c40, sol:8-keycode.c65]" },
        { "KEY-$64", 0x6A, 0x64, "left Alt (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $64", "[claims: opus:8-fig.c71, sol:8-layout.c73, opus:8-fig.c160, sol:8-layout.c164, opus:8-keycode.c41, sol:8-keycode.c66]" },
        { "KEY-$66", 0x6B, 0x66, "left Amiga (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $66", "[claims: opus:8-fig.c72, sol:8-layout.c74, opus:8-fig.c161, sol:8-layout.c165, opus:8-keycode.c43, sol:8-keycode.c68]" },
        { "KEY-$67", 0x6F, 0x67, "right Amiga (keyboard(s): A1000, A500/2000/3000; p.252+254) carries code $67", "[claims: opus:8-fig.c74, sol:8-layout.c76, opus:8-fig.c163, sol:8-layout.c167, opus:8-keycode.c44, sol:8-keycode.c69]" },
    };

    for (const auto& row : ordinary_rows) {
        uint8_t stim = row.stim;
#if MUT_ZERO
        stim = 0x00;
#endif
        uint8_t got = h.sample(stim);
        std::string label = std::string(row.token) + ": " + row.text + " " + row.claims;
        checks.expect(got == row.expected, label);
    }

    // One MUTATION line per KEY-$xx row above (94 of them), each sharing
    // the same MUT_ZERO build variant (see file header for why one shared
    // build validly covers every row: each row's own expected value is
    // distinct, and $7F -- what MUT_ZERO forces every stimulus to decode
    // to -- matches none of them).
// MUTATION KEY-$00 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($35); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $00 -- baseline PASS goes red.
// MUTATION KEY-$01 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($1E); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $01 -- baseline PASS goes red.
// MUTATION KEY-$02 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($1F); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $02 -- baseline PASS goes red.
// MUTATION KEY-$03 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($20); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $03 -- baseline PASS goes red.
// MUTATION KEY-$04 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($21); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $04 -- baseline PASS goes red.
// MUTATION KEY-$05 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($22); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $05 -- baseline PASS goes red.
// MUTATION KEY-$06 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($23); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $06 -- baseline PASS goes red.
// MUTATION KEY-$07 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($24); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $07 -- baseline PASS goes red.
// MUTATION KEY-$08 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($25); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $08 -- baseline PASS goes red.
// MUTATION KEY-$09 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($26); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $09 -- baseline PASS goes red.
// MUTATION KEY-$0A stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($27); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $0A -- baseline PASS goes red.
// MUTATION KEY-$0B stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($2D); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $0B -- baseline PASS goes red.
// MUTATION KEY-$0C stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($2E); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $0C -- baseline PASS goes red.
// MUTATION KEY-$0D stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($31); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $0D -- baseline PASS goes red.
// MUTATION KEY-$0F stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($62); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $0F -- baseline PASS goes red.
// MUTATION KEY-$10 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($14); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $10 -- baseline PASS goes red.
// MUTATION KEY-$11 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($1A); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $11 -- baseline PASS goes red.
// MUTATION KEY-$12 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($08); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $12 -- baseline PASS goes red.
// MUTATION KEY-$13 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($15); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $13 -- baseline PASS goes red.
// MUTATION KEY-$14 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($17); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $14 -- baseline PASS goes red.
// MUTATION KEY-$15 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($1C); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $15 -- baseline PASS goes red.
// MUTATION KEY-$16 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($18); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $16 -- baseline PASS goes red.
// MUTATION KEY-$17 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($0C); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $17 -- baseline PASS goes red.
// MUTATION KEY-$18 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($12); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $18 -- baseline PASS goes red.
// MUTATION KEY-$19 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($13); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $19 -- baseline PASS goes red.
// MUTATION KEY-$1A stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($2F); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $1A -- baseline PASS goes red.
// MUTATION KEY-$1B stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($30); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $1B -- baseline PASS goes red.
// MUTATION KEY-$1D stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($59); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $1D -- baseline PASS goes red.
// MUTATION KEY-$1E stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($5A); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $1E -- baseline PASS goes red.
// MUTATION KEY-$1F stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($5B); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $1F -- baseline PASS goes red.
// MUTATION KEY-$20 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($04); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $20 -- baseline PASS goes red.
// MUTATION KEY-$21 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($16); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $21 -- baseline PASS goes red.
// MUTATION KEY-$22 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($07); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $22 -- baseline PASS goes red.
// MUTATION KEY-$23 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($09); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $23 -- baseline PASS goes red.
// MUTATION KEY-$24 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($0A); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $24 -- baseline PASS goes red.
// MUTATION KEY-$25 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($0B); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $25 -- baseline PASS goes red.
// MUTATION KEY-$26 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($0D); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $26 -- baseline PASS goes red.
// MUTATION KEY-$27 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($0E); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $27 -- baseline PASS goes red.
// MUTATION KEY-$28 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($0F); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $28 -- baseline PASS goes red.
// MUTATION KEY-$29 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($33); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $29 -- baseline PASS goes red.
// MUTATION KEY-$2A stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($34); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $2A -- baseline PASS goes red.
// MUTATION KEY-$2B stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($32); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $2B -- baseline PASS goes red.
// MUTATION KEY-$2D stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($5C); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $2D -- baseline PASS goes red.
// MUTATION KEY-$2E stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($5D); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $2E -- baseline PASS goes red.
// MUTATION KEY-$2F stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($5E); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $2F -- baseline PASS goes red.
// MUTATION KEY-$31 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($1D); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $31 -- baseline PASS goes red.
// MUTATION KEY-$32 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($1B); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $32 -- baseline PASS goes red.
// MUTATION KEY-$33 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($06); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $33 -- baseline PASS goes red.
// MUTATION KEY-$34 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($19); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $34 -- baseline PASS goes red.
// MUTATION KEY-$35 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($05); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $35 -- baseline PASS goes red.
// MUTATION KEY-$36 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($11); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $36 -- baseline PASS goes red.
// MUTATION KEY-$37 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($10); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $37 -- baseline PASS goes red.
// MUTATION KEY-$38 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($36); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $38 -- baseline PASS goes red.
// MUTATION KEY-$39 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($37); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $39 -- baseline PASS goes red.
// MUTATION KEY-$3A stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($38); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $3A -- baseline PASS goes red.
// MUTATION KEY-$3C stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($63); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $3C -- baseline PASS goes red.
// MUTATION KEY-$3D stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($5F); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $3D -- baseline PASS goes red.
// MUTATION KEY-$3E stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($60); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $3E -- baseline PASS goes red.
// MUTATION KEY-$3F stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($61); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $3F -- baseline PASS goes red.
// MUTATION KEY-$40 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($2C); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $40 -- baseline PASS goes red.
// MUTATION KEY-$41 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($2A); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $41 -- baseline PASS goes red.
// MUTATION KEY-$42 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($2B); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $42 -- baseline PASS goes red.
// MUTATION KEY-$43 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($58); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $43 -- baseline PASS goes red.
// MUTATION KEY-$44 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($28); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $44 -- baseline PASS goes red.
// MUTATION KEY-$45 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($29); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $45 -- baseline PASS goes red.
// MUTATION KEY-$46 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($4C); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $46 -- baseline PASS goes red.
// MUTATION KEY-$4A stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($56); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $4A -- baseline PASS goes red.
// MUTATION KEY-$4C stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($52); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $4C -- baseline PASS goes red.
// MUTATION KEY-$4D stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($51); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $4D -- baseline PASS goes red.
// MUTATION KEY-$4E stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($4F); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $4E -- baseline PASS goes red.
// MUTATION KEY-$4F stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($50); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $4F -- baseline PASS goes red.
// MUTATION KEY-$50 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($3A); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $50 -- baseline PASS goes red.
// MUTATION KEY-$51 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($3B); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $51 -- baseline PASS goes red.
// MUTATION KEY-$52 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($3C); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $52 -- baseline PASS goes red.
// MUTATION KEY-$53 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($3D); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $53 -- baseline PASS goes red.
// MUTATION KEY-$54 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($3E); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $54 -- baseline PASS goes red.
// MUTATION KEY-$55 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($3F); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $55 -- baseline PASS goes red.
// MUTATION KEY-$56 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($40); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $56 -- baseline PASS goes red.
// MUTATION KEY-$57 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($41); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $57 -- baseline PASS goes red.
// MUTATION KEY-$58 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($42); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $58 -- baseline PASS goes red.
// MUTATION KEY-$59 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($43); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $59 -- baseline PASS goes red.
// MUTATION KEY-$5A stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($4A); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $5A -- baseline PASS goes red.
// MUTATION KEY-$5B stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($4B); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $5B -- baseline PASS goes red.
// MUTATION KEY-$5C stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($54); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $5C -- baseline PASS goes red.
// MUTATION KEY-$5D stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($55); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $5D -- baseline PASS goes red.
// MUTATION KEY-$5E stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($57); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $5E -- baseline PASS goes red.
// MUTATION KEY-$5F stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($4D); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $5F -- baseline PASS goes red.
// MUTATION KEY-$60 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($69); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $60 -- baseline PASS goes red.
// MUTATION KEY-$61 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($6D); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $61 -- baseline PASS goes red.
// MUTATION KEY-$62 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($39); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $62 -- baseline PASS goes red.
// MUTATION KEY-$63 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($68); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $63 -- baseline PASS goes red.
// MUTATION KEY-$64 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($6A); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $64 -- baseline PASS goes red.
// MUTATION KEY-$66 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($6B); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $66 -- baseline PASS goes red.
// MUTATION KEY-$67 stimulus red MUT_ZERO: harness drives code=$00 instead of this key's own code ($6F); amiga_keymap.v has no entry for $00 so it falls through to the $7F sentinel (:142), which never equals $67 -- baseline PASS goes red.

    // =====================================================================
    // CUTOUT-30 (p.252 + both p.254 figures) -- candidate S3-C1's finding
    // half. The key cut out beside left Shift is claimed at $30 by six
    // claim records; amiga_keymap.v's EUR-2 entry (:130, input 7'h64)
    // emits $2B instead. RED at baseline -- this is the finding.
    //
    // STIMULUS BINDING, WITH ITS OWN ORACLE (NM-HRM-050, coordinator
    // review corrected an earlier swap here): amiga_keymap.v names its two
    // international-cutout inputs only "EUR-1" (:72, code 7'h32) and
    // "EUR-2" (:130, code 7'h64) -- neither name says which physical key
    // is which, and the manual does not describe HID scancodes at all. The
    // oracle for that binding is the USB HID Usage Tables spec, external
    // to the manual, independently readable at
    // _oracle/Denise/driver/input/iokit/keyNames.cpp: usage $64, array
    // index 0x64 (line 199), is commented "Keyboard Non-US Backslash and
    // Pipe" -- the ISO key beside LEFT SHIFT. Usage $32, array index 0x32
    // (line 149), is commented "Non-US Hash and Tilde" -- the ISO key
    // beside RETURN. An earlier version of this file drove $32 here
    // (expecting the left-Shift key's $30) -- actually the Return key's
    // own HID code -- which made this assertion red for the wrong key,
    // while CUTOUT-2B below drove $64 (the real left-Shift key) and read
    // green while sitting on the actual defect. Corrected before commit;
    // both stimuli below are now bound the other way round.
    // =====================================================================
    {
        uint8_t got = h.sample(0x64); // HID usage $64, "Keyboard Non-US Backslash and Pipe" -- beside left Shift
        checks.expect(got == 0x30,
            "CUTOUT-30: the international key cut out beside left Shift "
            "[HID usage $64, Keyboard Non-US Backslash and Pipe -- "
            "keyNames.cpp:199] "
            "(p.252: \"These are $30, cut out from the the left shift\"; "
            "p.254 Figure 8-9 'international cutout beside left Shift'; "
            "Figure 8-10 same) carries code $30 "
            "[claims: opus:8-fig.c59, sol:8-layout.c58, opus:8-fig.c148, "
            "sol:8-layout.c152, opus:8-keycode.c14, sol:8-keycode.c13]");
        checks.note("CUTOUT-30 measured actual value: amiga_keymap.v's "
            "EUR-2 entry (code $64, HID Non-US Backslash and Pipe, beside "
            "left Shift) returns $2B, not $30 -- the same value the EUR-1 "
            "entry (code $32, the Return-side key, see CUTOUT-2B) also "
            "returns, so the RTL's two distinct international-cutout "
            "inputs collapse onto one output where the manual names two.");
    }
// MUTATION CUTOUT-30 none impossible: red at baseline, and per the S3-C1 measurement (see file header and SPECIFICATION.md) no entry anywhere in amiga_keymap.v's 98-entry table emits $30 -- there is no legitimate stimulus this harness could substitute to make this assertion read green, so none is applied.

    // =====================================================================
    // FUNCTION-RANGE (p.252: "50-59 Function keys F1-F10") -- the block-
    // level claim that the ten function keys are exactly the contiguous,
    // increasing range $50-$59, distinct from (and not implied by) the
    // ten individual KEY-$5x rows above each being individually correct.
    // =====================================================================
    {
        static const uint8_t f_stim[10] = {0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,0x42,0x43};
        uint8_t f[10];
        for (int i = 0; i < 10; ++i) {
            uint8_t s = f_stim[i];
#if MUT_ZERO
            s = 0x00;
#endif
            f[i] = h.sample(s);
        }
        bool contiguous = (f[0] == 0x50);
        for (int i = 1; i < 10 && contiguous; ++i) contiguous = (f[i] == f[i-1] + 1);
        checks.expect(contiguous,
            "FUNCTION-RANGE: the ten function-key codes are exactly the "
            "contiguous range $50 (F1) to $59 (F10), in order "
            "(p.252: \"50-59   Function keys F1-F10\") "
            "[claims: opus:8-keycode.c30]");
    }
// MUTATION FUNCTION-RANGE stimulus red MUT_ZERO: same shared mutation as the KEY-$5x rows -- all ten F-key stimuli are redirected to $00, so f[0] reads $7F (not $50) and the contiguity check goes red.

    // =====================================================================
    // GEN-NOT-ASCII (p.251) -- "The keyboard data is not received in the
    // form of ASCII characters." Sampled over the 26 letter keys (already
    // exhaustively covered, individually, by the KEY-$1x/$2x/$3x rows
    // above): none of their Amiga codes equals that letter's ASCII value.
    // =====================================================================
    {
        static const uint8_t letter_stim[26] = {
            0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,
            0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d};
        bool any_ascii_collision = false;
        for (int i = 0; i < 26; ++i) {
            uint8_t s = letter_stim[i];
#if MUT_ASCII_COLLIDE
            if (i == 0) s = 0x2a; // substitute backspace's own code for the 'a' slot
#endif
            uint8_t got = h.sample(s);
            char upper = 'A' + i, lower = 'a' + i;
            if (got == (uint8_t)upper || got == (uint8_t)lower) any_ascii_collision = true;
        }
        checks.expect(!any_ascii_collision,
            "GEN-NOT-ASCII: none of the 26 letter keys' Amiga codes equals "
            "that letter's upper- or lower-case ASCII value "
            "(p.251: \"The keyboard data is not received in the form of "
            "ASCII characters.\") "
            "[claims: opus:8-keycode.c1, sol:8-keycode.c1]");
    }
// MUTATION GEN-NOT-ASCII stimulus red MUT_ASCII_COLLIDE: the 'a'-key slot is stimulated with backspace's own code ($2a, amiga_keymap.v:63) instead of 'a''s own code ($04) -- backspace returns $41, which equals ASCII 'A', so the baseline PASS (no collision) goes red.

    // =====================================================================
    // Declined claims (record() only -- no expect(), no MUTATION line, per
    // the build brief: a MUTATION line's label is the leading token of a
    // label passed to expect(), and none of these four is).
    // =====================================================================

    // RIGHTALT-CONFLICT -- candidate S3-C2. p.252 says $65; both p.254
    // figures say $64 (see the four figure citations); this is a
    // disagreement between two printed pages, not an RTL divergence, and
    // is graded neither way, per the build brief and the finding-
    // candidates file.
    {
        uint8_t got = h.sample(0x6e); // amiga_keymap.v:139, the RTL's own right-alt input
        checks.note("RIGHTALT-CONFLICT measured actual value: "
            "amiga_keymap.v's own right-alt input ($6E) returns $" + hex2(got) +
            " -- matching p.252's $65 and contradicting both p.254 figures' $64.");
        checks.record(
            "RIGHTALT-CONFLICT (p.252 vs p.254): p.252's qualifier table "
            "reads \"65  Right Alt\"; both p.254 figures print $64 on the "
            "right Alt key in both keyboards. Source conflict between two "
            "printed pages -- graded neither way. "
            "[claims: opus:8-fig.c75, sol:8-layout.c77, opus:8-fig.c164, "
            "sol:8-layout.c168, opus:8-keycode.c42, sol:8-keycode.c67]");
    }

    // PROP-HEADING -- a section heading, not a numeric proposition.
    checks.record(
        "PROP-HEADING (p.252): \"Key codes for qualifier keys\" is a "
        "section heading; it names no key and no code and states nothing "
        "observable at code/amiga. [claims: opus:8-keycode.c17]");

    // PROP-RANGE-MAIN -- a block-level statement, not a per-key claim;
    // fully corroborated by (and not testable beyond) the main-body
    // KEY-$xx rows above.
    checks.record(
        "PROP-RANGE-MAIN (p.252): \"These are key codes assigned to "
        "specific positions on the main body of the keyboard\" is a "
        "block-level statement about the $00-$3F range as a whole, not a "
        "per-key claim -- already corroborated, key by key, by the "
        "$00-$3F rows in the KEY-$xx table above; not independently "
        "gradable beyond that. [claims: opus:8-keycode.c9, sol:8-keycode.c9]");

    // GEN-IS-KEYCODES -- the same measurement's positive framing; no
    // numeric content beyond what GEN-NOT-ASCII already checks.
    checks.record(
        "GEN-IS-KEYCODES (p.251): \"Instead, for maximum versatility, it "
        "is received in the form of keycodes\" restates GEN-NOT-ASCII's "
        "finding positively; it names no additional numeric fact to "
        "check. [claims: opus:8-keycode.c2, sol:8-keycode.c2]");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}