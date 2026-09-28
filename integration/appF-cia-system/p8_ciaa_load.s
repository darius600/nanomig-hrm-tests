; P8.4, CIAA (CRB/timer B). See p_load_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAA_CRB
TLO             equ     CIAA_TBLO
THI             equ     CIAA_TBHI
ICR             equ     CIAA_ICR
                include "p_load_body.s"
