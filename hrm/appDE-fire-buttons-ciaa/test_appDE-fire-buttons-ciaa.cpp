// Test purpose: HRM conformance package `appDE-fire-buttons-ciaa`, appendix
//   E printed pages 329-330 (GAME PORT INTERFACE TO FIRE BUTTONS / CONNECTOR
//   PIN USAGE FOR FIRE BUTTON INPUTS / READING FIRE BUTTONS), against the
//   `ciaa` module compiled alone. 16 claims, filtered by
//   classify.package=="appDE-fire-buttons-ciaa" from
//   work/2026-09-17_appDE_packages.jsonl. Built per
//   work/2026-09-17_appDE_build_brief.md.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed pages 329-330
//   ONLY (docs/references/amiga-hrm-3rd/appE/p329.txt, p330.txt, both read
//   and quote-checked against their renders before this file was written).
//   appE/corpus-defects.md names TWO defects on p.330, both accounted for
//   below: (a) the DDRA figure's own printed self-contradiction at bits 5:2
//   (value 0, label OUT) -- DDRA-FIGURE-CONTRADICTION, further down; (b)
//   "the text layer loses the FIRE0/FIRE1-to-PRA-bit paths and the diagram
//   structure" -- true, and the reason the connector-to-PRA wiring diagram
//   below (PORT 1/PORT 2 -> FIRE0\/FIRE1\ -> PRA bit 7/bit 6) was read from
//   figures/p330.png directly, not from p330.txt, per the corpus-wide rule
//   "read every table from the render." p327 ("J11 is the right controller
//   port connector... J12 is the left controller port connector...") is read
//   for context ONLY (candidate 1, below); no claim in this package quotes
//   it.
// Fixture boundary: `ciaa` (ciaa.v) plus the four sibling files Verilator
//   needs to elaborate it -- cia_int.v, cia_timera.v, cia_timerb.v,
//   cia_timerd.v -- confirmed by this file's own Makefile SRC list (the only
//   place a module could be added) and by direct elaboration: `ciaa.v` alone
//   fails with four missing-module errors, matching every claim's own
//   classify.evidence field. `ciaa` sees CIAAPRA/CIAADDRA and the `porta_in`
//   bus; it has NO notion of a DB9 connector, a pin number, or which
//   physical controller port ("left"/"right", J11/J12) a bit traces back to
//   -- that information exists only at `userio` (the connector-to-_sjoy
//   crossbar) and is joined to `ciaa` at `minimig` (minimig.v:622). Claims
//   that need it are declined below by module boundary, not regraded with a
//   weaker guess, per the build brief.
// Non-goals: anything outside the `ciaa` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/2026-09-17_appDE_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// RTL FACTS USED THROUGHOUT (ciaa.v, read directly -- not taken from any
//   report -- against the pinned submodule d6f7df957fad655bc07f6a381e5094a9edff0be0):
//   - Port declaration: `input [7:2] porta_in` (ciaa.v:95), `output [3:0]
//     porta_out` (ciaa.v:96). porta_in has NO bits 1:0 or an 8-bit range --
//     it is a 6-bit range covering CIAAPRA bits 7 down to 2 only.
//   - Address decode: `pra = enable && rs==4'h0` (ciaa.v:136), `ddra =
//     enable && rs==4'h2` (ciaa.v:138), where `enable = aen & (rd|wr)`
//     (ciaa.v:135).
//   - Input synchroniser: `always @(posedge clk) if (clk7_en) porta_in2[7:2]
//     <= porta_in[7:2];` (ciaa.v:227-230, assignment at :229) -- one clock
//     edge of latency between a change on `porta_in` and its appearance in
//     a PRA read.
//   - PRA/DDRA read mux, purely combinational: `if (!wr && pra) pa_out[7:0]
//     = {porta_in2[7:2],porta_out[1:0]}; else if (!wr && ddra) pa_out[7:0] =
//     ddrporta[7:0];` (ciaa.v:251-259, the pra branch at :254) -- CIAAPRA
//     bits 7:2 are `porta_in2[7:2]` UNCONDITIONALLY on every read, with NO
//     dependency on `ddrporta` anywhere in this mux.
//   - DDRA write: `ddrporta[7:0] <= data_in[7:0]` on a DDRA write, clk7_en
//     gated (ciaa.v:242-247, assignment at :247) -- the full 8-bit register,
//     read back unconditionally at ciaa.v:256.
//   - Output/pull-up model (bits 7:6, the fire-button positions):
//     `porta_out[3:0] = {(~ddrporta[7:6] | regporta[3:2]), (~ddrporta[1:0] |
//     regporta[1:0])}` (ciaa.v:262) -- this drives `porta_out`, a SEPARATE
//     output port from `porta_in`/`porta_in2`; it plays no role in what a
//     PRA read returns for bits 7:2, since the read mux (above) never reads
//     `porta_out` for those bits.
//   - grep -rniE 'switch|joy|button' over ciaa.v, cia_int.v, cia_timera.v,
//     cia_timerb.v, cia_timerd.v (this fixture's five source files, executable
//     bodies): zero hits. ciaa.v has no switch, no connector, no pin, and no
//     polarity logic of its own -- confirmed directly, not asserted from a
//     report.
//
// Bus-timing protocol (Harness class below): reused verbatim from
//   hrm/appF-cia-register-map's own Harness, which itself is "reused verbatim
//   from hrm/ch1-cia-timers' and hrm/ch8-ciaa-port-direction's own Harness"
//   -- all three already build this exact fixture (`ciaa` + the same four
//   sibling files) with `clk7_en`/`clk7n_en` tied permanently high, so every
//   `step()` (one clk posedge) is a synchronising edge. No ciaa-specific fact
//   was taken from any of those three files beyond this protocol; every
//   fact cited above was read from ciaa.v directly for this package.
//
// ============================================================================
// DISPOSITION OF ALL 16 CLAIMS (verified against work/2026-09-17_appDE_packages.jsonl
// filtered on classify.package=="appDE-fire-buttons-ciaa" -- 16 entries, matching
// the build brief's package table)
// ============================================================================
//
//  1. sol:E-fire.c1         p329 "fire buttons...routed to the 8520 [PRA0]" -- DECLINED (duplicate)
//  2. opus:E-firebutton.c2  p329 same quote, twin of #1                     -- DECLINED (duplicate)
//  3. sol:E-fire.c2         p329 "PRAO bit 7 = Fire* left controller port"  -- DECLINED (compound: duplicate + boundary) -- CANDIDATE 1
//  4. opus:E-firebutton.c3  p329 same quote, twin of #3                    -- DECLINED (compound) -- CANDIDATE 1
//  5. sol:E-fire.c3         p329 "PRAO bit 6 = Fire* right controller port" -- DECLINED (compound) -- CANDIDATE 1
//  6. opus:E-firebutton.c4  p329 same quote, twin of #5                    -- DECLINED (compound) -- CANDIDATE 1
//  7. sol:E-fire.c4         p329 "DDRA0<7:6> cleared as appropriate"        -- GRADED, PROP-DDRA76-INPUT-FOR-FIRE-READ
//  8. opus:E-firebutton.c5  p329 "Before reading this register..."         -- GRADED, same PROP line, different quoted span of the same sentence
//  9. opus:E-firebutton.c6  p329 "DDRA0<7:6> cleared as appropriate", twin of #7 -- GRADED, same PROP line
// 10. sol:E-fire.c6         p329 "Fire buttons are always valid..."         -- GRADED, PROP-FIRE-READ-ANY-TIME
// 11. opus:E-firebutton.c8  p329 same quote, twin of #10                    -- GRADED, same PROP line
// 12. sol:E-fire.c12        p330 figure, connector pin 6 = FIRE*            -- DECLINED (boundary)
// 13. opus:E-firebutton.c15 p330 text "6  FIRE*   Left mouse button/fire button", twin of #12 -- DECLINED (boundary)
// 14. sol:E-fire.c20        p330 figure, DDRA bits 7/6 = 0/IN (non-contradictory half) -- GRADED, folded into PROP-DDRA76-INPUT-FOR-FIRE-READ
// 15. opus:E-firebutton.c24 p330 figure, DDRA VALUES row (the bits-5:2 contradiction) -- RECORDED, measure both / grade neither
// 16. opus:E-firebutton.c25 p330 figure, DDRA LABELS row, other half of #15 -- RECORDED, measure both / grade neither
//
// Count: 6 graded (2 PROP lines, 10 uids between them), 8 declined (4 pairs),
// 2 recorded-as-conflict (1 pair). 16 of 16 accounted for.
//
// ============================================================================
// CANDIDATE 1 -- "the fire buttons look like they are on the wrong bits"
// (work/2026-09-17_appDE_build_brief.md, work/2026-09-17_appDE_finding_candidates.md
// section 8). Predicted RED, named on four uids that are ALL in THIS package:
// opus:E-firebutton.c3, opus:E-firebutton.c4, sol:E-fire.c2, sol:E-fire.c3.
// ============================================================================
//
// WHAT WAS MEASURED FOR THIS CANDIDATE (read from source, NOT simulated by
//   this ciaa-alone fixture -- the files below are outside this package's
//   Makefile SRC list and are never compiled here):
//
//   p.329 (this package's own oracle, quote-checked above): "PRAO bit 7 =
//     Fire* left controller port" / "PRAO bit 6 = Fire* right controller
//     port".
//   p.327 (read for context; docs/references/amiga-hrm-3rd/appE/p327.txt,
//     verified against its render): "J11 is the right controller port
//     connector (JOY1DAT, P0T1DAT). J12 is the left controller port
//     connector (JOYODAT, POTODAT)." -- so LEFT is anchored to JOY0DAT.
//   Together the manual pairs JOY0DAT's own connector with CIAAPRA bit 7.
//
//   nanomig-hrm-tests/nanomig/src/minimig-aga/minimig.v:622 --
//     `.porta_in({_fire1,_fire0,_ready,_track0,_wprot,_change})` against
//     ciaa.v:95's `input [7:2] porta_in` -- PA7=_fire1, PA6=_fire0.
//   nanomig-hrm-tests/nanomig/src/minimig-aga/userio.v:92 -- `wire joy_swap
//     = 1'b0;` (hard zero).
//   userio.v:215-218 (clk7_en block) -- `_sjoy1 <= joy_swap ? joy1 : joy2;`
//     so with joy_swap==0, `_sjoy1 = joy2`.
//   userio.v:313-314 -- `assign _fire0 = ... _sjoy1[4] & _mleft;` /
//     `assign _fire1 = ... _sjoy2[4];`.
//   userio.v:54, :233-239 -- `parameter JOY0DAT = 9'h00a;`, and
//     `dmouse0dat` (the JOY0DAT read data) is counted from `_djoy1`, which
//     is `_sjoy1` registered one cycle -- i.e. JOY0DAT is the `_joy2`-fed
//     port's register.
//
//   Chained: `_sjoy1` (== `joy2`, the port whose motion is read back at
//   JOY0DAT) feeds `_fire0` (userio.v:313), and `_fire0` lands on PA6
//   (minimig.v:622). So this RTL puts JOY0DAT's own connector's fire button
//   on CIAAPRA bit 6 -- the manual (p.329+p.327) puts it on bit 7. **The two
//   disagree.**
//
//   ADDITIONAL EVIDENCE FOUND WHILE CHECKING THIS PACKAGE'S OWN PAGES,
//   read from figures/p330.png (the wiring diagram under "CONNECTOR PIN
//   USAGE FOR FIRE BUTTON INPUTS" -- corpus-defects.md is explicit that its
//   own text layer LOSES this diagram, so it was read from the render only,
//   never from p330.txt): the diagram itself labels PRA's bit-7 box
//   "FIRE1\" and its bit-6 box "FIRE0\", and traces PORT 1 pin 6 to the
//   FIRE0\ box (bit 6) and PORT 2 pin 6 to the FIRE1\ box (bit 7) -- i.e.
//   this figure's OWN bit assignment (bit7=FIRE1, bit6=FIRE0) matches
//   minimig.v:622's PA7=_fire1/PA6=_fire0 exactly. What this figure does
//   NOT state is which of "PORT 1"/"PORT 2" is "left"/"right" -- a THIRD
//   port-numbering scheme on top of p.327's J11/J12 and p.329's own
//   left/right wording, none of which this page ties together explicitly.
//   Not adjudicated, not used in any graded assertion below (no claim in
//   this package quotes this diagram for its port-identity content); noted
//   here because it directly bears on what "measured" means for candidate 1
//   and was found only by reading the render, not the extracted text.
//
// WHY THIS PACKAGE DOES NOT GRADE THAT DISAGREEMENT, AND WHO DOES: every RTL
//   fact quoted above lives in userio.v or minimig.v. None of it is reachable
//   from `ciaa` compiled alone -- `ciaa` has no `_sjoy1`, no `joy_swap`, no
//   `_joy1`/`_joy2`, and (per the grep evidence above) no notion of "left"
//   or "right" at all; it has only bit positions of an internal bus,
//   `porta_in[7:2]`. Per the build brief: "the bit-level propositions (a
//   driven porta_in[7] appears at PRA bit 7, bits 6 and 7 are independent)
//   are yours; the which-port-is-left half is bound at minimig and belongs
//   to appDE-portsig-cia-bindings." The bit-level half is ALREADY graded, in
//   an accepted chapter, by hrm/ch8-ciaapra-fire-buttons (PROP-BIT6-PASSTHROUGH,
//   PROP-BIT7-PASSTHROUGH, PROP-BIT6-INDEPENDENT-OF-BIT7,
//   PROP-BIT7-INDEPENDENT-OF-BIT6 -- read directly from that package's own
//   test_ch8-ciaapra-fire-buttons.cpp before this file was written). This
//   package therefore has nothing left to grade for sol:E-fire.c2/.c3 and
//   opus:E-firebutton.c3/.c4: their bit-level content duplicates an accepted
//   package, and their port-identity content is out of `ciaa`'s reach. Both
//   are declined below (PROP-FIRE-LEFT-RIGHT-DECLINED), each with its own
//   two-part reason stated separately -- not swept under one "boundary" label,
//   per the build brief's own warning against letting a boundary decline
//   swallow a proposition this fixture COULD answer (it cannot answer either
//   half of THIS specific claim, but the two halves fail for different
//   reasons and that distinction is kept). The actual predicted-red
//   measurement -- whether NanoMig's `_sjoy1`-to-JOY0DAT-to-`_fire0`-to-PA6
//   chain really does conflict with the manual's bit7=left pairing -- can
//   only be constructed at a fixture that has BOTH the connector identity
//   and the CIA bit together, i.e. `minimig` (which instantiates `userio`
//   and `ciaa` both) -- `appDE-portsig-cia-bindings`, not this package.
//
// A DISCREPANCY FOUND WHILE CHECKING THE BRIEF: the classify.evidence field
//   for sol:E-fire.c12/opus:E-firebutton.c15 (the connector-pin-6 claim,
//   below) says "Build should pick it up at `ciaa` (or at `minimig`...)".
//   That is not correct for `ciaa`: `ciaa` alone has no connector-pin
//   concept at all (see the grep evidence above and PIN6-CONNECTOR-DECLINED
//   below) -- only `minimig` (or a `userio`+`ciaa` rig) can construct it.
//   Declined here, not built, for that reason.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup (one build serves the real run, MUTATE unset, and every
//   mutation run, MUTATE=<id>). Each mutation changes what the module is
//   ASKED TO DO (a driven pin level or a write sequence) and leaves the
//   `expect()` call's own comparison untouched. Baseline (MUTATE unset): 3
//   passed, 0 failed (A0-LIVENESS, PROP-DDRA76-INPUT-FOR-FIRE-READ,
//   PROP-FIRE-READ-ANY-TIME). Every mutation below was run with its own id,
//   confirmed red (each mutation reddens exactly and only its own
//   assertion), then reverted (MUTATE unset) and confirmed green again.
//
// MUTATION A0-LIVENESS stimulus red: the second porta_in word is driven identical to the first instead of its complement, so the two readings are indistinguishable (MUTATE=A0-LIVENESS). RUN: baseline 3 passed 0 failed; mutated 2 passed 1 failed, only this label red; reverted 3/0.
// MUTATION PROP-DDRA76-INPUT-FOR-FIRE-READ stimulus red: with DDRA7:6 cleared to input as the page instructs, the very first (bit7,bit6) combination in the sweep is driven with bit7 inverted relative to what the check still compares it against, so that one combination mismatches while DDRA remains exactly as the page says to set it (MUTATE=PROP-DDRA76-INPUT-FOR-FIRE-READ). RUN: baseline 3 passed 0 failed; mutated 2 passed 1 failed, only this label red; reverted 3/0.
// MUTATION PROP-FIRE-READ-ANY-TIME stimulus red: the first read of a freshly-driven word is taken with the ordinary one-cycle synchroniser edge skipped, so it observes the pre-change value instead of the settled one, while later reads (after the skipped edge is allowed to happen) see the new value -- the three reads disagree, which is exactly what "always valid, read at any time" rules out once the one unavoidable edge has passed (MUTATE=PROP-FIRE-READ-ANY-TIME). RUN: baseline 3 passed 0 failed; mutated 2 passed 1 failed, only this label red; reverted 3/0.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include "Vciaa.h"
#include "verilated.h"

namespace {
const char* g_mutate = nullptr;
inline bool mutating(const char* id) {
    return g_mutate != nullptr && std::strcmp(g_mutate, id) == 0;
}
std::string hex2(unsigned long v) {
    char b[8];
    std::snprintf(b, sizeof b, "0x%02lX", v);
    return std::string(b);
}

constexpr uint8_t RS_PRA  = 0x0;
constexpr uint8_t RS_DDRA = 0x2;

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

// Bus-timing protocol for ciaa.v, reused verbatim from
// hrm/appF-cia-register-map's Harness (itself reused from
// hrm/ch1-cia-timers and hrm/ch8-ciaa-port-direction, all three building
// this same fixture): regporta/ddrporta are posedge-clk, clk7_en-gated, so
// a write needs one step() to latch; pa_out (the PRA/DDRA read mux) is
// combinational and needs only eval(). clk7_en/clk7n_en are held
// permanently high, so every step() is a synchronising edge.
class Harness {
public:
    Harness() : dut(new Vciaa) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->clk7n_en = 1;
        dut->reset = 1;
        dut->aen = 0;
        dut->rd = 0;
        dut->wr = 0;
        dut->rs = 0;
        dut->data_in = 0;
        dut->tick = 0;
        dut->eclk = 0;
        dut->porta_in = 0;
        dut->portb_in = 0;
        dut->kms_level = 0;
        dut->kbd_mouse_type = 0;
        dut->kbd_mouse_data = 0;
        dut->hrtmon_en = 0;
        for (int i = 0; i < 8; ++i) step();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) step();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void write_reg(uint8_t rs, uint8_t data) {
        dut->aen = 1;
        dut->wr = 1;
        dut->rd = 0;
        dut->rs = rs;
        dut->data_in = data;
        step();
        dut->aen = 0;
        dut->wr = 0;
        dut->rs = 0;
        dut->data_in = 0;
        step();
    }

    uint8_t read_reg(uint8_t rs) {
        dut->aen = 1;
        dut->rd = 1;
        dut->wr = 0;
        dut->rs = rs;
        dut->eval();
        const uint8_t v = dut->data_out;
        dut->aen = 0;
        dut->rd = 0;
        dut->eval();
        return v;
    }

    void step() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    Vciaa* dut;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;
    Harness h;

    // ========================================================================
    // A0-LIVENESS -- FIXTURE DATA-PATH LIVENESS. No claim id, no manual page:
    // the build brief's own requirement that the fixture prove it can
    // deliver a known, distinct word before any read-based assertion is
    // trusted. A known 6-bit pattern and its bitwise complement are driven
    // onto porta_in[7:2] and read back at CIAAPRA (DDRA left at its reset
    // default, 0x00 -- input -- per ciaa.v:245); the expected value is
    // derived algebraically from the very word driven, not from a second
    // hard-coded assumption about bit layout.
    // porta_in is declared `input [7:2] porta_in` in ciaa.v -- a 6-bit
    // range. Verilator normalises a partial range so bit 0 of the C++ value
    // is the range's own LSB (Verilog bit 2) through bit 5 = Verilog bit 7.
    // ========================================================================
    {
        const uint8_t word1 = 0b101010;
        const uint8_t word2 = mutating("A0-LIVENESS") ? word1 : (uint8_t)0b010101;  // complement, MUTATION: identical instead

        h.dut->porta_in = word1;
        h.step();
        const uint8_t v1 = h.read_reg(RS_PRA);
        checks.note("A0-LIVENESS-WORD1: porta_in[7:2]=0b101010 reads back CIAAPRA bits7:2=" + hex2(v1 & 0xFC));

        h.dut->porta_in = word2;
        h.step();
        const uint8_t v2 = h.read_reg(RS_PRA);
        checks.note("A0-LIVENESS-WORD2: porta_in[7:2]=0b" + std::string(mutating("A0-LIVENESS") ? "101010 (mutated, should be 010101)" : "010101") +
                    " reads back CIAAPRA bits7:2=" + hex2(v2 & 0xFC));

        const bool w1ok = (v1 & 0xFC) == (uint8_t)((word1 & 0x3F) << 2);
        const bool w2ok = (v2 & 0xFC) == (uint8_t)((word2 & 0x3F) << 2);
        const bool distinct = (v1 & 0xFC) != (v2 & 0xFC);
        checks.expect(w1ok && w2ok && distinct,
            "A0-LIVENESS: a known word driven on porta_in[7:2] and its bitwise complement both arrive, unmodified "
            "and mutually distinguishable, at CIAAPRA bits 7:2 (ciaa.v:229 synchroniser, ciaa.v:254 pass-through "
            "mux) -- the fixture's data path is live, not stuck");
    }

    // ========================================================================
    // GROUP1-EXISTENCE-DECLINED -- sol:E-fire.c1, opus:E-firebutton.c2 (p.329,
    // quote "The fire buttons are normally opened switches routed to the
    // 8520 [adapter PRAO]"). Declined: this claim's own two facts are each
    // already settled by an accepted package building this identical
    // fixture. (i) porta_in bits 7/6 arrive unmodified at CIAAPRA bits 7/6
    // -- graded by hrm/ch8-ciaapra-fire-buttons's PROP-BIT6-PASSTHROUGH and
    // PROP-BIT7-PASSTHROUGH (read directly from that package's
    // test_ch8-ciaapra-fire-buttons.cpp before this file was written). (ii)
    // that a mechanically-open/closed SWITCH actually drives porta_in to
    // the corresponding level is a fact about the controller-port-to-chip
    // wiring, outside ciaa.v (grep evidence in the file header above) --
    // already declined by that same accepted package under its
    // P4-DECLINED line, for the identical boundary reason. Nothing in this
    // claim is narrower or on a different axis from what that package
    // already settled on both sides, so it is declined here rather than
    // regraded.
    // ========================================================================
    checks.record("sol:E-fire.c1 (p.329, quote 'The fire buttons are normally opened switches routed to the "
        "8520 [adapter PRAO]'): same proposition already settled, on both its constructible half and its "
        "out-of-boundary half, by hrm/ch8-ciaapra-fire-buttons -- PROP-BIT6-PASSTHROUGH/PROP-BIT7-PASSTHROUGH "
        "grade the pin-to-register half (porta_in bits 6/7 arrive unmodified at CIAAPRA), and that package's own "
        "P4-DECLINED line already declines the switch-to-pin half for the same reason this fixture would: ciaa.v "
        "has no switch, connector or polarity logic (grep over all five of this fixture's source files for "
        "'switch'/'joy'/'button': zero hits in any executable body). Declined, not re-measured.");
    checks.record("opus:E-firebutton.c2 (p.329, same quote, twin of sol:E-fire.c1): identical reasoning.");

    // ========================================================================
    // PROP-FIRE-LEFT-RIGHT-DECLINED -- CANDIDATE 1, the predicted-RED
    // finding. sol:E-fire.c2/opus:E-firebutton.c3 (p.329, "PRAO bit 7 =
    // Fire* left controller port") and sol:E-fire.c3/opus:E-firebutton.c4
    // (p.329, "PRAO bit 6 = Fire* right controller port"). See the file
    // header's CANDIDATE 1 section for the full measured RTL chain (read
    // from userio.v/minimig.v, NOT simulated by this ciaa-alone fixture).
    // Each of the four uids is declined below with its OWN two-part reason
    // stated separately, per the build brief's warning against a boundary
    // decline swallowing a proposition this fixture could answer.
    // ========================================================================
    checks.note("CANDIDATE-1-MEASURED (read from userio.v/minimig.v, not compiled or simulated by this "
        "ciaa-alone fixture -- see file header for the full citation chain): minimig.v:622 wires "
        "porta_in={_fire1,_fire0,...} so PA7=_fire1, PA6=_fire0; userio.v:92 ties joy_swap=1'b0 so userio.v:215-218 "
        "makes _sjoy1=joy2; userio.v:313 derives _fire0 from _sjoy1[4]; userio.v:54,233-239 counts JOY0DAT "
        "(dmouse0dat) from that same _sjoy1/_djoy1. So the connector whose motion is read back at JOY0DAT drives "
        "_fire0, which lands on PA6 -- while p.329 ('PRAO bit 7 = Fire* left controller port') plus p.327 ('J12 is "
        "the left controller port connector (JOYODAT, POTODAT)') pair that same connector's fire line with PA7. "
        "The two disagree. This package cannot grade the disagreement -- see the two decline reasons below.");
    checks.note("CANDIDATE-1-MEASURED-RENDER (figures/p330.png, read directly -- corpus-defects.md confirms "
        "p330.txt's text layer LOSES this diagram): the connector-to-PRA wiring figure labels PRA bit 7 \"FIRE1\\\" "
        "and bit 6 \"FIRE0\\\", tracing PORT 1 pin 6 to the FIRE0\\ (bit 6) box and PORT 2 pin 6 to the FIRE1\\ "
        "(bit 7) box -- matching minimig.v:622's PA7=_fire1/PA6=_fire0 exactly. This figure never states which of "
        "\"PORT 1\"/\"PORT 2\" is \"left\"/\"right\" (a third port-numbering scheme, alongside p.327's J11/J12 and "
        "p.329's own wording); not adjudicated here, not used by any graded assertion in this file.");

    checks.record("sol:E-fire.c2 (p.329, 'PRAO bit 7 = Fire* left controller port'): two-part decline. (i) The "
        "bit-level half -- CIAAPRA bit 7 tracks its own porta_in pin, independent of bit 6 -- is the SAME "
        "proposition hrm/ch8-ciaapra-fire-buttons already grades as PROP-BIT7-PASSTHROUGH and "
        "PROP-BIT7-INDEPENDENT-OF-BIT6; not regraded here. (ii) The port-identity half -- that THIS bit is the "
        "one wired to the LEFT (JOY0DAT) connector -- needs userio.v's _sjoy1/joy_swap crossbar and minimig.v's "
        "connector wiring, neither reachable from ciaa alone (see the grep evidence in the file header: ciaa.v has "
        "no notion of 'left', 'right', or a connector at all). Per the build brief, appendix E states this on a "
        "different AXIS from ch8-ciaapra-fire-buttons's declined numeral (left/right, not 0/1/2) -- which would "
        "normally be grounds to grade it fresh -- but that axis difference does not change what THIS fixture can "
        "reach: (ii) is out of ciaa's boundary regardless of which label the page uses. The fixture that has both "
        "halves together is `minimig` (appDE-portsig-cia-bindings), not this package. Declined, not re-measured "
        "with a weaker guess.");
    checks.record("opus:E-firebutton.c3 (p.329, same quote, twin of sol:E-fire.c2): identical two-part reasoning.");
    checks.record("sol:E-fire.c3 (p.329, 'PRAO bit 6 = Fire* right controller port'): symmetric two-part decline. "
        "(i) CIAAPRA bit 6 tracks its own porta_in pin, independent of bit 7 -- already graded as "
        "PROP-BIT6-PASSTHROUGH/PROP-BIT6-INDEPENDENT-OF-BIT7 by hrm/ch8-ciaapra-fire-buttons. (ii) That this bit "
        "is wired to the RIGHT connector needs the identical out-of-reach userio.v/minimig.v facts as "
        "sol:E-fire.c2 above. Declined for the same two reasons.");
    checks.record("opus:E-firebutton.c4 (p.329, same quote, twin of sol:E-fire.c3): identical two-part reasoning.");

    // ========================================================================
    // PROP-DDRA76-INPUT-FOR-FIRE-READ -- sol:E-fire.c4 ("DDRA0<7:6> cleared
    // as appropriate"), opus:E-firebutton.c5 ("Before reading this
    // register, the corresponding bits of the data [direction register
    // must be cleared]"), opus:E-firebutton.c6 (same quote as sol:E-fire.c4),
    // sol:E-fire.c20 (p.330 figure: DDRA bits 7/6 shown as value 0 / label
    // IN in the worked "READING FIRE BUTTONS" example -- the
    // non-contradictory half of that figure; bits 5:2 disagree with
    // themselves and are handled separately below, never used here).
    // All four uids restate one proposition: DDRA7:6 cleared to input is
    // the documented precondition for reading the fire buttons correctly.
    // NOT a duplicate of any accepted package: hrm/ch8-ciaa-port-direction's
    // PROP-1/PROP-2 grade the DDRA mechanism generically (bit-position
    // correspondence, and that a 1 makes a bit an output) and explicitly
    // carve the fire-button bits OUT OF SCOPE, declining them to
    // hrm/ch8-ciaapra-fire-buttons by name (its own SPECIFICATION.md,
    // "Scope carve-out" -- read directly before this file was written);
    // hrm/ch8-ciaapra-fire-buttons never varies DDRA at all (every one of
    // its checks runs at DDRA's reset default, never writing it). This
    // package is the first to grade DDRA7:6 specifically, as an explicit
    // precondition, for a PRA fire-button read.
    // ========================================================================
    {
        h.write_reg(RS_DDRA, 0x00);  // bits 7:6 cleared to input, per the page's own instruction
        const uint8_t ddra_after = h.read_reg(RS_DDRA);

        bool ok = true;
        std::string sweep;
        const bool want7[4] = {false, false, true, true};
        const bool want6[4] = {false, true, false, true};
        for (int i = 0; i < 4; i++) {
            const bool drive7 = (mutating("PROP-DDRA76-INPUT-FOR-FIRE-READ") && i == 0) ? !want7[i] : want7[i];
            const bool drive6 = want6[i];
            uint8_t w = 0x0F;  // bits 5:2 (unclaimed by any uid here) held high/open
            if (drive7) w |= (1 << 5);  // Verilog bit7 = C bit5 (porta_in[7:2] LSB-aligned at C bit0=V2)
            if (drive6) w |= (1 << 4);  // Verilog bit6 = C bit4
            h.dut->porta_in = w;
            h.step();
            const uint8_t v = h.read_reg(RS_PRA);
            const bool got7 = (v >> 7) & 1;
            const bool got6 = (v >> 6) & 1;
            if (got7 != want7[i] || got6 != want6[i]) ok = false;
            sweep += "(b7=" + std::to_string(want7[i]) + ",b6=" + std::to_string(want6[i]) + ")->PRA[7:6]=0b" +
                     std::to_string(got7) + std::to_string(got6) + " ";
        }
        checks.note("PROP-DDRA76-STIMULUS: DDRA written 0x00, read back " + hex2(ddra_after) + " (bits 7:6 "
            "confirmed cleared/input, matching p.330's figure); sweep " + sweep);
        checks.expect(ok,
            "PROP-DDRA76-INPUT-FOR-FIRE-READ (p.329 'DDRA0<7:6> cleared as appropriate' / 'Before reading this "
            "register, the corresponding bits of the data direction register must be cleared'; p.330 figure, "
            "DDRA bits 7/6 = 0/IN, the non-contradictory half): with DDRA7:6 cleared to input as the page "
            "instructs, CIAAPRA bits 7:6 independently track their own porta_in pins across all four "
            "combinations. sol:E-fire.c4 opus:E-firebutton.c5 opus:E-firebutton.c6 sol:E-fire.c20");

        // Non-graded context, not a new claim: the RTL fact that makes this
        // package's grade a POSITIVE confirmation rather than a coincidence
        // -- ciaa.v:254's read mux never consults ddrporta for a PRA read,
        // so the page's instructed precondition, though followed correctly
        // above, is not load-bearing in this implementation.
        h.write_reg(RS_DDRA, 0xFF);  // bits 7:6 NOT cleared (contrary to the page), all output
        h.dut->porta_in = 0x20;      // bit7=1,bit6=0 on porta_in (C bit5=V7=1, C bit4=V6=0)
        h.step();
        const uint8_t v_uncleared = h.read_reg(RS_PRA);
        checks.note("PROP-DDRA76-CONTEXT (uid-free, not graded): with DDRA7:6=1 (NOT cleared, contrary to the "
            "page's instruction), CIAAPRA bits 7:6 still read 0b" + std::to_string((v_uncleared >> 7) & 1) +
            std::to_string((v_uncleared >> 6) & 1) + " = porta_in unmodified (ciaa.v:254 never consults ddrporta "
            "for a PRA read) -- this implementation's guarantee is stronger than the page requires, not narrower.");
    }

    // ========================================================================
    // PROP-FIRE-READ-ANY-TIME -- sol:E-fire.c6, opus:E-firebutton.c8 (p.329,
    // "Fire buttons are always valid and may be read at any time."). New:
    // grep -rniE 'always valid|read at any time' over every *.md/*.cpp in
    // all of hrm/'s 193 package directories (repeated directly for this
    // package, not taken from the classify note): zero hits outside this
    // file. Tested as: once porta_in has propagated through the one
    // documented synchroniser edge (ciaa.v:229), CIAAPRA continues to read
    // the identical, correct value across any number of FURTHER reads and
    // clock edges, with no re-priming, no drift and no read-clears-on-access
    // side effect (unlike e.g. ICR, which does clear on read elsewhere in
    // this same chip -- cia_int.v, out of this claim's scope). "Any time" is
    // read as bounded by that one unavoidable edge, not as zero propagation
    // delay, which no real chip promises and which the mutation below makes
    // the actual, falsifiable content of this check.
    // ========================================================================
    {
        h.write_reg(RS_DDRA, 0x00);
        const uint8_t wordA = 0x15;  // arbitrary distinct pattern on porta_in[7:2], != reset default (0)
        h.dut->porta_in = wordA;
        if (!mutating("PROP-FIRE-READ-ANY-TIME")) {
            h.step();  // the one ordinary synchroniser edge this claim requires
        }
        const uint8_t r1 = h.read_reg(RS_PRA);
        const uint8_t r2 = h.read_reg(RS_PRA);  // re-read, no further clock edge: must be identical
        if (mutating("PROP-FIRE-READ-ANY-TIME")) {
            h.step();  // let the skipped edge happen late, so r1/r2 (pre-edge) and r3 (post-edge) genuinely differ
        }
        for (int i = 0; i < 6; ++i) h.step();  // many more, unrelated clock edges pass
        const uint8_t r3 = h.read_reg(RS_PRA);  // read again, much later: still identical to r1/r2 in the baseline

        checks.note("PROP-FIRE-READ-ANY-TIME-STIMULUS: r1=" + hex2(r1 & 0xFC) + " r2=" + hex2(r2 & 0xFC) +
            " r3=" + hex2(r3 & 0xFC) + " (expected steady " + hex2((uint8_t)((wordA & 0x3F) << 2)) + ")");
        const bool all_same = (r1 == r2) && (r2 == r3);
        const bool correct = (r1 & 0xFC) == (uint8_t)((wordA & 0x3F) << 2);
        checks.expect(all_same && correct,
            "PROP-FIRE-READ-ANY-TIME (p.329 'Fire buttons are always valid and may be read at any time.'): once "
            "porta_in has propagated through the one-cycle synchroniser (ciaa.v:229), CIAAPRA continues to read "
            "the identical, correct value across repeated reads and further, unrelated clock edges -- no "
            "re-arming or periodic refresh is required. sol:E-fire.c6 opus:E-firebutton.c8");
    }

    // ========================================================================
    // PIN6-CONNECTOR-DECLINED -- sol:E-fire.c12 (p.330 figure, "CONNECTOR
    // PIN USAGE FOR FIRE BUTTON INPUTS", pin 6 = FIRE*), opus:E-firebutton.c15
    // (p.330 text, same row, "6  FIRE*   Left mouse button/fire button").
    // Declined by module boundary: this claim binds a physical DB9 pin
    // NUMBER to the FIRE* signal. `ciaa` has no pins, no connector and no
    // pin-numbering concept anywhere in its source (grep evidence in the
    // file header) -- only `porta_in`, an already-abstracted internal bus.
    // The pin-to-signal binding is made entirely inside `userio.v` (the
    // _joy1/_joy2 vectors and their per-bit pin correspondence) and is
    // therefore out of reach for a `ciaa`-alone fixture, the same shape of
    // boundary argument ch8-ciaapra-fire-buttons already uses for its own
    // P4-DECLINED line.
    // ========================================================================
    checks.record("sol:E-fire.c12 (p.330 figure, connector pin 6 = FIRE*, 'Left mouse button/fire button'): "
        "declined by module boundary -- ciaa.v has no connector, pin, or pin-numbering concept of any kind (grep "
        "evidence in the file header: zero hits for 'switch'/'joy'/'button' across all five of this fixture's "
        "source files). The pin-6-to-FIRE*-signal binding is made entirely inside userio.v; not gradable at "
        "`ciaa` alone. NOTE: this package's own classify.evidence field suggested building this claim 'at ciaa "
        "(or at minimig)' -- checked directly against ciaa.v and found not correct for `ciaa`: only a fixture "
        "spanning userio (or minimig, which instantiates it) has the connector-pin data this claim needs.");
    checks.record("opus:E-firebutton.c15 (p.330 text, same row, twin of sol:E-fire.c12): identical reasoning.");

    // ========================================================================
    // DDRA-FIGURE-CONTRADICTION -- opus:E-firebutton.c24 (p.330 figure,
    // DDRA VALUES row: bit7..bit0 = 0 0 0 0 0 0 1 1) and opus:E-firebutton.c25
    // (p.330 figure, DDRA LABELS row: IN IN OUT OUT OUT OUT OUT OUT). The two
    // are the two halves of ONE printed contradiction at bits 5:2 (value 0,
    // label OUT -- appendix F p.341 makes a DDR bit of 0 an INPUT), per the
    // build brief's own instruction: "Where a claim rests on either side of
    // one of these, measure both, record both, grade neither." Bits 7:6 (the
    // ones this package's other claims use, PROP-DDRA76 above) are
    // internally consistent on this same figure (0/IN both rows) and are
    // NOT part of this contradiction -- confirmed directly against
    // docs/references/amiga-hrm-3rd/appE/p330.txt: "0  0  0  0  0  0  1  1
    // DDRA" / "IN IN OUT OUT OUT OUT OUT OUT", bit7 down to bit0.
    // ========================================================================
    checks.note("DDRA-FIGURE-CONTRADICTION-MEASURED (docs/references/amiga-hrm-3rd/appE/p330.txt, verified "
        "against figures/p330.png): value row bit7..bit0 = 0 0 0 0 0 0 1 1; label row bit7..bit0 = IN IN OUT OUT "
        "OUT OUT OUT OUT. Bits 7:6 (0/IN) and bits 1:0 (1/OUT) are self-consistent; bits 5:2 print value 0 with "
        "label OUT, which disagree (appendix F p.341: a DDR bit of 0 is an input). The manual settles which side "
        "is wrong without leaving itself (p.337/appendix F put RDY*/TK0*/WPRO*/CHNG* on PA5-PA2 as lines the "
        "drive asserts, i.e. inputs -- values right, labels wrong) but the page under classification prints both "
        "rows and states no resolution itself, so this package does not adjudicate it.");
    checks.record("opus:E-firebutton.c24 (p.330 figure, DDRA VALUES row, quote '0 0 0 0 0 0 1 1 DDRA'): the "
        "VALUES half of the printed bits-5:2 contradiction. Measured above; not graded -- see "
        "DDRA-FIGURE-CONTRADICTION-MEASURED. A test built from these values alone would demand PA5-PA2 read as "
        "genuinely input-only, which this package does not claim (its own graded proposition, "
        "PROP-DDRA76-INPUT-FOR-FIRE-READ, is scoped to bits 7:6 only and never touches bits 5:2).");
    checks.record("opus:E-firebutton.c25 (p.330 figure, DDRA LABELS row, quote 'IN IN OUT OUT OUT OUT OUT OUT'): "
        "the LABELS half of the same contradiction, twin of opus:E-firebutton.c24. A test built from these labels "
        "would demand PA5-PA2 be drivable as OUTPUTS, which would red hrm/ch8-ciaapra-disk-sense-bits' own "
        "accepted grading of those same bits as inputs (read directly: that package's PROP-INPUT). Measured, not "
        "graded.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
