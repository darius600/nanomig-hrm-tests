; prog_c1a.s -- configuration C1a (SPECIFICATION.md 2.9): four audio
; channels, period 124 (p.140). List L0 (END only, poked by the C++ test);
; DMACON bits AUD0EN-AUD3EN.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"

        org     PROGRAM_ORG
        PROG_HEADER

start:
        S0_QUIESCE
        ; S2 -- p.137 steps 3-6, per channel, period 124 (p.140)
        move.w  #(CH_AUD0)>>16,AUD0LCH(a0)
        move.w  #(CH_AUD0)&$FFFF,AUD0LCL(a0)
        move.w  #16,AUD0LEN(a0)
        move.w  #64,AUD0VOL(a0)
        move.w  #124,AUD0PER(a0)
        move.w  #(CH_AUD1)>>16,AUD1LCH(a0)
        move.w  #(CH_AUD1)&$FFFF,AUD1LCL(a0)
        move.w  #16,AUD1LEN(a0)
        move.w  #64,AUD1VOL(a0)
        move.w  #124,AUD1PER(a0)
        move.w  #(CH_AUD2)>>16,AUD2LCH(a0)
        move.w  #(CH_AUD2)&$FFFF,AUD2LCL(a0)
        move.w  #16,AUD2LEN(a0)
        move.w  #64,AUD2VOL(a0)
        move.w  #124,AUD2PER(a0)
        move.w  #(CH_AUD3)>>16,AUD3LCH(a0)
        move.w  #(CH_AUD3)&$FFFF,AUD3LCL(a0)
        move.w  #16,AUD3LEN(a0)
        move.w  #64,AUD3VOL(a0)
        move.w  #124,AUD3PER(a0)
        S3_COPPER COP_MAIN
        S4_DMAEN  AUD0123EN
        S5_ARM_INT
        S6_WAIT
        S7_FINISH
