; p5a_ciab_tb_d2.s -- see p5a_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
DELAYCNT        equ     48000
                include "p5a_body.s"
