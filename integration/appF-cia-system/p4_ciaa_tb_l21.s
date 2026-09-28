; p4_ciaa_tb_l21.s -- see p4_body.s.
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAA_PRB
DDRB            equ     CIAA_DDRB
CR              equ     CIAA_CRB
TLO             equ     CIAA_TBLO
THI             equ     CIAA_TBHI
ICR             equ     CIAA_ICR
LATCHVAL        equ     21
CRVAL           equ     $03
                include "p4_body.s"
