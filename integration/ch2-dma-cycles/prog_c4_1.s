; prog_c4_1.s -- configuration C4-1 (SPECIFICATION.md 2.9): one low-res
; bitplane. List L4(1) (V_bpl(1); END, poked by the C++ test); DMACON bit
; BPLEN.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"

        org     PROGRAM_ORG
        PROG_HEADER

start:
        S0_QUIESCE
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
