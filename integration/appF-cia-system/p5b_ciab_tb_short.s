; p5b_ciab_tb_short.s -- see p5b_body.s. The sampler runs 255 iterations instead of 256.
MUT_SHORT        equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
                include "p5b_body.s"
