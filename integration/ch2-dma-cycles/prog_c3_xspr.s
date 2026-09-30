; prog_c3_xspr.s -- run X-spr (SPECIFICATION.md P4.6): C3 with list L3x and
; structure X for sprite 0. From line $A0 the Copper executes X+4 onward as
; code (seven MOVE COLOR00<-0, then END); sprite 0's own DMA reads the same
; words as data in lines $50-$57.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        ; L3x at COP_MAIN (2.8): V_spr[X,null x7]; MOVE COP2LCH/L <- X+4;
        ; WAIT VP=$A0,HP=0 (word $A001); COPJMP2 strobe.
        move.l  #COP_MAIN,a1
        lea     xspr_spr_tab,a5
        bsr     build_Vspr8
        move.w  #COP2LCH,d1
        move.w  #COP2LCL,d2
        move.l  #CH_SPR0+4,d3
        bsr     wr_movel_copper
        move.w  #$A001,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #COPJMP2,(a1)+
        move.w  #$0000,(a1)+
        ; structure X (2.9) at CH_SPR0.
        CPY_TABLE CH_SPR0,structx_tbl,structx_tbl_end
        ; null block at CH_SPR_NULL.
        move.l  #CH_SPR_NULL,a1
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        S3_COPPER COP_MAIN
        S4_DMAEN  SPREN
        S5_ARM_INT
        S6_WAIT
        S7_FINISH
        VBLANK_HANDLER

xspr_spr_tab:   dc.l    CH_SPR0,CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL
                dc.l    CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL
structx_tbl:    dc.w    $5050,$5800
                dc.w    $0180,$0000
                dc.w    $0180,$0000
                dc.w    $0180,$0000
                dc.w    $0180,$0000
                dc.w    $0180,$0000
                dc.w    $0180,$0000
                dc.w    $0180,$0000
                dc.w    $FFFF,$FFFE
                dc.w    $0000,$0000
structx_tbl_end:

        include "ch2build_code.i"
