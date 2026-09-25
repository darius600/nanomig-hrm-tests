// ch4-sprite-colour-registers -- denise_sprites conformance test
//
// Test purpose: how a sprite pixel's 2-bit (unattached) or 4-bit (attached)
//   binary selector picks one of the 16 sprite colour registers (system
//   colour registers 16-31), the four-register group per sprite pair, the
//   transparent code (00 / 0000), and the "code counts up with increasing
//   register number" ordering within a group -- the subject of Figure 4-6
//   and Tables 4-2, 4-3, 4-5 and 4-6. Package ch4-sprite-colour-registers,
//   fixture denise_sprites (one Verilator build, denise_sprites.v +
//   denise_sprites_shifter.v, top module denise_sprites -- the same fixture
//   hrm/denise_sprites/, hrm/ch3-sprite-registers/ and
//   hrm/ch4-attached-sprites/ already build).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 97, 98, 101, 102, 103, 107, 108, 110, 114, 115, 116, 118, 119, 120,
//   121, 122, 124, 128.
//
// CLAIM SOURCE: this package's assigned claims file is
//   hrm-testpipeline/work/ch4_packages/
//   ch4-sprite-colour-encoding.jsonl (303 records, the largest in this
//   chapter). Five ids were named by the build task as already covered by
//   hrm/ch4-attached-sprites/ (built the same night): CH4-attach.c25,
//   CH4-attach.c26, CH4-attach.c27, CH4-p128.c8, CH4-sctl.c4. Checked
//   against this package's own 303-record file directly (grep, not
//   assumed): CH4-attach.c25/c26/c27 ARE in this file and are excluded
//   below, not re-asserted. CH4-p128.c8 and CH4-sctl.c4 are NOT in this
//   file at all -- hrm/ch4-attached-sprites/'s own header says they come
//   from a different package's inventory (ch4-sprite-shifter-and-
//   arming.jsonl). Recorded here rather than assumed, per this suite's own
//   "erst pruefen, dann behaupten" discipline.
//
// A NOTE ON HOW THIS FILE WAS BUILT, IN THE INTEREST OF NOT PAPERING OVER
//   IT: before this package's own claim-grouping pass began, one `head -c
//   2000` preview of the raw 303-record .jsonl (taken to learn the file's
//   JSON schema, before any per-record reading) printed the FIRST record's
//   `classify.evidence` field along with the fields this package actually
//   uses. That field is pipeline-internal, RTL-derived text this package's
//   own build brief says must not shape the specification; it is not
//   quoted or relied on anywhere below. It named denise_sprites.v's
//   sprdata output "the colour-table index" and mentioned hex values
//   $10-$1f for the four sprite pairs. Concretely, what this might have
//   biased is exactly the question this file spent the most design effort
//   avoiding: whether sprdata's raw numeric value equals the absolute
//   system colour-register number (16-31). Every assertion below was
//   deliberately written to NOT depend on that question -- see MEASUREMENT
//   PHILOSOPHY below -- so that even a fully contaminated belief about
//   sprdata's encoding could not have entered an expected value. Where the
//   two are in tension the manual quotations were followed. Reported
//   rather than concealed.
//
// Non-goals: sprite-to-sprite and sprite-to-playfield VIDEO PRIORITY (which
//   sprite is drawn in front when several overlap) is not tested --
//   denise_sprites' disclosed port list (reused from the sibling packages
//   below, not read from the RTL by this agent for this package) carries no
//   priority-resolution or collision output, only sprdata; testing priority
//   would need a wider module boundary (denise_spritepriority /
//   denise_collision) than the one assigned here. Sprite SIZE, PIXEL
//   RESOLUTION and POSITION independence from playfield mode, and the pure
//   WIDTH-only effect of overlapping/placing sprites side by side, are
//   likewise not colour-register propositions and are declined below, not
//   silently dropped. Table 4-4's own 64 worked-example pixel values (the
//   "spaceship sprite" example, p.121) are a worked example, not a
//   requirement, and per this package's own classify pass none of them
//   reached this package's claims file to begin with -- confirmed by direct
//   grep of the 303-record file, not assumed; there is nothing to decline
//   for that reason among this package's own ids.
// Claim ids: ids such as `CH4-p103.c1` and `CH4-p103.c2` name one sentence of
//   the manual in this chapter's reading. They carry no reader prefix, and
//   they resolve in the claim files that hrm-testpipeline/acceptance/ch4.json
//   lists under `inventories`.
//
// BOUNDARY: denise_sprites.v / denise_sprites_shifter.v, reached at
//   $(NANOMIG)/src/minimig-aga/. Port list, the word-address register map
//   (SPR0POS=0xA0 .. SPR1DATB=0xA7, "+4 words per sprite" for every
//   further sprite -- that exact phrase is hrm/ch3-sprite-registers/'s own
//   already-committed wording), the four-phase 7MHz clocking, the
//   DATB-then-DATA arm-on-write convention, and the SPR1CTL bit 7 (ATTACH)
//   mechanism are NOT read from denise_sprites.v/denise_sprites_shifter.v
//   by this agent for this package -- they are reused verbatim from THREE
//   already-committed sibling packages built against this exact fixture:
//   hrm/denise_sprites/test_denise_sprites.cpp, hrm/ch3-sprite-registers/
//   test_denise_sprites.cpp (whose own header discloses the "+4 words per
//   sprite" extrapolation this file extends to sprite channel 2), and
//   hrm/ch4-attached-sprites/test_denise_sprites.cpp (whose own header
//   discloses the ATTACH bit convention and the measure_pair() stimulus
//   shape this file's own attached-pair helper reuses). This is the same
//   precedent hrm/ch4-attached-sprites/ itself relied on and documented.
//   denise_sprites.v and denise_sprites_shifter.v themselves were not
//   opened by this agent until after the specification below was
//   committed.
//
// ORDER OF WORK: every assertion below -- its expected relationship, the
//   claim ids it cites, and the stimulus each measurement uses -- was
//   written and committed from claims.jsonl's quotations (the `extract`
//   field: page + verbatim quote + restatement) and the three
//   already-committed sibling harnesses' disclosed interface ONLY, per the
//   note above about the one incidental classify.evidence exposure.
//
// MEASUREMENT PHILOSOPHY: the manual states WHICH system colour registers
//   (16-31) exist and how many sprites share them, but says nothing about
//   how denise_sprites' own sprdata OUTPUT PORT numerically encodes "which
//   register" -- whether as the absolute register number, a per-pair
//   local code, or something else is an RTL representation choice the
//   manual does not make and this package must not guess at as an
//   expected VALUE (that would be exactly the "expected value starts life
//   as a fact about the implementation" failure this suite's build brief
//   warns against). So every assertion below is written as a RELATIONSHIP
//   the manual DOES state unambiguously -- zero vs non-zero (transparent
//   vs visible), equal vs distinct (this code vs that code, this pair vs
//   that pair), and ordered vs not (Figure 4-6's own stated "code counts
//   up with increasing register number", and "the highest numbered sprite
//   contributes the highest order bits") -- never a hard-coded absolute
//   sprdata number. A red result under this design is a claim about
//   denise_sprites' encoding relationships, not an artefact of guessing
//   its raw units.
//
// CLAIM COVERAGE -- 303 records in this package's own claims file; 3
//   excluded (already covered elsewhere), 227 asserted across 7
//   propositions (cited in full inside each expect()'s label, see the ids
//   listed with each block below), 73 declined with reason (8 groups).
//   227 + 3 + 73 = 303.
//
//   TRANSPARENT-CODE-IS-INVISIBLE-NONZERO-CODE-IS-VISIBLE (40 ids)
//   NONZERO-CODES-ORDER-MONOTONICALLY-WITHIN-A-GROUP (67 ids)
//   DIFFERENT-PAIRS-SELECT-DIFFERENT-REGISTER-GROUPS (19 ids)
//   HIGH-AND-LOW-ORDER-WORDS-BOTH-CONTRIBUTE-TO-THE-CODE (21 ids)
//   ATTACHED-PAIR-BOTH-SPRITES-CONTRIBUTE-DISTINCT-BITS (45 ids)
//   ATTACHED-ODD-SPRITE-CONTRIBUTES-HIGHER-ORDER-BITS (34 ids)
//   TABLE44-PIXEL-15-IS-LEFTMOST-PIXEL-0-IS-RIGHTMOST (1 id)
//   DECLINED, out of this package's colour-register subject or budget (73
//   ids, 8 groups: position/resolution independence, sprite size/pixel
//   resolution independence, sprite-to-sprite video priority, pure-width
//   overlap/placement, shift-rate/pixel-timing, generic playfield-
//   independence restatements, duplicate-of-sibling-package ATTACH-odd-
//   only restatement, and attached-pair reversion-on-separation).
//
// FIGURE 4-6'S BOXED NOTE, id CH4OM-fig46.note (p.103, not part of the
//   303-record count above -- a separate claims record): "Codes 01, 10 or
//   11 select one of three possible registers from the normal color
//   register group, from which the actual color data is taken." -- the
//   note in the box at the left of Figure 4-6, carried by the PRINTED
//   edition; the Developer CD rendering of the same figure, reproduced
//   elsewhere on this page as an ASCII table, carries no such box. The
//   three codes it names are this package's own three non-transparent
//   2-bit sprite pixel codes; the proposition is that each of the three
//   selects a DIFFERENT one of three registers within the sprite pair's
//   own colour-register group -- "one of three possible registers", read
//   the other way, is a distinctness/cardinality statement about exactly
//   those three codes. Whether an already-committed assertion establishes
//   the whole of this, or a new one is needed, is decided against the
//   full claims file's routing and the RTL, not here.
//
// CONTROL CASE: FIXTURE-LIVE-DATA-PATH [control] proves the write -> arm
//   -> hpos-sweep -> read-sprdata measurement basis every other assertion
//   shares actually delivers distinct fetched values, in order, before any
//   claim-based assertion depends on it (reused, same shape, from
//   hrm/ch4-attached-sprites/'s own control of the same name).
//
// RESULTS (WSL/Verilator 5.052, built and run 12.09.2026, AFTER the
//   specification commit -- denise_sprites.v and denise_sprites_shifter.v
//   were opened only once, for this build, not before): 8 passed, 0
//   failed. All 8 assertions (the control and all 7 propositions) are
//   GREEN -- on every relationship tested (transparent-vs-visible,
//   code ordering, pair distinctness, word significance, attached-pair
//   bit independence, attached odd-sprite higher order, and pixel-15-is-
//   leftmost), denise_sprites.v/denise_sprites_shifter.v match the
//   manual. No red findings from this package. Measured values: code00
//   sprdata=0, code01=1, code10=2, code11=3 (sprite 0); sprite 2 at code
//   01=5 (distinct from sprite 0's 1); DATB-only=2, DATA-only=1
//   (distinct); attached baseline=0, SPR0-alone code01=1, SPR1-alone
//   code01=4 (4 > 1, confirming odd = higher order); MSB-bit pixel at
//   hpos=130, LSB-bit pixel at hpos=145 (130 < 145, confirming pixel 15
//   is leftmost).
//
// MUTATION LOG (WSL/Verilator, same session as RESULTS above; each entry
//   actually run: rebuild with the named -CFLAGS -D flag -> run -> read
//   the PASS/FAIL lines -> confirm only the named assertion changed
//   colour -> rebuild with no flags -> confirm the 8-passed/0-failed
//   baseline was restored):
//
// MUTATION FIXTURE-LIVE-DATA-PATH stimulus red: built with -CFLAGS -DMUT_FIXTURE=1, making the MSB-run stimulus 0x0001 instead of 0x8000 (identical to the LSB-run) -> FAIL (7 passed, 1 failed), only FIXTURE-LIVE-DATA-PATH newly red (MSB hpos=145 became equal to LSB hpos=145). Reverted: 8 passed, 0 failed.
// MUTATION TRANSPARENT-CODE-IS-INVISIBLE-NONZERO-CODE-IS-VISIBLE stimulus red: built with -CFLAGS -DMUT_A=1, making the code-00 stimulus non-zero (SPR0DATA=0xFFFF, same pattern as code01) -> FAIL (7 passed, 1 failed), only this assertion newly red (the "stays invisible" half failed). Reverted: 8 passed, 0 failed.
// MUTATION NONZERO-CODES-ORDER-MONOTONICALLY-WITHIN-A-GROUP stimulus red: built with -CFLAGS -DMUT_C=1, swapping the code01 and code11 stimulus patterns -> FAIL (7 passed, 1 failed), only this assertion newly red (v01=3, v10=2, v11=1 -- ordering reversed, no longer monotonic). Reverted: 8 passed, 0 failed.
// MUTATION DIFFERENT-PAIRS-SELECT-DIFFERENT-REGISTER-GROUPS stimulus red: built with -CFLAGS -DMUT_D=1, measuring "pair 1" from SPR0 again instead of SPR2 -> FAIL (7 passed, 1 failed), only this assertion newly red (v_pair0 == v_pair1_mut, both 1, no longer distinct). Reverted: 8 passed, 0 failed.
// MUTATION HIGH-AND-LOW-ORDER-WORDS-BOTH-CONTRIBUTE-TO-THE-CODE stimulus red: built with -CFLAGS -DMUT_E=1, measuring the "DATB-only" case with the DATA-only (code01) stimulus instead -> FAIL (7 passed, 1 failed), only this assertion newly red (v_datb_only == v_data_only). Reverted: 8 passed, 0 failed.
// MUTATION ATTACHED-PAIR-BOTH-SPRITES-CONTRIBUTE-DISTINCT-BITS stimulus red: built with -CFLAGS -DMUT_F1=1, withholding SPR0's own stimulus in the "even only" case (SPR0 held at code 00, identical to baseline) -> FAIL (7 passed, 1 failed), only this assertion newly red (v_even_only == 0, fails the "!= 0" half); ATTACHED-ODD-SPRITE-CONTRIBUTES-HIGHER-ORDER-BITS stayed green (v_odd_only=4 > 0 still holds), confirming the mutation is isolated to this one assertion. Reverted: 8 passed, 0 failed. CORRECTION, recorded rather than dropped: this mutation's first design instead withheld SPR1's own stimulus in the "odd only" case, which zeroed v_odd_only and made BOTH this assertion and ATTACHED-ODD-SPRITE-CONTRIBUTES-HIGHER-ORDER-BITS go red at once, since the latter's own v_odd_only > v_even_only depends on the same measurement; replaced by the SPR0-side version above so each assertion's own mutation isolates to itself.
// MUTATION ATTACHED-ODD-SPRITE-CONTRIBUTES-HIGHER-ORDER-BITS stimulus red: built with -CFLAGS -DMUT_F2=1, swapping which sprite's measurement is labelled "even only" vs "odd only" -> FAIL (7 passed, 1 failed), only this assertion newly red (the mislabelled odd/even values reverse the comparison). Reverted: 8 passed, 0 failed.
// MUTATION TABLE44-PIXEL-15-IS-LEFTMOST-PIXEL-0-IS-RIGHTMOST stimulus red: built with -CFLAGS -DMUT_G=1, swapping the bit-15 and bit-0 stimulus words -> FAIL (7 passed, 1 failed), only this assertion newly red (hpos_msb no longer smaller than hpos_lsb). Reverted: 8 passed, 0 failed.

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vdenise_sprites.h"

// MUT_A: replay TRANSPARENT-CODE-IS-INVISIBLE-NONZERO-CODE-IS-VISIBLE's own
// mutation (-DMUT_A=1). Makes the code-00 stimulus non-zero (same as code01).
#ifndef MUT_A
#define MUT_A 0
#endif
// MUT_C: replay NONZERO-CODES-ORDER-MONOTONICALLY-WITHIN-A-GROUP's own
// mutation (-DMUT_C=1). Swaps which stimulus pattern feeds v01 and v11.
#ifndef MUT_C
#define MUT_C 0
#endif
// MUT_D: replay DIFFERENT-PAIRS-SELECT-DIFFERENT-REGISTER-GROUPS's own
// mutation (-DMUT_D=1). Measures "pair 1" from SPR0 again instead of SPR2.
#ifndef MUT_D
#define MUT_D 0
#endif
// MUT_E: replay HIGH-AND-LOW-ORDER-WORDS-BOTH-CONTRIBUTE-TO-THE-CODE's own
// mutation (-DMUT_E=1). Measures the "DATB-only" case with the DATA-only
// stimulus instead.
#ifndef MUT_E
#define MUT_E 0
#endif
// MUT_F1: replay ATTACHED-PAIR-BOTH-SPRITES-CONTRIBUTE-DISTINCT-BITS's own
// mutation (-DMUT_F1=1). Withholds SPR0's own stimulus in the "even only"
// case (measured with SPR0 held at code 00, same as baseline).
#ifndef MUT_F1
#define MUT_F1 0
#endif
// MUT_F2: replay ATTACHED-ODD-SPRITE-CONTRIBUTES-HIGHER-ORDER-BITS's own
// mutation (-DMUT_F2=1). Swaps which sprite's measurement is labelled
// "even only" vs "odd only".
#ifndef MUT_F2
#define MUT_F2 0
#endif
// MUT_G: replay TABLE44-PIXEL-15-IS-LEFTMOST-PIXEL-0-IS-RIGHTMOST's own
// mutation (-DMUT_G=1). Swaps the bit-15 and bit-0 stimulus words.
#ifndef MUT_G
#define MUT_G 0
#endif
// MUT_FIXTURE: replay FIXTURE-LIVE-DATA-PATH's own mutation (-DMUT_FIXTURE=1).
// Makes the MSB-run stimulus identical to the LSB-run.
#ifndef MUT_FIXTURE
#define MUT_FIXTURE 0
#endif

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    // For records of a declined claim, or what the source says. Never a
    // measurement result.
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Word-address register map. SPR0*/SPR1* reused verbatim from
// hrm/denise_sprites/ and hrm/ch3-sprite-registers/'s own already-committed
// test_denise_sprites.cpp for this same module. SPR2* is this file's own
// extrapolation of hrm/ch3-sprite-registers/'s own disclosed "+4 words per
// sprite" pattern (its header comment's own words) -- SPR2POS = SPR0POS +
// 2*4 = 0xA8 -- applied to reach a SECOND sprite pair (channel 2, the first
// half of the "sprites 2 and 3" pair) for DIFFERENT-PAIRS-SELECT-DIFFERENT-
// REGISTER-GROUPS below. This is arithmetic on an already-disclosed,
// already-committed pattern, not a fact read from this package's own RTL.
constexpr int SPR0POS = 0xA0;
constexpr int SPR0CTL = 0xA1;
constexpr int SPR0DATA = 0xA2;
constexpr int SPR0DATB = 0xA3;
constexpr int SPR1POS = 0xA4;
constexpr int SPR1CTL = 0xA5;
constexpr int SPR1DATA = 0xA6;
constexpr int SPR1DATB = 0xA7;
constexpr int SPR2POS = 0xA8;
constexpr int SPR2CTL = 0xA9;
constexpr int SPR2DATA = 0xAA;
constexpr int SPR2DATB = 0xAB;
constexpr int IDLE_ADDR = 0x00;

constexpr int HPOS_MAX = 512;  // SH8..SH0 is a 9-bit value: 0..511 covers it.
constexpr uint8_t POS_BYTE = 0x40;  // Reused known-reachable value from the
                                     // sibling packages' own recipes.

struct Meas {
    std::vector<uint8_t> trace;  // sprdata at every swept hpos.
};

// Clocking (clk7_en/c1/c3 four-phase tick, reg_address_in/data_in write
// sequencing) reused verbatim from the sibling harnesses for this same
// module, not learned from this package's own RTL (see ORDER OF WORK
// above).
class Harness {
public:
    explicit Harness(uint8_t aga = 0, uint8_t spres = 0) : dut(new Vdenise_sprites) {
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

    Meas sweep() {
        Meas m;
        for (int hp = 0; hp < HPOS_MAX; ++hp) {
            dut->hpos = hp;
            tick_7mhz();
            m.trace.push_back(static_cast<uint8_t>(dut->sprdata));
        }
        return m;
    }

    Vdenise_sprites* dut;
    int phase = 0;
};

// Arms exactly one sprite (given its own POS/CTL/DATA/DATB word addresses)
// with a fixed DATB (high order, MSB-contributing per CH4-cdw.c4) and DATA
// (low order, LSB-contributing per CH4-cdw.c5) word, attach bit left clear,
// and sweeps the whole hpos range.
Meas measure_single(int pos_addr, int ctl_addr, int data_addr, int datb_addr,
                     uint16_t datb_word, uint16_t data_word) {
    Harness h;
    h.write_reg(pos_addr, POS_BYTE);
    h.write_reg(ctl_addr, 0x0000);
    h.write_reg(datb_addr, datb_word);
    h.write_reg(data_addr, data_word);
    return h.sweep();
}

// Arms the SPR0/SPR1 pair, with independently chosen DATB/DATA per sprite
// and independently chosen CTL bit 7 (ATTACH) per sprite -- shape reused
// from hrm/ch4-attached-sprites/'s own measure_pair(), whose own ATTACH
// mechanism (SPR1CTL bit 7 = 1) is already disclosed and already measured
// GREEN by that sibling package for "attachment takes effect".
Meas measure_pair(uint8_t ctl0_bit7, uint8_t ctl1_bit7,
                   uint16_t spr0_datb, uint16_t spr0_data,
                   uint16_t spr1_datb, uint16_t spr1_data) {
    Harness h;
    h.write_reg(SPR0POS, POS_BYTE);
    h.write_reg(SPR0CTL, static_cast<uint16_t>(ctl0_bit7 ? 0x80 : 0x00));
    h.write_reg(SPR0DATB, spr0_datb);
    h.write_reg(SPR0DATA, spr0_data);
    h.write_reg(SPR1POS, POS_BYTE);
    h.write_reg(SPR1CTL, static_cast<uint16_t>(ctl1_bit7 ? 0x80 : 0x00));
    h.write_reg(SPR1DATB, spr1_datb);
    h.write_reg(SPR1DATA, spr1_data);
    return h.sweep();
}

bool all_zero(const Meas& m) {
    for (uint8_t v : m.trace) if (v != 0) return false;
    return true;
}

uint8_t max_value(const Meas& m) {
    uint8_t mx = 0;
    for (uint8_t v : m.trace) if (v > mx) mx = v;
    return mx;
}

bool has_nonzero(const Meas& m) { return max_value(m) != 0; }

struct FirstActive {
    int hpos = -1;
    int active_count = 0;
};
FirstActive first_active(const Meas& m) {
    FirstActive r;
    for (size_t i = 0; i < m.trace.size(); ++i) {
        if (m.trace[i] != 0) {
            if (r.hpos == -1) r.hpos = static_cast<int>(i);
            ++r.active_count;
        }
    }
    return r;
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
        Meas msb = measure_single(SPR0POS, SPR0CTL, SPR0DATA, SPR0DATB, 0x0000, msb_word);
        Meas mid = measure_single(SPR0POS, SPR0CTL, SPR0DATA, SPR0DATB, 0x0000, 0x0080);
        Meas lsb = measure_single(SPR0POS, SPR0CTL, SPR0DATA, SPR0DATB, 0x0000, lsb_word);
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
            "at three distinct hpos values monotonic in the same order as "
            "the three bit positions -- proving the write -> arm -> "
            "hpos-sweep -> read-sprdata measurement basis every assertion "
            "below shares actually delivers distinct fetched values, in "
            "order, not a single stuck value. Fixture check, no claim id, "
            "no page. MSB hpos=" + std::to_string(fa_msb.hpos) +
            " mid hpos=" + std::to_string(fa_mid.hpos) +
            " LSB hpos=" + std::to_string(fa_lsb.hpos) + ".");
    }

    // Base measurements reused by several assertions below: the four
    // 2-bit codes on sprite 0 (pair "sprite 0 or 1"), each held constant
    // across the whole 16-bit DATB/DATA word so every one of the 16
    // pixels of the armed sprite carries the SAME code -- code = (DATB
    // bit << 1) | DATA bit, DATB the high order/leftmost-contributing
    // word (CH4-cdw.c4), DATA the low order/rightmost-contributing word
    // (CH4-cdw.c5).
    Meas code00 = measure_single(SPR0POS, SPR0CTL, SPR0DATA, SPR0DATB, 0x0000, 0x0000);
#if MUT_C
    // MUTATION (shared with NONZERO-CODES-ORDER-MONOTONICALLY-WITHIN-A-GROUP):
    // v01's own stimulus swapped for what should be v11's, and vice versa.
    Meas code01 = measure_single(SPR0POS, SPR0CTL, SPR0DATA, SPR0DATB, 0xFFFF, 0xFFFF);
    Meas code11 = measure_single(SPR0POS, SPR0CTL, SPR0DATA, SPR0DATB, 0x0000, 0xFFFF);
#else
    Meas code01 = measure_single(SPR0POS, SPR0CTL, SPR0DATA, SPR0DATB, 0x0000, 0xFFFF);
    Meas code11 = measure_single(SPR0POS, SPR0CTL, SPR0DATA, SPR0DATB, 0xFFFF, 0xFFFF);
#endif
    Meas code10 = measure_single(SPR0POS, SPR0CTL, SPR0DATA, SPR0DATB, 0xFFFF, 0x0000);
    uint8_t v00 = max_value(code00);
    uint8_t v01 = max_value(code01);
    uint8_t v10 = max_value(code10);
    uint8_t v11 = max_value(code11);

    // ------- TRANSPARENT-CODE-IS-INVISIBLE-NONZERO-CODE-IS-VISIBLE -------
    {
#if MUT_A
        // MUTATION: the code-00 stimulus is made non-zero (same pattern as
        // code01), so the "stays invisible" half must fail.
        Meas code00_mut = measure_single(SPR0POS, SPR0CTL, SPR0DATA, SPR0DATB, 0x0000, 0xFFFF);
        bool transparent_invisible = all_zero(code00_mut);
#else
        bool transparent_invisible = all_zero(code00);
#endif
        bool nonzero_visible = has_nonzero(code01);
        checks.expect(transparent_invisible && nonzero_visible,
            "TRANSPARENT-CODE-IS-INVISIBLE-NONZERO-CODE-IS-VISIBLE: sprite 0 "
            "armed with a constant code 00 across all 16 pixels (SPR0DATB="
            "0x0000, SPR0DATA=0x0000) shows no sprite pixel anywhere in the "
            "hpos 0..511 sweep, while the same sprite armed with a constant "
            "code 01 (SPR0DATB=0x0000, SPR0DATA=0xFFFF) shows a non-zero "
            "sprdata somewhere in the sweep -- this is the precondition "
            "several other propositions in this file rest on: it must be "
            "possible for transparency to actually occur before any "
            "assertion about what a NON-transparent code selects can mean "
            "anything. 'The color value of the first register in each "
            "group of four registers is ignored by sprites. When the "
            "sprite bits select this register, the \"transparent\" value "
            "is used.' (p.103); 'A pixel whose value is 00 becomes "
            "transparent and shows the color of any other sprite or "
            "playfield that has lower video priority.' (p.103); 'Codes 01, "
            "10 or 11 select one of three possible registers from the "
            "normal color register group, from which the actual color "
            "data is taken.' (Figure 4-6's own boxed note, p.103 -- "
            "confirmed printed; the note text, not a row label). Printed "
            "Figure 4-6 (hrm/ch4/figures/p103.png) itself numbers only "
            "registers 16, 20, 24 and 28 (each marked 'unused') and 31; "
            "the intervening rows (17-19, 21-23, 25-27, 29-30) carry no "
            "printed number or role name -- this assertion is therefore "
            "built from the boxed note and the surrounding prose, not from "
            "a Figure 4-6 row label the print does not carry. "
            "CH4-p103.c1, CH4-p103.c2, CH4-p103.c19, CH4-color.c9, "
            "CH4-color.c10, CH4-color.c11, CH4-color.c12, CH4-tab42.c1, "
            "CH4-tab42.c5, CH4-tab42.c6, CH4-p108.c7, CH4-p108.c11, "
            "CH4-fig4-6.c1, CH4-fig4-6.c2, CH4-fig4-6.c3, CH4-fig4-6.c4, "
            "CH4-p103.c3, CH4-p103.c7, CH4-p103.c11, CH4-p103.c15, "
            "CH4-tab46.c1, CH4-tab46.c2, CH4-tab46.c4, CH4-tab46.c13, "
            "CH4-tab46.c22, CH4-tab46.c31, CH4-color.c1, CH4-color.c2, "
            "CH4-p102.c2, CH4-p97.c6, CH4-p97.c7, CH4-intro.c9, "
            "CH4-intro.c10, CH4-forming.c4, CH4-p98.c4, "
            "CH4-fig46.f6, CH4-tab46.c3, CH4-tab46.c12, CH4-tab46.c21, "
            "CH4-tab46.c30 .");
        checks.note("code00 sprdata max=" + std::to_string(static_cast<int>(v00)) +
            " code01 sprdata max=" + std::to_string(static_cast<int>(v01)) + ".");
    }

    // ----- NONZERO-CODES-ORDER-MONOTONICALLY-WITHIN-A-GROUP -----
    {
        bool ordered = v11 > v10 && v10 > v01 && v01 > v00;
        checks.expect(ordered,
            "NONZERO-CODES-ORDER-MONOTONICALLY-WITHIN-A-GROUP: for sprite 0 "
            "(pair \"sprite 0 or 1\"), the constant-code sprdata readings "
            "are strictly increasing with the code value: code 00 < code "
            "01 < code 10 < code 11 (v00=" + std::to_string((int)v00) +
            " v01=" + std::to_string((int)v01) + " v10=" +
            std::to_string((int)v10) + " v11=" + std::to_string((int)v11) +
            "). This matches Figure 4-6's own printed structure -- 'the "
            "code column against each group of four registers is drawn as "
            "00, 01, 10, 11 in that top-to-bottom order, so within a group "
            "the code counts up with increasing register number' -- and "
            "Table 4-2 (p.108)'s explicit rows for sprite channel 0 (00 -> "
            "Transparent, 01 -> 17, 10 -> 18, 11 -> 19) and Table 4-3 "
            "(p.115)'s explicit per-pair ranges (17-19, 21-23, 25-27, "
            "29-31 -- three ascending registers per pair). This assertion "
            "tests ORDER, not the absolute register number sprdata "
            "carries, which the manual does not specify at the RTL signal "
            "level (see MEASUREMENT PHILOSOPHY above); it therefore also "
            "covers the twelve Figure-4-6-row claims below, whose "
            "restatement cites a printed row label Figure 4-6 itself does "
            "not carry for those rows (16, 20, 24, 28 and 31 are the only "
            "printed numbers) -- the ordering proposition those claims "
            "state is nonetheless sound, and is asserted here from the "
            "boxed note, Table 4-2 and Table 4-3, not from the disputed "
            "row label. CH4-fig46.f7, CH4-p108.c5, CH4-p108.c6, "
            "CH4-p108.c8, CH4-p108.c9, CH4-p108.c10, CH4-tab42.c2, "
            "CH4-tab42.c3, CH4-tab42.c4, CH4-colreg.c3, CH4-p103.c4, "
            "CH4-p103.c5, CH4-p103.c6, CH4-p103.c8, CH4-p103.c9, "
            "CH4-p103.c10, CH4-p103.c12, CH4-p103.c13, CH4-p103.c14, "
            "CH4-p103.c16, CH4-p103.c17, CH4-p103.c18, CH4-tab46.c5, "
            "CH4-tab46.c6, CH4-tab46.c7, CH4-tab46.c8, CH4-tab46.c10, "
            "CH4-tab46.c11, CH4-tab46.c14, CH4-tab46.c15, CH4-tab46.c16, "
            "CH4-tab46.c17, CH4-tab46.c19, CH4-tab46.c20, CH4-tab46.c23, "
            "CH4-tab46.c24, CH4-tab46.c25, CH4-tab46.c26, CH4-tab46.c28, "
            "CH4-tab46.c29, CH4-tab46.c32, CH4-tab46.c33, CH4-tab46.c34, "
            "CH4-tab46.c35, CH4-tab46.c37, CH4-tab46.c38, CH4-p115.c1, "
            "CH4-p115.c2, CH4-p115.c3, CH4-p115.c4, CH4-tab43.c1, "
            "CH4-tab43.c2, CH4-tab43.c3, CH4-tab43.c4, CH4-fig4-6.c5, "
            "CH4-fig4-6.c6, CH4-fig4-6.c7, CH4-fig4-6.c8, CH4-fig4-6.c9, "
            "CH4-fig4-6.c10, CH4-fig4-6.c11, CH4-fig4-6.c12, "
            "CH4-fig4-6.c13, CH4-fig4-6.c14, CH4-fig4-6.c15, "
            "CH4-fig4-6.c16, CH4-fig46.f1, CH4OM-fig46.note .");
    }

    // ------- DIFFERENT-PAIRS-SELECT-DIFFERENT-REGISTER-GROUPS -------
    {
        Meas pair0_code01 = code01;  // sprite 0, pair "sprite 0 or 1".
#if MUT_D
        // MUTATION: "pair 1" is measured from SPR0 again instead of SPR2.
        Meas pair1_code01 = measure_single(SPR0POS, SPR0CTL, SPR0DATA, SPR0DATB, 0x0000, 0xFFFF);
#else
        Meas pair1_code01 = measure_single(SPR2POS, SPR2CTL, SPR2DATA, SPR2DATB, 0x0000, 0xFFFF);
#endif
        uint8_t v_pair0 = max_value(pair0_code01);
        uint8_t v_pair1 = max_value(pair1_code01);
        checks.expect(v_pair0 != v_pair1,
            "DIFFERENT-PAIRS-SELECT-DIFFERENT-REGISTER-GROUPS: the SAME "
            "constant code 01 produces a DIFFERENT sprdata value when "
            "armed on sprite 0 (pair \"sprite 0 or 1\", v=" +
            std::to_string((int)v_pair0) + ") than when armed on sprite 2 "
            "(pair \"sprite 2 or 3\", v=" + std::to_string((int)v_pair1) +
            ") -- each pair has its OWN four-register group, not a shared "
            "one. 'This binary number points to one of the four color "
            "registers assigned to that particular sprite DMA channel. "
            "The eight sprites use system color registers 16 - 31.' "
            "(p.102); 'For purposes of color selection, the eight sprites "
            "are organized into pairs and each pair uses four of the color "
            "registers as shown in Figure 4-6.' (p.102); 'Also, recall "
            "that each pair of sprites takes its color from different "
            "color registers.' (p.114). CH4-color.c5, CH4-color.c6, "
            "CH4-color.c7, CH4-color.c8, CH4-p102.c4, CH4-p102.c5, "
            "CH4-p102.c6, CH4-colreg.c1, CH4-colreg.c2, CH4-fig46.f2, "
            "CH4-fig46.f3, CH4-fig46.f4, CH4-fig46.f5, CH4-fig46.f8, "
            "CH4-tab46.c9, CH4-tab46.c18, CH4-tab46.c27, CH4-tab46.c36, "
            "CH4-more.c11 .");
    }

    // ----- HIGH-AND-LOW-ORDER-WORDS-BOTH-CONTRIBUTE-TO-THE-CODE -----
    {
#if MUT_E
        // MUTATION: the "DATB-only" reading is replaced by the DATA-only
        // (code01) pattern, so it can no longer differ from v_data_only.
        Meas datb_only = measure_single(SPR0POS, SPR0CTL, SPR0DATA, SPR0DATB, 0x0000, 0xFFFF);
#else
        Meas datb_only = code10;  // DATB=0xFFFF, DATA=0x0000.
#endif
        Meas data_only = code01;  // DATB=0x0000, DATA=0xFFFF.
        uint8_t v_datb_only = max_value(datb_only);
        uint8_t v_data_only = max_value(data_only);
        checks.expect(v_datb_only != v_data_only && v_datb_only != 0 && v_data_only != 0,
            "HIGH-AND-LOW-ORDER-WORDS-BOTH-CONTRIBUTE-TO-THE-CODE: setting "
            "only the high order word's bit (SPR0DATB=0xFFFF, SPR0DATA="
            "0x0000, v=" + std::to_string((int)v_datb_only) +
            ") produces a DIFFERENT non-zero sprdata than setting only the "
            "low order word's bit (SPR0DATB=0x0000, SPR0DATA=0xFFFF, v=" +
            std::to_string((int)v_data_only) + ") -- both data words "
            "genuinely contribute their own bit to the pixel's binary "
            "colour selector, neither is ignored. 'The 0s and 1s in the "
            "two data words that define each line of a sprite in the data "
            "structure form a binary number.' (p.102); 'The bits in the "
            "high order color descriptor word contribute the leftmost "
            "digit of the binary color selector number for each pixel; "
            "the low order word contributes the rightmost digit.' (p.107). "
            "CH4-color.c3, CH4-color.c4, CH4-p102.c3, CH4-cdw.c1, "
            "CH4-cdw.c2, CH4-cdw.c4, CH4-cdw.c5, CH4-cdw.c6, CH4-p107.c13, "
            "CH4-p107.c14, CH4-p107.c16, CH4-p107.c17, CH4-fig4-5.c1, "
            "CH4-fig4-5.c2, CH4-fig4-5.c3, CH4-fig45.f1, CH4-fig45.f2, "
            "CH4-fig45.f3, CH4-fig45.f4, CH4-hw.c5, CH4-p124.c3 .");
    }

    // Shared attached-pair measurements for the two ATTACHED-* assertions
    // below. ATTACH is set on SPR1 (the odd sprite) only, matching
    // hrm/ch4-attached-sprites/'s own already-GREEN "odd bit enables
    // attachment" measurement -- reused, not re-derived.
    Meas attach_base = measure_pair(/*ctl0=*/0, /*ctl1=*/1,
                                     /*spr0*/ 0x0000, 0x0000, /*spr1*/ 0x0000, 0x0000);
#if MUT_F2
    // MUTATION (ATTACHED-ODD-SPRITE-CONTRIBUTES-HIGHER-ORDER-BITS): the
    // "even only" and "odd only" stimuli are swapped -- SPR0 now carries
    // what should be SPR1's own probe and vice versa.
    Meas attach_even_only = measure_pair(0, 1, 0x0000, 0x0000, 0x0000, 0xFFFF);
    Meas attach_odd_only = measure_pair(0, 1, 0x0000, 0xFFFF, 0x0000, 0x0000);
#elif MUT_F1
    // MUTATION (ATTACHED-PAIR-BOTH-SPRITES-CONTRIBUTE-DISTINCT-BITS): the
    // "even only" case withholds SPR0's own stimulus (SPR0 held at code
    // 00, identical to the baseline), isolating the break to this
    // assertion's own "v_even_only != 0" half -- v_odd_only is untouched,
    // so ATTACHED-ODD-SPRITE-CONTRIBUTES-HIGHER-ORDER-BITS's own
    // v_odd_only > v_even_only (now v_odd_only > 0) stays true/green.
    Meas attach_even_only = measure_pair(0, 1, 0x0000, 0x0000, 0x0000, 0x0000);
    Meas attach_odd_only = measure_pair(0, 1, 0x0000, 0x0000, 0x0000, 0xFFFF);
#else
    Meas attach_even_only = measure_pair(0, 1, 0x0000, 0xFFFF, 0x0000, 0x0000);
    Meas attach_odd_only = measure_pair(0, 1, 0x0000, 0x0000, 0x0000, 0xFFFF);
#endif
    uint8_t v_attach_base = max_value(attach_base);
    uint8_t v_even_only = max_value(attach_even_only);
    uint8_t v_odd_only = max_value(attach_odd_only);

    // ----- ATTACHED-PAIR-BOTH-SPRITES-CONTRIBUTE-DISTINCT-BITS -----
    {
        bool base_transparent = all_zero(attach_base);
        checks.expect(base_transparent && v_even_only != 0 && v_odd_only != 0 &&
                          v_even_only != v_odd_only,
            "ATTACHED-PAIR-BOTH-SPRITES-CONTRIBUTE-DISTINCT-BITS: with "
            "SPR0 and SPR1 attached (SPR1CTL bit 7 set), both held at code "
            "00 shows no pixel anywhere in the sweep (v=" +
            std::to_string((int)v_attach_base) +
            ", the combined-code-0000 transparent precondition this "
            "assertion and the next one both rest on); giving SPR0 alone "
            "a constant code 01 produces a non-zero sprdata (v=" +
            std::to_string((int)v_even_only) +
            ") DIFFERENT from giving SPR1 alone the same code 01 (v=" +
            std::to_string((int)v_odd_only) +
            ") -- each sprite of the attached pair contributes its OWN, "
            "distinct two bits to the combined colour selector, not a "
            "shared or ignored pair. 'You can create sprites that have "
            "fifteen possible color choices (plus transparent) instead of "
            "three (plus transparent), by \"attaching\" two sprites.' "
            "(p.120); 'Each sprite in the attached pair contributes two "
            "bits to the binary color selector number.' (p.120); 'The "
            "extra color choices are possible because each pixel contains "
            "four bits instead of only two as in the normal, unattached "
            "sprite.' (p.120). CH4-attach.c1, CH4-attach.c2, "
            "CH4-attach.c3, CH4-attach.c4, CH4-attach.c5, CH4-attach.c6, "
            "CH4-attach.c7, CH4-attach.c8, CH4-attach.c9, CH4-attach.c10, "
            "CH4-attach.c11, CH4-attach.c16, CH4-attach.c17, CH4-p120.c1, "
            "CH4-p120.c2, CH4-p120.c3, CH4-p120.c4, CH4-p120.c5, "
            "CH4-p120.c6, CH4-p120.c7, CH4-p120.c8, CH4-p120.c9, "
            "CH4-p121.c1, CH4-p121.c2, CH4-p121.c3, CH4-p121.c4, "
            "CH4-p121.c5, CH4-fig4-12.c1, CH4-fig4-12.c2, CH4-fig412.f1, "
            "CH4-p128.c10, CH4-p128.c11, CH4-p128.c12, CH4-p128.c13, "
            "CH4-sctl.c6, CH4-sctl.c7, CH4-sctl.c8, CH4-sctl.c9, "
            "CH4-sctl.c10, CH4-sctl.c11, CH4-sctl.c12, CH4-intro.c2, "
            "CH4-intro.c3, CH4-p97.c8, CH4-p97.c9 .");
    }

    // ----- ATTACHED-ODD-SPRITE-CONTRIBUTES-HIGHER-ORDER-BITS -----
    {
        checks.expect(v_odd_only > v_even_only,
            "ATTACHED-ODD-SPRITE-CONTRIBUTES-HIGHER-ORDER-BITS: measured "
            "on the same attached pair and the same single-code-01 probe "
            "as the assertion above, giving SPR1 (the ODD, higher-"
            "numbered sprite of the pair) the code alone yields a LARGER "
            "sprdata (v=" + std::to_string((int)v_odd_only) +
            ") than giving SPR0 (the EVEN, lower-numbered sprite) the "
            "identical code alone (v=" + std::to_string((int)v_even_only) +
            ") -- the odd sprite's own two bits carry more numeric weight "
            "in the combined selector, matching the manual's own stated "
            "bit order, not just \"contributes some bits\". 'The highest "
            "numbered sprite (number 1, in this example) contributes the "
            "highest order bits (leftmost) in the binary number.' (p.121); "
            "'The high order data word in each sprite contributes the "
            "leftmost digit.' (p.121). This assertion tests ORDER "
            "(greater-than), not the absolute combined-code or register "
            "number, per MEASUREMENT PHILOSOPHY above. CH4-attach.c19, "
            "CH4-attach.c20, CH4-p121.c70, CH4-p121.c71, CH4-p122.c2, "
            "CH4-p122.c3, CH4-p122.c4, CH4-p122.c5, CH4-p122.c6, "
            "CH4-p122.c7, CH4-p122.c8, CH4-p122.c9, CH4-p122.c10, "
            "CH4-p122.c11, CH4-p122.c12, CH4-p122.c13, CH4-p122.c14, "
            "CH4-p122.c15, CH4-p122.c16, CH4-tab45.c2, CH4-tab45.c3, "
            "CH4-tab45.c4, CH4-tab45.c5, CH4-tab45.c6, CH4-tab45.c7, "
            "CH4-tab45.c8, CH4-tab45.c9, CH4-tab45.c10, CH4-tab45.c11, "
            "CH4-tab45.c12, CH4-tab45.c13, CH4-tab45.c14, CH4-tab45.c15, "
            "CH4-tab45.c16 .");
    }

    // ------- TABLE44-PIXEL-15-IS-LEFTMOST-PIXEL-0-IS-RIGHTMOST -------
    {
#if MUT_G
        // MUTATION: the bit-15 and bit-0 stimulus words are swapped.
        Meas msb_run = measure_single(SPR0POS, SPR0CTL, SPR0DATA, SPR0DATB, 0x0000, 0x0001);
        Meas lsb_run = measure_single(SPR0POS, SPR0CTL, SPR0DATA, SPR0DATB, 0x0000, 0x8000);
#else
        Meas msb_run = measure_single(SPR0POS, SPR0CTL, SPR0DATA, SPR0DATB, 0x0000, 0x8000);
        Meas lsb_run = measure_single(SPR0POS, SPR0CTL, SPR0DATA, SPR0DATB, 0x0000, 0x0001);
#endif
        FirstActive fa_msb = first_active(msb_run);
        FirstActive fa_lsb = first_active(lsb_run);
        bool both_found = fa_msb.hpos != -1 && fa_lsb.hpos != -1;
        bool msb_first = both_found && fa_msb.hpos < fa_lsb.hpos;
        checks.expect(msb_first,
            "TABLE44-PIXEL-15-IS-LEFTMOST-PIXEL-0-IS-RIGHTMOST: arming "
            "sprite 0 with a single bit at DATA bit 15 (the word's MSB) "
            "shows its one active pixel at hpos=" + std::to_string(fa_msb.hpos) +
            ", earlier (smaller hpos, i.e. further left as the beam "
            "sweeps) than the single active pixel from a bit at DATA bit "
            "0 (the word's LSB), at hpos=" + std::to_string(fa_lsb.hpos) +
            " -- matching Table 4-4's own pixel numbering, '15  14  13  "
            "12  11  10   9   8   7   6   5   4   3   2   1   0', "
            "numbered left to right, so pixel 15 (the MSB) is the "
            "leftmost pixel of the line and pixel 0 (the LSB) is the "
            "rightmost. hpos is a horizontal beam-position counter that "
            "increases as the raster moves rightward, a basic property of "
            "the port's own stated role (reused from the sibling "
            "harnesses' own hpos sweeps), not a fact read from this "
            "package's own RTL. CH4-attach.c18 .");
    }

    // ===================== DECLINED, WITH REASON =====================
    // No checks.expect() and no MUTATION record for any of the eight
    // groups below: there is no assertion here for a stimulus to turn
    // red, matching hrm/ch4-attached-sprites/'s own convention for its
    // one declined claim.

    checks.record(
        "OUT-OF-SUBJECT-POSITION [declined]: sprite X/Y position being "
        "defined independently of the playfield's resolution/interlace "
        "mode is a POSITION proposition, not a colour-register-selection "
        "one -- outside this package's assigned subject (a dedicated "
        "sibling package, hrm/ch4-sprite-horizontal-position/, exists for "
        "sprite position). Not measured here. CH4-pos.c4, CH4-pos.c5, "
        "CH4-p98.c9 .");

    checks.record(
        "OUT-OF-SUBJECT-SIZE-RESOLUTION [declined]: sprite pixel size/"
        "resolution being independent of the playfield's own resolution "
        "or interlace mode (and the Figure 4-4 spaceship shape example's "
        "own transparent-cell illustration) are SIZE/SHAPE propositions, "
        "not colour-register-selection ones -- outside this package's "
        "assigned subject. Not measured here. CH4-p101.c7, CH4-p101.c8, "
        "CH4-p101.c9, CH4-shape.c3, CH4-size.c6, CH4-size.c7, "
        "CH4-size.c8, CH4-size.c9, CH4-p101.c11 .");

    checks.record(
        "OUT-OF-SUBJECT-VIDEO-PRIORITY [declined]: sprite-to-sprite video "
        "priority (which sprite is drawn in front when several overlap) "
        "is a PRIORITY/ARBITRATION proposition. denise_sprites' disclosed "
        "port list (reused from the sibling packages, not read from the "
        "RTL for this package) carries only sprdata, no priority-"
        "resolution or collision output -- testing this would need a "
        "wider module boundary (denise_spritepriority / denise_collision) "
        "than the one this package was assigned. Declined per this "
        "suite's own HALT-on-wider-boundary guidance, not silently "
        "dropped. CH4-p103.c21, CH4-prio.c2, CH4-prio.c4, CH4-prio.c5, "
        "CH4-prio.c6, CH4-prio.c7, CH4-prio.c8, CH4-p115.c5, CH4-p115.c6, "
        "CH4-fig4-8.c1, CH4-fig4-8.c2, CH4-fig4-8.c3, CH4-fig4-8.c4, "
        "CH4-fig4-8.c5, CH4-fig4-8.c6, CH4-fig4-8.c7, CH4-fig4-8.c8, "
        "CH4-fig48.f1, CH4-overlap.c4, CH4-overlap.c5, CH4-overlap.c6, "
        "CH4-p118.c3, CH4-p118.c4, CH4-p118.c5 .");

    checks.record(
        "OUT-OF-SUBJECT-WIDTH-OVERLAP [declined]: placing or overlapping "
        "sprites side by side to build a WIDER composite image (as "
        "opposed to attaching a pair for MORE COLOUR) is a compositing/"
        "width proposition, not a colour-register-selection one -- "
        "outside this package's assigned subject. Not measured here. "
        "CH4-overlap.c1, CH4-overlap.c2, CH4-overlap.c3, CH4-p118.c1, "
        "CH4-p118.c2, CH4-fig4-11.c1, CH4-fig4-11.c2, CH4-fig411.f1, "
        "CH4-fig411.f2, CH4-wide.c1, CH4-p119.c1, CH4-reuse.c2, "
        "CH4-reuse.c3 .");

    checks.record(
        "OUT-OF-SUBJECT-SHIFT-TIMING [declined]: the sprite shift "
        "register's per-pixel advance rate and sprite resolution being "
        "independent of bitplane resolution are TIMING/RESOLUTION "
        "propositions, not colour-register-selection ones -- outside "
        "this package's assigned subject. Not measured here. CH4-hw.c10, "
        "CH4-hw.c17, CH4-p124.c6, CH4-p128.c3, CH4-res.c3 .");

    checks.record(
        "GENERIC-INDEPENDENCE-RESTATEMENT [declined]: \"sprites are "
        "created/moved independently of the playfield and of each other\" "
        "and \"the sprite is produced in the colors its color registers "
        "specify\" are generic restatements with no testable content "
        "beyond what TRANSPARENT-CODE-IS-INVISIBLE-NONZERO-CODE-IS-"
        "VISIBLE, NONZERO-CODES-ORDER-MONOTONICALLY-WITHIN-A-GROUP and "
        "DIFFERENT-PAIRS-SELECT-DIFFERENT-REGISTER-GROUPS above already "
        "cover precisely. Not independently measured. CH4-intro.c4, "
        "CH4-intro.c5, CH4-p97.c1, CH4-p97.c2, CH4-p110.c14, "
        "CH4-reset.c2 .");

    checks.record(
        "DUPLICATE-OF-SIBLING-ATTACH-ODD-ONLY [declined]: \"the attach "
        "bit is valid only for odd-numbered sprites\" is the SAME "
        "proposition hrm/ch4-attached-sprites/ already tested as "
        "ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE (using CH4-p128.c8, "
        "CH4-sctl.c4, CH4-attach.c27) and found RED (denise_sprites.v:474, "
        "`attach1 || (!aga && attach0)`, lets the EVEN sprite's own bit "
        "open the combined-colour path at aga=0) -- see that package's "
        "own RESULTS/FINDING. Not re-tested here to avoid re-measuring an "
        "already-confirmed finding. CH4-p128.c9, CH4-sctl.c5 .");

    checks.record(
        "ATTACHED-REVERT-ON-SEPARATION [declined, budget]: what colours "
        "an attached pair's two sprites each individually revert to once "
        "they are moved apart (no longer at the same X/Y) requires "
        "driving the two channels to DIFFERENT positions and comparing "
        "each one's resulting trace against an independently-measured "
        "unattached single-sprite trace at that same position -- a "
        "larger measurement than this package's remaining budget allowed "
        "to build and mutation-test soundly. Declined honestly rather "
        "than asserted on a rushed, unverified basis. CH4-attach.c28, "
        "CH4-attach.c29, CH4-attach.c30, CH4-attach.c31, CH4-attach.c32, "
        "CH4-attach.c33, CH4-p122.c17, CH4-p122.c18, CH4-p122.c19, "
        "CH4-p122.c20, CH4-p122.c21 .");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
