; p5a_ciaa_tb_d1.s -- see p5a_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAA_CRB
TLO             equ     CIAA_TBLO
THI             equ     CIAA_TBHI
ICR             equ     CIAA_ICR
DELAYCNT        equ     5000
                include "p5a_body.s"
