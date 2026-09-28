; P7.3, CIAB (CRA/timer A). See p_run_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
ICRMASK         equ     ICR_TA
                include "p_run_body.s"
