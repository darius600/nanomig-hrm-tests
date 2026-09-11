// ch4-sprite-vertical-position -- agnus_spritedma conformance test
//
// Test purpose: the vertical half of Amiga sprite positioning as printed
//   in appendix A (p284) and appendix B (pp287,291,292) -- SPRxPOS's
//   9-bit start-vertical value (SV7-SV0 plus the SV8 high bit), SPRxCTL's
//   9-bit stop-vertical value (EV7-EV0 plus the EV8 high bit), the word
//   address each of the 8 sprites' POS/CTL/DATA/DATB registers occupies,
//   which chip(s) the printed "chip" column claims for each register, the
//   "%" (DMA-usually/processor-sometimes) mark every register in this
//   range carries, and the general statement that SPRxPOS and SPRxCTL
//   work together as position/size/feature control registers -- all at
//   the agnus_spritedma module boundary. Package ch4-sprite-vertical-
//   position, fixture agnus_spritedma (one Verilator build, this module
//   only, per this package's own claims.jsonl).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 284, 287, 291, 292 (claims.jsonl for ch4-sprite-vertical-position,
//   178 records). A red result below means agnus_spritedma.v disagrees
//   with the manual, not that the assertion is wrong -- no assertion here
//   was, or should be, adjusted to make the RTL pass.
//
// FIXTURE BOUNDARY: agnus_spritedma's SPRxCTL write keeps only the
//   vertical fields -- measured during this package's own classification
//   stage (claims.jsonl classify.evidence): "the SPRxCTL write at
//   rtl/agnus_spritedma.v:221 keeps only
//   {data_in[15:8],data_in[6],data_in[5],data_in[2],data_in[1]}", and
//   "toggling SPRxCTL bit 7 (ATT) between 0 and 1 leaves the whole
//   reqdma/reg_address_out/address_out trace ... byte-identical". None of
//   this package's 178 claims requires a horizontal or ATTACH observable
//   (every one is about VSTART/VSTOP, the SV8/EV8 high bits, register
//   addresses, chip-column marks, or the "%" DMA/processor mark), so no
//   claim forces a boundary this fixture cannot deliver -- that finer
//   split (ATT, SH0, the horizontal start bits) belongs to denise_sprites,
//   a different package on a different fixture, not tested here.
//
// SPRxCTL BITS 6 AND 5 -- SILENT IN THE PRINTED SOURCE: this package's
//   own classify.evidence (from the classification stage, before this
//   file was written) already measured that "bits 6 and 5 -- which p284
//   calls unused -- carry the ECS ninth vertical bits, gated by the ecs
//   input" (rtl/agnus_spritedma.v:221/225, :242/244): vstart[9]=bit6,
//   vstop[9]=bit5, alongside the PRINTED SV8=bit2=vstart[8] and
//   EV8=bit1=vstop[8]. No claim in this package's 178 records names bits
//   6/5 that way (APPA-sprctl.c6 and CH4-appA-sprite.c21 print "06-04 X
//   Not used" and are themselves a recorded conflict against appendix C
//   p303's different bit assignment for that span -- see SPRXCTL-BITS-6-
//   4-SPAN-IS-DISPUTED below). Per this package's own build brief: where
//   the corpus is silent, measure and report, do not assert -- so bits
//   6/5 are exercised only by a RECORDED/MEASURED note, never inside a
//   checks.expect() pass/fail condition, and are cited by no claim id.
//
// CONFLICTS: 20 of this package's 178 claims are classified `conflict`,
//   each carrying its own classify.conflict_resolution naming an
//   undisputed sub-claim. Both conflicts are handled below by asserting
//   only the undisputed part: SPRXCTL-CHIP-MARK-AGNUS-HALF (18 ids, the
//   "A D(E)"/"A D" chip-column claims for SPRxCTL) asserts only that
//   Agnus is a genuine write target -- the "A" -- and leaves the (E)
//   qualifier and the Denise ("D") side undisputed-but-untested (out of
//   this boundary); SPRXCTL-BITS-6-4-SPAN-IS-DISPUTED (2 ids) declines
//   entirely, because the disputed part (what bits 6-3 are) is exactly
//   what an "unused" assertion would have to pick a side on, and the
//   undisputed bits in that same row (SV8 at bit 2, EV8 at bit 1) are
//   already separately asserted elsewhere.
//
// Non-goals: The "D" (Denise) half of every "A D"/"A D(E)" chip-column
//   claim, and the "(E)" ECS qualifier specifically, are not tested here
//   -- this fixture has no Denise instance, so only the "A" (Agnus) half
//   of any joint chip-mark claim can be shown. The "feature" component of
//   "SPRxPOS and SPRxCTL work together as position, size and feature
//   sprite-control registers" (p284) is likewise untested -- feature
//   (ATTACH) is a horizontal-boundary observable per the FIXTURE BOUNDARY
//   note above; only the position and size components are asserted.
//   SPRxDATA/SPRxDATB's "%" mark and "usually loaded by DMA... may be
//   loaded by processor" claims are classified `observable: yes` at this
//   boundary, but this package's own classify.evidence records that the
//   two halves of that claim are themselves SPLIT across two modules:
//   "agnus_spritedma does NOT decode $144/$146 for writes at all (:205/
//   220 accept only [2:1]==00/01 -- POS/CTL only); ... a processor write
//   to SPRxDATA/SPRxDATB lands in Denise". So SPRXDATA-SPRXDATB-DMA-PATH-
//   ONLY-AT-THIS-BOUNDARY below asserts only the DMA-emission half (this
//   module requests those addresses on its own); the processor-write half
//   is a non-goal here, not silently assumed. Frequency words themselves
//   ("usually", "sometimes", "normally", "at any time", "during
//   horizontal blank") are never asserted anywhere in this file -- this
//   package's own classify.evidence records, for the "%" mark generally,
//   that "no port carries how often a write happens" and that the DMA
//   slot window is "hpos-gated ..., not blank-gated" -- only the
//   structural fact that both a DMA path and (for POS/CTL) a processor
//   path exist is asserted, never their relative frequency or a precise
//   blanking-window correspondence.
//
// BOUNDARY: nanomig/src/minimig-aga/agnus_spritedma.v, per this package's
//   own claims.jsonl classify.evidence field (produced by an earlier
//   classification pass, not by this file opening the RTL): register
//   writes decode at reg_address_in[8:6]==$140[8:6] with [2:1] selecting
//   POS(00)/CTL(01) and [5:3] the sprite channel (0-7), "confirmed by
//   measurement I (eight distinct pointer banks)"; observable ports are
//   `reqdma`, `reg_address_out`, `address_out` (the DMA request/address
//   side) and `reg_address_in`/`data_in` (the direct-write side) -- no
//   data_out for the fetched sprite image exists at this boundary.
//   `vstart`/`vstop`/`sprdmastate` are internal wires with no read-back
//   port; every assertion below observes them only indirectly, through
//   the DMA-active line range they produce on reqdma/reg_address_out.
//
// ORDER OF WORK: this file (assertions, expected values, claim-id
// coverage, register addresses, bit positions) was written and committed
// from claims.jsonl's quotations and its own classify.evidence field
// ONLY -- agnus_spritedma.v itself was not opened until after that
// commit, per the build brief's explicit gate. The classify.evidence
// field is itself a pre-existing artifact of an earlier classification
// pass over the RTL (the same status as a sibling package's own
// PACKAGE.md evidence field, e.g. hrm/ch3-sprite-registers/), not
// something this file's own author derived by reading the module --
// exactly the precedent hrm/ch3-sprite-registers/test_denise_sprites.cpp
// itself sets ("BOUNDARY" section there cites denise.v line numbers "per
// PACKAGE.md"). The Harness's port names, clk7_en 4-cycle-per-hpos
// clocking, reg_address_in/data_in write sequencing, the reqdma/ackdma
// same-cycle-sample-then-grant handshake, and the word-address map
// ($140->0xA0 for SPR0POS, +4 words per sprite) are reused from
// hrm/agnus_spritedma/test_agnus_spritedma.cpp -- an already-built,
// already-verified sibling harness for this SAME module ("fixture
// agnus_spritedma (one module, builds standalone -- verified today)" per
// this package's own build brief) -- not learned by opening this
// package's own forbidden-until-committed file. Three numeric stimulus
// values (SPR0POS=$28/VSTART=40, SPR0CTL=$2C/VSTOP=44, and the second
// SPR0CTL=$2E/VSTOP=46) are reused directly from this package's own
// classify.evidence field, which already reports them as measured:
// "with SPR0POS=$2800 (VSTART=40) and SPR0CTL=$2C00 (VSTOP=44), reqdma is
// granted twice per line with reg_address_out naming SPR0DATA at hpos 53
// and SPR0DATB at hpos 57 on lines 40-43, and naming SPR0POS/SPR0CTL on
// line 44; with SPR0CTL=$2E00 (VSTOP=46) the identical stimulus carries
// the window through line 45 instead." Reusing an already-measured,
// already-reported stimulus recipe from the classification stage is not
// the same thing as this file's own author opening the RTL -- the
// classification stage's own job was exactly to establish what is
// observable and how, before any package is built on top of it.
//
// MEASUREMENT BASIS: every non-declined assertion below writes SPR0POS
// and/or SPR0CTL directly through reg_address_in/data_in (the same single
// write interface every write in this file uses -- there is no separate
// DMA-tagged write path into this module's own POS/CTL registers) on a
// freshly reset Harness, then sweeps vpos from 0 up to a stated maximum,
// and at every line sweeps the full 454-hpos line granting ackdma
// whenever reqdma is seen, recording the first and one-past-last line at
// which this sprite's own DATA/DATB address (0x144/0x146, +8 per sprite)
// appears on reg_address_out -- "Window::start"/"Window::end". A vpos
// sweep always starts at 0 (never mid-frame) so no measurement depends on
// the DMA sequencer having already been armed by an earlier, unobserved
// transition. SPRXPOS-SPRXCTL-ADDRESS-MAP instead sweeps with `vblend`
// held high (the automatic POS/CTL-prefetch trigger, per the sibling
// harness's own "Line 31 (vblend): POS/CTL fetch") and collects the SET
// of addresses this module requests on its own, unprompted by any
// register write -- proving address existence, not per-sprite
// independence under direct write (which the address/bit-field
// assertions below prove for sprite 0 specifically, and which this
// package's own classify.evidence separately reports as "confirmed by
// measurement I (eight distinct pointer banks)" for the decode logic
// itself).
//
// CLAIM COVERAGE: all 178 records of claims.jsonl are cited below, in
// full, inside an expect() or record() label. Ids restating the identical
// proposition under a different manual wording, page, sprite number (0-7)
// or reader are grouped and cited together on the single assertion that
// tests that proposition, as hrm/ch3-sprite-registers/ and
// hrm/ch3-collision-registers/ both do.
//
//   [control] FIXTURE-DELIVERS-DISTINCT-VALUES-IN-ORDER (0 ids)
//   SPRXPOS-SPRXCTL-ADDRESS-MAP (34 ids)
//   SPRXPOS-BYTE-IS-SV70-VSTART (11 ids)
//   SPRXCTL-BYTE-IS-EV70-VSTOP (11 ids)
//   SPRXCTL-BIT2-SV8-IS-VSTART-HIGH-BIT (4 ids)
//   SPRXCTL-BIT1-EV8-IS-VSTOP-HIGH-BIT (2 ids)
//   SPRXCTL-BITS-6-4-SPAN-IS-DISPUTED [declined] (2 ids)
//   AGNUS-IS-WRITE-TARGET-FOR-SPRXPOS-AND-SPRXCTL (39 ids)
//   SPRXPOS-SPRXCTL-WRITE-BUS-HAS-BOTH-DMA-AND-PROCESSOR-PATHS (39 ids)
//   SPRXDATA-SPRXDATB-DMA-PATH-ONLY-AT-THIS-BOUNDARY (36 ids)
//   34+11+11+4+2+2+39+39+36 = 178.
//
// CONTROL CASE: FIXTURE-DELIVERS-DISTINCT-VALUES-IN-ORDER (no page, no
// claim id, labelled "[control]" so the mutation gate does not require a
// mutation record beyond its own) proves this fixture can deliver a known
// word before anything reads one, per this package's own build brief:
// two DIFFERENT (VSTART,VSTOP) pairs, written directly through
// reg_address_in/data_in, each land in their OWN correct role (VSTART
// governs where the window starts, VSTOP where it ends -- not swapped,
// not merged, not dropped) and produce the exact window classify.evidence
// already measured (40..44, then 40..46) -- before any claim-based
// assertion below depends on the same write -> sweep -> observe path.
//
// RESULTS: filled in below, after opening the RTL and building, in a
// separate commit per the build brief's order of work.
//
// MUTATION LOG: filled in below, after opening the RTL and building, in
// a separate commit per the build brief's order of work.

#include <cstdint>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vagnus_spritedma.h"

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    // For records that state what the SOURCE says, or why a claim is
    // declined. Not measurements; must never be printed as though they
    // were.
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Word-address register map, reused from hrm/agnus_spritedma/
// test_agnus_spritedma.cpp for this same module, generalised to all 8
// sprite channels per this package's own classify.evidence ("reg_address_
// in[8:6]==$140[8:6] with [2:1] selecting POS/CTL and [5:3] the channel"):
// each sprite occupies 4 consecutive word addresses (POS,CTL,DATA,DATB),
// 8 bytes apart in byte-address terms, 8 sprites total.
constexpr int PosWordAddr(int n) { return (0x140 + 8 * n) >> 1; }
constexpr int CtlWordAddr(int n) { return (0x142 + 8 * n) >> 1; }
constexpr int DataByteAddr(int n) { return 0x144 + 8 * n; }
constexpr int DatbByteAddr(int n) { return 0x146 + 8 * n; }

// One isolated DUT: fresh reset, clk7_en 4-cycle-per-hpos clocking and the
// reg_address_in/data_in write sequencing are reused verbatim from the
// already-built, already-verified sibling harness for this SAME module
// (see ORDER OF WORK above), not learned from this package's own RTL.
class Harness {
public:
    explicit Harness(uint8_t ecs = 0, uint8_t aga = 0) : dut(new Vagnus_spritedma) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->reset = 1;
        dut->aga = aga;
        dut->ecs = ecs;
        dut->ackdma = 0;
        dut->vbl = 0;
        dut->vblend = 0;
        dut->hpos = 0;
        dut->vpos = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        tick();
        dut->reset = 0;
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

    void tick_hpos() {
        dut->clk7_en = 1;
        tick();
        dut->clk7_en = 0;
        tick();
        tick();
        tick();
    }

    void write_reg(int addr, uint16_t data) {
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick_hpos();
        dut->reg_address_in = 0;
        dut->data_in = 0;
    }

    Vagnus_spritedma* dut;
};

void write_pos(Harness& h, int sprite_idx, uint8_t vstart_low8) {
    h.write_reg(PosWordAddr(sprite_idx), static_cast<uint16_t>(vstart_low8) << 8);
}

// ev70_byte: SPRxCTL bits 15-8 (EV7-EV0). sv8/ev8: bit 2/bit 1, the
// PRINTED high bits (p284: "02 SV8", "01 EV8"). bit6/bit5: the ECS-only,
// manual-silent ninth-bit extension this package's own classify.evidence
// measured (vstart[9]=bit6, vstop[9]=bit5) -- used ONLY by the MEASURED
// note below, never inside a pass/fail expect().
void write_ctl(Harness& h, int sprite_idx, uint8_t ev70_byte, bool sv8, bool ev8,
               bool bit6 = false, bool bit5 = false) {
    uint16_t v = (static_cast<uint16_t>(ev70_byte) << 8)
               | (bit6 ? 0x0040u : 0u)
               | (bit5 ? 0x0020u : 0u)
               | (sv8  ? 0x0004u : 0u)
               | (ev8  ? 0x0002u : 0u);
    h.write_reg(CtlWordAddr(sprite_idx), v);
}

struct Window {
    int start = -1;  // first line (0-based) at which this sprite's own
                      // DATA/DATB address is seen on reg_address_out
                      // during a granted reqdma cycle.
    int end = -1;     // one past the last such line: [start,end) is the
                       // observed DMA-active range.
};

// Sweeps vpos 0..vpos_max (exclusive), vblend held low throughout (so no
// automatic POS/CTL re-fetch competes with a value already written
// directly before this call), granting ackdma immediately whenever reqdma
// is seen -- reqdma is always sampled by its OWN eval() BEFORE ackdma is
// asserted and evaluated again, never the reverse (the exact bug this
// package's own build brief warns caused a prior harness's data path to
// look silently dead: "the DMA grant was set inside the same eval() that
// sampled the request").
Window measure_window(Harness& h, int sprite_idx, int vpos_max) {
    Window w;
    const int data_byte = DataByteAddr(sprite_idx);
    const int datb_byte = DatbByteAddr(sprite_idx);
    for (int line = 0; line < vpos_max; ++line) {
        h.dut->vpos = line;
        h.dut->vblend = 0;
        bool active_this_line = false;
        for (int hp = 0; hp < 454; ++hp) {
            h.dut->hpos = hp;
            h.dut->clk7_en = 1;
            h.dut->eval();
            if (h.dut->reqdma) {
                h.dut->ackdma = 1;
                h.dut->eval();
                int reg_out = h.dut->reg_address_out << 1;
                if (reg_out == data_byte || reg_out == datb_byte) active_this_line = true;
            } else {
                h.dut->ackdma = 0;
            }
            h.tick();
            h.dut->clk7_en = 0;
            h.tick();
            h.tick();
            h.tick();
        }
        if (active_this_line) {
            if (w.start == -1) w.start = line;
            w.end = line + 1;
        }
    }
    return w;
}

// Sweeps vpos in [vpos_lo,vpos_hi) with vblend held HIGH throughout (the
// automatic prefetch trigger the sibling harness's own "Line 31 (vblend):
// POS/CTL fetch" exercises), collecting the SET of byte addresses this
// module's own DMA sequencer requests on its own -- no register is
// written before this call; the Harness is used at its power-on default.
std::set<int> sweep_prefetch_addresses(Harness& h, int vpos_lo, int vpos_hi) {
    std::set<int> seen;
    for (int line = vpos_lo; line < vpos_hi; ++line) {
        h.dut->vpos = line;
        h.dut->vblend = 1;
        for (int hp = 0; hp < 454; ++hp) {
            h.dut->hpos = hp;
            h.dut->clk7_en = 1;
            h.dut->eval();
            if (h.dut->reqdma) {
                h.dut->ackdma = 1;
                h.dut->eval();
                seen.insert(h.dut->reg_address_out << 1);
            } else {
                h.dut->ackdma = 0;
            }
            h.tick();
            h.dut->clk7_en = 0;
            h.tick();
            h.tick();
            h.tick();
        }
    }
    return seen;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ------------- [control] FIXTURE-DELIVERS-DISTINCT-VALUES-IN-ORDER -------------
    {
        // Stimulus reused directly from this package's own
        // classify.evidence measurement (see ORDER OF WORK above):
        // SPR0POS=$28 (VSTART=40), SPR0CTL=$2C (VSTOP=44) -> window
        // [40,44); SPR0CTL=$2E (VSTOP=46) -> window [40,46).
        Harness h1;
        write_pos(h1, 0, 0x28);
        write_ctl(h1, 0, 0x2C, false, false);
        Window w1 = measure_window(h1, 0, 50);
        bool first_pair_ok = (w1.start == 40 && w1.end == 44);

        Harness h2;
        write_pos(h2, 0, 0x28);
        write_ctl(h2, 0, 0x2E, false, false);
        Window w2 = measure_window(h2, 0, 50);
        bool second_pair_ok = (w2.start == 40 && w2.end == 46);

        checks.expect(first_pair_ok && second_pair_ok,
            "[control] FIXTURE-DELIVERS-DISTINCT-VALUES-IN-ORDER: writing "
            "SPR0POS=$28 (VSTART=40) then SPR0CTL=$2C (VSTOP=44) through "
            "reg_address_in/data_in -- the one write interface every write "
            "in this file uses -- produces the DMA-active window [40,44); "
            "writing the SAME SPR0POS=$28 with a DIFFERENT SPR0CTL=$2E "
            "(VSTOP=46) on a fresh DUT produces [40,46), not [40,44) again "
            "and not some corrupted or swapped range -- two distinct "
            "values arrive, in the correct role each was written for, at "
            "the cycle they were written on. Both windows match this "
            "package's own classify.evidence measurement exactly. Fixture "
            "check, no claim id, no page.");
    }

    // ------------- SPRXPOS-SPRXCTL-ADDRESS-MAP -------------
    {
        Harness h;
        std::set<int> seen = sweep_prefetch_addresses(h, 0, 30);
        bool all_present = true;
        std::vector<int> missing;
        for (int n = 0; n < 8; ++n) {
            int pos_addr = 0x140 + 8 * n;
            int ctl_addr = 0x142 + 8 * n;
            if (!seen.count(pos_addr)) { all_present = false; missing.push_back(pos_addr); }
            if (!seen.count(ctl_addr)) { all_present = false; missing.push_back(ctl_addr); }
        }
        checks.expect(all_present,
            "SPRXPOS-SPRXCTL-ADDRESS-MAP: from a freshly reset DUT (no "
            "register written), sweeping vpos 0..29 with vblend held high "
            "-- the automatic POS/CTL-prefetch trigger -- and granting "
            "ackdma on every reqdma, this module's own DMA sequencer "
            "requests all 16 of the printed word addresses on its own: "
            "$140/$142 (sprite 0) through $178/$17A (sprite 7), matching "
            "'SPR0POS 140 w A D' / 'SPR0CTL 142 w A D(E)' through "
            "'SPR7POS 178 W A D' / 'SPR7CTL 17A W A D' (pp284,291,292), "
            "confirming the printed address map exists as a real decode "
            "point in this module for every one of the 8 channels -- "
            "consistent with this package's own classify.evidence, which "
            "separately reports the decode logic itself as 'confirmed by "
            "measurement I (eight distinct pointer banks)'. "
            "CH4-appA-sprite.c5, APPB2-SPR0POS.addr, APPB2-SPR1POS.addr, "
            "CH4-APPB-SPR0POS.address, CH4-APPB-SPR1POS.address, "
            "APPB2-SPR2POS.addr, APPB2-SPR3POS.addr, APPB2-SPR4POS.addr, "
            "APPB2-SPR5POS.addr, APPB2-SPR6POS.addr, APPB2-SPR7POS.addr, "
            "CH4-APPB-SPR2POS.address, CH4-APPB-SPR3POS.address, "
            "CH4-APPB-SPR4POS.address, CH4-APPB-SPR5POS.address, "
            "CH4-APPB-SPR6POS.address, CH4-APPB-SPR7POS.address, "
            "CH4-appA-sprite.c1, APPB2-SPR0CTL.addr, "
            "CH4-APPB-SPR0CTL.address, APPB2-SPR1CTL.addr, "
            "APPB2-SPR2CTL.addr, APPB2-SPR3CTL.addr, APPB2-SPR4CTL.addr, "
            "APPB2-SPR5CTL.addr, APPB2-SPR6CTL.addr, APPB2-SPR7CTL.addr, "
            "CH4-APPB-SPR1CTL.address, CH4-APPB-SPR2CTL.address, "
            "CH4-APPB-SPR3CTL.address, CH4-APPB-SPR4CTL.address, "
            "CH4-APPB-SPR5CTL.address, CH4-APPB-SPR6CTL.address, "
            "CH4-APPB-SPR7CTL.address . ");
    }

    // ------------- SPRXPOS-BYTE-IS-SV70-VSTART -------------
    {
        Harness ha;
        write_pos(ha, 0, 0x20);
        write_ctl(ha, 0, 0xFF, false, false);  // VSTOP far out so the window does not close early.
        Window wa = measure_window(ha, 0, 50);

        Harness hb;
        write_pos(hb, 0, 0x21);
        write_ctl(hb, 0, 0xFF, false, false);
        Window wb = measure_window(hb, 0, 50);

        bool ok = wa.start != -1 && wb.start != -1 && (wb.start - wa.start) == 1;
        checks.expect(ok,
            "SPRXPOS-BYTE-IS-SV70-VSTART: incrementing SPR0POS's byte by "
            "1 (0x20 -> 0x21, SPR0CTL held at $FF/VSTOP far out) moves the "
            "measured DMA-active start line by exactly 1, matching "
            "'15-08 SV7-SV0 Start vertical value.' (p284) -- SPRxPOS bits "
            "15-8 ARE the (low 8 bits of the) vertical start value this "
            "module compares vpos against. CH4-appA-sprite.c8 and the "
            "per-sprite APPB2/CH4-APPB '.description'/'.mark' restatements "
            "of 'sprite x vert-horiz start position data' all restate the "
            "same identity. APPA-sprpos.c3, CH4-appA-sprite.c14, "
            "CH4-appA-sprite.c8, CH4-APPB-SPR0POS.description, "
            "CH4-APPB-SPR1POS.description, CH4-APPB-SPR2POS.description, "
            "CH4-APPB-SPR3POS.description, CH4-APPB-SPR4POS.description, "
            "CH4-APPB-SPR5POS.description, CH4-APPB-SPR6POS.description, "
            "CH4-APPB-SPR7POS.description . ");
    }

    // ------------- SPRXCTL-BYTE-IS-EV70-VSTOP -------------
    {
        Harness ha;
        write_pos(ha, 0, 0x02);  // VSTART early and fixed.
        write_ctl(ha, 0, 0x20, false, false);
        Window wa = measure_window(ha, 0, 50);

        Harness hb;
        write_pos(hb, 0, 0x02);
        write_ctl(hb, 0, 0x21, false, false);
        Window wb = measure_window(hb, 0, 50);

        bool ok = wa.end != -1 && wb.end != -1 && (wb.end - wa.end) == 1;
        checks.expect(ok,
            "SPRXCTL-BYTE-IS-EV70-VSTOP: with SPR0POS fixed at 0x02 "
            "(VSTART=2), incrementing SPR0CTL's upper byte by 1 (0x20 -> "
            "0x21) moves the measured DMA-active end line by exactly 1, "
            "matching '15-08 EV7-EV0 End (stop) vertical value low 8 "
            "bits' (p284) -- SPRxCTL bits 15-8 ARE the (low 8 bits of the) "
            "vertical stop value. The per-sprite '.description' rows "
            "restate the same register identity ('sprite x vert stop "
            "position and control data'). APPA-sprctl.c4, "
            "CH4-appA-sprite.c19, CH4-appA-sprite.c4, "
            "CH4-APPB-SPR0CTL.description, CH4-APPB-SPR1CTL.description, "
            "CH4-APPB-SPR2CTL.description, CH4-APPB-SPR3CTL.description, "
            "CH4-APPB-SPR4CTL.description, CH4-APPB-SPR5CTL.description, "
            "CH4-APPB-SPR6CTL.description, CH4-APPB-SPR7CTL.description . ");
    }

    // ------------- SPRXCTL-BIT2-SV8-IS-VSTART-HIGH-BIT -------------
    {
        Harness ha;
        write_pos(ha, 0, 0xFF);              // VSTART low byte maxed (255).
        write_ctl(ha, 0, 0xFF, false, false); // SV8=0, VSTOP far out.
        Window wa = measure_window(ha, 0, 260);

        Harness hb;
        write_pos(hb, 0, 0xFF);
        write_ctl(hb, 0, 0xFF, true, false);  // SV8=1.
        Window wb = measure_window(hb, 0, 520);

        bool ok = wa.start == 255 && wb.start == 511;
        checks.expect(ok,
            "SPRXCTL-BIT2-SV8-IS-VSTART-HIGH-BIT: with SPR0POS fixed at "
            "0xFF (SV7-SV0 maxed at 255), SPR0CTL bit 2 (SV8) clear gives "
            "a measured start of line 255; setting it alone (all other "
            "bits, including EV8, held clear) gives a measured start of "
            "line 511 = 255 + 256 -- SV8 is genuinely the 9th (high) bit "
            "of the vertical start value, matching '02 SV8 Start vertical "
            "value high bit' (p284) read together with the SPRxPOS row "
            "that names the high bit as living in SPRxCTL: 'High bit(SV8) "
            "is in SPRxCTL register below.' (p284). APPA-sprctl.c7, "
            "CH4-appA-sprite.c22, APPA-sprpos.c4, CH4-appA-sprite.c15 . ");
    }

    // ------------- SPRXCTL-BIT1-EV8-IS-VSTOP-HIGH-BIT -------------
    {
        Harness ha;
        write_pos(ha, 0, 0x02);               // VSTART early and fixed.
        write_ctl(ha, 0, 0xFF, false, false);  // EV8=0.
        Window wa = measure_window(ha, 0, 260);

        Harness hb;
        write_pos(hb, 0, 0x02);
        write_ctl(hb, 0, 0xFF, false, true);   // EV8=1.
        Window wb = measure_window(hb, 0, 520);

        bool ok = wa.end == 255 && wb.end == 511;
        checks.expect(ok,
            "SPRXCTL-BIT1-EV8-IS-VSTOP-HIGH-BIT: with SPR0POS fixed at "
            "0x02 (VSTART=2) and SPR0CTL's upper byte maxed at 0xFF, bit "
            "1 (EV8) clear gives a measured end of line 255; setting it "
            "alone (all other bits, including SV8, held clear) gives a "
            "measured end of line 511 = 255 + 256, matching '01 EV8 End "
            "(stop) vertical value high bit' (p284) -- EV8 is genuinely "
            "the 9th (high) bit of the vertical stop value. "
            "APPA-sprctl.c8, CH4-appA-sprite.c23 . ");
    }

    // ------------- SPRXCTL-BITS-6-4-SPAN-IS-DISPUTED [declined] -------------
    // No checks.expect() and no MUTATION record: the disputed part of
    // this claim (what bits 6-3 are) is exactly what an "unused"
    // assertion would have to pick a side on, and the undisputed bits in
    // this same printed row (SV8 at bit 2, EV8 at bit 1) are already
    // separately asserted above.
    checks.record(
        "SPRXCTL-BITS-6-4-SPAN-IS-DISPUTED [declined]: APPA-sprctl.c6, "
        "CH4-appA-sprite.c21 -- p284 prints '06-04 X Not used', but "
        "chapter pp.107/128 give the unused span as bits 6-3 instead (per "
        "this package's own classify.conflict_resolution), and appendix C "
        "p303 separately assigns bit 4 SHSH1 and bit 3 SHSH0 "
        "'(unimplemented)', scoped to ECS chips only. What is undisputed "
        "in this same row -- SV8 at bit 2, EV8 at bit 1, EV7-EV0 at bits "
        "15-8 -- is already asserted by SPRXCTL-BIT2-SV8-IS-VSTART-HIGH-"
        "BIT, SPRXCTL-BIT1-EV8-IS-VSTOP-HIGH-BIT and "
        "SPRXCTL-BYTE-IS-EV70-VSTOP above. Declined, not tested: asserting "
        "'bits 6-4 unused' would require taking a side the sources "
        "themselves do not agree on.");

    // MEASURED note: bits 6 and 5 are silent in every printed source this
    // package's 178 claims draw on (see the header note above). Reported,
    // not asserted -- no claim id, no pass/fail.
    {
        auto probe = [](bool ecs, bool bit6, bool bit5) -> int {
            Harness h(ecs);
            write_pos(h, 0, 0x02);
            write_ctl(h, 0, 0xF0, false, false, bit6, bit5);
            Window w = measure_window(h, 0, 520);
            return w.start;
        };
        int ecs0_base = probe(false, false, false);
        int ecs0_bit6 = probe(false, true, false);
        int ecs0_bit5 = probe(false, false, true);
        int ecs1_base = probe(true, false, false);
        int ecs1_bit6 = probe(true, true, false);
        int ecs1_bit5 = probe(true, false, true);
        checks.note(
            "SPRxCTL bits 6/5 (SPR0POS=0x02, SPR0CTL upper byte=0xF0, "
            "SV8/EV8 both clear, swept vpos 0..519): measured start line "
            "-- ecs=0: base=" + std::to_string(ecs0_base) +
            " bit6=" + std::to_string(ecs0_bit6) +
            " bit5=" + std::to_string(ecs0_bit5) +
            "; ecs=1: base=" + std::to_string(ecs1_base) +
            " bit6=" + std::to_string(ecs1_bit6) +
            " bit5=" + std::to_string(ecs1_bit5) + ". Per this package's "
            "own classify.evidence, bit6 is vstart[9] and bit5 is "
            "vstop[9] (rtl/agnus_spritedma.v:221/225), gated by the ecs "
            "input (:242/244) -- a fact no printed source in this "
            "package's 178 claims states. If either delta lands outside "
            "the swept range (vpos wraps or this module's own vpos port "
            "does not extend that far), that too is part of what was "
            "measured, not assumed.");
    }

    // ------------- AGNUS-IS-WRITE-TARGET-FOR-SPRXPOS-AND-SPRXCTL -------------
    {
        // Independent measurement (own Harness instances, not reusing
        // wa/wb from the assertions above) so a mutation aimed at this
        // assertion's own stimulus cannot silently move
        // SPRXPOS-BYTE-IS-SV70-VSTART or SPRXCTL-BYTE-IS-EV70-VSTOP too.
        Harness hp1;
        write_pos(hp1, 0, 0x10);
        write_ctl(hp1, 0, 0xFF, false, false);
        Window wp1 = measure_window(hp1, 0, 60);

        Harness hp2;
        write_pos(hp2, 0, 0x18);
        write_ctl(hp2, 0, 0xFF, false, false);
        Window wp2 = measure_window(hp2, 0, 60);

        bool pos_reaches_agnus = wp1.start != -1 && wp2.start != -1 && wp1.start != wp2.start;

        Harness hc1;
        write_pos(hc1, 0, 0x04);
        write_ctl(hc1, 0, 0x10, false, false);
        Window wc1 = measure_window(hc1, 0, 40);

        Harness hc2;
        write_pos(hc2, 0, 0x04);
        write_ctl(hc2, 0, 0x14, false, false);
        Window wc2 = measure_window(hc2, 0, 40);

        bool ctl_reaches_agnus = wc1.end != -1 && wc2.end != -1 && wc1.end != wc2.end;

        checks.expect(pos_reaches_agnus && ctl_reaches_agnus,
            "AGNUS-IS-WRITE-TARGET-FOR-SPRXPOS-AND-SPRXCTL: a write to "
            "SPR0POS changes this module's own measured DMA-active start "
            "(shown above by SPRXPOS-BYTE-IS-SV70-VSTART, confirmed again "
            "here with an independent measurement), and a write to "
            "SPR0CTL changes this module's own measured end -- Agnus "
            "(this module) is demonstrably one of the chips the printed "
            "'A D'/'A D(E)' chip column names for these two registers, "
            "matching 'SPR0POS 140 w A D ...' and 'SPR0CTL 142 w A D(E) "
            "...' (p291) and their per-sprite restatements (pp284,291,"
            "292). This module has no Denise instance, so the 'D' half of "
            "every such claim, and the '(E)' qualifier specifically, are "
            "NOT tested here -- see Non-goals above; for the SPRxCTL "
            "claims this is also the undisputed half of a recorded "
            "conflict over whether the Denise side carries an ECS "
            "qualifier (classify.conflict_resolution: 'Undisputed: SPRxCTL "
            "is written by Agnus and by Denise ... Disputed and kept out "
            "of any assertion: whether the Denise side carries an ECS "
            "qualifier'). Independently, the SAME two measurements show "
            "SPRxPOS alone changes the result (position) and SPRxCTL "
            "alone changes the result (size, i.e. the vertical extent "
            "VSTART..VSTOP) -- neither register alone accounts for the "
            "whole result, matching 'SPRxPOS and SPRxCTL work together as "
            "position, size and feature sprite-control registers.' "
            "(p284). This boundary exposes no ATTACH/feature observable "
            "(see FIXTURE BOUNDARY above), so only the position and size "
            "components of that joint claim are asserted here. "
            "APPA-sprpos.c2, CH4-appA-sprite.c7, APPB2-SPR0POS.chip, "
            "APPB2-SPR1POS.chip, CH4-APPB-SPR0POS.chip, "
            "CH4-APPB-SPR1POS.chip, APPB2-SPR2POS.chip, "
            "APPB2-SPR3POS.chip, APPB2-SPR4POS.chip, APPB2-SPR5POS.chip, "
            "APPB2-SPR6POS.chip, APPB2-SPR7POS.chip, "
            "CH4-APPB-SPR2POS.chip, CH4-APPB-SPR3POS.chip, "
            "CH4-APPB-SPR4POS.chip, CH4-APPB-SPR5POS.chip, "
            "CH4-APPB-SPR6POS.chip, CH4-APPB-SPR7POS.chip, "
            "APPA-sprctl.c2, CH4-appA-sprite.c3, APPB2-SPR0CTL.chip, "
            "CH4-APPB-SPR0CTL.chip, APPB2-SPR1CTL.chip, "
            "APPB2-SPR2CTL.chip, APPB2-SPR3CTL.chip, APPB2-SPR4CTL.chip, "
            "APPB2-SPR5CTL.chip, APPB2-SPR6CTL.chip, APPB2-SPR7CTL.chip, "
            "CH4-APPB-SPR1CTL.chip, CH4-APPB-SPR2CTL.chip, "
            "CH4-APPB-SPR3CTL.chip, CH4-APPB-SPR4CTL.chip, "
            "CH4-APPB-SPR5CTL.chip, CH4-APPB-SPR6CTL.chip, "
            "CH4-APPB-SPR7CTL.chip, APPA-sprpair.c1, "
            "CH4-appA-sprite.c10, CH4-appA-sprite.c9 . ");
    }

    // --- SPRXPOS-SPRXCTL-WRITE-BUS-HAS-BOTH-DMA-AND-PROCESSOR-PATHS ---
    {
        // DMA side: this module requests both POS/CTL (already shown by
        // SPRXPOS-SPRXCTL-ADDRESS-MAP above) unprompted by any write --
        // re-measured here, independently, restricted to sprite 0.
        Harness hd;
        std::set<int> seen = sweep_prefetch_addresses(hd, 0, 5);
        bool dma_side = seen.count(0x140) && seen.count(0x142);

        // Processor side: a direct reg_address_in/data_in write (the
        // SAME mechanism used everywhere in this file) reaches and
        // changes this module's own state -- independent measurement,
        // own Harness instances.
        Harness hp1;
        write_pos(hp1, 0, 0x30);
        write_ctl(hp1, 0, 0xFF, false, false);
        Window wp1 = measure_window(hp1, 0, 60);

        Harness hp2;
        write_pos(hp2, 0, 0x38);
        write_ctl(hp2, 0, 0xFF, false, false);
        Window wp2 = measure_window(hp2, 0, 60);

        bool processor_side = wp1.start != -1 && wp2.start != -1 && wp1.start != wp2.start;

        checks.expect(dma_side && processor_side,
            "SPRXPOS-SPRXCTL-WRITE-BUS-HAS-BOTH-DMA-AND-PROCESSOR-PATHS: "
            "this module's own DMA sequencer requests SPR0POS ($140) and "
            "SPR0CTL ($142) on its own (re-measured here from a fresh, "
            "unwritten DUT), AND a direct reg_address_in/data_in write to "
            "the same two addresses independently changes this module's "
            "measured behaviour -- both the DMA path and the processor "
            "path this package's own classify.evidence names genuinely "
            "exist at this boundary for SPRxPOS/SPRxCTL ('agnus_spritedma "
            "accepts processor writes on reg_address_in (:205/220) and "
            "emits the same addresses itself on reg_address_out during a "
            "granted slot'), matching the '%' mark's legend definition "
            "('Register used by DMA channel usually, processors "
            "sometimes.' p287) and the body text ('usually loaded by the "
            "sprite DMA channel ...; however, they may be loaded by "
            "either processor' p284). Per this package's own "
            "classify.evidence, the RELATIVE FREQUENCY those words name "
            "('usually'/'sometimes'/'at any time'/'during horizontal "
            "blank') is NOT assertable at this or any boundary ('no port "
            "carries how often a write happens'; the DMA slot window is "
            "'hpos-gated ..., not blank-gated') -- only that both paths "
            "structurally exist is claimed here, never their frequency or "
            "a precise blanking-window correspondence. "
            "APPB2-legend.pct1, APPB2-legend.pct2, CH4-APPB-LEGEND.c002, "
            "APPB2-SPR0CTL.mark, APPB2-SPR0POS.mark, APPB2-SPR1POS.mark, "
            "CH4-APPB-SPR0CTL.mark, CH4-APPB-SPR0POS.mark, "
            "CH4-APPB-SPR1POS.mark, APPB2-SPR1CTL.mark, "
            "APPB2-SPR2CTL.mark, APPB2-SPR2POS.mark, APPB2-SPR3CTL.mark, "
            "APPB2-SPR3POS.mark, APPB2-SPR4CTL.mark, APPB2-SPR4POS.mark, "
            "APPB2-SPR5CTL.mark, APPB2-SPR5POS.mark, APPB2-SPR6CTL.mark, "
            "APPB2-SPR6POS.mark, APPB2-SPR7CTL.mark, APPB2-SPR7POS.mark, "
            "CH4-APPB-SPR1CTL.mark, CH4-APPB-SPR2CTL.mark, "
            "CH4-APPB-SPR2POS.mark, CH4-APPB-SPR3CTL.mark, "
            "CH4-APPB-SPR3POS.mark, CH4-APPB-SPR4CTL.mark, "
            "CH4-APPB-SPR4POS.mark, CH4-APPB-SPR5CTL.mark, "
            "CH4-APPB-SPR5POS.mark, CH4-APPB-SPR6CTL.mark, "
            "CH4-APPB-SPR6POS.mark, CH4-APPB-SPR7CTL.mark, "
            "CH4-APPB-SPR7POS.mark, APPA-sprpair.c2, APPA-sprpair.c3, "
            "CH4-appA-sprite.c12, CH4-appA-sprite.c13 . ");
    }

    // ------- SPRXDATA-SPRXDATB-DMA-PATH-ONLY-AT-THIS-BOUNDARY -------
    {
        // SPR0DATA/SPR0DATB: this module's DMA sequencer requests them
        // on its own during the sprite's active window (VSTART=40,
        // VSTOP=44 -- the same classify.evidence-sourced stimulus the
        // control case uses); it does NOT decode them for direct writes
        // at all, per classify.evidence ("agnus_spritedma does NOT decode
        // $144/$146 for writes at all (:205/220 accept only [2:1]==00/
        // 01)") -- so no processor-write stimulus is attempted here for
        // these two addresses; doing so would test nothing this module
        // can answer.
        Harness hd;
        write_pos(hd, 0, 0x28);
        write_ctl(hd, 0, 0x2C, false, false);
        Window wd = measure_window(hd, 0, 50);
        bool dma_side = wd.start == 40 && wd.end == 44;

        checks.expect(dma_side,
            "SPRXDATA-SPRXDATB-DMA-PATH-ONLY-AT-THIS-BOUNDARY: arming "
            "SPR0POS/SPR0CTL for VSTART=40/VSTOP=44 makes this module's "
            "own DMA sequencer request SPR0DATA ($144) and SPR0DATB "
            "($146) automatically on lines 40-43 (measured here as the "
            "DMA-active window [40,44), the same window the control case "
            "above validates) -- the DMA half of the '%' mark and of "
            "'usually loaded by the sprite image DMA channel' (p284) IS "
            "observable at this boundary and IS asserted here. This "
            "package's own classify.evidence records that the OTHER half "
            "of the same claim -- 'may be loaded by either processor' -- "
            "is a genuine SPLIT across two modules, not merely unobserved: "
            "'agnus_spritedma does NOT decode $144/$146 for writes at all "
            "... a processor write to SPRxDATA/SPRxDATB lands in Denise'. "
            "That half belongs to a different package on a different "
            "fixture (denise_sprites) and is a non-goal here, not "
            "silently assumed true or false. Frequency words "
            "('usually'/'at any time') are not asserted, per the same "
            "reasoning as SPRXPOS-SPRXCTL-WRITE-BUS-HAS-BOTH-DMA-AND-"
            "PROCESSOR-PATHS above. APPB2-SPR0DATA.mark, "
            "APPB2-SPR0DATB.mark, CH4-APPB-SPR0DATA.mark, "
            "CH4-APPB-SPR0DATB.mark, APPB2-SPR1DATA.mark, "
            "APPB2-SPR1DATB.mark, APPB2-SPR2DATA.mark, "
            "APPB2-SPR2DATB.mark, APPB2-SPR3DATA.mark, "
            "APPB2-SPR3DATB.mark, APPB2-SPR4DATA.mark, "
            "APPB2-SPR4DATB.mark, APPB2-SPR5DATA.mark, "
            "APPB2-SPR5DATB.mark, APPB2-SPR6DATA.mark, "
            "APPB2-SPR6DATB.mark, APPB2-SPR7DATA.mark, "
            "APPB2-SPR7DATB.mark, CH4-APPB-SPR1DATA.mark, "
            "CH4-APPB-SPR1DATB.mark, CH4-APPB-SPR2DATA.mark, "
            "CH4-APPB-SPR2DATB.mark, CH4-APPB-SPR3DATA.mark, "
            "CH4-APPB-SPR3DATB.mark, CH4-APPB-SPR4DATA.mark, "
            "CH4-APPB-SPR4DATB.mark, CH4-APPB-SPR5DATA.mark, "
            "CH4-APPB-SPR5DATB.mark, CH4-APPB-SPR6DATA.mark, "
            "CH4-APPB-SPR6DATB.mark, CH4-APPB-SPR7DATA.mark, "
            "CH4-APPB-SPR7DATB.mark, APPA-sprdat.c4, APPA-sprdat.c5, "
            "CH4-appA-sprite.c34, CH4-appA-sprite.c35 . ");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
