; P7.5, CIAB (CRA/timer A, CNT), with a wait of 200 dbra iterations instead of 2200, so W is
; shorter than the window P7.5 requires. See p75_body.s.
START_BRACKET   equ     1
WAITCNT         equ     200
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
STARTVAL        equ     $21
                include "p75_body.s"
