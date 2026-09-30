; prog_c7a.s -- configuration C7a (SPECIFICATION.md 2.9): list L5, DMACON
; bit BPLEN only, registers of C4-6 (six low-res bitplanes -- odd-cycle
; contention from planes 5-6, P6's own use). L5's own sprite pointers are
; all the null block (SPREN is never set for this run).

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        BUILD_L5  COP_MAIN,c7a_spr_tab,c7a_bpl_tab,COP_SECOND,-1,0,-1
        BUILD_L5B COP_SECOND
        move.w  #$6200,BPLCON0(a0)     ; (6<<12)|$0200
        move.w  #0,BPLCON1(a0)
        move.w  #0,BPL1MOD(a0)
        move.w  #0,BPL2MOD(a0)
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$2CC1,DIWSTOP(a0)
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        move.w  #(CH_BPL1)>>16,BPL1PTH(a0)
        move.w  #(CH_BPL1)&$FFFF,BPL1PTL(a0)
        move.w  #(CH_BPL2)>>16,BPL2PTH(a0)
        move.w  #(CH_BPL2)&$FFFF,BPL2PTL(a0)
        move.w  #(CH_BPL3)>>16,BPL3PTH(a0)
        move.w  #(CH_BPL3)&$FFFF,BPL3PTL(a0)
        move.w  #(CH_BPL4)>>16,BPL4PTH(a0)
        move.w  #(CH_BPL4)&$FFFF,BPL4PTL(a0)
        move.w  #(CH_BPL5)>>16,BPL5PTH(a0)
        move.w  #(CH_BPL5)&$FFFF,BPL5PTL(a0)
        move.w  #(CH_BPL6)>>16,BPL6PTH(a0)
        move.w  #(CH_BPL6)&$FFFF,BPL6PTL(a0)
        S3_COPPER COP_MAIN
        S4_DMAEN  BPLEN
        S5_ARM_INT
        S6_WAIT
        S7_FINISH
        VBLANK_HANDLER

c7a_spr_tab:    dc.l    CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL
                dc.l    CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL
c7a_bpl_tab:    dc.l    CH_BPL1,CH_BPL2,CH_BPL3,CH_BPL4,CH_BPL5,CH_BPL6

        include "ch2build_code.i"
