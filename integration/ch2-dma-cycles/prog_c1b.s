; prog_c1b.s -- configuration C1b (SPECIFICATION.md 2.9): as C1a with
; AUDxPER <- 447 (the example value of p.143).

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        BUILD_END COP_MAIN
        move.w  #(CH_AUD0)>>16,AUD0LCH(a0)
        move.w  #(CH_AUD0)&$FFFF,AUD0LCL(a0)
        move.w  #16,AUD0LEN(a0)
        move.w  #64,AUD0VOL(a0)
        move.w  #447,AUD0PER(a0)
        move.w  #(CH_AUD1)>>16,AUD1LCH(a0)
        move.w  #(CH_AUD1)&$FFFF,AUD1LCL(a0)
        move.w  #16,AUD1LEN(a0)
        move.w  #64,AUD1VOL(a0)
        move.w  #447,AUD1PER(a0)
        move.w  #(CH_AUD2)>>16,AUD2LCH(a0)
        move.w  #(CH_AUD2)&$FFFF,AUD2LCL(a0)
        move.w  #16,AUD2LEN(a0)
        move.w  #64,AUD2VOL(a0)
        move.w  #447,AUD2PER(a0)
        move.w  #(CH_AUD3)>>16,AUD3LCH(a0)
        move.w  #(CH_AUD3)&$FFFF,AUD3LCL(a0)
        move.w  #16,AUD3LEN(a0)
        move.w  #64,AUD3VOL(a0)
        move.w  #447,AUD3PER(a0)
        S3_COPPER COP_MAIN
        S4_DMAEN  AUD0123EN
        S5_ARM_INT
        S6_WAIT
        S7_FINISH
        VBLANK_HANDLER

        include "ch2build_code.i"
