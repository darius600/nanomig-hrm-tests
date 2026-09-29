; prog_s2.s -- configuration S2 (SPECIFICATION.md 2.9): as S1; structures
; A and B in sprite block 0 (sprite 0's pointer alternates between them,
; rewritten by the Copper's own V_spr each field -- this program's own S2
; only sets the initial pointer, to structure A); R (the fixed reference)
; in sprite block 2. List L_A (S3 installs L_A; L_A/L_B alternate from
; frame to frame by each rewriting COP1LC to point at the other).

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"

        org     PROGRAM_ORG
        PROG_HEADER

start:
        S0_QUIESCE
        move.w  #0,COLOR00(a0)
        move.w  #$1200,BPLCON0(a0)
        move.w  #0,BPL1MOD(a0)
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$2CC1,DIWSTOP(a0)
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        move.w  #(CH_BPL1)>>16,BPL1PTH(a0)
        move.w  #(CH_BPL1)&$FFFF,BPL1PTL(a0)
        move.w  #(CH_SPR0)>>16,SPR0PTH(a0)         ; structure A
        move.w  #(CH_SPR0)&$FFFF,SPR0PTL(a0)
        move.w  #(CH_SPR_NULL)>>16,SPR1PTH(a0)
        move.w  #(CH_SPR_NULL)&$FFFF,SPR1PTL(a0)
        move.w  #(CH_SPR0+$0800)>>16,SPR2PTH(a0)   ; structure R
        move.w  #(CH_SPR0+$0800)&$FFFF,SPR2PTL(a0)
        move.w  #(CH_SPR_NULL)>>16,SPR3PTH(a0)
        move.w  #(CH_SPR_NULL)&$FFFF,SPR3PTL(a0)
        move.w  #(CH_SPR_NULL)>>16,SPR4PTH(a0)
        move.w  #(CH_SPR_NULL)&$FFFF,SPR4PTL(a0)
        move.w  #(CH_SPR_NULL)>>16,SPR5PTH(a0)
        move.w  #(CH_SPR_NULL)&$FFFF,SPR5PTL(a0)
        move.w  #(CH_SPR_NULL)>>16,SPR6PTH(a0)
        move.w  #(CH_SPR_NULL)&$FFFF,SPR6PTL(a0)
        move.w  #(CH_SPR_NULL)>>16,SPR7PTH(a0)
        move.w  #(CH_SPR_NULL)&$FFFF,SPR7PTL(a0)
        S3_COPPER COP_MAIN
        S4_DMAEN  SPREN|BPLEN
        S5_ARM_INT
        S6_WAIT
        S7_FINISH
