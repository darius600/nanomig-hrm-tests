; p3_ciab_tb_l2.s -- see p3_body.s.
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
LATCHVAL        equ     2
CRVAL           equ     $07
                include "p3_body.s"
