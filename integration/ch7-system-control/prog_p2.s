; prog_p2.s -- P2: six lores planes (dual playfield) and eight sprites in one
; row. See SPECIFICATION.md, "P2". A wrapper file sets zero or more of these
; symbols, then includes this file:
;
;   NTSC_BUILD     -- DIWSTOP $F4C1 instead of $2CC1
;   MUT_SPRENCLR   -- DMACON without SPREN
;   MUT_SCROLL     -- the CPU writes BPLCON1 $0011 at line 215 of the evaluated field (below the last band)
;   MUT_TAGID      -- the band's tag id is 2 instead of 1
;   MUT_NOCOP      -- the Copper starts from a second copy of the list, at another address, and no COP1LC write follows
;   MUT_BPL4PLANE2 -- BPL4PT at plane 2's image
;   MUT_BPL4ZERO   -- BPL4PT at the all-zero image of plane 4's own area
;   MUT_LATEMARK   -- the marker is stored after t1 instead of between t0 and t1

                include "pri_common.i"

NCFG            equ     1

                org     PROGRAM_ORG

start:
                COMMON_START

                ; ---- images: plane n (n = 0..5) at P2_IMG + n*$6000, its all-zero image at +$3000.
                ; Plane 1 carries the tags; plane n shows its block at x = 272 + 8n in the band rows.
                suba.l  a6,a6                   ; plane counter (BAND uses d7)
.pl:            move.l  a6,d5
                mulu.w  #$6000,d5
                lea     P2_IMG,a2
                adda.l  d5,a2
                movea.l a2,a3                   ; image base
                move.l  a6,d5
                tst.w   d5
                bne.s   .plain
                BAND    16,0,0,1
                ifd     MUT_TAGID
                BAND    20,2,0,1
                else
                BAND    20,1,0,1
                endif
                BAND    220,0,0,1
                bra.s   .patch
.plain:         BAND    256,0,0,0
.patch:         lea     16*40+34(a3),a2         ; the band rows (lines 60-79), byte 34 + n
                move.l  a6,d5
                adda.w  d5,a2
                moveq   #19,d0
.p:             move.b  #$FF,(a2)
                lea     40(a2),a2
                dbra    d0,.p
                lea     $3000(a3),a2            ; the all-zero image
                BAND    256,0,0,0
                lea     1(a6),a6
                move.l  a6,d5
                cmp.w   #6,d5
                bne     .pl

                ; ---- sprites: X = 170+20s, lines 60-79
                lea     SPR_P2,a3
                move.w  #80,d2
                bsr     mkgrp

                ; ---- Copper lists: 0 = the configuration, 1 = a copy of it at another address
                lea     cfg_table(pc),a1
                lea     LIST_BASE,a3
                moveq   #0,d7
.bl:            movea.l a3,a2
                bsr     mklist
                lea     $100(a3),a3
                addq.w  #1,d7
                cmp.w   #2,d7
                bne.s   .bl

                bsr     set_palette
                SET_WINDOW

                move.w  #$0020,INTREQ(a0)
.v0:            move.w  INTREQR(a0),d7
                btst    #5,d7
                beq.s   .v0
                ifd     MUT_NOCOP
                move.l  #LIST_BASE+$100,COP1LCH(a0)
                else
                move.l  #LIST_BASE,COP1LCH(a0)
                endif
                move.w  #0,COPJMP1(a0)
                move.w  #DMACON_RUN,DMACON(a0)

                lea     TICKS,a4
                lea     MARKS,a5
                moveq   #0,d0
                bsr     wait_vertb              ; t0
                ifnd    MUT_NOCOP
                move.l  #LIST_BASE,COP1LCH(a0)
                endif
                ifnd    MUT_LATEMARK
                move.w  #$C000,(a5)+            ; marker 0
                endif
                bsr     wait_vertb              ; t1: the evaluated field begins
                ifd     MUT_SCROLL
.ws:            move.w  VHPOSR(a0),d7
                lsr.w   #8,d7
                cmp.w   #215,d7
                blo.s   .ws
                move.w  #$0011,BPLCON1(a0)
                endif
                ifd     MUT_LATEMARK
                move.w  #$C000,(a5)+
                endif
                bsr     wait_vertb              ; t2: it ends
                ifd     MUT_SCROLL
                move.w  #$0000,BPLCON1(a0)
                endif
                move.w  #$CFFF,ENDMARK.l
                DONE

PLANE           macro                           ; \1 = plane index 0..5: the image address
                dc.l    P2_IMG+\1*$6000
                endm

; one table entry: BPLCON0, BPLCON2, six BPLnPT, the sprite base. Entry 1 repeats
; entry 0 at another list address: the stale list of MUT_NOCOP.
CFGENTRY        macro
                dc.w    $6600,$0000
                PLANE   0
                PLANE   1
                PLANE   2
                ifd     MUT_BPL4PLANE2
                dc.l    P2_IMG+1*$6000
                else
                ifd     MUT_BPL4ZERO
                dc.l    P2_IMG+3*$6000+$3000
                else
                PLANE   3
                endif
                endif
                PLANE   4
                PLANE   5
                dc.l    SPR_P2
                endm

cfg_table:
                CFGENTRY
                CFGENTRY

                include "pri_routines.i"
