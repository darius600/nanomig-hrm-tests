; prog_c0_uj.s -- contrast for P1 (SPECIFICATION.md P1.6, list L0uj): the
; Copper jumps to sub-list U (outside every declared block) right after line
; $80's refresh group (WAIT VP=$80,HP=$04) and reads U's words on odd cycles.
; U writes COP2LC back to an END word inside the Copper block and strobes
; COPJMP2, so the Copper's last read of the frame (the word after the END)
; lies in the Copper block and not in the refresh set. The refresh group of
; every line keeps its four slots at 0, 2, 4 and 6 memory cycles (F2) and
; every group start keeps its line interval (F3); only line $80's group gains
; accesses on odd cycles after its fourth slot.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        ; L0uj at COP_MAIN: MOVE COP2LCH/L <- U; WAIT VP=$80,HP=$04; MOVE
        ; COPJMP2; END (at COP_MAIN+$10).
        move.l  #COP_MAIN,a1
        move.w  #COP2LCH,d1
        move.w  #COP2LCL,d2
        move.l  #CH_U_SUBLIST,d3
        bsr     wr_movel_copper
        move.w  #$8005,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #COPJMP2,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$FFFF,(a1)+
        move.w  #$FFFE,(a1)+
        ; U at CH_U_SUBLIST: MOVE COLOR00 <- 0; MOVE COP2LCH/L <- the END
        ; word of L0uj; MOVE COPJMP2.
        move.l  #CH_U_SUBLIST,a1
        move.w  #COLOR00,(a1)+
        move.w  #$0000,(a1)+
        move.w  #COP2LCH,d1
        move.w  #COP2LCL,d2
        move.l  #COP_MAIN+$10,d3
        bsr     wr_movel_copper
        move.w  #COPJMP2,(a1)+
        move.w  #$0000,(a1)+
        S3_COPPER COP_MAIN
        S4_DMAEN  0
        S5_ARM_INT
        S6_WAIT
        S7_FINISH
        VBLANK_HANDLER

        include "ch2build_code.i"
