; p76_ciab.s -- P7.6, CIAB (CRA bit 6, SPMODE: 1 = serial port output).
; SPECIFICATION.md P7.6.
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
        move.b  #$41,CIAB_CRA           ; START, SPMODE output, continuous
        move.b  #$A5,CIAB_SDR
        RSAMPLE1 300,P76_ICR,CIAB_ICR
        move.b  #$00,CIAB_CRA

        DONE
