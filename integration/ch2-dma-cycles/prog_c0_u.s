; prog_c0_u.s -- run C0-u (SPECIFICATION.md P1.6, list L0u, SPECIFICATION.md
; 2.8): the Copper jumps to sub-list U (outside every declared block) right
; after line $80's refresh group (WAIT VP=$80,HP=$04), fetching U's words
; on odd cycles. Those extra accesses join the refresh group without moving
; its first access, so F2/F3 still hold, and the program still finishes.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        ; L0u at COP_MAIN (2.8): MOVE COP2LCH/L <- CH_U_SUBLIST; WAIT
        ; VP=$80,HP=$04 (word $8005); COPJMP2 strobe.
        move.l  #COP_MAIN,a1
        move.w  #COP2LCH,d1
        move.w  #COP2LCL,d2
        move.l  #CH_U_SUBLIST,d3
        bsr     wr_movel_copper
        move.w  #$8005,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #COPJMP2,(a1)+
        move.w  #$0000,(a1)+
        ; U sub-list at CH_U_SUBLIST (2.8): MOVE COLOR00 <- 0; END.
        move.l  #CH_U_SUBLIST,a1
        move.w  #COLOR00,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$FFFF,(a1)+
        move.w  #$FFFE,(a1)+
        S3_COPPER COP_MAIN
        S4_DMAEN  0
        S5_ARM_INT
        S6_WAIT
        S7_FINISH
        VBLANK_HANDLER

        include "ch2build_code.i"
