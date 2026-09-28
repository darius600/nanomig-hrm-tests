; P7.4, CIAB (CRA/timer A). See p_load_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
                include "p_load_body.s"
