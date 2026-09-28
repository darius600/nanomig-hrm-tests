; P8.65 mode 01, CIAB (CRB/timer B, CNT). No R-KEY. See p75_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
STARTVAL        equ     $21
                include "p75_body.s"
