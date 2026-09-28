; P8.0, CIAB, Pre-underflow contrast (MUT_ST_PRE): R-LATCH($0100) instead of
; $8000, so the precondition's own "no underflow" clause fails. See
; p_start_body.s.
MUT_ST_PRE      equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
ICRMASK         equ     ICR_TB
                include "p_start_body.s"
