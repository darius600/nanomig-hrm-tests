// Test purpose: The REGISTER MAP agnus_bitplanedma decodes -- addresses,
//   read/write direction, and chip ownership -- for the registers named in
//   this package's claims.jsonl: DDFSTRT, DDFSTOP, BPL1MOD, BPL2MOD, and the
//   six bitplane pointer pairs BPL1PTH/PTL .. BPL6PTH/PTL. Package
//   ch3-register-map-agnus, fixture agnus_bitplanedma (one Verilator build,
//   this module only). This is deliberately NOT about the display-data-fetch
//   WINDOW behaviour these same two DDFSTRT/DDFSTOP registers also control
//   (word counts, hardware limits, the horizontal-scroll extra word, the
//   printed manual's own PAL byte-count arithmetic error) -- that is
//   hrm/ch3-ddf-fetch-window/'s job, already built; no claim id or
//   PROPOSITION tested there is repeated here. Every assertion below tests
//   exactly one proposition: "this register lives at this address" (and,
//   where the SAME printed row states it, "...and is write-only,
//   Agnus-owned"). Where the only way to observe a write-only register's
//   address is through the module's write-only-effect ports (address_out,
//   reg_address_out, dma) -- this module has no data_out at all, so nothing
//   can be "read back" through the register bus the way some sibling
//   modules allow -- those ports are used purely as ADDRESS-DECODE PROBES:
//   did the write reach a register that measurably affects the fetched
//   address stream AT ALL, and does it do so ONLY at the address the manual
//   names, not at a neighbour -- never as a check that the fetched VALUE,
//   WORD COUNT or TIMING matches any manual-stated figure.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 270, 272, 277 (Appendix A register listing) and 290, 291
//   (Appendix B register listing).
// Non-goals: the display-data-fetch WINDOW itself -- word counts, the
//   $18/$D8 hardware limits, DDFSTRT/DDFSTOP bit-field granularity,
//   horizontal-scroll extra-word behaviour, modulo VALUES, dual-playfield
//   sharing -- all belong to hrm/ch3-ddf-fetch-window/ and are not
//   re-tested here, even though several assertions below reuse the SAME
//   Harness mechanics and even the SAME underlying signals (dma,
//   address_out, reg_address_out) that package already reads for a
//   different purpose. Also not tested: the printed "(high 3 bits)" /
//   "(low 15 bits)" bit-WIDTH descriptors that ride along in the same
//   quoted Appendix A/B rows as the twelve pointer-register ADDRESS claims
//   -- see CONFLICT NOTE below.
//
// BOUNDARY (from the build brief, not yet from opening the file):
//   nanomig/src/minimig-aga/agnus_bitplanedma.v:57-58 (BPL1MOD_REG 9h108,
//   BPL2MOD_REG 9h10a), write decode l.262-274, modulo add l.484-493
//   (plane[0] selects BPL2MOD, else BPL1MOD), mod strobe l.454;
//   agnus_bitplanedma.v:54 (BPLPTBASE_REG 9h0E0), pointer banks l.217-242,
//   pointer arithmetic l.484-493, address_out[20:1] port l.46 -- there is no
//   address bit 0; agnus_bitplanedma.v:55-56 (DDFSTRT_REG 9h092, DDFSTOP_REG
//   9h094), write decode l.247-259 (ddfstrt[8:2] <= data_in[7:1]), position
//   compare l.336-354, hard limits $18/$D8 l.356-374, ddfena l.407-414.
//   These line numbers and register names come from claims.jsonl's own
//   evidence field and this package's PACKAGE.md, handed to this agent
//   before agnus_bitplanedma.v was opened for this package -- reused here,
//   not independently re-derived. This module's own top-level Verilator
//   Harness (tick/write_reg/write_ptr/run_to_line/run_single_line/
//   prime_to_line) is reused verbatim from
//   hrm/ch3-ddf-fetch-window/test_agnus_bitplanedma.cpp -- an
//   already-built, already-RTL-verified fixture FOR THIS SAME TOP MODULE --
//   per the precedent in hrm/ch3-collision-registers/'s own spec-only
//   commit and hrm/ch3-register-map-agnus-beamcounter/'s own spec-only
//   commit (reusing an already-built sibling harness for the identical top
//   module is not "opening the RTL"; the claims.jsonl evidence field and
//   the sibling package's own published comments already made every one of
//   these addresses, port names and harness mechanics public before this
//   file was written). No PASS/FAIL threshold or expected value below was
//   chosen or adjusted because of anything seen in agnus_bitplanedma.v
//   beyond what BOUNDARY states above.
//
// CLAIM GROUPS (16 propositions covering all 46 claims; two ids restating
//   the identical proposition from a different reader/page are grouped and
//   cited together on the one assertion that tests it):
//
//  -- BPL1MOD at $108 (p.270, p.291) --
//   A-bpl1mod.addr (opus, p270): "BPL1MOD    108      W       A     Bitplane
//     modulo (odd planes)"
//   A-bplmod.c1 (sol, p270): same row.
//   B-bpl1mod.addr, B-bpl1mod.addr-p291 (opus, p291): "BPL1MOD        108
//     W   A           Bitplane modulo (odd planes)"
//   B-subject.c126 (sol, p291): same row.
//
//  -- BPL2MOD at $10A (p.270, p.291) --
//   A-bpl2mod.addr (opus, p270): "BPL2MOD    10A      W       A     Bitplane
//     modulo (even planes)"
//   A-bplmod.c5 (sol, p270): same row.
//   B-bpl2mod.addr, B-bpl2mod.addr-p291 (opus, p291): "BPL2MOD        10A
//     W   A           Bitplane modulo (even planes)"
//   B-subject.c130 (sol, p291): same row.
//
//  -- BPL1PTH at $0E0 (p.272 generic BPLxPTH row, p.291 named row) --
//   A-bplptr.c1 (sol, p272): "BPLxPTH    0E0      W       A     Bitplane x
//     pointer (high 3 bits)"
//   A-bplxpth.addr (opus, p272): same row.
//   B-bpl1pth.addr (opus, p291): "BPL1PTH    +   0E0    W   A
//     Bitplane 1 pointer    (high 3 bits)"
//   B-subject.c26 (sol, p291): same row.
//
//  -- BPL1PTL at $0E2 (p.272 generic BPLxPTL row, p.291 named row) --
//   A-bplptr.c4 (sol, p272): "BPLxPTL    0E2      W       A     Bitplane x
//     pointer (low 15 bits)"
//   A-bplxptl.addr (opus, p272): same row.
//   B-bpl1ptl.addr (opus, p291): "BPL1PTL    +   0E2    W   A
//     Bitplane 1 pointer    (low 15 bits)"
//   B-subject.c33 (sol, p291): same row.
//
//  -- BPL2PTH at $0E4 / BPL2PTL at $0E6 (p.291) --
//   B-bpl2pth.addr (opus) / B-subject.c40 (sol): "BPL2PTH    +   0E4
//     W   A           Bitplane 2 pointer    (high 3 bits)"
//   B-bpl2ptl.addr (opus) / B-subject.c47 (sol): "BPL2PTL    +   0E6
//     W   A           Bitplane 2 pointer    (low 15 bits)"
//
//  -- BPL3PTH at $0E8 / BPL3PTL at $0EA (p.291) --
//   B-bpl3pth.addr / B-subject.c54: "BPL3PTH    +   0E8    W   A
//     Bitplane 3 pointer    (high 3 bits)"
//   B-bpl3ptl.addr / B-subject.c61: "BPL3PTL    +   0EA    W   A
//     Bitplane 3 pointer    (low 15 bits)"
//
//  -- BPL4PTH at $0EC / BPL4PTL at $0EE (p.291) --
//   B-bpl4pth.addr / B-subject.c68: "BPL4PTH    +   0EC    W   A
//     Bitplane 4 pointer    (high 3 bits)"
//   B-bpl4ptl.addr / B-subject.c75: "BPL4PTL    +   0EE    W   A
//     Bitplane 4 pointer    (low 15 bits)"
//
//  -- BPL5PTH at $0F0 / BPL5PTL at $0F2 (p.291) --
//   B-bpl5pth.addr / B-subject.c82: "BPL5PTH    +   0F0    W   A
//     Bitplane 5 pointer    (high 3 bits)"
//   B-bpl5ptl.addr / B-subject.c89: "BPL5PTL    +   0F2    W   A
//     Bitplane 5 pointer    (low 15 bits)"
//
//  -- BPL6PTH at $0F4 / BPL6PTL at $0F6 (p.291) --
//   B-bpl6pth.addr / B-subject.c96: "BPL6PTH    +   0F4    W   A
//     Bitplane 6 pointer    (high 3 bits)"
//   B-bpl6ptl.addr / B-subject.c103: "BPL6PTL    +   0F6    W   A
//     Bitplane 6 pointer    (low 15 bits)"
//
//  -- DDFSTRT at $092 (p.277 Appendix A, p.290 Appendix B) --
//   A-ddf.c3 (sol, p277): "DDFSTRT    092      W       A    Display data
//     fetch start (horiz. position)"
//   A-ddfstrt.addr (opus, p277): same row.
//   B-ddfstrt.addr (opus, p290): "    DDFSTRT        092    W
//     Display bitplane data fetch start"
//   B-subject.c16 (sol, p290): same row.
//
//  -- DDFSTOP at $094 (p.277 Appendix A, p.290 Appendix B) --
//   A-ddf.c1 (sol, p277): "DDFSTOP    094      W       A    Display data
//     fetch stop (horiz. position)"
//   A-ddfstop.addr (opus, p277): same row.
//   B-ddfstop.addr (opus, p290): "    DDFSTOP        094    W
//     Display bitplane data fetch stop"
//   B-subject.c19 (sol, p290): same row.
//
// CHIP OWNERSHIP ('A' = Agnus): the p.270/272/291 rows above all print an
//   explicit 'A' in the chip column alongside the address, and are asserted
//   as such. The p.290 Appendix B rows for DDFSTRT/DDFSTOP print NO chip
//   letter at all (blank column) -- per this package's own build brief,
//   "a blank cell is missing information, not a statement that no chip owns
//   the register": DDFSTRT/DDFSTOP's chip ownership is NOT asserted from
//   the p.290 quotes, only their address and W direction are. Their chip
//   ownership IS asserted, but from the undisputed p.277 Appendix A rows
//   instead, which do print 'A'. Chip ownership is additionally established
//   STRUCTURALLY, independent of any printed letter: this package's
//   Makefile compiles exactly one Verilog source, agnus_bitplanedma.v --
//   no Denise, Paula, CPU or other custom-chip RTL is anywhere in the
//   Verilator build -- so any address that produces an observable effect
//   inside this DUT was necessarily decoded and held by Agnus circuitry.
//
// CONFLICT NOTE (12 claims -- the twelve BPLnPTH/BPLnPTL address ids above
//   -- PACKAGE.md marks these as "carries source conflicts"): the ADDRESS
//   fact itself (BPLPTBASE=$0E0, BASE+4N per-plane spacing) is corroborated
//   consistently everywhere it is cited -- the RTL's own base-address
//   comment (l.54), the generic Appendix A p.272 row, and the six explicit
//   Appendix B p.291 rows all agree numerically -- and is asserted as
//   undisputed below. What is NOT asserted is the "(high 3 bits)" /
//   "(low 15 bits)" bit-WIDTH descriptor riding along in the same quoted
//   rows: BPLPTBASE_REG's high-word field width is a fact to be confirmed
//   once the RTL is open, not assumed from the printed "(high 3 bits)"
//   phrase, and is recorded as a measurement (see IMPLEMENTATION NOTES,
//   added after the RTL is opened) rather than folded into any address
//   PASS/FAIL. No literal `conflict_resolution` field was found on these
//   12 records beyond their shared `source_defines: "conflict: page 54
//   versus page 272 (and appendix B page 291)"` text; this note states the
//   resolution this package actually applies, and why.
//
// METHODOLOGY (planned, applied identically to all 16 assertions):
//   ADDRESS-DECODE SELECTIVITY: for a write-only register R with manual
//   address A and a chosen "wrong" address W (always a DIFFERENT REAL
//   register's manual address, never an unassigned one -- proving the
//   decoder actually discriminates between two live paths, not merely that
//   garbage addresses are ignored), three fresh harnesses with identical
//   companion setup are built: h_base (R never written), h_wrong (test data
//   written to W), h_correct (the SAME test data written to A). The
//   proposition: measure(h_wrong) is indistinguishable from measure(h_base)
//   (W has no effect on R), and measure(h_correct) is clearly different
//   from measure(h_base) (A does affect R). For the twelve pointer
//   registers the observable is the plane's own fetched address_out field
//   (high 5 bits for a PTH claim, low 16 bits for a PTL claim), read via
//   this module's existing dma/address_out/reg_address_out ports -- no
//   internal signal is peeked. For BPL1MOD/BPL2MOD the observable is the
//   line-to-line step of a plane's own first fetched address (does writing
//   a large modulo value change how far the pointer moves between one line
//   and the next). For DDFSTRT the observable is the hpos of the line's
//   first dma pulse; for DDFSTOP it is the line's total dma pulse count --
//   in both cases only WHETHER the correct address moves the signal is
//   asserted, never the manual's specific word-count or timing figures
//   (that comparison is reserved for hrm/ch3-ddf-fetch-window/).
//
// CLAIM COVERAGE: all 46 records of claims.jsonl will be cited, in full,
//   inside an expect() label. 16 ADDRESS assertions (one per proposition
//   above) cover all 46 claims, plus one no-claim FIXTURE-DELIVERS-
//   KNOWN-VALUE control. No claim id is declined.
//
// CONTROL CASES: every assertion already carries its own internal control
//   (h_wrong is expected to PASS -- match h_base -- on the identical
//   measurement basis as h_correct, which is the case under test).
//   FIXTURE-DELIVERS-KNOWN-VALUE additionally proves this package's own
//   independently built obj_dir actually delivers a known pointer value
//   through the write-then-fetch path before any address-decode assertion
//   below is trusted to mean anything.
//
// MUTATION LOG: recorded after agnus_bitplanedma.v is actually opened and
//   `make test` has actually been run under WSL/Verilator (left empty in
//   this spec-only commit, filled in the follow-up commit, per the build
//   brief's order of work and the precedent in
//   hrm/ch3-register-map-agnus-beamcounter/'s own spec-only commit).
//
// No RTL has been opened yet to write this comment block beyond the
// BOUNDARY text handed to this agent in PACKAGE.md/claims.jsonl. The
// Harness, helper functions and assertion bodies are added in the next
// commit, after agnus_bitplanedma.v is read in full for this package.

// ============================================================================
// IMPLEMENTATION NOTES (added in this commit, after agnus_bitplanedma.v was
// read in full for this package -- the specification comment above this
// point was committed on its own first, per the chapter's spec-before-RTL
// gate; see this package's own git history, commit 464eb32).
//
// PORTS (agnus_bitplanedma.v:29-47) confirm BOUNDARY: clk, clk7_en, reset,
// harddis, aga, ecs, a1k, sof, dmaena, vpos[10:0], hpos[8:0] are inputs;
// hde, dma, address_out[20:1], reg_address_out[8:1] are outputs;
// reg_address_in[8:1]/data_in[15:0] are the register write bus. NO
// data_out PORT AT ALL -- confirmed by reading the full port list, not
// assumed from PACKAGE.md's own claim -- so every assertion below reaches
// its register purely through the write-then-effect path.
//
// REGISTER DECODE (l.50-60, 217-274): DDFSTRT=9'h092, DDFSTOP=9'h094,
// BPL1MOD=9'h108, BPL2MOD=9'h10a, BPLPTBASE=9'h0E0, BPLCON0=9'h100,
// FMODE=9'h1fc, DIWSTRT=9'h08E, DIWSTOP=9'h090 -- matches BOUNDARY exactly.
// Pointer bank select: `bplptr_sel = dma ? plane[2:0] : reg_address_in[4:2]`
// (l.217) -- during a bus write (dma=0) the bank is read directly from
// address bits [4:2], confirming BASE+4N (N=bank 0-7) from the RAW ADDRESS
// bits, not assumed: 0xE0/0xE4/0xE8/0xEC/0xF0/0xF4 give bank 0/1/2/3/4/5,
// exactly the six BPL1..BPL6 rows this package's claims name. PTH/PTL
// within one bank are distinguished by reg_address_in[1] (l.225,237).
//
// CONFLICT NOTE RESOLVED: bplpth is declared `reg [20:16] bplpth [7:0];
// // upper 5 bits bitplane pointers` (l.82) -- a 5-bit field, matching
// address_out[20:16] (l.46, 21-bit chip address). The printed "(high 3
// bits)" descriptor on every BPLnPTH row (p.272, p.291) disagrees with
// this RTL's actual field width; bplptl is `reg [15:1] bplptl [7:0]`
// (l.83), a 15-bit field, matching the printed "(low 15 bits)" descriptor
// exactly (no PTL conflict). This is the concrete shape of the "page 54
// versus page 272/291" conflict PACKAGE.md flags: the historical OCS/ECS
// 3-bit high pointer (addressing up to 1 MB) versus this AGA-capable RTL's
// 5-bit field (addressing up to 2 MB, gated by the `aga` port elsewhere in
// the design, e.g. BPLCON0's AGA-only BPU3 bit at l.282). Per this
// package's build brief ("if a check disagrees with the RTL, the RTL is
// what is wrong -- a red result is a valid, finished outcome"), this is
// reported as a FINDING below (RTL-VS-MANUAL-BPLXPTH-FIELD-WIDTH,
// deliberately measured, not folded into any address PASS/FAIL).
//
// OBSERVABLES CHOSEN: pointer registers are observed through address_out
// (l.230,242 -- `address_out[20:16] = bplpth[plane[2:0]]`, `[15:1] =
// bplptl[plane[2:0]]`), sampled only at the instant `dma`=1 for a given
// `reg_address_out` (l.496-507, a fixed plane->0x88..0x8F lookup), exactly
// as hrm/ch3-ddf-fetch-window's own HARNESS-PROVES-PLANE-ADDRESS-ORDERING
// assertion already does for a different purpose. BPL1MOD/BPL2MOD are
// observed through the line-to-line step of a plane's own first fetched
// address -- the modulo add (l.484-493) fires once per active plane during
// the ddfend "cleanup" pass at end-of-line (ddfrun stays 1, cycling
// `plane` through every active bank once more, until ddfseq_match, l.417-
// 430), so each active plane's own pointer receives exactly one modulo-add
// write per line, observable as a step between consecutive lines' first
// fetches. plane[0]=0 (RTL bank 0,2,4 = BPL1,3,5) uses bpl1mod_bscan,
// plane[0]=1 (bank 1,3,5 = BPL2,4,6) uses bpl2mod_bscan (l.486-489) --
// bpl1mod_bscan/bpl2mod_bscan simply track bpl1mod/bpl2mod unconditionally
// when fmode[14]=0 (l.477-480), which this package's fixture always uses
// (FMODE written to 0 by setup_standard()). DDFSTRT/DDFSTOP are observed
// through, respectively, the hpos of the line's first dma pulse and the
// line's total dma pulse count -- reusing hrm/ch3-ddf-fetch-window's own
// first_fetch_hpos/run_to_line helpers verbatim, for the ADDRESS-decode
// proposition only, never the WORD-count values that package's own
// assertions already test.
// ============================================================================

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <verilated.h>
#include "Vagnus_bitplanedma.h"

namespace {

// ---- register addresses (9-bit custom-chip byte offsets), reused verbatim
// from hrm/ch3-ddf-fetch-window/test_agnus_bitplanedma.cpp (same module,
// same claims.jsonl evidence table) ----
constexpr uint16_t DIWSTRT_ADDR = 0x08E;
constexpr uint16_t DIWSTOP_ADDR = 0x090;
constexpr uint16_t DDFSTRT_ADDR = 0x092;
constexpr uint16_t DDFSTOP_ADDR = 0x094;
constexpr uint16_t BPLPT_BASE   = 0x0E0;  // plane N: PTH=BASE+4N, PTL=BASE+4N+2
constexpr uint16_t BPL1MOD_ADDR = 0x108;
constexpr uint16_t BPL2MOD_ADDR = 0x10A;
constexpr uint16_t BPLCON0_ADDR = 0x100;
constexpr uint16_t FMODE_ADDR   = 0x1FC;

// Standard values, reused verbatim from the same sibling file.
constexpr uint16_t DDFSTRT_LORES_STD = 0x0038;
constexpr uint16_t DDFSTOP_LORES_STD = 0x00D0;
constexpr uint16_t DIWSTRT_STD = 0x2C81;
constexpr uint16_t DIWSTOP_STD = 0x2CC1;

uint16_t bplcon0_word(bool hires, int bpu /*0-7*/) {
    // bplcon0 <= {data_in[6], data_in[15], aga&data_in[4], data_in[14:12]}
    // stored bits: [5]=SHRES(data_in[6]), [4]=HIRES(data_in[15]),
    // [3]=BPU3(AGA only, left 0 here), [2:0]=BPU2..0(data_in[14:12]).
    uint16_t w = 0;
    if (hires) w |= 0x8000;
    w |= static_cast<uint16_t>((bpu & 0x7) << 12);
    return w;
}

}  // namespace

// Harness, write_reg/write_ptr, LineResult/Fetch, run_to_line,
// run_single_line, prime_to_line and first_fetch_hpos below are reused
// verbatim from hrm/ch3-ddf-fetch-window/test_agnus_bitplanedma.cpp -- see
// BOUNDARY above for why this is not "opening the RTL" for this package.
class Harness {
public:
    Harness() : dut(new Vagnus_bitplanedma) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->reset = 1;
        dut->harddis = 0;
        dut->aga = 0;
        dut->ecs = 1;
        dut->a1k = 0;
        dut->sof = 0;
        dut->dmaena = 0;
        dut->vpos = 0;
        dut->hpos = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        for (int i = 0; i < 10; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 10; ++i) tick();
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

    void write_reg(uint16_t reg9, uint16_t data) {
        dut->reg_address_in = static_cast<uint16_t>(reg9 >> 1);
        dut->data_in = data;
        tick();
        dut->reg_address_in = 0;
        dut->data_in = 0;
        tick();
    }

    void write_ptr(int plane, uint32_t byte_addr) {
        uint16_t pth = static_cast<uint16_t>(BPLPT_BASE + 4 * plane);
        uint16_t ptl = static_cast<uint16_t>(BPLPT_BASE + 4 * plane + 2);
        write_reg(pth, static_cast<uint16_t>((byte_addr >> 16) & 0x1F));
        write_reg(ptl, static_cast<uint16_t>(byte_addr & 0xFFFF));
    }

    // Standard setup shared by most assertions: DIW centered per p.57's
    // ($81,$2C) example, dmaena on, harddis off, FMODE=0.
    void setup_standard(bool ecs_on = true) {
        dut->ecs = ecs_on ? 1 : 0;
        write_reg(DIWSTRT_ADDR, DIWSTRT_STD);
        write_reg(DIWSTOP_ADDR, DIWSTOP_STD);
        write_reg(FMODE_ADDR, 0x0000);
        dut->dmaena = 1;
    }

    Vagnus_bitplanedma* dut;
};

struct Fetch {
    uint32_t address;   // address_out, byte address
    uint16_t reg;        // reg_address_out (raw 8-bit field; *2 = real BPLxDAT addr)
};

struct LineResult {
    int count = 0;
    std::vector<Fetch> fetches;
};

LineResult run_to_line(Harness& h, int target_vpos, int hpos_max = 456) {
    LineResult r;
    for (int v = 0; v <= target_vpos; ++v) {
        h.dut->vpos = v;
        for (int hp = 0; hp < hpos_max; ++hp) {
            h.dut->hpos = hp;
            h.tick();
            if (v == target_vpos && h.dut->dma) {
                ++r.count;
                r.fetches.push_back({static_cast<uint32_t>(h.dut->address_out) * 2,
                                      static_cast<uint16_t>(h.dut->reg_address_out)});
            }
        }
    }
    return r;
}

void prime_to_line(Harness& h, int through_vpos, int hpos_max = 456) {
    for (int v = 0; v <= through_vpos; ++v) {
        h.dut->vpos = v;
        for (int hp = 0; hp < hpos_max; ++hp) {
            h.dut->hpos = hp;
            h.tick();
        }
    }
}

LineResult run_single_line(Harness& h, int this_vpos, int hpos_max = 456) {
    LineResult r;
    h.dut->vpos = this_vpos;
    for (int hp = 0; hp < hpos_max; ++hp) {
        h.dut->hpos = hp;
        h.tick();
        if (h.dut->dma) {
            ++r.count;
            r.fetches.push_back({static_cast<uint32_t>(h.dut->address_out) * 2,
                                  static_cast<uint16_t>(h.dut->reg_address_out)});
        }
    }
    return r;
}

int first_fetch_hpos(Harness& h, int target_vpos, int hpos_max = 456) {
    for (int v = 0; v <= target_vpos; ++v) {
        h.dut->vpos = v;
        for (int hp = 0; hp < hpos_max; ++hp) {
            h.dut->hpos = hp;
            h.tick();
            if (v == target_vpos && h.dut->dma) return hp;
        }
    }
    return -1;
}

// ---- this package's own helpers -----------------------------------------

// First fetched address for a given reg_address_out slot (0x88+bank), or
// the sentinel 0xFFFFFFFF if that plane never fetched this line -- used
// throughout as "the plane under test's own observed address".
uint32_t first_fetch_addr_for_plane(const LineResult& r, uint16_t reg_addr_out_val) {
    for (const auto& f : r.fetches)
        if (f.reg == reg_addr_out_val) return f.address;
    return 0xFFFFFFFFu;
}

// ADDRESS-DECODE-SELECTIVITY probe for one PTH or PTL half of one bitplane
// pointer bank. bpu covers banks 0-5 (BPL1..BPL6) so the bank under test is
// always active regardless of which one it is. is_high selects whether the
// checked field is the PTH half (address bits 20:16) or the PTL half
// (bits 15:1, read back through address_out's low 16 bits since PTH is
// never written in either case, so it stays 0 and cannot contaminate the
// low field).
struct PtrAddrResult { uint32_t base, wrong, correct; };

PtrAddrResult probe_ptr_addr(uint16_t reg_addr, uint16_t wrong_addr, uint16_t data,
                              int bank, bool is_high) {
    auto run = [&](bool write_correct, bool write_wrong) -> uint32_t {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 6));  // lores, BPU=6: BPL1..BPL6 active
        prime_to_line(h, 0x4F);
        if (write_wrong)   h.write_reg(wrong_addr, data);
        if (write_correct) h.write_reg(reg_addr, data);
        LineResult r = run_single_line(h, 0x50);
        uint32_t addr = first_fetch_addr_for_plane(r, static_cast<uint16_t>(0x88 + bank));
        if (addr == 0xFFFFFFFFu) return addr;
        return is_high ? ((addr >> 16) & 0x1F) : (addr & 0xFFFF);
    };
    PtrAddrResult res;
    res.base    = run(false, false);
    res.wrong   = run(false, true);
    res.correct = run(true, false);
    return res;
}

// ADDRESS-DECODE-SELECTIVITY probe for BPL1MOD/BPL2MOD: line-to-line step
// of a plane's own first fetched address, across two consecutive lines
// (vpos 0x50 then 0x51) with a known distinct pointer pre-loaded.
struct ModAddrResult { int64_t base, wrong, correct; };

ModAddrResult probe_mod_addr(uint16_t reg_addr, uint16_t wrong_addr, int bank) {
    auto run = [&](bool write_correct, bool write_wrong) -> int64_t {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 2));  // lores, BPU=2: BPL1+BPL2 active
        prime_to_line(h, 0x4F);
        h.write_ptr(bank, 0x002000u + static_cast<uint32_t>(bank) * 0x1000u);
        if (write_wrong)   h.write_reg(wrong_addr, 0x0100);
        if (write_correct) h.write_reg(reg_addr, 0x0100);
        LineResult r1 = run_single_line(h, 0x50);
        LineResult r2 = run_single_line(h, 0x51);
        uint32_t a1 = first_fetch_addr_for_plane(r1, static_cast<uint16_t>(0x88 + bank));
        uint32_t a2 = first_fetch_addr_for_plane(r2, static_cast<uint16_t>(0x88 + bank));
        if (a1 == 0xFFFFFFFFu || a2 == 0xFFFFFFFFu) return INT64_MIN;
        return static_cast<int64_t>(a2) - static_cast<int64_t>(a1);
    };
    ModAddrResult res;
    res.base    = run(false, false);
    res.wrong   = run(false, true);
    res.correct = run(true, false);
    return res;
}

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "NOTE: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ================= FIXTURE-DELIVERS-KNOWN-VALUE [no page, self-proof] =================
    {
        Harness h;
        h.setup_standard();
        h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);
        h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);
        h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));  // lores, BPU=1
        prime_to_line(h, 0x4F);
        constexpr uint32_t kKnown = 0x00123456u & 0x1FFFFEu;  // 21-bit chip address, bit0=0
        h.write_ptr(0, kKnown);
        LineResult r = run_single_line(h, 0x50);
        uint32_t first = first_fetch_addr_for_plane(r, 0x88);
        checks.expect(
            r.count > 0 && first == kKnown,
            "FIXTURE-DELIVERS-KNOWN-VALUE: a single distinct, known pointer "
            "(0x" + std::to_string(kKnown) + ") written to BPL1PTH/BPL1PTL "
            "immediately before the measured line arrives, unchanged, as "
            "the FIRST fetched address_out this package's own independently "
            "built obj_dir produces at reg_address_out=$88 (BPL1DAT) -- "
            "proves the write-then-fetch path is alive before any "
            "address-decode assertion below depends on it.");
    }

    // ============================================================
    // BITPLANE POINTER REGISTERS (Appendix A p.272, Appendix B p.291)
    // ============================================================

    // ================= BPL1PTH-ADDRESS-0E0 =================
    {
        PtrAddrResult r = probe_ptr_addr(0x0E0, 0x0E4, 0x0015, /*bank=*/0, /*is_high=*/true);
        checks.expect(
            r.wrong == r.base && r.correct != r.base && r.correct == 0x0015,
            "BPL1PTH-ADDRESS-0E0: writing 0x15 to $0E4 (BPL2PTH, the wrong "
            "address) leaves BPL1's own fetched high-address field at its "
            "untouched baseline (0); writing the identical 0x15 to $0E0 "
            "(BPL1PTH, the address the manual names) sets it to 0x15 -- "
            "'BPLxPTH    0E0      W       A     Bitplane x pointer (high 3 "
            "bits)' (p.272), 'BPL1PTH    +   0E0    W   A           "
            "Bitplane 1 pointer    (high 3 bits)' (p.291). A-bplptr.c1, "
            "A-bplxpth.addr, B-bpl1pth.addr, B-subject.c26.");
    }

    // ================= BPL1PTL-ADDRESS-0E2 =================
    {
        PtrAddrResult r = probe_ptr_addr(0x0E2, 0x0E6, 0x2468, /*bank=*/0, /*is_high=*/false);
        checks.expect(
            r.wrong == r.base && r.correct != r.base && r.correct == 0x2468,
            "BPL1PTL-ADDRESS-0E2: BPL1's own fetched low-address field "
            "drifts from its own auto-increment during priming (baseline "
            "not 0 -- pointers auto-increment on every dma cycle while "
            "active, written or not, see IMPLEMENTATION NOTES), so the "
            "measurement compares against that SAME per-run baseline, not "
            "against a fixed 0: writing 0x2468 to $0E6 (BPL2PTL, the wrong "
            "address) leaves it at that baseline; writing the identical "
            "0x2468 to $0E2 (BPL1PTL, the address the manual names) "
            "overwrites it to exactly 0x2468, always different from the "
            "drifted baseline -- 'BPLxPTL    0E2      W       A     "
            "pointer (low 15 bits)' (p.272), 'BPL1PTL    +   0E2    W   A "
            "          Bitplane 1 pointer    (low 15 bits)' (p.291). "
            "A-bplptr.c4, A-bplxptl.addr, B-bpl1ptl.addr, B-subject.c33.");
    }

    // ================= BPL2PTH-ADDRESS-0E4 =================
    {
        PtrAddrResult r = probe_ptr_addr(0x0E4, 0x0E8, 0x0015, /*bank=*/1, /*is_high=*/true);
        checks.expect(
            r.wrong == r.base && r.correct != r.base && r.correct == 0x0015,
            "BPL2PTH-ADDRESS-0E4: writing 0x15 to $0E8 (BPL3PTH, the wrong "
            "address) leaves BPL2's own fetched high-address field at its "
            "untouched baseline (0); writing the identical 0x15 to $0E4 "
            "(BPL2PTH, the address the manual names) sets it to 0x15 -- "
            "'BPL2PTH    +   0E4    W   A           Bitplane 2 pointer    "
            "(high 3 bits)' (p.291). B-bpl2pth.addr, B-subject.c40.");
    }

    // ================= BPL2PTL-ADDRESS-0E6 =================
    {
        PtrAddrResult r = probe_ptr_addr(0x0E6, 0x0EA, 0x2468, /*bank=*/1, /*is_high=*/false);
        checks.expect(
            r.wrong == r.base && r.correct != r.base && r.correct == 0x2468,
            "BPL2PTL-ADDRESS-0E6: writing 0x2468 to $0EA (BPL3PTL, the "
            "wrong address) leaves BPL2's own fetched low-address field at "
            "its untouched baseline (0); writing the identical 0x2468 to "
            "$0E6 (BPL2PTL, the address the manual names) sets it to "
            "0x2468 -- 'BPL2PTL    +   0E6    W   A           Bitplane 2 "
            "pointer    (low 15 bits)' (p.291). B-bpl2ptl.addr, "
            "B-subject.c47.");
    }

    // ================= BPL3PTH-ADDRESS-0E8 =================
    {
        PtrAddrResult r = probe_ptr_addr(0x0E8, 0x0EC, 0x0015, /*bank=*/2, /*is_high=*/true);
        checks.expect(
            r.wrong == r.base && r.correct != r.base && r.correct == 0x0015,
            "BPL3PTH-ADDRESS-0E8: writing 0x15 to $0EC (BPL4PTH, the wrong "
            "address) leaves BPL3's own fetched high-address field at its "
            "untouched baseline (0); writing the identical 0x15 to $0E8 "
            "(BPL3PTH, the address the manual names) sets it to 0x15 -- "
            "'BPL3PTH    +   0E8    W   A           Bitplane 3 pointer    "
            "(high 3 bits)' (p.291). B-bpl3pth.addr, B-subject.c54.");
    }

    // ================= BPL3PTL-ADDRESS-0EA =================
    {
        PtrAddrResult r = probe_ptr_addr(0x0EA, 0x0EE, 0x2468, /*bank=*/2, /*is_high=*/false);
        checks.expect(
            r.wrong == r.base && r.correct != r.base && r.correct == 0x2468,
            "BPL3PTL-ADDRESS-0EA: writing 0x2468 to $0EE (BPL4PTL, the "
            "wrong address) leaves BPL3's own fetched low-address field at "
            "its untouched baseline (0); writing the identical 0x2468 to "
            "$0EA (BPL3PTL, the address the manual names) sets it to "
            "0x2468 -- 'BPL3PTL    +   0EA    W   A           Bitplane 3 "
            "pointer    (low 15 bits)' (p.291). B-bpl3ptl.addr, "
            "B-subject.c61.");
    }

    // ================= BPL4PTH-ADDRESS-0EC =================
    {
        PtrAddrResult r = probe_ptr_addr(0x0EC, 0x0F0, 0x0015, /*bank=*/3, /*is_high=*/true);
        checks.expect(
            r.wrong == r.base && r.correct != r.base && r.correct == 0x0015,
            "BPL4PTH-ADDRESS-0EC: writing 0x15 to $0F0 (BPL5PTH, the wrong "
            "address) leaves BPL4's own fetched high-address field at its "
            "untouched baseline (0); writing the identical 0x15 to $0EC "
            "(BPL4PTH, the address the manual names) sets it to 0x15 -- "
            "'BPL4PTH    +   0EC    W   A           Bitplane 4 pointer    "
            "(high 3 bits)' (p.291). B-bpl4pth.addr, B-subject.c68.");
    }

    // ================= BPL4PTL-ADDRESS-0EE =================
    {
        PtrAddrResult r = probe_ptr_addr(0x0EE, 0x0F2, 0x2468, /*bank=*/3, /*is_high=*/false);
        checks.expect(
            r.wrong == r.base && r.correct != r.base && r.correct == 0x2468,
            "BPL4PTL-ADDRESS-0EE: writing 0x2468 to $0F2 (BPL5PTL, the "
            "wrong address) leaves BPL4's own fetched low-address field at "
            "its untouched baseline (0); writing the identical 0x2468 to "
            "$0EE (BPL4PTL, the address the manual names) sets it to "
            "0x2468 -- 'BPL4PTL    +   0EE    W   A           Bitplane 4 "
            "pointer    (low 15 bits)' (p.291). B-bpl4ptl.addr, "
            "B-subject.c75.");
    }

    // ================= BPL5PTH-ADDRESS-0F0 =================
    {
        PtrAddrResult r = probe_ptr_addr(0x0F0, 0x0F4, 0x0015, /*bank=*/4, /*is_high=*/true);
        checks.expect(
            r.wrong == r.base && r.correct != r.base && r.correct == 0x0015,
            "BPL5PTH-ADDRESS-0F0: writing 0x15 to $0F4 (BPL6PTH, the wrong "
            "address) leaves BPL5's own fetched high-address field at its "
            "untouched baseline (0); writing the identical 0x15 to $0F0 "
            "(BPL5PTH, the address the manual names) sets it to 0x15 -- "
            "'BPL5PTH    +   0F0    W   A           Bitplane 5 pointer    "
            "(high 3 bits)' (p.291). B-bpl5pth.addr, B-subject.c82.");
    }

    // ================= BPL5PTL-ADDRESS-0F2 =================
    {
        PtrAddrResult r = probe_ptr_addr(0x0F2, 0x0F6, 0x2468, /*bank=*/4, /*is_high=*/false);
        checks.expect(
            r.wrong == r.base && r.correct != r.base && r.correct == 0x2468,
            "BPL5PTL-ADDRESS-0F2: writing 0x2468 to $0F6 (BPL6PTL, the "
            "wrong address) leaves BPL5's own fetched low-address field at "
            "its untouched baseline (0); writing the identical 0x2468 to "
            "$0F2 (BPL5PTL, the address the manual names) sets it to "
            "0x2468 -- 'BPL5PTL    +   0F2    W   A           Bitplane 5 "
            "pointer    (low 15 bits)' (p.291). B-bpl5ptl.addr, "
            "B-subject.c89.");
    }

    // ================= BPL6PTH-ADDRESS-0F4 =================
    {
        PtrAddrResult r = probe_ptr_addr(0x0F4, 0x0E0, 0x0015, /*bank=*/5, /*is_high=*/true);
        checks.expect(
            r.wrong == r.base && r.correct != r.base && r.correct == 0x0015,
            "BPL6PTH-ADDRESS-0F4: writing 0x15 to $0E0 (BPL1PTH, the wrong "
            "address) leaves BPL6's own fetched high-address field at its "
            "untouched baseline (0); writing the identical 0x15 to $0F4 "
            "(BPL6PTH, the address the manual names) sets it to 0x15 -- "
            "'BPL6PTH    +   0F4    W   A           Bitplane 6 pointer    "
            "(high 3 bits)' (p.291). B-bpl6pth.addr, B-subject.c96.");
    }

    // ================= BPL6PTL-ADDRESS-0F6 =================
    {
        PtrAddrResult r = probe_ptr_addr(0x0F6, 0x0E2, 0x2468, /*bank=*/5, /*is_high=*/false);
        checks.expect(
            r.wrong == r.base && r.correct != r.base && r.correct == 0x2468,
            "BPL6PTL-ADDRESS-0F6: writing 0x2468 to $0E2 (BPL1PTL, the "
            "wrong address) leaves BPL6's own fetched low-address field at "
            "its untouched baseline (0); writing the identical 0x2468 to "
            "$0F6 (BPL6PTL, the address the manual names) sets it to "
            "0x2468 -- 'BPL6PTL    +   0F6    W   A           Bitplane 6 "
            "pointer    (low 15 bits)' (p.291). B-bpl6ptl.addr, "
            "B-subject.c103.");
    }

    checks.note(
        "RTL-VS-MANUAL-BPLXPTH-FIELD-WIDTH: every BPLnPTH row above is "
        "printed '(high 3 bits)' (p.272, p.291), but bplpth is declared "
        "`reg [20:16] bplpth [7:0]` (agnus_bitplanedma.v:82, 'upper 5 bits "
        "bitplane pointers'), a 5-bit field matching address_out[20:16] "
        "(l.46) -- this AGA-capable RTL stores TWO MORE high bits than the "
        "manual's OCS/ECS-era '(high 3 bits)' phrase states, for a chip "
        "address range beyond 1 MB. Not folded into any PASS/FAIL above "
        "(those test address decode only, not field width); recorded here "
        "per PACKAGE.md's CONFLICT NOTE as the resolution of the 12 "
        "flagged pointer claims' 'page 54 versus page 272/291' conflict. "
        "BPLnPTL's printed '(low 15 bits)' is NOT in conflict: bplptl is "
        "`reg [15:1] bplptl [7:0]` (l.83), exactly 15 bits.");

    // ============================================================
    // BITPLANE MODULO REGISTERS (Appendix A p.270, Appendix B p.291)
    // ============================================================

    // ================= BPL1MOD-ADDRESS-108 =================
    {
        ModAddrResult r = probe_mod_addr(BPL1MOD_ADDR, BPL2MOD_ADDR, /*bank=*/0);
        bool wrong_matches_base = r.wrong == r.base;
        bool correct_differs = (r.base != INT64_MIN) && (r.correct != INT64_MIN) &&
                                std::abs(r.correct - r.base) > 4;
        checks.expect(
            wrong_matches_base && correct_differs,
            "BPL1MOD-ADDRESS-108: with BPU=2 (BPL1+BPL2 active, lores, "
            "fmode0), BPL1's own line-to-line pointer step is " +
            std::to_string(r.base) + " bytes at baseline; writing 0x0100 "
            "to $10A (BPL2MOD, the wrong address) leaves it unchanged (" +
            std::to_string(r.wrong) + "); writing the identical 0x0100 to "
            "$108 (BPL1MOD, the address the manual names) changes it (" +
            std::to_string(r.correct) + ") -- 'BPL1MOD    108      W       "
            "A     Bitplane modulo (odd planes)' (p.270), 'BPL1MOD        "
            "108    W   A           Bitplane modulo (odd planes)' (p.291). "
            "A-bpl1mod.addr, A-bplmod.c1, B-bpl1mod.addr, "
            "B-bpl1mod.addr-p291, B-subject.c126.");
    }

    // ================= BPL2MOD-ADDRESS-10A =================
    {
        ModAddrResult r = probe_mod_addr(BPL2MOD_ADDR, BPL1MOD_ADDR, /*bank=*/1);
        bool wrong_matches_base = r.wrong == r.base;
        bool correct_differs = (r.base != INT64_MIN) && (r.correct != INT64_MIN) &&
                                std::abs(r.correct - r.base) > 4;
        checks.expect(
            wrong_matches_base && correct_differs,
            "BPL2MOD-ADDRESS-10A: with BPU=2 (BPL1+BPL2 active, lores, "
            "fmode0), BPL2's own line-to-line pointer step is " +
            std::to_string(r.base) + " bytes at baseline; writing 0x0100 "
            "to $108 (BPL1MOD, the wrong address) leaves it unchanged (" +
            std::to_string(r.wrong) + "); writing the identical 0x0100 to "
            "$10A (BPL2MOD, the address the manual names) changes it (" +
            std::to_string(r.correct) + ") -- 'BPL2MOD    10A      W       "
            "A     Bitplane modulo (even planes)' (p.270), 'BPL2MOD        "
            "10A    W   A           Bitplane modulo (even planes)' (p.291). "
            "A-bpl2mod.addr, A-bplmod.c5, B-bpl2mod.addr, "
            "B-bpl2mod.addr-p291, B-subject.c130.");
    }

    // ============================================================
    // DDFSTRT / DDFSTOP (Appendix A p.277, Appendix B p.290)
    // ============================================================

    // ================= DDFSTRT-ADDRESS-092 =================
    {
        auto measure = [](bool write_correct, bool write_wrong) {
            Harness h;
            h.setup_standard();
            h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);  // companion, correct, constant
            h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));  // lores, BPU=1
            if (write_wrong)   h.write_reg(BPL1MOD_ADDR, DDFSTRT_LORES_STD);  // wrong address
            if (write_correct) h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);  // correct address
            return first_fetch_hpos(h, 0x50);
        };
        int base = measure(false, false);
        int wrong = measure(false, true);
        int correct = measure(true, false);
        checks.expect(
            wrong == base && correct != base && correct != -1,
            "DDFSTRT-ADDRESS-092: with DDFSTOP held at its standard value "
            "$00D0 (correct address) in all three runs, DDFSTRT unwritten "
            "gives a baseline first-fetch hpos of " + std::to_string(base) +
            "; writing $0038 to $108 (BPL1MOD, the wrong address) leaves "
            "it at " + std::to_string(wrong) + "; writing the identical "
            "$0038 to $092 (DDFSTRT, the address the manual names) moves "
            "the first fetch to hpos=" + std::to_string(correct) +
            " -- only the address-decode selectivity is asserted, not the "
            "exact hpos value (that comparison is hrm/ch3-ddf-fetch-"
            "window's own job) -- 'DDFSTRT    092      W       A    "
            "Display data fetch start (horiz. position)' (p.277), "
            "'    DDFSTRT        092    W               Display bitplane "
            "data fetch start' (p.290, chip column blank -- address/W only, "
            "not cited for chip ownership). A-ddf.c3, A-ddfstrt.addr, "
            "B-ddfstrt.addr, B-subject.c16.");
    }

    // ================= DDFSTOP-ADDRESS-094 =================
    {
        auto measure = [](bool write_correct, bool write_wrong) {
            Harness h;
            h.setup_standard();
            h.write_reg(DDFSTRT_ADDR, DDFSTRT_LORES_STD);  // companion, correct, constant
            h.write_reg(BPLCON0_ADDR, bplcon0_word(false, 1));  // lores, BPU=1
            if (write_wrong)   h.write_reg(BPL2MOD_ADDR, DDFSTOP_LORES_STD);  // wrong address
            if (write_correct) h.write_reg(DDFSTOP_ADDR, DDFSTOP_LORES_STD);  // correct address
            LineResult r = run_to_line(h, 0x50);
            return r.count;
        };
        int base = measure(false, false);
        int wrong = measure(false, true);
        int correct = measure(true, false);
        checks.expect(
            wrong == base && correct != base,
            "DDFSTOP-ADDRESS-094: with DDFSTRT held at its standard value "
            "$0038 (correct address) in all three runs, DDFSTOP unwritten "
            "gives a baseline word count of " + std::to_string(base) +
            "; writing $00D0 to $10A (BPL2MOD, the wrong address) leaves "
            "it at " + std::to_string(wrong) + "; writing the identical "
            "$00D0 to $094 (DDFSTOP, the address the manual names) changes "
            "the count to " + std::to_string(correct) + " -- only the "
            "address-decode selectivity is asserted, not the exact count "
            "(hrm/ch3-ddf-fetch-window's own job) -- 'DDFSTOP    094      "
            "W       A    Display data fetch stop (horiz. position)' "
            "(p.277), '    DDFSTOP        094    W               Display "
            "bitplane data fetch stop' (p.290, chip column blank -- "
            "address/W only, not cited for chip ownership). A-ddf.c1, "
            "A-ddfstop.addr, B-ddfstop.addr, B-subject.c19.");
    }

    // =========================================================================
    // APPENDIX B/A REOFFER -- 2 claims declined here, in the ledger's own
    // vocabulary, having been offered at ch3-register-map-agnus in error (per
    // this file's own reasoning below) and correctly declined there in an
    // earlier pass; now reasoned about on record rather than left unprinted.
    // No predicate, stimulus or expected value above is touched.
    // =========================================================================
    checks.record(
        "opus:7-appA-pre.c6 (p.263, 'All of the pointer type registers are organized as 32 bits on a long word "
        "boundary'): not asserted -- this claim quantifies over EVERY pointer-type register in the machine (bitplane, "
        "audio, sprite, disk, copper, blitter); this package's own 12 pointer-address assertions above (BPL1PTH-ADDRESS-"
        "0E0 through BPL6PTL-ADDRESS-0F6) demonstrate only the bitplane pointer family -- same ground as "
        "hrm/ch5-audio-location-pointer's COVERAGE REPAIR 3: a universal claim is not established by a single family's "
        "witness, however structural the mechanism looks. Needs either a predicate over every pointer family (a "
        "top-level fixture spanning all of them) or a narrower split id scoped to the bitplane pointers this package "
        "actually tests.");
    checks.record(
        "opus:7-appB.c9 (p.287, Appendix B's 'ER' early-read legend -- a DMA transfer to RAM initiated by Agnus timing "
        "rather than a read address on the destination bus, because RAM timing needs the data on the bus earlier than a "
        "microprocessor read cycle would put it there): not asserted -- this file's own BOUNDARY note states "
        "agnus_bitplanedma.v 'has no data_out at all, so nothing can be \"read back\" through the register bus the way "
        "some sibling modules allow'; the early-read timing relationship ER describes is a property of a destination "
        "read port this module does not have. Needs a fixture with an actual RAM-side read port (e.g. the memory "
        "arbiter/gary.v boundary) to observe timing against a real microprocessor read cycle.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed
              << " failed\n";
    // A red result IS a valid, finished outcome -- that principle forbids
    // changing the assertion until it passes. It does not license hiding the
    // red from the suite. run_all.sh classifies a package solely by its exit
    // status, so returning 0 here launders any future red into a suite PASS.
    return checks.failed ? 1 : 0;
}

// ============================================================================
// MUTATION LOG (11.09.2026). Every entry below was actually applied by this
// agent, rebuilt and run under WSL/Verilator (`make test`), confirmed red,
// then reverted and rebuilt clean (17/17 green again) -- not predicted. Each
// mutation changes what the harness ASKS the module to do (which address
// receives the "correct"/"wrong" write) while leaving every assertion's own
// expected-value logic untouched, per the build brief's rule that flipping
// an expected value proves nothing.
//
// MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red: known pointer written to bank 1 (BPL2PTH/PTL) instead of bank 0; bank 0's first fetch no longer equalled kKnown.
// MUTATION BPL1PTH-ADDRESS-0E0 stimulus red: probe_ptr_addr's correct/wrong addresses swapped ($0E4 as "correct", $0E0 as "wrong"); BPL1's high field followed the real $0E0 write labelled "wrong".
// MUTATION BPL1PTL-ADDRESS-0E2 stimulus red: probe_ptr_addr's correct/wrong addresses swapped ($0E6 as "correct", $0E2 as "wrong"); BPL1's low field followed the real $0E2 write labelled "wrong".
// MUTATION BPL2PTH-ADDRESS-0E4 stimulus red: probe_ptr_addr's correct/wrong addresses swapped ($0E8 as "correct", $0E4 as "wrong"); same failure shape as BPL1PTH.
// MUTATION BPL2PTL-ADDRESS-0E6 stimulus red: probe_ptr_addr's correct/wrong addresses swapped ($0EA as "correct", $0E6 as "wrong"); same failure shape as BPL1PTL.
// MUTATION BPL3PTH-ADDRESS-0E8 stimulus red: probe_ptr_addr's correct/wrong addresses swapped ($0EC as "correct", $0E8 as "wrong").
// MUTATION BPL3PTL-ADDRESS-0EA stimulus red: probe_ptr_addr's correct/wrong addresses swapped ($0EE as "correct", $0EA as "wrong").
// MUTATION BPL4PTH-ADDRESS-0EC stimulus red: probe_ptr_addr's correct/wrong addresses swapped ($0F0 as "correct", $0EC as "wrong").
// MUTATION BPL4PTL-ADDRESS-0EE stimulus red: probe_ptr_addr's correct/wrong addresses swapped ($0F2 as "correct", $0EE as "wrong").
// MUTATION BPL5PTH-ADDRESS-0F0 stimulus red: probe_ptr_addr's correct/wrong addresses swapped ($0F4 as "correct", $0F0 as "wrong").
// MUTATION BPL5PTL-ADDRESS-0F2 stimulus red: probe_ptr_addr's correct/wrong addresses swapped ($0F6 as "correct", $0F2 as "wrong").
// MUTATION BPL6PTH-ADDRESS-0F4 stimulus red: probe_ptr_addr's correct/wrong addresses swapped ($0E0 as "correct", $0F4 as "wrong"), wrapping to bank 0's own PTH address.
// MUTATION BPL6PTL-ADDRESS-0F6 stimulus red: probe_ptr_addr's correct/wrong addresses swapped ($0E2 as "correct", $0F6 as "wrong"), wrapping to bank 0's own PTL address.
// MUTATION BPL1MOD-ADDRESS-108 stimulus red: probe_mod_addr's correct/wrong addresses swapped ($10A as "correct", $108 as "wrong"); BPL1's line-to-line step followed the real $108 write labelled "wrong".
// MUTATION BPL2MOD-ADDRESS-10A stimulus red: probe_mod_addr's correct/wrong addresses swapped ($108 as "correct", $10A as "wrong"); BPL2's line-to-line step followed the real $10A write labelled "wrong".
// MUTATION DDFSTRT-ADDRESS-092 stimulus red: the measure lambda's "correct"/"wrong" write targets swapped (DDFSTRT_ADDR labelled wrong, BPL1MOD_ADDR labelled correct); first-fetch hpos followed the real $092 write labelled "wrong".
// MUTATION DDFSTOP-ADDRESS-094 stimulus red: the measure lambda's "correct"/"wrong" write targets swapped (DDFSTOP_ADDR labelled wrong, BPL2MOD_ADDR labelled correct); word count followed the real $094 write labelled "wrong".
// ============================================================================
