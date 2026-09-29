; prog_p5.s -- P5: BLTEN (bit 6) turns blitter DMA on (SPECIFICATION.md P5).
; Step 1: the CPU clears [Q, Q + $200) (SPECIFICATION.md P5 step 1). This
; file alone is the MAIN build. prog_p5_c1.s (C1 set) is the contrasting
; case (BLTEN stays clear). prog_p5_g1/_g2/_g3/_g4/_g5.s are the
; precondition mutations.

        include "ch5.i"

        ifnd    DELAY_N
DELAY_N equ     50000
        endif

        org     PROGRAM_ORG

OM      equ     $0040
CM      equ     $0041

start:
        move.w  #$2700,sr
        CLEAR_OVL
        lea     CUSTOM,a0
        lea     MK,a2
        lea     RSTORE,a3

        move.w  #$07FF,DMACON(a0)

        ; ---- step 1: the CPU clears [Q, Q + $200) ----------------------
        lea     QQ,a1
        move.w  #255,d1
.qclr:  move.w  #0,(a1)+
        dbra    d1,.qclr

        move.w  #$01FF,BLTCON0(a0)
        move.w  #0,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTDMOD(a0)
        move.l  #QQ,BLTDPTH(a0)

        move.w  #OM,(a2)
        ifd     C1
        move.w  #$8200,DMACON(a0)       ; BLTEN stays clear
        else
        ifd     G2_BREAK
        move.w  #$8000,DMACON(a0)       ; DMAEN left out
        else
        ifd     G5_BREAK
        move.w  #$8250,DMACON(a0)       ; SPREN also set
        else
        move.w  #$8240,DMACON(a0)
        endif
        endif
        endif
        move.w  DMACONR(a0),(a3)+
        ifd     G3_BREAK
        move.w  #$DEAD,QQ+$1000 ; G3's mutation: a CPU write into the block under test, inside the window
        endif

        move.w  #$0410,BLTSIZE(a0)      ; height=16,width=16 -- starts the blit

        move.l  #DELAY_N,d0
        beq.s   .sk
.d:     subq.l  #1,d0
        bne.s   .d
.sk:    move.w  #CM,(a2)

        ifd     G1_BREAK
.hang:  bra.s   .hang
        endif
        DONE
