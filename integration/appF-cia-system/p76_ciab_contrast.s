; p76_ciab_contrast.s -- P7.6, CIAB, contrasting case: CR<-$01 (bit 6 = 0,
; input) instead of $41. See p76_ciab.s.
                include "nanomig.i"
                include "ciaregs.i"

P76_ICR         equ     RESULTS+8       ; 300 words

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #$00,CIAB_CRA
        RLATCH  CIAB_TALO,CIAB_TAHI,10
        moveq   #0,d0
        move.b  CIAB_ICR,d0             ; discard
        move.b  #$01,CIAB_CRA           ; START, SPMODE input, continuous
        move.b  #$A5,CIAB_SDR
        RSAMPLE1 300,P76_ICR,CIAB_ICR
        move.b  #$00,CIAB_CRA

        FINISH
