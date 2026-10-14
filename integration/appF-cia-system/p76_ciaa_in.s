; p76_ciaa_in.s -- p76_ciaa.s with CR<-$01 (SPMODE input) instead of $41: CIA-A's serial
; port is not put into output mode, and no key is sent. See p76_ciaa.s.
                include "nanomig.i"
                include "ciaregs.i"

P76_ICR         equ     RESULTS+8       ; 300 words

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #$00,CIAA_CRA
        RLATCH  CIAA_TALO,CIAA_TAHI,10
        moveq   #0,d0
        move.b  CIAA_ICR,d0             ; discard
        move.b  #$01,CIAA_CRA           ; START, SPMODE input, continuous
        move.b  #$A5,CIAA_SDR
        RSAMPLE1 300,P76_ICR,CIAA_ICR
        move.b  #$00,CIAA_CRA

        FINISH
