; prog_c0_m.s -- contrast for F3 (SPECIFICATION.md 2.3, list L0m): as
; prog_c0_uj.s, but the WAIT's HP is $70 instead of $04. The Copper's reads
; of U (outside every declared block) fall near horizontal position $70 of
; line $80, far from any refresh group, and form a group of their own at
; every second memory cycle. That splits line $80 into two pieces whose
; lengths are neither 227 nor 228 memory cycles: F3 fails. U jumps back into
; the Copper block, so the Copper's last read of the frame lies in the
; Copper block and not in the refresh set, and the four refresh slots of
; every group keep their offsets: F2 holds.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        ; L0m at COP_MAIN: MOVE COP2LCH/L <- U; WAIT VP=$80,HP=$70; MOVE
        ; COPJMP2; END (at COP_MAIN+$10).
        move.l  #COP_MAIN,a1
        move.w  #COP2LCH,d1
        move.w  #COP2LCL,d2
        move.l  #CH_U_SUBLIST,d3
        bsr     wr_movel_copper
        move.w  #$8071,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #COPJMP2,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$FFFF,(a1)+
        move.w  #$FFFE,(a1)+
        ; U at CH_U_SUBLIST: MOVE COLOR00 <- 0; MOVE COP2LCH/L <- the END
        ; word of L0m; MOVE COPJMP2.
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
