// Test purpose: the sixteen sprite pointer registers Agnus decodes -
//   SPR0PTH/SPR0PTL through SPR7PTH/SPR7PTL - are write-only: a read aimed
//   at one of their own addresses does not return what was written there.
//   Package ch4-sprite-registers-write-only-agnus, 34 claim records, all
//   citing this one proposition repeated per register (plus, for SPR0PTH
//   and SPR0PTL only, the generic "SPRxPTH"/"SPRxPTL" family entry on
//   p.285, which names the same two addresses).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition. p.285
//   (Appendix A, the generic SPRxPTH/SPRxPTL family row) and p.291
//   (Appendix B, the sixteen per-sprite rows SPR0PTH..SPR7PTL). A red
//   result below means agnus.v disagrees with the manual, not that the
//   assertion is wrong - no assertion here is, or will be, adjusted to
//   make the RTL pass.
//
// THE OCR DAMAGE ON p.291 (read, not solved, here): all sixteen p.291 rows
//   this package cites carry a lower-case `w` in the access column where
//   the printed page sets upper-case `W` - one instance of a
//   corpus-wide, already-documented defect (hrm/appB/ocr-repairs.md
//   records exactly 21 lower-case-`w` rows on p.291; every one of this
//   package's sixteen rows is inside that count). This is not a defect in
//   the claims: each one quotes the damaged text verbatim (`quote_verbatim:
//   yes`) and states the correction in its own restatement, which is the
//   corpus's documented reading rule (`ocr-repairs.md`, "the second
//   reading quoted the damage as it stands and put the correction in the
//   restatement"). The letter being read is `w` or `W` does not change
//   what is being asserted: p.287's legend defines only `W=write-only`,
//   and both readers' restatements settle on that meaning for every row
//   cited here. Nothing about the OCR damage changes any assertion below.
//
// A MISSING COLUMN IS NOT A STATEMENT: every row this package cites - the
//   p.285 generic entry and all sixteen p.291 per-sprite rows - carries a
//   non-empty chip column (`A`, Agnus) and a non-empty access column (`W`/
//   `w`, corrected to `W`). Nothing here is asserted from a blank cell.
//
// Non-goal: the pointer registers' bit WIDTH, byte-vs-word addressing, and
//   the high/low split composing a chip-memory address are
//   hrm/ch4-sprite-pointer-registers' job (fixture agnus_spritedma,
//   already built) and are not reproduced here - this package's 34 claims
//   are exclusively about write-only-ness, never about what the written
//   bits do once stored.
//
// BOUNDARY: `agnus`, the Agnus top-level module - not agnus_spritedma.
//   Every one of this package's 34 claims carries the identical
//   classify.observable field: "needs a wider boundary: agnus --
//   agnus_spritedma has no data_out port (rtl/agnus_spritedma.v:86-102);
//   the read path is agnus.v:277/281" and the identical classify.evidence:
//   "port list rtl/agnus_spritedma.v:86-102 -- the module's outputs are
//   reqdma, reg_address_out and address_out; there is no data_out.
//   rtl/agnus.v:277 sources data_out from data_bmc|dmaconr|data_blt only,
//   so no sprite pointer address is ever read back". Write-only-ness is a
//   fact about what reaches the CPU-visible data bus, and agnus_spritedma
//   alone has no such bus to observe - only the top-level module carries
//   the mux that (per this evidence) never includes a sprite-pointer term.
//   This matches the build brief's own instruction: hrm/ch7-dmacon-register-bits
//   and hrm/ch4-sprite-dma-control (committed tonight, read in full before
//   writing this file) both already build exactly this fixture in this
//   same rtl/ tree.
//
// HOW TO MEASURE IT (read from hrm/ch4-sprite-dma-control, built tonight,
//   before writing this file, per the build brief's own instruction): a
//   read that returns something other than the pattern just written proves
//   nothing on its own - it could equally be a harness whose read path
//   never samples anything live. That package's own DMACON-IS-WRITE-ONLY-
//   AT-096 assertion pairs the suspect read (at DMACON's own address) with
//   a control read, in the SAME run, at DMACONR - a genuinely readable
//   register that reflects the very same write. This package's claims
//   name no readable twin of their own (classify.twin says so explicitly:
//   "none in this lot" for the p.285 pair; the p.291 pairs' twin field
//   only cross-references the OTHER reader's record of the same sentence,
//   not a readable register), so the paired control used below is the
//   same DMACON/DMACONR pair hrm/ch4-sprite-dma-control already
//   established on this identical fixture: each write-only check below
//   writes a known pattern to the sprite address under test AND writes a
//   known pattern to DMACON, then reads both addresses back in the same
//   run. DMACONR reflecting its pattern is what proves the read path is
//   live; the sprite address's read then not reflecting ITS pattern is
//   what the proposition claims.
//
// PORTS (from every claim's own classify.evidence field, and from
//   hrm/ch4-sprite-dma-control's and hrm/ch7-dmacon-register-bits's already-
//   committed Harness for this identical fixture - NOT discovered by this
//   package opening agnus.v, which has not happened yet):
//   in  - clk, clk7_en, cck, reset, aen, rd, hwr, lwr, address_in, data_in,
//         audio_dmal, audio_dmas, disk_dmal, disk_dmas, bls, ntsc, a1k,
//         ecs, aga, floppy_speed
//   out - data_out (the read-back bus every write-only check samples;
//         agnus.v:277 sources it from data_bmc|dmaconr|data_blt only, per
//         every claim's own evidence), dbr (bus-busy/arbiter-grant signal
//         the write_reg/read_reg retry loop polls, same as the sibling
//         packages' Harness)
//   Register-bus addressing: a 9-bit word address (byte address >> 1), the
//   same convention DMACON=$096/DMACONR=$002 use in
//   hrm/ch4-sprite-dma-control.
//
// ============================================================================
// CLAIMS (verbatim quote + restatement, from
// work/ch4_packages/ch4-sprite-registers-write-only-agnus.jsonl, package
// "ch4-sprite-registers-write-only-agnus", all 34 records, classify.status
// == "testable"). Grouped by register address; the p.291 rows were each
// read independently by two readers (opus, sol) and both ids are cited
// together wherever they name the same sentence - classify.twin on each
// pair records "both records of this sentence classify identically, so
// the sentence carries no reader disagreement".
//
//   $120 SPR0PTH (p.285 generic "SPRxPTH 120 W A Sprite x pointer (high 3
//     bits)" + p.291 "SPR0PTH + 120 w A Sprite 0 pointer (high 3 bits)"):
//     CH4-appA-sprite.c42 (sol, p.285) -> "SPRxPTH is write-only."
//     APPB2-SPR0PTH.acc (opus, p.291) -> "SPR0PTH is write-only. [...]"
//     CH4-APPB-SPR0PTH.access (sol, p.291) -> "The access field reads
//       lowercase 'w'. [...] this visibly damaged cell is corrected here
//       to 'W', meaning write-only."
//   $122 SPR0PTL (p.285 generic "SPRxPTL 122 W A Sprite x pointer (low 15
//     bits)" + p.291 "SPR0PTL + 122 w A Sprite 0 pointer (low 15 bits)"):
//     CH4-appA-sprite.c46 (sol, p.285), APPB2-SPR0PTL.acc (opus, p.291),
//     CH4-APPB-SPR0PTL.access (sol, p.291).
//   $124 SPR1PTH (p.291 only): APPB2-SPR1PTH.acc, CH4-APPB-SPR1PTH.access.
//   $126 SPR1PTL (p.291 only): APPB2-SPR1PTL.acc, CH4-APPB-SPR1PTL.access.
//   $128 SPR2PTH (p.291 only): APPB2-SPR2PTH.acc, CH4-APPB-SPR2PTH.access.
//   $12A SPR2PTL (p.291 only): APPB2-SPR2PTL.acc, CH4-APPB-SPR2PTL.access.
//   $12C SPR3PTH (p.291 only): APPB2-SPR3PTH.acc, CH4-APPB-SPR3PTH.access.
//   $12E SPR3PTL (p.291 only): APPB2-SPR3PTL.acc, CH4-APPB-SPR3PTL.access.
//   $130 SPR4PTH (p.291 only): APPB2-SPR4PTH.acc, CH4-APPB-SPR4PTH.access.
//   $132 SPR4PTL (p.291 only): APPB2-SPR4PTL.acc, CH4-APPB-SPR4PTL.access.
//   $134 SPR5PTH (p.291 only): APPB2-SPR5PTH.acc, CH4-APPB-SPR5PTH.access.
//   $136 SPR5PTL (p.291 only): APPB2-SPR5PTL.acc, CH4-APPB-SPR5PTL.access.
//   $138 SPR6PTH (p.291 only): APPB2-SPR6PTH.acc, CH4-APPB-SPR6PTH.access.
//   $13A SPR6PTL (p.291 only): APPB2-SPR6PTL.acc, CH4-APPB-SPR6PTL.access.
//   $13C SPR7PTH (p.291 only): APPB2-SPR7PTH.acc, CH4-APPB-SPR7PTH.access.
//   $13E SPR7PTL (p.291 only): APPB2-SPR7PTL.acc, CH4-APPB-SPR7PTL.access.
//
// That is all 34 records: 2 on p.285 (SPR0PTH/SPR0PTL's generic family
// entry only - the family entry names one representative address, the
// same $120/$122 p.291 already gives sprite 0, so it is cited alongside
// sprite 0's own rows rather than as a seventeenth register) + 16 opus
// p.291 records + 16 sol p.291 records = 34. Every id above is cited on
// exactly one planned assertion below; none are left uncovered.
//
// COVERAGE ALREADY MEASURED ELSEWHERE: none. hrm/ch4-sprite-pointer-
// registers builds agnus_spritedma alone, which (per every claim's own
// evidence, quoted above) has no data_out port at all and so cannot
// observe write-only-ness - it measures the composed address, not the
// register bus. No other package in hrm/ builds `agnus` and reads these
// sixteen addresses. All 34 ids are this package's own job.
//
// ============================================================================
// PLANNED ASSERTIONS (names, cited-id groups and intent only - no RTL line
// numbers beyond what the claims' own classify.evidence already handed
// over; no fixture code below this comment block, matching hrm/ch4-
// sprite-dma-control's own spec-before-RTL gate. Implementation added in
// the next commit, after agnus.v is opened to confirm the write_reg/
// read_reg pattern copied from hrm/ch4-sprite-dma-control still applies
// unchanged):
//
//   0. FIXTURE-DELIVERS-KNOWN-WORD-VIA-DMACONR: this package's own control
//      case, expected to PASS, and its fixture-delivers-a-known-word
//      proof, run fresh on this package's own Harness instance (the fix
//      does not travel with a copied file - see hrm/ch4-sprite-dma-
//      control's own IMPLEMENTATION NOTES on exactly this point). Three
//      different DMACON patterns are written in sequence and DMACONR is
//      read back after EACH one, confirming each arrives distinct and in
//      order. DMACON/DMACONR are not this package's subject - no claim id
//      is cited - this assertion exists purely to show the harness's read
//      path is alive before any write-only check below leans on that read
//      path returning "not what was written" as evidence of anything.
//
//   1-16. <REGISTER>-IS-WRITE-ONLY-AT-<ADDR>, one per address in the table
//      above (SPR0PTH-IS-WRITE-ONLY-AT-120 through SPR7PTL-IS-WRITE-ONLY-
//      AT-13E): write a distinct non-zero pattern to the sprite address
//      under test; in the SAME run, write a distinct non-zero pattern to
//      DMACON; read back DMACONR (expected to reflect its own pattern -
//      the paired control, on the same measurement basis, proving the
//      read path is live) and read back the sprite address under test
//      itself (expected NOT to reflect the pattern written to it - the
//      proposition under test). Each assertion cites the claim id group
//      for its own address from the table above.
//
// Seventeen planned assertions (one control + sixteen write-only checks).
// Together they cite all 34 ids in ch4-sprite-registers-write-only-
// agnus.jsonl.
//
// No RTL has been opened yet to write this comment block: the port names
// (listed above) and the register-bus mechanics (word-addressed CPU bus,
// arbitrated by `dbr`, an unselected address reading back 0 through an
// OR-mux) come from this package's own claims' classify.evidence fields
// and from hrm/ch4-sprite-dma-control's and hrm/ch7-dmacon-register-bits's
// already-committed files (both handed to or available to the building
// agent as inventory, not discovered by this agent opening agnus.v). The
// sixteen register addresses come directly from each claim's own quoted
// text, not from the RTL. Exact confirmation that the write_reg/read_reg
// retry shape still applies unchanged against this fixture, and the
// mutation log, are answered in the next commit, after agnus.v is read.
// ============================================================================

// ============================================================================
// IMPLEMENTATION NOTES (added in this commit, after agnus.v and
// agnus_spritedma.v were opened for the first time - the specification
// comment above this point was committed on its own first, per the
// chapter's spec-before-RTL gate; see this package's own git history).
//
// agnus.v confirms every fact the claims' own classify.evidence already
// handed over, at the exact lines cited there: `assign data_out =
// data_bmc | dmaconr | data_blt;` (:277) - three terms, none of them
// sourced from anything inside agnus_spritedma - and the module header
// (:54-100) carries exactly the port list named above, nothing more that
// this package's Harness needs to drive. `localparam DMACON = 9'h096`
// (:103), `localparam DMACONR = 9'h002` (:104), and the DMACONR read mux
// (`if (reg_address[8:1]==DMACONR[8:1]) dmaconr[15:0] = {...}; else
// dmaconr = 0`, :280-282) and DMACON write (`else if
// (reg_address[8:1]==DMACON[8:1]) ... if (data_in[15]) dmacon <=
// dmacon|data_in[12:0]; else dmacon <= dmacon & ~data_in[12:0]`, :286-293)
// are byte-for-byte what hrm/ch4-sprite-dma-control's own IMPLEMENTATION
// NOTES already documented for this identical fixture - reused here, not
// rediscovered. agnus_spritedma.v additionally confirms the sixteen
// addresses this package's claims quote: `parameter SPRPTBASE_REG =
// 9'h120` (:106, matching $120's own quoted row exactly) with `sprpth`/
// `sprptl` register banks indexed off it (:111 and nearby) - nothing in
// that module, nor anywhere else in agnus.v's read side, ever reaches
// `data_out`. Nothing here was a surprise against the inventory; opening
// both files confirmed the spec above rather than revising it.
//
// The write_reg/read_reg retry-on-pre-edge-dbr shape (copied unchanged
// from hrm/ch4-sprite-dma-control's Harness, which itself copied it from
// hrm/ch7-dmacon-register-bits) is still the correct way to drive this fixture,
// for the identical reason those packages' own notes give: the register
// write/read at :280/:289 is gated by the SAME `reg_address` wire the
// arbiter (:169-267) overwrites on every cycle some DMA engine holds the
// bus, and agnus_refresh's unconditional periodic pulse contends for it
// regardless of any DMACON or sprite-pointer state - the retry loop is
// what makes an access land on the first available non-refresh cycle
// instead of assuming one is always immediately free.
//
// MUTATION LOG: see the bottom of this file, after main(). Every entry
// was actually applied - edited into this file, rebuilt under WSL/
// Verilator (`make test`), run, the PASS/FAIL line read from real output,
// then reverted and rebuilt clean before the next entry.
// ============================================================================

#include <cstdint>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vagnus.h"

namespace {

// ---- register addresses (9-bit custom-chip offsets) ----
constexpr uint16_t DMACON_ADDR   = 0x096;  // agnus.v:103
constexpr uint16_t DMACONR_ADDR  = 0x002;  // agnus.v:104
constexpr uint16_t SPR0PTH_ADDR  = 0x120;  // agnus_spritedma.v:106 (SPRPTBASE_REG)
constexpr uint16_t SPR0PTL_ADDR  = 0x122;
constexpr uint16_t SPR1PTH_ADDR  = 0x124;
constexpr uint16_t SPR1PTL_ADDR  = 0x126;
constexpr uint16_t SPR2PTH_ADDR  = 0x128;
constexpr uint16_t SPR2PTL_ADDR  = 0x12A;
constexpr uint16_t SPR3PTH_ADDR  = 0x12C;
constexpr uint16_t SPR3PTL_ADDR  = 0x12E;
constexpr uint16_t SPR4PTH_ADDR  = 0x130;
constexpr uint16_t SPR4PTL_ADDR  = 0x132;
constexpr uint16_t SPR5PTH_ADDR  = 0x134;
constexpr uint16_t SPR5PTL_ADDR  = 0x136;
constexpr uint16_t SPR6PTH_ADDR  = 0x138;
constexpr uint16_t SPR6PTL_ADDR  = 0x13A;
constexpr uint16_t SPR7PTH_ADDR  = 0x13C;
constexpr uint16_t SPR7PTL_ADDR  = 0x13E;

// ---- DMACON bit masks, the paired control's own pattern ----
constexpr uint16_t SET_CLR = 0x8000;  // bit 15
constexpr uint16_t SPREN   = 1u << 5;
constexpr uint16_t DMAEN   = 1u << 9;
constexpr uint16_t BPLEN   = 1u << 8;

// Harness: copied unchanged from hrm/ch4-sprite-dma-control's own Harness
// (same fixture, same arbitrated register bus, same reason for the
// pre-edge-dbr retry - see IMPLEMENTATION NOTES above).
class Harness {
public:
    Harness() : dut(new Vagnus) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->cck = 0;
        dut->reset = 1;
        dut->aen = 0;
        dut->rd = 0;
        dut->hwr = 0;
        dut->lwr = 0;
        dut->data_in = 0;
        dut->address_in = 0;
        dut->audio_dmal = 0;
        dut->audio_dmas = 0;
        dut->disk_dmal = 0;
        dut->disk_dmas = 0;
        dut->bls = 0;
        dut->ntsc = 0;
        dut->a1k = 0;
        dut->ecs = 0;
        dut->aga = 0;
        dut->floppy_speed = 0;
        for (int i = 0; i < 16; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    // One CPU register write, held for up to `max_ticks` ticks and applied
    // on the first tick where `dbr==0`. `write_ok` reports whether a
    // dbr==0 tick was actually seen, checked at every call site so a
    // silently-missed write cannot masquerade as a passing assertion.
    void write_reg(uint16_t reg9, uint16_t data, int max_ticks = 8) {
        dut->aen = 1;
        dut->hwr = 1;
        dut->lwr = 1;
        dut->rd = 0;
        dut->address_in = static_cast<uint16_t>(reg9 >> 1);
        dut->data_in = data;
        write_ok = false;
        for (int i = 0; i < max_ticks; ++i) {
            bool pre_dbr = tick_pre_edge_dbr();
            if (!pre_dbr) { write_ok = true; break; }
        }
        dut->aen = 0;
        dut->hwr = 0;
        dut->lwr = 0;
        dut->address_in = 0;
        dut->data_in = 0;
    }

    // One CPU register read, same dbr==0 retry as write_reg. `read_ok`
    // reports whether a clean sample was taken.
    uint16_t read_reg(uint16_t reg9, int max_ticks = 8) {
        dut->aen = 1;
        dut->rd = 1;
        dut->hwr = 0;
        dut->lwr = 0;
        dut->address_in = static_cast<uint16_t>(reg9 >> 1);
        uint16_t val = 0xFFFF;
        read_ok = false;
        for (int i = 0; i < max_ticks; ++i) {
            tick();
            if (!last_dbr) { val = last_data_out; read_ok = true; break; }
        }
        dut->aen = 0;
        dut->rd = 0;
        dut->address_in = 0;
        return val;
    }

    void tick() { tick_pre_edge_dbr(); }

    bool tick_pre_edge_dbr() {
        dut->cck = cck_phase ? 1 : 0;
        cck_phase = !cck_phase;
        dut->eval();
        bool pre_dbr = dut->dbr;
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        last_dbr = dut->dbr;
        last_data_out = dut->data_out;
        return pre_dbr;
    }

    bool last_dbr = false;
    uint16_t last_data_out = 0;
    bool write_ok = false;
    bool read_ok = false;

    Vagnus* dut;

private:
    bool cck_phase = false;
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

    // ---------------- FIXTURE-DELIVERS-KNOWN-WORD-VIA-DMACONR ----------------
    // This package's own control case, expected to PASS, run fresh on this
    // package's own Harness instance - the fixture-integrity proof the
    // build brief requires, not inherited from a copied file. Three
    // different DMACON patterns are written in sequence and DMACONR is
    // sampled after EACH one, confirming each arrives distinct and in the
    // order it was written. DMACON/DMACONR are not this package's own
    // subject (no claim id here) - this establishes that the read path
    // used as the paired control in every check below is actually alive.
    {
        Harness h;
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t after_a = h.read_reg(DMACONR_ADDR);
        bool ok_a = h.read_ok && h.write_ok;

        h.write_reg(DMACON_ADDR, SET_CLR | BPLEN);
        uint16_t after_b = h.read_reg(DMACONR_ADDR);
        bool ok_b = h.read_ok && h.write_ok;

        h.write_reg(DMACON_ADDR, 0x0000 | SPREN);  // CLR only SPREN
        uint16_t after_c = h.read_reg(DMACONR_ADDR);
        bool ok_c = h.read_ok && h.write_ok;

        checks.expect(
            ok_a && ok_b && ok_c &&
                after_a == (SPREN | DMAEN) &&
                after_b == (SPREN | DMAEN | BPLEN) &&
                after_c == (DMAEN | BPLEN),
            "FIXTURE-DELIVERS-KNOWN-WORD-VIA-DMACONR: three distinct DMACON "
            "writes, sampled through DMACONR after each one on this "
            "package's own Harness instance, arrive in the order written - "
            "proving the read path every write-only check below leans on "
            "as its paired control is actually live, not a silently-dead "
            "data path. No claim id: DMACON/DMACONR are not this package's "
            "subject, only its measurement infrastructure.");
    }

    // ---------------- SPR0PTH-IS-WRITE-ONLY-AT-120 (p.285, p.291) ----------------
    {
        Harness h;
        h.write_reg(SPR0PTH_ADDR, 0x1111);
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t ctrl = h.read_reg(DMACONR_ADDR);
        bool ok_ctrl = h.read_ok;
        uint16_t at_addr = h.read_reg(SPR0PTH_ADDR);
        bool ok_addr = h.read_ok;

        checks.expect(
            ok_ctrl && ok_addr && ctrl == (SPREN | DMAEN) && at_addr == 0x0000,
            "SPR0PTH-IS-WRITE-ONLY-AT-120: after writing 0x1111 to SPR0PTH "
            "($120), a read through the SAME aen/rd/address_in path AIMED "
            "AT $120 itself returns 0x0000, not 0x1111 - while the SAME "
            "run's paired control (SPREN|DMAEN written to DMACON $096, read "
            "back through DMACONR $002) DOES return its own pattern, "
            "proving the read path is live and $120's silence is not a "
            "dead harness. 'SPRxPTH 120 W A Sprite x pointer (high 3 bits)' "
            "(p.285); 'SPR0PTH + 120 w A Sprite 0 pointer (high 3 bits)' "
            "(p.291, access column corrected w->W per p.287's legend, see "
            "header comment). CH4-appA-sprite.c42, APPB2-SPR0PTH.acc, "
            "CH4-APPB-SPR0PTH.access.");
    }

    // ---------------- SPR0PTL-IS-WRITE-ONLY-AT-122 (p.285, p.291) ----------------
    {
        Harness h;
        h.write_reg(SPR0PTL_ADDR, 0x2222);
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t ctrl = h.read_reg(DMACONR_ADDR);
        bool ok_ctrl = h.read_ok;
        uint16_t at_addr = h.read_reg(SPR0PTL_ADDR);
        bool ok_addr = h.read_ok;

        checks.expect(
            ok_ctrl && ok_addr && ctrl == (SPREN | DMAEN) && at_addr == 0x0000,
            "SPR0PTL-IS-WRITE-ONLY-AT-122: after writing 0x2222 to SPR0PTL "
            "($122), a read AIMED AT $122 itself returns 0x0000, not "
            "0x2222 - while the SAME run's paired control (SPREN|DMAEN "
            "written to DMACON $096, read back through DMACONR $002) DOES "
            "return its own pattern. 'SPRxPTL 122 W A Sprite x pointer (low "
            "15 bits)' (p.285); 'SPR0PTL + 122 w A Sprite 0 pointer (low 15 "
            "bits)' (p.291, access column corrected w->W per p.287's "
            "legend, see header comment). CH4-appA-sprite.c46, "
            "APPB2-SPR0PTL.acc, CH4-APPB-SPR0PTL.access.");
    }

    // ---------------- SPR1PTH-IS-WRITE-ONLY-AT-124 (p.291) ----------------
    {
        Harness h;
        h.write_reg(SPR1PTH_ADDR, 0x3333);
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t ctrl = h.read_reg(DMACONR_ADDR);
        bool ok_ctrl = h.read_ok;
        uint16_t at_addr = h.read_reg(SPR1PTH_ADDR);
        bool ok_addr = h.read_ok;

        checks.expect(
            ok_ctrl && ok_addr && ctrl == (SPREN | DMAEN) && at_addr == 0x0000,
            "SPR1PTH-IS-WRITE-ONLY-AT-124: after writing 0x3333 to SPR1PTH "
            "($124), a read AIMED AT $124 itself returns 0x0000, not "
            "0x3333 - while the SAME run's paired control (SPREN|DMAEN "
            "written to DMACON $096, read back through DMACONR $002) DOES "
            "return its own pattern. 'SPR1PTH + 124 w A Sprite 1 pointer "
            "(high 3 bits)' (p.291, access column corrected w->W per "
            "p.287's legend, see header comment). APPB2-SPR1PTH.acc, "
            "CH4-APPB-SPR1PTH.access.");
    }

    // ---------------- SPR1PTL-IS-WRITE-ONLY-AT-126 (p.291) ----------------
    {
        Harness h;
        h.write_reg(SPR1PTL_ADDR, 0x4444);
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t ctrl = h.read_reg(DMACONR_ADDR);
        bool ok_ctrl = h.read_ok;
        uint16_t at_addr = h.read_reg(SPR1PTL_ADDR);
        bool ok_addr = h.read_ok;

        checks.expect(
            ok_ctrl && ok_addr && ctrl == (SPREN | DMAEN) && at_addr == 0x0000,
            "SPR1PTL-IS-WRITE-ONLY-AT-126: after writing 0x4444 to SPR1PTL "
            "($126), a read AIMED AT $126 itself returns 0x0000, not "
            "0x4444 - while the SAME run's paired control (SPREN|DMAEN "
            "written to DMACON $096, read back through DMACONR $002) DOES "
            "return its own pattern. 'SPR1PTL + 126 w A Sprite 1 pointer "
            "(low 15 bits)' (p.291, access column corrected w->W per "
            "p.287's legend, see header comment). APPB2-SPR1PTL.acc, "
            "CH4-APPB-SPR1PTL.access.");
    }

    // ---------------- SPR2PTH-IS-WRITE-ONLY-AT-128 (p.291) ----------------
    {
        Harness h;
        h.write_reg(SPR2PTH_ADDR, 0x5555);
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t ctrl = h.read_reg(DMACONR_ADDR);
        bool ok_ctrl = h.read_ok;
        uint16_t at_addr = h.read_reg(SPR2PTH_ADDR);
        bool ok_addr = h.read_ok;

        checks.expect(
            ok_ctrl && ok_addr && ctrl == (SPREN | DMAEN) && at_addr == 0x0000,
            "SPR2PTH-IS-WRITE-ONLY-AT-128: after writing 0x5555 to SPR2PTH "
            "($128), a read AIMED AT $128 itself returns 0x0000, not "
            "0x5555 - while the SAME run's paired control (SPREN|DMAEN "
            "written to DMACON $096, read back through DMACONR $002) DOES "
            "return its own pattern. 'SPR2PTH + 128 w A Sprite 2 pointer "
            "(high 3 bits)' (p.291, access column corrected w->W per "
            "p.287's legend, see header comment). APPB2-SPR2PTH.acc, "
            "CH4-APPB-SPR2PTH.access.");
    }

    // ---------------- SPR2PTL-IS-WRITE-ONLY-AT-12A (p.291) ----------------
    {
        Harness h;
        h.write_reg(SPR2PTL_ADDR, 0x6666);
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t ctrl = h.read_reg(DMACONR_ADDR);
        bool ok_ctrl = h.read_ok;
        uint16_t at_addr = h.read_reg(SPR2PTL_ADDR);
        bool ok_addr = h.read_ok;

        checks.expect(
            ok_ctrl && ok_addr && ctrl == (SPREN | DMAEN) && at_addr == 0x0000,
            "SPR2PTL-IS-WRITE-ONLY-AT-12A: after writing 0x6666 to SPR2PTL "
            "($12A), a read AIMED AT $12A itself returns 0x0000, not "
            "0x6666 - while the SAME run's paired control (SPREN|DMAEN "
            "written to DMACON $096, read back through DMACONR $002) DOES "
            "return its own pattern. 'SPR2PTL + 12A w A Sprite 2 pointer "
            "(low 15 bits)' (p.291, access column corrected w->W per "
            "p.287's legend, see header comment). APPB2-SPR2PTL.acc, "
            "CH4-APPB-SPR2PTL.access.");
    }

    // ---------------- SPR3PTH-IS-WRITE-ONLY-AT-12C (p.291) ----------------
    {
        Harness h;
        h.write_reg(SPR3PTH_ADDR, 0x7777);
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t ctrl = h.read_reg(DMACONR_ADDR);
        bool ok_ctrl = h.read_ok;
        uint16_t at_addr = h.read_reg(SPR3PTH_ADDR);
        bool ok_addr = h.read_ok;

        checks.expect(
            ok_ctrl && ok_addr && ctrl == (SPREN | DMAEN) && at_addr == 0x0000,
            "SPR3PTH-IS-WRITE-ONLY-AT-12C: after writing 0x7777 to SPR3PTH "
            "($12C), a read AIMED AT $12C itself returns 0x0000, not "
            "0x7777 - while the SAME run's paired control (SPREN|DMAEN "
            "written to DMACON $096, read back through DMACONR $002) DOES "
            "return its own pattern. 'SPR3PTH + 12C w A Sprite 3 pointer "
            "(high 3 bits)' (p.291, access column corrected w->W per "
            "p.287's legend, see header comment). APPB2-SPR3PTH.acc, "
            "CH4-APPB-SPR3PTH.access.");
    }

    // ---------------- SPR3PTL-IS-WRITE-ONLY-AT-12E (p.291) ----------------
    {
        Harness h;
        h.write_reg(SPR3PTL_ADDR, 0x8888);
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t ctrl = h.read_reg(DMACONR_ADDR);
        bool ok_ctrl = h.read_ok;
        uint16_t at_addr = h.read_reg(SPR3PTL_ADDR);
        bool ok_addr = h.read_ok;

        checks.expect(
            ok_ctrl && ok_addr && ctrl == (SPREN | DMAEN) && at_addr == 0x0000,
            "SPR3PTL-IS-WRITE-ONLY-AT-12E: after writing 0x8888 to SPR3PTL "
            "($12E), a read AIMED AT $12E itself returns 0x0000, not "
            "0x8888 - while the SAME run's paired control (SPREN|DMAEN "
            "written to DMACON $096, read back through DMACONR $002) DOES "
            "return its own pattern. 'SPR3PTL + 12E w A Sprite 3 pointer "
            "(low 15 bits)' (p.291, access column corrected w->W per "
            "p.287's legend, see header comment). APPB2-SPR3PTL.acc, "
            "CH4-APPB-SPR3PTL.access.");
    }

    // ---------------- SPR4PTH-IS-WRITE-ONLY-AT-130 (p.291) ----------------
    {
        Harness h;
        h.write_reg(SPR4PTH_ADDR, 0x9999);
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t ctrl = h.read_reg(DMACONR_ADDR);
        bool ok_ctrl = h.read_ok;
        uint16_t at_addr = h.read_reg(SPR4PTH_ADDR);
        bool ok_addr = h.read_ok;

        checks.expect(
            ok_ctrl && ok_addr && ctrl == (SPREN | DMAEN) && at_addr == 0x0000,
            "SPR4PTH-IS-WRITE-ONLY-AT-130: after writing 0x9999 to SPR4PTH "
            "($130), a read AIMED AT $130 itself returns 0x0000, not "
            "0x9999 - while the SAME run's paired control (SPREN|DMAEN "
            "written to DMACON $096, read back through DMACONR $002) DOES "
            "return its own pattern. 'SPR4PTH + 130 w A Sprite 4 pointer "
            "(high 3 bits)' (p.291, access column corrected w->W per "
            "p.287's legend, see header comment). APPB2-SPR4PTH.acc, "
            "CH4-APPB-SPR4PTH.access.");
    }

    // ---------------- SPR4PTL-IS-WRITE-ONLY-AT-132 (p.291) ----------------
    {
        Harness h;
        h.write_reg(SPR4PTL_ADDR, 0xAAAA);
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t ctrl = h.read_reg(DMACONR_ADDR);
        bool ok_ctrl = h.read_ok;
        uint16_t at_addr = h.read_reg(SPR4PTL_ADDR);
        bool ok_addr = h.read_ok;

        checks.expect(
            ok_ctrl && ok_addr && ctrl == (SPREN | DMAEN) && at_addr == 0x0000,
            "SPR4PTL-IS-WRITE-ONLY-AT-132: after writing 0xAAAA to SPR4PTL "
            "($132), a read AIMED AT $132 itself returns 0x0000, not "
            "0xAAAA - while the SAME run's paired control (SPREN|DMAEN "
            "written to DMACON $096, read back through DMACONR $002) DOES "
            "return its own pattern. 'SPR4PTL + 132 w A Sprite 4 pointer "
            "(low 15 bits)' (p.291, access column corrected w->W per "
            "p.287's legend, see header comment). APPB2-SPR4PTL.acc, "
            "CH4-APPB-SPR4PTL.access.");
    }

    // ---------------- SPR5PTH-IS-WRITE-ONLY-AT-134 (p.291) ----------------
    {
        Harness h;
        h.write_reg(SPR5PTH_ADDR, 0xBBBB);
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t ctrl = h.read_reg(DMACONR_ADDR);
        bool ok_ctrl = h.read_ok;
        uint16_t at_addr = h.read_reg(SPR5PTH_ADDR);
        bool ok_addr = h.read_ok;

        checks.expect(
            ok_ctrl && ok_addr && ctrl == (SPREN | DMAEN) && at_addr == 0x0000,
            "SPR5PTH-IS-WRITE-ONLY-AT-134: after writing 0xBBBB to SPR5PTH "
            "($134), a read AIMED AT $134 itself returns 0x0000, not "
            "0xBBBB - while the SAME run's paired control (SPREN|DMAEN "
            "written to DMACON $096, read back through DMACONR $002) DOES "
            "return its own pattern. 'SPR5PTH + 134 w A Sprite 5 pointer "
            "(high 3 bits)' (p.291, access column corrected w->W per "
            "p.287's legend, see header comment). APPB2-SPR5PTH.acc, "
            "CH4-APPB-SPR5PTH.access.");
    }

    // ---------------- SPR5PTL-IS-WRITE-ONLY-AT-136 (p.291) ----------------
    {
        Harness h;
        h.write_reg(SPR5PTL_ADDR, 0xCCCC);
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t ctrl = h.read_reg(DMACONR_ADDR);
        bool ok_ctrl = h.read_ok;
        uint16_t at_addr = h.read_reg(SPR5PTL_ADDR);
        bool ok_addr = h.read_ok;

        checks.expect(
            ok_ctrl && ok_addr && ctrl == (SPREN | DMAEN) && at_addr == 0x0000,
            "SPR5PTL-IS-WRITE-ONLY-AT-136: after writing 0xCCCC to SPR5PTL "
            "($136), a read AIMED AT $136 itself returns 0x0000, not "
            "0xCCCC - while the SAME run's paired control (SPREN|DMAEN "
            "written to DMACON $096, read back through DMACONR $002) DOES "
            "return its own pattern. 'SPR5PTL + 136 w A Sprite 5 pointer "
            "(low 15 bits)' (p.291, access column corrected w->W per "
            "p.287's legend, see header comment). APPB2-SPR5PTL.acc, "
            "CH4-APPB-SPR5PTL.access.");
    }

    // ---------------- SPR6PTH-IS-WRITE-ONLY-AT-138 (p.291) ----------------
    {
        Harness h;
        h.write_reg(SPR6PTH_ADDR, 0xDDDD);
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t ctrl = h.read_reg(DMACONR_ADDR);
        bool ok_ctrl = h.read_ok;
        uint16_t at_addr = h.read_reg(SPR6PTH_ADDR);
        bool ok_addr = h.read_ok;

        checks.expect(
            ok_ctrl && ok_addr && ctrl == (SPREN | DMAEN) && at_addr == 0x0000,
            "SPR6PTH-IS-WRITE-ONLY-AT-138: after writing 0xDDDD to SPR6PTH "
            "($138), a read AIMED AT $138 itself returns 0x0000, not "
            "0xDDDD - while the SAME run's paired control (SPREN|DMAEN "
            "written to DMACON $096, read back through DMACONR $002) DOES "
            "return its own pattern. 'SPR6PTH + 138 w A Sprite 6 pointer "
            "(high 3 bits)' (p.291, access column corrected w->W per "
            "p.287's legend, see header comment). APPB2-SPR6PTH.acc, "
            "CH4-APPB-SPR6PTH.access.");
    }

    // ---------------- SPR6PTL-IS-WRITE-ONLY-AT-13A (p.291) ----------------
    {
        Harness h;
        h.write_reg(SPR6PTL_ADDR, 0xEEEE);
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t ctrl = h.read_reg(DMACONR_ADDR);
        bool ok_ctrl = h.read_ok;
        uint16_t at_addr = h.read_reg(SPR6PTL_ADDR);
        bool ok_addr = h.read_ok;

        checks.expect(
            ok_ctrl && ok_addr && ctrl == (SPREN | DMAEN) && at_addr == 0x0000,
            "SPR6PTL-IS-WRITE-ONLY-AT-13A: after writing 0xEEEE to SPR6PTL "
            "($13A), a read AIMED AT $13A itself returns 0x0000, not "
            "0xEEEE - while the SAME run's paired control (SPREN|DMAEN "
            "written to DMACON $096, read back through DMACONR $002) DOES "
            "return its own pattern. 'SPR6PTL + 13A w A Sprite 6 pointer "
            "(low 15 bits)' (p.291, access column corrected w->W per "
            "p.287's legend, see header comment). APPB2-SPR6PTL.acc, "
            "CH4-APPB-SPR6PTL.access.");
    }

    // ---------------- SPR7PTH-IS-WRITE-ONLY-AT-13C (p.291) ----------------
    {
        Harness h;
        h.write_reg(SPR7PTH_ADDR, 0x1234);
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t ctrl = h.read_reg(DMACONR_ADDR);
        bool ok_ctrl = h.read_ok;
        uint16_t at_addr = h.read_reg(SPR7PTH_ADDR);
        bool ok_addr = h.read_ok;

        checks.expect(
            ok_ctrl && ok_addr && ctrl == (SPREN | DMAEN) && at_addr == 0x0000,
            "SPR7PTH-IS-WRITE-ONLY-AT-13C: after writing 0x1234 to SPR7PTH "
            "($13C), a read AIMED AT $13C itself returns 0x0000, not "
            "0x1234 - while the SAME run's paired control (SPREN|DMAEN "
            "written to DMACON $096, read back through DMACONR $002) DOES "
            "return its own pattern. 'SPR7PTH + 13C w A Sprite 7 pointer "
            "(high 3 bits)' (p.291, access column corrected w->W per "
            "p.287's legend, see header comment). APPB2-SPR7PTH.acc, "
            "CH4-APPB-SPR7PTH.access.");
    }

    // ---------------- SPR7PTL-IS-WRITE-ONLY-AT-13E (p.291) ----------------
    {
        Harness h;
        h.write_reg(SPR7PTL_ADDR, 0x5678);
        h.write_reg(DMACON_ADDR, SET_CLR | SPREN | DMAEN);
        uint16_t ctrl = h.read_reg(DMACONR_ADDR);
        bool ok_ctrl = h.read_ok;
        uint16_t at_addr = h.read_reg(SPR7PTL_ADDR);
        bool ok_addr = h.read_ok;

        checks.expect(
            ok_ctrl && ok_addr && ctrl == (SPREN | DMAEN) && at_addr == 0x0000,
            "SPR7PTL-IS-WRITE-ONLY-AT-13E: after writing 0x5678 to SPR7PTL "
            "($13E), a read AIMED AT $13E itself returns 0x0000, not "
            "0x5678 - while the SAME run's paired control (SPREN|DMAEN "
            "written to DMACON $096, read back through DMACONR $002) DOES "
            "return its own pattern. 'SPR7PTL + 13E w A Sprite 7 pointer "
            "(low 15 bits)' (p.291, access column corrected w->W per "
            "p.287's legend, see header comment). APPB2-SPR7PTL.acc, "
            "CH4-APPB-SPR7PTL.access.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}

// ============================================================================
// MUTATION LOG (12.09.2026). Every entry below was actually applied by this
// agent - edited into this file, rebuilt under WSL/Verilator (`make test`),
// run, the PASS/FAIL line read from real output (quoted verbatim below, not
// predicted), then reverted and rebuilt clean before the next entry.
// Baseline before this log's first mutation and final clean run after the
// last revert were both "17 passed, 0 failed".
//
// All seventeen assertions get the SAME shape of stimulus mutation, for the
// same reason hrm/ch4-sprite-dma-control's own mutation #2 does: retarget
// one read to the address that IS live, so a check built to notice "this
// address stays silent while that one doesn't" instead sees both addresses
// agree - the module is being asked a different question, the expected
// value is untouched.
//
// 0. FIXTURE-DELIVERS-KNOWN-WORD-VIA-DMACONR: the first DMACON write's
//    target address changed from DMACON_ADDR (0x096) to 0x098, a nearby
//    but non-decoded address (agnus.v's write decode is
//    reg_address[8:1]==DMACON[8:1], and 0x098[8:1] != 0x096[8:1]). Result:
//    "16 passed, 1 failed" - FIXTURE-DELIVERS-KNOWN-WORD-VIA-DMACONR went
//    red (after_a read back 0x0000, not SPREN|DMAEN, since the write
//    landed nowhere DMACONR reads from). Reverted; rebuilt clean (17
//    passed, 0 failed).
//
// 1-16. <REGISTER>-IS-WRITE-ONLY-AT-<ADDR>: the suspect read's own target
//    address (`at_addr = h.read_reg(SPRxPTx_ADDR)`) changed to
//    DMACONR_ADDR (0x002), so the check reads the live control register
//    twice instead of reading the sprite address under test at all. Every
//    one of the sixteen went red exactly the same way - `at_addr` picked
//    up DMACONR's own live pattern (SPREN|DMAEN, non-zero) instead of the
//    sprite address's silence (0x0000), so `at_addr == 0x0000` failed -
//    and every other of the sixteen assertions (each its own fresh
//    Harness instance, untouched by this file edit) stayed green,
//    confirming the mutation is local to the one assertion whose read
//    call was edited. Reverted after each; rebuilt clean between every
//    one (confirmed "17 passed, 0 failed" before moving to the next).
//    Real output, one representative in full and the rest by their
//    unique FAIL line (all identical in shape, `16 passed, 1 failed`):
//      SPR0PTH-IS-WRITE-ONLY-AT-120 -> "16 passed, 1 failed" / "FAIL:
//        SPR0PTH-IS-WRITE-ONLY-AT-120: ... returns 0x0000, not 0x1111 ..."
//      SPR0PTL-IS-WRITE-ONLY-AT-122 -> "16 passed, 1 failed" / "FAIL:
//        SPR0PTL-IS-WRITE-ONLY-AT-122: ... returns 0x0000, not 0x2222 ..."
//      SPR1PTH-IS-WRITE-ONLY-AT-124 -> "16 passed, 1 failed" / "FAIL:
//        SPR1PTH-IS-WRITE-ONLY-AT-124: ... returns 0x0000, not 0x3333 ..."
//      SPR1PTL-IS-WRITE-ONLY-AT-126 -> "16 passed, 1 failed" / "FAIL:
//        SPR1PTL-IS-WRITE-ONLY-AT-126: ... returns 0x0000, not 0x4444 ..."
//      SPR2PTH-IS-WRITE-ONLY-AT-128 -> "16 passed, 1 failed" / "FAIL:
//        SPR2PTH-IS-WRITE-ONLY-AT-128: ... returns 0x0000, not 0x5555 ..."
//      SPR2PTL-IS-WRITE-ONLY-AT-12A -> "16 passed, 1 failed" / "FAIL:
//        SPR2PTL-IS-WRITE-ONLY-AT-12A: ... returns 0x0000, not 0x6666 ..."
//      SPR3PTH-IS-WRITE-ONLY-AT-12C -> "16 passed, 1 failed" / "FAIL:
//        SPR3PTH-IS-WRITE-ONLY-AT-12C: ... returns 0x0000, not 0x7777 ..."
//      SPR3PTL-IS-WRITE-ONLY-AT-12E -> "16 passed, 1 failed" / "FAIL:
//        SPR3PTL-IS-WRITE-ONLY-AT-12E: ... returns 0x0000, not 0x8888 ..."
//      SPR4PTH-IS-WRITE-ONLY-AT-130 -> "16 passed, 1 failed" / "FAIL:
//        SPR4PTH-IS-WRITE-ONLY-AT-130: ... returns 0x0000, not 0x9999 ..."
//      SPR4PTL-IS-WRITE-ONLY-AT-132 -> "16 passed, 1 failed" / "FAIL:
//        SPR4PTL-IS-WRITE-ONLY-AT-132: ... returns 0x0000, not 0xAAAA ..."
//      SPR5PTH-IS-WRITE-ONLY-AT-134 -> "16 passed, 1 failed" / "FAIL:
//        SPR5PTH-IS-WRITE-ONLY-AT-134: ... returns 0x0000, not 0xBBBB ..."
//      SPR5PTL-IS-WRITE-ONLY-AT-136 -> "16 passed, 1 failed" / "FAIL:
//        SPR5PTL-IS-WRITE-ONLY-AT-136: ... returns 0x0000, not 0xCCCC ..."
//      SPR6PTH-IS-WRITE-ONLY-AT-138 -> "16 passed, 1 failed" / "FAIL:
//        SPR6PTH-IS-WRITE-ONLY-AT-138: ... returns 0x0000, not 0xDDDD ..."
//      SPR6PTL-IS-WRITE-ONLY-AT-13A -> "16 passed, 1 failed" / "FAIL:
//        SPR6PTL-IS-WRITE-ONLY-AT-13A: ... returns 0x0000, not 0xEEEE ..."
//      SPR7PTH-IS-WRITE-ONLY-AT-13C -> "16 passed, 1 failed" / "FAIL:
//        SPR7PTH-IS-WRITE-ONLY-AT-13C: ... returns 0x0000, not 0x1234 ..."
//      SPR7PTL-IS-WRITE-ONLY-AT-13E -> "16 passed, 1 failed" / "FAIL:
//        SPR7PTL-IS-WRITE-ONLY-AT-13E: ... returns 0x0000, not 0x5678 ..."
//    (Each FAIL line above is the exact assertion label truncated with
//    "..." for length in this log only; the file's own checks.expect()
//    calls above carry the untruncated text, unedited by this log.)
//
// Two of these seventeen rebuilds (SPR1PTH-IS-WRITE-ONLY-AT-124 and
// SPR2PTL-IS-WRITE-ONLY-AT-12A) hit a transient WSL/DrvFs error on their
// first attempt - `rm: cannot remove 'obj_dir/...': Permission denied` /
// `make: *** [Makefile:15: test] Error 2` - unrelated to this file's
// content (the source edit was byte-identical to its sibling mutations,
// which built and ran cleanly). Both were re-run individually
// immediately after and produced the "16 passed, 1 failed" result quoted
// above; the source was reverted after the failed attempt exactly as
// after every other entry, so no stale mutation was left in place between
// attempts.
//
// Machine-readable records, one per line, full leading id immediately
// followed by the kind token:
// MUTATION FIXTURE-DELIVERS-KNOWN-WORD-VIA-DMACONR stimulus red: first DMACON write's target address moved from $096 to $098 (not decoded by agnus.v's DMACON path); after_a read back 0x0000, not SPREN|DMAEN.
// MUTATION SPR0PTH-IS-WRITE-ONLY-AT-120 stimulus red: the read aimed at $120 was retargeted to $002 (DMACONR); at_addr came back SPREN|DMAEN instead of 0x0000.
// MUTATION SPR0PTL-IS-WRITE-ONLY-AT-122 stimulus red: the read aimed at $122 was retargeted to $002 (DMACONR); at_addr came back SPREN|DMAEN instead of 0x0000.
// MUTATION SPR1PTH-IS-WRITE-ONLY-AT-124 stimulus red: the read aimed at $124 was retargeted to $002 (DMACONR); at_addr came back SPREN|DMAEN instead of 0x0000.
// MUTATION SPR1PTL-IS-WRITE-ONLY-AT-126 stimulus red: the read aimed at $126 was retargeted to $002 (DMACONR); at_addr came back SPREN|DMAEN instead of 0x0000.
// MUTATION SPR2PTH-IS-WRITE-ONLY-AT-128 stimulus red: the read aimed at $128 was retargeted to $002 (DMACONR); at_addr came back SPREN|DMAEN instead of 0x0000.
// MUTATION SPR2PTL-IS-WRITE-ONLY-AT-12A stimulus red: the read aimed at $12A was retargeted to $002 (DMACONR); at_addr came back SPREN|DMAEN instead of 0x0000.
// MUTATION SPR3PTH-IS-WRITE-ONLY-AT-12C stimulus red: the read aimed at $12C was retargeted to $002 (DMACONR); at_addr came back SPREN|DMAEN instead of 0x0000.
// MUTATION SPR3PTL-IS-WRITE-ONLY-AT-12E stimulus red: the read aimed at $12E was retargeted to $002 (DMACONR); at_addr came back SPREN|DMAEN instead of 0x0000.
// MUTATION SPR4PTH-IS-WRITE-ONLY-AT-130 stimulus red: the read aimed at $130 was retargeted to $002 (DMACONR); at_addr came back SPREN|DMAEN instead of 0x0000.
// MUTATION SPR4PTL-IS-WRITE-ONLY-AT-132 stimulus red: the read aimed at $132 was retargeted to $002 (DMACONR); at_addr came back SPREN|DMAEN instead of 0x0000.
// MUTATION SPR5PTH-IS-WRITE-ONLY-AT-134 stimulus red: the read aimed at $134 was retargeted to $002 (DMACONR); at_addr came back SPREN|DMAEN instead of 0x0000.
// MUTATION SPR5PTL-IS-WRITE-ONLY-AT-136 stimulus red: the read aimed at $136 was retargeted to $002 (DMACONR); at_addr came back SPREN|DMAEN instead of 0x0000.
// MUTATION SPR6PTH-IS-WRITE-ONLY-AT-138 stimulus red: the read aimed at $138 was retargeted to $002 (DMACONR); at_addr came back SPREN|DMAEN instead of 0x0000.
// MUTATION SPR6PTL-IS-WRITE-ONLY-AT-13A stimulus red: the read aimed at $13A was retargeted to $002 (DMACONR); at_addr came back SPREN|DMAEN instead of 0x0000.
// MUTATION SPR7PTH-IS-WRITE-ONLY-AT-13C stimulus red: the read aimed at $13C was retargeted to $002 (DMACONR); at_addr came back SPREN|DMAEN instead of 0x0000.
// MUTATION SPR7PTL-IS-WRITE-ONLY-AT-13E stimulus red: the read aimed at $13E was retargeted to $002 (DMACONR); at_addr came back SPREN|DMAEN instead of 0x0000.
// ============================================================================
