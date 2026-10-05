; prog_p4.s -- P4: COPEN (bit 7) turns Copper DMA on (SPECIFICATION.md P4).
; The Copper list at LST ($0180,$0F00 then $FFFF,$FFFE) is preloaded by
; the C++ harness before boot -- the same mechanism load_program() itself
; uses; no instruction here writes it. This file alone is the MAIN build.
; prog_p4_c1.s (C1 set) is the contrasting case (COPEN stays clear).
; prog_p4_g1/_g2/_g3/_g4/_g5.s are the precondition mutations.

        include "ch5.i"

        ifnd    DELAY_N
DELAY_N equ     50000
        endif

        org     PROGRAM_ORG

OM      equ     $0030
CM      equ     $0031

start:
        move.w  #$2700,sr
        CLEAR_OVL
        lea     CUSTOM,a0
        lea     MK,a2
        lea     RSTORE,a3

        move.w  #$07FF,DMACON(a0)
        move.l  #LST,COP1LCH(a0)
        moveq   #0,d0
        move.w  d0,COPJMP1(a0)          ; strobe (p.30)

        move.w  #OM,(a2)
        ifd     C1
        move.w  #$8200,DMACON(a0)       ; COPEN stays clear
        else
        ifd     G2_BREAK
        move.w  #$8080,DMACON(a0)       ; COPEN set, DMAEN (bit 9) left out
        else
        ifd     G5_BREAK
        move.w  #$82A0,DMACON(a0)       ; SPREN also set
        else
        move.w  #$8280,DMACON(a0)
        endif
        endif
        endif
        move.w  DMACONR(a0),(a3)+
        ifd     G3_BREAK
        move.w  #$DEAD,LST+$10  ; G3's mutation: a CPU write into the block under test, inside the window
        endif

        move.l  #DELAY_N,d0
        beq.s   .sk
.d:     subq.l  #1,d0
        bne.s   .d
.sk:    move.w  #CM,(a2)
        move.w  #$07FF,DMACON(a0)

        ifd     G1_BREAK
.hang:  bra.s   .hang
        endif
        DONE
