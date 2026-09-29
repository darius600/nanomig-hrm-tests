; prog_p6.s -- P6: SPREN (bit 5) turns sprite DMA on for all eight sprites
; (SPECIFICATION.md P6). S0's content (the real p.109 sprite structure) is
; supplied by the C++ harness's sprite0_real preload option, not here --
; the same mechanism load_program() itself uses; no instruction here
; writes it. This file alone is the MAIN build. prog_p6_c1.s (C1 set) is
; the contrasting case (SPREN stays clear). prog_p6_g1/_g2/_g3/_g4/_g5.s
; are the precondition mutations; G5's mutation (also set BPLEN) also
; stands for the "bitplane DMA is off" precondition, which is the same
; stored-value clause.

        include "ch5.i"

        ifnd    DELAY_N
DELAY_N equ     50000
        endif

        org     PROGRAM_ORG

OM      equ     $0050
CM      equ     $0051

start:
        move.w  #$2700,sr
        CLEAR_OVL
        lea     CUSTOM,a0
        lea     MK,a2
        lea     RSTORE,a3

        move.w  #$07FF,DMACON(a0)
        move.l  #S0,SPR0PTH(a0)
        move.l  #N1,SPR0PTH+4(a0)
        move.l  #N2,SPR0PTH+8(a0)
        move.l  #N3,SPR0PTH+12(a0)
        move.l  #N4,SPR0PTH+16(a0)
        move.l  #N5,SPR0PTH+20(a0)
        move.l  #N6,SPR0PTH+24(a0)
        move.l  #N7,SPR0PTH+28(a0)

        move.w  #OM,(a2)
        ifd     C1
        move.w  #$8200,DMACON(a0)       ; SPREN stays clear
        else
        ifd     G2_BREAK
        move.w  #$8000,DMACON(a0)       ; DMAEN left out
        else
        ifd     G5_BREAK
        move.w  #$8320,DMACON(a0)       ; BPLEN also set
        else
        move.w  #$8220,DMACON(a0)
        endif
        endif
        endif
        move.w  DMACONR(a0),(a3)+
        ifd     G3_BREAK
        move.w  #$DEAD,S0+$40   ; G3's mutation: a CPU write into the block under test, inside the window
        endif

        move.l  #DELAY_N,d0
        beq.s   .sk
.d:     subq.l  #1,d0
        bne.s   .d
.sk:    move.w  #CM,(a2)
        ; sprite DMA stays on to the end (p.109); no DMACON write here.

        ifd     G1_BREAK
.hang:  bra.s   .hang
        endif
        DONE
