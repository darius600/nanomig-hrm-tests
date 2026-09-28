; P7.0, CIAB, Pre-underflow contrast (MUT_ST_PRE): R-LATCH($0100) instead of
; $8000, so the precondition's own "no underflow" clause fails. See
; p_start_body.s.
MUT_ST_PRE      equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
ICRMASK         equ     ICR_TA
                include "p_start_body.s"
