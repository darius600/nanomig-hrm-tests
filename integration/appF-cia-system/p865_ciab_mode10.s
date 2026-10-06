; p865_ciab_mode10.s -- P8.65 mode 10, CIAB. See p865_ciaa_mode10.s.
;
; Optional: MUT_MODE00 (contrast: CRB<-$01, mode 00, instead of $41).
;           MUT_SHORTLATCH (Pre8.65-M10's own contrast: R-LATCH_A(2) instead
;           of 30, so flags appear in consecutive reads).
                include "nanomig.i"
                include "ciaregs.i"

P810_ICR        equ     RESULTS+8       ; 300 words
P810_FINAL      equ     P810_ICR+300*2
P810_V          equ     P810_FINAL+2

MK_810_START_A   equ     $0C1002         ; before CRB's start write
MK_810_START_B   equ     $0C1004         ; after it
MK_810_STOP_A    equ     $0C1006         ; before CRB's stop write
MK_810_STOP_B    equ     $0C1008         ; after it

        ifnd    LATCH_A
LATCH_A         equ     30
        endc

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #$00,CIAB_CRA
        RLATCH  CIAB_TALO,CIAB_TAHI,LATCH_A
        move.b  #$00,CIAB_CRB
        RLATCH  CIAB_TBLO,CIAB_TBHI,$FFFF
        moveq   #0,d0
        move.b  CIAB_ICR,d0             ; discard
        move.w  #MARKER_SENTINEL,MK_810_START_A
        ifd     MUT_MODE00
        move.b  #$01,CIAB_CRB           ; contrast: mode 00 (START, phi2)
        else
        move.b  #$41,CIAB_CRB           ; START, INMODE 10 (timer A underflow)
        endc
        move.w  #MARKER_SENTINEL,MK_810_START_B
        move.b  #$01,CIAB_CRA           ; START, phi2, continuous
        RSAMPLE1 300,P810_ICR,CIAB_ICR
        move.b  #$00,CIAB_CRA
        READ_STORE CIAB_ICR,P810_FINAL
        move.w  #MARKER_SENTINEL,MK_810_STOP_A
        move.b  #$00,CIAB_CRB
        move.w  #MARKER_SENTINEL,MK_810_STOP_B
        READ_TIMER CIAB_TBHI,CIAB_TBLO,P810_V

        DONE
