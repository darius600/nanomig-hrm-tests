; P7.5, CIAB: CRA<-$20 (INMODE=CNT, START=0) in place of $21: the timer is never started.
; See p75_body.s.
START_BRACKET   equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
STARTVAL        equ     $20
                include "p75_body.s"
