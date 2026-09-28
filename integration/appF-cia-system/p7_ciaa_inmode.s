; P7.5, CIAA (CRA/timer A, CNT). R-KEY is sent by the C++ side. See
; p75_body.s.
DO_KEY          equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAA_CRA
TLO             equ     CIAA_TALO
THI             equ     CIAA_TAHI
ICR             equ     CIAA_ICR
STARTVAL        equ     $21
                include "p75_body.s"
