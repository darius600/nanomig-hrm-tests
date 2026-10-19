; prog_p10.s -- P10: the Copper writes AUDxLCH and AUDxLCL whatever the
; danger bit, under OCS and ECS (SPECIFICATION.md P10). The Copper list at
; LST is preloaded by the C++ harness (per-run: the audio-register list, or
; the COLOR00 contrast list) -- the same mechanism load_program() itself
; uses; no instruction here writes it. OCS/ECS is a harness Config choice
; (chipset_config), not a symbol here. This file alone (COPCON_VAL defaults
; to $0000, CDANG clear) is one of the two builds; prog_p10_cdang.s
; (COPCON_VAL=$0002) is the other. After each enabling DMACON write the
; program reads DMACONR into R: R[0] after the Copper enable, R[1+x] after
; channel x's enable.
;
; Precondition mutations (each its own source, including this one):
;   prog_p10_g1.s      (G1_BREAK)  -- branch-to-self before END
;   prog_p10_g2k.s     (G2K_BREAK) -- DMAEN (bit 9) left out of the Copper enable
;   prog_p10_g2w.s     (G2W_BREAK) -- DMAEN cleared before, and left out of, channel 0's enable
;   prog_p10_g3.s      (G3_BREAK)  -- one extra CPU write into B_0 inside W_0
;   prog_p10_g3l.s     (G3L_BREAK) -- one extra CPU write into L inside [K_O,K_C]
;   prog_p10_g3x.s     (G3X_BREAK) -- one extra CPU write into B_0 inside W_1
;   prog_p10_g4.s      (DELAY_N=0) -- the per-channel delay collapses to nothing
;   prog_p10_g5.s      (G5_BREAK)  -- SPREN also set before the windows
;   prog_p10_g_nolist.s (G_NOLIST) -- COP1LC points at LST2, an end-of-list pair,
;                                     with COPEN still set: the Copper never reads L

        include "ch5.i"

        ifnd    COPCON_VAL
COPCON_VAL equ  $0000
        endif
        ifnd    DELAY_N
DELAY_N equ     35000
        endif
        ifnd    GAP_N
GAP_N   equ     35000
        endif
        ifnd    KDELAY_N
KDELAY_N equ    50000
        endif

        org     PROGRAM_ORG

KO      equ     $0070
KC      equ     $0071
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
        ifd     G5_BREAK
        move.w  #$8020,DMACON(a0)       ; G5's mutation: SPREN also set
        endif
        move.w  #$00FF,ADKCON(a0)
        move.w  #COPCON_VAL,COPCON(a0)

        move.l  #A0,AUD0LCH(a0)
        move.w  #8,AUD0LEN(a0)
        move.w  #64,AUD0VOL(a0)
        move.w  #200,AUD0PER(a0)
        move.l  #A1,AUD1LCH(a0)
        move.w  #8,AUD1LEN(a0)
        move.w  #64,AUD1VOL(a0)
        move.w  #200,AUD1PER(a0)
        move.l  #A2,AUD2LCH(a0)
        move.w  #8,AUD2LEN(a0)
        move.w  #64,AUD2VOL(a0)
        move.w  #200,AUD2PER(a0)
        move.l  #A3,AUD3LCH(a0)
        move.w  #8,AUD3LEN(a0)
        move.w  #64,AUD3VOL(a0)
        move.w  #200,AUD3PER(a0)

        ifd     G_NOLIST
        move.l  #LST2,COP1LCH(a0)       ; precondition mutation: the Copper is pointed away from L
        else
        move.l  #LST,COP1LCH(a0)
        endif
        moveq   #0,d0
        move.w  d0,COPJMP1(a0)          ; strobe (p.30)

        move.w  #KO,(a2)
        ifd     G2K_BREAK
        move.w  #$8080,DMACON(a0)       ; DMAEN (bit 9) left out (precondition mutation)
        else
        move.w  #$8280,DMACON(a0)
        endif
        move.w  DMACONR(a0),(a3)+
        ifd     G3L_BREAK
        move.w  #$DEAD,($64022).l       ; G3's mutation: a CPU write into L (its last word), inside [K_O,K_C]
        endif

        move.l  #KDELAY_N,d0
        beq.s   .skK
.dK:    subq.l  #1,d0
        bne.s   .dK
.skK:
        ifd     G2W_BREAK
        move.w  #$0280,DMACON(a0)       ; Copper DMA off and DMAEN cleared (precondition mutation)
        else
        move.w  #$0080,DMACON(a0)       ; Copper DMA off; DMAEN stays set
        endif
        move.w  #KC,(a2)

        ; ---- channel 0 ----
        move.w  #O0M,(a2)
        ifd     G2W_BREAK
        move.w  #$8001,DMACON(a0)       ; DMAEN left out (precondition mutation)
        else
        move.w  #$8201,DMACON(a0)
        endif
        move.w  DMACONR(a0),(a3)+
        ifd     G3_BREAK
        move.w  #$DEAD,($70800).l       ; G3's mutation: a CPU write into B_0, inside W_0
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
        move.w  #$DEAD,($70800).l       ; G3X's mutation: a CPU write into B_0, inside W_1
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
