; P8.65 mode 01, CIAB (CRB/timer B, CNT), with a wait of 200 dbra iterations instead of 2200, so W
; is shorter than the window P8.65 requires. No R-KEY. See p75_body.s.
START_BRACKET   equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
STARTVAL        equ     $21
WAITCNT         equ     200
                include "p75_body.s"
