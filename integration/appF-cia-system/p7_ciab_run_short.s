; P7.3, CIAB, population contrast (MUT_SHORT): the sampler runs 399 iterations instead of 400.
; See p_run_body.s.
MUT_SHORT       equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
ICRMASK         equ     ICR_TA
                include "p_run_body.s"
