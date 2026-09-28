; P8.65 mode 01, CIAA (CRB/timer B, CNT). R-KEY is sent by the C++ side.
; See p75_body.s.
DO_KEY          equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAA_CRB
TLO             equ     CIAA_TBLO
THI             equ     CIAA_TBHI
ICR             equ     CIAA_ICR
STARTVAL        equ     $21
                include "p75_body.s"
