; prog_c7b.s -- configuration C7b (SPECIFICATION.md 2.9): list L5, DMACON
; bit BPLEN only, four high-resolution bitplanes -- "all of the available
; memory time slots" (p.197), locking out the 68000/blitter/Copper. L5's
; own sprite pointers are all the null block (SPREN is never set).

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        BUILD_L5  COP_MAIN,c7b_spr_tab,c7b_bpl_tab,COP_SECOND,-1,0,-1
        BUILD_L5B COP_SECOND
        move.w  #$C200,BPLCON0(a0)     ; HIRES, 4 planes (p.63)
        move.w  #0,BPLCON1(a0)
        move.w  #0,BPL1MOD(a0)
        move.w  #0,BPL2MOD(a0)
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$2CC1,DIWSTOP(a0)
        move.w  #$003C,DDFSTRT(a0)     ; "normal high resolution" (p.60)
        move.w  #$00D4,DDFSTOP(a0)
        move.w  #(CH_BPL1)>>16,BPL1PTH(a0)
        move.w  #(CH_BPL1)&$FFFF,BPL1PTL(a0)
        move.w  #(CH_BPL2)>>16,BPL2PTH(a0)
        move.w  #(CH_BPL2)&$FFFF,BPL2PTL(a0)
        move.w  #(CH_BPL3)>>16,BPL3PTH(a0)
        move.w  #(CH_BPL3)&$FFFF,BPL3PTL(a0)
        move.w  #(CH_BPL4)>>16,BPL4PTH(a0)
        move.w  #(CH_BPL4)&$FFFF,BPL4PTL(a0)
        S3_COPPER COP_MAIN
        S4_DMAEN  BPLEN
        S5_ARM_INT
        S6_WAIT
        S7_FINISH
        VBLANK_HANDLER

c7b_spr_tab:    dc.l    CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL
                dc.l    CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL
c7b_bpl_tab:    dc.l    CH_BPL1,CH_BPL2,CH_BPL3,CH_BPL4,0,0

        include "ch2build_code.i"
