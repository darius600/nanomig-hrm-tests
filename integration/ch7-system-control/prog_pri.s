; prog_pri.s -- P1: display priority, fifteen configurations (A1, A2, B1-B8,
; E0-E4), three fields each. See SPECIFICATION.md, "P1" and "Program
; skeleton". A wrapper file sets zero or more of these symbols, then includes
; this file:
;
;   NTSC_BUILD     -- DIWSTOP $F4C1 instead of $2CC1
;   MUT_AMIRROR    -- A1: slot j holds sprite 7-j
;   MUT_BSWAP      -- B1-B8: the PF1P and PF2P fields exchanged
;   MUT_PRIFLIP    -- B1-B8: PF2PRI inverted
;   MUT_EREV       -- E0-E4: v replaced by 4-v
;   MUT_SCROLL     -- the CPU writes BPLCON1 $0011 at line 215 of every evaluated field (below the last band)
;   MUT_TAGBAD     -- group A: one row whose tag block 2 is half set (it does not decode)
;   MUT_SPRENCLR   -- DMACON without SPREN
;   MUT_NOCOP      -- no COP1LC write for configuration 4
;   MUT_BPL2ZERO   -- B1-B8: BPL2PT at the all-zero image
;   MUT_VSTOP130   -- B1-B8: sprites end at line 130 instead of 140
;   MUT_LATEMARK   -- the marker of each configuration is stored after t1 instead of between t0 and t1

                include "pri_common.i"

NCFG            equ     15
NOCOP_K         equ     4

                org     PROGRAM_ORG

start:
                COMMON_START

                ; ---- images
                lea     IMG_ZERO,a2
                BAND    256,0,0,0
                lea     IMG_A_P1,a2
                BAND    16,0,0,1
                BAND    20,1,0,1
                BAND    20,2,0,1
                BAND    20,3,0,1
                BAND    180,0,0,1
                ifd     MUT_TAGBAD
                move.b  #$F0,IMG_A_P1+2         ; row 0, block 2: four of its eight pixels set
                endif
                lea     IMG_B_P1,a2
                BAND    16,0,0,1
                BAND    20,1,0,1
                BAND    20,2,$FFFFFFFF,1
                BAND    20,3,0,1
                BAND    20,4,$FFFFFFFF,1
                BAND    10,5,0,1
                BAND    20,6,$FFFFFFFF,1
                BAND    20,7,0,1
                BAND    20,8,$FFFFFFFF,1
                BAND    90,0,0,1
                lea     IMG_B_P2,a2
                BAND    16,0,0,0
                BAND    20,0,0,0
                BAND    20,0,0,0
                BAND    20,0,$FFFFFFFF,0
                BAND    20,0,$FFFFFFFF,0
                BAND    10,0,0,0
                BAND    20,0,0,0
                BAND    20,0,$FFFFFFFF,0
                BAND    20,0,$FFFFFFFF,0
                BAND    90,0,0,0
                lea     IMG_E_P1,a2
                BAND    16,0,0,1
                BAND    40,2,$FFFFFFFF,1
                BAND    20,6,$FFFFFFFF,1
                BAND    180,0,0,1

                ; ---- sprites
                lea     SPR_A1,a3
                ifd     MUT_AMIRROR
                lea     slots_a1_mirror(pc),a1
                else
                lea     slots_a1(pc),a1
                endif
                bsr     mkgrp_a
                lea     SPR_A2,a3
                lea     slots_a2(pc),a1
                bsr     mkgrp_a
                lea     SPR_B,a3
                ifd     MUT_VSTOP130
                move.w  #130,d2
                else
                move.w  #140,d2
                endif
                bsr     mkgrp
                lea     SPR_E,a3
                move.w  #100,d2
                bsr     mkgrp

                ; ---- Copper lists
                lea     cfg_table(pc),a1
                lea     LIST_BASE,a3
                moveq   #0,d7
.bl:            movea.l a3,a2
                bsr     mklist
                lea     $100(a3),a3
                addq.w  #1,d7
                cmp.w   #NCFG,d7
                bne.s   .bl

                bsr     set_palette
                SET_WINDOW

                ; ---- step 4: VERTB, then step 5: COP1LC and its strobe before DMA is on
                move.w  #$0020,INTREQ(a0)
.v0:            move.w  INTREQR(a0),d7
                btst    #5,d7
                beq.s   .v0
                move.l  #LIST_BASE,COP1LCH(a0)
                move.w  #0,COPJMP1(a0)
                move.w  #DMACON_RUN,DMACON(a0)

                ; ---- step 6: the schedule
                lea     TICKS,a4
                lea     MARKS,a5
                moveq   #0,d0
                moveq   #0,d6
cfg_loop:
                bsr     wait_vertb              ; t0
                ifd     MUT_NOCOP
                cmp.w   #NOCOP_K,d6
                beq.s   .skipw
                endif
                moveq   #0,d3
                move.w  d6,d3
                lsl.l   #8,d3
                add.l   #LIST_BASE,d3
                move.l  d3,COP1LCH(a0)
.skipw:
                ifnd    MUT_LATEMARK
                move.w  d6,d3
                add.w   #$C000,d3
                move.w  d3,(a5)+                ; marker k
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
                move.w  d6,d3
                add.w   #$C000,d3
                move.w  d3,(a5)+
                endif
                bsr     wait_vertb              ; t2: it ends
                ifd     MUT_SCROLL
                move.w  #$0000,BPLCON1(a0)
                endif
                addq.w  #1,d6
                cmp.w   #NCFG,d6
                bne     cfg_loop

                move.w  #$CFFF,ENDMARK.l
                DONE

; ---- mkgrp_a: group A. a1 = slot of each sprite (bytes), a3 = base. Sprite s
; sits in slot j = (a1)[s] at X = 200+8j; even slots VSTART 60, odd slots 80;
; VSTOP = VSTART+40; the data pattern follows the sprite number.
mkgrp_a:
                moveq   #0,d7
.s:             moveq   #0,d4
                move.b  0(a1,d7.w),d4
                move.w  d4,d0
                lsl.w   #3,d0
                add.w   #200,d0
                moveq   #60,d1
                btst    #0,d4
                beq.s   .ev
                moveq   #80,d1
.ev:            move.w  d1,d2
                add.w   #40,d2
                move.w  d7,d3
                and.w   #1,d3
                movea.l a3,a2
                move.w  d7,d5
                lsl.w   #8,d5
                add.w   d5,d5
                adda.w  d5,a2
                bsr     mksprite
                addq.w  #1,d7
                cmp.w   #8,d7
                bne.s   .s
                rts

slots_a1:       dc.b    0,1,2,3,4,5,6,7
slots_a1_mirror: dc.b   7,6,5,4,3,2,1,0
slots_a2:       dc.b    1,3,5,7,6,4,2,0
                even

; ---- configuration table: BPLCON0, BPLCON2, BPL1PT..BPL6PT, sprite base.
; BCFG pf1p,pf2p,pf2pri: BPLCON2 = PF2PRI<<6 | PF2P<<3 | PF1P (p.210).
BCFG            macro
                ifd     MUT_BSWAP
                dc.w    $2600,(\3<<6)|(\1<<3)|\2
                else
                ifd     MUT_PRIFLIP
                dc.w    $2600,((1-\3)<<6)|(\2<<3)|\1
                else
                dc.w    $2600,(\3<<6)|(\2<<3)|\1
                endif
                endif
                dc.l    IMG_B_P1
                ifd     MUT_BPL2ZERO
                dc.l    IMG_ZERO
                else
                dc.l    IMG_B_P2
                endif
                dc.l    0,0,0,0,SPR_B
                endm

; ECFG v: BPLCON2 = v<<3 | v, one bitplane.
ECFG            macro
                ifd     MUT_EREV
                dc.w    $1200,((4-\1)<<3)|(4-\1)
                else
                dc.w    $1200,(\1<<3)|\1
                endif
                dc.l    IMG_E_P1,0,0,0,0,0,SPR_E
                endm

cfg_table:
                dc.w    $2600,$0024
                dc.l    IMG_A_P1,IMG_ZERO,0,0,0,0,SPR_A1
                dc.w    $2600,$0024
                dc.l    IMG_A_P1,IMG_ZERO,0,0,0,0,SPR_A2
                BCFG    0,4,0
                BCFG    1,0,1
                BCFG    2,1,1
                BCFG    3,2,1
                BCFG    4,3,1
                BCFG    2,2,0
                BCFG    2,2,1
                BCFG    0,2,1
                ECFG    0
                ECFG    1
                ECFG    2
                ECFG    3
                ECFG    4

                include "pri_routines.i"
