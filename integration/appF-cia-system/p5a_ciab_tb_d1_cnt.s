; p5a_ciab_tb_d1_cnt.s -- see p5a_body.s.
MUT_CNT          equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
DELAYCNT        equ     5000
                include "p5a_body.s"
