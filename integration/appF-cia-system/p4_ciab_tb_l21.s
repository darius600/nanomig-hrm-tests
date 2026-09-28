; p4_ciab_tb_l21.s -- see p4_body.s.
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
LATCHVAL        equ     21
CRVAL           equ     $03
                include "p4_body.s"
