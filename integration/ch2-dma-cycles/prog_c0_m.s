; prog_c0_m.s -- run C0-m (SPECIFICATION.md 2.3 D3, F2/F3's own contrast,
; list L0m): as L0u but the WAIT's HP is $70 instead of $04, so the
; Copper's fetch of U's words lands near horizontal position $70 of line
; $80 instead of right after its refresh group -- far enough from any
; refresh group to form a spurious group of its own, splitting line $80
; into two pieces whose lengths are neither 227 nor 228: F3 must fail.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        move.l  #COP_MAIN,a1
        move.w  #COP2LCH,d1
        move.w  #COP2LCL,d2
        move.l  #CH_U_SUBLIST,d3
        bsr     wr_movel_copper
        move.w  #$8071,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #COPJMP2,(a1)+
        move.w  #$0000,(a1)+
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
