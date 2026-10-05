; prog_c1a_xaud.s -- run X-aud (SPECIFICATION.md P3.6): C1a with list L5 and
; AUD0LCH/L at COP_MAIN-$100, AUD0LEN $100 words: audio channel 0 reads the
; $200 bytes from $0F00 to $1100, which straddle the start of the Copper
; block. The first half holds words only the audio channel reads; the second
; half holds the first words of L5, which the Copper reads too. L5's own
; sprite pointers are all the null block (SPREN is never set for this run)
; and its bitplane pointers are all zero (BPLEN is never set either): only
; the Copper's own fetches of L5/L5b, and the audio channel's overlapping
; reads, matter here.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        BUILD_L5  COP_MAIN,xaud_spr_tab,xaud_bpl_tab,COP_SECOND,-1,0,-1
        BUILD_L5B COP_SECOND
        move.w  #(COP_MAIN-$100)>>16,AUD0LCH(a0)
        move.w  #(COP_MAIN-$100)&$FFFF,AUD0LCL(a0)
        move.w  #$0100,AUD0LEN(a0)
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
        VBLANK_HANDLER

xaud_spr_tab:   dc.l    CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL
                dc.l    CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL
xaud_bpl_tab:   dc.l    0,0,0,0,0,0

        include "ch2build_code.i"
