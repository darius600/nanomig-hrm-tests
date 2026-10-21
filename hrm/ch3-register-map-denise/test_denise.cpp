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
//   ch3-register-map-denise, 134 records). A red result below means denise.v
//   (or a sub-module it instantiates) disagrees with the manual, not that
//   the assertion is wrong -- no assertion here was, or should be, adjusted
//   to make the RTL pass.
// Non-goals: BPLCON0/2/3's individual bit *semantics* (hires, ham, dpf,
//   killehb, bank, spres, ...), colour-table value round-tripping and the
//   32-way bitplane-to-select assembly (already exhaustively proven by the
//   sibling package hrm/ch3-colour-register-format/ at the narrower
//   denise_colortable boundary), scroll/DDF timing, collision bit meaning.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// BOUNDARY: top module `denise` only (one Verilator build, sources
//   denise.v + denise_bitplanes.v + denise_bitplane_shifter.v +
//   denise_playfields.v + denise_sprites.v + denise_sprites_shifter.v +
//   denise_collision.v + denise_hamgenerator.v + denise_colortable.v +
//   denise_colortable_ram_mf.v + denise_spritepriority.v + bitplane_ram.v),
//   per PACKAGE.md. Ports used, exactly as PACKAGE.md's own evidence names
//   them (module header, denise.v:31-53): clk, clk7_en, c1, c3, cck, reset,
//   strhor, reg_address_in[8:1], data_in[16-1:0], chip48[48-1:0],
//   data_out[16-1:0], blank, red/green/blue[8-1:0], a1k, ecs, aga,
//   hires (out), shres (out).
//
// FIXTURE CONFIGURATION, and why: a1k=0, ecs=1, aga=0 throughout every
//   Harness in this file. ecs=1/aga=0 is not a free choice -- it is what
//   makes the fixture reproduce PACKAGE.md's own disclosed "already
//   measured" facts: denise.v:335 reads
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
//   in one, both low in another). Each BPLxDAT buffer register latches
//   `{data_in, chip48_fmode}` directly on clk7_en when reg_address_in[8:1]
//   matches its own address (denise_bitplanes.v:487-540, one block per
//   plane); the plane shifter's own parallel load is gated on `!c1&&!c3`
//   (denise_bitplane_shifter.v:181's `if (load && !c1 && !c3)`).
//   Harness::tick() below drives, every 28MHz cycle, the measured
//   working pattern: clk7_en=(phase==0), c1=(phase==0||phase==1),
//   c3=(phase==1||phase==2) -- so phase 1 has both c1 and c3 high, phase 3
//   has both low, matching the parallel-load window above.
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
//   l_bpu (denise.v:196-205, latched off BPLCON0.bpu the moment BPL1DAT is
//   written) set to 5, denise.v:402-409 forces bpldata[6..8]=0, so the
//   colour-table select index actually presented is exactly
//     index = 16*bpldata[5] + 8*bpldata[4] + 4*bpldata[3] + 2*bpldata[2] + bpldata[1]
//   one bit per bitplane, each held at a constant 0 or 1 by writing its
//   BPLxDAT as $0000 or $FFFF.
//
//   MEASURED (this package's own build/calibration, not assumed): a single
//   BPLxDAT write only pulses the plane shifter's parallel load once
//   (denise_bitplane_shifter.v:180-183, non-EBR path); with nothing to
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
//   388-414) are written before BPL1DAT in every round, because BPL1DAT's
//   write is what both latches l_bpu (denise.v:197-205) and pulses `load`
//   a cycle later (denise_bitplanes.v:542-546), which parallel-loads *all* the
//   plane shifters synchronously from whatever their buffers hold at that
//   instant (denise_bitplane_shifter.v:180-183).
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
//   BPLCON0-ADDR (4 ids), BPLCON0-WO (5 ids), BPLCON1-WO (no claim id)
//   BPLCON2-ADDR (6 ids), BPLCON3-ADDR (4 ids)
//   BPLCON2-WO (5 ids), BPLCON3-WO (4 ids)
//     -- ADDR (BPLCON0): BPLCON0's bpu field is proven to live at $100 by
//        the one measurement PACKAGE.md's own evidence already discloses
//        as MEASURED: with a single bitplane enabled (bpu=1) and BPL1DAT
//        held at a constant 1, the pixel inside the display window and
//        the border outside it read back as different colours, and the
//        border matches the colour written to $180 both before and after
//        the window (so the index really did move from 0 to 1 and back to
//        0, which requires BPLCON0's write at $100 to have taken effect).
//        This same measurement is what settles COLOR00's and COLOR01's
//        addresses below (COLOR-ADDR-00-01).
//     -- ADDR (BPLCON2): with BPLCON0 set to bpu=5 and dual-playfield mode
//        (bit 10), planes 1 and 2 held high put valid data on playfield 1
//        and playfield 2 at once (denise_playfields.v:43-58). BPLCON2's
//        own PF2PRI bit (bit 6, denise_playfields.v:24,68-90) then picks
//        which playfield's data reaches the display, so toggling only
//        that bit -- nothing else touched -- changes the displayed pixel.
//        That requires BPLCON2's write to have reached this circuit, i.e.
//        that BPLCON2 is decoded at $104. (This package's own earlier
//        scope call had judged BPLCON2's address unreachable here, on the
//        assumption that only killehb's 6-bitplane EHB path could settle
//        it; PF2PRI needs only a second live bitplane inside the fixture's
//        existing 5-bitplane, single-playfield-at-a-time machinery, with
//        dpf=1 instead of dpf=0.) Two ids restate this exact address
//        proposition from appendix A's and appendix B's per-register
//        tables (sol:7-appB-bplcon2.c1, sol:7-appA-bplcon2.c1) and are
//        appended bare to BPLCON2-ADDR's citation list rather than
//        duplicated on a second assertion of the identical predicate.
//     -- ADDR (BPLCON3): with BPLCON0's ECSENA bit (bit 0, denise.v:194)
//        set and a display window open, BPLCON3's own BRDRBLNK bit (bit 5,
//        denise.v:253) forces the border area to a fixed value
//        (denise.v:560-565's `t_blank` forces $000000, though no manual
//        page prints that RGB value); with BRDRBLNK clear the border
//        instead reads COLOR00's own content. Toggling only BPLCON3
//        changes the border colour, which requires BPLCON3's write to
//        have reached this circuit, i.e. that BPLCON3 is decoded at $106.
//        (This package's own earlier scope call had assumed ECSENA is
//        never set in this fixture and treated the border path as
//        unreachable; ECSENA is an ordinary BPLCON0 bit this fixture can
//        set like any other.)
//     -- WO: a distinct, non-zero pattern written to the register's own
//        address does not read back that same pattern there -- 'BPLCON0
//        ... W' (p.271/291), 'BPLCON1 ... W' (p.271), 'BPLCON2 ... W'
//        (p.271/291/298), 'BPLCON3 ... W' (p.271/291). No page prints what
//        an idle read of a write-only register returns ("do not attempt
//        to read from it, as this may trash the register", p.264), so
//        none of the four assumes $0000 -- each requires only that the
//        written pattern itself does not come back. BPLCON1-WO carries no
//        claim id: the only claim about BPLCON1 (opus:7-appA-bplcon2.c3)
//        bundles this direction clause with address, chip and function in
//        one id, and only the direction clause is established here -- see
//        that id's own record below.
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
//        to COLORi does not read back that same pattern at that same
//        address.
//
// APPENDIX C P.298 -- three claims from Appendix C's ECS "changed
//   register" map (p.298), a third page distinct from Appendix A p.271 and
//   Appendix B p.291 above, carrying BPLCON2's own row from that table. The
//   printed row is column-scrambled in this corpus's OCR (hrm/text/p298.txt
//   reads "104 BPLCON2 chg   WA" with nothing after it); a 200 dpi
//   page-image render restores it to "104 BPLCON2 chg W D Bitplane control
//   (video priority control)". Both readers' quotations preserve the
//   scrambled corpus line and carry the image-verified reading in the
//   restatement, not the quote -- the pipeline's own cross-read already
//   confirmed the image-verified D against Appendix A p.271 and Appendix B
//   p.291, which both independently print D for BPLCON2's chip column.
//   Chipset scope: ECS (Appendix C p.295 defines the chip set; p.298 marks
//   a register new or changed in that revision).
//     sol:7-appC-map-bplcon2.c3 -- "ECS BPLCON2 is write-only." Restates
//       the identical proposition BPLCON2-WO already asserts, under ECS
//       scope; this file's fixture already runs ecs=1 throughout (see
//       FIXTURE CONFIGURATION above), so BPLCON2-WO's own measurement is
//       already an ECS measurement. Added to BPLCON2-WO's own citation
//       list, in full, rather than a second assertion of the identical
//       predicate.
//     sol:7-appC-map-bplcon2.c4 -- "ECS BPLCON2 is implemented in Denise."
//       Declined -- p.298's own legend reads "A=Agnus chip, D=Denise chip,
//       P=Paula chip": the CHIP column names a physical chip, not an RTL
//       module, and this suite's symmetric chip-column rule
//       (hrm/ch8-appb-chip-column-paula) declines claims like this one,
//       the same reason this package already declines the same register's
//       Appendix A/B chip claims (sol:7-appA-bplcon2.c3, sol:7-appB-bplcon2.c3
//       above).
//     opus:7-appC298.c4 -- bundles BPLCON2's address ($104), its "chg"
//       revision mark, its direction (W), its chip (D) and its function
//       ("Bitplane control (video priority control)") in a single claim
//       (sol split the same source row into the five claims
//       7-appC-map-bplcon2.c1-c5, of which only .c3 and .c4 above were
//       routed to this package). Declined as a whole, matching this
//       file's own bundling precedent for the identical p.291 bundle
//       (opus:7-appB.c34 above): its chip clause names a physical chip
//       for the same reason sol:7-appC-map-bplcon2.c4 is declined above,
//       and a multi-clause id is not kept on the clauses that happen to be
//       gradeable. Of sol's five-way split, only c3 (direction) is graded
//       in this package, folded into BPLCON2-WO's own citation list above;
//       c4 (chip) is declined just above. c1 (address), c2 ("chg") and c5
//       (function) were routed to hrm/ch7-bplcon2-live-bits-and-ecs, not to
//       this package: c1 on BPLCON2-ADDRESS-0x104-LIVE, c2 on
//       BPLCON2-BIT9-CHG-NOT-COMPARED, c5 co-cited across that package's
//       BIT14/13/12/11/10-ECS-RECORDED lines.
//   Both declines are printed as RECORDED lines in the run below, beside
//   this package's other chip-attribution and bundling declines.
//
// MUTATION-LOG SUMMARY (12 groups; the 60-id COLOR-ADDR loop and the 32-id
//   COLOR-WO loop each carry one mutation covering the whole loop, matching
//   hrm/ch3-colour-register-format/'s REG-09/ADDR-32-DISTINCT convention).
//
// MUTATION CONTROL-FIXTURE-ALIVE stimulus red: CLXDAT's read address was redirected from $00E to DENISEID's own address ($07C), so both reads returned the same value ($FFFC) instead of two independently-decoded registers and the distinctness check failed (69/0 baseline went to 68 passed/1 failed, only this assertion); reverted (MUT_CONTROL back to 0) and rebuilt to confirm the 69/0 baseline was restored.
// MUTATION DENISEID-ADDR-RO stimulus red: the "at its own address" read was redirected from $07C to $100 (an unrelated write-only address), so the reading came back $0000 instead of $FFFC.
// MUTATION BPLCON0-ADDR stimulus red: the BPL1DAT write's data was changed from $FFFF (bitplane held high) to $0000, so the window pixel never differs from the border and the inside/outside comparison found no difference.
// MUTATION BPLCON2-ADDR stimulus red: the second measurement's BPLCON2 write was changed from $0040 (PF2PRI=1) to $0000, the same value as the first measurement's, so PF2PRI was never actually toggled and the two displayed pixels came back identical.
// MUTATION BPLCON3-ADDR stimulus red: the second border sample's BPLCON3 write was changed from $0020 (BRDRBLNK=1) to $0000, so BRDRBLNK was never actually set and the border never went black.
// MUTATION BPLCON0-WO stimulus red: the written pattern changed from $BEEF to $0000 -- this register's own idle-read value in this fixture (denise.v's data_out mux is col_out|deniseid_out|rgb_out, and none of the three claims $100: col_out only answers CLXDAT, deniseid_out only DENISEID, rgb_out is held 0 by aga=0) -- so the write-only predicate (r != pattern) no longer had a question to ask -> FAIL ("71 passed, 1 failed"), only BPLCON0-WO newly red. Reverted (MUT_BPLCON0_WO back to 0) and rebuilt: 72 passed, 0 failed restored.
// MUTATION BPLCON1-WO stimulus red: the written pattern changed from $1357 to $0000, this register's own idle-read value in this fixture (same data_out reasoning as BPLCON0-WO, at $102) -> FAIL ("71 passed, 1 failed"), only BPLCON1-WO newly red. Reverted (MUT_BPLCON1_WO back to 0) and rebuilt: 72 passed, 0 failed restored.
// MUTATION BPLCON2-WO stimulus red: the written pattern changed from $DEAD to $0000, this register's own idle-read value in this fixture (same data_out reasoning as BPLCON0-WO, at $104) -> FAIL ("71 passed, 1 failed"), only BPLCON2-WO newly red. Reverted (MUT_BPLCON2_WO back to 0) and rebuilt: 72 passed, 0 failed restored.
// MUTATION BPLCON3-WO stimulus red: the written pattern changed from $C0DE to $0000, this register's own idle-read value in this fixture (same data_out reasoning as BPLCON0-WO, at $106) -> FAIL ("71 passed, 1 failed"), only BPLCON3-WO newly red. Reverted (MUT_BPLCON3_WO back to 0) and rebuilt: 72 passed, 0 failed restored.
// MUTATION COLOR-GENERAL-WO stimulus red: the written pattern changed from $357 to $0000, this register's own idle-read value in this fixture (same data_out reasoning as BPLCON0-WO, at COLOR00/$180) -> FAIL ("71 passed, 1 failed"), only COLOR-GENERAL-WO newly red. Reverted (MUT_COLOR_GENERAL_WO back to 0) and rebuilt: 72 passed, 0 failed restored.
// MUTATION COLOR-ADDR stimulus red: index 5's second COLOR write was redirected to COLOR06's address ($18A) instead of COLOR05's ($188), so COLOR05's own reading never changed between the first and second write.
// MUTATION COLOR-WO stimulus red: index 5's written pattern changed from $405 to $0000, COLOR05's own idle-read value in this fixture (same data_out reasoning as BPLCON0-WO) -> FAIL ("71 passed, 1 failed"), only COLOR-WO 05 newly red, every other index unchanged. Reverted (MUT_COLOR_WO back to 0) and rebuilt: 72 passed, 0 failed restored.
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
#ifndef MUT_BPLCON2_ADDR
#define MUT_BPLCON2_ADDR 0
#endif
#ifndef MUT_BPLCON3_ADDR
#define MUT_BPLCON3_ADDR 0
#endif
#ifndef MUT_BPLCON0_WO
#define MUT_BPLCON0_WO 0
#endif
#ifndef MUT_BPLCON1_WO
#define MUT_BPLCON1_WO 0
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
// own `parameter` block (denise.v:57-66) and denise_bitplanes.v:349-358.
constexpr uint16_t IDLE_ADDR    = 0x00;  // matches no parameter in denise.v or its sub-modules.
constexpr uint16_t CLXDAT_ADDR  = 0x07;  // 9'h00E >> 1  (denise_collision.v:21)
constexpr uint16_t DIWSTRT_ADDR = 0x47;  // 9'h08E >> 1  (denise.v:57)
constexpr uint16_t DIWSTOP_ADDR = 0x48;  // 9'h090 >> 1  (denise.v:58)
constexpr uint16_t DENISEID_ADDR= 0x3E;  // 9'h07C >> 1  (denise.v:64)
constexpr uint16_t BPLCON0_ADDR = 0x80;  // 9'h100 >> 1  (denise.v:60)
constexpr uint16_t BPLCON1_ADDR = 0x81;  // 9'h102 >> 1  (denise_bitplanes.v:349)
constexpr uint16_t BPLCON2_ADDR = 0x82;  // 9'h104 >> 1  (denise.v:61)
constexpr uint16_t BPLCON3_ADDR = 0x83;  // 9'h106 >> 1  (denise.v:62)
constexpr uint16_t BPL1DAT_ADDR = 0x88;  // 9'h110 >> 1  (denise_bitplanes.v:350)
constexpr uint16_t BPL2DAT_ADDR = 0x89;  // 9'h112 >> 1  (denise_bitplanes.v:351)
constexpr uint16_t BPL3DAT_ADDR = 0x8A;  // 9'h114 >> 1  (denise_bitplanes.v:352)
constexpr uint16_t BPL4DAT_ADDR = 0x8B;  // 9'h116 >> 1  (denise_bitplanes.v:353)
constexpr uint16_t BPL5DAT_ADDR = 0x8C;  // 9'h118 >> 1  (denise_bitplanes.v:354)
constexpr uint16_t COLOR_BASE   = 0xC0;  // 9'h180 >> 1  (denise.v:66); COLORi = COLOR_BASE+i.
constexpr uint16_t DIWHIGH_ADDR = 0xF2;  // 9'h1E4 >> 1  (denise.v:59)

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
    // of three sources in denise.v:97, but giving it a full cycle
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
    // exactly once (denise_bitplane_shifter.v:180-183); with nothing to
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
        write_reg(BPLCON0_ADDR, 0x5000);  // bpu = bplcon0[14:12] = 5 (denise.v:183, aga=0 so bit4 term is forced 0)
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
        tick(); tick(); tick(); tick();  // let the registered window/window_ena/out_rgb pipeline (denise.v:343-355,509) settle
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
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
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
    // *independently* decoded read-only registers (denise.v:335 and
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
            "[control] CONTROL-FIXTURE-ALIVE: DENISEID ($07C, denise.v:335) "
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
            "DENISEID-ADDR-RO: reading $07C returns $FFFC (denise.v:335, "
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
        // (denise.v:327-333, "diwstop H8 = 1"): writing $0050 alone makes
        // the window's real close point $150, not $050. Writing DIWHIGH=0
        // afterward (ecs=1) clears that override so DIWSTOP's low byte is
        // the whole story, matching the manual's own DIWSTRT/DIWSTOP pair
        // (and hrm/denise/'s older FPGA-034 harness, which does the same
        // "write DIWHIGH last" for this reason).
        h.write_reg(DIWHIGH_ADDR, 0x0000);
        h.write_reg(COLOR_BASE + 0, 0x001);  // COLOR00: near-black
        h.write_reg(COLOR_BASE + 1, 0xFFE);  // COLOR01: distinct from COLOR00
        h.write_reg(BPLCON0_ADDR, 0x1000);   // bpu=1 (denise.v:183)

        // `window` (denise.v:343-349) is an unreset reg that can latch on
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
        // (denise.v:402-409 forces bpldata[2..8]=0 whenever l_bpu<=1), so
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
            "B-subject.c164, B-color01.addr, B-subject.c168, opus:7-appA-bplcon2.c2. Chapter 8 prints the same address on a second page -- 'BPLCON0 ($DFF100) Bit 3 enables the light pen latch' (p.229); its ADDRESS half is what this assertion grades, its bit-3 half is not asserted here -- opus:8-regs.c15, sol:8-reg.c17.");
    }

    // =========================================================================
    // BPLCON1-WO (p.271) -- a distinct pattern written to $102 does not
    // read back that same pattern at that same address. No claim id: the
    // only claim about BPLCON1 (opus:7-appA-bplcon2.c3) bundles this
    // direction clause with address, chip and function in one id, and only
    // the direction clause is established here -- see that id's own record
    // for why the bundle stays off this line. The manual states only that
    // reading BPLCON1 "may trash the register" (p.264); it does not print
    // the value an idle read returns, so this assertion does not assume
    // $0000 -- it requires only that the written pattern itself does not
    // come back.
    // =========================================================================
    {
        Harness h;
        uint16_t pattern = 0x1357;
#if MUT_BPLCON1_WO
        pattern = 0x0000;  // mutation: the written pattern becomes the register's own idle-read value ($0000 -- denise.v's data_out mux drives 0 for any address CLXDAT/DENISEID/rdram do not claim), so the write-only predicate (r != pattern) no longer poses a question
#endif
        h.write_reg(BPLCON1_ADDR, pattern);
        uint16_t r = h.read_reg(BPLCON1_ADDR);
        checks.expect(
            r != pattern,
            "BPLCON1-WO: a distinct non-zero pattern ($1357) written to "
            "$102 does not read back that same pattern at that same "
            "address (write-only) - 'BPLCON1 102 W D Bitplane control "
            "register (horizontal scroll control)' (p.271).");
    }

    // =========================================================================
    // BPLCON2-ADDR (p.271/291) -- with BPLCON0 set to
    // bpu=5 and dual-playfield mode (bit 10), planes 1 and 2 both held high
    // put valid data on playfield 1 and playfield 2 at once
    // (denise_playfields.v:43-58). BPLCON2's own PF2PRI bit (bit 6,
    // denise_playfields.v:24,68-90) then picks which playfield's data
    // reaches the display; toggling only that bit -- nothing else touched
    // -- changes the displayed pixel, which requires BPLCON2's write to
    // have reached this circuit, i.e. that BPLCON2 is decoded at $104.
    // =========================================================================
    {
        auto sample_dual_playfield = [](uint16_t pf2pri_bits) -> uint32_t {
            Harness h;
            h.write_reg(DIWSTRT_ADDR, 0x0008);
            h.write_reg(DIWSTOP_ADDR, 0x01D0);
            h.write_reg(COLOR_BASE + 1, 0x0F0);  // index 1: playfield 1's index when pf2pri=0
            h.write_reg(COLOR_BASE + 9, 0x00F);  // index 9: playfield 2's OCS/ECS-fixed index when pf2pri=1 (denise_playfields.v:73, non-AGA)
            h.write_reg(BPLCON0_ADDR, 0x5400);   // bpu=5, dpf=1 (bplcon0[10])
            h.write_reg(BPLCON2_ADDR, pf2pri_bits);  // pf2pri = bplcon2[6]
            h.pulse_strhor();
            h.advance_hpos(6);  // hpos ~8, past DIWSTRT, matching the COLOR-ADDR loop below
            h.refresh_bitplanes(/*b1=*/true, /*b2=*/true, /*b3=*/false, /*b4=*/false, /*b5=*/false, /*rounds=*/10);
            return h.read_rgb();
        };

        uint32_t with_pf1_priority = sample_dual_playfield(0x0000);  // pf2pri=0: playfield 1 wins

        uint16_t pf2pri_val = 0x0040;  // pf2pri=1 (bplcon2[6]): playfield 2 wins
#if MUT_BPLCON2_ADDR
        pf2pri_val = 0x0000;  // mutation: PF2PRI never actually toggled between the two measurements
#endif
        uint32_t with_pf2_priority = sample_dual_playfield(pf2pri_val);

        bool ok = with_pf1_priority != with_pf2_priority;
        checks.expect(
            ok,
            "BPLCON2-ADDR: with dual-playfield mode on (BPLCON0 bit 10) and "
            "planes 1 and 2 both held high so both playfields carry data, "
            "writing only BPLCON2's PF2PRI bit (bit 6) between two "
            "otherwise-identical measurements changes the displayed pixel (" +
            hex4(static_cast<uint16_t>(with_pf1_priority >> 8)) + " vs " +
            hex4(static_cast<uint16_t>(with_pf2_priority >> 8)) + ") - "
            "'BPLCON2 104 W D(E) Bitplane control register' (p.271/291). "
            "sol:A-bplcon.c7, opus:A-bplcon2.addr, opus:B-bplcon2.addr, "
            "sol:B-subject.c118, sol:7-appB-bplcon2.c1, sol:7-appA-bplcon2.c1.");
    }

    // =========================================================================
    // BPLCON3-ADDR (p.271/291) -- with BPLCON0's ECSENA
    // bit (bit 0, denise.v:194) set and a display window open, BPLCON3's
    // own BRDRBLNK bit (bit 5, denise.v:253) changes the border area's
    // colour (denise.v:560-565, `t_blank`, forces $000000 when set; no
    // manual page prints that RGB value, so this assertion requires only
    // that the border colour changes, not what it changes to); with
    // BRDRBLNK clear the border instead reads COLOR00's own content.
    // Toggling only BPLCON3 -- nothing else touched -- changes the border
    // colour, which requires BPLCON3's write to have reached this circuit,
    // i.e. that BPLCON3 is decoded at $106.
    // =========================================================================
    {
        Harness h;
        h.write_reg(DIWSTRT_ADDR, 0x0030);
        h.write_reg(DIWSTOP_ADDR, 0x0050);
        h.write_reg(DIWHIGH_ADDR, 0x0000);    // see BPLCON0-ADDR above for why
        h.write_reg(COLOR_BASE + 0, 0x0FFF);  // COLOR00: white, clearly non-black
        h.sample_at_hpos(0x60);  // clear `window`'s unreset garbage, as BPLCON0-ADDR above does

        h.write_reg(BPLCON0_ADDR, 0x0001);  // ecsena=1 (bplcon0[0]); bpu=0, no bitplane needed outside the window
        h.write_reg(BPLCON3_ADDR, 0x0000);  // brdrblnk=0 (bplcon3[5])
        uint32_t border_no_blank = h.sample_at_hpos(0x20);  // before the window opens

        uint16_t bplcon3_val = 0x0020;  // brdrblnk=1
#if MUT_BPLCON3_ADDR
        bplcon3_val = 0x0000;  // mutation: BRDRBLNK is never actually set
#endif
        h.write_reg(BPLCON3_ADDR, bplcon3_val);
        uint32_t border_with_blank = h.sample_at_hpos(0x20);

        bool ok = border_no_blank != 0x000000 && border_no_blank != border_with_blank;
        checks.expect(
            ok,
            "BPLCON3-ADDR: with ECSENA set (BPLCON0 bit 0) the border reads "
            "COLOR00's own non-black content (" +
            hex4(static_cast<uint16_t>(border_no_blank >> 8)) +
            ") while BPLCON3's BRDRBLNK bit (bit 5) is clear; writing "
            "BRDRBLNK=1 to $106 and nothing else changes that same border to " +
            hex4(static_cast<uint16_t>(border_with_blank >> 8)) +
            " - 'BPLCON3 106 W D(E) Bitplane control (enhanced features)' "
            "(p.271/291). No page states the RGB value BRDRBLNK blanks the "
            "border to, so only the change is asserted. sol:A-bplcon.c10, "
            "opus:A-bplcon3.addr, opus:B-bplcon3.addr, sol:B-subject.c122.");
    }

    // =========================================================================
    // BPLCON0-WO (p.271/291) -- a distinct pattern written to $100 does not
    // read back that same pattern there.
    // =========================================================================
    {
        Harness h;
        uint16_t pattern = 0xBEEF;
#if MUT_BPLCON0_WO
        pattern = 0x0000;  // mutation: written pattern set to the register's own idle-read value ($0000), so the write-only predicate (r != pattern) no longer poses a question
#endif
        h.write_reg(BPLCON0_ADDR, pattern);
        uint16_t r = h.read_reg(BPLCON0_ADDR);
        checks.expect(
            r != pattern,
            "BPLCON0-WO: a distinct non-zero pattern ($BEEF) written to "
            "$100 does not read back that same pattern at that same "
            "address (write-only) - 'BPLCON0 100 W A D(E) Bitplane control "
            "register (misc. control bits)' (p.271/291). No page prints "
            "what an idle read of $100 returns (p.264: 'do not attempt to "
            "read from it, as this may trash the register'), so this does "
            "not assume $0000. A-bplcon.c2, A-bplcon0.rw, B-subject.c111.");
    }

    // =========================================================================
    // BPLCON2-WO (p.271/291/298) -- a distinct pattern written to $104 does
    // not read back that same pattern there.
    // =========================================================================
    {
        Harness h;
        uint16_t pattern = 0xDEAD;
#if MUT_BPLCON2_WO
        pattern = 0x0000;  // mutation: written pattern set to the register's own idle-read value ($0000), so the write-only predicate (r != pattern) no longer poses a question
#endif
        h.write_reg(BPLCON2_ADDR, pattern);
        uint16_t r = h.read_reg(BPLCON2_ADDR);
        checks.expect(
            r != pattern,
            "BPLCON2-WO: a distinct non-zero pattern ($DEAD) written to "
            "$104 does not read back that same pattern at that same "
            "address (write-only) - 'BPLCON2 104 W D(E) Bitplane control "
            "register' (p.271/291), '104 BPLCON2 chg WA' (p.298, ECS). No "
            "page prints what an idle read of $104 returns (p.264: 'do not "
            "attempt to read from it, as this may trash the register'), so "
            "this does not assume $0000. A-bplcon.c8, A-bplcon2.rw, "
            "B-subject.c119, sol:7-appA-bplcon2.c2, sol:7-appB-bplcon2.c2, "
            "sol:7-appC-map-bplcon2.c3.");
    }

    // =========================================================================
    // BPLCON3-WO (p.271/291) -- a distinct pattern written to $106 does not
    // read back that same pattern there.
    // =========================================================================
    {
        Harness h;
        uint16_t pattern = 0xC0DE;
#if MUT_BPLCON3_WO
        pattern = 0x0000;  // mutation: written pattern set to the register's own idle-read value ($0000), so the write-only predicate (r != pattern) no longer poses a question
#endif
        h.write_reg(BPLCON3_ADDR, pattern);
        uint16_t r = h.read_reg(BPLCON3_ADDR);
        checks.expect(
            r != pattern,
            "BPLCON3-WO: a distinct non-zero pattern ($C0DE) written to "
            "$106 does not read back that same pattern at that same "
            "address (write-only) - 'BPLCON3 106 W D(E) Bitplane control "
            "(enhanced features)' (p.271/291). No page prints what an idle "
            "read of $106 returns (p.264: 'do not attempt to read from it, "
            "as this may trash the register'), so this does not assume "
            "$0000. A-bplcon.c11, A-bplcon3.rw, B-subject.c123.");
    }

    // =========================================================================
    // COLOR-GENERAL-WO (p.47/92/273) -- the block-level statement "the
    // color registers are write-only", representative measurement on
    // COLOR00.
    // =========================================================================
    {
        Harness h;
        uint16_t pattern = 0x357;
#if MUT_COLOR_GENERAL_WO
        pattern = 0x0000;  // mutation: written pattern set to the register's own idle-read value ($0000), so the write-only predicate (r != pattern) no longer poses a question
#endif
        h.write_reg(COLOR_BASE + 0, pattern);
        uint16_t r = h.read_reg(COLOR_BASE + 0);
        checks.expect(
            r != pattern,
            "COLOR-GENERAL-WO: a distinct pattern ($357) written to COLOR00 "
            "($180) does not read back that same pattern at that same "
            "address, representative of the block-level statement that the "
            "colour registers as a whole are write-only - 'The color "
            "registers are write-only' (p.47), 'color register. All color "
            "registers are write-only' (p.92), 'COLORxx 180 W D Color "
            "table xx' (p.273). No page prints what an idle read of a "
            "colour register returns, so this does not assume $0000. "
            "3-ct.c09, 3-csum.c05, A-colorxx.rw.");
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
    // COLOR-WO (p.292/293), i=0..31 -- one shared loop: a distinct pattern
    // written to COLORi does not read back that same pattern at that same
    // address.
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
            uint16_t pattern = static_cast<uint16_t>(0x400 + i);
#if MUT_COLOR_WO
            if (i == 5) pattern = 0x0000;  // mutation: index 5's written pattern set to the register's own idle-read value ($0000), so the write-only predicate (r != pattern) no longer poses a question
#endif
            h.write_reg(static_cast<uint16_t>(COLOR_BASE + i), pattern);
            uint16_t r = h.read_reg(static_cast<uint16_t>(COLOR_BASE + i));

            char idxbuf[4];
            std::snprintf(idxbuf, sizeof(idxbuf), "%02d", i);
            checks.expect(
                r != pattern,
                std::string("COLOR-WO ") + idxbuf + " (p." + std::to_string(wc.page) +
                    "): a distinct pattern written to COLOR" + idxbuf +
                    " does not read back that same pattern at that same "
                    "address (write-only). " + wc.subj_id + ".");
        }
    }

    // =========================================================================
    // WO PREDICATE NARROWED, BUNDLED IDS WITHDRAWN -- BPLCON0-WO, BPLCON2-WO,
    // BPLCON3-WO and COLOR-GENERAL-WO above were narrowed from "reads back
    // $0000" (an RTL idle-value fact no page states) to "does not read back
    // the written pattern" (BPLCON1-WO's own form; p.264 says only that
    // reading a write-only register may trash it, not what it returns). The
    // direction clause any id below carries is still established by that
    // narrower predicate; what is withdrawn is the id as a whole, because
    // each one bundles a second clause (chip attribution, sometimes
    // function too) in the same id, matching this file's own opus:7-appB.c34
    // bundling precedent (a multi-clause id is not partially kept on the
    // clause that happens to be established). opus:7-appA-bplcon2.c2 is not
    // among these: it remains cited on BPLCON0-ADDR above for its address
    // clause (an assertion this block does not touch), and withdrawing it
    // here as well would put the same uid on both a graded line and a
    // record() line in this same package. No predicate, stimulus or
    // expected value above is touched by this block.
    // =========================================================================
    checks.record(
        "sol:C-shres.c8 (p.300, 'BPLCON0 chg W A,D Bitplane control register (misc control bits)'): not asserted "
        "as a whole -- the write-only clause is established here (BPLCON0-WO above); chip is the same "
        "physical-chip-vs-RTL-module problem this file already declines for BPLCON1/BPLCON2/BPLCON3 "
        "(opus:7-appA-bplcon2.c3, sol:7-appA-bplcon2.c3 above), and this id bundles both in one claim. A "
        "multi-clause id is not partially kept on the clause that happens to be established, matching this file's "
        "own bundling precedent (opus:7-appB.c34 above).");
    checks.record(
        "sol:C-genlock-ext.c5 (p.307, 'BPLCON0 W A,D Bitplane control (miscellaneous control bits)'): not "
        "asserted as a whole -- the write-only clause is established here (BPLCON0-WO above); chip is declined "
        "for the same reason (a physical chip column, not an RTL module), matching this file's own bundling "
        "precedent (opus:7-appB.c34 above).");
    checks.record(
        "sol:C-genlock-ext.c7 (p.307, 'BPLCON2 W D Bitplane control (video priority control)'): not asserted as "
        "a whole -- the write-only clause is established here (BPLCON2-WO above); chip is declined for the same "
        "reason as sol:7-appA-bplcon2.c3 / sol:7-appB-bplcon2.c3 above, and function ('video priority control') "
        "is excluded by this file's own Non-goals paragraph (sol:7-appB-bplcon2.c4 above). A multi-clause id is "
        "not partially kept on the clause that happens to be established, matching this file's own bundling "
        "precedent (opus:7-appB.c34 above).");
    checks.record(
        "sol:C-genlock-ext.c8 (p.307, 'BPLCON3 W D Bitplane control (enhanced features)'): not asserted as a "
        "whole -- the write-only clause is established here (BPLCON3-WO above); chip is declined for the same "
        "reason (a physical chip column, not an RTL module), matching this file's own bundling precedent "
        "(opus:7-appB.c34 above).");
    checks.record(
        "opus:7-appA-bplcon2.c4 (p.271, BPLCON3 -- address $106, write-only, Denise (E), function 'Bitplane "
        "control (enhanced features)'): not asserted as a whole -- the write-only clause is established here "
        "(BPLCON3-WO above) and the address clause is established above (BPLCON3-ADDR); chip is the same "
        "physical-chip-vs-RTL-module problem this file already declines for BPLCON1/BPLCON2 "
        "(opus:7-appA-bplcon2.c3, sol:7-appA-bplcon2.c3 above), and function is excluded by this file's own "
        "Non-goals paragraph (sol:7-appB-bplcon2.c4 above). A multi-clause id is not partially kept on the "
        "clauses that happen to be established, matching this file's own bundling precedent (opus:7-appB.c34, "
        "opus:7-appA-bplcon2.c1 above).");
    checks.record(
        "sol:A-color.c2 (p.273, 'COLORxx 180 W D Color table xx'): not asserted as a whole -- the write-only "
        "clause is established here (COLOR-GENERAL-WO above, also opus:A-colorxx.rw's own single-clause reading "
        "of the same row); chip ('implemented by Denise') is declined for the same physical-chip-vs-RTL-module "
        "reason this file already applies to BPLCON0/BPLCON2/BPLCON3 above. A multi-clause id is not partially "
        "kept on the clause that happens to be established, matching this file's own bundling precedent "
        "(opus:7-appB.c34 above).");

    // =========================================================================
    // APPENDIX B REOFFER -- 13 claims reasoned about in this file's own header
    // but never before printed to output, so the coverage ledger read them as
    // untouched. Declined here, in the ledger's own vocabulary, with a
    // self-contained reason. A fourteenth, sol:7-appB-bplcon2.c1, is graded
    // above instead, appended to BPLCON2-ADDR's citation list.
    // No predicate, stimulus or expected value above is touched by this block.
    // =========================================================================
    checks.record(
        "sol:7-appB-bplcon2.c4 (p.291, BPLCON2's function as a priority-control register): not asserted -- this is a "
        "function/semantic claim about what BPLCON2's fields do as a whole, which this file's own Non-goals paragraph "
        "excludes ('BPLCON0/2/3's individual bit *semantics*'); BPLCON2-ADDR above uses one such field (PF2PRI) only "
        "as an address-proving technique, not as an assertion of the register's function, and this claim's own "
        "evidence also depends on sprite-priority behaviour (bplcon2[2:0]) this fixture never drives. "
        "hrm/denise_spritepriority and hrm/ch3-sprite-playfield-priority are the packages that assert what BPLCON2's "
        "priority fields do.");
    checks.record(
        "sol:7-appB-bplcon2.c3 (p.291, BPLCON2's chip attribution -- Appendix B associates BPLCON2 with Denise): not "
        "asserted -- BPLCON2-WO's own text says this fixture observes only the Denise half of BPLCON2's write path and "
        "is not independently verified here beyond that write-only measurement; a single-chip fixture producing an "
        "effect does not, on its own text's own reading, settle which chip's column entry the manual meant, unlike this "
        "file's DENISEID-ADDR-RO or the sibling ch3-register-map-agnus-beamcounter package's structural chip reasoning "
        "-- this package's own author chose not to make that inference for BPLCON2/BPLCON3, and this record keeps that "
        "same choice rather than overriding it on a PASS line.");
    checks.record(
        "opus:7-appB.c34 (p.291): one id bundling BPLCON2's address, write-only direction, chip and function in a "
        "single claim -- the write-only clause and the address clause are both established here (BPLCON2-WO, "
        "BPLCON2-ADDR above); function is excluded by this file's own Non-goals paragraph (see "
        "sol:7-appB-bplcon2.c4 above) and chip is declined in the sol:7-appB-bplcon2.c3 record just above. This "
        "suite's precedent (hrm/ch5-audio-location-pointer, COVERAGE NOTE 2) is that a multi-clause id is not "
        "partially kept on the clauses that happen to be established, so this bundled id is declined as a whole, "
        "not weakened onto BPLCON2-WO's or BPLCON2-ADDR's citation list.");
    checks.record(
        "sol:7-appB-clxdat.c1, sol:7-appB-clxdat.c2, sol:7-appB-clxdat.c3, sol:7-appB-clxdat.c4, opus:7-appB.c19 "
        "(p.289, CLXDAT address/direction/chip/function): not asserted -- CLXDAT appears in this file only inside "
        "CONTROL-FIXTURE-ALIVE, which this file's own CLAIM GROUPS section lists as carrying no claim id by design (it "
        "exists to prove the fixture is not stuck at zero, not to test CLXDAT itself). What would settle these: a "
        "CLXDAT-ADDR-RO assertion mirroring the existing DENISEID-ADDR-RO -- reading $00E returns CLXDAT's own content, "
        "an unrelated write-only address reads $0000 through the same mux, and an attempted write to $00E does not "
        "change the next reading.");
    checks.record(
        "sol:7-appB-clxcon.c1, sol:7-appB-clxcon.c2, sol:7-appB-clxcon.c3, sol:7-appB-clxcon.c4, opus:7-appB.c30 "
        "(p.290, CLXCON address/direction/chip/function): not asserted -- CLXCON has no address constant and no "
        "assertion anywhere in this file. What would settle these: a CLXCON-WO assertion mirroring the existing "
        "BPLCON0-WO/BPLCON2-WO/BPLCON3-WO pattern -- a distinct pattern written to $098 reads back $0000 at that same "
        "address (write-only).");

    // =========================================================================
    // CHAPTER 7 REOFFER, ROUND 2 -- 4 claims offered to this package in an
    // earlier lot, declined then, never recorded then. Examined again here on
    // their own merits and declined again, in the ledger's own vocabulary. A
    // fifth, sol:7-appA-bplcon2.c1, is graded above instead, appended to
    // BPLCON2-ADDR's citation list.
    // No predicate, stimulus or expected value above is touched by this block.
    // =========================================================================
    checks.record(
        "sol:7-appA-bplcon2.c5 (p.271, BPLCON2 controls video priority -- appendix A's per-register table entry, "
        "the same function clause already declined above from appendix B's legend as sol:7-appB-bplcon2.c4): not "
        "asserted, for the identical reason -- this file's own Non-goals paragraph excludes BPLCON2's individual "
        "bit *semantics*, which is what 'controls video priority' states; BPLCON2-ADDR above uses PF2PRI only as "
        "an address-proving technique, and this claim's own evidence also depends on sprite-priority behaviour "
        "(bplcon2[2:0]) this fixture never drives.");
    checks.record(
        "sol:7-appA-bplcon2.c3 (p.271, BPLCON2 is implemented in Denise -- appendix A's chip-attribution reading, "
        "the twin of sol:7-appB-bplcon2.c3 declined above): not asserted, for the identical reason -- BPLCON2-WO's "
        "own text says this fixture observes only the Denise half of BPLCON2's write path and is not independently "
        "verified here beyond that write-only measurement; a single-chip fixture producing an effect does not, on "
        "its own text's own reading, settle which chip's column entry the manual meant.");
    checks.record(
        "opus:7-appA-bplcon2.c3 (p.271, BPLCON1 -- address $102, write-only, Denise, no ECS mark, function "
        "horizontal scroll control): not asserted -- BPLCON1-WO above establishes the write-only clause alone, not "
        "cited there because this id also bundles address, chip and function. Address would need a scroll-shift "
        "measurement at hpos resolution this file does not attempt; function is excluded by this file's own "
        "Non-goals paragraph; chip is the same physical-chip-vs-RTL-module problem this file already declines for "
        "BPLCON2/BPLCON3 (sol:7-appA-bplcon2.c3, sol:7-appB-bplcon2.c3 above). A multi-clause id is not partially "
        "kept on the one clause that happens to be established, matching this file's own bundling precedent "
        "(opus:7-appB.c34 above).");
    checks.record(
        "opus:7-appC-bplcon2.c8 (p.307, appendix C's ECS chip-assignment table for all four bitplane control "
        "registers -- BPLCON0 Agnus+Denise, BPLCON1/BPLCON2/BPLCON3 Denise, all four write-only): not asserted as a "
        "whole -- the write-only clause is established for all four registers (BPLCON0-WO, BPLCON1-WO, BPLCON2-WO, "
        "BPLCON3-WO above); the chip-attribution clause is not independently verified for any of them beyond that "
        "write-only measurement -- BPLCON0-WO's own text already limits itself to 'the Denise-visible half', "
        "BPLCON1's chip clause is declined at opus:7-appA-bplcon2.c3 above, and BPLCON2/BPLCON3's are declined at "
        "sol:7-appA-bplcon2.c3 and sol:7-appB-bplcon2.c3 above. This suite's own precedent "
        "(hrm/ch5-audio-location-pointer, COVERAGE NOTE 2) is that a multi-clause id bundling several registers "
        "and several properties is not partially kept on the clauses that happen to be established, so this bundled "
        "id is declined as a whole, matching this file's own opus:7-appB.c34 record above.");

    // =========================================================================
    // BPLCON2-WO BUNDLE CORRECTION (pre-existing) -- two bundled ids sat on
    // BPLCON2-WO's own citation list carrying only the write-only clause
    // each one establishes there, contradicting this file's own precedent
    // (opus:7-appB.c34 above) that a bundled id is not partially kept on
    // the citation list of the one clause that happens to be established.
    // Removed from BPLCON2-WO's citation list above and declined here
    // instead. No predicate, stimulus or expected value above is touched
    // by this block.
    // =========================================================================
    checks.record(
        "opus:7-appA-bplcon2.c1 (p.271, BPLCON2 -- address $104, write-only, Denise (E), function 'Bitplane "
        "control register (video priority control)'): not asserted as a whole -- the write-only clause is "
        "established here (BPLCON2-WO above) and the address clause is established above (BPLCON2-ADDR); chip is "
        "the same physical-chip-vs-RTL-module problem this file already declines for BPLCON2 "
        "(sol:7-appA-bplcon2.c3 above), and function is excluded by this file's own Non-goals paragraph "
        "(sol:7-appB-bplcon2.c4 above). A multi-clause id is not partially kept on the clauses that happen to be "
        "established, matching this file's own bundling precedent (opus:7-appB.c34, opus:7-appA-bplcon2.c3 "
        "above).");
    checks.record(
        "opus:7-appC-bplcon2.c1, sol:7-appC-bplcon2.c1 (p.307, BPLCON2 -- write-only, Denise, function 'Bitplane "
        "control (video priority control)'): not asserted as a whole -- the write-only clause is established here "
        "(BPLCON2-WO above); chip is declined for the same reason as sol:7-appA-bplcon2.c3 / sol:7-appB-bplcon2.c3 "
        "above, and function is excluded by this file's own Non-goals paragraph (sol:7-appB-bplcon2.c4 above). A "
        "multi-clause id is not partially kept on the clause that happens to be established, matching this file's "
        "own bundling precedent (opus:7-appB.c34, opus:7-appC-bplcon2.c8 above).");

    // =========================================================================
    // APPENDIX C P.298 -- two of the three claims from Appendix C's p.298
    // ECS "changed register" map for BPLCON2 (see this file's own header,
    // APPENDIX C P.298). The third, sol:7-appC-map-bplcon2.c3, is graded
    // above, folded into BPLCON2-WO's own citation list. No predicate,
    // stimulus or expected value above is touched by this block.
    // =========================================================================
    checks.record(
        "sol:7-appC-map-bplcon2.c4 (p.298, ECS BPLCON2 is implemented in Denise): not asserted -- p.298's own legend "
        "reads 'A=Agnus chip, D=Denise chip, P=Paula chip': the CHIP column names a physical chip, not an RTL "
        "module, and this suite's symmetric chip-column rule (hrm/ch8-appb-chip-column-paula) declines claims like "
        "this one, the same reason this package already declines the same register's Appendix A/B chip claims "
        "(sol:7-appA-bplcon2.c3, sol:7-appB-bplcon2.c3 above).");
    checks.record(
        "opus:7-appC298.c4 (p.298): one id bundling BPLCON2's address, its 'chg' revision mark, write-only "
        "direction, chip and function for the p.298 row (sol split the same source row into the five claims "
        "7-appC-map-bplcon2.c1-c5, of which only .c3 and .c4 were routed to this package). Declined as a whole, "
        "matching this file's own bundling precedent for the identical p.291 bundle (opus:7-appB.c34 above): its "
        "chip clause names a physical chip for the same reason sol:7-appC-map-bplcon2.c4 is declined just above, "
        "and this suite's precedent (hrm/ch5-audio-location-pointer, COVERAGE NOTE 2) is that a multi-clause id "
        "is not partially kept on the clauses that happen to be established. Of sol's five-way split, only c3 "
        "(direction) is graded in this package, on BPLCON2-WO above; c4 (chip) is declined just above. c1 "
        "(address), c2 ('chg') and c5 (function) were routed to hrm/ch7-bplcon2-live-bits-and-ecs, not to this "
        "package -- c1 on BPLCON2-ADDRESS-0x104-LIVE, c2 on BPLCON2-BIT9-CHG-NOT-COMPARED, c5 co-cited across "
        "that package's BIT14/13/12/11/10-ECS-RECORDED lines.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
