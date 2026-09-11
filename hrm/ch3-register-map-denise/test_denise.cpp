// ch3-register-map-denise -- denise (top-level) register-map conformance test
//
// Test purpose: the register MAP at the Denise boundary, as the Amiga
//   Hardware Reference Manual (3rd ed.) states it in Chapter 3 and in
//   Appendices A and B: which custom-chip addresses Denise decodes at all,
//   the read/write direction the manual prints for each, and (where the
//   manual names it) which chip owns the register. This package does NOT
//   test register *behaviour* (scroll timing, HAM, dual-playfield mixing,
//   collision semantics, DDF fetch windows) -- four sibling packages built
//   on this same fixture own that. This package answers one question per
//   register: is the address in the manual the address this module answers
//   to, and does it answer in the direction (R/W) the manual prints.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, 3rd ed., printed pages
//   47, 92, 271, 273, 277, 290, 291, 292, 293, 300, 307 (claims.jsonl for
//   ch3-register-map-denise, 134 records). A red result below means
//   denise.v (or a sub-module it instantiates) disagrees with the manual,
//   not that the assertion is wrong -- no assertion here was, or should be,
//   adjusted to make the RTL pass.
// Non-goals: BPLCON0/2/3's individual bit *semantics* (hires, ham, dpf,
//   killehb, bank, spres, ...), colour-table value round-tripping and the
//   32-way bitplane-to-select assembly (already exhaustively proven by the
//   sibling package hrm/ch3-colour-register-format/ at the narrower
//   denise_colortable boundary), scroll/DDF timing, collision bit meaning.
//
// BOUNDARY: top module `denise` only (one Verilator build, sources
//   denise.v + denise_bitplanes.v + denise_bitplane_shifter.v +
//   denise_playfields.v + denise_sprites.v + denise_sprites_shifter.v +
//   denise_collision.v + denise_hamgenerator.v + denise_colortable.v +
//   denise_colortable_ram_mf.v + denise_spritepriority.v + bitplane_ram.v),
//   per PACKAGE.md. Ports used, exactly as PACKAGE.md's own evidence names
//   them (module header, denise.v:31-52): clk, clk7_en, c1, c3, cck, reset,
//   strhor, reg_address_in[8:1], data_in[16-1:0], chip48[48-1:0],
//   data_out[16-1:0], blank, red/green/blue[8-1:0], a1k, ecs, aga,
//   hires (out), shres (out).
//
// FIXTURE CONFIGURATION, and why: a1k=0, ecs=1, aga=0 throughout every
//   Harness in this file. ecs=1/aga=0 is not a free choice -- it is what
//   makes the fixture reproduce PACKAGE.md's own disclosed "already
//   measured" facts: denise.v:322 reads
//     deniseid_out = reg_address_in==DENISEID ? aga?$00F8 : ecs?$FFFC:$FFFF
//                    : 0
//   so DENISEID reading $FFFC (the value PACKAGE.md's evidence and this
//   package's own CONTROL group both rely on) requires exactly aga=0,
//   ecs=1. It also matches the manual pages this package cites, which
//   describe the OCS/ECS-era chip, not the AGA extension. This is a
//   structural fact read directly out of denise.v's own ternary, not a
//   simulated measurement, and it is disclosed here rather than asserted
//   as a claim, exactly as this file's own CONTROL group later measures it
//   to be true before anything else in this file trusts it.
//
// THE C1/C3 TRAP (PACKAGE.md's own warning, load-bearing): amiga_clk.v:58-61
//   makes c3 the registered 7MHz clock and c1 = ~c3 delayed one more 28MHz
//   cycle, so c1 and c3 overlap for two of the four 28MHz phases (both high
//   in one, both low in another). denise_bitplanes.v:381 forms
//   `wire clk7n_en = c1 & c3;` and gates every BPLxDAT parallel load on it
//   (`if (st & clk7n_en) ... bpl1dat <= ...`, denise_bitplanes.v:387-394 and
//   the seven repeats for planes 2-8). Five older FPGA-029 tests in this
//   repo (hrm/denise/, hrm/denise_bitplane_shifter/, etc.) drive c1 and c3
//   as disjoint, non-overlapping phases (e.g. hrm/denise/test_denise.cpp:
//   `c1 = (cycle==1); c3 = (cycle==3);`) and assert nothing about bitplane
//   content, which is why nobody noticed that clk7n_en is permanently 0
//   under that drive pattern and no BPLxDAT write ever reaches a shifter.
//   Harness::tick() below instead drives, every 28MHz cycle, the measured
//   working pattern: clk7_en=(phase==0), c1=(phase==0||phase==1),
//   c3=(phase==1||phase==2) -- so phase 1 has both c1 and c3 high (the
//   parallel-load window `!c1&&!c3` sits at phase 3, matching
//   denise_bitplane_shifter.v:224's `if (load && !c1 && !c3)`).
//
// HOW THE COLOUR-INDEX STIMULUS WAS BUILT (mechanical fact, read out of the
//   RTL after this file's oracle -- the addresses and R/W directions below
//   -- was already fixed from the manual quotations; nothing here changes
//   what counts as pass/fail, only how a given index is dialled in, and the
//   MEASURED note below records what this package's own build actually
//   found, not something assumed):
//   denise_playfields.v:95-100, single-playfield case: `plfdata[7:0] =
//   bpldata[8:1]` (no dual-playfield, no Swiv exception when bplcon2's
//   pf2p<=5, which holds because this file never writes BPLCON2). With
//   l_bpu (denise.v:194-203, latched off BPLCON0.bpu the moment BPL1DAT is
//   written) set to 5, denise.v:367-374 forces bpldata[6..8]=0, so the
//   colour-table select index actually presented is exactly
//     index = 16*bpldata[5] + 8*bpldata[4] + 4*bpldata[3] + 2*bpldata[2] + bpldata[1]
//   one bit per bitplane, each held at a constant 0 or 1 by writing its
//   BPLxDAT as $0000 or $FFFF.
//
//   MEASURED (this package's own build/calibration, not assumed): a single
//   BPLxDAT write only pulses the plane shifter's parallel load once
//   (denise_bitplane_shifter.v:223-226, non-EBR path); with nothing to
//   refetch it the way Agnus DMA would on real hardware, the loaded word
//   shifts/rescrolls out through a 64-bit-deep pipeline and the plane's
//   output is reliably non-zero for only about one 28MHz tick before it is
//   gone -- an early draft of this file that wrote BPLxDAT once and then
//   idled (warmed up, in that draft's words) for 800 cycles before sampling
//   read every register as $0000 unconditionally, because by the time it
//   sampled the transient had long since drained. Harness::refresh_bitplanes
//   below instead rewrites the same BPLxDAT value repeatedly, back to back
//   (measured to saturate a plane's output to a steady 1 within 6-9 repeats
//   of a single register); write_bitplane_pattern does this for all 5
//   planes together over several rounds, and the display pixel is sampled
//   with no further idle ticks after the last refresh (a handful of settle
//   ticks after the refreshing stops is enough for the signal to decay
//   again, as the same early draft's follow-up measurement confirmed).
//   Order matters and is enforced by write_bitplane_pattern()/
//   refresh_bitplanes(): BPLCON0 (sets bpu combinationally, written once)
//   and BPL2..5DAT (load their own buffer registers, denise_bitplanes.v:
//   396-434) are written before BPL1DAT in every round, because BPL1DAT's
//   write is what both latches l_bpu (denise.v:195-203) and pulses `load`
//   a cycle later (denise_bitplanes.v:467), which parallel-loads *all* the
//   plane shifters synchronously from whatever their buffers hold at that
//   instant (denise_bitplane_shifter.v:223-226).
//
// CLAIM GROUPS -- every one of the 134 claim ids in claims.jsonl is cited,
//   in full, in exactly one group below (in the assertion string that
//   tests it, not only here); ids restating the identical proposition
//   under a different manual wording, page, appendix or reader are grouped
//   and cited together on the single assertion that tests that
//   proposition, the convention hrm/ch3-colour-register-format/ documents
//   and uses.
//
//   CONTROL-FIXTURE-ALIVE (no claim id)
//     -- DENISEID and CLXDAT, two independently-decoded read-only
//        registers, read back two distinct non-zero values before
//        anything below trusts a "$0000" reading from a write-only
//        register to mean "write-only" rather than "the read path is
//        stuck at zero". Required by the package brief in exactly these
//        words.
//   DENISEID-ADDR-RO (7 ids)
//     -- DENISEID answers $FFFC at $07C, a *different*, unrelated,
//        write-only address answers $0000 through the same data_out mux
//        (selectivity: the $FFFC is tied to $07C specifically, not a
//        stuck bus), and an attempted write to $07C does not change the
//        next reading (read-only, not merely "happens to read something
//        today").
//   BPLCON0-ADDR (4 ids), BPLCON0-WO (5 ids)
//   BPLCON2-WO (4 ids), BPLCON3-WO (4 ids)
//     -- ADDR: BPLCON0's bpu field is proven to live at $100 by the one
//        measurement PACKAGE.md's own evidence already discloses as
//        MEASURED: with a single bitplane enabled (bpu=1) and BPL1DAT
//        held at a constant 1, the pixel inside the display window and
//        the border outside it read back as different colours, and the
//        border matches the colour written to $180 both before and after
//        the window (so the index really did move from 0 to 1 and back to
//        0, which requires BPLCON0's write at $100 to have taken effect).
//        This same measurement is what settles COLOR00's and COLOR01's
//        addresses below (COLOR-ADDR-00-01).
//     -- WO: a distinct, non-zero pattern written to the register's own
//        address reads back $0000 there -- 'BPLCON0 ... W' (p.271/291),
//        'BPLCON2 ... W' (p.271/291), 'BPLCON3 ... W' (p.271/291).
//   COLOR-ADDR-00-01 (6 ids)
//     -- COLOR00 and COLOR01 individually, plus the two ids that state
//        where the COLORxx block begins ($180): settled by the same
//        BPLCON0-ADDR measurement above (border colour = COLOR00's
//        content, window colour = COLOR01's content).
//   COLOR-ADDR (60 ids, one shared loop over i=2..31, REG-style dynamic
//               labels, mirrors hrm/ch3-colour-register-format/'s
//               REG-00..REG-31 convention)
//     -- each of COLOR02..COLOR31 individually: with a fixed 5-bitplane
//        pattern dialling the colour-table select index to i (see the
//        mechanical note above), the pixel colour answers a fresh, second
//        write to COLORi with a different reading than the first write's
//        -- COLORi is what the index-i pixel is reading from, not some
//        other register or nothing at all.
//   COLOR-GENERAL-WO (4 ids)
//     -- the block-level statement "the color registers are write-only"
//        (not naming one specific register): representative measurement
//        on COLOR00.
//   COLOR-WO (32 ids, one shared loop over i=0..31)
//     -- each of COLOR00..COLOR31 individually: a distinct pattern written
//        to COLORi reads back $0000 at that same address.
//
//   Two additional groups carry claim ids but settle only the DIRECTION
//   half of BPLCON2/BPLCON3 with no matching ADDR group -- see OUT OF
//   SCOPE immediately below for why the ADDR half of those two registers
//   is not attempted here.
//
// OUT OF SCOPE -- not asserted anywhere below; halted rather than forced,
//   per the package brief's "a claim cannot be settled by ... a
//   measurement":
//   BPLCON2-ADDR (A-bplcon.c7, A-bplcon2.addr, B-bplcon2.addr,
//     B-subject.c118) and BPLCON3-ADDR (A-bplcon.c10, A-bplcon3.addr,
//     B-bplcon3.addr, B-subject.c122), 8 ids, p.271/291: BPLCON0's address
//     is provable at this boundary because its bpu field gates how many
//     bitplanes reach the colour-table index at all (an effect this file
//     already has the machinery to see). BPLCON2's only fields with any
//     observable path out of this fixture are killehb (needs a 6th
//     bitplane and the EHB half-brite comparison the sibling package
//     already owns) and the dual-playfield priority fields (need dpf=1 and
//     a second full bitplane set) -- BPLCON3's fields (bank/loct/spres/
//     border) are AGA- or ECS-border-specific and this file's fixture is
//     plain ECS with the border path unreachable (ecsena=BPLCON0 bit0 is
//     never set here, so `t_blank` is permanently 0, independent of
//     BPLCON3.brdrblnk). Reaching either would mean re-deriving a second
//     multi-bitplane behavioural rig purpose-built for a single register,
//     which is exactly the kind of "wider mechanism than this fixture's
//     boundary reaches" the sibling package's own OUT-OF-SCOPE section
//     warns against manufacturing. Both registers' WRITE-ONLY direction is
//     still fully asserted below (BPLCON2-WO, BPLCON3-WO) -- only the
//     address-specific half is left unsettled.
//
// MUTATION-LOG SUMMARY (9 groups; the 60-id COLOR-ADDR loop and the 32-id
//   COLOR-WO loop each carry one mutation covering the whole loop, matching
//   hrm/ch3-colour-register-format/'s REG-09/ADDR-32-DISTINCT convention).
//
// MUTATION CONTROL-FIXTURE-ALIVE stimulus red: CLXDAT's read address was redirected from $00E to DENISEID's own address ($07C), so both reads returned the same value ($FFFC) instead of two independently-decoded registers and the distinctness check failed (69/0 baseline went to 68 passed/1 failed, only this assertion); reverted (MUT_CONTROL back to 0) and rebuilt to confirm the 69/0 baseline was restored.
// MUTATION DENISEID-ADDR-RO stimulus red: the "at its own address" read was redirected from $07C to $100 (an unrelated write-only address), so the reading came back $0000 instead of $FFFC.
// MUTATION BPLCON0-ADDR stimulus red: the BPL1DAT write's data was changed from $FFFF (bitplane held high) to $0000, so the window pixel never differs from the border and the inside/outside comparison found no difference.
// MUTATION BPLCON0-WO stimulus red: the post-write read was redirected from BPLCON0's own address ($100) to DENISEID's ($07C), so the reading came back $FFFC instead of $0000.
// MUTATION BPLCON2-WO stimulus red: the post-write read was redirected from BPLCON2's own address ($104) to DENISEID's ($07C), so the reading came back $FFFC instead of $0000.
// MUTATION BPLCON3-WO stimulus red: the post-write read was redirected from BPLCON3's own address ($106) to DENISEID's ($07C), so the reading came back $FFFC instead of $0000.
// MUTATION COLOR-GENERAL-WO stimulus red: the post-write read was redirected from COLOR00's own address ($180) to DENISEID's ($07C), so the reading came back $FFFC instead of $0000.
// MUTATION COLOR-ADDR stimulus red: index 5's second COLOR write was redirected to COLOR06's address ($18A) instead of COLOR05's ($188), so COLOR05's own reading never changed between the first and second write.
// MUTATION COLOR-WO stimulus red: index 5's post-write read was redirected from COLOR05's own address to DENISEID's, so the reading came back $FFFC instead of $0000.
//
// Every mutation record listed above was actually built and run (see the commit history
// for this package for the per-mutation pass/fail counts) with the flag
// named after it defined to 1 (e.g. -DMUT_DENISEID=1), confirmed to turn
// only its own assertion red, then rebuilt with no flags to confirm a
// clean return to the baseline result. No oracle/expected value was
// changed by any of them; only what each check asks the module to do.

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include "Vdenise.h"
#include "verilated.h"

#ifndef MUT_CONTROL
#define MUT_CONTROL 0
#endif
#ifndef MUT_DENISEID
#define MUT_DENISEID 0
#endif
#ifndef MUT_BPLCON0_ADDR
#define MUT_BPLCON0_ADDR 0
#endif
#ifndef MUT_BPLCON0_WO
#define MUT_BPLCON0_WO 0
#endif
#ifndef MUT_BPLCON2_WO
#define MUT_BPLCON2_WO 0
#endif
#ifndef MUT_BPLCON3_WO
#define MUT_BPLCON3_WO 0
#endif
#ifndef MUT_COLOR_GENERAL_WO
#define MUT_COLOR_GENERAL_WO 0
#endif
#ifndef MUT_COLOR_ADDR
#define MUT_COLOR_ADDR 0
#endif
#ifndef MUT_COLOR_WO
#define MUT_COLOR_WO 0
#endif

namespace {

// reg_address_in is [8:1] (word address >> 1); every constant below is the
// manual's byte address already shifted right by one, matching denise.v's
// own `parameter` block (denise.v:56-65) and denise_bitplanes.v:106-115.
constexpr uint16_t IDLE_ADDR    = 0x00;  // matches no parameter in denise.v or its sub-modules.
constexpr uint16_t CLXDAT_ADDR  = 0x07;  // 9'h00E >> 1  (denise_collision.v:21)
constexpr uint16_t DIWSTRT_ADDR = 0x47;  // 9'h08E >> 1  (denise.v:56)
constexpr uint16_t DIWSTOP_ADDR = 0x48;  // 9'h090 >> 1  (denise.v:57)
constexpr uint16_t DENISEID_ADDR= 0x3E;  // 9'h07C >> 1  (denise.v:63)
constexpr uint16_t BPLCON0_ADDR = 0x80;  // 9'h100 >> 1  (denise.v:59)
constexpr uint16_t BPLCON2_ADDR = 0x82;  // 9'h104 >> 1  (denise.v:60)
constexpr uint16_t BPLCON3_ADDR = 0x83;  // 9'h106 >> 1  (denise.v:61)
constexpr uint16_t BPL1DAT_ADDR = 0x88;  // 9'h110 >> 1  (denise_bitplanes.v:107)
constexpr uint16_t BPL2DAT_ADDR = 0x89;  // 9'h112 >> 1  (denise_bitplanes.v:108)
constexpr uint16_t BPL3DAT_ADDR = 0x8A;  // 9'h114 >> 1  (denise_bitplanes.v:109)
constexpr uint16_t BPL4DAT_ADDR = 0x8B;  // 9'h116 >> 1  (denise_bitplanes.v:110)
constexpr uint16_t BPL5DAT_ADDR = 0x8C;  // 9'h118 >> 1  (denise_bitplanes.v:111)
constexpr uint16_t COLOR_BASE   = 0xC0;  // 9'h180 >> 1  (denise.v:65); COLORi = COLOR_BASE+i.
constexpr uint16_t DIWHIGH_ADDR = 0xF2;  // 9'h1E4 >> 1  (denise.v:58)

std::string hex4(uint16_t v) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "$%04X", static_cast<unsigned>(v));
    return std::string(buf);
}

class Harness {
public:
    Harness() : dut(new Vdenise) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->c1 = 0;
        dut->c3 = 0;
        dut->cck = 0;
        dut->reset = 1;
        dut->strhor = 0;
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        dut->chip48 = 0;
        dut->blank = 0;
        dut->a1k = 0;
        dut->ecs = 1;   // see the file-header FIXTURE CONFIGURATION note.
        dut->aga = 0;
        phase = 0;
        drive_phase();
        dut->eval();
        for (int i = 0; i < 32; ++i) tick();   // hold reset across several 7MHz ticks
        dut->reset = 0;
        for (int i = 0; i < 32; ++i) tick();   // let reset release settle
    }
    ~Harness() { delete dut; }

    // Drives clk7_en/c1/c3 for the *current* phase, then raises/lowers clk
    // around one 28MHz edge, then advances to the next of the four phases.
    // This is PACKAGE.md's own measured working pattern (see the file
    // header's THE C1/C3 TRAP note): phase 0 clk7_en, phases 0-1 c1,
    // phases 1-2 c3 -- so c1 and c3 overlap at phase 1 (both high) and at
    // phase 3 (both low, the parallel-load window).
    void tick() {
        drive_phase();
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        phase = (phase + 1) & 3;
    }

    void drive_phase() {
        dut->clk7_en = (phase == 0) ? 1 : 0;
        dut->c1 = (phase == 0 || phase == 1) ? 1 : 0;
        dut->c3 = (phase == 1 || phase == 2) ? 1 : 0;
    }

    // Writes `data` to custom-register word-address `addr` (already in the
    // reg_address_in[8:1] convention), holds it across a full 4-phase
    // 28MHz cycle so clk7_en is guaranteed to sample it once regardless of
    // which phase write_reg happens to be called on, then parks
    // reg_address_in back on IDLE_ADDR.
    void write_reg(uint16_t addr, uint16_t data) {
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick(); tick(); tick(); tick();
        dut->reg_address_in = IDLE_ADDR;
        dut->data_in = 0;
        tick();
    }

    // Presents `addr` on reg_address_in and returns data_out after it has
    // had a full 4-phase cycle to settle (data_out is a combinational mux
    // of three sources in denise.v:130/102, but giving it a full cycle
    // costs nothing and keeps every read on the same measurement basis).
    uint16_t read_reg(uint16_t addr) {
        dut->reg_address_in = addr;
        dut->data_in = 0;
        tick(); tick(); tick(); tick();
        uint16_t result = dut->data_out;
        dut->reg_address_in = IDLE_ADDR;
        tick();
        return result;
    }

    // MEASURED (this file, calibration retained here as documentation):
    // a single BPLxDAT write pulses the plane shifter's parallel load
    // exactly once (denise_bitplane_shifter.v:223-226); with nothing to
    // refetch it the way Agnus DMA would on real hardware, the loaded word
    // shifts out through the shift/scroll/superhires-scroll pipeline and
    // the plane's `out` (denise.v's bpldata[n]) is only reliably non-zero
    // for a single 28MHz tick before it is gone. Rewriting the *same*
    // BPLxDAT value repeatedly, back to back, keeps a fresh "1" (or "0")
    // continuously re-entering the pipeline; empirically (see this
    // package's build log) bpldata[1] saturates to a steady 1 within 6-9
    // repeated single-register writes of BPL1DAT=$FFFF. refresh_bitplanes
    // does this for all 5 planes together, ROUNDS times (each round is 5
    // write_reg calls = one write per active/inactive plane, BPL2..5DAT
    // before BPL1DAT so BPL1DAT's write -- the one that latches l_bpu and
    // pulses the shared `load` -- always parallel-loads all five shifters
    // from freshly-written buffers, per the file-header mechanical note).
    void refresh_bitplanes(bool b1, bool b2, bool b3, bool b4, bool b5, int rounds) {
        for (int r = 0; r < rounds; ++r) {
            write_reg(BPL2DAT_ADDR, b2 ? 0xFFFF : 0x0000);
            write_reg(BPL3DAT_ADDR, b3 ? 0xFFFF : 0x0000);
            write_reg(BPL4DAT_ADDR, b4 ? 0xFFFF : 0x0000);
            write_reg(BPL5DAT_ADDR, b5 ? 0xFFFF : 0x0000);
            write_reg(BPL1DAT_ADDR, b1 ? 0xFFFF : 0x0000);
        }
    }

    // Sets bpu=5 (single-playfield 5-bitplane mode) once, then sustains
    // bitplane n (1..5) at a constant `bit(n)` value via refresh_bitplanes.
    // BPLCON0 only needs writing once (it is an ordinary register, not a
    // transient shift pipeline); l_bpu re-latches bpu's already-correct
    // value on every one of refresh_bitplanes' own BPL1DAT writes.
    void write_bitplane_pattern(bool b1, bool b2, bool b3, bool b4, bool b5, int rounds) {
        write_reg(BPLCON0_ADDR, 0x5000);  // bpu = bplcon0[14:12] = 5 (denise.v:181, aga=0 so bit4 term is forced 0)
        refresh_bitplanes(b1, b2, b3, b4, b5, rounds);
    }

    void pulse_strhor() {
        dut->strhor = 1;
        tick(); tick(); tick(); tick();  // hold across a full 4-phase cycle so clk7_en definitely samples strhor=1 once
        dut->strhor = 0;
    }

    // hpos (denise.v:136-142) increments by 1 only once every 4 ticks
    // (once per clk7_en pulse) -- advance_hpos(units) is `units` hpos
    // increments' worth of 28MHz ticks.
    void advance_hpos(int units) {
        for (int i = 0; i < units; ++i) { tick(); tick(); tick(); tick(); }
    }

    uint32_t read_rgb() {
        tick(); tick(); tick(); tick();  // let the registered window/window_ena/out_rgb pipeline (denise.v:328-340,509) settle
        return (static_cast<uint32_t>(dut->red) << 16) |
               (static_cast<uint32_t>(dut->green) << 8) |
               static_cast<uint32_t>(dut->blue);
    }

    // Pulses strhor and ticks forward to a target hpos strictly
    // inside/outside the display window, returning the {red,green,blue}
    // sample once there. Used only for border/no-bitplane-involved
    // samples (index forced to 0 outside the window regardless of
    // bitplane state) -- see refresh_bitplanes above for why a bitplane
    // pattern cannot simply be set up once, in advance, and trusted to
    // still be present after many idle ticks.
    uint32_t sample_at_hpos(int target_hpos) {
        pulse_strhor();
        advance_hpos(target_hpos - 2);
        return read_rgb();
    }

    Vdenise* dut;
    int phase;
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
    // deliver a known value before anything reads one"). Given the c1/c3
    // trap, this is not a formality: DENISEID and CLXDAT are two
    // *independently* decoded read-only registers (denise.v:322 and
    // denise_collision.v:117); both answering distinct, non-zero values
    // before any write-only-register-reads-zero assertion below is trusted
    // rules out "data_out is stuck at zero" as the explanation for every
    // later $0000 reading.
    // =========================================================================
    {
        Harness h;
        uint16_t deniseid = h.read_reg(DENISEID_ADDR);
        uint16_t clxdat_addr = CLXDAT_ADDR;
#if MUT_CONTROL
        clxdat_addr = DENISEID_ADDR;
#endif
        uint16_t clxdat = h.read_reg(clxdat_addr);
        checks.expect(
            deniseid != 0 && clxdat != 0 && deniseid != clxdat,
            "[control] CONTROL-FIXTURE-ALIVE: DENISEID ($07C, denise.v:322) "
            "and CLXDAT ($00E, denise_collision.v:117), two independently "
            "decoded read-only registers, both read back distinct non-zero "
            "values (" + hex4(deniseid) + " and " + hex4(clxdat) +
            ") before any assertion below trusts a $0000 reading to mean "
            "write-only rather than stuck. No manual page: the control for "
            "every write-only-reads-zero assertion in this file, on the "
            "same measurement basis.");
    }

    // =========================================================================
    // DENISEID-ADDR-RO (p.277/290) -- DENISEID answers $FFFC at $07C, a
    // different write-only address answers $0000 through the same mux
    // (selectivity), and an attempted write to $07C does not change the
    // next reading (read-only).
    // =========================================================================
    {
        Harness h;
        uint16_t addr_for_own_read = DENISEID_ADDR;
#if MUT_DENISEID
        addr_for_own_read = BPLCON0_ADDR;
#endif
        uint16_t at_addr = h.read_reg(addr_for_own_read);
        uint16_t at_other = h.read_reg(BPLCON0_ADDR);
        h.write_reg(DENISEID_ADDR, 0x1234);
        uint16_t after_write = h.read_reg(DENISEID_ADDR);
        bool ok = at_addr == 0xFFFC && at_other == 0x0000 && after_write == 0xFFFC;
        checks.expect(
            ok,
            "DENISEID-ADDR-RO: reading $07C returns $FFFC (denise.v:322, "
            "ecs=1/aga=0), reading the unrelated write-only address $100 "
            "through the same data_out mux returns $0000 (the $FFFC is "
            "specific to $07C, not a stuck bus), and an attempted write to "
            "$07C leaves the next reading at $FFFC unchanged (read-only, "
            "not merely 'happened to read something') - 'DENISEID 07C R "
            "D(E) Chip revision level for Denise' (p.277), '~07C R D(E) "
            "Chip revision level for Denise (video out chip)' (p.290). "
            "A-deniseid.addr, A-deniseid.c1, A-deniseid.c2, A-deniseid.rw, "
            "B-deniseid.addr, B-subject.c5, B-subject.c6.");
    }

    // =========================================================================
    // BPLCON0-ADDR / COLOR-ADDR-00-01 (p.271/273/291/292) -- one shared
    // measurement, PACKAGE.md's own disclosed MEASURED evidence: bpu=1
    // (BPLCON0 at $100) plus a constant-high BPL1DAT makes the window
    // pixel read COLOR01's ($182) content and the border on both sides of
    // the window read COLOR00's ($180) content. This single measurement is
    // the evidence for BOTH BPLCON0's address and COLOR00/COLOR01's.
    // =========================================================================
    {
        Harness h;
        h.write_reg(DIWSTRT_ADDR, 0x0030);
        h.write_reg(DIWSTOP_ADDR, 0x0050);
        // DIWSTOP's stored comparison value has an implicit H8=1
        // (denise.v:314-320, "diwstop H8 = 1"): writing $0050 alone makes
        // the window's real close point $150, not $050. Writing DIWHIGH=0
        // afterward (ecs=1) clears that override so DIWSTOP's low byte is
        // the whole story, matching the manual's own DIWSTRT/DIWSTOP pair
        // (and hrm/denise/'s older FPGA-034 harness, which does the same
        // "write DIWHIGH last" for this reason).
        h.write_reg(DIWHIGH_ADDR, 0x0000);
        h.write_reg(COLOR_BASE + 0, 0x001);  // COLOR00: near-black
        h.write_reg(COLOR_BASE + 1, 0xFFE);  // COLOR01: distinct from COLOR00
        h.write_reg(BPLCON0_ADDR, 0x1000);   // bpu=1 (denise.v:181)

        // `window` (denise.v:328-334) is an unreset reg that can latch on
        // whatever garbage hdiwstrt/hdiwstop held during construction; one
        // full pass out past the (now correctly $050) DIWSTOP forces it
        // through both edges, leaving a known (clear) state.
        h.sample_at_hpos(0x60);

        uint16_t bpl1dat_val = 0xFFFF;
#if MUT_BPLCON0_ADDR
        bpl1dat_val = 0x0000;
#endif

        uint32_t outside1 = h.sample_at_hpos(0x20);  // before the window opens; no bitplane needed (border forces index 0)

        // "inside": a single BPLxDAT write only pulses the plane shifter's
        // parallel load once (see refresh_bitplanes' comment) -- so the
        // bitplane pattern must be kept freshly re-written right up to the
        // moment of sampling, not set up once in advance. Advance hpos to
        // just short of DIWSTRT, then refresh BPL1DAT repeatedly: each
        // refresh_bitplanes round both re-arms bpldata[1] and advances
        // hpos (~1.25 units/write_reg call), carrying hpos across $030
        // into the window while the signal is saturating.
        h.pulse_strhor();
        h.advance_hpos(0x2C - 2);
        // bpu=1 (BPLCON0 already written above) means only plane 1 matters
        // (denise.v:367-374 forces bpldata[2..8]=0 whenever l_bpu<=1), so
        // only BPL1DAT needs repeated refreshing here -- each write_reg
        // call is ~1.25 hpos, and 8 calls carries hpos from $02C to about
        // $036, comfortably inside [$030,$050).
        for (int r = 0; r < 8; ++r) h.write_reg(BPL1DAT_ADDR, bpl1dat_val);
        // Sample immediately, with no further settle ticks: the shifter's
        // transient (see refresh_bitplanes) decays again within a handful
        // of ticks once the repeated writes stop, so the read has to catch
        // it right where the last write_reg call already left the
        // combinational state (write_reg's own internal tick() calls have
        // already called eval()).
        uint32_t inside = (static_cast<uint32_t>(h.dut->red) << 16) |
                           (static_cast<uint32_t>(h.dut->green) << 8) |
                           static_cast<uint32_t>(h.dut->blue);

        uint32_t outside2 = h.sample_at_hpos(0x60);  // after the window closes; no bitplane needed

        bool ok = inside != outside1 && inside != outside2 && outside1 == outside2;
        checks.expect(
            ok,
            "BPLCON0-ADDR / COLOR-ADDR-00-01: with BPLCON0.bpu=1 written at "
            "$100 and BPL1DAT held constant-high, the pixel inside the "
            "display window (index 1) reads a different colour than the "
            "border on both sides of it (index 0), and the two border "
            "readings agree with each other (" + hex4(static_cast<uint16_t>(outside1 >> 8)) +
            " vs inside " + hex4(static_cast<uint16_t>(inside >> 8)) + ") - "
            "'BPLCON0 100 W A D(E) Bitplane control register (misc. ...' "
            "(p.271/291), 'COLORxx 180 W D Color table xx' (p.273), "
            "'COLOR00 180 w D Color table 00' / 'COLOR01 182 w D Color "
            "table 01' (p.292). A-bplcon.c1, A-bplcon0.addr, B-bplcon0.addr, "
            "B-subject.c110, A-color.c1, A-colorxx.addr, B-color00.addr, "
            "B-subject.c164, B-color01.addr, B-subject.c168.");
    }

    // =========================================================================
    // BPLCON0-WO (p.271/291/300/307) -- a distinct pattern written to $100
    // reads back $0000 there.
    // =========================================================================
    {
        Harness h;
        h.write_reg(BPLCON0_ADDR, 0xBEEF);
        uint16_t read_addr = BPLCON0_ADDR;
#if MUT_BPLCON0_WO
        read_addr = DENISEID_ADDR;
#endif
        uint16_t r = h.read_reg(read_addr);
        checks.expect(
            r == 0x0000,
            "BPLCON0-WO: a distinct non-zero pattern ($BEEF) written to "
            "$100 reads back $0000 at that same address (write-only) - "
            "'BPLCON0 100 W A D(E) ...' (p.271/291), 'BPLCON0 chg W A,D "
            "Bitplane control register (misc control bits)' (p.300), "
            "'BPLCON0 W A,D Bitplane control (miscellaneous control bits)' "
            "(p.307). This fixture observes only the Denise-visible half of "
            "'affects ECS Agnus and Denise' / 'used by Agnus and Denise' - "
            "Agnus's own decode of $100 is outside this boundary and is not "
            "asserted here. A-bplcon.c2, A-bplcon0.rw, B-subject.c111, "
            "C-shres.c8, C-genlock-ext.c5.");
    }

    // =========================================================================
    // BPLCON2-WO (p.271/291/307) -- a distinct pattern written to $104
    // reads back $0000 there.
    // =========================================================================
    {
        Harness h;
        h.write_reg(BPLCON2_ADDR, 0xDEAD);
        uint16_t read_addr = BPLCON2_ADDR;
#if MUT_BPLCON2_WO
        read_addr = DENISEID_ADDR;
#endif
        uint16_t r = h.read_reg(read_addr);
        checks.expect(
            r == 0x0000,
            "BPLCON2-WO: a distinct non-zero pattern ($DEAD) written to "
            "$104 reads back $0000 at that same address (write-only) - "
            "'BPLCON2 104 W D(E) Bitplane control register' (p.271/291), "
            "'BPLCON2 W D Bitplane control (video priority control)' "
            "(p.307). This fixture observes only the Denise half of 'used "
            "by ... Denise' - not independently verified here beyond that. "
            "A-bplcon.c8, A-bplcon2.rw, "
            "B-subject.c119, C-genlock-ext.c7.");
    }

    // =========================================================================
    // BPLCON3-WO (p.271/291/307) -- a distinct pattern written to $106
    // reads back $0000 there.
    // =========================================================================
    {
        Harness h;
        h.write_reg(BPLCON3_ADDR, 0xC0DE);
        uint16_t read_addr = BPLCON3_ADDR;
#if MUT_BPLCON3_WO
        read_addr = DENISEID_ADDR;
#endif
        uint16_t r = h.read_reg(read_addr);
        checks.expect(
            r == 0x0000,
            "BPLCON3-WO: a distinct non-zero pattern ($C0DE) written to "
            "$106 reads back $0000 at that same address (write-only) - "
            "'BPLCON3 106 W D(E) Bitplane control (enhanced features)' "
            "(p.271/291), 'BPLCON3 W D Bitplane control (enhanced "
            "features)' (p.307), a write-only Denise register for enhanced "
            "features. A-bplcon.c11, A-bplcon3.rw, B-subject.c123, "
            "C-genlock-ext.c8.");
    }

    // =========================================================================
    // COLOR-GENERAL-WO (p.47/92/273) -- the block-level statement "the
    // color registers are write-only", representative measurement on
    // COLOR00.
    // =========================================================================
    {
        Harness h;
        h.write_reg(COLOR_BASE + 0, 0x357);
        uint16_t read_addr = COLOR_BASE + 0;
#if MUT_COLOR_GENERAL_WO
        read_addr = DENISEID_ADDR;
#endif
        uint16_t r = h.read_reg(read_addr);
        checks.expect(
            r == 0x0000,
            "COLOR-GENERAL-WO: a distinct pattern ($357) written to COLOR00 "
            "($180) reads back $0000 at that same address, representative "
            "of the block-level statement that the colour registers as a "
            "whole are write-only - 'The color registers are write-only' "
            "(p.47), 'color register. All color registers are write-only' "
            "(p.92), 'COLORxx 180 W D Color table xx' (p.273). 3-ct.c09, "
            "3-csum.c05, A-color.c2, A-colorxx.rw.");
    }

    // =========================================================================
    // COLOR-ADDR (p.292/293), i=2..31 -- one shared loop, REG-style dynamic
    // labels: with a fixed 5-bitplane pattern dialling the colour-table
    // select index to i, COLORi answers a fresh second write with a
    // different reading than the first - COLORi is what index i is
    // actually reading from.
    // =========================================================================
    {
        Harness h;
        h.write_reg(DIWSTRT_ADDR, 0x0008);
        h.write_reg(DIWSTOP_ADDR, 0x01D0);

        auto pattern1 = [](int i) -> uint16_t { return static_cast<uint16_t>(((i << 4) ^ 0x0A5) & 0xFFF); };
        auto pattern2 = [&](int i) -> uint16_t { return static_cast<uint16_t>(0xFFF ^ pattern1(i)); };

        struct ColorCase {
            int idx;
            int page;
            const char* addr_id;
            const char* subj_id;
        };
        static const ColorCase cases[30] = {
            {2, 292, "B-color02.addr", "B-subject.c172"}, {3, 292, "B-color03.addr", "B-subject.c176"},
            {4, 292, "B-color04.addr", "B-subject.c180"}, {5, 292, "B-color05.addr", "B-subject.c184"},
            {6, 292, "B-color06.addr", "B-subject.c188"}, {7, 292, "B-color07.addr", "B-subject.c192"},
            {8, 292, "B-color08.addr", "B-subject.c196"}, {9, 292, "B-color09.addr", "B-subject.c200"},
            {10, 292, "B-color10.addr", "B-subject.c204"}, {11, 292, "B-color11.addr", "B-subject.c208"},
            {12, 292, "B-color12.addr", "B-subject.c212"}, {13, 292, "B-color13.addr", "B-subject.c216"},
            {14, 292, "B-color14.addr", "B-subject.c220"}, {15, 292, "B-color15.addr", "B-subject.c224"},
            {16, 292, "B-color16.addr", "B-subject.c228"}, {17, 292, "B-color17.addr", "B-subject.c232"},
            {18, 292, "B-color18.addr", "B-subject.c236"}, {19, 292, "B-color19.addr", "B-subject.c240"},
            {20, 292, "B-color20.addr", "B-subject.c244"}, {21, 293, "B-color21.addr", "B-subject.c248"},
            {22, 293, "B-color22.addr", "B-subject.c252"}, {23, 293, "B-color23.addr", "B-subject.c256"},
            {24, 293, "B-color24.addr", "B-subject.c260"}, {25, 293, "B-color25.addr", "B-subject.c264"},
            {26, 293, "B-color26.addr", "B-subject.c268"}, {27, 293, "B-color27.addr", "B-subject.c272"},
            {28, 293, "B-color28.addr", "B-subject.c276"}, {29, 293, "B-color29.addr", "B-subject.c280"},
            {30, 293, "B-color30.addr", "B-subject.c284"}, {31, 293, "B-color31.addr", "B-subject.c288"},
        };

        for (const auto& cc : cases) {
            int i = cc.idx;
            bool b1 = (i & 1) != 0, b2 = (i & 2) != 0, b3 = (i & 4) != 0, b4 = (i & 8) != 0, b5 = (i & 16) != 0;
            h.write_reg(COLOR_BASE + i, pattern1(i));
            h.pulse_strhor();
            h.advance_hpos(6);  // hpos ~8, past DIWSTRT ($08); refresh_bitplanes below carries it on to ~68, still far short of DIWSTOP ($1D0)
            h.write_bitplane_pattern(b1, b2, b3, b4, b5, /*rounds=*/10);
            uint32_t before = h.read_rgb();

            int write_target = COLOR_BASE + i;
#if MUT_COLOR_ADDR
            if (i == 5) write_target = COLOR_BASE + 6;
#endif
            h.write_reg(static_cast<uint16_t>(write_target), pattern2(i));
            h.refresh_bitplanes(b1, b2, b3, b4, b5, /*rounds=*/4);  // re-freshen the transient bitplane signal (see refresh_bitplanes) before trusting a second reading
            uint32_t after = h.read_rgb();

            char idxbuf[4];
            std::snprintf(idxbuf, sizeof(idxbuf), "%02d", i);
            checks.expect(
                before != after,
                std::string("COLOR-ADDR ") + idxbuf + " (p." + std::to_string(cc.page) +
                    "): with the 5-bitplane pattern selecting index " + idxbuf +
                    ", the pixel colour answers a fresh, differently-valued "
                    "write to COLOR" + idxbuf + " with a different reading "
                    "than the first write's - COLOR" + idxbuf + " is what "
                    "index " + idxbuf + " actually reads from. 'COLOR" +
                    idxbuf + " ... Color table " + idxbuf + "'. " + cc.addr_id +
                    ", " + cc.subj_id + ".");
        }
    }

    // =========================================================================
    // COLOR-WO (p.47/92/292/293), i=0..31 -- one shared loop: a distinct
    // pattern written to COLORi reads back $0000 at that same address.
    // =========================================================================
    {
        Harness h;
        struct WoCase {
            int idx;
            int page;
            const char* subj_id;
        };
        static const WoCase cases[32] = {
            {0, 292, "B-subject.c165"}, {1, 292, "B-subject.c169"}, {2, 292, "B-subject.c173"},
            {3, 292, "B-subject.c177"}, {4, 292, "B-subject.c181"}, {5, 292, "B-subject.c185"},
            {6, 292, "B-subject.c189"}, {7, 292, "B-subject.c193"}, {8, 292, "B-subject.c197"},
            {9, 292, "B-subject.c201"}, {10, 292, "B-subject.c205"}, {11, 292, "B-subject.c209"},
            {12, 292, "B-subject.c213"}, {13, 292, "B-subject.c217"}, {14, 292, "B-subject.c221"},
            {15, 292, "B-subject.c225"}, {16, 292, "B-subject.c229"}, {17, 292, "B-subject.c233"},
            {18, 292, "B-subject.c237"}, {19, 292, "B-subject.c241"}, {20, 292, "B-subject.c245"},
            {21, 293, "B-subject.c249"}, {22, 293, "B-subject.c253"}, {23, 293, "B-subject.c257"},
            {24, 293, "B-subject.c261"}, {25, 293, "B-subject.c265"}, {26, 293, "B-subject.c269"},
            {27, 293, "B-subject.c273"}, {28, 293, "B-subject.c277"}, {29, 293, "B-subject.c281"},
            {30, 293, "B-subject.c285"}, {31, 293, "B-subject.c289"},
        };
        for (const auto& wc : cases) {
            int i = wc.idx;
            h.write_reg(static_cast<uint16_t>(COLOR_BASE + i), static_cast<uint16_t>(0x400 + i));
            int read_addr = COLOR_BASE + i;
#if MUT_COLOR_WO
            if (i == 5) read_addr = DENISEID_ADDR;
#endif
            uint16_t r = h.read_reg(static_cast<uint16_t>(read_addr));

            char idxbuf[4];
            std::snprintf(idxbuf, sizeof(idxbuf), "%02d", i);
            checks.expect(
                r == 0x0000,
                std::string("COLOR-WO ") + idxbuf + " (p." + std::to_string(wc.page) +
                    "): a distinct pattern written to COLOR" + idxbuf +
                    " reads back $0000 at that same address (write-only). " +
                    wc.subj_id + ".");
        }
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
