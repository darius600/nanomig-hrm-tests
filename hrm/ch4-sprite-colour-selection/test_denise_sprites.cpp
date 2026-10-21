// ch4-sprite-colour-selection -- denise_sprites conformance test
//
// Test purpose: the sprite colour SELECTION MECHANISM at the denise_sprites
//   module boundary -- how a sprite's two data words (DATA, DATB) combine
//   into the 2-bit per-pixel value that picks a sprite colour, what code 00
//   (unattached) / 0000 (attached) means structurally (transparency, not a
//   colour-register selection), and that both data words genuinely
//   contribute to the selected value rather than one of them being ignored.
//   Package ch4-sprite-colour-selection, fixture denise_sprites (one
//   Verilator build, denise_sprites.v + denise_sprites_shifter.v, top module
//   denise_sprites -- the same fixture hrm/denise_sprites/,
//   hrm/ch3-sprite-registers/ and hrm/ch4-attached-sprites/ already build).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 130, 131 (this package's own assigned claims file, 67 records).
//
// CLAIM SOURCE / WHAT THIS FILE ACTUALLY COVERS -- READ THIS FIRST:
//   this package's own assigned claims file is 
//   hrm-testpipeline\work\ch4_packages\ch4-sprite-colour-selection.
//   jsonl (67 records, all on printed pp. 130-131: Table 4-6,
//   unattached sprites' 2-bit code, and Table 4-7, attached sprites'
//   4-bit code -- both explicitly flagged "chipset: OCS"). The build
//   task's own framing named pp. 102/107/113/121 and a DATA-vs-DATB
//   significance dispute (p107/p121 "high-order word", p126 "the
//   first fetched word... to SPRxDATA", p123 write order, Appendix A)
//   as this subject's wider context -- but NONE of those pages, and no
//   claim naming that dispute, occur anywhere in this package's own
//   67-record file (checked: grep of "page": across the whole file
//   returns only 130 and 131). This file therefore asserts NOTHING
//   about which word is the more significant one; see SPRITE-DATA-
//   WORDS-COMBINE-TO-SELECT-COLOUR below for how that is sidestepped,
//   and NOTE (DATA/DATB ORDER) further down for what was measured and
//   reported instead of adjudicated.
//
//   Of this package's own 67 records, 54 are, on inspection, PLAINLY
//   ABOUT WHICH REGISTER NUMBER A CODE MAPS TO ("selector 01 selects
//   color register 17", one row per code value per sprite pair/
//   nibble) rather than about the selection MECHANISM (how the bits
//   combine, and what a code means structurally) this package was
//   assigned. Per the build task's direct instruction, register-
//   number mapping belongs to the sibling package ch4-sprite-colour-
//   registers, built concurrently on this SAME fixture from the
//   303-claim ch4-sprite-colour-encoding.jsonl. Those 54 ids are
//   recorded, not asserted, by OUT-OF-SCOPE-REGISTER-MAPPING below --
//   see that record() call for the full list. The remaining 13 ids
//   ARE about the selection mechanism (transparency as a precondition/
//   special case of the code, and the general "data words select
//   colour" statement) and are asserted below.
//
//   Five ids the build task named as already covered by a package
//   built earlier tonight (hrm/ch4-attached-sprites: CH4-attach.c25,
//   CH4-attach.c26, CH4-attach.c27, CH4-p128.c8, CH4-sctl.c4) do NOT
//   appear in this package's own 67-record file at all (checked by
//   id, not merely by page) -- nothing to decline or skip for them
//   here; noted for the record only.
//
// Non-goals: the fifty-four register-number-mapping claim ids listed under
//   OUT-OF-SCOPE-REGISTER-MAPPING; collision and playfield- priority
//   behaviour (a different module boundary); vertical positioning (this
//   boundary carries no vertical-position input, only hpos, matching
//   hrm/ch3-sprite-registers/'s and hrm/ch4-attached-sprites/'s own
//   Non-goals note for the same module); the ATTACH-bit
//   odd/even-sprite-parity question (that is hrm/ch4-attached-sprites/'s own
//   finding, reused here only as already-proven plumbing to reach the
//   "attached" configuration Table 4-7 needs -- not re-asserted); which
//   physical word (DATA or DATB) is the more significant colour bit (see
//   CLAIM SOURCE above and the NOTE below
//   SPRITE-DATA-WORDS-COMBINE-TO-SELECT-COLOUR).
// Claim ids: ids such as `CH4-p130.c1` and `CH4-tab46s.c1` name one sentence
//   of the manual in this chapter's reading. They carry no reader prefix, and
//   they resolve in the claim files that hrm-testpipeline/acceptance/ch4.json
//   lists under `inventories`.
//   `FPGA-nnn` is a task number in the private workspace where this work was
//   planned; it resolves nowhere public, and docs/legacy-names.md gives the
//   finding keys that replaced the ones that were findings.
//
// BOUNDARY: denise_sprites.v / denise_sprites_shifter.v, reached at
//   $(NANOMIG)/src/minimig-aga/. Port list, register word-address map,
//   four-phase 7MHz clocking, and the DATB-then-DATA arm-on-write
//   convention are NOT read from those two files for
//   this package -- they are reused verbatim from THREE already-
//   committed sibling packages built against this exact fixture:
//   hrm/denise_sprites/test_denise_sprites.cpp (the original FPGA-030
//   harness), hrm/ch3-sprite-registers/test_denise_sprites.cpp (the
//   HRM-format harness for the same module), and hrm/ch4-attached-
//   sprites/test_denise_sprites.cpp (whose own FIXTURE-LIVE-DATA-PATH
//   control and CODE-0000-YIELDS-TRANSPARENT-PIXEL measurement recipes
//   this file's own control and attached-transparency assertions
//   follow). The SPR2/SPR4/SPR6 register base addresses this file adds
//   (needed because Table 4-6 lists FOUR separate sprite pairs, not
//   just sprite 0/1) are the arithmetic extension of that SAME
//   disclosed SPR0/SPR1 word-address pattern (+4 words per sprite
//   number), a pattern hrm/ch4-sprite-vertical-position/test_agnus_
//   spritedma.cpp and hrm/ch4-sprite-dma-sequence/test_agnus_
//   spritedma.cpp already disclose out to SPR7 ("addresses 0xA0-0xA3
//   ... through SPR7.. (0xBC-0xBF)") -- not learned by opening this
//   package's own RTL either.
//
// ORDER OF WORK: this file -- every assertion, its expected value, the
//   claim ids it cites, and the stimulus each measurement uses -- was
//   written and committed from claims.jsonl's own quotations and the
//   three already-committed sibling harnesses' disclosed interface
//   ONLY. denise_sprites.v and denise_sprites_shifter.v themselves
//   were not opened until after that commit. In particular, this file does NOT use
//   its own claims.jsonl records' "classify.evidence" fields (several
//   of which cite RTL line numbers and RTL-derived facts, e.g. a reset
//   value for esprm/osprm) -- only the "extract.quote" / "restatement"
//   fields (the manual's own words) were used to decide what to
//   assert; esprm/osprm are driven to a fixed 0 here, matching the
//   sibling harnesses' own explicit-drive convention, not any RTL
//   reset fact.
//
// THE PROPOSITIONS UNDER TEST, IN THE MANUAL'S OWN WORDS:
//   p130 (CH4-p130.c1/c5/c9/c13, CH4-tab46s.c1/c5/c9/c13 -- Table 4-6,
//   one row per unattached sprite pair): "0 or 1  00  Not used *" /
//   "2 or 3  00  Not used *" / "4 or 5  00  Not used *" / "6 or 7  00
//   Not used *".
//   p130 (CH4-p130.c17, CH4-tab46s.c17 -- Table 4-6's own footnote):
//   "* Selects transparent mode."
//   Read together: for EVERY unattached sprite pair, the 2-bit
//   combined code 00 is not a colour-register selector at all -- it is
//   the code that means "no colour", i.e. transparency is a case the
//   2-bit value can select, structurally different from the fifteen
//   codes 01/10/11-per-pair that DO name a register (those fifteen-
//   per-pair-times-four register numbers are OUT OF SCOPE here, see
//   above).
//   p131 (CH4-p131.c1, CH4-tab47.c1 -- Table 4-7, the attached
//   sprite's 4-bit combined code): "0000  Selects transparent mode".
//   Same structural fact, one level up: the FOUR-bit code (built from
//   both sprites' own two words each) also has one value, 0000, that
//   means transparency rather than a register.
//   p130 (CH4-sum.c1 -- the section's own opening sentence): "Sprite
//   data words are used to select the color of the sprite pixels
//   from" [the colour register set] (the claims.jsonl record's own
//   quote field is itself truncated at "from" -- quoted here exactly
//   as extracted, not completed by guesswork). This is the general
//   claim SPRITE-DATA-WORDS-COMBINE-TO-SELECT-COLOUR tests: that the
//   pixel's colour is actually a function of (both) data words, not a
//   fixed value or a function of only one of them.
//
// MEASUREMENT BASIS (FIXTURE-LIVE-DATA-PATH, [control]): identical
//   recipe to hrm/ch4-attached-sprites/'s own control (three single-
//   bit SPR0DATA stimuli -- 0x8000/0x0080/0x0001, bit 15/bit 7/bit 0
//   -- swept across hpos 0..511 from a fresh reset each time), proving
//   THIS package's own build of the harness delivers distinct fetched
//   values, in order, at the cycle each was fetched on, before any
//   claim-based assertion below depends on it (a harness whose data
//   path is silently dead reports every data-dependent assertion as a
//   finding about the design instead of about the harness). A passing build of a sibling
//   package on the same fixture does not substitute for this package's
//   own binary proving its own data path live.
//
// MEASUREMENT BASIS (CODE-00-SELECTS-TRANSPARENT-UNATTACHED-OCS): for
//   EACH of the four representative sprites 0, 2, 4, 6 (one per Table
//   4-6 pair: 0/1, 2/3, 4/5, 6/7), armed ALONE (unattached, that
//   sprite's own CTL=0x00) at the same SPRxPOS byte (0x40): (a) a
//   LIVENESS sub-check -- DATB=0x0000, DATA=0x8000 (a single 1-bit,
//   the same MSB stimulus the control above already validates for
//   sprite 0) is required to produce at least one non-zero sprdata
//   sample across the hpos sweep, proving THIS sprite's own shift/
//   output path is actually active in this configuration (the
//   precondition a green CODE-00 result depends on -- a blank image
//   showing zero output would otherwise be indistinguishable from a
//   dead or disarmed sprite); (b) the CODE-00 check itself --
//   DATB=0x0000, DATA=0x0000 (combined code 00 at every pixel) is
//   required to produce sprdata == 0 across the ENTIRE hpos sweep.
//   Both (a) and (b), for all four sprites, are combined into ONE
//   expect() (matching hrm/ch4-attached-sprites/'s own precedent of
//   bundling several claim ids that state the identical proposition
//   for different rows/pages into one assertion) since Table 4-6
//   states the SAME rule once per pair -- this is one proposition
//   repeated four times in the table's own layout, not four
//   propositions.
//
// MEASUREMENT BASIS (CODE-0000-SELECTS-TRANSPARENT-ATTACHED-OCS): the
//   pair (sprite 0 + sprite 1) is made genuinely ATTACHED (SPR1CTL bit
//   7 set -- the odd sprite's own bit, the mechanism hrm/ch4-attached-
//   sprites/ already proves live and odd-sprite-scoped; reused here
//   as already-proven plumbing, not re-asserted) with the SAME
//   pos_byte=0x40 on both sprites, reusing hrm/ch3-sprite-registers/'s
//   own measure_attach_color() recipe. (a) LIVENESS: SPR0 DATB=0x0000/
//   DATA=0x8000, SPR1 DATB=0x8000/DATA=0x8000 (both already-validated-
//   reachable values from that recipe) is required to produce at
//   least one non-zero sprdata sample -- proving the ATTACHED
//   configuration's own shift/output path is active, not merely that
//   an unattached sprite is. (b) the CODE-0000 check -- all four words
//   (SPR0 DATA/DATB, SPR1 DATA/DATB) held at 0x0000, combined 4-bit
//   code 0000 throughout -- is required to produce sprdata == 0 across
//   the entire hpos sweep. Both combined into one expect().
//
// MEASUREMENT BASIS (SPRITE-DATA-WORDS-COMBINE-TO-SELECT-COLOUR): an
//   UNattached sprite 0 at pos_byte=0x40 is armed three separate times
//   (fresh reset each time), always with bit 15 the only bit written
//   anywhere (matching the control's own already-validated single-
//   pixel stimulus), in three combinations of which WORD carries it:
//   run A -- DATB=0x0000, DATA=0x8000 (DATA alone); run B --
//   DATB=0x8000, DATA=0x0000 (DATB alone); run C -- DATB=0x8000,
//   DATA=0x8000 (both). Each run is required to show exactly one
//   active sample, at the SAME hpos across all three runs (bit 15 sets
//   the shift-out TIMING, which does not depend on which word the bit
//   came from -- if it did, "same hpos" would itself be informative
//   and is checked, not assumed). The three resulting sprdata VALUES
//   at that shared hpos (colourA, colourB, colourC) are required to be
//   (i) each non-zero (none of the three is the transparent/00 case)
//   and (ii) PAIRWISE DISTINCT. This is what "sprite data words are
//   used to select the color" (CH4-sum.c1) means at this boundary: the
//   selected value is a function of BOTH words' own bit, not of "was
//   any bit set anywhere" (which would make colourA==colourB==colourC)
//   and not of only one word (which would make two of the three
//   identical). No register NUMBER is asserted anywhere in this
//   measurement -- only distinctness -- keeping this package inside
//   "how the bits combine", not "which register", per the build task's
//   own boundary between this package and ch4-sprite-colour-registers.
//
// CLAIM COVERAGE -- 13 ids asserted (all cited in full inside an
//   expect() label), 54 ids declined with reason (register-number
//   mapping, out of scope):
//
//   CODE-00-SELECTS-TRANSPARENT-UNATTACHED-OCS (10 ids)
//     -- CH4-p130.c1, CH4-p130.c5, CH4-p130.c9, CH4-p130.c13,
//        CH4-p130.c17, CH4-tab46s.c1, CH4-tab46s.c5, CH4-tab46s.c9,
//        CH4-tab46s.c13, CH4-tab46s.c17
//   CODE-0000-SELECTS-TRANSPARENT-ATTACHED-OCS (2 ids)
//     -- CH4-p131.c1, CH4-tab47.c1
//   SPRITE-DATA-WORDS-COMBINE-TO-SELECT-COLOUR (1 id)
//     -- CH4-sum.c1
//   OUT-OF-SCOPE-REGISTER-MAPPING [declined] (54 ids) -- see that
//     record() call below for the full, explicit list.
//   10+2+1 = 13 asserted, +54 declined = 67, all 67 of this package's
//   own claims.jsonl records accounted for.
//
// CONTROL CASE: FIXTURE-LIVE-DATA-PATH (no page, no claim id, labelled
//   "[control]" so the mutation gate does not require a mutation
//   record naming a claim id for it, matching hrm/ch3-sprite-
//   registers/'s and hrm/ch4-attached-sprites/'s own convention)
//   proves the measurement basis every other assertion in this file
//   shares (write -> arm -> hpos-sweep -> read-sprdata) is alive, on
//   THIS package's own build. It is additionally the same recipe each
//   of the two CODE-* assertions' own embedded LIVENESS sub-checks
//   reuse per sprite/configuration, so a red CODE-* result cannot be
//   dismissed as "the sprite was just never on" -- its own liveness
//   half, measured the same way, is right there in the same
//   expect().
//
// RESULTS (after opening the RTL and building): 4
//   passed, 0 failed. All four graded assertions are GREEN: FIXTURE-
//   LIVE-DATA-PATH (MSB hpos=130, mid hpos=138, LSB hpos=145 --
//   distinct and monotonic), CODE-00-SELECTS-TRANSPARENT-UNATTACHED-
//   OCS (all four representative sprites 0/2/4/6 live=yes and
//   code00-transparent=yes), CODE-0000-SELECTS-TRANSPARENT-ATTACHED-
//   OCS, and SPRITE-DATA-WORDS-COMBINE-TO-SELECT-COLOUR (runA(DATA-
//   only)=1, runB(DATB-only)=2, runC(both)=3, all at hpos=130 --
//   non-zero and pairwise distinct). No red result on this package's
//   own 13 asserted claim ids; nothing was adjusted to make it so.
//
//   NOTE (DATA/DATB ORDER) FOLLOW-UP, now that the RTL is open: reading
//   denise_sprites_shifter.v directly (not this package's own claims.
//   jsonl, which names no page for this) explains WHY runA < runB <
//   runC above: the shift register's own load line is `shifta <=
//   datla` (DATA) and `shiftb <= datlb` (DATB), and the output tap is
//   `sprdata_r <= {shiftb[63], shifta[63], sprdata_r[7:2]}` -- shiftb's
//   own bit is written one position ABOVE shifta's own bit on every
//   injection and that relative order survives the shift, so at the
//   2-bit output DATB supplies the MORE significant bit and DATA the
//   LESS significant one (matching denise_sprites.v:596's own
//   {..,sprdat1,sprdat0} concatenation for the attached case one level
//   up, e.g. the pair value 11 at runC(both) above rather than a
//   simple OR). This is an RTL fact about THIS repository's copy of
//   the module, reported for the record as the build task's own
//   dispatch note asked -- it does not adjudicate the p107/p121-vs-
//   p126 printed-page dispute (still not present anywhere in this
//   package's own 67-record claims file) and is not graded as a
//   finding either way.
//
// MUTATION LOG (WSL/Verilator, same session as RESULTS above; each
//   entry actually run: rebuild with `-CFLAGS -DMUT_*=1` -> run -> read
//   the PASS/FAIL lines -> confirm only the named assertion changed
//   colour -> rebuild with no flags -> confirm the 4-passed/0-failed
//   baseline was restored):
//
// MUTATION FIXTURE-LIVE-DATA-PATH stimulus red: built with -DMUT_FIXTURE=1, which makes the MSB-run stimulus 0x0001 instead of 0x8000 (identical to the LSB-run) -> FAIL (3 passed, 1 failed), only FIXTURE-LIVE-DATA-PATH newly red (MSB hpos=145 became equal to LSB hpos=145, no longer distinct or monotonic; mid hpos=138 unchanged). Rebuilt with no flags: 4 passed, 0 failed, baseline restored.
// MUTATION CODE-00-SELECTS-TRANSPARENT-UNATTACHED-OCS stimulus red: built with -DMUT_PAIR4=1, which changes sprite 4's own CODE-00 stimulus from DATA=0x0000 to DATA=0x8000 (its own LIVENESS sub-check, and every other sprite's own stimulus, untouched) -> FAIL (3 passed, 1 failed), only CODE-00-SELECTS-TRANSPARENT-UNATTACHED-OCS newly red (detail string read "sprite 4 (pair 4/5) live=yes code00-transparent=no", the other three pairs unchanged). Rebuilt with no flags: 4 passed, 0 failed, baseline restored.
// MUTATION CODE-0000-SELECTS-TRANSPARENT-ATTACHED-OCS stimulus red: built with -DMUT_ATTACH0000=1, which changes SPR1's own CODE-0000 stimulus from DATA=0x0000 to DATA=0x8000 (combined code no longer 0000) -> FAIL (3 passed, 1 failed), only CODE-0000-SELECTS-TRANSPARENT-ATTACHED-OCS newly red. Rebuilt with no flags: 4 passed, 0 failed, baseline restored.
// MUTATION SPRITE-DATA-WORDS-COMBINE-TO-SELECT-COLOUR stimulus red: built with -DMUT_COMBINE=1, which makes run B's own stimulus identical to run A's (DATB=0x0000/DATA=0x8000 instead of DATB=0x8000/DATA=0x0000) -> FAIL (3 passed, 1 failed), only SPRITE-DATA-WORDS-COMBINE-TO-SELECT-COLOUR newly red (runB(DATB-only)=1, now equal to runA(DATA-only)=1, breaking pairwise distinctness; runC unchanged at 3). Rebuilt with no flags: 4 passed, 0 failed, baseline restored.
// MUTATION OUT-OF-SCOPE-REGISTER-MAPPING: no record -- this set of 54 ids is declined (checks.record() only, no checks.expect()), so there is no red/green state for a mutation to move, matching hrm/ch4-attached-sprites/'s own convention for its declined id.

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vdenise_sprites.h"

// MUT_FIXTURE: replay FIXTURE-LIVE-DATA-PATH's own mutation record above
// (-DMUT_FIXTURE=1). Makes the MSB-run stimulus identical to the LSB-run.
#ifndef MUT_FIXTURE
#define MUT_FIXTURE 0
#endif
// MUT_PAIR4: replay CODE-00-SELECTS-TRANSPARENT-UNATTACHED-OCS's own
// mutation record above (-DMUT_PAIR4=1). Sprite 4's own CODE-00 stimulus
// becomes non-zero.
#ifndef MUT_PAIR4
#define MUT_PAIR4 0
#endif
// MUT_ATTACH0000: replay CODE-0000-SELECTS-TRANSPARENT-ATTACHED-OCS's own
// mutation record above (-DMUT_ATTACH0000=1). SPR1's own CODE-0000
// stimulus becomes non-zero.
#ifndef MUT_ATTACH0000
#define MUT_ATTACH0000 0
#endif
// MUT_COMBINE: replay SPRITE-DATA-WORDS-COMBINE-TO-SELECT-COLOUR's own
// mutation record above (-DMUT_COMBINE=1). Run B's stimulus is made
// identical to run A's.
#ifndef MUT_COMBINE
#define MUT_COMBINE 0
#endif

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    // For records that state what the SOURCE says, or why a claim is
    // declined. Not a measurement -- must never be printed as though it
    // were one.
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Word-address register map, reused from hrm/denise_sprites/,
// hrm/ch3-sprite-registers/ and hrm/ch4-attached-sprites/'s own
// test_denise_sprites.cpp for this same module (SPRPOSCTLBASE byte
// address 0x140 -> word address 0xA0 for SPR0POS; 4 consecutive word
// addresses per sprite: POS, CTL, DATA, DATB). SPR2/SPR4/SPR6 extend
// this SAME disclosed pattern (+4 words per sprite number) -- see
// BOUNDARY above for the sibling packages that already disclose it out
// to SPR7 (0xBC-0xBF).
struct SpriteRegs { int pos, ctl, data, datb; };
constexpr SpriteRegs SPR0{0xA0, 0xA1, 0xA2, 0xA3};
constexpr SpriteRegs SPR1{0xA4, 0xA5, 0xA6, 0xA7};
constexpr SpriteRegs SPR2{0xA8, 0xA9, 0xAA, 0xAB};
constexpr SpriteRegs SPR4{0xB0, 0xB1, 0xB2, 0xB3};
constexpr SpriteRegs SPR6{0xB8, 0xB9, 0xBA, 0xBB};
constexpr int IDLE_ADDR = 0x00;

constexpr int HPOS_MAX = 512;  // SH8..SH0 is a 9-bit value: 0..511 covers it.

struct Meas {
    std::vector<uint8_t> trace;  // sprdata at every swept hpos.
};

// Clocking (clk7_en/c1/c3 four-phase tick, reg_address_in/data_in write
// sequencing) reused from the sibling harnesses for this same module, not
// learned from this package's own RTL (see ORDER OF WORK above).
class Harness {
public:
    Harness(uint8_t aga, uint8_t spres) : dut(new Vdenise_sprites) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->c1 = 0;
        dut->c3 = 0;
        dut->reset = 1;
        dut->aga = aga;
        dut->spres = spres;
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        dut->chip48 = 0;
        dut->hpos = 0;
        dut->sprena = 1;
        dut->esprm = 0;
        dut->osprm = 0;
        for (int i = 0; i < 4; ++i) tick_7mhz();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick_7mhz();
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
    }

    // c1=0,c3=0 -> shift/load; c1=1,c3=0; c1=1,c3=1; c1=0,c3=1.
    void tick_phase() {
        phase = (phase + 1) % 4;
        switch (phase) {
            case 0: dut->c1 = 0; dut->c3 = 0; dut->clk7_en = 1; break;
            case 1: dut->c1 = 1; dut->c3 = 0; dut->clk7_en = 0; break;
            case 2: dut->c1 = 1; dut->c3 = 1; dut->clk7_en = 0; break;
            case 3: dut->c1 = 0; dut->c3 = 1; dut->clk7_en = 0; break;
        }
        tick();
    }

    void tick_7mhz() {
        tick_phase();
        tick_phase();
        tick_phase();
        tick_phase();
    }

    void write_reg(int addr, uint16_t data) {
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick_7mhz();
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        tick_7mhz();
    }

    Vdenise_sprites* dut;
    int phase = 0;
};

// Arms one sprite (given its own register block r) alone, unattached
// (CTL=0x00), at pos_byte, with the given DATA/DATB words (DATB written
// first, then DATA -- the arm-on-DATA-write convention reused from the
// sibling harnesses), sweeps hpos, records the full sprdata trace.
Meas measure_single(const SpriteRegs& r, uint8_t pos_byte, uint16_t data_word,
                     uint16_t datb_word = 0x0000) {
    Harness h(/*aga=*/0, /*spres=*/0);
    h.write_reg(r.pos, pos_byte);
    h.write_reg(r.ctl, 0x00);
    h.write_reg(r.datb, datb_word);
    h.write_reg(r.data, data_word);
    Meas m;
    for (int hp = 0; hp < HPOS_MAX; ++hp) {
        h.dut->hpos = hp;
        h.tick_7mhz();
        m.trace.push_back(static_cast<uint8_t>(h.dut->sprdata));
    }
    return m;
}

// Arms BOTH sprites of the pair (sprite 0 = even, sprite 1 = odd) at the
// same pos_byte, sprite 1 with ATTACH (SPR1CTL bit 7) set, reusing
// hrm/ch3-sprite-registers/'s own measure_attach_color() recipe for this
// module (not derived from this package's own RTL). Used by
// CODE-0000-SELECTS-TRANSPARENT-ATTACHED-OCS.
Meas measure_pair_attached(uint8_t pos_byte, uint16_t spr0data_word,
                            uint16_t spr1data_word) {
    Harness h(/*aga=*/0, /*spres=*/0);
    h.write_reg(SPR0.pos, pos_byte);
    h.write_reg(SPR0.ctl, 0x00);
    h.write_reg(SPR0.datb, 0x0000);
    h.write_reg(SPR0.data, spr0data_word);
    h.write_reg(SPR1.pos, pos_byte);
    h.write_reg(SPR1.ctl, 0x80);  // ATTACH on the odd sprite.
    h.write_reg(SPR1.datb, spr1data_word);
    h.write_reg(SPR1.data, spr1data_word);
    Meas m;
    for (int hp = 0; hp < HPOS_MAX; ++hp) {
        h.dut->hpos = hp;
        h.tick_7mhz();
        m.trace.push_back(static_cast<uint8_t>(h.dut->sprdata));
    }
    return m;
}

// First (and, for a single-1-bit stimulus, only) hpos at which sprdata is
// non-zero, or -1. Also reports whether more than one sample was
// non-zero, and the sprdata value seen at that first active sample.
struct FirstActive {
    int hpos = -1;
    int active_count = 0;
    uint8_t value = 0;
};
FirstActive first_active(const Meas& m) {
    FirstActive r;
    for (size_t i = 0; i < m.trace.size(); ++i) {
        if (m.trace[i] != 0) {
            if (r.hpos == -1) { r.hpos = static_cast<int>(i); r.value = m.trace[i]; }
            ++r.active_count;
        }
    }
    return r;
}

bool all_zero(const Meas& m) {
    for (uint8_t v : m.trace) { if (v != 0) return false; }
    return true;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- FIXTURE-LIVE-DATA-PATH [control] ----------------
    {
        uint16_t lsb_word = 0x0001;
#if MUT_FIXTURE
        uint16_t msb_word = 0x0001;  // MUTATION: identical to lsb_word.
#else
        uint16_t msb_word = 0x8000;
#endif
        Meas msb = measure_single(SPR0, /*pos=*/0x40, msb_word);
        Meas mid = measure_single(SPR0, /*pos=*/0x40, 0x0080);
        Meas lsb = measure_single(SPR0, /*pos=*/0x40, lsb_word);
        FirstActive fa_msb = first_active(msb);
        FirstActive fa_mid = first_active(mid);
        FirstActive fa_lsb = first_active(lsb);
        bool each_single = fa_msb.active_count == 1 && fa_mid.active_count == 1 &&
                            fa_lsb.active_count == 1;
        bool all_found = fa_msb.hpos != -1 && fa_mid.hpos != -1 && fa_lsb.hpos != -1;
        bool distinct = fa_msb.hpos != fa_mid.hpos && fa_mid.hpos != fa_lsb.hpos &&
                         fa_msb.hpos != fa_lsb.hpos;
        bool monotonic = distinct && all_found &&
            ((fa_msb.hpos < fa_mid.hpos && fa_mid.hpos < fa_lsb.hpos) ||
             (fa_msb.hpos > fa_mid.hpos && fa_mid.hpos > fa_lsb.hpos));
        checks.expect(each_single && all_found && distinct && monotonic,
            "[control] FIXTURE-LIVE-DATA-PATH: arming sprite 0 (SPR0POS=0x40, "
            "SPR0CTL=0x00, SPR0DATB=0x0000) three times from a fresh reset, "
            "each time with a single distinct 1-bit SPR0DATA word (0x8000, "
            "0x0080, 0x0001 -- bit 15, bit 7, bit 0) and sweeping hpos "
            "0..511, makes exactly one sprdata sample go non-zero per run, "
            "at three distinct hpos values that are monotonic in the same "
            "order as the three bit positions -- proving the write -> arm "
            "-> hpos-sweep -> read-sprdata measurement basis every "
            "assertion below shares actually delivers distinct fetched "
            "values, in order, at the cycle each was fetched on, on THIS "
            "package's own build. Fixture check, no claim id, no page. "
            "MSB hpos=" + std::to_string(fa_msb.hpos) +
            " mid hpos=" + std::to_string(fa_mid.hpos) +
            " LSB hpos=" + std::to_string(fa_lsb.hpos) + ".");
    }

    // ------- CODE-00-SELECTS-TRANSPARENT-UNATTACHED-OCS -------
    {
        const struct { const char* label; SpriteRegs r; } pairs[4] = {
            {"sprite 0 (pair 0/1)", SPR0},
            {"sprite 2 (pair 2/3)", SPR2},
            {"sprite 4 (pair 4/5)", SPR4},
            {"sprite 6 (pair 6/7)", SPR6},
        };
        bool all_live = true;
        bool all_transparent = true;
        std::string detail;
        for (int i = 0; i < 4; ++i) {
            Meas live = measure_single(pairs[i].r, /*pos=*/0x40, /*data=*/0x8000);
            FirstActive fa = first_active(live);
            if (fa.hpos == -1) all_live = false;
#if MUT_PAIR4
            uint16_t zero_data = (i == 2) ? 0x8000 : 0x0000;  // MUTATION: sprite 4 (index 2) no longer 00.
#else
            uint16_t zero_data = 0x0000;
#endif
            Meas zero = measure_single(pairs[i].r, /*pos=*/0x40, zero_data);
            bool zero_ok = all_zero(zero);
            if (!zero_ok) all_transparent = false;
            detail += std::string(pairs[i].label) + " live=" + (fa.hpos != -1 ? "yes" : "no") +
                      " code00-transparent=" + (zero_ok ? "yes" : "no") + "; ";
        }
        checks.expect(all_live && all_transparent,
            "CODE-00-SELECTS-TRANSPARENT-UNATTACHED-OCS: for each of the "
            "four representative sprites 0, 2, 4, 6 (one per Table 4-6 "
            "pair: 0/1, 2/3, 4/5, 6/7), armed alone and unattached at the "
            "same position (SPRxPOS=0x40, SPRxCTL=0x00): a single-1-bit "
            "stimulus (SPRxDATB=0x0000, SPRxDATA=0x8000) is first required "
            "to produce a non-zero sprdata sample somewhere in the hpos "
            "0..511 sweep -- the precondition proving this sprite's own "
            "shift/output path is active in this configuration, without "
            "which an all-zero result below could not be told apart from "
            "the sprite simply being off; then, with both SPRxDATB and "
            "SPRxDATA held at 0x0000 (combined 2-bit code 00 at every "
            "pixel), sprdata is required to stay 0 across the entire "
            "sweep. 'Not used *' for every pair -- '0 or 1  00  Not used "
            "*' (CH4-p130.c1/CH4-tab46s.c1), '2 or 3  00  Not used *' "
            "(CH4-p130.c5/CH4-tab46s.c5), '4 or 5  00  Not used *' "
            "(CH4-p130.c9/CH4-tab46s.c9), '6 or 7  00  Not used *' "
            "(CH4-p130.c13/CH4-tab46s.c13) -- and the table's own "
            "footnote, '* Selects transparent mode.' (CH4-p130.c17/"
            "CH4-tab46s.c17), read together: code 00 is the value that "
            "means transparency, for every unattached sprite pair, not a "
            "register selection. " + detail);
    }

    // ------- CODE-0000-SELECTS-TRANSPARENT-ATTACHED-OCS -------
    {
        Meas live = measure_pair_attached(/*pos=*/0x40, /*spr0data=*/0x8000,
                                           /*spr1data=*/0x8000);
        FirstActive fa_live = first_active(live);
        bool attached_live = fa_live.hpos != -1;
#if MUT_ATTACH0000
        Meas zero = measure_pair_attached(/*pos=*/0x40, /*spr0data=*/0x8000,
                                           /*spr1data=*/0x0000);  // MUTATION: SPR1 no longer 0000.
#else
        Meas zero = measure_pair_attached(/*pos=*/0x40, /*spr0data=*/0x0000,
                                           /*spr1data=*/0x0000);
#endif
        bool zero_ok = all_zero(zero);
        checks.expect(attached_live && zero_ok,
            "CODE-0000-SELECTS-TRANSPARENT-ATTACHED-OCS: with sprite 0 and "
            "sprite 1 attached (SPR1CTL bit 7 set -- the odd sprite's own "
            "ATTACH bit, the mechanism hrm/ch4-attached-sprites/ already "
            "proves live; reused here as plumbing, not re-asserted) at the "
            "same position (SPRxPOS=0x40): a non-degenerate stimulus (both "
            "sprites' DATA/DATB carrying bit 15) is first required to "
            "produce a non-zero sprdata sample -- proving the ATTACHED "
            "configuration's own shift/output path is active, the "
            "precondition a green result below depends on; then, with all "
            "four words (SPR0DATA, SPR0DATB, SPR1DATA, SPR1DATB) held at "
            "0x0000 -- combined 4-bit code 0000 throughout -- sprdata is "
            "required to stay 0 across the entire hpos 0..511 sweep, "
            "matching Table 4-7's own row: '0000  Selects transparent "
            "mode' (CH4-p131.c1, CH4-tab47.c1, p.131) -- the OCS attached-"
            "sprite table's own transparency code, distinct from Table "
            "4-5's own page-122 row 0 conflict (hrm/ch4-attached-sprites/'s "
            "own TABLE45-ROW0-REGISTER-NUMBER-CONFLICT, a different page, "
            "not re-litigated here).");
    }

    // --- SPRITE-DATA-WORDS-COMBINE-TO-SELECT-COLOUR ---
    {
        Meas runA = measure_single(SPR0, /*pos=*/0x40, /*data=*/0x8000, /*datb=*/0x0000);
#if MUT_COMBINE
        Meas runB = measure_single(SPR0, /*pos=*/0x40, /*data=*/0x8000, /*datb=*/0x0000);  // MUTATION: same as run A.
#else
        Meas runB = measure_single(SPR0, /*pos=*/0x40, /*data=*/0x0000, /*datb=*/0x8000);
#endif
        Meas runC = measure_single(SPR0, /*pos=*/0x40, /*data=*/0x8000, /*datb=*/0x8000);
        FirstActive fa = first_active(runA);
        FirstActive fb = first_active(runB);
        FirstActive fc = first_active(runC);
        bool each_single = fa.active_count == 1 && fb.active_count == 1 && fc.active_count == 1;
        bool same_hpos = fa.hpos != -1 && fa.hpos == fb.hpos && fb.hpos == fc.hpos;
        bool all_nonzero = fa.value != 0 && fb.value != 0 && fc.value != 0;
        bool pairwise_distinct = fa.value != fb.value && fb.value != fc.value &&
                                  fa.value != fc.value;
        checks.expect(each_single && same_hpos && all_nonzero && pairwise_distinct,
            "SPRITE-DATA-WORDS-COMBINE-TO-SELECT-COLOUR: an unattached "
            "sprite 0 at SPR0POS=0x40, armed three separate times (fresh "
            "reset each time) with bit 15 the only bit written anywhere, "
            "in three combinations of which word carries it -- DATA alone "
            "(DATB=0x0000/DATA=0x8000), DATB alone (DATB=0x8000/"
            "DATA=0x0000), and both (DATB=0x8000/DATA=0x8000) -- produces "
            "exactly one active sprdata sample per run, at the SAME hpos "
            "across all three (bit position sets shift-out timing "
            "independent of which word it came from), and the three "
            "sprdata VALUES at that shared hpos are each non-zero and "
            "pairwise distinct -- proving the selected colour is a "
            "function of BOTH data words' own bit, not of merely 'was any "
            "bit set' (which would make all three identical) and not of "
            "only one word (which would make two of the three identical). "
            "No specific register number is checked anywhere in this "
            "measurement -- only distinctness -- matching 'Sprite  data "
            "words  are used to select the color of the sprite pixels "
            "from' [the colour register set] (CH4-sum.c1, p.130; quote "
            "truncated in the claims.jsonl record itself, quoted here "
            "exactly as extracted). runA(DATA-only)=" + std::to_string((int)fa.value) +
            " runB(DATB-only)=" + std::to_string((int)fb.value) +
            " runC(both)=" + std::to_string((int)fc.value) + " at hpos=" +
            std::to_string(fa.hpos) + ".");
        checks.note("NOTE (DATA/DATB ORDER, not graded): this package's own "
            "assigned claims file names no page for which word (DATA or "
            "DATB) is the more significant colour bit -- the build task's "
            "own dispatch note names that dispute (p107/p121 'high-order "
            "word' vs p126 'the first fetched word... to SPRxDATA') but "
            "none of those pages occur in this package's own 67 records "
            "(see CLAIM SOURCE above). The relative numeric order of "
            "runA(DATA-only) vs runB(DATB-only) above is measured and "
            "reported here as a raw RTL observation, not graded against "
            "either side of that dispute and not treated as this "
            "package's own finding -- esprm/osprm are held at a fixed 0 "
            "in this harness (see ORDER OF WORK above), which does not "
            "correspond to any manual-documented base, so the two raw "
            "sprdata values are not colour-register numbers either.");
    }

    // --------- OUT-OF-SCOPE-REGISTER-MAPPING [declined] ---------
    // No checks.expect() and no MUTATION record: these 54 ids state
    // WHICH register number a code maps to, the sibling package
    // ch4-sprite-colour-registers' own subject (see CLAIM SOURCE
    // above), not this package's own selection-mechanism boundary.
    checks.record(
        "OUT-OF-SCOPE-REGISTER-MAPPING [declined, 54 ids]: the following "
        "records in this package's own claims.jsonl state which SPECIFIC "
        "colour register number a combined code selects (e.g. 'selector "
        "01 selects color register 17'), one row per non-00/non-0000 code "
        "value per sprite pair/nibble -- the register-NUMBER mapping, not "
        "the selection MECHANISM (how the bits combine, and what a code "
        "means structurally) this package was assigned. Per the build "
        "task's direct instruction these belong to the sibling package "
        "ch4-sprite-colour-registers, built concurrently on this same "
        "fixture from the 303-claim ch4-sprite-colour-encoding.jsonl, and "
        "are recorded here, not asserted twice: Table 4-6 (p.130, "
        "unattached, 2-bit code) -- CH4-p130.c2, CH4-p130.c3, CH4-p130.c4, "
        "CH4-p130.c6, CH4-p130.c7, CH4-p130.c8, CH4-p130.c10, "
        "CH4-p130.c11, CH4-p130.c12, CH4-p130.c14, CH4-p130.c15, "
        "CH4-p130.c16, CH4-tab46s.c2, CH4-tab46s.c3, CH4-tab46s.c4, "
        "CH4-tab46s.c6, CH4-tab46s.c7, CH4-tab46s.c8, CH4-tab46s.c10, "
        "CH4-tab46s.c11, CH4-tab46s.c12, CH4-tab46s.c14, CH4-tab46s.c15, "
        "CH4-tab46s.c16 (24 ids). Table 4-7 (p.131, attached, 4-bit "
        "code) -- CH4-p131.c2, CH4-p131.c3, CH4-p131.c4, CH4-p131.c5, "
        "CH4-p131.c6, CH4-p131.c7, CH4-p131.c8, CH4-p131.c9, "
        "CH4-p131.c10, CH4-p131.c11, CH4-p131.c12, CH4-p131.c13, "
        "CH4-p131.c14, CH4-p131.c15, CH4-p131.c16, CH4-tab47.c2, "
        "CH4-tab47.c3, CH4-tab47.c4, CH4-tab47.c5, CH4-tab47.c6, "
        "CH4-tab47.c7, CH4-tab47.c8, CH4-tab47.c9, CH4-tab47.c10, "
        "CH4-tab47.c11, CH4-tab47.c12, CH4-tab47.c13, CH4-tab47.c14, "
        "CH4-tab47.c15, CH4-tab47.c16 (30 ids). 24+30 = 54.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
