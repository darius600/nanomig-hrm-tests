; prog_p3.s -- P3: BPLEN (bit 8) turns bitplane DMA on (SPECIFICATION.md P3).
; This file alone (no symbol set) is the MAIN build (BPLEN set, along with
; DMAEN). prog_p3_c1.s (C1 set) is the contrasting case: BPLEN stays clear.
; prog_p3_g1.s/_g2.s/_g3.s/_g4.s/_g5.s are the precondition mutations.

        include "ch5.i"

        ifnd    DELAY_N
DELAY_N equ     50000
        endif

        org     PROGRAM_ORG

OM      equ     $0020
CM      equ     $0021

start:
        move.w  #$2700,sr
        CLEAR_OVL
        lea     CUSTOM,a0
        lea     MK,a2
        lea     RSTORE,a3

        move.w  #$07FF,DMACON(a0)
        move.w  #$1200,BPLCON0(a0)     ; one bitplane
        move.w  #0,BPLCON1(a0)
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$2CC1,DIWSTOP(a0)
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        move.w  #0,BPL1MOD(a0)
        move.l  #PBM,BPL1PTH(a0)

        move.w  #OM,(a2)
        ifd     C1
        move.w  #$8200,DMACON(a0)      ; BPLEN stays clear
        else
        ifd     G2_BREAK
        move.w  #$8100,DMACON(a0)      ; BPLEN set, DMAEN (bit 9) left out
        else
        ifd     G5_BREAK
        move.w  #$8320,DMACON(a0)      ; SPREN also set
        else
        move.w  #$8300,DMACON(a0)
        endif
        endif
        endif
        move.w  DMACONR(a0),(a3)+
        ifd     G3_BREAK
        move.w  #$DEAD,PBM+$100 ; G3's mutation: a CPU write into the block under test, inside the window
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
