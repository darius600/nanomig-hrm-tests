; P8.65 mode 01, CIAB, contrasting case (STARTVAL=$01). See p75_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
STARTVAL        equ     $01
                include "p75_body.s"
