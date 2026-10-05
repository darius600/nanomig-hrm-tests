; P7.5, CIAB (CRA/timer A, CNT). CIAB's CNT (POUT) has no driver; no R-KEY.
; See p75_body.s.
START_BRACKET   equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
STARTVAL        equ     $21
                include "p75_body.s"
