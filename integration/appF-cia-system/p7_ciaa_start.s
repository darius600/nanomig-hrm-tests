; P7.0, CIAA (CRA/timer A). See p_start_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAA_CRA
TLO             equ     CIAA_TALO
THI             equ     CIAA_TAHI
ICR             equ     CIAA_ICR
ICRMASK         equ     ICR_TA
                include "p_start_body.s"
