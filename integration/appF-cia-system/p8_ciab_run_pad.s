; P8.3, CIAB, Pre8.3's own contrast (MUT_PAD): a loop body padded beyond 20
; E periods. See p_run_body.s.
MUT_PAD         equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
ICRMASK         equ     ICR_TB
                include "p_run_body.s"
