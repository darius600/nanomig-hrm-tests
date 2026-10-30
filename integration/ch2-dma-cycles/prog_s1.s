; prog_s1.s -- configuration S1 (SPECIFICATION.md 2.9): SPREN|BPLEN, list
; L_S1 (structure Z, four uses, sprite 0 only; sprites 1-7 -> null; one
; zeroed low-res bitplane so the background is the only visible thing not
; a sprite).

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        ; L_S1 at COP_MAIN (2.8): V_spr[Z,null x7]; V_bpl(1)[CH_BPL1];
        ; COLOR17/18/19; END.
        move.l  #COP_MAIN,a1
        lea     s1_spr_tab,a5
        bsr     build_Vspr8
        lea     s1_bpl_tab,a6
        moveq   #1,d4
        bsr     build_Vbpln
        move.w  #COLOR17,(a1)+
        ifd     S1_COLV
        ; L_S1v (2.8): the vertical-blanking colours are $0FF0,$00FF,$0F0F.
        move.w  #$0FF0,(a1)+
        move.w  #COLOR18,(a1)+
        move.w  #$00FF,(a1)+
        move.w  #COLOR19,(a1)+
        move.w  #$0F0F,(a1)+
        else
        move.w  #$0F00,(a1)+
        move.w  #COLOR18,(a1)+
        move.w  #$00F0,(a1)+
        move.w  #COLOR19,(a1)+
        move.w  #$000F,(a1)+
        endc
        ifd     S1_C
        ; L_S1c (2.8): WAIT VP=$48 (the line after the last line of use 1),
        ; then COLOR17-19 <- $0FF0,$00FF,$0F0F; L_S1cx writes $0FFF,$0F0F,$0FF0.
        move.w  #$4801,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #COLOR17,(a1)+
        ifd     S1_CX
        move.w  #$0FFF,(a1)+
        move.w  #COLOR18,(a1)+
        move.w  #$0F0F,(a1)+
        move.w  #COLOR19,(a1)+
        move.w  #$0FF0,(a1)+
        else
        move.w  #$0FF0,(a1)+
        move.w  #COLOR18,(a1)+
        move.w  #$00FF,(a1)+
        move.w  #COLOR19,(a1)+
        move.w  #$0F0F,(a1)+
        endc
        endc
        move.w  #$FFFF,(a1)+
        move.w  #$FFFE,(a1)+
        ; structure Z (2.9) at CH_SPR0.
        CPY_TABLE CH_SPR0,structz_tbl,structz_tbl_end
        ifd     S1_ZCOPY
        CPY_TABLE CH_SPR0+$100,structz_tbl,structz_tbl_end
        endc
        ; null block at CH_SPR_NULL.
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
        move.w  #(CH_SPR0)>>16,SPR0PTH(a0)
        move.w  #(CH_SPR0)&$FFFF,SPR0PTL(a0)
        move.w  #(CH_SPR_NULL)>>16,SPR1PTH(a0)
        move.w  #(CH_SPR_NULL)&$FFFF,SPR1PTL(a0)
        move.w  #(CH_SPR_NULL)>>16,SPR2PTH(a0)
        move.w  #(CH_SPR_NULL)&$FFFF,SPR2PTL(a0)
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
        ifd     S1_NOSPREN
        S4_DMAEN  BPLEN
        else
        S4_DMAEN  SPREN|BPLEN
        endc
        S5_ARM_INT
        S6_WAIT
        S7_FINISH
        VBLANK_HANDLER

s1_spr_tab:
                ifd     S1_ZCOPY
                dc.l    CH_SPR0+$100
                else
                dc.l    CH_SPR0
                endc
                dc.l    CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL
                dc.l    CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL
s1_bpl_tab:     dc.l    CH_BPL1

; structure Z (SPECIFICATION.md 2.9): four uses, U1 height 8, U2 height 16,
; U3 height 4, U4 height 12; each use's first data line is $FFFF,$0000.
structz_tbl:    dc.w    $4050,$4800
                ifd     S1_Q2
                dc.w    $7FFF,$0000
                else
                ifd     S1_Q2P
                dc.w    $FF00,$0000
                else
                dc.w    $FFFF,$0000
                endc
                endc
                rept    7
                dc.w    $F0F0,$FF00
                endr
                ifd     S1_END
                dc.w    $0000,$0000
                else
                ifd     S1_H
                dc.w    $5064,$5C00
                else
                dc.w    $5064,$6000
                endc
                endc
                dc.w    $FFFF,$0000
                ifd     S1_S
                rept    3
                dc.w    $0F0F,$00FF
                endr
                dc.w    $F0F0,$FF00
                rept    11
                dc.w    $0F0F,$00FF
                endr
                else
                ifd     S1_H
                rept    11
                dc.w    $0F0F,$00FF
                endr
                else
                rept    15
                dc.w    $0F0F,$00FF
                endr
                endc
                endc
                dc.w    $6848,$6C00
                dc.w    $FFFF,$0000
                rept    3
                dc.w    $CCCC,$F0F0
                endr
                dc.w    $7878,$8400
                dc.w    $FFFF,$0000
                rept    11
                dc.w    $AAAA,$CCCC
                endr
                dc.w    $0000,$0000
structz_tbl_end:

        include "ch2build_code.i"
