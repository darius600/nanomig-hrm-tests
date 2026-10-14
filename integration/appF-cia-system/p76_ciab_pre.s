; p76_ciab_pre.s -- P7.6 CIAB's own precondition contrast: CR<-$40 (not
; started) gives no TA and no SP. See p76_ciab.s.
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
        move.b  #$40,CIAB_CRA           ; SPMODE output, NOT started
        move.b  #$A5,CIAB_SDR
        RSAMPLE1 300,P76_ICR,CIAB_ICR
        move.b  #$00,CIAB_CRA

        FINISH
