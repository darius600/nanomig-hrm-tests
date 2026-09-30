; prog_c4_1.s -- configuration C4-1 (SPECIFICATION.md 2.9): one low-res
; bitplane. List L4(1) (V_bpl(1); END, built by S1); DMACON bit BPLEN.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        BUILD_L4 COP_MAIN,c4_bpl_tab,1
        move.w  #$1200,BPLCON0(a0)     ; (1<<12)|$0200
        move.w  #0,BPLCON1(a0)
        move.w  #0,BPL1MOD(a0)
        move.w  #0,BPL2MOD(a0)
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$2CC1,DIWSTOP(a0)     ; PAL (p.59)
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        move.w  #(CH_BPL1)>>16,BPL1PTH(a0)
        move.w  #(CH_BPL1)&$FFFF,BPL1PTL(a0)
        S3_COPPER COP_MAIN
        S4_DMAEN  BPLEN
        S5_ARM_INT
        S6_WAIT
        S7_FINISH
        VBLANK_HANDLER

c4_bpl_tab:     dc.l    CH_BPL1

        include "ch2build_code.i"
