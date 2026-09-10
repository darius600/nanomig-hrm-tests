// ch3-collision-registers -- denise_collision conformance test
//
// Test purpose: CLXCON and CLXDAT as chapter 3 reaches them through
//   appendix A: the enable and match fields that decide which bitplanes and
//   sprites count toward a collision, and the read that clears. Package
//   ch3-collision-registers, fixture denise_collision (one Verilator build,
//   this module only, per PACKAGE.md).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 272, 273, 289, 290 (claims.jsonl for ch3-collision-registers, 98
//   records). A red result below means denise_collision.v disagrees with
//   the manual, not that the assertion is wrong -- no assertion here was,
//   or should be, adjusted to make the RTL pass.
// Non-goals: CLXCON2 (AGA, 9h10e) is not in this package's 98-record
//   inventory and is not asserted. `aga`/`dblpf` are held at their power-up
//   default (0) throughout -- no claim in this package names them.
//
// BOUNDARY: nanomig/src/minimig-aga/denise_collision.v:19 (CLXCON 9h098),
//   per PACKAGE.md, whose own evidence field additionally cites
//   denise_collision.v:19-21 (CLXCON 9h098, CLXCON2 9h10e, CLXDAT 9h00e)
//   and denise.v:474-486 (instantiation with bpldata and nsprite).
//
// ORDER OF WORK: this file (assertions, expected values, claim-id coverage)
// was written and committed from claims.jsonl's quotations and PACKAGE.md's
// own evidence field ONLY -- denise_collision.v itself was not opened until
// after that commit, per the build brief's explicit gate. The Harness's
// port names and clocking mechanics (clk/clk7_en toggling, reg_address_in/
// data_in write sequencing, the reg_address_in word-address convention
// $098->0x4C, $00E->0x07, bpldata as an 8-bit per-bitplane hit vector,
// nsprite as an 8-bit per-sprite hit vector) are reused from
// hrm/denise_collision/test_denise_collision.cpp -- an already-committed,
// already-built sibling harness for the SAME module -- not learned by
// opening this package's own forbidden-until-committed file.
//
// ASSUMPTION carried from that sibling harness: bpldata bit (n-1) is
// bitplane n's live data bit (n=1..6), and nsprite bit k is sprite k's live
// hit bit (k=0..7). This is inherited usage, not something read out of the
// RTL body here. If it is wrong the symptom is a coherent, systematic
// mislabeling (e.g. every "bitplane 3" result actually measuring bitplane
// 4), which is a plumbing fix at the next commit, not an oracle change.
//
// THE CENTRAL DERIVED RULE below (used to build every stimulus, not read
// out of any single quote): "playfield N is present at a pixel iff every
// CLXCON-enabled bitplane assigned to playfield N has its live bit equal to
// its MVBP requirement; a bitplane with ENBP=0 is not consulted at all."
// This is the literal, most direct reading of three quoted sentences taken
// together -- "Enable bitplane n (match required for collision)" (p.272,
// each ENBPn bit), "Disabled bitplanes cannot prevent collisions" and
// "if all bitplanes are disabled, collisions will be continuous, regardless
// of the match values" (p.272 NOTE) -- and it is what turns the NOTE's
// extreme case (zero enabled bitplanes -> vacuously present) into the same
// rule as the one-bitplane case. Every stimulus in the BP-ENABLE-MATCH,
// ALL-DISABLED-CONTINUOUS, PLAYFIELD-ODD-EVEN and BITMAP-PAIR groups below
// is built from this rule and from nothing else. If denise_collision.v
// implements a different combination (AND across a different set, a
// different polarity, an OR instead of a vacuous-true), the mismatch is a
// finding, not a mistake in the harness.
//
// CLXDAT bit->pair table used throughout (from A-clxdat.b00..b14 /
// A-clxdat.c7..c21, p.273): 0 PF1-PF2, 1 PF1-SPR01, 2 PF1-SPR23,
// 3 PF1-SPR45, 4 PF1-SPR67, 5 PF2-SPR01, 6 PF2-SPR23, 7 PF2-SPR45,
// 8 PF2-SPR67, 9 SPR01-SPR23, 10 SPR01-SPR45, 11 SPR01-SPR67,
// 12 SPR23-SPR45, 13 SPR23-SPR67, 14 SPR45-SPR67, 15 not used.
//
// CLAIM COVERAGE: all 98 records of claims.jsonl are cited below, in full,
// inside an expect() label. Ids restating the identical proposition under a
// different manual wording, page, or reader are grouped and cited together
// on the single assertion (or single shared-mechanism loop) that tests that
// proposition -- the convention hrm/ch3-colour-register-format/ and
// hrm/ch3-ham-control-field/ both document and use. No claim id in this
// package's inventory was declined; every one names a proposition
// measurable at this module's own ports.
//
//   CLXCON-ADDR (7 ids)        -- register identity/address $098.
//   CLXCON-WRITEONLY (4 ids)   -- no readback path at CLXCON's own address.
//   BP-ENABLE-MATCH (27 ids, loop x6)  -- each bitplane's MVBPn+ENBPn.
//   ALL-DISABLED-CONTINUOUS (3 ids)    -- the NOTE's extreme case.
//   ENSP-OR (10 ids, loop x4)  -- each odd sprite ORed with its even pair.
//   CLXDAT-ADDR (6 ids)        -- register identity/address $00E.
//   CLXDAT-READONLY (3 ids)    -- data_in has no effect on CLXDAT content.
//   CLXDAT-READ-CLEAR (2 ids)  -- read returns, then clears.
//   PLAYFIELD-ODD-EVEN (4 ids) -- odd bitplanes are PF1, even are PF2.
//   BITMAP-PAIR (30 ids, loop x15)     -- each CLXDAT bit's collision pair.
//   BIT15-UNUSED (2 ids)       -- bit 15 stays clear even under maximal
//                                  stimulation.
//   7+4+27+3+10+6+3+2+4+30+2 = 98.
//
// CONTROL CASES: CONTROL-KNOWN-VALUES-ARRIVE (the write->collide->read data
// path is alive before anything below trusts it) and two paired in-block
// controls (ALL-DISABLED-CONTINUOUS's own control, and READ-CLEAR's own
// "a fresh collision after the clear is still captured" control) -- all
// three carry no claim id and are prefixed "[control]" so the mutation gate
// does not require a mutation record for them (matching the convention
// already used by hrm/ch3-colour-register-format/'s
// CONTROL-KNOWN-VALUES-ARRIVE and EHB-CONTROL-NORMAL-REPRODUCIBLE).
//
// RESULTS (after opening the RTL and building): 31 passed, 5 failed. Every
// numeric prediction the spec was written against held for 31 of the 36
// checks. Two genuine, real findings account for all 5 failures -- neither
// assertion was adjusted to hide them:
//
//   FINDING 1 -- oddmatch is gated by evenmatch. denise_collision.v:62-63:
//     `assign oddmatch = bm[6]&bm[4]&bm[2]&bm[0] & (dblpf | evenmatch);
//      assign evenmatch = bm[7]&bm[5]&bm[3]&bm[1];` The RTL's own comment
//     (l.57-61) calls this "an undocumented function in the real Denise
//     chip ... trigger was the game Rotor". With dblpf held at 0 (this
//     package's stated non-goal), playfield 1's presence is silently ANDed
//     with playfield 2's presence -- something Appendix A's per-bit table
//     (p.273) does not describe; the table treats each pair as its own
//     independent proposition. This fails 4 of BITMAP-PAIR's 15 cases
//     (every pair naming PF1 except PF1-PF2 itself, which does not need
//     PF1 isolated from PF2 to read correctly).
//   FINDING 2 -- CLXDAT bit 15 is hard-wired high, never 0.
//     denise_collision.v:117: `data_out = ...==CLXDAT[8:1] ?
//     {1'b1,clxdat[14:0]} : 16'd0;` -- every successful CLXDAT read forces
//     bit 15 to 1 regardless of accumulated collisions, contradicting the
//     most direct reading of Appendix A's '15  not used' (p.273) as "does
//     not carry collision data, reads as 0". This is orthogonal to every
//     other assertion in this file (masked out of them with `& 0x7FFF`,
//     a measurement-scoping fix, not an oracle change -- see BIT15-UNUSED,
//     the one assertion that does not mask it and is the one that measures
//     it) and fails only BIT15-UNUSED.
//
// One wiring-only fix was needed beyond opening the RTL: `clxdat` carries
// no reset branch of its own, and on the very first simulated clock edge
// `clxcon` still reads as Verilator's power-on 0x0000 (before its own
// `<=16'h0fff` reset assignment has propagated), which makes every
// bitplane look disabled -- vacuously matched -- and latches a stray
// PF1-PF2 bit into clxdat before clxcon ever reaches its real reset value.
// Harness's constructor now ends with one drain read (documented at the
// drain site) -- harness hygiene, not a claim about collision behaviour;
// no expected value anywhere in this file was changed to accommodate it.
//
// MUTATION LOG (every line below was actually built with its own flag
// defined to 1, e.g. `-CFLAGS -DMUT_ADDR=1`, confirmed to turn only its
// own assertion red -- on top of the 5 already-red findings above, which
// stayed exactly the same set except under MUT_BITMAP, where the mutation's
// own target (SPR23-SPR67) joined them as a 6th -- then rebuilt with no
// flags and reconfirmed the clean 31-passed/5-failed baseline):
//
// MUTATION CLXCON-ADDR stimulus red: config B (meant to fully disable all six bitplanes) was changed to re-enable BP1 with the same match requirement as config A instead, so the two reads became identical and bit 1 stayed clear in both.
// MUTATION CLXCON-WRITEONLY stimulus red: the second read's address was redirected from CLXCON (0x4C) to CLXDAT (0x07), picking up the real pending SPR0+SPR2 collision instead of the constant zero CLXCON's address returns.
// MUTATION BP-ENABLE-MATCH stimulus red: bitplane 3's "matched" stimulus was redirected to present bitplane 1's live bit instead of bitplane 3's own, so bitplane 3 (the only one CLXCON has enabled) never actually receives a matching value.
// MUTATION ALL-DISABLED-CONTINUOUS stimulus red: instead of leaving all six bitplanes disabled, bitplane 1 was enabled with its match value required and left mismatched (data 0), so PF1 is no longer vacuously present and the continuous PF1-PF2 bit does not set.
// MUTATION ENSP-OR stimulus red: ENSP3's own sprite-3 stimulus was dropped to 0 (sprite 3 never actually presented), so sprite 3 cannot join the SPR23 group regardless of ENSP3, and SPR23-SPR45 (bit 12) stays clear.
// MUTATION CLXDAT-ADDR stimulus red: the second collision's stimulus was changed to repeat the first pair (SPR0+SPR2) instead of a genuinely different one (SPR0+SPR4), so the two reads stopped differing.
// MUTATION CLXDAT-READONLY stimulus red: the second harness's collision was changed from the identical SPR0+SPR2 pair to a genuinely different SPR0+SPR4 pair, so val_a and val_b legitimately diverge, confirming the check does discriminate when the underlying collision actually differs.
// MUTATION CLXDAT-READ-CLEAR stimulus red: an unintended second SPR0+SPR2 collision was inserted between the first and second read, so the second read legitimately returns a nonzero value instead of the expected zero.
// MUTATION PLAYFIELD-ODD-EVEN stimulus red: the even-bitplane-6 case's live-bit stimulus was redirected to bitplane 5's bit position instead of bitplane 6's own, so bitplane 6 (the only one CLXCON has enabled in that harness) never receives a matching value and PF2-SPR01 stays clear.
// MUTATION BITMAP-PAIR stimulus red: the SPR23-SPR67 case's Y-entity stimulus was redirected from SPR67 to SPR45, so the pair actually produced is SPR23-SPR45 (bit 12) instead of SPR23-SPR67 (bit 13), and bit 13 stays clear.
// MUTATION BIT15-UNUSED none impossible: BIT15-UNUSED is already red at baseline (Finding 2 above) -- the RTL forces bit 15 high on every successful CLXDAT read regardless of any stimulus this harness can present, so no stimulus mutation can turn it from green to red; there is nothing green here to break. Swapping the inspected bit from 15 to 14 was tried and confirmed to still read FAIL, not a clean green-to-red transition, for exactly that reason.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include "Vdenise_collision.h"
#include "verilated.h"

#ifndef MUT_ADDR
#define MUT_ADDR 0
#endif
#ifndef MUT_WO
#define MUT_WO 0
#endif
#ifndef MUT_BP
#define MUT_BP 0
#endif
#ifndef MUT_ALLDIS
#define MUT_ALLDIS 0
#endif
#ifndef MUT_ENSP
#define MUT_ENSP 0
#endif
#ifndef MUT_CLXDATADDR
#define MUT_CLXDATADDR 0
#endif
#ifndef MUT_READONLY
#define MUT_READONLY 0
#endif
#ifndef MUT_READCLEAR
#define MUT_READCLEAR 0
#endif
#ifndef MUT_PFPARITY
#define MUT_PFPARITY 0
#endif
#ifndef MUT_BITMAP
#define MUT_BITMAP 0
#endif
#ifndef MUT_BIT15
#define MUT_BIT15 0
#endif

namespace {

constexpr int CLXCON_ADDR = 0x4C;  // $098 >> 1
constexpr int CLXDAT_ADDR = 0x07;  // $00E >> 1
constexpr int IDLE_ADDR = 0x00;    // outside both decoded addresses.

// CLXCON bit positions (p.272): MVBPn at bit (n-1), ENBPn at bit (6+n-1),
// ENSP1/3/5/7 at bits 12/13/14/15.
constexpr uint16_t MVBP(int n) { return static_cast<uint16_t>(1u << (n - 1)); }
constexpr uint16_t ENBP(int n) { return static_cast<uint16_t>(1u << (6 + n - 1)); }
constexpr uint16_t ENSP(int oddSprite) { return static_cast<uint16_t>(1u << (12 + (oddSprite - 1) / 2)); }

class Harness {
public:
    Harness() : dut(new Vdenise_collision) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->reset = 1;
        dut->aga = 0;
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        dut->dblpf = 0;
        dut->bpldata = 0;
        dut->nsprite = 0;
        for (int i = 0; i < 4; ++i) tick_7mhz();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick_7mhz();
        // Drain: clxdat has no reset branch of its own in the RTL, and on
        // the very first simulated edge clxcon still reads as Verilator's
        // power-on 0x0000 (before its own reset assignment has propagated),
        // which makes every bitplane look disabled -- vacuously matched --
        // and latches a stray bit 0 (PF1-PF2) into clxdat before clxcon
        // ever reaches its real 0x0fff reset value. One read clears it, as
        // documented (CLXDAT-READ-CLEAR); this is harness hygiene, not a
        // claim about collision behaviour.
        read_reg(CLXDAT_ADDR);
    }
    ~Harness() { delete dut; }

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    void tick_7mhz() {
        dut->clk7_en = 1; tick();
        dut->clk7_en = 0; tick(); tick(); tick();
    }

    void write_reg(int addr, uint16_t data) {
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick_7mhz();
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        tick_7mhz();
    }

    uint16_t read_reg(int addr) {
        dut->reg_address_in = addr;
        tick_7mhz();
        uint16_t out = dut->data_out;
        dut->reg_address_in = IDLE_ADDR;
        tick_7mhz();
        return out;
    }

    // Presents bpldata/nsprite for exactly one 7MHz tick, then clears both.
    void pulse(uint8_t bpl, uint8_t spr) {
        dut->bpldata = bpl;
        dut->nsprite = spr;
        tick_7mhz();
        dut->bpldata = 0;
        dut->nsprite = 0;
    }

    Vdenise_collision* dut;
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
    // is trusted to read one back (package brief, "the fixture proves it can
    // deliver a known value before anything reads one").
    // =========================================================================
    {
        Harness h;
        // BP1 and BP2 (one odd, one even) enabled with match=1 required,
        // bpldata held at 0 below -> both playfields are definitively
        // ABSENT throughout this block (mismatched, not vacuous), isolating
        // the sprite-only bits (9, 10) that this control actually measures.
        h.write_reg(CLXCON_ADDR, ENBP(1) | MVBP(1) | ENBP(2) | MVBP(2));
        h.read_reg(CLXDAT_ADDR);  // drain whatever accumulated before this config took effect
        h.pulse(0x00, 0x05);  // SPR0 + SPR2 -> SPR01-SPR23, bit 9
        uint16_t a = h.read_reg(CLXDAT_ADDR);
        h.pulse(0x00, 0x11);  // SPR0 + SPR4 -> SPR01-SPR45, bit 10
        uint16_t b = h.read_reg(CLXDAT_ADDR);
        checks.expect(
            (a & 0x7FFF) == 0x0200 && (b & 0x7FFF) == 0x0400 && a != b,
            "[control] CONTROL-KNOWN-VALUES-ARRIVE: two distinct, known "
            "sprite-collision stimuli (SPR0+SPR2, then SPR0+SPR4) produce "
            "two distinct, correctly-positioned CLXDAT readings ($0200 then "
            "$0400) through write_reg/pulse/read_reg before any assertion "
            "below trusts that path. No manual page: the control for every "
            "assertion in this file, on the same measurement basis.");
    }

    // =========================================================================
    // CLXCON-ADDR (p.272/290) -- CLXCON lives at word-address 0x4C ($098):
    // two different register contents, same stimulus otherwise, produce two
    // different collision outcomes.
    // =========================================================================
    {
        Harness h;
        h.write_reg(CLXCON_ADDR, ENBP(1) | MVBP(1));  // BP1 enabled, match=1
        h.read_reg(CLXDAT_ADDR);  // drain any garbage accumulated before this config took effect
        h.pulse(0x00, 0x01);  // BP1 data=0 (mismatch), SPR0 present
        uint16_t read1 = h.read_reg(CLXDAT_ADDR);

        bool bp1_disabled = true;
#if MUT_ADDR
        bp1_disabled = false;
#endif
        h.write_reg(CLXCON_ADDR, bp1_disabled ? 0x0000 : (ENBP(1) | MVBP(1)));
        h.pulse(0x00, 0x01);  // identical stimulus: SPR0 present, BP1 data=0
        uint16_t read2 = h.read_reg(CLXDAT_ADDR);

        checks.expect(
            (read1 & 0x0002) == 0 && (read2 & 0x0002) != 0,
            "CLXCON-ADDR (p.272/290): writing CLXCON at word-address 0x4C "
            "($098) changes the collision outcome of an otherwise identical "
            "stimulus -- BP1 enabled+required (bit1 PF1-SPR01 clear) versus "
            "all bitplanes disabled (bit1 set, PF1 vacuously present) -- "
            "'CLXCON 098 W D Collision control' (p.272). A-clxcon.addr, "
            "A-clxcon.c1, A-clxcon.fn, B-clxcon.addr, B-clxcon.chip, "
            "B-subject.c22, B-subject.c25.");
    }

    // =========================================================================
    // CLXCON-WRITEONLY (p.272/290) -- no readback path at CLXCON's own
    // address: attempting to read at 0x4C after two different CLXCON writes
    // does not distinguish them, unlike CLXDAT's own address.
    // =========================================================================
    {
        Harness h;
        h.write_reg(CLXCON_ADDR, 0x0FFF);
        h.pulse(0x00, 0x05);  // SPR0+SPR2 -> would set CLXDAT bit 9 if ever read
        uint16_t val1 = h.read_reg(CLXCON_ADDR);

        h.write_reg(CLXCON_ADDR, 0x0000);
        int second_read_addr = CLXCON_ADDR;
#if MUT_WO
        second_read_addr = CLXDAT_ADDR;
#endif
        uint16_t val2 = h.read_reg(second_read_addr);

        checks.expect(
            val1 == val2,
            "CLXCON-WRITEONLY: reading at CLXCON's own word-address 0x4C "
            "after two different CLXCON writes ($0FFF then $0000) returns "
            "the same value both times -- no per-content readback exists at "
            "that address -- 'CLXCON 098 W D Collision control' (p.272). "
            "A-clxcon.c2, A-clxcon.rw, B-clxcon.rw, B-subject.c23.");
    }

    // =========================================================================
    // BP-ENABLE-MATCH (p.272) -- each bitplane's MVBPn (match value) and
    // ENBPn (enable, match required for collision) bits together decide
    // whether that bitplane's live data counts toward its playfield's
    // presence. One shared mechanism, six bitplanes.
    // =========================================================================
    {
        struct BpCase {
            int n;
            bool odd;  // odd bitplane -> PF1 group; even -> PF2 group
            const char* ids;
        };
        static const BpCase cases[6] = {
            {1, true,  "A-clxcon.b00, A-clxcon.b06, A-clxcon.c22, A-clxcon.c16, "
                       "A-clxcon.c01, A-clxcon.c4, A-clxcon.c5"},
            {2, false, "A-clxcon.b01, A-clxcon.b07, A-clxcon.c21, A-clxcon.c15"},
            {3, true,  "A-clxcon.b02, A-clxcon.b08, A-clxcon.c20, A-clxcon.c14"},
            {4, false, "A-clxcon.b03, A-clxcon.b09, A-clxcon.c19, A-clxcon.c13"},
            {5, true,  "A-clxcon.b04, A-clxcon.b10, A-clxcon.c18, A-clxcon.c12"},
            {6, false, "A-clxcon.b05, A-clxcon.b11, A-clxcon.c17, A-clxcon.c11"},
        };
        for (const auto& bc : cases) {
            Harness h;
            h.write_reg(CLXCON_ADDR, ENBP(bc.n) | MVBP(bc.n));

            uint8_t own_bit = static_cast<uint8_t>(1u << (bc.n - 1));
            int mutated_own_bit = own_bit;
#if MUT_BP
            if (bc.n == 3) mutated_own_bit = static_cast<uint8_t>(1u << 0);  // BP1's bit instead of BP3's
#endif
            // matched: this bitplane's own live bit = 1 (its MVBP requirement), paired with SPR0
            h.pulse(static_cast<uint8_t>(mutated_own_bit), 0x01);
            uint16_t matched = h.read_reg(CLXDAT_ADDR);

            // mismatched: this bitplane's own live bit = 0, same SPR0 pairing
            h.pulse(0x00, 0x01);
            uint16_t mismatched = h.read_reg(CLXDAT_ADDR);

            int bit = bc.odd ? 1 : 5;  // PF1-SPR01 or PF2-SPR01
            uint16_t mask = static_cast<uint16_t>(1u << bit);
            checks.expect(
                (matched & mask) != 0 && (mismatched & mask) == 0,
                std::string("BP-ENABLE-MATCH BP") + std::to_string(bc.n) +
                    " (p.272): with only bitplane " + std::to_string(bc.n) +
                    " enabled and its match value set to 1, presenting its "
                    "own live bit as 1 (matched) sets the " +
                    (bc.odd ? std::string("PF1-SPR01 (bit 1)") : std::string("PF2-SPR01 (bit 5)")) +
                    " collision bit; presenting it as 0 (mismatched) leaves "
                    "that bit clear, with sprite 0 present identically in "
                    "both cases -- 'Enable bitplane " + std::to_string(bc.n) +
                    " (match required for collision)' and 'Match value for "
                    "bitplane " + std::to_string(bc.n) + " collision' (p.272). " +
                    bc.ids + ".");
        }
    }

    // =========================================================================
    // ALL-DISABLED-CONTINUOUS (p.272 NOTE) -- with every ENBPn bit clear,
    // collisions are continuous regardless of the match values: PF1-PF2
    // (bit 0) sets on its own, with no bitplane or sprite data presented at
    // all.
    // =========================================================================
    {
        Harness h;
        bool mutate_to_enabled = false;
#if MUT_ALLDIS
        mutate_to_enabled = true;
#endif
        h.write_reg(CLXCON_ADDR, mutate_to_enabled ? (ENBP(1) | MVBP(1)) : 0x0000);
        h.pulse(0x00, 0x00);  // nothing presented at all
        uint16_t result = h.read_reg(CLXDAT_ADDR);

        checks.expect(
            (result & 0x0001) != 0,
            "ALL-DISABLED-CONTINUOUS (p.272): with all six CLXCON bitplane "
            "enables clear, the PF1-PF2 collision bit (bit 0) is set with no "
            "bitplane data and no sprite data presented at all -- "
            "'Disabled bitplanes cannot prevent collisions. Therefore if "
            "all bitplanes are disabled, collisions will be continuous, "
            "regardless of the match values.' (p.272 NOTE). A-clxcon.note, "
            "A-clxcon.c23, A-clxcon.c24.");

        // paired control, on the same measurement basis: enabling one
        // bitplane and mismatching it stops the continuous PF1-PF2 result,
        // so the result above is not a stuck-at-1 harness bug.
        Harness h2;
        h2.write_reg(CLXCON_ADDR, ENBP(1) | MVBP(1));
        h2.pulse(0x00, 0x00);  // BP1 data=0, mismatched against MVBP1=1
        uint16_t control_result = h2.read_reg(CLXDAT_ADDR);
        checks.expect(
            (control_result & 0x0001) == 0,
            "[control] ALL-DISABLED-CONTINUOUS-CONTROL: the same PF1-PF2 "
            "bit (bit 0), same harness, with bitplane 1 enabled and "
            "mismatched instead of all bitplanes disabled, is NOT "
            "continuously set. No manual page: the control for "
            "ALL-DISABLED-CONTINUOUS, on the same measurement basis.");
    }

    // =========================================================================
    // ENSP-OR (p.272) -- each odd sprite's enable bit ORs it with its
    // even-numbered partner for collision purposes. One shared mechanism,
    // four sprite pairs.
    // =========================================================================
    {
        struct EnspCase {
            int odd_sprite;     // 1, 3, 5, 7
            uint8_t odd_bit;    // nsprite bit for the odd sprite
            uint8_t partner_bit;  // nsprite bit for an unrelated present group
            int expect_bit;     // CLXDAT bit that proves the odd sprite joined its group
            const char* ids;
        };
        static const EnspCase cases[4] = {
            {1, 0x02, 0x04, 9,  "A-clxcon.b12, A-clxcon.c10, A-clxcon.c02, A-clxcon.c6"},  // SPR1 + SPR2 -> SPR01-SPR23 bit9
            {3, 0x08, 0x10, 12, "A-clxcon.b13, A-clxcon.c9"},   // SPR3 + SPR4 -> SPR23-SPR45 bit12
            {5, 0x20, 0x40, 14, "A-clxcon.b14, A-clxcon.c8"},   // SPR5 + SPR6 -> SPR45-SPR67 bit14
            {7, 0x80, 0x01, 11, "A-clxcon.b15, A-clxcon.c7"},   // SPR7 + SPR0 -> SPR01-SPR67 bit11
        };
        for (const auto& ec : cases) {
            Harness h;
            h.write_reg(CLXCON_ADDR, ENSP(ec.odd_sprite));

            uint8_t odd_stim = ec.odd_bit;
#if MUT_ENSP
            if (ec.odd_sprite == 3) odd_stim = 0x00;  // drop the odd sprite's own presence
#endif
            h.pulse(0x00, static_cast<uint8_t>(odd_stim | ec.partner_bit));
            uint16_t with_ensp = h.read_reg(CLXDAT_ADDR);

            uint16_t mask = static_cast<uint16_t>(1u << ec.expect_bit);
            checks.expect(
                (with_ensp & mask) != 0,
                std::string("ENSP-OR ENSP") + std::to_string(ec.odd_sprite) +
                    " (p.272): with ENSP" + std::to_string(ec.odd_sprite) +
                    " set, sprite " + std::to_string(ec.odd_sprite) +
                    " alone (its even partner clear) still joins its group "
                    "for collision purposes -- 'Enable sprite " +
                    std::to_string(ec.odd_sprite) + " (ORed with sprite " +
                    std::to_string(ec.odd_sprite - 1) + ")' (p.272). " +
                    ec.ids + ".");
        }
    }

    // =========================================================================
    // CLXDAT-ADDR (p.273/289) -- CLXDAT lives at word-address 0x07 ($00E)
    // and its content tracks real, distinct collisions.
    // =========================================================================
    {
        Harness h;
        h.write_reg(CLXCON_ADDR, 0x0000);  // all disabled: playfields vacuously present, harmless here
        h.pulse(0x00, 0x05);  // SPR0+SPR2
        uint16_t first = h.read_reg(CLXDAT_ADDR);

        uint8_t second_stim = 0x11;  // SPR0+SPR4 -> a genuinely different pair
#if MUT_CLXDATADDR
        second_stim = 0x05;  // same pair again
#endif
        h.pulse(0x00, second_stim);
        uint16_t second = h.read_reg(CLXDAT_ADDR);

        checks.expect(
            (first & 0x0200) != 0 && (second & 0x0400) != 0 && first != second,
            "CLXDAT-ADDR (p.273/289): reading at word-address 0x07 ($00E) "
            "after two different collisions returns two different, "
            "correctly-positioned values ($0200 then $0400) -- 'CLXDAT  00E "
            "R  D  Collision data register (read and clear)' (p.273/289). "
            "A-clxdat.addr, A-clxdat.c1, A-clxdat.fn, B-clxdat.addr, "
            "B-clxdat.addr-verbatim, B-subject.c1.");
    }

    // =========================================================================
    // CLXDAT-READONLY (p.273/289) -- data_in has no effect on what CLXDAT
    // reports: the same collision, presented with two different data_in
    // values while addressing CLXDAT, reads back identically.
    // =========================================================================
    {
        Harness h1;
        // Both playfields held definitively absent (see
        // CONTROL-KNOWN-VALUES-ARRIVE), so val_a/val_b below are not
        // contaminated by the all-disabled quirk.
        h1.write_reg(CLXCON_ADDR, ENBP(1) | MVBP(1) | ENBP(2) | MVBP(2));
        h1.read_reg(CLXDAT_ADDR);  // drain whatever accumulated before this config took effect
        h1.pulse(0x00, 0x05);  // SPR0+SPR2 -> bit 9
        h1.dut->reg_address_in = CLXDAT_ADDR;
        h1.dut->data_in = 0x0000;
        h1.tick_7mhz();
        uint16_t val_a = h1.dut->data_out;

        Harness h2;
        h2.write_reg(CLXCON_ADDR, ENBP(1) | MVBP(1) | ENBP(2) | MVBP(2));
        h2.read_reg(CLXDAT_ADDR);
        uint8_t second_collision = 0x05;  // identical collision
#if MUT_READONLY
        second_collision = 0x11;  // a genuinely different collision (SPR0+SPR4)
#endif
        h2.pulse(0x00, second_collision);
        h2.dut->reg_address_in = CLXDAT_ADDR;
        h2.dut->data_in = 0xFFFF;  // deliberately different data_in
        h2.tick_7mhz();
        uint16_t val_b = h2.dut->data_out;

        checks.expect(
            val_a == val_b,
            "CLXDAT-READONLY: the same collision (SPR0+SPR2, bit 9), "
            "presented with data_in=$0000 versus data_in=$FFFF while "
            "addressing CLXDAT, reads back identically -- CLXDAT's content "
            "does not depend on data_in -- 'CLXDAT  00E  R  D  Collision "
            "data register (read and clear)' (p.273/289). A-clxdat.rw, "
            "A-clxdat.c2, B-subject.c2.");
    }

    // =========================================================================
    // CLXDAT-READ-CLEAR (p.273/289) -- THE package's specific instruction:
    // reading CLXDAT returns the accumulated collision bits and clears
    // them. A second read with no new collision in between must come back
    // zero.
    // =========================================================================
    {
        Harness h;
        // Same non-vacuous CLXCON reasoning as CONTROL-KNOWN-VALUES-ARRIVE
        // above: both playfields held definitively absent so the exact
        // values below are not contaminated by the all-disabled quirk
        // (ALL-DISABLED-CONTINUOUS, tested separately).
        h.write_reg(CLXCON_ADDR, ENBP(1) | MVBP(1) | ENBP(2) | MVBP(2));
        h.read_reg(CLXDAT_ADDR);  // drain whatever accumulated before this config took effect
        h.pulse(0x00, 0x05);  // SPR0+SPR2 -> bit 9
        uint16_t first_read = h.read_reg(CLXDAT_ADDR);

#if MUT_READCLEAR
        h.pulse(0x00, 0x05);  // an unintended extra collision before the second read
#endif
        uint16_t second_read = h.read_reg(CLXDAT_ADDR);

        checks.expect(
            (first_read & 0x7FFF) == 0x0200 && (second_read & 0x7FFF) == 0x0000,
            "CLXDAT-READ-CLEAR (p.273/289): the first read after a SPR0+SPR2 "
            "collision returns $0200 (bit 9 set); a second read with no new "
            "collision in between returns $0000 -- the first read cleared "
            "the accumulated bits -- 'This address reads (and clears) the "
            "collision detection register.' / 'Collision data register "
            "(read and clear)' (p.273/289). A-clxdat.c3, B-subject.c4.");

        // paired control: a fresh, different collision after the clear is
        // still captured -- rules out the second read's zero being a dead
        // data path rather than a genuine clear.
        h.pulse(0x00, 0x11);  // SPR0+SPR4 -> bit 10
        uint16_t third_read = h.read_reg(CLXDAT_ADDR);
        checks.expect(
            (third_read & 0x7FFF) == 0x0400,
            "[control] READ-CLEAR-CONTROL: a fresh, different collision "
            "(SPR0+SPR4) presented after the clear is still captured and "
            "read back correctly ($0400) -- the data path is not dead, the "
            "prior zero was a genuine clear. No manual page: the control "
            "for CLXDAT-READ-CLEAR, on the same measurement basis.");
    }

    // =========================================================================
    // PLAYFIELD-ODD-EVEN (p.273 NOTE) -- playfield 1 is odd-numbered
    // enabled bitplanes, playfield 2 is even-numbered: bitplane 5 (odd)
    // feeds PF1's group bit, bitplane 6 (even) feeds PF2's, independent of
    // the BP-ENABLE-MATCH group's own bitplane-1/2 witnesses.
    // =========================================================================
    {
        Harness h1;
        h1.write_reg(CLXCON_ADDR, ENBP(5) | MVBP(5));
        h1.pulse(0x10, 0x01);  // bitplane 5 (bit4) matched, SPR0 present
        uint16_t odd_result = h1.read_reg(CLXDAT_ADDR);

        Harness h2;
        h2.write_reg(CLXCON_ADDR, ENBP(6) | MVBP(6));
        uint8_t bp6_stim = 0x20;  // bitplane 6 (bit5)
#if MUT_PFPARITY
        bp6_stim = 0x10;  // bitplane 5's bit instead -- BP6 itself never matches
#endif
        h2.pulse(bp6_stim, 0x01);  // SPR0 present
        uint16_t even_result = h2.read_reg(CLXDAT_ADDR);

        checks.expect(
            (odd_result & 0x0002) != 0 && (even_result & 0x0020) != 0,
            "PLAYFIELD-ODD-EVEN (p.273 NOTE): bitplane 5 (odd) enabled and "
            "matched sets PF1-SPR01 (bit 1); bitplane 6 (even) enabled and "
            "matched sets PF2-SPR01 (bit 5) -- 'Playfield 1 is all "
            "odd-numbered enabled bitplanes. Playfield 2 is all "
            "even-numbered enabled bitplanes' (p.273). A-clxdat.note1, "
            "A-clxdat.note2, A-clxdat.c4, A-clxdat.c5.");
    }

    // =========================================================================
    // BITMAP-PAIR (p.273) -- each of CLXDAT's 15 used bits names one
    // specific collision pair among six entities (PF1, PF2, SPR01, SPR23,
    // SPR45, SPR67). One shared mechanism, fifteen pairs: present both
    // named entities simultaneously, all four others absent, and confirm
    // exactly that bit results.
    // =========================================================================
    {
        // CLXCON: BP1 (odd) is PF1's sole enabled+required member, BP2
        // (even) is PF2's -- all other bitplanes disabled (irrelevant to
        // PF1/PF2 membership, per BP-ENABLE-MATCH). All four ENSP bits set
        // so every sprite pair's odd half also participates unconditionally.
        static const uint16_t clxcon_cfg =
            ENBP(1) | MVBP(1) | ENBP(2) | MVBP(2) |
            ENSP(1) | ENSP(3) | ENSP(5) | ENSP(7);

        // Entity stimuli: bpldata bit for playfields, nsprite bit for
        // sprite groups (even sprite alone suffices given ENSP above).
        enum Entity { PF1 = 0, PF2, SPR01, SPR23, SPR45, SPR67, NUM_ENTITIES };
        struct EntityStim { uint8_t bpl; uint8_t spr; const char* name; };
        static const EntityStim stim[NUM_ENTITIES] = {
            {0x01, 0x00, "PF1"},
            {0x02, 0x00, "PF2"},
            {0x00, 0x01, "SPR01"},
            {0x00, 0x04, "SPR23"},
            {0x00, 0x10, "SPR45"},
            {0x00, 0x40, "SPR67"},
        };

        struct Pair { Entity x, y; int bit; const char* ids; };
        static const Pair pairs[15] = {
            {PF1, PF2,   0,  "A-clxdat.b00, A-clxdat.c21"},
            {PF1, SPR01, 1,  "A-clxdat.b01, A-clxdat.c20"},
            {PF1, SPR23, 2,  "A-clxdat.b02, A-clxdat.c19"},
            {PF1, SPR45, 3,  "A-clxdat.b03, A-clxdat.c18"},
            {PF1, SPR67, 4,  "A-clxdat.b04, A-clxdat.c17"},
            {PF2, SPR01, 5,  "A-clxdat.b05, A-clxdat.c16"},
            {PF2, SPR23, 6,  "A-clxdat.b06, A-clxdat.c15"},
            {PF2, SPR45, 7,  "A-clxdat.b07, A-clxdat.c14"},
            {PF2, SPR67, 8,  "A-clxdat.b08, A-clxdat.c13"},
            {SPR01, SPR23, 9,  "A-clxdat.b09, A-clxdat.c12"},
            {SPR01, SPR45, 10, "A-clxdat.b10, A-clxdat.c11"},
            {SPR01, SPR67, 11, "A-clxdat.b11, A-clxdat.c10"},
            {SPR23, SPR45, 12, "A-clxdat.b12, A-clxdat.c9"},
            {SPR23, SPR67, 13, "A-clxdat.b13, A-clxdat.c8"},
            {SPR45, SPR67, 14, "A-clxdat.b14, A-clxdat.c7"},
        };

        for (const auto& p : pairs) {
            Harness h;
            h.write_reg(CLXCON_ADDR, clxcon_cfg);

            Entity y_entity = p.y;
#if MUT_BITMAP
            if (p.x == SPR23 && p.y == SPR67) y_entity = SPR45;  // wrong partner for this one pair
#endif
            uint8_t bpl = static_cast<uint8_t>(stim[p.x].bpl | stim[y_entity].bpl);
            uint8_t spr = static_cast<uint8_t>(stim[p.x].spr | stim[y_entity].spr);
            h.pulse(bpl, spr);
            uint16_t result = h.read_reg(CLXDAT_ADDR);

            uint16_t expected = static_cast<uint16_t>(1u << p.bit);
            char hexbuf[8];
            std::snprintf(hexbuf, sizeof(hexbuf), "%04X", expected);
            checks.expect(
                (result & 0x7FFF) == expected,
                std::string("BITMAP-PAIR ") + stim[p.x].name + "-" + stim[p.y].name +
                    " (p.273): presenting " + stim[p.x].name + " and " +
                    stim[p.y].name + " simultaneously, all other four "
                    "entities absent, sets exactly CLXDAT bit " +
                    std::to_string(p.bit) + " ($" + hexbuf +
                    ") -- Appendix A's CLXDAT bit table (p.273). " +
                    std::string(p.ids) + ".");
        }
    }

    // =========================================================================
    // BIT15-UNUSED (p.273) -- CLXDAT bit 15 stays clear even under maximal
    // stimulation (all six entities present at once, setting every one of
    // the 15 used bits simultaneously).
    // =========================================================================
    {
        Harness h;
        h.write_reg(CLXCON_ADDR,
                     ENBP(1) | MVBP(1) | ENBP(2) | MVBP(2) |
                     ENSP(1) | ENSP(3) | ENSP(5) | ENSP(7));
        h.pulse(0x03, 0x55);  // PF1+PF2 (bpldata bits0,1) and all four sprite groups (nsprite 0,2,4,6)
        uint16_t result = h.read_reg(CLXDAT_ADDR);

        int check_bit = 15;
#if MUT_BIT15
        check_bit = 14;  // a bit that DOES get set under this maximal stimulus
#endif
        bool bit_clear = (result & (1u << check_bit)) == 0;

        checks.expect(
            bit_clear && (result & 0x7FFF) == 0x7FFF,
            "BIT15-UNUSED (p.273): under maximal stimulation (all six "
            "entities present at once, all 15 used bits 0-14 set, "
            "CLXDAT=$7FFF), bit 15 stays clear -- '15  not used' (p.273). "
            "A-clxdat.b15, A-clxdat.c6.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
