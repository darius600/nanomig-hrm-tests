; p5b_ciab_tb_nostart.s -- see p5b_body.s. The start write is CR<-$00: the timer never runs.
MUT_NOSTART      equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
                include "p5b_body.s"
