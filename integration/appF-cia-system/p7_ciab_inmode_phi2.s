; P7.5, CIAB, contrasting case (STARTVAL=$01). See p75_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
STARTVAL        equ     $01
                include "p75_body.s"
