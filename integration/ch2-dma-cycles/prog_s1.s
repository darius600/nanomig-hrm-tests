; prog_s1.s -- configuration S1 (SPECIFICATION.md 2.9): SPREN|BPLEN, list
; L_S1 (structure Z, four uses, sprite 0 only; sprites 1-7 -> null; one
; zeroed low-res bitplane so the background is the only visible thing not
; a sprite).

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
        move.w  #(CH_SPR0)>>16,SPR0PTH(a0)
        move.w  #(CH_SPR0)&$FFFF,SPR0PTL(a0)
        move.w  #(CH_SPR_NULL)>>16,SPR1PTH(a0)
        move.w  #(CH_SPR_NULL)&$FFFF,SPR1PTL(a0)
        move.w  #(CH_SPR_NULL)>>16,SPR2PTH(a0)
        move.w  #(CH_SPR_NULL)&$FFFF,SPR2PTL(a0)
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
