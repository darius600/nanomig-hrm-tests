; prog_p1.s -- P1: the CPU writes AUDxLCH/AUDxLCL, and each channel fetches
; its data from the location written (SPECIFICATION.md P1).
;
; This file alone (no symbol set) is the MAIN build: channel x gets
; location A_x. Contrasts (each its own source, including this one with a
; symbol set):
;   prog_p1_c1.s (LOC0..LOC3 = A_x^H)  -- C1, the high word
;   prog_p1_c2.s (LOC0..LOC3 = A_x^L)  -- C2, the low word
;   prog_p1_c3.s (LOC0=A1, LOC1=A0)    -- C3, register to channel
; Precondition mutations (each its own source, including this one):
;   prog_p1_g1.s (G1_BREAK)  -- branch-to-self before END
;   prog_p1_g2.s (G2_BREAK)  -- channel 0's enabling write leaves DMAEN out
;   prog_p1_g4.s (DELAY_N=0) -- the per-channel delay collapses to nothing
;   prog_p1_g5.s (G5_BREAK)  -- SPREN also set before the windows
;   prog_p1_g3.s (G3_BREAK)  -- one extra CPU write into A0 inside W_0
;   prog_p1_g3x.s (G3X_BREAK) -- one extra CPU write into A0 inside W_1

        include "ch5.i"

        ifnd    LOC0
LOC0    equ     A0
        endif
        ifnd    LOC1
LOC1    equ     A1
        endif
        ifnd    LOC2
LOC2    equ     A2
        endif
        ifnd    LOC3
LOC3    equ     A3
        endif
        ifnd    DELAY_N
DELAY_N equ     35000
        endif
        ifnd    GAP_N
GAP_N   equ     35000
        endif

        org     PROGRAM_ORG

O0M     equ     $0010
C0M     equ     $0011
O1M     equ     $0012
C1MK    equ     $0013
O2M     equ     $0014
C2MK    equ     $0015
O3M     equ     $0016
C3MK    equ     $0017
ENDM    equ     $00FF

start:
        move.w  #$2700,sr
        CLEAR_OVL
        lea     CUSTOM,a0
        lea     MK,a2
        lea     RSTORE,a3

        move.w  #$07FF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        ifd     G5_BREAK
        move.w  #$8020,DMACON(a0)      ; G5's mutation: SPREN also set
        endif

        move.l  #LOC0,AUD0LCH(a0)
        move.w  #8,AUD0LEN(a0)
        move.w  #64,AUD0VOL(a0)
        move.w  #200,AUD0PER(a0)

        move.l  #LOC1,AUD1LCH(a0)
        move.w  #8,AUD1LEN(a0)
        move.w  #64,AUD1VOL(a0)
        move.w  #200,AUD1PER(a0)

        move.l  #LOC2,AUD2LCH(a0)
        move.w  #8,AUD2LEN(a0)
        move.w  #64,AUD2VOL(a0)
        move.w  #200,AUD2PER(a0)

        move.l  #LOC3,AUD3LCH(a0)
        move.w  #8,AUD3LEN(a0)
        move.w  #64,AUD3VOL(a0)
        move.w  #200,AUD3PER(a0)

        ; ---- channel 0 ----
        move.w  #O0M,(a2)
        ifd     G2_BREAK
        move.w  #$8001,DMACON(a0)      ; G2's mutation: DMAEN left out
        else
        move.w  #$8201,DMACON(a0)
        endif
        move.w  DMACONR(a0),(a3)+
        ifd     G3_BREAK
        move.w  #$DEAD,($60000).l ; G3's mutation: a CPU write into A_0, inside W_0 (absolute: A0 would name the register)
        endif
        move.l  #DELAY_N,d0
        beq.s   .sk0
.d0:    subq.l  #1,d0
        bne.s   .d0
.sk0:   move.w  #C0M,(a2)
        move.w  #$0001,DMACON(a0)
        move.l  #GAP_N,d0
        beq.s   .sg0
.g0:    subq.l  #1,d0
        bne.s   .g0
.sg0:

        ; ---- channel 1 ----
        move.w  #O1M,(a2)
        move.w  #$8202,DMACON(a0)
        move.w  DMACONR(a0),(a3)+
        ifd     G3X_BREAK
        move.w  #$DEAD,($60000).l ; G3X's mutation: a CPU write into A_0, inside W_1 (absolute: A0 would name the register)
        endif
        move.l  #DELAY_N,d0
        beq.s   .sk1
.d1:    subq.l  #1,d0
        bne.s   .d1
.sk1:   move.w  #C1MK,(a2)
        move.w  #$0002,DMACON(a0)
        move.l  #GAP_N,d0
        beq.s   .sg1
.g1:    subq.l  #1,d0
        bne.s   .g1
.sg1:

        ; ---- channel 2 ----
        move.w  #O2M,(a2)
        move.w  #$8204,DMACON(a0)
        move.w  DMACONR(a0),(a3)+
        move.l  #DELAY_N,d0
        beq.s   .sk2
.d2:    subq.l  #1,d0
        bne.s   .d2
.sk2:   move.w  #C2MK,(a2)
        move.w  #$0004,DMACON(a0)
        move.l  #GAP_N,d0
        beq.s   .sg2
.g2:    subq.l  #1,d0
        bne.s   .g2
.sg2:

        ; ---- channel 3 ----
        move.w  #O3M,(a2)
        move.w  #$8208,DMACON(a0)
        move.w  DMACONR(a0),(a3)+
        move.l  #DELAY_N,d0
        beq.s   .sk3
.d3:    subq.l  #1,d0
        bne.s   .d3
.sk3:   move.w  #C3MK,(a2)
        move.w  #$0008,DMACON(a0)
        move.l  #GAP_N,d0
        beq.s   .sg3
.g3:    subq.l  #1,d0
        bne.s   .g3
.sg3:

        ifd     G1_BREAK
.hang:  bra.s   .hang
        endif
        move.w  #ENDM,(a2)
        DONE
