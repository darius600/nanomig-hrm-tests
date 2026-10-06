; pri_routines.i -- the builders shared by the P1 and P2 programs; included
; after the program body (see pri_common.i).

; ---- mkrows: a2 = destination (advanced), d0.w = rows-1, d1.b = tag id,
; d2.l = fill, d7.b = 1 draws the tag. A row is 40 bytes: byte 0 = $FF (block 0),
; bytes 1-4 = the id's bits (most significant first) as $FF or $00, bytes 5-39
; = fill.
mkrows:
                tst.b   d7
                bne.s   .tag
                moveq   #0,d4
                moveq   #0,d5
                bra.s   .fill
.tag:           move.l  #$FF000000,d4
                btst    #3,d1
                beq.s   .n1
                or.l    #$00FF0000,d4
.n1:            btst    #2,d1
                beq.s   .n2
                or.l    #$0000FF00,d4
.n2:            btst    #1,d1
                beq.s   .n3
                or.l    #$000000FF,d4
.n3:            moveq   #0,d5
                btst    #0,d1
                beq.s   .fill
                move.l  #$FF000000,d5
.fill:          move.l  d2,d6
                and.l   #$00FFFFFF,d6
                or.l    d6,d5
.row:           move.l  d4,(a2)+
                move.l  d5,(a2)+
                move.l  d2,(a2)+
                move.l  d2,(a2)+
                move.l  d2,(a2)+
                move.l  d2,(a2)+
                move.l  d2,(a2)+
                move.l  d2,(a2)+
                move.l  d2,(a2)+
                move.l  d2,(a2)+
                dbra    d0,.row
                rts

; ---- mksprite: a2 = destination (advanced), d0.w = X, d1.w = VSTART,
; d2.w = VSTOP, d3.w = 1 for an odd-numbered sprite (value 01), 0 for an
; even-numbered one (value 11). SPRxPOS = VSTART[7:0]<<8 | X[8:1],
; SPRxCTL = VSTOP[7:0]<<8 | X[0] (every line used is below 256).
mksprite:
                move.w  d1,d4
                lsl.w   #8,d4
                move.w  d0,d5
                lsr.w   #1,d5
                or.w    d5,d4
                move.w  d4,(a2)+
                move.w  d2,d4
                lsl.w   #8,d4
                btst    #0,d0
                beq.s   .xe
                or.w    #1,d4
.xe:            move.w  d4,(a2)+
                move.w  d2,d6
                sub.w   d1,d6
                subq.w  #1,d6
.l:             move.w  #$FFFF,(a2)+
                tst.w   d3
                bne.s   .odd
                move.w  #$FFFF,(a2)+
                bra.s   .nx
.odd:           clr.w   (a2)+
.nx:            dbra    d6,.l
                clr.l   (a2)+
                rts

; ---- mkgrp: eight sprites at X = 170+20*s, VSTART 60, VSTOP d2.w; a3 = base
; (block s at base + s*$200).
mkgrp:
                moveq   #0,d7
.s:             movea.l a3,a2
                move.w  d7,d5
                lsl.w   #8,d5
                add.w   d5,d5
                adda.w  d5,a2
                move.w  d7,d0
                mulu.w  #20,d0
                add.w   #170,d0
                moveq   #60,d1
                move.w  d7,d3
                and.w   #1,d3
                move.w  d2,-(sp)
                bsr     mksprite
                move.w  (sp)+,d2
                addq.w  #1,d7
                cmp.w   #8,d7
                bne.s   .s
                rts

; ---- mklist: a1 = table entry (advanced by 32), a2 = destination.
; Entry: dc.w BPLCON0,BPLCON2; six BPLnPT longs (0 = not written); the sprite
; base long. The list writes BPLCON0, BPLCON2, each non-zero BPLnPT, all eight
; SPRxPT, then ends with $FFFF,$FFFE.
mklist:
                move.w  #BPLCON0,(a2)+
                move.w  (a1)+,(a2)+
                move.w  #BPLCON2,(a2)+
                move.w  (a1)+,(a2)+
                moveq   #0,d5
                move.w  #BPL1PTH,d6
.pl:            move.l  (a1)+,d4
                beq.s   .skip
                move.w  d6,(a2)+
                swap    d4
                move.w  d4,(a2)+
                addq.w  #2,d6
                move.w  d6,(a2)+
                swap    d4
                move.w  d4,(a2)+
                subq.w  #2,d6
.skip:          addq.w  #4,d6
                addq.w  #1,d5
                cmp.w   #6,d5
                bne.s   .pl
                move.l  (a1)+,d4
                moveq   #7,d5
                move.w  #SPR0PTH,d6
.sp:            move.w  d6,(a2)+
                swap    d4
                move.w  d4,(a2)+
                addq.w  #2,d6
                move.w  d6,(a2)+
                swap    d4
                move.w  d4,(a2)+
                addq.w  #2,d6
                add.l   #$200,d4
                dbra    d5,.sp
                move.l  #$FFFFFFFE,(a2)+
                rts

; ---- palette: 32 words, COLOR00..COLOR31, SPECIFICATION.md "Palette".
set_palette:
                lea     palette(pc),a2
                lea     COLOR00(a0),a1
                moveq   #31,d0
.c:             move.w  (a2)+,(a1)+
                dbra    d0,.c
                rts

palette:        dc.w    $000,$F00,$A00,$222,$500,$222,$222,$222     ; 00-07
                dc.w    $333,$0F0,$0A0,$222,$050,$222,$222,$222     ; 08-15
                dc.w    $444,$F0F,$F0F,$FF0,$444,$F80,$F80,$0FF     ; 16-23
                dc.w    $444,$8F0,$8F0,$08F,$444,$888,$888,$FFF     ; 24-31

; ---- wait_vertb: waits for INTREQR bit 5, clears it, stores the field
; counter (d0) at (a4)+ and counts it.
wait_vertb:
.w:             move.w  INTREQR(a0),d7
                btst    #5,d7
                beq.s   .w
                move.w  #$0020,INTREQ(a0)
                move.w  d0,(a4)+
                addq.w  #1,d0
                rts

