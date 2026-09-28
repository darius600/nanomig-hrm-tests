; P7.3, CIAB, Pre7.3's own contrast (MUT_PAD): a loop body padded beyond 20
; E periods. See p_run_body.s.
MUT_PAD         equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
ICRMASK         equ     ICR_TA
                include "p_run_body.s"
