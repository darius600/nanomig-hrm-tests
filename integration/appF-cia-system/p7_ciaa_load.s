; P7.4, CIAA (CRA/timer A). See p_load_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAA_CRA
TLO             equ     CIAA_TALO
THI             equ     CIAA_TAHI
ICR             equ     CIAA_ICR
                include "p_load_body.s"
