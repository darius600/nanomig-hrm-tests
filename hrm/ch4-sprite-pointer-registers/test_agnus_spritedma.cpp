// ch4-sprite-pointer-registers -- agnus_spritedma conformance test
//
// Test purpose: SPRxPTH/SPRxPTL, the sprite-x DMA pointer pair, at the
//   agnus_spritedma module boundary -- that eight independent pairs exist
//   (one per sprite, x=0..7), each at its own documented address, each
//   implemented in Agnus, marked as an even-address register pair whose low
//   bit is not part of the fetched address, and (the undisputed half of a
//   four-way width conflict) that SPRxPTH supplies the more significant part
//   of the composed pointer and SPRxPTL the less significant part. Package
//   ch4-sprite-pointer-registers, fixture agnus_spritedma (one Verilator
//   build, this module only).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, Appendix A
//   p.285, Appendix B pp.287/291 (claims.jsonl for
//   ch4-sprite-pointer-registers, 145 records; the printed chapter-4 pages
//   110/126/127 this inventory's own conflict_resolution fields quote
//   against are NOT independently read here -- their wording reaches this
//   file only through those fields). A red result below means
//   agnus_spritedma.v disagrees with the manual, not that the assertion is
//   wrong -- no assertion here was, or should be, adjusted to make the RTL
//   pass.
// Non-goals: the exact pointer WIDTH (p110 "high three bits"+"low sixteen
//   bits"+"only fifteen bits used", p126 "18-bit", p127 "32-bit word
//   address", appendix A/B "(high 3 bits)"/"(low 15 bits)"+"18-bit") is a
//   four-way manual self-contradiction and is measured and reported below
//   (MEASURED-WIDTH), never asserted pass/fail against any one of the four
//   readings -- a width assertion would fire identically on all four and
//   would test the manual's disagreement with itself, not the RTL. The "must
//   point to chip memory" half of the appendix-B "+" mark is outside every
//   sprite-pointer-registers boundary (needs the chip memory map) and is not
//   asserted; only the "even address" half is. Whether Denise also decodes
//   these addresses is outside this single-module boundary and is RECORDED,
//   not measured, from the inventory's own classification evidence (see
//   ORDER OF WORK).
// Claim ids: ids such as `APPA-sprptr.c4` and `CH4-appA-sprite.c41` name one
//   sentence of the manual in this chapter's reading. They carry no reader
//   prefix, and they resolve in the claim files that
//   hrm-testpipeline/acceptance/ch4.json lists under `inventories`.
//
// BOUNDARY: nanomig/src/minimig-aga/agnus_spritedma.v, one Verilator
//   build, this module only.
//
// ORDER OF WORK: this file (assertions, expected relationships, claim-id
// coverage, register addresses) was written and committed from
// claims.jsonl's quotations ONLY -- agnus_spritedma.v itself was not
// opened until after that commit, per the build brief's explicit gate.
// Two things were nonetheless known before that commit, both disclosed
// here rather than left silent:
//   (1) The Harness's port names and clocking mechanics (clk/clk7_en
//   toggling, reg_address_in/data_in write sequencing as a WORD address --
//   byte address >> 1 -- reqdma/ackdma DMA-grant handshake, reg_address_out
//   as the granted register's own word address, the vstart/vstop arm-a-
//   window-then-sweep-hpos/vpos pattern) are reused from
//   hrm/agnus_spritedma/test_agnus_spritedma.cpp -- an already-committed,
//   already-built sibling harness for the SAME module, which the build
//   brief itself names as the source to copy the Makefile from. Not
//   learned by opening this package's own forbidden-until-committed file.
//   (2) That agnus_spritedma additionally exposes an `address_out` value
//   -- the composed chip-memory word address a sprite's DMA fetch actually
//   uses, distinct from reg_address_out -- comes from claims.jsonl's own
//   classify.evidence field (present on APPA-sprptr.c1/c2/c3 and their
//   twins, required reading to resolve this package's own conflict
//   status), which already records: writing $FFFF to SPR0PTH/SPR0PTL
//   gives address_out[20:1]=fffff (20 bits); SPR0PTH=$0018 (bits 4,3)
//   reaches address_out[20:16]=18; SPR0PTL's bit 0 is not wired into it;
//   the eight pointer banks read back distinct. That is a measurement the
//   inventory's own classification stage already made and recorded for
//   this package's use, not a fact this file went and read out of the RTL
//   independently -- but it IS RTL-derived, and every assertion built on
//   it (P1, P3, P4 below) is accordingly a plumbing choice, never an
//   expected VALUE: each compares two of this harness's OWN measurements
//   against each other (distinct/equal/bigger-than), not against a number
//   copied from that evidence field. If `address_out` turns out to be
//   named or shaped differently once the module is actually opened for
//   build, only the identifier changes, not any comparison below -- and
//   that will be said plainly in the build/results commit, not papered
//   over.
//
// MEASUREMENT BASIS: every address-valued check below arms exactly one
// sprite x (SPRxPOS=$2000 i.e. vstart=32, SPRxCTL=$2200 i.e. vstop=34,
// the same window hrm/agnus_spritedma/test_agnus_spritedma.cpp already
// uses for sprite 0) on a freshly reset DUT, writes that sprite's own
// SPRxPTH/SPRxPTL, sweeps vpos 32..33 (vstart and one body line -- NOT
// the vblend line the sibling harness also sweeps, see FIRST-GRANT-IS-
// CLEAN below) and, for each line, the full hpos 0..453 DMA slot table,
// and records `address_out` at the FIRST and SECOND cycles
// reg_address_out (word address, restored to a byte address by <<1)
// matches that sprite's own SPRxDATA or SPRxDATB register while
// reqdma&&ackdma -- i.e. the address presented for that sprite's own
// bitplane-data DMA fetch. No assertion below depends on the absolute
// value of that address (which carries whatever fixed relationship
// PTH/PTL->address_out this RTL happens to implement) -- every check
// instead compares two or more such measurements against each other.
//
// FIRST-GRANT-IS-CLEAN (found only once the RTL was open, in Phase 2):
// every granted DMA cycle -- not only a SPRxDATA/SPRxDATB one -- writes
// `sprpth[ptsel]`/`sprptl[ptsel]` from an auto-incremented `newptr`
// whenever ackdma=1 (agnus_spritedma.v:171-196), because on real
// hardware a granted SPRxPOS/SPRxCTL fetch also advances the pointer
// and expects its fetched word written back through reg_address_in/
// data_in on a later cycle. This harness never drives that write-back
// (it has no memory model), so a vblend-triggered SPRxPOS/SPRxCTL grant
// -- which the sibling harness's own sweep (vpos 31..35, vblend=1 on
// line 31) exercises -- would silently auto-increment the pointer bank
// twice before this file's first SPRxDATA/SPRxDATB sample, without ever
// changing SPRxPOS/SPRxCTL's content. The first build of this file did
// exactly that and its MEASURED-WIDTH note read back 1, not the raw
// composed value -- a plumbing artefact of the simplified harness, not
// an RTL finding. Fixed by never asserting vblend and starting the
// sweep at vpos=vstart directly (this module's own write path already
// lets a plain bus write set SPRxPOS/SPRxCTL without any DMA fetch, so
// nothing about arming the window depends on vblend) -- the FIRST
// SPRxDATA/SPRxDATB grant of the sweep is then the pointer's first ever
// use, un-incremented. This is a stimulus/harness correction, not an
// oracle change: no expected relationship below was altered by it.
//
// CLAIM COVERAGE: all 145 records of claims.jsonl are cited below, in
// full, inside an expect() label, grouped by the single proposition each
// restates (the same page/register-table row read by two classification
// readers -- "opus" as APPA-sprptr.*/APPB2-*, "sol" as
// CH4-appA-sprite.*/CH4-APPB-*/APPB2-conf.c6 -- and, for the appendix-B
// table, once per sprite 0..7), matching the grouping convention
// hrm/ch3-sprite-registers/ and hrm/ch3-collision-registers/ both use:
//
//   P1 SPRITE-POINTER-PAIR-PER-CHANNEL-DISTINCT (36 ids) -- a pointer pair
//     exists for each of the 8 sprites, at its own documented address,
//     addressing that sprite's own DMA data, distinctly from every other
//     sprite's.
//   P2 SPRITE-POINTER-REGISTERS-ARE-AGNUS (34 ids) -- SPRxPTH/SPRxPTL are
//     implemented within Agnus (this module).
//   P3 SPRITE-POINTER-EVEN-ADDRESS-PAIR (37 ids) -- the appendix-B "+"
//     mark / legend, and SPRxPTL's least significant bit is not part of
//     the fetched address (even-address pair).
//   P4 SPRXPTH-HIGH-SPRXPTL-LOW (38 ids) -- the undisputed half of the
//     width conflict: SPRxPTH carries the more significant part of the
//     composed pointer, SPRxPTL the less significant part. The disputed
//     exact bit counts these same 38 ids' quotes also carry ("high 3
//     bits"/"low 15 bits"/"18-bit"/"32-bit word address") are measured and
//     reported (MEASURED-WIDTH), not asserted, immediately after.
//   36+34+37+38 = 145. All four groups' id lists are reproduced in full
//   inside their own expect() label below (searchable there), not only in
//   this comment. None declined.
//
// CONTROL CASE: FIXTURE-LIVE-DATA-PATH (no page, no claim id, labelled
// "[control]") arms two different sprites with two different pointer
// patterns and confirms both actually reach a granted DMA fetch and
// produce two DIFFERENT, non-default address_out values, on the exact
// same measurement basis P1/P3/P4 depend on -- before any claim-based
// assertion relies on that path being alive.
//
// RESULTS: built and run under WSL/Verilator 5.052 against
// nanomig/src/minimig-aga/agnus_spritedma.v (the pinned submodule).
// 5 passed, 0 failed -- the control case and all four propositions are
// GREEN. No RTL deviation was found at this boundary for any of the 145
// claims: every sprite's pointer pair is independently addressable and
// distinct (P1), the registers live and take effect inside this Agnus
// module (P2), SPRxPTL's low bit is provably not part of the fetched
// address (P3), and SPRxPTH is provably the more significant half of
// the one composed pointer (P4). MEASURED-WIDTH's own fresh measurement
// (SPR0PTH=SPR0PTL=$FFFF -> address_out=0xfffff, 20 bits) independently
// reproduces the classify-stage's own prior 20-bit reading exactly,
// confirming both measurements agree and that number sits outside all
// four of the manual's own disputed readings (3+15=18, 18, 32) -- as
// the build brief anticipated, it is reported here, not scored.
// Opening the RTL (agnus_spritedma.v:171-196) also explained a plumbing
// bug this file's own first build caught in itself, not in the RTL: see
// FIRST-GRANT-IS-CLEAN above -- fixed before this RESULTS section was
// written, so every PASS/FAIL and MUTATION record here reflects the
// corrected harness.
//
// MUTATION LOG: each entry is one hand-edit -> rebuild -> rerun ->
// confirm ONLY the named assertion changed colour -> revert -> rebuild
// -> confirm the 5/0 baseline restored, run under WSL/Verilator.
// MUTATION FIXTURE-LIVE-DATA-PATH stimulus red: sprite 1's stimulus changed from (0x0002,0x2000) to (0x0001,0x1000) -- the same pattern already used for sprite 0 -- so the two sprites' address_out values collided -> FAIL ("4 passed, 1 failed"), only [control] FIXTURE-LIVE-DATA-PATH newly red (a.address_out == b.address_out). Reverted to (0x0002,0x2000): 5/0 restored.
// MUTATION SPRITE-POINTER-PAIR-PER-CHANNEL-DISTINCT stimulus red: sprite 1's stimulus in the P1 loop was overwritten to reuse sprite 0's own (pth[0],ptl[0]) pattern instead of its own (x+1, 0x1000*(x+1)) -> FAIL ("4 passed, 1 failed"), only SPRITE-POINTER-PAIR-PER-CHANNEL-DISTINCT newly red (m[0].address_out == m[1].address_out, all_distinct false). Reverted the loop body: 5/0 restored.
// MUTATION SPRITE-POINTER-REGISTERS-ARE-AGNUS stimulus red: the "written" measurement's stimulus changed from (0x0006,0x6000) to (0x0000,0x0000) -- identical to the "unwritten" measurement -- so writing no longer changed anything observable -> FAIL ("4 passed, 1 failed"), only SPRITE-POINTER-REGISTERS-ARE-AGNUS newly red (unwritten.address_out == written.address_out). Reverted to (0x0006,0x6000): 5/0 restored.
// MUTATION SPRITE-POINTER-EVEN-ADDRESS-PAIR stimulus red: the "odd_low" measurement's SPR0PTL stimulus changed from $1001 (bit 0, the claimed-ignored bit) to $1002 (bit 1, a meaningful address bit) -> FAIL ("4 passed, 1 failed"), only SPRITE-POINTER-EVEN-ADDRESS-PAIR newly red (even_low.address_out != odd_low.address_out, since bit 1 does reach address_out). Reverted to $1001: 5/0 restored, confirming the check mechanism is sound and that specifically bit 0, not bit 1, is the one the RTL ignores.
// MUTATION SPRXPTH-HIGH-SPRXPTL-LOW stimulus red: hi_h and hi_l's stimuli were swapped -- hi_h given SPRxPTL's own +1-on-bit-1 stimulus (0x0010,0x1002) and hi_l given SPRxPTH's own +1 stimulus (0x0011,0x1000), i.e. mislabelling which register's step is measured as the "high" one -> FAIL ("4 passed, 1 failed"), only SPRXPTH-HIGH-SPRXPTL-LOW newly red (delta_h, now measuring PTL's step, no longer exceeds delta_l, now measuring PTH's step). Reverted both lines to their original stimuli: 5/0 restored.
// No MUTATION record for the MEASURED-WIDTH note: it is a note(), not an expect() -- there is no red/green state for a mutation to move, per the build brief's own distinction between what is asserted and what is measured and reported.

#include <cstdint>
#include <iostream>
#include <sstream>
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
    // For a value this harness itself measured (printed, never asserted
    // pass/fail against one of several disputed manual readings).
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    // For a fact stated by the SOURCE (here: the inventory's own prior
    // classification evidence, out of this single-module boundary) -- not
    // measured by this harness and must not be printed as though it were.
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Byte addresses, per the appendix-B table (p.291) and appendix A (p.285):
// SPRxPTH at $120+4x, SPRxPTL at $122+4x; SPRxPOS at $140+8x, SPRxCTL at
// $142+8x, SPRxDATA at $144+8x, SPRxDATB at $146+8x (word-address base and
// +4-per-sprite layout reused from hrm/agnus_spritedma/
// test_agnus_spritedma.cpp for SPR0PTH/SPR0PTL/SPR0POS/SPR0CTL, extended
// here to sprites 1..7 by the appendix-B table's own printed addresses,
// each independently in this package's inventory).
constexpr int PTH_BYTE(int x) { return 0x120 + 4 * x; }
constexpr int PTL_BYTE(int x) { return 0x122 + 4 * x; }
constexpr int POS_BYTE(int x) { return 0x140 + 8 * x; }
constexpr int CTL_BYTE(int x) { return 0x142 + 8 * x; }
constexpr int DATA_BYTE(int x) { return 0x144 + 8 * x; }
constexpr int DATB_BYTE(int x) { return 0x146 + 8 * x; }

struct FetchMeas {
    bool seen = false;          // a DATA or DATB fetch for this sprite was granted at all
    uint32_t address_out = 0;   // address_out at that FIRST grant -- un-incremented, see FIRST-GRANT-IS-CLEAN
    bool seen2 = false;         // a second DATA/DATB fetch was also granted
    uint32_t address_out2 = 0;  // address_out at that SECOND grant
    int data_fetches = 0;
};

// Isolated measurement: fresh DUT, reset, arm sprite `sprite` alone with
// SPRxPOS=$2000 (vstart=32)/SPRxCTL=$2200 (vstop=34) via a plain bus
// write (this module's own write path for SPRxPOS/SPRxCTL never checks
// ackdma -- agnus_spritedma.v:203-223 -- so no DMA fetch, and therefore
// no vblend, is needed to arm the window), write that sprite's own
// SPRxPTH=pth/SPRxPTL=ptl, then sweep vpos 32..33 (vstart and one body
// line -- never vblend, so the pointer is never auto-incremented before
// its own first use, see FIRST-GRANT-IS-CLEAN above) x hpos 0..453 (one
// full line's DMA slot table, the sibling harness's own sweep range)
// watching for reqdma while granting every request (ackdma=1),
// classifying each grant by its reg_address_out (word address, restored
// to bytes by <<1).
FetchMeas measure_sprite(int sprite, uint16_t pth, uint16_t ptl) {
    Vagnus_spritedma* top = new Vagnus_spritedma;

    top->clk = 0;
    top->clk7_en = 0;
    top->reset = 1;
    top->aga = 0;
    top->ecs = 0;
    top->ackdma = 0;
    top->vbl = 0;
    top->vblend = 0;
    top->hpos = 0;
    top->vpos = 0;
    top->reg_address_in = 0;
    top->data_in = 0;

    auto tick = [&]() {
        top->eval();
        top->clk = 1; top->eval();
        top->clk = 0; top->eval();
    };
    auto tick_hpos = [&]() {
        top->clk7_en = 1;
        tick();
        top->clk7_en = 0;
        tick(); tick(); tick();
    };
    auto write_reg = [&](int byte_addr, uint16_t data) {
        top->reg_address_in = byte_addr >> 1;
        top->data_in = data;
        tick_hpos();
    };

    tick();
    top->reset = 0;

    write_reg(PTH_BYTE(sprite), pth);
    write_reg(PTL_BYTE(sprite), ptl);
    write_reg(POS_BYTE(sprite), 0x2000);
    write_reg(CTL_BYTE(sprite), 0x2200);
    top->reg_address_in = 0;

    FetchMeas result;
    const int data_addr = DATA_BYTE(sprite);
    const int datb_addr = DATB_BYTE(sprite);

    for (int line = 32; line <= 33; ++line) {
        top->vpos = line;
        top->vblend = 0;

        for (int h = 0; h < 454; ++h) {
            top->hpos = h;
            top->clk7_en = 1;
            top->eval();

            if (top->reqdma) {
                top->ackdma = 1;
                top->eval();
                int reg_out = top->reg_address_out << 1;
                if (reg_out == data_addr || reg_out == datb_addr) {
                    result.data_fetches++;
                    if (!result.seen) {
                        result.seen = true;
                        result.address_out = static_cast<uint32_t>(top->address_out);
                    } else if (!result.seen2) {
                        result.seen2 = true;
                        result.address_out2 = static_cast<uint32_t>(top->address_out);
                    }
                }
            } else {
                top->ackdma = 0;
            }
            tick();
            top->clk7_en = 0;
            tick(); tick(); tick();
        }
    }

    top->final();
    delete top;
    return result;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks c;

    // ---- CONTROL CASE: FIXTURE-LIVE-DATA-PATH ----
    {
        FetchMeas a = measure_sprite(0, 0x0001, 0x1000);
        FetchMeas b = measure_sprite(1, 0x0002, 0x2000);
        c.expect(a.seen && b.seen && a.address_out != b.address_out &&
                 a.seen2 && a.address_out2 == a.address_out + 1,
            "[control] FIXTURE-LIVE-DATA-PATH: two sprites armed with two "
            "different, known SPRxPTH/SPRxPTL pointers each reach a "
            "granted SPRxDATA/SPRxDATB DMA fetch and present two "
            "different address_out values, AND sprite 0's own two "
            "successive grants deliver address_out then address_out+1, "
            "in that order, at the two cycles they were actually granted "
            "on (not some fixed or zero value) -- the write-pointer -> "
            "arm -> DMA-grant -> address_out path this whole file depends "
            "on is alive, and delivers known, ordered words, before any "
            "claim-based assertion below relies on it.");
    }

    // ---- P1: SPRITE-POINTER-PAIR-PER-CHANNEL-DISTINCT (36 ids) ----
    {
        uint16_t pth[8], ptl[8];
        FetchMeas m[8];
        bool all_seen = true;
        for (int x = 0; x < 8; ++x) {
            pth[x] = static_cast<uint16_t>(x + 1);
            ptl[x] = static_cast<uint16_t>(0x1000 * (x + 1));
            m[x] = measure_sprite(x, pth[x], ptl[x]);
            all_seen = all_seen && m[x].seen;
        }
        bool all_distinct = true;
        for (int i = 0; i < 8 && all_distinct; ++i)
            for (int j = i + 1; j < 8; ++j)
                if (m[i].address_out == m[j].address_out) { all_distinct = false; break; }

        c.expect(all_seen && all_distinct,
            "SPRITE-POINTER-PAIR-PER-CHANNEL-DISTINCT [p.285,p.291]: "
            "SPRxPTH/SPRxPTL programmed independently for each of the "
            "eight sprites (x=0..7) each reach a granted DMA fetch and "
            "the eight resulting address_out values are pairwise "
            "distinct -- APPA-sprptr.c4, CH4-appA-sprite.c41, "
            "CH4-appA-sprite.c45, CH4-appA-sprite.c50, "
            "APPB2-SPR0PTH.addr, APPB2-SPR0PTL.addr, APPB2-SPR1PTH.addr, "
            "APPB2-SPR1PTL.addr, APPB2-SPR2PTH.addr, APPB2-SPR2PTL.addr, "
            "APPB2-SPR3PTH.addr, APPB2-SPR3PTL.addr, APPB2-SPR4PTH.addr, "
            "APPB2-SPR4PTL.addr, APPB2-SPR5PTH.addr, APPB2-SPR5PTL.addr, "
            "APPB2-SPR6PTH.addr, APPB2-SPR6PTL.addr, APPB2-SPR7PTH.addr, "
            "APPB2-SPR7PTL.addr, CH4-APPB-SPR0PTH.address, "
            "CH4-APPB-SPR0PTL.address, CH4-APPB-SPR1PTH.address, "
            "CH4-APPB-SPR1PTL.address, CH4-APPB-SPR2PTH.address, "
            "CH4-APPB-SPR2PTL.address, CH4-APPB-SPR3PTH.address, "
            "CH4-APPB-SPR3PTL.address, CH4-APPB-SPR4PTH.address, "
            "CH4-APPB-SPR4PTL.address, CH4-APPB-SPR5PTH.address, "
            "CH4-APPB-SPR5PTL.address, CH4-APPB-SPR6PTH.address, "
            "CH4-APPB-SPR6PTL.address, CH4-APPB-SPR7PTH.address, "
            "CH4-APPB-SPR7PTL.address");
    }

    // ---- P2: SPRITE-POINTER-REGISTERS-ARE-AGNUS (34 ids) ----
    {
        FetchMeas unwritten = measure_sprite(3, 0x0000, 0x0000);
        FetchMeas written = measure_sprite(3, 0x0006, 0x6000);
        c.expect(unwritten.seen && written.seen &&
                 unwritten.address_out != written.address_out,
            "SPRITE-POINTER-REGISTERS-ARE-AGNUS [p.285,p.291]: writing "
            "SPR3PTH/SPR3PTL changes the address_out this Agnus module "
            "(agnus_spritedma -- this fixture's only module) presents for "
            "sprite 3's own DMA fetch versus leaving them at their reset "
            "default -- these registers are implemented, and take effect, "
            "inside Agnus -- CH4-appA-sprite.c43, CH4-appA-sprite.c47, "
            "APPB2-SPR0PTH.chip, APPB2-SPR0PTL.chip, APPB2-SPR1PTH.chip, "
            "APPB2-SPR1PTL.chip, APPB2-SPR2PTH.chip, APPB2-SPR2PTL.chip, "
            "APPB2-SPR3PTH.chip, APPB2-SPR3PTL.chip, APPB2-SPR4PTH.chip, "
            "APPB2-SPR4PTL.chip, APPB2-SPR5PTH.chip, APPB2-SPR5PTL.chip, "
            "APPB2-SPR6PTH.chip, APPB2-SPR6PTL.chip, APPB2-SPR7PTH.chip, "
            "APPB2-SPR7PTL.chip, CH4-APPB-SPR0PTH.chip, "
            "CH4-APPB-SPR0PTL.chip, CH4-APPB-SPR1PTH.chip, "
            "CH4-APPB-SPR1PTL.chip, CH4-APPB-SPR2PTH.chip, "
            "CH4-APPB-SPR2PTL.chip, CH4-APPB-SPR3PTH.chip, "
            "CH4-APPB-SPR3PTL.chip, CH4-APPB-SPR4PTH.chip, "
            "CH4-APPB-SPR4PTL.chip, CH4-APPB-SPR5PTH.chip, "
            "CH4-APPB-SPR5PTL.chip, CH4-APPB-SPR6PTH.chip, "
            "CH4-APPB-SPR6PTL.chip, CH4-APPB-SPR7PTH.chip, "
            "CH4-APPB-SPR7PTL.chip");
        c.record("agnus_spritedma is the sole module in this package's "
            "boundary, so whether Denise separately decodes $120-$13E "
            "cannot be measured here; the inventory's own classify."
            "evidence (APPB2-SPR0PTH.chip and twins) already records "
            "denise_sprites.v:104 requiring reg_address_in[8:6]==101, "
            "i.e. only $140 and above -- outside the $120-$13E sprite-"
            "pointer range -- a fact carried from that prior "
            "classification measurement, not established by this "
            "harness.");
    }

    // ---- P3: SPRITE-POINTER-EVEN-ADDRESS-PAIR (37 ids) ----
    {
        FetchMeas even_low = measure_sprite(0, 0x0007, 0x1000);
        FetchMeas odd_low  = measure_sprite(0, 0x0007, 0x1001);
        c.expect(even_low.seen && odd_low.seen &&
                 even_low.address_out == odd_low.address_out,
            "SPRITE-POINTER-EVEN-ADDRESS-PAIR [p.287,p.291]: setting "
            "SPR0PTL's least significant bit (word address $1001 versus "
            "$1000, SPRxPTH held fixed) leaves the sprite-0 DMA fetch "
            "address_out unchanged -- the low bit is not part of the "
            "fetched address, matching the '+' mark's even-address pair "
            "requirement -- APPB2-legend.plus1, APPB2-legend.plus2, "
            "CH4-APPB-LEGEND.c003, CH4-APPB-LEGEND.c004, APPB2-conf.c6, "
            "APPB2-SPR0PTH.mark, APPB2-SPR0PTL.mark, APPB2-SPR1PTH.mark, "
            "APPB2-SPR1PTL.mark, APPB2-SPR2PTH.mark, APPB2-SPR2PTL.mark, "
            "APPB2-SPR3PTH.mark, APPB2-SPR3PTL.mark, APPB2-SPR4PTH.mark, "
            "APPB2-SPR4PTL.mark, APPB2-SPR5PTH.mark, APPB2-SPR5PTL.mark, "
            "APPB2-SPR6PTH.mark, APPB2-SPR6PTL.mark, APPB2-SPR7PTH.mark, "
            "APPB2-SPR7PTL.mark, CH4-APPB-SPR0PTH.mark, "
            "CH4-APPB-SPR0PTL.mark, CH4-APPB-SPR1PTH.mark, "
            "CH4-APPB-SPR1PTL.mark, CH4-APPB-SPR2PTH.mark, "
            "CH4-APPB-SPR2PTL.mark, CH4-APPB-SPR3PTH.mark, "
            "CH4-APPB-SPR3PTL.mark, CH4-APPB-SPR4PTH.mark, "
            "CH4-APPB-SPR4PTL.mark, CH4-APPB-SPR5PTH.mark, "
            "CH4-APPB-SPR5PTL.mark, CH4-APPB-SPR6PTH.mark, "
            "CH4-APPB-SPR6PTL.mark, CH4-APPB-SPR7PTH.mark, "
            "CH4-APPB-SPR7PTL.mark");
    }

    // ---- P4: SPRXPTH-HIGH-SPRXPTL-LOW (38 ids) ----
    {
        FetchMeas base  = measure_sprite(0, 0x0010, 0x1000);
        FetchMeas hi_h  = measure_sprite(0, 0x0011, 0x1000);  // SPRxPTH LSB +1
        FetchMeas hi_l  = measure_sprite(0, 0x0010, 0x1002);  // SPRxPTL effective LSB +1 (bit 0 excluded per P3)

        uint32_t delta_h = (hi_h.address_out > base.address_out)
            ? hi_h.address_out - base.address_out
            : base.address_out - hi_h.address_out;
        uint32_t delta_l = (hi_l.address_out > base.address_out)
            ? hi_l.address_out - base.address_out
            : base.address_out - hi_l.address_out;

        c.expect(base.seen && hi_h.seen && hi_l.seen &&
                 delta_h > 0 && delta_l > 0 && delta_h > delta_l,
            "SPRXPTH-HIGH-SPRXPTL-LOW [p.285,p.291]: stepping SPRxPTH by "
            "its own least significant bit moves sprite 0's fetch "
            "address_out by strictly more than stepping SPRxPTL by its "
            "own least significant meaningful bit (bit 1, per P3's "
            "SPRITE-POINTER-EVEN-ADDRESS-PAIR) moves it -- SPRxPTH "
            "supplies the more significant part of the one composed "
            "pointer and SPRxPTL the less significant part, the "
            "undisputed sub-claim this package's conflict_resolution "
            "names; the exact bit counts these same records' quotes give "
            "are measured, not asserted, immediately below -- "
            "APPA-sprptr.c1, APPA-sprptr.c2, APPA-sprptr.c3, "
            "CH4-appA-sprite.c44, CH4-appA-sprite.c48, "
            "CH4-appA-sprite.c49, APPB2-SPR0PTH.desc, APPB2-SPR0PTL.desc, "
            "APPB2-SPR1PTH.desc, APPB2-SPR1PTL.desc, APPB2-SPR2PTH.desc, "
            "APPB2-SPR2PTL.desc, APPB2-SPR3PTH.desc, APPB2-SPR3PTL.desc, "
            "APPB2-SPR4PTH.desc, APPB2-SPR4PTL.desc, APPB2-SPR5PTH.desc, "
            "APPB2-SPR5PTL.desc, APPB2-SPR6PTH.desc, APPB2-SPR6PTL.desc, "
            "APPB2-SPR7PTH.desc, APPB2-SPR7PTL.desc, "
            "CH4-APPB-SPR0PTH.description, CH4-APPB-SPR0PTL.description, "
            "CH4-APPB-SPR1PTH.description, CH4-APPB-SPR1PTL.description, "
            "CH4-APPB-SPR2PTH.description, CH4-APPB-SPR2PTL.description, "
            "CH4-APPB-SPR3PTH.description, CH4-APPB-SPR3PTL.description, "
            "CH4-APPB-SPR4PTH.description, CH4-APPB-SPR4PTL.description, "
            "CH4-APPB-SPR5PTH.description, CH4-APPB-SPR5PTL.description, "
            "CH4-APPB-SPR6PTH.description, CH4-APPB-SPR6PTL.description, "
            "CH4-APPB-SPR7PTH.description, CH4-APPB-SPR7PTL.description");

        // MEASURED-WIDTH: reported, never turned into a pass/fail against
        // any one of the manual's four disputed readings (p110 "high
        // three"+"low sixteen"+"only fifteen used", p126 "18-bit", p127
        // "32-bit word address", appendix A/B "(high 3 bits)"/"(low 15
        // bits)"+"18-bit" -- see this package's conflict_resolution
        // field). This harness's own measurement, independent of the
        // classify-stage evidence quoted in ORDER OF WORK above:
        FetchMeas allones = measure_sprite(0, 0xFFFF, 0xFFFF);
        std::string bits;
        for (int b = 31; b >= 0; --b) bits += ((allones.address_out >> b) & 1) ? '1' : '0';
        std::ostringstream hex;
        hex << std::hex << allones.address_out;
        c.note("MEASURED-WIDTH (same 38 claims as SPRXPTH-HIGH-SPRXPTL-LOW "
            "above): with SPR0PTH and SPR0PTL both written $FFFF, sprite "
            "0's fetch address_out reads binary " + bits + " (0x" +
            hex.str() +
            "). This is reported as this harness's own measurement of the "
            "pointer's total width and is not compared pass/fail against "
            "any of the manual's four disputed numbers (3+15=18, 18, 32, "
            "or the classify-stage's own prior 20-bit reading) -- doing "
            "so would score the manual's internal disagreement, not the "
            "RTL.");
    }

    std::cout << "\n" << c.passed << " passed, " << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
