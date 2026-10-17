; prog_s2.s -- configuration S2 (SPECIFICATION.md 2.9): as S1; structures
; A and B in sprite block 0 (sprite 0's pointer alternates between them,
; rewritten by the Copper's own V_spr each field -- this program's own S2
; only sets the initial pointer, to structure A); R (the fixed reference)
; in sprite block 2. List L_A (S3 installs L_A; L_A/L_B alternate from
; frame to frame by each rewriting COP1LC to point at the other).

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        ; L_A at COP_MAIN (2.8): V_spr[A,null,R,null,null,null,null,null];
        ; V_bpl(1)[CH_BPL1]; COLOR17-19,21-23; MOVE COP1LC <- COP_SECOND
        ; (L_B); END.
        move.l  #COP_MAIN,a1
        lea     s2_spr_tab_a,a5
        bsr     build_Vspr8
        lea     s2_bpl_tab,a6
        moveq   #1,d4
        bsr     build_Vbpln
        move.w  #COLOR17,(a1)+
        ifd     S2_COL0
        move.w  #$0000,(a1)+
        else
        move.w  #$0F00,(a1)+
        endc
        move.w  #COLOR18,(a1)+
        move.w  #$00F0,(a1)+
        move.w  #COLOR19,(a1)+
        move.w  #$000F,(a1)+
        move.w  #COLOR21,(a1)+
        move.w  #$0FF0,(a1)+
        move.w  #COLOR22,(a1)+
        move.w  #$00FF,(a1)+
        move.w  #COLOR23,(a1)+
        move.w  #$0F0F,(a1)+
        ifd     S2_ENDCOL
        ; END placed after the colour MOVEs and before the COP1LC pair (P8's
        ; contrast for Q5): the pair is written but never fetched.
        move.w  #$FFFF,(a1)+
        move.w  #$FFFE,(a1)+
        endc
        move.w  #COP1LCH,d1
        move.w  #COP1LCL,d2
        move.l  #COP_SECOND,d3
        bsr     wr_movel_copper
        move.w  #$FFFF,(a1)+
        move.w  #$FFFE,(a1)+

        ; L_B at COP_SECOND: as L_A, but sprite 0 -> B, and MOVE COP1LC <-
        ; COP_MAIN (L_A).
        move.l  #COP_SECOND,a1
        lea     s2_spr_tab_b,a5
        bsr     build_Vspr8
        lea     s2_bpl_tab,a6
        moveq   #1,d4
        bsr     build_Vbpln
        move.w  #COLOR17,(a1)+
        ifd     S2_COL0
        move.w  #$0000,(a1)+
        else
        move.w  #$0F00,(a1)+
        endc
        move.w  #COLOR18,(a1)+
        move.w  #$00F0,(a1)+
        move.w  #COLOR19,(a1)+
        move.w  #$000F,(a1)+
        move.w  #COLOR21,(a1)+
        move.w  #$0FF0,(a1)+
        move.w  #COLOR22,(a1)+
        move.w  #$00FF,(a1)+
        move.w  #COLOR23,(a1)+
        move.w  #$0F0F,(a1)+
        move.w  #COP1LCH,d1
        move.w  #COP1LCL,d2
        move.l  #COP_MAIN,d3
        bsr     wr_movel_copper
        move.w  #$FFFF,(a1)+
        move.w  #$FFFE,(a1)+

        ; Structures A, B, R (2.9) and the null block.
        CPY_TABLE CH_SPR0,s2_data_a,s2_data_a_end
        CPY_TABLE CH_SPR0+$40,s2_data_b,s2_data_b_end
        CPY_TABLE CH_SPR0+$0800,s2_data_r,s2_data_r_end
        ifd     S2_REFMOVE
        CPY_TABLE CH_SPR0+$0880,s2_data_r2,s2_data_r2_end
        endc
        move.l  #CH_SPR_NULL,a1
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+

        move.w  #0,COLOR00(a0)
        move.w  #$1200,BPLCON0(a0)
        move.w  #0,BPL1MOD(a0)
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$2CC1,DIWSTOP(a0)
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        move.w  #(CH_BPL1)>>16,BPL1PTH(a0)
        move.w  #(CH_BPL1)&$FFFF,BPL1PTL(a0)
        move.w  #(CH_SPR0)>>16,SPR0PTH(a0)         ; structure A
        move.w  #(CH_SPR0)&$FFFF,SPR0PTL(a0)
        move.w  #(CH_SPR_NULL)>>16,SPR1PTH(a0)
        move.w  #(CH_SPR_NULL)&$FFFF,SPR1PTL(a0)
        move.w  #(CH_SPR0+$0800)>>16,SPR2PTH(a0)   ; structure R
        move.w  #(CH_SPR0+$0800)&$FFFF,SPR2PTL(a0)
        move.w  #(CH_SPR_NULL)>>16,SPR3PTH(a0)
        move.w  #(CH_SPR_NULL)&$FFFF,SPR3PTL(a0)
        move.w  #(CH_SPR_NULL)>>16,SPR4PTH(a0)
        move.w  #(CH_SPR_NULL)&$FFFF,SPR4PTL(a0)
        move.w  #(CH_SPR_NULL)>>16,SPR5PTH(a0)
        move.w  #(CH_SPR_NULL)&$FFFF,SPR5PTL(a0)
        move.w  #(CH_SPR_NULL)>>16,SPR6PTH(a0)
        move.w  #(CH_SPR_NULL)&$FFFF,SPR6PTL(a0)
        move.w  #(CH_SPR_NULL)>>16,SPR7PTH(a0)
        move.w  #(CH_SPR_NULL)&$FFFF,SPR7PTL(a0)
        S3_COPPER COP_MAIN
        S4_DMAEN  SPREN|BPLEN
        S5_ARM_INT
        ifd     S2_EXIT6
        ; FRAMES' contrast: the wait loop exits when FRAME_CTR reaches 6.
        lea     CUSTOM,a0
.wait6  stop    #$2000
        cmp.w   #6,FRAME_CTR
        bne.s   .wait6
        else
        S6_WAIT
        endc
        S7_FINISH
        VBLANK_HANDLER

s2_spr_tab_a:
                ifd     S2_SWAP
                dc.l    CH_SPR0+$40
                else
                dc.l    CH_SPR0
                endc
                dc.l    CH_SPR_NULL,CH_SPR0+$0800,CH_SPR_NULL
                dc.l    CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL
s2_spr_tab_b:
                ifd     S2_STILL
                dc.l    CH_SPR0
                else
                ifd     S2_SWAP
                dc.l    CH_SPR0
                else
                dc.l    CH_SPR0+$40
                endc
                endc
                dc.l    CH_SPR_NULL
                ifd     S2_REFMOVE
                dc.l    CH_SPR0+$0880
                else
                dc.l    CH_SPR0+$0800
                endc
                dc.l    CH_SPR_NULL
                dc.l    CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL
s2_bpl_tab:     dc.l    CH_BPL1

s2_data_a:      dc.w    $6050,$6A00
                dc.w    $FFFF,$0000
                rept    9
                dc.w    $0F0F,$00FF
                endr
                dc.w    $0000,$0000
s2_data_a_end:

s2_data_b:      dc.w    $6A5C,$7400
                dc.w    $FFFF,$0000
                rept    9
                dc.w    $0F0F,$00FF
                endr
                dc.w    $0000,$0000
s2_data_b_end:

s2_data_r:      dc.w    $4048,$4600
                rept    6
                dc.w    $FFFF,$0000
                endr
                dc.w    $0000,$0000
s2_data_r_end:

        ifd     S2_REFMOVE
; a copy of R whose control word 1 has HSTART $98 instead of $90
s2_data_r2:     dc.w    $404C,$4600
                rept    6
                dc.w    $FFFF,$0000
                endr
                dc.w    $0000,$0000
s2_data_r2_end:
        endc

        include "ch2build_code.i"
