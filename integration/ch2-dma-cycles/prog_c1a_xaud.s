; prog_c1a_xaud.s -- run X-aud (SPECIFICATION.md P3.6): C1a with list L5,
; AUD0LCH/L at the start of the Copper block, AUD0LEN equal to the number
; of words the C++ test declares for that block in this one run (2048,
; COP_MAIN's own reserve -- ch2dma.i).

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"

        org     PROGRAM_ORG
        PROG_HEADER

start:
        S0_QUIESCE
        move.w  #(COP_MAIN)>>16,AUD0LCH(a0)
        move.w  #(COP_MAIN)&$FFFF,AUD0LCL(a0)
        move.w  #$0800,AUD0LEN(a0)
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
