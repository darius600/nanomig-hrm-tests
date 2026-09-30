; prog_c3.s -- configuration C3 (SPECIFICATION.md 2.9): eight sprite DMA
; channels, two uses each. List L3 (V_spr; END) plus all eight sprite
; structures, built by S1; DMACON bit SPREN.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        BUILD_L3 COP_MAIN,c3_spr_tab
        bsr     build_structs_c3
        S3_COPPER COP_MAIN
        S4_DMAEN  SPREN
        S5_ARM_INT
        S6_WAIT
        S7_FINISH
        VBLANK_HANDLER

c3_spr_tab:     dc.l    CH_SPR0,CH_SPR0+$0400,CH_SPR0+$0800,CH_SPR0+$0C00
                dc.l    CH_SPR0+$1000,CH_SPR0+$1400,CH_SPR0+$1800,CH_SPR0+$1C00

        include "ch2build_code.i"
