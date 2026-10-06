; P8.3, CIAB, population contrast (MUT_SHORT): the sampler runs 399 iterations instead of 400.
; See p_run_body.s.
MUT_SHORT       equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
ICRMASK         equ     ICR_TB
                include "p_run_body.s"
