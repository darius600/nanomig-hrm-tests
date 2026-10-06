; P8.65 mode 01, CIAB: CRB<-$20 (INMODE=01, START=0) in place of $21: the timer is never started.
; See p75_body.s.
START_BRACKET   equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
STARTVAL        equ     $20
                include "p75_body.s"
