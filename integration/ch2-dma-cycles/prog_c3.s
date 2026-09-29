; prog_c3.s -- configuration C3 (SPECIFICATION.md 2.9): eight sprite DMA
; channels, two uses each. List L3 (V_spr; END, poked by the C++ test);
; DMACON bit SPREN.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"

        org     PROGRAM_ORG
        PROG_HEADER

start:
        S0_QUIESCE
        move.w  #(CH_SPR0)>>16,SPR0PTH(a0)
        move.w  #(CH_SPR0)&$FFFF,SPR0PTL(a0)
        move.w  #(CH_SPR0+$0400)>>16,SPR1PTH(a0)
        move.w  #(CH_SPR0+$0400)&$FFFF,SPR1PTL(a0)
        move.w  #(CH_SPR0+$0800)>>16,SPR2PTH(a0)
        move.w  #(CH_SPR0+$0800)&$FFFF,SPR2PTL(a0)
        move.w  #(CH_SPR0+$0C00)>>16,SPR3PTH(a0)
        move.w  #(CH_SPR0+$0C00)&$FFFF,SPR3PTL(a0)
        move.w  #(CH_SPR0+$1000)>>16,SPR4PTH(a0)
        move.w  #(CH_SPR0+$1000)&$FFFF,SPR4PTL(a0)
        move.w  #(CH_SPR0+$1400)>>16,SPR5PTH(a0)
        move.w  #(CH_SPR0+$1400)&$FFFF,SPR5PTL(a0)
        move.w  #(CH_SPR0+$1800)>>16,SPR6PTH(a0)
        move.w  #(CH_SPR0+$1800)&$FFFF,SPR6PTL(a0)
        move.w  #(CH_SPR0+$1C00)>>16,SPR7PTH(a0)
        move.w  #(CH_SPR0+$1C00)&$FFFF,SPR7PTL(a0)
        S3_COPPER COP_MAIN
        S4_DMAEN  SPREN
        S5_ARM_INT
        S6_WAIT
        S7_FINISH
