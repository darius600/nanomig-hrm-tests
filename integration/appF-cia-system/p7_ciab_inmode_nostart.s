; P7.5-CIAB-GREENCTL: CRA<-$20 (INMODE=CNT, START=0) in place of $21. See
; p75_body.s. A proven-live input through the same "v=$1000" comparator
; P7.5's own CIAB leg uses.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
STARTVAL        equ     $20
                include "p75_body.s"
