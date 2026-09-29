; P8.65-MODE01-CIAB-GREENCTL: CRB<-$20 (INMODE=CNT, START=0) in place of
; $21. See p75_body.s. A proven-live input through the same "v=$1000"
; comparator P8.65 mode 01's own CIAB leg uses.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
STARTVAL        equ     $20
                include "p75_body.s"
