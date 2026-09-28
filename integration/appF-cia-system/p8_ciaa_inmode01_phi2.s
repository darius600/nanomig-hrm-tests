; P8.65 mode 01, CIAA, contrasting case (STARTVAL=$01). See p75_body.s.
DO_KEY          equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAA_CRB
TLO             equ     CIAA_TBLO
THI             equ     CIAA_TBHI
ICR             equ     CIAA_ICR
STARTVAL        equ     $01
                include "p75_body.s"
