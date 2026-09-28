; P8.3, CIAA (CRB/timer B). See p_run_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAA_CRB
TLO             equ     CIAA_TBLO
THI             equ     CIAA_TBHI
ICR             equ     CIAA_ICR
ICRMASK         equ     ICR_TB
                include "p_run_body.s"
