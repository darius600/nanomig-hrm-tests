; P8.0, CIAA (CRB/timer B). See p_start_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAA_CRB
TLO             equ     CIAA_TBLO
THI             equ     CIAA_TBHI
ICR             equ     CIAA_ICR
ICRMASK         equ     ICR_TB
                include "p_start_body.s"
