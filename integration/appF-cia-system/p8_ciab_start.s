; P8.0, CIAB (CRB/timer B). See p_start_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
ICRMASK         equ     ICR_TB
                include "p_start_body.s"
