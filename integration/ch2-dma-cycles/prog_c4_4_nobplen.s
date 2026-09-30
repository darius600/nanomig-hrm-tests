; prog_c4_4_nobplen.s -- K5's own contrast (SPECIFICATION.md P5.6): C4-4
; without BPLEN. No plane fetches occur.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        BUILD_L4 COP_MAIN,c4_bpl_tab,4
        move.w  #$4200,BPLCON0(a0)
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
        S3_COPPER COP_MAIN
        S4_DMAEN  0
        S5_ARM_INT
        S6_WAIT
        S7_FINISH
        VBLANK_HANDLER

c4_bpl_tab:     dc.l    CH_BPL1,CH_BPL2,CH_BPL3,CH_BPL4

        include "ch2build_code.i"
