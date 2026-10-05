; P7.5, CIAA, contrasting case (STARTVAL=$01, phi2 instead of CNT). See
; p75_body.s.
DO_KEY          equ     1
START_BRACKET   equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAA_CRA
TLO             equ     CIAA_TALO
THI             equ     CIAA_TAHI
ICR             equ     CIAA_ICR
STARTVAL        equ     $01
                include "p75_body.s"
