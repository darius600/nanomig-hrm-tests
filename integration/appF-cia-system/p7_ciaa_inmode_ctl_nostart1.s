; P7.5-CIAA-INMODE, CIAA: CRA<-$20 (INMODE=CNT, START=0) in place of $21 as the first interval's start
; write: the timer is not started in the interval with no CNT edge. See p75ctl_body.s.
C1_STARTVAL     equ     $20
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAA_CRA
TLO             equ     CIAA_TALO
THI             equ     CIAA_TAHI
ICR             equ     CIAA_ICR
                include "p75ctl_body.s"
