; p4c_ciab_tb_l40_two.s -- see p4c_body.s.
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
M               equ     $80
LATCHVAL        equ     40
CRVAL           equ     $01
XHI             equ     10
XHI2            equ     15
                include "p4c_body.s"
