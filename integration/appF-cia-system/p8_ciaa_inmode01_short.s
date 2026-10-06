; P8.65 mode 01, CIAA (CRB/timer B, CNT), with a wait of 200 dbra iterations instead of 2200, so W
; is shorter than the window P8.65 requires. R-KEY is sent by the C++ side. See p75_body.s.
DO_KEY          equ     1
START_BRACKET   equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAA_CRB
TLO             equ     CIAA_TBLO
THI             equ     CIAA_TBHI
ICR             equ     CIAA_ICR
STARTVAL        equ     $21
WAITCNT         equ     200
                include "p75_body.s"
